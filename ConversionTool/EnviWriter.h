#ifndef ENVIWRITER_H
#define ENVIWRITER_H

#include <QObject>

class EnviWriter : public QObject
{
    Q_OBJECT
public:
    explicit EnviWriter(QObject *parent = nullptr);

signals:

};

#endif // ENVIWRITER_H
