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

#define	DBGH
#include "dbg.h"


///////////////////////////
// CTreeLevel
///////////////////////////

HANDLE	CTreeLevel::s_hHeap = NULL;
unsigned int	CTreeLevel::s_uNumAllocsInHeap = 0;

#ifdef _DEBUG
	void	*CTreeLevel::operator new ( size_t size )
	{

		if( s_hHeap == NULL ){
			s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
			//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
			
			if( s_hHeap == NULL )
				return NULL;
		}

		void	*p;
		p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, size );
			
		if( p != NULL ){
			s_uNumAllocsInHeap++;
		}

		return (p);
	}


	void	CTreeLevel::operator delete ( void *p )
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

CTreeLevel::CTreeLevel( CTreeHandler* lphandler, int srcmode, int srcdepth, int srcparno )
{
	// srcmode は CTreeHandler でチェック済みのはず。
	int ret;

	SetParams( srcmode, srcdepth, lphandler, srcparno );

	if( lpthandler ){
		ret = lpthandler->Add2DepthIndex( srcdepth, this );
		if( ret ){
			DbgOut( "CTreeLevel : constructor : lpthandler->Add2DepthIndex error !!!\n" );
			_ASSERT( 0 );
		}
	}
}

CTreeLevel::~CTreeLevel()
{
	static int dbgcnt = 0;
	CTreeElem* del_elem = 0;
	int i;

	if( helem ){
		for( i = 0; i < levelleng; i++ ){
			del_elem = (*this)( i );
			del_elem->DestroyObjs();
		}
		free( helem );
		helem = 0;
	}
	levelleng = 0;

	dbgcnt++;
}


int CTreeLevel::SetParams( int srcmode, int srcdepth, CTreeHandler* lphandler, int srcparno )
{
	CTreeElem* parptr = 0;
	mode = srcmode;
	depth = srcdepth;

	lpthandler = lphandler;
	_ASSERT( lpthandler );

	parseri = srcparno;

	if( parseri >= 0 ){
		parptr = (*lpthandler)( parseri );
		parptr->child = this;
	}else{
		DbgOut( "CTreeLevel : SetParams : parent NULL warning !!!\n" );
	}


	levelleng = 0;
	helem = 0;

	return 0;
}

int CTreeLevel::SetChain()
{
	int ret = 0;
	int i;
	CTreeElem* curelem = 0;

	for( i = 0; i < levelleng; i++ ){
		curelem = (*this)( i );
		curelem->owner = this;
		ret += curelem->SetChain();
	}
	_ASSERT( !ret );
	return ret;
}

CTreeElem* CTreeLevel::AddElem( char* srcname, int srcserino )
{
		// srcserino は　import時のみ。
	CTreeElem* newelem = 0;
	int brono, ret;

	_ASSERT( lpthandler );

	if( (mode & TMODE_IMPORT) && (srcserino < 0) ){
		DbgOut( "CTreeLevel : AddElem : TMODE_IMPORT : srcserino error !!!\n" );
		return 0;
	}

DbgOut( "CtreeLevel : AddElem : srcserino %d\n", srcserino );

	levelleng++;
	brono = levelleng - 1;

	helem = (CTreeElem**)realloc( helem, sizeof( CTreeElem* ) * levelleng );
	if( !helem ){
		DbgOut( "CTreeLevel : AddElem : helem realloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	newelem = new CTreeElem( lpthandler, mode, brono, srcname, this, srcserino );
	if( !newelem ){
		DbgOut( "CTreeLevel : AddElem : newelem alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	*(helem + brono) = newelem;


	ret = lpthandler->Add2ElemIndex( newelem );
	if( ret ){
		DbgOut( "CTreeLevel : AddElem : lpthandler->Add2ElemIndex error !!!\n" );
		return 0;
	}

	ret = newelem->SetChain();// Add2ElemIndexより後。
	if( ret ){
		DbgOut( "CTreeLevel : AddElem : newelem->SetChain error !!!\n" );
		return 0;
	}

	return newelem;
}


int CTreeLevel::DumpText( HANDLE hfile )
{
	int ret = 0;
	int i;
	CTreeElem* dumpelem = 0;

	SetTabChar( depth );
	ret = Write2File( hfile, "%s CTreeLevel: depth %d, bronum %d\n", 
		tabchar, depth, levelleng );
	if( ret ){
		DbgOut( "CTreeLevel : DumpText : Write2File error !!!\n" );
		_ASSERT( 0 );
	}

	for( i = 0; i < levelleng; i++ ){
		dumpelem = (*this)( i );
		dumpelem->DumpReq( hfile, &ret );
		_ASSERT( !ret );
	}
	return ret;
}
