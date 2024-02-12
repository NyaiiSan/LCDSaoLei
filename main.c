#include "main.h"

// 保存整个程序窗口
View * screen;

View * initScreen(){
	screen = malloc(sizeof(View));
	screen->id = -1;
	screen->marginsX = screen->marginsY = 0;
	screen->state = 2;

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

	// 初始化一个游戏
	SaoleiGame * game = creatSaolei();
	game->screen = screen;

	/*设置游戏布局*/
	initSaoleiLayout(game);

	// 建立新线程刷新屏幕
	pthread_t t;
	pthread_create(&t, NULL, threadFlashView, (void *)screen);
	
	// 开始View事件监听
	printf("ready to initTouchEvent \n");
	initTouchEvent(screen);

	diffselect();

	while(1){
		usleep(100000);
	}

	usleep(1000000);
	pthread_cancel(t);
}