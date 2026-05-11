#ifndef CONVERSIONDIALOG_H
#define CONVERSIONDIALOG_H

#include <QDialog>

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

private:
    Ui::ConversionDialog *ui;
};

#endif  // CONVERSIONDIALOG_H
