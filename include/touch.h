#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "main.h"

//保存坐标的结构体 
struct touchPoint 
{
	int x ; //保存坐标的x轴值 
	int y ; //保存坐标的y轴值
};

typedef struct touchPoint Point;

//获取手指在屏幕的点击坐标并返回 
struct touchPoint getTouchPoint() ;

// 初始化触控功能
struct touchPoint * initTouch();

#endif