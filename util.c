/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util.h"

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = calloc(nmemb, size)))
		die("calloc:");
	return p;
}

void
die(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	exit(1);
}

static const char *Days[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
static const char *Months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
enum {Second,Minute,Hour,Day,Week,Month,Year};

char *gettimestringpretty(char *s,int maxsize)
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	snprintf(s,maxsize,"%s %s %d %02d:%02d",Days[t->tm_wday],Months[t->tm_mon],t->tm_mday,t->tm_hour,t->tm_min);
	//snprintf(s,256,"%d:%02d:%02d",t->tm_hour,t->tm_min); // int			
	return s;
}

/* used for file names */
char *getcurrenttimestring(char *inout,int maxsize)
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	snprintf(inout,maxsize,"%d_%d_%d_%d_%d_%d.xpm",t->tm_mon+1,t->tm_mday,t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec); // int
	return inout;
}
