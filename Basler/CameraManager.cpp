// CameraManager.cpp
#include "CameraManager.h"
#include <QDebug>
#include <QDir>

CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
    , m_master(nullptr)
    , m_slave(nullptr)
    , m_masterSettings(this, QDir::currentPath() + "/HS.ini")
    , m_slaveSettings(this, QDir::currentPath() + "/OC.ini")
    , m_connectedCount(0)
    , m_ready(false)
    , m_isImageNeeded(true)
{
    PylonInitialize();
    m_hsParams = m_masterSettings.loadParamsFromFile();
    m_ocParams = m_slaveSettings.loadParamsFromFile();

    m_master = new BaslerApi(true, m_hsParams);
    m_slave  = new BaslerApi(false, m_ocParams);
    m_master->setAutoDelete(false);
    m_slave->setAutoDelete(false);

    m_savingModule.setSavingPath(QDir::currentPath());

    connect(m_master, &BaslerApi::connectionComplete, this, &CameraManager::onMasterConnected, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::connectionComplete, this, &CameraManager::onSlaveConnected, Qt::QueuedConnection);

    connect(m_master, &BaslerApi::sendErrorMessage, this, &CameraManager::onMasterError, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::sendErrorMessage, this, &CameraManager::onSlaveError, Qt::QueuedConnection);

    connect(m_master, &BaslerApi::rawDataReceived, this, &CameraManager::onMasterRawData, Qt::QueuedConnection);
    connect(m_slave, &BaslerApi::rawDataReceived, this, &CameraManager::onSlaveRawData, Qt::QueuedConnection);

    QThreadPool::globalInstance()->start(m_master);
    QThreadPool::globalInstance()->start(m_slave);
}

CameraManager::~CameraManager()
{
    stop();
    PylonTerminate();
}

void CameraManager::start()
{
    if (!m_master || !m_slave) return;
    m_master->startGrabbing();
    m_slave->startGrabbing();
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

void CameraManager::setSavingPath(const QString path)
{
    m_savingModule.setSavingPath(path);
}

void CameraManager::onMasterConnected(bool success)
{
    if (!success) {
        emit errorOccurred("Master camera failed to connect");
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
    QImage img = SavingModule::convertToQImage(data, w, h, pixelFormat);
    if (!img.isNull() && m_isImageNeeded) {
        emit masterImageReady(img);
    }
    if(m_savingModule.isNeedToSave()){
        m_savingModule.saveDataAsync(data, w, h, pixelFormat, "/master", m_frameTimeStamp);
    }
}

void CameraManager::onSlaveRawData(const QByteArray& data, int w, int h, int pixelFormat)
{
    QImage img = SavingModule::convertToQImage(data, w, h, pixelFormat);
    if (!img.isNull()) {
        emit slaveImageReady(img);
    }
    if(m_savingModule.isNeedToSave()){
        m_frameTimeStamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
        if (m_savingModule.isNeedToSave()) {
            m_savingModule.saveDataAsync(data, w, h, pixelFormat, "/slave", m_frameTimeStamp);
        }
    }
}

void CameraManager::saveChangedSettings(BaslerSettings &baslerSettingsObject, BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value)
{
    switch (type) {
    case BaslerConstants::SettingTypes::Exposure:
    case BaslerConstants::SettingTypes::AcquisitionFramerate:
        processExposureAndFramerateChanging(cameraParams, type, value);
        break;
    case BaslerConstants::SettingTypes::Gain:
        cameraParams.gain = value.toDouble();
        break;
    case BaslerConstants::SettingTypes::Width:
    case BaslerConstants::SettingTypes::OffsetX:
    case BaslerConstants::SettingTypes::BinningHorizontal:
        processRoiAndBinningX(cameraParams, type, value);
        emit forceParameterChanging(cameraParams.isMaster, BaslerConstants::SettingTypes::Width, cameraParams.width);
        emit forceParameterChanging(cameraParams.isMaster, BaslerConstants::SettingTypes::OffsetX, cameraParams.offsetX);
        break;
    case BaslerConstants::SettingTypes::Height:
    case BaslerConstants::SettingTypes::OffsetY:
    case BaslerConstants::SettingTypes::BinningVertical:
        processRoiAndBinningY(cameraParams, type, value);
        emit forceParameterChanging(cameraParams.isMaster, BaslerConstants::SettingTypes::Height, cameraParams.height);
        emit forceParameterChanging(cameraParams.isMaster, BaslerConstants::SettingTypes::OffsetY, cameraParams.offsetY);
        break;

    case BaslerConstants::SettingTypes::PixelFormat:
    {
        int index = value.toInt();
        if (index == 0)
            cameraParams.pixelFormat = PixelType_Mono8;
        else if (index == 1)
            cameraParams.pixelFormat = PixelType_Mono12;
        else if (index == 2)
            cameraParams.pixelFormat = PixelType_Mono12p;
    }
        break;
    case BaslerConstants::SettingTypes::BinningHorizontalMode:
    {
        int modeIndex = value.toInt();
        if (modeIndex == 0)
            cameraParams.binningHorizontalMode = BinningHorizontalMode_Sum;
        else
            cameraParams.binningHorizontalMode = BinningHorizontalMode_Average;
    }
        break;
    case BaslerConstants::SettingTypes::BinningVerticalMode:
    {
        int modeIndex = value.toInt();
        if (modeIndex == 0)
            cameraParams.binningVerticalMode = BinningVerticalMode_Sum;
        else
            cameraParams.binningVerticalMode = BinningVerticalMode_Average;
    }
        break;
    default:
        return;
    }

    baslerSettingsObject.saveParams(cameraParams);
}

void CameraManager::processExposureAndFramerateChanging(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value)
{
    const double safetyMargin = 0.99;

    if(type == BaslerConstants::SettingTypes::AcquisitionFramerate){
        cameraParams.acquisitionFrameRate = value.toDouble();
        double framePeriodMs = 1e3 / cameraParams.acquisitionFrameRate;
        double maxExposureMs = framePeriodMs * safetyMargin;
        if (cameraParams.exposureTime > maxExposureMs) {
            cameraParams.exposureTime = maxExposureMs;
            emit forceParameterChanging(cameraParams.isMaster, BaslerConstants::SettingTypes::Exposure, cameraParams.exposureTime);
            qDebug() << "Exposure changed to" << cameraParams.exposureTime << "ms due to framerate limit";
        }
    }else{
        cameraParams.exposureTime = value.toDouble();
        double minRequiredPeriodMs = cameraParams.exposureTime / safetyMargin;
        double maxAllowedFramerate = 1000.0 / minRequiredPeriodMs;
        if (cameraParams.acquisitionFrameRate > maxAllowedFramerate) {
            cameraParams.acquisitionFrameRate = maxAllowedFramerate;
            emit forceParameterChanging(cameraParams.isMaster, BaslerConstants::SettingTypes::AcquisitionFramerate, cameraParams.acquisitionFrameRate);
            qDebug() << "Framerate adjusted to" << cameraParams.acquisitionFrameRate << "fps due to exposure limit";
        }
    }
}

void CameraManager::processRoiAndBinningX(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value)
{
    calcRoiOnAxe(cameraParams.width, cameraParams.offsetX, cameraParams.binningHorizontal,
                 type, value, MAX_WIDTH);
    cameraParams.offsetX = (cameraParams.offsetX / 4) * 4;
}

void CameraManager::processRoiAndBinningY(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value)
{
    calcRoiOnAxe(cameraParams.height, cameraParams.offsetY, cameraParams.binningVertical,
                 type, value, MAX_HEIGHT);
    cameraParams.offsetY = (cameraParams.offsetY / 2) * 2;
}

void CameraManager::calcRoiOnAxe(int &size, int &offset, int &binning, BaslerConstants::SettingTypes changedType, const QVariant &value, int maxSize)
{
    if (changedType == BaslerConstants::BinningHorizontal || changedType == BaslerConstants::BinningVertical) {
        double physSize = static_cast<double>(size) * binning;
        double physOffset = static_cast<double>(offset) * binning;
        binning = qBound(1, value.toInt(), 4);

        int desiredSize = qRound(physSize / binning);
        int desiredOffset = physOffset / binning;

        size = qMin(desiredSize, maxOutSize(maxSize, binning));
        int maxOffset = maxSize - desiredSize * binning;
        offset = qMin(desiredOffset, maxOffset);
    }else if (changedType == BaslerConstants::Width || changedType == BaslerConstants::Height) {
        int oldSize = size;
        size = qBound(1, value.toInt(), maxOutSize(maxSize, binning));

        double scale = static_cast<double>(size) / oldSize;
        int neededOffset = scale*offset;
        int maxOffset = maxSize - size * binning;
        offset = qBound(0, neededOffset, maxOffset);
    }else if (changedType == BaslerConstants::OffsetX  || changedType == BaslerConstants::OffsetY) {
        int maxOffset = maxSize - size * binning;
        offset = qBound(0, value.toInt(), maxOffset);
    }
}

int CameraManager::maxOutSize(int maxSize, int binning)
{
    return maxSize / binning;
}

void CameraManager::setIsNeedToSave(bool newIsNeedToSave)
{
    m_savingModule.setIsNeedToSave(newIsNeedToSave);
}

const BaslerCameraParams &CameraManager::ocParams() const
{
    return m_ocParams;
}

void CameraManager::setOcParams(const BaslerCameraParams &newOcParams)
{
    m_ocParams = newOcParams;
}

void CameraManager::onSettingsChanged(bool isMaster, BaslerConstants::SettingTypes type, QVariant value)
{
    if (isMaster) {
        saveChangedSettings(m_masterSettings, m_hsParams, type, value);
    } else {
        saveChangedSettings(m_slaveSettings, m_ocParams, type, value);
    }
}

void CameraManager::onSavingModeChanged(const int savingFormat)
{
    switch(savingFormat){
    case 1:     m_savingModule.setFormat(BaslerConstants::SavingFormat::Binary);    break;
    case 0:     m_savingModule.setFormat(BaslerConstants::SavingFormat::Bmp);       break;
    default:    break;
    }
}

const BaslerCameraParams &CameraManager::hsParams() const
{
    return m_hsParams;
}

void CameraManager::setHsParams(const BaslerCameraParams &newHsParams)
{
    m_hsParams = newHsParams;
}
