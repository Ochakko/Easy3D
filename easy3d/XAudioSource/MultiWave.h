#ifndef MULTIWAVEH
#define MULTIWAVEH

#include <windows.h>
#include <D3DX9.h>
#include "audiocoef.h"
#include <mmreg.h> //WAVEFORMATEX

#define MAXMULTIWAVE	30

class XAudioWave;
class XAudioVorbis;

class CMultiWave
{
public:
	CMultiWave( int srcflag3d, int reverbflag );
	~CMultiWave();

	int LoadSound( char* filename, int sourcenum );
	int LoadSoundFromBuf( char* buf, int bufsize, int type, int sourcenum );



	int Play( UINT32 startsample, int openo, int loopcnt );
	int Stop();
	
	int IsPlaying();
	int SetVolume( const float Volume );
	float GetVolume();
	int GetSamplesPlayed( double* psmp );
	int GetSamplesPerSec( DWORD* dstsample );
	int GetStopCnt( int* cntptr );
	int GetStartCnt( int* cntptr );

	int SetFreqRatio( float freq );

	int Fade( int msec, float begingain, float endgain );



	//3D
	int Update();
	int SetEmitterParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel );
	int SetListenerParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel, D3DXVECTOR3 dir, D3DXVECTOR3 up );
	int Set3DEmiDist( float dist );

	int GetDopplerScaler( float* dopplerptr );
	int SetDopplerScaler( float srcdoppler );

private:
	int InitParams();
	int DestroyObjs();

	int GetFreeSourceNo();
	int CalcTotalTime( int srcsize, WAVEFORMATEX* srcwf );
public:
	int flagreverb;
	int flag3d;
	int oggflag;

	float m_freqratio;

	double m_totaltime;
	int m_stopcnt;
	int m_startcnt;

private:
	int m_sourcenum;
	XAudioWave*	m_source;
	XAudioVorbis* m_oggsource;
	int* m_useflag;
	int m_cursourceno;


};

#endif