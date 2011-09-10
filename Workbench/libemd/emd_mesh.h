#ifndef EMD_MESH_H__
#define EMD_MESH_H__

#include "../Common/GW_Types.h"

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


GW_INLINE GW_UINT32 EMD_GetVertexCount(EMD_MESH* mesh)
{ return mesh->VertexCount; }

GW_INLINE float* EMD_GetVertexArray(EMD_MESH* mesh)
{ return mesh->VertexArray; }

GW_INLINE float* EMD_GetNormalArray(EMD_MESH* mesh)
{ return mesh->NormalArray; }

GW_INLINE float* EMD_GetTexcoordArray(EMD_MESH* mesh)
{ return mesh->TexcoordArray; }


GW_INLINE void EMD_GetVertexByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->VertexArray[index*3];
	*y = mesh->VertexArray[index*3 + 1];
	*z = mesh->VertexArray[index*3 + 2];
}

GW_INLINE void EMD_GetNormalByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->NormalArray[index*3];
	*y = mesh->NormalArray[index*3 + 1];
	*z = mesh->NormalArray[index*3 + 2];
}

GW_INLINE void EMD_GetTexcoordByIndex(EMD_MESH* mesh, GW_UINT32 index, float* u, float* v)
{
	*u = mesh->TexcoordArray[index*2];
	*v = mesh->TexcoordArray[index*2 + 1];
}

GW_INLINE void EMD_GetTangentByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->TangentArray[index*3];
	*y = mesh->TangentArray[index*3 + 1];
	*z = mesh->TangentArray[index*3 + 2];
}

GW_INLINE void EMD_GetBinormalByIndex(EMD_MESH* mesh, GW_UINT32 index, float* x, float* y, float *z)
{
	*x = mesh->BinormalArray[index*3];
	*y = mesh->BinormalArray[index*3 + 1];
	*z = mesh->BinormalArray[index*3 + 2];
}


GW_INLINE GW_UINT32 EMD_GetElementCount(EMD_MESH* mesh)
{ return mesh->ElementCount; }

GW_INLINE GW_UINT32 EMD_GetIndexCount(EMD_MESH* mesh, GW_UINT32 elem_index)
{ return mesh->IndexCount[elem_index]; }

GW_INLINE GW_UINT32* EMD_GetIndexArray(EMD_MESH* mesh, GW_UINT32 elem_index)
{ return mesh->IndexArray[elem_index]; }



GW_INLINE void EMD_GetBoundingMin(EMD_MESH* mesh, float* x, float* y, float* z)
{ *x = mesh->vMin[0]; *y = mesh->vMin[1]; *z = mesh->vMin[2]; }

GW_INLINE void EMD_GetBoundingMax(EMD_MESH* mesh, float* x, float* y, float* z)
{ *x = mesh->vMax[0]; *y = mesh->vMax[1]; *z = mesh->vMax[2]; }


#ifdef __cplusplus
}		// extern "C"
#endif

#endif // EMD_MESH_H__
