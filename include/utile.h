#ifndef __UTILE_H__
#define __UTILE_H__

#include "main.h"


//保存坐标的结构体 
struct point 
{
	int x ; //保存坐标的x轴值 
	int y ; //保存坐标的y轴值
};

//获取手指在屏幕的点击坐标并返回 
struct point Get_Touch() ;

// 一直获取屏幕点击坐标
void * alwaysGet_Touch(void *);

//计算一个点到另一个点的距离
int pointsDistance(int sx, int sy, int tx, int ty);

//合成一个颜色
unsigned int color(unsigned char, unsigned char, unsigned char);

// 获取一次案件输入事件
int * getEvents(char *);

// 范围正随机数
int randuint(int, int);

#endif