#ifndef __VIEW_H__
#define __VIEW_H__

#define MAX_VIEWS_NUM 20

#include "main.h"

typedef struct view
{
    char state; // View时候会被更新
    int marginsX; // x方向的相对与父的位置
    int marginsY; // y方向的相对与父的位置
    Canvas * canvas; // 区域显示的内容
    int subViewsNum;
    struct view * subViews[MAX_VIEWS_NUM];
}View;

/**
 * 创建一个View
*/
View * creatView(int width, int height, int marginsX, int marginsY);

/**
 * 删除一个View
*/
int delView(View * view);

/**
 * 
*/
int addView(View * targetView, View * view);

/**
 * 刷新View
*/
int flashView(View * view);

/**
 * 新线程中刷新View
*/
void * threadFlashView(void * argv);

#endif