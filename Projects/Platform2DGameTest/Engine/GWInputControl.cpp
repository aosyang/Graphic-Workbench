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
GWButtonState GWInput_GetBtnState(GWInputDeviceType device, int btn)
{
	switch (device)
	{
	case GW_INPUT_DEVICE_KEYBOARD:
		return GWInput_GetKeyState(btn);
	case GW_INPUT_DEVICE_MOUSE:
		return GWInput_GetMouseBtnState(btn);
	case GW_INPUT_DEVICE_CONTROLLER0:
		return GWInput_GetControllerState(btn, device - GW_INPUT_DEVICE_CONTROLLER0);
	default:
		break;
	}

	return GW_KEY_STATE_INVALID;
}


/************************************************************************/
/* GW Input Control
/************************************************************************/
static GW_KeyMap* control_key_map = 0;

void GWInputCon_Initialize( GW_KeyMap* key_map )
{
	control_key_map = key_map;
}

void GWInputCon_Update()
{
	GW_KeyMap* key_map = control_key_map;

	while (key_map->func)
	{
		if (GWInput_GetBtnState(key_map->device, key_map->btn_code) == key_map->state)
			(*key_map->func)();

		key_map++;
	}
}
