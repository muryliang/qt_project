#ifndef TOOL_H
#define TOOL_H

#include "nkrypto_global.h"

/*
 * error code
 */
typedef int		Nkrypto_Status;

#define E_OK			0	 // success
#define E_UNKNOWN		-1	 // unknown error
#define E_NOMEMORY		-2   // memory alloc fail
#define E_DEVINIT		-3   // init device fail
#define E_DEVNOTFOUND	-4   // can not enumerate device
#define E_THREAD		-5   // thread start fail
#define E_START			-6   // can not start work
#define E_SMALL			-7   // buffer size not enough
#define E_NULLPTR		-8   // null pointer passed in
#define E_NOIMAGE		-9   // image data currently not available, possibly capture thread is not running
#define E_DLL			-10  // can not load dll, possibly file or its dependency not found
#define E_CONFNOTFOUND  -11  // yml configuration file not found
#define E_DECODE		-12  // decode serial number error
#define E_STATE			-13  // function order not right, error mathine state
#define E_VERSTR		-14  // can not get version string
#define E_NOTIMPLEMENTED -15 // function not implemented
#define E_SET		 -16 // error camera setting

// definition of device id
#define HAR_USB_150		150
/*
 * init camere and do some prestart work
 * success return E_OK
 * see above macros for error code interpretation
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_Init(int devId);

/*
 * start camera and background process
 * success return E_OK
 * see above macros for error code interpretation
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_Open();

/*
 * stop background process
 * success return E_OK
 * see above macros for error code interpretation
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_Close();

/*
 * close camera, free memory allocated
 * success return E_OK
 * see above macros for error code interpretation
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_Exit();

/*
 * capture one image and return image data
 * success return ok, size will be set to bytes copied
 * see above macros for error code interpretation
 *
 * if error is E_SMALL, then param "size" will store the
 * smallest buffer size needed
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_GetImageData(char* recvBuf, int *size);

/*
 * get serial number from captured image
 * success return E_OK, size will be set to bytes copied
 * see above macros for error code interpretation
 *
 * every call of this function should follow one call of Nkrypto_GetImageData,
 * otherwise serial number will not change
 *
 * if error is E_SMALL, then param "size" will store the
 * smallest buffer size needed
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_GetSerialNum(char *recvBuf, int *size);

/*
 * get serial string from camera
 * if success, return E_OK, size will be set to the size copied,
 * if error is E_SMALL, then param "size" will store the
 * smallest buffer size needed
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_DeviceSerialNum(char *buf, int *size);

/*
 * get version string from camera
 * if success, return E_OK, size will be set to the size copied,
 * if error is E_SMALL, then param "size" will store the
 * smallest buffer size needed
 */
extern "C" DLLPORT Nkrypto_Status Nkrypto_DeviceVersion(char *buf, int *size);

/*
 * version of this lib, size >= 20 is enough
 * if success, return E_OK, size will be set to the size copied,
 * if error is E_SMALL, then param "size" will store the
 * smallest buffer size needed
 */
extern "C" DLLPORT Nkrypto_Status Nkrypot_SdkVersion(char *buf, int *size);

#endif
