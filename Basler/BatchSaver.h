#ifndef BATCHSAVER_H
#define BATCHSAVER_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QThread>
#include <QVector>

#include "BatchWriter.h"
#include "Types.h"

class BatchSaver : public QObject {
    Q_OBJECT
public:
    explicit BatchSaver(const QString &basePath, int maxFramesPerBatch = 100,
                        qint64 maxBufferBytesPerBatch = 50 * 1024 * 1024,
                        QObject *parent = nullptr);
    ~BatchSaver();

public slots:
    void addFrame(const QString &prefix, int width, int height,
                  QString pixelFormat, const QByteArray &data,
                  qint64 timestampMs = 0);
    void flush();

signals:
    void errorOccurred(const QString &error);
    void fileWritten(const QString &binFileName, int framesCount,
                     const QString &prefix);
    void flushed();

private slots:
    void onWriterError(const QString &err);
    void onWriterFileWritten(const QString &binFile, int count,
                             const QString &prefix);
    void onWriterFlushed();

private:
    struct FrameDataInternal {
        QByteArray data;
        qint64 timestampMs;
    };
    struct Buffer {
        QVector<FrameDataInternal> frames;
        qint64 totalBytes = 0;
        int width = 0;
        int height = 0;
        QString pixelFormat;
        bool active = false;
        QString prefix;
    };

    void sendBuffer(const QString &prefix, Buffer &buf);

    QString m_basePath;
    int m_maxFramesPerBatch;
    qint64 m_maxBufferBytesPerBatch;

    int m_buffersCount;
    int m_buffersWritten;

    Buffer m_bufferHS;
    Buffer m_bufferOC;

    QThread m_saverThread;
    QThread m_writerThread;
    BatchWriter *m_writer;
};

#endif  // BATCHSAVER_H
