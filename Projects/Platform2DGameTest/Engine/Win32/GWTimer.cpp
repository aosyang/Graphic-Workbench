/********************************************************************
	created:	2011/10/31
	filename: 	GWTimer.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "GWTimer.h"
#include <windows.h>

GW_UINT32 GWSys_GetTickCount()
{
	return GetTickCount();
}
