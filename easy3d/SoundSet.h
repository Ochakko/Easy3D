#ifndef SOUNDSETH
#define SOUNDSETH

class CSoundElem;
class CHuSound;

class CSoundSet
{
public:
	CSoundSet();
	~CSoundSet();

	int CreateNewSet( char* srcname, int* dstsetid );//setidセット！！！

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
	char m_setname[32];//音セットの名前
	int m_setid;//音セットのID
	char m_dirname[MAX_PATH];//相対パスのフォルダ名
	char m_ssfdir[MAX_PATH];//ssfファイルのパス

	CSoundElem* m_bgm;
	int m_soundnum;
	CSoundElem** m_ppsound;

};


#endif