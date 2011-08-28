#include "StdAfx.h"
#include "DX9DeviceWrapper.h"

namespace DX9ToolManaged
{
	bool DX9DeviceWrapper::Init( IntPtr hWnd )
	{
		return m_Impl->Init((HWND)(hWnd.ToPointer()));
	}

	void DX9DeviceWrapper::Clear()
	{
		m_Impl->Clear();
	}

	void DX9DeviceWrapper::BeginScene()
	{
		m_Impl->BeginScene();
	}

	void DX9DeviceWrapper::EndScene()
	{
		m_Impl->EndScene();
	}

	void DX9DeviceWrapper::Present()
	{
		m_Impl->Present();
	}

	void DX9DeviceWrapper::SetViewMatrix( const D3DXVECTOR3& vEyePt, const D3DXVECTOR3& vLookatPt, const D3DXVECTOR3& vUpVec )
	{
		m_Impl->SetViewMatrix(vEyePt, vLookatPt, vUpVec);
	}

}