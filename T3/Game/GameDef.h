/********************************************************************
	created:	2011/10/15
	filename: 	GameDef.h
	author:		Mwolf
	
	purpose:	Common definitions for Klein
*********************************************************************/
#ifndef GameDef_h__
#define GameDef_h__

#define KLEIN_CAMERA_FOVY		DEGREE(45.f)
#define KLEIN_CAMERA_ZPOS		(-20.0f)

#define KLEIN_SCREEN_WIDTH		1024
#define KLEIN_SCREEN_HEIGHT		768

#if defined GW_PLATFORM_PSP
#define KLEIN_SCREEN_ASPECT		(16.f / 9.f)
#else
#define KLEIN_SCREEN_ASPECT		((float)KLEIN_SCREEN_WIDTH / (float)KLEIN_SCREEN_HEIGHT)
#endif

#define KLEIN_FONT_HEIGHT		16

#define KLEIN_COLLISION_TOLERANCE	(0.001f)

#if defined GW_PLATFORM_PSP
#define KLEIN_NO_STAGE_SAVE
#endif

enum GameWorldviewEnum
{
	//GAME_WORLD_COMMON,
	GAME_WORLD_0,
	GAME_WORLD_1,
	//GAME_WORLD_2,
	GAME_WORLD_COUNT,
};

#endif // GameDef_h__