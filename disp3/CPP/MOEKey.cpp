#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <MOEKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CMOEKey::CMOEKey()
{
	InitParams();
}

CMOEKey::~CMOEKey()
{
	DestroyObjs();
}

int CMOEKey::InitParams()
{
	m_frameno = 0;
	ZeroMemory( m_idlingname, sizeof( char ) * 256 );

	m_moeenum = 0;
	m_pmoee = 0;

	prev = 0;
	next = 0;

	m_goonflag = 0;

	return 0;
}

int CMOEKey::DestroyObjs()
{
	if( m_pmoee ){
		free( m_pmoee );
		m_pmoee = 0;
	}
	m_moeenum = 0;

	return 0;
}

int CMOEKey::SetIdlingName( char* srcname )
{
	if( !srcname ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_idlingname, sizeof( char ) * 256 );
	strcpy_s( m_idlingname, 256, srcname ); 

	return 0;
}


int CMOEKey::AddMOEE( MOEELEM* srcmoee )
{
	int newnum = m_moeenum + 1;
	m_pmoee = (MOEELEM*)realloc( m_pmoee, sizeof( MOEELEM ) * newnum );
	if( !m_pmoee ){
		DbgOut( "moekey : AddMOEE : pmoee alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( m_pmoee + newnum - 1 ) = *srcmoee;
	m_moeenum = newnum;

	return 0;
}

int CMOEKey::DeleteMOEE( int srcindex )
{
	if( m_moeenum <= 0 ){
		return 0;
	}

	int ret;
	if( srcindex < 0 ){
		ret = DestroyObjs();
		_ASSERT( !ret );

	}else{
		if( srcindex >= m_moeenum ){
			DbgOut( "moekey : DeleteMOEE : srcindex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int newnum = m_moeenum - 1;
		MOEELEM* newmoee = (MOEELEM*)malloc( sizeof( MOEELEM ) * newnum );
		if( !newmoee ){
			DbgOut( "moekey : DeleteMOEE : newmoee alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( newmoee, sizeof( MOEELEM ) * newnum );

		int setno = 0;
		int eno;
		for( eno = 0; eno < m_moeenum; eno++ ){
			if( eno != srcindex ){
				*( newmoee + setno ) = *( m_pmoee + eno );
				setno++;
			}
		}
		if( setno != newnum ){
			DbgOut( "moekey : DeleteMOEE : setno error !!!\n" );
			_ASSERT( 0 );
			free( newmoee );
			return 1;
		}

		free( m_pmoee );
		m_pmoee = newmoee;
		m_moeenum = newnum;
	}

	return 0;
}

int CMOEKey::GetIndex( MOEELEM* srcmoee, int* indexptr )
{
	int findindex = -1;

	int eno;
	for( eno = 0; eno < m_moeenum; eno++ ){
		MOEELEM* chkmoee = m_pmoee + eno;
		if( IsSameMOEE( chkmoee, srcmoee ) ){
			findindex = eno;
			break;
		}
	}

	*indexptr = findindex;

	return 0;
}

int CMOEKey::IsSameMOEE( MOEELEM* moee1, MOEELEM* moee2 )
{
	if( moee1->eventno != moee2->eventno )
		return 0;
	if( moee1->listnum != moee2->listnum )
		return 0;
	if( moee1->notlistnum != moee2->notlistnum )
		return 0;

	int lno;
	for( lno = 0; lno < moee1->listnum; lno++ ){
		if( moee1->list[lno] != moee2->list[lno] ){
			return 0;
		}
	}

	for( lno = 0; lno < moee1->notlistnum; lno++ ){
		if( moee1->notlist[lno] != moee2->notlist[lno] ){
			return 0;
		}
	}

	return 1;
}


int CMOEKey::FramenoCmp( CMOEKey* cmpmoek )
{
	int cmpno;
	_ASSERT( cmpmoek );
	cmpno = cmpmoek->m_frameno;

	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
int CMOEKey::FramenoCmp( int cmpno )
{
	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CMOEKey::AddToPrev( CMOEKey* addmoek )
{
	CMOEKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addmoek->prev = prev;
	addmoek->next = this;

	prev = addmoek;

	if( saveprev ){
		saveprev->next = addmoek;
	}

	return 0;
}

int CMOEKey::AddToNext( CMOEKey* addmoek )
{
	CMOEKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addmoek->prev = this;
	addmoek->next = savenext;

	next = addmoek;

	if( savenext ){
		savenext->prev = addmoek;
	}

	return 0;
}

int CMOEKey::LeaveFromChain()
{
	CMOEKey *saveprev, *savenext;
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

int CMOEKey::CopyMOEElem( CMOEKey* srcmoek )
{
	DestroyObjs();

	//frameno はコピーしない！！！

	ZeroMemory( m_idlingname, sizeof( char ) * 256 );
	strcpy_s( m_idlingname, 256, srcmoek->m_idlingname );
	m_moeenum = srcmoek->m_moeenum;
	m_pmoee = (MOEELEM*)malloc( sizeof( MOEELEM ) * m_moeenum );
	if( !m_pmoee ){
		DbgOut( "moekey : CopyMOEElem : pmoee alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( m_pmoee, srcmoek->m_pmoee, sizeof( MOEELEM ) * m_moeenum );

	return 0;
}
