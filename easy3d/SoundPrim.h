#ifndef SOUNDPRIMH
#define SOUNDPRIMH

#include <windows.h>
#include <D3DX9.h>
//class CMusicManager;
//class CSoundManager;

//class CMusicSegment;
//class CSound;

class CMultiWave;


class CSoundPrim
{
public:
	//CSoundPrim( CMusicManager* pmm, CSoundManager* psm, IDirectMusicAudioPath* psp );
	CSoundPrim();
	~CSoundPrim();

	int LoadSoundFile( char* fname, int use3dflag, int reverbflag, int bufnum, int* soundid );
	int LoadSoundFileFromBuf( char* buf, int bufsize, int type, int use3dflag, int reverbflag, int bufnum );

	
	int Play( int startsample, int openo, int numloops );
	int Stop();
	int Unload();

	int SetRepeats( int repeatflag );
	int SetVolume( float vol );
	int SetFrequency( float freq );
	int Set3DEmiDist( float dist );

	int GetSoundSamplesPlayed( double* psmp );
	int GetVolume( float* pvol );
	int GetFrequency( float* pfreq );

	int Set3DDistance( float fMinDistance, float fMaxDistance );
	int SetEmitterParams( D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity );
	int SetListenerParams( D3DXVECTOR3 pos, D3DXVECTOR3 vel, D3DXVECTOR3 dir, D3DXVECTOR3 up );

	int SetStartPointOfSound( int sec );

	int IsSoundPlaying( int* playingptr );
	int GetMusicTime( int* mtimeptr, double* rtimeptr );

	int Update();

	int Fade( int msec, float begingain, float endgain );

	int GetTotalTime( double* dsttime );
	int GetSamplesPerSec( DWORD* dstsample );
	int GetStopCnt( int* cntptr );
	int GetStartCnt( int* cntptr );

	int GetDopplerScaler( float* dopplerptr );
	int SetDopplerScaler( float srcdoppler );

public:
	int serialno;
	int is3d;

	LONG m_lvolume;
	DWORD m_dwfreq;
	DWORD m_dwloop;

//	CMusicManager* m_pMusicManager;//コピーの値を保持するだけ
//	CSoundManager* m_pSoundManager;//コピーの値を保持するだけ
//	IDirectMusicAudioPath*  m_pStereoAudioPath;//コピーの値を保持するだけ

//	CMusicSegment* m_pMS;
//	CSound*			m_pS;

	CMultiWave* m_mwave;

	char* savebuf;//LoadSoundFileFromBuf用


	CSoundPrim*	next;

private:
	int InitParams();
	int FinalCleanUp();

};

#endif