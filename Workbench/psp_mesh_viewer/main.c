/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jesper Svennevid
 */

// This file is modified from original sample gu/cube from PSPSDK

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

#include <pspgu.h>
#include <pspgum.h>

#include "callbacks.h"
#include "vram.h"

#include "../libemd/emd_mesh.h"

PSP_MODULE_INFO("Cube Sample", 0, 1, 1);
PSP_HEAP_SIZE_MAX();

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

static unsigned int __attribute__((aligned(16))) list[262144];

typedef struct VERTEX_TYPE
{
	float u, v;
	//unsigned int color;
	float nx, ny, nz;
	float x,y,z;
} VERTEX;

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

int main(int argc, char* argv[])
{
	setupCallbacks();

	int i, j;

	// Load emd mesh from file
	EMD_MESH* mesh = EMD_LoadMeshFromFile("scene.emd");
	int vert_count = EMD_GetVertexCount(mesh);
	int idx_count = 0;

	int elem_count = EMD_GetElementCount(mesh);
	for (i=0; i<elem_count; i++)
	{
		idx_count += EMD_GetIndexCount(mesh, i);
	}

	VERTEX* vert_buf = (VERTEX*)memalign(16, sizeof(VERTEX) * vert_count);
	GW_UINT16* idx_buf = (GW_UINT16*)memalign(16, sizeof(GW_UINT16) * idx_count);

	for (i=0; i<vert_count; i++)
	{
		EMD_GetVertexByIndex(mesh, i, &vert_buf[i].x, &vert_buf[i].y, &vert_buf[i].z);
		EMD_GetTexcoordByIndex(mesh, i, &vert_buf[i].u, &vert_buf[i].v);
		EMD_GetNormalByIndex(mesh, i, &vert_buf[i].nx, &vert_buf[i].ny, &vert_buf[i].nz);
		//vert_buf[i].color = 0xffffffff;
	}

	GW_UINT32 idx_head = 0;
	GW_UINT32* data;
	GW_UINT32 count;

	// Pack 32-bit index into 16-bit buffer
	for (i=0; i<elem_count; i++)
	{
		count = EMD_GetIndexCount(mesh, i);
		data = EMD_GetIndexArray(mesh, i);
		for (j=0; j<count; j++)
		{
			idx_buf[j + idx_head] = (GW_UINT16)data[j];
		}

		idx_head += count;
	}

	EMD_FreeMesh(mesh);


	// flush cache so that no stray data remains

	sceKernelDcacheWritebackAll();

	// setup GU

	void* fbp0 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* zbp = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

	pspDebugScreenInit();
	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
	sceGuDepthBuffer(zbp,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	//sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuEnable(GU_LIGHTING);
	sceGuEnable(GU_LIGHT0);
	sceGuEnable(GU_LIGHT1);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	// run sample

	int val = 0;

	while(running())
	{
		sceGuStart(GU_DIRECT,list);

		// clear screen

		sceGuClearColor(0xff554433);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

		
		// setup lights

		ScePspFVector3 light_dir = { 1.0f, 1.0f, 1.0f };

		// GU_DIRECTIONAL
		// GU_POINTLIGHT
		sceGuLight(0, GU_POINTLIGHT, GU_DIFFUSE, &light_dir);
		sceGuLightColor(0, GU_DIFFUSE, 0xffffffff);
		sceGuAmbient(0x00202020);

		light_dir.x = -light_dir.x;
		light_dir.y = -light_dir.y;
		light_dir.z = -light_dir.z;

		sceGuLight(1, GU_DIRECTIONAL, GU_DIFFUSE_AND_SPECULAR, &light_dir);
		sceGuLightColor(1, GU_DIFFUSE, 0xff7f7f7f);
		

		// setup matrices for cube

		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		sceGumPerspective(75.0f,16.0f/9.0f,0.5f,1000.0f);

		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();

		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		{
			ScePspFVector3 pos = { 0, 0, -5.0f };
			ScePspFVector3 rot = { val * 0.79f * (GU_PI/180.0f), val * 0.98f * (GU_PI/180.0f), val * 1.32f * (GU_PI/180.0f) };
			sceGumTranslate(&pos);
			sceGumRotateXYZ(&rot);
		}

		// setup texture

		//sceGuTexMode(GU_PSM_4444,0,0,0);
		// sceGuTexImage(0,64,64,64,logo_start);
		//sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
		//sceGuTexEnvColor(0xffff00);
		//sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		//sceGuTexScale(1.0f,1.0f);
		//sceGuTexOffset(0.0f,0.0f);
		//sceGuAmbientColor(0xff7f7f7f);

		// draw cube

		sceGuColor(0xffffff);
		sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_INDEX_16BIT|GU_TRANSFORM_3D,
				idx_count, idx_buf, vert_buf);
		

		pspDebugScreenSetXY(0, 0);
		pspDebugScreenPrintf("v: %d", vert_count);
		pspDebugScreenSetXY(0, 1);
		pspDebugScreenPrintf("i: %d", idx_count);

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();

		val++;
	}

	sceGuTerm();

	// Release buffers
	free(vert_buf);
	free(idx_buf);

	sceKernelExitGame();
	return 0;
}
