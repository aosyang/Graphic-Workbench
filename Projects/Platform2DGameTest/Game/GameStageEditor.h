#ifndef GameStageEditor_h__
#define GameStageEditor_h__

#include "Math/GWVectors.h"
#include "GWInputMouse.h"

class GameStage;

class GameStageEditor
{
public:
	GameStageEditor();
	~GameStageEditor();

	void Render();

	void SetGameStage(GameStage* stage);
	GameStage* GetGameStage() const { return m_GameStage; }

	void PaintTileAtCursor();

	void OnMouseBtnPressed(GWMouseButton mbtn_code, int x_pos, int y_pos);
	void OnMouseBtnReleased(GWMouseButton mbtn_code, int x_pos, int y_pos);

private:
	Vector2 CursorToTilePos(int x_pos, int y_pos);

	GameStage*			m_GameStage;
};

#endif // GameStageEditor_h__