#ifndef NDCODE_IMAGE_RECOGNITION_H
#define NDCODE_IMAGE_RECOGNITION_H

extern "C" {

int nanoid_hexagon_recognition(unsigned char* imdata,  // input image
    int img_rows, int img_cols, int channels,  // image row, col and channels
    char* param, int* bitsArray, int bitArray_len); // parameters xml file name, output bit array, bit array's length

//void get_version(char* version, int len);
}

#endif // NDCODE_IMAGE_RECOGNITION_H
