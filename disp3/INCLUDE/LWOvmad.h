#ifndef LWOVMADH
#define LWOVMADH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <LWOvmadelem.h>

class CLWOvmad
{
public:
	CLWOvmad();
	~CLWOvmad();

	float operator() ( int srcvertindex, int srcpolyindex, int srcdimno ){
		if( (srcdimno < 0) || (srcdimno > dimension) ){
			DbgOut( "LWOvmap : operator () : dimension range error !!!\n" );
			_ASSERT( 0 );
			return 0.0f;
		}
		
		int datano;
		for( datano = 0; datano < datanum; datano++ ){
			if( (data->vertindex == srcvertindex) && (data->polyindex == srcpolyindex) ){
				return *(data->valueptr + srcdimno);
			}
			data++;
		}

		_ASSERT( 0 );
		return 0.0f;
	};

	int SetParams( char* srcname, int srcindex, char* srctype, int srcdimension, int srcdatanum );
	int SetData( int srcdatano, int srcvertindex, int srcpolyindex, float* srcvalueptr, int valuenum );


private:
	int InitParams();
	int DestroyObjs();


public:
	char* nameptr;
	int index;

	char typestr[5];
	int dimension;

	int datanum;
	CLWOvmadelem* data;

	//CLWOvmad* next;

};



#endif