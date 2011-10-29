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

#include "RenderDevice.h"
#include <d3d9.h>

#include "GWSingleton.h"

typedef struct TextureInfoType
{
	int					width, height;
	LPDIRECT3DTEXTURE9	d3d_tex;
} TEXTURE_INFO;

class TextureManager : public GWSingleton<TextureManager>
{
	friend class GWSingleton<TextureManager>;
public:

	void Reset();

	int GetTextureID(const char* filename) const;
	LPDIRECT3DTEXTURE9 GetD3DTexture(int id) const;
	const char* GetTextureName(int id) const;

	bool LoadTextureFromFile( const char* filename );

private:
	TextureManager();

private:
	int									m_TextureID;

	typedef std::map<std::string, int> TextureNameMap;
	TextureNameMap	m_TextureNameMap;

	typedef std::map<int, TextureInfoType>	TextureMap;
	TextureMap	m_Textures;
};

#endif // TextureManager_h__