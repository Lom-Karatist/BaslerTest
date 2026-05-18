#ifndef BATCHREADER_H
#define BATCHREADER_H

#include <QList>
#include <QObject>
#include <QString>

#include "HypercubeData.h"

class BatchReader : public QObject {
    Q_OBJECT
public:
    explicit BatchReader(QObject *parent = nullptr);

    // Чтение всех batch-файлов из папки experimentDir, возврат списка всех
    // кадров progressPercent – от 0 до 50 (доля от общего прогресса
    // конвертации)
    QList<FrameDescriptor> readAllBatches(const QString &experimentDir,
                                          int &progressPercent);

signals:
    void progressUpdate(
        int percent);  // 0..100 (в рамках работы BatchReader, будет 0..50)
    void error(const QString &message);

private:
    QByteArray unpackMono12p(const QByteArray &packed, int width, int height);
};

#endif  // BATCHREADER_H
