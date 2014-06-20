#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <coef.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>

#include "SoundSet.h"
#include "SoundElem.h"
#include "HuSound.h"

static int s_alloccnt = 0;


CSoundSet::CSoundSet()
{
	InitParams();
}

CSoundSet::~CSoundSet()
{
	DestroyObjs();
}
int CSoundSet::InitParams()
{
//	char m_setname[32];//音セットの名前
//	int m_setid;//音セットのID
//	char m_dirname[MAX_PATH];//相対パスのフォルダ名
//
//	CSoundElem* m_bgm;
//	int m_soundnum;
//	CSoundElem* m_ppsound;

	ZeroMemory( m_setname, sizeof( char ) * 32 );
	m_setid = -1;
	ZeroMemory( m_dirname, sizeof( char ) * MAX_PATH );
	ZeroMemory( m_ssfdir, sizeof( char ) * MAX_PATH );

	m_bgm = 0;
	m_soundnum = 0;
	m_ppsound = 0;

	return 0;
}
int CSoundSet::DestroyObjs()
{
	DestroyBGM();
	DestroySound();

	return 0;
}
int CSoundSet::DestroyBGM()
{
	if( m_bgm ){
		delete m_bgm;
		m_bgm = 0;
	}

	return 0;
}
int CSoundSet::DestroySound()
{
	if( m_ppsound ){
		int sno;
		for( sno = 0; sno < m_soundnum; sno++ ){
			CSoundElem* delse;
			delse = *( m_ppsound + sno );
			delete delse;
		}
		free( m_ppsound );
		m_ppsound = 0;
	}
	m_soundnum = 0;

	return 0;
}

int CSoundSet::CreateNewSet( char* srcname, int* dstsetid )
{
	DestroyObjs();

	strcpy_s( m_setname, 32, srcname );
//	strcpy_s( m_dirname, MAX_PATH, dirname );

	s_alloccnt++;
	m_setid = s_alloccnt;
	*dstsetid = s_alloccnt;

	return 0;
}

int CSoundSet::SetName( char* srcname )
{
	ZeroMemory( m_setname, sizeof( char ) * 32 );
	strcpy_s( m_setname, 32, srcname );

	return 0;
}
int CSoundSet::GetName( char* dstname, int arrayleng )
{
	strcpy_s( dstname, arrayleng, m_setname );

	return 0;
}

int CSoundSet::SetDir( char* srcdir )
{

	ZeroMemory( m_dirname, sizeof( char ) * MAX_PATH );
	strcpy_s( m_dirname, MAX_PATH, srcdir );

	return 0;
}
int CSoundSet::GetDir( char* dstdir, int arrayleng )
{
	strcpy_s( dstdir, arrayleng, m_dirname );

	return 0;
}

int CSoundSet::SetSSFDir( char* srcdir )
{

	ZeroMemory( m_ssfdir, sizeof( char ) * MAX_PATH );
	strcpy_s( m_ssfdir, MAX_PATH, srcdir );

	return 0;
}


CSoundElem* CSoundSet::AddSound( CHuSound* srchs, char* filename, int bgmflag, int reverbflag, int bufnum )
{
	CSoundElem* newse = 0;
	newse = new CSoundElem( srchs );
	if( !newse ){
		DbgOut( "soundset : AddSound : newse alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	int newseri = -1;
	ret = newse->LoadSound( m_setid, m_dirname, m_ssfdir, filename, bgmflag, reverbflag, bufnum, &newseri );
	if( ret || (newseri < 0 ) ){
		DbgOut( "soundset : AddSound : se LoadSound error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	if( bgmflag == 0 ){
		int newnum = m_soundnum + 1;

		m_ppsound = (CSoundElem**)realloc( m_ppsound, sizeof( CSoundElem* ) * newnum );
		if( !m_ppsound ){
			DbgOut( "soundset : AddSound : ppsound realloc error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}

		*( m_ppsound + newnum - 1 ) = newse;
		m_soundnum = newnum;

	}else{
		DestroyBGM();
		m_bgm = newse;
	}

	return newse;
}

int CSoundSet::DelBGM()
{
	DestroyBGM();

	return 0;
}

int CSoundSet::DelSound( int srcseri )
{
	int findindex = -1;


	if( m_bgm->m_serialno == srcseri ){
		findindex = -2;
	}else{
		int sno;
		for( sno = 0; sno < m_soundnum; sno++ ){
			CSoundElem* curse = *( m_ppsound + sno );
			if( curse->m_serialno == srcseri ){
				findindex = sno;
				break;
			}
		}
	}


	if( findindex == -2 ){
		DestroyBGM();
	}else if( findindex >= 0 ){

		int newnum = m_soundnum - 1;

		CSoundElem** newppse;
		newppse = (CSoundElem**)malloc( sizeof( CSoundElem* ) * newnum );
		if( !newppse ){
			DbgOut( "soundset : DelSound : newppse alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( newppse, sizeof( CSoundSet* ) * newnum );

		int setno = 0;
		int sno;
		for( sno = 0; sno < m_soundnum; sno++ ){
			if( sno != findindex ){
				*( newppse + setno ) = *( m_ppsound + sno );
				setno++;
			}
		}

		if( setno != newnum ){
			DbgOut( "soundset : DelSound : setno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		delete( *( m_ppsound + findindex ) );//!!!!!!!!!!

		free( m_ppsound );
		m_ppsound = newppse;

		m_soundnum = newnum;

	}else{
		_ASSERT( 0 );
	}
	return 0;
}

CSoundElem* CSoundSet::GetSoundElem( int srcseri )
{
	CSoundElem* retse = 0;

	int sno;
	for( sno = 0; sno < m_soundnum; sno++ ){
		CSoundElem* curse = *( m_ppsound + sno );
		if( curse->m_serialno == srcseri ){
			retse = curse;
			break;
		}
	}
	return retse;
}

CSoundElem* CSoundSet::GetSoundElem( char* filename )
{
	CSoundElem* retse = 0;

	int sno;
	for( sno = 0; sno < m_soundnum; sno++ ){
		CSoundElem* curse = *( m_ppsound + sno );
		if( strcmp( curse->m_filename, filename ) == 0 ){
			retse = curse;
			break;
		}
	}
	return retse;
}

int CSoundSet::GetSoundIndex( CSoundElem* srcse, int* dstindex )
{
	*dstindex = -1;
	int findindex = -1;
	int sno;
	for( sno = 0; sno < m_soundnum; sno++ ){
		if( *( m_ppsound + sno ) == srcse ){
			findindex = sno;
			break;
		}
	}

	*dstindex = sno;
	return 0;
}


int CSoundSet::GetSoundIndexByID( int setid, int* dstindex )
{
	int retindex = -1;
	int sno;
	for( sno = 0; sno < m_soundnum; sno++ ){
		CSoundElem* cursnd = *( m_ppsound + sno );
		if( cursnd->m_serialno == setid ){
			retindex = sno;
			break;
		}
	}
	*dstindex = retindex;
	return 0;
}

int CSoundSet::UpSound( int setid )
{
	if( m_soundnum <= 1 ){
		return 0;
	}

	int upindex = -1;
	GetSoundIndexByID( setid, &upindex );
	if( upindex < 0 ){
		DbgOut( "SoundSet : UpSound : GetSoundIndexByID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( upindex == 0 ){
		return 0;//一番上
	}

	CSoundElem** newppsound;
	newppsound = (CSoundElem**)malloc( sizeof( CSoundElem* ) * m_soundnum );
	if( !newppsound ){
		DbgOut( "SoundSet : UpSound : newppsound alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newppsound, sizeof( CSoundElem* ) * m_soundnum );

	int setno = 0;
	int sno;
	for( sno = 0; sno <= (upindex - 2); sno++ ){
		*( newppsound + setno ) = *( m_ppsound + sno );
		setno++;
	}

	*( newppsound + upindex - 1 ) = *( m_ppsound + upindex );
	setno++;

	*( newppsound + upindex ) = *( m_ppsound + upindex - 1 );
	setno++;


	for( sno = (upindex + 1); sno < m_soundnum; sno++ ){
		*( newppsound + setno ) = *( m_ppsound + sno );
		setno++;
	}

	if( setno != m_soundnum ){
		DbgOut( "SoundSet : UpSound : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	free( m_ppsound );
	m_ppsound = newppsound;

	return 0;

}
int CSoundSet::DownSound( int setid )
{
	if( m_soundnum <= 1 ){
		return 0;
	}

	int dindex = -1;
	GetSoundIndexByID( setid, &dindex );
	if( dindex < 0 ){
		DbgOut( "SoundSet : DowmSoundSet : GetSoundIndexByID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( dindex == (m_soundnum - 1) ){
		return 0;//一番下
	}

	CSoundElem** newppsound;
	newppsound = (CSoundElem**)malloc( sizeof( CSoundElem* ) * m_soundnum );
	if( !newppsound ){
		DbgOut( "SoundSet : DownSound : newppsound alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newppsound, sizeof( CSoundElem* ) * m_soundnum );

	int setno = 0;
	int sno;
	for( sno = 0; sno <= (dindex - 1); sno++ ){
		*( newppsound + setno ) = *( m_ppsound + sno );
		setno++;
	}

	*( newppsound + dindex ) = *( m_ppsound + dindex + 1 );
	setno++;

	*( newppsound + dindex + 1 ) = *( m_ppsound + dindex );
	setno++;

	for( sno = (dindex + 2); sno < m_soundnum; sno++ ){
		*( newppsound + setno ) = *( m_ppsound + sno );
		setno++;
	}

	if( setno != m_soundnum ){
		DbgOut( "SoundSet : DownSound : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	free( m_ppsound );
	m_ppsound = newppsound;

	return 0;
}

int CSoundSet::GetBGMTotalTime( double* dsttime )
{
	if( m_bgm ){
		*dsttime = m_bgm->m_totaltime;		
	}else{
		*dsttime = 0.0;
	}
	return 0;
}
