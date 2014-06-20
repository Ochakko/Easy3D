// HuSound.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "HuSound.h"
//#include "DMUtil_ed.h"
//#include "DXUTsound.h"

#include "SoundPrim.h"

//#include "DXUtil.h"

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

#include "./XAudioSource/XAudio.h"
#include "./XAudioSource/audiocoef.h"
#include <xaudio2.h>

#include <SndKey.h>
#include <SndFrame.h>

////////////////////////

static char strcpyright[256] = "Copyright (C) 2004 おちっこLAB　http://www5d.biglobe.ne.jp/~ochikko/";
UINT32 g_ope = 0;

////////////////////////



// これは、エクスポートされたクラスのコンストラクタです。
// クラス定義に関しては HuSound.h を参照してください。
CHuSound::CHuSound()
{ 
	InitParams();
	return; 
}

CHuSound::~CHuSound()
{
	FinalCleanUp();
	return;
}

int CHuSound::InitParams()
{
//	m_pMusicManager          = NULL;
//	m_pStereoAudioPath		= NULL;
//	m_pSoundManager			= NULL;
	m_pSoundPrim          = NULL;


	m_bDeferSettings         = FALSE;
	m_apphwnd = NULL;

	m_searchdir[0] = 0;

	initflag = 0;

	return 0;
}

int CHuSound::FinalCleanUp()
{
	// Cleanup everything

	initflag = 0;

	if( m_pSoundPrim ){
		CSoundPrim* cursp;
		CSoundPrim* delsp;

		cursp = m_pSoundPrim;
		while( cursp ){
			delsp = cursp;
			cursp = cursp->next;

			delsp->Stop();

			delete delsp;
		}

		m_pSoundPrim = 0;
	}

	XAudio::Release();

//	SAFE_RELEASE( m_pStereoAudioPath );
//	SAFE_DELETE( m_pSoundManager );
//	SAFE_DELETE( m_pMusicManager );
	
	return 0;
}

int CHuSound::GetVer()
{
	return 1003;

}

int CHuSound::Init( HWND apphwnd, int enablereverb )
{


	DbgOut( "HuSound : Init : start\n" );

//	m_apphwnd = apphwnd;
//
//	HRESULT hr;
    // Initialize the performance. This initializes both DirectMusic and DirectSound
    // and optionally sets up the synthesizer and default audio path.
    // However, since this app never uses the default audio path, we don't bother 
    // to do that here.

	/***
//MusicManager
	m_pMusicManager = new CMusicManager();
	if( !m_pMusicManager ){
		DbgOut( "HuSound : Init : MusicManager alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	DWORD dwtype;
	if( enablereverb ){
		dwtype = DMUS_APATH_SHARED_STEREOPLUSREVERB;
	}else{
		dwtype = DMUS_APATH_DYNAMIC_STEREO;
	}

	
	if( FAILED( hr = m_pMusicManager->Initialize( m_apphwnd, 128, dwtype ) ) ){
		DbgOut( "HuSound : Init : MusicManager Initialize error %x\n" );
        DXTRACE_ERR( TEXT("Initialize"), hr );
		_ASSERT( 0 );
		return 0;
	}

//AudioPath
	IDirectMusicPerformance8* pPerformance = m_pMusicManager->GetPerformance();
		
	if( FAILED( hr = pPerformance->CreateStandardAudioPath( 
		dwtype, 
		128, TRUE, &m_pStereoAudioPath ) ) )
	{

		DbgOut( "HuSound : Init : performance CreateStandardAudioPath Stereo error \n" );
		DXTRACE_ERR( TEXT("CreateStandardAudioPath"), hr );
		_ASSERT( 0 );
		return 0;
	}

//SoundManager
    m_pSoundManager = new CSoundManager();
    if( NULL == m_pSoundManager )
    {
        DXUT_ERR_MSGBOX( TEXT("Initialize"), E_OUTOFMEMORY );
		DbgOut( "HuSound : Init : SoundManager alloc error !!!\n" );
		_ASSERT( 0 );
        return 0;
    }

    hr = m_pSoundManager->Initialize( m_apphwnd, DSSCL_PRIORITY );
       
    hr |= m_pSoundManager->SetPrimaryBufferFormat( 2, 22050, 16 );
    if( FAILED(hr) )
    {
        //MessageBox( m_apphwnd, "Error initializing DirectSound.  Sample will now exit.", 
        //                  "DirectSound Sample", MB_OK | MB_ICONERROR );
		DbgOut( "HuSound : Init : SetPrimaryBufferFormat error !!!\n" );
		_ASSERT( 0 );
        return 0;
    }


/////////////

	***/
	DbgOut( "HuSound : Init : Success : end\n" );
	initflag = 1;

	return 0;
}


int CHuSound::SetSearchDirectory( TCHAR* dirpath )
{
	if( initflag == 0 ){
		return 0;
	}


	int dirleng;
	dirleng = (int)strlen( dirpath );
	if( dirleng >= MAX_PATH ){
		DbgOut( "HuSound : SetSearchDirectory : dirpath too long error !!!\n" );
		m_searchdir[0] = 0;
		_ASSERT( 0 );
		return 1;
	}else{
		strcpy( m_searchdir, dirpath );
	}

	DbgOut( "HuSound : SetSearchDirectory : %s\n", m_searchdir );

/***
	//最後に\\付
	HRESULT hr;
	hr = m_pMusicManager->SetSearchDirectory( dirpath );
	if( hr != DS_OK ){
		DbgOut( "HuSound : SetSearchDirectory : musicmanager SetSearchDirectory warning \n" );
	}
***/

	return 0;
}


int CHuSound::LoadSoundFileFromBuf( char* buf, int bufsize, int type, int use3dflag, int reverbflag, int bufnum, int* soundidptr )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* newsp;
	newsp = new CSoundPrim();
	if( !newsp ){
		DbgOut( "HuSound : LoadSoundFileFromBuf : soundprim alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	ret = newsp->LoadSoundFileFromBuf( buf, bufsize, type, use3dflag, reverbflag, bufnum );
	if( ret ){
		DbgOut( "HuSound : LoadSoundFileFromBuf : newsp LoadSoundFile error !!!\n" );
		_ASSERT( 0 );
		delete newsp;//!!!!!!!!!!
		return 1;
	}


	newsp->next = m_pSoundPrim;
	m_pSoundPrim = newsp;

	*soundidptr = newsp->serialno;
	_ASSERT( newsp->serialno > 0 );

	DbgOut( "HuSound : LoadSoundFileFromBuf : success : soundid %d\n", *soundidptr );

	return 0;
}


int CHuSound::LoadSoundFile( TCHAR* fname, int use3dflag, int reverbflag, int bufnum, int* soundidptr )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* newsp;
	newsp = new CSoundPrim();
	if( !newsp ){
		DbgOut( "HuSound : LoadSoundFile : soundprim alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	ret = newsp->LoadSoundFile( fname, use3dflag, reverbflag, bufnum, soundidptr );
	if( ret ){
		DbgOut( "HuSound : LoadSoundFile : newsp LoadSoundFile error !!!\n" );
		_ASSERT( 0 );
		delete newsp;//!!!!!!!
		return 1;
	}

	DbgOut( "HuSound : LoadSoundFile : success : soundid %d, name %s\n", *soundidptr, fname );

	newsp->next = m_pSoundPrim;
	m_pSoundPrim = newsp;

	*soundidptr = newsp->serialno;


	return 0;
}

/***
int CHuSound::SetStartPointOfSound( int soundid, int sec )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : SetStartPointOfSound : soundid error \n" );
        return 1;
	}

	int ret;
	ret = cursp->SetStartPointOfSound( sec );
	if( ret ){
		DbgOut( "husound : SetStartPointOfSound : sp SetStartPointOfSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/

int CHuSound::PlaySound( int soundid, int startsample, int openo, int numloops )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : PlaySound : soundid error \n" );
        return 1;
	}

	int ret;

	ret = cursp->Update();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = cursp->Play( startsample, openo, numloops );
	if( ret ){
		DbgOut( "HuSound : PlaySound : cursp Play error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CHuSound::PlayFrame( int bgmid, int bgmsample, int bgmloop, CSndFrame* srcframe )
{
	int ret;
	if( initflag == 0 ){
		return 0;
	}

	g_ope++;

	if( bgmid >= 0 ){
		ret = PlaySound( bgmid, bgmsample, g_ope, bgmloop );
		_ASSERT( !ret );
	}

	if( srcframe ){
		int keyno;
		for( keyno = 0; keyno < srcframe->m_keynum; keyno++ ){
			CSndKey* curkey;
			curkey = *( srcframe->m_ppkey + keyno );
			if( curkey->m_doneflag == 0 ){
				int seno;
				for( seno = 0; seno < curkey->m_sndenum; seno++ ){
					SNDELEM* curse;
					curse = curkey->m_psnde + seno;
					ret = PlaySound( curse->sndid, 0, g_ope, 0 );
					_ASSERT( !ret );
				}
				curkey->m_doneflag = 1;
			}
		}
	}

	XAudio::GetInstance().pXAudio2->CommitChanges( g_ope );

	return 0;
}




CSoundPrim* CHuSound::GetSoundPrim( int soundid )
{
	CSoundPrim* retsp = 0;

	CSoundPrim* cursp = m_pSoundPrim;
	while( cursp ){
		if( cursp->serialno == soundid ){
			retsp = cursp;
			break;
		}
		cursp = cursp->next;
	}

	return retsp;
}

//-----------------------------------------------------------------------------
// Name: Set3DParameters()
// Desc: Set the 3D buffer parameters
//-----------------------------------------------------------------------------
/***
int  CHuSound::Set3DListener( float fDopplerFactor, float fRolloffFactor )
{
	if( initflag == 0 ){
		return 0;
	}

	int ret;
	ret = m_pSoundManager->Set3DListener( fDopplerFactor, fRolloffFactor );
	if( ret ){
		DbgOut( "HuSound : Set3DListener : soundmanager Set3DListener error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
***/

int CHuSound::Set3DListenerMovement( D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity, D3DXVECTOR3* pvOrientFront, D3DXVECTOR3* pvOrientTop )
{
	if( initflag == 0 ){
		return 0;
	}

	int ret;
	CSoundPrim* cursp = m_pSoundPrim;
	while( cursp ){
		ret = cursp->SetListenerParams( *pvPosition, *pvVelocity, *pvOrientFront, *pvOrientTop );
		_ASSERT( !ret );
		cursp = cursp->next;
	}

	return 0;
}


/***
int  CHuSound::Set3DDistance( int soundid, float fMinDistance, float fMaxDistance )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : Set3DDistance : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->Set3DDistance( fMinDistance, fMaxDistance );
	if( ret ){
		DbgOut( "HuSound : Set3DDistance : sp Set3DDistance error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/
//-----------------------------------------------------------------------------
// Name: SetObjectProperties()
// Desc: Sets the position and velocity on the 3D buffer
//-----------------------------------------------------------------------------
int CHuSound::Set3DMovement( int soundid, D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : Set3DMovement : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->SetEmitterParams( pvPosition, pvVelocity );
	if( ret ){
		DbgOut( "HuSound : Set3DMovement : sp Set3DMovement error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHuSound::DestroySound( int soundid )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp = m_pSoundPrim;
	CSoundPrim* befsp = 0;
	CSoundPrim* delsp = 0;
	while( cursp ){
		if( cursp->serialno == soundid ){
			delsp = cursp;
			break;
		}

		befsp = cursp;
		cursp = cursp->next;
	}

	if( !delsp ){
		DbgOut( "HuSound : DestroySound : soundid not found warning \n" );
		_ASSERT( 0 );
		return 0;//!!!
	}

	if( befsp ){
		befsp->next = cursp->next;
	}else{
		//先頭を削除
		m_pSoundPrim = delsp->next; 
	}


	delsp->Stop();
	//delsp->Unload();

	//Sleep( 500 );

	delete delsp;

	return 0;
}
int CHuSound::StopSound( int soundid )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : StopSound : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->Stop();
	if( ret ){
		DbgOut( "HuSound : StopSound : sp Stop error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
/***
int CHuSound::SetRepeats( int soundid, int repeatflag )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : SetRepeats : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->SetRepeats( repeatflag );
	if( ret ){
		DbgOut( "HuSound : SetRepeats : sp SetRepeats error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/
int CHuSound::SetVolume( int soundid, float vol )
{
	if( initflag == 0 ){
		return 0;
	}

	if( soundid > 0 ){

		CSoundPrim* cursp;
		cursp = GetSoundPrim( soundid );
		if( !cursp ){
			DbgOut( "HuSound : SetVolume : soundid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int ret;
		ret = cursp->SetVolume( vol );
		if( ret ){
			DbgOut( "HuSound : SetVolume : sp SetVolume error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		CSoundPrim* cursp;
		cursp = m_pSoundPrim;
		while( cursp ){
			int ret;
			ret = cursp->SetVolume( vol );
			if( ret ){
				DbgOut( "HuSound : SetVolume : sp SetVolume error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			cursp = cursp->next;
		}
	}

	return 0;
}

/***
// 3d sound が操作できなかった。
int CHuSound::SetMasterVolume( LONG vol )
{
	if( initflag == 0 ){
		return 0;
	}

//デフォルトのソフトウェア シンセサイザの場合、許容範囲は +20 ～ -200dB であるが、実用範囲は +10 ～ -100db である。	
	if( vol > 20 )
		vol = 20;
	if( vol < -200 )
		vol = -200;

	IDirectMusicPerformance8* pPerformance = m_pMusicManager->GetPerformance();
	_ASSERT( pPerformance );

	HRESULT hr;
	hr = pPerformance->SetGlobalParam( GUID_PerfMasterVolume, (void*)&vol, sizeof(long) );                    
	if( hr != DS_OK ){
		DbgOut( "HuSound : SetMasterVolume : perf SetGlobalParam error \n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/


int CHuSound::SetFrequency( int soundid, float freq )
{
	if( initflag == 0 ){
		return 0;
	}
	int ret;

	if( soundid < 0 ){
		CSoundPrim* cursp = m_pSoundPrim;
		while( cursp ){
			if( cursp->serialno >= 0 ){
				ret = SetFrequency( cursp->serialno, freq );			
				if( ret ){
					DbgOut( "HuSound : SetFrequency : each error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			cursp = cursp->next;
		}

		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : SetFrequency : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( freq > XAUDIOFREQMAX ){
		freq = XAUDIOFREQMAX;
	}
	if( freq < XAUDIO2_MIN_FREQ_RATIO ){
		freq = XAUDIO2_MIN_FREQ_RATIO;
	}

	ret = cursp->SetFrequency( freq );
	if( ret ){
		DbgOut( "HuSound : SetFrequency : sp SetFrequency error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHuSound::GetDopplerScaler( int soundid, float* dopplerptr )
{
	*dopplerptr = 1.0f;

	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetDopplerScaler : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->GetDopplerScaler( dopplerptr );
	if( ret ){
		DbgOut( "HuSound : GetDopplerScaler : sp GetDopplerScaler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHuSound::SetDopplerScaler( int soundid, float srcdoppler )
{
	if( initflag == 0 ){
		return 0;
	}
	int ret;

	if( soundid >= 0 ){
		CSoundPrim* cursp;
		cursp = GetSoundPrim( soundid );
		if( !cursp ){
			DbgOut( "HuSound : SetDopplerScaler : soundid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = cursp->SetDopplerScaler( srcdoppler );
		if( ret ){
			DbgOut( "HuSound : SetDopplerScaler : sp SetDopplerScaler error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		CSoundPrim* cursp = m_pSoundPrim;
		while( cursp ){
			ret = cursp->SetDopplerScaler( srcdoppler );
			if( ret ){
				DbgOut( "HuSound : SetDopplerScaler : sp SetDopplerScaler error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			cursp = cursp->next;
		}
	}
	return 0;
}


/***
int CHuSound::SetMasterTempo( float ftempo )
{
	if( initflag == 0 )
		return 0;
	
	if( ftempo < DMUS_MASTERTEMPO_MIN )
		ftempo = DMUS_MASTERTEMPO_MIN;
	if( ftempo > DMUS_MASTERTEMPO_MAX )
		ftempo = DMUS_MASTERTEMPO_MAX;

	IDirectMusicPerformance8* pPerformance = m_pMusicManager->GetPerformance();
	if( !pPerformance ){
		DbgOut( "HuSound : SetMasterTempo : performance NULL error \n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	hr = pPerformance->SetGlobalParam( GUID_PerfMasterTempo, (void*)&ftempo, sizeof(float) );                    
	if( hr != DS_OK ){
		DbgOut( "HuSound : SetMasterTempo : perform SetGlobalParam error \n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
//////////////
**/

int CHuSound::IsSoundPlaying( int soundid, int* playingptr )
{
	if( initflag == 0 )
		return 0;

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : IsSoundPlaying : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->IsSoundPlaying( playingptr );
	if( ret ){
		DbgOut( "HuSound : IsSoundPlaying : sp IsSoundPlaying error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHuSound::Update()
{

	CSoundPrim* cursp = m_pSoundPrim;
	while( cursp ){
		int ret;
		ret = cursp->Update();
		_ASSERT( !ret );
		cursp = cursp->next;
	}

	return 0;
}

/***
int CHuSound::GetMusicTime( int soundid, int* mtimeptr, double* rtimeptr )
{
	if( initflag == 0 )
		return 0;

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetMusicTime : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = cursp->GetMusicTime( mtimeptr, rtimeptr );
	if( ret ){
		DbgOut( "husound : GetMusicTime : sp GetMusicTime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
***/

int CHuSound::GetSoundSamplesPlayed( int soundid, double* psmp )
{
	if( initflag == 0 )
		return 0;

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetSoundSamplesPlayed : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->GetSoundSamplesPlayed( psmp );
	if( ret ){
		DbgOut( "HuSound : GetSoundSamplesPlayed : sp GetSoundSamplesPlayed error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHuSound::GetVolume( int soundid, float* pvol )
{
	if( initflag == 0 )
		return 0;

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetVolume : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = cursp->GetVolume( pvol );
	if( ret ){
		DbgOut( "HuSound : GetVolume : sp GetVolume error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHuSound::GetFrequency( int soundid, float* pfreq )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetFrequency : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->GetFrequency( pfreq );
	if( ret ){
		DbgOut( "HuSound : GetFrequency : sp GetFrequency error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHuSound::GetStopCnt( int soundid, int* cntptr )
{
	*cntptr = 0;

	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetStopCnt : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->GetStopCnt( cntptr );
	_ASSERT( !ret );

	return 0;
}

int CHuSound::GetStartCnt( int soundid, int* cntptr )
{
	*cntptr = 0;

	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetStartCnt : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->GetStartCnt( cntptr );
	_ASSERT( !ret );

	return 0;
}

int CHuSound::GetTotalTime( int soundid, double* dsttime )
{
	*dsttime = 0.0;

	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetTotalTime : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->GetTotalTime( dsttime );
	_ASSERT( !ret );

	return 0;
}

int CHuSound::GetSamplesPerSec( int soundid, DWORD* dstsample )
{
	*dstsample = 0;

	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : GetSamplesPerSec : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->GetSamplesPerSec( dstsample );
	_ASSERT( !ret );

	return 0;
}


int CHuSound::Set3DEmiDist( int soundid, float dist )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : Set3DEmiDist : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->Set3DEmiDist( dist );
	if( ret ){
		DbgOut( "HuSound : Set3DEmiDist : sp Set3DEmiDist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHuSound::Fade( int soundid, int msec, float begingain, float endgain )
{
	if( initflag == 0 ){
		return 0;
	}

	CSoundPrim* cursp;
	cursp = GetSoundPrim( soundid );
	if( !cursp ){
		DbgOut( "HuSound : Set3DEmiDist : soundid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursp->Fade( msec, begingain, endgain );
	if( ret ){
		DbgOut( "HuSound : sp Fade error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


/***
int CHuSound::GetMasterTempo( float* pftempo )
{
	//一度もSetMasterTempoしていない状態で呼ぶとエラーになる。
	if( initflag == 0 )
		return 0;

	IDirectMusicPerformance8* pPerformance = m_pMusicManager->GetPerformance();
	if( !pPerformance ){
		DbgOut( "HuSound : GetMasterTempo : performance NULL error \n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	hr = pPerformance->GetGlobalParam( GUID_PerfMasterTempo, (void*)pftempo, sizeof(float) );                    
	if( hr != DS_OK ){
		DbgOut( "HuSound : GetMasterMaster : perform GetGlobalParam error \n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/


