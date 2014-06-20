#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <CameraSWKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CCameraSWKey::CCameraSWKey()
{
	InitParams();
}

CCameraSWKey::~CCameraSWKey()
{
	DestroyObjs();
}

int CCameraSWKey::InitParams()
{
	m_frameno = 0;
	m_camerano = 0;
	prev = 0;
	next = 0;

	return 0;
}

int CCameraSWKey::DestroyObjs()
{
	return 0;
}

int CCameraSWKey::SetCameraSWKey( int srccamno )
{
	m_camerano = srccamno;

	return 0;
}

int CCameraSWKey::FramenoCmp( CCameraSWKey* cmpcswk )
{
	int cmpno;
	_ASSERT( cmpcswk );
	cmpno = cmpcswk->m_frameno;

	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
int CCameraSWKey::FramenoCmp( int cmpno )
{
	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CCameraSWKey::AddToPrev( CCameraSWKey* addcswk )
{
	CCameraSWKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addcswk->prev = prev;
	addcswk->next = this;

	prev = addcswk;

	if( saveprev ){
		saveprev->next = addcswk;
	}

	return 0;
}

int CCameraSWKey::AddToNext( CCameraSWKey* addcswk )
{
	CCameraSWKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addcswk->prev = this;
	addcswk->next = savenext;

	next = addcswk;

	if( savenext ){
		savenext->prev = addcswk;
	}

	return 0;
}

int CCameraSWKey::LeaveFromChain()
{
	CCameraSWKey *saveprev, *savenext;
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

int CCameraSWKey::CopyCameraSW( CCameraSWKey* srccswk )
{
	DestroyObjs();

	int ret;
	ret = SetCameraSWKey( srccswk->m_camerano );
	if( ret ){
		DbgOut( "CameraSWKey : CopyCameraSW : SetCameraSW error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}
