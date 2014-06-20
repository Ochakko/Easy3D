#ifndef LWOVMAPH
#define LWOVMAPH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <LWOvmapelem.h>

class CLWOvmap
{
public:
	CLWOvmap();
	~CLWOvmap();

	float operator() ( int srcvertindex, int srcdimno ){
		if( (srcdimno < 0) || (srcdimno > dimension) ){
			DbgOut( "LWOvmap : operator () : dimension range error !!!\n" );
			_ASSERT( 0 );
			return 0.0f;
		}
		
		int datano;
		for( datano = 0; datano < datanum; datano++ ){
			if( data->vertindex == srcvertindex ){
				return *(data->valueptr + srcdimno);
			}
			data++;
		}

		_ASSERT( 0 );
		return 0.0f;
	};

	int SetParams( char* srcname, int srcindex, char* srctype, int srcdimension, int srcdatanum );
	int SetData( int srcdatano, int srcvertindex, float* srcvalueptr, int valuenum );


private:
	int InitParams();
	int DestroyObjs();


public:
	char* nameptr;
	int index;

	char typestr[5];
	int dimension;

	int datanum;
	CLWOvmapelem* data;

	//CLWOvmap* next;

};



#endif