/********************************************************************
	created:	2011/10/14
	filename: 	GameMain.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "GameMain.h"

#include "Character.h"
#include "GameStage.h"

#include "GWCommon.h"
#include "GameDef.h"

#include "GameStageEditor.h"
#include "Renderer/TextureManager.h"

#include "AreaTrigger.h"

#include <d3dx9.h>

GameMain::GameMain()
: m_GameStage(NULL),
  m_Player(NULL),
  m_IsEditorMode(true),
  m_GameStageEditor(NULL),
  m_ProtoFeatureBits(0),
  m_ActiveWorld((GameWorldviewEnum)0)
{
	memset(&m_InputState, 0, sizeof(m_InputState));

	memset(&m_Camera, 0, sizeof(m_Camera));
	m_Camera.fovy = KLEIN_CAMERA_FOVY;
}

GameMain::~GameMain()
{
}

void GameMain::Reset()
{
	GW_SAFE_DELETE(m_GameStageEditor);
	GW_SAFE_DELETE(m_GameStage);

	Actor* actor;
	while ( actor = GetFirstActor() )
	{
		RemoveActorFromGame( actor );
	}

	AREA_TRIGGER* trigger;
	while ( trigger = GetFirstAreaTrigger() )
	{
		RemoveAreaTriggerFromGame( trigger );
	}

	TextureManager::Instance().Reset();
}

void GameMain::Startup()
{
	m_GameStage = new GameStage;
	m_GameStage->LoadFromFile("Stage.lua");

	// Create player
	m_Player = CreatePlayer();

	m_Camera.dest_actor = m_Player;

	// Create a patient for test
	m_Patient = CreatePatient();
	m_Patient->SetPosition( Vector2(5.0f, 0.0f) );

	m_GameStageEditor = new GameStageEditor;
	m_GameStageEditor->SetGameStage(m_GameStage);
}

void GameMain::Shutdown()
{
	Reset();
}

void GameMain::Update( float delta_time )
{
	STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(m_Player->GetPosition());
	TileUsageEnum player_pos_type = geom ? m_GameStage->GetStageGeomUsage(geom) : TILE_USAGE_VOID;

	Vector2 moveVector(0.0f, 0.0f);

	// Reset player movement control
	m_Player->MoveController() = moveVector;

	GW_KEYBOARD_STATE* key = &m_InputState.key;

	if (m_IsEditorMode)
	{
		// Free move in editor mode
		if (key->pressed[GW_KEY_A]) moveVector += Vector2(-1.0f, 0.0f);
		if (key->pressed[GW_KEY_D]) moveVector += Vector2(1.0f, 0.0f);
		if (key->pressed[GW_KEY_W]) moveVector += Vector2(0.0f, 1.0f);
		if (key->pressed[GW_KEY_S]) moveVector += Vector2(0.0f, -1.0f);

		moveVector.Normalize();
		moveVector *= 0.5f;

		m_Player->Translate( moveVector );
		m_Player->Velocity().y = 0.0f;
	}
	else 
	{
		if (key->pressed[GW_KEY_LEFT]) moveVector += Vector2(-1.0f, 0.0f);
		if (key->pressed[GW_KEY_RIGHT]) moveVector += Vector2(1.0f, 0.0f);

		if (m_Player->IsClimbingLadder())
		{
			if (key->pressed[GW_KEY_UP]) moveVector += Vector2(0.0f, 1.0f);
			if (key->pressed[GW_KEY_DOWN]) moveVector += Vector2(0.0f, -1.0f);

			// Fall down if no ladder
			if (player_pos_type != TILE_USAGE_LADDER)
			{
				m_Player->SetClimbingLadder(false);
			}
		}
		else
		{
			if (key->pressed[GW_KEY_UP])
			{
				// Climb up if player stands near by a ladder
				if (player_pos_type == TILE_USAGE_LADDER)
				{
					m_Player->SetClimbingLadder(true);
				}
				else
				{
					HandlePlayerActorInteractivities();
				}
			}
		}

		moveVector.Normalize();
		if (m_Player->IsClimbingLadder())
		{
			// Slow down if climbing ladder
			moveVector *= 0.08f;
		}
		else
		{
			moveVector *= 0.1f;
		}

		m_Player->MoveController() = moveVector;

		HandlePlayerTriggerInteractivities();
		UpdateActors(delta_time);
	}

	UpdateCamera();

	UpdateEditorControl();

	ClearMouseWheelState();
}

void GameMain::Render()
{
	m_GameStage->RenderStage();

	if (m_IsEditorMode)
	{
		DebugDrawTriggers(m_ActiveWorld);
	}

	// Render actors
	Actor* actor = GetFirstActor();
	while ( actor )
	{
		actor->Render();

		actor = GetNextActor( actor );
	}

	m_Player->Render();

	if (m_IsEditorMode)
	{
		m_GameStageEditor->Render();
	}

	DrawDebugText();
}

void GameMain::SetKeyState( int key_code, bool key_down )
{
	bool old_state = m_InputState.key.pressed[key_code];
	m_InputState.key.pressed[key_code] = key_down;

	if ( key_down!=old_state )
	{
		if (key_down)
			OnKeyPressed(key_code);
		else
			OnKeyReleased(key_code);
	}
}

void GameMain::SetMouseBtnState( GWMouseButton mbtn_code, bool btn_down )
{
	bool old_state = m_InputState.mouse.btn_down[mbtn_code];
	m_InputState.mouse.btn_down[mbtn_code] = btn_down;

	if ( btn_down != old_state )
	{
		if (btn_down)
			OnMouseBtnPressed(mbtn_code);
		else
			OnMouseBtnReleased(mbtn_code);
	}
}

void GameMain::SetMousePosition( int x_pos, int y_pos )
{
	m_InputState.mouse.x = x_pos;
	m_InputState.mouse.y = y_pos;
}

Vector2 GameMain::GetCameraPos() const
{
	return m_Camera.position;
}

Vector2 GameMain::GetPlayerPos() const
{
	return m_Player->GetPosition();
}

void GameMain::GetMousePos( int* x, int* y )
{
	if (x) *x = m_InputState.mouse.x;
	if (y) *y = m_InputState.mouse.y;
}


void GameMain::SetWorldview( int world_id )
{
	m_ActiveWorld = (GameWorldviewEnum)world_id;
}

float GameMain::GetFovy() const
{
	return m_IsEditorMode ? m_GameStageEditor->GetFovy() : KLEIN_CAMERA_FOVY;
}

void GameMain::OnKeyPressed( int key_code )
{
	switch (key_code)
	{
	case GW_KEY_SPACE:
		// Toggle editor mode
		m_IsEditorMode = !m_IsEditorMode;
		break;
	case GW_KEY_1:
		m_GameStageEditor->SetPaintTool(PAINT_TOOL_PENCIL);
		break;
	case GW_KEY_2:
		m_GameStageEditor->SetPaintTool(PAINT_TOOL_BRUSH);
		break;
	case GW_KEY_9:
		SetWorldview(0);
		break;
	case GW_KEY_0:
		SetWorldview(1);
		break;
	case GW_KEY_Z:
		m_Player->Jump();
		break;
	case GW_KEY_C:
		ProtoFeatureFlipBit(PROTO_FEATURE_CIRCLE_OF_TRUE_VIEW);
		break;
	case GW_KEY_P:
		if (m_IsEditorMode)
		{
			m_GameStage->SaveToFile("Stage.lua");
			m_GameStageEditor->MarkMapSaved();
		}
		break;

	}
}

void GameMain::OnKeyReleased( int key_code )
{

}

void GameMain::OnMouseBtnPressed( GWMouseButton mbtn_code )
{
	switch (mbtn_code)
	{
	case MBTN_LEFT:
		if (m_IsEditorMode)
			m_GameStageEditor->StartPainting();
		break;
	}
}

void GameMain::OnMouseBtnReleased( GWMouseButton mbtn_code )
{
	switch (mbtn_code)
	{
	case MBTN_LEFT:
		if (m_IsEditorMode)
			m_GameStageEditor->EndPainting();
		break;
	}
}

void GameMain::ClearMouseWheelState()
{
	m_InputState.mouse.wheel = 0;
}

void GameMain::UpdateEditorControl()
{
	if (!m_IsEditorMode) return;

	GW_MOUSE_STATE* mouse = &m_InputState.mouse;

	if ( mouse->btn_down[MBTN_LEFT] && !mouse->btn_down[MBTN_RIGHT])//Edit by YLL
		//if (m_InputState.mouse_btn_pressed[MBTN_LEFT])
	{
		m_GameStageEditor->PaintTileAtCursor();
	}
	else if ( mouse->btn_down[MBTN_RIGHT] )
	{
		m_GameStageEditor->StartPicking();//Edit by YLL for right click pick mode
		//m_GameStageEditor->PickupTileTypeAtCursor();
	}
	else 
	{
		m_GameStageEditor->EndPicking();//Add by YLL for right click pick mode
	}

	m_GameStageEditor->ZoomView( mouse->wheel / 120 );
}

void GameMain::DrawDebugText()
{
	char debug_text[256];

	// Update debug info
	Vector2 char_pos = m_Player->GetPosition();
	int world_id = (int)GetWorldview();

	GW_MOUSE_STATE* mouse = &m_InputState.mouse;

	// Draw debug text
	sprintf(debug_text,
			"pos: %f, %f\n"
			"Block: x( %d ~ %d ) - y( %d ~ %d )\n"
			"World: %d\n"
			"Mouse: %d %d %d\n"
			"%s",
			char_pos.x, char_pos.y,
			(int)floor(char_pos.x), (int)ceil(char_pos.x),
			(int)floor(char_pos.y), (int)ceil(char_pos.y),
			world_id,
			mouse->x, mouse->y, mouse->wheel,
			m_GameStageEditor->IsMapUnsaved() ? "*Map unsaved*" : "");

	RenderSystem::DrawText(debug_text, 0, 0, 0xFFFFFF00);
}

void GameMain::ProtoFeatureBitSet( int bits, bool val )
{
	if (val)
	{
		m_ProtoFeatureBits |= bits;
	}
	else
	{
		m_ProtoFeatureBits &= ~bits;
	}
}

void GameMain::ProtoFeatureFlipBit( int bits )
{
	m_ProtoFeatureBits ^= bits;
}

bool GameMain::TestProtoFeatureBit( int bits ) const
{
	return (bits & m_ProtoFeatureBits) != 0;
}

void GameMain::HandlePlayerActorInteractivities()
{
	// Interactive with actors
	Actor* actor = GetFirstActor();
	do 
	{
		if ( actor == m_Player ) continue;

		if ( m_Player->TestCollision( actor->GetWorldBoundBox() ) )
			actor->Interactive();

	} while ( actor = GetNextActor( actor ) );
}

void GameMain::UpdateActors( float delta_time )
{
	Actor* actor = GetFirstActor();
	while ( actor )
	{
		actor->Update( delta_time );
		m_GameStage->TestCollision( actor );

		actor = GetNextActor( actor );
	}
}

void GameMain::UpdateCamera()
{
	Vector2 rel;

	// Update camera position
	if (m_Camera.dest_actor)
	{
		rel = m_Camera.dest_actor->GetPosition() - m_Camera.position;
	}
	else
	{
		rel = m_Camera.dest_point - m_Camera.position;
	}

	float dist = sqrtf(rel.SqrdLen());
	if (dist > 0.3f)
		m_Camera.position += rel * dist * 0.05f;
}

void GameMain::HandlePlayerTriggerInteractivities()
{
	AREA_TRIGGER* trigger = GetFirstAreaTrigger();

	while (trigger)
	{
		if (trigger->callback &&
			trigger->world_id == (int)m_ActiveWorld &&
			m_Player->GetWorldBoundBox().TestBoxCollision(trigger->bound))
			(*trigger->callback)(trigger, m_Player);

		trigger = GetNextAreaTrigger(trigger);
	}
}

GameMain* KleinGame()
{
	static GameMain game;
	return &game;
}

