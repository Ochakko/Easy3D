#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <CameraAnimHandler.h>

#include <CameraAnim.h>
#include <CameraKey.h>
#include <CameraSWKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

static int s_animcnt = 0;

CCameraAnimHandler::CCameraAnimHandler()
{
	InitParams();
}
CCameraAnimHandler::~CCameraAnimHandler()
{
	DestroyObjs();
}

int CCameraAnimHandler::InitParams()
{
/***
int m_animnum;
int m_animno;
CCameraAnim** m_ppcamanim;
int m_definterp;
***/
	m_animnum = 0;
	m_animno = 0;
	m_ppcamanim = 0;
	m_definterp = INTERPOLATION_SLERP;


	int cno;
	for( cno = 0; cno < 3; cno++ ){
		m_defaultcameras[cno].looktype = CAML_NORMAL;
		m_defaultcameras[cno].pos = D3DXVECTOR3( 0.0f, 0.0f, -5000.0f );
		m_defaultcameras[cno].target = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_defaultcameras[cno].up = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		m_defaultcameras[cno].dist = 5000.0f;
		m_defaultcameras[cno].hsid = -1;
		m_defaultcameras[cno].boneno = -1;
		m_defaultcameras[cno].nearz = 1000.0f;
		m_defaultcameras[cno].farz = 40000.0f;
		m_defaultcameras[cno].fov = 45.0f;
		m_defaultcameras[cno].ortho = 0;
		m_defaultcameras[cno].orthosize = 3000.0f;
	}


	return 0;
}
int CCameraAnimHandler::DestroyObjs()
{
	if( m_ppcamanim ){
		int animno;
		for( animno = 0; animno < m_animnum; animno++ ){
			CCameraAnim* delanim;
			delanim = *( m_ppcamanim + animno );
			if( delanim ){
				delete delanim;
			}
		}
		free( m_ppcamanim );
		m_ppcamanim = 0;
	}
	m_animnum = 0;
	m_animno = 0;

	return 0;
}
CCameraAnim* CCameraAnimHandler::GetCameraAnimByIndex( int srcindex )
{
	if( (srcindex < 0) || (srcindex >= m_animnum) ){
		_ASSERT( 0 );
		return 0;
	}

	return *( m_ppcamanim + srcindex );
}
CCameraAnim* CCameraAnimHandler::GetCameraAnim( int animno, int* indexptr )
{
	CCameraAnim* retanim = 0;
	if( indexptr ){
		*indexptr = -1;
	}

	int canino;
	for( canino = 0; canino < m_animnum; canino++ ){
		CCameraAnim* curanim;
		curanim = *( m_ppcamanim + canino );
		if( curanim && (curanim->m_motkind == animno) ){
			retanim = curanim;
			if( indexptr ){
				*indexptr = canino;
			}
			break;
		}
	}

	return retanim;
}
CCameraAnim* CCameraAnimHandler::GetCurCameraAnim()
{
	if( (m_animnum <= 0) || (m_animno < 0) ){
		return 0;
	}
	CCameraAnim* retanim;
	retanim = GetCameraAnim( m_animno );
	return retanim;
}

CCameraAnim* CCameraAnimHandler::AddAnim( char* animname, int animtype, int framenum, int srcmotjump )
{
	CCameraAnim* newanim = 0;

	int newnum = m_animnum + 1;
	m_ppcamanim = (CCameraAnim**)realloc( m_ppcamanim, sizeof( CCameraAnim* ) * newnum );
	if( !m_ppcamanim ){
		DbgOut( "CAH : AddAnim : ppcamanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	*( m_ppcamanim + m_animnum ) = 0;

	s_animcnt++;
	newanim = new CCameraAnim( animname, s_animcnt, m_defaultcameras, &m_definterp );
	if( !newanim ){
		DbgOut( "CAH : AddAnim : newanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int setframenum = max( framenum, 1 );

	newanim->m_maxframe = setframenum - 1;
	newanim->m_animtype = animtype;
	newanim->m_motjump = srcmotjump;

	*( m_ppcamanim + m_animnum ) = newanim;
	m_animnum = newnum;

	return newanim;
}
int CCameraAnimHandler::DestroyAnim( int delanimno )
{
	if( !m_ppcamanim || (m_animnum <= 0) ){
		return 0;
	}

	if( delanimno < 0 ){
		DestroyObjs();
	}else{
		
		CCameraAnim* delanim = 0;
		int delindex = -1;
		int canino;
		for( canino = 0; canino < m_animnum; canino++ ){
			CCameraAnim* curanim;
			curanim = *( m_ppcamanim + canino );
			if( curanim && (curanim->m_motkind == delanimno) ){
				delanim = curanim;
				delindex = canino;
				break;
			}
		}

		if( delanim && (delindex >= 0) && (m_animnum > 0) ){
			int newnum = m_animnum - 1;

			CCameraAnim** newppanim;
			newppanim = (CCameraAnim**)malloc( sizeof( CCameraAnim* ) * newnum );
			if( !newppanim ){
				DbgOut( "CAH : DestroyAnim : newppanim alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( newppanim, sizeof( CCameraAnim* ) * newnum );

			int setno = 0;
			int canino;
			for( canino = 0; canino < m_animnum; canino++ ){
				if( canino != delindex ){
					*( newppanim + setno ) = *( m_ppcamanim + canino );
					setno++;
				}
			}
			if( setno != newnum ){
				DbgOut( "CAH : DestroyAnim : setno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			delete delanim;
			free( m_ppcamanim );
			m_ppcamanim = newppanim;
			m_animnum = newnum;
		}
	}

	return 0;

}

int CCameraAnimHandler::SetTotalFrame( int animno, int newtotal )
{
	int ret;
	CCameraAnim* curanim;
	curanim = GetCameraAnim( animno );
	if( !curanim ){
		DbgOut( "CAH : SetTotalFrame : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal == (curanim->m_maxframe + 1) ){
		return 0;
	}

	if( newtotal < 1 ){
		DbgOut( "CAH : SetTotalFrame : totalframenum must be larger than 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal < (curanim->m_maxframe + 1) ){
		int camno;
		for( camno = 0; camno < CAMERANUMMAX; camno++ ){
			ret = curanim->DeleteCameraKeyOutOfRange( camno, newtotal - 1 );
			if( ret ){
				DbgOut( "CAH : SetTotalFrame : ca DeleteCameraKeyOutOfRange error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = curanim->DeleteSWKeyOutOfRange( newtotal - 1 );
			if( ret ){
				DbgOut( "CAH : SetTotalFrame : ca DeleteSWKeyOutOfRange error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	curanim->m_maxframe = newtotal - 1;

	return 0;
}

int CCameraAnimHandler::SetCurrentAnim( int animno )
{
	int ret;
	CCameraAnim* curanim;
	curanim = GetCameraAnim( animno );
	if( !curanim ){
		DbgOut( "CAH : SetCurrentAnim : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;
	int existflag = 0;
	int camno = 0;
	ret = curanim->GetSWAnim( &camno, 0, &existflag );
	_ASSERT( !ret );
	ret = curanim->SetCameraAnimNo( camno, 0, 1 );
	if( ret ){
		DbgOut( "CAH : SetCuurentAnim : ca SetCameraAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CCameraAnimHandler::GetCurrentAnim( int* animnoptr )
{
	int retno = -1;
	if( m_animnum > 0 ){
		retno = m_animno;
	}else{
		retno = -1;
	}
	*animnoptr = retno;

	return 0;
}

int CCameraAnimHandler::SetDefInterp( int srcinterp )
{
	if( (srcinterp >= INTERPOLATION_SLERP) && (srcinterp < INTERPOLATION_MAX) ){
		m_definterp = srcinterp;
	}else{
		_ASSERT( 0 );
		m_definterp = INTERPOLATION_SLERP;
	}
	return 0;
}
int CCameraAnimHandler::GetDefInterp( int* dstinterp )
{
	*dstinterp = m_definterp;
	return 0;
}
int CCameraAnimHandler::SetNextAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno )
{
	CCameraAnim* curanim;
	curanim = GetCameraAnim( animno );
	if( !curanim ){
		DbgOut( "CAH : SetNextAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CCameraAnim* nextanim;
	nextanim = GetCameraAnim( nextanimno );
	if( !nextanim ){
		DbgOut( "CAH : SetNextAnimFrameNo : nextanimno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	curanim->m_nextmot.mk = nextanimno;
	curanim->m_nextmot.aftframeno = nextframeno;
	curanim->m_nextmot.befframeno = befframeno;

	return 0;
}
int CCameraAnimHandler::GetNextAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr )
{
	CCameraAnim* curanim;
	curanim = GetCameraAnim( animno );
	if( !curanim ){
		DbgOut( "CAH : GetNextAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*nextanimptr = curanim->m_nextmot.mk;
	*nextframenoptr = curanim->m_nextmot.aftframeno;

	return 0;
}

int CCameraAnimHandler::SetAnimFrameNo( int animno, int camno, int srcframeno, int srcisfirst )
{
	int ret;
	CCameraAnim* curanim;
	curanim = GetCameraAnim( animno );
	if( !curanim ){
		DbgOut( "CAH : SetAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;

	int curcamera = 0;
	if( camno < 0 ){
		int existflag = 0;
		ret = curanim->GetSWAnim( &curcamera, srcframeno, &existflag );
		if( ret ){
			DbgOut( "CAH : SetAnimFrameNo : ca GetSWAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curcamera = camno;
	}

	ret = curanim->SetCameraAnimNo( curcamera, srcframeno, srcisfirst );
	if( ret ){
		DbgOut( "CAH : SetAnimFrameNo : ca SetCameraAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_curcamera = curanim->m_curcamera;

	return 0;
}
int CCameraAnimHandler::GetAnimFrameNo( int* animnoptr, int* camnoptr, int* framenoptr )
{
	if( m_animnum > 0 ){
		CCameraAnim* curanim;
		curanim = GetCurCameraAnim();
		if( curanim ){
			*animnoptr = m_animno;
			*camnoptr = curanim->m_camerano;
			*framenoptr = curanim->m_frameno;
		}else{
			*animnoptr = -1;
			*camnoptr = -1;
			*framenoptr = -1;			
		}
	}else{
		*animnoptr = -1;
		*camnoptr = -1;
		*framenoptr = -1;
	}

	return 0;
}

int CCameraAnimHandler::SetNewCamera( int camno )
{
	int ret;
	if( m_animnum <= 0 ){
		if( (camno >= 0) && (camno <= 3) ){
			m_curcamera = m_defaultcameras[camno];
		}else{
			m_curcamera = m_defaultcameras[0];
		}
		return 0;
	}

	CCameraAnim* curanim;
	curanim = GetCurCameraAnim();
	if( !curanim ){
		if( (camno >= 0) && (camno <= 3) ){
			m_curcamera = m_defaultcameras[camno];
		}else{
			m_curcamera = m_defaultcameras[0];
		}
		return 0;
	}


	NEXTMOTION nextmot;
	ret = curanim->GetNextCamera( &nextmot, 0 );
	if( ret ){
		DbgOut( "CAH : SetNewCamera : ca GetNextCamera error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( nextmot.mk != m_animno ){
		CCameraAnim* nextanim;
		nextanim = GetCameraAnim( nextmot.mk );
		if( !nextanim ){
			DbgOut( "CAH : SetNewCamera : nextanim NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = nextanim->SetCameraAnimNo( -1, nextmot.aftframeno, 0 );
		if( ret ){
			DbgOut( "CAH : SetNewCamera : ca nextanim SetCameraAnimNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		m_animno = nextmot.mk;
		m_curcamera = nextanim->m_curcamera;
		return 0;//!!!!!!!!!!!!!!!
	}

	
	int curcamno = 0;
	if( camno < 0 ){
		int existflag = 0;
		ret = curanim->GetSWAnim( &curcamno, nextmot.aftframeno, &existflag );
		if( ret ){
			DbgOut( "CAH : SetNewCamera : ca GetSWAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curcamno = camno;
	}

	ret = curanim->StepCamera( curcamno, nextmot.aftframeno );
	if( ret ){
		DbgOut( "CAH : SetNewCamera : ca StepCamera error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_curcamera = curanim->m_curcamera;

	return 0;
}

int CCameraAnimHandler::SetDefaultCamera( int camno, CAMERAELEM srcce )
{
	if( (camno < 0) || (camno > 3) ){
		DbgOut( "cah : SetDefaultCamera : camno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_defaultcameras[camno] = srcce;

	return 0;
}



