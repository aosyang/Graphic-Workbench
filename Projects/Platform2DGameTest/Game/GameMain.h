#ifndef GameMain_h__
#define GameMain_h__

#include "GWInputKeyboard.h"

#include "Math/GWVectors.h"

class GameStage;
class Character;

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

	const Vector3& GetCharacterPos() const;
	int GetWorldviewId() const;

private:
	void OnKeyPressed(int key_code);
	void OnKeyReleased(int key_code);

private:
	GameStage*		m_GameStage;
	Character*		m_Character;

	bool			m_KeyPressed[0xFF];
};

#endif // GameMain_h__