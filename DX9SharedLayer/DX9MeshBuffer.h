#ifndef DX9MeshBuffer_h__
#define DX9MeshBuffer_h__

#include <d3d9.h>
#include "../GraphicWorkbench/GWTypes.h"

#include "DllExport.h"

class EmdMesh;

enum STREAM_TYPE
{
	ST_VERTEX_POSITION = 0,
	ST_VERTEX_NORMAL,
	ST_VERTEX_TEXTUREUV,
	ST_VERTEX_TANGENT,
	ST_VERTEX_BINORMAL,
	ST_MAX_VB_STREAMS,
};

class DLL_EXPORT_CLASS DX9MeshBuffer
{
public:
	DX9MeshBuffer();
	~DX9MeshBuffer();

	HRESULT CreateFromFile(GWChar* filename);
	HRESULT CreateFromMesh(EmdMesh* mesh);
	void Destroy();

	void Render();

protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVBuffer[ST_MAX_VB_STREAMS];
	LPDIRECT3DINDEXBUFFER9			m_pIBuffer;
	IDirect3DVertexDeclaration9*	m_pDecl;

	uint32							m_VertexCount;
	uint32							m_IndexCount;
};

#endif // DX9MeshBuffer_h__
