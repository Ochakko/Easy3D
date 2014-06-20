#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include "CameraFile.h"

#include <CameraAnimHandler.h>
#include <CameraAnim.h>
#include <CameraKey.h>
#include <CameraSWKey.h>

#include <crtdbg.h>


enum {
	CAMFILE_WRITE,
	CAMFILE_LOAD,
	CAMFILE_MAX
};

static char strheader[256] = "RokDeBone2 Camera File ver1001 type0\r\n";
static char strheader2[256] = "RokDeBone2 Camera File ver1002 type0\r\n";
static char strheader3[256] = "RokDeBone2 Camera File ver1003 type0\r\n";

static char stranimstart[256] = "#AnimInfo Start\r\n";
static char stranimend[256] = "#AnimInfo End\r\n";

static char strcamstart[3][256] = {
	"#Camera0 Start\r\n",
	"#Camera1 Start\r\n",
	"#Camera2 Start\r\n"
};
static char strcamend[3][256] = {
	"#Camera0 End\r\n",
	"#Camera1 End\r\n",
	"#Camera2 End\r\n"
};

static char strcamkeystart[256] = "#CameraKey Start\r\n";
static char strcamkeyend[256] = "#CameraKey End\r\n";

static char strswstart[256] = "#CameraSwitch Start\r\n";
static char strswend[256] = "#CameraSwitch End\r\n";

static char strswkeystart[256] = "#SwitchKey Start\r\n";
static char strswkeyend[256] = "#SwitchKey End\r\n";

static char strendfile[256] = "#EndOfFile\r\n";

static char stranimtype[MOTIONTYPEMAX][20] = {
	"CLAMP", "STOP", "CLAMP", "ROUND", "INV", "JUMP"
};

static char strinterp[INTERPOLATION_MAX][20] = {
	"LINEAR", "SPLINE"
};

static char strlooktype[CAML_MAX][20] = {
	"NORMAL", "LOOKONECE", "LOCK"
};

CCameraFile::CCameraFile()
{
	InitParams();
}
CCameraFile::~CCameraFile()
{
	DestroyObjs();
}

int CCameraFile::InitParams()
{
/***
int m_mode;
HANDLE m_hfile;
CAMBUF m_cambuf;
char m_line[ CAMLINELEN ];
CCameraAnim* m_anim;
***/
	m_mode = CAMFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_cambuf.buf = 0;
	m_cambuf.bufleng = 0;
	m_cambuf.pos = 0;
	m_cambuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * CAMLINELEN );
	m_anim = 0;
	m_loadversion = 0;

	return 0;
}
int CCameraFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == CAMFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_cambuf.buf ){
		free( m_cambuf.buf );
		m_cambuf.buf = 0;
	}
	m_cambuf.bufleng = 0;
	m_cambuf.pos = 0;
	m_cambuf.isend = 0;

	return 0;
}

int CCameraFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[CAMLINELEN];
			
	ZeroMemory( outchar, CAMLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, CAMLINELEN, lpFormat, Marker );
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

int CCameraFile::WriteCameraFile( char* filename, CCameraAnim* srcanim )
{
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CameraFile : WriteCameraFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_anim = srcanim;
	if( !m_anim ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = Write2File( strheader3 );
	_ASSERT( !ret );

	ret = WriteAnimInfo();
	if( ret ){
		DbgOut( "CameraFile : WriteCameraFile : WriteAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int camno;
	for( camno = 0; camno < 3; camno++ ){
		ret = WriteCameraAnim( camno );
		if( ret ){
			DbgOut( "CameraFile : WriteCameraFile : WriteCameraAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = WriteSWAnim();
	if( ret ){
		DbgOut( "CameraFile : WriteCameraFile : WriteSWAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( strendfile );
	_ASSERT( !ret );

	return 0;
}

int CCameraFile::WriteAnimInfo()
{
/***
#AnimInfo Start
	name "camera_1"
	frameleng 60
	animtype "CLAMP"		//"STOP", "CLAMP", "ROUND", "INV", "JUMP"
	animstep 1
	motjump 0	
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


	ret = Write2File( stranimend );
	_ASSERT( !ret );

	return 0;
}
int CCameraFile::WriteCameraAnim( int camno )
{
	int ret;

	ret = Write2File( &(strcamstart[camno][0]) );
	_ASSERT( !ret );

	int keynum = 0;
	ret = m_anim->GetCameraKeyframeNoRange( camno, 0, m_anim->m_maxframe, 0, 0, &keynum );
	if( ret ){
		DbgOut( "CameraFile : WriteCameraAnim : ca GetCameraKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( "\tkeynum %d\r\n", keynum );
	_ASSERT( !ret );

	if( keynum > 0 ){
		int* keynoarray;
		keynoarray = (int*)malloc( sizeof( int ) * keynum );
		if( !keynoarray ){
			DbgOut( "CameraFile : WriteCameraAnim : keynoarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( keynoarray, sizeof( int ) * keynum );

		int getnum = 0;
		ret = m_anim->GetCameraKeyframeNoRange( camno, 0, m_anim->m_maxframe, keynoarray, keynum, &getnum );
		if( ret || (getnum != keynum) ){
			DbgOut( "CameraFile : WriteCameraAnim : ca GetCameraKeyframeNoRange 1 error !!!\n" );
			_ASSERT( 0 );
			free( keynoarray );
			return 1;
		}

		int keyno;
		for( keyno = 0; keyno < keynum; keyno++ ){
			int curframeno;
			curframeno = *( keynoarray + keyno );

			ret = Write2File( "\t%s", strcamkeystart );
			_ASSERT( !ret );

			CCameraKey* ckptr = 0;
			ret = m_anim->ExistCameraKey( camno, curframeno, &ckptr );
			if( ret || !ckptr ){
				DbgOut( "CameraFile : WriteCameraAnim : ca ExistCameraKey error !!!\n" );
				_ASSERT( 0 );
				free( keynoarray );
				return 1;
			}

			ret = Write2File( "\t\tframeno %d\r\n", ckptr->m_frameno );
			_ASSERT( !ret );


			int interpindex, fillupup;
			int divinterp;
			divinterp = ckptr->m_interp / 100;
			if( divinterp == 1 ){
				fillupup = 1;
				interpindex = ckptr->m_interp - 100;
			}else{
				fillupup = 0;
				interpindex = ckptr->m_interp;
			}
			ret = Write2File( "\t\tinterp \"%s\"\r\n", &(strinterp[interpindex][0]) );
			_ASSERT( !ret );

			ret = Write2File( "\t\tCalcUp %d\r\n", fillupup );
			_ASSERT( !ret );

			int looktypeindex;
			if( (ckptr->m_celem.looktype >= CAML_NORMAL) && (ckptr->m_celem.looktype <= CAML_LOCK) ){
				looktypeindex = ckptr->m_celem.looktype;
			}else{
				looktypeindex = CAML_NORMAL;
			}
			ret = Write2File( "\t\tlooktype \"%s\"\r\n", &(strlooktype[looktypeindex][0]) );
			_ASSERT( !ret );

			ret = Write2File( "\t\tpos %f, %f, %f\r\n", ckptr->m_celem.pos.x, ckptr->m_celem.pos.y, ckptr->m_celem.pos.z );
			_ASSERT( !ret );

			ret = Write2File( "\t\ttarget %f, %f, %f\r\n", ckptr->m_celem.target.x, ckptr->m_celem.target.y, ckptr->m_celem.target.z );
			_ASSERT( !ret );

			ret = Write2File( "\t\tup %f, %f, %f\r\n", ckptr->m_celem.up.x, ckptr->m_celem.up.y, ckptr->m_celem.up.z );
			_ASSERT( !ret );

			ret = Write2File( "\t\tdist %f\r\n", ckptr->m_celem.dist );
			_ASSERT( !ret );

			ret = Write2File( "\t\thsid %d\r\n", ckptr->m_celem.hsid );
			_ASSERT( !ret );

			ret = Write2File( "\t\tboneno %d\r\n", ckptr->m_celem.boneno );
			_ASSERT( !ret );

			ret = Write2File( "\t\tnearz %f\r\n", ckptr->m_celem.nearz );
			_ASSERT( !ret );

			ret = Write2File( "\t\tfarz %f\r\n", ckptr->m_celem.farz );
			_ASSERT( !ret );

			ret = Write2File( "\t\tfov %f\r\n", ckptr->m_celem.fov );
			_ASSERT( !ret );

			ret = Write2File( "\t\torthoflag %d\r\n", ckptr->m_celem.ortho );
			_ASSERT( !ret );

			ret = Write2File( "\t\torthosize %f\r\n", ckptr->m_celem.orthosize );
			_ASSERT( !ret );

			ret = Write2File( "\t%s", strcamkeyend );
			_ASSERT( !ret );
		}
		free( keynoarray );
	}
	ret = Write2File( &(strcamend[camno][0]) );
	_ASSERT( !ret );
/***
#Camera0 Start
	keynum	3
	#CameraKey Start
		frameno 0
		interp "LINEAR"		//"LINEAR", "SPLINE"
		looktype "NORMAL"	//"NORMAL", "LOOKONECE", "LOCK"
		pos 0.0, 0.0, 0.0
		target 0.0, 0.0, -5000.0
		up 0.0, 1.0, 0.0
		dist 5000.0
		hsid -1
		boneno -1
		nearz 1000.0
		farz 40000.0
		fov 45.0		
	#CameraKey End

	#CameraKey Start
		...
	#CameraKey End

	#CameraKey Start
		...
	#CameraKey End
#Camera0 End
***/
	return 0;
}
int CCameraFile::WriteSWAnim()
{
	int ret;

	ret = Write2File( strswstart );	
	_ASSERT( !ret );

	int keynum = 0;
	ret = m_anim->GetSWKeyframeNoRange( 0, m_anim->m_maxframe, 0, 0, &keynum );
	if( ret ){
		DbgOut( "CameraFile : WriteSWAnim : ca GetSWKeyframeNoRange 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( "\tkeynum %d\r\n", keynum );
	_ASSERT( !ret );

	if( keynum > 0 ){
		int* keynoarray;
		keynoarray = (int*)malloc( sizeof( int ) * keynum );
		if( !keynoarray ){
			DbgOut( "CameraFile : WriteSWAnim : keynoarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( keynoarray, sizeof( int ) * keynum );

		int getnum = 0;
		ret = m_anim->GetSWKeyframeNoRange( 0, m_anim->m_maxframe, keynoarray, keynum, &getnum );
		if( ret || (getnum != keynum) ){
			DbgOut( "CameraFile : WriteSWAnim : ca GetSWKeyframeNoRange 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int keyno;
		for( keyno = 0; keyno < keynum; keyno++ ){
			int curframeno;
			curframeno = *( keynoarray + keyno );

			CCameraSWKey* cswkptr = 0;
			ret = m_anim->ExistSWKey( curframeno, &cswkptr );
			if( ret || !cswkptr ){
				DbgOut( "CameraFile : WriteSWAnim : ca ExistSWKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = Write2File( "\t%s", strswkeystart );
			_ASSERT( !ret );

			ret = Write2File( "\t\tframeno %d\r\n", cswkptr->m_frameno );
			_ASSERT( !ret );

			ret = Write2File( "\t\tcamerano %d\r\n", cswkptr->m_camerano );
			_ASSERT( !ret );

			ret = Write2File( "\t%s", strswkeyend );
			_ASSERT( !ret );
		}

		free( keynoarray );
	}

	ret = Write2File( strswend );
	_ASSERT( !ret );
/***
#CameraSwitch Start
	keynum 2
	#SwitchKey Start
		frameno 0
		camerano 0
	#SwitchKey End

	#SwitchKey Start
		frameno 20
		camerano 1
	#SwitchKey End
#CameraSwitch End
***/
	return 0;
}



int CCameraFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "CameraFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "CameraFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "CameraFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "CameraFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_cambuf.buf = newbuf;
	m_cambuf.pos = 0;
	m_cambuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, strendfile );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_cambuf.bufleng = validleng;

	return 0;
}

int CCameraFile::CheckFileVersion( int* verptr )
{


	if( m_cambuf.bufleng <= 0 ){
		DbgOut( "camfile : CheckFileVersion : cambuf.bufleng error !!! %d\r\n", m_cambuf.bufleng );
		_ASSERT( 0 );
		return 1;
	}

	char* headerptr;
	headerptr = strstr( m_cambuf.buf, &(strheader[0]) );

	char* headerptr2;
	headerptr2 = strstr( m_cambuf.buf, &(strheader2[0]) );

	char* headerptr3;
	headerptr3 = strstr( m_cambuf.buf, &(strheader3[0]) );


	if( headerptr ){
		*verptr = 1001;
	}else if( headerptr2 ){
		*verptr = 1002;
	}else if( headerptr3 ){
		*verptr = 1003;
	}else{
		*verptr = 0;
	}

	return 0;
}
int CCameraFile::ReadAnimInfo( CCameraAnimHandler* srccah )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_cambuf.buf, stranimstart );
	endptr = strstr( m_cambuf.buf, stranimend );

	if( !startptr || !endptr ){
		DbgOut( "CameraFile : ReadAnimInfo : section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_cambuf.buf );
	if( (chkendpos >= (int)m_cambuf.bufleng) || (endptr < startptr) ){
		DbgOut( "CameraFile : ReadAnimInfo : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;

	char strname[256];
	ZeroMemory( strname, sizeof( char ) * 256 );
	ret = Read_Str( &infobuf, "name ", strname, 256 );
	if( ret ){
		DbgOut( "CameraFile : ReadAnimInfo : Read_Str name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int frameleng = 0;
	ret = Read_Int( &infobuf, "frameleng ", &frameleng );
	if( ret || (frameleng <= 0) ){
		DbgOut( "CameraFile : ReadAnimInfo : Read_Int frameleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char strtype[20];
	ZeroMemory( strtype, sizeof( char ) * 20 );
	ret = Read_Str( &infobuf, "animtype ", strtype, 20 );
	if( ret ){
		DbgOut( "CameraFile : ReadAnimInfo : Read_Str animtype error !!!\n" );
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
		DbgOut( "CameraFile : ReadAnimInfo : Read_Int animstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int motjump = 0;
	ret = Read_Int( &infobuf, "motjump ", &motjump );
	if( ret || (motjump < 0) || (motjump >= frameleng) ){
		DbgOut( "CameraFile : ReadAnimInfo : Read_Int motjump error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_anim = srccah->AddAnim( strname, animtype, frameleng, motjump );
	if( !m_anim ){
		DbgOut( "CameraFile : ReadAnimInfo : cah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_anim->m_animstep = animstep;

/***
#AnimInfo Start
	name "camera_1"
	frameleng 60
	animtype "CLAMP"		//"STOP", "CLAMP", "ROUND", "INV", "JUMP"
	animstep 1
	motjump 0	
#AnimInfo End
***/
	return 0;
}
int CCameraFile::ReadCameraAnim( int camno )
{
	int ret;
	char* startpat;
	char* endpat;
	startpat = &(strcamstart[camno][0]);
	endpat = &(strcamend[camno][0]);

	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( m_cambuf.buf, startpat );
	endptr = strstr( m_cambuf.buf, endpat );

	if( !startptr || !endptr ){
		DbgOut( "CameraFile : ReadCameraAnim : camera %d anim section pattern not found error !!!\n", camno );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_cambuf.buf );
	if( (chkendpos >= (int)m_cambuf.bufleng) || (endptr < startptr) ){
		DbgOut( "CameraFile : ReadCameraAnim %d : endmark error !!!\n", camno );
		_ASSERT( 0 );
		return 1;
	}

	CAMBUF animbuf;
	animbuf.buf = startptr;
	animbuf.bufleng = (int)( endptr - startptr );
	animbuf.pos = 0;
	animbuf.isend = 0;

	int keynum = 0;
	ret = Read_Int( &animbuf, "keynum ", &keynum );
	if( ret || (keynum < 0) ){
		DbgOut( "CameraFile : ReadCameraAnim : Read_Int keynum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int keyno;
	for( keyno = 0; keyno < keynum; keyno++ ){
		ret = ReadCameraKey( &animbuf, camno );
		if( ret ){
			DbgOut( "CameraFile : ReadCameraAnim : ReadCameraKey %d error !!!\n", keyno );
			_ASSERT( 0 );
			return 1;
		}
	}

/***
#Camera0 Start
	keynum	3
	#CameraKey Start
		frameno 0
		interp "LINEAR"		//"LINEAR", "SPLINE"
		looktype "NORMAL"	//"NORMAL", "LOOKONECE", "LOCK"
		pos 0.0, 0.0, 0.0
		target 0.0, 0.0, -5000.0
		up 0.0, 1.0, 0.0
		dist 5000.0
		hsid -1
		boneno -1
		nearz 1000.0
		farz 40000.0
		fov 45.0		
	#CameraKey End

	#CameraKey Start
		...
	#CameraKey End

	#CameraKey Start
		...
	#CameraKey End
#Camera0 End
***/
	return 0;
}
int CCameraFile::ReadCameraKey( CAMBUF* animbuf, int camno )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( animbuf->buf + animbuf->pos, strcamkeystart );
	endptr = strstr( animbuf->buf + animbuf->pos, strcamkeyend );

	if( !startptr || !endptr ){
		DbgOut( "CameraFile : ReadCameraKey : key section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - animbuf->buf );
	if( (chkendpos >= (int)animbuf->bufleng) || (endptr < startptr) ){
		DbgOut( "CameraFile : ReadCameraKey : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMBUF keybuf;
	keybuf.buf = startptr;
	keybuf.bufleng = (int)( endptr - startptr );
	keybuf.pos = 0;
	keybuf.isend = 0;
	
	int frameno = 0;
	ret = Read_Int( &keybuf, "frameno ", &frameno );
	if( ret || (frameno < 0) || (frameno > m_anim->m_maxframe) ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Int frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char tmpinterp[20];
	ZeroMemory( tmpinterp, sizeof( char ) * 20 );
	ret = Read_Str( &keybuf, "interp ", tmpinterp, 20 );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Str interp error !!!\n" );
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

	int tmpcalcup = 0;
	int calcup = 0;
	ret = Read_Int( &keybuf, "CalcUp ", &tmpcalcup );
	if( ret ){
		calcup = 0;
	}else{
		if( tmpcalcup == 0 ){
			calcup = 0;
		}else{
			calcup = 1;
		}
	}
	if( calcup == 1 ){
		interp += 100;
	}


	char tmplooktype[20];
	ZeroMemory( tmplooktype, sizeof( char ) * 20 );
	ret = Read_Str( &keybuf, "looktype ", tmplooktype, 20 );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Str looktype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int chklooktype;
	int looktype = CAML_NORMAL;
	for( chklooktype = CAML_NORMAL; chklooktype <= CAML_LOCK; chklooktype++ ){
		int cmp;
		cmp = strcmp( tmplooktype, &(strlooktype[chklooktype][0]) );
		if( cmp == 0 ){
			looktype = chklooktype;
			break;
		}
	}

	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	ret = Read_Vec3( &keybuf, "pos ", &pos );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Vec3 pos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 target( 0.0f, 0.0f, -5000.0f );
	ret = Read_Vec3( &keybuf, "target ", &target );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Vec3 target error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
	ret = Read_Vec3( &keybuf, "up ", &up );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Vec3 up error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float dist = 5000.0f;
	ret = Read_Float( &keybuf, "dist ", &dist );
	if( ret || (dist < 0.0f) ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Float dist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int hsid = -1;
	ret = Read_Int( &keybuf, "hsid ", &hsid );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Int hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int boneno = -1;
	ret = Read_Int( &keybuf, "boneno ", &boneno );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Int boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float nearz;
	ret = Read_Float( &keybuf, "nearz ", &nearz );
	if( ret || (nearz < 0.0f) ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Float nearz error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float farz;
	ret = Read_Float( &keybuf, "farz ", &farz );
	if( ret || (farz <= 0.0f) ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Float farz error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fov;
	ret = Read_Float( &keybuf, "fov ", &fov );
	if( ret || (fov < 30.0f) || (fov > 80.0f) ){
		DbgOut( "CameraFile : ReadCameraKey : Read_Float fov error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ortho;
	float orthosize;
	if( m_loadversion < 1002 ){
		ortho = 0;
		orthosize = 3000.0f;
	}else{
		ret = Read_Int( &keybuf, "orthoflag ", &ortho );
		if( ret ){
			DbgOut( "CameraFile : ReadCameraKey : Read_Int ortho error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = Read_Float( &keybuf, "orthosize ", &orthosize );
		if( ret ){
			DbgOut( "CameraFile : ReadCameraKey : Read_Float orthosize error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( orthosize < 500.0f ){
			orthosize = 500.0f;
		}
	}


	CAMERAELEM setce;
	ZeroMemory( &setce, sizeof( CAMERAELEM ) );
	setce.looktype = looktype;
	setce.pos = pos;
	setce.target = target;
	setce.up = up;
	setce.dist = dist;
	setce.hsid = hsid;
	setce.boneno = boneno;
	setce.nearz = nearz;
	setce.farz = farz;
	setce.fov = fov;
	setce.ortho = ortho;
	setce.orthosize = orthosize;

	ret = m_anim->SetCameraKey( camno, frameno, setce, interp );
	if( ret ){
		DbgOut( "CameraFile : ReadCameraKey : ca SetCameraKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nextpos;
	nextpos = chkendpos + (int)strlen( strcamkeyend );
	animbuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/***
	#CameraKey Start
		frameno 0
		interp "LINEAR"		//"LINEAR", "SPLINE"
		looktype "NORMAL"	//"NORMAL", "LOOKONECE", "LOCK"
		pos 0.0, 0.0, 0.0
		target 0.0, 0.0, -5000.0
		up 0.0, 1.0, 0.0
		dist 5000.0
		hsid -1
		boneno -1
		nearz 1000.0
		farz 40000.0
		fov 45.0		
	#CameraKey End
***/
	return 0;
}

int CCameraFile::ReadSWAnim()
{
	int ret;

	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( m_cambuf.buf, strswstart );
	endptr = strstr( m_cambuf.buf, strswend );

	if( !startptr || !endptr ){
		DbgOut( "CameraFile : ReadSWAnim : sw section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_cambuf.buf );
	if( (chkendpos >= (int)m_cambuf.bufleng) || (endptr < startptr) ){
		DbgOut( "CameraFile : ReadSWAnim : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMBUF swbuf;
	swbuf.buf = startptr;
	swbuf.bufleng = (int)( endptr - startptr );
	swbuf.pos = 0;
	swbuf.isend = 0;

	int keynum = 0;
	ret = Read_Int( &swbuf, "keynum ", &keynum );
	if( ret || (keynum < 0) ){
		DbgOut( "CameraFile : ReadSWAnim : Read_Int keynum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int keyno;
	for( keyno = 0; keyno < keynum; keyno++ ){
		ret = ReadSWKey( &swbuf );
		if( ret ){
			DbgOut( "CameraFile : ReadSWAnim : ReadSWKey %d error !!!\n", keyno );
			_ASSERT( 0 );
			return 1;
		}
	}

/***
#CameraSwitch Start
	keynum 2
	#SwitchKey Start
		frameno 0
		camerano 0
	#SwitchKey End

	#SwitchKey Start
		frameno 20
		camerano 1
	#SwitchKey End
#CameraSwitch End
***/
	return 0;
}

int CCameraFile::ReadSWKey( CAMBUF* animbuf )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( animbuf->buf + animbuf->pos, strswkeystart );
	endptr = strstr( animbuf->buf + animbuf->pos, strswkeyend );

	if( !startptr || !endptr ){
		DbgOut( "CameraFile : ReadSWKey : key section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - animbuf->buf );
	if( (chkendpos >= (int)animbuf->bufleng) || (endptr < startptr) ){
		DbgOut( "CameraFile : ReadSWKey : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMBUF keybuf;
	keybuf.buf = startptr;
	keybuf.bufleng = (int)( endptr - startptr );
	keybuf.pos = 0;
	keybuf.isend = 0;
	
	int frameno = 0;
	ret = Read_Int( &keybuf, "frameno ", &frameno );
	if( ret || (frameno < 0) || (frameno > m_anim->m_maxframe) ){
		DbgOut( "CameraFile : ReadSWKey : Read_Int frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int camerano;
	ret = Read_Int( &keybuf, "camerano ", &camerano );
	if( ret || (camerano < 0) || (camerano >= CAMERANUMMAX) ){
		DbgOut( "CameraFile : ReadSWKey : Read_Int camerano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_anim->SetSWKey( frameno, camerano );
	if( ret ){
		DbgOut( "CameraFile : ReadSWKey : ca SetSWKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nextpos;
	nextpos = chkendpos + (int)strlen( strcamkeyend );
	animbuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/***
	#SwitchKey Start
		frameno 0
		camerano 0
	#SwitchKey End
***/
	return 0;
}

int CCameraFile::DeleteAnim( CCameraAnimHandler* srccah )
{
	int ret;
	if( m_anim ){
		ret = srccah->DestroyAnim( m_anim->m_motkind );
		_ASSERT( !ret );
		m_anim = 0;
	}
	return 0;
}

int CCameraFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
int CCameraFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CCameraFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
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



int CCameraFile::Read_Int( CAMBUF* cambuf, char* srcpat, int* dstint )
{
	int ret;
	char* findpat;
	findpat = strstr( cambuf->buf + cambuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "CameraFile : Read_Int : %s pattern not found error !!!\n", srcpat );
//		_ASSERT( 0 );
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
		if( (srcchar + srcleng) >= (cambuf->buf + cambuf->bufleng) ){
			findend = 1;
			_ASSERT( 0 );
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "CameraFile : Read_Int : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "CameraFile : Read_Int : %s GetInt error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CCameraFile::Read_Float( CAMBUF* cambuf, char* srcpat, float* dstfloat )
{
	int ret;
	char* findpat;
	findpat = strstr( cambuf->buf + cambuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "CameraFile : Read_Float : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (cambuf->buf + cambuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "CameraFile : Read_Float : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "CameraFile : Read_Int : %s GetFloat error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CCameraFile::Read_Vec3( CAMBUF* cambuf, char* srcpat, D3DXVECTOR3* dstvec )
{
	int ret;
	char* findpat;
	findpat = strstr( cambuf->buf + cambuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "CameraFile : Read_Vec3 : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (cambuf->buf + cambuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "CameraFile : Read_Vec3 : %s value not found error !!!\n", srcpat );
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
		DbgOut( "CameraFile : Read_Vec3 : %s GetFloat X error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "CameraFile : Read_Vec3 : %s GetFloat Y error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "CameraFile : Read_Vec3 : %s GetFloat Z error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	dstvec->x = xval;
	dstvec->y = yval;
	dstvec->z = zval;


	return 0;
}
int CCameraFile::Read_Str( CAMBUF* cambuf, char* srcpat, char* dststr, int arrayleng )
{
	int ret;
	char* findpat;
	findpat = strstr( cambuf->buf + cambuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "CameraFile : Read_Str : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (cambuf->buf + cambuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "CameraFile : Read_Str : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "CameraFile : Read_Str : %s GetName error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




int CCameraFile::LoadCameraFile( char* filename, CCameraAnimHandler* srccah, int* animnoptr )
{
	int ret;

	if( !srccah ){
		DbgOut( "camfile : LoadCameraFile : cah NULL %s error !!!\n", filename );
		_ASSERT( 0 );
		return 1;
	}
	m_anim = 0;


	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CameraFile : LoadCameraFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	ret = SetBuffer();
	if( ret ){
		DbgOut( "CameraFile : LoadCameraFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CheckFileVersion( &m_loadversion );
	if( ret || (m_loadversion == 0) ){
		DbgOut( "CameraFile : LoadCameraFile : CheckFileVersion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadAnimInfo( srccah );
	if( ret ){
		DbgOut( "CameraFile : LoadCameraFile : ReadAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int camno;
	for( camno = 0; camno < 3; camno++ ){
		ret = ReadCameraAnim( camno );
		if( ret ){
			DbgOut( "CameraFile : LoadCameraFile : ReadCameraAnim error !!!\n" );
			_ASSERT( 0 );
			DeleteAnim( srccah );
			return 1;
		}
	}

	ret = ReadSWAnim();
	if( ret ){
		DbgOut( "CameraFile : LoadCameraFile : ReadSWAnim error !!!\n" );
		_ASSERT( 0 );
		DeleteAnim( srccah );
		return 1;
	}

	
	if( !m_anim ){
		DbgOut( "camfile : LoadCameraFile : m_anim NULL error !!! %s\r\n", filename );
		_ASSERT( 0 );
		*animnoptr = -1;
		return 1;
	}
	*animnoptr = m_anim->m_motkind;

	return 0;
}

