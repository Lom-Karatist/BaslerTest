#include "CubeBuilder.h"

#include <QDebug>
#include <QFile>

#include "CalibrationLoader.h"

CubeBuilder::CubeBuilder(QObject *parent) : QObject(parent) {}

HypercubeData CubeBuilder::buildCube(const QList<FrameDescriptor> &frames,
                                     const QString &calibrationFilePath,
                                     int &progressPercent) {
    HypercubeData result;
    progressPercent = 50;  // начинаем с 50%

    if (frames.isEmpty()) {
        emit error("Нет кадров для построения куба");
        return result;
    }

    // Определяем размерности куба
    int bands = frames.first().width;  // число спектральных каналов
    int cols =
        frames.first().height;  // число пространственных пикселей в строке
    int rows = frames.size();  // число строк (кадров)

    result.meta.rows = rows;
    result.meta.cols = cols;
    result.meta.bands = bands;
    result.meta.timestampsMs.clear();
    for (const FrameDescriptor &fd : frames) {
        result.meta.timestampsMs.append(fd.timestampMs);
    }

    // Создаём пустой массив для BSQ (Band Sequential)
    int totalPixels = rows * cols * bands;
    result.data.resize(totalPixels * 2);
    quint16 *dst = reinterpret_cast<quint16 *>(result.data.data());

    // Заполнение BSQ: для каждого канала (спектральной полосы) копируем
    // соответствующие пиксели из всех строк
    for (int b = 0; b < bands; ++b) {
        quint16 *bandStart = dst + (b * rows * cols);
        for (int r = 0; r < rows; ++r) {
            const quint16 *frameData = reinterpret_cast<const quint16 *>(
                frames[r].rawData.constData());
            for (int s = 0; s < cols; ++s) {
                bandStart[r * cols + s] = frameData[s * bands + b];
            }
        }
    }

    if (!calibrationFilePath.isEmpty()) {
        CalibrationLoader loader;
        if (loader.loadFromFile(calibrationFilePath, false)) {
            // Проверяем, что размеры матрицы соответствуют
            if (loader.bands() == bands && loader.cols() == cols) {
                result.meta.wavelengthMatrix = loader.wavelengthMatrix();
                result.meta.wavelengthsNm = loader.averageWavelengths();
                qDebug() << "Калибровка загружена: матрица" << bands << "x"
                         << cols;
            } else {
                emit error(QString("Размер калибровочной матрицы (%1x%2) не "
                                   "соответствует ожидаемому (%3x%4). "
                                   "Используется линейная шкала.")
                               .arg(loader.bands())
                               .arg(loader.cols())
                               .arg(bands)
                               .arg(cols));
                setLinearWavelengths(result.meta, bands);
            }
        } else {
            emit error("Не удалось загрузить калибровочный файл: " +
                       loader.errorString());
            setLinearWavelengths(result.meta, bands);
        }
    } else {
        setLinearWavelengths(result.meta, bands);
        qDebug()
            << "Калибровка не задана, используется линейная шкала 400-900 нм";
    }

    return result;
}

void CubeBuilder::setLinearWavelengths(HypercubeMetadata &meta, int bands) {
    meta.wavelengthsNm.clear();
    meta.wavelengthMatrix.clear();
    for (int b = 0; b < bands; ++b) {
        double wl = 400.0 + b * 500.0 / (bands - 1);
        meta.wavelengthsNm.append(wl);
        // Для матрицы: все столбцы имеют одинаковую длину волны в данном канале
        QVector<double> row(meta.cols, wl);
        meta.wavelengthMatrix.append(row);
    }
}
