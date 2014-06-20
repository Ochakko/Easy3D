#include <stdafx.h>
#include <windows.h>
#include <quaternion.h>
#include <math.h>

#include <quaternion2.h>

#include <matrix2.h>
#include <crtdbg.h>

#include <D3DX9.h>

#include <BoneProp.h>

CQuaternion::CQuaternion()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

CQuaternion::CQuaternion( float srcw, float srcx, float srcy, float srcz )
{
	w = srcw;
	x = srcx;
	y = srcy;
	z = srcz;
}
CQuaternion::~CQuaternion()
{

}

int CQuaternion::SetParams( float srcw, float srcx, float srcy, float srcz )
{
	w = srcw;
	x = srcx;
	y = srcy;
	z = srcz;
	return 0;
}

int CQuaternion::SetAxisAndRot( D3DXVECTOR3 srcaxis, float phai )
{
	float phai2;
	float cos_phai2, sin_phai2;

	phai2 = phai * 0.5f;
	cos_phai2 = cosf( phai2 );
	sin_phai2 = sinf( phai2 );

	w = cos_phai2;
	x = srcaxis.x * sin_phai2;
	y = srcaxis.y * sin_phai2;
	z = srcaxis.z * sin_phai2;

	return 0;
}
int CQuaternion::SetAxisAndRot( D3DXVECTOR3 srcaxis, double phai )
{
	double phai2;
	double cos_phai2, sin_phai2;

	phai2 = phai * 0.5;
	cos_phai2 = cos( phai2 );
	sin_phai2 = sin( phai2 );

	w = (float)cos_phai2;
	x = (float)( srcaxis.x * sin_phai2 );
	y = (float)( srcaxis.y * sin_phai2 );
	z = (float)( srcaxis.z * sin_phai2 );

	return 0;
}

int CQuaternion::GetAxisAndRot( D3DXVECTOR3* axisvecptr, float* frad )
{
	D3DXQUATERNION tempq;

	tempq.x = x;
	tempq.y = y;
	tempq.z = z;
	tempq.w = w;

	D3DXQuaternionToAxisAngle( &tempq, axisvecptr, frad );
	
	D3DXVec3Normalize( axisvecptr, axisvecptr );

	return 0;
}


int CQuaternion::SetRotation( float degx, float degy, float degz )
{
	// X軸、Y軸、Z軸の順番で、回転する、クォータニオンをセットする。

	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;
	float fDeg2Pai = (float)DEG2PAI;

	cosx = (float)cos( degx * 0.5f * fDeg2Pai );
	sinx = (float)sin( degx * 0.5f * fDeg2Pai );
	cosy = (float)cos( degy * 0.5f * fDeg2Pai );
	siny = (float)sin( degy * 0.5f * fDeg2Pai );
	cosz = (float)cos( degz * 0.5f * fDeg2Pai );
	sinz = (float)sin( degz * 0.5f * fDeg2Pai );

	qx.SetParams( cosx, sinx, 0.0f, 0.0f );
	qy.SetParams( cosy, 0.0f, siny, 0.0f );
	qz.SetParams( cosz, 0.0f, 0.0f, sinz );

	//q = qx * qy * qz;
	q = qz * qy * qx;//!!!!

	*this = q;

	return 0;
}

int CQuaternion::SetRotation( double degx, double degy, double degz )
{
	// X軸、Y軸、Z軸の順番で、回転する、クォータニオンをセットする。

	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;

	cosx = (float)cos( degx * 0.5 * DEG2PAI );
	sinx = (float)sin( degx * 0.5 * DEG2PAI );
	cosy = (float)cos( degy * 0.5 * DEG2PAI );
	siny = (float)sin( degy * 0.5 * DEG2PAI );
	cosz = (float)cos( degz * 0.5 * DEG2PAI );
	sinz = (float)sin( degz * 0.5 * DEG2PAI );

	qx.SetParams( cosx, sinx, 0.0f, 0.0f );
	qy.SetParams( cosy, 0.0f, siny, 0.0f );
	qz.SetParams( cosz, 0.0f, 0.0f, sinz );

	//q = qx * qy * qz;
	q = qz * qy * qx;//!!!!

	*this = q;

	return 0;
}




/***
int CQuaternion::SetRotationGlobal( float degx, float degy, float degz )
{

	CQuaternion curq, invcurq, qx, qy, qz;
	float radx, rady, radz;
	float fDeg2Pai = (float)DEG2PAI;

	D3DXVECTOR3 vecx0( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vecy0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vecz0( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 vecx, vecy, vecz;

	radx = degx * fDeg2Pai;
	rady = degy * fDeg2Pai;
	radz = degz * fDeg2Pai;

	qz.SetAxisAndRot( vecz0, radz );
	curq = qz;

	curq.inv( &invcurq );
	invcurq.normalize();
	invcurq.Rotate( &vecy, vecy0 );
	D3DXVec3Normalize( &vecy, &vecy );
	qy.SetAxisAndRot( vecy, rady );
	curq = curq * qy;

	curq.inv( &invcurq );
	invcurq.normalize();
	invcurq.Rotate( &vecx, vecx0 );
	D3DXVec3Normalize( &vecx, &vecx );
	qx.SetAxisAndRot( vecx, radx );
	curq = curq * qx;

	curq.normalize();

	//q = qz * qy * qx;//!!!!

	*this = curq;

	return 0;
}
***/

CQuaternion CQuaternion::operator* (float srcw) const { return CQuaternion(this->w * srcw, this->x * srcw, this->y * srcw, this->z * srcw); }
CQuaternion &CQuaternion::operator*= (float srcw) { *this = *this * srcw; return *this; }
CQuaternion CQuaternion::operator/ (float srcw) const { return CQuaternion(this->w / srcw, this->x / srcw, this->y / srcw, this->z / srcw); }
CQuaternion &CQuaternion::operator/= (float srcw) { *this = *this / srcw; return *this; }
CQuaternion CQuaternion::operator+ (const CQuaternion &q) const { return CQuaternion(w + q.w, x + q.x, y + q.y, z + q.z); }
CQuaternion &CQuaternion::operator+= (const CQuaternion &q) { *this = *this + q; return *this; }
CQuaternion CQuaternion::operator- (const CQuaternion &q) const { return CQuaternion(w - q.w, x - q.x, y - q.y, z - q.z); }
CQuaternion &CQuaternion::operator-= (const CQuaternion &q) { *this = *this - q; return *this; }
CQuaternion CQuaternion::operator* (const CQuaternion &q) const {
	return CQuaternion(
		w * q.w - x * q.x - y * q.y - z * q.z,
		w * q.x + q.w * x + y * q.z - z * q.y,
		w * q.y + q.w * y + z * q.x - x * q.z,
		w * q.z + q.w * z + x * q.y - y * q.x ).normalize();
}
CQuaternion &CQuaternion::operator*= (const CQuaternion &q) { *this = *this * q; return *this; }
CQuaternion CQuaternion::operator- () const { return *this * -1.0f; }
CQuaternion CQuaternion::normalize () const { 
	float mag = w*w+x*x+y*y+z*z;
	if( mag != 0.0f )
		return (*this)*(1.0f/(float)::sqrt(mag));
	else
		return CQuaternion( 1e6, 1e6, 1e6, 1e6 );
}

CMatrix2 CQuaternion::MakeRotMat()
{
	float dat00, dat01, dat02;
	float dat10, dat11, dat12;
	float dat20, dat21, dat22;

	dat00 = w * w + x * x - y * y - z * z;
	dat01 = 2.0f * ( x * y + w * z );
	dat02 = 2.0f * ( x * z - w * y );

	dat10 = 2.0f * ( x * y - w * z );
	dat11 = w * w - x * x + y * y - z * z;
	dat12 = 2.0f * ( y * z + w * x );

	dat20 = 2.0f * ( x * z + w * y );
	dat21 = 2.0f * ( y * z - w * x );
	dat22 = w * w - x * x - y * y + z * z;

	CMatrix2 retmat( 
		dat00, dat01, dat02, 0.0f,
		dat10, dat11, dat12, 0.0f,
		dat20, dat21, dat22, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );

	return retmat;
}

D3DXMATRIX CQuaternion::MakeRotMatX()
{
	float dat00, dat01, dat02;
	float dat10, dat11, dat12;
	float dat20, dat21, dat22;

	dat00 = w * w + x * x - y * y - z * z;
	dat01 = 2.0f * ( x * y + w * z );
	dat02 = 2.0f * ( x * z - w * y );

	dat10 = 2.0f * ( x * y - w * z );
	dat11 = w * w - x * x + y * y - z * z;
	dat12 = 2.0f * ( y * z + w * x );

	dat20 = 2.0f * ( x * z + w * y );
	dat21 = 2.0f * ( y * z - w * x );
	dat22 = w * w - x * x - y * y + z * z;

	D3DXMATRIX retmat(
		dat00, dat01, dat02, 0.0f,
		dat10, dat11, dat12, 0.0f,
		dat20, dat21, dat22, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );

	return retmat;
}


float CQuaternion::DotProduct( CQuaternion srcq )
{
	float dot;
	dot = w * srcq.w + 
		x * srcq.x + 
		y * srcq.y +
		z * srcq.z;
	return dot;
}

float CQuaternion::CalcRad( CQuaternion srcq )
{
	float dot, retrad;
	dot = this->DotProduct( srcq );

	//!!!!!!!!!!　注意　!!!!!!!!!!!!
	//!!!! dot が１より微妙に大きい値のとき、kakuには、無効な値(-1.#IN00)が入ってしまう。
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if( dot > 1.0f )
		dot = 1.0f;
	if( dot < -1.0f )
		dot = -1.0f;
	retrad = (float)acos( dot );

	return retrad;
}


CQuaternion CQuaternion::Slerp( CQuaternion endq, int framenum, int frameno )
{
	CQuaternion retq;
	retq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	float kaku;
	kaku = this->CalcRad( endq );

	if( kaku > (PI * 0.5f) ){
		//片方を-qにすれば、(PI * 0.5f)より小さくなる。（最短コースをたどれる）
		endq = -endq;
		kaku = this->CalcRad( endq );
		_ASSERT( kaku <= (PI * 0.5f) );
	}

	// sin( kaku ) == 0.0 付近を調整。
	//180度にはならないので（ならないようにするので）０度のみケア
	int kaku0flag = 0;
	if( (kaku <= 1e-4) && (kaku >= -1e-4) ){
		kaku0flag = 1;
//DbgOut( "Quaternion : Slerp : kaku0flag 1 : dot %f, kaku %f\n", dot, kaku );
	}


	float t = (float)frameno / (float)framenum;
	float alpha, beta;
	if( kaku0flag == 0 ){
		alpha = (float)sin( kaku * (1.0f - t) ) / (float)sin( kaku );
		beta = (float)sin( kaku * t ) / (float)sin( kaku );

		retq = *this * alpha + endq * beta;
	}else{
		retq = *this;
//DbgOut( "MotionInfo : FillUpMotionPoint 2: frame %d, %f, %f, %f, %f\n",
//	   frameno, startq.w, startq.x, startq.y, startq.z );
	}
		
	return retq;
}


int CQuaternion::Squad( CQuaternion q0, CQuaternion q1, CQuaternion q2, CQuaternion q3, float t )
{

	CQuaternion iq0, iq1, iq2, iq3;
	D3DXQUATERNION qx0, qx1, qx2, qx3;
	D3DXQUATERNION ax, bx, cx, resx;

	q0.inv( &iq0 );
	q1.inv( &iq1 );
	q2.inv( &iq2 );
	q3.inv( &iq3 );

	CQuaternion2X( &qx0, iq0 );
	CQuaternion2X( &qx1, iq1 );
	CQuaternion2X( &qx2, iq2 );
	CQuaternion2X( &qx3, iq3 );

	D3DXQuaternionSquadSetup( &ax, &bx, &cx, &qx0, &qx1, &qx2, &qx3 );
	D3DXQuaternionSquad( &resx, &qx1, &ax, &bx, &cx, t );

	D3DXQUATERNION iresx;
	D3DXQuaternionInverse( &iresx, &resx );

	this->x = iresx.x;
	this->y = iresx.y;
	this->z = iresx.z;
	this->w = iresx.w;


	return 0;
}


int CQuaternion::CQuaternion2X( D3DXQUATERNION* dstx )
{
	dstx->x = x;
	dstx->y = y;
	dstx->z = z;
	dstx->w = w;
	
	return 0;
}

int CQuaternion::CQuaternion2X( D3DXQUATERNION* dstx, CQuaternion srcq )
{
	dstx->x = srcq.x;
	dstx->y = srcq.y;
	dstx->z = srcq.z;
	dstx->w = srcq.w;
	
	return 0;
}

int CQuaternion::inv( CQuaternion* dstq ) 
{
	//return quaternion_class(w/(w*w+::dot_product(v(),v())), -v()/(w*w+::dot_product(v(),v()))); 

	float dot, mag;
	dot = x * x + y * y + z * z;
	mag = w * w + dot;
	
	if( mag != 0 ){
		dstq->w = w / mag;	
		dstq->x = -x / mag;
		dstq->y = -y / mag;
		dstq->z = -z / mag;
	}else{
		_ASSERT( 0 );
		dstq->w = 1.0f;
		dstq->x = 0.0f;
		dstq->y = 0.0f;
		dstq->z = 0.0f;
	}

	*dstq = dstq->normalize();

	return 0;
}

int CQuaternion::RotationArc( D3DXVECTOR3 srcvec0, D3DXVECTOR3 srcvec1 )
{
	//srcvec0, srcvec1は、normalizeされているとする。

	D3DXVECTOR3 c;
	D3DXVec3Cross( &c, &srcvec0, &srcvec1 );
	float d;
	d = D3DXVec3Dot( &srcvec0, &srcvec1 );
	float s;
	s = (float)sqrt( (1 + d) * 2.0f );

	x = c.x / s;
	y = c.y / s;
	z = c.z / s;
	w = s / 2.0f;

	return 0;
}


int CQuaternion::Rotate( D3DXVECTOR3* dstvec, D3DXVECTOR3 srcvec )
{
	CMatrix2 mat;

	mat = MakeRotMat();

	float xp, yp, zp, wp, invwp;

	xp = mat.data[0][0] * srcvec.x + mat.data[1][0] * srcvec.y + mat.data[2][0] * srcvec.z + mat.data[3][0];
	yp = mat.data[0][1] * srcvec.x + mat.data[1][1] * srcvec.y + mat.data[2][1] * srcvec.z + mat.data[3][1];
	zp = mat.data[0][2] * srcvec.x + mat.data[1][2] * srcvec.y + mat.data[2][2] * srcvec.z + mat.data[3][2];
	wp = mat.data[0][3] * srcvec.x + mat.data[1][3] * srcvec.y + mat.data[2][3] * srcvec.z + mat.data[3][3];

	if( wp != 0.0f ){
		invwp = 1.0f / wp;
	}else{
		_ASSERT( 0 );
		invwp = 0.0f;
	}

	dstvec->x = xp * invwp;
	dstvec->y = yp * invwp;
	dstvec->z = zp * invwp;
	
	return 0;
}
int CQuaternion::Rotate( DVEC3* dstvec, DVEC3 srcvec )
{
	CMatrix2 mat;

	mat = MakeRotMat();

	double xp, yp, zp, wp, invwp;

	xp = mat.data[0][0] * srcvec.x + mat.data[1][0] * srcvec.y + mat.data[2][0] * srcvec.z + mat.data[3][0];
	yp = mat.data[0][1] * srcvec.x + mat.data[1][1] * srcvec.y + mat.data[2][1] * srcvec.z + mat.data[3][1];
	zp = mat.data[0][2] * srcvec.x + mat.data[1][2] * srcvec.y + mat.data[2][2] * srcvec.z + mat.data[3][2];
	wp = mat.data[0][3] * srcvec.x + mat.data[1][3] * srcvec.y + mat.data[2][3] * srcvec.z + mat.data[3][3];

	if( wp != 0.0 ){
		invwp = 1.0 / wp;
	}else{
		_ASSERT( 0 );
		invwp = 0.0f;
	}

	dstvec->x = xp * invwp;
	dstvec->y = yp * invwp;
	dstvec->z = zp * invwp;
	
	return 0;
}


int CQuaternion::QuaternionToAxisAngle( D3DXVECTOR3* dstaxis, float* dstrad )
{
	D3DXQUATERNION xq;

	int ret;
	ret = CQuaternion2X( &xq );
	_ASSERT( !ret );

	D3DXQuaternionToAxisAngle( &xq, dstaxis, dstrad );

	return 0;
}

int CQuaternion::CopyFromCQuaternion2( CQuaternion2* q2 )
{
	w = q2->w;
	x = q2->x;
	y = q2->y;
	z = q2->z;

	return 0;
}

int CQuaternion::transpose( CQuaternion* dstq )
{
	CMatrix2 mat;

	mat = MakeRotMat();

	D3DXMATRIX matx;
	matx._11 = mat.data[0][0];
	matx._12 = mat.data[0][1];
	matx._13 = mat.data[0][2];
	matx._14 = mat.data[0][3];

	matx._21 = mat.data[1][0];
	matx._22 = mat.data[1][1];
	matx._23 = mat.data[1][2];
	matx._24 = mat.data[1][3];

	matx._31 = mat.data[2][0];
	matx._32 = mat.data[2][1];
	matx._33 = mat.data[2][2];
	matx._34 = mat.data[2][3];

	matx._41 = mat.data[3][0];
	matx._42 = mat.data[3][1];
	matx._43 = mat.data[3][2];
	matx._44 = mat.data[3][3];

	D3DXMATRIX tmatx;
	D3DXMatrixTranspose( &tmatx, &matx );

	D3DXQUATERNION qx;
	D3DXQuaternionRotationMatrix( &qx, &tmatx );

	dstq->x = qx.x;	
	dstq->y = qx.y;	
	dstq->z = qx.z;	
	dstq->w = qx.w;	

/***
	CQuaternion invq;
	inv( &invq );

	D3DXQUATERNION qx;
	qx.x = x;
	qx.y = y;
	qx.z = z;
	qx.w = w;

	D3DXMATRIX rotmat;
	D3DXMatrixRotationQuaternion( &rotmat, &qx );

	D3DXMATRIX transmat;
	D3DXMatrixTranspose( &transmat, &rotmat );

	D3DXQUATERNION transqx;
	D3DXQuaternionRotationMatrix( &transqx, &transmat );

	dstq->x = -transqx.x;	
	dstq->y = -transqx.y;	
	dstq->z = -transqx.z;	
	dstq->w = transqx.w;	
***/
	return 0;
}

CQuaternion CQuaternion::CalcFBXEul( CQuaternion befq, D3DXVECTOR3 befeul, D3DXVECTOR3* reteul )
{


	D3DXMATRIX rightmat;
	rightmat = MakeRotMatX();

	rightmat._31 *= -1;
	rightmat._32 *= -1;
	rightmat._34 *= -1;
	rightmat._13 *= -1;
	rightmat._23 *= -1;
	rightmat._43 *= -1;

	D3DXQUATERNION rqx;
	D3DXQuaternionRotationMatrix( &rqx, &rightmat );


	CQuaternion rq;
	rq.x = rqx.x;
	rq.y = rqx.y;
	rq.z = rqx.z;
	rq.w = rqx.w;

/*
	CQuaternion rq;
	rq = *this;
*/
/*
	CQuaternion rq;
	inv( &rq );
*/
/***
	float kaku;
	kaku = befq.CalcRad( rq );
	if( kaku > (PI * 0.5f) ){
		rq = -(rq);
	}

	CQuaternion rq;
	rq.SetParams( -w, x, y, -z );
***/

	CQuaternion axisq;
	axisq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
	qToEulerAxis( axisq, &rq, reteul );
	modifyEuler( reteul, &befeul );
//	rq.Q2Eul2( 0, befeul, reteul );

	return rq;
}

int CQuaternion::Q2Eul2( CQuaternion* axisq, D3DXVECTOR3 befeul, D3DXVECTOR3* reteul )
{

	CQuaternion axisQ, invaxisQ, EQ;
	if( axisq ){
		axisQ = *axisq;
		axisQ.inv( &invaxisQ );
		EQ = invaxisQ * *this * axisQ;
	}else{
		axisQ.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		invaxisQ.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		EQ = *this;
	}

	D3DXVECTOR3 Euler = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );


	D3DXVECTOR3 axisXVec( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisYVec( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZVec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 basevec( 1.0f, 1.0f, 1.0f );

	D3DXVECTOR3 targetVec, shadowVec;
	D3DXVECTOR3 tmpVec;


	EQ.Rotate( &targetVec, axisYVec );
	//EQ.Rotate( &targetVec, basevec );
	shadowVec.x = vecDotVec( &targetVec, &axisXVec );
	shadowVec.y = vecDotVec( &targetVec, &axisYVec );
	shadowVec.z = 0.0f;
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler.z = 90.0f;
	}else{
		Euler.z = aCos( vecDotVec( &shadowVec, &axisYVec ) / lengthVec( &shadowVec ) );		
	}
	if( vecDotVec( &shadowVec, &axisXVec ) > 0.0f ){
		Euler.z = -Euler.z;
	}


	EQ.Rotate( &targetVec, axisZVec );
	vec3RotateZ( &tmpVec, -Euler.z, &targetVec );
	shadowVec.x = vecDotVec( &tmpVec, &axisXVec );
	shadowVec.y = 0.0f;
	shadowVec.z = vecDotVec( &tmpVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler.y = 90.0f;
	}else{
		Euler.y = aCos( vecDotVec( &shadowVec, &axisZVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisXVec ) < 0.0f ){
		Euler.y = -Euler.y;
	}


	EQ.Rotate( &targetVec, axisYVec );
	vec3RotateZ( &tmpVec, -Euler.z, &targetVec );
	targetVec = tmpVec;
	vec3RotateY( &tmpVec, -Euler.y, &targetVec );
	shadowVec.x = 0.0f;
	shadowVec.y = vecDotVec( &tmpVec, &axisYVec );
	shadowVec.z = vecDotVec( &tmpVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler.x = 90.0f;
	}else{
		Euler.x = aCos( vecDotVec( &shadowVec, &axisYVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisYVec ) < 0.0f ){
		Euler.x = -Euler.x;
	}

	ModifyEuler2( &Euler, &befeul );
	*reteul = Euler;

	return 0;
}

int CQuaternion::ModifyEuler2( D3DXVECTOR3* eulerA, D3DXVECTOR3* eulerB )
{

	//オイラー角Aの値をオイラー角Bの値に近い表示に修正
	float tmpX1, tmpY1, tmpZ1;
	float tmpX2, tmpY2, tmpZ2;
	float s1, s2;

	//予想される角度1
	tmpX1 = eulerA->x + 360.0f * (GetRound( (eulerB->x - eulerA->x) / 360.0f ));
	tmpY1 = eulerA->y + 360.0f * (GetRound( (eulerB->y - eulerA->y) / 360.0f ));
	tmpZ1 = eulerA->z + 360.0f * (GetRound( (eulerB->z - eulerA->z) / 360.0f ));

	//予想される角度2
	tmpX2 = 180.0f - eulerA->x + 360.0f * GetRound( (eulerB->x + eulerA->x - 180.0f) / 360.0f );
	tmpY2 = eulerA->y + 180.0f + 360.0f * GetRound( (eulerB->y - eulerA->y - 180.0f) / 360.0f );
	tmpZ2 = eulerA->z + 180.0f + 360.0f * GetRound( (eulerB->z - eulerA->z - 180.0f) / 360.0f );

//	tmpX2 = eulerA->x + 360.0f * (GetRound( (-eulerB->x + eulerA->x) / 360.0f ));
//	tmpY2 = eulerA->y + 360.0f * (GetRound( (-eulerB->y + eulerA->y) / 360.0f ));
//	tmpZ2 = eulerA->z + 360.0f * (GetRound( (-eulerB->z + eulerA->z) / 360.0f ));

	//角度変化の大きさ
	s1 = (eulerB->x - tmpX1) * (eulerB->x - tmpX1) + (eulerB->y - tmpY1) * (eulerB->y - tmpY1) + (eulerB->z - tmpZ1) * (eulerB->z - tmpZ1);
	s2 = (eulerB->x - tmpX2) * (eulerB->x - tmpX2) + (eulerB->y - tmpY2) * (eulerB->y - tmpY2) + (eulerB->z - tmpZ2) * (eulerB->z - tmpZ2);

	//変化の少ない方に修正
	if( s1 < s2 ){
		eulerA->x = tmpX1; eulerA->y = tmpY1; eulerA->z = tmpZ1;
	}else{
		eulerA->x = tmpX2; eulerA->y = tmpY2; eulerA->z = tmpZ2;
	}

	return 0;
}
