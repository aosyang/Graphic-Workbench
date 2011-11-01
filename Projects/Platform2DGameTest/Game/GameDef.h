/********************************************************************
	created:	2011/10/15
	filename: 	GameDef.h
	author:		Mwolf
	
	purpose:	Common definitions for Klein
*********************************************************************/
#ifndef GameDef_h__
#define GameDef_h__

#define GW_MATH_PI				(float)(3.141592654f)

#define KLEIN_CAMERA_FOVY		(GW_MATH_PI / 4)
#define KLEIN_CAMERA_ZPOS		(-20.0f)

#define KLEIN_SCREEN_WIDTH		640
#define KLEIN_SCREEN_HEIGHT		480
#define KLEIN_SCREEN_ASPECT		(float)KLEIN_SCREEN_WIDTH / (float)KLEIN_SCREEN_HEIGHT

#define KLEIN_FONT_HEIGHT		16

#define KLEIN_COLLISION_TOLERANCE	(0.001f)

//#define KLEIN_NO_STAGE_SAVE

enum GameWorldviewEnum
{
	//GAME_WORLD_COMMON,
	GAME_WORLD_0,
	GAME_WORLD_1,
	//GAME_WORLD_2,
	GAME_WORLD_COUNT,
};

#endif // GameDef_h__