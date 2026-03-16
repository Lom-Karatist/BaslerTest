#include "BaslerWindow.h"
#include "ui_BaslerWindow.h"
#include <QPixmap>
#include <QImage>
#include <QSize>

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
        m_camerasCount = cameras.size();
        ui->labelDevicesCount->setText(QString::number(cameras.size()));
    }
}

BaslerWindow::~BaslerWindow()
{
    delete ui;
}


void BaslerWindow::on_pushButtonUpdate_clicked()
{
    if (!m_cameraManager || !m_cameraManager->isInitialized()) {
        qDebug() << "Camera manager not initialized";
        return;
    }

    // Захватываем кадры (если камер достаточно)
    QImage imgHS, imgOC;
    if (m_camerasCount > 0) {
        imgHS = m_cameraManager->grabFrame(0);   // гиперспектральная
    }
    if (m_camerasCount > 1) {
        imgOC = m_cameraManager->grabFrame(1);   // обзорная
    }

    // Отображение в labelHS
    if (!imgHS.isNull()) {
        QSize labelSize = ui->labelHS->size();
        QPixmap pix = QPixmap::fromImage(imgHS);
        if (labelSize.width() > 0 && labelSize.height() > 0) {
            pix = pix.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        ui->labelHS->setPixmap(pix);
        ui->labelHS->setAlignment(Qt::AlignCenter);
    } else {
        ui->labelHS->setText("Нет изображения");
        ui->labelHS->setAlignment(Qt::AlignCenter);
    }

    // Отображение в labelOC
    if (!imgOC.isNull()) {
        QSize labelSize = ui->labelOC->size();
        QPixmap pix = QPixmap::fromImage(imgOC);
        if (labelSize.width() > 0 && labelSize.height() > 0) {
            pix = pix.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        ui->labelOC->setPixmap(pix);
        ui->labelOC->setAlignment(Qt::AlignCenter);
    } else {
        ui->labelOC->setText("Нет изображения");
        ui->labelOC->setAlignment(Qt::AlignCenter);
    }
}

