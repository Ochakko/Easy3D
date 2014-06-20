#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <GPKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CGPKey::CGPKey()
{
	InitParams();
}

CGPKey::~CGPKey()
{
	DestroyObjs();
}

int CGPKey::InitParams()
{
	m_frameno = 0;

	ZeroMemory( &m_gpe, sizeof( GPELEM ) );
	m_gpe.ongmode = GROUND_NONE;
	m_gpe.pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_gpe.rot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_gpe.rayy = 5000.0f;
	m_gpe.rayleng = 8000.0f;
	m_gpe.offsety = 10.0f;

	m_interp = 0;
	m_keyflag = 0;

	prev = 0;
	next = 0;

	return 0;
}

int CGPKey::DestroyObjs()
{
	return 0;
}

int CGPKey::SetGPKey( GPELEM* srcgpelem )
{
	m_gpe = *srcgpelem;
	return 0;
}
int CGPKey::SetInterp( int srcinterp )
{
	m_interp = srcinterp;
	return 0;
}

int CGPKey::FramenoCmp( CGPKey* cmpgpk )
{
	int cmpno;
	_ASSERT( cmpgpk );
	cmpno = cmpgpk->m_frameno;

	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
int CGPKey::FramenoCmp( int cmpno )
{
	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CGPKey::AddToPrev( CGPKey* addgpk )
{
	CGPKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addgpk->prev = prev;
	addgpk->next = this;

	prev = addgpk;

	if( saveprev ){
		saveprev->next = addgpk;
	}

	return 0;
}

int CGPKey::AddToNext( CGPKey* addgpk )
{
	CGPKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addgpk->prev = this;
	addgpk->next = savenext;

	next = addgpk;

	if( savenext ){
		savenext->prev = addgpk;
	}

	return 0;
}

int CGPKey::LeaveFromChain()
{
	CGPKey *saveprev, *savenext;
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

int CGPKey::CopyGPElem( CGPKey* srcgpk )
{
	DestroyObjs();

	int ret;
	ret = SetGPKey( &srcgpk->m_gpe );
	if( ret ){
		DbgOut( "GPKey : CopyGPElem : SetGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = SetInterp( srcgpk->m_interp );
	if( ret ){
		DbgOut( "GPKey : CopyGPElem : SetInterp error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
