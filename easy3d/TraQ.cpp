#include "stdafx.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include "TraQ.h"

#include <shdhandler.h>
#include <BoneProp.h>

enum {
	CAX_INI,
	CAX_ZA3,
	CAX_MAX
};

CTraQ::CTraQ()
{
	InitParams();
}

CTraQ::~CTraQ()
{
}
int CTraQ::InitParams()
{
	m_tra.x = 0.0f;
	m_tra.y = 0.0f;
	m_tra.z = 0.0f;
	m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
	m_totalq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	m_finaltra.x = 0.0f;
	m_finaltra.y = 0.0f;
	m_finaltra.z = 0.0f;
	m_finalq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	m_orgtra = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_orgq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
	m_orgeul = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_cureul.x = 0.0f;
	m_cureul.y = 0.0f;
	m_cureul.z = 0.0f;

	m_befeul.x = 0.0f;
	m_befeul.y = 0.0f;
	m_befeul.z = 0.0f;

	m_neckbefeul.x = 0.0f;
	m_neckbefeul.y = 0.0f;
	m_neckbefeul.z = 0.0f;

	m_finaleul = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_outputflag = 1;

	return 0;
}

int CTraQ::CalcTorso( CTraQ* traqptr, RPSELEM* rpsptr, int frameno, int skipflag )
{

	D3DXVECTOR3 befpos, aftpos;
	if( (skipflag & 1) == 0 ){
		befpos = ( rpsptr + 0 * SKEL_MAX + SKEL_TORSO )->pos;
		aftpos = ( rpsptr + frameno * SKEL_MAX + SKEL_TORSO )->pos;

		D3DXVECTOR3 befhip, afthip;
		befhip = ( rpsptr + 0 * SKEL_MAX + SKEL_LEFT_HIP )->pos - ( rpsptr + 0 * SKEL_MAX + SKEL_RIGHT_HIP )->pos;
		afthip = ( rpsptr + frameno * SKEL_MAX + SKEL_LEFT_HIP )->pos - ( rpsptr + frameno * SKEL_MAX + SKEL_RIGHT_HIP )->pos;
		befhip.y = 0.0f;
		afthip.y = 0.0f;
		D3DXVECTOR3 nbefhip, nafthip;
		DVec3Normalize( &nbefhip, befhip );
		DVec3Normalize( &nafthip, afthip );

		double radxz;
		D3DXVECTOR3 vDir0, vDir;
		D3DXVECTOR3 dirY( 0.0f, 1.0f, 0.0f );
		DVec3Cross( &nafthip, &dirY, &vDir0 );
		DVec3Normalize( &vDir, vDir0 );
		if( vDir.x == 0.0f ){
			if( vDir.z >= 0.0f )
				radxz = 0.0;
			else
				radxz = PAI;

		}else if( vDir.x > 0.0f ){
			radxz = -atanf( vDir.z / vDir.x ) + PAI / 2;
		}else{
			radxz = -atanf( vDir.z / vDir.x ) - PAI / 2;
		}

		D3DXVECTOR3 axis( 0.0f, 1.0f, 0.0f );
		CQuaternion qxz;
		qxz.SetAxisAndRot( axis, radxz );

		( traqptr + frameno * SKEL_MAX + SKEL_TORSO )->m_q = qxz;
		( traqptr + frameno * SKEL_MAX + SKEL_TORSO )->m_totalq = qxz;
		( traqptr + frameno * SKEL_MAX + SKEL_TORSO )->m_cureul = D3DXVECTOR3( 0.0f, (float)( radxz * PAI2DEG ), 0.0f );
	}

/////////
	if( skipflag != 3 ){
		D3DXVECTOR3 difftorso;
		difftorso = aftpos - befpos;
		( traqptr + frameno * SKEL_MAX + SKEL_TOPOFJOINT )->m_tra = difftorso;
	}

	return 0;
}
int CTraQ::CalcNeck(  CShdHandler* lpsh, TSELEM* tsptr, CTraQ* traqptr, RPSELEM* rpsptr, int frameno, int skipflag )
{
	CQuaternion parq;
	parq = ( traqptr + frameno * SKEL_MAX + SKEL_TORSO )->m_totalq;
	CQuaternion invparq;
	parq.inv( &invparq );

	if( skipflag == 3 ){
		( traqptr + frameno * SKEL_MAX + SKEL_NECK )->m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		( traqptr + frameno * SKEL_MAX + SKEL_NECK )->m_totalq = parq;
		return 0;
	}


	D3DXVECTOR3 befLpos, aftLpos, befRpos, aftRpos;
	befLpos = ( rpsptr + 0 * SKEL_MAX + SKEL_LEFT_SHOULDER )->pos;
	aftLpos = ( rpsptr + frameno * SKEL_MAX + SKEL_LEFT_SHOULDER )->pos;
	befRpos = ( rpsptr + 0 * SKEL_MAX + SKEL_RIGHT_SHOULDER )->pos;
	aftRpos = ( rpsptr + frameno * SKEL_MAX + SKEL_RIGHT_SHOULDER )->pos;

	D3DXVECTOR3 paftLpos, paftRpos;
	invparq.Rotate( &paftLpos, aftLpos );
	invparq.Rotate( &paftRpos, aftRpos );

	double radxz;
	D3DXVECTOR3 vDir0, vDir;
	D3DXVECTOR3 dirY( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 diffpos;
	diffpos = paftLpos - paftRpos;
	DVec3Normalize( &diffpos, diffpos );

	DVec3Cross( &diffpos, &dirY, &vDir0 );
	DVec3Normalize( &vDir, vDir0 );
	if( vDir.x == 0.0f ){
		if( vDir.z >= 0.0f )
			radxz = 0.0;
		else
			radxz = PAI;
	}else if( vDir.x > 0.0f ){
		radxz = -atanf( vDir.z / vDir.x ) + PAI / 2;
	}else{
		radxz = -atanf( vDir.z / vDir.x ) - PAI / 2;
	}

//	D3DXVECTOR3 axis( 0.0f, 1.0f, 0.0f );
//	CQuaternion qxz;
//	qxz.SetAxisAndRot( axis, radxz );

///////////////
	D3DXVECTOR3 beftor, afttor, befneck, aftneck;
	beftor = ( rpsptr + 0 * SKEL_MAX + SKEL_TORSO )->pos;
	afttor = ( rpsptr + frameno * SKEL_MAX + SKEL_TORSO )->pos;
	befneck = ( rpsptr + 0 * SKEL_MAX + SKEL_NECK )->pos;
	aftneck = ( rpsptr + frameno * SKEL_MAX + SKEL_NECK )->pos;

	D3DXVECTOR3 pafttor, paftneck;
	invparq.Rotate( &pafttor, afttor );
	invparq.Rotate( &paftneck, aftneck );

	D3DXVECTOR3 vecA, vecB;
	vecA = befneck - beftor;
	vecB = paftneck - pafttor;
	D3DXVECTOR3 nvecA, nvecB;
	DVec3Normalize( &nvecA, vecA );
	DVec3Normalize( &nvecB, vecB );
	int ret;
	CQuaternion q2;
	ret = DCalcDiffQ( &nvecA, &nvecB, &q2 );
	_ASSERT( !ret );
	

	CQuaternion za3q;
	D3DXVECTOR3 cureul( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 befeul( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 neckbefeul( 0.0f, 0.0f, 0.0f );
	neckbefeul = ( traqptr + ( frameno - 1 ) * SKEL_MAX + SKEL_NECK )->m_neckbefeul;
	ret = QtoEul( lpsh, q2, neckbefeul, CAX_INI, 0, &cureul, &za3q );
	_ASSERT( !ret );

	CQuaternion invza3q;
	za3q.inv( &invza3q );

	D3DXVECTOR3 dirx( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 diry( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 dirz( 0.0f, 0.0f, 1.0f );

	CQuaternion qx2, qy2, qz2, qzx2;
	qx2.SetAxisAndRot( dirx, cureul.x * DEG2PAI );
	qy2.SetAxisAndRot( diry, cureul.y * DEG2PAI );
	qz2.SetAxisAndRot( dirz, cureul.z * DEG2PAI );

	CQuaternion q;
	q = qx2 * qz2;


	D3DXVECTOR3 axis;
	q.Rotate( &axis, diry );
	DVec3Normalize( &axis, axis );
	CQuaternion qxz;
	qxz.SetAxisAndRot( axis, radxz );

	CQuaternion setq;
	if( skipflag & 1 ){
		setq = q;
	}else{
		setq = qxz * q;
	}

/////////////////////
	D3DXVECTOR3 setcureul;
	befeul = ( traqptr + ( frameno - 1 ) * SKEL_MAX + SKEL_NECK )->m_befeul;

	ret = QtoEul( lpsh, setq, befeul, CAX_INI, 0, &setcureul, &za3q );
	_ASSERT( !ret );


	( traqptr + frameno * SKEL_MAX + SKEL_NECK )->m_q = setq;
	( traqptr + frameno * SKEL_MAX + SKEL_NECK )->m_totalq = parq * setq;

	( traqptr + frameno * SKEL_MAX + SKEL_NECK )->m_neckbefeul = cureul;
	( traqptr + frameno * SKEL_MAX + SKEL_NECK )->m_cureul = setcureul;
	( traqptr + frameno * SKEL_MAX + SKEL_NECK )->m_befeul = setcureul;


	return 0;
}


int CTraQ::CalcQ( CShdHandler* lpsh, TSELEM* tsptr, CTraQ* traqptr, RPSELEM* rpsptr, int frameno, int pivotskel, int skelno, int skipflag )
{
	CQuaternion parq;
	parq = ( traqptr + frameno * SKEL_MAX + pivotskel )->m_totalq;
	CQuaternion invparq;
	parq.inv( &invparq );

	if( skipflag != 0 ){
		( traqptr + frameno * SKEL_MAX + skelno )->m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		( traqptr + frameno * SKEL_MAX + skelno )->m_totalq = parq;
		return 0;
	}


	D3DXVECTOR3 befpivpos, aftpivpos, befpos, aftpos;
	befpivpos = ( rpsptr + 0 * SKEL_MAX + pivotskel )->pos;
	aftpivpos = ( rpsptr + frameno * SKEL_MAX + pivotskel )->pos;
	befpos = ( rpsptr + 0 * SKEL_MAX + skelno )->pos;
	aftpos = ( rpsptr + frameno * SKEL_MAX + skelno )->pos;

	D3DXVECTOR3 paftpivpos, paftpos;
	invparq.Rotate( &paftpivpos, aftpivpos );
	invparq.Rotate( &paftpos, aftpos );

	D3DXVECTOR3 vec1, vec2;
	vec1 = befpos - befpivpos;
	vec2 = paftpos - paftpivpos;
	D3DXVECTOR3 nvec1, nvec2;
	DVec3Normalize( &nvec1, vec1 );
	DVec3Normalize( &nvec2, vec2 );


	int ret;
	CQuaternion setq;
	ret = DCalcDiffQ( &nvec1, &nvec2, &setq );
	_ASSERT( !ret );
////////////////


	D3DXVECTOR3 cureul( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 befeul( 0.0f, 0.0f, 0.0f );
	befeul = ( traqptr + ( frameno - 1 ) * SKEL_MAX + skelno )->m_befeul;
	CQuaternion za3q;
	ret = QtoEul( lpsh, setq, befeul, CAX_ZA3, (tsptr + skelno)->jointno, &cureul, &za3q );
	_ASSERT( !ret );

	CQuaternion invza3q;
	za3q.inv( &invza3q );

	float twistdeg;
	if( ( tsptr + skelno )->twistflag == 0 ){

		D3DXVECTOR3 dirx( 1.0f, 0.0f, 0.0f );
		D3DXVECTOR3 diry( 0.0f, 1.0f, 0.0f );
		D3DXVECTOR3 dirz( 0.0f, 0.0f, 1.0f );

		CQuaternion qx, qy, qz;
		qx.SetAxisAndRot( dirx, cureul.x * DEG2PAI );
		qy.SetAxisAndRot( diry, cureul.y * DEG2PAI );
		qz.SetAxisAndRot( dirz, cureul.z * DEG2PAI );
		CQuaternion qxy;
		qxy = qy * qx;


///////////
		
		D3DXVECTOR3 aftdiry;
		qxy.Rotate( &aftdiry, diry );
		DVec3Normalize( &aftdiry, aftdiry );

		double doty;
		DCalcDot( &diry, &aftdiry, &doty );
		double rady;
		rady = acos( doty );

		CQuaternion ya, yb;
		ya.SetAxisAndRot( dirz, rady );
		yb.SetAxisAndRot( dirz, -rady );
		D3DXVECTOR3 afta, aftb;
		ya.Rotate( &afta, diry );
		yb.Rotate( &aftb, diry );
		DVec3Normalize( &afta, afta );
		DVec3Normalize( &aftb, aftb );
		double dotya, dotyb;
		DCalcDot( &aftdiry, &afta, &dotya );
		DCalcDot( &aftdiry, &aftb, &dotyb );
		CQuaternion twistq;
		if( dotya >= dotyb ){
			twistq = ya;
			twistdeg = (float)( rady * PAI2DEG );
		}else{
			twistq = yb;
			twistdeg = (float)( -rady * PAI2DEG );
		}
		

///////////
		setq = za3q * qxy * twistq * invza3q;
	}else{
		twistdeg = cureul.z;
	}


	( traqptr + frameno * SKEL_MAX + skelno )->m_q = setq;
	( traqptr + frameno * SKEL_MAX + skelno )->m_totalq = parq * setq;

	( traqptr + frameno * SKEL_MAX + skelno )->m_befeul = cureul;
	( traqptr + frameno * SKEL_MAX + skelno )->m_cureul = D3DXVECTOR3( cureul.x, cureul.y, twistdeg );


	return 0;
}


int CTraQ::DVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3 srcvec )
{
	double mag;
	mag = (double)srcvec.x * (double)srcvec.x + (double)srcvec.y * (double)srcvec.y + (double)srcvec.z * (double)srcvec.z;

	if( mag == 0.0 ){
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
	}else{
		double leng;
		leng = sqrt( mag );
		dstvec->x = (float)( srcvec.x / leng );
		dstvec->y = (float)( srcvec.y / leng );
		dstvec->z = (float)( srcvec.z / leng );
	}
	return 0;
}

int CTraQ::DVec3Cross( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2, D3DXVECTOR3* dstvec )
{
	
	double x1, y1, z1, x2, y2, z2;
	x1 = (double)vec1->x; y1 = (double)vec1->y; z1 = (double)vec1->z;
	x2 = (double)vec2->x; y2 = (double)vec2->y; z2 = (double)vec2->z;

	D3DXVECTOR3 axis, naxis;

	dstvec->x = (float)( y1 * z2 - z1 * y2 );
	dstvec->y = (float)( z1 * x2 - x1 * z2 );
	dstvec->z = (float)( x1 * y2 - y1 * x2 );

	return 0;
}


int CTraQ::DCalcDiffQ( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2, CQuaternion* dstq )
{
	double dot;
	DCalcDot( vec1, vec2, &dot );

	double rad;
	//if( dot >= 0.9999999998 ){
//	if( dot >= 0.99999998 ){
//		rad = 0.0;
//		dstq->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
//		return 0;
//	}


		rad = acos( dot );



		D3DXVECTOR3 axis, naxis;
		DVec3Cross( vec1, vec2, &axis );
		DVec3Normalize( &naxis, axis );


		CQuaternion q1;
		CQuaternion q2;
		q1.SetAxisAndRot( naxis, rad );
		q2.SetAxisAndRot( naxis, -rad );

		D3DXVECTOR3 aft1, aft2;
		q1.Rotate( &aft1, *vec1 );
		q2.Rotate( &aft2, *vec1 );

		double dot1, dot2;
		DCalcDot( &aft1, vec2, &dot1 );
		DCalcDot( &aft2, vec2, &dot2 );

//		if( dot1 >= dot2 ){
			*dstq = q1;
//		}else{
//			*dstq = q2;
//		}
//	}

	return 0;
}

int CTraQ::DCalcDot( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2, double* dstdot )
{

	*dstdot = (double)vec1->x * (double)vec2->x + (double)vec1->y * (double)vec2->y + (double)vec1->z * (double)vec2->z;

	return 0;
}



/***
int CTraQ::QtoEuler( D3DXVECTOR3* eulptr, D3DXVECTOR3 befeul ){
	int ret; 

	D3DXVECTOR3 af(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 bef(0.0f, 0.0f, 0.0f);
	//ret = E3DQtoEuler( this->m_qid, eulptr, befeul );

	ret = E3DQtoEuler( this->m_qid, &af, bef );
	//E3DDbgOut( "CTraQ : ‚p‚h‚c%d, %lf, %lf, %lf\n", this->m_qid, eulptr->x, eulptr->y, eulptr->z);

	E3DDbgOut( "CTraQ : ‚p‚h‚c%d, %lf, %lf, %lf\n", this->m_qid, af.x, af.y, af.z);
	return 0;
}
***/

int CTraQ::QtoEul( CShdHandler* lpsh, CQuaternion srcq, D3DXVECTOR3 befeul, int axisflag, int boneno, D3DXVECTOR3* eulptr, CQuaternion* axisqptr )
{
	int ret;
	if( axisflag == CAX_INI ){
		axisqptr->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
	}else{
		ret = lpsh->GetInitialBoneQ( boneno, axisqptr );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	D3DXVECTOR3 cureul( 0.0f, 0.0f, 0.0f );
	ret = qToEulerAxis( *axisqptr, &srcq, &cureul );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = modifyEuler( &cureul, &befeul );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*eulptr = cureul;

	return 0;
}
