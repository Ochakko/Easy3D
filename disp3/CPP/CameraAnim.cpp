#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <CameraAnim.h>

#include <CameraKey.h>
#include <CameraSWKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CCameraAnim::CCameraAnim( char* srcname, int srcmotkind, CAMERAELEM* defcamptr, int* definterptr )
{
	InitParams();
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}else{
		ZeroMemory( m_animname, sizeof( char ) * 256 );
	}

	m_motkind = srcmotkind;
	m_defcamptr = defcamptr;
	m_definterptr = definterptr;
}

CCameraAnim::~CCameraAnim()
{
	DestroyObjs();
}

int CCameraAnim::InitParams()
{
/***
	char m_animname[256];
	int m_camerano;
	int m_frameno;
	CAMEARELEM m_curcamera;

	int m_isfirst;
	int m_maxframe;
	int m_animtype;
	int m_animstep;
	int m_stepdir;
	int m_motjump;
	NEXTMOTION m_nextmot;
	
	CCameraKey* m_firstkey[CAMERANUMMAX];
	CCameraSWKey* m_firstsw;
***/

	ZeroMemory( m_animname, sizeof( char ) * 256 );
	m_camerano = 0;
	m_frameno = 0;
	ZeroMemory( &m_curcamera, sizeof( CAMERAELEM ) );
	m_curinterp = 0;
	m_isfirst = 1;
	m_maxframe = 0;
	m_animtype = MOTION_CLAMP;
	m_animstep = 1;
	m_stepdir = 1;
	m_motjump = 0;
	ZeroMemory( &m_nextmot, sizeof( NEXTMOTION ) );

	m_nextmot.mk = -1;
	m_nextmot.befframeno = -1;
	m_nextmot.aftframeno = -1;

	ZeroMemory( m_firstkey, sizeof( CCameraKey* ) * CAMERANUMMAX );
	m_firstsw = 0;

	m_defcamptr = 0;
	m_definterptr = 0;

	InitStepFunc();

	return 0;
}

void CCameraAnim::InitStepFunc()
{
	int i;
	for( i = 0; i < MOTIONTYPEMAX; i++ ){

		switch( i ){
		case MOTION_STOP:
			StepFunc[i] = &CCameraAnim::StepStop;
			break;
		case MOTION_CLAMP:
			StepFunc[i] = &CCameraAnim::StepClamp;
			break;
		case MOTION_ROUND:
			StepFunc[i] = &CCameraAnim::StepRound;
			break;
		case MOTION_INV:
			StepFunc[i] = &CCameraAnim::StepInv;
			break;
		case MOTION_JUMP:
			StepFunc[i] = &CCameraAnim::StepJump;
			break;
		default:
			StepFunc[i] = &CCameraAnim::StepDummy;
			break;
		}

	}
}

int CCameraAnim::DestroyObjs()
{
	int camno;
	for( camno = 0; camno < CAMERANUMMAX; camno++ ){
		CCameraKey* delck = m_firstkey[ camno ];
		CCameraKey* nextck = 0;
		while( delck ){
			nextck = delck->next;
			delete delck;
			delck = nextck;
		}
	}

	CCameraSWKey* delcswk = m_firstsw;
	CCameraSWKey* nextcswk = 0;
	while( delcswk ){
		nextcswk = delcswk->next;
		delete delcswk;
		delcswk = nextcswk;
	}

	InitParams();

	return 0;
}

int CCameraAnim::ExistCameraKey( int camno, int frameno, CCameraKey** ppck )
{
	*ppck = 0;

	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : ExistCameraKey : camno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : ExistCameraKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CCameraKey* curck = m_firstkey[ camno ];
	while( curck && (curck->m_frameno <= frameno) ){
		if( curck->m_frameno == frameno ){
			*ppck = curck;
			break;
		}

		curck = curck->next;
	}

	return 0;
}
int CCameraAnim::CreateCameraKey( int camno, int frameno, CCameraKey** ppck )
{
	*ppck = 0;

	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : CreateCameraKey : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : CreateCameraKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CCameraKey* newck = 0;
	newck = new CCameraKey();
	if( !newck ){
		DbgOut( "CameraAnim : CreateCameraKey : newck alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newck->m_frameno = frameno;


//////////////////
	int ret;
	CCameraKey* ckhead = m_firstkey[ camno ];
	CCameraKey* curck = ckhead;
	CCameraKey* largerck = 0;
	CCameraKey* smallerck = 0;
	int cmp;
	while( curck ){
		cmp = newck->FramenoCmp( curck );
		if( cmp < 0 ){
			largerck = curck;
			break;
		}else{
			smallerck = curck;
		}
		curck = curck->next;
	}

	if( !ckhead ){
		//先頭に追加
		m_firstkey[ camno ] = newck;
	}else{
		if( largerck ){
			//largerckの前に追加。
			ret = largerck->AddToPrev( newck );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newck->prev )
				m_firstkey[ camno ] = newck;
		}else{
			//最後に追加。(smallerck の後に追加)
			_ASSERT( smallerck );
			ret = smallerck->AddToNext( newck );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppck = newck;

	return 0;
}
CCameraKey* CCameraAnim::GetFirstCameraKey( int camno )
{

	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : GetFirstCameraKey : camno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	return m_firstkey[ camno ];
}
int CCameraAnim::DeleteCameraKey( int camno, int frameno )
{
	CCameraKey* delck = 0;
	delck = LeaveFromChainCameraKey( camno, frameno );
	if( delck ){
		delete delck;
	}
	
	return 0;
}
CCameraKey* CCameraAnim::LeaveFromChainCameraKey( int camno, int frameno )
{
	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : LeaveFromChainCameraKey : camno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : LeaveFromChainCameraKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	int ret;
	CCameraKey* curck = 0;
	ret = ExistCameraKey( camno, frameno, &curck );
	if( ret ){
		return 0;
	}

	if( curck ){
		CCameraKey* firstck;
		firstck = m_firstkey[ camno ];
		if( firstck == curck ){
			m_firstkey[ camno ] = curck->next;
		}

		curck->LeaveFromChain();
	}

	return curck;
}
int CCameraAnim::ChainCameraKey( int camno, CCameraKey* addck )
{
	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : ChainCameraKey : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (addck->m_frameno < 0) || (addck->m_frameno > m_maxframe) ){
		DbgOut( "CameraAnim : ChainCameraKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CCameraKey* ckhead = m_firstkey[ camno ];
	CCameraKey* curck = ckhead;
	CCameraKey* largerck = 0;
	CCameraKey* smallerck = 0;
	int cmp;
	while( curck ){
		cmp = addck->FramenoCmp( curck );
		if( cmp < 0 ){
			largerck = curck;
			break;
		}else{
			smallerck = curck;
		}
		curck = curck->next;
	}

	if( !ckhead ){
		//先頭に追加
		m_firstkey[ camno ] = addck;
	}else{
		if( largerck ){
			//largerckの前に追加。
			ret = largerck->AddToPrev( addck );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addck->prev )
				m_firstkey[ camno ] = addck;
		}else{
			//最後に追加。(smallerck の後に追加)
			_ASSERT( smallerck );
			ret = smallerck->AddToNext( addck );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}
int CCameraAnim::DeleteCameraKeyOutOfRange( int camno, int srcmaxframe )
{
	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : DeleteCameraKeyOutOfRange : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CCameraKey* ckptr = m_firstkey[ camno ];
	CCameraKey* nextck = 0;
	while( ckptr ){
		nextck = ckptr->next;
		int frameno = ckptr->m_frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteCameraKey( camno, frameno );
			if( ret ){
				DbgOut( "CameraAnim : DeleteCameraKeyOutOfRange : DeleteCameraKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		ckptr = nextck;
	}

	return 0;
}
int CCameraAnim::SetCameraAnim()
{
	int ret;
	int existflag = 0;
	ret = GetCameraAnim( &m_curcamera, &m_curinterp, m_camerano, m_frameno, &existflag );
	if( ret ){
		DbgOut( "cameraanim : SetCameraAnim : GetCameraAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CCameraAnim::SetCameraKey( int camno, int frameno, CAMERAELEM srcce, int srcinterp )
{
	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : SetCameraKey : camno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : SetCameraKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CCameraKey* pck = 0;
	ret = ExistCameraKey( camno, frameno, &pck );
	if( ret ){
		DbgOut( "CameraAnim : SetCameraKey : ExistCameraKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int setinterp;
	if( !pck ){
		ret = CreateCameraKey( camno, frameno, &pck );
		if( ret || !pck ){
			DbgOut( "CameraAnim : SetCameraKey : CreateCameraKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( srcinterp < 0 ){
			setinterp = *m_definterptr;
		}else{
			setinterp = srcinterp;
		}
	}else{
		if( srcinterp < 0 ){
			setinterp = pck->m_interp;
		}else{
			setinterp = srcinterp;
		}
	}

	_ASSERT( pck );

	ret = pck->SetCameraKey( &srcce );
	_ASSERT( !ret );
	ret = pck->SetInterp( setinterp );
	_ASSERT( !ret );

	return 0;
}

int CCameraAnim::GetCameraAnim( CAMERAELEM* ceptr, int* interpptr, int camno, int srcframeno, int* existflag )
{
	*existflag = 0;

	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : GetCameraAnim : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srcframeno < 0) || (srcframeno > m_maxframe) ){
		DbgOut( "CameraAnim : GetCameraAnim : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CCameraKey* ckhead = m_firstkey[ camno ];
	if( !ckhead ){
		*ceptr = *( m_defcamptr + camno );
		*interpptr = *m_definterptr;
		return 0;
	}

	CCameraKey* chkck = ckhead;
	CCameraKey* findck = 0;
	CCameraKey* befck = 0;
	while( chkck ){
		if( chkck->m_frameno == srcframeno ){
			findck = chkck;
		}else if( chkck->m_frameno > srcframeno ){
			break;
		}
		befck = chkck;
		chkck = chkck->next;
	}

	if( findck ){
		*ceptr = findck->m_celem;
		*interpptr = findck->m_interp;
		*existflag = 1;
	}else{
		*existflag = 0;//!!!!

		if( befck ){
			CCameraKey* nextck;
			nextck = befck->next;

			//befck, befck->next間の補間
			*interpptr = befck->m_interp;
			ret = FillUpCameraOnFrame( camno, befck, nextck, ceptr, srcframeno );
			if( ret ){
				DbgOut( "CameraAnim : FillUpCameraOnFrame error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			//init
			//_ASSERT( 0 );
			*ceptr = *( m_defcamptr + camno );
			*interpptr = *m_definterptr;
		}
	}

	return 0;
}

int CCameraAnim::FillUpCameraOnFrame( int camno, CCameraKey* srcstartck, CCameraKey* srcendck, CAMERAELEM* dstptr, int srcframe )
{
	int startframe, endframe, framenum;
	CAMERAELEM startce, endce;
	
	if( !srcstartck && !srcendck )
		return 1;

	if( srcstartck ){
		startce = srcstartck->m_celem;	
		startframe = srcstartck->m_frameno;
	}else{
		//default
		_ASSERT( srcendck );
		startframe = 0;
		endframe = srcendck->m_frameno;

		*dstptr = *( m_defcamptr + camno );

		return 0;
	}

	if( srcendck ){
		endce = srcendck->m_celem;
		endframe = srcendck->m_frameno;
		framenum = endframe - startframe;
	}else{
		//startqと同じ
		_ASSERT( srcstartck );
		//endframe = srcmpnum - 1;

		*dstptr = startce;

		return 0;
	}


	if( (srcframe < 0) || (srcframe > endframe) ){
		DbgOut( "CameraAnim : FillUpCameraOnFrame : srcframe error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	if( srcframe == startframe ){
		*dstptr = startce;
	}else if( srcframe == endframe ){
		*dstptr = endce;
	}else{
		int framecnt;
		framecnt = srcframe - startframe;
		ret = FillUpCameraElem( srcstartck, srcendck, dstptr, framenum, framecnt, srcstartck->m_interp );
		if( ret ){
			_ASSERT( 0 );
			return 0;
		}
	}

	return 0;
}

int CCameraAnim::FillUpCameraElem( CCameraKey* startck, CCameraKey* endck, CAMERAELEM* dstce, int framenum, int framecnt, int interp )
{
	CAMERAELEM* startce;
	CAMERAELEM* endce;
	startce = &( startck->m_celem );
	endce = &( endck->m_celem );

	dstce->looktype = startce->looktype;
	dstce->ortho = startce->ortho;

	int divinterp, calcinterp, calcup;
	divinterp = interp / 100;
	if( divinterp == 1 ){
		calcup = 1;
		calcinterp = interp - 100;
	}else{
		calcup = 0;
		calcinterp = interp;
	}

	if( calcinterp == INTERPOLATION_SLERP ){

		float startnear, startfar, startfov, startosize;
		float stepnear, stepfar, stepfov, steposize;
		startnear = startce->nearz;
		startfar = startce->farz;
		startfov = startce->fov;
		startosize = startce->orthosize;
		if( framenum != 0 ){
			stepnear = (endce->nearz - startnear) / (float)framenum;
			stepfar = (endce->farz - startfar) / (float)framenum;
			stepfov = (endce->fov - startfov) / (float)framenum;
			steposize = (endce->orthosize - startosize) / (float)framenum;
		}else{
			stepnear = 0.0f;
			stepfar = 0.0f;
			stepfov = 0.0f;
			steposize = 0.0f;
		}
		
		dstce->nearz = startnear + stepnear * (float)framecnt;
		dstce->farz = startfar + stepfar * (float)framecnt;
		dstce->fov = startfov + stepfov * (float)framecnt;
		dstce->orthosize = startosize + steposize * (float)framecnt;
/////////
		D3DXVECTOR3 startpos, starttar, startup;
		D3DXVECTOR3 steppos, steptar, stepup;
		startpos = startce->pos;
		starttar = startce->target;
		startup = startce->up;
		if( framenum != 0 ){
			steppos = ( endce->pos - startpos ) / (float)framenum;
			steptar = ( endce->target - starttar ) / (float)framenum;
			stepup = ( endce->up - startup ) / (float)framenum;
		}else{
			steppos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			steptar = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			stepup = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}
		
		dstce->pos = startpos + steppos * (float)framecnt;
		dstce->target = starttar + steptar * (float)framecnt;
		if( calcup == 1 ){
			dstce->up = startup + stepup * (float)framecnt;
		}else{
			dstce->up = startup;//補間なし
		}
		D3DXVec3Normalize( &dstce->up, &dstce->up );

		D3DXVECTOR3 diff;
		diff = dstce->target - dstce->pos;
		dstce->dist = D3DXVec3Length( &diff );

	}else{
		int ret;
		CAMERAELEM* befce;
		CAMERAELEM* aftce;
		
		if( startck->prev ){
			befce = &( startck->prev->m_celem );
		}else{
			befce = startce;
		}

		if( endck->next ){
			aftce = &( endck->next->m_celem );
		}else{
			aftce = endce;
		}
	
		float t;
		t = (float)framecnt / (float)framenum;

		ret = SplineVec3( befce->pos, startce->pos, endce->pos, aftce->pos, &( dstce->pos ), t );
		_ASSERT( !ret );

		ret = SplineVec3( befce->target, startce->target, endce->target, aftce->target, &( dstce->target ), t );
		_ASSERT( !ret );

		if( calcup == 1 ){
			ret = SplineVec3( befce->up, startce->up, endce->up, aftce->up, &( dstce->up ), t );
			_ASSERT( !ret );
		}else{
			dstce->up = startce->up;//補間なし
		}
		D3DXVec3Normalize( &dstce->up, &dstce->up );

		D3DXVECTOR3 diff;
		diff = dstce->target - dstce->pos;
		dstce->dist = D3DXVec3Length( &diff );

		ret = SplineF( befce->nearz, startce->nearz, endce->nearz, aftce->nearz, &( dstce->nearz ), t );
		_ASSERT( !ret );

		ret = SplineF( befce->farz, startce->farz, endce->farz, aftce->farz, &( dstce->farz ), t );
		_ASSERT( !ret );

		ret = SplineF( befce->fov, startce->fov, endce->fov, aftce->fov, &( dstce->fov ), t );
		_ASSERT( !ret );

		ret = SplineF( befce->orthosize, startce->orthosize, endce->orthosize, aftce->orthosize, &( dstce->orthosize ), t );
		_ASSERT( !ret );

	}
	
/***
	int looktype;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;
	float dist;

	int hsid;
	int boneno;

	float nearz;
	float farz;
	float fov;
***/

	return 0;
}

int CCameraAnim::SplineF( float val1, float val2, float val3, float val4, float* dstptr, float t )
{
	const float alpha = 0.0f;
	float m0, m1;

	m0 = ( 1.0f - alpha ) / 2.0f * ( ( val2 - val1 ) + ( val3 - val2 ) );
	m1 = ( 1.0f - alpha ) / 2.0f * ( ( val3 - val2 ) + ( val4 - val3 ) );

	float t2 = t * t;
	float t3 = t2 * t;

	float resval;

	resval = (((2 * t3) - (3 * t2) + 1) * val2) +
                     ((t3 - (2 * t2) + t) * m0) +
                     ((t3 - t2) * m1) +
                     (((-2 * t3) + (3 * t2)) * val3);

	*dstptr = resval;
	return 0;
}

int CCameraAnim::SplineVec3( D3DXVECTOR3 befv, D3DXVECTOR3 startv, D3DXVECTOR3 endv, D3DXVECTOR3 aftv, D3DXVECTOR3* dstv, float t )
{
	int ret;
	ret = SplineF( befv.x, startv.x, endv.x, aftv.x, &(dstv->x), t );
	_ASSERT( !ret );

	ret = SplineF( befv.y, startv.y, endv.y, aftv.y, &(dstv->y), t );
	_ASSERT( !ret );

	ret = SplineF( befv.z, startv.z, endv.z, aftv.z, &(dstv->z), t );
	_ASSERT( !ret );

	return 0;
}



int CCameraAnim::ExistSWKey( int frameno, CCameraSWKey** ppcswk )
{
	*ppcswk = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : ExistSWKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CCameraSWKey* curcswk = m_firstsw;
	while( curcswk && (curcswk->m_frameno <= frameno) ){
		if( curcswk->m_frameno == frameno ){
			*ppcswk = curcswk;
			break;
		}

		curcswk = curcswk->next;
	}

	return 0;
}
int CCameraAnim::CreateSWKey( int frameno, CCameraSWKey** ppcswk )
{
	*ppcswk = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : CreateSWKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CCameraSWKey* newcswk = 0;
	newcswk = new CCameraSWKey();
	if( !newcswk ){
		DbgOut( "CameraAnim : CreateSWKey : newcswk alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newcswk->m_frameno = frameno;


//////////////////
	int ret;
	CCameraSWKey* cswkhead = m_firstsw;
	CCameraSWKey* curcswk = cswkhead;
	CCameraSWKey* largercswk = 0;
	CCameraSWKey* smallercswk = 0;
	int cmp;
	while( curcswk ){
		cmp = newcswk->FramenoCmp( curcswk );
		if( cmp < 0 ){
			largercswk = curcswk;
			break;
		}else{
			smallercswk = curcswk;
		}
		curcswk = curcswk->next;
	}

	if( !cswkhead ){
		//先頭に追加
		m_firstsw = newcswk;
	}else{
		if( largercswk ){
			//largercswkの前に追加。
			ret = largercswk->AddToPrev( newcswk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newcswk->prev )
				m_firstsw = newcswk;
		}else{
			//最後に追加。(smallercswk の後に追加)
			_ASSERT( smallercswk );
			ret = smallercswk->AddToNext( newcswk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppcswk = newcswk;

	return 0;
}
CCameraSWKey* CCameraAnim::GetFirstSWKey()
{
	return m_firstsw;
}
int CCameraAnim::DeleteSWKey( int frameno )
{
	CCameraSWKey* delcswk = 0;
	delcswk = LeaveFromChainSWKey( frameno );
	if( delcswk ){
		delete delcswk;
	}

	return 0;
}
CCameraSWKey* CCameraAnim::LeaveFromChainSWKey( int frameno )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : LeaveFromChainSWKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	int ret;
	CCameraSWKey* curcswk = 0;
	ret = ExistSWKey( frameno, &curcswk );
	if( ret ){
		return 0;
	}

	if( curcswk ){
		CCameraSWKey* firstcswk;
		firstcswk = m_firstsw;
		if( firstcswk == curcswk ){
			m_firstsw = curcswk->next;
		}

		curcswk->LeaveFromChain();
	}

	return curcswk;

}
int CCameraAnim::ChainSWKey( CCameraSWKey* addcswk )
{
	if( (addcswk->m_frameno < 0) || (addcswk->m_frameno > m_maxframe) ){
		DbgOut( "CameraAnim : ChainSWKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CCameraSWKey* cswkhead = m_firstsw;
	CCameraSWKey* curcswk = cswkhead;
	CCameraSWKey* largercswk = 0;
	CCameraSWKey* smallercswk = 0;
	int cmp;
	while( curcswk ){
		cmp = addcswk->FramenoCmp( curcswk );
		if( cmp < 0 ){
			largercswk = curcswk;
			break;
		}else{
			smallercswk = curcswk;
		}
		curcswk = curcswk->next;
	}

	if( !cswkhead ){
		//先頭に追加
		m_firstsw = addcswk;
	}else{
		if( largercswk ){
			//largerckの前に追加。
			ret = largercswk->AddToPrev( addcswk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addcswk->prev )
				m_firstsw = addcswk;
		}else{
			//最後に追加。(smallerck の後に追加)
			_ASSERT( smallercswk );
			ret = smallercswk->AddToNext( addcswk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
int CCameraAnim::DeleteSWKeyOutOfRange( int srcmaxframe )
{
	int ret;
	CCameraSWKey* cswkptr = m_firstsw;
	CCameraSWKey* nextcswk = 0;
	while( cswkptr ){
		nextcswk = cswkptr->next;
		int frameno = cswkptr->m_frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteSWKey( frameno );
			if( ret ){
				DbgOut( "CameraAnim : DeleteSWKeyOutOfRange : DeleteSWKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		cswkptr = nextcswk;
	}

	return 0;
}
int CCameraAnim::SetSWAnim()
{
	int ret;
	int existflag = 0;
	ret = GetSWAnim( &m_camerano, m_frameno, &existflag );
	if( ret ){
		DbgOut( "CameraAnim : SetSWAnim : GetSWAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CCameraAnim::SetSWKey( int frameno, int srccamerano )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : SetSWKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srccamerano < 0) || (srccamerano >= CAMERANUMMAX) ){
		DbgOut( "CameraAnim : SetSWKey : camerano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CCameraSWKey* pcswk = 0;
	ret = ExistSWKey( frameno, &pcswk );
	if( ret ){
		DbgOut( "CameraAnim : SetSWKey : ExistSWKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !pcswk ){
		ret = CreateSWKey( frameno, &pcswk );
		if( ret || !pcswk ){
			DbgOut( "CameraAnim : SetSWKey : CreateSWKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	_ASSERT( pcswk );

	pcswk->m_camerano = srccamerano;
	
	return 0;
}

int CCameraAnim::GetSWAnim( int* camnoptr, int frameno, int* existflag )
{
	*existflag = 0;
	*camnoptr = 0;

	CCameraSWKey* cswkptr;
	cswkptr = m_firstsw;

	while( cswkptr ){
		if( cswkptr->m_frameno <= frameno ){
			*camnoptr = cswkptr->m_camerano;
			if( cswkptr->m_frameno == frameno ){
				*existflag = 1;
			}
		}else{
			break;
		}
		cswkptr = cswkptr->next;
	}

	return 0;
}


int CCameraAnim::SetCameraAnimNo( int camno, int frameno, int srcisfirst )
{
	int ret;
	if( camno >= CAMERANUMMAX ){
		DbgOut( "CameraAnim : SetCameraAnimNo : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : SetCameraAnimNo : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( camno < 0 ){
		int existflag = 0;
		ret = GetSWAnim( &m_camerano, frameno, &existflag );
		if( ret ){
			DbgOut( "CameraAnim : SetCameraAnimNo : GetSWAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		m_camerano = camno;
	}
	m_frameno = frameno;
	m_isfirst = srcisfirst;

	int existflag = 0;
	ret = GetCameraAnim( &m_curcamera, &m_curinterp, m_camerano, m_frameno, &existflag );
	if( ret ){
		DbgOut( "CameraAnim : SetCameraAnimNo : GetCameraAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CCameraAnim::GetNextCamera( NEXTMOTION* pnm, int getonly )
{
	if( m_motkind < 0 ){
		pnm->mk = -1;
		pnm->aftframeno = 0;
		return 0;
	}

	int ret;

	if( m_isfirst == 0 ){
		ret = (this->*StepFunc[m_animtype])( pnm );
		_ASSERT( !ret );
	}else{
		if( getonly == 0 ){
			m_isfirst = 0;
		}
		pnm->mk = m_motkind;
		pnm->aftframeno = m_frameno;
	}
	
	return 0;
}

int CCameraAnim::StepStop( NEXTMOTION* pnm )
{
	pnm->mk = m_motkind;
	pnm->aftframeno = m_frameno;
	return 0;
}
int CCameraAnim::StepClamp( NEXTMOTION* pnm )
{
	int nextno;	
	nextno = m_frameno + m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = m_maxframe;
		}else{
			*pnm = m_nextmot;
		}
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = 0;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}

	return 0;
}
int CCameraAnim::StepRound( NEXTMOTION* pnm )
{
	int nextno;
	nextno = m_frameno + m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = 0;
		}else{
			*pnm = m_nextmot;
		}
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = m_maxframe;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}
	
	return 0;
}
int CCameraAnim::StepInv( NEXTMOTION* pnm )
{
	int nextno;
		nextno = m_frameno + m_stepdir * m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = m_maxframe;
		}else{
			*pnm = m_nextmot;
		}
		m_stepdir *= -1;
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = 0;
		m_stepdir *= -1;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}

	return 0;
}
int CCameraAnim::StepJump( NEXTMOTION* pnm )
{
	int nextno;
	nextno = m_frameno + m_animstep;

	if( nextno > m_maxframe ){
		if( m_nextmot.mk == -1 ){
			pnm->mk = m_motkind;
			pnm->aftframeno = m_motjump;
		}else{
			*pnm = m_nextmot;
		}
	}else if( nextno < 0 ){
		pnm->mk = m_motkind;
		pnm->aftframeno = m_maxframe;
	}else{
		if( (m_nextmot.mk != -1) && (m_nextmot.befframeno != -1) && (nextno > m_nextmot.befframeno) ){
			*pnm = m_nextmot;
		}else{
			pnm->mk = m_motkind;
			pnm->aftframeno = nextno;
		}
	}
	
	return 0;
}

int CCameraAnim::StepDummy( NEXTMOTION* pnm )	
{
	DbgOut( "CCameraAnim : StepFunc called StepDummy warning !!!\n" );
	pnm->mk = m_motkind;
	pnm->aftframeno = m_frameno;
	return 0;
}


int CCameraAnim::GetCameraKeyframeNoRange( int camno, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : GetCameraKeyframeNoRange : camno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int setno = 0;
	CCameraKey* curck = m_firstkey[ camno ];
	while( curck ){
		if( (curck->m_frameno >= srcstart) && (curck->m_frameno <= srcend) ){
			if( framearray && (arrayleng > 0) ){
				if( setno >= arrayleng ){
					DbgOut( "CameraAnim : GetCameraKeyframeNoRange : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( framearray + setno ) = curck->m_frameno;
			}
			setno++;
		}
		curck = curck->next;
	}

	*framenumptr = setno;

	return 0;
}
int CCameraAnim::GetSWKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	int setno = 0;
	CCameraSWKey* curcswk = m_firstsw;
	while( curcswk ){
		if( (curcswk->m_frameno >= srcstart) && (curcswk->m_frameno <= srcend) ){
			if( framearray && (arrayleng > 0) ){
				if( setno >= arrayleng ){
					DbgOut( "CameraAnim : GetSWKeyframeNoRange : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( framearray + setno ) = curcswk->m_frameno;
			}
			setno++;
		}
		curcswk = curcswk->next;
	}

	*framenumptr = setno;

	return 0;
}

int CCameraAnim::SetAnimName( char* srcname )
{
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}else{
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CCameraAnim::GetAnimName( char* dstname )
{
	strcpy_s( dstname, 256, m_animname );
	return 0;
}

int CCameraAnim::StepCamera( int camno, int frameno )
{
	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : StepCamera : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "CameraAnim : StepCamera : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_camerano = camno;
	m_frameno = frameno;

	int ret;
	int existflag = 0;
	ret = GetCameraAnim( &m_curcamera, &m_curinterp, m_camerano, m_frameno, &existflag );
	if( ret ){
		DbgOut( "CameraAnim : StepCamera : GetCameraAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CCameraAnim::SetFillUpMode( int camno, int frameno, int setfu )
{
	if( (camno >= CAMERANUMMAX) || (camno < 0) ){
		DbgOut( "CameraAnim : SetFullUpMode : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	if( frameno >= 0 ){
		CCameraKey* ckptr = 0;
		ret = ExistCameraKey( camno, frameno, &ckptr );
		if( ret ){
			DbgOut( "CameraAnim : SetFillUpMode : ExistCameraKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( ckptr ){
			ckptr->m_interp = setfu;
		}

	}else{
		CCameraKey* ckptr;
		ckptr = GetFirstCameraKey( camno );
		while( ckptr ){
			ckptr->m_interp = setfu;
			ckptr = ckptr->next;
		}
	}

	return 0;
}

int CCameraAnim::SetTargetHsidAll( int srchsid )
{
		CCameraKey* ckptr;

	int camno;
	for( camno = 0; camno < CAMERANUMMAX; camno++ ){
		ckptr = GetFirstCameraKey( camno );
		while( ckptr ){
			ckptr->m_celem.hsid = srchsid;
			ckptr = ckptr->next;
		}
	}

	return 0;
}
