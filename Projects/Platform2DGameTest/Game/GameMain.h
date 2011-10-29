/********************************************************************
	created:	2011/10/14
	filename: 	GameMain.h
	author:		Mwolf
	
	purpose:	Main game loop for Klein
*********************************************************************/
#ifndef GameMain_h__
#define GameMain_h__

#include "GameDef.h"

#include "GWInputDeviceEnum.h"

#include "Math/GWVectors.h"

class GameStageEditor;
class GameStage;
class Actor;
class Player;
class Patient;

enum PrototypeFeatureEnum
{
	PROTO_FEATURE_CIRCLE_OF_TRUE_VIEW		= 0x00000001,
};

typedef struct GameCamera
{
	Vector2		position;
	float		fovy;
	Vector2		dest_point;
	Actor*		dest_actor;
} GAME_CAMERA;

class GameMain
{
public:
	GameMain();
	~GameMain();

	void Reset();

	void Startup();
	void Shutdown();
	void Update(float delta_time);

	void Render();

	void SetKeyState(int key_code, bool key_down);
	void SetMouseBtnState(GWMouseButton mbtn_code, bool btn_down);
	void SetMousePosition(int x_pos, int y_pos);
	void SetMouseWheelValue(int wheel) { m_InputState.mouse.wheel = wheel; }

	Vector2 GetCameraPos() const;
	Vector2 GetPlayerPos() const;
	void GetMousePos(int* x, int* y);

	void ProtoFeatureBitSet(int bits, bool val);
	void ProtoFeatureFlipBit(int bits);
	bool TestProtoFeatureBit(int bits) const;

	void SetWorldview(int world_id);
	GameWorldviewEnum GetWorldview() const { return m_ActiveWorld; }

	float GetFovy() const;

private:
	void OnKeyPressed(int key_code);
	void OnKeyReleased(int key_code);

	void OnMouseBtnPressed(GWMouseButton mbtn_code);
	void OnMouseBtnReleased(GWMouseButton mbtn_code);

	void ClearMouseWheelState();

	void UpdateCamera();
	void UpdateActors( float delta_time );
	void HandlePlayerTriggerInteractivities();
	void HandlePlayerActorInteractivities();
	void UpdateEditorControl();

	void DrawDebugText();

private:
	GameStage*			m_GameStage;
	Player*				m_Player;
	Patient*			m_Patient;

	GW_INPUT_STATE		m_InputState;

	bool				m_IsEditorMode;
	GameStageEditor*	m_GameStageEditor;

	GAME_CAMERA			m_Camera;

	int					m_ProtoFeatureBits;

	GameWorldviewEnum			m_ActiveWorld;
};

GameMain* KleinGame();

#endif // GameMain_h__