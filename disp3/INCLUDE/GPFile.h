#ifndef GPFILEH
#define GPFILEH

#include <D3DX9.h>

#define GPLINELEN	4096

class CGPAnimHandler;
class CGPAnim;
class CGPKey;

typedef struct tag_gpbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} GPBUF;

class CGPFile
{
public:
	CGPFile();
	~CGPFile();

	int WriteGPFile( char* filename, CGPAnim* srcanim );
	int LoadGPFile( char* filename, CGPAnimHandler* srcgpah, int* animnoptr );

private:
	int InitParams();
	int DestroyObjs();

	int Write2File( char* lpFormat, ... );

	int WriteAnimInfo();
	int WriteGPAnim();
	int WriteGPFullFrame();

	int SetBuffer();
	int CheckFileVersion( int* verptr );
	int ReadAnimInfo( CGPAnimHandler* srcgpah );
	int ReadGPAnim();
	int ReadGPFullFrame();
	int ReadGPKey( GPBUF* animbuf, int frameflag );

	int DeleteAnim( CGPAnimHandler* srcgpah );

	int Read_Int( GPBUF* gpbuf, char* srcpat, int* dstint );
	int Read_Float( GPBUF* gpbuf, char* srcpat, float* dstfloat );
	int Read_Vec3( GPBUF* gpbuf, char* srcpat, D3DXVECTOR3* dstvec );
	int Read_Str( GPBUF* gpbuf, char* srcpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

public:
	int m_mode;
	HANDLE m_hfile;
	GPBUF m_gpbuf;
	char m_line[ GPLINELEN ];

	CGPAnim* m_anim;
	int m_loadversion;
};


#endif