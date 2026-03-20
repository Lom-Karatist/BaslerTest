#include "BaslerSettings.h"
#include <QFile>

BaslerSettings::BaslerSettings(QObject *parent, QString fileName)
    : QObject{parent}
{
    m_settings = createSettingsObject(fileName);
}

BaslerSettings::~BaslerSettings()
{
    delete m_settings;
}

BaslerCameraParams BaslerSettings::loadParamsFromFile()
{
    BaslerCameraParams params;

    m_settings->beginGroup("Camera");
    params.serialNumber = m_settings->value("serialNumber", "").toString();
    params.isMaster = m_settings->value("isMaster", false).toBool();

    params.exposureTime = m_settings->value("exposureTime", 10000.0).toDouble();
    params.gain = m_settings->value("gain", 1.0).toDouble();
    params.acquisitionFrameRate = m_settings->value("acquisitionFrameRate", 10.0).toDouble();
    QString pixFmt = m_settings->value("pixelFormat", BaslerConstants::pixelFormats().at(0)).toString();
    if (pixFmt == BaslerConstants::pixelFormats().at(0)) params.pixelFormat = PixelType_Mono8;
    else if (pixFmt == BaslerConstants::pixelFormats().at(1)) params.pixelFormat = PixelType_Mono12;
    else if (pixFmt == BaslerConstants::pixelFormats().at(2)) params.pixelFormat = PixelType_Mono12p;

    params.width = m_settings->value("width", 1920).toInt();
    params.height = m_settings->value("height", 1200).toInt();
    params.offsetX = m_settings->value("offsetX", 0).toInt();
    params.offsetY = m_settings->value("offsetY", 0).toInt();
    params.binningHorizontal = m_settings->value("binningHorizontal", 1).toInt();
    params.binningVertical = m_settings->value("binningVertical", 1).toInt();

    QString binHMode = m_settings->value("binningHorizontalMode", "Average").toString();
    if (binHMode == BaslerConstants::binningModes().at(1)) params.binningHorizontalMode = BinningHorizontalMode_Average;
    else if (binHMode == BaslerConstants::binningModes().at(0)) params.binningHorizontalMode = BinningHorizontalMode_Sum;

    QString binVMode = m_settings->value("binningVerticalMode", "Average").toString();
    if (binVMode == BaslerConstants::binningModes().at(1)) params.binningVerticalMode = BinningVerticalMode_Average;
    else if (binVMode == BaslerConstants::binningModes().at(0)) params.binningVerticalMode = BinningVerticalMode_Sum;

    m_settings->endGroup();

    return params;
}

void BaslerSettings::saveParams(const BaslerCameraParams &cameraParams)
{
    m_settings->beginGroup("Camera");

    m_settings->setValue("serialNumber", cameraParams.serialNumber);
    m_settings->setValue("isMaster", cameraParams.isMaster);
    m_settings->setValue("exposureTime", cameraParams.exposureTime);
    m_settings->setValue("gain", cameraParams.gain);
    m_settings->setValue("acquisitionFrameRate", cameraParams.acquisitionFrameRate);

    QString pixFmt;
    if (cameraParams.pixelFormat == PixelType_Mono8)
       pixFmt = BaslerConstants::pixelFormats().at(0);
    else if (cameraParams.pixelFormat == PixelType_Mono12)
       pixFmt = BaslerConstants::pixelFormats().at(1);
    else if (cameraParams.pixelFormat == PixelType_Mono12p)
       pixFmt = BaslerConstants::pixelFormats().at(2);
    else
       pixFmt = BaslerConstants::pixelFormats().at(0); // значение по умолчанию
    m_settings->setValue("pixelFormat", pixFmt);

    m_settings->setValue("width", cameraParams.width);
    m_settings->setValue("height", cameraParams.height);
    m_settings->setValue("offsetX", cameraParams.offsetX);
    m_settings->setValue("offsetY", cameraParams.offsetY);

    m_settings->setValue("binningHorizontal", cameraParams.binningHorizontal);
    m_settings->setValue("binningVertical", cameraParams.binningVertical);

    QString binHMode;
    if (cameraParams.binningHorizontalMode == BinningHorizontalMode_Sum)
       binHMode = BaslerConstants::binningModes().at(0); // "Sum"
    else
       binHMode = BaslerConstants::binningModes().at(1); // "Average"
    m_settings->setValue("binningHorizontalMode", binHMode);

    QString binVMode;
    if (cameraParams.binningVerticalMode == BinningVerticalMode_Sum)
       binVMode = BaslerConstants::binningModes().at(0);
    else
       binVMode = BaslerConstants::binningModes().at(1);
    m_settings->setValue("binningVerticalMode", binVMode);

    m_settings->endGroup();
    m_settings->sync();
}

const QSettings *BaslerSettings::settings() const
{
    return m_settings;
}

QSettings *BaslerSettings::createSettingsObject(QString iniFileName)
{
    bool isIniExists = QFile(iniFileName).exists();

//    if(!isIniExists){
//        QFile resFile;
//        QString qrcFileName = ":/4Release/" + projectName + ".ini";
//        resFile.copy(qrcFileName, currentPath);
//        QFile fileCopied(currentPath);
//        fileCopied.setPermissions(QFileDevice::WriteOther);
//    }
    qDebug()<<"ini loading result:"<<isIniExists;

    return new QSettings(iniFileName, QSettings::IniFormat);
}
