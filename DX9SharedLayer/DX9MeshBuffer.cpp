#include "DX9MeshBuffer.h"
#include "DX9Device.h"
#include "../libEmdMesh/EmdMesh.h"

#include <d3dx9.h>

template<typename T> inline void Swap(T& a, T& b) { T t = a; a = b; b = t; }

DX9MeshBuffer::DX9MeshBuffer()
: m_pIBuffer(NULL), m_pDecl(NULL),
  m_VertexCount(0), m_IndexCount(0)
{
	for (int i=0; i<ST_MAX_VB_STREAMS; i++)
	{
		m_pVBuffer[i] = NULL;
	}
}

DX9MeshBuffer::~DX9MeshBuffer()
{
	Destroy();
}

HRESULT DX9MeshBuffer::CreateFromMesh( EmdMesh* mesh )
{
	HRESULT hr;

	if ( !mesh ) return E_FAIL;

	m_VertexCount = mesh->GetVertexCount();
	m_IndexCount = 0;

	for (uint32 i=0; i<mesh->GetElementCount(); i++ )
	{
		m_IndexCount += mesh->GetIndexCount(i);
	}

	D3DXVECTOR3* vertPosition = new D3DXVECTOR3[m_VertexCount];
	D3DXVECTOR3* vertNormal = new D3DXVECTOR3[m_VertexCount];
	D3DXVECTOR2* vertTextureUV = new D3DXVECTOR2[m_VertexCount];
	D3DXVECTOR3* vertTangent = new D3DXVECTOR3[m_VertexCount];
	D3DXVECTOR3* vertBinormal = new D3DXVECTOR3[m_VertexCount];
	for (uint32 i=0; i<m_VertexCount; i++)
	{
		mesh->GetVertex( i, vertPosition[i].x, vertPosition[i].y, vertPosition[i].z );
		mesh->GetNormal( i, vertNormal[i].x, vertNormal[i].y, vertNormal[i].z );
		mesh->GetTexcoord( i, vertTextureUV[i].x, vertTextureUV[i].y );
		mesh->GetTangent( i, vertTangent[i].x, vertTangent[i].y, vertTangent[i].z );
		mesh->GetBinormal( i, vertBinormal[i].x, vertBinormal[i].y, vertBinormal[i].z );

		// flip x axis
		vertPosition[i].z *= -1;
		vertNormal[i].z *= -1;

		//vertTextureUV[i].y = 1.0f - vertTextureUV[i].y;

		// Also flip tangent and binormal
		vertTangent[i].z *= -1;
		//vertBinormal[i] *= -1;
		vertBinormal[i].z *= -1;
	}

	uint32* indices = new uint32[m_IndexCount];
	uint32 indexHead = 0;
	for (uint32 i=0; i<mesh->GetElementCount(); i++ )
	{
		memcpy( &indices[indexHead], mesh->GetIndexArray(i), sizeof(uint32) * mesh->GetIndexCount(i) );
		indexHead += mesh->GetIndexCount(i);
	}

	// flip triangles
	for (uint32 i=0; i<indexHead; i+=3)
	{
		Swap(indices[i+1], indices[i+2]);
	}

	// Create and fill vertex buffer
	VOID* pData;

	// vertex buffer
	V_RETURN( D3DDevice()->CreateVertexBuffer( m_VertexCount * sizeof( D3DXVECTOR3 ), D3DUSAGE_WRITEONLY,
		NULL, D3DPOOL_DEFAULT, &m_pVBuffer[ST_VERTEX_POSITION], NULL ) );

	V_RETURN( m_pVBuffer[ST_VERTEX_POSITION]->Lock( 0, sizeof( D3DXVECTOR3 ) * m_VertexCount, (void**)&pData, 0 ) );
	memcpy( pData, vertPosition, sizeof( D3DXVECTOR3 ) * m_VertexCount );
	m_pVBuffer[ST_VERTEX_POSITION]->Unlock();

	// normal buffer
	V_RETURN( D3DDevice()->CreateVertexBuffer( m_VertexCount * sizeof( D3DXVECTOR3 ), D3DUSAGE_WRITEONLY,
		NULL, D3DPOOL_DEFAULT, &m_pVBuffer[ST_VERTEX_NORMAL], NULL ) );

	V_RETURN( m_pVBuffer[ST_VERTEX_NORMAL]->Lock( 0, sizeof( D3DXVECTOR3 ) * m_VertexCount, (void**)&pData, 0 ) );
	memcpy( pData, vertNormal, sizeof( D3DXVECTOR3 ) * m_VertexCount );
	m_pVBuffer[ST_VERTEX_NORMAL]->Unlock();

	// texcoord buffer
	V_RETURN( D3DDevice()->CreateVertexBuffer( m_VertexCount * sizeof( D3DXVECTOR2 ), D3DUSAGE_WRITEONLY,
		NULL, D3DPOOL_DEFAULT, &m_pVBuffer[ST_VERTEX_TEXTUREUV], NULL ) );

	V_RETURN( m_pVBuffer[ST_VERTEX_TEXTUREUV]->Lock( 0, sizeof( D3DXVECTOR2 ) * m_VertexCount, (void**)&pData, 0 ) );
	memcpy( pData, vertTextureUV, sizeof( D3DXVECTOR2 ) * m_VertexCount );
	m_pVBuffer[ST_VERTEX_TEXTUREUV]->Unlock();

	// tangent buffer
	V_RETURN( D3DDevice()->CreateVertexBuffer( m_VertexCount * sizeof( D3DXVECTOR3 ), D3DUSAGE_WRITEONLY,
		NULL, D3DPOOL_DEFAULT, &m_pVBuffer[ST_VERTEX_TANGENT], NULL ) );

	V_RETURN( m_pVBuffer[ST_VERTEX_TANGENT]->Lock( 0, sizeof( D3DXVECTOR3 ) * m_VertexCount, (void**)&pData, 0 ) );
	memcpy( pData, vertTangent, sizeof( D3DXVECTOR3 ) * m_VertexCount );
	m_pVBuffer[ST_VERTEX_TANGENT]->Unlock();

	// binormal buffer
	V_RETURN( D3DDevice()->CreateVertexBuffer( m_VertexCount * sizeof( D3DXVECTOR3 ), D3DUSAGE_WRITEONLY,
		NULL, D3DPOOL_DEFAULT, &m_pVBuffer[ST_VERTEX_BINORMAL], NULL ) );

	V_RETURN( m_pVBuffer[ST_VERTEX_BINORMAL]->Lock( 0, sizeof( D3DXVECTOR3 ) * m_VertexCount, (void**)&pData, 0 ) );
	memcpy( pData, vertBinormal, sizeof( D3DXVECTOR3 ) * m_VertexCount );
	m_pVBuffer[ST_VERTEX_BINORMAL]->Unlock();

	SAFE_DELETE_ARRAY( vertPosition );
	SAFE_DELETE_ARRAY( vertNormal );
	SAFE_DELETE_ARRAY( vertTextureUV );
	SAFE_DELETE_ARRAY( vertTangent );
	SAFE_DELETE_ARRAY( vertBinormal );

	// Create vertex declaration
	D3DVERTEXELEMENT9 declDesc[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 3, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 4, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 }
	};
	D3DDevice()->CreateVertexDeclaration( declDesc, &m_pDecl );

	// Create index buffer
	V_RETURN( D3DDevice()->CreateIndexBuffer( m_IndexCount * sizeof( uint32 ), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIBuffer, NULL) );


	// fill the index buffer
	VOID* pIndices;

	V_RETURN( m_pIBuffer->Lock( 0, m_IndexCount * sizeof( uint32 ), (void**)&pIndices, 0 ) );
	memcpy( pIndices, indices, m_IndexCount * sizeof( uint32 ) );
	m_pIBuffer->Unlock();

	SAFE_DELETE_ARRAY( indices );

	return S_OK;
}

void DX9MeshBuffer::Destroy()
{
	for (int i=0; i<ST_MAX_VB_STREAMS; i++)
	{
		SAFE_RELEASE(m_pVBuffer[i]);
	}
	SAFE_RELEASE(m_pIBuffer);
	SAFE_RELEASE(m_pDecl);

	m_VertexCount = 0;
	m_IndexCount = 0;
}

void DX9MeshBuffer::Render()
{
	D3DDevice()->SetStreamSource( 0, m_pVBuffer[ST_VERTEX_POSITION], 0, sizeof( D3DXVECTOR3 ) );
	D3DDevice()->SetStreamSource( 1, m_pVBuffer[ST_VERTEX_NORMAL], 0, sizeof( D3DXVECTOR3 ) );
	D3DDevice()->SetStreamSource( 2, m_pVBuffer[ST_VERTEX_TEXTUREUV], 0, sizeof( D3DXVECTOR2 ) );
	D3DDevice()->SetStreamSource( 3, m_pVBuffer[ST_VERTEX_TANGENT], 0, sizeof( D3DXVECTOR3 ) );
	D3DDevice()->SetStreamSource( 4, m_pVBuffer[ST_VERTEX_BINORMAL], 0, sizeof( D3DXVECTOR3 ) );
	D3DDevice()->SetVertexDeclaration( m_pDecl );
	D3DDevice()->SetIndices( m_pIBuffer );
	D3DDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_VertexCount, 0, m_IndexCount / 3 );
}
