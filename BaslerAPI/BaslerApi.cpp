#include "BaslerApi.h"
#include <QThread>
#include <QDebug>
#include <pylon/ImageEventHandler.h>

BaslerApi::BaslerApi(bool isMaster, const BaslerCameraParams& params, const QString &serialNumber, QObject *parent)
    : QObject(parent)
    , m_isGrabbing(false)
    , m_isConnected(false)
    , m_isMaster(isMaster)
    , m_params(params)
    , m_camera(nullptr)
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
    if (!m_isConnected) return;

    // Принудительно устанавливаем базовые настройки для гарантированной работы
    if (m_camera->TriggerMode.IsWritable()) {
        m_camera->TriggerMode.SetValue(TriggerMode_Off);
        qDebug() << "TriggerMode set to Off";
    }
    if (GenApi::IsAvailable(m_camera->AcquisitionMode)) {
        m_camera->AcquisitionMode.SetValue(AcquisitionMode_Continuous);
        qDebug() << "AcquisitionMode set to Continuous";
    }

//    configureMasterSlave();
    setupCameraFeatures();

    m_camera->StartGrabbing();
    m_isGrabbing = true;

    while (m_isGrabbing && m_camera->IsGrabbing()) {
        try {
            // Ждём кадр до 5 секунд
            m_camera->RetrieveResult(5000, m_ptrGrabResult, TimeoutHandling_ThrowException);

            if (m_ptrGrabResult.IsValid() && m_ptrGrabResult->GrabSucceeded()) {
                QByteArray rawData((const char*)m_ptrGrabResult->GetBuffer(),
                                   m_ptrGrabResult->GetImageSize());
                emit rawDataReceived(rawData, m_ptrGrabResult->GetWidth(),
                                     m_ptrGrabResult->GetHeight(),
                                     m_ptrGrabResult->GetPixelType());
            } else {
                QString err = m_ptrGrabResult.IsValid()
                    ? QString("Grab failed: %1").arg(QString::fromStdString(m_ptrGrabResult->GetErrorDescription().c_str()))
                    : "Invalid grab result";
                emit sendErrorMessage(err);
            }
        } catch (const GenericException& e) {
            emit sendErrorMessage(QString("Pylon Exception: %1").arg(e.GetDescription()));
            // Если ошибка критическая, можно выйти из цикла
            if (!m_isGrabbing) break;
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

    } catch (const GenericException& e) {
        emit sendErrorMessage(QString("Setup error: %1").arg(e.GetDescription()));
    }
}

void BaslerApi::configureMasterSlave()
{
    if (!m_camera || !m_camera->IsOpen()) return;

    try {
        if (m_isMaster) {
            // --- Настройка мастера ---
            // Отключаем внешний триггер (работа по внутреннему таймеру)
            m_camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
            m_camera->TriggerMode.SetValue(TriggerMode_Off);

            // Включаем AcquisitionFrameRate и устанавливаем желаемое значение
            if (GenApi::IsAvailable(m_camera->AcquisitionFrameRateEnable)) {
                m_camera->AcquisitionFrameRateEnable.SetValue(true);
            }
            if (GenApi::IsAvailable(m_camera->AcquisitionFrameRate)) {
                // Желаемую частоту можно передавать через m_params (добавить поле)
                m_camera->AcquisitionFrameRate.SetValue(m_params.acquisitionFrameRate);
            }

            // Настройка линии выхода (используем Line3, как в документе)
            if (GenApi::IsAvailable(m_camera->LineSelector)) {
                m_camera->LineSelector.SetValue(LineSelector_Line3);
            }
            if (GenApi::IsAvailable(m_camera->LineMode)) {
                m_camera->LineMode.SetValue(LineMode_Output);
            }
            if (GenApi::IsAvailable(m_camera->LineSource)) {
                // Сигнал "ожидание триггера кадра" – именно он нужен для синхронизации
                m_camera->LineSource.SetValue(LineSource_FrameTriggerWait);
            }
        } else {
            // --- Настройка слейва ---
            // Включаем внешний триггер на Line4
            m_camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
            m_camera->TriggerMode.SetValue(TriggerMode_On);
            m_camera->TriggerSource.SetValue(TriggerSource_Line4);
            m_camera->TriggerActivation.SetValue(TriggerActivation_RisingEdge);

            // Выключаем AcquisitionFrameRate (слейв следует за мастером)
            if (GenApi::IsAvailable(m_camera->AcquisitionFrameRateEnable)) {
                m_camera->AcquisitionFrameRateEnable.SetValue(false);
            }

            // Задержка триггера (опционально, можно оставить 0)
            if (GenApi::IsAvailable(m_camera->TriggerDelayAbs)) {
                m_camera->TriggerDelayAbs.SetValue(0.0);
            }
        }
    } catch (const GenericException& e) {
        emit sendErrorMessage(QString("MasterSlave config error: %1").arg(e.GetDescription()));
    }
}
