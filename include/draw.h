#ifndef __DRAW_H__
#define __DRAW_H__

#include "main.h"

// 画布，保存绘制区域的数据
typedef struct canvas{
    int width;  //画布的高度
    int height; //画布的宽度
    int * p;    // 存储绘图区域的指针
}Canvas;

typedef struct anime{
    int sx;     // 动画的起始x位置
    int sy;
    int tx;
    int ty;
    int speed;  // 动画的速度
    int type;   //动画类型
}Anime;

/**
 * 在画布上绘制一个点
 * x: 绘制点在画布上的x坐标
 * y: 绘制点在画布上的x坐标
 * color: 绘制点的颜色
*/
int drawPoint(Canvas * canvas, int x, int y, int color);

/**
 * 用指定的颜色填充整个画布
*/
int clearCanvas(Canvas * canvas, int color);

/**
 * 在画布上绘制一个矩形
 * x, y: 矩形的起始位置坐标
 * width: 矩形的宽度
 * height: 矩形的高度
*/
int drawRect(Canvas * canvas, int x, int y, int width, int height, int color);

/**
 * 在画布上绘制一个圆
 * x,y: 圆心的坐标
 * r: 圆的半径
*/
int drawCircle(Canvas * canvas, int x, int y, int r, int color);

// 绘制一个太极
int drawTaiji(Canvas * canvas, int sx, int sy, int r);

/**
 * 在一个画布上绘制另一个画布上的内容
 * buf: 要绘制的画布内容
 * x, y: 要绘制到目标画布上的坐标
*/
int buflash(Canvas * canvas, Canvas * buf, int x, int y);

// 将图片转化成Canvas
Canvas getBmpImg(char * path);

// 显示一张图片
int showBmpImg(Canvas * canvas, BmpImg * img, int x, int y);

// 绘制一个平移动画
int drawAnimeMove(Canvas * canvas, Canvas buf, Anime anime);

// 绘制一个渐变动画
int drawAnimeGrad(Canvas * canvas, Canvas buf, Anime anime);

/**
 * 在画布上绘制一个字符
 * x, y: 要绘制到画布上的坐标
 * width: 字符区域的宽度
 * height: 字符区域的高度
 * color: 字符的颜色
*/
int drawChar(Canvas * canvas, int x, int y, char c, int width, int height, int color);

#endif