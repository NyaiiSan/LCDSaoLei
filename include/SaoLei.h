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
    char name[16];
    int point;
}Player;

// 玩家数据库
struct playerData
{
    // 存储简单难度玩家数据
    Player **easyPlays;
    int easyPlaysNum;
    // 存储普通难度玩家数据
    Player **normalPlays;
    int normalPlaysNum;
    // 存储困难难度玩家数据
    Player **hardPlays;
    int hardPlaysNum;
};


typedef struct{
    View * screen;
    View * gameView;
    GameMap * sourceMap;
    GameMap * showMap;
    Timer * timer; // 游戏中嵌入一个计时器
    struct playerData * players;
    int width;  // 游戏区域的宽度
    int height; // 游戏区域的高度
    int diffic; // 游戏区域的地雷数量
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