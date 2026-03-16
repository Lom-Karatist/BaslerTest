#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#pragma once

#include <QObject>
#include <QImage>
#include <QDebug>
//#include <pylon/PylonIncludes.h>
//#include <pylon/BaslerUniversalInstantCamera.h>

//using namespace Pylon;
//using namespace Basler_UniversalCameraParams;


class CameraManager : public QObject
{
    Q_OBJECT
public:
    explicit CameraManager(QObject *parent = nullptr);
    ~CameraManager();

    bool openCamera();
    void closeCamera();
    bool grabOneFrame();

signals:
   void frameGrabbed(int width, int height, int pixelSize, quint64 timestamp);
   void errorOccurred(const QString &message);

private:
   CBaslerUniversalInstantCamera *m_camera;
   bool m_isOpen;

};

#endif // CAMERAMANAGER_H
