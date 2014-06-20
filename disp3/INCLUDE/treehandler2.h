#ifndef TREEHANDLER2H
#define TREEHANDLER2H

#include <crtdbg.h>

#include <treeelem2.h>

class CShdHandler;

class CTreeHandler2
{
public:
	CTreeHandler2( int mode );
	~CTreeHandler2();

	CTreeElem2* operator () (int srcserino ){
		
#ifdef	_DEBUG
		if( (srcserino >= 0) && (srcserino < s2e_leng) ){
			return *(s2e + srcserino);
		}else{
			_ASSERT( 0 );
			return 0;
		}
#else
		return *(s2e + srcserino);
#endif
	};
	
		// !!! data の　入出力時には必ずserialno を用いる。
	// tree作成用
	int AddTree( char* srcname, int srcserino = -1 );
		//serialno を返す。
		//s2e へのセット
		//modeによって、同一nameへの対応を変える。
		//SetChain もする。
		//import時はserialnoを呼び出し側で指定。
		// ！！！(export時はallocate番号を自動的にセット。)--->変更

	int Start( int offset = 0, CTreeElem2* parelem = 0 );
	int Begin();
	int End();
	int GetCurDepth();

	int Find( char* srcname, int srcdepth = -1 );
		//対応する一意なTreeを求めるのではなく、同じ名前が既にあるかどうかのチェックにしか使わない。
		//たぶん、treeの作成時のチェックだけに使用。
		// depth 指定時には、同一level内のみチェックする。

	int Add2ElemIndex( CTreeElem2* elemptr );


	int GetName( int partno, char* nameptr, int strleng );

	int GetParentSeriNo( int srcseri ){
		int retseri = -1;
		CTreeElem2* curelem = 0;
		CTreeElem2* parentelem = 0;
		
		if( srcseri < 0 )
				return -1;

		curelem = (*this)( srcseri );
		if( !curelem )
			return -1;

		parentelem = curelem->parent;
		if( !parentelem )
			return -1;

		retseri = parentelem->serialno;
		return retseri;
	};

	int SetElemType( int srcseri, int srctype ){
		return (*this)( srcseri )->SetType( srctype );
	};

	int Dbg_DumpTree();
	int DumpTree( int srcmode );


	int GetPartNoByName( char* partname, int* partnoptr );
	int GetBoneNoByName( char* bonename, int* bonenoptr, CShdHandler* lpsh, int needhaschild );
	int GetDispObjNoByName( char* dname, int* dispnoptr, CShdHandler* lpsh, int selectanchor );

	int ReplaceToEnglishName();


private:
	//int AddBrother( CTreeElem* srcbroelem, char* srcname, int srcserino );
	//int AddChild( CTreeElem* srcparelem, char* srcname, int srcdepth, int srcserino );

public:
	int mode;

	int s2e_allocleng;
	int s2e_leng;
	CTreeElem2** s2e;

	// AddTree用
	int curseri;
	int parseri;
	int curdepth;

	int addno;//export時のserialnoに使用。

private:
	int CheckMode( int srcmode );
	int CountBits( int chkbits );
	//int DumpText( HANDLE hfile );
	//int DumpBinary( HANDLE hfile );
};

#endif