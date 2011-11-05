/********************************************************************
	created:	2011/9/5
	filename: 	GWRenderDevice.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWRenderDevice_h__
#define GWRenderDevice_h__

#include "Math/GWVectors.h"
#include "GWRenderWindow.h"
#include "GWColor.h"
#include "GWCommon.h"

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
	static void Clear(const GWColor& color);
	static void BeginRender();
	static void EndRender();
	static void Flush();

	static void DrawSprite(const Vector2& vMin, const Vector2& vMax, const TEXTURE_INFO* tex = NULL, float depth = 0.0f);
	static void DrawColoredSprite(const Vector2& vMin, const Vector2& vMax, const GWColor& color = GWColor::WHITE, float depth = 0.0f);
	static void DrawWireframeRect(const Vector2& vMin, const Vector2& vMax, const GWColor& color = GWColor::WHITE, float depth = 0.0f);

	static void RenderText( const char* text, int x, int y, const GWColor& color = GWColor::WHITE );

private:
};

#endif // GWRenderDevice_h__