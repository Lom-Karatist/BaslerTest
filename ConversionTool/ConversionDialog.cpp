#include "ConversionDialog.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include "ui_ConversionDialog.h"

ConversionDialog::ConversionDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ConversionDialog) {
    ui->setupUi(this);
    setWindowTitle("Конвертация данных");

    m_initting = true;
    m_settings = new QSettings(
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
            "/ConverterSettings.ini",
        QSettings::IniFormat, this);
    qDebug() << QStandardPaths::writableLocation(
                    QStandardPaths::AppConfigLocation) +
                    "/ConverterSettings.ini";
    ui->comboBoxOutputFormat->addItem("ENVI (BSQ)", "ENVI_BSQ");
    ui->comboBoxOutputFormat->addItem("ENVI (BIL)", "ENVI_BIL");
    ui->comboBoxOutputFormat->addItem("ENVI (BIP)", "ENVI_BIP");
    ui->comboBoxOutputFormat->addItem("Сигнатура", "SIG");

    qDebug() << "1";

    loadSettings();
    m_initting = false;

    m_workerThread = nullptr;
    m_worker = nullptr;
}

ConversionDialog::~ConversionDialog() {
    delete ui;
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void ConversionDialog::accept() {
    QString expDir = ui->lineEditExperimentDir->text();
    if (expDir.isEmpty() || !QDir(expDir).exists()) {
        QMessageBox::warning(this, "Ошибка",
                             "Укажите существующую папку с экспериментом");
        return;
    }

    // Запрещаем повторный запуск
    ui->buttonBox->setEnabled(false);
    ui->progressBar->setValue(0);

    // Создаём рабочий поток
    m_workerThread = new QThread(this);
    m_worker = new ConversionWorker();
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, [=]() {
        m_worker->runConversion(
            ui->lineEditExperimentDir->text(),
            ui->lineEditCalibrationPath->text(), ui->lineEditSavingDir->text(),
            ui->lineEditSavingFileName->text(),
            ui->comboBoxOutputFormat->currentData().toString(),
            ui->checkBoxParseDataCubes->isChecked(),
            ui->checkBoxAddGpsData->isChecked());
    });
    connect(m_worker, &ConversionWorker::progressUpdated, this,
            &ConversionDialog::onProgressUpdated);
    connect(m_worker, &ConversionWorker::finished, this,
            &ConversionDialog::onConversionFinished);
    connect(m_workerThread, &QThread::finished, m_worker,
            &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread,
            &QThread::deleteLater);

    m_workerThread->start();
}

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
        m_settings->setValue("Paths/experimentDir",
                             ui->lineEditExperimentDir->text());
    }
}

void ConversionDialog::on_pushButtonCalibrationFile_clicked() {
    QString startDir = ui->lineEditCalibrationPath->text();
    if (startDir.isEmpty() || !QFileInfo::exists(startDir))
        startDir = QDir::homePath();
    else
        startDir = QFileInfo(startDir).path();

    QString fileName = QFileDialog::getOpenFileName(
        this, "Выберите калибровочный файл (CLB)", startDir,
        "Калибровочные файлы (*.clb);;Все файлы (*.*)");

    if (!fileName.isEmpty()) {
        ui->lineEditCalibrationPath->setText(fileName);
        m_settings->setValue("Paths/calibrationFile", fileName);
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
        m_settings->setValue("Paths/savingDir", ui->lineEditSavingDir->text());
    }
}

void ConversionDialog::loadSettings() {
    ui->lineEditExperimentDir->setText(
        m_settings->value("Paths/experimentDir", "").toString());
    ui->lineEditCalibrationPath->setText(
        m_settings->value("Paths/calibrationFile", "").toString());
    ui->lineEditSavingDir->setText(
        m_settings->value("Paths/savingDir", QDir::homePath()).toString());
    ui->lineEditSavingFileName->setText(
        m_settings->value("Paths/outputFileName", "converted_cube").toString());

    QString format = m_settings->value("Output/format", "ENVI_BSQ").toString();
    int idx = ui->comboBoxOutputFormat->findData(format);
    if (idx >= 0) ui->comboBoxOutputFormat->setCurrentIndex(idx);

    ui->checkBoxParseDataCubes->setChecked(
        m_settings->value("Options/splitCubes", false).toBool());
    ui->checkBoxAddGpsData->setChecked(
        m_settings->value("Options/addGeoreferencing", true).toBool());
}

void ConversionDialog::on_lineEditSavingFileName_editingFinished() {
    if (!m_initting) {
        m_settings->setValue("Paths/outputFileName",
                             ui->lineEditSavingFileName->text());
    }
}

void ConversionDialog::on_comboBoxOutputFormat_currentIndexChanged(int index) {
    if (!m_initting) {
        m_settings->setValue(
            "Output/format",
            ui->comboBoxOutputFormat->currentData().toString());
    }
}

void ConversionDialog::on_checkBoxParseDataCubes_stateChanged(int arg1) {
    if (!m_initting) {
        m_settings->setValue("Options/splitCubes",
                             ui->checkBoxParseDataCubes->isChecked());
    }
}

void ConversionDialog::on_checkBoxAddGpsData_stateChanged(int arg1) {
    if (!m_initting) {
        m_settings->setValue("Options/addGeoreferencing",
                             ui->checkBoxAddGpsData->isChecked());
    }
}

void ConversionDialog::onConversionFinished(bool success,
                                            const QString &message) {
    ui->buttonBox->setEnabled(true);
    if (success) {
        QMessageBox::information(this, "Конвертация", message);
        QDialog::accept();  // закрываем диалог
    } else {
        QMessageBox::critical(this, "Ошибка конвертации", message);
    }
}

void ConversionDialog::onProgressUpdated(int percent) {
    ui->progressBar->setValue(percent);
}
