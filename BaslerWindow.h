#ifndef BASLERWINDOW_H
#define BASLERWINDOW_H

#include <QMainWindow>
#include <Basler/CameraManager.h>
#include <QCloseEvent>
#include <BaslerSettingsForm.h>
#include <QButtonGroup>
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class BaslerWindow; }
QT_END_NAMESPACE

/**
 * @brief Главное окно приложения.
 *
 * Отвечает за инициализацию интерфейса, управление камерами,
 * отображение видеопотоков, обработку пользовательских действий
 * (запуск/остановка, настройка параметров, сохранение данных)
 * и переключение тем оформления.
 */
class BaslerWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор.
     * @param parent Родительский виджет.
     */
    BaslerWindow(QWidget *parent = nullptr);
    ~BaslerWindow();

    /**
     * @brief Обработка события закрытия окна.
     * @param event Событие закрытия.
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * @brief Запуск/остановка захвата кадров.
     */
    void on_pushButtonStartStop_clicked();

    /**
     * @brief Камеры готовы к работе.
     */
    void onManagerReady();

    /**
     * @brief Обработка ошибок.
     * @param msg Текст ошибки.
     */
    void onError(const QString& msg);

    /**
     * @brief Обновление изображения гиперспектрометра.
     * @param img Полученное изображение.
     */
    void updateMasterImage(const QImage& img);

    /**
     * @brief Обновление изображения обзорной камеры.
     * @param img Полученное изображение.
     */
    void updateSlaveImage(const QImage& img);

    /**
     * @brief Выбор папки для сохранения.
     */
    void on_pushButtonOpenFolderSaving_clicked();

    /**
     * @brief Начало/остановка сохранения данных.
     */
    void on_pushButtonSaving_clicked();

    /**
     * @brief Открытие папки с сохранёнными данными.
     */
    void on_pushButtonOpenDataFolder_clicked();

    /**
     * @brief Переключение ночного режима.
     */
    void on_actionDarkMode_triggered();

private:
    // --- Методы инициализации ---
    /**
     * @brief Общая настройка проекта.
     */
    void setupProject();

    /**
     * @brief Создание и настройка менеджера камер.
     */
    void initCameraManager();

    /**
     * @brief Настройка графического интерфейса.
     */
    void setupGui();

    /**
     * @brief Настройка формы параметров камеры.
     * @param form Указатель на форму.
     * @param formName Заголовок формы.
     * @param params Параметры камеры для отображения.
     */
    void setupSettingBoxes(BaslerSettingsForm *form, QString formName, BaslerCameraParams params);

    // --- Вспомогательные методы GUI ---
    /**
     * @brief Фильтр событий для отслеживания мыши.
     * @param watched Объект, на котором произошло событие.
     * @param event Событие.
     * @return true, если событие обработано.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

    /**
     * @brief Расчёт области отображения изображения внутри лейбла с учётом пропорций.
     * @param labelSize Размер лейбла.
     * @param imageSize Размер исходного изображения.
     * @return Прямоугольник, в котором отображается изображение.
     */
    QRect getImageRect(const QSize &labelSize, const QSize &imageSize);

    /**
     * @brief Смена цвета кнопки старта/остановки.
     * @param isRunning true — камеры работают, false — остановлены.
     */
    void updateStartStopButtonStyle(bool isRunning);

    /**
     * @brief Применить тёмную тему оформления.
     */
    void applyDarkTheme();

    /**
     * @brief Применить светлую тему оформления.
     */
    void applyLightTheme();

    /**
     * @brief Запустить мигание индикатора записи.
     */
    void startRecordingBlink();

    /**
     * @brief Остановить мигание индикатора записи.
     */
    void stopRecordingBlink();

    // --- Члены класса ---
    Ui::BaslerWindow *ui;                //!< Указатель на сгенерированный UI.
    QString m_title;                     //!< Заголовок окна.
    QSettings *m_settings;               //!< Объект для работы с настройками приложения.
    QButtonGroup *m_saveFormatGroup;     //!< Группа радио-кнопок формата сохранения.

    QLabel *m_masterOverlay;             //!< Оверлей для отображения значений пикселей.
    QImage m_currentMasterImage;         //!< Текущее изображение гиперспектрометра (исходное).

    CameraManager *m_cameraManager;      //!< Менеджер камер.
    bool m_isRunning;                    //!< Флаг: идёт ли захват кадров.

    QLabel *m_recordingIndicator;        //!< Мигающий индикатор записи.
    QTimer *m_blinkTimer;                //!< Таймер для мигания индикатора.
};

#endif // BASLERWINDOW_H
