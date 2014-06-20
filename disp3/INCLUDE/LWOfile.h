#ifndef LWOFILEH
#define LWOFILEH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>

#define DBGH
#include <dbg.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;

class CLWOtags;
class CLWOlayr;
class CLWOsurf;
class CLWOclip;

class CLWOsurfblok;

typedef struct tag_lwobuf
{
	HANDLE hfile;
	unsigned char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} LWOBUF;


typedef enum tag_lwocnktype
{
	LWOCNK_LAYR,
	LWOCNK_PNTS,
	LWOCNK_VMAP,
	LWOCNK_POLS,
	LWOCNK_TAGS,
	LWOCNK_PTAG,
	LWOCNK_VMAD,
	LWOCNK_CLIP,
	LWOCNK_SURF,
	LWOCNK_BBOX,

	LWOCNK_FINISH,

	LWOCNK_NOTSUPPORT,
	LWOCNK_MAX
} LWOCNKTYPE;

static char strcnktype[LWOCNK_MAX + 1][5] = 
{
	"LAYR",
	"PNTS",
	"VMAP",
	"POLS",
	"TAGS",
	"PTAG",
	"VMAD",
	"CLIP",
	"SURF",
	"BBOX",

	"aaaa",

	"bbbb",
	"cccc"
};

typedef enum tag_lwosubcnktype
{
	LWOSUBCNK_CLIPSTIL,
	LWOSUBCNK_CLIPISEQ,
	LWOSUBCNK_CLIPXREF,

	LWOSUBCNK_SURFCOLR,
	LWOSUBCNK_SURFDIFF,
	LWOSUBCNK_SURFSPEC,
	LWOSUBCNK_SURFTRAN,
	LWOSUBCNK_SURFSIDE,
	LWOSUBCNK_SURFVCOL,
	LWOSUBCNK_SURFBLOK,

	LWOSUBCNK_FINISH,

	LWOSUBCNK_NOTSUPPORT,
	LWOSUBCNK_MAX

} LWOSUBCNKTYPE;

static char strsubcnktype[LWOSUBCNK_MAX + 1][5] = 
{
	"STIL",
	"ISEQ",
	"XREF",

	"COLR",
	"DIFF",
	"SPEC",
	"TRAN",
	"SIDE",
	"VCOL",
	"BLOK",

	"aaaa",

	"bbbb",
	"cccc"
};


typedef enum tag_lwoblktype
{
	LWOBLK_IMAP,
	LWOBLK_TMAP,

	LWOBLK_PROJ,
	LWOBLK_AXIS,
	LWOBLK_IMAG,
	LWOBLK_WRAP,
	LWOBLK_WRPW,
	LWOBLK_WRPH,
	LWOBLK_VMAP,

	LWOBLK_FINISH,

	LWOBLK_NOTSUPPORT,
	LWOBLK_MAX

} LWOBLKTYPE;

static char strblktype[LWOBLK_MAX + 1][5] = 
{
	"IMAP",
	"TMAP",

	"PROJ",
	"AXIS",
	"IMAG",
	"WRAP",
	"WRPW",
	"WRPH",
	"VMAP",

	"aaaa",

	"bbbb",
	"cccc"
};

typedef enum tag_lwosubblktype
{
	LWOSUBBLK_IMCHAN,
	LWOSUBBLK_IMENAB,

	LWOSUBBLK_TMCNTR,
	LWOSUBBLK_TMSIZE,
	LWOSUBBLK_TMROTA,
	LWOSUBBLK_TMOREF,
	LWOSUBBLK_TMCSYS,

	LWOSUBBLK_FINISH,

	LWOSUBBLK_NOTSUPPORT,
	LWOSUBBLK_MAX

} LWOSUBBLKTYPE;

static char strsubblktype[LWOSUBBLK_MAX + 1][5] =
{
	"CHAN",
	"ENAB",

	"CNTR",
	"SIZE",
	"ROTA",
	"OREF",
	"CSYS",

	"aaaa",

	"bbbb",
	"cccc"
};


class CLWOfile
{
public:
	CLWOfile();
	~CLWOfile();

	int LoadLWOFile( float multiple, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, int groundflag );


	//for debug
	int DumpLWOObjects();


private:
	int InitParams();
	int InitLoadParams();

	int DestroyObjs();


	CLWOtags* AddLWOtags();
	CLWOlayr* AddLWOlayr();
	CLWOsurf* AddLWOsurf();
	CLWOclip* AddLWOclip();

		// for load
	int InitLWOBuf( char* srcname );
	int CheckFileHeader();
	int GetCnkType( unsigned int* lengptr );
	int GetSubCnkType( unsigned int* lengptr );
	int GetBlkType( unsigned int* lengptr );
	int GetSubBlkType( unsigned int* lengptr );
	int SkipLoad( int offset, unsigned int skipleng );
	
	CLWOlayr* GetCurrentLAYR();
	
	int CheckVMAPdimension( char* srcid4, int srcdim );
	int CheckVMAPDataNum( unsigned int dataleng, int srcdim, int* numptr );
	int CheckVMADDataNum( unsigned int dataleng, int srcdim, int* numptr );
	int CheckPOLSDataNum( unsigned int dataleng, int* numptr );
	int CheckPTAGDataNum( unsigned int dataleng, int* numptr );	

		// load chunk
	int LoadChunkLAYR( unsigned int cnkleng );
	int LoadChunkPNTS( unsigned int cnkleng );
	int LoadChunkVMAP( unsigned int cnkleng );
	int LoadChunkVMAD( unsigned int cnkleng );
	int LoadChunkPOLS( unsigned int cnkleng );
	int LoadChunkTAGS( unsigned int cnkleng );
	int LoadChunkPTAG( unsigned int cnkleng );
	int LoadChunkBBOX( unsigned int cnkleng );
	int LoadChunkCLIP( unsigned int cnkleng );
	int LoadChunkSURF( unsigned int cnkleng );
	

		// load subchunk
	int LoadSubChunkCLIPSTIL( CLWOclip* curclip, unsigned int subcnkleng );
	int LoadSubChunkCLIPISEQ( CLWOclip* curclip, unsigned int subcnkleng );
	int LoadSubChunkCLIPXREF( CLWOclip* curclip, unsigned int subcnkleng );


	int LoadSubChunkSURFCOLR( CLWOsurf* cursurf, unsigned int subcnkleng );
	int LoadSubChunkSURFShading( LWOSUBCNKTYPE type, CLWOsurf* cursurf, unsigned int subcnkleng );
	int LoadSubChunkSURFSIDE( CLWOsurf* cursurf, unsigned int subcnkleng );
	int LoadSubChunkSURFVCOL( CLWOsurf* cursurf, unsigned int subcnkleng );
	int LoadSubChunkSURFBLOK( CLWOsurf* cursurf, unsigned int subcnkleng );

		// load block
	int LoadBlkPROJ( CLWOsurfblok* curblk, unsigned int blkleng );
	int LoadBlkAXIS( CLWOsurfblok* curblk, unsigned int blkleng );
	int LoadBlkIMAG( CLWOsurfblok* curblk, unsigned int blkleng );
	int LoadBlkWRAP( CLWOsurfblok* curblk, unsigned int blkleng );
	int LoadBlkWrapAmount( LWOBLKTYPE blktype, CLWOsurfblok* curblk, unsigned int blkleng );
	int LoadBlkVMAP( CLWOsurfblok* curblk, unsigned int blkleng );
	int LoadBlkIMAP( CLWOsurfblok* curblk, unsigned int blkleng );
	int LoadBlkTMAP( CLWOsurfblok* curblk, unsigned int blkleng );

		// loadsubblock
	int LoadSubBlkIMCHAN( CLWOsurfblok* curblk, unsigned int subblkleng );
	int LoadSubBlkIMENAB( CLWOsurfblok* curblk, unsigned int subblkleng );
	
	int LoadSubBlkTMCSR( LWOSUBBLKTYPE subblktype, CLWOsurfblok* curblk, unsigned int subblkleng );
	int LoadSubBlkTMOREF( CLWOsurfblok* curblk, unsigned int subblkleng );
	int LoadSubBlkTMCSYS( CLWOsurfblok* curblk, unsigned int subblkleng );

		// load primitive
	int ReadID4( char* id4ptr, int offset, int* stepnum );

	int ReadI1( int* iptr, int offset, int* stepnum );
	int ReadI2( int* iptr, int offset, int* stepnum );
	int ReadI4( int* iptr, int offset, int* stepnum );

	int ReadU1( unsigned int* uiptr, int offset, int* stepnum );
	int ReadU2( unsigned int* uiptr, int offset, int* stepnum, int notposflag = 0 );
	int ReadU4( unsigned int* uiptr, int offset, int* stepnum );

	int ReadVX( unsigned int* uiptr, int offset, int* stepnum );

	int ReadF4( float* flptr, int offset, int* stepnum );

	int ReadS0( char* cptr, int dstleng, int offset, int* stepnum );
	int ReadVEC12( float* flptr1, float* flptr2, float* flptr3, int offset, int* stepnum );
	

	//for debug

	int DumpTAGS();
	int DumpCLIP();
	int DumpLAYR();
	int DumpSURF();

	int DumpVMAP( CLWOlayr* layrptr );
	int DumpVMAD( CLWOlayr* layrptr );
	int DumpPOLS( CLWOlayr* layrptr );
	int DumpPTAG( CLWOlayr* layrptr );
	int DumpSURFBLOK( CLWOsurf* surfptr );
	
	int DumpLine( char* lpFormat, ... );
	

private:
	LWOBUF lwobuf;

	float m_multiple;


	LWOCNKTYPE m_cnktype;
	LWOSUBCNKTYPE m_subcnktype;
	LWOBLKTYPE m_blktype;
	LWOSUBBLKTYPE m_subblktype;


	int tagsnum;
	CLWOtags**	pptags;
	
	int layrnum;
	CLWOlayr**	pplayr;

	int surfnum;
	CLWOsurf**	ppsurf;

	int clipnum;
	CLWOclip** ppclip;

	HANDLE m_dumpfile;

};

#endif