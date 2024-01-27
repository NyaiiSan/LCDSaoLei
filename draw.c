#include "main.h"

extern int fb;
extern int * ptr;

int clearScreen(Screen * screen, int color){
    int x, y;
    for(x=0; x<screen->width; x++){
        for(y=0; y<screen->height; y++){
            drawPoint(screen, x, y, color);
        }
    }
}

int drawPoint(Screen * screen, int x, int y, int color){
    if(x < 0 | x >= screen->width | y < 0 | y >= screen->height){
        printf("DrawPoint: Out of screen \n");
         return -1;
    }
    *(screen->p + screen->width * y + x) = color;
}

int drawRect(Screen * screen, int sx, int sy, int width, int height, int color){
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;

    int tx, ty;
    tx = sx + width;
    ty = sy + height;
    if(tx > screen->width) tx = screen->width;
    if(ty > screen->height) ty = screen->height;
    int x,y;
    for(x=sx; x<tx; x++){
        for(y=sy; y<ty; y++){
            drawPoint(screen, x, y, color);
        }
    }
    return 1;
}

int drawCircle(Screen * screen, int sx, int sy, int r, int color){
    int x, y;
    for(x=(sx-r>0?sx-r:0); x < (sx+r<screen->width ? sx+r: screen->width); x++){
        for(y=(sy-r>0?sy-r:0); y<(sy+r<screen->height ? sy+r: screen->height); y++){
            if(pointsDistance(sx, sy, x, y) < r){
                drawPoint(screen, x, y, color);
            }
        }
    }
}

int drawTaiji(Screen * screen, int sx, int sy, int r){
    // 创建一块缓冲区
    Screen buf;
    buf.height = 2*r;
    buf.width = 2*r;
    buf.p = malloc(4 * r * r * sizeof(int));

    clearScreen(&buf, -1);
    int x, y;
    for(x=0; x<2*r; x++){
        for(y=0; y<2*r; y++){
            if(pointsDistance(r, r, x, y) < r){
                if(x<r){
                    drawPoint(&buf, x, y, 0x00000000);
                }
                else{
                    drawPoint(&buf, x, y, 0x00ffffff);
                }
            }
        }
    }

    drawCircle(&buf, r, r/2, r/2, 0x00000000);
    drawCircle(&buf, r, r/2, r/6, 0x00ffffff);

    drawCircle(&buf, r, r+(r/2), r/2, 0x00ffffff);
    drawCircle(&buf, r, r+(r/2), r/6, 0x00000000);

    buflash(screen, &buf, sx-r, sy-r);

    free(buf.p);
    return 0;
}

int buflash(Screen * screen, Screen * buf, int sx, int sy){
    int x, y;
    int color;
    for(y=0; y<buf->height; y++){
        for(x=0; x<buf->width; x++){
            color = *(buf->p + buf->width * y + x);
            if(color == -1) continue;
            drawPoint(screen, sx+x, sy+y, color);
        }
    }
    return 1;
}

Screen getBmpImg(char * path){
    BmpImg * img = openBmpImg(path);
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
    free(img);
    return image;
}

int showBmpImg(Screen * screen, BmpImg * img, int sx, int sy){
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

	buflash(screen, &image, sx, sy);
}

int drawAnimeMove(Screen * screen, Screen buf, Anime anime){
    
    //计算x，y方向的速度
    int xd = anime.tx - anime.sx;
    int yd = anime.ty - anime.sy;

    int step = 100/anime.speed;

    // 计算xy方向的速度
    int xs = xd/step;
    int ys = yd/step;

    int x = anime.sx;
    int y = anime.sy;
    
    int i;
    for(i=0; i<step; i++){
        buflash(screen, &buf, x, y);
        usleep(40000);
        drawRect(screen, x, y, buf.width, buf.height, 0x00ffffff);
        x = x+xs;
        y = y+ys;
    }
}

int drawAnimeGrad(Screen * screen, Screen buf, Anime anime){
    int step = anime.speed;
    int i;
    for(i=1; i<100; i+=step){
        printf("%d \n", i);
        Screen newBuf;
        newBuf.width = buf.width;
        newBuf.height = buf.height;
        newBuf.p = malloc(newBuf.height*newBuf.width*4);
        int x, y;
        for(y=0; y < newBuf.height; y++){
            for(x=0; x<newBuf.width; x++){
                int color = buf.p[buf.width*y + x];
                int newColor = 0x00000000;
                char * c = (char*)&color;
                unsigned char r = *(c+2);
                unsigned char g = *(c+1);
                unsigned char b = *(c);
                
                r = 0xff + (float)(r - 0xff) * (100.00/i);
                g = 0xff + (float)(g - 0xff) * (100.00/i);
                b = 0xff + (float)(b - 0xff) * (100.00/i);

                newColor = r << 16 | g << 8 | b;

                newBuf.p[buf.width*y + x] = newColor;
            }
        }
        buflash(screen, &newBuf, anime.sx, anime.sy);
        free(newBuf.p);
    }
}

int drawChar(Screen * screen, int sx, int sy, char c, int width, int height, int color){
    
    // 获取字符的字形
    CharFont font = getCharFont(c);
    
    // 创建一个字符Screen
    Screen charScreen;
    charScreen.width = width;
    charScreen.height = height;
    charScreen.p = malloc(width*height*sizeof(int));

    // 初始化内容
    clearScreen(&charScreen, -1);

    // 绘制字符
    
    // 计算字符每个像素的像素宽度
    int pixSize;

    int wpixSize = charScreen.width / font.width; // 宽度能取得的最大像素
    int hpixSize = charScreen.height / font.height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (charScreen.width - font.width * pixSize) / 2;
    int ymargin = (charScreen.height - font.height * pixSize) / 2;

    // 绘制字符
    int x, y;
    for(y=0; y<font.height; y++){
        for(x=0; x<font.width; x++){
            if(*(font.p + y*font.width + x) != 0){
                drawRect(&charScreen, xmargin + pixSize * x, ymargin + pixSize * y, pixSize, pixSize, color);
            }
        }
    }

    buflash(screen, &charScreen, sx, sy);
    free(charScreen.p);
    free(font.p);

    return 1;
}