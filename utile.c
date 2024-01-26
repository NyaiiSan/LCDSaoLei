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

int * getEvents(char * evf){
	int fb = open(evf, O_RDONLY);

	struct input_event ev;

	return NULL;
}

int randuint(int min, int max){
	int num = rand();
	num = num > 0 ? num : -num;
	num = min + num % (max-min);

	return num;
}

struct point Get_Touch() 
{
	struct point p; 
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
			printf("222222\n");
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

void * alwaysGet_Touch(void * argv){
	while(1){
		struct point * point = (struct point *)argv;
		struct point getp = Get_Touch();
		point->x = getp.x;
		point->y = getp.y;
	}
}