#include "GameMain.h"

#include "Character.h"
#include "GameStage.h"

#include "GWCommon.h"
#include "GameDef.h"

#include "GameStageEditor.h"
#include "TextureManager.h"

#include <d3dx9.h>

GameMain::GameMain()
: m_GameStage(NULL),
  m_Player(NULL),
  m_IsEditorMode(true),
  m_GameStageEditor(NULL),
  m_MousePosX(0),
  m_MousePosY(0),
  m_CameraPos(Vector3::ZERO),
  m_ProtoFeatureBits(0)
{
	for (int i=0; i<0xFF; i++)
		m_KeyPressed[i] = false;

	for (int i=0; i<MBTN_COUNT; i++)
		m_MBtnPressed[i] = false;

	memset(m_DebugText, 0, sizeof(m_DebugText));
}

GameMain::~GameMain()
{
}

void GameMain::Reset()
{
	GW_SAFE_DELETE(m_GameStageEditor);
	GW_SAFE_DELETE(m_GameStage);
	GW_SAFE_DELETE(m_Player);

	TextureManager::Instance().Reset();
}

void GameMain::Startup()
{
	m_GameStage = new GameStage;
	m_GameStage->LoadFromFile("Stage.lua");

	m_Player = new Character;

	m_GameStageEditor = new GameStageEditor;
	m_GameStageEditor->SetGameStage(m_GameStage);
}

void GameMain::Shutdown()
{
	Reset();
}

void GameMain::Update( float delta_time )
{
	STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(m_Player->WorldPosition());
	TileUsageEnum player_pos_type = geom ? m_GameStage->GetStageGeomUsage(geom) : TILE_USAGE_VOID;

	Vector2 moveVector(0.0f, 0.0f);

	if (m_IsEditorMode)
	{
		// Free move in editor mode
		if (m_KeyPressed[GW_KEY_A]) moveVector += Vector2(-1.0f, 0.0f);
		if (m_KeyPressed[GW_KEY_D]) moveVector += Vector2(1.0f, 0.0f);
		if (m_KeyPressed[GW_KEY_W]) moveVector += Vector2(0.0f, 1.0f);
		if (m_KeyPressed[GW_KEY_S]) moveVector += Vector2(0.0f, -1.0f);

		moveVector.Normalize();
		moveVector *= 0.5f;

		m_Player->Translate(Vector3(moveVector, 0.0f));
		m_Player->Velocity().y = 0.0f;
	}
	else 
	{
		if (m_KeyPressed[GW_KEY_LEFT]) moveVector += Vector2(-1.0f, 0.0f);
		if (m_KeyPressed[GW_KEY_RIGHT]) moveVector += Vector2(1.0f, 0.0f);

		if (m_Player->IsClimbingLadder())
		{
			if (m_KeyPressed[GW_KEY_UP]) moveVector += Vector2(0.0f, 1.0f);
			if (m_KeyPressed[GW_KEY_DOWN]) moveVector += Vector2(0.0f, -1.0f);

			// Fall down if no ladder
			if (player_pos_type != TILE_USAGE_LADDER)
			{
				m_Player->SetClimbingLadder(false);
			}
		}
		else
		{
			// Climb up if player stands near by a ladder
			if (m_KeyPressed[GW_KEY_UP] && player_pos_type == TILE_USAGE_LADDER)
			{
				m_Player->SetClimbingLadder(true);
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

		m_Player->Update(delta_time);
		m_GameStage->TestCollision( m_Player, Vector3(moveVector, 0.0f) );
	}

	// Update camera position
	Vector3 rel = m_Player->WorldPosition() - m_CameraPos;
	float dist = sqrtf(rel.SqrdLen());
	if (dist > 0.3f)
		m_CameraPos += rel * dist * 0.05f;

	if (m_IsEditorMode)
	{	
		if ( m_MBtnPressed[MBTN_LEFT] && !m_MBtnPressed[MBTN_RIGHT])//Edit by YLL
		//if (m_MBtnPressed[MBTN_LEFT])
		{
			m_GameStageEditor->PaintTileAtCursor();
		}
		else if ( m_MBtnPressed[MBTN_RIGHT] )
		{
			m_GameStageEditor->StartPicking();//Edit by YLL for right click pick mode
			//m_GameStageEditor->PickupTileTypeAtCursor();
		}
		else 
		{
			m_GameStageEditor->EndPicking();//Add by YLL for right click pick mode
		}
	}

	UpdateDebugText();
}

void GameMain::Render()
{
	m_GameStage->RenderStage();

	m_Player->Render();

	if (m_IsEditorMode)
	{
		m_GameStageEditor->Render();
	}
}

void GameMain::SetKeyState( int key_code, bool key_down )
{
	bool old_state = m_KeyPressed[key_code];
	m_KeyPressed[key_code] = key_down;

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
	bool old_state = m_MBtnPressed[mbtn_code];
	m_MBtnPressed[mbtn_code] = btn_down;

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
	m_MousePosX = x_pos;
	m_MousePosY = y_pos;
}

Vector3 GameMain::GetCameraPos() const
{
	return m_CameraPos;
}

Vector3 GameMain::GetPlayerPos() const
{
	return m_Player->WorldPosition();
}

void GameMain::GetMousePos( int* x, int* y )
{
	if (x) *x = m_MousePosX;
	if (y) *y = m_MousePosY;
}

const char* GameMain::GetDebugText() const
{
	return m_DebugText;
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
	case GW_KEY_2:
	//case GW_KEY_3:
	//case GW_KEY_4:
		m_GameStage->SetWorldview(key_code - GW_KEY_1);
		break;
	case GW_KEY_Z:
		m_Player->Jump();
		break;
	case GW_KEY_C:
		ProtoFeatureFlipBit(PROTO_FEATURE_CIRCLE_OF_TRUE_VIEW);
		break;
	case GW_KEY_P:
		if (m_IsEditorMode)
			m_GameStage->SaveToFile("Stage.lua");
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
			m_GameStageEditor->PaintTileAtCursor();
		break;
	}
}

void GameMain::OnMouseBtnReleased( GWMouseButton mbtn_code )
{

}

void GameMain::UpdateDebugText()
{
	// Update debug info
	Vector3 char_pos = m_Player->WorldPosition();
	int world_id = (int)m_GameStage->GetWorldview();

	// Draw debug text
	sprintf(m_DebugText,
			"pos: %f, %f\n"
			"Block: x( %d ~ %d ) - y( %d ~ %d )\n"
			"World: %d\n"
			"Mouse: %d %d",
			char_pos.x, char_pos.y,
			(int)floor(char_pos.x), (int)ceil(char_pos.x),
			(int)floor(char_pos.y), (int)ceil(char_pos.y),
			world_id,
			m_MousePosX, m_MousePosY);
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

GameMain* KleinGame()
{
	static GameMain game;
	return &game;
}

