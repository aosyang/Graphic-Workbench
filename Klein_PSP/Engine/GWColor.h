/********************************************************************
	created:	2011/10/31
	filename: 	GWColor.h
	author:		Mwolf

	purpose:	
*********************************************************************/
#ifndef GWColor_h__
#define GWColor_h__

#include "GWTypes.h"

#define GW_COLOR_ARGB(a, r, g, b) \
	((GW_UINT32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define GW_COLOR_VALUE(r, g, b, a) \
	GW_COLOR_ARGB((GW_UINT32)((a)*255.f),(GW_UINT32)((r)*255.f),(GW_UINT32)((g)*255.f),(GW_UINT32)((b)*255.f))

class GWColor
{
public:
	float r, g, b, a;

	GWColor() {}
	GWColor(float _r, float _g, float _b, float _a=1.0f) : r(_r), g(_g), b(_b), a(_a) {}

	GW_INLINE GW_UINT32 ARGB() const { return GW_COLOR_VALUE(r, g, b, a); }
	GW_INLINE GW_UINT32 ABGR() const { return GW_COLOR_VALUE(b, g, r, a); }

	static GWColor WHITE;
	static GWColor BLACK;
	static GWColor RED;
	static GWColor GREEN;
	static GWColor BLUE;
	static GWColor YELLOW;
};

#endif // GWColor_h__