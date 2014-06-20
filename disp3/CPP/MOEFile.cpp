#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include "MOEFile.h"

#include <MOEAnimHandler.h>
#include <MOEAnim.h>
#include <MOEKey.h>

#include <MOEEHelper.h>

#include <treehandler2.h>
#include <treeelem2.h>

#include <crtdbg.h>


enum {
	MOEFILE_WRITE,
	MOEFILE_LOAD,
	MOEFILE_MAX
};

static char strheader[256] = "RokDeBone2 MoaEventAnim File ver1001 type0\r\n";
static char strheader2[256] = "RokDeBone2 MoaEventAnim File ver1002 type0\r\n";

static char stranimstart[256] = "#AnimInfo Start\r\n";
static char stranimend[256] = "#AnimInfo End\r\n";

static char strmoestart[256] = "#MOEAnim Start\r\n";
static char strmoeend[256] = "#MOEAnim End\r\n";

static char strmoekeystart[256] = "#MOEKey Start\r\n";
static char strmoekeyend[256] = "#MOEKey End\r\n";

static char strmoeelemstart[256] = "#MOEElem Start\r\n";
static char strmoeelemend[256] = "#MOEElem End\r\n";

static char strendfile[256] = "#EndOfFile\r\n";

static char stranimtype[MOTIONTYPEMAX][20] = {
	"CLAMP", "STOP", "CLAMP", "ROUND", "INV", "JUMP"
};



CMOEFile::CMOEFile()
{
	InitParams();
}
CMOEFile::~CMOEFile()
{
	DestroyObjs();
}

int CMOEFile::InitParams()
{
/***
int m_mode;
HANDLE m_hfile;
MOEBUF m_moebuf;
char m_line[ MOELINELEN ];
CMOEAnim* m_anim;
***/
	m_mode = MOEFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_moebuf.buf = 0;
	m_moebuf.bufleng = 0;
	m_moebuf.pos = 0;
	m_moebuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * MOELINELEN );
	m_anim = 0;
	m_loadversion = 0;
	m_lpsh = 0;
	m_lpth = 0;
	return 0;
}
int CMOEFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == MOEFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_moebuf.buf ){
		free( m_moebuf.buf );
		m_moebuf.buf = 0;
	}
	m_moebuf.bufleng = 0;
	m_moebuf.pos = 0;
	m_moebuf.isend = 0;

	return 0;
}

int CMOEFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[MOELINELEN];
			
	ZeroMemory( outchar, MOELINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, MOELINELEN, lpFormat, Marker );
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

int CMOEFile::WriteMOEFile( CTreeHandler2* lpth, CShdHandler* lpsh, char* filename, CMOEAnim* srcanim )
{
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "MOEFile : WriteMOEFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_anim = srcanim;
	if( !m_anim ){
		_ASSERT( 0 );
		return 1;
	}
	m_lpsh = lpsh;
	m_lpth = lpth;

	int ret;
	ret = Write2File( strheader2 );
	_ASSERT( !ret );

	ret = WriteAnimInfo();
	if( ret ){
		DbgOut( "MOEFile : WriteMOEFile : WriteAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteMOEAnim();
	if( ret ){
		DbgOut( "MOEFile : WriteMOEFile : WriteMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	ret = Write2File( strendfile );
	_ASSERT( !ret );

	return 0;
}

int CMOEFile::WriteAnimInfo()
{
/***
#AnimInfo Start
	name "moeera_1"
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

	ret = Write2File( stranimend );
	_ASSERT( !ret );

	return 0;
}
int CMOEFile::WriteMOEAnim()
{
	int ret;

	ret = Write2File( &(strmoestart[0]) );
	_ASSERT( !ret );

	int keynum = 0;
	ret = m_anim->GetMOEKeyframeNoRange( 0, m_anim->m_maxframe, 0, 0, &keynum );
	if( ret ){
		DbgOut( "MOEFile : WriteMOEAnim : ca GetMOEKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( "\tkeynum %d\r\n", keynum );
	_ASSERT( !ret );

	if( keynum > 0 ){
		int* keynoarray;
		keynoarray = (int*)malloc( sizeof( int ) * keynum );
		if( !keynoarray ){
			DbgOut( "MOEFile : WriteMOEAnim : keynoarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( keynoarray, sizeof( int ) * keynum );

		int getnum = 0;
		ret = m_anim->GetMOEKeyframeNoRange( 0, m_anim->m_maxframe, keynoarray, keynum, &getnum );
		if( ret || (getnum != keynum) ){
			DbgOut( "MOEFile : WriteMOEAnim : ca GetMOEKeyframeNoRange 1 error !!!\n" );
			_ASSERT( 0 );
			free( keynoarray );
			return 1;
		}

		int keyno;
		for( keyno = 0; keyno < keynum; keyno++ ){
			int curframeno;
			curframeno = *( keynoarray + keyno );

			ret = Write2File( "\t%s", strmoekeystart );
			_ASSERT( !ret );

			CMOEKey* moekptr = 0;
			ret = m_anim->ExistMOEKey( curframeno, &moekptr );
			if( ret || !moekptr ){
				DbgOut( "MOEFile : WriteMOEAnim : ca ExistMOEKey error !!!\n" );
				_ASSERT( 0 );
				free( keynoarray );
				return 1;
			}

			ret = Write2File( "\t\tframeno %d\r\n", moekptr->m_frameno );
			_ASSERT( !ret );

			ret = Write2File( "\t\telemnum %d\r\n", moekptr->m_moeenum );
			_ASSERT( !ret );

			ret = Write2File( "\t\tidlingname \"%s\"\r\n", moekptr->m_idlingname );
			_ASSERT( !ret );

			ret = Write2File( "\t\tgoonflag %d\r\n", moekptr->m_goonflag );
			_ASSERT( !ret );


			int elemno;
			for( elemno = 0; elemno < moekptr->m_moeenum; elemno++ ){
				MOEELEM* curelem = moekptr->m_pmoee + elemno;

				ret = CheckAndDelInvalidMOEE( m_lpsh, curelem );
				if( ret ){
					DbgOut( "MOEFile : WriteMOEAnim : CheckAndDelInvalidMOEE error !!!\n" );
					_ASSERT( 0 );
					free( keynoarray );
					return 1;
				}

				ret = Write2File( "\t\t%s", strmoeelemstart );
				_ASSERT( !ret );
		
				ret = Write2File( "\t\t\teventno %d\r\n", curelem->eventno );
				_ASSERT( !ret );

				ret = Write2File( "\t\t\tlistnum %d\r\n", curelem->listnum );
				_ASSERT( !ret );
				int listno;
				for( listno = 0; listno < curelem->listnum; listno++ ){
					int seri = curelem->list[listno];
					if( seri < 0 ){
						ret = Write2File( "\t\t\tlist%d \"全てのボーン\"\r\n", listno );
						_ASSERT( !ret );
					}else{
						CTreeElem2* telem;
						telem = (*m_lpth)( seri );
						_ASSERT( telem );
						ret = Write2File( "\t\t\tlist%d \"%s\"\r\n", listno, telem->name );
						_ASSERT( !ret );
					}
				}


				ret = Write2File( "\t\t\tnotlistnum %d\r\n", curelem->notlistnum );
				_ASSERT( !ret );
				int notlistno;
				for( notlistno = 0; notlistno < curelem->notlistnum; notlistno++ ){
					int seri = curelem->notlist[notlistno];
					if( seri < 0 ){
						ret = Write2File( "\t\t\tnotlist%d \"全てのボーン\"\r\n", notlistno );
						_ASSERT( !ret );
					}else{
						CTreeElem2* telem;
						telem = (*m_lpth)( seri );
						_ASSERT( telem );
						ret = Write2File( "\t\t\tnotlist%d \"%s\"\r\n", notlistno, telem->name );
						_ASSERT( !ret );
					}
				}

				ret = Write2File( "\t\t%s", strmoeelemend );
				_ASSERT( !ret );
			}

			ret = Write2File( "\t%s", strmoekeyend );
			_ASSERT( !ret );
		}
		free( keynoarray );
	}
	ret = Write2File( &(strmoeend[0]) );
	_ASSERT( !ret );

	return 0;
}


int CMOEFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "MOEFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "MOEFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "MOEFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "MOEFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_moebuf.buf = newbuf;
	m_moebuf.pos = 0;
	m_moebuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, strendfile );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_moebuf.bufleng = validleng;

	return 0;
}

int CMOEFile::CheckFileVersion( int* verptr )
{

	char* headerptr = 0;
	char* headerptr2 = 0;
	headerptr = strstr( m_moebuf.buf, strheader );
	headerptr2 = strstr( m_moebuf.buf, strheader2 );
	if( headerptr ){
		*verptr = 1001;
	}else if( headerptr2 ){
		*verptr = 2001;
	}else{
		*verptr = 0;
	}

	return 0;
}

int CMOEFile::ReadAnimInfo( CMOEAnimHandler* srcmoeah )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;

	startptr = strstr( m_moebuf.buf, stranimstart );
	endptr = strstr( m_moebuf.buf, stranimend );

	if( !startptr || !endptr ){
		DbgOut( "MOEFile : ReadAnimInfo : section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_moebuf.buf );
	if( (chkendpos >= (int)m_moebuf.bufleng) || (endptr < startptr) ){
		DbgOut( "MOEFile : ReadAnimInfo : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MOEBUF infobuf;
	infobuf.buf = startptr;
	infobuf.bufleng = (int)( endptr - startptr );
	infobuf.pos = 0;
	infobuf.isend = 0;

	char strname[256];
	ZeroMemory( strname, sizeof( char ) * 256 );
	ret = Read_Str( &infobuf, "name ", strname, 256 );
	if( ret ){
		DbgOut( "MOEFile : ReadAnimInfo : Read_Str name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int frameleng = 0;
	ret = Read_Int( &infobuf, "frameleng ", &frameleng );
	if( ret || (frameleng <= 0) ){
		DbgOut( "MOEFile : ReadAnimInfo : Read_Int frameleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char strtype[20];
	ZeroMemory( strtype, sizeof( char ) * 20 );
	ret = Read_Str( &infobuf, "animtype ", strtype, 20 );
	if( ret ){
		DbgOut( "MOEFile : ReadAnimInfo : Read_Str animtype error !!!\n" );
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
		DbgOut( "MOEFile : ReadAnimInfo : Read_Int animstep error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int motjump = 0;
	ret = Read_Int( &infobuf, "motjump ", &motjump );
	if( ret || (motjump < 0) || (motjump >= frameleng) ){
		DbgOut( "MOEFile : ReadAnimInfo : Read_Int motjump error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_anim = srcmoeah->AddAnim( m_lpsh, strname, animtype, frameleng, motjump );
	if( !m_anim ){
		DbgOut( "MOEFile : ReadAnimInfo : moeah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_anim->m_animstep = animstep;

	return 0;
}


int CMOEFile::ReadMOEAnim()
{
	int ret;
	char* startpat;
	char* endpat;
	startpat = &(strmoestart[0]);
	endpat = &(strmoeend[0]);

	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( m_moebuf.buf, startpat );
	endptr = strstr( m_moebuf.buf, endpat );

	if( !startptr || !endptr ){
		DbgOut( "MOEFile : ReadMOEAnim : anim section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - m_moebuf.buf );
	if( (chkendpos >= (int)m_moebuf.bufleng) || (endptr < startptr) ){
		DbgOut( "MOEFile : ReadMOEAnim : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MOEBUF animbuf;
	animbuf.buf = startptr;
	animbuf.bufleng = (int)( endptr - startptr );
	animbuf.pos = 0;
	animbuf.isend = 0;

	int keynum = 0;
	ret = Read_Int( &animbuf, "keynum ", &keynum );
	if( ret || (keynum < 0) ){
		DbgOut( "MOEFile : ReadMOEAnim : Read_Int keynum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int keyno;
	for( keyno = 0; keyno < keynum; keyno++ ){
		ret = ReadMOEKey( &animbuf );
		if( ret ){
			DbgOut( "MOEFile : ReadMOEAnim : ReadMOEKey %d error !!!\n", keyno );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CMOEFile::ReadMOEKey( MOEBUF* animbuf )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( animbuf->buf + animbuf->pos, strmoekeystart );
	endptr = strstr( animbuf->buf + animbuf->pos, strmoekeyend );

	if( !startptr || !endptr ){
		DbgOut( "MOEFile : ReadMOEKey : key section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - animbuf->buf );
	if( (chkendpos >= (int)animbuf->bufleng) || (endptr < startptr) ){
		DbgOut( "MOEFile : ReadMOEKey : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MOEBUF keybuf;
	keybuf.buf = startptr;
	keybuf.bufleng = (int)( endptr - startptr );
	keybuf.pos = 0;
	keybuf.isend = 0;
	
	int frameno = 0;
	ret = Read_Int( &keybuf, "frameno ", &frameno );
	if( ret || (frameno < 0) || (frameno > m_anim->m_maxframe) ){
		DbgOut( "MOEFile : ReadMOEKey : Read_Int frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int elemnum = 0;
	ret = Read_Int( &keybuf, "elemnum ", &elemnum );
	if( ret || (elemnum < 0) ){
		DbgOut( "MOEFile : ReadMOEKey : Read_Int elemnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char idlingname[256];
	ZeroMemory( idlingname, sizeof( char ) * 256 );
	ret = Read_Str( &keybuf, "idlingname ", idlingname, 256 );
	if( ret ){
		DbgOut( "MOEFile : ReadMOEKey : Read_Str idlingname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_anim->SetKeyIdlingName( frameno, idlingname );
	if( ret ){
		DbgOut( "MOEFile : ReadMOEKey : anim SetKeyIdlingName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int goonflag = 0;
	ret = Read_Int( &keybuf, "goonflag ", &goonflag );
	if( ret || (goonflag < 0) ){
		goonflag = 0;
	}


	int elemno;
	for( elemno = 0; elemno < elemnum; elemno++ ){
		MOEELEM moee;
		ZeroMemory( &moee, sizeof( MOEELEM ) );
		ret = ReadMOEElem( &keybuf, &moee );
		if( ret ){
			DbgOut( "MOEFile : ReadMOEKey : ReadMOEElem %d %d error !!!\n", elemnum, elemno );
			_ASSERT( 0 );
			return 1;
		}

		ret = m_anim->AddMOEE( frameno, &moee );
		if( ret ){
			DbgOut( "MOEFile : ReadMOEKey : anim AddMOEE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}		
	}

	if( elemnum > 0 ){
		CMOEKey* moekptr = 0;
		ret = m_anim->ExistMOEKey( frameno, &moekptr );
		if( ret || !moekptr ){
			DbgOut( "MOEFile : ReadMOEKey : anim ExistMOEKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		moekptr->m_goonflag = goonflag;

	}



	int nextpos;
	nextpos = chkendpos + (int)strlen( strmoekeyend );
	animbuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return 0;
}

int CMOEFile::ReadMOEElem( MOEBUF* keybuf, MOEELEM* dstmoee )
{
	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( keybuf->buf + keybuf->pos, strmoeelemstart );
	endptr = strstr( keybuf->buf + keybuf->pos, strmoeelemend );

	if( !startptr || !endptr ){
		DbgOut( "MOEFile : ReadMOEElem : elem section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - keybuf->buf );
	if( (chkendpos >= (int)keybuf->bufleng) || (endptr < startptr) ){
		DbgOut( "MOEFile : ReadMOEElem : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MOEBUF elembuf;
	elembuf.buf = startptr;
	elembuf.bufleng = (int)( endptr - startptr );
	elembuf.pos = 0;
	elembuf.isend = 0;

	int eventno = 0;
	ret = Read_Int( &elembuf, "eventno ", &eventno );
	if( ret || (eventno < 0) ){
		DbgOut( "MOEFIle : ReadMOEElem : Read_Int eventno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////
	int listnum = 0;
	ret = Read_Int( &elembuf, "listnum ", &listnum );
	if( ret || (listnum < 0) || (listnum > MOELISTLENG) ){
		DbgOut( "MOEFIle : ReadMOEElem : Read_Int listnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int savelist[MOELISTLENG];
	ZeroMemory( savelist, sizeof( int ) * MOELISTLENG );

	int lsetno = 0;
	int listno;
	for( listno = 0; listno < listnum; listno++ ){
		char listpat[50];
		sprintf_s( listpat, 50, "list%d ", listno );
		char jname[256];
		ZeroMemory( jname, sizeof( char ) * 256 );
		ret = Read_Str( &elembuf, listpat, jname, 256 );
		if( ret ){
			DbgOut( "MOEFile : ReadMOEElem : Read_Str jointname error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int cmp;
		cmp = strcmp( jname, "全てのボーン" );
		if( cmp == 0 ){
			savelist[lsetno] = -1;
			lsetno++;
		}else{
			int seri = 0;
			ret = m_lpth->GetBoneNoByName( jname, &seri, m_lpsh, 0 );
			if( ret ){
				DbgOut( "MOEFile : ReadMOEElem : th GetBoneNoByName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( seri > 0 ){
				savelist[lsetno] = seri;
				lsetno++;
			}
		}
	}
//////////
	int notlistnum = 0;
	ret = Read_Int( &elembuf, "notlistnum ", &notlistnum );
	if( ret || (notlistnum < 0) || (notlistnum > MOELISTLENG) ){
		DbgOut( "MOEFIle : ReadMOEElem : Read_Int notlistnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int savenotlist[MOELISTLENG];
	ZeroMemory( savenotlist, sizeof( int ) * MOELISTLENG );

	int nlsetno = 0;
	int nlistno;
	for( nlistno = 0; nlistno < notlistnum; nlistno++ ){
		char nlistpat[50];
		sprintf_s( nlistpat, 50, "notlist%d ", nlistno );
		char jname[256];
		ZeroMemory( jname, sizeof( char ) * 256 );
		ret = Read_Str( &elembuf, nlistpat, jname, 256 );
		if( ret ){
			DbgOut( "MOEFile : ReadMOEElem : Read_Str jointname error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int cmp;
		cmp = strcmp( jname, "全てのボーン" );
		if( cmp == 0 ){
			savenotlist[nlsetno] = -1;
			nlsetno++;
		}else{
			int seri = 0;
			ret = m_lpth->GetBoneNoByName( jname, &seri, m_lpsh, 0 );
			if( ret ){
				DbgOut( "MOEFile : ReadMOEElem : th GetBoneNoByName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( seri > 0 ){
				savenotlist[nlsetno] = seri;
				nlsetno++;
			}
		}
	}
////////////
	ZeroMemory( dstmoee, sizeof( MOEELEM ) );

	dstmoee->eventno = eventno;
	dstmoee->listnum = lsetno;
	if( lsetno > 0 ){
		MoveMemory( dstmoee->list, savelist, sizeof( int ) * MOELISTLENG );
	}
	dstmoee->notlistnum = nlsetno;
	if( nlsetno > 0 ){
		MoveMemory( dstmoee->notlist, savenotlist, sizeof( int ) * MOELISTLENG );
	}

/////////////
	int nextpos;
	nextpos = chkendpos + (int)strlen( strmoekeyend );
	keybuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return 0;
}


int CMOEFile::DeleteAnim( CMOEAnimHandler* srcmoeah )
{
	int ret;
	if( m_anim ){
		ret = srcmoeah->DestroyAnim( m_anim->m_motkind );
		_ASSERT( !ret );
		m_anim = 0;
	}
	return 0;
}

int CMOEFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
int CMOEFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CMOEFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
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



int CMOEFile::Read_Int( MOEBUF* moebuf, char* srcpat, int* dstint )
{
	int ret;
	char* findpat;
	findpat = strstr( moebuf->buf + moebuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "MOEFile : Read_Int : %s pattern not found error !!!\n", srcpat );
		//_ASSERT( 0 );
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
		if( (srcchar + srcleng) >= (moebuf->buf + moebuf->bufleng) ){
			findend = 1;
			_ASSERT( 0 );
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "MOEFile : Read_Int : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MOEFile : Read_Int : %s GetInt error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMOEFile::Read_Float( MOEBUF* moebuf, char* srcpat, float* dstfloat )
{
	int ret;
	char* findpat;
	findpat = strstr( moebuf->buf + moebuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "MOEFile : Read_Float : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (moebuf->buf + moebuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "MOEFile : Read_Float : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MOEFile : Read_Int : %s GetFloat error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMOEFile::Read_Str( MOEBUF* moebuf, char* srcpat, char* dststr, int arrayleng )
{
	int ret;
	char* findpat;
	findpat = strstr( moebuf->buf + moebuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "MOEFile : Read_Str : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (moebuf->buf + moebuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "MOEFile : Read_Str : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "MOEFile : Read_Str : %s GetName error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




int CMOEFile::LoadMOEFile( CTreeHandler2* lpth, CShdHandler* lpsh, char* filename, CMOEAnimHandler* srcmoeah, int* animnoptr )
{
	int ret;

	if( !srcmoeah ){
		_ASSERT( 0 );
		return 1;
	}
	m_anim = 0;
	m_lpsh = lpsh;
	m_lpth = lpth;
	m_mode = MOEFILE_LOAD;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "MOEFile : LoadMOEFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	ret = SetBuffer();
	if( ret ){
		DbgOut( "MOEFile : LoadMOEFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CheckFileVersion( &m_loadversion );
	if( ret || (m_loadversion == 0) ){
		DbgOut( "MOEFile : LoadMOEFile : CheckFileVersion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadAnimInfo( srcmoeah );
	if( ret ){
		DbgOut( "MOEFile : LoadMOEFile : ReadAnimInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ReadMOEAnim();
	if( ret ){
		DbgOut( "MOEFile : LoadMOEFile : ReadMOEAnim error !!!\n" );
		_ASSERT( 0 );
		DeleteAnim( srcmoeah );
		return 1;
	}

	_ASSERT( m_anim );
	*animnoptr = m_anim->m_motkind;

	return 0;
}
