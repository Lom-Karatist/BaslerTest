#ifndef BATCHREADER_H
#define BATCHREADER_H

#include <QObject>

class BatchReader : public QObject
{
    Q_OBJECT
public:
    explicit BatchReader(QObject *parent = nullptr);

signals:

};

#endif // BATCHREADER_H
