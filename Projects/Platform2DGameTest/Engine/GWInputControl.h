/********************************************************************
	created:	2011/11/03
	filename: 	GWInputControl.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#ifndef GWInputControl_h__
#define GWInputControl_h__

#include "GWInputDeviceEnum.h"

typedef struct GWRenderWindow GW_RENDER_WINDOW;



/************************************************************************/
/* GW Input Device
/************************************************************************/
void GWInput_InitializeDevice(GW_RENDER_WINDOW* rw);

void GWInput_UpdateInputState();

void GWInput_DestroyDevice();

GWButtonState GWInput_GetKeyState(int key);
GWButtonState GWInput_GetMouseBtnState(int btn);
int GWInput_GetMouseWheelValue();

GWButtonState GWInput_GetControllerState(int btn, int controller=0);

GWButtonState GWInput_GetBtnState(GWInputDeviceType device, int btn);


/************************************************************************/
/* GW Input Control
/************************************************************************/
typedef void(*GW_KeyFunc)();

struct GW_KeyMap
{
	GW_KeyFunc			func;
	GWInputDeviceType	device;
	int					btn_code;
	GWButtonState		state;		
};

void GWInputCon_Initialize(GW_KeyMap* key_map);

void GWInputCon_Update();



#endif // GWInputControl_h__