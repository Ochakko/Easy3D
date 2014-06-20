#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <LWOpols.h>

CLWOpols::CLWOpols()
{
	InitParams();
}
CLWOpols::~CLWOpols()
{
	DestroyObjs();
}


int CLWOpols::InitParams()
{
	typestr[0] = 0;
	datanum = 0;
	data = 0;

	return 0;
}

int CLWOpols::DestroyObjs()
{
	if( data ){
		delete [] data;
		data = 0;
	}
	datanum = 0;

	return 0;
}

int CLWOpols::CreateData( int srcdatanum, char* srctype )
{

	DestroyObjs();

	int typelen;
	typelen = (int)strlen( srctype );
	if( typelen != 4 ){
		DbgOut( "LWOpols : CreateData : srctype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( typestr, srctype );

	data = new CLWOpolselem[ srcdatanum ];
	if( !data ){
		DbgOut( "LWOpols : CreateData : data alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	datanum = srcdatanum;


	return 0;
}
int CLWOpols::SetPols( int srcdatano, unsigned int* srcdata, int srcdatalen )
{
	if( srcdatano >= datanum ){
		DbgOut( "LWOpols : SetPols : srcdatano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CLWOpolselem* dstelem;
	dstelem = data + srcdatano;
	_ASSERT( dstelem );

	int ret;
	ret = dstelem->CreateData( srcdatalen );
	if( ret ){
		DbgOut( "LWOpols : SetPols : elem CreateData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = dstelem->SetVert( srcdata, srcdatalen );
	if( ret ){
		DbgOut( "LWOpols : SetPols : elem SetVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}