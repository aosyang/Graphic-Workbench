#ifndef GWTypes_h__
#define GWTypes_h__

#include <string>

typedef unsigned int uint32;

#ifdef UNICODE
typedef wchar_t GWChar;
typedef std::wstring String;
#else
typedef char GWChar;
typedef std::string String;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)			if(x) { delete (x); x = 0; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if(x) { delete [] (x); x = 0; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)			if(x) { (x)->Release(); x = 0; }
#endif

#endif // GWTypes_h__