#include "main.h"

extern int fb;
extern int * ptr;

int clear(int * screen){
    int x, y;
    for(x=0; x<Width; x++){
        for(y=0; y<Height; y++){
            drawPoint(screen, x, y, 0x0088888888);
        }
    }
}

int drawPoint(int* p, int x, int y, int color){
    if(x < 0 | x > Width | y < 0 | y > Height){
        printf("draw Error! Out of screen. \n");
        return -1;
    }
    *(p + 1024 * y + x) = color;
}

int drawRect(int * screen, int sx, int sy, int width, int height, int color){
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;
    int tx, ty;
    tx = sx + width;
    ty = sy + height;
    if(tx > Width) tx = Width;
    if(ty > Height) ty = Height;

    int x,y;
    for(x=sx; x<tx; x++){
        for(y=sy; y<ty; y++){
            drawPoint(screen, x, y, color);
        }
    }

    return 1;
}

int drawCircle(int* screen, int sx, int sy, int r, int color){
    int x, y;
    for(x=(sx-r>0?sx-r:0); x < (sx+r<Width ? sx+r: Width); x++){
        for(y=(sy-r>0?sy-r:0); y<(sy+r<Height ? sy+r: Height); y++){
            if(pointsDistance(sx, sy, x, y) < r){
                drawPoint(screen, x, y, color);
            }
        }
    }
}

int drawTaiji(int* screen, int sx, int sy, int r){
    int x, y;
    for(x=(sx-r>0?sx-r:0); x < (sx+r<Width ? sx+r: Width); x++){
        for(y=(sy-r>0?sy-r:0); y<(sy+r<Height ? sy+r: Height); y++){
            if(pointsDistance(sx, sy, x, y) < r){
                if(x<sx){
                    drawPoint(screen, x, y, 0x00000000);
                }
                else{
                    drawPoint(screen, x, y, 0x00ffffff);
                }
            }
        }
    }

    drawCircle(screen, sx, sy-(r/2), r/2, 0x00000000);
    drawCircle(screen, sx, sy-(r/2), r/6, 0x00ffffff);

    drawCircle(screen, sx, sy+(r/2), r/2, 0x00ffffff);
    drawCircle(screen, sx, sy+(r/2), r/6, 0x00000000);

    return 0;
}