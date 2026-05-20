#include "CalibrationLoader.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

CalibrationLoader::CalibrationLoader() {}

bool CalibrationLoader::loadFromFile(const QString &filePath, bool skipHeader) {
    m_wavelengthMatrix.clear();
    m_bands = 0;
    m_rows = 0;
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

        if (m_rows == 0) {
            m_bands = row.size();
            qDebug() << row.size() << row.first() << row.last();
        } else if (row.size() != m_bands) {
            m_errorString = QString(
                                "Несовпадение числа столбцов в строке %1: "
                                "ожидалось %2, получено %3")
                                .arg(lineNumber)
                                .arg(m_rows)
                                .arg(row.size());
            qWarning() << m_errorString;
            return false;
        }

        m_wavelengthMatrix.append(row);
        m_rows++;
    }

    file.close();

    if (m_wavelengthMatrix.isEmpty()) {
        m_errorString = "Файл не содержит корректных данных о длинах волн";
        return false;
    }

    qDebug() << "CalibrationLoader: загружена матрица" << m_bands << "x"
             << m_rows << "из" << filePath;
    return true;
}

QVector<double> CalibrationLoader::averageWavelengths() const {
    QVector<double> avg(m_bands, 0.0);
    if (m_bands == 0 || m_rows == 0) return avg;

    for (int b = 0; b < m_bands; ++b) {
        const QVector<double> &row = m_wavelengthMatrix[b];
        double sum = 0.0;
        for (double wl : row) sum += wl;
        avg[b] = sum / m_rows;
    }
    return avg;
}
