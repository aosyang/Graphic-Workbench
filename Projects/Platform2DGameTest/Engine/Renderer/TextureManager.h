/********************************************************************
	created:	2011/9/5
	filename: 	TextureManager.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef TextureManager_h__
#define TextureManager_h__

#include <map>
#include <string>

#include "GWRenderDevice.h"
#include "Win32/DX9/GWTextureDX9.h"

#include "GWSingleton.h"

class TextureManager : public GWSingleton<TextureManager>
{
	friend class GWSingleton<TextureManager>;
public:

	void Reset();

	int GetTextureID(const char* filename) const;
	const TEXTURE_INFO* GetTexture(int id) const;
	const char* GetTextureName(int id) const;

	bool LoadTextureFromFile( const char* filename );

private:
	TextureManager();

private:
	int									m_TextureID;

	typedef std::map<std::string, int> TextureNameMap;
	TextureNameMap	m_TextureNameMap;

	typedef std::map<int, TEXTURE_INFO*>	TextureMap;
	TextureMap	m_Textures;
};

#endif // TextureManager_h__