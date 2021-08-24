#ifndef UART901_H
#define UART901_H
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<termios.h>
#include<string.h>
#include<sys/time.h>
#include<time.h>
#include<sys/types.h>
#include<errno.h>
#include <QDebug>
#include <QObject>

class Uart901 : public QObject
{
    Q_OBJECT
public:
    explicit Uart901(char *uartdev, int buadrate, QObject *parent = nullptr);
    void mainloop();
signals:
    void AngleData(float Angle0,float Angle1, float Angle2,float heading);
public slots:

private:
    int ret;
    int fd;
    float a[3],w[3],Angle[3],h[3], heading;
    int uart_open(int fd,const char *pathname);
    int uart_close(int fd);
    int uart_set(int fd,int nSpeed, int nBits, char nEvent, int nStop);
    int send_data(int  fd, char *send_buffer,int length);
    int recv_data(int fd, char* recv_buffer,int length);
    void ParseData(char chr);
};

#endif // UART901_H