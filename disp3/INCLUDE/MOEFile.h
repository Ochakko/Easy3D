#ifndef MOEFILEH
#define MOEFILEH

#include <D3DX9.h>

#define MOELINELEN	4096

class CMOEAnimHandler;
class CMOEAnim;
class CMOEKey;
class CShdHandler;
class CTreeHandler2;

typedef struct tag_moebuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} MOEBUF;

class CMOEFile
{
public:
	CMOEFile();
	~CMOEFile();

	int WriteMOEFile( CTreeHandler2* lpth, CShdHandler* lpsh, char* filename, CMOEAnim* srcanim );
	int LoadMOEFile( CTreeHandler2* lpth, CShdHandler* lpsh, char* filename, CMOEAnimHandler* srcmoeah, int* animnoptr );

private:
	int InitParams();
	int DestroyObjs();

	int Write2File( char* lpFormat, ... );

	int WriteAnimInfo();
	int WriteMOEAnim();

	int SetBuffer();
	int CheckFileVersion( int* verptr );
	int ReadAnimInfo( CMOEAnimHandler* srcmoeah );
	int ReadMOEAnim();
	int ReadMOEKey( MOEBUF* animbuf );
	int ReadMOEElem( MOEBUF* keybuf, MOEELEM* dstmoee );

	int DeleteAnim( CMOEAnimHandler* srcmoeah );

	int Read_Int( MOEBUF* moebuf, char* srcpat, int* dstint );
	int Read_Float( MOEBUF* moebuf, char* srcpat, float* dstfloat );
	int Read_Str( MOEBUF* moebuf, char* srcpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

public:
	int m_mode;
	HANDLE m_hfile;
	MOEBUF m_moebuf;
	char m_line[ MOELINELEN ];

	CMOEAnim* m_anim;
	int m_loadversion;

	CShdHandler* m_lpsh;
	CTreeHandler2* m_lpth;
};


#endif