#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include "writefile.h"

CWriteFile::CWriteFile( char* filename )
{
	InitParams();
	strcpy_s( m_filename, MAX_PATH, filename );
	SetBuffer();
}
CWriteFile::~CWriteFile()
{
	DestroyObjs();
}

int CWriteFile::Write2File( char* lpFormat, ... )
{
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[2048];
			
	ZeroMemory( outchar, 2048 );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, 2048, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}

	return 0;	
}

int CWriteFile::InitParams()
{
	//file‘€ì—p
	m_hfile = INVALID_HANDLE_VALUE;
	m_buf = 0;
	m_pos = 0;
	m_bufleng = 0;

	return 0;
}

int CWriteFile::DestroyObjs()
{

	if( m_hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hfile );
		SetEndOfFile( m_hfile );

		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}

	if( m_buf ){
		free( m_buf );
		m_buf = 0;
	}

	m_pos = 0;
	m_bufleng = 0;

	return 0;
}

int CWriteFile::SetBuffer()
{

	m_hfile = CreateFile( (LPCTSTR)m_filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	
 
	SetEndOfFile( m_hfile );

	m_buf = 0;
	m_bufleng = 0;
	m_pos = 0;	

	return 0;
}
