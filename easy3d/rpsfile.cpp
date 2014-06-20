#include "stdafx.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>


#include <algorithm>	//sort 用


//#include <coef.h>
#include "rpsfile.h"
#include "TraQ.h"

#include "handlerset.h"


#include "FilterType.h"
#include <quaternion.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <shdelem.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <motionpoint2.h>

#include <BoneProp.h>

static int s_sksym[SKEL_MAX] = {
	SKEL_TOPOFJOINT,
	SKEL_TORSO,

	SKEL_RIGHT_HIP,
	SKEL_RIGHT_KNEE,
	SKEL_RIGHT_FOOT,
	
	SKEL_LEFT_HIP,
	SKEL_LEFT_KNEE,
	SKEL_LEFT_FOOT,
	
	SKEL_NECK,
	SKEL_HEAD,
	
	SKEL_RIGHT_SHOULDER,
	SKEL_RIGHT_ELBOW,
	SKEL_RIGHT_HAND,
	
	SKEL_LEFT_SHOULDER,
	SKEL_LEFT_ELBOW,
	SKEL_LEFT_HAND
};


CRpsFile::CRpsFile( CHandlerSet* hsptr )
{
	InitParams();
	m_hs = hsptr;
}
CRpsFile::~CRpsFile()
{
	DestroyObjs();
}

int CRpsFile::InitParams()
{
	//file操作用
	m_hfile = INVALID_HANDLE_VALUE;
	m_buf = 0;
	m_pos = 0;
	m_bufleng = 0;

	ZeroMemory( m_line, sizeof( char ) * RPSLINELEN );
	m_pelem = 0;
	m_traq = 0;

	ZeroMemory( m_skipflag, sizeof( int ) * SKEL_MAX );

	ZeroMemory( m_armp, sizeof( CMotionPoint2* ) * SKEL_MAX );

	return 0;
}
int CRpsFile::DestroyObjs()
{

	if( m_hfile != INVALID_HANDLE_VALUE ){
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}

	if( m_buf ){
		free( m_buf );
		m_buf = 0;
	}

	m_pos = 0;
	m_bufleng = 0;

	if( m_pelem ){
		free( m_pelem );
		m_pelem = 0;
	}
	if( m_traq ){
		delete [] m_traq;
		m_traq = 0;
	}

	return 0;
}


int CRpsFile::CreateParams()
{
	int ret1;
	DestroyObjs();
	InitParams();

	ret1 = CreateRpsElem();
	if( ret1 ){
		_ASSERT( 0 );
		return ret1;//2の場合もあり！！！
	}

	int frameno;
	int skelno;
	for( frameno = 0; frameno < 3; frameno++ ){
		for( skelno = SKEL_TORSO; skelno < SKEL_MAX; skelno++ ){
			RPSELEM* curelem = m_pelem + frameno * SKEL_MAX + skelno;
			curelem->confidence = 0.0f;
			curelem->pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			curelem->twistflag = 1;
			curelem->framecnt = frameno;
		}
	}


	return 0;
}


int CRpsFile::CreateRpsElem()
{
	m_pelem = (RPSELEM*)malloc( sizeof( RPSELEM ) * SKEL_MAX * 3 );
	if( !m_pelem ){
		_ASSERT( 0 );
		return 1;
	}

	m_traq = new CTraQ[ SKEL_MAX * 3 ];
	if( !m_traq ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CRpsFile::CalcTraQ( TSELEM* tsptr, int chksym )
{
	if( !m_pelem || !m_traq ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = SetSkipFlag();
	_ASSERT( !ret );


	int frameno = 2;

	ret = m_traq->CalcTorso( m_traq, m_pelem, frameno, m_skipflag[SKEL_TORSO] );
	_ASSERT( !ret );

	ret = m_traq->CalcNeck( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, m_skipflag[SKEL_NECK] );
	_ASSERT( !ret );


	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_TORSO, SKEL_LEFT_HIP, 3 );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_LEFT_HIP, SKEL_LEFT_KNEE, m_skipflag[SKEL_LEFT_KNEE] );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_LEFT_KNEE, SKEL_LEFT_FOOT, m_skipflag[SKEL_LEFT_FOOT] );
	_ASSERT( !ret );


	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_TORSO, SKEL_RIGHT_HIP, 3 );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_RIGHT_HIP, SKEL_RIGHT_KNEE, m_skipflag[SKEL_RIGHT_KNEE] );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_RIGHT_KNEE, SKEL_RIGHT_FOOT, m_skipflag[SKEL_RIGHT_FOOT] );
	_ASSERT( !ret );


	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_NECK, SKEL_HEAD, m_skipflag[SKEL_HEAD] );
	_ASSERT( !ret );


	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_NECK, SKEL_LEFT_SHOULDER, 3 );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_LEFT_SHOULDER, SKEL_LEFT_ELBOW, m_skipflag[SKEL_LEFT_ELBOW] );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_LEFT_ELBOW, SKEL_LEFT_HAND, m_skipflag[SKEL_LEFT_HAND] );
	_ASSERT( !ret );


	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_NECK, SKEL_RIGHT_SHOULDER, 3 );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_RIGHT_SHOULDER, SKEL_RIGHT_ELBOW, m_skipflag[SKEL_RIGHT_ELBOW] );
	_ASSERT( !ret );
	ret = m_traq->CalcQ( m_hs->m_shandler, tsptr, m_traq, m_pelem, frameno, SKEL_RIGHT_ELBOW, SKEL_RIGHT_HAND, m_skipflag[SKEL_RIGHT_HAND] );
	_ASSERT( !ret );


	//sym
	if( chksym == 1 ){
		ret = SetSym( SKEL_TOPOFJOINT, SKEL_TOPOFJOINT, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );
		ret = SetSym( SKEL_TORSO, SKEL_TORSO, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );

		ret = SetSym( SKEL_LEFT_HIP, SKEL_RIGHT_HIP, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );
		ret = SetSym( SKEL_LEFT_KNEE, SKEL_RIGHT_KNEE, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );
		ret = SetSym( SKEL_LEFT_FOOT, SKEL_RIGHT_FOOT, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );

		ret = SetSym( SKEL_NECK, SKEL_NECK, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );
		ret = SetSym( SKEL_HEAD, SKEL_HEAD, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );

		ret = SetSym( SKEL_LEFT_SHOULDER, SKEL_RIGHT_SHOULDER, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );
		ret = SetSym( SKEL_LEFT_ELBOW, SKEL_RIGHT_ELBOW, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );
		ret = SetSym( SKEL_LEFT_HAND, SKEL_RIGHT_HAND, tsptr, m_hs->m_shandler );
		_ASSERT( !ret );
	}

	//m_q --> m_finalqへコピー
	for( int frameno = 0; frameno < 3; frameno++ ){
		for( int skelno = 0; skelno < SKEL_MAX; skelno++ ){
			(m_traq + frameno * SKEL_MAX + skelno)->m_finalq = (m_traq + frameno * SKEL_MAX + skelno)->m_q;
			(m_traq + frameno * SKEL_MAX + skelno)->m_finaltra = (m_traq + frameno * SKEL_MAX + skelno)->m_tra;
			(m_traq + frameno * SKEL_MAX + skelno)->m_finaleul = (m_traq + frameno * SKEL_MAX + skelno)->m_cureul;

			(m_traq + frameno * SKEL_MAX + skelno)->m_orgq = (m_traq + frameno * SKEL_MAX + skelno)->m_q;
			(m_traq + frameno * SKEL_MAX + skelno)->m_orgtra = (m_traq + frameno * SKEL_MAX + skelno)->m_tra;
			(m_traq + frameno * SKEL_MAX + skelno)->m_orgeul = (m_traq + frameno * SKEL_MAX + skelno)->m_cureul;
		}
	}

	return 0;
}

int CRpsFile::InitArMp( TSELEM* tsptr, int motid, int frameno )
{
	int skelno, boneno;
	for( skelno = 0; skelno < SKEL_MAX; skelno++ ){
		boneno = (tsptr + skelno)->jointno;

		CMotionCtrl* mcptr = (*m_hs->m_mhandler)( boneno );
		_ASSERT( mcptr );

		CMotionPoint2* mpptr;
		mpptr = mcptr->GetMotionPoint( motid );
		_ASSERT( mpptr );
		if( !mpptr ){
			_ASSERT( 0 );
			return 1;
		}

		while( mpptr && (mpptr->m_frameno != frameno) ){
			mpptr = mpptr->next;
		}

		m_armp[ skelno ] = mpptr;
	}

	return 0;
}


int CRpsFile::SetMotion( int posmode, TSELEM* tsptr, int cookie, int setframe )
{

	int ret;
	int frameno = 0;
	if( cookie < 0 ){
		return 0;
	}

	int skelno;
	for( skelno = 0; skelno < SKEL_MAX; skelno++ ){
		CTraQ* curtraq = (m_traq + 2 * SKEL_MAX + skelno);
		int boneno;
		boneno = (tsptr + skelno)->jointno;

		if( curtraq->m_outputflag == 1 ){
			CQuaternion curq;
			curq = curtraq->m_finalq;

			CMotionPoint2* mpptr;
			mpptr = m_armp[ skelno ];
			while( mpptr && (mpptr->m_frameno != setframe) ){
				mpptr = mpptr->next;
			}
			m_armp[ skelno ] = mpptr;

			if( mpptr ){
				if( skelno == SKEL_TOPOFJOINT ){
					D3DXVECTOR3 settra;
					settra = curtraq->m_finaltra;
					if( posmode == POSMODE_TOPOFJOINT ){
						mpptr->m_mvx = settra.x;
						mpptr->m_mvy = settra.y;
						mpptr->m_mvz = settra.z;
	
						mpptr->m_q = curtraq->m_finalq;
					}else if( posmode == POSMODE_ZERO ){
						mpptr->m_mvx = 0.0f;
						mpptr->m_mvy = 0.0f;
						mpptr->m_mvz = 0.0f;

						mpptr->m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
					}else if( posmode == POSMODE_GLOBAL ){
						mpptr->m_mvx = 0.0f;
						mpptr->m_mvy = 0.0f;
						mpptr->m_mvz = 0.0f;

						mpptr->m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

						m_hs->m_gpd.m_gpe.pos = settra;
						m_hs->m_gpd.m_gpe.e3dpos = m_hs->m_gpd.m_gpe.pos;
						m_hs->m_gpd.CalcMatWorld();

						D3DXVECTOR3 neweul;
						ret = qToEuler( 0, &(curtraq->m_finalq), &neweul );
						_ASSERT( !ret );
						ret = modifyEuler( &neweul, &m_hs->m_gpd.m_gpe.e3drot );
						_ASSERT( !ret );
						m_hs->m_gpd.m_gpe.e3drot = neweul;
						m_hs->m_twist = 0.0f;
						m_hs->m_gpd.CalcMatWorld();

					}else{
						_ASSERT( 0 );
					}
				}else{
					mpptr->m_q = curtraq->m_finalq;
				}
			}

			CShdElem* selem = (*m_hs->m_shandler)( boneno );
			ret = m_hs->m_mhandler->MakeRawmatCap( cookie, selem, setframe, m_hs->m_shandler, mpptr );
			_ASSERT( !ret );
		}

	}

//	ret = m_papp->m_mhandler->FillUpMotion( m_papp->m_shandler, cookie, (tsptr + SKEL_TOPOFJOINT)->jointno, setframe, setframe, 1 );
//	if( ret ){
//		_ASSERT( 0 );
//		return 1;
//	}

	int errorflag = 0;
	m_hs->m_mhandler->RawmatToMotionmatReqCap( cookie, m_hs->m_shandler, 1, &errorflag, setframe );
	_ASSERT( errorflag == 0 );


	ret = m_hs->m_mhandler->SetMotionFrameNo( m_hs->m_shandler, cookie, setframe, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	m_hs->m_mhandler->SetNewPose( m_hs->m_shandler );


//////////

	return 0;
}

int CRpsFile::SetSkipFlag()
{
	//3-->全skip
	//1-->部分skip

	D3DXVECTOR3 zerop( 0.0f, 0.0f, 0.0f );

	ZeroMemory( m_skipflag, sizeof( int ) * SKEL_MAX );


	int skno;
	if( (m_pelem + 0 * SKEL_MAX + SKEL_TORSO)->confidence != 1.0f){
		//|| ((m_pelem + 0 * SKEL_MAX + SKEL_TORSO)->pos == zerop) ){

		for( skno = 0; skno < SKEL_MAX; skno++ ){
			m_skipflag[skno] = 3;
		}
		return 0;
	}

	if( ((m_pelem + 0 * SKEL_MAX + SKEL_NECK)->confidence != 1.0f) || 
		((m_pelem + 0 * SKEL_MAX + SKEL_NECK)->pos == zerop) ){
		m_skipflag[SKEL_NECK] = 3;
		m_skipflag[SKEL_HEAD] = 3;
		m_skipflag[SKEL_LEFT_SHOULDER] = 3;
		m_skipflag[SKEL_LEFT_ELBOW] = 3;
		m_skipflag[SKEL_LEFT_HAND] = 3;
		m_skipflag[SKEL_RIGHT_SHOULDER] = 3;
		m_skipflag[SKEL_RIGHT_ELBOW] = 3;
		m_skipflag[SKEL_RIGHT_HAND] = 3;
	}

	if( ((m_pelem + 0 * SKEL_MAX + SKEL_HEAD)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_HEAD)->pos == zerop) ){
		m_skipflag[SKEL_HEAD] = 3;
	}

	if( ((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_HIP)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_HIP)->pos == zerop) ){
		m_skipflag[SKEL_TORSO] |= 1;//!!!!!!!
		m_skipflag[SKEL_LEFT_HIP] = 3;
		m_skipflag[SKEL_LEFT_KNEE] = 3;
		m_skipflag[SKEL_LEFT_FOOT] = 3;
	}
	if( ((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_HIP)->confidence != 1.0f) || 
		((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_HIP)->pos == zerop) ){
		m_skipflag[SKEL_TORSO] |= 1;//!!!!!!!
		m_skipflag[SKEL_RIGHT_HIP] = 3;
		m_skipflag[SKEL_RIGHT_KNEE] = 3;
		m_skipflag[SKEL_RIGHT_FOOT] = 3;
	}

	if( ((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_KNEE)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_KNEE)->pos == zerop) ){
		m_skipflag[SKEL_LEFT_KNEE] = 3;
		m_skipflag[SKEL_LEFT_FOOT] = 3;
	}
	if( ((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_KNEE)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_KNEE)->pos == zerop) ){
		m_skipflag[SKEL_RIGHT_KNEE] = 3;
		m_skipflag[SKEL_RIGHT_FOOT] = 3;
	}

	if( ((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_FOOT)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_FOOT)->pos == zerop) ){
		m_skipflag[SKEL_LEFT_FOOT] = 3;
	}
	if( ((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_FOOT)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_FOOT)->pos == zerop) ){
		m_skipflag[SKEL_RIGHT_FOOT] = 3;
	}


	if( ((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_SHOULDER)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_SHOULDER)->pos == zerop) ){
		//m_skipflag[SKEL_NECK] |= 1;//!!!!!!!!!!!!!!
		m_skipflag[SKEL_NECK] = 3;
		m_skipflag[SKEL_HEAD] = 3;
		m_skipflag[SKEL_LEFT_SHOULDER] = 3;
		m_skipflag[SKEL_LEFT_ELBOW] = 3;
		m_skipflag[SKEL_LEFT_HAND] = 3;
	}
	if( ((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_SHOULDER)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_SHOULDER)->pos == zerop) ){
		//m_skipflag[SKEL_NECK] |= 1;//!!!!!!!!!!!!!!
		m_skipflag[SKEL_NECK] = 3;
		m_skipflag[SKEL_HEAD] = 3;
		m_skipflag[SKEL_RIGHT_SHOULDER] = 3;
		m_skipflag[SKEL_RIGHT_ELBOW] = 3;
		m_skipflag[SKEL_RIGHT_HAND] = 3;
	}

	if( ((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_ELBOW)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_ELBOW)->pos == zerop) ){
		m_skipflag[SKEL_LEFT_ELBOW] = 3;
		m_skipflag[SKEL_LEFT_HAND] = 3;
	}
	if( ((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_ELBOW)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_ELBOW)->pos == zerop) ){
		m_skipflag[SKEL_RIGHT_ELBOW] = 3;
		m_skipflag[SKEL_RIGHT_HAND] = 3;
	}

	if( ((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_HAND)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_LEFT_HAND)->pos == zerop) ){
		m_skipflag[SKEL_LEFT_HAND] = 3;
	}
	if( ((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_HAND)->confidence != 1.0f) ||
		((m_pelem + 0 * SKEL_MAX + SKEL_RIGHT_HAND)->pos == zerop) ){
		m_skipflag[SKEL_RIGHT_HAND] = 3;
	}


	return 0;
}

/***
int CRpsFile::ResetOutputFlag( D3DXVECTOR3* keyeul, D3DXVECTOR3* keymv )
{
	int frameno, skelno;
	for( frameno = 0; frameno < m_framenum; frameno++ ){
		for( skelno = 0; skelno < SKEL_MAX; skelno++ ){
			CTraQ* curtraq = (m_traq + frameno * SKEL_MAX + skelno );
			if( frameno == 0 ){
				curtraq->m_outputflag = 1;
				*(keyeul + skelno) = curtraq->m_finaleul;
				if( skelno == 0 ){
					*keymv = curtraq->m_finaltra;
				}
			}else{
				curtraq->m_outputflag = 0;
			}
		}
	}

	return 0;
}


//ガウシアンフィルタ用
int CRpsFile::combi( int N, int rp ) {
	int n, r;

	//２次元配列の確保
	int **combi = new int*[N+1];
	combi[0] = new int[(N+1) * (N+1)];
	for(int i = 1; i < N+1; i++){
		combi[i] = combi[0] + i * (N+1);
	}

	//結果を残す方法
	//・nCr = n-1Cr-1 + n-1Cr 
	//・nC0 = 1, nCn = 1 　　　を用いて計算する
	for(n = 0; n <= N; n ++){
		combi[n][0] = combi[n][n] = 1;
		for(r = 1; r < n; r ++){
			combi[n][r] = combi[n - 1][r - 1] + combi[n - 1][r];
		}
	}

	int ret = combi[N][rp];

	delete[] combi[0];
	delete[] combi;

	return ret;
}
***/

int CRpsFile::SetSym( int skelno, int sksym, TSELEM* tsptr, CShdHandler* lpsh )
{
	int ret;


	D3DXVECTOR3 befeul1( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 befeul2( 0.0f, 0.0f, 0.0f );

	int frameno = 2;

	CTraQ* traq1 = ( m_traq + frameno * SKEL_MAX + skelno );
	CTraQ* traq2 = ( m_traq + frameno * SKEL_MAX + sksym );

	CQuaternion newq1;
	D3DXVECTOR3 newtra1, neweul1;
	CQuaternion newq2;
	D3DXVECTOR3 newtra2, neweul2;


	ret = CalcSymPose( tsptr + skelno, lpsh, traq2, &newq2, &newtra2, &neweul2, befeul2 );
	_ASSERT( !ret );
	if( skelno != sksym ){
		ret = CalcSymPose( tsptr + sksym, lpsh, traq1, &newq1, &newtra1, &neweul1, befeul1 );
		_ASSERT( !ret );
	}

		
	traq1->m_q = newq2;
	traq1->m_tra = newtra2;
	traq1->m_cureul = neweul2;

	if( skelno != sksym ){
		traq2->m_q = newq1;
		traq2->m_tra = newtra1;
		traq2->m_cureul = neweul1;
	}



	return 0;
}

int CRpsFile::CalcSymPose( TSELEM* tsptr, CShdHandler* lpsh, CTraQ* traq, 
	CQuaternion* newqptr, D3DXVECTOR3* newtraptr, D3DXVECTOR3* neweulptr, D3DXVECTOR3 befeul )
{
	int ret;
	CQuaternion tmpq =  traq->m_q;
	D3DXVECTOR3 tmptra = traq->m_tra;
	D3DXVECTOR3 tmpeul = traq->m_cureul;

	D3DXQUATERNION symmxq;
	symmxq.w = tmpq.w;
	symmxq.x = tmpq.x;
	symmxq.y = tmpq.y;
	symmxq.z = tmpq.z;

	D3DXVECTOR3 symmaxis;
	float symmangle;

	D3DXQuaternionToAxisAngle( &symmxq, &symmaxis, &symmangle );		

	symmangle *= -1.0f;//!!!

	symmaxis.x *= -1.0f;
	tmptra.x *= -1.0f;

		
	D3DXQUATERNION newxq;
	D3DXQuaternionRotationAxis( &newxq, &symmaxis, symmangle );
	CQuaternion srcq;
	srcq.SetParams( newxq.w, newxq.x, newxq.y, newxq.z );
	CQuaternion axisq;
	D3DXVECTOR3 cureul;
	ret = traq->QtoEul( lpsh, srcq, befeul, 1, tsptr->jointno, &cureul, &axisq );
	_ASSERT( !ret );


	newqptr->SetParams( newxq.w, newxq.x, newxq.y, newxq.z );
	*newtraptr = tmptra;
	*neweulptr = cureul;

	return 0;
}

int CRpsFile::CreateMotionPoints( int cookie )
{
	if( m_hs->m_mhandler->s2mot_leng < 1 ){
		return 0;//!!!!!!!!
	}


	int elemno;//, framenum;
	CMotionCtrl* mcptr;
	CMotionPoint2* curmp;
	CShdElem* selem;

	CQuaternion initq;
	initq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	int interp;
	m_hs->m_mhandler->GetDefInterp( cookie, &interp );

	int frameno = 0;

	for( elemno = 1; elemno < m_hs->m_mhandler->s2mot_leng; elemno++ ){
		mcptr = (*m_hs->m_mhandler)( elemno );
		if( !mcptr )
			break;

		selem = (*m_hs->m_shandler)( elemno );
		_ASSERT( selem );

		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH)){
			curmp = mcptr->IsExistMotionPoint( cookie, frameno );
			if( !curmp ){

				CMotionPoint2 tmpmp;
				tmpmp.InitParams();

				CMotionPoint2* newmp;
				newmp = mcptr->AddMotionPoint( cookie, frameno, tmpmp.m_q, tmpmp.m_mvx, tmpmp.m_mvy, tmpmp.m_mvz,
					0, interp, tmpmp.m_scalex, tmpmp.m_scaley, tmpmp.m_scalez, 0 );
				if( !newmp ){
					_ASSERT( 0 );
					return 1;
				}
			}		
		}
	}
		
	return 0;
}

int CRpsFile::SetRpsElem( int frameno, D3DXVECTOR3* srcpos )
{
	if( frameno == 0 ){
		int fno;
		//D3DXVECTOR3 firstpos = *( srcpos + SKEL_TORSO );
		for( fno = 0; fno < 3; fno++ ){
			int skno;
			for( skno = 0; skno < SKEL_MAX; skno++ ){
				RPSELEM* curelem;
				curelem = m_pelem + fno * SKEL_MAX + skno;

				curelem->pos = *( srcpos + skno );
				curelem->skelno = skno;
				curelem->framecnt = fno;
				curelem->confidence = 1.0f;
				curelem->twistflag = 1;

				CTraQ* curtq = m_traq + fno * SKEL_MAX + skno;
				curtq->InitParams();
			}
		}
	}else{
		int skno;

		if( frameno == 2 ){
			MoveMemory( m_pelem + 1 * SKEL_MAX, m_pelem + 2 * SKEL_MAX, sizeof( RPSELEM ) * SKEL_MAX );
			MoveMemory( m_traq + 1 * SKEL_MAX, m_traq + 2 * SKEL_MAX, sizeof( CTraQ ) * SKEL_MAX );
			for( skno = 0; skno < SKEL_MAX; skno++ ){
				( m_traq + 2 * SKEL_MAX + skno )->InitParams();
			}
		}

		for( skno = 0; skno < SKEL_MAX; skno++ ){
			RPSELEM* curelem;
			curelem = m_pelem + frameno * SKEL_MAX + skno;

			curelem->pos = *( srcpos + skno );// - (m_pelem + 0 * SKEL_MAX + SKEL_TORSO)->pos;
			curelem->skelno = skno;
			curelem->framecnt = frameno;
			curelem->confidence = 1.0f;
			curelem->twistflag = 1;
		}

	}

	return 0;
}


