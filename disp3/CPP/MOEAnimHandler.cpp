#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <mothandler.h>
#include <MOEAnimHandler.h>
#include <MOEAnim.h>
#include <MOEKey.h>
#include <MState.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

static int s_animcnt = 0;

CMOEAnimHandler::CMOEAnimHandler()
{
	InitParams();
}
CMOEAnimHandler::~CMOEAnimHandler()
{
	DestroyObjs();
}

int CMOEAnimHandler::InitParams()
{
/***
int m_animnum;
int m_animno;
CMOEAnim** m_ppmoeanim;
***/
	m_animnum = 0;
	m_animno = 0;
	m_ppmoeanim = 0;

	m_curms = 0;

	return 0;
}
int CMOEAnimHandler::DestroyObjs()
{
	if( m_ppmoeanim ){
		int animno;
		for( animno = 0; animno < m_animnum; animno++ ){
			CMOEAnim* delanim;
			delanim = *( m_ppmoeanim + animno );
			if( delanim ){
				delete delanim;
			}
		}
		free( m_ppmoeanim );
		m_ppmoeanim = 0;
	}
	m_animnum = 0;
	m_animno = 0;

	return 0;
}
CMOEAnim* CMOEAnimHandler::GetMOEAnimByIndex( int srcindex )
{
	if( (srcindex < 0) || (srcindex >= m_animnum) ){
		_ASSERT( 0 );
		return 0;
	}

	return *( m_ppmoeanim + srcindex );
}
CMOEAnim* CMOEAnimHandler::GetMOEAnim( int srcanimno, int* indexptr )
{
	CMOEAnim* retanim = 0;
	if( indexptr ){
		*indexptr = -1;
	}

	int anino;
	for( anino = 0; anino < m_animnum; anino++ ){
		CMOEAnim* curanim;
		curanim = *( m_ppmoeanim + anino );
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
CMOEAnim* CMOEAnimHandler::GetCurMOEAnim()
{
	if( (m_animnum <= 0) || (m_animno < 0) ){
		return 0;
	}
	CMOEAnim* retanim;
	retanim = GetMOEAnim( m_animno );
	return retanim;
}

CMOEAnim* CMOEAnimHandler::AddAnim( CShdHandler* lpsh, char* animname, int animtype, int framenum, int srcmotjump )
{
	CMOEAnim* newanim = 0;

	int newnum = m_animnum + 1;
	m_ppmoeanim = (CMOEAnim**)realloc( m_ppmoeanim, sizeof( CMOEAnim* ) * newnum );
	if( !m_ppmoeanim ){
		DbgOut( "MOEAH : AddAnim : ppmoeanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	*( m_ppmoeanim + m_animnum ) = 0;

	s_animcnt++;
	newanim = new CMOEAnim( animname, s_animcnt );
	if( !newanim ){
		DbgOut( "MOEAH : AddAnim : newanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int setframenum = max( framenum, 1 );

	newanim->m_maxframe = setframenum - 1;
	newanim->m_animtype = animtype;
	newanim->m_motjump = srcmotjump;

	int ret;
	ret = newanim->CreateFrameData( lpsh );
	if( ret ){
		DbgOut( "MOEAH : AddAnim : anim CreateFrameData error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( m_ppmoeanim + m_animnum ) = newanim;
	m_animnum = newnum;

	return newanim;
}
int CMOEAnimHandler::DestroyAnim( int delanimno )
{
	if( delanimno < 0 ){
		DestroyObjs();
	}else{
		
		CMOEAnim* delanim = 0;
		int delindex = -1;
		int canino;
		for( canino = 0; canino < m_animnum; canino++ ){
			CMOEAnim* curanim;
			curanim = *( m_ppmoeanim + canino );
			if( curanim && (curanim->m_motkind == delanimno) ){
				delanim = curanim;
				delindex = canino;
				break;
			}
		}

		if( delanim && (delindex >= 0) && (m_animnum > 0) ){
			int newnum = m_animnum - 1;

			CMOEAnim** newppanim;
			newppanim = (CMOEAnim**)malloc( sizeof( CMOEAnim* ) * newnum );
			if( !newppanim ){
				DbgOut( "MOEAH : DestroyAnim : newppanim alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( newppanim, sizeof( CMOEAnim* ) * newnum );

			int setno = 0;
			int canino;
			for( canino = 0; canino < m_animnum; canino++ ){
				if( canino != delindex ){
					*( newppanim + setno ) = *( m_ppmoeanim + canino );
					setno++;
				}
			}
			if( setno != newnum ){
				DbgOut( "MOEAH : DestroyAnim : setno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			delete delanim;
			free( m_ppmoeanim );
			m_ppmoeanim = newppanim;
			m_animnum = newnum;
		}
	}

	return 0;

}

int CMOEAnimHandler::SetTotalFrame( CShdHandler* lpsh, int animno, int newtotal )
{
	int ret;
	CMOEAnim* curanim;
	curanim = GetMOEAnim( animno );
	if( !curanim ){
		DbgOut( "MOEAH : SetTotalFrame : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal == (curanim->m_maxframe + 1) ){
		return 0;
	}

	if( newtotal < 1 ){
		DbgOut( "MOEAH : SetTotalFrame : totalframenum must be larger than 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal < (curanim->m_maxframe + 1) ){
		ret = curanim->DeleteMOEKeyOutOfRange( newtotal - 1 );
		if( ret ){
			DbgOut( "MOEAH : SetTotalFrame : ca DeleteMOEKeyOutOfRange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	curanim->m_maxframe = newtotal - 1;

	ret = curanim->CreateFrameData( lpsh );
	if( ret ){
		DbgOut( "MOEAH : SetTotalFrame : anim CreateFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CMOEAnimHandler::SetCurrentAnim( int animno )
{
	int ret;
	CMOEAnim* curanim;
	curanim = GetMOEAnim( animno );
	if( !curanim ){
		DbgOut( "MOEAH : SetCurrentAnim : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;
	ret = curanim->SetMOEAnimNo( 0, 1 );
	if( ret ){
		DbgOut( "MOEAH : SetCuurentAnim : ca SetMOEAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CMOEAnimHandler::GetCurrentAnim( int* animnoptr )
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

int CMOEAnimHandler::SetNextAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno )
{
	CMOEAnim* curanim;
	curanim = GetMOEAnim( animno );
	if( !curanim ){
		DbgOut( "MOEAH : SetNextAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMOEAnim* nextanim;
	nextanim = GetMOEAnim( nextanimno );
	if( !nextanim ){
		DbgOut( "MOEAH : SetNextAnimFrameNo : nextanimno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	curanim->m_nextmot.mk = nextanimno;
	curanim->m_nextmot.aftframeno = nextframeno;
	curanim->m_nextmot.befframeno = befframeno;

	return 0;
}
int CMOEAnimHandler::GetNextAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr )
{
	CMOEAnim* curanim;
	curanim = GetMOEAnim( animno );
	if( !curanim ){
		DbgOut( "MOEAH : GetNextAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*nextanimptr = curanim->m_nextmot.mk;
	*nextframenoptr = curanim->m_nextmot.aftframeno;

	return 0;
}

int CMOEAnimHandler::SetAnimFrameNo( CShdHandler* lpsh, CMotHandler* lpmh, int animno, int srcframeno, int srcisfirst )
{
	if( !lpmh ){
		m_curms = 0;
		return 0;//!!!!!!!!!!!!
	}

	int ret;
	CMOEAnim* curanim;
	curanim = GetMOEAnim( animno );
	if( !curanim ){
		DbgOut( "MOEAH : SetAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;

	ret = curanim->SetMOEAnimNo( srcframeno, srcisfirst );
	if( ret ){
		DbgOut( "MOEAH : SetAnimFrameNo : ca SetMOEAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_curms = curanim->m_curms;
	ret = lpmh->SetCurMatFromMState( lpsh, m_curms, curanim->m_bonenum, curanim->m_boneno2seri );
	if( ret ){
		DbgOut( "MOEAH : SetNewMOE : mh SetCurMatFromMState error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMOEAnimHandler::GetAnimFrameNo( int* animnoptr, int* framenoptr )
{
	if( m_animnum > 0 ){
		CMOEAnim* curanim;
		curanim = GetCurMOEAnim();
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

int CMOEAnimHandler::SetNewMOE( CShdHandler* lpsh, CMotHandler* lpmh )
{
	int ret;
	if( (m_animnum <= 0) || !lpmh ){
		m_curms = 0;
		return 0;
	}

	CMOEAnim* curanim;
	curanim = GetCurMOEAnim();
	if( !curanim ){
		m_curms = 0;
		return 0;
	}


	NEXTMOTION nextmot;
	ret = curanim->GetNextMOE( &nextmot, 0 );
	if( ret ){
		DbgOut( "MOEAH : SetNewMOE : ca GetNextMOE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( nextmot.mk != m_animno ){
		CMOEAnim* nextanim;
		nextanim = GetMOEAnim( nextmot.mk );
		if( !nextanim ){
			DbgOut( "MOEAH : SetNewMOE : nextanim NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = nextanim->SetMOEAnimNo( nextmot.aftframeno, 0 );
		if( ret ){
			DbgOut( "MOEAH : SetNewMOE : ca nextanim SetMOEAnimNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		m_animno = nextmot.mk;
		m_curms = nextanim->m_curms;
		return 0;//!!!!!!!!!!!!!!!
	}


	ret = curanim->StepMOE( nextmot.aftframeno );
	if( ret ){
		DbgOut( "MOEAH : SetNewMOE : ca StepMOE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_curms = curanim->m_curms;
	ret = lpmh->SetCurMatFromMState( lpsh, m_curms, curanim->m_bonenum, curanim->m_boneno2seri );
	if( ret ){
		DbgOut( "MOEAH : SetNewMOE : mh SetCurMatFromMState error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CMOEAnimHandler::CalcFrameData( int animno, CShdHandler* lpsh, CMotHandler* lpmh, CMCHandler* lpmch )
{
	CMOEAnim* anim;
	anim = GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "MOEAH : CalcFrameData : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = anim->CalcFrameData( lpsh, lpmh, lpmch );
	if( ret ){
		DbgOut( "MOEAH : CalcFrameData : anim CalcFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

