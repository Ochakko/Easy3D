#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#include <LWOvmapelem.h>


CLWOvmapelem::CLWOvmapelem()
{
	InitParams();
}

CLWOvmapelem::~CLWOvmapelem()
{
	DestroyData();

}
int CLWOvmapelem::DestroyData()
{
	if( valueptr ){ 
		free( valueptr );
		valueptr = 0;
	}
	InitParams();

	return 0;
}
int CLWOvmapelem::InitParams()
{
	dimension = 0;
	vertindex = 0;
	valueptr = 0;

	return 0;
}


int CLWOvmapelem::CreateData( int srcdimension )
{
	DestroyData();

	dimension = srcdimension;

	if( dimension > 0 ){
		valueptr = (float*)malloc( sizeof( float ) * dimension );
		if( !valueptr ){ 
			DbgOut( "LWOvmapelem : valueptr alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ZeroMemory( valueptr, sizeof( float ) * dimension );
	}

	return 0;

}
int CLWOvmapelem::SetParams( int srcvertindex, float* srcvalueptr, int valuenum )
{
	if( valuenum > dimension ){
		DbgOut( "LWOvmapelem : SetParams : valuenum overflow error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	vertindex = srcvertindex;
	int valueno;
	for( valueno = 0; valueno < valuenum; valueno++ ){
		*valueptr = *srcvalueptr;
		valueptr++;
		srcvalueptr++;
	}

	return 0;
}

