#ifndef GETUSBWIFIINFO_H
#define GETUSBWIFIINFO_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <libusb-1.0/libusb.h>
#include <QMutex>
#include <mutex>          // std::mutex
#define VENDOR_USBWIFI          0x0483
#define PRODUCT_USBWIFI         0x5868

#define BREQUESTMODULESTATUS1   0xE6
#define WLENGTHMODULESTATUS1    0x3D
#define BREQUESTMODULESTATUS2   0xE7
#define WLENGTHMODULESTATUS2    0x08

struct rf_bb_status
{
    quint32 rf_freq;
    quint32 g_ldpc_pass; // ground # of FEC correct codeword
    quint32 g_ldpc_failed; // ground # of FEC decoding errors
    qint32 rf_power; // ground TX power
    quint32 a_ldpc_pass; // air # of FEC correct codeword
    quint32 a_ldpc_failed; // air # of FEC decoding errors
    unsigned char downlink; // downlink throughput
    unsigned char bb_bw; // bandwidth
    unsigned char freq_hop; // frequency hopping enable/disable
    unsigned char bind_mode; // bind/unbind
    unsigned char tdd_ratio; // downlink/uplink time ratio
    unsigned char com_baudrate; // serial port baud rate
    unsigned char g_SNR; // ground SNR
    unsigned char g_VGA; // ground vga gain
    unsigned char g_RSSI; // ground antenna0 RSSI
    unsigned char a_SNR; // air SNR
    unsigned char a_VGA; // air vga gain
    unsigned char a_RSSI; // air antenna0 RSSI
    quint32 grd_link_status; // downlink status
    quint32 air_link_status; // uplink status
    quint32 air_tx_power; // air TX power
    quint32 range; // ranging distance
    quint32 eth_lost_packet; // air ethernet packet loss on MII
    quint32 eth_rev_packet; // air ethernet packet recved on MII
    unsigned char antenna_status; // bit7~bit4(0:antenna0,1:antenna1) bit3~bit0(1:enable,0disable)
    unsigned char rc_type; // 0-sbus,1-ppm

    qint8 air_bb_temp; // air baseband temperature
    qint8 air_rf_temp; // air radio temperature
    qint8 grd_bb_temp;// ground baseband temperature
    qint8 grd_rf_temp; // ground radio temperature
    quint8 air_rssi1; //air antenna1 RSSI
    quint8 grd_rssi1; //ground antenna1 RSSI
    quint8 config_status; /*bit0(reserved)
                                bit1(0: observer,1: pilot)
                                bit2(0: disable enc,1:enable enc)
                                bit3(reserved)
                                bit4(uplinkmode,0: BPSK 2/3, 1:16QAM 1/2)
                            */

};

class GetUSBWiFiInfo : public QObject
{
    Q_OBJECT
public:
    explicit GetUSBWiFiInfo(QObject *parent = nullptr);
    void checkUSBConnected();


signals:
    void moduleStatusChanged(rf_bb_status moduleStatus);
public slots:


private:
    enum devices
    {
        DEVICE_USBWIFI
    } device_found;

    rf_bb_status new_rf_bb_status;

    bool m_IsEventThreadRunning = false;
    QMutex mtx;
    libusb_device **devices;
    libusb_context *usb_context;
    struct libusb_device_handle *device_handle;

    int init_libusb();
    libusb_device *find_device();
    void free_libusb();
    int open_device(libusb_device* device);
    void close_device();
    int getModulestatus (libusb_device_handle *hDeviceHandle,
                         unsigned char bmRequestType,
                         unsigned char Request,
                         unsigned short Value,
                         unsigned short Index,
                         unsigned char* buffer,
                         unsigned short length,
                         unsigned long TimeOut
                         );
    void bb_status(quint8 bRequest, unsigned char *dataBuf);


};

#endif // GETUSBWIFIINFO_H
