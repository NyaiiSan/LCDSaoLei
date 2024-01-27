#include "main.h"


struct touchPoint getTouchPoint() 
{
	struct touchPoint p; 
	p.x = p.y = -1;
	//打开触摸屏文件 
	int fd_touch = open("/dev/input/event1",O_RDWR);
	if(fd_touch == -1)
	{
		perror("open error");
		return p;
	}
	//不停地从触摸屏文件中获取结构体地数据 
	struct input_event ev;
	int ret; 
	while(1)
	{
		ret = read(fd_touch, &ev, sizeof(ev));
		if( ret != sizeof(ev) ) //没有读到完整地数据就认为本次数据无效 
		{
			printf("getTouchPoint: No Data\n");
			continue; 
		}
		//分析结构体中地数据获取信息
		if(ev.type == EV_ABS && ev.code == ABS_MT_POSITION_X)
		{
			//此时value就是x轴坐标值
			if(p.x < 0) 
			{
				p.x = ev.value;
			}
		}
		else if(ev.type == EV_ABS  && ev.code == ABS_MT_POSITION_Y)
		{
			//此时value就是y轴坐标值
			if(p.y < 0)
			{
				p.y = ev.value;
			}
		}
		//if(ev.type == EV_ABS && ev.code == ABS_MT_TRACKING_ID && ev.value == -1 )
		if(p.x > 0 && p.y > 0)
		{
			close(fd_touch);
			return p;
		}	
	}
}

static void updataTouchPoint(struct touchPoint * tPoint){
    struct touchPoint nowPoint = getTouchPoint();
    tPoint->x = nowPoint.x;
    tPoint->y = nowPoint.y;
    printf("Now TouchPoint (%d, %d) \n", tPoint->x, tPoint->y);
}

static void * updataTouchPointForevery(void * argv){
    struct touchPoint * tpoint = (struct touchPoint *)argv;
    while(1){
        updataTouchPoint(tpoint);
        usleep(5000);
    }
}

struct touchPoint * initTouch(){

    struct touchPoint * tpoint = malloc(sizeof(struct touchPoint));
    tpoint->x = -1;
    tpoint->y = -1;

    pthread_t t;
    pthread_create(&t, NULL, updataTouchPointForevery, (void*)tpoint);

    return tpoint;
}

