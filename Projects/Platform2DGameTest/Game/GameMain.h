#ifndef GameMain_h__
#define GameMain_h__

#include "GameDef.h"

#include "GWInputKeyboard.h"
#include "GWInputMouse.h"

#include "Math/GWVectors.h"

class GameStageEditor;
class GameStage;
class Player;
class Patient;

enum PrototypeFeatureEnum
{
	PROTO_FEATURE_CIRCLE_OF_TRUE_VIEW		= 0x00000001,
};

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

	Vector2 GetCameraPos() const;
	Vector2 GetPlayerPos() const;
	void GetMousePos(int* x, int* y);

	const char* GetDebugText() const;

	void ProtoFeatureBitSet(int bits, bool val);
	void ProtoFeatureFlipBit(int bits);
	bool TestProtoFeatureBit(int bits) const;

	void SetWorldview(int world_id);
	GameWorldviewEnum GetWorldview() const { return m_ActiveWorld; }

private:
	void OnKeyPressed(int key_code);
	void OnKeyReleased(int key_code);

	void OnMouseBtnPressed(GWMouseButton mbtn_code);
	void OnMouseBtnReleased(GWMouseButton mbtn_code);

	void UpdateDebugText();

private:
	GameStage*			m_GameStage;
	Player*				m_Player;
	Patient*			m_Patient;

	bool				m_KeyPressed[0xFF];		// Keep the press state of each key
	bool				m_MBtnPressed[MBTN_COUNT];
	int					m_MousePosX, m_MousePosY;

	bool				m_IsEditorMode;
	GameStageEditor*	m_GameStageEditor;
	char				m_DebugText[256];

	Vector2				m_CameraPos;

	int					m_ProtoFeatureBits;

	GameWorldviewEnum			m_ActiveWorld;
};

GameMain* KleinGame();

#endif // GameMain_h__