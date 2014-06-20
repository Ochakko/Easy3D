#ifndef IM2FILEH
#define IM2FILEH

#include <D3DX9.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;

class CShdElem;
class CTreeElem2;

#define IM2LINELEN	4096

typedef struct tag_im2buf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} IM2BUF;

typedef struct tag_im2elem
{
	char partname[IM2LINELEN];
	int serialno;
	int vertno;
	int boneno1;
	int boneno2;
	int calcmode;
} IM2ELEM;


enum tag_ldim2mode
{
	IM2_NUTRAL,
	IM2_LOADNAME,
	IM2_LOADIM2,
	IM2_FINISH,
	IM2_MAX
};

typedef struct tag_infim2
{
	char bonename[256];
	int kind;//CALC_*
	float userrate;//％
	float orginf;//CALC_*で計算した値。
	float dispinf;//　orginf[] * userrate[]、normalizeflagが１のときは、正規化する。

	float reserved1, reserved2, reserved3;
}INFIM2;


class CIM2File
{
public:
	CIM2File();
	~CIM2File();

	int WriteIM2File( char* filename, int srcformqo, CTreeHandler2* lpth, CShdHandler* lpsh );
	int LoadIM2File( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );

	int SetIM2Params( CShdHandler* lpsh, int partno, int optvno, int childjoint, int parentjoint, int calcmode );
	//int GetIM2Params( CShdHandler* lpsh, int partno, int optvno, int* childjointptr, int* parentjointptr, int* calcmodeptr );


private:
	int InitParams();
	
	// for write
	int WriteIM2( CShdElem* selem, CTreeElem2* telem );
	int Write2File( char* lpFormat, ... );

	int GetFirstJointNo( int* firstno );
	int IM2ELEMInit( int inimode );

	int WritePolyMeshIM2( CShdElem* selem, char* name );
	int WritePolyMesh2IM2( CShdElem* selem, char* name );

///////
	int InitIM2BUF();
	int CheckFileHeader();

	int GetLoadMode( int* modeptr );
	int LoadPartName( int* modeptr );
	int LoadIM2( int* modeptr );

	int LoadLine( int* lenptr );
	int ParseIM2Line();
	int GetIntFromLine( int lineleng, int pos, int* lengptr, int* isendptr, int* intptr );
	int GetFloatFromLine( int lineleng, int pos, int* lengptr, int* isendptr, float* floatptr );
	int GetAlnumFromLine( int lineleng, int pos, int* lengptr, int* isendptr, char* strptr );

	int CheckSameName();


	int WriteUCharData( unsigned char* srcuchar, int num );
	int LoadUCharData( unsigned char* dstuc, int ucnum );

	int LoadIM2Binary();

public:
	HANDLE m_hfile;

	int m_version;

	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;

	IM2BUF im2buf;
	char m_line[ IM2LINELEN ];

	IM2ELEM curim2elem;

	int m_firstseri;

	int m_calcmode;
	int m_formqo;
	int m_invalidnameflag;

};

#endif