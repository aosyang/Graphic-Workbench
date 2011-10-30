/********************************************************************
	created:	2011/9/5
	filename: 	RenderDevice.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "RenderDevice.h"
#include "TextureManager.h"
#include "../Game/GameDef.h"

#include <d3d9.h>
#include <d3dx9.h>

static LPDIRECT3D9			pD3D = NULL;
static LPDIRECT3DDEVICE9	pD3Ddevice = NULL;
static LPD3DXFONT			pFont = NULL;

#define TexturedSpriteFVF D3DFVF_XYZ|D3DFVF_TEX1

struct TexturedVertex
{
	float x, y, z;
	float u, v;
};

void RenderSystem::DrawSprite( const Vector2& vMin, const Vector2& vMax, int tex_id /*= -1*/, float depth /*= 0.0f*/ )
{
	TexturedVertex v[6] =
	{
		{ vMin.x, vMin.y, depth, vMin.x, vMax.y },
		{ vMin.x, vMax.y, depth, vMin.x, vMin.y },
		{ vMax.x, vMax.y, depth, vMax.x, vMin.y },

		{ vMax.x, vMax.y, depth, vMax.x, vMin.y },
		{ vMax.x, vMin.y, depth, vMax.x, vMax.y },
		{ vMin.x, vMin.y, depth, vMin.x, vMax.y },
	};


	if (tex_id!=-1)
	{
		const TEXTURE_INFO* tex = TextureManager::Instance().GetTexture(tex_id);
		pD3Ddevice->SetTexture(0, tex->d3d_tex);

		// enable mip-map for texture
		pD3Ddevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		pD3Ddevice->SetTexture(0, NULL);
	}

	pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	pD3Ddevice->SetFVF(TexturedSpriteFVF);
	pD3Ddevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(TexturedVertex));

}


#define ColoredSpriteVertexFVF D3DFVF_XYZ|D3DFVF_DIFFUSE

struct ColoredVertex
{
	float x, y, z;
	DWORD color;
};

void RenderSystem::Initialize( GW_RENDER_WINDOW* rw )
{
	HWND hWnd = *((HWND*)rw->params[WND_PARAM_WIN32_HANDLE]);

	// Create d3d9 render device
	pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	D3DPRESENT_PARAMETERS pres_params;
	ZeroMemory( &pres_params, sizeof(pres_params) );

	pres_params.Windowed = TRUE;
	pres_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pres_params.BackBufferFormat = D3DFMT_UNKNOWN;
	//pres_params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	pres_params.EnableAutoDepthStencil = TRUE;
	pres_params.AutoDepthStencilFormat = D3DFMT_D16;

	pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_MIXED_VERTEXPROCESSING,
						&pres_params, &pD3Ddevice );

	D3DXCreateFont( pD3Ddevice, KLEIN_FONT_HEIGHT, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
					OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
					L"Arial", &pFont );

	pD3Ddevice->SetRenderState( D3DRS_LIGHTING, FALSE );
}

void RenderSystem::Destroy()
{
	if (pFont)
	{
		pFont->Release();
		pFont = NULL;
	}
}

TEXTURE_INFO* RenderSystem::CreateTexture( const char* filename )
{
	LPDIRECT3DTEXTURE9 d3d_tex;

	if (FAILED(D3DXCreateTextureFromFileA(pD3Ddevice, filename, &d3d_tex)))
		return NULL;

	TEXTURE_INFO* tex = new TEXTURE_INFO;
	memset(tex, 0, sizeof(TEXTURE_INFO));

	LPDIRECT3DSURFACE9 surf;
	D3DSURFACE_DESC desc;
	d3d_tex->GetSurfaceLevel(0, &surf);
	surf->GetDesc(&desc);

	tex->width = desc.Width;
	tex->height = desc.Height;

	tex->d3d_tex = d3d_tex;

	return tex;
}

void RenderSystem::DestroyTexture( TEXTURE_INFO* texture )
{
	texture->d3d_tex->Release();
	delete texture;
}

void RenderSystem::SetupCamera( const Vector2& cam_pos, float fovy )
{
	D3DXMATRIXA16 matWorld;

	D3DXMatrixIdentity(&matWorld);
	pD3Ddevice->SetTransform( D3DTS_WORLD, &matWorld );

	D3DXVECTOR3 vEyePt( cam_pos.x, cam_pos.y, KLEIN_CAMERA_ZPOS );
	D3DXVECTOR3 vLookatPt( cam_pos.x, cam_pos.y, 0.0f );

	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	pD3Ddevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, fovy, KLEIN_SCREEN_ASPECT, 1.0f, 100.0f );
	pD3Ddevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

void RenderSystem::DrawColoredSprite( const Vector2& vMin, const Vector2& vMax, DWORD color /*= 0xFFFFFFFF*/, float depth /*= 0.0f*/ )
{
	ColoredVertex v[6] =
	{
		{ vMin.x, vMin.y, depth, color },
		{ vMin.x, vMax.y, depth, color },
		{ vMax.x, vMax.y, depth, color },

		{ vMax.x, vMax.y, depth, color },
		{ vMax.x, vMin.y, depth, color },
		{ vMin.x, vMin.y, depth, color },
	};

	pD3Ddevice->SetTexture(0, NULL);

	pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pD3Ddevice->SetFVF(ColoredSpriteVertexFVF);
	pD3Ddevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(ColoredVertex));
}

void RenderSystem::DrawWireframeRect( const Vector2& vMin, const Vector2& vMax, DWORD color /*= 0xFFFFFFFF*/, float depth /*= 0.0f*/ )
{
	ColoredVertex v[6] =
	{
		{ vMin.x, vMin.y, depth, color },
		{ vMin.x, vMax.y, depth, color },
		{ vMax.x, vMax.y, depth, color },

		{ vMax.x, vMin.y, depth, color },
		{ vMin.x, vMin.y, depth, color },
	};

	pD3Ddevice->SetTexture(0, NULL);

	pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pD3Ddevice->SetFVF(ColoredSpriteVertexFVF);
	pD3Ddevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, v, sizeof(ColoredVertex));
}

void RenderSystem::DrawText( const char* text, int x, int y, DWORD color /*= 0xFFFFFFFF */ )
{
	RECT font_rect;
	SetRect( &font_rect, x, y, KLEIN_SCREEN_WIDTH, KLEIN_SCREEN_HEIGHT );
	pFont->DrawTextA( NULL, text, -1, &font_rect, DT_LEFT|DT_NOCLIP, color );
}

void RenderSystem::Clear()
{
	// Clear the render target and the zbuffer 
	pD3Ddevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 141, 153, 191 ), 1.0f, 0 );
}

void RenderSystem::BeginRender()
{
	pD3Ddevice->BeginScene();
	pD3Ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pD3Ddevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	pD3Ddevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pD3Ddevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
}

void RenderSystem::EndRender()
{
	pD3Ddevice->EndScene();
}

void RenderSystem::Flush()
{
	pD3Ddevice->Present( NULL, NULL, NULL, NULL );
}
