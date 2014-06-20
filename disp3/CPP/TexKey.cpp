#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <TexKey.h>
#include <TexChange.h>
#include <MQOMaterial.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CTexKey::CTexKey()
{
	InitParams();
}

CTexKey::~CTexKey()
{
	DestroyObjs();
}

int CTexKey::InitParams()
{
	frameno = 0;
	changenum = 0;
	texchange = 0;
	prev = 0;
	next = 0;

	return 0;
}

int CTexKey::DestroyObjs()
{
	if( texchange ){
		int chgno;
		for( chgno = 0; chgno < changenum; chgno++ ){

			CTexChange* curchg;
			curchg = *( texchange + chgno );

			if( curchg ){
				delete curchg;
			}
		}
		free( texchange );
		texchange = 0;
	}
	changenum = 0;

	return 0;
}

int CTexKey::SetTexChange( CMQOMaterial* srcmqomat, char* srcname )
{
	int ret;
	CTexChange* findchg;
	int index;
	findchg = FindTexChange( srcmqomat, &index );
	if( findchg ){
		ret = findchg->SetChangeName( srcname );
		if( ret ){
			DbgOut( "texkey : SetTexChange : findchg SetChangeName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		texchange = (CTexChange**)realloc( texchange, sizeof( CTexChange* ) * ( changenum + 1 ) );
		if( !texchange ){
			DbgOut( "texkey : SetTexChange : texchange alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		changenum++;
		*( texchange + changenum - 1 ) = 0;

		CTexChange* newchg;
		newchg = new CTexChange();
		if( !newchg ){
			DbgOut( "texkey : SetTexChange : newchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		newchg->m_mqomat = srcmqomat;
		ret = newchg->SetChangeName( srcname );
		if( ret ){
			DbgOut( "texkey : SetTexChange : newchg SetChangeName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*( texchange + changenum - 1 ) = newchg;

	}


	return 0;
}
int CTexKey::DeleteAllChange()
{
	return DestroyObjs();
}

int CTexKey::DeleteTexChange( CMQOMaterial* srcmqomat )
{
	int index;
	CTexChange* findchg;
	findchg = FindTexChange( srcmqomat, &index );

	if( index >= 0 ){
		CTexChange** ppnewchg;
		ppnewchg = (CTexChange**)malloc( sizeof( CTexChange* ) * ( changenum - 1 ) );
		if( ((changenum - 1) > 0) && !ppnewchg ){
			DbgOut( "texkey : DeleteTexChange : ppnewchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( ppnewchg, sizeof( CTexChange* ) * ( changenum - 1 ) );

		int setno = 0;
		int chgno;
		for( chgno = 0; chgno < changenum; chgno++ ){
			if( chgno != index ){
				*( ppnewchg + setno ) = *( texchange + chgno );
				setno++;
			}
		}

		delete findchg;//!!!!!!!!!!

		changenum--;
		free( texchange );
		texchange = ppnewchg;

	}

	return 0;
}

CTexChange* CTexKey::FindTexChange( CMQOMaterial* srcmqomat, int* pindex )
{
	CTexChange* retchg = 0;
	*pindex = -1;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CTexChange* curchg;
		curchg = *( texchange + chgno );
		if( curchg && ( curchg->m_mqomat == srcmqomat ) ){
			retchg = curchg;
			*pindex = chgno;
			break;
		}
	}

	return retchg;
}

int CTexKey::ExistTexChange( CMQOMaterial* srcmat, CTexChange** pptc )
{
	*pptc = 0;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CTexChange* curtc;
		curtc = *( texchange + chgno );

		if( curtc->m_mqomat == srcmat ){
			*pptc = curtc;
			break;
		}
	}
	return 0;
}

int CTexKey::FramenoCmp( CTexKey* cmptk )
{
	int cmpno;
	_ASSERT( cmptk );
	cmpno = cmptk->frameno;

	if( frameno > cmpno ){
		return 1;
	}else if( frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}
int CTexKey::FramenoCmp( int cmpno )
{
	if( frameno > cmpno ){
		return 1;
	}else if( frameno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

int CTexKey::AddToPrev( CTexKey* addtk )
{
	CTexKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addtk->prev = prev;
	addtk->next = this;

	prev = addtk;

	if( saveprev ){
		saveprev->next = addtk;
	}
	//else{
	//	_ASSERT( ishead );
	//	addmp->ishead = 1;
	//	ishead = 0;
	//}

	return 0;
}

int CTexKey::AddToNext( CTexKey* addtk )
{
	CTexKey *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addtk->prev = this;
	addtk->next = savenext;

	next = addtk;

	if( savenext ){
		savenext->prev = addtk;
	}

	//addmp->ishead = 0;

	return 0;
}

int CTexKey::LeaveFromChain()
{
	CTexKey *saveprev, *savenext;
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

int CTexKey::CopyTexChange( CTexKey* srctk )
{
	DestroyObjs();

	int ret;
	int tcno;
	for( tcno = 0; tcno < srctk->changenum; tcno++ ){
		CTexChange* srctc;
		srctc = *( srctk->texchange + tcno );
		if( srctc ){
			ret = SetTexChange( srctc->m_mqomat, srctc->m_change );
			if( ret ){
				DbgOut( "texkey : CopyTexChange : SetTexChange error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( changenum != srctk->changenum ){
		DbgOut( "texkey : CopyTexChange : changenum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
