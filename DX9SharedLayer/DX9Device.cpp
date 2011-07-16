#include "DX9Device.h"

LPDIRECT3DDEVICE9& D3DDevice()
{
	static LPDIRECT3DDEVICE9 d3dDevice = NULL;
	return d3dDevice;
}
