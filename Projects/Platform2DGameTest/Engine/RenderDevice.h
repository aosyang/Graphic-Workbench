#ifndef RenderDevice_h__
#define RenderDevice_h__

#include "Math/GWVectors.h"

#include <Windows.h>

struct IDirect3DDevice9;
struct ID3DXFont;

class RenderSystem
{
public:
	static void Initialize(IDirect3DDevice9* device);
	static void Destroy();
	static IDirect3DDevice9* Device() { return m_sDevice; }

	static void DrawSprite(const Vector2& vMin, const Vector2& vMax, int tex_id = -1, float depth = 0.0f);
	static void DrawColoredSprite(const Vector2& vMin, const Vector2& vMax, DWORD color = 0xFFFFFFFF, float depth = 0.0f);
	static void DrawWireframeRect(const Vector2& vMin, const Vector2& vMax, DWORD color = 0xFFFFFFFF, float depth = 0.0f);

	static void DrawText( const char* text, int x, int y, DWORD color = 0xFFFFFFFF );

private:
	static IDirect3DDevice9*	m_sDevice;
	static ID3DXFont*			m_sFont;
};

#endif // RenderDevice_h__