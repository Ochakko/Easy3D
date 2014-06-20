#include <stdafx.h> //É_É~Å[
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <math.h>

#include <morph.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <d3ddisp.h>
#include <polymesh.h>
#include <polymesh2.h>


#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CMorph::CMorph()
{
	InitParams();
}
CMorph::~CMorph()
{
	DestroyObjs();
}

void CMorph::InitParams()
{
	CBaseDat::InitParams();

	objtype = 0;
	morphnum = 0;

	hd3ddisp = 0;
	hselem = 0;
}
void CMorph::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	if( hd3ddisp ){
		CD3DDisp* deldisp;
		int delno;
		for( delno = 0; delno < morphnum; delno++ ){
			deldisp = *( hd3ddisp + delno );
			if( deldisp )
				delete deldisp;
		}

		free( hd3ddisp );
		hd3ddisp = 0;
	}


	if( hselem ){
		free( hselem );
		hselem = 0;
	}

	morphnum = 0;
}

int CMorph::AddMorphElem( CShdElem* addselem )
{
	morphnum++;

	hselem = (CShdElem**)realloc( hselem, sizeof( CShdElem* ) * morphnum );
	if( !hselem ){
		DbgOut( "morph : AddMorphElem : hselem alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hd3ddisp = (CD3DDisp**)realloc( hd3ddisp, sizeof( CD3DDisp* ) * morphnum );
	if( !hd3ddisp ){
		DbgOut( "morph : AddMorphElem : hd3ddisp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( hselem + morphnum - 1 ) = addselem;
	*( hd3ddisp + morphnum - 1 ) = 0;

	return 0;
}

int CMorph::DeleteMorphElem( CShdElem* delelem )
{
	int delindex = -1;
	int mno;
	CShdElem* melem;
	for( mno = 0; mno < morphnum; mno++ ){
		melem = *( hselem + mno );
		_ASSERT( melem );

		if( melem == delelem ){
			delindex = mno;
			break;
		}
	}


	if( delindex >= 0 ){

		if( morphnum != 1 ){
			CShdElem** newhselem;
			newhselem = (CShdElem**)malloc( sizeof( CShdElem* ) * (morphnum - 1) );
			if( !newhselem ){
				DbgOut( "morph : DeleteMorphElem : newhselem alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			CD3DDisp** newhd3ddisp;
			newhd3ddisp = (CD3DDisp**)malloc( sizeof( CD3DDisp* ) * (morphnum - 1) );
			if( !newhd3ddisp ){
				DbgOut( "morph : DeleteMorphElem : newhd3ddisp alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int setno = 0;
			for( mno = 0; mno < morphnum; mno++ ){
				if( mno != delindex ){
					*( newhselem + setno ) = *( hselem + mno );
					*( newhd3ddisp + setno ) = *( hd3ddisp + mno );
					setno++;
				}
			}
			_ASSERT( setno == (morphnum - 1) );


			if( hselem )
				free( hselem );
			if( hd3ddisp )
				free( hd3ddisp );

			hselem = newhselem;
			hd3ddisp = newhd3ddisp;

			morphnum--;
		}else{
			morphnum = 0;

			free( hselem );
			hselem = 0;

			free( hd3ddisp );
			hd3ddisp = 0;
		}
	}

	return 0;
}
