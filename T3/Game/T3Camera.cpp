/********************************************************************
	created:	2011/11/24
	filename: 	T3Camera.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "T3Camera.h"
#include "GameMain.h"
#include "Renderer/GWRenderDevice.h"

static const Vector3 tilt_offset[] =
{
	Vector3(0.f, 0.f, 1.f),
	Vector3(1.f, 0.f, 1.f),
	Vector3(-1.f, 0.f, 1.f),
	Vector3(0.f, -1.f, 1.f),
	Vector3(0.f, 1.f, 1.f),
};

void T3Camera_Init( T3_CAMERA* camera )
{
	memset(camera, 0, sizeof(T3_CAMERA));

	camera->proj_type = T3_CAMERA_PROJ_ORTHO;
	camera->animating = false;
	camera->tilt = T3_CAMERA_TILT_NONE;
}

static void UpdateCameraView( T3_CAMERA* camera )
{
	// View vector of camera
	Vector3 view_vec = Vector3(0.f, 0.f, KLEIN_CAMERA_ZPOS);
	float sqred_len = view_vec.SqrdLen();

	if (camera->proj_type==T3_CAMERA_PROJ_PERSPECTIVE)
	{
		view_vec.Normalize();

		float z_dist_scale = T3_CAMERA_ORTHO_HEIGHT * .5f / tanf(camera->fovy * .5f);
		z_dist_scale -= sqrtf(sqred_len);

		view_vec = view_vec * z_dist_scale + Vector3(0.f, 0.f, KLEIN_CAMERA_ZPOS);
	}

	// Adjust z near and z far
	camera->znear = sqrtf(view_vec.SqrdLen()) - abs(KLEIN_CAMERA_ZPOS) + 0.1f;
	camera->zfar = camera->znear + 30.f;

	//camera->znear = 0.1f;
	//camera->zfar = 1000.0f;

	camera->zdist = view_vec.z;
}

void T3Camera_Update( T3_CAMERA* camera )
{
	GW_UINT32 tick = KleinGame()->GetSysTickCount();

	if (camera->animating)
	{
		if (tick < camera->anim_end_time)
		{
			float p = GW_MATH_CLAMP((float)(camera->anim_end_time - tick) / T3_CAMERA_ANIM_TIME, 0.f, 1.f);
			if (camera->inverse_anim)
				p = 1.f - p;

			camera->fovy = p * (T3_CAMERA_FOVY - T3_CAMERA_FOVY_TOLERANCE) + T3_CAMERA_FOVY_TOLERANCE;
		}
		else
		{
			// End of animation playing
			if (camera->inverse_anim)
			{
				camera->animating = false;
				camera->proj_type = T3_CAMERA_PROJ_PERSPECTIVE;
			}
			else
			{
				camera->animating = false;
				camera->proj_type = T3_CAMERA_PROJ_ORTHO;
			}
		}
	}

	UpdateCameraView(camera);
}

// Get world position of a T3Camera according to the tilt
static Vector3 T3Camera_GetWorldPosition( T3_CAMERA* camera )
{
	//Vector3 offset = tilt_offset[camera->tilt];
	Vector3 offset = tilt_offset[T3_CAMERA_TILT_UP];
	offset.Normalize();
	return Vector3(camera->position, 0.f) + offset * camera->zdist;
}

void T3Camera_SetupViewWithCamera( T3_CAMERA* camera )
{
	if (camera->proj_type==T3_CAMERA_PROJ_PERSPECTIVE)
	{
		RenderSystem::SetPerspectiveProjMatrix(camera->fovy, KLEIN_SCREEN_ASPECT, camera->znear, camera->zfar);
	}
	else
	{
		RenderSystem::SetOrthoProjMatrix(T3_CAMERA_ORTHO_WIDTH, T3_CAMERA_ORTHO_HEIGHT, camera->znear, camera->zfar);
	}

	RenderSystem::SetViewMatrix(T3Camera_GetWorldPosition(camera),
								Vector3(camera->position, 0.f));

}

void T3Camera_ActiveProjectionAnimation( T3_CAMERA* camera )
{
	// Start animation
	camera->animating = true;
	camera->anim_end_time = KleinGame()->GetSysTickCount() + T3_CAMERA_ANIM_TIME;
	camera->proj_type = T3_CAMERA_PROJ_PERSPECTIVE;
	camera->inverse_anim = true;
}

void T3Camera_DeactiveProjectionAnimation( T3_CAMERA* camera )
{
	camera->animating = true;
	camera->anim_end_time = KleinGame()->GetSysTickCount() + T3_CAMERA_ANIM_TIME;
	camera->proj_type = T3_CAMERA_PROJ_PERSPECTIVE;
	camera->inverse_anim = false;
}