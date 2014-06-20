#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <namebank.h>

#include <coef.h>

#define	DBGH
#include <dbg.h>

static int s_addno = 0;

CNameBank::CNameBank()
{
	InitParams();
}

CNameBank::~CNameBank()
{
	DestroyObjs();
}
void CNameBank::InitParams()
{
	hname = 0;
	namenum = 0;
	pidata = 0;
	pidata2 = 0;
	pidata3 = 0;
	pidata4 = 0;
	porgsize = 0;
	pfmt = 0;
	Type = 0;
	pserial = 0;
	dirtyflag = 0;
}
void CNameBank::DestroyObjs()
{
	int i;
	//char* nameptr; 

	if( hname ){
		for( i = 0; i < namenum; i++ ){
			free( *(hname + i) );
			/***
			nameptr = *(hname + i);
			if( nameptr ){
				free( nameptr );
				DbgOut( "CNameBank : DestroyObjs : free nameptr %d\n", i );
			}
			***/
		}

		free( hname );
		hname = 0;
		DbgOut( "CNameBank : DestroyObjs : free hname\n" );
	}

	if( pidata ){
		free( pidata );
		pidata = 0;
	}

	if( pidata2 ){
		free( pidata2 );
		pidata2 = 0;
	}
	if( pidata3 ){
		free( pidata3 );
		pidata3 = 0;
	}
	if( pidata4 ){
		free( pidata4 );
		pidata4 = 0;
	}
	if( porgsize ){
		free( porgsize );
		porgsize = 0;
	}
	if( pfmt ){
		free( pfmt );
		pfmt = 0;
	}
	if( Type ){
		free( Type );
		Type = 0;
	}

	if( pserial ){
		free( pserial );
		pserial = 0;
	}
	if( dirtyflag ){
		free( dirtyflag );
		dirtyflag = 0;
	}

}

int CNameBank::AddName( char* srcname, int srcidata, int srcidata2, int lookintdata )
{
	int isfound;
	char* newname = 0;
	int leng;

	if( !srcname ){
		DbgOut( "namebank : AddName : srcname NULL error !!!" );
		_ASSERT( 0 );
		return -1;
	}

	if( lookintdata == 0 ){
		isfound = FindName( srcname );
	}else{
		isfound = FindName( srcname, srcidata );
	}
	if( isfound >= 0 ){
		return 0;
	}

	leng = (int)strlen( srcname );
	newname = (char*)malloc( leng + 1 );
	if( !newname ){
		DbgOut( "namebank : AddName : newname alloc error !!! %d", leng + 1 );
		_ASSERT( 0 );
		return -1;
	}

	ZeroMemory( newname, leng + 1 );
	strncpy_s( newname, leng + 1, srcname, leng );

	namenum++;

	hname = (char**)realloc( hname, sizeof( char* ) * namenum );
	if( !hname ){
		DbgOut( "namebank : AddName : hname alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(hname + namenum - 1) = newname;

	pidata = (int*)realloc( pidata, sizeof( int ) * namenum );
	if( !pidata ){
		DbgOut( "namebank : AddName : pidata alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(pidata + namenum - 1) = srcidata;

	pidata2 = (int*)realloc( pidata2, sizeof( int ) * namenum );
	if( !pidata2 ){
		DbgOut( "namebank : AddName : pidata2 alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(pidata2 + namenum - 1) = srcidata2;

	pidata3 = (int*)realloc( pidata3, sizeof( int ) * namenum );
	if( !pidata3 ){
		DbgOut( "namebank : AddName : pidata3 alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(pidata3 + namenum - 1) = 0;

	pidata4 = (int*)realloc( pidata4, sizeof( int ) * namenum );
	if( !pidata4 ){
		DbgOut( "namebank : AddName : pidata4 alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(pidata4 + namenum - 1) = 0;

	porgsize = (SIZE*)realloc( porgsize, sizeof( SIZE ) * namenum );
	if( !porgsize ){
		DbgOut( "namebank : AddName : porgsize alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	(porgsize + namenum - 1)->cx = 0;
	(porgsize + namenum - 1)->cy = 0;


	pfmt = (int*)realloc( pfmt, sizeof( int ) * namenum );
	if( !pfmt ){
		DbgOut( "namebank : AddName : pfmt alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(pfmt + namenum - 1) = D3DFMT_A8R8G8B8;


	Type = (int*)realloc( Type, sizeof( int ) * namenum );
	if( !Type ){
		DbgOut( "namebank : AddName : Type alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(Type + namenum - 1) = TEXTYPE_NONE;

	s_addno++;//!!!!!!!!!!!
	pserial = (int*)realloc( pserial, sizeof( int ) * namenum );
	if( !pserial ){
		DbgOut( "namebank : AddName : pserial alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(pserial + namenum - 1) = s_addno;


	dirtyflag = (int*)realloc( dirtyflag, sizeof( int ) * namenum );
	if( !dirtyflag ){
		DbgOut( "namebank : AddName : dirtyflag alloc error !!! %d", namenum );
		_ASSERT( 0 );
		return -1;
	}
	*(dirtyflag + namenum - 1) = 0;


	return s_addno;
}
int CNameBank::FindName( char* srcname )
{
	if( !srcname )
		return -1;


	int i, cmp;
	char* nameptr;

	for( i = 0; i < namenum; i++ ){
		nameptr = (*this)( i );
		cmp = strcmp( srcname, nameptr );
		if( cmp == 0 )
			return i;
	}
	return -1;
}

int CNameBank::FindName( char* srcname, int srcint )
{
	if( !srcname )
		return -1;

	int i, cmp;
	char* nameptr;

	for( i = 0; i < namenum; i++ ){
		nameptr = (*this)( i );
		cmp = strcmp( srcname, nameptr );
		if( cmp == 0 ){
			int tempint;
			tempint = GetIntData( i );
			if( tempint == srcint )
				return i;
		}
	}
	return -1;
}

int CNameBank::GetNameBySerial( int srcseri, char** ppname, int* ptrans )
{
	
	int i;
	for( i = 0; i < namenum; i++ ){
		int tempseri;
		tempseri = GetSerial( i );
		if( tempseri == srcseri ){
			*ppname = *( hname + i );
			*ptrans = GetIntData( i );
			return 0;
		}
	}

	*ppname = 0;
	*ptrans = 0;
	return 0;//!!!!!!!!!!

}

int CNameBank::GetIndexBySerial( int srcseri )
{
	int i;
	int findindex = -1;

	for( i = 0; i < namenum; i++ ){
		if( *( pserial + i ) == srcseri ){
			findindex = i;
			break;
		}
	}

	return findindex;
}


int CNameBank::ResetDirtyFlag()
{
	int i;
	for( i = 0; i < namenum; i++ ){
		*( dirtyflag + i ) = 0;
	}

	return 0;
}

int CNameBank::SetDirtyFlag( char* srcname, int srcint )
{
	int findno;

	findno = FindName( srcname, srcint );
	if( findno >= 0 ){
		*( dirtyflag + findno ) = 1;
	}

	return 0;
}
int CNameBank::SetFMT( int srcindex, int srcfmt )
{
	if( (srcindex < 0) || (srcindex >= namenum) ){
		DbgOut( "namebank : SetFMT : srcindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( pfmt + srcindex ) = srcfmt;

	return 0;
}


int CNameBank::Rename( int srcindex, char* srcname )
{
	if( (srcindex < 0) || (srcindex >= namenum) ){
		DbgOut( "namebank : Rename : srcindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
	int leng;
	leng = (int)strlen( srcname );

	char* newname;
	newname = (char*)malloc( sizeof( char ) * leng + 1 );
	if( !newname ){
		DbgOut( "namebank : Rename : newname alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( newname, leng + 1, srcname );

	char* delname;
	delname = *( hname + srcindex );
	if( delname ){
		free( delname );
	}

	*( hname + srcindex ) = newname;

	return 0;
}
