#include "GetUSBWiFiInfo.h"
GetUSBWiFiInfo::GetUSBWiFiInfo(QObject *parent) : QObject(parent)
{
    m_IsEventThreadRunning = true;
    if (init_libusb())
            exit(1);
    else
        qDebug() << "init libusb";
}

int GetUSBWiFiInfo::init_libusb()
{
    int usb_errno = libusb_init(&usb_context);

    if (usb_errno < 0)
    {
        printf("Failed initializing libusb %s", libusb_error_name(usb_errno));
        return usb_errno;
    }

    libusb_set_debug(usb_context, 3);

    return 0;
}
libusb_device* GetUSBWiFiInfo::find_device()
{
    // return value
    libusb_device* r = NULL;

    int usb_errno = 0;

    // number of devices found
    ssize_t cnt;

    cnt = libusb_get_device_list(usb_context, &devices);

//    qDebug() << "usb number of devices found" << cnt;
    for (ssize_t i = 0; i < cnt; i++)
    {
        struct libusb_device_descriptor desc;
        usb_errno = libusb_get_device_descriptor(devices[i], &desc);
        if (usb_errno < 0)
        {
            printf("Failed getting device descriptor of device %zd %s\n", i, libusb_error_name(usb_errno));
        }

        if (desc.idVendor == VENDOR_USBWIFI && desc.idProduct == PRODUCT_USBWIFI)
        {
            r = devices[i];
//            printf("Found USB!\n");
            device_found = DEVICE_USBWIFI;
            break;
        }
    }
    return r;
}


void GetUSBWiFiInfo::free_libusb()
{
    libusb_exit(usb_context);
}

void GetUSBWiFiInfo::bb_status(quint8 bRequest, unsigned char  *dataBuf)
{
//    qDebug() << "bRequest" << bRequest;
    int wLength = 0;
    switch (bRequest)
    {
    case BREQUESTMODULESTATUS1:
        wLength = WLENGTHMODULESTATUS1;
        new_rf_bb_status.rf_freq        = (dataBuf[3] << 24) | (dataBuf[2] << 16) | (dataBuf[1] << 8) | (dataBuf[0] << 0);
        new_rf_bb_status.g_ldpc_pass    = (dataBuf[7] << 24) | (dataBuf[6] << 16) | (dataBuf[5] << 8) | (dataBuf[4] << 0);
        new_rf_bb_status.g_ldpc_failed  = (dataBuf[11] << 24) | (dataBuf[10] << 16) | (dataBuf[9] << 8) | (dataBuf[8] << 0);
        new_rf_bb_status.rf_power       = (dataBuf[15] << 24) | (dataBuf[14] << 16) | (dataBuf[13] << 8) | (dataBuf[12] << 0);
        new_rf_bb_status.a_ldpc_pass    = (dataBuf[19] << 24) | (dataBuf[18] << 16) | (dataBuf[17] << 8) | (dataBuf[16] << 0);
        new_rf_bb_status.a_ldpc_failed  = (dataBuf[23] << 24) | (dataBuf[22] << 16) | (dataBuf[21] << 8) | (dataBuf[20] << 0);
        new_rf_bb_status.downlink       =  dataBuf[24];
        new_rf_bb_status.bb_bw          =  dataBuf[25];
        new_rf_bb_status.freq_hop       =  dataBuf[26];
        new_rf_bb_status.bind_mode      =  dataBuf[27];
        new_rf_bb_status.tdd_ratio      =  dataBuf[28];
        new_rf_bb_status.com_baudrate   =  dataBuf[29];
        new_rf_bb_status.g_SNR          =  dataBuf[30];
        new_rf_bb_status.g_VGA          =  dataBuf[31];
        new_rf_bb_status.g_RSSI         =  dataBuf[32];
        new_rf_bb_status.a_SNR          =  dataBuf[33];
        new_rf_bb_status.a_VGA          =  dataBuf[34];
        new_rf_bb_status.a_RSSI         =  dataBuf[35];
        new_rf_bb_status.grd_link_status= (dataBuf[39] << 24) | (dataBuf[38] << 16) | (dataBuf[37] << 8) | (dataBuf[36] << 0);
        new_rf_bb_status.air_link_status= (dataBuf[43] << 24) | (dataBuf[42] << 16) | (dataBuf[41] << 8) | (dataBuf[40] << 0);
        new_rf_bb_status.air_tx_power   = (dataBuf[47] << 24) | (dataBuf[46] << 16) | (dataBuf[45] << 8) | (dataBuf[44] << 0);
        new_rf_bb_status.range          = (dataBuf[51] << 24) | (dataBuf[50] << 16) | (dataBuf[49] << 8) | (dataBuf[48] << 0);
        new_rf_bb_status.eth_lost_packet= (dataBuf[55] << 24) | (dataBuf[54] << 16) | (dataBuf[53] << 8) | (dataBuf[52] << 0);
        new_rf_bb_status.eth_rev_packet = (dataBuf[59] << 24) | (dataBuf[58] << 16) | (dataBuf[57] << 8) | (dataBuf[56] << 0);
        new_rf_bb_status.antenna_status =  dataBuf[60];
        new_rf_bb_status.rc_type        =  dataBuf[61];

    break;
    case BREQUESTMODULESTATUS2:
        wLength = WLENGTHMODULESTATUS2;
        new_rf_bb_status.air_bb_temp    =  dataBuf[0];
        new_rf_bb_status.air_rf_temp    =  dataBuf[1];
        new_rf_bb_status.grd_bb_temp    =  dataBuf[2];
        new_rf_bb_status.grd_rf_temp    =  dataBuf[3];
        new_rf_bb_status.air_rssi1      =  dataBuf[4];
        new_rf_bb_status.grd_rssi1      =  dataBuf[5];
        new_rf_bb_status.config_status  =  dataBuf[6];
        emit moduleStatusChanged(new_rf_bb_status);
        break;
    default:
        break;
    }


}

int GetUSBWiFiInfo::getModulestatus (libusb_device_handle *hDeviceHandle,
                                     unsigned char bmRequestType,
                                     unsigned char Request,
                                     unsigned short Value,
                                     unsigned short Index,
                                     unsigned char* buffer,
                                     unsigned short length,
                                     unsigned long TimeOut
                                     )
{
    libusb_control_setup			ctrl_setup_packet;
    int                     	cbSent = 0;
    //bool						bResult;


    if (hDeviceHandle == nullptr)
    {
        return -1;
    }

    mtx.lock();
    memset(&ctrl_setup_packet,0,sizeof(ctrl_setup_packet));

    ctrl_setup_packet.bmRequestType = bmRequestType;
    ctrl_setup_packet.bRequest = Request;
    ctrl_setup_packet.wIndex = Index;
    ctrl_setup_packet.wValue = Value;
    ctrl_setup_packet.wLength = length;

    cbSent =  libusb_control_transfer(hDeviceHandle,
                                      ctrl_setup_packet.bmRequestType,
                                      ctrl_setup_packet.bRequest,
                                      ctrl_setup_packet.wValue,
                                      ctrl_setup_packet.wIndex,
                                      buffer,
                                      ctrl_setup_packet.wLength,
                                      TimeOut
                                      );
    mtx.unlock();
    return cbSent;

}

int GetUSBWiFiInfo::open_device(libusb_device* device)
{
    int usb_errno = libusb_open(device, &device_handle);
    unsigned char buffer[0x3d];
    if (usb_errno < 0)
    {
        fprintf(stderr, "Failed to open USB device %s\n", libusb_error_name(usb_errno));
        return 1;
    }

    usb_errno = libusb_set_auto_detach_kernel_driver(device_handle, 1);
    if (usb_errno < 0)
    {
        printf("Failed to call auto detach USB device %s\n", libusb_error_name(usb_errno));
        return 1;
    }

    usb_errno = libusb_claim_interface(device_handle, 3);

    int ret = getModulestatus(device_handle,0xc1,BREQUESTMODULESTATUS1,0x00,0x00,buffer,WLENGTHMODULESTATUS1,1000);
//    qDebug() << ret ;
    if (ret == WLENGTHMODULESTATUS1)
    {
        bb_status(BREQUESTMODULESTATUS1,buffer);
    }
    ret = getModulestatus(device_handle,0xc1,BREQUESTMODULESTATUS2,0x00,0x00,buffer,WLENGTHMODULESTATUS2,1000);
//    qDebug() << ret ;
    if (ret == WLENGTHMODULESTATUS2)
    {
        bb_status(BREQUESTMODULESTATUS2,buffer);
    }
    return 0;
}

void GetUSBWiFiInfo::close_device()
{
//    if (libusb_release_interface(device_handle, 3) != 0)
//        printf("Failed releasing interface\n");

    libusb_close(device_handle);
}

void GetUSBWiFiInfo::checkUSBConnected()
{
    while (m_IsEventThreadRunning)
    {
        QThread::msleep(1000);
        libusb_device* usb_device = NULL;
        usb_device = find_device();
        if (!usb_device)
        {
            qDebug() << "Couldn't find device";
//            free_libusb();
        }

        if (usb_device)
        {
            if (open_device(usb_device))
            {
                printf("Couldn't claim Device");
                close_device();
    //            free_libusb();
            }
            close_device();
        }
    }
}

