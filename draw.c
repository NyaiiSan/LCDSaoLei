#include "main.h"

extern int fb;
extern int * ptr;

int clearCanvas(Canvas * canvas, int color){
    int x, y;
    for(x=0; x<canvas->width; x++){
        for(y=0; y<canvas->height; y++){
            drawPoint(canvas, x, y, color);
        }
    }
}

int drawPoint(Canvas * canvas, int x, int y, int color){
    if(x < 0 | x >= canvas->width | y < 0 | y >= canvas->height){
        // printf("DrawPoint: Out of canvas \n");
         return -1;
    }
    *(canvas->p + canvas->width * y + x) = color;
}

int drawRect(Canvas * canvas, int sx, int sy, int width, int height, int color){
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;

    int tx, ty;
    tx = sx + width;
    ty = sy + height;
    if(tx > canvas->width) tx = canvas->width;
    if(ty > canvas->height) ty = canvas->height;
    int x,y;
    for(x=sx; x<tx; x++){
        for(y=sy; y<ty; y++){
            drawPoint(canvas, x, y, color);
        }
    }
    return 1;
}

int drawCircle(Canvas * canvas, int sx, int sy, int r, int color){
    int x, y;
    for(x=(sx-r>0?sx-r:0); x < (sx+r<canvas->width ? sx+r: canvas->width); x++){
        for(y=(sy-r>0?sy-r:0); y<(sy+r<canvas->height ? sy+r: canvas->height); y++){
            if(pointsDistance(sx, sy, x, y) < r){
                drawPoint(canvas, x, y, color);
            }
        }
    }
}

int drawTaiji(Canvas * canvas, int sx, int sy, int r){
    // 创建一块缓冲区
    Canvas buf;
    buf.height = 2*r;
    buf.width = 2*r;
    buf.p = malloc(4 * r * r * sizeof(int));

    clearCanvas(&buf, -1);
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

    buflash(canvas, &buf, sx-r, sy-r);

    free(buf.p);
    return 0;
}

int buflash(Canvas * canvas, Canvas * buf, int sx, int sy){
    int x, y;
    int color;
    for(y=0; y<buf->height; y++){
        for(x=0; x<buf->width; x++){
            color = *(buf->p + buf->width * y + x);
            if(color == -1) continue;
            drawPoint(canvas, sx+x, sy+y, color);
        }
    }
    return 1;
}

Canvas * getBmpImg(char * path){
    BmpImg * img = openBmpImg(path);
    Canvas * image = malloc(sizeof(Canvas));
	image->width = img->width;
	image->height = img->height;
	image->p = malloc(image->width * image->height * sizeof(int));

	int x, y;
	int bufSize = 0;

    // 将图片保存到Canvas
	for(y=0; y<image->height; y++){
		for(x=0; x<image->width; x++){
			char * cp = (img->p + (img->depth/8) * (y * img->width + x));
			int color = 0;
			color = *cp | *(cp+1) << 8 | *(cp+2) << 16;
			*(image->p + (image->width * image->height) - (image->width * y + (image->width) - x)) = color;
			bufSize ++;
		}
	}
    free(img);
    return image;
}

int showBmpImg(Canvas * canvas, BmpImg * img, int sx, int sy){
    Canvas image;
	image.width = img->width;
	image.height = img->height;
	image.p = malloc(image.width * image.height * sizeof(int));

	int x, y;
	int bufSize = 0;

    // 将图片保存到Canvas
	for(y=0; y<image.height; y++){
		for(x=0; x<image.width; x++){
			char * cp = (img->p + (img->depth/8) * (y * img->width + x));
			int color = 0;
			color = *cp | *(cp+1) << 8 | *(cp+2) << 16;
			*(image.p + (image.width * image.height) - (image.width * y + (image.width) - x)) = color;
			bufSize ++;
		}
	}

	buflash(canvas, &image, sx, sy);
}

int drawAnimeMove(Canvas * canvas, Canvas buf, Anime anime){
    
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
        buflash(canvas, &buf, x, y);
        usleep(40000);
        drawRect(canvas, x, y, buf.width, buf.height, 0x00ffffff);
        x = x+xs;
        y = y+ys;
    }
}

int drawAnimeGrad(Canvas * canvas, Canvas buf, Anime anime){
    int step = anime.speed;
    int i;
    for(i=1; i<100; i+=step){
        printf("%d \n", i);
        Canvas newBuf;
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
        buflash(canvas, &newBuf, anime.sx, anime.sy);
        free(newBuf.p);
    }
}

int drawChar(Canvas * canvas, int sx, int sy, char c, int width, int height, int color){
    
    // 获取字符的字形
    CharFont font = getCharFont(c);
    
    // 创建一个字符Canvas
    Canvas charCanvas;
    charCanvas.width = width;
    charCanvas.height = height;
    charCanvas.p = malloc(width*height*sizeof(int));

    // 初始化内容
    clearCanvas(&charCanvas, -1);

    // 绘制字符
    
    // 计算字符每个像素的像素宽度
    int pixSize;

    int wpixSize = charCanvas.width / font.width; // 宽度能取得的最大像素
    int hpixSize = charCanvas.height / font.height; // 高度能取得的最大像素
    pixSize = wpixSize > hpixSize ? hpixSize : wpixSize;

    // 计算外边距
    int xmargin = (charCanvas.width - font.width * pixSize) / 2;
    int ymargin = (charCanvas.height - font.height * pixSize) / 2;

    // 绘制字符
    int x, y;
    for(y=0; y<font.height; y++){
        for(x=0; x<font.width; x++){
            if(*(font.p + y*font.width + x) != 0){
                drawRect(&charCanvas, xmargin + pixSize * x, ymargin + pixSize * y, pixSize, pixSize, color);
            }
        }
    }

    buflash(canvas, &charCanvas, sx, sy);
    free(charCanvas.p);
    free(font.p);

    return 1;
}

int drawString(Canvas * canvas, int x, int y, char * str, int size, int color){
    // 计算字符串的长度
    int length = strlen(str);

    // 设置一个字符间距
    int space = 0.5 * size;

    //绘制每一个字符
    int i;
    for(i=0; i<length; i++){
        drawChar(canvas, x + space*i, y, *str, space, size, color);
        str++;
    }

    return 1;
}


int *saveAs(Canvas *canvas){
    // 计算填充字节大小
    int filler;
    if((canvas->width * 3)%4 == 0){
        filler = 0;
    }
    else{
        filler = 4 - ( (canvas->width * 3) % 4 );
    }
    // 初始化数据头
    BMIHeader *bmiHeader = malloc(sizeof(BMIHeader));
    bmiHeader->biSize = 40;
    bmiHeader->biWidth = canvas->width;
    bmiHeader->biHeight = canvas->height;
    bmiHeader->biPlanes = 1;
    bmiHeader->biBitCount = 24;
    bmiHeader->biCompression = 0;
    bmiHeader->biSize = (canvas->width*3 + filler) * canvas->height;
    bmiHeader->biXPelsPerMeter = 0;
    bmiHeader->biYPelsPerMeter = 0;
    bmiHeader->biClrUsed = 0;
    bmiHeader->biClrImportant = 0;

    // 写入图片数据
    char * biData = malloc(bmiHeader->biSize);
    int p = 0;
    int x,y;
    for(y=canvas->height; y>0; y--){
        for(x=0; x<canvas->width; x++){
            int color = *(canvas->p + x + canvas->width * y);
            unsigned char red = color>>16;
            unsigned char green = color>>8;
            unsigned char blue = color;
            biData[p++] = blue;
            biData[p++] = green;
            biData[p++] = red;
        }
        int j;
        for(j=0; j<filler; j++){
            biData[p++] = 0x00;
        }
    }

    // 初始化文件头
    BMFHeader bmfHeader;
    bmfHeader.bfType = 0x4d42;
    bmfHeader.bfSize = 40 + 14 + bmiHeader->biSize;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = 14;

    printf("saveAs: BMFHeader %d, BMIHeader %d,", sizeof(BMFHeader), sizeof(BMIHeader));

    // 保存为图片文件
    FILE *fb = fopen("ScreenShot.bmp", "wb");
    // 写入文件头
    fwrite(&bmfHeader, 2, 1, fb);
    fwrite(&bmfHeader.bfSize, 4, 1, fb);
    fwrite(&bmfHeader.bfReserved1, 2, 1, fb);
    fwrite(&bmfHeader.bfReserved2, 2, 1, fb);
    fwrite(&bmfHeader.bfOffBits, 4, 1, fb);
    fwrite(bmiHeader, 40, 1, fb);
    fwrite(biData, bmiHeader->biSize, 1, fb);

    fseek(fb, 14, 0);
    fputc(40, fb);
    fputc(0, fb);
    fputc(0, fb);
    fputc(0, fb);

    fclose(fb);
    free(biData);

    printf("saveAs: Image Saved  \n");
}