#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <coef.h>


#define DBGH
#include <dbg.h>
#include <crtdbg.h>

#include "SoundBank.h"
#include "SoundSet.h"
#include "SoundElem.h"
#include "HuSound.h"


CSoundBank::CSoundBank()
{
	InitParams();
}
CSoundBank::~CSoundBank()
{
	DestroyObjs();
}

int CSoundBank::InitParams()
{
//	int m_setnum;
//	CSoundSet** m_ppset;
	m_ppset = 0;
	m_setnum = 0;

	return 0;
}
int CSoundBank::DestroyObjs()
{
	if( m_ppset ){
		int sno;
		for( sno = 0; sno < m_setnum; sno++ ){
			CSoundSet* delset;
			delset = *( m_ppset + sno );
			if( delset ){
				delete delset;
			}
		}
		free( m_ppset );
		m_ppset = 0;
	}
	m_setnum = 0;


	return 0;
}

CSoundSet* CSoundBank::AddSoundSet( char* srcname, int* dstsetid )
{
	CSoundSet* newss = 0;
	newss = new CSoundSet();
	if( !newss ){
		DbgOut( "soundset : AddSoundSet newse alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = newss->CreateNewSet( srcname, dstsetid );
	if( ret ){
		DbgOut( "soundSet : AddSoundSet : ss CreateNewSet error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int newnum = m_setnum + 1;

	m_ppset = (CSoundSet**)realloc( m_ppset, sizeof( CSoundSet* ) * newnum );
	if( !m_ppset ){
		DbgOut( "soundset : AddSoundSet : ppset alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	*( m_ppset + m_setnum ) = newss;
	m_setnum = newnum;

	return newss;
}
int CSoundBank::DelSoundSet( int setid )
{
	if( setid >= 0 ){
		int sno;
		int delindex = -1;
		for( sno = 0; sno < m_setnum; sno++ ){
			CSoundSet* curse = *( m_ppset + sno );
			_ASSERT( curse );
			if( curse->m_setid == setid ){
				delindex = sno;
				break;
			}
		}
		
		if( delindex >= 0 ){
			int newnum = m_setnum - 1;
			CSoundSet** newppse;
			newppse = (CSoundSet**)malloc( sizeof( CSoundSet* ) * newnum );
			if( !newppse ){
				DbgOut( "soundbank : DelSoundSet : newppse alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ZeroMemory( newppse, sizeof( CSoundSet* ) * newnum );

			int setno = 0;
			for( sno = 0; sno < m_setnum; sno++ ){
				if( sno != delindex ){
					*( newppse + setno ) = *( m_ppset + sno );
					setno++;
				}
			}
			if( setno != newnum ){
				DbgOut( "soundBank : DelSoundSet : setno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			delete( *(m_ppset + delindex) );//!!!!!!!!!!

			free( m_ppset );
			m_ppset = newppse;
			m_setnum = newnum;

		}else{
			_ASSERT( 0 );
		}

	}else{
		DestroyObjs();
	}
	return 0;
}

int CSoundBank::AddSoundElem( CHuSound* srchs, int setid, CSoundElem* pse, int* dstsetid, int* dstsoundid )
{
	*dstsetid = -1;
	*dstsoundid = -1;

	CSoundSet* curss = 0;
	int ret;
	ret = GetSoundSet( setid, &curss );
	if( ret || !curss ){
		DbgOut( "soundbank : AddSoundElem : setid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundElem* newse = 0;
	newse = curss->AddSound( srchs, pse->m_filename, pse->m_bgmflag, pse->m_reverbflag, pse->m_bufnum );
	if( !newse ){
		DbgOut( "soundbank : AddSoundElem : ss AddSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstsetid = curss->m_setid;
	*dstsoundid = newse->m_serialno;

	return 0;
}
int CSoundBank::DelSoundElem( int setid, int soundid )
{
	int ret;
	if( setid >= 0 ){
		CSoundSet* curss = 0;
		ret = GetSoundSet( setid, &curss );
		if( ret || !curss ){
			DbgOut( "SoundBank : DelSoundElem : setid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( soundid >= 0 ){
			ret = curss->DelSound( soundid );
			_ASSERT( !ret );
		}else{
			ret = curss->DestroyObjs();	
			_ASSERT( !ret );
		}
	}else{
		DestroyObjs();
	}

	return 0;
}
int CSoundBank::GetIDByName( int setid, char* filename, int* dstsetid, int* dstsoundid )//setid -1Ç≈ëSåüçı
{
	*dstsetid = -1;
	*dstsoundid = -1;

	int ret;
	if( setid < 0 ){
		int sno;
		CSoundElem* findse = 0;
		for( sno = 0; sno < m_setnum; sno++ ){
			CSoundSet* curss = *( m_ppset + sno );
			ret = GetSoundElem( curss->m_setid, filename, &findse );
			_ASSERT( !ret );
			if( findse ){
				*dstsetid = curss->m_setid;
				*dstsoundid = findse->m_serialno;
				break;
			}
		}
	}else{
		CSoundSet* curss = 0;
		ret = GetSoundSet( setid, &curss );
		if( ret || !curss ){
			DbgOut( "soundbank : GetIDByName : setid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		CSoundElem* findse = 0;
		ret = GetSoundElem( curss->m_setid, filename, &findse );
		_ASSERT( !ret );
		if( findse ){
			*dstsetid = curss->m_setid;
			*dstsoundid = findse->m_serialno;
		}
	}

	return 0;
}

int CSoundBank::GetSoundSet( int setid, CSoundSet** ppss )
{
	CSoundSet* retss = 0;
	int sno;
	for( sno = 0; sno < m_setnum; sno++ ){
		CSoundSet* curss = *( m_ppset + sno );
		_ASSERT( curss );
		if( curss->m_setid == setid ){
			retss = curss;
			break;
		}
	}

	*ppss = retss;

	return 0;
}
int CSoundBank::GetSoundSet( char* setname, CSoundSet** ppss )
{
	CSoundSet* retss = 0;
	int sno;
	for( sno = 0; sno < m_setnum; sno++ ){
		CSoundSet* curss = *( m_ppset + sno );
		_ASSERT( curss );
		if( strcmp( curss->m_setname, setname ) == 0 ){
			retss = curss;
			break;
		}
	}

	*ppss = retss;

	return 0;
}

int CSoundBank::GetSoundElem( int setid, int soundid, CSoundElem** ppse )
{
	*ppse = 0;

	int ret;
	CSoundSet* curss = 0;
	ret = GetSoundSet( setid, &curss );
	if( ret || !curss ){
		DbgOut( "soundbank : GetSoundElem : setid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*ppse = curss->GetSoundElem( soundid );

	return 0;
}
int CSoundBank::GetSoundElem( int setid, char* filename, CSoundElem** ppse )
{
	*ppse = 0;

	int ret;
	CSoundSet* curss = 0;
	ret = GetSoundSet( setid, &curss );
	if( ret || !curss ){
		DbgOut( "soundbank : GetSoundElem : setid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*ppse = curss->GetSoundElem( filename );

	return 0;
}

int CSoundBank::GetSoundSetIndexByID( int setid, int* dstindex )
{
	int retindex = -1;
	int ssno;
	for( ssno = 0; ssno < m_setnum; ssno++ ){
		CSoundSet* curss = *( m_ppset + ssno );
		if( curss->m_setid == setid ){
			retindex = ssno;
			break;
		}
	}
	*dstindex = retindex;
	return 0;
}

int CSoundBank::UpSoundSet( int setid )
{
	if( m_setnum <= 1 ){
		return 0;
	}

	int upindex = -1;
	GetSoundSetIndexByID( setid, &upindex );
	if( upindex < 0 ){
		DbgOut( "soundbank : UpSoundSet : GetSoundSetIndexByID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( upindex == 0 ){
		return 0;//àÍî‘è„
	}

	CSoundSet** newppset;
	newppset = (CSoundSet**)malloc( sizeof( CSoundSet* ) * m_setnum );
	if( !newppset ){
		DbgOut( "soundbank : UpSoundSet : newppset alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newppset, sizeof( CSoundSet* ) * m_setnum );

	int setno = 0;
	int ssno;
	for( ssno = 0; ssno <= (upindex - 2); ssno++ ){
		*( newppset + setno ) = *( m_ppset + ssno );
		setno++;
	}

	*( newppset + upindex - 1 ) = *( m_ppset + upindex );
	setno++;

	*( newppset + upindex ) = *( m_ppset + upindex - 1 );
	setno++;


	for( ssno = (upindex + 1); ssno < m_setnum; ssno++ ){
		*( newppset + setno ) = *( m_ppset + ssno );
		setno++;
	}

	if( setno != m_setnum ){
		DbgOut( "SoundBank : UpSoundSet : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	free( m_ppset );
	m_ppset = newppset;

	return 0;

}
int CSoundBank::DownSoundSet( int setid )
{
	if( m_setnum <= 1 ){
		return 0;
	}

	int dindex = -1;
	GetSoundSetIndexByID( setid, &dindex );
	if( dindex < 0 ){
		DbgOut( "soundbank : DowmSoundSet : GetSoundSetIndexByID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( dindex == (m_setnum - 1) ){
		return 0;//àÍî‘â∫
	}

	CSoundSet** newppset;
	newppset = (CSoundSet**)malloc( sizeof( CSoundSet* ) * m_setnum );
	if( !newppset ){
		DbgOut( "soundbank : DownSoundSet : newppset alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newppset, sizeof( CSoundSet* ) * m_setnum );

	int setno = 0;
	int ssno;
	for( ssno = 0; ssno <= (dindex - 1); ssno++ ){
		*( newppset + setno ) = *( m_ppset + ssno );
		setno++;
	}

	*( newppset + dindex ) = *( m_ppset + dindex + 1 );
	setno++;

	*( newppset + dindex + 1 ) = *( m_ppset + dindex );
	setno++;

	for( ssno = (dindex + 2); ssno < m_setnum; ssno++ ){
		*( newppset + setno ) = *( m_ppset + ssno );
		setno++;
	}

	if( setno != m_setnum ){
		DbgOut( "SoundBank : DownSoundSet : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	free( m_ppset );
	m_ppset = newppset;

	return 0;
}
