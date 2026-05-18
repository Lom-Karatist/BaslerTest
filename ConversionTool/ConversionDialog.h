#ifndef CONVERSIONDIALOG_H
#define CONVERSIONDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class ConversionDialog;
}

class ConversionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConversionDialog(QWidget *parent = nullptr);
    ~ConversionDialog();

private slots:
    void on_pushButtonExperimentDir_clicked();

    void on_pushButtonCalibrationFile_clicked();

    void on_pushButtonSavingDir_clicked();

    void on_lineEditSavingFileName_editingFinished();

    void on_comboBoxOutputFormat_currentIndexChanged(int index);

    void on_checkBoxParseDataCubes_stateChanged(int arg1);

    void on_checkBoxAddGpsData_stateChanged(int arg1);

private:
    void loadSettings();

    Ui::ConversionDialog *ui;
    QSettings *m_settings;
    bool m_initting;
};

#endif  // CONVERSIONDIALOG_H
