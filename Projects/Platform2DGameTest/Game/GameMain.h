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
#include "GWTypes.h"

class GameStageEditor;
class GameStage;
class Actor;
class Player;
class Patient;
typedef struct GWRenderWindow GW_RENDER_WINDOW;

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
	void Update();

	void Render();

	Vector2 GetCameraPos() const;
	Vector2 GetPlayerPos() const;
	void GetMousePos(int* x, int* y);

	void ProtoFeatureBitSet(int bits, bool val);
	void ProtoFeatureFlipBit(int bits);
	bool TestProtoFeatureBit(int bits) const;

	bool IsEditorMode() const { return m_IsEditorMode; }

	void SetWorldview(int world_id);
	GameWorldviewEnum GetWorldview() const { return m_ActiveWorld; }

	float GetFovy() const;

	GW_UINT32 GetSysTickCount() const { return m_SysTime; }
public:
	/************************************************************************/
	/* Game Control methods
	/************************************************************************/
	static void Con_EditorMoveUp();
	static void Con_EditorMoveDown();
	static void Con_EditorMoveLeft();
	static void Con_EditorMoveRight();

	static void Con_MoveLeft();
	static void Con_MoveRight();
	static void Con_MoveUp();
	static void Con_MoveDown();

	static void Con_PlayerJump();
	static void Con_PlayerMove(float val);

	static void Con_WorldPerspec0();
	static void Con_WorldPerspec1();

	static void Con_SwitchPerspec();

private:
	void OnKeyPressed(int key_code);
	void OnKeyReleased(int key_code);

	void OnMouseBtnPressed(GWMouseButton mbtn_code);
	void OnMouseBtnReleased(GWMouseButton mbtn_code);

	void UpdateInputDevice();
	void UpdateCamera();
	void UpdateActors();
	void HandlePlayerTriggerInteractivities();
	void HandlePlayerActorInteractivities();
	void UpdateEditorControl();

	void DrawDebugText();

private:
	GW_RENDER_WINDOW*			m_RenderWindow;

	GW_UINT32					m_SysTime;

	GameStage*					m_GameStage;
	static Player*				m_Player;
	Patient*					m_Patient;

	static Vector2				m_MovingVector;
	static bool					m_UpPressed;

	bool						m_IsEditorMode;
	GameStageEditor*			m_GameStageEditor;

	GAME_CAMERA					m_Camera;

	int							m_ProtoFeatureBits;

	GameWorldviewEnum			m_ActiveWorld;
};

GameMain* KleinGame();

#endif // GameMain_h__