#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <coef.h>

#define	DBGH
#include "dbg.h"

#include <treehandler.h>



//////////////////////////
// CTreeHandler
//////////////////////////



CTreeHandler::CTreeHandler( int srcmode )
{
	int ret;
	CTreeElem* rootelem = 0;

	isfirstAdd2Depth = 1;

	ret = CheckMode( srcmode );
	if( ret ){
		DbgOut( "CTreeHandler : constructor : mode error !!!\n" );
		mode = 0;
		return;
	}else{
		mode = srcmode;
	}


	depthmax = 0;
	d2lnum = 0; // depth ごとの　CTreeLevel　の数
	d2l = 0; // depth ごとに　CTreeLevel** (２重の表なので注意)
	rootlevel = new CTreeLevel( this, mode, 0, -1 );
	if( !rootlevel ){
		DbgOut( "CTreeHandler : constructor : rootlevel alloc error !!!\n" );
		return;
	}	

DbgOut( "CTreeHandler : constructor : new rootlevel\n" );
	

	// AddTreeにparentが必要なので、ダミーのrootelem をここで作成。

	// ！！！！　rootelem の　serialno は必ず　０　！！！！
	s2e_leng = 0;
	s2e = 0;
	rootelem = rootlevel->AddElem( "root elem", 0 );
	if( !rootelem ){
		DbgOut( "CTreeHandler : constructor : rootlevel->AddElem error !!!\n" );
		return;
	}
	//ret = UpdateElemIndex( rootelem );

	ret = rootelem->SetType( SHDROOT );
	if( ret ){
		DbgOut( "CTreeHandler : constructor : rootelem SetMem BASE_TYPE error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	curseri = 0;
	befseri = 0;
	parseri = 0;
	curdepth = 0;
	
	addtype = ADDT_NONE;
	//befaddtype = ADDT_NONE;
}


int CTreeHandler::Add2DepthIndex( int srcdepth, CTreeLevel* srclevel )
{
	// CTreeLevelを　alloc するたびに呼び出して登録。

	HRESULT hres;
	CTreeLevel** levelarray = 0;
	int levelnum;


	if( (srcdepth > depthmax) || (isfirstAdd2Depth == 1) ){
		if( (srcdepth - depthmax) > 1 ){
			DbgOut( "CTreeHandler : Add2DepthIndex : diffdepth %d : depth growing warning !!!\n",
				srcdepth - depthmax );				
		}


		d2lnum = (int*)realloc( d2lnum, sizeof( int ) * (srcdepth + 1) );
		if( !d2lnum ){
			DbgOut( "CTreeHandler : Add2DepthIndex : d2lnum alloc error !!!\n" );
			return 1;
		}
		*(d2lnum + srcdepth) = 1;
		

		d2l = (CTreeLevel***)realloc( d2l, sizeof(CTreeLevel**) * (srcdepth + 1) );
		if( !d2l ){
			DbgOut( "CTreeHandler : Add2DepthIndex : d2l alloc error !!!\n" );
			return 1;
		}

		SetLastError( 0 );
		levelarray = (CTreeLevel**)malloc( sizeof( CTreeLevel* ) );
		if( !levelarray ){
			hres = GetLastError();
			ErrorMessage( "CTreeHandler : UpDataDepthIndex : levelarray alloc error !!! ", hres ); 
			return 1;
		}
		*levelarray = srclevel;		
		*(d2l + srcdepth) = levelarray;
		
		depthmax = srcdepth;

	}else{
		levelnum = *(d2lnum + srcdepth) + 1;
		levelarray = *(d2l + srcdepth);

DbgOut( "CTreeHandler : Add2DepthIndex : srcdepth %d, depthmax %d, levelnum %d\n", 
	   srcdepth, depthmax, levelnum );

		SetLastError( 0 );
		levelarray = (CTreeLevel**)realloc( levelarray, sizeof( CTreeLevel*) * levelnum );
		if( !levelarray ){
			hres = GetLastError();
			ErrorMessage( "CTreeHandler : UpDataDepthIndex : levelarray realloc error !!! ", hres );
			return 1;
		}

//DbgOut( "CTreeHandler : Add2DepthIndex : levelarray realloc %x\n", levelarray );

			// 格納
		*(d2lnum + srcdepth) = levelnum;
		*(levelarray + levelnum - 1) = srclevel;
		*(d2l + srcdepth) = levelarray;
	}

	isfirstAdd2Depth = 0;
	return 0;
}


int CTreeHandler::Add2ElemIndex( CTreeElem* elemptr )
{
		// CTreeElem を alloc するたびに呼び出して登録。
	int serino;

	serino = elemptr->serialno;

	if( serino >= s2e_leng ){

		s2e = (CTreeElem**)realloc( s2e, sizeof(CTreeElem*) * (serino + 1) );
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


CTreeHandler::~CTreeHandler()
{
	int i, j;
	CTreeLevel** levelarray = 0;
	CTreeLevel* del_level = 0;
	int levelnum;


	if( rootlevel ){
		// d2lの表の中身をdelete
		DbgOut( "CTreeHandler : destructor : depthmax %d\n", depthmax );
		for( i = 0; i <= depthmax; i++ ){
			levelnum = *(d2lnum + i);
			levelarray = *(d2l + i);
	
			DbgOut( "CTreeHandler : destructor : levelnum-%d %d\n", i, levelnum );

			for( j = 0; j < levelnum; j++ ){
				del_level = *(levelarray + j);
				if( del_level )
					delete del_level;
				else
					DbgOut( "CTreeHandler : destructor : del_level NULL warning %d %d !!!\n", i, j );
			}
			free( levelarray );
		}
	}

	DbgOut( "CTreeHandler : destructor : delete treelevel\n" );

	if( s2e ){
		free( s2e );
		s2e = 0;
	}
	s2e_leng = 0;

	DbgOut( "CTreeHandler : destructor : free s2e\n" );

	if( d2l ){
		// d2l表自体をfree
		free( d2l );
		d2l = 0;
	}

	DbgOut( "CTreeHandler : destructor : free d2l\n" );

	if( d2lnum ){
		free( d2lnum );
		d2lnum = 0;
	}
	depthmax = 0;

	DbgOut( "CTreeHandler : destructor : free d2lnum\n" );
}


/***
CTreeHandler::~CTreeHandler()
{
	int depthno;
	CTreeLevel** levelarray = 0;
	CTreeLevel* del_level = 0;
	//int levelnum;
	CTreeLevel** curarray = 0;

	if( rootlevel ){
		


	}

	DbgOut( "CTreeHandler : destructor : delete treelevel\n" );

	if( s2e ){
		free( s2e );
		s2e = 0;
	}
	s2e_leng = 0;

	DbgOut( "CTreeHandler : destructor : free s2e\n" );

	if( d2l ){
		// d2l表自体をfree
		for( depthno = 0; depthno < depthmax; depthno++ ){
			curarray = *(d2l + depthno);
			free( curarray );
		}

		free( d2l );
		d2l = 0;
	}

	DbgOut( "CTreeHandler : destructor : free d2l\n" );

	if( d2lnum ){
		free( d2lnum );
		d2lnum = 0;
	}
	depthmax = 0;

	DbgOut( "CTreeHandler : destructor : free d2lnum\n" );
}
***/
int CTreeHandler::AddTree( char* srcname, int srcserino )
{
		//serialno を返す。
		//no2ele, depth2ele へのセット
		//modeによって、同一nameへの対応を変える。
		//SetChain もする。
		//import時はserialnoを呼び出し側で指定。export時はallocate番号を自動的にセット。
	int ret;

	CTreeElem* parelem = 0;
	CTreeElem* befelem = 0;

	if( (mode & TMODE_IMPORT) && (srcserino < 0) ){
		DbgOut( "CTreeHandler : AddTree : serialno error %d !!!\n", srcserino );
		return -1;
	}
	
	// TMODE_MULT以外は　名前チェック
	if( mode & TMODE_ONCE ){
		// 全ての名前をチェックして、すでにあったら　アウト
		ret = Find( srcname );
		if( ret ){
			DbgOut( "CTreeHandler : AddTree : TMODE_ONCE : name check error !!!\n" );
			return -1;
		}
	}else if( mode & TMODE_LEVEL_ONCE ){
		// 同じLEVELの名前だけチェックして、すでにあったら　アウト
		ret = Find( srcname, curdepth );
		if( ret ){
			DbgOut( "CTreeHandler : AddTree : TMODE_LEVEL_ONCE : name check error !!!\n" );
			return -1;
		}
	}

	parelem = (*this)( parseri );
	befelem = (*this)( befseri );
	if( !parelem || !befelem ){
		DbgOut( "CTreeHandler : AddTree : invalid elemno error !!! : parseri %d, befseri %d\n",
			parseri, befseri );
		return -1;
	}

	switch( addtype ){
	case ADDT_DOWN:
		curseri = AddChild( parelem, srcname, curdepth, srcserino );
		//curseri = AddChild( parseri, srcname, curdepth, srcserino );
		DbgOut( "CTreeHandler : AddTree : AddChild :%d %s, parseri %d, curdepth %d\n", 
			curseri, srcname, parseri, curdepth );
		break;
	case ADDT_LEVEL:
		curseri = AddBrother( befelem, srcname, srcserino );
		DbgOut( "CTreeHandler : AddTree : AddBrother : %d %s, parseri %d, curdepth %d\n", 
			curseri, srcname, parseri, curdepth );
		break;

	case ADDT_UP:
	default:
		// error
		curseri = -1;
		break;
	}

	//befaddtype = addtype;
	befseri = curseri;

	_ASSERT( curseri > 0 );
	return curseri;

}

int CTreeHandler::AddBrother( CTreeElem* srcbefelem, char* srcname, int srcserino )
{
	CTreeLevel* joinlevel = 0;
	CTreeElem* newelem = 0;

	DbgOut( "CTreeHandler : AddBrother\n" );

	joinlevel = srcbefelem->owner;
	if( joinlevel ){

		newelem = joinlevel->AddElem( srcname, srcserino );
		if( !newelem ){
			DbgOut( "CTreeHandler : AddTree : joinlevel->AddElem error !!!\n" );
			return -1;
		}
		return newelem->serialno;

	}else{
		return -1;
	}
}

int CTreeHandler::AddChild( CTreeElem* srcparelem, char* srcname, int srcdepth, int srcserino )
//int CTreeHandler::AddChild( int srcparno, char* srcname, int srcdepth, int srcserino )
{
	CTreeLevel* newlevel = 0;
	CTreeElem* newelem = 0;
	int ret;
	DbgOut( "CTreeHandler : AddChild\n" );

	newlevel = new CTreeLevel( this, mode, srcdepth, srcparelem->serialno );
	if( !newlevel ){
		DbgOut( "CTreeHandler : AddTree : newlevel alloc error !!!\n" );
		return -1;
	}

	ret = srcparelem->SetChild( newlevel );
	if( ret ){
		DbgOut( "CTreeHandler : AddTree : srcparelem->SetChild error !!!\n" );
		return -1;
	}
	DbgOut( "CTreeHandler : AddTree : newlevel->AddElem : srcname %s, srcserino %d\n", srcname, srcserino );

	newelem = newlevel->AddElem( srcname, srcserino );
	if( !newelem ){
		DbgOut( "CTreeHandler : AddTree : newlevel->AddElem error !!!\n" );
		return -1;
	}

	return newelem->serialno;
}



int CTreeHandler::Find( char* srcname, int srcdepth )
{

		//対応する一意なTreeを求めるのではなく、同じ名前が既にあるかどうかのチェックにしか使わない。				
		//たぶん、treeの作成時のチェックだけに使用。
		// depth 指定時には、同一level内のみチェックする。
	int i, j, levelnum, elemnum;
	CTreeLevel** levelarray = 0;
	CTreeLevel* chklevel = 0;
	CTreeElem* chkelem = 0;
	//CTreeElem* chklevelptr = 0;

	if( srcdepth < 0 ){
		for( i = 0; i < s2e_leng; i++ ){
			chkelem = (*this)( i );
			if( strcmp( srcname, chkelem->name ) == 0 ){
				return 1;
			}
		}
	}else{
		levelnum = *(d2lnum + srcdepth);
		levelarray = *(d2l + srcdepth);
		for( i = 0; i < levelnum; i++ ){
			chklevel = *(levelarray + i);
			//chklevelptr = chklevel->re_elemptr;
			elemnum = chklevel->levelleng;

			for( j = 0; j < elemnum; j++ ){
				//chkelem = chklevelptr + j;
				chkelem = (*chklevel)( j );
				if( strcmp( srcname, chkelem->name ) == 0 ){
					return 1;
				}
			}
		}
	}
	return 0;
}


int CTreeHandler::CheckMode( int srcmode )
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

	return 0; // 0 が正常なの　おほほほほ
}

int CTreeHandler::CountBits( int chkbits )
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

int CTreeHandler::Dbg_DumpTree()
{
	HANDLE hfile;
	int ret = 0;
	CTreeElem* rootelem = 0;
	int errcnt = 0;

	hfile = CreateFile( (LPCTSTR)"dbgdump.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 1;
	}

	
	rootelem = (*this)( 0 );

	if( rootelem ){
		rootelem->Dbg_DumpReq( hfile, errcnt );
		_ASSERT( !errcnt );
		if( errcnt ){
			DbgOut( "CTreeHandler : Dbg_DumpTree : Dbg_DumpReq error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "CTreeHandler : Dbg_DumpTree : rootelem error !!!\n" );
		return 1;
	}
	

	FlushFileBuffers( hfile );
	CloseHandle( hfile );
	
	DbgOut( "CTreeHandler : DumpTree : CloseHandle\n" );

	return ret;

}


int CTreeHandler::DumpTree( int srcmode )
{
	HANDLE hfile;
	int ret;

	hfile = CreateFile( (LPCTSTR)"dumptest.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 1;
	}

	if( srcmode & DUMP_TEXT ){
		ret =  rootlevel->DumpText( hfile );

	}else if( srcmode & DUMP_BINARY ){
		//ret = DumpBinary( hfile );
		ret = 1;
	}


	FlushFileBuffers( hfile );
	CloseHandle( hfile );
	
	DbgOut( "CTreeHandler : DumpTree : CloseHandle\n" );

	return ret;
}

/***
int CTreeHandler::DumpTree( int srcmode, int* ar_handler, unsigned long* ar_hand_ptr )
{
	HANDLE hfile;
	int ret;

	hfile = CreateFile( "dumptest.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		return 1;
	}

	if( srcmode & DUMP_TEXT ){
		ret =  rootlevel->DumpText( hfile, ar_handler, ar_hand_ptr );

	}else{
		// バイナリ
		// 未実装
		ret = 1;
	}

	FlushFileBuffers( hfile );
	CloseHandle( hfile );
	
	DbgOut( "CTreeHandler : DumpTree : CloseHandle\n" );

	return ret;
}
***/


//int CTreeHandler::DumpText( HANDLE hfile )
//{
//	int ret;
//
//	ret = rootlevel->DumpText( hfile );
//	
//	return ret;
//}

//int CTreeHandler::DumpBinary( HANDLE hfile )
//{
//	return 1;
//}

int CTreeHandler::GetCurDepth()
{
	return curdepth;
}
int CTreeHandler::Start( int offset )
{

	//curseri = 0;
	//befseri = 0;
	if( offset == 0 ){
		curseri = 0;
	}else{
		curseri = offset;
	}

	befseri = 0;
	parseri = 0;
	curdepth = 0;
	addtype = ADDT_NONE;
	return 0;
}

int CTreeHandler::Begin()
{
	CTreeElem* parelem = 0;
	CTreeElem* befelem = 0;
	//CTreeElem* parofbef = 0;
	int pardepth = 0;
	int befdepth = 0;
	int savedepth = curdepth;
	int diffdepth = 0;
	int ret = 0;

	(curdepth)++;

	if( parseri >= 0 ){
		parelem = (*this)( parseri );
	}else{
		parelem = (*this)( 0 );
		parseri = 0;
	}

	befelem = (*this)( befseri );
	if( !parelem || !befelem ){
		DbgOut( "CTreeHandler : Begin : serialno error %d %d !!!\n",
			parseri, befseri );
		return 1;
	}
	//parofbef = befelem->parent;
	pardepth = parelem->GetDepth();
	befdepth = befelem->GetDepth();
	diffdepth = curdepth - befdepth;

	switch( addtype ){
	case ADDT_NONE:
		//初回
		if( (parseri == 0) && (befseri == 0) && (curdepth == 1) ){
			//addtype = ADDT_DOWN;
			
			if( (curseri == 0) || (curseri == 1) ){
				addtype = ADDT_DOWN;
			}else{
				//import時の先頭データ
DbgOut( "TreeHandler : Begin : Import head data %d\n", curseri );
				befseri = 1;
				befelem = (*this)( befseri );
				addtype = ADDT_LEVEL;
			}
			
		}else{
			DbgOut( "CTreeHandler : Begin : case ADDT_NONE : params error %d %d %d !!!\n", parseri, befseri, curdepth );
			ret = 1;
		}

		break;
	case ADDT_UP:
		if( diffdepth == 0 ){ // up のあとは、0
			parseri = curseri;
			addtype = ADDT_LEVEL;
		}else{
			// error
			addtype = ADDT_NONE;
			DbgOut( "CTreeHandler : Begin : case ADDT_UP : diffdepth error %d , %d %d!!!\n", 
				diffdepth, curdepth, befdepth );
			ret = 1;				
		}
		
		break;

	case ADDT_DOWN:
	case ADDT_LEVEL:
		if( diffdepth == 1 ){ // down のあとは、diffは正
			parseri = curseri;
			addtype = ADDT_DOWN;
		}else{
			// error
			addtype = ADDT_NONE;
			DbgOut( "CTreeHandler : Begin : case ADDT_DOWN, LEVEL : diffdepth error %d!!!\n", diffdepth );
			ret = 1;
		}

		break;
	default:
		// error
		addtype = ADDT_NONE;
		DbgOut( "CTreeHandler : Begin : addtype error %d !!!\n", addtype );
		ret = 1;
		break;
	}

	return ret;
}

int CTreeHandler::End()
{
	int ret = 0;

	(curdepth)--;

	switch( addtype ){
	case ADDT_NONE:
		//初回
		ret = 1;
		break;
	case ADDT_UP:
	case ADDT_DOWN:
	case ADDT_LEVEL:
		if( addtype == ADDT_UP ){
			befseri = curseri;
			curseri = parseri;
		}else{
			curseri = parseri;
		}

		if( parseri >= 0 ){
			parseri = GetParentSeriNo( parseri );
			if( parseri < 0 ){
				DbgOut( "CTreeHandler : End : GetParent reached to vacume warning !!!\n" );
			}
			addtype = ADDT_UP;
		}else{
			DbgOut( "CTreeHandler : End : parseri error %d !!!\n", parseri );
			ret = 1;
		}

		DbgOut( "CTreeHandler : End : addtype %d, curseri %d, befseri %d, parseri %d\n",
			addtype, curseri, befseri, parseri );
		
		break;
	default:
		ret = 1;
		DbgOut( "CTreeHandler : End : addtype error %d !!!\n", addtype );
		break;
	}
		
	return ret;

}






