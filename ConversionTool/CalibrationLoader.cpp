#include "CalibrationLoader.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

CalibrationLoader::CalibrationLoader() {}

bool CalibrationLoader::loadFromFile(const QString &filePath, bool skipHeader) {
    m_wavelengthMatrix.clear();
    m_bands = 0;
    m_cols = 0;
    m_errorString.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_errorString = QString("Не удалось открыть файл: %1").arg(filePath);
        qWarning() << m_errorString;
        return false;
    }

    QTextStream stream(&file);
    int lineNumber = 0;
    bool headerSkipped = false;
    QRegularExpression re("\\s+");

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lineNumber++;
        line = line.trimmed();
        if (line.isEmpty()) continue;
        if (line.startsWith('#')) continue;

        // Пропуск заголовка (первая некомментарная строка)
        if (!headerSkipped && skipHeader) {
            headerSkipped = true;
            continue;
        }

        QStringList parts = line.split(re, Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        // Текущая строка — спектральный канал
        QVector<double> row;
        for (const QString &part : parts) {
            bool ok;
            double wl = part.toDouble(&ok);
            if (!ok) {
                m_errorString = QString("Некорректное число в строке %1: %2")
                                    .arg(lineNumber)
                                    .arg(part);
                qWarning() << m_errorString;
                return false;
            }
            row.append(wl);
        }

        if (m_bands == 0) {
            // Первый канал — определяем количество столбцов
            m_cols = row.size();
        } else if (row.size() != m_cols) {
            m_errorString = QString(
                                "Несовпадение числа столбцов в строке %1: "
                                "ожидалось %2, получено %3")
                                .arg(lineNumber)
                                .arg(m_cols)
                                .arg(row.size());
            qWarning() << m_errorString;
            return false;
        }

        m_wavelengthMatrix.append(row);
        m_bands++;
    }

    file.close();

    if (m_wavelengthMatrix.isEmpty()) {
        m_errorString = "Файл не содержит корректных данных о длинах волн";
        return false;
    }

    //    QVector<QVector<double>> transposed(m_cols, QVector<double>(m_bands,
    //    0.0)); for (int i = 0; i < m_bands; ++i) {
    //        for (int j = 0; j < m_cols; ++j) {
    //            transposed[j][i] = m_wavelengthMatrix[i][j];
    //        }
    //    }

    //    m_wavelengthMatrix = transposed;
    //    int oldBands = m_bands;
    //    m_bands = m_cols;
    //    m_cols = oldBands;

    qDebug() << "CalibrationLoader: загружена матрица" << m_bands << "x"
             << m_cols << "из" << filePath;
    return true;
}

QVector<double> CalibrationLoader::averageWavelengths() const {
    QVector<double> avg(m_bands, 0.0);
    if (m_bands == 0 || m_cols == 0) return avg;

    for (int b = 0; b < m_bands; ++b) {
        const QVector<double> &row = m_wavelengthMatrix[b];
        double sum = 0.0;
        for (double wl : row) sum += wl;
        avg[b] = sum / m_cols;
    }
    return avg;
}
