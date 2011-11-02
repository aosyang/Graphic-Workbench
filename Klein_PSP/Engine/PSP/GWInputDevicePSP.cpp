/********************************************************************
	created:	2011/11/03
	filename: 	GWInputDevicePSP.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "GWInputControl.h"

#include <pspctrl.h>

static SceCtrlData pad;

void GWInput_InitializeDevice( GW_RENDER_WINDOW* rw )
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

void GWInput_UpdateInputState()
{
	sceCtrlReadBufferPositive(&pad, 1);
}

void GWInput_DestroyDevice()
{

}

GWButtonState GWInput_GetKeyState(int key)
{
	return GW_KEY_STATE_INVALID;
}

GWButtonState GWInput_GetMouseBtnState(int btn)
{
	return GW_KEY_STATE_INVALID;
}

int GWInput_GetMouseWheelValue()
{
	return 0;
}
