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
}

void MainWindow::AngleData1(float Angle0,float Angle1, float Angle2,float heading)
{
    qDebug() << "01  Pitch:" << Angle0 << "Roll:" << Angle1 << "Yaw:" << Angle2 << "Heading" << heading;
}

void MainWindow::AngleData2(float Angle0,float Angle1, float Angle2,float heading)
{
    qDebug() << "02  Pitch:" << Angle0 << "Roll:" << Angle1 << "Yaw:" << Angle2 << "Heading" << heading;
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
    pThis->getPitchRollYaw1->mainloop();
    return pThis;
}

void* MainWindow::ThreadPitchRollYaw2(void *pTr)
{
    MainWindow* pThis = static_cast<MainWindow*>(pTr);
    pThis->getPitchRollYaw2->mainloop();
    return pThis;
}

void MainWindow::moduleStatusChanged(rf_bb_status moduleStatus)
{
    qDebug() << "air rssi :" << moduleStatus.a_RSSI << moduleStatus.air_rssi1 << "gnd rssi:" << moduleStatus.g_RSSI;
}

