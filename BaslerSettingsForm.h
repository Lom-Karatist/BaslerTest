#ifndef BASLERSETTINGSFORM_H
#define BASLERSETTINGSFORM_H

#include <QWidget>
#include <Basler/CameraManager.h>

namespace Ui {
class BaslerSettingsForm;
}

/**
 * @class BaslerSettingsForm
 * @brief Виджет для отображения и редактирования параметров камеры.
 *
 * Предоставляет интерфейс с элементами управления для всех настроек,
 * определённых в BaslerConstants::SettingTypes. При изменении любого параметра
 * излучает сигнал `settingsWereChanged` с указанием роли камеры (isMaster),
 * типа настройки и нового значения.
 *
 * Виджет заполняется значениями из структуры BaslerCameraParams через метод
 * setCameraParams. Все изменения передаются наружу через сигнал, что позволяет
 * отделить GUI от логики применения настроек.
 */
class BaslerSettingsForm : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор.
     * @param parent Родительский QWidget.
     *
     * Инициализирует UI, заполняет комбобоксы списками из BaslerConstants.
     * По умолчанию флаг m_isMaster = false (должен быть установлен позже через setCameraParams).
     */
    explicit BaslerSettingsForm(QWidget *parent = nullptr);

    /// Деструктор.
    ~BaslerSettingsForm();

    /**
     * @brief Установить заголовок группы (групповой рамки).
     * @param name Новый заголовок.
     */
    void setFormName(QString name);

    /**
     * @brief Заполнить все виджеты значениями из структуры параметров.
     * @param params Структура с текущими параметрами камеры.
     *
     * Также сохраняет роль камеры (isMaster) в поле m_isMaster и вызывает
     * connectChangingSignals() для подключения сигналов виджетов.
     */
    void setCameraParams(const BaslerCameraParams &params);

    /// @name Сеттеры для отдельных параметров
    /// @{
    void setExposure(double value);
    void setGain(double value);
    /**
     * @brief Установить формат пикселя.
     * @param pixelFormat Числовое значение из перечисления EPixelType (например, PixelType_Mono8).
     */
    void setPixelFormat(int pixelFormat);
    void setAcquisitionFrameRate(double fps);
    void setWidth(int width);
    void setHeight(int height);
    void setOffsetX(int offsetX);
    void setOffsetY(int offsetY);
    void setBinningHorizontal(int bin);
    void setBinningVertical(int bin);
    void setBinningHorizontalMode(BinningHorizontalModeEnums mode);
    void setBinningVerticalMode(BinningVerticalModeEnums mode);
    /// @}

public slots:
    void updateValueInGui(bool isMaster, BaslerConstants::SettingTypes settingType, QVariant value);

signals:
    /**
     * @brief Сигнал, излучаемый при изменении любого параметра в форме.
     * @param isMaster true, если форма принадлежит мастер-камере (HS), false – слейву (OC).
     * @param settingType Тип изменённой настройки (значение из BaslerConstants::SettingTypes).
     * @param value Новое значение параметра, упакованное в QVariant.
     */
    void settingsWereChanged(bool isMaster, BaslerConstants::SettingTypes settingType, QVariant value);

private:
    /**
     * @brief Подключить сигналы виджетов к единому сигналу settingsWereChanged.
     *
     * Вызывается один раз после установки параметров через setCameraParams.
     * В лямбда-функциях формируется соответствующий тип настройки и значение.
     */
    void connectChangingSignals();

    Ui::BaslerSettingsForm *ui;     //!< Указатель на сгенерированный Qt Designer UI.
    bool m_isMaster;                //!< Флаг роли камеры (устанавливается в setCameraParams).
};

#endif // BASLERSETTINGSFORM_H
