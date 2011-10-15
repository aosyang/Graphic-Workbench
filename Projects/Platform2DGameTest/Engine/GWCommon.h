#ifndef GWCommon_h__
#define GWCommon_h__

#ifndef GW_SAFE_DELETE
#define GW_SAFE_DELETE(x)			{ if (x) { delete (x); (x) = NULL; } }
#endif

#ifndef GW_SAFE_DELETE_ARRAY
#define GW_SAFE_DELETE_ARRAY(x)		{ if (x) { delete [] (x); (x) = NULL; } }
#endif

#ifdef _DEBUG
#define GW_BREAKPOINT				{ while(0) {} }
#elif
#define GW_BREAKPOINT
#endif

#endif // GWCommon_h__