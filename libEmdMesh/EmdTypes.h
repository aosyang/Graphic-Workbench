#ifndef EmdTypes_h__
#define EmdTypes_h__

typedef unsigned int uint32;

#ifdef UNICODE
typedef wchar_t EChar;
#else
typedef char EChar;
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)			if(x) { delete (x); x = 0; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if(x) { delete [] (x); x = 0; }
#endif


#endif // EmdTypes_h__