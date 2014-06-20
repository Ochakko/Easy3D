#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <MState.h>
#include <Matrix2.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <shdhandler.h>
#include <mothandler.h>
#include <shdelem.h>
#include <motionctrl.h>
#include <motioninfo.h>

#include <morph.h>

#include <TexChangePt.h>
#include <MMotElem.h>
#include <AlpChange.h>
#include <mqomaterial.h>

#include <DispSwitch.h>

CMState::CMState() : m_tk(), m_mk(), m_ak()
{
	InitParams();
}
CMState::~CMState()
{
	DestroyObjs();
}

int CMState::InitParams()
{
//int m_bonenum;
//MSELEM* m_mselem;	

	m_bonenum = 0;
	m_mselem = 0;
	m_keyflag = 0;

	ZeroMemory( m_ds, sizeof( int ) * DISPSWITCHNUM );

	return 0;
}
int CMState::DestroyObjs()
{
	if( m_mselem ){
		free( m_mselem );
		m_mselem = 0;
	}
	m_bonenum = 0;

	int ret;

	ret = m_tk.DeleteAllChangePt();
	if( ret ){
		DbgOut( "ms : DestroyObjs : tk DeleteAllChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mk.DeleteAllElem();
	if( ret ){
		DbgOut( "ms : DestroyObjs : mk DeleteAllElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_ak.DeleteAllChange();
	if( ret ){
		DbgOut( "ms : DestroyObjs : ak DeleteAllChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "checkm!!! : ms DestroyObjs\r\n" );

	return 0;
}

int CMState::CreateMatrix( CShdHandler* lpsh, int bonenum )
{
	DestroyObjs();

	m_bonenum = bonenum;

	m_mselem = (MSELEM*)malloc( sizeof( MSELEM ) * m_bonenum );
	if( !m_mselem ){
		DbgOut( "mstate : CreateMatrix : mselem alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int eno;
	for( eno = 0; eno < m_bonenum; eno++ ){
		MSELEM* curmse = m_mselem + eno;
		curmse->bonemat.Identity();
		ZeroMemory( curmse->localinf, sizeof( int ) * MSLOCAL_MAX );
	}


	int dsno;
	for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
		m_ds[dsno] = 1;
	}

	int ret;

	CMQOMaterial* curmat;
	curmat = lpsh->m_mathead;
	while( curmat ){
		ret = m_tk.SetTexChangePt( curmat, 0 );
		if( ret ){
			DbgOut( "ms : CreateMatrix : tk SetTexChangePt error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = m_ak.SetAlpChange( curmat, curmat->orgalpha );
		if( ret ){
			DbgOut( "ms : CreateMatrix : ak SetAlpChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		curmat = curmat->next;
	}


	int mno;
	for( mno = 0; mno < lpsh->m_mnum; mno++ ){
		CShdElem* melem = *(lpsh->m_ppm + mno);
		CMorph* curmorph = melem->morph;
		if( curmorph ){
			CShdElem* baseptr = curmorph->m_baseelem;
			int tno;
			for( tno = 0; tno < curmorph->m_targetnum; tno++ ){
				TARGETPRIM* curtp = curmorph->m_ptarget + tno;
				CShdElem* targetptr = curtp->selem;

				ret = m_mk.SetValue( baseptr, targetptr, 0.0f );
				if( ret ){
					DbgOut( "ms : CreateMatrix : mk SetValue error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

//DbgOut( "checkm!!! : ms CreateMatrix : lpsh->m_mnum %d\r\n", lpsh->m_mnum );


	return 0;
}
int CMState::SetMSElem( int boneno, CMatrix2* srcmat )
{
	if( (boneno < 0) || (boneno >= m_bonenum) ){
		DbgOut( "mstate : SetMSElem : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	( m_mselem + boneno )->bonemat = *srcmat;

	return 0;
}

int CMState::SetMSLocalInf( int boneno, int srcanimno, int srcframeno, int srcnextanimno, int srcnextframeno )
{
	if( (boneno < 0) || (boneno >= m_bonenum) ){
		DbgOut( "mstate : SetMSLocalInf : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	MSELEM* setmse = ( m_mselem + boneno );
	setmse->localinf[MSLOCAL_ANIM] = srcanimno;
	setmse->localinf[MSLOCAL_FRAME] = srcframeno;
	setmse->localinf[MSLOCAL_NEXTANIM] = srcnextanimno;
	setmse->localinf[MSLOCAL_NEXTFRAME] = srcnextframeno;

	return 0;
}


int CMState::SetMSElem( CShdHandler* lpsh )
{
	int ret;

	int dsno;
	for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
		m_ds[dsno] = (lpsh->m_curds + dsno)->state;
	}


	CMQOMaterial* curmat = lpsh->m_mathead;
	while( curmat ){
		ret = m_tk.SetTexChangePt( curmat, curmat->curtexname );
		if( ret ){
			DbgOut( "ms : SetMSElem : tk SetTexChangePt error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
///////////
		ret = m_ak.SetAlpChange( curmat, curmat->col.a );
		if( ret ){
			DbgOut( "ms : SetMSElem : ak SetAlpChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		curmat = curmat->next;
	}

/////////

//DbgOut( "checkm!!! : ms SetMSElem : lpsh->mnum %d\r\n", lpsh->m_mnum );
	int mno;
	for( mno = 0; mno < lpsh->m_mnum; mno++ ){
		CShdElem* melem = *(lpsh->m_ppm + mno);
		CMorph* curmorph = melem->morph;
		if( curmorph ){
			CShdElem* baseelem = curmorph->m_baseelem;
			_ASSERT( baseelem );
			CMotionCtrl* boneelem = curmorph->m_boneelem;
			_ASSERT( boneelem );

//DbgOut( "checkm!!! : ms SetMSElem : targetnum %d\r\n", curmorph->m_targetnum );

			int tno;
			for( tno = 0; tno < curmorph->m_targetnum; tno++ ){
				TARGETPRIM* curtp = curmorph->m_ptarget + tno;
				CShdElem* targetelem = curtp->selem;
				_ASSERT( targetelem );

				float value = 0.0;
				CMMotElem* mmeptr = 0;
				mmeptr = boneelem->GetCurMMotElem( baseelem );
				if( mmeptr ){
					value = mmeptr->GetValue( targetelem );
				}else{
					value = 0.0f;
					_ASSERT( 0 );
				}
//DbgOut( "checkm!!! : ms SetMSElem : mno %d, tno %d, value %f\r\n", mno, tno, value );
				ret = m_mk.SetValue( baseelem, targetelem, value );
				if( ret ){
					DbgOut( "ms : SetMSElem : mk SetValue error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}else{
			_ASSERT( 0 );
		}
	}

	return 0;
}


int CMState::InitMatrix()
{
	int boneno;
	for( boneno = 0; boneno < m_bonenum; boneno++ ){
		MSELEM* curmse = m_mselem + boneno;
		curmse->bonemat.Identity();
		ZeroMemory( curmse->localinf, sizeof( int ) * MSLOCAL_MAX );
	}

	return 0;
}
