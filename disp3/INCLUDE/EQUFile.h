#ifndef EQUFILEH
#define EQUFILEH

#include <D3DX9.h>
#include <quaternion.h>

#define EQULINELEN	4096

typedef struct tag_equbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} EQUBUF;

typedef struct tag_keydat
{
	int frameno;
	CQuaternion q;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 tra;
}KEYDAT;

typedef struct tag_badat
{
	char bonename[256];
	int keynum;
	KEYDAT* keydat;
}BADAT;

typedef struct tag_animdat
{
	char animname[256];
	int frameleng;
	int bonenum;
	BADAT* badat;
}ANIMDAT;


class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CMotionCtrl;
class CMotionPoint2;

class CEQUFile
{
public:
	CEQUFile();
	~CEQUFile();

	int WriteEQUFile( char* filename, int* motidarray, int motnum, 
		CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );

	int LoadEQUFile( char* filename );
	int AddEQUMotion( int animno, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, LONG* motidptr, float mvmult );

private:
	int InitParams();
	int DestroyObjs();

	int Write2File( int tabnum, char* lpFormat, ... );
	int WriteAnimation( int motid );
	int WriteBoneAnim( int motid, CMotionCtrl* mcptr );
	int WriteAnimKey( CMotionPoint2* mpptr );
	
	int MakeJointArray();


	int SetBuffer();
	int ReadFileInfo();
	int ReadAnimation( EQUBUF* animbuf, int animno );
	int ReadBoneAnim( EQUBUF* bonebuf, int animno, int bano );
	int ReadAnimKey( EQUBUF* keybuf, int animno, int bano, int keyno );


	int Read_Int( EQUBUF* equbuf, char* startpat, char* endpat, int* dstint );
	int Read_Float( EQUBUF* equbuf, char* startpat, char* endpat, float* dstfloat );
	int Read_Str( EQUBUF* equbuf, char* startpat, char* endpat, char* dststr, int arrayleng );
	int Read_Vec3( EQUBUF* equbuf, char* startpat, char* endpat, D3DXVECTOR3* dstvec );
	int Read_Q( EQUBUF* equbuf, char* startpat, char* endpat, CQuaternion* dstq );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetInt2( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat2( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

public:
	int m_mode;
	HANDLE m_hfile;
	EQUBUF m_equbuf;
	char m_line[ EQULINELEN ];

	int m_loadversion;

	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;

	int* m_jointarray;
	int m_jointnum;

	float m_mult;
	int m_animnum;
	ANIMDAT* m_animdat;

	LONG m_motcookie;
};


#endif