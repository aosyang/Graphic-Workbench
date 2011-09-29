#include "GameStage.h"

#include "../DXUT/DXUT.h"
#include <string>

#include "../LuaPlus/LuaPlus.h"

using namespace LuaPlus;

TileTypeTable tile_table[] =
{
	{ TILE_SOLID,		"SOLID" },
	{ TILE_LADDER,		"LADDER" },
	{ TILE_END,			"" },
};

static STAGE_GEOM* StageGeomHead = NULL;
static STAGE_GEOM* StageGeomTail = NULL;

// Stage geometry methods
STAGE_GEOM* CreateStageGeom()
{
	STAGE_GEOM* geom = new STAGE_GEOM();
	geom->next = NULL;

	if (StageGeomTail == NULL)
	{
		// Build new linked list
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

STAGE_GEOM* GetFirstStageGeom()
{
	return StageGeomHead;
}

STAGE_GEOM* GetNextStageGeom(STAGE_GEOM* geom)
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

		// Load tile types
		LuaObject tiletype_script = stage_script["TileTypes"];
		int tiletype_count = tiletype_script.GetTableCount();

		for (int i=0; i<tiletype_count; i++)
		{
			std::string type_name = tiletype_script[i+1][1].GetString();

			TILE_TYPE_INFO tile_info;
			tile_info.type = tiletype_script[i+1][2].GetString();
			const char* tex_name = tiletype_script[i+1][3].GetString();

			int texID = -1;
			if ( (texID=m_TextureMgr.GetTextureID(tex_name)) == -1)
			{
				if (m_TextureMgr.LoadTextureFromFile(tex_name))
					texID = m_TextureMgr.GetTextureID(tex_name);
			}

			tile_info.texID = texID;

			m_TileTypes[type_name] = tile_info;
		}

		// Load geometries for the stage
		LuaObject geom_script = stage_script["Geometries"];
		m_GeomCount = geom_script.GetTableCount();

		for (int i=0; i<m_GeomCount; i++)
		{
			STAGE_GEOM* geom = CreateStageGeom();
			BoundBox box;

			int layer = geom_script[i+1][1].GetInteger();
			box.xMin = (float)geom_script[i+1][2].GetInteger();
			box.xMax = (float)geom_script[i+1][3].GetInteger();
			box.yMin = (float)geom_script[i+1][4].GetInteger();
			box.yMax = (float)geom_script[i+1][5].GetInteger();
			const char* tex_name = geom_script[i+1][6].GetString();

			float geom_depth = (float)layer;

			// build vertex buffer with vertex position as its own texture coordinate
			StageGeomVertex v[6] =
			{
#if 0		// Use layer as depth
				{ box.xMin, box.yMin, geom_depth, box.xMin, box.yMin },
				{ box.xMin, box.yMax, geom_depth, box.xMin, box.yMax },
				{ box.xMax, box.yMax, geom_depth, box.xMax, box.yMax },

				{ box.xMax, box.yMax, geom_depth, box.xMax, box.yMax },
				{ box.xMax, box.yMin, geom_depth, box.xMax, box.yMin },
				{ box.xMin, box.yMin, geom_depth, box.xMin, box.yMin },
#else
				{ box.xMin, box.yMin, 0.0f, box.xMin, box.yMin },
				{ box.xMin, box.yMax, 0.0f, box.xMin, box.yMax },
				{ box.xMax, box.yMax, 0.0f, box.xMax, box.yMax },

				{ box.xMax, box.yMax, 0.0f, box.xMax, box.yMax },
				{ box.xMax, box.yMin, 0.0f, box.xMax, box.yMin },
				{ box.xMin, box.yMin, 0.0f, box.xMin, box.yMin },
#endif
			};

			// Find texture id by name
			int texID = -1;
			TileType t = TILE_VOID;
			if (m_TileTypes.find(tex_name)!=m_TileTypes.end())
			{
				texID = m_TileTypes[tex_name].texID;
				std::string type_name = m_TileTypes[tex_name].type;
				for ( int i = 0; tile_table[i].type != TILE_END ; i++ )
				{
					if ( type_name == tile_table[i].name )
					{
						t = (TileType)i;
						break;
					}
				}
			}

			geom->bound = box;
			geom->textureID = texID;
			geom->type = t;

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
	STAGE_GEOM* geom;
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

		//DebugRenderStageGeom(geom);
	}
}

void GameStage::Reset()
{
	m_GeomCount = 0;

	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; )
	{
		geom->vbuffer->Release();

		STAGE_GEOM* old_geom = geom;
		geom = GetNextStageGeom(geom);
		delete old_geom;
	}

	StageGeomHead = NULL;
	StageGeomTail = NULL;

	m_TileTypes.clear();
}

void GameStage::TestCollision( Character* character, const Vector3& vecRel )
{
	bool result = false;
	Vector3 rel = vecRel + character->Velocity();

	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( geom->type != TILE_SOLID )
			continue;

		result |= character->DoCollisionMove(geom->bound, rel, &rel);
	}

	character->Translate(rel);
}

TileType GameStage::GetTileTypeAtPoint( const Vector3 point ) const
{
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( geom->bound.IsPointInsideBox(point.x, point.y) )
			return geom->type;
	}

	return TILE_VOID;
}

void GameStage::DebugRenderStageGeom( STAGE_GEOM* geom )
{
	StageGeomWireframeVertex v[6] =
	{
		{ geom->bound.xMin, geom->bound.yMin, 0.0f, 0xFFFFF200 },
		{ geom->bound.xMin, geom->bound.yMax, 0.0f, 0xFFFFF200 },
		{ geom->bound.xMax, geom->bound.yMax, 0.0f, 0xFFFFF200 },

		{ geom->bound.xMax, geom->bound.yMax, 0.0f, 0xFFFFF200 },
		{ geom->bound.xMax, geom->bound.yMin, 0.0f, 0xFFFFF200 },
		{ geom->bound.xMin, geom->bound.yMin, 0.0f, 0xFFFFF200 },
	};

	RenderSystem::Device()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	RenderSystem::Device()->SetTexture(0, NULL);
	RenderSystem::Device()->SetFVF(StageGeomWireframeFVF);
	RenderSystem::Device()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(StageGeomWireframeVertex));

	RenderSystem::Device()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
