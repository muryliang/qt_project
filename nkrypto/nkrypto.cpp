// nkrypto.cpp : 定义 DLL 应用程序的导出函数。
//

#include "nkrypto.h"
#include "HarUsb150Info.h"

static BaseInfo* factoryCreateInfo(int devId);

static BaseInfo *baseInfo = NULL;

/*
 * v1.0 first
 * v1.1 update decoder dll and security dll
 */
static std::string sdkVer = "v1.1";


// error status should modify to more specific
// should move machine state check here ??
Nkrypto_Status Nkrypto_Init(int devId) {
    if (baseInfo) {
        baseInfo->reset();
        baseInfo = NULL;
    }
    baseInfo = factoryCreateInfo(devId);
    if (baseInfo) {
        return baseInfo->init();
    }
    return E_NOTIMPLEMENTED;
}

Nkrypto_Status Nkrypto_Open() {
    if (baseInfo) return baseInfo->open();
    return E_NOTIMPLEMENTED;
}

Nkrypto_Status Nkrypto_Close() {
    if (baseInfo) return baseInfo->close();
    return E_NOTIMPLEMENTED;
}

Nkrypto_Status Nkrypto_Exit() {
    if (baseInfo) return baseInfo->exit();
    return E_NOTIMPLEMENTED;
}

Nkrypto_Status Nkrypto_GetImageData(char* recvBuf, int *size) {
    if (baseInfo) return baseInfo->getImage(recvBuf, size);
    return E_NOTIMPLEMENTED;  // should return not implemented
}
Nkrypto_Status Nkrypto_GetSerialNum(char *recvBuf, int *size) {
    if (baseInfo) return baseInfo->getSerialNum(recvBuf, size);
    return E_NOTIMPLEMENTED;
}

Nkrypto_Status Nkrypto_DeviceSerialNum(char *recvBuf, int *size) {
    if (baseInfo) return baseInfo->getDeviceSerialNum(recvBuf, size);
    return E_NOTIMPLEMENTED;
}

Nkrypto_Status Nkrypto_DeviceVersion(char *recvBuf, int *size) {
    if (baseInfo) return baseInfo->getDeviceVersion(recvBuf, size);
    return E_NOTIMPLEMENTED;
}

Nkrypto_Status Nkrypot_SdkVersion(char *buf, int *size) {
    if (buf == NULL || size == NULL || *size < (int)sdkVer.size()) {
        dprintf("buffer null or size not enough\n");
        if (size) *size = sdkVer.size();
        return E_SMALL;
    }
    memcpy(buf, sdkVer.c_str(), sdkVer.size());
    *size = sdkVer.size();
    return E_OK;
}

// factory function create info class according to devId
// currently we can only enable one device one time
static BaseInfo* factoryCreateInfo(int devId) {
    baseInfo = NULL;
    switch (devId) {
    case HAR_USB_150:
        dprintf("we are in 150 creation\n");
        baseInfo = new HarUsb150Info();
        break;
    default:
        dprintf("do not support devId %d\n", devId);
        break;
    }
    return baseInfo;
}


