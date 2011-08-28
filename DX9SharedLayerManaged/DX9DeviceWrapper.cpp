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

	void DX9DeviceWrapper::SetWindowSize( Int32 width, Int32 height )
	{
		m_Impl->SetWindowSize(width, height);
	}

	void DX9DeviceWrapper::ResetDevice()
	{
		m_Impl->ResetDevice();
	}

	void DX9DeviceWrapper::SetupView()
	{
		D3DXVECTOR3 vEyePt( 0.0f, 5.0f, -5.0f );
		D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

		m_Impl->SetViewMatrix(vEyePt, vLookatPt, vUpVec);

		m_Impl->SetProjectionMatrix(D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	}

}