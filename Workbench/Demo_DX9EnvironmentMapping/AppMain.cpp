/********************************************************************
	created:	2011/07/24
	filename: 	AppMain.cpp
	project: 	Demo_DX9EnvironmentMapping
	author:		Mwolf
	
	purpose:	
*********************************************************************/


#include "../DX9SharedLayer/DX9SharedLayer.h"

#include "../GraphicWorkbench/Workbench.h"
#include "../libEmdMesh/EmdMesh.h"

LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
ID3DXEffect*		g_pEffect = NULL;
ID3DXEffectPool*	g_pEffectPool = NULL;

float				g_ScreenAspect;
Vector3				g_CenterPoint;

DX9MeshBuffer		g_MeshBuffer;
LPDIRECT3DCUBETEXTURE9	g_EnvTexture = NULL;

void Update(uint32 deltaTime);
HRESULT InitD3D(HWND hWnd, uint32 width, uint32 height);
void LoadMesh();
void SetupLights();
void Cleanup();

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	RenderWindow* rw = new RenderWindow;
	rw->Create(L"Direct3D 9 Environment Mapping - Graphic Workbench", 1024, 768, 32, false);

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
	HRESULT hr;

	float rot = (float)Timer::GetElapsedTime() / 5000.0f;

	// Clear the backbuffer and the zbuffer
	D3DDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB( 127, 127, 255 ), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( D3DDevice()->BeginScene() ) )
	{

		// Set up our view matrix. A view matrix can be defined given an eye point,
		// a point to lookat, and a direction for which way is up. Here, we set the
		// eye five units back along the z-axis and up three units, look at the
		// origin, and define "up" to be in the y-direction.
		D3DXVECTOR3 vEyePt( sinf(rot) * 5.0f, 5.0f, cosf(rot) * 5.0f );
		D3DXVECTOR3 vLookatPt( 0.0f, g_CenterPoint.y, 0.0f );
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

		// For the projection matrix, we set up a perspective transform (which
		// transforms geometry from 3D view space to 2D viewport space, with
		// a perspective divide making objects smaller in the distance). To build
		// a perpsective transform, we need the field of view (1/4 pi is common),
		// the aspect ratio, and the near and far clipping planes (which define at
		// what distances geometry should be no longer be rendered).
		D3DXMATRIXA16 matProj;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, g_ScreenAspect, 1.0f, 100.0f );

		// Shared parameters for shaders
		D3DXMATRIXA16 mViewProjection = matView * matProj;
		V( g_pEffect->SetMatrix( "matView", &matView ) );
		V( g_pEffect->SetMatrix( "matProjection", &matProj ) );
		V( g_pEffect->SetMatrix( "matViewProjection", &mViewProjection ) );
		V( g_pEffect->SetFloatArray( "vEyePosWorld", vEyePt, 3 ));

		// Set up world matrix
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity( &matWorld );
		//D3DXMatrixRotationY( &matWorld, rot );

		D3DXMATRIXA16 mWorldViewProjection = matWorld * mViewProjection;

		V( g_pEffect->SetMatrix( "matWorld", &matWorld ) );
		V( g_pEffect->SetMatrix( "matWorldViewProjection", &mWorldViewProjection ) );

		float L00[3] = { 0.79f, 0.44f, 0.54f };
		float L1_1[3] = { 0.39f, 0.35f, 0.60f };
		float L10[3] = { -0.34f, -0.18f, -0.27f };
		float L11[3] = { -0.29f, -0.06f, 0.01f };
		float L2_2[3] = { -0.11f, -0.05f, -0.12f };
		float L2_1[3] = { -0.26f, -0.22f, -0.47f };
		float L20[3] = { -0.16f, -0.09f, -0.15f };
		float L21[3] = { 0.56f, 0.21f, 0.14f };
		float L22[3] = { 0.21f, -0.05f, -0.3f };

		V( g_pEffect->SetFloatArray("L00", L00, 3) );
		V( g_pEffect->SetFloatArray("L1_1", L1_1, 3) );
		V( g_pEffect->SetFloatArray("L10", L10, 3) );
		V( g_pEffect->SetFloatArray("L11", L11, 3) );
		V( g_pEffect->SetFloatArray("L2_2", L2_2, 3) );
		V( g_pEffect->SetFloatArray("L2_1", L2_1, 3) );
		V( g_pEffect->SetFloatArray("L20", L20, 3) );
		V( g_pEffect->SetFloatArray("L21", L21, 3) );
		V( g_pEffect->SetFloatArray("L22", L22, 3) );

		V( g_pEffect->SetTexture( "texEnv", g_EnvTexture ) );

		V( g_pEffect->SetTechnique( "Default" ) );

		UINT iPass, cPasses;

		// Apply the technique contained in the effect 
		V( g_pEffect->Begin( &cPasses, 0 ) );

		for( iPass = 0; iPass < cPasses; iPass++ )
		{
			V( g_pEffect->BeginPass( iPass ) );

			g_MeshBuffer.Render();

			V( g_pEffect->EndPass() );
		}
		V( g_pEffect->End() );

		// End the scene
		D3DDevice()->EndScene();
	}

	// Present the backbuffer contents to the display
	D3DDevice()->Present( NULL, NULL, NULL, NULL );
}

HRESULT InitD3D( HWND hWnd, uint32 width, uint32 height )
{
	HRESULT hr;

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

	V_RETURN ( D3DXCreateCubeTextureFromFile( D3DDevice(), L"../Data/Texture/grace_cube.dds", &g_EnvTexture ) );

	V_RETURN ( D3DXCreateEffectPool( &g_pEffectPool ) );

	LPD3DXBUFFER errorString;

	if ( FAILED( D3DXCreateEffectFromFile( D3DDevice(), L"../Data/Fx/EnvMapping.fx", NULL, NULL, NULL, 
		g_pEffectPool, &g_pEffect, &errorString ) ) )
	{
		OutputDebugStringA( (LPCSTR)errorString->GetBufferPointer() );
	}

	return S_OK;
}

void LoadMesh()
{
	EMD_MESH* mesh;
	if (NULL != (mesh = EMD_LoadMeshFromFile("../Data/Mesh/Berserker.emd")))
	{
		Vector3 vecMax, vecMin;

		EMD_GetBoundingMax(mesh, &vecMax.x, &vecMax.y, &vecMax.z);
		EMD_GetBoundingMin(mesh, &vecMin.x, &vecMin.y, &vecMin.z);

		g_CenterPoint = (vecMax + vecMin) * 0.5f;
		g_MeshBuffer.CreateFromMesh(mesh);

		EMD_FreeMesh(mesh);
	}
}

void Cleanup()
{
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pEffectPool);

	if( D3DDevice() != NULL )
		D3DDevice()->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}
