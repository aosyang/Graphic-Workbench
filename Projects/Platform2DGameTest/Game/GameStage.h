#ifndef GameStage_h__
#define GameStage_h__

#include "RenderDevice.h"
#include "TextureManager.h"
#include "BoundBox.h"
#include "Character.h"

struct lua_State;

struct StageGeom
{
	BoundBox	bound;
	int textureID;
	LPDIRECT3DVERTEXBUFFER9	vbuffer;

	StageGeom*	next;
};

StageGeom* CreateStageGeom();
StageGeom* GetFirstStageGeom();
StageGeom* GetNextStageGeom(StageGeom* geom);

struct StageGeomVertex
{
	float x, y, z;
	float u, v;
};

#define StageGeomFVF D3DFVF_XYZ|D3DFVF_TEX1

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

private:
	int							m_GeomCount;
	TextureManager				m_TextureMgr;
};

#endif // GameStage_h__