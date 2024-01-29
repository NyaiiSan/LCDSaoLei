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

static GameMap * creatGameMap(int width, int height, int n){
    GameMap * map = malloc(sizeof(GameMap));
    map->width = width;
    map->height = height;
    map->diffic = n;
    map->p = malloc(width * height);

    initSourceMap(map);
    
    return map;
}

static GameMap * creatMaskMap(GameMap * sourceMap){
    GameMap * maskMap = malloc(sizeof(GameMap));
    maskMap->width = sourceMap->width;
    maskMap->height = sourceMap->height;
    maskMap->diffic = sourceMap->diffic;
    maskMap->p = malloc(maskMap->width*maskMap->height);

    initMaskMap(maskMap);

    return maskMap;
}

SaoleiGame * creatSaolei(){
    // 初始化游戏
	SaoleiGame * game = malloc(sizeof(SaoleiGame)); //创建一个游戏

	// 创建一个游戏的两张基础图
	GameMap * sourceMap = creatGameMap(8, 10, 10);
    GameMap * showMap = creatMaskMap(sourceMap);
	
	// 将两张基础图添加到游戏中
	game->sourceMap = sourceMap;
	game->showMap = showMap;

	// 建立一个游戏Screen
	Screen * gameView = malloc(sizeof(Screen));
	gameView->height = 600;
	gameView->width = 600;
	gameView->relatX = gameView->relatY = 0;
	gameView->p = malloc(gameView->height * gameView->width * sizeof(int));
	clearScreen(gameView, 0x00ffffff);
	flashGameScreen(gameView, showMap);

	// 将两游戏Screen添加到游戏中
	game->gameView = gameView;

    return game;
}

void delSaolei(SaoleiGame * game){
    free(game->sourceMap->p);
    free(game->sourceMap);

    free(game->showMap->p);
    free(game->showMap);

    free(game->gameView->p);
    free(game->gameView);

    free(game);
}

int flashGameScreen(Screen * gameScreen, GameMap * map){

    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = gameScreen->width / map->width; // 宽度能取得的最大像素
    int hpixSize = gameScreen->height / map->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (gameScreen->width - map->width * pixSize) / 2;
    int ymargin = (gameScreen->height - map->height * pixSize) / 2;

    // 绘制边框
    int x, y;
    for(y=1; y<map->height; y++){
        drawRect(gameScreen, xmargin, ymargin + pixSize * y, map->width*pixSize, 4, 0x0066ccff);
    }
    for(x=1; x<map->width; x++){
        drawRect(gameScreen, xmargin + pixSize * x, ymargin, 4, map->height*pixSize, 0x0066ccff);
    }

    // 绘制GameMap
    for(y=0; y<map->height; y++){
        for(x=0; x<map->width; x++){
            drawRect(gameScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ffffff);
            char data;
            getMapData(map, x, y, &data);
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
                drawChar(gameScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, data, pixSize-4, pixSize-4, color);
            }
            else if(data == 'm'){
                drawRect(gameScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 'h'){
                drawRect(gameScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00888888);
            }
            else if(data == 's'){
                drawRect(gameScreen, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ffcc66);
            }
            else{
                continue;
            }
        }
    }
    return 0;
}

static int gameIsWin(GameMap * showMap){
    int x, y;
    int hideNum = 0;
    for(y = 0; y<showMap->height; y++){
        for(x = 0; x<showMap->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 'h' || data == 's') hideNum++;
        }
    }
    printf("gameIsWin: hideNum %d diffic %d \n", hideNum, showMap->diffic);
    if(hideNum == showMap->diffic){
        return 1;
    }
    else{
        return 0;
    }
}

int openGrid(GameMap * sourceMap, GameMap * showMap, int x, int y){
    printf("openGrid: (%d, %d) \n", x, y);
    char data;
    // 判断要打开的格子的状态
    getMapData(showMap, x, y, &data);
    if(data != 'h' && data != 's'){
        return -1;
    }

    // 获取格子中的实际内容
    getMapData(sourceMap, x, y, &data);
    if(data >= '0' && data <= '8'){ // 点到一个数字，揭开这一个格子
        setMapData(showMap, x, y, data);
        if(data == '0'){
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
        }
        if(gameIsWin(showMap) == 1){
            return 6;
        }
        return 0;
    }

    else if(data == 'm'){ // 点到地雷游戏结束
        return 1;
    }
}

int getRelatPoints(Screen * view, Point * touchP, int * relatPoint){
    relatPoint[0] = touchP->x - view->relatX;
    relatPoint[1] = touchP->y - view->relatY;
    // 判断坐标是否在view内部
    if(relatPoint[0] < 0 || relatPoint[0] > view->width || relatPoint[1] < 0 || relatPoint[1] > view->height){
        return -1;
    }

    touchP->x = touchP->y = -1;
    return 1;
}

int getTouchGrid(Screen * view, GameMap * map, int * relatP, int * res){
// 获取相对坐标并绘制光标
    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = view->width / map->width; // 宽度能取得的最大像素
    int hpixSize = view->height / map->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (view->width - map->width * pixSize) / 2;
    int ymargin = (view->height - map->height * pixSize) / 2;

    if(relatP[0] < xmargin || relatP[1] < ymargin){
        return -1;
    }

    *res = (relatP[0] - xmargin) / pixSize;
    *(res + 1) = (relatP[1] - ymargin) / pixSize;

    // printf("TouchGrid: (%d, %d) \n", *res, *(res + 1));
    return 1;
}

int selectGrid(Screen * gameView, GameMap * maskMap, int * relatPoints){

    // 获取触摸的网格
    int grid[2];
    getTouchGrid(gameView, maskMap, relatPoints, grid);

    char data;
    getMapData(maskMap, grid[0], grid[1], &data);
    int i, j;
    if(data == 'h'){
        for(j=0; j<maskMap->height; j++){
            for(i=0; i<maskMap->width; i++){
                getMapData(maskMap, i, j, &data);
                if(data == 's'){
                    setMapData(maskMap, i, j, 'h');
                }
            }
        }
        printf("selectGrid: Select(%d, %d) \n", grid[0], grid[1]);
        setMapData(maskMap, grid[0], grid[1], 's');
    }
    return 1;
}

int viewIsTouched(Screen * view, Point * touchP){
    int t[2];
    if(getRelatPoints(view, touchP, t) == -1){
        return 0;
    }
    return 1;
}

int openSelectedGrid(GameMap * sourceMap, GameMap * showMap){
    // 找到被选中的格子
    int x, y;
    for(y=0; y<showMap->height; y++){
        for(x=0; x<showMap->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 's'){
                return openGrid(sourceMap, showMap, x, y);
            }
        }
    }
    return -1;
}