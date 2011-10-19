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

	//Add by YLL for right click picking mode
	void StartPicking( bool bStart = true );
	void EndPicking(){ StartPicking( false ); }

	void PaintTileAtCursor();
	void PickupTileTypeAtCursor();

private:
	Vector2 CursorToTilePos(int x_pos, int y_pos);

	GameStage*			m_GameStage;
	std::string			m_TileTypeToPaint;

	bool				m_bPicking;
};

#endif // GameStageEditor_h__