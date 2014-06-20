#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <texbank.h>

//#include <D3DX9.h>

//#include "c:\program files\microsoft platform sdk\samples\multimedia\common\include\D3DUtil.h"
//#include "c:\program files\microsoft platform sdk\samples\multimedia\common\include\DXUtil.h"
//#include "c:\program files\microsoft sdk\samples\multimedia\common\include\D3DUtil.h"
//#include "c:\program files\microsoft sdk\samples\multimedia\common\include\DXUtil.h"

//#include "c:\DXSDK\samples\multimedia\common\include\D3DUtil.h"
//#include "c:\DXSDK\samples\multimedia\common\include\DXUtil.h"

#include "c:\pgfile9\e3dhsp3\D3DUtil.h"
#include "c:\pgfile9\e3dhsp3\DXUtil.h"

#include <Panda.h>

#define	DBGH
#include <dbg.h>

static int s_destroycount = 0;


CTexBank::CTexBank( UINT miplevels, DWORD mipfilter )
{
	pptexdata = 0;
	ppsurfdata = 0;
	ppbufdata = 0;
	ppdirname = 0;

	m_miplevels = miplevels;
	m_mipfilter = mipfilter;

	ptexapplyindex = 0;

	DbgOut( "texbank : miplevels %d\n", miplevels );
}

CTexBank::~CTexBank()
{
	DestroyTexData();
	//CNameBank::~CNameBank();
}

int CTexBank::DestroyTexData()
{
	int i;

	if( pptexdata || ppsurfdata || ppbufdata ){
		for( i = 0; i < namenum; i++ ){
			
			//InvalidateTexture( i );
			DestroyTexture( i );

			char* delname;
			delname = *( ppdirname + i );
			if( delname ){
				free( delname );
			}

		}

		if( pptexdata )
			free( pptexdata );
		pptexdata = 0;
		if( ppsurfdata )
			free( ppsurfdata );
		ppsurfdata = 0;
		if( ppbufdata )
			free( ppbufdata );
		ppbufdata = 0;

		if( ppdirname )
			free( ppdirname );
		ppdirname = 0;

		if( ptexapplyindex )
			free( ptexapplyindex );
		ptexapplyindex = 0;
	}
	return 0;
}

int CTexBank::SetTextureAlpha( LPDIRECT3DTEXTURE9 pd3dtex )
{
	DbgOut( "texbank : SetTextureAlpha : start\r\n" );

	if( !pd3dtex ){
		DbgOut( "texbank : SetTextureAlpha : pd3dtex NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	D3DSURFACE_DESC	sdesc;
	hr = pd3dtex->GetLevelDesc( 0, &sdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : SetTextureAlpha : GetLevelDesc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DFORMAT fmt = sdesc.Format;

	if( 
	(fmt != D3DFMT_X1R5G5B5)
	&& (fmt != D3DFMT_A1R5G5B5)

	//&& (fmt != D3DFMT_R5G6B5)
	&& (fmt != D3DFMT_X8R8G8B8)
	&& (fmt != D3DFMT_A8R8G8B8)
	//&& (fmt != D3DFMT_R8G8B8)
	){
		DbgOut( "TexBank : SetTextureAlpha : d3dformat %d is not supported error !!!\n",
			fmt );
		_ASSERT( 0 );
		return 1;
	}

	int w = sdesc.Width;
	int h = sdesc.Height;
	D3DLOCKED_RECT lockedrect;
	RECT rect;

	rect.left = 0; rect.right = w; rect.top = 0; rect.bottom = h;
	hr = pd3dtex->LockRect( 0, &lockedrect, &rect, 0 );
	if( hr != D3D_OK ){
		DbgOut( "texbank : SetTextureAlpha : LockRect error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	BYTE*	bitshead;
	DWORD* dwbits;
	WORD* wbits;

	int height, width;
	for( height = 0; height < h; height++ ){
			
		bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * height;
		for( width = 0; width < w; width++ ){
				
			if( (fmt == D3DFMT_A8R8G8B8) || (fmt == D3DFMT_X8R8G8B8) ){
				dwbits = (DWORD*)bitshead + width;

				DWORD col = *dwbits & 0xFFFFFF;
				DWORD colr, colg, colb;
				colr = (col & 0x00FF0000) >> 16;
				colg = (col & 0x0000FF00) >> 8;
				colb = (col & 0x000000FF);
				//if( col == 0 ){
				if( (colr == 0) && (colg == 0) && (colb == 0) ){
					//alpha 0
					*dwbits = 0;
				}else{
					//alpha 255
					*dwbits = col | 0xFF000000;
				}
			}else if( (fmt == D3DFMT_A1R5G5B5) || (fmt == D3DFMT_X1R5G5B5) ){
				wbits = (WORD*)bitshead + width;
				WORD col = *wbits & 0x7FFF;
				WORD colr, colg, colb;
				colr = (col & 0x7C00) >> 10;
				colg = (col & 0x03E0) >> 5;
				colb = (col & 0x001F);
				//if( col == 0 ){
				if( (colr == 0) && (colg == 0) && (colb == 0) ){
					*wbits = 0;
				}else{
					*wbits = col | 0x8000;
				}
			}

		}
	
	}

	hr = pd3dtex->UnlockRect( 0 );
	if( hr != D3D_OK ){
		DbgOut( "texbank : SetTextureAlpha : UnlockRect error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DbgOut( "texbank : SetTextureAlpha : end\r\n" );

	return 0;
}

int CTexBank::CreateTexData( int datano, LPDIRECT3DDEVICE9 pdev, int srcsizex, int srcsizey )
{
	char* temptexname = 0;
	LPDIRECT3DTEXTURE9 newtex = 0;
	char fullname[1024];
	char fullname2[1024];
	int fullpathflag = 0;
	char* dirname = 0;

	char resdir[1024];
	int resdirleng;

	if( (datano < 0) || (datano >= namenum) ){
		DbgOut( "texbank : CreateTexData : datano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int appindex = *( ptexapplyindex + datano );

	if( *( pptexdata + datano ) || *( ppsurfdata + datano ) || (appindex >= 0) ){
		DbgOut( "texbank : CreateTexData : data allready set, skip !!!\r\n" );
		return -2;//!!!!!!!!!!!!!!!!!!!!!!
	}

	temptexname = (*this)( datano );
	dirname = *( ppdirname + datano );



///// main window
	int cmpmainwin;
	cmpmainwin = strncmp( temptexname, "MainWindow_", 11 );
	if( cmpmainwin == 0 ){
		return CreateTexDataFromMainWindow( datano, pdev );//!!!!!!!!!!!!!!!!!!!!
	}

//// RenderTarget
	int cmprt;
	cmprt = strncmp( temptexname, "RenderTarget_", 13 );

//////////
	char texname[4098];
	char* texidptr;
	texidptr = strstr( temptexname, "_TEXID" );
	if( texidptr ){
		int leng;
		leng = (int)(texidptr - temptexname);
		strncpy_s( texname, 4098, temptexname, leng );
		texname[ leng ] = 0;

	}else{
		strcpy_s( texname, 4098, temptexname );
	}

////////
	char destroyname[13] = "__Destroyed_";
	int cmpdest;
	cmpdest = strncmp( texname, destroyname, 12 );
	if( cmpdest == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
/////////

	// FromBuf!!!

	int cmpfrombuf;
	cmpfrombuf = strncmp( temptexname, "FromBuf_TEXID", 13 );
	if( cmpfrombuf != 0 ){
		cmpfrombuf = strncmp( temptexname, "FromBMSCR_TEXID", 15 );
	}
	if( cmpfrombuf != 0 ){
		DATAFROMBUF* setbuf;
		setbuf = ppbufdata + datano;
		if( !setbuf ){
			_ASSERT( 0 );
			return 1;
		}
		if( setbuf->pdata && (setbuf->leng > 0) ){
			cmpfrombuf = 0;
		}
	}


//////////
	HRESULT hr, hrsize;

	int pool;
	pool = GetIntData2( datano );

//pool = D3DPOOL_DEFAULT;
//pool = D3DPOOL_MANAGED;
	if( appindex >= 0 ){
		//
	}else if( cmprt == 0 ){
		int createfmt;
		createfmt = GetFMT( datano );

		int createx, createy;
		if( (srcsizex == 0) || (srcsizey == 0) ){
			createx = *( pidata3 + datano );
			createy = *( pidata4 + datano );
		}else{
			createx = srcsizex;
			createy = srcsizey;
		}


		hr = pdev->CreateTexture(
				createx, createy, 1, 
				D3DUSAGE_RENDERTARGET, 
				//D3DFMT_A8R8G8B8,
				//D3DFMT_A16B16G16R16F,
				(D3DFORMAT)createfmt,
				D3DPOOL_DEFAULT, &newtex, NULL);
		if( hr != D3D_OK ){
			DbgOut( "texbank : CreateTexData : rendertarget : pdev CreateTexture error %d, %d!!!\n", createx, createy );
			_ASSERT( 0 );
			return 1;
		}

		(porgsize + datano)->cx = createx;
		(porgsize + datano)->cy = createy;

	}else if( (cmpfrombuf != 0) && !((ppbufdata + datano)->pdata) ){//名前がファイル名でbufにセットがあるのがPndなのでbufセットがあるものははじく

		int ch = '\\';
		char* enptr;
		enptr = strchr( texname, ch );

		ZeroMemory( fullname, 1024 );

		if( enptr ){
			//フルパス
			strcpy_s( fullname, 1024, texname );
			fullpathflag = 1;//!!!!!!

			//上記ファイルが無かったときは、RESDIRを探す。
			char* lastenptr;
			lastenptr = strrchr( texname, ch );
			_ASSERT( lastenptr );

			ZeroMemory( resdir, 1024 );
			resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)resdir, 1024 );
			_ASSERT( resdirleng );
			strcpy_s( fullname2, 1024, resdir );
			strcat_s( fullname2, 1024, lastenptr + 1 );

		}else{
			if( dirname && *dirname ){
				strcpy_s( fullname, 1024, dirname );
				strcat_s( fullname, 1024, texname );
			}else{
				ZeroMemory( resdir, 1024 );
				resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)resdir, 1024 );
				_ASSERT( resdirleng );
				strcpy_s( fullname, 1024, resdir );
				strcat_s( fullname, 1024, texname );
			}
		}

		hr = D3DXCreateTextureFromFileEx( pdev, (LPTSTR)fullname, 
							D3DX_DEFAULT, D3DX_DEFAULT, 
							m_miplevels,//!!!!!!! 
							0, D3DFMT_A8R8G8B8, 
							(D3DPOOL)pool, 
							D3DX_FILTER_TRIANGLE,// | D3DX_FILTER_MIRROR, 
							//D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
							m_mipfilter,//!!!!!!! 
							0, NULL, NULL, &newtex );
		if( hr == D3D_OK ){
			D3DXIMAGE_INFO info1;
			hrsize = D3DXGetImageInfoFromFile( fullname, &info1 );
			if( hrsize == D3D_OK ){
				(porgsize + datano)->cx = info1.Width;
				(porgsize + datano)->cy = info1.Height;
			}else{
				DbgOut( "texbank : CreateTexData : GetImageInfoFromFile error !!! skip size setting %s\r\n", fullname );
				_ASSERT( 0 );
			}
		}


		if( hr != D3D_OK ){
			if( fullpathflag == 1 ){
				HRESULT hr2;

				hr2 = D3DXCreateTextureFromFileEx( pdev, (LPTSTR)fullname2, 
									D3DX_DEFAULT, D3DX_DEFAULT, 
									m_miplevels,//!!!!!!! 
									0, D3DFMT_A8R8G8B8, 
									(D3DPOOL)pool, 
									D3DX_FILTER_TRIANGLE,// | D3DX_FILTER_MIRROR, 
									//D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
									m_mipfilter,//!!!!!!! 
									0, NULL, NULL, &newtex );
				if( hr2 != D3D_OK ){
					DbgOut( "texbank : CreateTexData : D3DXCreateTextureFromFileEx error %x, %s!!!\n", hr, fullname );
					_ASSERT( 0 );
					return 1;
				}else{
					D3DXIMAGE_INFO info2;
					hrsize = D3DXGetImageInfoFromFile( fullname2, &info2 );
					if( hrsize == D3D_OK ){
						(porgsize + datano)->cx = info2.Width;
						(porgsize + datano)->cy = info2.Height;
					}else{
						DbgOut( "texbank : CreateTexData : GetImageInfoFromFile error !!! skip size setting %s\r\n", fullname );
						_ASSERT( 0 );
					}
				}
			}else{
				DbgOut( "texbank : CreateTexData : D3DXCreateTextureFromFileEx error %x, %s!!!\n", hr, fullname );
				//_ASSERT( 0 );
				return 1;
			}
		}

	}else{

		DATAFROMBUF* dataptr;
		dataptr = ppbufdata + datano;

		if( !dataptr || !dataptr->pdata || (dataptr->leng <= 0) ){
			DbgOut( "texbank : CreateTexData : FromBuf : ppbufdata NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hr = D3DXCreateTextureFromFileInMemoryEx( pdev, dataptr->pdata, dataptr->leng, 
			D3DX_DEFAULT, D3DX_DEFAULT, 
			m_miplevels,
			0, D3DFMT_A8R8G8B8,
			(D3DPOOL)pool,
			D3DX_FILTER_TRIANGLE,// | D3DX_FILTER_MIRROR,
			//D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
			m_mipfilter,//!!!!!!! 
			0, NULL, NULL, &newtex );
		if( hr != D3D_OK ){
			DbgOut( "texbank : CreateTexData : D3DXCreateTextureFromFileInMemoryEx error %x!!!\n", hr );
			_ASSERT( 0 );
			return 1;
		}else{
			D3DXIMAGE_INFO info3;
			hrsize = D3DXGetImageInfoFromFileInMemory( dataptr->pdata, dataptr->leng, &info3 );
			if( hrsize == D3D_OK ){
				(porgsize + datano)->cx = info3.Width;
				(porgsize + datano)->cy = info3.Height;
			}else{
				DbgOut( "texbank : CreateTexData : GetImageInfoFromFile error !!! skip size setting frommem\r\n" );
				_ASSERT( 0 );
			}
		}
	}


	*( pptexdata + datano ) = newtex;

	if( newtex ){
		D3DSURFACE_DESC desc;
		hr = newtex->GetLevelDesc( 0, &desc );
		if( hr != D3D_OK ){
			DbgOut( "texbank : CreateTexData : newtex GetLevelDesc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*( pidata3 + datano ) = desc.Width;
		*( pidata4 + datano ) = desc.Height;

		*( Type + datano ) = TEXTYPE_TEXTURE;//!!!!!!!!


		int ret;
		int transparent;
		transparent = GetIntData( datano );
		if( (transparent == 1) && (pool == D3DPOOL_MANAGED) ){
			DbgOut( "texbank : CreateTexData : SetTextureAlpha call %s\r\n", temptexname );
			ret = SetTextureAlpha( newtex );
			if( ret ){
				DbgOut( "texbank : CreateTexData : SetTextureAlpha error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		DbgOut( "texbank : CreateTexData : newtex NULL warning !!! %s\r\n", temptexname );
	}

	return 0;
}

int CTexBank::LoadTextureFromBuf( int srcindex, D3DCOLOR colkey )
{

	DATAFROMBUF* dataptr;
	dataptr = ppbufdata + srcindex;

	if( !dataptr || !dataptr->pdata || (dataptr->leng <= 0) ){
		DbgOut( "texbank : LoadTextureFromBuf : ppbufdata NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 lpsurf = 0;
	int textype = 0;
	lpsurf = GetSurface( srcindex, &textype );
	if( !lpsurf ){
		DbgOut( "texbank : LoadTextureFromBuf : GetSurface error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//	HRESULT D3DXLoadSurfaceFromFileInMemory(          LPDIRECT3DSURFACE9 pDestSurface,
//		CONST PALETTEENTRY *pDestPalette,
//		CONST RECT *pDestRect,
//		LPCVOID pSrcData,
//		UINT SrcData,
//		CONST RECT *pSrcRect,
//		DWORD Filter,
//		D3DCOLOR ColorKey,
//		D3DXIMAGE_INFO *pSrcInfo
//	);

//	D3DCOLOR colkey;
//	int transparent;
//	transparent = GetIntData( srcindex );
//	if( transparent == 1 ){
//		colkey = 0xFF000000;
//	}else{
//		colkey = 0;
//	}

	HRESULT hr;
	hr = D3DXLoadSurfaceFromFileInMemory( lpsurf, NULL, NULL,
		dataptr->pdata, dataptr->leng, NULL,
		D3DX_FILTER_TRIANGLE,// | D3DX_FILTER_MIRROR,
		colkey, NULL );	
	if( hr != D3D_OK ){
		DbgOut( "texbank : LoadTextureFromBuf : D3DXLoadSurfaceFromFileInMemory error !!!\n" );
		_ASSERT( 0 );
		if( textype == TEXTYPE_TEXTURE ){
			lpsurf->Release();
		}
		return 1;
	}

	if( textype == TEXTYPE_TEXTURE ){
		lpsurf->Release();
	}

	return 0;
}


int CTexBank::CreateTexDataFromMainWindow( int datano, LPDIRECT3DDEVICE9 pdev )
{
	/***
	int ret;
	HRESULT hr;

	int pool;
	pool = GetIntData2( datano );

	D3DSURFACE_DESC desc;
	ret = GetBackSurfaceDesc( pdev, &desc );
	if( ret ){
		DbgOut( "texbank : CreateTexDataFromMainWindow : GetBackSurfaceDesc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 newsurf;
	//hr = pdev->CreateImageSurface( desc.Width, desc.Height, desc.Format, &newsurf );
	hr = pdev->CreateOffscreenPlainSurface( desc.Width, desc.Height, desc.Format, D3DPOOL_SCRATCH, &newsurf, NULL );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CreateTexDataFromMainWindow : CreateImageSurface error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*( ppsurfdata + datano ) = newsurf;

	*( pidata3 + datano ) = desc.Width;//!!!!!!!!!!!!!!
	*( pidata4 + datano ) = desc.Height;//!!!!!!!!!!!!!

	*( Type + datano ) = TEXTYPE_SURFACE;//!!!!!!!!!!!!

///////////
	LPDIRECT3DSURFACE9 pBack;
	hr = pdev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBack );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CreateTexDataFromMainWindow : GetBackBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = desc.Width;
	rect.bottom = desc.Height;

	POINT point;
	point.x = 0;
	point.y = 0;

	hr = pdev->CopyRects( pBack, &rect, 1, newsurf, &point );
	//hr = pdev->StretchRect( pBack, &rect, newsurf, &rect, D3DTEXF_NONE );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CreateTextureFromMainWindow : CopyRects error !!!\n" );
		_ASSERT( 0 );
		pBack->Release();
		return 1;
	}


//////////

	pBack->Release();

	return 0;
	***/
	return 1;
}

int CTexBank::GetBackSurfaceDesc( LPDIRECT3DDEVICE9 pdev, D3DSURFACE_DESC* pdesc )
{
	HRESULT hr;
	LPDIRECT3DSURFACE9 pBackBuffer;
	hr = pdev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	if( hr != D3D_OK ){
		DbgOut( "texbank : GetBackSurfaceDesc : GetBakcBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	hr = pBackBuffer->GetDesc( pdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : GetBackSurfaceDesc : GetDesc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	pBackBuffer->Release();

	return 0;
}


int CTexBank::SetTexData( int datano, LPDIRECT3DTEXTURE9 lptex )
{
	if( (datano < 0) || (datano >= namenum) ){
		_ASSERT( 0 );
		return 1;
	}

	*( pptexdata + datano ) = lptex;

	return 0;
}

int CTexBank::SetBufDataFromPnd( int srcindex, CPanda* panda, int pndid, char* texpath )
{
	int ret;
	PNDPROP prop;
	ZeroMemory( &prop, sizeof( PNDPROP ) );
	ret = panda->GetProperty( pndid, texpath, &prop );
	if( ret ){
		return 1;
	}

	DATAFROMBUF* setbuf;
	setbuf = ppbufdata + srcindex;
	if( !setbuf ){
		DbgOut( "texbank : SetBufData : setbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( setbuf->pdata ){
		return 0;
	}

//	if( setbuf->pdata ){
//		free( setbuf->pdata );
//		setbuf->pdata = 0;
//	}

	int findsize = prop.sourcesize;

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * (findsize + 1) );
	if( !newbuf ){
		DbgOut( "texbank : SetBufData : newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * (findsize + 1) );

	int getsize = 0;
	ret = panda->Decrypt( pndid, texpath, (unsigned char*)newbuf, findsize, &getsize );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( getsize != findsize ){
		_ASSERT( 0 );
		return 1;
	}

//	if( setbuf->pdata ){
//		free( setbuf->pdata );//!!!!!!!
//		setbuf->pdata = 0;
//	}
	setbuf->pdata = newbuf;
	setbuf->leng = findsize;

	return 0;

}

int CTexBank::SetBufData( int srcindex, char* bufptr, int bufleng )
{
	if( (srcindex < 0) || (srcindex >= namenum) ){
		_ASSERT( 0 );
		return 1;
	}

	if( !bufptr || (bufleng <= 0) ){
		DbgOut( "texbank : SetBufData : params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !ppbufdata ){
		DbgOut( "texbank : SetBufData : ppbufdata NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	DATAFROMBUF* setbuf;
	setbuf = ppbufdata + srcindex;
	if( !setbuf ){
		DbgOut( "texbank : SetBufData : setbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int createflag;
	if( setbuf->pdata && (setbuf->leng == bufleng) ){
		createflag = 0;
	}else{
		createflag = 1;
	}

	if( createflag == 1 ){
		if( setbuf->pdata ){
			free( setbuf->pdata );
			setbuf->pdata = 0;
		}

		char* newbuf;
		newbuf = (char*)malloc( sizeof( char ) * (bufleng + 1) );
		if( !newbuf ){
			DbgOut( "texbank : SetBufData : newbuf alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( newbuf, bufptr, sizeof( char ) * bufleng );
		*( newbuf + bufleng ) = 0;

		if( setbuf->pdata ){
			free( setbuf->pdata );//!!!!!!!
			setbuf->pdata = 0;
		}
		setbuf->pdata = newbuf;
		setbuf->leng = bufleng;

	}else{
		memcpy( setbuf->pdata, bufptr, sizeof( char ) * bufleng );
	}


	return 0;
}

int CTexBank::SetBufData2( int srcindex, BITMAPINFOHEADER* pbi, BYTE* pBit )
{

	if( (srcindex < 0) || (srcindex >= namenum) ){
		_ASSERT( 0 );
		return 1;
	}

	if( !pbi || !pBit ){
		DbgOut( "texbank : SetBufData2 : params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !ppbufdata ){
		DbgOut( "texbank : SetBufData2 : ppbufdata NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	DATAFROMBUF* setbuf;
	setbuf = ppbufdata + srcindex;
	if( !setbuf ){
		DbgOut( "texbank : SetBufData2 : setbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int bitcount, sizewidth, sizeheight, linewidth, sizeimage;

	bitcount = pbi->biBitCount;
	sizewidth = pbi->biWidth;
	sizeheight = pbi->biHeight;
	linewidth = (( sizewidth * bitcount + 31 ) & ~31 ) / 8;
	sizeimage = linewidth * sizeheight;

	BITMAPFILEHEADER hdr;
	ZeroMemory( &hdr, sizeof( BITMAPFILEHEADER ) );
	hdr.bfType = 0x4D42; // BM
	hdr.bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + sizeimage;
	hdr.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	int bufleng;
	bufleng = hdr.bfSize;

/***
DbgOut( "texbank : SetBufData2 : bitcount %d, sizewidth %d, sizeheight %d, linewidth %d, sizeimage %d, bisizeImage %d, bfSize %d\r\n",
		bitcount, sizewidth, sizeheight, linewidth, sizeimage, pbi->biSizeImage, hdr.bfSize );
	
DbgOut( "biSize %d, biWidth %d, biHeight %d, ibPlanes %d, biBitCount %d, biCompression %d - BIRGB %d, biSizeImage %d, biXPelsPerMeter %d, biYPelsPerMeter %d, biClrUsed %d, biClrImportant %d\r\n",
	   pbi->biSize, pbi->biWidth, pbi->biHeight, pbi->biPlanes, pbi->biBitCount, pbi->biCompression, BI_RGB,
	   pbi->biSizeImage, pbi->biXPelsPerMeter, pbi->biYPelsPerMeter, pbi->biClrUsed, pbi->biClrImportant );
***/

	/////////////

	int createflag;
	if( setbuf->pdata && (setbuf->leng == bufleng) ){
		createflag = 0;
	}else{
		createflag = 1;
	}

	if( createflag == 1 ){
		char* newbuf;
		newbuf = (char*)malloc( sizeof( char ) * (bufleng + 1) );
		if( !newbuf ){
			DbgOut( "texbank : SetBufData2 : newbuf alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int curpos = 0;
		memcpy( newbuf, &hdr, sizeof( BITMAPFILEHEADER ) );
		curpos += sizeof( BITMAPFILEHEADER );
		memcpy( newbuf + curpos, pbi, sizeof( BITMAPINFOHEADER ) );
		curpos += sizeof( BITMAPINFOHEADER );
		memcpy( newbuf + curpos, pBit, sizeimage );

		*( newbuf + bufleng ) = 0;

		if( setbuf->pdata ){
			free( setbuf->pdata );
		}
		setbuf->pdata = newbuf;
		setbuf->leng = bufleng;
	}else{

		int curpos = 0;
		memcpy( setbuf->pdata, &hdr, sizeof( BITMAPFILEHEADER ) );
		curpos += sizeof( BITMAPFILEHEADER );
		memcpy( setbuf->pdata + curpos, pbi, sizeof( BITMAPINFOHEADER ) );
		curpos += sizeof( BITMAPINFOHEADER );
		memcpy( setbuf->pdata + curpos, pBit, sizeimage );
	}


	return 0;
}



int CTexBank::AddName( char* dirname, char* srcname, int transparent, int pool )
{
	int ret;

	ret = CNameBank::AddName( srcname, transparent, pool, 1 );

	if( ret > 0 ){
		pptexdata = (LPDIRECT3DTEXTURE9*)realloc( pptexdata, sizeof( LPDIRECT3DTEXTURE9 ) * namenum );
		if( !pptexdata ){
			_ASSERT( 0 );
			return -1;
		}
		*( pptexdata + namenum - 1 ) = 0;


		ppsurfdata = (LPDIRECT3DSURFACE9*)realloc( ppsurfdata, sizeof( LPDIRECT3DSURFACE9 ) * namenum );
		if( !ppsurfdata ){
			_ASSERT( 0 );
			return -1;
		}
		*( ppsurfdata + namenum - 1 ) = 0;


		ppbufdata = (DATAFROMBUF*)realloc( ppbufdata, sizeof( DATAFROMBUF ) * namenum );
		if( !ppbufdata ){
			_ASSERT( 0 );
			return -1;
		}
		( ppbufdata + namenum - 1 )->pdata = 0;
		( ppbufdata + namenum - 1 )->leng = 0;


		ppdirname = (char**)realloc( ppdirname, sizeof( char* ) * namenum );
		if( !ppdirname ){
			_ASSERT( 0 );
			return -1;
		}
		char* newdirname;
		newdirname = (char*)malloc( sizeof( char ) * PATH_LENG );
		if( !newdirname ){
			_ASSERT( 0 );
			return -1;
		}
		ZeroMemory( newdirname, sizeof( char ) * PATH_LENG );
		if( dirname && *dirname ){
			strcpy_s( newdirname, PATH_LENG, dirname );
		}
		*( ppdirname + namenum - 1 ) = newdirname;
			
		ptexapplyindex = (int*)realloc( ptexapplyindex, sizeof( int ) * namenum );
		if( !ptexapplyindex ){
			_ASSERT( 0 );
			return -1;
		}
		*( ptexapplyindex + namenum - 1 ) = -1;

		return ret;

	}else{
		return ret;
	}
}

LPDIRECT3DTEXTURE9 CTexBank::GetTexData( char* srcname, int srctransparent )
{
	int findno;

//DbgOut( "texbank : GetTexData : %s, %d\r\n", srcname, srctransparent );


	if( !srcname )
		return NULL;


	findno = FindName( srcname, srctransparent );
	if( findno < 0 )
		return NULL;

	LPDIRECT3DTEXTURE9 rettex = 0;

	int appindex = *( ptexapplyindex + findno );
	if( appindex < 0 ){
		rettex = *( pptexdata + findno );
	}else{
		rettex = *( pptexdata + appindex );
	}
//DbgOut( "texbank : GetTexData : findno %d, tex %x\r\n", findno, rettex );

	return rettex;
}
int CTexBank::GetTexDataBySerial( int texid, LPDIRECT3DTEXTURE9* pptex )
{
	int ret;
	char* texname;
	int transparent;

	ret = GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "texbank : GetTexDataBySerial : GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !texname ){
		DbgOut( "texbank : GetTexDataBySerial : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DTEXTURE9 lptex = 0;
	*pptex = GetTexData( texname, transparent );
	if( !*pptex ){
		DbgOut( "texbank : GetTexDataBySerial : GetTexData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

LPDIRECT3DSURFACE9 CTexBank::GetSurface( int srcindex, int* textype )
{
	if( (srcindex < 0) || (srcindex >= namenum) ){
		DbgOut( "texbank : GetSurface : index error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	*textype = GetType( srcindex );

	HRESULT hr;
	LPDIRECT3DTEXTURE9 curtex;
	LPDIRECT3DSURFACE9 cursurf;

	switch( *textype ){
	case TEXTYPE_TEXTURE:
		curtex = *( pptexdata + srcindex );
		hr = curtex->GetSurfaceLevel( 0, &cursurf );
		if( hr != D3D_OK ){
			DbgOut( "texbank : GetSurface : GetSurfaceLevel error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}
		return cursurf;

		break;
	case TEXTYPE_SURFACE:

		cursurf = *( ppsurfdata + srcindex );
		return cursurf;

		break;
	default:
		return 0;
		break;
	}

}

/***
LPDIRECT3DTEXTURE9 CTexBank::GetTexDataBySerial( int srcserial )
{
	int findno = 0;

	int tno;
	for( tno = 0; tno < namenum; tno++ ){
		int tempseri;
		tempseri = GetSerial( tno );
		if(  tempseri == srcserial ){
			findno = tno;
			break;
		}
	}

	if( findno > 0 ){
		return *( pptexdata + findno );			
	}else{
		return NULL;
	}

}
***/


int CTexBank::GetTexNoByName( char* srcname, int srctrans )
{
	int retno = -1;

	retno = FindName( srcname, srctrans );

	return retno;
}

int CTexBank::RestorePoolDefault( LPDIRECT3DDEVICE9 pdev )
{
	int ret;
	int pool;
	int index;

	for( index = 0; index < namenum; index++ ){
		pool = GetIntData2( index );

		if( pool == D3DPOOL_DEFAULT ){
			int width, height;
			width = *( pidata3 + index );
			height = *( pidata4 + index );

			ret = CreateTexData( index, pdev, width, height );
			if( ret && (ret != -2) ){
				DbgOut( "texbank : RestorePoolDefault : CreateTexData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


int CTexBank::InvalidatePoolDefault()
{
	int pool;
	int index;

	for( index = 0; index < namenum; index++ ){
		pool = GetIntData2( index );

		if( pool == D3DPOOL_DEFAULT ){
			InvalidateTexture( index );
		}
	}

	return 0;
}


int CTexBank::InvalidateTextureIfNotDirty( char* srcname, int srctrans, int resetflag )
{

	int findno;

	findno = FindName( srcname, srctrans );
	if( findno < 0 )
		return 0;

	int curdirty = GetDirtyFlag( findno );
	int pool = GetIntData2( findno );

	int doflag = 1;
	if( resetflag ){
		if( pool == D3DPOOL_DEFAULT ){
			doflag = 1;
		}else{
			doflag = 0;
		}
	}else{
		doflag = 1;
	}

	if( curdirty == 0 && (doflag == 1) ){
		InvalidateTexture( findno );
	}

	return 0;
}


int CTexBank::InvalidateTexture( char* srcname, int srctrans, int resetflag )
{

	int findno;

	findno = FindName( srcname, srctrans );
	if( findno < 0 )
		return 0;

	DWORD pool = GetIntData2( findno );

	if( (resetflag == 1) && (pool == D3DPOOL_MANAGED) ){
		//何もしない
	}else{
		InvalidateTexture( findno );
	}


	return 0;
}

int CTexBank::InvalidateTexture( int delindex )
{
	if( (delindex < 0) || (delindex >= namenum) ){
		return 0;
	}


	LPDIRECT3DTEXTURE9 deltex;
	LPDIRECT3DSURFACE9 delsurf;
	if( pptexdata ){
		deltex = *( pptexdata + delindex );
		if( deltex ){
			//SAFE_RELEASE( deltex );
			deltex->Release();
			*( pptexdata + delindex ) = 0;
		}
	}

	if( ppsurfdata ){
		delsurf = *( ppsurfdata + delindex );
		if( delsurf ){
			//SAFE_RELEASE( delsurf );
			delsurf->Release();
			*( ppsurfdata + delindex ) = 0;
		}
	}


	// ここでは、ppbufdataは、リリースしない（restore出来なくなるため。）


	return 0;
}


int CTexBank::DestroyNotDirtyTexture( int saveusers )
{
	int i;
	int curdirty;
	int chkusers = 0;

	for( i = 0; i < namenum; i++ ){
		curdirty = GetDirtyFlag( i );

		if( saveusers == 1 ){
			char* curname;
			curname = (*this)( i );

			char* struserid = 0;
			struserid = strstr( curname, "_TEXID" );

			char* frombm = 0;
			frombm = strstr( curname, "FromBMSCR" );


			int cmp0;
			cmp0 = strncmp( curname, "MainWindow_", 11 );

			int cmprt;
			cmprt = strncmp( curname, "RenderTarget_", 13 );

			if( (struserid && !frombm ) || (cmp0 == 0) || (cmprt == 0) ){
				chkusers = 0;
			}else{
				chkusers = 1;
			}

		}else{
			chkusers = 1;
		}


//DbgOut( "texbank : DestroyNotDirty : id %d, curdirty %d, chkusers %d\r\n", i, curdirty, chkusers );

		if( (curdirty == 0) && (chkusers) ){

			int dstcmp = 0;
			dstcmp = strncmp( (*this)( i ), "__Destroyed_", 12 );
			if( dstcmp != 0 ){

				DbgOut( "texbank : DestroyNotDirty : DestroyTexture %s\r\n", (*this)( i ) );

				DestroyTexture( i );

				DbgOut( "texbank : DestroyNotDirty : DestroyTexture %d\r\n", i );
			}

		}
	}

	return 0;
}

int CTexBank::DestroyTexture( int destroyindex, int renameflag )
{

	InvalidateTexture( destroyindex );


// ppbufdataのメンバは、invalidateではなくて、ここで、リリースする。

	DATAFROMBUF* delbufdata;
	delbufdata = ppbufdata + destroyindex;
	if( delbufdata && delbufdata->pdata ){
		free( delbufdata->pdata );
		( ppbufdata + destroyindex )->pdata = 0;
		( ppbufdata + destroyindex )->leng = 0;
	}

/////////

	*( ptexapplyindex + destroyindex ) = -1;

///////////
	if( renameflag == 1 ){
		int dstcmp;
		dstcmp = strncmp( (*this)( destroyindex ), "__Destroyed_", 12 );
		if( dstcmp != 0 ){
			s_destroycount++;
			char destroyname[256];
			sprintf_s( destroyname, 256, "__Destroyed_%d", s_destroycount );

			Rename( destroyindex, destroyname );
		}
	}
	return 0;
}

/***
int CTexBank::CopySurface2BackBuffer( RECT cprect, D3DLOCKED_RECT srcrect, D3DFORMAT srcformat, D3DLOCKED_RECT dstrect, D3DFORMAT dstformat )
{

	if( srcformat != D3DFMT_A8R8G8B8 ){
		DbgOut( "texbank : CopySurface2Surface : srcformat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !srcrect.pBits || !dstrect.pBits ){
		DbgOut( "texbank : CopySurface2Surface : pBits NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int dstelemsize;
	switch( dstformat ){
		case D3DFMT_X1R5G5B5:
		case D3DFMT_R5G6B5:
		case D3DFMT_A1R5G5B5:
			dstelemsize = 2;
			break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
			dstelemsize = 4;
			break;
		default:
			DbgOut( "texbank : CopySurface2Surface : dstformat error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
	}

	int srcelemsize;
	switch( srcformat ){
		case D3DFMT_X1R5G5B5:
		case D3DFMT_R5G6B5:
		case D3DFMT_A1R5G5B5:
			srcelemsize = 2;
			_ASSERT( 0 );//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
			srcelemsize = 4;
			break;
		default:
			DbgOut( "texbank : CopySurface2Surface : srcformat error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
	}

	if( ((cprect.right * srcelemsize) > srcrect.Pitch) || ((cprect.right * dstelemsize) > dstrect.Pitch) ){
		DbgOut( "texbank : CopySurface2Surface : cprect error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////////////
	int hno, wno;
	BYTE* srchead8;
	BYTE* dsthead8;

	//WORD* srchead16;
	//DWORD* dsthead32;

	DWORD* srchead32;
	WORD* dsthead16;


	if( srcformat == dstformat ){
		for( hno = 0; hno < cprect.bottom; hno++ ){
			srchead8 = (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno;
			dsthead8 = (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno;

			memcpy( dsthead8, srchead8, dstelemsize * cprect.right );
		}

	}else{

		switch( dstformat ){
			case D3DFMT_R5G6B5:
				for( hno = 0; hno < cprect.bottom; hno++ ){
					srchead32 = (DWORD*)( (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno );
					dsthead16 = (WORD*)( (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno );

					for( wno = 0; wno < cprect.right; wno++ ){
						ConvA8R8G8B8toR5G6B5( srchead32, dsthead16 );

						srchead32++;
						dsthead16++;
					}
					
				}	
				break;
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
				for( hno = 0; hno < cprect.bottom; hno++ ){
					srchead32 = (DWORD*)( (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno );
					dsthead16 = (WORD*)( (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno );

					for( wno = 0; wno < cprect.right; wno++ ){
						ConvA8R8G8B8to1R5G5B5( srchead32, dsthead16 );

						srchead32++;
						dsthead16++;
					}
					
				}	
				break;
			case D3DFMT_X8R8G8B8:
			case D3DFMT_A8R8G8B8:
				for( hno = 0; hno < cprect.bottom; hno++ ){
					srchead8 = (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno;
					dsthead8 = (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno;

					memcpy( dsthead8, srchead8, dstelemsize * cprect.right );
				}
				break;
			default:
				DbgOut( "texbank : CopySurface2Surface : dstformat error !!!\n" );
				_ASSERT( 0 );
				return 1;
				break;
		}
				
	}

	return 0;
}
***/

/***
int CTexBank::CopySurface2Surface( RECT cprect, D3DLOCKED_RECT srcrect, D3DFORMAT srcformat, D3DLOCKED_RECT dstrect, D3DFORMAT dstformat )
{

	// とりあえず、destのフォーマットは固定。
	if( dstformat != D3DFMT_A8R8G8B8 ){
		DbgOut( "texbank : CopySurface2Surface : dstformat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !srcrect.pBits || !dstrect.pBits ){
		DbgOut( "texbank : CopySurface2Surface : pBits NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int dstelemsize;
	switch( dstformat ){
		case D3DFMT_X1R5G5B5:
		case D3DFMT_R5G6B5:
		case D3DFMT_A1R5G5B5:
			dstelemsize = 2;
			_ASSERT( 0 );//!!!!!!!!!!!!dest固定のため、エラーのはず。
			return 1;//!!!!!!!!!!!!!!!!!!
			break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
			dstelemsize = 4;
			break;
		default:
			DbgOut( "texbank : CopySurface2Surface : dstformat error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
	}

	int srcelemsize;
	switch( srcformat ){
		case D3DFMT_X1R5G5B5:
		case D3DFMT_R5G6B5:
		case D3DFMT_A1R5G5B5:
			srcelemsize = 2;
			break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
			srcelemsize = 4;
			break;
		default:
			DbgOut( "texbank : CopySurface2Surface : srcformat error !!!\n" );
			_ASSERT( 0 );
			return 1;
			break;
	}


	if( ((cprect.right * srcelemsize) > srcrect.Pitch) || ((cprect.right * dstelemsize) > dstrect.Pitch) ){
		DbgOut( "texbank : CopySurface2Surface : cprect error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int hno, wno;
	BYTE* srchead8;
	BYTE* dsthead8;

	WORD* srchead16;
	DWORD* dsthead32;

	if( srcformat == dstformat ){
		for( hno = 0; hno < cprect.bottom; hno++ ){
			srchead8 = (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno;
			dsthead8 = (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno;

			memcpy( dsthead8, srchead8, dstelemsize * cprect.right );
		}

	}else{

		switch( srcformat ){
			case D3DFMT_R5G6B5:
				for( hno = 0; hno < cprect.bottom; hno++ ){
					srchead16 = (WORD*)( (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno );
					dsthead32 = (DWORD*)( (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno );

					for( wno = 0; wno < cprect.right; wno++ ){
						ConvR5G6B5toA8R8G8B8( srchead16, dsthead32 );

						srchead16++;
						dsthead32++;
					}
					
				}	
				break;
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
				for( hno = 0; hno < cprect.bottom; hno++ ){
					srchead16 = (WORD*)( (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno );
					dsthead32 = (DWORD*)( (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno );

					for( wno = 0; wno < cprect.right; wno++ ){
						Conv1R5G5B5toA8R8G8B8( srchead16, dsthead32 );

						srchead16++;
						dsthead32++;
					}
					
				}	
				break;
			case D3DFMT_X8R8G8B8:
			case D3DFMT_A8R8G8B8:
				for( hno = 0; hno < cprect.bottom; hno++ ){
					srchead8 = (BYTE*)(srcrect.pBits) + srcrect.Pitch * hno;
					dsthead8 = (BYTE*)(dstrect.pBits) + dstrect.Pitch * hno;

					memcpy( dsthead8, srchead8, dstelemsize * cprect.right );
				}
				break;
			default:
				DbgOut( "texbank : CopySurface2Surface : srcformat error !!!\n" );
				_ASSERT( 0 );
				return 1;
				break;
		}
				
	}

	return 0;
}
***/


int CTexBank::GetTextureInfoBySerial( int texid, int* widthptr, int* heightptr, int* poolptr, int* transparentptr, int* formatptr )
{

	int ret;

	char* texname;
	ret = GetNameBySerial( texid, &texname, transparentptr );//!!!!!!!
	if( ret ){
		DbgOut( "texbank : GetTextureInfoBySerial : GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


/////////
	D3DSURFACE_DESC desc;
	HRESULT hr;

	int curindex;
	curindex = GetIndexBySerial( texid );
	if( curindex < 0 ){
		DbgOut( "texbank : GetTextureInfoBySerial : GetIndexBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int curtype;
	curtype = GetType( curindex );

	switch( curtype ){
	case TEXTYPE_TEXTURE:
		LPDIRECT3DTEXTURE9 lptex;
		ret = GetTexDataBySerial( texid, &lptex );
		if( ret ){
			DbgOut( "texbank : GetTextureInfoBySerial : GetTexDataBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hr = lptex->GetLevelDesc( 0, &desc );
		if( hr != D3D_OK ){
			DbgOut( "texbank : GetTextureInfoBySerial : GetDesc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case TEXTYPE_SURFACE:
		
		int temptype;
		LPDIRECT3DSURFACE9 lpsurf;
		lpsurf = GetSurface( curindex, &temptype );

		if( !lpsurf ){
			DbgOut( "texbank : GetTextureInfoBySerial : surface NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hr = lpsurf->GetDesc( &desc );
		if( hr != D3D_OK ){
			DbgOut( "texbank : GetTextureInfoBySerial : GetDesc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		break;
	default:
		DbgOut( "texbank : GetTextureInfoBySerial : curtype error !!!\n" );
		_ASSERT( 0 );
		return 1;
		break;
	}


	*widthptr = desc.Width;
	*heightptr = desc.Height;
	*poolptr = desc.Pool;
	*formatptr = desc.Format;

	return 0;
}


int CTexBank::CopyTexture2Texture( int srctexid, int dsttexid )
{
	/***
	HRESULT hr;

	if( srctexid == dsttexid )
		return 0;

	int srcindex, dstindex;
	int srctype, dsttype;
	LPDIRECT3DSURFACE9 srcsurf;
	LPDIRECT3DSURFACE9 dstsurf;

	srcindex = GetIndexBySerial( srctexid );
	if( srcindex < 0 ){
		DbgOut( "texbank : CopyTexture2Texture : srctexid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	srcsurf = GetSurface( srcindex, &srctype );
	if( !srcsurf ){
		DbgOut( "texbank : CopyTexture2Texture : srcsurf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DSURFACE_DESC srcdesc;
	hr = srcsurf->GetDesc( &srcdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CopyTexture2Texture : GetDesc src error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}


	dstindex = GetIndexBySerial( dsttexid );
	if( dstindex < 0 ){
		DbgOut( "texbank : CopyTexture2Texture : dsttexid error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}
	dstsurf = GetSurface( dstindex, &dsttype );
	if( !srcsurf ){
		DbgOut( "texbank : CopyTexture2Texture : dstsurf NULL error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}
	D3DSURFACE_DESC dstdesc;
	hr = dstsurf->GetDesc( &dstdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CopyTexture2Texture : GetDesc dst error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}


	if( (srcdesc.Width != dstdesc.Width) || (srcdesc.Height != dstdesc.Height) ){
		DbgOut( "texbank : CopyTexture2Texture : srcsize not equal dstsize error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}

	if( srcdesc.Format != dstdesc.Format ){
		DbgOut( "texbank : CopyTexture2Texture : format not equal error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	hr = srcsurf->GetDevice( &pdev );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CopyTexture2Texture : GetDevice error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}

////////
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = srcdesc.Width;
	rect.bottom = srcdesc.Height;

	POINT point;
	point.x = 0;
	point.y = 0;

	hr = pdev->CopyRects( srcsurf, &rect, 1, dstsurf, &point );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CopyTexture2Texture : CopyRects error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		pdev->Release();
		return 1;
	}

	if( srctype == TEXTYPE_TEXTURE )
		srcsurf->Release();
	if( dsttype == TEXTYPE_TEXTURE )
		dstsurf->Release();
	pdev->Release();

	return 0;
	***/

	return 1;
}

int CTexBank::CopyTextureToBackBuffer( LPDIRECT3DDEVICE9 pdev, int texid )
{
	/***
	int ret;
	HRESULT hr;

	int srcindex, srctype;
	LPDIRECT3DSURFACE9 srcsurf;
	srcindex = GetIndexBySerial( texid );
	if( srcindex < 0 ){
		DbgOut( "texbank : CopyTextureToBackBuffer : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	srcsurf = GetSurface( srcindex, &srctype );
	if( !srcsurf ){
		DbgOut( "texbank : CopyTextureToBackBuffer : srcsurf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DSURFACE_DESC srcdesc;
	hr = srcsurf->GetDesc( &srcdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CopyTextureToBackBuffer : GetDesc src error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}



	LPDIRECT3DSURFACE9 pBack;
	hr = pdev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBack );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CopyTextureToBackBuffer : GetBackBuffer error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}

	D3DSURFACE_DESC backdesc;
	ret = GetBackSurfaceDesc( pdev, &backdesc );
	if( ret ){
		DbgOut( "texbank : CopyTextureToBackBuffer : GetBackSurfaceDesc error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		pBack->Release();
		return 1;
	}


	if( srcdesc.Format != backdesc.Format ){
		DbgOut( "texbank : CopyTextureToBackBuffer : Format not equal error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		pBack->Release();
		return 1;
	}


////////////////////	

	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = backdesc.Width;
	rect.bottom = backdesc.Height;

	POINT point;
	point.x = 0;
	point.y = 0;


	hr = pdev->CopyRects( srcsurf, &rect, 1, pBack, &point );
	if( hr != D3D_OK ){
		DbgOut( "texbank : CopyTextureToBackBuffer : CopyRects error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		pBack->Release();
		return 1;
	}
	

///////////
	if( srctype == TEXTYPE_TEXTURE )
		srcsurf->Release();
	pBack->Release();


	return 0;
	***/
	return 1;
}

int CTexBank::RasterScrollTexture( LPDIRECT3DDEVICE9 pdev, int srctexid, int dsttexid, int t, float fparam1, float fparam2 )
{
	/***
	HRESULT hr;

	if( srctexid == dsttexid )
		return 0;


	int srcindex, dstindex;
	int srctype, dsttype;
	LPDIRECT3DSURFACE9 srcsurf;
	LPDIRECT3DSURFACE9 dstsurf;

	srcindex = GetIndexBySerial( srctexid );
	if( srcindex < 0 ){
		DbgOut( "texbank : RasterScrollTexture : srctexid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	srcsurf = GetSurface( srcindex, &srctype );
	if( !srcsurf ){
		DbgOut( "texbank : RasterScrollTexture : srcsurf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DSURFACE_DESC srcdesc;
	hr = srcsurf->GetDesc( &srcdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : RasterScrollTexture : GetDesc src error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}


	dstindex = GetIndexBySerial( dsttexid );
	if( dstindex < 0 ){
		DbgOut( "texbank : RasterScrollTexture : dsttexid error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}
	dstsurf = GetSurface( dstindex, &dsttype );
	if( !srcsurf ){
		DbgOut( "texbank : RasterScrollTexture : dstsurf NULL error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		return 1;
	}
	D3DSURFACE_DESC dstdesc;
	hr = dstsurf->GetDesc( &dstdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : RasterScrollTexture : GetDesc dst error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}

/////////

	if( (srcdesc.Width != dstdesc.Width) || (srcdesc.Height != dstdesc.Height) ){
		DbgOut( "texbank : RasterScrollTexture : srcsize not equal dstsize error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}

	if( srcdesc.Format != dstdesc.Format ){
		DbgOut( "texbank : RasterScrollTexture : format not equal error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}
	

/////////
	int elemsize;
	switch( srcdesc.Format ){
		case D3DFMT_X1R5G5B5:
		case D3DFMT_R5G6B5:
		case D3DFMT_A1R5G5B5:
			elemsize = 2;
			break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
			elemsize = 4;
			break;
		default:
			DbgOut( "texbank : RasterScrollTexture : format error !!!\n" );
			_ASSERT( 0 );
			if( srctype == TEXTYPE_TEXTURE )
				srcsurf->Release();
			if( dsttype == TEXTYPE_TEXTURE )
				dstsurf->Release();
			return 1;
			break;
	}


	int Width, Height;
	Width = GetIntData3( srcindex );//!!!!!!!!!!!!! textureの大きさではなく、backbufferの大きさ
	Height = GetIntData4( srcindex );//!!!!!!!!!!!!


	int rectcnt = 0;
	int rectarraymax = Height * 2;
	RECT* rectarray;
	rectarray = (RECT*)malloc( sizeof( RECT ) * rectarraymax );
	if( !rectarray ){
		DbgOut( "texbank : RasterScrollTexture : rectarray alloc error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}
	POINT* pointarray;
	pointarray = (POINT*)malloc( sizeof( POINT ) * rectarraymax );
	if( !pointarray ){
		DbgOut( "texbank : RasterScrollTexture : pointarray alloc error !!!\n" );
		_ASSERT( 0 );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}





	int hno;
	int dx;
	for( hno = 0; hno < Height; hno++ ){
		dx = (int)(sinf(( fparam1 * hno + t) * 0.05f) * fparam2);
		if( (dx > Width) || (dx < -Width ) ){
			int show;
			show = dx / Width;
			dx = dx - show * Width;
		}

		if( dx > 0 ){
			(rectarray + rectcnt)->left = Width - dx;
			(rectarray + rectcnt)->right = Width;
			(rectarray + rectcnt)->top = hno;
			(rectarray + rectcnt)->bottom = hno + 1;

			(pointarray + rectcnt)->x = 0;
			(pointarray + rectcnt)->y = hno;
			rectcnt++;

			
			(rectarray + rectcnt)->left = 0;
			(rectarray + rectcnt)->right = Width - dx;
			(rectarray + rectcnt)->top = hno;
			(rectarray + rectcnt)->bottom = hno + 1;

			(pointarray + rectcnt)->x = dx;
			(pointarray + rectcnt)->y = hno;
			rectcnt++;
		}else if( dx < 0 ){
			(rectarray + rectcnt)->left = -dx;
			(rectarray + rectcnt)->right = Width;
			(rectarray + rectcnt)->top = hno;
			(rectarray + rectcnt)->bottom = hno + 1;

			(pointarray + rectcnt)->x = 0;
			(pointarray + rectcnt)->y = hno;
			rectcnt++;


			(rectarray + rectcnt)->left = 0;
			(rectarray + rectcnt)->right = -dx;
			(rectarray + rectcnt)->top = hno;
			(rectarray + rectcnt)->bottom = hno + 1;
			(pointarray + rectcnt)->x = Width + dx;
			(pointarray + rectcnt)->y = hno;
			rectcnt++;
		}else if( dx == 0 ){
			(rectarray + rectcnt)->left = 0;
			(rectarray + rectcnt)->right = Width;
			(rectarray + rectcnt)->top = hno;
			(rectarray + rectcnt)->bottom = hno + 1;

			(pointarray + rectcnt)->x = 0;
			(pointarray + rectcnt)->y = hno;
			rectcnt++;
		}
	}

	hr = pdev->CopyRects( srcsurf, rectarray, rectcnt, dstsurf, pointarray );
	if( hr != D3D_OK ){
		DbgOut( "texbank : RasterScrollTexture : CopyRects 6 error !!!\n" );
		_ASSERT( 0 );
		free( rectarray );
		free( pointarray );
		if( srctype == TEXTYPE_TEXTURE )
			srcsurf->Release();
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}


	free( rectarray );
	free( pointarray );

	if( srctype == TEXTYPE_TEXTURE )
		srcsurf->Release();
	if( dsttype == TEXTYPE_TEXTURE )
		dstsurf->Release();

	return 0;
	***/
	return 1;
}


int CTexBank::InvColTexture( LPDIRECT3DDEVICE9 pdev, int texid )
{
	/***
	HRESULT hr;
	int dstindex;
	int dsttype;
	LPDIRECT3DSURFACE9 dstsurf;

	dstindex = GetIndexBySerial( texid );
	if( dstindex < 0 ){
		DbgOut( "texbank : InvColTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	dstsurf = GetSurface( dstindex, &dsttype );
	if( !dstsurf ){
		DbgOut( "texbank : InvColTexture : dstsurf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DSURFACE_DESC dstdesc;
	hr = dstsurf->GetDesc( &dstdesc );
	if( hr != D3D_OK ){
		DbgOut( "texbank : InvColTexture : GetDesc dst error !!!\n" );
		_ASSERT( 0 );
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}


	int Width, Height;
	Width = GetIntData3( dstindex );//!!!!!!!!!!!!! textureの大きさではなく、backbufferの大きさ
	Height = GetIntData4( dstindex );//!!!!!!!!!!!!

/////////	

	LPDIRECT3DSURFACE9 tempsurf;
	hr = pdev->CreateImageSurface( dstdesc.Width, dstdesc.Height, dstdesc.Format, &tempsurf );
	if( hr != D3D_OK ){
		DbgOut( "texbank : InvColTexture : CreateImageSurface error !!!\n" );
		_ASSERT( 0 );
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		return 1;
	}
	
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = dstdesc.Width;
	rect.bottom = dstdesc.Height;

	POINT point;
	point.x = 0;
	point.y = 0;

	hr = pdev->CopyRects( dstsurf, &rect, 1, tempsurf, &point );
	if( hr != D3D_OK ){
		DbgOut( "texbank : InvColTexture : CopyRects error !!!\n" );
		_ASSERT( 0 );
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		tempsurf->Release();
		return 1;
	}
		
	D3DLOCKED_RECT lockedrect;
	hr = tempsurf->LockRect( &lockedrect, NULL, 0 );
	if( hr != D3D_OK ){
		DbgOut( "texbank : InvColTexture : LockRect error !!!\n" );
		_ASSERT( 0 );
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		tempsurf->Release();
		return 1;
	}

//////////////////
	DWORD* temphead32;
	WORD* temphead16;
	int hno, wno;

	DWORD alpha32;
	DWORD col32, newcol32;

	WORD alpha16;
	WORD col16, newcol16;

	WORD tempcol16;

	switch( dstdesc.Format ){
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		for( hno = 0; hno < Height; hno++ ){
			temphead32 = (DWORD*)( (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno );

			for( wno = 0; wno < Width; wno++ ){
				alpha32 = *temphead32 & 0xFF000000;
				col32 = *temphead32 & 0x00FFFFFF;
				newcol32 = ~col32;

				*temphead32 = alpha32 | newcol32;
				
				temphead32++;
			}				
		}
		break;

	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		for( hno = 0; hno < Height; hno++ ){
			temphead16 = (WORD*)( (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno );

			for( wno = 0; wno < Width; wno++ ){
				alpha16 = *temphead16 & 0x8000;
				col16 = *temphead16 & 0x7FFF;
				newcol16 = ~col16;

				*temphead16 = alpha16 | newcol16;

				temphead16++;

			}
		}
		break;

	case D3DFMT_R5G6B5:
		for( hno = 0; hno < Height; hno++ ){
			temphead16 = (WORD*)( (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno );

			for( wno = 0; wno < Width; wno++ ){
				tempcol16 = *temphead16;

				*temphead16 = ~tempcol16;

				temphead16++;

			}
		}
		break;
	default:
		DbgOut( "texbank : InvColTexture : format error !!!\n" );
		_ASSERT( 0 );
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		tempsurf->Release();
		return 1;
		break;
	}
///////////////////
	tempsurf->UnlockRect();

	hr = pdev->CopyRects( tempsurf, &rect, 1, dstsurf, &point );
	if( hr != D3D_OK ){
		DbgOut( "texbank : InvColTexture : CopyRects 2 error !!!\n" );
		_ASSERT( 0 );
		if( dsttype == TEXTYPE_TEXTURE )
			dstsurf->Release();
		tempsurf->Release();
		return 1;
	}


	if( dsttype == TEXTYPE_TEXTURE )
		dstsurf->Release();
	tempsurf->Release();

	return 0;
	***/
	return 1;
}

int CTexBank::GetTexSize( int srcindex, int* dstx, int* dsty )
{
	if( (srcindex < 0) || (srcindex >= namenum) ){
		DbgOut( "texbank : GetTexSize : srcindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstx = *( pidata3 + srcindex );
	*dsty = *( pidata4 + srcindex );

	return 0;
}

int CTexBank::GetTexOrgSize( int srcindex, int* dstx, int* dsty )
{
	if( (srcindex < 0) || (srcindex >= namenum) ){
		DbgOut( "texbank : GetTexSize : srcindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstx = (int)(( porgsize + srcindex )->cx);
	*dsty = (int)(( porgsize + srcindex )->cy);

	return 0;
}

int CTexBank::SetDirtyFlag( char* srcname, int srcint )
{

	int findno;

	findno = FindName( srcname, srcint );
	if( findno >= 0 ){
		*( dirtyflag + findno ) = 1;

DbgOut( "texbank : SetDirty %s\r\n", srcname );
		int appindex = *( ptexapplyindex + findno );
		if( appindex >= 0 ){
			int dirtyindex;
			dirtyindex = *( ptexapplyindex + findno );
			if( dirtyindex >= 0 ){
				*( dirtyflag + dirtyindex ) = 1;
			}
		}
	}
DbgOut( "texbank : SetDirty exit\r\n" );

	return 0;
}

int CTexBank::SetAppTexture( char* dstname, int dsttransparent, int srctexid )
{
DbgOut( "texbank : SetAppTexture start\r\n" );
	int dstindex;
	int srcindex;
	dstindex = FindName( dstname, dsttransparent );
	if( dstindex < 0 ){
		_ASSERT( 0 );
		return 1;
	}
	srcindex = GetIndexBySerial( srctexid );
	if( srcindex < 0 ){
		_ASSERT( 0 );
		return 1;
	}
DbgOut( "texbank : SetAppTexture 0\r\n" );
	int ret;
	ret = DestroyTexture( dstindex, 0 );
	_ASSERT( !ret );
DbgOut( "texbank : SetAppTexture 1\r\n" );
	*( ptexapplyindex + dstindex ) = srcindex;
DbgOut( "texbank : SetAppTexture exit\r\n" );
	return 0;
}
