#include "stdafx.h"

//#include "DMUtil_ed.h"
//#include "DXUTsound.h"

#include "SoundPrim.h"

#include "./XAudioSource/MultiWave.h"

#include <wchar.h>
#include <tchar.h>
#include <string.h>

#include <new.h>      // for placement new
#include <math.h>      
#include <limits.h>      
#include <stdio.h>

#include <atlbase.h>

#include <crtdbg.h>

#define DBGH
#include <dbg.h>

//// static
static int s_allocno = 0;


CSoundPrim::CSoundPrim()
{
	InitParams();

	s_allocno++;
	serialno = s_allocno;
//	m_pMusicManager = pmm;
//	m_pSoundManager = psm;
//	m_pStereoAudioPath = psp;


}
CSoundPrim::~CSoundPrim()
{
	FinalCleanUp();
}


int CSoundPrim::InitParams()
{
	is3d = 0;

//	m_pMusicManager = 0;
//	m_pSoundManager = 0;
//	m_pMS = 0;
//	m_pS = 0;
	m_mwave = 0;

	m_lvolume = 0;
	m_dwfreq = 22050;
	m_dwloop = 0;////DSBPLAY_LOOPING

	savebuf = 0;

	next = 0;

	return 0;
}
int CSoundPrim::FinalCleanUp()
{
	if( m_mwave ){
		delete m_mwave;
		m_mwave = 0;
	}

//	Sleep( 500 );

	if( savebuf ){
		free( savebuf );
		savebuf = 0;
	}

	return 0;
}

int CSoundPrim::LoadSoundFile( char* fname, int use3dflag, int reverbflag, int bufnum, int* soundid )
{
	// pathなしファイル名のみ
//	HRESULT hr;
	int ret;

DbgOut( "HuSound : SoundPrim : LoadSoundFile : %s, %d\n", fname, use3dflag );


	is3d = use3dflag;
	*soundid = serialno;

	// For DirectMusic must know if the file is a standard MIDI file or not
	// in order to load the correct instruments.
	BOOL bMidiFile = FALSE;
	if( _tcsstr( fname, ".mid" ) != NULL ||
		_tcsstr( fname, ".rmi" ) != NULL ) 
	{
		bMidiFile = TRUE;
		DbgOut( "HuSound : SoundPrim : LoadSoundFile : mid, rmi not supported error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// Load the file into a DirectMusic segment, but don't download 
	// it to the default audio path -- use the 3D audio path instead

	m_mwave = new CMultiWave( use3dflag, reverbflag );
	if( !m_mwave ){
		DbgOut( "HuSound : SroundPrim : LoadSoundFile mwave alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "HuSound : SoundPrim : LoadSoundFile : load 0\r\n" );
	ret = m_mwave->LoadSound( fname, bufnum );
	if( ret ){
		DbgOut( "HuSound : SoundPrim : LoadSoundFile mwave LoadSound error !!!\n" );
		_ASSERT( 0 );
		delete m_mwave;//!!!!!!!!!!
		m_mwave = 0;
		return 1;
	}
//DbgOut( "HuSound : SoundPrim : LoadSoundFile : load 1\r\n" );


//	int ret;
//	ret = m_pS->GetFrequency( &m_dwfreq );
//	if( ret ){
//		DbgOut( "HuSound : SoundPrim : LoadSoundFile : ps GetFrequency error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}



	return 0;
}

int CSoundPrim::LoadSoundFileFromBuf( char* buf, int bufsize, int type, int use3dflag, int reverbflag, int bufnum )
{
	// pathなしファイル名のみ
//	HRESULT hr;
	int ret;




	is3d = use3dflag;
//	*soundid = serialno;

	m_mwave = new CMultiWave( use3dflag, reverbflag );
	if( !m_mwave ){
		DbgOut( "HuSound : SroundPrim : LoadSoundFile mwave alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mwave->LoadSoundFromBuf( buf, bufsize, type, bufnum );
	if( ret ){
		DbgOut( "HuSound : SoundPrim : LoadSoundFile mwave LoadSoundFromBuf error !!!\n" );
		_ASSERT( 0 );
		delete m_mwave;//!!!!!!!!!!!!
		m_mwave = 0;
		return 1;
	}

	return 0;
}


/***
int CSoundPrim::SetStartPointOfSound( int sec )
{
	HRESULT hr;

	if( is3d ){
		hr = m_pS->SetStartPointOfSound( sec );
	}else{
		hr = m_pMS->SetStartPointOfSound( sec );
	}

	if( FAILED( hr ) ){
		DbgOut( "HuSound : SoundPrim : SetStartPointOfSound error \n" );
		_ASSERT( 0 );
        return 1;
	}

	return 0;
}
***/

int CSoundPrim::IsSoundPlaying( int* playingptr )
{
	*playingptr = m_mwave->IsPlaying();
	return 0;
}


int CSoundPrim::Play( int startsample, int openo, int numloops )
{
	int ret;
	ret = m_mwave->Play( startsample, openo, numloops );
	if( ret ){
		DbgOut( "HuSound : SoundPrim : PlaySound : play error \n" );
		_ASSERT( 0 );
        return 1;
	}

	return 0;
}

int CSoundPrim::Stop()
{
	int ret;
	ret = m_mwave->Stop();

	if( ret != 0 )
		return 1;

	return 0;
}
/***
int CSoundPrim::Unload()
{
	if( m_mwave ){
		delete m_mwave;
		m_mwave = 0;
	}
	return 0;
}
***/

int CSoundPrim::SetVolume( const float Vol )
{
	int ret;
	if( m_mwave ){
		ret = m_mwave->SetVolume( Vol );
		if( ret ){
			DbgOut( "HuSound : SoundPrim : SetVolume : mw SetVolume error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CSoundPrim::SetFrequency( float freq )
{
	
	int ret;
	ret = m_mwave->SetFreqRatio( freq );
	if( ret ){
		DbgOut( "HuSound : SoundPrim : SetFrequency : mwave SetFrequency error \n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CSoundPrim::GetDopplerScaler( float* dopplerptr )
{
	int ret;
	ret = m_mwave->GetDopplerScaler( dopplerptr );
	if( ret ){
		DbgOut( "HuSound : SoundPrim : GetDopplerScaler : mwave GetDopplerScaler error \n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSoundPrim::SetDopplerScaler( float srcdoppler )
{
	int ret;
	ret = m_mwave->SetDopplerScaler( srcdoppler );
	if( ret ){
		DbgOut( "HuSound : SoundPrim : SetDopplerScaler : mwave SetDopplerScaler error \n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

/***
int CSoundPrim::GetMusicTime( int* mtimeptr, double* rtimeptr )
{
	HRESULT hr;

	if( is3d ){
		*mtimeptr = 0;
	}else{
		hr = m_pMS->GetMusicTime( mtimeptr, rtimeptr );
		if( hr != DS_OK ){
			DbgOut( "HuSound : SoundPrim : GetMusicTime : ms GetMusicTime error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
***/

int CSoundPrim::GetSoundSamplesPlayed( double* psmp )
{
	*psmp = 0.0;
	if( m_mwave ){
		int ret;
		ret = m_mwave->GetSamplesPlayed( psmp );
		_ASSERT( !ret );
	}

	return 0;
}

int CSoundPrim::GetVolume( float* pvol )
{
	*pvol = 0.0f;
	if( m_mwave ){
		*pvol = m_mwave->GetVolume();
	}
	return 0;
}


int CSoundPrim::GetFrequency( float* pfreq )
{
	*pfreq = m_mwave->m_freqratio;

	return 0;
}

/***
int CSoundPrim::Set3DDistance( float fMinDistance, float fMaxDistance )
{
	if( is3d ){
		int ret;
		ret = m_pS->Set3DDistance( fMinDistance, fMaxDistance );
		if( ret ){
			DbgOut( "HuSound : SoundPrim : Set3DDistance : pS Set3DDistance error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "HuSound : SoundPrim : Set3DDistance : this sound is not 3D sound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
***/

int CSoundPrim::SetEmitterParams( D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity )
{
	int ret;
	ret = m_mwave->SetEmitterParams( *pvPosition, *pvVelocity );
	if( ret ){
		DbgOut( "HuSound : SoundPrim : Set3DMovement : this sound is not 3D sound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CSoundPrim::SetListenerParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel, D3DXVECTOR3 dir, D3DXVECTOR3 up )
{
	int ret;
	ret = m_mwave->SetListenerParams( pos, vel, dir, up );
	_ASSERT( !ret );

	return 0;
}

int CSoundPrim::Update()
{
	int ret;
	if( m_mwave ){
		ret = m_mwave->Update();
		if( ret ){
			DbgOut( "SoundPrim : Update error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CSoundPrim::Set3DEmiDist( float dist )
{
	int ret;
	if( m_mwave ){
		ret = m_mwave->Set3DEmiDist( dist );
		if( ret ){
			DbgOut( "SoundPrim : Set3DEmiDist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CSoundPrim::Fade( int msec, float begingain, float endgain )
{
	int ret;
	if( m_mwave ){
		ret = m_mwave->Fade( msec, begingain, endgain );
		if( ret ){
			DbgOut( "SoundPrim : Set3DEmiDist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CSoundPrim::GetStopCnt( int* cntptr )
{
	if( m_mwave ){
		m_mwave->GetStopCnt( cntptr );
	}else{
		*cntptr = 0;
	}
	return 0;
}
int CSoundPrim::GetStartCnt( int* cntptr )
{
	if( m_mwave ){
		m_mwave->GetStartCnt( cntptr );
	}else{
		*cntptr = 0;
	}
	return 0;
}

int CSoundPrim::GetTotalTime( double* dsttime )
{
	if( m_mwave ){
		*dsttime = m_mwave->m_totaltime;
	}else{
		*dsttime = 0.0;
	}
	return 0;
}

int CSoundPrim::GetSamplesPerSec( DWORD* dstsample )
{
	if( m_mwave ){
		m_mwave->GetSamplesPerSec(  dstsample );
	}else{
		*dstsample = 0;
	}

	return 0;
}

