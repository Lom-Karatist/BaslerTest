#include "SigWriter.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

SigWriter::SigWriter(QObject *parent) : QObject(parent) {}

bool SigWriter::write(const HypercubeData &cube, const QString &basePath,
                      bool addGeoreferencing) {
    emitProgress(0);
    if (!writeBinaryFile(basePath + ".bin", cube)) {
        emit error("Ошибка записи бинарного файла");
        return false;
    }
    emitProgress(85);

    if (!writeHdrFile(basePath + ".hdr", cube, addGeoreferencing)) {
        emit error("Ошибка записи HDR-файла");
        return false;
    }
    emitProgress(90);

    if (!writeWavelengthsFile(basePath + "_waves.clb", cube)) {
        emit error("Ошибка записи файла длин волн");
        return false;
    }
    emitProgress(95);

    if (!writeCalibrationFile(basePath + ".clb", cube)) {
        emit error("Ошибка записи калибровочного файла");
        return false;
    }
    emitProgress(100);

    return true;
}

bool SigWriter::writeBinaryFile(const QString &path,
                                const HypercubeData &cube) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Не удалось открыть файл для записи:" << path;
        return false;
    }

    qint64 written = file.write(cube.data);
    file.close();
    return written == cube.data.size();
}

bool SigWriter::writeHdrFile(const QString &path, const HypercubeData &cube,
                             bool addGeoreferencing) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть HDR-файл:" << path;
        return false;
    }

    QTextStream out(&file);
    // В Qt 6 кодировка UTF-8 по умолчанию, setCodec не нужен

    // Секция [Paths]
    out << "[Paths]\n";
    out << "dataPath = \""
        << "/" + QFileInfo(path).baseName() + ".bin\"\n";
    out << "calibrationPath = \""
        << "/" + QFileInfo(path).baseName() + ".clb\"\n";
    out << "wavelength = \""
        << "/" + QFileInfo(path).baseName() + "_waves.clb\"\n";

    // Секция [Params]
    out << "[Params]\n";
    out << "samples = " << cube.meta.cols << "\n";
    out << "lines = " << cube.meta.rows << "\n";
    out << "bands = " << cube.meta.bands << "\n";
    out << "byteOrder = 0\n";
    out << "dataType = \"uint16\"\n";

    if (addGeoreferencing) {
        out << "leftTopCornerCoord = {33.2651, 35.2967}\n";
        out << "rightTopCornerCoord = {33.2323, 35.4832}\n";
        out << "rightBottomCornerCoord = {32.8231, 35.3815}\n";
        out << "leftBottomCornerCoord = {32.8557, 35.1958}\n";
        out << "pixelSize = 0.5\n";
        out << "altitude = 150\n";
        out << "rotationAngle = 349.5\n";
    }

    out << "viewingDirection = 0\n";  // 0 — надир
    out << "aperture = 3.17\n";
    out << "date = " << QDateTime::currentDateTime().toString("dd/MM/yyyy")
        << "\n";
    out << "time = " << QDateTime::currentDateTime().toString("hh:mm:ss")
        << "\n";
    out << "measurementsType = ADC\n";
    out << "valueMultiplier = 1\n";

    file.close();
    return true;
}

bool SigWriter::writeWavelengthsFile(const QString &path,
                                     const HypercubeData &cube) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл длин волн:" << path;
        return false;
    }
    QTextStream out(&file);
    const QVector<double> &wl = cube.meta.wavelengthsNm;
    for (double v : wl) {
        out << QString::number(v, 'f', 4) << "\n";
    }
    file.close();
    return true;
}

bool SigWriter::writeCalibrationFile(const QString &path,
                                     const HypercubeData &cube) {
    // Пока записываем единичные коэффициенты (по одному на канал)
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть калибровочный файл:" << path;
        return false;
    }
    QTextStream out(&file);
    for (int i = 0; i < cube.meta.bands; ++i) {
        out << "1.0\n";
    }
    file.close();
    return true;
}

void SigWriter::emitProgress(int absolutePercent) {
    emit progressUpdated(absolutePercent);
}
