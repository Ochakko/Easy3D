#include <stdafx.h>

#include <windows.h>
#include <motionpoint2.h>
#include <math.h>

#include <BoneProp.h>

//#include <SPParam.h>

#include <crtdbg.h>
#define DBGH
#include <dbg.h>

static int s_allocno = 0;

CMotionPoint2::CMotionPoint2()
{
	s_allocno++;
	serialno = s_allocno;


	InitParams();

//	m_spp = new CSPParam();
}
CMotionPoint2::~CMotionPoint2()
{
	if( m_spp ){
		delete m_spp;
		m_spp = 0;
	}
}

int CMotionPoint2::CreateSppIfNot()
{
	if( m_spp )
		return 0;

	m_spp = new CSPParam();
	if( !m_spp ){
		DbgOut( "mp2 : CreateSppIfNot spp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotionPoint2::InitParams()
{
	CMotionPoint::InitParams();
	dispswitch = 0;
	interp = 0;

	prev = 0;
	next = 0;

	//reservedint1 = 0;
	//reservedint2 = 0;
	interp = 0;
	
	
	//reservedint3 = 0;
	//reservedint4 = 0;
	//reservedint5 = 0;
	m_scalex = 1.0f;
	m_scaley = 1.0f;
	m_scalez = 1.0f;

	m_userint1 = 0;
	//reservedfl1 = 0;
	//reservedfl2 = 0;
	//reservedfl3 = 0;
	//reservedfl4 = 0;

	m_eul.x = 0.0f;
	m_eul.y = 0.0f;
	m_eul.z = 0.0f;

	m_spp = 0;

	return 0;
}

int CMotionPoint2::SetParams( int srcframe, CQuaternion srcq, 
	float srcmvx, float srcmvy, float srcmvz, DWORD srcdispswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1 )
{
	int ret;

	ret = CMotionPoint::SetParams( srcframe, srcq, srcmvx, srcmvy, srcmvz );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	dispswitch = srcdispswitch;
	interp = srcinterp;

	m_scalex = scalex;
	m_scaley = scaley;
	m_scalez = scalez;

	m_userint1 = userint1;

	return 0;
}

int CMotionPoint2::SetDispSwitch( DWORD srcdswitch )
{
	dispswitch = srcdswitch;

	return 0;
}


int CMotionPoint2::FillUpMotionPoint( CQuaternion* axisq, CMotionPoint2* srcstartmp, CMotionPoint2* srcendmp, int framenum, int framecnt, int interp )
{
	int ret;

// mv, scale
	if( interp == INTERPOLATION_SLERP ){

		//m_q
		ret = CMotionPoint::FillUpMotionPoint( (CMotionPoint*)srcstartmp, (CMotionPoint*)srcendmp, framenum, framecnt, interp );	
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}


		float startmvx, startmvy, startmvz;
		float stepmvx, stepmvy, stepmvz;
		startmvx = srcstartmp->m_mvx;
		startmvy = srcstartmp->m_mvy;
		startmvz = srcstartmp->m_mvz;
		if( framenum != 0 ){
			stepmvx = (srcendmp->m_mvx - startmvx) / (float)framenum;
			stepmvy = (srcendmp->m_mvy - startmvy) / (float)framenum;
			stepmvz = (srcendmp->m_mvz - startmvz) / (float)framenum;
		}else{
			stepmvx = 0.0f;
			stepmvy = 0.0f;
			stepmvz = 0.0f;
		}
		
		m_mvx = startmvx + stepmvx * (float)framecnt;
		m_mvy = startmvy + stepmvy * (float)framecnt;
		m_mvz = startmvz + stepmvz * (float)framecnt;

		/////
		float startscalex, startscaley, startscalez;
		float stepscalex, stepscaley, stepscalez;
		startscalex = srcstartmp->m_scalex;
		startscaley = srcstartmp->m_scaley;
		startscalez = srcstartmp->m_scalez;
		if( framenum != 0 ){
			stepscalex = (srcendmp->m_scalex - startscalex) / (float)framenum;
			stepscaley = (srcendmp->m_scaley - startscaley) / (float)framenum;
			stepscalez = (srcendmp->m_scalez - startscalez) / (float)framenum;
		}else{
			stepscalex = 1.0f;
			stepscaley = 1.0f;
			stepscalez = 1.0f;
		}
		
		m_scalex = startscalex + stepscalex * (float)framecnt;
		m_scaley = startscaley + stepscaley * (float)framecnt;
		m_scalez = startscalez + stepscalez * (float)framecnt;


	}else{

		CMotionPoint2* befmp;
		CMotionPoint2* aftmp;
		
		if( srcstartmp->prev ){
			befmp = srcstartmp->prev;
		}else{
			befmp = srcstartmp;
		}

		if( srcendmp->next ){
			aftmp = srcendmp->next;
		}else{
			aftmp = srcendmp;
		}
	
		float t;
		t = (float)framecnt / (float)framenum;


		int srcx = framecnt + srcstartmp->m_frameno;
		ret = RotBSpline( axisq, srcstartmp, srcendmp, srcx );
		if( ret ){
			DbgOut( "MotionPoint : FillUpMotionPoint : RotBSpline error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = MvBSpline( srcstartmp, srcendmp, srcx );
		if( ret ){
			DbgOut( "MotionPoint : FillUpMotionPoint : MvBSpline error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = ScBSpline( srcstartmp, srcendmp, srcx );
		if( ret ){
			DbgOut( "MotionPoint : FillUpMotionPoint : ScBSpline error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		ret = MvHermiteSpline( befmp, srcstartmp, srcendmp, aftmp, t );
//		if( ret ){
//			DbgOut( "MotionPoint : FillUpMotionPoint : MvHermiteSprine error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//		ret = ScaleHermiteSpline( befmp, srcstartmp, srcendmp, aftmp, t );
//		if( ret ){
//			DbgOut( "MotionPoint : FillUpMotionPoint : ScaleHermiteSprine error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}


	}



//dispswitch
	if( framecnt == 0 ){
		if( srcstartmp )
			dispswitch = srcstartmp->dispswitch;
		else
			dispswitch = 0;

	}else if( framecnt == (framenum - 1) ){
		if( srcendmp ){
			dispswitch = srcendmp->dispswitch;
		}else{
			if( srcstartmp )
				dispswitch = srcstartmp->dispswitch;
			else
				dispswitch = 0;
		}
	}else{
		if( srcstartmp )
			dispswitch = srcstartmp->dispswitch;
		else
			dispswitch = 0;
	}

//interp
	if( framecnt == 0 ){
		if( srcstartmp )
			interp = srcstartmp->interp;
		else
			interp = 0;

	}else if( framecnt == (framenum - 1) ){
		if( srcendmp ){
			interp = srcendmp->interp;
		}else{
			if( srcstartmp )
				interp = srcstartmp->interp;
			else
				interp = 0;
		}
	}else{
		if( srcstartmp )
			interp = srcstartmp->interp;
		else
			interp = 0;
	}



	return 0;
}
int CMotionPoint2::CopyMotion( CMotionPoint2* srcmp )
{
	int ret;

	ret = CMotionPoint::CopyMotion( (CMotionPoint*)srcmp );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	dispswitch = srcmp->dispswitch;
	interp = srcmp->interp;

	m_scalex = srcmp->m_scalex;
	m_scaley = srcmp->m_scaley;
	m_scalez = srcmp->m_scalez;

	m_eul = srcmp->m_eul;

	return 0;
}


////////////
int CMotionPoint2::AddToPrev( CMotionPoint2* addmp )
{
	CMotionPoint2 *saveprev, *savenext;
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

int CMotionPoint2::AddToNext( CMotionPoint2* addmp )
{
	CMotionPoint2 *saveprev, *savenext;
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

int CMotionPoint2::LeaveFromChain()
{
	CMotionPoint2 *saveprev, *savenext;
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

int CMotionPoint2::MvHermiteSpline( CMotionPoint2* mp0, CMotionPoint2* mp1, CMotionPoint2* mp2, CMotionPoint2* mp3, float t )
{

	m_mvx = HermiteSpline( mp0->m_mvx, mp1->m_mvx, mp2->m_mvx, mp3->m_mvx, t );
	m_mvy = HermiteSpline( mp0->m_mvy, mp1->m_mvy, mp2->m_mvy, mp3->m_mvy, t );
	m_mvz = HermiteSpline( mp0->m_mvz, mp1->m_mvz, mp2->m_mvz, mp3->m_mvz, t );


	return 0;
}
int CMotionPoint2::ScaleHermiteSpline( CMotionPoint2* mp0, CMotionPoint2* mp1, CMotionPoint2* mp2, CMotionPoint2* mp3, float t )
{

	m_scalex = HermiteSpline( mp0->m_scalex, mp1->m_scalex, mp2->m_scalex, mp3->m_scalex, t );
	m_scaley = HermiteSpline( mp0->m_scaley, mp1->m_scaley, mp2->m_scaley, mp3->m_scaley, t );
	m_scalez = HermiteSpline( mp0->m_scalez, mp1->m_scalez, mp2->m_scalez, mp3->m_scalez, t );

	return 0;
}




float CMotionPoint2::HermiteSpline( float val1, float val2, float val3, float val4, float t )
{
	const float alpha = 0.0f;
	float m0, m1;

	m0 = ( 1.0f - alpha ) / 2.0f * ( ( val2 - val1 ) + ( val3 - val2 ) );
	m1 = ( 1.0f - alpha ) / 2.0f * ( ( val3 - val2 ) + ( val4 - val3 ) );

	float t2 = t * t;
	float t3 = t2 * t;

	float retval;

	retval = (((2 * t3) - (3 * t2) + 1) * val2) +
                     ((t3 - (2 * t2) + t) * m0) +
                     ((t3 - t2) * m1) +
                     (((-2 * t3) + (3 * t2)) * val3);

	return retval;
}

int CMotionPoint2::IsInitMP()
{
	if( (m_q.x == 0.0f) && (m_q.y == 0.0f) && (m_q.z == 0.0f) && (m_q.w == 1.0f) &&
		(m_mvx == 0.0f) && (m_mvy == 0.0f) && (m_mvz == 0.0f) &&
		(m_scalex == 1.0f) && (m_scaley == 1.0f) && (m_scalez == 1.0f) ){

		return 1;
	}else{
		return 0;
	}
}
int CMotionPoint2::IsSameMP( CMotionPoint2* cmpmp )
{

	if( (m_q.x == cmpmp->m_q.x) && (m_q.y == cmpmp->m_q.y) && (m_q.z == cmpmp->m_q.z) && (m_q.w == cmpmp->m_q.w) &&
		(m_mvx == cmpmp->m_mvx) && (m_mvy == cmpmp->m_mvy) && (m_mvz == cmpmp->m_mvz) &&
		(m_scalex == cmpmp->m_scalex) && (m_scaley == cmpmp->m_scaley) && (m_scalez == cmpmp->m_scalez) &&
		(interp == cmpmp->interp) ){

		return 1;
	}else{
		return 0;
	}
}

int CMotionPoint2::RotBSpline( CQuaternion* axisq, CMotionPoint2* startmp, CMotionPoint2* endmp, int srcx )
{
	D3DXVECTOR2 startp[3];
	D3DXVECTOR2 endp[3];

	startp[0].x = (float)(startmp->m_frameno);
	startp[0].y = startmp->m_eul.x;
	startp[1].x = (float)(startmp->m_frameno);
	startp[1].y = startmp->m_eul.y;
	startp[2].x = (float)(startmp->m_frameno);
	startp[2].y = startmp->m_eul.z;

	endp[0].x = (float)(endmp->m_frameno);
	endp[0].y = endmp->m_eul.x;
	endp[1].x = (float)(endmp->m_frameno);
	endp[1].y = endmp->m_eul.y;
	endp[2].x = (float)(endmp->m_frameno);
	endp[2].y = endmp->m_eul.z;

	_ASSERT( startmp->m_spp );

	int ret;
	ret = CalcSPPoint( &(startmp->m_spp->m_rotparam), startp, endp );
	if( ret ){
		DbgOut( "mp2 : RotBSpline : CalcSPPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	float result[3];

	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		D3DXVECTOR2 spcenter;
		spcenter = ( startmp->m_spp->m_rotparam.ctrlS[ikind].sppoint + startmp->m_spp->m_rotparam.ctrlE[ikind].sppoint ) * 0.5f;
		if( (float)srcx < spcenter.x  ){
			result[ikind] = CalcBSpline( (float)srcx, startp[ikind], startmp->m_spp->m_rotparam.ctrlS[ikind].sppoint, spcenter );
		}else if( (float)srcx > spcenter.x  ){
			result[ikind] = CalcBSpline( (float)srcx, spcenter, startmp->m_spp->m_rotparam.ctrlE[ikind].sppoint, endp[ikind] );
		}else{
			result[ikind] = spcenter.y;
		}
	}

	CQuaternion invaxisQ;
	ret = axisq->inv( &invaxisQ );
	_ASSERT( !ret );

	CQuaternion q, qx, qy, qz;
	D3DXVECTOR3 axisX( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisY( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZ( 0.0f, 0.0f, 1.0f );
	qx.SetAxisAndRot( axisX, result[0] * (float)DEG2PAI );
	qy.SetAxisAndRot( axisY, result[1] * (float)DEG2PAI );
	qz.SetAxisAndRot( axisZ, result[2] * (float)DEG2PAI );

	q = *axisq * qy * qx * qz * invaxisQ;

	m_q = q;
	m_eul.x = result[0];
	m_eul.y = result[1];
	m_eul.z = result[2];

	return 0;
}

int CMotionPoint2::MvBSpline( CMotionPoint2* startmp, CMotionPoint2* endmp, int srcx )
{
	D3DXVECTOR2 startp[3];
	D3DXVECTOR2 endp[3];

	startp[0].x = (float)(startmp->m_frameno);
	startp[0].y = startmp->m_mvx;
	startp[1].x = (float)(startmp->m_frameno);
	startp[1].y = startmp->m_mvy;
	startp[2].x = (float)(startmp->m_frameno);
	startp[2].y = startmp->m_mvz;

	endp[0].x = (float)(endmp->m_frameno);
	endp[0].y = endmp->m_mvx;
	endp[1].x = (float)(endmp->m_frameno);
	endp[1].y = endmp->m_mvy;
	endp[2].x = (float)(endmp->m_frameno);
	endp[2].y = endmp->m_mvz;

	_ASSERT( startmp->m_spp );

	int ret;
	ret = CalcSPPoint( &(startmp->m_spp->m_mvparam), startp, endp );
	if( ret ){
		DbgOut( "mp2 : MvBSpline : CalcSPPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	float result[3];

	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		D3DXVECTOR2 spcenter;
		spcenter = ( startmp->m_spp->m_mvparam.ctrlS[ikind].sppoint + startmp->m_spp->m_mvparam.ctrlE[ikind].sppoint ) * 0.5f;
		if( (float)srcx < spcenter.x  ){
			result[ikind] = CalcBSpline( (float)srcx, startp[ikind], startmp->m_spp->m_mvparam.ctrlS[ikind].sppoint, spcenter );
		}else if( (float)srcx > spcenter.x  ){
			result[ikind] = CalcBSpline( (float)srcx, spcenter, startmp->m_spp->m_mvparam.ctrlE[ikind].sppoint, endp[ikind] );
		}else{
			result[ikind] = spcenter.y;
		}
	}
	m_mvx = result[0];
	m_mvy = result[1];
	m_mvz = result[2];

	return 0;
}

int CMotionPoint2::ScBSpline( CMotionPoint2* startmp, CMotionPoint2* endmp, int srcx )
{
	D3DXVECTOR2 startp[3];
	D3DXVECTOR2 endp[3];

	startp[0].x = (float)(startmp->m_frameno);
	startp[0].y = startmp->m_scalex;
	startp[1].x = (float)(startmp->m_frameno);
	startp[1].y = startmp->m_scaley;
	startp[2].x = (float)(startmp->m_frameno);
	startp[2].y = startmp->m_scalez;

	endp[0].x = (float)(endmp->m_frameno);
	endp[0].y = endmp->m_scalex;
	endp[1].x = (float)(endmp->m_frameno);
	endp[1].y = endmp->m_scaley;
	endp[2].x = (float)(endmp->m_frameno);
	endp[2].y = endmp->m_scalez;

	_ASSERT( startmp->m_spp );

	int ret;
	ret = CalcSPPointScale( &(startmp->m_spp->m_scparam), startp, endp );
	if( ret ){
		DbgOut( "mp2 : MvBSpline : CalcSPPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	float result[3];

	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		D3DXVECTOR2 spcenter;
		spcenter = ( startmp->m_spp->m_scparam.ctrlS[ikind].sppoint + startmp->m_spp->m_scparam.ctrlE[ikind].sppoint ) * 0.5f;
		if( (float)srcx < spcenter.x  ){
			result[ikind] = CalcBSpline( (float)srcx, startp[ikind], startmp->m_spp->m_scparam.ctrlS[ikind].sppoint, spcenter );
		}else if( (float)srcx > spcenter.x  ){
			result[ikind] = CalcBSpline( (float)srcx, spcenter, startmp->m_spp->m_scparam.ctrlE[ikind].sppoint, endp[ikind] );
		}else{
			result[ikind] = spcenter.y;
		}
	}
	m_scalex = result[0];
	m_scaley = result[1];
	m_scalez = result[2];

	return 0;
}
int CMotionPoint2::CalcSPPoint( SPPARAM* spp, D3DXVECTOR2* startp, D3DXVECTOR2* endp )
{

	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		float slope0;
		_ASSERT( ( (endp + ikind)->x - (startp + ikind)->x ) != 0.0f );
		slope0 = ( (endp + ikind)->y - (startp + ikind)->y ) / ( (endp + ikind)->x - (startp + ikind)->x );

		D3DXVECTOR2 slopevec1;
		if( slope0 != 0.0f ){
			float slope1;
			slope1 = -1.0f / slope0;

			if( slope1 >= 0.0f ){
				slopevec1.x = 1.0f;
				slopevec1.y = slope1;
			}else{
				slopevec1.x = -1.0f;
				slopevec1.y = -slope1;
			}
			D3DXVec2Normalize( &slopevec1, &slopevec1 );
		}else{
			slopevec1 = D3DXVECTOR2( 0.0f, 1.0f );
		}


		D3DXVECTOR2 pointS0;
//		pointS0 = (*(endp + ikind) - *(startp + ikind)) * spp->ctrlS[ikind].spt + *(startp + ikind);
		pointS0 = ( 1.0f - spp->ctrlS[ikind].spt ) * *(startp + ikind) + spp->ctrlS[ikind].spt * *(endp + ikind);
		spp->ctrlS[ikind].sppoint = pointS0 + spp->ctrlS[ikind].spdist * slopevec1;
		
		D3DXVECTOR2 pointE0;
//		pointE0 = (*(endp + ikind) - *(startp + ikind)) * spp->ctrlE[ikind].spt + *(startp + ikind);
		pointE0 = ( 1.0f - spp->ctrlE[ikind].spt ) * *(startp + ikind) + spp->ctrlE[ikind].spt * *(endp + ikind);
		spp->ctrlE[ikind].sppoint = pointE0 + spp->ctrlE[ikind].spdist * slopevec1;
	}

	return 0;
}

int CMotionPoint2::CalcSPPointScale( SPPARAM* spp, D3DXVECTOR2* startp, D3DXVECTOR2* endp )
{

	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		float slope0;
		_ASSERT( ( (endp + ikind)->x - (startp + ikind)->x ) != 0.0f );
		slope0 = ( (endp + ikind)->y - (startp + ikind)->y ) / ( (endp + ikind)->x - (startp + ikind)->x )* 500.0f;

		D3DXVECTOR2 slopevec1;
		if( slope0 != 0.0f ){
			float slope1;
			slope1 = -1.0f / slope0;

			if( slope1 >= 0.0f ){
				slopevec1.x = 1.0f;
				slopevec1.y = slope1;
			}else{
				slopevec1.x = -1.0f;
				slopevec1.y = -slope1;
			}
			D3DXVec2Normalize( &slopevec1, &slopevec1 );

//if( (ikind == INDX) && ((startp + ikind)->x == 0) ){
//	DbgOut( "mp2 : CalpSPPoint : indx frame0 --> slope0 %f, slope1(%f, %f)\r\n",
//		slope0, slopevec1.x, slopevec1.y );
//}
		}else{
			slopevec1 = D3DXVECTOR2( 0.0f, 1.0f );
		}


		D3DXVECTOR2 pointS0;
//		pointS0 = (*(endp + ikind) - *(startp + ikind)) * spp->ctrlS[ikind].spt + *(startp + ikind);
		pointS0 = ( 1.0f - spp->ctrlS[ikind].spt ) * *(startp + ikind) + spp->ctrlS[ikind].spt * *(endp + ikind);
		spp->ctrlS[ikind].sppoint = pointS0 + spp->ctrlS[ikind].spdist * slopevec1;
		
		D3DXVECTOR2 pointE0;
//		pointE0 = (*(endp + ikind) - *(startp + ikind)) * spp->ctrlE[ikind].spt + *(startp + ikind);
		pointE0 = ( 1.0f - spp->ctrlE[ikind].spt ) * *(startp + ikind) + spp->ctrlE[ikind].spt * *(endp + ikind);
		spp->ctrlE[ikind].sppoint = pointE0 + spp->ctrlE[ikind].spdist * slopevec1;
	}

	return 0;
}

int CMotionPoint2::ScaleSplineMv( CMotionPoint2* endmp, D3DXVECTOR3 srcmult )
{
	if( !m_spp || !(endmp->m_spp) ){
		return 0;
	}

	SPPARAM* spp = &( m_spp->m_mvparam );

	D3DXVECTOR2 P0[3], P1[3], ps[3], pe[3];
	D3DXVECTOR2 As, Bs, Cs, Ds;
	D3DXVECTOR2 Ae, Be, Ce, De;
	float dotabs, magbs;
	float dotabe, magbe;
	float sigcs, sigds;
	float sigce, sigde;

	P0[INDX].x = (float)m_frameno;
	P0[INDX].y = m_mvx;
	P0[INDY].x = (float)m_frameno;
	P0[INDY].y = m_mvy;
	P0[INDZ].x = (float)m_frameno;
	P0[INDZ].y = m_mvz;

	P1[INDX].x = (float)( endmp->m_frameno );
	P1[INDX].y = endmp->m_mvx;
	P1[INDY].x = (float)( endmp->m_frameno );
	P1[INDY].y = endmp->m_mvy;
	P1[INDZ].x = (float)( endmp->m_frameno );
	P1[INDZ].y = endmp->m_mvz;

	ps[INDX].x = spp->ctrlS[INDX].sppoint.x;
	ps[INDY].x = spp->ctrlS[INDY].sppoint.x;
	ps[INDZ].x = spp->ctrlS[INDZ].sppoint.x;
	ps[INDX].y = spp->ctrlS[INDX].sppoint.y * srcmult.x;
	ps[INDY].y = spp->ctrlS[INDY].sppoint.y * srcmult.y;
	ps[INDZ].y = spp->ctrlS[INDZ].sppoint.y * srcmult.z;

	pe[INDX].x = spp->ctrlE[INDX].sppoint.x;
	pe[INDY].x = spp->ctrlE[INDY].sppoint.x;
	pe[INDZ].x = spp->ctrlE[INDZ].sppoint.x;
	pe[INDX].y = spp->ctrlE[INDX].sppoint.y * srcmult.x;
	pe[INDY].y = spp->ctrlE[INDY].sppoint.y * srcmult.y;
	pe[INDZ].y = spp->ctrlE[INDZ].sppoint.y * srcmult.z;

	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		spp->ctrlS[ikind].distmax *= D3DXVec3Length( &srcmult );
		spp->ctrlS[ikind].sppoint = ps[ikind];
		spp->ctrlE[ikind].distmax *= D3DXVec3Length( &srcmult );
		spp->ctrlE[ikind].sppoint = pe[ikind];

		As = ps[ikind] - P0[ikind];
		Bs = P1[ikind] - P0[ikind];
		dotabs = As.x * Bs.x + As.y * Bs.y;
		magbs = Bs.x * Bs.x + Bs.y * Bs.y;
		Cs = dotabs / magbs * Bs;
		Ds = As - Cs;
		if( D3DXVec2Dot( &Cs, &Bs ) >= 0.0f ){
			sigcs = 1.0f;
		}else{
			sigcs = -1.0f;
		}
		if( Ds.y >= 0.0f ){
			sigds = 1.0f;
		}else{
			sigds = -1.0f;
		}
		spp->ctrlS[ikind].spt = sigcs * D3DXVec2Length( &Cs ) / D3DXVec2Length( &Bs );
		spp->ctrlS[ikind].spdist = sigds * D3DXVec2Length( &Ds );

		Ae = pe[ikind] - P0[ikind];
		Be = P1[ikind] - P0[ikind];
		dotabe = Ae.x * Be.x + Ae.y * Be.y;
		magbe = Be.x * Be.x + Be.y * Be.y;
		Ce = dotabe / magbe * Be;
		De = Ae - Ce;
		if( D3DXVec2Dot( &Ce, &Be ) >= 0.0f ){
			sigce = 1.0f;
		}else{
			sigce = -1.0f;
		}
		if( De.y >= 0.0f ){
			sigde = 1.0f;
		}else{
			sigde = -1.0f;
		}
		spp->ctrlE[ikind].spt = sigce * D3DXVec2Length( &Ce ) / D3DXVec2Length( &Be );
		spp->ctrlE[ikind].spdist = sigde * D3DXVec2Length( &De );
	}

	return 0;
}





float CMotionPoint2::CalcBSpline( float x, D3DXVECTOR2 pos1, D3DXVECTOR2 pos2, D3DXVECTOR2 pos3 )
{
	float result = 0.0f;
	float t;

	float a, b, c;
	a = pos1.x - 2.0f * pos2.x + pos3.x;
	b = -2.0f * pos1.x + 2.0f * pos2.x;
	c = pos1.x - x;

	if( a == 0.0f ){
		if( b == 0.0f ){
			t = 0.0f;
		}else{
			t = -c / b;
		}
	}else{
		float rval = b * b - 4.0f * a * c;
		if( rval > 0.0f ){
			t = ( -b + (float)sqrt( rval ) ) / ( 2.0f * a );
		}else if( rval == 0.0f ){
			t = -b / ( 2.0f * a );
		}else{
			_ASSERT( 0 );
			t = 0.0f;
		}
	}


	float t2 = 1.0f - t;
	result = t2 * t2 * pos1.y + 2.0f * t * t2 * pos2.y + t * t * pos3.y;

/***
				a= x1-2.*x2+ x3
		16		b= -2.*x1+ 2.*x2
		17		c= x1- x
		18		if( a==0 ){
		19			t= -c/b
		20		}else{
		21			t= (-b+sqrt(b*b-4.*a*c))/ (2.*a)
		22		}
		23		t2= 1.- t
		24		y= t2*t2*y1+ 2.*t*t2*y2+ t*t*y3
***/

	return result;
}

int CMotionPoint2::CalcFBXEul( CQuaternion* befq, D3DXVECTOR3 befeul, D3DXVECTOR3* reteul )
{

/***
	D3DXMATRIX rightmat;
	rightmat = srcq->MakeRotMatX();

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
***/

	CQuaternion rq;
	/***
	rq = *srcq;
	rq.w *= -1.0f;
	rq.z *= -1.0f;
	***/
	rq = m_q;
	rq.w *= -1.0f;
	rq.z *= -1.0f;

	CQuaternion axisq;
	axisq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
	qToEulerAxis( axisq, &rq, reteul );
	modifyEuler( reteul, &befeul );

	return 0;
}
