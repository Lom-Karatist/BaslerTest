#ifndef BASLERWINDOW_H
#define BASLERWINDOW_H

#include <QMainWindow>
#include <BaslerAPI/CameraManager.h>

QT_BEGIN_NAMESPACE
namespace Ui { class BaslerWindow; }
QT_END_NAMESPACE

class BaslerWindow : public QMainWindow
{
    Q_OBJECT

public:
    BaslerWindow(QWidget *parent = nullptr);
    ~BaslerWindow();

private slots:
    void on_pushButtonStartStop_clicked();
    void onManagerReady();
    void onError(const QString& msg);
    void updateMasterImage(const QImage& img);
    void updateSlaveImage(const QImage& img);

private:
    Ui::BaslerWindow *ui;
    CameraManager *m_cameraManager;
    bool m_isRunning;
};
#endif // BASLERWINDOW_H
