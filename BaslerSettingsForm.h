#ifndef BASLERSETTINGSFORM_H
#define BASLERSETTINGSFORM_H

#include <QWidget>
#include <BaslerAPI/CameraManager.h>

namespace Ui {
class BaslerSettingsForm;
}

class BaslerSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit BaslerSettingsForm(QWidget *parent = nullptr);
    ~BaslerSettingsForm();

    void setFormName(QString name);
    void setCameraParams(const BaslerCameraParams &params);

    void setExposure(double value);
    void setGain(double value);
    void setPixelFormat(int pixelFormat);  // принимает значение из EPixelType
    void setAcquisitionFrameRate(int fps);
    void setWidth(int width);
    void setHeight(int height);
    void setOffsetX(int offsetX);
    void setOffsetY(int offsetY);
    void setBinningHorizontal(int bin);
    void setBinningVertical(int bin);
    void setBinningHorizontalMode(BinningHorizontalModeEnums mode);
    void setBinningVerticalMode(BinningVerticalModeEnums mode);

signals:
    void settingsWereChanged(bool isMaster, BaslerConstants::SettingTypes settingType, QVariant value);

private:
    void connectChangingSignals();

    Ui::BaslerSettingsForm *ui;
    bool m_isMaster;
};

#endif // BASLERSETTINGSFORM_H
