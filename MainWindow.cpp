#include "MainWindow.h"

MainWindow::MainWindow(QObject *parent) : QObject(parent)
{
    usbWiFiInfo = new GetUSBWiFiInfo;
    getPitchRollYaw1 = new Uart901("/dev/ttyUL1",9600);
    getPitchRollYaw2 = new Uart901("/dev/ttyUL2",9600);

    int ret;
    ret=pthread_create(&idUSBWiFiThread, NULL, ThreadUSBWiFiFunc, this);
    if(ret==0){
        qDebug() <<("ThreadUSBWiFiFunc created successfully.\n");
    }
    else{
        qDebug() <<("ThreadUSBWiFiFunc not created.\n");
    }

    ret=pthread_create(&idPitchRollYawThread1, NULL, ThreadPitchRollYaw1, this);
    if(ret==0){
        qDebug() <<("ThreadPitchRollYaw1 created successfully.\n");
    }
    else{
        qDebug() <<("ThreadPitchRollYaw1 not created.\n");
    }

    ret=pthread_create(&idPitchRollYawThread2, NULL, ThreadPitchRollYaw2, this);
    if(ret==0){
        qDebug() <<("ThreadPitchRollYaw2 created successfully.\n");
    }
    else{
        qDebug() <<("ThreadPitchRollYaw2 not created.\n");
    }

    qRegisterMetaType<rf_bb_status>("rf_bb_status");
    connect(usbWiFiInfo,SIGNAL(moduleStatusChanged(rf_bb_status)),this,SLOT(moduleStatusChanged(rf_bb_status)));
    connect(getPitchRollYaw1,SIGNAL(AngleData(float ,float , float ,float )),this,SLOT(AngleData1(float ,float , float ,float )));
    connect(getPitchRollYaw2,SIGNAL(AngleData(float ,float , float ,float )),this,SLOT(AngleData2(float ,float , float ,float )));

    qDebug() << distanceEarth(13.765525,100.543142, 15.304474, 103.712097) << "km.";
    qDebug() << degreePitchEarth(16,1) << "degree";
    qDebug() << degreeYawEarth(13.765525,100.543142, 15.304474, 103.712097) << "degree(N)";
//    exit(0);
}

void MainWindow::AngleData1(float Angle0,float Angle1, float Angle2,float heading)
{
//    qDebug() << "01  Pitch:" << Angle0 << "Roll:" << Angle1;
}

void MainWindow::AngleData2(float Angle0,float Angle1, float Angle2,float heading)
{
    qDebug() << "Yaw:" << Angle2 << "Heading" << heading;
}

void* MainWindow::ThreadUSBWiFiFunc(void *pTr)
{
    MainWindow* pThis = static_cast<MainWindow*>(pTr);
    pThis->usbWiFiInfo->checkUSBConnected();
    return pThis;
}

void* MainWindow::ThreadPitchRollYaw1(void *pTr)
{
    MainWindow* pThis = static_cast<MainWindow*>(pTr);
    pThis->getPitchRollYaw1->mainloop("/tmp/rec1.txt");
    return pThis;
}

void* MainWindow::ThreadPitchRollYaw2(void *pTr)
{
    MainWindow* pThis = static_cast<MainWindow*>(pTr);
    pThis->getPitchRollYaw2->mainloop("/tmp/rec2.txt");
    return pThis;
}

void MainWindow::moduleStatusChanged(rf_bb_status moduleStatus)
{
    qDebug() << "air rssi :" << moduleStatus.a_RSSI << moduleStatus.air_rssi1 << "gnd rssi:" << moduleStatus.g_RSSI;
}

// This function converts decimal degrees to radians
double MainWindow::deg2rad(double deg) {
  return (deg * M_PI / 180);
}

//  This function converts radians to decimal degrees
double MainWindow::rad2deg(double rad) {
  return (rad * 180 / M_PI);
}

/**
 * Returns the distance between two points on the Earth.
 * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 * @param lat1d Latitude of the first point in degrees
 * @param lon1d Longitude of the first point in degrees
 * @param lat2d Latitude of the second point in degrees
 * @param lon2d Longitude of the second point in degrees
 * @return The distance between the two points in kilometers
 */
double MainWindow::distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d)
{
    double lat1r, lon1r, lat2r, lon2r, u, v;
    lat1r = deg2rad(lat1d);
    lon1r = deg2rad(lon1d);
    lat2r = deg2rad(lat2d);
    lon2r = deg2rad(lon2d);
    u = sin((lat2r - lat1r)/2);
    v = sin((lon2r - lon1r)/2);
    return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

double MainWindow::degreePitchEarth(double distance, double height)
{
    return atan2(height,distance) * (180 / M_PI);
}

double MainWindow::degreeYawEarth(double lat1d, double lon1d, double lat2d, double lon2d)
{
    double northDegree = (atan2(lat2d-lat1d,lon2d-lon1d) * (180 / M_PI));
    if (northDegree < 0)
    {
        northDegree = (northDegree*(-1)) + 90.0;
    }
    else if ((northDegree <= 90) & (northDegree > 0))
    {
        northDegree = 90 - northDegree;
    }
    else if (northDegree > 90)
    {
         northDegree = 450 - northDegree;
    }
    return northDegree;
}


