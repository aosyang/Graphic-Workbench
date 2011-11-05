/********************************************************************
	created:	2011/10/31
	filename: 	TGA.h
	author:		Mwolf
	
	purpose:	TGA image loader function, from NEHE tutorial
	link:		http://nehe.gamedev.net/tutorial/loading_compressed_and_uncompressed_tga's/22001/
*********************************************************************/
#ifndef TGA_h__
#define TGA_h__

#include "GWTypes.h"
#include "GWImage.h"

#if defined (__cplusplus)
extern "C" {
#endif // #if defined (__cplusplus)

GW_BOOL LoadTGAImage(GW_IMAGE* image, const char* filename);
void UnloadTGAImage(GW_IMAGE* image);

#if defined (__cplusplus)
}
#endif // #if defined (__cplusplus)

#endif // TGA_h__
