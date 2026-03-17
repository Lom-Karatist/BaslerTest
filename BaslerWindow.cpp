#include "BaslerWindow.h"
#include "ui_BaslerWindow.h"
#include <QPixmap>
#include <QImage>
#include <QSize>
#include <QMessageBox>

BaslerWindow::BaslerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BaslerWindow)
    , m_cameraManager(nullptr)
    , m_isRunning(false)
{
    ui->setupUi(this);
    statusBar()->showMessage("Not started");

    BaslerCameraParams masterParams;
    masterParams.isMaster = true;
    masterParams.exposureTimeAbs = 10000.0; // микросекунды
    masterParams.gainRaw = 0.0;
    masterParams.width = 640;
    masterParams.height = 480;

    BaslerCameraParams slaveParams;
    slaveParams.isMaster = false;
    slaveParams.exposureTimeAbs = 10000.0;
    slaveParams.gainRaw = 0.0;
    slaveParams.width = 640;
    slaveParams.height = 480;

    m_cameraManager = new CameraManager(masterParams, slaveParams, this);
    connect(m_cameraManager, &CameraManager::ready, this, &BaslerWindow::onManagerReady);
    connect(m_cameraManager, &CameraManager::errorOccurred, this, &BaslerWindow::onError);
    connect(m_cameraManager, &CameraManager::masterImageReceived, this, &BaslerWindow::updateMasterImage);
    connect(m_cameraManager, &CameraManager::slaveImageReceived, this, &BaslerWindow::updateSlaveImage);
}

BaslerWindow::~BaslerWindow()
{
    if (m_cameraManager) {
        m_cameraManager->stop();
    }
    delete ui;
}


void BaslerWindow::on_pushButtonStartStop_clicked()
{
    if (!m_isRunning) {
        statusBar()->showMessage("Starting cameras...");
        m_cameraManager->start();
        ui->pushButtonStartStop->setText("Stop cameras");
        m_isRunning = true;
    } else {
        m_cameraManager->stop();
        ui->pushButtonStartStop->setText("Start cameras");
        m_isRunning = false;
        statusBar()->showMessage("Stopped");
    }
}

void BaslerWindow::onManagerReady()
{
    statusBar()->showMessage("Cameras ready and grabbing", 3000);
}

void BaslerWindow::onError(const QString& msg)
{
    QMessageBox::critical(this, "Camera Error", msg);
    statusBar()->showMessage("Error: " + msg);
    qDebug()<<"Error: " + msg;
    ui->pushButtonStartStop->setText("Start cameras");
    m_isRunning = false;
}

void BaslerWindow::updateMasterImage(const QImage& img)
{
    // Масштабируем изображение под размер лейбла с сохранением пропорций
    QPixmap pix = QPixmap::fromImage(img);
    ui->labelHS->setPixmap(pix.scaled(ui->labelHS->size(),
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
}

void BaslerWindow::updateSlaveImage(const QImage& img)
{
    QPixmap pix = QPixmap::fromImage(img);
    ui->labelOC->setPixmap(pix.scaled(ui->labelOC->size(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
}
