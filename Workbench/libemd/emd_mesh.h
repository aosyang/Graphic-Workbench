#ifndef EMD_MESH_H__
#define EMD_MESH_H__

typedef unsigned char GW_UINT8;
typedef unsigned short GW_UINT16;
typedef unsigned int GW_UINT32;

#ifndef __cplusplus

#if defined (WIN32)
// Win32 need __inline instead of inline when source compiled as C
#define inline __inline
#else
// for gcc, use static inline instead of inline
#define inline static inline
#endif // #if defined (WIN32)

#endif // #ifndef __cplusplus

// EMD mesh data structure
typedef struct EMD_MESH_TYPE
{
	GW_UINT32	ver;
	GW_UINT32	VertexCount;

	// TODO: replace those with a single data ptr
	float*		VertexArray;
	float*		NormalArray;
	float*		TexcoordArray;

	float*		TangentArray;
	float*		BinormalArray;

	GW_UINT32	ElementCount;

	GW_UINT32*	IndexCount;
	GW_UINT32**	IndexArray;

	// Bounding box;
	float		vMin[3];
	float		vMax[3];
} EMD_MESH;

#ifdef __cplusplus
extern "C" {
#endif

EMD_MESH* EMD_LoadMeshFromFile(const char* filename);
void EMD_FreeMesh(EMD_MESH* mesh);


inline GW_UINT32 EMD_GetVertexCount(EMD_MESH* mesh)
{ return mesh->VertexCount; }

inline float* EMD_GetVertexArray(EMD_MESH* mesh)
{ return mesh->VertexArray; }

inline float* EMD_GetNormalArray(EMD_MESH* mesh)
{ return mesh->NormalArray; }

inline float* EMD_GetTexcoordArray(EMD_MESH* mesh)
{ return mesh->TexcoordArray; }


inline void EMD_GetVertexByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->VertexArray[index*3];
	*y = mesh->VertexArray[index*3 + 1];
	*z = mesh->VertexArray[index*3 + 2];
}

inline void EMD_GetNormalByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->NormalArray[index*3];
	*y = mesh->NormalArray[index*3 + 1];
	*z = mesh->NormalArray[index*3 + 2];
}

inline void EMD_GetTexcoordByIndex(EMD_MESH* mesh, GW_UINT32 index, float* u, float* v)
{
	*u = mesh->TexcoordArray[index*2];
	*v = mesh->TexcoordArray[index*2 + 1];
}

inline void EMD_GetTangentByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->TangentArray[index*3];
	*y = mesh->TangentArray[index*3 + 1];
	*z = mesh->TangentArray[index*3 + 2];
}

inline void EMD_GetBinormalByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->BinormalArray[index*3];
	*y = mesh->BinormalArray[index*3 + 1];
	*z = mesh->BinormalArray[index*3 + 2];
}


inline GW_UINT32 EMD_GetElementCount(EMD_MESH* mesh)
{ return mesh->ElementCount; }

inline GW_UINT32 EMD_GetIndexCount(EMD_MESH* mesh, GW_UINT32 elem_index)
{ return mesh->IndexCount[elem_index]; }

inline GW_UINT32* EMD_GetIndexArray(EMD_MESH* mesh, GW_UINT32 elem_index)
{ return mesh->IndexArray[elem_index]; }



inline void EMD_GetBoundingMin(EMD_MESH* mesh, float* x, float* y, float* z)
{ *x = mesh->vMin[0]; *y = mesh->vMin[1]; *z = mesh->vMin[2]; }

inline void EMD_GetBoundingMax(EMD_MESH* mesh, float* x, float* y, float* z)
{ *x = mesh->vMax[0]; *y = mesh->vMax[1]; *z = mesh->vMax[2]; }


#ifdef __cplusplus
}		// extern "C"
#endif

#endif // EMD_MESH_H__
