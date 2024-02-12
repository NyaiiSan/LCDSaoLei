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

static int flashView(View * view, Canvas * frame, int x, int y){

    int flashedNum = 0;
    if(view->subViewsNum == 0){
        return flashedNum;
    }

    int i;
    for(i=0; i<view->subViewsNum; i++){
        if(view->subViews[i]->state == 0) continue; // 判断view是否为可刷新状态
        
        if(view->subViews[i]->state != 2){
            buflash(frame, view->subViews[i]->canvas, view->subViews[i]->marginsX + x, view->subViews[i]->marginsY + y); // 刷新本身
            flashedNum++;
        }
        flashedNum += flashView(view->subViews[i], frame, view->subViews[i]->marginsX, view->subViews[i]->marginsY); // 刷新子项
    }

    return flashedNum;
}

void * threadFlashView(void * argv){
    View * screen = argv;
    // 新建一个帧缓冲区
    Canvas frame;
    frame.width = screen->canvas->width;
    frame.height = screen->canvas->height;
    frame.p = malloc(sizeof(int) * frame.height * frame.width);
    while(1){
        //清空并刷新一帧画面
        clearCanvas(&frame, -1);
        flashView(screen, &frame, 0, 0);
        buflash(screen->canvas, &frame, 0, 0);

        // 帧绘制延迟
        usleep(16000);
    }
}

void setViewById(View * view, int id, char state){
    if(view->id == id){
        printf("hideViewById: View %d has been hided \n", view->id);
        view->state = state;
        return;
    }
    int i;
    for(i=0; i<view->subViewsNum;i++){
        setViewById(view->subViews[i], id, state);
    }
}
/**
 * 事件相关功能
*/

static void touchEventFunction(View * view);

static void updateTouchEvent(View *view, Point *relatP){
    // printf("updateTouchEvent: Update View %d \n", view->id);
    if(view->state == 0) return ;
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

    // printf("updateTouchEvent: View %d have Event. relatP(%d, %d) \n", view->id, view->event.value[0], view->event.value[1]);
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
        usleep(9000);
        if(touchP->x < 0) continue;

        // printf("updateTouchEvent_t: TouchP-(%d, %d) \n", touchP->x, touchP->y);
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

    // 事件更新
    pthread_t evenT;
    printf("ready to pthread_create updateTouchEvent_t \n");
    pthread_create(&evenT, NULL, updateTouchEvent_t, (void *)argv);
}

static void touchEventFunction(View * view){
    switch (view->id)
    {
    case 11:
        selectGrid();
        break;
    case 12:
        openSelectedGrid();
        break;
    case 13:
        flagSelectedGrid();
        break;
    case 14:
        restartSaolei();
        break;
    case 21:
        diffChange2Easy();
        break;
    case 22:
        diffChange2Normal();
        break;
    case 23:
        diffChange2Hard();
        break;
    default:
        break;
    }
}

/**
 * 计时器相关功能
*/

static int timeReset(Timer * timer);

Timer * creaTimer(int id, int width, int height, int marginsX, int marginsY, int color){
    // 创建一个Timer
    Timer * timer = malloc(sizeof(Timer));

    // 初始化Timer
    View * view = creatView(id, width, height, marginsX, marginsY);
    timer->view = view;
    timer->t = 0;
    timer->cmd[0] = 0;
    timer->state = 0;
    timer->color = color;

    return timer;
}

static void * timeRun(void * argv){
    printf("timerStart: Start timer thread \n");
    Timer * timer = argv;
    // 设置起始时间
    timer->startTime = time(NULL);
    printf("timerStart: Start timer while \n");
    // 循环计时
    while(1){
        usleep(100000);
        // 接受命令
        if(timer->cmd[0] == 0){
            // 计算时间
            timer->t = time(NULL) - timer->startTime;
        }
        else if(timer->cmd[0] == 1){
            timeReset(timer);
            timer->cmd[0] = 0;
        }
        else if(timer->cmd[0] == 2){
            continue;
        }
        else if(timer->cmd[0] == 3){
            timeReset(timer);
            // 计算时间
            timer->cmd[0] = 2;
        }
        else{
            timer->cmd[0] = 0;
        }
        // 显示这个事件
            char timeStr[32];
            sprintf(timeStr, "%d", timer->t);
            clearCanvas(timer->view->canvas, -1);
            drawString(timer->view->canvas, 0, 0, timeStr, timer->view->canvas->height, timer->color);
    }
}

static int timeReset(Timer * timer){
    timer->startTime = time(NULL);
    timer->t = 0;
}

void timeStart(Timer * timer){
    //创建线程运行timer
    pthread_create(&timer->thread, NULL, timeRun, (void *)timer);
}