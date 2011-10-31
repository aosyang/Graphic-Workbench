/********************************************************************
	created:	2011/9/5
	filename: 	RenderDevice.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef RenderDevice_h__
#define RenderDevice_h__

#include "Math/GWVectors.h"
#include "GWRenderWindow.h"
#include "GWColor.h"

typedef struct TextureInfoType TEXTURE_INFO;
typedef struct GWRenderWindow GW_RENDER_WINDOW;

class RenderSystem
{
public:
	static void Initialize(GW_RENDER_WINDOW* rw);
	static void Destroy();

	static TEXTURE_INFO* CreateTexture( const char* filename );
	static void DestroyTexture( TEXTURE_INFO* texture );

	static void SetupCamera(const Vector2& cam_pos, float fovy);
	static void Clear();
	static void BeginRender();
	static void EndRender();
	static void Flush();

	static void DrawSprite(const Vector2& vMin, const Vector2& vMax, int tex_id = -1, float depth = 0.0f);
	static void DrawColoredSprite(const Vector2& vMin, const Vector2& vMax, const GWColor& color = GWColor::WHITE, float depth = 0.0f);
	static void DrawWireframeRect(const Vector2& vMin, const Vector2& vMax, const GWColor& color = GWColor::WHITE, float depth = 0.0f);

	static void RenderText( const char* text, int x, int y, const GWColor& color = GWColor::WHITE );

private:
};

#endif // RenderDevice_h__