#include "BaslerSettingsForm.h"
#include "ui_BaslerSettingsForm.h"
#include <QDebug>

BaslerSettingsForm::BaslerSettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaslerSettingsForm)
{
    ui->setupUi(this);
    m_isMaster = false;
    ui->comboBoxPixelFormat->addItems(BaslerConstants::pixelFormats());
    ui->comboBoxBinningHorizontal->addItems(BaslerConstants::binningTypes());
    ui->comboBoxBinningVertical->addItems(BaslerConstants::binningTypes());
    ui->comboBoxBinningHMode->addItems(BaslerConstants::binningModes());
    ui->comboBoxBinningVMode->addItems(BaslerConstants::binningModes());
}

BaslerSettingsForm::~BaslerSettingsForm()
{
    delete ui;
}

void BaslerSettingsForm::setFormName(QString name)
{
    ui->groupBox->setTitle(name);
}

void BaslerSettingsForm::setCameraParams(const BaslerCameraParams& params)
{
    m_isMaster = params.isMaster;
    setExposure(params.exposureTime);
    setGain(params.gain);
    setPixelFormat(params.pixelFormat);
    setAcquisitionFrameRate(static_cast<int>(params.acquisitionFrameRate));
    setWidth(params.width);
    setHeight(params.height);
    setOffsetX(params.offsetX);
    setOffsetY(params.offsetY);
    setBinningHorizontal(params.binningHorizontal);
    setBinningVertical(params.binningVertical);
    setBinningHorizontalMode(params.binningHorizontalMode);
    setBinningVerticalMode(params.binningVerticalMode);

    connectChangingSignals();
}

void BaslerSettingsForm::setExposure(double value)
{
    ui->doubleSpinBoxExpo->setValue(value);
}

void BaslerSettingsForm::setGain(double value)
{
    ui->doubleSpinBoxGain->setValue(value);
}

void BaslerSettingsForm::setPixelFormat(int pixelFormat)
{
    int index = 0;
    switch (pixelFormat) {
    case PixelType_Mono8:
        index = BaslerConstants::pixelFormats().indexOf("Mono8");
        break;
    case PixelType_Mono12:
        index = BaslerConstants::pixelFormats().indexOf("Mono12");
        break;
    case PixelType_Mono12p:
        index = BaslerConstants::pixelFormats().indexOf("Mono12p");
        break;
    default:
        index = 0;
    }
    if (index >= 0)
        ui->comboBoxPixelFormat->setCurrentIndex(index);
}

void BaslerSettingsForm::setAcquisitionFrameRate(double fps)
{
    ui->doubleSpinBoxAcquisitionFramerate->setValue(fps);
}

void BaslerSettingsForm::setWidth(int width)
{
    ui->spinBoxWidth->setValue(width);
}

void BaslerSettingsForm::setHeight(int height)
{
    ui->spinBoxHeight->setValue(height);
}

void BaslerSettingsForm::setOffsetX(int offsetX)
{
    ui->spinBoxOffsetX->setValue(offsetX);
}

void BaslerSettingsForm::setOffsetY(int offsetY)
{
    ui->spinBoxOffsetY->setValue(offsetY);
}

void BaslerSettingsForm::setBinningHorizontal(int bin)
{
    int index = BaslerConstants::binningTypes().indexOf(QString::number(bin));
    if (index >= 0)
        ui->comboBoxBinningHorizontal->setCurrentIndex(index);
}

void BaslerSettingsForm::setBinningVertical(int bin)
{
    int index = BaslerConstants::binningTypes().indexOf(QString::number(bin));
    if (index >= 0)
        ui->comboBoxBinningVertical->setCurrentIndex(index);
}

void BaslerSettingsForm::setBinningHorizontalMode(BinningHorizontalModeEnums mode)
{
    int index = 0;
    if (mode == BinningHorizontalMode_Sum)
        index = BaslerConstants::binningModes().indexOf("Sum");
    else if (mode == BinningHorizontalMode_Average)
        index = BaslerConstants::binningModes().indexOf("Average");
    if (index >= 0)
        ui->comboBoxBinningHMode->setCurrentIndex(index);
}

void BaslerSettingsForm::setBinningVerticalMode(BinningVerticalModeEnums mode)
{
    int index = 0;
    if (mode == BinningVerticalMode_Sum)
        index = BaslerConstants::binningModes().indexOf("Sum");
    else if (mode == BinningVerticalMode_Average)
        index = BaslerConstants::binningModes().indexOf("Average");
    if (index >= 0)
        ui->comboBoxBinningVMode->setCurrentIndex(index);
}

void BaslerSettingsForm::updateValueInGui(bool isMaster, BaslerConstants::SettingTypes settingType, QVariant value)
{
    if(isMaster != m_isMaster)
        return;

    switch(settingType){
    case BaslerConstants::Exposure:
        ui->doubleSpinBoxExpo->setValue(value.toDouble());
        break;
    case BaslerConstants::AcquisitionFramerate:
        ui->doubleSpinBoxAcquisitionFramerate->setValue(value.toDouble());
        break;
    case BaslerConstants::Width:
        ui->spinBoxWidth->setValue(value.toInt());
        break;
    case BaslerConstants::Height:
        ui->spinBoxHeight->setValue(value.toInt());
        break;
    case BaslerConstants::OffsetX:
        ui->spinBoxOffsetX->setValue(value.toInt());
        break;
    case BaslerConstants::OffsetY:
        ui->spinBoxOffsetY->setValue(value.toInt());
        break;
    default:
        break;
    }
}

void BaslerSettingsForm::connectChangingSignals()
{
    connect(ui->doubleSpinBoxExpo, &QDoubleSpinBox::editingFinished,
            this, [this]() {
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::Exposure,
                                         ui->doubleSpinBoxExpo->value());
            });
    connect(ui->doubleSpinBoxGain, &QDoubleSpinBox::editingFinished,
            this, [this]() {
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::Gain,
                                         ui->doubleSpinBoxGain->value());
            });
    connect(ui->doubleSpinBoxAcquisitionFramerate, &QDoubleSpinBox::editingFinished,
            this, [this]() {
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::AcquisitionFramerate,
                                         ui->doubleSpinBoxAcquisitionFramerate->value());
            });
    connect(ui->spinBoxWidth, &QSpinBox::editingFinished,
            this, [this]() {
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::Width,
                                         ui->spinBoxWidth->value());
            });
    connect(ui->spinBoxHeight, &QSpinBox::editingFinished,
            this, [this]() {
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::Height,
                                         ui->spinBoxHeight->value());
            });
    connect(ui->spinBoxOffsetX, &QSpinBox::editingFinished,
            this, [this]() {
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::OffsetX,
                                         ui->spinBoxOffsetX->value());
            });
    connect(ui->spinBoxOffsetY, &QSpinBox::editingFinished,
            this, [this]() {
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::OffsetY,
                                         ui->spinBoxOffsetY->value());
            });

    connect(ui->comboBoxPixelFormat, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int /*index*/) {
                QString text = ui->comboBoxPixelFormat->currentText();
                auto fmt = BaslerConstants::pixelFormats().indexOf(text);
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::PixelFormat, static_cast<int>(fmt));
            });
    connect(ui->comboBoxBinningHorizontal, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int /*index*/) {
                int bin = ui->comboBoxBinningHorizontal->currentText().toInt();
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::BinningHorizontal, bin);
            });
    connect(ui->comboBoxBinningVertical, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int /*index*/) {
                int bin = ui->comboBoxBinningVertical->currentText().toInt();
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::BinningVertical, bin);
            });
    connect(ui->comboBoxBinningHMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int /*index*/) {
                QString mode = ui->comboBoxBinningHMode->currentText();
                auto binMode = BaslerConstants::binningModes().indexOf(mode);
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::BinningHorizontalMode, static_cast<int>(binMode));
            });
    connect(ui->comboBoxBinningVMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int /*index*/) {
                QString mode = ui->comboBoxBinningVMode->currentText();
                auto binMode = BaslerConstants::binningModes().indexOf(mode);
                emit settingsWereChanged(m_isMaster, BaslerConstants::SettingTypes::BinningVerticalMode, static_cast<int>(binMode));
            });
}
