#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <LWOsurf.h>


CLWOsurf::CLWOsurf()
{
	InitParams();
}
CLWOsurf::~CLWOsurf()
{
	DestroyObjs();
}

int CLWOsurf::InitParams()
{
	name = 0;
	source = 0;

	color.x = 0.0f;
	color.y = 0.0f;
	color.z = 0.0f;

	diff = 1.0f;
	spec = 0.0f;
	tran = 0.0f;

	side = 1; // 両面は３、裏面は？？

	vcoltypestr[0] = 0;
	vcolname = 0;

	sbnum = 0;
	ppsb = 0;

	//next = 0;

	return 0;
}
int CLWOsurf::DestroyObjs()
{
	if( name ){
		free( name );
		name = 0;
	}

	if( source ){
		free( source );
		source = 0;
	}

	if( vcolname ){
		free( vcolname );
		vcolname = 0;
	}

	if( sbnum > 0 ){
		int sbno;
		for( sbno = 0; sbno < sbnum; sbno++ ){
			CLWOsurfblok* delsb;
			delsb = *( ppsb + sbno );

			if( delsb ){
				delete delsb;
			}
		}

		free( ppsb );
		ppsb = 0;
	}
	sbnum = 0;

	return 0;
}

int CLWOsurf::SetName( char* srcname )
{
	int namelen;
	namelen = (int)strlen( srcname );

	name = (char*)realloc( name, namelen + 1 );
	if( !name ){
		DbgOut( "LWOsurf : SetName : name alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( name, srcname );

	return 0;
}
int CLWOsurf::SetSourceName( char* srcsource )
{
	int sourcelen;
	sourcelen = (int)strlen( srcsource );

	source = (char*)realloc( source, sourcelen + 1 );
	if( !source ){
		DbgOut( "LWOsurf : SetSourceName : source alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( source, srcsource );

	return 0;
}

int CLWOsurf::SetColor( D3DXVECTOR3 srccol )
{
	color = srccol;

	return 0;
}
int CLWOsurf::SetDiff( float srcdiff )
{

	diff = srcdiff;

	return 0;
}
int CLWOsurf::SetSpec( float srcspec )
{
	spec = srcspec;

	return 0;
}
int CLWOsurf::SetTran( float srctran )
{
	tran = srctran;

	return 0;
}

int CLWOsurf::SetSide( int srcside )
{
	side = srcside;

	return 0;
}

int CLWOsurf::SetVcol( char* srctype, char* srcname )
{
	int typelen;
	typelen = (int)strlen( srctype );
	if( typelen != 4 ){
		DbgOut( "LWOsurf : SetVcol : typelen error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( vcoltypestr, srctype );
////
	int namelen;
	namelen = (int)strlen( srcname );
	vcolname = (char*)realloc( vcolname, namelen + 1 );
	if( !vcolname ){
		DbgOut( "LWOsurf : SetVcol : vcolname alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	strcpy( vcolname, srcname );


	return 0;
}

CLWOsurfblok* CLWOsurf::AddSurfBlock()
{
	CLWOsurfblok* newsb = 0;

	newsb = new CLWOsurfblok();
	if( !newsb ){
		DbgOut( "LWOsurf : AddSurfBlock : newsb alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;// !!!
	}

	ppsb = (CLWOsurfblok**)realloc( ppsb, sizeof( CLWOsurfblok* ) * (sbnum + 1) );
	if( !ppsb ){
		DbgOut( "LWOsurf : AddSurfBlock : ppsb alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;// !!!
	}

	*( ppsb + sbnum ) = newsb;

	sbnum++;

	return newsb;//!!!
}


