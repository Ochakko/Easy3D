#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <MAOpe.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>

#include <crtdbg.h>

#include <MAFile.h>
#include <MCHandler.h>


#define DBGH
#include <dbg.h>

#include "handlerset.h"


CMAOpe::MAOpe( CHandlerSet* srchs )
{
	InitParams();

	m_hs = srchs;
}
CMAOpe::~MAOpe()
{	
	DestroyObjs();
}

int CMAOpe::InitParams()
{
	m_hs = 0;
	m_mch = 0;
	return 0;
}
int CMAOpe::DestroyObjs()
{

	if( m_mch ){
		delete m_mch;
		m_mch = 0;
	}

	return 0;
}


int CMAOpe::LoadMAFile( char* srcname, HWND srchwnd )
{
	int ret;

	DestroyObjs();

	m_mch = new CMCHandler( m_hs->m_mhandler );
	if( m_mch ){
		DbgOut( "maope : LoadMAFile : mch alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMAFile mafile;
	ret = mafile.LoadMAFile( srcname, srchwnd, this );
	if( ret ){
		DbgOut( "maope : LoadMAFile : mafile LoadMAFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//int SetNewPose( int srcevent, int* motidptr, int* framenoptr );
