#ifndef CONVERSIONDIALOG_H
#define CONVERSIONDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QThread>

#include "ConversionWorker.h"

namespace Ui {
class ConversionDialog;
}

class ConversionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConversionDialog(QWidget *parent = nullptr);
    ~ConversionDialog();

    void accept();

private slots:
    void on_pushButtonExperimentDir_clicked();

    void on_pushButtonCalibrationFile_clicked();

    void on_pushButtonSavingDir_clicked();

    void on_lineEditSavingFileName_editingFinished();

    void on_comboBoxOutputFormat_currentIndexChanged(int index);

    void on_checkBoxParseDataCubes_stateChanged(int arg1);

    void on_checkBoxAddGpsData_stateChanged(int arg1);

    void onConversionFinished(bool success, const QString &message);
    void onProgressUpdated(int percent);

private:
    void loadSettings();

    Ui::ConversionDialog *ui;
    QSettings *m_settings;
    bool m_initting;

    QThread *m_workerThread;
    ConversionWorker *m_worker;
};

#endif  // CONVERSIONDIALOG_H
