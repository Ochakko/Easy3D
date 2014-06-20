#ifndef QUAFILEH
#define QUAFILEH


#include <coef.h>
#include <basedat.h>
#include <matrix2.h>
#include "motionpoint2.h"

#define DBGH
#include <dbg.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CMotionCtrl;

typedef struct quafilehdr
{
	int magicno;
	char motname[256];
	int framenum;
	int mottype;
} QUAFILEHDR;

typedef struct tag_quafilehdr2
{
//RokDeBone2 ver1008, easy3d ver2005以降
	int interpolation;


	DWORD flags;//2005/04/22
	int motjump;//2005/08/06

	int jointnum;//2009/03/30
	char zatype;//2010/10/21

	char reservedc1, reservedc2, reservedc3;

	//int reservedint4;
	//int reservedint1, reservedint2, reservedint3, reservedint4;
	float reservedfl1, reservedfl2, reservedfl3, reservedfl4;
} QUAFILEHDR2;


typedef struct quadata
{
	int boneno;//boneno == -1 がデータの終わり。
	CMotionPoint mp;
} QUADATA;

typedef struct tag_quadata2
{
	int boneno;//boneno == -1 がデータの終わり。
	CMotionPoint2 mp;
} QUADATA2;

typedef struct tag_quabuf
{
	char* buf;
	int bufsize;
	int pos;
} QUABUF;

typedef struct tag_quatexkey
{
	int boneno;
	int frameno;
	int changenum;
} QUATEXKEY;

typedef struct tag_quatexchange
{
	char matname[256];
	char texname[256];
} QUATEXCHANGE;

typedef struct tag_quaalpkey
{
	int boneno;
	int frameno;
	int changenum;
} QUAALPKEY;

typedef struct tag_quaalpchange
{
	char matname[256];
	float alpha;
} QUAALPCHANGE;


typedef struct tag_quadskey
{
	int boneno;
	int frameno;
	int changenum;
} QUADSKEY;

typedef struct tag_quadschange
{
	int switchno;
	int change;
} QUADSCHANGE;

enum {
	SAVEQUA_FILE,
	SAVEQUA_BUF,
	SAVEQUA_BUFSIZE,
	SAVEQUA_MAX
};

typedef struct tag_quadata3
{
	int ishead;
	int m_frameno;
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_mv;
	CQuaternion m_q;
		
	int serialno;
	int interp;
	D3DXVECTOR3 m_scale;
	int m_userint1;
	D3DXVECTOR3 m_eul;

	int reserved[16];

} QUADATA3;


typedef struct tag_quanamehdr
{
	int seri;
	char name[256];
	int parent;

	int framenum;
	int texframenum;
	int dsframenum;

	QUADATA3* dataptr;
	int dirtyflag;


	int alpframenum;//reservedの直前。
	int reserved[15];

} QUANAMEHDR;

typedef struct tag_morphindex
{
	int seri;
	char name[256];
	int index;
} MORPHINDEX;

typedef struct tag_morphval
{
	int frameno;
	int baseindex;
	int targetindex;
	float value;
} MORPHVAL;

class CPanda;

class CQuaFile
{
public:
	CQuaFile();
	~CQuaFile();

	int SaveQuaFile( int namequaflag, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, char* fname, int mcookie, int srcstart, int srcend );
	int SaveQuaFile2Buf( int namequaflag, CTreeHandler2* srclpth, CShdHandler* srclpsh, char* buf, int bufsize, int* writesize, CMotHandler* srclpmh, int mcookie );

	
	int LoadQuaFile( CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, char* fname, LONG* mcookieptr, D3DXVECTOR3 quamult = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
	int LoadQuaFileFromBuf( CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srcmh, char* bufptr, int bufsize, LONG* mcookieptr, float quamult = 1.0f );
	int LoadQuaFileFromPnd( CPanda* panda, int pndid, char* fname, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srcmh, LONG* mcookieptr, float quamult = 1.0f );

	int ImportQuaFile( CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, char* fname, int motid, int offset );

	int ConvSymmXMotion( char* fname );

private:
	void DestroyObjs();
	int ReadQuaHeader( QUAFILEHDR* hdrptr );
	int ReadQuaHeader2( QUAFILEHDR2* hdrptr );

	int ReadQuaData( QUADATA* dataptr );
	int ReadQuaData2( QUADATA2* dataptr );
	int ReadQuaTexKey( QUATEXKEY* dataptr );
	int ReadQuaTexChange( QUATEXCHANGE* dataptr );
	int ReadQuaAlpKey( QUAALPKEY* dataptr );
	int ReadQuaAlpChange( QUAALPCHANGE* dataptr );
	int ReadQuaDSKey( QUADSKEY* dataptr );
	int ReadQuaDSChange( QUADSCHANGE* dataptr );
	int ReadUCharaData( unsigned char* dataptr, int insize );
	int ReadQuaNameHdr( QUANAMEHDR* dataptr );
	int ReadQuaData3( QUADATA3* dataptr );
	int ReadMorphIndex( MORPHINDEX* dataptr, int num );
	int ReadMorphVal( MORPHVAL* dataptr );

	int WriteQuaHeader( QUAFILEHDR hdr );
	int WriteQuaHeader2( QUAFILEHDR2 hdr );

	//int WriteQuaData( QUADATA data );
	int WriteQuaData2( QUADATA2 data );
	void WriteQuaDataReq( CMotionCtrl* mcptr );

	int WriteUCharaData( HANDLE outfile, unsigned char* dataptr, DWORD outsize );

	int WriteQuaTexKey( QUATEXKEY data );
	int WriteQuaTexChange( QUATEXCHANGE data );
	void WriteTexAnimReq( CMotionCtrl* mcptr );

	int WriteQuaAlpKey( QUAALPKEY data );
	int WriteQuaAlpChange( QUAALPCHANGE data );
	void WriteAlpAnimReq( CMotionCtrl* mcptr );


	int WriteQuaDSKey( QUADSKEY data );
	int WriteQuaDSChange( QUADSCHANGE data );
	void WriteDSReq( CMotionCtrl* mcptr );


	int WriteQuaNameHdr( QUANAMEHDR data );
	int WriteQuaData3( QUADATA3 data );


	int LoadQuaFile_aft( LONG* mcookieptr );
	int WriteQuaFile_aft( int namequaflag );

	int GetTopLevelElem( CMotionCtrl** pptopmc, int* topnum );
	void WriteNameQuaElemReq( CMotionCtrl* curtop, int broflag, int* errorcnt );

	int LoadNameQua( int magicno, int motid, int framemax, int interpolation, int frameoffset );
	int LoadMorphIndices( int quatype );
	int LoadMorphVals( CMotionCtrl* mcptr, int motid, int frameoffset );

	int CheckNameFileMark();
	int WriteNameFileMark( HANDLE srchfile );

	int WriteMorphIndices( HANDLE srchfile );
	int WriteMorphIndex( HANDLE srchfile, MORPHINDEX* data, int num );
	int WriteMorphVal( MORPHVAL* data );
	int WriteMorphKey( CMotionCtrl* mcptr, int quatype );
	void WriteMorphReq( CMotionCtrl* mcptr );

	int Seri2MorphIndex( int srcseri );
	int MorphIndex2Seri( int srcindex );


//	int GetQuaFileQ( QUANAMEHDR* calchdrptr, int calcframeno, CQuaternion* calcqptr );

	

private:
	HANDLE hfile;
	CTreeHandler2* lpth;
	CShdHandler* lpsh;
	CMotHandler* lpmh;
	int m_motcookie;
	int m_writemode;// 名前とはうらはらに、主に、ロード時に使用
	int m_savemode;// SAVEQUA_FILE, SAVEQUA_BUF, SAVEQUA_BUFSIZE

	int m_hasscale;

	int m_frombuf;
	QUABUF m_buf;

	int m_savestart;
	int m_saveend;
	
	D3DXVECTOR3 m_quamult;

	MORPHINDEX* m_morphindex;
	int m_morphindexnum;

};


#endif
