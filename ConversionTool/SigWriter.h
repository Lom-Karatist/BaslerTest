#ifndef SIGWRITER_H
#define SIGWRITER_H

#include <QObject>
#include <QString>

#include "HypercubeData.h"

/**
 * @brief Класс для сохранения гиперкуба в формат ПО "Сигнатура".
 *
 * Формат включает:
 * - бинарный файл с данными (BSQ, uint16)
 * - HDR-файл (заголовок) с метаданными
 * - файл длин волн (wavelengths)
 * - файл калибровочных коэффициентов (пока коэффициенты = 1)
 */
class SigWriter : public QObject {
    Q_OBJECT
public:
    explicit SigWriter(QObject *parent = nullptr);

    /**
     * @brief Запись гиперкуба в файлы.
     * @param cube      данные гиперкуба
     * @param basePath  базовый путь и имя файла (без расширения)
     *                  Например, "/home/user/result" -> будут созданы
     *                  result.bin, result.hdr, result.wv, result.cal
     * @param addGeoreferencing  добавлять ли геопривязку (из метаданных)
     * @return true при успешной записи, false при ошибке
     */
    bool write(const HypercubeData &cube, const QString &basePath,
               bool addGeoreferencing);

signals:
    void progressUpdated(int percent);  // абсолютный процент (0..100)
    void error(const QString &message);

private:
    bool writeBinaryFile(const QString &path, const HypercubeData &cube);
    bool writeHdrFile(const QString &path, const HypercubeData &cube,
                      bool addGeoreferencing);
    bool writeWavelengthsFile(const QString &path, const HypercubeData &cube);
    bool writeCalibrationFile(const QString &path, const HypercubeData &cube);

    void emitProgress(int absolutePercent);
};

#endif  // SIGWRITER_H
