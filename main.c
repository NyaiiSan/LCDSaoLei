#include "main.h"

View * initScreen(){
	View * screen = malloc(sizeof(View));
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
	View * screen = initScreen();
	clearCanvas(screen->canvas, 0x00888888);
	
	// 初始化触控坐标
	Point * touchP = initTouch();
	
	// 设置布局
	// 新建一个点击View 功能是开启一个格子
	View * openGrid = creatView(200, 100, 650, 50);
	clearCanvas(openGrid->canvas, 0x00ff00);

	// 新建一个点击Canvas 功能重新开始游戏
	View * restartGame = creatView(200, 100, 650, 330);
	clearCanvas(restartGame->canvas, 0x0000ffff);

	// 初始化一个游戏
	SaoleiGame * game = creatSaolei();

	addView(screen, openGrid);
	addView(screen, restartGame);
	addView(screen, game->gameView);

	// 建立新线程刷新屏幕
	pthread_t t;
	pthread_create(&t, NULL, threadFlashView, (void *)screen);

	// 保存游戏View中的触点相对位置
	int touchInGame[2];
	int touchInGame_last[2];

	while(1){
		usleep(5000);

		// 获取gameCanvas点击事件
		if(getRelatPoints(game->gameView, touchP, touchInGame) != -1){
			if(touchInGame_last[0] != touchInGame[0] && touchInGame_last[1] != touchInGame[1]){
				// 获取触摸的网格坐标
				selectGrid(game->gameView, game->showMap, touchInGame);
				game->gameView->state = 0;
				flashGameCanvas(game->gameView->canvas, game->showMap);
				game->gameView->state = 1;
				touchInGame_last[0] = touchInGame[0];
				touchInGame_last[1] = touchInGame[1];
			}
		}
		
		// 获取gridOpenView点击
		if(viewIsTouched(openGrid, touchP)){
			printf("main: gridOpenView is be touched \n");
			
			int openRes = openSelectedGrid(game->sourceMap, game->showMap);
			if(openRes == 1){
				printf("Game Over \n");
				flashGameCanvas(game->gameView->canvas, game->sourceMap);
				break;
			}
			else if(openRes == 6){
				printf("Game Win! \n");
				break;
			}
			flashGameCanvas(game->gameView->canvas, game->showMap);
		}

		// 获取restartGameView点击
		if(viewIsTouched(restartGame, touchP)){
			printf("main: restartGameView is be touched \n");
			delSaolei(game);
			game = creatSaolei(); 
		}
	}

	usleep(1000000);
	pthread_cancel(t);
}