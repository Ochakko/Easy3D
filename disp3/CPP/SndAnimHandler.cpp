#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <SndAnimHandler.h>

#include "c:/pgfile9/RokDeBone2DX/SoundSet.h"
#include <SndAnim.h>
#include <SndKey.h>
#include <SndFrame.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

static int s_animcnt = 0;

CSndAnimHandler::CSndAnimHandler()
{
	InitParams();
}
CSndAnimHandler::~CSndAnimHandler()
{
	DestroyObjs();
}
int CSndAnimHandler::InitParams()
{
//	int m_animnum;
//	int m_animno;
//	CSndAnim** m_ppsndanim;
//	CSndFrame* m_cursf;
	m_animnum = 0;
	m_animno = 0;
	m_ppsndanim = 0;
	m_cursf = 0;

	return 0;
}
int CSndAnimHandler::DestroyObjs()
{
	if( m_ppsndanim ){
		int animno;
		for( animno = 0; animno < m_animnum; animno++ ){
			CSndAnim* delanim;
			delanim = *( m_ppsndanim + animno );
			if( delanim ){
				delete delanim;
			}
		}
		free( m_ppsndanim );
		m_ppsndanim = 0;
	}
	m_animnum = 0;
	m_animno = 0;

	return 0;
}


CSndAnim* CSndAnimHandler::GetSndAnim( int srcanimno, int* indexptr )
{
	CSndAnim* retanim = 0;
	if( indexptr ){
		*indexptr = -1;
	}

	int anino;
	for( anino = 0; anino < m_animnum; anino++ ){
		CSndAnim* curanim;
		curanim = *( m_ppsndanim + anino );
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
CSndAnim* CSndAnimHandler::GetSndAnimByIndex( int srcindex )
{
	if( (srcindex < 0) || (srcindex >= m_animnum) ){
		_ASSERT( 0 );
		return 0;
	}

	return *( m_ppsndanim + srcindex );
}
CSndAnim* CSndAnimHandler::GetCurSndAnim()
{
	if( (m_animnum <= 0) || (m_animno < 0) ){
		return 0;
	}
	CSndAnim* retanim;
	retanim = GetSndAnim( m_animno );
	return retanim;
}

CSndAnim* CSndAnimHandler::AddAnim( char* animname, int srcfps, CSoundSet* srcss )
{
	CSndAnim* newanim = 0;

	int newnum = m_animnum + 1;
	m_ppsndanim = (CSndAnim**)realloc( m_ppsndanim, sizeof( CSndAnim* ) * newnum );
	if( !m_ppsndanim ){
		DbgOut( "SndAH : AddAnim : ppsndanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	*( m_ppsndanim + m_animnum ) = 0;

	s_animcnt++;
	newanim = new CSndAnim( animname, s_animcnt );
	if( !newanim ){
		DbgOut( "SndAH : AddAnim : newanim alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	strcpy_s( newanim->m_ssname, 32, srcss->m_setname );

	double totaltime;
	srcss->GetBGMTotalTime( &totaltime );

	double calcframe;
	calcframe = (double)srcfps * totaltime;

	newanim->m_maxframe = max( ((int)( calcframe + 0.5 )), 1 );
	newanim->m_fps = srcfps;

//	newanim->m_maxframe = max( (srcframenum - 1), 1 );

	int ret;
	ret = newanim->SetFrameData();
	if( ret ){
		DbgOut( "SndAH : AddAnim : anim CreateFrameData error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*( m_ppsndanim + m_animnum ) = newanim;
	m_animnum = newnum;

	return newanim;
}
int CSndAnimHandler::DestroyAnim( int delanimno )
{
	if( delanimno < 0 ){
		DestroyObjs();
	}else{
		
		CSndAnim* delanim = 0;
		int delindex = -1;
		int canino;
		for( canino = 0; canino < m_animnum; canino++ ){
			CSndAnim* curanim;
			curanim = *( m_ppsndanim + canino );
			if( curanim && (curanim->m_motkind == delanimno) ){
				delanim = curanim;
				delindex = canino;
				break;
			}
		}

		if( delanim && (delindex >= 0) && (m_animnum > 0) ){
			int newnum = m_animnum - 1;

			CSndAnim** newppanim;
			newppanim = (CSndAnim**)malloc( sizeof( CSndAnim* ) * newnum );
			if( !newppanim ){
				DbgOut( "SndAH : DestroyAnim : newppanim alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( newppanim, sizeof( CSndAnim* ) * newnum );

			int setno = 0;
			int canino;
			for( canino = 0; canino < m_animnum; canino++ ){
				if( canino != delindex ){
					*( newppanim + setno ) = *( m_ppsndanim + canino );
					setno++;
				}
			}
			if( setno != newnum ){
				DbgOut( "SndAH : DestroyAnim : setno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			delete delanim;
			free( m_ppsndanim );
			m_ppsndanim = newppanim;
			m_animnum = newnum;
		}
	}

	return 0;
}
int CSndAnimHandler::SetTotalFrame( int animno, int newtotal )
{
	int ret;
	CSndAnim* curanim;
	curanim = GetSndAnim( animno );
	if( !curanim ){
		DbgOut( "SndAH : SetTotalFrame : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal == (curanim->m_maxframe + 1) ){
		return 0;
	}

	if( newtotal < 1 ){
		DbgOut( "SndAH : SetTotalFrame : totalframenum must be larger than 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( newtotal < (curanim->m_maxframe + 1) ){
		ret = curanim->DeleteSndKeyOutOfRange( newtotal - 1 );
		if( ret ){
			DbgOut( "SndAH : SetTotalFrame : ca DeleteSndKeyOutOfRange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	curanim->m_maxframe = newtotal - 1;

	ret = curanim->SetFrameData();
	if( ret ){
		DbgOut( "SndAH : SetTotalFrame : anim CreateFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CSndAnimHandler::SetCurrentAnim( int animno )
{
	int ret;
	CSndAnim* curanim;
	curanim = GetSndAnim( animno );
	if( !curanim ){
		DbgOut( "SndAH : SetCurrentAnim : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;
	ret = curanim->SetSndAnimFrameNo( 0 );
	if( ret ){
		DbgOut( "SndAH : SetCuurentAnim : ca SetSndAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CSndAnimHandler::GetCurrentAnim( int* animnoptr )
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

int CSndAnimHandler::SetAnimFrameNo( int animno, int srcframeno )
{

	int ret;
	CSndAnim* curanim;
	curanim = GetSndAnim( animno );
	if( !curanim ){
		DbgOut( "SndAH : SetAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_animno = animno;

	ret = curanim->SetSndAnimFrameNo( srcframeno );
	if( ret ){
		DbgOut( "SndAH : SetAnimFrameNo : ca SetSndAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_cursf = curanim->m_curframe;

	return 0;
}
int CSndAnimHandler::GetAnimFrameNo( int* animnoptr, int* framenoptr )
{
	if( m_animnum > 0 ){
		CSndAnim* curanim;
		curanim = GetCurSndAnim();
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

int CSndAnimHandler::CalcFrameData( int animno, CSoundSet* pss )
{
	CSndAnim* anim;
	anim = GetSndAnim( animno );
	if( !anim ){
		DbgOut( "SndAH : CalcFrameData : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = anim->CheckAndDelInvalid( pss );
	if( ret ){
		DbgOut( "SndAH : CalcFrameData : anim CheckAndDelInvalid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->SetFrameData();
	if( ret ){
		DbgOut( "SndAH : CalcFrameData : anim CalcFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

