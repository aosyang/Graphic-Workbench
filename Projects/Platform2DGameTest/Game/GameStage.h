#ifndef GameStage_h__
#define GameStage_h__

#include "RenderDevice.h"
#include "BoundBox.h"
#include "Actor.h"
#include "GameDef.h"

#include <map>
#include <string>


namespace LuaPlus
{
	class LuaObject;
}

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
	int							tile_type_id[GAME_WORLD_COUNT];

	StageGeom*					next;
} STAGE_GEOM;

STAGE_GEOM* CreateStageGeom();
STAGE_GEOM* GetFirstStageGeom();
STAGE_GEOM* GetNextStageGeom(STAGE_GEOM* geom);

void DebugRenderStageGeom(STAGE_GEOM* geom);

// -----------------------------------------------------------
// Game stage
// -----------------------------------------------------------
//Add by YLL
#define TILE_TYPE_INFO_MAP std::map<int, TILE_TYPE_INFO>
class GameStage
{
public:
	GameStage();
	~GameStage();

	bool LoadFromFile(const char* filename);
	bool SaveToFile(const char* filename);

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

private:
	
	void ScriptLoadTileTypes(const LuaPlus::LuaObject* script);
	void ScriptLoadGeometries(const LuaPlus::LuaObject* script);
	void ScriptLoadTriggers(const LuaPlus::LuaObject* script);

	void ScriptSaveGeometries( LuaPlus::LuaObject* script );
	void ScriptSaveTileTypes( LuaPlus::LuaObject* script );
	void ScriptSaveTriggers( LuaPlus::LuaObject* script );

	void RenderStageGeom(STAGE_GEOM* geom);

	TileUsageEnum GetTileUsageById(int id);
private:
	int							m_TileTypeIndex;
	std::map<std::string, int>
								m_TileName2Id;
	//Edit by YLL //std::map<int, TILE_TYPE_INFO>
	TILE_TYPE_INFO_MAP
								m_TileId2TypeInfo;
};

#endif // GameStage_h__