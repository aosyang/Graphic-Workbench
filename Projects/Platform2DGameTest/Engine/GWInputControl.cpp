/********************************************************************
	created:	2011/11/03
	filename: 	GWInputControl.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "GWInputControl.h"


/************************************************************************/
/* GW Input Device
/************************************************************************/
GW_BUTTON_STATE GWInput_GetDeviceBtnState(GW_INPUT_DEVICE device, int btn)
{
	switch (device)
	{
	case GW_INPUT_DEVICE_KEYBOARD:
		return GWInput_GetKeyState(btn);
	case GW_INPUT_DEVICE_MOUSE:
		return GWInput_GetMouseBtnState(btn);
	case GW_INPUT_DEVICE_CONTROLLER0:
		return GWInput_GetControllerBtnState(btn, device - GW_INPUT_DEVICE_CONTROLLER0);
	default:
		break;
	}

	return GW_KEY_STATE_INVALID;
}


/************************************************************************/
/* GW Input Control
/************************************************************************/
static GWControlMap* control_key_map = 0;
static GWAxisMap* control_axis_map = 0;

void GWInputCon_Initialize( GWControlMap* control_map, GWAxisMap* axis_map )
{
	control_key_map = control_map;
	control_axis_map = axis_map;
}

static bool GWInputCon_KeyStatesEqual(GW_BUTTON_STATE check_state, GW_BUTTON_STATE key_state)
{
	if (check_state == key_state)
		return true;

	if (key_state == GW_KEY_STATE_ON_PRESSED &&
		check_state == GW_KEY_STATE_DOWN)
		return true;

	if (key_state == GW_KEY_STATE_ON_RELEASED &&
		check_state == GW_KEY_STATE_UP)
		return true;

	return false;
}

void GWInputCon_Update()
{
	GWControlMap* key_map = control_key_map;
	if (!key_map) return;

	while (key_map->func)
	{
		GW_BUTTON_STATE key_state = GWInput_GetDeviceBtnState(key_map->device, key_map->btn_code);
		if ( GWInputCon_KeyStatesEqual( key_map->state, key_state ) )
			(*key_map->func)();

		key_map++;
	}


	GWAxisMap* axis_map = control_axis_map;
	if (!axis_map) return;

	while (axis_map->func)
	{
		float axis_val = GWInput_GetControllerAxisState(axis_map->axis);
		(*axis_map->func)(axis_val);

		axis_map++;
	}
}
