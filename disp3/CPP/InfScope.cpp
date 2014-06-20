#include "StdAfx.h"
#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <coef.h>
#include <InfScope.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <polymesh.h>
#include <polymesh2.h>
#include <d3ddisp.h>


///// extern 

extern int g_useGPU;

////////////


CInfScope::CInfScope()
{
	InitParams();
}

CInfScope::~CInfScope()
{
	DestroyObjs();
}

int CInfScope::InitParams()
{

	type = SHDTYPENONE;
	dispflag = 0;
	applychild = 0;
	target = 0;
	anchormaterialno = -1;
	polymesh2 = 0;
	polymesh = 0;
	d3ddisp = 0;
	pm2facet = 180.0f;

	return 0;

}
int CInfScope::DestroyObjs()
{
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	if( polymesh2 ){
		delete polymesh2;
		polymesh2 = 0;
	}

	if( polymesh ){
		delete polymesh;
		polymesh = 0;
	}

	return 0;
}

int CInfScope::InvalidateDispObj()
{
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	return 0;
}

int CInfScope::ResetPrimNum()
{
	if( d3ddisp ){
		d3ddisp->ResetPrimNum();
	}
	return 0;
}

int CInfScope::CalcInitialInfElem()
{
	int ret;

	int bonematno, childno;
	CShdElem* parelem;

	if( applychild ){
		childno = applychild->serialno;
		parelem = applychild->parent;
		if( parelem ){
			bonematno = parelem->serialno;
		}else{
			bonematno = 0;
		}
	}else{
		childno = 0;
		bonematno = 0;
	}
	

	switch( type ){
	case SHDPOLYMESH:
		_ASSERT( polymesh );
		ret = polymesh->CalcInitialInfElem( bonematno, childno );
		if( ret ){
			DbgOut( "infscope : pm CalcInitialInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		_ASSERT( polymesh2 );
		ret = polymesh2->CalcInitialInfElem( bonematno, childno );
		if( ret ){
			DbgOut( "infscope : pm2 CalcInitialInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		break;
	}

	return 0;
}

int CInfScope::SetPolyMesh2( CPolyMesh2* pm2, CD3DDisp* srcd3ddisp, float srcfacet )
{

	DestroyObjs();//!!!!!!!


	type = SHDPOLYMESH2;
	//dispflag = 0;
	applychild = pm2->m_mikoapplychild;
	target = pm2->m_mikotarget;
	anchormaterialno = pm2->m_anchormaterialno;
	polymesh2 = pm2;
	polymesh = 0;
	d3ddisp = srcd3ddisp;

	pm2facet = srcfacet;

	return 0;
}
int CInfScope::SetPolyMesh( CPolyMesh* pm, CD3DDisp* srcd3ddisp )
{
	DestroyObjs();//!!!!!!!


	type = SHDPOLYMESH;
	//dispflag = 0;
	applychild = pm->m_mikoapplychild;
	target = pm->m_mikotarget;
	anchormaterialno = 0;
	polymesh2 = 0;
	polymesh = pm;
	d3ddisp = srcd3ddisp;

	return 0;

}


int CInfScope::CheckVertInScope( D3DXVECTOR3 srcv, int clockwise, int* insideptr )
{

	int ret;

	switch( type ){
	case SHDPOLYMESH:
		ret = polymesh->CheckVertInShape( srcv, clockwise, insideptr );
		if( ret ){
			DbgOut( "is : CheckVertInScope : pm CheckVertInShape error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		ret = polymesh2->CheckVertInShape( srcv, clockwise, insideptr );
		if( ret ){
			DbgOut( "is : CheckVertInScope : pm2 CheckVertInShape error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		*insideptr = 0;
		break;
	}

	return 0;
}

int CInfScope::GetPointNum( int* pointnumptr )
{
	switch( type ){
	case SHDPOLYMESH:
		if( polymesh ){
			*pointnumptr = polymesh->meshinfo->m;
		}else{
			*pointnumptr = 0;
		}
		break;
	case SHDPOLYMESH2:
		if( polymesh2 ){
			*pointnumptr = polymesh2->optpleng;
		}else{
			*pointnumptr = 0;
		}
		break;
	default:
		*pointnumptr = 0;
		break;
	}

	return 0;
}
int CInfScope::GetPointBuf( D3DXVECTOR3* pbuf )
{
	int ret;
	int pno, pointnum;

	switch( type ){
	case SHDPOLYMESH:
		if( !polymesh ){
			_ASSERT( 0 );
			return 1;
		}
		pointnum = polymesh->meshinfo->m;
		for( pno = 0; pno < pointnum; pno++ ){
			ret = polymesh->GetPointBuf( pno, pbuf + pno );
			if( ret ){
				DbgOut( "infscope : GetPontBuf : pm GetPointBuf error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		break;
	case SHDPOLYMESH2:
		if( !polymesh2 ){
			_ASSERT( 0 );
			return 1;
		}
		pointnum = polymesh2->optpleng;
		for( pno = 0; pno < pointnum; pno++ ){
			ret = polymesh2->GetOptPos( pno, pbuf + pno );
			if( ret ){
				DbgOut( "infscope : GetPointBuf : pm2 GetOptPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		break;
	default:
		_ASSERT( 0 );
		return 1;
		break;
	}
	return 0;
}

int CInfScope::GetCenter( D3DXVECTOR3* centerptr )
{

	int ret;

	switch( type ){
	case SHDPOLYMESH:
		if( !polymesh ){
			_ASSERT( 0 );
			return 1;
		}
		ret = polymesh->CalcAnchorCenter();
		_ASSERT( !ret );
		*centerptr = polymesh->m_center;

		break;
	case SHDPOLYMESH2:
		if( !polymesh2 ){
			_ASSERT( 0 );
			return 1;
		}
		ret = polymesh2->CalcAnchorCenter();
		_ASSERT( !ret );
		*centerptr = polymesh2->m_center;

		break;
	default:
		_ASSERT( 0 );
		return 1;
		break;
	}


	return 0;
}

int CInfScope::SetPointBuf( D3DXVECTOR3* newp, int vertno )
{

	if( !d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}
		
	int vertnum;
	vertnum = d3ddisp->m_numTLV;
	if( (vertno < 0) || (vertno >= vertnum) ){
		DbgOut( "infscope : SetPointBuf : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( d3ddisp->m_skinv );
	( d3ddisp->m_skinv + vertno )->pos[0] = newp->x;
	( d3ddisp->m_skinv + vertno )->pos[1] = newp->y;
	( d3ddisp->m_skinv + vertno )->pos[2] = newp->z;

	int ret;

	switch( type ){
	case SHDPOLYMESH:
		if( !polymesh ){
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->SetPointBuf( vertno, newp->x, newp->y, newp->z, -999 );
		if( ret ){
			DbgOut( "infscope : SetPointBuf : pm SetPointBuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SHDPOLYMESH2:
		if( !polymesh2 ){
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetOptPos( vertno, *newp );
		if( ret ){
			DbgOut( "infscope : SetPointBuf : pm2 SetOptPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		_ASSERT( 0 );
		break;
	}

	return 0;
}

