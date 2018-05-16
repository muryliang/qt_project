#include "SnapShot.h"
#include "ndcode_decoder.h"
#include "ndcode_image_recognition.h"
#include "krypto_trace_security_toolset.h"

// initialized static member here
Nkrypto_Status			SnapShot::errornum = E_OK;

SnapShot::SnapShot() {
    _data = NULL;
}

SnapShot::SnapShot(char * data, int row, int col, int channel):
    _row(row), _col(col), _channel(channel)
{

    _ymlfilename = ".\\algorithm_parameters_v01.yml";
    _data = (char *)malloc(_row*_col*_channel);
    if (_data == NULL) {
        errornum = E_CONFNOTFOUND;
        return;
    }
    memset(_serialNo, 0, sizeof(_serialNo));
    memset(_bitsArray, 0, sizeof(_bitsArray));
    memset(_uid, 0, sizeof(_uid));
    memcpy(_data, data, _row*_col*_channel);


    if (errornum == E_OK) {
        processImage();
    }
}

// copy ctor
SnapShot::SnapShot(SnapShot& copyInfo):
_ymlfilename(copyInfo._ymlfilename),
_row(copyInfo._row),
_col(copyInfo._col),
_channel(copyInfo._channel)
{
    _data = (char *)malloc(_row*_col*_channel);
    if (_data == NULL) {
        dprintf("can not alloc\n");
        errornum = E_NOMEMORY;
        return;
    }
    memcpy(_data, copyInfo._data, _row*_col*_channel);
    memcpy(_bitsArray, copyInfo._bitsArray, sizeof(_bitsArray));
    memcpy(_serialNo, copyInfo._serialNo, sizeof(_serialNo));
    memcpy(_uid, copyInfo._uid, sizeof(_uid));
}

SnapShot::~SnapShot() {
    free(_data);
}

int SnapShot::processImage() {

    char* imgdata = _data;
    int img_rows = _row;
    int img_cols = _col;
    int img_channels = 1;

    dprintf("now can show image\n");
    int res = nanoid_hexagon_recognition((uchar*)imgdata, img_rows, img_cols, img_channels, &_ymlfilename[0], _bitsArray, 91);
    if (res < 0) {
        dprintf("get bitarray error %d\n", res);
        return res;
    }

    // hard code, should use macro here
    for(int i = 0; i < 91; ++i) {
        dprintf("%d ", _bitsArray[i]);
    }
    dprintf("\nsuccess get bitarray\n");

    // just define, we do not use this number
    uint8_t model_id;
    // hardcode here
    set_decoder_paras(0,5,0,0);
    res = decoder(_bitsArray, 91, _serialNo, 8, &model_id);
    if (res != E_OK) {
        dprintf("get error serial %d\n", res);
        return E_DECODE;
    }

    dprintf("success get serial %s\n", _serialNo);

    // we should reserve enough space for uid here, Demo says 128 is enough

    res = generateUid(_serialNo, _uid);
    if (res < 0) {
        dprintf("get uid error %d\n", res);
        return res;
    }

    dprintf("success get uid %s\n", _uid);

    return E_OK;
}

// pass in NULL pointer, get required size
// code here have two return ,redundent
int SnapShot::getImageData(char* recvBuf, int *size) {
    if (errornum != E_OK) return errornum; //check constructor functions's errornum
    if (!isValid()) return E_NOIMAGE;

    int totalSize = _row * _col * _channel;
    if (recvBuf == NULL || size == NULL || *size < totalSize ) {
        if (size) *size = totalSize;
        return E_SMALL;
    }
    memcpy(recvBuf, _data, totalSize);
    *size = totalSize;
    return E_OK;
}

int SnapShot::getSerialNum(char *recvBuf, int *size) {
    if (errornum != E_OK)		return errornum; //check constructor functions's errornum
    if (_serialNo[0] == '\0')	return E_NOIMAGE;

    int totalSize = sizeof(_serialNo);
    if (recvBuf == NULL || size == NULL || *size < totalSize ) {
        if (size) *size = totalSize;
        return E_SMALL;
    }
    memcpy(recvBuf, _serialNo, totalSize);
    *size = totalSize;
    return E_OK;
}

bool SnapShot::isValid() {
    return _row != 0;
}
