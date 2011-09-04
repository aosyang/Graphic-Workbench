#include "RootExporter.h"

#include <IGame/IGame.h>

#include "../../libEmdMesh/EmdDefine.h"

RootExporter::RootExporter()
{
	Reset();
}

RootExporter::~RootExporter()
{
}

void RootExporter::AddNode( IGameNode* gameNode )
{
	switch ( gameNode->GetIGameObject()->GetIGameType() )
	{
	case IGameObject::IGAME_MESH:
		// TODO: Save data to mesh hierarchy
		GetMeshData( gameNode );
		break;
	}

	// TODO: We'll deal with mesh hierarchy later
	//int childCount = gameNode->GetChildCount();
	//for ( int i=0; i<childCount; i++ )
	//	AddNode( gameNode->GetNodeChild( i ) );

}

void RootExporter::Reset()
{
	m_PositionArray.clear();
	m_NormalArray.clear();
	m_TexcoordArray.clear();

	m_TangentArray.clear();
	m_BinormalArray.clear();

	m_VertexMap.clear();
	m_IndexArray.clear();
}

bool RootExporter::SaveToFile( const EChar* filename )
{
	std::ofstream fout( filename, std::ios_base::binary );
	if (!fout.is_open()) return false;

	fout.write("EMDL", 4);

	uint32 ver = EMDL_VERSION;
	fout.write( (char*)&ver, sizeof(ver) );

	// export indices
	WriteLump( fout, LUMP_INDEX, m_IndexArray.size() * sizeof(uint32), &m_IndexArray[0] );
	WriteLump( fout, LUMP_VERTEX_POSITION, m_PositionArray.size() * sizeof(float), &m_PositionArray[0] );
	WriteLump( fout, LUMP_VERTEX_NORMAL, m_NormalArray.size() * sizeof(float), &m_NormalArray[0] );
	WriteLump( fout, LUMP_VERTEX_TEXCOORD0, m_TexcoordArray.size() * sizeof(float), &m_TexcoordArray[0] );
	WriteLump( fout, LUMP_VERTEX_TANGENT, m_TangentArray.size() * sizeof(float), &m_TangentArray[0] );
	WriteLump( fout, LUMP_VERTEX_BINORMAL, m_BinormalArray.size() * sizeof(float), &m_BinormalArray[0] );

	fout.close();

	//std::ofstream textOut( L"F:/textOut.txt" );
	//textOut << "Tangent" << std::endl;
	//for (int i=0; i<m_TangentArray.size(); i+=3)
	//{
	//	textOut << m_TangentArray[i] << " " << m_TangentArray[i+1] << " " << m_TangentArray[i+2] << std::endl;
	//}

	//textOut << "Binormal" << std::endl;
	//for (int i=0; i<m_BinormalArray.size(); i+=3)
	//{
	//	textOut << m_BinormalArray[i] << " " << m_BinormalArray[i+1] << " " << m_BinormalArray[i+2] << std::endl;
	//}

	//textOut.close();

	return true;
}

void RootExporter::WriteLump( std::ofstream& fout, uint32 lump, uint32 size_byte, void* data )
{
	fout.write( (char*)&lump, sizeof(lump) );
	fout.write( (char*)&size_byte, sizeof(size_byte) );
	fout.write( (char*)data, size_byte );
}

void RootExporter::GetMeshData( IGameNode* gameNode )
{
	// TODO: for now, only one mesh is stored in the class
	IGameObject* obj = gameNode->GetIGameObject();
	obj->InitializeData();

	IGameMesh* mesh = (IGameMesh*)obj;

	mesh->InitializeBinormalData();

	int faceCount = mesh->GetNumberOfFaces();

	// TODO: Handle materials
	int index = 0;

	// save index data
	for (int faceIndex=0; faceIndex<faceCount; faceIndex++)
	{
		FaceEx* face = mesh->GetFace(faceIndex);

		for (int faceVertexIndex=0; faceVertexIndex<3; faceVertexIndex++)
		{
			VertexData v;

			DWORD vi = face->vert[faceVertexIndex];
			Point3 pos = mesh->GetVertex(vi, true);

			v.x = pos.x;
			v.y = pos.y;
			v.z = pos.z;

			vi = face->norm[faceVertexIndex];
			Point3 normal = mesh->GetNormal(vi, true);

			v.nx = normal.x;
			v.ny = normal.y;
			v.nz = normal.z;

			vi = face->texCoord[faceVertexIndex];
			Point2 texcoord = mesh->GetTexVertex(vi);

			v.u = texcoord.x;
			v.v = texcoord.y;

			vi = mesh->GetFaceVertexTangentBinormal(faceIndex, faceVertexIndex);
			Point3 tangent = mesh->GetTangent(vi);

			v.tx = tangent.x;
			v.ty = tangent.y;
			v.tz = tangent.z;

			Point3 binormal = mesh->GetBinormal(vi);

			v.bx = binormal.x;
			v.by = binormal.y;
			v.bz = binormal.z;

			// remove duplicated vertex with std::map
			if ( m_VertexMap.find(v) != m_VertexMap.end() )
			{
				m_IndexArray.push_back( m_VertexMap[v] );
			}
			else
			{
				m_IndexArray.push_back( index );
				m_VertexMap[v] = index;

				// save vertex to array
				m_PositionArray.push_back( v.x );
				m_PositionArray.push_back( v.y );
				m_PositionArray.push_back( v.z );

				m_NormalArray.push_back( v.nx );
				m_NormalArray.push_back( v.ny );
				m_NormalArray.push_back( v.nz );

				m_TexcoordArray.push_back( v.u );
				m_TexcoordArray.push_back( v.v );

				m_TangentArray.push_back( v.tx );
				m_TangentArray.push_back( v.ty );
				m_TangentArray.push_back( v.tz );

				m_BinormalArray.push_back( v.bx );
				m_BinormalArray.push_back( v.by );
				m_BinormalArray.push_back( v.bz );

				index++;
			}
		}
	}

	int b = 0;
}
