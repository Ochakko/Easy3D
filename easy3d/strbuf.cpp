
//
//	HSP3 string support
//	(おおらかなメモリ管理をするバッファマネージャー)
//	(sbAllocでSTRBUF_BLOCKSIZEのバッファを確保します)
//	(あとはsbCopy,sbAddで自動的にバッファの再確保を行ないます)
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "supio.h"
#include "strbuf.h"

#define REALLOC realloc
#define MALLOC malloc
#define FREE free

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static int str_cur;
static int str_max;
static int str_blockcur;
static STRBUF **mem_sb;
static STRBUF **mem_ptr;

#define GETBUF(num) (mem_ptr[num])
#define GETINF(num) (&(mem_ptr[num]->inf))
#define GETDATA(num) (&(mem_ptr[num]->data))

/*------------------------------------------------------------*/
/*
		internal function
*/
/*------------------------------------------------------------*/

static void BlockPtrPrepare( void )
{
	int i;
	int total;
	STRINF *inf;
	STRBUF *sb;

	sb = (STRBUF *)MALLOC( sizeof(STRBUF) * STRBUF_BLOCK_DEFAULT );
	if ( sb == NULL ) Alert( "Memory Overflow" );
	str_cur = str_max;
	total = str_max + STRBUF_BLOCK_DEFAULT;

	if ( str_blockcur == 0 ) {
		mem_sb = (STRBUF **)MALLOC( sizeof(void *) );
		mem_ptr = (STRBUF **)MALLOC( sizeof(void *) * total );
	} else {
		mem_sb = (STRBUF **)REALLOC( mem_sb, sizeof(void *) * ( str_blockcur + 1 ) );
		mem_ptr = (STRBUF **)REALLOC( mem_ptr, sizeof(void *) * total );
	}

	for( i=0; i<STRBUF_BLOCK_DEFAULT; i++ ) {
		mem_ptr[str_max] = &(sb[i]);
		inf = GETINF(str_max);
		inf->myblock = str_max;
		inf->flag = STRINF_FLAG_NONE;
		str_max++;
	}

	mem_sb[ str_blockcur ] = sb;
	str_blockcur++;
}


static int BlockEntry( void )
{
	//		空きエントリーブロックを探す
	//
	int i;
	STRINF *inf;
	i = 0;
	while(1) {
		inf = GETINF(str_cur);
		if ( inf->flag == STRINF_FLAG_NONE ) return str_cur;
		str_cur++;if ( str_cur >= str_max ) str_cur = 0;
		i++; if ( i >= str_max ) break;
	}

	//		エントリーブロックを拡張する
	//
	BlockPtrPrepare();

	//Alertf( "Expand:%d",str_max );
	return str_cur;

	//Alert( "Memory Overflow" );
	//return -1;
}

static char *BlockAlloc( int size )
{
	int i;
	int *p;
	STRBUF *st;
	STRBUF *st2;
	STRINF *inf;
	i = BlockEntry();
	if ( i < 0 ) return NULL;
	st = GETBUF(i);
	inf = &(st->inf);
	if ( size <= STRBUF_BLOCKSIZE ) {
		inf->flag = STRINF_FLAG_USEINT;
		inf->size = STRBUF_BLOCKSIZE;
		p = (int *)st->data;
		inf->ptr = (char *)p;
	} else {
		inf->flag = STRINF_FLAG_USEEXT;
		inf->size = size;
		st2 = (STRBUF *)MALLOC( size + sizeof(STRINF) );
		p = (int *)(st2->data);
		inf->extptr = (void *)st2;
		inf->ptr = (char *)p;
		st2->inf = *inf;
	}
	*p = 0;
	//return inf->ptr;
	return (char *)p;
}

static void BlockFree( STRINF *inf )
{
	switch( inf->flag ) {
	case STRINF_FLAG_NONE:
		return;
	case STRINF_FLAG_USEINT:
		break;
	case STRINF_FLAG_USEEXT:
		FREE( inf->extptr );
		break;
	}
	inf->flag = STRINF_FLAG_NONE;
}

static char *BlockRealloc( STRBUF *st, int size )
{
	char *p;
	STRINF *inf;
	STRBUF *newst;
	inf = GETINF( st->inf.myblock );
	if ( size <= inf->size ) return inf->ptr;

	newst = (STRBUF *)MALLOC( size + sizeof(STRINF) );
	p = newst->data;
	memcpy( p, inf->ptr, inf->size );
	BlockFree( inf );
	inf->size = size;
	inf->flag = STRINF_FLAG_USEEXT;
	inf->ptr = p;
	inf->extptr = (void *)newst;

	newst->inf = *inf;
	return p;
}

void BlockInfo( STRINF *inf )
{
	STRBUF *newst;
	if ( inf->flag == STRINF_FLAG_USEEXT ) {
		newst = (STRBUF *)inf->extptr;
	}
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void sbInit( void )
{
	str_cur = 0;
	str_max = 0;
	str_blockcur = 0;
	BlockPtrPrepare();
}


void sbBye( void )
{
	int i;
	//for( i=0; i<str_max; i++ ) {
	//	BlockInfo( GETINF(i) );
	//}
	for( i=0; i<str_max; i++ ) {
		BlockFree( GETINF(i) );
	}
	for( i=0; i<str_blockcur; i++ ) {
		FREE( mem_sb[i] );
	}
	FREE( mem_ptr );
	FREE( mem_sb );
}


STRINF *sbGetSTRINF( char *ptr )
{
	return (STRINF *)( ptr - sizeof(STRINF) );
}


char *sbAlloc( int size )
{
	int sz;
	sz = size; if ( size < STRBUF_BLOCKSIZE ) sz = STRBUF_BLOCKSIZE;
	return BlockAlloc( sz );
}


char *sbAllocClear( int size )
{
	char *p;
	p = sbAlloc( size );
	memset( p, 0, size );
	return p;
}


void sbFree( void *ptr )
{
	char *p;
	STRBUF *st;
	STRINF *inf;
	int i;
	p = (char *)ptr;
	st = (STRBUF *)( p - sizeof(STRINF) );
	i = st->inf.myblock;
	inf = GETINF( i );
	if ( p != (inf->ptr) ) return;
	BlockFree( inf );
}


char *sbExpand( char *ptr, int size )
{
	STRBUF *st;
	st = (STRBUF *)( ptr - sizeof(STRINF) );
	return BlockRealloc( st, size );
}


void sbCopy( char **pptr, char *data, int size )
{
	int sz;
	char *ptr;
	char *p;
	STRBUF *st;
	ptr = *pptr;
	st = (STRBUF *)( ptr - sizeof(STRINF) );
	sz = st->inf.size;
	p = st->inf.ptr;
	if ( size > sz ) { p = BlockRealloc( st, size ); *pptr = p; }
	memcpy( p, data, size );
}


void sbAdd( char **pptr, char *data, int size, int mode )
{
	//		mode:0=normal/1=string
	int sz,newsize;
	STRBUF *st;
	char *ptr;
	char *p;
	ptr = *pptr;
	st = (STRBUF *)( ptr - sizeof(STRINF) );
	p = st->inf.ptr;
	if ( mode ) {
		sz = (int)strlen( p );					// 文字列データ
	} else {
		sz = st->inf.size;						// 通常データ
	}
	newsize = sz + size;
	if ( newsize > (st->inf.size) ) {
		newsize = ( newsize + 0xfff ) & 0xfffff000;						// 8K単位で確保
		//Alertf( "#Alloc%d",newsize );
		p = BlockRealloc( st, newsize );
		*pptr = p;
	}
	memcpy( p+sz, data, size );
}


void sbStrCopy( char **ptr, char *str )
{
	sbCopy( ptr, str, (int)strlen(str)+1 );
}


void sbStrAdd( char **ptr, char *str )
{
	sbAdd( ptr, str, (int)strlen(str)+1, 1 );
}


void *sbGetOption( char *ptr )
{
	STRBUF *st;
	st = (STRBUF *)( ptr - sizeof(STRINF) );
	return st->inf.opt;
}


void sbSetOption( char *ptr, void *option )
{
	STRBUF *st;
	STRINF *inf;
	st = (STRBUF *)( ptr - sizeof(STRINF) );
	st->inf.opt = option;
	inf = GETINF( st->inf.myblock );
	inf->opt = option;
}

/*
void sbInfo( char *ptr )
{
	STRBUF *st;
	st = (STRBUF *)( ptr - sizeof(STRINF) );
	Alertf( "size:%d (%x)",st->inf.size, st->inf.ptr );
}
*/

