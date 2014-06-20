
#include "StdAfx.h"
#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <coef.h>
#include <boneelem.h>

#include <boneinfo.h>

#include <shdhandler.h>
#include <shdelem.h>

CBoneElem::CBoneElem()
{
	bonematno = -1;
	parmatno = -1;
	bonerate = 0.0f;

	childno = -1;
	calcmode = CALCMODE_ONESKIN0;

	mikoapchildnum = 0;
	mikoapchildseri = 0;
	nearestbi = 0;	
	nearestbipar = 0;

}

CBoneElem::~CBoneElem()
{

	ClearMikoParams();
}


int CBoneElem::ClearMikoParams()
{
	if( mikoapchildseri ){
		free( mikoapchildseri );
		mikoapchildseri = 0;
	}
	mikoapchildnum = 0;
	nearestbi = 0;
	nearestbipar = 0;

	return 0;
}

int CBoneElem::AddMikoApChild( int srcchildseri )
{
	mikoapchildnum++;

	mikoapchildseri = (int*)realloc( mikoapchildseri, sizeof( int ) * mikoapchildnum );
	if( !mikoapchildseri ){
		DbgOut( "bi : AddMikoApChild : mikoapchildseri alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( mikoapchildseri + mikoapchildnum - 1 ) = srcchildseri;

	return 0;
}

int CBoneElem::SetNearestBI( CShdHandler* srclpsh, D3DXVECTOR3 srcv )
{
	int ret;

	/***
	ret = Replace2FloatBone( srclpsh );
	if( ret ){
		DbgOut( "boneelem : SetNearestBI : Replace2FloatBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

	int i;
	int curapchil;
	CShdElem* curapchilelem;
	CPart* chilpart;

	CShdElem* parelem;
	CPart* parpart;

	D3DXVECTOR3 bonecenter;
	D3DXVECTOR3 diff;
	float curdist;

	float mindist = 1e6;
	CShdElem* nearapchilelem = 0;
	CPart* nearparpart = 0;

	for( i = 0; i < mikoapchildnum; i++ ){
		curapchil = *( mikoapchildseri + i );
		curapchilelem = (*srclpsh)( curapchil );
		if( !curapchilelem ){
			_ASSERT( 0 );
			return 1;
		}
		chilpart = curapchilelem->part;
		if( !chilpart ){
			DbgOut( "boneelem : SetNearestBI : chilpart NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		parelem = srclpsh->FindUpperJoint( curapchilelem, 0 );
		if( parelem ){
			parpart = parelem->part;
			if( !parpart ){
				_ASSERT( 0 );
				return 1;
			}
			
			/***
			bonecenter.x = ( chilpart->jointloc.x + parpart->jointloc.x ) * 0.5f;
			bonecenter.y = ( chilpart->jointloc.y + parpart->jointloc.y ) * 0.5f;
			bonecenter.z = ( chilpart->jointloc.z + parpart->jointloc.z ) * 0.5f;

			diff = srcv - bonecenter;
			***/
			diff.x = srcv.x - parpart->jointloc.x;
			diff.y = srcv.y - parpart->jointloc.y;
			diff.z = srcv.z - parpart->jointloc.z;


			curdist = D3DXVec3Length( &diff );

			if( curdist <= mindist ){
				nearapchilelem = curapchilelem;
				nearparpart = parpart;
				mindist = curdist;
			}

		}
	}

	if( nearapchilelem && nearparpart ){
		ret = nearparpart->GetBoneInfo( nearapchilelem->serialno, &nearestbi );
		if( ret ){
			DbgOut( "boneelem : SetNearestBI : part GetBoneInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int parno;
		if( nearestbi )
			parno = nearestbi->parentjoint;
		else
			parno = -1;

		if( parno > 0 ){
			nearestbipar = (*srclpsh)( parno );
		}else{
			nearestbipar = 0;
		}
		
	}

	return 0;
}

int CBoneElem::Replace2FloatBone( CShdHandler* srclpsh )
{
	if( mikoapchildnum != 2 )
		return 0;

	CShdElem* selem1;
	CShdElem* selem2;

	selem1 = (*srclpsh)( *mikoapchildseri );
	selem2 = (*srclpsh)( *(mikoapchildseri + 1) );

	if( !selem1 || !selem2 ){
		DbgOut( "boneelem : Replace2FloatBone : selem NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* par1;
	CShdElem* par2;

	par1 = srclpsh->FindUpperJoint( selem1, 0 );
	par2 = srclpsh->FindUpperJoint( selem2, 0 );

	if( (par1 == selem2) || (par2 == selem1) ){
		return 0;
	}

	CShdElem* gpa1 = 0;
	CShdElem* gpa2 = 0;
	
	if( par1 )
		gpa1 = srclpsh->FindUpperJoint( par1, 0 );

	if( par2 )
		gpa2 = srclpsh->FindUpperJoint( par2, 0 );


	if( gpa1 && (gpa1 == selem2) ){
		*( mikoapchildseri + 1 ) = par1->serialno;
	}else if( gpa2 && (gpa2 == selem1) ){
		*mikoapchildseri = par2->serialno;
	}


	return 0;
}


