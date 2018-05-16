#ifndef HARUSB150INFO_H
#define HARUSB150INFO_H

#include <string>

#include <QThread>
#include <QTimer>
#include <QCoreApplication>

#include "BaseInfo.h"
#include "SnapShot.h"
#include "CameraApi.h"

#define VERSTRSIZE 33 // according to sdk document, must > 32

//thread runing capture thread
class CaptureThread : public QThread {
    Q_OBJECT
protected:
    // maybe we can use a signal to tell thread to stop
    void run() override;
};

class AlarmThread : public QThread {
    Q_OBJECT
private:
    QCoreApplication *qptr = NULL;
protected:
    // we run event loop in it , create coreapplication in it
public:
    AlarmThread();
    ~AlarmThread();
    void run() override;
};

class Trigger : public QObject {
    Q_OBJECT
public slots:
    void setUpTrigger();
};

class HarUsb150Info : public BaseInfo {
    Q_OBJECT
private:
    //function state, use enum
    enum {
        PREINIT, INIT, OPEN, CLOSE, EXIT
    };

    // vars for camera
    tSdkCameraCapbility     g_tCapability;      //设备描述信息
    std::string snNum, verNum;

     // used to store SnapShot  object
    SnapShot *g_info = NULL;

    // init State of functin call order
    int State = PREINIT;

    // alarm beep thread pointer
    CaptureThread *pCapture = NULL;
    AlarmThread *pAlarm = NULL;
    Trigger *triggerClass = NULL;

    // private funcs
    void setHardTrigger();
    void setGpioMode();    

    Nkrypto_Status Nkrypto_GetInfo();
    Nkrypto_Status Nkrypto_CreateInfo(char *data, int row, int col, int channel);
    Nkrypto_Status Nkrypto_DeleteInfo();
    Nkrypto_Status cameraSet();

public :
    virtual Nkrypto_Status init() override;

    virtual Nkrypto_Status open() override;

    virtual  Nkrypto_Status close() override;

    virtual  Nkrypto_Status exit() override;

    // reset internal status, after call this, one should start calling init again
    virtual  Nkrypto_Status reset() override;

    // only used in har-usb-150, others should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getImage(char *recvBuf, int *size) override;

    // only used in har-usb-150, should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getSerialNum(char *recvBuf, int *size) override;

    // only used in har-usb-150, others should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getDeviceSerialNum(char *recvBuf, int *size) override;

    // only used in har-usb-150, others should return E_NOTIMPLEMENTED
    virtual Nkrypto_Status getDeviceVersion(char *recvBuf, int *size) override;

    virtual ~HarUsb150Info() {}
signals:
    void sendTriggerSignal();
};

#endif // HARUSB150INFO_H
