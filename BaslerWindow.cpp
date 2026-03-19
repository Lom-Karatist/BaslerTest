#include "BaslerWindow.h"
#include "ui_BaslerWindow.h"
#include <QPixmap>
#include <QImage>
#include <QSize>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>

BaslerWindow::BaslerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BaslerWindow)
    , m_cameraManager(nullptr)
    , m_isRunning(false)
{
    ui->setupUi(this);
    statusBar()->showMessage("Not started");

    m_cameraManager = new CameraManager(this);
    connect(m_cameraManager, &CameraManager::ready, this, &BaslerWindow::onManagerReady);
    connect(m_cameraManager, &CameraManager::errorOccurred, this, &BaslerWindow::onError);
    connect(m_cameraManager, &CameraManager::masterImageReady, this, &BaslerWindow::updateMasterImage);
    connect(m_cameraManager, &CameraManager::slaveImageReady, this, &BaslerWindow::updateSlaveImage);

    setupSettingBoxes(ui->widgetOCSettings, "Настройки обзорной камеры", m_cameraManager->ocParams());
    setupSettingBoxes(ui->widgetHSSettings, "Настройки камеры гиперспектрометра",  m_cameraManager->hsParams());
}

BaslerWindow::~BaslerWindow()
{
    delete ui;
}

void BaslerWindow::closeEvent(QCloseEvent *event)
{
    if (m_cameraManager) {
        m_cameraManager->stop();
    }
    event->accept();
}

void BaslerWindow::on_pushButtonStartStop_clicked()
{
    if (!m_isRunning) {
        statusBar()->showMessage("Starting cameras...");
        m_cameraManager->start();
        ui->pushButtonStartStop->setText("Stop cameras");
        m_isRunning = true;
    } else {
        m_cameraManager->pause();
        ui->pushButtonStartStop->setText("Resume cameras");
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

void BaslerWindow::setupSettingBoxes(BaslerSettingsForm *form, QString formName, BaslerCameraParams params)
{
    form->setFormName(formName);
    form->setCameraParams(params);
    connect(form, &BaslerSettingsForm::settingsWereChanged, m_cameraManager, &CameraManager::onSettingsChanged);
    connect(m_cameraManager, &CameraManager::forceParameterChanging, form, &BaslerSettingsForm::updateValueInGui);
}

void BaslerWindow::on_pushButtonOpenFolderSaving_clicked()
{
    QString checkPath;
    QDir existingDir(m_cameraManager->savingPath());
    if(existingDir.isReadable()){
        checkPath = QFileDialog::getExistingDirectory(this, tr("Выбор папки для сохранения"),
                                                      m_cameraManager->savingPath(), QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    }else{
        checkPath = QFileDialog::getExistingDirectory(this, tr("Выбор папки для сохранения"),
                                                      "/home", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    }
    QDir checkDir(checkPath);
    if(checkDir.isReadable()){
        m_cameraManager->setSavingPath(checkPath);
    }
}
