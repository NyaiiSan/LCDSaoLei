#include "main.h"

View * creatView(int id, int width, int height, int marginsX, int marginsY){
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
    view->id = id;

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

    int flashedNum = 0;
    if(view->subViewsNum == 0){
        return flashedNum;
    }

    int i;
    for(i=0; i<view->subViewsNum; i++){
        if(view->subViews[i]->state == 0) continue;
        flashedNum += flashView(view->subViews[i]);
        buflash(view->canvas, view->subViews[i]->canvas, view->subViews[i]->marginsX, view->subViews[i]->marginsY);
        flashedNum++;
    }

    return flashedNum;
}

void * threadFlashView(void * argv){
    while(1){
        int flashNum = flashView(argv);
        // printf("threadFlashView: Flash: %d \n", flashNum);
        usleep(33000);
    }
}

// 获取一个View在屏幕上的位置
// static Point * getViewPosi(View * screen, View * view){
//     // 初始化一个坐标
//     Point * posi = malloc(sizeof(Point));
//     posi->x = screen->marginsX;
//     posi->y = screen->marginsY;

//     // 在screen中寻找这个view
//     int i;
//     for(i=0; i<screen->subViewsNum; i++){
//         if(screen->subViews[i] == view){
//             posi->x += view->marginsX;
//             posi->y += view->marginsY;
//             return posi;
//         }
//         Point * tPoint = getViewPosi(screen->subViews[i], view);
//         if(tPoint != NULL){
//             posi->x += tPoint->x;
//             posi->y += tPoint->y;
//             free(tPoint);
//             return posi;
//         }
//     }
//     free(posi);
//     return NULL;
// }

// 获取触控点相对View的坐标
// static Point * getRelatCoor(View * screen, View * view, Point * touchP){
//     // 获取View在屏幕上的位置
//     Point * viewPosi = getViewPosi(screen, view);

//     Point *relaCoor;

//     // 判断触点是否在View的内部
//     if(touchP->x < viewPosi->x | touchP->x > viewPosi->x + view->canvas->width |\
//        touchP->y < viewPosi->y | touchP->y > viewPosi->y + view->canvas->height){
//         relaCoor = NULL;
//     }
//     else{
//         relaCoor = malloc(sizeof(Point));
//         relaCoor->x = touchP->x - viewPosi->x;
//         relaCoor->y = touchP->y - viewPosi->y;
//     }
    
//     free(viewPosi);
//     return relaCoor;
// }
static void touchEventFunction(View * view);

static void updateTouchEvent(View *view, Point *relatP){
    // printf("updateTouchEvent: Update View %d \n", view->id);
    int i;
    // 判断这个相对坐标是否合理
    if(relatP->x < 0 | relatP->x > view->canvas->width | \
       relatP->y < 0 | relatP->y > view->canvas->height){
        view->event.state = 0;
        for(i=0; i<view->subViewsNum; i++){
            View * subView = view->subViews[i];
            Point subViewRelatP;
            subViewRelatP.x = -1;
            subViewRelatP.y = -1;
            updateTouchEvent(subView, &subViewRelatP);
        }
        return;
    }

    view->event.state = 1;
    view->event.value[0] = relatP->x;
    view->event.value[1] = relatP->y;

    printf("updateTouchEvent: View %d have Event. relatP(%d, %d) \n", view->id, view->event.value[0], view->event.value[1]);
    touchEventFunction(view);

    for(i=0; i<view->subViewsNum; i++){
        View * subView = view->subViews[i];
        Point subViewRelatP;
        subViewRelatP.x = relatP->x - subView->marginsX;
        subViewRelatP.y = relatP->y - subView->marginsY;
        updateTouchEvent(subView, &subViewRelatP);
    }
}

// 更新触摸事件
static void * updateTouchEvent_t(void * argv){

    Point ** a = (Point **)argv;

    View * screen = (View *)*a;
    Point * touchP = *(a+1);

    while(1){
        usleep(1000);
        if(touchP->x < 0) continue;

        printf("updateTouchEvent_t: TouchP-(%d, %d) \n", touchP->x, touchP->y);
        updateTouchEvent(screen, touchP);
        touchP->x = -1;
        touchP->y = -1;
    }
}

// 初始化触摸事件
void initTouchEvent(View * screen){
    // 初始化触控坐标
	Point * touchP = initTouch();
    
    Point ** argv = malloc(sizeof(int *)*2);
    argv[0] = (Point *)screen;
    argv[1] = touchP;

    printf("initTouchEvent: %#x, %#x \n", argv, argv+1);

    printf("initTouchEvent: TouchP-(%d, %d) \n", ((Point *)(argv[1]))->x, ((Point *)(argv[1]))->y);

    // 事件更新
    pthread_t evenT;
    printf("ready to pthread_create updateTouchEvent_t \n");
    pthread_create(&evenT, NULL, updateTouchEvent_t, (void *)argv);
}

// 信号函数相关内容
// 触发了点击事件
static void touchEventFunction(View * view){
    switch (view->id)
    {
    case 1:
        selectGrid();
        break;
    case 2:
        openSelectedGrid();
        break;
    case 4:
        restartSaolei();
        break;
    default:
        break;
    }
}