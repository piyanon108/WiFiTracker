#include "Uart901.h"
#include <QThread>
Uart901::Uart901(char *uartdev, int buadrate, QObject *parent) : QObject(parent)
{
    uart = new UART(uartdev,buadrate);
    uartDev = uartdev;
}

void Uart901::mainloop(char *recfile)
{
    char r_buf[1024];
    bzero(r_buf,1024);
    FILE *fp;
    fp = fopen(recfile,"w");
    while(1)
    {
        ret = uart->recv_data(r_buf,12);
        if(ret == -1)
        {
            fprintf(stderr,"uart read failed!\n");
            exit(EXIT_FAILURE);
        }
        for (int i=0;i<ret;i++)
        {
//            fprintf(fp,"%2X ",r_buf[i]);
            ParseData(r_buf[i]);
        }
        QThread::msleep(1);
    }

    ret = uart->uart_close();
    if(ret == -1)
    {
        fprintf(stderr,"uart_close error\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void Uart901::ParseData(char chr)
{
    char chrBuf[100];

    signed short sData[4];
    unsigned char i;
    char cTemp=0;
    time_t now;
    chrBuf[chrCnt++]=chr;
//    printf("%d %s %x %x %x %x %x %x %x %x %x %x %x %x\r\n",chrCnt, uartDev, chrBuf[0],chrBuf[1], chrBuf[2],chrBuf[3],chrBuf[4],chrBuf[5],chrBuf[6],chrBuf[7], chrBuf[8],chrBuf[9],chrBuf[10],chrBuf[11]);
    if (chrCnt<11) return;
    for (i=0;i<10;i++) cTemp+=chrBuf[i];
    if ((chrBuf[0]!=0x55)||((chrBuf[1]&0x50)!=0x50)||(cTemp!=chrBuf[10]))
    {
        printf("%s Error:%x %x chrCnt:%d\r\n",uartDev, chrBuf[0],chrBuf[1],chrCnt);
        memcpy(&chrBuf[0],&chrBuf[1],10);
        chrCnt--;
        return;
    }

    memcpy(&sData[0],&chrBuf[2],8);
    switch(chrBuf[1])
    {
            case 0x51:
//                    for (i=0;i<3;i++) a[i] = (float)sData[i]/32768.0*16.0;
//                    time(&now);
//                    printf("\r\nT:%s a:%6.3f %6.3f %6.3f ",asctime(localtime(&now)),a[0],a[1],a[2]);

                break;
            case 0x52:
//                    for (i=0;i<3;i++) w[i] = (float)sData[i]/32768.0*2000.0;
//                    printf("w:%7.3f %7.3f %7.3f ",w[0],w[1],w[2]);
                break;
            case 0x53:
                for (i=0;i<3;i++) Angle[i] = (float)sData[i]/32768.0*180.0;
                if (Angle[2]<0) heading = Angle[2]*(-1);
                else if (Angle[2]>0) heading = 360 - Angle[2];
                emit AngleData(Angle[0],Angle[1],Angle[2],heading);
//                qDebug() << "Pitch:" << Angle[0] << "Roll:" << Angle[1] << "Yaw:" << Angle[2] << "Heading" << heading;
//                    printf("A:%7.3f %7.3f %7.3f ",Angle[0],Angle[1],Angle[2]);
                break;
            case 0x54:
//                    for (i=0;i<3;i++) h[i] = (float)sData[i];
//                    printf("h:%4.0f %4.0f %4.0f ",h[0],h[1],h[2]);

                break;
    }
    chrCnt=0;
}
