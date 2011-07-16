#include "../GraphicWorkbench/Workbench.h"

void Update(uint32 deltaTime);

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	RenderWindow* rw = new RenderWindow;
	rw->Create(L"Hello World", 640, 480, 32, false);

	Timer::UpdateElapsedTime();
	uint32 elapsedTime = Timer::GetElapsedTime();

	while (rw->HandleWindowMessage())
	{
		uint32 deltaTime = Timer::GetElapsedTime() - elapsedTime;
		elapsedTime = Timer::GetElapsedTime();

		Update(deltaTime);

		Timer::UpdateElapsedTime();
	}

	rw->Destroy();
	delete rw;

	return 0;
}

void Update( uint32 deltaTime )
{

}
