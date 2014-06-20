#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
//#include <wingdi.h>
#include <ExtLineIO.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>

//#include <polymesh.h>

#include <ExtLine.h>
#include <ExtPoint.h>


#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


CExtLineIO::CExtLineIO()
{
	InitLoadParams();

	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;


}
CExtLineIO::~CExtLineIO()
{


}

int CExtLineIO::CreateLine( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, D3DXVECTOR3* pptr, int pointnum, int maxpointnum, int linekind )
{
	int ret;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;


	InitLoadParams();

	ret = m_lpth->Start( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;	
	}

	befseri = curseri;
	curseri = 1;// 0‚ÍACreateHandler‚Åì¬Ï

	befdepth = curdepth;
	curdepth = 1;// 0‚ÍACreateHandler‚Åì¬Ï

	befshdtype = curshdtype;
	curshdtype = SHDEXTLINE;


	char lname[1024];
	ZeroMemory( lname, 1024 );

	SYSTEMTIME systime;
	GetLocalTime( &systime );
	sprintf_s( lname, 1024, "Line_%d_%d_%d_%d_%d_%d_%d",
		systime.wYear,
		systime.wMonth,
		systime.wDay,
		systime.wHour,
		systime.wMinute,
		systime.wSecond,
		systime.wMilliseconds
	);


	ret = AddShape2Tree( lname );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = SetMeshInfo( &tempinfo, SHDEXTLINE, pointnum, maxpointnum, linekind );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

////////
	CExtLine* curline;
	CShdElem* curselem;

	curselem = (*m_lpsh)( curseri );
	if( !curselem ){
		_ASSERT( 0 );
		return 1;
	}

	curline = curselem->extline;
	if( !curline ){
		_ASSERT( 0 );
		return 1;
	}

	int pno;
	int epid;
	for( pno = 0; pno < pointnum; pno++ ){
		ret = curline->AddExtPoint( -1, 1, &epid );
		if( ret || (epid < 0) ){
			DbgOut( "extlineio : curline AddExtPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = curline->SetExtPointPos( epid, pptr + pno );
		if( ret ){
			DbgOut( "extlineio : curline SetExtPointPos error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}
	}


////////
	ret = m_lpsh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_lpsh->SetColors();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = m_lpmh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
////////////

	m_lpmh->m_curbs.visibleflag = 1;//!!!!!!!!!!!!


	return 0;
}

void CExtLineIO::InitLoadParams()
{
	int ret;

	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	curseri = 0;
	befseri = 0;
	curdepth = 0;
	befdepth = 0;
	curshdtype = 0;
	befshdtype = 0;

}
int CExtLineIO::AddShape2Tree( char* elemname )
{
	int diffdepth, endcnt;
	int ret, i, chkdepth;
	diffdepth = curdepth - befdepth;

	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	if( diffdepth <= 0 ){
		endcnt = -diffdepth + 1;
		for( i = 0; i < endcnt; i++ ){
			ret = m_lpth->End();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	ret = m_lpth->Begin();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	chkdepth = m_lpth->GetCurDepth();
	if( chkdepth != curdepth ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_lpth->AddTree( elemname, curseri );
	if( curseri != ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_lpth->SetElemType( curseri, curshdtype );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	
// shdhandler
	ret = m_lpsh->AddShdElem( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_lpsh->SetClockwise( curseri, 2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_lpsh->SetBDivUV( curseri, 0, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//if( texname && *texname ){
	//	ret = m_lpsh->SetTexName( curseri, texname );
	//	if( ret ){
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//}

	//ret = (*m_lpsh)( curseri )->SetTexRepetition( 1, 1 );
	//_ASSERT( !ret );

	//if( transparentflag == 0 ){
	//	ret = (*m_lpsh)( curseri )->SetTexTransparent( 2 );
	//	_ASSERT( !ret );
	//}else if( transparentflag == 1 ){
	//	ret = (*m_lpsh)( curseri )->SetTexTransparent( 1 );
	//	_ASSERT( !ret );
	//}else{
	//	ret = (*m_lpsh)( curseri )->SetTexTransparent( 2 );
	//	_ASSERT( !ret );
	//}

	//ret = (*m_lpsh)( curseri )->SetUVAnime( 0.0f, 0.0f );
	//_ASSERT( !ret );

	//ret = (*m_lpsh)( curseri )->SetAlpha( 1.0f );
	//_ASSERT( !ret );

	//ret = (*m_lpsh)( curseri )->SetTexRule( TEXRULE_MQ );//!!!!!!!!!!!!!
	//_ASSERT( !ret );

	ret = (*m_lpsh)( curseri )->SetNotUse( 0 );
	_ASSERT( !ret );


	ret = (*m_lpsh)( curseri )->SetNoSkinning( 0 );

	//if( sheader.flags & FLAGS_NOSKINNING ){
	//	ret = (*lpsh)( curseri )->SetNoSkinning( 1 );
	//	_ASSERT( !ret );
	//}else{
	//	ret = (*lpsh)( curseri )->SetNoSkinning( 0 );
	//	_ASSERT( !ret );
	//}

	ret = (*m_lpsh)( curseri )->SetDispSwitchNo( 0 );
	_ASSERT( !ret );



// mothandler
	ret = m_lpmh->AddMotionCtrl( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_lpmh->SetHasMotion( curseri, 0 );
	_ASSERT( !ret );


	return 0;
}
int CExtLineIO::SetMeshInfo( CMeshInfo* dstmi, int mitype, int mim, int min, int linekind )
{
	int ret;

	
	//if( curshdtype != mih.type ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	ret = dstmi->SetMem( mitype, BASE_TYPE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mim, MESHI_M );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( min, MESHI_N );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( min, MESHI_TOTAL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( linekind, MESHI_MCLOSED );//!!!!!!!!!!!!!
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( 0, MESHI_NCLOSED );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( 0, MESHI_SKIP );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( 1, MESHI_HASCOLOR );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	VEC3F color, spec, amb;
	color.x = 1.0f; color.y = 1.0f; color.z = 1.0f;
	spec.x = 0.05f; spec.y = 0.05f; spec.z = 0.05f;
	amb.x = 0.5f; amb.y = 0.5f; amb.z = 0.5f;

	// material
	ret = dstmi->SetMem( color, MAT_DIFFUSE | VEC_ALL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( spec, MAT_SPECULAR | VEC_ALL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( amb, MAT_AMBIENT | VEC_ALL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CExtLineIO::Init3DObj()
{
	int ret;

	ret = m_lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

