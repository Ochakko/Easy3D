#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <CameraKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CCameraKey::CCameraKey()
{
	InitParams();
}

CCameraKey::~CCameraKey()
{
	DestroyObjs();
}

int CCameraKey::InitParams()
{
	m_frameno = 0;
	ZeroMemory( &m_celem, sizeof( CAMERAELEM ) );
	m_interp = 0;

	prev = 0;
	next = 0;

	return 0;
}

int CCameraKey::DestroyObjs()
{
	return 0;
}

int CCameraKey::SetCameraKey( CAMERAELEM* srccelem )
{
	m_celem = *srccelem;
	return 0;
}
int CCameraKey::SetInterp( int srcinterp )
{
	m_interp = srcinterp;
	return 0;
}

int CCameraKey::FramenoCmp( CCameraKey* cmpck )
{
	int cmpno;
	_ASSERT( cmpck );
	cmpno = cmpck->m_frameno;

	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
int CCameraKey::FramenoCmp( int cmpno )
{
	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CCameraKey::AddToPrev( CCameraKey* addck )
{
	CCameraKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addck->prev = prev;
	addck->next = this;

	prev = addck;

	if( saveprev ){
		saveprev->next = addck;
	}

	return 0;
}

int CCameraKey::AddToNext( CCameraKey* addck )
{
	CCameraKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addck->prev = this;
	addck->next = savenext;

	next = addck;

	if( savenext ){
		savenext->prev = addck;
	}

	return 0;
}

int CCameraKey::LeaveFromChain()
{
	CCameraKey *saveprev, *savenext;
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

int CCameraKey::CopyCameraElem( CCameraKey* srcck )
{
	DestroyObjs();

	int ret;
	ret = SetCameraKey( &srcck->m_celem );
	if( ret ){
		DbgOut( "CameraKey : CopyCameraElem : SetCameraKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = SetInterp( srcck->m_interp );
	if( ret ){
		DbgOut( "CameraKey : CopyCameraElem : SetInterp error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
