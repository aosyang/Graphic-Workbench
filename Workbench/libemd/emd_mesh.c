#include "emd_def.h"
#include "emd_mesh.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void EMD_LoadDataVer100(EMD_MESH* mesh, FILE* fp);

static void EMD_CalculateBoundingBox(EMD_MESH* mesh);

EMD_MESH* EMD_LoadMeshFromFile( const char* filename )
{
	FILE* fp;
	char header[4];
	EMD_MESH* mesh = 0;

	fp = fopen(filename, "rb");
	
	if (!fp)
		return 0;

	mesh = malloc(sizeof(EMD_MESH));
	memset(mesh, 0, sizeof(EMD_MESH));

	// Read header string from file
	fread(header, sizeof(char), 4, fp);

	// Failed if no header found
	if ( strncmp(header, "EMDL", 4) == 0 )
	{
		// Get version of data
		fread(&mesh->ver, sizeof(GW_UINT32), 1, fp);

		// Right now we only deal with ver 1.00
		if ( mesh->ver == 100 )
		{
			EMD_LoadDataVer100(mesh, fp);
		}
	}

	fclose(fp);

	return mesh;
}

void EMD_LoadDataVer100( EMD_MESH* mesh, FILE* fp )
{
	GW_UINT32 lump, size, size_byte;

	mesh->ElementCount = 1;
	mesh->IndexCount = malloc(sizeof(GW_UINT32));
	mesh->IndexArray = malloc(sizeof(GW_UINT32*));

	fread(&lump, sizeof(lump), 1, fp);

	while ( !feof(fp) )
	{
		fread(&size_byte, sizeof(size_byte), 1, fp);

		// size = size_byte / 4
		size = size_byte >> 2;

		switch (lump)
		{
		case LUMP_INDEX:
			mesh->IndexCount[0] = size;
			mesh->IndexArray[0] = malloc(sizeof(GW_UINT32) * size);
			fread(mesh->IndexArray[0], sizeof(GW_UINT32), size, fp);
			break;

		case LUMP_VERTEX_POSITION:
			if ( mesh->VertexCount == 0 )
			{
				mesh->VertexCount = size / 3;
			}
			mesh->VertexArray = malloc(sizeof(float) * size);
			fread(mesh->VertexArray, sizeof(float), size, fp);

			EMD_CalculateBoundingBox(mesh);
			break;

		case LUMP_VERTEX_NORMAL:
			if ( mesh->VertexCount == 0 )
			{
				mesh->VertexCount = size / 3;
			}
			mesh->NormalArray = malloc(sizeof(float) * size);
			fread(mesh->NormalArray, sizeof(float), size, fp);
			break;

		case LUMP_VERTEX_TEXCOORD0:
			if ( mesh->VertexCount == 0 )
			{
				mesh->VertexCount = size / 2;
			}
			mesh->TexcoordArray = malloc(sizeof(float) * size);
			fread(mesh->TexcoordArray, sizeof(float), size, fp);
			break;

		case LUMP_VERTEX_TANGENT:
			if ( mesh->VertexCount == 0 )
			{
				mesh->VertexCount = size / 3;
			}
			mesh->TangentArray = malloc(sizeof(float) * size);
			fread(mesh->TangentArray, sizeof(float), size, fp);
			break;

		case LUMP_VERTEX_BINORMAL:
			if ( mesh->VertexCount == 0 )
			{
				mesh->VertexCount = size / 3;
			}
			mesh->BinormalArray = malloc(sizeof(float) * size);
			fread(mesh->BinormalArray, sizeof(float), size, fp);
			break;

		}
		fread(&lump, sizeof(lump), 1, fp);
	}
}

void EMD_FreeMesh( EMD_MESH* mesh )
{
	int i;

	if (!mesh) return;

	if (mesh->VertexArray) free(mesh->VertexArray);
	if (mesh->NormalArray) free(mesh->NormalArray);
	if (mesh->TexcoordArray) free(mesh->TexcoordArray);

	if (mesh->TangentArray) free(mesh->TangentArray);
	if (mesh->BinormalArray) free(mesh->BinormalArray);

	for (i=0; i<mesh->ElementCount; i++)
	{
		if (mesh->IndexArray[i]) free(mesh->IndexArray[i]);
	}

	if (mesh->IndexArray) free(mesh->IndexArray);
	if (mesh->IndexCount) free(mesh->IndexCount);

	free(mesh);
}

void EMD_CalculateBoundingBox( EMD_MESH* mesh )
{
	int i, j;

	if (!mesh)
		return;

	for (i=0; i<mesh->VertexCount*3; i+=3)
	{
		if (i==0)
		{
			for (j=0; j<3; j++)
			{
				mesh->vMin[j] = mesh->VertexArray[i+j];
				mesh->vMax[j] = mesh->VertexArray[i+j];
			}
		}
		else
		{
			for (j=0; j<3; j++)
			{
				if (mesh->vMin[j] > mesh->VertexArray[i+j])
					mesh->vMin[j] = mesh->VertexArray[i+j];
				if (mesh->vMax[j] < mesh->VertexArray[i+j])
					mesh->vMax[j] = mesh->VertexArray[i+j];
			}
		}
	}
}

