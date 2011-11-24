/********************************************************************
	created:	2011/11/24
	filename: 	T3Camera.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/

#include "T3Camera.h"
#include "GameMain.h"
#include "Renderer/GWRenderDevice.h"

void T3Camera_Init( T3_CAMERA* camera )
{
	camera->proj_type = T3_CAMERA_PROJ_PERSPECTIVE;
	camera->animating = false;
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

		view_vec = view_vec * z_dist_scale + Vector3(camera->position, KLEIN_CAMERA_ZPOS);
	}

	// Adjust z near and z far
	//camera->zfar = sqrtf(view_vec.SqrdLen()) + 1.0f;
	//camera->znear = sqrtf(view_vec.SqrdLen()) - 2.0f;
	camera->zfar = 1000.f;
	camera->znear = 1.f;

	camera->world_position = view_vec;
}

void T3Camera_Update( T3_CAMERA* camera )
{
	GW_UINT32 tick = KleinGame()->GetSysTickCount();

	if (camera->animating)
	{
		if (tick < camera->anim_end_time)
		{
			float p = (float)(camera->anim_end_time - tick) / T3_CAMERA_ANIM_TIME;
			camera->fovy = DEGREE(p * 44.5f + 0.5f);
		}
		else
		{
			// End of animation playing
			camera->animating = false;
			camera->proj_type = T3_CAMERA_PROJ_ORTHO;
		}
	}

	UpdateCameraView(camera);
}

void T3Camera_SetupViewWithCamera( T3_CAMERA* camera )
{
	if (camera->proj_type==T3_CAMERA_PROJ_PERSPECTIVE)
	{
		RenderSystem::SetPerspectiveProjMatrix(camera->fovy, KLEIN_SCREEN_ASPECT, camera->znear, camera->zfar);
		//RenderSystem::SetPerspectiveProjMatrix(camera->fovy, KLEIN_SCREEN_ASPECT, .1f, 100.f);
	}
	else
	{
		RenderSystem::SetOrthoProjMatrix(T3_CAMERA_ORTHO_WIDTH, T3_CAMERA_ORTHO_HEIGHT, camera->znear, camera->zfar);
	}

	RenderSystem::SetViewMatrix(Vector3(camera->position, camera->world_position.z),
								Vector3(camera->position, 0.f));

}

void T3Camera_ActiveProjectionAnimation( T3_CAMERA* camera )
{
	// Start animation
	camera->animating = true;
	camera->anim_end_time = KleinGame()->GetSysTickCount() + T3_CAMERA_ANIM_TIME;
	camera->proj_type = T3_CAMERA_PROJ_PERSPECTIVE;
}
