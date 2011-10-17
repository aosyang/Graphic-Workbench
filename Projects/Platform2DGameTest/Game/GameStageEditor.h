#ifndef GameStageEditor_h__
#define GameStageEditor_h__

#include "Math/GWVectors.h"
#include "GWInputMouse.h"

#include <string>

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
	void PickupTileTypeAtCursor();

private:
	Vector2 CursorToTilePos(int x_pos, int y_pos);

	GameStage*			m_GameStage;
	std::string			m_TileTypeToPaint;
};

#endif // GameStageEditor_h__