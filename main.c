#include "main.h"

// 保存整个程序窗口
View * screen;

View * initScreen(){
	screen = malloc(sizeof(View));
	screen->id = -1;
	screen->marginsX = screen->marginsY = 0;
	screen->state = 1;

	Canvas * canvas = malloc(sizeof(Canvas));
	canvas->width = 1024;
	canvas->height = 600;

	int fb;
	int * ptr;
	fb = open("/dev/fb0", O_RDWR);
	if (fb == 0){
        perror("file Error\n");
		exit(1);
    }

    ptr = mmap(NULL, 1024*600*4 , PROT_READ|PROT_WRITE, MAP_SHARED , fb , 0);
	if (ptr == NULL){
        perror("mmap Error\n");
		exit(1);
    }
	canvas->p = ptr;

	screen->canvas = canvas;

	return screen;
}

int main(int argc, char * argv[]){
	srand((unsigned)time(NULL));
	initScreen();
	clearCanvas(screen->canvas, 0x00ffffff);
	
	/*设置布局*/

	// 新建一张背景
	Canvas * backgroundImg = getBmpImg("bg.bmp");	// 导入背景图片
	View * background = creatView(0, screen->canvas->width, screen->canvas->height, 0, 0); // 创建背景控件
	buflash(background->canvas, backgroundImg, 0, 0);	// 设置控件背景

	// 新建一个点击View 功能是开启一个格子
	View * openGrid = creatView(2, 100, 70, 870, 100);	// 创建按键
	clearCanvas(openGrid->canvas, 0x0000ff00);	// 设置按钮颜色
	drawString(openGrid->canvas, 5, 10, "OPEN", 30, 0x00ffffff);	// 添加提示文字

	// 新建一个View 功能是插旗子
	View * setFlag = creatView(3, 100, 70, 870, 200);	// 创建按键
	clearCanvas(setFlag->canvas, 0x00ff0000);	// 设置按钮颜色
	drawString(setFlag->canvas, 5, 10, "FLAG", 30, 0x00ffffff);	// 添加提示文字

	// 新建一个View 功能重新开始游戏
	View * restartGame = creatView(4, 100, 70, 870, 350);	// 创建按键
	clearCanvas(restartGame->canvas, 0x0000ffff);	// 设置按钮颜色
	drawString(restartGame->canvas, 5, 10, "RESTART", 30, 0x00ffffff);	// 添加提示文字
	
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

	addView(diffChange, diffEasy);
	addView(diffChange, diffNormal);
	addView(diffChange, diffHard);

	// 初始化一个游戏
	SaoleiGame * game = creatSaolei();
	game->screen = screen;

	// printf("main: game create over \n");
	//注册所有的View
	addView(screen, background);
	addView(background, openGrid);
	addView(background, setFlag);
	addView(background, restartGame);
	addView(background, game->gameView);
	addView(background, game->timer->view);
	addView(background, diffChange);

	// 开始游戏计时器
	timeStart(game->timer);

	// 建立新线程刷新屏幕
	pthread_t t;
	pthread_create(&t, NULL, threadFlashView, (void *)screen);
	
	// 开始View事件监听
	printf("ready to initTouchEvent \n");
	initTouchEvent(screen);

	hideViewById(screen, 1);

	while(1){
		usleep(100000);
	}

	usleep(1000000);
	pthread_cancel(t);
}