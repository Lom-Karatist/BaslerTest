#include "BaslerSettingsForm.h"
#include "ui_BaslerSettingsForm.h"

BaslerSettingsForm::BaslerSettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaslerSettingsForm)
{
    ui->setupUi(this);
    ui->comboBoxPixelFormat->addItems(m_pixelFormats);
    ui->comboBoxBinningHorizontal->addItems(m_binningTypes);
    ui->comboBoxBinningVertical->addItems(m_binningTypes);
    ui->comboBoxBinningHMode->addItems(m_binningModes);
    ui->comboBoxBinningVMode->addItems(m_binningModes);
}

BaslerSettingsForm::~BaslerSettingsForm()
{
    delete ui;
}

void BaslerSettingsForm::setFormName(QString name)
{
    ui->groupBox->setTitle(name);
}

void BaslerSettingsForm::setCameraParams(BaslerCameraParams params)
{

}
