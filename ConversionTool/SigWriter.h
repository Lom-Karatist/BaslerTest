#ifndef SIGWRITER_H
#define SIGWRITER_H

#include <QObject>

class SigWriter : public QObject
{
    Q_OBJECT
public:
    explicit SigWriter(QObject *parent = nullptr);

signals:

};

#endif // SIGWRITER_H
