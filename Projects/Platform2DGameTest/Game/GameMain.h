#ifndef GameMain_h__
#define GameMain_h__

#include "GWInputKeyboard.h"
#include "GWInputMouse.h"

#include "Math/GWVectors.h"

class GameStageEditor;
class GameStage;
class Character;

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

	Vector3 GetCameraPos() const;
	void GetMousePos(int* x, int* y);

	const char* GetDebugText() const;

private:
	void OnKeyPressed(int key_code);
	void OnKeyReleased(int key_code);

	void OnMouseBtnPressed(GWMouseButton mbtn_code);
	void OnMouseBtnReleased(GWMouseButton mbtn_code);

	void UpdateDebugText();

private:
	GameStage*			m_GameStage;
	Character*			m_Character;

	bool				m_KeyPressed[0xFF];		// Keep the press state of each key
	bool				m_MBtnPressed[MBTN_COUNT];
	int					m_MousePosX, m_MousePosY;

	bool				m_IsEditorMode;
	GameStageEditor*	m_GameStageEditor;
	char				m_DebugText[256];

	Vector3				m_CameraPos;
};

GameMain* KleinGame();

#endif // GameMain_h__