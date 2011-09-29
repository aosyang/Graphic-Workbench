#ifndef GameStage_h__
#define GameStage_h__

#include "RenderDevice.h"
#include "TextureManager.h"
#include "BoundBox.h"
#include "Character.h"

#include <vector>

struct lua_State;

struct StageGeom
{
	BoundBox	bound;
	int textureID;
	LPDIRECT3DVERTEXBUFFER9	vbuffer;
};

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

	//void GetStageGeoms(lua_State* ls);

	struct Vertex
	{
		float x, y, z;
		float u, v;
	};

	int							m_GeomCount;

	std::vector<StageGeom>		m_StageGeoms;

	TextureManager				m_TextureMgr;
};

#endif // GameStage_h__