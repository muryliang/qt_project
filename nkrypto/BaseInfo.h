#pragma once
#ifndef BASEINFO_INFO
#define BASEINFO_INFO

#include "nkrypto.h"

class BaseInfo : public QObject{
public:

    BaseInfo() {}

    virtual ~BaseInfo() {}

    virtual  Nkrypto_Status init() = 0;

    virtual  Nkrypto_Status open() = 0;

    virtual  Nkrypto_Status close() = 0;

    virtual  Nkrypto_Status exit() = 0;

    // reset internal status, after call this, one should start calling init again
    virtual  Nkrypto_Status reset() = 0;

    // only used in har-usb-150, others should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getImage(char *recvBuf, int *size) = 0;

    // only used in har-usb-150, should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getSerialNum(char *recvBuf, int *size) = 0;

    // only used in har-usb-150, others should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getDeviceSerialNum(char *recvBuf, int *size) = 0;

    // only used in har-usb-150, others should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getDeviceVersion(char *recvBuf, int *size) = 0;
};

#endif
