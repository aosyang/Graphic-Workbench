/********************************************************************
	created:	2011/11/03
	filename: 	GWInputDevicePSP.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWInputDevicePSP_h__
#define GWInputDevicePSP_h__

#include "GWInputDeviceEnum.h"

typedef struct GWRenderWindow GW_RENDER_WINDOW;

void GWInput_InitializeDevice(GW_RENDER_WINDOW* rw);

void GWInput_UpdateInputState();

void GWInput_DestroyDevice();

#endif // GWInputDevicePSP_h__