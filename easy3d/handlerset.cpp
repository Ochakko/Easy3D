#include "stdafx.h"
#include <windows.h>
#include <math.h>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <D3DX9.h>

#include <mmsystem.h>

#include <coef.h>
#define DBGH
#include "dbg.h"

#include <treehandler2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>

#include <sigfile.h>
#include <quafile.h>
//#include <motfile.h>
#include <quaternion.h>
#include <quaternion2.h>
#include <bsphere.h>
#include <groundbmp.h>
#include <billboardIO.h>
#include <extlineIO.h>


#include <mqofile.h>

#include "handlerset.h"

#include <crtdbg.h>

#include <billboard.h>
#include <billboardelem.h>

#include <naviline.h>
#include <navipoint.h>

//#include <bimfile.h>
#include <im2file.h>

#include <bbox.h>
#include <texbank.h>

#include <gndfile.h>

#include <infelem.h>


#include <MCHandler.h>
#include <MAFile.h>

#include <DSKey.h>
#include <DSChange.h>
#include <DispSwitch.h>

#include <TexKey.h>
#include <TexChange.h>

#include <AlpKey.h>
#include <AlpChange.h>
#include <mqomaterial.h>

#include <MMotKey.h>
#include <MMotElem.h>

#include <GPAnimHandler.h>
#include <GPAnim.h>
#include <GPKey.h>
#include <GPFile.h>

#include <MOEAnimHandler.h>
#include <MOEAnim.h>
#include <MOEKey.h>
#include <MOEFile.h>

#include <InfScope.h>
#include <d3ddisp.h>

#include <AutoSpline.h>
#include <Panda.h>

#include "resource.h"

/////////////////
// global
CHandlerSet* g_bbhs = 0;

/////////////////
// extern
extern int g_useGPU;
extern int g_usePS;
extern float g_proj_near;
extern float g_proj_far;
extern float g_proj_fov;


extern UINT g_miplevels;
extern DWORD g_mipfilter;

extern CTexBank* g_texbnk;// shdhandler.cpp
extern D3DXMATRIX g_matProj;//main.cpp
extern POINT	g_scsize;//main.cpp


extern CHandlerSet* GetHandlerSet( int hsid );

extern LPDIRECT3DTEXTURE9 g_curtex0;
extern LPDIRECT3DTEXTURE9 g_curtex1;


////////////////
// static 
int CHandlerSet::s_alloccnt = 1;

////////////////

CHandlerSet::CHandlerSet()
{
	serialno = s_alloccnt;
	s_alloccnt++;

	m_thandler = 0;
	m_shandler = 0;
	m_mhandler = 0;

    //D3DXMatrixIdentity( &m_gpd.m_matWorld );
	//D3DXMatrixIdentity( &m_matView );
	//D3DXMatrixIdentity( &matProj );
	
	m_befpos.x = 0.0f;
	m_befpos.y = 0.0f;
	m_befpos.z = 0.0f;

	m_gpd.InitParams();
	m_twist = 0.0f;

	m_clientWidth = 0;
	m_clientHeight = 0;

	m_bbio = 0;

	m_TLmode = TLMODE_ORG;

	m_groundflag = 0;
	m_billboardflag = 0;
	m_extlineflag = 0;
	D3DXMatrixIdentity( &m_billmatW );
	D3DXMatrixIdentity( &m_billmatW2 );

	m_lineio = 0;


//	ZeroMemory( m_resdir, sizeof( char ) * MAX_PATH );

	InitializeCriticalSection( &m_crit_addmotion );

	m_mch = 0;

	m_mllist = 0;
	m_mlleng = 0;

	m_dsfillupmode = 0;
	m_texfillupmode = 0;

	m_syminfnum = 0;
	ZeroMemory( &m_symIE, sizeof( INFELEM ) * INFNUMMAX );

	m_gpah = 0;
	m_moeah = 0;
}


CHandlerSet::~CHandlerSet()
{
	DestroyHandler();
	DeleteCriticalSection( &m_crit_addmotion );
}

int CHandlerSet::CreateHandler()
{
	m_thandler = new CTreeHandler2( TMODE_IMPORT | TMODE_MULT );
	if( !m_thandler ){
		DbgOut( "CHandlerSet : CreateHandler : m_thandler alloc error !!!\n" );
		return 1;
	}
	m_shandler = new CShdHandler( m_thandler );
	if( !m_shandler ){
		DbgOut( "CHandlerSet : CreateShdHandler : shandler alloc error !!!\n" );
		return 1;
	}

	m_shandler->m_inRDBflag = 0;//!!!!

	m_mhandler = new CMotHandler( m_thandler );
	if( !m_mhandler ){
		DbgOut( "CHandlerSet : CreateHandler : mhandler alloc error !!!\n" );
		return 1;
	}

	if( m_gpah ){
		delete m_gpah;
		m_gpah = 0;
	}
	m_gpah = new CGPAnimHandler();
	if( !m_gpah ){
		DbgOut( "hs : CreateHandler : gpah alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_moeah ){
		delete m_moeah;
		m_moeah = 0;
	}
	m_moeah = new CMOEAnimHandler();
	if( !m_moeah ){
		DbgOut( "hs : CreateHandler : moeah alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::DestroyHandler()
{

	if( m_thandler ){
		delete m_thandler;
		m_thandler = 0;
	}

	if( m_shandler ){
		delete m_shandler;
		m_shandler = 0;
	}

	if( m_mhandler ){
		delete m_mhandler;
		m_mhandler = 0;
	}

	if( m_bbio ){
		delete m_bbio;
		m_bbio = 0;
	}

	if( m_lineio ){
		delete m_lineio;
		m_lineio = 0;
	}

	if( m_mch ){
		delete m_mch;
		m_mch = 0;
	}

	if( m_mllist ){
		free( m_mllist );
		m_mllist = 0;
	}
	m_mlleng = 0;

	if( m_gpah ){
		delete m_gpah;
		m_gpah = 0;
	}

	if( m_moeah ){
		delete m_moeah;
		m_moeah = 0;
	}

	return 0;
}


int CHandlerSet::SerialnoCmp( CHandlerSet* cmphs )
{
	int cmpno;
	if( !cmphs ){
		_ASSERT( 0 );
		return 1;
	}

	cmpno = cmphs->serialno;

	return SerialnoCmp( cmpno );
}

int CHandlerSet::SerialnoCmp( int cmpno )
{
	if( serialno > cmpno ){
		return 1;
	}else if( serialno < cmpno ){
		return -1;
	}else{
		return 0;
	}
}

/***
int CHandlerSet::AddToPrev( CHandlerSet* addhs )
{
	CHandlerSet *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addhs->prev = prev;
	addhs->next = this;

	prev = addhs;

	if( saveprev ){
		saveprev->next = addhs;
	}else{
		_ASSERT( ishead );
		addhs->ishead = 1;
		ishead = 0;
	}

	return 0;
}

int CHandlerSet::AddToNext( CHandlerSet* addhs )
{
	CHandlerSet *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	addhs->prev = this;
	addhs->next = savenext;

	next = addhs;

	if( savenext ){
		savenext->prev = addhs;
	}

	addhs->ishead = 0;

	return 0;
}

int CHandlerSet::LeaveFromChain()
{
	CHandlerSet *saveprev, *savenext;
	saveprev = prev;
	savenext = next;

	prev = 0;
	next = 0;

	if( saveprev ){
		saveprev->next = savenext;
	}else{
		_ASSERT( ishead );
		if( savenext )
			savenext->ishead = 1;
		ishead = 0;
	}

	if( savenext ){
		savenext->prev = saveprev;
	}

	return 0;
}
***/

int CHandlerSet::LoadSigFileFromPnd( CPanda* panda, int pndid, int propno, float srcmult )
{
	int ret = 0;
		
	CSigFile* sigfile;
	sigfile = new CSigFile();
	if( !sigfile ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );
	ret = sigfile->LoadSigFileFromPnd( panda, pndid, propno, m_thandler, m_shandler, m_mhandler, 0, srcmult, 0, offsetpos, rot, m_groundflag );
	if( ret ){
		DbgOut( "handlerset : LoadSigFile : sigfile LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		delete sigfile;
		return 1;
	}
	delete sigfile;
	
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : LoadSigFile : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	return 0;
}


int CHandlerSet::LoadSigFile( char* fname, int adjustuvflag, float srcmult )
{
	int ret = 0;
		
	CSigFile* sigfile;
	sigfile = new CSigFile();
	if( !sigfile ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 multvec( srcmult, srcmult, srcmult );
	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );
	ret = sigfile->LoadSigFile( fname, m_thandler, m_shandler, m_mhandler, 0, multvec, adjustuvflag, offsetpos, rot, m_groundflag );
	if( ret ){
		DbgOut( "handlerset : LoadSigFile : sigfile LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		delete sigfile;
		return 1;
	}
	delete sigfile;
	
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : LoadSigFile : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	return 0;
}

int CHandlerSet::LoadSigFileFromBuf( char* bufptr, int bufsize, int adjustuvflag, float srcmult )
{

	int ret = 0;
		
	CSigFile* sigfile;
	sigfile = new CSigFile();
	if( !sigfile ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );

	ret = sigfile->LoadSigFileFromBuf( bufptr, bufsize, m_thandler, m_shandler, m_mhandler, 0, srcmult, adjustuvflag, offsetpos, rot, m_groundflag );
	if( ret ){
		DbgOut( "handlerset : LoadSigFileFromBuf : sigfile LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		delete sigfile;
		return 1;
	}
	delete sigfile;
	
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : LoadSigFileFromBuf : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	return 0;
}



int CHandlerSet::ImportSigFile( char* fname, int adjustuvflag, float srcmult, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot )
{
	int ret;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、Loadを行う。
		ret = LoadSigFile( fname, adjustuvflag, 1.0f );
		if( ret ){
			DbgOut( "handlerset : ImportSigFile : LoadSigFile 0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		return 0;
	}

	if( m_thandler || m_shandler || m_mhandler ){
		ret = m_shandler->DestroyBoneInfo( m_mhandler );
		if( ret ){
			DbgOut( "handlerset : ImportSigFile : DestroyBoneInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = m_mhandler->DestroyMotionObj();
		if( ret ){
			DbgOut( "handlerset : ImportSigFile : DestroyMotionObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	CSigFile* sigfile;
	sigfile = new CSigFile();
	if( !sigfile ){
		DbgOut( "handlerset : ImportSigFile : sigfile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 multvec( srcmult, srcmult, srcmult );
	ret = sigfile->LoadSigFile( fname, m_thandler, m_shandler, m_mhandler, m_shandler->s2shd_leng, multvec, adjustuvflag, offsetpos, rot, m_groundflag );
	if( ret ){
		if( ret == D3DAPPERR_MEDIANOTFOUND ){
			DbgOut( "handlerset : ImportSigFile : media not found warning %s !!!\n", fname );
			_ASSERT( 0 );
			delete sigfile;
			return 0;//!!!!!!!!
		}else{
			DbgOut( "handlerset : ImportSigFile : sigfile LoadSigFile error 1 !!!\n" );
			delete sigfile;
			_ASSERT( 0 );
			return 1;
		}
	}
	
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : ImportSigFile : CreateBoneInfo error !!!\n" );
		delete sigfile;
		_ASSERT( 0 );
		return -1;
	}

	
	delete sigfile;//!!!!!

	return 0;
}

int CHandlerSet::ImportSigFileFromBuf( char* bufptr, int bufsize, int adjustuvflag, float srcmult, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot )
{

	int ret;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、Loadを行う。
		ret = LoadSigFileFromBuf( bufptr, bufsize, adjustuvflag, srcmult );
		if( ret ){
			DbgOut( "handlerset : ImportSigFileFromBuf : LoadSigFile 0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		return 0;
	}

	if( m_thandler || m_shandler || m_mhandler ){
		ret = m_shandler->DestroyBoneInfo( m_mhandler );
		if( ret ){
			DbgOut( "handlerset : ImportSigFileFromBuf : DestroyBoneInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = m_mhandler->DestroyMotionObj();
		if( ret ){
			DbgOut( "handlerset : ImportSigFileFromBuf : DestroyMotionObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	CSigFile* sigfile;
	sigfile = new CSigFile();
	if( !sigfile ){
		DbgOut( "handlerset : ImportSigFileFromBuf : sigfile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = sigfile->LoadSigFileFromBuf( bufptr, bufsize, m_thandler, m_shandler, m_mhandler, m_shandler->s2shd_leng, srcmult, adjustuvflag, offsetpos, rot, m_groundflag );
	if( ret ){
		if( ret == D3DAPPERR_MEDIANOTFOUND ){
			DbgOut( "handlerset : ImportSigFileFromBuf : media not found warning !!!\n" );
			_ASSERT( 0 );
			delete sigfile;
			return 0;//!!!!!!!!
		}else{
			DbgOut( "handlerset : ImportSigFileFromBuf : sigfile LoadSigFile error 1 !!!\n" );
			delete sigfile;
			_ASSERT( 0 );
			return 1;
		}
	}
	
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : ImportSigFileFromBuf : CreateBoneInfo error !!!\n" );
		delete sigfile;
		_ASSERT( 0 );
		return -1;
	}

	
	delete sigfile;//!!!!!

	return 0;
}



int CHandlerSet::LoadQuaFile( char* fname, int* cookieptr, int* framemaxptr, float mvmult, CPanda* panda, int pndid )
{
	//DWORD time0, time1, time2, time3, time4, time5;

//time0 = timeGetTime();

	int ret = 0;
	CQuaFile* quafile;
	quafile = new CQuaFile();
	if( !quafile ){
		_ASSERT( 0 );
		return 1;
	}
	
	D3DXVECTOR3 multvec = D3DXVECTOR3( mvmult, mvmult, mvmult );
	LONG motid;
	if( !panda ){
		ret = quafile->LoadQuaFile( m_thandler, m_shandler, m_mhandler, fname, &motid, multvec );
	}else{
		ret = quafile->LoadQuaFileFromPnd( panda, pndid, fname, m_thandler, m_shandler, m_mhandler, &motid, mvmult );
	}
	*cookieptr = (int)motid;
	if( ret ){
		_ASSERT( 0 );
		delete quafile;
		return 1;
	}
	delete quafile;

	CMotionCtrl* mcptr = (*m_mhandler)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	*framemaxptr = *(miptr->motnum + *cookieptr) - 1; //(framenum - 1)
	
//time1 = timeGetTime();



//////////////////////////




//time2 = timeGetTime();


	ret = m_mhandler->MakeRawmat( *cookieptr, m_shandler, -1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->RawmatToMotionmat( *cookieptr, m_shandler, -1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = CalcSplineParam( *cookieptr, 1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = m_mhandler->CalcMPEuler( m_shandler, *cookieptr, m_mhandler->GetZaType( *cookieptr ) );//!!!!!!!!!!!!!!!
	if( ret ){
		DbgOut( "handlerset : LoadQUAFile : mh CalcMPEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( m_mhandler->m_kindnum == 1 ){
		ret = m_mhandler->SetMotionKind( *cookieptr );
		if( ret ){
			DbgOut( "handlerset : LoadQUAFile : mh SetMotionKind error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}


int CHandlerSet::LoadQuaFileFromBuf( char* bufptr, int bufsize, int* cookieptr, int* framemaxptr, float mvmult )
{
	int ret = 0;
	CQuaFile* quafile;
	quafile = new CQuaFile();
	if( !quafile ){
		_ASSERT( 0 );
		return 1;
	}
	LONG motid;
	ret = quafile->LoadQuaFileFromBuf( m_thandler, m_shandler, m_mhandler, bufptr, bufsize, &motid, mvmult );
	*cookieptr = (int)motid;
	if( ret ){
		DbgOut( "handlerset : LoadQuaFileFromBuf : quafile LoadQuaFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		delete quafile;
		return 1;
	}
	delete quafile;

	CMotionCtrl* mcptr = (*m_mhandler)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	*framemaxptr = *(miptr->motnum + *cookieptr) - 1; //(framenum - 1)
	
//time1 = timeGetTime();


//////////////////////////

//time2 = timeGetTime();


	ret = m_mhandler->MakeRawmat( *cookieptr, m_shandler, -1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->RawmatToMotionmat( *cookieptr, m_shandler, -1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = CalcSplineParam( *cookieptr, 1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->CalcMPEuler( m_shandler, *cookieptr, m_mhandler->GetZaType( *cookieptr ) );//!!!!!!!!!!!!!!!
	if( ret ){
		DbgOut( "handlerset : LoadQUAFile : mh CalcMPEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_mhandler->m_kindnum == 1 ){
		ret = m_mhandler->SetMotionKind( *cookieptr );
		if( ret ){
			DbgOut( "handlerset : LoadQUAFile : mh SetMotionKind error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

int CHandlerSet::CalcSplineParam( int motid, int allmkind, int calcseri )
{
	int ret;

	int calcrot, calcmv, calcsc;
	calcrot = 1;
	calcmv = 1;
	calcsc = 1;
	int changefumode = 0;

	if( calcseri == -1 ){
		int doseri;
		for( doseri = 0; doseri < m_shandler->s2shd_leng; doseri++ ){
			CShdElem* dose;
			dose = (*m_shandler)( doseri );
			_ASSERT( dose );
			if( dose->IsJoint() ){
				if( calcrot == 1 ){
					ret = AutoSplineAll( m_mhandler, motid, doseri, MKIND_ROT, changefumode );
					_ASSERT( !ret );
				}
				if( calcmv == 1 ){
					ret = AutoSplineAll( m_mhandler, motid, doseri, MKIND_MV, changefumode );
					_ASSERT( !ret );
				}
				if( calcsc == 1 ){
					ret = AutoSplineAll( m_mhandler, motid, doseri, MKIND_SC, changefumode );
					_ASSERT( !ret );
				}
			}
		}
		ret = m_mhandler->FillUpMotion( m_shandler, motid, -1, 0, -1, 0 );
		_ASSERT( !ret );
	}else{
		if( calcrot == 1 ){
			ret = AutoSplineAll( m_mhandler, motid, calcseri, MKIND_ROT, changefumode );
			_ASSERT( !ret );
		}
		if( calcmv == 1 ){
			ret = AutoSplineAll( m_mhandler, motid, calcseri, MKIND_MV, changefumode );
			_ASSERT( !ret );
		}
		if( calcsc == 1 ){
			ret = AutoSplineAll( m_mhandler, motid, calcseri, MKIND_SC, changefumode );
			_ASSERT( !ret );
		}
		ret = m_mhandler->FillUpMotion( m_shandler, motid, calcseri, 0, -1, 0 );
		_ASSERT( !ret );
	}

	return 0;
}


int CHandlerSet::SetCurrentBSphereData()
{
	int ret;
	ret = m_shandler->SetBSphereData( m_mhandler, -1, 0, 0 );
	if( ret ){
		DbgOut( "HandlerSet : SetCurrentBSphereData : SetBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	DbgOut( "handlerset : SetCurrentBSphereData : totalbs : befcenter %f %f %f, tracenter %f %f %f, rmag %f\n",
		m_mhandler->m_curbs.befcenter.x, m_mhandler->m_curbs.befcenter.y, m_mhandler->m_curbs.befcenter.z,
		m_mhandler->m_curbs.tracenter.x, m_mhandler->m_curbs.tracenter.y, m_mhandler->m_curbs.tracenter.z,
		m_mhandler->m_curbs.rmag );

	return 0;
}


int CHandlerSet::SetMotionKind( int motcookie )
{
	int ret;
	ret = m_mhandler->SetMotionKind( motcookie );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return ret;
}	
int CHandlerSet::GetMotionKind( int* motkindptr )
{
	int ret;
	ret = m_mhandler->GetMotionKind( motkindptr );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return ret;
}

int CHandlerSet::SetNewPoseML()
{
	int ret;
	ret = m_mhandler->SetNewPoseML( m_shandler );
	if( ret ){
		DbgOut( "hs : SetNewPoseML : mh SetNewPoseML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::SetNewPose()
{
	int nextno;
	nextno = m_mhandler->SetNewPose( m_shandler );
	if( nextno < 0 ){
		DbgOut( "handlerset : mh SetNewPause error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	return nextno;
}

int CHandlerSet::TransformBillboard( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX matView, D3DXVECTOR3* camerapos, D3DXVECTOR3* cameratarget )
{

    DWORD dwClipWidth  = g_scsize.x / 2;
    DWORD dwClipHeight = g_scsize.y / 2;

	HRESULT hr;
	D3DCAPS9 caps8;
	hr = pd3dDevice->GetDeviceCaps( &caps8 );
	if( hr != D3D_OK ){
		DbgOut( "HandlerSet : TransformBillboard : GetDeviceCaps error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////////////
	//D3DXMATRIX billmatW;
	
    D3DXVECTOR3 vDir = *cameratarget - *camerapos;
	D3DXVec3Normalize( &vDir, &vDir );

    //if( vDir.x > 0.0f )
    //    D3DXMatrixRotationY( &billmatW, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
    //else
    //   D3DXMatrixRotationY( &billmatW, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );

///// for dirmode == 0
	if( vDir.x == 0.0f ){
		if( vDir.z >= 0.0f )
			D3DXMatrixRotationY( &m_billmatW, 0.0f );
		else
			D3DXMatrixRotationY( &m_billmatW, D3DX_PI );

	}else if( vDir.x > 0.0f ){
		D3DXMatrixRotationY( &m_billmatW, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
	}else{
		D3DXMatrixRotationY( &m_billmatW, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
	}

///// for dirmode == 1
	D3DXMATRIX vm;
	vm = matView;
	vm._41 = 0.0f;
	vm._42 = 0.0f;
	vm._43 = 0.0f;

	D3DXQUATERNION vmq;
	D3DXQuaternionRotationMatrix( &vmq, &vm );

	D3DXQUATERNION vmqinv;
	D3DXQuaternionInverse( &vmqinv, &vmq );

	D3DXMatrixRotationQuaternion( &m_billmatW2, &vmqinv );
	


//////////////
	int ret = 0;

	CBillboard::s_campos = *camerapos;//!!!!!!!!!!!!!!!!!!!!!!!!
	ret = m_shandler->SortBillboard( vDir );
	if( ret ){
		DbgOut( "handlerset : TransformBillboard : shandler SortBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////////////

	if( 0 ){// billboard をTLMODE_D3Dに移行したため、必要なし。
//		ret = m_shandler->TransformDispData( *camerapos, dwClipWidth, dwClipHeight, m_mhandler, &m_billmatW, &matView, &g_matProj, 0, 1 );
//		if( ret ){
//			_ASSERT( 0 );
//			return 1;
//		}
		//ret = m_shandler->UpdateVertexBuffer( 0 );
		//if( ret ){
		//	_ASSERT( 0 );
		//	return 1;
		//}
	}

	return 0;
}


int CHandlerSet::UpdateParticle( D3DXMATRIX matView, D3DXVECTOR3* camerapos, D3DXVECTOR3* cameratarget, int srcfps )
{
////////////////
	//D3DXMATRIX billmatW;
	
    D3DXVECTOR3 vDir = *cameratarget - *camerapos;
	D3DXVec3Normalize( &vDir, &vDir );

///// for dirmode == 0
	if( vDir.x == 0.0f ){
		if( vDir.z >= 0.0f )
			D3DXMatrixRotationY( &m_billmatW, 0.0f );
		else
			D3DXMatrixRotationY( &m_billmatW, D3DX_PI );

	}else if( vDir.x > 0.0f ){
		D3DXMatrixRotationY( &m_billmatW, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
	}else{
		D3DXMatrixRotationY( &m_billmatW, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
	}

///// for dirmode == 1
	D3DXMATRIX vm;
	vm = matView;
	vm._41 = 0.0f;
	vm._42 = 0.0f;
	vm._43 = 0.0f;

	D3DXQUATERNION vmq;
	D3DXQuaternionRotationMatrix( &vmq, &vm );

	D3DXQUATERNION vmqinv;
	D3DXQuaternionInverse( &vmqinv, &vmq );

	D3DXMatrixRotationQuaternion( &m_billmatW2, &vmqinv );
	


//////////////
	int ret = 0;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : UpdateParticle : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( srcfps >= 0 ){
		ret = bbselem->billboard->UpdateParticle( m_shandler, srcfps );
		if( ret ){
			DbgOut( "handlerset : UpdateParticle : bb UpdateParticle error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CHandlerSet::Transform( D3DXVECTOR3 camerapos, LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX matView, int lightflag, int trabone_only )
{
	int ret = 0;


	ret = m_shandler->TransformDispData( camerapos, m_mhandler, &m_gpd.m_matWorld, &matView, &g_matProj, lightflag, trabone_only );
	if( ret ){
		DbgOut( "handlerset : Transform : sh TransformDispData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return ret;
}
int CHandlerSet::RenderShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 camerapos )
{
	int ret = 0;
	
	if( m_billboardflag == 0 ){
		ret = m_shandler->RenderShadowMap0( pd3dDevice, POLYDISP_ALL, m_mhandler, &m_gpd.m_matWorld, matView, matProj, camerapos, 0, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		//billboardには、worldmatに、回転行列を入れる。
		ret = m_shandler->RenderShadowMap0( pd3dDevice, POLYDISP_ALL, m_mhandler, &m_gpd.m_matWorld, matView, matProj, camerapos, &m_billmatW, &m_billmatW2 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return ret;
}

int CHandlerSet::RenderShadowMap1( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 camerapos )
{
	int ret = 0;
	
	if( m_billboardflag == 0 ){
		ret = m_shandler->RenderShadowMap1( pd3dDevice, withalpha, POLYDISP_ALL, m_mhandler, &m_gpd.m_matWorld, matView, matProj, camerapos, 0, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		//billboardには、worldmatに、回転行列を入れる。
		ret = m_shandler->RenderShadowMap1( pd3dDevice, withalpha, POLYDISP_ALL, m_mhandler, &m_gpd.m_matWorld, matView, matProj, camerapos, &m_billmatW, &m_billmatW2 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return ret;
}


int CHandlerSet::Render( int inbatch, LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos )
{
	int ret = 0;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// この関数内での、matProjは、TLMODE_ORGのmatProjとは、異なるので、注意！！！！！！！！！
// Renderに渡すmatProjは、TLMODE_D3Dの時にしか、影響しない！！！！！！
// TLMODE_ORG時のmatProjは、Transform時に計算される！！！！！
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	int curcookie = -1;

	if( m_billboardflag == 0 ){
		ret = m_shandler->Render( 0,inbatch, pd3dDevice, withalpha, POLYDISP_ALL, m_mhandler, curcookie, m_gpd.m_matWorld, matView, g_matProj, camerapos, 0, 0, NOGLOW );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		//billboardには、worldmatに、回転行列を入れる。
		ret = m_shandler->Render( 0, inbatch, pd3dDevice, withalpha, POLYDISP_ALL, m_mhandler, curcookie, m_gpd.m_matWorld, matView, g_matProj, camerapos, &m_billmatW, &m_billmatW2, NOGLOW );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return ret;
}

int CHandlerSet::RenderZandN( int znflag, LPDIRECT3DDEVICE9 pdev, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos )
{
	int ret;
	ret= m_shandler->Render( znflag, 0, pdev, withalpha, POLYDISP_ALL, m_mhandler, -1, 
		m_gpd.m_matWorld, matView, g_matProj, camerapos, 0, 0, NOGLOW );		
	if( ret ){
		DbgOut( "hs : RenderZndN : sh Render error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::RenderZandN2( int znflag, LPDIRECT3DDEVICE9 pdev, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos )
{
	int ret;
	ret= m_shandler->Render( znflag, 0, pdev, withalpha, POLYDISP_ALL, m_mhandler, -1, 
		m_gpd.m_matWorld, matView, g_matProj, camerapos, 0, 0, NOGLOW );		
	if( ret ){
		DbgOut( "hs : RenderZndN : sh Render error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}



int CHandlerSet::TransformBillboardDispData( D3DXMATRIX matView, D3DXVECTOR3 campos )
{
	int ret;

	if( m_billboardflag ){
		ret = m_shandler->TransformBillboardDispData( m_gpd.m_matWorld, m_billmatW, m_billmatW2, matView, g_matProj, campos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CHandlerSet::Restore( LPDIRECT3DDEVICE9 pd3dDevice, HWND appwnd, int billboardid, int needcalc, CPanda* panda, int pndid )
{
	HRESULT hr;
	D3DCAPS9 caps8;
	hr = pd3dDevice->GetDeviceCaps( &caps8 );
	if( hr != D3D_OK ){
		DbgOut( "HandlerSet : Restore : GetDeviceCaps error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_groundflag == 1) || (m_billboardflag != 0) || (m_extlineflag == 1))
	//if( (m_billboardflag != 0) || (m_extlineflag == 1))
		m_shandler->m_TLmode = TLMODE_D3D;//!!!!!
	else
		m_shandler->m_TLmode = TLMODE_ORG;

	//DbgOut( "HandlerSet : Restore : Caps : MaxActiveLights %d\n", caps8.MaxActiveLights );

	int ret = 0;

	if( billboardid == -2 ){
		CShdElem* selem;
		int seri;
		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			selem = (*m_shandler)( seri );
			_ASSERT( selem );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				ret = Restore( pd3dDevice, appwnd, seri, 1 );
				if( ret ){
					DbgOut( "hs : Restore : bb Restore error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
		return 0;//!!!!!!!!!!!!!!!!!!!!

	}else if( billboardid == -1 ){
		if( panda ){
			ret = m_shandler->CreateTextureFromPnd( pd3dDevice, appwnd, 0, g_miplevels, g_mipfilter, 0, panda, pndid );
			if( ret ){
				DbgOut( "HandlerSet : Restore : m_shandler->CreateTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = m_shandler->CreateTexture( pd3dDevice, appwnd, 0, g_miplevels, g_mipfilter, 0, m_billboardflag );
			if( ret ){
				DbgOut( "HandlerSet : Restore : m_shandler->CreateTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		ret = m_shandler->SetMikoAnchorApply( m_thandler );
		if( ret ){
			DbgOut( "handlerset : Restore : SetMikoAnchorApply error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		DbgOut( "handlerset : Restore : bef CreateDispObj\n" );		

		ret = m_shandler->CreateDispObj( pd3dDevice, m_mhandler, needcalc );
		if( ret ){
			DbgOut( "HandlerSet : Restore : m_shandler->CreateDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		DbgOut( "handlerset : Restore : aft CreateDispObj\n" );		


		if( m_shandler->m_shader == COL_TOON1 ){
			ret = m_shandler->CreateToon1Buffer( pd3dDevice );
			if( ret ){
				DbgOut( "handlerset : restore : sh CreateToon1Buffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


	}else{
		if( billboardid >= m_shandler->s2shd_leng ){
			DbgOut( "HandlerSet : Restore : billboardid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = m_shandler->AddTexture( pd3dDevice, appwnd, billboardid, g_miplevels, g_mipfilter );
		if( ret ){
			DbgOut( "HandlerSet : Restore : m_shandler->CreateTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		ret = (*m_shandler)( billboardid )->CreateDispObj( m_shandler->m_seri2boneno, pd3dDevice, m_mhandler, m_shandler, m_shandler->m_TLmode, 1 );
		if( ret ){
			DbgOut( "HandlerSet : Restore : selem CreateDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( m_shandler->m_shader == COL_TOON1 ){
			ret = (*m_shandler)( billboardid )->CreateToon1Buffer( m_shandler, pd3dDevice );
			if( ret ){
				DbgOut( "handlerset : restore : billboard CreateToon1Buffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	ret = m_shandler->SetDispFlag( 1 );
	if( ret ){
		DbgOut( "HandlerSet : Restore : m_shandler->SetDispFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->SetTexTransparent();
	if( ret ){
		DbgOut( "HandlerSet : Restore : m_shandler->SetTexTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_curtex0 != NULL ){
		pd3dDevice->SetTexture( 0, NULL );
		g_curtex0 = NULL;
	}
	if( g_curtex1 != NULL ){
		pd3dDevice->SetTexture( 1, NULL );
		g_curtex1 = NULL;
	}


//	if( billboardid == -1 ){
//		m_shandler->ChkAlphaNum();
//	}
	
//	ret = m_shandler->CreateMorphObj( pd3dDevice, m_mhandler );
//	if( ret ){
//		DbgOut( "HandlerSet : Restore : m_shandler->CreateMorphObj error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}



	return 0;
}

int CHandlerSet::Invalidate( int resetflag )
{
	int ret = 0;
	ret = m_shandler->InvalidateDispObj();
	if( ret ){
		DbgOut( "HandlerSet : Invalidate : m_shandler->InvalidateDispObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->InvalidateTexture( resetflag );
	if( ret ){
		DbgOut( "HandlerSet : Invalidate : m_shandler->InvalidateTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = m_shandler->DestroyAfterImage();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::SetMotionStep( int motcookie, int step )
{
	int ret = 0;
	ret = m_mhandler->SetMotionStep( motcookie, step );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

/***
int CHandlerSet::SetPosition( D3DXVECTOR3 pos )
{
	m_matWorld._41 = pos.x;
	m_matWorld._42 = pos.y;
	m_matWorld._43 = pos.z;
	
	return 0;
}
int CHandlerSet::SetRotate( D3DXMATRIX mat )
{
	//m_matWorld = mat;

	//平行移動成分は、コピーしない。
	m_matWorld._11 = mat._11;
	m_matWorld._12 = mat._12;
	m_matWorld._13 = mat._13;

	m_matWorld._21 = mat._21;
	m_matWorld._22 = mat._22;
	m_matWorld._23 = mat._23;

	m_matWorld._31 = mat._31;
	m_matWorld._32 = mat._32;
	m_matWorld._33 = mat._33;

	return 0;
}
***/

int CHandlerSet::GetDirVector3( int mult, int* xptr, int* yptr, int* zptr )
{



	return 0;
}


/***
int CHandlerSet::SetRotate( D3DXVECTOR3 rot )
{
	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;
	float fDeg2Pai = (float)DEG2PAI;

	cosx = (float)cos( rot.x * 0.5f * fDeg2Pai );
	sinx = (float)sin( rot.x * 0.5f * fDeg2Pai );
	cosy = (float)cos( rot.y * 0.5f * fDeg2Pai );
	siny = (float)sin( rot.y * 0.5f * fDeg2Pai );
	cosz = (float)cos( rot.z * 0.5f * fDeg2Pai );
	sinz = (float)sin( rot.z * 0.5f * fDeg2Pai );

	qx.SetParams( cosx, sinx, 0.0f, 0.0f );
	qy.SetParams( cosy, 0.0f, siny, 0.0f );
	qz.SetParams( cosz, 0.0f, 0.0f, sinz );

	q = qx * qy * qz;

	CMatrix2 rotmat;
	rotmat = q.MakeRotMat();
	m_matWorld._11 = rotmat.data[0][0];
	m_matWorld._12 = rotmat.data[0][1];
	m_matWorld._13 = rotmat.data[0][2];

	m_matWorld._21 = rotmat.data[1][0];
	m_matWorld._22 = rotmat.data[1][1];
	m_matWorld._23 = rotmat.data[1][2];

	m_matWorld._31 = rotmat.data[2][0];
	m_matWorld._32 = rotmat.data[2][1];
	m_matWorld._33 = rotmat.data[2][2];

	return 0;
}
***/

int CHandlerSet::ChkConfBillboard2( float centerx, float centery, float centerz, float srcr, int* resultptr, float confrate, int* confidptr, int arrayleng, int* confnumptr )
{

	//オブジェクト単位の判定は、しない。

	D3DXVECTOR3 srccenter;
	srccenter.x = centerx;
	srccenter.y = centery;
	srccenter.z = centerz;

	int ret;	
	ret = m_shandler->ChkConfPartsBySphere( m_mhandler, srccenter, srcr, resultptr, confrate, 1.0f, confidptr, arrayleng, confnumptr );
	if( ret ){
		DbgOut( "handlerset : ChkConfBillboard2 : shandler ChkConfPartsBySphere error !!!\n" );
		_ASSERT( 0 );
		return 1;			
	}

	return 0;

}


int CHandlerSet::ChkConfBillboard( CHandlerSet* chkhs, int* confflagptr, float confrate, int* confbbid, int arrayleng, int* confnumptr )
{
	//オブジェクト単位の判定は、しない。

	int ret;	
	ret = m_shandler->ChkConfPartsBySphere( m_mhandler, chkhs->m_shandler, chkhs->m_mhandler, confflagptr, confrate, 1.0f, confbbid, arrayleng, confnumptr );
	if( ret ){
		DbgOut( "handlerset : ChkConfBillboard : shandler ChkConfPartsBySphere error !!!\n" );
		_ASSERT( 0 );
		return 1;			
	}

	return 0;
}

int CHandlerSet::ChkConfParticle( CHandlerSet* chkhs, float rate, int* flagptr )
{
	*flagptr = 0;//!!!!!!!!!!!!

	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : ChkConfParticle : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//ret = bbselem->billboard->SetParticlePos( srcpos );

	int flag1 = 0;
	flag1 = bbselem->billboard->ChkConfParticle( &(chkhs->m_mhandler->m_curbs), rate );
	if( flag1 == 0 ){
		return 0;//!!!!!!!!!!!!
	}

	chkhs->m_shandler->SetCurDS();

	int seri;
	for( seri = 0; seri < chkhs->m_shandler->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*chkhs->m_shandler)( seri );
		_ASSERT( selem );

		if( (selem->notuse == 0) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){
			DWORD dispswitchno = selem->dispswitchno;
			if( (chkhs->m_shandler->m_curds + dispswitchno)->state != 0 ){  
				flag1 = bbselem->billboard->ChkConfParticle( &(selem->curbs), rate );
				if( flag1 ){
					*flagptr = 1;
					return 0;//!!!!!!!!!
				}
			}
		}
	}

	return 0;
}


int CHandlerSet::ChkConfBySphere( CHandlerSet* chkhs, int conflevel, int* confflagptr )
{
	if( conflevel < 1 )
		return 0;

	int ret;

	int result1 = 0;
	//オブジェクト単位の判定。
	ret = m_mhandler->ChkConfTotalBySphere( chkhs->m_mhandler, &result1 );
	if( ret ){
		DbgOut( "handlerset : ChkConfBySphere : total ChkConflict error !!!\n" );
		_ASSERT( 0 );
		return 1;			
	}


	int result2 = 0;
	if( (conflevel >= 2) && result1 ){
	//パーツ単位の判定。
		int confnum;
		ret = m_shandler->ChkConfPartsBySphere( m_mhandler, chkhs->m_shandler, chkhs->m_mhandler, &result2, 1.0f, 1.0f, 0, 0, &confnum );
		if( ret ){
			DbgOut( "handlerset : ChkConfBySphere : parts ChkConflict error !!!\n" );
			_ASSERT( 0 );
			return 1;			
		}

		*confflagptr = result2;
	}else{
		*confflagptr = result1;
	}

	return 0;
}

int CHandlerSet::ChkConfBySphere2( int srcpartno, CHandlerSet* chkhs, int chkpartno, int* confflagptr )
{

	*confflagptr = 0;

	CBSphere srcbs, chkbs;
	int srctotal = 0;
	int chktotal = 0;
	CShdElem* selem;
	if( srcpartno >= 0 ){
		if( (srcpartno == 0) || (srcpartno >= m_shandler->s2shd_leng) ){
			DbgOut( "handlerset : ChkConfBySphere2 : partno1 error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}
		
		selem = (*m_shandler)( srcpartno );
		srcbs = selem->curbs;

	}else{
		srcbs = m_mhandler->m_curbs;
		srctotal = 1;
	}

	if( chkpartno >= 0 ){
		if( (chkpartno == 0) || (chkpartno >= chkhs->m_shandler->s2shd_leng) ){
			DbgOut( "handlerset : ChkConfBySphere2 : partno2 error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}

		selem = (*(chkhs->m_shandler))( chkpartno );
		chkbs = selem->curbs;
	}else{
		chkbs = chkhs->m_mhandler->m_curbs;
		chktotal = 1;
	}

	int ret;
	if( (srctotal == 1) && (chktotal == 1) ){
		ret = ChkConfBySphere( chkhs, 2, confflagptr );
		if( ret ){
			DbgOut( "handlerset : ChkConfBySphere error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	}else{

		int confnum;
		if( srctotal == 1 ){
			ret = m_shandler->ChkConfPartsBySphere( m_mhandler, chkhs->m_shandler, chkhs->m_mhandler, chkpartno, confflagptr, 1.0f, 1.0f, 0, 0, &confnum );
			if( ret ){
				DbgOut( "handlerset : ChkConfBySphere 2 : sh ChkConfPartsBySphere error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


		}else if( chktotal == 1 ){
			ret = chkhs->m_shandler->ChkConfPartsBySphere( chkhs->m_mhandler, m_shandler, m_mhandler, srcpartno, confflagptr, 1.0f, 1.0f, 0, 0, &confnum );
			if( ret ){
				DbgOut( "handlerset : ChkConfBySphere 3 : sh ChkConfPartsBySphere error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = srcbs.ChkConflict( &chkbs, confflagptr, 1.0f, 1.0f );
			//*confflagptr = ret;//retは０

		}
	}
	


	return 0;
}

int CHandlerSet::ChkConfBySphere3( int* srcpartno, int srcpartnum, CHandlerSet* chkhs, int* chkpartno, int chkpartnum, int* confflagptr )
{

	int srccnt, chkcnt;
	int result = 0;
	int ret;

	for( srccnt = 0; srccnt < srcpartnum; srccnt++ ){
		for( chkcnt = 0; chkcnt < chkpartnum; chkcnt++ ){
			ret = ChkConfBySphere2( *(srcpartno + srccnt), chkhs, *(chkpartno + chkcnt), &result );
			if( ret ){
				DbgOut( "handlerset : ChkConfBySphere3 : ChkConfBySphere2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( result != 0 ){
				break;
			}
		}

		if( result != 0 ){
			break;
		}
	}

	*confflagptr = result;

	return 0;
}




int CHandlerSet::ChkConflict( CHandlerSet* chkhs, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2, int* inviewflag )
{
	
	CBSphere srcbs, chkbs;
	srcbs = m_mhandler->m_curbs;
	chkbs = chkhs->m_mhandler->m_curbs;

	int vflag = 0;
	if( srcbs.visibleflag == 0 )
		vflag |= 1;
	if( chkbs.visibleflag == 0 )
		vflag |= 2;

	*inviewflag = vflag;
	//if( vflag )
	//	return 0;
	
	int ret;
	ret = m_shandler->m_bbox.ChkConflict( chkhs->m_shandler->m_bbox, rate1, rate2 );

	return ret;
	

	/***
	CBSphere srcbs, chkbs;
	srcbs = m_mhandler->m_curbs;
	chkbs = chkhs->m_mhandler->m_curbs;
	int vflag = 0;
	if( srcbs.visibleflag == 0 )
		vflag |= 1;
	if( chkbs.visibleflag == 0 )
		vflag |= 2;
	*inviewflag = vflag;

	int ret;
	ret = ChkConflict2( -1, chkhs, -1, rate1, rate2, inviewflag );

	return ret;
	***/

}

int CHandlerSet::ChkConflict2( int srcpartno, CHandlerSet* chkhs, int chkpartno, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2, int* inviewflag )
{
	CBBox srcbb, chkbb;
	CShdElem* selem;
	int vflag = 0;
	int srctotal = 0;
	int chktotal = 0;

	int confflag = 0;

	*inviewflag = 0;

	int srcdsstate = 1;
	int chkdsstate = 1;
	int srcnotuse = 0;
	int chknotuse = 0;

	if( srcpartno >= 0 ){
		if( (srcpartno == 0) || (srcpartno >= m_shandler->s2shd_leng) ){
			DbgOut( "handlerset : ChkConflict2 : partno1 error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}
		
		selem = (*m_shandler)( srcpartno );
		srcbb = selem->m_bbox;
		srcdsstate = ( m_shandler->m_curds + selem->dispswitchno)->state;
		srcnotuse = selem->notuse;
		if( (selem->curbs.visibleflag == 0) || !srcdsstate || (srcnotuse == 1) )
			vflag |= 1;
	}else{
		srcbb = m_shandler->m_bbox;
		if( m_mhandler->m_curbs.visibleflag == 0 )
			vflag |= 1;
		srctotal = 1;
	}

	if( chkpartno >= 0 ){
		if( (chkpartno == 0) || (chkpartno >= chkhs->m_shandler->s2shd_leng) ){
			DbgOut( "handlerset : ChkConflict2 : partno2 error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}

		selem = (*(chkhs->m_shandler))( chkpartno );
		chkbb = selem->m_bbox;
		chkdsstate = ( chkhs->m_shandler->m_curds + selem->dispswitchno)->state;
		chknotuse = selem->notuse;

		if( (selem->curbs.visibleflag == 0) || !chkdsstate || (chknotuse == 1) )
			vflag |= 2;
	}else{
		chkbb = chkhs->m_shandler->m_bbox;
		if( chkhs->m_mhandler->m_curbs.visibleflag == 0 )
			vflag |= 2;
		chktotal = 1;
	}

	*inviewflag = vflag;

	int ret;
	int srcno;

	if( (srctotal == 1) && (chktotal == 1) ){
		int result1 = 0;
		int result2 = 0;
		//confflag = 0;

		result1 = srcbb.ChkConflict( chkbb, rate1, rate2 );
		if( result1 > 0 ){
			for( srcno = 0; srcno < m_shandler->s2shd_leng; srcno++ ){
				ret = chkhs->m_shandler->ChkConfParts( chkhs->m_mhandler, m_shandler, m_mhandler, srcno, &result2 );
				if( ret ){
					DbgOut( "handlerset : ChkConflict2 : 0 : sh ChkConfParts error !!!\n" );
					_ASSERT( 0 );
					return -1;
				}
				//confflag += result2;
				if( result2 > 0 )
					break;//!!!!!!!!!
			}
		}
		
		return result2;
		
	}else{
		if( srctotal == 1 ){
			if( (chkdsstate != 0) && (chknotuse == 0) ){
				ret = m_shandler->ChkConfParts( m_mhandler, chkhs->m_shandler, chkhs->m_mhandler, chkpartno, &confflag );
				if( ret ){
					DbgOut( "handlerset : ChkConflict2 : 0 : sh ChkConfParts error !!!\n" );
					_ASSERT( 0 );
					return -1;
				}
				return confflag;
			}else{
				return 0;
			}
		}else if( chktotal == 1 ){
			if( (srcdsstate != 0) && (srcnotuse == 0) ){
				ret = chkhs->m_shandler->ChkConfParts( chkhs->m_mhandler, m_shandler, m_mhandler, srcpartno, &confflag );
				if( ret ){
					DbgOut( "handlerset : ChkConflict2 : 1 : sh ChkConfParts error !!!\n" );
					_ASSERT( 0 );
					return -1;
				}
				return confflag;
			}else{
				return 0;
			}
		}else{
			if( (chkdsstate == 0) || (chknotuse == 1) || (srcdsstate == 0) || (srcnotuse == 1) ){
				return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}else{
				ret = m_shandler->ChkConfBBX( srcpartno, chkhs->m_shandler, chkpartno, &confflag );
				if( ret ){
					DbgOut( "handlerset : ChkConflict2 : 2 : sh ChkConfBBX error !!!\n" );
					_ASSERT( 0 );
					return -1;
				}
				return confflag;
			}
		}
	}

	//return 0;//!!!
}

int CHandlerSet::ChkConflict3( int* srcpartno, int srcpartnum, CHandlerSet* chkhs, int* chkpartno, int chkpartnum, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2, int* inviewflag )
{

	int srccnt, chkcnt;
	int result = 0;
	int outviewcnt = 0;
	int calccnt = 0;
	int tempinview;

	for( srccnt = 0; srccnt < srcpartnum; srccnt++ ){

		for( chkcnt = 0; chkcnt < chkpartnum; chkcnt++ ){
		
			tempinview = 0;
			result = ChkConflict2( *(srcpartno + srccnt), chkhs, *(chkpartno + chkcnt), rate1, rate2, &tempinview );
			if( result < 0 ){
				DbgOut( "handlerset : ChkConflict3 : ChkConflict2 error !!!\n" );
				_ASSERT( 0 );
				return -1;
			}
			calccnt++;

			if( tempinview != 0 ){
				outviewcnt++;
			}

			if( result != 0 ){
				break;
			}
		}

		if( result != 0 ){
			break;
		}
	}

	if( calccnt == outviewcnt ){
		*inviewflag = 3;
	}else{
		*inviewflag = 0;
	}

	return result;
}


/***
int CHandlerSet::ChkConflict( CHandlerSet* chkhs, float rate )
{
	if( (m_shandler->m_bbox.setflag == 0) || 
		(chkhs->m_shandler->m_bbox.setflag == 0) )
		return 0;

	int conf = 0;
	float minx, maxx, miny, maxy, minz, maxz;
	float chkminx, chkmaxx, chkminy, chkmaxy, chkminz, chkmaxz;
	minx = m_shandler->m_bbox.minx;
	maxx = m_shandler->m_bbox.maxx;
	miny = m_shandler->m_bbox.miny;
	maxy = m_shandler->m_bbox.maxy;
	minz = m_shandler->m_bbox.minz;
	maxz = m_shandler->m_bbox.maxz;

	chkminx = chkhs->m_shandler->m_bbox.minx;
	chkmaxx = chkhs->m_shandler->m_bbox.maxx;
	chkminy = chkhs->m_shandler->m_bbox.miny;
	chkmaxy = chkhs->m_shandler->m_bbox.maxy;
	chkminz = chkhs->m_shandler->m_bbox.minz;
	chkmaxz = chkhs->m_shandler->m_bbox.maxz;

	float lengx, lengy, lengz;
	float chklengx, chklengy, chklengz;

	lengx = maxx - minx;
	lengy = maxy - miny;
	lengz = maxz - minz;

	chklengx = chkmaxx - chkminx;
	chklengy = chkmaxy - chkminy;
	chklengz = chkmaxz - chkminz;

	int xconf = 0;
	int yconf = 0;
	int zconf = 0;
	if( lengx >= chklengx ){
		if( ((chkminx >= minx) && (chkminx <= maxx)) ||
			((chkmaxx >= minx) && (chkmaxx <= maxx)) ){
			xconf = 1;
		}
	}else{
		if( ((minx >= chkminx) && (minx <= chkmaxx)) ||
			((maxx >= chkminx) && (maxx <= chkmaxx)) ){
			xconf = 1;
		}
	}

	if( lengy >= chklengy ){
		if( ((chkminy >= miny) && (chkminy <= maxy)) ||
			((chkmaxy >= miny) && (chkmaxy <= maxy)) ){
			yconf = 1;
		}
	}else{
		if( ((miny >= chkminy) && (miny <= chkmaxy)) ||
			((maxy >= chkminy) && (maxy <= chkmaxy)) ){
			yconf = 1;
		}
	}

	if( lengz >= chklengz ){
		if( ((chkminz >= minz) && (chkminz <= maxz)) ||
			((chkmaxz >= minz) && (chkmaxz <= maxz)) ){
			zconf = 1;
		}
	}else{
		if( ((minz >= chkminz) && (minz <= chkmaxz)) ||
			((maxz >= chkminz) && (maxz <= chkmaxz)) ){
			zconf = 1;
		}
	}

	if( xconf && yconf && zconf )
		return 1;
	else
		return 0;	
}
***/
int CHandlerSet::CreateAfterImage( LPDIRECT3DDEVICE9 pd3dDevice, int imagenum )
{

	return 0;
}
int CHandlerSet::DestroyAfterImage()
{

	return 0;
}

int CHandlerSet::SetAlpha( float alpha, int partno, int updateflag )
{

//DbgOut( "handlerset : SetAlpha : alpha %f, partno %d\n", alpha, partno );

	int ret = 0;

	/***
	int updateflag;
	if( m_TLmode == TLMODE_D3D ){
		updateflag = 1;
	}else{
		updateflag = 0;
	}
	ret = m_shandler->SetAlpha( alpha, partno, updateflag );
	***/

	ret = m_shandler->SetAlpha( alpha, partno, updateflag );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

/***
int CHandlerSet::SetDirectionalLight( D3DXVECTOR3 dir )
{
	int ret = 0;
	ret = m_shandler->SetDirectionalLight( dir );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}
***/

int CHandlerSet::GetBoundaryBox( float* dstptr )
{

	*dstptr = (m_shandler->m_bbox).minx;
	*( dstptr + 1 )= (m_shandler->m_bbox).maxx;

	*( dstptr + 2 )= (m_shandler->m_bbox).miny;
	*( dstptr + 3 )= (m_shandler->m_bbox).maxy;

	*( dstptr + 4 )= (m_shandler->m_bbox).minz;
	*( dstptr + 5 )= (m_shandler->m_bbox).maxz;

	return 0;
}

int CHandlerSet::GetDispSwitch2( int* swptr, int leng )
{
	int ret;

	ret = m_mhandler->GetDispSwitch2( m_shandler, swptr, leng );
	if( ret ){
		DbgOut( "handlerset : GetDispSwitch2 : mh GetDispSwitch2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int CHandlerSet::GetDispSwitch( int motid, int frameno, int* dswitch )
{
	int ret;

	if( motid == -1 ){
	
		*dswitch = m_shandler->m_curdispswitch;
	
	}else{
		
		DWORD dwds;
		ret = m_mhandler->GetDispSwitch( m_shandler, motid, frameno, &dwds );
		if( ret ){
			DbgOut( "handlerset : GetDispSwitch : mh GetDispSwitch error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*dswitch = dwds;

	}



	return 0;
}

int CHandlerSet::SetDispSwitch2( int boneno, int mk, int swid, int srcframeno, int flag )
{
	//swid のちぇっく
	if( (swid < 0) || (swid >= DISPSWITCHNUM) ){
		DbgOut( "hs SetDispSwitch2 : swid out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( mk >= 0 ){

		CDSKey* curdsk = 0;
		int ret;
		ret = m_mhandler->ExistDSKey( boneno, mk, srcframeno, &curdsk );
		if( ret ){
			DbgOut( "hs SetDispSwitch2 : mh ExistDSKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( !curdsk ){
			ret = m_mhandler->CreateDSKey( boneno, mk, srcframeno, &curdsk );
			if( ret || !curdsk ){
				DbgOut( "hs SetDispSwitch2 : mh CreateDSKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = curdsk->SetDSChange( m_shandler->m_ds + swid, flag );
		if( ret ){
			DbgOut( "hs SetDispSwitch2 : dsk SetDSChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		( m_shandler->m_ds2 + swid )->state = flag;
	}

	return 0;

}

int CHandlerSet::SetDispSwitchForce( int swid, int state )
{
	//swid のちぇっく
	if( (swid < 0) || (swid >= DISPSWITCHNUM) ){
		DbgOut( "hs SetDispSwitchForce : swid out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	( m_shandler->m_dsF + swid )->state = state;

	return 0;
}



int CHandlerSet::SetDispSwitch( int mk, int swid, int srcframeno, int flag )
{
	/***
	//swid のちぇっく
	if( (swid < 0) || (swid >= DISPSWITCHNUM) ){
		DbgOut( "SetDispSwitch : swid out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( mk >= 0 ){
		//フレーム数のチェック
		CMotionCtrl* mcptr;
		int i, framemax;
		CMotionCtrl* firstjoint = 0;
		for( i = 0; i < m_mhandler->s2mot_leng; i++ ){
			mcptr = (*m_mhandler)( i );
			if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
				firstjoint = mcptr;
				break;
			}
		}

		if( firstjoint == 0 )
			return 0;

		CMotionInfo* minfo;
		minfo = firstjoint->motinfo;
		if( !minfo )
			return 0;

		//mkのちぇっく
		if( (mk < 0) || (mk >= minfo->kindnum) ){
			DbgOut( "SetDispSwitch : mk out of range error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		framemax = *(minfo->motnum + mk) - 1;

		//dispswitch のセット

		CMotionPoint2* mpptr;
		CMotionPoint2* curmp;
		int fno;
		for( i = 0; i < m_mhandler->s2mot_leng; i++ ){
			mcptr = (*m_mhandler)( i );
			if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){

				for( fno = 0; fno <= framemax; fno++ ){
					//最初のmotionpointを探す。
					mpptr = mcptr->IsExistMotionPoint( mk, fno );
					if( mpptr )
						break;
				}
				curmp = mpptr;
				while( curmp ){
					
					if( curmp->m_frameno >= srcframeno ){
						if( flag == 0 )
							curmp->dispswitch &= ~(Bitno2Mask[ swid ]);
						else
							curmp->dispswitch |= Bitno2Mask[ swid ];
					}

					curmp = curmp->next;//全てのmotionpointのswitchを変更する。
				}

			}

		}
	}else{
		if( flag == 0 ){
			m_shandler->m_curdispswitch &= ~(Bitno2Mask[ swid ]);
		}else{
			m_shandler->m_curdispswitch |= Bitno2Mask[ swid ];
		}
	}
	***/

	return 0;
}

int CHandlerSet::SetMotionFrameNo( int mk, int srcframeno )
{
	int ret;

	CMotionCtrl* curmc;	
	CMotionCtrl* firstjoint = 0;
	int elemno;
	for( elemno = 1; elemno < m_mhandler->s2mot_leng; elemno++ ){
		curmc = (*m_mhandler)( elemno );

		if( curmc->IsJoint() && (curmc->type != SHDMORPH) ){
			firstjoint = curmc;
			break;
		}
	}

	if( !firstjoint ){
		_ASSERT( 0 );
		return 0;
	}

	CMotionInfo* minfo;
	minfo = firstjoint->motinfo;

	if( !minfo ){
		_ASSERT( 0 );
		return 0;
	}

	if( (mk < 0) || (mk >= minfo->kindnum) ){
		DbgOut( "HandlerSet : SetMotionFrameNo : mk out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int maxframe;
	maxframe = *(minfo->motnum + mk) - 1;
	if( (srcframeno < 0) || (srcframeno > maxframe) ){
		DbgOut( "HandlerSet : SetMotionFrameNo : srcframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->SetMotionFrameNo( m_shandler, mk, srcframeno, 1 );
	if( ret ){
		DbgOut( "HandlerSet : SetMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::ChkInView( D3DXMATRIX matView, int* retptr, D3DXMATRIX* matProj )
{
	int ret;
	
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// この関数内での、matProjは、TLMODE_ORGのmatProjとは、異なるので、注意！！！！！！！！！
// Renderに渡すmatProjは、TLMODE_D3Dの時にしか、影響しない！！！！！！
// TLMODE_ORG時のmatProjは、Transform時に計算される！！！！！
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	D3DXMATRIX* mProjptr;
	if( matProj ){
		mProjptr = matProj;
	}else{
		mProjptr = &g_matProj;
	}

	ret = m_shandler->ChkInView( m_mhandler, &m_gpd.m_matWorld, &matView, mProjptr, retptr );
	if( ret ){
		DbgOut( "handlerset : shandler->ChkInView error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::SetMovableArea( char* bmpname, float maxx, float maxz, int divx, int divz, float wallheight )
{

	int ret = 0;
		
	CGroundBMP* gbmp;
	gbmp = new CGroundBMP();
	if( !gbmp ){
		_ASSERT( 0 );
		return 1;
	}

	ret = gbmp->LoadMovableBMP( m_thandler, m_shandler, m_mhandler, bmpname, maxx, maxz, divx, divz, wallheight );
	if( ret ){
		_ASSERT( 0 );
		delete gbmp;
		return 1;
	}
	delete gbmp;
	

	ret = SetInvisibleFlag( -1, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}	


	return 0;
}


int CHandlerSet::LoadGroundBMP( char* bmpname1, char* bmpname2, char* bmpname3, char* texturename, float maxx, float maxz, int divx, int divz, float maxheight )
{
	int ret = 0;
		
	CGroundBMP* gbmp;
	gbmp = new CGroundBMP();
	if( !gbmp ){
		_ASSERT( 0 );
		return 1;
	}

	ret = gbmp->LoadGroundBMP( m_thandler, m_shandler, m_mhandler, bmpname1, bmpname2, bmpname3, texturename, maxx, maxz, divx, divz, maxheight );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	delete gbmp;
	

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}	


	return 0;
}

int CHandlerSet::SetBeforePos()
{
	
	m_befpos.x = m_gpd.m_matWorld._41;
	m_befpos.y = m_gpd.m_matWorld._42;
	m_befpos.z = m_gpd.m_matWorld._43;
	
	return 0;
}


int CHandlerSet::ChkConfGround( CHandlerSet* charahs, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr )
{
	D3DXVECTOR3 bpos, npos;
	bpos = charahs->m_befpos;
	npos.x = charahs->m_gpd.m_matWorld._41;
	npos.y = charahs->m_gpd.m_matWorld._42;
	npos.z = charahs->m_gpd.m_matWorld._43;


	int ret;
	ret = ChkConfGround2( bpos, npos, mode, diffmaxy, mapminy, resultptr, adxptr, adyptr, adzptr, nxptr, nyptr, nzptr );
	if( ret ){
		DbgOut( "hs : ChkConfGround : ChkConfGround2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::ChkConfGround2( D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr )
{

	D3DXVECTOR3 adjustv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 nv(0.0f, 0.0f, 0.0f);
	int ret;
	ret = m_shandler->ChkConfGround( 0, 0, &m_gpd.m_matWorld, befpos, newpos, m_mhandler, mode, diffmaxy, mapminy, resultptr, &adjustv, &nv );
	if( ret ){
		DbgOut( "handlerset : ChkConfGround2 : shandler ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*adxptr = adjustv.x;
	*adyptr = adjustv.y;
	*adzptr = adjustv.z;

	
	*nxptr = nv.x;
	*nyptr = nv.y;
	*nzptr = nv.z;

	return 0;

}

int CHandlerSet::ChkConfGroundPart( CHandlerSet* charahs, int groundpart, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr )
{
	D3DXVECTOR3 befpos, newpos;
	befpos = charahs->m_befpos;
	newpos.x = charahs->m_gpd.m_matWorld._41;
	newpos.y = charahs->m_gpd.m_matWorld._42;
	newpos.z = charahs->m_gpd.m_matWorld._43;


	D3DXVECTOR3 adjustv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 nv(0.0f, 0.0f, 0.0f);
	int ret;
	ret = m_shandler->ChkConfGroundPart( &m_gpd.m_matWorld, groundpart, befpos, newpos, m_mhandler, mode, diffmaxy, mapminy, resultptr, &adjustv, &nv );
	if( ret ){
		DbgOut( "handlerset : ChkConfGroundPart : shandler ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*adxptr = adjustv.x;
	*adyptr = adjustv.y;
	*adzptr = adjustv.z;

	
	*nxptr = nv.x;
	*nyptr = nv.y;
	*nzptr = nv.z;

	return 0;
}

int CHandlerSet::ChkConfGroundPart2( int groundpart, D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int mode, float diffmaxy, float mapminy, int* resultptr, float* adxptr, float* adyptr, float* adzptr, float* nxptr, float* nyptr, float* nzptr )
{

	D3DXVECTOR3 adjustv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 nv(0.0f, 0.0f, 0.0f);
	int ret;
	ret = m_shandler->ChkConfGroundPart( &m_gpd.m_matWorld, groundpart, befpos, newpos, m_mhandler, mode, diffmaxy, mapminy, resultptr, &adjustv, &nv );
	if( ret ){
		DbgOut( "handlerset : ChkConfGroundPart2 : shandler ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*adxptr = adjustv.x;
	*adyptr = adjustv.y;
	*adzptr = adjustv.z;

	*nxptr = nv.x;
	*nyptr = nv.y;
	*nzptr = nv.z;
	
	return 0;

}

int CHandlerSet::SetNaviLineOnGround( CNaviLine* nl, float mapmaxy, float mapminy )
{
	CNaviPoint* curnp = nl->nphead;

	D3DXVECTOR3 adjustv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 nv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 befpos, newpos;
	
	int result;
	int ret;
	
	while( curnp ){

		befpos.x = curnp->point.x;
		befpos.y = mapmaxy;
		befpos.z = curnp->point.z;

		newpos.x = curnp->point.x;
		newpos.y = mapminy;
		newpos.z = curnp->point.z;

		ret = m_shandler->ChkConfGround( 0, 0, &m_gpd.m_matWorld, befpos, newpos, m_mhandler, 0, 200.0f, mapminy - 100.0f, &result, &adjustv, &nv );
		if( ret ){
			DbgOut( "handlerset : SetNaviLineOnGround : shandler ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( result != 0 ){
			curnp->point = adjustv;
		}else{
			curnp->point = newpos;
		}

		curnp = curnp->next;
	}

	return 0;
}


int CHandlerSet::SetPosOnGround( CHandlerSet* hs, float mapmaxy, float mapminy, float posx, float posz )
{
	D3DXVECTOR3 adjustv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 nv(0.0f, 0.0f, 0.0f);

	D3DXVECTOR3 befpos, newpos;
	befpos.x = posx;
	befpos.y = mapmaxy;
	befpos.z = posz;

	newpos.x = posx;
	newpos.y = mapminy;
	newpos.z = posz;

	int result;
	int ret;
	ret = m_shandler->ChkConfGround( 0, 0, &m_gpd.m_matWorld, befpos, newpos, m_mhandler, 0, 200.0f, mapminy - 100.0f, &result, &adjustv, &nv );
	if( ret ){
		DbgOut( "handlerset : SetPosOnGround : shandler ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	D3DXVECTOR3 setpos;
	if( result != 0 ){
		setpos = adjustv;
	}else{
		setpos = newpos;
	}

	hs->m_gpd.m_gpe.pos = setpos;
	hs->m_gpd.m_gpe.e3dpos = setpos;
	hs->m_gpd.CalcMatWorld();

	return 0;
}
int CHandlerSet::SetPosOnGroundPart( int groundpart, CHandlerSet* hs, float mapmaxy, float mapminy, float posx, float posz )
{
	D3DXVECTOR3 adjustv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 nv(0.0f, 0.0f, 0.0f);

	D3DXVECTOR3 befpos, newpos;
	befpos.x = posx;
	befpos.y = mapmaxy;
	befpos.z = posz;

	newpos.x = posx;
	newpos.y = mapminy;
	newpos.z = posz;

	int result;
	int ret;
	ret = m_shandler->ChkConfGroundPart( &m_gpd.m_matWorld, groundpart, befpos, newpos, m_mhandler, 0, 200.0f, -100.0f, &result, &adjustv, &nv );
	if( ret ){
		DbgOut( "handlerset : SetPosOnGroundPart : shandler ChkConfGroundPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	D3DXVECTOR3 setpos;
	if( result != 0 ){
		setpos = adjustv;
	}else{
		setpos = newpos;
	}

	hs->m_gpd.m_gpe.pos = setpos;
	hs->m_gpd.m_gpe.e3dpos = setpos;
	hs->m_gpd.CalcMatWorld();

	return 0;
}


int CHandlerSet::GetPartNoByName( char* partname, int* partnoptr )
{
	
	int ret;

	ret = m_thandler->GetPartNoByName( partname, partnoptr );
	if( ret ){
		DbgOut( "handlerset : GetPartNoByName : thandler GetPartNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetBoneNoByName( char* bonename, int* bonenoptr )
{
	
	int ret;

	ret = m_thandler->GetBoneNoByName( bonename, bonenoptr, m_shandler, 0 );
	if( ret ){
		DbgOut( "handlerset : GetBoneNoByName : thandler GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CHandlerSet::GetVertNumOfPart( int partno, int* vertnumptr )
{
	int ret;

	ret = m_shandler->GetVertNumOfPart( partno, vertnumptr );
	if( ret ){
		DbgOut( "handlerset : GetVertNumOfPart : shandler GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetVertPos( int kind, int partno, int vertno, float* vxptr, float* vyptr, float* vzptr )
{
	int ret;

	D3DXVECTOR3 vpos( 0.0f, 0.0f, 0.0f );

	ret = m_shandler->GetVertPos3( kind, m_mhandler, m_gpd.m_matWorld, partno, vertno, &vpos );
	if( ret ){
		DbgOut( "handlerset : GetVertPos : shandler GetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	*vxptr = vpos.x;
	*vyptr = vpos.y;
	*vzptr = vpos.z;

	return 0;
}

int CHandlerSet::SetVertPosBatchDouble( int partno, int* vnoarray, int vnum, double* varray, int aorder )
{
	int ret;

	ret = m_shandler->SetVertPosBatchDouble( m_mhandler, partno, vnoarray, vnum, varray, aorder );
	if( ret ){
		DbgOut( "handlerset : SetVertPosBatch : SetVertPosBatchDouble error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetVertPosBatchVec3( int partno, int* vnoarray, int vnum, D3DXVECTOR3* varray )
{
	int ret;

	ret = m_shandler->SetVertPosBatchVec3( m_mhandler, partno, vnoarray, vnum, varray );
	if( ret ){
		DbgOut( "handlerset : SetVertPosBatch : SetVertPosBatchDouble error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}



int CHandlerSet::SetVertPos( int partno, int vertno, float vertx, float verty, float vertz )
{
	int ret;

	D3DXVECTOR3 vpos;

	vpos.x = vertx;
	vpos.y = verty;
	vpos.z = vertz;

	ret = m_shandler->SetVertPos( m_mhandler, partno, vertno, vpos );
	if( ret ){
		DbgOut( "handlerset : SetVertpos : shandler SetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::CreateBillboard( int particleflag, int cmpalways )
{
	int ret;

	if( m_bbio ){
		DbgOut( "handlerset : CreateBillboard : m_bbio already exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_bbio = new CBillboardIO();
	if( !m_bbio ){
		DbgOut( "handlerset : CreateBillboard : m_bbio alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_bbio->CreateBillboard( m_thandler, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "handlerset : CreateBillboard : m_bbio CreateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	_ASSERT( bbselem->type == SHDBILLBOARD );

	if( particleflag != 0 ){
		ret = bbselem->InitParticleParams();
		if( ret ){
			DbgOut( "handlerset : CreateBillboard : bbselem IniParticleParams error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( bbselem->billboard ){
			bbselem->billboard->cmpalways = cmpalways;
		}else{
			_ASSERT( 0 );
		}
	}


	return 0;
}
int CHandlerSet::AddBillboard( char* bmpname, float width, float height, int transparentflag, int dirmode, int orgflag, int* bbidptr )
{

	if( !m_bbio ){
		DbgOut( "handlerset : AddBillboard : m_bbio not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = m_bbio->AddBillboard( bmpname, width, height, transparentflag, dirmode, orgflag, bbidptr );
	if( ret ){
		DbgOut( "handlerset : AddBillboard : m_bbio AddBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = m_shandler->DestroyBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : AddBillboard : DestroyBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : AddBillboard : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	//{
	//	CBillboard* bb;
	//	bb = (*m_shandler)( 1 )->billboard;
	//	_ASSERT( bb );

	//	CBillboardElem* bbelem;
	//	bbelem = bb->bbarray;

	//	DbgOut( "handlerset : selem 1 type %d, selem 2 type %d\n", 
	//		(*m_shandler)( 1 )->type, (*m_shandler)( 2 )->type );

	//	DbgOut( "handlerset : bbarray 0 : selem %x, useflag %d, dispflag %d\n",
	//		bbelem->selem, bbelem->useflag, bbelem->dispflag );
	//}


	return 0;
}

int CHandlerSet::SetBillboardPos( int bbid, float posx, float posy, float posz )
{
	int ret;	

	ret = m_shandler->SetBillboardPos( bbid, posx, posy, posz );
	if( ret ){
		DbgOut( "handlerset : SetBillboardPos : shandler SetBillboardPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::RotateBillboard( int bbid, float fdeg, int rotkind )
{
	int ret;

	ret = m_shandler->RotateBillboard( bbid, fdeg, rotkind );
	if( ret ){
		DbgOut( "handlerset : RotateBillboard : sh RotateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetBillboardSize( int bbid, float width, float height, int dirmode, int orgflag )
{
	int ret;
	ret = m_shandler->SetBillboardSize( bbid, width, height, dirmode, orgflag );
	if( ret ){
		DbgOut( "hs : SetBillboardSize : sh SetBillboardSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag )
{
	int ret;

	ret = m_shandler->SetBillboardUV( bbid, unum, vnum, texno, revuflag );
	if( ret ){
		DbgOut( "handlerset : SetBillboardUV : sh SetBillboardUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CHandlerSet::SetBillboardDispFlag( int bbid, int flag )
{
	int ret;

	ret = m_shandler->SetBillboardDispFlag( bbid, flag );
	if( ret ){
		DbgOut( "handlerset : SetBillboardDispFlag : shandler SetBillboardDispFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CHandlerSet::SetBillboardOnGround( int bbid, CHandlerSet* groundhs, float mapmaxy, float mapminy, float posx, float posz )
{
	D3DXVECTOR3 adjustv(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 nv(0.0f, 0.0f, 0.0f);

	D3DXVECTOR3 befpos, newpos;
	befpos.x = posx;
	befpos.y = mapmaxy;
	befpos.z = posz;

	newpos.x = posx;
	newpos.y = mapminy;
	newpos.z = posz;

	int result;
	int ret;
	ret = groundhs->m_shandler->ChkConfGround( 0, 0, &(groundhs->m_gpd.m_matWorld), befpos, newpos, groundhs->m_mhandler, 0, 200.0f, 0.0f, &result, &adjustv, &nv );
	if( ret ){
		DbgOut( "handlerset : SetBillboardOnGround : shandler ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	D3DXVECTOR3 setpos;
	if( result != 0 ){
		setpos = adjustv;
	}else{
		setpos = newpos;
//		_ASSERT( 0 );
	}
	
	
	ret = m_shandler->SetBillboardPos( bbid, setpos.x, setpos.y, setpos.z );
	if( ret ){
		DbgOut( "handlerset : SetBillboardOnGround : shandler SetBillboardPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::DestroyBillboard( int bbid )
{
	int ret;

	ret = m_shandler->DestroyBillboard( bbid, m_thandler, m_mhandler );
	if( ret ){
		DbgOut( "handlerset : DestroyBillboard : shandler DestroyBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::LoadMQOFileAsMovableAreaFromBuf( HWND apphwnd, char* bufptr, int bufsize, float fmult )
{

	int ret;

	CMQOFile* mqofile;
	mqofile = new CMQOFile( apphwnd );
	if( !mqofile ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );

	ret = mqofile->LoadMQOFileFromBuf( fmult, bufptr, bufsize, m_thandler, m_shandler, m_mhandler, 0, 1, BONETYPE_RDB2, 0, offsetpos, rot );
	if( ret ){
		DbgOut( "handlerset : LoadMQOFileAsMovableAreaFromBuf : mqofile LoadMQOFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}


	ret = SetInvisibleFlag( -1, 1 );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}


	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}	

	delete mqofile;

	return 0;
}


int CHandlerSet::LoadMQOFileAsMovableArea( char* filename, float mult, HWND apphwnd, int* hsidptr )
{
	int ret;

	if( !filename ){
		DbgOut( "handlerset : LoadMQOFileAsMovableArea : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !*filename ){
		DbgOut( "handlerset : LoadMQOFileAsMovableArea : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CMQOFile* mqofile;
	mqofile = new CMQOFile( apphwnd );
	if( !mqofile ){
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );

	ret = mqofile->LoadMQOFile( mult, filename, m_thandler, m_shandler, m_mhandler, 0, 1, BONETYPE_RDB2, 0, offsetpos, rot );
	if( ret ){
		DbgOut( "handlerset : LoadMQOFileAsMovableArea : mqofile LoadMQOFile error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}


	ret = SetInvisibleFlag( -1, 1 );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}


	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}	

	delete mqofile;

	return 0;
}


int CHandlerSet::ImportMQOFileAsGround( char* mqoname, HWND apphwnd, int adjustuvflag, float fmult, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot )
{

	int ret;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、エラー。
		DbgOut( "handlerset : ImportMQOFileAsGround : data not exist before import error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_thandler || m_shandler || m_mhandler ){
		ret = m_shandler->DestroyBoneInfo( m_mhandler );
		if( ret ){
			DbgOut( "handlerset : ImportMQOFileAsGround : DestroyBoneInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = m_mhandler->DestroyMotionObj();
		if( ret ){
			DbgOut( "handlerset : ImportMQOFileAsGround : DestroyMotionObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	CMQOFile* mqofile;
	mqofile = new CMQOFile( apphwnd );
	if( !mqofile ){
		_ASSERT( 0 );
		return 1;
	}

	ret = mqofile->LoadMQOFile( fmult, mqoname, m_thandler, m_shandler, m_mhandler, m_shandler->s2shd_leng, 1, BONETYPE_RDB2, adjustuvflag, offsetpos, rot );
	if( ret ){
		DbgOut( "handlerset : LoadMQOFileAsGround : mqofile LoadMQOFile error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}	

	delete mqofile;

	return 0;
}


int CHandlerSet::LoadMQOFileAsGround( char* filename, float mult, HWND apphwnd, int adjustuvflag, int* hsidptr, int bonetype )
{

	int ret;

	if( !filename ){
		DbgOut( "handlerset : LoadMQOFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !*filename ){
		DbgOut( "handlerset : LoadMQOFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CMQOFile* mqofile;
	mqofile = new CMQOFile( apphwnd );
	if( !mqofile ){
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );

	ret = mqofile->LoadMQOFile( mult, filename, m_thandler, m_shandler, m_mhandler, 0, 1, bonetype, adjustuvflag, offsetpos, rot );
	if( ret ){
		DbgOut( "handlerset : LoadMQOFileAsGround : mqofile LoadMQOFile error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}

	if( bonetype == BONETYPE_MIKO ){
		m_shandler->m_mikoblendtype = MIKOBLEND_MIX;//!!!!!!!!!!!!!!!!!!!
	}

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}	

	delete mqofile;

	return 0;
}

int CHandlerSet::LoadMQOFileAsChara( char* filename, float mult, HWND apphwnd, int adjustuvflag, int* hsidptr, int bonetype )
{

	int ret;

	if( !filename ){
		DbgOut( "handlerset : LoadMQOFileAsChara : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !*filename ){
		DbgOut( "handlerset : LoadMQOFileAsChara : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CMQOFile* mqofile;
	mqofile = new CMQOFile( apphwnd );
	if( !mqofile ){
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );

	ret = mqofile->LoadMQOFile( mult, filename, m_thandler, m_shandler, m_mhandler, 0, 0, bonetype, adjustuvflag, offsetpos, rot );
	if( ret ){
		DbgOut( "handlerset : LoadMQOFileAsChara : mqofile LoadMQOFile error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}

	if( bonetype == BONETYPE_MIKO ){
		m_shandler->m_mikoblendtype = MIKOBLEND_MIX;//!!!!!!!!!!!!!!!!!!!
	}

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}	

	delete mqofile;

	return 0;
}


int CHandlerSet::SaveSigFile( char* filename )
{
	int ret;

	if( !filename || !*filename ){
		DbgOut( "handlerset : SaveSigFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSigFile* sigfile;
	sigfile = new CSigFile();
	if( !sigfile ){
		DbgOut( "handlerset : SaveSigFile : sigfile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	ret = sigfile->WriteSigFile( filename, m_thandler, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "handlerset : SaveSigFile : sigfile WriteSigFile error !!!\n" );
		_ASSERT( 0 );
		delete sigfile;
		return 1;
	}

	delete sigfile;

	return 0;
}

int CHandlerSet::SaveMQOFile( char* filename, HWND apphwnd )
{
	int ret;

	if( !filename || !*filename ){
		DbgOut( "handlerset : SaveMQOFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOFile* mqofile;
	mqofile = new CMQOFile( apphwnd );
	if( !mqofile ){
		DbgOut( "handlerset : SaveMQOFile : mqofile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = mqofile->SaveMQOFile( filename, m_thandler, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "handlerset : SaveMQOFile : mqofile SaveMQOFile error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}

	delete mqofile;

	return 0;
}

int CHandlerSet::GetBillboardInfo( int srcbbid, float* posxptr, float* posyptr, float* poszptr, char* texnameptr, int* transparentptr, float* widthptr, float* heightptr )
{
	CShdElem* bbselem = 0;
	CShdElem* curselem;
	int elemno;

	for( elemno = 0; elemno < m_shandler->s2shd_leng; elemno++ ){
		curselem = (*m_shandler)( elemno );
		if( curselem->type == SHDBILLBOARD ){
			bbselem = curselem;
			break;
		}
	}

	if( !bbselem ){
		DbgOut( "handlerSet : GetBillboardInfo : SHDBILLBOARD not eixst error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 getpos;

	int ret;
	ret = bbselem->GetBillboardInfo( srcbbid, &getpos, texnameptr, transparentptr, widthptr, heightptr );
	if( ret ){
		DbgOut( "handlerset : GetBillboardInfo : bbselem GetAllBillboardInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posxptr = getpos.x;
	*posyptr = getpos.y;
	*poszptr = getpos.z;


	return 0;
}


int CHandlerSet::GetNearBillboard( float srcposx, float srcposy, float srcposz, float maxdist, int* bbidptr )
{
	CShdElem* bbselem = 0;
	CShdElem* curselem;
	int elemno;

	for( elemno = 0; elemno < m_shandler->s2shd_leng; elemno++ ){
		curselem = (*m_shandler)( elemno );
		if( curselem->type == SHDBILLBOARD ){
			bbselem = curselem;
			break;
		}
	}

	if( !bbselem ){
		*bbidptr = -1;
		return 0;
	}

	D3DXVECTOR3 srcpos;

	srcpos.x = srcposx;
	srcpos.y = srcposy;
	srcpos.z = srcposz;

	int ret;
	ret = bbselem->GetNearBillboard( srcpos, maxdist, bbidptr );
	if( ret ){
		DbgOut( "handlerset : GetNearBillboard : bbselem GetNearBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetInvisibleFlag( int partno, int* flagptr )
{
	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : GetInvisibleFlag : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem = (*m_shandler)( partno );
	_ASSERT( selem );

	*flagptr = selem->invisibleflag;


	return 0;
}


int CHandlerSet::SetInvisibleFlag( int partno, int srcflag )
{
	if( partno >= 0 ){
		if( partno >= m_shandler->s2shd_leng ){
			DbgOut( "handlerset : SetInvisibleFlag : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem = (*m_shandler)( partno );
		_ASSERT( selem );

		selem->invisibleflag = srcflag;

	}else{
		// partno < 0 の場合は、全てのelem のinvisibleflagをセットする。

		int elemno;
		for( elemno = 0; elemno < m_shandler->s2shd_leng; elemno++ ){
			CShdElem* selem = (*m_shandler)( elemno );
			_ASSERT( selem );

			selem->invisibleflag = srcflag;
		}

	}
	return 0;
}


int CHandlerSet::ChkConfWall( CHandlerSet* charahs, float dist, int* resultptr, float* adjustxptr, float* adjustyptr, float* adjustzptr, float* nxptr, float* nyptr, float* nzptr )
{
	D3DXVECTOR3 bpos, npos;
	bpos = charahs->m_befpos;
	npos.x = charahs->m_gpd.m_matWorld._41;
	npos.y = charahs->m_gpd.m_matWorld._42;
	npos.z = charahs->m_gpd.m_matWorld._43;


	int ret;
	ret = ChkConfWall2( 0, 0, bpos.x, bpos.y, bpos.z, npos.x, npos.y, npos.z, dist, resultptr, adjustxptr, adjustyptr, adjustzptr, nxptr, nyptr, nzptr );
	_ASSERT( !ret );

	return ret;

}

int CHandlerSet::ChkConfWall2( int* partarray, int partnum, float befposx, float befposy, float befposz, float newposx, float newposy, float newposz, float dist, int* resultptr, float* adjustxptr, float* adjustyptr, float* adjustzptr, float* nxptr, float* nyptr, float* nzptr )
{

	int ret;

	D3DXVECTOR3 befpos, newpos;
	D3DXVECTOR3 adjustv, nv;

	befpos.x = befposx;
	befpos.y = befposy;
	befpos.z = befposz;

	newpos.x = newposx;
	newpos.y = newposy;
	newpos.z = newposz;


	ret = m_shandler->ChkConfWall( partarray, partnum, &m_gpd.m_matWorld, befpos, newpos, m_mhandler, dist, resultptr, &adjustv, &nv );
	if( ret ){
		DbgOut( "handlerset : ChkConfWall2 : shandler ChkConfWall error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	*adjustxptr = adjustv.x;
	*adjustyptr = adjustv.y;
	*adjustzptr = adjustv.z;

	*nxptr = nv.x;
	*nyptr = nv.y;
	*nzptr = nv.z;


	return 0;

}

int CHandlerSet::ChkConfWall3( CHandlerSet* charahs, int* partarray, int partnum, float dist, int* resultptr, float* adjustxptr, float* adjustyptr, float* adjustzptr, float* nxptr, float* nyptr, float* nzptr )
{
	D3DXVECTOR3 bpos, npos;
	bpos = charahs->m_befpos;
	npos.x = charahs->m_gpd.m_matWorld._41;
	npos.y = charahs->m_gpd.m_matWorld._42;
	npos.z = charahs->m_gpd.m_matWorld._43;


	int ret;
	ret = ChkConfWall2( partarray, partnum, bpos.x, bpos.y, bpos.z, npos.x, npos.y, npos.z, dist, resultptr, adjustxptr, adjustyptr, adjustzptr, nxptr, nyptr, nzptr );
	_ASSERT( !ret );

	return ret;

}


int CHandlerSet::SetWallOnGround( CHandlerSet* wallhs, float mapmaxy, float mapminy, float wheight )
{
	int ret;

//頂点データの修正
	ret = wallhs->m_shandler->SetWallOnGround( &(wallhs->m_gpd.m_matWorld), &m_gpd.m_matWorld, m_shandler, m_mhandler, mapmaxy, mapminy, wheight );
	if( ret ){
		DbgOut( "handlerset : SetWallOnGround : shandler SetWallOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	return 0;
}

int CHandlerSet::MagnetPosition( float dist )
{
	int ret;

	ret = m_shandler->MagnetPosition( dist );
	if( ret ){
		DbgOut( "handlerset : MagnetPosition : shandler MagnetPosition error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::CreateNaviPointClearFlag( CNaviLine* nlptr, int roundnum )
{
	int ret;

	ret = m_npcd.CreateData( serialno, nlptr, roundnum );
	if( ret ){
		DbgOut( "handlerset : CreateNaviPointClearFlag : npcd CreateData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::DestroyNaviPointClearFlag()
{
	int ret;

	ret = m_npcd.DestroyData();
	if( ret ){
		DbgOut( "handlerset : DestroyNaviPointClearFlag : npcd DestroyData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::InitNaviPointClearFlag()
{
	int ret;

	ret = m_npcd.InitFlag();
	if( ret ){
		DbgOut( "handlerset : InitNaviPointClearFlag : npcd InitFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::SetNaviPointClearFlag( D3DXVECTOR3 srcpos, float maxdist, int* pidptr, int* roundptr, float* distptr )
{
	int ret;

	ret = m_npcd.SetNaviPointClearFlag( srcpos, maxdist, pidptr, roundptr, distptr );
	if( ret ){
		DbgOut( "handlerset : SetNaviPointClearFlag : npcd SetNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetValidFlag( int partno, int flag )
{
	int ret;

	ret = m_shandler->SetValidFlag( partno, flag );
	if( ret ){
		DbgOut( "handlerset : SetValidFlag : shandler SetValidFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetDiffuse( int partno, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;

	ret = m_shandler->GetDiffuse( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "handlerset : GetDiffuse : shandler GetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetAmbient( int partno, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;

	ret = m_shandler->GetAmbient( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "handlerset : GetAmbient : shandler GetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetSpecular( int partno, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;

	ret = m_shandler->GetSpecular( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "handlerset : GetSpecular : shandler GetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetAlpha( int partno, int vertno, int* aptr )
{
	int ret;

	ret = m_shandler->GetAlpha( partno, vertno, aptr );
	if( ret ){
		DbgOut( "handlerset : GetAlpha : shandler GetAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetAlphaF( int partno, int vertno, float* aptr )
{
	int ret;

	ret = m_shandler->GetAlphaF( partno, vertno, aptr );
	if( ret ){
		DbgOut( "handlerset : GetAlpha : shandler GetAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::SetDispDiffuse( int partno, int r, int g, int b, int setflag, int vertno, int updateflag )
{
	int ret;

	ret = m_shandler->SetDispDiffuse( partno, r, g, b, setflag, vertno, updateflag );
	if( ret ){
		DbgOut( "handlerset : SetDiffuse : shandler SetRGB error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetDispSpecular( int partno, int r, int g, int b, int setflag, int vertno )
{
	int ret;

	ret = m_shandler->SetDispSpecular( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "handlerset : SetSpecular : shandler SetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetDispAmbient( int partno, int r, int g, int b, int setflag, int vertno )
{
	int ret;

	ret = m_shandler->SetDispAmbient( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "handlerset : SetAmbient : shandler SetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetBlendingMode( int partno, int bmode )
{
	int ret;

	ret = m_shandler->SetBlendingMode( partno, bmode );
	if( ret ){
		DbgOut( "handlerset : SetBlendingMode : shandler SetBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetRenderState( int partno, int statetype, DWORD value )
{
	int ret;

	ret = m_shandler->SetElemRenderState( partno, statetype, value );
	if( ret ){
		DbgOut( "handlerset : SetRenderState : shandler SetElemRenderState error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetScale( int partno, D3DXVECTOR3 scalevec, int centerflag )
{
	int ret;

	ret = m_shandler->SetDispScale( partno, scalevec, centerflag, m_gpd.m_matWorld );
	if( ret ){
		DbgOut( "handlerset : SetDispScale : shandler SetDispScale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetScreenPos3F( int partno, int vertno, int calcmode, D3DXMATRIX matView, D3DXVECTOR3* scpos )
{
	int ret;

    DWORD dwClipWidth  = g_scsize.x / 2;
    DWORD dwClipHeight = g_scsize.y / 2;

	ret = m_shandler->GetScreenPos3F( m_billboardflag, m_mhandler, partno, vertno, calcmode, dwClipWidth, dwClipHeight,
		m_gpd.m_matWorld, matView, g_matProj, scpos );
	if( ret ){
		DbgOut( "handlerset : GetScreenPos3F : shandler GetScreenPos3F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::GetScreenPos3( int partno, int* scxptr, int* scyptr, float* sczptr, D3DXMATRIX matView, int vertno, int calcmode )
{
	int ret;

    DWORD dwClipWidth  = g_scsize.x / 2;
    DWORD dwClipHeight = g_scsize.y / 2;


	//int GetScreenPos( CMotHandler* lpmh, int srcseri, int* scxptr, int* scyptr, DWORD dwClipWidth, DWORD dwClipHeight, 
	//	D3DXMATRIX matView, D3DXMATRIX matProj, int projmode );
	ret = m_shandler->GetScreenPos3( m_billboardflag, m_mhandler, partno, vertno, calcmode, scxptr, scyptr, sczptr, dwClipWidth, dwClipHeight,
		m_gpd.m_matWorld, matView, g_matProj );
	if( ret ){
		DbgOut( "handlerset : GetScreenPos : shandler GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//DbgOut( "check !!! : handlerset : GetScreenPos3 : %d %d %f\r\n", *scxptr, *scyptr, *sczptr );

	return 0;
}


int CHandlerSet::GetScreenPos( int partno, int* scxptr, int* scyptr, D3DXMATRIX matView, int vertno, int calcmode )
{
	int ret;

    DWORD dwClipWidth  = g_scsize.x / 2;
    DWORD dwClipHeight = g_scsize.y / 2;


	//int GetScreenPos( CMotHandler* lpmh, int srcseri, int* scxptr, int* scyptr, DWORD dwClipWidth, DWORD dwClipHeight, 
	//	D3DXMATRIX matView, D3DXMATRIX matProj, int projmode );
	ret = m_shandler->GetScreenPos( m_billboardflag, m_mhandler, partno, vertno, calcmode, scxptr, scyptr, dwClipWidth, dwClipHeight,
		m_gpd.m_matWorld, matView, g_matProj );
	if( ret ){
		DbgOut( "handlerset : GetScreenPos : shandler GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetNextMP( int motid, int boneno, int prevmpid, int* mpidptr )
{
	int ret;

	ret = m_mhandler->GetNextMP( motid, boneno, prevmpid, mpidptr );
	if( ret ){
		DbgOut( "handlerset : GetNextMP : mhandler GetNextMP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetBoneQ( int boneno, int motid, int frameno, int kind, CQuaternion* dstq )
{
	int ret;
	ret = m_mhandler->GetBoneQ( m_shandler, boneno, motid, frameno, kind, dstq );
	if( ret ){
		DbgOut( "handlerset : GetBoneQ : mhandler GetBoneQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( kind == 2 ){
		m_gpd.CalcMatWorld();
		*dstq = m_gpd.m_q * *dstq;//!!!!!!!!!!!!!!!!
	}


	return 0;
}

int CHandlerSet::GetCurrentBoneQ( int boneno, int kind, CQuaternion* dstq )
{
	int ret;
	ret = m_mhandler->GetCurrentBoneQ( m_shandler, boneno, kind, dstq );
	if( ret ){
		DbgOut( "handlerset : GetCurrentBoneQ : mhandler GetCurrentBoneQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( kind == 2 ){
		m_gpd.CalcMatWorld();
		*dstq = m_gpd.m_q * *dstq;//!!!!!!!!!!!!!!!!
	}


	return 0;
}

int CHandlerSet::GetMPInfo( int motid, int boneno, int mpid, CQuaternion* dstq, D3DXVECTOR3* dsttra, int* dstframeno, DWORD* dstds, int* dstinterp, D3DXVECTOR3* dstscale, int* dstuserint1 )
{

	int ret;

	ret = m_mhandler->GetMPInfo( motid, boneno, mpid, dstq, dsttra, dstframeno, dstds, dstinterp, dstscale, dstuserint1 );
	if( ret ){
		DbgOut( "handlerset : GetMPInfo : mhandler GetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMPInfo( int motid, int boneno, CQuaternion* srcqptr, MPINFO2* mpiptr, int* flagptr )
{
	int ret;
	ret = m_mhandler->SetMPInfo( motid, boneno, srcqptr, mpiptr, flagptr );
	if( ret ){
		DbgOut( "handlerset : SetMPInfo : mhandler SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}


int CHandlerSet::SetMPInfo( int motid, int boneno, int mpid, CQuaternion* srcqptr, D3DXVECTOR3* srctra, int srcframeno, DWORD srcds, int srcinterp, D3DXVECTOR3* srcscale, int srcuserint1, int* infoflagptr )
{
	int ret;

	ret = m_mhandler->SetMPInfo( motid, boneno, mpid, srcqptr, srctra, srcframeno, srcds, srcinterp, srcscale, srcuserint1, infoflagptr );
	if( ret ){
		DbgOut( "handlerset : SetMPInfo : mhandler SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::IsExistMP( int motid, int boneno, int frameno, int* mpidptr )
{
	int ret;

	ret = m_mhandler->IsExistMotionPoint( motid, boneno, frameno, mpidptr );
	if( ret ){
		DbgOut( "handlerset : IsExistMP : mhandler IsExistMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetMotionFrameLength( int motid, int* lengptr )
{
	int ret;

	ret = m_mhandler->GetMotionFrameLength( motid, lengptr );
	if( ret ){
		DbgOut( "handlerset : GetMotionFrameLength : mhandler GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
	
}


int CHandlerSet::SetMotionFrameLength( int motid, int frameleng, int initflag )
{

	int ret;

	ret = m_mhandler->ChangeTotalFrame( motid, frameleng, -1 );
	if( ret ){
		DbgOut( "handlerset : SetMotionFrameLength : mhandler ChangeTotalFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = m_mhandler->MakeRawmat( motid, m_shandler, -1, -1 );
	if( ret ){
		DbgOut( "handlerset : SetMotionFrameLength : mhandler MakeRawmat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->RawmatToMotionmat( motid, m_shandler, -1, -1 );
	if( ret ){
		DbgOut( "handlerset : SetMotionFrameLength : mhandler RawmatToMotionmat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	ret = m_shandler->RemakeBSphereData( m_mhandler, motid, frameleng );
	if( ret ){
		DbgOut( "handlerset : SetMotionFrameLength : shandler RemaekBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

	/***
	if( initflag == 1 ){
		ret = m_shandler->SetBSphereData( m_mhandler, motid, 0, frameleng - 1 );
		if( ret ){
			DbgOut( "HandlerSet : SetMotionFrameLength : SetBSphere error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	***/

	return 0;
}

int CHandlerSet::AddMotionPoint( int motid, int boneno, CQuaternion* srcqptr, D3DXVECTOR3* srctra, int srcframeno, DWORD srcds, int srcinterp, D3DXVECTOR3* srcscale, int srcuserint1, int* mpidptr )
{

	int ret;
	ret = m_mhandler->AddMotionPoint( motid, boneno, srcqptr, srctra, srcscale, srcframeno, srcds, srcinterp, srcuserint1, mpidptr );
	if( ret ){
		DbgOut( "handlerset : AddMotionPoint : mhandler AddMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::DeleteMotionPoint( int motid, int boneno, int mpid )
{
	int ret;
	ret = m_mhandler->DeleteMotionPoint( motid, boneno, mpid );
	if( ret ){
		DbgOut( "handlerset : AddMotionPoint : mhandler DeleteMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::FillUpMotion( int motid, int boneno, int startframe, int endframe, int initflag )
{
	int ret;

	ret = m_mhandler->FillUpMotion( m_shandler, motid, boneno, startframe, endframe, initflag );
	if( ret ){
		DbgOut( "handlerset : FillUpMotion : mhandler FillUpMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::CopyMotionFrame( int srcmotid, int srcframe, int dstmotid, int dstframe )
{
	int ret;

	ret = m_mhandler->CopyMotionFrame( m_shandler, srcmotid, srcframe, dstmotid, dstframe );
	if( ret ){
		DbgOut( "handlerset : CopyMotionFrame : mhandler CopyMotionFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::GetBonePos( int boneno, int poskind, int motid, int frameno, int scaleflag, D3DXVECTOR3* dstpos )
{
	int ret;

	ret = m_shandler->GetBonePos( boneno, poskind, motid, frameno, dstpos, m_mhandler, m_gpd.m_matWorld, scaleflag );
	if( ret ){
		DbgOut( "handlerset : GetBonePos : shandler GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetCurrentBonePos( int boneno, int poskind, D3DXVECTOR3* dstpos )
{
	int ret;

	ret = m_shandler->GetCurrentBonePos( boneno, poskind, dstpos, m_mhandler, m_gpd.m_matWorld );
	if( ret ){
		DbgOut( "handlerset : GetCurrentBonePos : shandler GetCurrentBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::CreateLine( D3DXVECTOR3* posptr, int pointnum, int maxpointnum, int linekind )
{

	if( m_lineio ){
		DbgOut( "handlerset : CreateLine : lineio already exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
	int ret;


	m_lineio = new CExtLineIO();
	if( !m_lineio ){
		DbgOut( "handlerset : CreateLine : lineio alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_lineio->CreateLine( m_thandler, m_shandler, m_mhandler, posptr, pointnum, maxpointnum, linekind );
	if( ret ){
		DbgOut( "handlerset : CreateLine : lineio CreateLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;


}

int CHandlerSet::CreateLine( double* posptr, int pointnum, int maxpointnum, int linekind )
{

	if( m_lineio ){
		DbgOut( "handlerset : CreateLine : lineio already exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
	int ret;
	int pointno;
	D3DXVECTOR3* pptr = 0;


	pptr = new D3DXVECTOR3[ pointnum ];
	if( !pptr ){
		DbgOut( "handlerset : CreateLine : pptr alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	//D3DXVECTOR3* curp;
	//int* cursrcp;
	//for( pointno = 0; pointno < pointnum; pointno++ ){
	//	curp = pptr + pointno;
	//	cursrcp = posptr + pointno * 3;
	//	curp->x = (float)(*cursrcp);
	//	curp->y = (float)(*(cursrcp + 1));
	//	curp->z = (float)(*(cursrcp + 2));
	//}



	//!!!!!!!! hspの２次元配列は、通常のＣの並び方と違うので注意！！！！

	D3DXVECTOR3* curp;
	double* cursrcp;
	int indexno;
	for( indexno = 0; indexno < 3; indexno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			curp = pptr + pointno;

			cursrcp = posptr + indexno * pointnum + pointno;
			
			if( indexno == 0 )
				curp->x = (float)(*cursrcp);
			else if( indexno == 1 )
				curp->y = (float)(*cursrcp);
			else if( indexno == 2 )
				curp->z = (float)(*cursrcp);

		}
	}

	m_lineio = new CExtLineIO();
	if( !m_lineio ){
		DbgOut( "handlerset : CreateLine : lineio alloc error !!!\n" );
		_ASSERT( 0 );
		delete [] pptr;//!!!!!!
		return 1;
	}

	ret = m_lineio->CreateLine( m_thandler, m_shandler, m_mhandler, pptr, pointnum, maxpointnum, linekind );
	if( ret ){
		DbgOut( "handlerset : CreateLine : lineio CreateLine error !!!\n" );
		_ASSERT( 0 );
		delete [] pptr;
		return 1;
	}
	
	if( pptr ){
		delete [] pptr;
		pptr = 0;
	}

	return 0;
}

int CHandlerSet::SetExtLineColor( int a, int r, int g, int b )
{
	//if( m_extlineflag == 0 ){
	//	_ASSERT( 0 );
	//	return 0;
	//}

	int ret;
	ret = m_shandler->SetExtLineColor( -1, a, r, g, b );
	if( ret ){
		DbgOut( "handlerset : SetExtLineColor : shandler SetExtLineColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::AddPoint2ExtLine( int previd, int* newidptr )
{
	if( m_extlineflag == 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_shandler->AddPoint2ExtLine( previd, newidptr );
	if( ret ){
		DbgOut( "handlerset : AddPoint2ExtLine : shandler AddPoint2ExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::DeletePointOfExtLine( int pid )
{
	if( m_extlineflag == 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_shandler->DeletePointOfExtLine( pid );
	if( ret ){
		DbgOut( "handlerset : DeletePointOfExtLine : shandler DeletePointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetPointPosOfExtLine( int pid, D3DXVECTOR3 srcpos )
{
	if( m_extlineflag == 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_shandler->SetPointPosOfExtLine( pid, srcpos );
	if( ret ){
		DbgOut( "handlerset : SetPointPosOfExtLine : shandler SetPointPosOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetPointPosOfExtLine( int pid, D3DXVECTOR3* dstpos )
{
	if( m_extlineflag == 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_shandler->GetPointPosOfExtLine( pid, dstpos );
	if( ret ){
		DbgOut( "handlerset : GetPointPosOfExtLine : shandler GetPointPosOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::GetNextPointOfExtLine( int previd, int* nextptr )
{
	if( m_extlineflag == 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_shandler->GetNextPointOfExtLine( previd, nextptr );
	if( ret ){
		DbgOut( "handlerset : GetNextPointOfExtLine : shandler GetNextPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::GetPrevPointOfExtLine( int pid, int* prevptr )
{
	if( m_extlineflag == 0 ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_shandler->GetPrevPointOfExtLine( pid, prevptr );
	if( ret ){
		DbgOut( "handlerset : GetPrevPointOfExtLine : shandler GetPrevPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

/***
int CHandlerSet::LoadBIMFile( char* fname )
{
	CBIMFile bimfile;

	int ret;
	ret = bimfile.LoadBIMFile( fname, m_thandler, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "handlerset : LoadBIMFile : bimfile LoadBIMFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/

int CHandlerSet::PickVert( D3DXMATRIX matView, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr )
{

	int ret;

	if( (posx < 0) || (posx > g_scsize.x) || (posy < 0) || (posy > g_scsize.y) ){
		*partptr = -1;
		*vertptr = -1;
		*getnumptr = 0;
		return 0;
	}
	DWORD dwClipWidth = g_scsize.x / 2;
	DWORD dwClipHeight = g_scsize.y / 2;
	float aspect;
	aspect = (float)dwClipWidth / (float)dwClipHeight;

	ret = m_shandler->PickVert( m_mhandler, dwClipWidth, dwClipHeight, m_gpd.m_matWorld, matView, g_matProj,
		aspect, posx, posy, rangex, rangey, partptr, vertptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "handlerset : PickVert : shandler PickVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int part = -1;
	int vert = -1;
	if( *getnumptr > 0 ){
		part = *partptr;
		vert = *vertptr;
	}

	D3DXMATRIX invview;
	D3DXMatrixInverse( &invview, NULL, &matView );

	D3DXVECTOR3 chkcampos;
	chkcampos.x = invview._41;
	chkcampos.y = invview._42;
	chkcampos.z = invview._43;


	if( (part > 0) && (vert >= 0) ){

		D3DXVECTOR3 chkvpos;
		ret = m_shandler->GetVertPos2( m_mhandler, m_gpd.m_matWorld, m_mhandler->m_curmotkind, m_mhandler->m_curframeno,
			part, vert, &chkvpos, 1 );
		if( ret ){
			DbgOut( "handlerset : PickVert : sh GetVertPos2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		int chkpart, chkface, chkrev;
		D3DXVECTOR3 chkdstpos, chkdstn;
		ret = m_shandler->ChkConfLineAndFace( m_clientWidth, m_clientHeight, 0, m_mhandler,
			m_gpd.m_matWorld, matView,// matProjX,
			chkcampos, chkvpos, 1, &chkpart, &chkface, &chkdstpos, &chkdstn, &chkrev );
		if( ret ){
			DbgOut( "handlerset : PickVert : shandler  ChkConfLineAndFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (chkpart >= 0) && (chkface >= 0) ){
			if( chkpart == part ){
				int v1, v2, v3;
				ret = m_shandler->GetVertNoOfFace( part, chkface, &v1, &v2, &v3 );
				if( ret ){
					DbgOut( "handlerset : PickVert : sh GetVertNoOfFace error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				D3DXVECTOR3 xv1, xv2, xv3;
				m_shandler->GetVertPos2( m_mhandler, m_gpd.m_matWorld, m_mhandler->m_curmotkind, m_mhandler->m_curframeno,
					part, v1, &xv1, 1 );;
				m_shandler->GetVertPos2( m_mhandler, m_gpd.m_matWorld, m_mhandler->m_curmotkind, m_mhandler->m_curframeno,
					part, v2, &xv2, 1 );;
				m_shandler->GetVertPos2( m_mhandler, m_gpd.m_matWorld, m_mhandler->m_curmotkind, m_mhandler->m_curframeno,
					part, v3, &xv3, 1 );;

				if( (chkvpos != xv1) && (chkvpos != xv2) && (chkvpos != xv3) ){
					//とりけし
					part = -1;
					vert = -1;
				}

				//if( (v1 != vert) && (v2 != vert) && (v3 != vert) ){
				//	//とりけし
				//	part = -1;
				//	vert = -1;
				//}
			}else{
				//とりけし
				part = -1;
				vert = -1;
			}
		}

	}

	if( (part < 0) || (vert < 0) ){
		*getnumptr = 0;//!!!!!!!!!!!!!!!!
	}

	return 0;
}



int CHandlerSet::ChkConfLineAndFace( D3DXVECTOR3 camdir, D3DXMATRIX matView, D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int needtrans, 
		int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr )
{
	int ret;

	D3DXMATRIX matProj;
	float fov;
	fov = (float)g_proj_fov * (float)DEG2PAI;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// この関数内での、matProjは、TLMODE_ORGのmatProjとは、異なるので、注意！！！！！！！！！
// Renderに渡すmatProjは、TLMODE_D3Dの時にしか、影響しない！！！！！！
// TLMODE_ORG時のmatProjは、Transform時に計算される！！！！！
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //float aspect;
	//aspect = (float)m_clientWidth / (float)m_clientHeight;//!!!!!!!!!!!!!!!!!!! ver1037
	//D3DXMatrixPerspectiveFovLH( &matProj, fov, aspect, (float)g_proj_near, (float)g_proj_far );
	

	ret = m_shandler->ChkConfLineAndFace( m_clientWidth, m_clientHeight, m_groundflag, m_mhandler, m_gpd.m_matWorld, matView,
		srcpos1, srcpos2, needtrans, partptr, faceptr, dstpos, dstn, revfaceptr );
	if( ret ){
		DbgOut( "handlerset : ChkConfLineAndFace : sh ChkConfLineAndFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( *partptr > 0 ){
		D3DXVec3Normalize( &camdir, &camdir );

		float dot;
		dot = D3DXVec3Dot( &camdir, dstn );

		if( dot > 0.0f ){
			*revfaceptr = 1;
		}else{
			*revfaceptr = 0;
		}

	}else{
		*revfaceptr = 0;
	}



	return 0;
}


int CHandlerSet::PickFace( D3DXMATRIX matView, float maxdist, int pos2x, int pos2y, int* partptr, int* faceptr, D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr, int calcmode )
{
	int ret;

	if( (pos2x < 0) || (pos2x > g_scsize.x) || (pos2y < 0) || (pos2y > g_scsize.y) ){
		*partptr = -1;
		return 0;
	}
	
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// この関数内での、matProjは、TLMODE_ORGのmatProjとは、異なるので、注意！！！！！！！！！
// Renderに渡すmatProjは、TLMODE_D3Dの時にしか、影響しない！！！！！！
// TLMODE_ORG時のmatProjは、Transform時に計算される！！！！！
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	

	ret = m_shandler->PickFace( maxdist, g_scsize.x, g_scsize.y, m_groundflag, m_mhandler, m_gpd.m_matWorld, matView, g_matProj, 0, 0, pos2x, pos2y, partptr, faceptr, pos3ptr, nptr, distptr, calcmode );
	if( ret ){
		DbgOut( "handlerset : PickFace : shandler PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::PickFace2( D3DXMATRIX matView, int* partarray, int partnum, float maxdist, int pos2x, int pos2y, int* partptr, int* faceptr, D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr, int calcmode )
{

	int ret;

	if( (pos2x < 0) || (pos2x > g_scsize.x) || (pos2y < 0) || (pos2y > g_scsize.y) ){
		*partptr = -1;
		return 0;
	}

	int pno;
	for( pno = 0; pno < partnum; pno++ ){
		int curp;
		curp = *( partarray + pno );
		if( (curp < 0) || (curp >= m_shandler->s2shd_leng) ){
			DbgOut( "handlerset : PickFace2 : partno error !!!\n" );
			_ASSERT( 0 );
			*partptr = -1;
			return 1;
		}
	}
	
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// この関数内での、matProjは、TLMODE_ORGのmatProjとは、異なるので、注意！！！！！！！！！
// Renderに渡すmatProjは、TLMODE_D3Dの時にしか、影響しない！！！！！！
// TLMODE_ORG時のmatProjは、Transform時に計算される！！！！！
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	ret = m_shandler->PickFace( maxdist, g_scsize.x, g_scsize.y, m_groundflag, m_mhandler, m_gpd.m_matWorld, matView, g_matProj, partarray, partnum, pos2x, pos2y, partptr, faceptr, pos3ptr, nptr, distptr, calcmode );
	if( ret ){
		DbgOut( "handlerset : PickFace : shandler PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



int CHandlerSet::GetCullingFlag( int partno, int vertno, int* viewcullptr, int* revcullptr )
{
	int ret;

	ret = m_shandler->GetCullingFlag( m_mhandler, partno, vertno, viewcullptr, revcullptr );
	if( ret ){
		DbgOut( "handlerset : GetCullingFlag : shandler GetCullingFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetOrgVertNo( int partno, int vertno, int* orgnoarray, int arrayleng, int* getnumptr )
{

	int ret;

	ret = m_shandler->GetOrgVertNo( partno, vertno, orgnoarray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "handlerset : GetOrgVertNo : shandler GetOrgVertNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::ChkIM2Status( int* statusptr )
{

	*statusptr = m_shandler->m_im2enableflag;

	return 0;
}


int CHandlerSet::LoadIM2File( char* fname )
{

	CIM2File im2file;
	int ret;
	ret = im2file.LoadIM2File( fname, m_thandler, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "handlerset : LoadIM2File : im2file LoadIM2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SaveIM2File( char* fname )
{

	CIM2File im2file;
	int ret;
	ret = im2file.WriteIM2File( fname, 0, m_thandler, m_shandler );
	if( ret ){
		DbgOut( "handlerset : LoadIM2File : im2file WriteIM2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetJointNum( int* numptr )
{

	*numptr = 0;

	int findcnt = 0;
	CShdElem* selem;
	int seri;

	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){

			findcnt++;
		}

	}

	*numptr = findcnt;

	return 0;
}

int CHandlerSet::GetMoaInfo( int maxnum, MOAINFO* moainfoptr, int* getnumptr )
{
	if( !m_mch ){
		_ASSERT( 0 );
		*getnumptr = 0;
		return 0;//!!!!!!!!!!
	}

	if( m_mhandler->s2mot_leng <= 1 ){
		_ASSERT( 0 );
		*getnumptr = 0;
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	CMotionCtrl* mcptr = (*m_mhandler)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}


///////
	if( m_mch->m_mcnum > maxnum ){
		DbgOut( "handlerset : GetMoaInfo : maxnum small error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*getnumptr = m_mch->m_mcnum;//!!!!!!!


	int mcno;
	for( mcno = 0; mcno < m_mch->m_mcnum; mcno++ ){
		int curid;
		curid = ( m_mch->m_mcarray + mcno )->id;
		(moainfoptr + mcno)->id = curid;


		char* nameptr;
		nameptr = *(miptr->motname + curid);
		
		int namelen;
		namelen = (int)strlen( nameptr );
		if( namelen >= 256 ){
			DbgOut( "handlerset : GetMoaInfo : namelen too large warning !!!\n" );
			_ASSERT( 0 );
			//strcpy( *(ppstr + setcnt), "エラー。名前が長すぎて格納できません。" );
			//code_setva( pvalstr, aptrstr + mcno, TYPE_STRING, "エラー。名前が長すぎて格納できません。" );
			strcpy_s( (moainfoptr + mcno)->name, 256, "エラー。名前が長すぎて格納できません。" );

		}else{
			//strcpy( *(ppstr + setcnt), telem->name );
			//code_setva( pvalstr, aptrstr + mcno, TYPE_STRING, nameptr );
			strcpy_s( (moainfoptr + mcno)->name, 256, nameptr );
		}
		
	}

	return 0;



}


int CHandlerSet::GetMoaInfo( int maxnum, char* dstnameptr, int* idptr, int* getnumptr )
{

	if( !m_mch ){
		_ASSERT( 0 );
		*getnumptr = 0;
		return 0;//!!!!!!!!!!
	}

	if( m_mhandler->s2mot_leng <= 1 ){
		_ASSERT( 0 );
		*getnumptr = 0;
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	CMotionCtrl* mcptr = (*m_mhandler)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}


///////
	if( m_mch->m_mcnum > maxnum ){
		DbgOut( "handlerset : GetMoaInfo : maxnum small error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*getnumptr = m_mch->m_mcnum;//!!!!!!!


	int mcno;
	for( mcno = 0; mcno < m_mch->m_mcnum; mcno++ ){
		int curid;
		curid = ( m_mch->m_mcarray + mcno )->id;
		*( idptr + mcno ) = curid; 


		char* nameptr;
		nameptr = *(miptr->motname + curid);
		
		int namelen;
		namelen = (int)strlen( nameptr );
		if( namelen >= 256 ){
			DbgOut( "handlerset : GetMoaInfo : namelen too large warning !!!\n" );
			_ASSERT( 0 );
			//strcpy( *(ppstr + setcnt), "エラー。名前が長すぎて格納できません。" );
			//code_setva( pvalstr, aptrstr + mcno, TYPE_STRING, "エラー。名前が長すぎて格納できません。" );
			strcpy_s( dstnameptr + mcno * 256, 256, "エラー。名前が長すぎて格納できません。" );
		}else{
			//strcpy( *(ppstr + setcnt), telem->name );
			//code_setva( pvalstr, aptrstr + mcno, TYPE_STRING, nameptr );
			strcpy_s( dstnameptr + mcno * 256, 256, nameptr );
		}
		
	}

	return 0;
}

int CHandlerSet::GetJointInfo( JOINTINFO* jiarray, int arrayleng, int* getnumptr )
{

	*getnumptr = 0;
	int setcnt = 0;
	CShdElem* selem;
	CTreeElem2* telem;
	int seri;

	int namelen;

	JOINTINFO* curji;

	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		telem = (*m_thandler)( seri );

		if( selem->IsJoint() && (selem->type != SHDMORPH) ){

			if( setcnt >= arrayleng ){
				DbgOut( "handlerset: GetJointInfo : arrayleng too short error %d !!!\n", arrayleng );
				_ASSERT( 0 );
				return 1;
			}

			curji = jiarray + setcnt;

			namelen = (int)strlen( telem->name );
			if( namelen > 256 ){
				DbgOut( "handlerset : GetJointInfo : namelen too large warning !!!\n" );
				_ASSERT( 0 );
				strcpy_s( curji->name, 256, "エラー。名前が長すぎて格納できません。" );
			}else{
				strcpy_s( curji->name, 256, telem->name );
			}

			curji->serialno = selem->serialno;

			curji->notuse = selem->notuse;


			if( selem->parent && selem->parent->IsJoint() ){//!!!!!!! joint以外の親は、記述しない。
				curji->parent = selem->parent->serialno;
			}else{
				curji->parent = -1;
			}

			if( selem->child ){
				curji->child = selem->child->serialno;
			}else{
				curji->child = -1;
			}

			if( selem->brother ){
				curji->brother = selem->brother->serialno;
			}else{
				curji->brother = -1;
			}

			if( selem->sister ){
				curji->sister = selem->sister->serialno;
			}else{
				curji->sister = -1;
			}

			setcnt++;
		}
	}

	*getnumptr = setcnt;

	return 0;
}



int CHandlerSet::GetJointInfo( int maxnum, char* nameptr, int* seriptr, int* getnumptr )
{

	*getnumptr = 0;
	int setcnt = 0;
	CShdElem* selem;
	CTreeElem2* telem;
	int seri;

	int namelen;

	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		telem = (*m_thandler)( seri );

		if( selem->IsJoint() ){

			if( setcnt >= maxnum ){
				DbgOut( "handlerset: GetJointInfo : maxnum too short error %d !!!\n", maxnum );
				_ASSERT( 0 );
				return 1;
			}

			namelen = (int)strlen( telem->name );
			if( namelen > 256 ){
				DbgOut( "handlerset : GetJointInfo : namelen too large warning !!!\n" );
				_ASSERT( 0 );
				//strcpy( *(ppstr + setcnt), "エラー。名前が長すぎて格納できません。" );
				//code_setva( pvalstr, aptrstr + setcnt, TYPE_STRING, "エラー。名前が長すぎて格納できません。" );
				strcpy_s( nameptr + setcnt * 256, 256, "エラー。名前が長すぎて格納できません。" );
			}else{
				//strcpy( *(ppstr + setcnt), telem->name );
				//code_setva( pvalstr, aptrstr + setcnt, TYPE_STRING, telem->name );
				strcpy_s( nameptr + setcnt * 256, 256, telem->name );
			}

			*(seriptr + JI_MAX * setcnt + JI_SERIAL) = selem->serialno;

			*(seriptr + JI_MAX * setcnt + JI_NOTUSE) = selem->notuse;


			if( selem->parent && selem->parent->IsJoint() ){//!!!!!!! joint以外の親は、記述しない。
				*(seriptr + JI_MAX * setcnt + JI_PARENT) = selem->parent->serialno;
			}else{
				*(seriptr + JI_MAX * setcnt + JI_PARENT) = -1;
			}

			if( selem->child ){
				*(seriptr + JI_MAX * setcnt + JI_CHILD) = selem->child->serialno;
			}else{
				*(seriptr + JI_MAX * setcnt + JI_CHILD) = -1;
			}

			if( selem->brother ){
				*(seriptr + JI_MAX * setcnt + JI_BROTHER) = selem->brother->serialno;
			}else{
				*(seriptr + JI_MAX * setcnt + JI_BROTHER) = -1;
			}

			if( selem->sister ){
				*(seriptr + JI_MAX * setcnt + JI_SISTER) = selem->sister->serialno;
			}else{
				*(seriptr + JI_MAX * setcnt + JI_SISTER) = -1;
			}

			//*(seriptr + JI_MAX * setcnt + JI_RESERVED1) = 0;
			//*(seriptr + JI_MAX * setcnt + JI_RESERVED2) = 0;
			//*(seriptr + JI_MAX * setcnt + JI_RESERVED3) = 0;

			///debug
			//DbgOut( "JointInfo : name %s, serial %d notuse %d, parent %d, child %d, brother %d, sister %d, reserved1 %d, reserved2 %d, reserved3 %d\n",
			//	strptr + 256 * setcnt,
			//	*(seriptr + JI_MAX * setcnt + JI_SERIAL),
			//	*(seriptr + JI_MAX * setcnt + JI_NOTUSE),
			//	*(seriptr + JI_MAX * setcnt + JI_PARENT),
			//	*(seriptr + JI_MAX * setcnt + JI_CHILD),
			//	*(seriptr + JI_MAX * setcnt + JI_BROTHER),
			//	*(seriptr + JI_MAX * setcnt + JI_SISTER),
			//	*(seriptr + JI_MAX * setcnt + JI_RESERVED1),
			//	*(seriptr + JI_MAX * setcnt + JI_RESERVED2),
			//	*(seriptr + JI_MAX * setcnt + JI_RESERVED3)
			//);

			setcnt++;
		}
	}

	*getnumptr = setcnt;

	return 0;
}

int CHandlerSet::GetFirstJointNo( int* noptr )
{

	*noptr = -1;

	int isfind = 0;
	CShdElem* selem;
	int seri;

	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		if( selem->IsJoint() ){

			isfind = 1;
			break;
		}
	}

	if( isfind == 1 ){
		*noptr = selem->serialno;

	}else{
		*noptr = -1;
	}


	return 0;
}

int CHandlerSet::GetDispObjNum( int* numptr )
{

	*numptr = 0;

	int findcnt = 0;
	CShdElem* selem;
	int seri;

	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

			findcnt++;
		}

	}

	*numptr = findcnt;

	return 0;
}

int CHandlerSet::GetDispObjInfo( DISPOBJINFO* doiarray, int arrayleng, int* getnumptr )
{

	*getnumptr = 0;
	int setcnt = 0;
	CShdElem* selem;
	CTreeElem2* telem;
	int seri;

	int namelen;

	DISPOBJINFO* curdoi;

	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		telem = (*m_thandler)( seri );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

			if( setcnt >= arrayleng ){
				DbgOut( "handlerset: GetDispObjInfo : arrayleng too short error %d !!!\n", arrayleng );
				_ASSERT( 0 );
				return 1;
			}

			curdoi = doiarray + setcnt;

			namelen = (int)strlen( telem->name );
			if( namelen > 256 ){
				DbgOut( "handlerset : GetDispObjInfo : namelen too large warning !!!\n" );
				_ASSERT( 0 );
				strcpy_s( curdoi->name, 256, "エラー。名前が長すぎて格納できません。" );
			}else{
				strcpy_s( curdoi->name, 256, telem->name );
			}

			curdoi->serialno = selem->serialno;
			curdoi->notuse = selem->notuse;
			curdoi->dispswitch = selem->dispswitchno;
			curdoi->invisible = selem->invisibleflag;

			setcnt++;
		}
	}

	*getnumptr = setcnt;

	return 0;


}



int CHandlerSet::GetDispObjInfo( int maxnum, char* nameptr, int* seriptr, int* getnumptr )
{

	*getnumptr = 0;
	int setcnt = 0;
	CShdElem* selem;
	CTreeElem2* telem;
	int seri;

	int namelen;

	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		selem = (*m_shandler)( seri );
		telem = (*m_thandler)( seri );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

			if( setcnt >= maxnum ){
				DbgOut( "handlerset: GetDispObjInfo : maxnum too short error %d !!!\n", maxnum );
				_ASSERT( 0 );
				return 1;
			}

			namelen = (int)strlen( telem->name );
			if( namelen > 256 ){
				DbgOut( "handlerset : GetDispObjInfo : namelen too large warning !!!\n" );
				_ASSERT( 0 );
				//strcpy( strptr + 256 * setcnt, "エラー。名前が長すぎて格納できません。" );
				//code_setva( pvalstr, aptrstr + setcnt, TYPE_STRING, "エラー。名前が長すぎて格納できません。" );
				strcpy_s( nameptr + setcnt * 256, 256, "エラー。名前が長すぎて格納できません。" );
			}else{
				//strcpy( strptr + 256 * setcnt, telem->name );
				//code_setva( pvalstr, aptrstr + setcnt, TYPE_STRING, telem->name );
				strcpy_s( nameptr + setcnt * 256, 256, telem->name );
			}

			*(seriptr + DOI_MAX * setcnt + DOI_SERIAL) = selem->serialno;

			*(seriptr + DOI_MAX * setcnt + DOI_NOTUSE) = selem->notuse;

			*(seriptr + DOI_MAX * setcnt + DOI_DISPSWITCH) = selem->dispswitchno;
			*(seriptr + DOI_MAX * setcnt + DOI_INVISIBLE) = selem->invisibleflag;

			///debug
			//DbgOut( "JointInfo : name %s, serial %d notuse %d, parent %d, child %d, brother %d, sister %d, reserved1 %d, reserved2 %d, reserved3 %d\n",
			//	strptr + 256 * setcnt,
			//	*(seriptr + JI_MAX * setcnt + JI_SERIAL),
			//	*(seriptr + JI_MAX * setcnt + JI_NOTUSE),
			//	*(seriptr + JI_MAX * setcnt + JI_PARENT),
			//	*(seriptr + JI_MAX * setcnt + JI_CHILD),
			//	*(seriptr + JI_MAX * setcnt + JI_BROTHER),
			//	*(seriptr + JI_MAX * setcnt + JI_SISTER),
			//	*(seriptr + JI_MAX * setcnt + JI_RESERVED1),
			//	*(seriptr + JI_MAX * setcnt + JI_RESERVED2),
			//	*(seriptr + JI_MAX * setcnt + JI_RESERVED3)
			//);

			setcnt++;
		}
	}

	*getnumptr = setcnt;

	return 0;
}

int CHandlerSet::SetIM2Params( int partno, int optvno, int childjoint, int parentjoint, int calcmode )
{
	CIM2File im2file;
	int ret;

	ret = im2file.SetIM2Params( m_shandler, partno, optvno, childjoint, parentjoint, calcmode );
	if( ret ){
		DbgOut( "handlerset : SetIM2Params : im2file SetIM2Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetIM2Params( int partno, int optvno, int* childjointptr, int* parentjointptr, int* calcmodeptr )
{
	/***
	CIM2File im2file;
	int ret;

	ret = im2file.GetIM2Params( m_shandler, partno, optvno, childjointptr, parentjointptr, calcmodeptr );
	if( ret ){
		DbgOut( "handlerset : GetIM2Params : im2file GetIM2Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/


	return 0;
}

int CHandlerSet::EnableTexture( int partno, int enableflag )
{
	int ret;

	ret = m_shandler->EnableTexture( partno, enableflag );
	if( ret ){
		DbgOut( "handlerset: EnableTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::JointAddToTree( int parentjoint, int childjoint, int lastflag )
{
	int isvalid;
	isvalid = m_shandler->IsValidJoint( parentjoint );
	if( isvalid == 0 ){
		DbgOut( "handlerset : JointAddToTree : parentjoint not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	isvalid = m_shandler->IsValidJoint( childjoint );
	if( isvalid == 0 ){
		DbgOut( "handlerset : JointAddToTree : childjoint not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* parelem;
	CShdElem* chilelem;

	parelem = (*m_shandler)( parentjoint );
	chilelem = (*m_shandler)( childjoint );


	//addする前にparentと切り離す。
	ret = chilelem->LeaveFromChain();
	if( ret ){
		DbgOut( "handlerset : JointAddToTree : selem LeaveFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = chilelem->PutToChain( parelem, lastflag );
	if( ret ){
		DbgOut( "handlerset : JointAddToTree : selem PutToChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::JointRemoveFromParent( int rmjoint )
{
	int isvalid;
	isvalid = m_shandler->IsValidJoint( rmjoint );
	if( isvalid == 0 ){
		DbgOut( "handlerset : JointRemoveFromparent : rmjoint not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* rmelem;
	rmelem = (*m_shandler)( rmjoint );
	_ASSERT( rmelem );

	ret = rmelem->LeaveFromChain();
	if( ret ){
		DbgOut( "handlerset : JointRemoveFromParent : selem LeaveFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::JointRemake()
{
	int ret;
	ret = m_shandler->CalcDepth();
	if( ret ){
		DbgOut( "handlerset : JointRemake : shandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_mhandler->CopyChainFromShd( m_shandler );
	if( ret ){
		DbgOut( "handlerset : JointRemake : mhandler CopyChainFromShd error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->CalcDepth();
	if( ret ){
		DbgOut( "handlerset : JointRemake : mhandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////
	//BoneInfoの作り直し。
	ret = m_shandler->DestroyBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : JointRemake : DestroyBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : JointRemake : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	ret = m_shandler->SetMikoAnchorApply( m_thandler );
	if( ret ){
		DbgOut( "handlerset : JointRemake : SetMikoAnchorApply error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//BoneElemの計算しなおし
	ret = m_shandler->CalcInfElem( -1, 1 );
	if( ret ){
		DbgOut( "handlerset : JointRemake : CalcInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetBBox( int partno, int mode, float* fminx, float* fmaxx, float* fminy, float* fmaxy, float* fminz, float* fmaxz )
{

	CBBox dstbb;
	
	int ret;

	ret = m_shandler->GetBBox( partno, mode, &dstbb );
	if( ret ){
		DbgOut( "handlerset : GetBBox : shandler GetBBox error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*fminx = dstbb.minx;
	*fmaxx = dstbb.maxx;

	*fminy = dstbb.miny;
	*fmaxy = dstbb.maxy;

	*fminz = dstbb.minz;
	*fmaxz = dstbb.maxz;


	return 0;
}

int CHandlerSet::GetVertNoOfFace( int partno, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr )
{
	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : GetVertNoOfFace : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_shandler->GetVertNoOfFace( partno, faceno, vert1ptr, vert2ptr, vert3ptr );
	if( ret ){
		DbgOut( "handlerset : GetVertNoOfFace : shandler GetVertNoOfFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetSamePosVert( int partno, int vertno, int* sameptr, int arrayleng, int* samenumptr )
{
	int ret;
	ret = m_shandler->GetSamePosVert( partno, vertno, sameptr, arrayleng, samenumptr );
	if( ret ){
		DbgOut( "handlerset : GetSamePosVert : shandler GetSamePosVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SaveQuaFile( int quatype, int mkid, char* filename )
{

	if( (mkid < 0) || (mkid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : SaveQuaFile : mkid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaFile* quafile;
	quafile = new CQuaFile();
	if( !quafile ){
		DbgOut( "handlerset : SaveQuaFile : quafile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int endframe;
	endframe = *(m_mhandler->m_motnum + mkid) - 1;
	ret = quafile->SaveQuaFile( quatype, m_thandler, m_shandler, m_mhandler, filename, mkid, 0, endframe );
	if( ret ){
		DbgOut( "handlerset : SaveMotion : SaveQuaFile error !!!\n" );
		_ASSERT( 0 );
		delete quafile;
		return 1;
	}

	delete quafile;

	return 0;
}


int CHandlerSet::SaveSig2Buf( char* buf, int bufsize, int* writesize )
{
	int ret;

	if( (bufsize > 0) && !buf ){
		DbgOut( "handlerset : SaveSig2Buf : buf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !writesize ){
		DbgOut( "handlerset : SaveSig2Buf : writesizeptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSigFile* sigfile;
	sigfile = new CSigFile();
	if( !sigfile ){
		DbgOut( "handlerset : SaveSig2Buf : sigfile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	ret = sigfile->WriteSigFile2Buf( buf, bufsize, writesize, m_thandler, m_shandler, m_mhandler );	
	if( ret ){
		DbgOut( "handlerset : SaveSig2Buf : sigfile WriteSigFile2Buf error !!!\n" );
		_ASSERT( 0 );
		delete sigfile;
		return 1;
	}

	delete sigfile;


	return 0;
}
int CHandlerSet::SaveQua2Buf( int quatype, int mkid, char* buf, int bufsize, int* writesize )
{
	int ret;

	if( (bufsize > 0) && !buf ){
		DbgOut( "handlerset : SaveQua2Buf : buf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !writesize ){
		DbgOut( "handlerset : SaveQua2Buf : writesizeptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (mkid < 0) || (mkid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : SaveQua2Buf : mkid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaFile* quafile;
	quafile = new CQuaFile();
	if( !quafile ){
		DbgOut( "handlerset : SaveQua2Buf : quafile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = quafile->SaveQuaFile2Buf( quatype, m_thandler, m_shandler, buf, bufsize, writesize, m_mhandler, mkid );
	if( ret ){
		DbgOut( "handlerset : SaveQua2Buf : SaveQuaFile2Buf error !!!\n" );
		_ASSERT( 0 );
		delete quafile;
		return 1;
	}

	delete quafile;

	return 0;
}


int CHandlerSet::IKRotateBeta( D3DXVECTOR3 camerapos, LPDIRECT3DDEVICE9 pdev, D3DXMATRIX matView, int jointno, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axisvec, int calclevel, D3DXVECTOR3 targetpos, D3DXVECTOR3* respos, int* lastparent )
{
	if( (jointno < 0) || (jointno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : IKRotateBeta : jointno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( jointno );
	if( selem->IsJoint() == 0 ){
		DbgOut( "handlerset : IKRotateBeta : this object is not joint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_mhandler->m_kindnum <= 0) || (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : IKRotateBeta : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || ( frameno >= *(m_mhandler->m_motnum + motid) ) ){
		DbgOut( "handlerset : IKRotateBeta : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;
	D3DXVECTOR3 curpos;
	ret = GetBonePos( jointno, 1, motid, frameno, 1, &curpos );
	if( ret ){
		DbgOut( "handlerset : IKRotateBeta : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 newtarget;
	if( calclevel != 0 ){
		newtarget = curpos + ( targetpos - curpos ) / (float)calclevel;
	}else{
		newtarget = targetpos;
	}

/////////

	ret = MoveBone( camerapos, pdev, jointno, newtarget, lastparent, matView, motid, frameno, iklevel, axiskind, axisvec, calclevel );
	if( ret ){
		DbgOut( "handlerset : IKRotateBeta : MoveBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetBonePos( jointno, 1, motid, frameno, 1, respos );
	if( ret ){
		DbgOut( "handlerset : IKRotateBeta : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CHandlerSet::MoveBone( D3DXVECTOR3 camerapos, LPDIRECT3DDEVICE9 pdev, int mvno, D3DXVECTOR3 targetobj, int* lastparent, D3DXMATRIX matView, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axisvec, int calclevel )
{
	if( !m_thandler || !m_shandler || !m_mhandler )
		return -1;
	int ret;	


	D3DXVECTOR3 mvobj, parobj;
	D3DXVECTOR3 parsdw;
	//D3DXVECTOR3 planeaxis;
	D3DXVECTOR3 rotaxis;

	D3DXVECTOR3 diffmvtarget;

	//D3DXMATRIX inimat;
	//D3DXMatrixIdentity( &inimat );


	float dist;
	float dist1, dist2;

	int movecntmax;
	int movecnt;

	CQuaternion saveq, saveq1st, saveq2nd;

	movecntmax = iklevel;

	CShdElem* mvbone = (*m_shandler)( mvno );
	if( !mvbone )
		return 0;

	CShdElem* parbone;
	int parboneno;

	parbone = m_shandler->FindUpperJoint( mvbone );
	if( !parbone )
		return 0;
	parboneno = parbone->serialno;

	CShdElem* curbone;
	curbone = (*m_shandler)( mvno );


	movecnt = 0;

	int ikboneno = parboneno;
	//int ikboneno = mvno;
	int curboneno = mvno;


	while( (movecnt < movecntmax) && parbone ){

		if( (movecnt != 0) && (curbone->m_notransik != 0) ){//!!!!!!!!!!!!!!!!!!!!!!!!!
			break;
		}

		*lastparent = parboneno;

		//ret = GetBonePos( mvno, m_gpd.m_matWorld, 1, &mvobj );
		ret = GetBonePos( mvno, 1, motid, frameno, 1, &mvobj );
		if( ret ){
			DbgOut( "MoveBone : GetBonePos error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}

		//ret = GetBonePos( parboneno, m_gpd.m_matWorld, 1, &parobj );
		ret = GetBonePos( parboneno, 1, motid, frameno, 1, &parobj );
		if( ret ){
			DbgOut( "MoveBone : GetBonePos error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}

		if( movecnt == 0 ){

			diffmvtarget = targetobj - mvobj;
			dist = D3DXVec3Length( &diffmvtarget );
			
//////////
			switch( axiskind ){
			case 0:
			case 1:
				rotaxis = axisvec;
				break;
			case 2:
			default:

				D3DXVECTOR3 par2mv, par2targ;
				par2mv = mvobj - parobj;
				DXVec3Normalize( &par2mv, &par2mv );
				par2targ = targetobj - parobj;
				DXVec3Normalize( &par2targ, &par2targ );

				D3DXVec3Cross( &rotaxis, &par2mv, &par2targ );
				DXVec3Normalize( &rotaxis, &rotaxis );

				break;

			}

		}


		if( (targetobj.x != mvobj.x) || (targetobj.y != mvobj.y) || (targetobj.z != mvobj.z) ){		
			

			D3DXVECTOR3 targetsdw;
			D3DXVECTOR3 parsdw2mv, parsdw2target;
			ret = GetShadow( &parsdw, rotaxis, mvobj, parobj, rotaxis );
			if( ret )
				return 0;

			ret = GetShadow( &targetsdw, rotaxis, mvobj, targetobj, rotaxis );
			if( ret )
				return 0;

			parsdw2mv = mvobj - parsdw;
			DXVec3Normalize( &parsdw2mv, &parsdw2mv );


			parsdw2target = targetsdw - parsdw;
			DXVec3Normalize( &parsdw2target, &parsdw2target );

			CQuaternion multq1, multq2;
			multq1.RotationArc( parsdw2mv, parsdw2target );
			multq1.normalize();

//////////////


			multq2 = -multq1;
			multq2.w = multq1.w;

			

			ret = GetQ( curboneno, motid, frameno, &saveq );


			//1回目
			ret = SetMotionPointIK( curboneno, multq1, motid, frameno );
			_ASSERT( !ret );


			//！！！boneのスクリーン座標を更新するために、FrameMove	が必要！！！
			SetBoneArray( parboneno );
			Transform( camerapos, pdev, matView, 1, 1 );
			//m_d3dapp->FrameMove( parboneno, 1 );


			//ret = GetBonePos( mvno, m_gpd.m_matWorld, 1, &mvobj );
			ret = GetBonePos( mvno, 1, motid, frameno, 1, &mvobj );
			if( ret ){
				DbgOut( "MoveBone : GetBonePos error !!!\n" );
				_ASSERT( 0 );
				return 0;
			}

			diffmvtarget = targetobj - mvobj;
			dist1 = D3DXVec3Length( &diffmvtarget );
			

			
			//２回目
			ret = SetQ( curboneno, motid, frameno, saveq );
			if( ret )
				return 0;
			ret = SetMotionPointIK( curboneno, multq2, motid, frameno );
			_ASSERT( !ret );
			//SetBoneArray( parboneno );
			//m_d3dapp->FrameMove( parboneno, 1 );
			Transform( camerapos, pdev, matView, 1, 1 );
						

			//ret = GetBonePos( mvno, m_gpd.m_matWorld, 1, &mvobj );
			ret = GetBonePos( mvno, 1, motid, frameno, 1, &mvobj );
			if( ret ){
				DbgOut( "MoveBone : GetBonePos error !!!\n" );
				_ASSERT( 0 );
				return 0;
			}


			diffmvtarget = targetobj - mvobj;
			dist2 = D3DXVec3Length( &diffmvtarget );



			if( dist1 < dist2 ){
				ret = SetQ( curboneno, motid, frameno, saveq );
				if( ret )
					return 0;
				ret = SetMotionPointIK( curboneno, multq1, motid, frameno );
				_ASSERT( !ret );
				//SetBoneArray( parboneno );
				//m_d3dapp->FrameMove( parboneno, 1 );
				Transform( camerapos, pdev, matView, 1, 1 );

			}else{
				//2回目の計算結果をそのまま、採用。することなし。
			}
			
		}

		//ikboneno = parboneno;

		goto movebonenext;

movebonenext:

		curboneno = parboneno;//!!!!!!!!
		curbone = parbone;

		parbone = m_shandler->FindUpperJoint( parbone );
		if( !parbone )
			break;
		parboneno = parbone->serialno;
		movecnt++;


		ikboneno = parboneno;//!!!!!!

	}


	return 0;
}


int CHandlerSet::SetBoneArray( int srcseri )
{
	//ZeroMemory( m_bonearray, sizeof(int) * (MAXBONENUM + 1) );
	
	int setnum = 0;
	CShdElem* selem;
	selem = (*m_shandler)( srcseri );

	SetBoneArrayReq( selem, &setnum, 0 );

	m_bonearray[ setnum ] = 0;
	setnum++;

	return 0;
}


void CHandlerSet::SetBoneArrayReq( CShdElem* selem, int* setnum, int broflag )
{
	if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( *setnum < MAXBONENUM ){
			m_bonearray[ *setnum ] = selem->serialno;
			(*setnum)++;
		}else{
			DbgOut( "CMyD3DApplication : SetBoneArray : setnum overflow !!!\n" );
			_ASSERT( 0 );
			return;
		}
	}

	if( broflag ){
		CShdElem* broelem;
		broelem = selem->brother;
		if( broelem ){
			SetBoneArrayReq( broelem, setnum, 1 );
		}
	}

	CShdElem* chilelem;
	chilelem = selem->child;
	if( chilelem ){
		SetBoneArrayReq( chilelem, setnum, 1 );
	}

}

int CHandlerSet::GetQ( int boneno, int motid, int frameno, CQuaternion* dstq )
{
	if( !m_thandler || !m_shandler || !m_mhandler )
		return 0;

	int ret;

	CMotionCtrl* mcptr = (*m_mhandler)( boneno );//注意！！引数は、m_selectednoではい。！！
	_ASSERT( mcptr );
		

	CMotionPoint2* curmp = mcptr->IsExistMotionPoint( motid, frameno );
	if( curmp ){
		*dstq = curmp->m_q;
	}else{

		CMotionCtrl* firstjoint = 0;
		CMotionCtrl* tmpmc;
		CShdElem* tmpse;
		int seri;
		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			tmpse = (*m_shandler)( seri );
			tmpmc = (*m_mhandler)( seri );
			if( tmpse->IsJoint() && (tmpse->type != SHDMORPH) && tmpse->parent && tmpse->parent->IsJoint() &&
				tmpmc->motinfo && tmpmc->motinfo->firstmp && *(tmpmc->motinfo->firstmp + motid ) ){
				
				firstjoint = (*m_mhandler)( tmpse->serialno );
				break;
			}
		}

		ret = CreateMotionPoints( boneno, firstjoint, motid, frameno );
		if( ret )
			return -1;
		curmp = mcptr->IsExistMotionPoint( motid, frameno );
		if( !curmp )
			return -1;

		*dstq = curmp->m_q;

	}
	
	return 0;
}

int CHandlerSet::SetQ( int boneno, int motid, int frameno, CQuaternion srcq, int curboneonly )
{
	if( !m_thandler || !m_shandler || !m_mhandler )
		return 0;
/////////////
	if( m_mhandler->m_kindnum <= 0 ){
		DbgOut( "handlerset : SetQ : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : SetQ : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "handlerset : SetQ : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_mhandler->m_motnum + motid)) ){
		DbgOut( "mothandler : SetQ : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////////
	int ret;
	CMotionCtrl* mcptr = (*m_mhandler)( boneno );//注意！！引数は、m_selectednoではい。！！
	_ASSERT( mcptr );
		
	CQuaternion q;
	float mvx, mvy, mvz;
	DWORD dispswitch;
	int interp;

	float scx, scy, scz;
	int userint1;

	CMotionPoint2* curmp = mcptr->IsExistMotionPoint( motid, frameno );
	if( curmp ){
		q = srcq;
		mvx = curmp->m_mvx;
		mvy = curmp->m_mvy;
		mvz = curmp->m_mvz;
		dispswitch = curmp->dispswitch;
		interp = curmp->interp;
		
		scx = curmp->m_scalex;
		scy = curmp->m_scaley;
		scz = curmp->m_scalez;
		userint1 = curmp->m_userint1;

		ret = curmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scx, scy, scz, userint1 );
		_ASSERT( !ret );
	}else{
		q = srcq;
		mvx = 0.0f;
		mvy = 0.0f;
		mvz = 0.0f;
		mcptr->GetDispSwitch( &dispswitch, motid, frameno );
		mcptr->GetInterpolation( &interp, motid, frameno );

		scx = 1.0f;
		scy = 1.0f;
		scz = 1.0f;
		userint1 = 0;

		CMotionPoint2* newmp = mcptr->AddMotionPoint( motid, frameno,
			q, mvx, mvy, mvz, dispswitch, interp, scx, scy, scz, userint1 );
		if( !newmp ){
			_ASSERT( 0 );
			DbgOut( "handlerset : SetQ : AddMotionPoint error" );
			return -1;
		}
	}

	//UpdateMatrix( parboneno );
	//UpdateAllMatrix();
	//SetCurrentMotion();
	
	if( curboneonly == 0 ){
		UpdateMatrixSelected( boneno, motid, frameno );
	}

	return 0;
}

int CHandlerSet::UpdateMatrixSelected( int srcseri, int motid, int frameno )
{
	if( !m_thandler || !m_shandler || !m_mhandler )
		return 0;

	int startframe, endframe;

	startframe = frameno;
	endframe = frameno;

	CShdElem* selem = (*m_shandler)( srcseri );

	int ret = 0;
	ret = m_mhandler->MakeRawmat( motid, selem, startframe, endframe, m_shandler );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int errorflag = 0;
	m_mhandler->RawmatToMotionmatReq( motid, m_shandler, srcseri, &errorflag, startframe, endframe, 0 );
	if( errorflag ){
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	int errorflag2 = 0;
	mcptr = (*m_mhandler)( srcseri );
	if( mcptr ){
		m_mhandler->SetCurrentMotionReq( m_shandler->m_mathead, mcptr, motid, frameno, 0, &errorflag2 );
		if( errorflag2 ){
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}


int CHandlerSet::SetMotionPointIK( int mvboneno, CQuaternion multq, int motcookie, int frameno )
{
	if( !m_thandler || !m_shandler || !m_mhandler )
		return 0;

	int ret;

	CQuaternion q;
	float mvx, mvy, mvz;
	DWORD dispswitch;
	int interp;

	float scx, scy, scz;
	int userint1;

	CMotionCtrl* mcptr = (*m_mhandler)( mvboneno );//注意！！引数は、m_selectednoではい。！！
	_ASSERT( mcptr );
		
	CShdElem* curselem;
	curselem = (*m_shandler)( mvboneno );
	_ASSERT( curselem );
	CShdElem* parselem;
	parselem = m_shandler->FindUpperJoint( curselem );

// worldmat
	CQuaternion worldq;
	D3DXMATRIX worldm;
	worldm = m_gpd.m_matWorld;
	worldm._41 = 0.0f;
	worldm._42 = 0.0f;
	worldm._43 = 0.0f;
	D3DXQUATERNION wqx;
	D3DXQuaternionRotationMatrix( &wqx, &worldm );
	worldq.x = wqx.x;
	worldq.y = wqx.y;
	worldq.z = wqx.z;
	worldq.w = wqx.w;


	CQuaternion parq( 1.0f, 0.0f, 0.0f, 0.0f );
	CQuaternion curq( 1.0f, 0.0f, 0.0f, 0.0f );
	CQuaternion invcurq( 1.0f, 0.0f, 0.0f, 0.0f );
	CQuaternion multq2;

	CMotionPoint2* curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
	if( curmp ){
		
		//m_q = invparq * multq;
		//m_q = multq * curmp->m_q;//後に掛ける
		//m_q = curmp->m_q * multq;//最初に掛ける		
		
		while( parselem ){
			CMotionCtrl* parmc;
			CMotionPoint2* parmp;
			parmc = (*m_mhandler)( parselem->serialno );
			
			parmp = parmc->IsExistMotionPoint( motcookie, frameno );
			//_ASSERT( parmp );
			if( parmp ){
				parq = parmp->m_q * parq;
				//parq = parq * parmp->m_q;//!!!!!!
			}
			parq.normalize();
			parselem = m_shandler->FindUpperJoint( parselem );
		}
		curq = worldq * parq * curmp->m_q;
		curq.normalize();
		curq.inv( &invcurq );
		invcurq.normalize();

		D3DXQUATERNION xmultq;
		multq.CQuaternion2X( &xmultq );

		D3DXVECTOR3 axis, multaxis;
		float rad;
		D3DXQuaternionToAxisAngle( &xmultq, &axis, &rad );
		DXVec3Normalize( &axis, &axis );
		invcurq.Rotate( &multaxis, axis );
		DXVec3Normalize( &multaxis, &multaxis );


		multq2.SetAxisAndRot( multaxis, rad );

		q = curmp->m_q * multq2;


		mvx = curmp->m_mvx;
		mvy = curmp->m_mvy;
		mvz = curmp->m_mvz;
		dispswitch = curmp->dispswitch;
		interp = curmp->interp;

		scx = curmp->m_scalex;
		scy = curmp->m_scaley;
		scz = curmp->m_scalez;
		userint1 = curmp->m_userint1;

		ret = curmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scx, scy, scz, userint1 );
		_ASSERT( !ret );
	}else{
		CMotionCtrl* firstjoint = 0;
		CMotionCtrl* tmpmc;
		CShdElem* tmpse;
		int seri;
		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			tmpse = (*m_shandler)( seri );
			tmpmc = (*m_mhandler)( seri );
			if( tmpse->IsJoint() && (tmpse->type != SHDMORPH) && tmpse->parent && tmpse->parent->IsJoint() &&
				tmpmc->motinfo && tmpmc->motinfo->firstmp && *(tmpmc->motinfo->firstmp + motcookie ) ){
				
				firstjoint = (*m_mhandler)( tmpse->serialno );
				break;
			}
		}

		ret = CreateMotionPoints( mcptr->serialno, firstjoint, motcookie, frameno );
		if( ret )
			return -1;

		curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
		if( !curmp )
			return -1;

		//m_q = invparq * multq;
		//m_q = multq * curmp->m_q;//後に掛ける
		//m_q = curmp->m_q * multq;//最初に掛ける

		while( parselem ){
			CMotionCtrl* parmc;
			CMotionPoint2* parmp;
			parmc = (*m_mhandler)( parselem->serialno );
			
			parmp = parmc->IsExistMotionPoint( motcookie, frameno );
			_ASSERT( parmp );

			parq = parmp->m_q * parq;
			//parq = parq * parmp->m_q;//!!!!!!!!!
			parq.normalize();
			parselem = m_shandler->FindUpperJoint( parselem );
		}
		curq = worldq * parq * curmp->m_q;
		curq.normalize();
		curq.inv( &invcurq );
		invcurq.normalize();

		D3DXQUATERNION xmultq;
		multq.CQuaternion2X( &xmultq );

		D3DXVECTOR3 axis, multaxis;
		float rad;
		D3DXQuaternionToAxisAngle( &xmultq, &axis, &rad );
		DXVec3Normalize( &axis, &axis );
		invcurq.Rotate( &multaxis, axis );
		DXVec3Normalize( &multaxis, &multaxis );

		multq2.SetAxisAndRot( multaxis, rad );


		q = curmp->m_q * multq2;


		mvx = curmp->m_mvx;
		mvy = curmp->m_mvy;
		mvz = curmp->m_mvz;
		dispswitch = curmp->dispswitch;
		interp = curmp->interp;

		scx = curmp->m_scalex;
		scy = curmp->m_scaley;
		scz = curmp->m_scalez;
		userint1 = curmp->m_userint1;
		
		/***
		m_mvx = 0.0f;
		m_mvy = 0.0f;
		m_mvz = 0.0f;
		***/
		ret = curmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scx, scy, scz, userint1 );
		_ASSERT( !ret );

	}


	//DbgOut( "handlerset : SetMotionPointIK finishflag %d : m_q %f %f %f %f\n",
	//	finishflag,
	//	m_q.w, m_q.x, m_q.y, m_q.z );

	//UpdateAllMatrix();
	UpdateMatrixSelected( mvboneno, motcookie, frameno );


	//if( needredraw )
	//	Redraw();

	return 0;
}

int CHandlerSet::SetMotionPointIK( int mvboneno, float srcmvx, float srcmvy, float srcmvz, int motcookie, int frameno )
{
	if( !m_thandler || !m_shandler || !m_mhandler )
		return 0;

	int ret;

	CQuaternion q;
	float mvx, mvy, mvz;
	DWORD dispswitch;
	int interp;
	float scalex, scaley, scalez;
	int userint1;


	CMotionCtrl* mcptr = (*m_mhandler)( mvboneno );//注意！！
	_ASSERT( mcptr );

	CMotionPoint2* curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
	if( curmp ){
		q = curmp->m_q;

//DbgOut( "handlerset : SetMotionPointIK : befmv %f %f %f\r\n", curmp->m_mvx, curmp->m_mvy, curmp->m_mvz ); 

		mvx = curmp->m_mvx + srcmvx;
		mvy = curmp->m_mvy + srcmvy;
		mvz = curmp->m_mvz + srcmvz;

		//m_mvx = srcmvx;
		//m_mvy = srcmvy;
		//m_mvz = srcmvz;

		dispswitch = curmp->dispswitch;
		interp = curmp->interp;

		scalex = curmp->m_scalex;
		scaley = curmp->m_scaley;
		scalez = curmp->m_scalez;
		userint1 = curmp->m_userint1;
		ret = curmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scalex, scaley, scalez, userint1 );
		_ASSERT( !ret );
	}else{
		CMotionCtrl* firstjoint = 0;
		CMotionCtrl* tmpmc;
		CShdElem* tmpse;
		int seri;
		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			tmpse = (*m_shandler)( seri );
			tmpmc = (*m_mhandler)( seri );
			if( tmpse->IsJoint() && (tmpse->type != SHDMORPH) && tmpse->parent && tmpse->parent->IsJoint() &&
				tmpmc->motinfo && tmpmc->motinfo->firstmp && *(tmpmc->motinfo->firstmp + motcookie ) ){
				
				firstjoint = (*m_mhandler)( tmpse->serialno );
				break;
			}
		}

		ret = CreateMotionPoints( mcptr->serialno, firstjoint, motcookie, frameno );
		if( ret )
			return -1;

		curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
		if( !curmp )
			return -1;

//DbgOut( "handlerset : SetMotionPointIK : befmv %f %f %f\r\n", curmp->m_mvx, curmp->m_mvy, curmp->m_mvz ); 


		q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		//m_mvx = 0.0f;
		//m_mvy = 0.0f;
		//m_mvz = 0.0f;
		q = curmp->m_q;
		mvx = curmp->m_mvx + srcmvx;
		mvy = curmp->m_mvy + srcmvy;
		mvz = curmp->m_mvz + srcmvz;

		//m_mvx = srcmvx;
		//m_mvy = srcmvy;
		//m_mvz = srcmvz;


		dispswitch = curmp->dispswitch;
		interp = curmp->interp;

		scalex = curmp->m_scalex;
		scaley = curmp->m_scaley;
		scalez = curmp->m_scalez;
		userint1 = curmp->m_userint1;
		ret = curmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scalex, scaley, scalez, userint1 );
		_ASSERT( !ret );

		
		//DbgOut( "handlerset : SetMotionPointIK : AddMotionPoint %d\n", m_current );
	}


	//DbgOut( "handlerset : SetMotionPointIK finishflag %d : m_q %f %f %f %f\n",
	//	finishflag,
	//	m_q.w, m_q.x, m_q.y, m_q.z );

	//UpdateAllMatrix();
	UpdateMatrixSelected( mvboneno, motcookie, frameno );


	return 0;
}



int CHandlerSet::GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir )
{

		//planepを通り、planedirを法線ベクトルとする平面：ax + by + cz + d = 0;
	float a, b, c, d;
	a = planedir.x; b = planedir.y; c = planedir.z;
	d = -a * planep.x - b * planep.y - c * planep.z;

		//平面ax+by+cz+d=0と、点pobj1を通り方向ベクトルdirectionの直線との交点、targetを求める。
		//OpenGL赤本p402参照
	D3DXMATRIX mat;
	mat._11 = b * srcdir.y + c * srcdir.z;
	mat._12 = -a * srcdir.y;
	mat._13 = -a * srcdir.z;
	mat._14 = 0.0f;

	mat._21 = -b * srcdir.x;
	mat._22 = a * srcdir.x + c * srcdir.z;
	mat._23 = -b * srcdir.z;
	mat._24 = 0.0f;

	mat._31 = -c * srcdir.x;
	mat._32 = -c * srcdir.y;
	mat._33 = a * srcdir.x + b * srcdir.y;
	mat._34 = 0.0f;

	mat._41 = -d * srcdir.x;
	mat._42 = -d * srcdir.y;
	mat._43 = -d * srcdir.z;
	mat._44 = a * srcdir.x + b * srcdir.y + c * srcdir.z;

	if( mat._44 == 0.0f )
		return 1;

	dstshadow->x = (srcp.x * mat._11 + srcp.y * mat._21 + srcp.z * mat._31 + mat._41) / mat._44;
	dstshadow->y = (srcp.x * mat._12 + srcp.y * mat._22 + srcp.z * mat._32 + mat._42) / mat._44;
	dstshadow->z = (srcp.x * mat._13 + srcp.y * mat._23 + srcp.z * mat._33 + mat._43) / mat._44;


	return 0;
}

/***
int CHandlerSet::CreateMotionPoints( int srcseri, int motcookie, int frameno )
{
	int ret;
	int elemno, framenum;
	CMotionCtrl* mcptr;
	CMotionPoint2* curmp;

	CQuaternion initq;
	initq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );


	CQuaternion q;
	float mvx, mvy, mvz;
	DWORD dispswitch = 0xFFFFFFFF;
	int interp = 0;

	float scx, scy, scz;
	int userint1;

	for( elemno = 1; elemno < m_mhandler->s2mot_leng; elemno++ ){
		mcptr = (*m_mhandler)( elemno );
		if( !mcptr )
			break;

		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH)){
			curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
			if( !curmp ){

				mcptr->GetDispSwitch( &dispswitch, motcookie, frameno );
				mcptr->GetInterpolation( &interp, motcookie, frameno );

				CMotionPoint2* newmp = mcptr->AddMotionPoint( motcookie, frameno,
					initq, 0.0f, 0.0f, 0.0f, dispswitch, interp, 1.0f, 1.0f, 1.0f, 0 );
				if( !newmp ){
					_ASSERT( 0 );
					DbgOut( "handlerset : CreateMotionPoints : AddMotionPoint error" );
					return 1;
				}
							
				
				ret = mcptr->PutQuaternionInOrder( motcookie );
				if( ret ){
					_ASSERT( 0 );
					DbgOut( "handlerset : CreateMotionPoints : PutQuaternionInOrder error" );
					return 1;
				}
				

				//前後のモーションポイントを補完した値を代入する。
				CMotionPoint2* prevmp;
				CMotionPoint2* nextmp;
				prevmp = newmp->prev;
				nextmp = newmp->next;

				if( prevmp ){
					if( nextmp ){
						framenum = nextmp->m_frameno - prevmp->m_frameno + 1;

						ret = newmp->FillUpMotionPoint( prevmp, nextmp, framenum, frameno - prevmp->m_frameno, prevmp->interp );
						if( ret ){
							_ASSERT( 0 );
							return 1;
						}

					}else{
						q = prevmp->m_q;
						mvx = prevmp->m_mvx;
						mvy = prevmp->m_mvy;
						mvz = prevmp->m_mvz;
						dispswitch = prevmp->dispswitch;
						interp = prevmp->interp;

						scx = prevmp->m_scalex;
						scy = prevmp->m_scaley;
						scz = prevmp->m_scalez;
						userint1 = prevmp->m_userint1;

						ret = newmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scx, scy, scz, userint1 );
						_ASSERT( !ret );
					}
				}
			}		
		}

	}

		
	return 0;
}
***/

int CHandlerSet::DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec )
{
	float mag;
	mag = srcvec->x * srcvec->x + srcvec->y * srcvec->y + srcvec->z * srcvec->z;

	float sqmag;
	sqmag = (float)sqrt( mag );

	if( sqmag != 0.0f ){
		float magdiv;
		magdiv = 1.0f / sqmag;
//		dstvec->x *= magdiv;
//		dstvec->y *= magdiv;
//		dstvec->z *= magdiv;
		dstvec->x = srcvec->x * magdiv;
		dstvec->y = srcvec->y * magdiv;
		dstvec->z = srcvec->z * magdiv;

	}else{
		DbgOut( "q2 : DXVec3Normalize : zero warning !!!\n" );
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
		_ASSERT( 0 );
	}

	return 0;
}


int CHandlerSet::GetMotionType( int motid, int* typeptr )
{
	int ret;

	ret = m_mhandler->GetMotionType( motid, typeptr );
	if( ret ){
		DbgOut( "handlerset : mh GetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMotionType( int motid, int type )
{
	int ret;

	ret = m_mhandler->SetMotionType( motid, type );
	if( ret ){
		DbgOut( "handlerset : mh SetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::GetIKTransFlag( int jointno, int* flagptr )
{
	int ret;
	ret = m_shandler->GetIKTransFlag( jointno, flagptr );
	if( ret ){
		DbgOut( "handlerset : GetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
	}
	return 0;
}
int CHandlerSet::SetIKTransFlag( int jointno, int flag )
{
	int ret;
	ret = m_shandler->SetIKTransFlag( jointno, flag );
	if( ret ){
		DbgOut( "handlerset : SetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
	}

	return 0;
}

int CHandlerSet::DestroyAllMotion()
{

	EnterCriticalSection( &m_crit_addmotion );//######## start crit

	if( m_mch ){
		delete m_mch;
		m_mch = 0;
	}

	int ret;
	ret = m_mhandler->DestroyMotionObj();
	if( ret ){
		DbgOut( "handlerset : mh DestroyMotionObj error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &m_crit_addmotion );//###### end crit
		return 1;
	}

	LeaveCriticalSection( &m_crit_addmotion );//###### end crit

	return 0;
}

int CHandlerSet::GetUserInt1OfPart( int partno, int* userint1ptr )
{
	int ret;

	ret = m_shandler->GetUserInt1OfPart( partno, userint1ptr );
	if( ret ){
		DbgOut( "handlerset : sh GetUserInt1OfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetUserInt1OfPart( int partno, int userint1 )
{
	int ret;

	ret = m_shandler->SetUserInt1OfPart( partno, userint1 );
	if( ret ){
		DbgOut( "handlerset : sh SetUserInt1OfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetBSphere( int partno, D3DXVECTOR3* dstcenter, float* dstr )
{
	int ret;
	if( partno < 0 ){
		*dstcenter = m_mhandler->m_curbs.tracenter;
		*dstr = m_mhandler->m_curbs.rmag;
	}else{
		ret = m_shandler->GetBSphere( partno, dstcenter, dstr );
		if( ret ){
			DbgOut( "handlerset : shd GetBSphere error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;

}

int CHandlerSet::GetChildJoint( int parentno, int arrayleng, int* childarray, int* childnum )
{
	int ret;

	ret = m_shandler->GetChildJoint( parentno, arrayleng, childarray, childnum );
	if( ret ){
		DbgOut( "handlerset : sh GetChildJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::DestroyMotionFrame( int motid, int frameno )
{

	EnterCriticalSection( &m_crit_addmotion );//######## start crit

	int ret;

	ret = m_mhandler->DestroyMotionFrame( m_shandler, motid, frameno );
	if( ret ){
		DbgOut( "handlerset : DestroyMotionFrame : mh DestroyMotionFrame error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &m_crit_addmotion );//###### end crit
		return 1;
	}

	LeaveCriticalSection( &m_crit_addmotion );//###### end crit

	return 0;
}


int CHandlerSet::HuGetKeyFrameNo( int motid, int boneno, int* framearray, int arrayleng, int* framenumptr )
{
	int ret;
	ret = m_mhandler->HuGetKeyframeNo( motid, boneno, framearray, arrayleng, framenumptr );
	if( ret ){
		DbgOut( "handlerset : HuGetKeyframeNo : mh HuGetKeyframeNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::SetUVBatchUV( int partno, int* vertnoptr, int setnum, UV* uvptr, int setflag, int clampflag )
{
	int ret;
	ret = m_shandler->SetUVBatchUV( partno, vertnoptr, setnum, uvptr, setflag, clampflag );
	if( ret ){
		DbgOut( "hs : sh SetUVBatchUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}


int CHandlerSet::SetUV( int partno, int vertno, float fu, float fv, int setflag, int clampflag )
{

	int ret;

	ret = m_shandler->SetUV( partno, vertno, fu, fv, setflag, clampflag );
	if( ret ){
		DbgOut( "handlerset : sh SetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetUVTile( int partno, int texrule, int unum, int vnum, int tileno )
{
	int ret;

	ret = m_shandler->SetUVTile( partno, texrule, unum, vnum, tileno );
	if( ret ){
		DbgOut( "handlerset : sh SetUVTile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;

}


int CHandlerSet::GetUV( int partno, int vertno, float* uptr, float* vptr )
{

	int ret;

	ret = m_shandler->GetUV( partno, vertno, uptr, vptr );
	if( ret ){
		DbgOut( "handlerset : sh GetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int CHandlerSet::PickBone( D3DXMATRIX matView, int posx, int posy, int* bonenoptr )
{
	*bonenoptr = -1;
	int pickno;

	DWORD dwClipWidth = g_scsize.x / 2;
	DWORD dwClipHeight = g_scsize.y / 2;
	float aspect;
	aspect = (float)dwClipWidth / (float)dwClipHeight;

	pickno = m_shandler->PickBone( m_mhandler, posx, posy, dwClipWidth, dwClipHeight, m_gpd.m_matWorld, matView, g_matProj, aspect );


	*bonenoptr = pickno;


	return 0;
}

int CHandlerSet::ShiftBoneTreeDiff( int jointno, D3DXVECTOR3 mvdiff, int motcookie, int frameno )
{

	int ret;
	int topjointno = 0;
	ret = GetTopJointNo( jointno, &topjointno );
	if( ret ){
		DbgOut( "handlerset : ShiftBoneTreeDiff : GetTopJointNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( topjointno <= 0 )
		return 0;

	ret = SetMotionPointIK( topjointno, mvdiff.x, mvdiff.y, mvdiff.z, motcookie, frameno );
	if( ret ){
		DbgOut( "handlerset : ShiftBoneTreeDiff : SetMotionPointIK error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::GetTopJointNo( int srcseri, int* topseri )
{
	if( (srcseri <= 0) || (srcseri >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : GetTopJointNo : srcseri error !!!\n" );
		_ASSERT( 0 );
		*topseri = 0;
		return 1;
	}

	CShdElem* srcelem;
	srcelem = (*m_shandler)( srcseri );

	CShdElem* curselem;
	CShdElem* upperelem = 0;
	CShdElem* befupperelem = 0;

	curselem = srcelem;
	upperelem = srcelem;
	befupperelem = srcelem;
	while( curselem ){

		befupperelem = upperelem;
		upperelem = m_shandler->FindUpperJoint( curselem, 0 );

		curselem = upperelem;
	}

	if( befupperelem ){
		*topseri = befupperelem->serialno;
	}else{
		//なし
		*topseri = 0;
	}

	return 0;
}


int CHandlerSet::SetTextureDirtyFlag( int srcseri )
{
	int ret;

	if( srcseri >= 0 ){
		ret = m_shandler->SetTextureDirtyFlag( srcseri );
		if( ret ){
			DbgOut( "handlerset : SetTextureDirtyFlag : sh SetTextureDirtyFlag error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( srcseri == -999999 ){

		ret = m_shandler->SetTextureDirtyFlag( -1 );
		if( ret ){
			DbgOut( "handlerset: SetTextureDirtyFlag : sh SetTextureDirtyFlag -1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{

		//srcseriが、負のときは、srcseri以外に、dirtyflagを立てる。

		int chkseri;
		for( chkseri = 0; chkseri < m_shandler->s2shd_leng; chkseri++ ){
			if( chkseri != srcseri ){
				ret = m_shandler->SetTextureDirtyFlag( chkseri );
				if( ret ){
					DbgOut( "handlerset: SetTextureDirtyFlag : sh SetTextureDirtyFlag 2 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;
}


int CHandlerSet::GetTextureFromDispObj( int partno, int* texidptr )
{
	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : GetTextureFromDispObj : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : GetTextureFromDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* selem = (*m_shandler)( partno );
	_ASSERT( selem );

	if( m_billboardflag ){

		if( selem->texname && (*(selem->texname) != 0) ){
			int texindex;
			texindex = g_texbnk->FindName( selem->texname, selem->transparent );

			if( texindex >= 0 ){
				*texidptr = g_texbnk->GetSerial( texindex );
			}else{
				*texidptr = -1;
			}
		}else{
			*texidptr = -1;
		}
	}else{
		int matno = -3;
		ret = selem->GetFirstMaterialNo( &matno );
		_ASSERT( !ret );

		CMQOMaterial* curmat;
		curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
		if( curmat ){
			if( curmat->tex[0] != 0 ){
				int texindex;
				texindex = g_texbnk->FindName( curmat->tex, curmat->transparent );

				if( texindex >= 0 ){
					*texidptr = g_texbnk->GetSerial( texindex );
				}else{
					*texidptr = -1;
				}
			}else{
				*texidptr = -1;
			}

		}else{
			*texidptr = -1;
		}

	}
	return 0;
}

int CHandlerSet::SetTextureToDispObj( int partno, int texid )
{
	if( (m_billboardflag != 2) && ( (partno < 0) || (partno >= m_shandler->s2shd_leng) ) ){
		DbgOut( "handlerset : SetTextureToDispObj : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : SetTextureToDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;



	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "handlerset : SetTextureToDispObj : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "handlerset : SetTextureToDispObj : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_billboardflag == 1 ){
		CShdElem* selem = (*m_shandler)( partno );
		_ASSERT( selem );


		ret = selem->SetTexName( texname );
		if( ret ){
			DbgOut( "handlerset : SetTextureToDispObj : selem SetTexName error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		selem->transparent = transparent;
	}else if( m_billboardflag == 2 ){
		int seri;
		CShdElem* selem;
		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			selem = (*m_shandler)( seri );
			if( selem->type == SHDPOLYMESH ){
				ret = selem->SetTexName( texname );
				if( ret ){
					DbgOut( "handlerset : SetTextureToDispObj : selem SetTexName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				selem->transparent = transparent;
			}
		}
	}else{
		CShdElem* selem = (*m_shandler)( partno );
		_ASSERT( selem );

		int matno = -3;
		ret = selem->GetFirstMaterialNo( &matno );
		_ASSERT( !ret );

		CMQOMaterial* curmat;
		curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
		if( curmat ){
			strcpy_s( curmat->tex, 256, texname );
			curmat->transparent = transparent;
			ret = m_shandler->ChkAlphaNum();
			_ASSERT( !ret );
		}
	}
	return 0;
}

int CHandlerSet::SaveGNDFile( char* filename )
{
	int ret;

	if( !filename || !*filename ){
		DbgOut( "handlerset : SaveGndFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CGndFile* gndfile;
	gndfile = new CGndFile();
	if( !gndfile ){
		DbgOut( "handlerset : SaveGndFile : gndfile alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	ret = gndfile->WriteGndFile( filename, m_thandler, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "handlerset : SaveGndFile : sigfile WriteGndFile error !!!\n" );
		_ASSERT( 0 );
		delete gndfile;
		return 1;
	}

	delete gndfile;

	return 0;

}


int CHandlerSet::LoadGNDFile( char* fname, int adjustuvflag, float srcmult )
{
	int ret = 0;
		
	CGndFile* gndfile;
	gndfile = new CGndFile();
	if( !gndfile ){
		_ASSERT( 0 );
		return 1;
	}

	ret = gndfile->LoadGndFile( fname, m_thandler, m_shandler, m_mhandler, 0, srcmult, adjustuvflag );
	if( ret ){
		DbgOut( "handlerset : LoadGndFile : gndfile LoadGndFile error !!!\n" );
		_ASSERT( 0 );
		delete gndfile;
		return 1;
	}
	delete gndfile;

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : LoadGndFile : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	return 0;

}
int CHandlerSet::LoadGNDFileFromBuf( char* bufptr, int bufsize, int adjustuvflag )
{
	int ret = 0;
		
	CGndFile* gndfile;
	gndfile = new CGndFile();
	if( !gndfile ){
		_ASSERT( 0 );
		return 1;
	}

	ret = gndfile->LoadGndFileFromBuf( bufptr, bufsize, m_thandler, m_shandler, m_mhandler, 0, 1.0f, adjustuvflag );
	if( ret ){
		DbgOut( "handlerset : LoadGndFileFromBuf : gndfile LoadGndFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		delete gndfile;
		return 1;
	}
	delete gndfile;
	
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "handlerset : LoadGndFileFromBuf : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	
	return 0;

}



int CHandlerSet::TwistBone( int motid, int frameno, int twistno, float twistrad )
{
	if( (twistno < 0) || (twistno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : TwistBone : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( twistno );
	if( selem->IsJoint() == 0 ){
		DbgOut( "handlerset : TwistBone : this object is not joint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_mhandler->m_kindnum <= 0) || (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : TwistBone : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || ( frameno >= *(m_mhandler->m_motnum + motid) ) ){
		DbgOut( "handlerset : TwistBone : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	if( !m_thandler || !m_shandler || !m_mhandler )
		return 0;

	int ret;
	int parno;
	CShdElem* twistelem;
	CShdElem* parelem;

	CQuaternion q;
	float mvx, mvy, mvz;
	DWORD dispswitch;
	int interp;
	float scalex, scaley, scalez;
	int userint1;


	twistelem = (*m_shandler)( twistno );
	if( !twistelem )
		return 0;

	parelem = m_shandler->FindUpperJoint( twistelem );
	if( !parelem )
		return 0;
	parno = parelem->serialno;

	D3DXVECTOR3 direction;
	float twistx, twisty, twistz;
	float parx, pary, parz;


	//bonematがかかる前の座標を元に計算し、ｑは、左から掛ける

	ret = twistelem->GetBoneOrgPos( &twistx, &twisty, &twistz );
	if( ret )
		return 0;
	ret = parelem->GetBoneOrgPos( &parx, &pary, &parz );
	if( ret )
		return 0;
	direction.x = twistx - parx;
	direction.y = twisty - pary;
	direction.z = twistz - parz;
	DXVec3Normalize( &direction, &direction );



	//CMotionCtrl* mcptr = (*m_mhandler)( parno );
CMotionCtrl* mcptr = (*m_mhandler)( twistno );
	_ASSERT( mcptr );
	
	CQuaternion multq;
	ret = multq.SetAxisAndRot( direction, twistrad );
	_ASSERT( !ret );

	CMotionPoint2* curmp = mcptr->IsExistMotionPoint( motid, frameno );
	if( curmp ){
		q = curmp->m_q;
		//curq = multq * m_q;
		q = q * multq;//一番最初に、multqが作用するように。
		
		mvx = curmp->m_mvx;
		mvy = curmp->m_mvy;
		mvz = curmp->m_mvz;
		dispswitch = curmp->dispswitch;
		interp = curmp->interp;

		scalex = curmp->m_scalex;
		scaley = curmp->m_scaley;
		scalez = curmp->m_scalez;
		userint1 = curmp->m_userint1;
		ret = curmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scalex, scaley, scalez, userint1 );
		_ASSERT( !ret );
	}else{
		CMotionCtrl* firstjoint = 0;
		CMotionCtrl* tmpmc;
		CShdElem* tmpse;
		int seri;
		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			tmpse = (*m_shandler)( seri );
			tmpmc = (*m_mhandler)( seri );
			if( tmpse->IsJoint() && (tmpse->type != SHDMORPH) && tmpse->parent && tmpse->parent->IsJoint() &&
				tmpmc->motinfo && tmpmc->motinfo->firstmp && *(tmpmc->motinfo->firstmp + motid ) ){
				
				firstjoint = (*m_mhandler)( tmpse->serialno );
				break;
			}
		}

		ret = CreateMotionPoints( mcptr->serialno, firstjoint, motid, frameno );
		if( ret )
			return -1;

		curmp = mcptr->IsExistMotionPoint( motid, frameno );
		if( !curmp )
			return -1;

		/***
		m_q = multq;
		m_mvx = 0.0f;
		m_mvy = 0.0f;
		m_mvz = 0.0f;
		m_dispswitch = curmp->dispswitch;
		m_interp = curmp->interp;

		m_scalex = 1.0f;
		m_scaley = 1.0f;
		m_scalez = 1.0f;
		m_userint1 = 0;
		***/

		q = curmp->m_q * multq;

		mvx = curmp->m_mvx;
		mvy = curmp->m_mvy;
		mvz = curmp->m_mvz;
		dispswitch = curmp->dispswitch;
		interp = curmp->interp;

		scalex = curmp->m_scalex;
		scaley = curmp->m_scaley;
		scalez = curmp->m_scalez;
		userint1 = curmp->m_userint1;
		ret = curmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scalex, scaley, scalez, userint1 );
		_ASSERT( !ret );

	}

	//UpdateAllMatrix();
	//UpdateMatrixSelected( parno, 0 );
	//UpdateMatrixSelected( twistno, 0 );

	UpdateMatrixSelected( twistno, motid, frameno );
	
	return 0;
}

int CHandlerSet::IKTranslate( int shiftno, int motid, int frameno, D3DXVECTOR3 srcpos )
{
	if( !m_thandler || !m_shandler || !m_mhandler )
		return 0;

	
	if( (shiftno < 0) || (shiftno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : IKTranslate : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( shiftno );
	if( selem->IsJoint() == 0 ){
		DbgOut( "handlerset : IKTranslate : this object is not joint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_mhandler->m_kindnum <= 0) || (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : IKTranslate : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || ( frameno >= *(m_mhandler->m_motnum + motid) ) ){
		DbgOut( "handlerset : IKTranslate : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;

	D3DXVECTOR3 target;
	D3DXMATRIX invw;
	D3DXMatrixInverse( &invw, NULL, &m_gpd.m_matWorld );

	D3DXVec3TransformCoord( &target, &srcpos, &invw );

	/////////////


	D3DXVECTOR3 orgobj;

	//ret = GetBonePos( shiftno, 0, motid, frameno, 1, &orgobj );
	//if( ret ){
	//	DbgOut( "handlerset : ShiftBone vec : GetBonePos error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}


	D3DXMATRIX inimat;
	D3DXMatrixIdentity( &inimat );
	// ローカル座標で計算するので、matworldは、inimat
	ret = m_shandler->GetBonePos( shiftno, 1, motid, frameno, &orgobj, m_mhandler, inimat, 1 );
	if( ret ){
		DbgOut( "handlerset : IKTranslate : GetBonePos : shandler GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	D3DXMATRIX mat1, raw1, invraw1, par1, mat2;
	float rate1, rate2;
	int boneno1, boneno2;


	int infnum = 0;
	ret = m_shandler->GetInfElemNum( shiftno, 0, &infnum );
	if( ret || (infnum < 1) ){
		DbgOut( "handlerset : ShiftBone vec : sh GetInfElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->GetInfElemParam( shiftno, 0, 0, &boneno1, &rate1 );
	if( ret ){
		DbgOut( "handlerset : ShiftBone vec : sh GetInfElemParam error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( infnum >= 2 ){
		ret = m_shandler->GetInfElemParam( shiftno, 0, 1, &boneno2, &rate2 );
		if( ret ){
			DbgOut( "handlerset : ShiftBone vec : sh GetInfElemParam error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		boneno2 = -1;
		rate2 = 0.0f;
	}


	if( boneno1 > 0 ){
		ret = m_mhandler->HuGetBoneMatrix( boneno1, motid, frameno, &mat1 );
		if( ret ){
			DbgOut( "handlerset : ShiftBone vec : HuGetBoneMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = m_mhandler->HuGetRawMatrix( boneno1, motid, frameno, &raw1 );
		if( ret ){
			DbgOut( "handlerset : ShiftBone vec : HuGetRawMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		D3DXMatrixIdentity( &mat1 );
		D3DXMatrixIdentity( &raw1 );
	}
	if( boneno2 > 0 ){
		ret = m_mhandler->HuGetBoneMatrix( boneno2, motid, frameno, &mat2 );
		if( ret ){
			DbgOut( "handlerset : ShiftBone vec : HuGetBoneMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		D3DXMatrixIdentity( &mat2 );
	}


	D3DXMatrixInverse( &invraw1, NULL, &raw1 );

	//par1 = invraw1 * mat1;

	CShdElem* shiftelem;
	shiftelem = (*m_shandler)( shiftno );

	CShdElem* setelem;
	setelem = m_shandler->FindUpperJoint( shiftelem );
	if( !setelem ){
		setelem = shiftelem;//!!!!!!!!!!!!!!
	}


	/***
	CShdElem* parelem;
	int parno;
	parelem = m_shandler->FindUpperJoint( shiftelem );
	if( parelem ){
		parno = parelem->serialno;
		ret = m_mhandler->HuGetBoneMatrix( parno, &par1 );
		if( ret ){
			DbgOut( "handlerset : ShiftBone vec : HuGetBoneMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		D3DXMatrixIdentity( &par1 );
	}
	***/
	CShdElem* bone1;
	bone1 = (*m_shandler)( boneno1 );
	_ASSERT( bone1 );

	CShdElem* parelem;
	int parno;
	parelem = m_shandler->FindUpperJoint( bone1 );
	if( parelem ){
		parno = parelem->serialno;
		ret = m_mhandler->HuGetBoneMatrix( parno, motid, frameno, &par1 );
		if( ret ){
			DbgOut( "handlerset : ShiftBone vec : HuGetBoneMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		D3DXMatrixIdentity( &par1 );
	}


///////
	// org * raw1 * mvm * par1 = ( target - mat2 * org * rate2 ) / rate1

	D3DXVECTOR3 pos2;
	D3DXVec3TransformCoord( &pos2, &orgobj, &mat2 );

	float alpha, beta, ganma;
	alpha = ( target.x - pos2.x * rate2 ) / rate1;
	beta = ( target.y - pos2.y * rate2 ) / rate1;
	ganma = ( target.z - pos2.z * rate2 ) / rate1;

////////
	float a11, a12, a13, a21, a22, a23, a31, a32, a33, a41, a42, a43;
	a11 = raw1._11;
	a12 = raw1._12;
	a13 = raw1._13;

	a21 = raw1._21;
	a22 = raw1._22;
	a23 = raw1._23;

	a31 = raw1._31;
	a32 = raw1._32;
	a33 = raw1._33;

	a41 = raw1._41;
	a42 = raw1._42;
	a43 = raw1._43;

	float b11, b12, b13, b21, b22, b23, b31, b32, b33, b41, b42, b43;
	b11 = par1._11;
	b12 = par1._12;
	b13 = par1._13;

	b21 = par1._21;
	b22 = par1._22;
	b23 = par1._23;

	b31 = par1._31;
	b32 = par1._32;
	b33 = par1._33;

	b41 = par1._41;
	b42 = par1._42;
	b43 = par1._43;

	float c11, c12, c13, c21, c22, c23, c31, c32, c33, c41, c42, c43;
	c11 = a11*b11 + a12*b21 + a13*b31;
	c12 = a11*b12 + a12*b22 + a13*b32;
	c13 = a11*b13 + a12*b23 + a13*b33;

	c21 = a21*b11 + a22*b21 + a23*b31;
	c22 = a21*b12 + a22*b22 + a23*b32;
	c23 = a21*b13 + a22*b23 + a23*b33;

	c31 = a31*b11 + a32*b21 + a33*b31;
	c32 = a31*b12 + a32*b22 + a33*b32;
	c33 = a31*b13 + a32*b23 + a33*b33;

	c41 = b11*a41 + b21*a42 + b31*a43 + b41;
	c42 = b12*a41 + b22*a42 + b32*a43 + b42;
	c43 = b13*a41 + b23*a42 + b33*a43 + b43;

	//zero div check
	if( b33 == 0.0f )
		return 0;
	float B33 = 1.0f / b33;

	float p, q, r, s, t, v, w;
	float x, y, z;

	x = orgobj.x;
	y = orgobj.y;
	z = orgobj.z;


	//zero div check
	if( (b11 - b31 * B33 * b13) == 0.0f )
		return 0;

	p = B33 * ( ganma - c13 * x - c23 * y - c33 * z - c43 );
	q = 1.0f / (b11 - b31 * B33 * b13);
	//r = q * (alpha - c11 * x - c21 * y - c31 * z - c41 - b31 * p - b21);
	r = q * (alpha - c11 * x - c21 * y - c31 * z - c41 - b31 * p);

	s = b12 - b32 * B33 * b13;
	t = beta - c12 * x - c22 * y - c32 * z - c42 - b32 * p;

	v = b21 - b31 * B33 * b23;
	w = b22 - b32 * B33 * b23;


	////zero div check
	//if( (s * q * b31 * B33 * b23 + b22 - b32 * B33 * b23) == 0.0f )
	//	return 0;
	//u = 1.0f / (s * q * b31 * B33 * b23 + b22 - b32 * B33 * b23);


	float mvx, mvy, mvz;

	//mvy = u * ( t - s * r );
	mvy = ( t -  s * r ) / ( w - s * q * v );

	//mvx = q * b31 * B33 * b23 * u * ( t - s * r ) + r;
	mvx = -q * ( b21 - b31 * B33 * b23 ) * mvy + r;

	mvz = p - B33 * b13 * mvx - B33 * b23 * mvy;


//DbgOut( "handlerset : ShiftBone : mv %f %f %f, orgobj %f %f %f\r\n\r\n",
//	   mvx, mvy, mvz,
//	   orgobj.x, orgobj.y, orgobj.z );


	//ret = SetMotionPointIK( shiftno, mvx, mvy, mvz );
	//ret = SetMotionPointIK( setelem->serialno, mvx, mvy, mvz );
	ret = SetMotionPointIK( bone1->serialno, mvx, mvy, mvz, motid, frameno );
	_ASSERT( !ret );


	return 0;
}

int CHandlerSet::GetShaderType( int* shaderptr, int* overflowptr )
{
	*shaderptr = m_shandler->m_shader;
	*overflowptr = m_shandler->m_overflow;

	return 0;
}
int CHandlerSet::SetShaderType( LPDIRECT3DDEVICE9 pdev, int shader, int overflow )
{
	int ret;

	if( shader == COL_TOON0 ){
		shader = COL_TOON1;//!!!!!!!!!!!!
	}

	if( (shader >= COL_OLD) && (shader < COL_MAX) ){
		m_shandler->m_shader = shader;

		if( (shader != COL_TOON1) ){
			ret = m_shandler->SetIndexBufColP();
			if( ret ){
				DbgOut( "handlerset : SetShaderType : sh SetIndexBufColP error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( (overflow >= OVERFLOW_CLAMP) && (overflow < OVERFLOW_MAX) ){
		m_shandler->m_overflow = overflow;
	}

//////////

	if( shader == COL_TOON1 ){
		int ret;
		ret = m_shandler->CreateToon1Buffer( pdev );
		if( ret ){
			DbgOut( "handlerset : SetShaderType : COL_TOON1 : sh CreateToon1Buffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CHandlerSet::SetDispEmissive( int partno, int r, int g, int b, int setflag, int vertno )
{
	int ret;

	ret = m_shandler->SetDispEmissive( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "handlerset : SetEmissive : shandler SetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetDispSpecularPower( int partno, float pow, int setflag, int vertno )
{
	int ret;

	ret = m_shandler->SetDispSpecularPower( partno, pow, setflag, vertno );
	if( ret ){
		DbgOut( "handlerset : SetSpecularPower : shandler SetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetEmissive( int partno, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;

	ret = m_shandler->GetEmissive( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "handlerset : GetEmissive : shandler GetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetSpecularPower( int partno, int vertno, float* powptr )
{
	int ret;

	ret = m_shandler->GetSpecularPower( partno, vertno, powptr );
	if( ret ){
		DbgOut( "handlerset : GetSpecularPower : shandler GetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetInfElemNum( int partno, int vertno, int* numptr )
{

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : GetInfElemNum : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = m_shandler->GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "handlerset : GetInfElemNum : sh GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "handlerset : GetInfElemNum : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->GetInfElemNum( partno, vertno, numptr );
	if( ret ){
		DbgOut( "handlerset : GetInfElemNum : sh GetInfElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;	

}

int CHandlerSet::GetInfElem( int partno, int vertno, int infno, int* childjointptr, int* calcmodeptr, float* userrate, float* orginf, float* dispinf )
{
	INFELEM* pIE = 0;
	int ret;

	ret = m_shandler->GetInfElem( partno, vertno, infno, &pIE );
	if( ret ){
		DbgOut( "handlerset : GetInfElem : sh GetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !pIE ){
		DbgOut( "handlerset : GetInfElem : pIE NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*childjointptr = pIE->childno;
	*calcmodeptr = pIE->kind;
	*userrate = pIE->userrate;
	*orginf = pIE->orginf;
	*dispinf = pIE->dispinf;
		
	return 0;
}

int CHandlerSet::SetInfElem( int partno, int vertno, int childjointno, int calcmode, int paintmode, int normalizeflag, float fuserrate, float fdirectval )
{
	int ret;

	ret = m_shandler->SetInfElem( partno, vertno, childjointno, paintmode, calcmode, fuserrate, normalizeflag, fdirectval );
	if( ret ){
		DbgOut( "handlerset : SetInfElem : sh SetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::DeleteInfElem( int partno, int vertno, int childjointno, int normalizeflag )
{
	int ret;
	ret = m_shandler->DeleteInfElem( partno, vertno, childjointno, normalizeflag );
	if( ret ){
		DbgOut( "handlerset : DeleteInfElem : sh DeleteInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::SetInfElemDefault( int partno, int vertno )
{
	int ret;
	
	if( partno < 0 ){
		ret = m_shandler->CalcInfElem( -1, 1 );
		if( ret ){
			DbgOut( "handlerset : SetInfElemDefault : sh CalcInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = m_shandler->CalcInfElem1Vert( partno, vertno );
		if( ret ){
			DbgOut( "handlerset : SetInfElemDefault : sh CalcInfElem1Vert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CHandlerSet::NormalizeInfElem( int partno, int vertno )
{
	int ret;

	if( vertno < 0 ){
		ret = m_shandler->NormalizeInfElem( partno, -1 );
		if( ret ){
			DbgOut( "handlerset : NormalizeInfElem : sh NormalizeInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = m_shandler->NormalizeInfElem( partno, vertno );
		if( ret ){
			DbgOut( "handlerset : NormalizeInfElem : sh NormalizeInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CHandlerSet::GetVisiblePolygonNum( int* num1ptr, int* num2ptr )
{
	int ret;

	ret = m_shandler->GetVisiblePolygonNum( m_mhandler, num1ptr, num2ptr );
	if( ret ){
		DbgOut( "handlerset : GetVisiblePolygonNum : sh GetVisiblePolygonNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetNextMotionFrameNo( int mk, int nextmk, int nextframeno, int befframeno )
{
	int ret;

	CMotionCtrl* curmc;	
	CMotionCtrl* firstjoint = 0;
	int elemno;
	for( elemno = 1; elemno < m_mhandler->s2mot_leng; elemno++ ){
		curmc = (*m_mhandler)( elemno );

		if( curmc->IsJoint() && (curmc->type != SHDMORPH) ){
			firstjoint = curmc;
			break;
		}
	}

	if( !firstjoint ){
		_ASSERT( 0 );
		return 0;
	}

	CMotionInfo* minfo;
	minfo = firstjoint->motinfo;

	if( !minfo ){
		_ASSERT( 0 );
		return 0;
	}

	if( (mk < 0) || (mk >= minfo->kindnum) ){
		DbgOut( "HandlerSet : SetNextMotionFrameNo : mk out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (nextmk < 0) || (nextmk >= minfo->kindnum) ){
		DbgOut( "HandlerSet : SetNextMotionFrameNo : nextmk out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int maxframe;
	maxframe = *(minfo->motnum + nextmk) - 1;
	if( (nextframeno < 0) || (nextframeno > maxframe) ){
		DbgOut( "HandlerSet : SetNextMotionFrameNo : nextframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->SetNextMotionFrameNo( m_shandler, mk, nextmk, nextframeno, befframeno );
	if( ret ){
		DbgOut( "HandlerSet : SetNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetToon0Params( int partno, float darkrate, float brightrate, float darkcos, float brightcos )
{

	if( partno < 0 ){

		int elemno;
		for( elemno = 0; elemno < m_shandler->s2shd_leng; elemno++ ){
			float* paramptr;
			paramptr = (*m_shandler)( elemno )->m_toonparams;

			*paramptr = darkrate;
			*(paramptr + 1) = brightrate;
			*(paramptr + 2) = darkcos;
			*(paramptr + 3) = brightcos;
		}

	}else{

		if( partno >= m_shandler->s2shd_leng ){
			DbgOut( "handlerset : SetToon0Params : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float* paramptr;
		paramptr = (*m_shandler)( partno )->m_toonparams;

		*paramptr = darkrate;
		*(paramptr + 1) = brightrate;
		*(paramptr + 2) = darkcos;
		*(paramptr + 3) = brightcos;

	}

	return 0;
}

int CHandlerSet::CreateMotionPoints( int srcseri, CMotionCtrl* firstjoint, int motcookie, int frameno )
{
	if( m_mhandler->s2mot_leng < 1 ){
		return 0;//!!!!!!!!
	}

	int ret;
	int elemno, framenum;
	CMotionCtrl* mcptr;
	CMotionPoint2* curmp;
	CShdElem* selem;

	CQuaternion initq;
	initq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	DWORD dispswitch;
	int interp;
	CQuaternion q;
	float mvx, mvy, mvz;
	float scalex, scaley, scalez;
	int userint1;
	CQuaternion axisq;


	if( firstjoint != 0 ){
		firstjoint->GetDispSwitch( &dispswitch, motcookie, frameno );
		firstjoint->GetInterpolation( &interp, motcookie, frameno );
	}else{
		dispswitch = 0xFFFFFFFF;
		interp = INTERPOLATION_SLERP;
	}


//DbgOut( "motparamdlg : CreateMotionPoints : frameno %d, firstjoint seri %d, m_dispswitch %d\r\n",
//	frameno, firstjoint->serialno, m_dispswitch );

	if( srcseri < 0 ){
		for( elemno = 1; elemno < m_mhandler->s2mot_leng; elemno++ ){
			mcptr = (*m_mhandler)( elemno );
			if( !mcptr )
				break;

			if( mcptr->IsJoint() && (mcptr->type != SHDMORPH)){

				selem = (*m_shandler)( elemno );
				_ASSERT( selem );
				selem->GetBoneAxisQ( &axisq );

				curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
				if( !curmp ){
					CMotionPoint2 tmpmp;
					int hasmpflag;
					ret = mcptr->CalcMotionPointOnFrame( selem, &tmpmp, motcookie, frameno, &hasmpflag, m_shandler, m_mhandler );
					if( ret ){
						_ASSERT( 0 );
						return -1;
					}

					CMotionPoint2* newmp = mcptr->AddMotionPoint( motcookie, frameno, 
						tmpmp.m_q, tmpmp.m_mvx, tmpmp.m_mvy, tmpmp.m_mvz,
						dispswitch, interp, tmpmp.m_scalex, tmpmp.m_scaley, tmpmp.m_scalez, 0, tmpmp.m_eul );
					if( !newmp ){
						_ASSERT( 0 );
						return -1;
					}


/***
					CMotionPoint2* newmp = mcptr->AddMotionPoint( motcookie, frameno,
						initq, 0.0f, 0.0f, 0.0f, dispswitch, interp, 1.0f, 1.0f, 1.0f, 0 );
					if( !newmp ){
						_ASSERT( 0 );
						return -1;
					}
								
					
					ret = mcptr->PutQuaternionInOrder( motcookie );
					if( ret ){
						_ASSERT( 0 );
						return -1;
					}
					

					//前後のモーションポイントを補完した値を代入する。
					CMotionPoint2* prevmp;
					CMotionPoint2* nextmp;
					prevmp = newmp->prev;
					nextmp = newmp->next;

					if( prevmp ){
						if( nextmp ){
							framenum = nextmp->m_frameno - prevmp->m_frameno + 1;

							ret = newmp->FillUpMotionPoint( &axisq, prevmp, nextmp, framenum, frameno - prevmp->m_frameno, prevmp->interp );
							if( ret ){
								_ASSERT( 0 );
								return 1;
							}
						}else{
							q = prevmp->m_q;
							mvx = prevmp->m_mvx;
							mvy = prevmp->m_mvy;
							mvz = prevmp->m_mvz;
							dispswitch = prevmp->dispswitch;
							interp = prevmp->interp;

							scalex = prevmp->m_scalex;
							scaley = prevmp->m_scaley;
							scalez = prevmp->m_scalez;
							userint1 = prevmp->m_userint1;
							ret = newmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scalex, scaley, scalez, userint1 );
							_ASSERT( !ret );
						}
					}
***/
				}		
			}

		}
	}else{
		mcptr = (*m_mhandler)( srcseri );
		if( !mcptr ){
			_ASSERT( 0 );
			return 1;
		}

		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH)){
			selem = (*m_shandler)( srcseri );
			_ASSERT( selem );
			selem->GetBoneAxisQ( &axisq );

			curmp = mcptr->IsExistMotionPoint( motcookie, frameno );
			if( !curmp ){

//				mcptr->GetDispSwitch( &m_dispswitch, m_mot_cookie, frameno );
//				mcptr->GetInterpolation( &m_interp, m_mot_cookie, frameno );

				CMotionPoint2* newmp = mcptr->AddMotionPoint( motcookie, frameno,
					initq, 0.0f, 0.0f, 0.0f, dispswitch, interp, 1.0f, 1.0f, 1.0f, 0 );
				if( !newmp ){
					_ASSERT( 0 );
					return -1;
				}
							
				
				ret = mcptr->PutQuaternionInOrder( motcookie );
				if( ret ){
					_ASSERT( 0 );
					return -1;
				}
				

				//前後のモーションポイントを補完した値を代入する。
				CMotionPoint2* prevmp;
				CMotionPoint2* nextmp;
				prevmp = newmp->prev;
				nextmp = newmp->next;

				if( prevmp ){
					if( nextmp ){
						framenum = nextmp->m_frameno - prevmp->m_frameno + 1;

						ret = newmp->FillUpMotionPoint( &axisq, prevmp, nextmp, framenum, frameno - prevmp->m_frameno, prevmp->interp );
						if( ret ){
							_ASSERT( 0 );
							return 1;
						}
					}else{
						q = prevmp->m_q;
						mvx = prevmp->m_mvx;
						mvy = prevmp->m_mvy;
						mvz = prevmp->m_mvz;
						dispswitch = prevmp->dispswitch;
						interp = prevmp->interp;

						scalex = prevmp->m_scalex;
						scaley = prevmp->m_scaley;
						scalez = prevmp->m_scalez;
						userint1 = prevmp->m_userint1;
						ret = newmp->SetParams( frameno, q, mvx, mvy, mvz, dispswitch, interp, scalex, scaley, scalez, userint1 );
						_ASSERT( !ret );
					}
				}
			}		
		}


	}
		
	return 0;
}


int CHandlerSet::LoadMAFile( char* srcname, int fillupleng, HWND srchwnd, HMODULE hMod, float mvmult, CPanda* panda, int pndid )
{

	int ret;


/// destroy
	if( m_mch ){
		delete m_mch;
		m_mch = 0;
	}

	ret = m_mhandler->DestroyMotionObj();
	if( ret ){
		DbgOut( "handlerset : LoadMAFile : DestroyMotionObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///

	m_mch = new CMCHandler( m_mhandler, fillupleng );
	if( !m_mch ){
		DbgOut( "handlerset : LoadMAFile : mch alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//	m_mhandler->m_fuleng = m_mch->m_fillupleng;


/// motion for fillup
	HRSRC hRs;
	hRs = FindResource( hMod, MAKEINTRESOURCE(IDR_RCDATA3), RT_RCDATA );//これで通らないと使えない
	if( hRs == NULL ){
		DbgOut( "handlerset : LoadMAFile : FindResource error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	DWORD dwResSize;
	dwResSize = SizeofResource( hMod, hRs );

	HGLOBAL hGl;
	hGl = LoadResource( hMod, hRs );
	if( hGl == NULL ){
		DbgOut( "handlerset : LoadMAFile : LoadResource error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* lpdata;
	lpdata = (char*)LockResource( hGl );
	if( !lpdata ){
		DbgOut( "handlerset : LoadMAFile : LockResource error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int fucookie = -1;
	int fumaxframe = 0;
	ret = LoadQuaFileFromBuf( lpdata, (int)dwResSize, &fucookie, &fumaxframe, 1.0f );
	if( ret ){
		DbgOut( "handlerset : LoadMAFile : LadQuaFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( fucookie != FILLUPMOTIONID ){
		DbgOut( "handlerset: LoadMAFile : fucookie error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	ret = SetMotionFrameLength( fucookie, fillupleng, 1 );
//	if( ret ){
//		DbgOut( "handlerset : LoadMAFile : SetMotionFrameLength error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


	ret = m_mch->AddParentMC( fucookie, 0, 0, 0, 0, 0, 0 );
	if( ret ){
		DbgOut( "handlerset : LoadMAFile : mch AddParentMC error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/// trunk motion
	CMAFile mafile;
	if( !panda ){
		ret = mafile.LoadMAFile( srcname, srchwnd, this, mvmult );
		if( ret ){
			DbgOut( "handlerset : LoadMAFile : mafile LoadMAFile error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = mafile.LoadMAFileFromPnd( panda, pndid, srcname, srchwnd, this, mvmult );
		if( ret ){
			DbgOut( "handlerset : LoadMAFile : mafile LoadMAFileFromPnd error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( fillupleng < 0 ){
		m_mhandler->m_fuleng = mafile.m_fuleng;
		m_mch->m_fillupleng = mafile.m_fuleng;
		ret = SetMotionFrameLength( fucookie, mafile.m_fuleng, 1 );
		if( ret ){
			DbgOut( "handlerset : LoadMAFile : SetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		m_mhandler->m_fuleng = fillupleng;
		m_mch->m_fillupleng = fillupleng;
		ret = SetMotionFrameLength( fucookie, fillupleng, 1 );
		if( ret ){
			DbgOut( "handlerset : LoadMAFile : SetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

///

	ret = m_mch->SetIdlingMotion( m_shandler, 1 );
	if( ret ){
		DbgOut( "handlerset : LoadMAFile : mch SetIdlingMotionToNext error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	return 0;
}

int CHandlerSet::LoadMAFMotion( char* srcfilename, int srcidling, int ev0idle, int commonid, int forbidnum, int* forbidid, 
		int* cookieptr, float mvmult, int srcnotfu, CPanda* panda, int pndid )
{
	int ret;

	_ASSERT( m_mch );

	int framemax;

	ret = LoadQuaFile( srcfilename, cookieptr, &framemax, mvmult, panda, pndid );
	if( ret ){
		DbgOut( "handlerset : LoadMAFMotion : LoadQuaFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->CalcMPEuler( m_shandler, *cookieptr, m_mhandler->GetZaType( *cookieptr ) );
	if( ret ){
		DbgOut( "handlerset : LoadMAFMotion : mh CalcMPEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = m_mch->AddParentMC( *cookieptr, srcidling, ev0idle, commonid, forbidnum, forbidid, srcnotfu );
	if( ret ){
		DbgOut( "handlerset : LoadMAFMotion : mch AddParentMC error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::AddChildMC( int parentcookie, MCELEM childmc )
{

	int ret;
	int always;
	if( childmc.frameno1 >= 0 ){
		always = 0;
	}else{
		always = 1;
	}

	_ASSERT( m_mch );

	int parentsetno;
	parentsetno = m_mch->MotID2Setno( parentcookie );
	if( parentsetno < 0 ){
		DbgOut( "handlerset : AddChildMC : parentcookie error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mch->AddChild( parentsetno, childmc.id, always, childmc.frameno1, childmc.frameno2, childmc.eventno1, childmc.notfu, childmc.nottoidle );
	if( ret ){
		DbgOut( "handlerset : AddChildMC : mch AddChlid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetMotionFrameNoML( int boneno, int* dstmotkind, int* dstmotframe )
{

	*dstmotkind = -1;
	*dstmotframe = 0;
	int ret;
	ret = m_mhandler->GetMotionFrameNoML( boneno, dstmotkind, dstmotframe );
	if( ret || (*dstmotkind < 0) ){
		DbgOut( "handlerset : GetMotionFrameNoML : mh GetMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;


}


int CHandlerSet::GetMotionFrameNo( int* dstmotkind, int* dstmotframe )
{
	*dstmotkind = -1;
	*dstmotframe = 0;
	int ret;
	ret = m_mhandler->GetMotionFrameNo( dstmotkind, dstmotframe );
	if( ret || (*dstmotkind < 0) ){
		DbgOut( "handlerset : GetMotionFrameNo : mh GetMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CHandlerSet::GetNextMotionFrameNoML( int boneno, int motid, int* nextid, int* nextframe )
{
	int ret;
	ret = m_mhandler->GetNextMotionFrameNoML( boneno, motid, nextid, nextframe );
	if( ret ){
		DbgOut( "handlerset : GetNextMotionFrameNoML : mh GetNextMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::GetNextMotionFrameNo( int motid, int* nextid, int* nextframe )
{
	int ret;
	ret = m_mhandler->GetNextMotionFrameNo( motid, nextid, nextframe );
	if( ret ){
		DbgOut( "handlerset : GetNextMotionFrameNo : mh GetNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetNewPoseByMOAML( int eventno, int* listptr, int* notlistptr )
{

	int ret;

	ret = SetMLList( listptr, notlistptr );
	if( ret ){
		DbgOut( "hs : SetNewPoseByMOAML : SetMLList error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int idlingid;
	int idlingleng;
	int idlingtype;
	idlingid = m_mch->GetIdlingMotID();
	if( idlingid < 0 ){
		DbgOut( "hs : SetNewPoseByMOAML : idlingid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->GetMotionFrameLength( idlingid, &idlingleng );
	if( ret ){
		DbgOut( "handlerset : SetNewPoseByMOAML : mh GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->GetMotionType( idlingid, &idlingtype );
	if( ret ){
		DbgOut( "handlerset : SetNewPoseByMOAML : GetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int firstid;
	int firstframe;
	int syncroframe;
	int nextflag = 1;
	ret = m_mhandler->GetFirstTickJoint( nextflag, idlingid, &firstid, &firstframe );
	if( ret ){
		DbgOut( "hs : SetNewPoseByMOAML : mh GetFirstTickJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( firstid > 0 ){
		syncroframe = firstframe + m_mch->m_fillupleng - 1;
		if( syncroframe >= idlingleng ){
			switch( idlingtype ){
			case MOTION_CLAMP:
				syncroframe = idlingleng - 1;
				break;
			case MOTION_ROUND:
				while( syncroframe >= idlingleng ){
					syncroframe -= idlingleng;
				}
				break;
			case MOTION_INV:
				syncroframe = idlingleng - 1 - (syncroframe - idlingleng);
				syncroframe = max( 0, syncroframe );
				syncroframe = min( idlingleng, syncroframe );
				break;
			default:
				syncroframe = idlingleng - 1;
				break;
			}
		}
	}else{
		syncroframe = 0;
	}


	DWORD curtick;
	curtick = GetTickCount();


	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*m_shandler)( seri );
		_ASSERT( selem );

		if( *( m_mllist + seri ) == 1 ){

			int curmotid = -1;
			int curframe = 0;
			int curframeleng = 0;

			ret = m_mhandler->GetMotionFrameNoML( seri, &curmotid, &curframe );
			if( ret || (curmotid < 0) ){
				DbgOut( "handlerset : SetNewPoseByMOAML : mh GetMotionFrameNoML error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = m_mhandler->GetMotionFrameLength( curmotid, &curframeleng );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOAML : mh GetMotionFrameLength error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int curmottype = -1;
			ret = m_mhandler->GetMotionType( curmotid, &curmottype );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOAML : GetMotionType error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


		//DbgOut( "check !!! : hs : SetNewPoseByMOA : curmotid %d, curframe %d, FILLUPMOTIONID %d\r\n",
		//	curmotid, curframe, FILLUPMOTIONID );

			int checkfillup = 0;


			int nextmotid = -1;
			int nextframe = 0;
			int nextframeleng = 0;

			int befmotid = -1;
			int befframe = 0;

			int fillupflag = 0;
			int notfu = 0;
			int ev0idle = 0;
			int nottoidle = 0;

			if( curmotid != FILLUPMOTIONID ){
				int tmpnottoidle = 0;
				ret = m_mch->GetNextMotion( curmotid, curframe, eventno, &nextmotid, &nextframe, &notfu, &tmpnottoidle );
				if( ret ){
					DbgOut( "handlerset : SetNewPoseByMOAML : mch GetNextMotion error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( tmpnottoidle < 0 ){
					m_mhandler->GetNotToIdle( seri, &nottoidle );
				}else{
					nottoidle = tmpnottoidle;
					m_mhandler->SetNotToIdle( seri, nottoidle );
				}


				ret = m_mch->GetEv0Idle( curmotid, &ev0idle );
				if( ret ){
					DbgOut( "handlerset : SetNewPoseByMOAML : mch GetEv0Idle 0 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				if( (eventno == 0) && (ev0idle != 0) ){
					nextmotid = idlingid;
					nextframe = 0;
					notfu = 0;
				}else{
					if( curmotid == nextmotid ){
						//最終フレームだった場合は、アイドリングへ
						if( (curmottype == MOTION_CLAMP) && (curframe >= (curframeleng - 1)) && (nottoidle == 0) ){
							nextmotid = idlingid;
							nextframe = 0;
							notfu = 0;
						}
					}
				}

		//DbgOut( "check !!! : hs : SetNewPoseByMOA : curmotid != FILLUPMOTIONID, nextmotid %d, nextframe %d\r\n",
		//	   nextmotid, nextframe );

			}else{
				//補間モーション中だった場合。
				checkfillup = 1;

				ret = m_mhandler->GetNextMotionFrameNoML( seri, FILLUPMOTIONID, &befmotid, &befframe );
				if( ret || (befmotid < 0) ){
					DbgOut( "handlerset : SetNewPoseByMOAML : mh GetNextMotionFrameNo error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				int tmpframe;
				tmpframe = max( 0, (befframe - m_mch->m_fillupleng) );
				int tmpnottoidle = 0;
				ret = m_mch->GetNextMotion( befmotid, tmpframe + curframe, eventno, &nextmotid, &nextframe, &notfu, &tmpnottoidle );
				//ret = m_mch->GetNextMotion( befmotid, befframe + curframe, eventno, &nextmotid, &nextframe, &notfu );
				if( ret ){
					DbgOut( "handlerset : SetNewPoseByMOAML : mch GetNextMotion error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
		//DbgOut( "check !!! : hs : SetNewPoseByMOA : curmotid == FILLUPMOTIONID, befmotid %d, befframe %d, nextmotid %d, nextframe %d\r\n",
		//	   befmotid, befframe, nextmotid, nextframe );

				if( tmpnottoidle < 0 ){
					m_mhandler->GetNotToIdle( seri, &nottoidle );
				}else{
					nottoidle = tmpnottoidle;
					m_mhandler->SetNotToIdle( seri, nottoidle );
				}
				
				ret = m_mch->GetEv0Idle( befmotid, &ev0idle );
				if( ret ){
					DbgOut( "handlerset : SetNewPoseByMOAML : mch GetEv0Idle 2 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				if( (eventno == 0) && (ev0idle != 0) ){
					nextmotid = idlingid;
					nextframe = 0;
					notfu = 0;
				}
				
				if( befmotid != nextmotid ){
					fillupflag = 1;
				}

			}


			if( ((checkfillup == 0) && (curmotid != nextmotid)) || fillupflag ){

//DbgOut( "hs : SetNewPoseByMOAML : fillup calc !!!\n" );

				if( notfu == 0 ){

					ret = m_mhandler->GetMotionFrameLength( nextmotid, &nextframeleng );
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh GetMotionFrameLength error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int filluppoint;
					if( nextmotid != idlingid ){
						filluppoint = min( (nextframeleng - 1), (nextframe + m_mch->m_fillupleng - 1) );
					}else{
						filluppoint = syncroframe;
					}
					ret = m_mhandler->CopyMotionFrameML( m_shandler, seri, curmotid, curframe, FILLUPMOTIONID, 0 );
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh CopyMotionFrameML fu0 error %d, %d!!!\n", curmotid, curframe );
						_ASSERT( 0 );
						return 1;
					}
					ret = m_mhandler->CopyMotionFrameML( m_shandler, seri, nextmotid, filluppoint, FILLUPMOTIONID, m_mch->m_fillupleng - 1 );
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh CopyMotionFrameML fu last error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					//morph anim
					ret = m_mhandler->CopyMMotAnimFrame( seri, m_shandler, curmotid, curframe, FILLUPMOTIONID, 0 );
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh CopyMMotAnimFrame 0 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ret = m_mhandler->CopyMMotAnimFrame( seri, m_shandler, nextmotid, filluppoint, FILLUPMOTIONID, m_mch->m_fillupleng - 1 );
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh CopyMMotAnimFrame 0 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}



					//dskey, texkey, alpkeyの設定
					CShdElem* selem;
					selem = (*m_shandler)( seri );
					_ASSERT( selem );
					CMQOMaterial* curmat;
					if( selem->IsJoint() && selem->type != SHDMORPH ){

						ret = m_shandler->InitTempDS();
						_ASSERT( !ret );

						ret = m_mhandler->InitTexAnim( m_shandler );
						_ASSERT( !ret );

						if( m_dsfillupmode == 0 ){
							ret = m_mhandler->GetDSAnim( m_shandler->m_tempds, seri, curmotid, curframe );
							_ASSERT( !ret );
						}else{
							ret = m_mhandler->GetDSAnim( m_shandler->m_tempds, seri, nextmotid, filluppoint );
							_ASSERT( !ret );
						}

						if( m_texfillupmode == 0 ){
							ret = m_mhandler->GetTexAnim( seri, curmotid, curframe );
							_ASSERT( !ret );
						}else{
							ret = m_mhandler->GetTexAnim( seri, nextmotid, filluppoint );
							_ASSERT( !ret );
						}

						int dirtyds = 0;
						int dsno;
						for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
							if( (m_shandler->m_tempds + dsno)->state == 0 ){
								dirtyds++;
								break;
							}
						}

						curmat = m_shandler->m_mathead;
						int dirtytex = 0;
						while( curmat ){
							if( curmat->curtexname != NULL ){
								dirtytex++;
								break;
							}
							curmat = curmat->next;
						}

						ret = m_mhandler->DeleteDSKey( seri, FILLUPMOTIONID, 0 );
						_ASSERT( !ret );

						ret = m_mhandler->DeleteTexKey( seri, FILLUPMOTIONID, 0 );
						_ASSERT( !ret );

						ret = m_mhandler->DeleteAlpKey( seri, FILLUPMOTIONID, 0 );
						_ASSERT( !ret );


						if( dirtyds ){
							CDSKey* dskptr;
							ret = m_mhandler->CreateDSKey( seri, FILLUPMOTIONID, 0, &dskptr );
							_ASSERT( !ret );

							for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
								if( (m_shandler->m_tempds + dsno)->state == 0 ){
									ret = dskptr->SetDSChange( m_shandler->m_ds + dsno, 0 );
									_ASSERT( !ret );
								}
							}
						}

						if( dirtytex ){
							CTexKey* tkptr;
							ret = m_mhandler->CreateTexKey( seri, FILLUPMOTIONID, 0, &tkptr );
							_ASSERT( !ret );

							curmat = m_shandler->m_mathead;
							while( curmat ){
								if( curmat->curtexname ){
									ret = tkptr->SetTexChange( curmat, curmat->curtexname );
									_ASSERT( !ret );
								}
								curmat = curmat->next;
							}
						}

						CAlpKey* alpkptr = 0;
						ret = m_mhandler->CreateAlpKey( seri, FILLUPMOTIONID, 0, &alpkptr );
						_ASSERT( !ret );

						curmat = m_shandler->m_mathead;
						while( curmat ){
							float alphaval = 1.0f;
							int existflag = 0;
							ret = m_mhandler->GetAlpAnim( seri, curmotid, curframe, curmat, &alphaval, &existflag );
							_ASSERT( !ret );

							if( alphaval != curmat->orgalpha ){
								ret = alpkptr->SetAlpChange( curmat, alphaval );
								_ASSERT( !ret );
							}
							curmat = curmat->next;
						}

					}


					ret = m_mhandler->FillUpMotionML( m_shandler, FILLUPMOTIONID, seri, 0, m_mch->m_fillupleng - 1 );
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh FillUpMotionML fu error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int nextfirstframe;
					nextfirstframe = min( (filluppoint + 1), (nextframeleng - 1) );

					//ret = m_mhandler->SetNextMotionFrameNo( m_shandler, FILLUPMOTIONID, nextmotid, filluppoint );//// filluppointへジャンプ
					//ret = m_mhandler->SetNextMotionFrameNoML( m_shandler, FILLUPMOTIONID, nextmotid, nextfirstframe, -1 );//// filluppoint + 1へジャンプ
					ret = m_mhandler->SetNextMotionFrameNoML( seri, FILLUPMOTIONID, nextmotid, nextfirstframe, -1 );//// filluppoint + 1へジャンプ
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh SetNextMotionFrameNoML fu error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					ret = m_mhandler->SetMotionKindML( seri, FILLUPMOTIONID );
					if( ret ){
						DbgOut( "handlerset : SetNewPoseByMOAML : mh SetMotionKindML fu error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					if( nextmotid == idlingid ){
						ret = m_mhandler->SetTickCount( seri, curtick );
						_ASSERT( !ret );
					}


			//DbgOut( "check !!! : hs : SetNewPoseByMOA : !!!!!!!!!! FillUp !!!!!!!!!!!!, filluppoint %d, nextframeleng %d\r\n",
			//	   filluppoint, nextframeleng );

				}else{

					// notfu != 0  direct change
					if( nextmotid != idlingid ){
						ret = m_mhandler->SetMotionFrameNoML( m_shandler->m_mathead, seri, nextmotid, nextframe, 1 );
						if( ret ){
							DbgOut( "handlerset : SetNewPoseByMOAML : mh SetMotionFrameNoML error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						int frameno;
						frameno = min( idlingleng, (firstframe + 1) );
						ret = m_mhandler->SetMotionFrameNoML( m_shandler->m_mathead, seri, idlingid, frameno, 1 );
						if( ret ){
							DbgOut( "handlerset : SetNewPoseByMOAML : mh SetMotionFrameNoML error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						ret = m_mhandler->SetTickCount( seri, curtick );
						_ASSERT( !ret );

					}
				}
			}

		}
	}


	int chkid, chkframe;
	ret = m_mhandler->GetFirstTickJoint( 0, idlingid, &chkid, &chkframe );
	if( ret ){
		DbgOut( "hs : SetNewPoseByMOAML : mh GetFirstTickJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( chkid > 0 ){
		int chkseri;
		for( chkseri = 0; chkseri < m_shandler->s2shd_leng; chkseri++ ){
			CShdElem* selem;
			selem = (*m_shandler)( chkseri );

			if( (chkseri != chkid) && selem->IsJoint() && (selem->type != SHDMORPH) ){
				int id, frame;
				ret = m_mhandler->GetMotionFrameNoML( chkseri, &id, &frame );
				if( ret ){
					DbgOut( "hs : SetNewPoseByMOAML : mh GetMotionFrameNoML chk error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( (id == idlingid) && (frame != chkframe) ){
					ret = m_mhandler->SetMotionFrameNoML( m_shandler->m_mathead, chkseri, idlingid, chkframe, 0 );
					if( ret ){
						DbgOut( "hs : SetNewPoseByMOAML : mh SetMotionFrameNoML error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}
	}



// SetNewPoseMLは、複数回E3DSetMOAEventNoMLを呼び出したのち、１回だけ呼び出す。

//	ret = m_mhandler->SetNewPoseML( m_shandler );
//	if( ret ){
//		DbgOut( "hs : SetNewPoseByMOAML : mh SetNewPoseML error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


	return 0;
}



int CHandlerSet::SetNewPoseByMOA( int eventno )
{
	int ret;

	int curmotid = -1;
	int curframe = 0;
	int curframeleng = 0;
	ret = m_mhandler->GetMotionFrameNo( &curmotid, &curframe );
	if( ret || (curmotid < 0) ){
		DbgOut( "handlerset : SetNewPoseByMOA : mh GetMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->GetMotionFrameLength( curmotid, &curframeleng );
	if( ret ){
		DbgOut( "handlerset : SetNewPoseByMOA : mh GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int curmottype = -1;
	ret = m_mhandler->GetMotionType( curmotid, &curmottype );
	if( ret ){
		DbgOut( "handlerset : SetNewPoseByMOA : GetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//DbgOut( "check !!! : hs : SetNewPoseByMOA : curmotid %d, curframe %d, FILLUPMOTIONID %d\r\n",
//	curmotid, curframe, FILLUPMOTIONID );

	int checkfillup = 0;


	int nextmotid = -1;
	int nextframe = 0;
	int nextframeleng = 0;

	int befmotid = -1;
	int befframe = 0;

	int fillupflag = 0;
	int notfu = 0;
	int ev0idle = 0;
	int nottoidle = 0;

	if( curmotid != FILLUPMOTIONID ){
		int tmpnottoidle = 0;
		ret = m_mch->GetNextMotion( curmotid, curframe, eventno, &nextmotid, &nextframe, &notfu, &tmpnottoidle );
		if( ret ){
			DbgOut( "handlerset : SetNewPoseByMOA : mch GetNextMotion error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( tmpnottoidle < 0 ){
			m_mhandler->GetNotToIdle( -1, &nottoidle );
		}else{
			nottoidle = tmpnottoidle;
			m_mhandler->SetNotToIdle( -1, nottoidle );
		}

		ret = m_mch->GetEv0Idle( curmotid, &ev0idle );
		if( ret ){
			DbgOut( "handlerset : SetNewPoseByMOA : mch GetEv0Idle 0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		if( (eventno == 0) && (ev0idle != 0) ){
			nextmotid = m_mch->GetIdlingMotID();
			nextframe = 0;
			notfu = 0;
			if( nextmotid < 0 ){
				DbgOut( "handlerset : SetNewPoseByMOA : mch GetIdlingMotID 0 : idling motion not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			if( curmotid == nextmotid ){
				//最終フレームだった場合は、アイドリングへ
				if( (curmottype == MOTION_CLAMP) && (curframe >= (curframeleng - 1)) && (nottoidle == 0) ){
					nextmotid = m_mch->GetIdlingMotID();
					nextframe = 0;
					notfu = 0;
					if( nextmotid < 0 ){
						DbgOut( "handlerset : SetNewPoseByMOA : mch GetIdlingMotID 1 : idling motion not exist error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

				}
			}
		}
		
		/***
			if( curmotid == nextmotid ){
				//最終フレームだった場合は、アイドリングへ
				if( (curmottype == MOTION_CLAMP) && (curframe >= (curframeleng - 1)) ){
					nextmotid = m_mch->GetIdlingMotID();
					nextframe = 0;
					notfu = 0;
					if( nextmotid < 0 ){
						DbgOut( "handlerset : SetNewPoseByMOA : mch GetIdlingMotID 1 : idling motion not exist error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

				}
			}
		***/


//DbgOut( "check !!! : hs : SetNewPoseByMOA : curmotid != FILLUPMOTIONID, nextmotid %d, nextframe %d\r\n",
//	   nextmotid, nextframe );

	}else{
		//補間モーション中だった場合。
		checkfillup = 1;

		ret = m_mhandler->GetNextMotionFrameNo( FILLUPMOTIONID, &befmotid, &befframe );
		if( ret || (befmotid < 0) ){
			DbgOut( "handlerset : SetNewPoseByMOA : mh GetNextMotionFrameNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int tmpframe;
		tmpframe = max( 0, (befframe - m_mch->m_fillupleng) );
		int tmpnottoidle = 0;
		ret = m_mch->GetNextMotion( befmotid, tmpframe + curframe, eventno, &nextmotid, &nextframe, &notfu, &tmpnottoidle );
		//ret = m_mch->GetNextMotion( befmotid, befframe + curframe, eventno, &nextmotid, &nextframe, &notfu );
		if( ret ){
			DbgOut( "handlerset : SetNewPoseByMOA : mch GetNextMotion error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//DbgOut( "check !!! : hs : SetNewPoseByMOA : curmotid == FILLUPMOTIONID, befmotid %d, befframe %d, nextmotid %d, nextframe %d\r\n",
//	   befmotid, befframe, nextmotid, nextframe );

		if( tmpnottoidle < 0 ){
			m_mhandler->GetNotToIdle( -1, &nottoidle );
		}else{
			nottoidle = tmpnottoidle;
			m_mhandler->SetNotToIdle( -1, nottoidle );
		}


		ret = m_mch->GetEv0Idle( befmotid, &ev0idle );
		if( ret ){
			DbgOut( "handlerset : SetNewPoseByMOA : mch GetEv0Idle 2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		if( (eventno == 0) && (ev0idle != 0) ){
			nextmotid = m_mch->GetIdlingMotID();
			nextframe = 0;
			notfu = 0;
			if( nextmotid < 0 ){
				DbgOut( "handlerset : SetNewPoseByMOA : mch GetIdlingMotID 2 : idling motion not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
		
		if( befmotid != nextmotid ){
			fillupflag = 1;
		}

	}


	if( ((checkfillup == 0) && (curmotid != nextmotid)) || fillupflag ){

		if( notfu == 0 ){

			ret = m_mhandler->GetMotionFrameLength( nextmotid, &nextframeleng );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh GetMotionFrameLength error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int filluppoint;
			filluppoint = min( (nextframeleng - 1), (nextframe + m_mch->m_fillupleng - 1) );

			ret = m_mhandler->CopyMotionFrame( m_shandler, curmotid, curframe, FILLUPMOTIONID, 0 );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh CopyMotionFrame fu0 error %d, %d!!!\n", curmotid, curframe );
				_ASSERT( 0 );
				return 1;
			}

			ret = m_mhandler->CopyMotionFrame( m_shandler, nextmotid, filluppoint, FILLUPMOTIONID, m_mch->m_fillupleng - 1 );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh CopyMotionFrame fu last error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//morph anim
			ret = m_mhandler->CopyMMotAnimFrame( -1, m_shandler, curmotid, curframe, FILLUPMOTIONID, 0 );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh CopyMMotAnimFrame 0 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = m_mhandler->CopyMMotAnimFrame( -1, m_shandler, nextmotid, filluppoint, FILLUPMOTIONID, m_mch->m_fillupleng - 1 );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh CopyMMotAnimFrame 0 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			//dskey, texkey, alpkeyの設定
			int bno;
			for( bno = 0; bno < m_shandler->s2shd_leng; bno++ ){
				CShdElem* selem;
				selem = (*m_shandler)( bno );
				_ASSERT( selem );
				if( selem->IsJoint() && selem->type != SHDMORPH ){

					ret = m_shandler->InitTempDS();
					_ASSERT( !ret );

					ret = m_mhandler->InitTexAnim( m_shandler );
					_ASSERT( !ret );

					if( m_dsfillupmode == 0 ){
						ret = m_mhandler->GetDSAnim( m_shandler->m_tempds, bno, curmotid, curframe );
						_ASSERT( !ret );
					}else{
						ret = m_mhandler->GetDSAnim( m_shandler->m_tempds, bno, nextmotid, filluppoint );
						_ASSERT( !ret );
					}

					if( m_texfillupmode == 0 ){
						ret = m_mhandler->GetTexAnim( bno, curmotid, curframe );
						_ASSERT( !ret );
					}else{
						ret = m_mhandler->GetTexAnim( bno, nextmotid, filluppoint );
						_ASSERT( !ret );
					}

					int dirtyds = 0;
					int dsno;
					for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
						if( (m_shandler->m_tempds + dsno)->state == 0 ){
							dirtyds++;
							break;
						}
					}

					CMQOMaterial* curmat = m_shandler->m_mathead;
					int dirtytex = 0;
					while( curmat ){
						if( curmat->curtexname != NULL ){
							dirtytex++;
							break;
						}
						curmat = curmat->next;
					}

					ret = m_mhandler->DeleteDSKey( bno, FILLUPMOTIONID, 0 );
					_ASSERT( !ret );

					ret = m_mhandler->DeleteTexKey( bno, FILLUPMOTIONID, 0 );
					_ASSERT( !ret );

					ret = m_mhandler->DeleteAlpKey( bno, FILLUPMOTIONID, 0 );
					_ASSERT( !ret );


					if( dirtyds ){
						CDSKey* dskptr;
						ret = m_mhandler->CreateDSKey( bno, FILLUPMOTIONID, 0, &dskptr );
						_ASSERT( !ret );

						for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
							if( (m_shandler->m_tempds + dsno)->state == 0 ){
								ret = dskptr->SetDSChange( m_shandler->m_ds + dsno, 0 );
								_ASSERT( !ret );
							}
						}
					}

					if( dirtytex ){
						CTexKey* tkptr;
						ret = m_mhandler->CreateTexKey( bno, FILLUPMOTIONID, 0, &tkptr );
						_ASSERT( !ret );

						curmat = m_shandler->m_mathead;
						while( curmat ){
							if( curmat->curtexname ){
								ret = tkptr->SetTexChange( curmat, curmat->curtexname );
								_ASSERT( !ret );
							}
							curmat = curmat->next;
						}
					}

					CAlpKey* alpkptr = 0;
					ret = m_mhandler->CreateAlpKey( bno, FILLUPMOTIONID, 0, &alpkptr );
					_ASSERT( !ret );

					curmat = m_shandler->m_mathead;
					while( curmat ){
						float alphaval = 1.0f;
						int existflag = 0;
						ret = m_mhandler->GetAlpAnim( bno, curmotid, curframe, curmat, &alphaval, &existflag );
						_ASSERT( !ret );
						if( alphaval != curmat->orgalpha ){
							ret = alpkptr->SetAlpChange( curmat, alphaval );
							_ASSERT( !ret );
						}
						curmat = curmat->next;
					}


				}
			}
			


			///////
			ret = m_mhandler->FillUpMotion( m_shandler, FILLUPMOTIONID, -1, 0, m_mch->m_fillupleng - 1, 0 );///// initflag 0 !!!!!
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh FillUpMotion fu error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int nextfirstframe;
			nextfirstframe = min( (filluppoint + 1), (nextframeleng - 1) );

			//ret = m_mhandler->SetNextMotionFrameNo( m_shandler, FILLUPMOTIONID, nextmotid, filluppoint );//// filluppointへジャンプ
			ret = m_mhandler->SetNextMotionFrameNo( m_shandler, FILLUPMOTIONID, nextmotid, nextfirstframe, -1 );//// filluppoint + 1へジャンプ
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh SetNextMotionFrameNo fu error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = m_mhandler->SetMotionKind( FILLUPMOTIONID );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh SetMotionKind fu error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

	//DbgOut( "check !!! : hs : SetNewPoseByMOA : !!!!!!!!!! FillUp !!!!!!!!!!!!, filluppoint %d, nextframeleng %d\r\n",
	//	   filluppoint, nextframeleng );

		}else{

			// notfu != 0  direct change
			ret = m_mhandler->SetMotionFrameNo( m_shandler, nextmotid, nextframe, 1 );
			if( ret ){
				DbgOut( "handlerset : SetNewPoseByMOA : mh SetMotionFrameNo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	int tmpnextframe;
	tmpnextframe = m_mhandler->SetNewPose( m_shandler );
	if( tmpnextframe < 0 ){
		DbgOut( "handlerset : SetNewPoseByMOA : mh SetNewPose error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::EnableToonEdge( int srcpartno, int srctype, int srcflag )
{
	int ret;

	ret = m_shandler->EnableToonEdge( srcpartno, srctype, srcflag );
	if( ret ){
		DbgOut( "handlerset : EnableToonEdge : sh EnableToonEdge error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetToonEdge0Color( int srcpartno, char* srcname, int srcr, int srcg, int srcb )
{
	int ret;
	ret = m_shandler->SetToonEdge0Color( srcpartno, srcname, srcr, srcg, srcb );
	if( ret ){
		DbgOut( "handlerset : SetToonEdge0Color : sh SetToonEdge0Color error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CHandlerSet::SetToonEdge0Width( int srcpartno, char* srcname, float srcwidth )
{
	int ret;
	ret = m_shandler->SetToonEdge0Width( srcpartno, srcname, srcwidth );
	if( ret ){
		DbgOut( "handlerset : SetToonEdge0Width : sh SetToonEdge0Width error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CHandlerSet::GetToonEdge0Color( int srcpartno, char* srcname, float* dstr, float* dstg, float* dstb )
{
	int ret;
	ret = m_shandler->GetToonEdge0Color( srcpartno, srcname, dstr, dstg, dstb );
	if( ret ){
		DbgOut( "handlerset : GetToonEdge0Color : sh GetToonEdge0Color error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetToonEdge0Width( int srcpartno, char* srcname, float* dstwidth )
{
	int ret;
	ret = m_shandler->GetToonEdge0Width( srcpartno, srcname, dstwidth );
	if( ret ){
		DbgOut( "handlerset : GetToonEdge0Width : sh GetToonEdge0Width error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetParticlePos( D3DXVECTOR3 srcpos )
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : SetParticlePos : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = bbselem->billboard->SetParticlePos( srcpos );
	if( ret ){
		DbgOut( "handlerset : SetParticlePos : billboard SetParticlePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetParticleGravity( float srcgravity )
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : SetParticleGravity : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = bbselem->billboard->SetParticleGravity( srcgravity );
	if( ret ){
		DbgOut( "handlerset : SetParticleGravity : billboard SetParticleGravity error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetParticleLife( float srclife )
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : SetParticleLife : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = bbselem->billboard->SetParticleLife( srclife );
	if( ret ){
		DbgOut( "handlerset : SetParticleLife : billboard SetParticleLife error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetParticleEmitNum( float srcemitnum )
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : SetParticleEmitNum : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = bbselem->billboard->SetParticleEmitNum( srcemitnum );
	if( ret ){
		DbgOut( "handlerset : SetParticleEmitNum : billboard SetParticleEmitNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetParticleVel0( D3DXVECTOR3 srcminvel, D3DXVECTOR3 srcmaxvel )
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : SetParticleVel0 : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = bbselem->billboard->SetParticleVel0( srcminvel, srcmaxvel );
	if( ret ){
		DbgOut( "handlerset : SetParticleVel0 : billboard SetParticleVel0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetParticleAlpha( float srcmintime, float srcmaxtime, float srcalpha )
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : SetParticleAlpha : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = bbselem->billboard->SetParticleAlpha( m_shandler, srcmintime, srcmaxtime, srcalpha );
	if( ret ){
		DbgOut( "handlerset : SetParticleAlpha : bb SetParticleAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetParticleUVTile( float srcmintime, float srcmaxtime, int srcunum, int srcvnum, int srctileno )
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : SetParticleUVTile : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = bbselem->billboard->SetParticleUVTile( m_shandler, srcmintime, srcmaxtime, srcunum, srcvnum, srctileno );
	if( ret ){
		DbgOut( "handlerset : SetParticleUVTile : bb SetParticleUVTile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::InitParticle()
{
	int ret;
	CShdElem* bbselem;
	bbselem = (*m_shandler)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "handlerset : InitParticle : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = bbselem->billboard->InitParticle();
	if( ret ){
		DbgOut( "handlerset : InitParticle : bb InitParticle error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}

int CHandlerSet::GetMotionIDByName( char* srcmotname, int* dstid )
{

	*dstid = m_mhandler->GetMotionIDByName( srcmotname );

	return 0;
}

int CHandlerSet::GetMotionNum( int* motnum )
{
	*motnum = m_mhandler->m_kindnum;

	return 0;
}

int CHandlerSet::GetCenterPos( int partno, D3DXVECTOR3* posptr )
{
	
	if( partno < 0 ){
		*posptr = m_shandler->m_center;
	}else{
		if( (partno > 0) && (partno < m_shandler->s2shd_leng) ){
			CShdElem* selem;
			selem = (*m_shandler)( partno );
			*posptr = selem->m_center;
		}else{
			DbgOut( "handlerset : GetCenterPos partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CHandlerSet::GetFaceNum( int partno, int* facenumptr )
{
	int ret;

	ret = m_shandler->GetFaceNum( partno, facenumptr );
	if( ret ){
		DbgOut( "handlerset : GetFaceNum : sh GetFaceNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetFaceNormal( int partno, int faceno, D3DXVECTOR3* dstn )
{
	int ret;
	ret = m_shandler->GetFaceNormal( partno, faceno, m_gpd.m_matWorld, m_mhandler, dstn );
	if( ret ){
		DbgOut( "handlerset : GetFaceNormal : sh GetFaceNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::CreateEmptyMotion( char* motionname, int frameleng, int* motidptr, int zatype )
{
	int ret;
	int motiontype = MOTION_CLAMP;
	int motionjump = 0;
	int interpolation = INTERPOLATION_SLERP;

	*motidptr = -1;

	*motidptr = m_mhandler->AddMotion( zatype, motionname, motiontype, frameleng, interpolation, motionjump );
	if( *motidptr < 0 ){
		DbgOut( "handlerset : CreateEmptyMotion : mhandler->AddMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->MakeRawmat( *motidptr, m_shandler, -1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->RawmatToMotionmat( *motidptr, m_shandler, -1, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->SetMotionStep( *motidptr, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = m_mhandler->CalcMPEuler( m_shandler, *motidptr, zatype );//!!!!!!!!!!!!!!!
	if( ret ){
		DbgOut( "handlerset : CreateEmptyMotion : mh CalcMPEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( m_mhandler->m_kindnum == 1 ){
		ret = m_mhandler->SetMotionKind( *motidptr );
		if( ret ){
			DbgOut( "handlerset : CreateEmptyMotion : mh SetMotionKind error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

int CHandlerSet::SetTextureMinMagFilter( int partno , int minfilter, int magfilter )
{
	int ret;

	ret = m_shandler->SetElemTextureMinMagFilter( partno, minfilter, magfilter );
	if( ret ){
		DbgOut( "handlerset : SetTextureMinMagFilter : sh SetTextureMinMagFilter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::GetMotionName( char* dstnameptr, int motid )
{

	if( m_mhandler->s2mot_leng <= 1 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	CMotionCtrl* mcptr = (*m_mhandler)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return 1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetMotionName : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


///////

	char* nameptr;
	nameptr = *(miptr->motname + motid);
		
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= 256 ){
		DbgOut( "handlerset : GetMotionName : namelen too large warning !!!\n" );
		_ASSERT( 0 );
		//strcpy( *(ppstr + setcnt), "エラー。名前が長すぎて格納できません。" );
		//code_setva( pvalstr, aptrstr, TYPE_STRING, "エラー。名前が長すぎて格納できません。" );
		strcpy_s( dstnameptr, 256, "エラー。名前が長すぎて格納できません。" );
	}else{
		//strcpy( *(ppstr + setcnt), telem->name );
		//code_setva( pvalstr, aptrstr, TYPE_STRING, nameptr );
		strcpy_s( dstnameptr, 256, nameptr );
	}
		
	return 0;
}
int CHandlerSet::SetMotionName( int motid, char* srcmotname )
{

	if( *srcmotname == 0 ){
		DbgOut( "hs : SetMotionName : name NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int namelen;
	namelen = (int)strlen( srcmotname );
	if( namelen >= 256 ){
		DbgOut( "hs : SetMotionName : name length too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : SetMotionName : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////

	int ret;
	ret = m_mhandler->SetMotionName( motid, srcmotname );
	if( ret ){
		DbgOut( "hs : SetMotionName : mh SetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetMaterialNoByName( char* srcname, int* matnoptr )
{
	int ret;
	*matnoptr = -3;

	ret = m_shandler->GetMaterialNoByName( srcname, matnoptr );
	if( ret ){
		DbgOut( "hs : GetMaterialNoByName : mqomat GetMaterialNoByName errorr !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetMaterialAlpha( int matno, float* alphaptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialAlpha : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*alphaptr = curmat->col.a;

	return 0;
}

int CHandlerSet::GetMaterialDiffuse( int matno, int* rptr, int* gptr, int* bptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialDiffuse : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	COLORREF col;
	int ret;
	ret = curmat->GetColorrefDiffuse( &col );
	if( ret ){
		DbgOut( "hs : GetMaterialDiffuse : mat GetColorrefDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*rptr = GetRValue( col );
	*gptr = GetGValue( col );
	*bptr = GetBValue( col );

	return 0;
}
int CHandlerSet::GetMaterialSpecular( int matno, int* rptr, int* gptr, int* bptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialSpecular : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	COLORREF col;
	int ret;
	ret = curmat->GetColorrefSpecular( &col );
	if( ret ){
		DbgOut( "hs : GetMaterialSpecular : mat GetColorrefSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*rptr = GetRValue( col );
	*gptr = GetGValue( col );
	*bptr = GetBValue( col );

	return 0;
}
int CHandlerSet::GetMaterialAmbient( int matno, int* rptr, int* gptr, int* bptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialAmbient : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	COLORREF col;
	int ret;
	ret = curmat->GetColorrefAmbient( &col );
	if( ret ){
		DbgOut( "hs : GetMaterialAmbient : mat GetColorrefAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*rptr = GetRValue( col );
	*gptr = GetGValue( col );
	*bptr = GetBValue( col );

	return 0;
}
int CHandlerSet::GetMaterialEmissive( int matno, int* rptr, int* gptr, int* bptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialEmissive : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	COLORREF col;
	int ret;
	ret = curmat->GetColorrefEmissive( &col );
	if( ret ){
		DbgOut( "hs : GetMaterialEmissive : mat GetColorrefEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*rptr = GetRValue( col );
	*gptr = GetGValue( col );
	*bptr = GetBValue( col );

	return 0;
}
int CHandlerSet::GetMaterialPower( int matno, float* powptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialPower : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*powptr = curmat->power;

	return 0;
}

int CHandlerSet::GetMaterialBlendingMode( int matno, int* modeptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialBlendingMode : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*modeptr = curmat->blendmode;

	return 0;
}


int CHandlerSet::SetMaterialAlpha( int matno, float srcalpha )
{
	int ret;
	ret = m_shandler->SetMaterialAlpha( matno, srcalpha );
	if( ret ){
		DbgOut( "hs : SetMaterialAlpha : sh SetMaterialAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMaterialDiffuse( int matno, int setflag, int r, int g, int b )
{
	int ret;
	ret = m_shandler->SetMaterialDiffuse( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "hs : SetMaterialDiffuse : sh SetMaterialDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMaterialSpecular( int matno, int setflag, int r, int g, int b )
{
	int ret;
	ret = m_shandler->SetMaterialSpecular( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "hs : SetMaterialSpecular : sh SetMaterialSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMaterialAmbient( int matno, int setflag, int r, int g, int b )
{
	int ret;
	ret = m_shandler->SetMaterialAmbient( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "hs : SetMaterialAmbient : sh SetMaterialAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMaterialEmissive( int matno, int setflag, int r, int g, int b )
{
	int ret;
	ret = m_shandler->SetMaterialEmissive( matno, setflag, r, g, b );
	if( ret ){
		DbgOut( "hs : SetMaterialEmissive : sh SetMaterialEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::SetMaterialPower( int matno, int setflag, float power )
{
	int ret;
	ret = m_shandler->SetMaterialPower( matno, setflag, power );
	if( ret ){
		DbgOut( "hs : SetMaterialPower : sh SetMaterialPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMaterialBlendingMode( int matno, int mode )
{
	int ret;
	ret = m_shandler->SetMaterialBlendingMode( matno, mode );
	if( ret ){
		DbgOut( "hs : SetMaterialBlendingMode : sh SetMaterialBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CHandlerSet::GetTextureFromMaterial( int matno, int* texidptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : GetTextureFromMaterial : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : GetTextureFromMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curmat->tex[0] != 0 ){
		int texindex;
		texindex = g_texbnk->FindName( curmat->tex, curmat->transparent );

		if( texindex >= 0 ){
			*texidptr = g_texbnk->GetSerial( texindex );
		}else{
			*texidptr = -1;
		}
	}else{
		*texidptr = -1;
	}

	return 0;
}
int CHandlerSet::SetTextureToMaterial( int matno, int texid )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : SetTextureToMaterial : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : SetTextureToMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "handlerset : SetTextureToMaterial : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "handlerset : SetTextureToMaterial : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( curmat->tex, 256, texname );
	curmat->transparent = transparent;

	ret = m_shandler->ChkAlphaNum();
	_ASSERT( !ret );

	return 0;
}

int CHandlerSet::SetBumpMapToMaterial( int matno, int texid )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : SetBumpMapToMaterial : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : SetBumpMapToMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;


	if( texid >= 0 ){

		char* texname;
		int transparent;

		ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
		if( ret ){
			DbgOut( "handlerset : SetBumpMapToMaterial : texbank GetNameBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( !texname ){
			DbgOut( "handlerset : SetBumpMapToMaterial : texid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		strcpy_s( curmat->bump, 256, texname );
		//curmat->transparent = transparent;

	}else{
		// NULL テクスチャ
		curmat->bump[0] = 0;

	}

	ret = m_shandler->ChkAlphaNum();
	_ASSERT( !ret );

	return 0;
}

int CHandlerSet::GetMaterialNo( int partno, int faceno, int* matnoptr )
{
	int ret;
	ret = m_shandler->GetMaterialNo2( partno, faceno, matnoptr );
	if( ret ){
		DbgOut( "hs : GetMaterialNo : sh GetMaterialNo2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMotionKindML( int motid, int* listptr, int* notlistptr )
{
	int ret;

	ret = SetMLList( listptr, notlistptr );
	if( ret ){
		DbgOut( "hs : SetMotionKindML : SetMLList error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		if( *( m_mllist + seri ) == 1 ){
			ret = m_mhandler->SetMotionKindML( seri, motid );
			if( ret ){
				DbgOut( "hs : SetMotionKindML : mh SetMotionKindML error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}

int CHandlerSet::SetMotionFrameNoML( int motid, int frameno, int* listptr, int* notlistptr )
{
	int ret;

	ret = SetMLList( listptr, notlistptr );
	if( ret ){
		DbgOut( "hs : SetMotionFrameNoML : SetMLList error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		if( *( m_mllist + seri ) == 1 ){
			ret = m_mhandler->SetMotionFrameNoML( m_shandler->m_mathead, seri, motid, frameno, 1 );
			if( ret ){
				DbgOut( "hs : SetMotionFrameNoML : mh SetMotionFrameNoML error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}

int CHandlerSet::SetNextMotionFrameNoML( int motid, int nextmk, int nextframe, int befframe, int* listptr, int* notlistptr )
{
	int ret;

	ret = SetMLList( listptr, notlistptr );
	if( ret ){
		DbgOut( "hs : SetNextMotionFrameNoML : SetMLList error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		if( *( m_mllist + seri ) == 1 ){
			ret = m_mhandler->SetNextMotionFrameNoML( seri, motid, nextmk, nextframe, befframe );
			if( ret ){
				DbgOut( "hs : SetNextMotionFrameNoML : mh SetNextMotionFrameNoML error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}


int CHandlerSet::SetMLList( int* listptr, int* notlistptr )
{
	if( !m_mllist || (m_mlleng != m_shandler->s2shd_leng) ){
		if( m_mllist ){
			free( m_mllist );
			m_mllist = 0;
		}

		m_mllist = (int*)malloc( sizeof( int ) * m_shandler->s2shd_leng );
		if( !m_mllist ){
			DbgOut( "hs : SetMLList : mllist alloc error!!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		m_mlleng = m_shandler->s2shd_leng;
	}
	ZeroMemory( m_mllist, sizeof( int ) * m_mlleng );


	int notno = 0;
	int curnot = *notlistptr;
	while( curnot ){
		
		if( notno >= BONELISTNUM ){
			DbgOut( "hs : SetMLList : notlist length too long error (limit%d)!!!\n", BONELISTNUM );
			_ASSERT( 0 );
			return 1;
		}

		SetMLNotListReq( curnot, 0 );

		notno++;
		curnot = *( notlistptr + notno );
	}


	int seri;
	CShdElem* selem;
	int no = 0;
	int curlist = *listptr;
	if( curlist < 0 ){
		
		//notlist以外のすべてのジョイントに１をセット

		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			selem = (*m_shandler)( seri );
			if( selem->IsJoint() && (selem->type != SHDMORPH) ){
				int curval;
				curval = *( m_mllist + seri );
				if( curval == 0 ){
					*( m_mllist + seri ) = 1;
				}
			}
		}

	}else{
		while( curlist ){
			if( no >= BONELISTNUM ){
				DbgOut( "hs : SetMLList : list length too long error (limit%d)!!!\n", BONELISTNUM );
				_ASSERT( 0 );
				return 1;
			}

			SetMLListReq( curlist, 0 );

			no++;
			curlist = *( listptr + no );
		}
	}



	return 0;
}

void CHandlerSet::SetMLNotListReq( int srcnot, int broflag )
{
	if( (srcnot >= 0) && (srcnot < m_shandler->s2shd_leng) ){
		CShdElem* selem;
		selem = (*m_shandler)( srcnot );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			*( m_mllist + srcnot ) = 2;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		if( broflag && selem->brother ){
			SetMLNotListReq( selem->brother->serialno, 1 );
		}
		
		if( selem->child ){
			SetMLNotListReq( selem->child->serialno, 1 );
		}

	}
}
void CHandlerSet::SetMLListReq( int srclist, int broflag )
{
	if( (srclist >= 0) && (srclist < m_shandler->s2shd_leng) ){
		CShdElem* selem;
		selem = (*m_shandler)( srclist );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){

			if( *( m_mllist + srclist ) == 0 ){
				*( m_mllist + srclist ) = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!
			}
		}

		if( broflag && selem->brother ){
			SetMLListReq( selem->brother->serialno, 1 );
		}
		
		if( selem->child ){
			SetMLListReq( selem->child->serialno, 1 );
		}

	}
}
int CHandlerSet::GetMOATrunkInfo( int motid, MOATRUNKINFO* trunkptr )
{
	if( !m_mch ){
		DbgOut( "hs : GetMOATrunkInfo : mch NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mch->GetTrunkInfo( motid, trunkptr );
	if( ret ){
		DbgOut( "hs : GetMOATrunkInfo : mch GetTrunkInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;



}


int CHandlerSet::GetMOATrunkInfo( int motid, int* infoptr )
{
	if( !m_mch ){
		DbgOut( "hs : GetMOATrunkInfo : mch NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mch->GetTrunkInfo( motid, infoptr );
	if( ret ){
		DbgOut( "hs : GetMOATrunkInfo : mch GetTrunkInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::GetMOABranchInfo( int motid, MOABRANCHINFO* branchptr, int arrayleng, int* getnum )
{

	if( !m_mch ){
		DbgOut( "hs : GetMOABranchInfo : mch NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mch->GetBranchInfo( motid, branchptr, arrayleng, getnum );
	if( ret ){
		DbgOut( "hs : GetMOATrunkInfo : mch GetTrunkInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int CHandlerSet::GetMOABranchInfo( int motid, int* infoptr, int branchnum, int* getnum )
{

	if( !m_mch ){
		DbgOut( "hs : GetMOABranchInfo : mch NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mch->GetBranchInfo( motid, infoptr, branchnum, getnum );
	if( ret ){
		DbgOut( "hs : GetMOATrunkInfo : mch GetTrunkInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::SetMOABranchFrame1( int tmotid, int bmotid, int frame1 )
{
	if( !m_mch ){
		DbgOut( "hs : SetMOABranchFrame1 : mch NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mch->SetBranchFrame1( tmotid, bmotid, frame1 );
	if( ret ){
		DbgOut( "hs : SetMOABranchFrame1 : mch SetBranchFrame1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMOABranchFrame2( int tmotid, int bmotid, int frame2 )
{
	if( !m_mch ){
		DbgOut( "hs : SetMOABranchFrame2 : mch NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mch->SetBranchFrame2( tmotid, bmotid, frame2 );
	if( ret ){
		DbgOut( "hs : SetMOABranchFrame2 : mch SetBranchFrame2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::EnablePhongEdge0( int flag )
{
	int ret;
	ret = m_shandler->EnablePhongEdge0( flag );
	_ASSERT( !ret );

	return ret;
}

int CHandlerSet::SetPhongEdge0Params( int r, int g, int b, float width, int blendmode, float alpha )
{
	int ret;
	ret = m_shandler->SetPhongEdge0Params( r, g, b, width, blendmode, alpha );
	_ASSERT( !ret );

	return ret;
}

int CHandlerSet::SetResDir( char* pathname )
{
	if( !m_shandler ){
		_ASSERT( 0 );
		return 0;
	}


	int ret;
	ret = m_shandler->SetDirName( pathname );
	if( ret ){
		DbgOut( "hs : SetResDir : sh SetDirName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

char* CHandlerSet::GetResDir()
{
	if( !m_shandler ){
		_ASSERT( 0 );
		return 0;
	}

	if( m_shandler->m_dirname[0] ){
		return m_shandler->m_dirname;
	}else{
		return 0;
	}
}

int CHandlerSet::SetDSFillUpMode( int mode )
{

	m_dsfillupmode = mode;

	return 0;
}
int CHandlerSet::SetTexFillUpMode( int mode )
{

	m_texfillupmode = mode;

	return 0;
}

int CHandlerSet::SetTempFogEnable( int srcenable )
{
	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*m_shandler)( seri );

		selem->m_tempfogenable = selem->m_renderstate[ D3DRS_FOGENABLE ];
		selem->m_renderstate[ D3DRS_FOGENABLE ] = srcenable;
	}

	return 0;
}
int CHandlerSet::RestoreTempFogEnable()
{
	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*m_shandler)( seri );

		selem->m_renderstate[ D3DRS_FOGENABLE ] = selem->m_tempfogenable;
	}

	return 0;
}

int CHandlerSet::SetPointListForShadow( CVec3List* plist )
{
	int ret;

	if( m_groundflag == 0 ){
		ret = m_shandler->SetPointListForShadow( plist );
		if( ret ){
			DbgOut( "handlerset : SetPointListForShadow : sh SetPointListForShadow error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CHandlerSet::GlobalToLocal( D3DXVECTOR3 srcv, D3DXVECTOR3* dstv )
{
	D3DXMATRIX invw;
	D3DXMatrixInverse( &invw, NULL, &m_gpd.m_matWorld );

	D3DXVec3TransformCoord( dstv, &srcv, &invw );

	return 0;
}
int CHandlerSet::LocalToGlobal( D3DXVECTOR3 srcv, D3DXVECTOR3* dstv )
{

	D3DXVec3TransformCoord( dstv, &srcv, &m_gpd.m_matWorld );

	return 0;
}

int CHandlerSet::SetShadowInterFlag( int partno, int srcflag )
{
	int ret;
	ret = m_shandler->SetShadowInterFlag( partno, srcflag );
	if( ret ){
		DbgOut( "hs : SetShadowInterFlag : sh SetShadowInterFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMaterialAlphaTest( int matno, int alphatest0, int alphaval0, int alphatest1, int alphaval1 )
{
	int ret;

	ret = m_shandler->SetMaterialAlphaTest( matno, alphatest0, alphaval0, alphatest1, alphaval1 );
	if( ret ){
		DbgOut( "hs : SetMaterialAlphaTest : sh SetMaterialAlphaTest error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetMaterialAlphaTest( int materialno, int* test0ptr, int* val0ptr, int* test1ptr, int* val1ptr )
{
	int ret;

	ret = m_shandler->GetMaterialAlphaTest( materialno, test0ptr, val0ptr, test1ptr, val1ptr );
	if( ret ){
		DbgOut( "hs : GetMaterialAlphaTest : sh GetMaterialAlphaTest error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::GetMaterialNum( int* numptr )
{
	if( !m_shandler ){
		*numptr = 0;
		return 0;
	}

	CMQOMaterial* curmat = m_shandler->m_mathead;
	int setno = 0;

	while( curmat ){
		if( curmat->materialno >= 0 ){
			setno++;
		}
		curmat = curmat->next;
	}

	*numptr = setno;

	return 0;
}
int CHandlerSet::GetMaterialInfo( MATERIALINFO* infoptr, int arrayleng, int* getnumptr )
{
	if( !m_shandler ){
		*getnumptr = 0;
		return 0;
	}

	CMQOMaterial* curmat = m_shandler->m_mathead;
	int setno = 0;

	while( curmat ){
		if( curmat->materialno >= 0 ){
			if( setno >= arrayleng ){
				DbgOut( "handlerset : GetMaterialInfo : array too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			(infoptr + setno)->materialno = curmat->materialno;

			strcpy_s( (infoptr + setno)->name, 256, curmat->name );

			(infoptr + setno)->alpha = curmat->dif4f.a;
			
			(infoptr + setno)->diffuse.a = 255;
			(infoptr + setno)->diffuse.r = (unsigned char)( curmat->dif4f.r * 255.0f );
			(infoptr + setno)->diffuse.g = (unsigned char)( curmat->dif4f.g * 255.0f );
			(infoptr + setno)->diffuse.b = (unsigned char)( curmat->dif4f.b * 255.0f );

			(infoptr + setno)->specular.a = 255;
			(infoptr + setno)->specular.r = (unsigned char)( curmat->spc3f.r * 255.0f );
			(infoptr + setno)->specular.g = (unsigned char)( curmat->spc3f.g * 255.0f );
			(infoptr + setno)->specular.b = (unsigned char)( curmat->spc3f.b * 255.0f );

			(infoptr + setno)->ambient.a = 255;
			(infoptr + setno)->ambient.r = (unsigned char)( curmat->amb3f.r * 255.0f );
			(infoptr + setno)->ambient.g = (unsigned char)( curmat->amb3f.g * 255.0f );
			(infoptr + setno)->ambient.b = (unsigned char)( curmat->amb3f.b * 255.0f );

			(infoptr + setno)->emissive.a = 255;
			(infoptr + setno)->emissive.r = (unsigned char)( curmat->emi3f.r * 255.0f );
			(infoptr + setno)->emissive.g = (unsigned char)( curmat->emi3f.g * 255.0f );
			(infoptr + setno)->emissive.b = (unsigned char)( curmat->emi3f.b * 255.0f );
			
			(infoptr + setno)->power = curmat->power;
			
			setno++;
		}
		curmat = curmat->next;
	}

	*getnumptr = setno;

	return 0;
}

int CHandlerSet::GetToon1MaterialNum( int partno, int* numptr )
{
	*numptr = 0;

	if( !m_shandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "handlerset : GetToon1MaterialNum : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->GetToon1MaterialNum( numptr );
	if( ret ){
		DbgOut( "handlerset : GetToon1MaterialNum : se GetToon1MaterialNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetToon1MaterialInfo( int partno, TOON1MATERIALINFO* infoptr, int arrayleng, int* getnumptr )
{
	if( !m_shandler ){
		*getnumptr = 0;
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		*getnumptr = 0;
		DbgOut( "handlerset : GetToon1MaterialInfo : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->GetToon1MaterialInfo( infoptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "handlerset : GetToon1MaterialInfo : se GetToon1MaterialInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CHandlerSet::SetJointInitialPos( int jointno, D3DXVECTOR3 pos, int calcflag, int excludeflag )
{
	int ret;
	if( !m_shandler ){
		return 0;
	}

	ret = m_shandler->ChangeJointInitialPos( jointno, pos );
	if( ret ){
		DbgOut( "handlerset : SetJointInitialPos : sh ChangeJointInitialPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( calcflag ){
		ret = m_shandler->CalcInfElem( -1, 1, excludeflag );
		if( ret ){
			DbgOut( "handlerset : SetJointInitialPos : sh CalcInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CHandlerSet::CreateSkinMat( int partno )
{
	if( !m_shandler ){
		return 0;
	}

	int ret;
	ret = m_shandler->CreateSkinMat( partno, 1 );
	if( ret ){
		DbgOut( "hs : CreateSkinMat : sh CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetSymInfElem( int partno, int vertno, int symaxis, float symdist )
{
	if( !m_shandler ){
		return 0;
	}

	int ret;
	if( partno < 0 ){
		int seri;
		for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
			ret = SetSymInfElemPart( seri, vertno, symaxis, symdist );
			if( ret ){
				DbgOut( "hs : SetSymInfElem : SetSymInfElemPart error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;

	}else{
		if( partno >= m_shandler->s2shd_leng ){
			DbgOut( "hs : SetSymInfElem : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetSymInfElemPart( partno, vertno, symaxis, symdist );
		if( ret ){
			DbgOut( "hs : SetSymInfElem : SetSymInfElemPart error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		return 0;
	}

}

int CHandlerSet::SetSymInfElemPart( int partno, int vertno, int symaxis, float symdist )
{
	// partno はチェック済み

	int ret;

	if( vertno < 0 ){
		int vertnum = 0;
		ret = m_shandler->GetVertNumOfPart( partno, &vertnum );
		if( (ret == 0) && (vertnum > 0) ){ 
			int vno;
			for( vno = 0; vno < vertnum; vno++ ){
				int ret2;
				ret2 = SetSymInfElemPart( partno, vno, symaxis, symdist );
				if( ret2 ){
					DbgOut( "hs : SetSymInfElemPart : %d error !!!\n", vno );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}else{
		int sympart = -1;
		int symvert = -1;
		
		ret = m_shandler->GetSymVert( partno, vertno, symaxis, symdist, &sympart, &symvert );
		if( ret ){
			DbgOut( "hs SetSymInfElemPart : sh GetSymVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( (sympart <= 0) || (symvert < 0) ){
			return 0;
		}

		ret = SetSymIE( sympart, symvert, symaxis, &m_syminfnum );
		if( ret ){
			DbgOut( "hs SetSymInfElemPart : SetSymIE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = CopySymInf( partno, vertno, symaxis, symdist );
		if( ret ){
			DbgOut( "hs SetSymInfElemPart : CopySymInf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CHandlerSet::CopySymInf( int seldisp, int selvert, int symaxis, float symdist )
{
	int ret;
	if( m_syminfnum <= 0 ){
		return 0;
	}

	ret = m_shandler->DestroyIE( seldisp, selvert );
	if( ret ){
		DbgOut( "hs : CopySymInf : sh DestroyIE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int infno;
	for( infno = 0; infno < m_syminfnum; infno++ ){
		ret = m_shandler->AddInfElem( seldisp, selvert, m_symIE[infno] );
		if( ret ){
			DbgOut( "hs : CopySymInf : sh AddInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = m_shandler->SetInfElemSymParams( seldisp, selvert, symaxis, symdist );
	if( ret ){
		DbgOut( "hs : CopySymInf : sh SetInfElemSymParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



int CHandlerSet::SetSymIE( int sympart, int symvert, int symaxis, int* syminfnumptr )
{

	int ret;
	INFELEM tempIE[ INFNUMMAX ];

	ret = m_shandler->GetInfElemNum( sympart, symvert, syminfnumptr );
	if( ret ){
		DbgOut( "hs : SetSymIE : sh GetInfElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( *syminfnumptr > INFNUMMAX )
		*syminfnumptr = INFNUMMAX;//!!!!!!!!!!

	int sinfno;
	INFELEM* psIE;
	for( sinfno = 0; sinfno < *syminfnumptr; sinfno++ ){
		ret = m_shandler->GetInfElem( sympart, symvert, sinfno, &psIE );
		if( ret || !psIE ){
			DbgOut( "hs : SetSymIE : sh GetInfElem sym error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		tempIE[sinfno] = *psIE;
	}

	//////////

	ret = Conv2SymIE( tempIE, *syminfnumptr, symaxis, m_symIE );
	if( ret ){
		DbgOut( "hs : SetSymIE : Conv2SymIE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::Conv2SymIE( INFELEM* srcIE, int infnum, int symaxis, INFELEM* dstIE )
{
	//int ret;
	int infno;
	INFELEM* cursrc;
	INFELEM* curdst;

	int childno;
	int symchildno;

	CTreeElem2* childte;
	int childleng;

	int patleng1, patleng2, patleng3, patleng4;
	int cmp1, cmp2, cmp3, cmp4;

	for( infno = 0; infno < infnum; infno++ ){
		int childsymkind = -1;//1:xp, 2:xm, 3:mikoxp, 4:mikoxm, 5:yp, 6:ym, 7:zp, 8:zm
		int bonesymkind = -1;

		cursrc = srcIE + infno;
		curdst = dstIE + infno;

		childno = cursrc->childno;

		if( childno > 0 ){
			childte = (*m_thandler)( childno );
			childleng = (int)strlen( childte->name );
		}else{
			childte = 0;
			childleng = 0;
		}

		if( symaxis == SYMAXIS_X ){
			patleng1 = (int)strlen( strsymmtype[SYMMTYPE_XP] );
			if( childleng > patleng1 ){
				cmp1 = strcmp( childte->name + childleng - patleng1, strsymmtype[SYMMTYPE_XP] );
				if( cmp1 == 0 ){
					childsymkind = 1;
				}
			}
			patleng2 = (int)strlen( strsymmtype[SYMMTYPE_XM] );
			if( childleng > patleng2 ){
				cmp2 = strcmp( childte->name + childleng - patleng2, strsymmtype[SYMMTYPE_XM] );
				if( cmp2 == 0 ){
					childsymkind = 2;
				}
			}
			patleng3 = (int)strlen( strsymmtype2[SYMMTYPE_XP] );
			if( childleng > patleng3 ){
				cmp3 = strcmp( childte->name + childleng - patleng3, strsymmtype2[SYMMTYPE_XP] );
				if( cmp3 == 0 ){
					childsymkind = 3;
				}
			}
			patleng4 = (int)strlen( strsymmtype2[SYMMTYPE_XM] );
			if( childleng > patleng4 ){
				cmp4 = strcmp( childte->name + childleng - patleng4, strsymmtype2[SYMMTYPE_XM] );
				if( cmp4 == 0 ){
					childsymkind = 4;
				}
			}
		}else if( symaxis == SYMAXIS_Y ){
			patleng1 = (int)strlen( strsymmtype[SYMMTYPE_YP] );
			if( childleng > patleng1 ){
				cmp1 = strcmp( childte->name + childleng - patleng1, strsymmtype[SYMMTYPE_YP] );
				if( cmp1 == 0 ){
					childsymkind = 5;
				}
			}
			patleng2 = (int)strlen( strsymmtype[SYMMTYPE_YM] );
			if( childleng > patleng2 ){
				cmp2 = strcmp( childte->name + childleng - patleng2, strsymmtype[SYMMTYPE_YM] );
				if( cmp2 == 0 ){
					childsymkind = 6;
				}
			}
		}else if( symaxis == SYMAXIS_Z ){
			patleng1 = (int)strlen( strsymmtype[SYMMTYPE_ZP] );
			if( childleng > patleng1 ){
				cmp1 = strcmp( childte->name + childleng - patleng1, strsymmtype[SYMMTYPE_ZP] );
				if( cmp1 == 0 ){
					childsymkind = 7;
				}
			}
			patleng2 = (int)strlen( strsymmtype[SYMMTYPE_ZM] );
			if( childleng > patleng2 ){
				cmp2 = strcmp( childte->name + childleng - patleng2, strsymmtype[SYMMTYPE_ZM] );
				if( cmp2 == 0 ){
					childsymkind = 8;
				}
			}
		}else{
			_ASSERT( 0 );
		}


//DbgOut( "paintdlg : Conv2SymIE : kind %d, te name %s\r\n", childte->name );


		symchildno = GetSymJointNo( childte->name, childno, childsymkind );

		/***
		int symboneno;
		if( symchildno > 0 ){
			CShdElem* childse;
			childse = (*m_papp->m_shandler)( symchildno );
			CShdElem* parentse;
			parentse = m_papp->m_shandler->FindUpperJoint( childse );
			if( parentse ){
				symboneno = parentse->serialno;
			}else{
				symboneno = -1;
			}
		}else{
			symboneno = -1;
		}
		***/

		*curdst = *cursrc;
		curdst->kind = CALCMODE_SYM;
		curdst->childno = symchildno;
		//curdst->bonematno = symboneno;
		curdst->bonematno = symchildno;//!!!!!!!!

	}

	return 0;
}

int CHandlerSet::GetSymJointNo( char* srcname, int srcno, int symkind )
{
	int symno = srcno;//!!!!!!!!!!!!!!

	char findname[ 2048 ];
	int patleng;

	int namelen;
	namelen = (int)strlen( srcname );
	if( namelen >= 2048 ){
		DbgOut( "hs : GetSymJointNo : namelen too long error !!!\n" );
		_ASSERT( 0 );
		return symno;//!!!!!!!!!!!!!!
	}


	switch( symkind ){
	case 1:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype[SYMMTYPE_XP] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype[SYMMTYPE_XM] );

		break;
	case 2:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype[SYMMTYPE_XM] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype[SYMMTYPE_XP] );

		break;
	case 3:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype2[SYMMTYPE_XP] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype2[SYMMTYPE_XM] );

		break;
	case 4:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype2[SYMMTYPE_XM] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype2[SYMMTYPE_XP] );

		break;
	case 5:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype[SYMMTYPE_YP] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype[SYMMTYPE_YM] );

		break;
	case 6:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype[SYMMTYPE_YM] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype[SYMMTYPE_YP] );

		break;
	case 7:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype[SYMMTYPE_ZP] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype[SYMMTYPE_ZM] );

		break;
	case 8:
		strcpy_s( findname, 2048, srcname );
		patleng = (int)strlen( strsymmtype[SYMMTYPE_ZM] );
		strcpy_s( findname + namelen - patleng, 2048 - namelen + patleng, strsymmtype[SYMMTYPE_ZP] );

		break;
	default:
		strcpy_s( findname, 2048, srcname );
		break;
	}

	int ret;
	ret = m_thandler->GetBoneNoByName( findname, &symno, m_shandler, 0 );
	if( ret ){
		DbgOut( "hs : GetSymJointNo : th GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		symno = srcno;
	}
	if( symno <= 0 )
		symno = srcno;

	return symno;
	
}

int CHandlerSet::UpdateSymInfElem( int excludeflag )
{
	if( !m_shandler ){
		return 0;
	}


	int ret;

	int dispnum;
	dispnum = m_shandler->s2shd_leng;
	int dispno;
	for( dispno = 0; dispno < dispnum; dispno++ ){
		CShdElem* selem;
		selem = (*m_shandler)( dispno );
		if( ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) && 
			( (excludeflag == 0) || (selem->notuse == 0) ) ){
			int vertnum;

			ret = m_shandler->GetVertNumOfPart( dispno, &vertnum );
			if( ret ){
				DbgOut( "hs : UpdateSymInfElem : sh GetVertNumOfPart error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			int vertno;
			for( vertno = 0; vertno < vertnum; vertno++ ){
				int calcmode;
				ret = m_shandler->GetInfElemCalcmode( dispno, vertno, 0, &calcmode );
				if( ret ){
					DbgOut( "hs : UpdateSymInfElem : sh GetInfElemCalcmode error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( calcmode == CALCMODE_SYM ){
					int symaxis;
					float symdist;
					ret = m_shandler->GetInfElemSymParams( dispno, vertno, &symaxis, &symdist );
					if( ret ){
						DbgOut( "hs : UpdateSymInfElem : sh GetInfElemSymParams error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int sympart, symvert;

					ret = m_shandler->GetSymVert( dispno, vertno, symaxis, symdist, &sympart, &symvert );
					if( ret ){
						DbgOut( "hs : UpdateSymInfElem : sh GetSymVert error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					if( (sympart > 0) && (symvert >= 0) ){

						int syminfnum = 0;

						ret = SetSymIE( sympart, symvert, symaxis, &m_syminfnum );
						if( ret ){
							DbgOut( "paintdlg : OnSymRenew : SetSymIE error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}


						ret = m_shandler->DestroyIE( dispno, vertno );
						if( ret ){
							DbgOut( "hs : UpdateSymInfElem : sh DestroyIE error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
						int infno;
						for( infno = 0; infno < m_syminfnum; infno++ ){
							ret = m_shandler->AddInfElem( dispno, vertno, m_symIE[infno] );
							if( ret ){
								DbgOut( "hs : UpdateSymInfElem : sh AddInfElem error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
					}
				}
			}


			ret = m_shandler->CreateSkinMat( dispno, 1 );
			if( ret ){
				DbgOut( "hs : UpdateSymInfElem : sh CreateSkinMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CHandlerSet::CalcMLMotion()
{
	if( !m_shandler || !m_mhandler ){
		return 0;
	}

	int ret;
	ret = m_mhandler->CalcMLMotion( m_shandler );
	if( ret ){
		DbgOut( "handlerset : CalcMLMotion : mh CalcMLMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::ChangeMoaIdling( int motid )
{
	if( !m_mch ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_mch->ChangeIdlingMotion( m_shandler, motid );
	if( ret ){
		DbgOut( "hs : ChangeMoaIdling : mch ChangeIdlingMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMotionBlur( int mode, int blurtime )
{
	if( !m_shandler )
		return 0;

	int ret;

	ret = m_shandler->SetMotionBlur( mode, blurtime );
	if( ret ){
		DbgOut( "hs : SetMotionBlur : sh SetMotionBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMotionBlurPart( int mode, int blurtime, int* partptr, int num )
{
	if( !m_shandler )
		return 0;

	int ret;
	int no;
	for( no = 0; no < num; no++ ){
		int seri;
		seri = *( partptr + no );
		if( (seri >= 0) && (seri < m_shandler->s2shd_leng) ){
			CShdElem* selem;
			selem = (*m_shandler)( seri );
			_ASSERT( selem );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) || (selem->type == SHDMORPH) ){
				ret = selem->SetMotionBlur( m_shandler, mode, blurtime );
				if( ret ){
					DbgOut( "hs : SetMotionBlurPart : se SetMotionBlur error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;
}


int CHandlerSet::SetBeforeBlur()
{
	if( !m_shandler )
		return 0;

	int ret;
	ret = m_shandler->SetBeforeBlur( m_billboardflag );
	if( ret ){
		DbgOut( "hs : SetBeforeBlur : sh SetBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::InitBeforeBlur()
{
	if( !m_shandler )
		return 0;

	int ret;
	ret = m_shandler->InitBeforeBlur();
	if( ret ){
		DbgOut( "hs : InitBeforeBlur : sh InitBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::RenderMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !m_shandler )
		return 0;

	int ret;
	ret = m_shandler->RenderMotionBlur( pd3dDevice, m_mhandler );
	if( ret ){
		DbgOut( "hs : RenderMotionBlur : sh RenderMotionBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMotionBlurMinAlpha( int partno, float minalpha )
{
	if( !m_shandler )
		return 0;

	int ret;
	ret = m_shandler->SetMotionBlurMinAlpha( partno, minalpha );
	if( ret ){
		DbgOut( "hs : SetMotionBlurMinAlpha : sh SetMotionBlurMinAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CHandlerSet::SetMotionBlurMaxAlpha( int partno, float maxalpha )
{
	if( !m_shandler )
		return 0;

	int ret;
	ret = m_shandler->SetMotionBlurMaxAlpha( partno, maxalpha );
	if( ret ){
		DbgOut( "hs : SetMotionBlurMaxAlpha : sh SetMotionBlurMaxAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::RenderGlow( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, D3DXMATRIX matView, D3DXVECTOR3 camerapos )
{
	int ret;
	int inbatch = 0;
	int curcookie = -1;
	if( m_billboardflag == 0 ){
		ret = m_shandler->Render( 0, inbatch, pd3dDevice, withalpha, POLYDISP_ALL, m_mhandler, curcookie, m_gpd.m_matWorld, matView, g_matProj, camerapos, 0, 0, WITHGLOW );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		//billboardのグローはまだ未対応
	}

	return 0;
}

int CHandlerSet::SetMaterialGlowParams( int matno, int glowmode, int setcolflag, E3DCOLOR4F* colptr )
{
	int ret;
	ret = m_shandler->SetMaterialGlowParams( matno, glowmode, setcolflag, colptr );
	if( ret ){
		DbgOut( "hs : SetMaterialGlowParams : sh SetMaterialGlowParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::GetMaterialGlowParams( int matno, int* glowmodeptr, E3DCOLOR4F* colptr )
{
	int ret;
	ret = m_shandler->GetMaterialGlowParams( matno, glowmodeptr, colptr );
	if( ret ){
		DbgOut( "hs : GetMaterialGlowParams : sh GetMaterialGlowParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CHandlerSet::SetMaterialTransparent( int matno, int transparent )
{
	int ret;
	ret = m_shandler->SetMaterialTransparent( matno, transparent );
	if( ret ){
		DbgOut( "hs : SetMaterialTransparent : sh SetMaterialTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMaterialTransparent( int matno, int* transptr )
{
	int ret;
	ret = m_shandler->GetMaterialTransparent( matno, transptr );
	if( ret ){
		DbgOut( "hs : GetMaterialTransparent : sh GetMaterialTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::CreateBone( LPDIRECT3DDEVICE9 pdev, HWND appwnd, char* nameptr, int parentid, int* seriptr )
{
	*seriptr = -1;
	int ret;

	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* parselem = 0;
	if( parentid >= 0 ){
		if( parentid >= m_shandler->s2shd_leng ){
			DbgOut( "hs : CreateBone : parentid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		parselem = (*m_shandler)( parentid );
		_ASSERT( parselem );
	}else{
		int firstparent = 1;
		ret = m_shandler->GetFirstParentNo( &firstparent );
		if( ret ){
			DbgOut( "hs : CreateBone : sh GetFirstParentNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		parselem = (*m_shandler)( firstparent );
	}
	_ASSERT( parselem );


	Invalidate( 1 );

	ret = m_shandler->DestroyBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "hs : CreateBone : DestroyBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = DestroyAllMotion();
	if( ret ){
		DbgOut( "hs : CreateBone : DestroyAllMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////////////////
	int destseri = -1;
	ret = m_shandler->GetDestroyedSeri( &destseri );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int serino;
	int srctype = SHDBALLJOINT;
	int depth;	

	if( destseri < 0 ){
		serino = m_shandler->s2shd_leng;

		m_thandler->Start( serino - 1 );
		m_thandler->Begin();
		depth = m_thandler->GetCurDepth();

		int createflag = 1;
		ret = Add2Tree( nameptr, serino, SHDBALLJOINT, depth, 1, createflag );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		m_thandler->End();

	}else{
		serino = destseri;
		depth = 2;

		int createflag = 0;
		ret = Add2Tree( nameptr, serino, SHDBALLJOINT, depth, 1, createflag );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	CVec3f temploc;
	temploc.x = 0.0f;
	temploc.y = 0.0f;
	temploc.z = 0.0f;
	ret = ((*m_shandler)( serino ))->SetJointLoc( &temploc );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* curelem;
	curelem = (*m_shandler)( serino );
	_ASSERT( curelem );
	ret = curelem->LeaveFromChain();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = curelem->PutToChain( parselem, 1 );
	if( ret ){
		DbgOut( "hs : CreateBone : PutToChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->CalcDepth();
	if( ret ){
		DbgOut( "hs : CreateBone : shandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_mhandler->CopyChainFromShd( m_shandler );
	if( ret ){
		DbgOut( "hs : CreateBone : mhandler CopyChainFromShd error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->CalcDepth();
	if( ret ){
		DbgOut( "hs : CreateBone : mhandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	////////////////////
	////////////////////
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "hs : CreateBone : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Restore( pdev, appwnd, -1, 1 );
	if( ret ){
		DbgOut( "hs : CreateBone : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*seriptr = serino;

	return 0;
}
int CHandlerSet::DestroyBone( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int delno )
{

	int ret = 0;
	if( !m_shandler || ( m_shandler && (m_shandler->s2shd_leng <= 1)) || !m_thandler || !m_mhandler ){
		//データが無い時は、何もしない。
		return 0;
	}
	if( (delno <= 0) || (delno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs DestroyBone : delno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int saveparent;
	m_shandler->GetParent( delno, &saveparent );
	if( saveparent <= 1 ){
		saveparent = 1;
	}

	///////////////////

	Invalidate( 1 );

	ret = m_shandler->DestroyBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "hs : DestroyBone : DestroyBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = DestroyAllMotion();
	if( ret ){
		DbgOut( "hs : DestroyBone : DestroyAllMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	//////////////////

	ret = m_shandler->DeleteJoint( delno, m_thandler, m_mhandler );
	if( ret ){
		DbgOut( "hs : DestroyBone : sh DeleteJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	///////////////////

	//chain は、sh DeleteJoint内で修正する。thandlerはそのまま。

	////////////////////

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "hs : DestroyBone : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Restore( pdev, appwnd, -1, 1 );
	if( ret ){
		DbgOut( "hs : DestroyBone : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->CalcInfElem( -1, 1 );
	if( ret ){
		DbgOut( "hs : DestroyBone : CalcInfElem error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	return 0;
}

int CHandlerSet::Add2Tree( char* newname, int serino, int srctype, int depth, int hasmotion, int createflag )
{
	int ret = 0;
	if( createflag ){
		int retseri;
		retseri = m_thandler->AddTree( newname, serino );
		if( retseri != serino ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = (*m_thandler)( serino )->SetName( newname );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	ret = m_thandler->SetElemType( serino, srctype );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//////////////
	if( createflag ){
		ret = m_shandler->AddShdElem( serino, depth );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		(*m_shandler)( serino )->type = srctype;
	}
	ret = m_shandler->SetClockwise( serino, 2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//////////////
	if( createflag ){
		ret = m_mhandler->AddMotionCtrl( serino, depth );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}	
	}else{
		(*m_mhandler)( serino )->type = srctype;
	}
	ret = m_mhandler->SetHasMotion( serino, hasmotion );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	///////////////
	CMeshInfo tempinfo;
	tempinfo.type = srctype;
	tempinfo.SetMem( 0, MESHI_HASCOLOR );
	ret = m_shandler->Init3DObj( serino, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::GetMorphBaseNum( int* basenumptr )
{
	*basenumptr = 0;

	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = m_shandler->GetMorphElem( 0, basenumptr, 0 );
	if( ret ){
		DbgOut( "hs : GetMorphBaseNum : sh GetMorphElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMorphBaseInfo( E3DMORPHBASE* baseinfoptr, int arrayleng )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem** ppmorphelem = 0;
	ppmorphelem = (CShdElem**)malloc( sizeof( CShdElem* ) * arrayleng );
	if( !ppmorphelem ){
		DbgOut( "hs : GetMorphBaseInfo : ppmorphelem alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( ppmorphelem, sizeof( CShdElem* ) * arrayleng );

	int getnum = 0;
	ret = m_shandler->GetMorphElem( ppmorphelem, &getnum, arrayleng );
	if( ret ){
		DbgOut( "hs : GetMorphBaseInfo : sh GetMorphElem error !!!\n" );
		_ASSERT( 0 );
		free( ppmorphelem );
		return 1;
	}

	if( arrayleng != getnum ){
		DbgOut( "hs : GetMorphBaseInfo : arrayleng error !!!\n" );
		_ASSERT( 0 );
		free( ppmorphelem );
		return 1;
	}

	int morphcnt;
	for( morphcnt = 0; morphcnt < getnum; morphcnt++ ){
		E3DMORPHBASE* dstinfo = baseinfoptr + morphcnt;

		CShdElem* curmorph;
		curmorph = *( ppmorphelem + morphcnt );
		CMorph* morphptr;
		morphptr = curmorph->morph;
		if( !morphptr ){
			DbgOut( "hs : GetMorphBaseInfo : morph NULL error !!!\n" );
			_ASSERT( 0 );
			free( ppmorphelem );
			return 1;
		}

		CShdElem* baseelem;
		baseelem = morphptr->m_baseelem;
		_ASSERT( baseelem );

		CTreeElem2* basetelem;
		basetelem = (*m_thandler)( baseelem->serialno );
		_ASSERT( basetelem );

		sprintf_s( dstinfo->name, 256, basetelem->name );
		dstinfo->dispno = baseelem->serialno;
		dstinfo->boneno = morphptr->m_boneseri;
		dstinfo->targetnum = morphptr->m_targetnum;

	}

	free( ppmorphelem );


	return 0;
}
int CHandlerSet::GetMorphTargetInfo( int baseid, E3DMORPHTARGET* targetinfoptr, int arrayleng )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* morphelem = 0;
	ret = m_shandler->GetMorphElem( &morphelem, baseid );
	if( ret || !morphelem ){
		DbgOut( "hs : GetMorphTargetInfo : sh GetMorphElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMorph* morphptr;
	morphptr = morphelem->morph;
	_ASSERT( morphptr );

	if( arrayleng != morphptr->m_targetnum ){
		DbgOut( "hs : GetMorphTargetInfo : arrayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int targetcnt;
	for( targetcnt = 0; targetcnt < morphptr->m_targetnum; targetcnt++ ){

		E3DMORPHTARGET* dstinfo = targetinfoptr + targetcnt;

		TARGETPRIM* tprim = morphptr->m_ptarget + targetcnt;
		int targetseri;
		targetseri = tprim->seri;

		CTreeElem2* telem;
		telem = (*m_thandler)( targetseri );

		strcpy_s( dstinfo->name, 256, telem->name );
		dstinfo->dispno = targetseri;
	}

	return 0;
}
int CHandlerSet::GetMorphKeyNum( int motid, int boneid, int baseid, int targetid, int* keynumptr )
{
	*keynumptr = 0;

	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 0;
	}

	if( m_mhandler->m_kindnum <= 0 ){
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetMorphKeyNum : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (boneid <= 0) || (boneid >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetMorphKeyNum : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (baseid <= 0) || (baseid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetMorphKeyNum : baseid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (targetid <= 0) || (targetid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetMorphKeyNum : targetid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*m_mhandler)( boneid );
	_ASSERT( mcptr );
	if( !mcptr->IsJoint() ){
		DbgOut( "hs : GetMorphKeyNum : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr;
	miptr = mcptr->motinfo;
	if( !miptr ){
		DbgOut( "hs : GetMorphKeyNum : motioninfo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* baseelem;
	baseelem = (*m_shandler)( baseid );
	_ASSERT( baseelem );
	CShdElem* targetelem;
	targetelem = (*m_shandler)( targetid );
	_ASSERT( targetelem );

	int ret;
	ret = miptr->GetMMotAnimKeyAll( motid, baseelem, targetelem, 0, 0, keynumptr );
	if( ret ){
		DbgOut( "hs : GetMorphKeyNum : mi GetMMotAnimKeyAll error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	return 0;
}
int CHandlerSet::GetMorphKey( int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr, int arrayleng )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	if( m_mhandler->m_kindnum <= 0 ){
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetMorphKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (boneid <= 0) || (boneid >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetMorphKey : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (baseid <= 0) || (baseid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetMorphKey : baseid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (targetid <= 0) || (targetid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetMorphKey : targetid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*m_mhandler)( boneid );
	_ASSERT( mcptr );
	if( !mcptr->IsJoint() ){
		DbgOut( "hs : GetMorphKey : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr;
	miptr = mcptr->motinfo;
	if( !miptr ){
		DbgOut( "hs : GetMorphKey : motioninfo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* baseelem;
	baseelem = (*m_shandler)( baseid );
	_ASSERT( baseelem );
	CShdElem* targetelem;
	targetelem = (*m_shandler)( targetid );
	_ASSERT( targetelem );

	int ret;
	int getnum = 0;
	ret = miptr->GetMMotAnimKeyAll( motid, baseelem, targetelem, 0, 0, &getnum );
	if( ret ){
		DbgOut( "hs : GetMorphKey : mi GetMMotAnimKeyAll keynum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( getnum != arrayleng ){
		DbgOut( "hs : GetMorphKey : arrayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int tmpnum = 0;
	ret = miptr->GetMMotAnimKeyAll( motid, baseelem, targetelem, (E3DMORPHMOTION*)motioninfoptr, arrayleng, &tmpnum );
	if( ret || (tmpnum != arrayleng) ){
		DbgOut( "hs : GetMorphKey : mi GetMMotAnimKeyAll keyval error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetCurMorphInfo( int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	if( m_mhandler->m_kindnum <= 0 ){
		motioninfoptr->frameno = 0;
		motioninfoptr->blendrate = 0.0f;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	if( (boneid <= 0) || (boneid >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetCurMorphInfo : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (baseid <= 0) || (baseid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetCurMorphInfo : baseid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (targetid <= 0) || (targetid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetCurMorphInfo : targetid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*m_mhandler)( boneid );
	_ASSERT( mcptr );
	if( !mcptr->IsJoint() ){
		DbgOut( "hs : GetCurMorphInfo : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* baseelem;
	baseelem = (*m_shandler)( baseid );
	_ASSERT( baseelem );
	CShdElem* targetelem;
	targetelem = (*m_shandler)( targetid );
	_ASSERT( targetelem );

	motioninfoptr->frameno = mcptr->curframeno;//!!!!!!!!!

	CMMotElem* mmeptr = mcptr->GetCurMMotElem( baseelem );
	if( mmeptr ){
		motioninfoptr->blendrate = mmeptr->GetValue( targetelem );
	}else{
		motioninfoptr->blendrate = 0.0f;
	}

	return 0;
}

int CHandlerSet::CreateMorph( LPDIRECT3DDEVICE9 pdev, HWND apphwnd, int baseid, int boneid )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	
	if( (boneid <= 0) || (boneid >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs ; CreateMorph : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (baseid <= 0) || (baseid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : CreateMorph : baseid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	if( m_shandler->s2shd_leng <= 1 ){
		//データが無い時は、何もしない。
		return 0;
	}

	int jointnum = 0;
	ret = m_shandler->GetJointNum( &jointnum );
	_ASSERT( !ret );
	if( jointnum <= 0 ){
		DbgOut( "ジョイントが１つも無いので作成できません。" );
		_ASSERT( 1 );
		return 1;
	}

	CShdElem* baseelem = 0;
	CMotionCtrl* motionelem = 0;
	CTreeElem2* basetelem = 0;
	basetelem = (*m_thandler)( baseid );
	_ASSERT( basetelem );
	baseelem = (*m_shandler)( baseid );
	_ASSERT( baseelem );
	motionelem = (*m_mhandler)( boneid );
	_ASSERT( motionelem );
	

	if( baseelem->m_mtype != M_NONE ){
		DbgOut( "hs : AddMorphFolder : baseelem already in morph error !!!\n" );
		return 0;
	}

	ret = DestroyAllMotion();
	if( ret ){
		DbgOut( "hs : AddMorphFolder : DestroyAllMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//////////////////

	int serino = m_shandler->s2shd_leng;
	int srctype = SHDMORPH;

	int depth;	
	char newname[256];
	m_thandler->Start( serino - 1 );
	m_thandler->Begin();
	depth = m_thandler->GetCurDepth();
	ZeroMemory( newname, 256 );
	sprintf_s( newname, 256, "MFolder:%s", basetelem->name );

	ret = Add2Tree( newname, serino, SHDMORPH, depth, 0, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	m_thandler->End();


	CShdElem* curelem;
	curelem = (*m_shandler)( serino );
	_ASSERT( curelem );
	CMorph* morph;
	morph = curelem->morph;
	_ASSERT( morph );
	ret = morph->SetBaseParams( m_shandler, m_mhandler, baseelem, motionelem );
	if( ret ){
		DbgOut( "hs : AddMorphFolder : morph SetBaseParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* topelem;
	topelem = (*m_shandler)( 1 );
	_ASSERT( topelem );
	ret = curelem->PutToChain( topelem, 0 );
	if( ret ){
		DbgOut( "hs : AddMorphFolder : PutToChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->CalcDepth();
	if( ret ){
		DbgOut( "hs : AddMorphFolder : shandler CalcDepth 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = baseelem->LeaveFromChain();
	if( ret ){
		DbgOut( "hs : AddMorphFolder : base LeaveFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = baseelem->PutToChain( curelem, 1 );
	if( ret ){
		DbgOut( "hs : AddMorphFolder : PutToChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->CalcDepth();
	if( ret ){
		DbgOut( "hs : CreateBone : shandler CalcDepth 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	ret = m_mhandler->CopyChainFromShd( m_shandler );
	if( ret ){
		DbgOut( "hs : AddMorphFolder : mhandler CopyChainFromShd error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->CalcDepth();
	if( ret ){
		DbgOut( "hs : AddMorphFolder : mhandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Restore( pdev, apphwnd, -1, 1 );
	if( ret ){
		DbgOut( "hs : AddMorphFolder : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::AddMorphTarget( int baseid, int targetid )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	if( (baseid <= 0) || (baseid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : AddMorphTarget : baseid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (targetid <= 0) || (targetid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : AddMorphTarget : targetid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	CShdElem* morphelem = 0;
	ret = m_shandler->GetMorphElem( &morphelem, baseid );
	if( ret || !morphelem ){
		DbgOut( "hs : AddMorphTarget : sh GetMorphElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* baseelem;
	baseelem = (*m_shandler)( baseid );
	_ASSERT( baseelem );
	if( baseelem->m_mtype != M_BASE ){
		DbgOut( "hs : AddMorphTarget : base not in morph error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* targetelem;
	targetelem = (*m_shandler)( targetid );
	_ASSERT( targetelem );
	if( targetelem->m_mtype != M_NONE ){
		DbgOut( "hs : AddMorphTarget : target already in morph return 0 warning !!!\n" );
		//_ASSERT( 0 );
		return 0;
	}

	CMorph* morph;
	morph = morphelem->morph;
	_ASSERT( morph );
	int okflag = 0;
	okflag = morph->CheckTargetOK( targetelem );
	if( okflag == 0 ){
		DbgOut( "ベースとターゲットの構成が異なるので移動できません。" );
		_ASSERT( 0 );
		return 1;
	}

	ret = morph->AddMorphTarget( targetelem );
	if( ret ){
		DbgOut( "hs : AddMorphTarget : morph AddMorphTarget error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = targetelem->LeaveFromChain();// child情報は、そのままで、チェインから離す。
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = targetelem->PutToChain( morphelem );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->CalcDepth();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->CopyChainFromShd( m_shandler );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->CalcDepth();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	

	ret = m_shandler->DestroyBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "hs : AddMorphTarget : sh DestroyBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "hs : AddMorphTarget : sh CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* chkpar;
	chkpar = targetelem->parent;
	if( !chkpar || (chkpar->type != SHDMORPH) ){
		_ASSERT( 0 );
	}

	//targetelem->dispflag = 0;//!!!!!!!!!
	ret = m_shandler->SetDispFlag( 1 );
	_ASSERT( !ret );

	return 0;
}

int CHandlerSet::SetMorphKey( int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : SetMorphKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (boneid <= 0) || (boneid >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : SetMorphKey : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (baseid <= 0) || (baseid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetMorphKey : baseid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (targetid <= 0) || (targetid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetMorphKey : targetid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*m_mhandler)( boneid );
	_ASSERT( mcptr );
	if( !mcptr->IsJoint() ){
		DbgOut( "hs : SetMorphKey : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr;
	miptr = mcptr->motinfo;
	if( !miptr ){
		DbgOut( "hs : SetMorphKey : motioninfo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* baseelem;
	baseelem = (*m_shandler)( baseid );
	_ASSERT( baseelem );
	CShdElem* targetelem;
	targetelem = (*m_shandler)( targetid );
	_ASSERT( targetelem );

	int ret;
	ret = miptr->SetMMotValue( motid, motioninfoptr->frameno, baseelem, targetelem, motioninfoptr->blendrate );
	if( ret ){
		DbgOut( "hs : SetMorphKey : mi SetMMotValue error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::DestroyMorphKey( int motid, int boneid, int frameno, int baseid )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 0;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : DestroyMorphKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (boneid <= 0) || (boneid >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : DestroyMorphKey : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (baseid <= 0) || (baseid >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : DestroyMorphKey : baseid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*m_mhandler)( boneid );
	_ASSERT( mcptr );
	if( !mcptr->IsJoint() ){
		DbgOut( "hs : DestroyMorphKey : boneid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr;
	miptr = mcptr->motinfo;
	if( !miptr ){
		DbgOut( "hs : DestroyMorphKey : motioninfo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* baseelem;
	baseelem = (*m_shandler)( baseid );
	_ASSERT( baseelem );

	int ret;
	if( frameno < 0 ){
		CMMotKey* mmkptr;
		CMMotKey* nextmmk;
		mmkptr = miptr->GetFirstMMotKey( motid );
		while( mmkptr ){
			nextmmk = mmkptr->next;
			ret = miptr->DeleteMMotKey( motid, mmkptr->frameno, baseelem );
			if( ret ){
				DbgOut( "hs : DestroyMorphKey : mi DeleteMMotKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			mmkptr = nextmmk;
		}
	}else{
		ret = miptr->DeleteMMotKey( motid, frameno, baseelem );
		if( ret ){
			DbgOut( "hs : DestroyMorphKey : mi DeleteMMotKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CHandlerSet::GetToon1Material( int partno, E3DTOON1MATERIAL* toon1ptr, int arrayleng )
{
	if( !m_thandler || !m_shandler || !m_mhandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetToon1Material : paratno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	int getnum = 0;
	ret = selem->GetToon1Material( toon1ptr, arrayleng, &getnum );
	if( ret ){
		DbgOut( "hs : GetToon1Material : se GetToon1Material error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( arrayleng != getnum ){
		DbgOut( "hs : GetToon1Material : arrayleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetToon1Name( int partno, char* oldname, char* newname )
{
	if( !m_thandler || !m_shandler || !m_mhandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetToon1Name : paratno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->SetToon1Name( oldname, newname );
	if( ret ){
		DbgOut( "hs : SetToon1Name : se SetToon1Name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::SetToon1Diffuse( int partno, char* matname, RDBColor3f diffuse )
{
	if( !m_thandler || !m_shandler || !m_mhandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetToon1Diffuse : paratno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->SetToon1Diffuse( matname, diffuse );
	if( ret ){
		DbgOut( "hs : SetToon1Diffuse : se SetToon1Diffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetToon1Specular( int partno, char* matname, RDBColor3f specular )
{
	if( !m_thandler || !m_shandler || !m_mhandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetToon1Specular : paratno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->SetToon1Specular( matname, specular );
	if( ret ){
		DbgOut( "hs : SetToon1Specular : se SetToon1Specular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	


	return 0;
}
int CHandlerSet::SetToon1Ambient( int partno, char* matname, RDBColor3f ambient )
{
	if( !m_thandler || !m_shandler || !m_mhandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetToon1Ambient : paratno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->SetToon1Ambient( matname, ambient );
	if( ret ){
		DbgOut( "hs : SetToon1Ambient : se SetToon1Ambient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::SetToon1NL( int partno, char* matname, float darknl, float brightnl )
{
	if( !m_thandler || !m_shandler || !m_mhandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetToon1NL : paratno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->SetToon1NL( matname, darknl, brightnl );
	if( ret ){
		DbgOut( "hs : SetToon1NL : se SetToon1NL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::SetToon1Edge0( int partno, char* matname, RDBColor3f col, int validflag, int invflag, float width )
{
	if( !m_thandler || !m_shandler || !m_mhandler ){
		return 0;
	}

	if( (partno < 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetToon1Edge0 : paratno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );

	int ret;
	ret = selem->SetToon1Edge0( matname, col, validflag, invflag, width );
	if( ret ){
		DbgOut( "hs : SetToon1Edge0 : se SetToon1Edge0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}


int CHandlerSet::LoadGPFile( char* fname, int* animnoptr )
{
	int ret;
	CGPFile gpfile;

	ret = gpfile.LoadGPFile( fname, m_gpah, animnoptr );
	if( ret ){
		DbgOut( "hs : LoadGPFile : gpfile LoadGPFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::DestroyGPAnim( int animno )
{
	int ret;
	ret = m_gpah->DestroyAnim( animno );
	if( ret ){
		DbgOut( "hs : DestroyGPAnim : gpah DestroyAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SaveGPFile( char* fname, int animno )
{
	int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : SaveGPFile : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CGPFile gpfile;
	ret = gpfile.WriteGPFile( fname, anim );
	if( ret ){
		DbgOut( "hs SaveGPFile : gpfile WriteGPFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetGPAnimNo( int animno )
{

	int ret;
	ret = m_gpah->SetCurrentAnim( animno );
	if( ret ){
		DbgOut( "hs SetGPAnimNo : gpah SetCurrentAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_gpd.m_gpe = m_gpah->m_curgpe;
	m_gpd.CalcMatWorld();

	return 0;
}
int CHandlerSet::GetGPAnimNo( int* animnoptr )
{
	int ret;
	ret = m_gpah->GetCurrentAnim( animnoptr );
	if( ret ){
		DbgOut( "hs GetGPAnimNo : gpah GetCurrentAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetGPFrameNo( int animno, int frameno, int isfirst )
{
	int ret;
	ret = m_gpah->SetAnimFrameNo( animno, frameno, isfirst );
	if( ret ){
		DbgOut( "hs SetGPFrameNo : gpah SetAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_gpd.m_gpe = m_gpah->m_curgpe;
	m_gpd.CalcMatWorld();

	return 0;
}
int CHandlerSet::GetGPFrameNo( int* animnoptr, int* framenoptr )
{
	int ret;
	ret = m_gpah->GetAnimFrameNo( animnoptr, framenoptr );
	if( ret ){
		DbgOut( "hs GetGPFrameNo : gpah GetAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetNewGP()
{
	int ret;
	ret = m_gpah->SetNewGP();
	if( ret ){
		DbgOut( "hs SetNewGP : gpah SetNewGP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_gpd.m_gpe = m_gpah->m_curgpe;
	m_gpd.CalcMatWorld();

	return 0;
}
int CHandlerSet::SetNextGPAnimFarmeNo( int animno, int nextanimno, int nextframeno, int befframeno )
{
	int ret;
	ret = m_gpah->SetNextAnimFrameNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "hs SetNextGPAnimFrameNo : gpah SetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetNextGPAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr )
{
	int ret;
	ret = m_gpah->GetNextAnimFrameNo( animno, nextanimptr, nextframenoptr );
	if( ret ){
		DbgOut( "hs GetNextGPAnimFrameNo : gpah GetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::CreateNewGPAnim( char* aname, int animtype, int framenum, int* animnoptr )
{
	*animnoptr = -1;

//	int ret;
	CGPAnim* newanim = 0;
	newanim = m_gpah->AddAnim( aname, animtype, framenum, 0 );
	if( !newanim ){
		DbgOut( "hs CreateNewGPAnim : gpah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*animnoptr = newanim->m_motkind;

	return 0;
}
int CHandlerSet::SetGPFrameNum( int animno, int newtotal )
{
	int ret;
	ret = m_gpah->SetTotalFrame( animno, newtotal );
	if( ret ){
		DbgOut( "hs SetGPFrameNum : gpah SetTotalFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetGPFrameNum( int animno, int* framenumptr )
{
	*framenumptr = 0;

	//int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetGPFrameNum : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*framenumptr = anim->m_maxframe + 1;

	return 0;
}
int CHandlerSet::SetGPAnimName( int animno, char* aname )
{
	int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetGPAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->SetAnimName( aname );
	if( ret ){
		DbgOut( "hs SetGPAnimName : anim SetAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::GetGPAnimName( int animno, char* dstname, int arrayleng )
{
	//int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetGPAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( dstname, arrayleng, anim->m_animname );

	return 0;
}
int CHandlerSet::SetGPAnimType( int animno, int type )
{
	//int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetGPAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	anim->m_animtype = type;

	return 0;
}
int CHandlerSet::GetGPAnimType( int animno, int* typeptr )
{
	*typeptr = 0;

	//int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetGPAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	*typeptr = anim->m_animtype;

	return 0;
}
int CHandlerSet::SetGPKey( int animno, int frameno, E3DGPSTATE gpstate )
{
	int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetGPKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	GPELEM gpe;
	gpe.pos = gpstate.pos;
	gpe.rot = gpstate.rot;
	gpe.ongmode = gpstate.ongmode;
	gpe.rayy = gpstate.rayy;
	gpe.rayleng = gpstate.rayleng;
	gpe.offsety = gpstate.offsety;
	int interp;
	interp = gpstate.interp;

	gpe.e3dpos = gpe.pos;
	gpe.e3drot = gpe.rot;

	ret = anim->SetGPKey( frameno, gpe, interp );
	if( ret ){
		DbgOut( "hs : SetGPKey : anim SetGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetGPKey( int animno, int frameno, E3DGPSTATE* gpstateptr, int* existptr )
{
	*existptr = 0;

	int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetGPKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	GPELEM gpe;
	ZeroMemory( &gpe, sizeof( GPELEM ) );
	int interp = 0;
	ret = anim->GetGPAnim( &gpe, &interp, frameno, existptr );
	if( ret ){
		DbgOut( "hs GetGPKey : anim GetGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	gpstateptr->pos = gpe.e3dpos;
	gpstateptr->rot = gpe.e3drot;
	gpstateptr->ongmode = gpe.ongmode;
	gpstateptr->rayy = gpe.rayy;
	gpstateptr->rayleng = gpe.rayleng;
	gpstateptr->offsety = gpe.offsety;
	gpstateptr->interp = interp;

	return 0;
}
int CHandlerSet::DeleteGPKey( int animno, int frameno )
{
	int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : DeleteGPKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->DeleteGPKey( frameno );
	if( ret ){
		DbgOut( "hs DeleteGPKey : anim DeleteGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetGPKeyframeNoRange( int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr )
{
	int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetGPKeyframeNoRange : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetGPKeyframeNoRange( start, end, framearray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "hs GetGPKeyframeNoRange : anim GetGPKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::CalcGPFullFrame( int animno )
{
	int ret;
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : CalcGPFullFrame : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* ghs = 0;
	CShdHandler* lpsh = 0;
	CMotHandler* lpmh = 0;
	D3DXMATRIX* lpmw = 0;
	ghs = GetHandlerSet( anim->m_groundhsid );
	if( ghs ){
		lpsh = ghs->m_shandler;
		lpmh = ghs->m_mhandler;
		lpmw = &( ghs->m_gpd.m_matWorld );
	}

	ret = anim->CalcFrameData( lpsh, lpmh, lpmw );
	if( ret ){
		DbgOut( "hs CalcGPFullFrame : anim CalcFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_gpd.m_gpe = m_gpah->m_curgpe;
	m_gpd.CalcMatWorld();

	return 0;
}
int CHandlerSet::SetGPGroundHSID( int animno, int ghsid )
{
	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetGPGroundHSID : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	anim->m_groundhsid = ghsid;

	return 0;
}
int CHandlerSet::GetGPGroundHSID( int animno, int* ghsidptr )
{
	*ghsidptr = 0;

	CGPAnim* anim;
	anim = m_gpah->GetGPAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetGPGroundHSID : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*ghsidptr = anim->m_groundhsid;

	return 0;
}

int CHandlerSet::LoadMOEFile( char* filename, int* animnoptr )
{
	if( !m_thandler || !m_shandler || !m_mch || !m_moeah ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEFile moefile;
	ret = moefile.LoadMOEFile( m_thandler, m_shandler, filename, m_moeah, animnoptr );
	if( ret || (*animnoptr < 0) ){
		DbgOut( "hs : LoadMOEFile : moefile LoadMOEFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_moeah->CalcFrameData( *animnoptr, m_shandler, m_mhandler, m_mch );
	if( ret ){
		DbgOut( "hs : LoadMOEFile : moeah CalcFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::DestroyMOEAnim( int animno )
{
	int ret;
	ret = m_moeah->DestroyAnim( animno );
	if( ret ){
		DbgOut( "hs : DestroyMOEAnim : moeah DestroyAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SaveMOEFile( char* filename, int animno )
{
	int ret;

	if( !m_thandler || !m_shandler || !m_moeah ){
		_ASSERT( 0 );
		return 1;
	}

	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : SaveMOEFile : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMOEFile moefile;
	ret = moefile.WriteMOEFile( m_thandler, m_shandler, filename, anim );
	if( ret ){
		DbgOut( "hs : SaveMOEFile : moefile WriteMOEFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMOEAnimNo( int animno )
{
	int ret;
	ret = m_moeah->SetCurrentAnim( animno );
	if( ret ){
		DbgOut( "hs : SetMOEAnimNo : moeah SetCurrentAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOEAnimNo( int* animnoptr )
{
	int ret;
	ret = m_moeah->GetCurrentAnim( animnoptr );
	if( ret ){
		DbgOut( "hs : GetMOEAnimNo : moeah GetCurrentAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMOEFrameNo( int animno, int frameno, int isfirst )
{
	if( !m_moeah || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_moeah->SetAnimFrameNo( m_shandler, m_mhandler, animno, frameno, isfirst );
	if( ret ){
		DbgOut( "hs : SetMOEFrameNo : moeah SetAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOEFrameNo( int* animnoptr, int* framenoptr )
{

	int ret;
	ret = m_moeah->GetAnimFrameNo( animnoptr, framenoptr );
	if( ret ){
		DbgOut( "hs : GetMOEFrameNo : moeah GetAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetNewMOE()
{
	int ret;
	ret = m_moeah->SetNewMOE( m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "hs : SetNewMOE : moeah SetNewMOE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetNextMOEAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno )
{
	int ret;
	ret = m_moeah->SetNextAnimFrameNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "hs : SetNextMOEAnimFrameNo : moeah SetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetNextMOEAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr )
{
	int ret;
	ret = m_moeah->GetNextAnimFrameNo( animno, nextanimptr, nextframenoptr );
	if( ret ){
		DbgOut( "hs : GetNextMEAnimFrameNo : moeah GetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::CreateNewMOEAnim( char* animname, int animtype, int framenum, int* animnoptr )
{
	*animnoptr = -1;

	CMOEAnim* anim;
	anim = m_moeah->AddAnim( m_shandler, animname, animtype, framenum, 0 );
	if( !anim ){
		DbgOut( "hs : CreateNewMOEAnim : moeah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*animnoptr = anim->m_motkind;

	return 0;
}
int CHandlerSet::SetMOEFrameNum( int animno, int newtotal )
{
	int ret;
	ret = m_moeah->SetTotalFrame( m_shandler, animno, newtotal );
	if( ret ){
		DbgOut( "hs : SetMOEFrameNum : moeah SetTotalFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOEFrameNum( int animno, int* framenumptr )
{
	*framenumptr = 0;

	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEFrameNum : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*framenumptr = anim->m_maxframe + 1;

	return 0;
}
int CHandlerSet::SetMOEAnimName( int animno, char* srcname )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetMOEAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = anim->SetAnimName( srcname );
	if( ret ){
		DbgOut( "hs : SetMOEAnimName : anim SetAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOEAnimName( int animno, char* dstname, int arrayleng )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = anim->GetAnimName( dstname, arrayleng );
	if( ret ){
		DbgOut( "hs : GetMOEAnimName : anim GetAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMOEAnimType( int animno, int type )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetMOEAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	anim->m_animtype = type;

	return 0;
}
int CHandlerSet::GetMOEAnimType( int animno, int* typeptr )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*typeptr = anim->m_animtype;

	return 0;
}
int CHandlerSet::GetMOEKeyframeNoRange( int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr )
{

	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEKeyFrameNoRange : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = anim->GetMOEKeyframeNoRange( start, end, framearray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "hs : GetMOEKeyframeNoRange : anim GetMOEKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOEElemNum( int animno, int frameno, int* moeenumptr, int* existkeyptr )
{
	*moeenumptr = 0;
	*existkeyptr = 0;

	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEElemNum : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEKey* moek = 0;
	ret = anim->ExistMOEKey( frameno, &moek );
	if( ret ){
		DbgOut( "hs : GetMOEElemNum : anim ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( moek ){
		*moeenumptr = moek->m_moeenum;
		*existkeyptr = 1;
	}else{
		*moeenumptr = 0;
		*existkeyptr = 0;
	}

	return 0;
}
int CHandlerSet::GetMOEElem( int animno, int frameno, int moeeindex, MOEELEM* moeeptr )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEElem : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEKey* moek = 0;
	ret = anim->ExistMOEKey( frameno, &moek );
	if( ret ){
		DbgOut( "hs : GetMOEElem : anim ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !moek ){
		DbgOut( "hs : GetMOEElem : key not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (moeeindex < 0) || (moeeindex >= moek->m_moeenum) ){
		DbgOut( "hs : GetMOEElem : moeeindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*moeeptr = *( moek->m_pmoee + moeeindex );

	return 0;
}
int CHandlerSet::AddMOEElem( int animno, int frameno, MOEELEM moee )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : AddMOEElem : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = anim->AddMOEE( frameno, &moee );
	if( ret ){
		DbgOut( "hs : AddMOEElem : anim AddMOEE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::DeleteMOEElem( int animno, int frameno, int moeeindex )
{

	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : DeleteMOEElem : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = anim->DeleteMOEE( frameno, moeeindex );
	if( ret ){
		DbgOut( "hs : DeleteMOEElem : anim DeleteMOEE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMOEKeyIdling( int animno, int frameno, int motid )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetMOEKeyIdling : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	char motname[256];
	ZeroMemory( motname, sizeof( char ) * 256 );
	ret = m_mhandler->GetMotionName( motid, motname );
	if( ret || (motname[0] == 0) ){
		DbgOut( "hs : SetMOEKeyIdling : mh GetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->SetKeyIdlingName( frameno, motname );
	if( ret ){
		DbgOut( "hs : SetMOEKeyIdling : anim SetKeyIdlingName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOEKeyIdling( int animno, int frameno, int* motidptr )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEKeyIdling : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEKey* moek = 0;
	ret = anim->ExistMOEKey( frameno, &moek );
	if( ret ){
		DbgOut( "hs : GetMOEKeyIdling : anim ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !moek ){
		*motidptr = -1;
		_ASSERT( 0 );
		return 0;
	}

	*motidptr = m_mhandler->GetMotionIDByName( moek->m_idlingname );

	return 0;
}
int CHandlerSet::DeleteMOEKey( int animno, int frameno )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : DeleteMOEKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = anim->DeleteMOEKey( frameno );
	if( ret ){
		DbgOut( "hs : DeleteMOEKey : anim DeleteMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::CalcMOEFullFrame( int animno )
{
	if( !m_shandler || !m_mhandler || !m_mch ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_moeah->CalcFrameData( animno, m_shandler, m_mhandler, m_mch );
	if( ret ){
		DbgOut( "hs : CalcMOEFullFrame : moeah CalcFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::CreateAndSaveTempMaterial()
{
	if( !m_shandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_shandler->CreateAndSaveTempMaterial();
	if( ret ){
		DbgOut( "hs : CreateAndSaveTempMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::RestoreAndDestroyTempMaterial()
{
	if( !m_shandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_shandler->RestoreAndDestroyTempMaterial();
	if( ret ){
		DbgOut( "hs : RestoreAndDestroyTempMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetMaterialFromInfElem( int jointno, int partno, int vertno, E3DCOLOR3UC* infcolarray )
{
	if( !m_shandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_shandler->SetMaterialFromInfElem( jointno, partno, vertno, infcolarray );
	if( ret ){
		DbgOut( "hs : SetMaterialFromInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GlobalToLocalParBone( int motid, int frameno, int boneno, int flag, 
	CQuaternion2* srcqptr, CQuaternion2* dstqptr, D3DXVECTOR3* srcvec, D3DXVECTOR3* dstvec )
{
	if( (flag & 2) && ( !srcvec || !dstvec ) ){
		DbgOut( "hs : GlobalToLocalBone : vecptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) || (m_mhandler->m_kindnum <= 0) ){
		DbgOut( "hs : GlobalToLocalBone : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int framenum = 0;
	framenum = *( m_mhandler->m_motnum + motid );
	if( (frameno < 0) || (frameno >= framenum) ){
		DbgOut( "hs : GlobalToLocalBone : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GlobalToLocalBone : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* curse = 0;
	curse = (*m_shandler)( boneno );
	if( curse->IsJoint() == 0 ){
		DbgOut( "hs : GlobalToLocalBone : not bone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* parse = 0;
	parse = m_shandler->FindUpperJoint( curse );

	CQuaternion parq( 1.0f, 0.0f, 0.0f, 0.0f );
	D3DXMATRIX parmat;
	D3DXMatrixIdentity( &parmat );

	int ret;
	if( parse ){
		if( flag & 1 ){
			ret = m_mhandler->GetBoneQ( m_shandler, parse->serialno, motid, frameno, 1, &parq );
			if( ret ){
				DbgOut( "hs : GlobalToLocalBone : mh GetBoneQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			CQuaternion totalq( 1.0f, 0.0f, 0.0f, 0.0f );
			totalq = m_gpd.m_q * parq;
			CQuaternion invq;
			totalq.inv( &invq );
			CQuaternion2 invq2( invq.w, invq.x, invq.y, invq.z, 0.0f );
			*dstqptr = invq2 * *srcqptr;
			dstqptr->twist = srcqptr->twist;

		}

		if( flag & 2 ){
			ret = m_mhandler->HuGetBoneMatrix( parse->serialno, motid, frameno, &parmat );
			if( ret ){
				DbgOut( "hs : GlobalToLocalBone : mh HuGetBoneMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			D3DXMATRIX totalmat;
			D3DXMatrixIdentity( &totalmat );
			totalmat = m_gpd.m_matWorld * parmat;
			D3DXMATRIX invw;
			D3DXMatrixInverse( &invw, NULL, &totalmat );
			D3DXVec3TransformCoord( dstvec, srcvec, &invw );
		}
	}else{
		if( flag & 1 ){
			CQuaternion totalq( 1.0f, 0.0f, 0.0f, 0.0f );
			totalq = m_gpd.m_q;
			CQuaternion invq;
			totalq.inv( &invq );
			CQuaternion2 invq2( invq.w, invq.x, invq.y, invq.z, 0.0f );
			*dstqptr = invq2 * *srcqptr;
			dstqptr->twist = srcqptr->twist;

		}

		if( flag & 2 ){
			D3DXMATRIX totalmat;
			D3DXMatrixIdentity( &totalmat );
			totalmat = m_gpd.m_matWorld;
			D3DXMATRIX invw;
			D3DXMatrixInverse( &invw, NULL, &totalmat );
			D3DXVec3TransformCoord( dstvec, srcvec, &invw );
		}
	}



	return 0;
}
int CHandlerSet::LocalToGlobalParBone( int motid, int frameno, int boneno, int flag, 
	CQuaternion2* srcqptr, CQuaternion2* dstqptr, D3DXVECTOR3* srcvec, D3DXVECTOR3* dstvec )
{
	if( (flag & 2) && ( !srcvec || !dstvec ) ){
		DbgOut( "hs : GlobalToLocalBone : vecptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) || (m_mhandler->m_kindnum <= 0) ){
		DbgOut( "hs : LocalToGlobalBone : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int framenum = 0;
	framenum = *( m_mhandler->m_motnum + motid );
	if( (frameno < 0) || (frameno >= framenum) ){
		DbgOut( "hs : LocalToGlobalBone : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : LocalToGlobalBone : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* curse = 0;
	curse = (*m_shandler)( boneno );
	if( curse->IsJoint() == 0 ){
		DbgOut( "hs : LocalToGlobalBone : not bone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* parse = 0;
	parse = m_shandler->FindUpperJoint( curse );

	CQuaternion parq( 1.0f, 0.0f, 0.0f, 0.0f );
	D3DXMATRIX parmat;
	D3DXMatrixIdentity( &parmat );

	int ret;
	if( parse ){
		if( flag & 1 ){
			ret = m_mhandler->GetBoneQ( m_shandler, parse->serialno, motid, frameno, 1, &parq );
			if( ret ){
				DbgOut( "hs : LocalToGlobalBone : mh GetBoneQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			CQuaternion totalq( 1.0f, 0.0f, 0.0f, 0.0f );
			totalq = m_gpd.m_q * parq;
			CQuaternion2 t2( totalq.w, totalq.x, totalq.y, totalq.z, 0.0f );
			*dstqptr = t2 * *srcqptr;
			dstqptr->twist = srcqptr->twist;


		}

		if( flag & 2 ){
			ret = m_mhandler->HuGetBoneMatrix( parse->serialno, motid, frameno, &parmat );
			if( ret ){
				DbgOut( "hs : LocalToGlobalBone : mh HuGetBoneMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			D3DXMATRIX totalmat;
			D3DXMatrixIdentity( &totalmat );
			totalmat = m_gpd.m_matWorld * parmat;
			D3DXVec3TransformCoord( dstvec, srcvec, &totalmat );


		}
	}else{
		if( flag & 1 ){
			CQuaternion totalq( 1.0f, 0.0f, 0.0f, 0.0f );
			totalq = m_gpd.m_q;
			CQuaternion2 t2( totalq.w, totalq.x, totalq.y, totalq.z, 0.0f );
			*dstqptr = t2 * *srcqptr;
			dstqptr->twist = srcqptr->twist;
		}

		if( flag & 2 ){
			D3DXMATRIX totalmat;
			D3DXMatrixIdentity( &totalmat );
			totalmat = m_gpd.m_matWorld;
			D3DXVec3TransformCoord( dstvec, srcvec, &totalmat );
		}
	}


	return 0;
}

int CHandlerSet::GetInfElemSymParams( int partno, int vertno, int* symtypeptr, float* distptr )
{
	int ret;

	ret = m_shandler->GetInfElemSymParams( partno, vertno, symtypeptr, distptr );
	if( ret ){
		DbgOut( "hs : GetInfELemSymParams : sh GetInfElemSymParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::DestroyMotion( int motid, IDCHANGE* idchgptr, int idnum )
{
	int ret;

	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : DestroyMotion motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_mhandler->m_kindnum <= 0 ){
		int idno;
		for( idno = 0; idno < idnum; idno++ ){
			( idchgptr + idno )->newid = -1;
		}
		return 0;
	}

	if( (m_mhandler->m_kindnum == 1) && (motid == 0) ){
		int idno;
		for( idno = 0; idno < idnum; idno++ ){
			( idchgptr + idno )->newid = -1;
		}

		DestroyAllMotion();

	}else{
		int idno;
		for( idno = 0; idno < idnum; idno++ ){
			int befid = ( idchgptr + idno )->befid;

			if( (befid >= 0) && (befid < m_mhandler->m_kindnum) ){
				if( befid < motid ){
					( idchgptr + idno )->newid = befid;
				}else if( motid == befid ){
					( idchgptr + idno )->newid = -1;
				}else{
					( idchgptr + idno )->newid = befid - 1;
				}
			}else{
				( idchgptr + idno )->newid = -1;
			}
		}


		EnterCriticalSection( &m_crit_addmotion );//######## start crit

		ret = m_mhandler->DestroyMotionObj( motid );		
		_ASSERT( !ret );

		if( m_mch ){
			int delsetno;
			delsetno = m_mch->MotID2Setno( motid );
			if( delsetno < 0 ){
				_ASSERT( 0 );
				LeaveCriticalSection( &m_crit_addmotion );//###### end crit
				return 0;//!!!!!!!!!!!!!
			}
			ret = m_mch->DeleteMCElem( delsetno );
			if( ret ){
				DbgOut( "hs : DestroyMotion : mch DeleteMCElem error !!!\n" );
				_ASSERT( 0 );
				LeaveCriticalSection( &m_crit_addmotion );//###### end crit
				return 1;
			}
		}

		LeaveCriticalSection( &m_crit_addmotion );//###### end crit



	}

	return 0;
}

int CHandlerSet::GetIKTargetPos( int boneno, D3DXVECTOR3* posptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetIKTargetPos : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetIKTargetPos : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = selem->m_iktargetpos;

	return 0;
}
int CHandlerSet::GetIKTargetState( int boneno, int* stateptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetIKTargetState : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetIKTargetState : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*stateptr = selem->m_iktarget;

	return 0;
}
int CHandlerSet::GetIKTargetCnt( int boneno, int* cntptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetIKTargetCnt : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetIKTargetCnt : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*cntptr = selem->m_iktargetcnt;

	return 0;
}
int CHandlerSet::GetIKTargetLevel( int boneno, int* levelptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetIKTargetLevel : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetIKTargetLevel : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*levelptr = selem->m_iktargetlevel;

	return 0;
}
int CHandlerSet::GetIKLevel( int boneno, int* levelptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetIKLevel : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetIKLevel : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	*levelptr = selem->m_iklevel;

	return 0;
}
int CHandlerSet::SetIKTargetPos( int boneno, D3DXVECTOR3 srcpos )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetIKTargetPos : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : SetIKTargetPos : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_iktargetpos = srcpos;

	return 0;
}
int CHandlerSet::SetIKTargetState( int boneno, int srcstate )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetIKTargetState : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : SetIKTargetState : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_iktarget = srcstate;

	return 0;
}
int CHandlerSet::SetIKTargetCnt( int boneno, int srccnt )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetIKTargetCnt : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : SetIKTargetCnt : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_iktargetcnt = srccnt;

	return 0;
}
int CHandlerSet::SetIKTargetLevel( int boneno, int srclevel )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetIKTargetLevel : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : SetIKTargetLevel : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_iktargetlevel = srclevel;

	return 0;
}
int CHandlerSet::SetIKLevel( int boneno, int srclevel )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetIKLevel : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : SetIKLevel : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	selem->m_iklevel = srclevel;

	return 0;
}

int CHandlerSet::GetEulerLimit( int boneno, EULERLIMIT* elptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetEulerLimit : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetEulerLimit : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DVEC3 lim0, lim1;
	int ret;
	ret = selem->GetBoneLim01( &lim0, &lim1 );
	if( ret ){
		DbgOut( "hs ; GetEulerLimit : se GetBoneLim01 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int igflag;
	ret = selem->GetIgnoreLim( &igflag );
	if( ret ){
		DbgOut( "hs ; GetEulerLimit : se GetIgnoreLim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	elptr->ignoreflag = igflag;
	elptr->lim0.x = (float)lim0.x;
	elptr->lim0.y = (float)lim0.y;
	elptr->lim0.z = (float)lim0.z;
	elptr->lim1.x = (float)lim1.x;
	elptr->lim1.y = (float)lim1.y;
	elptr->lim1.z = (float)lim1.z;

	return 0;
}
int CHandlerSet::SetEulerLimit( int boneno, EULERLIMIT srcel )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs :  : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs :  : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	DVEC3 lim0, lim1;
	lim0.x = (double)srcel.lim0.x;
	lim0.y = (double)srcel.lim0.y;
	lim0.z = (double)srcel.lim0.z;
	lim1.x = (double)srcel.lim1.x;
	lim1.y = (double)srcel.lim1.y;
	lim1.z = (double)srcel.lim1.z;

	int ret;
	ret = selem->SetBoneLim01( &lim0, &lim1 );
	if( ret ){
		DbgOut( "hs ; SetEulerLimit : se SetBoneLim01 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = selem->SetIgnoreLim( srcel.ignoreflag );
	if( ret ){
		DbgOut( "hs ; SetEulerLimit : se SetIgnoreLim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::CalcEuler( int motid, int zakind )
{
	
	int ret;
	ret = m_mhandler->CalcMPEuler( m_shandler, motid, zakind );
	_ASSERT( !ret );

	return 0;
}
int CHandlerSet::GetBoneEuler( int boneno, int motid, int frameno, D3DXVECTOR3* eulptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetBoneEuler : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionCtrl* mcptr = (*m_mhandler)( boneno );
	_ASSERT( mcptr );
	if( !(mcptr->IsJoint()) ){
		DbgOut( "hs : GetBoneEuler : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );

	int ret;
	CMotionPoint2 mp;
	mp.m_frameno = frameno;
	int hasmpflag = 0;
	ret = mcptr->CalcMotionPointOnFrame( selem, &mp, motid, frameno, &hasmpflag, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "hs : GetBoneEuler : mc CalcMotionPointOnFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*eulptr = mp.m_eul;

	return 0;
}
int CHandlerSet::GetZa4Elem( int boneno, ZA4ELEM* z4eptr )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetZa4Elem : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetZa4Elem : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion za4q;
	ret = selem->GetZa4Q( &za4q );
	_ASSERT( !ret );
	z4eptr->za4q.x = za4q.x;
	z4eptr->za4q.y = za4q.y;
	z4eptr->za4q.z = za4q.z;
	z4eptr->za4q.w = za4q.w;

	ret = selem->GetZa4Type( &(z4eptr->zatype) );
	_ASSERT( !ret );

	ret = selem->GetZa4RotAxis( &(z4eptr->rotaxis) );
	_ASSERT( !ret );

	return 0;
}
int CHandlerSet::SetZa4Elem( int boneno, ZA4ELEM srcz4e )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetZa4Elem : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : SetZa4Elem : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion za4q;
	za4q.x = srcz4e.za4q.x;
	za4q.y = srcz4e.za4q.y;
	za4q.z = srcz4e.za4q.z;
	za4q.w = srcz4e.za4q.w;
	ret = selem->SetZa4Q( za4q );
	_ASSERT( !ret );

	ret = selem->SetZa4Type( srcz4e.zatype );
	_ASSERT( !ret );

	ret = selem->SetZa4RotAxis( srcz4e.rotaxis );
	_ASSERT( !ret );

	return 0;
}
int CHandlerSet::CalcZa4LocalQ( int motid, int frameno )
{
	int ret;
	ret = m_shandler->CalcZa4LocalQ( m_mhandler, motid, frameno );
	if( ret ){
		DbgOut( "hs : CalcZa4LocalQ : sh CalcZa4LocalQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::GetZa4LocalQ( int boneno, E3DQ* dstq )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetZa4LocalQ : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetZa4LocalQ : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion q;
	ret = selem->GetZa4LocalQ( &q );
	if( ret ){
		DbgOut( "hs : GetZa4LocalQ : se GetZa4LocalQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	dstq->x = q.x;
	dstq->y = q.y;
	dstq->z = q.z;
	dstq->w = q.w;



	return 0;
}
int CHandlerSet::GetZa3IniQ( int boneno, E3DQ* dstq )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetZa3IniQ : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion q;
	ret = m_shandler->GetInitialBoneQ( boneno, &q );
	if( ret ){
		DbgOut( "hs : GetZa3IniQ : sh GetInitialBoneQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	dstq->x = q.x;
	dstq->y = q.y;
	dstq->z = q.z;
	dstq->w = q.w;

	return 0;
}

int CHandlerSet::GetBoneAxisQ( int boneno, E3DQ* dstq )
{
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetBoneAxisQ : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !(selem->IsJoint()) ){
		DbgOut( "hs : GetBoneAxisQ : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	CQuaternion q;
	ret = selem->GetBoneAxisQ( &q );
	if( ret ){
		DbgOut( "hs : GetBoneAxisQ : sh GetBoneAxisQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	dstq->x = q.x;
	dstq->y = q.y;
	dstq->z = q.z;
	dstq->w = q.w;

	return 0;
}
int CHandlerSet::SetBoneAxisQ( int boneno, int motid, int frameno, int zakind )
{
	int ret;
	ret = m_mhandler->SetBoneAxisQ( m_shandler, boneno, zakind, motid, frameno );
	if( ret ){
		DbgOut( "hs : SetBoneAxisQ : mh SetBoneAxisQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CHandlerSet::GetBoneTra( int boneno, int motid, int frameno, D3DXVECTOR3* traptr, int* existptr )
{
	*existptr = 0;

	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetBoneTra : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionCtrl* mcptr = (*m_mhandler)( boneno );
	_ASSERT( mcptr );
	if( !(mcptr->IsJoint()) ){
		DbgOut( "hs : GetBoneTra : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );

	int ret;
	CMotionPoint2 mp;
	mp.m_frameno = frameno;
	int hasmpflag = 0;
	ret = mcptr->CalcMotionPointOnFrame( selem, &mp, motid, frameno, &hasmpflag, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "hs : GetBoneTra : mc CalcMotionPointOnFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( hasmpflag == 1 ){
		*existptr = 1;
	}else{
		*existptr = 0;
	}

	traptr->x = mp.m_mvx;
	traptr->y = mp.m_mvy;
	traptr->z = mp.m_mvz;

	return 0;
}
int CHandlerSet::GetBoneScale( int boneno, int motid, int frameno, D3DXVECTOR3* scaleptr, int* existptr )
{
	*existptr = 0;

	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetBoneScale : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionCtrl* mcptr = (*m_mhandler)( boneno );
	_ASSERT( mcptr );
	if( !(mcptr->IsJoint()) ){
		DbgOut( "hs : GetBoneScale : this is not joint object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );

	int ret;
	CMotionPoint2 mp;
	mp.m_frameno = frameno;
	int hasmpflag = 0;
	ret = mcptr->CalcMotionPointOnFrame( selem, &mp, motid, frameno, &hasmpflag, m_shandler, m_mhandler );
	if( ret ){
		DbgOut( "hs : GetBoneScale : mc CalcMotionPointOnFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( hasmpflag == 1 ){
		*existptr = 1;
	}else{
		*existptr = 0;
	}

	scaleptr->x = mp.m_scalex;
	scaleptr->y = mp.m_scaley;
	scaleptr->z = mp.m_scalez;


	return 0;
}
int CHandlerSet::SetBoneTra( int boneno, int motid, int frameno, D3DXVECTOR3 srctra )
{
	int ret;
	CMotionPoint2* mpptr = 0;
	ret = m_mhandler->IsExistMotionPoint( motid, boneno, frameno, &mpptr );
	if( ret ){
		DbgOut( "hs : SetBoneTra : mh IsExistMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !mpptr ){
		DbgOut( "hs : SetBoneTra : no motion point exist in this frame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	mpptr->m_mvx = srctra.x;
	mpptr->m_mvy = srctra.y;
	mpptr->m_mvz = srctra.z;

	return 0;
}
int CHandlerSet::SetBoneScale( int boneno, int motid, int frameno, D3DXVECTOR3 srcscale )
{
	int ret;
	CMotionPoint2* mpptr = 0;
	ret = m_mhandler->IsExistMotionPoint( motid, boneno, frameno, &mpptr );
	if( ret ){
		DbgOut( "hs : SetBoneScale : mh IsExistMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !mpptr ){
		DbgOut( "hs : SetBoneScale : no motion point exist in this frame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	mpptr->m_scalex = srcscale.x;
	mpptr->m_scaley = srcscale.y;
	mpptr->m_scalez = srcscale.z;

	return 0;
}
int CHandlerSet::GetBoneMatrix( int boneno, int motid, int frameno, int kind, D3DXMATRIX* matptr )
{
	int ret;
	if( kind == 0 ){
		ret = m_mhandler->HuGetBoneMatrix( boneno, motid, frameno, matptr );
		if( ret ){
			DbgOut( "hs : GetBoneMatrix : mh HuGetBoneMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = m_mhandler->HuGetRawMatrix( boneno, motid, frameno, matptr );
		if( ret ){
			DbgOut( "hs : GetBoneMatrix : mh HuGetRawMatrix error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CHandlerSet::CreateMotionPoint( int boneno, int motid, int frameno )
{
	int ret;

	CMotionCtrl* firstjoint = 0;
	CMotionCtrl* tmpmc;
	CShdElem* tmpse;
	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		tmpse = (*m_shandler)( seri );
		tmpmc = (*m_mhandler)( seri );
		if( tmpse->IsJoint() && (tmpse->type != SHDMORPH) && tmpse->parent && tmpse->parent->IsJoint() &&
			tmpmc->motinfo && tmpmc->motinfo->firstmp && *(tmpmc->motinfo->firstmp + motid ) ){
			
			firstjoint = (*m_mhandler)( tmpse->serialno );
			break;
		}
	}

	ret = CreateMotionPoints( boneno, firstjoint, motid, frameno );
	if( ret ){
		DbgOut( "hs : CreateMotionPoint : CreateMotionPoints error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetIKSkip( int boneno, int* flagptr )
{
	if( (boneno <= 0) || (boneno > m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetIKSkip : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( boneno );

	*flagptr = selem->m_ikskip;

	return 0;
}
int CHandlerSet::SetIKSkip( int boneno, int flag )
{
	if( (boneno <= 0) || (boneno > m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetIKSkip : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( boneno );

	selem->m_ikskip = flag;

	return 0;
}

int CHandlerSet::CreateTexKey( int boneno, int motid, int frameno )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : CreateTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : CreateTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CTexKey* ptexkey = 0;
	ret = m_mhandler->ExistTexKey( boneno, motid, frameno, &ptexkey );
	if( ret ){
		DbgOut( "hs : CreateTexKey : mh ExistTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( ptexkey ){
		return 0;
	}

	CTexKey* newtk = 0;
	ret = m_mhandler->CreateTexKey( boneno, motid, frameno, &newtk );
	if( ret || !newtk ){
		DbgOut( "hs : CreateTexKey : mh CreateTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}
int CHandlerSet::DelTexKey( int boneno, int motid, int frameno )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : DelTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : DelTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CTexKey* ptexkey = 0;
	ret = m_mhandler->ExistTexKey( boneno, motid, frameno, &ptexkey );
	if( ret ){
		DbgOut( "hs : DelTexKey : mh ExistTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !ptexkey ){
		return 0;
	}

	ret = m_mhandler->DeleteTexKey( boneno, motid, frameno );
	if( ret ){
		DbgOut( "hs : DelTexKey : mh DeleteTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CHandlerSet::AddTexChange( int boneno, int motid, int frameno, TEXCHANGE srctc )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : AddTexChange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : AddTexChange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, srctc.materialno );
	if( !curmat ){
		DbgOut( "hs : AddTexChange : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CTexKey* newtk = 0;
	CTexKey* ptexkey = 0;
	ret = m_mhandler->ExistTexKey( boneno, motid, frameno, &ptexkey );
	if( ret ){
		DbgOut( "hs : AddTexChange : mh ExistTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !ptexkey ){
		ret = m_mhandler->CreateTexKey( boneno, motid, frameno, &newtk );
		if( ret || !newtk ){
			DbgOut( "hs : AddTexChange : mh CreateTexKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		newtk = ptexkey;
	}

	ret = newtk->SetTexChange( curmat, srctc.texname );
	if( ret ){
		DbgOut( "hs : AddTexChange : tk SetTexChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}
int CHandlerSet::DelTexChange( int boneno, int motid, int frameno, int materialno )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : DelTexChange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : DelTexChange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, materialno );
	if( !curmat ){
		DbgOut( "hs : DelTexChange : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CTexKey* ptexkey = 0;
	ret = m_mhandler->ExistTexKey( boneno, motid, frameno, &ptexkey );
	if( ret ){
		DbgOut( "hs : DelTexChange : mh ExistTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !ptexkey ){
		return 0;
	}

	ret = ptexkey->DeleteTexChange( curmat );
	if( ret ){
		DbgOut( "hs : DelTexChange : tk DeleteTexChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CHandlerSet::GetTexChangeNum( int boneno, int motid, int frameno, int* tcnumptr )
{
	*tcnumptr = 0;

	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetTexChangeNum : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetTexChangeNum : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CTexKey* ptexkey = 0;
	ret = m_mhandler->ExistTexKey( boneno, motid, frameno, &ptexkey );
	if( ret ){
		DbgOut( "hs : GetTexChangeNum : mh ExistTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !ptexkey ){
		*tcnumptr = 0;
	}else{
		*tcnumptr = ptexkey->changenum;
	}

	return 0;

}
int CHandlerSet::GetTexChange( int boneno, int motid, int frameno, int tcindex, TEXCHANGE* dsttc )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetTexChange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetTexChange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !dsttc ){
		DbgOut( "hs : GetTexChange : dsttc NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CTexKey* ptexkey = 0;
	ret = m_mhandler->ExistTexKey( boneno, motid, frameno, &ptexkey );
	if( ret ){
		DbgOut( "hs : GetTexChange : mh ExistTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( ptexkey ){
		if( (tcindex < 0) || (tcindex >= ptexkey->changenum) ){
			DbgOut( "hs : GetTexChange : tcindex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		CTexChange* tcptr = *( ptexkey->texchange + tcindex );
		_ASSERT( tcptr );
		if( tcptr ){
			dsttc->materialno = tcptr->m_mqomat->materialno;
			strcpy_s( dsttc->texname, 256, tcptr->m_change );
		}
	}else{
		dsttc->materialno = -1;
		strcpy_s( dsttc->texname, 256, "nodata" );
	}


	return 0;

}
int CHandlerSet::GetTexAnimKeyframeNoRange( int boneno, int motid, 
		int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetTexAnimKeyframeNoRange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetTexAnimKeyframeNoRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mhandler->GetTexAnimKeyframeNoRange( boneno, motid, srcstart, srcend, framearray, arrayleng, framenumptr );
	if( ret ){
		DbgOut( "hs : GetTexAnimKeyframeNoRange : mh GetTexAnimKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CHandlerSet::CreateDSKey( int boneno, int motid, int frameno )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : CreateDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : CreateDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CDSKey* pdskey = 0;
	ret = m_mhandler->ExistDSKey( boneno, motid, frameno, &pdskey );
	if( ret ){
		DbgOut( "hs : CreateDSKey : mh ExistDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( pdskey ){
		return 0;
	}

	CDSKey* newdsk = 0;
	ret = m_mhandler->CreateDSKey( boneno, motid, frameno, &newdsk );
	if( ret || !newdsk ){
		DbgOut( "hs : CreateDSKey : mh CreateDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}
int CHandlerSet::DelDSKey( int boneno, int motid, int frameno )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : DelDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : DelDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CDSKey* pdskey = 0;
	ret = m_mhandler->ExistDSKey( boneno, motid, frameno, &pdskey );
	if( ret ){
		DbgOut( "hs : DelDSKey : mh ExistDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !pdskey ){
		return 0;
	}

	ret = m_mhandler->DeleteDSKey( boneno, motid, frameno );
	if( ret ){
		DbgOut( "hs : DelDSKey : mh DeleteDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}
int CHandlerSet::AddDSChange( int boneno, int motid, int frameno, DSCHANGE srcdsc )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : AddDSChange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : AddDSChange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( (srcdsc.switchno < 0) || (srcdsc.switchno >= DISPSWITCHNUM) ){
		DbgOut( "hs : AddDSChange : srcdsc.switchno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !m_shandler->m_ds ){
		DbgOut( "hs : AddDSChange : ds NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CDispSwitch* curds;
	curds = m_shandler->m_ds + srcdsc.switchno;

	int ret;
	CDSKey* newdsk = 0;
	CDSKey* pdskey = 0;
	ret = m_mhandler->ExistDSKey( boneno, motid, frameno, &pdskey );
	if( ret ){
		DbgOut( "hs : AddDSChange : mh ExistDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !pdskey ){
		ret = m_mhandler->CreateDSKey( boneno, motid, frameno, &newdsk );
		if( ret || !newdsk ){
			DbgOut( "hs : AddDSChange : mh CreateDSKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		newdsk = pdskey;
	}

	ret = newdsk->SetDSChange( curds, srcdsc.state );
	if( ret ){
		DbgOut( "hs : AddDSChange : tk SetDSChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;

}
int CHandlerSet::DelDSChange( int boneno, int motid, int frameno, int switchno )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : DelDSChange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : DelDSChange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (switchno < 0) || (switchno >= DISPSWITCHNUM) ){
		DbgOut( "hs : DelDSChange : switchno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !m_shandler->m_ds ){
		DbgOut( "hs : DelDSChange : ds NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CDispSwitch* curds;
	curds = m_shandler->m_ds + switchno;

	int ret;
	CDSKey* pdskey = 0;
	ret = m_mhandler->ExistDSKey( boneno, motid, frameno, &pdskey );
	if( ret ){
		DbgOut( "hs : DelDSChange : mh ExistDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !pdskey ){
		return 0;
	}

	ret = pdskey->DeleteDSChange( curds );
	if( ret ){
		DbgOut( "hs : DelDSChange : tk DeleteDSChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CHandlerSet::GetDSChangeNum( int boneno, int motid, int frameno, int* dscnumptr )
{

	*dscnumptr = 0;

	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs :  : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs :  : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CDSKey* pdskey = 0;
	ret = m_mhandler->ExistDSKey( boneno, motid, frameno, &pdskey );
	if( ret ){
		DbgOut( "hs : GetDSChangeNum : mh ExistDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !pdskey ){
		*dscnumptr = 0;
	}else{
		*dscnumptr = pdskey->changenum;
	}

	return 0;

}
int CHandlerSet::GetDSChange( int boneno, int motid, int frameno, int dscindex, DSCHANGE* dstdsc )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetDSChange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetDSChange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !dstdsc ){
		DbgOut( "hs : GetDSChange : dstdsc NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CDSKey* pdskey = 0;
	ret = m_mhandler->ExistDSKey( boneno, motid, frameno, &pdskey );
	if( ret ){
		DbgOut( "hs : GetDSChange : mh ExistDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( pdskey ){
		if( (dscindex < 0) || (dscindex >= pdskey->changenum) ){
			DbgOut( "hs : GetDSChange : dscindex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		CDSChange* dscptr = *( pdskey->dschange + dscindex );
		_ASSERT( dscptr );
		if( dscptr ){
			dstdsc->switchno = dscptr->m_dsptr->switchno;
			dstdsc->state = dscptr->m_change;
		}
	}else{
		dstdsc->switchno = -1;
		dstdsc->state = 0;
	}

	return 0;

}
int CHandlerSet::GetDSAnimKeyframeNoRange( int boneno, int motid, 
		int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_mhandler->m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= m_mhandler->s2mot_leng) ){
		DbgOut( "hs : GetDSAnimKeyframeNoRange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "hs : GetDSAnimKeyframeNoRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mhandler->GetDSAnimKeyframeNoRange( boneno, motid, srcstart, srcend, framearray, arrayleng, framenumptr );
	if( ret ){
		DbgOut( "hs : GetDSAnimKeyframeNoRange : mh GetDSAnimKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CHandlerSet::GetMaterialTexParams( int materialno, MATERIALTEX* dstparams )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	if( materialno < 0 ){
		DbgOut( "hs : GetMaterialTexParams : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, materialno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialTexParams : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( dstparams->tex, sizeof( char ) * 256 );
	if( curmat->tex[0] ){
		strcpy_s( dstparams->tex, 256, curmat->tex );	
	}
	ZeroMemory( dstparams->bump, sizeof( char ) * 256 );
	if( curmat->bump[0] ){
		strcpy_s( dstparams->bump, 256, curmat->bump );
	}
	dstparams->exttexnum = curmat->exttexnum;
	ZeroMemory( dstparams->exttex, sizeof( char ) * MAXEXTTEXNUM * 256 );
	int extno;
	for( extno = 0; extno < curmat->exttexnum; extno++ ){
		strcpy_s( &(dstparams->exttex[extno][0]), 256, *( curmat->exttexname + extno ) ); 
	}
	dstparams->exttexmode = curmat->exttexmode;

	dstparams->transparent = curmat->transparent;
	dstparams->uanim = curmat->uanime;
	dstparams->vanim = curmat->vanime;
	dstparams->texrule = curmat->texrule;
	dstparams->texanimtype = curmat->texanimtype;

	dstparams->glowmult.r = curmat->glowmult[0];
	dstparams->glowmult.g = curmat->glowmult[1];
	dstparams->glowmult.b = curmat->glowmult[2];



	return 0;
}
int CHandlerSet::SetMaterialTexParams( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int materialno, MATERIALTEX* srcparams )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	if( materialno < 0 ){
		DbgOut( "hs : SetMaterialTexParams : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, materialno );
	if( !curmat ){
		DbgOut( "hs : SetMaterialTexParams : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcparams->exttexnum) < 0 || (srcparams->exttexnum > 100) ){
		DbgOut( "hs : SetMaterialTexParams : exttexnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( curmat->tex, sizeof( char ) * 256 );
	if( srcparams->tex[0] ){
		strcpy_s( curmat->tex, 256, srcparams->tex );
	}
	ZeroMemory( curmat->bump, sizeof( char ) * 256 );
	if( srcparams->bump[0] ){
		strcpy_s( curmat->bump, 256, srcparams->bump );
	}

	char* exttexlist[MAXEXTTEXNUM];
	ZeroMemory( exttexlist, sizeof( char* ) * MAXEXTTEXNUM );
	int extno;
	for( extno = 0; extno < MAXEXTTEXNUM; extno++ ){
		exttexlist[extno] = &( srcparams->exttex[extno][0] );
	}

	int ret;
	ret = curmat->SetExtTex( srcparams->exttexnum, exttexlist );
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams : mat SetExtTex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	curmat->transparent = srcparams->transparent;
	curmat->uanime = srcparams->uanim;
	curmat->vanime = srcparams->vanim;
	
	curmat->texrule = srcparams->texrule;
	curmat->texanimtype = srcparams->texanimtype;

	curmat->exttexmode = srcparams->exttexmode;
	if( curmat->exttexmode == EXTTEXMODE_GLOW ){
		curmat->transparent = 0;
	}else if( curmat->exttexmode == EXTTEXMODE_GLOWALPHA ){
		curmat->transparent = 2;
	}

	curmat->glowmult[0] = srcparams->glowmult.r;
	curmat->glowmult[1] = srcparams->glowmult.g;
	curmat->glowmult[2] = srcparams->glowmult.b;

/////////////
	//rename_texture
	ret = m_shandler->InvalidateTexture( 1 );
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams : sh InvalidateTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->CreateTexture( pdev, appwnd, 0, g_miplevels, g_mipfilter, 0, 0 );
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams : sh CreateTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->SetTexTransparent();
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams : sh SetTexTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->ChkAlphaNum();
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams : sh ChkAlphaNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//remake_uv
	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*m_shandler)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->SetUV( m_shandler->m_mathead );
			if( ret ){
				DbgOut( "hs : SetMaterialTexParams : se SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = selem->UpdateVertexBuffer( 0, TLMODE_ORG, m_shandler->m_shader );
			if( ret ){
				DbgOut( "hs : SetMaterialTexParams : se UpdateVertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CHandlerSet::GetMaterialTexParams1DIM( int materialno, MATERIALTEX1DIM* dstparams )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	if( materialno < 0 ){
		DbgOut( "hs : GetMaterialTexParams1DIM : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, materialno );
	if( !curmat ){
		DbgOut( "hs : GetMaterialTexParams1DIM : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( dstparams->tex, sizeof( char ) * 256 );
	if( curmat->tex[0] ){
		strcpy_s( dstparams->tex, 256, curmat->tex );	
	}
	ZeroMemory( dstparams->bump, sizeof( char ) * 256 );
	if( curmat->bump[0] ){
		strcpy_s( dstparams->bump, 256, curmat->bump );
	}
	dstparams->exttexnum = curmat->exttexnum;
	ZeroMemory( dstparams->exttex, sizeof( char ) * MAXEXTTEXNUM * 256 );
	int extno;
	char* dstptr = dstparams->exttex;
	for( extno = 0; extno < curmat->exttexnum; extno++ ){
		char* curext = *( curmat->exttexname + extno );
		int leng;
		leng = (int)strlen( curext );
		if( ( leng <= 0 ) || ( leng >= 256 ) ){
			DbgOut( "hs : GetMaterialTexParams1DIM : exttex leng error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		strncpy( dstptr, curext, leng );
		*( dstptr + leng ) = '\n';
		dstptr = dstptr + leng + 1;
	}

	dstparams->exttexmode = curmat->exttexmode;

	dstparams->transparent = curmat->transparent;
	dstparams->uanim = curmat->uanime;
	dstparams->vanim = curmat->vanime;
	dstparams->texrule = curmat->texrule;
	dstparams->texanimtype = curmat->texanimtype;

	dstparams->glowmult.r = curmat->glowmult[0];
	dstparams->glowmult.g = curmat->glowmult[1];
	dstparams->glowmult.b = curmat->glowmult[2];


	return 0;
}
int CHandlerSet::SetMaterialTexParams1DIM( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int materialno, MATERIALTEX1DIM* srcparams )
{
	if( !m_shandler || !m_thandler || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	if( materialno < 0 ){
		DbgOut( "hs : SetMaterialTexParams1DIM : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, materialno );
	if( !curmat ){
		DbgOut( "hs : SetMaterialTexParams1DIM : materialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcparams->exttexnum) < 0 || (srcparams->exttexnum > 100) ){
		DbgOut( "hs : SetMaterialTexParams1DIM : exttexnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( curmat->tex, sizeof( char ) * 256 );
	if( srcparams->tex[0] ){
		strcpy_s( curmat->tex, 256, srcparams->tex );
	}
	ZeroMemory( curmat->bump, sizeof( char ) * 256 );
	if( srcparams->bump[0] ){
		strcpy_s( curmat->bump, 256, srcparams->bump );
	}

	char tempext[256 * MAXEXTTEXNUM + 1];
	ZeroMemory( tempext, sizeof( char ) * ( 256 * MAXEXTTEXNUM + 1 ) );
	MoveMemory( tempext, srcparams->exttex, sizeof( char ) * ( 256 * MAXEXTTEXNUM ) );

	char* exttexlist[MAXEXTTEXNUM];
	ZeroMemory( exttexlist, sizeof( char* ) * MAXEXTTEXNUM );
	int extno;
	int ch = '\n';
	char* startptr = tempext;
	int curpos = 0;
	for( extno = 0; extno < srcparams->exttexnum; extno++ ){
		if( curpos >= (256 * MAXEXTTEXNUM) ){
			DbgOut( "hs : SetMaterialTexParams1DIM : curpos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		char* endptr = strchr( startptr, ch );
		if( endptr ){
			int leng;
			leng = (int)( endptr - startptr );
			if( (leng <= 0) || (leng >= 256) ){
				DbgOut( "hs : SetMaterialTexParams1DIM : exttex leng error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			*endptr = 0;

			exttexlist[extno] = startptr;
			startptr = endptr + 1;
			curpos += leng + 1;
		}else{
			DbgOut( "hs : SetMaterialTexParams1DIM : exttex separater error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	ret = curmat->SetExtTex( srcparams->exttexnum, exttexlist );
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams1DIM : mat SetExtTex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	curmat->transparent = srcparams->transparent;
	curmat->uanime = srcparams->uanim;
	curmat->vanime = srcparams->vanim;
	
	curmat->texrule = srcparams->texrule;
	curmat->texanimtype = srcparams->texanimtype;

	curmat->exttexmode = srcparams->exttexmode;
	if( curmat->exttexmode == EXTTEXMODE_GLOW ){
		curmat->transparent = 0;
	}else if( curmat->exttexmode == EXTTEXMODE_GLOWALPHA ){
		curmat->transparent = 2;
	}

	curmat->glowmult[0] = srcparams->glowmult.r;
	curmat->glowmult[1] = srcparams->glowmult.g;
	curmat->glowmult[2] = srcparams->glowmult.b;

/////////////
	//rename_texture
	ret = m_shandler->InvalidateTexture( 1 );
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams1DIM : sh InvalidateTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->CreateTexture( pdev, appwnd, 0, g_miplevels, g_mipfilter, 0, 0 );
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams1DIM : sh CreateTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->SetTexTransparent();
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams1DIM : sh SetTexTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->ChkAlphaNum();
	if( ret ){
		DbgOut( "hs : SetMaterialTexParams1DIM : sh ChkAlphaNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//remake_uv
	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*m_shandler)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->SetUV( m_shandler->m_mathead );
			if( ret ){
				DbgOut( "hs : SetMaterialTexParams1DIM : se SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = selem->UpdateVertexBuffer( 0, TLMODE_ORG, m_shandler->m_shader );
			if( ret ){
				DbgOut( "hs : SetMaterialTexParams1DIM : se UpdateVertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CHandlerSet::GetPartPropName( int partno, char* nameptr, int arrayleng )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}

	if( (partno <= 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetPartPropName : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CTreeElem2* telem;
	telem = (*m_thandler)( partno );
	_ASSERT( telem );

	strcpy_s( nameptr, arrayleng, telem->name );


	return 0;
}
int CHandlerSet::SetPartPropName( int partno, char* srcname )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}
	int ret;

	if( (partno <= 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetPartPropName : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nameleng;
	nameleng = (int)strlen( srcname );
	if( (nameleng <= 0) || (nameleng >= 256) ){
		DbgOut( "hs : SetPartPropName : nameleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CTreeElem2* telem;
	telem = (*m_thandler)( partno );
	_ASSERT( telem );

	ret = telem->SetName( srcname );
	if( ret ){
		DbgOut( "hs : SetPartPropName : te SetName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetPartPropDS( int partno, int* dsptr )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}
	if( (partno <= 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetPartPropDS : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );
	*dsptr = selem->dispswitchno;

	return 0;
}
int CHandlerSet::SetPartPropDS( int partno, int srcds )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}
	if( (partno <= 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetPartPropDS : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srcds < 0) || (srcds >= DISPSWITCHNUM) ){
		DbgOut( "hs : SetPartPropDS : dsno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );
	selem->dispswitchno = srcds;

	return 0;
}
int CHandlerSet::GetPartPropNotUse( int partno, int* flagptr )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}
	if( (partno <= 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetPartPropNotUse : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );
	*flagptr = selem->notuse;

	return 0;
}
int CHandlerSet::SetPartPropNotUse( int partno, int srcflag )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}
	if( (partno <= 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetPartPropNotUse : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );
	_ASSERT( selem );
	selem->notuse = srcflag;

	return 0;
}
int CHandlerSet::GetJointNotSel( int boneno, int* notselptr )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : GetJointNotSel : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	*notselptr = selem->m_notsel;

	return 0;
}
int CHandlerSet::SetJointNotSel( int boneno, int notsel )
{
	if( !m_thandler || !m_shandler ){
		return 1;
	}
	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : SetJointNotSel : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	selem->m_notsel = notsel;

	return 0;
}

int CHandlerSet::CloneDispObj( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int orgseri, char* tmpname, int* newseriptr )
{
	int ret;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、何もしない。
		return 0;
	}
	//////////////////
	if( m_mch ){
		delete m_mch;
		m_mch = 0;
	}

	ret = m_mhandler->DestroyMotionObj();
	if( ret ){
		DbgOut( "hs : CloneDispObj : mh DestroyMotionObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	////////////////////
	CShdElem* orgelem;
	orgelem = (*m_shandler)( orgseri );
	if( (orgelem->type != SHDPOLYMESH) && (orgelem->type != SHDPOLYMESH2) ){
		return 0;
	}

	CTreeElem2* orgtelem;
	orgtelem = (*m_thandler)( orgseri );

	CShdElem* parelem;
	parelem = (*m_shandler)( 1 );

	//////////////////
	int serino = m_shandler->s2shd_leng;
	int srctype = orgelem->type;
	int depth;	
	char newname[256];
	ZeroMemory( newname, 256 );
	strcpy_s( newname, 256, tmpname );

	m_thandler->Start( serino - 1 );
	m_thandler->Begin();
	depth = m_thandler->GetCurDepth();

///////////
	int retseri;
	retseri = m_thandler->AddTree( newname, serino );
	if( retseri != serino ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_thandler->SetElemType( serino, srctype );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//////////////
	ret = m_shandler->AddShdElem( serino, depth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->AddMotionCtrl( serino, depth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}	
//	ret = m_mhandler->SetHasMotion( serino, hasmotion );
//	if( ret ){
//		_ASSERT( 0 );
//		return 1;
//	}
	///////////////

	m_thandler->End();

	CShdElem* curelem;
	curelem = (*m_shandler)( serino );
	_ASSERT( curelem );
	ret = curelem->LeaveFromChain();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = curelem->PutToChain( parelem, 1 );
	if( ret ){
		DbgOut( "hs : CloneDispObj : PutToChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->CalcDepth();
	if( ret ){
		DbgOut( "hs : CloneDispObj : shandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_mhandler->CopyChainFromShd( m_shandler );
	if( ret ){
		DbgOut( "hs : CloneDispObj : mhandler CopyChainFromShd error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_mhandler->CalcDepth();
	if( ret ){
		DbgOut( "hs : CloneDispObj : mhandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	curelem->facet = orgelem->facet;
	curelem->alpha = orgelem->alpha;
	curelem->clockwise = orgelem->clockwise;
	curelem->transparent = orgelem->transparent;
	curelem->setalphaflag = orgelem->setalphaflag;
	curelem->texrule = orgelem->texrule;
	curelem->noskinning = orgelem->noskinning;
	curelem->blendmode = orgelem->blendmode;
	curelem->m_minfilter = orgelem->m_minfilter;
	curelem->m_magfilter = orgelem->m_magfilter;
	curelem->m_loadbimflag = orgelem->m_loadbimflag;
	curelem->m_enabletexture = orgelem->m_enabletexture;
	curelem->m_anchorflag = orgelem->m_anchorflag;
	curelem->m_mikodef = orgelem->m_mikodef;


	ret = curelem->CloneDispObj( orgelem );
	if( ret ){
		DbgOut( "hs : CloneDispObj : se CloneDispObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	////////////////////
	////////////////////
	ret = Restore( pdev, appwnd, -1, 0 );
	if( ret ){
		DbgOut( "hs : CloneDispObj : RestoreHandler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*newseriptr = serino;

	return 0;
}

int CHandlerSet::DestroyMorph( int baseno )
{
	int ret;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、何もしない。
		return 0;
	}
	//////////////////

	if( m_mch ){
		delete m_mch;
		m_mch = 0;
	}

	ret = m_mhandler->DestroyMotionObj();
	if( ret ){
		DbgOut( "hs : DestroyMorph : mh DestroyMotionObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//	ret = g_motdlg->RemakeMOEAH();
//	if( ret ){
//		_ASSERT( 0 );
//		return 1;
//	}

	////////////////////

	int delseri = -1;
	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		CShdElem* selem = (*m_shandler)( seri );
		_ASSERT( selem );
		if( selem->type == SHDMORPH ){
			if( selem->morph && selem->morph->m_baseelem && (selem->morph->m_baseelem->serialno == baseno) ){
				delseri = seri;
				break;
			}
		}
	}

	if( delseri <= 0 ){
		DbgOut( "hs : DestroyMorph : baseno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* delelem;
	delelem = (*m_shandler)( delseri );
	if( delelem->type != SHDMORPH ){
		return 0;
	}
	CMorph* delmorph;
	delmorph = delelem->morph;

	CShdElem* parelem = (*m_shandler)( 1 );

	int saveparent;
	m_shandler->GetParent( delseri, &saveparent );
	if( saveparent <= 1 ){
		saveparent = 1;
	}


	/////////////////////
	//baseとtargetを退避
	//morphからのドラッグ
	CShdElem* baseelem = delmorph->m_baseelem;
	ret = baseelem->LeaveFromChain();// child情報は、そのままで、チェインから離す。
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = baseelem->PutToChain( parelem, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	baseelem->dispflag = 1;
	baseelem->m_mtype = M_NONE;

	int tno;
	for( tno = 0; tno < delmorph->m_targetnum; tno++ ){
		CShdElem* curtarelem = (delmorph->m_ptarget + tno)->selem;
		ret = curtarelem->LeaveFromChain();// child情報は、そのままで、チェインから離す。
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = curtarelem->PutToChain( parelem, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		curtarelem->dispflag = 1;
	}

	ret = delmorph->DeleteAllTarget();
	_ASSERT( !ret );
	
	/////////////////////
	//morphをdestroy
	delelem->DestroyObjs();
	delelem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!

	CMotionCtrl* delmc;
	delmc = (*m_mhandler)( delseri );
	_ASSERT( delmc );
	delmc->type = SHDDESTROYED;//!!!!!!!!!!!!!!

	CTreeElem2* delte;
	delte = (*m_thandler)( delseri );
	_ASSERT( delte );
	delte->type = SHDDESTROYED;//!!!!!!!!!!!!!!!
	
	///////////////////////
	ret = m_shandler->CalcDepth();
	if( ret )
		return 1;
	
	ret = m_mhandler->CopyChainFromShd( m_shandler );
	if( ret )
		return 1;

	ret = m_mhandler->CalcDepth();
	if( ret )
		return 1;


	return 0;
}
int CHandlerSet::DestroyDispObj( int delseri )
{
	int ret;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、何もしない。
		return 0;
	}
	//////////////////
	if( m_mch ){
		delete m_mch;
		m_mch = 0;
	}

	ret = m_mhandler->DestroyMotionObj();
	if( ret ){
		DbgOut( "hs : DestroyDispObj : mh DestroyMotionObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//	ret = g_motdlg->RemakeMOEAH();
//	if( ret ){
//		_ASSERT( 0 );
//		return 1;
//	}

	////////////////////
	if( (delseri <= 0) || (delseri >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : DestroyDispObj : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* delelem;
	delelem = (*m_shandler)( delseri );
	if( (delelem->type != SHDPOLYMESH) && (delelem->type != SHDPOLYMESH2) ){
		return 1;
	}
	if( delelem->m_mtype == M_BASE ){
		_ASSERT( 0 );
		return 1;
	}

	int saveparent;
	m_shandler->GetParent( delseri, &saveparent );
	if( saveparent <= 1 ){
		saveparent = 1;
	}


	/////////////////////
	int targetflag = 0;
	if( delelem->m_mtype == M_TARGET ){

		CShdElem* morphelem = (*m_shandler)( saveparent );
		_ASSERT( morphelem );
		if( morphelem->type != SHDMORPH ){
			_ASSERT( 0 );
			return 1;
		}
		CMorph* curmorph = morphelem->morph;
		_ASSERT( curmorph );

		ret = curmorph->DeleteMorphTarget( delelem );
		_ASSERT( !ret );

		targetflag = 1;
	}

	ret = m_shandler->DeleteDispObj( delseri, m_thandler, m_mhandler );
	_ASSERT( !ret );


	return 0;
}

int CHandlerSet::AllClearInfElem( int partno, int vertno )
{
	int ret;
	ret = m_shandler->DestroyIE( partno, vertno );
	if( ret ){
		DbgOut( "hs : AllClearInfElem : sh DestroyIE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetGPOffset( D3DXVECTOR3 tra, CQuaternion2* srcqptr )
{
	int ret;

	D3DXMATRIX offmat;
	offmat = srcqptr->MakeRotMatX();
	offmat._41 = tra.x;
	offmat._42 = tra.y;
	offmat._43 = tra.z;

	if( m_gpah ){
		ret = m_gpah->SetOffsetMat( offmat );
		_ASSERT( !ret );
	}

	return 0;
}

int CHandlerSet::CreateInfScope( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int selbone, int seldisp, int* isindexptr )
{
	int ret;
	if( (selbone <= 0) || (selbone >= m_shandler->s2shd_leng) || (seldisp <= 0) || (seldisp >= m_shandler->s2shd_leng) ){
		DbgOut( "hs : CreateInfScope : param error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//存在したら、削除
	int chkindex;
	chkindex = m_shandler->ExistInfScope( selbone, seldisp );
	if( chkindex >= 0 ){
		CShdElem* iselem;
		iselem = m_shandler->GetInfScopeElem();
		_ASSERT( iselem );

		ret = iselem->DeleteInfScopeElemByIndex( chkindex );
		if( ret ){
			DbgOut( "hs : CreateInfScope : is DeleteInfScopeElemByIndex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = m_shandler->CreateInfScopePM2( selbone, seldisp );
	if( ret ){
		DbgOut( "hs : CreateInfScope : sh CreateInfScopePM2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Restore( pdev, appwnd, -1, 1 );
	if( ret ){
		DbgOut( "hs : CreateInfScope : app RestoreHandler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( seldisp > 0 ){
		(*m_shandler)( seldisp )->dispflag = 1;//!!!!!!!!!!!
	}

	ret = m_shandler->CalcInfElem( -1, 1 );
	if( ret ){
		DbgOut( "hs : CreateInfScope : sh CalcInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



////////

	int isindex;
	isindex = m_shandler->ExistInfScope( selbone, seldisp );
	if( isindex >= 0 ){
		ret = m_shandler->SetInfScopeDispFlag( isindex, 1, 1 );
		_ASSERT( !ret );


		CShdElem* iselem;
		iselem = m_shandler->GetInfScopeElem();
		if( iselem ){
			COLORREF srccol = 0xFF0000FF;

			ret = iselem->SetInfScopeDiffuseAmbient( isindex, srccol );
			if( ret ){
				DbgOut( "hs : CreateInfScope : is SetInfScopeDiffuse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		*isindexptr = isindex;

	}else{
		_ASSERT( 0 );
	}

	return 0;
}
int CHandlerSet::DestroyInfScope( int isindex )
{
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = iselem->DeleteInfScopeElemByIndex( isindex );
	if( ret ){
		DbgOut( "hs : DestroyInfScope : is DeleteInfScopeElemByIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::ExistInfScope( int boneno, int dispno, int* isindexptr )
{

	int chkindex;
	chkindex = m_shandler->ExistInfScope( boneno, dispno );
	if( chkindex >= 0 ){
		*isindexptr = chkindex;
	}else{
		*isindexptr = -1;
	}

	return 0;
}
int CHandlerSet::GetInfScopeNum( int* numptr )
{

	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		*numptr = 0;
		return 0;
	}

	*numptr = iselem->scopenum;

	return 0;
}
int CHandlerSet::GetInfScopeInfo( ISINFO* isiptr, int arrayleng, int* getnumptr )
{
	if( !isiptr ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		*getnumptr = 0;
		return 0;
	}

	int isno;
	for( isno = 0; isno < iselem->scopenum; isno++ ){
		if( isno >= arrayleng ){
			DbgOut( "hs : GetInfScopeInfo : arrayleng too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CInfScope* curis;
		curis = *( iselem->ppscope + isno );
		
		ISINFO* dstisi = isiptr + isno;

		dstisi->boneno = curis->applychild->serialno;
		dstisi->dispno = curis->target->serialno;
		dstisi->isid = isno;
	}

	*getnumptr = iselem->scopenum;

	return 0;
}
int CHandlerSet::SetInfScopeVert( ISVERT srcisv )
{

	int ret;

	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}

	if( (srcisv.isid < 0) || (srcisv.isid >= iselem->scopenum) ){
		DbgOut( "hs : SetInfScopeVert : isid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curis = *( iselem->ppscope + srcisv.isid );

	ret = m_shandler->SetVertPosInfScope( curis, m_mhandler, srcisv.vertno, srcisv.pos );
	if( ret ){
		DbgOut( "hs : SetInfScopeVert : shandler : SetVertPosInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CHandlerSet::GetInfScopeVert( ISVERT* isvptr )
{

	int ret;

	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}

	if( (isvptr->isid < 0) || (isvptr->isid >= iselem->scopenum) ){
		DbgOut( "hs : GetInfScopeVert : isid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curis = *( iselem->ppscope + isvptr->isid );

	ret = m_shandler->GetVertPosInfScope( curis, isvptr->vertno, &isvptr->pos );
	if( ret ){
		DbgOut( "hs : GetInfScopeVert : shandler : GetVertPosInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetInfScopeVertNum( int isindex, int* numptr )
{


	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}

	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "hs : GetInfScopeVertNum : isid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curis = *( iselem->ppscope + isindex );

	if( curis->d3ddisp ){
		*numptr = (int)curis->d3ddisp->m_numTLV;
	}else{
		*numptr = 0;
	}

	return 0;
}
int CHandlerSet::PickInfScopeVert( D3DXMATRIX matView, POINT scpos, POINT range, int isindex, ISVERT* isvptr, int arrayleng, int* getnumptr )
{
	if( (scpos.x < 0) || (scpos.x > g_scsize.x) || (scpos.y < 0) || (scpos.y > g_scsize.y) ){
		*getnumptr = 0;
		return 0;
	}
	if( (arrayleng < 0) || (arrayleng > 65535) ){
		_ASSERT( 0 );
		return 1;
	}

	int arno;
	for( arno = 0; arno < arrayleng; arno++ ){
		(isvptr + arno)->isid = -1;
		(isvptr + arno)->vertno = -1;
	}

	DWORD dwClipWidth = g_scsize.x / 2;
	DWORD dwClipHeight = g_scsize.y / 2;
	float aspect;
	aspect = (float)dwClipWidth / (float)dwClipHeight;

	int* arvertno;
	arvertno = (int*)malloc( sizeof( int ) * arrayleng );
	ZeroMemory( arvertno, sizeof( int ) * arrayleng );


	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}
	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "hs : PickInfScopeVert : isindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	curis = *( iselem->ppscope + isindex );
	int ret;
	ret = m_shandler->PickVertInfScope( isindex, dwClipWidth, dwClipHeight, m_mhandler,
		m_gpd.m_matWorld, matView, g_matProj, aspect,
		scpos.x, scpos.y, range.x, range.y, arvertno, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "hs : PickInfScopeVert : sh PickVertInfScope error !!!\n" );
		_ASSERT( 0 );
		free( arvertno );
		return 1;
	}

	if( *getnumptr > 0 ){
		for( arno = 0; arno < *getnumptr; arno++ ){
			(isvptr + arno)->isid = isindex;
			(isvptr + arno)->vertno = *( arvertno + arno );

			ret = m_shandler->GetVertPosInfScope( curis, (isvptr + arno)->vertno, &((isvptr + arno)->pos) );
			if( ret ){
				DbgOut( "hs : PickInfScopeVert : sh GetVertPosInfScope error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	free( arvertno );

	return 0;
}
int CHandlerSet::GetInfScopeAxis( int isindex, D3DXVECTOR3* dstvec3x3ptr )
{
	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}
	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "hs : PickInfScopeVert : isindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	curis = *( iselem->ppscope + isindex );
	int ret;
	D3DXVECTOR3 vecx, vecy, vecz;
	ret = m_shandler->GetBone3Vec( curis->applychild->serialno, dstvec3x3ptr, dstvec3x3ptr + 1, dstvec3x3ptr + 2 );
	if( ret ){
		DbgOut( "hs GetInfScopeAxis : sh GetBone3Vec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetInfScopeColor( int isindex, E3DCOLOR4UC srccol )
{
	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}
	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "hs : SetInfScopeColor : isindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	COLORREF col0;
	col0 = (srccol.a << 24) | (srccol.r << 16) | (srccol.g << 8) | srccol.b;
	ret = iselem->SetInfScopeDiffuseAmbient( isindex, col0 );
	if( ret ){
		DbgOut( "hs SetInfScopeColor : iselem SetInfScopeDiffuseAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetInfScopeColor( int isindex, E3DCOLOR4UC* dstcolptr )
{
	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}
	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "hs : SetInfScopeColor : isindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	COLORREF col0;
	ret = iselem->GetInfScopeDiffuse( isindex, &col0 );
	if( ret ){
		DbgOut( "hs : SetInfScopeColor ; iselem GetInfScopeDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	dstcolptr->a = (col0 & 0xFF000000) >> 24;
	dstcolptr->r = (col0 & 0x00FF0000) >> 16;
	dstcolptr->g = (col0 & 0x0000FF00) >> 8;
	dstcolptr->b = (col0 & 0x000000FF);

	return 0;
}
int CHandlerSet::SetInfScopeDispFlag( int isindex, int srcflag, int exclusiveflag )
{
	int ret;
	ret = m_shandler->SetInfScopeDispFlag( isindex,  srcflag, exclusiveflag );
	if( ret ){
		DbgOut( "hs : SetInfScopeDispFlag : sh SetInfScopeDispFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetInfScopeDispFlag( int isindex, int* dstflagptr )
{
	CInfScope* curis = 0;
	CShdElem* iselem;
	iselem = m_shandler->GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 0;
	}
	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "hs : GetInfScopeDispFlag : isindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	curis = *( iselem->ppscope + isindex );

	*dstflagptr = curis->dispflag;

	return 0;
}
int CHandlerSet::ImportMQOAsInfScope( LPDIRECT3DDEVICE9 pdev, HWND appwnd, char* filename, float mult, int boneno, int dispno, int* isindexptr )
{
	int ret;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、エラー。
		DbgOut( "hs : ImportMOQAsInfScope : data not exist before import error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_thandler || m_shandler || m_mhandler ){
		ret = m_shandler->DestroyBoneInfo( m_mhandler );
		if( ret ){
			DbgOut( "hs : ImportMOQAsInfScope : DestroyBoneInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = m_mhandler->DestroyMotionObj();
		if( ret ){
			DbgOut( "hs : ImportMOQAsInfScope : DestroyMotionObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	CMQOFile* mqofile;
	mqofile = new CMQOFile( appwnd );
	if( !mqofile ){
		_ASSERT( 0 );
		return 1;
	}
	D3DXVECTOR3 offset( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );

	ret = mqofile->LoadMQOFile( mult, filename, m_thandler, m_shandler, m_mhandler, m_shandler->s2shd_leng, 0, BONETYPE_RDB2, 0, offset, rot );
	if( ret ){
		DbgOut( "hs : ImportMOQAsInfScope : mqofile LoadMQOFile error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}	
	delete mqofile;

/***
	ret = Restore( pdev, appwnd, -1, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
***/
/////////////////////////
	int isseri = m_shandler->s2shd_leng - 2;

	///////////////////

//	InvalidateHandler( 1, 0 );

	if( m_thandler || m_shandler || m_mhandler ){


		ret = m_shandler->DestroyBoneInfo( m_mhandler );
		if( ret ){
			DbgOut( "hs : ImportMOQAsInfScope : DestroyBoneInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = m_mhandler->DestroyMotionObj();
		if( ret ){
			DbgOut( "hs : ImportMOQAsInfScope : DestroyMotionObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//		ret = g_motdlg->RemakeMOEAH();
//		if( ret ){
//			_ASSERT( 0 );
//			return 1;
//		}
	}

	//////////////////

	ret = m_shandler->MeshConv2InfScope( m_thandler, m_mhandler, boneno, dispno, isseri );
	if( ret ){
		DbgOut( "hs : ImportMOQAsInfScope : MeshConv2InfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	///////////////////

	//chain は、sh MeshConv2InfScope内で修正する。thandlerはそのまま。

	////////////////////
	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		DbgOut( "hs : ImportMOQAsInfScope : CreateBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = Restore( pdev, appwnd, -1, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->CalcInfElem( -1, 1 );
	if( ret ){
		DbgOut( "hs : ImportMOQAsInfScope : CalcInfElem error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

/////////////////
	*isindexptr = m_shandler->ExistInfScope( boneno, dispno );
	_ASSERT( *isindexptr >= 0 );

	return 0;
}

int CHandlerSet::GetMOELocalMotionFrameNo( int moeanimno, int moeframeno, int* lanimno, int* lframeno )
{
	
	if( !m_moeah || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( moeanimno );
	if( !anim ){
		DbgOut( "hs : GetMOELocalMotionFrameNo : moeanimno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetLocalMotionFrameNo( moeframeno, lanimno, lframeno );
	if( ret ){
		DbgOut( "hs : GetLocalMotionFrameNo : moeanim GetLocalMotionFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOELocalNextMotionFrameNo( int moeanimno, int moeframeno, int* lnanimno, int* lnframeno )
{
	if( !m_moeah || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( moeanimno );
	if( !anim ){
		DbgOut( "hs : GetMOELocalNextMotionFrameNo : moeanimno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetLocalNextMotionFrameNo( moeframeno, lnanimno, lnframeno );
	if( ret ){
		DbgOut( "hs : GetLocalNextMotionFrameNo : moeanim GetLocalMotionFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetMOEKeyGoOnFlag( int animno, int frameno, int flag )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : SetMOEKeyGoOnFlag : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEKey* moek = 0;
	ret = anim->ExistMOEKey( frameno, &moek );
	if( ret ){
		DbgOut( "hs : SetMOEKeyGoOnFlag : anim ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !moek ){
		DbgOut( "hs : SetMOEKeyGoOnFlag : key not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	moek->m_goonflag = flag;


	return 0;
}
int CHandlerSet::GetMOEKeyGoOnFlag( int animno, int frameno, int* flagptr )
{
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( animno );
	if( !anim ){
		DbgOut( "hs : GetMOEKeyGoOnFlag : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEKey* moek = 0;
	ret = anim->ExistMOEKey( frameno, &moek );
	if( ret ){
		DbgOut( "hs : GetMOEKeyGoOnFlag : anim ExistMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !moek ){
		DbgOut( "hs : GetMOEKeyGoOnFlag : key not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*flagptr = moek->m_goonflag;

	return 0;
}

int CHandlerSet::SetMOATrunkNotComID( int motid, int arrayleng, int* srcid )
{
	if( !m_mch ){
		DbgOut( "hs : SetMOATrunkNotComID : mch NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mch->SetTrunkNotComID( motid, arrayleng, srcid );
	if( ret ){
		DbgOut( "hs : SetMOATrunkNotComID : mch SetTruncNotComID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::GetMOELocalMotionFrameNoML( int moeanimno, int moeframeno, int boneid, int* lanimno, int* lframeno )
{
	
	if( !m_moeah || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( moeanimno );
	if( !anim ){
		DbgOut( "hs : GetMOELocalMotionFrameNoML : moeanimno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetLocalMotionFrameNoML( m_shandler, moeframeno, boneid, lanimno, lframeno );
	if( ret ){
		DbgOut( "hs : GetLocalMotionFrameNoML : moeanim GetLocalMotionFrameML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::GetMOELocalNextMotionFrameNoML( int moeanimno, int moeframeno, int boneid, int* lnanimno, int* lnframeno )
{
	if( !m_moeah || !m_mhandler ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMOEAnim* anim;
	anim = m_moeah->GetMOEAnim( moeanimno );
	if( !anim ){
		DbgOut( "hs : GetMOELocalNextMotionFrameNoML : moeanimno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetLocalNextMotionFrameNoML( m_shandler, moeframeno, boneid, lnanimno, lnframeno );
	if( ret ){
		DbgOut( "hs : GetLocalNextMotionFrameNoML : moeanim GetLocalMotionFrameML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CHandlerSet::SetDefaultGP( E3DGPSTATE gpstate )
{
	int ret;

	GPELEM gpe;
	gpe.pos = gpstate.pos;
	gpe.rot = gpstate.rot;
	gpe.ongmode = gpstate.ongmode;
	gpe.rayy = gpstate.rayy;
	gpe.rayleng = gpstate.rayleng;
	gpe.offsety = gpstate.offsety;
	int interp;
	interp = gpstate.interp;

	gpe.e3dpos = gpe.pos;
	gpe.e3drot = gpe.rot;

	ret = m_gpah->SetDefaultGPE( gpe );
	if( ret ){
		DbgOut( "hs : SetDefaultGP : gpah SetDefaultGPE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::SetKinectUpVec( int boneno, D3DXVECTOR3 upvec )
{
	int ret;

	if( (boneno <= 0) || (boneno >= m_shandler->s2shd_leng) ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( boneno );
	_ASSERT( selem );
	if( !selem->IsJoint() ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 nupvec;
	D3DXVec3Normalize( &nupvec, &upvec );

	ret = m_shandler->SetMikoBonePos( boneno, nupvec );
	if( ret ){
		DbgOut( "hs : SetKinectUpVec : sh SetMikoBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CHandlerSet::GetInViewFlag( int partno, int* flagptr )
{
	if( (partno <= 0) || (partno >= m_shandler->s2shd_leng) ){
		DbgOut( "hs GetInViewFlag : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*m_shandler)( partno );

	*flagptr = selem->curbs.visibleflag;

	return 0;
}

int CHandlerSet::GetMotionJumpFrame( int motid, int* frameptr )
{
	if( (m_mhandler->m_kindnum <= 0) || (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : GetMotionJumpFrame : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int jframe = 0;

	CMotionCtrl* mcptr = 0;
	CMotionInfo* motinfo = 0;
	int seri;
	for( seri = 0; seri < m_mhandler->s2mot_leng; seri++ ){
		mcptr = (*m_mhandler)( seri );
		if( mcptr->IsJoint() && mcptr->motinfo ){
			motinfo = mcptr->motinfo;
			break;
		}
	}

	if( motinfo ){
		jframe = motinfo->GetMotionJump( motid );
	}

	*frameptr = jframe;


	return 0;
}
int CHandlerSet::SetMotionJumpFrame( int motid, int frameno )
{
	if( (m_mhandler->m_kindnum <= 0) || (motid < 0) || (motid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : SetMotionJumpFrame : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || ( frameno >= *(m_mhandler->m_motnum + motid) ) ){
		DbgOut( "handlerset : SetMotionJumpFrame : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = m_mhandler->SetMotionJump( motid, frameno );
	if( ret ){
		DbgOut( "hs : SetMotionJumpFrame : mh SetMotionJump error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CHandlerSet::CopyTexAnimFrame( int srcmotid, int srcframe, int dstmotid, int dstframe )
{
	int ret;

	if( (m_mhandler->m_kindnum <= 0) || (srcmotid < 0) || (srcmotid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : CopyTexAnimFrame : srcmotid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srcframe < 0) || ( srcframe >= *(m_mhandler->m_motnum + srcmotid) ) ){
		DbgOut( "handlerset : CopyTexAnimFrame : srcframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (m_mhandler->m_kindnum <= 0) || (dstmotid < 0) || (dstmotid >= m_mhandler->m_kindnum) ){
		DbgOut( "handlerset : CopyTexAnimFrame : dstmotid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (dstframe < 0) || ( dstframe >= *(m_mhandler->m_motnum + dstmotid) ) ){
		DbgOut( "handlerset : CopyTexAnimFrame : dstframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int bno;
	for( bno = 0; bno < m_shandler->s2shd_leng; bno++ ){
		CShdElem* selem;
		selem = (*m_shandler)( bno );
		_ASSERT( selem );
		if( selem->IsJoint() && selem->type != SHDMORPH ){

			ret = m_mhandler->InitTexAnim( m_shandler );
			_ASSERT( !ret );
			ret = m_mhandler->GetTexAnim( bno, srcmotid, srcframe );
			_ASSERT( !ret );

			CMQOMaterial* curmat = m_shandler->m_mathead;
			int dirtytex = 0;
			while( curmat ){
				if( curmat->curtexname != NULL ){
					dirtytex++;
					break;
				}
				curmat = curmat->next;
			}
			ret = m_mhandler->DeleteTexKey( bno, dstmotid, dstframe );
			_ASSERT( !ret );

			if( dirtytex ){
				CTexKey* tkptr;
				ret = m_mhandler->CreateTexKey( bno, dstmotid, dstframe, &tkptr );
				_ASSERT( !ret );

				curmat = m_shandler->m_mathead;
				while( curmat ){
					if( curmat->curtexname ){
						ret = tkptr->SetTexChange( curmat, curmat->curtexname );
						_ASSERT( !ret );
					}
					curmat = curmat->next;
				}
			}
		}
	}
	return 0;
}
int CHandlerSet::DestroyTexAnimFrame( int motid, int frameno )
{
	EnterCriticalSection( &m_crit_addmotion );//######## start crit

	int ret;
	int bno;
	for( bno = 0; bno < m_shandler->s2shd_leng; bno++ ){
		CShdElem* selem;
		selem = (*m_shandler)( bno );
		_ASSERT( selem );
		if( selem->IsJoint() && selem->type != SHDMORPH ){
			ret = m_mhandler->DeleteTexKey( bno, motid, frameno );
			_ASSERT( !ret );
		}
	}

	LeaveCriticalSection( &m_crit_addmotion );//###### end crit

	return 0;
}

int CHandlerSet::GetMaterialExtTextureNum( int matno, int* numptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : MaterialExtTextureNum : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = curmat->exttexnum;

	return 0;
}
int CHandlerSet::GetMaterialExtTexture( char* dstnameptr, int matno, int extno )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : MaterialExtTexture : sh GetMaterialFromNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int extnum = curmat->exttexnum;
	if( (extno < 0) || (extno >= extnum) ){
		DbgOut( "hs : MaterialExtTexture : extno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* nameptr;
	nameptr = *( curmat->exttexname + extno );
		
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= 256 ){
		DbgOut( "handlerset : MaterialExtTexture : namelen too large warning !!!\n" );
		_ASSERT( 0 );
		strcpy_s( dstnameptr, 256, "エラー。名前が長すぎて格納できません。" );
	}else{
		strcpy_s( dstnameptr, 256, nameptr );
	}

	return 0;
}
int CHandlerSet::SetExtTextureToMaterial( int matno, int extno, int texid )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : SetExtTextureToMaterial : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : SetExtTextureToMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (extno < 0) || (extno >= curmat->exttexnum) ){
		DbgOut( "hs : SetExtTextureToMaterial : extno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "handlerset : SetExtTextureToMaterial : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "handlerset : SetExtTextureToMaterial : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nameleng;
	nameleng = (int)strlen( texname );
	if( (nameleng <= 0) || (nameleng > 255) ){
		DbgOut( "hs : SetExtTextureToMaterial : nameleng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* newnameptr;
	newnameptr = (char*)malloc( sizeof( char ) * (nameleng + 1) );
	if( !newnameptr ){
		_ASSERT( 0 );
		return 1;
	}
	strcpy_s( newnameptr, 256, texname );

	free( *(curmat->exttexname + extno) );
	*(curmat->exttexname + extno) = newnameptr;

	ret = m_shandler->ChkAlphaNum();
	_ASSERT( !ret );

	return 0;
}

int CHandlerSet::LoadMQOFileAsGroundFromBuf( HWND hwnd, char* bufptr, int bufsize, int adjustuvflag, float mult )
{

	int ret;

	CMQOFile* mqofile;
	mqofile = new CMQOFile( hwnd );
	if( !mqofile ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 offsetpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 rot( 0.0f, 0.0f, 0.0f );

	ret = mqofile->LoadMQOFileFromBuf( mult, bufptr, bufsize, m_thandler, m_shandler, m_mhandler, 0, 1, BONETYPE_RDB2, adjustuvflag, offsetpos, rot );
	if( ret ){
		DbgOut( "handlerset : LoadMQOFileAsGroundFromBuf : mqofile LoadMQOFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}

	ret = m_shandler->CreateBoneInfo( m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		delete mqofile;
		return 1;
	}	

	delete mqofile;

	return 0;
}

int CHandlerSet::ChgMaterial1stTexFromBuf( LPDIRECT3DDEVICE9 pdev, int matno, char* buf, int bufsize, int* texidptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : ChgMaterial1stTexFromBuf : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : ChgMaterial1stTexFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texindex = -1;
	texindex = g_texbnk->GetTexNoByName( curmat->tex, curmat->transparent );
	if( texindex < 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!
	}

	int ret;
	ret = g_texbnk->DestroyTexture( texindex, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->SetBufData( texindex, buf, bufsize );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = g_texbnk->CreateTexData( texindex, pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*texidptr = g_texbnk->GetSerial( texindex );

	return 0;
}
int CHandlerSet::ChgMaterialExtTexFromBuf( LPDIRECT3DDEVICE9 pdev, int matno, int extno, char* buf, int bufsize, int* texidptr )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : ChgMaterialExtTexFromBuf : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : ChgMaterialExtTexFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (extno < 0) || (extno >= curmat->exttexnum) ){
		DbgOut( "hs : ChgMaterialExtTexFromBuf : extno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texindex = -1;
	texindex = g_texbnk->GetTexNoByName( *(curmat->exttexname + extno), curmat->transparent );
	if( texindex < 0 ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!!!!!!
	}

	int ret;
	ret = g_texbnk->DestroyTexture( texindex, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->SetBufData( texindex, buf, bufsize );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = g_texbnk->CreateTexData( texindex, pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*texidptr = g_texbnk->GetSerial( texindex );

	return 0;
}

int CHandlerSet::ChgMaterial1stTexFromID( LPDIRECT3DDEVICE9 pdev, int matno, int texid )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : ChgMaterial1stTexFromID : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : ChgMaterial1stTexFromID : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = g_texbnk->SetAppTexture( curmat->tex, curmat->transparent, texid );
	if( ret ){
		DbgOut( "hs : ChgMaterial1stTexFromID : texbnk SetAppTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->ChkAlphaNum();
	_ASSERT( !ret );

	return 0;
}
int CHandlerSet::ChgMaterialExtTexFromID( LPDIRECT3DDEVICE9 pdev, int matno, int extno, int texid )
{
	CMQOMaterial* curmat;
	curmat = m_shandler->GetMaterialFromNo( m_shandler->m_mathead, matno );
	if( !curmat ){
		DbgOut( "hs : ChgMaterialExtTexFromID : sh GetMaterialFromNo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "handlerset : ChgMaterialExtTexFromID : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (extno < 0) || (extno >= curmat->exttexnum) ){
		DbgOut( "hs : ChgMaterialExtTexFromID : extno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = g_texbnk->SetAppTexture( *( curmat->exttexname + extno ), curmat->transparent, texid );
	if( ret ){
		DbgOut( "hs : ChgMaterialExtTexFromID : texbnk SetAppTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->ChkAlphaNum();
	_ASSERT( !ret );

	return 0;
}

int CHandlerSet::DestroyDispPart( int delseri )
{
	int ret;
	int targetflag = 0;
	if( !m_shandler || (m_shandler->s2shd_leng <= 1) ){
		//データが無い時は、何もしない。
		return 0;
	}

	if( delseri <= 0 ){
		return 0;
	}

	CShdElem* delelem;
	delelem = (*m_shandler)( delseri );
	if( (delelem->type != SHDPOLYMESH) && (delelem->type != SHDPOLYMESH2) ){
		return 0;
	}
	if( delelem->m_mtype == M_BASE ){
		::MessageBox( NULL, "モーフのベースは削除できません。", "禁止", MB_OK );
		return 0;
	}

	int saveparent;
	m_shandler->GetParent( delseri, &saveparent );
	if( saveparent <= 1 ){
		saveparent = 1;
	}


	/////////////////////
/***
	if( m_mcdlg->m_firstmake ){
		ret = m_mcdlg->DestroyMCElem( -1 );
		if( ret ){
			DbgOut( "viewer : DelDispObj : mcdlg DestroyMCElem all error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( g_motdlg )
		g_motdlg->InvalidateHandler();

	ret = m_mhandler->DestroyMotionObj();
	if( ret ){
		DbgOut( "viewer : DelDispObj : mh DestroyMotionObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = g_motdlg->RemakeMOEAH();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
***/

	////////////////////
	if( delelem->m_mtype == M_TARGET ){

		CShdElem* morphelem = (*m_shandler)( saveparent );
		_ASSERT( morphelem );
		if( morphelem->type != SHDMORPH ){
			_ASSERT( 0 );
			return 1;
		}
		CMorph* curmorph = morphelem->morph;
		_ASSERT( curmorph );

		ret = curmorph->DeleteMorphTarget( delelem );
		_ASSERT( !ret );

		targetflag = 1;
	}

	ret = m_shandler->DeleteDispObj( delseri, m_thandler, m_mhandler );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
