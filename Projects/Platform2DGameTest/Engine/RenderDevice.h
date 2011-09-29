#ifndef RenderDevice_h__
#define RenderDevice_h__

#include <d3d9.h>

class RenderSystem
{
public:
	static void Initialize(LPDIRECT3DDEVICE9 device) { m_sDevice = device; }
	static LPDIRECT3DDEVICE9 Device() { return m_sDevice; }

private:
	static LPDIRECT3DDEVICE9 m_sDevice;
};

#endif // RenderDevice_h__