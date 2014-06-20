#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#include <LWOvmad.h>


CLWOvmad::CLWOvmad()
{
	InitParams();
}
CLWOvmad::~CLWOvmad()
{
	DestroyObjs();
}


int CLWOvmad::SetParams( char* srcname, int srcindex, char* srctype, int srcdimension, int srcdatanum )
{

	DestroyObjs();

	int namelen;
	namelen = (int)strlen( srcname );
	nameptr = (char*)realloc( nameptr, namelen + 1 );
	if( !nameptr ){
		DbgOut( "LWOvmad : SetParams : nameptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( nameptr, srcname );

	index = srcindex;

	int typelen;
	typelen = (int)strlen( srctype );
	if( typelen != 4 ){
		DbgOut( "LWOvmad : SetParams : srctype length error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( typestr, srctype );

	dimension = srcdimension;

/////
	datanum = srcdatanum;
	data = new CLWOvmadelem [ datanum ];
	if( !data ){
		DbgOut( "LWOvmad : SetParams : data alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	int datano;
	for( datano = 0; datano < datanum; datano++ ){
		ret = (data + datano)->CreateData( dimension );
		if( ret ){
			DbgOut( "LWOvmad : SetParam : data->CreateData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}
int CLWOvmad::SetData( int srcdatano, int srcvertindex, int srcpolyindex, float* srcvalueptr, int valuenum )
{
	if( (srcdatano < 0) || (srcdatano >= datanum) ){
		DbgOut( "LWOvmad : SetData : srcdatano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CLWOvmadelem* dstelem;
	dstelem = data + srcdatano;
	_ASSERT( dstelem );

	int ret;
	ret = dstelem->SetParams( srcvertindex, srcpolyindex, srcvalueptr, valuenum );
	if( ret ){
		DbgOut( "LWOvmad : SetData : elem SetParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CLWOvmad::InitParams()
{
	nameptr = 0;
	index = 0;

	ZeroMemory( typestr, 5 );
	dimension = 0;

	datanum = 0;
	data = 0;

	//next = 0;

	return 0;
}

int CLWOvmad::DestroyObjs()
{
	if( nameptr ){
		free( nameptr );
		nameptr = 0;
	}

	if( data ){
		delete [] data;
		data = 0;
	}

	return 0;

}