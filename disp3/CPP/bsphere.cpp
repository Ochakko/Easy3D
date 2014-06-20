#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <bsphere.h>
#include <BBox.h>

#define	DBGH
#include <dbg.h>

// extern
extern int g_useGPU;




CBSphere::CBSphere()
{
	InitParams();

}

CBSphere::~CBSphere()
{

}


int CBSphere::InitParams()
{
	befcenter.x = 0.0f; befcenter.y = 0.0f; befcenter.z = 0.0f;//bonematのみの変換後の中心
	tracenter.x = 0.0f; tracenter.y = 0.0f; tracenter.z = 0.0f;//befcenter * worldMat の中心
	rmag = 0.0f; //パーツの中心からの半径。
	//totalrmag = 0.0f; //モデル全体の中心からの半径の２乗。
	visibleflag = 0; //視野内か外かの判定結果フラグ

	return 0;
}

int CBSphere::SetBSphere( CBBox* srcbbx )
{

	tracenter.x = ( srcbbx->minx + srcbbx->maxx ) * 0.5f;
	tracenter.y = ( srcbbx->miny + srcbbx->maxy ) * 0.5f;
	tracenter.z = ( srcbbx->minz + srcbbx->maxz ) * 0.5f;


	float tmpmag;

	tmpmag = ( tracenter.x - srcbbx->maxx ) * ( tracenter.x - srcbbx->maxx )
		+ ( tracenter.y - srcbbx->maxy ) * ( tracenter.y - srcbbx->maxy )
		+ ( tracenter.z - srcbbx->maxz ) * ( tracenter.z - srcbbx->maxz );

	if( tmpmag != 0.0f ){
		rmag = sqrtf( tmpmag );
	}else{
		rmag = 0.0f;
	}

	return 0;
}


int CBSphere::Transform2ScreenPos( DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int* scxptr, int* scyptr, int outflag )
{

	// bonemat * matWorld の変換後の頂点を、スクリーン座標に変換する。


	float width = (float)dwClipWidth * 2.0f;
	float height = (float)dwClipHeight * 2.0f;

	float flClipWidth = (float)dwClipWidth;
	float flClipHeight = (float)dwClipHeight;

	float projfar, divprojfar;
	projfar = -matProj._43 / ( matProj._33 - 1.0f );
	divprojfar = -( matProj._33 - 1.0f ) / matProj._43;

	D3DXMATRIX mat;
	mat = matView;// !!!!!!!!!!!!!!!!!!!!!!! matWorldは、既にかかっているので、注意！！！！！！！！！！！

	float x = tracenter.x;
	float y = tracenter.y;
	float z = tracenter.z;

	float xp, yp, zp;//, wp;

	xp = mat._11*x + mat._21*y + mat._31*z + mat._41;
	yp = mat._12*x + mat._22*y + mat._32*z + mat._42;
	zp = mat._13*x + mat._23*y + mat._33*z + mat._43;
				

	float xp2, yp2, zp2, wp2;

	//xp2 = xp * matProj._11;
	//yp2 = yp * matProj._22;
	//zp2 = zp * matProj._33 + matProj._43;

//	if( matProj._43 < -1.0f ){ 
//		xp2 = xp * matProj._11;
//		yp2 = yp * matProj._22;
//		zp2 = zp * matProj._33 + matProj._43;
//		wp2 = zp;
//	}else{
		//正射影

		xp2 = matProj._11*xp + matProj._21*yp + matProj._31*zp + matProj._41;
		yp2 = matProj._12*xp + matProj._22*yp + matProj._32*zp + matProj._42;
		zp2 = matProj._13*xp + matProj._23*yp + matProj._33*zp + matProj._43;
		wp2 = matProj._14*xp + matProj._24*yp + matProj._34*zp + matProj._44;
//	}



	D3DTLVERTEX dsttlv;

	ConvScreenPos( PROJ_NORMAL, xp2, yp2, zp2, wp2, flClipHeight, flClipWidth, aspect, &dsttlv );

	if( outflag == 0 ){
		if( (dsttlv.sz > 0.0f) && (dsttlv.sz < 1.0f) ){
			*scxptr = (int)dsttlv.sx;
			*scyptr = (int)dsttlv.sy;
		}else{
			*scxptr = 0xFFFFFFFF;
			*scyptr = 0xFFFFFFFF;
		}
	}else{
		//画面外でも数値を格納
		*scxptr = (int)dsttlv.sx;
		*scyptr = (int)dsttlv.sy;
	}

	return 0;
}

int CBSphere::Transform2ScreenPos( DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int* scxptr, int* scyptr, float* sczptr, int outflag )
{

	// bonemat * matWorld の変換後の頂点を、スクリーン座標に変換する。


	float width = (float)dwClipWidth * 2.0f;
	float height = (float)dwClipHeight * 2.0f;

	float flClipWidth = (float)dwClipWidth;
	float flClipHeight = (float)dwClipHeight;

	float projfar, divprojfar;
	projfar = -matProj._43 / ( matProj._33 - 1.0f );
	divprojfar = -( matProj._33 - 1.0f ) / matProj._43;

	D3DXMATRIX mat;
	mat = matView;// !!!!!!!!!!!!!!!!!!!!!!! matWorldは、既にかかっているので、注意！！！！！！！！！！！

	float x = tracenter.x;
	float y = tracenter.y;
	float z = tracenter.z;

	float xp, yp, zp;//, wp;

	xp = mat._11*x + mat._21*y + mat._31*z + mat._41;
	yp = mat._12*x + mat._22*y + mat._32*z + mat._42;
	zp = mat._13*x + mat._23*y + mat._33*z + mat._43;
				

	float xp2, yp2, zp2, wp2;

	//xp2 = xp * matProj._11;
	//yp2 = yp * matProj._22;
	//zp2 = zp * matProj._33 + matProj._43;

//	if( matProj._43 < -1.0f ){ 
//		xp2 = xp * matProj._11;
//		yp2 = yp * matProj._22;
//		zp2 = zp * matProj._33 + matProj._43;
//		wp2 = zp;
//	}else{
		//正射影

		xp2 = matProj._11*xp + matProj._21*yp + matProj._31*zp + matProj._41;
		yp2 = matProj._12*xp + matProj._22*yp + matProj._32*zp + matProj._42;
		zp2 = matProj._13*xp + matProj._23*yp + matProj._33*zp + matProj._43;
		wp2 = matProj._14*xp + matProj._24*yp + matProj._34*zp + matProj._44;
//	}



	D3DTLVERTEX dsttlv;

	ConvScreenPos( PROJ_NORMAL, xp2, yp2, zp2, wp2, flClipHeight, flClipWidth, aspect, &dsttlv );

	if( outflag == 0 ){
		if( (dsttlv.sz > 0.0f) && (dsttlv.sz < 1.0f) ){
			*scxptr = (int)dsttlv.sx;
			*scyptr = (int)dsttlv.sy;
			*sczptr = dsttlv.sz;
		}else{
			*scxptr = 0xFFFFFFFF;
			*scyptr = 0xFFFFFFFF;
			*sczptr = 0.0f;
		}
	}else{
		*scxptr = (int)dsttlv.sx;
		*scyptr = (int)dsttlv.sy;
		*sczptr = dsttlv.sz;
	}

	return 0;
}

int CBSphere::Transform2ScreenPos( DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, D3DXVECTOR3* scpos )
{

	// bonemat * matWorld の変換後の頂点を、スクリーン座標に変換する。


	float width = (float)dwClipWidth * 2.0f;
	float height = (float)dwClipHeight * 2.0f;

	float flClipWidth = (float)dwClipWidth;
	float flClipHeight = (float)dwClipHeight;

	float projfar, divprojfar;
	projfar = -matProj._43 / ( matProj._33 - 1.0f );
	divprojfar = -( matProj._33 - 1.0f ) / matProj._43;

	D3DXMATRIX mat;
	mat = matView;// !!!!!!!!!!!!!!!!!!!!!!! matWorldは、既にかかっているので、注意！！！！！！！！！！！

	float x = tracenter.x;
	float y = tracenter.y;
	float z = tracenter.z;

	float xp, yp, zp;//, wp;

	xp = mat._11*x + mat._21*y + mat._31*z + mat._41;
	yp = mat._12*x + mat._22*y + mat._32*z + mat._42;
	zp = mat._13*x + mat._23*y + mat._33*z + mat._43;
				

	float xp2, yp2, zp2, wp2;
	xp2 = matProj._11*xp + matProj._21*yp + matProj._31*zp + matProj._41;
	yp2 = matProj._12*xp + matProj._22*yp + matProj._32*zp + matProj._42;
	zp2 = matProj._13*xp + matProj._23*yp + matProj._33*zp + matProj._43;
	wp2 = matProj._14*xp + matProj._24*yp + matProj._34*zp + matProj._44;


	D3DTLVERTEX dsttlv;
	ConvScreenPos( PROJ_NORMAL, xp2, yp2, zp2, wp2, flClipHeight, flClipWidth, aspect, &dsttlv );

	scpos->x = dsttlv.sx;
	scpos->y = dsttlv.sy;
	scpos->z = dsttlv.sz;

	return 0;
}

int CBSphere::ConvScreenPos( int projmode, float xp, float yp, float zp, float wp, float h, float w, float aspect, D3DTLVERTEX* dsttlv )
{
	float invwp;

	if( wp != 0.0f )
		invwp = 1.0f / wp;
	else
		invwp = 1e6;
	float xp2, yp2;
	xp2 = xp * invwp;
	yp2 = yp * invwp;

		
	if( projmode == PROJ_NORMAL ){
		dsttlv->sx  = ( 1.0f + xp2 ) * aspect * h + w - aspect * h;
		dsttlv->sy  = ( 1.0f - yp2 ) * h;
		dsttlv->sz  = zp * invwp;
		//dsttlv->rhw = wp;
		dsttlv->rhw = invwp;
	}else{
		float dx, dy;
		float deg, scale;

		dx = (float)fabs( xp2 );
		dy = (float)fabs( yp2 );

		deg = sqrtf( dx * dx + dy * dy );
		if( deg > PAIDIV2 )
			deg = (float)PAIDIV2;

		scale = 0.8f + 0.5f / tanf( deg );
						
		//dsttlv->sx  = ( 1.0f + xp2 * scale ) * dwClipWidth;
		dsttlv->sx  = ( 1.0f + xp2 * scale ) * aspect * h + w - aspect * h;
		dsttlv->sy  = ( 1.0f - yp2 * scale ) * h;
		dsttlv->sz  = zp * invwp;
		//dsttlv->rhw = wp;
		dsttlv->rhw = invwp;
	}

	return 0;

}

int CBSphere::ChkInView( FRUSTUMINFO* frinfo )
{

	int ret;
	D3DXVECTOR3 foot;
	D3DXVECTOR3 foot2center;
	D3DXVECTOR3 n;
	int planeno;
	int visiblecnt = 0;
	for( planeno = 0; planeno < 6; planeno++ ){
		
		// ベクトルチェック
		ret = GetFootOnPlane( &foot, &tracenter, &frinfo->planeFrustum[planeno] );
		if( ret ){
			DbgOut( "BSphere : ChkInView : GetFootOnPlane error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		foot2center = tracenter - foot;
		n.x = frinfo->planeFrustum[planeno].a;
		n.y = frinfo->planeFrustum[planeno].b;
		n.z = frinfo->planeFrustum[planeno].c;

		float dot;
		dot = D3DXVec3Dot( &n, &foot2center );
		
		//距離チェック
		float a0, a1;
		a0 = (float)fabs( n.x * tracenter.x + n.y * tracenter.y + n.z * tracenter.z + frinfo->planeFrustum[planeno].d );
		a1 = sqrtf( n.x * n.x + n.y * n.y + n.z * n.z );

		float dist;
		if( a1 != 0.0f ){
			dist = a0 / a1;
		}else{
			dist = 0.0f;
		}

		if( (dot < 0.0f) && (dist > rmag) ){
			//視野外
			break;//!!!!!!!!!
		}else{
			visiblecnt++;
		}
	}

	if( visiblecnt == 6 ){
		visibleflag = 1;
	}else{
		visibleflag = 0;
	}

	//visibleflag = 1;

	return 0;
}



int CBSphere::ChkInView( FRUSTUMINFO* frinfo, D3DXMATRIX matWorld )
{


////world変換。
	float x = befcenter.x;
	float y = befcenter.y;
	float z = befcenter.z;

	float xp, yp, zp;//, wp;

	xp = matWorld._11*x + matWorld._21*y + matWorld._31*z + matWorld._41;
	yp = matWorld._12*x + matWorld._22*y + matWorld._32*z + matWorld._42;
	zp = matWorld._13*x + matWorld._23*y + matWorld._33*z + matWorld._43;
	//wp = matWorld._14*x + matWorld._24*y + matWorld._34*z + matWorld._44;

	tracenter.x = xp;
	tracenter.y = yp;
	tracenter.z = zp;

/////
	int ret;
	D3DXVECTOR3 foot;
	D3DXVECTOR3 foot2center;
	D3DXVECTOR3 n;
	int planeno;
	int visiblecnt = 0;
	for( planeno = 0; planeno < 6; planeno++ ){
		
		// ベクトルチェック
		ret = GetFootOnPlane( &foot, &tracenter, &frinfo->planeFrustum[planeno] );
		if( ret ){
			DbgOut( "BSphere : ChkInView : GetFootOnPlane error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		foot2center = tracenter - foot;
		n.x = frinfo->planeFrustum[planeno].a;
		n.y = frinfo->planeFrustum[planeno].b;
		n.z = frinfo->planeFrustum[planeno].c;

		float dot;
		dot = D3DXVec3Dot( &n, &foot2center );
		
		//距離チェック
		float a0, a1;
		a0 = (float)fabs( n.x * tracenter.x + n.y * tracenter.y + n.z * tracenter.z + frinfo->planeFrustum[planeno].d );
		a1 = sqrtf( n.x * n.x + n.y * n.y + n.z * n.z );

		float dist;
		if( a1 != 0.0f ){
			dist = a0 / a1;
		}else{
			dist = 0.0f;
		}

		if( (dot < 0.0f) && (dist > rmag) ){
			//視野外
			break;//!!!!!!!!!
		}else{
			visiblecnt++;
		}
	}

	if( visiblecnt == 6 ){
		visibleflag = 1;
	}else{
		visibleflag = 0;
	}

	return 0;
}

int CBSphere::GetFootOnPlane( D3DXVECTOR3* pfoot, D3DXVECTOR3* ppos, D3DXPLANE* pplane )
{
	float t0, t1, t;
	t0 = pplane->a * ppos->x + pplane->b * ppos->y + pplane->c * ppos->z + pplane->d;
	t1 = sqrtf( pplane->a * pplane->a + pplane->b * pplane->b + pplane->c * pplane->c );

	if( t1 != 0.0f ){
		t = -( t0 / t1 );
	}else{
		t = 0;
	}

	pfoot->x = ppos->x + t * pplane->a;
	pfoot->y = ppos->y + t * pplane->b;
	pfoot->z = ppos->z + t * pplane->c;
	
	return 0;
}


/***
int CBSphere::ChkInView( D3DXMATRIX matWorld, D3DXVECTOR3 viewpos, D3DXVECTOR3 viewvec, float projnear, float projfar, float* fovindex )
{

////world変換。
	float x = befcenter.x;
	float y = befcenter.y;
	float z = befcenter.z;

	float xp, yp, zp;//, wp;

	xp = matWorld._11*x + matWorld._21*y + matWorld._31*z + matWorld._41;
	yp = matWorld._12*x + matWorld._22*y + matWorld._32*z + matWorld._42;
	zp = matWorld._13*x + matWorld._23*y + matWorld._33*z + matWorld._43;
	//wp = matWorld._14*x + matWorld._24*y + matWorld._34*z + matWorld._44;

	tracenter.x = xp;
	tracenter.y = yp;
	tracenter.z = zp;

////////
	//距離のチェック

	D3DXVECTOR3 view2center;
	view2center.x = tracenter.x - viewpos.x;
	view2center.y = tracenter.y - viewpos.y;
	view2center.z = tracenter.z - viewpos.z;

	float zdist;
	zdist = D3DXVec3Dot( &viewvec, &view2center );
	if( zdist < 0.0f )
		zdist *= -1.0f;


	if( (zdist + rmag) < projnear ){
		visibleflag = 0;
		return 0;
	}else if( (zdist > rmag) && ((zdist - rmag) > projfar) ){
		visibleflag = 0;
		return 0;
	}


	//float v2cdist;
	//v2cdist = sqrtf( view2center.x * view2center.x + view2center.y * view2center.y + view2center.z * view2center.z );
	//if( v2cdist < projnear ){
	//	visibleflag = 0;
	//	return 0;
	//}else if( v2cdist > projfar ){
	//	visibleflag = 0;
	//	return 0;
	//}

////////
	float diststep;
	diststep = (projfar - projnear) / (float)FOVINDEXSIZE;

	int distindex;
	distindex = (int)( (zdist - projnear) / diststep );
	if( distindex > FOVINDEXSIZE )
		distindex = FOVINDEXSIZE;


	//角度のチェック
	float fov2;
	D3DXVECTOR3 vback;
	float backdist;

	fov2 = fovindex[ distindex ] * 0.5f;
	backdist = rmag / sinf( fov2 );
	vback.x = viewpos.x - viewvec.x * backdist;
	vback.y = viewpos.y - viewvec.y * backdist;
	vback.z = viewpos.z - viewvec.z * backdist;


	D3DXVECTOR3 vb2c;
	vb2c.x = tracenter.x - vback.x;
	vb2c.y = tracenter.y - vback.y;
	vb2c.z = tracenter.z - vback.z;

	D3DXVECTOR3 vb2cvec;
	D3DXVec3Normalize( &vb2cvec, &vb2c );

	float cosfov2;

	cosfov2 = cosf( fov2 );

	float dot;
	dot = viewvec.x * vb2cvec.x + viewvec.y * vb2cvec.y + viewvec.z * vb2cvec.z;

	if( dot < cosfov2 ){
		//視野外
		visibleflag = 0;
		return 0;
	}else{
		visibleflag = 1;
		return 0;
	}
///////


	return 0;
}
***/


int CBSphere::SetBSFromVEC3F( VEC3F* srcv, int srcvnum )
{
	int vno;
	D3DXVECTOR3 totalv( 0.0f, 0.0f, 0.0f );

	for( vno = 0; vno < srcvnum; vno++ ){
		totalv.x += (srcv + vno)->x;
		totalv.y += (srcv + vno)->y;
		totalv.z += (srcv + vno)->z;
	}

	befcenter = totalv / (float)srcvnum;

	float tmpmag;
	float maxmag = 0.0f;
	for( vno = 0; vno < srcvnum; vno++ ){
		tmpmag = ( befcenter.x - (srcv + vno)->x ) * ( befcenter.x - (srcv + vno)->x ) 
			+ ( befcenter.y - (srcv + vno)->y ) * ( befcenter.y - (srcv + vno)->y )
			+ ( befcenter.z - (srcv + vno)->z ) * ( befcenter.z - (srcv + vno)->z );

		if( tmpmag > maxmag ){
			maxmag = tmpmag;
		}
	}

	if( maxmag != 0.0f ){
		rmag = sqrtf( maxmag );
	}else{
		rmag = 0.0f;
	}

	return 0;

}


int CBSphere::SetBSFrom3V( D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3 )
{

	//!!!!!!!!!!!!!!!!
	// tracenter, befcenter に同じものを入れておく。
	// tracenterのみ使用すると仮定
	//!!!!!!!!!!!!!!!!


	//centerを求める
	tracenter.x = (v1->x + v2->x + v3->x) / 3.0f;
	tracenter.y = (v1->y + v2->y + v3->y) / 3.0f;
	tracenter.z = (v1->z + v2->z + v3->z) / 3.0f;

	befcenter = tracenter;//!!!!!


	//半径を求める
	float mag1, mag2, mag3;
	float diffx, diffy, diffz;

	//
	diffx = tracenter.x - v1->x;
	diffy = tracenter.y - v1->y;
	diffz = tracenter.z - v1->z;

	mag1 = diffx * diffx + diffy * diffy + diffz * diffz;
	//
	diffx = tracenter.x - v2->x;
	diffy = tracenter.y - v2->y;
	diffz = tracenter.z - v2->z;

	mag2 = diffx * diffx + diffy * diffy + diffz * diffz;
	//
	diffx = tracenter.x - v3->x;
	diffy = tracenter.y - v3->y;
	diffz = tracenter.z - v3->z;

	mag3 = diffx * diffx + diffy * diffy + diffz * diffz;
	//

	float maxmag;
	maxmag = max( mag1, max( mag2, mag3 ) );

	rmag = sqrtf( maxmag );

	return 0;
}

int CBSphere::ChkConfMoveSphere( D3DXVECTOR3 Q1, D3DXVECTOR3 Q2, float rq, float srcrate )
{
	//初期状態でぶつかっているかチェック
	D3DXVECTOR3 diff0;
	diff0 = Q1 - tracenter;
	float mag = diff0.x * diff0.x + diff0.y * diff0.y + diff0.z * diff0.z;
	float maxmag = ( rq + rmag * srcrate ) * ( rq + rmag * srcrate );
	if( mag <= maxmag ){
		return 1;//!!!!!!!!!!!!!!!!!!!!
	}

	//移動してぶつかるかチェック　３Dグラフィックス数学p222
	D3DXVECTOR3 vp( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vq;
	vq = Q2 - Q1;

	D3DXVECTOR3 a, b;
	a = tracenter - Q1;
	b = vp - vq;
	float B2;
	B2 = b.x * b.x + b.y * b.y + b.z * b.z;
	if( B2 == 0.0f ){
		return 0;//!!!!!!!
	}
	float A2;
	A2 = a.x * a.x + a.y * a.y + a.z * a.z;

	float AB;
	AB = D3DXVec3Dot( &a, &b );
	

	float root;
	root = AB * AB - B2 * ( A2 - (rq + rmag * srcrate) * (rq + rmag * srcrate) );

	if( root < 0.0f ){
		return 0;
	}

	float t1;
	if( root != 0.0f ){
		t1 = (-AB - (float)sqrt( root )) / B2;
	}else{
		t1 = -AB / B2;
	}
		
	if( (t1 >= 0.0) && (t1 <= 1.0f) ){
		return 1;
	}else{
		return 0;
	}
}
