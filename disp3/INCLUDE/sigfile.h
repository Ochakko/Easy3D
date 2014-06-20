#ifndef SIGFILEH
#define SIGFILEH


#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>

#define DBGH
#include <dbg.h>

#include <map>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;

class CPolyMesh;
class CPolyMesh2;

class CMQOMaterial;

enum {
	SIGWRITEMODE_FILE,
	SIGWRITEMODE_BUF,
	SIGWRITEMODE_BUFSIZE,
	SIGWRITEMODE_MAX
};


/****								 ****/
/**** binary file : *.sig の読み書き ****/
/****								 ****/


/***
serialno 0, type SHDROOT, name root elem, brono 0
shdelem depth : 0, clockwise 0, BDivU 0, BDivV 0,
texname 0, TexRepX 0, TexRepY 0, TRANSPARENTBLACK 0, UANIME 0.0, VANIME 0.0, ALPHA 1.000000, : type SHDROOT,
***/

/***
typedef struct tag_sigheader
{
	int serialno;
	int type;
	char name[256];
	int brono;
	int depth;
	int clockwise;
	int bdivu, bdivv;
	char texname[256];
	int texrepx, texrepy;
	int transparent;
	float uanime, vanime;
	float alpha;

	int hasmotion;
	
	int texrule;
	int notuse;
} SIGHEADER;
***/

typedef struct tag_sigheader
{
	int serialno;
	int type;
	char name[256];
	int brono;
	int depth;
	int clockwise;
	int bdivu, bdivv;
	char texname[256];
	int texrepx, texrepy;
	int transparent;
	float uanime, vanime;
	float alpha;

	int hasmotion;
	
	int texrule;
	int notuse;
	int influencenum;
	
	//将来の拡張のために余分に取っておく。
	int ignorenum;	//int reservedint1;
	unsigned int flags;//int reservedint2; //<---- ver1.0.0.9
	DWORD dispswitchno; //int reservedint3; //<---- ver1.0.1.0
	unsigned int exttexmem; //int reservedint4;//<---- ver1.0.1.4
	
	float aspect;//float reservedfloat1;
	int userint1;//float reservedfloat2;
	
	unsigned char iklevel;//float reservedfloat3;
	unsigned char mikodef;

	unsigned char flags2;
	unsigned char flags3;

	float facetm180;

} SIGHEADER;




// SIGHEADER.type に使う。curmat, partmatのヘッダー
//#define CURMATHEADER	0x12345678
//#define PARTMATHEADER	0x87654321


/***
MESHINFO : type SHDPATH,
M : 0, N : 0, Total : 0, M_CLOSED : 0, N_CLOSED : 0, skip : 0
hascolor 0
diffuse
3DOBJ : type SHDTYPENONE,
X : 1.000000, Y : 1.000000, Z : 1.000000
specular
3DOBJ : type SHDTYPENONE,
X : 1.000000, Y : 1.000000, Z : 1.000000
ambient
3DOBJ : type SHDTYPENONE,
X : 1.000000, Y : 1.000000, Z : 1.000000
***/

typedef struct _miheader
{
	int type;
	int m, n, total;
	int mclosed, nclosed;
	int skip;
	int hascolor;
	VEC3F diffuse;
	VEC3F specular;
	VEC3F ambient;
} MIHEADER;

typedef struct _revinfo
{
	float radfrom, radto;
	int isnormalset;
} REVINFO;

typedef struct tag_sigbuf
{
	char* buf;
	int bufsize;
	int pos;
} SIGBUF;

typedef struct tag_sigmaterial
{
	int materialno;
	int legacyflag;
	char name[256];

	ARGBF col;
	float dif;
	float amb;
	float emi;
	float spc;
	float power;
	char tex[256];
	char alpha[256];
	char bump[256];

	int vcolflag;
////

	int shader;

////
	ARGBF dif4f;
	RGB3F amb3f;
	RGB3F emi3f;
	RGB3F spc3f;

	int transparent;
	int texrule;
	int blendmode;

	float uanime;
	float vanime;
/////
	unsigned char exttexnum;
	unsigned char exttexmode; //重ねるか、パラパラアニメするかどうか。
	unsigned char exttexrep; //繰り返しかどうか
	unsigned char exttexstep; //何フレームごとに、変化するか。
	//char** exttexname;

/// for miko
	int convnamenum;
	//char** ppconvname;

	//DWORD reserved1;
	char alphatest0;
	unsigned char alphaval0;
	char alphatest1;
	unsigned char alphaval1;

	//DWORD reserved2;
	char texanimtype;
	char creserved2_1;
	char creserved2_2;
	char creserved2_3;

	float orgalpha;
	DWORD reserved4;

} SIGMATERIAL;

typedef struct tag_sigmaterial2
{
	SIGMATERIAL material1;
	float glowmult[3];
	DWORD reserved[16];
} SIGMATERIAL2;

class CPanda;

class CSigFile
{
public:
	CSigFile();
	~CSigFile();

	int WriteSigFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh );
	int WriteSigFile2Buf( char* buf, int bufsize, int* writesizeptr, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh );

	int LoadSigFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, D3DXVECTOR3 datamult, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot, int groundflag );
	int LoadSigFileFromBuf( char* bufptr, int bufsize, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot, int groundflag );	
	int LoadSigFileFromPnd( CPanda* panda, int pndid, int propno, 
		CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot, int groundflag );	
	

	int Conv2SigFile( char* infilename, char* outfilename );

private:
	void WriteShdReq( CShdElem* selem, int* errorflag );
	int WriteShdElem( CShdElem* selem );

	int WritePart( CShdElem* selem );
	int WriteMorph( CShdElem* selem );
	int WritePolyLine( CShdElem* selem );
	int WritePolygon( CShdElem* selem );
	int WriteSphere( CShdElem* selem );
	int WriteBezMesh( CShdElem* selem );
	int WriteRevolved( CShdElem* selem );
	int WriteDisk( CShdElem* selem );
	int WriteBezLine( CShdElem* selem );
	int WriteExtrude( CShdElem* selem );
	int WritePolyMesh( CPolyMesh* pm, int isinfscope );
	int WritePolyMesh2( CPolyMesh2* pm2, int isinfscope );
	int WriteInfScope( CShdElem* selem );
	int WriteGroundData( CShdElem* selem );


	int WriteSigHeader( SIGHEADER sigheader );
	int WriteMIHeader( CMeshInfo* srcmi );
	int WriteVecLine( CVecLine* srcvl );
	int WriteVecData( VEC3F* srcvec, int vnum );
	int WriteMatData( CMatrix2* srcmat, int matnum );
	int WriteIntData( int* srcint, int intnum );
	int WriteFloatData( float* srcfloat, int flnum );
	int WriteUCharData( unsigned char* srcuchar, int num );
	int WriteRGBAData( ARGBF* srcargb, int colnum );
	int WriteDWData( DWORD* srcdw, int dwnum );


	int WriteExtTexName( int exttexnum, char** exttexname );

	int WriteMQOMaterial();


/////////////
	void InitLoadParams();

	int LoadSigHeader( SIGHEADER* sigheader );
	int LoadMIHeader( CMeshInfo* dstmi );
	int LoadVecData( VEC3F* dstvec, int vnum );
	int LoadMatData( CMatrix2* dstmat, int matnum );
	int LoadIntData( int* dstint, int intnum );
	int LoadFloatData( float* dstfloat, int flnum );
	int LoadUCharData( unsigned char* dstuc, int ucnum );
	int LoadARGBData( ARGBF* dstargb, int colnum );
	int LoadDWData( DWORD* dstdw, int dwnum );

	int AddShape2Tree( SIGHEADER sh );
	int LoadInfluenceList( int listleng, int offset );
	int LoadIgnoreList( int listleng, int offset );

	int LoadExtTex( SIGHEADER sh );

	int Init3DObj();

	int LoadPart( unsigned char flags2 );
	int LoadMorph();
	int LoadPolyLine();
	int LoadPolygon();
	int LoadSphere();
	int LoadBezMesh();
	int LoadRevolved();
	int LoadDisk();
	int LoadBezLine();
	int LoadExtrude();
	int LoadPolyMesh( unsigned int flags, unsigned int flags2 );
	int LoadPolyMesh2( int adjustuvflag, unsigned char flags, unsigned char flags2 );
	int LoadInfScope( int scnum, unsigned char flags2 );
	int LoadGroundData();

	int LoadSigMaterial();


	int LoadPolyMesh2_aft( CShdElem* selem, CPolyMesh2* pm2, int adjustuvflag, unsigned char flags, unsigned char flags2, float facet );
	int LoadPolyMesh_aft( CShdElem* selem, CPolyMesh* pm, unsigned char flags, unsigned char flags2 );



	///////
	int CreateHandler();
	int DestroyHandler();

	///////
	int LoadSigFile_aft( int offset );
	int WriteSigFile_aft();


	///////
	CMQOMaterial* AddMQOMaterial( CShdElem* selem );

private:
	HANDLE hfile;
	CTreeHandler2* lpth;
	CShdHandler* lpsh;
	CMotHandler* lpmh;

	///
	int curseri, befseri;
	int curdepth, befdepth;
	int curshdtype, befshdtype;
	CMeshInfo	tempinfo;

	int bezlinenum;
	int bezlineno;
	int bezsurfno;

	int m_writeno;
	int m_offset;

	char* exttexlist[MAXEXTTEXNUM];
	char exttexbuf[ MAXEXTTEXNUM ][ 256 ];

	D3DXVECTOR3 m_datamult;
	int m_adjustuvflag;

	int loadmagicno;
	int writemagicno;

	SIGBUF m_buf;
	int m_frombuf;

	int m_writemode;//enum SIGWRITEMODE_*

	//D3DXVECTOR3 m_offsetpos;
	D3DXMATRIX m_offsetmat;

	int m_groundflag;
	int m_materialoffset;
	int m_oldmaterialnum;

	//std::vector<int> m_newmatindex;
	std::map<int, int> m_newmatindex;
};

#endif