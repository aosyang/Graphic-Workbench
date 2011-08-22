#ifndef DllExport_h__
#define DllExport_h__

#ifdef DLL_PROJECT
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define DLL_EXPORT_CLASS __declspec(dllexport)
#else
#define DLL_EXPORT extern "C" __declspec(dllimport)
#define DLL_EXPORT_CLASS __declspec(dllimport)
#endif

#endif // DllExport_h__