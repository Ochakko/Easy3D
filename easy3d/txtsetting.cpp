#include "stdafx.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>


//#include <coef.h>
#include "txtsetting.h"

#include "handlerset.h"
#include <treehandler2.h>
#include <treeelem2.h>

#define DBGH
#include <dbg.h>

static char s_fileheader[256] = "Rps2Qua:ConvSettingFile:ver1001:type01\r\n";


enum {
	KSTMODE_LOAD,
	KSTMODE_WRITE,
	KSTMODE_MAX
};


CTxtSetting::CTxtSetting( CHandlerSet* hsptr )
{
	InitParams();
	m_hs = hsptr;
}
CTxtSetting::~CTxtSetting()
{
	DestroyObjs();
}

int CTxtSetting::InitParams()
{
	//file操作用
	m_hfile = INVALID_HANDLE_VALUE;
	m_buf = 0;
	m_pos = 0;
	m_bufleng = 0;

	m_filemode = KSTMODE_LOAD;

	int skelno;
	for( skelno = 0; skelno < SKEL_MAX; skelno++ ){
		TSELEM* curelem;
		curelem = m_elem + skelno;
		curelem->skelno = skelno;
		ZeroMemory( curelem->jointname, sizeof( char ) * 256 );
		curelem->jointno = -1;
		curelem->twistflag = 1;
	}

	return 0;
}
int CTxtSetting::DestroyObjs()
{

	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_filemode == KSTMODE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
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


int CTxtSetting::LoadParams( char* filename )
{
	int ret, ret1;
	DestroyObjs();
	InitParams();

	m_filemode = KSTMODE_LOAD;

	ret = SetBuffer( filename );
	if( ret == 0 ){
		int skelno;
		char lineheader[256];
		for( skelno = 0; skelno < SKEL_MAX; skelno++ ){
			ZeroMemory( lineheader, sizeof( char ) * 256 );
			sprintf_s( lineheader, 256, "%s, ", &(strconvskel[skelno][0]) );
			ret1 = GetElemLine( lineheader );
			if( ret1 ){
				_ASSERT( 0 );
				return 1;
			}
			

			char jointname[256];
			ZeroMemory( jointname, sizeof( char ) * 256 );
			int namesetflag = 0;

			char* headptr = m_line;
			headptr = GetName( headptr, jointname, 256, &namesetflag );
			if( namesetflag == 0 ){
				_ASSERT( 0 );
				return 1;
			}
			
			int jointno = -1;
			ret1 = m_hs->m_thandler->GetBoneNoByName( jointname, &jointno, m_hs->m_shandler, 0 );
			if( ret1 || (jointno <= 0) ){
				_ASSERT( 0 );
				return 1;
			}

			char strtwist[256];
			ZeroMemory( strtwist, sizeof( char ) * 256 );
			int twistsetflag = 0;
			headptr = GetName( headptr, strtwist, 256, &twistsetflag );
			if( twistsetflag == 0 ){
				_ASSERT( 0 );
				return 1;
			}

			int twistflag = 1;
			int cmp;
			cmp = strcmp( strtwist, "nottwist" );
			if( cmp == 0 ){
				twistflag = 0;
			}

			TSELEM* curelem = m_elem + skelno;
			if( curelem->skelno != skelno ){
				_ASSERT( 0 );
				return 1;
			}
			strcpy_s( curelem->jointname, 256, jointname );
			curelem->jointno = jointno;
			curelem->twistflag = twistflag;

//E3DDbgOut( "check!!! : skel %s, jointname %s, jointno %d, twistflag %d\r\n",
//	strconvskel[ skelno ], curelem->jointname, curelem->jointno, curelem->twistflag );

		}

	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CTxtSetting::SetBuffer( char* filename )
{
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	

	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "TxtSetting : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "TxtSetting : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * bufleng + 1 );//bufleng + 1
	if( !newbuf ){
		DbgOut( "TxtSetting : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "TxtSetting : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_buf = newbuf;
	*(m_buf + bufleng) = 0;//!!!!!!!!
	m_bufleng = bufleng;
	m_pos = 0;	

	return 0;
}


int CTxtSetting::GetElemLine( char* strpat )
{
	ZeroMemory( m_line, sizeof( char ) * TXTLINELEN );

	char* headptr;
	headptr = strstr( m_buf, strpat );
	if( !headptr ){
		_ASSERT( 0 );
		return 1;
	}

	headptr += (int)strlen( strpat );
	m_pos = (int)( headptr - m_buf );

	char* tailptr;
	tailptr = strstr( headptr, "\r\n" );
	if( !tailptr ){
		_ASSERT( 0 );
		return 1;
	}
	
	int linelen;
	linelen = (int)( tailptr - headptr );
	if( linelen >= TXTLINELEN ){
		_ASSERT( 0 );
		return 1;
	}
	strncpy_s( m_line, TXTLINELEN, headptr, linelen );

	return 0;
}



char* CTxtSetting::GetName( char* srcstr, char* dststr, int maxleng, int* dstsetflag )
{
	char* valuehead = srcstr;


	//開始の"を検索
	valuehead = strstr( valuehead, "\"" );
	if( valuehead == NULL ){
		*dstsetflag = 0;
		return 0;
	}

	char* valueend;
	//終了の"を検索
	valueend = strstr( valuehead + 1, "\"" );
	if( valueend == NULL ){
		*dstsetflag = 0;
		return 0;
	}

	int valueleng;
	valueleng = (int)( valueend - (valuehead + 1) );

	if( (valueleng >= maxleng) || (valueleng <= 0) ){
		*dstsetflag = 0;
		return 0;
	}

	strncpy_s( dststr, maxleng, valuehead + 1, valueleng );
	*( dststr + valueleng ) = 0;
	*dstsetflag = 1;//

	return (valueend + 1);

}
int CTxtSetting::Write2File( char* lpFormat, ... )
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

int CTxtSetting::SaveKstFile( char* filename )
{
	m_filemode = KSTMODE_WRITE;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}
	SetEndOfFile( m_hfile );


	int ret;
	ret = Write2File( s_fileheader );
	_ASSERT( !ret );
	ret = Write2File( "\r\n" );
	_ASSERT( !ret );


	int skno;
	for( skno = 0; skno < SKEL_MAX; skno++ ){
		TSELEM* curts;
		curts = ( m_elem + skno );

		ZeroMemory( m_line, sizeof( char ) * TXTLINELEN );

		char strtwist[20];
		if( curts->twistflag == 1 ){
			strcpy_s( strtwist, 20, "twist" );
		}else{
			strcpy_s( strtwist, 20, "nottwist" );
		}

		sprintf_s( m_line, TXTLINELEN, "%s, \"%s\", \"%s\"\r\n",
			strconvskel[skno], curts->jointname, strtwist );

		ret = Write2File( m_line );
		_ASSERT( !ret );

	}


	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_filemode == KSTMODE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}

	return 0;
}
