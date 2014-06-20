#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <AlpKey.h>
#include <AlpChange.h>
#include <MQOMaterial.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CAlpKey::CAlpKey()
{
	InitParams();
}

CAlpKey::~CAlpKey()
{
	DestroyObjs();
}

int CAlpKey::InitParams()
{
	frameno = 0;
	changenum = 0;
	alpchange = 0;
	prev = 0;
	next = 0;

	return 0;
}

int CAlpKey::DestroyObjs()
{
	if( alpchange ){
		int chgno;
		for( chgno = 0; chgno < changenum; chgno++ ){

			CAlpChange* curchg;
			curchg = *( alpchange + chgno );

			if( curchg ){
				delete curchg;
			}
		}
		free( alpchange );
		alpchange = 0;
	}
	changenum = 0;

	return 0;
}

int CAlpKey::SetAlpChange( CMQOMaterial* srcmat, float srcchange )
{
	int ret;
	CAlpChange* findchg;
	int index;
	findchg = FindAlpChange( srcmat, &index );
	if( findchg ){
		ret = findchg->SetAlpChange( srcchange );
		if( ret ){
			DbgOut( "AlpKey : SetAlpChange : findchg SetAlpChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		alpchange = (CAlpChange**)realloc( alpchange, sizeof( CAlpChange* ) * ( changenum + 1 ) );
		if( !alpchange ){
			DbgOut( "AlpKey : SetAlpChange : alpchange alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		changenum++;
		*( alpchange + changenum - 1 ) = 0;

		CAlpChange* newchg;
		newchg = new CAlpChange();
		if( !newchg ){
			DbgOut( "AlpKey : SetAlpChange : newchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		newchg->mqomat = srcmat;
		ret = newchg->SetAlpChange( srcchange );
		if( ret ){
			DbgOut( "AlpKey : SetAlpChange : newchg SetAlpChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*( alpchange + changenum - 1 ) = newchg;

	}


	return 0;
}
int CAlpKey::DeleteAllChange()
{
	return DestroyObjs();
}

int CAlpKey::DeleteAlpChange( CMQOMaterial* srcmat )
{
	int index;
	CAlpChange* findchg;
	findchg = FindAlpChange( srcmat, &index );

	if( index >= 0 ){
		CAlpChange** ppnewchg;
		ppnewchg = (CAlpChange**)malloc( sizeof( CAlpChange* ) * ( changenum - 1 ) );
		if( ((changenum - 1) > 0) && !ppnewchg ){
			DbgOut( "AlpKey : DeleteAlpChange : ppnewchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( ppnewchg, sizeof( CAlpChange* ) * ( changenum - 1 ) );

		int setno = 0;
		int chgno;
		for( chgno = 0; chgno < changenum; chgno++ ){
			if( chgno != index ){
				*( ppnewchg + setno ) = *( alpchange + chgno );
				setno++;
			}
		}

		delete findchg;//!!!!!!!!!!

		changenum--;
		free( alpchange );
		alpchange = ppnewchg;

	}

	return 0;
}

CAlpChange* CAlpKey::FindAlpChange( CMQOMaterial* srcmat, int* pindex )
{
	CAlpChange* retchg = 0;
	*pindex = -1;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CAlpChange* curchg;
		curchg = *( alpchange + chgno );
		if( curchg && ( curchg->mqomat == srcmat ) ){
			retchg = curchg;
			*pindex = chgno;
			break;
		}
	}

	return retchg;
}

int CAlpKey::ExistAlpChange( CMQOMaterial* srcmat, CAlpChange** ppdsc )
{
	*ppdsc = 0;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CAlpChange* curdsc;
		curdsc = *( alpchange + chgno );

		if( curdsc->mqomat == srcmat ){
			*ppdsc = curdsc;
			break;
		}
	}
	return 0;
}

int CAlpKey::FramenoCmp( CAlpKey* cmpdsk )
{
	int cmpno;
	_ASSERT( cmpdsk );
	cmpno = cmpdsk->frameno;

	if( frameno > cmpno ){
		return 1;
	}else if( frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
int CAlpKey::FramenoCmp( int cmpno )
{
	if( frameno > cmpno ){
		return 1;
	}else if( frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CAlpKey::AddToPrev( CAlpKey* adddsk )
{
	CAlpKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	adddsk->prev = prev;
	adddsk->next = this;

	prev = adddsk;

	if( saveprev ){
		saveprev->next = adddsk;
	}
	//else{
	//	_ASSERT( ishead );
	//	addmp->ishead = 1;
	//	ishead = 0;
	//}

	return 0;
}

int CAlpKey::AddToNext( CAlpKey* adddsk )
{
	CAlpKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	adddsk->prev = this;
	adddsk->next = savenext;

	next = adddsk;

	if( savenext ){
		savenext->prev = adddsk;
	}

	//addmp->ishead = 0;

	return 0;
}

int CAlpKey::LeaveFromChain()
{
	CAlpKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	prev = 0;
	next = 0;

	if( saveprev ){
		saveprev->next = savenext;
	}
	//else{
	//	_ASSERT( ishead );
	//	if( savenext )
	//		savenext->ishead = 1;
	//	ishead = 0;
	//}

	if( savenext ){
		savenext->prev = saveprev;
	}
	return 0;
}

int CAlpKey::CopyAlpChange( CAlpKey* srcdsk )
{
	DestroyObjs();

	int ret;
	int dscno;
	for( dscno = 0; dscno < srcdsk->changenum; dscno++ ){
		CAlpChange* srcdsc;
		srcdsc = *( srcdsk->alpchange + dscno );
		if( srcdsc ){
			ret = SetAlpChange( srcdsc->mqomat, srcdsc->change );
			if( ret ){
				DbgOut( "AlpKey : CopyAlpChange : SetAlpChange error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( changenum != srcdsk->changenum ){
		DbgOut( "AlpKey : CopyAlpChange : changenum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
