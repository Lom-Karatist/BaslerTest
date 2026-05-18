#ifndef CONVERSIONWORKER_H
#define CONVERSIONWORKER_H

#include <QObject>
#include "HypercubeData.h"

class ConversionWorker : public QObject {
    Q_OBJECT
public:
    explicit ConversionWorker(QObject *parent = nullptr);

public slots:
    void runConversion(const QString &experimentDir,
                       const QString &calibrationFile,
                       const QString &outputDir,
                       const QString &outputFileName,
                       const QString &outputFormat,
                       bool splitCubes,
                       bool addGeoreferencing);

signals:
    void progressUpdated(int percent);
    void finished(bool success, const QString &message);
};

#endif // CONVERSIONWORKER_H