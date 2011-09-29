#ifndef GameStage_h__
#define GameStage_h__

#include "RenderDevice.h"
#include "TextureManager.h"
#include "BoundBox.h"
#include "Character.h"

#include <map>
#include <string>

struct lua_State;

enum TileType
{
	TILE_VOID = -1,

	TILE_SOLID = 0,
	TILE_LADDER,
	TILE_END,
};

struct TileTypeTable
{
	TileType	type;
	char		name[256];
};

// -----------------------------------------------------------
// Tile types
// -----------------------------------------------------------
typedef struct TileTypeInfo
{
	std::string		type;
	int				texID;
} TILE_TYPE_INFO;

// -----------------------------------------------------------
// Stage geometry structure and methods
// -----------------------------------------------------------
typedef struct StageGeom
{
	BoundBox					bound;
	int							textureID;
	LPDIRECT3DVERTEXBUFFER9		vbuffer;
	TileType					type;

	StageGeom*					next;
} STAGE_GEOM;

STAGE_GEOM* CreateStageGeom();
STAGE_GEOM* GetFirstStageGeom();
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

	TileType GetTileTypeAtPoint(const Vector3 point) const;

private:
	void DebugRenderStageGeom(STAGE_GEOM* geom);

private:
	int							m_GeomCount;
	TextureManager				m_TextureMgr;

	std::map<std::string, TILE_TYPE_INFO>
								m_TileTypes;
};

#endif // GameStage_h__