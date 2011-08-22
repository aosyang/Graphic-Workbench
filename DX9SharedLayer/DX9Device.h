#ifndef DX9Device_h__
#define DX9Device_h__

#include <d3d9.h>

#include "DllExport.h"

DLL_EXPORT LPDIRECT3DDEVICE9& D3DDevice();

#ifndef V
#define V(x)           { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

#endif // DX9Device_h__