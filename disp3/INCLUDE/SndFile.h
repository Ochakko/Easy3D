#ifndef SndFILEH
#define SndFILEH

#include <D3DX9.h>

#define SNDLINELEN	4096

class CSndAnimHandler;
class CSndAnim;
class CSndKey;
class CSoundSet;
class CSoundBank;

typedef struct tag_sndbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} SNDBUF;

class CSndFile
{
public:
	CSndFile();
	~CSndFile();

	int WriteSndFile( char* filename, CSndAnim* srcanim, CSoundSet* ssptr );
	int LoadSndFile( char* filename, CSndAnimHandler* srcsndah, CSoundBank* sbptr, CSndAnim** ppanim );

private:
	int InitParams();
	int DestroyObjs();

	int Write2File( char* lpFormat, ... );

	int WriteAnimInfo();
	int WriteSndAnim();

	int SetBuffer();
	int CheckFileVersion( int* verptr );
	int ReadAnimInfo( CSndAnimHandler* srcsndah, CSoundBank* sbptr );
	int ReadSndAnim();
	int ReadSndKey( SNDBUF* animbuf );
	int ReadSndElem( SNDBUF* keybuf, SNDELEM* dstsnde );

	int DeleteAnim( CSndAnimHandler* srcsndah );

	int Read_Int( SNDBUF* sndbuf, char* srcpat, int* dstint );
	int Read_Float( SNDBUF* sndbuf, char* srcpat, float* dstfloat );
	int Read_Str( SNDBUF* sndbuf, char* srcpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

public:
	int m_mode;
	HANDLE m_hfile;
	SNDBUF m_sndbuf;
	char m_line[ SNDLINELEN ];

	CSndAnim* m_anim;
	CSoundSet* m_ss;
	int m_loadversion;
};


#endif