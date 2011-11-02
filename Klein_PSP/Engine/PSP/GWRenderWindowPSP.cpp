#include "Renderer/GWRenderWindow.h"
#include "GWRenderWindowPSP.h"
#include "GWCommon.h"

#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>

PSP_MODULE_INFO("Klein PSP", 0, 1, 1);
PSP_HEAP_SIZE_MAX();

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

static int exitRequest = 0;

int exitCallback(int arg1, int arg2, void *common)
{
	exitRequest = 1;
	return 0;
}

int callbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

int setupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

GW_RENDER_WINDOW* GWWnd_CreateRenderWindow()
{
	setupCallbacks();

	GW_RENDER_WINDOW* rw = new GW_RENDER_WINDOW;
	memset(rw, 0, sizeof(GW_RENDER_WINDOW));
	rw->width = SCR_WIDTH;
	rw->height = SCR_HEIGHT;

	return rw;
}

void GWWnd_DestroyRenderWindow(GW_RENDER_WINDOW* rw)
{
	GW_SAFE_DELETE(rw)
	sceKernelExitGame();
}

bool HandleWindowMessage()
{
	return !exitRequest;
}
