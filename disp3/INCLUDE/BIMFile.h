#ifndef BIMFILEH
#define BIMFILEH

#include <D3DX9.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;

class CShdElem;
class CTreeElem2;

#define BIMLINELEN	4096

typedef struct tag_bimbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} BIMBUF;

typedef struct tag_bimelem
{
	char partname[BIMLINELEN];
	int serialno;
	int vertno;
	int boneno1;
	int boneno2;
	int calcmode;
} BIMELEM;


enum tag_ldbimmode
{
	BIM_NUTRAL,
	BIM_LOADNAME,
	BIM_LOADBIM,
	BIM_FINISH,
	BIM_MAX
};


class CBIMFile
{
public:
	CBIMFile();
	~CBIMFile();

	int WriteBIMFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh );
	int LoadBIMFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );

private:
	int InitParams();
	
	// for write
	int WriteBIM( CShdElem* selem, CTreeElem2* telem );
	int Write2File( char* lpFormat, ... );

	int GetFirstJointNo( int* firstno );
	int BIMELEMInit( int inimode );

	int WritePolyMeshBIM( CShdElem* selem );
	int WritePolyMesh2BIM( CShdElem* selem );

///////
	int InitBIMBUF();
	int CheckFileHeader();

	int GetLoadMode( int* modeptr );
	int LoadPartName( int* modeptr );
	int LoadBIM( int* modeptr );

	int LoadLine( int* lenptr );
	int ParseBIMLine();
	int GetIntFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int* intptr );
	int GetFloatFromLine( int lineleng, int pos, int* lengptr, int* isendptr, float* floatptr );
	int GetAlnumFromLine( int lineleng, int pos, int* lengptr, int* isendptr, char* strptr );

	int CheckSameName();

public:
	HANDLE m_hfile;

	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;

	BIMBUF bimbuf;
	char m_line[ BIMLINELEN ];

	BIMELEM curbimelem;

	int m_firstseri;

	int m_calcmode;
};

#endif