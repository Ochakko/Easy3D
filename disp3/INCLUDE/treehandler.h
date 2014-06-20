
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
		// CTreeLevel->AddElem�@����Ă΂�邱�Ƃ�z�肵��constructor�B
		// srcserino �́@import���̂ݗL���B
	CTreeElem( CTreeHandler* lphandler, int srcmode, int srcbrono, char* srcname, CTreeLevel* srcowner, int srcserino = -1 );

		// realloc ���ɂ́A��������constructor ���Ă�ŁA���Ƃ�SetParams ����B
	//CTreeElem();	
	~CTreeElem();
	void DestroyObjs();

#ifdef _DEBUG
	void	*operator new( size_t size );
	void	operator delete( void *p );

	void	*operator new( size_t size, CTreeHandler* lphandler, void** hcur_p, int srcobjnum ); // realloc �p
	void	operator delete( void* lpMem, CTreeHandler* lphandler, void** hcur_p, int srcobjnum );
#endif

	int SetChild( CTreeLevel* srcchild );

	int InitChain();

		// owner ���������Ă���΁Aparent, brother, sister�@�͎����B
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
		// tree����ӂɌ��肷��B
		// export���ɂ́Anew ���Ƃ�inclement����allocno ���i�[����B
		// import���ɂ́A�n���ꂽ�l���Z�b�g����B
	
	int brono; // ownter->levelarray ���Ŏ������w��indexno 

	int type;
	char* name;

	int pridatanum;
	unsigned long* privateptr; // �t���f�[�^

	CTreeLevel* owner;	
		// �I�I�I�I�I�@�e�͈�A�q�͂����ς����B�@�I�I�I�I
	CTreeLevel* child;	// ���ւ̎}������́@���R�B

	//CTreeElem* parent;	//owner->parent
	//CTreeElem* brother;	// owner->levelarray[brono + 1]
	//CTreeElem* sister; // owner->levelarray[brono - 1]


	// CTreeElem* ��re_elemptr��realloc�Œl���ς��̂ŁA�g��Ȃ��B
	int parseri;
	int broseri;
	int sisseri;

private:
	static HANDLE	s_hHeap;
	static unsigned int	s_uNumAllocsInHeap;


};



class CTreeLevel 	// ������(����depth)
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

		// srcserino �́@import���̂݁B
	CTreeElem* AddElem( char* srcname, int srcserino = -1 ); 

	int SetChain(); // re_elemptr->SetChain() ���ĂԁB

	void DeleteChild( int delcnt );

	int DumpText( HANDLE hfile );
	//int DumpText( HANDLE hfile, int* ar_handler, unsigned long* ar_hand_ptr );

public:
	CTreeHandler* lpthandler;

	int mode;
	int depth;
	int levelleng;
	//CTreeElem* re_elemptr; // CTreeElem �ϒ��z��B realloc �p new �Ł@���B
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


		// !!! data �́@���o�͎��ɂ͕K��serialno ��p����B
	// tree�쐬�p
	//int AddTree( int srcparentno, int srcdepth, char* srcname, int srcserino = -1 );
	//int AddTree( int addtype, int srcparno, int srcbefno, int srcdepth, char* srcname, int srcserino = -1 );
	int AddTree( char* srcname, int srcserino = -1 );
		//serialno ��Ԃ��B
		//no2ele, depth2ele �ւ̃Z�b�g
		//mode�ɂ���āA����name�ւ̑Ή���ς���B
		//SetChain ������B
		//import����serialno���Ăяo�����Ŏw��Bexport����allocate�ԍ��������I�ɃZ�b�g�B

	int Start( int offset = 0 );
	int Begin();
	int End();
	int GetCurDepth();

	int GetAddType( int* srcparno, int srcbefno, int srcdepth );
		// _addt_type ��Ԃ��B
		// AddTree �O�ɁAparentno ���X�V����B


	int Find( char* srcname, int srcdepth = -1 );
		//�Ή������ӂ�Tree�����߂�̂ł͂Ȃ��A�������O�����ɂ��邩�ǂ����̃`�F�b�N�ɂ����g��Ȃ��B				
		//���Ԃ�Atree�̍쐬���̃`�F�b�N�����Ɏg�p�B
		// depth �w�莞�ɂ́A����level���̂݃`�F�b�N����B
	


		// realloc���̕\�ƃ`�F�C���̏C���B
	//int UpdateElemIndex( CTreeElem* oldptr, unsigned long oldleng, int oldnum, CTreeElem* curptr );


		// �o�^	
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
	int* d2lnum; // depth ���Ƃ́@CTreeLevel�@�̐�
	//unsigned long** depth2level; // depth ���ƂɁ@CTreeLevel�ւ�pointer�z��(unsigned long*)���i�[
	CTreeLevel*** d2l; // depth ���ƂɁ@CTreeLevel** (�Q�d�̕\�Ȃ̂Œ���)
	CTreeLevel*	rootlevel;

	// AddTree�p
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