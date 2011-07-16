/********************************************************************
	created:	2011/05/01
	filename: 	EmdDefine.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef EmdDefine_h__
#define EmdDefine_h__

// Lump indicates the usage of following data
enum Lump
{
	LUMP_MATERIALS = 100,
	LUMP_MESH_ELEMENTS,
	LUMP_VERTICES,

	// NEW format

	// Vertex attributes
	LUMP_VERTEX_POSITION,
	LUMP_VERTEX_NORMAL,

	// Texcoord
	LUMP_VERTEX_TEXCOORD0,
	LUMP_VERTEX_TEXCOORD1,
	LUMP_VERTEX_TEXCOORD2,
	LUMP_VERTEX_TEXCOORD3,
	LUMP_VERTEX_TEXCOORD4,
	LUMP_VERTEX_TEXCOORD5,

	// TODO: More texcoords

	LUMP_VERTEX_TANGENT,
	LUMP_VERTEX_BINORMAL,

	LUMP_INDEX,
};

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define EMDL_VERSION (uint32)(MAJOR_VERSION * 100 + MINOR_VERSION)

#endif // EmdDefine_h__