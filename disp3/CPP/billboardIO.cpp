#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <wingdi.h>
#include <billboardIO.h>
#include <D3DX9.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>

#include <polymesh.h>

#define	DBGH
#include "dbg.h"

#include <crtdbg.h>

CBillboardIO::CBillboardIO()
{
	InitLoadParams();

	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;


}

void CBillboardIO::InitLoadParams()
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

	m_pmnum = 0;

}

CBillboardIO::~CBillboardIO()
{

}


int CBillboardIO::CreateBillboard( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
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
	curseri = 1;// 0は、CreateHandlerで作成済

	befdepth = curdepth;
	curdepth = 1;// 0は、CreateHandlerで作成済

	befshdtype = curshdtype;
	curshdtype = SHDBILLBOARD;


	char bbname[1024];
	ZeroMemory( bbname, 1024 );

	SYSTEMTIME systime;
	GetLocalTime( &systime );
	sprintf( bbname, "Billboard_%d_%d_%d_%d_%d_%d_%d",
		systime.wYear,
		systime.wMonth,
		systime.wDay,
		systime.wHour,
		systime.wMinute,
		systime.wSecond,
		systime.wMilliseconds
	);


	ret = AddShape2Tree( bbname, 0, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = SetMeshInfo( &tempinfo, SHDBILLBOARD, MAX_BILLBOARD_NUM, 1 );
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

	return 0;
}

int CBillboardIO::AddBillboard( char* bmpname, float width, float height, int transparentflag, int dirmode, int orgflag, int* retseri )
{
	*retseri = -1;

	int ret;
	char bbname[1024];
	CBillboard* curbb;


	curbb = (*m_lpsh)( 1 )->billboard;// offset時には、修正必要！！！
	if( !curbb ){
		DbgOut( "billboardIO : AddBillboard : curbb NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	befseri = curseri;
	curseri = befseri + 1;// 0は、CreateHandlerで作成済

	befdepth = curdepth;
	curdepth = 2;// 0は、CreateHandlerで作成済

	befshdtype = curshdtype;
	curshdtype = SHDPOLYMESH;
		////////

	sprintf( bbname, "polymesh of BB %d", m_pmnum );


	ret = AddShape2Tree( bbname, bmpname, transparentflag );
	if( ret ){
		DbgOut( "billboardIO : AddBillboard : AddShape2Tree error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		/////////
	int pmvnum = 4;
	int pmfnum = 2;

	ret = SetMeshInfo( &tempinfo, SHDPOLYMESH, pmvnum, pmfnum );
	if( ret ){
		DbgOut( "billboardIO : AddBillboard : SetMeshInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		////////
	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = (*m_lpsh)( curseri )->CreatePolyMeshIndex( pmfnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

		/////////
	CPolyMesh* pmptr;
	pmptr = (*m_lpsh)( curseri )->polymesh;
	if( !pmptr ){
		DbgOut( "billboardIO : AddBillboard : pmptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
	ret = curbb->SetSElemPtr( (*m_lpsh)( curseri ), max( width, height ) );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	pmptr->billboardflag = 1;

	ret = pmptr->SetBillboardPoints( width, height, orgflag );
	if( ret ){
		DbgOut( "billboardIO : AddBillboard : pmptr SetBillboardPoints error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//dirmode
	(*m_lpsh)( curseri )->m_bbdirmode = dirmode;//!!!!!!!!!!!


	(*m_lpsh)( curseri )->curbs.SetBSFromVEC3F( pmptr->pointbuf, 4 );


//DbgOut( "bbio : add : seri %d, mode %d\r\n", curseri, dirmode );

//transparent




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


	m_pmnum++;//!!!!!!!!!!


	*retseri = curseri;//!!!!!!!


	return 0;
}


int CBillboardIO::AddShape2Tree( char* elemname, char* texname, int transparentflag )
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

	if( texname && *texname ){
		ret = m_lpsh->SetTexName( curseri, texname );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = (*m_lpsh)( curseri )->SetTexRepetition( 1, 1 );
	_ASSERT( !ret );

	if( transparentflag == 0 ){
		ret = (*m_lpsh)( curseri )->SetTexTransparent( 2 );
		_ASSERT( !ret );
	}else if( transparentflag == 1 ){
		ret = (*m_lpsh)( curseri )->SetTexTransparent( 1 );
		_ASSERT( !ret );
	}else{
		ret = (*m_lpsh)( curseri )->SetTexTransparent( 2 );
		_ASSERT( !ret );
	}

	ret = (*m_lpsh)( curseri )->SetUVAnime( 0.0f, 0.0f );
	_ASSERT( !ret );

	ret = (*m_lpsh)( curseri )->SetAlpha( 1.0f );
	_ASSERT( !ret );

	ret = (*m_lpsh)( curseri )->SetTexRule( TEXRULE_MQ );//!!!!!!!!!!!!!
	_ASSERT( !ret );

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

int CBillboardIO::SetMeshInfo( CMeshInfo* dstmi, int mitype, int mim, int min )
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
	ret = dstmi->SetMem( mim * min, MESHI_TOTAL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( 0, MESHI_MCLOSED );
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

int CBillboardIO::Init3DObj()
{
	int ret;

	ret = m_lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
