#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <TexKeyPt.h>
#include <TexChangePt.h>
#include <MQOMaterial.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CTexKeyPt::CTexKeyPt()
{
	InitParams();
}

CTexKeyPt::~CTexKeyPt()
{
	DestroyObjs();
}

int CTexKeyPt::InitParams()
{
	changenum = 0;
	texchange = 0;

	return 0;
}

int CTexKeyPt::DestroyObjs()
{
	if( texchange ){
		int chgno;
		for( chgno = 0; chgno < changenum; chgno++ ){

			CTexChangePt* curchg;
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

int CTexKeyPt::SetTexChangePt( CMQOMaterial* srcmqomat, char* srcname )
{
	int ret;
	CTexChangePt* findchg;
	int index;
	findchg = FindTexChangePt( srcmqomat, &index );
	if( findchg ){
		ret = findchg->SetChangeName( srcname );
		if( ret ){
			DbgOut( "texkey : SetTexChangePt : findchg SetChangeName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		texchange = (CTexChangePt**)realloc( texchange, sizeof( CTexChangePt* ) * ( changenum + 1 ) );
		if( !texchange ){
			DbgOut( "texkey : SetTexChangePt : texchange alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		changenum++;
		*( texchange + changenum - 1 ) = 0;

		CTexChangePt* newchg;
		newchg = new CTexChangePt();
		if( !newchg ){
			DbgOut( "texkey : SetTexChangePt : newchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		newchg->m_mqomat = srcmqomat;
		ret = newchg->SetChangeName( srcname );
		if( ret ){
			DbgOut( "texkey : SetTexChangePt : newchg SetChangeName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*( texchange + changenum - 1 ) = newchg;

	}


	return 0;
}
int CTexKeyPt::DeleteAllChangePt()
{
	return DestroyObjs();
}

int CTexKeyPt::DeleteTexChangePt( CMQOMaterial* srcmqomat )
{
	int index;
	CTexChangePt* findchg;
	findchg = FindTexChangePt( srcmqomat, &index );

	if( index >= 0 ){
		CTexChangePt** ppnewchg;
		ppnewchg = (CTexChangePt**)malloc( sizeof( CTexChangePt* ) * ( changenum - 1 ) );
		if( ((changenum - 1) > 0) && !ppnewchg ){
			DbgOut( "texkey : DeleteTexChangePt : ppnewchg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( ppnewchg, sizeof( CTexChangePt* ) * ( changenum - 1 ) );

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

CTexChangePt* CTexKeyPt::FindTexChangePt( CMQOMaterial* srcmqomat, int* pindex )
{
	CTexChangePt* retchg = 0;
	*pindex = -1;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CTexChangePt* curchg;
		curchg = *( texchange + chgno );
		if( curchg && ( curchg->m_mqomat == srcmqomat ) ){
			retchg = curchg;
			*pindex = chgno;
			break;
		}
	}

	return retchg;
}

int CTexKeyPt::ExistTexChangePt( CMQOMaterial* srcmat, CTexChangePt** pptc )
{
	*pptc = 0;

	int chgno;
	for( chgno = 0; chgno < changenum; chgno++ ){
		CTexChangePt* curtc;
		curtc = *( texchange + chgno );

		if( curtc->m_mqomat == srcmat ){
			*pptc = curtc;
			break;
		}
	}
	return 0;
}


int CTexKeyPt::CopyTexChangePt( CTexKeyPt* srctk )
{
	DestroyObjs();

	int ret;
	int tcno;
	for( tcno = 0; tcno < srctk->changenum; tcno++ ){
		CTexChangePt* srctc;
		srctc = *( srctk->texchange + tcno );
		if( srctc ){
			ret = SetTexChangePt( srctc->m_mqomat, srctc->m_change );
			if( ret ){
				DbgOut( "texkey : CopyTexChangePt : SetTexChangePt error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( changenum != srctk->changenum ){
		DbgOut( "texkey : CopyTexChangePt : changenum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
