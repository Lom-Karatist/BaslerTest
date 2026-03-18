#ifndef BASLERSETTINGS_H
#define BASLERSETTINGS_H

#include <QObject>
#include "BaslerApi.h"
#include <QSettings>
#include <QStringList>

namespace BaslerConstants {
    inline const QStringList pixelFormats() {
        static const QStringList list = {"Mono8", "Mono12", "Mono12p"};
        return list;
    }

    inline const QStringList binningTypes() {
        static const QStringList list = {"1", "2", "3", "4"};
        return list;
    }

    inline const QStringList binningModes() {
        static const QStringList list = {"Sum", "Average"};
        return list;
    }

    enum SettingTypes{
        Exposure,
        Gain,
        PixelFormat,
        AcquisitionFramerate,
        Width,
        Height,
        OffsetX,
        OffsetY,
        BinningHorizontal,
        BinningVertical,
        BinningHorizontalMode,
        BinningVerticalMode
    };
}

class BaslerSettings : public QObject
{
    Q_OBJECT
public:
    explicit BaslerSettings(QObject *parent = nullptr, QString fileName = "");
    ~BaslerSettings();

    BaslerCameraParams loadParamsFromFile();

    void saveParams(BaslerCameraParams &cameraParams);

    /**
     * @brief settings  Function to get Settings object
     * @return Settings object
     */
    QSettings *settings() const;

signals:

private:
    /**
     * @brief createSettingsObject  Static function to create settings object depending on project name
     * @param iniFileName   Ini file text name
     * @return  Settings object
     */
    static QSettings *createSettingsObject(QString iniFileName);

    QSettings *m_settings;
};

#endif // BASLERSETTINGS_H
