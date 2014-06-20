#include "stdafx.h"
#include "MultiWave.h"
#include "XAudioWave.h"
#include "XAudioVorbis.h"

#include <stdio.h>
#include <crtdbg.h>

//#include <mmreg.h> //WAVEFORMATEX

#define DBGH
#include "dbg.h"

#pragma warning( disable: 4996 )


CMultiWave::CMultiWave( int srcflag3d, int srcreverbflag )
{
	InitParams();
	flag3d = srcflag3d;
	flagreverb = srcreverbflag;
	
}
CMultiWave::~CMultiWave()
{
	DestroyObjs();
}

int CMultiWave::InitParams()
{
//	int m_sourcenum;
//	CXAudioWave*	m_source;
//	int* m_useflag;
//	int m_cursourceno;
	oggflag = 0;
	m_sourcenum = 0;
	m_source = 0;
	m_oggsource = 0;

	m_useflag = 0;
	m_cursourceno = 0;
	flag3d = 0;

	m_totaltime = 0.0;

	m_stopcnt = 0;
	m_startcnt = 0;

	return 0;
}
int CMultiWave::DestroyObjs()
{
	if( m_source ){
		delete [] m_source;
//_ASSERT( 0 );
		m_source = 0;
	}
	if( m_oggsource ){
		delete [] m_oggsource;
		m_oggsource = 0;
	}
	m_sourcenum = 0;

	if( m_useflag ){
		free( m_useflag );
		m_useflag = 0;
	}
//_ASSERT( 0 );

	m_cursourceno = 0;

	return 0;
}

int CMultiWave::LoadSoundFromBuf( char* buf, int bufsize, int type, int sourcenum )
{
	DestroyObjs();

	if( (sourcenum <= 0) || (sourcenum > MAXMULTIWAVE) ){
		_ASSERT( 0 );
		return 1;
	}

	oggflag = 0;

	m_useflag = (int*)malloc( sizeof( int ) * sourcenum );
	if( !m_useflag ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_useflag, sizeof( int ) * sourcenum );


	if( oggflag == 0 ){
		m_source = new XAudioWave[ sourcenum ];
		if( !m_source ){
			_ASSERT( 0 );
			return 1;
		}
		int sno;
		for( sno = 0; sno < sourcenum; sno++ ){
			( m_source + sno )->SetFlag3D( flag3d );
			( m_source + sno )->flagreverb = flagreverb;
			( m_source + sno )->m_stopcntptr = &m_stopcnt;
			( m_source + sno )->m_startcntptr = &m_startcnt;
		}

		int ret;
		for( sno = 0; sno < sourcenum; sno++ ){
			ret = (m_source + sno)->LoadFromBuf( buf, bufsize );
			if( ret != 0 ){
				_ASSERT( 0 );
				DestroyObjs();//!!!!!!!!!!!!
				return 1;
			}
		}

		CalcTotalTime( m_source->WaveSize, &(m_source->m_WaveFormatEx) );
	}else{
		::MessageBox( NULL, "oggはバッファから読めません。", "エラー", MB_OK );
		_ASSERT( 0 );
		DestroyObjs();//!!!!!!!!!!!!!!
		return 1;
	}


	m_sourcenum = sourcenum;


	return 0;
}


int CMultiWave::LoadSound( char* filename, int sourcenum )
{
	DestroyObjs();

	if( (sourcenum <= 0) || (sourcenum > MAXMULTIWAVE) ){
		_ASSERT( 0 );
		return 1;
	}

	oggflag = 0;
	char* extptr = 0;
	int ch = '.';
	extptr = strrchr( filename, ch );
	if( extptr ){
		int cmp;
		cmp = strcmp( extptr, ".ogg" );
		if( cmp == 0 ){
			oggflag = 1;
		}
	}

	m_useflag = (int*)malloc( sizeof( int ) * sourcenum );
	if( !m_useflag ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_useflag, sizeof( int ) * sourcenum );

	if( oggflag == 0 ){
		m_source = new XAudioWave[ sourcenum ];
		if( !m_source ){
			_ASSERT( 0 );
			return 1;
		}
		int sno;
		for( sno = 0; sno < sourcenum; sno++ ){
			( m_source + sno )->SetFlag3D( flag3d );
			( m_source + sno )->flagreverb = flagreverb;
			( m_source + sno )->m_stopcntptr = &m_stopcnt;
			( m_source + sno )->m_startcntptr = &m_startcnt;
		}

		bool ret;
		for( sno = 0; sno < sourcenum; sno++ ){
			ret = (m_source + sno)->Load( filename );
			if( ret == false ){
				DbgOut( "multiwave : LoadSound : source load %s  %d error !!!\r\n", filename, sno );
				_ASSERT( 0 );
				DestroyObjs();//!!!!!!!!!!!!
				return 1;
			}
		}

		CalcTotalTime( m_source->WaveSize, &(m_source->m_WaveFormatEx) );

	}else{
		m_oggsource = new XAudioVorbis[ sourcenum ];
		if( !m_oggsource ){
			_ASSERT( 0 );
			return 1;
		}
		int sno;
		for( sno = 0; sno < sourcenum; sno++ ){
			( m_oggsource + sno )->SetFlag3D( flag3d );
			( m_oggsource + sno )->flagreverb = flagreverb;
			( m_oggsource + sno )->m_stopcntptr = &m_stopcnt;
			( m_oggsource + sno )->m_startcntptr = &m_startcnt;
		}

		bool ret;
		for( sno = 0; sno < sourcenum; sno++ ){
			ret = (m_oggsource + sno)->Load( filename );
			if( ret == false ){
				_ASSERT( 0 );
				DestroyObjs();//!!!!!!!!!!!
				return 1;
			}
		}

		CalcTotalTime( m_oggsource->WaveSize, &(m_oggsource->m_WaveFormatEx) );
	}


	m_sourcenum = sourcenum;

//DbgOut( "multiwave : LoadSound : load 3\r\n" );
	return 0;
}

int CMultiWave::GetFreeSourceNo()
{
	int freeno = -1;

	int sno;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		STATUS st;
		if( oggflag == 0 ){
			st = ( m_source + sno )->GetStatus();
		}else{
			st = ( m_oggsource + sno )->GetStatus();
		}

		if( (st != STATUS_PLAYING) || (*( m_useflag + sno ) == 0) ){
			*( m_useflag + sno ) = 1;
			freeno = sno;
			break;
		}
	}

	return freeno;
}

int CMultiWave::Play( UINT32 startsample, int openo, int loopcnt )
{
	_ASSERT( m_source || m_oggsource );
	_ASSERT( m_useflag );

	int freeno;
	freeno = GetFreeSourceNo();
	if( freeno < 0 ){
		_ASSERT( 0 );
		return 0;
	}

	if( oggflag == 0 ){
		( m_source + freeno )->FlushSourceBuffers();
		( m_source + freeno )->Play( startsample, loopcnt );
	}else{
		( m_oggsource + freeno )->FlushSourceBuffers();
		( m_oggsource + freeno )->Play( startsample, loopcnt );
	}
	return 0;
}
int CMultiWave::Stop()
{
	int sno;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		STATUS st;
		if( oggflag == 0 ){
			st = ( m_source + sno )->GetStatus();
		}else{
			st = ( m_oggsource + sno )->GetStatus();
		}

		if( st == STATUS_PLAYING ){		
//		if( *( m_useflag + sno ) == 1 ){
			if( oggflag == 0 ){
				( m_source + sno )->Stop();
			}else{
				( m_oggsource + sno )->Stop();
			}
			//*( m_useflag + sno ) = 0;
		}
	}

	return 0;
}

int CMultiWave::SetVolume( const float Volume )
{
	int sno;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		if( oggflag == 0 ){
			( m_source + sno )->SetVolume( Volume );
		}else{
			( m_oggsource + sno )->SetVolume( Volume );
		}
		//*( m_useflag + sno ) = 0;
	}
	return 0;
}

int CMultiWave::Update()
{
	int sno;
	int ret;

	if( flag3d ){
		for( sno = 0; sno < m_sourcenum; sno++ ){

			STATUS st;
			if( oggflag == 0 ){
				st = ( m_source + sno )->GetStatus();
			}else{
				st = ( m_oggsource + sno )->GetStatus();
			}

			if( st == STATUS_PLAYING ){
			//if( *( m_useflag + sno ) == 1 ){
				if( oggflag == 0 ){
					ret = ( m_source + sno )->Update3DSound(); 
				}else{
					ret = ( m_oggsource + sno )->Update3DSound();
				}
				_ASSERT( !ret );
			}
		}
	}

	return 0;
}

int CMultiWave::SetEmitterParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel )
{
	int sno;
	int ret;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		//if( *( m_useflag + sno ) == 1 ){
			if( oggflag == 0 ){
				ret = ( m_source + sno )->SetEmitterParams( pos, vel ); 
			}else{
				ret = ( m_oggsource + sno )->SetEmitterParams( pos, vel ); 
			}
			_ASSERT( !ret );
		//}
	}
	return 0;
}

int CMultiWave::SetListenerParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel, D3DXVECTOR3 dir, D3DXVECTOR3 up )
{
	int sno;
	int ret;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		if( oggflag == 0 ){
			ret = ( m_source + sno )->SetListenerParams( pos, vel, dir, up ); 
		}else{
			ret = ( m_oggsource + sno )->SetListenerParams( pos, vel, dir, up ); 
		}
		_ASSERT( !ret );
	}
 
	return 0;
}

int CMultiWave::IsPlaying()
{
	int isp = 0;
	int sno;
	STATUS st;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		if( *( m_useflag + sno ) == 1 ){
			if( oggflag == 0 ){
				st = ( m_source + sno )->GetStatus();
				if( st == STATUS_PLAYING ){
					isp = 1;
					break;
				}
			}else{
				st = ( m_oggsource + sno )->GetStatus();
				if( st == STATUS_PLAYING ){
					isp = 1;
					break;
				}
			}
		}
	}
	return isp;
}

int CMultiWave::GetSamplesPlayed( double* psmp )
{
	if( m_sourcenum <= 0 ){
		return 0;
	}

	int setflag = 0;
	double minsmp = 1e200;
	double tmpsmp;
	double startsmp, diffsmp;
	int sno;
	int ret;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		STATUS st;
		if( oggflag == 0 ){
			st = ( m_source + sno )->GetStatus();
		}else{
			st = ( m_oggsource + sno )->GetStatus();
		}

		if( st == STATUS_PLAYING ){
		//if( *( m_useflag + sno ) == 1 ){
			if( oggflag == 0 ){
				ret = ( m_source + sno )->GetSamplesPlayed( &tmpsmp );	
				_ASSERT( !ret );
				startsmp = ( m_source + sno )->startsmp;
				//diffsmp = ( m_source + sno )->startsmp;
			}else{
				ret = ( m_oggsource + sno )->GetSamplesPlayed( &tmpsmp );
				_ASSERT( !ret );
				startsmp = ( m_oggsource + sno )->startsmp;
				//diffsmp = ( m_oggsource + sno )->startsmp;
			}
			diffsmp = tmpsmp - startsmp;
			if( minsmp > diffsmp ){
				minsmp = diffsmp;
				setflag = 1;
			}
		}
	}

	if( setflag ){
		*psmp = minsmp;
	}else{
		*psmp = 0.0;
	}

	return 0;
}


float CMultiWave::GetVolume()
{

	if( m_sourcenum <= 0 ){
		return 0;
	}

	float retvol = 0.0f;
	if( oggflag == 0 ){
		retvol = m_source->GetVolume();
	}else{
		retvol = m_oggsource->GetVolume();
	}

	return retvol;
}

int CMultiWave::SetFreqRatio( float freq )
{
	m_freqratio = freq;

	int ret;	
	int sno;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		if( oggflag == 0 ){
			ret = ( m_source + sno )->SetFrequencyRatio( freq );
		}else{
			ret = ( m_oggsource + sno )->SetFrequencyRatio( freq );
		}
		if( ret ){
			DbgOut( "multiwave : SetFreqRatio : source SetFreqRatio error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

int CMultiWave::Set3DEmiDist( float dist )
{
	int ret;	
	int sno;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		if( oggflag == 0 ){
			ret = ( m_source + sno )->Set3DEmiDist( dist );
		}else{
			ret = ( m_oggsource + sno )->Set3DEmiDist( dist );
		}
		if( ret ){
			DbgOut( "multiwave : Set3DEmiDist : source Set3DEmiDist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

int CMultiWave::GetDopplerScaler( float* dopplerptr )
{
	int ret = 0;	
	if( oggflag == 0 ){
		ret = m_source->GetDopplerScaler( dopplerptr );
	}else{
		ret = m_oggsource->GetDopplerScaler( dopplerptr );
	}
	_ASSERT( !ret );

	return ret;
}
int CMultiWave::SetDopplerScaler( float srcdoppler )
{
	int ret = 0;	
	int sno;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		if( oggflag == 0 ){
			ret = ( m_source + sno )->SetDopplerScaler( srcdoppler );
		}else{
			ret = ( m_oggsource + sno )->SetDopplerScaler( srcdoppler );
		}
		_ASSERT( !ret );
	}
	return ret;
}


int CMultiWave::Fade( int msec, float begingain, float endgain )
{
	bool bret;	
	int sno;
	for( sno = 0; sno < m_sourcenum; sno++ ){
		if( oggflag == 0 ){
			bret = ( m_source + sno )->Fade( msec, begingain, endgain );
		}else{
			bret = ( m_oggsource + sno )->Fade( msec, begingain, endgain );
		}
		if( bret != true ){
			DbgOut( "multiwave : Set3DEmiDist : source Set3DEmiDist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMultiWave::CalcTotalTime( int srcsize, WAVEFORMATEX* srcwf )
{

	m_totaltime = (double)srcsize / (srcwf->wBitsPerSample / 8.0) / (double)srcwf->nSamplesPerSec / srcwf->nChannels;

	return 0;
}

int CMultiWave::GetSamplesPerSec( DWORD* dstsample )
{
	if( oggflag == 0 ){
		*dstsample = m_source->m_WaveFormatEx.nSamplesPerSec;
	}else{
		*dstsample = m_oggsource->m_WaveFormatEx.nSamplesPerSec;
	}

	return 0;
}

int CMultiWave::GetStopCnt( int* cntptr )
{
	*cntptr = m_stopcnt;
	return 0;
}
int CMultiWave::GetStartCnt( int* cntptr )
{
	*cntptr = m_startcnt;
	return 0;
}
