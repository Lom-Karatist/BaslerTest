#include "CameraManager.h"
#include <QDebug>
#include <QString>
#include <pylon/BaslerUniversalInstantCamera.h>

using namespace Pylon;

CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
    try {
        PylonInitialize();
        m_initialized = true;
        qDebug() << "Pylon SDK initialized successfully.";
    } catch (const GenericException &e) {
        qCritical() << "Failed to initialize Pylon SDK:" << e.what();
    }
}

CameraManager::~CameraManager()
{
    if (m_initialized) {
        PylonTerminate();
        qDebug() << "Pylon SDK terminated.";
    }
}

QList<CameraInfo> CameraManager::enumerateCameras()
{
    QList<CameraInfo> cameras;
    if (!m_initialized) {
        qWarning() << "Pylon not initialized, cannot enumerate cameras.";
        return cameras;
    }

    try {
        CTlFactory& tlFactory = CTlFactory::GetInstance();
        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices(devices) == 0) {
            qDebug() << "No cameras found.";
            return cameras;
        }

        for (size_t i = 0; i < devices.size(); ++i) {
            CameraInfo info;
            info.fullName = devices[i].GetFullName();
            info.modelName = devices[i].GetModelName();
            info.serialNumber = devices[i].GetSerialNumber();
            info.vendor = devices[i].GetVendorName();
            info.deviceVersion = devices[i].GetDeviceVersion();
            info.isAvailable = devices[i].IsUsbDriverTypeAvailable();

            cameras.append(info);
            qDebug() << "Found camera:" << info.modelName << "SN:" << info.serialNumber;
        }
    } catch (const GenericException &e) {
        qCritical() << "Error enumerating cameras:" << e.what();
    }

    return cameras;
}

QImage CameraManager::grabFrame(int index, int timeoutMs)
{
    if (!m_initialized) return QImage();

        try {
            CTlFactory& tlFactory = CTlFactory::GetInstance();
            DeviceInfoList_t devices;
            if (tlFactory.EnumerateDevices(devices) == 0 || index < 0 || index >= (int)devices.size())
                return QImage();

            // Создаём объект камеры для выбранного устройства
            CBaslerUniversalInstantCamera camera(tlFactory.CreateDevice(devices[index]));
            camera.Open();

            // Захватываем ровно один кадр
            camera.StartGrabbing(1, GrabStrategy_OneByOne);
            CGrabResultPtr ptrGrabResult;
            if (camera.RetrieveResult(timeoutMs, ptrGrabResult, TimeoutHandling_ThrowException)) {
                if (ptrGrabResult->GrabSucceeded()) {
                    uint8_t* buffer = (uint8_t*)ptrGrabResult->GetBuffer();
                    int width = ptrGrabResult->GetWidth();
                    int height = ptrGrabResult->GetHeight();
                    EPixelType pixelType = ptrGrabResult->GetPixelType();

                    QImage image;
                    if (pixelType == PixelType_Mono8) {
                        // Прямое копирование 8-битных данных
                        image = QImage(buffer, width, height, width, QImage::Format_Grayscale8).copy();
                    }
                    else if (pixelType == PixelType_Mono12 || pixelType == PixelType_Mono12p) {
                        // Конвертация 12-бит → 8 бит (сдвиг на 4 бита)
                        QByteArray data8(width * height, Qt::Uninitialized);
                        uint16_t* src = (uint16_t*)buffer;
                        uint8_t* dst = (uint8_t*)data8.data();
                        for (int i = 0; i < width * height; ++i) {
                            dst[i] = src[i] >> 4;   // можно заменить на src[i] / 16 или другое масштабирование
                        }
                        image = QImage((uchar*)data8.constData(), width, height, width, QImage::Format_Grayscale8).copy();
                    }
                    // При необходимости добавьте обработку других форматов (Bayer, RGB и т.д.)

                    camera.StopGrabbing();
                    camera.Close();
                    return image;
                }
            }
            camera.Close();
        } catch (const GenericException &e) {
            qCritical() << "Error grabbing frame:" << e.what();
        }
        return QImage();
}
