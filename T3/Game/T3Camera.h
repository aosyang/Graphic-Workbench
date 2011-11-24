/********************************************************************
	created:	2011/11/24
	filename: 	T3Camera.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#ifndef T3Camera_h__
#define T3Camera_h__

#include "Math/GWVectors.h"
#include "GWCommon.h"
#include "GWTypes.h"
#include "GameDef.h"

enum T3CameraProjectionType
{
	T3_CAMERA_PROJ_PERSPECTIVE,
	T3_CAMERA_PROJ_ORTHO,
};

static const float T3_CAMERA_ORTHO_HEIGHT	= 16.5685f;
static const float T3_CAMERA_ORTHO_WIDTH	= T3_CAMERA_ORTHO_HEIGHT * KLEIN_SCREEN_ASPECT;

// Camera projection animation time duration
static const GW_UINT32 T3_CAMERA_ANIM_TIME	= 200;

typedef struct T3Camera
{
	T3CameraProjectionType	proj_type;

	Vector2					position;			// Camera position in 2D world
	GWAngle					fovy;
	GW_UINT32				anim_end_time;
	GW_BOOL					animating;
	GW_BOOL					inverse_anim;

	Vector3					world_position;		// Camera position in 3D world
	float					znear, zfar;

} T3_CAMERA;

void T3Camera_Init(T3_CAMERA* camera);

void T3Camera_Update(T3_CAMERA* camera);

void T3Camera_SetupViewWithCamera(T3_CAMERA* camera);

void T3Camera_ActiveProjectionAnimation(T3_CAMERA* camera);
void T3Camera_DeactiveProjectionAnimation(T3_CAMERA* camera);

#endif // T3Camera_h__