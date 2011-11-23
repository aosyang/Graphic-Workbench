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

	static void SetPerspectiveProjMatrix( GWAngle fovy, float aspect, float znear, float zfar );
	static void SetOrthoProjMatrix( float width, float height, float znear, float zfar );
	static void SetViewMatrix( const Vector3& eye, const Vector3& look_at, const Vector3& up = Vector3(0.f, 1.f, 0.f) );
	static void LoadIdentityModelMatrix();

	static void Clear(const GWColor& color);
	static void BeginRender();
	static void EndRender();
	static void Flush();

	static void ToggleFog(bool enable);
	static void SetFogParameters(float fog_near, float fog_far, const GWColor& color);

	static void DrawCube(const Vector2& vMin, const Vector2& vMax, const TEXTURE_INFO* tex = NULL, float depth = 0.0f);

	static void DrawSprite(const Vector2& vMin, const Vector2& vMax, const TEXTURE_INFO* tex = NULL, float depth = 0.0f);
	static void DrawColoredSprite(const Vector2& vMin, const Vector2& vMax, const GWColor& color = GWColor::WHITE, float depth = 0.0f);
	static void DrawWireframeRect(const Vector2& vMin, const Vector2& vMax, const GWColor& color = GWColor::WHITE, float depth = 0.0f);

	static void RenderText( const char* text, int x, int y, const GWColor& color = GWColor::WHITE );

private:
};

#endif // GWRenderDevice_h__