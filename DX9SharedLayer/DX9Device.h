#ifndef DX9Device_h__
#define DX9Device_h__

#include <d3d9.h>
#include <d3dx9.h>

#include "DllExport.h"

DLL_EXPORT LPDIRECT3DDEVICE9& D3DDevice();

#ifndef V
#define V(x)           { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

class DLL_EXPORT_CLASS DX9Device
{
public:
	DX9Device();
	bool Init(HWND hWnd);

	void Clear();
	void BeginScene();
	void EndScene();
	void Present();

	void SetWindowSize(int width, int height);
	void ResetDevice();

	void SetViewMatrix(const D3DXVECTOR3& vEyePt, const D3DXVECTOR3& vLookatPt, const D3DXVECTOR3& vUpVec);

	void SetProjectionMatrix(float fovy, float aspect, float zn, float zf);

private:
	LPDIRECT3D9		m_pD3D;
	D3DPRESENT_PARAMETERS	m_d3dParam;

	float			m_ScreenAspect;
};

#endif // DX9Device_h__