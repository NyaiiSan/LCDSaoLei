#include "main.h"

View * creatView(int width, int height, int marginsX, int marginsY){
    View * view = malloc(sizeof(View));

	// 为View创建画布
    Canvas * canvas = malloc(sizeof(Canvas));
	canvas->width = width;
	canvas->height = height;
    canvas->p = malloc(canvas->height * canvas->width * sizeof(int));
    // 初始化view画布中的内容
	clearCanvas(canvas, -1);

    // 设置View
    view->state = 1;
	view->marginsX = marginsX;
	view->marginsY = marginsY;
    view->subViewsNum = 0;
    view->canvas = canvas;

    return view;
}

int delView(View * view){
    int i;
    for(i=0; i<view->subViewsNum; i++){
        delView(view->subViews[i]);
    }
    free(view->canvas->p);
    free(view->canvas);
    free(view);
}

int addView(View * targetView, View * view){
    if(targetView == NULL || view == NULL){
        return -1;
    }
    targetView->subViews[targetView->subViewsNum] = view;
    targetView->subViewsNum++;
    return 1;
}

int flashView(View * view){
    if(view->state == 0) return 0;

    int flashedNum = 0;
    if(view->subViewsNum == 0){
        return flashedNum;
    }

    int i;
    for(i=0; i<view->subViewsNum; i++){
        flashedNum += flashView(view->subViews[i]);
        buflash(view->canvas, view->subViews[i]->canvas, view->subViews[i]->marginsX, view->subViews[i]->marginsY);
        flashedNum++;
    }

    return flashedNum;
}

void * threadFlashView(void * argv){
    while(1){
        printf("threadFlashView: Flash: %d \n", flashView(argv));
        usleep(33000);
    }
}