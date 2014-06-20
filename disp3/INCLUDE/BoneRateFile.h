#ifndef BONERATEFILEH
#define BONERATEFILEH

#include <D3DX9.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;

class CShdElem;
class CTreeElem2;

#define RATLINELEN	4096

typedef struct tag_ratbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} RATBUF;

typedef struct tag_ratelem
{
	char partname[RATLINELEN];
	int serialno;
	int vertno;
	int boneno1;
	int boneno2;
	float bonerate1;
} RATELEM;


enum tag_ldratmode
{
	RAT_NUTRAL,
	RAT_LOADNAME,
	RAT_LOADRAT,
	RAT_FINISH,
	RAT_MAX
};


class CBoneRateFile
{
public:
	CBoneRateFile();
	~CBoneRateFile();

	int WriteBoneRateFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh );
	int LoadBoneRateFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );

private:
	int InitParams();
	
	// for write
	int WriteRate( CShdElem* selem, CTreeElem2* telem );
	int Write2File( char* lpFormat, ... );

	int GetFirstJointNo( int* firstno );
	int RATELEMInit( int inimode );

	int WritePolyMeshRate( CShdElem* selem );
	int WritePolyMesh2Rate( CShdElem* selem );

///////
	int InitRATBUF();
	int CheckFileHeader();

	int GetLoadMode( int* modeptr );
	int LoadPartName( int* modeptr );
	int LoadRat( int* modeptr );

	int LoadLine( int* lenptr );
	int ParseRatLine();
	int GetIntFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int* intptr );
	int GetFloatFromLine( int lineleng, int pos, int* lengptr, int* isendptr, float* floatptr );

	/***
	// for load
	int InitBONBUF();
	int GetFileVersion( int* typeptr );

	int LoadLine( int* lenptr );
	int ParseLine();
	
	int BFELEMInit();
	int GetSerialFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int collongflag = 1 );
	int GetNameFromLine( int lineleng, int pos, int* lengptr, int* isendptr );
	int Copy2BefBFELEM();


	int GetLoadMode( int* modeptr );
	int LoadNamePart( int* modeptr );
	int ParseNameLine();
	int LoadTreePart( int* modeptr );
	int ParseTreeLine();

	int CheckRelativeMode();
	int GetFirstJointNo( int* firstno );
	***/

public:
	HANDLE m_hfile;

	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;

	RATBUF ratbuf;
	char m_line[ RATLINELEN ];

	RATELEM curratelem;

	int m_firstseri;
};

#endif