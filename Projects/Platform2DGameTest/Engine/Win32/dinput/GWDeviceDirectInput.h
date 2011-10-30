/********************************************************************
	created:	2011/10/30
	filename: 	GWDeviceDirectInput.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWDeviceDirectInput_h__
#define GWDeviceDirectInput_h__

#include "GWInputDeviceEnum.h"

typedef struct GWRenderWindow GW_RENDER_WINDOW;

void GWInput_InitializeDevice(GW_RENDER_WINDOW* rw);

void GWInput_UpdateInputState();

void GWInput_DestroyDevice();

bool GWInput_GetKeyDownState(GWKeyCode key);
bool GWInput_GetMouseBtnDownState(GWMouseButton btn);
int GWInput_GetMouseWheelValue();

#endif // GWDeviceDirectInput_h__