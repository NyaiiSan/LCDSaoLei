#ifndef __SAOLEI_H__
#define __SAOLEI_H__

#include "main.h"

typedef struct{
    int width;  // 宽度
    int height; // 高度
    int sx;
    int sy;
    int diffic; // 地雷的数量
    char * p;   // 存储地图的数组
}GameMap;

typedef struct{
    View * gameView;
    GameMap * sourceMap;
    GameMap * showMap;
    View * screen; // 游戏将要在哪个窗口运行
}SaoleiGame;

// 初始化一个扫雷游戏
SaoleiGame * creatSaolei();

// 游戏重新开始
int restartSaolei();

/**
 * 打开被选中的格子
*/
int openSelectedGrid();

#endif