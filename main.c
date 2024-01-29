#include "main.h"

Screen * initScreen(){
	Screen * screen = malloc(sizeof(Screen));
	screen->width = 1024;
	screen->height = 600;

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
	screen->p = ptr;

	return screen;
}

void * flashAllView(void * argv){
	// 初始化主屏幕
	Screen * screen = initScreen();
	clearScreen(screen, 0x00ffffff);

	// 获取参数
	Screen ** views = (Screen **)argv;
	int * viewNum = (int *)*views;
	printf("flashAllView: Views num %d \n", *viewNum);

	int i;
	while(1){
		for(i=1; i<=*viewNum; i++){
			buflash(screen, views[i], views[i]->relatX, views[i]->relatY);
		}
		usleep(33000);
	}
}


int main(int argc, char * argv[]){
	srand((unsigned)time(NULL));
	
	// 初始化触控坐标
	Point * touchP = initTouch();
	
	// 新建一个点击Screen 功能是开启一个格子
	Screen gridOpenView;
	gridOpenView.width = 200;
	gridOpenView.height = 100;
	gridOpenView.relatX = 650;
	gridOpenView.relatY = 30;
	gridOpenView.p = malloc(gridOpenView.height * gridOpenView.width * sizeof(int));
	clearScreen(&gridOpenView, 0x0000ff00);

	// 新建一个点击Screen 功能重新开始游戏
	Screen restartGameView;
	restartGameView.width = 200;
	restartGameView.height = 100;
	restartGameView.relatX = 650;
	restartGameView.relatY = 330;
	restartGameView.p = malloc(restartGameView.height * restartGameView.width * sizeof(int));
	clearScreen(&restartGameView, 0x00ffff00);

	// 初始化一个游戏
	SaoleiGame * game = creatSaolei();

	// 建立新线程刷新屏幕
	int screenNum = 3;
	Screen * views[] = {(Screen *)&screenNum, game->gameView, &gridOpenView, &restartGameView};
	pthread_t t;
	pthread_create(&t, NULL, flashAllView, (void *)views);

	// 保存游戏View中的触点相对位置
	int touchInGame[2];
	int touchInGame_last[2];

	while(1){
		usleep(5000);

		// 获取gameScreen点击事件
		if(getRelatPoints(game->gameView, touchP, touchInGame) != -1){
			if(touchInGame_last[0] != touchInGame[0] && touchInGame_last[1] != touchInGame[1]){
				// 获取触摸的网格坐标
				selectGrid(game->gameView, game->showMap, touchInGame);
				flashGameScreen(game->gameView, game->showMap);
				touchInGame_last[0] = touchInGame[0];
				touchInGame_last[1] = touchInGame[1];
			}
		}
		
		// 获取gridOpenView点击
		if(viewIsTouched(&gridOpenView, touchP)){
			printf("main: gridOpenView is be touched \n");
			
			int openRes = openSelectedGrid(game->sourceMap, game->showMap);
			if(openRes == 1){
				printf("Game Over \n");
				flashGameScreen(game->gameView, game->sourceMap);
				break;
			}
			else if(openRes == 6){
				printf("Game Win! \n");
				break;
			}
			flashGameScreen(game->gameView, game->showMap);
		}

		// 获取restartGameView点击
		if(viewIsTouched(&restartGameView, touchP)){
			printf("main: restartGameView is be touched \n");
			delSaolei(game);
			game = creatSaolei(); 
		}
	}

	usleep(1000000);
	pthread_cancel(t);
}