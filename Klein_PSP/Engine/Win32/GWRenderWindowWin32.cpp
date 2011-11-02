/********************************************************************
	created:	2011/10/30
	filename: 	GWRenderWindowWin32.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "GWRenderWindowWin32.h"
#include "Renderer/GWRenderWindow.h"
#include <windows.h>
#include <windowsx.h>
#include <memory.h>
#include "GWCommon.h"

static GW_RENDER_WINDOW* RenderWindowList = NULL;
static void AddToRenderWindowList(GW_RENDER_WINDOW* rw)
{
	if (!RenderWindowList)
	{
		RenderWindowList = rw;
		return;
	}

	GW_RENDER_WINDOW* rw_elem = RenderWindowList;
	while (rw_elem->next)
	{
		rw_elem = rw_elem->next;
	}

	rw_elem->next = rw;
}

static void RemoveFromRenderWindowList(GW_RENDER_WINDOW* rw)
{
	if (!rw) return;

	if (RenderWindowList == rw)
	{
		RenderWindowList = rw->next;
		delete rw;
		return;
	}

	GW_RENDER_WINDOW* rw_elem = RenderWindowList;
	while (rw_elem->next)
	{
		if (rw_elem->next == rw)
		{
			rw_elem->next = rw->next;
			delete rw;
			return;
		}
	}
}

WNDPROC CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	GW_RENDER_WINDOW* rw = RenderWindowList;
	while (rw)
	{
		HWND rw_hwnd = *((HWND*)rw->params[WND_PARAM_WIN32_HANDLE]);
		if (rw_hwnd == hWnd)
		{
			break;
		}

		rw = rw->next;
	}

	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_MOUSEMOVE:
		if (rw)
		{
			rw->mouse_x = GET_X_LPARAM(lParam);
			rw->mouse_y = GET_Y_LPARAM(lParam);
		}
		break;
	}

	return (WNDPROC)DefWindowProc(hWnd, uMsg, wParam, lParam);
}

GW_RENDER_WINDOW* GWWnd_CreateRenderWindow( int width, int height, const char* title )
{
#if defined UNICODE
	// Title ansi to wchar
	wchar_t* title_str = NULL;

	if (title)
	{
		size_t len = strlen(title);
		size_t n = (size_t)MultiByteToWideChar(CP_ACP, 0, title, (int)len, NULL, 0);

		title_str = new wchar_t[n + 1];
		MultiByteToWideChar(CP_ACP, 0, title, (int)len, title_str, (int)n);

		title_str[n] = 0;
	}
#else
	char* title_str = title;
#endif

	// Create render window
	WNDCLASS wc;

	DWORD	dwExStyle;
	DWORD	dwStyle;

	RECT rect;
	rect.left = (long)0;
	rect.right = (long)width;
	rect.top = (long)0;
	rect.bottom = (long)height;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC) WndProc;	
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title_str;

	if (!RegisterClass(&wc)) return NULL;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;

	AdjustWindowRectEx (&rect, dwStyle, false, dwExStyle);

	HWND hWnd = CreateWindowEx(	dwExStyle,
								title_str,
								title_str,
								dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								(GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2,
								(GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2,
								rect.right - rect.left,
								rect.bottom - rect.top,
								NULL, NULL, hInstance, NULL);

	GW_RENDER_WINDOW* rw = new GW_RENDER_WINDOW;
	memset(rw, 0, sizeof(GW_RENDER_WINDOW));
	rw->width = width;
	rw->height = height;
	strcpy(rw->title, title);

	HWND* pHWND = (HWND*)malloc(sizeof(HWND));
	*pHWND = hWnd;

	rw->params[WND_PARAM_WIN32_HANDLE] = pHWND;
	AddToRenderWindowList(rw);

	ShowWindow( hWnd, SW_SHOW );
	SetForegroundWindow( hWnd );
	SetFocus( hWnd );

#if defined UNICODE
	GW_SAFE_DELETE(title_str);
#endif

	return rw;
}

void GWWnd_DestroyRenderWindow( GW_RENDER_WINDOW* rw )
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hWnd = *((HWND*)rw->params[WND_PARAM_WIN32_HANDLE]);

#if defined UNICODE
	// Title ansi to wchar
	wchar_t* title_str = NULL;

	if (rw->title)
	{
		size_t len = strlen(rw->title);
		size_t n = (size_t)MultiByteToWideChar(CP_ACP, 0, rw->title, (int)len, NULL, 0);

		title_str = new wchar_t[n + 1];
		MultiByteToWideChar(CP_ACP, 0, rw->title, (int)len, title_str, (int)n);
		title_str[n] = 0;
	}
#else
	char* title_str = rw->title;
#endif

	for (int i=0; i<WND_PARAM_COUNT; i++)
	{
		if (rw->params[i])
			free( rw->params[i] );
	}

	DestroyWindow( hWnd );
	UnregisterClass( title_str, hInstance );

	RemoveFromRenderWindowList(rw);

#if defined UNICODE
	GW_SAFE_DELETE(title_str);
#endif

}

bool HandleWindowMessage()
{
	MSG msg;
	bool bQuit = false;

	if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
	{
		if ( msg.message == WM_QUIT )
		{
			bQuit = true;
		}
		else
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return !bQuit;
}
