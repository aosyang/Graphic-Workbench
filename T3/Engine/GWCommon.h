/********************************************************************
	created:	2011/10/14
	filename: 	GWCommon.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWCommon_h__
#define GWCommon_h__

#include <string.h>

#ifndef NULL
#define NULL 0
#endif

#ifndef GW_SAFE_DELETE
#define GW_SAFE_DELETE(x)			{ if (x) { delete (x); (x) = NULL; } }
#endif

#ifndef GW_SAFE_DELETE_ARRAY
#define GW_SAFE_DELETE_ARRAY(x)		{ if (x) { delete [] (x); (x) = NULL; } }
#endif

#ifndef GW_MATH_MAX
#define GW_MATH_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef GW_MATH_MIN
#define GW_MATH_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define GW_MATH_CLAMP(v,_min,_max)	GW_MATH_MAX(GW_MATH_MIN(v,_max),_min)

#ifdef _DEBUG
#define GW_BREAKPOINT				{ while(0) {} }
#else
#define GW_BREAKPOINT
#endif

#define GW_MATH_PI				(float)(3.141592654f)

typedef float GWAngle;

#define DEGREE(rad) ((float)(rad) * GW_MATH_PI / 180.f)

#endif // GWCommon_h__