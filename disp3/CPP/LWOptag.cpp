#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <LWOptag.h>

CLWOptag::CLWOptag()
{
	InitParams();
}
CLWOptag::~CLWOptag()
{
	DestroyObjs();
}

int CLWOptag::InitParams()
{
	typestr[0] = 0;
	datanum = 0;
	dataptr = 0;

	return 0;
}
int CLWOptag::DestroyObjs()
{
	if( dataptr ){
		free( dataptr );
		dataptr = 0;
	}
	datanum = 0;

	return 0;
}


int CLWOptag::CreateData( char* srctype, int srcdatanum )
{
	DestroyObjs();

	int typelen;
	typelen = (int)strlen( srctype );
	if( typelen != 4 ){
		DbgOut( "LWOptag : CreateData : typelen not eq 4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( typestr, srctype );
	

	dataptr = (LWOPTAG*)malloc( sizeof( LWOPTAG ) * srcdatanum );
	if( !dataptr ){
		DbgOut( "LWOptag : CreateData : dataptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( dataptr, sizeof( LWOPTAG ) * srcdatanum );

	datanum = srcdatanum;

	return 0;
}
int CLWOptag::SetPtag( int srcdatano, int srcpolyindex, int srctag )
{
	if( (srcdatano < 0) || (srcdatano >= datanum) ){
		DbgOut( "LWOptag : SetPtag : srcdatano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LWOPTAG* dstdata;
	dstdata = dataptr + srcdatano;
	_ASSERT( dstdata );

	dstdata->polyindex = srcpolyindex;
	dstdata->tag = srctag;

	return 0;
}



