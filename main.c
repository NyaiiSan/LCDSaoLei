#include "main.h"

// 保存整个程序窗口
View * screen;

View * initScreen(){
	screen = malloc(sizeof(View));
	screen->id = 0;
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
	Canvas * background = getBmpImg("bg.bmp");
	// clearCanvas(screen->canvas, 0x00ffffff);
	buflash(screen->canvas, background, 0, 0);
	
	// 设置布局
	// 新建一个点击View 功能是开启一个格子
	View * openGrid = creatView(2, 100, 100, 700, 50);
	clearCanvas(openGrid->canvas, 0x0000ff00);

	// 新建一个View 功能是插旗子
	View * setFlag = creatView(3, 100, 100, 850, 50);
	clearCanvas(setFlag->canvas, 0x00ff0000);

	// 新建一个点击Canvas 功能重新开始游戏
	View * restartGame = creatView(4, 100, 100, 700, 250);
	clearCanvas(restartGame->canvas, 0x0000ffff);

	// 初始化一个游戏
	SaoleiGame * game = creatSaolei();

	addView(screen, openGrid);
	addView(screen, setFlag);
	addView(screen, restartGame);
	addView(screen, game->gameView);

	// 建立新线程刷新屏幕
	pthread_t t;
	pthread_create(&t, NULL, threadFlashView, (void *)screen);

	// 保存游戏View中的触点相对位置
	int touchInGame[2];
	int touchInGame_last[2];
	
	printf("ready to initTouchEvent \n");
	initTouchEvent(screen);

	while(1){
		usleep(5000);
		
	}

	usleep(1000000);
	pthread_cancel(t);
}