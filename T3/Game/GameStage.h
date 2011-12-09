/********************************************************************
	created:	2011/9/5
	filename: 	GameStage.h
	author:		Mwolf
	
	purpose:	Definitions for stage structure of game
*********************************************************************/
#ifndef GameStage_h__
#define GameStage_h__

#include "Renderer/GWRenderDevice.h"
#include "BoundBox.h"
#include "Actor.h"
#include "GameDef.h"
#include "GWTypes.h"

#include <map>
#include <string>

struct lua_State;

static const float GAME_STAGE_LAYER_DIST = 5.0f;

enum TileUsageEnum
{
	TILE_USAGE_VOID = -1,

	TILE_USAGE_SOLID = 0,
	TILE_USAGE_LADDER,
	TILE_USAGE_TRIGGER_HURT,
	TILE_USAGE_END,
};

TileUsageEnum StringToTileUsage(const char* type_name);
const char* TileUsageToString(TileUsageEnum usage);

// -----------------------------------------------------------
// Tile types
// -----------------------------------------------------------
typedef struct TileTypeInfo
{
	int					tex_id;
	TileUsageEnum		usage;
} TILE_TYPE_INFO;

// -----------------------------------------------------------
// Stage geometry structure and methods
// -----------------------------------------------------------
typedef struct StageGeom
{
	BoundBox					bound;
	BoundBox					post_proj_bound;
	BoundBox					post_proj_bound_last_frame;					// For morph collision test
	int							tile_type_id[GAME_WORLD_COUNT];

	float						depth;

	STAGE_GEOM*					next;
} STAGE_GEOM;

STAGE_GEOM* CreateStageGeom();
STAGE_GEOM* GetFirstStageGeom();
STAGE_GEOM* GetNextStageGeom(STAGE_GEOM* geom);
int GetStageGeomCount();

void DebugRenderStageGeom(STAGE_GEOM* geom);

//Add by YLL
typedef std::map<int, TILE_TYPE_INFO> TILE_TYPE_INFO_MAP;

// -----------------------------------------------------------
// Game stage
// -----------------------------------------------------------

class GameStage
{
public:
	GameStage();
	~GameStage();

	bool LoadFromFile(const char* filename);
#if !defined KLEIN_NO_STAGE_SAVE
	bool SaveToFile(const char* filename);
#endif

	// Render entire stage
	void RenderStage();
	void Reset();
	void TestCollision(Actor* character);

	TILE_TYPE_INFO_MAP& GetTileTypeInfo(){ return m_TileId2TypeInfo; }//Add by YLL for stage editor
	STAGE_GEOM* GetTileAtPoint(const Vector2& point);
	TileUsageEnum GetStageGeomUsage(STAGE_GEOM* geom);
	int GetTileIdByName(const char* tile_name) const;

	const char* GetTileNameById(int tile_id) const;

	STAGE_GEOM* AddStageGeom(int layer_id, const BoundBox& bound, const char* tile_type_name[GAME_WORLD_COUNT]);

	void AnimSwapWorlds();
private:
	
	void ScriptLoadTileTypes(lua_State* state);
	void ScriptLoadGeometries(lua_State* state);
	void ScriptLoadTriggers(lua_State* state);

#if !defined KLEIN_NO_STAGE_SAVE
	void ScriptSaveGeometries( FILE* fp );
	void ScriptSaveTileTypes( FILE* fp );
	void ScriptSaveTriggers( FILE* fp );

#endif

	void RenderStageGeom(STAGE_GEOM* geom, int world_id, float depth=0.0f);

	TileUsageEnum GetTileUsageById(int id);
private:
	int							m_TileTypeIndex;
	std::map<std::string, int>
								m_TileName2Id;
	//Edit by YLL //std::map<int, TILE_TYPE_INFO>
	TILE_TYPE_INFO_MAP			m_TileId2TypeInfo;

	GW_UINT32					m_WorldSwapTime;
};

#endif // GameStage_h__