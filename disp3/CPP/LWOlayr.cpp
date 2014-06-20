#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <LWOlayr.h>


#include <LWOvmap.h>
#include <LWOvmad.h>
#include <LWOpols.h>
#include <LWOptag.h>


CLWOlayr::CLWOlayr()
{
	InitParams();
}
CLWOlayr::~CLWOlayr()
{
	DestroyObjs();
}


int CLWOlayr::InitParams()
{
	number = 0;
	name = 0;
	parent = 0xFFFFFFFF;
	flag = 0;
	pivot.x = 0.0f;
	pivot.y = 0.0f;
	pivot.z = 0.0f;

	pntsnum = 0;
	PNTS = 0;

	BBOXmin.x = 0.0f;
	BBOXmin.y = 0.0f;
	BBOXmin.z = 0.0f;
	BBOXmax.x = 0.0f;
	BBOXmax.y = 0.0f;
	BBOXmax.z = 0.0f;

	vmapnum = 0;
	ppvmap = 0;

	vmadnum = 0;
	ppvmad = 0;

	polsnum = 0;
	pppols = 0;

	ptagnum = 0;
	ppptag = 0;

	//next = 0;

	return 0;
}
int CLWOlayr::DestroyObjs()
{
	if( name ){
		free( name );
		name = 0;
	}

	if( PNTS ){
		delete [] PNTS;
	}
	pntsnum = 0;

	if( vmapnum > 0 ){
		int vmapno;
		for( vmapno = 0; vmapno < vmapnum; vmapno++ ){
			CLWOvmap* delvmap;
			delvmap = *( ppvmap + vmapno );
			if( delvmap )
				delete delvmap;
		}
	}
	vmapnum = 0;
	if( ppvmap ){
		free( ppvmap );
		ppvmap = 0;
	}

	if( vmadnum > 0 ){
		int vmadno;
		for( vmadno = 0; vmadno < vmadnum; vmadno++ ){
			CLWOvmad* delvmad;
			delvmad = *( ppvmad + vmadno );
			if( delvmad )
				delete delvmad;
		}
	}
	vmadnum = 0;
	if( ppvmad ){
		free( ppvmad );
		ppvmad = 0;
	}

	if( polsnum > 0 ){
		int polsno;
		for( polsno = 0; polsno < polsnum; polsno++ ){
			CLWOpols* delpols;
			delpols = *( pppols + polsno );
			if( delpols )
				delete delpols;
		}
	}
	polsnum = 0;
	if( pppols ){
		free( pppols );
		pppols = 0;
	}

	if( ptagnum > 0 ){
		int ptagno;
		for( ptagno = 0; ptagno < ptagnum; ptagno++ ){
			CLWOptag* delptag;
			delptag = *( ppptag + ptagno );
			if( delptag )
				delete delptag;
		}
	}
	ptagnum = 0;
	if( ppptag ){
		free( ppptag );
		ppptag = 0;
	}

	return 0;
}


int CLWOlayr::SetLayerParams( unsigned int srcnumber, unsigned int srcflag, D3DXVECTOR3 srcpivot, char* srcname, unsigned int srcparent )
{
	number = srcnumber;
	flag = srcflag;
	pivot = srcpivot;

	int namelen;
	namelen = (int)strlen( srcname );
	name = (char*)realloc( name, namelen + 1 );
	if( !name ){
		DbgOut( "LWOlayr : SetLayerParams : name alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	parent = srcparent;

	return 0;
}

int CLWOlayr::CreatePNTS( int srcnum )
{
	if( PNTS ){
		delete [] PNTS;
		PNTS = 0;
	}

	PNTS = new D3DXVECTOR3[srcnum];
	if( !PNTS ){
		DbgOut( "LWOlayr : CreatePNTS : PNTS alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	pntsnum = srcnum;

	return 0;
}
int CLWOlayr::SetPNTS( int srcno, D3DXVECTOR3 srcvec )
{
	if( (srcno < 0) || (srcno >= pntsnum) ){
		DbgOut( "LWOlayr : SetPNTS : srcno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( PNTS + srcno ) = srcvec;

	return 0;
}

int CLWOlayr::SetBBOX( D3DXVECTOR3 srcmin, D3DXVECTOR3 srcmax )
{
	BBOXmin = srcmin;
	BBOXmax = srcmax;

	return 0;
}

CLWOvmap* CLWOlayr::AddVmap()
{
	CLWOvmap* newvmap = 0;

	ppvmap = (CLWOvmap**)realloc( ppvmap, sizeof( CLWOvmap* ) * (vmapnum + 1) );
	if( !ppvmap ){
		DbgOut( "LWOlayr : AddVmap : ppvmap alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newvmap = new CLWOvmap();
	if( !newvmap ){
		DbgOut( "LWOlayr : AddVmap : newvmap alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( ppvmap + vmapnum ) = newvmap;

	vmapnum++;

	return newvmap;
}

CLWOvmad* CLWOlayr::AddVmad()
{
	CLWOvmad* newvmad = 0;

	ppvmad = (CLWOvmad**)realloc( ppvmad, sizeof( CLWOvmad* ) * (vmadnum + 1) );
	if( !ppvmad ){
		DbgOut( "LWOlayr : AddVmad : ppvmad alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newvmad = new CLWOvmad();
	if( !newvmad ){
		DbgOut( "LWOlayr : AddVmad : newvmad alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( ppvmad + vmadnum ) = newvmad;

	vmadnum++;

	return newvmad;
}

CLWOpols* CLWOlayr::AddPols()
{
	CLWOpols* newpols = 0;

	pppols = (CLWOpols**)realloc( pppols, sizeof( CLWOpols* ) * (polsnum + 1) );
	if( !pppols ){
		DbgOut( "LWOlayr : AddPols : pppols alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newpols = new CLWOpols();
	if( !newpols ){
		DbgOut( "LWOlayr : AddPols : newpols alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( pppols + polsnum ) = newpols;

	polsnum++;

	return newpols;
}

CLWOptag* CLWOlayr::AddPtag()
{
	CLWOptag* newptag = 0;

	ppptag = (CLWOptag**)realloc( ppptag, sizeof( CLWOptag* ) * (ptagnum + 1) );
	if( !ppptag ){
		DbgOut( "LWOlayr : AddPtag : ppptag alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	newptag = new CLWOptag();
	if( !newptag ){
		DbgOut( "LWOlayr : AddPtag : newptag alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( ppptag + ptagnum ) = newptag;

	ptagnum++;

	return newptag;
}
