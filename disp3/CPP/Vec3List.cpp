#include <stdafx.h>

#include <Vec3List.h>

#include <bsphere.h>
#include <bbox.h>


#include <crtdbg.h>


CVec3List::CVec3List()
{
	m_pointnum = 0;
}
CVec3List::~CVec3List()
{

}

//! ƒ|ƒCƒ“ƒg‚ð•ÏŠ·‚·‚é
void CVec3List::Transform(D3DXMATRIX* pMat)
{
	for(int i = 0; i < m_pointnum; i++){
		D3DXVECTOR3	vec = m_cPointList[i];
		D3DXVec3TransformCoord(&vec, &vec, pMat);
		m_cPointList[i] = vec;
	}
}

//! AABB‚ðŽæ“¾‚·‚é
void CVec3List::GetAABB(D3DXVECTOR3* pMax, D3DXVECTOR3* pMin)
{
	*pMax = *pMin = m_cPointList[0];
	for(int i = 1; i < m_pointnum; i++){
		if(pMax->x < m_cPointList[i].x)			{ pMax->x = m_cPointList[i].x; }
		else if(pMin->x > m_cPointList[i].x)	{ pMin->x = m_cPointList[i].x; }
		if(pMax->y < m_cPointList[i].y)			{ pMax->y = m_cPointList[i].y; }
		else if(pMin->y > m_cPointList[i].y)	{ pMin->y = m_cPointList[i].y; }
		if(pMax->z < m_cPointList[i].z)			{ pMax->z = m_cPointList[i].z; }
		else if(pMin->z > m_cPointList[i].z)	{ pMin->z = m_cPointList[i].z; }
	}
}

int CVec3List::AddPoint( D3DXVECTOR3* srcv )
{

	if( m_pointnum >= (VEC3LISTMAX - 1) ){
		_ASSERT( 0 );
		return 0;
	}

	m_cPointList[m_pointnum] = *srcv;

	m_pointnum++;

	return 0;
}

int CVec3List::AddBSphere( CBSphere* srcbs, float srcscale )
{
	D3DXVECTOR3 tmpv;

	tmpv.x = srcbs->tracenter.x - srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y + srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z - srcbs->rmag * srcscale;
	AddPoint( &tmpv );

	tmpv.x = srcbs->tracenter.x - srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y - srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z - srcbs->rmag * srcscale;
	AddPoint( &tmpv );

	tmpv.x = srcbs->tracenter.x + srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y - srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z - srcbs->rmag * srcscale;
	AddPoint( &tmpv );

	tmpv.x = srcbs->tracenter.x + srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y + srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z - srcbs->rmag * srcscale;
	AddPoint( &tmpv );



	tmpv.x = srcbs->tracenter.x - srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y + srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z + srcbs->rmag * srcscale;
	AddPoint( &tmpv );

	tmpv.x = srcbs->tracenter.x - srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y - srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z + srcbs->rmag * srcscale;
	AddPoint( &tmpv );

	tmpv.x = srcbs->tracenter.x + srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y - srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z + srcbs->rmag * srcscale;
	AddPoint( &tmpv );

	tmpv.x = srcbs->tracenter.x + srcbs->rmag * srcscale;
	tmpv.y = srcbs->tracenter.y + srcbs->rmag * srcscale;
	tmpv.z = srcbs->tracenter.z + srcbs->rmag * srcscale;
	AddPoint( &tmpv );

	return 0;
}
int CVec3List::AddBBox( CBBox* srcbbx )
{
	if( srcbbx->setflag == 0 ){
		return 0;
	}


	D3DXVECTOR3 tmpv;

	tmpv.x = srcbbx->minx;
	tmpv.y = srcbbx->maxy;
	tmpv.z = srcbbx->minz;
	AddPoint( &tmpv );

	tmpv.x = srcbbx->minx;
	tmpv.y = srcbbx->miny;
	tmpv.z = srcbbx->minz;
	AddPoint( &tmpv );

	tmpv.x = srcbbx->maxx;
	tmpv.y = srcbbx->miny;
	tmpv.z = srcbbx->minz;
	AddPoint( &tmpv );

	tmpv.x = srcbbx->maxx;
	tmpv.y = srcbbx->maxy;
	tmpv.z = srcbbx->minz;
	AddPoint( &tmpv );



	tmpv.x = srcbbx->minx;
	tmpv.y = srcbbx->maxy;
	tmpv.z = srcbbx->maxz;
	AddPoint( &tmpv );

	tmpv.x = srcbbx->minx;
	tmpv.y = srcbbx->miny;
	tmpv.z = srcbbx->maxz;
	AddPoint( &tmpv );

	tmpv.x = srcbbx->maxx;
	tmpv.y = srcbbx->miny;
	tmpv.z = srcbbx->maxz;
	AddPoint( &tmpv );

	tmpv.x = srcbbx->maxx;
	tmpv.y = srcbbx->maxy;
	tmpv.z = srcbbx->maxz;
	AddPoint( &tmpv );


	return 0;
}


