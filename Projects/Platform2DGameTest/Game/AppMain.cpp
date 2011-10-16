
#include <windows.h>
#include <Windowsx.h>
#include "../DXUT/DXUT.h"
#include "../DXUT/SDKmisc.h"

#include <d3dx9.h>

#include "RenderDevice.h"
#include "GameDef.h"
#include "GameMain.h"

LPDIRECT3DDEVICE9 RenderSystem::m_sDevice = NULL;
GameMain*		g_Game		= NULL;
LPD3DXFONT		g_pFont;

int				g_MousePosX = 0,
				g_MousePosY = 0;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
								void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
							   void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
						 void* pUserContext );
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
						 bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );
void CALLBACK OnLostDevice( void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );
void Startup();
void Cleanup();

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	// setup callbacks for DXUT
	DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice);
	DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
	DXUTSetCallbackD3D9FrameRender( OnFrameRender );
	DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
	DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( KeyboardProc );
	DXUTSetCallbackMouse( MouseProc );
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

	DXUTSetCursorSettings( true, true );

	DXUTInit();
	DXUTSetHotkeyHandling();
	DXUTCreateWindow( L"2D Platform Test" );
	DXUTCreateDevice( true, KLEIN_SCREEN_WIDTH, KLEIN_SCREEN_HEIGHT );

	Startup();

	DXUTMainLoop();

	Cleanup();

	return DXUTGetExitCode();
}

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	assert( DXUT_D3D9_DEVICE == pDeviceSettings->ver );

	HRESULT hr;
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	D3DCAPS9 caps;

	V( pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal,
		pDeviceSettings->d3d9.DeviceType,
		&caps ) );

	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
	// then switch to SWVP.
	if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
		caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
	{
		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// Debugging vertex shaders requires either REF or software vertex processing 
	// and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
	if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
	{
		pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
		pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
#endif
#ifdef DEBUG_PS
	pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif

	// Enable anti-aliasing for HAL devices which support it
	CD3D9Enumeration* pEnum = DXUTGetD3D9Enumeration();
	CD3D9EnumDeviceSettingsCombo* pCombo = pEnum->GetDeviceSettingsCombo( &pDeviceSettings->d3d9 );

	if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_HAL &&
		pCombo->multiSampleTypeList.Contains( D3DMULTISAMPLE_4_SAMPLES ) )
	{
		pDeviceSettings->d3d9.pp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
		pDeviceSettings->d3d9.pp.MultiSampleQuality = 0;
	}
	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if( s_bFirstTime )
	{
		s_bFirstTime = false;
		if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
			DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
	}

	return true;
}

HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	//LPD3DXBUFFER errorString;

	RenderSystem::Initialize( pd3dDevice );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	V_RETURN( D3DXCreateFont( pd3dDevice, 16, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
							  OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
							  L"Arial", &g_pFont ) );
	return S_OK;
}

HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	//HRESULT hr;

	g_pFont->OnResetDevice();

	return S_OK;
}

void CALLBACK OnLostDevice( void* pUserContext )
{
	g_pFont->OnLostDevice();
}

void CALLBACK OnDestroyDevice( void* pUserContext )
{
	SAFE_RELEASE( g_pFont );
}

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	g_Game->Update(fElapsedTime);

}

void SetupLight() 
{
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	HRESULT hr;

	// For our world matrix, we will just rotate the object about the y-axis.
	D3DXMATRIXA16 matWorld;

	// Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
	// every 1000 ms. To avoid the loss of precision inherent in very high 
	// floating point numbers, the system time is modulated by the rotation 
	// period before conversion to a radian angle.
	//UINT iTime = timeGetTime() % 1000;
	//FLOAT fAngle = iTime * ( 2.0f * D3DX_PI ) / 1000.0f;
	//D3DXMatrixRotationY( &matWorld, fAngle );
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	Vector3 cam_pos = g_Game->GetCameraPos();

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.

#if 1		// Fixed Camera
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, KLEIN_CAMERA_ZPOS );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
#else
	D3DXVECTOR3 vEyePt( cam_pos.x, cam_pos.y, KLEIN_CAMERA_ZPOS );
	D3DXVECTOR3 vLookatPt( cam_pos.x, cam_pos.y, 0.0f );
#endif

	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, KLEIN_CAMERA_FOVY, KLEIN_SCREEN_ASPECT, 1.0f, 100.0f );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Clear the render target and the zbuffer 
	V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 141, 153, 191 ), 1.0f, 0 ) );

	// Render the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		g_Game->Render();

		RECT font_rect;
		SetRect( &font_rect, 0, 0, 400, 400 );
		g_pFont->DrawTextA( NULL, g_Game->GetDebugText(), -1, &font_rect, DT_LEFT|DT_NOCLIP, 0xFFFFFF00 );

		V( pd3dDevice->EndScene() );
	}
}

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		g_Game->SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		g_MousePosX = GET_X_LPARAM(lParam); 
		g_MousePosY = GET_Y_LPARAM(lParam); 
		break;
	}

	return 0;
}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	g_Game->SetKeyState(nChar, bKeyDown);
}

void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
						 bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext )
{
	g_Game->SetMouseBtnState(MBTN_LEFT, bLeftButtonDown);
	g_Game->SetMouseBtnState(MBTN_RIGHT, bRightButtonDown);

	//g_MousePosX = xPos;
	//g_MousePosY = yPos;
}

void Startup()
{
	g_Game = new GameMain();
	g_Game->Startup();
}

void Cleanup()
{
	SAFE_DELETE(g_Game);
}
