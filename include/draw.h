#ifndef __DRAW_H__
#define __DRAW_H__

#include "main.h"

// 绘制一个点
int drawPoint(int * p, int x, int y, int color);

// 清空屏幕
int clear(int * screen);

// 绘制一个矩形
int drawRect(int * screen, int x, int y, int width, int height, int color);

// 绘制一个圆
int drawCircle(int * screen, int sx, int sy, int r, int color);

// 绘制一个太极
int drawTaiji(int * screen, int sx, int sy, int r);

#endif