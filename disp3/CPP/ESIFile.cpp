#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include <ESIFile.h>
#include <mqofile.h>

#include <TreeHandler2.h>
#include <TreeElem2.h>
#include <ShdHandler.h>
#include <ShdElem.h>
#include <MotHandler.h>
#include <polymesh.h>
#include <polymesh2.h>
#include <InfElem.h>

#include <crtdbg.h>


static char esiheader[512] = "<ESIFILE>\r\n  <FileHeader>RokDeBone2 esi file ver1001 type00</FileHeader>\r\n";

enum {
	ESIFILE_WRITE,
	ESIFILE_LOAD,
	ESIFILE_MAX
};

extern UINT g_miplevels;
extern DWORD g_mipfilter;

/////////////////////////////

CESIFile::CESIFile()
{
	InitParams();
}
CESIFile::~CESIFile()
{
	DestroyObjs();
}

int CESIFile::InitParams()
{
	m_mode = ESIFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_esibuf.buf = 0;
	m_esibuf.bufleng = 0;
	m_esibuf.pos = 0;
	m_esibuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * ESILINELEN );

	m_loadversion = 0;

	m_mqofile = 0;
	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;

	m_curparent = 0;
	m_curjoint = 0;

	ZeroMemory( m_curname, sizeof( char ) * 256 );
	m_curpos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_enterjoint = 0;

	m_offset = 0;
	m_curseri = 0;

	ZeroMemory( m_dispname, sizeof( char ) * 256 );
	ZeroMemory( m_bonename, sizeof( char ) * 256 );

	m_mult = 1.0f;

	return 0;
}

int CESIFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == ESIFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_esibuf.buf ){
		free( m_esibuf.buf );
		m_esibuf.buf = 0;
	}
	m_esibuf.bufleng = 0;
	m_esibuf.pos = 0;
	m_esibuf.isend = 0;

	if( m_mqofile ){
		delete m_mqofile;
		m_mqofile = 0;
	}

	return 0;
}

int CESIFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "ESIFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "ESIFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "ESIFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "ESIFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_esibuf.buf = newbuf;
	m_esibuf.pos = 0;
	m_esibuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, "</ESIFILE>" );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_esibuf.bufleng = validleng;

	*( m_esibuf.buf + validleng ) = 0;

	return 0;
}

int CESIFile::ReadFileInfo()
{
	int ret;
	char tmpheader[256];
	ZeroMemory( tmpheader, sizeof( char ) * 256 );
	ret = Read_Str( &m_esibuf, "<FileHeader>", "</FileHeader>", tmpheader, 256 );
	if( ret ){
		DbgOut( "ESIFile : ReadFileInfo : Read_Str tmpheader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int cmp;
	cmp = strcmp( tmpheader, "RokDeBone2 esi file ver1001 type00" );
	if( cmp != 0 ){
		DbgOut( "ESIFile : ReadFileInfo : FileHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CESIFile::ReadMQO()
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_esibuf.buf, "<MQODATA>" );
	if( startptr ){
		startptr += (int)strlen( "<MQODATA>" );
	}

	endptr = strstr( m_esibuf.buf, "</MQODATA>" );

	if( !startptr || !endptr ){
		DbgOut( "ESIFile : ReadMQO : section pattern not found skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_esibuf.buf );
	if( (chkendpos >= (int)m_esibuf.bufleng) || (endptr < startptr) ){
		DbgOut( "ESIFile : ReadMQO : endmark skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	ESIBUF mqobuf;
	mqobuf.buf = startptr;
	mqobuf.bufleng = (int)( endptr - startptr );
	mqobuf.pos = 0;
	mqobuf.isend = 0;

	m_mqofile = new CMQOFile( 0 );
	if( !m_mqofile ){
		DbgOut( "ESIFile : ReadMQO : mqofile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );
	ret = m_mqofile->LoadMQOFileFromBuf( m_mult, mqobuf.buf, mqobuf.bufleng, m_lpth, m_lpsh, m_lpmh,
		0, 0, BONETYPE_RDB2, 0, offsetpos, rot );
	if( ret ){
		DbgOut( "ESIFile : ReadMQO : mqofile LoadMQOFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

/***
int CESIFile::ReadGround()
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_esibuf.buf, "<Ground>" );
	endptr = strstr( m_esibuf.buf, "</Ground>" );

	if( !startptr || !endptr ){
		DbgOut( "ESIFile : ReadGround : section pattern not found skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_esibuf.buf );
	if( (chkendpos >= (int)m_esibuf.bufleng) || (endptr < startptr) ){
		DbgOut( "ESIFile : ReadGround : endmark skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	ESIBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;


	char tempchar[MAX_PATH];
	ZeroMemory( tempchar, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &infobuf, "<file>", "</file>", tempchar, MAX_PATH );
	if( ret ){
		DbgOut( "ESIFile : ReadGround : Read_Str filename skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	sprintf_s( m_gesi.name, MAX_PATH, "%s\\%s", m_dir, tempchar );
	m_gesi.flag = 1;


	float mult = 1.0f;
	ret = Read_Float( &infobuf, "<mult>", "</mult>", &mult );
	if( ret || (mult <= 0.0f) ){
		DbgOut( "ESIFile : ReadGround : Read_Float mult skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	m_gesi.mult = mult;

	return 0;
}
***/

int CESIFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	strncpy_s( tempchar, 256, srcchar + pos, srcleng );

	*dstint = atoi( tempchar );

	*stepnum = srcleng;

	return 0;
}
int CESIFile::GetInt2( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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


int CESIFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );
	strncpy_s( tempchar, 256, srcchar + pos, srcleng );
	*dstfloat = (float)atof( tempchar );

	*stepnum = srcleng;


	return 0;
}
int CESIFile::GetFloat2( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CESIFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
{

	strncpy_s( dstchar, dstleng, srcchar + pos, srcleng );
	*( dstchar + srcleng ) = 0;

	return 0;
}



int CESIFile::Read_Int( ESIBUF* esibuf, char* startpat, char* endpat, int* dstint )
{
	int ret;
	char* startptr;
	startptr = strstr( esibuf->buf + esibuf->pos, startpat );
	if( !startptr ){
		DbgOut( "ESIFile : Read_Int : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( esibuf->buf + esibuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "ESIFile : Read_Int : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - esibuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)esibuf->bufleng) ){
		DbgOut( "ESIFile : Read_Int : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}


	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "ESIFile : Read_Int : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "ESIFile : Read_Int : %s GetInt error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CESIFile::Read_Float( ESIBUF* esibuf, char* startpat, char* endpat, float* dstfloat )
{
	int ret;
	char* startptr;
	startptr = strstr( esibuf->buf + esibuf->pos, startpat );
	if( !startptr ){
		DbgOut( "ESIFile : Read_Float : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( esibuf->buf + esibuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "ESIFile : Read_Float : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - esibuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)esibuf->bufleng) ){
		DbgOut( "ESIFile : Read_Float : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "ESIFile : Read_Float : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "ESIFile : Read_Float : %s GetFloat error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CESIFile::Read_Vec3( ESIBUF* esibuf, char* startpat, char* endpat, D3DXVECTOR3* dstvec )
{
	int ret;
	char* startptr;
	startptr = strstr( esibuf->buf + esibuf->pos, startpat );
	if( !startptr ){
		DbgOut( "ESIFile : Read_Vec3 : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( esibuf->buf + esibuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "ESIFile : Read_Vec3 : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - esibuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)esibuf->bufleng) ){
		DbgOut( "ESIFile : Read_Vec3 : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		DbgOut( "ESIFile : Read_Vec3 : elem leng error !!!\n", startpat );
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
		DbgOut( "ESIFile : Read_Vec3 : %s GetFloat X error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

//_ASSERT( 0 );
	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat2( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "ESIFile : Read_Vec3 : %s GetFloat Y error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

//_ASSERT( 0 );

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat2( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "ESIFile : Read_Vec3 : %s GetFloat Z error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

//_ASSERT( 0 );
	dstvec->x = xval;
	dstvec->y = yval;
	dstvec->z = zval;

	return 0;
}


int CESIFile::Read_Str( ESIBUF* esibuf, char* startpat, char* endpat, char* dststr, int arrayleng )
{
	int ret;
	char* startptr;
	startptr = strstr( esibuf->buf + esibuf->pos, startpat );
	if( !startptr ){
		DbgOut( "ESIFile : Read_Str : %s pattern not found error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( esibuf->buf + esibuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		DbgOut( "ESIFile : Read_Str : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - esibuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)esibuf->bufleng) ){
		DbgOut( "ESIFile : Read_Str : %s pattern error !!!\n", endpat );
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= arrayleng) ){
		DbgOut( "ESIFile : Read_Str : elem leng error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "ESIFile : Read_Str : %s GetName error !!!\n", startpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CESIFile::ReadJointData()
{
	int ret;

	ret = m_lpsh->DestroyBoneInfo( m_lpmh );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_lpmh->DestroyMotionObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}




	int firstparno = 1;
	ret = m_lpsh->GetFirstParentNo( &firstparno );
	if( ret || (firstparno <= 0) ){
		DbgOut( "ESIFile : ReadJointData : sh GetFirstParentNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_curparent = (*m_lpsh)( firstparno );
	_ASSERT( m_curparent );
	m_curjoint = m_curparent;


	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_esibuf.buf, "<JOINTDATA>" );
	endptr = strstr( m_esibuf.buf, "</JOINTDATA>" );

	if( !startptr || !endptr ){
		DbgOut( "ESIFile : ReadJointData : section pattern not found skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_esibuf.buf );
	if( (chkendpos >= (int)m_esibuf.bufleng) || (endptr < startptr) ){
		DbgOut( "ESIFile : ReadJointData : endmark skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	CTreeElem2* firstte = (*m_lpth)( m_curparent->serialno );
	_ASSERT( firstte );

	m_offset = m_lpsh->s2shd_leng;
	ret = m_lpth->Start( m_offset, firstte );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	m_curseri = m_offset;//!!!!!!!!!!!!


	ESIBUF jbuf;
	jbuf.buf = startptr;
	jbuf.bufleng = (int)( endptr - startptr );
	jbuf.pos = 0;
	jbuf.isend = 0;
	
	int firstflag = 1;
	int finishflag = 0;
	while( finishflag == 0 ){
		int getlen = 0;
		GetLine( &jbuf, &getlen );
		if( jbuf.isend ){
			break;
		}
		if( getlen <= 0 ){
			continue;
		}

		char strtreeelem[20] = "<TreeElem>";		
		char strtreeelem2[20] = "</TreeElem>";
		char strjointelem[20] = "<JointElem>";
		char strjointelem2[20] = "</JointElem>";
		char strjointname[20] = "<JointName>";
		char strjointpos[20] = "<JointPos>";
		char* cmptreeelem;
		char* cmptreeelem2;
		char* cmpjointelem;
		char* cmpjointelem2;
		char* cmpjointname;
		char* cmpjointpos;
		cmptreeelem = strstr( m_line, strtreeelem );
		cmptreeelem2 = strstr( m_line, strtreeelem2 );
		cmpjointelem = strstr( m_line, strjointelem );
		cmpjointelem2 = strstr( m_line, strjointelem2 );
		cmpjointname = strstr( m_line, strjointname );
		cmpjointpos = strstr( m_line, strjointpos );

		if( cmptreeelem != 0 ){
			m_curparent = m_curjoint;
			if( firstflag == 0 ){
				ret = m_lpth->Begin();
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}else{
				firstflag = 0;
			}
		}else if( cmptreeelem2 != 0 ){
			ret = m_lpth->End();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			m_curparent = m_lpsh->FindUpperJoint( m_curjoint );
		}else if( cmpjointelem != 0 ){
			ZeroMemory( m_curname, sizeof( char ) * 256 );
			m_curpos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_enterjoint = 1;
		}else if( cmpjointelem2 != 0 ){
			if( m_enterjoint == 0 ){
				DbgOut( "ESIFile : ReadJointData : enterjoint error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = AddJoint();
			if( ret ){
				DbgOut( "ESIFile : ReadJointData : AddJoint error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			m_enterjoint = 0;
		}else if( cmpjointname != 0 ){
			ESIBUF namebuf;
			ZeroMemory( &namebuf, sizeof( ESIBUF ) );
			namebuf.buf = m_line;
			namebuf.bufleng = (int)strlen( m_line );
			namebuf.isend = 0;
			namebuf.pos = 0;
			ret = Read_Str( &namebuf, "<JointName>", "</JointName>", m_curname, 256 );
			if( ret ){
				DbgOut( "ESIFile : ReadJointData : Read_Str jointname error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( cmpjointpos != 0 ){
			ESIBUF namebuf;
			ZeroMemory( &namebuf, sizeof( ESIBUF ) );
			namebuf.buf = m_line;
			namebuf.bufleng = (int)strlen( m_line );
			namebuf.isend = 0;
			namebuf.pos = 0;
			ret = Read_Vec3( &namebuf, "<JointPos>", "</JointPos>", &m_curpos );
			if( ret ){
				DbgOut( "ESIFile : ReadJointData : Read_Vec3 jointpos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	ret = m_lpsh->SetChain( m_offset );// treehandlerのchainが出来あがってから。
	if( ret ){
		DbgOut( "ESIFile : ReadJointData : shandler->SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_lpsh->SetClockwise();
	if( ret ){
		DbgOut( "ESIFile : ReadJointData : shandler->SetClockwise error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_lpmh->SetChain( m_offset );
	if( ret ){
		DbgOut( "ESIFile : ReadJointData : mhandler->SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CESIFile::AddJoint()
{
	int ret;

	int curdepth;
	curdepth = m_lpth->GetCurDepth();


	ret = m_lpth->AddTree( m_curname, m_curseri );
	if( m_curseri != ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_lpth->SetElemType( m_curseri, SHDBALLJOINT );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	
// shdhandler
	ret = m_lpsh->AddShdElem( m_curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
//	ret = lpsh->SetClockwise( curseri, sheader.clockwise );
//	if( ret ){
//		_ASSERT( 0 );
//		return 1;
//	}


// mothandler
	ret = m_lpmh->AddMotionCtrl( m_curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_lpmh->SetHasMotion( m_curseri, 1 );
	_ASSERT( !ret );
	

////////
////////
	CMeshInfo tempinfo;
	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	ret = tempinfo.SetMem( SHDBALLJOINT, BASE_TYPE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}	

	ret = m_lpsh->Init3DObj( m_curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CVec3f jointloc;
	jointloc.x = m_mult * m_curpos.x;
	jointloc.y = m_mult * m_curpos.y;
	jointloc.z = m_mult * -m_curpos.z;

//	jointloc.x = 7.0f * m_curpos.x;
//	jointloc.y = 7.0f * m_curpos.y - 25.0f;
//	jointloc.z = 7.0f * -m_curpos.z;
	
//	DbgOut( "checkloc!!! : ESIFile : pos %f, %f, %f\r\n", jointloc.x, jointloc.y, jointloc.z );

	m_curjoint = (*m_lpsh)(m_curseri);
	ret = m_curjoint->SetJointLoc( &jointloc );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


////////
	m_curseri++;

	return 0;
}


int CESIFile::GetLine( ESIBUF* srcbuf, int* cplenptr )
{
	ZeroMemory( m_line, sizeof( char ) * ESILINELEN );

	if( srcbuf->pos >= srcbuf->bufleng ){
		srcbuf->isend = 1;
		*cplenptr = 0;
		return 0;
	}

	char* startptr = srcbuf->buf + srcbuf->pos;
	char pat[3] = "\r\n";
	char* findptr = 0;
	findptr = strstr( startptr, pat );
//	if( findptr == startptr ){
//		_ASSERT( 0 );
//	}

	int cpleng = 0;
	if( findptr ){
		cpleng = (int)( findptr - startptr );
		strncpy_s( m_line, ESILINELEN, startptr, cpleng );
		m_line[ cpleng ] = 0;
	}else{
		cpleng = srcbuf->bufleng - srcbuf->pos;
		strncpy_s( m_line, ESILINELEN, startptr, cpleng );
		m_line[ cpleng ] = 0;
		//srcbuf->isend = 1;
	}
	*cplenptr = cpleng;
	srcbuf->pos += cpleng + 2;

	return 0;
}

int CESIFile::ReadInfData()
{
	int ret;
	int finishflag = 0;

	m_esibuf.pos = 0;
	while( finishflag == 0 ){
		char* startptr = 0;
		char* endptr = 0;

		startptr = strstr( m_esibuf.buf + m_esibuf.pos, "<INFDATA>" );
		if( !startptr ){
			finishflag = 1;
			break;
		}
		startptr += (int)strlen( "<INFDATA>" );

		endptr = strstr( startptr, "</INFDATA>" );
		if( !endptr ){
			DbgOut( "ESIFile : ReadInfData : section pattern </INFDATA> not found skip !!!\n" );
			finishflag = 1;
			break;
		}

		int chkendpos;
		chkendpos = (int)( endptr - m_esibuf.buf );
		if( (chkendpos >= (int)m_esibuf.bufleng) || (endptr < startptr) ){
			DbgOut( "ESIFile : ReadInfData : endmark skip !!!\n" );
			finishflag = 1;
			break;
		}

		ESIBUF ibuf;
		ibuf.buf = startptr;
		ibuf.bufleng = (int)( endptr - startptr );
		ibuf.pos = 0;
		ibuf.isend = 0;

		ret = Read_Str( &ibuf, "<PartName>", "</PartName>", m_dispname, 256 );
		if( ret ){
			DbgOut( "ESIFile : ReadInfData : Read_Str partname error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int binum = 0;
		ret = Read_Int( &ibuf, "<BoneInfNum>", "</BoneInfNum>", &binum );
		if( ret ){
			DbgOut( "ESIFile : ReadInfData : Read_Int boneinfnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int dispseri = 0;
		ret = m_lpth->GetDispObjNoByName( m_dispname, &dispseri, m_lpsh, 0 );
		if( ret || (dispseri <= 0) ){
			DbgOut( "ESIFile : ReadInfData : th GetDispObjNoByName %s not found skp !!!\n", m_dispname );
		}else{





			CShdElem* dispse = 0;
			dispse = (*m_lpsh)( dispseri );
			_ASSERT( dispse );

			int vertnum = 0;
			ret = m_lpsh->GetVertNumOfPart( dispseri, &vertnum );
			if( ret || (vertnum < 3) ){
				DbgOut( "ESIFile : ReadInfData : sh GetVertNumOfPart error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int vno0;
			for( vno0 = 0; vno0 < vertnum; vno0++ ){
				ret = dispse->DestroyIE( vno0 );
				if( ret ){
					DbgOut( "ESIFile : ReadInfData : se DestroyIE error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			int* dirtyflag = (int*)malloc( sizeof( int ) * vertnum );
			if( !dirtyflag ){
				DbgOut( "ESIFile : ReadInfData : dirtyflag alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( dirtyflag, sizeof( int ) * vertnum );

			int bino;
			for( bino = 0; bino < binum; bino++ ){

				if( ibuf.pos >= ibuf.bufleng ){
					DbgOut( "ESIFile : ReadInfData : ibuf.pos error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				char* bistartptr = 0;
				char* biendptr = 0;

				bistartptr = strstr( ibuf.buf + ibuf.pos, "<BoneInf>" );
				if( !bistartptr ){
					DbgOut( "ESIFile : ReadInfData : BoneInf Pattern not found error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				bistartptr += (int)strlen( "<BoneInf>" );

				biendptr = strstr( bistartptr, "</BoneInf>" );
				if( !endptr ){
					DbgOut( "ESIFile : ReadInfData : section pattern </BoneInf> not found error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				int bichkendpos;
				bichkendpos = (int)( biendptr - ibuf.buf );
				if( (bichkendpos >= (int)ibuf.bufleng) || (biendptr < bistartptr) ){
					DbgOut( "ESIFile : ReadInfData : endmark </BoneInf> error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ESIBUF bibuf;
				ZeroMemory( &bibuf, sizeof( ESIBUF ) );
				bibuf.buf = ibuf.buf;
				bibuf.pos = ibuf.pos;
				bibuf.bufleng = ibuf.bufleng;

				ret = ReadBoneInf( &bibuf, dispse, vertnum, dirtyflag );
				if( ret ){
					DbgOut( "ESIFile : ReadInfData : ReadBoneInf error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ibuf.pos = bichkendpos + (int)strlen( "</BoneInf>" );
			}
			
			CTreeElem2* dispte = (*m_lpth)( dispse->serialno );
			_ASSERT( dispte );

			int chkv;
			for( chkv = 0; chkv < vertnum; chkv++ ){
				if( *(dirtyflag + chkv) == 0 ){
					DbgOut( "ESIFile : ReadInfData : dispname %s, vertno %d infdata not exist warning !!!\r\n",
						dispte->name, chkv );
				}
			}

			free( dirtyflag );

		}

		

		m_esibuf.pos = chkendpos + (int)strlen( "</INFDATA>" );
		if( m_esibuf.pos >= m_esibuf.bufleng ){
			finishflag = 1;
		}
	}

	return 0;
}

int CESIFile::ConvOrgVertNo( CShdElem* dispse, BI* biptr, int vertnum )
{
	if( dispse->type != SHDPOLYMESH2 ){
		_ASSERT( 0 );
		return 1;
	}
	CPolyMesh2* pm2 = dispse->polymesh2;
	if( !pm2 ){
		_ASSERT( 0 );
		return 1;
	}

	int oldvnum = pm2->meshinfo->n * 3;
	_ASSERT( pm2->orgnobuf );


	int* optdirty = (int*)malloc( sizeof( int ) * vertnum );
	if( !optdirty ){
		DbgOut( "ESIFile : ConvOrgVertNo : optdirty alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( optdirty, sizeof( int ) * vertnum );

	int findnum = 0;
	int oldi;
	for( oldi = 0; oldi < oldvnum; oldi++ ){
		if( *( pm2->orgnobuf + oldi ) == biptr->convorg.orgvno ){
			_ASSERT( pm2->oldpno2optpno );
			int optno = *( pm2->oldpno2optpno + oldi );
			if( *( optdirty + optno ) == 0 ){
				*( optdirty + optno ) = 1;
				findnum++;
			}
		}
	}
	if( findnum <= 0 ){
		biptr->convorg.optvnum = 0;
		biptr->convorg.optvptr = 0;
		return 0;
	}
	int* tmpopt = (int*)malloc( sizeof( int ) * findnum );
	if( !tmpopt ){
		DbgOut( "ESIFile : ConvOrgVertNo : tmpmopt alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( tmpopt, sizeof( int ) * findnum );

	ZeroMemory( optdirty, sizeof( int ) * vertnum );

	int setno = 0;
	for( oldi = 0; oldi < oldvnum; oldi++ ){
		if( *( pm2->orgnobuf + oldi ) == biptr->convorg.orgvno ){
			_ASSERT( pm2->oldpno2optpno );
			int optno = *( pm2->oldpno2optpno + oldi );
			if( *( optdirty + optno ) == 0 ){
				*( tmpopt + setno ) = optno;
				*( optdirty + optno ) = 1;
				setno++;
			}
		}
	}

	if( setno != findnum ){
		_ASSERT( 0 );
		return 1;
	}

	free( optdirty );

	biptr->convorg.optvnum = findnum;
	biptr->convorg.optvptr = tmpopt;

	return 0;
}


int CESIFile::ReadBoneInf( ESIBUF* bibuf, CShdElem* dispse, int vertnum, int* dirtyflag )
{
	int ret;
	ret = Read_Str( bibuf, "<BoneName>", "</BoneName>", m_bonename, 256 );
	if( ret ){
		DbgOut( "ESIFile : ReadBoneInf : Read_Str bonename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int wnum = 0;
	ret = Read_Int( bibuf, "<WeightNum>", "</WeightNum>", &wnum );
	if( ret ){
		DbgOut( "ESIFile : ReadBoneInf : Read_Int WeightNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( wnum <= 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int boneseri = 0;
	CTreeElem2* bonete = 0;
	ret = m_lpth->GetBoneNoByName( m_bonename, &boneseri, m_lpsh, 0 );
	if( ret || (boneseri <= 0) ){
		DbgOut( "ESIFile : ReadBoneInf : bonename %s not found skip !!!\n", m_bonename );
		_ASSERT( 0 );
		return 0;
	}
	bonete = (*m_lpth)( boneseri );
	_ASSERT( bonete );

	BI* biptr;
	biptr = (BI*)malloc( sizeof( BI ) * wnum );
	if( !biptr ){
		DbgOut( "ESIFile : ReadBoneInf : biptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( biptr, sizeof( BI ) * wnum );

	char vipat[20] = "<VertIndex>\r\n";
	char* vifirst = 0;
	vifirst = strstr( bibuf->buf + bibuf->pos, vipat );
	if( !vifirst ){
		DbgOut( "ESIFile : ReadBoneInf : <VertIndex> not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	vifirst += (int)strlen( vipat );


	bibuf->pos = (int)( vifirst - bibuf->buf );
	int wno;
	for( wno = 0; wno < wnum; wno++ ){
		if( bibuf->pos >= bibuf->bufleng ){
			DbgOut( "ESIFile : ReadBoneInf : VertIndex bibuf->pos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int getlen = 0;
		GetLine( bibuf, &getlen );
		if( bibuf->isend ){
			DbgOut( "ESIFile : ReadBoneInf : VertIndex bibuf->isend error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( getlen <= 0 ){
			DbgOut( "ESIFile : ReadBoneInf : VertIndex getlen error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	
		int tmpvno = 0;
		int stepnum = 0;
		ret = GetInt2( &tmpvno, m_line, 0, ESILINELEN, &stepnum );
		if( ret ){
			DbgOut( "ESIFile : ReadBoneInf : VertIndex GetInt2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		(biptr + wno)->convorg.orgvno = tmpvno;
		ret = ConvOrgVertNo( dispse, (biptr + wno), vertnum );
		if( ret ){
			DbgOut( "ESIFile : ReadBoneInf : VertIndex ConvOrgVertNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}
	


	char wpat[20] = "<Weight>\r\n";
	char* wfirst = 0;
	wfirst = strstr( bibuf->buf + bibuf->pos, wpat );
	if( !wfirst ){
		DbgOut( "ESIFile : ReadBoneInf : <Weight> not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	wfirst += (int)strlen( wpat );
	
	bibuf->pos = (int)( wfirst - bibuf->buf );
	for( wno = 0; wno < wnum; wno++ ){
		if( bibuf->pos >= bibuf->bufleng ){
			DbgOut( "ESIFile : ReadBoneInf : Weight bibuf->pos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int getlen = 0;
		GetLine( bibuf, &getlen );
		if( bibuf->isend ){
			DbgOut( "ESIFile : ReadBoneInf : Weight bibuf->isend error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( getlen <= 0 ){
			DbgOut( "ESIFile : ReadBoneInf : Weight getlen error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	
		int stepnum = 0;
		ret = GetFloat2( &((biptr + wno)->weight), m_line, 0, ESILINELEN, &stepnum );
		if( ret ){
			DbgOut( "ESIFile : ReadBoneInf : VertIndex GetInt2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}
	
//////////
	for( wno = 0; wno < wnum; wno++ ){
		BI* curbi = biptr + wno;
		int opti;
		for( opti = 0; opti < curbi->convorg.optvnum; opti++ ){
			int optvno = *( curbi->convorg.optvptr + opti );
			if( (optvno >= 0) && (optvno < vertnum) ){
				int updateflag;
				updateflag = 0;
				int paintmode;
				//if( *( dirtyflag + optvno ) == 1 ){
					paintmode = PAINT_NOR;
				//}else{
				//	paintmode = PAINT_EXC;
				//}

				ret = m_lpsh->SetInfElem( dispse->serialno, optvno, boneseri, paintmode, CALCMODE_DIRECT0, 1.0f,
					1, curbi->weight, updateflag );
				if( ret ){
					DbgOut( "ESIFile : ReadBoneInf : sh SetInfElem error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				CTreeElem2* dispte = (*m_lpth)( dispse->serialno );
				_ASSERT( dispte );

				*( dirtyflag + optvno ) = 1;
			}
		}
	}

	if( biptr ){
		int bno;
		for( bno = 0; bno < wnum; bno++ ){
			BI* curbi = biptr + bno;
			if( curbi->convorg.optvptr ){
				free( curbi->convorg.optvptr );
			}
		}
		free( biptr );
	}

	return 0;
}



int CESIFile::LoadESIFile( LPDIRECT3DDEVICE9 pdev, char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, float mult )
{
	int ret;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;
	m_mode = ESIFILE_LOAD;
	m_mult = mult;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "ESIFile : LoadESIFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	ret = SetBuffer();
	if( ret ){
		DbgOut( "ESIFile : LoadESIFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadFileInfo();
	if( ret ){
		DbgOut( "ESIFile : LoadESIFile : ReadFileInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = ReadMQO();
	if( ret ){
		DbgOut( "ESIFile : LoadESIFile : ReadMQO error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = ReadJointData();
	if( ret ){
		DbgOut( "ESIFile : LoadESIFile : ReadJointData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = Restore( pdev );
	if( ret ){
		DbgOut( "ESIFile : LoadESIFile : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ReadInfData();
	if( ret ){
		DbgOut( "ESIFile : LoadESIFile : ReadInfData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = m_lpsh->CreateSkinMat( -1, 1 );
	if( ret ){
		DbgOut( "ESIFile : LoadESIFile : sh CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	lpmh->m_standard = 1;//!!!!!!!!!!!!!!!!!!!


	return 0;
}

int CESIFile::Restore( LPDIRECT3DDEVICE9 pdev )
{
	int ret;

	switch( m_lpsh->m_shader ){
	case COL_PHONG:
	case COL_OLD:
	case COL_BLINN:
	case COL_SCHLICK:
	case COL_MQCLASSIC:
		ret = m_lpsh->SetIndexBufColP();
		if( ret ){
			DbgOut( "ESIFile : Restore : IDTBB_COLP : COL_PHONG : sh SetIndexBufColP error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case COL_PPHONG:
		ret = m_lpsh->SetIndexBufColP();
		if( ret ){
			DbgOut( "ESIFile : Restore : IDTBB_COLPP : COL_PPHONG : sh SetIndexBufColP error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case COL_TOON1:
		break;
	default:
		_ASSERT( 0 );
		m_lpsh->m_shader = COL_PHONG;

		ret = m_lpsh->SetIndexBufColP();
		if( ret ){
			DbgOut( "ESIFile : Restore : IDTBB_COLP : COL_PHONG : sh SetIndexBufColP error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	}

	ret = m_lpsh->CreateBoneInfo( m_lpmh );
	if( ret ){
		DbgOut( "ESIFile : Restore : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_lpsh ){
		ret = m_lpsh->CreateTexture( pdev, NULL, 1, g_miplevels, g_mipfilter, 0, 0 );
		if( ret ){
			DbgOut( "ESIFile : Restore : m_shandler->CreateTexture error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}

		ret = m_lpsh->SetMikoAnchorApply( m_lpth );
		if( ret ){
			DbgOut( "ESIFile : Restore : sh SetMikoAnchorApply error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}

		ret = m_lpsh->CreateDispObj( pdev, m_lpmh, 1 );
		if( ret ){
			DbgOut( "ESIFile : Restore : m_shandler->CreateDispObj error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}
		ret = m_lpsh->SetTexTransparent();
		_ASSERT( !ret );

		ret = m_lpsh->SetDispFlag( 1 );
		if( ret ){
			DbgOut( "ESIFile : Restore : m_shandler->SetDispFlag error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}
		//COL_TOON1
		if( m_lpsh->m_shader == COL_TOON1 ){
			ret = m_lpsh->CreateToon1Buffer( pdev );
			if( ret ){
				DbgOut( "ESIFile : Restore : IDTBB_COLT : COL_TOON1 : sh CreateToon1Buffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


int CESIFile::Write2File( int tabnum, char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar0[ESILINELEN];
	char outchar1[ESILINELEN];
	char tabchar[ESILINELEN];
	ZeroMemory( outchar0, ESILINELEN );
	ZeroMemory( outchar1, ESILINELEN );
	ZeroMemory( tabchar, ESILINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar0, ESILINELEN, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 ){
		_ASSERT( 0 );
		return 1;
	}

	if( tabnum >= 1 ){
		strcpy_s( tabchar, ESILINELEN, "  " );
	}
	int tno;
	for( tno = 1; tno < tabnum; tno++ ){
		strcat_s( tabchar, ESILINELEN, "  " );
	}
	sprintf_s( outchar1, ESILINELEN, "%s%s", tabchar, outchar0 );

	wleng = (unsigned long)strlen( outchar1 );
	WriteFile( m_hfile, outchar1, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CESIFile::WriteESIFileOnFrame( char* filename, int motid, int frameno, 
		CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
{
	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "ESIFile : WriteESIFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	m_mode = ESIFILE_WRITE;	

	int ret;
	ret = Write2File( 0, esiheader );
	_ASSERT( !ret );
///////
	ret = Write2File( 0, "<MQODATA>\r\n" );
	_ASSERT( !ret );

	m_mqofile = new CMQOFile( 0 );
	if( !m_mqofile ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mqofile->SaveMQOFileOnFrame( 0, motid, frameno, 0, m_lpth, m_lpsh, m_lpmh, 1.0f, 0, m_hfile );
	if( ret ){
		DbgOut( "ESIFile : WriteESIFileOnFrame : mqofile SaveMQOFileOnFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( 0, "</MQODATA>\r\n" );
	_ASSERT( !ret );

	delete m_mqofile;
	m_mqofile = 0;

///////
	ret = Write2File( 0, "<EXTENDDATA>\r\n" );
	_ASSERT( !ret );

	ret = WriteJointData( motid, frameno );
	if( ret ){
		DbgOut( "ESIFile : WriteESIFileOnFrame : WriteJointData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteInfData();
	if( ret ){
		DbgOut( "ESIFile : WriteESIFileOnFrame : WriteInfData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( 0, "</EXTENDDATA>\r\n" );
	_ASSERT( !ret );

///////

	ret = Write2File( 0, "</ESIFILE>\r\n" );
	_ASSERT( !ret );

	return 0;
}


int CESIFile::WriteJointData( int motid, int frameno )
{
	int ret;
	ret = Write2File( 1, "<JOINTDATA>\r\n" );
	_ASSERT( !ret );

	CShdElem* topse = (*m_lpsh)( 0 );
	_ASSERT( topse );

	WriteJointElemReq( topse, 1, 0, motid, frameno );

	ret = Write2File( 1, "</JOINTDATA>\r\n\r\n" );
	_ASSERT( !ret );

	return 0;
}

void CESIFile::WriteJointElemReq( CShdElem* srcse, int tabcnt, int broflag, int motid, int frameno )
{
	int ret;
	if( srcse->IsJoint() ){
		ret = Write2File( tabcnt, "<TreeElem>\r\n" );
		_ASSERT( !ret );

		ret = Write2File( tabcnt + 1, "<JointElem>\r\n" );
		_ASSERT( !ret );
      
		CTreeElem2* telem;
		telem = (*m_lpth)( srcse->serialno );
		_ASSERT( telem );

		ret = Write2File( tabcnt + 2, "<JointName>%s</JointName>\r\n", telem->name );
		_ASSERT( !ret );


		ret = m_lpmh->SetCurrentMotion( m_lpsh, motid, frameno );
		if( ret ){
			DbgOut( "ESIFile : WriteJointElemReq : mh SetCurrentMotion error !!!\n" );
			_ASSERT( 0 );
			return;
		}
		D3DXMATRIX iniwmat;
		D3DXMatrixIdentity( &iniwmat );
		D3DXVECTOR3 trav;
		ret = srcse->TransformOnlyWorld3( m_lpmh, iniwmat, &trav );
		if( ret ){
			DbgOut( "ESIFile : WriteJointElemReq : selem TransformOnlyWorld3 error !!!\n" );
			_ASSERT( 0 );
			return;
		}
		ret = Write2File( tabcnt + 2, "<JointPos>%f, %f, %f</JointPos>\r\n", trav.x, trav.y, -trav.z );
		_ASSERT( !ret );
		
		ret = Write2File( tabcnt + 1, "</JointElem>\r\n" );
		_ASSERT( !ret );

	}

	if( srcse->child ){
		WriteJointElemReq( srcse->child, tabcnt + 1, 1, motid, frameno );
	}

	if( srcse->IsJoint() ){
		ret = Write2File( tabcnt, "</TreeElem>\r\n" );
		_ASSERT( !ret );
	}
	
	if( broflag && srcse->brother ){
		WriteJointElemReq( srcse->brother, tabcnt, 1, motid, frameno );
	}
}


int CESIFile::WriteInfData()
{
	int ret;
	int seri;
	for( seri = 0; seri < m_lpsh->s2shd_leng; seri++ ){
		CShdElem* curse = (*m_lpsh)( seri );
		if( (curse->type == SHDPOLYMESH) || (curse->type == SHDPOLYMESH2) ){

			int vertnum = 0;
			ret = m_lpsh->GetVertNumOfPart( seri, &vertnum );
			if( ret || (vertnum <= 0) ){
				DbgOut( "ESIFile : WriteInfData : sh GetVertNumOfPart error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = Write2File( 1, "<INFDATA>\r\n" );
			_ASSERT( !ret );

			CTreeElem2* curte = (*m_lpth)( seri );
			_ASSERT( curte );
			ret = Write2File( 2, "<PartName>%s</PartName>\r\n", curte->name );
			_ASSERT( !ret );

		//skinmesh header
			int infbonenum = 0;
			int* seri2infbno;

			seri2infbno = (int*)malloc( sizeof( int ) * m_lpsh->s2shd_leng );
			if( !seri2infbno ){
				DbgOut( "ESIFile : WriteInfData : seri2infbno alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = curse->MakeXSeri2InfBno( m_lpsh->s2shd_leng, seri2infbno, &infbonenum );
			if( ret ){
				DbgOut( "ESIFile : WriteInfData : MakeXSeri2InfBno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = Write2File( 2, "<BoneInfNum>%d</BoneInfNum>\r\n\r\n", infbonenum - 1 );//不動のためのダミーボーンの個数を含んでいる
			_ASSERT( !ret );

		//skin weights
			BONEINFLUENCE* boneinf;
			boneinf = (BONEINFLUENCE*)malloc( sizeof( BONEINFLUENCE ) * m_lpsh->s2shd_leng );
			if( !boneinf ){
				DbgOut( "ESIFile : WriteInfData : boneinf alloc error !!!\n" );
				_ASSERT( 0  );
				return 1;
			}
			ZeroMemory( boneinf, sizeof( BONEINFLUENCE ) * m_lpsh->s2shd_leng );

			ret = curse->MakeXBoneInfluence( m_lpsh->s2shd_leng, infbonenum, seri2infbno, boneinf );
			if( ret ){
				DbgOut( "ESIFile : WriteInfData : MakeXBoneInfluence error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int bisetno = 0;
			int bino;
			for( bino = 0; bino < m_lpsh->s2shd_leng; bino++ ){
				BONEINFLUENCE* curbi = boneinf + bino;
				_ASSERT( curbi );

				if( (curbi->Bone >= 0) && (curbi->numInfluences > 0) ){

					ret = Write2File( 2, "<BoneInf>\r\n" );
					_ASSERT( !ret );

					CTreeElem2* bonete = (*m_lpth)( bino );
					_ASSERT( bonete );

					ret = Write2File( 3, "<BoneName>%s</BoneName>\r\n", bonete->name );
					_ASSERT( !ret );
					ret = Write2File( 3, "<WeightNum>%d</WeightNum>\r\n", curbi->numInfluences );
					_ASSERT( !ret );


					ret = Write2File( 3, "<VertIndex>\r\n" );
					_ASSERT( !ret );
					DWORD ivno;
					for( ivno = 0; ivno < curbi->numInfluences; ivno++ ){
						if( ivno != (curbi->numInfluences - 1) ){
							ret = Write2File( 4, "%d,\r\n", *(curbi->vertices + ivno) );
						}else{
							ret = Write2File( 4, "%d\r\n", *(curbi->vertices + ivno) );
						}
						_ASSERT( !ret );
					}
					ret = Write2File( 3, "</VertIndex>\r\n" );
					_ASSERT( !ret );


					ret = Write2File( 3, "<Weight>\r\n" );
					_ASSERT( !ret );
					for( ivno = 0; ivno < curbi->numInfluences; ivno++ ){
						if( ivno != (curbi->numInfluences - 1) ){
							ret = Write2File( 4, "%f,\r\n", *(curbi->weights + ivno) );
						}else{
							ret = Write2File( 4, "%f\r\n", *(curbi->weights + ivno) );
						}
						_ASSERT( !ret );
					}
					ret = Write2File( 3, "</Weight>\r\n" );
					_ASSERT( !ret );



					ret = Write2File( 2, "</BoneInf>\r\n\r\n" );
					_ASSERT( !ret );

					bisetno++;
				}

			}

			if( bisetno != (infbonenum - 1) ){
				DbgOut( "ESIFile : WriteInfData : bisetno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		// free
			free( seri2infbno );

			int seri;
			for( seri = 0; seri < m_lpsh->s2shd_leng; seri++ ){
				BONEINFLUENCE* curbi;
				curbi = boneinf + seri;

				if( curbi->vertices ){
					free( curbi->vertices );
				}
				if( curbi->weights ){
					free( curbi->weights );
				}
			}
			free( boneinf );


			ret = Write2File( 1, "</INFDATA>\r\n\r\n" );
			_ASSERT( !ret );

		}
	}
	
	return 0;
}

