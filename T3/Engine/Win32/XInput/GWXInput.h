/********************************************************************
	created:	2011/11/10
	filename: 	GWXInput.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWXInput_h__
#define GWXInput_h__

#include "GWInputDeviceEnum.h"

void GWXInputResetState();

void GWXInputUpdateState();

GW_BUTTON_STATE GWXInputGetBtnState( int btn );
float GWXInputGetAxisState( int axis );

#endif // GWXInput_h__