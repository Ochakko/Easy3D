#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <SndFrame.h>
#include <SndKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CSndFrame::CSndFrame()
{
	InitParams();
}
CSndFrame::~CSndFrame()
{
	DestroyObjs();
}
int CSndFrame::InitParams()
{
//	int frameno;
//	int keynum;
//	CSndKey** m_ppkey;

	m_frameno = 0;
	m_keynum = 0;
	m_ppkey = 0;
	m_keyflag = 0;
	return 0;
}
int CSndFrame::DestroyObjs()
{
	if( m_ppkey ){
		free( m_ppkey );
		m_ppkey = 0;
	}
	m_keynum = 0;

	return 0;
}

int CSndFrame::AddSndKey( CSndKey* psndkey )
{
	if( !psndkey ){
		DbgOut( "sndframe : AddSndKey : sndkey NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int newnum;
	newnum = m_keynum + 1;

	m_ppkey = (CSndKey**)realloc( m_ppkey, sizeof( CSndKey* ) * newnum );
	if( !m_ppkey ){
		DbgOut( "sndframe : AddSndKey : ppkey alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( m_ppkey + m_keynum ) = psndkey;
	m_keynum = newnum;

	return 0;
}
