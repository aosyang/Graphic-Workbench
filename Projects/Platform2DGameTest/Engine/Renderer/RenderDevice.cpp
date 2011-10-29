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
		m_sDevice->SetTexture(0, tex->d3d_tex);

		// enable mip-map for texture
		m_sDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		m_sDevice->SetTexture(0, NULL);
	}

	m_sDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_sDevice->SetFVF(TexturedSpriteFVF);
	m_sDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(TexturedVertex));

}


#define ColoredSpriteVertexFVF D3DFVF_XYZ|D3DFVF_DIFFUSE

struct ColoredVertex
{
	float x, y, z;
	DWORD color;
};

void RenderSystem::Initialize( IDirect3DDevice9* device )
{
	m_sDevice = device;

	D3DXCreateFont( device, KLEIN_FONT_HEIGHT, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
					OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
					L"Arial", &m_sFont );
}

void RenderSystem::Destroy()
{
	if (m_sFont)
	{
		m_sFont->Release();
		m_sFont = NULL;
	}
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

	m_sDevice->SetTexture(0, NULL);

	m_sDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_sDevice->SetFVF(ColoredSpriteVertexFVF);
	m_sDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(ColoredVertex));
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

	m_sDevice->SetTexture(0, NULL);

	m_sDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_sDevice->SetFVF(ColoredSpriteVertexFVF);
	m_sDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, v, sizeof(ColoredVertex));
}

void RenderSystem::DrawText( const char* text, int x, int y, DWORD color /*= 0xFFFFFFFF */ )
{
	RECT font_rect;
	SetRect( &font_rect, x, y, KLEIN_SCREEN_WIDTH, KLEIN_SCREEN_HEIGHT );
	m_sFont->DrawTextA( NULL, text, -1, &font_rect, DT_LEFT|DT_NOCLIP, color );
}

ID3DXFont* RenderSystem::m_sFont = NULL;
