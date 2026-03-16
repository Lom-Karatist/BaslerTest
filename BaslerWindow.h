#ifndef BASLERWINDOW_H
#define BASLERWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class BaslerWindow; }
QT_END_NAMESPACE

class BaslerWindow : public QMainWindow
{
    Q_OBJECT

public:
    BaslerWindow(QWidget *parent = nullptr);
    ~BaslerWindow();

private:
    Ui::BaslerWindow *ui;
};
#endif // BASLERWINDOW_H
