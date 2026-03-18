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
    void setCameraParams(BaslerCameraParams params);

private:
    Ui::BaslerSettingsForm *ui;
};

#endif // BASLERSETTINGSFORM_H
