#ifndef BASLERAPI_H
#define BASLERAPI_H

#include <QRunnable>
#include <QObject>
#include <QImage>
#include <QElapsedTimer>
#include <QElapsedTimer>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

using namespace Pylon;
using namespace Basler_UniversalCameraParams;

/**
 * @brief The BaslerCameraParams struct includes all needed camera parameters
 */
struct BaslerCameraParams {
    double exposureTimeAbs = 10000.0; // микросекунды
    double gainRaw = 1.0;             // в условных единицах камеры
    bool isMaster = true;             // Роль камеры: мастер или слейв
    int width = 1920;
    int height = 1200;
};

/**
 * @brief The BaslerApi class executing in QThreadPool
 */
class BaslerApi : public QObject, public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief BaslerApi Constructor for camera object
     * @param isMaster  Master or Slave
     * @param params    Basler camera parameters
     * @param parent    Parent object
     */
    explicit BaslerApi(bool isMaster, const BaslerCameraParams& params,
                       const QString& serialNumber, QObject *parent = nullptr);
    ~BaslerApi();

    /**
     * @brief run   executing function
     */
    void run() override;

    void startGrabbing();
    void stopGrabbing();
    bool isGrabbing() const { return m_isGrabbing; }
    bool isConnected() const { return m_isConnected; }

    int getImageWidth() const { return m_params.width; }
    int getImageHeight() const { return m_params.height; }

signals:
    // Сигналы для связи с GUI
    void connectionComplete(bool success);
    void sendErrorMessage(const QString& error);
    void imageReceived(const QImage& image); // Для отображения
    void rawDataReceived(const QByteArray& data, int width, int height); // Для сохранения/обработки
    void cameraStats(double fps, double temperature); // Если доступно

private:
    bool initializeCamera();
    void setupCameraFeatures();
    void configureMasterSlave();

    std::atomic<bool> m_isGrabbing;
    volatile bool m_isConnected;

    bool m_isMaster;
    BaslerCameraParams m_params;

    CBaslerUniversalInstantCamera* m_camera;
    CGrabResultPtr m_ptrGrabResult;
    CImageFormatConverter m_converter; // Для конвертации в QImage
    CPylonImage m_pylonImage;

    QElapsedTimer m_fpsTimer;
    int m_frameCount;
    QString m_serialNumber;
};

#endif // BASLERAPI_H
