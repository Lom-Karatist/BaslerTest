#include "BatchSaver.h"

#include <QDateTime>
#include <QMetaObject>

BatchSaver::BatchSaver(const QString &basePath, int maxFramesPerBatch,
                       qint64 maxBufferBytesPerBatch, QObject *parent)
    : QObject(parent),
      m_basePath(basePath),
      m_maxFramesPerBatch(maxFramesPerBatch),
      m_maxBufferBytesPerBatch(maxBufferBytesPerBatch),
      m_buffersCount(0),
      m_buffersWritten(0) {
    m_writer = new BatchWriter(basePath, maxFramesPerBatch,
                               maxBufferBytesPerBatch, nullptr);
    m_writer->moveToThread(&m_writerThread);
    qDebug() << "writer was added to thread";

    connect(m_writer, &BatchWriter::errorOccurred, this,
            &BatchSaver::onWriterError, Qt::QueuedConnection);
    connect(m_writer, &BatchWriter::fileWritten, this,
            &BatchSaver::onWriterFileWritten, Qt::QueuedConnection);

    m_writerThread.start();

    moveToThread(&m_saverThread);
    qDebug() << "saver was added to thread";
    m_saverThread.start();

    qRegisterMetaType<BaslerConstants::FrameData>();
    qRegisterMetaType<QVector<BaslerConstants::FrameData>>();
}

BatchSaver::~BatchSaver() {
    m_saverThread.quit();
    m_saverThread.wait();

    QMetaObject::invokeMethod(m_writer, "shutdown", Qt::QueuedConnection);
    m_writerThread.quit();
    m_writerThread.wait();
    delete m_writer;
}

void BatchSaver::addFrame(const QString &prefix, int width, int height,
                          QString pixelFormat, const QByteArray &data,
                          qint64 timestampMs) {
    Buffer *buf = (prefix == "HS") ? &m_bufferHS : &m_bufferOC;
    if (!buf->active) {
        buf->frames.clear();
        buf->totalBytes = 0;
        buf->width = width;
        buf->height = height;
        buf->pixelFormat = pixelFormat;
        buf->prefix = prefix;
        buf->active = true;
    } else if (buf->width != width || buf->height != height ||
               buf->pixelFormat != pixelFormat) {
        emit errorOccurred(
            QString("Parameter mismatch for %1, discarding frame").arg(prefix));
        return;
    }
    buf->frames.append({data, timestampMs});
    buf->totalBytes += data.size();

    if (buf->frames.size() >= m_maxFramesPerBatch ||
        buf->totalBytes >= m_maxBufferBytesPerBatch) {
        m_buffersCount++;
        sendBuffer(prefix, *buf);
        qDebug() << "[SEND]" << QTime::currentTime().toString("hh::mm::ss.zzzz")
                 << m_buffersCount;
        buf->active = false;
    }
}

void BatchSaver::flush() {
    if (m_bufferHS.active) {
        sendBuffer("HS", m_bufferHS);
        m_bufferHS.active = false;
    }
    if (m_bufferOC.active) {
        sendBuffer("OC", m_bufferOC);
        m_bufferOC.active = false;
    }
}

void BatchSaver::sendBuffer(const QString &prefix, Buffer &buf) {
    if (buf.frames.isEmpty()) return;

    QVector<BaslerConstants::FrameData> frames;
    frames.reserve(buf.frames.size());
    for (const auto &f : buf.frames) {
        frames.append({prefix, buf.width, buf.height, buf.pixelFormat, f.data,
                       f.timestampMs});
    }

    QMetaObject::invokeMethod(
        m_writer, "writeBatch", Qt::QueuedConnection, Q_ARG(QString, prefix),
        Q_ARG(QVector<BaslerConstants::FrameData>, frames));

    buf.frames.clear();
    buf.totalBytes = 0;
}

void BatchSaver::onWriterError(const QString &err) { emit errorOccurred(err); }

void BatchSaver::onWriterFileWritten(const QString &binFile, int count,
                                     const QString &prefix) {
    m_buffersWritten++;
    emit fileWritten(binFile, count, prefix);
    qDebug() << "[WRITTEN]" << QTime::currentTime().toString("hh::mm::ss.zzzz")
             << m_buffersWritten;
}

void BatchSaver::onWriterFlushed() { emit flushed(); }
