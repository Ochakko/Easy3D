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
#include "dbg.h"

#include <crtdbg.h>

#include <coef.h>

#include <bgdisp.h>
#include <texbank.h>
//#include "dxutil.h"
//#include "c:\program files\microsoft platform sdk\samples\multimedia\Direct3D\RokDeBone\dxutil.h"
//#include "c:\program files\microsoft platform sdk\samples\multimedia\common\include\DXUtil.h"
//#include "c:\program files\microsoft sdk\samples\multimedia\common\include\DXUtil.h"
#include "c:\DXSDK\samples\multimedia\common\include\DXUtil.h"

extern LPDIRECT3DTEXTURE9 g_curtex0;
extern LPDIRECT3DTEXTURE9 g_curtex1;


CBgDisp::CBgDisp( LPDIRECT3DDEVICE9 pd3dDevice, char* name1, char* name2, float mvu, float mvv, COLORREF srccolor )
{

	int i, ret;
	//float zmax = 0.9999999f;
	float zmax = 1.0f;
	D3DCOLOR	curcolor;

	ZeroMemory( texname, _MAX_PATH );
	ZeroMemory( texname2, _MAX_PATH );
	if( name1 && *name1 ){
		strcpy( texname, name1 );
	}
	if( name2 && *name2 )
		strcpy( texname2, name2 );

	
	m_Indices[0] = 1;
	m_Indices[1] = 3;
	m_Indices[2] = 0;
	m_Indices[3] = 2;
	

	ZeroMemory( m_tlv, sizeof( D3DTLVERTEX2 ) * 4 );
	//curcolor = D3DCOLOR_ARGB( 255, 255, 255, 255 );

	int srcr, srcg, srcb;
	srcr = GetRValue(srccolor);
	srcg = GetGValue(srccolor);
	srcb = GetBValue(srccolor);

//DbgOut( "bgdisp : constructor : color %d %d %d\n", srcr, srcg, srcb );

	curcolor = D3DCOLOR_ARGB( 255, srcr, srcg, srcb );
	for( i = 0; i < 4; i++ ){
		// ˆê”Ô‰œ
		m_tlv[i].sz = zmax;
		m_tlv[i].rhw = 1.0f / zmax;
		m_tlv[i].color = curcolor;
		m_tlv[i].specular = 0xFF000000;
	}
	m_tlv[0].tu1 = 0.0f;
	m_tlv[0].tv1 = 0.0f;
	m_tlv[1].tu1 = 0.999999f;
	m_tlv[1].tv1 = 0.0f;
	m_tlv[2].tu1 = 0.0f;
	m_tlv[2].tv1 = 0.999999f;
	m_tlv[3].tu1 = 0.999999f;
	m_tlv[3].tv1 = 0.999999f;

	m_tlv[0].tu2 = 0.0f;
	m_tlv[0].tv2 = 0.0f;
	m_tlv[1].tu2 = 0.999999f;
	m_tlv[1].tv2 = 0.0f;
	m_tlv[2].tu2 = 0.0f;
	m_tlv[2].tv2 = 0.999999f;
	m_tlv[3].tu2 = 0.999999f;
	m_tlv[3].tv2 = 0.999999f;

	uanime = mvu;
	vanime = mvv;

	ret = Create3DBuffers( pd3dDevice );
	_ASSERT( !ret );

	ret = CreateTexture( pd3dDevice );
	_ASSERT( !ret );

	ret = Copy2IndexBuffer();
	_ASSERT( !ret );

	ret = Copy2VertexBuffer();
	_ASSERT( !ret );


}

int CBgDisp::Create3DBuffers( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

    hr = pd3dDevice->CreateVertexBuffer( 4 * sizeof(D3DTLVERTEX2),
		D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX2,
		D3DPOOL_MANAGED, &m_VB );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}

    hr = pd3dDevice->CreateIndexBuffer( 4 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED,
		&m_IB );
    if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CBgDisp::CreateTexture( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int ret = 0;

	m_texbank = new CTexBank( 1, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR );
	if( !m_texbank ){
		DbgOut( "CBgDisp : CreateTexture : new CTexBank error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( *texname ){
		ret = m_texbank->AddName( texname, 0 );		
		_ASSERT( ret >= 0 );
	}
	if( *texname2 ){
		ret = m_texbank->AddName( texname2, 0 );		
		_ASSERT( ret >= 0 );
	}

	int j;
	for( j = 0; j < m_texbank->namenum; j++ ){
		ret = m_texbank->CreateTexData( j, pd3dDevice );
		if( ret ){
			DbgOut( "BgDisp : m_texbank : CreateTexData error !!! set null texture\n" );
			int ret2;
			ret2 = m_texbank->SetTexData( j, NULL );
			if( ret2 ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	

	return 0;
}
int CBgDisp::Copy2IndexBuffer()
{
	HRESULT hr;
    WORD* pIndices;

	if( !m_IB )
		return 1;

    hr = m_IB->Lock( 0, 4 * sizeof(WORD), (BYTE**)&pIndices, 0 );
    if( FAILED(hr) )
		return 1;
		
	MoveMemory( pIndices, m_Indices, 4 * sizeof(WORD) );

    m_IB->Unlock();

	return 0;

}
int CBgDisp::Copy2VertexBuffer()
{
	if( !m_VB )
		return 1;

    D3DTLVERTEX2* pVertices;
    if( FAILED( m_VB->Lock( 0, 0, (BYTE**)&pVertices, NULL ) ) )
		return 1;
	
	MoveMemory( pVertices, m_tlv, 4 * sizeof( D3DTLVERTEX2 ) );

	m_VB->Unlock();

	return 0;

}

CBgDisp::~CBgDisp()
{
	SAFE_RELEASE( m_VB );
	SAFE_RELEASE( m_IB );
	if( m_texbank ){
		delete m_texbank;
		m_texbank = 0;
	}
}

int CBgDisp::SetBgSize( DWORD dwWidth, DWORD dwHeight )
{
	//int ret;
	float w, h;
	w = (float)dwWidth;
	h = (float)dwHeight;

	m_tlv[0].sx = 0.0f;
	m_tlv[0].sy = 0.0f;
	m_tlv[1].sx = w;
	m_tlv[1].sy = 0.0f;
	m_tlv[2].sx = 0.0f;
	m_tlv[2].sy = h;
	m_tlv[3].sx = w;
	m_tlv[3].sy = h;

	//ret = Copy2VertexBuffer();
	//_ASSERT( !ret );

	return 0;
}

int CBgDisp::SetUVAnime( float srcuanime, float srcvanime )
{
	uanime = srcuanime;
	vanime = srcvanime;
	return 0;
}

int CBgDisp::MoveTexUV()
{
	int i;
	D3DTLVERTEX2* tlvptr = m_tlv;

	if( (uanime != 0.0f) || (vanime != 0.0f ) ){		
		for( i = 0; i < 4; i++ ){
			tlvptr->tu2 += uanime;
			tlvptr->tv2 += vanime;
			tlvptr++;
		}
	}

	//ret = Copy2VertexBuffer();
	//_ASSERT( !ret );

	return 0;
}

int CBgDisp::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hres;
	int ret = 0;

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	pd3dDevice->SetRenderState( D3DRS_WRAP0, 0 );
	pd3dDevice->SetRenderState( D3DRS_WRAP1, 0 );



	LPDIRECT3DTEXTURE9 tex0 = m_texbank->GetTexData( texname, 0 );
	LPDIRECT3DTEXTURE9 tex1 = m_texbank->GetTexData( texname2, 0 );

	if( g_curtex0 != tex0 ){
		pd3dDevice->SetTexture( 0, tex0 );
		g_curtex0 = tex0;
	}
	if( g_curtex1 != tex1 ){
		pd3dDevice->SetTexture( 1, tex1 );
		g_curtex1 = tex1;
	}

    pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	
    /***
	//m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SUBTRACT );
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_PREMODULATE );
	***/

    pd3dDevice->SetVertexShader( D3DFVF_TLVERTEX2 );
    pd3dDevice->SetStreamSource( 0, m_VB, sizeof(D3DTLVERTEX2) );
    pd3dDevice->SetIndices( m_IB, 0L );

	hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP,
									0,
									4,
									0L, //m_dispIndices, 
									2
									);

	if( FAILED( hres ) ){
		DbgOut( "Render error !!! bgdisp\n" );
		ret = 1;
	}

	if( g_curtex1 != NULL ){
		pd3dDevice->SetTexture( 1, NULL );
		g_curtex1 = NULL;
	}
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );


	return ret;

}

int CBgDisp::UpdateVertexBuffer()
{
	int ret;

	ret = Copy2VertexBuffer();
	_ASSERT( !ret );
	return ret;
}