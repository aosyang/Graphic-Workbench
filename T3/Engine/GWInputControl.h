/********************************************************************
	created:	2011/11/03
	filename: 	GWInputControl.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#ifndef GWInputControl_h__
#define GWInputControl_h__

#include "GWCommon.h"
#include "GWInputDeviceEnum.h"

typedef struct GWRenderWindow GW_RENDER_WINDOW;



/************************************************************************/
/* GW Input Device
/************************************************************************/
void GWInput_InitializeDevice(GW_RENDER_WINDOW* rw);

void GWInput_UpdateInputState();

void GWInput_DestroyDevice();

GW_BUTTON_STATE GWInput_GetKeyState(int key);
GW_BUTTON_STATE GWInput_GetMouseBtnState(int btn);
int GWInput_GetMouseWheelValue();

GW_BUTTON_STATE GWInput_GetControllerBtnState(int btn, int controller=0);
float GWInput_GetControllerAxisState(int axis, int controller=0);

GW_BUTTON_STATE GWInput_GetDeviceBtnState(GW_INPUT_DEVICE device, int btn);


/************************************************************************/
/* GW Input Control
/************************************************************************/
typedef void(*GW_KeyFunc)();
typedef void(*GW_AxisFunc)(float val);

struct GWControlMap
{
	GW_KeyFunc			func;
	GW_INPUT_DEVICE		device;
	int					btn_code;
	GW_BUTTON_STATE		state;		
};

struct GWAxisMap
{
	GW_AxisFunc			func;
	GW_INPUT_DEVICE		device;
	int					axis;
};

void GWInputCon_Initialize(GWControlMap* control_map, GWAxisMap* axis_map=NULL);

void GWInputCon_Update();



#endif // GWInputControl_h__