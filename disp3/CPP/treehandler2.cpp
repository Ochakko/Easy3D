#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#ifndef NOTWINDOWSH
#include <windows.h>
#endif


#include <coef.h>

#define	DBGH
#include <dbg.h>

#include <treehandler2.h>

#include <shdhandler.h>



CTreeHandler2::CTreeHandler2( int srcmode )
{
	int ret;
	CTreeElem2* rootelem = 0;

	ret = CheckMode( srcmode );
	if( ret ){
		DbgOut( "CTreeHandler : constructor : mode error !!!\n" );
		mode = 0;
		return;
	}else{
		mode = srcmode;
	}

	// AddTreeにparentが必要なので、ダミーのrootelem をここで作成。

	// ！！！！　rootelem の　serialno は必ず　０　！！！！
	s2e_allocleng = 0;
	s2e_leng = 0;
	s2e = 0;
	//CTreeElem2( CTreeHandler2* lphandler, int srcmode, char* srcname, CTreeElem2* srcparent, int srcserino );
	rootelem = new CTreeElem2( this, srcmode, "root elem", 0, 0 );
	if( !rootelem ){
		DbgOut( "CTreeHandler2 : constructor : rootelem alloc error !!!\n" );
		return;
	}
	
	ret = Add2ElemIndex( rootelem );
	if( ret ){
		_ASSERT( 0 );
		DbgOut( "CTreeHandler : constructor : Add2ElemIndex error !!!\n" );
		return;
	}

	ret = rootelem->SetType( SHDROOT );
	if( ret ){
		DbgOut( "CTreeHandler : constructor : rootelem SetMem BASE_TYPE error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	curseri = 0;
	parseri = -1;
	curdepth = 0;
	addno = 0;	
}
CTreeHandler2::~CTreeHandler2()
{
	int i;
	
	for( i = 0; i < s2e_leng; i++ ){
		CTreeElem2* delelem = (*this)( i );
		if( delelem )
			delete delelem;
		*(s2e + i) = 0;
	}

	if( s2e ){
		free( s2e );
		s2e = 0;
	}
	s2e_leng = 0;

	DbgOut( "CTreeHandler : destructor : free s2e\n" );
}
int CTreeHandler2::AddTree( char* srcname, int srcserino )
{		//serialno を返す。
		//s2e へのセット
		//modeによって、同一nameへの対応を変える。
		//SetChain もする。
		//import時はserialnoを呼び出し側で指定。
		// ！！！(export時はallocate番号を自動的にセット。)--->変更
	int ret;

	CTreeElem2* parelem = 0;

	addno++;
	if( (mode & TMODE_IMPORT) && (srcserino < 0) ){
		DbgOut( "CTreeHandler : AddTree : serialno error %d !!!\n", srcserino );
		return -1;
	}

	if( srcserino < 0 )
		srcserino = addno;
	curseri = srcserino;
	_ASSERT( curseri > 0 );

	// TMODE_MULT以外は　名前チェック
	if( mode & TMODE_ONCE ){
		// 全ての名前をチェックして、すでにあったら　アウト
		ret = Find( srcname );
		if( ret ){
			DbgOut( "CTreeHandler : AddTree : TMODE_ONCE : name check error !!!\n" );
			return -1;
		}
	}else if( mode & TMODE_LEVEL_ONCE ){
		_ASSERT( 0 );
		return -1;
	}

	if( parseri < 0 ){
		_ASSERT( 0 );
		DbgOut( "CTreeHandler : AddTree : parseri error %d!!!\n", parseri );
		return -1;
	}
	parelem = (*this)( parseri );
	if( !parelem ){
		_ASSERT( 0 );
		DbgOut( "CTreeHandler : AddTree : invalid elemno error !!! : parseri %d\n",
			parseri );
		return -1;
	}

	//curseri = AddChild( parelem, srcname, curdepth, srcserino );
	CTreeElem2* newelem;
	//CTreeElem2( CTreeHandler2* lphandler, int srcmode, char* srcname, CTreeElem2* srcparent, int srcserino = -1 );
	newelem = new CTreeElem2( this, mode, srcname, parelem, srcserino );
	if( !newelem ){
		_ASSERT( 0 );
		DbgOut( "TreeHandler2 : AddTree : newelem alloc error !!!\n" );
		return -1;
	}

	ret = Add2ElemIndex( newelem );
	if( ret ){
		_ASSERT( 0 );
		DbgOut( "TreeHandler : AddTree : Add2ElemIndex error !!!\n" );
		return -1;
	}

	return curseri;

}
int CTreeHandler2::Start( int offset, CTreeElem2* parelem )
{
	if( offset == 0 ){
		curseri = 0;
	}else{
		curseri = offset;
	}

	if( parelem == 0 ){
		parseri = -1;
		curdepth = 0;
	}else{
		parseri = parelem->serialno;
		curdepth = parelem->depth;
	}

	addno = 0;

	return 0;
}
int CTreeHandler2::Begin()
{
	curdepth++;

	if( parseri >= 0 ){
		//一つ下の階層の末っ子が新しい親
		CTreeElem2* curparent;
		curparent = (*this)( parseri );
		_ASSERT( curparent );

		CTreeElem2* downelem;
		downelem = curparent->child;
		_ASSERT( downelem );

		CTreeElem2* broelem = downelem;
		CTreeElem2* newparent = broelem;
		while( broelem ){
			broelem = broelem->brother;
			if( broelem )
				newparent = broelem;			
		}
		_ASSERT( newparent );
		parseri = newparent->serialno;

	}else{
		//初回の親はrootelem
		parseri = 0;
	}


	return 0;
}
int CTreeHandler2::End()
{
	curdepth--;

	parseri = GetParentSeriNo( parseri );
	if( parseri < 0 ){
		//_ASSERT( 0 );
		DbgOut( "CTreeHandler : End : GetParent reached to vacume warning !!!\n" );
	}

	return 0;
}
int CTreeHandler2::GetCurDepth()
{
	return curdepth;
}

int CTreeHandler2::Find( char* srcname, int srcdepth )
{
		//対応する一意なTreeを求めるのではなく、同じ名前が既にあるかどうかのチェックにしか使わない。
		//たぶん、treeの作成時のチェックだけに使用。
	CTreeElem2* chkelem;
	int i;
	for( i = 0; i < s2e_leng; i++ ){
		chkelem = (*this)( i );
		if( strcmp( srcname, chkelem->name ) == 0 ){
			return 1;
		}
	}
	return 0;
}

int CTreeHandler2::Dbg_DumpTree()
{

	return 0;
}
int CTreeHandler2::DumpTree( int srcmode )
{

	return 0;
}


int CTreeHandler2::CheckMode( int srcmode )
{
	// import か　exportは必ず、どちらか１個だけセット
	int chkmask;
	int chkcnt;

	chkmask = TMODE_IMPORT | TMODE_EXPORT;
	chkcnt = CountBits( srcmode & chkmask );
	if( chkcnt != 1 )
		return 1;

	// once または　level_once または mult は必ず、どちらか１個だけセット
	chkmask = TMODE_ONCE | TMODE_LEVEL_ONCE | TMODE_MULT;
	chkcnt = CountBits( srcmode & chkmask );
	if( chkcnt != 1 )
		return 1;

	return 0; // 0 が正常
}

int CTreeHandler2::CountBits( int chkbits )
{
	int chkleng, i, retcnt, chkmask;


	chkleng = sizeof( int ) * 8;
	retcnt = 0;
	for( i = 0; i < chkleng; i++ ){
		chkmask = 1 << i;
		if( chkbits & chkmask )
			retcnt++;
	}

	return retcnt;

}

/***
int CTreeHandler2::Add2ElemIndex( CTreeElem2* elemptr )
{
		// CTreeElem を alloc するたびに呼び出して登録。
	int serino;

	serino = elemptr->serialno;

	if( serino >= s2e_leng ){

		s2e = (CTreeElem2**)realloc( s2e, sizeof(CTreeElem2*) * (serino + 1) );
		if( !s2e ){
			DbgOut( "CTreeHandler : Add2ElemIndex : s2e realloc error !!!\n" );
			return 1;
		}

		*(s2e + serino) = elemptr;		
		s2e_leng = serino + 1;
	}else{

		*(s2e + serino) = elemptr;		
	}

//DbgOut( "CTreeHandler : Add2ElemIndex : serino %d --- elem %x\n", serino, elemptr );
DbgOut( "CTreeHandler : Add2ElemIndex : serino %d\n", serino );


	return 0;
}
***/
int CTreeHandler2::Add2ElemIndex( CTreeElem2* elemptr )
{
		// CTreeElem を alloc するたびに呼び出して登録。
	int serino;

	serino = elemptr->serialno;
	if( serino >= s2e_leng )
		s2e_leng = serino + 1;

	if( s2e_leng > s2e_allocleng ){
		while( s2e_leng > s2e_allocleng ){
			s2e_allocleng += 1024;
		}
		s2e = (CTreeElem2**)realloc( s2e, sizeof(CTreeElem2*) * s2e_allocleng );
		if( !s2e ){
			DbgOut( "CTreeHandler : Add2ElemIndex : s2e realloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	*(s2e + serino) = elemptr;		

	DbgOut( "CTreeHandler : Add2ElemIndex : serino %d\n", serino );

	return 0;
}

int CTreeHandler2::GetPartNoByName( char* partname, int* partnoptr )
{

	if( !partname || (*partname == 0) ){
		DbgOut( "treehandler2 : GetPartNoByName : partname NULL error !!!\n" );
		
		*partnoptr = -1;

		_ASSERT( 0 );
		return 1;
	}


	CTreeElem2* chkelem;
	int i;

	int retno = -1;
	
	for( i = 0; i < s2e_leng; i++ ){
		chkelem = (*this)( i );

		if( (strcmp( partname, chkelem->name ) == 0) && (chkelem->type != SHDDESTROYED) && (chkelem->type != SHDINFSCOPE) && (chkelem->type != SHDBBOX) ){
			retno = chkelem->serialno;
			break;
		}
	}

	*partnoptr = retno;
	
	return 0;
}

int CTreeHandler2::GetDispObjNoByName( char* dname, int* dispnoptr, CShdHandler* lpsh, int selectanchor )
{
	if( !dname || (*dname == 0) ){
		DbgOut( "treehandler2 : GetDispObjNoByName : dname NULL error !!!\n" );
		
		*dispnoptr = -1;

		_ASSERT( 0 );
		return 1;
	}


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ！！！！　selectanchor == 0 のときは、アンカーの番号は、返さないようにする。！！！！
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!


	CTreeElem2* chkelem;
	CShdElem* selem;
	int i;

	int retno = -1;
	int isdisp;
	for( i = 0; i < s2e_leng; i++ ){
		chkelem = (*this)( i );
		selem = (*lpsh)( i );

		isdisp = ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2));


		if( isdisp && (strcmp( dname, chkelem->name ) == 0) ){
			if( (selectanchor == 1) || (selem->m_anchorflag == 0) ){
				retno = chkelem->serialno;
				break;
			}
		}
	}

	*dispnoptr = retno;

	return 0;
}


int CTreeHandler2::GetBoneNoByName( char* bonename, int* bonenoptr, CShdHandler* lpsh, int needhaschild )
{
	if( !bonename || (*bonename == 0) ){
		DbgOut( "treehandler2 : GetBoneNoByName : bonename NULL error !!!\n" );
		
		*bonenoptr = -1;

		_ASSERT( 0 );
		return 1;
	}


	// ジョイント以外は、名前が同じでも、だめ。

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ！！！！　needhaschild != 0 のときは、エンドジョイントの番号は、返さないようにする。！！！！
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!


	CTreeElem2* chkelem;
	CShdElem* selem;
	int i;

	int retno = -1;
	int isjoint;
	for( i = 0; i < s2e_leng; i++ ){
		chkelem = (*this)( i );
		selem = (*lpsh)( i );

		if( chkelem->IsJoint() && (chkelem->type != SHDMORPH) )
			isjoint = 1;
		else
			isjoint = 0;

		if( isjoint && (strcmp( bonename, chkelem->name ) == 0) ){
			if( selem->part && ( (selem->part->bonenum >= 1) || (needhaschild == 0) ) ){
				retno = chkelem->serialno;
				break;
			}
		}
	}

	*bonenoptr = retno;
	
	return 0;
}

int CTreeHandler2::GetName( int partno, char* nameptr, int strleng )
{
	if( (partno < 0) || (partno >= s2e_leng) ){
		DbgOut( "treehandler2 : GetName : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CTreeElem2* telem;
	telem = (*this)( partno );
	_ASSERT( telem );

	int leng;
	leng = (int)strlen( telem->name );
	if( leng > strleng ){
		DbgOut( "treehandler2 : GetName : strleng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( nameptr, 256, telem->name );

	return 0;
}

int CTreeHandler2::ReplaceToEnglishName()
{
	int ret;
	static char headname[256] = "ReplacedName";
	int replaceno = 0;
	char tempname[256];

	char tempprinta[2048];
	char namepm[2048];




	int serino;
	CTreeElem2* telem;
	for( serino = 0; serino < s2e_leng; serino++ ){
		telem = (*this)( serino );

		strcpy_s( namepm, 2048, telem->name );

		// + --> P, - --> M に変換
		int namelen;
		namelen = (int)strlen( telem->name );
		int nameno;
		for( nameno = 0; nameno < namelen; nameno++ ){
			char* curp = namepm + nameno;

			if( *curp == '+' ){
				*curp = 'P';
			}else if( *curp == '-' ){
				*curp = 'M';
			}
		}

		int btflag = 0;
		int cmp0;
		cmp0 = strncmp( telem->name, "BT_", 3 );
		if( cmp0 == 0 ){
			btflag = 1;
		}


		int findilleagal = 0;
		int alnum;
		char curc;
		for( nameno = 0; nameno < namelen; nameno++ ){
			curc = *(namepm + nameno);

			if( ((curc >= 'a') && (curc <= 'z')) || ((curc >= 'A') && (curc <= 'Z')) || ((curc >= '0') && (curc <= '9')) || 
				(curc == '_') ){// || (curc == ':') ){
				alnum = 1;
			}else{
				alnum = 0;
			}

			if( alnum == 0 ){
				findilleagal = 1;
				break;
			}
		}

		if( findilleagal != 0 ){
			if( telem->IsJoint() ){
				if( btflag == 0 ){
					sprintf_s( tempname, 256, "%s%d_Joint", headname, replaceno );
				}else{
					sprintf_s( tempname, 256, "BT_%s%d_Joint", headname, replaceno );
				}
			}else{
				sprintf_s( tempname, 256, "%s%d", headname, replaceno );
			}
			
			sprintf_s( tempprinta, 2048, "Xファイル用パーツ名変更\r\n\t%s ---> %s\r\n", telem->name, tempname );
			DbgOut( tempprinta );

			ret = telem->SetEngName( tempname );
			if( ret ){
				DbgOut( "ReplaceToEnglishName : telem SetEngName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			replaceno++;
		}else{
			if( telem->IsJoint() ){

				sprintf_s( tempname, 256, "%s_Joint", namepm );

				ret = telem->SetEngName( tempname );
				if( ret ){
					DbgOut( "ReplaceToEnglishName : telem SetEngName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}else{
				ret = telem->SetEngName( namepm );
				if( ret ){
					DbgOut( "ReplaceToEnglishName : telem SetEngName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}


		}

	}

//	if( replaceno > 0 ){
//		::MessageBox( NULL, "パーツ名に使用できない文字があったので、\nパーツの名前を変換しました。\n詳しくは、dbg.txtをご覧ください。", "名前の変換", MB_OK );
//	}

	return 0;
}
