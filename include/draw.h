#ifndef __DRAW_H__
#define __DRAW_H__

#include "main.h"

typedef struct screen{
    int * p;
    int width;
    int height;
}Screen;

typedef struct anime{
    int sx;     // 动画的起始x位置
    int sy;
    int tx;
    int ty;
    int speed;  // 动画的速度
    int type;   //动画类型
}Anime;

// 绘制一个点
int drawPoint(Screen screen, int x, int y, int color);

// 清空屏幕
int clearScreen(Screen screen, int color);

// 绘制一个矩形
int drawRect(Screen screen, int x, int y, int width, int height, int color);

// 绘制一个圆
int drawCircle(Screen screen, int sx, int sy, int r, int color);

// 绘制一个太极
int drawTaiji(Screen screen, int sx, int sy, int r);

// 将一段内存中的数据复制到屏幕中
int buflash(Screen screen, Screen buf, int sx, int sy);

// 将图片转化成Screen
Screen getBmpImg(char * path);

// 显示一张图片
int showBmpImg(Screen screen, BmpImg * img, int x, int y);

// 绘制一个平移动画
int drawAnimeMove(Screen screen, Screen buf, Anime anime);

// 绘制一个渐变动画
int drawAnimeGrad(Screen screen, Screen buf, Anime anime);

// 绘制一个字符
int drawChar(Screen screen, int sx, int sy, char c, int width, int height, int color);

#endif