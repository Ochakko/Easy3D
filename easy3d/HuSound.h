
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

// ���̃N���X�� HuSound.dll ����G�N�X�|�[�g����܂����B
class CHuSound {
public:
	CHuSound();
	~CHuSound();

public:
	//������
	int GetVer();
	int Init( HWND apphwnd, int enbalereverb );//�������Ɏ��s���Ă��O���Ԃ�B���̂����A���s�����ꍇ�́Ainitflag���O�ɂȂ�B
	int SetSearchDirectory( TCHAR* dirpath );//�Ō��\\�t
	
	//�ǂݍ���	
	int LoadSoundFile( TCHAR* fname, int use3dflag, int reverbflag, int bufnum, int* soundid );// path�Ȃ��t�@�C�����̂�
	int LoadSoundFileFromBuf( char* buf, int bufsize, int type, int use3dflag, int reverbflag, int bufnum, int* soundidptr );

	int Update();



	//�폜
	int DestroySound( int soundid );


	//������
//	int PlaySound( int soundid, int isprimary, DWORD boundaryflag );
	int PlaySound( int soundid, int startsample, int openo, int numloops );
	int PlayFrame( int bgmid, int bgmsample, int bgmloop, CSndFrame* srcframe );

	int StopSound( int soundid );

//	int SetRepeats( int soundid, int repeatflag );
	int SetVolume( int soundid, float vol );
	int SetFrequency( int soundid, float freq );
//	int SetMasterTempo( float ftempo );//stereo�T�E���h�̂�

//	int SetStartPointOfSound( int soundid, int sec );

	int Set3DEmiDist( int soundid, float dist );
	int SetDopplerScaler( int soundid, float srcdoppler );

	int Fade( int soundid, int msec, float begingain, float endgain );


	//���擾
	int GetSoundSamplesPlayed( int soundid, double* psmp );
	int GetVolume( int soundid, float* pvol );
	int GetFrequency( int soundid, float* pfreq );
//	int GetMasterTempo( float* pftempo );//��x��SetMasterTempo���Ă��Ȃ���ԂŌĂԂƃG���[�ɂȂ�B
	int IsSoundPlaying( int soundid, int* playingptr );
//	int GetMusicTime( int soundid, int* mtimeptr, double* rtimeptr );
	int GetTotalTime( int soundid, double* dsttime );
	int GetSamplesPerSec( int soundid, DWORD* dstsample );	
	int GetStopCnt( int soundid, int* cntptr );
	int GetStartCnt( int soundid, int* cntptr );

	//3d sound�̑���, LoadSoundFile�ŁAuse3dflag�ɂP���Z�b�g����soundid�̂�
//	int Set3DListener( float fDopplerFactor, float fRolloffFactor );
	//int Set3DDistance( int soundid, float fMinDistance, float fMaxDistance );
	int Set3DMovement( int soundid, D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity );

	int Set3DListenerMovement( D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity, D3DXVECTOR3* pvOrientFront, D3DXVECTOR3* pOrientTop );
	int GetDopplerScaler( int soundid, float* dopplerptr );


public:
	int initflag;//�������Ɏ��s�����Ƃ��͂O���Z�b�g�����B

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
