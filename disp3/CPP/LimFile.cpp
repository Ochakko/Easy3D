#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <shdelem.h>
#include <part.h>

#include <crtdbg.h>

#define DBGH
#include <dbg.h>

#include <LimFile.h>

enum {
	LIMH_ELEMHEADER,
	LIMH_JOINTNAME,
	LIMH_STARTX,
	LIMH_ENDX,
	LIMH_STARTY,
	LIMH_ENDY,
	LIMH_STARTZ,
	LIMH_ENDZ,
	LIMH_IGNOREFLAG,
	LIMH_ELEMEND,
	LIMH_MAX
};

static char s_limheader[50] = "RokDeBone2 LimitFile ver1.0.0.1\r\n";

static char s_elemheader[ LIMH_MAX ][ 20 ] = {
	"LIMELEM{\r\n",
	"JointName=",
	"StartX=",
	"EndX=",
	"StartY=",
	"EndY=",
	"StartZ=",
	"EndZ=",
	"IgnoreFlag=",
	"}\r\n"
};

CLimFile::CLimFile()
{
	InitParams();
}

CLimFile::~CLimFile()
{
	DestroyObjs();
}

int CLimFile::InitParams()
{
	m_lpsh = 0;
	m_lpth = 0;
	m_hwfile = INVALID_HANDLE_VALUE;

	ZeroMemory( &m_buf, sizeof( LIMBUF ) );
	m_buf.hfile = INVALID_HANDLE_VALUE;


	return 0;
}

int CLimFile::DestroyObjs()
{
	if( m_hwfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hwfile );
		SetEndOfFile( m_hwfile );
		CloseHandle( m_hwfile );
		m_hwfile = INVALID_HANDLE_VALUE;
	}

	if( m_buf.hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_buf.hfile );
		m_buf.hfile = INVALID_HANDLE_VALUE;
	}
	if( m_buf.buf ){
		free( m_buf.buf );
		m_buf.buf = 0;
	}


	return 0;
}

int CLimFile::Write2File( char* lpFormat, ... )
{
	if( !m_hwfile ){
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

	if( ret < 0 ){
		_ASSERT( 0 );
		return 1;
	}

	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hwfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );	
		return 1;
	}

	return 0;	
}

int CLimFile::SaveLimFile( CShdHandler* lpsh, CTreeHandler2* lpth, char* filename )
{
	int ret = 0;

	m_lpsh = lpsh;
	m_lpth = lpth;


	m_hwfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hwfile == INVALID_HANDLE_VALUE ){
		DbgOut( "limfile : SaveLimFile : CreateFile error !!! %s\n", filename );
		_ASSERT( 0 );
		ret = 1;
		goto savelimexit;
	}	
	SetEndOfFile( m_hwfile );//!!!!!!!!!!

	ret = Write2File( "%s", s_limheader );
	if( ret ){
		DbgOut( "limfile : SaveLimFile : Write header error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto savelimexit;
	}

	int seri;
	CShdElem* selem;
	CTreeElem2* telem;
	for( seri = 0; seri < lpsh->s2shd_leng; seri++ ){
		selem = (*m_lpsh)( seri );
		telem = (*m_lpth)( seri );

		if( selem->IsJoint() && (selem->type != SHDMORPH) ){

			ret = WriteLimElem( selem, telem );
			if( ret ){
				DbgOut( "limfile : SaveLimFile : WriteLimElem error !!!\n" );
				_ASSERT( 0 );
				ret = 1;
				goto savelimexit;
			}

		}
	}

	goto savelimexit;
savelimexit:
	if( m_hwfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hwfile );
		SetEndOfFile( m_hwfile );
		CloseHandle( m_hwfile );
		m_hwfile = INVALID_HANDLE_VALUE;
	}
	return ret;
}

int CLimFile::WriteLimElem( CShdElem* selem, CTreeElem2* telem )
{
	int ret;

	ret = Write2File( "%s", &(s_elemheader[LIMH_ELEMHEADER][0]) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( "\t%s%s\r\n", &(s_elemheader[LIMH_JOINTNAME][0]), telem->name );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	DVEC3 lim0, lim1;
	ret = selem->GetBoneLim01( &lim0, &lim1 );
	if( ret ){
		DbgOut( "limfile : WriteLimElem : se GetBoneLim01 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ignorelim;
	ret = selem->GetIgnoreLim( &ignorelim );
	if( ret ){
		DbgOut( "limfile : WriteLimElem : se GetIgnoreLim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = Write2File( "\t%s%f\r\n", &(s_elemheader[LIMH_STARTX][0]), lim0.x );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = Write2File( "\t%s%f\r\n", &(s_elemheader[LIMH_ENDX][0]), lim1.x );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = Write2File( "\t%s%f\r\n", &(s_elemheader[LIMH_STARTY][0]), lim0.y );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = Write2File( "\t%s%f\r\n", &(s_elemheader[LIMH_ENDY][0]), lim1.y );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = Write2File( "\t%s%f\r\n", &(s_elemheader[LIMH_STARTZ][0]), lim0.z );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = Write2File( "\t%s%f\r\n", &(s_elemheader[LIMH_ENDZ][0]), lim1.z );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = Write2File( "\t%s%d\r\n", &(s_elemheader[LIMH_IGNOREFLAG][0]), ignorelim );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( "%s\r\n", &(s_elemheader[LIMH_ELEMEND][0]) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CLimFile::LoadLimFile( HWND srchwnd, CShdHandler* lpsh, CTreeHandler2* lpth, char* filename )
{
	int ret = 0;

	m_lpsh = lpsh;
	m_lpth = lpth;

	ret = SetBuffer( filename );
	if( ret ){
		DbgOut( "limfile : LoadLimFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		goto ldlimexit;
	}

	ret = CheckFileHeader();
	if( ret ){
		::MessageBox( srchwnd, "ファイルのフォーマットが異なります。\n読み込めません。", "エラー", MB_OK );
		return 0;
	}

	while( m_buf.isend == 0 ){
		ret = FindLimElem();
		if( ret ){
			DbgOut( "limfile : LoadLimFile : FindLimElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( m_buf.isend == 0 ){
			ret = ReadLimElem();
			if( ret ){
				DbgOut( "limfile : LoadLimFile : ReadLimElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	goto ldlimexit;
ldlimexit:
	if( m_buf.hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_buf.hfile );
		m_buf.hfile = INVALID_HANDLE_VALUE;
	}
	if( m_buf.buf ){
		free( m_buf.buf );
		m_buf.buf = 0;
	}

	if( ret != 0 ){
		::MessageBox( srchwnd, "読み込み中にエラーが発生しました。\n詳しくはdbg.txtをご覧ください", "エラー", MB_OK );
	}

	return ret;
}

int CLimFile::SetBuffer( char* filename )
{
	DestroyObjs();

	HANDLE hfile;
	hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	
	m_buf.hfile = hfile;


	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_buf.hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "LimFile : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "LimFile : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut( "LimFile : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_buf.hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "LimFile : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_buf.buf = newbuf;
	*(m_buf.buf + bufleng) = 0;//!!!!!!!!
	m_buf.bufleng = bufleng;
	m_buf.startpos = 0;
	m_buf.endpos = 0;

	return 0;
}

int CLimFile::CheckFileHeader()
{
	char* headerptr = 0;
	headerptr = strstr( m_buf.buf, s_limheader );
	if( !headerptr ){
		return 1;
	}

	int headerleng;
	headerleng = (int)strlen( s_limheader );

	m_buf.startpos = 0;
	m_buf.endpos = headerleng - 1;

	return 0;
}

int CLimFile::FindLimElem()
{
	if( (m_buf.endpos + 1) >= m_buf.bufleng ){
		m_buf.isend = 1;
		return 0;
	}

	m_buf.startpos = m_buf.endpos + 1;

	char* startptr = 0;
	startptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_ELEMHEADER ][0]) );
	if( !startptr ){
		m_buf.isend = 1;
		return 0;
	}

	m_buf.startpos = (int)( startptr - m_buf.buf );

	char* endptr = 0;
	endptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_ELEMEND ][0]) );
	if( endptr ){
		m_buf.endpos = (int)( endptr - m_buf.buf - 1 );
	}else{
		m_buf.endpos = m_buf.bufleng - 1;
	}

	return 0;	
}
int CLimFile::ReadLimElem()
{

	int ret;

// jointname
	char* findptr = 0;
	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_JOINTNAME ][0]) );
	if( !findptr ){
		return 0;
	}

	int findpos;
	findpos = (int)( findptr - m_buf.buf );
	if( findpos >= m_buf.endpos ){
		return 0;
	}

	char* elemptr;
	elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_JOINTNAME ][0]) );

	char jointname[256];
	ret = GetName( elemptr, jointname, 256 );
	if( ret ){
		DbgOut( "limfile : ReadLimElem : GetName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	DVEC3 lim0 = { -179.0, -179.0, -179.0 };
	DVEC3 lim1 = { 179.0, 179.0, 179.0 };

// startx	
	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_STARTX ][0]) );
	if( findptr ){
		findpos = (int)( findptr - m_buf.buf );
		if( findpos < m_buf.endpos ){
			elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_STARTX ][0]) );

			ret = GetDouble( elemptr, &lim0.x );
			if( ret ){
				DbgOut( "limfile : ReadLimElem : GetDouble lim0.x error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
// endx
	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_ENDX ][0]) );
	if( findptr ){
		findpos = (int)( findptr - m_buf.buf );
		if( findpos < m_buf.endpos ){
			elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_ENDX ][0]) );

			ret = GetDouble( elemptr, &lim1.x );
			if( ret ){
				DbgOut( "limfile : ReadLimElem : GetDouble lim1.x error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


// starty	
	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_STARTY ][0]) );
	if( findptr ){
		findpos = (int)( findptr - m_buf.buf );
		if( findpos < m_buf.endpos ){
			elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_STARTY ][0]) );

			ret = GetDouble( elemptr, &lim0.y );
			if( ret ){
				DbgOut( "limfile : ReadLimElem : GetDouble lim0.y error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
// endy
	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_ENDY ][0]) );
	if( findptr ){
		findpos = (int)( findptr - m_buf.buf );
		if( findpos < m_buf.endpos ){
			elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_ENDY ][0]) );

			ret = GetDouble( elemptr, &lim1.y );
			if( ret ){
				DbgOut( "limfile : ReadLimElem : GetDouble lim1.y error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

// startz	
	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_STARTZ ][0]) );
	if( findptr ){
		findpos = (int)( findptr - m_buf.buf );
		if( findpos < m_buf.endpos ){
			elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_STARTZ ][0]) );

			ret = GetDouble( elemptr, &lim0.z );
			if( ret ){
				DbgOut( "limfile : ReadLimElem : GetDouble lim0.z error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
// endz
	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_ENDZ ][0]) );
	if( findptr ){
		findpos = (int)( findptr - m_buf.buf );
		if( findpos < m_buf.endpos ){
			elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_ENDZ ][0]) );

			ret = GetDouble( elemptr, &lim1.z );
			if( ret ){
				DbgOut( "limfile : ReadLimElem : GetDouble lim1.z error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

// ignoreflag
	int ignoreflag = 1;

	findptr = strstr( m_buf.buf + m_buf.startpos, &(s_elemheader[ LIMH_IGNOREFLAG ][0]) );
	if( findptr ){
		findpos = (int)( findptr - m_buf.buf );
		if( findpos < m_buf.endpos ){
			elemptr = findptr + (int)strlen( &(s_elemheader[ LIMH_IGNOREFLAG ][0]) );

			ret = GetInt( elemptr, &ignoreflag );
			if( ret ){
				DbgOut( "limfile : ReadLimElem : GetInt error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

// check
	double tmpd1;
	if( lim0.x > lim1.x ){
		tmpd1 = lim0.x;
		lim0.x = lim1.x;
		lim1.x = tmpd1;
	}
	if( lim0.y > lim1.y ){
		tmpd1 = lim0.y;
		lim0.y = lim1.y;
		lim1.y = tmpd1;
	}
	if( lim0.z > lim1.z ){
		tmpd1 = lim0.z;
		lim0.z = lim1.z;
		lim1.z = tmpd1;
	}

// set
	int seri;
	ret = m_lpth->GetBoneNoByName( jointname, &seri, m_lpsh, 0 );
	if( ret ){
		DbgOut( "limfile : ReadLimElem : th GetBoneNoByName error skip!!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!
	}

	if( seri <= 0 ){
		DbgOut( "limfile : ReadLimElem : jointname %s is not found skip !!!\n", jointname );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!
	}

	CShdElem* selem;
	selem = (*m_lpsh)( seri );
	_ASSERT( selem );

	ret = selem->SetBoneLim01( &lim0, &lim1 );
	if( ret ){
		DbgOut( "limfile : ReadLimFile : se SetBoneLim01 error skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!
	}

	ret = selem->SetIgnoreLim( ignoreflag );
	if( ret ){
		DbgOut( "limfile : ReadLimFile : se SetIgnoreLim error skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!
	}

	return 0;
}


int CLimFile::GetName( char* srcptr, char* dstptr, int dstleng )
{
	char* endptr;
	endptr = strstr( srcptr,"\r\n" );
	if( !endptr ){
		DbgOut( "limfile : GetName : return mark not find error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)( endptr - srcptr );

	if( leng >= dstleng ){
		DbgOut( "limfile : GetName : line leng too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( dstptr, dstleng, srcptr, leng );
	*( dstptr + leng ) = 0;

	return 0;
}

int CLimFile::GetDouble( char* srcptr, double* dstptr )
{
	char* endptr;
	endptr = strstr( srcptr,"\r\n" );
	if( !endptr ){
		DbgOut( "limfile : GetDouble : return mark not find error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)( endptr - srcptr );

	if( leng >= 256 ){
		DbgOut( "limfile : GetDouble : line leng too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char strline[ 256 ];
	ZeroMemory( strline, sizeof( char ) * 256 );
	strncpy_s( strline, 256, srcptr, leng );

	*dstptr = atof( strline );

	return 0;
}

int CLimFile::GetInt( char* srcptr, int* dstptr )
{
	char* endptr;
	endptr = strstr( srcptr,"\r\n" );
	if( !endptr ){
		DbgOut( "limfile : GetInt : return mark not find error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)( endptr - srcptr );

	if( leng >= 256 ){
		DbgOut( "limfile : GetInt : line leng too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char strline[ 256 ];
	ZeroMemory( strline, sizeof( char ) * 256 );
	strncpy_s( strline, 256, srcptr, leng );

	*dstptr = atoi( strline );

	return 0;
}
