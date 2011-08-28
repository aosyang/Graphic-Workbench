#include "DX9Device.h"

#pragma comment (lib, "d3d9.lib")

#if defined (_DEBUG)
#pragma comment (lib, "d3dx9d.lib")
#else
#pragma comment (lib, "d3dx9.lib")
#endif

static LPDIRECT3DDEVICE9 g_d3dDevice = NULL;

LPDIRECT3DDEVICE9& D3DDevice()
{
	return g_d3dDevice;
}

DX9Device::DX9Device()
: m_pD3D(NULL)
{
}

bool DX9Device::Init( HWND hWnd )
{
	// Create the D3D object.
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	// Set up the structure used to create the D3DDevice. Since we are now
	// using more complex geometry, we will create a device with a zbuffer.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// Create the D3DDevice
	if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &D3DDevice() ) ) )
	{
		return false;
	}

	// Turn on the zbuffer
	D3DDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );

	return true;
}

void DX9Device::Clear()
{
	D3DDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0x7f, 0x7f, 0xff), 1.0f, 0);
}

void DX9Device::BeginScene()
{
	D3DDevice()->BeginScene();
}

void DX9Device::EndScene()
{
	D3DDevice()->EndScene();
}

void DX9Device::Present()
{
	D3DDevice()->Present( NULL, NULL, NULL, NULL );
}

void DX9Device::SetViewMatrix( const D3DXVECTOR3& vEyePt, const D3DXVECTOR3& vLookatPt, const D3DXVECTOR3& vUpVec )
{
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DDevice()->SetTransform(D3DTS_VIEW, &matView);
}


