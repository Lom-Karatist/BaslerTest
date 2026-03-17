// CameraManager.cpp
#include "CameraManager.h"
#include <QDebug>

CameraManager::CameraManager(const BaslerCameraParams& masterParams,
                             const BaslerCameraParams& slaveParams,
                             QObject *parent)
    : QObject(parent)
    , m_master(nullptr)
    , m_slave(nullptr)
    , m_connectedCount(0)
    , m_ready(false)
    , m_isImageNeeded(true)
{
    PylonInitialize();
    m_master = new BaslerApi(true, masterParams, m_serialMaster);
    m_slave  = new BaslerApi(false, slaveParams, m_serialSlave);
    m_master->setAutoDelete(false);
    m_slave->setAutoDelete(false);

    // Подключаем сигналы (используем QueuedConnection, чтобы слоты выполнялись в потоке менеджера, т.е. главном)
    connect(m_master, &BaslerApi::connectionComplete,
            this, &CameraManager::onMasterConnected, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::connectionComplete,
            this, &CameraManager::onSlaveConnected, Qt::QueuedConnection);

    connect(m_master, &BaslerApi::sendErrorMessage,
            this, &CameraManager::onMasterError, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::sendErrorMessage,
            this, &CameraManager::onSlaveError, Qt::QueuedConnection);

    connect(m_master, &BaslerApi::rawDataReceived,
            this, &CameraManager::onMasterRawData, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::rawDataReceived,
            this, &CameraManager::onSlaveRawData, Qt::QueuedConnection);
}

CameraManager::~CameraManager()
{
    stop();
    PylonTerminate();
}

void CameraManager::start()
{
    if (!m_master || !m_slave) return;

    QThreadPool::globalInstance()->start(m_master);
    QThreadPool::globalInstance()->start(m_slave);
}

void CameraManager::pause()
{
    if (m_master) m_master->pauseGrabbing();
    if (m_slave) m_slave->pauseGrabbing();
}

void CameraManager::stop()
{
    if (m_master) m_master->stopGrabbing();
    if (m_slave) m_slave->stopGrabbing();

    QThreadPool::globalInstance()->waitForDone(2000);

    if (m_master) {
        m_master->deleteLater();
        m_master = nullptr;
    }
    if (m_slave) {
        m_slave->deleteLater();
        m_slave = nullptr;
    }
}

void CameraManager::onMasterConnected(bool success)
{
    if (!success) {
        emit errorOccurred("Master camera failed to connect");
        stop(); // останавливаем обе камеры
        return;
    }

    int count = m_connectedCount.fetchAndAddOrdered(1) + 1;
    if (count == 2) {
        QMutexLocker locker(&m_mutex);
        m_ready = true;
        emit ready();
    }
}

void CameraManager::onSlaveConnected(bool success)
{
    if (!success) {
        emit errorOccurred("Slave camera failed to connect");
        stop();
        return;
    }

    int count = m_connectedCount.fetchAndAddOrdered(1) + 1;
    if (count == 2) {
        QMutexLocker locker(&m_mutex);
        m_ready = true;
        emit ready();
    }
}

void CameraManager::onMasterError(const QString& err)
{
    emit errorOccurred("Master: " + err);
    stop();
    m_ready = false;
}

void CameraManager::onSlaveError(const QString& err)
{
    emit errorOccurred("Slave: " + err);
    stop();
    m_ready = false;
}

void CameraManager::onMasterRawData(const QByteArray& data, int w, int h, int pixelFormat)
{
    QImage img = convertToQImage(data, w, h, pixelFormat);
    if (!img.isNull() && m_isImageNeeded) {
        emit masterImageReady(img);
    }
}

void CameraManager::onSlaveRawData(const QByteArray& data, int w, int h, int pixelFormat)
{
    QImage img = convertToQImage(data, w, h, pixelFormat);
    if (!img.isNull()) {
        emit slaveImageReady(img);
    }
}

QImage CameraManager::convertToQImage(const QByteArray &data, int width, int height, int pixelFormat)
{
    QImage::Format format = QImage::Format_Invalid;
    int bytesPerPixel = 0;

    switch (pixelFormat) {
    case PixelType_Mono8:
        format = QImage::Format_Grayscale8;
        bytesPerPixel = 1;
        break;
    case PixelType_Mono12:
    case PixelType_Mono16:
        format = QImage::Format_Grayscale16;
        bytesPerPixel = 2;
        break;
    default:
        qWarning() << "Unsupported pixel format:" << pixelFormat;
        return QImage();
    }

    if (format == QImage::Format_Invalid) return QImage();

    if (data.size() != width * height * bytesPerPixel) {
        qWarning() << "Data size mismatch";
        return QImage();
    }

    QImage image(reinterpret_cast<const uchar*>(data.constData()), width, height, width * bytesPerPixel, format);
    return image.copy();
}
