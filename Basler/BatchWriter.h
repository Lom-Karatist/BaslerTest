#ifndef BATCHWRITER_H
#define BATCHWRITER_H

#include <QAtomicInt>
#include <QObject>
#include <QString>
#include <QVector>

#include "Types.h"
using namespace BaslerConstants;

class BatchWriter : public QObject {
    Q_OBJECT
public:
    explicit BatchWriter(const QString &basePath, int maxFramesPerBatch = 100,
                         qint64 maxBufferBytesPerBatch = 50 * 1024 * 1024,
                         QObject *parent = nullptr);
    ~BatchWriter();

public slots:
    void writeBatch(const QString prefix,
                    const QVector<BaslerConstants::FrameData> &frames);
    void shutdown();

signals:
    void errorOccurred(const QString &error);
    void fileWritten(const QString &binFileName, int framesCount,
                     const QString &prefix);

private:
    void writeBatchImpl(const QString &prefix,
                        const QVector<FrameData> &frames);

    void writeBinary(const QString binPath, const QVector<FrameData> &frames);
    void writeHeader(const QString headerPath, QString baseName,
                     const QVector<FrameData> &frames);
    QString generateBaseName(const QString &prefix) const;

    QString m_basePath;
    int m_maxFramesPerBatch;
    qint64 m_maxBufferBytesPerBatch;
    QAtomicInt m_shutdown;
    int m_recievedCount;
};

#endif  // BATCHWRITER_H
