/********************************************************************
	created:	2011/10/30
	filename: 	GWTextureDX9.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWTextureDX9_h__
#define GWTextureDX9_h__

#include <d3d9.h>

typedef struct TextureInfoType
{
	int					width, height;
	LPDIRECT3DTEXTURE9	d3d_tex;
} TEXTURE_INFO;


#endif // GWTextureDX9_h__