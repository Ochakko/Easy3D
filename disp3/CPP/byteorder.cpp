#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#include <byteorder.h>


// N2HIで、int のサイズを4バイト、shortのサイズを2バイトと仮定するので、使う前に、チェックする。
int CheckDataSize()
{
	int isize;
	int ssize;
	int fsize;

	isize = (int)sizeof( int );
	ssize = (int)sizeof( short );
	fsize = (int)sizeof( float );

	if( (isize == 4) && (ssize == 2) && (fsize == 4) ){
		return 1;
	}else{
		return 0;
	}
}


unsigned short N2HUS( unsigned short orgshort )
{
	unsigned short retshort = 0;
	unsigned char* srcptr;
	unsigned char* dstptr;

	srcptr = (unsigned char*)( &orgshort );
	dstptr = (unsigned char*)( &retshort ) + 1;

	int i;
	for( i = 0; i < 2; i++ ){

		*dstptr = *srcptr;

		if( i == 0 ){
			srcptr++;
			dstptr--;
		}
	}

	return retshort;

}

unsigned int N2HUI( unsigned int orgint )
{
	unsigned int retint = 0;
	unsigned char* srcptr;
	unsigned char* dstptr;

	srcptr = (unsigned char*)( &orgint );
	dstptr = (unsigned char*)( &retint ) + 3;

	int i;
	for( i = 0; i < 4; i++ ){

		*dstptr = *srcptr;

		if( i <= 2 ){
			srcptr++;
			dstptr--;
		}
	}

	return retint;
}


short N2HS( short orgshort )
{
	short retshort = 0;
	unsigned char* srcptr;
	unsigned char* dstptr;

	srcptr = (unsigned char*)( &orgshort );
	dstptr = (unsigned char*)( &retshort ) + 1;

	int i;
	for( i = 0; i < 2; i++ ){

		*dstptr = *srcptr;

		if( i == 0 ){
			srcptr++;
			dstptr--;
		}
	}

	return retshort;

}

int N2HI( int orgint )
{
	int retint = 0;
	unsigned char* srcptr;
	unsigned char* dstptr;

	srcptr = (unsigned char*)( &orgint );
	dstptr = (unsigned char*)( &retint ) + 3;

	int i;
	for( i = 0; i < 4; i++ ){

		*dstptr = *srcptr;

		if( i <= 2 ){
			srcptr++;
			dstptr--;
		}
	}

	return retint;
}


float N2HF( float orgfloat )
{
	float retfloat = 0.0f;
	unsigned char* srcptr;
	unsigned char* dstptr;

	srcptr = (unsigned char*)( &orgfloat );
	dstptr = (unsigned char*)( &retfloat ) + 3;

	int i;
	for( i = 0; i < 4; i++ ){

		*dstptr = *srcptr;

		if( i <= 2 ){
			srcptr++;
			dstptr--;
		}
	}
	return retfloat;
}



