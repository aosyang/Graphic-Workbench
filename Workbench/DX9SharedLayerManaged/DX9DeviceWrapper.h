#ifndef DX9DeviceWrapper_h__
#define DX9DeviceWrapper_h__

#include <vcclr.h>

#include "../DX9SharedLayer/DX9Device.h"

using namespace System;

namespace DX9ToolManaged
{
	public ref class DX9DeviceWrapper
	{
	public:
		DX9DeviceWrapper() : m_Impl(new DX9Device()) {}
		~DX9DeviceWrapper() { delete m_Impl; }

		bool Init(IntPtr hWnd);

		void Clear();
		void BeginScene();
		void EndScene();
		void Present();

		void SetWindowSize(Int32 width, Int32 height);
		void ResetDevice();

		void SetupView();

	protected:
		DX9Device*	m_Impl;
	};
}

#endif // DX9DeviceWrapper_h__
