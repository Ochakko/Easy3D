//-----------------------------------------------------------------------------
//
// (c) 2002-2003 おちっこ　http://www5d.biglobe.ne.jp/~ochikko
//
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include <windows.h>

#include <stdlib.h>
#include <crtdbg.h>

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <D3DX9.h>
#include "D3DUtil.h"
#include "DXUtil.h"

#include <coef.h>
#define DBGH
#include <dbg.h>

#include <dxerr.h>

#include <texbank.h>

#include "mysprite.h"
#include <Mmsystem.h>


// extern

extern CTexBank* g_texbnk;// shdhandler.cpp
extern DWORD g_renderstate[ D3DRS_BLENDOP + 1 ];
extern int g_rsindex[ 80 ];
extern DWORD g_minfilter;
extern DWORD g_magfilter;

extern DWORD g_cop0;
extern DWORD g_cop1;
extern DWORD g_aop0;
extern DWORD g_aop1;

//// static /////
static int s_allocno = 0;
/////////////////



CMySprite::CMySprite( int* idptr )
{
	ZeroMemory( spritename, 1024 );
	pd3dtex = 0;
	
	transparentflag = 0;
	texbanktransparent = 0;
	
	texWidth = 0;
	texHeight = 0;

	next = 0;

	tpr = 0;
	tpg = 0;
	tpb = 0;

	rotcen.x = 0.0f;
	rotcen.y = 0.0f;
	rotdeg = 0.0f;
	multcol = 0xFFFFFFFF;

	m_miplevels = 0;
	m_mipfilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR;

	serialno = s_allocno;
	*idptr = s_allocno;

	SetDefaultRenderState();
	
	s_allocno++;

	m_blendmode = 0;

	m_pbi = 0;
	m_pBit = 0;

	startu = 0.0f;
	endu = 1.0f;
	startv = 0.0f;
	endv = 1.0f;
}
CMySprite::~CMySprite()
{
	//InvalidateTexture();
	int resetflag = 0;
	Invalidate( resetflag );

}

int CMySprite::SetSpriteParams( char* fname, int srctpflag, int srctpr, int srctpg, int srctpb, UINT miplevels, DWORD mipfilter )
{
	if( !fname ){
		_ASSERT( 0 );
		return 1;
	}

	char* enmark;
	enmark = strchr( fname, '\\' );
	if( !enmark ){

		char resdir[1024];
		int resdirleng;
		
		ZeroMemory( resdir, 1024 );
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)resdir, 1024 );
		if( (resdirleng == 0) || (resdirleng >= 1024) ){
			_ASSERT( 0 );
			return 1;
		}

		if( (resdirleng + strlen( fname )) >= 1024 ){
			_ASSERT( 0 );
			return 1;
		}

		ZeroMemory( spritename, 1024 );
		strcpy( spritename, resdir );
		strcat( spritename, fname );
	
	}else{
		int nameleng;
		nameleng = (int)strlen( fname );
		if( nameleng >= 1024 ){
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( spritename, 1024 );
		strcpy( spritename, fname );
	}

	transparentflag = srctpflag;
	switch( transparentflag ){
	case 0:
		texbanktransparent = 2;
		break;
	case 1:
		texbanktransparent = 1;
		break;
	case 2:
		texbanktransparent = 1;
		break;
	default:
		texbanktransparent = 2;
		break;
	}

	tpr = srctpr;
	tpr = max( 0, tpr );
	tpr = min( 255, tpr );

	tpg = srctpg;
	tpg = max( 0, tpg );
	tpg = min( 255, tpg );

	tpb = srctpb;
	tpb = max( 0, tpb );
	tpb = min( 255, tpb );

	m_miplevels = miplevels;
	m_mipfilter = mipfilter;

	return 0;
}

int CMySprite::SetSpriteParams2( BITMAPINFOHEADER* pbi, BYTE* pBit, int srctpflag, int srctpr, int srctpg, int srctpb, UINT miplevels, DWORD mipfilter )
{
	sprintf( spritename, "FromBMSCR_TEXID%d", serialno );

	transparentflag = srctpflag;
	switch( transparentflag ){
	case 0:
		texbanktransparent = 2;
		break;
	case 1:
		texbanktransparent = 1;
		break;
	case 2:
		texbanktransparent = 1;
		break;
	default:
		texbanktransparent = 2;
		break;
	}

	tpr = srctpr;
	tpr = max( 0, tpr );
	tpr = min( 255, tpr );

	tpg = srctpg;
	tpg = max( 0, tpg );
	tpg = min( 255, tpg );

	tpb = srctpb;
	tpb = max( 0, tpb );
	tpb = min( 255, tpb );

	m_miplevels = miplevels;
	m_mipfilter = mipfilter;

	m_pbi = pbi;
	m_pBit = pBit;

	return 0;

}


int CMySprite::RestoreTexture( LPDIRECT3DDEVICE9 pd3dDevice )
{
    //if( FAILED( D3DUtil_CreateTexture( pd3dDevice, (LPTSTR)spritename,
	//	&pd3dtex, D3DFMT_A8R8G8B8 ) ) ){
	//	_ASSERT( 0 );
    //  return	1;
	//}
	

/***
	HRESULT hr;
	hr = D3DXCreateTextureFromFileEx( pd3dDevice, (LPTSTR)spritename, 
						D3DX_DEFAULT, D3DX_DEFAULT, 
						m_miplevels,//!!!!!!! 
						0, D3DFMT_A8R8G8B8, 
						D3DPOOL_MANAGED, 
						D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
						m_mipfilter,//!!!!!!! 
						0, NULL, NULL, &pd3dtex );
	if( hr != D3D_OK ){
		DbgOut( "mysprite : RestoreTexture : D3DXCreateTextureFromFileEx error %x!!!\n", hr );
		_ASSERT( 0 );
		return 1;
	}
***/
	
	if( pd3dtex ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}


	int ret;
	if( !g_texbnk ){
		g_texbnk = new CTexBank( m_miplevels, m_mipfilter );
		if( !g_texbnk ){
			DbgOut( "mysprite : RestoreTexture : new CTexBank error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int findno;

	int addflag = 0;


	if( spritename[0] != 0 ){
		addflag = g_texbnk->AddName( 0, spritename, texbanktransparent, D3DPOOL_MANAGED );
		//ret = g_texbnk->AddName( spritename, texbanktransparent, D3DPOOL_DEFAULT );
		if( addflag < 0 ){
			//エラー
			_ASSERT( 0 );
			return 1;
		}
		
//		if( addflag != 0 ){//0のときはすでにテクスチャがある

			//追加する
			findno = g_texbnk->GetTexNoByName( spritename, texbanktransparent );
			if( (findno < 0) || (findno >= g_texbnk->namenum) ){
				DbgOut( "mysprite : RestoreTexture : GetTexNoByName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( m_pbi && m_pBit ){
				ret = g_texbnk->SetBufData2( findno, m_pbi, m_pBit );
				if( ret ){
					DbgOut( "mysprite : RestoreTexture ; SetBufData2 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				m_pbi = 0;//!!!!!!!!!!!!
				m_pBit = 0;//!!!!!!!!!!!!
			}


			ret = g_texbnk->CreateTexData( findno, pd3dDevice );
			if( ret && (ret != -2) ){
				DbgOut( "mysprite : RestoreTexture : CreateTexData : media not found %s error !!!\n",
					(*g_texbnk)( findno ) );

				int ret2;
				ret2 = g_texbnk->SetTexData( findno, NULL );
				if( ret2 ){
					_ASSERT( 0 );
					return 1;
				}
			}
//		}
			
		pd3dtex = g_texbnk->GetTexData( spritename, texbanktransparent );


		//if( pd3dtex && (transparentflag == 1) ){
		//	ret = SetTextureAlpha();
		//	if( ret ){
		//		DbgOut( "mysprite : RestoreTexture : SetTextureAlpha error !!!\n" );
		//		_ASSERT( 0 );
		//		return 1;
		//	}
		//}

		DbgOut( "mysprite : RestoreTexture : %s\n", spritename );

	}

////////////
	if( pd3dtex ){

		HRESULT hr;
		D3DSURFACE_DESC	sdesc;
		hr = pd3dtex->GetLevelDesc( 0, &sdesc );
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		texWidth = sdesc.Width;
		texHeight = sdesc.Height;

	//	if( addflag != 0 ){
			if( transparentflag && pd3dtex ){
				ret = SetTextureAlpha();
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
	//	}

	}

	return 0;
}

int CMySprite::LoadSpriteFromBuf()
{
	int ret;
	int findno;
	findno = g_texbnk->GetTexNoByName( spritename, texbanktransparent );
	if( (findno < 0) || (findno >= g_texbnk->namenum) ){
		DbgOut( "mysprite : LoadSpriteFromBuf : GetTexNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_pbi && m_pBit ){
		ret = g_texbnk->SetBufData2( findno, m_pbi, m_pBit );
		if( ret ){
			DbgOut( "mysprite : LoadSpriteFromBuf ; SetBufData2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_pbi = 0;//!!!!!!!!!!!!
		m_pBit = 0;//!!!!!!!!!!!!
	}

	D3DCOLOR colkey;
	if( texbanktransparent == 1 ){
		colkey = D3DCOLOR_ARGB( 255, tpr, tpg, tpb );
	}else{
		colkey = 0;
	}

	ret = g_texbnk->LoadTextureFromBuf( findno, colkey );
	if( ret ){
		DbgOut( "mysprite : LoadSpriteFromBuf : texbank LoadTextureFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}




int CMySprite::SetTextureAlpha()
{
	if( !pd3dtex ){
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	D3DSURFACE_DESC	sdesc;
	hr = pd3dtex->GetLevelDesc( 0, &sdesc );
	if( hr != D3D_OK ){
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
		DbgOut( "MySprite : SetTextureAlpha : d3dformat %d is not supported error !!!\n",
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
				if( (colr == (unsigned char)tpr) && (colg == (unsigned char)tpg) && (colb == (unsigned char)tpb) ){
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
				if( (colr == (unsigned char)(tpr * 31 / 255)) && (colg == (unsigned char)(tpg * 31 / 255)) && (colb == (unsigned char)(tpb * 31 / 255)) ){
					*wbits = 0;
				}else{
					*wbits = col | 0x8000;
				}
			}

		}
	
	}

	hr = pd3dtex->UnlockRect( 0 );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

/***
int CMySprite::InvalidateTexture()
{
	if( pd3dtex ){
		pd3dtex->Release();
		pd3dtex = 0;
	}
	return 0;
}
***/
int CMySprite::Invalidate( int resetflag )
{
	int ret;

	ret = g_texbnk->InvalidateTextureIfNotDirty( spritename, texbanktransparent, resetflag );
	if( ret ){
		DbgOut( "mysprite : texbnk InvalidateTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	LPDIRECT3DTEXTURE9 chktex;
//	chktex = g_texbnk->GetTexData( spritename, texbanktransparent );
//	if( chktex ){
//		_ASSERT( 0 );
//	}


	pd3dtex = 0;//!!!!!!!!!!!!!!!

	return 0;
}


int CMySprite::Render( LPDIRECT3DDEVICE9 pd3ddev, LPD3DXSPRITE pd3dxsprite, D3DXVECTOR2 scale, D3DXVECTOR3 tra )
{
	//D3DCOLOR col;
	//col = D3DCOLOR_ARGB( 255, 255, 255, 255 );

	HRESULT hr;	
	

	//sprite->Beginで、レンダーステートが初期化されるため、ここで、ブレンディングモード関係のステートを設定する必要がある。
	pd3ddev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_cop0 = D3DTOP_MODULATE;
	pd3ddev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
	g_aop0 = D3DTOP_MODULATE;
	pd3ddev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
	g_cop1 = D3DTOP_DISABLE;
	pd3ddev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
	g_aop1 = D3DTOP_DISABLE;

	pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3ddev->SetRenderState( D3DRS_ALPHAREF, 0x08 );
	pd3ddev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_renderstate[ D3DRS_ALPHATESTENABLE ] = 1;
	g_renderstate[ D3DRS_ALPHAREF ] = 0x08;
	g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;


	if( m_blendmode == 0 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;

	}else if( m_blendmode == 1 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ONE;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;
	}else if( m_blendmode ==  2 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

	}else if( m_blendmode == 103 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_DESTCOLOR;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ZERO;
	}else if( m_blendmode == 104 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_REVSUBTRACT;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ZERO;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;
	}else if( m_blendmode == 105 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;
	}else if( m_blendmode == 106 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;
	}else if( m_blendmode == 107 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_DESTCOLOR;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_DESTCOLOR;
	}else if( m_blendmode == 108 ){
//		pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3ddev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );

//		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = 1;
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_DESTCOLOR;
	}



	pd3ddev->SetSamplerState( 0, D3DSAMP_MINFILTER, m_minfilter );
	pd3ddev->SetSamplerState( 1, D3DSAMP_MINFILTER, m_minfilter );
	g_minfilter = m_minfilter;

	pd3ddev->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_magfilter );
	pd3ddev->SetSamplerState( 1, D3DSAMP_MAGFILTER, m_magfilter );
	g_magfilter = m_magfilter;


	//pd3ddev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    //pd3ddev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );


	D3DXMATRIX transmat;
	D3DXMATRIX rotmat;
	D3DXMATRIX beftra, afttra;
	D3DXMATRIX scalemat;
	D3DXMatrixIdentity( &scalemat );
	D3DXMatrixIdentity( &rotmat );
	D3DXMatrixIdentity( &beftra );
	D3DXMatrixIdentity( &afttra );

	D3DXMATRIX posmat;
	D3DXMatrixIdentity( &posmat );
	posmat._41 = tra.x;
	posmat._42 = tra.y;
	posmat._43 = tra.z;

	scalemat._11 = scale.x;
	scalemat._22 = scale.y;
	D3DXMatrixRotationZ( &rotmat, rotdeg * (float)DEG2PAI );
	beftra._41 = -rotcen.x;
	beftra._42 = -rotcen.y;
	afttra._41 = rotcen.x;
	afttra._42 = rotcen.y;

//	transmat = beftra * scalemat * rotmat * afttra * posmat;
	transmat = scalemat * beftra * rotmat * afttra * posmat;

	pd3dxsprite->SetTransform( &transmat );

	RECT rrect;
	rrect.top = (int)( (float)texHeight * startv );
	rrect.bottom = (int)( (float)texHeight * endv );
	rrect.left = (int)( (float)texWidth * startu );
	rrect.right = (int)( (float)texWidth * endu );

	if( pd3dtex ){
		hr = pd3dxsprite->Draw( pd3dtex, &rrect, NULL, NULL, multcol );
	//	hr = pd3dxsprite->Draw( pd3dtex,
	//		NULL, 
	//		&scale,
	//		&rotcen, rotdeg * (float)DEG2PAI,
	//		&tra, multcol );
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			pd3ddev->Release();
			return 1;
		}
	}

	return 0;
}

int CMySprite::SetRotation( D3DXVECTOR2 srccen, float srcdeg )
{
	rotcen = srccen;
	rotdeg = srcdeg;

	return 0;
}
int CMySprite::SetARGB( int srca, int srcr, int srcg, int srcb )
{
	multcol = D3DCOLOR_ARGB( srca, srcr, srcg, srcb );

	return 0;
}

int CMySprite::SetTextureDirtyFlag()
{
	int ret;

	ret = g_texbnk->SetDirtyFlag( spritename, texbanktransparent );
	if( ret ){
		DbgOut( "mysprite : texbnk SetDirtyFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMySprite::GetTextureFromDispObj( int* texidptr )
{
	if( !g_texbnk ){
		DbgOut( "MySprite : GetTextureFromDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( spritename[0] != 0 ){
		int texindex;
		texindex = g_texbnk->FindName( spritename, texbanktransparent );

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


int CMySprite::SetTextureToDispObj( int texid )
{
	if( !g_texbnk ){
		DbgOut( "MySprite : SetTextureToDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;


	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "MySprite : SetTextureToDispObj : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "MySprite : SetTextureToDispObj : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy( spritename, texname );
	texbanktransparent = transparent;



	pd3dtex = g_texbnk->GetTexData( spritename, texbanktransparent );
	if( !pd3dtex ){
		DbgOut( "MySprite : SetTextureToDispObj : texbank GetTexData NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////////

	if( pd3dtex ){
		HRESULT hr;
		D3DSURFACE_DESC	sdesc;
		hr = pd3dtex->GetLevelDesc( 0, &sdesc );
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		texWidth = sdesc.Width;
		texHeight = sdesc.Height;
	}

/***
	if( transparentflag && pd3dtex ){
		ret = SetTextureAlpha();
		if( ret ){
			DbgOut( "MySprite : SetTextureToDispObj : SetTextureAlpha error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
***/

	return 0;
}

int CMySprite::SetDefaultRenderState()
{
	float fogdensity = 1.0f;
	float pointsize = 1.0f;
	float pointscale = 1.0f;
	float pointscaleb = 0.0f;
	float pointscalec = 0.0f;
	float numsegments = 0.0f;
	float tweenfactor = 1.0f;

	m_renderstate[D3DRS_ZENABLE] = D3DZB_TRUE;
	m_renderstate[D3DRS_FILLMODE] =	D3DFILL_SOLID;
	m_renderstate[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
	//m_renderstate[D3DRS_LINEPATTERN] = 0;
	m_renderstate[D3DRS_ZWRITEENABLE] = TRUE;
	m_renderstate[D3DRS_ALPHATESTENABLE] = TRUE;
	m_renderstate[D3DRS_LASTPIXEL] = TRUE;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
	//m_renderstate[D3DRS_SRCBLEND] = D3DBLEND_ONE;
	//m_renderstate[D3DRS_DESTBLEND] = D3DBLEND_ZERO ;
m_renderstate[D3DRS_SRCBLEND] = D3DBLEND_SRCALPHA;
m_renderstate[D3DRS_DESTBLEND] = D3DBLEND_INVSRCALPHA;
	//m_renderstate[D3DRS_SRCBLEND] = D3DBLEND_ONE;
	//m_renderstate[D3DRS_DESTBLEND] = D3DBLEND_ONE;

	m_renderstate[D3DRS_CULLMODE] = D3DCULL_CCW;




	m_renderstate[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL;
	m_renderstate[D3DRS_ALPHAREF] = 0x08;
	m_renderstate[D3DRS_ALPHAFUNC] = D3DCMP_GREATEREQUAL;
	m_renderstate[D3DRS_DITHERENABLE] = FALSE;//<------------
	m_renderstate[D3DRS_ALPHABLENDENABLE] = FALSE;
	m_renderstate[D3DRS_FOGENABLE] = FALSE;
	m_renderstate[D3DRS_SPECULARENABLE] = TRUE;//<----------
	//m_renderstate[D3DRS_ZVISIBLE] = 0;
	m_renderstate[D3DRS_FOGCOLOR] = 0;
	m_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;

	m_renderstate[D3DRS_FOGSTART] = 0;
	m_renderstate[D3DRS_FOGEND] = 0;
	m_renderstate[D3DRS_FOGDENSITY ] = *((DWORD*)&fogdensity);
	//m_renderstate[D3DRS_EDGEANTIALIAS] = FALSE;
	//m_renderstate[D3DRS_ZBIAS] = 0;
	m_renderstate[D3DRS_RANGEFOGENABLE] = FALSE;
	m_renderstate[D3DRS_STENCILENABLE] = FALSE;
	m_renderstate[D3DRS_STENCILFAIL] = D3DSTENCILOP_KEEP;
	m_renderstate[D3DRS_STENCILZFAIL] = D3DSTENCILOP_KEEP;
	m_renderstate[D3DRS_STENCILPASS] = D3DSTENCILOP_KEEP;

	m_renderstate[D3DRS_STENCILFUNC] = D3DCMP_ALWAYS;
	m_renderstate[D3DRS_STENCILREF] = 0;
	m_renderstate[D3DRS_STENCILMASK] = 0xFFFFFFFF;
	m_renderstate[D3DRS_STENCILWRITEMASK] = 0xFFFFFFFF;
	m_renderstate[D3DRS_TEXTUREFACTOR] = 0xFFFFFFFF;//!!!
	m_renderstate[D3DRS_WRAP0] = 0;
	m_renderstate[D3DRS_WRAP1] = 0;
	m_renderstate[D3DRS_WRAP2] = 0;
	m_renderstate[D3DRS_WRAP3] = 0;
	m_renderstate[D3DRS_WRAP4] = 0;


	m_renderstate[D3DRS_WRAP5] = 0;
	m_renderstate[D3DRS_WRAP6] = 0;
	m_renderstate[D3DRS_WRAP7] = 0;
	m_renderstate[D3DRS_CLIPPING] = TRUE;
	//m_renderstate[D3DRS_LIGHTING] = FALSE;
	m_renderstate[D3DRS_LIGHTING] = TRUE;//!!!!!!!!!!!!!!!
	m_renderstate[D3DRS_AMBIENT] = D3DCOLOR_COLORVALUE( 0.40, 0.40, 0.40, 1.0 );
	m_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_NONE;
	m_renderstate[D3DRS_COLORVERTEX] = TRUE;
	m_renderstate[D3DRS_LOCALVIEWER] = TRUE;
	m_renderstate[D3DRS_NORMALIZENORMALS] = FALSE;

	m_renderstate[D3DRS_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
	m_renderstate[D3DRS_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
	m_renderstate[D3DRS_AMBIENTMATERIALSOURCE] = D3DMCS_COLOR2;
	m_renderstate[D3DRS_EMISSIVEMATERIALSOURCE] = D3DMCS_MATERIAL;
	m_renderstate[D3DRS_VERTEXBLEND] = D3DVBF_DISABLE;
	m_renderstate[D3DRS_CLIPPLANEENABLE] = 0;
	//m_renderstate[D3DRS_SOFTWAREVERTEXPROCESSING] = FALSE;//!!!
	m_renderstate[D3DRS_POINTSIZE] = *((DWORD*)&pointsize);
	m_renderstate[D3DRS_POINTSIZE_MIN] = *((DWORD*)&pointsize);
	m_renderstate[D3DRS_POINTSPRITEENABLE] = FALSE;


	m_renderstate[D3DRS_POINTSCALEENABLE] = FALSE;
	m_renderstate[D3DRS_POINTSCALE_A] = *((DWORD*)&pointscale);
	m_renderstate[D3DRS_POINTSCALE_B] = *((DWORD*)&pointscaleb);
	m_renderstate[D3DRS_POINTSCALE_C] = *((DWORD*)&pointscalec);
	m_renderstate[D3DRS_MULTISAMPLEANTIALIAS] = FALSE;//!!!
	m_renderstate[D3DRS_MULTISAMPLEMASK] = 0xFFFFFFFF;
	m_renderstate[D3DRS_PATCHEDGESTYLE] = 0;
	//m_renderstate[D3DRS_PATCHSEGMENTS] = *((DWORD*)&numsegments);
	m_renderstate[D3DRS_DEBUGMONITORTOKEN] = D3DDMT_ENABLE;
	m_renderstate[D3DRS_POINTSIZE_MAX] = *((DWORD*)&pointsize);


	m_renderstate[D3DRS_INDEXEDVERTEXBLENDENABLE] = TRUE;//!!!
	m_renderstate[D3DRS_COLORWRITEENABLE] = 0x0000000F;
	m_renderstate[D3DRS_TWEENFACTOR] = *((DWORD*)&tweenfactor);
	m_renderstate[D3DRS_BLENDOP] = D3DBLENDOP_ADD;

//////////////
	m_minfilter = D3DTEXF_LINEAR;
	m_magfilter = D3DTEXF_LINEAR;



	return 0;
}

int CMySprite::SetRenderState( int statetype, int value )
{
	if( (statetype < 0) || (statetype > D3DRS_BLENDOP) ){
		DbgOut( "mysprite : SetRenderState : statetype error !!!\n" );
		return 1;
	}

	m_renderstate[ statetype ] = value;

//DbgOut( "mysprite : SetRenderState : type %d, value %d\r\n", statetype, value );

	return 0;
}


int CMySprite::SetRenderState( LPDIRECT3DDEVICE9 pdev )
{
	HRESULT hr;
	int i;
	int rsno;
	int curvalue;
	for( i = 0; i < 80; i++ ){
		rsno = g_rsindex[i];

		if( rsno >= 0 ){
			curvalue = m_renderstate[ rsno ];
			if( g_renderstate[ rsno ] != curvalue ){
				hr = pdev->SetRenderState( (D3DRENDERSTATETYPE)rsno, curvalue );
				if( hr != D3D_OK ){
					DbgOut( "mysprite : SetRenderState : %d error !!!\r\n", rsno );
				}
				g_renderstate[ rsno ] = curvalue;

//DbgOut( "mysprite : SetRenderState pdev : rsno %d, value %d\r\n", rsno, curvalue );
			}
		}
	}




	return 0;
}

int CMySprite::SetBlendingMode( int bmode )
{

	m_blendmode = bmode;
	return 0;
}

int CMySprite::SetSpriteUV( float srcstartu, float srcendu, float srcstartv, float srcendv )
{
	startu = srcstartu;
	startu = max( 0.0f, startu );
	startu = min( 1.0f, startu );

	endu = srcendu;
	endu = max( 0.0f, endu );
	endu = min( 1.0f, endu );
	if( endu < startu ){
		endu = startu;
	}
//////
	startv = srcstartv;
	startv = max( 0.0f, startv );
	startv = min( 1.0f, startv );

	endv = srcendv;
	endv = max( 0.0f, endv );
	endv = min( 1.0f, endv );
	if( endv < startv ){
		endv = startv;
	}

	return 0;
}
