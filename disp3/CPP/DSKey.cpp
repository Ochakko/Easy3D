#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <DSKey.h>
#include <DSChange.h>
#include <DispSwitch.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CDSKey::CDSKey()
{
	InitParams();
}

CDSKey::~CDSKey()
{
	DestroyObjs();
}

int CDSKey::InitParams()
{
	frameno = 0;
	changenum = 0;
	dschange = 0;
	prev = 0;
	next = 0;

	return 0;
}

int CDSKey::DestroyObjs()
{
	if( dschange ){
		int chgno;
		for( chgno = 0; chgno < changenum; chgno++ ){

			CDSChange* curchg;
			curchg = *( dschange + chgno );

			if( curchg ){
				delete curchg;
			}
		}
		free( dschange );
		dschange = 0;
	}
	changenum = 0;

	return 0;
}

int CDSKey::SetDSChange( CDispSwitch* srcdsptr, int srcstate )
{
	int ret;
	CDSChange* findchg;
	int index;
	findchg = FindDSChange( srcdsptr, &index );
	if( findchg ){
		ret = findchg->SetDSChange( srcstate );
		if( ret ){
			DbgOut( "DSKey : SetDSChange : findchg SetDSChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		dschange = (CDSChange**)realloc( dschange, sizeof( CDSChange* ) * ( changenum + 1 ) );
		if( !dschange ){
			DbgOut( "DSKey : SetDSChange : dschange alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		changenum++;
		*( dschange + changenum - 1 ) = 0;

		CDSChange* newchg;
		newchg = new CDSChange();
		if( !newchg ){
			DbgOut( "DSKey : SetDSChange : newchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		newchg->m_dsptr = srcdsptr;
		ret = newchg->SetDSChange( srcstate );
		if( ret ){
			DbgOut( "DSKey : SetDSChange : newchg SetDSChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*( dschange + changenum - 1 ) = newchg;

	}


	return 0;
}
int CDSKey::DeleteDSChange( CDispSwitch* srcdsptr )
{
	int index;
	CDSChange* findchg;
	findchg = FindDSChange( srcdsptr, &index );

	if( index >= 0 ){
		CDSChange** ppnewchg;
		ppnewchg = (CDSChange**)malloc( sizeof( CDSChange* ) * ( changenum - 1 ) );
		if( ((changenum - 1) > 0) && !ppnewchg ){
			DbgOut( "DSKey : DeleteDSChange : ppnewchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( ppnewchg, sizeof( CDSChange* ) * ( changenum - 1 ) );

		int setno = 0;
		int chgno;
		for( chgno = 0; chgno < changenum; chgno++ ){
			if( chgno != index ){
				*( ppnewchg + setno ) = *( dschange + chgno );
				setno++;
			}
		}

		delete findchg;//!!!!!!!!!!

		changenum--;
		free( dschange );
		dschange = ppnewchg;

	}

	return 0;
}

CDSChange* CDSKey::FindDSChange( CDispSwitch* srcdsptr, int* pindex )
{
	CDSChange* retchg = 0;
	*pindex = -1;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CDSChange* curchg;
		curchg = *( dschange + chgno );
		if( curchg && ( curchg->m_dsptr == srcdsptr ) ){
			retchg = curchg;
			*pindex = chgno;
			break;
		}
	}

	return retchg;
}

int CDSKey::ExistDSChange( CDispSwitch* srcdsptr, CDSChange** ppdsc )
{
	*ppdsc = 0;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CDSChange* curdsc;
		curdsc = *( dschange + chgno );

		if( curdsc->m_dsptr == srcdsptr ){
			*ppdsc = curdsc;
			break;
		}
	}
	return 0;
}

int CDSKey::FramenoCmp( CDSKey* cmpdsk )
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
int CDSKey::FramenoCmp( int cmpno )
{
	if( frameno > cmpno ){
		return 1;
	}else if( frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CDSKey::AddToPrev( CDSKey* adddsk )
{
	CDSKey *saveprev, *savenext;
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

int CDSKey::AddToNext( CDSKey* adddsk )
{
	CDSKey *saveprev, *savenext;
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

int CDSKey::LeaveFromChain()
{
	CDSKey *saveprev, *savenext;
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

int CDSKey::CopyDSChange( CDSKey* srcdsk )
{
	DestroyObjs();

	int ret;
	int dscno;
	for( dscno = 0; dscno < srcdsk->changenum; dscno++ ){
		CDSChange* srcdsc;
		srcdsc = *( srcdsk->dschange + dscno );
		if( srcdsc ){
			ret = SetDSChange( srcdsc->m_dsptr, srcdsc->m_change );
			if( ret ){
				DbgOut( "DSKey : CopyDSChange : SetDSChange error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( changenum != srcdsk->changenum ){
		DbgOut( "DSKey : CopyDSChange : changenum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
