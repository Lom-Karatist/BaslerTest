#ifndef CALIBRATIONLOADER_H
#define CALIBRATIONLOADER_H

#include <QObject>

class CalibrationLoader : public QObject
{
    Q_OBJECT
public:
    explicit CalibrationLoader(QObject *parent = nullptr);

signals:

};

#endif // CALIBRATIONLOADER_H
