/********************************************************************
	created:	2011/10/31
	filename: 	GWColor.h
	author:		Mwolf

	purpose:	
*********************************************************************/
#ifndef GWColor_h__
#define GWColor_h__

class GWColor
{
public:
	float r, g, b, a;

	GWColor() {}
	GWColor(float _r, float _g, float _b, float _a=1.0f) : r(_r), g(_g), b(_b), a(_a) {}

	static GWColor WHITE;
	static GWColor BLACK;
	static GWColor RED;
	static GWColor GREEN;
	static GWColor BLUE;
	static GWColor YELLOW;
};

#endif // GWColor_h__