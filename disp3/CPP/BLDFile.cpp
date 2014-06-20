#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include <BldFile.h>

#include <crtdbg.h>

static char bldheader[512] = 
{
"<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\r\n<BLD>\r\n  <FileInfo>\r\n    <kind>RokDeBone2BatchLoad</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"
};
//static char bldheader2[512] = 
//{
//"<?xml version=\"1.0\"?>\r\n<BLD>\r\n  <FileInfo>\r\n    <kind>RokDeBone2BatchLoad</kind>\r\n    <version>1001</version>\r\n    <type>0</type>\r\n  </FileInfo>\r\n"
//};


enum {
	BLDFILE_WRITE,
	BLDFILE_LOAD,
	BLDFILE_MAX
};

CBLDFile::CBLDFile()
{
	InitParams();
}
CBLDFile::~CBLDFile()
{
	DestroyObjs();
}

int CBLDFile::InitParams()
{
	m_mode = BLDFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_bldbuf.buf = 0;
	m_bldbuf.bufleng = 0;
	m_bldbuf.pos = 0;
	m_bldbuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * BLDLINELEN );

	m_loadversion = 0;

	InitBLD( &m_ssbld );
	InitBLD( &m_sabld );
	InitBLD( &m_gbld );
	InitBLD( &m_cambld );
	InitBLD( &m_sigbld );
	InitBLD( &m_moabld );
	InitBLD( &m_gpabld );
	InitBLD( &m_moebld );

	ZeroMemory( m_dir, sizeof( char ) * MAX_PATH );

	return 0;
}
int CBLDFile::InitBLD( BLD* dstbld )
{
	ZeroMemory( dstbld, sizeof( BLD ) );
	dstbld->mult = 1.0f;
	return 0;
}

int CBLDFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == BLDFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_bldbuf.buf ){
		free( m_bldbuf.buf );
		m_bldbuf.buf = 0;
	}
	m_bldbuf.bufleng = 0;
	m_bldbuf.pos = 0;
	m_bldbuf.isend = 0;

	return 0;
}

int CBLDFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "BLDFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "BLDFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "BLDFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "BLDFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_bldbuf.buf = newbuf;
	m_bldbuf.pos = 0;
	m_bldbuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, "</BLD>" );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_bldbuf.bufleng = validleng;

	return 0;
}

int CBLDFile::ReadFileInfo()
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_bldbuf.buf, "<FileInfo>" );
	endptr = strstr( m_bldbuf.buf, "</FileInfo>" );

	if( !startptr || !endptr ){
		DbgOut( "BLDFile : ReadFileInfo : section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_bldbuf.buf );
	if( (chkendpos >= (int)m_bldbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "BLDFile : ReadFileInfo : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	BLDBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;

	char kindname[256];
	ZeroMemory( kindname, sizeof( char ) * 256 );
	ret = Read_Str( &infobuf, "<kind>", "</kind>", kindname, 256 );
	if( ret ){
		DbgOut( "BLDFile : ReadFileInfo : Read_Str kindname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int cmp;
	cmp = strcmp( kindname, "RokDeBone2BatchLoad" );
	if( cmp != 0 ){
		DbgOut( "BLDFile : ReadFileInfo : FileInfo kind error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int version = 0;
	ret = Read_Int( &infobuf, "<version>", "</version>", &version );
	if( ret || (version != 1001) ){
		DbgOut( "BLDFile : ReadFileInfo : Read_Int version error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int type = 0;
	ret = Read_Int( &infobuf, "<type>", "</type>", &type );
	if( ret || (type != 0) ){
		DbgOut( "BLDFile : ReadFileInfo : Read_Int type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CBLDFile::ReadSound()
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_bldbuf.buf, "<Sound>" );
	endptr = strstr( m_bldbuf.buf, "</Sound>" );

	if( !startptr || !endptr ){
		DbgOut( "BLDFile : ReadSound : section pattern not found skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_bldbuf.buf );
	if( (chkendpos >= (int)m_bldbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "BLDFile : ReadSound : endmark skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	BLDBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;


	char tempchar[MAX_PATH];
	ZeroMemory( tempchar, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<SoundSet>", "</SoundSet>", tempchar, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadSound : Read_Str ssname skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	sprintf_s( m_ssbld.name, MAX_PATH, "%s\\%s", m_dir, tempchar );
	m_ssbld.flag = 1;


	char tempchar2[MAX_PATH];
	ZeroMemory( tempchar2, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<SoundAnim>", "</SoundAnim>", tempchar2, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadSound : Read_Str saname skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	sprintf_s( m_sabld.name, MAX_PATH, "%s\\%s", m_dir, tempchar2 );
	m_sabld.flag = 1;


	return 0;
}
int CBLDFile::ReadGround()
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_bldbuf.buf, "<Ground>" );
	endptr = strstr( m_bldbuf.buf, "</Ground>" );

	if( !startptr || !endptr ){
		DbgOut( "BLDFile : ReadGround : section pattern not found skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_bldbuf.buf );
	if( (chkendpos >= (int)m_bldbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "BLDFile : ReadGround : endmark skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	BLDBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;


	char tempchar[MAX_PATH];
	ZeroMemory( tempchar, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<file>", "</file>", tempchar, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadGround : Read_Str filename skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	sprintf_s( m_gbld.name, MAX_PATH, "%s\\%s", m_dir, tempchar );
	m_gbld.flag = 1;


	float mult = 1.0f;
	ret = Read_Float( &infobuf, "<mult>", "</mult>", &mult );
	if( ret || (mult <= 0.0f) ){
		DbgOut( "BLDFile : ReadGround : Read_Float mult skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	m_gbld.mult = mult;

	return 0;
}
int CBLDFile::ReadCamera()
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_bldbuf.buf, "<Camera>" );
	endptr = strstr( m_bldbuf.buf, "</Camera>" );

	if( !startptr || !endptr ){
		DbgOut( "BLDFile : ReadCamera : section pattern not found skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_bldbuf.buf );
	if( (chkendpos >= (int)m_bldbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "BLDFile : ReadCamera : endmark skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	BLDBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;


	char tempchar[MAX_PATH];
	ZeroMemory( tempchar, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<file>", "</file>", tempchar, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadCamera : Read_Str filename skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	sprintf_s( m_cambld.name, MAX_PATH, "%s\\%s", m_dir, tempchar );
	m_cambld.flag = 1;


	char tempchar2[256];
	ZeroMemory( tempchar2, sizeof( char ) * 256 );
	ret = Read_Str( &infobuf, "<mode>", "</mode>", tempchar2, 256 );
	if( ret ){
		DbgOut( "BLDFile : ReadCamera : Read_Str mode skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int cmpkae, cmpdef;
	cmpkae = strcmp( tempchar2, "KAE" );
	cmpdef = strcmp( tempchar2, "DEFAULT" );
	if( cmpkae == 0 ){
		m_cambld.mode = 1;
	}else{
		m_cambld.mode = 0;
	}

	return 0;
}
int CBLDFile::ReadCharacter()
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_bldbuf.buf, "<Character>" );
	endptr = strstr( m_bldbuf.buf, "</Character>" );

	if( !startptr || !endptr ){
		DbgOut( "BLDFile : ReadCharacter : section pattern not found skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_bldbuf.buf );
	if( (chkendpos >= (int)m_bldbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "BLDFile : ReadCharacter : endmark skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	BLDBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;


	char tempchar[MAX_PATH];
	ZeroMemory( tempchar, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<shape>", "</shape>", tempchar, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadCharacter : Read_Str shape error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	sprintf_s( m_sigbld.name, MAX_PATH, "%s\\%s", m_dir, tempchar );
	m_sigbld.flag = 1;

	float mult = 1.0f;
	ret = Read_Float( &infobuf, "<shapemult>", "</shapemult>", &mult );
	if( ret || (mult <= 0.0f) ){
		DbgOut( "BLDFile : ReadCharacter : Read_Float shapemult skip !!!\n" );
	}else{
		m_sigbld.mult = mult;
	}


	char tempchar2[MAX_PATH];
	ZeroMemory( tempchar2, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<moa>", "</moa>", tempchar2, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadCharacter : Read_Str moa skip !!!\n" );
	}else{
		sprintf_s( m_moabld.name, MAX_PATH, "%s\\%s", m_dir, tempchar2 );
		m_moabld.flag = 1;
	}


	char tempchar3[MAX_PATH];
	ZeroMemory( tempchar3, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<gpa>", "</gpa>", tempchar3, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadCharacter : Read_Str gpa skip !!!\n" );
	}else{
		sprintf_s( m_gpabld.name, MAX_PATH, "%s\\%s", m_dir, tempchar3 );
		m_gpabld.flag = 1;
	}


	char tempchar4[MAX_PATH];
	ZeroMemory( tempchar4, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<moe>", "</moe>", tempchar4, MAX_PATH );
	if( ret ){
		DbgOut( "BLDFile : ReadCharacter : Read_Str moe skip !!!\n" );
	}else{
		sprintf_s( m_moebld.name, MAX_PATH, "%s\\%s", m_dir, tempchar4 );
		m_moebld.flag = 1;
	}

	return 0;
}

int CBLDFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	strncpy_s( tempchar, 256, srcchar + pos, srcleng );

	*dstint = atoi( tempchar );

	*stepnum = srcleng;

	return 0;
}
int CBLDFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	strncpy_s( tempchar, 256, srcchar + pos, srcleng );

	*dstfloat = (float)atof( tempchar );

	*stepnum = srcleng;


	return 0;
}

int CBLDFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
{

	strncpy_s( dstchar, dstleng, srcchar + pos, srcleng );
	*( dstchar + srcleng ) = 0;

	return 0;
}



int CBLDFile::Read_Int( BLDBUF* bldbuf, char* startpat, char* endpat, int* dstint )
{
	int ret;
	char* startptr;
	startptr = strstr( bldbuf->buf + bldbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "BLDFile : Read_Int : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( bldbuf->buf + bldbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "BLDFile : Read_Int : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - bldbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)bldbuf->bufleng) ){
		DbgOut( "BLDFile : Read_Int : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}


	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "BLDFile : Read_Int : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "BLDFile : Read_Int : %s GetInt error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CBLDFile::Read_Float( BLDBUF* bldbuf, char* startpat, char* endpat, float* dstfloat )
{
	int ret;
	char* startptr;
	startptr = strstr( bldbuf->buf + bldbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "BLDFile : Read_Float : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( bldbuf->buf + bldbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "BLDFile : Read_Float : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - bldbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)bldbuf->bufleng) ){
		DbgOut( "BLDFile : Read_Float : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "BLDFile : Read_Float : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "BLDFile : Read_Float : %s GetFloat error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CBLDFile::Read_Str( BLDBUF* bldbuf, char* startpat, char* endpat, char* dststr, int arrayleng )
{
	int ret;
	char* startptr;
	startptr = strstr( bldbuf->buf + bldbuf->pos, startpat );
	if( !startptr ){
		DbgOut( "BLDFile : Read_Str : %s pattern not found error !!!\n", startpat );
		//_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( bldbuf->buf + bldbuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "BLDFile : Read_Str : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - bldbuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)bldbuf->bufleng) ){
		DbgOut( "BLDFile : Read_Str : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= arrayleng) ){
		DbgOut( "BLDFile : Read_Str : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "BLDFile : Read_Str : %s GetName error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CBLDFile::LoadBLDFile( char* filename )
{
	int ret;

	m_mode = BLDFILE_LOAD;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BLDFile : LoadBLDFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	int ch = '\\';
	char* lasten;
	lasten = strrchr( filename, ch );
	if( !lasten ){
		DbgOut( "BLDFile : LoadBLDFile : lasten error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int dirleng;
	dirleng = (int)( lasten - filename );
	if( (dirleng < 0) || (dirleng >= MAX_PATH) ){
		DbgOut( "BLDFile : LoadBLDFile : dirleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strncpy_s( m_dir, MAX_PATH, filename, dirleng );
	m_dir[dirleng] = 0;



	ret = SetBuffer();
	if( ret ){
		DbgOut( "BLDFile : LoadBLDFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadFileInfo();
	if( ret ){
		DbgOut( "BLDFile : LoadBLDFile : ReadFileInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = ReadSound();
	if( ret ){
		DbgOut( "BLDFile : LoadBLDFile : ReadSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = ReadGround();
	if( ret ){
		DbgOut( "BLDFile : LoadBLDFile : ReadGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = ReadCamera();
	if( ret ){
		DbgOut( "BLDFile : LoadBLDFile : ReadCamera error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = ReadCharacter();
	if( ret ){
		DbgOut( "BLDFile : LoadBLDFile : ReadCharacter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CBLDFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[BLDLINELEN];
			
	ZeroMemory( outchar, BLDLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, BLDLINELEN, lpFormat, Marker );
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

int CBLDFile::SetBLD( BLD* sigbld, BLD* gbld, BLD* moabld, BLD* gpabld, BLD* moebld, BLD* cambld, BLD* ssbld, BLD* sabld )
{
	MoveMemory( &m_sigbld, sigbld, sizeof( BLD ) );
	MoveMemory( &m_gbld, gbld, sizeof( BLD ) );
	MoveMemory( &m_moabld, moabld, sizeof( BLD ) );
	MoveMemory( &m_gpabld, gpabld, sizeof( BLD ) );
	MoveMemory( &m_moebld, moebld, sizeof( BLD ) );
	MoveMemory( &m_cambld, cambld, sizeof( BLD ) );
	MoveMemory( &m_ssbld, ssbld, sizeof( BLD ) );
	MoveMemory( &m_sabld, sabld, sizeof( BLD ) );

	return 0;
}
int CBLDFile::WriteBLDFile( char* filename )
{
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "BLDFile : WriteBLDFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char tempchar[1024];
	m_mode = BLDFILE_WRITE;	

	int ret;
	ret = Write2File( bldheader );
	_ASSERT( !ret );

/////
	if( m_ssbld.flag ){
		ret = Write2File( "  <Sound>\r\n" );
		_ASSERT( !ret );

		if( m_ssbld.flag ){
			sprintf_s( tempchar, 1024, "    <SoundSet>%s</SoundSet>\r\n", m_ssbld.name );
		}else{
			strcpy_s( tempchar, 1024, "    <SoundSet></SoundSet>\r\n" );
		}
		ret = Write2File( tempchar );
		_ASSERT( !ret );

		if( m_sabld.flag ){
			sprintf_s( tempchar, 1024, "    <SoundAnim>%s</SoundAnim>\r\n", m_sabld.name );
		}else{
			strcpy_s( tempchar, 1024, "    <SoundAnim></SoundAnim>\r\n" );
		}
		ret = Write2File( tempchar );
		_ASSERT( !ret );

		ret = Write2File( "  </Sound>\r\n" );
		_ASSERT( !ret );
	}
//////
	if( m_gbld.flag ){
		ret = Write2File( "  <Ground>\r\n" );
		_ASSERT( !ret );

		ret = Write2File( "    <file>%s</file>\r\n    <mult>%f</mult>\r\n", m_gbld.name, m_gbld.mult );
		_ASSERT( !ret );

		ret = Write2File( "  </Ground>\r\n" );
		_ASSERT( !ret );
	}
//////
	if( m_cambld.flag ){
		ret = Write2File( "  <Camera>\r\n" );
		_ASSERT( !ret );

		char strmode[20];
		if( m_cambld.mode == 0 ){
			strcpy_s( strmode, 20, "DEFAULT" );
		}else{
			strcpy_s( strmode, 20, "KAE" );
		}

		ret = Write2File( "    <mode>%s</mode>\r\n    <file>%s</file>\r\n", strmode, m_cambld.name );
		_ASSERT( !ret );

		ret = Write2File( "  </Camera>\r\n" );
		_ASSERT( !ret );
	}
//////
	ret = Write2File( "  <Character>\r\n" );
	_ASSERT( !ret );

	ret = Write2File( "    <shape>%s</shape>\r\n    <shapemult>%f</shapemult>\r\n", m_sigbld.name, m_sigbld.mult );
	_ASSERT( !ret );

	ret = Write2File( "    <moa>%s</moa>\r\n", m_moabld.name );
	_ASSERT( !ret );
	
	if( m_gpabld.flag ){
		ret = Write2File( "    <gpa>%s</gpa>\r\n", m_gpabld.name );
		_ASSERT( !ret );
	}

	if( m_moebld.flag ){
		ret = Write2File( "    <moe>%s</moe>\r\n", m_moebld.name );
		_ASSERT( !ret );
	}

	ret = Write2File( "  </Character>\r\n" );
	_ASSERT( !ret );
///////

	ret = Write2File( "</BLD>" );
	_ASSERT( !ret );

	return 0;
}
