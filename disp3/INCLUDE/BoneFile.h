#ifndef BONEFILEH
#define BONEFILEH

#include <D3DX9.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CMotionPoint2;
class CBVHElem;

#define BFLINELEN	4096

typedef struct tag_bonbuf
{
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} BONBUF;

typedef struct tag_bfelem
{
	int serialno;
	char name[BFLINELEN];
} BFELEM;

typedef struct tag_bonrel
{
	int seri;
	int relseri;
	int contrel;
} BONREL;

class CBoneFile
{
public:
	CBoneFile();
	~CBoneFile();

	int WriteBoneFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, int filetype, int relativeflag, int mqoflag );
	int WriteBoneFileFromBVH( char* filename, int srcoffset, CBVHElem* srcbe );


	int LoadBoneFile( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );

private:
	int InitParams();
	
	// for write
	void WriteElemReq( int firstflag, int srcseri, int addmode, int filetype );
	int Write2File( char* lpFormat, ... );
	int Conv4Digits( int srcseri, char* dstptr );

	int WriteElemName();


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

	int MakeAllTreeFlat();

	void WriteBVHNameReq( CBVHElem* srcbe, int srcoffset );
	void WriteBVHTreeReq( CBVHElem* srcbe, int srcoffset );

	int MakeBonRel();

public:
	HANDLE m_hfile;

	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;

	BONBUF bonbuf;
	char m_line[ BFLINELEN ];

	BFELEM befbfelem;
	BFELEM curbfelem;

	int relativemode;//相対値モードで使用
	int m_firstparentno;//読み込み時のMakeAllTreeFlatで使用
	int m_mqoflag;

	BONREL* m_bonrel;

};

#endif