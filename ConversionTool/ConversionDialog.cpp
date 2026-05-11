#include "ConversionDialog.h"

#include <QDir>
#include <QFileDialog>

#include "ui_ConversionDialog.h"

ConversionDialog::ConversionDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConversionDialog) {
    ui->setupUi(this);
    setWindowTitle("Конвертация данных");

    ui->comboBoxOutputFormat->addItem("ENVI (BSQ)", "ENVI_BSQ");
    ui->comboBoxOutputFormat->addItem("ENVI (BIL)", "ENVI_BIL");
    ui->comboBoxOutputFormat->addItem("ENVI (BIP)", "ENVI_BIP");
    ui->comboBoxOutputFormat->addItem("Сигнатура", "SIG");
}

ConversionDialog::~ConversionDialog() { delete ui; }

void ConversionDialog::on_pushButtonExperimentDir_clicked() {
    QString checkPath;
    QDir existingDir(ui->lineEditExperimentDir->text());
    if (existingDir.isReadable()) {
        checkPath = QFileDialog::getExistingDirectory(
            this, tr("Выбор папки для сохранения"),
            ui->lineEditExperimentDir->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    } else {
        checkPath = QFileDialog::getExistingDirectory(
            this, tr("Выбор папки для сохранения"), "/home",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    QDir checkDir(checkPath);
    if (checkDir.isReadable()) {
        ui->lineEditExperimentDir->setText(checkPath);
        //        m_settings->setValue("Pathes/saving", checkPath);
    }
}

void ConversionDialog::on_pushButtonCalibrationFile_clicked() {
    QString checkPath;
    QDir existingDir(ui->lineEditCalibrationPath->text());
    if (existingDir.isReadable()) {
        checkPath = QFileDialog::getExistingDirectory(
            this, tr("Выбор папки для сохранения"),
            ui->lineEditCalibrationPath->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    } else {
        checkPath = QFileDialog::getExistingDirectory(
            this, tr("Выбор папки для сохранения"), "/home",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    QDir checkDir(checkPath);
    if (checkDir.isReadable()) {
        ui->lineEditCalibrationPath->setText(checkPath);
        //        m_settings->setValue("Pathes/saving", checkPath);
    }
}

void ConversionDialog::on_pushButtonSavingDir_clicked() {
    QString checkPath;
    QDir existingDir(ui->lineEditSavingDir->text());
    if (existingDir.isReadable()) {
        checkPath = QFileDialog::getExistingDirectory(
            this, tr("Выбор папки для сохранения"),
            ui->lineEditSavingDir->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    } else {
        checkPath = QFileDialog::getExistingDirectory(
            this, tr("Выбор папки для сохранения"), "/home",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    QDir checkDir(checkPath);
    if (checkDir.isReadable()) {
        ui->lineEditSavingDir->setText(checkPath);
        //        m_settings->setValue("Pathes/saving", checkPath);
    }
}
