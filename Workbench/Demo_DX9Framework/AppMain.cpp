/********************************************************************
	created:	2011/07/16
	filename: 	AppMain.cpp
	project: 	Demo_DX9Framework
	author:		Mwolf
	
	purpose:	A basic framework with dx9 rendering
*********************************************************************/

#include "../DX9SharedLayer/DX9SharedLayer.h"

#include "../GraphicWorkbench/Workbench.h"
#include "../libemd/emd_mesh.h"

LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
float				g_ScreenAspect;
Vector3				g_CenterPoint;

DX9MeshBuffer		g_MeshBuffer;

void Update(uint32 deltaTime);
HRESULT InitD3D(HWND hWnd, uint32 width, uint32 height);
void LoadMesh();
void SetupLights();
void SetupMatrices();
void Cleanup();

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	RenderWindow* rw = new RenderWindow;
	rw->Create(L"Direct3D 9 Framework - Graphic Workbench", 640, 480, 32, false);

	g_ScreenAspect = (float)rw->GetWidth() / rw->GetHeight();

	HWND hWnd = rw->GetWindowHandle()->handle;
	InitD3D(hWnd, rw->GetWidth(), rw->GetHeight());
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
	D3DDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_XRGB( 127, 127, 255 ), 1.0f, 0 );


	// Begin the scene
	if( SUCCEEDED( D3DDevice()->BeginScene() ) )
	{
		SetupLights();

		SetupMatrices();

		g_MeshBuffer.Render();

		// End the scene
		D3DDevice()->EndScene();
	}

	// Present the backbuffer contents to the display
	D3DDevice()->Present( NULL, NULL, NULL, NULL );
}

HRESULT InitD3D( HWND hWnd, uint32 width, uint32 height )
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
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &D3DDevice() ) ) )
	{
		return E_FAIL;
	}

	// Turn on the zbuffer
	D3DDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );

	return S_OK;
}

void LoadMesh()
{
	EMD_MESH* mesh;
	if (NULL != (mesh = EMD_LoadMeshFromFile("../Data/Mesh/scene.emd")))
	{
		Vector3 vecMax, vecMin;

		EMD_GetBoundingMax(mesh, &vecMax.x, &vecMax.y, &vecMax.z);
		EMD_GetBoundingMin(mesh, &vecMin.x, &vecMin.y, &vecMin.z);

		g_CenterPoint = (vecMax + vecMin) * 0.5f;
		g_MeshBuffer.CreateFromMesh(mesh);

		EMD_FreeMesh(mesh);
	}
}

void SetupLights()
{
	// Set up a material. The material here just has the diffuse and ambient
	// colors set to yellow. Note that only one material can be used at a time.
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof( D3DMATERIAL9 ) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	D3DDevice()->SetMaterial( &mtrl );

	// Set up a white, directional light, with an oscillating direction.
	// Note that many Lights may be active at a time (but each one slows down
	// the rendering of our scene). However, here we are just using one. Also,
	// we need to set the D3DRS_LIGHTING renderstate to enable lighting
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof( D3DLIGHT9 ) );
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	vecDir = D3DXVECTOR3( -1.0f, -1.0f, 1.0f );
	D3DXVec3Normalize( ( D3DXVECTOR3* )&light.Direction, &vecDir );
	light.Range = 1000.0f;
	D3DDevice()->SetLight( 0, &light );
	D3DDevice()->LightEnable( 0, TRUE );
	D3DDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );

	// Finally, turn on some ambient light.
	D3DDevice()->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
}

void SetupMatrices()
{
	// Set up world matrix
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationY( &matWorld, (float)Timer::GetElapsedTime() / 1000.0f );
	D3DDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.
	D3DXVECTOR3 vEyePt( 0.0f, 5.0f,-5.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, g_CenterPoint.y, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	D3DDevice()->SetTransform( D3DTS_VIEW, &matView );

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, g_ScreenAspect, 1.0f, 100.0f );
	D3DDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
}

void Cleanup()
{
	if( D3DDevice() != NULL )
		D3DDevice()->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}
