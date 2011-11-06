/********************************************************************
	created:	2011/11/03
	filename: 	GWInputDevicePSP.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "GWInputControl.h"

#include <pspctrl.h>
#include <string.h>

static SceCtrlData pad;

bool psp_btn_down[GW_PSPBTN_COUNT];
GWButtonState psp_btn_state[GW_PSPBTN_COUNT];

static void SetPSPBtnState(int btn, bool down)
{
	if (psp_btn_down[btn] != down)
	{
		psp_btn_state[btn] = down ? GW_KEY_STATE_ON_PRESSED : GW_KEY_STATE_ON_RELEASED;
	}
	else
	{
		psp_btn_state[btn] = down ? GW_KEY_STATE_DOWN : GW_KEY_STATE_UP;
	}
	psp_btn_down[btn] = down;
}

void GWInput_InitializeDevice( GW_RENDER_WINDOW* rw )
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	memset(psp_btn_down, 0, sizeof(psp_btn_down));
	memset(psp_btn_state, 0, sizeof(psp_btn_state));
}

void GWInput_UpdateInputState()
{
	sceCtrlReadBufferPositive(&pad, 1);

	SetPSPBtnState(GW_PSPBTN_SQUARE,	(pad.Buttons & PSP_CTRL_SQUARE) != 0);
	SetPSPBtnState(GW_PSPBTN_TRIANGLE,	(pad.Buttons & PSP_CTRL_TRIANGLE) != 0);
	SetPSPBtnState(GW_PSPBTN_CIRCLE,	(pad.Buttons & PSP_CTRL_CIRCLE) != 0);
	SetPSPBtnState(GW_PSPBTN_CROSS,		(pad.Buttons & PSP_CTRL_CROSS) != 0);

	SetPSPBtnState(GW_PSPBTN_UP,		(pad.Buttons & PSP_CTRL_UP) != 0);
	SetPSPBtnState(GW_PSPBTN_DOWN,		(pad.Buttons & PSP_CTRL_DOWN) != 0);
	SetPSPBtnState(GW_PSPBTN_LEFT,		(pad.Buttons & PSP_CTRL_LEFT) != 0);
	SetPSPBtnState(GW_PSPBTN_RIGHT,		(pad.Buttons & PSP_CTRL_RIGHT) != 0);

	SetPSPBtnState(GW_PSPBTN_START,		(pad.Buttons & PSP_CTRL_START) != 0);
	SetPSPBtnState(GW_PSPBTN_SELECT,	(pad.Buttons & PSP_CTRL_SELECT) != 0);
	SetPSPBtnState(GW_PSPBTN_L,			(pad.Buttons & PSP_CTRL_LTRIGGER) != 0);
	SetPSPBtnState(GW_PSPBTN_R,			(pad.Buttons & PSP_CTRL_RTRIGGER) != 0);
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

GWButtonState GWInput_GetControllerState( int btn, int controller/*=0*/ )
{
	if (controller == 0 && btn >= 0 && btn <GW_PSPBTN_COUNT)
	{
		return psp_btn_state[btn];
	}

	return GW_KEY_STATE_INVALID;
}
