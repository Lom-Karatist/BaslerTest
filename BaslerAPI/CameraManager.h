#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QThreadPool>
#include <QMutex>
#include <QAtomicInt>
#include "BaslerApi.h"

class CameraManager : public QObject
{
    Q_OBJECT

public:
    explicit CameraManager(QObject *parent = nullptr);
    ~CameraManager();
    void start();
    void pause();
    void stop();
    bool isReady() const { return m_ready; }

signals:
    void ready();
    void errorOccurred(const QString& message);
    void masterImageReady(const QImage& image);
    void slaveImageReady(const QImage& image);
    void masterRawData(const QByteArray& data, int w, int h);
    void slaveRawData(const QByteArray& data, int w, int h);

private slots:
    void onMasterConnected(bool success);
    void onSlaveConnected(bool success);
    void onMasterError(const QString& err);
    void onSlaveError(const QString& err);
    void onMasterRawData(const QByteArray& data, int w, int h, int pixelFormat);
    void onSlaveRawData(const QByteArray& data, int w, int h, int pixelFormat);

private:
    static BaslerCameraParams loadParamsFromFile(const QString& filePath);
    QImage convertToQImage(const QByteArray& data, int width, int height, int pixelFormat);

    BaslerApi* m_master;
    BaslerApi* m_slave;

    QAtomicInt m_connectedCount;   // счётчик успешных подключений
    bool m_ready;                  // флаг готовности
    QMutex m_mutex;                // защита m_ready
    bool m_isImageNeeded;
};

#endif // CAMERAMANAGER_H
