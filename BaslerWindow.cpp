#include "BaslerWindow.h"
#include "ui_BaslerWindow.h"

BaslerWindow::BaslerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BaslerWindow)
    , m_cameraManager(new CameraManager(this))
{
    ui->setupUi(this);

    if (!m_cameraManager->isInitialized()) {
        qDebug()<<"Pylon SDK не инициализирован.";
        return;
    }

    QList<CameraInfo> cameras = m_cameraManager->enumerateCameras();
    if (cameras.isEmpty()) {
        qDebug()<<"Камеры не найдены.";
        return;
    }else{
        qDebug() << "Найдено камер: " << cameras.size() << "\n\n";
    }
}

BaslerWindow::~BaslerWindow()
{
    delete ui;
}

