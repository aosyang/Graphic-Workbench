/********************************************************************
	created:	2011/07/17
	filename: 	AppMain.cpp
	project: 	Demo_DX9ShadowMap
	author:		Mwolf
	
	purpose:	Dx9 shadow map demo
*********************************************************************/

#include "../DX9SharedLayer/DX9SharedLayer.h"

#include "../GraphicWorkbench/Workbench.h"
#include "../libEmdMesh/EmdMesh.h"

LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
ID3DXEffect*		g_pEffect = NULL;
ID3DXEffectPool*	g_pEffectPool = NULL;

LPDIRECT3DTEXTURE9	g_DepthTexture = NULL;
IDirect3DSurface9*	g_DepthBuffer = NULL;
IDirect3DSurface9*	g_BackBuffer = NULL;
#define SHADOW_BUFFER_SIZE 512

float				g_ScreenAspect;
Vector3				g_CenterPoint;

DX9MeshBuffer		g_MeshBuffer;

void Update(uint32 deltaTime);
HRESULT InitD3D(HWND hWnd, uint32 width, uint32 height);
void LoadMesh();
void SetupLights();
void Cleanup();

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	RenderWindow* rw = new RenderWindow;
	rw->Create(L"Direct3D 9 Shadow map - Graphic Workbench", 1024, 768, 32, false);

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

	// Render to render target
	D3DDevice()->SetRenderTarget(0, g_DepthBuffer);

	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = SHADOW_BUFFER_SIZE;
	vp.Height = SHADOW_BUFFER_SIZE;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	D3DDevice()->SetViewport(&vp);

	// clear depth buffer
	D3DDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xff, 0xff, 0xff), 1.0f, 0);

	D3DXVECTOR4 vecDir;

	// Begin the scene
	if( SUCCEEDED( D3DDevice()->BeginScene() ) )
	{
		// Setup light view
		D3DXVECTOR3 vEyePt( -5.0f, 5.0f, -5.0f );
		D3DXVECTOR3 vLookatPt( 0.0f, g_CenterPoint.y, 0.0f );
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

		vecDir = D3DXVECTOR4(vLookatPt - vEyePt, 0.0f);
		D3DXVec4Normalize( &vecDir, &vecDir );

		D3DXMATRIXA16 matProj;
		D3DXMatrixOrthoLH(&matProj, 15.0f, 15.0f, 1.0f, 40.0f);

		// Shared parameters for shaders
		D3DXMATRIXA16 mViewProjection = matView * matProj;
		V( g_pEffect->SetMatrix( "matView", &matView ) );
		V( g_pEffect->SetMatrix( "matProjection", &matProj ) );
		V( g_pEffect->SetMatrix( "matViewProjection", &mViewProjection ) );


		// Set up world matrix
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixRotationY( &matWorld, rot );

		D3DXMATRIXA16 mWorldViewProjection = matWorld * mViewProjection;

		V( g_pEffect->SetMatrix( "matWorld", &matWorld ) );
		V( g_pEffect->SetMatrix( "matWorldViewProjection", &mWorldViewProjection ) );
		V( g_pEffect->SetMatrix( "matLightSpace", &mWorldViewProjection ) );

		D3DXMATRIXA16 matBias;
		float bias[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
						   0.0f, -0.5f, 0.0f, 0.0f,
						   0.0f, 0.0f, 0.5f, 0.0f,
						   0.5f, 0.5f, 0.5f, 1.0f };
		memcpy(matBias, bias, sizeof(float)*16);

		D3DXMATRIXA16 matMVPBias = mWorldViewProjection * matBias;

		V( g_pEffect->SetMatrix( "matLightViewProjBias", &matMVPBias ) );

		V( g_pEffect->SetTechnique( "DepthMap" ) );

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


	D3DDevice()->SetRenderTarget(0, g_BackBuffer);
	D3DSURFACE_DESC desc;
	g_BackBuffer->GetDesc(&desc);

	vp.X = 0;
	vp.Y = 0;
	vp.Width = desc.Width;
	vp.Height = desc.Height;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	D3DDevice()->SetViewport(&vp);

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
		D3DXVECTOR3 vEyePt( 0.0f, 5.0f, -5.0f );
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

		// Set up world matrix
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixRotationY( &matWorld, rot );

		D3DXMATRIXA16 mWorldViewProjection = matWorld * mViewProjection;

		V( g_pEffect->SetMatrix( "matWorld", &matWorld ) );
		V( g_pEffect->SetMatrix( "matWorldViewProjection", &mWorldViewProjection ) );

		V( g_pEffect->SetVector( "vLightDir", &vecDir ) );
		V( g_pEffect->SetTexture( "texLightSpaceDepth", g_DepthTexture ) );

		V( g_pEffect->SetTechnique( "Shadowed" ) );

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

	V_RETURN ( D3DXCreateEffectPool( &g_pEffectPool ) );

	LPD3DXBUFFER errorString;

	if ( FAILED( D3DXCreateEffectFromFile( D3DDevice(), L"../Data/Fx/SingleLighting.fx", NULL, NULL, NULL, 
										   g_pEffectPool, &g_pEffect, &errorString ) ) )
	{
		//char* p = (char*)errorString->GetBufferPointer();
		OutputDebugStringA( (LPCSTR)errorString->GetBufferPointer() );
	}

	// store back buffer
	D3DDevice()->GetRenderTarget(0, &g_BackBuffer);
	//D3DSURFACE_DESC desc;
	//g_BackBuffer->GetDesc(&desc);

	//// Create shadow depth target
	//D3DDevice()->CreateRenderTarget(SHADOW_BUFFER_SIZE, SHADOW_BUFFER_SIZE, D3DFMT_R32F,
	//								desc.MultiSampleType, desc.MultiSampleQuality,
	//								false, &g_DepthBuffer, NULL);

	D3DXCreateTexture(D3DDevice(), SHADOW_BUFFER_SIZE, SHADOW_BUFFER_SIZE, 1, D3DUSAGE_RENDERTARGET,
					  D3DFMT_R32F, D3DPOOL_DEFAULT, &g_DepthTexture);
	g_DepthTexture->GetSurfaceLevel(0, &g_DepthBuffer);

	return S_OK;
}

void LoadMesh()
{
	EmdMesh mesh;
	if (mesh.LoadMesh(L"../Data/Mesh/scene.emd"))
	{
		g_CenterPoint = (mesh.GetBoundingMin() + mesh.GetBoundingMax()) * 0.5f;
		g_MeshBuffer.CreateFromMesh(&mesh);
	}
}

void Cleanup()
{
	SAFE_RELEASE(g_DepthBuffer);
	SAFE_RELEASE(g_DepthTexture);
	SAFE_RELEASE(g_BackBuffer);
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pEffectPool);

	if( D3DDevice() != NULL )
		D3DDevice()->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}
