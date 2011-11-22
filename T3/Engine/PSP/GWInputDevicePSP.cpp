/********************************************************************
	created:	2011/11/03
	filename: 	GWInputDevicePSP.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "GWInputControl.h"
#include "GWTypes.h"

#include <pspctrl.h>
#include <string.h>

#define GW_PSP_ANALOG_DEAD_ZONE	20

static SceCtrlData pad;

bool				psp_btn_down[GW_PSPBTN_COUNT];
GW_BUTTON_STATE		psp_btn_state[GW_PSPBTN_COUNT];

float				psp_axis[GW_PSPAXIS_COUNT];

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

	for (int i=0; i<GW_PSPAXIS_COUNT; i++)
	{
		psp_axis[i] = 0.f;
	}
}

static bool GWInput_IsPSPAnalogInsideDeadZone(GW_UINT8 val)
{
	return (val >= 128 - GW_PSP_ANALOG_DEAD_ZONE) && (val <= 128 + GW_PSP_ANALOG_DEAD_ZONE);
}

void GWInput_UpdateInputState()
{
	sceCtrlReadBufferPositive(&pad, 1);

	float x_val = (float)pad.Lx / 255.f;
	float y_val = (float)pad.Ly / 255.f;

	psp_axis[GW_PSPAXIS_LX] = GWInput_IsPSPAnalogInsideDeadZone(pad.Lx) ? 0.f : x_val * 2.f - 1.f;
	psp_axis[GW_PSPAXIS_LY] = GWInput_IsPSPAnalogInsideDeadZone(pad.Ly) ? 0.f : 1.f - y_val * 2.f;

	SetPSPBtnState(GW_PSPBTN_SQUARE,	(pad.Buttons & PSP_CTRL_SQUARE) != 0);
	SetPSPBtnState(GW_PSPBTN_TRIANGLE,	(pad.Buttons & PSP_CTRL_TRIANGLE) != 0);
	SetPSPBtnState(GW_PSPBTN_CIRCLE,	(pad.Buttons & PSP_CTRL_CIRCLE) != 0);
	SetPSPBtnState(GW_PSPBTN_CROSS,		(pad.Buttons & PSP_CTRL_CROSS) != 0);

	SetPSPBtnState(GW_PSPBTN_UP,		(pad.Buttons & PSP_CTRL_UP) != 0);
	SetPSPBtnState(GW_PSPBTN_DOWN,		(pad.Buttons & PSP_CTRL_DOWN) != 0);
	SetPSPBtnState(GW_PSPBTN_LEFT,		(pad.Buttons & PSP_CTRL_LEFT) != 0);
	SetPSPBtnState(GW_PSPBTN_RIGHT,		(pad.Buttons & PSP_CTRL_RIGHT) != 0);

	SetPSPBtnState(GW_PSPBTN_L,			(pad.Buttons & PSP_CTRL_LTRIGGER) != 0);
	SetPSPBtnState(GW_PSPBTN_R,			(pad.Buttons & PSP_CTRL_RTRIGGER) != 0);

	SetPSPBtnState(GW_PSPBTN_START,		(pad.Buttons & PSP_CTRL_START) != 0);
	SetPSPBtnState(GW_PSPBTN_SELECT,	(pad.Buttons & PSP_CTRL_SELECT) != 0);
}

void GWInput_DestroyDevice()
{

}

GW_BUTTON_STATE GWInput_GetKeyState(int key)
{
	return GW_KEY_STATE_INVALID;
}

GW_BUTTON_STATE GWInput_GetMouseBtnState(int btn)
{
	return GW_KEY_STATE_INVALID;
}

int GWInput_GetMouseWheelValue()
{
	return 0;
}

GW_BUTTON_STATE GWInput_GetControllerBtnState( int btn, int controller/*=0*/ )
{
	if (controller == 0 && btn >= 0 && btn <GW_PSPBTN_COUNT)
	{
		return psp_btn_state[btn];
	}

	return GW_KEY_STATE_INVALID;
}

float GWInput_GetControllerAxisState(int axis, int controller)
{
	if (axis >= 0 && axis < GW_PSPAXIS_COUNT)
		return psp_axis[axis];

	return 0.f;
}
