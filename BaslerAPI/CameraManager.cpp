#include "CameraManager.h"

CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
    , m_camera(nullptr)
    , m_isOpen(false)
{
    PylonInitialize();
}

CameraManager::~CameraManager()
{
    closeCamera();
    PylonTerminate();
}

bool CameraManager::openCamera()
{
    try {
        // Получаем список устройств
        CTlFactory& tlFactory = CTlFactory::GetInstance();
        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices(devices) == 0) {
            emit errorOccurred("No cameras found");
            return false;
        }

        // Создаем объект камеры для первого устройства
        m_camera = new CBaslerUniversalInstantCamera(tlFactory.CreateDevice(devices[0]));
        m_camera->Open();

        // Выведем информацию о камере
        qDebug() << "Camera opened:" << m_camera->GetDeviceInfo().GetModelName();
        m_isOpen = true;
        return true;
    }
    catch (const GenericException &e) {
        emit errorOccurred(QString("Failed to open camera: %1").arg(e.what()));
        return false;
    }
}

void CameraManager::closeCamera()
{
    if (m_camera) {
        if (m_camera->IsOpen())
            m_camera->Close();
        delete m_camera;
        m_camera = nullptr;
        m_isOpen = false;
    }
}

bool CameraManager::grabOneFrame()
{
    if (!m_isOpen || !m_camera) return false;

    try {
        // Настраиваем захват одного кадра
        m_camera->StartGrabbing(1, GrabStrategy_OneByOne);

        CGrabResultPtr ptrGrabResult;
        if (m_camera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException)) {
            if (ptrGrabResult->GrabSucceeded()) {
                // Получаем информацию о кадре
                int width = ptrGrabResult->GetWidth();
                int height = ptrGrabResult->GetHeight();
                int pixelSize = ptrGrabResult->GetPixelType() == PixelType_Mono8 ? 1 : 2; // упрощённо
                quint64 timestamp = ptrGrabResult->GetTimeStamp();

                // Здесь можно получить указатель на данные: ptrGrabResult->GetBuffer()
                // Но для теста просто передадим параметры
                emit frameGrabbed(width, height, pixelSize, timestamp);

                qDebug() << "Frame grabbed:"
                         << width << "x" << height
                         << "pixelSize:" << pixelSize
                         << "timestamp:" << timestamp;
            } else {
                emit errorOccurred(QString("Grab failed: %1").arg(ptrGrabResult->GetErrorCode()));
            }
        }
        m_camera->StopGrabbing();
        return true;
    }
    catch (const GenericException &e) {
        emit errorOccurred(QString("Grab exception: %1").arg(e.what()));
        return false;
    }
}
