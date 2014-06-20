#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include "GPFile.h"

#include <GPAnimHandler.h>
#include <GPAnim.h>
#include <GPKey.h>

#include <crtdbg.h>


enum {
	GPFILE_WRITE,
	GPFILE_LOAD,
	GPFILE_MAX
};

static char strheader[256] = "RokDeBone2 GlobalPositionAnim File ver1001 type0\r\n";

static char stranimstart[256] = "#AnimInfo Start\r\n";
static char stranimend[256] = "#AnimInfo End\r\n";

static char strgpstart[256] = "#GPAnim Start\r\n";
static char strgpend[256] = "#GPAnim End\r\n";

static char strgpkeystart[256] = "#GPKey Start\r\n";
static char strgpkeyend[256] = "#GPKey End\r\n";

static char strframestart[256] = "#GPFullFrame Start\r\n";
static char strframeend[256] = "#GPFullFrame End\r\n";

static char strendfile[256] = "#EndOfFile\r\n";

static char stranimtype[MOTIONTYPEMAX][20] = {
	"CLAMP", "STOP", "CLAMP", "ROUND", "INV", "JUMP"
};

static char strinterp[INTERPOLATION_MAX][20] = {
	"LINEAR", "SPLINE"
};

static char strongmode[GROUND_MAX][20] = {
	"NONE", "ONECE", "ON"
};

CGPFile::CGPFile()
{
	InitParams();
}
CGPFile::~CGPFile()
{
	DestroyObjs();
}

int CGPFile::InitParams()
{
/***
int m_mode;
HANDLE m_hfile;
GPBUF m_gpbuf;
char m_line[ GPLINELEN ];
CGPAnim* m_anim;
***/
	m_mode = GPFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_gpbuf.buf = 0;
	m_gpbuf.bufleng = 0;
	m_gpbuf.pos = 0;
	m_gpbuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * GPLINELEN );
	m_anim = 0;
	m_loadversion = 0;

	return 0;
}
int CGPFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == GPFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_gpbuf.buf ){
		free( m_gpbuf.buf );
		m_gpbuf.buf = 0;
	}
	m_gpbuf.bufleng = 0;
	m_gpbuf.pos = 0;
	m_gpbuf.isend = 0;

	return 0;
}

int CGPFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[GPLINELEN];
			
	ZeroMemory( outchar, GPLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, GPLINELEN, lpFormat, Marker );
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

int CGPFile::WriteGPFile( char* filename, CGPAnim* srcanim )
{
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "GPFile : WriteGPFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_anim = srcanim;
	if( !m_anim ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = Write2File( strheader );
	_ASSERT( !ret );

	ret = WriteAnimInfo();
	if( ret ){
		DbgOut( "GPFile : WriteGPFile : WriteAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteGPAnim();
	if( ret ){
		DbgOut( "GPFile : WriteGPFile : WriteGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteGPFullFrame();
	if( ret ){
		DbgOut( "GPFile : WriteGPFile : WriteGPFullFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = Write2File( strendfile );
	_ASSERT( !ret );

	return 0;
}

int CGPFile::WriteAnimInfo()
{
/***
#AnimInfo Start
	name "gpera_1"
	frameleng 60
	animtype "CLAMP"		//"STOP", "CLAMP", "ROUND", "INV", "JUMP"
	animstep 1
	motjump 0
	groundhsid 0
#AnimInfo End
***/

	int ret;
	ret = Write2File( stranimstart );
	_ASSERT( !ret );

	ret = Write2File( "\tname \"%s\"\r\n", m_anim->m_animname );
	_ASSERT( !ret );

	ret = Write2File( "\tframeleng %d\r\n", m_anim->m_maxframe + 1 );
	_ASSERT( !ret );


	int typeindex;
	if( (m_anim->m_animtype >= MOTION_CLAMP) && (m_anim->m_animtype <= MOTION_JUMP) ){
		typeindex = m_anim->m_animtype;
	}else{
		typeindex = 0;
	}
	ret = Write2File( "\tanimtype \"%s\"\r\n", &( stranimtype[ typeindex ][0] ) );
	_ASSERT( !ret );

	ret = Write2File( "\tanimstep %d\r\n", m_anim->m_animstep );
	_ASSERT( !ret );

	ret = Write2File( "\tmotjump %d\r\n", m_anim->m_motjump );
	_ASSERT( !ret );

	ret = Write2File( "\tgroundhsid %d\r\n", m_anim->m_groundhsid );
	_ASSERT( !ret );

	ret = Write2File( stranimend );
	_ASSERT( !ret );

	return 0;
}
int CGPFile::WriteGPAnim()
{
	int ret;

	ret = Write2File( &(strgpstart[0]) );
	_ASSERT( !ret );

	int keynum = 0;
	ret = m_anim->GetGPKeyframeNoRange( 0, m_anim->m_maxframe, 0, 0, &keynum );
	if( ret ){
		DbgOut( "GPFile : WriteGPAnim : ca GetGPKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( "\tkeynum %d\r\n", keynum );
	_ASSERT( !ret );

	if( keynum > 0 ){
		int* keynoarray;
		keynoarray = (int*)malloc( sizeof( int ) * keynum );
		if( !keynoarray ){
			DbgOut( "GPFile : WriteGPAnim : keynoarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( keynoarray, sizeof( int ) * keynum );

		int getnum = 0;
		ret = m_anim->GetGPKeyframeNoRange( 0, m_anim->m_maxframe, keynoarray, keynum, &getnum );
		if( ret || (getnum != keynum) ){
			DbgOut( "GPFile : WriteGPAnim : ca GetGPKeyframeNoRange 1 error !!!\n" );
			_ASSERT( 0 );
			free( keynoarray );
			return 1;
		}

		int keyno;
		for( keyno = 0; keyno < keynum; keyno++ ){
			int curframeno;
			curframeno = *( keynoarray + keyno );

			ret = Write2File( "\t%s", strgpkeystart );
			_ASSERT( !ret );

			CGPKey* gpkptr = 0;
			ret = m_anim->ExistGPKey( curframeno, &gpkptr );
			if( ret || !gpkptr ){
				DbgOut( "GPFile : WriteGPAnim : ca ExistGPKey error !!!\n" );
				_ASSERT( 0 );
				free( keynoarray );
				return 1;
			}

			ret = Write2File( "\t\tframeno %d\r\n", gpkptr->m_frameno );
			_ASSERT( !ret );

			int interpindex;
			if( (gpkptr->m_interp >= INTERPOLATION_SLERP) && (gpkptr->m_interp <= INTERPOLATION_SQUAD) ){
				interpindex = gpkptr->m_interp;
			}else{
				interpindex = 0;
			}
			ret = Write2File( "\t\tinterp \"%s\"\r\n", &(strinterp[interpindex][0]) );
			_ASSERT( !ret );


			ret = Write2File( "\t\tpos %f, %f, %f\r\n", gpkptr->m_gpe.pos.x, gpkptr->m_gpe.pos.y, gpkptr->m_gpe.pos.z );
			_ASSERT( !ret );

			ret = Write2File( "\t\trot %f, %f, %f\r\n", gpkptr->m_gpe.rot.x, gpkptr->m_gpe.rot.y, gpkptr->m_gpe.rot.z );
			_ASSERT( !ret );

			int ongmodeindex;
			if( (gpkptr->m_gpe.ongmode >= GROUND_NONE) && (gpkptr->m_gpe.ongmode <= GROUND_ON) ){
				ongmodeindex = gpkptr->m_gpe.ongmode;
			}else{
				ongmodeindex = GROUND_NONE;
			}
			ret = Write2File( "\t\tongmode \"%s\"\r\n", &(strongmode[ongmodeindex][0]) );
			_ASSERT( !ret );


			ret = Write2File( "\t\trayy %f\r\n", gpkptr->m_gpe.rayy );
			_ASSERT( !ret );

			ret = Write2File( "\t\trayleng %f\r\n", gpkptr->m_gpe.rayleng );
			_ASSERT( !ret );

			ret = Write2File( "\t\toffsety %f\r\n", gpkptr->m_gpe.offsety );
			_ASSERT( !ret );

			ret = Write2File( "\t%s", strgpkeyend );
			_ASSERT( !ret );
		}
		free( keynoarray );
	}
	ret = Write2File( &(strgpend[0]) );
	_ASSERT( !ret );

	return 0;
}

int CGPFile::WriteGPFullFrame()
{
	int ret;

	ret = Write2File( &(strframestart[0]) );
	_ASSERT( !ret );

	int framenum = m_anim->m_maxframe + 1;
	if( framenum > 0 ){
		int curframeno;
		for( curframeno = 0; curframeno < framenum; curframeno++ ){
			ret = Write2File( "\t%s", strgpkeystart );
			_ASSERT( !ret );


			GPELEM gpe;
			int interp;
			int existflag;
			ret = m_anim->GetGPAnim( &gpe, &interp, curframeno, &existflag );
			if( ret ){
				DbgOut( "GPFile : WriteGPAnim : ca GetGPAnim error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = Write2File( "\t\tframeno %d\r\n", curframeno );
			_ASSERT( !ret );

			int interpindex;
			if( (interp >= INTERPOLATION_SLERP) && (interp <= INTERPOLATION_SQUAD) ){
				interpindex = interp;
			}else{
				interpindex = 0;
			}
			ret = Write2File( "\t\tinterp \"%s\"\r\n", &(strinterp[interpindex][0]) );
			_ASSERT( !ret );


			ret = Write2File( "\t\tpos %f, %f, %f\r\n", gpe.pos.x, gpe.pos.y, gpe.pos.z );
			_ASSERT( !ret );

			ret = Write2File( "\t\trot %f, %f, %f\r\n", gpe.rot.x, gpe.rot.y, gpe.rot.z );
			_ASSERT( !ret );

			int ongmodeindex;
			if( (gpe.ongmode >= GROUND_NONE) && (gpe.ongmode <= GROUND_ON) ){
				ongmodeindex = gpe.ongmode;
			}else{
				ongmodeindex = GROUND_NONE;
			}
			ret = Write2File( "\t\tongmode \"%s\"\r\n", &(strongmode[ongmodeindex][0]) );
			_ASSERT( !ret );


			ret = Write2File( "\t\trayy %f\r\n", gpe.rayy );
			_ASSERT( !ret );

			ret = Write2File( "\t\trayleng %f\r\n", gpe.rayleng );
			_ASSERT( !ret );

			ret = Write2File( "\t\toffsety %f\r\n", gpe.offsety );
			_ASSERT( !ret );

			ret = Write2File( "\t%s", strgpkeyend );
			_ASSERT( !ret );
		}
	}
	ret = Write2File( &(strframeend[0]) );
	_ASSERT( !ret );

	return 0;
}

int CGPFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "GPFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "GPFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "GPFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "GPFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_gpbuf.buf = newbuf;
	m_gpbuf.pos = 0;
	m_gpbuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, strendfile );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_gpbuf.bufleng = validleng;

	return 0;
}

int CGPFile::CheckFileVersion( int* verptr )
{

	char* headerptr;
	headerptr = strstr( m_gpbuf.buf, strheader );
	if( headerptr ){
		*verptr = 1001;
	}else{
		*verptr = 0;
	}

	return 0;
}
int CGPFile::ReadAnimInfo( CGPAnimHandler* srcgpah )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_gpbuf.buf, stranimstart );
	endptr = strstr( m_gpbuf.buf, stranimend );

	if( !startptr || !endptr ){
		DbgOut( "GPFile : ReadAnimInfo : section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_gpbuf.buf );
	if( (chkendpos >= (int)m_gpbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "GPFile : ReadAnimInfo : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	GPBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;

	char strname[256];
	ZeroMemory( strname, sizeof( char ) * 256 );
	ret = Read_Str( &infobuf, "name ", strname, 256 );
	if( ret ){
		DbgOut( "GPFile : ReadAnimInfo : Read_Str name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int frameleng = 0;
	ret = Read_Int( &infobuf, "frameleng ", &frameleng );
	if( ret || (frameleng <= 0) ){
		DbgOut( "GPFile : ReadAnimInfo : Read_Int frameleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char strtype[20];
	ZeroMemory( strtype, sizeof( char ) * 20 );
	ret = Read_Str( &infobuf, "animtype ", strtype, 20 );
	if( ret ){
		DbgOut( "GPFile : ReadAnimInfo : Read_Str animtype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int animtype = MOTION_CLAMP;
	int chktype;
	for( chktype = MOTION_STOP; chktype <= MOTION_JUMP; chktype++ ){
		int cmp;
		cmp = strcmp( strtype, &(stranimtype[chktype][0]) );
		if( cmp == 0 ){
			animtype = chktype;
			break;
		}
	}

	int animstep = 0;
	ret = Read_Int( &infobuf, "animstep ", &animstep );
	if( ret || (animstep <= 0) ){
		DbgOut( "GPFile : ReadAnimInfo : Read_Int animstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int motjump = 0;
	ret = Read_Int( &infobuf, "motjump ", &motjump );
	if( ret || (motjump < 0) || (motjump >= frameleng) ){
		DbgOut( "GPFile : ReadAnimInfo : Read_Int motjump error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int groundhsid = 0;
	ret = Read_Int( &infobuf, "groundhsid ", &groundhsid );
	if( ret ){
		DbgOut( "GPFile : ReadAnimInfo : Read_Int groundhsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	m_anim = srcgpah->AddAnim( strname, animtype, frameleng, motjump );
	if( !m_anim ){
		DbgOut( "GPFile : ReadAnimInfo : gpah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_anim->m_animstep = animstep;
	m_anim->m_groundhsid = groundhsid;

	return 0;
}
int CGPFile::ReadGPAnim()
{
	int ret;
	char* startpat;
	char* endpat;
	startpat = &(strgpstart[0]);
	endpat = &(strgpend[0]);

	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( m_gpbuf.buf, startpat );
	endptr = strstr( m_gpbuf.buf, endpat );

	if( !startptr || !endptr ){
		DbgOut( "GPFile : ReadGPAnim : anim section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_gpbuf.buf );
	if( (chkendpos >= (int)m_gpbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "GPFile : ReadGPAnim : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	GPBUF animbuf;
	animbuf.buf = startptr;
	animbuf.bufleng = (int)( endptr - startptr );
	animbuf.pos = 0;
	animbuf.isend = 0;

	int keynum = 0;
	ret = Read_Int( &animbuf, "keynum ", &keynum );
	if( ret || (keynum < 0) ){
		DbgOut( "GPFile : ReadGPAnim : Read_Int keynum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int keyno;
	for( keyno = 0; keyno < keynum; keyno++ ){
		int frameflag = 0;
		ret = ReadGPKey( &animbuf, frameflag );
		if( ret ){
			DbgOut( "GPFile : ReadGPAnim : ReadGPKey %d error !!!\n", keyno );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CGPFile::ReadGPFullFrame()
{
	int ret;
	char* startpat;
	char* endpat;
	startpat = &(strframestart[0]);
	endpat = &(strframeend[0]);

	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( m_gpbuf.buf, startpat );
	endptr = strstr( m_gpbuf.buf, endpat );

	if( !startptr || !endptr ){
		DbgOut( "GPFile : ReadGPAnim : anim section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_gpbuf.buf );
	if( (chkendpos >= (int)m_gpbuf.bufleng) || (endptr < startptr) ){
		DbgOut( "GPFile : ReadGPAnim : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	GPBUF animbuf;
	animbuf.buf = startptr;
	animbuf.bufleng = (int)( endptr - startptr );
	animbuf.pos = 0;
	animbuf.isend = 0;

	int framenum = m_anim->m_maxframe + 1;
	int frameno;
	for( frameno = 0; frameno < framenum; frameno++ ){
		int frameflag = 1;
		ret = ReadGPKey( &animbuf, frameflag );
		if( ret ){
			DbgOut( "GPFile : ReadGPFullFrame : ReadGPKey %d error !!!\n", frameno );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CGPFile::ReadGPKey( GPBUF* animbuf, int frameflag )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( animbuf->buf + animbuf->pos, strgpkeystart );
	endptr = strstr( animbuf->buf + animbuf->pos, strgpkeyend );

	if( !startptr || !endptr ){
		DbgOut( "GPFile : ReadGPKey : key section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - animbuf->buf );
	if( (chkendpos >= (int)animbuf->bufleng) || (endptr < startptr) ){
		DbgOut( "GPFile : ReadGPKey : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	GPBUF keybuf;
	keybuf.buf = startptr;
	keybuf.bufleng = (int)( endptr - startptr );
	keybuf.pos = 0;
	keybuf.isend = 0;
	
	int frameno = 0;
	ret = Read_Int( &keybuf, "frameno ", &frameno );
	if( ret || (frameno < 0) || (frameno > m_anim->m_maxframe) ){
		DbgOut( "GPFile : ReadGPKey : Read_Int frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char tmpinterp[20];
	ZeroMemory( tmpinterp, sizeof( char ) * 20 );
	ret = Read_Str( &keybuf, "interp ", tmpinterp, 20 );
	if( ret ){
		DbgOut( "GPFile : ReadGPKey : Read_Str interp error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int chkinterp;
	int interp = INTERPOLATION_SLERP;
	for( chkinterp = INTERPOLATION_SLERP; chkinterp <= INTERPOLATION_SQUAD; chkinterp++ ){
		int cmp;
		cmp = strcmp( tmpinterp, &(strinterp[chkinterp][0]) );
		if( cmp == 0 ){
			interp = chkinterp;
			break;
		}
	}

	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	ret = Read_Vec3( &keybuf, "pos ", &pos );
	if( ret ){
		DbgOut( "GPFile : ReadGPKey : Read_Vec3 pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );
	ret = Read_Vec3( &keybuf, "rot ", &rot );
	if( ret ){
		DbgOut( "GPFile : ReadGPKey : Read_Vec3 rot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char tmpongmode[20];
	ZeroMemory( tmpongmode, sizeof( char ) * 20 );
	ret = Read_Str( &keybuf, "ongmode ", tmpongmode, 20 );
	if( ret ){
		DbgOut( "GPFile : ReadGPKey : Read_Str ongmode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int chkongmode;
	int ongmode = GROUND_NONE;
	for( chkongmode = GROUND_NONE; chkongmode <= GROUND_ON; chkongmode++ ){
		int cmp;
		cmp = strcmp( tmpongmode, &(strongmode[chkongmode][0]) );
		if( cmp == 0 ){
			ongmode = chkongmode;
			break;
		}
	}

	float rayy = 5000.0f;
	ret = Read_Float( &keybuf, "rayy ", &rayy );
	if( ret ){
		DbgOut( "GPFile : ReadGPKey : Read_Float rayy error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float rayleng = 8000.0f;
	ret = Read_Float( &keybuf, "rayleng ", &rayleng );
	if( ret ){
		DbgOut( "GPFile : ReadGPKey : Read_Float rayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float offsety = 10.0f;
	ret = Read_Float( &keybuf, "offsety ", &offsety );
	if( ret ){
		DbgOut( "GPFile : ReadGPKey : Read_Float offsety error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	GPELEM gpe;
	ZeroMemory( &gpe, sizeof( GPELEM ) );
	gpe.pos = pos;
	gpe.rot = rot;
	gpe.ongmode = ongmode;
	gpe.rayy = rayy;
	gpe.rayleng = rayleng;
	gpe.offsety = offsety;

	gpe.e3dpos = pos;//!!!!!!
	gpe.e3drot = rot;//!!!!!!

	if( frameflag == 0 ){
		ret = m_anim->SetGPKey( frameno, gpe, interp );
		if( ret ){
			DbgOut( "GPFile : ReadGPKey : ca SetGPKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		( m_anim->m_framedata + frameno )->m_frameno = frameno;
		( m_anim->m_framedata + frameno )->m_interp = interp;
		( m_anim->m_framedata + frameno )->m_gpe = gpe;

		CGPKey* pgpk = 0;
		ret = m_anim->ExistGPKey( frameno, &pgpk );
		_ASSERT( !ret );
		if( pgpk ){
			( m_anim->m_framedata + frameno )->m_keyflag = 1;
		}else{
			( m_anim->m_framedata + frameno )->m_keyflag = 0;
		}
	}

	int nextpos;
	nextpos = chkendpos + (int)strlen( strgpkeyend );
	animbuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return 0;
}



int CGPFile::DeleteAnim( CGPAnimHandler* srcgpah )
{
	int ret;
	if( m_anim ){
		ret = srcgpah->DestroyAnim( m_anim->m_motkind );
		_ASSERT( !ret );
		m_anim = 0;
	}
	return 0;
}

int CGPFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
int CGPFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CGPFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
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



int CGPFile::Read_Int( GPBUF* gpbuf, char* srcpat, int* dstint )
{
	int ret;
	char* findpat;
	findpat = strstr( gpbuf->buf + gpbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "GPFile : Read_Int : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (gpbuf->buf + gpbuf->bufleng) ){
			findend = 1;
			_ASSERT( 0 );
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "GPFile : Read_Int : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "GPFile : Read_Int : %s GetInt error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CGPFile::Read_Float( GPBUF* gpbuf, char* srcpat, float* dstfloat )
{
	int ret;
	char* findpat;
	findpat = strstr( gpbuf->buf + gpbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "GPFile : Read_Float : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (gpbuf->buf + gpbuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "GPFile : Read_Float : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "GPFile : Read_Int : %s GetFloat error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CGPFile::Read_Vec3( GPBUF* gpbuf, char* srcpat, D3DXVECTOR3* dstvec )
{
	int ret;
	char* findpat;
	findpat = strstr( gpbuf->buf + gpbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "GPFile : Read_Vec3 : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (gpbuf->buf + gpbuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "GPFile : Read_Vec3 : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	float xval = 0.0f;
	float yval = 0.0f;
	float zval = 0.0f;

	int srcpos = 0;
	int stepnum = 0;
	ret = GetFloat( &xval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "GPFile : Read_Vec3 : %s GetFloat X error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "GPFile : Read_Vec3 : %s GetFloat Y error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "GPFile : Read_Vec3 : %s GetFloat Z error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	dstvec->x = xval;
	dstvec->y = yval;
	dstvec->z = zval;


	return 0;
}
int CGPFile::Read_Str( GPBUF* gpbuf, char* srcpat, char* dststr, int arrayleng )
{
	int ret;
	char* findpat;
	findpat = strstr( gpbuf->buf + gpbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "GPFile : Read_Str : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (gpbuf->buf + gpbuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "GPFile : Read_Str : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "GPFile : Read_Str : %s GetName error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




int CGPFile::LoadGPFile( char* filename, CGPAnimHandler* srcgpah, int* animnoptr )
{
	int ret;

	if( !srcgpah ){
		_ASSERT( 0 );
		return 1;
	}
	m_anim = 0;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "GPFile : LoadGPFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	ret = SetBuffer();
	if( ret ){
		DbgOut( "GPFile : LoadGPFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CheckFileVersion( &m_loadversion );
	if( ret || (m_loadversion == 0) ){
		DbgOut( "GPFile : LoadGPFile : CheckFileVersion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadAnimInfo( srcgpah );
	if( ret ){
		DbgOut( "GPFile : LoadGPFile : ReadAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadGPAnim();
	if( ret ){
		DbgOut( "GPFile : LoadGPFile : ReadGPAnim error !!!\n" );
		_ASSERT( 0 );
		DeleteAnim( srcgpah );
		return 1;
	}

	ret = ReadGPFullFrame();
	if( ret ){
		DbgOut( "GPFile : LoadGPFile : ReadGPFullFrame error !!!\n" );
		_ASSERT( 0 );
		DeleteAnim( srcgpah );
		return 1;
	}

	_ASSERT( m_anim );
	*animnoptr = m_anim->m_motkind;

	return 0;
}

