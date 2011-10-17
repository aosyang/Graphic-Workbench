#include "GameStage.h"

#include "../DXUT/DXUT.h"
#include <string>
#include <vector>

#include "../LuaPlus/LuaPlus.h"

using namespace LuaPlus;

typedef struct TileUsageTable
{
	TileUsageEnum	usage;
	char			name[256];
} TILE_USAGE_TABLE;

TILE_USAGE_TABLE tile_usage_table[] =
{
	{ TILE_USAGE_SOLID,			"SOLID"		},
	{ TILE_USAGE_LADDER,		"LADDER"	},
	{ TILE_USAGE_END,			""			},
};

TileUsageEnum StringToTileUsage( const char* type_name )
{
	for ( int i = 0; tile_usage_table[i].usage != TILE_USAGE_END ; i++ )
	{
		if ( strcmp(type_name, tile_usage_table[i].name) == 0 )
		{
			return (TileUsageEnum)i;
		}
	}

	return TILE_USAGE_VOID;
}

const char* TileUsageToString( TileUsageEnum usage )
{
	return tile_usage_table[usage].name;
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

	std::map<int, TILE_TYPE_INFO>
							world_tile_types;
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

void DebugRenderStageGeom( STAGE_GEOM* geom )
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
}

GameStage::GameStage()
: m_ActiveWorld(GAME_WORLD_COMMON),
  m_TileTypeIndex(0)
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
		//state->DumpObject("Stage_out.lua", "Stage", state->GetGlobals()["Stage"], 0);

		LuaObject stage_script = state->GetGlobals()["Stage"];

		for (int i=0; i<GAME_WORLD_COUNT; i++)
		{
			LuaObject world_script = stage_script[GameWorldviewKeyWord[i]];

			if (world_script.IsNil())
				continue;

			//int layers = stage["Layers"].GetInteger();

			// Load tile types
			LuaObject tiletype_script = world_script["TileTypes"];
			ScriptLoadTileTypes(&tiletype_script, i);

			// Load geometries for the stage
			LuaObject geom_script = world_script["Geometries"];
			ScriptLoadGeometries(&geom_script, i);

			// Load tile types for each worldview
		}

		result = true;
	}

	return result;
}

bool GameStage::SaveToFile( const char* filename )
{
	LuaStateOwner state;

	LuaObject stage_script = state->GetGlobals().CreateTable("Stage");

	LuaObject world_script = stage_script.CreateTable(GameWorldviewKeyWord[0]);
	LuaObject tile_type_script = world_script.CreateTable("TileTypes");

	// Write tile types
	int i = 1;
	std::map<std::string, int>::iterator iter;
	for (iter=m_TileName2Id.begin(); iter!=m_TileName2Id.end(); iter++)
	{
		LuaObject tile_type_obj = tile_type_script.CreateTable(i);
		int tile_id = iter->second;
		int tex_id = m_TileId2TypeInfo[tile_id].tex_id;

		tile_type_obj.SetString(1, iter->first.c_str());
		tile_type_obj.SetString(2, TileUsageToString(m_TileId2TypeInfo[tile_id].usage));
		tile_type_obj.SetString(3, m_TextureMgr.GetTextureName(tex_id));

		i++;
	}

	LuaObject geom_group_script = world_script.CreateTable("Geometries");

	i = 1;
	// Write geometries
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom!=NULL; geom = GetNextStageGeom(geom))
	{
		LuaObject geom_script = geom_group_script.CreateTable(i);

		geom_script.SetInteger(1, 0);
		geom_script.SetNumber(2, geom->bound.xMin);
		geom_script.SetNumber(3, geom->bound.xMax);
		geom_script.SetNumber(4, geom->bound.yMin);
		geom_script.SetNumber(5, geom->bound.yMax);

		for (iter=m_TileName2Id.begin(); iter!=m_TileName2Id.end(); iter++)
		{
			if (iter->second == geom->tile_type_id)
			{
				geom_script.SetString(6, iter->first.c_str());
				break;
			}
		}

		if (iter==m_TileName2Id.end())
		{
			geom_script.SetString(6, "");
		}

		i++;
	}

	return state->DumpGlobals(filename);
}

void GameStage::RenderStage()
{
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom != NULL; geom = GetNextStageGeom(geom))
	{
		RenderStageGeom(geom);
	}

	// Got an extra world to render
	if (m_ActiveWorld>GAME_WORLD_COMMON && m_ActiveWorld<GAME_WORLD_COUNT)
	{
		for (geom = GetFirstStageGeom(m_ActiveWorld); geom != NULL; geom = GetNextStageGeom(geom))
		{
			RenderStageGeom(geom);
		}
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
		StageGeomList[i].world_tile_types.clear();
	}

	m_ActiveWorld = GAME_WORLD_COMMON;

	m_TileTypeIndex = 0;
	m_TileName2Id.clear();
	m_TileId2TypeInfo.clear();
}

void GameStage::TestCollision( Character* character, const Vector3& vecRel )
{
	bool result = false;
	Vector3 rel = vecRel + character->Velocity();

	std::vector<STAGE_GEOM*> col_group;

	STAGE_GEOM* geom;

	// Collect collision objects
	for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( GetTileUsageById(geom->tile_type_id) != TILE_USAGE_SOLID )
			continue;

		if (character->TestCollision(geom, rel))
			col_group.push_back(geom);
	}

	if (m_ActiveWorld != GAME_WORLD_COMMON)
	{
		for (geom = GetFirstStageGeom(m_ActiveWorld); geom != NULL; geom = GetNextStageGeom(geom))
		{
			if ( GetTileUsageById(geom->tile_type_id) != TILE_USAGE_SOLID )
				continue;

			if (character->TestCollision(geom, rel))
				col_group.push_back(geom);
		}
	}

	if (!col_group.empty())
	{
		Vector3 rel_x, rel_y;
		rel_x = rel;
		rel_y = rel;

		rel_x.y = 0.0f;
		rel_y.x = 0.0f;

		std::vector<STAGE_GEOM*>::iterator iter;
		// Test collision move in x dir and y dir separately
		for ( iter = col_group.begin(); iter != col_group.end(); iter++ )
		{
			result |= character->DoCollisionMove((*iter)->bound, rel_y, &rel_y);
		}

		rel = rel_x + rel_y;

		for ( iter = col_group.begin(); iter != col_group.end(); iter++ )
		{
			result |= character->DoCollisionMove((*iter)->bound, rel, &rel);
		}
	}

	character->Translate(rel);
}

STAGE_GEOM* GameStage::GetTileAtPoint( const Vector3& point )
{
	STAGE_GEOM* geom;
	if (m_ActiveWorld != GAME_WORLD_COMMON)
	{
		for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom != NULL; geom = GetNextStageGeom(geom))
		{
			if ( geom->bound.IsPointInsideBox(point.x, point.y) )
				return geom;
		}
	}

	for (geom = GetFirstStageGeom(GAME_WORLD_COMMON); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( geom->bound.IsPointInsideBox(point.x, point.y) )
			return geom;
	}

	return NULL;
}

TileUsageEnum GameStage::GetStageGeomUsage( STAGE_GEOM* geom )
{
	return GetTileUsageById(geom->tile_type_id);
}

void GameStage::SetWorldview( int world_id )
{
	m_ActiveWorld = (GameWorldviewEnum)world_id;
}



const char* GameStage::GetTileNameById( int tile_id ) const
{
	std::map<std::string, int>::const_iterator iter;
	for (iter=m_TileName2Id.begin(); iter!=m_TileName2Id.end(); iter++)
	{
		if (iter->second == tile_id)
			return iter->first.c_str();
	}

	return NULL;
}

STAGE_GEOM* GameStage::AddStageGeom( int world_id, int layer_id, const BoundBox& bound, const char* tile_type_name )
{
	STAGE_GEOM* geom = CreateStageGeom(world_id);

	float geom_depth = (float)layer_id;

	// build vertex buffer with vertex position as its own texture coordinate
	StageGeomVertex v[6] =
	{
#if 0		// Use layer as depth
		{ bound.xMin, bound.yMin, geom_depth, bound.xMin, bound.yMin },
		{ bound.xMin, bound.yMax, geom_depth, bound.xMin, bound.yMax },
		{ bound.xMax, bound.yMax, geom_depth, bound.xMax, bound.yMax },

		{ bound.xMax, bound.yMax, geom_depth, bound.xMax, bound.yMax },
		{ bound.xMax, bound.yMin, geom_depth, bound.xMax, bound.yMin },
		{ bound.xMin, bound.yMin, geom_depth, bound.xMin, bound.yMin },
#else
		{ bound.xMin, bound.yMin, 0.0f, bound.xMin, bound.yMin },
		{ bound.xMin, bound.yMax, 0.0f, bound.xMin, bound.yMax },
		{ bound.xMax, bound.yMax, 0.0f, bound.xMax, bound.yMax },

		{ bound.xMax, bound.yMax, 0.0f, bound.xMax, bound.yMax },
		{ bound.xMax, bound.yMin, 0.0f, bound.xMax, bound.yMin },
		{ bound.xMin, bound.yMin, 0.0f, bound.xMin, bound.yMin },
#endif
	};

	// Find texture id by name
	int geom_tile_type = -1;
	TileUsageEnum t = TILE_USAGE_VOID;
	if (m_TileName2Id.find(tile_type_name)!=m_TileName2Id.end())
	{
		geom_tile_type = m_TileName2Id[tile_type_name];
	}

	geom->bound = bound;
	geom->tile_type_id = geom_tile_type;

	RenderSystem::Device()->CreateVertexBuffer(sizeof(StageGeomVertex) * 6, D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ|D3DFVF_TEX1, D3DPOOL_DEFAULT, &geom->vbuffer, NULL);

	void* pData;
	geom->vbuffer->Lock(0, sizeof(StageGeomVertex) * 6, (void**)&pData, 0);
	memcpy(pData, v, sizeof(StageGeomVertex) * 6);
	geom->vbuffer->Unlock();

	return geom;
}

void GameStage::ScriptLoadTileTypes( const LuaPlus::LuaObject* script, int world_id )
{
	if (script->IsNil()) return;

	int tiletype_count = script->GetTableCount();

	for (int i=0; i<tiletype_count; i++)
	{
		// Load tile type info into common container
		std::string tile_name = (*script)[i+1][1].GetString();

		TILE_TYPE_INFO tile_info;
		tile_info.usage = StringToTileUsage((*script)[i+1][2].GetString());
		const char* tex_name = (*script)[i+1][3].GetString();

		int texID = -1;
		if ( (texID=m_TextureMgr.GetTextureID(tex_name)) == -1)
		{
			if (m_TextureMgr.LoadTextureFromFile(tex_name))
				texID = m_TextureMgr.GetTextureID(tex_name);
		}

		tile_info.tex_id = texID;

		// Only load tile types overridden by this worldview
		if ( world_id != GAME_WORLD_COMMON && m_TileName2Id.find(tile_name) != m_TileName2Id.end() )
		{
			// We've already got this tile in common world, override!
			int tile_id = m_TileName2Id[tile_name];
			StageGeomList[world_id].world_tile_types[tile_id] = tile_info;
		}
		else
		{
			// Add tile type to stage containers
			m_TileName2Id[tile_name] = m_TileTypeIndex;
			m_TileId2TypeInfo[m_TileTypeIndex] = tile_info;
			m_TileTypeIndex++;
		}
	}
}

void GameStage::ScriptLoadGeometries( const LuaPlus::LuaObject* script, int world_id )
{
	if (script->IsNil()) return;

	int geom_count = StageGeomList[world_id].geom_count = script->GetTableCount();

	for (int i=0; i<geom_count; i++)
	{
		BoundBox box;

		int layer = (*script)[i+1][1].GetInteger();
		box.xMin = (float)(*script)[i+1][2].GetInteger();
		box.xMax = (float)(*script)[i+1][3].GetInteger();
		box.yMin = (float)(*script)[i+1][4].GetInteger();
		box.yMax = (float)(*script)[i+1][5].GetInteger();
		const char* tile_type_name = (*script)[i+1][6].GetString();

		AddStageGeom(world_id, layer, box, tile_type_name);
	}
}

void GameStage::RenderStageGeom( STAGE_GEOM* geom )
{
	int tile_id = geom->tile_type_id;
	int tex_id = m_TileId2TypeInfo[tile_id].tex_id;

	if (m_ActiveWorld!=GAME_WORLD_COMMON)
	{
		if ( StageGeomList[m_ActiveWorld].world_tile_types.find(tile_id) !=
			 StageGeomList[m_ActiveWorld].world_tile_types.end() )
		{
			tex_id = StageGeomList[m_ActiveWorld].world_tile_types[tile_id].tex_id;
		}
	}

	if (tex_id!=-1)
	{
		LPDIRECT3DTEXTURE9 tex = m_TextureMgr.GetD3DTexture(tex_id);
		RenderSystem::Device()->SetTexture(0, tex);

		// enable mip-map for texture
		RenderSystem::Device()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		RenderSystem::Device()->SetTexture(0, NULL);
	}

	RenderSystem::Device()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	RenderSystem::Device()->SetStreamSource(0, geom->vbuffer, 0, sizeof(StageGeomVertex));
	RenderSystem::Device()->SetFVF(StageGeomFVF);
	RenderSystem::Device()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	//DebugRenderStageGeom(geom);
}

TileUsageEnum GameStage::GetTileUsageById( int id )
{
	if ( m_ActiveWorld != GAME_WORLD_COMMON )
	{
		if ( StageGeomList[m_ActiveWorld].world_tile_types.find(id)
			 != StageGeomList[m_ActiveWorld].world_tile_types.end() )
		{
			return StageGeomList[m_ActiveWorld].world_tile_types[id].usage;
		}
	}

	if ( m_TileId2TypeInfo.find(id) != m_TileId2TypeInfo.end() )
	{
		return m_TileId2TypeInfo[id].usage;
	}

	return TILE_USAGE_VOID;
}
