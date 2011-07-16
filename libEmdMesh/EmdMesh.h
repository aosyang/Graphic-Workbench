#ifndef EmdMesh_h__
#define EmdMesh_h__

#include <string>
#include <vector>

#include "EmdDefine.h"
#include "EmdTypes.h"

// EmdMesh holds data loaded from .emd file
class EmdMesh
{
public:

	EmdMesh();
	~EmdMesh();

	// Load mesh from file
	bool LoadMesh( const EChar* filename );

	// Remove all data
	void Reset();

	float* GetVertexArray() const { return m_VertexArray; }
	float* GetNormalArray() const { return m_NormalArray; }
	float* GetTexcoordArray() const { return m_TexcoordArray; }

	bool GetVertex(uint32 index, float& x, float& y, float &z) const;
	bool GetNormal(uint32 index, float& x, float& y, float &z) const;
	bool GetTexcoord(uint32 index, float& u, float& v) const;
	bool GetTangent(uint32 index, float& x, float& y, float& z) const;
	bool GetBinormal(uint32 index, float& x, float& y, float& z) const;

	uint32 GetVertexCount() const { return m_VertexNum; }

	uint32 GetElementCount() const { return m_ElementCount; }
	uint32 GetIndexCount(uint32 elementIndex) const { return m_IndexCount[elementIndex]; }
	uint32* GetIndexArray(uint32 elementIndex) const { return m_IndexArray[elementIndex]; }

protected:
	std::vector<std::string>	m_MaterialNames;

	uint32		m_VertexNum;

	// TODO: replace those with a single data ptr
	float*		m_VertexArray;
	float*		m_NormalArray;
	float*		m_TexcoordArray;

	float*		m_TangentArray;
	float*		m_BinormalArray;

	uint32		m_ElementCount;

	uint32*		m_IndexCount;
	uint32**	m_IndexArray;

private:
	// Load emd mesh version 0.20
	bool LoadMeshVer020( std::ifstream& fin );

	// Load emd mesh version 1.00
	bool LoadMeshVer100( std::ifstream& fin );
};

#endif // EmdMesh_h__
