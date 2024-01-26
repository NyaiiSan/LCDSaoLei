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

	BmpImg * img = openBmpImg("Header.bmp");
	printf("Width = %d, Height = %d, Depth = %d \n", img->width, img->height, img->depth);

	clearScreen(screen, 0x00ffffff);

	while(1){
		int * e = getButton("/dev/input/event1");
		
		int i;
		for(i=0; i<12; i++){
			printf("%#x ", *(e+i));
		}
		// x: e+7
		// y: e+11
		puts("");
		free(e);

		if(*(e+7) == 0 || *(e+11)==0) continue;
		int x = *(e+7) - 100;
		int y = *(e+11) - 100;
		clearScreen(screen, 0x00ffffffff);
		showBmpImg(screen, img, x, y);
	}

	return 0;
}