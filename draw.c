#include "main.h"

extern int fb;
extern int * ptr;

int clearScreen(Screen screen, int color){
    int x, y;
    for(x=0; x<screen.width; x++){
        for(y=0; y<screen.height; y++){
            drawPoint(screen, x, y, color);
        }
    }
}

int drawPoint(Screen screen, int x, int y, int color){
    if(x < 0 | x >= screen.width | y < 0 | y >= screen.height){
        return -1;
    }
    *(screen.p + screen.width * y + x) = color;
}

int drawRect(Screen screen, int sx, int sy, int width, int height, int color){
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;

    int tx, ty;
    tx = sx + width;
    ty = sy + height;
    if(tx > screen.width) tx = screen.width;
    if(ty > screen.height) ty = screen.height;

    int x,y;
    for(x=sx; x<tx; x++){
        for(y=sy; y<ty; y++){
            drawPoint(screen, x, y, color);
        }
    }
    return 1;
}

int drawCircle(Screen screen, int sx, int sy, int r, int color){
    int x, y;
    for(x=(sx-r>0?sx-r:0); x < (sx+r<screen.width ? sx+r: screen.width); x++){
        for(y=(sy-r>0?sy-r:0); y<(sy+r<screen.height ? sy+r: screen.height); y++){
            if(pointsDistance(sx, sy, x, y) < r){
                drawPoint(screen, x, y, color);
            }
        }
    }
}

int drawTaiji(Screen screen, int sx, int sy, int r){
    // 创建一块缓冲区
    Screen buf;
    buf.height = 2*r;
    buf.width = 2*r;
    buf.p = malloc(4 * r * r * sizeof(int));

    clearScreen(buf, -1);
    int x, y;
    for(x=0; x<2*r; x++){
        for(y=0; y<2*r; y++){
            if(pointsDistance(r, r, x, y) < r){
                if(x<r){
                    drawPoint(buf, x, y, 0x00000000);
                }
                else{
                    drawPoint(buf, x, y, 0x00ffffff);
                }
            }
        }
    }

    drawCircle(buf, r, r/2, r/2, 0x00000000);
    drawCircle(buf, r, r/2, r/6, 0x00ffffff);

    drawCircle(buf, r, r+(r/2), r/2, 0x00ffffff);
    drawCircle(buf, r, r+(r/2), r/6, 0x00000000);

    buflash(screen, buf, sx-r, sy-r);

    free(buf.p);
    return 0;
}

int buflash(Screen screen, Screen buf, int sx, int sy){
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;

    int x, y;
    int color;
    for(y=0; y<buf.height; y++){
        if(y+sy > screen.height) break;

        for(x=0; x<buf.width; x++){
            if(x+buf.width > screen.width) break;

            color = *(buf.p + buf.width * y + x);
            if(color == -1) continue;

            drawPoint(screen, sx+x, sy+y, color);
        }
    }

    return 1;
}

int showBmpImg(Screen screen, BmpImg * img, int sx, int sy){
    Screen image;
	image.width = img->width;
	image.height = img->height;
	image.p = malloc(image.width * image.height * sizeof(int));

	int x, y;
	int bufSize = 0;

    // 将图片保存到Screen
	for(y=0; y<image.height; y++){
		for(x=0; x<image.width; x++){
			char * cp = (img->p + (img->depth/8) * (y * img->width + x));
			int color = 0;
			color = *cp | *(cp+1) << 8 | *(cp+2) << 16;
			*(image.p + (image.width * image.height) - (image.width * y + (image.width) - x)) = color;
			bufSize ++;
		}
	}

	buflash(screen, image, sx, sy);
}