#ifndef GameMain_h__
#define GameMain_h__

#include "GWInputKeyboard.h"

#include "Math/GWVectors.h"

class GameStage;
class Character;

enum GWMouseButton
{
	MBTN_LEFT,
	MBTN_RIGHT,
	MBTN_MIDDLE,

	MBTN_COUNT,
};

class GameMain
{
public:
	GameMain();
	~GameMain();

	void Reset();

	void Startup();
	void Update(float fElapsedTime);

	void Render();

	void SetKeyState(int key_code, bool key_down);
	void SetMouseBtnState(GWMouseButton mbtn_code, bool btn_down);
	void SetMousePosition(int x_pos, int y_pos);

	const Vector3& GetCharacterPos() const;
	int GetWorldviewId() const;

private:
	void OnKeyPressed(int key_code);
	void OnKeyReleased(int key_code);

	void OnMouseBtnPressed(GWMouseButton mbtn_code);
	void OnMouseBtnReleased(GWMouseButton mbtn_code);

private:
	GameStage*		m_GameStage;
	Character*		m_Character;

	bool			m_KeyPressed[0xFF];		// Keep the press state of each key
	bool			m_MBtnPressed[MBTN_COUNT];
	int				m_MousePosX, m_MousePosY;

	bool			m_EditorMode;
};

#endif // GameMain_h__