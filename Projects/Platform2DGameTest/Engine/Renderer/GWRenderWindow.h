/********************************************************************
	created:	2011/10/30
	filename: 	GWRenderWindow.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWRenderWindow_h__
#define GWRenderWindow_h__

#include "Win32/GWRenderWindowWin32.h"

typedef struct GWRenderWindow
{
	int		width, height;
	char	title[256];
	void*	params[WND_PARAM_COUNT];

	int		mouse_x, mouse_y;

	GW_RENDER_WINDOW* next;
} GW_RENDER_WINDOW;

#endif // GWRenderWindow_h__