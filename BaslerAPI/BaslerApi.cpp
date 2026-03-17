#include "BaslerApi.h"
#include <QThread>
#include <QDebug>

BaslerApi::BaslerApi(bool isMaster, const BaslerCameraParams& params, const QString &serialNumber, QObject *parent)
    : QObject(parent)
    , m_isGrabbing(false)
    , m_isConnected(false)
    , m_isMaster(isMaster)
    , m_params(params)
    , m_camera(nullptr)
    , m_frameCount(0)
    , m_serialNumber(serialNumber)
{

}

BaslerApi::~BaslerApi()
{
    stopGrabbing();
    if (m_camera) {
        m_camera->Close();
        delete m_camera;
        m_camera = nullptr;
    }
    PylonTerminate();
}

void BaslerApi::run()
{
    m_isConnected = initializeCamera();

    emit connectionComplete(m_isConnected);

    if (!m_isConnected) {
        return;
    }

    setupCameraFeatures();
    configureMasterSlave();

    // Запускаем захват
    startGrabbing(); // m_isGrabbing = true

    m_fpsTimer.start();
    m_frameCount = 0;

    // Для мастера: непрерывный захват (GrabStrategy_OneByOne)
    // Для слейва: тоже непрерывный, но он будет ждать триггера
    if (m_isMaster) {
        m_camera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
    } else {
        m_camera->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
    }

    while (m_isGrabbing) {
        try {
            // Для слейва имеет смысл проверить готовность триггера, но StartGrabbing уже запущен,
            // и RetrieveResult будет ждать кадр. Для мастера просто ждём кадр.
            if (m_camera->GetGrabResultWaitObject().Wait(1000)) {
                m_camera->RetrieveResult(5000, m_ptrGrabResult, TimeoutHandling_ThrowException);

                if (m_ptrGrabResult->GrabSucceeded()) {
                    // Конвертация и отправка сигналов (как раньше)
                    if (m_converter.IsSupportedOutputFormat(PixelType_BGRA8packed) &&
                            m_converter.IsSupportedInputFormat(m_ptrGrabResult->GetPixelType())) {
                        m_converter.Convert(m_pylonImage, m_ptrGrabResult);
                        QImage image((const uchar*)m_pylonImage.GetBuffer(),
                                     m_pylonImage.GetWidth(),
                                     m_pylonImage.GetHeight(),
                                     QImage::Format_RGB32);
                        emit imageReceived(image.copy());
                    }

                    QByteArray rawData((const char*)m_ptrGrabResult->GetBuffer(),
                                       m_ptrGrabResult->GetImageSize());
                    emit rawDataReceived(rawData, m_ptrGrabResult->GetWidth(),
                                         m_ptrGrabResult->GetHeight());

                    m_frameCount++;
                    qint64 elapsed = m_fpsTimer.elapsed();
                    if (elapsed >= 2000) {
                        double fps = m_frameCount / (elapsed / 1000.0);
                        emit cameraStats(fps, 0.0);
                        m_frameCount = 0;
                        m_fpsTimer.restart();
                    }
                } else {
                    emit sendErrorMessage(QString("Grab failed: %1").arg(QString::fromStdString(m_ptrGrabResult->GetErrorDescription().c_str())));
                }
            } else {
                // Таймаут ожидания кадра – для мастера это может означать проблемы,
                // для слейва – отсутствие триггера.
                // Не спамим ошибками, просто продолжаем цикл.
                QThread::msleep(1);
            }
        } catch (const GenericException& e) {
            emit sendErrorMessage(QString("Pylon Exception: %1").arg(e.GetDescription()));
            // Возможно, стоит выйти из цикла, если ошибка критическая
            // m_isGrabbing = false;
        }
    }

    m_camera->StopGrabbing();
    m_camera->Close();
}

void BaslerApi::startGrabbing()
{
    m_isGrabbing = true;
}

void BaslerApi::stopGrabbing()
{
    m_isGrabbing = false;
}

bool BaslerApi::initializeCamera()
{
    try {
        PylonInitialize();
        CTlFactory& tlFactory = CTlFactory::GetInstance();
        DeviceInfoList_t devices;
        tlFactory.EnumerateDevices(devices);
        if (tlFactory.EnumerateDevices(devices) == 0) {
            emit sendErrorMessage("No Basler devices found.");
            return false;
        }

        // Ищем устройство с нужным серийным номером
        bool found = false;
        for (size_t i = 0; i < devices.size(); ++i) {
            QString serial = devices[i].GetSerialNumber().c_str();
            if (serial == m_serialNumber) {
                m_camera = new CBaslerUniversalInstantCamera(tlFactory.CreateDevice(devices[i]));
                found = true;
                break;
            }
        }

        if (!found) {
            emit sendErrorMessage(QString("Camera with serial %1 not found.").arg(m_serialNumber));
            return false;
        }

        m_camera->RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

        m_camera->Open();
        return true;
    } catch (const GenericException& e) {
        emit sendErrorMessage(QString("Init error: %1").arg(e.GetDescription()));
        return false;
    }
}

void BaslerApi::setupCameraFeatures()
{
    if (!m_camera || !m_camera->IsOpen()) return;

    try {
        // Отключаем автонастройки, если они есть
        if (m_camera->ExposureAuto.IsWritable())
            m_camera->ExposureAuto.SetValue(ExposureAuto_Off);
        if (m_camera->GainAuto.IsWritable())
            m_camera->GainAuto.SetValue(GainAuto_Off);
        if (m_camera->BalanceWhiteAuto.IsWritable())
            m_camera->BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);

        // Устанавливаем размеры, если поддерживаются
        if (m_camera->Width.IsWritable())
            m_camera->Width.SetValue(m_params.width);
        if (m_camera->Height.IsWritable())
            m_camera->Height.SetValue(m_params.height);

        // Экспозиция
        if (m_camera->ExposureTimeAbs.IsWritable())
            m_camera->ExposureTimeAbs.SetValue(m_params.exposureTimeAbs);
        else if (m_camera->ExposureTime.IsWritable())
            m_camera->ExposureTime.SetValue(m_params.exposureTimeAbs); // для некоторых моделей

        // Gain
        if (m_camera->GainRaw.IsWritable())
            m_camera->GainRaw.SetValue(m_params.gainRaw);
        else if (m_camera->Gain.IsWritable())
            m_camera->Gain.SetValue(m_params.gainRaw);

        // Пиксельный формат
        if (m_camera->PixelFormat.IsWritable()) {
            // Для монохромных камер лучше Mono8 или Mono12
            m_camera->PixelFormat.SetValue(PixelFormat_Mono8);
        }

        // Конвертер для QImage
        m_converter.OutputPixelFormat = PixelType_BGRA8packed;

    } catch (const GenericException& e) {
        emit sendErrorMessage(QString("Setup error: %1").arg(e.GetDescription()));
    }
}

void BaslerApi::configureMasterSlave()
{
    if (!m_camera || !m_camera->IsOpen()) return;

    try {
        if (m_isMaster) {
            // Мастер: работает по внутреннему таймеру, триггер отключён
            m_camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
            m_camera->TriggerMode.SetValue(TriggerMode_Off);

            // Настройка линии синхронизации как выхода
            if (m_camera->LineSelector.IsWritable() && m_camera->LineMode.IsWritable()) {
                m_camera->LineSelector.SetValue(LineSelector_Line2); // Выберите нужную линию
                m_camera->LineMode.SetValue(LineMode_Output);
                if (m_camera->LineSource.IsWritable()) {
                    m_camera->LineSource.SetValue(LineSource_FrameTriggerWait);
                }
            }
        } else {
            // Слейв: ждёт внешний триггер на Line1
            m_camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
            m_camera->TriggerMode.SetValue(TriggerMode_On);
            m_camera->TriggerSource.SetValue(TriggerSource_Line1);
            m_camera->TriggerActivation.SetValue(TriggerActivation_RisingEdge);
            if (m_camera->TriggerDelayAbs.IsWritable()) {
                m_camera->TriggerDelayAbs.SetValue(0.0);
            }
        }
    } catch (const GenericException& e) {
        emit sendErrorMessage(QString("MasterSlave config error: %1").arg(e.GetDescription()));
    }
}

