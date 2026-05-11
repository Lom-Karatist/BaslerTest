#include "SavingModule.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QtConcurrent/QtConcurrent>

#include "ImageFormatConverter.h"

SavingModule::SavingModule(QObject *parent)
    : QObject{parent},
      m_isNeedToSave(false),
      m_format(BaslerConstants::Tiff),
      m_batchSaver(nullptr) {}

SavingModule::~SavingModule() {
    if (m_batchSaver != nullptr) {
        //        m_batchSaver->shutdown();
        delete m_batchSaver;
    }
}

void SavingModule::setSavingPath(const QString &newSavingPath) {
    m_savingPath = newSavingPath;
    makeDir(newSavingPath);
    makeDir(newSavingPath + "/HS");
    makeDir(newSavingPath + "/OC");
}

void SavingModule::makeDir(const QString &newSavingPath) {
    QDir dir(newSavingPath);
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
    saveDataAsync(data, width, height, pixelFormat, prefix, timeStamp,
                  m_format);
}

void SavingModule::saveDataAsync(const QByteArray &data, int width, int height,
                                 int pixelFormat, const QString &prefix,
                                 const QString &timeStamp,
                                 BaslerConstants::SavingFormat format) {
    switch (format) {
        case BaslerConstants::Batched:
            if (!m_batchSaver) {
                m_batchSaver = new BatchSaver(m_savingPath, 150,
                                              400 * 1024 * 1024, nullptr);
            }
            m_batchSaver->addFrame(
                prefix, width, height,
                ImageFormatConverter::getPixelFormatName(pixelFormat), data, 0);
            break;
        case BaslerConstants::Png:
            (void)QtConcurrent::run(&SavingModule::saveAsPngAsync, data, width,
                                    height, pixelFormat, prefix, timeStamp,
                                    m_savingPath);
            break;
        case BaslerConstants::Tiff:
            (void)QtConcurrent::run(&SavingModule::saveAsTiffAsync, data, width,
                                    height, pixelFormat, prefix, timeStamp,
                                    m_savingPath);
            break;
        case BaslerConstants::Binary:
            (void)QtConcurrent::run(&SavingModule::saveAsBinaryAsync, data,
                                    prefix, timeStamp, m_savingPath);

            break;
    }
}

void SavingModule::saveAsTiffAsync(const QByteArray &data, int width,
                                   int height, int pixelFormat,
                                   const QString &prefix,
                                   const QString timeStamp,
                                   const QString savingPath) {
    QImage img =
        ImageFormatConverter::convertToQImage(data, width, height, pixelFormat);
    if (img.isNull()) {
        qWarning() << "Failed to convert to QImage for TIFF save";
        return;
    }

    QString fileName =
        QString("%1/%2/%3.tiff").arg(savingPath).arg(prefix).arg(timeStamp);
    qDebug() << fileName;
    if (!img.save(fileName, "TIFF")) {
        qWarning() << "Failed to save TIFF:" << fileName;
    }
}

void SavingModule::saveAsPngAsync(const QByteArray &data, int width, int height,
                                  int pixelFormat, const QString &prefix,
                                  const QString timeStamp,
                                  const QString savingPath) {
    QImage img =
        ImageFormatConverter::convertToQImage(data, width, height, pixelFormat);
    if (img.isNull()) {
        qWarning() << "Failed to convert to QImage for PNG save";
        return;
    }

    QString fileName =
        QString("%1/%2/%3.png").arg(savingPath).arg(prefix).arg(timeStamp);
    qDebug() << fileName;
    if (!img.save(fileName, "PNG")) {
        qWarning() << "Failed to save PNG:" << fileName;
    }
}

void SavingModule::saveAsBinaryAsync(const QByteArray &data,
                                     const QString &prefix,
                                     const QString timeStamp,
                                     const QString savingPath) {
    QString fileName =
        QString("%1/%2/%3.raw").arg(savingPath).arg(prefix).arg(timeStamp);
    qDebug() << fileName;
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
        case BaslerConstants::Batched:
            break;
        case BaslerConstants::Tiff:
        case BaslerConstants::Png:
            saveAsTiff(data, width, height, pixelFormat, appendix, timeStamp);
            break;
        case BaslerConstants::Binary:
            saveAsBinary(data, appendix, timeStamp);
            break;
    }
}

void SavingModule::saveAsTiff(const QByteArray &data, int width, int height,
                              int pixelFormat, const QString &prefix,
                              QString timeStamp) {
    QImage img =
        ImageFormatConverter::convertToQImage(data, width, height, pixelFormat);
    if (img.isNull()) {
        qWarning() << "Failed to convert to QImage for TIFF save";
        return;
    }

    QString fileName = generateFileName(prefix, timeStamp) + ".tiff";
    if (!img.save(fileName, "TIFF")) {
        qWarning() << "Failed to save TIFF:" << fileName;
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
    if (!newIsNeedToSave && m_batchSaver) {
        m_batchSaver->flush();
    }
    m_isNeedToSave = newIsNeedToSave;
}
