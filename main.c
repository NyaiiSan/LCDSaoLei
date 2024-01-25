#include "main.h"

int * initScreen(){
	int fb;
	int * ptr;
	fb = open("/dev/fb0", O_RDWR);
	if (fb == 0){
        perror("file Error\n");
		exit(1);
    }

    ptr = mmap(NULL, Width*Height*4 , PROT_READ|PROT_WRITE, MAP_SHARED , fb , 0);
	if (ptr == NULL){
        perror("mmap Error\n");
		exit(1);
    }

	return ptr;
}

int main(int argc, char * argv[]){
	int * screen = initScreen();

	int x, y;
	x = 100;
	y = 200;
	int xs, ys;
	xs = 5;
	ys = 5;

	clear(screen);

	while(1){
		drawTaiji(screen, x, y, 100);
		usleep(33000);
		drawRect(screen, x-100, y-100, 200, 200, 0x00888888);
		
		x += xs;
		y += ys;
		
		if(x > Width - 100){
			xs = -5;
		}
		else if(x < 0 + 100){
			xs = 5;
		}
		if(y > Height - 100){
			ys = -5;
		}
		else if(y < 0 + 100){
			ys = 5;
		}
	}
}
