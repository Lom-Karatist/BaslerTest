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
    explicit CameraManager(const BaslerCameraParams& masterParams,
                           const BaslerCameraParams& slaveParams,
                           QObject *parent = nullptr);
    ~CameraManager();

    // Запуск обеих камер (инициализация и захват)
    void start();

    // Остановка захвата и освобождение ресурсов
    void stop();

    // Получить состояние готовности
    bool isReady() const { return m_ready; }

signals:
    // Сигнал, что обе камеры подключены и готовы к работе
    void ready();

    // Сигнал ошибки (для GUI)
    void errorOccurred(const QString& message);

    // Проброс сигналов от камер (например, для отображения)
    void masterImageReceived(const QImage& image);
    void slaveImageReceived(const QImage& image);
    void masterRawData(const QByteArray& data, int w, int h);
    void slaveRawData(const QByteArray& data, int w, int h);

private slots:
    // Слоты для обработки сигналов от камер
    void onMasterConnected(bool success);
    void onSlaveConnected(bool success);
    void onMasterError(const QString& err);
    void onSlaveError(const QString& err);
    void onMasterImage(const QImage& img);
    void onSlaveImage(const QImage& img);
    void onMasterRawData(const QByteArray& data, int w, int h);
    void onSlaveRawData(const QByteArray& data, int w, int h);

private:
    BaslerApi* m_master;
    BaslerApi* m_slave;

    QAtomicInt m_connectedCount;   // счётчик успешных подключений
    bool m_ready;                  // флаг готовности
    QMutex m_mutex;                // защита m_ready
    const QString m_serialMaster = "25338664";
    const QString m_serialSlave = "25338650";
};

#endif // CAMERAMANAGER_H
