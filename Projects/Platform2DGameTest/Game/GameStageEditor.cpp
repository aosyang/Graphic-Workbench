/********************************************************************
	created:	2011/10/16
	filename: 	GameStageEditor.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "GameStageEditor.h"

#include "GameDef.h"
#include "GameStage.h"
#include "GameMain.h"
#include "AreaTrigger.h"

#include <d3dx9.h>

GameStageEditor::GameStageEditor()
: m_GameStage(NULL),
  m_bPicking(false),
  m_PopupMenuPos(0.0f, 0.0f),
  m_PopupMenuScreenPosX(0),
  m_PopupMenuScreenPosY(0),
  m_RemoveTrigger(false),
  m_ToolBoxSelectedTriggerNameIndex(-1),
  m_SelectedAreaTrigger(NULL),
  m_Fovy(GW_MATH_PI / 3),
  m_PaintTool(PAINT_TOOL_PENCIL),
  m_ToolState(TOOL_STATE_IDLE),
  m_MapUnsaved(false)
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

	if ( m_bPicking )
	{
		switch (m_PaintTool)
		{
		case PAINT_TOOL_PENCIL:
			{
				TILE_TYPE_INFO_MAP& mapTileType = m_GameStage->GetTileTypeInfo();
				int nTileType = mapTileType.size();

				int nTileLayoutWidth = nTileType > 5 ? 5 : nTileType;
				Vector2 vec_min, vec_max;

				if ( nTileType )
				{
					// Draw background of tile picking tool box
					RenderSystem::DrawColoredSprite(vec_min - Vector2(0.3f, 0.3f),
						vec_max + Vector2(0.3f, 0.3f),
						GWColor(0.0f, 0.749f, 0.5f, 0.749f));
				}

				for ( int i = 0; i < nTileType; ++i)
				{
					float xBias = float(i % nTileLayoutWidth - (nTileLayoutWidth - 1) / 2);
					float yBias = float(i / nTileLayoutWidth ) + 1.0f;

					RenderSystem::DrawSprite(Vector2(floorf(m_PopupMenuPos.x) + xBias, floorf(m_PopupMenuPos.y)+ yBias),
						Vector2(ceilf(m_PopupMenuPos.x)  + xBias, ceilf(m_PopupMenuPos.y) + yBias),
						mapTileType[i].tex_id);
				}
			}
			break;
		case PAINT_TOOL_BRUSH:
			{
				// Draw trigger names
				char text[256];

				TriggerFuncTable* table = GetTriggerFuncTable();

				m_ToolBoxSelectedTriggerNameIndex = -1;
				m_RemoveTrigger = false;

				// Delete area trigger
				if (mouse_ypos > m_PopupMenuScreenPosY + KLEIN_FONT_HEIGHT * -2 &&
					mouse_ypos < m_PopupMenuScreenPosY + KLEIN_FONT_HEIGHT * -1 &&
					mouse_xpos > m_PopupMenuScreenPosX)
				{
					sprintf(text, "Remove <");
					m_RemoveTrigger = true;
				}
				else
				{
					sprintf(text, "Remove");
				}
				RenderSystem::RenderText(text, m_PopupMenuScreenPosX,
										m_PopupMenuScreenPosY + KLEIN_FONT_HEIGHT * -2);

				for (int i=0; table[i].callback; i++)
				{
					if (mouse_ypos > m_PopupMenuScreenPosY + KLEIN_FONT_HEIGHT * i &&
						mouse_ypos < m_PopupMenuScreenPosY + KLEIN_FONT_HEIGHT * (i + 1) &&
						mouse_xpos > m_PopupMenuScreenPosX)
					{
						sprintf(text, "%s <", table[i].name);
						m_ToolBoxSelectedTriggerNameIndex = i;
					}
					else
					{
						sprintf(text, "%s", table[i].name);
					}

					RenderSystem::RenderText(text, m_PopupMenuScreenPosX,
											m_PopupMenuScreenPosY + KLEIN_FONT_HEIGHT * i);
				}
			}
			break;
		default:
			break;
		}

	}

	GWColor color;
	switch (m_PaintTool)
	{
	case PAINT_TOOL_PENCIL:
		color = GWColor(1.0f, 0.949f, 0.0f);

		// Draw cursor tile rect
		RenderSystem::DrawWireframeRect(Vector2(floorf(tile_pos.x), floorf(tile_pos.y)),
										Vector2(ceilf(tile_pos.x), ceilf(tile_pos.y)),
										color);

		break;
	case PAINT_TOOL_BRUSH:
		{
			color = GWColor(0.0f, 0.9f, 1.0f);

			if (m_ToolState == TOOL_STATE_DRAWING)
			{
				Vector2 vMin(min( m_SelectedArea.xMin, floorf(tile_pos.x) ),
							 min( m_SelectedArea.yMin, floorf(tile_pos.y) ) );
				Vector2 vMax(max( m_SelectedArea.xMax, ceilf(tile_pos.x) ),
							 max( m_SelectedArea.yMax, ceilf(tile_pos.y) ) );

				RenderSystem::DrawWireframeRect(vMin, vMax, color);
			}
			else
			{
				if (!m_SelectedAreaTrigger)
				{
					// Draw selected area
					RenderSystem::DrawWireframeRect(m_SelectedArea.vMin(),
													m_SelectedArea.vMax(),
													color);
				}

				if (m_SelectedAreaTrigger)
				{
					// Draw selected trigger
					RenderSystem::DrawWireframeRect(m_SelectedAreaTrigger->bound.vMin(),
						m_SelectedAreaTrigger->bound.vMax(),
						color);
				}

				if ( !m_bPicking )
				{
					AREA_TRIGGER* trigger = PickAreaTriggerAtPoint(tile_pos, KleinGame()->GetWorldview());

					if (trigger)
					{
						RenderSystem::DrawWireframeRect(trigger->bound.vMin(),
							trigger->bound.vMax(),
							color);
					}
					else
					{
						// Draw cursor tile rect
						RenderSystem::DrawWireframeRect(Vector2(floorf(tile_pos.x), floorf(tile_pos.y)),
														Vector2(ceilf(tile_pos.x), ceilf(tile_pos.y)),
														color);
					}

				}
			}

		}
		break;
	default:
		break;
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

		int mouse_xpos, mouse_ypos;
		KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);
		Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

		if ( m_bPicking )
		{
			// start picking stuff

			m_PopupMenuScreenPosX = mouse_xpos;
			m_PopupMenuScreenPosY = mouse_ypos;
			m_PopupMenuPos = tile_pos;
		}
		else
		{
			// stop picking stuff
			TILE_TYPE_INFO_MAP& mapTileType = m_GameStage->GetTileTypeInfo();
			int nTileType = mapTileType.size();

			int nTileLayoutWidth = nTileType > 5 ? 5 : nTileType;
			int i;
			for ( i = 0; i < nTileType; ++i)
			{
				float xBias = float(i % nTileLayoutWidth - (nTileLayoutWidth - 1) / 2);
				float yBias = float(i / nTileLayoutWidth ) + 1.0f;

				if ( tile_pos.x >= floorf(m_PopupMenuPos.x) + xBias &&
					 tile_pos.x <= ceilf(m_PopupMenuPos.x)  + xBias &&
					 tile_pos.y >= floorf(m_PopupMenuPos.y) + yBias &&
					 tile_pos.y <= ceilf(m_PopupMenuPos.y) + yBias )
				{
					m_TileTypeToPaint = m_GameStage->GetTileNameById(i);
					break;
				}
			}

			// Nothing picked, pick null
			if ( i == nTileType )
			{
				m_TileTypeToPaint = "";
			}

			// Fill selected area with tile just picked
			if ( m_PaintTool == PAINT_TOOL_BRUSH )
			{
				if ( m_ToolBoxSelectedTriggerNameIndex != -1 )
				{
					if (m_SelectedAreaTrigger)
					{
						// Pick up new function for selected trigger
						m_SelectedAreaTrigger->callback = NULL;
						if ( GetTriggerFuncTable()[m_ToolBoxSelectedTriggerNameIndex].callback )
						{
							m_SelectedAreaTrigger->callback = GetTriggerFuncTable()[m_ToolBoxSelectedTriggerNameIndex].callback;
						}
					}
					else
					{
						// Create new trigger with selected area
						AREA_TRIGGER* trigger = new AREA_TRIGGER;

						trigger->bound = m_SelectedArea;
						trigger->world_id = KleinGame()->GetWorldview();
						trigger->callback = NULL;
						trigger->next = NULL;

						if ( GetTriggerFuncTable()[m_ToolBoxSelectedTriggerNameIndex].callback )
						{
							trigger->callback = GetTriggerFuncTable()[m_ToolBoxSelectedTriggerNameIndex].callback;
						}

						AddAreaTriggerToGame(trigger);

						m_SelectedAreaTrigger = trigger;
					}

					MarkMapUnsaved();
				}
				else
				{
					if (m_SelectedAreaTrigger && m_RemoveTrigger)
					{
						RemoveAreaTriggerFromGame(m_SelectedAreaTrigger);
						m_SelectedAreaTrigger = NULL;

						MarkMapUnsaved();
					}
				}
			}
		}
	}
}

void GameStageEditor::PaintTileAtCursor()
{
	if (!m_GameStage) return;

	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	switch (m_PaintTool)
	{
	case PAINT_TOOL_PENCIL:
		{

			STAGE_GEOM* geom = m_GameStage->GetTileAtPoint( tile_pos );

			// No geom found at point, create a new one
			if (!geom)
			{
				BoundBox box(floorf(tile_pos.x), floorf(tile_pos.y), ceilf(tile_pos.x), ceilf(tile_pos.y));

				const char* tile_type_name[GAME_WORLD_COUNT];
				for (int i=0; i<GAME_WORLD_COUNT; i++)
					tile_type_name[i] = "";

				tile_type_name[KleinGame()->GetWorldview()] = m_TileTypeToPaint.c_str();
				m_GameStage->AddStageGeom(0, box, tile_type_name);
			}
			else
			{
				geom->tile_type_id[KleinGame()->GetWorldview()] = m_GameStage->GetTileIdByName(m_TileTypeToPaint.c_str());
			}

			MarkMapUnsaved();
		}
		break;
	default:
		break;
	}
}

void GameStageEditor::PickupTileTypeAtCursor()
{
	if (!m_GameStage) return;

	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	STAGE_GEOM* geom = m_GameStage->GetTileAtPoint( tile_pos );
	if (geom)
	{
		const char* tile_name = m_GameStage->GetTileNameById(geom->tile_type_id[KleinGame()->GetWorldview()]);

		if (tile_name)
		{
			m_TileTypeToPaint = tile_name;
			return;
		}
	}

	m_TileTypeToPaint = "";
}

void GameStageEditor::ZoomView( int zoom )
{
	if (zoom == 0) return;

	float height = tanf(m_Fovy / 2.0f) * abs(KLEIN_CAMERA_ZPOS);

	for (int i=0; i<abs(zoom); i++)
	{
		if (zoom > 0)
			height /= 1.5f;
		else
			height *= 1.5f;
	}

	m_Fovy = atanf( height / abs(KLEIN_CAMERA_ZPOS) ) * 2.0f;
}

Vector2 GameStageEditor::CursorToTilePos( int x_pos, int y_pos )
{
	Vector2 cam_pos = KleinGame()->GetCameraPos();

	// Render tile position at cursor
	float height = tanf(KleinGame()->GetFovy() / 2.0f) * abs(KLEIN_CAMERA_ZPOS);
	float width = height * KLEIN_SCREEN_ASPECT;

	float step_x = (float)KLEIN_SCREEN_WIDTH / (width * 2);
	float tile_x = ((float)x_pos - (float)KLEIN_SCREEN_WIDTH * 0.5f)  / step_x;

	float step_y = (float)KLEIN_SCREEN_HEIGHT / (height * 2);
	float tile_y = -((float)y_pos - (float)KLEIN_SCREEN_HEIGHT * 0.5f)  / step_y;

	tile_x += cam_pos.x;
	tile_y += cam_pos.y;

	return Vector2(tile_x, tile_y);
}

void GameStageEditor::StartPainting()
{
	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	switch (m_PaintTool)
	{
	case PAINT_TOOL_BRUSH:
		m_SelectedArea.xMin = floorf(tile_pos.x);
		m_SelectedArea.xMax = ceilf(tile_pos.x);
		m_SelectedArea.yMin = floorf(tile_pos.y);
		m_SelectedArea.yMax = ceilf(tile_pos.y);

		break;
	default:
		break;
	}

	m_ToolState = TOOL_STATE_DRAWING;
}

void GameStageEditor::EndPainting()
{
	int mouse_xpos, mouse_ypos;
	KleinGame()->GetMousePos(&mouse_xpos, &mouse_ypos);

	Vector2 tile_pos = CursorToTilePos(mouse_xpos, mouse_ypos);

	switch (m_PaintTool)
	{
	case PAINT_TOOL_BRUSH:
		{
			m_SelectedAreaTrigger = PickAreaTriggerAtPoint(tile_pos, KleinGame()->GetWorldview());

			if (!m_SelectedAreaTrigger)
			{
				// Update selected area
				m_SelectedArea.xMin = min( m_SelectedArea.xMin, floorf(tile_pos.x) );
				m_SelectedArea.xMax = max( m_SelectedArea.xMax, ceilf(tile_pos.x) );
				m_SelectedArea.yMin = min( m_SelectedArea.yMin, floorf(tile_pos.y) );
				m_SelectedArea.yMax = max( m_SelectedArea.yMax, ceilf(tile_pos.y) );
			}
		}

		break;
	default:
		break;
	}

	m_ToolState = TOOL_STATE_IDLE;
}
