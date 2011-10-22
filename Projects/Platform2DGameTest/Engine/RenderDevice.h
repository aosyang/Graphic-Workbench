#ifndef RenderDevice_h__
#define RenderDevice_h__

#include "Math/GWVectors.h"

#include <Windows.h>

struct IDirect3DDevice9;

class RenderSystem
{
public:
	static void Initialize(IDirect3DDevice9* device) { m_sDevice = device; }
	static IDirect3DDevice9* Device() { return m_sDevice; }

	static void DrawSprite(const Vector2& vMin, const Vector2& vMax, int tex_id = -1, float depth = 0.0f);
	static void DrawColoredSprite(const Vector2& vMin, const Vector2& vMax, DWORD color = 0xFFFFFFFF, bool wireframe = false, float depth = 0.0f);

private:
	static IDirect3DDevice9* m_sDevice;
};

#endif // RenderDevice_h__