#include "GWTimer.h"

#include <sys/time.h>

struct timeval start;

GW_UINT32 GWSys_GetTickCount()
{
	struct timeval now;

	static GW_BOOL initTimer = GW_FALSE;

	// One-time initialization
	if (initTimer == GW_FALSE)
	{
		gettimeofday(&start, NULL);
		initTimer = GW_TRUE;
	}

	gettimeofday(&now, NULL);
	return (now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;
}