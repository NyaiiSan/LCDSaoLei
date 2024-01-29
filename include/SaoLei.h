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
    Screen * gameView;
    GameMap * sourceMap;
    GameMap * showMap;
}SaoleiGame;

// 初始化一个扫雷游戏
SaoleiGame * creatSaolei();

// 删除一个扫雷游戏
void delSaolei(SaoleiGame *);

// 在屏幕上绘制一张游戏图
int flashGameScreen(Screen * screen, GameMap * map);

// 打开一个网格
int openGrid(GameMap * sourceMap, GameMap * showMap, int x, int y);

// // 初始化游戏屏幕中的光标
// struct touchPoint * initCursor(Screen screen, struct touchPoint * point);

// 获取相对坐标
int getRelatPoints(Screen * view, Point * touchP, int* posi);

// 根据坐标获取网格
int getTouchGrid(Screen * view, GameMap * map, int * relatP, int * res);

// 选择一个网格
int selectGrid(Screen * gameView, GameMap * maskMap, int * relatPoints);

// 是否被点击
int viewIsTouched(Screen * view, Point * touchP);

// 打开被选中的格子
int openSelectedGrid(GameMap * sourceMap, GameMap * showMap);

#endif