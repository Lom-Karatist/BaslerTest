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
#include <QDesktopServices>

BaslerWindow::BaslerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BaslerWindow)
    , m_cameraManager(nullptr)
    , m_isRunning(false)
{
    setupProject();
    initCameraManager();
    setupGui();
}

BaslerWindow::~BaslerWindow()
{
    qDebug()<<"Window destructor";
    delete ui;
    qDebug()<<"Window destructor ok";
}

void BaslerWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<"---------------------------------\nWindow close event";
    if (m_cameraManager) {
        m_cameraManager->stop();
        delete m_cameraManager;
        m_cameraManager = nullptr;
    }
    event->accept();
    qDebug()<<"Window close event accepted";
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
    m_currentMasterImage = img;

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

bool BaslerWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->labelHS && ui->actionShowHsValues->isChecked())
    {
        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (!m_currentMasterImage.isNull())
            {
                QPoint pos = mouseEvent->pos();
                QRect imageRect = getImageRect(ui->labelHS->size(), m_currentMasterImage.size());
                if (imageRect.contains(pos))
                {
                    int x = (pos.x() - imageRect.x()) * m_currentMasterImage.width() / imageRect.width();
                    int y = (pos.y() - imageRect.y()) * m_currentMasterImage.height() / imageRect.height();

                    if (x >= 0 && x < m_currentMasterImage.width() && y >= 0 && y < m_currentMasterImage.height())
                    {
                        QRgb pixel = m_currentMasterImage.pixel(x, y);
                        int gray = qGray(pixel);
                        m_masterOverlay->setText(QString("X=%1 Y=%2 Value=%3").arg(x).arg(y).arg(gray));
                        m_masterOverlay->adjustSize();
                        m_masterOverlay->move(ui->labelHS->width() - m_masterOverlay->width() - 5, 5);
                        m_masterOverlay->show();
                    }
                    else
                        m_masterOverlay->hide();
                }
                else
                    m_masterOverlay->hide();
            }
            return true;
        }
        else if (event->type() == QEvent::Leave)
        {
            m_masterOverlay->hide();
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

QRect BaslerWindow::getImageRect(const QSize &labelSize, const QSize &imageSize)
{
    if (imageSize.isEmpty()) return QRect();

    QRect rect;
    double ratioLabel = (double)labelSize.width() / labelSize.height();
    double ratioImage = (double)imageSize.width() / imageSize.height();

    if (ratioImage > ratioLabel)
    {
        int newWidth = labelSize.width();
        int newHeight = newWidth / ratioImage;
        rect.setX(0);
        rect.setY((labelSize.height() - newHeight) / 2);
        rect.setWidth(newWidth);
        rect.setHeight(newHeight);
    }
    else
    {
        int newHeight = labelSize.height();
        int newWidth = newHeight * ratioImage;
        rect.setX((labelSize.width() - newWidth) / 2);
        rect.setY(0);
        rect.setWidth(newWidth);
        rect.setHeight(newHeight);
    }
    return rect;
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
        ui->pushButtonSaving->setText("Остановить запись");
        m_cameraManager->setIsNeedToSave(true, ui->actionSaveHS->isChecked(), ui->actionSaveOC->isChecked());
    }else{
        ui->pushButtonSaving->setText("Начать запись");
        m_cameraManager->setIsNeedToSave(false,  ui->actionSaveHS->isChecked(), ui->actionSaveOC->isChecked());
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

void BaslerWindow::initCameraManager()
{
    m_cameraManager = new CameraManager(this, m_settings->value("Cameras/isMasterSlaveNeeded").toBool());
    connect(m_cameraManager, &CameraManager::ready, this, &BaslerWindow::onManagerReady);
    connect(m_cameraManager, &CameraManager::errorOccurred, this, &BaslerWindow::onError);
    connect(m_cameraManager, &CameraManager::masterImageReady, this, &BaslerWindow::updateMasterImage);
    connect(m_cameraManager, &CameraManager::slaveImageReady, this, &BaslerWindow::updateSlaveImage);
    m_cameraManager->setSavingPath(ui->lineEditSavingPath->text());
}

void BaslerWindow::setupGui()
{
    setupSettingBoxes(ui->widgetOCSettings, "Настройки обзорной камеры", m_cameraManager->ocParams());
    setupSettingBoxes(ui->widgetHSSettings, "Настройки камеры гиперспектрометра",  m_cameraManager->hsParams());

    m_saveFormatGroup = new QButtonGroup(this);
    m_saveFormatGroup->addButton(ui->radioButtonSaveBmp, 0);
    m_saveFormatGroup->addButton(ui->radioButtonSaveBinary, 1);
    connect(m_saveFormatGroup, &QButtonGroup::idClicked, m_cameraManager, &CameraManager::onSavingModeChanged);

    m_masterOverlay = new QLabel(ui->labelHS);
    m_masterOverlay->setAlignment(Qt::AlignTop | Qt::AlignRight);
    m_masterOverlay->setStyleSheet("QLabel { background-color: rgba(0,0,0,128); color: white; padding: 2px; border-radius: 3px; }");
    m_masterOverlay->hide();

    ui->labelHS->setMouseTracking(true);
    ui->labelHS->installEventFilter(this);
}

void BaslerWindow::on_pushButtonOpenDataFolder_clicked()
{
    QDir dir;
    dir.setPath(m_settings->value("Pathes/saving").toString());
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absolutePath()));
}
