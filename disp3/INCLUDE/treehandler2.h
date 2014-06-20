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
	
		// !!! data �́@���o�͎��ɂ͕K��serialno ��p����B
	// tree�쐬�p
	int AddTree( char* srcname, int srcserino = -1 );
		//serialno ��Ԃ��B
		//s2e �ւ̃Z�b�g
		//mode�ɂ���āA����name�ւ̑Ή���ς���B
		//SetChain ������B
		//import����serialno���Ăяo�����Ŏw��B
		// �I�I�I(export����allocate�ԍ��������I�ɃZ�b�g�B)--->�ύX

	int Start( int offset = 0, CTreeElem2* parelem = 0 );
	int Begin();
	int End();
	int GetCurDepth();

	int Find( char* srcname, int srcdepth = -1 );
		//�Ή������ӂ�Tree�����߂�̂ł͂Ȃ��A�������O�����ɂ��邩�ǂ����̃`�F�b�N�ɂ����g��Ȃ��B
		//���Ԃ�Atree�̍쐬���̃`�F�b�N�����Ɏg�p�B
		// depth �w�莞�ɂ́A����level���̂݃`�F�b�N����B

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

	// AddTree�p
	int curseri;
	int parseri;
	int curdepth;

	int addno;//export����serialno�Ɏg�p�B

private:
	int CheckMode( int srcmode );
	int CountBits( int chkbits );
	//int DumpText( HANDLE hfile );
	//int DumpBinary( HANDLE hfile );
};

#endif