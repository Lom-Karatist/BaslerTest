#ifndef HYPERCUBEDATA_H
#define HYPERCUBEDATA_H

#include <QList>
#include <QString>
#include <QVector>
#include <cstdint>

struct FrameDescriptor {
    quint64 timestampMs;  // временная метка кадра (Unix epoch ms)
    QByteArray
        rawData;  // распакованные 16-битные данные (uint16) – flat массив
    int width;  // ширина кадра (пикселей по горизонтали = число спектральных
                // каналов)
    int height;  // высота кадра (пикселей по вертикали = число пространственных
                 // точек)
    // для гиперспектрометра height=1 (одна строка), но для универсальности
    // храним всё
};

struct HypercubeMetadata {
    int rows;  // общее число строк (пространственных положений)
    int cols;  // число столбцов (пространственных пикселей в строке, обычно =
               // width)
    int bands;  // число спектральных каналов (высота исходного кадра)
    QVector<double> wavelengthsNm;  // длины волн для каждого канала
    QVector<QVector<double>> wavelengthMatrix;  // полная матрица (bands × cols)
                                                // для учёта smile-эффекта
    QVector<quint64> timestampsMs;  // временные метки для каждой строки
};

// Данные гиперкуба: плоский массив uint16 размером rows * cols * bands (порядок
// BSQ)
struct HypercubeData {
    QByteArray data;  // сырые 16-битные значения (little-endian)
    HypercubeMetadata meta;
};

#endif  // HYPERCUBEDATA_H
