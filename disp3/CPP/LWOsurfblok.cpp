#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <LWOsurfblok.h>


CLWOsurfblok::CLWOsurfblok()
{
	InitParams();
}
CLWOsurfblok::~CLWOsurfblok()
{
	DestroyObjs();
}


int CLWOsurfblok::InitParams()
{
	im_orderstr = 0;
	im_chan[0] = 0;
	im_enab = 1;//!!!!!!!!

	tm_cntr.x = 0.0f;
	tm_cntr.y = 0.0f;
	tm_cntr.z = 0.0f;

	tm_size.x = 1.0f;
	tm_size.y = 1.0f;
	tm_size.z = 1.0f;

	tm_rota.x = 0.0f;
	tm_rota.y = 0.0f;
	tm_rota.z = 0.0f;

	tm_oref = 0;
	tm_csys = 0;

	proj = 0;
	axis = 0;
	imag = -1;//!!!
	width_wrap = 0;
	height_wrap = 0;
	wrpw = 1.0f;
	wrph = 1.0f;

	vmapname = 0;

	return 0;
}
int CLWOsurfblok::DestroyObjs()
{
	if( im_orderstr ){
		free( im_orderstr );
		im_orderstr = 0;
	}

	if( tm_oref ){
		free( tm_oref );
		tm_oref = 0;
	}

	if( vmapname ){
		free( vmapname );
		vmapname = 0;
	}

	return 0;
}

int CLWOsurfblok::SetImOrderStr( char* srcorder )
{
	int orderlen;
	orderlen = (int)strlen( srcorder );
	im_orderstr = (char*)realloc( im_orderstr, orderlen + 1 );
	if( !im_orderstr ){
		DbgOut( "LWOsurfblok : SetImOrderStr : im_orderstr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( im_orderstr, srcorder );


	return 0;
}
int CLWOsurfblok::SetImChan( char* srcchan )
{
	int chanlen;
	chanlen = (int)strlen( srcchan );
	if( chanlen != 4 ){
		DbgOut( "LWOsurfblok : SetImChan : chanlen error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( im_chan, srcchan );


	return 0;
}
int CLWOsurfblok::SetImEnab( int srcenab )
{

	im_enab = srcenab;

	return 0;
}

int CLWOsurfblok::SetTmCntr( D3DXVECTOR3 srccntr )
{
	tm_cntr = srccntr;

	return 0;
}
int CLWOsurfblok::SetTmSize( D3DXVECTOR3 srcsize )
{
	tm_size = srcsize;

	return 0;
}
int CLWOsurfblok::SetTmRota( D3DXVECTOR3 srcrota )
{
	tm_rota = srcrota;

	return 0;
}
int CLWOsurfblok::SetTmOref( char* srcoref )
{
	int oreflen;
	oreflen = (int)strlen( srcoref );
	tm_oref = (char*)realloc( tm_oref, oreflen + 1 );
	if( !tm_oref ){
		DbgOut( "LWOsurfblok : SetTmOref : tm_oref alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( tm_oref, srcoref );

	return 0;
}
int CLWOsurfblok::SetTmCsys( int srccsys )
{

	tm_csys = srccsys;

	return 0;
}

int CLWOsurfblok::SetProj( int srcproj )
{

	proj = srcproj;

	return 0;
}
int CLWOsurfblok::SetAxis( int srcaxis )
{

	axis = srcaxis;

	return 0;
}
int CLWOsurfblok::SetImag( int srcimag )
{

	imag = srcimag;

	return 0;
}
int CLWOsurfblok::SetWidthWrap( int srcwwrap )
{

	width_wrap = srcwwrap;

	return 0;
}
int CLWOsurfblok::SetHeightWrap( int srchwrap )
{

	height_wrap = srchwrap;

	return 0;
}
int CLWOsurfblok::SetWrapW( float srcwrpw )
{
	wrpw = srcwrpw;

	return 0;
}
int CLWOsurfblok::SetWrapH( float srcwrph )
{
	wrph = srcwrph;

	return 0;
}

int CLWOsurfblok::SetVmapname( char* srcvmap )
{
	int vmaplen;
	vmaplen = (int)strlen( srcvmap );
	vmapname = (char*)realloc( vmapname, vmaplen + 1 );
	if( !vmapname ){
		DbgOut( "LWOsurfblok : SetVmapname : vmapname alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( vmapname, srcvmap );

	return 0;
}


