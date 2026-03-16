#include "BaslerWindow.h"
#include "ui_BaslerWindow.h"

BaslerWindow::BaslerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BaslerWindow)
{
    ui->setupUi(this);
}

BaslerWindow::~BaslerWindow()
{
    delete ui;
}

