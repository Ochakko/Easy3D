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
#include <polymesh.h>
#include <polymesh2.h>
#include <Toon1Params.h>


#include <crtdbg.h>

#include <Toon1File.h>

#define DBGH
#include <dbg.h>


static char s_headerstr[256] = "RokDeBone2 Toon1 File ver1001 type0001\r\n";
static char s_headerstr2[256] = "RokDeBone2 Toon1 File ver1002 type0001\r\n";
static char s_begindispobj[16] = "BEGINDISPOBJ\r\n";
static char s_enddispobj[16] = "ENDDISPOBJ\r\n";
static char s_beginmaterial[16] = "BEGINMATERIAL\r\n";
static char s_endmaterial[16] = "ENDMATERIAL\r\n";
static char s_objname[16] = "objname = ";
static char s_darknl[16] = "darknl = ";
static char s_brightnl[16] = "brightnl = ";
static char s_matname[16] = "matname = ";
static char s_diffuse[16] = "diffuse = ";
static char s_ambient[16] = "ambient = ";
static char s_specular[16] = "specular = ";
static char s_darkh[16] = "darkh = ";
static char s_brighth[16] = "brighth = ";
static char s_edgecol0[16] = "edgecol0 = ";
static char s_edgevalid0[16] = "edgevalid0 = ";
static char s_edgeinv0[16] = "edgeinv0 = ";
static char s_edgewidth0[16] = "edgewidth0 = ";

CToon1File::CToon1File( int srcmode )
{
	InitParams();
	m_mode = srcmode;
}
CToon1File::~CToon1File()
{
	DestroyObjs();
}


int CToon1File::InitParams()
{
	m_to1buf.hfile = INVALID_HANDLE_VALUE;
	m_to1buf.buf = 0;
	m_to1buf.bufleng = 0;
	m_to1buf.pos = 0;
	m_to1buf.isend = 0;

	ZeroMemory( m_linechar, sizeof( char ) * TO1LINELENG );

	m_dispbuf.buf = 0;
	m_dispbuf.bufleng = 0;
	m_dispbuf.pos = 0;
	m_dispbuf.isend = 0;

	m_matbuf.buf = 0;
	m_matbuf.bufleng = 0;
	m_matbuf.pos = 0;
	m_matbuf.isend = 0;

	m_loadversion = 0;

	return 0;
}
int CToon1File::DestroyObjs()
{
	if( m_to1buf.buf ){
		free( m_to1buf.buf );
		m_to1buf.buf = 0;
	}
	
	if( m_to1buf.hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == TO1_SAVE ){
			FlushFileBuffers( m_to1buf.hfile );
			SetEndOfFile( m_to1buf.hfile );
		}
		CloseHandle( m_to1buf.hfile );
		m_to1buf.hfile = INVALID_HANDLE_VALUE;
	}

	DestroyDispBuf();
	DestroyMatBuf();

	return 0;
}

int CToon1File::DestroyDispBuf()
{
	if( m_dispbuf.buf ){
		free( m_dispbuf.buf );
		m_dispbuf.buf = 0;
	}
	m_dispbuf.bufleng = 0;
	m_dispbuf.pos = 0;
	m_dispbuf.isend = 0;

	return 0;
}

int CToon1File::DestroyMatBuf()
{
	if( m_matbuf.buf ){
		free( m_matbuf.buf );
		m_matbuf.buf = 0;
	}
	m_matbuf.bufleng = 0;
	m_matbuf.pos = 0;
	m_matbuf.isend = 0;

	return 0;
}

int CToon1File::SaveToon1File( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh )
{

	if( !lpth || !lpsh ){
		DbgOut( "toon1file : SaveToon1File : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_thandler = lpth;
	m_shandler = lpsh;

	m_to1buf.hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_to1buf.hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "toon1file : SaveToon1File : CreateFile error !!! %s\n", filename );
		_ASSERT( 0 );
		return 1;
	}	

	int ret;
	ret = WriteHeader();
	if( ret ){
		DbgOut( "toon1file : SaveToon1File : WriteHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int seri;
	CShdElem* selem;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		_ASSERT( selem );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = WriteDispObj( selem );
			if( ret ){
				DbgOut( "toon1file : SaveToon1File : WriteDispObj error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


int CToon1File::WriteHeader()
{
	strcpy_s( m_linechar, TO1LINELENG, s_headerstr2 );

	int ret;
	ret = WriteLinechar( 1 );
	if( ret ){
		DbgOut( "toon1file : WriteHeader : WriteLinechar error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CToon1File::WriteLinechar( int addreturn )
{
	unsigned long wleng, writeleng;

	if( addreturn )
		strcat_s( m_linechar, TO1LINELENG, "\r\n" );

	wleng = (int)strlen( m_linechar );
	if( wleng >= TO1LINELENG ){
		DbgOut( "toon1file : WriteLinechar : wleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	WriteFile( m_to1buf.hfile, m_linechar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CToon1File::WriteDispObj( CShdElem* selem )
{
	int ret;
	int materialnum = 0;
	CToon1Params* toon1ptr = 0;
	if( selem->type == SHDPOLYMESH ){
		CPolyMesh* pm;
		pm = selem->polymesh;
		if( pm ){
			materialnum = pm->m_materialnum;
			toon1ptr = pm->m_toon1;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2;
		pm2 = selem->polymesh2;
		if( pm2 ){
			materialnum = pm2->m_materialnum;
			toon1ptr = pm2->m_toon1;
		}
	}else{
		DbgOut( "toon1file : WRiteDispObj : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (materialnum <= 0) || !toon1ptr ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	strcpy_s( m_linechar, TO1LINELENG, s_begindispobj );
	ret = WriteLinechar( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CTreeElem2* telem;
	telem = (*m_thandler)( selem->serialno );
	_ASSERT( telem );
	sprintf_s( m_linechar, TO1LINELENG, "\t%s\"%s\"", s_objname, telem->name );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int matno;
	for( matno = 0; matno < materialnum; matno++ ){
		CToon1Params* curtoon1;
		curtoon1 = toon1ptr + matno;
		_ASSERT( curtoon1 );
		ret = WriteMaterial( curtoon1 );
		if( ret ){
			DbgOut( "toon1file : WriteDispObj : WriteMaterial %d error !!!\n", matno );
			_ASSERT( 0 );
			return 1;
		}
	}

	strcpy_s( m_linechar, TO1LINELENG, s_enddispobj );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CToon1File::WriteMaterial( CToon1Params* toon1param )
{
	int ret;
	sprintf_s( m_linechar, TO1LINELENG, "\t%s", s_beginmaterial );
	ret = WriteLinechar( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}




	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s\"%s\"", s_matname, toon1param->name );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s( %f, %f, %f )", 
		s_diffuse, toon1param->diffuse.r, toon1param->diffuse.g, toon1param->diffuse.b );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s( %f, %f, %f )", 
		s_ambient, toon1param->ambient.r, toon1param->ambient.g, toon1param->ambient.b );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s( %f, %f, %f )", 
		s_specular, toon1param->specular.r, toon1param->specular.g, toon1param->specular.b );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	sprintf_s( m_linechar, TO1LINELENG, "\t%s%f", s_darknl, toon1param->toon0dnl );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	sprintf_s( m_linechar, TO1LINELENG, "\t%s%f", s_brightnl, toon1param->toon0bnl );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s( %f, %f, %f )", 
		s_edgecol0, toon1param->edgecol0.r, toon1param->edgecol0.g, toon1param->edgecol0.b );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s%d", s_edgevalid0, toon1param->edgevalid0 );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s%d", s_edgeinv0, toon1param->edgeinv0 );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	sprintf_s( m_linechar, TO1LINELENG, "\t\t%s%f", s_edgewidth0, toon1param->edgecol0.a );
	ret = WriteLinechar( 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}




	sprintf_s( m_linechar, TO1LINELENG, "\t%s", s_endmaterial );
	ret = WriteLinechar( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int CToon1File::LoadToon1File( HWND srchwnd, char* filename, CTreeHandler2* lpth, CShdHandler* lpsh )
{


	int ret = 0;

	m_thandler = lpth;
	m_shandler = lpsh;

	ret = SetBuffer( filename );
	if( ret ){
		DbgOut( "toon1file : LoadToon1File : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CheckFileHeader();
	if( ret && srchwnd ){
		::MessageBox( srchwnd, "ファイルのフォーマットが異なります。\n読み込めません。", "エラー", MB_OK );
		return 0;
	}

//_ASSERT( 0 );

	while( m_to1buf.isend == 0 ){
//_ASSERT( 0 );
		int seri = 0;
		float darknl = 0.4f;
		float brightnl = 0.9f;
		ret = SetDispBuffer( &seri, &darknl, &brightnl );
		if( ret ){
			DbgOut( "toon1file : SetDispBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//_ASSERT( 0 );
		if( seri > 0 ){
			ret = SetNL( seri, darknl, brightnl );
			_ASSERT( !ret );

			while( m_dispbuf.isend == 0 ){
				int materialno = -1;
				ret = SetMaterialBuffer( seri, &materialno );
				if( ret ){
					DbgOut( "toon1file : SetMaterialBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				//if( materialno >= 0 ){
				if( m_dispbuf.isend == 0 ){
					ret = ReadMaterialBuffer( seri, materialno );
					if( ret ){
						DbgOut( "toon1file : ReadMaterialBuffer error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				//}
			}
		}
	}


	return 0;
}

int CToon1File::SetBuffer( char* filename )
{
	DestroyObjs();

	HANDLE hfile;
	hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	
	m_to1buf.hfile = hfile;


	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_to1buf.hfile, &sizehigh );
	if( bufleng <= 0 ){
		DbgOut( "Toon1File : SetBuffer :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "Toon1File : SetBuffer :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );//bufleng + 1
	if( !newbuf ){
		DbgOut( "Toon1File : SetBuffer :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_to1buf.hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "Toon1File : SetBuffer :  ReadFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_to1buf.buf = newbuf;
	*(m_to1buf.buf + bufleng) = 0;//!!!!!!!!
	m_to1buf.bufleng = bufleng;
	m_to1buf.pos = 0;
	m_to1buf.isend = 0;

	return 0;
}

int CToon1File::CheckFileHeader()
{
	m_loadversion = 0;

	int headerleng = 0;
	char* headerptr = 0;
	headerptr = strstr( m_to1buf.buf, s_headerstr );
	if( headerptr ){
		m_loadversion = 1001;
		headerleng = (int)strlen( s_headerstr );
	}else{
		char* header2 = 0;
		header2 = strstr( m_to1buf.buf, s_headerstr2 );
		if( header2 ){
			m_loadversion = 1002;
			headerleng = (int)strlen( s_headerstr2 );
		}else{
			DbgOut( "toon1file : CheckFileHeader : this file is not supported error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	m_to1buf.pos = headerleng;

	return 0;
}

int CToon1File::SetDispBuffer( int* seriptr, float* darknl, float* brightnl )
{
	int ret;
	
	DestroyDispBuf();//!!!!!!!!!!!!!!!!


	char* startptr;
	startptr = strstr( m_to1buf.buf + m_to1buf.pos, s_begindispobj );
	if( !startptr ){
		*seriptr = 0;
		m_to1buf.isend = 1;
		return 0;
	}

	int startleng;
	startleng = (int)strlen( s_begindispobj );
	char* endptr;
	endptr = strstr( startptr + startleng, s_enddispobj );
	if( !endptr ){
		endptr = m_to1buf.buf + m_to1buf.bufleng;
		m_to1buf.isend = 1;
	}else{
		int endleng;
		endleng = (int)strlen( s_enddispobj );
		endptr = endptr + endleng;
	}

	int displeng;
	displeng = (int)( endptr - startptr ) + 1;
	m_dispbuf.buf = (char*)malloc( sizeof( char ) * (displeng + 1) );
	if( !m_dispbuf.buf ){
		*seriptr = 0;
		m_to1buf.isend = 1;
		return 1;
	}
	strncpy_s( m_dispbuf.buf, displeng + 1, startptr, displeng );
	*( m_dispbuf.buf + displeng ) = 0;
	m_dispbuf.bufleng = displeng;

	m_to1buf.pos = (int)( endptr - m_to1buf.buf );//!!!!!!!!!!!!!!!!!!!!

/////
	char* objnameptr;
	objnameptr = strstr( m_dispbuf.buf, s_objname );
	if( !objnameptr ){
		*seriptr = 0;
		m_to1buf.isend = 1;
		return 0;
	}
	int objnameleng;
	objnameleng = (int)strlen( s_objname );
	char* namevalptr;
	namevalptr = objnameptr + objnameleng;
	int restsize;
	restsize = m_dispbuf.bufleng - (int)( namevalptr - m_dispbuf.buf );

	char name[256];
	ret = GetName( name, 256, namevalptr, 0, restsize );
	if( ret ){
		*seriptr = 0;
		return 0;
	}

	ret = m_thandler->GetDispObjNoByName( name, seriptr, m_shandler, 0 );
	if( ret ){
		*seriptr = 0;
		return 0;
	}

	//if( *seriptr > 0 ){
	if( m_loadversion == 1001 ){
		char* darknlptr;
		darknlptr = strstr( m_dispbuf.buf, s_darknl );
		if( darknlptr ){
			int darknlleng;
			darknlleng = (int)strlen( s_darknl );
			char* darknlvalptr;
			darknlvalptr = darknlptr + darknlleng;
			restsize = m_dispbuf.bufleng - (int)( darknlvalptr - m_dispbuf.buf );
			int stepnum = 0;
			ret = GetFloat( darknl, darknlvalptr, 0, restsize, &stepnum );
			_ASSERT( !ret );
		}

		char* brightnlptr;
		brightnlptr = strstr( m_dispbuf.buf, s_brightnl );
		if( brightnlptr ){
			int brightnlleng;
			brightnlleng = (int)strlen( s_brightnl );
			char* brightnlvalptr;
			brightnlvalptr = brightnlptr + brightnlleng;
			restsize = m_dispbuf.bufleng - (int)( brightnlvalptr - m_dispbuf.buf );
			int stepnum = 0;
			ret = GetFloat( brightnl, brightnlvalptr, 0, restsize, &stepnum );
			_ASSERT( !ret );
		}
	}

	//}


	return 0;
}

int CToon1File::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
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
		return 1;
	}


	return 0;
}
int CToon1File::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CToon1File::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CToon1File::SetNL( int seri, float darknl, float brightnl )
{
	int materialnum = 0;
	CToon1Params* toon1ptr = 0;
	CShdElem* selem;
	selem = (*m_shandler)( seri );
	_ASSERT( selem );
	if( selem->type == SHDPOLYMESH ){
		toon1ptr = selem->polymesh->m_toon1;
		materialnum = selem->polymesh->m_materialnum;
	}else if( selem->type == SHDPOLYMESH2 ){
		toon1ptr = selem->polymesh2->m_toon1;
		materialnum = selem->polymesh2->m_materialnum;
	}else{
		_ASSERT( 0 );
		return 0;
	}

	if( !toon1ptr || (materialnum <= 0) ){
		_ASSERT( 0 );
		return 0;
	}

	int matno;
	for( matno = 0; matno < materialnum; matno++ ){
		( toon1ptr + matno )->darknl = darknl;
		( toon1ptr + matno )->brightnl = brightnl;

		if( m_loadversion == 1001 ){
			( toon1ptr + matno )->toon0dnl = darknl;
			( toon1ptr + matno )->toon0bnl = brightnl;
		}
	}

	return 0;
}

int CToon1File::SetMaterialBuffer( int seri, int* matnoptr )
{
	int ret;

	DestroyMatBuf();

	char* beginmatptr;
	beginmatptr = strstr( m_dispbuf.buf + m_dispbuf.pos, s_beginmaterial );
	if( !beginmatptr ){
		*matnoptr = -1;
		m_dispbuf.isend = 1;
		return 0;
	}
	int beginmatleng;
	beginmatleng = (int)strlen( s_beginmaterial );
	m_dispbuf.pos += beginmatleng;

	char* endmatptr;
	endmatptr = strstr( m_dispbuf.buf + m_dispbuf.pos, s_endmaterial );
	char* endmatvalptr;
	if( !endmatptr ){
		endmatvalptr = m_dispbuf.buf + m_dispbuf.bufleng - 1;
		m_dispbuf.isend = 1;
	}else{
		int endmatleng;
		endmatleng = (int)strlen( s_endmaterial );
		endmatvalptr = endmatptr + endmatleng;
		m_dispbuf.pos = (int)( endmatptr - m_dispbuf.buf ) + endmatleng;
	}

	int matbufleng;
	matbufleng = (int)( endmatvalptr - beginmatptr ) + 1;

	m_matbuf.buf = (char*)malloc( sizeof( char ) * (matbufleng + 1) );
	if( !m_matbuf.buf ){
		DbgOut( "toon1file : SetMaterialBuffer : matbuf.buf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( m_matbuf.buf, matbufleng + 1, beginmatptr, matbufleng );
	*( m_matbuf.buf + matbufleng ) = 0;
	m_matbuf.bufleng = matbufleng;

///////////
	char* matnameptr;
	matnameptr = strstr( m_matbuf.buf + m_matbuf.pos, s_matname );
	if( !matnameptr ){
		_ASSERT( 0 );
		*matnoptr = -1;
		return 0;
	}
	int matnameleng;
	matnameleng = (int)strlen( s_matname );
	char* matnamevalptr;
	matnamevalptr = matnameptr + matnameleng;
	int restsize;
	restsize = m_matbuf.bufleng - (int)( matnamevalptr - m_matbuf.buf );
	char matname[32];
	ret = GetName( matname, 32, matnamevalptr, 0, restsize );	
	if( ret ){
		_ASSERT( 0 );
		*matnoptr = -1;
		return 0;
	}

	CShdElem* selem;
	selem = (*m_shandler)( seri );
	_ASSERT( selem );

	ret = selem->GetToon1MaterialNoByName( matname, matnoptr );
	if( ret ){
		_ASSERT( 0 );
		*matnoptr = -1;
		return 0;
	}


	return 0;
}

int CToon1File::ReadMaterialBuffer( int seri, int matno )
{
	int ret, ret2, ret3;

	if( seri < 0 ){
		_ASSERT( 0 );
		return 0;
	}

	CShdElem* selem;
	selem = (*m_shandler)( seri );
	_ASSERT( selem );
	CToon1Params* toon1ptr = 0;
	if( matno >= 0 ){
		ret = selem->GetToon1Ptr( matno, &toon1ptr );
		_ASSERT( !ret );
	}

	if( m_matbuf.isend || !m_matbuf.buf || (m_matbuf.bufleng <= 0) ){
		_ASSERT( 0 );
		return 0;
	}

	int restsize;
	int stepnum;

	char* diffuseptr;
	diffuseptr = strstr( m_matbuf.buf, s_diffuse );
	if( diffuseptr ){
		int diffuseleng;
		diffuseleng = (int)strlen( s_diffuse );
		char* diffuseval1ptr;
		diffuseval1ptr = diffuseptr + diffuseleng;
		restsize = m_matbuf.bufleng - (int)( diffuseval1ptr - m_matbuf.buf );
		stepnum = 0;
		D3DCOLORVALUE diffuse;
		ret = GetFloat( &(diffuse.r), diffuseval1ptr, 0, restsize, &stepnum );
		if( ret == 0 ){
			char* diffuseval2ptr = diffuseval1ptr + stepnum;
			restsize -= stepnum;
			stepnum = 0;
			ret2 = GetFloat( &(diffuse.g), diffuseval2ptr, 0, restsize, &stepnum );
			if( ret2 == 0 ){
				char* diffuseval3ptr = diffuseval2ptr + stepnum;
				restsize -= stepnum;
				stepnum = 0;
				ret3 = GetFloat( &(diffuse.b), diffuseval3ptr, 0, restsize, &stepnum );
				if( ret3 == 0 ){
					diffuse.a = 1.0f;
					if( toon1ptr ){
						toon1ptr->diffuse = diffuse;
					}
//_ASSERT( 0 );
				}else{
					_ASSERT( 0 );
				}
			}else{
				_ASSERT( 0 );
			}
		}else{
			_ASSERT( 0 );
		}
	}else{
		_ASSERT( 0 );
	}

	char* ambientptr;
	ambientptr = strstr( m_matbuf.buf, s_ambient );
	if( ambientptr ){
		int ambientleng;
		ambientleng = (int)strlen( s_ambient );
		char* ambientval1ptr;
		ambientval1ptr = ambientptr + ambientleng;
		restsize = m_matbuf.bufleng - (int)( ambientval1ptr - m_matbuf.buf );
		stepnum = 0;
		D3DCOLORVALUE ambient;
		ret = GetFloat( &(ambient.r), ambientval1ptr, 0, restsize, &stepnum );
		if( ret == 0 ){
			char* ambientval2ptr = ambientval1ptr + stepnum;
			restsize -= stepnum;
			stepnum = 0;
			ret2 = GetFloat( &(ambient.g), ambientval2ptr, 0, restsize, &stepnum );
			if( ret2 == 0 ){
				char* ambientval3ptr = ambientval2ptr + stepnum;
				restsize -= stepnum;
				stepnum = 0;
				ret3 = GetFloat( &(ambient.b), ambientval3ptr, 0, restsize, &stepnum );
				if( ret3 == 0 ){
					ambient.a = 1.0f;
					if( toon1ptr ){
						toon1ptr->ambient = ambient;
					}
				}
			}
		}
	}

	char* specularptr;
	specularptr = strstr( m_matbuf.buf, s_specular );
	if( specularptr ){
		int specularleng;
		specularleng = (int)strlen( s_specular );
		char* specularval1ptr;
		specularval1ptr = specularptr + specularleng;
		restsize = m_matbuf.bufleng - (int)( specularval1ptr - m_matbuf.buf );
		stepnum = 0;
		D3DCOLORVALUE specular;
		ret = GetFloat( &(specular.r), specularval1ptr, 0, restsize, &stepnum );
		if( ret == 0 ){
			char* specularval2ptr = specularval1ptr + stepnum;
			restsize -= stepnum;
			stepnum = 0;
			ret2 = GetFloat( &(specular.g), specularval2ptr, 0, restsize, &stepnum );
			if( ret2 == 0 ){
				char* specularval3ptr = specularval2ptr + stepnum;
				restsize -= stepnum;
				stepnum = 0;
				ret3 = GetFloat( &(specular.b), specularval3ptr, 0, restsize, &stepnum );
				if( ret3 == 0 ){
					specular.a = 1.0f;
					if( toon1ptr ){
						toon1ptr->specular = specular;
					}
				}
			}
		}
	}

	if( m_loadversion >= 1002 ){
		float dstdarknl;
		char* darknlptr;
		darknlptr = strstr( m_matbuf.buf, s_darknl );
		if( darknlptr ){
			int darknlleng;
			darknlleng = (int)strlen( s_darknl );
			char* darknlvalptr;
			darknlvalptr = darknlptr + darknlleng;
			restsize = m_matbuf.bufleng - (int)( darknlvalptr - m_matbuf.buf );
			int stepnum = 0;
			ret = GetFloat( &dstdarknl, darknlvalptr, 0, restsize, &stepnum );
			_ASSERT( !ret );
			if( toon1ptr ){
				toon1ptr->toon0dnl = dstdarknl;
			}
		}

		float dstbrightnl;
		char* brightnlptr;
		brightnlptr = strstr( m_matbuf.buf, s_brightnl );
		if( brightnlptr ){
			int brightnlleng;
			brightnlleng = (int)strlen( s_brightnl );
			char* brightnlvalptr;
			brightnlvalptr = brightnlptr + brightnlleng;
			restsize = m_matbuf.bufleng - (int)( brightnlvalptr - m_matbuf.buf );
			int stepnum = 0;
			ret = GetFloat( &dstbrightnl, brightnlvalptr, 0, restsize, &stepnum );
			_ASSERT( !ret );
			if( toon1ptr ){
				toon1ptr->toon0bnl = dstbrightnl;
			}
		}
	}

	char* edgecol0ptr;
	edgecol0ptr = strstr( m_matbuf.buf, s_edgecol0 );
	if( edgecol0ptr ){
		int edgecol0leng;
		edgecol0leng = (int)strlen( s_edgecol0 );
		char* edgecol0val1ptr;
		edgecol0val1ptr = edgecol0ptr + edgecol0leng;
		restsize = m_matbuf.bufleng - (int)( edgecol0val1ptr - m_matbuf.buf );
		stepnum = 0;
		D3DCOLORVALUE edgecol0;
		ret = GetFloat( &(edgecol0.r), edgecol0val1ptr, 0, restsize, &stepnum );
		if( ret == 0 ){
			char* edgecol0val2ptr = edgecol0val1ptr + stepnum;
			restsize -= stepnum;
			stepnum = 0;
			ret2 = GetFloat( &(edgecol0.g), edgecol0val2ptr, 0, restsize, &stepnum );
			if( ret2 == 0 ){
				char* edgecol0val3ptr = edgecol0val2ptr + stepnum;
				restsize -= stepnum;
				stepnum = 0;
				ret3 = GetFloat( &(edgecol0.b), edgecol0val3ptr, 0, restsize, &stepnum );
				if( (ret3 == 0) && toon1ptr ){
					//edgecol0.a = 1.0f; aはwidth !!!!
					toon1ptr->edgecol0.r = edgecol0.r;
					toon1ptr->edgecol0.g = edgecol0.g;
					toon1ptr->edgecol0.b = edgecol0.b;
				}
			}
		}
	}

	if( m_loadversion == 1001 ){
		char* darkhptr;
		darkhptr = strstr( m_matbuf.buf, s_darkh );
		if( darkhptr ){
			int darkhleng;
			darkhleng = (int)strlen( s_darkh );
			char* darkhvalptr;
			darkhvalptr = darkhptr + darkhleng;
			restsize = m_matbuf.bufleng - (int)( darkhvalptr - m_matbuf.buf );
			float darkh;
			ret = GetFloat( &darkh, darkhvalptr, 0, restsize, &stepnum );
			if( (ret == 0) && toon1ptr ){
				toon1ptr->darkh = darkh;
			}
		}

		char* brighthptr;
		brighthptr = strstr( m_matbuf.buf, s_brighth );
		if( brighthptr ){
			int brighthleng;
			brighthleng = (int)strlen( s_brighth );
			char* brighthvalptr;
			brighthvalptr = brighthptr + brighthleng;
			restsize = m_matbuf.bufleng - (int)( brighthvalptr - m_matbuf.buf );
			float brighth;
			ret = GetFloat( &brighth, brighthvalptr, 0, restsize, &stepnum );
			if( (ret == 0) && toon1ptr ){
				toon1ptr->brighth = brighth;
			}
		}
	}

	char* edgevalid0ptr;
	edgevalid0ptr = strstr( m_matbuf.buf, s_edgevalid0 );
	if( edgevalid0ptr ){
		int edgevalid0leng;
		edgevalid0leng = (int)strlen( s_edgevalid0 );
		char* edgevalid0valptr;
		edgevalid0valptr = edgevalid0ptr + edgevalid0leng;
		restsize = m_matbuf.bufleng - (int)( edgevalid0valptr - m_matbuf.buf );
		int edgevalid0;
		ret = GetInt( &edgevalid0, edgevalid0valptr, 0, restsize, &stepnum );
		if( (ret == 0) && toon1ptr ){
			toon1ptr->edgevalid0 = edgevalid0;
		}
	}

	char* edgeinv0ptr;
	edgeinv0ptr = strstr( m_matbuf.buf, s_edgeinv0 );
	if( edgeinv0ptr ){
		int edgeinv0leng;
		edgeinv0leng = (int)strlen( s_edgeinv0 );
		char* edgeinv0valptr;
		edgeinv0valptr = edgeinv0ptr + edgeinv0leng;
		restsize = m_matbuf.bufleng - (int)( edgeinv0valptr - m_matbuf.buf );
		int edgeinv0;
		ret = GetInt( &edgeinv0, edgeinv0valptr, 0, restsize, &stepnum );
		if( (ret == 0) && toon1ptr ){
			toon1ptr->edgeinv0 = edgeinv0;
		}
	}

	char* edgewidth0ptr;
	edgewidth0ptr = strstr( m_matbuf.buf, s_edgewidth0 );
	if( edgewidth0ptr ){
		int edgewidth0leng;
		edgewidth0leng = (int)strlen( s_edgewidth0 );
		char* edgewidth0valptr;
		edgewidth0valptr = edgewidth0ptr + edgewidth0leng;
		restsize = m_matbuf.bufleng - (int)( edgewidth0valptr - m_matbuf.buf );
		float edgewidth0;
		ret = GetFloat( &edgewidth0, edgewidth0valptr, 0, restsize, &stepnum );
		if( (ret == 0) && toon1ptr ){
			toon1ptr->edgecol0.a = edgewidth0;
		}
	}


	return 0;
}
