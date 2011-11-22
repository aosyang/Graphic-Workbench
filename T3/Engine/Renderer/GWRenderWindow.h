/********************************************************************
	created:	2011/10/30
	filename: 	GWRenderWindow.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWRenderWindow_h__
#define GWRenderWindow_h__

#if defined GW_PLATFORM_WIN32
#include "Win32/GWRenderWindowWin32.h"
#elif defined GW_PLATFORM_PSP
#include "PSP/GWRenderWindowPSP.h"
#endif

typedef struct GWRenderWindow GW_RENDER_WINDOW;

typedef struct GWRenderWindow
{
	int		width, height;
	void*	params[WND_PARAM_COUNT];

#if !defined GW_PLATFORM_PSP
	char	title[256];
	int		mouse_x, mouse_y;
#endif

	GW_RENDER_WINDOW* next;
} GW_RENDER_WINDOW;

#if defined GW_PLATFORM_PSP
GW_RENDER_WINDOW* GWWnd_CreateRenderWindow();
#else
GW_RENDER_WINDOW* GWWnd_CreateRenderWindow(int width, int height, const char* title);
#endif

void GWWnd_DestroyRenderWindow(GW_RENDER_WINDOW* rw);

bool HandleWindowMessage();

#endif // GWRenderWindow_h__