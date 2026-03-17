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
{
    m_master = new BaslerApi(true, masterParams, m_serialMaster);
    m_slave  = new BaslerApi(false, slaveParams, m_serialSlave);

    // Подключаем сигналы (используем QueuedConnection, чтобы слоты выполнялись в потоке менеджера, т.е. главном)
    connect(m_master, &BaslerApi::connectionComplete,
            this, &CameraManager::onMasterConnected, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::connectionComplete,
            this, &CameraManager::onSlaveConnected, Qt::QueuedConnection);

    connect(m_master, &BaslerApi::sendErrorMessage,
            this, &CameraManager::onMasterError, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::sendErrorMessage,
            this, &CameraManager::onSlaveError, Qt::QueuedConnection);

    connect(m_master, &BaslerApi::imageReceived,
            this, &CameraManager::onMasterImage, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::imageReceived,
            this, &CameraManager::onSlaveImage, Qt::QueuedConnection);

    connect(m_master, &BaslerApi::rawDataReceived,
            this, &CameraManager::onMasterRawData, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::rawDataReceived,
            this, &CameraManager::onSlaveRawData, Qt::QueuedConnection);
}

CameraManager::~CameraManager()
{
    stop();

    if (m_master) {
        m_master->deleteLater();
        m_master = nullptr;
    }
    if (m_slave) {
        m_slave->deleteLater();
        m_slave = nullptr;
    }

    QThreadPool::globalInstance()->waitForDone(3000);
}

void CameraManager::start()
{
    if (!m_master || !m_slave) return;

    m_connectedCount = 0;
    m_ready = false;

    QThreadPool::globalInstance()->start(m_master);
    QThreadPool::globalInstance()->start(m_slave);
}

void CameraManager::stop()
{
    if (m_master) {
        m_master->stopGrabbing();
    }
    if (m_slave) {
        m_slave->stopGrabbing();
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

void CameraManager::onMasterImage(const QImage& img)
{
    emit masterImageReceived(img);
}

void CameraManager::onSlaveImage(const QImage& img)
{
    emit slaveImageReceived(img);
}

void CameraManager::onMasterRawData(const QByteArray& data, int w, int h)
{
    emit masterRawData(data, w, h);
}

void CameraManager::onSlaveRawData(const QByteArray& data, int w, int h)
{
    emit slaveRawData(data, w, h);
}
