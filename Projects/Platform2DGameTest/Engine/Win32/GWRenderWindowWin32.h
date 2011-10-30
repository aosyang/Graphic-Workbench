/********************************************************************
	created:	2011/10/30
	filename: 	GWRenderWindowWin32.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWRenderWindowWin32_h__
#define GWRenderWindowWin32_h__

typedef struct GWRenderWindow GW_RENDER_WINDOW;

enum WndSpecialParam
{
	WND_PARAM_WIN32_HANDLE,
	WND_PARAM_COUNT,
};

GW_RENDER_WINDOW* GWWnd_CreateRenderWindow(int width, int height, const char* title);

void GWWnd_DestroyRenderWindow(GW_RENDER_WINDOW* rw);

bool HandleWindowMessage();

#endif // GWRenderWindowWin32_h__