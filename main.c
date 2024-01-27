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
	clearScreen(screen, 0x00ffffff);
	
	GameMap sourceMap = creatGameMap(8, 10, 10);
    GameMap maskMap = creatMaskMap(sourceMap);
	drawGameMap(screen, maskMap, 0, 0, 600, 600);
	drawGameMap(screen, sourceMap, 800, 0, 200, 200);

	int x, y;
	while(1){
		printf("input: ");
		scanf("%d %d", &x, &y);
		if(openGrid(sourceMap, &maskMap, x, y) == 1){
			drawGameMap(screen, sourceMap, 0, 0, 600, 600);
			break;
		}
		drawGameMap(screen, maskMap, 0, 0, 600, 600);
		drawGameMap(screen, sourceMap, 800, 0, 200, 200);
	}
}