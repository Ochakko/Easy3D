
#ifndef TREEHANDLERH
#define TREEHANDLERH

#include <crtdbg.h>

//#include <shdhandler.h>
//#include <mothandler.h>


class CTreeLevel;
class CTreeHandler;
class CTreeElem;

typedef struct tree_index
{
	CTreeElem** htelem;
	int offset;
}TREEINDEX;

class CTreeElem
{
public:
		// CTreeLevel->AddElem　から呼ばれることを想定したconstructor。
		// srcserino は　import時のみ有効。
	CTreeElem( CTreeHandler* lphandler, int srcmode, int srcbrono, char* srcname, CTreeLevel* srcowner, int srcserino = -1 );

		// realloc 時には、引数無しconstructor を呼んで、あとでSetParams する。
	//CTreeElem();	
	~CTreeElem();
	void DestroyObjs();

#ifdef _DEBUG
	void	*operator new( size_t size );
	void	operator delete( void *p );

	void	*operator new( size_t size, CTreeHandler* lphandler, void** hcur_p, int srcobjnum ); // realloc 用
	void	operator delete( void* lpMem, CTreeHandler* lphandler, void** hcur_p, int srcobjnum );
#endif

	int SetChild( CTreeLevel* srcchild );

	int InitChain();

		// owner が分かっていれば、parent, brother, sister　は自明。
	int SetChain(); 


	int SetType( int srctype );
	int SetName( char* srcname );
	int SetOwner( CTreeLevel* srcowner );
	int SetBroNo( int srcbrono );
	int SetSerialNo( int srcserino );

	int GetDepth();

	int AddPrivateData( unsigned long srcpridata );


	int DumpText( HANDLE hfile );

	void Dbg_DumpReq( HANDLE hfile, int errcnt );
	void DumpReq( HANDLE hfile, int* errcnt );
	//int DumpText( HANDLE hfile, int* ar_handle, unsigned long* ar_hand_ptr );
	//void DumpReq( HANDLE hfile, int* ar_handle, unsigned long* ar_hand_ptr, int* errcnt );


private:
	int SetParams( CTreeHandler* lphandler, int srcmode, int srcbrono, char* srcname, CTreeLevel* srcowner, int srcserino = -1 );


public:
	CTreeHandler* lpthandler;

	static int allocno; 
	
	int mode;
	int serialno;
		// treeを一意に決定する。
		// export時には、new ごとにinclementするallocno を格納する。
		// import時には、渡された値をセットする。
	
	int brono; // ownter->levelarray 中で自分を指すindexno 

	int type;
	char* name;

	int pridatanum;
	unsigned long* privateptr; // 付加データ

	CTreeLevel* owner;	
		// ！！！！！　親は一個、子はいっぱいも可。　！！！！
	CTreeLevel* child;	// 下への枝分かれは　自由。

	//CTreeElem* parent;	//owner->parent
	//CTreeElem* brother;	// owner->levelarray[brono + 1]
	//CTreeElem* sister; // owner->levelarray[brono - 1]


	// CTreeElem* はre_elemptrのreallocで値が変わるので、使わない。
	int parseri;
	int broseri;
	int sisseri;

private:
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;


};



class CTreeLevel 	// 横方向(同じdepth)
{
public:
	//CTreeLevel( CTreeHandler* lphandler, int srcmode, int srcdepth, CTreeElem* srcparent );
	CTreeLevel( CTreeHandler* lphandler, int srcmode, int srcdepth, int srcparno );
	~CTreeLevel();

#ifdef _DEBUG
	void	*operator new( size_t size );
	void	operator delete( void *p );
#endif

	//int SetParams( int srcmode, int srcdepth, CTreeElem* srcparent );
	//int SetParams( int srcmode, int srcdepth, int srcparno );
	int SetParams( int srcmode, int srcdepth, CTreeHandler* lpthandler, int srcparno );

/***
	CTreeElem* operator() ( int srcbrono ){
#ifdef _DEBUG
		if( (srcbrono >= 0) && (srcbrono < levelleng) ){
			return re_elemptr + srcbrono;
		}else{
			_ASSERT( 0 );
			return 0;
		}
#else
		return re_elemptr + srcbrono;
#endif
	};
***/
	CTreeElem* operator() ( int srcbrono ){
#ifdef _DEBUG
		if( (srcbrono >= 0) && (srcbrono < levelleng) ){
			return *(helem + srcbrono);
		}else{
			_ASSERT( 0 );
			return 0;
		}
#else
		return *(helem + srcbrono);
#endif
	};

		// srcserino は　import時のみ。
	CTreeElem* AddElem( char* srcname, int srcserino = -1 ); 

	int SetChain(); // re_elemptr->SetChain() を呼ぶ。

	void DeleteChild( int delcnt );

	int DumpText( HANDLE hfile );
	//int DumpText( HANDLE hfile, int* ar_handler, unsigned long* ar_hand_ptr );

public:
	CTreeHandler* lpthandler;

	int mode;
	int depth;
	int levelleng;
	//CTreeElem* re_elemptr; // CTreeElem 可変長配列。 realloc 用 new で　作る。
	CTreeElem** helem;


	//CTreeElem* parent;
	//TREEINDEX* tiparent;
	int parseri;

private:
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;

};



class CTreeHandler
{
public:
	CTreeHandler( int mode );
	~CTreeHandler();

/***
	CTreeElem* operator () (int srcserino ){
		TREEINDEX* tindex = 0;
		
#ifdef	_DEBUG
		if( (srcserino >= 0) && (srcserino < s2e_leng) ){
			tindex = s2eti + srcserino;
			return ( *(tindex->htelem) + tindex->offset );
		}else{
			_ASSERT( 0 );
			return 0;
		}
#else
		return ( *(tindex->htelem) + tindex->offset );
#endif
	};
***/
	CTreeElem* operator () (int srcserino ){
		TREEINDEX* tindex = 0;
		
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
	


	CTreeLevel* operator() (int srcdepth, int srcbrono ){

		CTreeLevel** levelarray = 0;

#ifndef _DEBUG

		if( (srcdepth >= 0) && (srcdepth < depthmax) 
			&& (srcbrono >= 0) && (srcbrono < *(d2lnum + srcdepth)) ){
			levelarray = *(d2l + srcdepth);
			return *(levelarray + srcbrono);
		}else{
			return 0;
		}
#else
		levelarray = *(d2l + srcdepth);
		return *(levelarray + srcbrono);
#endif
	};


		// !!! data の　入出力時には必ずserialno を用いる。
	// tree作成用
	//int AddTree( int srcparentno, int srcdepth, char* srcname, int srcserino = -1 );
	//int AddTree( int addtype, int srcparno, int srcbefno, int srcdepth, char* srcname, int srcserino = -1 );
	int AddTree( char* srcname, int srcserino = -1 );
		//serialno を返す。
		//no2ele, depth2ele へのセット
		//modeによって、同一nameへの対応を変える。
		//SetChain もする。
		//import時はserialnoを呼び出し側で指定。export時はallocate番号を自動的にセット。

	int Start( int offset = 0 );
	int Begin();
	int End();
	int GetCurDepth();

	int GetAddType( int* srcparno, int srcbefno, int srcdepth );
		// _addt_type を返す。
		// AddTree 前に、parentno を更新する。


	int Find( char* srcname, int srcdepth = -1 );
		//対応する一意なTreeを求めるのではなく、同じ名前が既にあるかどうかのチェックにしか使わない。				
		//たぶん、treeの作成時のチェックだけに使用。
		// depth 指定時には、同一level内のみチェックする。
	


		// realloc時の表とチェインの修正。
	//int UpdateElemIndex( CTreeElem* oldptr, unsigned long oldleng, int oldnum, CTreeElem* curptr );


		// 登録	
	int Add2DepthIndex( int srcdepth, CTreeLevel* srclevel );
	//int Add2ElemIndex( CTreeElem* addelem );
	int Add2ElemIndex( CTreeElem* elemptr );


	int GetParentSeriNo( int srcseri ){
		int retseri = -1;
		CTreeElem* parentelem = 0;

		retseri = (*this)( srcseri )->parseri;
		return retseri;
	};

	int SetElemType( int srcseri, int srctype ){
		return (*this)( srcseri )->SetType( srctype );
	};

	int Dbg_DumpTree();
	int DumpTree( int srcmode );
	//int DumpTree( int srcmode, int* ar_handler, unsigned long* ar_hand_ptr );

private:
	int AddBrother( CTreeElem* srcbroelem, char* srcname, int srcserino );
	int AddChild( CTreeElem* srcparelem, char* srcname, int srcdepth, int srcserino );
	//int AddChild( int srcparno, char* srcname, int srcdepth, int srcserino );

public:
	int mode;
	int s2e_leng;
	//unsigned long* seri2elem;
	
	CTreeElem** s2e;
	//TREEINDEX* s2eti;

	int isfirstAdd2Depth;

	int depthmax;
	int* d2lnum; // depth ごとの　CTreeLevel　の数
	//unsigned long** depth2level; // depth ごとに　CTreeLevelへのpointer配列(unsigned long*)を格納
	CTreeLevel*** d2l; // depth ごとに　CTreeLevel** (２重の表なので注意)
	CTreeLevel*	rootlevel;

	// AddTree用
	int curseri;
	int befseri;
	int parseri;
	int curdepth;
	int addtype;
	int befaddtype;

private:
	int CheckMode( int srcmode );
	int CountBits( int chkbits );
	//int DumpText( HANDLE hfile );
	//int DumpBinary( HANDLE hfile );
};

#endif