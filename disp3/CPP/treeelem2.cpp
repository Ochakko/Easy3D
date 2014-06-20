#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <coef.h>
#include <treehandler2.h>
#include <treeelem2.h>

#ifndef DBGH
	#define	DBGH
#endif
#include <dbg.h>

/***
HANDLE	CTreeElem2::s_hHeap = NULL;
unsigned int	CTreeElem2::s_uNumAllocsInHeap = 0;

unsigned int	CTreeElem2::s_DataNo = 0;
void*	CTreeElem2::s_DataHead = 0;

int		CTreeElem2::s_HeadNum = 0;
BYTE*	CTreeElem2::s_HeadIndex[2048];

int	CTreeElem2::s_useflag[2048][TE2_BLOCKLENG];
***/

/***
void	*CTreeElem2::operator new ( size_t size )
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

void	CTreeElem2::operator delete ( void *p )
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
***/

/***
void	*CTreeElem2::operator new ( size_t size )
{

	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
		if( s_hHeap == NULL )
			return NULL;
		ZeroMemory( s_HeadIndex, sizeof( BYTE* ) * 2048 );
		ZeroMemory( s_useflag, sizeof( int ) * 2048 * TE2_BLOCKLENG );
	}


	if( !s_DataHead || (s_DataNo >= TE2_BLOCKLENG) ){
		if( s_HeadNum >= 2048 ){
			_ASSERT( 0 );
			DbgOut( "CTreeElem2 : operator new : s_HeadNum too large error !!!\n" );
			return NULL;
		}

		s_DataHead = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size * TE2_BLOCKLENG );
		if( !s_DataHead ){
			_ASSERT( 0 );
			return NULL;
		}
		s_DataNo = 0;

		s_HeadIndex[s_HeadNum] = (BYTE*)s_DataHead;
		s_HeadNum++;
	}

	void* p;
	p = (void*)((BYTE*)s_DataHead + size * s_DataNo);
	if( p != NULL ){
		s_uNumAllocsInHeap++;
		s_DataNo++;
		s_useflag[s_HeadNum - 1][s_DataNo - 1] = 1;
	}

	return (p);
}

void	CTreeElem2::operator delete ( void *p )
{
		
	s_uNumAllocsInHeap--;

	int headno;
	void* delhead = 0;
	int delno = -1;
	//pより小さい中で、一番大きいheadindexを探す。
	for( headno = 0; headno < s_HeadNum; headno++ ){
		void* headptr = (void*)(s_HeadIndex[headno]);
		if( ((unsigned long)p >= (unsigned long)headptr) 
			&& ((unsigned long)delhead < (unsigned long)headptr) ){
			delhead = headptr;
			delno = headno;
		}
	}
	if( delno < 0 ){
		_ASSERT( 0 );
		DbgOut( "TreeElem2 : operator delete invalid address error !!!\n" );
		return;
	}
	int datano;
	datano = ((unsigned long)p - (unsigned long)delhead) / sizeof( CTreeElem2 );
	_ASSERT( (datano >= 0) && (datano < TE2_BLOCKLENG) ); 
	s_useflag[delno][datano] = 0;
	
	int usecnt = 0;
	int chkno;
	for( chkno = 0; chkno < TE2_BLOCKLENG; chkno++ ){
		if( s_useflag[delno][chkno] == 1 )
			usecnt++;
	}
	if( usecnt == 0 ){
		HeapFree( s_hHeap, HEAP_NO_SERIALIZE, delhead );
		s_HeadIndex[delno] = 0;

		DbgOut( "TreeElem2 : operator delete : HeapFree %d\n", delno );

		if( delno == (s_HeadNum - 1) ){
			//最後のHeadIndexを消す場合の処理。
			s_DataHead = 0;
			DbgOut( "TreeElem2 : operator delete : last heap\n" );
		}
	}


	//if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
	//	s_uNumAllocsInHeap--;
	//}

	if( s_uNumAllocsInHeap == 0 ){		
		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
		DbgOut( "TreeElem2 : operator delete : HeapDestroy : %d\n", s_HeadNum );
	}
}
***/


CTreeElem2::CTreeElem2( CTreeHandler2* lphandler, int srcmode, char* srcname, CTreeElem2* srcparent, int srcserino )
{
	lpthandler = lphandler;
	name = 0;
	engname = 0;

	mode = srcmode;
	if( srcserino < 0 ){
		DbgOut( "CTreeElem : Constructor : TMODE_IMPORT : serino not set error !!!\n" );
		_ASSERT( 0 );
	}
	serialno = srcserino;

	if( !srcname ){
		DbgOut( "CTreeElem : constructor : srcname NULL error !!!\n" );
		_ASSERT( 0 );
	}
	int nameleng = (int)strlen( srcname );
	name = (char*)realloc( name, nameleng + 1 );
	ZeroMemory( name, nameleng + 1 );
	strncpy_s( name, nameleng + 1, srcname, nameleng );
	
	pridatanum = 0;
	privateptr = 0; // 付加データ

	SetChain( srcparent );
	

}

CTreeElem2::~CTreeElem2()
{
	DestroyObjs();
}
void CTreeElem2::DestroyObjs()
{
	int datano;
	unsigned long dataval;

	if( name ){
		free( name );
		name = 0;
	}
	if( engname ){
		free( engname );
		engname = 0;
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

int CTreeElem2::SetType( int srctype )
{
	type = srctype;
	return 0;
}
int CTreeElem2::SetName( char* srcname )
{
	int nameleng;
	if( !srcname ){
		DbgOut( "CTreeElem : SetName : srcname NULL warning !!!\n" );
		name = (char*)realloc( name, 0 );
		return 0;

	}else{
		nameleng = (int)strlen( srcname );
		name = (char*)realloc( name, nameleng + 1 );
		if( !name ){
			DbgOut( "CTreeElem : SetName : name realloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( name, nameleng + 1 );
		strncpy_s( name, nameleng + 1, srcname, nameleng );
		return 0;
	}

	return 0;
}
int CTreeElem2::SetEngName( char* srcname )
{
	int nameleng;
	if( !srcname ){
		DbgOut( "CTreeElem : SetEngName : srcname NULL warning !!!\n" );
		engname = (char*)realloc( engname, 0 );
		return 0;

	}else{
		nameleng = (int)strlen( srcname );
		engname = (char*)realloc( engname, nameleng + 1 );
		if( !engname ){
			DbgOut( "CTreeElem : SetEngName : engname realloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( engname, nameleng + 1 );
		strncpy_s( engname, nameleng + 1, srcname, nameleng );
		return 0;
	}

	return 0;
}


int CTreeElem2::SetParent( CTreeElem2* srcparent )
{
	parent = srcparent;
	return 0;
}
int CTreeElem2::SetBroNo( int srcbrono )
{
	brono = srcbrono;
	return 0;
}
int CTreeElem2::SetSerialNo( int srcserino )
{
	serialno = srcserino;
	return 0;
}

int CTreeElem2::GetDepth()
{
	return depth;
}

int CTreeElem2::AddPrivateData( unsigned long srcpridata )
{
	pridatanum++;
	
	privateptr = (unsigned long*)realloc( privateptr, sizeof( unsigned long) * pridatanum );
	if ( !privateptr ){
		_ASSERT( 0 );
		DbgOut( "CTreeElem : AddPrivateData privatptr realloc error !!!\n" );
		return 1;
	}

	*(privateptr + pridatanum - 1) = srcpridata;


	return 0;
}

int CTreeElem2::DumpText( HANDLE hfile )
{

	return 0;
}
void CTreeElem2::Dbg_DumpReq( HANDLE hfile, int errcnt )
{

}
void CTreeElem2::DumpReq( HANDLE hfile, int* errcnt )
{

}

int CTreeElem2::SetChain( CTreeElem2* srcparent )
{
	parent = srcparent;
	
	CTreeElem2* chilelem;

	if( parent ){
		chilelem = parent->child;
		if( chilelem ){
			int brocnt = 0;
			CTreeElem2* broelem  = chilelem;
			CTreeElem2* lastbroelem = broelem;
			while( broelem ){
				brocnt++;
				broelem = broelem->brother;
				if( broelem )
					lastbroelem = broelem;
			}
			lastbroelem->brother = this;
			brother = 0;
			sister = lastbroelem;
			brono = brocnt;

		}else{
			parent->child = this;
			brother = 0;
			sister = 0;
			brono = 0;
		}
	}else{
		//parent == 0
		brother = 0;
		sister = 0;
		brono = 0;
	}

	child = 0;

	int depthcnt = 0;
	CTreeElem2* parelem = srcparent;
	while( parelem ){
		depthcnt++;
		parelem = parelem->parent;
	}
	depth = depthcnt;

	return 0;
}

int CTreeElem2::IsJoint()
{
	//if( (type >= SHDROTATE) && (type <= SHDMORPH) )
	if( (type >= SHDROTATE) && (type <= SHDBALLJOINT) && (type != SHDMORPH) )
		return 1;
	else
		return 0;

}

int CTreeElem2::GetDispObjApplyName( char* applyname )
{
	*applyname = 0;

	if( *name == 0 )
		return 0;
	
	char* minusptr;
	int ch2 = '-';
	minusptr = strchr( name, ch2 );
	if( minusptr ){
		strcpy_s( applyname, 256, minusptr + 1 );
	}else{
		*applyname = 0;
	}

	return 0;
}


int CTreeElem2::IsAnchorName( char* targetname, char* applyname )
{
	*targetname = 0;
	*applyname = 0;

	if( *name == 0 )
		return 0;

	int cmp0, cmp1;
	cmp0 = strncmp( name, "anchor", 6 );
	if( cmp0 != 0 )
		return 0;

	char* barptr;
	int ch = '|';
	barptr = strchr( name, ch );
	if( !barptr )
		return 0;

	
	char* minusptr;
	int ch2 = '-';
	minusptr = strchr( barptr, ch2 );
	if( !minusptr ){
		
		strcpy_s( targetname, 256, barptr + 1 );
		*applyname = 0;

		return 3;//適用ボーン無し
	}else{
		int leng;
		leng = (int)strlen( name );

		int targetleng;
		targetleng = (int)( minusptr - barptr - 1 );

		strncpy_s( targetname, 256, barptr + 1, targetleng );
		*( targetname + targetleng ) = 0;

		strcpy_s( applyname, 256, minusptr + 1 );

		if( leng > 2 )
			cmp1 = strcmp( name + leng - 2, "[]" );
		else
			cmp1 = 1;

		if( cmp1 ){			
			return 1;//[]ではない適用ボーン
		}else{
			return 2;//[]の適用ボーン
		}

	}

}
