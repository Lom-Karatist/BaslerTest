#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QStringList>
#include <pylon/PylonIncludes.h>
#include <QImage>  // добавить в начало

struct CameraInfo {
    QString fullName;
    QString modelName;
    QString serialNumber;
    QString vendor;
    QString deviceVersion;
    bool isAvailable;
};

class CameraManager : public QObject
{
    Q_OBJECT
public:
    explicit CameraManager(QObject *parent = nullptr);
    ~CameraManager();

    QList<CameraInfo> enumerateCameras();

    bool isInitialized() const { return m_initialized; }

    /**
     * @brief grabFrame Захват кадра с камеры по индексу
     * @param index Индекс кадра
     * @param timeoutMs Время ожидания
     * @return  Изображение
     */
    QImage grabFrame(int index, int timeoutMs = 5000);  // захват кадра с камеры по индексу

private:
    bool m_initialized;
};

#endif // CAMERAMANAGER_H
