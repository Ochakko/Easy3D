#include "stdafx.h"

#include <math.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <direct.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include "SndSetFile.h"



#include "c:\\pgfile9\\RokDeBone2DX\\SoundBank.h"
#include "c:\\pgfile9\\RokDeBone2DX\\SoundSet.h"
#include "c:\\pgfile9\\RokDeBone2DX\\SoundElem.h"

#include <SndPath.h>

#include <crtdbg.h>


enum {
	SSTFILE_WRITE,
	SSTFILE_LOAD,
	SSTFILE_MAX
};

static char strheader[256] = "RokDeBone2 SoundSetFile ver1001 type00\r\n";

static char strsetstart[256] = "#SoundSet Start\r\n";
static char strsetend[256] = "#SoundSet End\r\n";

static char strbgmstart[256] = "#BGM Start\r\n";
static char strbgmend[256] = "#BGM End\r\n";

static char streffectstart[256] = "#Effect Start\r\n";
static char streffectend[256] = "#Effect End\r\n";

static char strendfile[256] = "#EndOfFile\r\n";


CSndSetFile::CSndSetFile( CHuSound* srchs, CSoundBank* srcsb )
{
	InitParams();
	m_sndbnk = srcsb;
	m_hs = srchs;
}
CSndSetFile::~CSndSetFile()
{
	DestroyObjs();
}

int CSndSetFile::InitParams()
{
	m_mode = SSTFILE_WRITE;
	m_hfile = INVALID_HANDLE_VALUE;
	m_sstbuf.buf = 0;
	m_sstbuf.bufleng = 0;
	m_sstbuf.pos = 0;
	m_sstbuf.isend = 0;
	ZeroMemory( m_line, sizeof( char ) * SSTLINELEN );
	m_sndbnk = 0;
	m_hs = 0;
	m_loadversion = 0;

	ZeroMemory( m_ssfdir, sizeof( char ) * MAX_PATH );

	return 0;
}
int CSndSetFile::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == SSTFILE_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_sstbuf.buf ){
		free( m_sstbuf.buf );
		m_sstbuf.buf = 0;
	}
	m_sstbuf.bufleng = 0;
	m_sstbuf.pos = 0;
	m_sstbuf.isend = 0;

	return 0;
}

int CSndSetFile::Write2File( char* lpFormat, ... )
{
	if( !m_hfile ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[SSTLINELEN];
			
	ZeroMemory( outchar, SSTLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, SSTLINELEN, lpFormat, Marker );
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

int CSndSetFile::WriteSndSetFile( char* filename, char* ssfdir )
{
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "SndSetFile : WriteSndSetFile : file open error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !m_sndbnk ){
		_ASSERT( 0 );
		return 0;
	}
	if( m_sndbnk->m_setnum <= 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = Write2File( strheader );
	_ASSERT( !ret );

	ret = Write2File( "$SoundSetNum %d\r\n\r\n", m_sndbnk->m_setnum );
	_ASSERT( !ret );

	int ssno;
	for( ssno = 0; ssno < m_sndbnk->m_setnum; ssno++ ){
		ret = WriteSoundSet( ssno, ssfdir );
		if( ret ){
			DbgOut( "SndSetFile : WriteSndSetFile : WriteSoundSet error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = Write2File( strendfile );
	_ASSERT( !ret );

	return 0;
}

int CSndSetFile::WriteSoundSet( int ssno, char* ssfdir )
{
	int ret;

	CSoundSet* curss = *( m_sndbnk->m_ppset + ssno );
	if( !curss ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( strsetstart );
	_ASSERT( !ret );

	ret = Write2File( "\t$name \"%s\"\r\n", curss->m_setname );
	_ASSERT( !ret );

//	char dirname[512];
//	ZeroMemory( dirname, sizeof( char ) * 512 );
//	ret = ConvEn( dirname, 512, curss->m_dirname );
//	if( ret ){
//		DbgOut( "SndSetFile : WriteSoundSet : ConvEn error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}
//	ret = Write2File( "\t$dirname \"%s\"\r\n", dirname );
//	_ASSERT( !ret );

	ret = Write2File( "\t$dirname \"%s\"\r\n", curss->m_dirname );
	_ASSERT( !ret );


	ret = Write2File( "\t$effectnum %d\r\n", curss->m_soundnum );
	_ASSERT( !ret );


	ret = WriteBGM( curss, ssfdir );
	if( ret ){
		DbgOut( "SndSetFile : WriteSoundSet : WriteBGM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curss->m_soundnum >= 0 ){
		ret = WriteEffect( curss, ssfdir );
		if( ret ){
			DbgOut( "SndSetFile : WriteSoundSet : WriteEffect error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = Write2File( "%s\r\n", strsetend );
	_ASSERT( !ret );

	return 0;
}
int CSndSetFile::WriteBGM( CSoundSet* curss, char* ssfdir )
{
//	#BGM Start
//		$name "test.ogg"
//		$reverb 1
//		$bufnum 1
//	#BGM End

	CSoundElem* bgm = curss->m_bgm;
	if( !bgm ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = Write2File( "\t%s", strbgmstart );
	_ASSERT( !ret );

	ret = Write2File( "\t\t$name \"%s\"\r\n", bgm->m_filename );
	_ASSERT( !ret );

	ret = Write2File( "\t\t$reverb %d\r\n", bgm->m_reverbflag );
	_ASSERT( !ret );

//	ret = Write2File( "\t\t$bufnum %d\r\n", bgm->m_bufnum );
//	_ASSERT( !ret );

	ret = Write2File( "\t%s\r\n", strbgmend );
	_ASSERT( !ret );

	if( ssfdir ){
		char srcpath[MAX_PATH];
		ZeroMemory( srcpath, sizeof( char ) * MAX_PATH );
		ret = SndMakePath( curss->m_dirname, curss->m_ssfdir, bgm->m_filename, srcpath );
		if( ret ){
			DbgOut( "SndSetFile : WriteBGM : SndMakePath 0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		char wavdir[MAX_PATH];
		ZeroMemory( wavdir, sizeof( char ) * MAX_PATH );
		char ssfendir[MAX_PATH];
		strcpy_s( ssfendir, MAX_PATH, ssfdir );
		strcat_s( ssfendir, MAX_PATH, "\\" );
		ret = SndMakePath( curss->m_dirname, ssfendir, bgm->m_filename, wavdir );
		if( ret ){
			DbgOut( "SndSetFile : WriteBGM : SndMakePath 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		char *wavlasten;
		int ch = '\\';
		wavlasten = strrchr( wavdir, ch );
		if( !wavlasten ){
			DbgOut( "SndSetFile : WriteBGM : wavlasten NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		*wavlasten = 0;


		ret = _access( wavdir, 0 );
		if( ret != 0 ){
			int ret2;
			ret2 = _mkdir( wavdir );
			if( ret2 != 0 ){
				DbgOut( "SndSetFile : WriteBGM : mkdir error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


		char dstpath[MAX_PATH];
		ZeroMemory( dstpath, sizeof( char ) * MAX_PATH );
		sprintf_s( dstpath, MAX_PATH, "%s\\%s", wavdir, bgm->m_filename );

		int cmpsnd;
		cmpsnd = strcmp( srcpath, dstpath );
		if( cmpsnd != 0 ){
			BOOL bret;
			BOOL bcancel = FALSE;
			bret = CopyFileEx( srcpath, dstpath, NULL, NULL, &bcancel, 0 );
			if( bret == 0 ){
				DbgOut( "SndSetFile : WriteBGM : CopyFileEx error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}


	return 0;
}
int CSndSetFile::WriteEffect( CSoundSet* curss, char* ssfdir )
{
//	#Effect Start
//		$name "beep.wav"
//		$reverb 0
//		$bufnum 5
//	#Effect End	
	int ret;
	int eno;
	for( eno = 0; eno < curss->m_soundnum; eno++ ){
		CSoundElem* curse = *( curss->m_ppsound + eno );
		if( !curse ){
			DbgOut( "SndSetFile : WriteEffect : curse NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = Write2File( "\t%s", streffectstart );
		_ASSERT( !ret );

		ret = Write2File( "\t\t$name \"%s\"\r\n", curse->m_filename );
		_ASSERT( !ret );

		ret = Write2File( "\t\t$reverb %d\r\n", curse->m_reverbflag );
		_ASSERT( !ret );

		ret = Write2File( "\t\t$bufnum %d\r\n", curse->m_bufnum );
		_ASSERT( !ret );

		ret = Write2File( "\t%s", streffectend );
		_ASSERT( !ret );


		if( ssfdir ){
			char srcpath[MAX_PATH];
			ZeroMemory( srcpath, sizeof( char ) * MAX_PATH );
			ret = SndMakePath( curss->m_dirname, curss->m_ssfdir, curse->m_filename, srcpath );
			if( ret ){
				DbgOut( "SndSetFile : WriteEffect : SndMakePath error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			char wavdir[MAX_PATH];
			ZeroMemory( wavdir, sizeof( char ) * MAX_PATH );
			char ssfendir[MAX_PATH];
			strcpy_s( ssfendir, MAX_PATH, ssfdir );
			strcat_s( ssfendir, MAX_PATH, "\\" );
			ret = SndMakePath( curss->m_dirname, ssfendir, curse->m_filename, wavdir );
			if( ret ){
				DbgOut( "SndSetFile : WriteEffect : SndMakePath 1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			char *wavlasten;
			int ch = '\\';
			wavlasten = strrchr( wavdir, ch );
			if( !wavlasten ){
				DbgOut( "SndSetFile : WriteEffecct : wavlasten NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			*wavlasten = 0;


			char dstpath[MAX_PATH];
			ZeroMemory( dstpath, sizeof( char ) * MAX_PATH );
			sprintf_s( dstpath, MAX_PATH, "%s\\%s", wavdir, curse->m_filename );

			int cmpsnd;
			cmpsnd = strcmp( srcpath, dstpath );
			if( cmpsnd != 0 ){
				BOOL bret;
				BOOL bcancel = FALSE;
				bret = CopyFileEx( srcpath, dstpath, NULL, NULL, &bcancel, 0 );
				if( bret == 0 ){
					DbgOut( "SndSetFile : WriteEffect : CopyFileEx error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;
}

int CSndSetFile::ConvEn( char* dstname, int arrayleng, char* srcname )
{

	int srcleng = (int)strlen( srcname );
	if( srcleng <= 0 ){
		*dstname = 0;
		return 0;
	}

	int srcno;
	int setno = 0;
	for( srcno = 0; srcno < srcleng; srcno++ ){
		char srcchar = *( srcname + srcno );
		if( srcchar != '\\' ){
			if( setno >= arrayleng ){
				_ASSERT( 0 );
				return 1;
			}
			*( dstname + setno ) = srcchar;
			setno++;
		}else{
			// --> 
			if( setno >= arrayleng ){
				_ASSERT( 0 );
				return 1;
			}
			*( dstname + setno ) = '\\';
			setno++;


			if( setno >= arrayleng ){
				_ASSERT( 0 );
				return 1;
			}
			*( dstname + setno ) = '\\';
			setno++;
		}
	}

	*( dstname + setno ) = 0;

	return 0;
}

int CSndSetFile::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "SndSetFile : SetBuffer  :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "SndSetFile : SetBuffer  :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		DbgOut( "SndSetFile : SetBuffer  :  newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "SndSetFile : SetBuffer  :  ReadFile error !!!\n" );
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_sstbuf.buf = newbuf;
	m_sstbuf.pos = 0;
	m_sstbuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, strendfile );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_sstbuf.bufleng = validleng;

	return 0;
}

int CSndSetFile::CheckFileVersion( int* verptr )
{

	char* headerptr;
	headerptr = strstr( m_sstbuf.buf, strheader );
	if( headerptr ){
		*verptr = 1001;
	}else{
		*verptr = 0;
	}

	return 0;
}
int CSndSetFile::ReadSoundSetNum( int* ssnum )
{
	*ssnum = 0;

	int ret;	
	int tmpnum = 0;
	ret = Read_Int( &m_sstbuf, "$SoundSetNum ", &tmpnum );
	if( ret || (tmpnum <= 0) ){
		DbgOut( "SndSetFile : ReadSoundSetNum : Read_Int ssnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*ssnum = tmpnum;

	return 0;
}
int CSndSetFile::ReadSoundSet( SNDSETBUF* bnkbuf )
{
	if( !m_sndbnk ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;

//	$name "ƒeƒXƒg‚P"
//	$dirname "..\\SoundTest"
//	$effectnum 2

	char strname[32];
	ZeroMemory( strname, sizeof( char ) * 32 );
	ret = Read_Str( bnkbuf, "$name ", strname, 32 );
	if( ret ){
		DbgOut( "SndSetFile : ReadSoundSet : Read_Str name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	char strdirname[MAX_PATH];
	ZeroMemory( strdirname, sizeof( char ) * MAX_PATH );
	ret = Read_Str( bnkbuf, "$dirname ", strdirname, MAX_PATH );
	if( ret ){
		DbgOut( "SndSetFile : ReadSoundSet : Read_Str dirname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int effectnum = 0;
	ret = Read_Int( bnkbuf, "$effectnum ", &effectnum );
	if( ret || (effectnum < 0) ){
		DbgOut( "SndSetFile : ReadSoundSet : Read_Int effectnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	CSoundSet* newss = 0;
	int ssid = -1;
	newss = m_sndbnk->AddSoundSet( strname, &ssid );
	if( !newss || (ssid < 0) ){
		DbgOut( "SndSetFile : ReadSoundSet : sndbnk AddSoundSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newss->SetDir( strdirname );
	if( ret ){
		DbgOut( "SndSetFile : ReadSoundSet : ss SetDir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newss->SetSSFDir( m_ssfdir );
	if( ret ){
		DbgOut( "SndSetFile : ReadSoundSet : ss SetSSFDir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( bnkbuf->buf, strbgmstart );
	endptr = strstr( bnkbuf->buf, strbgmend );

	if( !startptr || !endptr ){
		DbgOut( "SndSetFile : ReadSoundSet : bgm section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - bnkbuf->buf );
	if( (chkendpos >= (int)bnkbuf->bufleng) || (endptr < startptr) ){
		DbgOut( "SndSetFile : ReadSoundSet : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SNDSETBUF bgmbuf;
	bgmbuf.buf = startptr;
	bgmbuf.bufleng = (int)( endptr - startptr );
	bgmbuf.pos = 0;
	bgmbuf.isend = 0;

	ret = ReadBGM( &bgmbuf, newss );
	if( ret ){
		DbgOut( "SndSetFile : ReadSoundSet : ReadBGM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int eno;
	for( eno = 0; eno < effectnum; eno++ ){
		ret = ReadEffect( bnkbuf, newss );
		if( ret ){
			DbgOut( "SndSetFile : ReadSoundSet : ReadEffect %d error !!!\n", eno );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CSndSetFile::ReadBGM( SNDSETBUF* bgmbuf, CSoundSet* dstsst )
{
//	#BGM Start
//		$name "test.ogg"
//		$reverb 1
//		$bufnum 1
//	#BGM End

	if( !m_sndbnk || !m_hs ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	char strname[MAX_PATH];
	ZeroMemory( strname, sizeof( char ) * MAX_PATH );
	ret = Read_Str( bgmbuf, "$name ", strname, MAX_PATH );
	if( ret ){
		DbgOut( "SndSetFile : ReadBGM : Read_Str name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int reverb = 0;
	ret = Read_Int( bgmbuf, "$reverb ", &reverb );
	if( ret || (reverb < 0) ){
		DbgOut( "SndSetFile : ReadBGM : Read_Int reverb error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int bufnum = 1;
//	ret = Read_Int( bgmbuf, "$bufnum ", &bufnum );
//	if( ret || (bufnum <= 0) ){
//		DbgOut( "SndSetFile : ReadBGM : Read_Int bufnum error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	CSoundElem* newse;
	newse = dstsst->AddSound( m_hs, strname, 1, reverb, bufnum );
	if( !newse ){
		DbgOut( "SndSetFile : ReadBGM : ss AddSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndSetFile::ReadEffect( SNDSETBUF* setbuf, CSoundSet* dstsst )
{
	if( !m_sndbnk || !m_hs ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( setbuf->buf + setbuf->pos, streffectstart );
	endptr = strstr( setbuf->buf + setbuf->pos, streffectend );

	if( !startptr || !endptr ){
		DbgOut( "SndSetFile : ReadEffect : key section pattern not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int chkendpos;
	chkendpos = (int)( endptr - setbuf->buf );
	if( (chkendpos >= (int)setbuf->bufleng) || (endptr < startptr) ){
		DbgOut( "SndSetFile : ReadEffect : endmark error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SNDSETBUF efbuf;
	efbuf.buf = startptr;
	efbuf.bufleng = (int)( endptr - startptr );
	efbuf.pos = 0;
	efbuf.isend = 0;
	
//	#Effect Start
//		$name "beep.wav"
//		$reverb 0
//		$bufnum 5
//	#Effect End


	char strname[MAX_PATH];
	ZeroMemory( strname, sizeof( char ) * MAX_PATH );
	ret = Read_Str( &efbuf, "$name ", strname, MAX_PATH );
	if( ret ){
		DbgOut( "SndSetFile : ReadEffecct : Read_Str name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int reverb = 0;
	ret = Read_Int( &efbuf, "$reverb ", &reverb );
	if( ret || (reverb < 0) ){
		DbgOut( "SndSetFile : ReadEffect : Read_Int reverb error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int bufnum = 0;
	ret = Read_Int( &efbuf, "$bufnum ", &bufnum );
	if( ret || (bufnum <= 0) ){
		DbgOut( "SndSetFile : ReadEffect : Read_Int bufnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundElem* newse;
	newse = dstsst->AddSound( m_hs, strname, 0, reverb, bufnum );
	if( !newse ){
		DbgOut( "SndSetFile : ReadBGM : ss AddSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int nextpos;
	nextpos = chkendpos + (int)strlen( streffectend );
	setbuf->pos = nextpos;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return 0;
}





int CSndSetFile::DeleteSoundSet( CSoundSet* delss )
{
	int ret;
	if( m_sndbnk ){
		ret = m_sndbnk->DelSoundSet( delss->m_setid );
		_ASSERT( !ret );
	}
	return 0;
}

int CSndSetFile::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
int CSndSetFile::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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

int CSndSetFile::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
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
		if( (endpos - startpos) != 0 ){
			_ASSERT( 0 );
		}
	}


	return 0;
}



int CSndSetFile::Read_Int( SNDSETBUF* sstbuf, char* srcpat, int* dstint )
{
	int ret;
	char* findpat;
	findpat = strstr( sstbuf->buf + sstbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "SndSetFile : Read_Int : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (sstbuf->buf + sstbuf->bufleng) ){
			findend = 1;
			_ASSERT( 0 );
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "SndSetFile : Read_Int : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "SndSetFile : Read_Int : %s GetInt error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndSetFile::Read_Float( SNDSETBUF* sstbuf, char* srcpat, float* dstfloat )
{
	int ret;
	char* findpat;
	findpat = strstr( sstbuf->buf + sstbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "SndSetFile : Read_Float : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (sstbuf->buf + sstbuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "SndSetFile : Read_Float : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "SndSetFile : Read_Int : %s GetFloat error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndSetFile::Read_Str( SNDSETBUF* sstbuf, char* srcpat, char* dststr, int arrayleng )
{
	int ret;
	char* findpat;
	findpat = strstr( sstbuf->buf + sstbuf->pos, srcpat );
	if( !findpat ){
		DbgOut( "SndSetFile : Read_Str : %s pattern not found error !!!\n", srcpat );
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
		if( (srcchar + srcleng) >= (sstbuf->buf + sstbuf->bufleng) ){
			findend = 1;
			break;
		}
		srcleng++;
	}

	if( srcleng <= 0 ){
		DbgOut( "SndSetFile : Read_Str : %s value not found error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		DbgOut( "SndSetFile : Read_Str : %s GetName error !!!\n", srcpat );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




int CSndSetFile::LoadSndSetFile( char* filename )
{
	int ret;

	if( !m_sndbnk ){
		_ASSERT( 0 );
		return 1;
	}
	m_mode = SSTFILE_LOAD;

	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "SndSetFile : LoadSndSetFile : CreateFile : INVALID_HANDLE_VALUE error !!!\n" );
		//return D3DAPPERR_MEDIANOTFOUND;//!!!!!!!!
		return 1;
	}	

	char* lasten;
	int ch = '\\';
	lasten = strrchr( filename, ch );
	if( !lasten ){
		DbgOut( "SndSetFile : LoadSndSetFile : path error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int dirleng;
	dirleng = (int)( lasten - filename + 1 );
	strncpy_s( m_ssfdir, MAX_PATH, filename, dirleng );
	m_ssfdir[dirleng] = 0;


	ret = SetBuffer();
	if( ret ){
		DbgOut( "SndSetFile : LoadSndSetFile : SetBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CheckFileVersion( &m_loadversion );
	if( ret || (m_loadversion == 0) ){
		DbgOut( "SndSetFile : LoadSndSetFile : CheckFileVersion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ssnum = 0;
	ret = ReadSoundSetNum( &ssnum );
	if( ret ){
		DbgOut( "SndSetFile : LoadSndSetFile : ReadSoundSetNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ssno;
	for( ssno = 0; ssno < ssnum; ssno++ ){
		char* startptr = 0;
		char* endptr = 0;
		startptr = strstr( m_sstbuf.buf + m_sstbuf.pos, strsetstart );
		endptr = strstr( m_sstbuf.buf + m_sstbuf.pos, strsetend );

		if( !startptr || !endptr ){
			DbgOut( "SndSetFile : LoadSndSetFile : section pattern not found error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int chkendpos;
		chkendpos = (int)( endptr - m_sstbuf.buf );
		if( (chkendpos >= (int)m_sstbuf.bufleng) || (endptr < startptr) ){
			DbgOut( "SndSetFile : LoadSndSetFile : endmark error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		SNDSETBUF bnkbuf;
		bnkbuf.buf = startptr;
		bnkbuf.bufleng = (int)( endptr - startptr );
		bnkbuf.pos = 0;
		bnkbuf.isend = 0;

		ret = ReadSoundSet( &bnkbuf );
		if( ret ){
			DbgOut( "SndSetFile : LoadSndSetFile : ReadSoundSet error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_sstbuf.pos = chkendpos + (int)strlen( strsetend );//!!!!!!!!!!!
	}

	return 0;
}
