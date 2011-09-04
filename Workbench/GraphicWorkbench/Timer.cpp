#include "Timer.h"
#include <windows.h>

namespace Timer
{
	static uint32 g_SystemElapsedTime = 0;

	uint32 GetElapsedTime()
	{
		return g_SystemElapsedTime;
	}


	void UpdateElapsedTime()
	{
		g_SystemElapsedTime = GetTickCount();
	}
}
