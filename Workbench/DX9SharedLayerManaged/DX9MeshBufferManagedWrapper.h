#ifndef DX9MeshBufferManagedWrapper_h__
#define DX9MeshBufferManagedWrapper_h__

#include <vcclr.h>

#include "../DX9SharedLayer/DX9MeshBuffer.h"

namespace DX9ToolManaged
{
	public ref class DX9MeshBufferManagedWrapper
	{
	public:
		DX9MeshBufferManagedWrapper() : m_Impl(new DX9MeshBuffer()) {}
		~DX9MeshBufferManagedWrapper() { delete m_Impl; }

		HRESULT CreateFromFile(System::String ^ filename);

		void Render();

	private:
		DX9MeshBuffer*	m_Impl;
	};
}
#endif // DX9MeshBufferManagedWrapper_h__
