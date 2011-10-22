#include "TextureManager.h"
#include "../DXUT/DXUT.h"

TextureManager::TextureManager()
: m_TextureID(0)
{
}

void TextureManager::Reset()
{
	TextureMap::iterator iter;

	for (iter=m_Textures.begin(); iter!=m_Textures.end(); iter++)
	{
		iter->second.d3d_tex->Release();
	}

	m_TextureNameMap.clear();
	m_Textures.clear();
	m_TextureID = 0;
}

int TextureManager::GetTextureID(const char* filename) const
{
	TextureNameMap::const_iterator iter;

	if ( (iter=m_TextureNameMap.find(filename)) != m_TextureNameMap.end() )
	{
		return iter->second;
	}

	return -1;
}

LPDIRECT3DTEXTURE9 TextureManager::GetD3DTexture( int id ) const
{
	TextureMap::const_iterator iter;

	if ( (iter=m_Textures.find(id)) != m_Textures.end() )
	{
		return iter->second.d3d_tex;
	}

	return NULL;
}

const char* TextureManager::GetTextureName( int id ) const
{
	TextureNameMap::const_iterator iter;
	for (iter=m_TextureNameMap.begin(); iter!=m_TextureNameMap.end(); iter++)
	{
		if (iter->second == id)
		{
			return iter->first.c_str();
		}
	}

	return NULL;
}

bool TextureManager::LoadTextureFromFile( const char* filename )
{
	int id;
	LPDIRECT3DTEXTURE9 tex;

	if (FAILED(D3DXCreateTextureFromFileA(RenderSystem::Device(), filename, &tex)))
		return false;

	id = m_TextureID++;

	m_TextureNameMap[filename] = id;
	m_Textures[id].d3d_tex = tex;

	return true;
}
