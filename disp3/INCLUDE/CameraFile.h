#ifndef CAMERAFILEH
#define CAMERAFILEH

#include <D3DX9.h>

#define CAMLINELEN	4096

class CCameraAnimHandler;
class CCameraAnim;
class CCameraKey;
class CCameraSWKey;

typedef struct tag_cambuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} CAMBUF;

class CCameraFile
{
public:
	CCameraFile();
	~CCameraFile();

	int WriteCameraFile( char* filename, CCameraAnim* srcanim );
	int LoadCameraFile( char* filename, CCameraAnimHandler* srccah, int* animnoptr );

private:
	int InitParams();
	int DestroyObjs();

	int Write2File( char* lpFormat, ... );

	int WriteAnimInfo();
	int WriteCameraAnim( int camno );
	int WriteSWAnim();

	int SetBuffer();
	int CheckFileVersion( int* verptr );
	int ReadAnimInfo( CCameraAnimHandler* srccah );
	int ReadCameraAnim( int camno );
	int ReadCameraKey( CAMBUF* animbuf, int camno );
	int ReadSWAnim();
	int ReadSWKey( CAMBUF* animbuf );

	int DeleteAnim( CCameraAnimHandler* srccah );

	int Read_Int( CAMBUF* cambuf, char* srcpat, int* dstint );
	int Read_Float( CAMBUF* cambuf, char* srcpat, float* dstfloat );
	int Read_Vec3( CAMBUF* cambuf, char* srcpat, D3DXVECTOR3* dstvec );
	int Read_Str( CAMBUF* cambuf, char* srcpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

public:
	int m_mode;
	HANDLE m_hfile;
	CAMBUF m_cambuf;
	char m_line[ CAMLINELEN ];

	CCameraAnim* m_anim;
	int m_loadversion;
};


#endif