#ifndef CUBEBUILDER_H
#define CUBEBUILDER_H

#include <QObject>

class CubeBuilder : public QObject
{
    Q_OBJECT
public:
    explicit CubeBuilder(QObject *parent = nullptr);

signals:

};

#endif // CUBEBUILDER_H
