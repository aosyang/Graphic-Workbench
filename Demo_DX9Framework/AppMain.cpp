/********************************************************************
	created:	2011/07/16
	filename: 	AppMain.cpp
	project: 	Demo_DX9Framework
	author:		Mwolf
	
	purpose:	A basic framework with dx9 rendering
*********************************************************************/

#include <d3d9.h>
#include <d3dx9.h>
#include "../GraphicWorkbench/Workbench.h"
#include "../libEmdMesh/EmdMesh.h"

#pragma comment (lib, "d3d9.lib")
#if defined _DEBUG
#pragma comment (lib, "d3dx9d.lib")
#else
#pragma comment (lib, "d3dx9.lib")
#endif

#include "DX9MeshBuffer.h"

LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device
float				g_ScreenAspect;

DX9MeshBuffer		g_MeshBuffer;

void Update(uint32 deltaTime);
HRESULT InitD3D(HWND hWnd);
void LoadMesh();
void SetupMatrices();
void Cleanup();

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	RenderWindow* rw = new RenderWindow;
	rw->Create(L"Direct3D 9 Framework - Graphic Workbench", 640, 480, 32, false);

	g_ScreenAspect = 640.f / 480.f;

	HWND hWnd = rw->GetWindowHandle()->handle;
	InitD3D(hWnd);
	LoadMesh();

	Timer::UpdateElapsedTime();
	uint32 elapsedTime = Timer::GetElapsedTime();

	while (rw->HandleWindowMessage())
	{
		uint32 deltaTime = Timer::GetElapsedTime() - elapsedTime;
		elapsedTime = Timer::GetElapsedTime();

		Update(deltaTime);

		Timer::UpdateElapsedTime();
	}

	Cleanup();

	rw->Destroy();
	delete rw;

	return 0;
}

void Update( uint32 deltaTime )
{
	// Clear the backbuffer and the zbuffer
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_XRGB( 127, 127, 255 ), 1.0f, 0 );


	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		SetupMatrices();

		g_MeshBuffer.Render();

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

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
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// Turn on the zbuffer
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	// Turn on ambient lighting 
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	return S_OK;
}

void LoadMesh()
{
	EmdMesh mesh;
	if (mesh.LoadMesh(L"../Data/Mesh/scene.emd"))
	{
		g_MeshBuffer.CreateFromMesh(&mesh);
	}
}

void SetupMatrices()
{
	// Set up world matrix
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationY( &matWorld, (float)Timer::GetElapsedTime() / 1000.0f );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	D3DXMATRIXA16 mat;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixTranslation(&mat, 10.0f, 10.0f, 15.0f);

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, g_ScreenAspect, 1.0f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

void Cleanup()
{
	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}
