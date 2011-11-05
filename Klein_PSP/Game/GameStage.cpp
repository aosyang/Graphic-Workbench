/********************************************************************
	created:	2011/9/5
	filename: 	GameStage.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "GameStage.h"
#include "Renderer/TextureManager.h"
#include "GWLog.h"
#include "AreaTrigger.h"

#include <cstring>
#include <string>
#include <vector>

#if !defined KLEIN_NO_STAGE_SAVE
#include "../LuaPlus/LuaPlus.h"
using namespace LuaPlus;
#endif

#if defined GW_PLATFORM_PSP
#include <lua.hpp>
#else
#include "../Lua/lua.hpp"
#endif

#include "GameMain.h"

#define TIME_WORLD_SWAP_ANIM 1000

typedef struct TileUsageTable
{
	TileUsageEnum	usage;
	char			name[256];
} TILE_USAGE_TABLE;

TILE_USAGE_TABLE tile_usage_table[] =
{
	{ TILE_USAGE_SOLID,			"SOLID"		},
	{ TILE_USAGE_LADDER,		"LADDER"	},
	{ TILE_USAGE_TRIGGER_HURT,	"HURT"		},
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

int GetStageGeomCount()
{
	return StageGeomList.geom_count;
}

void DebugRenderStageGeom( STAGE_GEOM* geom )
{
	if (geom->tile_type_id[KleinGame()->GetWorldview()] != -1)
	{
		RenderSystem::DrawWireframeRect(Vector2(geom->bound.xMin, geom->bound.yMin),
										Vector2(geom->bound.xMax, geom->bound.yMax),
										GWColor(1.0f, 0.949f, 0.0f));
	}
}

GameStage::GameStage()
: m_TileTypeIndex(0)
{

}

GameStage::~GameStage()
{
	Reset();
}

bool GameStage::LoadFromFile( const char* filename )
{
	bool result = false;
	lua_State* pLuaState = lua_open();
	//luaL_openlibs(pLuaState);
	int err_code = luaL_dofile(pLuaState, filename);

	if (!err_code)
	{
		lua_getglobal(pLuaState, "Stage");

		if (lua_istable(pLuaState, -1))
		{
			//int layers = stage["Layers"].GetInteger();

			// Load tile types
			GWLog_Print("Load tile types.\n");
			ScriptLoadTileTypes(pLuaState);

			// Load geometries for the stage
			GWLog_Print("Load geometries.\n");
			ScriptLoadGeometries(pLuaState);

			// Load area triggers
			GWLog_Print("Load area triggers.\n");
			ScriptLoadTriggers(pLuaState);

			result = true;
		}
	}

	lua_close(pLuaState);

	return result;
}

#if !defined KLEIN_NO_STAGE_SAVE
bool GameStage::SaveToFile( const char* filename )
{
	LuaStateOwner state;

	LuaObject stage_script = state->GetGlobals().CreateTable("Stage");

	ScriptSaveTileTypes(&stage_script);
	ScriptSaveGeometries(&stage_script);
	ScriptSaveTriggers(&stage_script);

	return state->DumpGlobals(filename);
}
#endif

void GameStage::RenderStage()
{
	STAGE_GEOM* geom;

	int world_id = KleinGame()->GetWorldview();
	float depth = 0.0f;
	int remain_time = m_WorldSwapTime - KleinGame()->GetSysTickCount();
	if (remain_time < 0)
	{
		// Time up, render final result of swap animation

		// Render other world
		for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
		{
			RenderStageGeom(geom, 1 - world_id, 5.0f);
		}

		// Render current world
		for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
		{
			RenderStageGeom(geom, world_id);
			DebugRenderStageGeom(geom);
		}
	}
	else if (remain_time > TIME_WORLD_SWAP_ANIM / 2)
	{
		// depth: 2.5f ~ 0.0f
		depth = 2.5f * (float)(remain_time - TIME_WORLD_SWAP_ANIM / 2) / (float)(TIME_WORLD_SWAP_ANIM / 2);

		// Render current world
		for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
		{
			RenderStageGeom(geom, world_id, 2.5f + depth);
		}

		// Render other world
		for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
		{
			RenderStageGeom(geom, 1 - world_id, 2.5f - depth);
		}

	}
	else
	{
		// depth: 2.5f ~ 0.0f
		depth = 2.5f * (float)(remain_time) / (float)(TIME_WORLD_SWAP_ANIM / 2);

		// Render other world
		for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
		{
			RenderStageGeom(geom, 1 - world_id, 5.0f - depth);
		}

		// Render current world
		for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
		{
			RenderStageGeom(geom, world_id, depth);
		}
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

	m_TileTypeIndex = 0;
	m_TileName2Id.clear();
	m_TileId2TypeInfo.clear();
}

void GameStage::TestCollision( Actor* actor )
{
	bool result = false;
	Vector2 rel = actor->MoveController() + actor->Velocity();

	std::vector<STAGE_GEOM*> col_group;

	STAGE_GEOM* geom;

	// Collect collision objects
	for (geom = GetFirstStageGeom(); geom != NULL; geom = GetNextStageGeom(geom))
	{
		if ( GetTileUsageById(geom->tile_type_id[KleinGame()->GetWorldview()]) != TILE_USAGE_SOLID )
			continue;

		if (actor->TestCollision(geom->bound, rel))
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
			result |= actor->DoCollisionMove((*iter)->bound, rel_y, &rel_y);
		}

		rel = rel_x + rel_y;

		for ( iter = col_group.begin(); iter != col_group.end(); iter++ )
		{
			result |= actor->DoCollisionMove((*iter)->bound, rel, &rel);
		}
	}

	actor->Translate(rel);
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
	return GetTileUsageById(geom->tile_type_id[KleinGame()->GetWorldview()]);
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

void GameStage::AnimSwapWorlds()
{
	m_WorldSwapTime = KleinGame()->GetSysTickCount() + TIME_WORLD_SWAP_ANIM;
}

void GameStage::ScriptLoadTileTypes( lua_State* state )
{
	lua_pushstring(state, "TileTypes");
	lua_gettable(state, -2);

	// ["TileTypes"]
	if (lua_istable(state, -1))
	{
		//int tiletype_count = lua_objlen(state, -1);
		GWLog_Print("Tile type count: %d\n", lua_objlen(state, -1));

		lua_pushnil(state);

		// Read elements
		while (lua_next(state, -2) != 0)
		{
			if (lua_istable(state, -1))
			{
				lua_pushnil(state);

				const char* lua_str[3] = { NULL };
				int s = 0;

				// Strings
				while (lua_next(state, -2) != 0)
				{
					int t = lua_type(state, -1);
					if (lua_type(state, -1) == LUA_TSTRING)
						lua_str[s] = lua_tostring(state, -1);

					GWLog_Print("Tile type string[%d]: %s\n", s, lua_str[s]);

					s++;
					lua_pop(state, 1);
				}

				// Load tile type info into common container
				std::string tile_name = lua_str[0];

				TILE_TYPE_INFO tile_info;
				tile_info.usage = StringToTileUsage(lua_str[1]);
				const char* tex_name = lua_str[2];

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

			lua_pop(state, 1);
		}
	}

	lua_pop(state, 1);
}

void GameStage::ScriptLoadGeometries( lua_State* state )
{
	lua_pushstring(state, "Geometries");
	lua_gettable(state, -2);

	// ["Geometries"]
	if (lua_istable(state, -1))
	{
		StageGeomList.geom_count = lua_objlen(state, -1);
		GWLog_Print("Total stage geom count: %d\n", StageGeomList.geom_count);

		lua_pushnil(state);

		// Elements
		while (lua_next(state, -2) != 0)
		{
			if (lua_istable(state, -1))
			{
				lua_pushnil(state);

				float lua_num[5];
				const char* lua_str[GAME_WORLD_COUNT];
				int n = 0, s = 0;

				// Strings
				while (lua_next(state, -2) != 0)
				{
					if (lua_isnumber(state, -1))
					{
						lua_num[n] = (float)lua_tonumber(state, -1);
						GWLog_Print("Stage geom num[%d]: %f\n", n, lua_tonumber(state, -1));
						GWLog_Print("Stage geom num[%d]: %f\n", n, lua_num[n]);
						n++;
					}
					else if (lua_isstring(state, -1))
					{
						lua_str[s] = lua_tostring(state, -1);
						GWLog_Print("Stage geom string[%d]: %s\n", s, lua_str[s]);
						s++;
					}

					lua_pop(state, 1);
				}

				BoundBox box;

				int layer = (int)lua_num[0];
				box.xMin = lua_num[1];
				box.xMax = lua_num[2];
				box.yMin = lua_num[3];
				box.yMax = lua_num[4];

				GWLog_Print("\n");

				// In case we don't have enough string
				for (; n<GAME_WORLD_COUNT; n++)
				{
					lua_str[n] = "";
				}

				AddStageGeom(layer, box, lua_str);
			}

			lua_pop(state, 1);
		}
	}

	lua_pop(state, 1);
}

void GameStage::ScriptLoadTriggers( lua_State* state )
{
	lua_pushstring(state, "AreaTriggers");
	lua_gettable(state, -2);

	// ["AreaTriggers"]
	if (lua_istable(state, -1))
	{
		//StageGeomList.geom_count = lua_objlen(state, -1);
		GWLog_Print("Total area trigger count: %d\n", lua_objlen(state, -1));
		lua_pushnil(state);

		// Elements
		while (lua_next(state, -2) != 0)
		{
			if (lua_istable(state, -1))
			{
				lua_pushnil(state);

				float lua_num[5];
				const char* lua_str[1];
				int n = 0, s = 0;

				// Strings
				while (lua_next(state, -2) != 0)
				{
					int t = lua_type(state, -1);
					if (t == LUA_TSTRING)
					{
						lua_str[s] = lua_tostring(state, -1);
						GWLog_Print("Area trigger string[%d]: %s\n", s, lua_str[s]);
						s++;
					}
					else if (t == LUA_TNUMBER)
					{
						lua_num[n] = (float)lua_tonumber(state, -1);
						GWLog_Print("Area trigger num[%d]: %f\n", n, lua_num[n]);
						n++;
					}

					lua_pop(state, 1);
				}

				AREA_TRIGGER* trigger = new AREA_TRIGGER;
				BoundBox box;

				box.xMin = lua_num[0];
				box.xMax = lua_num[1];
				box.yMin = lua_num[2];
				box.yMax = lua_num[3];
				trigger->bound = box;

				trigger->world_id = (int)lua_num[4];

				const char* callback_name = lua_str[0];
				trigger->callback = StringToTriggerFunc(callback_name);

				AddAreaTriggerToGame(trigger);
			}

			lua_pop(state, 1);
		}
	}

	lua_pop(state, 1);
}

#if !defined KLEIN_NO_STAGE_SAVE
void GameStage::ScriptSaveTileTypes( LuaPlus::LuaObject* state )
{
	//LuaObject world_script = stage_script.CreateTable(GameWorldviewKeyWord[0]);
	LuaObject tile_type_script = state->CreateTable("TileTypes");

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
}

void GameStage::ScriptSaveGeometries( LuaPlus::LuaObject* state )
{
	LuaObject geom_group_script = state->CreateTable("Geometries");

	int i = 1;
	std::map<std::string, int>::iterator iter;

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
}

void GameStage::ScriptSaveTriggers( LuaPlus::LuaObject* state )
{
	LuaObject trig_group_script = state->CreateTable("AreaTriggers");

	int i = 1;

	AREA_TRIGGER* trigger = GetFirstAreaTrigger();
	while (trigger)
	{
		LuaObject trig_script = trig_group_script.CreateTable(i);

		trig_script.SetNumber(1, trigger->bound.xMin);
		trig_script.SetNumber(2, trigger->bound.xMax);
		trig_script.SetNumber(3, trigger->bound.yMin);
		trig_script.SetNumber(4, trigger->bound.yMax);
		trig_script.SetNumber(5, trigger->world_id);
		trig_script.SetString(6, TriggerFuncToString(trigger->callback));

		trigger = GetNextAreaTrigger(trigger);
		i++;
	}
}
#endif

void GameStage::RenderStageGeom( STAGE_GEOM* geom, int world_id, float depth/*=0.0f*/ )
{
	if (KleinGame()->TestProtoFeatureBit(PROTO_FEATURE_CIRCLE_OF_TRUE_VIEW))
	{
		Vector2 geom_centre = geom->bound.GetCentrePoint();
		float sqrd_dist = ( geom_centre - KleinGame()->GetPlayerPos() ).SqrdLen();

		// Revert geoms far from player
		if (sqrd_dist > 25) world_id = 1 - world_id;
	}

	int tile_id = geom->tile_type_id[world_id];

	// Don't render tile with no type
	if (tile_id != -1)
	{
		int tex_id = m_TileId2TypeInfo[tile_id].tex_id;

		RenderSystem::DrawSprite(Vector2(geom->bound.xMin, geom->bound.yMin),
								 Vector2(geom->bound.xMax, geom->bound.yMax),
								 tex_id, depth);
	}
}

TileUsageEnum GameStage::GetTileUsageById( int id )
{
	if ( m_TileId2TypeInfo.find(id) != m_TileId2TypeInfo.end() )
	{
		return m_TileId2TypeInfo[id].usage;
	}

	return TILE_USAGE_VOID;
}
