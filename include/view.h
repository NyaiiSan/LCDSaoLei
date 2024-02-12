#ifndef __VIEW_H__
#define __VIEW_H__

#define MAX_VIEWS_NUM 20 // 每个View可以有的最大子View的数量

#include "main.h"

typedef struct event{
    char state; // 是否有事件
    char type; // 事件的类型
    int value[4]; // 事件的返回值
}Event;

typedef struct view
{
    char state;
    // View更新状态
    // 0: 不会被更新
    // 1: 全部更新
    // 2: 只刷新子View, 本身不会被刷新, 一般是一些约束性的View
    int id; // 保存View的唯一ID
    int marginsX; // x方向的相对与父的位置
    int marginsY; // y方向的相对与父的位置
    int subViewsNum; // 该View的子View数量
    struct view * subViews[MAX_VIEWS_NUM]; // 保存该View所有的子Views的地址
    Event event; // 保存View的事件信息
    Canvas * canvas; // 区域显示的内容
}View;

/**
 * 创建一个View
*/
View * creatView(int id, int width, int height, int marginsX, int marginsY);

/**
 * 删除一个View
*/
int delView(View * view);

/**
 * 向一个View中添加一个View
*/
int addView(View * targetView, View * view);

/**
 * 新线程中刷新View
*/
void * threadFlashView(void * argv);

/**
 * 初始化触摸事件
*/
void initTouchEvent(View * screen);

/**
 * 根据ID设置一个View的状态
*/
void setViewById(View * view, int id, char state);

/**
 * 根据ID查找一个View
*/
View * getViewById(View * view, int id);

/*********************
 * Timer
**********************/
// 计时器对象
typedef struct{
    View * view;
    int t;
    unsigned startTime;
    pthread_t thread;
    char cmd[4];
    char state;
    int color;
}Timer;

// 创建并初始化一个计时器
Timer * creaTimer(int color);

// 运行一个计时器
void timeStart(Timer * timer);

#endif