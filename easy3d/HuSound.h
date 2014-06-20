
// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

// DirectSound includes
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

// DirectMusic
//#include <dmusicc.h>
//#include <dmusici.h>

//#include "./XAudio/MultiWave.h"

#include <stdio.h>

//class CMusicManager;
//class CSoundManager;
class CSoundPrim;
class CSndFrame;

// このクラスは HuSound.dll からエクスポートされました。
class CHuSound {
public:
	CHuSound();
	~CHuSound();

public:
	//初期化
	int GetVer();
	int Init( HWND apphwnd, int enbalereverb );//初期化に失敗しても０が返る。そのかわり、失敗した場合は、initflagが０になる。
	int SetSearchDirectory( TCHAR* dirpath );//最後に\\付
	
	//読み込み	
	int LoadSoundFile( TCHAR* fname, int use3dflag, int reverbflag, int bufnum, int* soundid );// pathなしファイル名のみ
	int LoadSoundFileFromBuf( char* buf, int bufsize, int type, int use3dflag, int reverbflag, int bufnum, int* soundidptr );

	int Update();



	//削除
	int DestroySound( int soundid );


	//音操作
//	int PlaySound( int soundid, int isprimary, DWORD boundaryflag );
	int PlaySound( int soundid, int startsample, int openo, int numloops );
	int PlayFrame( int bgmid, int bgmsample, int bgmloop, CSndFrame* srcframe );

	int StopSound( int soundid );

//	int SetRepeats( int soundid, int repeatflag );
	int SetVolume( int soundid, float vol );
	int SetFrequency( int soundid, float freq );
//	int SetMasterTempo( float ftempo );//stereoサウンドのみ

//	int SetStartPointOfSound( int soundid, int sec );

	int Set3DEmiDist( int soundid, float dist );
	int SetDopplerScaler( int soundid, float srcdoppler );

	int Fade( int soundid, int msec, float begingain, float endgain );


	//情報取得
	int GetSoundSamplesPlayed( int soundid, double* psmp );
	int GetVolume( int soundid, float* pvol );
	int GetFrequency( int soundid, float* pfreq );
//	int GetMasterTempo( float* pftempo );//一度もSetMasterTempoしていない状態で呼ぶとエラーになる。
	int IsSoundPlaying( int soundid, int* playingptr );
//	int GetMusicTime( int soundid, int* mtimeptr, double* rtimeptr );
	int GetTotalTime( int soundid, double* dsttime );
	int GetSamplesPerSec( int soundid, DWORD* dstsample );	
	int GetStopCnt( int soundid, int* cntptr );
	int GetStartCnt( int soundid, int* cntptr );

	//3d soundの操作, LoadSoundFileで、use3dflagに１をセットしたsoundidのみ
//	int Set3DListener( float fDopplerFactor, float fRolloffFactor );
	//int Set3DDistance( int soundid, float fMinDistance, float fMaxDistance );
	int Set3DMovement( int soundid, D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity );

	int Set3DListenerMovement( D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity, D3DXVECTOR3* pvOrientFront, D3DXVECTOR3* pOrientTop );
	int GetDopplerScaler( int soundid, float* dopplerptr );


public:
	int initflag;//初期化に失敗したときは０がセットされる。

private:
	int InitParams();
	int FinalCleanUp();
	CSoundPrim* GetSoundPrim( int soundid );

private:

//	CMusicManager*	m_pMusicManager;
//	IDirectMusicAudioPath*  m_pStereoAudioPath;
//	CSoundManager*	m_pSoundManager;
	CSoundPrim*		m_pSoundPrim;
	char			m_searchdir[MAX_PATH];

	BOOL	m_bDeferSettings;
	HWND	m_apphwnd;

};

//extern HUSOUND_API int nHuSound;
//HUSOUND_API int fnHuSound(void);
