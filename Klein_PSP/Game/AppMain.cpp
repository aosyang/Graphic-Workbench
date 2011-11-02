/********************************************************************
	created:	2011/9/5
	filename: 	AppMain.cpp
	author:		Mwolf
	
	purpose:	Application loop for game
*********************************************************************/

#include "Renderer/GWRenderWindow.h"
#include "GameMain.h"

#if defined GW_PLATFORM_WIN32
#include <windows.h>
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
#else
int main()
#endif
{
	KleinGame()->Startup();

	while ( HandleWindowMessage() )
	{
		KleinGame()->Update();
		KleinGame()->Render();
	}

	KleinGame()->Shutdown();

	return 0;
}
