/********************************************************************
	created:	2011/11/03
	filename: 	GWRenderDeviceGu.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "Renderer/GWRenderDevice.h"
#include "../Game/GameDef.h"

#include <pspkernel.h>
#include <pspdisplay.h>

#include <pspgu.h>
#include <pspgum.h>

static unsigned int __attribute__((aligned(16))) list[262144];

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

static unsigned int staticOffset = 0;

static unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm)
{
	switch (psm)
	{
	case GU_PSM_T4:
		return (width * height) >> 1;

	case GU_PSM_T8:
		return width * height;

	case GU_PSM_5650:
	case GU_PSM_5551:
	case GU_PSM_4444:
	case GU_PSM_T16:
		return 2 * width * height;

	case GU_PSM_8888:
	case GU_PSM_T32:
		return 4 * width * height;

	default:
		return 0;
	}
}

static void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
{
	unsigned int memSize = getMemorySize(width,height,psm);
	void* result = (void*)staticOffset;
	staticOffset += memSize;

	return result;
}

static void* getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm)
{
	void* result = getStaticVramBuffer(width,height,psm);
	return (void*)(((unsigned int)result) + ((unsigned int)sceGeEdramGetAddr()));
}

void RenderSystem::Initialize( GW_RENDER_WINDOW* rw )
{
	// flush cache so that no stray data remains

	sceKernelDcacheWritebackAll();

	// setup GU

	void* fbp0 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* zbp = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

	pspDebugScreenInit();
	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
	sceGuDepthBuffer(zbp,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

void RenderSystem::Destroy()
{
	sceGuTerm();
}

TEXTURE_INFO* RenderSystem::CreateTexture( const char* filename )
{

}

void RenderSystem::DestroyTexture( TEXTURE_INFO* texture )
{

}

void RenderSystem::SetupCamera( const Vector2& cam_pos, float fovy )
{
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective( fovy, 16.0f/9.0f, 1.0f, 100.0f );

	ScePspFVector3 eye = { cam_pos.x, cam_pos.y, -KLEIN_CAMERA_ZPOS };
	ScePspFVector3 look_at = { cam_pos.x, cam_pos.y, 0.0f };
	ScePspFVector3 up = { 0.0f, 1.0f, 0.0f };

	sceGumMatrixMode(GU_VIEW);
	sceGumLookAt(&eye, &look_at, &up);

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
}

void RenderSystem::Clear()
{
	sceGuClearColor(0xff554433);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

	//pspDebugScreenPrintf("RenderSystem::Clear()\n");
}

void RenderSystem::BeginRender()
{
	sceGuStart(GU_DIRECT,list);
}

void RenderSystem::EndRender()
{
	sceGuFinish();
	sceGuSync(0,0);
}

void RenderSystem::Flush()
{
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

void RenderSystem::DrawSprite( const Vector2& vMin, const Vector2& vMax, int tex_id /*= -1*/, float depth /*= 0.0f*/ )
{

}

struct GuColoredVertex
{
	GW_UINT32 color;
	float x, y, z;
};

void RenderSystem::DrawColoredSprite( const Vector2& vMin, const Vector2& vMax, const GWColor& color /*= GWColor::WHITE*/, float depth /*= 0.0f*/ )
{
	struct GuColoredVertex __attribute__((aligned(16))) vertices[] =
	{
		{color.ARGB(), vMin.x, vMin.y, depth},
		{color.ARGB(), vMin.x, vMax.y, depth},
		{color.ARGB(), vMax.x, vMax.y, depth},

		{color.ARGB(), vMax.x, vMax.y, depth},
		{color.ARGB(), vMax.x, vMin.y, depth},
		{color.ARGB(), vMin.x, vMin.y, depth},

		{color.ARGB(), vMin.x, vMin.y, depth},
		{color.ARGB(), vMax.x, vMax.y, depth},
		{color.ARGB(), vMin.x, vMax.y, depth},

		{color.ARGB(), vMax.x, vMax.y, depth},
		{color.ARGB(), vMin.x, vMin.y, depth},
		{color.ARGB(), vMax.x, vMin.y, depth},
	};

	sceGumDrawArray(GU_TRIANGLES, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4*3, 0, vertices);
}

void RenderSystem::DrawWireframeRect( const Vector2& vMin, const Vector2& vMax, const GWColor& color /*= GWColor::WHITE*/, float depth /*= 0.0f*/ )
{

}

void RenderSystem::RenderText( const char* text, int x, int y, const GWColor& color /*= GWColor::WHITE */ )
{

}
