#include "GameStageEditor.h"

#include "GameDef.h"
#include "GameStage.h"
#include "GameMain.h"

#include <d3dx9.h>

GameStageEditor::GameStageEditor()
: m_GameStage(NULL),m_bPicking(false)
{

}

GameStageEditor::~GameStageEditor()
{

}

void GameStageEditor::Render()
{
	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	RenderSystem::Device()->SetTransform( D3DTS_WORLD, &matWorld );

	//STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(Vector3(tile_pos, 0.0f));
	//if (geom)
	//{
	//	DebugRenderStageGeom(geom);
	//}
	//else
	if ( m_bPicking )
	{
		StageGeomWireframeVertex v[6] =
		{
			{ floorf(tile_pos.x), floorf(tile_pos.y), 0.0f, 0xFFFFF200 },
			{ floorf(tile_pos.x), ceilf(tile_pos.y), 0.0f, 0xFFFFF200 },
			{ ceilf(tile_pos.x), ceilf(tile_pos.y), 0.0f, 0xFFFFF200 },

			{ ceilf(tile_pos.x), ceilf(tile_pos.y), 0.0f, 0xFFFFF200 },
			{ ceilf(tile_pos.x), floorf(tile_pos.y), 0.0f, 0xFFFFF200 },
			{ floorf(tile_pos.x), floorf(tile_pos.y), 0.0f, 0xFFFFF200 },
		};

		RenderSystem::Device()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		RenderSystem::Device()->SetTexture(0, NULL);
		RenderSystem::Device()->SetFVF(StageGeomWireframeFVF);
		RenderSystem::Device()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof(StageGeomWireframeVertex));

		TILE_TYPE_INFO_MAP& mapTileType = m_GameStage->GetTileTypeInfo();
		int nTileType = mapTileType.size();

		LPDIRECT3DVERTEXBUFFER9 vbuffer;
		RenderSystem::Device()->CreateVertexBuffer(sizeof(StageGeomVertex) * 6, D3DUSAGE_WRITEONLY,
			D3DFVF_XYZ|D3DFVF_TEX1, D3DPOOL_DEFAULT, &vbuffer, NULL);

		int nTileLayoutWidth = nTileType > 5 ? 5 : nTileType;
		for ( int i = 0; i < nTileType; ++i)
		{
			
			float xBias = float(i % nTileLayoutWidth - (nTileLayoutWidth - 1) / 2);
			float yBias = float(i / nTileLayoutWidth ) + 1.0f;

			StageGeomVertex v[6] =
			{
				{ floorf(tile_pos.x) + xBias, floorf(tile_pos.y)+ yBias, 0.0f, 0.0f, 0.0f },
				{ floorf(tile_pos.x) + xBias, ceilf(tile_pos.y) + yBias, 0.0f, 0.0f, 1.0f },
				{ ceilf(tile_pos.x)  + xBias, ceilf(tile_pos.y) + yBias, 0.0f, 1.0f, 1.0f },

				{ ceilf(tile_pos.x) + xBias, ceilf(tile_pos.y)  + yBias, 0.0f, 1.0f, 1.0f },
				{ ceilf(tile_pos.x) + xBias, floorf(tile_pos.y) + yBias, 0.0f, 1.0f, 0.0f },
				{ floorf(tile_pos.x)+ xBias, floorf(tile_pos.y) + yBias, 0.0f, 0.0f, 0.0f },
			};
			void* pData;
			vbuffer->Lock(0, sizeof(StageGeomVertex) * 6, (void**)&pData, 0);
			memcpy(pData, v, sizeof(StageGeomVertex) * 6);
			vbuffer->Unlock();
			
			LPDIRECT3DTEXTURE9 tex = m_GameStage->GetTextureManager().GetD3DTexture(mapTileType[i].tex_id);
			RenderSystem::Device()->SetTexture(0, tex);
			RenderSystem::Device()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			RenderSystem::Device()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			RenderSystem::Device()->SetStreamSource(0, vbuffer, 0, sizeof(StageGeomVertex));
			RenderSystem::Device()->SetFVF(StageGeomFVF);
			RenderSystem::Device()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		}
		vbuffer->Unlock();
		vbuffer->Release();
	}

}

void GameStageEditor::SetGameStage( GameStage* stage )
{
	m_GameStage = stage;

}


void GameStageEditor::StartPicking( bool bStart )
{
	if ( m_bPicking != bStart )
	{
		m_bPicking = bStart;
		if ( m_bPicking )
		{
			//TODO:start picking stuff
		}
		else
		{
			//TODO:stop picking stuff
		}
	}
}

void GameStageEditor::PaintTileAtCursor()
{
	if (!m_GameStage) return;

	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(Vector3(tile_pos, 0.0f));

	// No geom found at point, create a new one
	if (!geom)
	{
		BoundBox box(floorf(tile_pos.x), floorf(tile_pos.y), ceilf(tile_pos.x), ceilf(tile_pos.y));

		const char* tile_type_name[GAME_WORLD_COUNT];
		for (int i=0; i<GAME_WORLD_COUNT; i++)
			tile_type_name[i] = "";

		tile_type_name[m_GameStage->GetWorldview()] = m_TileTypeToPaint.c_str();
		m_GameStage->AddStageGeom(0, box, tile_type_name);
	}
	else
	{
		geom->tile_type_id[m_GameStage->GetWorldview()] = m_GameStage->GetTileIdByName(m_TileTypeToPaint.c_str());
	}
}

void GameStageEditor::PickupTileTypeAtCursor()
{
	if (!m_GameStage) return;

	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(Vector3(tile_pos, 0.0f));
	if (geom)
	{
		const char* tile_name = m_GameStage->GetTileNameById(geom->tile_type_id[m_GameStage->GetWorldview()]);

		if (tile_name)
		{
			m_TileTypeToPaint = tile_name;
			return;
		}
	}

	m_TileTypeToPaint = "";
}

Vector2 GameStageEditor::CursorToTilePos( int x_pos, int y_pos )
{
	Vector3 cam_pos = KleinGame()->GetCameraPos();

	// Render tile position at cursor
	float height = tanf(KLEIN_CAMERA_FOVY / 2.0f) * abs(KLEIN_CAMERA_ZPOS);
	float width = height * KLEIN_SCREEN_ASPECT;

	float step_x = (float)KLEIN_SCREEN_WIDTH / (width * 2);
	float tile_x = ((float)x_pos - (float)KLEIN_SCREEN_WIDTH * 0.5f)  / step_x;

	float step_y = (float)KLEIN_SCREEN_HEIGHT / (height * 2);
	float tile_y = -((float)y_pos - (float)KLEIN_SCREEN_HEIGHT * 0.5f)  / step_y;

	tile_x += cam_pos.x;
	tile_y += cam_pos.y;

	return Vector2(tile_x, tile_y);
}
