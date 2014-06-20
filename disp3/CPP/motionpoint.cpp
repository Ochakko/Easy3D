#include <stdafx.h>

#include <windows.h>
#include <coef.h>
#include <motionpoint.h>
#include <math.h>

#include <crtdbg.h>
#define DBGH
#include <dbg.h>

CMotionPoint::CMotionPoint()
{

	InitParams();
}

int CMotionPoint::InitParams()
{
	ishead = 0;
	m_frameno = 0;
	
	m_rotx = 0.0f;
	m_roty = 0.0f;
	m_rotz = 0.0f;

	m_mvx = 0.0f;
	m_mvy = 0.0f;
	m_mvz = 0.0f;

	//m_axis.x = 0.0f;
	//m_axis.y = 0.0f;
	//m_axis.z = 0.0f;
	//m_axisrot = 0.0f;


	m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	prev = 0;
	next = 0;
	//InitConvFlag();


	return 0;
}


CMotionPoint::~CMotionPoint()
{

}
/***
int CMotionPoint::InitConvFlag()
{
	convflagx = 0;
	convflagy = 0;
	convflagz = 0;
	return 0;
}
***/
/***
int CMotionPoint::SetParams( int srcframe,
	float srcrotx, float srcroty, float srcrotz,
	float srcmvx, float srcmvy, float srcmvz )
{
	int ret;
	ret = SetFrameNo( srcframe );
	_ASSERT( !ret );
	ret += SetRotParams( srcrotx, srcroty, srcrotz );
	_ASSERT( !ret );
	ret += SetMoveParams( srcmvx, srcmvy, srcmvz );
	_ASSERT( !ret );

	//InitConvFlag();

	return ret;
}
***/

/***
int CMotionPoint::SetParams( int srcframe, D3DXVECTOR3 axis, float axisrot,
		float srcmvx, float srcmvy, float srcmvz )
{
	int ret;
	ret = SetFrameNo( srcframe );
	_ASSERT( !ret );
	m_axis = axis;
	m_axisrot = axisrot;
	ret = SetMoveParams( srcmvx, srcmvy, srcmvz );
	_ASSERT( !ret );

	return 0;
}
***/
int CMotionPoint::SetQuaternion( CQuaternion srcq )
{
	m_q = srcq;
	return 0;
}

int CMotionPoint::MultQuaternion( CQuaternion srcq )
{

	m_q *= srcq;
	return 0;
}


int CMotionPoint::SetParams( int srcframe, CQuaternion srcq, 
							float srcmvx, float srcmvy, float srcmvz )
{

	int ret;
	ret = SetFrameNo( srcframe );
	_ASSERT( !ret );
	
	m_q = srcq;

	ret = SetMoveParams( srcmvx, srcmvy, srcmvz );
	_ASSERT( !ret );

	return 0;
}

int CMotionPoint::SetFrameNo( int srcframe )
{
	m_frameno = srcframe;
	return 0;
}
int CMotionPoint::SetRotParams( float srcrotx, float srcroty, float srcrotz )
{
	m_rotx = srcrotx;
	m_roty = srcroty;
	m_rotz = srcrotz;

	//InitConvFlag();
	
	return 0;
}

int CMotionPoint::SetMoveParams( float srcmvx, float srcmvy, float srcmvz )
{
	m_mvx = srcmvx;
	m_mvy = srcmvy;
	m_mvz = srcmvz;
	return 0;
}

/***
// inline化
int CMotionPoint::FramenoCmp( CMotionPoint* cmpmp )
{
	int cmpno;
	_ASSERT( cmpmp );
	cmpno = cmpmp->m_frameno;

	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CMotionPoint::FramenoCmp( int cmpno )
{
	if( m_frameno > cmpno ){
		return 1;
	}else if( m_frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
***/

int CMotionPoint::AddToPrev( CMotionPoint* addmp )
{
	CMotionPoint *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addmp->prev = prev;
	addmp->next = this;

	prev = addmp;

	if( saveprev ){
		saveprev->next = addmp;
	}else{
		_ASSERT( ishead );
		addmp->ishead = 1;
		ishead = 0;
	}

	return 0;
}

int CMotionPoint::AddToNext( CMotionPoint* addmp )
{
	CMotionPoint *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addmp->prev = this;
	addmp->next = savenext;

	next = addmp;
	if( savenext ){
		savenext->prev = addmp;
	}

	addmp->ishead = 0;

	return 0;
}

int CMotionPoint::LeaveFromChain()
{
	CMotionPoint *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	prev = 0;
	next = 0;

	if( saveprev ){
		saveprev->next = savenext;
	}else{
		_ASSERT( ishead );
		if( savenext )
			savenext->ishead = 1;
		ishead = 0;
	}

	if( savenext ){
		savenext->prev = saveprev;
	}
	return 0;
}

int CMotionPoint::FillUpMotionPoint( CMotionPoint* srcstartmp, CMotionPoint* srcendmp, int framenum, int framecnt, int interp )
{

	int ret;
	//mvx, mvy, mvzは、成分ごとの線形補間。

	if( interp == INTERPOLATION_SLERP ){

		this->m_q = (srcstartmp->m_q).Slerp( srcendmp->m_q, framenum, framecnt );

	}else if( interp == INTERPOLATION_SQUAD ){
		CQuaternion befq;
		CQuaternion aftq;
		
		if( srcstartmp->prev ){
			befq = (srcstartmp->prev)->m_q;
		}else{
			befq = srcstartmp->m_q;
		}

		if( srcendmp->next ){
			aftq = (srcendmp->next)->m_q;
		}else{
			aftq = srcendmp->m_q;
		}
		
		
		float t;
		t = (float)framecnt / (float)framenum;
		ret = (this->m_q).Squad( befq, srcstartmp->m_q, srcendmp->m_q, aftq, t );
		if( ret ){
			DbgOut( "MotionPoint : FillUpMotionPoint : Squad error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	}



	return 0;
}

int CMotionPoint::CopyMotion( CMotionPoint* srcmp )
{
	m_q = srcmp->m_q;
	m_mvx = srcmp->m_mvx;
	m_mvy = srcmp->m_mvy;
	m_mvz = srcmp->m_mvz;

	return 0;
}


/***
int CMotionPoint::SetQuaternionIK()
{
	float phai2;
	float cos_phai2, sin_phai2;

	phai2 = m_axisrot * 0.5f;
	cos_phai2 = cosf( phai2 );
	sin_phai2 = sinf( phai2 );

	m_q.SetParams( cos_phai2, m_axis.x * sin_phai2, m_axis.y * sin_phai2, m_axis.z * sin_phai2 );

	return 0;
}
***/
/***
int CMotionPoint::SetQuaternion()
{
	CQuaternion qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;
	float deg2rad = PI / 180.0f;

	float radx, rady, radz;
	radx = m_rotx * deg2rad * 0.5f;
	rady = m_roty * deg2rad * 0.5f;
	radz = m_rotz * deg2rad * 0.5f;


	cosx = (float)cos( radx );
	sinx = (float)sin( radx );
	cosy = (float)cos( rady );
	siny = (float)sin( rady );
	cosz = (float)cos( radz );
	sinz = (float)sin( radz );

	qx.SetParams( cosx, sinx, 0.0f, 0.0f );
	qy.SetParams( cosy, 0.0f, siny, 0.0f );
	qz.SetParams( cosz, 0.0f, 0.0f, sinz );

	m_q = qx * qy * qz;


	// ２番目にかける角度が９０度のとき、じんばるロックが起こる？？
	// ９０度に一番近いQuaternionが２番目にこないようにする。
	//float diff[3];
	//float cos45;

	//cos45 = (float)cos( 45.0f * deg2rad );
	//diff[0] = (float)fabs( cos45 - cosx );
	//diff[1] = (float)fabs( cos45 - cosy );
	//diff[2] = (float)fabs( cos45 - cosz );

	//int minno1;
	//if( diff[0] < diff[1] )
	//	minno1 = 0;
	//else
	//	minno1 = 1;

	//int minno2;
	///if( diff[minno1] < diff[2] )
	//	minno2 = minno1;
	//else
	//	minno2 = 2;

	//switch( minno2 ){
	//case 0:
	//	m_q = qx * qy * qz;
	//	break;
	//case 1:
	//	m_q = qy * qx * qz;
	//	break;
	//case 2:
	//	m_q = qz * qy * qx;
	//	break;
	//default:
	//	m_q = qx * qy * qz;
	//	_ASSERT( 0 );
	//	break;
	//}

	return 0;
}
***/