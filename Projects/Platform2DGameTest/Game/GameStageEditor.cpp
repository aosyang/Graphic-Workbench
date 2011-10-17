#include "GameStageEditor.h"

#include "GameDef.h"
#include "GameStage.h"
#include "GameMain.h"

#include <d3dx9.h>

GameStageEditor::GameStageEditor()
: m_GameStage(NULL)
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

	STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(Vector3(tile_pos, 0.0f));
	if (geom)
	{
		DebugRenderStageGeom(geom);
	}
	else
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
	}

}

void GameStageEditor::SetGameStage( GameStage* stage )
{
	m_GameStage = stage;
}

void GameStageEditor::PaintTileAtCursor()
{
	if (!m_GameStage) return;

	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(Vector3(tile_pos, 0.0f));
	if (!geom)
	{
		BoundBox box(floorf(tile_pos.x), floorf(tile_pos.y), ceilf(tile_pos.x), ceilf(tile_pos.y));
		m_GameStage->AddStageGeom(GAME_WORLD_COMMON, 0, box, m_TileTypeToPaint.c_str());
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
		m_TileTypeToPaint = m_GameStage->GetTileNameById(geom->tile_type_id);
	}
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
