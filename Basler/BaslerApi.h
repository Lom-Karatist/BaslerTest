#ifndef BASLERAPI_H
#define BASLERAPI_H

#include <QRunnable>
#include <QObject>
#include <QImage>
#include <QElapsedTimer>
#include <QMutex>
#include "Types.h"
#include "Commands/ParameterCommand.h"
#include "Commands/SetBinningHorizontalCommand.h"
#include "Commands/SetBinningVerticalCommand.h"
#include "Commands/SetExposureCommand.h"
#include "Commands/SetFramerateCommand.h"
#include "Commands/SetHeightCommand.h"
#include "Commands/SetWidthCommand.h"
#include "Commands/SetOffsetXCommand.h"
#include "Commands/SetOffsetYCommand.h"
#include "Commands/SetGainCommand.h"
#include "Commands/SetPixelFormatCommand.h"
#include "Commands/SetBinningHorizontalModeCommand.h"
#include "Commands/SetBinningVerticalModeCommand.h"

/**
 * @brief Класс для управления камерой Basler в отдельном потоке.
 *
 * Наследует QObject для работы с сигналами/слотами и QRunnable для выполнения в QThreadPool.
 * Обеспечивает инициализацию камеры, настройку параметров, поддержку режима master-slave,
 * асинхронный захват изображений и передачу данных через сигналы.
 *
 * Поток выполняет бесконечный цикл, управляемый атомарными флагами, что позволяет
 * приостанавливать и возобновлять захват без перезапуска потока.
 */
class BaslerApi : public QObject, public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор.
     * @param isMaster Роль камеры: true — мастер, false — слейв.
     * @param params Структура с параметрами камеры.
     * @param parent Родительский QObject.
     */
    explicit BaslerApi(bool isMaster, const BaslerCameraParams& params, QObject *parent = nullptr, bool isMasterSlaveNeeded = true);

    /// Деструктор. Освобождает ресурсы камеры и Pylon (если не было глобальной инициализации).
    ~BaslerApi();

    /**
     * @brief Запуск выполнения в потоке (реализация QRunnable).
     *
     * Содержит основной цикл жизни потока: инициализация, настройка, бесконечный цикл
     * с проверкой флагов m_isActive и m_isGrabbing, захват и отправка данных.
     */
    void run() override;

    /**
     * @brief Начать или возобновить захват кадров.
     *
     * Атомарно устанавливает флаг m_isGrabbing в true.
     * Если камера была на паузе, запускает захват.
     */
    void startGrabbing();

    /**
     * @brief Приостановить захват кадров.
     *
     * Атомарно устанавливает флаг m_isGrabbing в false.
     * Если камера была в процессе захвата, останавливает его.
     */
    void pauseGrabbing();

    /**
     * @brief Полное завершение работы потока.
     *
     * Устанавливает флаг m_isActive в false, что приводит к выходу из цикла run().
     * После этого поток завершается, камера закрывается.
     */
    void stopGrabbing();

    /**
     * @brief Проверка, выполняется ли захват в данный момент.
     * @return true, если флаг m_isGrabbing установлен.
     */
    bool isGrabbing() const { return m_isGrabbing; }

    /**
     * @brief Проверка, активен ли поток (не завершён).
     * @return true, если флаг m_isActive установлен.
     */
    bool isActive() const { return m_isActive.load(); }

    /**
     * @brief Проверка, успешно ли подключена камера.
     * @return true, если камера подключена.
     */
    bool isConnected() const { return m_isConnected; }

    /**
     * @brief Применить изменение усиления (Gain).
     * @param value Новое значение усиления.
     */
    void applyGainChanging(double value);

    /**
     * @brief Применить изменение формата пикселя.
     * @param value Новый формат пикселя (значение из EPixelType).
     */
    void applyPixelFormatChanging(int value);

    /**
     * @brief Применить изменение режима биннинга по горизонтали.
     * @param mode Новый режим (Sum или Average).
     */
    void applyBinningHorizontalModeChanging(BinningHorizontalModeEnums mode);

    /**
     * @brief Применить изменение режима биннинга по вертикали.
     * @param mode Новый режим (Sum или Average).
     */
    void applyBinningVerticalModeChanging(BinningVerticalModeEnums mode);

    /**
     * @brief Применить изменение ширины изображения.
     * @param value Новая ширина.
     */
    void applyWidthChanging(int value);

    /**
     * @brief Применить изменение высоты изображения.
     * @param value Новая высота.
     */
    void applyHeightChanging(int value);

    /**
     * @brief Применить изменение смещения по X.
     * @param value Новое смещение.
     */
    void applyOffsetXChanging(int value);

    /**
     * @brief Применить изменение смещения по Y.
     * @param value Новое смещение.
     */
    void applyOffsetYChanging(int value);

    /**
     * @brief Применить изменение коэффициента биннинга по горизонтали.
     * @param value Новый коэффициент (1–4).
     */
    void applyBinningHorizontalChanging(int value);

    /**
     * @brief Применить изменение коэффициента биннинга по вертикали.
     * @param value Новый коэффициент (1–4).
     */
    void applyBinningVerticalChanging(int value);

    /**
     * @brief Применить изменение экспозиции.
     * @param exposureMs Новая экспозиция в миллисекундах.
     */
    void applyExposureChanging(double exposureMs);

    /**
     * @brief Применить изменение частоты кадров.
     * @param fps Новая частота кадров (кадров/с).
     */
    void applyFramerateChanging(double fps);

    /**
     * @brief Передать список команд для выполнения в потоке камеры.
     * @param commands Список команд (владение передаётся вызываемому методу).
     *
     * Команды будут выполнены асинхронно в потоке run() с остановкой захвата.
     */
    void submitCommands(std::vector<std::unique_ptr<ParameterCommand>> commands);

signals:
    /**
     * @brief Сигнал о завершении попытки подключения.
     * @param success true, если подключение успешно.
     */
    void connectionComplete(bool success);

    /**
     * @brief Сигнал об ошибке.
     * @param error Текст ошибки.
     */
    void sendErrorMessage(const QString& error);

    /**
     * @brief Сигнал с сырыми данными захваченного кадра.
     * @param data Байтовый массив с данными изображения.
     * @param width Ширина изображения.
     * @param height Высота изображения.
     * @param pixelFormat Формат пикселей (значение из EPixelType).
     */
    void rawDataReceived(const QByteArray& data, int width, int height, int pixelFormat);

private:
    /**
     * @brief Инициализация подключения к камере по серийному номеру.
     * @return true, если камера успешно открыта.
     */
    bool initializeCamera();

    /**
     * @brief Настройка основных параметров камеры (экспозиция, размеры, формат и т.д.).
     *
     * Использует значения из m_params. Проверяет доступность каждого параметра.
     */
    void setupCameraFeatures();

    /** @brief Конфигурация режима master-slave.
     *
     * Для мастера: отключает внешний триггер, включает генерацию кадров, настраивает выходной сигнал на Line3.
     * Для слейва: включает внешний триггер на Line4, отключает собственную генерацию кадров.
     */
    void configureMasterSlave();

    /**
     * @brief processRawData    Промежуточная функция обработки данных с матрицы
     *
     * Преобразует данные с матриц сенсора для отправки через сигнал в менеджер камер.
     */
    void processRawData();

    /**
     * @brief Выполнить все накопленные команды из очереди.
     *
     * Останавливает захват, последовательно выполняет команды и возобновляет захват.
     */
    void applyPendingCommands();

    std::atomic<bool> m_isActive;   //!< Флаг активности потока. true — поток должен работать.
    std::atomic<bool> m_isGrabbing; //!< Флаг захвата. true — нужно захватывать и отправлять кадры.

    bool m_isConnected;             //!< Флаг успешного подключения камеры.
    bool m_isMaster;                //!< Роль камеры (true — мастер).
    bool m_isMasterSlaveNeeded;     //!< Флаг необходимости настройки мастер/слейв

    BaslerCameraParams m_params;    //!< Структура с текущими параметрами камеры.

    CBaslerUniversalInstantCamera* m_camera;    //!< Указатель на объект камеры.
    CGrabResultPtr m_ptrGrabResult;             //!< Умный указатель на результат захвата.    

    std::vector<std::unique_ptr<ParameterCommand>> m_pendingCommands;   //!< Очередь ожидающих выполнения команд.
    std::atomic<bool> m_commandsPending{false};                         //!< Флаг наличия ожидающих команд.
    QMutex m_commandsMutex;                                             //!< Мьютекс для защиты доступа к очереди команд.
};

#endif // BASLERAPI_H
