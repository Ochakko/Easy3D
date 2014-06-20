#include <stdafx.h> //ダミー
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <math.h>

#include <morph.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <d3ddisp.h>
#include <polymesh.h>
#include <polymesh2.h>

#include <motionctrl.h>
#include <MMotElem.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CMorph::CMorph()
{
	InitParams();
}

int CMorph::SetBaseParams( CShdHandler* lpsh, CMotHandler* lpmh, CShdElem* srcbaseelem, CMotionCtrl* srcboneelem )
{
	int ret;

	DestroyObjs();//!!!!!!!!!!!!!!!

	m_shandler = lpsh;
	m_mhandler = lpmh;
	m_baseelem = srcbaseelem;
	m_boneelem = srcboneelem;
	m_baseseri = srcbaseelem->serialno;
	m_boneseri = srcboneelem->serialno;

	if( !m_boneelem->IsJoint() ){
		_ASSERT( 0 );
		return 1;
	}

	if( m_baseelem->m_mtype != M_NONE ){
		DbgOut( "morph : SetBaseParams : mtype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_baseelem->type == SHDPOLYMESH ){
		CPolyMesh* pm;
		pm = m_baseelem->polymesh;
		if( pm ){
			m_objtype = SHDPOLYMESH;
			m_vertnum = pm->meshinfo->m;
			m_facenum = pm->meshinfo->n;
			m_baseelem->m_mtype = M_BASE;
		}else{
			DbgOut( "morph : SetBaseParams : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			m_objtype = 0;
			m_vertnum = 0;
			m_facenum = 0;
			m_baseelem->m_mtype = M_NONE;
			return 1;
		}
	}else if( m_baseelem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2;
		pm2 = m_baseelem->polymesh2;
		if( pm2 ){
			m_objtype = SHDPOLYMESH2;
			m_vertnum = pm2->meshinfo->m;
			m_facenum = pm2->meshinfo->n;
			m_baseelem->m_mtype = M_BASE;
		}else{
			DbgOut( "morph : SetBaseParams : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			m_objtype = 0;
			m_vertnum = 0;
			m_facenum = 0;
			m_baseelem->m_mtype = M_NONE;
			return 1;
		}
	}else{
		DbgOut( "morph : SetBaseParams : type error !!!\n" );
		_ASSERT( 0 );		
		m_objtype = 0;
		m_vertnum = 0;
		m_facenum = 0;
		m_baseelem->m_mtype = M_NONE;
		return 1;
	}

	ret = m_boneelem->AddCurMMotElem( m_baseelem );
	if( ret ){
		DbgOut( "morph : SetBaseParams : boneelem AddCurMMotElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CMorph::SetBaseParams( CShdHandler* lpsh, CMotHandler* lpmh, int srcbaseseri, int srcboneseri )
{
	DestroyObjs();//!!!!!!!!!!!!!!!

	m_shandler = lpsh;
	m_mhandler = lpmh;
	m_baseelem = 0;
	m_boneelem = 0;
	m_baseseri = srcbaseseri;
	m_boneseri = srcboneseri;

	m_objtype = 0;
	m_vertnum = 0;
	m_facenum = 0;

	return 0;
}


CMorph::~CMorph()
{
	DestroyObjs();
}

int CMorph::InitParams()
{
	m_baseseri = 0;
	m_baseelem = 0;
	m_boneseri = 0;
	m_boneelem = 0;
	m_objtype = 0;
	m_vertnum = 0;
	m_facenum = 0;

	m_targetnum = 0;
	m_ptarget = 0;
	
	m_shandler = 0;
	m_mhandler = 0;

	return 0;
}

void CMorph::DestroyObjs()
{
	if( m_ptarget ){
		free( m_ptarget );
		m_ptarget = 0;
	}
	m_targetnum = 0;
}

int CMorph::AddMorphTarget( CShdElem* addelem )
{
	if( addelem->m_mtype != M_NONE ){
		DbgOut( "morph : AddMorphTarget : mtype error skip !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!!
	}

	int okflag;
	okflag = CheckTargetOK( addelem );
	if( okflag == 0 ){
		::MessageBox( NULL, "ベースオブジェクトと構造が一致しないため\nターゲットに追加できません。", "失敗", MB_OK );
		DbgOut( "morph : AddMorphTarget : okflag zero error !!!\n" );
		return 0;
	}

	int sameindex = -1;
	sameindex = FindTarget( addelem );
	if( sameindex >= 0 ){
		DbgOut( "morph : AddMorphTarget : this elem is already exist skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	m_ptarget = (TARGETPRIM*)realloc( m_ptarget, sizeof( TARGETPRIM ) * ( m_targetnum + 1 ) );
	if( !m_ptarget ){
		DbgOut( "morph : AddMorphTarget : pptarget alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	( m_ptarget + m_targetnum )->selem = addelem;
	( m_ptarget + m_targetnum )->seri = addelem->serialno;
	addelem->m_mtype = M_TARGET;
	m_targetnum++;


	CMMotElem* curmme;
	curmme = m_boneelem->GetCurMMotElem( m_baseelem );
	if( !curmme ){
		DbgOut( "morph : AddMorphTarget : curmme NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curmme->AddPrim( addelem );
	if( ret ){
		DbgOut( "morph : AddMorphTarget : curmme AddPrim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMorph::AddMorphTarget( int addseri )
{
	int sameindex = -1;
	sameindex = FindTarget( addseri );
	if( sameindex >= 0 ){
		DbgOut( "morph : AddMorphTarget : this elem is already exist skip !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	m_ptarget = (TARGETPRIM*)realloc( m_ptarget, sizeof( TARGETPRIM ) * ( m_targetnum + 1 ) );
	if( !m_ptarget ){
		DbgOut( "morph : AddMorphTarget : pptarget alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	( m_ptarget + m_targetnum )->selem = 0;
	( m_ptarget + m_targetnum )->seri = addseri;
	m_targetnum++;

	return 0;
}


int CMorph::CheckTargetOK( CShdElem* chkelem )
{
	if( m_baseelem->clockwise != chkelem->clockwise ){
		return 0;
	}

	if( chkelem->type == SHDPOLYMESH ){
		if( m_objtype == SHDPOLYMESH ){
			CPolyMesh* pm;
			pm = chkelem->polymesh;
			if( pm ){
				if( (pm->meshinfo->m == m_vertnum) && (pm->meshinfo->n == m_facenum) ){
					return 1;
				}else{
					return 0;
				}
			}else{
				return 0;
			}
		}else{
			return 0;
		}

	}else if( chkelem->type == SHDPOLYMESH2 ){
		if( m_objtype == SHDPOLYMESH2 ){
			CPolyMesh2* pm2;
			pm2 = chkelem->polymesh2;
			if( pm2 ){
				if( (pm2->meshinfo->m == m_vertnum) && (pm2->meshinfo->n == m_facenum) ){
					return 1;
				}else{
					return 0;
				}
			}else{
				return 0;
			}
		}else{
			return 0;
		}

	}else{
		return 0;
	}
}
int CMorph::FindTarget( int findseri )
{
	int retindex = -1;

	int tno;
	for( tno = 0; tno < m_targetnum; tno++ ){
		int chkseri;
		chkseri = (m_ptarget + tno)->seri;
		if( chkseri == findseri ){
			retindex = tno;
			break;
		}
	}

	return retindex;
}


int CMorph::FindTarget( CShdElem* findelem )
{
	int retindex = -1;

	int tno;
	for( tno = 0; tno < m_targetnum; tno++ ){
		CShdElem* chkelem;
		chkelem = (m_ptarget + tno)->selem;
		if( chkelem == findelem ){
			retindex = tno;
			break;
		}
	}

	return retindex;
}

int CMorph::DeleteAllTarget()
{
	int tno;
	for( tno = 0; tno < m_targetnum; tno++ ){
		TARGETPRIM* curtp = m_ptarget + tno;
		curtp->selem->m_mtype = M_NONE;
	}

	if( m_ptarget ){
		free( m_ptarget );
		m_ptarget = 0;
	}
	m_targetnum = 0;

	return 0;
}

int CMorph::DeleteMorphTarget( CShdElem* delelem )
{
	int findindex = -1;
	findindex = FindTarget( delelem );
	if( findindex < 0 ){
		return 0;
	}

	TARGETPRIM* newptarget;
	newptarget = (TARGETPRIM*)malloc( sizeof( TARGETPRIM ) * ( m_targetnum - 1 ) );
	if( !newptarget ){
		DbgOut( "morph : DeleteMorphTarget : newptarget alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int tno;
	int setno = 0;
	for( tno = 0; tno < m_targetnum; tno++ ){
		if( tno != findindex ){
			*( newptarget + setno ) = *( m_ptarget + tno );
			setno++;
		}
	}
	
	if( setno != (m_targetnum - 1) ){
		DbgOut( "morph : DeleteMorphTarget : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delelem->m_mtype = M_NONE;//!!!!!!!!!!

	free( m_ptarget );
	m_ptarget = newptarget;
	m_targetnum--;


	CMMotElem* curmme;
	curmme = m_boneelem->GetCurMMotElem( m_baseelem );
	if( !curmme ){
		DbgOut( "morph : DeleteMorphTarget : curmme NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curmme->DeletePrim( delelem );
	if( ret ){
		DbgOut( "morph : DeleteMorphTarget : curmmme DeletePrim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMorph::ConvSeriToElem()
{
	//M_BASE, M_TARGETセット、AddPrimも。

	int ret;

	if( (m_baseseri <= 0) || (m_baseseri >= m_shandler->s2shd_leng) ){
		DbgOut( "morph : ConvSeriToElem : baseseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (m_boneseri <= 0) || (m_boneseri >= m_shandler->s2shd_leng) ){
		DbgOut( "morph : ConvSeriToElem : boneseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_baseelem = (*m_shandler)( m_baseseri );
	_ASSERT( m_baseelem );
	m_baseelem->dispflag = 0;
	if( m_baseelem->type == SHDPOLYMESH ){
		CPolyMesh* pm;
		pm = m_baseelem->polymesh;
		if( pm ){
			m_objtype = SHDPOLYMESH;
			m_vertnum = pm->meshinfo->m;
			m_facenum = pm->meshinfo->n;
			m_baseelem->m_mtype = M_BASE;
		}else{
			DbgOut( "morph : ConvSeriToElem : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			m_objtype = 0;
			m_vertnum = 0;
			m_facenum = 0;
			m_baseelem->m_mtype = M_NONE;
			return 1;
		}
	}else if( m_baseelem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2;
		pm2 = m_baseelem->polymesh2;
		if( pm2 ){
			m_objtype = SHDPOLYMESH2;
			m_vertnum = pm2->meshinfo->m;
			m_facenum = pm2->meshinfo->n;
			m_baseelem->m_mtype = M_BASE;
		}else{
			DbgOut( "morph : ConvSeriToElem : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			m_objtype = 0;
			m_vertnum = 0;
			m_facenum = 0;
			m_baseelem->m_mtype = M_NONE;
			return 1;
		}
	}else{
		DbgOut( "morph : ConvSeriToElem : type error !!!\n" );
		_ASSERT( 0 );		
		m_objtype = 0;
		m_vertnum = 0;
		m_facenum = 0;
		m_baseelem->m_mtype = M_NONE;
		return 1;
	}


	m_boneelem = (*m_mhandler)( m_boneseri );
	_ASSERT( m_boneelem );
	if( !m_boneelem->IsJoint() ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_boneelem->AddCurMMotElem( m_baseelem );
	if( ret ){
		DbgOut( "morph : ConvSeriToElem : boneelem AddCurMMotElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMMotElem* curmme;
	curmme = m_boneelem->GetCurMMotElem( m_baseelem );
	if( !curmme ){
		DbgOut( "morph : ConvSeriToElem : curmme NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int tno;
	for( tno = 0; tno < m_targetnum; tno++ ){
		TARGETPRIM* curtp = m_ptarget + tno;

		if( (curtp->seri > 0) && (curtp->seri < m_shandler->s2shd_leng) ){
			CShdElem* chkelem;
			chkelem = (*m_shandler)( curtp->seri );
			int okflag;
			okflag = CheckTargetOK( chkelem );
			if( okflag == 0 ){
				DbgOut( "morph : ConvSeriToElem : target okflag 0 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			curtp->selem = chkelem;
			chkelem->m_mtype = M_TARGET;
			chkelem->dispflag = 0;

			ret = curmme->AddPrim( chkelem );
			if( ret ){
				DbgOut( "morph : ConvSeriToElem : curmorph AddPrim error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			DbgOut( "morph : ConvSeriToElem : target seri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
