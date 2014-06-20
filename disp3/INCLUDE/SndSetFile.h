#ifndef SSTFILEH
#define SSTFILEH

#define SSTLINELEN	4096

class CHuSound;
class CSoundBank;
class CSoundSet;
class CSoundElem;

typedef struct tag_sndsetbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} SNDSETBUF;

class CSndSetFile
{
public:
	CSndSetFile( CHuSound* srchs, CSoundBank* srcsb );
	~CSndSetFile();

	int WriteSndSetFile( char* filename, char* ssfdir = 0 );//ssfdirはwavファイルをssfからの相対位置にコピーする際に指定する。
	int LoadSndSetFile( char* filename );

private:
	int InitParams();
	int DestroyObjs();

	int Write2File( char* lpFormat, ... );

	int WriteSoundSet( int ssno, char* ssfdir );
	int WriteBGM( CSoundSet* curss, char* ssfdir );
	int WriteEffect( CSoundSet* curss, char* ssfdir );
	int ConvEn( char* dstname, int arrayleng, char* srcname );

	int SetBuffer();
	int CheckFileVersion( int* verptr );
	int ReadSoundSetNum( int* ssnum );
	int ReadSoundSet( SNDSETBUF* bnkbuf );
	int ReadBGM( SNDSETBUF* bgmbuf, CSoundSet* dstsst );
	int ReadEffect( SNDSETBUF* setbuf, CSoundSet* dstsst );

	int DeleteSoundSet( CSoundSet* delsst );

	int Read_Int( SNDSETBUF* moebuf, char* srcpat, int* dstint );
	int Read_Float( SNDSETBUF* moebuf, char* srcpat, float* dstfloat );
	int Read_Str( SNDSETBUF* moebuf, char* srcpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

public:
	int m_mode;
	HANDLE m_hfile;
	SNDSETBUF m_sstbuf;
	char m_line[ SSTLINELEN ];

	CHuSound* m_hs;
	CSoundBank* m_sndbnk;
	int m_loadversion;

	char m_ssfdir[MAX_PATH];
};


#endif