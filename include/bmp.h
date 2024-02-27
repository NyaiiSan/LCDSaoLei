#ifndef __BMP_H__
#define __BMP_H__

#include "main.h"

typedef struct bmp
{
    int width;
    int height;
    short sp;
    short depth;
    int fillerSize;
    char * p;
}BmpImg;

typedef struct{
    short bfType;
    int bfSize;
    short bfReserved1;
    short bfReserved2;
    int bfOffBits;
}BMFHeader;

typedef struct
{
    unsigned int biSize;
    int biWidth;
    int biHeight;
    short biPlanes;
    short biBitCount;
    int biCompression;
    int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    int biClrUsed;
    int biClrImportant;
}BMIHeader;

// 打开一张bmp格式的图片
BmpImg * openBmpImg(char * path);

#endif