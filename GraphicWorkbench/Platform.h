#ifndef Platform_h__
#define Platform_h__

#if defined _WIN32
#define __PLATFORM_WIN32
#include <windows.h>
#elif defined _LINUX
#define __PLATFORM_LINUX
#else
#error Unsupported platform
#endif

#ifdef __PLATFORM_WIN32
#define DLLEXPORT __declspec(dllexport)
#elif defined __PLATFORM_LINUX
#define DLLEXPORT __attribute__ ((visibility("default")))
#endif

#endif // Platform_h__