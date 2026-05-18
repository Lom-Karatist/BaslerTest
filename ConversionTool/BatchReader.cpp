#include "BatchReader.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtDebug>

BatchReader::BatchReader(QObject *parent) : QObject(parent) {}

QList<FrameDescriptor> BatchReader::readAllBatches(const QString &experimentDir,
                                                   int &progressPercent) {
    QList<FrameDescriptor> allFrames;
    progressPercent = 0;

    QDir hsDir(experimentDir + "/HS");
    if (!hsDir.exists()) {
        emit error("Папка HS не найдена: " + hsDir.absolutePath());
        return allFrames;
    }

    QStringList jsonFiles =
        hsDir.entryList(QStringList() << "*.json", QDir::Files);
    jsonFiles.sort();

    if (jsonFiles.isEmpty()) {
        emit error("Нет JSON-файлов в папке HS");
        return allFrames;
    }

    int totalBatches = jsonFiles.size();
    int processedBatches = 0;

    for (const QString &jsonFileName : jsonFiles) {
        QString baseName = jsonFileName.left(jsonFileName.lastIndexOf('.'));
        QString binPath = hsDir.absolutePath() + "/" + baseName + ".bin";
        QString jsonPath = hsDir.absolutePath() + "/" + jsonFileName;

        QFile jsonFile(jsonPath);
        if (!jsonFile.open(QIODevice::ReadOnly)) {
            emit error("Не удалось открыть JSON: " + jsonPath);
            continue;
        }
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
        jsonFile.close();
        if (doc.isNull()) {
            emit error("Ошибка парсинга JSON: " + jsonPath);
            continue;
        }
        QJsonObject root = doc.object();

        int width = root["width"].toInt();
        int height = root["height"].toInt();
        QString pixelFormat = root["pixelFormat"].toString();
        if (pixelFormat != "Mono12p") {
            emit error("Неподдерживаемый формат пикселя: " + pixelFormat);
            continue;
        }

        QJsonArray framesArray = root["frames"].toArray();
        int frameCount = framesArray.size();
        qDebug() << "frameCount" << frameCount;

        QFile binFile(binPath);
        if (!binFile.open(QIODevice::ReadOnly)) {
            emit error("Не удалось открыть BIN: " + binPath);
            continue;
        }

        for (int i = 0; i < frameCount; ++i) {
            QJsonObject frameObj = framesArray[i].toObject();
            quint64 ts = frameObj["timestamp_ms"].toVariant().toULongLong();
            int sizeBytes = frameObj["bytes"].toInt();

            QByteArray packedData = binFile.read(sizeBytes);
            if (packedData.size() != sizeBytes) {
                emit error("Неполные данные в BIN файле: " + binPath);
                break;
            }

            QByteArray unpacked = unpackMono12p(packedData, width, height);
            if (unpacked.isEmpty()) {
                emit error("Ошибка распаковки Mono12p в кадре " +
                           QString::number(i) + " пакета " + baseName);
                continue;
            }

            FrameDescriptor frame;
            frame.timestampMs = ts;
            frame.rawData = unpacked;
            frame.width = width;
            frame.height = height;
            allFrames.append(frame);
        }
        binFile.close();

        processedBatches++;
        progressPercent =
            (processedBatches * 50) / totalBatches;  // 50% от общей работы
        emit progressUpdate(progressPercent);
    }

    return allFrames;
}

QByteArray BatchReader::unpackMono12p(const QByteArray &packed, int width,
                                      int height) {
    int totalPixels = width * height;
    QByteArray unpacked(totalPixels * 2, Qt::Uninitialized);
    quint16 *dst = reinterpret_cast<quint16 *>(unpacked.data());
    const uchar *src = reinterpret_cast<const uchar *>(packed.constData());

    for (int i = 0; i < totalPixels; ++i) {
        int byteOffset = (i * 12) / 8;
        int bitOffset = (i * 12) % 8;
        quint16 val = 0;
        if (bitOffset == 0) {
            val = (src[byteOffset] | (src[byteOffset + 1] << 8)) & 0x0FFF;
        } else {
            val = ((src[byteOffset] >> bitOffset) |
                   (src[byteOffset + 1] << (8 - bitOffset))) &
                  0x0FFF;
        }
        dst[i] = val;
    }
    return unpacked;
}
