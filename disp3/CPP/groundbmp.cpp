#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <wingdi.h>
#include <groundbmp.h>
#include <D3DX9.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>

#include <grounddata.h>
#include <polymesh.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

CGroundBMP::CGroundBMP()
{
	ZeroMemory( &m_hdr1, sizeof( BITMAPFILEHEADER ) );
	ZeroMemory( &m_biheader1, sizeof( BITMAPINFOHEADER ) );
	m_rgbdat1 = 0;
	m_ntsc_y1 = 0;

	ZeroMemory( &m_hdr2, sizeof( BITMAPFILEHEADER ) );
	ZeroMemory( &m_biheader2, sizeof( BITMAPINFOHEADER ) );
	m_rgbdat2 = 0;
	m_ntsc_y2 = 0;

	ZeroMemory( &m_hdr3, sizeof( BITMAPFILEHEADER ) );
	ZeroMemory( &m_biheader3, sizeof( BITMAPINFOHEADER ) );
	m_rgbdat3 = 0;
	m_ntsc_y3 = 0;

	m_bmpw = 0;
	m_bmph = 0;

	m_lpth = 0;
	m_lpsh = 0;
	m_lpmh = 0;


}

void CGroundBMP::InitLoadParams()
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

CGroundBMP::~CGroundBMP()
{

}

int CGroundBMP::LoadMovableBMP( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, 
		char* bmpname, float maxx, float maxz, int divx, int divz, float wallheight )
{
	int ret = 0;
	int setpmno;

	CVec3f spcvec;
	spcvec.x = 0.0f;
	spcvec.y = 0.0f;
	spcvec.z = 0.0f;

	HANDLE hfile1 = 0;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	int notfound = 0;

	m_ntsc_y1 = GetBmpY( bmpname, &hfile1, &m_hdr1, &m_biheader1, &miny1, &maxy1, &notfound );
	if( !m_ntsc_y1 ){
		DbgOut( "GroundBMP : LoadMovableBMP : file1 bmp error !!!\n" );
		_ASSERT( 0 );
		if( notfound == 1 )
			ret = D3DAPPERR_MEDIANOTFOUND;
		else
			ret = 1;

		goto loadmvbmpexit;
	}

	m_bmpw = m_biheader1.biWidth;
	m_bmph = m_biheader1.biHeight;

//////////
	ret = m_lpsh->InitMQOMaterial();
	if( ret ){
		DbgOut( "groundbmp : LoadMovableBMP : sh InitMQOMaterail error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



//////////	
	InitLoadParams();

	ret = m_lpth->Start( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;
	}

///// CGroundDataの作成
	befseri = curseri;
	curseri = 1;// 0は、CreateHandlerで作成済

	befdepth = curdepth;
	curdepth = 1;// 0は、CreateHandlerで作成済

	befshdtype = curshdtype;
	curshdtype = SHDGROUNDDATA;

	char gdname[1024];
	ZeroMemory( gdname, 1024 );

	SYSTEMTIME systime;
	GetLocalTime( &systime );
	sprintf_s( gdname, 1024, "GroundData_%d_%d_%d_%d_%d_%d_%d",
		systime.wYear,
		systime.wMonth,
		systime.wDay,
		systime.wHour,
		systime.wMinute,
		systime.wSecond,
		systime.wMilliseconds
	);


	ret = AddShape2Tree( gdname, 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;
	}

	//tempinfo.SetMem( &spcvec, MAT_SPECULAR );

	ret = SetMeshInfo( &tempinfo, SHDGROUNDDATA, divx, divz );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;
	}
/////// PolyMeshの作成
	//int pmno, pmnum;
	int bfnum, pmvnum, pmfnum;
	char pmname[1024];

	CGroundData* gdataptr;

	gdataptr = (*m_lpsh)( curseri )->grounddata;
	_ASSERT( gdataptr );



	ret = gdataptr->CreateInvisibleWallObj( m_ntsc_y1, miny1, maxy1, m_bmpw, m_bmph, maxx, maxz );
	if( ret ){
		DbgOut( "groundbmp : LoadMovableBMP : gdataptr PrepareInvisibleWallObj error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;
	}


//// polymeshの作成。
	int pmnum;
	pmnum = gdataptr->m_pmnum;//!!!

	int pmno;
	setpmno = 0;
	for( pmno = 0; pmno < pmnum; pmno++ ){

		ret = gdataptr->GetBorderFaceNum( pmno, &bfnum );
		if( ret ){
			DbgOut( "groundbmp : LoadMovableBMP : gdataptr GetBorderFaceNum error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;
		}

		if( bfnum == 0 ){
			ret = gdataptr->SetPolyMeshPtr( pmno, 0 );
			if( ret ){
				_ASSERT( 0 );
				ret = 1;
				goto loadmvbmpexit;
			}
			continue;//!!!!!!
		}

		pmvnum = bfnum * 4;
		pmfnum = bfnum * 2;

		///////
		befseri = curseri;
		curseri = 1 + setpmno + 1;// 0は、CreateHandlerで作成済

		befdepth = curdepth;
		curdepth = 2;// 0は、CreateHandlerで作成済

		befshdtype = curshdtype;
		curshdtype = SHDPOLYMESH;
		////////

		sprintf_s( pmname, 1024, "polymesh_of_G_IW_%d", setpmno );

		ret = AddShape2Tree( pmname, 0 );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;
		}

		/////////

		ret = SetMeshInfo( &tempinfo, SHDPOLYMESH, pmvnum, pmfnum );
		if( ret ){
			DbgOut( "groundbmp : LoadMovableBMP : SetMeshInfo pm error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;
		}
		////////
		ret = Init3DObj();
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;
		}


		ret = (*m_lpsh)( curseri )->CreatePolyMeshIndex( pmfnum );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;
		}

		/////////
		CPolyMesh* pmptr;
		pmptr = (*m_lpsh)( curseri )->polymesh;
		_ASSERT( pmptr );
		ret = gdataptr->SetPolyMeshPtr( pmno, pmptr );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;
		}

		ret = pmptr->SetGroundFlag( 1 );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;	
		}

		//ret = pmptr->CreateTextureBuffer();
		//if( ret ){
		//	_ASSERT( 0 );
		//	ret = 1;
		//	goto loadmvbmpexit;
		//}
		ret = gdataptr->SetInvisibleWallPM( pmno, pmvnum, pmfnum, divx, divz, maxx, maxz, wallheight );
		if( ret ){
			DbgOut( "groundbmp : LoadMovableBMP : gdataptr SetInvisibleWallPM error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto loadmvbmpexit;
		}


		int faceno;
		for( faceno = 0; faceno < pmptr->meshinfo->n; faceno++ ){
			*( pmptr->m_attrib0 + faceno ) = -1;
		}

		setpmno++;

	}


	ret = gdataptr->DestroyInvisibleWallObj();
	if( ret ){
		DbgOut( "groundbmp : LoadMovableBMP : gdataptr DestroyInvisibleWallObj error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;
	}

DbgOut( "groundbmp : LoadMovableBMP : pmnum %d, setpmno %d\n", pmnum, setpmno );	

	ret = m_lpsh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;	
	}
	ret = m_lpsh->SetColors();
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;	
	}
	ret = m_lpmh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadmvbmpexit;	
	}


	goto loadmvbmpexit;
loadmvbmpexit:
	if( hfile1 )
		CloseHandle( hfile1 );

	if( m_ntsc_y1 ){
		free( m_ntsc_y1 );
		m_ntsc_y1 = 0;
	}

	return ret;
}

int CGroundBMP::LoadGroundBMP( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, 
		char* bmpname1,
		char* bmpname2, 
		char* bmpname3, 
		char* texname, 
		float maxx, float maxz, 
		int divx, int divz, 
		float maxheight )
{
	int ret = 0;

	HANDLE hfile1 = 0;
	HANDLE hfile2 = 0;
	HANDLE hfile3 = 0;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	int notfound1 = 0;
	int notfound2 = 0;
	int notfound3 = 0;

	m_ntsc_y1 = GetBmpY( bmpname1, &hfile1, &m_hdr1, &m_biheader1, &miny1, &maxy1, &notfound1 );
	if( !m_ntsc_y1 ){
		DbgOut( "GroundBMP : LoadGroundBMP : file1 bmp error !!!\n" );
		_ASSERT( 0 );
		if( notfound1 == 1 )
			ret = D3DAPPERR_MEDIANOTFOUND;
		else
			ret = 1;
		goto loadgbmpexit;
	}

	m_bmpw = m_biheader1.biWidth;
	m_bmph = m_biheader1.biHeight;


	m_ntsc_y2 = GetBmpY( bmpname2, &hfile2, &m_hdr2, &m_biheader2, &miny2, &maxy2, &notfound2 );
	//m_rgbdat2 == 0 も有り！！！
	if( m_ntsc_y2 && ((m_bmpw != m_biheader2.biWidth) || (m_bmph != m_biheader2.biHeight)) ){
		DbgOut( "GroundBMP : LoadGroundBMP : file2 size error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

	m_ntsc_y3 = GetBmpY( bmpname3, &hfile3, &m_hdr3, &m_biheader3, &miny3, &maxy3, &notfound3 );
	//m_rgbdat3 == 0 も有り！！！
	if( m_ntsc_y3 && ((m_bmpw != m_biheader3.biWidth) || (m_bmph != m_biheader3.biHeight)) ){
		DbgOut( "GroundBMP : LoadGroundBMP : file3 size error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}


////////////////
	ret = lpsh->InitGroundBMPMaterial( texname );
	if( ret ){
		DbgOut( "groundbmp : LoadGroundBMP : sh InitGroundBMPMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
//////////	
	InitLoadParams();

	ret = m_lpth->Start( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

///// CGroundDataの作成
	befseri = curseri;
	curseri = 1;// 0は、CreateHandlerで作成済

	befdepth = curdepth;
	curdepth = 1;// 0は、CreateHandlerで作成済

	befshdtype = curshdtype;
	curshdtype = SHDGROUNDDATA;

	char gdname[1024];
	ZeroMemory( gdname, 1024 );

	SYSTEMTIME systime;
	GetLocalTime( &systime );
	sprintf_s( gdname, 1024, "GroundData_%d_%d_%d_%d_%d_%d_%d",
		systime.wYear,
		systime.wMonth,
		systime.wDay,
		systime.wHour,
		systime.wMinute,
		systime.wSecond,
		systime.wMilliseconds
	);


	ret = AddShape2Tree( gdname, 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

	ret = SetMeshInfo( &tempinfo, SHDGROUNDDATA, divx, divz );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}
/////// PolyMeshの作成
	int pmno, pmnum;
	char pmname[1024];
	RECT* pmrect;
	RECT* currect;

	CGroundData* gdataptr;

	gdataptr = (*m_lpsh)( curseri )->grounddata;
	_ASSERT( gdataptr );

	pmnum = gdataptr->m_pmnum;
	pmrect = gdataptr->m_rect;

	int pmvnum, pmfnum;
	for( pmno = 0; pmno < pmnum; pmno++ ){

		befseri = curseri;
		curseri = 1 + pmno + 1;// 0は、CreateHandlerで作成済

		befdepth = curdepth;
		curdepth = 2;// 0は、CreateHandlerで作成済

		befshdtype = curshdtype;
		curshdtype = SHDPOLYMESH;
		////////

		sprintf_s( pmname, 1024, "polymesh_of_G_%d", pmno );
		currect = pmrect + pmno;


//DbgOut( "groundbmp : LoadGroundBMP : bef AddShape : befseri %d, curseri %d, befdepth %d, curdepth %d, befshdtype %d, curshdtype %d\n",
//	   befseri, curseri, befdepth, curdepth, befshdtype, curshdtype );


		ret = AddShape2Tree( pmname, texname );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		/////////

		
		pmvnum = ( currect->right - currect->left + 1 ) * ( currect->top - currect->bottom + 1);
		pmfnum = ( currect->right - currect->left ) * ( currect->top - currect->bottom ) * 2;

		ret = SetMeshInfo( &tempinfo, SHDPOLYMESH, pmvnum, pmfnum );
		if( ret ){
			DbgOut( "groundbmp : LoadGroundBMP : SetMeshInfo pm error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}
		////////
		ret = Init3DObj();
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}


		ret = (*m_lpsh)( curseri )->CreatePolyMeshIndex( pmfnum );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		/////////
		CPolyMesh* pmptr;
		pmptr = (*m_lpsh)( curseri )->polymesh;
		_ASSERT( pmptr );
		ret = gdataptr->SetPolyMeshPtr( pmno, pmptr );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		ret = pmptr->SetGroundFlag( 1 );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		ret = pmptr->CreateTextureBuffer();
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

	}
//////// bmp dataの読み込み、pmへのセット
	
	//ret = gdataptr->SetGroundData( m_rgbdat1, rgbflag1, m_rgbdat2, rgbflag2, m_rgbdat3, rgbflag3,
	//	m_bmpw, m_bmph,
	//	maxx, maxz, divx, divz, maxheight );
	ret = gdataptr->SetGroundData( m_ntsc_y1, miny1, maxy1, m_ntsc_y2, miny2, maxy2, m_ntsc_y3, miny3, maxy3,
		m_bmpw, m_bmph,
		maxx, maxz, divx, divz, maxheight );

	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

////////
	ret = m_lpsh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}
	ret = m_lpsh->SetColors();
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}
	ret = m_lpmh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}


	goto loadgbmpexit;

loadgbmpexit :
	if( hfile1 )
		CloseHandle( hfile1 );
	if( hfile2 )
		CloseHandle( hfile2 );
	if( hfile3 )
		CloseHandle( hfile3 );

	if( m_rgbdat1 ){
		free( m_rgbdat1 );
		m_rgbdat1 = 0;
	}
	if( m_rgbdat2 ){
		free( m_rgbdat2 );
		m_rgbdat2 = 0;
	}
	if( m_rgbdat3 ){
		free( m_rgbdat3 );
		m_rgbdat3 = 0;
	}

	if( m_ntsc_y1 ){
		free( m_ntsc_y1 );
		m_ntsc_y1 = 0;
	}

	if( m_ntsc_y2 ){
		free( m_ntsc_y2 );
		m_ntsc_y2 = 0;
	}

	if( m_ntsc_y3 ){
		free( m_ntsc_y3 );
		m_ntsc_y3 = 0;
	}

	return ret;
}


int CGroundBMP::LoadGroundBMPTS( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, 
		char* bmpname1,
		float maxx, float maxz, 
		int divx, int divz, 
		float maxheight )
{
	int ret = 0;

	HANDLE hfile1 = 0;
	HANDLE hfile2 = 0;
	HANDLE hfile3 = 0;

	m_lpth = lpth;
	m_lpsh = lpsh;
	m_lpmh = lpmh;

	int notfound1 = 0;
	int notfound2 = 0;
	int notfound3 = 0;

	int revflag = 1;//!!!!!!!!!

	m_ntsc_y1 = GetBmpY( bmpname1, &hfile1, &m_hdr1, &m_biheader1, &miny1, &maxy1, &notfound1, revflag );
	if( !m_ntsc_y1 ){
		DbgOut( "GroundBMP : LoadGroundBMP : file1 bmp error !!!\n" );
		_ASSERT( 0 );
		if( notfound1 == 1 )
			ret = D3DAPPERR_MEDIANOTFOUND;
		else
			ret = 1;
		goto loadgbmpexit;
	}

	m_bmpw = m_biheader1.biWidth;
	m_bmph = m_biheader1.biHeight;


////////////////
	ret = lpsh->InitGroundBMPMaterial( 0 );
	if( ret ){
		DbgOut( "groundbmp : LoadGroundBMP : sh InitGroundBMPMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
//////////	
	InitLoadParams();

	ret = m_lpth->Start( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

///// CGroundDataの作成
	befseri = curseri;
	curseri = 1;// 0は、CreateHandlerで作成済

	befdepth = curdepth;
	curdepth = 1;// 0は、CreateHandlerで作成済

	befshdtype = curshdtype;
	curshdtype = SHDGROUNDDATA;

	char gdname[1024];
	ZeroMemory( gdname, 1024 );

	SYSTEMTIME systime;
	GetLocalTime( &systime );
	sprintf_s( gdname, 1024, "GroundData_%d_%d_%d_%d_%d_%d_%d",
		systime.wYear,
		systime.wMonth,
		systime.wDay,
		systime.wHour,
		systime.wMinute,
		systime.wSecond,
		systime.wMilliseconds
	);


	ret = AddShape2Tree( gdname, 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

	ret = SetMeshInfo( &tempinfo, SHDGROUNDDATA, divx, divz, -1 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}
/////// PolyMeshの作成
	int pmno, pmnum;
	char pmname[1024];
	RECT* pmrect;
	RECT* currect;

	CGroundData* gdataptr;

	gdataptr = (*m_lpsh)( curseri )->grounddata;
	_ASSERT( gdataptr );

	pmnum = gdataptr->m_pmnum;
	pmrect = gdataptr->m_rect;

	_ASSERT( pmnum == 1 );

	int pmvnum, pmfnum;
	for( pmno = 0; pmno < pmnum; pmno++ ){

		befseri = curseri;
		curseri = 1 + pmno + 1;// 0は、CreateHandlerで作成済

		befdepth = curdepth;
		curdepth = 2;// 0は、CreateHandlerで作成済

		befshdtype = curshdtype;
		curshdtype = SHDPOLYMESH;
		////////

		sprintf_s( pmname, 1024, "polymesh_of_G_%d", pmno );
		currect = pmrect + pmno;


//DbgOut( "groundbmp : LoadGroundBMP : bef AddShape : befseri %d, curseri %d, befdepth %d, curdepth %d, befshdtype %d, curshdtype %d\n",
//	   befseri, curseri, befdepth, curdepth, befshdtype, curshdtype );


		ret = AddShape2Tree( pmname, 0 );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		/////////

		
		pmvnum = ( currect->right - currect->left + 1 ) * ( currect->top - currect->bottom + 1);
		pmfnum = ( currect->right - currect->left ) * ( currect->top - currect->bottom ) * 2;

		ret = SetMeshInfo( &tempinfo, SHDPOLYMESH, pmvnum, pmfnum );
		if( ret ){
			DbgOut( "groundbmp : LoadGroundBMP : SetMeshInfo pm error !!!\n" );
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}
		////////
		ret = Init3DObj();
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}


		ret = (*m_lpsh)( curseri )->CreatePolyMeshIndex( pmfnum );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		/////////
		CPolyMesh* pmptr;
		pmptr = (*m_lpsh)( curseri )->polymesh;
		_ASSERT( pmptr );
		ret = gdataptr->SetPolyMeshPtr( pmno, pmptr );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		ret = pmptr->SetGroundFlag( 1 );
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

		ret = pmptr->CreateTextureBuffer();
		if( ret ){
			_ASSERT( 0 );
			ret = 1;
			goto loadgbmpexit;
		}

	}
//////// bmp dataの読み込み、pmへのセット
	
	//ret = gdataptr->SetGroundData( m_rgbdat1, rgbflag1, m_rgbdat2, rgbflag2, m_rgbdat3, rgbflag3,
	//	m_bmpw, m_bmph,
	//	maxx, maxz, divx, divz, maxheight );
	ret = gdataptr->SetGroundDataTS( m_ntsc_y1, miny1, maxy1,
		m_bmpw, m_bmph,
		maxx, maxz, divx, divz, maxheight );

	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}

////////
	ret = m_lpsh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}
	ret = m_lpsh->SetColors();
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}
	ret = m_lpmh->SetChain( 0 );
	if( ret ){
		_ASSERT( 0 );
		ret = 1;
		goto loadgbmpexit;
	}


	goto loadgbmpexit;

loadgbmpexit :
	if( hfile1 )
		CloseHandle( hfile1 );
	if( hfile2 )
		CloseHandle( hfile2 );
	if( hfile3 )
		CloseHandle( hfile3 );

	if( m_rgbdat1 ){
		free( m_rgbdat1 );
		m_rgbdat1 = 0;
	}
	if( m_rgbdat2 ){
		free( m_rgbdat2 );
		m_rgbdat2 = 0;
	}
	if( m_rgbdat3 ){
		free( m_rgbdat3 );
		m_rgbdat3 = 0;
	}

	if( m_ntsc_y1 ){
		free( m_ntsc_y1 );
		m_ntsc_y1 = 0;
	}

	if( m_ntsc_y2 ){
		free( m_ntsc_y2 );
		m_ntsc_y2 = 0;
	}

	if( m_ntsc_y3 ){
		free( m_ntsc_y3 );
		m_ntsc_y3 = 0;
	}

	return ret;
}



float* CGroundBMP::GetBmpY( char* fname, HANDLE* fileptr, BITMAPFILEHEADER* hdrptr, BITMAPINFOHEADER* biheaderptr, float* minntscy, float* maxntscy, int* notfound, int revflag )
{

	RGBDAT* rgbptr = 0;
	HANDLE hfile;

	*notfound = 0;

	hfile = CreateFile( (LPCTSTR)fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		*notfound = 1;//!!!!!!!!
		*fileptr = 0;
		return 0;
	}	
	*fileptr = hfile;


	DWORD rleng, readleng;
	rleng = sizeof( BITMAPFILEHEADER );
	ReadFile( hfile, (void*)(hdrptr), rleng, &readleng, NULL );
	if( rleng != readleng ){
		_ASSERT( 0 );
		return 0;
	}

	//magic no. check
	if( hdrptr->bfType != 0x4D42 ){
		DbgOut( "GroundBMP : GetBmpPtr : file type error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	rleng = sizeof( BITMAPINFOHEADER );
	ReadFile( hfile, (void*)(biheaderptr), rleng, &readleng, NULL );
	if( rleng != readleng ){
		_ASSERT( 0 );
		return 0;
	}
	
	//bpp 24bit check
	if( biheaderptr->biBitCount != 24 ){
		DbgOut( "GroundBMP : GetBmpPtr : bpp is not 24 error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	
	DWORD div4, bmppitch;
	div4 = ( biheaderptr->biWidth * (biheaderptr->biBitCount / 8) + 3 ) / 4;
	bmppitch = div4 * 4;//!!!!!!!!!!!!!!!
	

	DWORD datasize;
	//datasize = sizeof( RGBDAT ) * biheaderptr->biWidth * biheaderptr->biHeight;
	datasize = biheaderptr->biSizeImage;//!!!!!!!!!!!!!!!!!!!!
	if( datasize <= 0 ){
		datasize = bmppitch * biheaderptr->biHeight;
	}
	
// 読み込みデータ長と、ntscyのデータ長が異なることに注意！！　ピッチが関係するから。

	rgbptr = (RGBDAT*)malloc( datasize );
	if( !rgbptr ){
		DbgOut( "GroundBMP : GetBmpPtr : retrgb alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	ZeroMemory( rgbptr, datasize );


	rleng = datasize;
	ReadFile( hfile, (void*)(rgbptr), rleng, &readleng, NULL );
	if( rleng != readleng ){
		_ASSERT( 0 );
		free( rgbptr );
		return 0;
	}
////////
	DWORD datasize2;
	datasize2 = sizeof( float ) * biheaderptr->biWidth * biheaderptr->biHeight;
	float* ntscy;
	ntscy = (float*)malloc( datasize2 );
	if( !ntscy ){
		_ASSERT( 0 );
		free( rgbptr );
		return 0;
	}
	ZeroMemory( ntscy, datasize2 );

	int w, h;
	int setno = 0;
	BYTE* rgbhead;
	RGBDAT* currgb;
	float cury;
	float maxy = -100;
	float miny = 500;
	for( h = 0; h < biheaderptr->biHeight; h++ ){
		rgbhead = (BYTE*)rgbptr + bmppitch * h;//!!!!!!!!!!!!! ピッチはバイト数
		for( w = 0; w < biheaderptr->biWidth; w++ ){
			//currgb = rgbptr + biheaderptr->biWidth * h + w;
			currgb = (RGBDAT*)rgbhead + w;// <--------------- ピッチを考慮！！！！！！
			cury = 0.59f * (float)currgb->r + 0.30f * (float)currgb->g + 0.11f * (float)currgb->b;
			*(ntscy + setno) = cury;

			if( cury > maxy )
				maxy = cury;
			if( cury < miny )
				miny = cury;

			setno++;
		}
	}

	int setnum = setno;

	if( revflag == 1 ){
		int setindex;
		for( setindex = 0; setindex < setnum; setindex++ ){
			float cury = *(ntscy + setindex);

			float newy = cury - maxy + miny;
			*(ntscy + setindex) = newy;
		}

		*minntscy = maxy;
		*maxntscy = miny;
	}else{
		*minntscy = miny;
		*maxntscy = maxy;
	}

	free( rgbptr );

	return ntscy;
}


int CGroundBMP::AddShape2Tree( char* elemname, char* texname )
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

	ret = (*m_lpsh)( curseri )->SetTexTransparent( 0 );
	_ASSERT( !ret );

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

int CGroundBMP::SetMeshInfo( CMeshInfo* dstmi, int mitype, int mim, int min, int skip )
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
	//spec.x = 0.2f; spec.y = 0.2f; spec.z = 0.2f;
	spec.x = 0.0f; spec.y = 0.0f; spec.z = 0.0f;
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

	dstmi->skip = skip;//!!!!!!!!!!!!!!


	return 0;
}

int CGroundBMP::Init3DObj()
{
	int ret;

	ret = m_lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CGroundBMP::DbgOutBMPHeader( char* bmpname )
{

	RGBDAT* rgbptr = 0;
	HANDLE hfile;

	BITMAPFILEHEADER* hdrptr = &m_hdr1;
	BITMAPINFOHEADER* biheaderptr = &m_biheader1;


	hfile = CreateFile( (LPCTSTR)bmpname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		_ASSERT( 0 );
		return 1;
	}	


	DWORD rleng, readleng;
	rleng = sizeof( BITMAPFILEHEADER );
	ReadFile( hfile, (void*)(hdrptr), rleng, &readleng, NULL );
	if( rleng != readleng ){
		_ASSERT( 0 );
		return 1;
	}

	//magic no. check
	if( hdrptr->bfType != 0x4D42 ){
		DbgOut( "GroundBMP : GetBmpPtr : file type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	rleng = sizeof( BITMAPINFOHEADER );
	ReadFile( hfile, (void*)(biheaderptr), rleng, &readleng, NULL );
	if( rleng != readleng ){
		_ASSERT( 0 );
		return 1;
	}
	

	DbgOut( "DbgOutBMPHeader : filename %s\n", bmpname );
	DbgOut( "DbgOutBMPHeader : width %d, height %d, bpp %d, clrused %d\n",
		biheaderptr->biWidth,
		biheaderptr->biHeight,
		biheaderptr->biBitCount,
		biheaderptr->biClrUsed
	);



	//bpp 24bit check
	//if( biheaderptr->biBitCount != 24 ){
	//	DbgOut( "GroundBMP : GetBmpPtr : bpp is not 24 error !!!\n" );
	//	_ASSERT( 0 );
	//	return 0;
	//}


	if( hfile )
		CloseHandle( hfile );


	return 0;
}




