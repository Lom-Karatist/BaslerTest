#include "SavingModule.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QtConcurrent/QtConcurrent>

#include "ImageFormatConverter.h"

SavingModule::SavingModule(QObject *parent)
    : QObject{parent}, m_isNeedToSave(false), m_format(BaslerConstants::Bmp) {}

void SavingModule::setSavingPath(const QString &newSavingPath) {
    m_savingPath = newSavingPath;
    QDir dir(m_savingPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

void SavingModule::setFormat(BaslerConstants::SavingFormat newFormat) {
    m_format = newFormat;
}

void SavingModule::saveDataAsync(const QByteArray &data, int width, int height,
                                 int pixelFormat, const QString &prefix,
                                 const QString &timeStamp) {
    switch (m_format) {
        case BaslerConstants::Bmp:
            (void)QtConcurrent::run(&SavingModule::saveAsBmpAsync, data, width,
                                    height, pixelFormat, prefix, timeStamp,
                                    m_savingPath);
            break;
        case BaslerConstants::Binary:
            (void)QtConcurrent::run(&SavingModule::saveAsBinaryAsync, data,
                                    prefix, timeStamp, m_savingPath);
            break;
    }
}

void SavingModule::saveAsBmpAsync(const QByteArray &data, int width, int height,
                                  int pixelFormat, const QString &prefix,
                                  const QString timeStamp,
                                  const QString savingPath) {
    QImage img =
        ImageFormatConverter::convertToQImage(data, width, height, pixelFormat);
    if (img.isNull()) {
        qWarning() << "Failed to convert to QImage for BMP save";
        return;
    }

    QString fileName =
        QString("%1/%2_%3.bmp").arg(savingPath).arg(prefix).arg(timeStamp);
    if (!img.save(fileName, "BMP")) {
        qWarning() << "Failed to save BMP:" << fileName;
    }
}

void SavingModule::saveAsBinaryAsync(const QByteArray &data,
                                     const QString &prefix,
                                     const QString timeStamp,
                                     const QString savingPath) {
    QString fileName =
        QString("%1/%2_%3.raw").arg(savingPath).arg(prefix).arg(timeStamp);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    } else {
        qWarning() << "Failed to save raw data:" << fileName;
    }
}

void SavingModule::saveData(const QByteArray &data, int width, int height,
                            int pixelFormat, QString appendix,
                            QString timeStamp) {
    switch (m_format) {
        case BaslerConstants::Bmp:
            saveAsBmp(data, width, height, pixelFormat, appendix, timeStamp);
            break;
        case BaslerConstants::Binary:
            saveAsBinary(data, appendix, timeStamp);
            break;
    }
}

void SavingModule::saveAsBmp(const QByteArray &data, int width, int height,
                             int pixelFormat, const QString &prefix,
                             QString timeStamp) {
    QImage img =
        ImageFormatConverter::convertToQImage(data, width, height, pixelFormat);
    if (img.isNull()) {
        qWarning() << "Failed to convert to QImage for BMP save";
        return;
    }

    QString fileName = generateFileName(prefix, timeStamp) + ".bmp";
    if (!img.save(fileName, "BMP")) {
        qWarning() << "Failed to save BMP:" << fileName;
    }
}

void SavingModule::saveAsBinary(const QByteArray &data, const QString &prefix,
                                QString timeStamp) {
    QString fileName = generateFileName(prefix, timeStamp) + ".raw";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
    } else {
        qWarning() << "Failed to save raw data:" << fileName;
    }
}

QString SavingModule::generateFileName(const QString &prefix,
                                       QString timeStamp) const {
    return QString("%1/%2_%3").arg(m_savingPath).arg(prefix).arg(timeStamp);
}

bool SavingModule::isNeedToSave() const { return m_isNeedToSave; }

void SavingModule::setIsNeedToSave(bool newIsNeedToSave) {
    m_isNeedToSave = newIsNeedToSave;
}
