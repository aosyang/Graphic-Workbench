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

enum T3CameraTilt
{
	T3_CAMERA_TILT_NONE,
	T3_CAMERA_TILT_LEFT,
	T3_CAMERA_TILT_RIGHT,
	T3_CAMERA_TILT_UP,
	T3_CAMERA_TILT_DOWN,
};

enum T3CameraProjAnim
{
	T3_CAMERA_PROJ_ANIM_TO_PERSPECTIVE,
	T3_CAMERA_PROJ_ANIM_TO_ORTHO,
};

const char* T3Camera_GetTiltTypeName(T3CameraTilt tile);

static const float T3_CAMERA_ORTHO_HEIGHT	= 16.5685f;		// tanf( DEGREE(22.5f) ) * abs( KLEIN_CAMERA_ZPOS ) * 2
static const float T3_CAMERA_ORTHO_WIDTH	= T3_CAMERA_ORTHO_HEIGHT * KLEIN_SCREEN_ASPECT;

// Camera projection animation time duration
static const GW_UINT32 T3_CAMERA_ANIM_TIME	= 200;
static const GW_UINT32 T3_CAMERA_TILT_TIME	= 200;

static const float T3_CAMERA_FOVY			= DEGREE(45.f);
static const float T3_CAMERA_FOVY_TOLERANCE	= DEGREE(0.1f);

typedef struct T3Camera
{
	T3CameraProjectionType	proj_type;

	Vector2					position;				// Camera position in 2D world
	GWAngle					fovy;
	GW_UINT32				anim_end_time;
	GW_BOOL					animating;
	GW_BOOL					reverse_anim;

	float					zdist;
	float					znear, zfar;

	T3CameraTilt			tilt;
	GW_UINT32				tilt_anim_end_time;
	GW_BOOL					reverse_tile_anim;
} T3_CAMERA;

void T3Camera_Init(T3_CAMERA* camera);

void T3Camera_Update(T3_CAMERA* camera);

float T3Camera_GetAnimationProgress(T3_CAMERA* camera);

void T3Camera_SetupViewWithCamera(T3_CAMERA* camera);

void T3Camera_ActiveProjectionAnimation(T3_CAMERA* camera, T3CameraProjAnim anim);

void T3Camera_DoTilt(T3_CAMERA* camera, T3CameraTilt tilt);

#endif // T3Camera_h__