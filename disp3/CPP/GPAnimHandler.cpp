#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <GPAnimHandler.h>

#include <GPAnim.h>
#include <GPKey.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

static int s_animcnt = 0;

CGPAnimHandler::CGPAnimHandler()
{
	InitParams();
}
CGPAnimHandler::~CGPAnimHandler()
{
	DestroyObjs();
}

int CGPAnimHandler::InitParams()
{
/***
int m_animnum;
int m_animno;
CGPAnim** m_ppgpanim;
int m_definterp;
***/
	m_animnum = 0;
	m_animno = 0;
	m_ppgpanim = 0;
	m_definterp = INTERPOLATION_SLERP;

	m_defaultgpe.ongmode = GROUND_NONE;
	m_defaultgpe.pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_defaultgpe.rot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_defaultgpe.rayy = 5000.0f;
	m_defaultgpe.rayleng = 8000.0f;
	m_defaultgpe.offsety = 10.0f;

	m_curgpe = m_defaultgpe;

	D3DXMatrixIdentity( &m_offmat );
	m_offflag = 0;

	return 0;
}
int CGPAnimHandler::DestroyObjs()
{
	if( m_ppgpanim ){
		int animno;
		for( animno = 0; animno < m_animnum; animno++ ){
			CGPAnim* delanim;
			delanim = *( m_ppgpanim + animno );
			if( delanim ){
				delete delanim;
			}
		}
		free( m_ppgpanim );
		m_ppgpanim = 0;
	}
	m_animnum = 0;
	m_animno = 0;

	return 0;
}
CGPAnim* CGPAnimHandler::GetGPAnimByIndex( int srcindex )
{
	if( (srcindex < 0) || (srcindex >= m_animnum) ){
		_ASSERT( 0 );
		return 0;
	}

	return *( m_ppgpanim + srcindex );
}
CGPAnim* CGPAnimHandler::GetGPAnim( int srcanimno, int* indexptr )
{
	CGPAnim* retanim = 0;
	if( indexptr ){
		*indexptr = -1;
	}

	int anino;
	for( anino = 0; anino < m_animnum; anino++ ){
		CGPAnim* curanim;
		curanim = *( m_ppgpanim + anino );
		if( curanim && (curanim->m_motkind == srcanimno) ){
			retanim = curanim;
			if( indexptr ){
				*indexptr = anino;
			}
			break;
		}
	}

	return retanim;
}
CGPAnim* CGPAnimHandler::GetCurGPAnim()
{
	if( (m_animnum <= 0) || (m_animno < 0) ){
		return 0;
	}
	CGPAnim* retanim;
	retanim = GetGPAnim( m_animno );
	return retanim;
}

CGPAnim* CGPAnimHandler::AddAnim( char* animname, int animtype, int framenum, int srcmotjump )
{
	CGPAnim* newanim = 0;

	int newnum = m_animnum + 1;
	m_ppgpanim = (CGPAnim**)realloc( m_ppgpanim, sizeof( CGPAnim* ) * newnum );
	if( !m_ppgpanim ){
		DbgOut( "GPAH : AddAnim : ppgpanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	*( m_ppgpanim + m_animnum ) = 0;

	s_animcnt++;
	newanim = new CGPAnim( animname, s_animcnt, &m_defaultgpe, &m_definterp, &m_offmat, &m_offflag );
	if( !newanim ){
		DbgOut( "GPAH : AddAnim : newanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int setframenum = max( framenum, 1 );

	newanim->m_maxframe = setframenum - 1;
	newanim->m_animtype = animtype;
	newanim->m_motjump = srcmotjump;

	int ret;
	ret = newanim->CreateFrameData( setframenum );
	if( ret ){
		DbgOut( "GPAH : AddAnim : anim CreateFrameData error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( m_ppgpanim + m_animnum ) = newanim;
	m_animnum = newnum;

	return newanim;
}
int CGPAnimHandler::DestroyAnim( int delanimno )
{
	if( delanimno < 0 ){
		DestroyObjs();
	}else{
		
		CGPAnim* delanim = 0;
		int delindex = -1;
		int canino;
		for( canino = 0; canino < m_animnum; canino++ ){
			CGPAnim* curanim;
			curanim = *( m_ppgpanim + canino );
			if( curanim && (curanim->m_motkind == delanimno) ){
				delanim = curanim;
				delindex = canino;
				break;
			}
		}

		if( delanim && (delindex >= 0) && (m_animnum > 0) ){
			int newnum = m_animnum - 1;

			CGPAnim** newppanim;
			newppanim = (CGPAnim**)malloc( sizeof( CGPAnim* ) * newnum );
			if( !newppanim ){
				DbgOut( "GPAH : DestroyAnim : newppanim alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( newppanim, sizeof( CGPAnim* ) * newnum );

			int setno = 0;
			int canino;
			for( canino = 0; canino < m_animnum; canino++ ){
				if( canino != delindex ){
					*( newppanim + setno ) = *( m_ppgpanim + canino );
					setno++;
				}
			}
			if( setno != newnum ){
				DbgOut( "GPAH : DestroyAnim : setno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			delete delanim;
			free( m_ppgpanim );
			m_ppgpanim = newppanim;
			m_animnum = newnum;
		}
	}

	return 0;

}

int CGPAnimHandler::SetTotalFrame( int animno, int newtotal )
{
	int ret;
	CGPAnim* curanim;
	curanim = GetGPAnim( animno );
	if( !curanim ){
		DbgOut( "GPAH : SetTotalFrame : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal == (curanim->m_maxframe + 1) ){
		return 0;
	}

	if( newtotal < 1 ){
		DbgOut( "GPAH : SetTotalFrame : totalframenum must be larger than 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal < (curanim->m_maxframe + 1) ){
		ret = curanim->DeleteGPKeyOutOfRange( newtotal - 1 );
		if( ret ){
			DbgOut( "GPAH : SetTotalFrame : ca DeleteGPKeyOutOfRange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = curanim->CreateFrameData( newtotal );
	if( ret ){
		DbgOut( "GPAH : SetTotalFrame : anim CreateFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curanim->m_maxframe = newtotal - 1;

	return 0;
}

int CGPAnimHandler::SetCurrentAnim( int animno )
{
	int ret;
	CGPAnim* curanim;
	curanim = GetGPAnim( animno );
	if( !curanim ){
		DbgOut( "GPAH : SetCurrentAnim : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;
	int existflag = 0;
	ret = curanim->SetGPAnimNo( 0, 1 );
	if( ret ){
		DbgOut( "GPAH : SetCuurentAnim : ca SetGPAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CGPAnimHandler::GetCurrentAnim( int* animnoptr )
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

int CGPAnimHandler::SetDefInterp( int srcinterp )
{
	if( (srcinterp >= INTERPOLATION_SLERP) && (srcinterp < INTERPOLATION_MAX) ){
		m_definterp = srcinterp;
	}else{
		_ASSERT( 0 );
		m_definterp = INTERPOLATION_SLERP;
	}
	return 0;
}
int CGPAnimHandler::GetDefInterp( int* dstinterp )
{
	*dstinterp = m_definterp;
	return 0;
}
int CGPAnimHandler::SetNextAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno )
{
	CGPAnim* curanim;
	curanim = GetGPAnim( animno );
	if( !curanim ){
		DbgOut( "GPAH : SetNextAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CGPAnim* nextanim;
	nextanim = GetGPAnim( nextanimno );
	if( !nextanim ){
		DbgOut( "GPAH : SetNextAnimFrameNo : nextanimno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	curanim->m_nextmot.mk = nextanimno;
	curanim->m_nextmot.aftframeno = nextframeno;
	curanim->m_nextmot.befframeno = befframeno;

	return 0;
}
int CGPAnimHandler::GetNextAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr )
{
	CGPAnim* curanim;
	curanim = GetGPAnim( animno );
	if( !curanim ){
		DbgOut( "GPAH : GetNextAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*nextanimptr = curanim->m_nextmot.mk;
	*nextframenoptr = curanim->m_nextmot.aftframeno;

	return 0;
}

int CGPAnimHandler::SetAnimFrameNo( int animno, int srcframeno, int srcisfirst )
{
	int ret;
	CGPAnim* curanim;
	curanim = GetGPAnim( animno );
	if( !curanim ){
		DbgOut( "GPAH : SetAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;

	ret = curanim->SetGPAnimNo( srcframeno, srcisfirst );
	if( ret ){
		DbgOut( "GPAH : SetAnimFrameNo : ca SetGPAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_curgpe = curanim->m_curgpe;

	return 0;
}
int CGPAnimHandler::GetAnimFrameNo( int* animnoptr, int* framenoptr )
{
	if( m_animnum > 0 ){
		CGPAnim* curanim;
		curanim = GetCurGPAnim();
		if( curanim ){
			*animnoptr = m_animno;
			*framenoptr = curanim->m_frameno;
		}else{
			*animnoptr = -1;
			*framenoptr = -1;			
		}
	}else{
		*animnoptr = -1;
		*framenoptr = -1;
	}

	return 0;
}

int CGPAnimHandler::SetNewGP()
{
	int ret;
	if( m_animnum <= 0 ){
		m_curgpe = m_defaultgpe;
		return 0;
	}

	CGPAnim* curanim;
	curanim = GetCurGPAnim();
	if( !curanim ){
		m_curgpe = m_defaultgpe;
		return 0;
	}


	NEXTMOTION nextmot;
	ret = curanim->GetNextGP( &nextmot, 0 );
	if( ret ){
		DbgOut( "GPAH : SetNewGP : ca GetNextGP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( nextmot.mk != m_animno ){
		CGPAnim* nextanim;
		nextanim = GetGPAnim( nextmot.mk );
		if( !nextanim ){
			DbgOut( "GPAH : SetNewGP : nextanim NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = nextanim->SetGPAnimNo( nextmot.aftframeno, 0 );
		if( ret ){
			DbgOut( "GPAH : SetNewGP : ca nextanim SetGPAnimNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		m_animno = nextmot.mk;
		m_curgpe = nextanim->m_curgpe;
		return 0;//!!!!!!!!!!!!!!!
	}


	ret = curanim->StepGP( nextmot.aftframeno );
	if( ret ){
		DbgOut( "GPAH : SetNewGP : ca StepGP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_curgpe = curanim->m_curgpe;

	return 0;
}

int CGPAnimHandler::CalcFrameData( int animno, CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX* matWorld )
{
	CGPAnim* curanim;
	curanim = GetCurGPAnim();
	if( !curanim ){
		return 1;
	}

	int ret;
	ret = curanim->CalcFrameData( lpsh, lpmh, matWorld );
	if( ret ){
		DbgOut( "GPAH : CalcFrameData : anim CalcFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CGPAnimHandler::SetOffsetMat( D3DXMATRIX srcoff )
{
	m_offmat = srcoff;
	m_offflag = 1;
	return 0;
}

int CGPAnimHandler::SetDefaultGPE( GPELEM srcgpe )
{
	m_defaultgpe = srcgpe;

	return 0;
}