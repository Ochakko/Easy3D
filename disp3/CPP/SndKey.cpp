#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <SndKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CSndKey::CSndKey()
{
	InitParams();
}
CSndKey::~CSndKey()
{
	DestroyObjs();
}

int CSndKey::InitParams()
{
//int m_frameno;
//int m_sndenum;
//SNDELEM* m_psnde;
//int m_doneflag;
//CSndKey* prev;
//CSndKey* next;

	m_frameno = 0;
	m_sndenum = 0;
	m_psnde = 0;
	m_doneflag = 0;
	prev = 0;
	next = 0;

	return 0;
}
int CSndKey::DestroyObjs()
{
	if( m_psnde ){
		free( m_psnde );
		m_psnde = 0;
	}
	m_sndenum = 0;

	return 0;
}

int CSndKey::AddSndE( SNDELEM* srcsnde )
{
	int newnum = m_sndenum + 1;
	m_psnde = (SNDELEM*)realloc( m_psnde, sizeof( SNDELEM ) * newnum );
	if( !m_psnde ){
		DbgOut( "sndkey : AddSndE : psnde alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( m_psnde + newnum - 1 ) = *srcsnde;
	m_sndenum = newnum;

	return 0;
}
int CSndKey::DeleteSndE( int srcindex )
{
	if( m_sndenum <= 0 ){
		return 0;
	}

	int ret;
	if( srcindex < 0 ){
		ret = DestroyObjs();
		_ASSERT( !ret );

	}else{
		if( srcindex >= m_sndenum ){
			DbgOut( "sndkey : DeleteSNDE : srcindex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int newnum = m_sndenum - 1;
		SNDELEM* newsnde = (SNDELEM*)malloc( sizeof( SNDELEM ) * newnum );
		if( !newsnde ){
			DbgOut( "sndkey : DeleteSNDE : newsnde alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( newsnde, sizeof( SNDELEM ) * newnum );

		int setno = 0;
		int eno;
		for( eno = 0; eno < m_sndenum; eno++ ){
			if( eno != srcindex ){
				*( newsnde + setno ) = *( m_psnde + eno );
				setno++;
			}
		}
		if( setno != newnum ){
			DbgOut( "sndkey : DeleteSNDE : setno error !!!\n" );
			_ASSERT( 0 );
			free( newsnde );
			return 1;
		}

		free( m_psnde );
		m_psnde = newsnde;
		m_sndenum = newnum;
	}


	return 0;
}
int CSndKey::GetIndex( SNDELEM* srcsnde, int* indexptr )
{
	int findindex = -1;

	int eno;
	for( eno = 0; eno < m_sndenum; eno++ ){
		SNDELEM* chksnde = m_psnde + eno;
		if( IsSameSndE( chksnde, srcsnde ) ){
			findindex = eno;
			break;
		}
	}

	*indexptr = findindex;

	return 0;
}
int CSndKey::FramenoCmp( CSndKey* cmpsndk )
{
	int cmpno;
	_ASSERT( cmpsndk );
	cmpno = cmpsndk->m_frameno;

	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
	return 0;
}
int CSndKey::FramenoCmp( int cmpno )
{
	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
	return 0;
}
int CSndKey::AddToPrev( CSndKey* addsndk )
{
	CSndKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addsndk->prev = prev;
	addsndk->next = this;

	prev = addsndk;

	if( saveprev ){
		saveprev->next = addsndk;
	}
	return 0;
}
int CSndKey::AddToNext( CSndKey* addsndk )
{
	CSndKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addsndk->prev = this;
	addsndk->next = savenext;

	next = addsndk;

	if( savenext ){
		savenext->prev = addsndk;
	}
	return 0;
}
int CSndKey::LeaveFromChain()
{
	CSndKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	prev = 0;
	next = 0;

	if( saveprev ){
		saveprev->next = savenext;
	}

	if( savenext ){
		savenext->prev = saveprev;
	}
	return 0;
}
int CSndKey::CopySndElem( CSndKey* srcsndk )
{
	DestroyObjs();

	//frameno はコピーしない！！！

	m_doneflag = 0;//!!!!!!!!!

	m_sndenum = srcsndk->m_sndenum;
	m_psnde = (SNDELEM*)malloc( sizeof( SNDELEM ) * m_sndenum );
	if( !m_psnde ){
		DbgOut( "sndkey : CopySNDElem : psnde alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( m_psnde, srcsndk->m_psnde, sizeof( SNDELEM ) * m_sndenum );

	return 0;
}

int CSndKey::IsSameSndE( SNDELEM* snde1, SNDELEM* snde2 )
{
//	if( snde1->sndsetid != snde2->sndsetid ){
//		return 0;
//	}
	int cmp0;
	cmp0 = strcmp( snde1->sndsetname, snde2->sndsetname );
	if( cmp0 != 0 ){
		return 0;
	}

	int cmp1;
	cmp1 = strcmp( snde1->sndname, snde2->sndname );
	if( cmp1 != 0 ){
		return 0;
	}

	return 1;
}

