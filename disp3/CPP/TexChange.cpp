#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <TexChange.h>
#include <MQOMaterial.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CTexChange::CTexChange()
{
	InitParams();
}

CTexChange::~CTexChange()
{
	DestroyObjs();
}

int CTexChange::InitParams()
{
	m_mqomat = 0;
	ZeroMemory( m_change, sizeof( char ) * 256 );

	return 0;
}

int CTexChange::DestroyObjs()
{
	return 0;
}

int CTexChange::SetChangeName( char* srcname )
{

	if( srcname ){
		int leng;
		leng = (int)strlen( srcname );
		if( leng >= 256 ){
			DbgOut( "texchange : SetChangeName : name too long error !!!\n" );
			_ASSERT( 0 );
			ZeroMemory( m_change, sizeof( char ) * 256 );
			return 1;
		}

		strcpy_s( m_change, 256, srcname );
	}else{
		ZeroMemory( m_change, sizeof( char ) * 256 );
	}
	return 0;
}


