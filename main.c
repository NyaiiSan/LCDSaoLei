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
	int viewNum = 2;
	printf("flashAllView: Views num %d \n", viewNum);

	int i;
	while(1){
		for(i=1; i<viewNum; i++){
			buflash(screen, views[i], views[i]->relatX, views[i]->relatY);
		}
		usleep(33000);
	}
}


int main(int argc, char * argv[]){
	srand((unsigned)time(NULL));
	
	// Screen * screen = initScreen();
	// clearScreen(screen, 0x00ffffff);

	// 初始化触控坐标
	Point * touchP = initTouch();
	
	// 初始化游戏
	GameMap sourceMap = creatGameMap(8, 10, 10);
    GameMap maskMap = creatMaskMap(sourceMap);
	// 建立一个游戏区域
	Screen gameView;
	gameView.height = 600;
	gameView.width = 600;
	gameView.relatX = gameView.relatY = 0;
	gameView.p = malloc(gameView.height * gameView.width * sizeof(int));
	clearScreen(&gameView, 0x00ffffff);
	flashGameScreen(&gameView, &maskMap);

	// 建立一个功能

	int screenNum = 1;
	Screen * views[] = {(Screen * )&screenNum, &gameView};
	pthread_t t;
	pthread_create(&t, NULL, flashAllView, (void *)views);

	int x = -1;
	int y = -1;

	while(1){
		usleep(50000);

		// 获取游戏View中的触点位置
		int posi[2];
		if(getTouchGrid(&gameView, &maskMap, touchP, posi) == -1){
			continue;
		}
		else if(x == posi[0] && y == posi[1]){
			continue;
		}
		x = posi[0];
		y = posi[1];
		
		selectGrid(&maskMap, x, y, &gameView);
		printf("main: select(%d, %d)", x, y);
		

		// int openRes = openGrid(sourceMap, &maskMap, x, y);
		// if(openRes == 1){
		// 	printf("Game Over \n");
		// 	flashGameScreen(&gameView, sourceMap);
		// 	break;
		// }
		// else if(openRes == 6){
		// 	printf("Game Win! \n");
		// 	break;
		// }
		// flashGameScreen(&gameView, maskMap);
	}

	usleep(1000000);
}