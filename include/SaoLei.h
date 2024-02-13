#ifndef __SAOLEI_H__
#define __SAOLEI_H__

#include "main.h"

#define HIDE_COLOR 0x00e1babf

typedef struct{
    int sx;
    int sy;
    char * p;   // 存储地图的数组
}GameMap;

typedef struct{
    View * screen;
    View * gameView;
    GameMap * sourceMap;
    GameMap * showMap;
    Timer * timer; // 游戏中嵌入一个计时器
    int width;
    int height;
    int diffic;
    int startPoint[2]; // 第一次点击的格子
    char state;
}SaoleiGame;

// 初始化一个扫雷游戏
SaoleiGame * creatSaolei();

// 初始化游戏显示布局
int initSaoleiLayout(SaoleiGame * game);

// 游戏重新开始
int restartSaolei();

/**
 * 选择一个格子: 光标移动到选中格子
*/
int selectGrid();

/**
 * 打开被选中的格子
*/
int openSelectedGrid();

/**
 * 被选中的格子插旗
*/
int flagSelectedGrid();

// 选择游戏难度
int diffselect();
// 打开菜单
int menuOpen();
// 关闭菜单
int menuClose();

#endif