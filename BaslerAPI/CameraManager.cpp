#include "CameraManager.h"
#include <QDebug>
#include <QString>

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
