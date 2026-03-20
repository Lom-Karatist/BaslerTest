#include "BaslerWindow.h"
#include "ui_BaslerWindow.h"
#include <QPixmap>
#include <QImage>
#include <QSize>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QStyleFactory>
#include "version.h"
#include <BaseTools/IniFileLoader.h>

BaslerWindow::BaslerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BaslerWindow)
    , m_cameraManager(nullptr)
    , m_isRunning(false)
{
    setupProject();

    m_cameraManager = new CameraManager(this);
    connect(m_cameraManager, &CameraManager::ready, this, &BaslerWindow::onManagerReady);
    connect(m_cameraManager, &CameraManager::errorOccurred, this, &BaslerWindow::onError);
    connect(m_cameraManager, &CameraManager::masterImageReady, this, &BaslerWindow::updateMasterImage);
    connect(m_cameraManager, &CameraManager::slaveImageReady, this, &BaslerWindow::updateSlaveImage);

    setupSettingBoxes(ui->widgetOCSettings, "Настройки обзорной камеры", m_cameraManager->ocParams());
    setupSettingBoxes(ui->widgetHSSettings, "Настройки камеры гиперспектрометра",  m_cameraManager->hsParams());

    m_saveFormatGroup = new QButtonGroup(this);
    m_saveFormatGroup->addButton(ui->radioButtonSaveBmp, 0);
    m_saveFormatGroup->addButton(ui->radioButtonSaveBinary, 1);
    connect(m_saveFormatGroup, &QButtonGroup::idClicked, m_cameraManager, &CameraManager::onSavingModeChanged);
    m_cameraManager->setSavingPath(ui->lineEditSavingPath->text());
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
    QDir existingDir(ui->lineEditSavingPath->text());
    if(existingDir.isReadable()){
        checkPath = QFileDialog::getExistingDirectory(this, tr("Выбор папки для сохранения"),
                                                      ui->lineEditSavingPath->text(), QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    }else{
        checkPath = QFileDialog::getExistingDirectory(this, tr("Выбор папки для сохранения"),
                                                      "/home", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    }
    QDir checkDir(checkPath);
    if(checkDir.isReadable()){
        ui->lineEditSavingPath->setText(checkPath);
        m_settings->setValue("Pathes/saving", checkPath);
        m_cameraManager->setSavingPath(checkPath);
    }
}

void BaslerWindow::on_pushButtonSaving_clicked()
{
    if(ui->pushButtonSaving->isChecked()){        
        qDebug()<<"******START!";
        ui->pushButtonSaving->setText("Остановить запись");
        m_cameraManager->setIsNeedToSave(true);
    }else{
        qDebug()<<"******STOP!";
        ui->pushButtonSaving->setText("Начать запись");
        m_cameraManager->setIsNeedToSave(false);
    }
}

void BaslerWindow::setupProject()
{
    m_title.append(VER_PRODUCTNAME_STR).append(" v_").append(VER_FILEVERSION_STR);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    ui->setupUi(this);
    this->setWindowTitle(m_title + ". Пожалуйста, подождите, идет запуск ПО...");
    m_settings = IniFileLoader::createSettingsObject(VER_PRODUCTNAME_STR);
    ui->lineEditSavingPath->setText(m_settings->value("Pathes/saving").toString());
    statusBar()->showMessage("Not started");
}

