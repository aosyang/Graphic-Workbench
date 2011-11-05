/********************************************************************
	created:	2011/10/31
	filename: 	GWTypes.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#ifndef GWTypes_h__
#define GWTypes_h__

typedef	unsigned char		GW_UINT8;
typedef	unsigned short		GW_UINT16;
typedef	unsigned int		GW_UINT32;

#if defined (__cplusplus)
typedef bool			GW_BOOL;
#else
#	if (__STDC_VERSION__ >= 199901L)	// C99
typedef _Bool			GW_BOOL;
#	else
typedef GW_UINT8		GW_BOOL;
#	endif	// #if (__STDC_VERSION__ >= 199901L)
#endif // #if !defined (__cplusplus)

enum GWBoolValue
{
	GW_FALSE = 0,
	GW_TRUE,
};

#if defined (__cplusplus)
#	define GW_INLINE inline
#else
#	if defined (WIN32)
		// Win32 need __inline instead of inline when source compiled as C
#		define GW_INLINE __inline
#	else
		// for gcc, use static inline instead of inline
#		define GW_INLINE static inline
#	endif // #if defined (WIN32)
#endif // #if defined (__cplusplus)

#endif // GWTypes_h__
