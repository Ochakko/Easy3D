#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <EventKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CEventKey::CEventKey()
{
	InitParams();
}
CEventKey::~CEventKey()
{
	DestroyObjs();
}

int CEventKey::InitParams()
{
	ZeroMemory( m_ekey, sizeof( EKEY ) * 256 );
	m_keynum = 0;
	m_savemotid = 0;
	m_saveframeno = 0;

	return 0;
}
int CEventKey::DestroyObjs()
{
	return 0;
}
int CEventKey::AddEKey( EKEY srcek )
{
	if( m_keynum >= 255 ){
		DbgOut( "ek : AddEkey : keynum overflow error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	EKEY* dstek = m_ekey + m_keynum;
	*dstek = srcek;
	dstek->validflag = 1;

	m_keynum++;

	return 0;
}
int CEventKey::DelEKeyByIndex( int srcindex )
{
	if( srcindex < 0 ){
		ZeroMemory( m_ekey, sizeof( EKEY ) * 256 );
		m_keynum = 0;
	}else{
		m_ekey[srcindex].validflag = 0;

		EKEY saveek[256];
		ZeroMemory( saveek, sizeof( EKEY ) * 256 );

		int setno = 0;
		int kno;
		for( kno = 0; kno < m_keynum; kno++ ){
			if( kno != srcindex ){
				saveek[setno] = m_ekey[kno];
				setno++;
			}
		}
		if( setno != (m_keynum - 1) ){
			DbgOut( "ek : DelEKeyByIndex : setno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ZeroMemory( m_ekey, sizeof( EKEY ) * 256 );
		MoveMemory( m_ekey, saveek, sizeof( EKEY ) * (m_keynum - 1) );

		m_keynum--;
	}

	return 0;
}

int CEventKey::GetEventNo( int srckey, int srccnt )
{
	int findevent = 0;

	int kindex;
	for( kindex = 0; kindex < m_keynum; kindex++ ){
		if( (m_ekey[kindex].validflag == 1) && (m_ekey[kindex].key == srckey) ){
			if( m_ekey[kindex].singleevent == 0 ){
				if( srccnt != 0 ){
					findevent = m_ekey[kindex].eventno;
					break;
				}
			}else{
				if( srccnt == 1 ){
					findevent = m_ekey[kindex].eventno;
					break;
				}
			}
		}
	}
	return findevent;
}

int CEventKey::CheckSameKey( int srckey, int* indexptr )
{
	*indexptr = -1;
	int isfind = 0;

	int kindex;
	for( kindex = 0; kindex < m_keynum; kindex++ ){
		if( (m_ekey[kindex].validflag == 1) && (m_ekey[kindex].key == srckey) ){
			isfind = 1;
			*indexptr = kindex;
			break;
		}
	}
	return isfind;
}

