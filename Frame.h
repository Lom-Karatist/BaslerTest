#ifndef FRAME_H
#define FRAME_H
#include <QByteArray>
#include <QMetaType>
#include <cstdint>

enum class PixelFormat {
    Unknown,
    Mono8,
    Mono12
};

enum class CameraType {
    Hyperspectral,
    Overview
};

struct Frame {
    QByteArray data;           // сырые пиксельные данные
    int width = 0;
    int height = 0;
    int bytesPerLine = 0;      // может быть width * pixelSize, если без выравнивания
    PixelFormat pixelFormat = PixelFormat::Unknown;
    quint64 timestamp = 0;      // в наносекундах
    quint64 frameNumber = 0;
    CameraType cameraType = CameraType::Hyperspectral;

    int pixelSize() const {
        switch (pixelFormat) {
            case PixelFormat::Mono8: return 1;
            case PixelFormat::Mono12: return 2;
            default: return 0;
        }
    }

    bool isValid() const {
        return !data.isEmpty() && width > 0 && height > 0 && pixelSize() > 0;
    }
};

Q_DECLARE_METATYPE(Frame)
#endif // FRAME_H
