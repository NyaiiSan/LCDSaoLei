#include "main.h"

int pointsDistance(int sx, int sy, int tx, int ty){
	int xd = sx > tx ? sx - tx : tx - sx;
	int yd = sy > ty ? sy - ty : ty - sy;

	return sqrt(xd*xd + yd*yd);
}

unsigned int color(unsigned char r, unsigned char g, unsigned char b){
	unsigned int color = 0x00000000;
	color = r << 16 | g << 8 | b;

	return color;
}

int * getButton(char * but){
	FILE * fb = fopen(but, "r");
	int * e = malloc(16 * sizeof(int));
	fgets((char*)e, 16 * sizeof(int), fb);
	fclose(fb);

	return e;
}

int randuint(int min, int max){
	int num = rand();
	num = num > 0 ? num : -num;
	num = min + num % (max-min);

	return num;
}