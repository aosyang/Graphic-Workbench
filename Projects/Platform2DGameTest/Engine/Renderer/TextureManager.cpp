/********************************************************************
	created:	2011/9/5
	filename: 	TextureManager.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "TextureManager.h"

TextureManager::TextureManager()
: m_TextureID(0)
{
}

void TextureManager::Reset()
{
	TextureMap::iterator iter;

	for (iter=m_Textures.begin(); iter!=m_Textures.end(); iter++)
	{
		RenderSystem::DestroyTexture(iter->second);
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

const TEXTURE_INFO* TextureManager::GetTexture( int id ) const
{
	TextureMap::const_iterator iter;

	if ( (iter=m_Textures.find(id)) != m_Textures.end() )
	{
		return iter->second;
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
	TEXTURE_INFO* tex = RenderSystem::CreateTexture(filename);

	if (!tex) return false;

	int id;
	id = m_TextureID++;

	m_TextureNameMap[filename] = id;
	m_Textures[id] = tex;

	return true;
}
