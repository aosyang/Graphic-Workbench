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
#include "GWTimer.h"

#include "GameStageEditor.h"
#include "Renderer/TextureManager.h"
#include "GWInputControl.h"
#include "T3Camera.h"

#include "AreaTrigger.h"

GWControlMap KleinControlMap[] =
{
	/************************************************************************/
	/* PC Controls
	/************************************************************************/

	// Editor control
	{ GameMain::Con_EditorMoveLeft,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_A,			GW_KEY_STATE_DOWN },
	{ GameMain::Con_EditorMoveRight,	GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_D,			GW_KEY_STATE_DOWN },
	{ GameMain::Con_EditorMoveUp,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_W,			GW_KEY_STATE_DOWN },
	{ GameMain::Con_EditorMoveDown,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_S,			GW_KEY_STATE_DOWN },

	// Character control
	{ GameMain::Con_MoveLeft,			GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_LEFT,		GW_KEY_STATE_DOWN },
	{ GameMain::Con_MoveRight,			GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_RIGHT,		GW_KEY_STATE_DOWN },
	{ GameMain::Con_MoveUp,				GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_UP,			GW_KEY_STATE_DOWN },
	{ GameMain::Con_MoveDown,			GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_DOWN,		GW_KEY_STATE_DOWN },

	{ GameMain::Con_PlayerJump,			GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_SPACE,		GW_KEY_STATE_ON_PRESSED },

	{ GameMain::Con_WorldPerspec0,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_9,			GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_WorldPerspec1,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_0,			GW_KEY_STATE_ON_PRESSED },

	{ GameMain::Con_ActivePerspectiveView,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_H,			GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_DeactivePerspectiveView,	GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_H,			GW_KEY_STATE_ON_RELEASED },

	{ GameMain::Con_CameraTiltLeft,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_J,			GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_CameraTiltRight,	GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_L,			GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_CameraTiltUp,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_I,			GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_CameraTiltDown,		GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_K,			GW_KEY_STATE_ON_PRESSED },

	{ GameMain::Con_CameraTiltCenter,	GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_J,			GW_KEY_STATE_ON_RELEASED },
	{ GameMain::Con_CameraTiltCenter,	GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_L,			GW_KEY_STATE_ON_RELEASED },
	{ GameMain::Con_CameraTiltCenter,	GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_I,			GW_KEY_STATE_ON_RELEASED },
	{ GameMain::Con_CameraTiltCenter,	GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_K,			GW_KEY_STATE_ON_RELEASED },

	/************************************************************************/
	/* PSP Key Controls
	/************************************************************************/
#if defined GW_PLATFORM_PSP
	//{ GameMain::Con_MoveLeft,			GW_INPUT_DEVICE_PSP,			GW_PSPBTN_LEFT,		GW_KEY_STATE_DOWN },
	//{ GameMain::Con_MoveRight,			GW_INPUT_DEVICE_PSP,			GW_PSPBTN_RIGHT,	GW_KEY_STATE_DOWN },
	//{ GameMain::Con_MoveUp,				GW_INPUT_DEVICE_PSP,			GW_PSPBTN_UP,		GW_KEY_STATE_DOWN },
	//{ GameMain::Con_MoveDown,			GW_INPUT_DEVICE_PSP,			GW_PSPBTN_DOWN,		GW_KEY_STATE_DOWN },

	{ GameMain::Con_PlayerJump,			GW_INPUT_DEVICE_PSP,			GW_PSPBTN_CROSS,	GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_SwitchPerspec,		GW_INPUT_DEVICE_PSP,			GW_PSPBTN_TRIANGLE,	GW_KEY_STATE_ON_PRESSED },

	{ GameMain::Con_ActivePerspectiveView,		GW_INPUT_DEVICE_PSP,		GW_PSPBTN_SQUARE,			GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_DeactivePerspectiveView,	GW_INPUT_DEVICE_PSP,		GW_PSPBTN_SQUARE,			GW_KEY_STATE_ON_RELEASED },
#endif	// #if defined GW_PLATFORM_PSP

	/************************************************************************/
	/* XInput Controls
	/************************************************************************/
#if defined GW_XINPUT
	//{ GameMain::Con_MoveLeft,			GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XBTN_DPAD_LEFT,	GW_KEY_STATE_DOWN },
	//{ GameMain::Con_MoveRight,			GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XBTN_DPAD_RIGHT,	GW_KEY_STATE_DOWN },
	//{ GameMain::Con_MoveUp,				GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XBTN_DPAD_UP,	GW_KEY_STATE_DOWN },
	//{ GameMain::Con_MoveDown,			GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XBTN_DPAD_DOWN,	GW_KEY_STATE_DOWN },

	{ GameMain::Con_PlayerJump,			GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XBTN_A,			GW_KEY_STATE_ON_PRESSED },
	{ GameMain::Con_SwitchPerspec,		GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XBTN_Y,			GW_KEY_STATE_ON_PRESSED },
#endif	// #if defined GW_XINPUT

	/************************************************************************/
	/* End of control table
	/************************************************************************/
	{ NULL,								GW_INPUT_DEVICE_KEYBOARD,		GW_KEY_UNDEFINED,	GW_KEY_STATE_INVALID },
};

GWAxisMap KleinAxisMap[] =
{
#if defined GW_PLATFORM_PSP
	{ GameMain::Con_PlayerMove,			GW_INPUT_DEVICE_PSP,						GW_PSPAXIS_LX },
	{ GameMain::Con_PlayerClimb,		GW_INPUT_DEVICE_PSP,						GW_PSPAXIS_LY },
#endif	// #if defined GW_PLATFORM_PSP

#if defined GW_XINPUT
	{ GameMain::Con_PlayerMove,			GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XAXIS_LX },
	{ GameMain::Con_PlayerClimb,		GW_INPUT_DEVICE_XINPUT_CONTROLLER0,			GW_XAXIS_LY },
#endif	// #if defined GW_XINPUT

	{ NULL,								GW_INPUT_DEVICE_KEYBOARD,		0 },
};

Player* GameMain::m_Player = NULL;
Vector2 GameMain::m_MovingVector;
bool GameMain::m_UpPressed = false;

GameMain::GameMain()
: m_RenderWindow(NULL),
  m_SysTime(0),
  m_GameStage(NULL),
  m_GameStageEditor(NULL),
  m_ProtoFeatureBits(0),
  m_ActiveWorld((GameWorldviewEnum)0)
{
	T3Camera_Init(&m_Camera);
	m_Camera.fovy = KLEIN_CAMERA_FOVY;

	m_IsEditorMode = false;
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
#if defined GW_PLATFORM_PSP
	m_RenderWindow = GWWnd_CreateRenderWindow();
#else
	m_RenderWindow = GWWnd_CreateRenderWindow(KLEIN_SCREEN_WIDTH, KLEIN_SCREEN_HEIGHT, "Klein");
#endif

	GWInput_InitializeDevice( m_RenderWindow );
	GWInputCon_Initialize( KleinControlMap, KleinAxisMap );

	// Game initializations
	RenderSystem::Initialize( m_RenderWindow );
	RenderSystem::ToggleFog(true);

	m_GameStage = new GameStage;
	m_GameStage->LoadFromFile("Stage.lua");

	// Create player
	m_Player = CreatePlayer();

	m_CameraDestActor = m_Player;

	// Create a patient for test
	m_Patient = CreatePatient();
	m_Patient->SetPosition( Vector2(5.0f, 0.0f) );

	m_GameStageEditor = new GameStageEditor;
	m_GameStageEditor->SetGameStage(m_GameStage);
}

void GameMain::Shutdown()
{
	Reset();

	GWInput_DestroyDevice();
	RenderSystem::Destroy();

	GWWnd_DestroyRenderWindow( m_RenderWindow );
}

void GameMain::Update()
{
	// Update timer
	m_SysTime = GWSys_GetTickCount();

	UpdateInputDevice();

	// Clear input state for last frame
	m_MovingVector = Vector2::ZERO;
	m_UpPressed = false;

	GWInputCon_Update();

	// Reset player movement control
	m_Player->MoveController() = Vector2::ZERO;

	if (m_IsEditorMode)
	{
		// Free move in editor mode
		m_MovingVector.Normalize();
		m_MovingVector *= 0.5f;

		m_Player->Translate( m_MovingVector );
		m_Player->Velocity().y = 0.0f;
	}
	else 
	{
		STAGE_GEOM* geom = m_GameStage->GetTileAtPoint(m_Player->GetPosition());
		TileUsageEnum player_pos_type = geom ? m_GameStage->GetStageGeomUsage(geom) : TILE_USAGE_VOID;

		if (m_Player->IsClimbingLadder())
		{
			// Fall down if no ladder
			if (player_pos_type != TILE_USAGE_LADDER)
			{
				m_Player->SetClimbingLadder(false);
			}
		}
		else
		{
			if (m_UpPressed)
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

		//m_MovingVector.Normalize();
		if (m_Player->IsClimbingLadder())
		{
			// Slow down if climbing ladder
			m_MovingVector *= 0.08f;
		}
		else
		{
			m_MovingVector *= 0.1f;
		}

		// Adjust moving vector by camera tilt
		switch ( GetCameraTilt() )
		{
		case T3_CAMERA_TILT_LEFT:
		case T3_CAMERA_TILT_RIGHT:
			m_MovingVector *= Vector2( sinf( DEGREE(45.f) ) * 2, 1.f );
			break;
		case T3_CAMERA_TILT_UP:
		case T3_CAMERA_TILT_DOWN:
			m_MovingVector *= Vector2( 1.f, sinf( DEGREE(45.f) ) * 2 );
			break;
		}

		m_Player->MoveController() = m_MovingVector;

		HandlePlayerTriggerInteractivities();
		UpdateActors();
	}

	UpdateCamera();

	UpdateEditorControl();
}

void GameMain::Render()
{
	RenderSystem::BeginRender();

	Vector2 cam_pos = GetCameraPos();

	RenderSystem::LoadIdentityModelMatrix();
	if (m_IsEditorMode)
	{
		RenderSystem::SetPerspectiveProjMatrix(m_GameStageEditor->GetFovy(), KLEIN_SCREEN_ASPECT, m_Camera.znear, m_Camera.zfar);
		RenderSystem::SetViewMatrix( Vector3(cam_pos.x, cam_pos.y, KLEIN_CAMERA_ZPOS),
									 Vector3(cam_pos.x, cam_pos.y, 0.f) );
	}
	else
	{
		T3Camera_SetupViewWithCamera(&m_Camera);
	}
	RenderSystem::SetFogParameters(m_Camera.znear + 20.f, m_Camera.zfar, GWIntegerColor(141, 153, 191));

	RenderSystem::Clear(GWIntegerColor(141, 153, 191, 0));

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

	RenderSystem::EndRender();
	RenderSystem::Flush();
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
#if defined GW_PLATFORM_PSP
	if (x) *x = 0;
	if (y) *y = 0;
#else
	if (x) *x = m_RenderWindow->mouse_x;
	if (y) *y = m_RenderWindow->mouse_y;
#endif
}


void GameMain::SetWorldview( int world_id )
{
	if (m_ActiveWorld != (GameWorldviewEnum)world_id)
	{
		m_ActiveWorld = (GameWorldviewEnum)world_id;

		if (!m_IsEditorMode)
			m_GameStage->AnimSwapWorlds();
	}
}

void GameMain::ActivePerspectiveView()
{
	T3Camera_ActiveProjectionAnimation(&m_Camera, T3_CAMERA_PROJ_ANIM_TO_PERSPECTIVE);
}

void GameMain::DeactivePerspectiveView()
{
	T3Camera_ActiveProjectionAnimation(&m_Camera, T3_CAMERA_PROJ_ANIM_TO_ORTHO);
}

void GameMain::OnKeyPressed( int key_code )
{
	switch (key_code)
	{
	case GW_KEY_F1:
		// Toggle editor mode
		m_IsEditorMode = !m_IsEditorMode;
		break;
	case GW_KEY_1:
		m_GameStageEditor->SetPaintTool(PAINT_TOOL_PENCIL);
		break;
	case GW_KEY_2:
		m_GameStageEditor->SetPaintTool(PAINT_TOOL_BRUSH);
		break;
	case GW_KEY_C:
		ProtoFeatureFlipBit(PROTO_FEATURE_CIRCLE_OF_TRUE_VIEW);
		break;
	case GW_KEY_P:
		if (m_IsEditorMode)
		{
#if !defined KLEIN_NO_STAGE_SAVE
			m_GameStage->SaveToFile("Stage.lua");
			m_GameStageEditor->MarkMapSaved();
#endif
		}
		break;
	default:
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
	default:
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
	default:
		break;
	}
}

void GameMain::UpdateInputDevice()
{
	GWInput_UpdateInputState();

	for (int i=0; i<0xFF; i++)
	{
		if (GWInput_GetKeyState(i) == GW_KEY_STATE_ON_PRESSED)
			OnKeyPressed(i);
		//if (GWInput_GetKeyState(i) == GW_KEY_STATE_ON_RELEASED)
		//	OnKeyReleased(i);
	}	

#if !defined GW_PLATFORM_PSP
	for (int i=0; i<MBTN_COUNT; i++)
	{
		if (GWInput_GetMouseBtnState(i) == GW_KEY_STATE_ON_PRESSED)
			OnMouseBtnPressed((GWMouseButton)i);
		if (GWInput_GetMouseBtnState(i) == GW_KEY_STATE_ON_RELEASED)
			OnMouseBtnReleased((GWMouseButton)i);
	}
#endif

}

void GameMain::UpdateEditorControl()
{
	if (!m_IsEditorMode) return;

	if ( GWInput_GetMouseBtnState(MBTN_LEFT) == GW_KEY_STATE_DOWN && 
		 !GWInput_GetMouseBtnState(MBTN_RIGHT) == GW_KEY_STATE_DOWN )//Edit by YLL
	{
		m_GameStageEditor->PaintTileAtCursor();
	}
	else if ( GWInput_GetMouseBtnState(MBTN_RIGHT) == GW_KEY_STATE_DOWN )
	{
		m_GameStageEditor->StartPicking();//Edit by YLL for right click pick mode
		//m_GameStageEditor->PickupTileTypeAtCursor();
	}
	else 
	{
		m_GameStageEditor->EndPicking();//Add by YLL for right click pick mode
	}

	m_GameStageEditor->ZoomView( GWInput_GetMouseWheelValue() / 120 );
}

void GameMain::DrawDebugText()
{
	char debug_text[256];

	// Update debug info
	Vector2 char_pos = m_Player->GetPosition();
	int world_id = (int)GetWorldview();

	// Draw debug text
	sprintf(debug_text,
			"pos: %f, %f\n"
			"Block: x( %d ~ %d ) - y( %d ~ %d )\n"
			"World: %d\n"
#if !defined GW_PLATFORM_PSP
			"Mouse: %d %d %d\n"
#endif
			"znear: %f zfar: %f\n"
			"CAM_TILT: %s\n"
			"%s",
			char_pos.x, char_pos.y,
			(int)floor(char_pos.x), (int)ceil(char_pos.x),
			(int)floor(char_pos.y), (int)ceil(char_pos.y),
			world_id,
#if !defined GW_PLATFORM_PSP
			m_RenderWindow->mouse_x, m_RenderWindow->mouse_y, GWInput_GetMouseWheelValue(),
#endif
			m_Camera.znear, m_Camera.zfar,
			T3Camera_GetTiltTypeName(m_Camera.tilt),
			m_GameStageEditor->IsMapUnsaved() ? "*Map unsaved*" : "");

	RenderSystem::RenderText(debug_text, 0, 0, GWColor::YELLOW);
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

void GameMain::UpdateActors()
{
	Actor* actor = GetFirstActor();
	while ( actor )
	{
		actor->Update();
		m_GameStage->TestCollision( actor );

		actor = GetNextActor( actor );
	}
}

void GameMain::UpdateCamera()
{
	Vector2 rel;

	// Update camera position
	if (m_CameraDestActor)
	{
		rel = m_CameraDestActor->GetPosition() - m_Camera.position + Vector2 (0.0f, 4.5f);
	}
	else
	{
		rel = m_CameraDestPoint - m_Camera.position;
	}

	float dist = sqrtf(rel.SqrdLen());
	if (dist > 0.3f)
		m_Camera.position += rel * dist * 0.05f;

	T3Camera_Update(&m_Camera);
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

/************************************************************************/
/* Klein game controller methods
/************************************************************************/
void GameMain::Con_EditorMoveLeft()
{
	if (KleinGame()->IsEditorMode())
		m_MovingVector += Vector2(-1.0f, 0.0f);
}

void GameMain::Con_EditorMoveRight()
{
	if (KleinGame()->IsEditorMode())
		m_MovingVector += Vector2(1.0f, 0.0f);
}

void GameMain::Con_EditorMoveUp()
{
	if (KleinGame()->IsEditorMode())
		m_MovingVector += Vector2(0.0f, 1.0f);
}

void GameMain::Con_EditorMoveDown()
{
	if (KleinGame()->IsEditorMode())
		m_MovingVector += Vector2(0.0f, -1.0f);
}

void GameMain::Con_MoveLeft()
{
	if (!KleinGame()->IsEditorMode())
		m_MovingVector += Vector2(-1.0f, 0.0f);
}

void GameMain::Con_MoveRight()
{
	if (!KleinGame()->IsEditorMode())
		m_MovingVector += Vector2(1.0f, 0.0f);
}

void GameMain::Con_MoveUp()
{
	if (m_Player->IsClimbingLadder())
	{
		m_MovingVector += Vector2(0.0f, 1.0f);
	}
	else
	{
		m_UpPressed = true;
	}
}

void GameMain::Con_MoveDown()
{
	if (m_Player->IsClimbingLadder())
	{
		m_MovingVector += Vector2(0.0f, -1.0f);
	}
}

void GameMain::Con_PlayerJump()
{
	m_Player->Jump();
}

void GameMain::Con_WorldPerspec0()
{
	KleinGame()->SetWorldview(0);
}

void GameMain::Con_WorldPerspec1()
{
	KleinGame()->SetWorldview(1);
}

void GameMain::Con_SwitchPerspec()
{
	KleinGame()->SetWorldview(1 - KleinGame()->GetWorldview());
}

void GameMain::Con_PlayerMove( float val )
{
	if (!KleinGame()->IsEditorMode())
		m_MovingVector += Vector2(val, 0.0f);
}

void GameMain::Con_PlayerClimb( float val )
{
	if (!KleinGame()->IsEditorMode())
	{
		if (m_Player->IsClimbingLadder())
		{
			m_MovingVector += Vector2(0.0f, val);
		}
		else if (val > 0.f)
		{
			m_UpPressed = true;
		}
	}
}

void GameMain::Con_ActivePerspectiveView()
{
	KleinGame()->ActivePerspectiveView();
}

void GameMain::Con_DeactivePerspectiveView()
{
	KleinGame()->DeactivePerspectiveView();
}

void GameMain::Con_CameraTiltLeft()
{
	KleinGame()->DoCameraTilt(T3_CAMERA_TILT_LEFT);
}

void GameMain::Con_CameraTiltRight()
{
	KleinGame()->DoCameraTilt(T3_CAMERA_TILT_RIGHT);
}

void GameMain::Con_CameraTiltUp()
{
	KleinGame()->DoCameraTilt(T3_CAMERA_TILT_UP);
}

void GameMain::Con_CameraTiltDown()
{
	KleinGame()->DoCameraTilt(T3_CAMERA_TILT_DOWN);
}

void GameMain::Con_CameraTiltCenter()
{
	KleinGame()->DoCameraTilt(T3_CAMERA_TILT_NONE);
}
