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

enum TileTypeEnum
{
	TILE_VOID = -1,

	TILE_SOLID = 0,
	TILE_LADDER,
	TILE_END,
};

TileTypeEnum StringToTileType(const char* type_name);

// -----------------------------------------------------------
// Tile types
// -----------------------------------------------------------
typedef struct TileTypeInfo
{
	char		into_type_name[256];
	int			tex_id;
} TILE_TYPE_INFO;

// -----------------------------------------------------------
// Stage geometry structure and methods
// -----------------------------------------------------------
typedef struct StageGeom
{
	BoundBox					bound;
	int							texture_id;
	LPDIRECT3DVERTEXBUFFER9		vbuffer;
	TileTypeEnum				type;

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

	TileTypeEnum GetTileTypeAtPoint(const Vector3 point) const;

private:
	
	void ScriptLoadTileTypes(const LuaPlus::LuaObject* script, int world_id);
	void ScriptLoadGeometries(const LuaPlus::LuaObject* script, int world_id);

	void DebugRenderStageGeom(STAGE_GEOM* geom);

private:
	TextureManager				m_TextureMgr;
};

#endif // GameStage_h__