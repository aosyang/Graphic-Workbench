/********************************************************************
	created:	2011/05/01
	filename: 	RootExporter.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef RootExporter_h__
#define RootExporter_h__

#include "../../libEmdMesh/EmdTypes.h"

#include <fstream>
#include <vector>
#include <map>

class IGameNode;

class RootExporter
{
public:
	RootExporter();
	~RootExporter();

	// add node to mesh hierarchy
	void AddNode( IGameNode* gameNode );

	// clear data
	void Reset();

	// TODO: Implement this
	bool SaveToFile( const EChar* filename );
private:
	
	void WriteLump( std::ofstream& fout, uint32 lump, uint32 size_byte, void* data );

	void GetMeshData( IGameNode* gameNode );

	struct VertexData
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		float tx, ty, tz;
		float bx, by, bz;

		bool operator< ( const VertexData& rhs ) const
		{
			return std::lexicographical_compare(&x, &bz, &rhs.x, &rhs.bz);
		}
	};

	std::vector<float>				m_PositionArray;
	std::vector<float>				m_NormalArray;
	std::vector<float>				m_TexcoordArray;

	std::vector<float>				m_TangentArray;
	std::vector<float>				m_BinormalArray;

	std::map<VertexData, uint32>	m_VertexMap;
	std::vector<uint32>				m_IndexArray;
};

#endif // RootExporter_h__
