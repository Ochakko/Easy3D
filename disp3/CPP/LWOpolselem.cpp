#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <LWOpolselem.h>

CLWOpolselem::CLWOpolselem()
{
	InitParams();
}
CLWOpolselem::~CLWOpolselem()
{
	DestroyObjs();
}

int CLWOpolselem::CreateData( int srcvertnum )
{
	DestroyObjs();

	vertptr = (unsigned int*)malloc( sizeof( unsigned int ) * srcvertnum );
	if( !vertptr ){
		DbgOut( "LWOpolselem : CreateData : vertptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( vertptr, sizeof( unsigned int ) * srcvertnum );

	vertnum = srcvertnum;


	return 0;
}
int CLWOpolselem::SetVert( unsigned int* srcvert, int srcvertnum )
{
	if( srcvertnum > vertnum ){
		DbgOut( "LWOpolselem : SetVert : srcvertnum range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( vertptr, srcvert, sizeof( unsigned int ) * srcvertnum );


	return 0;
}

int CLWOpolselem::InitParams()
{
	vertnum = 0;
	vertptr = 0;

	return 0;
}

int CLWOpolselem::DestroyObjs()
{
	if( vertptr ){
		free( vertptr );
		vertptr = 0;
	}
	vertnum = 0;

	return 0;
}

