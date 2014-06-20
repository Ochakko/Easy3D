#ifndef MAFILEH
#define MAFILEH

class CMotHandler;
class CMCHandler;
class CMyD3DApplication;
class CHandlerSet;

#include <coef.h>

typedef enum tag_mastate {
	MA_NONE,
	MA_FINDCHUNK,
	MA_TRUNK,
	MA_BRANCH,
	MA_FULENG,
	MA_EVENTKEY,
	MA_FINISH,
	MA_UNKCHUNK,
	MA_MAX
} MASTATE;

typedef struct tag_matrunk
{
	char motname[MAX_PATH];
	char filename[MAX_PATH];
	int idling;
	int ev0idle;
	int commonid;
	int forbidnum;
	int* forbidid;
	int notfu;
} MATRUNK;

typedef struct tag_mabranch
{
	char motname[MAX_PATH];
	int eventno;
	int frameno1;
	int frameno2;
	int notfu;
	int nottoidle;
} MABRANCH;


#define MALINELENG 10000

class CPanda;

class CMAFile
{
public:
	CMAFile();
	~CMAFile();

	int SaveMAFile( char* srcfilename, CMotHandler* srcmh, CMCHandler* srcmch, HWND srchwnd, int overwriteflag );


#ifdef INEASY3D
	int LoadMAFile( char* srcfilename, HWND srchwnd, CHandlerSet* srchs, float mvmult );
#else
	int LoadMAFile( char* srcfilename, CMyD3DApplication* srcpapp );
#endif

	int LoadMAFileFromPnd( CPanda* panda, int pndid, char* srcfilename, HWND srchwnd, CHandlerSet* srchs, float mvmult );


	int CheckIdlingMotion( CMCHandler* mch );
	int CheckSameMotionName();
	int CheckCommonid( HWND srchwnd );

private:
	int InitParams();
	int DestroyObjs();


	int WriteFULeng();
	int WriteEventKey();
	int WriteTrunk();
	int WriteBranch();

	int WriteMAHeader();
	int WriteLinechar( int addreturn );
///////////
	int SetBuffer( char* srcfilename );
	int SetBuffer( CPanda* panda, int pndid, char* srcfilename );
	int GetLine( int* getlen );
	int CheckFileVersion();

	int FindChunk();
	int GetChunkType( char* chunkname, int nameleng );
	int SkipChunk();

	int ReadFULeng();
	int ReadEventKey();
	int ReadTrunk( CPanda* panda = 0, int pndid = 0 );
	int ReadBranch();
	int SetMATrunk( char* srcline, MATRUNK* srctrunk );
	int SetMABranch( char* srcline, MABRANCH* srcbranch );

	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

	int LoadMAFile_aft( char* srcfilename );

public:
	int m_loadtrunknum;

	int m_moaversion;
	int m_fuleng;

	CMotHandler* m_mh;
	CMCHandler* m_mch;

private:
	MASTATE m_state;
	MABUF mabuf;	
	char m_linechar[ MALINELENG ];


	HWND m_apphwnd;


	CMyD3DApplication* m_papp;
	CHandlerSet* m_hs;

	char m_mediadir[ MAX_PATH ];

	float m_quamult;
};

#endif

