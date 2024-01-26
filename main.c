#include "main.h"

Screen initScreen(){
	Screen screen;
	screen.width = 1024;
	screen.height = 600;

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
	screen.p = ptr;
	return screen;
}

int main(int argc, char * argv[]){
	srand((unsigned)time(NULL));
	Screen screen = initScreen();
	Screen img = getBmpImg("Header.bmp");

	struct point * point = malloc(sizeof(struct point));

	pthread_t t;
	pthread_create(&t, NULL, alwaysGet_Touch, (void *)point);

	int x,y;

	while(1){
		if(x==point->x && y==point->y) continue;
		drawRect(screen, x-100, y-100, 200, 200, 0x00ffffff);
		x = point->x;
		y = point->y;
		buflash(screen, img, point->x-100, point->y-100);
	}

	return 0;
}