#ifndef ESIFILEH
#define ESIFILEH

#include <D3DX9.h>


#define ESILINELEN	4096

typedef struct tag_esibuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} ESIBUF;

typedef struct tag_convorg
{
	int orgvno;
	int optvnum;
	int* optvptr;
}CONVORG;

typedef struct tag_bi
{
	CONVORG convorg;
	float weight;
}BI;

class CMQOFile;
class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;


class CESIFile
{
public:
	CESIFile();
	~CESIFile();

	int WriteESIFileOnFrame( char* filename, int motid, int frameno, 
		CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );

	int LoadESIFile( LPDIRECT3DDEVICE9 pdev, char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult );

private:
	int InitParams();
	int DestroyObjs();

	int Write2File( int tabnum, char* lpFormat, ... );
	int WriteJointData( int motid, int frameno );
	void WriteJointElemReq( CShdElem* srcse, int tabcnt, int broflag, int motid, int frameno );
	int WriteInfData();


	int SetBuffer();
	int ReadFileInfo();
	int ReadMQO();
	int ReadJointData();
	int GetLine( ESIBUF* srcbuf, int* cplenptr );
	int ReadInfData();
	int ReadBoneInf( ESIBUF* bibuf, CShdElem* dispse, int vertnum, int* dirtyflag );


	int Read_Int( ESIBUF* esibuf, char* startpat, char* endpat, int* dstint );
	int Read_Float( ESIBUF* esibuf, char* startpat, char* endpat, float* dstfloat );
	int Read_Str( ESIBUF* esibuf, char* startpat, char* endpat, char* dststr, int arrayleng );
	int Read_Vec3( ESIBUF* esibuf, char* startpat, char* endpat, D3DXVECTOR3* dstvec );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetInt2( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat2( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

	int AddJoint();

	int Restore( LPDIRECT3DDEVICE9 pdev );
	int ConvOrgVertNo( CShdElem* dispse, BI* biptr, int vertnum );

public:
	int m_mode;
	HANDLE m_hfile;
	ESIBUF m_esibuf;
	char m_line[ ESILINELEN ];

	int m_loadversion;

	CMQOFile* m_mqofile;

	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;

	CShdElem* m_curparent;
	CShdElem* m_curjoint;
	char m_curname[256];
	D3DXVECTOR3 m_curpos;
	int m_enterjoint;
	int m_offset;
	int m_curseri;

	char m_dispname[256];
	char m_bonename[256];

	float m_mult;
};


#endif