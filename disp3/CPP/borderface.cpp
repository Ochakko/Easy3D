#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <borderface.h>

// static
static int s_allocno = 0;


CBorderFace::CBorderFace()
{
	InitParams();
	s_allocno++;
}

void CBorderFace::InitParams()
{
	serialno = s_allocno;
	p1 = 0;
	p2 = 0;
	other = 0;
	clockwise = 0;
	useflag = 0;
	nvec.x = 0.0f;
	nvec.y = 0.0f;
	nvec.z = 0.0f;
	next = 0;

	areano = 0;
}

CBorderFace::~CBorderFace()
{
	DestroyObjs();
}

void CBorderFace::DestroyObjs()
{

}

int CBorderFace::SetParams( int srcp1, int srcp2, int srcother, int divx, int divz )
{
	int ret;

	p1 = srcp1;
	p2 = srcp2;
	other = srcother;

// nvec
	D3DXVECTOR3 v1, v2, vother;
	ret = Index2Position( p1, divx, divz, &v1 );
	if( ret ){
		DbgOut( "borderface : SetParams : Index2Position error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Index2Position( p2, divx, divz, &v2 );
	if( ret ){
		DbgOut( "borderface : SetParams : Index2Position error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = Index2Position( other, divx, divz, &vother );
	if( ret ){
		DbgOut( "borderface : SetParams : Index2Position error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 v3;
	v3.x = v1.x;
	v3.z = v1.z;
	v3.y = 10.0f;

	ret = CalcNormal( &nvec, &v1, &v2, &v3 );
	if( ret ){
		DbgOut( "borderface : SetParams : CalcNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// clockwise
	D3DXVECTOR3 center, compvec;
	center = (v1 + v2) * 0.5f;

	compvec = vother - center;
	D3DXVec3Normalize( &compvec, &compvec );

	float dot;
	dot = D3DXVec3Dot( &nvec, &compvec );

	if( dot >= 0 ){
		clockwise = 1;
	}else{
		clockwise = 2;
	}

// useflag
	useflag = 1;


	return 0;
}



int CBorderFace::Index2Position( int pno, int divx, int divz, D3DXVECTOR3* dstv )
{
	float step = 10.0f;
	int indexx, indexz;


	indexz = pno / (divx + 1);
	indexx = pno - (divx + 1) * indexz;

	dstv->x = step * indexx;
	dstv->z = step * indexz;
	dstv->y = 0.0f;
	
	return 0;
}


int CBorderFace::CalcNormal( D3DXVECTOR3* newn, D3DXVECTOR3* curp, D3DXVECTOR3* aftp1, D3DXVECTOR3* aftp2 )
{
	D3DXVECTOR3 vec1, vec2, crossvec;

	vec1.x = aftp1->x - curp->x;
	vec1.y = aftp1->y - curp->y;
	vec1.z = aftp1->z - curp->z;

	vec2.x = aftp2->x - curp->x;
	vec2.y = aftp2->y - curp->y;
	vec2.z = aftp2->z - curp->z;

	D3DXVec3Cross( &crossvec, &vec1, &vec2 );
	D3DXVec3Normalize( newn, &crossvec );

	//XVec3Cross( &crossvec, &vec1, &vec2 );
	//XVec3Normalize( newn, &crossvec );

	return 0;
}

float CBorderFace::CalcDot( CBorderFace* srcbf )
{
	float retdot = 1.0f;

	D3DXVECTOR3 nvec1, nvec2;

	if( clockwise == 1 )
		nvec1 = nvec;
	else
		nvec1 = -1.0f * nvec;

	if( srcbf->clockwise == 1 )
		nvec2 = srcbf->nvec;
	else
		nvec2 = -1.0f * srcbf->nvec;

	retdot = D3DXVec3Dot( &nvec1, &nvec2 );

	return retdot;
}


int CBorderFace::SetClockwise( D3DXVECTOR3 srcnvec )
{
	// srcnvec との内積がの符号により、clockwiseをセットする。

	float dot;

	dot = D3DXVec3Dot( &nvec, &srcnvec );

	if( dot >= 0.0f ){
		clockwise = 1;
	}else{
		clockwise = 2;
	}
	return 0;
}


