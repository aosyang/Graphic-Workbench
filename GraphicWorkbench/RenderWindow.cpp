#include "RenderWindow.h"

WNDPROC CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

RenderWindow::RenderWindow()
{

}

RenderWindow::~RenderWindow()
{

}

bool RenderWindow::Create( const GWChar* titleName, uint32 width, uint32 height, uint32 depthBits, bool fullscreen )
{
	m_TitleName = titleName;

	m_RenderWindowParam.width = width;
	m_RenderWindowParam.height = height;

	m_RenderWindowParam.colorDepthBit = depthBits;

	m_FullScreen = fullscreen;

#if defined __PLATFORM_WIN32
	WNDCLASS wc;

	DWORD	dwExStyle;
	DWORD	dwStyle;

	RECT WindowRect;
	WindowRect.left = (long)0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)height;

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
	wc.lpszClassName = m_TitleName.data();

	if (!RegisterClass(&wc)) return false;
	//AssertFatal(RegisterClass(&wc), "System::CreateRenderWindow() : Failed to register window class.");

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset (&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = depthBits;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH |DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) return false;
		//AssertFatal(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL,
		//	"System::CreateRenderWindow() : Unable to use selected display mode.");
	}

	if (fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(false);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx (&WindowRect, dwStyle, false, dwExStyle);

	m_RenderWindowParam.handle =
		CreateWindowEx(	dwExStyle,
						m_TitleName.data(),
						m_TitleName.data(),
						dwStyle |
						WS_CLIPSIBLINGS |
						WS_CLIPCHILDREN,
						(GetSystemMetrics(SM_CXSCREEN) - WindowRect.right) / 2,
						(GetSystemMetrics(SM_CYSCREEN) - WindowRect.bottom) / 2,
						WindowRect.right - WindowRect.left,
						WindowRect.bottom - WindowRect.top,
						NULL,
						NULL,
						hInstance,
						NULL);

	if (!m_RenderWindowParam.handle) return false;
	//AssertFatal(m_RenderWindowParam.handle, "System::CreateRenderWindow(): Error creating render window.");

	ShowWindow(m_RenderWindowParam.handle,SW_SHOW);						// Show The Window
	SetForegroundWindow(m_RenderWindowParam.handle);						// Slightly Higher Priority
	SetFocus(m_RenderWindowParam.handle);									// Sets Keyboard Focus To The Window
#endif	// #if defined __PLATFORM_WIN32

	return true;
}

void RenderWindow::Destroy()
{
#if defined __PLATFORM_WIN32
	if (m_RenderWindowParam.handle)
	{
		if (!DestroyWindow(m_RenderWindowParam.handle)) return;
		//AssertFatal(DestroyWindow(m_RenderWindowParam.handle), "System::DestroyRenderWindow() : Could Not Release hWnd.");
		m_RenderWindowParam.handle = NULL;
	}

	if (m_FullScreen)											// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);						// If So Switch Back To The Desktop
		ShowCursor(TRUE);									// Show Mouse Pointer
	}

	if (!UnregisterClass(m_TitleName.data(), GetModuleHandle(NULL))) return;
	//AssertFatal(UnregisterClass(m_TitleName.Data(), GetModuleHandle(NULL)), "System::DestroyRenderWindow(): Could not unregister class.");

#endif	// #if defined __PLATFORM_WIN32
}

bool RenderWindow::HandleWindowMessage()
{
	bool continueLoop = true;
#ifdef __PLATFORM_WIN32
	MSG msg;

	if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			continueLoop = false;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
#endif	// #if defined __PLATFORM_LINUX

	return continueLoop;
}

RenderWindowParam* RenderWindow::GetWindowHandle()
{
	return &m_RenderWindowParam;
}

WNDPROC CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	return (WNDPROC)DefWindowProc(hWnd, uMsg, wParam, lParam);
}
