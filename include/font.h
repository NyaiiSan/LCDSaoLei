#ifndef __FONT_H__
#define __FONT_H__

#include "main.h"

typedef struct charfont{
    char c;
    int width;
    int height;
    char * p;
}CharFont;

CharFont getCharFont(char c);

#endif