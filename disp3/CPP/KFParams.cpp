
#include "StdAfx.h"
#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <KFParams.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

//#include <shdhandler.h>
//#include <shdelem.h>

CKFParams::CKFParams()
{
	InitParams();
}

CKFParams::~CKFParams()
{
	DestroyObjs();
};

int CKFParams::InitParams()
{
//	int m_frameno;
//	int m_iparams[ KFPINUM ];
//	double m_dparams[ KFPDNUM ];

	m_frameno = -1;
	ZeroMemory( m_iparams, sizeof( int ) * KFPINUM );
	ZeroMemory( m_dparams, sizeof( double ) * KFPDNUM );

	return 0;
}
int CKFParams::DestroyObjs()
{

	return 0;
}


int CKFParams::SetIParam( int srcindex, int srcival )
{
	if( (srcindex < 0) || (srcindex >= KFPINUM) ){
		DbgOut( "kfp : SetIParam : srcindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_iparams[ srcindex ] = srcival;

	return 0;
}
int CKFParams::SetDParam( int srcindex, double srcdval )
{
	if( (srcindex < 0) || (srcindex >= KFPINUM) ){
		DbgOut( "kfp : SetDParam : srcindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_dparams[ srcindex ] = srcdval;

	return 0;
}

