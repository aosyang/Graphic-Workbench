#include "GameStage.h"

#include "../DXUT/DXUT.h"
#include <string>

#include "../LuaPlus/LuaPlus.h"

using namespace LuaPlus;

static StageGeom* StageGeomHead = NULL;
static StageGeom* StageGeomTail = NULL;

// Stage geometry methods
StageGeom* CreateStageGeom()
{
	StageGeom* geom = new StageGeom();
	geom->next = NULL;

	if (StageGeomTail == NULL)
	{
		// Build new chain list
		StageGeomHead = StageGeomTail = geom;
	}
	else
	{
		// Add new geometry to the tail
		StageGeomTail->next = geom;
		StageGeomTail = geom;
	}

	return geom;
}

StageGeom* GetFirstStageGeom()
{
	return StageGeomHead;
}

StageGeom* GetNextStageGeom(StageGeom* geom)
{
	return geom->next;
}


GameStage::GameStage()
: m_GeomCount(0)
{

}

GameStage::~GameStage()
{
	Reset();
}

bool GameStage::LoadFromFile( const char* filename )
{
	bool result = false;
	LuaStateOwner state;
	int err_code = state->DoFile(filename);

	if (!err_code)
	{
		LuaObject stage_script = state->GetGlobals()["Stage"];
		//int layers = stage["Layers"].GetInteger();
		LuaObject geom_script = stage_script["Geometries"];
		m_GeomCount = geom_script.GetTableCount();
		for (int i=0; i<m_GeomCount; i++)
		{
			StageGeom* geom = CreateStageGeom();
			BoundBox box;

			int layer = geom_script[i+1][1].GetInteger();
			box.xMin = geom_script[i+1][2].GetInteger();
			box.xMax = geom_script[i+1][3].GetInteger();
			box.yMin = geom_script[i+1][4].GetInteger();
			box.yMax = geom_script[i+1][5].GetInteger();
			const char* tex_name = geom_script[i+1][6].GetString();

			// build vertex buffer with vertex position as its own texture coordinate
			StageGeomVertex v[6] =
			{
				{ box.xMin, box.yMin, 0.0f, box.xMin, box.yMin },
				{ box.xMin, box.yMax, 0.0f, box.xMin, box.yMax },
				{ box.xMax, box.yMax, 0.0f, box.xMax, box.yMax },

				{ box.xMax, box.yMax, 0.0f, box.xMax, box.yMax },
				{ box.xMax, box.yMin, 0.0f, box.xMax, box.yMin },
				{ box.xMin, box.yMin, 0.0f, box.xMin, box.yMin },
			};

			// Find texture id by name
			int texID = -1;
			if ( (texID=m_TextureMgr.GetTextureID(tex_name)) == -1)
			{
				if (m_TextureMgr.LoadTextureFromFile(tex_name))
					texID = m_TextureMgr.GetTextureID(tex_name);
			}

			geom->bound = box;
			geom->textureID = texID;

			RenderSystem::Device()->CreateVertexBuffer(sizeof(StageGeomVertex) * 6, D3DUSAGE_WRITEONLY,
				D3DFVF_XYZ|D3DFVF_TEX1, D3DPOOL_DEFAULT, &geom->vbuffer, NULL);

			void* pData;
			geom->vbuffer->Lock(0, sizeof(StageGeomVertex) * 6, (void**)&pData, 0);
			memcpy(pData, v, sizeof(StageGeomVertex) * 6);
			geom->vbuffer->Unlock();
		}

		result = true;
	}

	return result;
}

void GameStage::RenderStage()
{
	StageGeom* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if (geom->textureID!=-1)
		{
			LPDIRECT3DTEXTURE9 tex = m_TextureMgr.GetD3DTexture(geom->textureID);
			RenderSystem::Device()->SetTexture(0, tex);

			// enable mip-map for texture
			RenderSystem::Device()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}
		else
		{
			RenderSystem::Device()->SetTexture(0, NULL);
		}

		RenderSystem::Device()->SetStreamSource(0, geom->vbuffer, 0, sizeof(StageGeomVertex));
		RenderSystem::Device()->SetFVF(StageGeomFVF);
		RenderSystem::Device()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	}
}

void GameStage::Reset()
{
	m_GeomCount = 0;

	StageGeom* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; )
	{
		geom->vbuffer->Release();

		StageGeom* old_geom = geom;
		geom = GetNextStageGeom(geom);
		delete old_geom;
	}

	StageGeomHead = NULL;
	StageGeomTail = NULL;
}

void GameStage::TestCollision( Character* character, const Vector3& vecRel )
{
	bool result = false;
	Vector3 rel = vecRel + character->Velocity();

	StageGeom* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		result |= character->DoCollisionMove(geom->bound, rel, &rel);
		//if (result) break;
	}

	character->Translate(rel);
}
