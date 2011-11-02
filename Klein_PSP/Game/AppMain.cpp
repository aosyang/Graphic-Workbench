/********************************************************************
	created:	2011/9/5
	filename: 	AppMain.cpp
	author:		Mwolf
	
	purpose:	Application loop for game
*********************************************************************/
#include <windows.h>

#include "Renderer/GWRenderWindow.h"
#include "GameMain.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
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
