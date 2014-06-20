
#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "PluginKinect.h"

#define DBGH
#include <dbg.h>

#include <crtdbg.h>


#include "txtsetting.h"
#include "rpsfile.h"

#include "handlerset.h"
#include <shdhandler.h>
#include <shdelem.h>

#include "FilterType.h"

#include <BoneProp.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <motionpoint2.h>
#include <quaternion.h>


CPluginKinect::CPluginKinect( int fullscflag )
{
	m_fullscflag = fullscflag;

	ZeroMemory( m_filepath, sizeof( char ) * _MAX_PATH );
	m_pluginversion = 0.0f;
	ZeroMemory( m_filename, sizeof( char ) * _MAX_PATH );
	strcpy_s( m_filename, _MAX_PATH, "KinecoDX.dll" );

	m_validflag = 0;//!!!!!!!!!!!!!!!!!!!!!!
	m_hModule = NULL;

	OpenNIInit = NULL;
	OpenNIClean = NULL;
	OpenNIDrawDepthMap = NULL;
	OpenNIDepthTexture = NULL;
	OpenNIGetSkeltonJointPosition = NULL;
	OpenNIIsTracking = NULL;
	OpenNIGetVersion = NULL;

	m_pdev = NULL;
	m_ptex = NULL;
	m_hwnd = NULL;

	m_texwidth = 256;
	m_texheight = 256;

	m_rendercnt = 0;

	m_ts = 0;
	m_rps = 0;

	m_capmode = 0;
	m_capmotid = 0;
	m_capstartframe = 0;
	m_capendframe = 0;
	m_capframe = 0;

	m_timermax = 9 * 30;
	m_curtimer = m_timermax;

}
void CPluginKinect::DestroyRps()
{
	if( m_rps ){
		delete m_rps;
		m_rps = 0;
	}

}
void CPluginKinect::DestroyTxtSetting()
{
	if( m_ts ){
		delete m_ts;
		m_ts = 0;
	}
}


CPluginKinect::~CPluginKinect()
{
	UnloadPlugin();
	DestroyRps();
	DestroyTxtSetting();
}

int CPluginKinect::SetFilePath( char* pluginpath )
{
	int leng;
	leng = (int)strlen( pluginpath );

	if( (leng <= 0) || (leng >= _MAX_PATH) ){
		DbgOut( "PluginKinect : SetFilePath : leng error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( m_filepath, _MAX_PATH, pluginpath );

	return 0;
}


#define GPA(proc) \
	*(FARPROC *)&proc = GetProcAddress(m_hModule, #proc);


int CPluginKinect::LoadPlugin()
{
	m_validflag = 0;//!!!!!!!!!!!!

	char dllname[_MAX_PATH];
	sprintf_s( dllname, _MAX_PATH, "%s\\%s", m_filepath, m_filename );




	m_hModule = LoadLibrary( dllname );
	if(m_hModule == NULL){
		DbgOut( "PluginKinect : LoadPlugin : LoadLibrary error %s!!!\r\n", dllname );
		_ASSERT( 0 );
		return 0;
	}

	GPA(OpenNIInit);
	GPA(OpenNIClean);
	GPA(OpenNIDrawDepthMap);
	GPA(OpenNIDepthTexture);
	GPA(OpenNIGetSkeltonJointPosition);
	GPA(OpenNIIsTracking);
	GPA(OpenNIGetVersion);



	if( !OpenNIInit || !OpenNIClean || !OpenNIDrawDepthMap || !OpenNIDepthTexture || !OpenNIGetSkeltonJointPosition || 
		!OpenNIIsTracking || !OpenNIGetVersion ){
		DbgOut( "PluginKinect : LoadPlugin : this dll is not for RDB2 %s\r\n", m_filename );
		return 0;//!!!!!!!!!!!!!!!!!
	}

////////////
	if( OpenNIGetVersion ){
		OpenNIGetVersion( &m_pluginversion );
	}


	m_validflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!	


	return 0;
}
int CPluginKinect::UnloadPlugin()
{
	m_validflag = 0;//!!!!!!!!!!!!!!!!

	if( m_hModule ){
		FreeLibrary( m_hModule );
		m_hModule = NULL;
	}
	
	return 0;
}

int CPluginKinect::SetChara( CHandlerSet* hsptr, char* kstname, int motid, int capmode )
{
//	if( m_validflag == 0 ){
//		_ASSERT( 0 );
//		return 0;
//	}

	DestroyRps();
	DestroyTxtSetting();

	m_hs = hsptr;

	int ret;



	m_rps = new CRpsFile( m_hs );
	if( !m_rps ){
		_ASSERT( 0 );
		m_validflag = 0;
		return 1;
	}
	ret = m_rps->CreateParams();
	if( ret ){
		_ASSERT( 0 );
		m_validflag = 0;
		return 1;
	}

	m_ts = new CTxtSetting( m_hs );
	if( !m_ts ){
		_ASSERT( 0 );
		m_validflag = 0;
		return 1;
	}
	ret = m_ts->LoadParams( kstname );
	if( ret ){
		_ASSERT( 0 );
		m_validflag = 0;
		DestroyTxtSetting();
		return 1;
	}

	D3DXVECTOR3 xpdir( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 zmdir( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 zpdir( 0.0f, 0.0f, 1.0f );

	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_RIGHT_KNEE)->jointno, xpdir );
	_ASSERT( !ret );
	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_RIGHT_FOOT)->jointno, xpdir );
	_ASSERT( !ret );
	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_LEFT_KNEE)->jointno, xpdir );
	_ASSERT( !ret );
	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_LEFT_FOOT)->jointno, xpdir );
	_ASSERT( !ret );

	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_RIGHT_ELBOW)->jointno, zmdir );
	_ASSERT( !ret );
	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_RIGHT_HAND)->jointno, xpdir );
	_ASSERT( !ret );
	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_LEFT_ELBOW)->jointno, zpdir );
	_ASSERT( !ret );
	ret = m_hs->m_shandler->SetMikoBonePos( (m_ts->m_elem + SKEL_LEFT_HAND)->jointno, xpdir );
	_ASSERT( !ret );


	m_capmotid = motid;
	if( m_capmotid < 0 ){
		_ASSERT( 0 );
		return 0;
	}
	m_capmode = capmode;

	int motleng = 0;
	ret = m_hs->m_mhandler->GetMotionFrameLength( m_capmotid, &motleng );
	_ASSERT( !ret );


	if( m_capmode == 0 ){
		m_capstartframe = 0;
		m_capendframe = motleng;
		m_capframe = 0;

	}else{
		m_capstartframe = 0;
		m_capendframe = m_capstartframe;
		m_capframe = m_capstartframe;

		m_curtimer = m_timermax;
	}

	ret = CreateMotionPoints( m_capmotid, m_capstartframe, m_capendframe, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_hs->m_mhandler->FillUpMotion( m_hs->m_shandler, m_capmotid, 
		(m_ts->m_elem + SKEL_TOPOFJOINT)->jointno, m_capstartframe, m_capendframe, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CPluginKinect::StartCapture()
{
	bool bret;
	if( m_validflag == 0 ){
		::MessageBoxA( m_hwnd, "Kinect用のプラグインが正常に読み込まれなかったので\nキャプチャ出来ません。", "エラー", MB_OK );
		return 1;
	}
	if( !m_ts ){
		::MessageBoxA( m_hwnd, "kstファイルが読み込まれていないのでキャプチャ出来ません。\nkstファイルを読み込んでから再試行してください。", "エラー", MB_OK );
		return 1;
	}

//	::MessageBox( m_hwnd, "kinect capture start", "kinect", MB_OK );

	m_rendercnt = 0;

//	DestroyRps();

	OpenNIClean();
	bret = OpenNIInit( m_fullscflag, m_hwnd, 0, m_pdev, m_filepath, 0 );
	if( bret == false ){
		_ASSERT( 0 );
		m_validflag = 0;
		return 1;
	}

	OpenNIDepthTexture( &m_ptex );
	if( !m_ptex ){
		_ASSERT( 0 );
		return 1;
	}


	HRESULT hr;
	D3DSURFACE_DESC	sdesc;
	hr = m_ptex->GetLevelDesc( 0, &sdesc );
	if( hr != D3D_OK ){
		DbgOut( "PluginKinect : GetLevelDesc error !!!\n" );
		_ASSERT( 0 );
		m_validflag = 0;
		return 1;
	}
	m_texwidth = sdesc.Width;
	m_texheight = sdesc.Height;



	return 0;
}
int CPluginKinect::EndCapture()
{
	if( m_validflag == 0 ){
		return 0;
	}

	int ret;
	if( m_rendercnt != 0 ){
		ret = m_hs->m_mhandler->FillUpMotion( m_hs->m_shandler, m_capmotid, 
			(m_ts->m_elem + SKEL_TOPOFJOINT)->jointno, -1, -1, 1 );
		_ASSERT( !ret );
	}

	m_ptex = NULL;
	OpenNIClean();

	return 0;
}

int CPluginKinect::GetSkelPos( D3DXVECTOR3* posptr, int* getflag )
{
	*getflag = 0;

	if( (m_validflag == 0) || !m_ts || !m_rps ){
		_ASSERT( 0 );
		return 0;
	}

	OpenNIDrawDepthMap( 1 );

	bool trflag = false;
	OpenNIIsTracking( &trflag );

	D3DXVECTOR3 tmppos[ SKEL_MAX ];


	if( trflag ){
		OpenNIGetSkeltonJointPosition( SKEL_MAX, tmppos );
		int skno;
		for( skno = 0; skno < SKEL_MAX; skno++ ){
			tmppos[skno].x *= -1.0f;
		}

		MoveMemory( posptr, tmppos, sizeof( D3DXVECTOR3 ) * SKEL_MAX );
		*getflag = 1;
	}else{
		*getflag = 0;
	}


	return 0;
}
int CPluginKinect::IsTracking( int* flagptr, int* cntptr )
{
	if( m_validflag == 0 ){
		*flagptr = 0;
		*cntptr = 0;
		_ASSERT( 0 );
		return 0;
	}

	bool trflag = false;
	OpenNIIsTracking( &trflag );

	*flagptr = (int)trflag;
	*cntptr = m_rendercnt;

	return 0;
}
int CPluginKinect::SetSkelPos( int kinectflag, int posmode, D3DXVECTOR3* srcpos, int* framenoptr )
{
	if( !m_ts || !m_rps ){
		_ASSERT( 0 );
		return 0;
	}
	if( (kinectflag == 1) && (m_validflag == 0) ){
		_ASSERT( 0 );
		return 1;
	}

	bool trflag = false;
	if( kinectflag == 1 ){
		OpenNIIsTracking( &trflag );
	}else{
		trflag = true;//bulletなど用
	}

	int ret;

	if( (m_rendercnt == 0) && trflag ){
		m_capframe = m_capstartframe;

		ret = m_rps->InitArMp( m_ts->m_elem, m_capmotid, m_capframe );
		_ASSERT( !ret );
		
		ret = m_rps->SetRpsElem( 0, srcpos );
		_ASSERT( !ret );

		ret = m_rps->CalcTraQ( m_ts->m_elem, 1 );
		_ASSERT( !ret );

		ret = m_rps->SetMotion( posmode, m_ts->m_elem, m_capmotid, m_capframe );
		_ASSERT( !ret );

		*framenoptr = m_capframe;

		m_rendercnt++;
	}else if( trflag ){
		if( m_capmode == 0 ){
			m_capframe++;
			if( m_capframe > m_capendframe ){
				m_rendercnt = 0;

				ret = m_hs->m_mhandler->FillUpMotion( m_hs->m_shandler, m_capmotid, 
					(m_ts->m_elem + SKEL_TOPOFJOINT)->jointno, -1, -1, 1 );
				_ASSERT( !ret );

				*framenoptr = m_capendframe;

				return 2;//!!!!!!!!!!!!!!!!!終わりの印
			}
			*framenoptr = m_capframe;
		}else{
			/***
			m_curtimer--;
			if( m_curtimer <= 0 ){
				m_rendercnt = 0;

				ret = m_hs->m_mhandler->FillUpMotion( m_hs->m_shandler, m_capmotid, 
					(m_ts->m_elem + SKEL_TOPOFJOINT)->jointno, -1, -1, 1 );
				_ASSERT( !ret );
				return 2;//!!!!!!!!!!!!!!!!!終わりの印
			}
			***/
			*framenoptr = m_capframe;
		}

		ret = m_rps->SetRpsElem( 2, srcpos );
		_ASSERT( !ret );

		ret = m_rps->CalcTraQ( m_ts->m_elem, 1 );
		_ASSERT( !ret );

		ret = m_rps->SetMotion( posmode, m_ts->m_elem, m_capmotid, m_capframe );
		_ASSERT( !ret );

		m_rendercnt++;
	}else{
		m_rendercnt = 0;
		*framenoptr = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!
	}

	return 0;
}


/***
int CPluginKinect::Update()
{
	if( (m_validflag == 0) || !m_ts || !m_rps ){
		_ASSERT( 0 );
		return 0;
	}

	OpenNIDrawDepthMap( 1 );

	bool trflag = false;
	OpenNIIsTracking( &trflag );

	D3DXVECTOR3 tmppos[ SKEL_MAX ];
	int ret;


	if( (m_rendercnt == 0) && trflag ){
		m_capframe = m_capstartframe;

		ret = m_rps->InitArMp( m_ts->m_elem, m_capmotid, m_capframe );
		_ASSERT( !ret );
		
		OpenNIGetSkeltonJointPosition( SKEL_MAX, tmppos );
		int skno;
		for( skno = 0; skno < SKEL_MAX; skno++ ){
			tmppos[skno].x *= -1.0f;
		}
		ret = m_rps->SetRpsElem( 0, tmppos );
		_ASSERT( !ret );

		ret = m_rps->CalcTraQ( m_ts->m_elem, 1 );
		_ASSERT( !ret );

		ret = m_rps->SetMotion( m_ts->m_elem, m_capmotid, m_capframe );
		_ASSERT( !ret );

		m_rendercnt++;
	}else if( trflag ){
		if( m_capmode == 0 ){
			m_capframe++;
			if( m_capframe > m_capendframe ){
				m_rendercnt = 0;

				ret = m_hs->m_mhandler->FillUpMotion( m_hs->m_shandler, m_capmotid, 
					(m_ts->m_elem + SKEL_TOPOFJOINT)->jointno, -1, -1, 1 );
				_ASSERT( !ret );
				return 2;//!!!!!!!!!!!!!!!!!終わりの印
			}
		}else{
			m_curtimer--;
			if( m_curtimer <= 0 ){
				m_rendercnt = 0;

				ret = m_hs->m_mhandler->FillUpMotion( m_hs->m_shandler, m_capmotid, 
					(m_ts->m_elem + SKEL_TOPOFJOINT)->jointno, -1, -1, 1 );
				_ASSERT( !ret );
				return 2;//!!!!!!!!!!!!!!!!!終わりの印
			}
		}

		OpenNIGetSkeltonJointPosition( SKEL_MAX, tmppos );
		int skno;
		for( skno = 0; skno < SKEL_MAX; skno++ ){
			tmppos[skno].x *= -1.0f;
		}
		ret = m_rps->SetRpsElem( 2, tmppos );
		_ASSERT( !ret );

		ret = m_rps->CalcTraQ( m_ts->m_elem, 1 );
		_ASSERT( !ret );

		ret = m_rps->SetMotion( m_ts->m_elem, m_capmotid, m_capframe );
		_ASSERT( !ret );

		m_rendercnt++;
	}else{
		m_rendercnt = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!
	}


	return 0;
}
***/


int CPluginKinect::CreateMotionPoints( int cookie, int startframe, int endframe, int forceflag )
{
	//forceflag == 1のときは初期状態のモーションポイントを作る。

	if( m_hs->m_mhandler->s2mot_leng < 1 ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!
	}

	int elemno;//, framenum;
	CMotionCtrl* mcptr;
	CMotionPoint2* curmp;
	CShdElem* selem;

	CQuaternion initq;
	initq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	int interp;
	m_hs->m_mhandler->GetDefInterp( cookie, &interp );

	int frameno;
	int ret;
	for( frameno = startframe; frameno <= endframe; frameno++ ){
		for( elemno = 1; elemno < m_hs->m_mhandler->s2mot_leng; elemno++ ){
			mcptr = (*m_hs->m_mhandler)( elemno );
			if( !mcptr ){
				_ASSERT( 0 );
				return 1;
			}
			selem = (*m_hs->m_shandler)( elemno );
			_ASSERT( selem );

			if( mcptr->IsJoint() && (mcptr->type != SHDMORPH)){
				curmp = mcptr->IsExistMotionPoint( cookie, frameno );
				if( !curmp ){

					CMotionPoint2 tmpmp;
					tmpmp.InitParams();

					if( forceflag == 0 ){
						int hasmpflag = 0;
						ret = mcptr->CalcMotionPointOnFrame( selem, &tmpmp, cookie, frameno, &hasmpflag,
							m_hs->m_shandler, m_hs->m_mhandler );
						if( ret ){
							_ASSERT( 0 );
							return 1;
						}
					}

					CMotionPoint2* newmp;
					newmp = mcptr->AddMotionPoint( cookie, frameno, tmpmp.m_q, tmpmp.m_mvx, tmpmp.m_mvy, tmpmp.m_mvz,
						0, interp, tmpmp.m_scalex, tmpmp.m_scaley, tmpmp.m_scalez, 0 );
					if( !newmp ){
						_ASSERT( 0 );
						return 1;
					}
				}else{
					if( forceflag == 1 ){
						curmp->m_q.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
						curmp->m_eul = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
						curmp->m_mvx = 0.0f;
						curmp->m_mvy = 0.0f;
						curmp->m_mvz = 0.0f;
						curmp->m_scalex = 1.0f;
						curmp->m_scaley = 1.0f;
						curmp->m_scalez = 1.0f;
					}
				}
			}
		}
	}
		
	return 0;
}


int CPluginKinect::QtoEul( CShdHandler* lpsh, CQuaternion srcq, D3DXVECTOR3 befeul, int boneno, D3DXVECTOR3* eulptr, CQuaternion* axisqptr )
{
	int ret;
	ret = lpsh->GetInitialBoneQ( boneno, axisqptr );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 cureul( 0.0f, 0.0f, 0.0f );
	ret = qToEulerAxis( *axisqptr, &srcq, &cureul );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = modifyEuler( &cureul, &befeul );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*eulptr = cureul;

	return 0;
}
