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

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifdef _DEBUG
#define GW_BREAKPOINT				{ while(0) {} }
#else
#define GW_BREAKPOINT
#endif

#endif // GWCommon_h__