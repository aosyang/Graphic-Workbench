/********************************************************************
	created:	2011/11/10
	filename: 	GWXInput.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "GWXInput.h"

#include <Windows.h>
#include <XInput.h>

XINPUT_STATE	xinput_state;

bool			xi_btn_down[GW_XBTN_COUNT];
GW_BUTTON_STATE	xi_btn_state[GW_XBTN_COUNT];

float			xi_axis[GW_XAXIS_COUNT];

void GWXInputResetState()
{
	memset(xi_btn_down, 0, sizeof(xi_btn_down));
	memset(xi_btn_state, 0, sizeof(xi_btn_state));

	for (int i=0; i<GW_XAXIS_COUNT; i++)
		xi_axis[i] = 0.f;

	XINPUT_VIBRATION vibration;

	ZeroMemory( &vibration, sizeof(XINPUT_VIBRATION) );
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState( 0, &vibration );

}

static void SetXBtnState(int btn, int xinput_btn)
{
	bool down = ((xinput_state.Gamepad.wButtons & xinput_btn) != 0);

	if (xi_btn_down[btn] != down)
	{
		xi_btn_state[btn] = down ? GW_KEY_STATE_ON_PRESSED : GW_KEY_STATE_ON_RELEASED;
	}
	else
	{
		xi_btn_state[btn] = down ? GW_KEY_STATE_DOWN : GW_KEY_STATE_UP;
	}
	xi_btn_down[btn] = down;
}

void GWXInputUpdateState()
{
	ZeroMemory( &xinput_state, sizeof(XINPUT_STATE) );

	for (int i=0; i<GW_XAXIS_COUNT; i++)
		xi_axis[i] = 0.f;

	DWORD dwResult = XInputGetState( 0, &xinput_state );

	if ( dwResult == ERROR_SUCCESS )
	{
		// XInput device connected
		SetXBtnState(GW_XBTN_X,				XINPUT_GAMEPAD_X);
		SetXBtnState(GW_XBTN_Y,				XINPUT_GAMEPAD_Y);
		SetXBtnState(GW_XBTN_A,				XINPUT_GAMEPAD_A);
		SetXBtnState(GW_XBTN_B,				XINPUT_GAMEPAD_B);

		SetXBtnState(GW_XBTN_DPAD_UP,		XINPUT_GAMEPAD_DPAD_UP);
		SetXBtnState(GW_XBTN_DPAD_DOWN,		XINPUT_GAMEPAD_DPAD_DOWN);
		SetXBtnState(GW_XBTN_DPAD_LEFT,		XINPUT_GAMEPAD_DPAD_LEFT);
		SetXBtnState(GW_XBTN_DPAD_RIGHT,	XINPUT_GAMEPAD_DPAD_RIGHT);

		SetXBtnState(GW_XBTN_LB,			XINPUT_GAMEPAD_LEFT_SHOULDER);
		SetXBtnState(GW_XBTN_RB,			XINPUT_GAMEPAD_RIGHT_SHOULDER);

		SetXBtnState(GW_XBTN_LTHUMB,		XINPUT_GAMEPAD_LEFT_THUMB);
		SetXBtnState(GW_XBTN_RTHUMB,		XINPUT_GAMEPAD_RIGHT_THUMB);

		SetXBtnState(GW_XBTN_START,			XINPUT_GAMEPAD_START);
		SetXBtnState(GW_XBTN_BACK,			XINPUT_GAMEPAD_BACK);

		if ( abs(xinput_state.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			xi_axis[GW_XAXIS_LX] = (float)xinput_state.Gamepad.sThumbLX / 32767;

		if ( abs(xinput_state.Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
			xi_axis[GW_XAXIS_LY] = (float)xinput_state.Gamepad.sThumbLY / 32767;

		if ( abs(xinput_state.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
			xi_axis[GW_XAXIS_RX] = (float)xinput_state.Gamepad.sThumbRX / 32767;

		if ( abs(xinput_state.Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
			xi_axis[GW_XAXIS_RY] = (float)xinput_state.Gamepad.sThumbRY / 32767;

		if ( xinput_state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
			xi_axis[GW_XAXIS_LT] = (float)xinput_state.Gamepad.bLeftTrigger / 255;

		if ( xinput_state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD )
			xi_axis[GW_XAXIS_RT] = (float)xinput_state.Gamepad.bRightTrigger / 255;
	}
	else
	{
		// XInput device disconnected
		GWXInputResetState();
	}
}

GW_BUTTON_STATE GWXInputGetBtnState( int btn )
{
	if (btn >= 0 && btn < GW_XBTN_COUNT)
		return xi_btn_state[btn];

	return GW_KEY_STATE_INVALID;
}

float GWXInputGetAxisState( int axis )
{
	if (axis >= 0 && axis < GW_XAXIS_COUNT)
		return xi_axis[axis];

	return 0.f;
}
