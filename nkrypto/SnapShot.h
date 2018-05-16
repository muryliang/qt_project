#ifndef SNAPSHOT
#define SNAPSHOT

#include <windows.h>
#include <cstdio>
#include <string>
#include "nkrypto.h"

class SnapShot {

private:
    std::string _ymlfilename;
    char * _data;	   // should use shared_ptr here??
    int _row, _col, _channel;
    int _bitsArray[91]; // bit array decode from image opencv matrix
    char _serialNo[9]; // serial number length 8  + 1 for '\0'
    char _uid[129];  // uid generated from serial num 128 + 1 for '\0'

    // only initialized once at first time
    static Nkrypto_Status  errornum;  // used in constructor func to denote error status

    int processImage(); //helper function of ctor, calculate serials
    int loadDynFunc();
    // check if the returned object is valid for next process
    bool isValid();

public:
    // default ctor
    SnapShot();

    // get mat, calculate bitsarray, serialno and uid
    // currently we just hard set use channel as 1, if image
    // have multiple channel, FixMe!! (self implement opencv's split channel function)
    SnapShot(char *data, int row, int col, int channel = 1);

    // copy constructor
    SnapShot(SnapShot& copyInfo);

    // used to delete some inside allocated buffer if used
    ~SnapShot();

    // return imagedata into recvBuf of size
    // pass in NULL ptr will set size needed
    int getImageData(char *recvBuf, int *size);

    int getSerialNum(char *recvBuf, int *size);
};

#endif
