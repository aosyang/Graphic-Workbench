#include "StdAfx.h"
#include "DX9MeshBufferManagedWrapper.h"

namespace DX9ToolManaged
{
	HRESULT DX9ToolManaged::DX9MeshBufferManagedWrapper::CreateFromFile( System::String ^ filename )
	{
		pin_ptr<const wchar_t> name = PtrToStringChars(filename);
		return m_Impl->CreateFromFile(name);
	}


	void DX9MeshBufferManagedWrapper::Render()
	{
		m_Impl->Render();
	}

}
