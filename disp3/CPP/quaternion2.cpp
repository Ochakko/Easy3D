#include <stdafx.h>
#include <windows.h>
#include <quaternion2.h>
#include <math.h>

#include <matrix2.h>
#include <crtdbg.h>

#include <D3DX9.h>

#define	DBGH
#include <dbg.h>

//#define CHECKPRINT 1
//#define CHECKPRINT 0


static CQuaternion2* s_tempq = 0;


int CQuaternion2::InitTempQ()
{
	if( !s_tempq ){
		s_tempq = new CQuaternion2[ 10 ];
		if( !s_tempq ){
			DbgOut( "q2 : InitTempQ : s_tempq alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CQuaternion2::DestroyTempQ()
{
	if( s_tempq ){
		delete [] s_tempq;
		s_tempq = 0;
	}

	return 0;
}



CQuaternion2::CQuaternion2()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	twist = 0.0f;
}

CQuaternion2::CQuaternion2( float srcw, float srcx, float srcy, float srcz, float srctwist )
{
	w = srcw;
	x = srcx;
	y = srcy;
	z = srcz;
	twist = srctwist;
}
CQuaternion2::~CQuaternion2()
{
	
}

int CQuaternion2::SetParams( float srcw, float srcx, float srcy, float srcz, float srctwist )
{
	w = srcw;
	x = srcx;
	y = srcy;
	z = srcz;
	twist = srctwist;
	return 0;
}

int CQuaternion2::SetAxisAndRot( D3DXVECTOR3 srcaxis, float phai )
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

int CQuaternion2::GetAxisAndRot( D3DXVECTOR3* axisvecptr, float* frad )
{
	D3DXQUATERNION tempq;

	tempq.x = x;
	tempq.y = y;
	tempq.z = z;
	tempq.w = w;

	D3DXQuaternionToAxisAngle( &tempq, axisvecptr, frad );
	
	D3DXVECTOR3 zerovec( 0.0f, 0.0f, 0.0f );
	if( *axisvecptr != zerovec )
		D3DXVec3Normalize( axisvecptr, axisvecptr );

	return 0;
}


int CQuaternion2::SetRotation( float degx, float degy, float degz )
{
	// X軸、Y軸、Z軸の順番で、回転する、クォータニオンをセットする。

	CQuaternion2 q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;
	float fDeg2Pai = (float)DEG2PAI;

	cosx = (float)cos( degx * 0.5f * fDeg2Pai );
	sinx = (float)sin( degx * 0.5f * fDeg2Pai );
	cosy = (float)cos( degy * 0.5f * fDeg2Pai );
	siny = (float)sin( degy * 0.5f * fDeg2Pai );
	cosz = (float)cos( degz * 0.5f * fDeg2Pai );
	sinz = (float)sin( degz * 0.5f * fDeg2Pai );

	qx.SetParams( cosx, sinx, 0.0f, 0.0f, twist );
	qy.SetParams( cosy, 0.0f, siny, 0.0f, twist );
	qz.SetParams( cosz, 0.0f, 0.0f, sinz, twist );

	//q = qx * qy * qz;
	q = qz * qy * qx;//!!!!

	float savetwist = twist;
	*this = q;
	twist = savetwist;

	return 0;
}
/***
int CQuaternion2::SetRotationGlobal( float degx, float degy, float degz )
{

	CQuaternion2 curq, invcurq, qx, qy, qz;
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

CQuaternion2 CQuaternion2::operator* (float srcw) const { 
	return CQuaternion2(this->w * srcw, this->x * srcw, this->y * srcw, this->z * srcw, twist); 
}
CQuaternion2 &CQuaternion2::operator*= (float srcw) { 
	float savetwist = twist;
	*this = *this * srcw; 
	twist = savetwist;
	return *this; 
}
CQuaternion2 CQuaternion2::operator/ (float srcw) const { 
	return CQuaternion2(this->w / srcw, this->x / srcw, this->y / srcw, this->z / srcw, twist); 
}
CQuaternion2 &CQuaternion2::operator/= (float srcw) { 
	float savetwist = twist;
	*this = *this / srcw; 
	twist = savetwist;
	return *this; 
}
CQuaternion2 CQuaternion2::operator+ (const CQuaternion2 &q) const { 
	return CQuaternion2(w + q.w, x + q.x, y + q.y, z + q.z, twist); 
}
CQuaternion2 &CQuaternion2::operator+= (const CQuaternion2 &q) { 
	float savetwist = twist;
	*this = *this + q; 
	twist = savetwist;
	return *this; 
}
CQuaternion2 CQuaternion2::operator- (const CQuaternion2 &q) const { 
	return CQuaternion2(w - q.w, x - q.x, y - q.y, z - q.z, twist); 
}
CQuaternion2 &CQuaternion2::operator-= (const CQuaternion2 &q) { 
	float savetwist = twist;
	*this = *this - q; 
	twist = savetwist;
	return *this; 
}
CQuaternion2 CQuaternion2::operator* (const CQuaternion2 &q) const {
	//return CQuaternion2(
	//	w * q.w - x * q.x - y * q.y - z * q.z,
	//	w * q.x + q.w * x + y * q.z - z * q.y,
	//	w * q.y + q.w * y + z * q.x - x * q.z,
	//	w * q.z + q.w * z + x * q.y - y * q.x, twist ).normalize();
	CQuaternion2 tempq(
		w * q.w - x * q.x - y * q.y - z * q.z,
		w * q.x + q.w * x + y * q.z - z * q.y,
		w * q.y + q.w * y + z * q.x - x * q.z,
		w * q.z + q.w * z + x * q.y - y * q.x, twist );
	tempq.normalize();
	tempq.twist = twist;
	return tempq;
}
CQuaternion2 &CQuaternion2::operator*= (const CQuaternion2 &q) { 
	float savetwist = twist;
	*this = *this * q; 
	twist = savetwist;
	return *this; 
}
CQuaternion2 CQuaternion2::operator- () const {
	CQuaternion2 tempq;
	tempq = *this * -1.0f;
	tempq.twist = twist;
	return tempq; 
}
CQuaternion2 CQuaternion2::normalize () const { 
	//float mag = w*w+x*x+y*y+z*z;
	//if( mag != 0.0f )
	//	return (*this)*(1.0f/(float)::sqrt(mag));
	//else
	//	return CQuaternion2( 1e6, 1e6, 1e6, 1e6 );
	float mag = w*w+x*x+y*y+z*z;
	if( mag != 0.0f ){
		CQuaternion2 tempq;
		tempq = (*this) * (1.0f/(float)::sqrt(mag)); 
		tempq.twist = twist;
		return tempq;
	}else{
		DbgOut( "quaternion2 : normalize : zero warning !!!\n" );
		return CQuaternion2( 1.0f, 0.0f, 0.0f, 0.0f, twist );
	}
}

//CQuaternion2 CQuaternion2::operator= (const CQuaternion &q)
//{
//	return CQuaternion2( q.w, q.x, q.y, q.z, 0.0f );
//}
//CQuaternion CQuaternion2::operator= (const CQuaternion2 &q2 )
//{
//	return CQuaternion( q2.w, q2.x, q2.y, q2.z );
//}

CMatrix2 CQuaternion2::MakeRotMat()
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

D3DXMATRIX CQuaternion2::MakeRotMatX()
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


float CQuaternion2::DotProduct( CQuaternion2 srcq )
{
	float dot;
	dot = w * srcq.w + 
		x * srcq.x + 
		y * srcq.y +
		z * srcq.z;
	return dot;
}

float CQuaternion2::CalcRad( CQuaternion2 srcq )
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

	if( dot >= 0.999999619f ){
		retrad = 0.0f;
	}else if( dot <= -0.999999619f ){
		retrad = (float)PI;
	}else{

		retrad = (float)acos( dot );

	}

	//retrad = (float)acos( dot );

	return retrad;
}

CQuaternion2 CQuaternion2::Slerp( CQuaternion2 endq, float t )
{
	CQuaternion2 retq;	
	
	retq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );

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
	}else if( kaku == (float)PI ){
		kaku0flag = 2;
	}

	float alpha, beta;
	if( kaku0flag == 0 ){
		alpha = (float)sin( kaku * (1.0f - t) ) / (float)sin( kaku );
		beta = (float)sin( kaku * t ) / (float)sin( kaku );

		retq = *this * alpha + endq * beta;
	}else if( kaku0flag == 1 ){
		retq = *this;
//DbgOut( "MotionInfo : FillUpMotionPoint 2: frame %d, %f, %f, %f, %f\n",
//	   frameno, startq.w, startq.x, startq.y, startq.z );
	}else{
		retq.x = ( 1.0f - t ) * x + t * endq.x;
		retq.y = ( 1.0f - t ) * y + t * endq.y;
		retq.z = ( 1.0f - t ) * z + t * endq.z;
		retq.w = ( 1.0f - t ) * w + t * endq.w;
		retq.normalize();
	}

	
//twist
	twist = twist * (1.0f - t) + endq.twist * t;

	return retq;
}



CQuaternion2 CQuaternion2::Slerp( CQuaternion2 endq, int framenum, int frameno )
{
	CQuaternion2 retq;	
	
	retq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );

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
	}else if( kaku == (float)PI ){
		kaku0flag = 2;
	}


	float t = (float)frameno / (float)framenum;
	float alpha, beta;
	if( kaku0flag == 0 ){
		alpha = (float)sin( kaku * (1.0f - t) ) / (float)sin( kaku );
		beta = (float)sin( kaku * t ) / (float)sin( kaku );

		retq = *this * alpha + endq * beta;
	}else if( kaku0flag == 1 ){
		retq = *this;
//DbgOut( "MotionInfo : FillUpMotionPoint 2: frame %d, %f, %f, %f, %f\n",
//	   frameno, startq.w, startq.x, startq.y, startq.z );
	}else{
		retq.x = ( 1.0f - t ) * x + t * endq.x;
		retq.y = ( 1.0f - t ) * y + t * endq.y;
		retq.z = ( 1.0f - t ) * z + t * endq.z;
		retq.w = ( 1.0f - t ) * w + t * endq.w;
		retq.normalize();
	}

	
//twist
	twist = twist * (1.0f - t) + endq.twist * t;

	return retq;
}


int CQuaternion2::Squad( CQuaternion2 q0, CQuaternion2 q1, CQuaternion2 q2, CQuaternion2 q3, float t )
{
	CQuaternion2 iq0, iq1, iq2, iq3;
	D3DXQUATERNION qx0, qx1, qx2, qx3;
	D3DXQUATERNION ax, bx, cx, resx;

	q0.inv( &iq0 );
	q1.inv( &iq1 );
	q2.inv( &iq2 );
	q3.inv( &iq3 );

	CQuaternion22X( &qx0, iq0 );
	CQuaternion22X( &qx1, iq1 );
	CQuaternion22X( &qx2, iq2 );
	CQuaternion22X( &qx3, iq3 );

	D3DXQuaternionSquadSetup( &ax, &bx, &cx, &qx0, &qx1, &qx2, &qx3 );
	D3DXQuaternionSquad( &resx, &qx1, &ax, &bx, &cx, t );

	D3DXQUATERNION iresx;
	D3DXQuaternionInverse( &iresx, &resx );

	this->x = iresx.x;
	this->y = iresx.y;
	this->z = iresx.z;
	this->w = iresx.w;

//twist
	twist = q0.twist * ( 1.0f - t ) + q3.twist * t;

	return 0;
}


int CQuaternion2::CQuaternion22X( D3DXQUATERNION* dstx )
{
	dstx->x = x;
	dstx->y = y;
	dstx->z = z;
	dstx->w = w;
	
	return 0;
}

int CQuaternion2::CQuaternion22X( D3DXQUATERNION* dstx, CQuaternion2 srcq )
{
	dstx->x = srcq.x;
	dstx->y = srcq.y;
	dstx->z = srcq.z;
	dstx->w = srcq.w;
	
	return 0;
}

int CQuaternion2::inv( CQuaternion2* dstq ) 
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
		dstq->w = 0.0f;
		dstq->x = 0.0f;
		dstq->y = 0.0f;
		dstq->z = 0.0f;
	}

	*dstq = dstq->normalize();

	dstq->twist = -twist;//!!!!!!!!!!
	
	return 0;
}

int CQuaternion2::RotationArc( D3DXVECTOR3 srcvec0, D3DXVECTOR3 srcvec1 )
{
	//srcvec0, srcvec1は、normalizeされているとする。
	
	D3DXVECTOR3 c;
	DXVec3Cross( &c, &srcvec0, &srcvec1 );
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


int CQuaternion2::Rotate( D3DXVECTOR3* dstvec, D3DXVECTOR3 srcvec )
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


int CQuaternion2::QuaternionToAxisAngle( D3DXVECTOR3* dstaxis, float* dstrad )
{
	D3DXQUATERNION xq;

	int ret;
	ret = CQuaternion22X( &xq );
	_ASSERT( !ret );

	D3DXQuaternionToAxisAngle( &xq, dstaxis, dstrad );

	return 0;
}

int CQuaternion2::CopyFromCQuaternion( CQuaternion* srcq )
{
	w = srcq->w;
	x = srcq->x;
	y = srcq->y;
	z = srcq->z;
	twist = 0.0f;

	return 0;
}


int CQuaternion2::DXVec3Cross( D3DXVECTOR3* dstvec, D3DXVECTOR3* pV1, D3DXVECTOR3* pV2 )
{
    D3DXVECTOR3 v;
 
    v.x = pV1->y * pV2->z - pV1->z * pV2->y;
    v.y = pV1->z * pV2->x - pV1->x * pV2->z;
    v.z = pV1->x * pV2->y - pV1->y * pV2->x;
 
    *dstvec = v;

	return 0;
}

int CQuaternion2::LookAt( D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, float fmindeg, int upflag, 
	int gradyflag, CQuaternion2* finalq, D3DXVECTOR3* dstup, CQuaternion2* dstrot1, CQuaternion2* dstrot2 )

{
	int ret;
	int curupflag;
	float savetwist;
	savetwist = twist;//!!!!!!!!!!!!
	
	DXVec3Normalize( &tarvec, &tarvec );

	
	if( (upflag == 0) || (upflag == 1) ){

		D3DXVECTOR3 upvec;
		//D3DXVECTOR3 tempup( 0.0f, 1.0f, 0.0f );


		if( upflag == 0 ) {
			upvec.x = 0.0f;
			upvec.y = 1.0f;
			upvec.z = 0.0f;
		}else if( upflag == 1 ){
			upvec.x = 0.0f;
			upvec.y = -1.0f;
			upvec.z = 0.0f;
		}else{
			_ASSERT( 0 );
			upvec.x = 0.0f;
			upvec.y = 1.0f;
			upvec.z = 0.0f;		
		}

		ret = LookAtQ( s_tempq, *this, tarvec, upvec, basevec, fmindeg, 1, gradyflag, finalq, dstrot1, dstrot2 );
		if( ret ){
			DbgOut( "quaternion2 : LookAt : LookAtQ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*this = *s_tempq;
		*dstup = upvec;


		curupflag = upflag;

	}else if( upflag == 2 ){
		
		D3DXVECTOR3 plusup( 0.0f, 1.0f, 0.0f );
		D3DXVECTOR3 minusup( 0.0f, -1.0f, 0.0f );

		
		ret = LookAtQ( s_tempq, *this, tarvec, plusup, basevec, fmindeg, 1, gradyflag, finalq, s_tempq + 1, s_tempq + 2 );
		if( ret ){
			DbgOut( "quaternion2 : LookAt : LookAtQ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LookAtQ( s_tempq + 5, *this, tarvec, minusup, basevec, fmindeg, 1, gradyflag, finalq, s_tempq + 6, s_tempq + 7 );
		if( ret ){
			DbgOut( "quaternion2 : LookAt : LookAtQ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		

		D3DXVECTOR3 testaxis;
		float chkup;
		chkup = D3DXVec3Dot( &plusup, &basevec );
		if( chkup > 0.939f ){
			testaxis.x = 1.0f;
			testaxis.y = 0.0f;
			testaxis.z = 0.0f;

//#ifdef CHECKPRINT
//	DbgOut( "q2 : LookAt : testaxis X !!!\n" );
//#endif

		}else if( chkup < -0.939f ){

			testaxis.x = -1.0f;
			testaxis.y = 0.0f;
			testaxis.z = 0.0f;

		}else{
			testaxis.x = 0.0f;
			testaxis.y = 1.0f;
			testaxis.z = 0.0f;
		}


		D3DXVECTOR3 resvec0, resvec1, resvec2;
		
		Rotate( &resvec0, testaxis );


		s_tempq->Rotate( &resvec1, testaxis );
		(s_tempq + 5)->Rotate( &resvec2, testaxis );

		DXVec3Normalize( &resvec0, &resvec0 );
		DXVec3Normalize( &resvec1, &resvec1 );
		DXVec3Normalize( &resvec2, &resvec2 );

		float dot1, dot2;
		dot1 = D3DXVec3Dot( &resvec0, &resvec1 );
		dot2 = D3DXVec3Dot( &resvec0, &resvec2 );


//#ifdef CHECKPRINT
//					if( (dot1 * dot2) >= 0.0f ){
//						DbgOut( " : quaternion2 : LookAt : same dir warning !!! %f %f\n", dot1, dot2 );

//						DbgOut( " : quaternion2 : LookAt : resvec0 %f %f %f\n", resvec0.x, resvec0.y, resvec0.z );
//						DbgOut( " : quaternion2 : LookAt : resvec1 %f %f %f\n", resvec1.x, resvec1.y, resvec1.z );
//						DbgOut( " : quaternion2 : LookAt : resvec2 %f %f %f\n", resvec2.x, resvec2.y, resvec2.z );

//						DbgOut( " : quaternion2 : LookAt : dot1 %f dot2 %f\n", dot1, dot2 );
//					}else{
						//DbgOut( " : quaternion2 : LookAt : different dir\n" );
//					}
//#endif

		if( dot1 > dot2 ){
			*this = *s_tempq;
			curupflag = 0;

			*dstup = plusup;
			*dstrot1 = *(s_tempq + 1);
			*dstrot2 = *(s_tempq + 2);
//#ifdef CHECKPRINT
//			if( dot1 < 0.0f ){
//				DbgOut( "q2 : LookAt : select minus dot warning !!! %f %f\n", dot1, dot2 );
//			}
//#endif		
		}else{
			*this = *(s_tempq + 5);
			curupflag = 1;

			*dstup = minusup;
			*dstrot1 = *(s_tempq + 6);
			*dstrot2 = *(s_tempq + 7);

//#ifdef CHECKPRINT
//			if( dot2 < 0.0f ){
//				DbgOut( "q2 : LookAt : select minus dot warning !!! %f %f\n", dot1, dot2 );
//			}
//#endif		
		}
		
	}else{

		D3DXVECTOR3 upvec;

		upvec.x = 0.0f;
		upvec.y = 1.0f;
		upvec.z = 0.0f;

		ret = LookAtQ( s_tempq, *this, tarvec, upvec, basevec, fmindeg, 0, gradyflag, finalq, s_tempq + 1, s_tempq + 2 );
		if( ret ){
			DbgOut( "quaternion2 : LookAt : LookAtQ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*this = *s_tempq;

		*dstup = upvec;//!!! このモードでは意味なし
		*dstrot1 = *(s_tempq + 1);
		*dstrot2 = *(s_tempq + 2);

		curupflag = upflag;
	}


	normalize();
	twist = savetwist;//!!!!!!!!!!!!


	return 0;
}

//static
int CQuaternion2::GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir )
{

		//planepを通り、planedirを法線ベクトルとする平面：ax + by + cz + d = 0;
	float a, b, c, d;
	a = planedir.x; b = planedir.y; c = planedir.z;
	d = -a * planep.x - b * planep.y - c * planep.z;

		//平面ax+by+cz+d=0と、点pobj1を通り方向ベクトルdirectionの直線との交点、targetを求める。
		//OpenGL赤本p402参照
	D3DXMATRIX mat;
	mat._11 = b * srcdir.y + c * srcdir.z;
	mat._12 = -a * srcdir.y;
	mat._13 = -a * srcdir.z;
	mat._14 = 0.0f;

	mat._21 = -b * srcdir.x;
	mat._22 = a * srcdir.x + c * srcdir.z;
	mat._23 = -b * srcdir.z;
	mat._24 = 0.0f;

	mat._31 = -c * srcdir.x;
	mat._32 = -c * srcdir.y;
	mat._33 = a * srcdir.x + b * srcdir.y;
	mat._34 = 0.0f;

	mat._41 = -d * srcdir.x;
	mat._42 = -d * srcdir.y;
	mat._43 = -d * srcdir.z;
	mat._44 = a * srcdir.x + b * srcdir.y + c * srcdir.z;

	if( mat._44 == 0.0f )
		return 1;

	dstshadow->x = (srcp.x * mat._11 + srcp.y * mat._21 + srcp.z * mat._31 + mat._41) / mat._44;
	dstshadow->y = (srcp.x * mat._12 + srcp.y * mat._22 + srcp.z * mat._32 + mat._42) / mat._44;
	dstshadow->z = (srcp.x * mat._13 + srcp.y * mat._23 + srcp.z * mat._33 + mat._43) / mat._44;


	return 0;
}

int CQuaternion2::LookAtQ( CQuaternion2* resqptr, CQuaternion2 befq, D3DXVECTOR3 tarvec, D3DXVECTOR3 upvec, D3DXVECTOR3 basevec, 
	float fmindeg, int keepupflag, int gradyflag, CQuaternion2* finalq, CQuaternion2* dstrot1, CQuaternion2* dstrot2 )
{
	D3DXVECTOR3 zerovec( 0.0f, 0.0f, 0.0f );	
	int ret;

	int zeroaxisflag = 0;

	//D3DXVECTOR3 basevec( 0.0f, 0.0f, -1.0f );

	DXVec3Normalize( &tarvec, &tarvec );
	if( (tarvec.x == 0.0f) && (tarvec.y == 0.0f) && (tarvec.z == 0.0f) ){
		//DbgOut( "q2 : LookAtQ : tarvec 0 warning return !!!\n" );
		//dstqptr->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		*resqptr = befq;

		CQuaternion2 initq( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );
		*dstrot1 = initq;
		*dstrot2 = initq;
		return 0;
	}

	
	DXVec3Normalize( &basevec, &basevec );	
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}


	// basevecがＹ軸に近いときは、ずらす
	CQuaternion2 basecheckq;
	basecheckq.RotationArc( basevec, upvec );
	D3DXVECTOR3 chkbaseaxis;
	float chkbaserad;
	basecheckq.GetAxisAndRot( &chkbaseaxis, &chkbaserad );
	if( fabs( chkbaserad ) < 0.1f * (float)DEG2PAI ){
		D3DXVECTOR3 chgaxis( 1.0f, 0.0f, 0.0f );
		CQuaternion2 chgq;
		chgq.SetAxisAndRot( chgaxis, 0.1f * (float)DEG2PAI );
		chgq.Rotate( &basevec, basevec );

		DXVec3Normalize( &basevec, &basevec );
	}
	
	
	/***
	// tarvecがＹ軸に近いときは、ずらす
	CQuaternion2 tarcheckq;
	tarcheckq.RotationArc( tarvec, upvec );
	D3DXVECTOR3 chktaraxis;
	float chktarrad;
	tarcheckq.GetAxisAndRot( &chktaraxis, &chktarrad );
	if( fabs( chktarrad ) < 0.1f * (float)DEG2PAI ){
		D3DXVECTOR3 chgaxis( 1.0f, 0.0f, 0.0f );
		CQuaternion2 chgq;
		chgq.SetAxisAndRot( chgaxis, 0.1f * (float)DEG2PAI );
		chgq.Rotate( &tarvec, tarvec );

		DXVec3Normalize( &tarvec, &tarvec );
	}
	***/



	D3DXVECTOR3 vec;

	CQuaternion2 q_rot0;
	CQuaternion2 q_rot1;
	CQuaternion2 q_rot2;
	CQuaternion2 q_rot3;

	D3DXVECTOR3 axis;



	D3DXVECTOR3 trabase;
	befq.Rotate( &trabase, basevec );
	DXVec3Normalize( &trabase, &trabase );



	float rad;
	CQuaternion2 diffq;
	diffq.RotationArc( trabase, tarvec );
	diffq.GetAxisAndRot( &axis, &rad );

	/***
	// targetが遠い場合は、近くに再設定する。
	if( rad > (10.0f * (float)DEG2PAI) ){
		CQuaternion2 chgq;
		if( rad <= (float)PAI ){
			chgq.SetAxisAndRot( axis, 10.0f * (float)DEG2PAI );
		}else{
			chgq.SetAxisAndRot( axis, -10.0f * (float)DEG2PAI );
		}
		chgq.Rotate( &tarvec, trabase );
		DXVec3Normalize( &tarvec, &tarvec );

		diffq.RotationArc( trabase, tarvec );
		diffq.GetAxisAndRot( &axis, &rad );

	}
	***/

	if( (rad != 0.0f) && (axis != zerovec) ){
		if( fmindeg != 10000.0f ){
			if( rad <= (float)PAI ){
				rad = fmindeg * (float)DEG2PAI;
			}else{
				rad = -fmindeg * (float)DEG2PAI;
			}
		}else{
			//radのまま
		}		
	}else{

		//rad は、0 or PAI
		zeroaxisflag = 1;

		ret = Get180Axis( tarvec, &axis );
		if( ret ){
			DbgOut( "q2 : LookAtQ : Get180Axis error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		float dot;
		dot = D3DXVec3Dot( &tarvec, &trabase );
		if( dot < 0.0f ){
			rad = (float)PAI;
		}else{
			rad = 0.0f;
		}
		
		if( (fmindeg != 10000.0f) && (rad != 0.0f) ){
			rad = fmindeg * (float)DEG2PAI;
		}else{
			//そのまま 0
		}


//DbgOut( "!!! check !!! : q2 : LookAtQ : axis %f, %f, %f---rad %f\r\n", axis.x, axis.y, axis.z, rad );

	}

//DbgOut( "q2 : LookAtQ : 0 : axis %f %f %f, rad %f\n", axis.x, axis.y, axis.z, rad );	

	/***
	if( (axis.x != 0.0f) || (axis.y != 0.0f) || (axis.z != 0.0f) ){
		if( fmindeg != 10000.0f ){
			if( rad <= (float)PAI ){
				rad = fmindeg * (float)DEG2PAI;
			}else{
				rad = -fmindeg * (float)DEG2PAI;
			}
		}else{
			//radのまま
		}
	}else{
		zeroaxisflag = 1;

		ret = Get180Axis( tarvec, &axis );
		if( ret ){
			DbgOut( "q2 : LookAtQ : Get180Axis error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		if( fmindeg != 10000.0f ){
			if( rad <= (float)PAI ){
				rad = fmindeg * (float)DEG2PAI;
			}else{
				rad = -fmindeg * (float)DEG2PAI;
			}
		}else{
			//radのまま
		}

	}
	***/


	CQuaternion2 qstep1;
	qstep1.SetAxisAndRot( axis, rad );
	q_rot1 = qstep1 * befq;


	int addflag = 0;

	
//Ｙ軸に平行な姿勢を避ける
	// 0.1 -->0.9999996
	// 0.2 -->0.9999939
	// 0.03 --> 0.99999986292216422770342144067523

	D3DXVECTOR3 chkvec;
	q_rot1.Rotate( &chkvec, basevec );

	DXVec3Normalize( &chkvec, &chkvec );
	float chkdot0;
	chkdot0 = D3DXVec3Dot( &upvec, &chkvec );
	//if( (chkdot0 >= 0.9999999f) || (chkdot0 <= -0.9999999f) ){
	//if( (chkdot0 >= 0.9999939f) || (chkdot0 <= -0.9999939f) ){
	if( (chkdot0 >= 0.999999619f) || (chkdot0 <= -0.999999619f) ){
	//if( (chkdot0 >= 0.9999998f) || (chkdot0 <= -0.9999998f) ){
		if( (axis.x != 0.0f) || (axis.y != 0.0f) || (axis.z != 0.0f) ){
			if( rad >= 0.0f ){
				rad += 0.1f * (float)DEG2PAI;
			}else{
				rad -= 0.1f * (float)DEG2PAI;
			}


			CQuaternion2 addrot;
			addrot.SetAxisAndRot( axis, rad );


			CQuaternion2 newqrot1;
			newqrot1 = addrot * befq;
		

			D3DXVECTOR3 secondcheck;
			newqrot1.Rotate( &secondcheck, basevec );
			DXVec3Normalize( &secondcheck, &secondcheck );
			float seconddot;
			seconddot = D3DXVec3Dot( &secondcheck, &upvec );
			//if( fabs( seconddot ) >= 0.9999939f ){
				//rad += 0.2f * (float)DEG2PAI;
			if( fabs( seconddot ) >= 0.999999619f ){
				if( rad >= 0.0f ){
					rad += 0.1f * (float)DEG2PAI;
				}else{
					rad -= 0.1f * (float)DEG2PAI;
				}
			//if( fabs( seconddot ) >= 0.9999998f ){
			//	rad += 0.03f * (float)DEG2PAI;
				addrot.SetAxisAndRot( axis, rad );
				newqrot1 = addrot * befq;
			}

			q_rot1 = newqrot1;
		}

//#ifdef CHECKPRINT
//	DbgOut( "q2 : LookAtQ : addflag 1 : rad %f, rad2deg %f\n", rad, rad * (float)PAI2DEG );


//	if( zeroaxisflag == 1 ){
//		DbgOut( "q2 : LookAtQ : zeroaxisflag == 1 : axis %f %f %f, rad %f\n", axis.x, axis.y, axis.z, rad );
//	}
//#endif

//{
//	D3DXVECTOR3 shiftvec;
//	q_rot1.Rotate( &shiftvec, basevec );
//	float shiftcheck;
//	shiftcheck = D3DXVec3Dot( &shiftvec, &upvec );
//	if( fabs( shiftcheck ) >= 0.9999939f ){
//		DbgOut( "q2 : LookAtQ : shiftcheck warning !!!! %f, rad(deg) %f\n", shiftcheck, rad * (float)PAI2DEG );	
//		DbgOut( "axis %f %f %f\n", axis.x, axis.y, axis.z );	
//	}
//}


		addflag = 1;
	}
	
	
	//q_rot1回転で、ターゲットに近づかない回転は、キャンセルする。
	//(Ｙ軸からずらした関係で、そのままだと、振動することがある)

	/***
	D3DXVECTOR3 aftrot1;
	q_rot1.Rotate( &aftrot1, basevec );
	DXVec3Normalize( &aftrot1, &aftrot1 );

	D3DXVECTOR3 tar2trabase, tar2aftrot;
	tar2trabase = trabase - tarvec;
	tar2aftrot = aftrot1 - tarvec;
	
	float dircheck;
	dircheck = D3DXVec3Dot( &tar2trabase, &tar2aftrot );
	if( dircheck < 0.0f ){
		*resqptr = befq;
		// *dstrot1, *dstrot2 は不定。使用しないようにする！！！！！！
		return 0;
	}
	***/


	D3DXVECTOR3 aftrot1;
	q_rot1.Rotate( &aftrot1, basevec );
	DXVec3Normalize( &aftrot1, &aftrot1 );

	float aftroty;
	aftroty = D3DXVec3Dot( &aftrot1, &upvec );
	float aftcheck, befcheck;
	aftcheck = D3DXVec3Dot( &aftrot1, &tarvec );
	befcheck = D3DXVec3Dot( &trabase, &tarvec );

	float diffcheck;
	diffcheck = aftcheck - befcheck;
	//if( diffcheck < -0.0001f ){//振動
	//if( diffcheck < -0.00001f ){//振動
	//if( diffcheck <= -0.0000001f ){
	//if( diffcheck <= -0.0000002f ){
	//	q_rot1 = befq;
	//}

	if( befcheck > aftcheck ){

		//q_rot1 = befq;

		*resqptr = befq;
		// *dstrot1, *dstrot2 は不定。使用しないようにする！！！！！！
		return 0;

		//q_rot1 = befq;
		// *dstrot1, *dstrot2 は不定。使用しないようにする！！！！！！

	}


///////////////////////////
// upvec を考慮した回転を行う

	D3DXVECTOR3 curdir;
	q_rot1.Rotate( &curdir, basevec );
	DXVec3Normalize( &curdir, &curdir );


	float chkdot1;
	chkdot1 = D3DXVec3Dot( &curdir, &upvec );
	if( chkdot1 > 1.0f )
		chkdot1 = 1.0f;
	if( chkdot1 < -1.0f )
		chkdot1 = -1.0f;


	if( (keepupflag == 1) ){
		DXVec3Normalize( &upvec, &upvec );
		D3DXVECTOR3 tempup( 0.0f, 1.0f, 0.0f );

		
		D3DXVECTOR3 sdwup, sdwtempup;

		//float chkdot1;
		//chkdot1 = D3DXVec3Dot( &tarvec, &upvec );
		//if( (chkdot1 != 1.0f) || (chkdot1 != -1.0f) ){



		// 微笑なup方向の移動を避けるため、わざと、ずらしてしまう
		//CQuaternion2 kozaikuq;
		//kozaikuq.SetAxisAndRot( curdir, 30.0f * (float)DEG2PAI );
		//q_rot1 = kozaikuq * q_rot1;


		//q_rot1.Rotate( &curdir, basevec );
		//DXVec3Normalize( &curdir, &curdir );



	//////////////


		//if( (chkdot1 < 0.99999999f) && (chkdot1 > -0.99999999f) ){
		//if( (chkdot1 < 1.0f) && (chkdot1 > -1.0f) ){
		//if( (curdir.x != 0.0f) || (curdir.z != 0.0f) ){
		
		if( (chkdot1 < 0.99999999f) && (chkdot1 > -0.99999999f) ){
			q_rot1.Rotate( &tempup, tempup );
			
			ret = GetShadow( &sdwup, curdir, zerovec, upvec, curdir );
			if( ret ){
				DbgOut( "q2 : LookAtQ : GetShadow error 3!!!\n" );
				_ASSERT( 0 );
				return 1;
			}		
			ret = GetShadow( &sdwtempup, curdir, zerovec, tempup, curdir );
			if( ret ){
				DbgOut( "q2 : LookAtQ : GetShadow error 4!!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( befq.twist != 0.0f ){
				CQuaternion2 twistq;
				twistq.SetAxisAndRot( curdir, befq.twist * (float)DEG2PAI );
				twistq.Rotate( &sdwup, sdwup );
			}

			DXVec3Normalize( &sdwup, &sdwup );
			DXVec3Normalize( &sdwtempup, &sdwtempup );

//#ifdef CHECKPRINT

//	if( (sdwup.x == 0.0f) && (sdwup.y == 0.0f) && (sdwup.z == 0.0f) ){
//								DbgOut( "q2 : LookAtQ : sdwup zero pat0 warning !!!\n" );
//								DbgOut( "curdir %f %f %f\n", curdir.x, curdir.y, curdir.z );
//								DbgOut( "upvec %f %f %f\n", upvec.x, upvec.y, upvec.z );
//								DbgOut( "chkdot1 %f\n", chkdot1 );
//	}

//	if( (sdwtempup.x == 0.0f) && (sdwtempup.y == 0.0f) && (sdwtempup.z == 0.0f) ){
//								DbgOut( "q2 : LookAtQ : sdwtempup zero pat0 warning !!!\n" );
//	}

//	if( addflag == 1 ){
//								DbgOut( "\n" );
//								DbgOut( "q2 : LookAtQ : addflag == 1 : sdwup %f %f %f\n", sdwup.x, sdwup.y, sdwup.z );
//								DbgOut( "q2 : LookAtQ : addflag == 1 : sdwtempup %f %f %f\n", sdwtempup.x, sdwtempup.y, sdwtempup.z );
//	}
//#endif

		}else{

//#ifdef CHECKPRINT
//	DbgOut( "q2 : LookAtQ : curdir para y warning !!!\n" );
	//_ASSERT( 0 );
//#endif		

			befq.Rotate( &sdwup, upvec );
			q_rot1.Rotate( &sdwtempup, tempup );

			ret = GetShadow( &sdwup, curdir, zerovec, sdwup, curdir );
			if( ret ){
				DbgOut( "q2 : LookAtQ : GetShadow error 3!!!\n" );
				_ASSERT( 0 );
				return 1;
			}		
			ret = GetShadow( &sdwtempup, curdir, zerovec, sdwtempup, curdir );
			if( ret ){
				DbgOut( "q2 : LookAtQ : GetShadow error 4!!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//float difftwist;
			//difftwist = ftwist - befq.twist;
			//CQuaternion2 twistq;
			//twistq.SetAxisAndRot( curdir, difftwist * (float)DEG2PAI );
			//twistq.Rotate( &sdwup, sdwup );


			//DbgOut( "e3dhsp : LookAt : sdwup %f %f %f\n", sdwup.x, sdwup.y, sdwup.z );
			//DbgOut( "e3dhsp : LookAt : sdwtempup %f %f %f\n", sdwtempup.x, sdwtempup.y, sdwtempup.z );

			DXVec3Normalize( &sdwup, &sdwup );
			DXVec3Normalize( &sdwtempup, &sdwtempup );

//#ifdef CHECKPRINT			
//	if( (sdwup.x == 0.0f) && (sdwup.y == 0.0f) && (sdwup.z == 0.0f) ){
//							DbgOut( "q2 : LookAtQ : sdwup zero pat1 warning !!!\n" );
//	}

//	if( (sdwtempup.x == 0.0f) && (sdwtempup.y == 0.0f) && (sdwtempup.z == 0.0f) ){
//							DbgOut( "q2 : LookAtQ : sdwtempup zero pat1 warning !!!\n" );
//	}
//#endif
		}
		
			
		{
			int rotarcflag2 = 0;
			float testdot0up, testdot1up;
			D3DXVECTOR3 testvec0up, testvec1up;
			D3DXVECTOR3 axisup;
			CQuaternion2 qup;

			float dotup, radup;
			dotup = D3DXVec3Dot( &sdwtempup, &sdwup );
			if( dotup > 1.0f )
				dotup = 1.0f;
			else if( dotup < -1.0f )
				dotup = -1.0f;

			/***
			qup.RotationArc( sdwtempup, sdwup );
			qup.GetAxisAndRot( &axisup, &radup );
			rotarcflag2 = 1;

			if( (radup == 0.0f) && (dotup < 0.0f) ){
				//180度
				radup = (float)PAI;
				rotarcflag2 = 0;
			
				qup.SetAxisAndRot( axisup, radup );
				q_rot2 = qup;

			}else{
				q_rot2 = qup;
			}
			***/

			
			if( dotup > 0.0f ){
				qup.RotationArc( sdwtempup, sdwup );
				qup.GetAxisAndRot( &axisup, &radup );
				rotarcflag2 = 1;
			}else{
				radup = (float)acos( dotup );
			}

			if( (radup != 0.0f) && ((curdir.x != 0.0f) || (curdir.y != 0.0f) || (curdir.z != 0.0f))){
				if( rotarcflag2 == 1 ){
					q_rot2 = qup;
				}else{
					CQuaternion2 q0up, q1up;
					q0up.SetAxisAndRot( curdir, radup );
					q1up.SetAxisAndRot( curdir, -radup );


					q0up.Rotate( &testvec0up, sdwtempup );
					q1up.Rotate( &testvec1up, sdwtempup );

					testdot0up = D3DXVec3Dot( &sdwup, &testvec0up );//!!!!!!!!!
					testdot1up = D3DXVec3Dot( &sdwup, &testvec1up );//!!!!!!!!!

					if( testdot0up >= testdot1up )
						q_rot2 = q0up;
					else
						q_rot2 = q1up;
				}
			}else{

//#ifdef CHECKPRINT
				//DbgOut( "q2 : LookAtQ : q_rot2 init !!!!!! curdir %f %f %f\n", curdir.x, curdir.y, curdir.z );
//#endif

				q_rot2.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );
			}
		}

		
		// 上向きベクトルを徐々に動かすように、戻す方向に回転する。
		//D3DXVECTOR3 rot3axis;

		if( gradyflag == 1 ){
			float rot3rad;
			
			CQuaternion2 curq;
			curq = q_rot2 * q_rot1;

			D3DXVECTOR3 curdir2;
			curq.Rotate( &curdir2, basevec );
			DXVec3Normalize( &curdir2, &curdir2 );


			D3DXVECTOR3 befup4, curup4;
			befq.Rotate( &befup4, upvec );
			curq.Rotate( &curup4, upvec );

			DXVec3Normalize( &befup4, &befup4 );
			DXVec3Normalize( &curup4, &curup4 );

			D3DXVECTOR3 sdwbefup4, sdwcurup4;
			ret = GetShadow( &sdwbefup4, curdir2, zerovec, befup4, curdir2 );
			if( ret ){
				DbgOut( "q2 : LookAtQ : GetShadow error 3!!!\n" );
				_ASSERT( 0 );
				return 1;
			}		
			ret = GetShadow( &sdwcurup4, curdir2, zerovec, curup4, curdir2 );
			if( ret ){
				DbgOut( "q2 : LookAtQ : GetShadow error 3!!!\n" );
				_ASSERT( 0 );
				return 1;
			}		


			CQuaternion2 invupq;
			D3DXVECTOR3 invupaxis;
			float invuprad;
			
			float invdot;
			invdot = D3DXVec3Dot( &sdwcurup4, &sdwbefup4 );
			if( invdot > 0.9999f ){	
				invupq.RotationArc( sdwcurup4, sdwbefup4 );// cur --> bef			
			}else{
				if( invdot > 1.0f )
					invdot = 1.0f;
				if( invdot < -1.0f )
					invdot = -1.0f;
				float tempinvrad;
				tempinvrad = (float)acos( invdot );

				CQuaternion2 invup0, invup1;
				invup0.SetAxisAndRot( curdir2, tempinvrad );
				invup1.SetAxisAndRot( curdir2, -tempinvrad );

				D3DXVECTOR3 chkinv0, chkinv1;
				float chkinvdot0, chkinvdot1;
				invup0.Rotate( &chkinv0, sdwcurup4 );
				invup1.Rotate( &chkinv1, sdwcurup4 );
				DXVec3Normalize( &chkinv0, &chkinv0 );
				DXVec3Normalize( &chkinv1, &chkinv1 );

				chkinvdot0 = D3DXVec3Dot( &chkinv0, &sdwbefup4 );
				chkinvdot1 = D3DXVec3Dot( &chkinv1, &sdwbefup4 );

				if( chkinvdot0 > chkinvdot1 ){
					invupq = invup0;
				}else{
					invupq = invup1;
				}
			}
			invupq.GetAxisAndRot( &invupaxis, &invuprad );

			//if( fabs( chkdot1 ) < 0.99996f ){
			if( fabs( chkdot1 ) < 0.9993f ){
				if( invuprad > (float)PAI ){
					invuprad = invuprad - 2.0f * (float)PAI;
				}

				if( invuprad == 0.0f ){
					rot3rad = 0.0f;
				}else if( invuprad > 0.0f ){
					rot3rad = invuprad - invuprad * 0.05f;
					//rot3rad = invuprad - fmindeg * (float)DEG2PAI;
					if( rot3rad < 0.0f )
						rot3rad = 0.0f;
				}else{
					rot3rad = invuprad + invuprad * 0.05f;
					//rot3rad = invuprad + fmindeg * (float)DEG2PAI;
					if( rot3rad > 0.0f )
						rot3rad = 0.0f;
				}
			}else{
				rot3rad = invuprad;
			}
	//#ifdef CHECKPRINT
	//		DbgOut( "q2 : LookAtQ : rot3 : invuprad %f, rot3rad %f\n", invuprad, rot3rad );
	//#endif
			q_rot3.SetAxisAndRot( curdir2, rot3rad );
		

		}else{

			q_rot3.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );

		}

		/***
		// befqのupが、(q_rot3 * q_rot2 * q_rot1)の回転によって、 finalq のupに、近づくかどうかを調べて、
		// 近づかないようなら、up方向の変更は、行わない。
		if( finalq ){
			D3DXVECTOR3 		



		}
		***/


	}else{
		q_rot2.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );
		q_rot3.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	}

	*resqptr = q_rot3 * q_rot2 * q_rot1;

	*dstrot1 = q_rot1;
	*dstrot2 = q_rot2;

//#ifdef CHECKPRINT

//	if( addflag == 1 ){
//		D3DXVECTOR3 chkadd;
//		D3DXVECTOR3 chkaddup;
//		resqptr->Rotate( &chkadd, basevec );
//		resqptr->Rotate( &chkaddup, upvec );

//		DbgOut( "q2 : LookAtQ : chkadd %f %f %f\n", chkadd.x, chkadd.y, chkadd.z );
//		DbgOut( "q2 : LookAtQ : chkaddup %f %f %f\n", chkaddup.x, chkaddup.y, chkaddup.z );

//	}
//#endif

	return 0;
}

int CQuaternion2::GradUp( CQuaternion2 befq, D3DXVECTOR3 basevec, D3DXVECTOR3 upvec, CQuaternion2 rot1, CQuaternion2 rot2 )
{

	// 上向きベクトルを徐々に動かすように、戻す方向に回転する。
	//D3DXVECTOR3 rot3axis;
	int ret;
	D3DXVECTOR3 zerovec( 0.0f, 0.0f, 0.0f );
	float rot3rad;
			
	CQuaternion2 curq;
	curq = rot2 * rot1;

	D3DXVECTOR3 curdir2;
	curq.Rotate( &curdir2, basevec );
	DXVec3Normalize( &curdir2, &curdir2 );


	D3DXVECTOR3 befup4, curup4;
	befq.Rotate( &befup4, upvec );
	curq.Rotate( &curup4, upvec );

	DXVec3Normalize( &befup4, &befup4 );
	DXVec3Normalize( &curup4, &curup4 );

	D3DXVECTOR3 sdwbefup4, sdwcurup4;
	ret = GetShadow( &sdwbefup4, curdir2, zerovec, befup4, curdir2 );
	if( ret ){
		DbgOut( "q2 : GradUp : GetShadow error 1!!!\n" );
		_ASSERT( 0 );
		return 1;
	}		
	ret = GetShadow( &sdwcurup4, curdir2, zerovec, curup4, curdir2 );
	if( ret ){
		DbgOut( "q2 : GradUp : GetShadow error 2!!!\n" );
		_ASSERT( 0 );
		return 1;
	}		


	CQuaternion2 invupq;
	D3DXVECTOR3 invupaxis;
	float invuprad;

	float invdot;
	invdot = D3DXVec3Dot( &sdwcurup4, &sdwbefup4 );
	
	invupq.RotationArc( sdwcurup4, sdwbefup4 );// cur --> bef			
	invupq.GetAxisAndRot( &invupaxis, &invuprad );
	

	if( invupaxis != zerovec )
		DXVec3Normalize( &invupaxis, &invupaxis );

	if( invuprad != 0.0f ){
		//そのまま	
	}else{
		// 0度、or １８０度
		if( invdot > 0.0f ){
			invupaxis = curdir2;
			invuprad = 0.0f;
		}else{
			invupaxis = curdir2;
			invuprad = (float)PAI;
		}
	}

	/***
	if( invdot > 0.9999f ){	
		invupq.RotationArc( sdwcurup4, sdwbefup4 );// cur --> bef			
	}else{
		if( invdot > 1.0f )
			invdot = 1.0f;
		if( invdot < -1.0f )
			invdot = -1.0f;
		float tempinvrad;
		tempinvrad = (float)acos( invdot );

		CQuaternion2 invup0, invup1;
		invup0.SetAxisAndRot( curdir2, tempinvrad );
		invup1.SetAxisAndRot( curdir2, -tempinvrad );

		D3DXVECTOR3 chkinv0, chkinv1;
		float chkinvdot0, chkinvdot1;
		invup0.Rotate( &chkinv0, sdwcurup4 );
		invup1.Rotate( &chkinv1, sdwcurup4 );
		DXVec3Normalize( &chkinv0, &chkinv0 );
		DXVec3Normalize( &chkinv1, &chkinv1 );

		chkinvdot0 = D3DXVec3Dot( &chkinv0, &sdwbefup4 );
		chkinvdot1 = D3DXVec3Dot( &chkinv1, &sdwbefup4 );

		if( chkinvdot0 > chkinvdot1 ){
			invupq = invup0;
		}else{
			invupq = invup1;
		}
	}
	invupq.GetAxisAndRot( &invupaxis, &invuprad );
	***/


	float chkdot1;
	chkdot1 = D3DXVec3Dot( &curdir2, &upvec );
	if( chkdot1 > 1.0f )
		chkdot1 = 1.0f;
	if( chkdot1 < -1.0f )
		chkdot1 = -1.0f;

	//if( fabs( chkdot1 ) < 0.99996f ){
	//if( fabs( chkdot1 ) < 0.9993f ){//2deg
	//if( fabs( chkdot1 ) < 0.9998f ){//1deg
	if( fabs( chkdot1 ) < 0.999961f ){//0.5deg
		if( invuprad > (float)PAI ){
			invuprad = invuprad - 2.0f * (float)PAI;
		}

		float minrad;
		minrad = invuprad * 0.05f;
		if( minrad < 0.5f * (float)DEG2PAI )
			minrad = 0.5f * (float)DEG2PAI;

		if( invuprad == 0.0f ){
			rot3rad = 0.0f;
		}else if( invuprad > 0.0f ){
			rot3rad = invuprad - minrad;
			if( rot3rad < 0.0f )
				rot3rad = 0.0f;
		}else{
			rot3rad = invuprad + minrad;
			if( rot3rad > 0.0f )
				rot3rad = 0.0f;
		}

		/***
		if( invuprad == 0.0f ){
			rot3rad = 0.0f;
		}else if( invuprad > 0.0f ){
			rot3rad = invuprad - invuprad * 0.05f;
			//rot3rad = invuprad - fmindeg * (float)DEG2PAI;
			if( rot3rad < 0.0f )
				rot3rad = 0.0f;
		}else{
			rot3rad = invuprad + invuprad * 0.05f;
			//rot3rad = invuprad + fmindeg * (float)DEG2PAI;
			if( rot3rad > 0.0f )
				rot3rad = 0.0f;
		}
		***/
	}else{
		rot3rad = invuprad;
	}
	//#ifdef CHECKPRINT
	//		DbgOut( "q2 : LookAtQ : rot3 : invuprad %f, rot3rad %f\n", invuprad, rot3rad );
	//#endif
	CQuaternion2 rot3;
	rot3.SetAxisAndRot( curdir2, rot3rad );
		
	
	CQuaternion2 testq1, testq2;
	testq1.SetAxisAndRot( curdir2, rot3rad );
	testq2.SetAxisAndRot( curdir2, -rot3rad );

	D3DXVECTOR3 testup1, testup2;
	testq1.Rotate( &testup1, upvec );
	testq2.Rotate( &testup2, upvec );

	DXVec3Normalize( &testup1, &testup1 );
	DXVec3Normalize( &testup2, &testup2 );

	float testdot1, testdot2;
	testdot1 = D3DXVec3Dot( &testup1, &befup4 );
	testdot2 = D3DXVec3Dot( &testup2, &befup4 );

	if( testdot1 > testdot2 ){
		rot3 = testq1;
	}else{
		rot3 = testq2;
	}


	*this = rot3 * rot2 * rot1;


	return 0;
}



int CQuaternion2::DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec )
{
	float mag;
	mag = srcvec->x * srcvec->x + srcvec->y * srcvec->y + srcvec->z * srcvec->z;

	float sqmag;
	sqmag = (float)sqrt( mag );

	if( sqmag != 0.0f ){
		float magdiv;
		magdiv = 1.0f / sqmag;
//		dstvec->x *= magdiv;
//		dstvec->y *= magdiv;
//		dstvec->z *= magdiv;
		dstvec->x = srcvec->x * magdiv;
		dstvec->y = srcvec->y * magdiv;
		dstvec->z = srcvec->z * magdiv;
	}else{
		//DbgOut( "q2 : DXVec3Normalize : zero warning !!!\n" );
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
		//_ASSERT( 0 );
	}

	return 0;
}


int CQuaternion2::CheckSault( D3DXVECTOR3 basevec, D3DXVECTOR3 tarvec, int upflag, int* resptr )
{

	if( upflag != 2 ){
		*resptr = 0;
		return 0;
	}

	int ret;

	int checkdotflag = 0;
	int checkmvflag = 0;
	int checkdist1flag = 0;
	int checksame2dflag = 0;
	int checkrflag = 0;
	int checkdist2flag = 0;
	

	DXVec3Normalize( &basevec, &basevec );
	DXVec3Normalize( &tarvec, &tarvec );

	D3DXVECTOR3 toppos;
	if( tarvec.y > 0.0f ){
		toppos.x = 0.0f;
		toppos.y = 1.0f;
		toppos.z = 0.0f;
	}else{
		toppos.x = 0.0f;
		toppos.y = -1.0f;
		toppos.z = 0.0f;
	}

	D3DXVECTOR3 curpos;
	Rotate( &curpos, basevec );
	DXVec3Normalize( &curpos, &curpos );


	*resptr = 0;


	//top->tar と cur->top が垂直の時は、saultしない。
	D3DXVECTOR3 top2tar, cur2top;
	top2tar = tarvec - toppos;
	cur2top = toppos - curpos;
	float vertcheck;
	vertcheck = D3DXVec3Dot( &top2tar, &cur2top );
	if( vertcheck == 0.0f ){
		//*resptr = 0;
		//return 0;
		checkdotflag = 0;
	}else{
		checkdotflag = 1;
	}

	//cur2tarが、top2tarより小さいときは、saultしない。（ＸＺ平面）
	D3DXVECTOR2 tarxz, curxz, topxz;
	tarxz.x = tarvec.x;
	tarxz.y = tarvec.z;

	curxz.x = curpos.x;
	curxz.y = curpos.z;

	topxz.x = toppos.x;
	topxz.y = toppos.z;

	D3DXVECTOR2 cur2tarxz, top2tarxz;
	float mag_cur2tar, mag_top2tar;

	cur2tarxz = tarxz - curxz;
	top2tarxz = tarxz - topxz;

	mag_cur2tar = cur2tarxz.x * cur2tarxz.x + cur2tarxz.y * cur2tarxz.y;
	mag_top2tar = top2tarxz.x * top2tarxz.x + top2tarxz.y * top2tarxz.y;

	if( mag_cur2tar <= mag_top2tar ){
		//*resptr = 0;
		//return 0;
		checkmvflag = 0;
	}else{
		checkmvflag = 1;
	}

	// 線分　curpos2d<--->tarpos2dの長さをチェックする。
	// 短い移動は、saultしない
	//float minmag;
	float minelem;
	minelem = (float)sin( 6.0f * (float)DEG2PAI ) * (float)sin( 6.0f * (float)DEG2PAI );
	//minmag = 2.0f * minelem * minelem;
	if( mag_cur2tar <= minelem ){
		//*resptr = 0;
		//return 0;
		checkdist1flag = 0;
	}else{
		checkdist1flag = 1;
	}


///////////////////////


	D3DXVECTOR2 curpos2d, tarpos2d;
	curpos2d.x = curpos.x;
	curpos2d.y = curpos.z;//
	tarpos2d.x = tarvec.x;
	tarpos2d.y = tarvec.z;//

	if( (curpos2d.x == tarpos2d.x) && (curpos2d.y == tarpos2d.y) ){
		//*resptr = 0;
		//return 0;
		checksame2dflag = 0;
	}else{
		checksame2dflag = 1;
	}


	// orgpos から、線分　curpos2d<--->tarpos2d への垂線の足footposを求める。
	D3DXVECTOR2 footpos2d;
	D3DXVECTOR2 orgpos2d( 0.0f, 0.0f );
	int betweenflag = 0;
	float rett = 0.0f;
	ret = GetFootPos2D( orgpos2d, curpos2d, tarpos2d, &footpos2d, &betweenflag, &rett );
	if( ret ){
		DbgOut( "q2 : CheckSault : GetFootPos2D : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( betweenflag == 1 ){
		
		// curpos2d-->tarpos2dが、半径と重なっていて、Ｙ軸をまたいでいない場合を、はじく。
		D3DXVECTOR3 org3d( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 org2cur0y, org2tar0y;
		org2cur0y = curpos - org3d;
		org2cur0y.y = 0.0f;
		org2tar0y = tarvec - org3d;
		org2tar0y.y = 0.0f;

		DXVec3Normalize( &org2cur0y, &org2cur0y );
		DXVec3Normalize( &org2tar0y, &org2tar0y );

		CQuaternion2 rot0y;
		rot0y.RotationArc( org2cur0y, org2tar0y );
		D3DXVECTOR3 rot0yaxis;
		float rot0yrad;
		rot0y.GetAxisAndRot( &rot0yaxis, &rot0yrad );


		//if( (rot0yrad == 0.0f) &&  ((rett >= 0.000001f) || (rett >= 0.999999f)) ){
		if( rot0yrad == 0.0f ){
			float dot1;
			D3DXVECTOR2 chkvec1, chkvec2;
			chkvec1 = curpos2d - orgpos2d;
			chkvec2 = tarpos2d - orgpos2d;

			dot1 = D3DXVec2Dot( &chkvec1, &chkvec2 );
			if( dot1 >= 0.0f ){
				//*resptr = 0;
				//return 0;
				checkrflag = 0;
			}else{
				checkrflag = 1;
			}
		}else{
			checkrflag = 1;
		}

		// orgposからfootposまでの距離をチェックする。
		D3DXVECTOR2 diffvec;
		diffvec.x = footpos2d.x - orgpos2d.x;
		diffvec.y = footpos2d.y - orgpos2d.y;

		float dist;
		float mag;
		mag = diffvec.x * diffvec.x + diffvec.y * diffvec.y;
		if( mag > 0.0f ){
			dist = (float)sqrt( mag );
		}else{
			dist = 0.0f;
		}
		
		if( dist <= 0.30f ){
			//*resptr = 1;
			checkdist2flag = 1;
		}else{
			//*resptr = 0;
			checkdist2flag = 0;
		}

	}else{
		//*resptr = 0;
		checkrflag = 0;
		checkdist2flag = 0;
	}

	//DbgOut( "q2 : CheckSault : saultflag %d, mag_cur2tar %f, minelem %f\n", *resptr, mag_cur2tar, minelem );


		

	if( (checkdotflag == 1) && (checkmvflag == 1) && (checkdist1flag == 1) && (checksame2dflag == 1) && (checkrflag == 1) && (checkdist2flag == 1) ){
		*resptr = 1;
	}else{

		if( (checkdotflag == 1) && (checkmvflag == 0) && (checkdist1flag == 0) && (checksame2dflag == 1) && (checkrflag == 1) && (checkdist2flag == 1) ){
			*resptr = 2;
		}
	}


	//DbgOut( "q2 : checkdotflag %d, checkmvflag %d, checkdist1flag %d, checksame2dflag %d, checkrflag %d, checkdist2flag %d, betweenflag %d\n",
	//	checkdotflag, checkmvflag, checkdist1flag, checksame2dflag, checkrflag, checkdist2flag, betweenflag );


	return 0;
}


int CQuaternion2::GetFootPos2D( D3DXVECTOR2 o0, D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DXVECTOR2* footpos2d, int* betweenptr, float* rett )
{
	// 線分p1-->p2上に、o0からおろした垂線の足、footpos2dを求める。
	// (1 - t)p1 + t p2 の線分上での、垂線の足のt を*rettに代入する。
	// 足が線分上にある場合は、*betweenptr に１を入れる。そうでない場合は、０を入れる。
	
	float alpha, beta;
	float t;
	
	alpha = p1.x - p2.x;
	beta = p1.y - p2.y;

	if( (alpha * alpha + beta * beta) == 0.0f ){
		//DbgOut( "q2 : GetFootPos2D : p1 == p2 warning !!!\n" );
		footpos2d->x = 0.0f;
		footpos2d->y = 0.0f;
		*betweenptr = 0;
		return 0;
	}

	t = ( alpha * ( p1.x - o0.x ) + beta * ( p1.y - o0.y ) ) / ( alpha * alpha + beta * beta );

	*rett = t;

	if( (t >= 0.0f) && (t <= 1.0f) ){
		*betweenptr = 1;
	}else{
		*betweenptr = 0;
	}

	footpos2d->x = ( 1.0f - t ) * p1.x + t * p2.x;
	footpos2d->y = ( 1.0f - t ) * p1.y + t * p2.y;


	return 0;
}

int CQuaternion2::Get180Axis( D3DXVECTOR3 tarvec, D3DXVECTOR3* axisptr )
{
	float ax, ay, az;
	if( (tarvec.x == 0.0f) && (tarvec.y == 0.0f) ){
		az = 0.0f;
		ay = 0.0f;
		ax = 1.0f;

	}else if( (tarvec.x == 0.0f) && (tarvec.z == 0.0f) ){
		ay = 0.0f;
		ax = 0.0f;
		az = 1.0f;

	}else if( (tarvec.y == 0.0f) && (tarvec.z == 0.0f) ){
		ax = 0.0f;
		ay = 1.0f;
		az = 0.0f;

	}else{
		if( tarvec.x != 0.0f ){
			ax = -tarvec.z / tarvec.x;
			ay = 0.0f;
			az = 1.0f;

		}else if( tarvec.y != 0.0f ){
			az = 1.0f;
			ax = 0.0f;
			ay = -tarvec.z / tarvec.y;

		}else if( tarvec.z != 0.0f ){
			ax = 0.0f;
			ay = 1.0f;
			az = -tarvec.y / tarvec.z;

		}else{
			DbgOut( "e3dhsp : LookUpQ : diffvec error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	axisptr->x = ax;
	axisptr->y = ay;
	axisptr->z = az;
	DXVec3Normalize( axisptr, axisptr );

	return 0;
}

int CQuaternion2::ShiftVecNearY( D3DXVECTOR3 basevec, D3DXVECTOR3 tarvec, int* shiftflag, D3DXVECTOR3* shifttarvec )
{
	// tarvecがＹ軸に近いときは、ずらす
	int ret;

	DXVec3Normalize( &basevec, &basevec );
	DXVec3Normalize( &tarvec, &tarvec );

	CQuaternion2 q_rot1;

	D3DXVECTOR3 trabase;
	Rotate( &trabase, basevec );

	D3DXVECTOR3 axis;
	float rad;
	CQuaternion2 diffq;

	diffq.RotationArc( trabase, tarvec );
	diffq.GetAxisAndRot( &axis, &rad );

	int zeroaxisflag = 0;


	if( rad != 0.0f ){
		if( rad <= (float)PAI ){
				//そのまま
		}else{
			rad = -rad;
		}
	}else{

		//rad は、0 or PAI
		zeroaxisflag = 1;

		ret = Get180Axis( tarvec, &axis );
		if( ret ){
			DbgOut( "q2 : LookAtQ : Get180Axis error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		float dot;
		dot = D3DXVec3Dot( &tarvec, &trabase );
		if( dot < 0.0f ){
			rad = (float)PAI;
		}else{
			rad = 0.0f;
		}

	}
	
	/***
	if( (axis.x != 0.0f) || (axis.y != 0.0f) || (axis.z != 0.0f) ){
		if( rad <= (float)PAI ){
				//そのまま
		}else{
			rad = -rad;
		}
	}else{
		zeroaxisflag = 1;

		ret = Get180Axis( tarvec, &axis );
		if( ret ){
			DbgOut( "q2 : LookAtQ : Get180Axis error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		if( rad <= (float)PAI ){
				//そのまま
		}else{
			rad = -rad;
		}

	}
	***/

	CQuaternion2 qstep1;
	qstep1.SetAxisAndRot( axis, rad );
	q_rot1 = qstep1 * *this;


	int addflag = 0;

	
//Ｙ軸に平行な姿勢を避ける
	// 0.1 -->0.9999996
	// 0.2 -->0.9999939
	// 0.03 --> 0.99999986292216422770342144067523

	D3DXVECTOR3 chkvec;
	q_rot1.Rotate( &chkvec, basevec );

	DXVec3Normalize( &chkvec, &chkvec );
	float chkdot0;
	D3DXVECTOR3 upvec( 0.0f, 1.0f, 0.0f );
	chkdot0 = D3DXVec3Dot( &upvec, &chkvec );
	if( (chkdot0 >= 0.999999619f) || (chkdot0 <= -0.999999619f) ){
		if( (axis.x != 0.0f) || (axis.y != 0.0f) || (axis.z != 0.0f) ){

			if( rad >= 0.0f )
				rad += 0.1f * (float)DEG2PAI;
			else
				rad -= 0.1f * (float)DEG2PAI;


			CQuaternion2 addrot;
			addrot.SetAxisAndRot( axis, rad );


			CQuaternion2 newqrot1;
			newqrot1 = addrot * *this;
		

			D3DXVECTOR3 secondcheck;
			newqrot1.Rotate( &secondcheck, basevec );
			DXVec3Normalize( &secondcheck, &secondcheck );
			float seconddot;
			seconddot = D3DXVec3Dot( &secondcheck, &upvec );
			if( fabs( seconddot ) >= 0.999999619f ){
				
				if( rad >= 0.0f )
					rad += 0.1f * (float)DEG2PAI;
				else
					rad -= 0.1f * (float)DEG2PAI;

				addrot.SetAxisAndRot( axis, rad );
				newqrot1 = addrot * *this;
			}

			q_rot1 = newqrot1;
		}else{
DbgOut( "q2 : ShiftVecNearY : axis 0 warning !!!\n" );
		}
		addflag = 1;
	}

	if( addflag == 1 ){
		*shiftflag = 1;
	}else{
		*shiftflag = 0;
	}

	D3DXVECTOR3 finaltarget;
	q_rot1.Rotate( shifttarvec, basevec );
	DXVec3Normalize( shifttarvec, shifttarvec );


if( *shifttarvec == upvec ){
	DbgOut( "q2 : ShiftVecNearY : not shift warning !!! addflag %d, axis %f %f %f, rad %f\n", addflag, axis.x, axis.y, axis.z, rad );
}


	return 0;
}



int CQuaternion2::GetDividedTarget( D3DXVECTOR3 basevec, D3DXVECTOR3 target, D3DXVECTOR3 lasttarget, int olddivnum, D3DXVECTOR3* newtargetptr, int* newdivnumptr )
{
	// targetを経由して、lasttargetへ回転する場合の、小分けにしたnewtargetptrを取得する。


//DbgOut( "q2 : Divided : 0 : target %f %f %f\n", target.x, target.y, target.z );

	int ret;
	D3DXVECTOR3 firstpos;
	Rotate( &firstpos, basevec );
	DXVec3Normalize( &firstpos, &firstpos );

	DXVec3Normalize( &target, &target );
	DXVec3Normalize( &lasttarget, &lasttarget );
	

	// 近すぎるtargetには、回転しない。（振動防止）
	float checkfirst;
	checkfirst = D3DXVec3Dot( &firstpos, &target );
	if( checkfirst > 0.99f ){
		CQuaternion2 checkq;
		checkq.RotationArc( firstpos, target );
		D3DXVECTOR3 checkaxis;
		float checkrad;
		checkq.GetAxisAndRot( &checkaxis, &checkrad );

		if( checkrad <= 0.0000001f * (float)DEG2PAI ){
		//if( checkrad <= 0.00001f * (float)DEG2PAI ){
			*newtargetptr = target;
			*newdivnumptr = 0;//!!!!!!!!!

//DbgOut( "q2 : Divided : target %f %f %f\n", target.x, target.y, target.z );
//DbgOut( "q2 : Divided : near target : newtarget %f %f %f\n", newtargetptr->x, newtargetptr->y, newtargetptr->z );
			
			return 0;
		}
	}


	CQuaternion2 tempq;
	D3DXVECTOR3 tempaxis;
	float temprad;

	tempq.RotationArc( firstpos, target );
	tempq.GetAxisAndRot( &tempaxis, &temprad );

	if( temprad == 0.0f ){
		float chkdot;
		chkdot = D3DXVec3Dot( &firstpos, &target );

		if( chkdot < 0.0f ){
			// dot が負の時は、１８０度回転
			ret = tempq.Get180Axis( target, &tempaxis );
			if( ret ){
				DbgOut( "e3dhsp : E3DLookAtQ : Get180Axis error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			temprad = (float)PAI;
		}
	}
	if( temprad > (float)PAI ){
		temprad -= 2.0f * (float)PAI;
	}


	//DbgOut( "q2 : GetDividedTarget : temprad %f\n", temprad );

	/***
	//newfinalposが、Ｙ軸に近すぎないかチェックして修正。
	D3DXVECTOR3 newfinalpos;
	tempq.Rotate( &newfinalpos, firstpos );
	DXVec3Normalize( &newfinalpos, &newfinalpos );
	D3DXVECTOR3 yvec( 0.0f, 1.0f, 0.0f );
	float doty;
	doty = D3DXVec3Dot( &newfinalpos, &yvec );
	if( fabs( doty ) >= 0.99939f ){
		if( temprad > 0.0f ){
			temprad += 2.0f * (float)DEG2PAI;
		}else{
			temprad -= 2.0f * (float)DEG2PAI;
		}
	}
	***/


	float maxrad = 10.0f * (float)DEG2PAI;

				
	CQuaternion2 tempq2;
	if( temprad >= 0.0f ){
		tempq2.SetAxisAndRot( tempaxis, maxrad );
	}else{
		tempq2.SetAxisAndRot( tempaxis, -maxrad );
	}
	D3DXVECTOR3 newtarget;
	tempq2.Rotate( &newtarget, firstpos );
	DXVec3Normalize( &newtarget, &newtarget );


	//newtargetが、Ｙ軸に近い場合はずらす。
	float doty2;
	D3DXVECTOR3 yvec( 0.0f, 1.0f, 0.0f );
	doty2 = D3DXVec3Dot( &newtarget, &yvec );
	if( fabs( doty2 ) >= 0.99939f ){
		maxrad += 2.0f * (float)DEG2PAI;
	}
	if( temprad >= 0.0f ){
		tempq2.SetAxisAndRot( tempaxis, maxrad );
	}else{
		tempq2.SetAxisAndRot( tempaxis, -maxrad );
	}
	tempq2.Rotate( &newtarget, firstpos );
	DXVec3Normalize( &newtarget, &newtarget );

		// second check
	float doty3;
	doty3 = D3DXVec3Dot( &newtarget, &yvec );
	if( fabs( doty3 ) >= 0.99939f ){
		maxrad += 2.0f * (float)DEG2PAI;
	}
	if( temprad >= 0.0f ){
		tempq2.SetAxisAndRot( tempaxis, maxrad );
	}else{
		tempq2.SetAxisAndRot( tempaxis, -maxrad );
	}
	tempq2.Rotate( &newtarget, firstpos );
	DXVec3Normalize( &newtarget, &newtarget );


/***
	//newtargetが、Ｙ軸に近い場合はずらす。
	float doty2;
	D3DXVECTOR3 yvec( 0.0f, 1.0f, 0.0f );
	doty2 = D3DXVec3Dot( &newtarget, &yvec );
	if( fabs( doty2 ) >= 0.99999f ){
		maxrad += 0.1f * (float)DEG2PAI;
	}
	if( temprad >= 0.0f ){
		tempq2.SetAxisAndRot( tempaxis, maxrad );
	}else{
		tempq2.SetAxisAndRot( tempaxis, -maxrad );
	}
	tempq2.Rotate( &newtarget, firstpos );
	DXVec3Normalize( &newtarget, &newtarget );

		// second check
	float doty3;
	doty3 = D3DXVec3Dot( &newtarget, &yvec );
	if( fabs( doty3 ) >= 0.99999f ){
		maxrad += 0.1f * (float)DEG2PAI;
	}
	if( temprad >= 0.0f ){
		tempq2.SetAxisAndRot( tempaxis, maxrad );
	}else{
		tempq2.SetAxisAndRot( tempaxis, -maxrad );
	}
	tempq2.Rotate( &newtarget, firstpos );
	DXVec3Normalize( &newtarget, &newtarget );
***/


	// lastradを求める
	CQuaternion2 lastq;
	D3DXVECTOR3 lastaxis;
	float lastrad;
	lastq.RotationArc( firstpos, lasttarget );
	lastq.GetAxisAndRot( &lastaxis, &lastrad );

	if( lastrad == 0.0f ){
		float chkdot;
		chkdot = D3DXVec3Dot( &firstpos, &lasttarget );

		if( chkdot < 0.0f ){
			// dot が負の時は、１８０度回転
			ret = lastq.Get180Axis( lasttarget, &lastaxis );
			if( ret ){
				DbgOut( "e3dhsp : E3DLookAtQ : Get180Axis error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			lastrad = (float)PAI;
		}
	}
	if( lastrad > (float)PAI ){
		lastrad -= 2.0f * (float)PAI;
	}


	// newdivnumを求める
	float firststep;
	firststep = (float)fabs( lastrad ) / (float)olddivnum;//!!!!!!!!!!!!
	if( firststep > maxrad )
		firststep = maxrad;
				
	int newdivnum;

	newdivnum = (int)( maxrad / firststep ) + 1;
	
	if( newdivnum < 1 ){
		newdivnum = 1;
	}

	*newtargetptr = newtarget;
	*newdivnumptr = newdivnum;

//DbgOut( "q2 : Divided : newtarget %f %f %f, tempaxis %f %f %f, rad %f\n", newtargetptr->x, newtargetptr->y, newtargetptr->z, tempaxis.x, tempaxis.y, tempaxis.z, temprad );


	return 0;
}


