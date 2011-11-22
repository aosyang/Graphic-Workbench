/********************************************************************
	created:	2011/11/05
	filename: 	GWLog.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "GWLog.h"

#include <stdio.h>
#include <stdarg.h>

#if !defined DISABLE_GW_LOG

static FILE* log_file;

void GWLog_StartRecord( const char* log_filename )
{
	log_file = fopen(log_filename, "w");
}

void GWLog_EndRecord()
{
	fclose(log_file);
}

int GWLog_Print( const char* format, ... )
{
	va_list ap; 
	va_start(ap, format); 
	int n = vfprintf(log_file, format, ap); 
	va_end(ap); 

	fflush(log_file);

	return n; 
}

#endif	// #if !defined DISABLE_GW_LOG
