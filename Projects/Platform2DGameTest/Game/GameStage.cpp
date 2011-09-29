#include "GameStage.h"

#include "../DXUT/DXUT.h"
#include <string>

#include "../LuaPlus/LuaPlus.h"

using namespace LuaPlus;

GameStage::GameStage()
: m_GeomCount(0)
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
		LuaObject stage = state->GetGlobals()["Stage"];
		//int layers = stage["Layers"].GetInteger();
		LuaObject geoms = stage["Geometries"];
		m_GeomCount = geoms.GetTableCount();
		for (int i=0; i<m_GeomCount; i++)
		{
			StageGeom geom;
			BoundBox box;

			int layer = geoms[i+1][1].GetInteger();
			box.xMin = geoms[i+1][2].GetInteger();
			box.xMax = geoms[i+1][3].GetInteger();
			box.yMin = geoms[i+1][4].GetInteger();
			box.yMax = geoms[i+1][5].GetInteger();
			const char* tex_name = geoms[i+1][6].GetString();

			// build vertex buffer with vertex position as its own texture coordinate
			Vertex v[6] =
			{
				{ box.xMin, box.yMin, 0.0f, box.xMin, box.yMin },
				{ box.xMin, box.yMax, 0.0f, box.xMin, box.yMax },
				{ box.xMax, box.yMax, 0.0f, box.xMax, box.yMax },

				{ box.xMax, box.yMax, 0.0f, box.xMax, box.yMax },
				{ box.xMax, box.yMin, 0.0f, box.xMax, box.yMin },
				{ box.xMin, box.yMin, 0.0f, box.xMin, box.yMin },
			};

			// Find texture id by name
			int texID = -1;
			if ( (texID=m_TextureMgr.GetTextureID(tex_name)) == -1)
			{
				if (m_TextureMgr.LoadTextureFromFile(tex_name))
					texID = m_TextureMgr.GetTextureID(tex_name);
			}

			geom.bound = box;
			geom.textureID = texID;

			RenderSystem::Device()->CreateVertexBuffer(sizeof(Vertex) * 6, D3DUSAGE_WRITEONLY,
				D3DFVF_XYZ|D3DFVF_TEX1, D3DPOOL_DEFAULT, &geom.vbuffer, NULL);

			void* pData;
			geom.vbuffer->Lock(0, sizeof(Vertex) * 6, (void**)&pData, 0);
			memcpy(pData, v, sizeof(Vertex) * 6);
			geom.vbuffer->Unlock();

			m_StageGeoms.push_back(geom);
		}

		result = true;
	}

	return result;
}

void GameStage::RenderStage()
{
	std::vector<StageGeom>::iterator iter;
	for (iter=m_StageGeoms.begin(); iter!=m_StageGeoms.end(); iter++)
	{
		if (iter->textureID!=-1)
		{
			LPDIRECT3DTEXTURE9 tex = m_TextureMgr.GetD3DTexture(iter->textureID);
			RenderSystem::Device()->SetTexture(0, tex);

			// enable mip-map for texture
			RenderSystem::Device()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}
		else
		{
			RenderSystem::Device()->SetTexture(0, NULL);
		}

		RenderSystem::Device()->SetStreamSource(0, iter->vbuffer, 0, sizeof(Vertex));
		RenderSystem::Device()->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
		RenderSystem::Device()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	}
}

void GameStage::Reset()
{
	m_GeomCount = 0;

	std::vector<StageGeom>::iterator iter;
	for (iter=m_StageGeoms.begin(); iter!=m_StageGeoms.end(); iter++)
	{
		iter->vbuffer->Release();
	}

	m_StageGeoms.clear();
}

void GameStage::TestCollision( Character* character, const Vector3& vecRel )
{
	bool result = false;
	Vector3 rel = vecRel + character->Velocity();

	std::vector<StageGeom>::iterator iter;
	for (iter=m_StageGeoms.begin(); iter!=m_StageGeoms.end(); iter++)
	{
		result |= character->TestCollision(rel, iter->bound);
		//if (result) break;
	}

	character->Translate(rel);
}
//
//void GameStage::GetStageGeoms( lua_State* ls )
//{
//	m_GeomCount = lua_objlen(ls, -1);
//
//	int geom_index = 0;
//
//	lua_pushnil(ls);
//
//	while(lua_next(ls, -2) != 0)
//	{
//		if (lua_istable(ls, -1))
//		{
//			// read info for a single geometry
//			lua_pushnil(ls);
//
//			int n = 0;
//			int layer;
//			std::string tex_name;
//			BoundBox box;
//			StageGeom geom;
//
//			while(lua_next(ls, -2) != 0)
//			{
//				if (n==0)
//					layer = lua_tointeger(ls, -1);
//				else if (n==5)
//					tex_name = lua_tostring(ls, -1);
//				else
//					(&box.xMin)[n-1] = (float)lua_tonumber(ls, -1);
//				n++;
//
//				lua_pop(ls, 1);
//			}
//
//			Vertex v[6] =
//			{
//				{ box.xMin, box.yMin, 0.0f, 0.0f, 1.0f },
//				{ box.xMin, box.yMax, 0.0f, 0.0f, 0.0f },
//				{ box.xMax, box.yMax, 0.0f, 1.0f, 0.0f },
//
//				{ box.xMax, box.yMax, 0.0f, 1.0f, 0.0f },
//				{ box.xMax, box.yMin, 0.0f, 1.0f, 1.0f },
//				{ box.xMin, box.yMin, 0.0f, 0.0f, 1.0f },
//			};
//
//			int texID = -1;
//			if ( (texID=m_TextureMgr.GetTextureID(tex_name.data())) == -1)
//			{
//				if (m_TextureMgr.LoadTextureFromFile(tex_name.data()))
//					texID = m_TextureMgr.GetTextureID(tex_name.data());
//			}
//
//			geom.bound = box;
//			geom.textureID = texID;
//
//			RenderSystem::Device()->CreateVertexBuffer(sizeof(Vertex) * 6, D3DUSAGE_WRITEONLY,
//													   D3DFVF_XYZ|D3DFVF_TEX1, D3DPOOL_DEFAULT, &geom.vbuffer, NULL);
//
//			void* pData;
//			geom.vbuffer->Lock(0, sizeof(Vertex) * 6, (void**)&pData, 0);
//			memcpy(pData, v, sizeof(Vertex) * 6);
//			geom.vbuffer->Unlock();
//
//			m_StageGeoms.push_back(geom);
//
//			geom_index++;
//		}
//
//		lua_pop(ls, 1);
//	}
//
//}
