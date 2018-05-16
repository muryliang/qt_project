#include "HarUsb150Info.h"
#include "SnapShot.h"

namespace nk {
    static int                      g_hCamera = -1;
    static tSdkFrameHead 			g_FrameInfo;
    static unsigned char           * g_pRawBuffer = NULL;     //raw数据
    static BYTE                    * g_pRgbBuffer = NULL;     //处理后数据缓存区

    // flag for device plugged in detect
    static int imageSet = 0;  // 0 at start, 1 is set, 2 not set(maybe device not connect, should re init)

    // vars for thread
    static HANDLE hMutex = NULL;
    static bool quitThread = false;   // used in thread loop check , in close, should set to false
} // end of namespace

void HarUsb150Info::setHardTrigger() {
    CameraSetTriggerMode(nk::g_hCamera, 2);
    CameraSetTriggerCount(nk::g_hCamera, 1);
}

void HarUsb150Info::setGpioMode() {
    CameraSetOutPutIOMode(nk::g_hCamera, 0, 3);
    CameraSetIOState(nk::g_hCamera, 0, 0); // default to 0
}

// reset state to preinit state
Nkrypto_Status HarUsb150Info::reset() {
    Nkrypto_Status res = E_OK;
    if (State == PREINIT) return res;
    if (State == INIT) res = Nkrypto_Exit();
    if (State == OPEN) res = Nkrypto_Close();
    if (State == CLOSE) res = Nkrypto_Exit();
    return res;
}

Nkrypto_Status HarUsb150Info::cameraSet() {
    // set image process output format to 0~255 colors 8bit
    if (g_tCapability.sIspCapacity.bMonoSensor)
    {
        CameraSetIspOutFormat(nk::g_hCamera, CAMERA_MEDIA_TYPE_MONO8);
    }

    // set vertical mirror to true so we can get photo in correct perspective
    CameraSetMirror(nk::g_hCamera, 0, FALSE);
    CameraSetMirror(nk::g_hCamera, 1, TRUE);

    // set exposure time to 5ms
    if (CameraSetAeState(nk::g_hCamera, false) != CAMERA_STATUS_SUCCESS) {
        return E_SET;
    }
    if (CameraSetExposureTime(nk::g_hCamera, 5000) != CAMERA_STATUS_SUCCESS) {
        return E_SET;
    }

    // step is 0.0625 , so set 32 here , == 2
    if (CameraSetAnalogGain(nk::g_hCamera, 32) != CAMERA_STATUS_SUCCESS) {
        return E_SET;
    }

    // set trigger mode
    setHardTrigger();

    // set gpio mode to strobe
    setGpioMode();
    return E_OK;
}


Nkrypto_Status HarUsb150Info::init() {

    if (State != PREINIT) {
        dprintf("not in preinit state %d, reset\n", State);
        reset();
        //		return E_STATE;
    }
    int                     iCameraCounts = 4;
    int                     res = -1;
    tSdkCameraDevInfo       tCameraEnumList[4];

    // init sdk
    CameraSdkInit(1);

    // get device list
    CameraEnumerateDevice(tCameraEnumList, &iCameraCounts);
    if (iCameraCounts == 0) {
        return E_DEVNOTFOUND;
    }

    // init the first device
    res = CameraInit(&tCameraEnumList[0], -1, -1, &nk::g_hCamera);
    if (res != CAMERA_STATUS_SUCCESS) {
        return res;
    }

    // copy SNnumber and devVer
    snNum = "HAR-" + std::string(tCameraEnumList[0].acSn);
    verNum = "HAR-150-" + std::string(tCameraEnumList[0].acDriverVersion);

    //get camera info
    CameraGetCapability(nk::g_hCamera, &g_tCapability);

    dprintf("gain max %d, min %d, step %f\n", g_tCapability.sExposeDesc.uiAnalogGainMax, g_tCapability.sExposeDesc.uiAnalogGainMin, g_tCapability.sExposeDesc.fAnalogGainStep);

    // create buffer for photo data storage
    nk::g_pRgbBuffer = (BYTE *)CameraAlignMalloc(g_tCapability.sResolutionRange.iWidthMax*g_tCapability.sResolutionRange.iWidthMax * 3, 16);
    if (nk::g_pRgbBuffer == NULL) {
        dprintf("alloc image buffer fail\n");
        return E_NOMEMORY;
    }
    memset(nk::g_pRgbBuffer, 0, g_tCapability.sResolutionRange.iWidthMax*g_tCapability.sResolutionRange.iWidthMax * 3);
    memset(&nk::g_FrameInfo, 0, sizeof(nk::g_FrameInfo));

    res = cameraSet();
    if (res != E_OK) {
        return res;
    }


    // set state to init
    State = INIT;
    return E_OK;
}

Nkrypto_Status HarUsb150Info::open() {

    if (State != INIT && State != CLOSE) {
        dprintf("not call after close before exit or after init %d\n", State);
        return E_STATE;
    }
    if (CameraPlay(nk::g_hCamera) != CAMERA_STATUS_SUCCESS)
        return E_START;

    // create a mutex
    nk::hMutex = CreateMutex(NULL, FALSE, (LPCWSTR)"camera");

    // create and begin thread
    // NOTE: there is some delay between threadcreate and run, less than 1s
    nk::quitThread = false;

    if (pCapture) {
        pCapture->quit();
        pCapture = NULL;
    }

    pCapture = new CaptureThread();
    dprintf("begin capture thrad\n");
    pCapture->start();

    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer2 = new QTimer(this);
    timer2->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(setUpTrigger()));
    connect(timer2, SIGNAL(timeout()), this, SLOT(setDownTrigger()));

    // set state to open
    State = OPEN;
    return 0;
}

Nkrypto_Status HarUsb150Info::close() {

    if (State != OPEN) {
        dprintf("we are not in open status %d\n", State);
        return E_STATE;
    }
    // wait thread stop
    nk::quitThread = true;
    CloseHandle(nk::hMutex);

    pCapture->quit();
    pCapture->wait();
    delete pCapture;
    pCapture = NULL;

    disconnect(timer, SIGNAL(timeout()), this, SLOT(setUpTrigger()));
    disconnect(timer, SIGNAL(timeout()), this, SLOT(setDownTrigger()));
    delete timer;
    delete timer2;
    timer = timer2 = NULL;

    // free internal object
    Nkrypto_DeleteInfo();

    // set state to close
    State = CLOSE;

    dprintf("now we are closing\n");
    return E_OK;
}

Nkrypto_Status HarUsb150Info::exit() {

    if (State != CLOSE && State != INIT) {
        dprintf("we are not in close or init state %d\n", State);
        return E_STATE;
    }

    // uninit camera
    CameraStop(nk::g_hCamera);
    CameraUnInit(nk::g_hCamera);
    CameraAlignFree(nk::g_pRgbBuffer);
    nk::g_hCamera = -1;
    nk::g_pRgbBuffer = NULL;
    // set state to preinit
    State = PREINIT;

    dprintf("now we are exiting\n");
    return E_OK;
}

Nkrypto_Status HarUsb150Info::Nkrypto_GetInfo() {
    if (State != OPEN) {
        dprintf("not in open state ,can not get image %d\n", State);
        return E_STATE;
    }

    WaitForSingleObject(nk::hMutex, INFINITE);

    if (nk::imageSet == 2) {
//        dprintf("camera in empty state, possibly disconnect\n");
        ReleaseMutex(nk::hMutex);
        return E_DEVNOTFOUND;
    }
    int res = Nkrypto_CreateInfo((char*)nk::g_pRgbBuffer, nk::g_FrameInfo.iHeight, nk::g_FrameInfo.iWidth, 1);
    nk::imageSet = 2;
    ReleaseMutex(nk::hMutex);

    return res;
}

Nkrypto_Status HarUsb150Info::getDeviceSerialNum(char *buf, int *size) {
    if (State == PREINIT) {
        dprintf("should at least init first\n");
        return E_STATE;
    }

    if (buf == NULL || size == NULL || *size < VERSTRSIZE) {
        dprintf("buffer null or size not enough\n");
        if (size) *size = snNum.size();
        return E_SMALL;
    }
    memcpy(buf, snNum.c_str(), snNum.size());
    *size = snNum.size();
    return E_OK;
}

Nkrypto_Status HarUsb150Info::getDeviceVersion(char *buf, int *size) {
    if (State == PREINIT) {
        dprintf("should at least init first\n");
        return E_STATE;
    }

    if (buf == NULL || size == NULL || *size < VERSTRSIZE) {
        dprintf("buffer null or size not enough\n");
        if (size) *size = verNum.size();
        return E_SMALL;
    }
    memcpy(buf, verNum.c_str(), verNum.size());
    *size = verNum.size();
    return E_OK;
}

Nkrypto_Status HarUsb150Info::Nkrypto_CreateInfo(char *data, int row, int col, int channel) {
    if (g_info != NULL) {
        delete g_info;
        g_info = NULL;
    }
    g_info = new SnapShot(data, row, col, channel);
    return E_OK;
}

Nkrypto_Status HarUsb150Info::Nkrypto_DeleteInfo() {
    if (g_info) {
        delete g_info;
        g_info = NULL;
    }
    return E_OK;
}

Nkrypto_Status HarUsb150Info::getImage(char* recvBuf, int *size) {
    int res = E_OK;
    if ((res = Nkrypto_GetInfo()) != E_OK) {
        return res;
    }
    return g_info->getImageData(recvBuf, size);
}

Nkrypto_Status HarUsb150Info::getSerialNum(char* recvBuf, int *size) {
    if (!g_info) {
        int res = E_OK;
        if ((res = Nkrypto_GetInfo()) != E_OK) {
            g_info = NULL;
            return res;
        }
    }

    int res = g_info->getSerialNum(recvBuf, size);

    if (res == E_OK) {
        dprintf("get success serial\n");

        if (inuse) {
            dprintf("we are in use\n");
        } else {
            inuse = 1;
            dprintf("begin set\n");
            CameraSetIOState(nk::g_hCamera, 0, 1);

            timer->start(100);
        }
    }
    return res;
}

void HarUsb150Info::setUpTrigger() {
    dprintf("begin set to zero\n");
    CameraSetIOState(nk::g_hCamera, 0, 0);
    timer2->start(100);
}

void HarUsb150Info::setDownTrigger() {
    dprintf("we set back to inuse 0\n");
    inuse = 0;
}
// this is a thread function called when getimage
void CaptureThread::run()
{
    tSdkFrameHead 	sFrameInfo;
    CameraSdkStatus status;

    while (true) {
        if (nk::quitThread) break;

        // mutex between this thread function and camera() function
        WaitForSingleObject(nk::hMutex, INFINITE);
        // wait and get buffer, timeout is 2000ms
        if ((status = CameraGetImageBuffer(nk::g_hCamera, &sFrameInfo, &nk::g_pRawBuffer, 2000)) == CAMERA_STATUS_SUCCESS)
        {
            nk::imageSet = 1;
            // process and optimise image
            status = CameraImageProcess(nk::g_hCamera, nk::g_pRawBuffer, nk::g_pRgbBuffer, &sFrameInfo);
            if (status == CAMERA_STATUS_SUCCESS)
            {
                memcpy(&nk::g_FrameInfo, &sFrameInfo, sizeof(sFrameInfo));
            }
        }
        else {
            nk::imageSet = 2;
            dprintf("timeout currently unavaliable, retry %d\n", status);
        }

        // release buffer after process for the next frame
        CameraReleaseImageBuffer(nk::g_hCamera, nk::g_pRawBuffer);
        ReleaseMutex(nk::hMutex);
    }
}


