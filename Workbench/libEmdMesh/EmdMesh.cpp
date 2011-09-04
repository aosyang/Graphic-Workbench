#include "EmdMesh.h"

#include <fstream>
#include <iostream>

EmdMesh::EmdMesh()
: m_VertexNum(0),
  m_VertexArray(NULL),
  m_NormalArray(NULL),
  m_TexcoordArray(NULL),
  m_TangentArray(NULL),
  m_BinormalArray(NULL),
  m_ElementCount(0),
  m_IndexCount(NULL),
  m_IndexArray(NULL)
{
	Reset();
}

EmdMesh::~EmdMesh()
{
	Reset();
}

bool EmdMesh::LoadMesh( const EChar* filename )
{

	// TODO: 文件读取到内存中，通过指针访问
	std::ifstream fin;
	fin.open(filename, std::ios_base::in|std::ios_base::binary);

	if (!fin.is_open())
		return false;

	bool result = true;
	char header[4];
	fin.read((char*)header, 4);

	std::cout << "Loading mesh from " << filename << "\n";

	if (strncmp(header, "EMDL", 4)==0)		// No header, failed
	{
		unsigned int ver;
		fin.read((char*)&ver, sizeof(ver));

		// 版本号检查
		// 10代表0.10版，以此类推
		switch (ver)
		{
		case 20:
			std::cout << "EMD mesh version 0.20.\n";
			if (!LoadMeshVer020(fin)) result = false;
			break;
		case 100:
			if (!LoadMeshVer100(fin)) result = false;
			break;
		default:
			// unsupported version
			std::cout << "EmdMesh::LoadFromFile() : Unsupported mesh version." << std::endl;
			result = false;
			break;
		}
	}
	else
	{
		std::cout << "EmdMesh::LoadFromFile(): No header found" << std::endl;

		result = false;
	}

	fin.close();
	return result;
}

bool EmdMesh::LoadMeshVer020( std::ifstream& fin )
{
	unsigned int materialCount = 1;	// 注：对于没有材质的模型，默认子模型数量为1
	unsigned int lump;
	fin.read((char*)&lump, sizeof(lump));

	while (!fin.eof())
	{
		switch (lump)
		{
		case LUMP_MATERIALS:		// Load materials
			{
				fin.read((char*)&materialCount, sizeof(materialCount));

				for (unsigned int i=0; i<materialCount; i++)
				{
					char matName[256];
					unsigned int len;
					fin.read((char*)&len, sizeof(len));

					fin.read(matName, sizeof(char) * len);
					matName[len] = 0;

					// store material names
					m_MaterialNames.push_back(matName);
				}

				break;
			}
		case LUMP_MESH_ELEMENTS:	// Load mesh elements
			{
				// Load mesh parts
				// TODO: Material count should be replace with element count

				// Allocate arrays for indices
				m_IndexCount = new uint32[materialCount];
				m_IndexArray = new uint32*[materialCount];
				memset(m_IndexArray, NULL, sizeof(uint32*) * materialCount);

				for (unsigned int i=0; i<materialCount; i++)
				{
					int faceCount;
					fin.read((char*)&faceCount, sizeof(faceCount));

					unsigned int* indexArray = new unsigned int[faceCount*3];

					fin.read((char*)indexArray, sizeof(unsigned int) * faceCount * 3);

					m_IndexCount[i] = faceCount * 3;
					m_IndexArray[i] = new uint32[faceCount * 3];
					memcpy(m_IndexArray[i], indexArray, sizeof(uint32) * faceCount * 3);

					SAFE_DELETE_ARRAY(indexArray);
				}
				break;
			}
		case LUMP_VERTICES:			// Load vertices shared by mesh elements
			{
				fin.read((char*)&m_VertexNum, sizeof(m_VertexNum));

				m_VertexArray = new float[m_VertexNum * 3];
				m_NormalArray = new float[m_VertexNum * 3];
				m_TexcoordArray = new float[m_VertexNum * 2];

				float* data = new float[m_VertexNum * 8];
				fin.read((char*)data, sizeof(float) * m_VertexNum * 8);

				int offset = 0;
				float maxSquaredLength = 0.0f;
				for (uint32 i=0; i<m_VertexNum; i++)
				{

					memcpy(&m_VertexArray[i * 3], &data[offset], sizeof(float) * 3);
					offset += 3;
					memcpy(&m_NormalArray[i * 3], &data[offset], sizeof(float) * 3);
					offset += 3;
					memcpy(&m_TexcoordArray[i * 2], &data[offset], sizeof(float) * 2);
					offset += 2;

					//float squaredLen = Vector3f(&m_VertexArray[i * 3]).SquaredLength();
					//if (squaredLen>maxSquaredLength)
					//	maxSquaredLength = squaredLen;
				}
				//m_BoundingRadius = sqrt(maxSquaredLength);

				SAFE_DELETE_ARRAY(data);
			}
			break;
		default:
			std::cout << "EmdMesh::LoadMesh_020(): Invalid lump from file." << std::endl;
			return false;
			break;
		}

		fin.read((char*)&lump, sizeof(lump));
	}

	//UpdateElementsOBB();

	m_ElementCount = materialCount;

	return true;
}

bool EmdMesh::LoadMeshVer100( std::ifstream& fin )
{
	uint32 lump, size_byte;
	fin.read( (char*)&lump, sizeof(lump) );

	m_ElementCount = 1;
	m_IndexCount = new uint32[1];
	m_IndexArray = new uint32*[1];

	while (!fin.eof())
	{
		fin.read( (char*)&size_byte, sizeof(size_byte) );

		switch (lump)
		{
		case LUMP_INDEX:
			m_IndexCount[0] = size_byte >> 2;
			m_IndexArray[0] = new uint32[m_IndexCount[0]];
			fin.read( (char*)m_IndexArray[0], size_byte );
			break;

		case LUMP_VERTEX_POSITION:
			if ( !m_VertexNum ) m_VertexNum = (size_byte >> 2) / 3;
			m_VertexArray = new float[m_VertexNum * 3];
			fin.read( (char*)m_VertexArray, size_byte );

			// calculate bounding box
			for (uint32 i=0; i<m_VertexNum*3; i+=3)
			{
				for (uint32 j=0; j<3; j++)
				{
					if (m_vMin[j]>m_VertexArray[i+j]) m_vMin[j] = m_VertexArray[i+j];
					if (m_vMax[j]<m_VertexArray[i+j]) m_vMax[j] = m_VertexArray[i+j];
				}
			}

			break;

		case LUMP_VERTEX_NORMAL:
			if ( !m_VertexNum ) m_VertexNum = (size_byte >> 2) / 3;
			m_NormalArray = new float[m_VertexNum * 3];
			fin.read( (char*)m_NormalArray, size_byte );
			break;

		case LUMP_VERTEX_TEXCOORD0:
			if ( !m_VertexNum ) m_VertexNum = (size_byte >> 2) / 2;
			m_TexcoordArray = new float[m_VertexNum * 2];
			fin.read( (char*)m_TexcoordArray, size_byte );
			break;

		case LUMP_VERTEX_TANGENT:
			if ( !m_VertexNum ) m_VertexNum = (size_byte >> 2) / 3;
			m_TangentArray = new float[m_VertexNum * 3];
			fin.read( (char*)m_TangentArray, size_byte );
			break;

		case LUMP_VERTEX_BINORMAL:
			if ( !m_VertexNum ) m_VertexNum = (size_byte >> 2) / 3;
			m_BinormalArray = new float[m_VertexNum * 3];
			fin.read( (char*)m_BinormalArray, size_byte );
			break;
		}
		fin.read( (char*)&lump, sizeof(lump) );
	}

	return true;
}

void EmdMesh::Reset()
{
	m_MaterialNames.clear();

	m_VertexNum = 0;
	SAFE_DELETE_ARRAY(m_VertexArray);
	SAFE_DELETE_ARRAY(m_NormalArray);
	SAFE_DELETE_ARRAY(m_TexcoordArray);

	SAFE_DELETE_ARRAY(m_TangentArray);
	SAFE_DELETE_ARRAY(m_BinormalArray);

	for (uint32 i=0; i<m_ElementCount; i++)
	{
		SAFE_DELETE_ARRAY(m_IndexArray[i]);
	}

	SAFE_DELETE_ARRAY(m_IndexArray);
	SAFE_DELETE_ARRAY(m_IndexCount);
	m_ElementCount = 0;

	for (int i=0; i<3; i++)
	{
		m_vMin[i] = 9999999.9f;
		m_vMax[i] = -9999999.9f;
	}
}

bool EmdMesh::GetVertex( uint32 index, float& x, float& y, float &z ) const
{
	if ( index >= m_VertexNum || !m_VertexArray ) return false;
	x = m_VertexArray[index*3];
	y = m_VertexArray[index*3 + 1];
	z = m_VertexArray[index*3 + 2];
	
	return true;
}

bool EmdMesh::GetNormal( uint32 index, float& x, float& y, float &z ) const
{
	if ( index >= m_VertexNum || !m_NormalArray ) return false;
	x = m_NormalArray[index*3];
	y = m_NormalArray[index*3 + 1];
	z = m_NormalArray[index*3 + 2];

	return true;
}

bool EmdMesh::GetTexcoord( uint32 index, float& u, float& v ) const
{
	if ( index >= m_VertexNum || !m_TexcoordArray ) return false;
	u = m_TexcoordArray[index*2];
	v = m_TexcoordArray[index*2 + 1];

	return true;
}

bool EmdMesh::GetTangent( uint32 index, float& x, float& y, float& z ) const
{
	if ( index >= m_VertexNum || !m_TangentArray ) return false;
	x = m_TangentArray[index*3];
	y = m_TangentArray[index*3 + 1];
	z = m_TangentArray[index*3 + 2];

	return true;
}

bool EmdMesh::GetBinormal( uint32 index, float& x, float& y, float& z ) const
{
	if ( index >= m_VertexNum || !m_BinormalArray ) return false;
	x = m_BinormalArray[index*3];
	y = m_BinormalArray[index*3 + 1];
	z = m_BinormalArray[index*3 + 2];

	return true;
}

Vector3 EmdMesh::GetBoundingMin() const
{
	return Vector3(m_vMin);
}

Vector3 EmdMesh::GetBoundingMax() const
{
	return Vector3(m_vMax);
}
