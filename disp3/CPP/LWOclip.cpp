#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#include <LWOclip.h>


CLWOclip::CLWOclip()
{
	InitParams();
}
CLWOclip::~CLWOclip()
{
	DestroyObjs();
}

int CLWOclip::SetIndex( unsigned int srcindex )
{
	index = srcindex;
	return 0;
}

int CLWOclip::SetStil( char* srcname )
{
	DestroyObjs();

	int namelen;
	namelen = (int)strlen( srcname );
	nameptr = (char*)realloc( nameptr, namelen + 1 );
	if( !nameptr ){
		DbgOut( "LWOclip : SetClip : nameptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( nameptr, srcname );

	strcpy( typestr, "CLIP" );


	return 0;
}
int CLWOclip::SetIseq( unsigned char srcnumdig, unsigned char srcflag, short srcoff, unsigned short srcres,
	short srcstart, short srcend, char* srcpre, char* srcsuff )
{
	DestroyObjs();

	int prelen;
	prelen = (int)strlen( srcpre );
	prefixptr = (char*)realloc( prefixptr, prelen + 1 );
	if( !prefixptr ){
		DbgOut( "LWOclip : SetIseq : prefixptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( prefixptr, srcpre );


	int sufflen;
	sufflen = (int)strlen( srcsuff );
	suffixptr = (char*)realloc( suffixptr, sufflen + 1 );
	if( !suffixptr ){
		DbgOut( "LWOclip : SetIseq : suffixptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( suffixptr, srcsuff );

	numdigits = srcnumdig;
	flag = srcflag;
	offset = srcoff;
	reserved = srcres;
	start = srcstart;
	end = srcend;


	strcpy( typestr, "ISEQ" );

	return 0;
}
int CLWOclip::SetXref( unsigned int srcindex, char* srcname )
{
	DestroyObjs();

	int namelen;
	namelen = (int)strlen( srcname );
	nameptr = (char*)realloc( nameptr, namelen + 1 );
	if( !nameptr ){
		DbgOut( "LWOclip : SetXref : nameptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( nameptr, srcname );

	xrefindex = srcindex;
	
	strcpy( typestr, "XREF" );

	return 0;
}


int CLWOclip::InitParams()
{
	index = 0;
	typestr[0] = 0;
	
	nameptr = 0;

	numdigits = 0;
	flag = 0;
	offset = 0;
	reserved = 0;
	start = 0;
	end = 0;
	prefixptr = 0;
	suffixptr = 0;

	xrefindex = 0;

	//next = 0;

	return 0;
}
int CLWOclip::DestroyObjs()
{
	if( nameptr ){
		free( nameptr );
		nameptr = 0;
	}

	if( prefixptr ){
		free( prefixptr );
		prefixptr = 0;
	}

	if( suffixptr ){
		free( suffixptr );
		suffixptr = 0;
	}

	return 0;
}


