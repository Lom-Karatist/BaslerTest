#ifndef CALIBRATIONLOADER_H
#define CALIBRATIONLOADER_H

#include <QDebug>
#include <QString>
#include <QVector>

/**
 * @brief Загрузчик калибровочной матрицы длин волн из файла .clb.
 *
 * Файл .clb представляет собой текстовый файл, где каждая строка соответствует
 * одному спектральному каналу (строке матрицы гиперспектрометра), а столбцы —
 * пространственным пикселям (столбцам матрицы). Значения разделяются пробелами
 * или табуляцией.
 *
 * Пример для 3 каналов и 2 пространственных пикселей:
 * 400.2   401.5
 * 500.1   502.0
 * 600.4   603.3
 *
 * Поддерживаются комментарии (#) и пропуск пустых строк.
 */
class CalibrationLoader {
public:
    CalibrationLoader();

    /**
     * @brief Загружает матрицу длин волн из файла.
     * @param filePath путь к файлу .clb
     * @param skipHeader если true, первая непустая строка файла пропускается
     * @return true при успешной загрузке
     */
    bool loadFromFile(const QString &filePath, bool skipHeader = false);

    /**
     * @brief Возвращает матрицу длин волн (bands × cols).
     * @return QVector<QVector<double>> – внешний вектор по каналам, внутренний
     * — по столбцам
     */
    QVector<QVector<double>> wavelengthMatrix() const {
        return m_wavelengthMatrix;
    }

    /**
     * @brief Возвращает число спектральных каналов (строк матрицы).
     */
    int bands() const { return m_bands; }

    /**
     * @brief Возвращает число пространственных пикселей (столбцов матрицы).
     */
    int cols() const { return m_rows; }

    /**
     * @brief Проверяет, были ли загружены данные.
     */
    bool isValid() const { return !m_wavelengthMatrix.isEmpty(); }

    /**
     * @brief Возвращает сообщение об ошибке, если загрузка не удалась.
     */
    QString errorString() const { return m_errorString; }

    /**
     * @brief Вычисляет средние длины волн по столбцам (усреднение по
     * пространственным пикселям).
     * @return вектор длин волн для каждого канала (размер = bands)
     */
    QVector<double> averageWavelengths() const;

private:
    QVector<QVector<double>> m_wavelengthMatrix;
    int m_bands = 0;
    int m_rows = 0;
    QString m_errorString;
};

#endif  // CALIBRATIONLOADER_H
