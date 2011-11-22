/********************************************************************
	created:	2011/10/31
	filename: 	GWImage.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#ifndef GWImage_h__
#define GWImage_h__

typedef enum GWPixelFormat
{
	GW_PIXFMT_UNKNOWN = 0,

	GW_PIXFMT_RGB,
	GW_PIXFMT_RGBA,
} GW_PIXEL_FORMAT;

typedef	struct GWImage
{
	GW_UINT8*		imageData;
	GW_UINT32		bpp;
	GW_UINT32		width;
	GW_UINT32		height;
	GW_PIXEL_FORMAT	format;
} GW_IMAGE;

#endif // GWImage_h__
