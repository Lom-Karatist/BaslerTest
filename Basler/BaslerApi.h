#ifndef BASLERAPI_H
#define BASLERAPI_H

#include <QRunnable>
#include <QObject>
#include <QImage>
#include <QElapsedTimer>
#include "Types.h"

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
    explicit BaslerApi(bool isMaster, const BaslerCameraParams& params, QObject *parent = nullptr);

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

    void setExposure(double value);
    void setGain(double value);
    void setAcquisitionFrameRate(double value);
    void setWidth(int value);
    void setHeight(int value);
    void setOffsetX(int value);
    void setOffsetY(int value);
    void setBinningHorizontal(int value);
    void setBinningVertical(int value);
    void setPixelFormat(int value);
    void setBinningHorizontalMode(BinningHorizontalModeEnums mode);
    void setBinningVerticalMode(BinningVerticalModeEnums mode);

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

    void applyPendingChanges();

    std::atomic<bool> m_isActive;   //!< Флаг активности потока. true — поток должен работать.
    std::atomic<bool> m_isGrabbing; //!< Флаг захвата. true — нужно захватывать и отправлять кадры.

    bool m_isConnected;             //!< Флаг успешного подключения камеры.
    bool m_isMaster;                //!< Роль камеры (true — мастер).

    BaslerCameraParams m_params;    //!< Структура с текущими параметрами камеры.

    CBaslerUniversalInstantCamera* m_camera;    //!< Указатель на объект камеры.
    CGrabResultPtr m_ptrGrabResult;             //!< Умный указатель на результат захвата.    

    std::atomic<double> m_requestedExposure;
    std::atomic<double> m_requestedGain;
    std::atomic<double> m_requestedFrameRate;
    std::atomic<int> m_requestedWidth;
    std::atomic<int> m_requestedHeight;
    std::atomic<int> m_requestedOffsetX;
    std::atomic<int> m_requestedOffsetY;
    std::atomic<int> m_requestedBinningH;
    std::atomic<int> m_requestedBinningV;
    std::atomic<int> m_requestedPixelFormat;
    std::atomic<int> m_requestedBinningHMode; // 0=Sum, 1=Average
    std::atomic<int> m_requestedBinningVMode;
    std::atomic<bool> m_reconfigureNeeded;    // флаг необходимости обновления
};

#endif // BASLERAPI_H
