#ifndef DX9SharedLayer_h__
#define DX9SharedLayer_h__

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#if defined _DEBUG
#pragma comment (lib, "d3dx9d.lib")
#else
#pragma comment (lib, "d3dx9.lib")
#endif

#include "DX9Device.h"
#include "DX9MeshBuffer.h"

#endif // DX9SharedLayer_h__