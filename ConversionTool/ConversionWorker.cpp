#include "ConversionWorker.h"

#include <QDebug>

#include "BatchReader.h"
#include "CubeBuilder.h"
#include "SigWriter.h"

ConversionWorker::ConversionWorker(QObject *parent) : QObject(parent) {}

void ConversionWorker::runConversion(const QString &experimentDir,
                                     const QString &calibrationFile,
                                     const QString &outputDir,
                                     const QString &outputFileName,
                                     const QString &outputFormat,
                                     bool splitCubes, bool addGeoreferencing) {
    Q_UNUSED(outputDir)  // пока не используется
    Q_UNUSED(outputFileName)
    Q_UNUSED(outputFormat)
    Q_UNUSED(splitCubes)
    Q_UNUSED(addGeoreferencing)

    BatchReader reader;
    CubeBuilder builder;

    connect(&reader, &BatchReader::progressUpdate, [this](int p) {
        int scaled = p * 20 / 100;
        emit progressUpdated(scaled);
    });
    connect(&builder, &CubeBuilder::progressUpdate, [this](int p) {
        int scaled = 20 + p * 40 / 100;
        emit progressUpdated(scaled);
    });

    connect(&reader, &BatchReader::error,
            [this](const QString &msg) { emit finished(false, msg); });
    connect(&builder, &CubeBuilder::error,
            [this](const QString &msg) { emit finished(false, msg); });

    int progressPlaceholder = 0;
    QList<FrameDescriptor> frames =
        reader.readAllBatches(experimentDir, progressPlaceholder);
    qDebug() << "Frames were red:" << frames.count();

    if (frames.isEmpty()) {
        emit finished(false, "Не найдено ни одного кадра в " + experimentDir);
        return;
    }

    HypercubeData cube =
        builder.buildCube(frames, calibrationFile, progressPlaceholder);
    if (cube.data.isEmpty()) {
        emit finished(false, "Не удалось построить гиперкуб");
        return;
    }

    QString basePath = outputDir + "/" + outputFileName;
    if (!basePath.endsWith("/")) basePath = outputDir + "/" + outputFileName;

    SigWriter writer;
    connect(&writer, &SigWriter::progressUpdated, [this](int p) {
        int scaled = 60 + p * 40 / 100;
        emit progressUpdated(scaled);
    });
    connect(&writer, &SigWriter::error,
            [this](const QString &msg) { emit finished(false, msg); });

    bool success = writer.write(cube, basePath, addGeoreferencing);
    if (!success) {
        return;
    }

    emit finished(true, "Конвертация завершена успешно");
}
