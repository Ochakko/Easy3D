#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include <SndFile.h>

#include <SndAnimHandler.h>
#include <SndAnim.h>
#include <SndKey.h>

#include "c:\pgfile9\RokDeBone2DX\SoundBank.h"
#include "c:\pgfile9\RokDeBone2DX\SoundSet.h"
#include "c:\pgfile9\RokDeBone2DX\SoundElem.h"

#include <crtdbg.h>

enum {
	SNDFILE_WRITE,
	SNDFILE_LOAD,
	SNDFILE_MAX
};

static char strheader[256] = "RokDeBone2 SndAnim File ver1001 type0\r\n";

static char stranimstart[256] = "#AnimInfo Start\r\n";
static char stranimend[256] = "#AnimInfo End\r\n";

static char strsndstart[256] = "#SndAnim Start\r\n";
static char strsndend[256] = "#SndAnim End\r\n";

static char strsndkeystart[256] = "#SndKey Start\r\n";
static char strsndkeyend[256] = "#SndKey End\r\n";

static char strsndelemstart[256] = "#SndElem Start\r\n";
static char strsndelemend[256] = "#SndElem End\r\n";

static char strendfile[256] = "#EndOfFile\r\n";


CSndFile::CSndFile()
{
	InitParams();
}
CSndFile::~CSndFile()
{
	DestroyObjs();
}

int CSndFile::InitParams()
{
	m_mode = SNDFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_sndbuf.buf = 0;
	m_sndbuf.bufleng = 0;
	m_sndbuf.pos = 0;
	m_sndbuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * SNDLINELEN );
	m_anim = 0;
	m_loadversion = 0;
	return 0;
}
int CSndFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == SNDFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_sndbuf.buf ){
		free( m_sndbuf.buf );
		m_sndbuf.buf = 0;
	}
	m_sndbuf.bufleng = 0;
	m_sndbuf.pos = 0;
	m_sndbuf.isend = 0;

	return 0;
}

int CSndFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[SNDLINELEN];
			
	ZeroMemory( outchar, SNDLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, SNDLINELEN, lpFormat, Marker );
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

int CSndFile::WriteSndFile( char* filename, CSndAnim* srcanim, CSoundSet* ssptr )
{
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "SndFile : WriteSndFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_anim = srcanim;
	if( !m_anim ){
		_ASSERT( 0 );
		return 1;
	}
	m_ss = ssptr;
	if( !m_ss ){
		_ASSERT( 0 );
		return 1;
	}

	m_anim->CheckAndDelInvalid( m_ss );//チェック。不正データ削除。

	int ret;
	ret = Write2File( strheader );
	_ASSERT( !ret );

	ret = WriteAnimInfo();
	if( ret ){
		DbgOut( "SndFile : WriteSndFile : WriteAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteSndAnim();
	if( ret ){
		DbgOut( "SndFile : WriteSndFile : WriteSndAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = Write2File( strendfile );
	_ASSERT( !ret );

	return 0;
}

int CSndFile::WriteAnimInfo()
{
/***
#AnimInfo Start
	$animname "sndanim_1"
	$sndsetname "test2"
	$fps 60
	$loop 1
#AnimInfo End
***/

	int ret;
	ret = Write2File( stranimstart );
	_ASSERT( !ret );

	ret = Write2File( "\t$animname \"%s\"\r\n", m_anim->m_animname );
	_ASSERT( !ret );

	ret = Write2File( "\t$sndsetname \"%s\"\r\n", m_anim->m_ssname );
	_ASSERT( !ret );

	ret = Write2File( "\t$fps %d\r\n", m_anim->m_fps );
	_ASSERT( !ret );

	int wloop = 0;
	if( m_anim->m_bgmloop == 0 ){
		wloop = 0;
	}else{
		wloop = 1;
	}
	ret = Write2File( "\t$loop %d\r\n", wloop );
	_ASSERT( !ret );



	ret = Write2File( stranimend );
	_ASSERT( !ret );

	return 0;
}
int CSndFile::WriteSndAnim()
{
	int ret;

	ret = Write2File( &(strsndstart[0]) );
	_ASSERT( !ret );

	int keynum = 0;
	ret = m_anim->GetSndKeyframeNoRange( 0, m_anim->m_maxframe, 0, 0, &keynum );
	if( ret ){
		DbgOut( "SndFile : WriteSndAnim : ca GetSndKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( "\t$keynum %d\r\n", keynum );
	_ASSERT( !ret );

	if( keynum > 0 ){
		int* keynoarray;
		keynoarray = (int*)malloc( sizeof( int ) * keynum );
		if( !keynoarray ){
			DbgOut( "SndFile : WriteSndAnim : keynoarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( keynoarray, sizeof( int ) * keynum );

		int getnum = 0;
		ret = m_anim->GetSndKeyframeNoRange( 0, m_anim->m_maxframe, keynoarray, keynum, &getnum );
		if( ret || (getnum != keynum) ){
			DbgOut( "SndFile : WriteSndAnim : ca GetSndKeyframeNoRange 1 error !!!\n" );
			_ASSERT( 0 );
			free( keynoarray );
			return 1;
		}

		int keyno;
		for( keyno = 0; keyno < keynum; keyno++ ){
			int curframeno;
			curframeno = *( keynoarray + keyno );

			ret = Write2File( "\t%s", strsndkeystart );
			_ASSERT( !ret );

			CSndKey* sndkptr = 0;
			ret = m_anim->ExistSndKey( curframeno, &sndkptr );
			if( ret || !sndkptr ){
				DbgOut( "SndFile : WriteSndAnim : ca ExistSndKey error !!!\n" );
				_ASSERT( 0 );
				free( keynoarray );
				return 1;
			}

			ret = Write2File( "\t\t$frameno %d\r\n", sndkptr->m_frameno );
			_ASSERT( !ret );

			ret = Write2File( "\t\t$elemnum %d\r\n", sndkptr->m_sndenum );
			_ASSERT( !ret );

			int elemno;
			for( elemno = 0; elemno < sndkptr->m_sndenum; elemno++ ){
				SNDELEM* curelem = sndkptr->m_psnde + elemno;

				ret = Write2File( "\t\t%s", strsndelemstart );
				_ASSERT( !ret );
		
				ret = Write2File( "\t\t\t$sndname %s\r\n", curelem->sndname );
				_ASSERT( !ret );

				ret = Write2File( "\t\t%s", strsndelemend );
				_ASSERT( !ret );
			}

			ret = Write2File( "\t%s", strsndkeyend );
			_ASSERT( !ret );
		}
		free( keynoarray );
	}
	ret = Write2File( &(strsndend[0]) );
	_ASSERT( !ret );

	return 0;
}


int CSndFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "SndFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "SndFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "SndFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "SndFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_sndbuf.buf = newbuf;
	m_sndbuf.pos = 0;
	m_sndbuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, strendfile );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_sndbuf.bufleng = validleng;

	return 0;
}

int CSndFile::CheckFileVersion( int* verptr )
{

	char* headerptr;
	headerptr = strstr( m_sndbuf.buf, strheader );
	if( headerptr ){
		*verptr = 1001;
	}else{
		*verptr = 0;
	}

	return 0;
}

int CSndFile::ReadAnimInfo( CSndAnimHandler* srcsndah, CSoundBank* sbptr )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_sndbuf.buf, stranimstart );
	endptr = strstr( m_sndbuf.buf, stranimend );

	if( !startptr || !endptr ){
		DbgOut( "SndFile : ReadAnimInfo : section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_sndbuf.buf );
	if( (chkendpos >= (int)m_sndbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "SndFile : ReadAnimInfo : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SNDBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;

	char strname[256];
	ZeroMemory( strname, sizeof( char ) * 256 );
	ret = Read_Str( &infobuf, "$animname ", strname, 256 );
	if( ret ){
		DbgOut( "SndFile : ReadAnimInfo : Read_Str animname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	char strssname[32];
	ZeroMemory( strssname, sizeof( char ) * 32 );
	ret = Read_Str( &infobuf, "$sndsetname ", strssname, 32 );
	if( ret ){
		DbgOut( "SndFile : ReadAnimInfo : Read_Str sndsetname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_ss = 0;
	ret = sbptr->GetSoundSet( strssname, &m_ss );
	if( ret || !m_ss ){
		DbgOut( "SndFile : ReadAnimInfo : sb GetSoundSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int fps = 0;
	ret = Read_Int( &infobuf, "$fps ", &fps );
	if( ret || (fps <= 0) ){
		DbgOut( "SndFile : ReadAnimInfo : Read_Int fps error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int rloop = 0;
	ret = Read_Int( &infobuf, "$loop ", &rloop );
	if( (ret == 0) && (rloop != 0) ){
		rloop = 255;
	}else{
		rloop = 0;
	}

	m_anim = srcsndah->AddAnim( strname, fps, m_ss );
	if( !m_anim ){
		DbgOut( "SndFile : ReadAnimInfo : sndah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_anim->m_bgmloop = rloop;

	return 0;
}


int CSndFile::ReadSndAnim()
{
	int ret;
	char* startpat;
	char* endpat;
	startpat = &(strsndstart[0]);
	endpat = &(strsndend[0]);

	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( m_sndbuf.buf, startpat );
	endptr = strstr( m_sndbuf.buf, endpat );

	if( !startptr || !endptr ){
		DbgOut( "SndFile : ReadSndAnim : anim section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_sndbuf.buf );
	if( (chkendpos >= (int)m_sndbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "SndFile : ReadSndAnim : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SNDBUF animbuf;
	animbuf.buf = startptr;
	animbuf.bufleng = (int)( endptr - startptr );
	animbuf.pos = 0;
	animbuf.isend = 0;

	int keynum = 0;
	ret = Read_Int( &animbuf, "$keynum ", &keynum );
	if( ret || (keynum < 0) ){
		DbgOut( "SndFile : ReadSndAnim : Read_Int keynum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int keyno;
	for( keyno = 0; keyno < keynum; keyno++ ){
		ret = ReadSndKey( &animbuf );
		if( ret ){
			DbgOut( "SndFile : ReadSndAnim : ReadSndKey %d error !!!\n", keyno );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CSndFile::ReadSndKey( SNDBUF* animbuf )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( animbuf->buf + animbuf->pos, strsndkeystart );
	endptr = strstr( animbuf->buf + animbuf->pos, strsndkeyend );

	if( !startptr || !endptr ){
		DbgOut( "SndFile : ReadSndKey : key section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - animbuf->buf );
	if( (chkendpos >= (int)animbuf->bufleng) || (endptr < startptr) ){
		DbgOut( "SndFile : ReadSndKey : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SNDBUF keybuf;
	keybuf.buf = startptr;
	keybuf.bufleng = (int)( endptr - startptr );
	keybuf.pos = 0;
	keybuf.isend = 0;
	
	int frameno = 0;
	ret = Read_Int( &keybuf, "$frameno ", &frameno );
	if( ret || (frameno < 0) || (frameno > m_anim->m_maxframe) ){
		DbgOut( "SndFile : ReadSndKey : Read_Int frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int elemnum = 0;
	ret = Read_Int( &keybuf, "$elemnum ", &elemnum );
	if( ret || (elemnum <= 0) ){
		DbgOut( "SndFile : ReadSndKey : Read_Int elemnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int elemno;
	for( elemno = 0; elemno < elemnum; elemno++ ){
		SNDELEM snde;
		ZeroMemory( &snde, sizeof( SNDELEM ) );
		ret = ReadSndElem( &keybuf, &snde );
		if( ret ){
			DbgOut( "SndFile : ReadSndKey : ReadSndElem %d %d error !!!\n", elemnum, elemno );
			_ASSERT( 0 );
			return 1;
		}

		ret = m_anim->AddSndE( frameno, &snde );
		if( ret ){
			DbgOut( "SndFile : ReadSndKey : anim AddSndE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}		
	}

	int nextpos;
	nextpos = chkendpos + (int)strlen( strsndkeyend );
	animbuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return 0;
}

int CSndFile::ReadSndElem( SNDBUF* keybuf, SNDELEM* dstsnde )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( keybuf->buf + keybuf->pos, strsndelemstart );
	endptr = strstr( keybuf->buf + keybuf->pos, strsndelemend );

	if( !startptr || !endptr ){
		DbgOut( "SndFile : ReadSndElem : elem section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - keybuf->buf );
	if( (chkendpos >= (int)keybuf->bufleng) || (endptr < startptr) ){
		DbgOut( "SndFile : ReadSndElem : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SNDBUF elembuf;
	elembuf.buf = startptr;
	elembuf.bufleng = (int)( endptr - startptr );
	elembuf.pos = 0;
	elembuf.isend = 0;

	char sndname[256];
	ZeroMemory( sndname, sizeof( char ) * 256 );
	ret = Read_Str( &elembuf, "$sndname ", sndname, 256 );
	if( ret ){
		DbgOut( "SndFIle : ReadSndElem : Read_Str sndname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////
	ZeroMemory( dstsnde, sizeof( SNDELEM ) );
	strcpy_s( dstsnde->sndname, 256, sndname );
	strcpy_s( dstsnde->sndsetname, 32, m_anim->m_ssname );

/////////////
	int nextpos;
	nextpos = chkendpos + (int)strlen( strsndkeyend );
	keybuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return 0;
}


int CSndFile::DeleteAnim( CSndAnimHandler* srcsndah )
{
	int ret;
	if( m_anim ){
		ret = srcsndah->DestroyAnim( m_anim->m_motkind );
		_ASSERT( !ret );
		m_anim = 0;
	}
	return 0;
}

int CSndFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstint = atoi( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}
int CSndFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') && (*(srcchar + startpos) != '.') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) || (*(srcchar + endpos) == '.') )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstfloat = (float)atof( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}

int CSndFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) && 
		( ( *(srcchar + startpos) == ' ' ) || ( *(srcchar + startpos) == '\t' ) || ( *(srcchar + startpos) == '\"' ) ) 
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( ( *(srcchar + endpos) != ' ' ) && ( *(srcchar + endpos) != '\t' ) && (*(srcchar + endpos) != '\r') && (*(srcchar + endpos) != '\n') && (*(srcchar + endpos) != '\"') )
	){
		endpos++;
	}

	if( (endpos - startpos < dstleng) && (endpos - startpos > 0) ){
		strncpy_s( dstchar, dstleng, srcchar + startpos, endpos - startpos );
		*(dstchar + endpos - startpos) = 0;

	}else{
		_ASSERT( 0 );
	}


	return 0;
}



int CSndFile::Read_Int( SNDBUF* sndbuf, char* srcpat, int* dstint )
{
	int ret;
	char* findpat;
	findpat = strstr( sndbuf->buf + sndbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "SndFile : Read_Int : %s pattern not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}
	char* srcchar = findpat + (int)strlen( srcpat );
	int srcleng = 0;
	int findend = 0;
	while( findend == 0 ){
		if( (*(srcchar + srcleng) == '\r') || (*(srcchar + srcleng) == '\n') ){
			findend = 1;
			break;
		}
		if( (srcchar + srcleng) >= (sndbuf->buf + sndbuf->bufleng) ){
			findend = 1;
			_ASSERT( 0 );
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "SndFile : Read_Int : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "SndFile : Read_Int : %s GetInt error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndFile::Read_Float( SNDBUF* sndbuf, char* srcpat, float* dstfloat )
{
	int ret;
	char* findpat;
	findpat = strstr( sndbuf->buf + sndbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "SndFile : Read_Float : %s pattern not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}
	char* srcchar = findpat + (int)strlen( srcpat );
	int srcleng = 0;
	int findend = 0;
	while( findend == 0 ){
		if( (*(srcchar + srcleng) == '\r') || (*(srcchar + srcleng) == '\n') ){
			findend = 1;
			break;
		}
		if( (srcchar + srcleng) >= (sndbuf->buf + sndbuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "SndFile : Read_Float : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "SndFile : Read_Int : %s GetFloat error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndFile::Read_Str( SNDBUF* sndbuf, char* srcpat, char* dststr, int arrayleng )
{
	int ret;
	char* findpat;
	findpat = strstr( sndbuf->buf + sndbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "SndFile : Read_Str : %s pattern not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}
	char* srcchar = findpat + (int)strlen( srcpat );
	int srcleng = 0;
	int findend = 0;
	while( findend == 0 ){
		if( (*(srcchar + srcleng) == '\r') || (*(srcchar + srcleng) == '\n') ){
			findend = 1;
			break;
		}
		if( (srcchar + srcleng) >= (sndbuf->buf + sndbuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "SndFile : Read_Str : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "SndFile : Read_Str : %s GetName error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CSndFile::LoadSndFile( char* filename, CSndAnimHandler* srcsndah, CSoundBank* sbptr, CSndAnim** ppanim )
{
	int ret;

	*ppanim = 0;

	if( !srcsndah ){
		_ASSERT( 0 );
		return 1;
	}
	m_anim = 0;
	m_mode = SNDFILE_LOAD;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "SndFile : LoadSndFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	ret = SetBuffer();
	if( ret ){
		DbgOut( "SndFile : LoadSndFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CheckFileVersion( &m_loadversion );
	if( ret || (m_loadversion == 0) ){
		DbgOut( "SndFile : LoadSndFile : CheckFileVersion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadAnimInfo( srcsndah, sbptr );
	if( ret ){
		DbgOut( "SndFile : LoadSndFile : ReadAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadSndAnim();
	if( ret ){
		DbgOut( "SndFile : LoadSndFile : ReadSndAnim error !!!\n" );
		_ASSERT( 0 );
		DeleteAnim( srcsndah );
		return 1;
	}

	_ASSERT( m_anim );

	ret = m_anim->CheckAndDelInvalid( m_ss );
	if( ret ){
		DbgOut( "SndFile : LoadSndFile : anim CheckAndDelInvalid error !!!\n" );
		_ASSERT( 0 );
		DeleteAnim( srcsndah );
		return 1;
	}

	ret = m_anim->SetFrameData();
	if( ret ){
		DbgOut( "SndFile : LoadSndFile : anim SetFrameData error !!!\n" );
		_ASSERT( 0 );
		DeleteAnim( srcsndah );
		return 1;
	}


	*ppanim = m_anim;

	return 0;
}
