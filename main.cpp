#include "BaslerWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BaslerWindow w;
    w.show();
    return a.exec();
}
