/*
 *  GW_Types.h
 *  iOS_GLESTest
 *
 *  Created by Mwolf on 11-9-11.
 *  Copyright 2011. All rights reserved.
 *
 */

#ifndef GW_TYPES_H_
#define GW_TYPES_H_


typedef unsigned char GW_UINT8;
typedef unsigned short GW_UINT16;
typedef unsigned int GW_UINT32;

#ifndef __cplusplus

#if defined (WIN32)
// Win32 need __inline instead of inline when source compiled as C
#define GW_INLINE __inline
#else
// for gcc, use static inline instead of inline
#define GW_INLINE static inline
#endif // #if defined (WIN32)

#endif // #ifndef __cplusplus


#endif	// #ifndef GW_TYPES_H_
