#include "main.h"

// 初始化扫雷游戏
static void initSaolei(SaoleiGame * game);

// 将ShowMap刷新到GameView中
static int flashGameView();

// 计算标记后的剩余地雷数量
static void updateMineNum();

// 向玩家列表中添加一名玩家
static int addPlayer(char *name, int point);

// 从文件中读取玩家数据
static int loadPlayersData();

// 将玩家数据保存到文件中
static int savePlayersData();

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
    if(x < 0 | x >= game->width | y < 0 | y >= game->height){
        printf("setMapData: (%d, %d) beyond Map \n", x, y);
        return -1;
    }
    *(map->p + game->width * y + x) = data;
    return 1;
}

// 获取图中一个点的数据
static int getMapData(GameMap * map, int x, int y, char * data){
    if(x < 0 | x >= game->width | y < 0 | y >= game->height){
        printf("getMapData: (%d, %d) beyond Map \n", x, y);
        return -1;
    }
    *data = *(map->p + game->width * y + x);
    return 1;
}

// 计算某个格子周围地雷的数量
static int countMine(GameMap * map, int x, int y){
    int count = 0;
    int i,j;
    for(j=y-1; j<y+2; j++){
        if(j < 0) continue;
        else if(j >= game->height) break;
        for(i=x-1; i<x+2; i++){
            if(i < 0) continue;
            else if(i >= game->width) break;

            char data;
            if(getMapData(map, i, j, &data) == -1) continue;
            if(data == 'm') count++;
        }
    }
    return count;
}

static GameMap * creatSourceMap(){
    GameMap * map = malloc(sizeof(GameMap));
    map->p = malloc(game->width * game->height);

    // 初始化Map
    int x, y;
    for(y=0;y<game->height;y++){
        for(x=0;x<game->width;x++){
            setMapData(map, x, y, '0');
        }
    }

    // 生成地雷的位置
    int m = 0;
    while(1){
        if(m == game->diffic) break;
        // 计算地雷生成的坐标
        int mx = randuint(0, game->width);
        int my = randuint(0, game->height);
        if(mx == game->startPoint[0] && my == game->startPoint[1]) continue;
        char data;
        getMapData(map, mx, my, &data);
        if(data != 'm'){
            setMapData(map, mx, my, 'm');
            m++;
        }
    }

    // 计算数字
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
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

    return map;
}

static GameMap * creatShowMap(){
    GameMap * showMap = malloc(sizeof(GameMap));
    showMap->p = malloc(game->width*game->height);

    int x, y;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            setMapData(showMap, x, y, 'h');
        }
    }

    return showMap;
}

static void initSaolei(SaoleiGame * game){
    printf("initSaolei: SaoLei Init  \n");
    if(game->sourceMap){
        free(game->sourceMap);
    }
    if(game->showMap){
        free(game->showMap);
    }

    // 创建一个游戏的两张基础图
	GameMap * sourceMap = creatSourceMap();
    GameMap * showMap = creatShowMap();
    
    game->sourceMap = sourceMap;
    game->showMap = showMap;

    game->state = 7;
    game->timer->cmd[0] = 3;

    clearCanvas(game->gameView->canvas, -1);
    updateMineNum();
    flashGameView();
}


static int flashGameView(){
    View * view = game->gameView;
    GameMap * map = game->showMap;

    if(game->state == 0){
        return -1;
    }

    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = view->canvas->width / game->width; // 宽度能取得的最大像素
    int hpixSize = view->canvas->height / game->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (view->canvas->width - game->width * pixSize) / 2;
    int ymargin = (view->canvas->height - game->height * pixSize) / 2;

    // 绘制边框
    int x, y;
    for(y=1; y<game->height; y++){
        drawRect(view->canvas, xmargin, ymargin + pixSize * y, game->width*pixSize, 4, 0x0066ccff);
    }
    for(x=1; x<game->width; x++){
        drawRect(view->canvas, xmargin + pixSize * x, ymargin, 4, game->height*pixSize, 0x0066ccff);
    }

    // 绘制GameMap
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            // 获取数据
            char data;
            getMapData(map, x, y, &data);
            // printf("(%d, %d) : %c \t", x, y, data);
            int sx = xmargin + x*pixSize+4; // 绘制的格子x轴起始坐标
            int sy = ymargin + y*pixSize+4; // 绘制的格子y轴起始坐标
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
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, HIDE_COLOR);
            }
            else if(data >= '0' && data <= '8'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, -1);
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
                drawChar(view->canvas, sx, sy, data, pixSize-4, pixSize-4, color);
            }
            else if(data == 'm'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 's'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, 0x00ffcc66);
            }
            else if(data == 'f'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, HIDE_COLOR);
                drawChar(view->canvas, sx, sy, 'F', pixSize-4, pixSize-4, 0x00ff0000);
            }
            else if(data == 'g'){
                drawRect(view->canvas, sx, sy, pixSize-4, pixSize-4, 0x00ffcc66);
                drawChar(view->canvas, sx, sy, 'F', pixSize-4, pixSize-4, 0x00ff0000);
            }
            else{
                continue;
            }
        }
    }
    return 0;
}

// 判断游戏胜利的条件
static int gameIsWin(){

    GameMap * showMap = game->showMap;

    int x, y;
    int hideNum = 0;
    for(y = 0; y<game->height; y++){
        for(x = 0; x<game->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 'h' || data == 's' || data == 'f' || data == 'g') hideNum++;
        }
    }
    // printf("gameIsWin: hideNum %d diffic %d \n", hideNum, game->diffic);
    if(hideNum == game->diffic){
        return 1;
    }
    else{
        return 0;
    }
}

static int openGrid(int x, int y){
    // 判断是否是第一次点开，第一次点开重新布局并开始计时器
    if(game->state == 7){
        game->startPoint[0] = x;
        game->startPoint[1] = y;
        free(game->sourceMap);
        game->sourceMap = creatSourceMap();
        game->state = 1;
        game->timer->cmd[0] = 1;
    }

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
    printf("openGrid: Opened(%d, %d): %c \n", x, y, data);
    if(data >= '0' && data <= '8'){ // 点到一个数字，揭开这一个格子
        setMapData(showMap, x, y, data);
        if(data == '0'){
            int i, j;
            for(j=y-1; j<y+2; j++){
                if(j < 0) continue;
                if(j >= game->height) break;
                for(i=x-1; i<x+2; i++){
                    if(i < 0) continue;
                    if(i >= game->width) break;
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

// 获取触摸到的网格并绘制光标
static int getTouchGrid(int * res){
    View * view = game->gameView;
    GameMap * map = game->showMap;

    // 计算字符每个网格的像素宽度
    int pixSize;
    int wpixSize = view->canvas->width / game->width; // 宽度能取得的最大像素
    int hpixSize = view->canvas->height / game->height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (view->canvas->width - game->width * pixSize) / 2;
    int ymargin = (view->canvas->height - game->height * pixSize) / 2;

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

SaoleiGame * creatSaolei(){
	game = malloc(sizeof(SaoleiGame) + 4); //创建一个游戏
	
    // 设置游戏规格
    game->width = 9;
    game->height = 9;
    game->diffic = 10;

	// 将两张基础图添加到游戏中
	game->sourceMap = NULL;
	game->showMap = NULL;

	// 游戏View添加到游戏中
	game->gameView = NULL;

    // 为Game添加一个计时器
    game->timer = creaTimer(0xffcc66);

    game->startPoint[0] = -1;
    game->startPoint[1] = -1;

    // 开始游戏计时器
    timeStart(game->timer);

    // 初始化计时器
    game->timer->cmd[0] = 3;

    // 初始化游戏玩家数据
    game->players = malloc(sizeof(struct playerData));
    game->players->easyPlays = malloc(sizeof(Player *) * 20);
    game->players->normalPlays = malloc(sizeof(Player *) * 20);
    game->players->hardPlays = malloc(sizeof(Player *) * 20);
    if(loadPlayersData() == -1){
        game->players->easyPlaysNum = 0;
        game->players->normalPlaysNum = 0;
        game->players->hardPlaysNum = 0;
    }

    return game;
}

int initSaoleiLayout(SaoleiGame * game){
    View * screen = game->screen;

    // 新建一张背景
	Canvas * backgroundImg = getBmpImg("bg.bmp");	// 导入背景图片
	View * background = creatView(0, screen->canvas->width, screen->canvas->height, 0, 0); // 创建背景控件
	buflash(background->canvas, backgroundImg, 0, 0);	// 设置控件背景

    // 新建一个游戏操作View
    View * gamePlayView = creatView(10, screen->canvas->width, screen->canvas->height, 0, 0);

    // 新建一个游戏区域
    game->gameView = creatView(11, 600, 600, 0, 0);

    // 新建计时器显示
    game->timer->view = creatView(19, 150, 40, 920, 10);

    // 新建剩余地雷数量显示
    View * mineNumView = creatView(18, 150, 40, 720, 10);

	// 新建一个点击View 功能是开启一个格子
	View * openGrid = creatView(12, 100, 70, 870, 100);	// 创建按键
	clearCanvas(openGrid->canvas, 0x0000ff00);	// 设置按钮颜色
	drawString(openGrid->canvas, 5, 10, "OPEN", 30, 0x00ffffff);	// 添加提示文字

	// 新建一个View 功能是插旗子
	View * setFlag = creatView(13, 100, 70, 870, 200);	// 创建按键
	clearCanvas(setFlag->canvas, 0x00ff0000);	// 设置按钮颜色
	drawString(setFlag->canvas, 5, 10, "FLAG", 30, 0x00ffffff);	// 添加提示文字

    // 新建一个View 功能打开菜单
	View * openMenu = creatView(14, 100, 70, 870, 350);	// 创建按键
	clearCanvas(openMenu->canvas, 0x0000ffff);	// 设置按钮颜色
	drawString(openMenu->canvas, 5, 10, "MENU", 30, 0x00ffffff);	// 添加提示文字
	
	// 创建一个难度选择
	View * diffChange = creatView(20, 300, 600, 50, 10);	// 难度选择布局
	clearCanvas(diffChange->canvas, -1);	// 布局按钮颜色
	drawString(diffChange->canvas, 5, 5, "DIFFICULTY", 43, 0x00444444);	// 添加提示文字

	View * diffEasy = creatView(21, 200, 70, 50, 100); // 简单
	clearCanvas(diffEasy->canvas, 0x0066ccff);	// 设置按钮颜色
	drawString(diffEasy->canvas, 5, 5, "EASY", 40, 0x00ffffff);	// 添加提示文字

	View * diffNormal = creatView(22, 200, 70, 50, 250); // 普通
	clearCanvas(diffNormal->canvas, 0x0066ccff);	// 设置按钮颜色
	drawString(diffNormal->canvas, 5, 5, "NORMAL", 40, 0x00ffffff);	// 添加提示文字

	View * diffHard = creatView(23, 200, 70, 50, 400); // 困难
	clearCanvas(diffHard->canvas, 0x0066ccff);	// 设置按钮颜色
	drawString(diffHard->canvas, 5, 5, "HARD", 40, 0x00ffffff);	// 添加提示文字

    // 添加功能选择菜单
	View * menu = creatView(40, 300, 550, 600, 10);	// 创建菜单
	clearCanvas(menu->canvas, -1);	// 设置菜单颜色
	drawString(menu->canvas, 5, 10, "MENU", 40, 0x00ffffff);	// 添加提示文字

    View * menu_continue = creatView(41, 200, 70, 50, 100); // 点击继续游戏
    clearCanvas(menu_continue->canvas, 0x0066ccff);	// 设置按钮颜色
	drawString(menu_continue->canvas, 5, 10, "CONTINUE", 30, 0x00ffffff);	// 添加提示文字

    View * menu_restart = creatView(42, 200, 70, 50, 200); // 点击重新开始游戏
    clearCanvas(menu_restart->canvas, 0x0066ccff);	// 设置按钮颜色
	drawString(menu_restart->canvas, 5, 10, "RESTART", 30, 0x00ffffff);	// 添加提示文字

    View * menu_setdiff = creatView(43, 200, 70, 50, 300); // 点击重新开始游戏
    clearCanvas(menu_setdiff->canvas, 0x0066ccff);	// 设置按钮颜色
	drawString(menu_setdiff->canvas, 5, 10, "DIFFICULTY", 30, 0x00ffffff);	// 添加提示文字

    View * menu_players = creatView(44, 200, 70, 50, 400); // 点击显示玩家榜单
    clearCanvas(menu_players->canvas, 0x0066ccff);	// 设置按钮颜色
	drawString(menu_players->canvas, 5, 10, "PLAYERS", 30, 0x00ffffff);	// 添加提示文字

    View * menu_win = creatView(47, 200, 70, 50, 500); // 点击直接赢得游戏
    clearCanvas(menu_win->canvas, 0x00ff66cc);	// 设置按钮颜色
	drawString(menu_win->canvas, 5, 10, "TOUCH WIN", 30, 0x00ffffff);	// 添加提示文字
    drawString(menu_win->canvas, 5, 35, "ONLY FOR DEBUG", 20, 0x00ffffff);	// 添加提示文字

    // 创建一个提示弹窗
    View * messageBox = creatView(15, 400, 300, 312, 150);
    clearCanvas(messageBox->canvas, 0x00eeeeee);	// 设置弹窗背景颜色
    messageBox->state = 0;

    menu->state = 0;

    // 添加用户数据显示区域
    View * playersView = creatView(50, 400, 550, 50, 25); // 用户数据显示区域
    clearCanvas(playersView->canvas, 0x00666666);	// 设置背景颜色

    playersView->state = 0;

    //注册所有的View
    addView(screen, background);
    
    addView(background, diffChange);
	addView(diffChange, diffEasy);
	addView(diffChange, diffNormal);
	addView(diffChange, diffHard);

    addView(menu, menu_continue);
    addView(menu, menu_restart);
    addView(menu, menu_setdiff);
    addView(menu, menu_players);
    addView(menu, menu_win);

    addView(background, gamePlayView);
	addView(gamePlayView, openGrid);
	addView(gamePlayView, setFlag);
    addView(gamePlayView, openMenu);
	addView(gamePlayView, game->gameView);
	addView(gamePlayView, game->timer->view);
    addView(gamePlayView, mineNumView);
    addView(background, menu);
    addView(background, playersView);
    addView(background, messageBox);
	
}

int selectGrid(){
    GameMap * showMap = game->showMap;

    // 获取触摸的网格
    int grid[2];
    getTouchGrid(grid);

    char data;
    getMapData(showMap, grid[0], grid[1], &data);
    int i, j;
    
    if(data == 'h' || data == 'f'){  // 判断是否可以被选中
        // 清除上一次的选中
        for(j=0; j<game->height; j++){
            for(i=0; i<game->width; i++){
                getMapData(showMap, i, j, &data);
                if(data == 's'){
                    setMapData(showMap, i, j, 'h');
                }
                else if(data == 'g'){
                    setMapData(showMap, i, j, 'f');
                }
            }
        }
        
        // 选中指定的格子
        printf("selectGrid: Select(%d, %d) \n", grid[0], grid[1]);
        getMapData(showMap, grid[0], grid[1], &data);
        if(data == 'h'){
            setMapData(showMap, grid[0], grid[1], 's');
        }
        else if(data == 'f'){
            setMapData(showMap, grid[0], grid[1], 'g');
        }
    }

    flashGameView();
    return 1;
}

// 弹窗提示
int gameMessageBox(int type){
    // 获取弹窗
    View *messageBox = getViewById(game->screen, 15);
    clearCanvas(messageBox->canvas, 0x00eeeeee);	// 设置弹窗背景颜色

    if(type == 0) { // 失败时弹窗
        drawString(messageBox->canvas, 130, 70, "FAILED", 50, 0x00ff0000);
    }
    else if(type == 1) { // 成功时弹窗
        drawString(messageBox->canvas, 120, 70, "SUCCESS", 50, 0x0000ff00);
    }
    drawString(messageBox->canvas, 120, 250, "TOUCH to RESTART", 20, 0x00444444);
    messageBox->state = 1;  // 弹窗设置为可见

    return 1;
}

static int gameOver(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    int x, y;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            char data;
            getMapData(sourceMap, x, y, &data);
            if(data == 'm'){
                setMapData(showMap, x, y, 'm');
            }
        }
    }
    flashGameView();
    game->state = 0;
    game->timer->cmd[0] = 2;

    gameMessageBox(0);

    return 1;
}

static int gameWin(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    int x, y;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            char data;
            getMapData(sourceMap, x, y, &data);
            if(data == 'm'){
                setMapData(showMap, x, y, 'f');
            }
        }
    }
    flashGameView();

    // 向玩家列表中添加这位获胜者的数据
    // 合成玩家name, 以当前时间作为玩家的名字
    // 获取当前时间
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    char name[16];
    sprintf(name, "%04d%03d%02d%02d%02d",t->tm_year+1900, t->tm_yday, t->tm_hour, t->tm_min, t->tm_sec);
    printf("gameWin: %s \n", name);
    // 添加玩家
    addPlayer(name, game->timer->t);

    game->state = 0;
    game->timer->cmd[0] = 2;

    gameMessageBox(1);

    return 1;
}

int openSelectedGrid(){
    GameMap * sourceMap = game->sourceMap;
    GameMap * showMap = game->showMap;

    // 找到被选中的格子
    int x, y;
    int openRes;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
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

static void updateMineNum(){
    GameMap * showMap = game->showMap;

    int mineNum = 0;
    int x, y;
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
            char data;
            getMapData(showMap, x, y, &data);
            if(data == 'f' || data == 'g'){
                mineNum++;
            }
        }
    }

    mineNum = game->diffic - mineNum < 0 ? 0 : game->diffic - mineNum;

    View * mineNumView = getViewById(game->screen, 18);
    if(mineNumView == NULL) return ;

    char mineNumStr[10];
    sprintf(mineNumStr, "%d", mineNum);
    clearCanvas(mineNumView->canvas, -1);
    drawString(mineNumView->canvas, 0, 0, mineNumStr, mineNumView->canvas->height, game->timer->color);
}

// 将一个格子插旗
static int flagGrid(int x, int y){
    GameMap * showMap = game->showMap;

    char data;
    getMapData(showMap, x, y, &data);
    if(data == 's'){
        setMapData(showMap, x, y, 'f');
        updateMineNum();
        return 1;
    }
    else if(data == 'g'){
        setMapData(showMap, x, y, 'h');
        updateMineNum();
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
    for(y=0; y<game->height; y++){
        for(x=0; x<game->width; x++){
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
    initSaolei(game);
    setViewById(game->screen, 15, 0);
}

int diffselect(){
    setViewById(game->screen, 10, 0);
    setViewById(game->screen, 20, 1);
}

int diffChange2Easy(){
    game->width = 8;
    game->height = 10;
    game->diffic = 10;

    initSaolei(game);

    setViewById(game->screen, 10, 2);
    setViewById(game->screen, 20, 0);

    return 1;
}

int diffChange2Normal(){
    game->width = 16;
    game->height = 16;
    game->diffic = 40;

    initSaolei(game);

    setViewById(game->screen, 10, 2);
    setViewById(game->screen, 20, 0);

    return 1;
}

int diffChange2Hard(){
    game->width = 22;
    game->height = 22;
    game->diffic = 99;

    initSaolei(game);

    setViewById(game->screen, 10, 2);
    setViewById(game->screen, 20, 0);

    return 1;
}

int menuOpen(){
    setViewById(game->screen, 10, 0);
    setViewById(game->screen, 40, 1);

    return 1;
}

int menuClose(){
    setViewById(game->screen, 10, 2);
    setViewById(game->screen, 40, 0);

    return 1;
}

/**
 * 玩家统计
*/
struct Players{
    Player **players;
    int *num;
};

static void getPlayers(struct Players * players){
    switch (game->diffic)
    {
    case 10:
        players->players = game->players->easyPlays;
        players->num = &game->players->easyPlaysNum;
        break;
    case 40:
        players->players = game->players->normalPlays;
        players->num = &game->players->normalPlaysNum;
        break;
    case 99:
        players->players = game->players->hardPlays;
        players->num = &game->players->hardPlaysNum;
        break;
    default:
        break;
    }
}

// 根据分数对玩家进行排序
static int sortPlays(){
    struct Players players;
    getPlayers(&players);

    printf("sortPlays: start  \n");
    int i,j;
    for(i=0; i<*players.num; i++){
        int t = players.players[i]->point;
        for(j=i+1; j<*players.num; j++){
            if(players.players[j]->point < t){
                Player *tp = players.players[j];
                players.players[j] = players.players[i];
                players.players[i] = tp;
            }
        }
    }
    return 1;
}

static int addPlayer(char *name, int point){
    Player * player = malloc(sizeof(Player));
    strncpy(player->name, name, 16);
    player->point = point;

    // 获取玩家存储地址
    struct Players players;
    getPlayers(&players);

    if(*players.num < 100){
        players.players[*players.num] = player;
        *players.num += 1;
    }
    else{
        if(players.players[*players.num - 1]->point > point){
            players.players[*players.num - 1] = player;
        }
    }
    sortPlays();
    savePlayersData();
}

static int savePlayersData(){
    FILE *fb = fopen("GameData.hex", "w");
    if(fb == NULL){
        printf("savePlayersData: Open file error \n");
        return -1;
    }

    int i;
    // 保存简单玩家数据
    fwrite(&game->players->easyPlaysNum, sizeof(int), 1, fb);
    for(i=0; i<game->players->easyPlaysNum; i++){
        fwrite(game->players->easyPlays[i], sizeof(Player), 1, fb);
    }

    // 保存普通玩家数据
    fwrite(&game->players->normalPlaysNum, sizeof(int), 1, fb);
    for(i=0; i<game->players->normalPlaysNum; i++){
        fwrite(game->players->normalPlays[i], sizeof(Player), 1, fb);
    }

    // 保存困难玩家数据
    fwrite(&game->players->hardPlaysNum, sizeof(int), 1, fb);
    for(i=0; i<game->players->hardPlaysNum; i++){
        fwrite(game->players->hardPlays[i], sizeof(Player), 1, fb);
    }

    fclose(fb);

    printf("savePlayersData: %d   \n", game->players->easyPlaysNum+game->players->normalPlaysNum+game->players->hardPlaysNum);
    return 0;
}

static int loadPlayersData(){
    FILE *fb = fopen("GameData.hex", "r");
    if(fb == NULL){
        printf("loadPlayersData: Open file error \n");
        return -1;
    }
    int i;
    Player * player;
    // 读取简单玩家数据
    fread(&game->players->easyPlaysNum, sizeof(int), 1, fb);
    printf("loadPlayersData: easyPlaysNum %d \n", game->players->easyPlaysNum);
    for(i=0; i<game->players->easyPlaysNum; i++){
        player = malloc(sizeof(Player)+4);
        fread(player, sizeof(Player), 1, fb);
        printf("loadPlayersData: Playername %s \n", player->name);
        printf("loadPlayersData: Playerpoint %d \n", player->point);
        puts("");
        game->players->easyPlays[i] = player;
    }

    // 读取普通玩家数据
    fread(&game->players->normalPlaysNum, sizeof(int), 1, fb);
    for(i=0; i<game->players->normalPlaysNum; i++){
        player = malloc(sizeof(Player));
        fread(player, sizeof(Player), 1, fb);
        game->players->normalPlays[i] = player;
    }

    // 读取困难玩家数据
    fread(&game->players->hardPlaysNum, sizeof(int), 1, fb);
    for(i=0; i<game->players->hardPlaysNum; i++){
        player = malloc(sizeof(Player));
        fread(player, sizeof(Player), 1, fb);
        game->players->hardPlays[i] = player;
    }

    printf("loadPlayersData: %d   \n", game->players->easyPlaysNum+game->players->normalPlaysNum+game->players->hardPlaysNum);

    fclose(fb);
}

int showPlayers(){
    View *playersView = getViewById(game->screen, 50);
    clearCanvas(playersView->canvas, 0x00666666);
    drawString(playersView->canvas, 5, 10, "RANKINGS: TOUCH TO CLOSE", 22, 0x00ffffff);


    printf("showPlayers: Players  \n");
    // 获取玩家存储地址
    struct Players players;
    getPlayers(&players);

    int y = 50;
    int i;
    for(i=0; i<*players.num; i++){
        char playerInfo[60];
        sprintf(playerInfo, "%d. NAME: %s   TIME: %d",i+1, players.players[i]->name, players.players[i]->point);
        drawString(playersView->canvas, 20, y+i*20, playerInfo, 20, 0x00ffffff);
    }

    setViewById(game->screen, 50, 1);
    setViewById(game->screen, 40, 0);
    printf("showPlayers: PlayersNum %d  \n", *players.num);
    return *players.num;
}

int cheat_win(){
    gameWin();

    return 1;
}

int closePlayerView(){
    setViewById(game->screen, 50, 0);
    setViewById(game->screen, 40, 1);
}