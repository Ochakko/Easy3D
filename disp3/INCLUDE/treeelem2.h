#ifndef TREEELEM2H
#define TREEELEM2H

#include <crtdbg.h>

#define TE2_BLOCKLENG	50

class CTreeHandler2;

class CTreeElem2
{
public:
		// srcserino は　import時のみ有効。
	CTreeElem2( CTreeHandler2* lphandler, int srcmode, char* srcname, CTreeElem2* srcparent, int srcserino );

	~CTreeElem2();
	void DestroyObjs();

	//void	*operator new( size_t size );
	//void	operator delete( void *p );

	/***
#ifdef _DEBUG
	void	*operator new( size_t size );
	void	operator delete( void *p );

	//void	*operator new( size_t size, CTreeHandler2* lphandler, void** hcur_p, int srcobjnum ); // realloc 用
	//void	operator delete( void* lpMem, CTreeHandler2* lphandler, void** hcur_p, int srcobjnum );
#endif
	***/

	int SetType( int srctype );
	int SetName( char* srcname );
	int SetEngName( char* srcengname );
	int SetParent( CTreeElem2* srcparent );
	int SetBroNo( int srcbrono );
	int SetSerialNo( int srcserino );

	int GetDepth();

	int AddPrivateData( unsigned long srcpridata );

	int DumpText( HANDLE hfile );
	void Dbg_DumpReq( HANDLE hfile, int errcnt );
	void DumpReq( HANDLE hfile, int* errcnt );

	int SetChain( CTreeElem2* srcparent );

	int IsJoint();

	int IsAnchorName( char* targetname, char* applyname );
	int GetDispObjApplyName( char* applyname );

private:
	//int SetParams( CTreeHandler* lphandler, int srcmode, int srcbrono, char* srcname, CTreeElem2* srcparent, int srcserino = -1 );


public:
	CTreeHandler2* lpthandler;

	//static int allocno;//treehandler2::addnoに変更 
	
	int mode;
	int serialno;
		// treeを一意に決定する。
		// export時には、new ごとにinclementするallocno を格納する。
		// import時には、渡された値をセットする。
	
	int brono; // ownter->levelarray 中で自分を指すindexno 

	int type;
	char* name;
	int depth;

	char* engname;

	int pridatanum;
	unsigned long* privateptr; // 付加データ

	CTreeElem2* parent;
	CTreeElem2* brother;
	CTreeElem2* sister;
	CTreeElem2* child;

	/***
private:
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

	static unsigned int s_DataNo;
	static void*	s_DataHead;

	static BYTE*	s_HeadIndex[2048];// s_DataHeadを格納する配列
	static int		s_HeadNum;// s_HeadIndexの有効データの数。

	static int	s_useflag[2048][TE2_BLOCKLENG];
	***/
};


#endif