#include "BaslerWindow.h"

#include <QApplication>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <BaseTools/QrcFilesRestorer.h>

void myMessageOutput(QtMsgType Mtype, const QMessageLogContext &context, const QString &msg)
{
    QFile file(QCoreApplication::applicationDirPath()+"/BaslerTest.log");
    if (file.exists()) file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    else file.open(QIODevice::WriteOnly | QIODevice::Text);
    QString time = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    QString OutMessage;
    QTextStream out(&file);

    if(Mtype == QtInfoMsg)    {OutMessage = QString("Info[%1]: %2\n").arg(time).arg(msg);}
    if(Mtype == QtDebugMsg)   {OutMessage = QString("Debug[%1]: %2 (%3:%4, %5)\n").arg(time).arg(msg).arg(context.file).arg(context.line).arg(context.function);}
    if(Mtype == QtWarningMsg) {OutMessage = QString("Warning[%1]: %2 (%3:%4, %5)\n").arg(time).arg(msg).arg(context.file).arg(context.line).arg(context.function);}
    if(Mtype == QtCriticalMsg){OutMessage = QString("Critical[%1]: %2 (%3:%4, %5)\n").arg(time).arg(msg).arg(context.file).arg(context.line).arg(context.function);}
    if(Mtype == QtFatalMsg)   {OutMessage = QString("Fatal[%1]: %2 (%3:%4, %5)\n").arg(time).arg(msg).arg(context.file).arg(context.line).arg(context.function);abort();}

    out << OutMessage;
    file.close();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QrcFilesRestorer::restoreFilesFromQrc(":/Basler/4Release/");
//    qInstallMessageHandler(myMessageOutput);
    BaslerWindow w;
    w.show();
    return a.exec();
}
