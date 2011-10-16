#ifndef GameStage_h__
#define GameStage_h__

#include "RenderDevice.h"
#include "TextureManager.h"
#include "BoundBox.h"
#include "Character.h"

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
	TILE_USAGE_END,
};

TileUsageEnum StringToTileUsage(const char* type_name);

// -----------------------------------------------------------
// Tile types
// -----------------------------------------------------------
typedef struct TileTypeInfo
{
	char				tile_usage_str[256];
	int					tex_id;
	TileUsageEnum		usage;

	TileTypeInfo& operator=(const TileTypeInfo& rhs)
	{
		strcpy(this->tile_usage_str, rhs.tile_usage_str);
		this->tex_id = rhs.tex_id;
		this->usage = rhs.usage;

		return *this;
	}
} TILE_TYPE_INFO;

// -----------------------------------------------------------
// Stage geometry structure and methods
// -----------------------------------------------------------
typedef struct StageGeom
{
	BoundBox					bound;
	int							tile_type_id;
	LPDIRECT3DVERTEXBUFFER9		vbuffer;
	//TileUsageEnum				usage;

	StageGeom*					next;
} STAGE_GEOM;

enum GameWorldviewEnum
{
	GAME_WORLD_COMMON,
	GAME_WORLD_0,
	GAME_WORLD_1,
	GAME_WORLD_2,
	GAME_WORLD_COUNT,
};

STAGE_GEOM* CreateStageGeom(int world_id);
STAGE_GEOM* GetFirstStageGeom(int world_id);
STAGE_GEOM* GetNextStageGeom(STAGE_GEOM* geom);

struct StageGeomVertex
{
	float x, y, z;
	float u, v;
};

#define StageGeomFVF D3DFVF_XYZ|D3DFVF_TEX1

struct StageGeomWireframeVertex
{
	float x, y, z;
	DWORD color;
};

#define StageGeomWireframeFVF D3DFVF_XYZ|D3DFVF_DIFFUSE

// -----------------------------------------------------------
// Game stage
// -----------------------------------------------------------
class GameStage
{
public:
	GameStage();
	~GameStage();

	bool LoadFromFile(const char* filename);

	// Render entire stage
	void RenderStage();
	void Reset();
	void TestCollision(Character* character, const Vector3& vecRel);

	STAGE_GEOM* GetTileAtPoint(const Vector3& point);
	TileUsageEnum GetStageGeomUsage(STAGE_GEOM* geom);
	void SetWorldview(int world_id);
	GameWorldviewEnum GetWorldview() const { return m_ActiveWorld; }

	STAGE_GEOM* AddStageGeom(int world_id, int layer_id, const BoundBox& bound, const char* tile_type_name);

private:
	
	void ScriptLoadTileTypes(const LuaPlus::LuaObject* script, int world_id);
	void ScriptLoadGeometries(const LuaPlus::LuaObject* script, int world_id);

	void RenderStageGeom(STAGE_GEOM* geom);
	void DebugRenderStageGeom(STAGE_GEOM* geom);

	TileUsageEnum GetTileUsageById(int id);

private:
	TextureManager				m_TextureMgr;
	GameWorldviewEnum			m_ActiveWorld;

	int							m_TileTypeIndex;
	std::map<std::string, int>
								m_TileName2Id;
	std::map<int, TILE_TYPE_INFO>
								m_TileId2TypeInfo;
};

#endif // GameStage_h__