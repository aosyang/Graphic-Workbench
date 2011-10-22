#include "RenderDevice.h"
#include "TextureManager.h"

#include <d3d9.h>

#define TexturedSpriteFVF D3DFVF_XYZ|D3DFVF_TEX1

struct TexturedSpriteVertex
{
	float x, y, z;
	float u, v;
};

void RenderSystem::DrawSprite( const Vector2& vMin, const Vector2& vMax, int tex_id /*= -1*/, float depth /*= 0.0f*/ )
{
	TexturedSpriteVertex v[6] =
	{
		{ vMin.x, vMin.y, depth, vMin.x, vMin.y },
		{ vMin.x, vMax.y, depth, vMin.x, vMax.y },
		{ vMax.x, vMax.y, depth, vMax.x, vMax.y },

		{ vMax.x, vMax.y, depth, vMax.x, vMax.y },
		{ vMax.x, vMin.y, depth, vMax.x, vMin.y },
		{ vMin.x, vMin.y, depth, vMin.x, vMin.y },
	};


	if (tex_id!=-1)
	{
		LPDIRECT3DTEXTURE9 tex = TextureManager::Instance().GetD3DTexture(tex_id);
		m_sDevice->SetTexture(0, tex);

		// enable mip-map for texture
		m_sDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		m_sDevice->SetTexture(0, NULL);
	}

	m_sDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_sDevice->SetFVF(TexturedSpriteFVF);
	m_sDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(TexturedSpriteVertex));

}


#define ColoredSpriteVertexFVF D3DFVF_XYZ|D3DFVF_DIFFUSE

struct ColoredSpriteVertex
{
	float x, y, z;
	DWORD color;
};

void RenderSystem::DrawColoredSprite( const Vector2& vMin, const Vector2& vMax, DWORD color /*= 0xFFFFFFFF*/, bool wireframe /*= false*/, float depth /*= 0.0f*/ )
{
	ColoredSpriteVertex v[6] =
	{
		{ vMin.x, vMin.y, depth, color },
		{ vMin.x, vMax.y, depth, color },
		{ vMax.x, vMax.y, depth, color },

		{ vMax.x, vMax.y, depth, color },
		{ vMax.x, vMin.y, depth, color },
		{ vMin.x, vMin.y, depth, color },
	};

	m_sDevice->SetTexture(0, NULL);

	m_sDevice->SetRenderState(D3DRS_FILLMODE, wireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	m_sDevice->SetFVF(ColoredSpriteVertexFVF);
	m_sDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(ColoredSpriteVertex));
}
