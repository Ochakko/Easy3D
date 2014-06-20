#ifndef SOUNDSETH
#define SOUNDSETH

class CSoundElem;
class CHuSound;

class CSoundSet
{
public:
	CSoundSet();
	~CSoundSet();

	int CreateNewSet( char* srcname, int* dstsetid );//setid�Z�b�g�I�I�I

	int SetName( char* srcname );
	int GetName( char* dstname, int arrayleng );

	int SetDir( char* srcdir );
	int GetDir( char* dstdir, int arrayleng );
	int SetSSFDir( char* srcdir );

	CSoundElem* AddSound( CHuSound* srchs, char* filename, int bgmflag, int reverbflag, int bufnum );
	int DelSound( int srcseri );
	int DelBGM();
	int DestroySound();

	int DestroyObjs();

	CSoundElem* GetSoundElem( int srcseri );
	CSoundElem* GetSoundElem( char* filename );

	int GetSoundIndex( CSoundElem* srcse, int* dstindex );
	int GetSoundIndexByID( int setid, int* dstindex );

	int UpSound( int srcseri );
	int DownSound( int srcseri );

	int GetBGMTotalTime( double* dsttime );

private:
	int InitParams();
	int DestroyBGM();

public:
	char m_setname[32];//���Z�b�g�̖��O
	int m_setid;//���Z�b�g��ID
	char m_dirname[MAX_PATH];//���΃p�X�̃t�H���_��
	char m_ssfdir[MAX_PATH];//ssf�t�@�C���̃p�X

	CSoundElem* m_bgm;
	int m_soundnum;
	CSoundElem** m_ppsound;

};


#endif