#include "GameStage.h"
#include "TextureManager.h"

#include "../DXUT/DXUT.h"
#include <string>
#include <vector>

#include "../LuaPlus/LuaPlus.h"

using namespace LuaPlus;

#include "GameMain.h"

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
	//"GameWorldCommon",
	"GameWorld0",
	"GameWorld1",
	//"GameWorld2",
};

typedef struct StageGeomListType
{
	STAGE_GEOM*				StageGeomHead;
	STAGE_GEOM*				StageGeomTail;
	int						geom_count;
} STAGE_GEOM_LIST;

static STAGE_GEOM_LIST StageGeomList = { NULL, NULL, 0 };

// Stage geometry methods
STAGE_GEOM* CreateStageGeom()
{
	STAGE_GEOM* geom = new STAGE_GEOM();
	geom->next = NULL;

	if (StageGeomList.StageGeomTail == NULL)
	{
		// Build new linked list
		StageGeomList.StageGeomHead = geom;
		StageGeomList.StageGeomTail = geom;
	}
	else
	{
		// Add new geometry to the tail
		StageGeomList.StageGeomTail->next = geom;
		StageGeomList.StageGeomTail = geom;
	}

	return geom;
}

STAGE_GEOM* GetFirstStageGeom()
{
	return StageGeomList.StageGeomHead;
}

STAGE_GEOM* GetNextStageGeom(STAGE_GEOM* geom)
{
	return geom->next;
}

void DebugRenderStageGeom( STAGE_GEOM* geom )
{
	RenderSystem::DrawColoredSprite(Vector2(geom->bound.xMin, geom->bound.yMin),
									Vector2(geom->bound.xMax, geom->bound.yMax),
									0xFFFFF200, true);
}

GameStage::GameStage()
: m_ActiveWorld((GameWorldviewEnum)0),
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

		//int layers = stage["Layers"].GetInteger();

		// Load tile types
		LuaObject tiletype_script = stage_script["TileTypes"];
		ScriptLoadTileTypes(&tiletype_script);

		// Load geometries for the stage
		LuaObject geom_script = stage_script["Geometries"];
		ScriptLoadGeometries(&geom_script);

		result = true;
	}

	return result;
}

bool GameStage::SaveToFile( const char* filename )
{
	LuaStateOwner state;

	LuaObject stage_script = state->GetGlobals().CreateTable("Stage");

	//LuaObject world_script = stage_script.CreateTable(GameWorldviewKeyWord[0]);
	LuaObject tile_type_script = stage_script.CreateTable("TileTypes");

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
		tile_type_obj.SetString(3, TextureManager::Instance().GetTextureName(tex_id));

		i++;
	}

	LuaObject geom_group_script = stage_script.CreateTable("Geometries");

	i = 1;
	// Write geometries
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(); geom!=NULL; geom = GetNextStageGeom(geom))
	{
		// Don't output if tile is null in all worlds
		bool ignore_geom = true;
		for (int j=0; j<GAME_WORLD_COUNT; j++)
		{
			if (geom->tile_type_id[j] != -1)
			{
				ignore_geom = false;
				break;
			}
		}

		if (ignore_geom) continue;

		LuaObject geom_script = geom_group_script.CreateTable(i);

		geom_script.SetInteger(1, 0);
		geom_script.SetNumber(2, geom->bound.xMin);
		geom_script.SetNumber(3, geom->bound.xMax);
		geom_script.SetNumber(4, geom->bound.yMin);
		geom_script.SetNumber(5, geom->bound.yMax);

		for (int j=0; j<GAME_WORLD_COUNT; j++)
		{
			for (iter=m_TileName2Id.begin(); iter!=m_TileName2Id.end(); iter++)
			{
				if (iter->second == geom->tile_type_id[j])
				{
					geom_script.SetString(6 + j, iter->first.c_str());
					break;
				}
			}

			if (iter==m_TileName2Id.end())
			{
				geom_script.SetString(6 + j, "");
			}
		}

		i++;
	}

	return state->DumpGlobals(filename);
}

void GameStage::RenderStage()
{
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		RenderStageGeom(geom);
	}
}

void GameStage::Reset()
{
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; )
	{
		STAGE_GEOM* old_geom = geom;
		geom = GetNextStageGeom(geom);
		delete old_geom;
	}

	StageGeomList.StageGeomHead = NULL;
	StageGeomList.StageGeomTail = NULL;
	StageGeomList.geom_count = 0;

	m_ActiveWorld = (GameWorldviewEnum)0;

	m_TileTypeIndex = 0;
	m_TileName2Id.clear();
	m_TileId2TypeInfo.clear();
}

void GameStage::TestCollision( Character* character, const Vector2& vecRel )
{
	bool result = false;
	Vector2 rel = vecRel + character->Velocity();

	std::vector<STAGE_GEOM*> col_group;

	STAGE_GEOM* geom;

	// Collect collision objects
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( GetTileUsageById(geom->tile_type_id[m_ActiveWorld]) != TILE_USAGE_SOLID )
			continue;

		if (character->TestCollision(geom, rel))
			col_group.push_back(geom);
	}

	if (!col_group.empty())
	{
		Vector2 rel_x, rel_y;
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

STAGE_GEOM* GameStage::GetTileAtPoint( const Vector2& point )
{
	STAGE_GEOM* geom;
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( geom->bound.IsPointInsideBox( point ) )
			return geom;
	}

	return NULL;
}

TileUsageEnum GameStage::GetStageGeomUsage( STAGE_GEOM* geom )
{
	return GetTileUsageById(geom->tile_type_id[m_ActiveWorld]);
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

int GameStage::GetTileIdByName( const char* tile_name ) const
{
	std::map<std::string, int>::const_iterator iter;
	if ( (iter = m_TileName2Id.find(tile_name)) != m_TileName2Id.end() )
		return iter->second;

	return -1;
}

STAGE_GEOM* GameStage::AddStageGeom( int layer_id, const BoundBox& bound, const char* tile_type_name[GAME_WORLD_COUNT] )
{
	STAGE_GEOM* geom = CreateStageGeom();

	float geom_depth = (float)layer_id;

	// Find texture id by name
	for (int i=0; i<GAME_WORLD_COUNT; i++)
	{
		int geom_tile_type = -1;
		TileUsageEnum t = TILE_USAGE_VOID;
		if (m_TileName2Id.find(tile_type_name[i])!=m_TileName2Id.end())
		{
			geom_tile_type = m_TileName2Id[tile_type_name[i]];
		}
		geom->tile_type_id[i] = geom_tile_type;
	}

	geom->bound = bound;

	return geom;
}

void GameStage::ScriptLoadTileTypes( const LuaPlus::LuaObject* script )
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
		if ( (texID=TextureManager::Instance().GetTextureID(tex_name)) == -1)
		{
			if (TextureManager::Instance().LoadTextureFromFile(tex_name))
				texID = TextureManager::Instance().GetTextureID(tex_name);
		}

		tile_info.tex_id = texID;

		// Add tile type to stage containers
		m_TileName2Id[tile_name] = m_TileTypeIndex;
		m_TileId2TypeInfo[m_TileTypeIndex] = tile_info;
		m_TileTypeIndex++;
	}
}

void GameStage::ScriptLoadGeometries( const LuaPlus::LuaObject* script )
{
	if (script->IsNil()) return;

	int geom_count = StageGeomList.geom_count = script->GetTableCount();

	for (int i=0; i<geom_count; i++)
	{
		BoundBox box;

		int elem_count = (*script)[i+1].GetCount();

		int layer = (*script)[i+1][1].GetInteger();
		box.xMin = (float)(*script)[i+1][2].GetInteger();
		box.xMax = (float)(*script)[i+1][3].GetInteger();
		box.yMin = (float)(*script)[i+1][4].GetInteger();
		box.yMax = (float)(*script)[i+1][5].GetInteger();
		const char* tile_type_name[GAME_WORLD_COUNT];
		for (int j=0; j<GAME_WORLD_COUNT; j++)
		{
			// In case we don't have enough string
			if (6+j <= elem_count)
			{
				tile_type_name[j] = (*script)[i+1][6+j].GetString();
			}
			else
				tile_type_name[j] = "";
		}

		AddStageGeom(layer, box, tile_type_name);
	}
}

void GameStage::RenderStageGeom( STAGE_GEOM* geom )
{
	int world_id = m_ActiveWorld;

	if (KleinGame()->TestProtoFeatureBit(PROTO_FEATURE_CIRCLE_OF_TRUE_VIEW))
	{
		Vector2 geom_centre = geom->bound.GetCentrePoint();
		float sqrd_dist = ( geom_centre - KleinGame()->GetPlayerPos() ).SqrdLen();

		// Revert geoms far from player
		if (sqrd_dist > 25) world_id = 1 - world_id;
	}

	int tile_id = geom->tile_type_id[world_id];

	// Don't render tile with no type
	if (tile_id == -1) return;

	int tex_id = m_TileId2TypeInfo[tile_id].tex_id;

	RenderSystem::DrawSprite(Vector2(geom->bound.xMin, geom->bound.yMin),
							 Vector2(geom->bound.xMax, geom->bound.yMax), tex_id);

	//DebugRenderStageGeom(geom);
}

TileUsageEnum GameStage::GetTileUsageById( int id )
{
	if ( m_TileId2TypeInfo.find(id) != m_TileId2TypeInfo.end() )
	{
		return m_TileId2TypeInfo[id].usage;
	}

	return TILE_USAGE_VOID;
}
