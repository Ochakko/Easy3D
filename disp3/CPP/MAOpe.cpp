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



CMAOpe::MAOpe( CMotHandler* srcmh )
{
	InitParams();

	m_mh = srcmh;
}
CMAOpe::~MAOpe()
{	
	DestroyObjs();
}

int CMAOpe::InitParams()
{
	m_mh = 0;
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

	m_mch = new CMCHandler( m_mh );
	if( m_mch ){
		DbgOut( "maope : LoadMAFile : mch alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMAFile mafile;
	ret = mafile.LoadMAFile( srcname, m_mch, srchwnd );
	if( ret ){
		DbgOut( "maope : LoadMAFile : mafile LoadMAFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//int SetNewPose( int srcevent, int* motidptr, int* framenoptr );
