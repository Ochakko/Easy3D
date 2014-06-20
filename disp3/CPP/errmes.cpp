#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef	ERRMESH
	#include "errmes.h"
#endif


CErrMes::CErrMes()
{

}

CErrMes::~CErrMes()
{

}

char* CErrMes::GetErrorMes( HRESULT	hr )
{
	if( HRESULT_FACILITY(hr) == FACILITY_WINDOWS )
		hr = HRESULT_CODE(hr);

	DWORD	dwret;
	char*	szError = 0;

	dwret = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr,
			MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&szError, 0, NULL );
		
	if( dwret ){
		return szError;
	}else{
		return 0;
	}
	
}

void CErrMes::ErrStdOut( char* szMessage, HRESULT hr )
{
	char* szError = 0;

	szError = GetErrorMes( hr );
	if( hr )
	{
		fprintf( stdout, "CErrMes : %s : %s\n", szMessage, szError );
		LocalFree( szError );
	}else{
		fprintf( stdout, "CErrMes : (%s) : FormatMessage Error!!!!\n", szMessage );
	}
}

void CErrMes::ErrTrace(  char* szMessage, HRESULT hr )
{
	char* szError = 0;

	szError = GetErrorMes( hr );
	if( hr )
	{
		TRACE( "CErrMes : %s : %s\n", szMessage, szError );
		LocalFree( szError );
	}else{
		TRACE( "CErrMes : (%s) : FormatMessage Error!!!!\n", szMessage );
	}
}

/***
void CErrMes::Win32Error( char* szMessage, HRESULT hr )
{
	if( HRESULT_FACILITY(hr) == FACILITY_WINDOWS )
		hr = HRESULT_CODE(hr);

	char*	szError;
	if( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr,
			MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&szError, 0, NULL ) 
		!= 0 )
	{
		//fprintf( stdout, "ERROR : %s %s\n", szMessage, szError );
		ErrPrint( szMessage, FREE_NOT );
		ErrPrint( szError, FREE_NOT );
		LocalFree( szError );
	}
}
***/