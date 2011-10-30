/********************************************************************
	created:	2011/9/5
	filename: 	RenderDevice.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef RenderDevice_h__
#define RenderDevice_h__

#include "Math/GWVectors.h"
#include "GWRenderWindow.h"

#include <Windows.h>

typedef struct GWRenderWindow GW_RENDER_WINDOW;
struct IDirect3DDevice9;

class RenderSystem
{
public:
	static void Initialize(GW_RENDER_WINDOW* rw);
	static void Destroy();
	static IDirect3DDevice9* Device();

	static void SetupCamera(const Vector2& cam_pos, float fovy);
	static void Clear();
	static void BeginRender();
	static void EndRender();
	static void Flush();

	static void DrawSprite(const Vector2& vMin, const Vector2& vMax, int tex_id = -1, float depth = 0.0f);
	static void DrawColoredSprite(const Vector2& vMin, const Vector2& vMax, DWORD color = 0xFFFFFFFF, float depth = 0.0f);
	static void DrawWireframeRect(const Vector2& vMin, const Vector2& vMax, DWORD color = 0xFFFFFFFF, float depth = 0.0f);

	static void DrawText( const char* text, int x, int y, DWORD color = 0xFFFFFFFF );

private:
};

#endif // RenderDevice_h__