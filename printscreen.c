#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <X11/xpm.h>
#include <X11/Xlib.h>

#define MAX_FILE_SIZE 0xff
char currenttime[MAX_FILE_SIZE] = {0};

char *getcurrenttimestring(char *inout,int maxsize)
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	snprintf(inout,maxsize,"%02d_%02d_%d_%02d_%02d_%02d.xpm",t->tm_mon+1,t->tm_mday,t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec); // int
	return inout;
}

static void printscreen(int focusedwindow)
{
	int ret;
	Window window;
	XWindowAttributes xwa;
	Display *display = XOpenDisplay(NULL);
	if(focusedwindow)
		XGetInputFocus(display,&window,&ret); // int
	else
		window = DefaultRootWindow(display);
	XGetWindowAttributes(display,window,&xwa);
	XImage *img = XGetImage(display,window,0,0,xwa.width,xwa.height,AllPlanes,XYPixmap);
	XpmWriteFileFromImage(display,getcurrenttimestring(currenttime,MAX_FILE_SIZE),img,NULL,NULL);
}

int main(int argc,char **argv)
{
	int flag = 0;
	if(argc > 1 && strcmp(argv[1],"-w") == 0)
		flag = 1;
	printscreen(flag);
			
}
