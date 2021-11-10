#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include "GetUSBWiFiInfo.h"
#include "Uart901.h"
#include <QDebug>
#include <math.h>
#include <cmath>
#define earthRadiusKm 6371.0
#define ACCEL_IIO_DEV           "/sys/bus/iio/devices/iio:device0/"
#define MAGN_IIO_DEV            "/sys/bus/iio/devices/iio:device2/"
#define ACCEL_IIO_NAME          "lsm303dlhc_accel"
#define MAGN_IIO_NAME           "lsm303dlhc_magn"

class MainWindow : public QObject
{
    Q_OBJECT
public:
    explicit MainWindow(QObject *parent = nullptr);


signals:

public slots:
//    void wifiSignalInfo();
    void moduleStatusChanged(rf_bb_status moduleStatus);
    void AngleData1(float Angle0,float Angle1, float Angle2,float heading);
    void AngleData2(float Angle0,float Angle1, float Angle2,float heading);
private:
    GetUSBWiFiInfo *usbWiFiInfo;
    Uart901 *getPitchRollYaw1;
    Uart901 *getPitchRollYaw2;

    static void* ThreadUSBWiFiFunc( void* pTr );
    typedef void * (*THREADFUNC1)(void *);
    pthread_t idUSBWiFiThread;


    static void* ThreadPitchRollYaw1( void* pTr );
    typedef void * (*THREADFUNC2)(void *);
    pthread_t idPitchRollYawThread1;

    static void* ThreadPitchRollYaw2( void* pTr );
    typedef void * (*THREADFUNC3)(void *);
    pthread_t idPitchRollYawThread2;


    double deg2rad(double deg);
    double rad2deg(double rad);
    double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d);
    double degreePitchEarth(double distance, double height);
    double degreeYawEarth(double lat1d, double lon1d, double lat2d, double lon2d);
};

#endif // MAINWINDOW_H
