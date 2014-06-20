#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define	DBGH
#include "dbg.h"

#include <crtdbg.h>

#include <math.h>

#include <LWOtags.h>


CLWOtags::CLWOtags()
{
	InitParams();
}
CLWOtags::~CLWOtags()
{
	DestroyObjs();
}

int CLWOtags::InitParams()
{
	nameptr = 0;
	index = -1;
	//next = 0;

	return 0;
}

int CLWOtags::DestroyObjs()
{
	if( nameptr ){
		free( nameptr );
		nameptr = 0;
	}

	index = -1;

	return 0;
}

int CLWOtags::SetParams( char* srcname, int srcindex )
{
	if( !srcname || (srcindex < 0) ){
		DbgOut( "LWOtags : SetParams : params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int srcleng;
	srcleng = (int)strlen( srcname );

	nameptr = (char*)realloc( nameptr, srcleng + 1 );
	if( !nameptr ){
		DbgOut( "LWOtags : SetParams : nameptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( nameptr, srcname );

	index = srcindex;

	return 0;
}
