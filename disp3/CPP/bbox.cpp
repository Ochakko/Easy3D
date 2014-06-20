#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <bbox.h>

#include <polymesh.h>
#include <polymesh2.h>

#include <bbox2.h>

#define	DBGH
#include <dbg.h>


CBBox::CBBox()
{
	InitParams();
}

CBBox::~CBBox()
{

}

int CBBox::InitParams()
{
	minx = 1e8;
	maxx = -1e8;
	miny = 1e8;
	maxy = -1e8;
	minz = 1e8;
	maxz = -1e8;

	setflag = 0;
	return 0;
}

int CBBox::SetBBox( float srcminx, float srcmaxx, float srcminy, float srcmaxy, float srcminz, float srcmaxz )
{
	setflag = 1;//!!!!!!!

	minx = srcminx;
	maxx = srcmaxx;

	miny = srcminy;
	maxy = srcmaxy;

	minz = srcminz;
	maxz = srcmaxz;

	return 0;
}

int CBBox::SetBBox( CBBox* srcbbx )
{
	setflag = 1;

	minx = srcbbx->minx;
	maxx = srcbbx->maxx;

	miny = srcbbx->miny;
	maxy = srcbbx->maxy;

	minz = srcbbx->minz;
	maxz = srcbbx->maxz;

	return 0;
}


int CBBox::SetBBox( CPolyMesh* srcpm )
{
	InitParams();

	int vno;
	float x, y, z;
	for( vno = 0; vno < srcpm->meshinfo->m; vno++ ){
		x = (srcpm->pointbuf + vno)->x;
		y = (srcpm->pointbuf + vno)->y;
		z = (srcpm->pointbuf + vno)->z;

		if( x < minx ){
			minx = x;
		}
		if( x > maxx ){
			maxx = x;
		}

		if( y < miny ){
			miny = y;
		}
		if( y > maxy ){
			maxy = y;
		}

		if( z < minz ){
			minz = z;
		}
		if( z > maxz ){
			maxz = z;
		}

	}
	
	setflag = 1;//!!!!!!!

	return 0;
}
int CBBox::SetBBox( CPolyMesh2* srcpm2 )
{
	InitParams();

	int vno;
	float x, y, z;
	for( vno = 0; vno < (srcpm2->meshinfo->n * 3); vno++ ){
		x = (srcpm2->pointbuf + vno)->x;
		y = (srcpm2->pointbuf + vno)->y;
		z = (srcpm2->pointbuf + vno)->z;

		if( x < minx ){
			minx = x;
		}
		if( x > maxx ){
			maxx = x;
		}

		if( y < miny ){
			miny = y;
		}
		if( y > maxy ){
			maxy = y;
		}

		if( z < minz ){
			minz = z;
		}
		if( z > maxz ){
			maxz = z;
		}

	}

	setflag = 1;//!!!!!!!!!!

	return 0;
}


int CBBox::ChkConflict( CBBox chkbb, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2 )
{

	if( (setflag == 0) || (chkbb.setflag == 0) )
		return 0;


	int conf = 0;
	float srcminx, srcmaxx, srcminy, srcmaxy, srcminz, srcmaxz;
	float srcmidx, srcmidy, srcmidz;
	float chkmidx, chkmidy, chkmidz;
	float chkminx, chkmaxx, chkminy, chkmaxy, chkminz, chkmaxz;

	srcmidx = ( minx + maxx ) * 0.5f;
	srcmidy = ( miny + maxy ) * 0.5f;
	srcmidz = ( minz + maxz ) * 0.5f;


	chkminx = chkbb.minx;
	chkmaxx = chkbb.maxx;
	chkminy = chkbb.miny;
	chkmaxy = chkbb.maxy;
	chkminz = chkbb.minz;
	chkmaxz = chkbb.maxz;

	chkmidx = ( chkminx + chkmaxx ) * 0.5f;
	chkmidy = ( chkminy + chkmaxy ) * 0.5f;
	chkmidz = ( chkminz + chkmaxz ) * 0.5f;

	///////
	srcminx = ( minx - srcmidx ) * rate1.x + srcmidx;
	srcmaxx = ( maxx - srcmidx ) * rate1.x + srcmidx;

	srcminy = ( miny - srcmidy ) * rate1.y + srcmidy;
	srcmaxy = ( maxy - srcmidy ) * rate1.y + srcmidy;

	srcminz = ( minz - srcmidz ) * rate1.z + srcmidz;
	srcmaxz = ( maxz - srcmidz ) * rate1.z + srcmidz;

	////////
	chkminx = ( chkminx - chkmidx ) * rate2.x + chkmidx;
	chkmaxx = ( chkmaxx - chkmidx ) * rate2.x + chkmidx;

	chkminy = ( chkminy - chkmidy ) * rate2.y + chkmidy;
	chkmaxy = ( chkmaxy - chkmidy ) * rate2.y + chkmidy;

	chkminz = ( chkminz - chkmidz ) * rate2.z + chkmidz;
	chkmaxz = ( chkmaxz - chkmidz ) * rate2.z + chkmidz;


//DbgOut( "HandlerSet : ChkConflict : src %f %f, %f %f, %f %f\n", 
//	   minx, maxx, miny, maxy, minz, maxz );
//DbgOut( "HandlerSet : ChkConflict : chk %f %f, %f %f, %f %f\n", 
//	   chkminx, chkmaxx, chkminy, chkmaxy, chkminz, chkmaxz );

	float lengx, lengy, lengz;
	float chklengx, chklengy, chklengz;

	lengx = srcmaxx - srcminx;
	lengy = srcmaxy - srcminy;
	lengz = srcmaxz - srcminz;

	chklengx = chkmaxx - chkminx;
	chklengy = chkmaxy - chkminy;
	chklengz = chkmaxz - chkminz;

	int xconf = 0;
	int yconf = 0;
	int zconf = 0;
	if( lengx >= chklengx ){
		if( ((chkminx >= srcminx) && (chkminx <= srcmaxx)) ||
			((chkmaxx >= srcminx) && (chkmaxx <= srcmaxx)) ){
			xconf = 1;
		}
	}else{
		if( ((srcminx >= chkminx) && (srcminx <= chkmaxx)) ||
			((srcmaxx >= chkminx) && (srcmaxx <= chkmaxx)) ){
			xconf = 1;
		}
	}

	if( lengy >= chklengy ){
		if( ((chkminy >= srcminy) && (chkminy <= srcmaxy)) ||
			((chkmaxy >= srcminy) && (chkmaxy <= srcmaxy)) ){
			yconf = 1;
		}
	}else{
		if( ((srcminy >= chkminy) && (srcminy <= chkmaxy)) ||
			((srcmaxy >= chkminy) && (srcmaxy <= chkmaxy)) ){
			yconf = 1;
		}
	}

	if( lengz >= chklengz ){
		if( ((chkminz >= srcminz) && (chkminz <= srcmaxz)) ||
			((chkmaxz >= srcminz) && (chkmaxz <= srcmaxz)) ){
			zconf = 1;
		}
	}else{
		if( ((srcminz >= chkminz) && (srcminz <= chkmaxz)) ||
			((srcmaxz >= chkminz) && (srcmaxz <= chkmaxz)) ){
			zconf = 1;
		}
	}

	if( xconf && yconf && zconf )
		return 1;
	else
		return 0;

}

int CBBox::JoinBBox( CBBox* addbb )
{
	//bboxの合成。

	if( (setflag == 0) && (addbb->setflag != 0) ){
		minx = addbb->minx;
		maxx = addbb->maxx;
		miny = addbb->miny;
		maxy = addbb->maxy;
		minz = addbb->minz;
		maxz = addbb->maxz;
		setflag = 1;
		return 0;
	}


	if( addbb->setflag ){
		if( minx > addbb->minx )
			minx = addbb->minx;
		else if( maxx < addbb->maxx )
			maxx = addbb->maxx;

		if( miny > addbb->miny )
			miny = addbb->miny;
		else if( maxy < addbb->maxy )
			maxy = addbb->maxy;

		if( minz > addbb->minz )
			minz = addbb->minz;
		else if( maxz < addbb->maxz )
			maxz = addbb->maxz;
	}

	return 0;
}

int CBBox::JoinBBox( D3DXVECTOR3* addv )
{
	//bboxの更新。

	if( setflag == 0 ){
		minx = addv->x;
		maxx = addv->x;

		miny = addv->y;
		maxy = addv->y;

		minz = addv->z;
		maxz = addv->z;
		
		setflag = 1;

		return 0;
	}else{

		setflag = 1;

		if( minx > addv->x )
			minx = addv->x;
		else if( maxx < addv->x )
			maxx = addv->x;

		if( miny > addv->y )
			miny = addv->y;
		else if( maxy < addv->y )
			maxy = addv->y;

		if( minz > addv->z )
			minz = addv->z;
		else if( maxz < addv->z )
			maxz = addv->z;

		return 0;
	}
}

int CBBox::SetBBox( CBBox2* srcbbx2 )
{

	InitParams();

	int vno;
	float x, y, z;
	for( vno = 0; vno < 8; vno++ ){
		x = srcbbx2->m_transvert[ vno ].x;
		y = srcbbx2->m_transvert[ vno ].y;
		z = srcbbx2->m_transvert[ vno ].z;

		if( x < minx ){
			minx = x;
		}
		if( x > maxx ){
			maxx = x;
		}

		if( y < miny ){
			miny = y;
		}
		if( y > maxy ){
			maxy = y;
		}

		if( z < minz ){
			minz = z;
		}
		if( z > maxz ){
			maxz = z;
		}
	}

	setflag = 1;

	return 0;
}

int CBBox::JoinBBox( CBBox2* srcbbx2 )
{

	int vno;
	float x, y, z;
	for( vno = 0; vno < 8; vno++ ){
		x = srcbbx2->m_transvert[ vno ].x;
		y = srcbbx2->m_transvert[ vno ].y;
		z = srcbbx2->m_transvert[ vno ].z;

		if( x < minx ){
			minx = x;
		}
		if( x > maxx ){
			maxx = x;
		}

		if( y < miny ){
			miny = y;
		}
		if( y > maxy ){
			maxy = y;
		}

		if( z < minz ){
			minz = z;
		}
		if( z > maxz ){
			maxz = z;
		}
	}

	setflag = 1;

	return 0;
}

int CBBox::TransformOnlyWorld( CBBox* srcbbx, D3DXMATRIX* matWorld )
{
	D3DXVECTOR3 befvert[8];

	befvert[0] = D3DXVECTOR3( srcbbx->maxx, srcbbx->maxy, srcbbx->maxz );
	befvert[1] = D3DXVECTOR3( srcbbx->maxx, srcbbx->miny, srcbbx->maxz );
	befvert[2] = D3DXVECTOR3( srcbbx->minx, srcbbx->miny, srcbbx->maxz );
	befvert[3] = D3DXVECTOR3( srcbbx->minx, srcbbx->maxy, srcbbx->maxz );

	befvert[4] = D3DXVECTOR3( srcbbx->maxx, srcbbx->maxy, srcbbx->minz );
	befvert[5] = D3DXVECTOR3( srcbbx->maxx, srcbbx->miny, srcbbx->minz );
	befvert[6] = D3DXVECTOR3( srcbbx->minx, srcbbx->miny, srcbbx->minz );
	befvert[7] = D3DXVECTOR3( srcbbx->minx, srcbbx->maxy, srcbbx->minz );

	D3DXVECTOR3 aftvert[8];

	InitParams();//!!!!!!!!!

	int vno;
	float x, y, z;
	for( vno = 0; vno < 8; vno++ ){
		D3DXVec3TransformCoord( aftvert + vno, befvert + vno, matWorld );
		x = aftvert[vno].x;
		y = aftvert[vno].y;
		z = aftvert[vno].z;

		if( x < minx ){
			minx = x;
		}
		if( x > maxx ){
			maxx = x;
		}

		if( y < miny ){
			miny = y;
		}
		if( y > maxy ){
			maxy = y;
		}

		if( z < minz ){
			minz = z;
		}
		if( z > maxz ){
			maxz = z;
		}
	}
	setflag = 1;


	return 0;
}


