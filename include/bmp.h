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

// 打开一张bmp格式的图片
BmpImg * openBmpImg(char * path);

#endif