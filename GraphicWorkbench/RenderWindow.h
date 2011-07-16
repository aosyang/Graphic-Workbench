#ifndef RenderWindow_h__
#define RenderWindow_h__

#include "GWTypes.h"
#include "Platform.h"

#ifdef __PLATFORM_WIN32
typedef HWND RenderWindowHandle;
#elif defined __PLATFORM_LINUX
typedef struct
{
	void*              display;
	int         	   screen;
	unsigned long      window;
} RenderWindowHandle;
#endif

struct RenderWindowParam
{
	RenderWindowHandle	handle;
	unsigned int		width, height;
	unsigned int		colorDepthBit;
};

class RenderWindow
{
public:
	RenderWindow();
	~RenderWindow();

	bool Create(const GWChar* titleName, uint32 width, uint32 height, uint32 depthBits, bool fullscreen);
	void Destroy();

	RenderWindowParam* GetWindowHandle();

	bool HandleWindowMessage();

protected:
	RenderWindowParam	m_RenderWindowParam;

	bool				m_FullScreen;
	String				m_TitleName;
};

#endif // RenderWindow_h__