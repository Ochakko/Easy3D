#ifndef BLDFILEH
#define BLDFILEH

#include <D3DX9.h>

#define BLDLINELEN	4096

typedef struct tag_bldbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} BLDBUF;


class CBLDFile
{
public:
	CBLDFile();
	~CBLDFile();

	int LoadBLDFile( char* filename );

	int SetBLD( BLD* sigbld, BLD* gbld, BLD* moabld, BLD* gpabld, BLD* moebld, BLD* cambld, BLD* ssbld, BLD* sabld );
	int WriteBLDFile( char* filename );

private:
	int InitParams();
	int DestroyObjs();

	int SetBuffer();
	int ReadFileInfo();
	int ReadSound();
	int ReadGround();
	int ReadCamera();
	int ReadCharacter();

	int Read_Int( BLDBUF* bldbuf, char* startpat, char* endpat, int* dstint );
	int Read_Float( BLDBUF* bldbuf, char* startpat, char* endpat, float* dstfloat );
	int Read_Str( BLDBUF* bldbuf, char* startpat, char* endpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

	int InitBLD( BLD* dstbld );

	int Write2File( char* lpFormat, ... );

public:
	int m_mode;
	HANDLE m_hfile;
	BLDBUF m_bldbuf;
	char m_line[ BLDLINELEN ];

	int m_loadversion;

	BLD m_ssbld;
	BLD m_sabld;
	BLD m_gbld;
	BLD m_cambld;
	BLD m_sigbld;
	BLD m_moabld;
	BLD m_gpabld;
	BLD m_moebld;

	char m_dir[MAX_PATH];
};


#endif