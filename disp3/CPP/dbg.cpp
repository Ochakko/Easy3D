#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>

#include <crtdbg.h>

#ifdef DBGH
#undef DBGH
#endif
#include <dbg.h>

#define DBGLENG 7000

int DbgOut( char* lpFormat, ... )
{
	if( !dbgfile ){
		//_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[DBGLENG];
			
	ZeroMemory( outchar, DBGLENG );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, DBGLENG, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	int findrep = 0;
	int curlen;
	curlen = (int)strlen( outchar );
	if( (curlen >= 1) && (outchar[curlen - 1] == '\n') ){
		if( curlen == 1 ){
			findrep = 1;
		}else{
			if( outchar[curlen - 2] != '\r' ){
				findrep = 1;
			}
		}

		if( findrep && (curlen <= (DBGLENG - 2)) ){
			outchar[curlen - 1] = '\r';
			outchar[curlen] = '\n';
			outchar[curlen + 1] = 0;
		}
	}

	wleng = (unsigned long)strlen( outchar );
	WriteFile( dbgfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}

	return 0;
	
}

int DbgOutB( void* lpvoid, int datsize )
{
	if( !dbgfile )
		return 0;

	unsigned long writeleng;

	WriteFile( dbgfile, lpvoid, datsize, &writeleng, NULL );
	if( datsize != (long)writeleng ){
		return 1;
	}

	return 0;

}

int Write2File( HANDLE hfile, char* lpFormat, ... )
{
	return 0;

}

int SetTabChar( int tabnum )
{

	return 0;
}

void ErrorMessage(char* szMessage, HRESULT hr)
{
    if(HRESULT_FACILITY(hr) == FACILITY_WINDOWS)
		hr = HRESULT_CODE(hr);

	char* szError;
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&szError, 0, NULL) != 0)
	{
		DbgOut("ErrorMessage : %s: %s", szMessage, szError);

		MessageBox( NULL, szError, szMessage, MB_OK );

		LocalFree(szError);
	}
	else
		DbgOut("ErrorMessage : Error number not found\n");
}

int OpenDbgFile()
{
	if( dbgfile )
		return 1;

	dbgfile = CreateFile( (LPCTSTR)"dbg.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( dbgfile == INVALID_HANDLE_VALUE ){
		return 1;
	}

	SetEndOfFile( dbgfile );

	DbgOut( "InitFunc: dbgfile created\n" );
		
	return 0;
}

int CloseDbgFile()
{
	if( dbgfile ){
		
		DbgOut( "CloseDbgFile\n" );

		FlushFileBuffers( dbgfile );
		SetEndOfFile( dbgfile );
		CloseHandle( dbgfile );
		dbgfile = 0;
	}
	return 0;
}