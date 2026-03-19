#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QThreadPool>
#include <QMutex>
#include <QAtomicInt>
#include "BaslerApi.h"
#include "BaslerSettings.h"

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

    const QString &savingPath() const;
    void setSavingPath(const QString &newSavingPath);

    const BaslerCameraParams &hsParams() const;
    void setHsParams(const BaslerCameraParams &newHsParams);

    const BaslerCameraParams &ocParams() const;
    void setOcParams(const BaslerCameraParams &newOcParams);

signals:
    void ready();
    void errorOccurred(const QString& message);
    void masterImageReady(const QImage& image);
    void slaveImageReady(const QImage& image);
    void masterRawData(const QByteArray& data, int w, int h);
    void slaveRawData(const QByteArray& data, int w, int h);
    void forceParameterChanging(bool isMaster, BaslerConstants::SettingTypes settingType, QVariant value);

public slots:
    void onSettingsChanged(bool isMaster, BaslerConstants::SettingTypes type, QVariant value);

private slots:
    void onMasterConnected(bool success);
    void onSlaveConnected(bool success);
    void onMasterError(const QString& err);
    void onSlaveError(const QString& err);
    void onMasterRawData(const QByteArray& data, int w, int h, int pixelFormat);
    void onSlaveRawData(const QByteArray& data, int w, int h, int pixelFormat);

private:
    QImage convertToQImage(const QByteArray& data, int width, int height, int pixelFormat);
    void saveChangedSettings(BaslerSettings &baslerSettingsObject, BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value);
    void processExposureAndFramerateChanging(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value);
    void processRoiAndBinningX(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value);
    void processRoiAndBinningY(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value);
    void calcRoiOnAxe(int &size, int &offset, int &binning, BaslerConstants::SettingTypes changedType, const QVariant &value, int maxSize);

    BaslerApi* m_master;
    BaslerApi* m_slave;
    BaslerCameraParams m_hsParams;
    BaslerCameraParams m_ocParams;
    BaslerSettings m_masterSettings;
    BaslerSettings m_slaveSettings;

    QAtomicInt m_connectedCount;
    bool m_ready;                  // флаг готовности
    QMutex m_mutex;                // защита m_ready
    bool m_isImageNeeded;

    QString m_savingPath;

    static const int MAX_WIDTH = 1936;
    static const int MAX_HEIGHT = 1216;
};

#endif // CAMERAMANAGER_H
