#ifndef BASLERSETTINGS_H
#define BASLERSETTINGS_H

#include <QObject>
#include "BaslerApi.h"
#include <QSettings>

/**
 * @class BaslerSettings
 * @brief Класс для загрузки и сохранения параметров камеры в INI-файл.
 *
 * Инкапсулирует работу с QSettings для конкретного файла настроек.
 * Предоставляет методы для чтения параметров в структуру BaslerCameraParams
 * и записи обратно в файл. Используется в CameraManager для хранения
 * конфигурации мастер-камеры (HS) и слейв-камеры (OC).
 */
class BaslerSettings : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор.
     * @param parent Родительский QObject.
     * @param fileName Полный путь к INI-файлу настроек.
     */
    explicit BaslerSettings(QObject *parent = nullptr, QString fileName = "");    

    /// Деструктор. Освобождает внутренний объект QSettings.
    ~BaslerSettings();

    /**
     * @brief Загрузить параметры из INI-файла.
     * @return Структура BaslerCameraParams, заполненная значениями из файла.
     *         Если файл отсутствует или ключи не найдены, используются значения по умолчанию.
     */
    BaslerCameraParams loadParamsFromFile();

    /**
     * @brief Сохранить параметры камеры в INI-файл.
     * @param cameraParams Структура с актуальными параметрами.
     *
     * Запись производится в группу [Camera]. После вызова данные синхронизируются с диском.
     */
    void saveParams(const BaslerCameraParams &cameraParams);

    /**
     * @brief Получить указатель на внутренний объект QSettings.
     * @return Указатель на QSettings (может быть использован для прямого доступа, если требуется).
     */
    const QSettings *settings() const;

private:
    /**
     * @brief Создать объект QSettings для указанного INI-файла.
     * @param iniFileName Имя INI-файла.
     * @return Указатель на новый QSettings.
     *
     * Статический метод, проверяет существование файла и выводит отладочную информацию.
     */
    static QSettings *createSettingsObject(QString iniFileName);

    QSettings *m_settings;  //!< Внутренний объект для работы с INI-файлом.
};

#endif // BASLERSETTINGS_H
