#include "main.h"

// 将ShowMap刷新到GameView中
static int flashGameView();

/**
 * 打开一个格子
 * 返回值:
 * -1 打开失败
 * 0 正常打开
 * 1 打开后游戏失败
 * 6 打开后游戏胜利
*/
static int openGrid(int x, int y);

// 扫雷游戏
SaoleiGame * game;

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
	game = malloc(sizeof(SaoleiGame)); //创建一个游戏

	// 创建一个游戏的两张基础图
	GameMap * sourceMap = creatGameMap(8, 10, 10);
    GameMap * showMap = creatMaskMap(sourceMap);
	
	// 将两张基础图添加到游戏中
	game->sourceMap = sourceMap;
	game->showMap = showMap;

	// 建立一个游戏View
	View * gameView = creatView(1, 600, 600, 0, 0);

	// 将两游戏Canvas添加到游戏中
	game->gameView = gameView;

    game->state = 1;

    flashGameView();

    return game;
}


static int flashGameView(){
    View * view = game->gameView;
    GameMap * map = game->showMap;

    if(game->state != 1){
        return -1;
    }

    // 刷新的时候锁定
    view->state = 0;

    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = view->canvas->width / map->width; // 宽度能取得的最大像素
    int hpixSize = view->canvas->height / map->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (view->canvas->width - map->width * pixSize) / 2;
    int ymargin = (view->canvas->height - map->height * pixSize) / 2;

    // 绘制边框
    int x, y;
    for(y=1; y<map->height; y++){
        drawRect(view->canvas, xmargin, ymargin + pixSize * y, map->width*pixSize, 4, 0x0066ccff);
    }
    for(x=1; x<map->width; x++){
        drawRect(view->canvas, xmargin + pixSize * x, ymargin, 4, map->height*pixSize, 0x0066ccff);
    }

    // 绘制GameMap
    for(y=0; y<map->height; y++){
        for(x=0; x<map->width; x++){
            // 获取数据
            char data;
            getMapData(map, x, y, &data);
            // printf("(%d, %d) : %c \t", x, y, data);
            /**
             * 判断数据的类型
             * 0-9: 该网格是一个数字或空
             * m: 该网格是一个地雷
             * h: 该网格是一个隐藏内容的格子
             * s: 该网格是一个被选中的格子
             * f: 插旗的格子
             * g: 被选中的旗子
            */
            if(data == 'h'){
                drawRect(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00888888);
            }
            else if(data >= '0' && data <= '8'){
                drawRect(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ffffff);
                if(data == '0') continue;
                int color;
                // 不同数字显示不同颜色
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
                drawChar(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, data, pixSize-4, pixSize-4, color);
            }
            else if(data == 'm'){
                drawRect(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 's'){
                drawRect(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ffcc66);
            }
            else if(data == 'f'){
                drawRect(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00888888);
                drawChar(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, 'F', pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 'g'){
                drawRect(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, pixSize-4, pixSize-4, 0x00ffcc66);
                drawChar(view->canvas, xmargin + x*pixSize+4, ymargin + y*pixSize+4, 'F', pixSize-4, pixSize-4, 0x00ff0000);
            }
            else{
                continue;
            }
        }
    }
    view->state = 1; // 刷新结束解锁
    return 0;
}

// 判断游戏胜利的条件
static int gameIsWin(){

    GameMap * showMap = game->showMap;

    int x, y;
    int hideNum = 0;
    for(y = 0; y<showMap->height; y++){
        for(x = 0; x<showMap->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 'h' || data == 's' || data == 'f' || data == 'g') hideNum++;
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


static int openGrid(int x, int y){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;
    // printf("openGrid: (%d, %d) \n", x, y);
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
                    openGrid(i, j);
                }
            }
        }
        if(gameIsWin() == 1){
            return 6;
        }
        return 0;
    }

    else if(data == 'm'){ // 点到地雷游戏结束
        return 1;
    }
}

int getRelatPoints(View * view, Point * touchP, int * relatPoint){
    relatPoint[0] = touchP->x - view->marginsX;
    relatPoint[1] = touchP->y - view->marginsY;
    // 判断坐标是否在view内部
    if(relatPoint[0] < 0 || relatPoint[0] > view->canvas->width || relatPoint[1] < 0 || relatPoint[1] > view->canvas->height){
        return -1;
    }

    touchP->x = touchP->y = -1;
    return 1;
}

// 获取触摸到的网格并绘制光标
static int getTouchGrid(int * res){
    View * view = game->gameView;
    GameMap * map = game->showMap;

    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = view->canvas->width / map->width; // 宽度能取得的最大像素
    int hpixSize = view->canvas->height / map->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (view->canvas->width - map->width * pixSize) / 2;
    int ymargin = (view->canvas->height - map->height * pixSize) / 2;

    // 相对坐标
    int relatP[] = {view->event.value[0], view->event.value[1]};

    if(relatP[0] < xmargin || relatP[1] < ymargin){
        return -1;
    }

    *res = (relatP[0] - xmargin) / pixSize;
    *(res + 1) = (relatP[1] - ymargin) / pixSize;

    // printf("TouchGrid: (%d, %d) \n", *res, *(res + 1));
    return 1;
}

int selectGrid(){
    GameMap * maskMap = game->showMap;

    // 获取触摸的网格
    int grid[2];
    getTouchGrid(grid);

    char data;
    getMapData(maskMap, grid[0], grid[1], &data);
    int i, j;
    
    if(data == 'h' || data == 'f'){  // 判断是否可以被选中
        // 清除上一次的选中
        for(j=0; j<maskMap->height; j++){
            for(i=0; i<maskMap->width; i++){
                getMapData(maskMap, i, j, &data);
                if(data == 's'){
                    setMapData(maskMap, i, j, 'h');
                }
                else if(data == 'g'){
                    setMapData(maskMap, i, j, 'f');
                }
            }
        }
        
        // 选中指定的格子
        printf("selectGrid: Select(%d, %d) \n", grid[0], grid[1]);
        getMapData(maskMap, grid[0], grid[1], &data);
        if(data == 'h'){
            setMapData(maskMap, grid[0], grid[1], 's');
        }
        else if(data == 'f'){
            setMapData(maskMap, grid[0], grid[1], 'g');
        }
    }

    flashGameView();
    return 1;
}

static int gameOver(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    int x, y;
    for(y=0; y<showMap->height; y++){
        for(x=0; x<showMap->width; x++){
            char data;
            getMapData(sourceMap, x, y, &data);
            if(data == 'm'){
                setMapData(showMap, x, y, 'm');
            }
        }
    }
    flashGameView();
    game->state = 0;

    return 1;
}

static int gameWin(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    int x, y;
    for(y=0; y<showMap->height; y++){
        for(x=0; x<showMap->width; x++){
            char data;
            getMapData(sourceMap, x, y, &data);
            if(data == 'm'){
                setMapData(showMap, x, y, 'f');
            }
        }
    }
    flashGameView();
    game->state = 0;

    return 1;
}

int openSelectedGrid(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    // 找到被选中的格子
    int x, y;
    int openRes;
    for(y=0; y<showMap->height; y++){
        for(x=0; x<showMap->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 's'){
                openRes = openGrid(x, y);
            }
        }
    }

    if(openRes == 0){
        flashGameView();
    }
    else if (openRes == 1){
        gameOver();
    }
    else if(openRes == 6){
        gameWin();
    }
    return openRes;
}

// 将一个格子插旗
static int flagGrid(int x, int y){
    GameMap * showMap = game->showMap;

    char data;
    getMapData(showMap, x, y, &data);
    if(data == 's'){
        setMapData(showMap, x, y, 'f');
        return 1;
    }
    else if(data == 'g'){
        setMapData(showMap, x, y, 'h');
        return 0;
    }
    else{
        return -1;
    }
}

int flagSelectedGrid(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    // 找到被选中的格子
    int x, y;
    int flagRes;
    for(y=0; y<showMap->height; y++){
        for(x=0; x<showMap->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 's' || data == 'g'){
                flagRes = flagGrid(x, y);
            }
        }
    }
    flashGameView();
}

int restartSaolei(){
    free(game->showMap);
    free(game->sourceMap);

    // 创建一个游戏的两张基础图
	GameMap * sourceMap = creatGameMap(8, 10, 10);
    GameMap * showMap = creatMaskMap(sourceMap);
	
	// 将两张基础图添加到游戏中
	game->sourceMap = sourceMap;
	game->showMap = showMap;

    game->state = 1;
    flashGameView();
}