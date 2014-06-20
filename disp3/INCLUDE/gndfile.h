#ifndef GNDFILEH
#define GNDFILEH


#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>

#include <sigfile.h>

#define DBGH
#include <dbg.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;
class CMQOMaterial;

class CGndFile
{
public:
	CGndFile();
	~CGndFile();

	int WriteGndFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh );
	int WriteGndFile2Buf( char* buf, int bufsize, int* writesizeptr, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh );

	int LoadGndFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset = 0, float datamult = 1.0f, int adjustuvflag = 0 );
	int LoadGndFileFromBuf( char* bufptr, int bufsize, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset = 0, float datamult = 1.0f, int adjustuvflag = 0 );	
	
	int Conv2GndFile( char* infilename, char* outfilename );

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
	int WritePolyMesh( CShdElem* selem );
	int WritePolyMesh2( CShdElem* selem );
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

	int LoadPart();
	int LoadMorph();
	int LoadPolyLine();
	int LoadPolygon();
	int LoadSphere();
	int LoadBezMesh();
	int LoadRevolved();
	int LoadDisk();
	int LoadBezLine();
	int LoadExtrude();
	int LoadPolyMesh( unsigned int flags );
	int LoadPolyMesh2( int adjustuvflag, unsigned int flags, unsigned int flags2 );
	int LoadGroundData();

	///////
	int CreateHandler();
	int DestroyHandler();

	///////
	int LoadGndFile_aft( int offset );
	int WriteGndFile_aft();
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

	float m_datamult;
	int m_adjustuvflag;

	int loadmagicno;
	int writemagicno;

	SIGBUF m_buf;
	int m_frombuf;

	int m_writemode;//enum GNDWRITEMODE_*

	D3DXMATRIX m_offsetmat;

	int m_materialoffset;
};

#endif