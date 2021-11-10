#ifndef UART_H
#define UART_H
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

class UART
{
public:
    UART(char *uartdev, int buadrate);
    int uart_open(const char *pathname);
    int uart_close();
    int uart_set(int nSpeed, int nBits, char nEvent, int nStop);
    int send_data(char *send_buffer,int length);
    int recv_data(char* recv_buffer,int length);
private:
    int ret;
    int fd;
};

#endif // UART_H
