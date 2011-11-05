/********************************************************************
	created:	2011/9/5
	filename: 	GWRenderDeviceDx9.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "Renderer/GWRenderDevice.h"
#include "../Game/GameDef.h"
#include "TGA.h"

#include "Win32/DX9/GWTextureDX9.h"

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

void RenderSystem::DrawSprite( const Vector2& vMin, const Vector2& vMax, const TEXTURE_INFO* tex /*= NULL*/, float depth /*= 0.0f*/ )
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


	if (tex)
	{
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

	float Start = 20.0f,    // Linear fog distances
		  End   = 28.0f;

	// Enable fog blending.
	pD3Ddevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	// Set the fog color.
	pD3Ddevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB( 0, 141, 153, 191 ));

	// Set fog parameters.
	pD3Ddevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
	pD3Ddevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
	pD3Ddevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
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

	GW_IMAGE tex;
	if (!LoadTGAImage(&tex, filename))
	{
		return NULL;
	}

	D3DFORMAT fmt = (tex.format == GW_PIXFMT_RGB) ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8;
	if ( FAILED(pD3Ddevice->CreateTexture(tex.width, tex.height, 0,
		 D3DUSAGE_DYNAMIC | D3DUSAGE_AUTOGENMIPMAP, fmt, D3DPOOL_DEFAULT, &d3d_tex, NULL) ) )
	{
		UnloadTGAImage(&tex);
		return NULL;
	}

	D3DLOCKED_RECT rect;
	if ( FAILED( d3d_tex->LockRect(0, &rect, NULL, 0) ) )
	{
		UnloadTGAImage(&tex);
		return NULL;
	}

	GW_UINT8* pRect = (GW_UINT8*) rect.pBits;
	switch (tex.format)
	{
	case GW_PIXFMT_RGB:
		for (GW_UINT32 x=0; x<tex.width; x++)
			for (GW_UINT32 y=0; y<tex.height; y++)
			{
				// vertical flip for dx9 texture
				int i = y * tex.width + x;
				int src = (tex.height - y - 1) * tex.width + x;

				pRect[i * 4] = tex.imageData[src * 3 + 2];
				pRect[i * 4 + 1] = tex.imageData[src * 3 + 1];
				pRect[i * 4 + 2] = tex.imageData[src * 3 + 0];
				pRect[i * 4 + 3] = 0xFF;

				//pRect[i * 4] = 0x00;		// B		
				//pRect[i * 4 + 1] = 0x00;	// G
				//pRect[i * 4 + 2] = 0xFF;	// R
				//pRect[i * 4 + 3] = 0xFF;	// A
			}
			break;

	case GW_PIXFMT_RGBA:
		for (GW_UINT32 x=0; x<tex.width; x++)
			for (GW_UINT32 y=0; y<tex.height; y++)
			{
				// vertical flip for dx9 texture
				int i = y * tex.width + x;
				int src = (tex.height - y - 1) * tex.width + x;

				pRect[i * 4] = tex.imageData[src * 4 + 2];
				pRect[i * 4 + 1] = tex.imageData[src * 4 + 1];
				pRect[i * 4 + 2] = tex.imageData[src * 4 + 0];
				pRect[i * 4 + 3] = tex.imageData[src * 4 + 3];
			}
			break;

	default:
		break;
	}

	d3d_tex->UnlockRect(0);

	TEXTURE_INFO* tex_info = new TEXTURE_INFO;
	memset(tex_info, 0, sizeof(TEXTURE_INFO));

	tex_info->width = tex.width;
	tex_info->height = tex.height;

	tex_info->d3d_tex = d3d_tex;

	UnloadTGAImage(&tex);

	return tex_info;
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

void RenderSystem::DrawColoredSprite( const Vector2& vMin, const Vector2& vMax, const GWColor& color /*= GWColor::WHITE*/, float depth /*= 0.0f*/ )
{
	DWORD d3d_color = color.ARGB();
	ColoredVertex v[6] =
	{
		{ vMin.x, vMin.y, depth, d3d_color },
		{ vMin.x, vMax.y, depth, d3d_color },
		{ vMax.x, vMax.y, depth, d3d_color },

		{ vMax.x, vMax.y, depth, d3d_color },
		{ vMax.x, vMin.y, depth, d3d_color },
		{ vMin.x, vMin.y, depth, d3d_color },
	};

	pD3Ddevice->SetTexture(0, NULL);

	pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pD3Ddevice->SetFVF(ColoredSpriteVertexFVF);
	pD3Ddevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(ColoredVertex));
}

void RenderSystem::DrawWireframeRect( const Vector2& vMin, const Vector2& vMax, const GWColor& color /*= GWColor::WHITE*/, float depth /*= 0.0f*/ )
{
	DWORD d3d_color = color.ARGB();

	ColoredVertex v[5] =
	{
		{ vMin.x, vMin.y, depth, d3d_color },
		{ vMin.x, vMax.y, depth, d3d_color },
		{ vMax.x, vMax.y, depth, d3d_color },

		{ vMax.x, vMin.y, depth, d3d_color },
		{ vMin.x, vMin.y, depth, d3d_color },
	};

	pD3Ddevice->SetTexture(0, NULL);

	pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	pD3Ddevice->SetFVF(ColoredSpriteVertexFVF);
	pD3Ddevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, v, sizeof(ColoredVertex));
}

void RenderSystem::RenderText( const char* text, int x, int y, const GWColor& color /*= GWColor::WHITE*/ )
{
	RECT font_rect;
	SetRect( &font_rect, x, y, KLEIN_SCREEN_WIDTH, KLEIN_SCREEN_HEIGHT );

	pFont->DrawTextA( NULL, text, -1, &font_rect, DT_LEFT|DT_NOCLIP, color.ARGB() );
}

void RenderSystem::Clear(const GWColor& color)
{
	// Clear the render target and the zbuffer 
	pD3Ddevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color.ARGB(), 1.0f, 0 );
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
