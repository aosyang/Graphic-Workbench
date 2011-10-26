#ifndef GameStageEditor_h__
#define GameStageEditor_h__

#include "Math/GWVectors.h"
#include "GWInputMouse.h"
#include "BoundBox.h"

#include <string>

class GameStage;

enum PaintToolType
{
	PAINT_TOOL_PENCIL,
	PAINT_TOOL_BRUSH,
};

enum PaintToolState
{
	TOOL_STATE_IDLE,
	TOOL_STATE_DRAWING,
};

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

	void StartPainting();
	void EndPainting();
	void PaintTileAtCursor();
	void PickupTileTypeAtCursor();

	void SetPaintTool(PaintToolType tool) { m_PaintTool = tool; }
	PaintToolType GetPaintTool() const { return m_PaintTool; }

	void ZoomView(int zoom);

	float GetFovy() const { return m_Fovy; }

private:
	Vector2 CursorToTilePos(int x_pos, int y_pos);

	GameStage*			m_GameStage;
	std::string			m_TileTypeToPaint;

	bool				m_bPicking;
	Vector2				m_TileMenuPos;

	float				m_Fovy;

	PaintToolType		m_PaintTool;
	BoundBox			m_SelectedArea;

	PaintToolState		m_ToolState;
};

#endif // GameStageEditor_h__