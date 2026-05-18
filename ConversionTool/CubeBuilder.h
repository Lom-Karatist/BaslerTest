#ifndef CUBEBUILDER_H
#define CUBEBUILDER_H

#include <QList>
#include <QObject>

#include "HypercubeData.h"

class CubeBuilder : public QObject {
    Q_OBJECT
public:
    explicit CubeBuilder(QObject *parent = nullptr);

    // Принимает список всех кадров, строит гиперкуб в порядке BSQ.
    // progressPercent – доля от 50 до 100 (второй этап конвертации)
    HypercubeData buildCube(const QList<FrameDescriptor> &frames,
                            const QString &calibrationFilePath,
                            int &progressPercent);

signals:
    void progressUpdate(int percent);
    void error(const QString &message);

private:
    void setLinearWavelengths(HypercubeMetadata &meta, int bands);
};

#endif  // CUBEBUILDER_H
