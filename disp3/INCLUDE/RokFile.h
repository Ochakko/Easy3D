#ifndef ROKFILEH
#define ROKFILEH

#include <coef.h>
#include <basedat.h>

#define DBGH
#include <dbg.h>

//////////

#define BUF_BLOCK_LENG	1024

typedef struct tag_rokpoint
{
	int pointno;
	int groupno;
	int dispflag;// 0 : display
	int dummy;
	int dimensionflag;// 0 : 2dim
	float x;
	float y;
	float z;
} ROKPOINT;

typedef struct tag_rokline
{
	int p1;
	int p2;
	int groupno;
	int dispflag; // 0 : display
} ROKLINE;

typedef struct tag_rokface
{
	int pointnum; // 3または4の時以外は使用しない。
	int colorno;
	int faceno;
	int line[4];// !!! 
} ROKFACE;

typedef struct tag_rokpallet
{
	int diffuse[3];
	int ambient[3];
	int reserved;
} ROKPALLET;

///////////

class CTreeHandler2;
class CShdHandler;
class CMotHandler;

class CRokFile
{
public:
	CRokFile();
	~CRokFile();
	
	void InitParams();
	int LoadRokFile( char* fname, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, HWND appwnd, int offset = 0 );

	int ReadRokHeader( char* buf );
	int ReadPoint( char* buf );
	int ReadLine( char* buf );
	int ReadFace( char* buf );
	int ReadPallet( char* buf );

private:
	void DestroyObjs();
	int MakeShdTree();

	int GrowUpPointBuf();
	int GrowUpLineBuf();
	int GrowUpFaceBuf();

	char* GetIntData( char* buf, int* dstint );
	char* GetFloatData( char* buf, float* dstfloat );
	char* SkipOnLine( char* buf );

	int MakePart( char* name );
	int MakeBoneElem( ROKPOINT* pointptr, int boneno );

	int AddPolyMesh( int colorno, int gno );
	int AddBone();

	int Add2Tree( char* nameptr, int hasmotion );
	int SetMaterials( int colorno );

	int begin_polymesh_vertex( int colorno, int vertnum );
	int polymesh_vertex( int* dirtypoint, int* indexconv );
	int begin_polymesh_face( int facenum, int colorno );
	int polymesh_face( int* dirtyface, int* indexconv );

	int SetVertNo( ROKFACE* rfptr, int* dstvertno );
	int SearchSamePoint( ROKLINE* curline, ROKLINE* nextline );

	int SetLoadParams( float multiple, int swapxz, int centeringx, int centerringz, int invz, int divmode, int chkinv );
	int SetGroupMax();
	int CheckGroupNo( ROKFACE* rfptr, int gno );

	int CalcNormal( ROKFACE* rf, CVec3f* dstn );

	void CheckInvFace( int fno, int* chkindex, int* errorflag );
	int FindCommonFace( ROKFACE* rf, int* findptr, int* commline, int* findnumptr );
	int CheckFaceDir( int chkflag, ROKFACE* rf, ROKFACE* chkrf, int clineno );
	int CheckOrder( int* vlist, int vnum, int lineno );

	int InverseIndex( int* dirtyface, int* chkindex );

	int CheckUpDataRange();

	int RokPallet2MQOMaterial();

public:
	ROKPOINT*	pointbuf;
	int pointbufleng;
	int rokpointnum;
	ROKLINE*	linebuf;
	int linebufleng;
	int roklinenum;
	ROKFACE*	facebuf;
	int facebufleng;
	int rokfacenum;
	ROKPALLET	palletbuf[13];
	int m_palletno;

private:
	CTreeHandler2*	m_thandler;
	CShdHandler*	m_shandler;
	CMotHandler*	m_mhandler;

	int m_offset;

	int curdepth;
	int curseri;
	int curtype;
	CMeshInfo	tempinfo;

	float m_multiple;
	int m_swapxz;
	float m_centerx;
	float m_centerz;
	int m_invz;
	int m_divmode;
	int m_chkinv;

	int m_groupmax;

	CVec3f m_minvec;
	CVec3f m_maxvec;

	int m_materialoffset;
};


#endif