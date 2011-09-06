/********************************************************************
	created:	2011/09/06
	filename: 	emd_def.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef EMD_DEF_H__
#define EMD_DEF_H__

// Lump indicates the usage of following data
enum Lump
{
	LUMP_MATERIALS					= 100,
	LUMP_MESH_ELEMENTS,
	LUMP_VERTICES,

	// NEW format

	// Vertex attributes
	LUMP_VERTEX_POSITION			= 200,
	LUMP_VERTEX_NORMAL,

	// Texcoord
	LUMP_VERTEX_TEXCOORD0,
	LUMP_VERTEX_TEXCOORD1,
	LUMP_VERTEX_TEXCOORD2,
	LUMP_VERTEX_TEXCOORD3,
	LUMP_VERTEX_TEXCOORD4,
	LUMP_VERTEX_TEXCOORD5,

	// TODO: More texcoords

	LUMP_VERTEX_TANGENT				= LUMP_VERTEX_TEXCOORD0 + 10,
	LUMP_VERTEX_BINORMAL,

	LUMP_INDEX						= 300,
};

#define EMDL_MAJOR_VERSION 1
#define EMDL_MINOR_VERSION 0
#define EMDL_VERSION (unsigned int)(EMDL_MAJOR_VERSION * 100 + EMDL_MINOR_VERSION)


#endif // EMD_DEF_H__
