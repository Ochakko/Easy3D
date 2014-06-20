#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include <EQUFile.h>
#include <mqofile.h>

#include <TreeHandler2.h>
#include <TreeElem2.h>
#include <ShdHandler.h>
#include <ShdElem.h>
#include <MotHandler.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <motionpoint2.h>

#include <crtdbg.h>


static char equheader[512] = "<EQU>\r\n  <FileInfo>RokDeBone2 equ file ver1001 type00</FileInfo>\r\n";

enum {
	EQUFILE_WRITE,
	EQUFILE_LOAD,
	EQUFILE_MAX
};

/////////////////////////////

CEQUFile::CEQUFile()
{
	InitParams();
}
CEQUFile::~CEQUFile()
{
	DestroyObjs();
}

int CEQUFile::InitParams()
{
	m_mode = EQUFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_equbuf.buf = 0;
	m_equbuf.bufleng = 0;
	m_equbuf.pos = 0;
	m_equbuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * EQULINELEN );

	m_loadversion = 0;

	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;

	m_jointarray = 0;
	m_jointnum = 0;

	m_mult = 1.0f;
	m_animnum = 0;
	m_animdat = 0;

	m_motcookie = -1;

	return 0;
}

int CEQUFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == EQUFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_equbuf.buf ){
		free( m_equbuf.buf );
		m_equbuf.buf = 0;
	}
	m_equbuf.bufleng = 0;
	m_equbuf.pos = 0;
	m_equbuf.isend = 0;

	if( m_jointarray ){
		free( m_jointarray );
		m_jointarray = 0;
	}
	m_jointnum = 0;

	if( m_animdat ){
		int animcnt;
		for( animcnt = 0; animcnt < m_animnum; animcnt++ ){
			ANIMDAT* delanim = m_animdat + animcnt;
			if( delanim->badat ){
				int bacnt;
				for( bacnt = 0; bacnt < delanim->bonenum; bacnt++ ){
					BADAT* delba = delanim->badat + bacnt;
					if( delba->keydat ){
						free( delba->keydat );
						delba->keydat = 0;
					}
				}
				free( delanim->badat );
				delanim->badat = 0;
			}
		}
		free( m_animdat );
		m_animdat = 0;
	}
	m_animnum = 0;

	return 0;
}


int CEQUFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "EQUFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "EQUFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "EQUFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "EQUFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_equbuf.buf = newbuf;
	m_equbuf.pos = 0;
	m_equbuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, "</EQU>" );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_equbuf.bufleng = validleng;

	*( m_equbuf.buf + validleng ) = 0;

	return 0;
}

int CEQUFile::ReadFileInfo()
{
	int ret;
	char tmpheader[256];
	ZeroMemory( tmpheader, sizeof( char ) * 256 );
	ret = Read_Str( &m_equbuf, "<FileInfo>", "</FileInfo>", tmpheader, 256 );
	if( ret ){
		DbgOut( "EQUFile : ReadFileInfo : Read_Str tmpheader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int cmp;
	cmp = strcmp( tmpheader, "RokDeBone2 equ file ver1001 type00" );
	if( cmp != 0 ){
		DbgOut( "EQUFile : ReadFileInfo : FileHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CEQUFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	strncpy_s( tempchar, 256, srcchar + pos, srcleng );

	*dstint = atoi( tempchar );

	*stepnum = srcleng;

	return 0;
}
int CEQUFile::GetInt2( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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


int CEQUFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );
	strncpy_s( tempchar, 256, srcchar + pos, srcleng );
	*dstfloat = (float)atof( tempchar );

	*stepnum = srcleng;


	return 0;
}
int CEQUFile::GetFloat2( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CEQUFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
{

	strncpy_s( dstchar, dstleng, srcchar + pos, srcleng );
	*( dstchar + srcleng ) = 0;

	return 0;
}



int CEQUFile::Read_Int( EQUBUF* equbuf, char* startpat, char* endpat, int* dstint )
{
	int ret;
	char* startptr;
	startptr = strstr( equbuf->buf + equbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "EQUFile : Read_Int : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( equbuf->buf + equbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "EQUFile : Read_Int : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - equbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)equbuf->bufleng) ){
		DbgOut( "EQUFile : Read_Int : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}


	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "EQUFile : Read_Int : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Int : %s GetInt error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CEQUFile::Read_Float( EQUBUF* equbuf, char* startpat, char* endpat, float* dstfloat )
{
	int ret;
	char* startptr;
	startptr = strstr( equbuf->buf + equbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "EQUFile : Read_Float : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( equbuf->buf + equbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "EQUFile : Read_Float : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - equbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)equbuf->bufleng) ){
		DbgOut( "EQUFile : Read_Float : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "EQUFile : Read_Float : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Float : %s GetFloat error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CEQUFile::Read_Vec3( EQUBUF* equbuf, char* startpat, char* endpat, D3DXVECTOR3* dstvec )
{
	int ret;
	char* startptr;
	startptr = strstr( equbuf->buf + equbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "EQUFile : Read_Vec3 : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( equbuf->buf + equbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "EQUFile : Read_Vec3 : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - equbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)equbuf->bufleng) ){
		DbgOut( "EQUFile : Read_Vec3 : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "EQUFile : Read_Vec3 : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	float xval = 0.0f;
	float yval = 0.0f;
	float zval = 0.0f;

	int srcpos = 0;
	int stepnum = 0;
	ret = GetFloat2( &xval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Vec3 : %s GetFloat X error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

//_ASSERT( 0 );
	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat2( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Vec3 : %s GetFloat Y error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

//_ASSERT( 0 );

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat2( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Vec3 : %s GetFloat Z error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

//_ASSERT( 0 );
	dstvec->x = xval;
	dstvec->y = yval;
	dstvec->z = zval;

	return 0;
}
int CEQUFile::Read_Q( EQUBUF* equbuf, char* startpat, char* endpat, CQuaternion* dstq )
{
	int ret;
	char* startptr;
	startptr = strstr( equbuf->buf + equbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "EQUFile : Read_Q : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( equbuf->buf + equbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "EQUFile : Read_Q : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - equbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)equbuf->bufleng) ){
		DbgOut( "EQUFile : Read_Q : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "EQUFile : Read_Q : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	float xval = 0.0f;
	float yval = 0.0f;
	float zval = 0.0f;
	float wval = 1.0f;

	int srcpos = 0;
	int stepnum = 0;
	ret = GetFloat2( &xval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Q : %s GetFloat X error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat2( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Q : %s GetFloat Y error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat2( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Q : %s GetFloat Z error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat2( &wval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "EQUFile : Read_Q : %s GetFloat W error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	dstq->x = xval;
	dstq->y = yval;
	dstq->z = zval;
	dstq->w = wval;

	return 0;
}


int CEQUFile::Read_Str( EQUBUF* equbuf, char* startpat, char* endpat, char* dststr, int arrayleng )
{
	int ret;
	char* startptr;
	startptr = strstr( equbuf->buf + equbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "EQUFile : Read_Str : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( equbuf->buf + equbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "EQUFile : Read_Str : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - equbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)equbuf->bufleng) ){
		DbgOut( "EQUFile : Read_Str : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= arrayleng) ){
		DbgOut( "EQUFile : Read_Str : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "EQUFile : Read_Str : %s GetName error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CEQUFile::LoadEQUFile( char* filename )
{
	int ret;
	m_mode = EQUFILE_LOAD;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "EQUFile : LoadEQUFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	ret = SetBuffer();
	if( ret ){
		DbgOut( "EQUFile : LoadEQUFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadFileInfo();
	if( ret ){
		DbgOut( "EQUFile : LoadEQUFile : ReadFileInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Read_Int( &m_equbuf, "<AnimNum>", "</AnimNum>", &m_animnum );
	if( ret ){
		DbgOut( "EQUFile : LoadEQUFile : Read_Int animnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_animnum <= 0 ){
		return 0;
	}

	m_animdat = (ANIMDAT*)malloc( sizeof( ANIMDAT ) * m_animnum );
	if( !m_animdat ){
		DbgOut( "EQUFile : LoadEQUFile : animdat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_animdat, sizeof( ANIMDAT ) * m_animnum );

	int animcnt;
	for( animcnt = 0; animcnt < m_animnum; animcnt++ ){
		if( m_equbuf.pos >= m_equbuf.bufleng ){
			DbgOut( "EQUFile : LoadEQUFile : pos overflow error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		char* startptr = 0;
		char* endptr = 0;

		startptr = strstr( m_equbuf.buf + m_equbuf.pos, "<Animation>" );
		if( startptr ){
			startptr += (int)strlen( "<Animation>" );
		}
		endptr = strstr( m_equbuf.buf + m_equbuf.pos, "</Animation>" );
		if( endptr ){
			endptr += (int)strlen( "</Animation>" );
		}

		if( !startptr || !endptr ){
			DbgOut( "EQUFile : LoadEQUFile : Animation pattern not found skip !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int chkendpos;
		chkendpos = (int)( endptr - m_equbuf.buf );
		if( (chkendpos >= (int)m_equbuf.bufleng) || (endptr < startptr) ){
			DbgOut( "EQUFile : LoadEQUFile : Animation endmark error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		
		EQUBUF animbuf;
		ZeroMemory( &animbuf, sizeof( EQUBUF ) );
		animbuf.buf = startptr;
		animbuf.pos = 0;
		animbuf.bufleng = (int)( endptr - startptr );
		animbuf.isend = 0;

		ret = ReadAnimation( &animbuf, animcnt );
		if( ret ){
			DbgOut( "EQUFile : LoadEQUFile : ReadAnimation error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_equbuf.pos = chkendpos;
	}

	return 0;
}

int CEQUFile::ReadAnimation( EQUBUF* animbuf, int animno )
{
	int ret;
	ANIMDAT* curanimdat = m_animdat + animno;
	_ASSERT( curanimdat );

	ret = Read_Str( animbuf, "<AnimName>", "</AnimName>", curanimdat->animname, 256 );
	if( ret ){
		DbgOut( "EQUFile : ReadAnimation : Read_Str AnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Read_Int( animbuf, "<FrameLeng>", "</FrameLeng>", &(curanimdat->frameleng) );
	if( ret || (curanimdat->bonenum < 0) ){
		DbgOut( "EQUFile : ReadAnimation : Read_Int BoneAnimNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Read_Int( animbuf, "<BoneAnimNum>", "</BoneAnimNum>", &(curanimdat->bonenum) );
	if( ret || (curanimdat->bonenum < 0) ){
		DbgOut( "EQUFile : ReadAnimation : Read_Int BoneAnimNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curanimdat->bonenum == 0 ){
		return 0;
	}

	BADAT* badat = (BADAT*)malloc( sizeof( BADAT ) * curanimdat->bonenum );
	if( !badat ){
		DbgOut( "EQUFile : ReadAnimation : badat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( badat, sizeof( BADAT ) * curanimdat->bonenum );
	curanimdat->badat = badat;

	int bacnt;
	for( bacnt = 0; bacnt < curanimdat->bonenum; bacnt++ ){
		if( animbuf->pos >= animbuf->bufleng ){
			DbgOut( "EQUFile : ReadAnimation : pos overflow error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		char* startptr = 0;
		char* endptr = 0;

		startptr = strstr( animbuf->buf + animbuf->pos, "<BoneAnim>" );
		if( startptr ){
			startptr += (int)strlen( "<BoneAnim>" );
		}
		endptr = strstr( animbuf->buf + animbuf->pos, "</BoneAnim>" );
		if( endptr ){
			endptr += (int)strlen( "</BoneAnim>" );
		}
		if( !startptr || !endptr ){
			DbgOut( "EQUFile : ReadAnimation : section pattern not found error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int chkendpos;
		chkendpos = (int)( endptr - animbuf->buf );
		if( (chkendpos >= (int)animbuf->bufleng) || (endptr < startptr) ){
			DbgOut( "EQUFile : ReadAnimation : endmark error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		EQUBUF babuf;
		ZeroMemory( &babuf, sizeof( EQUBUF ) );
		babuf.buf = startptr;
		babuf.pos = 0;
		babuf.bufleng = (int)( endptr - startptr );
		babuf.isend = 0;

		ret = ReadBoneAnim( &babuf, animno, bacnt );
		if( ret ){
			DbgOut( "EQUFile : LoadAnimation : ReadBoneAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		animbuf->pos = chkendpos;
	}

	return 0;
}

int CEQUFile::ReadBoneAnim( EQUBUF* bonebuf, int animno, int bano )
{
	int ret;
	_ASSERT( m_animdat );
	ANIMDAT* curanimdat = m_animdat + animno;
	_ASSERT( curanimdat->badat );
	BADAT* curbadat = curanimdat->badat + bano;

	ret = Read_Str( bonebuf, "<BoneName>", "</BoneName>", curbadat->bonename, 256 );
	if( ret ){
		DbgOut( "EQUFile : ReadBoneAnim : Read_Str BoneName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Read_Int( bonebuf, "<KeyNum>", "</KeyNum>", &(curbadat->keynum) );
	if( ret || (curbadat->keynum < 0) ){
		DbgOut( "EQUFile : ReadBoneAnim : Read_Int KeyNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curbadat->keynum == 0 ){
		return 0;
	}

	KEYDAT* keydat = (KEYDAT*)malloc( sizeof( KEYDAT ) * curbadat->keynum );
	if( !keydat ){
		DbgOut( "EQUFile : ReadBoneAnim : keydat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( keydat, sizeof( KEYDAT ) * curbadat->keynum );
	curbadat->keydat = keydat;

	int keycnt;
	for( keycnt = 0; keycnt < curbadat->keynum; keycnt++ ){
		if( bonebuf->pos >= bonebuf->bufleng ){
			DbgOut( "EQUFile : ReadBoneAnim : pos overflow error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		char* startptr = 0;
		char* endptr = 0;

		startptr = strstr( bonebuf->buf + bonebuf->pos, "<AnimKey>" );
		if( startptr ){
			startptr += (int)strlen( "<AnimKey>" );
		}
		endptr = strstr( bonebuf->buf + bonebuf->pos, "</AnimKey>" );
		if( endptr ){
			endptr += (int)strlen( "</AnimKey>" );
		}
		if( !startptr || !endptr ){
			DbgOut( "EQUFile : ReadBoneAnim : section pattern not found error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int chkendpos;
		chkendpos = (int)( endptr - bonebuf->buf );
		if( (chkendpos >= (int)bonebuf->bufleng) || (endptr < startptr) ){
			DbgOut( "EQUFile : ReadBoneAnim : endmark error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		EQUBUF keybuf;
		ZeroMemory( &keybuf, sizeof( EQUBUF ) );
		keybuf.buf = startptr;
		keybuf.pos = 0;
		keybuf.bufleng = (int)( endptr - startptr );
		keybuf.isend = 0;

		ret = ReadAnimKey( &keybuf, animno, bano, keycnt );
		if( ret ){
			DbgOut( "EQUFile : LoadBoneAnim : ReadAnimKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		bonebuf->pos = chkendpos;
	}



	return 0;
}
int CEQUFile::ReadAnimKey( EQUBUF* keybuf, int animno, int bano, int keyno )
{
	int ret;
	_ASSERT( m_animdat );
	ANIMDAT* curanimdat = m_animdat + animno;
	_ASSERT( curanimdat->badat );
	BADAT* curbadat = curanimdat->badat + bano;
	_ASSERT( curbadat->keydat );
	KEYDAT* curkeydat = curbadat->keydat + keyno;

 	ret = Read_Int( keybuf, "<FrameNo>", "</FrameNo>", &(curkeydat->frameno) );
	if( ret ){
		DbgOut( "EQUFile : ReadAnimKey : FrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Read_Q( keybuf, "<Q>", "</Q>", &(curkeydat->q) );
	if( ret ){
		DbgOut( "EQUFile : ReadAnimKey : Q error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Read_Vec3( keybuf, "<Scale>", "</Scale>", &(curkeydat->scale) );
	if( ret ){
		DbgOut( "EQUFile : ReadAnimKey : Scale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Read_Vec3( keybuf, "<Tra>", "</Tra>", &(curkeydat->tra) );
	if( ret ){
		DbgOut( "EQUFile : ReadAnimKey : Scale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	DbgOut( "checkkey!!! : animno %d, bano %d, keyno %d\r\n", animno, bano, keyno );
//	DbgOut( "checkkey!!! : frameno %d\r\n", curkeydat->frameno );
//	DbgOut( "checkkey!!! : q %f, %f, %f, %f\r\n", curkeydat->q.x, curkeydat->q.y, curkeydat->q.z, curkeydat->q.w );
//	DbgOut( "checkkey!!! : scale %f, %f, %f\r\n", curkeydat->scale.x, curkeydat->scale.y, curkeydat->scale.z );
//	DbgOut( "checkkey!!! : tra %f, %f, %f\r\n", curkeydat->tra.x, curkeydat->tra.y, curkeydat->tra.z );

	return 0;
}


int CEQUFile::Write2File( int tabnum, char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar0[EQULINELEN];
	char outchar1[EQULINELEN];
	char tabchar[EQULINELEN];
	ZeroMemory( outchar0, EQULINELEN );
	ZeroMemory( outchar1, EQULINELEN );
	ZeroMemory( tabchar, EQULINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar0, EQULINELEN, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 ){
		_ASSERT( 0 );
		return 1;
	}

	if( tabnum >= 1 ){
		strcpy_s( tabchar, EQULINELEN, "  " );
	}
	int tno;
	for( tno = 1; tno < tabnum; tno++ ){
		strcat_s( tabchar, EQULINELEN, "  " );
	}
	sprintf_s( outchar1, EQULINELEN, "%s%s", tabchar, outchar0 );

	wleng = (unsigned long)strlen( outchar1 );
	WriteFile( m_hfile, outchar1, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CEQUFile::WriteEQUFile( char* filename, int* motidarray, int motnum, 
	CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
{
	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	int ret;
	ret = MakeJointArray();
	if( ret ){
		DbgOut( "EQUFile : WriteEQUFile : MakeJointArray error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_jointnum <= 0 ){
		_ASSERT( 0 );
		return 0;
	}

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "EQUFile : WriteEQUFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_mode = EQUFILE_WRITE;	

	ret = Write2File( 0, equheader );
	_ASSERT( !ret );
///////
	ret = Write2File( 1, "<AnimNum>%d</AnimNum>\r\n\r\n", motnum );
	_ASSERT( !ret );

	int motcnt;
	for( motcnt = 0; motcnt < motnum; motcnt++ ){
		int motid = *( motidarray + motcnt );
		if( (motid < 0) || (motid >= m_lpmh->m_kindnum) ){
			DbgOut( "EQUFile : WriteEQUFile : motid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteAnimation( motid );
		if( ret ){
			DbgOut( "EQUFile : WriteEQUFile : WriteAnimation error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
///////

	ret = Write2File( 0, "</EQU>\r\n" );
	_ASSERT( !ret );

	return 0;
}

int CEQUFile::MakeJointArray()
{

	int seri;
	CMotionCtrl* mcptr;

	if( m_jointarray ){
		free( m_jointarray );
		m_jointarray = 0;
	}
	m_jointnum = 0;

	for( seri = 0; seri < m_lpmh->s2mot_leng; seri++ ){
		mcptr = (*m_lpmh)( seri );
		_ASSERT( mcptr );
		if( mcptr->IsJoint() ){
			m_jointnum++;
		}
	}

	if( m_jointnum <= 0 ){
		return 0;
	}

	m_jointarray = (int*)malloc( sizeof( int ) * m_jointnum );
	if( !m_jointarray ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_jointarray, sizeof( int ) * m_jointnum );


	int setno = 0;
	for( seri = 0; seri < m_lpmh->s2mot_leng; seri++ ){
		mcptr = (*m_lpmh)( seri );
		_ASSERT( mcptr );
		if( mcptr->IsJoint() ){
			*( m_jointarray + setno ) = seri;
			setno++;
		}
	}

	if( setno != m_jointnum ){
		DbgOut( "EQUFile : MakeJointArray : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CEQUFile::WriteAnimation( int motid )
{
	int ret;
 
	CMotionCtrl* mcptr = (*m_lpmh)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}
	char* szName;
	szName = *(miptr->motname + motid );

	int frameleng;
	frameleng = *(miptr->motnum + motid);

	ret = Write2File( 1, "<Animation>\r\n" );
	_ASSERT( !ret );

	ret = Write2File( 2, "<AnimName>%s</AnimName>\r\n", szName );
	_ASSERT( !ret );

	ret = Write2File( 2, "<FrameLeng>%d</FrameLeng>\r\n", frameleng );
	_ASSERT( !ret );

	ret = Write2File( 2, "<BoneAnimNum>%d</BoneAnimNum>\r\n\r\n", m_jointnum );
	_ASSERT( !ret );

	CMotionCtrl* jointmc;
	int jcnt;
	for( jcnt = 0; jcnt < m_jointnum; jcnt++ ){
		int jseri = *( m_jointarray + jcnt );
		jointmc = (*m_lpmh)( jseri );
		_ASSERT( jointmc );

		ret = WriteBoneAnim( motid, jointmc );
		if( ret ){
			DbgOut( "EQUFile : WriteAnimation : WriteBoneAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = Write2File( 1, "</Animation>\r\n\r\n" );
	_ASSERT( !ret );

	return 0;
}
int CEQUFile::WriteBoneAnim( int motid, CMotionCtrl* mcptr )
{
	int ret;
   
	ret = Write2File( 2, "<BoneAnim>\r\n" );
	_ASSERT( !ret );

	CTreeElem2* jte;
	jte = (*m_lpth)( mcptr->serialno );
	_ASSERT( jte );
	ret = Write2File( 3, "<BoneName>%s</BoneName>\r\n", jte->name );
	_ASSERT( !ret );

	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		DbgOut( "EQUFile : WriteBoneAnim : miptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionPoint2* mphead = miptr->GetMotionPoint( motid );

	int mpnum = 0;
	CMotionPoint2* curmp = mphead;
	while( curmp ){
		mpnum++;
		curmp = curmp->next;
	}

	ret = Write2File( 3, "<KeyNum>%d</KeyNum>\r\n", mpnum );
	_ASSERT( !ret );

	int wcnt = 0;
	curmp = mphead;
	while( curmp ){
		ret = WriteAnimKey( curmp );
		if( ret ){
			DbgOut( "EQUFile : WriteBoneAnim : WriteAnimKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		wcnt++;
		curmp = curmp->next;
	}

	if( wcnt != mpnum ){
		DbgOut( "EQUFile : WriteBoneAnim : wcnt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( 2, "</BoneAnim>\r\n" );
	_ASSERT( !ret );


	return 0;
}
int CEQUFile::WriteAnimKey( CMotionPoint2* mpptr )
{        
	int ret;
	ret = Write2File( 3, "<AnimKey>\r\n" );
	_ASSERT( !ret );

	ret = Write2File( 4, "<FrameNo>%d</FrameNo>\r\n", mpptr->m_frameno );
	_ASSERT( !ret );

	ret = Write2File( 4, "<Q>%f, %f, %f, %f</Q>\r\n", mpptr->m_q.x, mpptr->m_q.y, mpptr->m_q.z, mpptr->m_q.w );
	_ASSERT( !ret );

	ret = Write2File( 4, "<Scale>%f, %f, %f</Scale>\r\n", mpptr->m_scalex, mpptr->m_scaley, mpptr->m_scalez );
	_ASSERT( !ret );

	ret = Write2File( 4, "<Tra>%f, %f, %f</Tra>\r\n", mpptr->m_mvx, mpptr->m_mvy, -mpptr->m_mvz );
	_ASSERT( !ret );

	ret = Write2File( 3, "</AnimKey>\r\n" );
	_ASSERT( !ret );

	return 0;
}

int CEQUFile::AddEQUMotion( int animno, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, LONG* motidptr, float mvmult )
{
	*motidptr = -1;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;
	m_mult = mvmult;

	int ret;
	if( (animno < 0) || (animno >= m_animnum) ){
		DbgOut( "EQUFile : AddEQUMotion animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !m_animdat ){
		_ASSERT( 0 );
		return 1;
	}
	ANIMDAT* curanimdat = m_animdat + animno;


	m_motcookie = m_lpmh->AddMotion( curanimdat->animname, MOTION_CLAMP, curanimdat->frameleng, INTERPOLATION_SLERP, 0 );
	if( m_motcookie < 0 ){
		DbgOut( "EQUFile : AddEQUMotion : mhandler->AddMotion error !!!\n" );
		_ASSERT( 0 );
		*motidptr = -1;
		return 1;
	}
	*motidptr = m_motcookie;

	int bonecnt;
	for( bonecnt = 0; bonecnt < curanimdat->bonenum; bonecnt++ ){
		if( !curanimdat->badat ){
			_ASSERT( 0 );
			return 1;
		}
		BADAT* curbadat = curanimdat->badat + bonecnt;

		int boneseri = -1;
		ret = m_lpth->GetBoneNoByName( curbadat->bonename, &boneseri, m_lpsh, 0 );
		if( ret ){
			DbgOut( "EQUFile : AddEQUMotion : th GetBoneNoByName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( boneseri > 0 ){
			CMotionCtrl* mcptr = (*m_lpmh)( boneseri );
			_ASSERT( mcptr );

			int keycnt;
			for( keycnt = 0; keycnt < curbadat->keynum; keycnt++ ){
				if( !curbadat->keydat ){
					_ASSERT( 0 );
					return 1;
				}
				KEYDAT* curkeydat = curbadat->keydat + keycnt;
				if( (curkeydat->frameno >= 0) && (curkeydat->frameno < curanimdat->frameleng) ){
					CMotionPoint2 srcmp;
					srcmp.m_frameno = curkeydat->frameno;
	
					curkeydat->q.inv( &(srcmp.m_q) );

					//srcmp.m_q.x = curkeydat->q.x;
					//srcmp.m_q.y = curkeydat->q.y;
					//srcmp.m_q.z = curkeydat->q.z;
					//srcmp.m_q.w = curkeydat->q.w;
					
					srcmp.m_mvx = mvmult * curkeydat->tra.x;
					srcmp.m_mvy = mvmult * curkeydat->tra.y;
					srcmp.m_mvz = mvmult * -curkeydat->tra.z;

					srcmp.m_scalex = curkeydat->scale.x;
					srcmp.m_scaley = curkeydat->scale.y;
					srcmp.m_scalez = curkeydat->scale.z;

					CMotionPoint2* retmp;
					retmp = mcptr->AddMotionPoint( m_motcookie, &srcmp, INTERPOLATION_SLERP );
					if( !retmp ){
						DbgOut( "EQUFile : AddEQUMotion : mc AddMotionPoint error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}
	}

	return 0;
}
