#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <TexChangePt.h>
#include <MQOMaterial.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CTexChangePt::CTexChangePt()
{
	InitParams();
}

CTexChangePt::~CTexChangePt()
{
	DestroyObjs();
}

int CTexChangePt::InitParams()
{
	m_mqomat = 0;
	m_change = 0;

	return 0;
}

int CTexChangePt::DestroyObjs()
{
	return 0;
}

int CTexChangePt::SetChangeName( char* srcname )
{
	if( srcname ){
		int leng;
		leng = (int)strlen( srcname );
		if( leng >= 256 ){
			DbgOut( "texchange : SetChangeName : name too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_change = srcname;
	}else{
		m_change = 0;
	}
	return 0;
}


