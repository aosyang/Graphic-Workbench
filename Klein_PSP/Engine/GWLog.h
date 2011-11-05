/********************************************************************
	created:	2011/11/05
	filename: 	GWLog.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef GWLog_h__
#define GWLog_h__

#define DISABLE_GW_LOG

#if !defined DISABLE_GW_LOG

void GWLog_StartRecord(const char* log_filename);
void GWLog_EndRecord();

int GWLog_Print(const char* format, ...);

#else

#define GWLog_StartRecord(x)
#define GWLog_EndRecord()
#define GWLog_Print(x, ...)

#endif

#endif // GWLog_h__