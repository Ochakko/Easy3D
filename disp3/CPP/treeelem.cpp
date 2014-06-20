#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <coef.h>
#include <treehandler.h>
//#include <shdhandler.h>

#ifndef DBGH
	#define	DBGH
#endif
#include "dbg.h"


///////////////////////////
// CTreeElem
///////////////////////////

int CTreeElem::allocno = 0;

HANDLE	CTreeElem::s_hHeap = NULL;
unsigned int	CTreeElem::s_uNumAllocsInHeap = 0;

#ifdef _DEBUG
	void	*CTreeElem::operator new ( size_t size )
	{

		if( s_hHeap == NULL ){
			s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
			//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
			
			if( s_hHeap == NULL )
				return NULL;
		}

		void	*p;
		p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size );
			
		if( p != NULL ){
			s_uNumAllocsInHeap++;
		}

		return (p);
	}

	void	CTreeElem::operator delete ( void *p )
	{
		
		if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
			s_uNumAllocsInHeap--;
		}

		if( s_uNumAllocsInHeap == 0 ){
			if( HeapDestroy( s_hHeap ) ){
				s_hHeap = NULL;
			}
		}
	}
#endif

/***
CTreeElem::CTreeElem()
{
	// HeapReAlloc から配列としてallocするときのコンストラクタ

	// 長さが増えた分だけ、目的のobjectを初期化してくれるほど、親切ではないらしい。
	// 配列の先頭のobject（既存のものであっても）に対して、実行されてしまうので、空にしておく。


	//allocno++;
//DbgOut( "CTreeElem::CTreeElem : allocno++ %d\n", allocno );

}
***/

CTreeElem::CTreeElem( CTreeHandler* lphandler, int srcmode, int srcbrono, char* srcname, CTreeLevel* srcowner, int srcserino )
{
		// CTreeLevel->AddElem　から呼ばれることを想定したconstructor。
		// srcserino は　import時のみ有効。
	int ret;

	if( (srcmode & TMODE_EXPORT) &&  (srcserino < 0) ){
		srcserino = allocno;
	}

	ret = SetParams( lphandler, srcmode, srcbrono, srcname, srcowner, srcserino );
	if( ret ){
		DbgOut( "CTreeElem : constructor : SetParams error !!!\n" );
	}

	allocno++;
DbgOut( "CTreeElem::CTreeElem : param ver : allocno++ %d\n", allocno );

	return;
}

int CTreeElem::SetParams( CTreeHandler* lphandler, int srcmode, int srcbrono, char* srcname, CTreeLevel* srcowner, int srcserino )
{
	int ret, nameleng;

	lpthandler = lphandler;
	_ASSERT( lpthandler );

	mode = srcmode;
	brono = srcbrono; // ownter->levelarray 中で自分を指すindexno 

	// allocno は既に++されているので使うときは注意　！！！
	if( (mode & TMODE_IMPORT) && (srcserino < 0) ){
		DbgOut( "CTreeElem : SetParams : TMODE_IMPORT : serino not set error !!!\n" );
		goto elemerror;
	}

	if( srcserino >= 0 )
		serialno = srcserino;

DbgOut( "CTreeElem : SetParams : srcserino %d, serialno %d\n", srcserino, serialno );

	type = 0;

	if( !srcname ){
		DbgOut( "CTreeElem : constructor : srcname NULL error !!!\n" );
		goto elemerror;
	}
	nameleng = strlen( srcname );
	name = (char*)malloc( nameleng + 1 );
	ZeroMemory( name, nameleng + 1 );
	strncpy( name, srcname, nameleng );


	pridatanum = 0;
	privateptr = 0; // 付加データ

	if( !srcowner ){
		DbgOut( "CTreeElem : constructor : srcowner NULL error !!!\n" );
		goto elemerror;
	}
	owner = srcowner;

	//ret = SetChain();
	ret = InitChain();
	if( ret ){
		DbgOut( "CTreeElem : constructor : InitChain error !!!\n" );
		goto elemerror;
	}

	return 0;

elemerror:
	mode = 0;
	serialno = -1;
	owner = 0;
	return 1;

}

CTreeElem::~CTreeElem()
{

	DbgOut( "CTreeElem : destructor : %x %s %d\n", this, name, serialno );

	DestroyObjs();
}

int CTreeElem::InitChain()
{

	if( !owner ){
		DbgOut( "CTreeElem : InitChain : owner NULL error !!!\n" );
		return 1;		
	}

	parseri = owner->parseri;
	child = 0;
	
	broseri = -1;
	sisseri = -1;

	return 0;
}

int CTreeElem::SetChain()
{
	// tibrother, tisisterのセット

	CTreeElem* sisptr = 0;
	CTreeElem* broptr = 0;
	int bronum = 0;
	//int sisno = 0;
	//int brono = 0;


	if( brono > 0 ){
		sisptr = (*owner)( brono - 1 );
		sisseri = sisptr->serialno;

		sisptr->broseri = serialno;
	}else{
		sisseri = -1;
	}

	bronum = owner->levelleng;
	if( bronum > brono + 1 ){
		DbgOut( "CTreeElem : SetChain : set brother warning %d!!\n", serialno ); //通常chainの途中にはセットしない。

		broptr = (*owner)( brono + 1 );
		broseri = broptr->serialno;

		(*lpthandler)( broseri )->sisseri = serialno;
	}else{
		broseri = -1;
	}

	return 0;
}


int CTreeElem::SetChild( CTreeLevel* srcchild )
{
	child = srcchild;
	return 0;
}

int CTreeElem::SetType( int srctype )
{
	type = srctype;
	return 0;
}

int CTreeElem::SetName( char* srcname )
{
	int nameleng;
	if( !srcname ){
		DbgOut( "CTreeElem : SetName : srcname NULL warning !!!\n" );
		name = (char*)realloc( name, 0 );
		return 0;

	}else{
		nameleng = strlen( srcname );
		name = (char*)realloc( name, nameleng + 1 );
		if( !name ){
			DbgOut( "CTreeElem : SetName : name realloc error !!!\n" );
			return 1;
		}
		ZeroMemory( name, nameleng + 1 );
		strncpy( name, srcname, nameleng );
		return 0;
	}

}
int CTreeElem::SetOwner( CTreeLevel* srcowner )
{
	owner = srcowner;
	return 0;
}
int CTreeElem::SetBroNo( int srcbrono )
{
	brono = srcbrono;
	return 0;
}
int CTreeElem::SetSerialNo( int srcserino )
{
	serialno = srcserino;
	return 0;
}

int CTreeElem::AddPrivateData( unsigned long srcpridata )
{
	pridatanum++;
	
	privateptr = (unsigned long*)realloc( privateptr, sizeof( unsigned long) * pridatanum );
	if ( !privateptr ){
		DbgOut( "CTreeElem : AddPrivateData privatptr realloc error !!!\n" );
		return 1;
	}

	*(privateptr + pridatanum - 1) = srcpridata;

	return 0;
}

void CTreeElem::Dbg_DumpReq( HANDLE hfile, int errcnt )
{
	int ret;
	CTreeElem* chilelem = 0;
	CTreeElem* broptr = 0;

	if( errcnt != 0 )
		return;

	ret = DumpText( hfile );
	errcnt += ret;

	if( child ){
		chilelem = (*child)( 0 );
		_ASSERT( chilelem );
		
		chilelem->Dbg_DumpReq( hfile, errcnt );

	}

	if( broseri > 0 ){
		broptr = (*lpthandler)( broseri );
		_ASSERT( broptr );
		broptr->Dbg_DumpReq( hfile, errcnt );
		
	}
		
}

void CTreeElem::DumpReq( HANDLE hfile, int* errcnt )
{
	int ret;

	if( *errcnt != 0 )
		return;

	ret = DumpText( hfile );
	*errcnt += ret;

	if( child )
		*errcnt += child->DumpText( hfile );
	
}

/***
void CTreeElem::DumpReq( HANDLE hfile, int* ar_handle, unsigned long* ar_hand_ptr, int* errcnt )
{
	int ret;

	if( *errcnt != 0 )
		return;

	//ret = DumpText( hfile );
	ret = DumpText( hfile, ar_handle, ar_hand_ptr );
	*errcnt += ret;


	if( child )
		*errcnt += child->DumpText( hfile, ar_handle, ar_hand_ptr );
	

}
***/


int CTreeElem::DumpText( HANDLE hfile )
{
	int ret = 0;
	int depth;

	int handleno = 0;
	int handlerkind = 0;
	//CShdHandler* shandlerptr = 0;

	if( !owner ){
		DbgOut( "CTreeElem : DumpText : owner NULL error !!!\n" );
		return 1;
	}

	depth = owner->depth;

	SetTabChar( depth + 1 );
	ret = Write2File( hfile, "%s serialno %d, type %s, name %s, brono %d\n",
		tabchar, serialno, typechar[ type ], name, brono );
	_ASSERT( !ret );

//// dbg
	//if( brono == 0 ){
	//	_ASSERT( owner );
	//	ret = Write2File( hfile, "%s    brono == 0 : levelleng %d\n", tabchar, owner->levelleng );
	//	_ASSERT( !ret );
	//}
////

	return ret;
}

int CTreeElem::GetDepth()
{
	if( !owner ){
		DbgOut( "CTreeElem : GetDepth : owner NULL error !!!\n" );
		return -1;
	}

	return owner->depth;
}

void CTreeElem::DestroyObjs()
{
	int datano;
	unsigned long dataval;

	if( name ){
		free( name );
		name = 0;
	}
	if( privateptr ){
		for( datano = 0; datano < pridatanum; datano++ ){
			dataval = *(privateptr + datano);
			if( dataval != 0 )
				DbgOut( "CTreeElem : destructor : private data[%d] still exists warning !!!!\n", datano );
		}

		free( privateptr );
		privateptr = 0;
	}

}