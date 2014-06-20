#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#include <LWOvmadelem.h>


CLWOvmadelem::CLWOvmadelem()
{
	InitParams();
}

CLWOvmadelem::~CLWOvmadelem()
{
	DestroyData();

}
int CLWOvmadelem::DestroyData()
{
	if( valueptr ){ 
		free( valueptr );
		valueptr = 0;
	}
	InitParams();

	return 0;
}
int CLWOvmadelem::InitParams()
{
	dimension = 0;
	vertindex = 0;
	polyindex = 0;
	valueptr = 0;

	return 0;
}


int CLWOvmadelem::CreateData( int srcdimension )
{
	DestroyData();

	dimension = srcdimension;

	if( dimension > 0 ){
		valueptr = (float*)malloc( sizeof( float ) * dimension );
		if( !valueptr ){ 
			DbgOut( "LWOvmadelem : valueptr alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ZeroMemory( valueptr, sizeof( float ) * dimension );
	}

	return 0;

}
int CLWOvmadelem::SetParams( int srcvertindex, int srcpolyindex, float* srcvalueptr, int valuenum )
{
	if( valuenum > dimension ){
		DbgOut( "LWOvmadelem : SetParams : valuenum overflow error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	vertindex = srcvertindex;
	polyindex = srcpolyindex;
	int valueno;
	for( valueno = 0; valueno < valuenum; valueno++ ){
		*valueptr = *srcvalueptr;
		valueptr++;
		srcvalueptr++;
	}

	return 0;
}

