#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QStringList>
#include <pylon/PylonIncludes.h>

// Структура для хранения информации о камере
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

    // Получить список доступных камер
    QList<CameraInfo> enumerateCameras();

    // Проверить, инициализирован ли Pylon
    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized;
};

#endif // CAMERAMANAGER_H
