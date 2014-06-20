#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
//#include "D3DTextr.h"

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <coef.h>

#include <bgdisp2.h>
#include <texbank.h>
//#include "dxutil.h"
//#include "c:\program files\microsoft platform sdk\samples\multimedia\Direct3D\RokDeBone\dxutil.h"
//#include "c:\program files\microsoft platform sdk\samples\multimedia\common\include\DXUtil.h"
//#include "c:\program files\microsoft sdk\samples\multimedia\common\include\DXUtil.h"

#include "c:\pgfile9\e3dhsp3\DXUtil.h"


extern D3DCOLOR g_fogcolor;
extern int g_fogenable;
extern float g_fogstart;
extern float g_fogend;	
extern float g_proj_far;
extern float g_fogtype;

extern DWORD g_minfilter;
extern DWORD g_magfilter;

extern DWORD g_renderstate[ D3DRS_BLENDOP + 1 ];

extern LPDIRECT3DTEXTURE9 g_curtex0;
extern LPDIRECT3DTEXTURE9 g_curtex1;

extern DWORD g_cop0;
extern DWORD g_cop1;
extern DWORD g_aop0;
extern DWORD g_aop1;

extern CTexBank* g_texbnk;// shdhandler.cpp


CBgDisp2::CBgDisp2( int resetflag, LPDIRECT3DDEVICE9 pd3dDevice, char* name1, int trans1, char* name2, int trans2, float mvu, float mvv, 
	COLORREF srccolor, int srcisround, float fogdist, UV startuv, UV enduv,
	int srcfixsize )
{
	transparent1 = trans1;
	transparent2 = trans2;

	uanime = mvu;
	vanime = mvv;

	m_fogdist = fogdist;
	col = srccolor;
	isround = srcisround;

	m_startuv = startuv;
	m_enduv = enduv;

	fixsize = srcfixsize;

	pd3dDevice->SetTexture( 0, NULL );
	g_curtex0 = NULL;
	pd3dDevice->SetTexture( 1, NULL );
	g_curtex1 = NULL;


	int ret;
	float zmax = 0.9999999f;
	//float zmax = 1.0f;
	//float zmax = 0.999999999999f;


	D3DCOLOR	curcolor;

	if( resetflag == 0 ){
		ZeroMemory( texname, _MAX_PATH );
		ZeroMemory( texname2, _MAX_PATH );
		if( name1 && *name1 ){
			strcpy_s( texname, _MAX_PATH, name1 );
		}
		if( name2 && *name2 )
			strcpy_s( texname2, _MAX_PATH, name2 );
	}


	ZeroMemory( m_tlv1, sizeof( D3DTLVERTEX ) * BGTLVNUM );
	ZeroMemory( m_tlv2, sizeof( D3DTLVERTEX2 ) * BGTLVNUM );


	curcolor = D3DCOLOR_ARGB( 255, GetRValue(srccolor), GetGValue(srccolor), GetBValue(srccolor) );

	float ffogval;
	if( g_fogtype == 0.0f ){
		//ffogval = ( g_fogend - g_proj_far ) / ( g_fogend - g_fogstart );
		ffogval = ( g_fogend - m_fogdist ) / ( g_fogend - g_fogstart );
		if( ffogval < 0.0f )
			ffogval = 0.0f;
		if( ffogval > 1.0f )
			ffogval = 1.0f;
	}else{
		ffogval = 0.5f;
	}
	unsigned char ucfog;
	ucfog = (unsigned char)(ffogval * 255.0f);


	float deltau, deltav;
	deltau = ( enduv.u - startuv.u ) / (float)(BGUNUM - 1);
	deltav = ( enduv.v - startuv.v ) / (float)(BGVNUM - 1);
	int uno, vno;
	int tlvno = 0;
	for( vno = 0; vno < BGVNUM; vno++ ){
		for( uno = 0; uno < BGUNUM; uno++ ){

			m_tlv1[tlvno].tu = startuv.u + (float)uno * deltau;
			m_tlv1[tlvno].tv = startuv.v + (float)vno * deltav;
			// 一番奥
			m_tlv1[tlvno].sz = zmax;
			m_tlv1[tlvno].rhw = 1.0f / zmax;
			m_tlv1[tlvno].color = curcolor;
			//m_tlv1[tlvno].specular = 0xFF000000;
			m_tlv1[tlvno].specular = ucfog << 24;

/////////
			m_tlv2[tlvno].tu1 = startuv.u + (float)uno * deltau;
			m_tlv2[tlvno].tv1 = startuv.v + (float)vno * deltav;
			m_tlv2[tlvno].tu2 = startuv.u + (float)uno * deltau;
			m_tlv2[tlvno].tv2 = startuv.v + (float)vno * deltav;
			// 一番奥
			m_tlv2[tlvno].sz = zmax;
			m_tlv2[tlvno].rhw = 1.0f / zmax;
			m_tlv2[tlvno].color = curcolor;
			m_tlv2[tlvno].specular = ucfog << 24;
			//m_tlv2[tlvno].specular = 0xFF000000;

			tlvno++;
		}
	}


	int curindex = 0;
	for( vno = 0; vno < (BGVNUM - 1); vno++ ){
		for( uno = 0; uno < (BGUNUM - 1); uno++ ){
			m_Indices[curindex] = BGUNUM * vno + uno;
			m_Indices[curindex + 2] = BGUNUM * (vno + 1) + uno;
			m_Indices[curindex + 1] = BGUNUM * vno + uno + 1;
			curindex += 3;

			m_Indices[curindex] = BGUNUM * (vno + 1) + uno;
			m_Indices[curindex + 2] = BGUNUM * (vno + 1) + uno + 1;
			m_Indices[curindex + 1] = BGUNUM * vno + uno + 1;
			curindex += 3;
		}
		
	}

	//m_Indices[0] = 1;
	//m_Indices[1] = 3;
	//m_Indices[2] = 0;
	//m_Indices[3] = 2;



	ret = CheckMultiTexOk( pd3dDevice );
	if( ret ){
		multitexok = 0;
		DbgOut( "bgdisp2 : ini : CheckMultiTexOk : warning !!!\n" );
	}

	ret = Create3DBuffers( pd3dDevice );
	_ASSERT( !ret );
	if( ret ){
		DbgOut( "bgdisp2 : Create3DBuffers error !!!\n" );
	}

	if( resetflag == 0 ){
		ret = CreateTexture( pd3dDevice );
		_ASSERT( !ret );
		if( ret ){
			DbgOut( "bgdisp2 : CreateTexture error !!!\n" );
		}
	}

	ret = Copy2IndexBuffer();
	_ASSERT( !ret );
	if( ret ){
		DbgOut( "bgdisp2 : Copy2IndexBuffer error !!!\n" );
	}

	ret = Copy2VertexBuffer();
	_ASSERT( !ret );
	if( ret ){
		DbgOut( "bgdisp2 : Copy2VertexBuffer error !!!\n" );
	}


///////////////
//    pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//    pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	if( g_cop0 != D3DTOP_MODULATE ){
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_aop0 != D3DTOP_MODULATE ){
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );



    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    //pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );//頂点色はソフトウェアで計算します。
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  TRUE );
    pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    //pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	

	pd3dDevice->SetRenderState( D3DRS_WRAP0, 0 );
    pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,        D3DZB_TRUE );
    //pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x40404040 );
    pd3dDevice->SetRenderState( D3DRS_AMBIENT, 
        D3DCOLOR_COLORVALUE( 0.40, 0.40, 0.40, 1.0 ) );

	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
//////

	//pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, D3DZB_TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );


	m_minfilter = D3DTEXF_LINEAR;
	m_magfilter = D3DTEXF_LINEAR;


}

int CBgDisp2::Create3DBuffers( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	DWORD elemsize, fvf;

	if( multitexok == 0 ){
		elemsize = sizeof( D3DTLVERTEX );
		fvf = D3DFVF_TLVERTEX;
	}else{
		elemsize = sizeof( D3DTLVERTEX2 );
		fvf = D3DFVF_TLVERTEX2;
	}


    hr = pd3dDevice->CreateVertexBuffer( BGTLVNUM * elemsize,
		D3DUSAGE_WRITEONLY, fvf,
		D3DPOOL_MANAGED, &m_VB, NULL );
	if( FAILED(hr) ){
		DbgOut( "bgdisp2 : Create3DBuffers : CreateVertexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

    hr = pd3dDevice->CreateIndexBuffer( BGINDEXNUM * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED,
		&m_IB, NULL );
    if( FAILED(hr) ){
		DbgOut( "bgdisp2 : Create3DBuffers : CreateIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CBgDisp2::CreateTexture( LPDIRECT3DDEVICE9 pd3dDevice )
{

	pd3dDevice->SetTexture( 0, NULL );
	g_curtex0 = NULL;
	pd3dDevice->SetTexture( 1, NULL );
	g_curtex1 = NULL;


	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_cop0 = D3DTOP_MODULATE;
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
//	g_aop0 = D3DTOP_MODULATE;
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
	g_cop1 = D3DTOP_DISABLE;
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_aop0 = D3DTOP_SELECTARG1;
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
	g_aop1 = D3DTOP_DISABLE;


	int ret = 0;

	if( !g_texbnk ){
		g_texbnk = new CTexBank( 1, D3DX_FILTER_LINEAR );//!!!!!!!!!!!
		if( !g_texbnk ){
			DbgOut( "CBgDisp2 : CreateTexture : new CTexBank error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( *texname ){
		ret = g_texbnk->AddName( 0, texname, transparent1, D3DPOOL_MANAGED );		
		_ASSERT( ret >= 0 );
		if( ret < 0 ){
			DbgOut( "bgdisp2 : CreateTexture : AddName 1 error !!!\n" );
		}
	}
	if( *texname2 ){
		ret = g_texbnk->AddName( 0, texname2, transparent2, D3DPOOL_MANAGED );		
		_ASSERT( ret >= 0 );
		if( ret < 0 ){
			DbgOut( "bgdisp2 : CreateTexture : AddName 2 error !!!\n" );
		}
	}

	int texi1, texi2;
	texi1 = g_texbnk->GetTexNoByName( texname, transparent1 );
	texi2 = g_texbnk->GetTexNoByName( texname2, transparent2 );

	if( texi1 >= 0 ){
		ret = g_texbnk->CreateTexData( texi1, pd3dDevice );
		if( (ret != 0) && (ret != -2) ){
			DbgOut( "BgDisp : g_texbnk : CreateTexData 1 error !!! set null texture\n" );
			int ret2;
			ret2 = g_texbnk->SetTexData( texi1, NULL );
			if( ret2 ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	if( texi2 >= 0 ){
		ret = g_texbnk->CreateTexData( texi2, pd3dDevice );
		if( (ret != 0) && (ret != -2) ){
			DbgOut( "BgDisp : g_texbnk : CreateTexData 2 error !!! set null texture\n" );
			int ret2;
			ret2 = g_texbnk->SetTexData( texi2, NULL );
			if( ret2 ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( texi1 >= 0 ){
		ret = g_texbnk->GetTexOrgSize( texi1, &fixx, &fixy );
		if( ret ){
			fixx = 256;
			fixy = 256;
		}
	}else{
		fixx = 256;
		fixy = 256;
	}

	DbgOut( "BgDisp : CreateTexture : fixx %d, fixy %d\r\n", fixx, fixy );

	return 0;
}
int CBgDisp2::Copy2IndexBuffer()
{
	HRESULT hr;
    WORD* pIndices;

	if( !m_IB ){
		DbgOut( "bgdisp2 : Copy2IndexBuffer : m_IB null error !!!\n" );
		return 1;
	}

    hr = m_IB->Lock( 0, BGINDEXNUM * sizeof(WORD), (void**)&pIndices, 0 );
    if( FAILED(hr) ){
		DbgOut( "bgdisp2 : Copy2IndexBuffer : m_IB Lock error !!!\n" );
		return 1;
	}
		
	MoveMemory( pIndices, m_Indices, BGINDEXNUM * sizeof(WORD) );

    m_IB->Unlock();

	return 0;

}
int CBgDisp2::Copy2VertexBuffer()
{
	if( !m_VB ){
		DbgOut( "bgdisp2 : Copy2VertexBuffer : m_VB null error !!!\n" );
		return 1;
	}

	
	if( multitexok == 0 ){
		D3DTLVERTEX* pVertices;
		if( FAILED( m_VB->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
			DbgOut( "bgdisp2 : Copy2VertexBuffer : m_VB Lock error !!!\n" );
			return 1;
		}

		MoveMemory( pVertices, m_tlv1, BGTLVNUM * sizeof( D3DTLVERTEX ) );

	}else{
		D3DTLVERTEX2* pVertices;
		if( FAILED( m_VB->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
			DbgOut( "bgdisp2 : Copy2VertexBuffer : m_VB Lock error !!!\n" );
			return 1;
		}

		MoveMemory( pVertices, m_tlv2, BGTLVNUM * sizeof( D3DTLVERTEX2 ) );
	}

	m_VB->Unlock();

	return 0;

}

CBgDisp2::~CBgDisp2()
{
	SAFE_RELEASE( m_VB );
	SAFE_RELEASE( m_IB );

}

int CBgDisp2::SetBgSize( DWORD dwWidth, DWORD dwHeight )
{
	float startx, starty;
	float w, h;
	if( (fixsize == 0) || (fixx <= 0) || (fixy <= 0) ){
		startx = 0.0f;
		starty = 0.0f;
		w = (float)dwWidth;
		h = (float)dwHeight;
	}else{
		starty = 0;
		h = (float)dwHeight;

		w = ((float)fixx / (float)fixy) * h;
		startx = (float)dwWidth * 0.5f - (float)w * 0.5f;
	}

	float deltaw, deltah;
	deltaw = w / (float)(BGUNUM - 1);
	deltah = h / (float)(BGVNUM - 1);

	int uno, vno;
	int tlvno = 0;
	for( vno = 0; vno < BGVNUM; vno++ ){
		for( uno = 0; uno < BGUNUM; uno++ ){
			m_tlv1[tlvno].sx = startx + (float)uno * deltaw - 0.5f;
			m_tlv1[tlvno].sy = starty + (float)vno * deltah - 0.5f;

			m_tlv2[tlvno].sx = startx + (float)uno * deltaw - 0.5f;
			m_tlv2[tlvno].sy = starty + (float)vno * deltah - 0.5f;

			tlvno++;
		}
	}
	return 0;
}

/***
int CBgDisp2::SetBgSize( DWORD dwWidth, DWORD dwHeight )
{
	float startx, starty;
	float w, h;
	if( fixsize == 0 ){
		startx = 0.0f;
		starty = 0.0f;
		w = (float)dwWidth;
		h = (float)dwHeight;
	}else{
		startx = (float)dwWidth * 0.5f - (float)fixx * 0.5f;
		starty = (float)dwHeight * 0.5f - (float)fixy * 0.5f;
		w = (float)fixx;
		h = (float)fixy;
	}

	float deltaw, deltah;
	deltaw = w / (float)(BGUNUM - 1);
	deltah = h / (float)(BGVNUM - 1);

	int uno, vno;
	int tlvno = 0;
	for( vno = 0; vno < BGVNUM; vno++ ){
		for( uno = 0; uno < BGUNUM; uno++ ){
			m_tlv1[tlvno].sx = startx + (float)uno * deltaw - 0.5f;
			m_tlv1[tlvno].sy = starty + (float)vno * deltah - 0.5f;

			m_tlv2[tlvno].sx = startx + (float)uno * deltaw - 0.5f;
			m_tlv2[tlvno].sy = starty + (float)vno * deltah - 0.5f;

			tlvno++;
		}
	}
	return 0;
}
***/

int CBgDisp2::SetUVAnime( float srcuanime, float srcvanime )
{
	uanime = srcuanime;
	vanime = srcvanime;
	return 0;
}

int CBgDisp2::MoveTexUV( D3DXMATRIX matView )
{
	int i;
	D3DTLVERTEX* tlvptr1 = m_tlv1;
	D3DTLVERTEX2* tlvptr2 = m_tlv2;

	if( (uanime != 0.0f) || (vanime != 0.0f ) ){		
		for( i = 0; i < BGTLVNUM; i++ ){
			tlvptr2->tu2 += uanime;
			tlvptr2->tv2 += vanime;
			tlvptr2++;
		}
	}

	if( isround ){
		D3DXVECTOR3 vecbef;// = {0.0, 0.0, -1.0};
		D3DXVECTOR3 vecaft;

		vecbef.x = 0.0f; vecbef.y = 0.0f; vecbef.z = -1.0f;

		float lx = vecbef.x;
		float ly = vecbef.y;
		float lz = vecbef.z;

		//！！！！　平行移動成分は、加味しない。
		vecaft.x = matView._11*lx + matView._21*ly + matView._31*lz;// + matView._41;
		vecaft.y = matView._12*lx + matView._22*ly + matView._32*lz;// + matView._42;
		vecaft.z = matView._13*lx + matView._23*ly + matView._33*lz;// + matView._43;

		D3DXVec3Normalize( &vecaft, &vecaft );
		
		
		D3DXVECTOR2 vec0; // = {0.0f, -1.0f};
		D3DXVECTOR2 vec1;
		vec0.x = 0.0f; vec0.y = -1.0f;
		vec1.x = vecaft.x; vec1.y = vecaft.z;
		D3DXVec2Normalize( &vec1, &vec1 );

		float dot;
		dot = D3DXVec2Dot( &vec0, &vec1 );
		float rad;
		if( dot <= -1.0f )
			dot = -1.0f;
		else if( dot >= 1.0f )
			dot = 1.0f;

		rad = (float)acos( dot );
		float ccw;
		ccw = D3DXVec2CCW( &vec0, &vec1 );

		//float roty;
		//if( ccw >= 0.0f ){
		//	roty = -rad;
		//}else{
		//	roty = rad;
		//}
		float roty;
		if( ccw >= 0.0f ){
			roty = rad;
		}else{
			roty = -rad;
		}


		float centeru;
		centeru = 0.5f + roty * ( 0.5f / PI );
		float usize, befu, aftu;
		usize = 0.4f;
		befu = centeru - usize;
		aftu = centeru + usize;
		
		//if( befu < 0.0f )
		//	befu += 1.0f;
		//if( aftu > 1.0f )
		//	aftu -= 1.0f;


		_ASSERT( BGUNUM == 3 );//!!!!!!!!!!!!!!!!!
		int uno, vno;
		int tlvno = 0;
		for( vno = 0; vno < BGVNUM; vno++ ){
			for( uno = 0; uno < BGUNUM; uno++ ){
				if( uno == 0 ){
					m_tlv1[tlvno].tu = befu;
					m_tlv2[tlvno].tu1 = befu;
				}else if( uno == 1 ){
					m_tlv1[tlvno].tu = centeru;
					m_tlv2[tlvno].tu1 = centeru;
				}else if( uno == 2 ){
					m_tlv1[tlvno].tu = aftu;
					m_tlv2[tlvno].tu1 = aftu;
				}else{
					_ASSERT( 0 );
					m_tlv1[tlvno].tu = 0.0f;
					m_tlv2[tlvno].tu1 = 0.0f;
				}
				tlvno++;
			}
		}
	}

	return 0;
}

int CBgDisp2::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
//	pd3dDevice->SetTexture( 0, NULL );
//	g_curtex0 = NULL;
//	pd3dDevice->SetTexture( 1, NULL );
//	g_curtex1 = NULL;

////////
/***
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_cop0 = D3DTOP_MODULATE;
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
	g_aop0 = D3DTOP_MODULATE;

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    //pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );//頂点色はソフトウェアで計算します。
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  TRUE );
    pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    //pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	
	pd3dDevice->SetRenderState( D3DRS_WRAP0, 0 );
    pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,        D3DZB_TRUE );
    //pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x40404040 );
    pd3dDevice->SetRenderState( D3DRS_AMBIENT, 
        D3DCOLOR_COLORVALUE( 0.40, 0.40, 0.40, 1.0 ) );

	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );

	//pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, D3DZB_TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
***/
///////

	HRESULT hres;
	int ret = 0;

	hres = pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	if( hres != D3D_OK ){
		DbgOut( "bgdisp2 : Render : SetRenderState 0 error !!!\n" );
	}
	
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	hres = pd3dDevice->SetRenderState( D3DRS_WRAP0, 0 );
	g_renderstate[ D3DRS_WRAP0 ] = 0;
	if( hres != D3D_OK ){
		DbgOut( "bgdisp2 : Render : SetRenderState 1 error !!!\n" );
	}

	if( multitexok != 0 ){
		hres = pd3dDevice->SetRenderState( D3DRS_WRAP1, 0 );
		g_renderstate[ D3DRS_WRAP1 ] = 0;
		if( hres != D3D_OK ){
			DbgOut( "bgdisp2 : Render : SetRenderState 2 error !!!\n" );
		}
	}


	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, g_fogenable );
	pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, g_fogcolor );
	pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
	pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
	pd3dDevice->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );



	LPDIRECT3DTEXTURE9 tex0 = g_texbnk->GetTexData( texname, transparent1 );
	LPDIRECT3DTEXTURE9 tex1 = g_texbnk->GetTexData( texname2, transparent2 );

//	if( g_curtex0 != tex0 ){
		pd3dDevice->SetTexture( 0, tex0 );
		g_curtex0 = tex0;
//	}

//DbgOut( "bgdisp2 : Render : %s, %x : %s, %x\r\n", texname, tex0, texname2, tex1 );


	if( tex1 && (multitexok != 0) ){

		if( g_cop0 != D3DTOP_MODULATE ){
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_cop0 = D3DTOP_MODULATE;
		}
		if( g_aop0 != D3DTOP_MODULATE ){
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			g_aop0 = D3DTOP_MODULATE;
		}
		if( g_cop1 != D3DTOP_MODULATE ){
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_cop1 = D3DTOP_MODULATE;
		}
		if( g_aop1 != D3DTOP_MODULATE ){
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
			g_aop1 = D3DTOP_MODULATE;
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ONE;
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

//		if( g_curtex1 != tex1 ){
			pd3dDevice->SetTexture( 1, tex1 );
			g_curtex1 = tex1;
//		}

	}else{
		if( g_cop0 != D3DTOP_MODULATE ){
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_cop0 = D3DTOP_MODULATE;
		}
		if( g_cop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
			g_cop1 = D3DTOP_DISABLE;
		}
		if( g_aop0 != D3DTOP_SELECTARG1 ){
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			g_aop0 = D3DTOP_SELECTARG1;
		}
		if( g_aop1 = D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
			g_aop1 = D3DTOP_DISABLE;
		}
//		if( g_curtex1 != NULL ){
			pd3dDevice->SetTexture( 1, NULL );
			g_curtex1 = NULL;
//		}

	}
	
    
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SUBTRACT );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_PREMODULATE );
	
	HRESULT hr1, hr2;
	DWORD curfilter;
	curfilter = m_minfilter;
	if( g_minfilter != curfilter ){
		hr1 = pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, curfilter );
		if( hr1 != D3D_OK ){
			DbgOut( "sh : SetRenderState : minfilter 0 error !!!\n" );
		}
		hr2 = pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, curfilter );
		if( hr2 != D3D_OK ){
			DbgOut( "sh : SetRenderState : minfilter 1 error !!!\n" );
		}
		g_minfilter = curfilter;
	}

	curfilter = m_magfilter;
	if( g_magfilter != curfilter ){
		hr1 = pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, curfilter );
		if( hr1 != D3D_OK ){
			DbgOut( "sh : SetRenderState : magfilter 0 error !!!\n" );
		}
		hr2 = pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, curfilter );
		if( hr2 != D3D_OK ){
			DbgOut( "sh : SetRenderState : magfilter 1 error !!!\n" );
		}
		g_magfilter = curfilter;
	}




	if( multitexok == 0 ){
		hres = pd3dDevice->SetVertexShader( NULL );
		pd3dDevice->SetFVF( D3DFVF_TLVERTEX );
		if( hres != D3D_OK ){
			DbgOut( "bgdisp2 : Render : SetVertexShader error !!!\n" );
		}
		hres = pd3dDevice->SetStreamSource( 0, m_VB, 0, sizeof(D3DTLVERTEX) );
		if( hres != D3D_OK ){
			DbgOut( "bgdisp2 : Render : SetStreamSource error !!!\n" );
		}		
	}else{
		hres = pd3dDevice->SetVertexShader( NULL );
		pd3dDevice->SetFVF( D3DFVF_TLVERTEX2 );
		if( hres != D3D_OK ){
			DbgOut( "bgdisp2 : Render : SetVertexShader error !!!\n" );
		}
		hres = pd3dDevice->SetStreamSource( 0, m_VB, 0, sizeof(D3DTLVERTEX2) );
		if( hres != D3D_OK ){
			DbgOut( "bgdisp2 : Render : SetStreamSource error !!!\n" );
		}
	}
    hres = pd3dDevice->SetIndices( m_IB );
	if( hres != D3D_OK ){
		DbgOut( "bgdisp2 : Render : SetIndices error !!!\n" );
	}

	//hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP,
	//								0,
	//								4,
	//								0L, //m_dispIndices, 
	//								2
	//);

	hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
									0,
									0,
									BGTLVNUM,
									0L, //m_dispIndices, 
									(BGUNUM - 1) * (BGVNUM - 1) * 2	//m_numPrim
									);

	if( FAILED( hres ) ){
		DbgOut( "Render error !!! bgdisp\n" );
		ret = 1;
	}

	if( multitexok != 0 ){
		if( g_curtex1 != NULL ){
			pd3dDevice->SetTexture( 1, NULL );
			g_curtex1 = NULL;
		}
		if( g_cop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
			g_cop1 = D3DTOP_DISABLE;
		}
	}


	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );


	return ret;

}

int CBgDisp2::UpdateVertexBuffer()
{
	int ret;

//	ret = Copy2IndexBuffer();
//	_ASSERT( !ret );

	ret = Copy2VertexBuffer();
	_ASSERT( !ret );
	if( ret ){
		DbgOut( "bgdisp2 : UpdateVertexBuffer : Copy2VertexBuffer error !!!\n" );
		return 1;
	}
	return ret;
}

int CBgDisp2::CheckMultiTexOk( LPDIRECT3DDEVICE9 pd3ddev )
{

	HRESULT hr;
	D3DCAPS9 caps8;
	hr = pd3ddev->GetDeviceCaps( &caps8 );
	if( hr != D3D_OK ){
		DbgOut( "bgdisp2 : checkmultitexok : GetDeviceCaps error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	WORD maxtexblendnum = (WORD)( caps8.FVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK );
	DWORD maxstages = caps8.MaxTextureBlendStages;
	DWORD maxsimu = caps8.MaxSimultaneousTextures;
	DWORD modu = caps8.TextureOpCaps & D3DTEXOPCAPS_MODULATE;

	DbgOut( "bgdisp2 : CheckMultiTexOk : caps : maxtexcoord %d, maxstages %d, maxsimu %d, moduleop %d\n", maxtexblendnum, maxstages, maxsimu, modu );

	if( (maxtexblendnum >= 2) && (maxstages >= 2) && (maxsimu >= 2) && (modu > 0) ){
		multitexok = 1;
	}else{
		multitexok = 0;
	}


	DbgOut( "bgdisp2 : multitexok %d\n", multitexok );


	return 0;
}

int CBgDisp2::SetU( float srcstartu, float srcendu )
{

	//startu = srcstartu;
	//endu = srcendu;

	m_startuv.u = srcstartu;
	m_enduv.u = srcendu;


	float deltau;
	deltau = (srcendu - srcstartu) / (float)(BGUNUM - 1);
	int uno, vno;
	int tlvno = 0;
	for( vno = 0; vno < BGVNUM; vno++ ){
		for( uno = 0; uno < BGUNUM; uno++ ){

			m_tlv1[tlvno].tu = srcstartu + (float)uno * deltau;

			m_tlv2[tlvno].tu1 = srcstartu + (float)uno * deltau;

			
			tlvno++;
		}
	}

	return 0;
}
int CBgDisp2::SetV( float srcstartv, float srcendv )
{

	//startv = srcstartv;
	//endv = srcendv;

	m_startuv.v = srcstartv;
	m_enduv.v = srcendv;


	float deltav;
	deltav = (srcendv - srcstartv) / (float)(BGVNUM - 1);
	int uno, vno;
	int tlvno = 0;
	for( vno = 0; vno < BGVNUM; vno++ ){
		for( uno = 0; uno < BGUNUM; uno++ ){

			m_tlv1[tlvno].tv = srcstartv + (float)vno * deltav;

			m_tlv2[tlvno].tv1 = srcstartv + (float)vno * deltav;

			
			tlvno++;
		}
	}

	return 0;
}

int CBgDisp2::SetTextureDirtyFlag()
{
	int ret;
	ret = g_texbnk->SetDirtyFlag( texname, transparent1 );	
	if( ret ){
		DbgOut( "sh : SetTextureDirtyFlag 1 : texbnk SetDirtyFlag texname error !!!\n" );
		_ASSERT( 0 );
	}
	
	ret = g_texbnk->SetDirtyFlag( texname2, transparent2 );	
	if( ret ){
		DbgOut( "sh : SetTextureDirtyFlag 2 : texbnk SetDirtyFlag texname error !!!\n" );
		_ASSERT( 0 );
	}

	return 0;
}