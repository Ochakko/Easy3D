#include <stdafx.h> //É_É~Å[
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <math.h>

#include <MMotElem.h>

#include <shdelem.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CMMotElem::CMMotElem( CShdElem* srcbaseelem )
{
	InitParams();
	m_baseelem = srcbaseelem;
}

CMMotElem::~CMMotElem()
{
	DestroyObjs();
}

int CMMotElem::InitParams()
{
	m_baseelem = 0;
	m_primnum = 0;
	m_primarray = 0;

	return 0;
}

int CMMotElem::DestroyObjs()
{
	if( m_primarray ){
		free( m_primarray );
		m_primarray = 0;
	}
	m_primnum = 0;

	return 0;
}

MPRIM* CMMotElem::GetPrim( CShdElem* findelem )
{
	MPRIM* findprim = 0;

	int pno;
	for( pno = 0; pno < m_primnum; pno++ ){
		MPRIM* curprim;
		curprim = m_primarray + pno;
		if( curprim->selem == findelem ){
			findprim = curprim;
			break;
		}
	}
	return findprim;
}


int CMMotElem::FindPrim( CShdElem* findelem )
{
	int findindex = -1;

	int pno;
	for( pno = 0; pno < m_primnum; pno++ ){
		MPRIM* curprim;
		curprim = m_primarray + pno;
		if( curprim->selem == findelem ){
			findindex = pno;
			break;
		}
	}
	return findindex;
}

int CMMotElem::AddPrim( CShdElem* addelem )
{
	int findindex;
	findindex = FindPrim( addelem );
	if( findindex >= 0 ){
		return 0;
	}

	m_primarray = (MPRIM*)realloc( m_primarray, sizeof( MPRIM ) * ( m_primnum + 1 ) );
	if( !m_primarray ){
		DbgOut( "mmotelem : AddPrim : primarray alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	( m_primarray + m_primnum )->selem = addelem;
	( m_primarray + m_primnum )->value = 0.0f;
	m_primnum++;

	return 0;
}
int CMMotElem::DeletePrim( CShdElem* delelem )
{
	int findindex;
	findindex = FindPrim( delelem );
	if( findindex < 0 ){
		return 0;
	}

	MPRIM* newarray;
	newarray = (MPRIM*)malloc( sizeof( MPRIM ) * ( m_primnum - 1 ) );
	if( !newarray ){
		DbgOut( "mmotelem : DeletePrim : newarray alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newarray, sizeof( MPRIM ) * ( m_primnum - 1 ) );

	int pno;
	int setno = 0;
	for( pno = 0; pno < m_primnum; pno++ ){
		if( pno != findindex ){
			*( newarray + setno ) = *( m_primarray + pno );
			setno++;
		}
	}
	if( setno != (m_primnum - 1) ){
		DbgOut( "mmotelem : DeletePrim : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	free( m_primarray );
	m_primarray = newarray;
	m_primnum--;

	return 0;
}
float CMMotElem::GetValue( CShdElem* selem )
{
	float retval = 0.0f;
	int findindex;
	findindex = FindPrim( selem );
	if( findindex >= 0 ){
		retval = ( m_primarray + findindex )->value;
	}
	return retval;
}
	
int CMMotElem::SetValue( CShdElem* selem, float srcvalue )
{
	int ret;
	int findindex;
	findindex = FindPrim( selem );
	if( findindex >= 0 ){
		( m_primarray + findindex )->value = srcvalue;
	}else{
		ret = AddPrim( selem );
		if( ret ){
			DbgOut( "mme : SetValue : AddPrim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int findindex2;
		findindex2 = FindPrim( selem );
		if( findindex2 < 0 ){
			DbgOut( "mme : SetValue : findindex2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		( m_primarray + findindex2 )->value = srcvalue;
	}

	return 0;
}

int CMMotElem::CopyTarget( CMMotElem* srcmme )
{
	DestroyObjs();

	int ret;
	int srcpno;
	for( srcpno = 0; srcpno < srcmme->m_primnum; srcpno++ ){
		MPRIM* srcmp;
		srcmp = srcmme->m_primarray + srcpno;

		ret = SetValue( srcmp->selem, srcmp->value );
		if( ret ){
			DbgOut( "mme : CopyTarget : SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
