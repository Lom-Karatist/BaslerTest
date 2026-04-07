#include "ImageFormatConverter.h"

#include <QDebug>

ImageFormatConverter::ImageFormatConverter(QObject *parent) : QObject{parent} {}

QRgb ImageFormatConverter::getHeatmapColor(double value, double maxVal,
                                           HeatmapPalette palette) {
    double t = qBound(0.0, value / maxVal, 1.0);
    int r, g, b;

    switch (palette) {
        case HeatmapPalette::Default:
            if (t < 0.5) {
                double t2 = t / 0.5;  // 0..1
                r = 0;
                g = static_cast<int>(255 * t2);
                b = static_cast<int>(255 * (1 - t2));
            } else {
                double t2 = (t - 0.5) / 0.5;  // 0..1
                r = static_cast<int>(255 * t2);
                g = static_cast<int>(255 * (1 - t2));
                b = 0;
            }
            break;
        case HeatmapPalette::Plasma:
            if (t <= 0.5) {
                double t2 = t / 0.5;  // 0..1
                r = static_cast<int>(13 + (237 - 13) * t2);
                g = static_cast<int>(8 + (121 - 8) * t2);
                b = static_cast<int>(135 + (83 - 135) * t2);
            } else {
                double t2 = (t - 0.5) / 0.5;  // 0..1
                r = static_cast<int>(237 + (249 - 237) * t2);
                g = static_cast<int>(121 + (248 - 121) * t2);
                b = static_cast<int>(83 + (113 - 83) * t2);
            }
            break;
        case HeatmapPalette::Viridis:
            if (t <= 0.25) {
                double t2 = t / 0.25;  // 0..1
                r = static_cast<int>(68 + (59 - 68) * t2);
                g = static_cast<int>(1 + (82 - 1) * t2);
                b = static_cast<int>(84 + (139 - 84) * t2);
            } else if (t <= 0.5) {
                double t2 = (t - 0.25) / 0.25;  // 0..1
                r = static_cast<int>(59 + (33 - 59) * t2);
                g = static_cast<int>(82 + (145 - 82) * t2);
                b = static_cast<int>(139 + (140 - 139) * t2);
            } else if (t <= 0.75) {
                double t2 = (t - 0.5) / 0.25;  // 0..1
                r = static_cast<int>(33 + (94 - 33) * t2);
                g = static_cast<int>(145 + (201 - 145) * t2);
                b = static_cast<int>(140 + (98 - 140) * t2);
            } else {
                double t2 = (t - 0.75) / 0.25;  // 0..1
                r = static_cast<int>(94 + (253 - 94) * t2);
                g = static_cast<int>(201 + (231 - 201) * t2);
                b = static_cast<int>(98 + (37 - 98) * t2);
            }
            break;
        case HeatmapPalette::Cividis:
            if (t <= 0.25) {
                double t2 = t / 0.25;
                r = static_cast<int>(0 + (72 - 0) * t2);
                g = static_cast<int>(51 + (92 - 51) * t2);
                b = static_cast<int>(102 + (142 - 102) * t2);
            } else if (t <= 0.5) {
                double t2 = (t - 0.25) / 0.25;
                r = static_cast<int>(72 + (104 - 72) * t2);
                g = static_cast<int>(92 + (136 - 92) * t2);
                b = static_cast<int>(142 + (148 - 142) * t2);
            } else if (t <= 0.75) {
                double t2 = (t - 0.5) / 0.25;
                r = static_cast<int>(104 + (167 - 104) * t2);
                g = static_cast<int>(136 + (173 - 136) * t2);
                b = static_cast<int>(148 + (98 - 148) * t2);
            } else {
                double t2 = (t - 0.75) / 0.25;
                r = static_cast<int>(167 + (255 - 167) * t2);
                g = static_cast<int>(173 + (255 - 173) * t2);
                b = static_cast<int>(98 + (128 - 98) * t2);
            }
            break;
        case HeatmapPalette::Grayscale:
            r = g = b = static_cast<int>(255 * t);
            break;
    }

    return qRgb(r, g, b);
}

QImage ImageFormatConverter::convertToHeatmapImage(const QByteArray &data,
                                                   int width, int height,
                                                   int pixelFormat,
                                                   int numBands,
                                                   HeatmapPalette palette) {
    if (width <= 0 || height <= 0 || numBands <= 0 || data.isEmpty())
        return QImage();

    // 1. Получаем пиксельные значения в универсальном формате (16 бит)
    QVector<quint16> pixels = getPixelValues(data, width, height, pixelFormat);
    if (pixels.isEmpty()) return QImage();

    // 2. Ширина поддиапазона (целочисленное деление)
    int bandWidth = width / numBands;
    if (bandWidth == 0) {
        qWarning() << "numBands > width, cannot split into bands";
        return QImage();
    }

    // 3. Вычисляем максимумы для каждого поддиапазона и каждого спектра
    QVector<quint16> maxVals(numBands * height, 0);
    quint16 globalMax = 0;

    for (int y = 0; y < height; ++y) {
        int rowStart = y * width;
        for (int band = 0; band < numBands; ++band) {
            int startCol = band * bandWidth;
            int endCol = (band == numBands - 1) ? width : startCol + bandWidth;
            quint16 maxVal = 0;
            for (int x = startCol; x < endCol; ++x) {
                quint16 val = pixels[rowStart + x];
                if (val > maxVal) maxVal = val;
            }
            int idx = band * height + y;
            maxVals[idx] = maxVal;
            if (maxVal > globalMax) globalMax = maxVal;
        }
    }

    if (globalMax == 0) {
        QImage blackImage(height, numBands, QImage::Format_RGB32);
        blackImage.fill(Qt::black);
        return blackImage;
    }

    // 4. Строим тепловую карту
    QImage heatmap(height, numBands, QImage::Format_RGB32);
    for (int band = 0; band < numBands; ++band) {
        for (int spec = 0; spec < height; ++spec) {
            quint16 value = maxVals[band * height + spec];
            QRgb color = getHeatmapColor(value, globalMax);
            heatmap.setPixelColor(spec, band, QColor(color));
        }
    }
    return heatmap;
}

QImage ImageFormatConverter::convertToQImage(const QByteArray &data, int width,
                                             int height, int pixelFormat) {
    QImage::Format format = QImage::Format_Invalid;

    switch (pixelFormat) {
        case PixelType_Mono8:
            format = QImage::Format_Grayscale8;
            break;
        case PixelType_Mono12:
        case PixelType_Mono12p:
            format = QImage::Format_Grayscale16;
            break;
        default:
            return QImage();
    }

    if (format == QImage::Format_Invalid) return QImage();

    if (pixelFormat == PixelType_Mono8) {
        if (data.size() != width * height) return QImage();
        QImage image(reinterpret_cast<const uchar *>(data.constData()), width,
                     height, width, format);
        return image.copy();
    } else if (pixelFormat == PixelType_Mono12) {
        if (data.size() != width * height * 2) return QImage();
        QImage image(reinterpret_cast<const uchar *>(data.constData()), width,
                     height, width * 2, format);
        return image.copy();
    } else if (pixelFormat == PixelType_Mono12p) {
        // Распаковка в 16-битные значения
        int numPixels = width * height;
        QVector<quint16> unpacked = unpackMono12p(data, numPixels);
        if (unpacked.size() != numPixels) return QImage();
        // Создаём QImage с копированием данных
        QImage image(width, height, format);
        for (int y = 0; y < height; ++y) {
            memcpy(image.scanLine(y), unpacked.constData() + y * width,
                   width * 2);
        }
        return image;
    }
    return QImage();
}

QVector<quint16> ImageFormatConverter::unpackMono12p(const QByteArray &data,
                                                     int numPixels) {
    QVector<quint16> result(numPixels);
    const uchar *src = reinterpret_cast<const uchar *>(data.constData());
    int bytesNeeded = (numPixels * 12 + 7) / 8;
    if (data.size() < bytesNeeded) {
        qWarning() << "Insufficient data for Mono12p unpacking";
        return result;
    }

    for (int i = 0; i < numPixels; ++i) {
        int byteOffset = (i * 12) / 8;
        int bitOffset = (i * 12) % 8;
        quint16 val = 0;
        // Читаем 12 бит из сырых данных (little-endian порядок в байтах)
        // В Pylon порядок байт для Mono12p: первый байт содержит младшие 8 бит
        // первого пикселя, старшие 4 бита первого пикселя и младшие 4 бита
        // второго пикселя — во втором байте, и т.д. Но проще извлечь по битам,
        // используя сдвиги. Стандартный метод: считываем 2 байта, маскируем,
        // сдвигаем. Для простоты используем побайтовое чтение.
        if (bitOffset == 0) {
            // Пиксель начинается с границы байта
            val = (src[byteOffset] | (src[byteOffset + 1] << 8)) & 0x0FFF;
        } else {
            // Пиксель начинается не с границы байта
            val = ((src[byteOffset] >> bitOffset) |
                   (src[byteOffset + 1] << (8 - bitOffset))) &
                  0x0FFF;
        }
        result[i] = val;
    }
    return result;
}

QVector<quint16> ImageFormatConverter::getPixelValues(const QByteArray &data,
                                                      int width, int height,
                                                      int pixelFormat) {
    QVector<quint16> result;
    result.reserve(width * height);

    switch (pixelFormat) {
        case PixelType_Mono8: {
            const quint8 *ptr =
                reinterpret_cast<const quint8 *>(data.constData());
            int numPixels = data.size();
            if (numPixels != width * height) {
                qWarning() << "Data size mismatch for Mono8";
                return QVector<quint16>();
            }
            for (int i = 0; i < numPixels; ++i) {
                result.append(ptr[i]);
            }
            break;
        }
        case PixelType_Mono12: {
            const quint16 *ptr =
                reinterpret_cast<const quint16 *>(data.constData());
            int numPixels = data.size() / sizeof(quint16);
            if (numPixels != width * height) {
                qWarning() << "Data size mismatch for Mono12";
                return QVector<quint16>();
            }
            for (int i = 0; i < numPixels; ++i) {
                result.append(ptr[i] & 0x0FFF);
            }
            break;
        }
        case PixelType_Mono12p: {
            QVector<quint16> unpacked = unpackMono12p(data, width * height);
            if (unpacked.size() != width * height) {
                qWarning() << "Failed to unpack Mono12p data";
                return QVector<quint16>();
            }
            result = unpacked;
            break;
        }
        default:
            qWarning() << "Unsupported pixel format for getPixelValues:"
                       << pixelFormat;
            return QVector<quint16>();
    }
    return result;
}
