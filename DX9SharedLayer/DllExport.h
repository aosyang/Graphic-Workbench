#ifndef DllExport_h__
#define DllExport_h__

#if 0
	#ifdef DLL_PROJECT
	#define DLL_EXPORT extern "C" __declspec(dllexport)
	#define DLL_EXPORT_CLASS __declspec(dllexport)
	#else
	#define DLL_EXPORT extern "C" __declspec(dllimport)
	#define DLL_EXPORT_CLASS __declspec(dllimport)
	#endif
#else
	#define DLL_EXPORT
	#define DLL_EXPORT_CLASS
#endif // if 0

#endif // DllExport_h__