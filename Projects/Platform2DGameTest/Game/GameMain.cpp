#include "GameMain.h"

#include "Character.h"
#include "GameStage.h"

#include "GWCommon.h"

GameMain::GameMain()
: m_GameStage(NULL),
  m_Character(NULL),
  m_EditorMode(false),
  m_MousePosX(0),
  m_MousePosY(0)
{
	for (int i=0; i<0xFF; i++)
		m_KeyPressed[i] = false;

	for (int i=0; i<MBTN_COUNT; i++)
		m_MBtnPressed[i] = false;
}

GameMain::~GameMain()
{
	Reset();
}

void GameMain::Reset()
{
	GW_SAFE_DELETE(m_GameStage);
	GW_SAFE_DELETE(m_Character);
}

void GameMain::Startup()
{
	m_GameStage = new GameStage;
	m_GameStage->LoadFromFile("stage.lua");

	m_Character = new Character;
}

void GameMain::Update( float fElapsedTime )
{
	m_Character->Update(fElapsedTime);

	TileUsageEnum player_pos_type = m_GameStage->GetTileTypeAtPoint(m_Character->WorldPosition());

	Vector2 moveVector(0.0f, 0.0f);
	if (m_KeyPressed[GW_KEY_LEFT]) moveVector += Vector2(-1.0f, 0.0f);
	if (m_KeyPressed[GW_KEY_RIGHT]) moveVector += Vector2(1.0f, 0.0f);

	if (m_Character->IsClimbingLadder())
	{
		if (m_KeyPressed[GW_KEY_UP]) moveVector += Vector2(0.0f, 1.0f);
		if (m_KeyPressed[GW_KEY_DOWN]) moveVector += Vector2(0.0f, -1.0f);

		// Fall down if no ladder
		if (player_pos_type != TILE_USAGE_LADDER)
		{
			m_Character->SetClimbingLadder(false);
		}
	}
	else
	{
		// Climb up if player stands near by a ladder
		if (m_KeyPressed[GW_KEY_UP] && player_pos_type == TILE_USAGE_LADDER)
		{
			m_Character->SetClimbingLadder(true);
		}
	}

	moveVector.Normalize();
	if (m_Character->IsClimbingLadder())
	{
		// Slow down if climbing ladder
		moveVector *= 0.05f;
	}
	else
	{
		moveVector *= 0.1f;
	}

	m_GameStage->TestCollision( m_Character, Vector3(moveVector, 0.0f) );
}

void GameMain::Render()
{
	m_GameStage->RenderStage();

	m_Character->Render();
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

void GameMain::OnKeyPressed( int key_code )
{
	switch (key_code)
	{
	case GW_KEY_SPACE:
		// Toggle editor mode
		m_EditorMode = !m_EditorMode;
		break;
	case GW_KEY_1:
	case GW_KEY_2:
	case GW_KEY_3:
	case GW_KEY_4:
		m_GameStage->SetWorldview(GAME_WORLD_COMMON + key_code - GW_KEY_1);
		break;
	case GW_KEY_Z:
		m_Character->Jump();
		break;
	}
}

void GameMain::OnKeyReleased( int key_code )
{

}

void GameMain::OnMouseBtnPressed( GWMouseButton mbtn_code )
{

}

void GameMain::OnMouseBtnReleased( GWMouseButton mbtn_code )
{

}

const Vector3& GameMain::GetCharacterPos() const
{
	return m_Character->WorldPosition();
}

int GameMain::GetWorldviewId() const
{
	return (int)m_GameStage->GetWorldview();
}

