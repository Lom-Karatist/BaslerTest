#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QThreadPool>
#include <QMutex>
#include <QAtomicInt>
#include "BaslerApi.h"
#include "BaslerSettings.h"
#include "SavingModule.h"

/**
 * @brief Класс-менеджер, координирующий работу двух камер (мастер и слейв).
 *
 * Отвечает за инициализацию, запуск/паузу/остановку захвата,
 * обработку изменений параметров (с формированием команд),
 * передачу сырых данных в модуль сохранения и визуализацию.
 */
class CameraManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор.
     * @param parent Родительский QObject.
     * @param isMasterSlaveNeeded Флаг: использовать ли аппаратную синхронизацию master-slave.
     */
    explicit CameraManager(QObject *parent = nullptr, bool isMasterSlaveNeeded = true);
    ~CameraManager();

    /**
     * @brief Запустить захват кадров (возобновить после паузы).
     */
    void start();

    /**
     * @brief Приостановить захват кадров.
     */
    void pause();

    /**
     * @brief Полная остановка работы камер и освобождение ресурсов.
     */
    void stop();

    /**
     * @brief Проверка готовности обеих камер.
     * @return true, если обе камеры подключены и инициализированы.
     */
    bool isReady() const { return m_ready; }

    /**
     * @brief Установить путь для сохранения файлов.
     * @param path Новый путь.
     */
    void setSavingPath(const QString path);

    /**
     * @brief Получить текущие параметры гиперспектрометра.
     * @return Константная ссылка на структуру параметров.
     */
    const BaslerCameraParams &hsParams() const;

    /**
     * @brief Установить параметры гиперспектрометра.
     * @param newHsParams Новые параметры.
     */
    void setHsParams(const BaslerCameraParams &newHsParams);

    /**
     * @brief Получить текущие параметры обзорной камеры.
     * @return Константная ссылка на структуру параметров.
     */
    const BaslerCameraParams &ocParams() const;

    /**
     * @brief Установить параметры обзорной камеры.
     * @param newOcParams Новые параметры.
     */
    void setOcParams(const BaslerCameraParams &newOcParams);

    /**
     * @brief Включить/выключить сохранение данных.
     * @param newIsNeedToSave Общий флаг сохранения.
     * @param isNeedToSaveHS Сохранять данные гиперспектрометра.
     * @param isNeedToSaveOC Сохранять данные обзорной камеры.
     */
    void setIsNeedToSave(bool newIsNeedToSave, bool isNeedToSaveHS, bool isNeedToSaveOC);

signals:
    /**
     * @brief Сигнал о готовности обеих камер.
     */
    void ready();

    /**
     * @brief Сигнал об ошибке.
     * @param message Текст ошибки.
     */
    void errorOccurred(const QString& message);

    /**
     * @brief Сигнал с изображением гиперспектрометра.
     * @param image QImage для отображения.
     */
    void masterImageReady(const QImage& image);

    /**
     * @brief Сигнал с изображением обзорной камеры.
     * @param image QImage для отображения.
     */
    void slaveImageReady(const QImage& image);

    /**
     * @brief Сигнал с сырыми данными гиперспектрометра.
     * @param data Байтовый массив.
     * @param w Ширина.
     * @param h Высота.
     */
    void masterRawData(const QByteArray& data, int w, int h);

    /**
     * @brief Сигнал с сырыми данными обзорной камеры.
     * @param data Байтовый массив.
     * @param w Ширина.
     * @param h Высота.
     */
    void slaveRawData(const QByteArray& data, int w, int h);

    /**
     * @brief Сигнал для принудительного обновления GUI при изменении параметра.
     * @param isMaster Роль камеры (true – мастер, false – слейв).
     * @param settingType Тип параметра.
     * @param value Новое значение.
     */
    void forceParameterChanging(bool isMaster, BaslerConstants::SettingTypes settingType, QVariant value);

public slots:
    /**
     * @brief Слот для обработки изменения параметра из GUI.
     * @param isMaster Роль камеры.
     * @param type Тип параметра.
     * @param value Новое значение.
     */
    void onSettingsChanged(bool isMaster, BaslerConstants::SettingTypes type, QVariant value);

    /**
     * @brief Слот для смены формата сохранения.
     * @param savingFormat Код формата (0 – BMP, 1 – бинарный).
     */
    void onSavingModeChanged(const int savingFormat);

private slots:
    /**
     * @brief Обработка успешного подключения мастер-камеры.
     * @param success true при успешном подключении.
     */
    void onMasterConnected(bool success);

    /**
     * @brief Обработка успешного подключения слейв-камеры.
     * @param success true при успешном подключении.
     */
    void onSlaveConnected(bool success);

    /**
     * @brief Обработка ошибок мастер-камеры.
     * @param err Текст ошибки.
     */
    void onMasterError(const QString& err);

    /**
     * @brief Обработка ошибок слейв-камеры.
     * @param err Текст ошибки.
     */
    void onSlaveError(const QString& err);

    /**
     * @brief Приём сырых данных от мастер-камеры.
     * @param data Байтовый массив с данными.
     * @param w Ширина изображения.
     * @param h Высота изображения.
     * @param pixelFormat Формат пикселя.
     */
    void onMasterRawData(const QByteArray& data, int w, int h, int pixelFormat);

    /**
     * @brief Приём сырых данных от слейв-камеры.
     * @param data Байтовый массив с данными.
     * @param w Ширина изображения.
     * @param h Высота изображения.
     * @param pixelFormat Формат пикселя.
     */
    void onSlaveRawData(const QByteArray& data, int w, int h, int pixelFormat);

private:
    // --- Методы обработки и сохранения настроек ---
    /**
     * @brief Сохранить изменённые настройки и сформировать команды для камеры.
     * @param baslerSettingsObject Объект для сохранения в INI.
     * @param cameraParams Структура параметров камеры.
     * @param type Тип изменённого параметра.
     * @param value Новое значение.
     */
    void saveChangedSettings(BaslerSettings &baslerSettingsObject, BaslerCameraParams &cameraParams,
                             BaslerConstants::SettingTypes type, QVariant value);

    /**
     * @brief Обработать изменение экспозиции или частоты кадров с учётом взаимосвязи.
     * @param cameraParams Структура параметров камеры.
     * @param type Тип изменённого параметра.
     * @param value Новое значение.
     * @param commands Вектор для добавления команд.
     */
    void processExposureAndFramerateChanging(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value,
                                             std::vector<std::unique_ptr<ParameterCommand>> &commands);

    /**
     * @brief Обработать изменение параметров по оси X (ширина, смещение X, биннинг X).
     * @param cameraParams Структура параметров камеры.
     * @param type Тип изменённого параметра.
     * @param value Новое значение.
     * @param commands Вектор для добавления команд.
     */
    void processRoiAndBinningX(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value,
                               std::vector<std::unique_ptr<ParameterCommand>> &commands);

    /**
     * @brief Обработать изменение параметров по оси Y (высота, смещение Y, биннинг Y).
     * @param cameraParams Структура параметров камеры.
     * @param type Тип изменённого параметра.
     * @param value Новое значение.
     * @param commands Вектор для добавления команд.
     */
    void processRoiAndBinningY(BaslerCameraParams &cameraParams, BaslerConstants::SettingTypes type, QVariant value,
                               std::vector<std::unique_ptr<ParameterCommand>> &commands);

    /**
     * @brief Пересчитать значения для одной оси (размер, смещение, биннинг) и сформировать порядок команд.
     * @param size Текущий размер (width/height).
     * @param offset Текущее смещение (offsetX/offsetY).
     * @param binning Текущий коэффициент биннинга.
     * @param changedType Тип изменённого параметра.
     * @param value Новое значение.
     * @param maxSize Максимальный физический размер.
     * @param commands Список типов команд в нужном порядке.
     */
    void calcRoiOnAxe(int &size, int &offset, int &binning,
                      BaslerConstants::SettingTypes changedType, const QVariant &value, int maxSize,
                      QList<BaslerConstants::SettingTypes> &commands);

    /**
     * @brief Вычислить максимальный выходной размер с учётом биннинга.
     * @param maxSize Максимальный физический размер.
     * @param binning Коэффициент биннинга.
     * @return Максимальный размер выходного изображения.
     */
    inline int maxOutSize(int maxSize, int binning) { return maxSize / binning; }

    // --- Вспомогательные методы для установки параметров ---
    /**
     * @brief Установить усиление (Gain) для указанной камеры.
     * @param isMaster Роль камеры.
     * @param value Значение усиления.
     */
    void setGain(bool isMaster, double value);

    /**
     * @brief Установить формат пикселя для указанной камеры.
     * @param isMaster Роль камеры.
     * @param value Код формата пикселя.
     */
    void setPixelFormat(bool isMaster, int value);

    /**
     * @brief Установить режим биннинга по горизонтали.
     * @param isMaster Роль камеры.
     * @param mode Режим (Sum/Average).
     */
    void setBinningHorizontalMode(bool isMaster, BinningHorizontalModeEnums mode);

    /**
     * @brief Установить режим биннинга по вертикали.
     * @param isMaster Роль камеры.
     * @param mode Режим (Sum/Average).
     */
    void setBinningVerticalMode(bool isMaster, BinningVerticalModeEnums mode);

    /**
     * @brief Передать список команд соответствующей камере.
     * @param isMaster Роль камеры.
     * @param commands Вектор команд (владение передаётся).
     */
    void submitCommands(bool isMaster, std::vector<std::unique_ptr<ParameterCommand>> commands);

    // --- Объекты управления камерами ---
    BaslerApi* m_master;                 //!< Управление мастер-камерой (гиперспектрометр).
    BaslerApi* m_slave;                  //!< Управление слейв-камерой (обзорная камера).
    BaslerCameraParams m_hsParams;       //!< Текущие параметры гиперспектрометра.
    BaslerCameraParams m_ocParams;       //!< Текущие параметры обзорной камеры.
    BaslerSettings m_masterSettings;     //!< Загрузчик/сохранитель настроек для мастера.
    BaslerSettings m_slaveSettings;      //!< Загрузчик/сохранитель настроек для слейва.

    // --- Состояние системы ---
    QAtomicInt m_connectedCount;         //!< Счётчик успешных подключений камер.
    bool m_ready;                        //!< Флаг готовности обеих камер.
    QMutex m_mutex;                      //!< Мьютекс для защиты m_ready.
    bool m_isImageNeeded;                //!< Флаг необходимости отправлять изображения в GUI.
    bool m_stopped;                      //!< Флаг, что остановка уже выполнена (защита от повторного вызова).

    // --- Модуль сохранения ---
    SavingModule m_savingModule;         //!< Модуль сохранения данных.
    QString m_frameTimeStamp;            //!< Временная метка для текущего кадра.
    bool m_isNeedToSaveHS;               //!< Флаг сохранения данных гиперспектрометра.
    bool m_isNeedToSaveOC;               //!< Флаг сохранения данных обзорной камеры.

    static const int MAX_WIDTH = 1936;   //!< Максимальная физическая ширина сенсора (пиксели).
    static const int MAX_HEIGHT = 1216;  //!< Максимальная физическая высота сенсора (пиксели).
};

#endif // CAMERAMANAGER_H
