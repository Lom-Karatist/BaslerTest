#include "BaslerApi.h"

#include <pylon/ImageEventHandler.h>

#include <QApplication>
#include <QDebug>
#include <QThread>

BaslerApi::BaslerApi(bool isMaster, const BaslerCameraParams& params,
                     QObject* parent, bool isMasterSlaveNeeded)
    : QObject(parent),
      m_isActive(true),
      m_isGrabbing(false),
      m_isConnected(false),
      m_isMaster(isMaster),
      m_isMasterSlaveNeeded(isMasterSlaveNeeded),
      m_params(params),
      m_camera(nullptr) {}

BaslerApi::~BaslerApi() {
    //    qDebug() << "BaslerApi destructor, m_camera =" << m_camera;
    stopGrabbing();
    if (m_camera) {
        //        qDebug() << "Closing camera";
        if (m_camera->IsOpen()) {
            m_camera->Close();
        }
        delete m_camera;
        m_camera = nullptr;
        qDebug() << "Basler camera object was destryed";
    }
    //    qDebug() << "BaslerApi destructor finished";
}

void BaslerApi::run() {
    m_isConnected = initializeCamera();
    emit connectionComplete(m_isConnected);
    if (!m_isConnected) {
        m_isActive = false;
        return;
    }

    if (m_isMasterSlaveNeeded) {
        configureMasterSlave();
    } else {
        if (m_camera->TriggerMode.IsWritable()) {
            m_camera->TriggerMode.SetValue(TriggerMode_Off);
            qDebug() << "TriggerMode set to Off";
        }
    }
    setupCameraFeatures();
    //    m_camera->StartGrabbing();

    while (m_isActive.load()) {
        if (m_commandsPending.load()) {
            applyPendingCommands();
            continue;
        }
        if (!m_isGrabbing.load()) QApplication::processEvents();
        if (m_isGrabbing.load()) {
            if (!m_camera->IsGrabbing()) {
                startGrabbing();
            } else {
                try {
                    m_camera->RetrieveResult(5000, m_ptrGrabResult,
                                             TimeoutHandling_ThrowException);
                    if (m_ptrGrabResult.IsValid() &&
                        m_ptrGrabResult->GrabSucceeded()) {
                        processRawData();
                    } else {
                        if (m_isGrabbing.load()) {
                            QString err =
                                m_ptrGrabResult.IsValid()
                                    ? QString("Grab failed: %1")
                                          .arg(QString::fromStdString(
                                              m_ptrGrabResult
                                                  ->GetErrorDescription()
                                                  .c_str()))
                                    : "Invalid grab result";
                            emit sendErrorMessage(err);
                        }
                    }
                } catch (const GenericException& e) {
                    emit sendErrorMessage(
                        QString("Pylon Exception: %1").arg(e.GetDescription()));
                } catch (const std::exception& e) {
                    emit sendErrorMessage(
                        QString("Standard exception: %1").arg(e.what()));
                } catch (...) {
                    emit sendErrorMessage(
                        "Unknown exception in BaslerApi::run");
                }
            }
        }
    }

    if (m_camera && m_camera->IsGrabbing()) {
        m_camera->StopGrabbing();
    }
    if (m_camera && m_camera->IsOpen()) {
        m_camera->Close();
    }
}

void BaslerApi::startGrabbing() {
    if (!m_isGrabbing.exchange(true)) {
        if (m_camera && !m_camera->IsGrabbing() && m_camera->IsOpen()) {
            m_camera->StartGrabbing();
        }
    }
}

void BaslerApi::pauseGrabbing() {
    if (m_isGrabbing.exchange(false)) {
        if (m_camera && m_camera->IsGrabbing()) {
            m_camera->StopGrabbing();
        }
    }
}

void BaslerApi::stopGrabbing() {
    pauseGrabbing();
    m_isActive = false;
}

void BaslerApi::applyGainChanging(double value) {
    if (m_camera->GainRaw.IsWritable())
        m_camera->GainRaw.SetValue(value);
    else if (m_camera->Gain.IsWritable())
        m_camera->Gain.SetValue(value);
}

void BaslerApi::applyPixelFormatChanging(int value) {
    if (m_camera->PixelFormat.IsWritable()) {
        try {
            m_camera->PixelFormat.SetIntValue(
                static_cast<PixelFormatEnums>(value));
        } catch (const GenericException& e) {
            qDebug() << "Pylon exception:" << e.GetDescription();
            emit sendErrorMessage(QString("Failed to set pixel format: %1")
                                      .arg(e.GetDescription()));
        } catch (const std::exception& e) {
            qDebug() << "std exception:" << e.what();
        } catch (...) {
            qDebug() << "Unknown exception in applyPixelFormatChanging";
        }
    } else {
        qDebug() << "PixelFormat not writable";
    }
}

void BaslerApi::applyBinningHorizontalModeChanging(
    BinningHorizontalModeEnums mode) {
    if (m_camera->BinningHorizontalMode.IsWritable())
        m_camera->BinningHorizontalMode.SetValue(mode);
}

void BaslerApi::applyBinningVerticalModeChanging(
    BinningVerticalModeEnums mode) {
    if (m_camera->BinningVerticalMode.IsWritable())
        m_camera->BinningVerticalMode.SetValue(mode);
}

void BaslerApi::submitCommands(
    std::vector<std::unique_ptr<ParameterCommand>> commands) {
    QMutexLocker locker(&m_commandsMutex);

    m_pendingCommands = std::move(commands);
    m_commandsPending = true;
}

void BaslerApi::applyWidthChanging(int value) {
    if (m_camera->Width.IsWritable()) m_camera->Width.SetValue(value);
}

void BaslerApi::applyHeightChanging(int value) {
    if (m_camera->Height.IsWritable()) m_camera->Height.SetValue(value);
}

void BaslerApi::applyOffsetXChanging(int value) {
    if (m_camera->OffsetX.IsWritable()) m_camera->OffsetX.SetValue(value);
}

void BaslerApi::applyOffsetYChanging(int value) {
    if (m_camera->OffsetY.IsWritable()) m_camera->OffsetY.SetValue(value);
}

void BaslerApi::applyBinningHorizontalChanging(int value) {
    if (m_camera->BinningHorizontal.IsWritable())
        m_camera->BinningHorizontal.SetValue(value);
}

void BaslerApi::applyBinningVerticalChanging(int value) {
    if (m_camera->BinningVertical.IsWritable())
        m_camera->BinningVertical.SetValue(value);
}

void BaslerApi::applyExposureChanging(double exposureMs) {
    if (m_camera->ExposureTime.IsWritable())
        m_camera->ExposureTime.SetValue(exposureMs * 1000.0);
}

void BaslerApi::applyFramerateChanging(double fps) {
    qDebug() << "setting framerate to " << fps;
    if (GenApi::IsAvailable(m_camera->AcquisitionFrameRate))
        m_camera->AcquisitionFrameRate.SetValue(fps);
    qDebug() << "current framerate:"
             << m_camera->AcquisitionFrameRate.GetValue();
}

bool BaslerApi::initializeCamera() {
    try {
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
            if (serial == m_params.serialNumber) {
                m_camera = new CBaslerUniversalInstantCamera(
                    tlFactory.CreateDevice(devices[i]));
                found = true;
                break;
            }
        }
        if (!found) {
            emit sendErrorMessage(QString("Camera with serial %1 not found.")
                                      .arg(m_params.serialNumber));
            return false;
        }

        m_camera->RegisterConfiguration(new CSoftwareTriggerConfiguration,
                                        RegistrationMode_ReplaceAll,
                                        Cleanup_Delete);

        m_camera->Open();
        return true;
    } catch (const GenericException& e) {
        emit sendErrorMessage(
            QString("Init error: %1").arg(e.GetDescription()));
        return false;
    }
}

void BaslerApi::setupCameraFeatures() {
    if (!m_camera || !m_camera->IsOpen()) return;

    try {
        // Отключаем автонастройки, если они есть
        if (m_camera->ExposureAuto.IsWritable())
            m_camera->ExposureAuto.SetValue(ExposureAuto_Off);
        if (m_camera->GainAuto.IsWritable())
            m_camera->GainAuto.SetValue(GainAuto_Off);
        if (m_camera->BalanceWhiteAuto.IsWritable())
            m_camera->BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);

        applyBinningHorizontalChanging(m_params.binningHorizontal);
        applyBinningVerticalChanging(m_params.binningVertical);
        applyWidthChanging(m_params.width);
        applyHeightChanging(m_params.height);
        applyOffsetXChanging(m_params.offsetX);
        applyOffsetYChanging(m_params.offsetY);
        applyExposureChanging(m_params.exposureTime);
        applyFramerateChanging(m_params.acquisitionFrameRate);

        applyGainChanging(m_params.gain);
        applyPixelFormatChanging(m_params.pixelFormat);

    } catch (const GenericException& e) {
        emit sendErrorMessage(
            QString("Setup error: %1").arg(e.GetDescription()));
    }
}

void BaslerApi::configureMasterSlave() {
    if (!m_camera || !m_camera->IsOpen()) return;

    try {
        if (m_isMaster) {
            m_camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
            m_camera->TriggerMode.SetValue(TriggerMode_Off);

            if (GenApi::IsAvailable(m_camera->LineSelector)) {
                m_camera->LineSelector.SetValue(LineSelector_Line3);
            }
            if (GenApi::IsAvailable(m_camera->LineMode)) {
                m_camera->LineMode.SetValue(LineMode_Output);
            }
            if (GenApi::IsAvailable(m_camera->LineSource)) {
                m_camera->LineSource.SetValue(LineSource_ExposureActive);
            }
            if (GenApi::IsAvailable(m_camera->LineInverter)) {
                m_camera->LineInverter.SetValue(true);
            }
            if (GenApi::IsAvailable(m_camera->AcquisitionFrameRateEnable)) {
                m_camera->AcquisitionFrameRateEnable.SetValue(true);
            }
        } else {
            m_camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
            m_camera->TriggerMode.SetValue(TriggerMode_On);
            m_camera->TriggerSource.SetValue(TriggerSource_Line3);
            m_camera->TriggerActivation.SetValue(TriggerActivation_RisingEdge);

            if (GenApi::IsAvailable(m_camera->LineSelector)) {
                m_camera->LineSelector.SetValue(LineSelector_Line4);
            }
            if (GenApi::IsAvailable(m_camera->LineMode)) {
                m_camera->LineMode.SetValue(LineMode_Output);
            }
            if (GenApi::IsAvailable(m_camera->LineSource)) {
                m_camera->LineSource.SetValue(LineSource_ExposureActive);
            }
            if (GenApi::IsAvailable(m_camera->LineInverter)) {
                m_camera->LineInverter.SetValue(true);
            }
            if (GenApi::IsAvailable(m_camera->AcquisitionFrameRateEnable)) {
                m_camera->AcquisitionFrameRateEnable.SetValue(false);
            }

            // Задержка триггера (опционально, можно оставить 0)
            if (GenApi::IsAvailable(m_camera->TriggerDelayAbs)) {
                m_camera->TriggerDelayAbs.SetValue(0.0);
            }
        }
    } catch (const GenericException& e) {
        emit sendErrorMessage(
            QString("MasterSlave config error: %1").arg(e.GetDescription()));
    }
}

void BaslerApi::processRawData() {
    QByteArray rawData((const char*)m_ptrGrabResult->GetBuffer(),
                       m_ptrGrabResult->GetImageSize());
    emit rawDataReceived(rawData, m_ptrGrabResult->GetWidth(),
                         m_ptrGrabResult->GetHeight(),
                         m_ptrGrabResult->GetPixelType());
}

void BaslerApi::applyPendingCommands() {
    if (!m_commandsPending.load()) return;

    std::vector<std::unique_ptr<ParameterCommand>> commands;
    {
        QMutexLocker locker(&m_commandsMutex);
        m_commandsPending = false;
        commands.swap(m_pendingCommands);
    }

    bool wasGrabbing = m_isGrabbing.load();
    if (wasGrabbing) {
        pauseGrabbing();
    }

    if (commands.empty()) return;

    for (auto& cmd : commands) {
        cmd->execute(this);
    }
    if (wasGrabbing) {
        startGrabbing();
    }
}
