#include "GWTimer.h"

#include <sys/time.h>

struct timeval start;

GW_UINT32 GWSys_GetTickCount()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;
}