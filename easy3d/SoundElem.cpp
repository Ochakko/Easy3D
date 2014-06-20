#include "stdafx.h"

#include <stdio.h>

#include <coef.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>

#include "SoundElem.h"

#include "HuSound.h"
#include <SndPath.h>


CSoundElem::CSoundElem( CHuSound* phs )
{
	InitParams();
	_ASSERT( phs );
	m_hs = phs;
}
CSoundElem::~CSoundElem()
{
	DestroyObjs();
}

int CSoundElem::InitParams()
{
//	char m_filename[256];//フォルダ名なしのファイル名のみ
//	int m_serialno;
//	int m_setid;
//	int m_bgmflag;
//	int m_bufnum;

	ZeroMemory( m_filename, sizeof( char ) * 256 );

	m_serialno = -1;
	m_setid = -1;
	m_bgmflag = 0;
	m_bufnum = 1;
	m_reverbflag = 1;
	m_totaltime = 0.0;

	return 0;
}
int CSoundElem::DestroyObjs()
{
	if( m_hs && (m_serialno >= 0 ) ){
		int ret;
		ret = m_hs->DestroySound( m_serialno );
		if( ret ){
			DbgOut( "soundelem : DestroyObjs : hs DestroySound error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		m_serialno = -1;
	}

	return 0;
}


int CSoundElem::LoadSound( int setid, char* dirname, char* ssfdir, char* filename, 
	int bgmflag, int reverbflag, int bufnum, int* dstseriptr )
{
	int ret, ret2;
	
	ret = DestroyObjs();
	_ASSERT( !ret );
	
	m_setid = setid;
	m_bufnum = bufnum;
	m_reverbflag = reverbflag;

	strcpy_s( m_filename, 256, filename );

	int soundid = -1;
	char pathname[MAX_PATH];
	ZeroMemory( pathname, sizeof( char ) * MAX_PATH );

	ret = SndMakePath( dirname, ssfdir, filename, pathname );
	if( ret == 0 ){
		ret2 = m_hs->LoadSoundFile( pathname, 0, m_reverbflag, m_bufnum, &soundid );
		if( ret2 || (soundid < 0) ){
			DbgOut( "soundelem : LoadSound : hs LoadSoundFile error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		DbgOut( "soundelem : LoadSound : SndMakePath error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_serialno = soundid;
	*dstseriptr = soundid;

	ret = m_hs->GetTotalTime( m_serialno, &m_totaltime );
	_ASSERT( !ret );

	return 0;
}

int CSoundElem::GetSamplesPerSec( DWORD* dstsample )
{
	if( !m_hs ){
		_ASSERT( 0 );
		*dstsample = 0;
		return 1;
	}

	int ret;
	ret = m_hs->GetSamplesPerSec( m_serialno, dstsample );
	_ASSERT( !ret );

	return 0;
}

int CSoundElem::GetSamplesPlayed( double* dstsample )
{
	if( !m_hs ){
		_ASSERT( 0 );
		*dstsample = 0;
		return 1;
	}

	int ret;
	ret = m_hs->GetSoundSamplesPlayed( m_serialno, dstsample );
	_ASSERT( !ret );

	return 0;
}