#ifndef TextureManager_h__
#define TextureManager_h__

#include <map>
#include <string>

#include "RenderDevice.h"

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	void Reset();

	int GetTextureID(const char* filename) const;
	LPDIRECT3DTEXTURE9 GetD3DTexture(int id) const;

	bool LoadTextureFromFile( const char* filename );

private:
	int									m_TextureID;

	typedef std::map<std::string, int> TextureNameMap;
	TextureNameMap	m_TextureNameMap;

	typedef std::map<int, LPDIRECT3DTEXTURE9>	TextureMap;
	TextureMap	m_Textures;
};

#endif // TextureManager_h__