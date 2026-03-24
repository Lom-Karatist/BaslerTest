#ifndef BASLERWINDOW_H
#define BASLERWINDOW_H

#include <QMainWindow>
#include <Basler/CameraManager.h>
#include <QCloseEvent>
#include <BaslerSettingsForm.h>
#include <QButtonGroup>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class BaslerWindow; }
QT_END_NAMESPACE

class BaslerWindow : public QMainWindow
{
    Q_OBJECT

public:
    BaslerWindow(QWidget *parent = nullptr);
    ~BaslerWindow();
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButtonStartStop_clicked();
    void onManagerReady();
    void onError(const QString& msg);
    void updateMasterImage(const QImage& img);
    void updateSlaveImage(const QImage& img);

    void on_pushButtonOpenFolderSaving_clicked();

    void on_pushButtonSaving_clicked();

    void on_pushButtonOpenDataFolder_clicked();

private:
    /**
     * @brief setupProject   Function that setups project
     */
    void setupProject();

    void initCameraManager();

    void setupGui();

    void setupSettingBoxes(BaslerSettingsForm *form, QString formName, BaslerCameraParams params);

    bool eventFilter(QObject *watched, QEvent *event) override;

    QRect getImageRect(const QSize &labelSize, const QSize &imageSize);


    Ui::BaslerWindow *ui;
    QString m_title;
    QSettings *m_settings;      //!< Settings
    QButtonGroup *m_saveFormatGroup;

    QLabel *m_masterOverlay;
    QImage m_currentMasterImage;

    CameraManager *m_cameraManager;
    bool m_isRunning;
};
#endif // BASLERWINDOW_H
