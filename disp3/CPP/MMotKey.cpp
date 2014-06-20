#include <stdafx.h> //ダミー
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <math.h>

#include <shdelem.h>

#include <MMotKey.h>
#include <MMotElem.h>


#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CMMotKey::CMMotKey()
{
	InitParams();
}
CMMotKey::~CMMotKey()
{
	DestroyObjs();
}

int CMMotKey::InitParams()
{
	frameno = 0;
	mmenum = 0;
	ppmme = 0;//morphのバリュー。ベースの数だけの配列。
	prev = 0;
	next = 0;

	return 0;
}
int CMMotKey::DestroyObjs()
{

	if( ppmme ){
		int mmeno;
		CMMotElem* delmme;
		for( mmeno = 0; mmeno < mmenum; mmeno++ ){
			delmme = *( ppmme + mmeno );
			if( delmme ){
				delete delmme;
			}
		}
		free( ppmme );
		ppmme = 0;
	}
	mmenum = 0;

	return 0;
}

CMMotElem* CMMotKey::ExistBase( CShdElem* srcbase )
{
	CMMotElem* retmme = 0;

	int mmeno;
	for( mmeno = 0; mmeno < mmenum; mmeno++ ){
		CMMotElem* chkmme;
		chkmme = *( ppmme + mmeno );
		if( chkmme->m_baseelem == srcbase ){
			retmme = chkmme;
			break;
		}
	}
	return retmme;
}

MPRIM* CMMotKey::ExistTarget( CShdElem* srcbase, CShdElem* srctarget )
{
	MPRIM* retmprim = 0;

	CMMotElem* basemme;
	basemme = ExistBase( srcbase );
	if( !basemme ){
		return 0;
	}

	retmprim = basemme->GetPrim( srctarget );
	return retmprim;
}

int CMMotKey::AddBase( CShdElem* srcbase )
{
	CMMotElem* findmme;
	findmme = ExistBase( srcbase );
	if( findmme ){
		return 0;
	}

	ppmme = (CMMotElem**)realloc( ppmme, sizeof( CMMotElem* ) * ( mmenum + 1 ) );
	if( !ppmme ){
		DbgOut( "mmk : AddBase : ppmme alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMMotElem* newmme;
	newmme = new CMMotElem( srcbase );
	if( !newmme ){
		DbgOut( "mmk : AddBase : newmme alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( ppmme + mmenum ) = newmme;
	mmenum++;
	
	return 0;
}
int CMMotKey::DeleteAllElem()
{
	return DestroyObjs();
}

int CMMotKey::DeleteBase( CShdElem* delbase )
{
	CMMotElem* delmme;
	delmme = ExistBase( delbase );
	if( !delmme ){
		return 0;
	}

	CMMotElem** newppmme;
	newppmme = (CMMotElem**)malloc( sizeof( CMMotElem* ) * ( mmenum - 1 ) );
	if( !newppmme ){
		DbgOut( "mmk : DeleteBase : newppmme alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int setno = 0;
	int mmeno;
	for( mmeno = 0; mmeno < mmenum; mmeno++ ){
		CMMotElem* cpmme;
		cpmme = *( ppmme + mmeno );
		if( cpmme != delmme ){
			*( newppmme + setno ) = cpmme;
			setno++;
		}
	}

	if( setno != (mmenum - 1) ){
		DbgOut( "mmk : DeleteBase : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delete delmme;
	free( ppmme );
	ppmme = newppmme;
	mmenum--;

	return 0;
}
int CMMotKey::AddTarget( CShdElem* srcbase, CShdElem* srctarget )
{
	int ret;
	CMMotElem* basemme;
	basemme = ExistBase( srcbase );
	if( !basemme ){
		ret = AddBase( srcbase );
		if( ret ){
			DbgOut( "mmk : AddTarget : AddBase error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		basemme = ExistBase( srcbase );
		if( !basemme ){
			DbgOut( "mmk : AddTarget : basemme NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = basemme->AddPrim( srctarget );
	if( ret ){
		DbgOut( "mmk : AddTarget : basemme AddPrim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMMotKey::DeleteTarget( CShdElem* srcbase, CShdElem* srctarget )
{
	CMMotElem* basemme;
	basemme = ExistBase( srcbase );
	if( !basemme ){
		return 0;
	}

	int ret;
	ret = basemme->DeletePrim( srctarget );
	if( ret ){
		DbgOut( "mmk : DeleteTarget : basemme DeletePrim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMMotKey::SetValue( CShdElem* srcbase, CShdElem* srctarget, float srcvalue )
{
	int ret;
	CMMotElem* basemme;
	basemme = ExistBase( srcbase );
	if( !basemme ){
		ret = AddBase( srcbase );
		if( ret ){
			DbgOut( "mmk : SetValue : AddBase error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		basemme = ExistBase( srcbase );
		if( !basemme ){
			DbgOut( "mmk : SetValue : basemme NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	MPRIM* targetprim;
	targetprim = basemme->GetPrim( srctarget );
	if( !targetprim ){
		ret = AddTarget( srcbase, srctarget );
		if( ret ){
			DbgOut( "mmk : SetValue : AddTarget error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		targetprim = basemme->GetPrim( srctarget );
		if( !targetprim ){
			DbgOut( "mmk : SetValue : targetprim NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	targetprim->value = srcvalue;


	return 0;
}
float CMMotKey::GetValue( CShdElem* srcbase, CShdElem* srctarget, int* existflag )
{
	CMMotElem* basemme;
	basemme = ExistBase( srcbase );
	if( !basemme ){
		*existflag = 0;
		return 0.0f;
	}

	MPRIM* targetprim;
	targetprim = ExistTarget( srcbase, srctarget );
	if( !targetprim ){
		*existflag = 0;
		return 0.0f;
	}

	*existflag = 1;
	return targetprim->value;
	
}

int CMMotKey::FramenoCmp( CMMotKey* cmpmmk )
{
	int cmpno;
	_ASSERT( cmpmmk );
	cmpno = cmpmmk->frameno;

	if( frameno > cmpno ){
		return 1;
	}else if( frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
int CMMotKey::FramenoCmp( int cmpno )
{
	if( frameno > cmpno ){
		return 1;
	}else if( frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CMMotKey::AddToPrev( CMMotKey* addmmk )
{
	CMMotKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addmmk->prev = prev;
	addmmk->next = this;

	prev = addmmk;

	if( saveprev ){
		saveprev->next = addmmk;
	}

	return 0;
}
int CMMotKey::AddToNext( CMMotKey* addmmk )
{
	CMMotKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addmmk->prev = this;
	addmmk->next = savenext;

	next = addmmk;

	if( savenext ){
		savenext->prev = addmmk;
	}


	return 0;
}
int CMMotKey::LeaveFromChain()
{
	CMMotKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	prev = 0;
	next = 0;

	if( saveprev ){
		saveprev->next = savenext;
	}

	if( savenext ){
		savenext->prev = saveprev;
	}

	return 0;
}

int CMMotKey::CopyMMotKey( CMMotKey* srcmmk )
{
	DestroyObjs();

	int ret;
	int mmeno;
	for( mmeno = 0; mmeno < srcmmk->mmenum; mmeno++ ){
		CMMotElem* srcmme;
		srcmme = *( srcmmk->ppmme + mmeno );

		int tno;
		for( tno = 0; tno < srcmme->m_primnum; tno++ ){
			MPRIM* srcmp;
			srcmp = srcmme->m_primarray + tno;

			ret = SetValue( srcmme->m_baseelem, srcmp->selem, srcmp->value );
			if( ret ){
				DbgOut( "mmk : CopyMMotKey : SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CMMotKey::CopyMMotElem( CMMotElem* srcmme )
{
	int ret;
	CShdElem* srcbase;
	srcbase = srcmme->m_baseelem;

	ret = DeleteBase( srcbase );
	if( ret ){
		DbgOut( "mmk : CopyMMotElem : DeleteBase error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int tno;
	for( tno = 0; tno < srcmme->m_primnum; tno++ ){
		MPRIM* srcmp;
		srcmp = srcmme->m_primarray + tno;

		ret = SetValue( srcbase, srcmp->selem, srcmp->value );
		if( ret ){
			DbgOut( "mmk : CopyMMotElem : SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
