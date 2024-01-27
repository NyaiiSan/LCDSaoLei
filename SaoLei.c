#include "main.h"

// 改变图中一个点的数据
static int setMapData(GameMap * map, int x, int y, char data){
    if(x < 0 | x >= map->width | y < 0 | y >= map->height){
        printf("setMapData: (%d, %d) beyond Map \n", x, y);
        return -1;
    }
    *(map->p + map->width * y + x) = data;
    return 1;
}

// 获取图中一个点的数据
static int getMapData(GameMap * map, int x, int y, char * data){
    if(x < 0 | x >= map->width | y < 0 | y >= map->height){
        printf("getMapData: (%d, %d) beyond Map \n", x, y);
        return -1;
    }
    *data = *(map->p + map->width * y + x);
    return 1;
}

// 计算某个格子周围地雷的数量
static int countMine(GameMap * map, int x, int y){
    int count = 0;
    int i,j;
    for(j=y-1; j<y+2; j++){
        if(j < 0) continue;
        else if(j >= map->height) break;
        for(i=x-1; i<x+2; i++){
            if(i < 0) continue;
            else if(i >= map->width) break;

            char data;
            if(getMapData(map, i, j, &data) == -1) continue;
            if(data == 'm') count++;
        }
    }
    return count;
}

static int initSourceMap(GameMap * map){
    int x, y;
    for(y=0;y<map->height;y++){
        for(x=0;x<map->width;x++){
            setMapData(map, x, y, '0');
        }
    }

    // 生成地雷的位置
    int i = 0;
    while(1){
        if(i == map->diffic) break;
        // 计算地雷生成的坐标
        int mx = randuint(0, map->width);
        int my = randuint(0, map->height);
        char data;
        getMapData(map, mx, my, &data);
        if(data != 'm'){
            // printf("Make mine: (%d, %d) \n", mx, my);
            setMapData(map, mx, my, 'm');
            i++;
        }
    }

    // 计算数字
    for(y=0; y<map->height; y++){
        for(x=0; x<map->width; x++){
            // 判断格子是不是地雷
            char data;
            getMapData(map, x, y, &data);
            if(data == 'm') continue;

            int count = countMine(map, x, y);
            // printf("(%d, %d) count: %d \n",x,y,count);

            data = '0' + count;
            setMapData(map, x, y, data);
        }
    }

    return 1;
}

static int initMaskMap(GameMap * map){
    int x, y;
    for(y=0; y<map->height; y++){
        for(x=0; x<map->width; x++){
            setMapData(map, x, y, 'h');
        }
    }
}

GameMap creatGameMap(int width, int height, int n){
    GameMap map;
    map.width = width;
    map.height = height;
    map.diffic = n;
    map.p = malloc(width * height);

    initSourceMap(&map);
    
    return map;
}

GameMap creatMaskMap(GameMap sourceMap){
    GameMap maskMap;
    maskMap.width = sourceMap.width;
    maskMap.height = sourceMap.height;
    maskMap.p = malloc(maskMap.width*maskMap.height);

    initMaskMap(&maskMap);

    return maskMap;
}

int drawGameMap(Screen screen, GameMap map, int sx, int sy, int width, int height){
    printf("Draw GameMap \n");
    // 创建一个绘制map的Screen
    Screen mapScreen;
    mapScreen.height = height;
    mapScreen.width = width;
    mapScreen.p = malloc(width*height*sizeof(int));

    //初始化mapScreen
    clearScreen(mapScreen, -1);

    // 计算字符每个网格的像素宽度
    int pixSize;

    int wpixSize = mapScreen.width / map.width; // 宽度能取得的最大像素
    int hpixSize = mapScreen.height / map.height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (mapScreen.width - map.width * pixSize) / 2;
    int ymargin = (mapScreen.height - map.height * pixSize) / 2;

    printf("xmargin: %d, yamrgin: %d \n", xmargin, ymargin);

    // 绘制边框
    int x, y;
    for(y=1; y<map.height; y++){
        drawRect(mapScreen, xmargin, ymargin + pixSize * y, map.width*pixSize, 4, 0x0066ccff);
    }
    for(x=1; x<map.width; x++){
        drawRect(mapScreen, xmargin + pixSize * x, ymargin, 4, map.height*pixSize, 0x0066ccff);
    }

    // 绘制GameMap
    for(y=0; y<map.height; y++){
        for(x=0; x<map.width; x++){
            drawRect(mapScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ffffff);
            char data;
            getMapData(&map, x, y, &data);
            // printf("(%d, %d) : %c \t", x, y, data);
            if(data >= '1' && data <= '8'){
                int color;
                switch (data)
                {
                case '1':
                    color = 0x00B5E61D;
                    break;

                case '2':
                    color = 0x0022b14c;
                    break;

                case '3':
                    color = 0x0000A2E8;
                    break;
                
                case '4':
                    color = 0x003F48CC;
                    break;

                case '5':
                    color = 0x00A349A4;
                    break;
                
                default:
                    color = 0x00222222;
                    break;
                }
                drawChar(mapScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, data, pixSize-4, pixSize-4, color);
            }
            else if(data == 'm'){
                drawRect(mapScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 'h'){
                drawRect(mapScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00888888);
            }
            else{
                continue;
            }
        }
    }

    buflash(screen, mapScreen, sx, sy);
    free(mapScreen.p);
    return 0;
}

int openGrid(GameMap sourceMap, GameMap * showMap, int x, int y){
    printf("openGrid: (%d, %d) \n", x, y);
    char data;
    // 判断要打开的格子的状态
    getMapData(showMap, x, y, &data);
    if(data != 'h'){
        return -1;
    }

    // 获取格子中的实际内容
    getMapData(&sourceMap, x, y, &data);
    if(data >= '1' && data <= '9'){ // 点到一个数字，揭开这一个格子
        setMapData(showMap, x, y, data);
        return 0;
    }
    else if(data == '0'){
        setMapData(showMap, x, y, data);
        int i, j;
        for(j=y-1; j<y+2; j++){
            if(j < 0) continue;
            if(j >= showMap->height) break;
            for(i=x-1; i<x+2; i++){
                if(i < 0) continue;
                if(i >= showMap->width) break;
                openGrid(sourceMap, showMap, i, j);
            }
        }
        return 0;
    }
    else if(data == 'm'){ // 点到地雷游戏结束
        return 1;
    }
}