#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <SndAnim.h>

#include "c:/pgfile9/RokDeBone2DX/SoundSet.h"
#include "c:/pgfile9/RokDeBone2DX/SoundElem.h"
#include <SndKey.h>
#include <SndFullFrame.h>
#include <SndFrame.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CSndAnim::CSndAnim( char* srcname, int srcmotkind )
{
	InitParams();
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}
	m_motkind = srcmotkind;
}
CSndAnim::~CSndAnim()
{
	DestroyObjs();
}

int CSndAnim::InitParams()
{
//	char m_animname[256];
//	int m_motkind;
//	int m_frameno;
//	SNDELEM m_defsnde;
//	int m_maxframe;	
//	CSndKey* m_firstkey;
//	CSndFullFrame* m_fullframe;
//	CSndFrame* m_curframe;

	ZeroMemory( m_ssname, sizeof( char ) * 32 );
	ZeroMemory( m_animname, sizeof( char ) * 256 );
	m_motkind = 0;
	m_frameno = 0;
	
	ZeroMemory( &m_defsnde, sizeof( SNDELEM ) );
	m_defsnde.sndsetid = -1;
	m_defsnde.sndid = -1;

	m_maxframe = 0;
	m_firstkey = 0;
	m_fullframe = 0;
	m_curframe = 0;

	m_fps = 60;
	m_bgmloop = 0;
	return 0;
}

int CSndAnim::DestroyObjs()
{
	DestroyAllKeys();
	DestroyFrameData();
	InitParams();

	return 0;
}
int CSndAnim::DestroyAllKeys()
{
	CSndKey* delsndk = m_firstkey;
	CSndKey* nextsndk = 0;
	while( delsndk ){
		nextsndk = delsndk->next;
		delete delsndk;
		delsndk = nextsndk;
	}
	m_firstkey = 0;

	return 0;
}
int CSndAnim::DestroyFrameData()
{
	if( m_fullframe ){
		delete m_fullframe;
		m_fullframe = 0;
	}

	return 0;
}

int CSndAnim::ExistSndKey( int frameno, CSndKey** ppsndk )
{
	*ppsndk = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "SndAnim : ExistSndKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CSndKey* cursndk = m_firstkey;
	while( cursndk && (cursndk->m_frameno <= frameno) ){
		if( cursndk->m_frameno == frameno ){
			*ppsndk = cursndk;
			break;
		}

		cursndk = cursndk->next;
	}


	return 0;
}
int CSndAnim::CreateSndKey( int frameno, CSndKey** ppsndk )
{
	*ppsndk = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "SndAnim : CreateSndKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CSndKey* newsndk = 0;
	newsndk = new CSndKey();
	if( !newsndk ){
		DbgOut( "SndAnim : CreateSndKey : newsndk alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newsndk->m_frameno = frameno;


//////////////////
	int ret;
	CSndKey* sndkhead = m_firstkey;
	CSndKey* cursndk = sndkhead;
	CSndKey* largersndk = 0;
	CSndKey* smallersndk = 0;
	int cmp;
	while( cursndk ){
		cmp = newsndk->FramenoCmp( cursndk );
		if( cmp < 0 ){
			largersndk = cursndk;
			break;
		}else{
			smallersndk = cursndk;
		}
		cursndk = cursndk->next;
	}

	if( !sndkhead ){
		//先頭に追加
		m_firstkey = newsndk;
	}else{
		if( largersndk ){
			//largersndkの前に追加。
			ret = largersndk->AddToPrev( newsndk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newsndk->prev )
				m_firstkey = newsndk;
		}else{
			//最後に追加。(smallersndk の後に追加)
			_ASSERT( smallersndk );
			ret = smallersndk->AddToNext( newsndk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppsndk = newsndk;

	return 0;
}
CSndKey* CSndAnim::GetFirstSndKey()
{
	return m_firstkey;

}
CSndKey* CSndAnim::LeaveFromChainSndKey( int frameno )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "SndAnim : LeaveFromChainSndKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	int ret;
	CSndKey* cursndk = 0;
	ret = ExistSndKey( frameno, &cursndk );
	if( ret ){
		return 0;
	}

	if( cursndk ){
		CSndKey* firstsndk;
		firstsndk = m_firstkey;
		if( firstsndk == cursndk ){
			m_firstkey = cursndk->next;
		}

		cursndk->LeaveFromChain();
	}

	return cursndk;
}
int CSndAnim::ChainSndKey( CSndKey* addsndk )
{
	if( (addsndk->m_frameno < 0) || (addsndk->m_frameno > m_maxframe) ){
		DbgOut( "SndAnim : ChainSndKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CSndKey* sndkhead = m_firstkey;
	CSndKey* cursndk = sndkhead;
	CSndKey* largersndk = 0;
	CSndKey* smallersndk = 0;
	int cmp;
	while( cursndk ){
		cmp = addsndk->FramenoCmp( cursndk );
		if( cmp < 0 ){
			largersndk = cursndk;
			break;
		}else{
			smallersndk = cursndk;
		}
		cursndk = cursndk->next;
	}

	if( !sndkhead ){
		//先頭に追加
		m_firstkey = addsndk;
	}else{
		if( largersndk ){
			//largersndkの前に追加。
			ret = largersndk->AddToPrev( addsndk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addsndk->prev )
				m_firstkey = addsndk;
		}else{
			//最後に追加。(smallersndk の後に追加)
			_ASSERT( smallersndk );
			ret = smallersndk->AddToNext( addsndk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}
int CSndAnim::DeleteSndKeyOutOfRange( int srcmaxframe )
{
	int ret;
	CSndKey* sndkptr = m_firstkey;
	CSndKey* nextsndk = 0;
	while( sndkptr ){
		nextsndk = sndkptr->next;
		int frameno = sndkptr->m_frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteSndKey( frameno );
			if( ret ){
				DbgOut( "SndAnim : DeleteSndKeyOutOfRange : DeleteSndKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		sndkptr = nextsndk;
	}

	return 0;
}

int CSndAnim::SetSndKey( int frameno, CSndKey* srcsndk )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "SndAnim : SetSndKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CSndKey* psndk = 0;
	ret = ExistSndKey( frameno, &psndk );
	if( ret ){
		DbgOut( "SndAnim : SetSndKey : ExistSndKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !psndk ){
		ret = CreateSndKey( frameno, &psndk );
		if( ret || !psndk ){
			DbgOut( "SndAnim : SetSndKey : CreateSndKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	_ASSERT( psndk );

	ret = psndk->CopySndElem( srcsndk );
	if( ret ){
		DbgOut( "SndAnim : SetSndKey : sndk CopySndElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndAnim::DeleteSndKey( int frameno )
{
	if( frameno < 0 ){
		DestroyAllKeys();
	}else{
		CSndKey* delsndk = 0;
		delsndk = LeaveFromChainSndKey( frameno );
		if( delsndk ){
			delete delsndk;
		}
	}

	return 0;
}
int CSndAnim::AddSndE( int frameno, SNDELEM* srcsnde )
{
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "SndAnim : AddSndE : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CSndKey* psndk = 0;
	ret = ExistSndKey( frameno, &psndk );
	if( ret ){
		DbgOut( "SndAnim : AddSndE : ExistSndKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !psndk ){
		ret = CreateSndKey( frameno, &psndk );
		if( ret || !psndk ){
			DbgOut( "SndAnim : AddSndE : CreateSndKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	_ASSERT( psndk );

	ret = psndk->AddSndE( srcsnde );
	if( ret ){
		DbgOut( "SndAnim : AddSndE : sndk AddSndE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndAnim::DeleteSndE( int frameno, int delindex, int* delkeyflag )
{
	*delkeyflag = 0;

	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "SndAnim : DeleteSndE : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CSndKey* psndk = 0;
	ret = ExistSndKey( frameno, &psndk );
	if( ret ){
		DbgOut( "SndAnim : DeleteSndE : ExistSndKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( !psndk ){
		return 0;
	}

	ret = psndk->DeleteSndE( delindex );
	if( ret ){
		DbgOut( "SndAnim : DeleteSndE : sndk DeleteSndE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( psndk->m_sndenum <= 0 ){
		*delkeyflag = 1;//!!!!!!!!!!!!!!!!!!
		ret = DeleteSndKey( frameno );
		if( ret ){
			DbgOut( "SndAnim : DeleteSndE : DeleteSndKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CSndAnim::GetSndAnim( CSndFrame** ppsf, int srcframeno )
{
	if( (srcframeno < 0) || (srcframeno > m_maxframe) ){
		DbgOut( "SndAnim : GetSndAnim : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !m_fullframe ){
		*ppsf = 0;
	}else{
		*ppsf = m_fullframe->GetFrame( srcframeno );
	}

	return 0;
}

int CSndAnim::SetSndAnimFrameNo( int frameno )
{
	int ret;
	if( (frameno < 0) || (frameno > m_maxframe) ){
		DbgOut( "SndAnim : SetSndAnimNo : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	m_frameno = frameno;


	ret = GetSndAnim( &m_curframe, m_frameno );
	if( ret ){
		DbgOut( "SndAnim : SetSndAnimNo : GetSndAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CSndAnim::GetSndKeyframeNoRange( int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	int setno = 0;
	CSndKey* cursndk = m_firstkey;
	while( cursndk ){
		if( (cursndk->m_frameno >= srcstart) && (cursndk->m_frameno <= srcend) ){
			if( framearray && (arrayleng > 0) ){
				if( setno >= arrayleng ){
					DbgOut( "SndAnim : GetSndKeyframeNoRange : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( framearray + setno ) = cursndk->m_frameno;
			}
			setno++;
		}
		cursndk = cursndk->next;
	}

	*framenumptr = setno;

	return 0;
}


int CSndAnim::SetAnimName( char* srcname )
{
	if( srcname && *srcname ){
		strcpy_s( m_animname, 256, srcname );
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndAnim::GetAnimName( char* dstname, int arrayleng )
{
	if( !dstname || (arrayleng <= 0) ){
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( dstname, arrayleng, m_animname );

	return 0;
}

int CSndAnim::SetFrameData()
{
	int ret;
	ret = DestroyFrameData();
	_ASSERT( !ret );

	m_fullframe = new CSndFullFrame();
	if( !m_fullframe ){
		DbgOut( "SndAnim : SetFrameData : fullframe alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_fullframe->CreateFullFrame( m_firstkey, m_maxframe + 1, 3 );
	if( ret ){
		DbgOut( "SndAnim : SetFrameData : fullframe CreateFullFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSndAnim::CheckAndDelInvalid( CSoundSet* lpss )
{
	if( !m_firstkey ){
		return 0;
	}

	int ret;
	CSndKey* curkey = m_firstkey;
	CSndKey* nextkey = 0;
	while( curkey ){
		nextkey = curkey->next;

		int eno;
		for( eno = 0; eno < curkey->m_sndenum; eno++ ){
			SNDELEM* cursnde = curkey->m_psnde + eno;

			int sndsetid = lpss->m_setid;
			int cmp0;
			cmp0 = strcmp( lpss->m_setname, cursnde->sndsetname );
			if( cmp0 == 0 ){
				cursnde->sndsetid = sndsetid;
			}else{
				int delkeyflag = 0;
				ret = DeleteSndE( curkey->m_frameno, eno, &delkeyflag );
				_ASSERT( !ret );
				if( delkeyflag ){
					break;
				}
			}

			if( cmp0 == 0 ){
				CSoundElem* chkse;
				if( cursnde->sndname[0] ){
					chkse = lpss->GetSoundElem( cursnde->sndname );
				}else{
					chkse = 0;
				}

				if( chkse ){
					cursnde->sndid = chkse->m_serialno;
				}else{
					int delkeyflag = 0;
					ret = DeleteSndE( curkey->m_frameno, eno, &delkeyflag );
					_ASSERT( !ret );
					if( delkeyflag ){
						break;
					}
				}
			}
		}

		curkey = nextkey;
	}

	return 0;
}

int CSndAnim::ResetDoneFlag( int srcframe )
{
	CSndKey* sndk = m_firstkey;
	while( sndk ){
		if( sndk->m_frameno < srcframe ){
			sndk->m_doneflag = 1;
		}else{
			sndk->m_doneflag = 0;
		}
		sndk = sndk->next;
	}
	return 0;
}