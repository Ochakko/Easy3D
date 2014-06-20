#include "stdafx.h"

#include <MCHandler.h>

//#include <coef.h>
#define DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>

#include <forbidid.h>


static int compareMCE( void *context, const void *elem1, const void *elem2);
static int GetAndNum( int srcno );


CMCHandler::CMCHandler( CMotHandler* srcmh, int srcfillupleng )
{
	InitParams();
	m_mh = srcmh;
	m_fillupleng = srcfillupleng;
}

CMCHandler::~CMCHandler()
{
	DestroyObjs();
}

int CMCHandler::InitParams()
{
	m_mh = 0;
	m_mcnum = 0;
	m_mcarray = 0;
	m_fillupleng = 10;

	//m_totalmcnum = 0;

	return 0;
}

int CMCHandler::DestroyObjs()
{
	if( m_mcarray ){

		int mcno;
		for( mcno = 0; mcno < m_mcnum; mcno++ ){
			MCELEM* curmce;
			curmce = m_mcarray + mcno;

			if( curmce->childmc ){
				free( curmce->childmc );
				curmce->childmc = 0;
			}
			curmce->childnum = 0;

			if( curmce->forbidid ){
				free( curmce->forbidid );
				curmce->forbidid = 0;
			}
			curmce->forbidnum = 0;
		}

		free( m_mcarray );
		m_mcarray = 0;
	}
	m_mcnum = 0;

	return 0;
}

int CMCHandler::InitMCArray( int motnum, MOTID* motidarray, int srcidlingid )
{

//DbgOut( "check !!! : mchandler : InitMCArray \r\n" );
	int ret;

	if( !m_mcarray ){
		m_mcnum = motnum;
		//m_totalmcnum = motnum;

		m_mcarray = (MCELEM*)malloc( sizeof( MCELEM ) * m_mcnum );
		if( !m_mcarray ){
			DbgOut( "mchandler : InitMCArray : mcarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_mcarray, sizeof( MCELEM ) * m_mcnum );


		int mcno;
		for( mcno = 0; mcno < m_mcnum; mcno++ ){
			(m_mcarray + mcno)->setno = mcno;
			(m_mcarray + mcno)->id = (motidarray + mcno)->id;

			if( srcidlingid == (m_mcarray + mcno)->id ){
				(m_mcarray + mcno)->idling = 1;
			}else{
				(m_mcarray + mcno)->idling = 0;
			}
			(m_mcarray + mcno)->frameno1 = -1;
			(m_mcarray + mcno)->frameno2 = 0;

			(m_mcarray + mcno)->ev0idle = (motidarray + mcno)->ev0idle;
			(m_mcarray + mcno)->commonid = (motidarray + mcno)->commonid;
			ret = SetMCEForbidID( m_mcarray + mcno, (motidarray + mcno)->forbidnum, (motidarray + mcno)->forbidid );
			if( ret ){
				DbgOut( "mch : InitMCArray : SetMCEForbidID error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			(m_mcarray + mcno)->notfu = (motidarray + mcno)->notfu;
		}

	}else{
		if( m_mcnum == motnum ){
			//何もしない
		}else if( m_mcnum < motnum ){

			m_mcarray = (MCELEM*)realloc( m_mcarray, sizeof( MCELEM ) * motnum );
			if( !m_mcarray ){
				DbgOut( "mchandler : InitMCArray : mcarray realloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			int mcno;
			for( mcno = m_mcnum; mcno < motnum; mcno++ ){
				(m_mcarray + mcno)->id = (motidarray + mcno)->id;
				(m_mcarray + mcno)->childnum = 0;
				(m_mcarray + mcno)->childmc = 0;
				//(m_mcarray + mcno)->idling = 0;

				if( srcidlingid == (m_mcarray + mcno)->id ){
					(m_mcarray + mcno)->idling = 1;
				}else{
					(m_mcarray + mcno)->idling = 0;
				}

				(m_mcarray + mcno)->frameno1 = -1;
				(m_mcarray + mcno)->frameno2 = 0;
				(m_mcarray + mcno)->eventno1 = 0;
				(m_mcarray + mcno)->eventno2 = 0;

				(m_mcarray + mcno)->ev0idle = (motidarray + mcno)->ev0idle;
				(m_mcarray + mcno)->commonid = (motidarray + mcno)->commonid;

				(m_mcarray + mcno)->forbidid = 0;//初期化
				ret = SetMCEForbidID( m_mcarray + mcno, (motidarray + mcno)->forbidnum, (motidarray + mcno)->forbidid );
				if( ret ){
					DbgOut( "mch : InitMCArray : SetMCEForbidID error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				(m_mcarray + mcno)->notfu = (motidarray + mcno)->notfu;
				(m_mcarray + mcno)->closetree = 0;
				(m_mcarray + mcno)->nottoidle = 0;
			}

			//m_totalmcnum += (motnum - m_mcnum);
			m_mcnum = motnum;

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//setnoの付け直し
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			ReorderSetno();

		}else{
			DbgOut( "mchandler : InitMCArray : motnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMCHandler::GetTotalMCNum()
{
	int mcno;
	int childcnt = 0;

	if( m_mcarray ){
		for( mcno = 0; mcno < m_mcnum; mcno++ ){
			childcnt += (m_mcarray + mcno)->childnum;
		}

		return (childcnt + m_mcnum);
	}else{
		return 0;
	}
}

int CMCHandler::Setno2MotID( int srcsetno )
{

	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->setno == srcsetno ){
			return curmce->id;
		}

		int cno;
		for( cno = 0; cno < curmce->childnum; cno++ ){
			MCELEM* curchild;
			curchild = curmce->childmc + cno;

			if( curchild->setno == srcsetno ){
				return curchild->id;
			}
		}
	}

	return -1;
}

int CMCHandler::MotID2Setno( int srcmotid )
{
	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == srcmotid ){
			return curmce->setno;
		}
	}

	return -1;
}


MCELEM* CMCHandler::Setno2MCElem( int srcsetno, MCELEM** parentmce )
{
	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->setno == srcsetno ){
			*parentmce = 0;
			return curmce;
		}

		int cno;
		for( cno = 0; cno < curmce->childnum; cno++ ){
			MCELEM* curchild;
			curchild = curmce->childmc + cno;

			if( curchild->setno == srcsetno ){
				*parentmce = curmce;
				return curchild;
			}
		}
	}

	return 0;
}


int CMCHandler::AddChild( int parentsetno, int srccookie, int srcalways, int srcframeno1, int srcframeno2, 
	int srceventno1, int srcnotfu, int srcnottoidle )
{
	MCELEM* parmce;
	MCELEM* gparmce;
	parmce = Setno2MCElem( parentsetno, &gparmce );
	if( !parmce ){
		DbgOut( "mchandler : AddChild : parmc NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int curchildnum = parmce->childnum;
	MCELEM* newchildmc;
	newchildmc = (MCELEM*)realloc( parmce->childmc, sizeof( MCELEM ) * (curchildnum + 1) );
	if( !newchildmc ){
		DbgOut( "mchandler : AddChild : childmc alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	parmce->childmc = newchildmc;
	parmce->childnum = curchildnum + 1;


	MCELEM* setmc;
	setmc = newchildmc + curchildnum;
	
	setmc->id = srccookie;
	if( srcalways == 0 ){
		setmc->frameno1 = srcframeno1;
	}else{
		setmc->frameno1 = -1;
	}
	setmc->frameno2 = srcframeno2;
	setmc->eventno1 = srceventno1;
	setmc->eventno2 = 0;

	setmc->childnum = 0;
	setmc->childmc = 0;
	setmc->idling = 0;

	setmc->notfu = srcnotfu;
	setmc->nottoidle = srcnottoidle;

	//以下３つは親用メンバー
	setmc->ev0idle = 0;
	setmc->commonid = 0;
	setmc->forbidnum = 0;
	setmc->forbidid = 0;
	
	setmc->closetree = 0;
	///////////

	QSortChild( parmce );

	//////////
	ReorderSetno();


	return 0;
}

void CMCHandler::ReorderSetno()
{

	int setno = 0;
	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		(m_mcarray + mcno)->setno = setno;
		setno++;

		int cnum;
		for( cnum = 0; cnum < (m_mcarray + mcno)->childnum; cnum++ ){
			((m_mcarray + mcno)->childmc + cnum)->setno = setno;
			setno++;
		}
	}

}

int CMCHandler::SetMCElem( int srcsetno, int srccookie, int srcalways, int srcframeno1, int srcframeno2, 
	int srceventno1, int srcnotfu, int idling, int srcnottoidle )
{
	MCELEM* setmce;
	MCELEM* parmce;
	setmce = Setno2MCElem( srcsetno, &parmce );
	if( !setmce ){
		DbgOut( "mchandler : SetMCElem : srcsetno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	setmce->id = srccookie;
	if( srcalways == 0 ){
		setmce->frameno1 = srcframeno1;
	}else{
		setmce->frameno1 = -1;
	}
	setmce->frameno2 = srcframeno2;

	setmce->eventno1 = srceventno1;

	setmce->notfu = srcnotfu;

	setmce->idling = idling;

	setmce->nottoidle = srcnottoidle;

	int ret;
	ret = QSortChild( parmce );
	if( ret ){
		DbgOut( "mchandler : SetMCElem : QSortChild error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////////
	ReorderSetno();


	return 0;
}


int CMCHandler::DeleteChild( MCELEM* parmce, MCELEM* chilmce )
{

	MCELEM* newchild;
	int curchildnum = parmce->childnum;
	newchild = (MCELEM*)malloc( sizeof( MCELEM ) * ( curchildnum - 1 ) );
	if( !newchild ){
		DbgOut( "mchandler : DeleteChild : newchild alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int cno;
	int setno2 = 0;
	for( cno = 0; cno < curchildnum; cno++ ){
		MCELEM* chkmce;
		chkmce = parmce->childmc + cno;

		if( chkmce != chilmce ){
			*(newchild + setno2) = *chkmce;
			setno2++;
		}
	}
	if( setno2 != (curchildnum - 1) ){
		DbgOut( "mchandler : DeleteChild : setno2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	free( parmce->childmc );
	parmce->childnum = curchildnum - 1;
	parmce->childmc = newchild;

	return 0;
}

int CMCHandler::DeleteMCElem( int srcsetno )
{
	int ret;
	MCELEM* delmce;
	MCELEM* parmce = 0;

	delmce = Setno2MCElem( srcsetno, &parmce );
	if( !delmce ){
		DbgOut( "mchandler : DeleteMCElem : srcsetno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "check !!! : mchandler : DeleteMCElem : srcsetno %d, delmce->setno %d, delmce->id %d\r\n",
//		srcsetno, delmce->setno, delmce->id );


	int delcookie;
	delcookie = delmce->id;

	if( parmce ){

		ret = DeleteChild( parmce, delmce );
		if( ret ){
			DbgOut( "mchandler : Setno2MCElem : DeleteChild error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ReorderSetno();

	}else{

		MCELEM* newmcarray;
		newmcarray = (MCELEM*)malloc( sizeof( MCELEM ) * (m_mcnum - 1) );
		if( !newmcarray ){
			DbgOut( "mchandler : DeleteMCElem : newmcarray alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		int mcno;
		int setno3 = 0;
		for( mcno = 0; mcno < m_mcnum; mcno++ ){
			MCELEM* chkmce;
			chkmce = m_mcarray + mcno;

			if( chkmce != delmce ){
				*(newmcarray + setno3) = *chkmce;
				setno3++;
			}
		}
		if( setno3 != (m_mcnum - 1 ) ){
			DbgOut( "mchandler : DeleteMCElem : setno3 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( delmce->childmc ){
//DbgOut( "check !!! : DeleteMCElem : free delmce->childmc setno %d, id %d\r\n", delmce->setno, delmce->id );
			free( delmce->childmc );
		}

		if( delmce->forbidid ){
			free( delmce->forbidid );
		}

		free( m_mcarray );

		m_mcnum--;
		m_mcarray = newmcarray;

		//ReorderSetno();


		//子供項目の削除
		for( mcno = 0; mcno < m_mcnum; mcno++ ){
			MCELEM* curmce;
			curmce = m_mcarray + mcno;

			MCELEM* delchild = 0;
			int isfirst = 1;
			while( delchild || isfirst ){

				delchild = 0;
				int cno;
				for( cno = 0; cno < curmce->childnum; cno++ ){
					MCELEM* chkchild;
					chkchild = curmce->childmc + cno;

					if( chkchild->id == delcookie ){
						delchild = chkchild;
						break;
					}
				}

				if( delchild ){

//DbgOut( "check !!! : mchandler : DeleteMCElem : DeleteChild %d\r\n", delchild->id );

					ret = DeleteChild( curmce, delchild );
					if( ret ){
						DbgOut( "mchandler : DeleteMCElem : del child : DeleteChild error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				isfirst = 0;
			}
		}


		ReorderSetno();


		//mce->idの振りなおし
		int* old2newid;
		old2newid = (int*)malloc( sizeof( int ) * (m_mcnum + 1) );
		if( !old2newid ){
			DbgOut( "mchandler : DeleteMCElem : old2newid alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		int setno4 = 0;
		for( mcno = 0; mcno < (m_mcnum + 1); mcno++ ){
			if( mcno != delcookie ){
				*(old2newid + mcno) = setno4;
				setno4++;
			}else{
				*(old2newid + mcno) = -1;
			}
		}

		for( mcno = 0; mcno < m_mcnum; mcno++ ){
			MCELEM* curmce;
			curmce = m_mcarray + mcno;
			
			curmce->id = *(old2newid + curmce->id);
			if( curmce->id == -1 ){
				_ASSERT( 0 );
			}

			int cno;
			for( cno = 0; cno < curmce->childnum; cno++ ){
				MCELEM* curchild;
				curchild = curmce->childmc + cno;

				curchild->id = *(old2newid + curchild->id);
				if( curchild->id == -1 ){
					_ASSERT( 0 );
				}
			}
		}

		free( old2newid );


	}


	return 0;
}

int CMCHandler::AddParentMC( int addcookie, int srcidling, int srcev0idle, int srccommonid, int srcforbidnum, int* srcforbidid, int srcnotfu )
{
	//if( addcookie != m_mcnum ){
	//	DbgOut( "mchandler : AddParentMC : addcookie error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}
	int ret;

	MCELEM* newarray;
	newarray = (MCELEM*)realloc( m_mcarray, sizeof( MCELEM ) * (m_mcnum + 1) );
	if( !newarray ){
		DbgOut( "mchandler : AddParentMC : newarray alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MCELEM* addelem;
	addelem = newarray + m_mcnum;


	addelem->childnum = 0;
	addelem->childmc = 0;
	addelem->id = addcookie;
	addelem->idling = srcidling;
	addelem->frameno1 = -1;
	addelem->frameno2 = 0;
	addelem->eventno1 = 0;
	addelem->eventno2 = 0;
	addelem->nottoidle = 0;

	addelem->ev0idle = srcev0idle;
	addelem->commonid = srccommonid;

	addelem->forbidid = 0;
	ret = SetMCEForbidID( addelem, srcforbidnum, srcforbidid );
	if( ret ){
		DbgOut( "mch : AddParentMC : SetMCEForbidID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	addelem->notfu = srcnotfu;
	addelem->closetree = 0;

	m_mcarray = newarray;
	m_mcnum++;

	
	ReorderSetno();


DbgOut( "check !!! : mchandler : AddParentMC : addcookie %d, srcidling %d\r\n",
	addcookie, srcidling );


	return 0;
}

int CMCHandler::QSortChild( MCELEM* parmce )
{
	if( !parmce ){
		_ASSERT( 0 );
		return 0;//!!!!!!!
	}


	if( (parmce->childnum >= 2) && parmce->childmc ){

		qsort_s( parmce->childmc, parmce->childnum, sizeof( MCELEM ), compareMCE, parmce );	

	}

	return 0;
}


int compareMCE( void *context, const void *elem1, const void *elem2)
{
	MCELEM* mce1;
	MCELEM* mce2;
	mce1 = (MCELEM*)elem1;
	mce2 = (MCELEM*)elem2;


	if( (mce1->frameno1 == -1) && (mce2->frameno1 != -1) ){
		return 1;
	}else if( (mce1->frameno1 != -1) && (mce2->frameno1 == -1) ){
		return -1;
	}else{
		if( mce1->frameno1 != mce2->frameno1 ){
			return ( mce1->frameno1 - mce2->frameno1 );
		}else{
			//frameno1が同じときは、イベント番号の大きさでソート

			return ( mce1->eventno1 - mce2->eventno1 );
		}
	}
}


int CMCHandler::SetIdlingMotion( CShdHandler* srclpsh, int exceptfirst )
{
	int ret;

	int idlingid = -1;
	idlingid = GetIdlingMotID();

	if( idlingid < 0 ){
		if( exceptfirst ){
			if( m_mcnum >= 2 ){
				(m_mcarray + 1)->idling = 1;//!!!!!!!!!
				idlingid = (m_mcarray + 1)->id;
			}else{
				DbgOut( "mchandler : SetIdlingMotion : idling motion not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;//!!!!!!!!!
			}
		}else{
			if( m_mcnum >= 1 ){
				(m_mcarray + 0)->idling = 1;//!!!!!!!!!
				idlingid = (m_mcarray + 0)->id;
			}else{
				DbgOut( "mchandler : SetIdlingMotion : idling motion not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;//!!!!!!!!!
			}
		}
	}

	ret = m_mh->SetMotionKind( idlingid );
	if( ret ){
		DbgOut( "mchandler : SetIdlingMotion : mh SetMotionKind error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD tick;
	tick = GetTickCount();

	ret = m_mh->SetTickCount( -1, tick );
	_ASSERT( !ret );

	/***
	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( ( (exceptfirst == 0) || (mcno != 0) ) && (curmce->id != idlingid) ){
			ret = m_mh->SetNextMotionFrameNo( srclpsh, curmce->id, idlingid, 0 );
			DbgOut( "mchandler : SetIdlingMotionNext : mh SetNextMotionFrameNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	***/
	return 0;
}

int CMCHandler::GetIdlingMotID()
{
	int idlingid = -1;

	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->idling == 1 ){
			idlingid = curmce->id;
			break;
		}
	}

	return idlingid;
}

int CMCHandler::ChangeIdlingMotion( CShdHandler* srclpsh, int srcmotid )
{
	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == srcmotid ){
			curmce->idling = 1;
		}else{
			curmce->idling = 0;
		}
	}

	int ret;
	ret = SetIdlingMotion( srclpsh, 1 );
	if( ret ){
		DbgOut( "mch : ChangeIdlingMotion : SetIdlingMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMCHandler::GetEv0Idle( int srcmotid, int* dstev0idle )
{
	*dstev0idle = 0;//!!!!!!!

	int mcno;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == srcmotid ){
			*dstev0idle = curmce->ev0idle;//!!!!!!!!
			break;
		}
	}

	return 0;
}

int CMCHandler::GetNextMotion( int srccurmotid, int srccurframe, int srceventno, 
	int* nextmotidptr, int* nextframeptr, int* notfuptr, int* nottoidleptr )
{
	//int ret;

	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == srccurmotid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : GetNextMotion : parentmce NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//共通分岐チェック
	MCELEM* nextmce = 0;
	MCELEM* chkmce;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		chkmce = m_mcarray + mcno;

		int hitflag;
		hitflag = HitTestForbidID( parentmce, chkmce->commonid );
		if( hitflag == 0 ){
			if( (chkmce->commonid == srceventno) && (srceventno != 0) ){
				nextmce = chkmce;
				break;
			}
		}
	}
	if( nextmce ){
		*nextmotidptr = chkmce->id;
		*nextframeptr = 0;
		*notfuptr = chkmce->notfu;
		*nottoidleptr = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	//Branch設定チェック
	int chilno;
	for( chilno = 0; chilno < parentmce->childnum; chilno++ ){
		MCELEM* chilmce;
		chilmce = parentmce->childmc + chilno;

		if( (chilmce->frameno1 == -1) || (chilmce->frameno1 >= srccurframe) ){

			if( (chilmce->eventno1 == srceventno) && (srceventno != 0) ){
				nextmce = chilmce;
				break;
			}
		}
	}



	if( nextmce ){
		*nextmotidptr = nextmce->id;
		*nextframeptr = nextmce->frameno2;
		*notfuptr = nextmce->notfu;
		*nottoidleptr = nextmce->nottoidle;
	}else{
		*nextmotidptr = srccurmotid;
		*nextframeptr = srccurframe;
		*notfuptr = 0;
		*nottoidleptr = -1;
	}


//if( srceventno != 0 ){
//	DbgOut( "mchandler : GetNextMotion : userevent %d, nextmotid %d, nextframe %d\r\n",
//		srceventno, *nextmotidptr, *nextframeptr );
//
//}


	return 0;
}

int CMCHandler::GetTrunkNotComID( int motid, int arrayleng, int* dstid, int* getnum )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == motid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : GetTrunkNotComID : parentmce NULL return !!!\n" );
		//_ASSERT( 0 );
		*getnum = 0;
		return 1;
	}

	*getnum = parentmce->forbidnum;

	if( (arrayleng > 0) && (arrayleng < parentmce->forbidnum) ){
		DbgOut( "mch : GetTrunkNotComID : arrayleng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( arrayleng > 0 ){
		MoveMemory( dstid, parentmce->forbidid, sizeof( int ) * *getnum );
	}

	return 0;
}
int CMCHandler::SetTrunkNotComID( int motid, int arrayleng, int* srcid )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == motid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : SetTrunkNotComID : parentmce NULL return !!!\n" );
		//_ASSERT( 0 );
		return 1;
	}

	parentmce->forbidnum = arrayleng;
	if( parentmce->forbidid ){
		free( parentmce->forbidid );
		parentmce->forbidid = 0;
	}
	parentmce->forbidid = (int*)malloc( sizeof( int ) * arrayleng );
	if( !parentmce->forbidid ){
		DbgOut( "mch : SetTrunkNotComID : forbidid alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	MoveMemory( parentmce->forbidid, srcid, sizeof( int ) * arrayleng );

	return 0;
}


int CMCHandler::GetTrunkInfo( int motid, MOATRUNKINFO* trunkptr )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == motid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : GetTrunkInfo : parentmce NULL return !!!\n" );
		//_ASSERT( 0 );
		trunkptr->idling = 0;
		trunkptr->ev0idle = 0;
		trunkptr->comid = 0;
		trunkptr->notcomnum = 0;
		trunkptr->notcomid = 0;
		trunkptr->branchnum = 0;
		return 0;
	}

	trunkptr->idling = parentmce->idling;
	trunkptr->ev0idle = parentmce->ev0idle;
	trunkptr->comid = parentmce->commonid;
	trunkptr->notcomnum = parentmce->forbidnum;
	trunkptr->notcomid = parentmce->forbidid;
	trunkptr->branchnum = parentmce->childnum;

	return 0;



}

int CMCHandler::GetTrunkInfo( int motid, int* infoptr )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == motid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : GetTrunkInfo : parentmce NULL return !!!\n" );
		//_ASSERT( 0 );

		*( infoptr + MOAT_IDLING ) = 0;
		*( infoptr + MOAT_EV0IDLE ) = 0;
		*( infoptr + MOAT_COMID ) = 0;
		*( infoptr + MOAT_NOTCOMID ) = 0;
		*( infoptr + MOAT_BRANCHNUM ) = 0;

		return 0;
	}

	*( infoptr + MOAT_IDLING ) = parentmce->idling;
	*( infoptr + MOAT_EV0IDLE ) = parentmce->ev0idle;
	*( infoptr + MOAT_COMID ) = parentmce->commonid;
	*( infoptr + MOAT_NOTCOMID ) = parentmce->forbidnum;
	*( infoptr + MOAT_BRANCHNUM ) = parentmce->childnum;

	return 0;
}

int CMCHandler::GetBranchInfo( int motid, MOABRANCHINFO* branchptr, int branchnum, int* getnum )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == motid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : GetBranchInfo : parentmce NULL return !!!\n" );
		//_ASSERT( 0 );
		*getnum = 0;
		return 0;
	}

	if( branchnum < parentmce->childnum ){
		DbgOut( "mch : GetBranchInfo : branchnum too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int setno = 0;
	MCELEM* curchil;
	int chilno;
	for( chilno = 0; chilno < parentmce->childnum; chilno++ ){
		curchil = parentmce->childmc + chilno;

		(branchptr + setno)->motid = curchil->id;
		(branchptr + setno)->eventno = curchil->eventno1;
		(branchptr + setno)->frameno1 = curchil->frameno1;
		(branchptr + setno)->frameno2 = curchil->frameno2;
		(branchptr + setno)->notfu = curchil->notfu;
		setno++;
	}

	*getnum = setno;

	return 0;
}


int CMCHandler::GetBranchInfo( int motid, int* infoptr, int branchnum, int* getnum )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == motid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : GetBranchInfo : parentmce NULL return !!!\n" );
		//_ASSERT( 0 );
		*getnum = 0;
		return 0;
	}

	if( branchnum < parentmce->childnum ){
		DbgOut( "mch : GetBranchInfo : branchnum too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int setno = 0;
	MCELEM* curchil;
	int chilno;
	for( chilno = 0; chilno < parentmce->childnum; chilno++ ){
		curchil = parentmce->childmc + chilno;

		*( infoptr + MOAB_MAX * setno + MOAB_MOTID ) = curchil->id;
		*( infoptr + MOAB_MAX * setno + MOAB_EVENTID ) = curchil->eventno1;
		*( infoptr + MOAB_MAX * setno + MOAB_FRAME1 ) = curchil->frameno1;
		*( infoptr + MOAB_MAX * setno + MOAB_FRAME2 ) = curchil->frameno2;
		*( infoptr + MOAB_MAX * setno + MOAB_NOTFU ) = curchil->notfu;

		setno++;
	}

	*getnum = setno;

	return 0;
}

int CMCHandler::SetBranchFrame1( int tmotid, int bmotid, int frame1 )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == tmotid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : SetBranchFrame1 : parentmce NULL return !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	MCELEM* chilmce = 0;
	int cno;
	for( cno = 0; cno < parentmce->childnum; cno++ ){
		MCELEM* curmce;
		curmce = parentmce->childmc + cno;

		if( curmce->id == bmotid ){
			chilmce = curmce;
			break;
		}
	}
	if( !chilmce ){
		DbgOut( "mch : SetBranchFrame1 : chilmce NULL return !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	chilmce->frameno1 = frame1;


	return 0;
}
int CMCHandler::SetBranchFrame2( int tmotid, int bmotid, int frame2 )
{
	int mcno;
	MCELEM* parentmce = 0;
	for( mcno = 0; mcno < m_mcnum; mcno++ ){
		MCELEM* curmce;
		curmce = m_mcarray + mcno;

		if( curmce->id == tmotid ){
			parentmce = curmce;
			break;
		}
	}
	if( !parentmce ){
		DbgOut( "mch : SetBranchFrame2 : parentmce NULL return !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	MCELEM* chilmce = 0;
	int cno;
	for( cno = 0; cno < parentmce->childnum; cno++ ){
		MCELEM* curmce;
		curmce = parentmce->childmc + cno;

		if( curmce->id == bmotid ){
			chilmce = curmce;
			break;
		}
	}
	if( !chilmce ){
		DbgOut( "mch : SetBranchFrame2 : chilmce NULL return !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	chilmce->frameno2 = frame2;


	return 0;
}
