#include "GameStage.h"

#include "../DXUT/DXUT.h"
#include <string>

#include "../LuaPlus/LuaPlus.h"

using namespace LuaPlus;

typedef struct TileTypeTable
{
	TileTypeEnum	type;
	char			name[256];
} TILE_TYPE_TABLE;

TILE_TYPE_TABLE tile_table[] =
{
	{ TILE_SOLID,		"SOLID"		},
	{ TILE_LADDER,		"LADDER"	},
	{ TILE_END,			""			},
};

TileTypeEnum StringToTileType( const char* type_name )
{
	for ( int i = 0; tile_table[i].type != TILE_END ; i++ )
	{
		if ( strcmp(type_name, tile_table[i].name) == 0 )
		{
			return (TileTypeEnum)i;
		}
	}

	return TILE_VOID;
}

char GameWorldviewKeyWord[GAME_WORLD_COUNT][32] =
{
	"GameWorldCommon",
	"GameWorld0",
	"GameWorld1",
	"GameWorld2",
};

typedef struct StageGeomListType
{
	STAGE_GEOM*				StageGeomHead;
	STAGE_GEOM*				StageGeomTail;
	int						geom_count;
	std::map<std::string, TILE_TYPE_INFO>
							tile_types;
} STAGE_GEOM_LIST;

static STAGE_GEOM_LIST StageGeomList[GAME_WORLD_COUNT] = { { NULL, NULL, 0 } };

// Stage geometry methods
STAGE_GEOM* CreateStageGeom(int world_id)
{
	STAGE_GEOM* geom = new STAGE_GEOM();
	geom->next = NULL;

	if (StageGeomList[world_id].StageGeomTail == NULL)
	{
		// Build new linked list
		StageGeomList[world_id].StageGeomHead = geom;
		StageGeomList[world_id].StageGeomTail = geom;
	}
	else
	{
		// Add new geometry to the tail
		StageGeomList[world_id].StageGeomTail->next = geom;
		StageGeomList[world_id].StageGeomTail = geom;
	}

	return geom;
}

STAGE_GEOM* GetFirstStageGeom(int world_id)
{
	return StageGeomList[world_id].StageGeomHead;
}

STAGE_GEOM* GetNextStageGeom(STAGE_GEOM* geom)
{
	return geom->next;
}


GameStage::GameStage()
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

		for (int i=0; i<GAME_WORLD_COUNT; i++)
		{
			LuaObject world_script = stage_script[GameWorldviewKeyWord[i]];

			if (world_script.IsNil())
				continue;

			//int layers = stage["Layers"].GetInteger();

			// Load tile types
			LuaObject tiletype_script = world_script["TileTypes"];
			ScriptLoadTileTypes(&tiletype_script, GAME_WORLD_COMMON);

			// Load geometries for the stage
			LuaObject geom_script = world_script["Geometries"];
			ScriptLoadGeometries(&geom_script, GAME_WORLD_COMMON);

			// Load tile types for each worldview
		}

		result = true;
	}

	return result;
}

void GameStage::RenderStage()
{
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if (geom->texture_id!=-1)
		{
			LPDIRECT3DTEXTURE9 tex = m_TextureMgr.GetD3DTexture(geom->texture_id);
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
	for ( int i = 0; i < GAME_WORLD_COUNT; i++ )
	{
		STAGE_GEOM* geom;
		for (geom = GetFirstStageGeom(i); geom != NULL; )
		{
			geom->vbuffer->Release();

			STAGE_GEOM* old_geom = geom;
			geom = GetNextStageGeom(geom);
			delete old_geom;
		}

		StageGeomList[i].StageGeomHead = NULL;
		StageGeomList[i].StageGeomTail = NULL;
		StageGeomList[i].geom_count = 0;
		StageGeomList[i].tile_types.clear();
	}
}

void GameStage::TestCollision( Character* character, const Vector3& vecRel )
{
	bool result = false;
	Vector3 rel = vecRel + character->Velocity();

	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( geom->type != TILE_SOLID )
			continue;

		result |= character->DoCollisionMove(geom->bound, rel, &rel);
	}

	character->Translate(rel);
}

TileTypeEnum GameStage::GetTileTypeAtPoint( const Vector3 point ) const
{
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( geom->bound.IsPointInsideBox(point.x, point.y) )
			return geom->type;
	}

	return TILE_VOID;
}

void GameStage::ScriptLoadTileTypes( const LuaPlus::LuaObject* script, int world_id )
{
	int tiletype_count = script->GetTableCount();

	for (int i=0; i<tiletype_count; i++)
	{
		std::string type_name = (*script)[i+1][1].GetString();

		TILE_TYPE_INFO tile_info;
		strcpy(tile_info.into_type_name, (*script)[i+1][2].GetString());
		const char* tex_name = (*script)[i+1][3].GetString();

		int texID = -1;
		if ( (texID=m_TextureMgr.GetTextureID(tex_name)) == -1)
		{
			if (m_TextureMgr.LoadTextureFromFile(tex_name))
				texID = m_TextureMgr.GetTextureID(tex_name);
		}

		tile_info.tex_id = texID;

		StageGeomList[world_id].tile_types[type_name] = tile_info;
	}
}

void GameStage::ScriptLoadGeometries( const LuaPlus::LuaObject* script, int world_id )
{
	int geom_count = StageGeomList[world_id].geom_count = script->GetTableCount();

	for (int i=0; i<geom_count; i++)
	{
		STAGE_GEOM* geom = CreateStageGeom(world_id);
		BoundBox box;

		int layer = (*script)[i+1][1].GetInteger();
		box.xMin = (float)(*script)[i+1][2].GetInteger();
		box.xMax = (float)(*script)[i+1][3].GetInteger();
		box.yMin = (float)(*script)[i+1][4].GetInteger();
		box.yMax = (float)(*script)[i+1][5].GetInteger();
		const char* tex_name = (*script)[i+1][6].GetString();

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
		TileTypeEnum t = TILE_VOID;
		if (StageGeomList[world_id].tile_types.find(tex_name)!=StageGeomList[world_id].tile_types.end())
		{
			texID = StageGeomList[world_id].tile_types[tex_name].tex_id;
			t = StringToTileType(StageGeomList[world_id].tile_types[tex_name].into_type_name);
		}

		geom->bound = box;
		geom->texture_id = texID;
		geom->type = t;

		RenderSystem::Device()->CreateVertexBuffer(sizeof(StageGeomVertex) * 6, D3DUSAGE_WRITEONLY,
			D3DFVF_XYZ|D3DFVF_TEX1, D3DPOOL_DEFAULT, &geom->vbuffer, NULL);

		void* pData;
		geom->vbuffer->Lock(0, sizeof(StageGeomVertex) * 6, (void**)&pData, 0);
		memcpy(pData, v, sizeof(StageGeomVertex) * 6);
		geom->vbuffer->Unlock();
	}
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
