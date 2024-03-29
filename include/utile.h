#ifndef __UTILE_H__
#define __UTILE_H__

#include "main.h"


//计算一个点到另一个点的距离
int pointsDistance(int sx, int sy, int tx, int ty);

//合成一个颜色
unsigned int color(unsigned char, unsigned char, unsigned char);

// 获取一次案件输入事件
int * getEvents(char *);

// 范围正随机数
int randuint(int, int);

#endif