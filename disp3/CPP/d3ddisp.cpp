#include <stdafx.h> //ダミー


#include <windows.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>


#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


#include <mothandler.h>
#include <motionctrl.h>

#include <d3ddisp.h>

#include <polymesh.h>
#include <polymesh2.h>
#include <billboard.h>
#include <ExtLine.h>

//#include <bezmesh.h>
//#include <planes.h>
#include <math.h>

#include <shdhandler.h>

#include <lightdata.h>

#include <mcache.h>
#include <pickdata.h>
#include <cpuid.h>

#include <asmfunc.h>
#include <InfScope.h>

#include <Toon1Params.h>
#include <mqomaterial.h>
#include <texbank.h>

#include <morph.h>
#include <MMotElem.h>

//#include "c:\DXSDK\samples\multimedia\common\include\dxutil.h"
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }




// static member の　init
int CD3DDisp::s_bonebmpsizex = 0;
int CD3DDisp::s_bonebmpsizey = 0;

DWORD CD3DDisp::totalPrim = 0;
DWORD CD3DDisp::invalidPrim = 0;
DWORD CD3DDisp::pracPrim = 0;

float CD3DDisp::s_minx = 1e6;
float CD3DDisp::s_maxx = -1e6;
float CD3DDisp::s_miny = 1e6;
float CD3DDisp::s_maxy = -1e6;
float CD3DDisp::s_minz = 1e6;
float CD3DDisp::s_maxz = -1e6;

float CD3DDisp::s_tbsminx = 0.0f;
float CD3DDisp::s_tbsmaxx = 0.0f;
float CD3DDisp::s_tbsminy = 0.0f;
float CD3DDisp::s_tbsmaxy = 0.0f;
float CD3DDisp::s_tbsminz = 0.0f;
float CD3DDisp::s_tbsmaxz = 0.0f;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/// global member
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
CLightData* g_lighthead = 0;
LPDIRECT3DTEXTURE9 g_curtex0 = (LPDIRECT3DTEXTURE9)( -1 );
LPDIRECT3DTEXTURE9 g_curtex1 = (LPDIRECT3DTEXTURE9)( -1 );
DWORD g_cop0 = 0xFFFFFFFF;
DWORD g_cop1 = 0xFFFFFFFF;
DWORD g_aop0 = 0xFFFFFFFF;
DWORD g_aop1 = 0xFFFFFFFF;


//////////////////////////////
// extern
//////////////////////////////
extern int g_numrts;
extern CCpuid g_cpuinfo;
extern int g_useGPU;
extern int g_usePS;
extern int g_chkVS;
extern int g_bumpUse;
extern ID3DXEffect*	g_pEffect;//main.cpp

extern float g_proj_far;
extern float g_fogstart;
extern float g_fogend;

extern int g_index32bit;

extern int MAXSKINMATRIX;//!!!!!!!!!
extern int MAXBLURMATRIX;

extern DWORD g_renderstate[ D3DRS_BLENDOP + 1 ];

extern CTexBank* g_texbnk;

extern DWORD g_minfilter;
extern DWORD g_magfilter;

//extern int g_LightOfBump[3];
extern int g_lightnum;
extern int g_bumplightnum;

	//shader
extern D3DXHANDLE g_hRenderSceneBone4;
extern D3DXHANDLE g_hRenderSceneBone1;
extern D3DXHANDLE g_hRenderSceneBone0;
extern D3DXHANDLE g_hRenderSceneBone4Toon;
extern D3DXHANDLE g_hRenderSceneBone1Toon;
extern D3DXHANDLE g_hRenderSceneBone0Toon;
extern D3DXHANDLE g_hRenderSceneBone4ToonNoTex;
extern D3DXHANDLE g_hRenderSceneBone1ToonNoTex;
extern D3DXHANDLE g_hRenderSceneBone0ToonNoTex;
extern D3DXHANDLE g_hRenderSceneBone4Toon1;
extern D3DXHANDLE g_hRenderSceneBone1Toon1;
extern D3DXHANDLE g_hRenderSceneBone0Toon1;
extern D3DXHANDLE g_hRenderSceneBone4Bump;//vs2
extern D3DXHANDLE g_hRenderSceneBone1Bump;//vs2
extern D3DXHANDLE g_hRenderSceneBone0Bump;//vs2
extern D3DXHANDLE g_hRenderSceneBone4Edge0;
extern D3DXHANDLE g_hRenderSceneBone1Edge0;
extern D3DXHANDLE g_hRenderSceneBone0Edge0;
extern D3DXHANDLE g_hRenderSceneNormalShadow;//vs2
extern D3DXHANDLE g_hRenderSceneBumpShadow;//vs2
extern D3DXHANDLE g_hRenderSceneToon1Shadow;//vs2
extern D3DXHANDLE g_hRenderSceneToon0Shadow;//vs2
extern D3DXHANDLE g_hRenderSceneBone4PP;
extern D3DXHANDLE g_hRenderSceneBone0PP;
extern D3DXHANDLE g_hRenderScenePPShadow;
extern D3DXHANDLE g_hRenderZN_2path_NormalBone;
extern D3DXHANDLE g_hRenderZN_2path_NormalNoBone;


extern D3DXHANDLE g_hmWorldMatrixArray;
extern D3DXHANDLE g_hmView;
extern D3DXHANDLE g_hmoldView;
extern D3DXHANDLE g_hmProj;
extern D3DXHANDLE g_hmEyePos;
extern D3DXHANDLE g_hmLightParams;
extern D3DXHANDLE g_hmBumpLight;//vs2
extern D3DXHANDLE g_hmFogParams;
extern D3DXHANDLE g_hmToonParams;
extern D3DXHANDLE g_hmLightNum;
extern D3DXHANDLE g_hmEdgeCol0;
extern D3DXHANDLE g_hmAlpha;
extern D3DXHANDLE g_hNormalMap;//vs2
extern D3DXHANDLE g_hDecaleTex;//vs2
extern D3DXHANDLE g_hDecaleTex1;//vs2
extern D3DXHANDLE g_hmMinAlpha;
extern D3DXHANDLE g_hmGlowMult;
extern D3DXHANDLE g_hmToonDiffuse;
extern D3DXHANDLE g_hmToonAmbient;
extern D3DXHANDLE g_hmToonSpecular;

////////////

CD3DDisp::CD3DDisp()
{
	InitParams();
}
CD3DDisp::~CD3DDisp()
{
	DestroyObjs();
}

void CD3DDisp::InitParams()
{
	m_pdev = 0;
	m_clockwise = 0;
	
	m_lv = 0;

	m_orgNormal = 0;
	m_revNormal = 0;

	m_faceNormal = 0;

    m_numTLV = 0;

    m_dispIndices = 0;
	m_revIndices = 0;
    m_numIndices = 0;

	m_skinv = 0;
	m_revskinv = 0;
	m_smaterial = 0;

	m_skintan = 0;
	m_revskintan = 0;

	m_faceBinormal = 0;
	m_revfaceBinormal = 0;
	m_Binormal = 0;
	m_revBinormal = 0;

	mskinblocknum = 0;
	msaveskinblocknum = 0;
	mskinblock = 0;
	mrenderblocknum = 0;
	mrenderblock = 0;
	mbonemat2skinmat = 0;
	mskinmat2bonemat = 0;
	mskinmatnum = 0;
	mskinmat = 0;

	m_blurtime = 0;
	m_blurpos = 0;
	m_blurmat = 0;


	m_IB = 0;
	m_revIB = 0;
	m_numPrim = 0;

    m_VB = 0;
	m_ArrayVB = 0;
	m_revArrayVB = 0;

	m_VBtan = 0;
	m_revVBtan = 0;
	m_VBmaterial = 0;

	m_unum = 0;
	m_vnum = 0;

	m_IEptr = 0;


	m_dbgfindcnt = 0;


	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixIdentity( &m_matView );
	D3DXMatrixIdentity( &m_matProj );
	D3DXMatrixIdentity( &m_oldView );
	m_blurViewMat = 0;

	m_worldv = 0;
	m_scv = 0;
	m_bs = 0;
	m_pc = 0;

	D3DXMatrixIdentity( &m_scalemat );

	m_TLmode = TLMODE_ORG;
	m_linekind = 0;

	m_savelightflag = 0;

	m_dwClipWidth = 0;
	m_dwClipHeight = 0;

	m_pskindecl_tima = 0;
	m_pskindecl_mema = 0;
	//m_plinedecl = 0;
	m_pskintandecl = 0;


	m_EyePt.x = 0.0f;
	m_EyePt.y = 0.0f;
	m_EyePt.z = 0.0f;

	m_umove = 0.0f;
	m_vmove = 0.0f;

	m_tempdiffuse = 0;

	m_alpha = 1.0f;
	m_blurmode = BLUR_NONE;
}

int CD3DDisp::DestroySkinMat()
{
	int releasenum;
	if( msaveskinblocknum != 0 ){
		releasenum = msaveskinblocknum;//影響度を編集すると、mskinblocknumが変化することがある。変化前のnumでreleaseしなければならない。
	}else{
		releasenum = mskinblocknum;
	}

	if( mbonemat2skinmat ){
		int blno;
		//for( blno = 0; blno < mskinblocknum; blno++ ){
		for( blno = 0; blno < releasenum; blno++ ){
			int* delb2s;
			int* dels2b;
			D3DXMATRIX* delmat;


			delb2s = *(mbonemat2skinmat + blno);
			dels2b = *(mskinmat2bonemat + blno);
			delmat = *(mskinmat + blno);


			if( delb2s ){
				free( delb2s );
			}
			if( dels2b ){
				free( dels2b );
			}
			if( delmat ){
				delete [] delmat;
			}
		}
	}

	if( m_blurmat ){
		int blno;
		for( blno = 0; blno < releasenum; blno++ ){
			D3DXMATRIX* delmat;
			delmat = *(m_blurmat + blno);
			if( delmat ){
				delete [] delmat;
			}
		}

		free( m_blurmat );
		m_blurmat = 0;

		//m_blurtime = 0;<-- ここで初期化してはいけない
		m_blurpos = 0;
	}

	if( m_blurViewMat ){
		delete [] m_blurViewMat;
		m_blurViewMat = 0;
	}


	if( mbonemat2skinmat ){
		free( mbonemat2skinmat );
		mbonemat2skinmat = 0;
	}
	if( mskinmat2bonemat ){
		free( mskinmat2bonemat );
		mskinmat2bonemat = 0;
	}
	if( mskinmatnum ){
		free( mskinmatnum );
		mskinmatnum = 0;
	}
	if( mskinmat ){
		free( mskinmat );
		mskinmat = 0;
	}
	if( mskinblock ){
		free( mskinblock );
		mskinblock = 0;
	}
	mskinblocknum = 0;

	if( mrenderblock ){
		free( mrenderblock );
		mrenderblock = 0;
	}
	mrenderblocknum = 0;

	return 0;
}
void CD3DDisp::DestroyObjs()
{
	if( m_lv ){
		delete [] m_lv;
		m_lv = 0;
	}

	if( m_orgNormal ){
		delete [] m_orgNormal;
		m_orgNormal = 0;
	}
	if( m_revNormal ){
		delete [] m_revNormal;
		m_revNormal = 0;
	}

	if( m_faceNormal ){
		delete [] m_faceNormal;
		m_faceNormal = 0;
	}


	if( m_dispIndices ){
		delete [] m_dispIndices;
		m_dispIndices = 0;
	}
	if( m_revIndices ){
		delete [] m_revIndices;
		m_revIndices = 0;
	}

	if( m_worldv ){
		delete [] m_worldv;
		m_worldv = 0;
	}
	
	if( m_scv ){
		delete [] m_scv;
		m_scv = 0;
	}


	if( m_bs ){
		delete [] m_bs;
		m_bs = 0;
	}

	if( m_pc ){
		delete [] m_pc;
		m_pc = 0;
	}

	if( m_skinv ){
		free( m_skinv );
		m_skinv = 0;
	}
	if( m_revskinv ){
		free( m_revskinv );
		m_revskinv = 0;
	}
	if( m_smaterial ){
		free( m_smaterial );
		m_smaterial = 0;
	}
	
	if( m_pskindecl_tima ){
		m_pskindecl_tima->Release();
		m_pskindecl_tima = 0;
	}
	if( m_pskindecl_mema ){
		m_pskindecl_mema->Release();
		m_pskindecl_mema = 0;
	}
	if( m_pskintandecl ){
		m_pskintandecl->Release();
		m_pskintandecl = 0;
	}


	if( m_tempdiffuse ){
		delete [] m_tempdiffuse;
		m_tempdiffuse = 0;
	}

////////
	if( m_faceBinormal ){
		delete [] m_faceBinormal;
		m_faceBinormal = 0;
	}
	if( m_revfaceBinormal ){
		delete [] m_revfaceBinormal;
		m_revfaceBinormal = 0;
	}
	if( m_Binormal ){
		delete [] m_Binormal;
		m_Binormal = 0;
	}
	if( m_revBinormal ){
		delete [] m_revBinormal;
		m_revBinormal = 0;
	}
	if( m_skintan ){
		free( m_skintan );
		m_skintan = 0;
	}
	if( m_revskintan ){
		free( m_revskintan );
		m_revskintan = 0;
	}


    SAFE_RELEASE( m_VB );

	SAFE_RELEASE( m_IB );
	SAFE_RELEASE( m_revIB );

	SAFE_RELEASE( m_VBtan );
	SAFE_RELEASE( m_revVBtan );

	SAFE_RELEASE( m_VBmaterial );

	DestroyArrayVB();//mskinblocknumを使用するので、DestroySkinMatより前。
	DestroySkinMat();


	//if( m_BE ){
	//	delete [] m_BE;
	//	m_BE = 0;
	//}

	m_IEptr = 0;

}

int CD3DDisp::DestroyArrayVB()
{

	int blno;
	int releasenum;

	if( msaveskinblocknum != 0 ){
		releasenum = msaveskinblocknum;//影響度を編集すると、mskinblocknumが変化することがある。変化前のnumでreleaseしなければならない。
	}else{
		releasenum = mskinblocknum;
	}

//DbgOut( "d3ddisp : start DestroyArrayVB %d, %d, %d\n", releasenum, mskinblocknum, msaveskinblocknum );


	for( blno = 0; blno < releasenum; blno++ ){
		if( m_ArrayVB ){
			LPDIRECT3DVERTEXBUFFER9 delvb;
			delvb = *(m_ArrayVB + blno);
			if( delvb ){
				delvb->Release();
			}
		}

		if( m_revArrayVB ){
			LPDIRECT3DVERTEXBUFFER9 delrevvb;
			delrevvb = *(m_revArrayVB + blno);
			if( delrevvb ){
				delrevvb->Release();
			}
		}
	}

	if( m_ArrayVB ){
		free( m_ArrayVB );
		m_ArrayVB = 0;
	}
	if( m_revArrayVB ){
		free( m_revArrayVB );
		m_revArrayVB = 0;
	}

//DbgOut( "d3ddisp : end DestroyArrayVB\n" );


	return 0;
}

int CD3DDisp::Create3DBuffersLine( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !pd3dDevice || !m_linekind ){
		DbgOut( "d3ddisp : Create3DBuffers : skip !!!\n" );
		return 0;//!!!!!!!!!
	}


	HRESULT hr;

	UINT elemleng;
	DWORD curFVF;
	
	elemleng = sizeof( D3DLVERTEX );
	curFVF = D3DFVF_LVERTEX;

    hr = pd3dDevice->CreateVertexBuffer( m_numTLV * elemleng,
		D3DUSAGE_WRITEONLY, curFVF,
		D3DPOOL_MANAGED,
		//D3DPOOL_DEFAULT,
		&m_VB, NULL );
	if( FAILED(hr) ){
		DbgOut( "d3ddisp : Create3DBuffers : CreateVertexBuffer error res %x, num %d, vb %x, curFVF %d, elemleng %d!!!\n", hr, m_numTLV, m_VB, curFVF, elemleng );
		DbgOut( "invalidcall %x, outofvideomem %x, outofmem %x\r\n", D3DERR_INVALIDCALL, D3DERR_OUTOFVIDEOMEMORY, E_OUTOFMEMORY ); 
		_ASSERT( 0 );
		return 1;
	}

	if( g_index32bit ){
		hr = pd3dDevice->CreateIndexBuffer( m_numIndices * sizeof(int),
			0,
			D3DFMT_INDEX32, 
			D3DPOOL_MANAGED,
			&m_IB, NULL );
	}else{
		hr = pd3dDevice->CreateIndexBuffer( m_numIndices * sizeof(WORD),
			0,
			D3DFMT_INDEX16, 
			D3DPOOL_MANAGED,
			&m_IB, NULL );
	}
	if( FAILED(hr) ){
		DbgOut( "d3ddisp : Create3DBuffers : CreateIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CD3DDisp::Create3DBuffersSkin()
{
//DbgOut( "d3ddisp : start Create3DBuffersSkin\n" );

	if( !m_pdev ){
		DbgOut( "d3ddisp : Create3DBuffersSkin : pdev NULL skip !!!\n" );
		return 0;//!!!!!!!!!
	}

	DestroyArrayVB();//!!!!!!!!!!
	if( m_IB ){
		m_IB->Release();
		m_IB = 0;
	}
	if( m_revIB ){
		m_revIB->Release();
		m_revIB = 0;
	}
	if( m_VBtan ){
		m_VBtan->Release();
		m_VBtan = 0;
	}
	if( m_revVBtan ){
		m_revVBtan->Release();
		m_revVBtan = 0;
	}
	if( m_VBmaterial ){
		m_VBmaterial->Release();
		m_VBmaterial = 0;
	}

	HRESULT hr;

	UINT elemleng;
	DWORD curFVF;

	elemleng = sizeof( SKINVERTEX );
	curFVF = 0;

	m_ArrayVB = (LPDIRECT3DVERTEXBUFFER9*)malloc( sizeof( LPDIRECT3DVERTEXBUFFER9 ) * mskinblocknum );
	if( !m_ArrayVB ){
		DbgOut( "d3ddisp : Create3DBuffersSkin : arrayVB alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_revskinv ){
		m_revArrayVB = (LPDIRECT3DVERTEXBUFFER9*)malloc( sizeof( LPDIRECT3DVERTEXBUFFER9 ) * mskinblocknum );
		if( !m_revArrayVB ){
			DbgOut( "d3ddisp : Create3DBuffersSkin : revarrayVB alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb;
		hr = m_pdev->CreateVertexBuffer( m_numTLV * elemleng,
			D3DUSAGE_WRITEONLY, curFVF,
			D3DPOOL_MANAGED,
			//D3DPOOL_DEFAULT,
			&curvb, NULL );
		if( FAILED(hr) ){
			DbgOut( "d3ddisp : Create3DBuffersSkin : CreateVertexBuffer error res %x, num %d, vb %x, curFVF %d, elemleng %d!!!\n", hr, m_numTLV, curvb, curFVF, elemleng );
			DbgOut( "invalidcall %x, outofvideomem %x, outofmem %x\r\n", D3DERR_INVALIDCALL, D3DERR_OUTOFVIDEOMEMORY, E_OUTOFMEMORY ); 
			_ASSERT( 0 );
			return 1;
		}
		*(m_ArrayVB + blno) = curvb;

		if( m_revArrayVB ){
			LPDIRECT3DVERTEXBUFFER9 currevvb;
			hr = m_pdev->CreateVertexBuffer( m_numTLV * elemleng,
				D3DUSAGE_WRITEONLY, curFVF,
				D3DPOOL_MANAGED,
				//D3DPOOL_DEFAULT,
				&currevvb, NULL );
			if( FAILED(hr) ){
				DbgOut( "d3ddisp : Create3DBuffersSkin : CreateVertexBuffer rev error res %x, num %d, vb %x, curFVF %d, elemleng %d!!!\n", hr, m_numTLV, currevvb, curFVF, elemleng );
				DbgOut( "invalidcall %x, outofvideomem %x, outofmem %x\r\n", D3DERR_INVALIDCALL, D3DERR_OUTOFVIDEOMEMORY, E_OUTOFMEMORY ); 
				_ASSERT( 0 );
				return 1;
			}
			*(m_revArrayVB + blno) = currevvb;
		}

	}

	hr = m_pdev->CreateVertexBuffer( m_numTLV * sizeof( SKINTANGENT ),
		D3DUSAGE_WRITEONLY, curFVF,
		D3DPOOL_MANAGED,
		//D3DPOOL_DEFAULT,
		&m_VBtan, NULL );
	if( FAILED(hr) ){
		DbgOut( "d3ddisp : Create3DBuffersSkin : CreateVertexBuffer error vbtan !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( m_revskintan ){
		hr = m_pdev->CreateVertexBuffer( m_numTLV * sizeof( SKINTANGENT ),
			D3DUSAGE_WRITEONLY, curFVF,
			D3DPOOL_MANAGED,
			//D3DPOOL_DEFAULT,
			&m_revVBtan, NULL );
		if( FAILED(hr) ){
			DbgOut( "d3ddisp : Create3DBuffersSkin : CreateVertexBuffer error revvbtan !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	hr = m_pdev->CreateVertexBuffer( m_numTLV * sizeof( SKINMATERIAL ),
		D3DUSAGE_WRITEONLY, curFVF,
		D3DPOOL_MANAGED,
		//D3DPOOL_DEFAULT,
		&m_VBmaterial, NULL );
	if( FAILED(hr) ){
		DbgOut( "d3ddisp : Create3DBuffersSkin : CreateVertexBuffer error vbmaterial !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_index32bit ){
		hr = m_pdev->CreateIndexBuffer( m_numIndices * sizeof(int),
			0,
			D3DFMT_INDEX32, 
			D3DPOOL_MANAGED,
			&m_IB, NULL );
		if( FAILED(hr) ){
			DbgOut( "d3ddisp : Create3DBuffersSkin : CreateIndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hr = m_pdev->CreateIndexBuffer( m_numIndices * sizeof(int),
			0,
			D3DFMT_INDEX32, 
			D3DPOOL_MANAGED,
			&m_revIB, NULL );
		if( FAILED(hr) ){
			DbgOut( "d3ddisp : Create3DBuffers : CreateIndexBuffer rev error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		hr = m_pdev->CreateIndexBuffer( m_numIndices * sizeof(WORD),
			0,
			D3DFMT_INDEX16, 
			D3DPOOL_MANAGED,
			&m_IB, NULL );
		if( FAILED(hr) ){
			DbgOut( "d3ddisp : Create3DBuffersSkin : CreateIndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hr = m_pdev->CreateIndexBuffer( m_numIndices * sizeof(WORD),
			0,
			D3DFMT_INDEX16, 
			D3DPOOL_MANAGED,
			&m_revIB, NULL );
		if( FAILED(hr) ){
			DbgOut( "d3ddisp : Create3DBuffers : CreateIndexBuffer rev error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	msaveskinblocknum = mskinblocknum;//!!!!!!!!!!!!!!!!!! release用に保存。

//DbgOut( "d3ddisp : end Create3DBuffersSkin %d\n", msaveskinblocknum );

	return 0;
}

int CD3DDisp::Copy2IndexBufferNotCulling()
{

	if( !m_IB ){
		DbgOut( "d3ddisp : Copy2IndexBuffer : m_IB not exist skip !!!\n" );
		return 0;///!!!!!!!
	}


	HRESULT hr;
	if( g_index32bit ){
		int* pIndices;
		if( m_dispIndices ){
			if( !m_IB )
				return 1;
			hr = m_IB->Lock( 0, m_numIndices * sizeof(int), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}
			MoveMemory( pIndices, m_dispIndices, m_numIndices * sizeof(int) );	
			m_IB->Unlock();
		}

		if( m_revIndices ){
			if( !m_revIB ){
				_ASSERT( 0 );
				return 1;
			}
			hr = m_revIB->Lock( 0, m_numIndices * sizeof(int), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}	
			MoveMemory( pIndices, m_revIndices, m_numIndices * sizeof(int) );
			m_revIB->Unlock();
		}
	}else{
		WORD* pIndices;
		if( m_dispIndices ){
			if( !m_IB )
				return 1;
			hr = m_IB->Lock( 0, m_numIndices * sizeof(WORD), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}
			DWORD i;
			for( i = 0; i < m_numIndices; i++ ){
				*( pIndices + i ) = (WORD)(*(m_dispIndices + i));
			}
			m_IB->Unlock();
		}

		if( m_revIndices ){
			if( !m_revIB ){
				_ASSERT( 0 );
				return 1;
			}
			hr = m_revIB->Lock( 0, m_numIndices * sizeof(WORD), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}
			DWORD i;
			for( i = 0; i < m_numIndices; i++ ){
				*( pIndices + i ) = (WORD)(*(m_revIndices + i));
			}
			m_revIB->Unlock();
		}
	}
	return 0;
}


int CD3DDisp::Copy2IndexBuffer( int framecnt )
{
	if( !m_IB ){
		DbgOut( "d3ddisp : Copy2IndexBuffer : m_IB not exist skip !!!\n" );
		return 0;///!!!!!!!
	}


	HRESULT hr;
	if( g_index32bit ){
		int* pIndices;
		if( m_dispIndices ){
			if( !m_IB )
				return 1;
			hr = m_IB->Lock( 0, m_numIndices * sizeof(int), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}
			MoveMemory( pIndices, m_dispIndices, m_numIndices * sizeof(int) );	
			m_IB->Unlock();
		}
		if( m_revIndices ){
			if( !m_revIB ){
				_ASSERT( 0 );
				return 1;
			}
			hr = m_revIB->Lock( 0, m_numIndices * sizeof(int), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}	
			MoveMemory( pIndices, m_revIndices, m_numIndices * sizeof(int) );
			m_revIB->Unlock();
		}
	}else{
		WORD* pIndices;
		if( m_dispIndices ){
			if( !m_IB )
				return 1;
			hr = m_IB->Lock( 0, m_numIndices * sizeof(WORD), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}
			DWORD i;
			for( i = 0; i < m_numIndices; i++ ){
				*( pIndices + i ) = (WORD)(*(m_dispIndices + i));
			}
			m_IB->Unlock();
		}
		if( m_revIndices ){
			if( !m_revIB ){
				_ASSERT( 0 );
				return 1;
			}
			hr = m_revIB->Lock( 0, m_numIndices * sizeof(WORD), (void**)&pIndices, 0 );
			if( FAILED(hr) ){
				_ASSERT( 0 );
				return 1;
			}	
			DWORD i;
			for( i = 0; i < m_numIndices; i++ ){
				*( pIndices + i ) = (WORD)(*(m_revIndices + i));
			}
			m_revIB->Unlock();
		}
	}


	return 0;
}
int CD3DDisp::CopyUV2VertexBufferBatch( int* vertnoptr, int setnum )
{
	_ASSERT( m_skinv && m_ArrayVB );


	int blno;
	int vertno;

	for( blno = 0; blno < mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb;
		curvb = *(m_ArrayVB + blno);
		SKINVERTEX* pVertices;
		if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
			_ASSERT( 0 );
			return 1;
		}

		int vindex;
		for( vindex = 0; vindex < setnum; vindex++ ){
			vertno = *( vertnoptr + vindex );
			if( (vertno >= 0) && (vertno < (int)m_numTLV) ){
				SKINVERTEX* curskinv = m_skinv + vertno;
				SKINVERTEX* dstv = pVertices + vertno;

				dstv->tex1[0] = curskinv->tex1[0];
				dstv->tex1[1] = curskinv->tex1[1];
			}
		}
						
		curvb->Unlock();
	}


////////////////////
	if( m_revskinv && m_revArrayVB ){

		for( blno = 0; blno < mskinblocknum; blno++ ){

			LPDIRECT3DVERTEXBUFFER9 curvb;
			curvb = *(m_revArrayVB + blno);
			SKINVERTEX* pVertices;
			if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
				_ASSERT( 0 );
				return 1;
			}

			int vindex;
			for( vindex = 0; vindex < setnum; vindex++ ){
				vertno = *( vertnoptr + vindex );
				if( (vertno >= 0) && (vertno < (int)m_numTLV) ){
					SKINVERTEX* curskinv = m_revskinv + vertno;
					SKINVERTEX* dstv = pVertices + vertno;

					dstv->tex1[0] = curskinv->tex1[0];
					dstv->tex1[1] = curskinv->tex1[1];
				}
			}
			curvb->Unlock();
		}
	}

	return 0;
}


int CD3DDisp::CopyUV2VertexBuffer( int vertno )
{
	_ASSERT( m_skinv && m_ArrayVB );


	int blno;

	if( vertno < 0 ){

		for( blno = 0; blno < mskinblocknum; blno++ ){

			LPDIRECT3DVERTEXBUFFER9 curvb;
			curvb = *(m_ArrayVB + blno);
			SKINVERTEX* pVertices;
			if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
				_ASSERT( 0 );
				return 1;
			}

			//int* curbone2skin = *(mbonemat2skinmat + blno);
			//int* curskin2bone = *(mskinmat2bonemat + blno);
			SKINBLOCK* curskinblock = mskinblock + blno;

			int faceno;
			int index[3];
			for( faceno = curskinblock->startface; faceno < curskinblock->endface; faceno++ ){
				index[0] = *(m_dispIndices + faceno * 3);
				index[1] = *(m_dispIndices + faceno * 3 + 1);
				index[2] = *(m_dispIndices + faceno * 3 + 2);

				int ino;
				for( ino = 0; ino < 3; ino++ ){
					SKINVERTEX* curskinv = m_skinv + index[ino];
					SKINVERTEX* dstv = pVertices + index[ino];

					dstv->tex1[0] = curskinv->tex1[0];
					dstv->tex1[1] = curskinv->tex1[1];
							
				}

			}
			curvb->Unlock();
		}


	////////////////////
		if( m_revskinv && m_revArrayVB ){

			for( blno = 0; blno < mskinblocknum; blno++ ){

				LPDIRECT3DVERTEXBUFFER9 curvb;
				curvb = *(m_revArrayVB + blno);
				SKINVERTEX* pVertices;
				if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
					_ASSERT( 0 );
					return 1;
				}

				//int* curbone2skin = *(mbonemat2skinmat + blno);
				//int* curskin2bone = *(mskinmat2bonemat + blno);
				SKINBLOCK* curskinblock = mskinblock + blno;

				int faceno;
				int index[3];
				for( faceno = curskinblock->startface; faceno < curskinblock->endface; faceno++ ){
					index[0] = *(m_dispIndices + faceno * 3);
					index[1] = *(m_dispIndices + faceno * 3 + 2);// 裏面なので、index反転
					index[2] = *(m_dispIndices + faceno * 3 + 1);

					int ino;
					for( ino = 0; ino < 3; ino++ ){
						SKINVERTEX* curskinv = m_revskinv + index[ino];
						SKINVERTEX* dstv = pVertices + index[ino];

						dstv->tex1[0] = curskinv->tex1[0];
						dstv->tex1[1] = curskinv->tex1[1];
						
					}

				}
				curvb->Unlock();
			}
		}

	}else{

		for( blno = 0; blno < mskinblocknum; blno++ ){

			LPDIRECT3DVERTEXBUFFER9 curvb;
			curvb = *(m_ArrayVB + blno);
			SKINVERTEX* pVertices;
			if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
				_ASSERT( 0 );
				return 1;
			}

			SKINVERTEX* curskinv = m_skinv + vertno;
			SKINVERTEX* dstv = pVertices + vertno;

			dstv->tex1[0] = curskinv->tex1[0];
			dstv->tex1[1] = curskinv->tex1[1];
							
			curvb->Unlock();
		}


	////////////////////
		if( m_revskinv && m_revArrayVB ){

			for( blno = 0; blno < mskinblocknum; blno++ ){

				LPDIRECT3DVERTEXBUFFER9 curvb;
				curvb = *(m_revArrayVB + blno);
				SKINVERTEX* pVertices;
				if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
					_ASSERT( 0 );
					return 1;
				}

				SKINVERTEX* curskinv = m_revskinv + vertno;
				SKINVERTEX* dstv = pVertices + vertno;

				dstv->tex1[0] = curskinv->tex1[0];
				dstv->tex1[1] = curskinv->tex1[1];
						
				curvb->Unlock();
			}
		}

	}

	return 0;
}


int CD3DDisp::Copy2VertexBuffer( int framecnt, int cpmaterial )
{
//DbgOut( "d3ddisp : start Copy2VertexBuffer\n" );

	if( !m_skinv && !m_lv ){
		DbgOut( "d3ddisp : Copy2VertexBuffer : skinv not exist skip !!!\n" );
		return 0;
	}


	if( m_linekind == 0 ){

		int blno;
		for( blno = 0; blno < mskinblocknum; blno++ ){

			LPDIRECT3DVERTEXBUFFER9 curvb;
			curvb = *(m_ArrayVB + blno);
			SKINVERTEX* pVertices;
			if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
				_ASSERT( 0 );
				return 1;
			}

			int* curbone2skin = *(mbonemat2skinmat + blno);
			int* curskin2bone = *(mskinmat2bonemat + blno);
			SKINBLOCK* curskinblock = mskinblock + blno;

			int faceno;
			int index[3];
			for( faceno = curskinblock->startface; faceno < curskinblock->endface; faceno++ ){
				index[0] = *(m_dispIndices + faceno * 3);
				index[1] = *(m_dispIndices + faceno * 3 + 1);
				index[2] = *(m_dispIndices + faceno * 3 + 2);

				int ino;
				for( ino = 0; ino < 3; ino++ ){
					SKINVERTEX* curskinv = m_skinv + index[ino];
					SKINVERTEX* dstv = pVertices + index[ino];

					*dstv = *curskinv;

					CInfElem* curie = m_IEptr + index[ino];
					int ieno;
					int setno = 0;
					for( ieno = 0; ieno < curie->infnum; ieno++ ){
						INFELEM* curIE = curie->ie + ieno;
							
						int setskinmatno;
						setskinmatno = *(curbone2skin + curIE->bonematno);
						if( setskinmatno > 0 ){
							dstv->boneindex[setno] = (float)setskinmatno;
							dstv->weight[setno] = curIE->dispinf;
							setno++;
						}
					}
						
				}

			}
			curvb->Unlock();
		}


////////////////////
		if( m_revskinv && m_revArrayVB ){

			int blno;
			for( blno = 0; blno < mskinblocknum; blno++ ){

				LPDIRECT3DVERTEXBUFFER9 curvb;
				curvb = *(m_revArrayVB + blno);
				SKINVERTEX* pVertices;
				if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
					_ASSERT( 0 );
					return 1;
				}

				int* curbone2skin = *(mbonemat2skinmat + blno);
				int* curskin2bone = *(mskinmat2bonemat + blno);
				SKINBLOCK* curskinblock = mskinblock + blno;

				int faceno;
				int index[3];
				for( faceno = curskinblock->startface; faceno < curskinblock->endface; faceno++ ){
					index[0] = *(m_dispIndices + faceno * 3);
					index[1] = *(m_dispIndices + faceno * 3 + 2);// 裏面なので、index反転
					index[2] = *(m_dispIndices + faceno * 3 + 1);

					int ino;
					for( ino = 0; ino < 3; ino++ ){
						SKINVERTEX* curskinv = m_revskinv + index[ino];
						SKINVERTEX* dstv = pVertices + index[ino];

						*dstv = *curskinv;

						CInfElem* curie = m_IEptr + index[ino];
						int ieno;
						int setno = 0;
						for( ieno = 0; ieno < curie->infnum; ieno++ ){
							INFELEM* curIE = curie->ie + ieno;
								
							int setskinmatno;
							setskinmatno = *(curbone2skin + curIE->bonematno);
							if( setskinmatno > 0 ){
								dstv->boneindex[setno] = (float)setskinmatno;
								dstv->weight[setno] = curIE->dispinf;
								setno++;
							}
						}
							
					}

				}
				curvb->Unlock();
			}
		}


		if( m_smaterial && m_VBmaterial && (cpmaterial != 0) ){
			SKINMATERIAL* pv;
			if( FAILED( m_VBmaterial->Lock( 0, sizeof( SKINMATERIAL ) * m_numTLV, (void**)&pv, 0 ) ) ){
				DbgOut( "d3ddisp : m_VBmaterial lock error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			MoveMemory( pv, m_smaterial, m_numTLV * sizeof( SKINMATERIAL ) );
			m_VBmaterial->Unlock();
		}


	}else{
		D3DLVERTEX* pVertices;
		if( FAILED( m_VB->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
			DbgOut( "d3ddisp : m_VB lock 10  : error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		MoveMemory( pVertices, m_lv, m_numTLV * sizeof( D3DLVERTEX ) );
					
		m_VB->Unlock();
	}

	if( m_skintan && m_VBtan ){
		SKINTANGENT* pv;
		if( FAILED( m_VBtan->Lock( 0, sizeof( SKINTANGENT ) * m_numTLV, (void**)&pv, 0 ) ) ){
			DbgOut( "d3ddisp : m_VBtan lock error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( pv, m_skintan, m_numTLV * sizeof( SKINTANGENT ) );
		m_VBtan->Unlock();
	}

	if( m_revskintan && m_revVBtan ){
		SKINTANGENT* pv;
		if( FAILED( m_revVBtan->Lock( 0, sizeof( SKINTANGENT ) * m_numTLV, (void**)&pv, NULL ) ) ){
			DbgOut( "d3ddisp : m_VBtan lock error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( pv, m_revskintan, m_numTLV * sizeof( SKINTANGENT ) );
		m_revVBtan->Unlock();
	}


//DbgOut( "d3ddisp : end Copy2VertexBuffer\n" );

	return 0;

}

int CD3DDisp::CreateVertexAndIndex( int clockwise )
{	
	if( m_linekind == 0 ){
		m_skinv = (SKINVERTEX*)malloc( sizeof( SKINVERTEX ) * m_numTLV );
		ZeroMemory( m_skinv, sizeof( SKINVERTEX ) * m_numTLV );

		m_skintan = (SKINTANGENT*)malloc( sizeof( SKINTANGENT ) * m_numTLV );
		ZeroMemory( m_skintan, sizeof( SKINTANGENT ) * m_numTLV );

		m_smaterial = (SKINMATERIAL*)malloc( sizeof( SKINMATERIAL ) * m_numTLV );
		ZeroMemory( m_smaterial, sizeof( SKINMATERIAL ) * m_numTLV );

	}else{
		m_lv = new D3DLVERTEX[ m_numTLV ];
	}


	m_dispIndices = new int[ m_numIndices ];
		
	m_orgNormal = new D3DXVECTOR3[ m_numTLV ];

	if( ((!m_skinv || !m_skintan || !m_smaterial) && !m_lv ) || !m_dispIndices || !m_orgNormal ){
		DbgOut( "d3ddisp : CreateVertexAndIndex : alloc error %x %x!!!!\n",
			m_skinv, m_dispIndices );
		_ASSERT( 0 );
		return 1;
	}

	if( (clockwise == 3) && (m_linekind == 0) ){
		m_revskinv = (SKINVERTEX*)malloc( sizeof( SKINVERTEX ) * m_numTLV );
		ZeroMemory( m_revskinv, sizeof( SKINVERTEX ) * m_numTLV );

		m_revskintan = (SKINTANGENT*)malloc( sizeof( SKINTANGENT ) * m_numTLV );
		ZeroMemory( m_revskintan, sizeof( SKINTANGENT ) * m_numTLV );
		
		m_revNormal = new D3DXVECTOR3[ m_numTLV ];

		if( !m_revskinv || !m_revNormal || !m_revskintan ){
			DbgOut( "d3ddisp : CreateVertexAndIndex : alloc error !!!!\n" );
			_ASSERT( 0 );
			return 1;			
		}
	}

	if( m_linekind == 0 ){
		m_revIndices = new int[ m_numIndices ];
		if( !m_revIndices ){
			DbgOut( "d3ddisp : CreateVertexAndIndex : revindices alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CD3DDisp::CreateDispData( LPDIRECT3DDEVICE9 pdev, CPart* part, int tlmode )
{
	m_unum = 1;
	m_vnum = 1;
	m_numTLV = 1;
	m_numIndices = 0;

	m_pdev = pdev;
	m_clockwise = 0;

	m_TLmode = tlmode;

	int ret;
	ret = part->CreateInfElemIfNot( (int)m_numTLV );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispData : part : part CreateInfElemIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_IEptr = part->m_IE;

	ret = SetSkinDecl( pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	m_skinv = (SKINVERTEX*)malloc( sizeof( SKINVERTEX ) * m_numTLV );
	m_dispIndices = new int[ m_numIndices ];
		
	m_worldv = new D3DXVECTOR3[ m_numTLV ];
	m_scv = new D3DXVECTOR3[ m_numTLV ];

	if( !m_skinv || !m_dispIndices || !m_worldv || !m_scv ){
		DbgOut( "CD3DDisp : CreateDispData[ part ] : alloc error %x %x\n", 
			m_skinv, m_dispIndices );
		_ASSERT( 0 );
		return 1;
	}

	CVec3f jloc = part->jointloc;
	SKINVERTEX* dstptr = m_skinv;
	dstptr->pos[0] = jloc.x;
	dstptr->pos[1] = jloc.y;
	dstptr->pos[2] = jloc.z;
	dstptr->tex1[0] = 0.0f;
	dstptr->tex1[1] = 0.0f;

	return 0;
}

int CD3DDisp::CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CExtLine* extline, int tlmode )
{
	m_pdev = pd3dDevice;
	m_clockwise = 0;

	CMeshInfo* mi;
	mi = extline->meshinfo;

	m_TLmode = tlmode;


	m_unum = mi->m;
	m_vnum = mi->n;

	m_linekind = mi->mclosed;//!!!!!!!!!
	DbgOut( "d3ddisp : CreateDispData extline : linekind %d\n", m_linekind );

	m_numTLV = m_vnum;
	m_numIndices = m_vnum;


	int clockwise = 2;

	int ret;
	ret = CreateVertexAndIndex( clockwise );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Create3DBuffersLine( pd3dDevice );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	ret = SetExtLineLV( extline );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispData[ extline ] : SetExtLineLV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = SetExtLineIndices( m_dispIndices, m_numIndices );
	if( ret ){
		DbgOut( "CD3DDisp : CreateDispData[ extline ] : SetExtLineIndices error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = Copy2IndexBuffer( 0 );	
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	if( m_linekind == D3DPT_LINELIST ){
		m_numPrim = m_unum / 2;//!!!!!!!!
	}else if( m_linekind == D3DPT_LINESTRIP ){
		m_numPrim = m_unum - 1;//!!!!!!!!
	}

	totalPrim += m_numPrim;
	pracPrim = totalPrim - invalidPrim;
	DbgOut( "CD3DDisp : CreateDispData : totalPrim %d, invalidPrim %d, pracPrim %d\n", 
		totalPrim, invalidPrim, pracPrim );

	return 0;
}


int CD3DDisp::CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CPolyMesh* polymesh, int clockwise, int srcrepx, int srcrepy, int tlmode )
{
	m_pdev = pd3dDevice;
	m_clockwise = clockwise;

	CMeshInfo* mi;
	mi = polymesh->meshinfo;

	m_TLmode = tlmode;


	int pointnum, facenum;
	
	pointnum = mi->m;
	facenum = mi->n;

	m_unum = 3;
	m_vnum = facenum;

	m_numTLV = pointnum;
	m_numIndices = facenum * 3;
	
//if( polymesh->billboardflag == 1 ){
//	DbgOut( "d3ddisp : CreateDispData : billboard : pointnum %d, facenum %d\r\n", pointnum, facenum );		
//}

//DbgOut( "disp : CreateDisp : 00 \n" );

	int ret;
	ret = CreateVertexAndIndex( clockwise );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "disp : CreateDisp : 01 \n" );
//DbgOut( "disp : CreateDisp : 02 \n" );

	ret = polymesh->CreateInfElemIfNot( (int)m_numTLV );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispData : polymesh : pm CreateInfElemIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_IEptr = polymesh->m_IE;

//DbgOut( "disp : CreateDisp : 03 \n" );

	
//DbgOut( "disp : CreateDisp : 0 \n" );
	ret = SetSkinDecl( pd3dDevice );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispObj pm ; SetSkinDecl error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//DbgOut( "disp : CreateDisp : 1 \n" );

	m_worldv = new D3DXVECTOR3 [ m_numTLV ];
	if( !m_worldv ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh ] : m_worldv alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_worldv, sizeof( D3DXVECTOR3 ) * m_numTLV );


	m_scv = new D3DXVECTOR3 [ m_numTLV ];
	if( !m_worldv ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh ] : m_scv alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_scv, sizeof( D3DXVECTOR3 ) * m_numTLV );

	m_tempdiffuse = new DWORD [ m_numTLV ];
	if( !m_tempdiffuse ){
		DbgOut( "D3DDisp : CraeteDispData[ polymesh ] : m_tempdiffuse alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_tempdiffuse, sizeof( DWORD ) * m_numTLV );


	m_Binormal = new D3DXVECTOR3[ m_numTLV ];
	if( !m_Binormal ){
		DbgOut( "d3ddisp : CreateDispData pm : binormal alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_Binormal, sizeof( D3DXVECTOR3 ) * m_numTLV );

	if( clockwise == 3 ){
		m_revBinormal = new D3DXVECTOR3[ m_numTLV ];
		if( !m_revBinormal ){
			DbgOut( "d3ddisp : CreateDispData pm : revbinormal alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_revBinormal, sizeof( D3DXVECTOR3 ) * m_numTLV );
	}


//DbgOut( "disp : CreateDisp : 2 \n" );

	if( polymesh->groundflag == 1 ){

		m_bs = new CBSphere [ m_vnum ];
		if( !m_bs ){
			DbgOut( "D3DDisp : CreateDispData[ polymesh ] : m_bs alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_pc = new CPCoef [ m_vnum ];
		if( !m_pc ){
			DbgOut( "D3DDisp : CreateDispData[ polymesh ] : m_pc alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}
//DbgOut( "disp : CreateDisp : 3 \n" );


	ret = SetPolyMeshSkinV( polymesh );
	if( ret ){
		DbgOut( "CD3DDisp : CreateDispData[ polymesh ] : SetPolyMeshTLV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "disp : CreateDisp : 4 \n" );

	ret = SetTriUV();
	if( ret ){
		DbgOut( "CD3DDisp : CreateDispData[ polymesh ] : SetTriUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "disp : CreateDisp : 5 \n" );

	if( clockwise != 3 ){
		ret = SetTriIndices( m_dispIndices, clockwise, facenum, polymesh->indexbuf );
		if( ret ){
			DbgOut( "CD3DDisp : CreateDispData[ polymesh ] : SetTriIndices error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( clockwise == 1 ){
			ret = SetTriIndices( m_revIndices, 2, facenum, polymesh->indexbuf );
			if( ret ){
				DbgOut( "CD3DDisp : CreateDispData[ polymesh ] : SetTriIndices 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = SetTriIndices( m_revIndices, 1, facenum, polymesh->indexbuf );
			if( ret ){
				DbgOut( "CD3DDisp : CreateDispData[ polymesh ] : SetTriIndices 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{

		MoveMemory( m_revskinv, m_skinv, sizeof( SKINVERTEX ) * m_numTLV );

		ret = SetTriIndices( m_dispIndices, 1, facenum, polymesh->indexbuf );
		if( ret ){
			DbgOut( "CD3DDisp : CreateDispData[ polymesh ] : SetTriIndices 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = SetTriIndices( m_revIndices, 2, facenum, polymesh->indexbuf );
		if( ret ){
			DbgOut( "CD3DDisp : CreateDispData[ polymesh ] : SetTriIndices 2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	m_numPrim = facenum;

	if( clockwise != 3 )
		totalPrim += m_numPrim;
	else
		totalPrim += m_numPrim * 2;
	pracPrim = totalPrim - invalidPrim;
	DbgOut( "CD3DDisp : CreateDispData : totalPrim %d, invalidPrim %d, pracPrim %d\n", 
		totalPrim, invalidPrim, pracPrim );

	return 0;
}

int CD3DDisp::CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CPolyMesh2* polymesh2, int clockwise, int srcrepx, int srcrepy, int tlmode, float facet )
{
	int ret;

	m_pdev = pd3dDevice;
	m_clockwise = clockwise;

	//int adjustuvflag = polymesh2->groundflag;
	int adjustuvflag = 0;//<---- この変数の使用は止める。polymesh2->adjustuvflagを使う。


	m_TLmode = tlmode;


	ret = polymesh2->CreateOptDataBySamePointBuf( adjustuvflag, facet );
	if( ret ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : CreateOptDataBySamePointBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	CMeshInfo* mi;
	mi = polymesh2->meshinfo;

	int pointnum, facenum;
	
	//pointnum = mi->n * 3;
	pointnum = polymesh2->optpleng;//!!!
	facenum = mi->n;

	m_unum = 3;
	m_vnum = facenum;

	m_numTLV = pointnum;
	m_numIndices = facenum * 3;
	
	ret = CreateVertexAndIndex( clockwise );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispData : pm2 : CreateVertexAndIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = polymesh2->CreateInfElemIfNot( (int)m_numTLV );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispData : polymesh2 : pm2 CreateInfElemIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	m_IEptr = polymesh2->m_IE;



	ret = SetSkinDecl( pd3dDevice );
	_ASSERT( !ret );

	m_worldv = new D3DXVECTOR3 [ m_numTLV ];
	if( !m_worldv ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_worldv alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_worldv, sizeof( D3DXVECTOR3 ) * m_numTLV );


	m_scv = new D3DXVECTOR3 [ m_numTLV ];
	if( !m_worldv ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_scv alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_scv, sizeof( D3DXVECTOR3 ) * m_numTLV );

	m_tempdiffuse = new DWORD [ m_numTLV ];
	if( !m_tempdiffuse ){
		DbgOut( "D3DDisp : CraeteDispData[ polymesh2 ] : m_tempdiffuse alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_tempdiffuse, sizeof( DWORD ) * m_numTLV );


	m_faceNormal = new D3DXVECTOR3 [ m_vnum ];
	if( !m_faceNormal ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_faceNormal alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_faceNormal, sizeof( D3DXVECTOR3 ) * m_vnum );


	m_faceBinormal = new D3DXVECTOR3[ m_vnum ];
	if( !m_faceBinormal ){
		DbgOut( "d3ddisp : CreateDispData pm2 : faceBinormal alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_faceBinormal, sizeof( D3DXVECTOR3 ) * m_vnum );

	if( clockwise == 3 ){
		m_revfaceBinormal = new D3DXVECTOR3[ m_vnum ];
		if( !m_revfaceBinormal ){
			DbgOut( "d3ddisp : CreateDispData pm2 : revfaceBinormal alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_revfaceBinormal, sizeof( D3DXVECTOR3 ) * m_vnum );
	}


	if( polymesh2->groundflag == 1 ){

		m_bs = new CBSphere [ m_vnum ];
		if( !m_bs ){
			DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_bs alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_pc = new CPCoef [ m_vnum ];
		if( !m_pc ){
			DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_pc alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	ret = SetPolyMesh2SkinV( polymesh2 );
	if( ret ){
		DbgOut( "CD3DDisp : CreateDispData[ polymesh2 ] : SetPolyMesh2TLV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( clockwise != 3 ){
		ret = SetTriIndices( m_dispIndices, clockwise, facenum, polymesh2->optindexbuf );
		if( ret ){
			DbgOut( "CD3DDisp : CreateDispData[ polymesh2 ] : SetTriIndices error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( clockwise == 1 ){
			ret = SetTriIndices( m_revIndices, 2, facenum, polymesh2->optindexbuf );
			if( ret ){
				DbgOut( "CD3DDisp : CreateDispData[ polymesh2 ] : SetTriIndices 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = SetTriIndices( m_revIndices, 1, facenum, polymesh2->optindexbuf );
			if( ret ){
				DbgOut( "CD3DDisp : CreateDispData[ polymesh2 ] : SetTriIndices 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{

		MoveMemory( m_revskinv, m_skinv, sizeof( SKINVERTEX ) * m_numTLV );

		ret = SetTriIndices( m_dispIndices, 1, facenum, polymesh2->optindexbuf );
		if( ret ){
			DbgOut( "CD3DDisp : CreateDispData[ polymesh2 ] : SetTriIndices 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = SetTriIndices( m_revIndices, 2, facenum, polymesh2->optindexbuf );
		if( ret ){
			DbgOut( "CD3DDisp : CreateDispData[ polymesh2 ] : SetTriIndices 2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	m_numPrim = facenum;

	if( clockwise != 3 )
		totalPrim += m_numPrim;
	else
		totalPrim += m_numPrim * 2;
	pracPrim = totalPrim - invalidPrim;
	DbgOut( "CD3DDisp : CreateDispData : totalPrim %d, invalidPrim %d, pracPrim %d\n", 
		totalPrim, invalidPrim, pracPrim );

	return 0;
}


int CD3DDisp::SetSkinDecl( LPDIRECT3DDEVICE9 pdev )
{
	D3DVERTEXELEMENT9 skindecl_tima[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//normal[4]
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

		//weight[4]
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },

		//boneindex[4]
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		//tex1[2]//tex0
		{ 0, 64, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		//diffuse
		{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

		//specular
		{ 1, 4, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },

		//ambient
		{ 1, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },

		//power
		{ 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },

		//emissive
		{ 1, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },

		D3DDECL_END()

	};
	
	D3DVERTEXELEMENT9 skindecl_mema[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//normal[4]
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

		//weight[4]
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },

		//boneindex[4]
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		//tex1[2]//tex0
		{ 0, 64, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		D3DDECL_END()

	};

	D3DVERTEXELEMENT9 skintandecl[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//normal[4]
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

		//weight[4]
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },

		//boneindex[4]
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		//tex1[2]//tex0
		{ 0, 64, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		//diffuse
		{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

		//specular
		{ 1, 4, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },

		//ambient
		{ 1, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },

		//power
		{ 1, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },

		//emissive
		{ 1, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },

	//stream 1
		//tangent
		//{ 1, 92, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		{ 2, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		
		D3DDECL_END()

	};

	if( m_pskindecl_tima ){
		m_pskindecl_tima->Release();
		m_pskindecl_tima = 0;
	}
	if( m_pskindecl_mema ){
		m_pskindecl_mema->Release();
		m_pskindecl_mema = 0;
	}
	if( m_pskintandecl ){
		m_pskintandecl->Release();
		m_pskintandecl = 0;
	}


	HRESULT hr;
	hr = pdev->CreateVertexDeclaration( skindecl_tima, &m_pskindecl_tima );
	if( hr != D3D_OK ){
		DbgOut( "d3ddisp : SetSkinDecl tima : CreateVertexDeclaration error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	hr = pdev->CreateVertexDeclaration( skindecl_mema, &m_pskindecl_mema );
	if( hr != D3D_OK ){
		DbgOut( "d3ddisp : SetSkinDecl mema : CreateVertexDeclaration error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	hr = pdev->CreateVertexDeclaration( skintandecl, &m_pskintandecl );
	if( hr != D3D_OK ){
		DbgOut( "d3ddisp : SetSkinDecl : CreateVertexDeclaration skintan error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CD3DDisp::SetPolyMeshPos( CPolyMesh* polymesh )
{
	SKINVERTEX* curptr = m_skinv;
	VEC3F* srcvec = polymesh->pointbuf;

	DWORD i;
	for( i = 0; i < m_numTLV; i++ ){
		curptr->pos[0] = srcvec->x;
		curptr->pos[1] = srcvec->y;
		curptr->pos[2] = srcvec->z;
		curptr->pos[3] = 1.0f;//!!!!!!!!

		curptr++;
		srcvec++;
	}


	return 0;
}


int CD3DDisp::SetPolyMeshSkinV( CPolyMesh* polymesh )
{
	SKINVERTEX* curptr = m_skinv;
	VEC3F* srcvec = polymesh->pointbuf;

	DWORD i;
	for( i = 0; i < m_numTLV; i++ ){
		curptr->pos[0] = srcvec->x;
		curptr->pos[1] = srcvec->y;
		curptr->pos[2] = srcvec->z;
		curptr->pos[3] = 1.0f;//!!!!!!!!

		curptr->tex1[0] = 0.0f;
		curptr->tex1[1] = 0.0f;
			
		curptr->weight[0] = 0.0f;
		curptr->weight[1] = 0.0f;
		curptr->weight[2] = 0.0f;
		curptr->weight[3] = 0.0f;

		curptr->boneindex[0] = 0.0f;
		curptr->boneindex[1] = 0.0f;
		curptr->boneindex[2] = 0.0f;
		curptr->boneindex[3] = 0.0f;


		curptr++;
		srcvec++;
	}

	return 0;
}

int CD3DDisp::SetPolyMesh2SkinVMorph( CPolyMesh2* polymesh2 )
{
	SKINVERTEX* curptr;
	D3DTLVERTEX* srcptr;

	int optno;
	int oldno;

	for( oldno = 0; oldno < (int)m_numTLV; oldno++ ){
		optno = *( polymesh2->oldpno2optpno + oldno );

		_ASSERT( optno >= 0 );

		curptr = m_skinv + oldno;
		srcptr = polymesh2->opttlv + optno;

		curptr->pos[0] = srcptr->sx;
		curptr->pos[1] = srcptr->sy;
		curptr->pos[2] = srcptr->sz;
		curptr->pos[3] = 1.0f;

		curptr->tex1[0] = srcptr->tu;
		curptr->tex1[1] = srcptr->tv;

		curptr->weight[0] = 0.0f;
		curptr->weight[1] = 0.0f;
		curptr->weight[2] = 0.0f;
		curptr->weight[3] = 0.0f;

		curptr->boneindex[0] = 0.0f;
		curptr->boneindex[1] = 0.0f;
		curptr->boneindex[2] = 0.0f;
		curptr->boneindex[3] = 0.0f;

	}

	return 0;
}


int CD3DDisp::SetPolyMesh2SkinV( CPolyMesh2* polymesh2 )
{
	SKINVERTEX* curptr = m_skinv;
	
	D3DTLVERTEX* srcptr = polymesh2->opttlv;//!!!
	DWORD i;

	for( i = 0; i < m_numTLV; i++ ){
		curptr->pos[0] = srcptr->sx;
		curptr->pos[1] = srcptr->sy;
		curptr->pos[2] = srcptr->sz;
		curptr->pos[3] = 1.0f;

		curptr->tex1[0] = srcptr->tu;
		curptr->tex1[1] = srcptr->tv;

		curptr->weight[0] = 0.0f;
		curptr->weight[1] = 0.0f;
		curptr->weight[2] = 0.0f;
		curptr->weight[3] = 0.0f;

		curptr->boneindex[0] = 0.0f;
		curptr->boneindex[1] = 0.0f;
		curptr->boneindex[2] = 0.0f;
		curptr->boneindex[3] = 0.0f;

		curptr++;
		srcptr++;
	}

	return 0;
}


int CD3DDisp::SetExtLineLV( CExtLine* extline )
{

	D3DLVERTEX* curlv;
	D3DXVECTOR3 curp;
	int setno = 0;
	int curpid = -1;
	int nextpid;
	int ret;

	while( setno < m_unum ){
		ret = extline->GetNextExtPoint( curpid, &nextpid );
		if( ret ){
			DbgOut( "d3ddisp : SetExtLineLV : extline GetNextExtPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( nextpid < 0 ){
			DbgOut( "d3ddisp : SetExtLineLV : nextpid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = extline->GetExtPointPos( nextpid, &curp );
		if( ret ){
			DbgOut( "d3ddisp : SetExtLineLV : extline GetExtPointPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		curlv = m_lv + setno;
		curlv->x = curp.x;
		curlv->y = curp.y;
		curlv->z = curp.z;

		curlv->color = 0xFFFFFFFF;

//DbgOut( "d3ddisp : SetExtLineLV : %d, (%f, %f, %f)\n", setno, curp.x, curp.y, curp.z );

		setno++;
		curpid = nextpid;

	}

	return 0;
}


int CD3DDisp::SetExtLineIndices( int* dstindices, DWORD numindices )
{
	int* dstp;
	DWORD i;

	for( i = 0; i < numindices; i++ ){
		dstp = dstindices + i;
		*dstp = i;
	}

	return 0;
}

int CD3DDisp::SetTriIndices( int* dstindices, int clockwise, int facenum, int* srclist )
{
	int faceno;
	if( (clockwise != 1) && (clockwise != 2) )
		return 1;

	int* dstp = dstindices;
	int* srcp = srclist;

	if( srclist ){
		for( faceno = 0; faceno < facenum; faceno++ ){
			if( clockwise == 2 ){
				*dstp = *srcp;
				*(dstp + 1) = *(srcp + 1);
				*(dstp + 2) = *(srcp + 2);
			}else if( clockwise == 1 ){
				*dstp = *srcp;
				*(dstp + 1) = *(srcp + 2);
				*(dstp + 2) = *(srcp + 1);
			}

			dstp += 3;
			srcp += 3;
		}
	}else{
		// srclist指定が無い場合は、デフォルト順序
		for( faceno = 0; faceno < facenum; faceno++ ){
			if( clockwise == 2 ){
				*dstp = (faceno * 3);
				*(dstp + 1) = (faceno * 3 + 1);
				*(dstp + 2) = (faceno * 3 + 2);
			}else if( clockwise == 1 ){
				*dstp = (faceno * 3);
				*(dstp + 1) = (faceno * 3 + 2);
				*(dstp + 2) = (faceno * 3 + 1);
			}

			dstp += 3;
			//srcp += 3;
		}
	}

	return 0;
}




int CD3DDisp::SetRevVertex( int copymode )// for TLMODE_D3D
{
	
	if( !m_skinv || !m_revskinv ){
		DbgOut( "d3ddisp : SetRevNormal : first error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD vno;

	SKINVERTEX* curv;
	SKINVERTEX* currevv;

	for( vno = 0; vno < m_numTLV; vno++ ){
		curv = m_skinv + vno;
		currevv = m_revskinv + vno;

		if( copymode & COPYVERTEX ){
			currevv->pos[0] = curv->pos[0];
			currevv->pos[1] = curv->pos[1];
			currevv->pos[2] = curv->pos[2];
			currevv->pos[3] = curv->pos[3];
		}

		if( copymode & COPYNORMAL ){
			//反転コピー
			currevv->normal[0] = -curv->normal[0];
			currevv->normal[1] = -curv->normal[1];
			currevv->normal[2] = -curv->normal[2];
			currevv->normal[3] = curv->normal[3];
		}

		if( copymode & COPYUV ){
			currevv->tex1[0] = curv->tex1[0];
			currevv->tex1[1] = curv->tex1[1];
		}
			
		if( copymode & COPYBLEND ){
			currevv->weight[0] = curv->weight[0];
			currevv->weight[1] = curv->weight[1];
			currevv->weight[2] = curv->weight[2];
			currevv->weight[3] = curv->weight[3];
		
			currevv->boneindex[0] = curv->boneindex[0];
			currevv->boneindex[1] = curv->boneindex[1];
			currevv->boneindex[2] = curv->boneindex[2];
			currevv->boneindex[3] = curv->boneindex[3];
		}
	}
	return 0;
}

int CD3DDisp::SetSkinNormalPM()
{
	char* pmcalcflag;

	int faceno;
	int pno0, pno1, pno2;
	SKINVERTEX* v0;
	SKINVERTEX* v1;
	SKINVERTEX* v2;
	D3DXVECTOR3 nvec;

	pmcalcflag = (char*)malloc( sizeof( char ) * m_numTLV );
	if( !pmcalcflag ){
		DbgOut( "d3ddisp : SetVertNormal : pmcalcflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( pmcalcflag, sizeof( char ) * m_numTLV );


	DWORD vno;
	SKINVERTEX* curv;
	for( vno = 0; vno < m_numTLV; vno++ ){
		curv = m_skinv + vno;
		curv->normal[0] = 0.0f;
		curv->normal[1] = 0.0f;
		curv->normal[2] = 0.0f;
		curv->normal[3] = 0.0f;
	}
	ZeroMemory( m_skintan, sizeof( SKINTANGENT ) * m_numTLV );
	ZeroMemory( m_Binormal, sizeof( D3DXVECTOR3 ) * m_numTLV );
	if( m_revBinormal ){
		ZeroMemory( m_revBinormal, sizeof( D3DXVECTOR3 ) * m_numTLV );
	}

	for( faceno = 0; faceno < m_vnum; faceno++ ){

		//normalの計算
		pno0 = *(m_dispIndices + faceno * 3);
		pno1 = *(m_dispIndices + faceno * 3 + 1);
		pno2 = *(m_dispIndices + faceno * 3 + 2);

		v0 = m_skinv + pno0;
		v1 = m_skinv + pno1;
		v2 = m_skinv + pno2;

		int hassame;
		if( (v0->pos[0] == v1->pos[0]) && (v0->pos[1] == v1->pos[1]) && (v0->pos[2] == v1->pos[2]) ){
			hassame = 1;
		}else if( (v0->pos[0] == v2->pos[0]) && (v0->pos[1] == v2->pos[1]) && (v0->pos[2] == v2->pos[2]) ){
			hassame = 1;
		}else if( (v1->pos[0] == v2->pos[0]) && (v1->pos[1] == v2->pos[1]) && (v1->pos[2] == v2->pos[2]) ){
			hassame = 1;
		}else{
			hassame = 0;
		}

		if( hassame == 1 )
			continue;

		int ret;
		ret = CalcNormal( &nvec, v0, v1, v2 );
		if( ret ){
			DbgOut( "CD3DDisp : SetVertNormalPM : CalcNormal error !!!\n" );
			return 1;
		}
		
		v0->normal[0] += nvec.x;
		v0->normal[1] += nvec.y;
		v0->normal[2] += nvec.z;

		v1->normal[0] += nvec.x;
		v1->normal[1] += nvec.y;
		v1->normal[2] += nvec.z;

		v2->normal[0] += nvec.x;
		v2->normal[1] += nvec.y;
		v2->normal[2] += nvec.z;


		D3DXVECTOR3 bn;
		D3DXVECTOR3 pos0, pos1, pos2;
		pos0.x = v0->pos[0]; pos0.y = v0->pos[1]; pos0.z = v0->pos[2];
		pos1.x = v1->pos[0]; pos1.y = v1->pos[1]; pos1.z = v1->pos[2];
		pos2.x = v2->pos[0]; pos2.y = v2->pos[1]; pos2.z = v2->pos[2];

		bn = -( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 ) + ( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 );
		//bn = -( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 ) + ( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 );
		D3DXVec3Normalize( &bn, &bn );

		*( m_Binormal + pno0 ) += bn;
		*( m_Binormal + pno1 ) += bn;
		*( m_Binormal + pno2 ) += bn;

		if( m_revBinormal ){
			D3DXVECTOR3 revbn;
			revbn = -( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 ) + ( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 );
			//revbn = -( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 ) + ( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 );
			D3DXVec3Normalize( &revbn, &revbn );

			*( m_revBinormal + pno0 ) += revbn;
			*( m_revBinormal + pno1 ) += revbn;
			*( m_revBinormal + pno2 ) += revbn;
		}

		*(pmcalcflag + pno0) += 1;
		*(pmcalcflag + pno1) += 1;
		*(pmcalcflag + pno2) += 1;
	}


// 複数回足した法線を、正規化する。
	for( vno = 0; vno < m_numTLV; vno++ ){
		int flag;
		flag = *(pmcalcflag + vno);
		if( flag >= 2 ){
			D3DXVECTOR3 curn;
			curv = m_skinv + vno;
			curn.x = curv->normal[0];
			curn.y = curv->normal[1];
			curn.z = curv->normal[2];

			D3DXVec3Normalize( &curn, &curn );
		
			curv->normal[0] = curn.x;
			curv->normal[1] = curn.y;
			curv->normal[2] = curn.z;

			D3DXVec3Normalize( m_Binormal + vno, m_Binormal + vno );
			if( m_revBinormal ){
				D3DXVec3Normalize( m_revBinormal + vno, m_revBinormal + vno );
			}
		}
	}

	if( pmcalcflag )
		free( pmcalcflag );

	///////////
	
	for( vno = 0; vno < m_numTLV; vno++ ){
		curv = m_skinv + vno;

		(m_orgNormal + vno)->x = curv->normal[0];
		(m_orgNormal + vno)->y = curv->normal[1];
		(m_orgNormal + vno)->z = curv->normal[2];

		if( m_revNormal ){
			*( m_revNormal + vno ) = -1.0f * *( m_orgNormal + vno );
		}

		D3DXVECTOR3 tmpt;
		D3DXVec3Cross( &tmpt, m_Binormal + vno, m_orgNormal + vno );
		D3DXVec3Normalize( &tmpt, &tmpt );

		( m_skintan + vno )->tangent[0] = tmpt.x;
		( m_skintan + vno )->tangent[1] = tmpt.y;
		( m_skintan + vno )->tangent[2] = tmpt.z;
		( m_skintan + vno )->tangent[3] = 0.0f;

		if( m_revskintan ){

			D3DXVECTOR3 tmprevt;
			D3DXVec3Cross( &tmprevt, m_revBinormal + vno, m_revNormal + vno );
			D3DXVec3Normalize( &tmprevt, &tmprevt );

			( m_revskintan + vno )->tangent[0] = tmprevt.x;
			( m_revskintan + vno )->tangent[1] = tmprevt.y;
			( m_revskintan + vno )->tangent[2] = tmprevt.z;
			( m_revskintan + vno )->tangent[3] = 0.0f;
		}

	}

	return 0;
}


int CD3DDisp::SetSkinColorPM( CPolyMesh* pm, float alpha )// for TLMODE_D3D
{


	DWORD vno;
	SKINMATERIAL* curv;


	for( vno = 0; vno < m_numTLV; vno++ ){
		curv = m_smaterial + vno;

		int diffr, diffg, diffb;
		diffr = (int)((pm->diffusebuf + vno)->r * 255.0f );
		diffr = min( diffr, 255 );
		diffr = max( diffr, 0 );

		diffg = (int)((pm->diffusebuf + vno)->g * 255.0f );
		diffg = min( diffg, 255 );
		diffg = max( diffg, 0 );

		diffb = (int)((pm->diffusebuf + vno)->b * 255.0f );
		diffb = min( diffb, 255 );
		diffb = max( diffb, 0 );

		int ialpha;
		ialpha = (int)( alpha * 255.0f );
		ialpha = min( ialpha, 255 );
		ialpha = max( ialpha, 0 );

		curv->diffuse = D3DCOLOR_ARGB( ialpha, diffr, diffg, diffb );

		int specr, specg, specb;
		specr = (int)((pm->specularbuf + vno)->r * 255.0f );
		specr = min( specr, 255 );
		specr = max( specr, 0 );

		specg = (int)((pm->specularbuf + vno)->g * 255.0f );
		specg = min( specg, 255 );
		specg = max( specg, 0 );

		specb = (int)((pm->specularbuf + vno)->b * 255.0f );
		specb = min( specb, 255 );
		specb = max( specb, 0 );
		
		curv->specular = D3DCOLOR_ARGB( 255, specr, specg, specb );

		int ambr, ambg, ambb;
		ambr = (int)((pm->ambientbuf + vno)->r * 255.0f );
		ambr = min( ambr, 255 );
		ambr = max( ambr, 0 );

		ambg = (int)((pm->ambientbuf + vno)->g * 255.0f );
		ambg = min( ambg, 255 );
		ambg = max( ambg, 0 );

		ambb = (int)((pm->ambientbuf + vno)->b * 255.0f );
		ambb = min( ambb, 255 );
		ambb = max( ambb, 0 );

		curv->ambient = D3DCOLOR_ARGB( 255, ambr, ambg, ambb );

		int emir, emig, emib;
		emir = (int)((pm->emissivebuf + vno)->r * 255.0f );
		emir = min( emir, 255 );
		emir = max( emir, 0 );

		emig = (int)((pm->emissivebuf + vno)->g * 255.0f );
		emig = min( emig, 255 );
		emig = max( emig, 0 );

		emib = (int)((pm->emissivebuf + vno)->b * 255.0f );
		emib = min( emib, 255 );
		emib = max( emib, 0 );

		curv->emissive = D3DCOLOR_ARGB( 255, emir, emig, emib );

		curv->power = *(pm->powerbuf + vno);

	}

	return 0;
}



int CD3DDisp::SetSkinNormalPM2( CPolyMesh2* pm2 )// for TLMODE_D3D
{
	int faceno;
	//WORD pno0, pno1, pno2;
	int pno0, pno1, pno2;
	SKINVERTEX* v0;
	SKINVERTEX* v1;
	SKINVERTEX* v2;
	D3DXVECTOR3 nvec;

	char* pm2calcflag;

	pm2calcflag = (char*)malloc( sizeof( char ) * m_numTLV );
	if( !pm2calcflag ){
		DbgOut( "d3ddisp : SetVertNormalPM2 : pm2calcflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( pm2calcflag, sizeof( char ) * m_numTLV );

	DWORD vno;
	SKINVERTEX* curv;
	for( vno = 0; vno < m_numTLV; vno++ ){
		curv = m_skinv + vno;
		curv->normal[0] = 0.0f;
		curv->normal[1] = 0.0f;
		curv->normal[2] = 0.0f;
		curv->normal[3] = 0.0f;
	}

	ZeroMemory( m_skintan, sizeof( SKINTANGENT ) * m_numTLV );
	ZeroMemory( m_faceBinormal, sizeof( D3DXVECTOR3 ) * m_vnum );
	if( m_revfaceBinormal ){
		ZeroMemory( m_revfaceBinormal, sizeof( D3DXVECTOR3 ) * m_vnum );
	}

	for( faceno = 0; faceno < m_vnum; faceno++ ){
		//normalの計算
//		pno0 = *(m_dispIndices + faceno * 3);
//		pno1 = *(m_dispIndices + faceno * 3 + 1);
//		pno2 = *(m_dispIndices + faceno * 3 + 2);
		if( (m_clockwise == 2) || (m_clockwise == 3) ){
			pno0 = *(pm2->optindexbuf + faceno * 3);
			pno1 = *(pm2->optindexbuf + faceno * 3 + 1);
			pno2 = *(pm2->optindexbuf + faceno * 3 + 2);
		}else if( m_clockwise == 1 ){
			pno0 = *(pm2->optindexbuf + faceno * 3);
			pno1 = *(pm2->optindexbuf + faceno * 3 + 2);
			pno2 = *(pm2->optindexbuf + faceno * 3 + 1);
		}

		v0 = m_skinv + pno0;
		v1 = m_skinv + pno1;
		v2 = m_skinv + pno2;

		//!!!!!!!!!!!!!!!!!!!!!
		//同じ頂点があると、法線が不自然になり、黒っぽいポリゴンが出来てしまうので、除外する。
		//メタセコイアで、接続距離ゼロの鏡面設定をすると、たくさん、現れる。
		//!!!!!!!!!!!!!!!!!!!!!
		int hassame;
		if( (v0->pos[0] == v1->pos[0]) && (v0->pos[1] == v1->pos[1]) && (v0->pos[2] == v1->pos[2]) ){
			hassame = 1;
		}else if( (v0->pos[0] == v2->pos[0]) && (v0->pos[1] == v2->pos[1]) && (v0->pos[2] == v2->pos[2]) ){
			hassame = 1;
		}else if( (v1->pos[0] == v2->pos[0]) && (v1->pos[1] == v2->pos[1]) && (v1->pos[2] == v2->pos[2]) ){
			hassame = 1;
		}else{
			hassame = 0;
		}

		if( hassame == 1 )
			continue;

	
		int ret;
		ret = CalcNormal( &nvec, v0, v1, v2 );
		if( ret ){
			DbgOut( "CD3DDisp : SetVertNormalPM2 : CalcNormal error !!!\n" );
			return 1;
		}
		
		*( m_faceNormal + faceno ) = nvec;


		D3DXVECTOR3 pos0, pos1, pos2;
		pos0.x = v0->pos[0]; pos0.y = v0->pos[1]; pos0.z = v0->pos[2];
		pos1.x = v1->pos[0]; pos1.y = v1->pos[1]; pos1.z = v1->pos[2];
		pos2.x = v2->pos[0]; pos2.y = v2->pos[1]; pos2.z = v2->pos[2];

		*( m_faceBinormal + faceno ) = -( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 ) + ( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 );
//		*( m_faceBinormal + faceno ) = -( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 ) + ( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 );

		/***
		D3DXVECTOR3		dv;
		D3DXVECTOR3		cp;

		D3DXVECTOR3		edge01(pos1.x - pos0.x, v1->tex1[0] - v0->tex1[0], v1->tex1[1] - v0->tex1[1]);
		D3DXVECTOR3		edge02(pos2.x - pos0.x, v2->tex1[0] - v0->tex1[0], v2->tex1[1] - v0->tex1[1]);
		D3DXVec3Cross(&cp, &edge01, &edge02);
		if(fabs(cp.x) > 1e-6){
			dv.x = -cp.z / cp.x;
		}

		edge01 = D3DXVECTOR3(pos1.y - pos0.y, v1->tex1[0] - v0->tex1[0], v1->tex1[1] - v0->tex1[1]);
		edge02 = D3DXVECTOR3(pos2.y - pos0.y, v2->tex1[0] - v0->tex1[0], v2->tex1[1] - v0->tex1[1]);
		D3DXVec3Cross(&cp, &edge01, &edge02);
		if(fabs(cp.x) > 1e-6){
			dv.y = -cp.z / cp.x;
		}

		edge01 = D3DXVECTOR3(pos1.z - pos0.z, v1->tex1[0] - v0->tex1[0], v1->tex1[1] - v0->tex1[1]);
		edge02 = D3DXVECTOR3(pos2.z - pos0.z, v2->tex1[0] - v0->tex1[0], v2->tex1[1] - v0->tex1[1]);
		D3DXVec3Cross(&cp, &edge01, &edge02);
		if(fabs(cp.x) > 1e-6){
			dv.z = -cp.z / cp.x;
		}
		*( m_faceBinormal + faceno ) = dv;
		***/
		XVec3Normalize( m_faceBinormal + faceno, m_faceBinormal + faceno );

		if( m_revfaceBinormal ){
			*( m_revfaceBinormal + faceno ) = -( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 ) + ( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 );
			//*( m_revfaceBinormal + faceno ) = -( v2->tex1[0] - v0->tex1[0] ) * ( pos1 - pos0 ) + ( v1->tex1[0] - v0->tex1[0] ) * ( pos2 - pos0 );
			XVec3Normalize( m_revfaceBinormal + faceno, m_revfaceBinormal + faceno );
		}

	}

// 同じ座標を持つ頂点の法線ベクトルを足しあう
	DWORD oldvno;
	int optvno;
	for( oldvno = 0; oldvno < m_numIndices; oldvno++ ){
		optvno = *( pm2->oldpno2optpno + oldvno );

		if( *( pm2calcflag + optvno ) == 0 ){

			SKINVERTEX* curskinv;
			curskinv = m_skinv + optvno;

			SKINTANGENT* curskintan;
			curskintan = m_skintan + optvno;


			D3DXVECTOR3 tmpn;
			tmpn = *( m_faceNormal + (oldvno / 3) );

			D3DXVECTOR3 tmpb;
			tmpb = *( m_faceBinormal + (oldvno / 3) );

			SKINTANGENT* currevskintan = 0;
			D3DXVECTOR3 tmprevb;
			if( m_revskintan ){
				currevskintan = m_revskintan + optvno;
				tmprevb = *( m_revfaceBinormal + (oldvno / 3) );
			}


			SMFACE* cursmf;
			cursmf = *( pm2->smface + oldvno );
			while( cursmf ){
				if( (cursmf->faceno != -1) && (cursmf->smoothflag == 1 ) ){
					tmpn += *( m_faceNormal + cursmf->faceno );			
					tmpb += *( m_faceBinormal + cursmf->faceno );
					if( m_revskintan ){
						tmprevb += *( m_revfaceBinormal + cursmf->faceno );
					}
				}

				if( cursmf->faceno != -1 ){
					cursmf++;
				}else{
					cursmf = 0;
				}
			}

			XVec3Normalize( &tmpn, &tmpn );

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if( (tmpn.x == 0.0f) && (tmpn.y == 0.0f) && (tmpn.z == 0.0f ) ){
				tmpn.x = 0.0f; tmpn.y = -1.0f; tmpn.z = 0.0f;
			}


			curskinv->normal[0] = tmpn.x;
			curskinv->normal[1] = tmpn.y;
			curskinv->normal[2] = tmpn.z;

			XVec3Normalize( &tmpb, &tmpb );
			D3DXVECTOR3 tmpt;
			D3DXVec3Cross( &tmpt, &tmpb, &tmpn );
			XVec3Normalize( &tmpt, &tmpt );

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if( (tmpt.x == 0.0f) && (tmpt.y == 0.0f) && (tmpt.z == 0.0f ) ){
				tmpt.x = 1.0f; tmpt.y = 0.0f; tmpt.z = 0.0f;
			}

			curskintan->tangent[0] = tmpt.x;
			curskintan->tangent[1] = tmpt.y;
			curskintan->tangent[2] = tmpt.z;
			curskintan->tangent[3] = 0.0f;
	
			if( m_revskintan ){
				XVec3Normalize( &tmprevb, &tmprevb );
				D3DXVECTOR3 revn = -tmpn;
				D3DXVECTOR3 tmprevt;
				D3DXVec3Cross( &tmprevt, &tmprevb, &revn );
				XVec3Normalize( &tmprevt, &tmprevt );

				currevskintan->tangent[0] = tmprevt.x;
				currevskintan->tangent[1] = tmprevt.y;
				currevskintan->tangent[2] = tmprevt.z;
				currevskintan->tangent[3] = 0.0f;
			}

			*( pm2calcflag + optvno ) = 1;
		}
	}


	if( pm2calcflag )
		free( pm2calcflag );


	if( m_revskinv ){
		curv = m_skinv;
		SKINVERTEX* currevv = m_revskinv;

		for( vno = 0; vno < m_numTLV; vno++ ){
			currevv->normal[0] = -curv->normal[0];
			currevv->normal[1] = -curv->normal[1];
			currevv->normal[2] = -curv->normal[2];

			curv++;
			currevv++;
		}

	}


	return 0;
}

int CD3DDisp::SetLVColor( CExtLine* extline, float alpha )
{
	CMeshInfo* mi;
	mi = extline->meshinfo;

	CVec3f* diffuse = 0;
	diffuse = mi->GetMaterial( MAT_DIFFUSE );

	unsigned char a;
	unsigned char diffr, diffg, diffb;

	a = (unsigned char)( alpha * 255.0f );

	diffr = (unsigned char)( diffuse->x * 255.0f );
	diffg = (unsigned char)( diffuse->y * 255.0f );
	diffb = (unsigned char)( diffuse->z * 255.0f );

	if( a < 0 )
		a = 0;
	else if( a > 255 )
		a = 255;

	if( diffr < 0 )
		diffr = 0;
	else if( diffr > 255 )
		diffr = 255;

	if( diffg < 0 )
		diffg = 0;
	else if( diffg > 255 )
		diffg = 255;

	if( diffb < 0 )
		diffb = 0;
	else if( diffb > 255 )
		diffb = 255;

//DbgOut( "d3ddisp : SetLVColor : %d %d %d %d\n", a, diffr, diffg, diffb );

	int lvno;
	D3DLVERTEX* curlv;
	for( lvno = 0; lvno < m_vnum; lvno++ ){
		curlv = m_lv + lvno;
		curlv->color = D3DCOLOR_RGBA( diffr, diffg, diffb, a );
	}


	return 0;
}


int CD3DDisp::SetSkinColorPM2( CPolyMesh2* pm2, float alpha )// for TLMODE_D3D
{

	DWORD i;
	SKINMATERIAL* curv;
	float div255 = 1.0f / 255.0f;
	D3DTLVERTEX* optptr;

	for( i = 0; i < m_numTLV; i++ ){

		curv = m_smaterial + i;
		optptr = pm2->opttlv + i;

		int ialpha = (int)(alpha * 255.0f);
		ialpha = min( ialpha, 255 );
		ialpha = max( ialpha, 0 );
		curv->diffuse = D3DCOLOR_ARGB( ialpha, (optptr->color >> 16) & 0xFF, (optptr->color >> 8) & 0xFF, optptr->color & 0xFF );
		curv->specular = optptr->specular;

		int iambr, iambg, iambb;
		iambr = (int)((pm2->optambient + i)->r * 255.0f);
		iambr = min( iambr, 255 );
		iambr = max( iambr, 0 );

		iambg = (int)((pm2->optambient + i)->g * 255.0f);
		iambg = min( iambg, 255 );
		iambg = max( iambg, 0 );

		iambb = (int)((pm2->optambient + i)->b * 255.0f);
		iambb = min( iambb, 255 );
		iambb = max( iambb, 0 );
		
		curv->ambient = D3DCOLOR_ARGB( 255, iambr, iambg, iambb );

//DbgOut( "check !!!, d3ddisp : SetSkinColorPM2 : vert %d, amb %x\r\n", i, curv->ambient );


		int iemir, iemig, iemib;
		iemir = (int)( (pm2->optemissivebuf + i)->r * 255.0f );
		iemir = min( iemir, 255 );
		iemir = max( iemir, 0 );

		iemig = (int)( (pm2->optemissivebuf + i)->g * 255.0f );
		iemig = min( iemig, 255 );
		iemig = max( iemig, 0 );

		iemib = (int)( (pm2->optemissivebuf + i)->b * 255.0f );
		iemib = min( iemib, 255 );
		iemib = max( iemib, 0 );
	
		curv->emissive = D3DCOLOR_ARGB( 255, iemir, iemig, iemib );

		curv->power = *(pm2->optpowerbuf + i);
	}

	return 0;
}




int CD3DDisp::GetCurrentBlockNo( int srcfaceno, RENDERBLOCK* srcrb )
{
	int retblock = -1;
	int blno;

	for( blno = 0; blno < mrenderblocknum; blno++ ){
		if( (srcfaceno >= (srcrb + blno)->startface) && (srcfaceno < (srcrb + blno)->endface) ){
			retblock = blno;
			break;
		}
	}

	return retblock;
}

int CD3DDisp::GetCurrentSkinBlockNo( int srcfaceno, SKINBLOCK* srcsb )
{
	int retblock = -1;
	int blno;

	for( blno = 0; blno < mskinblocknum; blno++ ){
		if( (srcfaceno >= (srcsb + blno)->startface) && (srcfaceno < (srcsb + blno)->endface) ){
			retblock = blno;
			break;
		}
	}

	return retblock;
}

int CD3DDisp::SetTriNormalPM2_nolight( D3DXVECTOR3* worldv, CPolyMesh2* pm2 )
{
	int faceno;
	//WORD pno0, pno1, pno2;
	int pno0, pno1, pno2;

	D3DXVECTOR3 nvec;
	D3DXVECTOR3* v0;
	D3DXVECTOR3* v1;
	D3DXVECTOR3* v2;

	//lightflag == 0 の時は、PREINITの一回しか、ここを通らない。
	//lightflag == 0 の時は、まだ、clipIndicesがセットされていないので、使用できない。！！！


	ZeroMemory( m_orgNormal, sizeof( D3DXVECTOR3 ) * m_numTLV );

	int* pm2calcflag;
	pm2calcflag = (int*)malloc( sizeof( int ) * m_numTLV );
	if( !pm2calcflag ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( pm2calcflag, sizeof( int ) * m_numTLV );

	for( faceno = 0; faceno < m_vnum; faceno++ ){
		//normalの計算
//		pno0 = *(m_dispIndices + faceno * 3);
//		pno1 = *(m_dispIndices + faceno * 3 + 1);
//		pno2 = *(m_dispIndices + faceno * 3 + 2);
		pno0 = *(pm2->optindexbuf + faceno * 3);
		pno1 = *(pm2->optindexbuf + faceno * 3 + 1);
		pno2 = *(pm2->optindexbuf + faceno * 3 + 2);

		v0 = worldv + pno0;
		v1 = worldv + pno1;
		v2 = worldv + pno2;

		//!!!!!!!!!!!!!!!!!!!!!
		//同じ頂点があると、法線が不自然になり、黒っぽいポリゴンが出来てしまうので、除外する。
		//メタセコイアで、接続距離ゼロの鏡面設定をすると、たくさん、現れる。
		//!!!!!!!!!!!!!!!!!!!!!
		int hassame;
		if( (v0->x == v1->x) && (v0->y == v1->y) && (v0->z == v1->z) ){
			hassame = 1;
		}else if( (v0->x == v2->x) && (v0->y == v2->y) && (v0->z == v2->z) ){
			hassame = 1;
		}else if( (v1->x == v2->x) && (v1->y == v2->y) && (v1->z == v2->z) ){
			hassame = 1;
		}else{
			hassame = 0;
		}

		if( hassame == 1 )
			continue;

	
		int ret;
		ret = CalcNormal( &nvec, v0, v1, v2 );
		if( ret ){
			DbgOut( "CD3DDisp : SetVertNormalPM2 : CalcNormal error !!!\n" );
			return 1;
		}
		
		*( m_faceNormal + faceno ) = nvec;
	}


	DWORD oldvno;
	int optvno;
	for( oldvno = 0; oldvno < m_numIndices; oldvno++ ){
		optvno = *( pm2->oldpno2optpno + oldvno );

		if( *( pm2calcflag + optvno ) == 0 ){

			D3DXVECTOR3* curnorm;
			curnorm = m_orgNormal + optvno;
			D3DXVECTOR3 tmpn;
			tmpn = *( m_faceNormal + (oldvno / 3) );

			SMFACE* cursmf;
			cursmf = *( pm2->smface + oldvno );
			while( cursmf ){
				if( (cursmf->faceno != -1) && (cursmf->smoothflag == 1 ) ){
					tmpn += *( m_faceNormal + cursmf->faceno );			
				}

				if( cursmf->faceno != -1 ){
					cursmf++;
				}else{
					cursmf = 0;
				}
			}

			D3DXVec3Normalize( &tmpn, &tmpn );

			*curnorm = tmpn;

			*( pm2calcflag + optvno ) = 1;
		}
	}

	if( pm2calcflag ){
		free( pm2calcflag );
		pm2calcflag = 0;
	}


	//m_cullflag = 0;

/***
	if( mrenderblock && mcullblock ){
		MoveMemory( mcullblock, mclipblock, sizeof( RENDERBLOCK ) * mrenderblocknum );
	}
	if( mrenderblock && mcullrevblock ){
		MoveMemory( mcullrevblock, mcliprevblock, sizeof( RENDERBLOCK ) * mrenderblocknum );
	}
***/

	return 0;
}

int CD3DDisp::TransformSkinMatMorph( CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3* vEyePt )
{
	m_matWorld = *matWorld;
	m_matView = *matView;
	m_matProj = *matProj;
	m_EyePt = *vEyePt;


	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){
		int* curskin2bone = *(mskinmat2bonemat + blno);
		int curskinnum = *(mskinmatnum + blno);
		D3DXMATRIX* curskinmat = *(mskinmat + blno);

		*curskinmat = m_scalemat * m_matWorld;//!!!!!!!!!

		int skno;
		for( skno = 1; skno < curskinnum; skno++ ){
			int boneno;
			boneno = *(curskin2bone + skno);
			if( boneno > 0 ){
				CMotionCtrl* curmc;
				CMatrix2* curmatptr;
				D3DXMATRIX curxmat;
				curmc = (*lpmh)( boneno );
				curmatptr = &(curmc->curmat);
				MatConvD3DX( &curxmat, curmatptr );

				*( curskinmat + skno ) = curxmat * m_scalemat * m_matWorld;
			}else{
				*( curskinmat + skno ) = m_scalemat * m_matWorld;
			}
		}
	}


	return 0;


}


int CD3DDisp::TransformSkinMat( int serialno, int* seri2boneno, D3DXMATRIX* firstscale, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3* vEyePt )
{
//DbgOut( "d3ddisp : start TransformSkinMat\n" );
	m_matWorld = *matWorld;
	m_matView = *matView;
	m_matProj = *matProj;
	m_EyePt = *vEyePt;


//DbgOut( "\r\n" );
	if( lpmh ){
		int blno;
		for( blno = 0; blno < mskinblocknum; blno++ ){


			int* curskin2bone = *(mskinmat2bonemat + blno);
			int curskinnum = *(mskinmatnum + blno);
			D3DXMATRIX* curskinmat = *(mskinmat + blno);

			*curskinmat = m_scalemat * m_matWorld;//!!!!!!!!!

			int skno;
			for( skno = 1; skno < curskinnum; skno++ ){
				int boneno;
				boneno = *(curskin2bone + skno);
				if( boneno > 0 ){
					CMotionCtrl* curmc;
					CMatrix2* curmatptr;
					D3DXMATRIX curxmat;
					curmc = (*lpmh)( boneno );
					curmatptr = &(curmc->curmat);
					MatConvD3DX( &curxmat, curmatptr );

					*( curskinmat + skno ) = curxmat * m_scalemat * m_matWorld;
				}else{
					*( curskinmat + skno ) = m_scalemat * m_matWorld;
				}
			}
		}
	}else{
		//billboard
		int skno;
		int curskinnum = *mskinmatnum;
		D3DXMATRIX* curskinmat = *mskinmat;
		for( skno = 0; skno < curskinnum; skno++ ){
			//*( curskinmat + skno ) = m_scalemat * m_matWorld;
			//scalematはshdelem RenderBillboardでmatWorldに掛けられている。
			*( curskinmat + skno ) = m_matWorld;
		}

	}

//DbgOut( "d3ddisp : end TransformSkinMat\n" );

	return 0;
}

/***
int CD3DDisp::TransformSkinMat( int serialno, int* seri2boneno, D3DXMATRIX* firstscale, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3* vEyePt )
{

//DbgOut( "d3ddisp : start TransformSkinMat\n" );
	m_matWorld = *matWorld;
	m_matView = *matView;
	m_matProj = *matProj;
	m_EyePt = *vEyePt;


//DbgOut( "\r\n" );
	if( lpmh ){

		CMotionCtrl* melem = (*lpmh)( serialno );
		D3DXMATRIX* mattable;
		if( (m_scalemat == *firstscale) || (melem->type == SHDINFSCOPE) || (melem->type == SHDBBOX) ){
			mattable = lpmh->swvmat;
		}else{
			mattable = melem->swvmat;
		}
		_ASSERT( mattable );


		int blno;
		for( blno = 0; blno < mskinblocknum; blno++ ){

			int* curskin2bone = *(mskinmat2bonemat + blno);
			int curskinnum = *(mskinmatnum + blno);
			D3DXMATRIX* curskinmat = *(mskinmat + blno);

			*curskinmat = m_scalemat * *m_matWorld;//!!!!!!!!!

			int skno;
			for( skno = 1; skno < curskinnum; skno++ ){
				int boneseri;
				boneseri = max( *(curskin2bone + skno), 0 );

				int bno;	
				bno = *( seri2boneno + boneseri );
				*( curskinmat + skno ) = *( mattable + bno );

//				if( boneno > 0 ){
//					CMotionCtrl* curmc;
//					CMatrix2* curmatptr;
//					D3DXMATRIX curxmat;
//					curmc = (*lpmh)( boneno );
//					curmatptr = &(curmc->curmat);
//					MatConvD3DX( &curxmat, curmatptr );
//
//					*( curskinmat + skno ) = curxmat * m_scalemat * m_matWorld;
//				}else{
//					*( curskinmat + skno ) = m_scalemat * m_matWorld;
//				}
			}
		}
	}else{
		//billboard
		int skno;
		int curskinnum = *mskinmatnum;
		D3DXMATRIX* curskinmat = *mskinmat;
		D3DXMATRIX mat0 = m_scalemat * m_matWorld;
		for( skno = 0; skno < curskinnum; skno++ ){
			*( curskinmat + skno ) = mat0;
		}

	}

//DbgOut( "d3ddisp : end TransformSkinMat\n" );

	return 0;
}
***/

int CD3DDisp::TransformDispDataSCV( int* seri2boneno, int serialno, D3DXMATRIX* firstscale, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect )
{
	DWORD tlvno;
	D3DXMATRIX matSet1;

	SKINVERTEX* srctlv = m_skinv;
	D3DXVECTOR3* dsttlv = m_scv;
	D3DXVECTOR3* curwv = m_worldv;

	CInfElem* curie = m_IEptr;

	m_matView = matView;// !!! InitColor での、カリングに使用。
	m_matProj = matProj;


	D3DXMATRIX swmat = m_scalemat * matWorld;
	D3DXMATRIX vpmat = matView * matProj;

	CMotionCtrl* melem = (*srclpmh)( serialno );

	float width = (float)dwClipWidth * 2.0f;
	float height = (float)dwClipHeight * 2.0f;

	float flClipWidth = (float)dwClipWidth;
	float flClipHeight = (float)dwClipHeight;

	m_dwClipWidth = dwClipWidth;
	m_dwClipHeight = dwClipHeight;

	float projfar, divprojfar;
	projfar = -matProj._43 / ( matProj._33 - 1.0f );
	divprojfar = -( matProj._33 - 1.0f ) / matProj._43;


	//m_numsubPrim = 0;	
	m_dbgfindcnt = 0;
	
	CMatrix2 inimat;
	int curboneno;
	inimat.Identity();

	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		
		float x = srctlv->pos[0];
		float y = srctlv->pos[1];
		float z = srctlv->pos[2];

		float xp = 0.0f;
		float yp = 0.0f;
		float zp = 0.0f;//, wp;

		int infno;
		for( infno = 0; infno < curie->infnum; infno++ ){
			INFELEM* curIE;
			curIE = curie->ie + infno;
			curboneno = max( curIE->bonematno, 0 );
			if( curboneno > 0 ){
				CMotionCtrl* curmc;
				CMatrix2* curmatptr;
				D3DXMATRIX curxmat;
				curmc = (*srclpmh)( curboneno );
				curmatptr = &(curmc->curmat);
				MatConvD3DX( &curxmat, curmatptr );
				matSet1 = curxmat * swmat;
			}else{
				matSet1 = swmat;
			}

			float xpa, ypa, zpa;//, wpa;

			xpa = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
			ypa = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
			zpa = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;
			//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
						
			xp += xpa * curIE->dispinf;
			yp += ypa * curIE->dispinf;
			zp += zpa * curIE->dispinf;

		}

		if( curie->infnum <= 0 ){
			matSet1 = swmat;
			float xpa, ypa, zpa;//, wpa;

			xpa = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
			ypa = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
			zpa = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;
			//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
						
			xp += xpa;
			yp += ypa;
			zp += zpa;
		}

		curwv->x = xp;//!!!!!!!!
		curwv->y = yp;
		curwv->z = zp;
					
		float xp2, yp2, zp2, wp2;

		xp2 = vpmat._11*xp + vpmat._21*yp + vpmat._31*zp + vpmat._41;
		yp2 = vpmat._12*xp + vpmat._22*yp + vpmat._32*zp + vpmat._42;
		zp2 = vpmat._13*xp + vpmat._23*yp + vpmat._33*zp + vpmat._43;
		wp2 = vpmat._14*xp + vpmat._24*yp + vpmat._34*zp + vpmat._44;

		D3DTLVERTEX tmptlv;
		ConvScreenPos( PROJ_NORMAL, xp2, yp2, zp2, wp2, flClipHeight, flClipWidth, aspect, &tmptlv );
		dsttlv->x = tmptlv.sx;
		dsttlv->y = tmptlv.sy;
		dsttlv->z = tmptlv.sz;

		srctlv++;
		dsttlv++;		
			
		curie++; // !!!!
		curwv++;//!!!!!
	}


	return 0;
}

int CD3DDisp::MoveUVArray( int texanimtype, int* dirtyptr, float ustep, float vstep, int srctype )
{
	DWORD i;

	SKINVERTEX* curskinv;
	SKINVERTEX* currevskinv;

	float newu, newv;
	float setu, setv;
	int intu, intv;


	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb = 0;
		LPDIRECT3DVERTEXBUFFER9 currevvb = 0;
			
		curvb = *( m_ArrayVB + blno );
		if( m_revArrayVB ){
			currevvb = *( m_revArrayVB + blno );
		}

		SKINVERTEX* pSkinv = 0;
		SKINVERTEX* prevSkinv = 0;
		if( FAILED( curvb->Lock( 0, 0, (void**)&pSkinv, NULL ) ) ){
			_ASSERT( 0 );
			return 1;
		}			
		if( currevvb ){
			if( FAILED( currevvb->Lock( 0, 0, (void**)&prevSkinv, NULL ) ) ){
				_ASSERT( 0 );
				return 1;
			}			
		}

		for( i = 0; i < m_numTLV; i++ ){
			if( *( dirtyptr + i ) != 0 ){
				curskinv = m_skinv + i;

				newu = curskinv->tex1[0] + ustep;
				newv = curskinv->tex1[1] + vstep;

				if( texanimtype == TEXANIM_WRAP ){
					if( newu > 1.0f ){
						intu = (int)newu;
						setu = newu - (float)intu;
					}else if( newu < 0.0f ){
						intu = (int)newu;
						float flu = newu - (float)intu;
						setu = 1.0f + flu;
					}else{
						setu = newu;
					}

					if( newv > 1.0f ){
						intv = (int)newv;
						setv = newv - (float)intv;
					}else if( newv < 0.0f ){
						intv = (int)newv;
						float flv = newv - (float)intv;
						setv = 1.0f + flv;
					}else{
						setv = newv;
					}
				}else{
					setu = newu;
					setv = newv;
				}
							
				curskinv->tex1[0] = setu;
				curskinv->tex1[1] = setv;
				(pSkinv + i)->tex1[0] = setu;
				(pSkinv + i)->tex1[1] = setv;

				if( m_revskinv ){
					currevskinv = m_revskinv + i;
					currevskinv->tex1[0] = setu;
					currevskinv->tex1[1] = setv;

					if( prevSkinv ){
						(prevSkinv + i)->tex1[0] = setu;
						(prevSkinv + i)->tex1[1] = setv;
					}
				}
			}
		}

		curvb->Unlock();
		if( currevvb ){
			currevvb->Unlock();
		}

	}

	return 0;

}


int CD3DDisp::MoveTexUV( float ustep, float vstep, int srctype )
{
	//D3DTLVERTEX* srctlv = m_orgTLV;
	//D3DTLVERTEX* dsttlv = m_dispTLV;
//	static float umove = 0.0f;
//	static float vmove = 0.0f;
	DWORD i;

	SKINVERTEX* curskinv;
	SKINVERTEX* currevskinv;

	float newu, newv;
	float setu, setv;
	int intu, intv;

	for( i = 0; i < m_numTLV; i++ ){
		curskinv = m_skinv + i;

		newu = curskinv->tex1[0] + ustep;
		newv = curskinv->tex1[1] + vstep;

		//if( srctype != SHDPOLYMESH2 ){
		if( newu > 1.0f ){
			intu = (int)newu;
			setu = newu - (float)intu;
		}else if( newu < 0.0f ){
			intu = (int)newu;
			float flu = newu - (float)intu;
			setu = 1.0f + flu;
		}else{
			setu = newu;
		}

		if( newv > 1.0f ){
			intv = (int)newv;
			setv = newv - (float)intv;
		}else if( newv < 0.0f ){
			intv = (int)newv;
			float flv = newv - (float)intv;
			setv = 1.0f + flv;
		}else{
			setv = newv;
		}
					
		//}else{
		//	setu = newu;
		//	setv = newv;
		//}

		curskinv->tex1[0] = setu;
		curskinv->tex1[1] = setv;

		if( m_revskinv ){
			currevskinv = m_revskinv + i;
			currevskinv->tex1[0] = setu;
			currevskinv->tex1[1] = setv;
		}
	}
		

	return 0;
}



int CD3DDisp::RenderLine( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, float srcalpha )
{
	HRESULT hres;
	int ret = 0;

	if( m_linekind == 0 ){
		_ASSERT( 0 );
		return 1;
	}

	if( withalpha && (srcalpha >= 0.99f ) ){
		return 0;
	}
	if( (withalpha == 0) && (srcalpha < 0.99f) ){
		return 0;
	}

//	if( g_cop0 != D3DTOP_DISABLE ){
//		pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_DISABLE );
//		g_cop0 = D3DTOP_DISABLE;
//	}
//	if( g_aop0 != D3DTOP_DISABLE ){
//		pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );	
//		g_aop0 = D3DTOP_DISABLE;
//	}
	if( g_cop0 != D3DTOP_MODULATE ){
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_aop0 != D3DTOP_MODULATE ){
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}


	if( g_cop1 != D3DTOP_DISABLE ){
		pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE );
		g_cop1 = D3DTOP_DISABLE;
	}
	if( g_aop1 != D3DTOP_DISABLE ){
		pd3dDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
		g_aop1 = D3DTOP_DISABLE;
	}
	if( g_curtex0 != NULL ){
		pd3dDevice->SetTexture( 0, NULL );
		g_curtex0 = NULL;
	}
	if( g_curtex1 != NULL ){
		pd3dDevice->SetTexture( 1, NULL );
		g_curtex1 = NULL;
	}
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
	g_renderstate[ D3DRS_LIGHTING ] = FALSE;

	//pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );

	hres = pd3dDevice->SetVertexShader( NULL );
	_ASSERT( hres == D3D_OK );
	hres = pd3dDevice->SetFVF( D3DFVF_LVERTEX );
	_ASSERT( hres == D3D_OK );
	hres =  pd3dDevice->SetStreamSource( 0, m_VB, 0, sizeof(D3DLVERTEX) );
	_ASSERT( hres == D3D_OK );
	hres = pd3dDevice->SetIndices( m_IB );
	_ASSERT( hres == D3D_OK );


	hres = pd3dDevice->DrawIndexedPrimitive( (D3DPRIMITIVETYPE)m_linekind,
					0,
					0,
					m_numTLV,
					0L, //m_dispIndices, 
					m_numPrim
					//renderPrim
		);
						
	if( FAILED( hres ) ){
		DbgOut( "Render error !!! %x: numTLV %d, numPrim %d\n", 
			hres, m_numTLV, m_numPrim );
		ret = 1;
	}


//	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  TRUE );
//	g_renderstate[ D3DRS_LIGHTING ] = TRUE;



	return ret;

}


int CD3DDisp::SetRenderStateIfNotSame( LPDIRECT3DDEVICE9 pd3dDevice, int srcstate, DWORD srcval )
{
	_ASSERT( pd3dDevice );

	if( (g_renderstate[ srcstate ] != srcval) && (srcstate >= 0) && (srcstate <= D3DRS_BLENDOP) ){
		pd3dDevice->SetRenderState( (D3DRENDERSTATETYPE)srcstate, srcval );
		g_renderstate[ srcstate ] = srcval;
	}

	return 0;
}

int CD3DDisp::RenderPhongShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, int srcenabletex )
{


	HRESULT hres;
	int ret = 0;

///////////////
	UINT cPasses, iPass;
	//UINT cPasses2, iPass2;

	
	int renderno;
	int befblno = -1;
	for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + renderno;

		int blno;
		blno = currb->skinno;

		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		char* curtexname = 0;
		int newrapstate;

		CMQOMaterial* curmqomat;
		curmqomat = currb->mqomat;
		
		if( srcenabletex ){
			if( curmqomat->curtexname == 0 ){
				if( curmqomat->tex[0] ){
					curtexname = curmqomat->tex;
				}else{
					curtexname = 0;
				}
			}else{
				if( *curmqomat->curtexname == 0 ){
					if( curmqomat->tex[0] ){
						curtexname = curmqomat->tex;
					}else{
						curtexname = 0;
					}
				}else{					
					curtexname = curmqomat->curtexname;
				}
			}
		}else{
			curtexname = 0;
		}

		switch( curmqomat->texrule ){
		case TEXRULE_X:
		case TEXRULE_Y:
		case TEXRULE_Z:
			newrapstate = 0;
			break;
		case TEXRULE_CYLINDER:
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_SPHERE:
			//newrapstate = D3DWRAP_U | D3DWRAP_V;
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_MQ:
		default:
			newrapstate = 0;
			break;
		}
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_WRAP0, newrapstate );



		LPDIRECT3DTEXTURE9 curtex = NULL;		
		curtex = g_texbnk->GetTexData( curtexname, curmqomat->transparent );

		int bumpflag = 0;

		hres = g_pEffect->SetTexture( g_hDecaleTex, curtex );
		_ASSERT( hres == D3D_OK );

		hres = g_pEffect->SetFloat( g_hmAlpha, curmqomat->col.a );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderPhongShadowMap0 : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->SetTechnique( g_hRenderSceneNormalShadow );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderPhongShadowMap0 : SetTechnique error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		if( blno != befblno ){
			hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderPhongShadowMap0 : worldmatrixarray set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			befblno = blno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		}

		/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderPhongShadowMap0 : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//for (iPass = 0; iPass < cPasses; iPass++){
			if( curskinnum != 1 ){
				if( curtex ){
					iPass = 0;
				}else{
					iPass = 1;
				}
			}else{
				if( curtex ){
					iPass = 4;
				}else{
					iPass = 5;
				}
			}
			hres = g_pEffect->BeginPass(iPass);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderPhongShadowMap0 : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			curnumprim = currb->endface - currb->startface;

			if( curnumprim > 0 ){
				//if( bumpflag == 0 ){
					pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
					pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
					pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				//}else{
				//	pd3dDevice->SetVertexDeclaration( m_pskintandecl );
				//	pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				//	pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				//	pd3dDevice->SetStreamSource( 2, m_VBtan, 0, sizeof( SKINTANGENT ) );
				//}
				pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb->startface * 3,
													curnumprim
													);
				//_ASSERT( hres == D3D_OK );
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				//if( bumpflag == 0 ){
					pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
					pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
					pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				//}else{
				//	pd3dDevice->SetVertexDeclaration( m_pskintandecl );
				//	pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
				//	pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				//	pd3dDevice->SetStreamSource( 2, m_revVBtan, 0, sizeof( SKINTANGENT ) );
				//}
				pd3dDevice->SetIndices( m_revIB );
				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb->startface * 3,
												curnumprim
												);
				//_ASSERT( hres == D3D_OK );
			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderPhongShadowMap0 : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		//}
		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderPhongShadowMap0 : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}	
	return ret;
}

int CD3DDisp::RenderPhongShadowMap1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* toonparams, 
	float* fogparams, int srcenabletex, CHKALPHA* chkalpha )
{

	HRESULT hres;
	int ret = 0;

	if( setalphaflag == 0 ){
		if( (withalpha == 0) && (chkalpha->notalphanum == 0) ){
			//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			goto phongedge0shadow;
		}
		if( (withalpha != 0) && (chkalpha->alphanum == 0) ){
			//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			goto phongedge0shadow;
		}
	}else{
		if( (withalpha == 0) && (alpha != 1.0f) && (chkalpha->notalphanum == 0) ){
			//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			goto phongedge0shadow;
		}
		if( (withalpha != 0) && (alpha == 1.0f) && (chkalpha->alphanum == 0) ){
			//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			goto phongedge0shadow;
		}
	}

//DbgOut( "d3ddisp : RenderTriPhong\n" );
	hres = g_pEffect->SetValue( g_hmFogParams, fogparams, sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriPhong : fogparams set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fsetlno;
	if( (g_renderstate[D3DRS_LIGHTING] == FALSE) ){
		fsetlno = -1.0f;//<--- lighting 無効のサイン
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		fsetlno = (float)g_lightnum;
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&m_EyePt), sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriPhong : LightSpecular set error !!!\n" );
		_ASSERT( 0 );
		return 1;	
	}

	if( (g_usePS == 1) && (toonparams) ){
		hres = g_pEffect->SetValue( g_hmToonParams, toonparams, sizeof( float ) * 4 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : toonparams set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

///////////////
	UINT cPasses, iPass;
	
	int renderno;
	int befblno = -1;
	for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + renderno;

		int blno;
		blno = currb->skinno;

		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		char* curtexname = 0;
		int newrapstate;

		CMQOMaterial* curmqomat;
		curmqomat = currb->mqomat;

		if( setalphaflag == 0 ){
			if( (withalpha == 0) && ( (curmqomat->col.a != 1.0f) || (curmqomat->transparent != 0) ) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (curmqomat->col.a == 1.0f) && (curmqomat->transparent == 0) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
		}else{
			if( (withalpha == 0) && ( (alpha != 1.0f) || (curmqomat->transparent != 0) ) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (alpha == 1.0f) && (curmqomat->transparent == 0) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
		}

		
		if( srcenabletex ){
			if( curmqomat->curtexname == 0 ){
				if( curmqomat->tex[0] ){
					curtexname = curmqomat->tex;
				}else{
					curtexname = 0;
				}
			}else{
				if( *curmqomat->curtexname == 0 ){
					if( curmqomat->tex[0] ){
						curtexname = curmqomat->tex;
					}else{
						curtexname = 0;
					}
				}else{					
					curtexname = curmqomat->curtexname;
				}
			}
		}else{
			curtexname = 0;
		}
		
		switch( curmqomat->texrule ){
		case TEXRULE_X:
		case TEXRULE_Y:
		case TEXRULE_Z:
			newrapstate = 0;
			break;
		case TEXRULE_CYLINDER:
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_SPHERE:
			//newrapstate = D3DWRAP_U | D3DWRAP_V;
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_MQ:
		default:
			newrapstate = 0;
			break;
		}
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_WRAP0, newrapstate );



		LPDIRECT3DTEXTURE9 curtex = NULL;		
		curtex = g_texbnk->GetTexData( curtexname, curmqomat->transparent );

		LPDIRECT3DTEXTURE9 bumptex = NULL;
		if( curmqomat->bump[0] != 0 ){
			bumptex = g_texbnk->GetTexData( curmqomat->bump, 0 );
		}

		int bumpflag;
		if( curtex && bumptex && g_bumpUse ){
			bumpflag = 1;
		}else{
			bumpflag = 0;
		}

		if( bumpflag == 0 ){
			hres = g_pEffect->SetTexture( g_hDecaleTex, curtex );
			_ASSERT( hres == D3D_OK );
		}else{
			hres = g_pEffect->SetTexture( g_hNormalMap, bumptex );
			_ASSERT( hres == D3D_OK );
			hres = g_pEffect->SetTexture( g_hDecaleTex, curtex );
			_ASSERT( hres == D3D_OK );
		}

		if( withalpha != 0 ){
			if( (curmqomat->blendmode == 0) || (curmqomat->blendmode == 100) ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}else if( (curmqomat->blendmode == 1) || (curmqomat->blendmode == 101) ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ONE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
			}else if( (curmqomat->blendmode == 2) || (curmqomat->blendmode == 102) ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
			}else if( curmqomat->blendmode == 103 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ZERO );
			}else if( curmqomat->blendmode == 104 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ZERO );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			}else if( curmqomat->blendmode == 105 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
			}else if( curmqomat->blendmode == 106 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			}else if( curmqomat->blendmode == 107 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
			}else if( curmqomat->blendmode == 108 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
			}

			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest1 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval1 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		}else{
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest0 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval0 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		if( setalphaflag == 0 ){
			hres = g_pEffect->SetFloat( g_hmAlpha, curmqomat->col.a );
		}else{
			hres = g_pEffect->SetFloat( g_hmAlpha, alpha );
		}
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( bumpflag == 0 ){
			if( lpsh->m_shader != COL_TOON0 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneNormalShadow );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( curskinnum != 1 ){
					if( curtex ){
						iPass = 2;
					}else{
						iPass = 3;
					}
				}else{
					if( curtex ){
						iPass = 6;
					}else{
						iPass = 7;
					}
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneToon0Shadow );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( curskinnum != 1 ){
					if( curtex ){
						iPass = 0;
					}else{
						iPass = 1;
					}
				}else{
					if( curtex ){
						iPass = 2;
					}else{
						iPass = 3;
					}
				}
			}
		}else if( lpsh->m_shader == COL_PPHONG ){
			hres = g_pEffect->SetTechnique( g_hRenderScenePPShadow );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( curskinnum != 1 ){
				if( curtex ){
					iPass = 0;
				}else{
					iPass = 1;
				}
			}else{
				if( curtex ){
					iPass = 2;
				}else{
					iPass = 3;
				}
			}

		}else{
			hres = g_pEffect->SetTechnique( g_hRenderSceneBumpShadow );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( curskinnum != 1 ){
				iPass = 0;
			}else{
				iPass = 1;
			}
		}


		if( blno != befblno ){
			hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : worldmatrixarray set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			befblno = blno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		}

		/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//for (iPass = 0; iPass < cPasses; iPass++){
			hres = g_pEffect->BeginPass(iPass);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			curnumprim = currb->endface - currb->startface;

			if( curnumprim > 0 ){
				if( bumpflag == 0 ){
					pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
					pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
					pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				}else{
					pd3dDevice->SetVertexDeclaration( m_pskintandecl );
					pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
					pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
					pd3dDevice->SetStreamSource( 2, m_VBtan, 0, sizeof( SKINTANGENT ) );
				}
				pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb->startface * 3,
													curnumprim
													);
				//_ASSERT( hres == D3D_OK );
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				if( bumpflag == 0 ){
					pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
					pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
					pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				}else{
					pd3dDevice->SetVertexDeclaration( m_pskintandecl );
					pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
					pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
					pd3dDevice->SetStreamSource( 2, m_revVBtan, 0, sizeof( SKINTANGENT ) );
				}
				pd3dDevice->SetIndices( m_revIB );
				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb->startface * 3,
												curnumprim
												);
				//_ASSERT( hres == D3D_OK );
			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		//}
		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}	

	goto phongedge0shadow;

//////////////
//////////////
//////////// edge
phongedge0shadow:


	UINT cPasses2, iPass2;
	if( lpsh->m_phongedge0enable && 
		( ( (withalpha == 0) && (lpsh->m_phongedge0Blend == 3) ) || ( (withalpha != 0) && (lpsh->m_phongedge0Blend != 3) ) ) ){

		if( g_cop0 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_DISABLE );
			g_cop0 = D3DTOP_DISABLE;
		}
		if( g_aop0 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
			g_aop0 = D3DTOP_DISABLE;
		}
		if( g_cop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE );
			g_cop1 = D3DTOP_DISABLE;
		}
		if( g_aop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
			g_aop1 = D3DTOP_DISABLE;
		}
		if( g_curtex0 != NULL ){
			pd3dDevice->SetTexture( 0, NULL );
			g_curtex0 = NULL;
		}
		if( g_curtex1 != NULL ){
			pd3dDevice->SetTexture( 1, NULL );
			g_curtex1 = NULL;
		}
			

		if( withalpha != 0 ){
			if( lpsh->m_phongedge0Blend == 0 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				//SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, FALSE );						
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, TRUE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, 0x08 );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
			}else if( lpsh->m_phongedge0Blend == 1 ){
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ONE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, TRUE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, 0x0 );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
			}else if( lpsh->m_phongedge0Blend == 2 ){
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				//SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, FALSE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, TRUE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, 0x0 );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
			}
		}
		hres = g_pEffect->SetFloat( g_hmAlpha, lpsh->m_phongedge0Alpha );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->SetValue( g_hmEdgeCol0, &(lpsh->m_phongedge0[0]), sizeof( float ) * 4 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mEdgeCol0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int renderno2;
		int befblno2 = -1;
		for( renderno2 = 0; renderno2 < mrenderblocknum; renderno2++ ){
			RENDERBLOCK* currb2;
			currb2 = mrenderblock + renderno2;

			int blno2;
			blno2 = currb2->skinno;

			D3DXMATRIX* curskinmat = *(mskinmat + blno2);
			int curskinnum = *(mskinmatnum + blno2);

	//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
	//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );


			////////////
			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( curskinnum == 2 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( blno2 != befblno2 ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : edge worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				befblno2 = blno2;
			}

			/////////
			hres = g_pEffect->Begin(&cPasses2, 0);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : effect Begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			for (iPass2 = 0; iPass2 < cPasses2; iPass2++){
				hres = g_pEffect->BeginPass(iPass2);
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : effect BeginPass error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


				// The effect interface queues up the changes and performs them 
				// with the CommitChanges call. You do not need to call CommitChanges if 
				// you are not setting any parameters between the BeginPass and EndPass.
				// V( g_pEffect->CommitChanges() );

				int curnumprim;
				curnumprim = currb2->endface - currb2->startface;

				if( curnumprim > 0 ){

					pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
					pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno2), 0, sizeof(SKINVERTEX) );
					//pd3dDevice->SetIndices( m_IB );
					pd3dDevice->SetIndices( m_revIB );

					hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb2->startface * 3,
													curnumprim
													);
				}

				hres = g_pEffect->EndPass();
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : effect EndPass error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
			hres = g_pEffect->End();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : effect End error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return ret;


}



int CD3DDisp::RenderTriPhong( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* toonparams, float* fogparams, int lightflag,
	int srcenabletex, CHKALPHA* chkalpha, float* bluralpha, int blurflag, int glowflag )
{
	HRESULT hres;
	int ret = 0;
	static int dbgcnt = 0;
//	static LPDIRECT3DTEXTURE9 s_tex = (LPDIRECT3DTEXTURE9)-1;


	if( glowflag ){
		srcenabletex = 1;//!!!!!!!!!!!!!!!!!!!!
	}


	if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
		if( setalphaflag == 0 ){
			if( (withalpha == 0) && (chkalpha->notalphanum == 0) ){
				//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
				goto phongedge0;
			}
			if( (withalpha != 0) && (chkalpha->alphanum == 0) ){
				//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
				goto phongedge0;
			}
		}else{
			if( (withalpha == 0) && (alpha != 1.0f ) && (chkalpha->notalphanum == 0) ){
				//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
				goto phongedge0;
			}
			if( (withalpha != 0) && (alpha == 1.0f) && (chkalpha->alphanum == 0) ){
				//return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
				goto phongedge0;
			}			
		}
	}else{
		hres = g_pEffect->SetMatrix( g_hmoldView, &m_oldView );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : view set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//DbgOut( "d3ddisp : RenderTriPhong\n" );
	hres = g_pEffect->SetValue( g_hmFogParams, fogparams, sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriPhong : fogparams set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int uselightnum = 0;
	float fsetlno;
	if( (g_renderstate[D3DRS_LIGHTING] == FALSE) || (lightflag == 0) ){
		uselightnum = 0;
		fsetlno = -1.0f;//<--- lighting 無効のサイン
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		uselightnum = g_lightnum;
		fsetlno = (float)g_lightnum;
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&m_EyePt), sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriPhong : LightSpecular set error !!!\n" );
		_ASSERT( 0 );
		return 1;	
	}

	if( (g_usePS == 1) && (toonparams) ){
		hres = g_pEffect->SetValue( g_hmToonParams, toonparams, sizeof( float ) * 4 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : toonparams set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmMinAlpha, bluralpha, sizeof( float ) * 2 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriPhong : mMinAlpha SetValue error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


///////////////
	int passno = 0;
	UINT cPasses;

	
	int renderno;
	int befblno = -1;
	for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + renderno;

		int blno;
		blno = currb->skinno;

		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		char* curtexname = 0;
		int newrapstate;

//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );

//		LPDIRECT3DTEXTURE9 curtex;
//		curtex = (ptoon1 + currb->materialno)->tex;
//		pd3dDevice->SetTexture( 0, curtex );//!!!!!!!!!

		CMQOMaterial* curmqomat;
		curmqomat = currb->mqomat;

		if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
			if( setalphaflag == 0 ){
				if( (withalpha == 0) && ( (curmqomat->col.a != 1.0f) || (curmqomat->transparent != 0) ) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
				if( (withalpha != 0) && (curmqomat->col.a == 1.0f) && (curmqomat->transparent == 0) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
			}else{
				if( (withalpha == 0) && ( (alpha != 1.0f) || (curmqomat->transparent != 0) ) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
				if( (withalpha != 0) && (alpha == 1.0f) && (curmqomat->transparent == 0) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
			}
		}
		
		if( glowflag ){
			if( (curmqomat->exttexmode != EXTTEXMODE_GLOW) && (curmqomat->exttexmode != EXTTEXMODE_GLOWALPHA) ){
				continue;//!!!!!!!!!!!!!!!!!!!!!
			}
		}

		if( glowflag ){
			hres = g_pEffect->SetValue( g_hmGlowMult, curmqomat->glowmult, sizeof( float ) * 3 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : SetValue glowmult error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			//DbgOut( "check !!! d3ddisp : glowmult %f %f %f\r\n", curmqomat->glowmult[0], curmqomat->glowmult[1], curmqomat->glowmult[2] );
		}

		if( srcenabletex ){
			if( curmqomat->curtexname == 0 ){
				if( curmqomat->tex[0] ){
					curtexname = curmqomat->tex;
				}else{
					curtexname = 0;
				}
			}else{
				if( *curmqomat->curtexname == 0 ){
					if( curmqomat->tex[0] ){
						curtexname = curmqomat->tex;
					}else{
						curtexname = 0;
					}
				}else{					
					curtexname = curmqomat->curtexname;
				}
			}
		}else{
			curtexname = 0;
		}

		switch( curmqomat->texrule ){
		case TEXRULE_X:
		case TEXRULE_Y:
		case TEXRULE_Z:
			newrapstate = 0;
			break;
		case TEXRULE_CYLINDER:
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_SPHERE:
			//newrapstate = D3DWRAP_U | D3DWRAP_V;
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_MQ:
		default:
			newrapstate = 0;
			break;
		}
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_WRAP0, newrapstate );



		LPDIRECT3DTEXTURE9 curtex = NULL;		
		curtex = g_texbnk->GetTexData( curtexname, curmqomat->transparent );


		LPDIRECT3DTEXTURE9 bumptex = NULL;
		if( curmqomat->bump[0] != 0 ){
			bumptex = g_texbnk->GetTexData( curmqomat->bump, 0 );
		}

		int bumpflag;
		if( curtex && bumptex && g_bumpUse ){
			bumpflag = 1;
		}else{
			bumpflag = 0;
		}


		if( bumpflag == 0 ){
			if( g_curtex0 != curtex ){
				pd3dDevice->SetTexture( 0, curtex );
				g_curtex0 = curtex;
			}
			if( g_curtex1 != NULL ){
				pd3dDevice->SetTexture( 1, NULL );
				g_curtex1 = NULL;
			}
			if( g_cop0 != D3DTOP_MODULATE ){
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				g_cop0 = D3DTOP_MODULATE;
			}
			if( g_cop1 = D3DTOP_DISABLE ){
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
				g_cop1 = D3DTOP_DISABLE;
			}
			if( g_aop0 != D3DTOP_MODULATE ){
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
				g_aop0 = D3DTOP_MODULATE;
			}
//			if( g_aop0 != D3DTOP_SELECTARG1 ){
//				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//				g_aop0 = D3DTOP_MODULATE;
//			}


			//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			if( g_aop1 != D3DTOP_DISABLE ){
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
				g_aop1 = D3DTOP_DISABLE;
			}

			hres = g_pEffect->SetTexture( g_hDecaleTex, curtex );
			_ASSERT( hres == D3D_OK );

		}else{
			hres = g_pEffect->SetTexture( g_hNormalMap, bumptex );
			_ASSERT( hres == D3D_OK );
			hres = g_pEffect->SetTexture( g_hDecaleTex, curtex );
			_ASSERT( hres == D3D_OK );

		}

		if( blurflag == 0 ){
			if( withalpha != 0 ){
				if( (curmqomat->blendmode == 0) || (curmqomat->blendmode == 100) ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				}else if( (curmqomat->blendmode == 1) || (curmqomat->blendmode == 101) ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ONE );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				}else if( (curmqomat->blendmode == 2) || (curmqomat->blendmode == 102) ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				}else if( curmqomat->blendmode == 103 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ZERO );
				}else if( curmqomat->blendmode == 104 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ZERO );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				}else if( curmqomat->blendmode == 105 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				}else if( curmqomat->blendmode == 106 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				}else if( curmqomat->blendmode == 107 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
				}else if( curmqomat->blendmode == 108 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
				}

				if( glowflag == 0 ){
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest1 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval1 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				}
			}else{
				if( glowflag == 0 ){
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest0 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval0 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				}
			}
		}else{
			SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHABLENDENABLE, TRUE );
			SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
			SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		if( setalphaflag == 0 ){
			hres = g_pEffect->SetFloat( g_hmAlpha, curmqomat->col.a );
		}else{
			hres = g_pEffect->SetFloat( g_hmAlpha, alpha );
		}

//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;
//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//g_cop0 = D3DTOP_MODULATE;
//pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
//g_cop1 = D3DTOP_DISABLE;
//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
//g_aop0 = D3DTOP_MODULATE;
//pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
//g_aop1 = D3DTOP_DISABLE;
//DbgOut( "check!!!: d3ddisp : phongtri : withalpha %d, matname %s, setalphaflag %d, matalpha %f, alpha %f\r\n",
//	withalpha, curmqomat->name, setalphaflag, curmqomat->col.a, alpha );


		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( bumpflag ){

			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Bump );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( curskinnum == 2 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Bump );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Bump );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				if( glowflag ){
					if( curmqomat->exttexmode == EXTTEXMODE_GLOW ){
						if( g_bumplightnum <= 1 ){
							passno = 9;
						}else if( g_bumplightnum == 2 ){
							passno = 10;
						}else{
							passno = 11;
						}
					}else{
						if( g_bumplightnum <= 1 ){
							passno = 12;
						}else if( g_bumplightnum == 2 ){
							passno = 13;
						}else{
							passno = 14;
						}
					}
				}else{
					if( g_bumplightnum <= 1 ){
						passno = 0;
					}else if( g_bumplightnum == 2 ){
						passno = 1;
					}else{
						passno = 2;
					}
				}
			}else if( m_blurmode == BLUR_WORLD ){
				if( g_bumplightnum <= 1 ){
					passno = 3;
				}else if( g_bumplightnum == 2 ){
					passno = 4;
				}else{
					passno = 5;
				}
			}else{
				if( g_bumplightnum <= 1 ){
					passno = 6;
				}else if( g_bumplightnum == 2 ){
					passno = 7;
				}else{
					passno = 8;
				}
			}

		}else if( (lpsh->m_shader == COL_TOON0) && (g_usePS == 1) ){

			if( curtex != NULL ){
				if( curskinnum == 1 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Toon );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( curskinnum == 2 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Toon );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Toon );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}else{
				if( curskinnum == 1 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone0ToonNoTex );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( curskinnum == 2 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone1ToonNoTex );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone4ToonNoTex );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				if( curtex ){
					if( glowflag != 0 ){
						if( curmqomat->exttexmode == EXTTEXMODE_GLOW ){
							passno = 3;
						}else{
							passno = 4;
						}
					}else{
						passno = 0;
					}
				}else{
					passno = 0;
				}
			}else if( m_blurmode == BLUR_WORLD ){
				passno = 1;
			}else{
				passno = 2;
			}

		}else if( lpsh->m_shader == COL_PPHONG ){
			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0PP );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4PP );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				if( glowflag ){
					if( curtex ){
						if( curmqomat->exttexmode == EXTTEXMODE_GLOW ){
							if( uselightnum <= 1 ){
								passno = 18;
							}else if( uselightnum == 2 ){
								passno = 19;
							}else{
								passno = 20;
							}
						}else{
							if( uselightnum <= 1 ){
								passno = 21;
							}else if( uselightnum == 2 ){
								passno = 22;
							}else{
								passno = 23;
							}
						}					
					}else{
						if( uselightnum <= 1 ){
							passno = 3;
						}else if( uselightnum == 2 ){
							passno = 4;
						}else{
							passno = 5;
						}
					}
				}else{
					if( curtex ){
						if( uselightnum <= 1 ){
							passno = 0;
						}else if( uselightnum == 2 ){
							passno = 1;
						}else{
							passno = 2;
						}
					}else{
						if( uselightnum <= 1 ){
							passno = 3;
						}else if( uselightnum == 2 ){
							passno = 4;
						}else{
							passno = 5;
						}
					}
				}
			}else if( m_blurmode == BLUR_WORLD ){
				if( curtex ){
					if( uselightnum <= 1 ){
						passno = 6;
					}else if( uselightnum == 2 ){
						passno = 7;
					}else{
						passno = 8;
					}
				}else{
					if( uselightnum <= 1 ){
						passno = 9;
					}else if( uselightnum == 2 ){
						passno = 10;
					}else{
						passno = 11;
					}
				}
			}else{
				if( curtex ){
					if( uselightnum <= 1 ){
						passno = 12;
					}else if( uselightnum == 2 ){
						passno = 13;
					}else{
						passno = 14;
					}
				}else{
					if( uselightnum <= 1 ){
						passno = 15;
					}else if( uselightnum == 2 ){
						passno = 16;
					}else{
						passno = 17;
					}
				}
			}

		}else{
			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( curskinnum == 2 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone1 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				if( glowflag ){
					if( curtex ){
						if( curmqomat->exttexmode == EXTTEXMODE_GLOW ){
							passno = 3;
						}else{
							passno = 4;
						}					
					}else{
						passno = 0;
					}
				}else{
					passno = 0;
				}
			}else if( m_blurmode == BLUR_WORLD ){
				passno = 1;
			}else{
				passno = 2;
			}
		}

		if( blno != befblno ){
			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
			}else{
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, MAXSKINMATRIX + 1 );
			}
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : worldmatrixarray set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			befblno = blno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		}

		/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->BeginPass( passno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect BeginPass error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		// The effect interface queues up the changes and performs them 
		// with the CommitChanges call. You do not need to call CommitChanges if 
		// you are not setting any parameters between the BeginPass and EndPass.
		// V( g_pEffect->CommitChanges() );

		int curnumprim;
		curnumprim = currb->endface - currb->startface;

		if( curnumprim > 0 ){
			if( bumpflag == 0 ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
			}else{
				pd3dDevice->SetVertexDeclaration( m_pskintandecl );
				pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				pd3dDevice->SetStreamSource( 2, m_VBtan, 0, sizeof( SKINTANGENT ) );
			}
			pd3dDevice->SetIndices( m_IB );

			hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb->startface * 3,
												curnumprim
												);
			if( hres != D3D_OK ){
				_ASSERT( 0 );
			}
			//_ASSERT( hres == D3D_OK );
		}

		if( m_revArrayVB && (curnumprim > 0) ){
			if( bumpflag == 0 ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
				pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
			}else{
				pd3dDevice->SetVertexDeclaration( m_pskintandecl );
				pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
				pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
				pd3dDevice->SetStreamSource( 2, m_revVBtan, 0, sizeof( SKINTANGENT ) );
			}
			pd3dDevice->SetIndices( m_revIB );
			hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											0,
											m_numTLV,
											currb->startface * 3,
											curnumprim
											);
			if( hres != D3D_OK ){
				_ASSERT( 0 );
			}

			//_ASSERT( hres == D3D_OK );
		}

		hres = g_pEffect->EndPass();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect EndPass error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}	

	goto phongedge0;

//////////////
//////////////
//////////// edge
phongedge0:

	
	UINT cPasses2;
	if( ((m_blurmode == BLUR_NONE) || (blurflag == 0)) && lpsh->m_phongedge0enable && 
		( ( (withalpha == 0) && (lpsh->m_phongedge0Blend == 3) ) || ( (withalpha != 0) && (lpsh->m_phongedge0Blend != 3) ) ) && 
		(glowflag == 0) ){

		if( g_cop0 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_DISABLE );
			g_cop0 = D3DTOP_DISABLE;
		}
		if( g_aop0 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
			g_aop0 = D3DTOP_DISABLE;
		}
		if( g_cop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE );
			g_cop1 = D3DTOP_DISABLE;
		}
		if( g_aop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
			g_aop1 = D3DTOP_DISABLE;
		}
		if( g_curtex0 != NULL ){
			pd3dDevice->SetTexture( 0, NULL );
			g_curtex0 = NULL;
		}
		if( g_curtex1 != NULL ){
			pd3dDevice->SetTexture( 1, NULL );
			g_curtex1 = NULL;
		}
			

		if( withalpha != 0 ){
			if( lpsh->m_phongedge0Blend == 0 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				//SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, FALSE );						
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, TRUE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, 0x08 );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

			}else if( lpsh->m_phongedge0Blend == 1 ){
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ONE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, TRUE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, 0x0 );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
			}else if( lpsh->m_phongedge0Blend == 2 ){
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				//SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, FALSE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, TRUE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, 0x0 );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
			}
		}

		hres = g_pEffect->SetFloat( g_hmAlpha, lpsh->m_phongedge0Alpha );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->SetValue( g_hmEdgeCol0, &(lpsh->m_phongedge0[0]), sizeof( float ) * 4 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mEdgeCol0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int renderno2;
		int befblno2 = -1;
		for( renderno2 = 0; renderno2 < mrenderblocknum; renderno2++ ){
			RENDERBLOCK* currb2;
			currb2 = mrenderblock + renderno2;

			int blno2;
			blno2 = currb2->skinno;

			D3DXMATRIX* curskinmat = *(mskinmat + blno2);
			int curskinnum = *(mskinmatnum + blno2);

	//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
	//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );


			////////////
			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( curskinnum == 2 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( blno2 != befblno2 ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : edge worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				befblno2 = blno2;
			}

			/////////
			hres = g_pEffect->Begin(&cPasses2, 0);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : effect Begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//for (iPass2 = 0; iPass2 < cPasses2; iPass2++){
				hres = g_pEffect->BeginPass( 0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : effect BeginPass error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


				// The effect interface queues up the changes and performs them 
				// with the CommitChanges call. You do not need to call CommitChanges if 
				// you are not setting any parameters between the BeginPass and EndPass.
				// V( g_pEffect->CommitChanges() );

				int curnumprim;
				curnumprim = currb2->endface - currb2->startface;

				if( curnumprim > 0 ){

					pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
					pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno2), 0, sizeof(SKINVERTEX) );
					//pd3dDevice->SetIndices( m_IB );
					pd3dDevice->SetIndices( m_revIB );

					hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb2->startface * 3,
													curnumprim
													);
					if( hres != D3D_OK ){
						_ASSERT( 0 );
					}

				}

				hres = g_pEffect->EndPass();
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriPhong : effect EndPass error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			//}
			hres = g_pEffect->End();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : effect End error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return ret;
}

int CD3DDisp::RenderZNPhong( int znflag, float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CHKALPHA* chkalpha )
{
	HRESULT hres;
	int ret = 0;
	static int dbgcnt = 0;

	if( setalphaflag == 0 ){
		if( (withalpha == 0) && (chkalpha->notalphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		if( (withalpha != 0) && (chkalpha->alphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}
	}else{
		if( (withalpha == 0) && (alpha != 1.0f ) && (chkalpha->notalphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		if( (withalpha != 0) && (alpha == 1.0f) && (chkalpha->alphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}			
	}
	

	int uselightnum = 0;

///////////////
	int passno = 0;
	UINT cPasses;

	
	int renderno;
	int befblno = -1;
	for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + renderno;

		int blno;
		blno = currb->skinno;

		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		CMQOMaterial* curmqomat;
		curmqomat = currb->mqomat;

		if( setalphaflag == 0 ){
			if( (withalpha == 0) && ( (curmqomat->col.a != 1.0f) || (curmqomat->transparent != 0) ) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (curmqomat->col.a == 1.0f) && (curmqomat->transparent == 0) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
		}else{
			if( (withalpha == 0) && ( (alpha != 1.0f) || (curmqomat->transparent != 0) ) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (alpha == 1.0f) && (curmqomat->transparent == 0) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
		}

		SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, FALSE );

		if( setalphaflag == 0 ){
			hres = g_pEffect->SetFloat( g_hmAlpha, curmqomat->col.a );
		}else{
			hres = g_pEffect->SetFloat( g_hmAlpha, alpha );
		}
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curskinnum == 1 ){
			hres = g_pEffect->SetTechnique( g_hRenderZN_2path_NormalNoBone );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderZNPhong : SetTechnique error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			hres = g_pEffect->SetTechnique( g_hRenderZN_2path_NormalBone );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderZNPhong : SetTechnique error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		if( blno != befblno ){
			hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriPhong : worldmatrixarray set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			befblno = blno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int passno;
		if( znflag == 1 ){
			passno = 0;
		}else if( znflag == 2 ){
			passno = 1;
		}else if( znflag == 3 ){
			passno = 2;
		}else if( znflag == 101 ){
			passno = 3;
		}else if( znflag == 102 ){
			passno = 4;
		}else if( znflag == 103 ){
			passno = 5;
		}else if( znflag == 105 ){
			passno = 6;
		}else{
			_ASSERT( 0 );
			passno = 0;
		}


		hres = g_pEffect->BeginPass( passno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect BeginPass error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int curnumprim;
		curnumprim = currb->endface - currb->startface;

//DbgOut( "check!!!: d3ddisp : blno %d, curnumprim %d, numTLV %d, tima %x, arrayVB %x, vbmaterial %x, m_VBmaterial %x, IB %x\r\n", 
//	   blno, curnumprim, m_numTLV, m_pskindecl_tima, *(m_ArrayVB + blno), m_VBmaterial, m_IB );		

		if( curnumprim > 0 ){
			pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
			pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
			pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
			pd3dDevice->SetIndices( m_IB );

			hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb->startface * 3,
												curnumprim
												);
			//_ASSERT( hres == D3D_OK );
		}
		if( m_revArrayVB && (curnumprim > 0) ){
			pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
			pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
			pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
			pd3dDevice->SetIndices( m_revIB );
			hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											0,
											m_numTLV,
											currb->startface * 3,
											curnumprim
											);
			//_ASSERT( hres == D3D_OK );
		}
		hres = g_pEffect->EndPass();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect EndPass error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriPhong : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}	

	return 0;
}
int CD3DDisp::RenderZNToon1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CHKALPHA* chkalpha )
{

	return 0;
}



int CD3DDisp::RenderToon1ShadowMap1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* fogparams, 
		CToon1Params* ptoon1, int lightflag, int srcenabletex, CHKALPHA* chkalpha )
{

	HRESULT hres;
	int ret = 0;


	if( setalphaflag == 0 ){
		if( (withalpha == 0) && (chkalpha->notalphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		if( (withalpha != 0) && (chkalpha->alphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}
	}else{
		if( (withalpha == 0) && (alpha != 1.0f ) && (chkalpha->notalphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		if( (withalpha != 0) && (alpha == 1.0f) && (chkalpha->alphanum == 0) ){
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
		}			
	}




//DbgOut( "d3ddisp : RenderTriPhong\n" );
	hres = g_pEffect->SetValue( g_hmFogParams, fogparams, sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderToon1ShadowMap1 : fogparams set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fsetlno;
	if( (g_renderstate[D3DRS_LIGHTING] == FALSE) || (lightflag == 0) ){
		fsetlno = -1.0f;//<--- lighting 無効のサイン
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderToon1ShadowMap1 : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		fsetlno = (float)g_lightnum;
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderToon1ShadowMap1 : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&m_EyePt), sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderToon1ShadowMap1 : LightSpecular set error !!!\n" );
		_ASSERT( 0 );
		return 1;	
	}

//	if( (g_usePS == 1) && (toonparams) ){
//		hres = g_pEffect->SetValue( g_hmToonParams, toonparams, sizeof( float ) * 4 );
//		if( hres != D3D_OK ){
//			DbgOut( "d3ddisp : RenderTriToon1 : toonparams set error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}

///////////////
	UINT cPasses, iPass;
	UINT cPasses2, iPass2;

	int renderno;
	int befblno = -1;
	for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + renderno;

		int blno;
		blno = currb->skinno;

		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		char* curtexname = 0;
		//int newrapstate;

//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );


		CMQOMaterial* curmqomat;
		curmqomat = currb->mqomat;

		if( setalphaflag == 0 ){
			if( (withalpha == 0) && ( (curmqomat->col.a != 1.0f) || (curmqomat->transparent != 0) ) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (curmqomat->col.a == 1.0f) && (curmqomat->transparent == 0) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
		}else{
			if( (withalpha == 0) && ( (alpha != 1.0f) || (curmqomat->transparent != 0) ) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (alpha == 1.0f) && (curmqomat->transparent == 0) ){
				continue;//!!!!!!!!!!!!!!!!!!!!
			}
		}

		if( srcenabletex ){
			if( curmqomat->curtexname == 0 ){
				if( curmqomat->tex[0] ){
					curtexname = curmqomat->tex;
				}else{
					curtexname = 0;
				}
			}else{
				if( *curmqomat->curtexname == 0 ){
					if( curmqomat->tex[0] ){
						curtexname = curmqomat->tex;
					}else{
						curtexname = 0;
					}
				}else{					
					curtexname = curmqomat->curtexname;
				}
			}
		}else{
			curtexname = 0;
		}

		LPDIRECT3DTEXTURE9 curtex1 = NULL;		
		curtex1 = g_texbnk->GetTexData( curtexname, curmqomat->transparent );
		hres = g_pEffect->SetTexture( g_hDecaleTex, curtex1 );
		_ASSERT( hres == D3D_OK );

		if( withalpha != 0 ){
			if( (curmqomat->blendmode == 0) || (curmqomat->blendmode == 100) ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}else if( (curmqomat->blendmode == 1) || (curmqomat->blendmode == 101) ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ONE );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
			}else if( (curmqomat->blendmode == 2) || (curmqomat->blendmode == 102) ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
			}else if( curmqomat->blendmode == 103 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ZERO );
			}else if( curmqomat->blendmode == 104 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ZERO );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			}else if( curmqomat->blendmode == 105 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
			}else if( curmqomat->blendmode == 106 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			}else if( curmqomat->blendmode == 107 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
			}else if( curmqomat->blendmode == 108 ){
				SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
			}

			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest1 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval1 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

		}else{
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest0 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval0 );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}


		if( setalphaflag == 0 ){
			hres = g_pEffect->SetFloat( g_hmAlpha, curmqomat->col.a );
		}else{
			hres = g_pEffect->SetFloat( g_hmAlpha, alpha );
		}
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		hres = g_pEffect->SetTechnique( g_hRenderSceneToon0Shadow );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : SetTechnique error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( blno != befblno ){
			hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : worldmatrixarray set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			befblno = blno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		}

		float tmpfl[4];
		tmpfl[0] = 0.0f;
		tmpfl[1] = 0.0f;
		tmpfl[2] = (ptoon1 + currb->materialno)->toon0dnl;
		tmpfl[3] = (ptoon1 + currb->materialno)->toon0bnl;
		hres = g_pEffect->SetValue( g_hmToonParams, tmpfl, sizeof( float ) * 4 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toon1 params set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpdif[3];
		tmpdif[0] = (ptoon1 + currb->materialno)->diffuse.r;
		tmpdif[1] = (ptoon1 + currb->materialno)->diffuse.g;
		tmpdif[2] = (ptoon1 + currb->materialno)->diffuse.b;
		hres = g_pEffect->SetValue( g_hmToonDiffuse, tmpdif, sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toonDiffuse set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpamb[3];
		tmpamb[0] = (ptoon1 + currb->materialno)->ambient.r;
		tmpamb[1] = (ptoon1 + currb->materialno)->ambient.g;
		tmpamb[2] = (ptoon1 + currb->materialno)->ambient.b;
		hres = g_pEffect->SetValue( g_hmToonAmbient, tmpamb, sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toonambient set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpspc[3];
		tmpspc[0] = (ptoon1 + currb->materialno)->specular.r;
		tmpspc[1] = (ptoon1 + currb->materialno)->specular.g;
		tmpspc[2] = (ptoon1 + currb->materialno)->specular.b;
		hres = g_pEffect->SetValue( g_hmToonSpecular, tmpspc, sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toonspecular set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

			/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		for (iPass = 0; iPass < cPasses; iPass++){
			if( curskinnum == 1 ){
				if( curtex1 ){
					iPass = 2;
				}else{
					iPass = 3;
				}
			}else{
				if( curtex1 ){
					iPass = 0;
				}else{
					iPass = 1;
				}
			}

			hres = g_pEffect->BeginPass(iPass);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			curnumprim = currb->endface - currb->startface;

			if( curnumprim > 0 ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
				pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb->startface * 3,
													curnumprim
													);
				_ASSERT( hres == D3D_OK );
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
				pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );		
				pd3dDevice->SetIndices( m_revIB );
				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb->startface * 3,
												curnumprim
												);
				_ASSERT( hres == D3D_OK );
			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		//}
		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}				
	}	




//////////// edge
	if( g_cop0 != D3DTOP_DISABLE ){
		pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_DISABLE );
		g_cop0 = D3DTOP_DISABLE;
	}
	if( g_aop0 != D3DTOP_DISABLE ){
		pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
		g_aop0 = D3DTOP_DISABLE;
	}
	if( g_cop1 != D3DTOP_DISABLE ){
		pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE );
		g_cop1 = D3DTOP_DISABLE;
	}
	if( g_aop1 != D3DTOP_DISABLE ){
		pd3dDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
		g_aop1 = D3DTOP_DISABLE;
	}
	if( g_curtex0 != NULL ){
		pd3dDevice->SetTexture( 0, NULL );
		g_curtex0 = NULL;
	}
	if( g_curtex1 != NULL ){
		pd3dDevice->SetTexture( 1, NULL );
		g_curtex1 = NULL;
	}
		

	int renderno2;
	int befblno2 = -1;
	for( renderno2 = 0; renderno2 < mrenderblocknum; renderno2++ ){
		RENDERBLOCK* currb2;
		currb2 = mrenderblock + renderno2;

		int blno2;
		blno2 = currb2->skinno;

		D3DXMATRIX* curskinmat = *(mskinmat + blno2);
		int curskinnum = *(mskinmatnum + blno2);

//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );


		////////////

		if( (ptoon1 + currb2->materialno)->edgevalid0 != 0 ){

			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( curskinnum == 2 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Edge0 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( blno2 != befblno2 ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : edge worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				befblno2 = blno2;
			}


			float edge0[4];
			edge0[0] = (ptoon1 + currb2->materialno)->edgecol0.r;
			edge0[1] = (ptoon1 + currb2->materialno)->edgecol0.g;
			edge0[2] = (ptoon1 + currb2->materialno)->edgecol0.b;
			edge0[3] = (ptoon1 + currb2->materialno)->edgecol0.a;//<--- 内容はwidth

			hres = g_pEffect->SetValue( g_hmEdgeCol0, &(edge0[0]), sizeof( float ) * 4 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : mEdgeCol0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			/////////
			hres = g_pEffect->Begin(&cPasses2, 0);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : effect Begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			for (iPass2 = 0; iPass2 < cPasses2; iPass2++){
				hres = g_pEffect->BeginPass(iPass2);
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : effect BeginPass error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


				// The effect interface queues up the changes and performs them 
				// with the CommitChanges call. You do not need to call CommitChanges if 
				// you are not setting any parameters between the BeginPass and EndPass.
				// V( g_pEffect->CommitChanges() );

				int curnumprim;
				curnumprim = currb2->endface - currb2->startface;

				if( curnumprim > 0 ){

					if( (ptoon1 + currb2->materialno)->edgeinv0 && m_revArrayVB ){
						//裏面
						pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
						pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno2), 0, sizeof(SKINVERTEX) );		
						//pd3dDevice->SetIndices( m_revIB );
						pd3dDevice->SetIndices( m_IB );
						hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb2->startface * 3,
													curnumprim
													);
					}else{

						pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
						pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno2), 0, sizeof(SKINVERTEX) );
						//pd3dDevice->SetIndices( m_IB );
						pd3dDevice->SetIndices( m_revIB );

						hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
														0,
														0,
														m_numTLV,
														currb2->startface * 3,
														curnumprim
														);
						}
				}

				/***
				if( m_revArrayVB && (curnumprim > 0) ){
					pd3dDevice->SetVertexDeclaration( m_pskindecl );
					pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno2), 0, sizeof(SKINVERTEX) );		
					pd3dDevice->SetIndices( m_revIB );
					hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb2->startface * 3,
												curnumprim
												);

				}
				***/
				hres = g_pEffect->EndPass();
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : effect EndPass error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
			hres = g_pEffect->End();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : effect End error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}



	return 0;
}



int CD3DDisp::RenderTriToon1( float alpha, int setalphaflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* toonparams, float* fogparams, 
	CToon1Params* ptoon1, int lightflag,
	int srcenabletex, CHKALPHA* chkalpha, float* bluralpha, int blurflag, int glowflag )
{
	HRESULT hres;
	int ret = 0;
	static int dbgcnt = 0;

	if( glowflag ){
		srcenabletex = 1;//!!!!!!!!!!!!!!!!!
	}

	if( setalphaflag == 0 ){
		if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
			if( (withalpha == 0) && (chkalpha->notalphanum == 0) ){
				return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (chkalpha->alphanum == 0) ){
				return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			}
		}
	}else{
		if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
			if( (withalpha == 0) && (alpha != 1.0f) && (chkalpha->notalphanum == 0) ){
				return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			}
			if( (withalpha != 0) && (alpha == 1.0f) && (chkalpha->alphanum == 0) ){
				return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
			}
		}
	}

//DbgOut( "d3ddisp : RenderTriPhong\n" );
	hres = g_pEffect->SetValue( g_hmFogParams, fogparams, sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriToon1 : fogparams set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fsetlno;
	if( (g_renderstate[D3DRS_LIGHTING] == FALSE) || (lightflag == 0) ){
		fsetlno = -1.0f;//<--- lighting 無効のサイン
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		fsetlno = (float)g_lightnum;
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&m_EyePt), sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriToon1 : LightSpecular set error !!!\n" );
		_ASSERT( 0 );
		return 1;	
	}

	if( blurflag != 0 ){
		hres = g_pEffect->SetMatrix( g_hmoldView, &m_oldView );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : oldview set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmMinAlpha, bluralpha, sizeof( float ) * 2 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTriToon1 : mMinAlpha SetFloat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////////////
	int passno = 0;
	UINT cPasses;
	UINT cPasses2;

	int renderno;
	int befblno = -1;
	for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + renderno;

		int blno;
		blno = currb->skinno;

		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		char* curtexname = 0;
		int newrapstate;

//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );

		CMQOMaterial* curmqomat;
		curmqomat = currb->mqomat;

		if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
			if( setalphaflag == 0 ){
				if( (withalpha == 0) && ( (curmqomat->col.a != 1.0f) || (curmqomat->transparent != 0) ) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
				if( (withalpha != 0) && (curmqomat->col.a == 1.0f) && (curmqomat->transparent == 0) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
			}else{
				if( (withalpha == 0) && ( (alpha != 1.0f) || (curmqomat->transparent != 0) ) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
				if( (withalpha != 0) && (alpha == 1.0f) && (curmqomat->transparent == 0) ){
					continue;//!!!!!!!!!!!!!!!!!!!!
				}
			}
		}

		if( srcenabletex ){
			if( curmqomat->curtexname == 0 ){
				if( curmqomat->tex[0] ){
					curtexname = curmqomat->tex;
				}else{
					curtexname = 0;
				}
			}else{
				if( *curmqomat->curtexname == 0 ){
					if( curmqomat->tex[0] ){
						curtexname = curmqomat->tex;
					}else{
						curtexname = 0;
					}
				}else{					
					curtexname = curmqomat->curtexname;
				}
			}
		}else{
			curtexname = 0;
		}
		
		switch( curmqomat->texrule ){
		case TEXRULE_X:
		case TEXRULE_Y:
		case TEXRULE_Z:
			newrapstate = 0;
			break;
		case TEXRULE_CYLINDER:
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_SPHERE:
			//newrapstate = D3DWRAP_U | D3DWRAP_V;
			newrapstate = D3DWRAP_U;
			break;
		case TEXRULE_MQ:
		default:
			newrapstate = 0;
			break;
		}
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_WRAP0, newrapstate );


		LPDIRECT3DTEXTURE9 curtex1 = NULL;		
		curtex1 = g_texbnk->GetTexData( curtexname, curmqomat->transparent );

		if( glowflag != 0 ){
			if( !curtex1 || ((curmqomat->exttexmode != EXTTEXMODE_GLOW) && (curmqomat->exttexmode != EXTTEXMODE_GLOWALPHA)) ){
				continue;//!!!!!!!!!!!!!!!!!!!!!
			}
		}

		hres = g_pEffect->SetTexture( g_hDecaleTex, curtex1 );
		_ASSERT( !ret );
		g_curtex0 = curtex1;


		if( glowflag ){
			hres = g_pEffect->SetValue( g_hmGlowMult, curmqomat->glowmult, sizeof( float ) * 3 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : SetValue glowmult error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


		if( blurflag == 0 ){
			if( withalpha != 0 ){
				if( (curmqomat->blendmode == 0) || (curmqomat->blendmode == 100) ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				}else if( (curmqomat->blendmode == 1) || (curmqomat->blendmode == 101) ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ONE );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				}else if( (curmqomat->blendmode == 2) || (curmqomat->blendmode == 102) ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				}else if( curmqomat->blendmode == 103 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ZERO );
				}else if( curmqomat->blendmode == 104 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ZERO );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				}else if( curmqomat->blendmode == 105 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
				}else if( curmqomat->blendmode == 106 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				}else if( curmqomat->blendmode == 107 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
				}else if( curmqomat->blendmode == 108 ){
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
				}

				if( glowflag == 0 ){
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest1 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval1 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				}

			}else{
				if( glowflag == 0 ){
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHATESTENABLE, curmqomat->alphatest0 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAREF, curmqomat->alphaval0 );
					SetRenderStateIfNotSame( pd3dDevice,  D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				}
			}
		}else{
			SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHABLENDENABLE, TRUE );
			SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
			SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}


		if( setalphaflag == 0 ){
			hres = g_pEffect->SetFloat( g_hmAlpha, curmqomat->col.a );
		}else{
			hres = g_pEffect->SetFloat( g_hmAlpha, alpha );
		}
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : mAlpha SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curtex1 != NULL ){
			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Toon );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( curskinnum == 2 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Toon );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Toon );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}else{
			if( curskinnum == 1 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone0ToonNoTex );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( curskinnum == 2 ){
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone1ToonNoTex );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetTechnique( g_hRenderSceneBone4ToonNoTex );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

		if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
			if( glowflag ){
				if( curmqomat->exttexmode == EXTTEXMODE_GLOW ){
					passno = 3;
				}else{
					passno = 4;
				}
			}else{
				passno = 0;
			}
		}else if( m_blurmode == BLUR_WORLD ){
			passno = 1;
		}else{
			passno = 2;
		}

		if( blno != befblno ){
			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( m_blurmode == BLUR_WORLD ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, MAXSKINMATRIX + 1 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, MAXSKINMATRIX + 1 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

		float tmpfl[4];
		tmpfl[0] = 0.0f;
		tmpfl[1] = 0.0f;
		tmpfl[2] = (ptoon1 + currb->materialno)->toon0dnl;
		tmpfl[3] = (ptoon1 + currb->materialno)->toon0bnl;
		hres = g_pEffect->SetValue( g_hmToonParams, tmpfl, sizeof( float ) * 4 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toon1 params set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpdif[3];
		tmpdif[0] = (ptoon1 + currb->materialno)->diffuse.r;
		tmpdif[1] = (ptoon1 + currb->materialno)->diffuse.g;
		tmpdif[2] = (ptoon1 + currb->materialno)->diffuse.b;
		hres = g_pEffect->SetValue( g_hmToonDiffuse, tmpdif, sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toonDiffuse set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpamb[3];
		tmpamb[0] = (ptoon1 + currb->materialno)->ambient.r;
		tmpamb[1] = (ptoon1 + currb->materialno)->ambient.g;
		tmpamb[2] = (ptoon1 + currb->materialno)->ambient.b;
		hres = g_pEffect->SetValue( g_hmToonAmbient, tmpamb, sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toonambient set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpspc[3];
		tmpspc[0] = (ptoon1 + currb->materialno)->specular.r;
		tmpspc[1] = (ptoon1 + currb->materialno)->specular.g;
		tmpspc[2] = (ptoon1 + currb->materialno)->specular.b;
		hres = g_pEffect->SetValue( g_hmToonSpecular, tmpspc, sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toonspecular set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

			/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//for (iPass = 0; iPass < cPasses; iPass++){
			hres = g_pEffect->BeginPass( passno );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			curnumprim = currb->endface - currb->startface;

			if( curnumprim > 0 ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
				pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb->startface * 3,
													curnumprim
													);
				_ASSERT( hres == D3D_OK );
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
				pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );		
				pd3dDevice->SetIndices( m_revIB );
				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb->startface * 3,
												curnumprim
												);
				_ASSERT( hres == D3D_OK );
			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTriToon1 : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		//}
		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTriToon1 : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}				
	}	




//////////// edge
	if( ((m_blurmode == BLUR_NONE) || (blurflag == 0)) && (glowflag == 0) ){
		if( g_cop0 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_DISABLE );
			g_cop0 = D3DTOP_DISABLE;
		}
		if( g_aop0 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
			g_aop0 = D3DTOP_DISABLE;
		}
		if( g_cop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE );
			g_cop1 = D3DTOP_DISABLE;
		}
		if( g_aop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
			g_aop1 = D3DTOP_DISABLE;
		}
		if( g_curtex0 != NULL ){
			pd3dDevice->SetTexture( 0, NULL );
			g_curtex0 = NULL;
		}
		if( g_curtex1 != NULL ){
			pd3dDevice->SetTexture( 1, NULL );
			g_curtex1 = NULL;
		}
			

		int renderno2;
		int befblno2 = -1;
		for( renderno2 = 0; renderno2 < mrenderblocknum; renderno2++ ){
			RENDERBLOCK* currb2;
			currb2 = mrenderblock + renderno2;

			int blno2;
			blno2 = currb2->skinno;

			D3DXMATRIX* curskinmat = *(mskinmat + blno2);
			int curskinnum = *(mskinmatnum + blno2);

	//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
	//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );


			////////////

			if( (ptoon1 + currb2->materialno)->edgevalid0 != 0 ){

				if( curskinnum == 1 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Edge0 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriToon1 : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( curskinnum == 2 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Edge0 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriToon1 : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Edge0 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriToon1 : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				if( blno2 != befblno2 ){
					hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriToon1 : edge worldmatrixarray set error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					befblno2 = blno2;
				}


				float edge0[4];
				edge0[0] = (ptoon1 + currb2->materialno)->edgecol0.r;
				edge0[1] = (ptoon1 + currb2->materialno)->edgecol0.g;
				edge0[2] = (ptoon1 + currb2->materialno)->edgecol0.b;
				edge0[3] = (ptoon1 + currb2->materialno)->edgecol0.a;//<--- 内容はwidth

				hres = g_pEffect->SetValue( g_hmEdgeCol0, &(edge0[0]), sizeof( float ) * 4 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : mEdgeCol0 SetValue error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


				/////////
				hres = g_pEffect->Begin(&cPasses2, 0);
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : effect Begin error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				//for (iPass2 = 0; iPass2 < cPasses2; iPass2++){
					hres = g_pEffect->BeginPass( 0 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriToon1 : effect BeginPass error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}


					// The effect interface queues up the changes and performs them 
					// with the CommitChanges call. You do not need to call CommitChanges if 
					// you are not setting any parameters between the BeginPass and EndPass.
					// V( g_pEffect->CommitChanges() );

					int curnumprim;
					curnumprim = currb2->endface - currb2->startface;

					if( curnumprim > 0 ){

						if( (ptoon1 + currb2->materialno)->edgeinv0 && m_revArrayVB ){
							//裏面
							pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
							pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno2), 0, sizeof(SKINVERTEX) );		
							//pd3dDevice->SetIndices( m_revIB );
							pd3dDevice->SetIndices( m_IB );
							hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
														0,
														0,
														m_numTLV,
														currb2->startface * 3,
														curnumprim
														);
						}else{

							pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
							pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno2), 0, sizeof(SKINVERTEX) );
							//pd3dDevice->SetIndices( m_IB );
							pd3dDevice->SetIndices( m_revIB );

							hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
															0,
															0,
															m_numTLV,
															currb2->startface * 3,
															curnumprim
															);
							}
					}

					/***
					if( m_revArrayVB && (curnumprim > 0) ){
						pd3dDevice->SetVertexDeclaration( m_pskindecl );
						pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno2), 0, sizeof(SKINVERTEX) );		
						pd3dDevice->SetIndices( m_revIB );
						hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													currb2->startface * 3,
													curnumprim
													);

					}
					***/
					hres = g_pEffect->EndPass();
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTriToon1 : effect EndPass error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

				//}
				hres = g_pEffect->End();
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTriToon1 : effect End error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return ret;
}






int CD3DDisp::RenderBillboardShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, LPDIRECT3DTEXTURE9 lptex )
{
	HRESULT hres;
	int ret = 0;
	
	hres = g_pEffect->SetFloat( g_hmAlpha, m_alpha );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderBillboardShadowMap0 : mAlpha SetFloat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hres = g_pEffect->SetTexture( g_hDecaleTex, lptex );
	_ASSERT( hres == D3D_OK );


///////////////
	UINT cPasses, iPass;

	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){
		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		hres = g_pEffect->SetTechnique( g_hRenderSceneNormalShadow );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderBillboardShadowMap0 : SetTechnique error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderBillboardShadowMap0 : worldmatrixarray set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderBillboardShadowMap0 : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//for (iPass = 0; iPass < cPasses; iPass++){
			iPass = 4;//!!!!!!!!!!!!

			hres = g_pEffect->BeginPass(iPass);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderBillboardShadowMap0 : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			SKINBLOCK* curskinblock;
			curskinblock = mskinblock + blno;
			curnumprim = curskinblock->endface - curskinblock->startface;

			if( curnumprim > 0 ){
				hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				hres = pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );

				hres = pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												curskinblock->startface * 3,
												curnumprim
												);
				_ASSERT( hres == D3D_OK );
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				hres = pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );		
				hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );

				hres = pd3dDevice->SetIndices( m_revIB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											0,
											m_numTLV,
											curskinblock->startface * 3,
											curnumprim
											);
				_ASSERT( hres == D3D_OK );

			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderBillboardShadowMap0 : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		//}
		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderBillboardShadowMap0 : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return ret;


}

int CD3DDisp::RenderBillboardShadowMap1( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, LPDIRECT3DTEXTURE9 lptex, float* fogparams )
{

	HRESULT hres;
	int ret = 0;

	
//DbgOut( "d3ddisp : RenderTri\n" );
	hres = g_pEffect->SetValue( g_hmFogParams, fogparams, sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTri : fogparams set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hres = g_pEffect->SetFloat( g_hmAlpha, m_alpha );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTri : mAlpha SetFloat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hres = g_pEffect->SetTexture( g_hDecaleTex, lptex );
	_ASSERT( hres == D3D_OK );


	float fsetlno;
	if( (g_renderstate[D3DRS_LIGHTING] == FALSE) ){
		fsetlno = -1.0f;//<--- lighting 無効のサイン
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		fsetlno = (float)g_lightnum;
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&m_EyePt), sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTri : LightSpecular set error !!!\n" );
		_ASSERT( 0 );
		return 1;	
	}

//	if( (g_usePS == 1) && (toonparams) ){
//		hres = g_pEffect->SetValue( g_hmToonParams, toonparams, sizeof( float ) * 4 );
//		if( hres != D3D_OK ){
//			DbgOut( "d3ddisp : RenderTri : toonparams set error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}

///////////////
	UINT cPasses, iPass;
//	UINT cPasses2, iPass2;

	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){
		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		int curskinnum = *(mskinmatnum + blno);

		hres = g_pEffect->SetTechnique( g_hRenderSceneNormalShadow );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	/////////
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : effect Begin error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//for (iPass = 0; iPass < cPasses; iPass++){
			iPass = 6;
			hres = g_pEffect->BeginPass(iPass);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			SKINBLOCK* curskinblock;
			curskinblock = mskinblock + blno;
			curnumprim = curskinblock->endface - curskinblock->startface;

			if( curnumprim > 0 ){
				hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				hres = pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );

				hres = pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												curskinblock->startface * 3,
												curnumprim
												);
				_ASSERT( hres == D3D_OK );
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				hres = pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );		
				hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );

				hres = pd3dDevice->SetIndices( m_revIB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											0,
											m_numTLV,
											curskinblock->startface * 3,
											curnumprim
											);
				_ASSERT( hres == D3D_OK );

			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		//}
		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : effect End error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}


	return 0;

}


int CD3DDisp::RenderTri( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, float* toonparams, LPDIRECT3DTEXTURE9 lptex, float* fogparams, 
	CToon1Params* ptoon1, int lightflag, float* bluralpha, int blurflag, int glowmode, float* glowmult )
{
	HRESULT hres;
	int ret = 0;
	static int dbgcnt = 0;

	
//DbgOut( "d3ddisp : RenderTri\n" );
	hres = g_pEffect->SetValue( g_hmFogParams, fogparams, sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTri : fogparams set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hres = g_pEffect->SetFloat( g_hmAlpha, m_alpha );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTri : mAlpha SetFloat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hres = g_pEffect->SetValue( g_hmMinAlpha, bluralpha, sizeof( float ) * 2 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTri : mMinAlpha SetFloat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int uselightnum = 0;
	float fsetlno;
	if( (g_renderstate[D3DRS_LIGHTING] == FALSE) || (lightflag == 0) ){
		uselightnum = 0;
		fsetlno = -1.0f;//<--- lighting 無効のサイン
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		uselightnum = g_lightnum;
		fsetlno = (float)g_lightnum;
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&m_EyePt), sizeof( float ) * 3 );
	if( hres != D3D_OK ){
		DbgOut( "d3ddisp : RenderTri : LightSpecular set error !!!\n" );
		_ASSERT( 0 );
		return 1;	
	}

	if( (g_usePS == 1) && (toonparams) ){
		hres = g_pEffect->SetValue( g_hmToonParams, toonparams, sizeof( float ) * 4 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : toonparams set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( blurflag != 0 ){
		hres = g_pEffect->SetMatrix( g_hmoldView, &m_oldView );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : view set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( glowmode != 0 ){
		if( glowmult ){
			hres = g_pEffect->SetValue( g_hmGlowMult, glowmult, sizeof( float ) * 3 );
		}else{
			float tempmult[3];
			tempmult[0] = 1.0f;
			tempmult[1] = 1.0f;
			tempmult[2] = 1.0f;
			hres = g_pEffect->SetValue( g_hmGlowMult, tempmult, sizeof( float ) * 3 );
		}
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : SetValue glowmult error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


///////////////
	int passno = 0;
	UINT cPasses;
	UINT cPasses2, iPass2;

	if( (lpsh->m_shader != COL_TOON1) || (toonparams == 0) ){

		hres = g_pEffect->SetTexture( g_hDecaleTex, lptex );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : SetTexture DecaleTex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int blno;
		for( blno = 0; blno < mskinblocknum; blno++ ){
			D3DXMATRIX* curskinmat = *(mskinmat + blno);
			int curskinnum = *(mskinmatnum + blno);

			if( (lpsh->m_shader == COL_TOON0) && (g_usePS == 1) ){

				if( lptex != NULL ){
					if( curskinnum == 1 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Toon );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else if( curskinnum == 2 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Toon );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Toon );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}else{
					if( curskinnum == 1 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone0ToonNoTex );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else if( curskinnum == 2 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone1ToonNoTex );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone4ToonNoTex );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
			}else if( lpsh->m_shader == COL_PPHONG ){
				if( curskinnum == 1 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone0PP );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone4PP );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}				
			}else{
				if( curskinnum == 1 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone0 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( curskinnum == 2 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone1 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone4 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( lpsh->m_shader == COL_PPHONG ){
					if( lptex ){
						if( glowmode == 0 ){
							if( uselightnum <= 1 ){
								passno = 0;	
							}else if( uselightnum == 2 ){
								passno = 1;
							}else if( uselightnum >= 3 ){
								passno = 2;
							}
						}else if( glowmode == EXTTEXMODE_GLOW ){
							if( uselightnum <= 1 ){
								passno = 18;	
							}else if( uselightnum == 2 ){
								passno = 19;
							}else if( uselightnum >= 3 ){
								passno = 20;
							}
						}else if( glowmode == EXTTEXMODE_GLOWALPHA ){
							if( uselightnum <= 1 ){
								passno = 21;	
							}else if( uselightnum == 2 ){
								passno = 22;
							}else if( uselightnum >= 3 ){
								passno = 23;
							}
						}else{
							_ASSERT( 0 );
							return 1;//!!!!!!!
						}
					}else{
						if( uselightnum <= 1 ){
							passno = 3;	
						}else if( uselightnum == 2 ){
							passno = 4;
						}else if( uselightnum >= 3 ){
							passno = 5;
						}
					}

				}else{	
					if( lptex ){
						if( glowmode == 0 ){
							passno = 0;
						}else if( glowmode == EXTTEXMODE_GLOW ){
							passno = 3;
						}else if( glowmode == EXTTEXMODE_GLOWALPHA ){
							passno = 4;
						}else{
							passno = 0;
						}
					}else{
						passno = 0;
					}
				}
			}else if( m_blurmode == BLUR_WORLD ){
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, MAXSKINMATRIX + 1 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( lpsh->m_shader == COL_PPHONG ){
					if( lptex ){
						if( uselightnum <= 1 ){
							passno = 6;
						}else if( uselightnum == 2 ){
							passno = 7;
						}else if( uselightnum >= 3 ){
							passno = 8;
						}
					}else{
						if( uselightnum <= 1 ){
							passno = 9;
						}else if( uselightnum == 2 ){
							passno = 10;
						}else if( uselightnum >= 3 ){
							passno = 11;
						}
					}
				}else{
					passno = 1;
				}
			}else{
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, MAXSKINMATRIX + 1 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( lpsh->m_shader == COL_PPHONG ){
					if( lptex ){
						if( uselightnum <= 1 ){
							passno = 12;
						}else if( uselightnum == 2 ){
							passno = 13;
						}else if( uselightnum >= 3 ){
							passno = 14;
						}
					}else{
						if( uselightnum <= 1 ){
							passno = 15;
						}else if( uselightnum == 2 ){
							passno = 16;
						}else if( uselightnum >= 3 ){
							passno = 17;
						}
					}
				}else{
					passno = 2;
				}
			}


		/////////
			hres = g_pEffect->Begin(&cPasses, 0);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect Begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->BeginPass( passno );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			SKINBLOCK* curskinblock;
			curskinblock = mskinblock + blno;
			curnumprim = curskinblock->endface - curskinblock->startface;

			if( curnumprim > 0 ){
				hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				hres = pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );

				hres = pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												curskinblock->startface * 3,
												curnumprim
												);
				_ASSERT( hres == D3D_OK );
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
				hres = pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );		
				hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );

				hres = pd3dDevice->SetIndices( m_revIB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											0,
											m_numTLV,
											curskinblock->startface * 3,
											curnumprim
											);
				_ASSERT( hres == D3D_OK );

			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->End();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect End error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}else{

		hres = g_pEffect->SetTexture( g_hDecaleTex, lptex );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : SetTexture DecaleTex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int renderno;
		int befblno = -1;
		for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
			RENDERBLOCK* currb;
			currb = mrenderblock + renderno;

			int blno;
			blno = currb->skinno;

			D3DXMATRIX* curskinmat = *(mskinmat + blno);
			int curskinnum = *(mskinmatnum + blno);

//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );


			if( lptex != NULL ){
				if( curskinnum == 1 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Toon );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( curskinnum == 2 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Toon );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Toon );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}else{
				if( curskinnum == 1 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone0ToonNoTex );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( curskinnum == 2 ){
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone1ToonNoTex );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetTechnique( g_hRenderSceneBone4ToonNoTex );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
				if( lptex ){
					if( glowmode == 0 ){
						passno = 0;
					}else if( glowmode == EXTTEXMODE_GLOW ){
						passno = 3;
					}else if( glowmode == EXTTEXMODE_GLOWALPHA ){
						passno = 4;
					}else{
						passno = 0;
					}
				}else{
					passno = 0;
				}
			}else if( m_blurmode == BLUR_WORLD ){
				passno = 1;
			}else{
				passno = 2;
			}

			if( blno != befblno ){
				if( (m_blurmode == BLUR_NONE) || (blurflag == 0) ){
					hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( m_blurmode == BLUR_WORLD ){
					hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, MAXSKINMATRIX + 1 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, MAXSKINMATRIX + 1 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			float tmpfl[4];
			tmpfl[0] = 0.0f;
			tmpfl[1] = 0.0f;
			tmpfl[2] = (ptoon1 + currb->materialno)->toon0dnl;
			tmpfl[3] = (ptoon1 + currb->materialno)->toon0bnl;
			hres = g_pEffect->SetValue( g_hmToonParams, tmpfl, sizeof( float ) * 4 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : toon1 params set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			float tmpdif[3];
			tmpdif[0] = (ptoon1 + currb->materialno)->diffuse.r;
			tmpdif[1] = (ptoon1 + currb->materialno)->diffuse.g;
			tmpdif[2] = (ptoon1 + currb->materialno)->diffuse.b;
			hres = g_pEffect->SetValue( g_hmToonDiffuse, tmpdif, sizeof( float ) * 3 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : toonDiffuse set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			float tmpamb[3];
			tmpamb[0] = (ptoon1 + currb->materialno)->ambient.r;
			tmpamb[1] = (ptoon1 + currb->materialno)->ambient.g;
			tmpamb[2] = (ptoon1 + currb->materialno)->ambient.b;
			hres = g_pEffect->SetValue( g_hmToonAmbient, tmpamb, sizeof( float ) * 3 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : toonambient set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			float tmpspc[3];
			tmpspc[0] = (ptoon1 + currb->materialno)->specular.r;
			tmpspc[1] = (ptoon1 + currb->materialno)->specular.g;
			tmpspc[2] = (ptoon1 + currb->materialno)->specular.b;
			hres = g_pEffect->SetValue( g_hmToonSpecular, tmpspc, sizeof( float ) * 3 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : toonspecular set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			/////////
			hres = g_pEffect->Begin(&cPasses, 0);
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect Begin error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->BeginPass( passno );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect BeginPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			// The effect interface queues up the changes and performs them 
			// with the CommitChanges call. You do not need to call CommitChanges if 
			// you are not setting any parameters between the BeginPass and EndPass.
			// V( g_pEffect->CommitChanges() );

			int curnumprim;
			curnumprim = currb->endface - currb->startface;

			if( curnumprim > 0 ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
				pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
				pd3dDevice->SetIndices( m_IB );

				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
												0,
												0,
												m_numTLV,
												currb->startface * 3,
												curnumprim
												);
			}

			if( m_revArrayVB && (curnumprim > 0) ){
				pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
				pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );		
				pd3dDevice->SetIndices( m_revIB );
				hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											0,
											m_numTLV,
											currb->startface * 3,
											curnumprim
											);

			}

			hres = g_pEffect->EndPass();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect EndPass error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			hres = g_pEffect->End();
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : effect End error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			befblno = blno;
				
		}


		
//////////// edge
		if( ((m_blurmode == BLUR_NONE) || (blurflag == 0)) && (glowmode == 0) ){
			if( g_cop0 != D3DTOP_DISABLE ){
				pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_DISABLE );
				g_cop0 = D3DTOP_DISABLE;
			}
			if( g_aop0 != D3DTOP_DISABLE ){
				pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
				g_aop0 = D3DTOP_DISABLE;
			}
			if( g_cop1 != D3DTOP_DISABLE ){
				pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE );
				g_cop1 = D3DTOP_DISABLE;
			}
			if( g_aop1 != D3DTOP_DISABLE ){
				pd3dDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_DISABLE );
				g_aop1 = D3DTOP_DISABLE;
			}
			if( g_curtex0 != NULL ){
				pd3dDevice->SetTexture( 0, NULL );
				g_curtex0 = NULL;
			}
			if( g_curtex1 != NULL ){
				pd3dDevice->SetTexture( 1, NULL );
				g_curtex1 = NULL;
			}
			

			int renderno2;
			int befblno2 = -1;
			for( renderno2 = 0; renderno2 < mrenderblocknum; renderno2++ ){
				RENDERBLOCK* currb2;
				currb2 = mrenderblock + renderno2;

				int blno2;
				blno2 = currb2->skinno;

				D3DXMATRIX* curskinmat = *(mskinmat + blno2);
				int curskinnum = *(mskinmatnum + blno2);

	//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
	//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );


				////////////

				if( (ptoon1 + currb2->materialno)->edgevalid0 != 0 ){

					if( curskinnum == 1 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Edge0 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else if( curskinnum == 2 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Edge0 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Edge0 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				//}

					if( blno2 != befblno2 ){
						hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : edge worldmatrixarray set error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}


					float edge0[4];
					edge0[0] = (ptoon1 + currb2->materialno)->edgecol0.r;
					edge0[1] = (ptoon1 + currb2->materialno)->edgecol0.g;
					edge0[2] = (ptoon1 + currb2->materialno)->edgecol0.b;
					edge0[3] = (ptoon1 + currb2->materialno)->edgecol0.a;//<--- 内容はwidth

					hres = g_pEffect->SetValue( g_hmEdgeCol0, &(edge0[0]), sizeof( float ) * 4 );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : mEdgeCol0 SetValue error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}


					/////////
					hres = g_pEffect->Begin(&cPasses2, 0);
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : effect Begin error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					for (iPass2 = 0; iPass2 < cPasses2; iPass2++){
						hres = g_pEffect->BeginPass(iPass2);
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : effect BeginPass error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}


						// The effect interface queues up the changes and performs them 
						// with the CommitChanges call. You do not need to call CommitChanges if 
						// you are not setting any parameters between the BeginPass and EndPass.
						// V( g_pEffect->CommitChanges() );

						int curnumprim;
						curnumprim = currb2->endface - currb2->startface;

						if( curnumprim > 0 ){

							if( (ptoon1 + currb2->materialno)->edgeinv0 && m_revArrayVB ){
								//裏面
								pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
								pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno2), 0, sizeof(SKINVERTEX) );		
								//pd3dDevice->SetIndices( m_revIB );
								pd3dDevice->SetIndices( m_IB );
								hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
															0,
															0,
															m_numTLV,
															currb2->startface * 3,
															curnumprim
															);
							}else{

								pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
								pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno2), 0, sizeof(SKINVERTEX) );
								//pd3dDevice->SetIndices( m_IB );
								pd3dDevice->SetIndices( m_revIB );

								hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
																0,
																0,
																m_numTLV,
																currb2->startface * 3,
																curnumprim
																);
								}
						}

						/***
						if( m_revArrayVB && (curnumprim > 0) ){
							pd3dDevice->SetVertexDeclaration( m_pskindecl );
							pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno2), 0, sizeof(SKINVERTEX) );		
							pd3dDevice->SetIndices( m_revIB );
							hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
														0,
														0,
														m_numTLV,
														currb2->startface * 3,
														curnumprim
														);

						}
						***/
						hres = g_pEffect->EndPass();
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : effect EndPass error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

					}
					hres = g_pEffect->End();
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : effect End error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					befblno2 = blno2;

				}
			}
		}

	}

	return ret;
}


int CD3DDisp::RenderTri( LPDIRECT3DDEVICE9 pd3dDevice, int faceno, CShdHandler* lpsh, float* toonparams, LPDIRECT3DTEXTURE9 lptex, float* fogparams, CToon1Params* ptoon1 )
{
	HRESULT hres;
	int ret = 0;


	DWORD renderPrim = 1;
	DWORD renderrevPrim = 1;

	static int dbgcnt = 0;

	
//DbgOut( "d3ddisp : RenderTri\n" );
		hres = g_pEffect->SetValue( g_hmFogParams, fogparams, sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : fogparams set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		float fsetlno = (float)g_lightnum;
		hres = g_pEffect->SetFloat( g_hmLightNum, fsetlno );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : mLightNum SetFloat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&m_EyePt), sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "d3ddisp : RenderTri : LightSpecular set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (g_usePS == 1) && (toonparams) ){
			hres = g_pEffect->SetValue( g_hmToonParams, toonparams, sizeof( float ) * 4 );
			if( hres != D3D_OK ){
				DbgOut( "d3ddisp : RenderTri : toonparams set error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

///////////////
		UINT cPasses, iPass;

		if( (lpsh->m_shader != COL_TOON1) || (toonparams == 0) ){

			int blno = GetCurrentSkinBlockNo( faceno, mskinblock );
			_ASSERT( blno > 0 );

			//int blno;
			//for( blno = 0; blno < mskinblocknum; blno++ ){
				D3DXMATRIX* curskinmat = *(mskinmat + blno);
				int curskinnum = *(mskinmatnum + blno);

				if( (lpsh->m_shader == COL_TOON0) && (g_usePS == 1) ){
					if( lptex != NULL ){
						if( curskinnum == 1 ){
							hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Toon );
							if( hres != D3D_OK ){
								DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}else if( curskinnum == 2 ){
							hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Toon );
							if( hres != D3D_OK ){
								DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}else{
							hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Toon );
							if( hres != D3D_OK ){
								DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
					}else{
						if( curskinnum == 1 ){
							hres = g_pEffect->SetTechnique( g_hRenderSceneBone0ToonNoTex );
							if( hres != D3D_OK ){
								DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}else if( curskinnum == 2 ){
							hres = g_pEffect->SetTechnique( g_hRenderSceneBone1ToonNoTex );
							if( hres != D3D_OK ){
								DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}else{
							hres = g_pEffect->SetTechnique( g_hRenderSceneBone4ToonNoTex );
							if( hres != D3D_OK ){
								DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
					}
				}else{
					if( curskinnum == 1 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone0 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else if( curskinnum == 2 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone1 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone4 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
				hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			/////////
				hres = g_pEffect->Begin(&cPasses, 0);
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : effect Begin error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				for (iPass = 0; iPass < cPasses; iPass++){
					hres = g_pEffect->BeginPass(iPass);
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : effect BeginPass error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}


					// The effect interface queues up the changes and performs them 
					// with the CommitChanges call. You do not need to call CommitChanges if 
					// you are not setting any parameters between the BeginPass and EndPass.
					// V( g_pEffect->CommitChanges() );

					int curnumprim;
					SKINBLOCK* curskinblock;
					curskinblock = mskinblock + blno;
					curnumprim = curskinblock->endface - curskinblock->startface;

					if( curnumprim > 0 ){
						hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
						hres = pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
						hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );
						
						hres = pd3dDevice->SetIndices( m_IB );

						hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
														0,
														0,
														m_numTLV,
														faceno * 3,
														1
														);
						_ASSERT( hres == D3D_OK );
					}

					if( m_revArrayVB && (curnumprim > 0) ){
						hres = pd3dDevice->SetVertexDeclaration( m_pskindecl_tima );
						hres = pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );
						hres = pd3dDevice->SetStreamSource( 1, m_VBmaterial, 0, sizeof(SKINMATERIAL) );

						hres = pd3dDevice->SetIndices( m_revIB );
						_ASSERT( hres == D3D_OK );
						hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													faceno * 3,
													1
													);
						_ASSERT( hres == D3D_OK );

					}

					hres = g_pEffect->EndPass();
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : effect EndPass error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

				}
				hres = g_pEffect->End();
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : effect End error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			//}
		}else{
			//int renderno;
			int renderno = GetCurrentBlockNo( faceno, mrenderblock );
			_ASSERT( renderno > 0 );

			int befblno = -1;
			//for( renderno = 0; renderno < mrenderblocknum; renderno++ ){
				RENDERBLOCK* currb;
				currb = mrenderblock + renderno;

				int blno;
				blno = currb->skinno;

				D3DXMATRIX* curskinmat = *(mskinmat + blno);
				int curskinnum = *(mskinmatnum + blno);

//DbgOut( "d3ddisp render : RENDERBLOCK : no %d, materialno %d, skinno %d, startface %d, endface %d\r\n",
//	renderno, currb->materialno, currb->skinno, currb->startface, currb->endface );

				LPDIRECT3DTEXTURE9 curtex;
				curtex = (ptoon1 + currb->materialno)->tex;

				if( g_curtex0 != curtex ){
					pd3dDevice->SetTexture( 0, curtex );//!!!!!!!!!
					g_curtex0 = curtex;
				}


				//if( lptex != NULL ){
					if( curskinnum == 1 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone0Toon1 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else if( curskinnum == 2 ){
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone1Toon1 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						hres = g_pEffect->SetTechnique( g_hRenderSceneBone4Toon1 );
						if( hres != D3D_OK ){
							DbgOut( "d3ddisp : RenderTri : SetTechnique error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				//}

				if( blno != befblno ){
					hres = g_pEffect->SetMatrixArray( g_hmWorldMatrixArray, curskinmat, curskinnum );
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : worldmatrixarray set error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				float tmpfl[4];
				tmpfl[0] = 0.0f;
				tmpfl[1] = 0.0f;
				tmpfl[2] = (ptoon1 + currb->materialno)->darknl;
				tmpfl[3] = (ptoon1 + currb->materialno)->brightnl;
				hres = g_pEffect->SetValue( g_hmToonParams, tmpfl, sizeof( float ) * 4 );
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : toon1 params set error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			/////////
				hres = g_pEffect->Begin(&cPasses, 0);
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : effect Begin error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				for (iPass = 0; iPass < cPasses; iPass++){
					hres = g_pEffect->BeginPass(iPass);
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : effect BeginPass error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}


					// The effect interface queues up the changes and performs them 
					// with the CommitChanges call. You do not need to call CommitChanges if 
					// you are not setting any parameters between the BeginPass and EndPass.
					// V( g_pEffect->CommitChanges() );

					int curnumprim;
					curnumprim = currb->endface - currb->startface;

					if( curnumprim > 0 ){
						pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
						pd3dDevice->SetStreamSource( 0, *(m_ArrayVB + blno), 0, sizeof(SKINVERTEX) );
						pd3dDevice->SetIndices( m_IB );

						hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
														0,
														0,
														m_numTLV,
														faceno * 3,
														1
														);
					}

					if( m_revArrayVB && (curnumprim > 0) ){
						pd3dDevice->SetVertexDeclaration( m_pskindecl_mema );
						pd3dDevice->SetStreamSource( 0, *(m_revArrayVB + blno), 0, sizeof(SKINVERTEX) );		
						pd3dDevice->SetIndices( m_revIB );
						hres = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
													0,
													0,
													m_numTLV,
													faceno * 3,
													1
													);

					}

					hres = g_pEffect->EndPass();
					if( hres != D3D_OK ){
						DbgOut( "d3ddisp : RenderTri : effect EndPass error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

				}
				hres = g_pEffect->End();
				if( hres != D3D_OK ){
					DbgOut( "d3ddisp : RenderTri : effect End error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				befblno = blno;
				
			//}
		}

	return ret;
}

int CD3DDisp::RenderBone( LPD3DXSPRITE pd3dxsprite, LPDIRECT3DTEXTURE9 ptexture, int isselected, float jointsize, int jointalpha )
{
	int ret;
	HRESULT hr;
	D3DXVECTOR3	tra;
	D3DXVECTOR2	scl;
	D3DCOLOR col;
	static int colcnt = 0;
	static int red = 0;
	static int blue = 255;
	static int pmred = 1;
	static int pmblue = -1;
	//int bonebmpsizex = 12;
	//int bonebmpsizey = 12;

	/***
	static int dbgcnt = 0;
	if( dbgcnt < 10 )
		DbgOut( "D3DDisp : RenderBone : bonebmpsize %d %d\n", s_bonebmpsizex, s_bonebmpsizey );
	dbgcnt++;
	***/

	//if( g_useGPU == 0 ){
	//	if( *m_clipflag != 0 )
	//		return 0;

		//if( (colcnt % 6) == 0 ) 
			red += pmred;
		colcnt++;

		if( red > 255 ){
			red = 255;
			pmred = -1;
		}
		if( red < 0 ){
			red = 0;
			pmred = 1;
		}
			
		blue += pmblue;
		if( blue > 255 ){
			blue = 255;
			pmblue = -1;
		}
		if( blue < 0 ){
			blue = 0;
			pmblue = 1;
		}

		float scale0;
		if( isselected ){
			col = D3DCOLOR_ARGB( jointalpha, 255, 0, 0 );
			scale0 = 0.5f;
		}else{
			col = D3DCOLOR_ARGB( jointalpha, 255, 255, 255 );
			scale0 = 0.4f;
		}

		float scale;
		scale = 0.5f * jointsize;
		scl.x = scale0 * scale; scl.y = scale0 * scale;

		tra.x = m_scv->x - s_bonebmpsizex * scale0 * scale;
		tra.y = m_scv->y - s_bonebmpsizey * scale0 * scale;
		tra.z = 0.0f;

		D3DXMATRIX transmat;
		D3DXMATRIX scalemat;
		D3DXMatrixIdentity( &scalemat );

		D3DXMATRIX posmat;
		D3DXMatrixIdentity( &posmat );

		posmat._41 = tra.x;
		posmat._42 = tra.y;
		//posmat._43 = 0.000001f;

		scalemat._11 = scl.x;
		scalemat._22 = scl.y;

		transmat = scalemat * posmat;
		pd3dxsprite->SetTransform( &transmat );

		hr = pd3dxsprite->Draw( ptexture, NULL, NULL, NULL, col );


		_ASSERT( hr == D3D_OK );

		ret = (hr != D3D_OK);
		return ret;
	//}else{
	//	return 0;
	//}
}

int CD3DDisp::ResetPrimNum()
{
	CD3DDisp::totalPrim = 0;
	CD3DDisp::invalidPrim = 0;
	CD3DDisp::pracPrim = 0;

	return 0;
}

int CD3DDisp::SetRevNormal()
{
	DWORD i;
	D3DXVECTOR3* srcnormal = m_orgNormal;
	D3DXVECTOR3* dstnormal = m_revNormal;
	for( i = 0; i < m_numTLV; i++ ){
		dstnormal->x = -srcnormal->x;
		dstnormal->y = -srcnormal->y;
		dstnormal->z = -srcnormal->z;

		srcnormal++;
		dstnormal++;
	}

	return 0;
}


int CD3DDisp::InitColor( D3DXVECTOR3 vEyePt, int srcshader, int srcoverflow, CPolyMesh* polymesh, float alpha, D3DXMATRIX matWorld, int flag )
{

	int ret;

	m_alpha = alpha;
	
//DbgOut( "disp : InitColor : bef normal\n" );
	ret = SetSkinNormalPM();
	if( ret ){
		DbgOut( "d3ddisp : InitColor : polymesh : SetSkinNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "disp : InitColor : bef color\n" );
	ret = SetSkinColorPM( polymesh, alpha );
	if( ret ){
		DbgOut( "d3ddisp : InitColor : polymesh : SetSkinColorPM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//DbgOut( "disp : InitColor : end\n" );

	return 0;
}

int CD3DDisp::InitColor( D3DXVECTOR3 vEyePt, int srcshader, int srcoverflow, CPolyMesh2* polymesh2, float alpha, D3DXMATRIX matWorld, int flag )
{
	int ret;

	m_alpha = alpha;

	ret = SetSkinNormalPM2( polymesh2 );
	if( ret ){
		DbgOut( "d3ddisp : InitColor : polymesh2 : SetSkinNormalPM2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = SetSkinColorPM2( polymesh2, alpha );
	if( ret ){
		DbgOut( "d3ddisp : InitColor : polymesh2 : SetSkinColorPM2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CD3DDisp::InitColor( CExtLine* extline, float alpha, D3DXMATRIX matWorld, int flag )
{
	int ret;

	m_alpha = alpha;

	ret = SetLVColor( extline, alpha );
	if( ret ){
		DbgOut( "d3ddisp : InitColor : SetLVColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




int CD3DDisp::CheckMikoAnchor( CShdHandler* srclpsh, int srcseri )
{
	int ret;

	if( !m_IEptr )
		return 1;

	if( !m_skinv ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* dispselem;

	dispselem = (*srclpsh)( srcseri );
	if( !dispselem ){
		_ASSERT( 0 );
		return 1;
	}

	if( (dispselem->m_anchorflag == 1) || ( (dispselem->type != SHDPOLYMESH2) && (dispselem->type != SHDPOLYMESH) ) ){
		return 0;//!!!!!!!!!
	}


	DWORD tlvno;
	CInfElem* curie;

	SKINVERTEX* curskinv = 0;
	D3DXVECTOR3 curxv;

	CPolyMesh2* disppm2 = 0;
	CPolyMesh* disppm = 0;

	if( dispselem->type == SHDPOLYMESH2 ){
		disppm2 = dispselem->polymesh2;
		if( !disppm2 ){
			DbgOut( "d3ddisp : CheckMikoAnchor : disppm2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	if( dispselem->type == SHDPOLYMESH ){
		disppm = dispselem->polymesh;
		if( !disppm ){
			DbgOut( "d3ddisp : CheckMikoAnchor : disppm NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	if( disppm2 && disppm2->m_mikoapplychild ){
		// pm2の表示オブジェクトに、適応ボーン指定がある場合

		for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
			curie = m_IEptr + tlvno;
			ret = curie->AddMikoApChild( disppm2->m_mikoapplychild->serialno );
			if( ret ){
				DbgOut( "d3ddisp : CheckMikoAnchor : ie AddMikoApChild error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else if( disppm && disppm->m_mikoapplychild ){
		// pmの表示オブジェクトに、適応ボーン指定がある場合
		for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
			curie = m_IEptr + tlvno;
			ret = curie->AddMikoApChild( disppm->m_mikoapplychild->serialno );
			if( ret ){
				DbgOut( "d3ddisp : CheckMikoAnchor : ie AddMikoApChild error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		// 表示オブジェクトに、適応ボーン指定がない場合
	
		CShdElem* iselem;
		iselem = srclpsh->GetInfScopeElem();
		if( iselem ){
			int scno;
			for( scno = 0; scno < iselem->scopenum; scno++ ){
				CInfScope* isptr;
				isptr = *( iselem->ppscope + scno );
				_ASSERT( isptr );

				if( (isptr->target == dispselem) && isptr->applychild ){
					//スコープのターゲットに、自分（表示オブジェクト）が指定されているとき

					for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
						curskinv = m_skinv + tlvno;
						curxv.x = curskinv->pos[0];
						curxv.y = curskinv->pos[1];
						curxv.z = curskinv->pos[2];

						curie = m_IEptr + tlvno;

						int insideflag = 0;
						ret = isptr->CheckVertInScope( curxv, iselem->clockwise, &insideflag );
						if( ret ){
							DbgOut( "d3ddisp : CheckMikoAnchor ; is CheckVertInScope error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						if( insideflag != 0 ){
							ret = curie->AddMikoApChild( isptr->applychild->serialno );
							if( ret ){
								DbgOut( "d3ddisp : CheckMikoAnchor : ie AddMikoApChild error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}

					}

				}

			}

		}

	}

	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		curie = m_IEptr + tlvno;
		
		curskinv = m_skinv + tlvno;
		curxv.x = curskinv->pos[0];
		curxv.y = curskinv->pos[1];
		curxv.z = curskinv->pos[2];

		ret = curie->SetNearestBI( srclpsh, curxv );
		if( ret ){
			DbgOut( "d3ddisp : CheckMikoAnchor : ie SetNearestBI error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CD3DDisp::CreateSkinMat( CShdHandler* lpsh )
{
	if( !m_IEptr || !m_skinv || !m_dispIndices ){
		DbgOut( "d3ddisp : CreateSkinMat : data NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_numTLV < 3 ){
		DbgOut( "d3ddisp : CreateSkinMat : tlvnum less than 3 ; skip\n" );
		return 0;//!!!!!!!!!
	}

	int limitnum;
	if( m_blurmode == BLUR_NONE ){
		limitnum = MAXSKINMATRIX;
	}else{
		limitnum = MAXBLURMATRIX;
	}

//DbgOut( "d3ddisp : CreateSkinMat : 0\n" );

	DestroySkinMat();

	int leng = lpsh->s2shd_leng;

	int ifaceno, startfaceno;;
	int index[3];
	int blockno = 0;

	int* bonemat2skinmat;
	int* skinmat2bonemat;
	int* dirtyflag;
	D3DXMATRIX* skinmat;

	int hasjointflag;


	dirtyflag = (int*)malloc( sizeof( int ) * leng );
	if( !dirtyflag ){
		DbgOut( "d3ddisp : CreateSkinMat : dirtyflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	startfaceno = 0;
	while( startfaceno < m_vnum ){
		hasjointflag = 0;

		mskinblock = (SKINBLOCK*)realloc( mskinblock, sizeof( SKINBLOCK ) * (blockno + 1) );
		if( !mskinblock ){
			DbgOut( "d3ddisp : CreateSkinMat : skinblock alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		(mskinblock + blockno)->startface = startfaceno;

		bonemat2skinmat = (int*)malloc( sizeof( int ) * leng );
		if( !bonemat2skinmat ){
			DbgOut( "d3ddisp : CreateSkinMat : bonemat2skinmat alloc error !!!\n" );
			_ASSERT( 0 );
			free( dirtyflag );
			return 1;
		}
		ZeroMemory( dirtyflag, sizeof( int ) * leng );
		ZeroMemory( bonemat2skinmat, sizeof( int ) * leng );

		int skinmatnum;
		skinmatnum = 0;

		for( ifaceno = startfaceno; ifaceno < m_vnum; ifaceno++ ){
			index[0] = *(m_dispIndices + ifaceno * 3);
			index[1] = *(m_dispIndices + ifaceno * 3 + 1);
			index[2] = *(m_dispIndices + ifaceno * 3 + 2);
		

			int ino;
			int ieno;
			int addmatnum = 0;
			int addboneno[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			for( ino = 0; ino < 3; ino++ ){
				CInfElem* curie = m_IEptr + index[ino];
				
				if( curie->infnum > 4 ){
					char messtr[1024];
					sprintf_s( messtr, 1024, "ひとつの頂点に作用できるボーンの数は、４個までです。\n影響ボーンの数が多すぎるので処理できません。エラー終了します。" );
					::MessageBox( NULL, messtr, "エラーです", MB_OK );
					free( dirtyflag );
					free( bonemat2skinmat );
					_ASSERT( 0 );
					return 1;
				}

				for( ieno = 0; ieno < curie->infnum; ieno++ ){
					INFELEM* curIE = curie->ie + ieno;

					if( (curIE->bonematno > 0) && (*(dirtyflag + curIE->bonematno) == 0) ){

						int findsame = 0;
						int ano;
						for( ano = 0; ano < addmatnum; ano++ ){
							if( addboneno[ano] == curIE->bonematno ){
								findsame = 1;
								break;
							}
						}
						if( findsame == 0 ){
							addboneno[addmatnum] = curIE->bonematno;
							addmatnum++;
						}
					}
				}
			}
			if( (skinmatnum + addmatnum + 1) >= limitnum ){
				hasjointflag = ifaceno;
				break;
			}else{
				skinmatnum += addmatnum;
				int addno;
				for( addno = 0; addno < addmatnum; addno++ ){
					if( *(dirtyflag + addboneno[addno]) == 1 ){
						DbgOut( "d3ddisp : CreateSkinMat : dirtyflag error %d\r\n", addboneno[addno] );
						_ASSERT( 0 );
						free( dirtyflag );
						free( bonemat2skinmat );
						return 1;
					}
					*(dirtyflag + addboneno[addno]) = 1;
				}
			}
		}
		(mskinblock + blockno)->endface = ifaceno;//!!!!!!!!! 次の最初の番号

		startfaceno = ifaceno;
		skinmatnum += 1;// index 0 の分。
		*(dirtyflag + 0) = 1;
////////////////
		skinmat2bonemat = (int*)malloc( sizeof( int ) * skinmatnum );
		if( !skinmat2bonemat ){
			DbgOut( "d3ddisp : CreateSkinMat : skinmat2bonemat alloc error !!!\n" );
			_ASSERT( 0 );
			free( dirtyflag );
			free( bonemat2skinmat );
			return 1;
		}
		ZeroMemory( skinmat2bonemat, sizeof( int ) * skinmatnum );


		int elemno;
		*(skinmat2bonemat + 0) = 0;
		int setno = 1;

		for( elemno = 1; elemno < leng; elemno++ ){
			if( *(dirtyflag + elemno) == 1 ){
				*(bonemat2skinmat + elemno) = setno;
				*(skinmat2bonemat + setno) = elemno;
				setno++;
			}
		}
		if( setno != skinmatnum ){
			DbgOut( "d3ddisp : CreateSkinMat : setno error %d, %d, %d!!!\n", setno, skinmatnum, hasjointflag );

//int dbgcnt;
//for( dbgcnt = 0; dbgcnt < leng; dbgcnt++ ){
//	DbgOut( "d3ddisp : CreateSkinMat : error dirty : %d, %d\r\n", dbgcnt, *(dirtyflag + dbgcnt) );
//}
			_ASSERT( 0 );
			free( dirtyflag );
			free( bonemat2skinmat );
			free( skinmat2bonemat );
			return 1;
		}

		if( m_blurmode == BLUR_NONE ){
			skinmat = new D3DXMATRIX[ skinmatnum ];
			if( !skinmat ){
				DbgOut( "d3ddisp : CreateSkinMat : skinmat alloc error !!!\n" );
				_ASSERT( 0 );
				free( dirtyflag );
				free( bonemat2skinmat );
				free( skinmat2bonemat );
				return 1;
			}
			int mno;
			for( mno = 0; mno < skinmatnum; mno++ ){
				D3DXMatrixIdentity( skinmat + mno );
			}
		}else{
			skinmat = new D3DXMATRIX[ MAXSKINMATRIX + 1 ];
			if( !skinmat ){
				DbgOut( "d3ddisp : CreateSkinMat : skinmat alloc error !!!\n" );
				_ASSERT( 0 );
				free( dirtyflag );
				free( bonemat2skinmat );
				free( skinmat2bonemat );
				return 1;
			}
			int mno;
			for( mno = 0; mno < (MAXSKINMATRIX + 1); mno++ ){
				D3DXMatrixIdentity( skinmat + mno );
			}
		}
		/////////////////
		mskinmat2bonemat = (int**)realloc( mskinmat2bonemat, sizeof( int* ) * (blockno + 1) );
		if( !mskinmat2bonemat ){
			DbgOut( "d3ddisp : CreateSkinMat : mskinmat2bonemat alloc error %d!!!\n", blockno );
			_ASSERT( 0 );
			return 1;
		}
		*(mskinmat2bonemat + blockno) = skinmat2bonemat;
	
		mbonemat2skinmat = (int**)realloc( mbonemat2skinmat, sizeof( int* ) * (blockno + 1) );
		if( !mbonemat2skinmat ){
			DbgOut( "d3ddisp : CreateSkinMat : mbonemat2skinmat alloc error %d !!!\n", blockno );
			_ASSERT( 0 );
			return 1;
		}
		*(mbonemat2skinmat + blockno) = bonemat2skinmat;

		mskinmat = (D3DXMATRIX**)realloc( mskinmat, sizeof( D3DXMATRIX*) * (blockno + 1) );
		if( !mskinmat ){
			DbgOut( "d3ddisp : CreateSkinMat : mskinmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		*(mskinmat + blockno) = skinmat;

		mskinmatnum = (int*)realloc( mskinmatnum, sizeof( int ) * (blockno + 1) );
		if( !mskinmatnum ){
			DbgOut( "d3ddisp : CreateSkinMat : mskinmatnum alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		*(mskinmatnum + blockno) = skinmatnum;

		blockno++;
	}

	mskinblocknum = blockno;
	free( dirtyflag );
	DbgOut( "d3ddisp : CreateSkinMat : %d\r\n", mskinblocknum );
////////////
	int ret;
	ret = CreateBlurMat();
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMat : CreateBlurMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////////
	ret = Create3DBuffersSkin();
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMat : Create3DBuffersSkin error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "d3ddisp : CreateSkinMat : 1\n" );

	return 0;
}

int CD3DDisp::CreateSkinMatToon1( CShdHandler* lpsh, MATERIALBLOCK* materialblock, int materialnum )
{
	int ret;

	if( m_numTLV < 3 ){
		DbgOut( "d3ddisp : CreateSkinMatToon1 : tlvnum less than 3 ; skip\n" );
		return 0;//!!!!!!!!!
	}

	ret = CreateSkinMat( lpsh );
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMatToon1 : CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////
	int rblocknum = 0;
	int skinno, curskinstart, curskinend;
	MATERIALBLOCK* curmaterialblock;
	RENDERBLOCK* currenderblock;
	int curstart;
	int materialno = 0;
	CMQOMaterial* curmqomat = 0;


	curmaterialblock = materialblock;

	for( skinno = 0; skinno < mskinblocknum; skinno++ ){


		curskinstart = (mskinblock + skinno)->startface;
		curskinend = (mskinblock + skinno)->endface;

		curstart = curskinstart;

		while( curskinend >= curmaterialblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatToon1 : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			currenderblock->materialno = curmaterialblock->materialno;
			currenderblock->startface = curstart;
			currenderblock->endface = curmaterialblock->endface;
			currenderblock->mqomat = curmaterialblock->mqomat;//!!!!!!!!!!!!
			materialno++;
			if( materialno < materialnum ){
				curmaterialblock = materialblock + materialno;
				curstart = curmaterialblock->startface;
			}else{
				break;
			}

		}
		if( curskinend < curmaterialblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatToon1 : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			currenderblock->materialno = curmaterialblock->materialno;
			currenderblock->startface = curstart;
			currenderblock->endface = curskinend;//!!!
			currenderblock->mqomat = curmaterialblock->mqomat;
		}

	}

	mrenderblocknum = rblocknum;

	/***
// for dbg
	int blno;
	for( blno = 0; blno < mrenderblocknum; blno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + blno;

		DbgOut( "check renderblock : no %d : materialno %d, skinno %d, startface %d, endface %d\r\n",
			blno, currb->materialno, currb->skinno, currb->startface, currb->endface );
	}
	DbgOut( "\r\n" );
	***/

	return 0;
}

int CD3DDisp::CreateSkinMatTex( CShdHandler* lpsh, TEXTUREBLOCK* texblock, int texblocknum )
{
	int ret;

	if( m_numTLV < 3 ){
		DbgOut( "d3ddisp : CreateSkinMatTex : tlvnum less than 3 ; skip\n" );
		return 0;//!!!!!!!!!
	}

	ret = CreateSkinMat( lpsh );
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMatTex : CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////
	int rblocknum = 0;
	int skinno, curskinstart, curskinend;
	TEXTUREBLOCK* curtexblock;
	RENDERBLOCK* currenderblock;
	int curstart;
	int texblockno = 0;


	curtexblock = texblock;

	for( skinno = 0; skinno < mskinblocknum; skinno++ ){


		curskinstart = (mskinblock + skinno)->startface;
		curskinend = (mskinblock + skinno)->endface;
		
		curstart = curskinstart;

		while( curskinend >= curtexblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatTex : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			//currenderblock->materialno = curtexblock->materialno;
			currenderblock->materialno = 0;
			currenderblock->startface = curstart;
			currenderblock->endface = curtexblock->endface;
			currenderblock->mqomat = curtexblock->mqomat;

			texblockno++;
			if( texblockno < texblocknum ){
				curtexblock = texblock + texblockno;
				curstart = curtexblock->startface;
			}else{
				break;
			}

		}
		if( curskinend < curtexblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatTex : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			//currenderblock->materialno = curtexblock->materialno;
			currenderblock->materialno = 0;
			currenderblock->startface = curstart;
			currenderblock->endface = curskinend;//!!!
			currenderblock->mqomat = curtexblock->mqomat;

		}

	}

	mrenderblocknum = rblocknum;

	/***
// for dbg
	int blno;
	for( blno = 0; blno < mrenderblocknum; blno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + blno;

		DbgOut( "check renderblock : no %d : materialno %d, skinno %d, texname %s, startface %d, endface %d\r\n",
			blno, currb->mqomat->materialno, currb->skinno, currb->mqomat->tex, currb->startface, currb->endface );
	}
	DbgOut( "\r\n" );
	***/

	return 0;

}




int CD3DDisp::CalcInfElem( CShdHandler* srclpsh, int srcseri )
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 変更するときは、CalcInfElem1Vertも！！！！！！！
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if( !m_IEptr )
		return 1;

	if( !m_skinv ){
		_ASSERT( 0 );
		return 1;
	}

	DWORD tlvno;
	SKINVERTEX* curskinv = 0;
	D3DXVECTOR3 curxv;
	CShdElem* nearestjoint = 0;
	CBoneInfo* nearestbone = 0;
	CInfElem* curie;
	int ret = 0;

	CShdElem* curselem = (*srclpsh)( srcseri );
	_ASSERT( curselem );

	if( curselem->m_loadbimflag != 0 ){
		/***
		if( g_useGPU == 1 ){
			ret = CreateSkinMat( srclpsh );
			if( ret ){
				DbgOut( "d3ddisp : CalcInfElem : loadbimflag : CreateSkinMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		***/
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!  bim ファイル読み込み時は、この計算は行わない。
	}


	int* ignoreflag = 0;
	ignoreflag = (int*)malloc( sizeof( int ) * srclpsh->s2shd_leng );
	if( !ignoreflag ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( ignoreflag, sizeof( int ) * srclpsh->s2shd_leng );

	ret = SetIgnoreFlag( srclpsh, srcseri, ignoreflag );
	if( ret ){
		_ASSERT( 0 );

		if( ignoreflag )
			free( ignoreflag );

		return 1;
	}


	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		curie = m_IEptr + tlvno;
		ret = curie->ClearMikoParams();
		if( ret ){
			DbgOut( "d3ddisp : CalcInfElem : ie ClearMikoParams error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	if( curselem->noskinning == 0 ){
		if( curselem->IsJoint() && (curselem->type != SHDMORPH) ){
						
			curie = m_IEptr;
			CShdElem* parselem = srclpsh->FindUpperJoint( curselem );
			
			if( parselem ){
				curie->SetInfElemDefaultNoSkin( parselem->serialno, srcseri );
			}else{
				//curie->SetInfElemDefaultNoSkin( srcseri, -1 );
				curie->SetInfElemDefaultNoSkin( srcseri, srcseri );
			}

		}else{

			ret = CheckMikoAnchor( srclpsh, srcseri );
			if( ret ){
				DbgOut( "d3ddisp : CalcInfElem : CheckMikoAnchor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			//curtlv = 0;
			//curv = 0;
			for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
				curskinv = m_skinv + tlvno;
				curxv.x = curskinv->pos[0];
				curxv.y = curskinv->pos[1];
				curxv.z = curskinv->pos[2];

				curie = m_IEptr + tlvno;

				if( !curie->nearestbi ){

					if( curselem->m_mikodef == MIKODEF_NONE ){

						//一番近いjoint(bonenum 1以上)を探す。
						nearestjoint = FindNearestJoint( srclpsh, &curxv, srcseri, ignoreflag, srclpsh->s2shd_leng );

						//一番近いボーンを探す。
						if( nearestjoint ){
							nearestbone = FindNearestBone( nearestjoint, &curxv, ignoreflag, srclpsh->s2shd_leng );		
						}else{
						
							nearestjoint = FindNearestJoint( srclpsh, &curxv, srcseri, ignoreflag, srclpsh->s2shd_leng, 0 );
							CShdElem* parjoint;
							if( nearestjoint ){
								parjoint = srclpsh->FindUpperJoint( nearestjoint );
								if( parjoint ){
									CPart* parpart;
									parpart = parjoint->part;
									if( parpart ){
										parpart->GetBoneInfo( nearestjoint->serialno, &nearestbone );
									}else{
										nearestbone = 0;
									}
								}else{
									nearestbone = 0;
								}
							}else{
								nearestbone = 0;
							}							
							//nearestbone = 0;
						}


					}else{
						nearestbone = FindNearestBoneMiko( srclpsh, &curxv, curselem, 0 );
					}

					int parno;
					if( nearestbone )
						parno = nearestbone->parentjoint;
					else
						parno = -1;

					CShdElem* parselem;
					if( parno > 0 ){
						parselem = (*srclpsh)( parno );
					}else{
						parselem = 0;
					}

					//InfElemをセットする。

					ret = curie->SetInfElemDefault( nearestbone, parselem, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
					if( ret ){
						DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

						if( ignoreflag )
							free( ignoreflag );

						return 1;
					}
				}else{

					if( (srclpsh->m_mikoblendtype == MIKOBLEND_MIX) && (curie->mikoapchildnum == 1) ){

						//MIXモードで、アンカーが重なっていない場合は、スキニング無しモードにする。

						ret = curie->SetInfElemDefault( curie->nearestbi, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
						if( ret ){
							DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

							if( ignoreflag )
								free( ignoreflag );

							return 1;
						}

					}else{

						ret = curie->SetInfElemDefault( curie->nearestbi, curie->nearestbipar, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
						if( ret ){
							DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

							if( ignoreflag )
								free( ignoreflag );

							return 1;
						}
					}

				}
			}
		}
	}else{
		// skinning なし
		if( curselem->IsJoint() && (curselem->type != SHDMORPH) ){
			
			curie = m_IEptr;
			
			CShdElem* parselem = srclpsh->FindUpperJoint( curselem );
			if( parselem ){
				curie->SetInfElemDefaultNoSkin( parselem->serialno, srcseri );
			}else{
				//curie->SetInfElemDefaultNoSkin( srcseri, -1 );
				curie->SetInfElemDefaultNoSkin( srcseri, srcseri );
			}
		}else{
		
			ret = CheckMikoAnchor( srclpsh, srcseri );
			if( ret ){
				DbgOut( "d3ddisp : CalcInfElem : CheckMikoAnchor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			
			//!!!!!!!!!!!!!!!!!
			//最初の頂点から一番近いボーンを、全ての頂点に対応付ける。
			//!!!!!!!!!!!!!!!!!

			if( curselem->m_mikodef == MIKODEF_NONE ){

				SKINVERTEX* firstskinv;
				D3DXVECTOR3 firstxv;

				firstskinv = m_skinv;
				firstxv.x = firstskinv->pos[0];
				firstxv.y = firstskinv->pos[1];
				firstxv.z = firstskinv->pos[2];


				//一番近いjoint(end joint を含む)を探す。！！！！needhaschild 引数０！！！！
				nearestjoint = FindNearestJoint( srclpsh, &firstxv, srcseri, ignoreflag, srclpsh->s2shd_leng, 0 );


				////一番近いボーンを探す。
				//if( nearestjoint ){
				//	nearestbone = FindNearestBone( nearestjoint, &firstxv, ignoreflag, srclpsh->s2shd_leng );		
				//}else{
				//	nearestbone = 0;
				//}

				nearestbone = 0;

			}else{
				D3DXVECTOR3 centerv;
				ret = CalcCenterV( &centerv );
				if( ret ){
					DbgOut( "d3ddisp : CalcInfElem : CalcCenterV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				nearestbone = FindNearestBoneMiko( srclpsh, &centerv, curselem, 1 );

			}
			
			for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){

				curskinv = m_skinv + tlvno;
				curxv.x = curskinv->pos[0];
				curxv.y = curskinv->pos[1];
				curxv.z = curskinv->pos[2];

				//InfElemをセットする。
				curie = m_IEptr + tlvno;


				if( !curie->nearestbi ){
					//ret = curie->SetInfElemDefault( nearestbone, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
					//curie->SetInfElemDefaultNoSkin( nearestjoint->serialno, nearestjoint->serialno );
					if( nearestjoint ){
						curie->SetInfElemDefaultNoSkin( nearestjoint->serialno, nearestjoint->serialno );
					}else{
						ret = curie->SetInfElemDefault( nearestbone, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
					}

				}else{
					ret = curie->SetInfElemDefault( curie->nearestbi, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
				}
				if( ret ){
					DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

					if( ignoreflag )
						free( ignoreflag );

					return 1;
				}

			}

		}

	}


	if( ignoreflag )
		free( ignoreflag );

///////////////
	/***
	if( g_useGPU == 1 ){
		ret = CreateSkinMat( srclpsh );
		if( ret ){
			DbgOut( "d3ddisp : CalcInfElem : CreateSkinMat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	***/

	return 0;
}

int CD3DDisp::CalcInfElem1Vert( CShdHandler* srclpsh, int srcseri, int srcvert )
{

	if( !m_IEptr )
		return 1;

	if( !m_skinv ){
		_ASSERT( 0 );
		return 1;
	}

	//DWORD tlvno;
	SKINVERTEX* curskinv = 0;
	D3DXVECTOR3 curxv;
	CShdElem* nearestjoint = 0;
	CBoneInfo* nearestbone = 0;
	CInfElem* curie;
	int ret = 0;

	CShdElem* curselem = (*srclpsh)( srcseri );
	_ASSERT( curselem );

	//if( curselem->m_loadbimflag != 0 ){
	//	return 0;//!!!!!!!!!!!!!!!!!!!!!!!!  bim ファイル読み込み時は、この計算は行わない。
	//}


	int* ignoreflag = 0;
	ignoreflag = (int*)malloc( sizeof( int ) * srclpsh->s2shd_leng );
	if( !ignoreflag ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( ignoreflag, sizeof( int ) * srclpsh->s2shd_leng );

	ret = SetIgnoreFlag( srclpsh, srcseri, ignoreflag );
	if( ret ){
		_ASSERT( 0 );

		if( ignoreflag )
			free( ignoreflag );

		return 1;
	}


	curie = m_IEptr + srcvert;
	ret = curie->ClearMikoParams();
	if( ret ){
		DbgOut( "d3ddisp : CalcInfElem : ie ClearMikoParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	if( curselem->noskinning == 0 ){
		if( curselem->IsJoint() && (curselem->type != SHDMORPH) ){
						
			curie = m_IEptr;
			CShdElem* parselem = srclpsh->FindUpperJoint( curselem );
			
			if( parselem ){
				curie->SetInfElemDefaultNoSkin( parselem->serialno, srcseri );
			}else{
				//curie->SetInfElemDefaultNoSkin( srcseri, -1 );
				curie->SetInfElemDefaultNoSkin( srcseri, srcseri );
			}

		}else{

			ret = CheckMikoAnchor( srclpsh, srcseri );
			if( ret ){
				DbgOut( "d3ddisp : CalcInfElem : CheckMikoAnchor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			//curtlv = 0;
			//curv = 0;
			//for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
				curskinv = m_skinv + srcvert;
				curxv.x = curskinv->pos[0];
				curxv.y = curskinv->pos[1];
				curxv.z = curskinv->pos[2];

				curie = m_IEptr + srcvert;

				if( !curie->nearestbi ){

					if( curselem->m_mikodef == MIKODEF_NONE ){
						//一番近いjoint(bonenum 1以上)を探す。
						nearestjoint = FindNearestJoint( srclpsh, &curxv, srcseri, ignoreflag, srclpsh->s2shd_leng );

						//一番近いボーンを探す。
						if( nearestjoint ){
							nearestbone = FindNearestBone( nearestjoint, &curxv, ignoreflag, srclpsh->s2shd_leng );		
						}else{
						
							nearestjoint = FindNearestJoint( srclpsh, &curxv, srcseri, ignoreflag, srclpsh->s2shd_leng, 0 );
							CShdElem* parjoint;
							if( nearestjoint ){
								parjoint = srclpsh->FindUpperJoint( nearestjoint );
								if( parjoint ){
									CPart* parpart;
									parpart = parjoint->part;
									if( parpart ){
										parpart->GetBoneInfo( nearestjoint->serialno, &nearestbone );
									}else{
										nearestbone = 0;
									}
								}else{
									nearestbone = 0;
								}
							}else{
								nearestbone = 0;
							}							
							//nearestbone = 0;
						}
					}else{
						nearestbone = FindNearestBoneMiko( srclpsh, &curxv, curselem, 0 );
					}

					int parno;
					if( nearestbone )
						parno = nearestbone->parentjoint;
					else
						parno = -1;

					CShdElem* parselem;
					if( parno > 0 ){
						parselem = (*srclpsh)( parno );
					}else{
						parselem = 0;
					}

					//InfElemをセットする。

					ret = curie->SetInfElemDefault( nearestbone, parselem, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
					if( ret ){
						DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

						if( ignoreflag )
							free( ignoreflag );

						return 1;
					}
				}else{

					if( (srclpsh->m_mikoblendtype == MIKOBLEND_MIX) && (curie->mikoapchildnum == 1) ){

						//MIXモードで、アンカーが重なっていない場合は、スキニング無しモードにする。

						ret = curie->SetInfElemDefault( curie->nearestbi, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
						if( ret ){
							DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

							if( ignoreflag )
								free( ignoreflag );

							return 1;
						}

					}else{

						ret = curie->SetInfElemDefault( curie->nearestbi, curie->nearestbipar, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
						if( ret ){
							DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

							if( ignoreflag )
								free( ignoreflag );

							return 1;
						}
					}

				}
			//}
		}
	}else{
		// skinning なし
		if( curselem->IsJoint() && (curselem->type != SHDMORPH) ){
			
			curie = m_IEptr;
			
			CShdElem* parselem = srclpsh->FindUpperJoint( curselem );
			if( parselem ){
				curie->SetInfElemDefaultNoSkin( parselem->serialno, srcseri );
			}else{
				//curie->SetInfElemDefaultNoSkin( srcseri, -1 );
				curie->SetInfElemDefaultNoSkin( srcseri, srcseri );
			}
		}else{
		
			ret = CheckMikoAnchor( srclpsh, srcseri );
			if( ret ){
				DbgOut( "d3ddisp : CalcInfElem : CheckMikoAnchor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			
			//!!!!!!!!!!!!!!!!!
			//最初の頂点から一番近いボーンを、全ての頂点に対応付ける。
			//!!!!!!!!!!!!!!!!!

			if( curselem->m_mikodef == MIKODEF_NONE ){

				SKINVERTEX* firstskinv;
				D3DXVECTOR3 firstxv;

				firstskinv = m_skinv;
				firstxv.x = firstskinv->pos[0];
				firstxv.y = firstskinv->pos[1];
				firstxv.z = firstskinv->pos[2];

				//一番近いjoint(end joint を含む)を探す。！！！！needhaschild 引数０！！！！
				nearestjoint = FindNearestJoint( srclpsh, &firstxv, srcseri, ignoreflag, srclpsh->s2shd_leng, 0 );


				////一番近いボーンを探す。
				//if( nearestjoint ){
				//	nearestbone = FindNearestBone( nearestjoint, &firstxv, ignoreflag, srclpsh->s2shd_leng );		
				//}else{
				//	nearestbone = 0;
				//}

				nearestbone = 0;

			}else{
				D3DXVECTOR3 centerv;
				ret = CalcCenterV( &centerv );
				if( ret ){
					DbgOut( "d3ddisp : CalcInfElem : CalcCenterV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				nearestbone = FindNearestBoneMiko( srclpsh, &centerv, curselem, 1 );

			}
			
			//for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){

				curskinv = m_skinv + srcvert;
				curxv.x = curskinv->pos[0];
				curxv.y = curskinv->pos[1];
				curxv.z = curskinv->pos[2];


				//InfElemをセットする。
				curie = m_IEptr + srcvert;


				//ret = curie->SetInfElemDefault( nearestbone, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
				//curie->SetInfElemDefaultNoSkin( nearestjoint->serialno, nearestjoint->serialno );
				if( nearestjoint ){
					curie->SetInfElemDefaultNoSkin( nearestjoint->serialno, nearestjoint->serialno );
				}else{
					ret = curie->SetInfElemDefault( nearestbone, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
				}
				if( ret ){
					DbgOut( "CD3DDisp : CalcInfElem : SetInfElemDefault error !!!\n" );

					if( ignoreflag )
						free( ignoreflag );

					return 1;
				}

			//}

		}

	}


	if( ignoreflag )
		free( ignoreflag );


///////////////
//	if( g_useGPU == 1 ){
//		ret = CreateSkinMat( srclpsh );
//		if( ret ){
//			DbgOut( "d3ddisp : CalcInfElem : CreateSkinMat error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}


	return 0;
}

int CD3DDisp::CalcCenterV( D3DXVECTOR3* dstv )
{

	DWORD tlvno;
	SKINVERTEX* curskinv = 0;
	D3DXVECTOR3 curxv;

	D3DXVECTOR3 totalv( 0.0f, 0.0f, 0.0f );

	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){

		curskinv = m_skinv + tlvno;
		curxv.x = curskinv->pos[0];
		curxv.y = curskinv->pos[1];
		curxv.z = curskinv->pos[2];

		totalv += curxv;
	}

	if( m_numTLV ){
		*dstv = totalv / (float)m_numTLV;
	}else{
		dstv->x = 0.0f;
		dstv->y = 0.0f;
		dstv->z = 0.0f;
	}

	return 0;
}

int CD3DDisp::SetIgnoreFlag( CShdHandler* srclpsh, int srcseri, int* ignoreflag )
{
	CShdElem* selem = (*srclpsh)( srcseri );
	if( !selem ){
		_ASSERT( 0 );
		return 1;
	}
	
	int leng = srclpsh->s2shd_leng;

	int listno;
	for( listno = 0; listno < selem->ignorebonenum; listno++ ){
		int ignoreno = *(selem->ignorebone + listno);
		if( (ignoreno < 0) || (ignoreno >= leng) ){
			continue;
		}

		CShdElem* ignoreelem = (*srclpsh)( ignoreno );
		if( !ignoreelem ){
			continue;
		}
		
		SetIgnoreFlagReq( ignoreelem, ignoreflag, leng, 0 );
	}

	return 0;
}

void CD3DDisp::SetIgnoreFlagReq( CShdElem* ignoreelem, int* ignoreflag, int leng, int broflag )
{
	int ignoreno;
	ignoreno = ignoreelem->serialno;
	if( (ignoreno >= 0) && (ignoreno < leng) ){
		*(ignoreflag + ignoreno) = 1;
		//DbgOut( "D3DDisp : SetIgnoreFlagReq : %d\n", ignoreno );
	}

	CShdElem* chilelem = ignoreelem->child;
	if( chilelem ){
		SetIgnoreFlagReq( chilelem, ignoreflag, leng, 1 );
	}

	if( broflag ){
		CShdElem* broelem = ignoreelem->brother;
		if( broelem ){
			SetIgnoreFlagReq( broelem, ignoreflag, leng, 1 );
		}
	}

}

CShdElem* CD3DDisp::FindNearestJoint( CShdHandler* srclpsh, D3DXVECTOR3* srcxv, int srcseri, int* ignoreflag, int ignoreleng, int needhaschild )
{
	int i;
	CShdElem* selem = 0;
	CShdElem* nearestelem = 0;
	CPart* curpart = 0;
	float vecx, vecy, vecz;
	float mag, minmag;

	minmag = 1e8;
	mag = -1e8;

	CShdElem* curselem = (*srclpsh)( srcseri );
	if( curselem->influencebonenum == 0 ){
		for( i = 0; i < srclpsh->s2shd_leng; i++ ){
			selem = (*srclpsh)(i);
			if( selem->IsJoint() && (selem->type != SHDMORPH) && (selem->notuse != 1) ){
				curpart = selem->part;
				_ASSERT( curpart );

				int chkflag = *(ignoreflag + i);

				if( ((needhaschild == 0) || (curpart->bonenum > 0)) && (chkflag == 0) ){
					int chkflag2;
					chkflag2 = IsExistValidBone( i, curpart, ignoreflag, ignoreleng, needhaschild );
					
					if( chkflag2 ){
						vecx = curpart->jointloc.x - srcxv->x;
						vecy = curpart->jointloc.y - srcxv->y;
						vecz = curpart->jointloc.z - srcxv->z;
						mag = vecx * vecx + vecy * vecy + vecz * vecz;
						if( !nearestelem || (mag < minmag) ){
							minmag = mag;
							nearestelem = selem;
						}
					}
				}
			}
		}
	}else{
		int listno;
		float mindist2 = 1e8;
		for( listno = 0; listno < curselem->influencebonenum; listno++ ){
			int influenceno = *(curselem->influencebone + listno);
			//_ASSERT( (influenceno > 0) && (influenceno < srclpsh->s2shd_leng) );

			if( (influenceno > 0) && (influenceno < srclpsh->s2shd_leng) )
			{
				CShdElem* influenceelem = (*srclpsh)( influenceno );
				_ASSERT( influenceelem );

				FindNearestJointReq( srcxv, influenceelem, &mindist2, &nearestelem, 0, ignoreflag, ignoreleng, needhaschild );
			}

		}
	}
	return nearestelem;
}
void CD3DDisp::FindNearestJointReq( D3DXVECTOR3* srcxv, CShdElem* chkelem, float* distptr, CShdElem** ppnear, int broflag, int* ignoreflag, int ignoreleng, int needhaschild )
{
	CPart* curpart;
	float vecx, vecy, vecz, mag;
	if( chkelem->IsJoint() && (chkelem->type != SHDMORPH) && (chkelem->notuse != 1) ){
		curpart = chkelem->part;
		_ASSERT( curpart );
		
		int chkflag = 0;
		int serino = chkelem->serialno;
		if( (serino >= 0) && (serino < ignoreleng) ){
			chkflag = *(ignoreflag + serino);
		}

		if( ( (needhaschild == 0) || (curpart->bonenum > 0)) && (chkflag == 0) ){
			int chkflag2;
			chkflag2 = IsExistValidBone( serino, curpart, ignoreflag, ignoreleng, needhaschild );
			if( chkflag2 ){
				vecx = curpart->jointloc.x - srcxv->x;
				vecy = curpart->jointloc.y - srcxv->y;
				vecz = curpart->jointloc.z - srcxv->z;
				mag = vecx * vecx + vecy * vecy + vecz * vecz;
				if( (*ppnear == 0) || (mag < *distptr) ){
					*distptr = mag;
					*ppnear = chkelem;
				}
			}
		}
	}
	
	CShdElem* chilelem = chkelem->child;
	if( chilelem ){
		FindNearestJointReq( srcxv, chilelem, distptr, ppnear, 1, ignoreflag, ignoreleng, needhaschild );
	}

	if( broflag == 1 ){
		CShdElem* broelem = chkelem->brother;
		if( broelem ){
			FindNearestJointReq( srcxv, broelem, distptr, ppnear, 1, ignoreflag, ignoreleng, needhaschild );
		}
	}

}

int CD3DDisp::IsExistValidBone( int srcseri, CPart* partptr, int* ignoreflag, int leng, int needhaschild )
{
	int bonenum = partptr->bonenum;
	int i;
	CBoneInfo* curbi = 0;
	int findflag = 0;

	for( i = 0; i < bonenum; i++ ){
		curbi = *(partptr->ppBI + i);
		//int jointno = curbi->jointno;
		int jointno = curbi->childno;//!!!!!!!!!!!!!!!!!!!!!!!
		if( (jointno >= 0) && (jointno < leng) ){
			int flag = *(ignoreflag + jointno);
			if( flag == 0 ){
				findflag = 1;
				break;
			}
		}
	}

	// end joint support
	if( (needhaschild == 0) && (findflag == 0) ){
		if( (srcseri >= 0) && (srcseri < leng) ){
			int flag = *(ignoreflag + srcseri);
			if( flag == 0 ){
				findflag = 1;
			}
		}
	}

	return findflag;
}

CBoneInfo* CD3DDisp::FindNearestBone( CShdElem* srcelem, D3DXVECTOR3* srcxv, int* ignoreflag, int ignoreleng )
{
	CPart* curpart = srcelem->part;
	_ASSERT( curpart );

	int bonenum = curpart->bonenum;
	int i;
	CBoneInfo* curbi = 0;
	CBoneInfo* nearestbi = 0;
	float mag, minmag, vecx, vecy, vecz;
	
	minmag = 1e8;
	mag = -1e8;

	for( i = 0; i < bonenum; i++ ){
		curbi = *(curpart->ppBI + i);
		//int jointno = curbi->jointno;
		int jointno = curbi->childno;//!!!!!!!!!!!!!!!!!!!!!!!!

		int chkflag = 0;
		if( (jointno >= 0) && (jointno < ignoreleng) ){
			chkflag = *(ignoreflag + jointno);
		}

		if( chkflag == 0 ){
			vecx = curbi->boneend.x - srcxv->x;
			vecy = curbi->boneend.y - srcxv->y;
			vecz = curbi->boneend.z - srcxv->z;
			mag = vecx * vecx + vecy * vecy + vecz * vecz;
			
			if( !nearestbi || (mag < minmag) ){
				minmag = mag;
				nearestbi = curbi;
			}
		}
	}
	return nearestbi;
}

CBoneInfo* CD3DDisp::FindNearestBoneMiko( CShdHandler* srclpsh, D3DXVECTOR3* srcv, CShdElem* srcselem, int centerflag )
{
	CBoneInfo* retbi = 0;

	int serino;
	CShdElem* curselem;
	CPart* curpart;

	D3DXVECTOR3 diff;
	float curdist;
	float mindist = 1e6;

	//CShdElem* chilelem;

	for( serino = 0; serino < srclpsh->s2shd_leng; serino++ ){
		curselem = (*srclpsh)( serino );

		if( curselem->IsJoint() && (curselem->type != SHDMORPH) ){
			curpart = curselem->part;
			if( !curpart ){
				DbgOut( "d3ddisp : FindNearestBoneMiko : curpart NULL error !!!\n" );
				_ASSERT( 0 );
				return 0;
			}

			if( curpart->bonenum >= 1 ){
				int bno;
				CBoneInfo* curbi;
				for( bno = 0; bno < curpart->bonenum; bno++ ){
					curbi = *( curpart->ppBI + bno );
				
					if( centerflag == 1 ){
						diff = curbi->bonecenter - *srcv;
					}else{
						diff.x = curpart->jointloc.x - srcv->x;
						diff.y = curpart->jointloc.y - srcv->y;
						diff.z = curpart->jointloc.z - srcv->z;
					}

					curdist = D3DXVec3Length( &diff );

					int isfloat;
					isfloat = 0;
/***
					//浮動ボーンは選ばない！！！
					chilelem = (*srclpsh)( curbi->childno );

					if( (srclpsh->m_bonetype == BONETYPE_MIKO) && chilelem ){
						if( (curselem->m_notransik) && (chilelem->m_notransik) ){
							isfloat = 1;
						}else{
							isfloat = 0;
						}
					}else{
						isfloat = 0;
					}
***/

					if( (curdist <= mindist) && (isfloat == 0) ){
						retbi = curbi;
						mindist = curdist;
					}
				}
			}
		}
	}

	return retbi;
}


int CD3DDisp::SetTriUV()
{
	DWORD pno;
	SKINVERTEX* curv = m_skinv;
	for( pno = 0; pno < m_numTLV; pno++ ){
		curv->tex1[0] = 0.0f;
		curv->tex1[1] = 0.0f;

		curv++;
	}


	return 0;
}


/***
int CD3DDisp::SetTriUV()
{
	DWORD pno;
	D3DXVECTOR3 center;
	center.x = 0.0f; center.y = 0.0f; center.z = 0.0f;
	SKINVERTEX* curv = m_skinv;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < m_numTLV; pno++ )
	{
		center.x += curv->pos[0];
		center.y += curv->pos[1];
		center.z += curv->pos[2];

		if( curv->pos[1] > maxy )
			maxy = curv->pos[1];

		if( curv->pos[1] < miny )
			miny = curv->pos[1];

		curv++;
	}
	center.x /= (float)m_numTLV;
	center.y /= (float)m_numTLV;
	center.z /= (float)m_numTLV;
	float ystep;
	if( maxy != miny )
		ystep = 1.0f / ( maxy - miny );
	else
		ystep = 0.0f;

	D3DXVECTOR2 base;
	base.x = 0.0f; base.y = -1.0f;// y にはz座標！！！

	curv = m_skinv;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	float dot, kaku, flccw;
	float xstep = 1.0f / ( 2.0f * PI );

	for( pno = 0; pno < m_numTLV; pno++ ){
		curv->tex1[0] = (curv->pos[1] - miny) * ystep;
				
		vec.x = curv->pos[0] - center.x;
		vec.y = curv->pos[2] - center.z;
		D3DXVec2Normalize( &nvec, &vec );

		dot = D3DXVec2Dot( &nvec, &base );
		if( dot > 1.0f )
			dot = 1.0f;
		if( dot < -1.0f )
			dot = -1.0f;
		kaku = (float)acos( dot );
		flccw = D3DXVec2CCW( &nvec, &base );
		if( flccw > 0.0f ){
			kaku = 2.0f * PI - kaku;
		}

		curv->tex1[1] = kaku * xstep;

		curv++;
	}


	return 0;
}
***/
int CD3DDisp::GetUV( int vertno, float* uptr, float* vptr )
{
	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : GetUV : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SKINVERTEX* curskinv;

	curskinv = m_skinv + vertno;
	*uptr = curskinv->tex1[0];
	*vptr = curskinv->tex1[1];

	return 0;
}

int CD3DDisp::SetUVArray( int* dirtyflag, int srctype, int srcrule, CPolyMesh2* pm2, CPolyMesh* pm )
{
	switch( srcrule ){
	case TEXRULE_X:
		SetUV_X( dirtyflag );
		break;
	case TEXRULE_Y:
		SetUV_Y( dirtyflag );
		break;
	case TEXRULE_Z:
		SetUV_Z( dirtyflag );
		break;
	case TEXRULE_CYLINDER:
		SetUV_Cylinder( dirtyflag );
		break;
	case TEXRULE_SPHERE:
		SetUV_Sphere( dirtyflag );
		break;
	case TEXRULE_MQ:
		if( pm2 ){
			SetUV_MQ( dirtyflag,  pm2, -1 );
		}else if( pm ){
			//if( (pm->groundflag) && (pm->uvbuf) )
			if( pm->uvbuf ){
				SetUV_Tex4( dirtyflag, pm, -1 );
			}else if( pm->billboardflag ){

				//billboardで、uvbuf有りは、SetUV_Tex4
				SetUV_Billboard( dirtyflag );
			}
		}
		break;
	default:
		break;
	}


	return 0;
}


int CD3DDisp::SetUV( int srctype, int srcrule, CPolyMesh2* pm2, CPolyMesh* pm, int vertno )
{
	switch( srcrule ){
	case TEXRULE_X:
		SetUV_X( 0 );
		break;
	case TEXRULE_Y:
		SetUV_Y( 0 );
		break;
	case TEXRULE_Z:
		SetUV_Z( 0 );
		break;
	case TEXRULE_CYLINDER:
		SetUV_Cylinder( 0 );
		break;
	case TEXRULE_SPHERE:
		SetUV_Sphere( 0 );
		break;
	case TEXRULE_MQ:
		if( pm2 ){
			SetUV_MQ( 0, pm2, vertno );
		}else if( pm ){
			//if( (pm->groundflag) && (pm->uvbuf) )
			if( pm->uvbuf ){
				SetUV_Tex4( 0, pm, vertno );
			}else if( pm->billboardflag ){

				//billboardで、uvbuf有りは、SetUV_Tex4
				SetUV_Billboard( 0 );
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

int CD3DDisp::SetUV_Billboard( int* dirtyflag )
{
	if( m_numTLV != 4 ){
		DbgOut( "d3ddisp : SetUV_Billboard : numTLV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	( m_skinv + 0 )->tex1[0] = 0.0f;
	( m_skinv + 0 )->tex1[1] = 1.0f;

	( m_skinv + 1 )->tex1[0] = 0.0f;
	( m_skinv + 1 )->tex1[1] = 0.0f;

	( m_skinv + 2 )->tex1[0] = 1.0f;
	( m_skinv + 2 )->tex1[1] = 1.0f;

	( m_skinv + 3 )->tex1[0] = 1.0f;
	( m_skinv + 3 )->tex1[1] = 0.0f;
		

	return 0;
}

int CD3DDisp::SetUV_Tex4( int* dirtyflag, CPolyMesh* pm, int vertno )
{
	if( !pm )
		return 0;

	DWORD pno;
	SKINVERTEX* curv = m_skinv;
	COORDINATE* srcuv = pm->uvbuf;
	SKINVERTEX* currevv = m_revskinv;

	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){

		for( pno = 0; pno < m_numTLV; pno++ ){

			float setu, setv;

			if( pm->m_texorgflag == 0 ){
				if( (srcuv->u >= 0.0f) && (srcuv->u <= 1.0f) ){
					setu = srcuv->u;
				}else{
					setu = 0.0f;
				}
				if( (srcuv->v >= 0.0f) && (srcuv->v <= 1.0f) ){
					setv = srcuv->v;
				}else{
					setv = 0.0f;
				}
			}else{
				setu = srcuv->u;
				setv = srcuv->v;
			}

			int doset;
			if( !dirtyflag ){
				doset = 1;
			}else{
				if( *(dirtyflag + pno) != 0 ){
					doset = 1;
				}else{
					doset = 0;
				}
			}

			if( doset ){
				curv->tex1[0] = setu;
				curv->tex1[1] = setv;

				if( m_revskinv ){
					currevv->tex1[0] = setu;
					currevv->tex1[1] = setv;
					//currevv++;
				}
			}

			curv++;
			srcuv++;
			if( currevv )
				currevv++;
		}

	}else{

		curv = m_skinv + vertno;
		srcuv = pm->uvbuf + vertno;
		currevv = m_revskinv + vertno;

		float setu, setv;

		if( pm->m_texorgflag == 0 ){
			if( (srcuv->u >= 0.0f) && (srcuv->u <= 1.0f) ){
				setu = srcuv->u;
			}else{
				setu = 0.0f;
			}
			if( (srcuv->v >= 0.0f) && (srcuv->v <= 1.0f) ){
				setv = srcuv->v;
			}else{
				setv = 0.0f;
			}
		}else{
			setu = srcuv->u;
			setv = srcuv->v;
		}

		curv->tex1[0] = setu;
		curv->tex1[1] = setv;

		if( m_revskinv ){
			currevv->tex1[0] = setu;
			currevv->tex1[1] = setv;
		}
	}

	return 0;
}

int CD3DDisp::SetUV_MQ( int* dirtyflag, CPolyMesh2* pm2, int vertno )
{
	if( !pm2 )
		return 0;

	DWORD pno;
	SKINVERTEX* curv = m_skinv;
	//COORDINATE* srcuv = pm2->uvbuf;
	D3DTLVERTEX* srctlv = pm2->opttlv;

	SKINVERTEX* currevv = m_revskinv;

	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		for( pno = 0; pno < m_numTLV; pno++ ){
			int doset;
			if( !dirtyflag ){
				doset = 1;
			}else{
				if( *(dirtyflag + pno) != 0 ){
					doset = 1;
				}else{
					doset = 0;
				}
			}

			if( doset ){
				curv->tex1[0] = srctlv->tu;
				curv->tex1[1] = srctlv->tv;

				if( m_revskinv ){
					currevv->tex1[0] = srctlv->tu;
					currevv->tex1[1] = srctlv->tv;
					//currevv++;
				}
			}
			curv++;
			srctlv++;
			if( m_revskinv )
				currevv++;
		}

	}else{
		curv = m_skinv + vertno;
		srctlv = pm2->opttlv + vertno;
		currevv = m_revskinv + vertno;

		curv->tex1[0] = srctlv->tu;
		curv->tex1[1] = srctlv->tv;


		if( m_revskinv ){
			currevv->tex1[0] = srctlv->tu;
			currevv->tex1[1] = srctlv->tv;
		}				
	}


	return 0;
}

int CD3DDisp::SetUV_X( int* dirtyflag )
{
	// U : Z, V : Y

	
	DWORD pno;
	SKINVERTEX* curv = m_skinv;
	SKINVERTEX* currevv = m_revskinv;
	float minz = 1e6;
	float maxz = -1e6;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < m_numTLV; pno++ )
	{
		if( curv->pos[2] > maxz )
			maxz = curv->pos[2];

		if( curv->pos[2] < minz )
			minz = curv->pos[2];

		if( curv->pos[1] > maxy )
			maxy = curv->pos[1];

		if( curv->pos[1] < miny )
			miny = curv->pos[1];

		curv++;
	}

	float ystep, zstep;
	if( maxy != miny )
		ystep = 1.0f / ( maxy - miny );
	else
		ystep = 0.0f;

	if( maxz != minz )
		zstep = 1.0f / ( maxz - minz );
	else
		zstep = 0.0f;

	curv = m_skinv;
	for( pno = 0; pno < m_numTLV; pno++ ){

		int doset;
		if( !dirtyflag ){
			doset = 1;
		}else{
			if( *(dirtyflag + pno) != 0 ){
				doset = 1;
			}else{
				doset = 0;
			}
		}

		if( doset ){
			curv->tex1[0] = (curv->pos[2] - minz) * zstep;
			curv->tex1[1] = 1.0f - (curv->pos[1] - miny) * ystep;

			if( m_revskinv ){
				currevv = m_revskinv + pno;
				currevv->tex1[0] = curv->tex1[0];
				currevv->tex1[1] = curv->tex1[1];
			}
		}
		curv++;
	}

	return 0;
}
int CD3DDisp::SetUV_Y( int* dirtyflag )
{
	// U : X, V : Z
	
	DWORD pno;
	SKINVERTEX* curv = m_skinv;
	SKINVERTEX* currevv = m_revskinv;
	float minz = 1e6;
	float maxz = -1e6;
	float minx = 1e6;
	float maxx = -1e6;
	for( pno = 0; pno < m_numTLV; pno++ )
	{
		if( curv->pos[2] > maxz )
			maxz = curv->pos[2];

		if( curv->pos[2] < minz )
			minz = curv->pos[2];

		if( curv->pos[0] > maxx )
			maxx = curv->pos[0];

		if( curv->pos[0] < minx )
			minx = curv->pos[0];

		curv++;
	}

	float zstep, xstep;
	if( maxz != minz )
		zstep = 1.0f / ( maxz - minz );
	else
		zstep = 0.0f;

	if( maxx != minx )
		xstep = 1.0f / ( maxx - minx );
	else
		xstep = 0.0f;

	curv = m_skinv;
	for( pno = 0; pno < m_numTLV; pno++ ){

		int doset;
		if( !dirtyflag ){
			doset = 1;
		}else{
			if( *(dirtyflag + pno) != 0 ){
				doset = 1;
			}else{
				doset = 0;
			}
		}

		if( doset ){

			curv->tex1[0] = (curv->pos[0] - minx) * xstep;
			curv->tex1[1] = 1.0f - (curv->pos[2] - minz) * zstep;

			if( m_revskinv ){
				currevv = m_revskinv + pno;
				currevv->tex1[0] = curv->tex1[0];
				currevv->tex1[1] = curv->tex1[1];
			}
		}
		curv++;
	}
	
	return 0;
}
int CD3DDisp::SetUV_Z( int* dirtyflag )
{
	// U : X, V : Y
	
	DWORD pno;
	SKINVERTEX* curv = m_skinv;
	SKINVERTEX* currevv = m_revskinv;
	float minx = 1e6;
	float maxx = -1e6;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < m_numTLV; pno++ )
	{
		if( curv->pos[0] > maxx )
			maxx = curv->pos[0];

		if( curv->pos[0] < minx )
			minx = curv->pos[0];

		if( curv->pos[1] > maxy )
			maxy = curv->pos[1];

		if( curv->pos[1] < miny )
			miny = curv->pos[1];

		curv++;
	}

	float xstep, ystep;
	if( maxy != miny )
		ystep = 1.0f / ( maxy - miny );
	else
		ystep = 0.0f;

	if( maxx != minx )
		xstep = 1.0f / ( maxx - minx );
	else
		xstep = 0.0f;

	curv = m_skinv;
	for( pno = 0; pno < m_numTLV; pno++ ){
		int doset;
		if( !dirtyflag ){
			doset = 1;
		}else{
			if( *(dirtyflag + pno) != 0 ){
				doset = 1;
			}else{
				doset = 0;
			}
		}

		if( doset ){
			curv->tex1[0] = (curv->pos[0] - minx) * xstep;
			curv->tex1[1] = 1.0f - (curv->pos[1] - miny) * ystep;

			if( m_revskinv ){
				currevv = m_revskinv + pno;
				currevv->tex1[0] = curv->tex1[0];
				currevv->tex1[1] = curv->tex1[1];
			}
		}
		curv++;
	}

	return 0;
}
int CD3DDisp::SetUV_Cylinder( int* dirtyflag )
{
	DWORD pno;
	D3DXVECTOR3 center;
	center.x = 0.0f; center.y = 0.0f; center.z = 0.0f;
	SKINVERTEX* curv = m_skinv;
	SKINVERTEX* currevv = m_revskinv;
	float miny = 1e6;
	float maxy = -1e6;
	for( pno = 0; pno < m_numTLV; pno++ )
	{
		center.x += curv->pos[0];
		center.y += curv->pos[1];
		center.z += curv->pos[2];

		if( curv->pos[1] > maxy )
			maxy = curv->pos[1];

		if( curv->pos[1] < miny )
			miny = curv->pos[1];

		curv++;
	}
	center.x /= (float)m_numTLV;
	center.y /= (float)m_numTLV;
	center.z /= (float)m_numTLV;
	float ystep;
	if( maxy != miny )
		ystep = 1.0f / ( maxy - miny );
	else
		ystep = 0.0f;

	D3DXVECTOR2 base;
	base.x = 0.0f; base.y = -1.0f;// y にはz座標！！！

	curv = m_skinv;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	float dot, kaku, flccw;
	float xstep = 1.0f / ( 2.0f * PI );

	for( pno = 0; pno < m_numTLV; pno++ ){
				
		vec.x = curv->pos[0] - center.x;
		vec.y = curv->pos[2] - center.z;
		D3DXVec2Normalize( &nvec, &vec );

		dot = D3DXVec2Dot( &nvec, &base );
		if( dot > 1.0f )
			dot = 1.0f;
		if( dot < -1.0f )
			dot = -1.0f;
		kaku = (float)acos( dot );
		flccw = D3DXVec2CCW( &nvec, &base );
		if( flccw > 0.0f ){
			kaku = 2.0f * PI - kaku;
		}

		int doset;
		if( !dirtyflag ){
			doset = 1;
		}else{
			if( *(dirtyflag + pno) != 0 ){
				doset = 1;
			}else{
				doset = 0;
			}
		}

		if( doset ){

			curv->tex1[0] = kaku * xstep;
			curv->tex1[1] = 1.0f - (curv->pos[1] - miny) * ystep;

			if( m_revskinv ){
				currevv = m_revskinv + pno;
				currevv->tex1[0] = curv->tex1[0];
				currevv->tex1[1] = curv->tex1[1];
			}
		}
		curv++;
	}

	return 0;
}
int CD3DDisp::SetUV_Sphere( int* dirtyflag )
{

	DWORD pno;
	D3DXVECTOR3 center;
	center.x = 0.0f; center.y = 0.0f; center.z = 0.0f;
	SKINVERTEX* curv = m_skinv;
	SKINVERTEX* currevv = m_revskinv;
	for( pno = 0; pno < m_numTLV; pno++ )
	{
		center.x += curv->pos[0];
		center.y += curv->pos[1];
		center.z += curv->pos[2];

		curv++;
	}
	center.x /= (float)m_numTLV;
	center.y /= (float)m_numTLV;
	center.z /= (float)m_numTLV;

	D3DXVECTOR2 base_xz;
	base_xz.x = 0.0f; base_xz.y = -1.0f;// y にはz座標！！！
	D3DXVECTOR2 base_xy;
	base_xy.x = 0.0f; base_xy.y = 1.0f;

	curv = m_skinv;
	D3DXVECTOR2 vec_xz;
	D3DXVECTOR2 nvec_xz;
	D3DXVECTOR2 vec_xy;
	D3DXVECTOR2 nvec_xy;
	float dot_xz, kaku_xz, flccw_xz;
	float dot_xy, kaku_xy, flccw_xy;
	float step = 1.0f / ( 2.0f * PI );

	for( pno = 0; pno < m_numTLV; pno++ ){
				
		vec_xz.x = curv->pos[0] - center.x;
		vec_xz.y = curv->pos[2] - center.z;
		D3DXVec2Normalize( &nvec_xz, &vec_xz );

		dot_xz = D3DXVec2Dot( &nvec_xz, &base_xz );
		if( dot_xz > 1.0f )
			dot_xz = 1.0f;
		if( dot_xz < -1.0f )
			dot_xz = -1.0f;

		kaku_xz = (float)acos( dot_xz );
		flccw_xz = D3DXVec2CCW( &nvec_xz, &base_xz );
		if( flccw_xz > 0.0f ){
			kaku_xz = 2.0f * PI - kaku_xz;
		}

	///////////
		vec_xy.x = curv->pos[0] - center.x;
		vec_xy.y = curv->pos[1] - center.y;
		D3DXVec2Normalize( &nvec_xy, &vec_xy );

		dot_xy = D3DXVec2Dot( &nvec_xy, &base_xy );
		if( dot_xy > 1.0f )
			dot_xy = 1.0f;
		if( dot_xy < -1.0f )
			dot_xy = -1.0f;
		kaku_xy = (float)acos( dot_xy );
		flccw_xy = D3DXVec2CCW( &nvec_xy, &base_xy );
		if( flccw_xy > 0.0f ){
			kaku_xy = 2.0f * PI - kaku_xy;
		}


		int doset;
		if( !dirtyflag ){
			doset = 1;
		}else{
			if( *(dirtyflag + pno) != 0 ){
				doset = 1;
			}else{
				doset = 0;
			}
		}

		if( doset ){
			curv->tex1[0] = kaku_xz * step;
			curv->tex1[1] = 1.0f - kaku_xy * step;

			if( m_revskinv ){
				currevv = m_revskinv + pno;
				currevv->tex1[0] = curv->tex1[0];
				currevv->tex1[1] = curv->tex1[1];
			}
		}

		curv++;
	}
	return 0;
}
int CD3DDisp::CheckRestVert()
{

	return 0;
}

int CD3DDisp::SetGroundObj( CMCache* mcache, CMotHandler* srclpmh, D3DXMATRIX matWorld )
{

	//!!!!!!!!!!!!!!!!!
	//変更時は、　SetGroundObj2 も変更すること！！！
	//!!!!!!!!!!!!!!!!!

	int ret;

	ret = TransformOnlyWorld( mcache, srclpmh, matWorld, CALC_WORLDV, 0, 0 );
	if( ret ){
		DbgOut( "d3ddisp : SetGroundObj : TransformOnlyWorld error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int i1, i2, i3;

	D3DXVECTOR3* v1;
	D3DXVECTOR3* v2;
	D3DXVECTOR3* v3;

	CBSphere* curbs = m_bs;
	CPCoef* curpc = m_pc;

	int faceno;
	for( faceno = 0; faceno < m_vnum; faceno++ ){
		i1 = *(m_dispIndices + faceno * 3);	
		i2 = *(m_dispIndices + faceno * 3 + 1);
		i3 = *(m_dispIndices + faceno * 3 + 2);

		v1 = m_worldv + i1;
		v2 = m_worldv + i2;
		v3 = m_worldv + i3;

		ret = curbs->SetBSFrom3V( v1, v2, v3 );
		if( ret ){
			DbgOut( "d3ddisp : SetGroundObj : SetBSFrom3V error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = curpc->SetPCFrom3V( v1, v2, v3 );
		if( ret ){
			DbgOut( "d3ddisp : SetGroundObj : SetPCFrom3V error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curbs++;
		curpc++;
	}


	return 0;
}

int CD3DDisp::SetGroundObj2( CMotHandler* srclpmh, D3DXMATRIX matWorld, int vertno )
{

	//!!!!!!!!!!!!!!!!!
	//変更時は、　SetGroundObj も変更すること！！！
	//!!!!!!!!!!!!!!!!!

	int ret;
	ret = TransformOnlyWorld2( srclpmh, matWorld, vertno );
	if( ret ){
		DbgOut( "d3ddisp : SetGroundObj2 : TransformOnlyWorld error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int i1, i2, i3;

	D3DXVECTOR3* v1;
	D3DXVECTOR3* v2;
	D3DXVECTOR3* v3;

	CBSphere* curbs = m_bs;
	CPCoef* curpc = m_pc;

	int faceno;
	for( faceno = 0; faceno < m_vnum; faceno++ ){
		i1 = *(m_dispIndices + faceno * 3);	
		i2 = *(m_dispIndices + faceno * 3 + 1);
		i3 = *(m_dispIndices + faceno * 3 + 2);

		// vertno を含む面のみ、変更する。
		if( (i1 == vertno) || (i2 == vertno) || (i3 == vertno) ){
			v1 = m_worldv + i1;
			v2 = m_worldv + i2;
			v3 = m_worldv + i3;

			ret = curbs->SetBSFrom3V( v1, v2, v3 );
			if( ret ){
				DbgOut( "d3ddisp : SetGroundObj : SetBSFrom3V error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = curpc->SetPCFrom3V( v1, v2, v3 );
			if( ret ){
				DbgOut( "d3ddisp : SetGroundObj : SetPCFrom3V error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		curbs++;
		curpc++;
	}


	return 0;
}

int CD3DDisp::TransformOnlyWorldMorph( CMotHandler* srclpmh, D3DXMATRIX matWorld, TEXV* dstvec, CInfElem* srcie, int* oldpno2optpno )
{
	//！！！！！！！！！！！！！！！！！
	// 変更時には、　TransformOnlyWorld, 2, 3 も変更すること　！！！！
	//！！！！！！！！！！！！！！！！！

	DWORD tlvno, optno;

	//int parno;
	CMatrix2 bonemat, parmat;
	D3DXMATRIX bonexmat, parxmat;
	D3DXMATRIX savemat1, savemat2;
	D3DXMATRIX* matSet1;
	//D3DXMATRIX* matSet2;
	//D3DXMATRIX mat;

	SKINVERTEX* srcskinv = m_skinv;

	//D3DTLVERTEX* dsttlv = m_dispTLV;

	CInfElem* curie;
	TEXV* curwv = dstvec;//!!!!!!!!!!!!!!!!!!!!!	

	//
	matWorld = m_scalemat * matWorld;//!!!!!!!!!!!!!!!!

	int curboneno;
	//int curparno;
	//D3DXMATRIX* mcptr;

	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		
		srcskinv = m_skinv + tlvno;
		optno = *(oldpno2optpno + tlvno);
		curie = srcie + optno;
		curwv = dstvec + optno;

		float x, y, z;
		x = srcskinv->pos[0];
		y = srcskinv->pos[1];
		z = srcskinv->pos[2];

		float xp = 0.0f;
		float yp = 0.0f;
		float zp = 0.0f;//, wp;

		int infno;
		for( infno = 0; infno < curie->infnum; infno++ ){
			INFELEM* curIE;
			curIE = curie->ie + infno;
			curboneno = curIE->bonematno;
			if( curboneno >= 0 ){
				bonemat = (*srclpmh)( curboneno )->curmat;
				MatConvD3DX( &bonexmat, bonemat );
				savemat1 = bonexmat * matWorld;
				matSet1 = &savemat1;

			}else{
				//bonemat = inimat;
				matSet1 = &matWorld;
			}


			float xpa, ypa, zpa;//, wpa;

			xpa = matSet1->_11*x + matSet1->_21*y + matSet1->_31*z + matSet1->_41;
			ypa = matSet1->_12*x + matSet1->_22*y + matSet1->_32*z + matSet1->_42;
			zpa = matSet1->_13*x + matSet1->_23*y + matSet1->_33*z + matSet1->_43;
			//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
			xp += xpa * curIE->dispinf;
			yp += ypa * curIE->dispinf;
			zp += zpa * curIE->dispinf;

		}



		curwv->pos.x = xp;
		curwv->pos.y = yp;
		curwv->pos.z = zp;
		curwv->uv.x = srcskinv->tex1[0];
		curwv->uv.y = srcskinv->tex1[1];

	}

	return 0;
}


int CD3DDisp::TransformOnlyWorld3( CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXVECTOR3* dstvec, CInfElem* srcie )
{
	//！！！！！！！！！！！！！！！！！
	// 変更時には、　TransformOnlyWorld, 2 も変更すること　！！！！
	//！！！！！！！！！！！！！！！！！

	DWORD tlvno;

	//int parno;
	CMatrix2 bonemat, parmat;
	D3DXMATRIX bonexmat, parxmat;
	D3DXMATRIX savemat1, savemat2;
	D3DXMATRIX* matSet1;
	//D3DXMATRIX* matSet2;
	//D3DXMATRIX mat;

	D3DLVERTEX* srclv = m_lv;
	SKINVERTEX* srcskinv = m_skinv;

	//D3DTLVERTEX* dsttlv = m_dispTLV;

	CInfElem* curie;
	if( srcie ){
		curie = srcie;
	}else{
		curie = m_IEptr;
	}
	
	D3DXVECTOR3* curwv = dstvec;//!!!!!!!!!!!!!!!!!!!!!	

	//
	matWorld = m_scalemat * matWorld;//!!!!!!!!!!!!!!!!

	int curboneno;
	//int curparno;
	//D3DXMATRIX* mcptr;

	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		
		float x, y, z;
		x = srcskinv->pos[0];
		y = srcskinv->pos[1];
		z = srcskinv->pos[2];

		float xp = 0.0f;
		float yp = 0.0f;
		float zp = 0.0f;//, wp;

		int infno;
		for( infno = 0; infno < curie->infnum; infno++ ){
			INFELEM* curIE;
			curIE = curie->ie + infno;
			curboneno = curIE->bonematno;
			if( curboneno >= 0 ){
				bonemat = (*srclpmh)( curboneno )->curmat;
				MatConvD3DX( &bonexmat, bonemat );
				savemat1 = bonexmat * matWorld;
				matSet1 = &savemat1;

			}else{
				//bonemat = inimat;
				matSet1 = &matWorld;
			}


			float xpa, ypa, zpa;//, wpa;

			xpa = matSet1->_11*x + matSet1->_21*y + matSet1->_31*z + matSet1->_41;
			ypa = matSet1->_12*x + matSet1->_22*y + matSet1->_32*z + matSet1->_42;
			zpa = matSet1->_13*x + matSet1->_23*y + matSet1->_33*z + matSet1->_43;
			//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
			xp += xpa * curIE->dispinf;
			yp += ypa * curIE->dispinf;
			zp += zpa * curIE->dispinf;

		}



		curwv->x = xp;
		curwv->y = yp;
		curwv->z = zp;
		curwv++;

		srcskinv++;

		curie++; // !!!!
	}

	return 0;
}


int CD3DDisp::TransformOnlyWorldInfOneBone( CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXVECTOR3 srcpos, D3DXVECTOR3* dstpos, int infboneno ){
	//！！！！！！！！！！！！！！！！！
	// 変更時には、　TransformOnlyWorld, 2 も変更すること　！！！！
	//！！！！！！！！！！！！！！！！！

	//int parno;
	CMatrix2 bonemat, parmat;
	D3DXMATRIX bonexmat, parxmat;
	D3DXMATRIX savemat1, savemat2;
	D3DXMATRIX* matSet1;
	
	matWorld = m_scalemat * matWorld;//!!!!!!!!!!!!!!!!
		
	float x, y, z;
	x = srcpos.x;
	y = srcpos.y;
	z = srcpos.z;

	float xp = 0.0f;
	float yp = 0.0f;
	float zp = 0.0f;//, wp;

	bonemat = (*srclpmh)( infboneno )->curmat;
	MatConvD3DX( &bonexmat, bonemat );
	savemat1 = bonexmat * matWorld;
	matSet1 = &savemat1;

	xp = matSet1->_11*x + matSet1->_21*y + matSet1->_31*z + matSet1->_41;
	yp = matSet1->_12*x + matSet1->_22*y + matSet1->_32*z + matSet1->_42;
	zp = matSet1->_13*x + matSet1->_23*y + matSet1->_33*z + matSet1->_43;

	dstpos->x = xp;
	dstpos->y = yp;
	dstpos->z = zp;

	return 0;
}




int CD3DDisp::TransformOnlyWorld( CMCache* mcache, CMotHandler* srclpmh, D3DXMATRIX matWorld, int calcflag, CBSphere* setbs, CBSphere* totalbs )
{

	//！！！！！！！！！！！！！！！！！
	// 変更時には、　TransformOnlyWorld2, 3 も変更すること　！！！！
	//！！！！！！！！！！！！！！！！！
	
	bsminx = 1e6;
	bsmaxx = -1e6;
	bsminy = 1e6;
	bsmaxy = -1e6;
	bsminz = 1e6;
	bsmaxz = -1e6;


	DWORD tlvno;

	//int parno;
	CMatrix2 bonemat, parmat;
	D3DXMATRIX bonexmat, parxmat;
	D3DXMATRIX savemat1, savemat2;
	D3DXMATRIX* matSet1;
	//D3DXMATRIX* matSet2;
	//D3DXMATRIX mat;

	D3DLVERTEX* srclv = m_lv;
	SKINVERTEX* srcskinv = m_skinv;

	//D3DTLVERTEX* dsttlv = m_dispTLV;

	CInfElem* curie = m_IEptr;
	
	D3DXVECTOR3* curwv = m_worldv;


	if( setbs ){
		setbs->rmag = 0.0f;
		//setbs->totalrmag = 0.0f;
	}
	

	//
	matWorld = m_scalemat * matWorld;//!!!!!!!!!!!!!!!!

	int curboneno;
	//int curparno;
	//D3DXMATRIX* mcptr;

	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		
		float x, y, z;
		x = srcskinv->pos[0];
		y = srcskinv->pos[1];
		z = srcskinv->pos[2];

		float xp = 0.0f; float yp = 0.0f; float zp = 0.0f;
		int infno;
		for( infno = 0; infno < curie->infnum; infno++ ){
			INFELEM* curIE;
			curIE = curie->ie + infno;
			curboneno = curIE->bonematno;
			if( curboneno >= 0 ){
				bonemat = (*srclpmh)( curboneno )->curmat;
				MatConvD3DX( &bonexmat, bonemat );
				savemat1 = bonexmat * matWorld;
				matSet1 = &savemat1;

			}else{
				//bonemat = inimat;
				matSet1 = &matWorld;
			}


			float xpa, ypa, zpa;//, wpa;

			xpa = matSet1->_11*x + matSet1->_21*y + matSet1->_31*z + matSet1->_41;
			ypa = matSet1->_12*x + matSet1->_22*y + matSet1->_32*z + matSet1->_42;
			zpa = matSet1->_13*x + matSet1->_23*y + matSet1->_33*z + matSet1->_43;
			//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
			xp += xpa * curIE->dispinf;
			yp += ypa * curIE->dispinf;
			zp += zpa * curIE->dispinf;

		}


		if( (calcflag & CALC_WORLDV) && curwv ){
			curwv->x = xp;
			curwv->y = yp;
			curwv->z = zp;

			curwv++;
		}

		if( xp < bsminx )
			bsminx = xp;
		if( xp > bsmaxx )
			bsmaxx = xp;

		if( yp < bsminy )
			bsminy = yp;
		if( yp > bsmaxy )
			bsmaxy = yp;

		if( zp < bsminz )
			bsminz = zp;
		if( zp > bsmaxz )
			bsmaxz = zp;
////////
		if( xp < s_tbsminx )
			s_tbsminx = xp;
		if( xp > s_tbsmaxx )
			s_tbsmaxx = xp;

		if( yp < s_tbsminy )
			s_tbsminy = yp;
		if( yp > s_tbsmaxy )
			s_tbsmaxy = yp;

		if( zp < s_tbsminz )
			s_tbsminz = zp;
		if( zp > s_tbsmaxz )
			s_tbsmaxz = zp;

		srcskinv++;

		curie++; // !!!!
	}

	if( (calcflag & CALC_CENTER_FLAG) && setbs ){
		if( m_numTLV != 0 ){
			setbs->befcenter.x = ( bsminx + bsmaxx ) * 0.5f; 
			setbs->befcenter.y = ( bsminy + bsmaxy ) * 0.5f; 
			setbs->befcenter.z = ( bsminz + bsmaxz ) * 0.5f;
			
			float diffx, diffy, diffz;
			diffx = ( bsmaxx - bsminx ) * 0.5f;
			diffy = ( bsmaxy - bsminy ) * 0.5f;
			diffz = ( bsmaxz - bsminz ) * 0.5f;

			setbs->rmag = (float)sqrt( diffx * diffx + diffy * diffy + diffz * diffz );

		}else{
			setbs->befcenter.x = 0.0f;
			setbs->befcenter.y = 0.0f;
			setbs->befcenter.z = 0.0f;

			setbs->rmag = 0.0f;
		}
	}
	

	return 0;
}
int CD3DDisp::TransformOnlyWorld2( CMotHandler* srclpmh, D3DXMATRIX matWorld, int vertno )
{

	//！！！！！！！！！！！！！！！！！
	// 変更時には、　TransformOnlyWorld, 3 も変更すること　！！！！
	//！！！！！！！！！！！！！！！！！
	
	
	int curboneno;
	//int parno;
	CMatrix2 bonemat, parmat;
	D3DXMATRIX bonexmat, parxmat;
	D3DXMATRIX matSet1;
	//D3DXMATRIX matSet2;
	//D3DXMATRIX mat;

	SKINVERTEX* srcskinv = 0;

	//D3DTLVERTEX* dsttlv = m_dispTLV;

	CInfElem* curie = m_IEptr + vertno;
	
	D3DXVECTOR3* curwv = m_worldv + vertno;

	srcskinv = m_skinv + vertno;

	matWorld = m_scalemat * matWorld;//!!!!!!!!!!!!!!!!


	float x, y, z;
	x = srcskinv->pos[0];
	y = srcskinv->pos[1];
	z = srcskinv->pos[2];


	float xp = 0.0f; float yp = 0.0f; float zp = 0.0f;
	int infno;
	for( infno = 0; infno < curie->infnum; infno++ ){
		INFELEM* curIE;
		curIE = curie->ie + infno;
		curboneno = curIE->bonematno;
		if( curboneno >= 0 ){
			bonemat = (*srclpmh)( curboneno )->curmat;
			MatConvD3DX( &bonexmat, bonemat );
			matSet1 = bonexmat * matWorld;

		}else{
			//bonemat = inimat;
			matSet1 = matWorld;
		}


		float xpa, ypa, zpa;//, wpa;

		xpa = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
		ypa = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
		zpa = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;
		//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
		xp += xpa * curIE->dispinf;
		yp += ypa * curIE->dispinf;
		zp += zpa * curIE->dispinf;

	}

	curwv->x = xp;
	curwv->y = yp;
	curwv->z = zp;

	//curwv++;

	//srctlv++;
	//curie++; // !!!!


	return 0;
}

int CD3DDisp::ChkConfVecAndFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds )
{
	*confpointnum = 0;//!!!!!!!!!!!!!!!!!

	if( m_linekind != 0 ){
		DbgOut( "d3ddisp : ChkConfVecAndFace : line object is not supported warning !!!\n" );
		_ASSERT( 0 );
		*confpointnum = 0;
		return 0;//!!!!
	}

	if( !m_worldv ){
		DbgOut( "d3ddisp : ChkConfVecAndFace : worldv NULL error !!!\n" );
		_ASSERT( 0 );
		*confpointnum = 0;
		return 1;
	}


	int ret;
	CBSphere curbs;
	CPCoef curpc;
	
	float diffx, diffy, diffz;
	float dist;
	diffx = newpos->x - befpos->x;
	diffy = newpos->y - befpos->y;
	diffz = newpos->z - befpos->z;
	
	dist = sqrtf( diffx * diffx + diffy * diffy + diffz * diffz ) * 0.50f;

	D3DXVECTOR3 srccenter;
	srccenter.x = ( befpos->x + newpos->x ) * 0.50f;
	srccenter.y = ( befpos->y + newpos->y ) * 0.50f;
	srccenter.z = ( befpos->z + newpos->z ) * 0.50f;


	int i1, i2, i3;
	D3DXVECTOR3 v1;
	D3DXVECTOR3 v2;
	D3DXVECTOR3 v3;

	int faceno;
	int confno = 0;//!!!!


	static int dbgflag = 0;

	DISTSAMPLE neards;
	neards.setflag = 0;
	neards.dist = 1e10;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;


	for( faceno = 0; faceno < m_vnum; faceno++ ){

		i1 = *(m_dispIndices + faceno * 3);	
		i2 = *(m_dispIndices + faceno * 3 + 1);
		i3 = *(m_dispIndices + faceno * 3 + 2);

		v1.x = (m_worldv + i1)->x;
		v1.y = (m_worldv + i1)->y;
		v1.z = (m_worldv + i1)->z;

		v2.x = (m_worldv + i2)->x;
		v2.y = (m_worldv + i2)->y;
		v2.z = (m_worldv + i2)->z;
			
		v3.x = (m_worldv + i3)->x;
		v3.y = (m_worldv + i3)->y;
		v3.z = (m_worldv + i3)->z;


//球判定
		
		int result1;
		ret = curbs.SetBSFrom3V( &v1, &v2, &v3 );
		if( ret ){
			DbgOut( "d3ddisp : ChkConfVecAndFace : bs SetBSFrom3V error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = curbs.ChkConflict( &srccenter, dist, &result1 );
		if( ret ){
			DbgOut( "D3DDisp : ChkConfVecAndFace : curbs ChkConflict error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		//result1 = 1;

		if( result1 ){

//詳細判定	
			ret = curpc.SetPCFrom3V( &v1, &v2, &v3 );
			if( ret ){
				DbgOut( "d3ddisp : ChkConfVecAndFace : pc SetPCFrom3V error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( (curpc.m_a == 0.0f) && (curpc.m_b == 0.0f) && (curpc.m_c == 0.0f) ){
				//何もしない
			}else{
				int result2;
				CONFDATA curcd;
				ret = ChkConfVecAndFace2( befpos, newpos, &curpc, &v1, &v2, &v3, &result2, &curcd );
				if( ret ){
					DbgOut( "D3DDisp : ChkConfVecAndFace : ChkConfVecAndFace2 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( result2 ){
					if( confno >= confmaxnum ){
						DbgOut( "D3DDisp : ChkConfVecAndFace : confno error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					*( confdata + confno ) = curcd;
					(confdata + confno)->faceno = faceno;
					

	//for dist sample
					if( ds ){
						float d0;//, d1, d2;
						/***
						d0 = (float)fabs( curpc.m_a * newpos->x + curpc.m_b * newpos->y + curpc.m_c * newpos->z + curpc.m_d );
						//d1 = sqrtf( curpc->m_a * curpc->m_a + curpc->m_b * curpc->m_b + curpc->m_c * curpc->m_c );
						//d2 = d0 / d1;
						
						//curpc の法線ベクトルは、正規化してあるので、sqrtf( ... は必要ない。
						***/
						D3DXVECTOR3 diff;
						diff = *befpos - curcd.adjustv;
						d0 = sqrtf( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );

						if( neards.dist > d0 ){
							neards.setflag = 1;
							neards.dist = d0;
							neards.nv.x = curpc.m_a;
							neards.nv.y = curpc.m_b;
							neards.nv.z = curpc.m_c;
						}
					}

					confno++;
				}
			}
		}
	}


	*confpointnum = confno;///!!!!!!!!!!! 


	if( ds )
		*ds = neards;

	dbgflag = 1;

	return 0;
}


int CD3DDisp::ChkConfVecAndGroundFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds )
{
	int ret;
	CBSphere* curbs;
	CPCoef* curpc;
	
	float diffx, diffy, diffz;
	float dist;
	diffx = newpos->x - befpos->x;
	diffy = newpos->y - befpos->y;
	diffz = newpos->z - befpos->z;
	
	dist = sqrtf( diffx * diffx + diffy * diffy + diffz * diffz ) * 0.50f;

	D3DXVECTOR3 srccenter;
	srccenter.x = ( befpos->x + newpos->x ) * 0.50f;
	srccenter.y = ( befpos->y + newpos->y ) * 0.50f;
	srccenter.z = ( befpos->z + newpos->z ) * 0.50f;


	int i1, i2, i3;
	D3DXVECTOR3* v1;
	D3DXVECTOR3* v2;
	D3DXVECTOR3* v3;

	int faceno;
	int confno = 0;//!!!!


	static int dbgflag = 0;

	DISTSAMPLE neards;
	neards.setflag = 0;
	neards.dist = 1e10;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;

	for( faceno = 0; faceno < m_vnum; faceno++ ){
		curbs = m_bs + faceno;
		curpc = m_pc + faceno;

//if( dbgflag == 0 ){
//	DbgOut( "d3ddisp : CHkConfVecAndGroundFace : faceno %d, curpc %f %f %f %f\n",
//		faceno, curpc->m_a, curpc->m_b, curpc->m_c, curpc->m_d );
//}


//球判定
		
		int result1;
		
		ret = curbs->ChkConflict( &srccenter, dist, &result1 );
		if( ret ){
			DbgOut( "D3DDisp : ChkConfVecAndGroundFace : curbs ChkConflict error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		//result1 = 1;

		if( result1 ){
			
			/***
//for dist sample
			if( ds ){
				float d0;//, d1, d2;
				d0 = (float)fabs( curpc->m_a * newpos->x + curpc->m_b * newpos->y + curpc->m_c * newpos->z + curpc->m_d );
				//d1 = sqrtf( curpc->m_a * curpc->m_a + curpc->m_b * curpc->m_b + curpc->m_c * curpc->m_c );
				//d2 = d0 / d1;
				
				//curpc の法線ベクトルは、正規化してあるので、sqrtf( ... は必要ない。
				

				if( neards.dist > d0 ){
					neards.setflag = 1;
					neards.dist = d0;
					neards.nv.x = curpc->m_a;
					neards.nv.y = curpc->m_b;
					neards.nv.z = curpc->m_c;
				}
			}
			***/
			
//詳細判定	
			i1 = *(m_dispIndices + faceno * 3);	
			i2 = *(m_dispIndices + faceno * 3 + 1);
			i3 = *(m_dispIndices + faceno * 3 + 2);

			v1 = m_worldv + i1;
			v2 = m_worldv + i2;
			v3 = m_worldv + i3;

			int result2;
			CONFDATA curcd;
			ret = ChkConfVecAndFace2( befpos, newpos, curpc, v1, v2, v3, &result2, &curcd );
			if( ret ){
				DbgOut( "D3DDisp : ChkConfVecAndGroundFace : ChkConfVecAndFace2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( result2 ){
				if( confno >= confmaxnum ){
					DbgOut( "D3DDisp : ChkConfVecAndGroundFace : confno error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( confdata + confno ) = curcd;
				(confdata + confno)->faceno = faceno;
				
//for dist sample
				if( ds ){
					float d0;//, d1, d2;
					/***
					d0 = (float)fabs( curpc->m_a * newpos->x + curpc->m_b * newpos->y + curpc->m_c * newpos->z + curpc->m_d );
					//d1 = sqrtf( curpc->m_a * curpc->m_a + curpc->m_b * curpc->m_b + curpc->m_c * curpc->m_c );
					//d2 = d0 / d1;
					
					//curpc の法線ベクトルは、正規化してあるので、sqrtf( ... は必要ない。
					***/

					D3DXVECTOR3 diff;
					diff = *befpos - curcd.adjustv;
					d0 = sqrtf( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z );

					if( neards.dist > d0 ){
						neards.setflag = 1;
						neards.dist = d0;
						neards.nv.x = curpc->m_a;
						neards.nv.y = curpc->m_b;
						neards.nv.z = curpc->m_c;
					}
				}
				
				confno++;
			}
		}
	}


	*confpointnum = confno;

	if( ds )
		*ds = neards;

	dbgflag = 1;

	return 0;
}

int CD3DDisp::SetOrgScale( D3DXVECTOR3 scalevec, D3DXVECTOR3 centervec )
{

	int ret;


	ret = SetTLVScale( scalevec, centervec );
	if( ret ){
		DbgOut( "d3ddisp : SetOrgScale : SetTLVScale rev error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CD3DDisp::SetTLVScale( D3DXVECTOR3 scalevec, D3DXVECTOR3 centervec )
{

	//m_scale = scalevec;
	//m_scalecen = center;

	D3DXMATRIX beftram, afttram, scalem;

	D3DXMatrixIdentity( &beftram );
	D3DXMatrixIdentity( &afttram );
	D3DXMatrixIdentity( &scalem );

	beftram._41 = -centervec.x;
	beftram._42 = -centervec.y;
	beftram._43 = -centervec.z;

	afttram._41 = centervec.x;
	afttram._42 = centervec.y;
	afttram._43 = centervec.z;

	scalem._11 = scalevec.x;
	scalem._22 = scalevec.y;
	scalem._33 = scalevec.z;

	m_scalemat = beftram * scalem * afttram;

	return 0;
}

int CD3DDisp::TransformOnlyWorld1VertCurrent( CMotHandler* srclpmh, D3DXMATRIX matWorld, int vertno, D3DXVECTOR3* dstpos, int scaleflag, CInfElem* ieptr )
{
	if( (vertno < 0) || ((DWORD)vertno > m_numTLV) ){
		DbgOut( "d3ddisp : TransformOnlyWorld1VertCurrent : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//DWORD tlvno;
	//int parno;
	CMatrix2 bonemat;//, parmat;
	D3DXMATRIX bonexmat;//, parxmat;
	D3DXMATRIX matSet1;//, matSet2;
	D3DXMATRIX mat;

	SKINVERTEX* skinv = 0;

	CInfElem* curie;
	if( !ieptr ){
		curie = m_IEptr + vertno;
	}else{
		curie = ieptr + vertno;
	}

	int curboneno;
	//mat = m_scalemat * matWorld * matView * matProj;

	if( scaleflag == 0 ){
		mat = matWorld;
	}else{
		mat = m_scalemat * matWorld;
	}
	

	float x, y, z;
	skinv = m_skinv + vertno;
		
	x = skinv->pos[0];
	y = skinv->pos[1];
	z = skinv->pos[2];


	float xp = 0.0f; float yp = 0.0f; float zp = 0.0f;
	int infno;
	for( infno = 0; infno < curie->infnum; infno++ ){
		INFELEM* curIE;
		curIE = curie->ie + infno;
		curboneno = curIE->bonematno;

		if( curboneno >= 0 ){
			CMotionCtrl* mc;
			mc = (*srclpmh)( curboneno );
			if( !mc ){
				DbgOut( "d3ddisp : TransformOnlyWorld1Vert : mc NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			bonemat = mc->curmat;
		}else{
			bonemat.Identity();
		}
		MatConvD3DX( &bonexmat, bonemat );
		matSet1 = bonexmat * mat;

		float xpa, ypa, zpa;//, wpa;

		xpa = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
		ypa = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
		zpa = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;
			//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
		xp += xpa * curIE->dispinf;
		yp += ypa * curIE->dispinf;
		zp += zpa * curIE->dispinf;

	}



	dstpos->x = xp;
	dstpos->y = yp;
	dstpos->z = zp;

	return 0;

}



int CD3DDisp::TransformOnlyWorld1Vert( CMotHandler* srclpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag, CInfElem* ieptr )
{
	if( (vertno < 0) || ((DWORD)vertno > m_numTLV) ){
		DbgOut( "d3ddisp : TransformOnlyWorld1Vert : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//DWORD tlvno;
	//int parno;
	CMatrix2 bonemat;//, parmat;
	D3DXMATRIX bonexmat;//, parxmat;
	D3DXMATRIX matSet1;//, matSet2;
	D3DXMATRIX mat;

	SKINVERTEX* skinv = 0;

	CInfElem* curie;
	if( !ieptr ){
		curie = m_IEptr + vertno;
	}else{
		curie = ieptr + vertno;
	}

	int curboneno;
	//mat = m_scalemat * matWorld * matView * matProj;

	if( scaleflag == 0 ){
		mat = matWorld;
	}else{
		mat = m_scalemat * matWorld;
	}
	

	float x, y, z;
	skinv = m_skinv + vertno;
		
	x = skinv->pos[0];
	y = skinv->pos[1];
	z = skinv->pos[2];


	float xp = 0.0f; float yp = 0.0f; float zp = 0.0f;
	int infno;
	for( infno = 0; infno < curie->infnum; infno++ ){
		INFELEM* curIE;
		curIE = curie->ie + infno;
		curboneno = curIE->bonematno;

		if( curboneno >= 0 ){
			CMotionCtrl* mc;
			CMotionInfo* mi;
			CMatrix2* bm;
			mc = (*srclpmh)( curboneno );
			if( !mc ){
				DbgOut( "d3ddisp : TransformOnlyWorld1Vert : mc NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			mi = mc->motinfo;
			if( !mi ){
				DbgOut( "d3ddisp : TransformOnlyWorld1Vert : mi NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( (srclpmh->m_kindnum >= 1) && (motid >= 0) && (mi->kindnum >= 1) ){
				bm = (*mi)( motid, frameno );
				bonemat = *bm;
			}else{
				bonemat = mc->curmat;
			}
			
		}else{
			bonemat.Identity();
		}
		MatConvD3DX( &bonexmat, bonemat );
		matSet1 = bonexmat * mat;

		float xpa, ypa, zpa;//, wpa;

		xpa = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
		ypa = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
		zpa = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;
			//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
		xp += xpa * curIE->dispinf;
		yp += ypa * curIE->dispinf;
		zp += zpa * curIE->dispinf;

	}



	dstpos->x = xp;
	dstpos->y = yp;
	dstpos->z = zp;

	return 0;
}


int CD3DDisp::SetDispTlvAlpha( float srcalpha )
{
	m_alpha = srcalpha;
	if( m_alpha > 1.0f )
		m_alpha = 1.0f;
	if( m_alpha < 0.0f )
		m_alpha = 0.0f;

/***
	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb = 0;
		LPDIRECT3DVERTEXBUFFER9 currevvb = 0;
			
		curvb = *( m_ArrayVB + blno );
		if( m_revArrayVB ){
			currevvb = *( m_revArrayVB + blno );
		}


		SKINVERTEX* pSkinv = 0;
		SKINVERTEX* prevSkinv = 0;
		if( FAILED( curvb->Lock( 0, 0, (void**)&pSkinv, NULL ) ) ){
			_ASSERT( 0 );
			return 1;
		}			
		if( currevvb ){
			if( FAILED( currevvb->Lock( 0, 0, (void**)&prevSkinv, NULL ) ) ){
				_ASSERT( 0 );
				return 1;
			}			
		}

		SKINVERTEX* skinv;
		DWORD vertno;
		DWORD oldcol, newcol;
		for( vertno = 0; vertno < m_numTLV; vertno++ ){

			skinv = m_skinv + vertno;

			oldcol = skinv->diffuse;
			newcol = oldcol & 0x00FFFFFF;
			newcol |= (ialpha << 24);
			(pSkinv + vertno)->diffuse = newcol;
			skinv->diffuse = newcol;//!!!!!

			if( prevSkinv ){
				(prevSkinv + vertno)->diffuse = newcol;
				(m_revskinv + vertno)->diffuse = newcol;//!!!!
			}
		}

		curvb->Unlock();
		if( currevvb ){
			currevvb->Unlock();
		}
	}
***/

	return 0;

}

int CD3DDisp::SetOrgTlvColor( int vertno, DWORD dwcol, int colkind )
{
	int ret;
	if( vertno < 0 ){
		int vno;
		for( vno = 0; vno < (int)m_numTLV; vno++ ){
			ret = SetOrgTlvColor( vno, dwcol, colkind );
			_ASSERT( !ret );
		}
		return 0;
	}

/////////////
	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : SetOrgTlvColor : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//DWORD oldcol;
	int newr, newg, newb;
	//newa = (dwcol & 0xFF000000) >> 24;
	newr = (dwcol & 0x00FF0000) >> 16;
	newg = (dwcol & 0x0000FF00) >> 8;
	newb = (dwcol & 0x000000FF);

	
	LPDIRECT3DVERTEXBUFFER9 curvb = m_VBmaterial;

	SKINMATERIAL* pSkinv = 0;
	if( FAILED( curvb->Lock( 0, 0, (void**)&pSkinv, NULL ) ) ){
		_ASSERT( 0 );
		return 1;
	}			

	SKINMATERIAL* skinv;
	//DWORD vertno;
	DWORD oldcol, newcol;
	//for( vertno = 0; vertno < m_numTLV; vertno++ ){

		skinv = m_smaterial + vertno;

		if( colkind == MATERIAL_DIFFUSE ){
			oldcol = skinv->diffuse;
			newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
			skinv->diffuse = newcol;
			(pSkinv + vertno)->diffuse = newcol;
		}

		if( colkind == MATERIAL_SPECULAR ){
			oldcol = skinv->specular;
			newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
			skinv->specular = newcol;
			(pSkinv + vertno)->specular = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
		}

		if( colkind == MATERIAL_AMBIENT ){
			oldcol = skinv->ambient;
			newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
			skinv->ambient = newcol;
			(pSkinv + vertno)->ambient = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
		}

		if( colkind == MATERIAL_EMISSIVE ){
			oldcol = skinv->emissive;
			newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
			skinv->emissive = newcol;
			(pSkinv + vertno)->emissive = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
		}

	curvb->Unlock();


	return 0;
}

int CD3DDisp::SetOrgTlvPower( int vertno, float srcpower )
{

	int ret;
	if( vertno < 0 ){
		int vno;
		for( vno = 0; vno < (int)m_numTLV; vno++ ){
			ret = SetOrgTlvPower( vno, srcpower );
			_ASSERT( !ret );
		}
		return 0;
	}

/////////////
	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : SetOrgTlvPower : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	LPDIRECT3DVERTEXBUFFER9 curvb = m_VBmaterial;

	SKINMATERIAL* pSkinv = 0;
	if( FAILED( curvb->Lock( 0, 0, (void**)&pSkinv, NULL ) ) ){
		_ASSERT( 0 );
		return 1;
	}			

	SKINMATERIAL* skinv;

	skinv = m_smaterial + vertno;
	skinv->power = srcpower;
	(pSkinv + vertno)->power = srcpower;

	curvb->Unlock();

	return 0;
}

int CD3DDisp::SetOrgTlvColorArray( int* dirtyptr, DWORD dwcol, int colkind )
{
	//DWORD oldcol;
	int newr, newg, newb;
	//newa = (dwcol & 0xFF000000) >> 24;
	newr = (dwcol & 0x00FF0000) >> 16;
	newg = (dwcol & 0x0000FF00) >> 8;
	newb = (dwcol & 0x000000FF);

	

	LPDIRECT3DVERTEXBUFFER9 curvb = m_VBmaterial;

	SKINMATERIAL* pSkinv = 0;
	if( FAILED( curvb->Lock( 0, 0, (void**)&pSkinv, NULL ) ) ){
		_ASSERT( 0 );
		return 1;
	}			

	SKINMATERIAL* skinv;
	DWORD oldcol, newcol;
	DWORD vertno;
	for( vertno = 0; vertno < m_numTLV; vertno++ ){

		if( *( dirtyptr + vertno ) != 0 ){
			skinv = m_smaterial + vertno;

			if( colkind == MATERIAL_DIFFUSE ){
				oldcol = skinv->diffuse;
				newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
				skinv->diffuse = newcol;
				(pSkinv + vertno)->diffuse = newcol;
			}

			if( colkind == MATERIAL_SPECULAR ){
				oldcol = skinv->specular;
				newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
				skinv->specular = newcol;
				(pSkinv + vertno)->specular = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
			}

			if( colkind == MATERIAL_AMBIENT ){
				oldcol = skinv->ambient;
				newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
				skinv->ambient = newcol;
				(pSkinv + vertno)->ambient = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
			}

			if( colkind == MATERIAL_EMISSIVE ){
				oldcol = skinv->emissive;
				newcol = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
				skinv->emissive = newcol;
				(pSkinv + vertno)->emissive = D3DCOLOR_RGBA( newr, newg, newb, (oldcol >> 24) );
			}
		}
	}

	curvb->Unlock();

	return 0;
}
int CD3DDisp::SetOrgTlvPowerArray( int* dirtyptr, float srcpower )
{

	LPDIRECT3DVERTEXBUFFER9 curvb = m_VBmaterial;

	SKINMATERIAL* pSkinv = 0;
	if( FAILED( curvb->Lock( 0, 0, (void**)&pSkinv, NULL ) ) ){
		_ASSERT( 0 );
		return 1;
	}			

	SKINMATERIAL* skinv;

	DWORD vertno;
	for( vertno = 0; vertno < m_numTLV; vertno++ ){
		if( *( dirtyptr + vertno ) != 0 ){

			skinv = m_smaterial + vertno;
			skinv->power = srcpower;
			(pSkinv + vertno)->power = srcpower;

		}
	}

	curvb->Unlock();

	return 0;
}

int CD3DDisp::UpdateExtLine( CExtLine* extline, float alpha )
{
	CMeshInfo* mi;
	mi = extline->meshinfo;

	m_unum = mi->m;
	m_vnum = mi->n;

	//m_linekind = mi->mclosed;//!!!!!!!!!
	//DbgOut( "d3ddisp : CreateDispData extline : linekind %d\n", m_linekind );

	//m_numTLV = m_vnum;
	//m_numIndices = m_vnum;

	int ret;
	ret = SetExtLineLV( extline );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispData[ extline ] : SetExtLineLV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_linekind == D3DPT_LINELIST ){
		m_numPrim = m_unum / 2;//!!!!!!!!
	}else if( m_linekind == D3DPT_LINESTRIP ){
		m_numPrim = m_unum - 1;//!!!!!!!!
	}


	D3DXMATRIX inimat;
	D3DXMatrixIdentity( &inimat );
	ret = InitColor( extline, alpha, inimat );
	if( ret ){
		DbgOut( "d3ddisp : UpdateExtLine : InitColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Copy2VertexBuffer( 0 );
	if( ret ){
		DbgOut( "d3ddisp : UpdateExtLine : Copy2VertexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CD3DDisp::SetBIM( CShdHandler* srclpsh, int srcseri, int srcvertno, int srcchildno, int srcparentno, int srccalcmode )
{
	if( !m_IEptr || !m_skinv )
		return 1;

	int ret = 0;
	
	/////////////
	if( srcvertno == -1 ){
		DWORD vno;
		for( vno = 0; vno < m_numTLV; vno++ ){
			ret = SetBIM( srclpsh, srcseri, vno, srcchildno, srcparentno, srccalcmode );
			if( ret ){
				DbgOut( "d3ddisp : SetBIM : srcvertno -1 : SetBIM error %d !!!\n", vno );
				_ASSERT( 0 );
				return 1;
			}
		}

		return 0;//!!!!!!!!!!!!!!
	}


	/////////////


	if( (srcvertno < 0) || (srcvertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : SetBIM : vertno error %d !!!\n", srcvertno );
		_ASSERT( 0 );
		return 1;
	}


	SKINVERTEX* curskinv;
	D3DXVECTOR3 curxv;
	CShdElem* nearestjoint;
	CBoneInfo* nearestbone;
	CInfElem* curie;

	CShdElem* parselem;


	CShdElem* curselem = (*srclpsh)( srcseri );


	if( srccalcmode == CALCMODE_ONESKIN0 ){

		curskinv = m_skinv + srcvertno;
		curxv.x = curskinv->pos[0];
		curxv.y = curskinv->pos[1];
		curxv.z = curskinv->pos[2];

		if( srcparentno > 0 ){
			//一番近いjoint(bonenum 1以上)
			nearestjoint = (*srclpsh)( srcparentno );

			//一番近いボーン
			nearestbone = FindNearestBone( nearestjoint, srcchildno );

			//parselem = (*srclpsh)( srcparentno );
			parselem = srclpsh->FindUpperJoint( nearestjoint );

		}else{
			nearestbone = 0;
			parselem = 0;
		}

		//InfElemをセットする。
		curie = m_IEptr + srcvertno;


		ret = curie->SetInfElemDefault( nearestbone, parselem, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
		if( ret ){
			DbgOut( "CD3DDisp : SetBIM : SetInfElemDefault error !!!\n" );
			return 1;
		}
	
	}else if( srccalcmode == CALCMODE_NOSKIN0 ){
		// カクカク曲げ


		curskinv = m_skinv + srcvertno;
		curxv.x = curskinv->pos[0];
		curxv.y = curskinv->pos[1];
		curxv.z = curskinv->pos[2];

		if( srcparentno > 0 ){
			//一番近いjoint(bonenum 1以上)
			nearestjoint = (*srclpsh)( srcparentno );

			//一番近いボーン
			nearestbone = FindNearestBone( nearestjoint, srcchildno );

			//parselem = (*srclpsh)( srcparentno );
			parselem = srclpsh->FindUpperJoint( nearestjoint );

		}else{
			nearestbone = 0;
			parselem = 0;
		}

			
		//InfElemをセットする。
		curie = m_IEptr + srcvertno;

		ret = curie->SetInfElemDefault( nearestbone, 0, &curxv, (curselem->m_mikodef != MIKODEF_NONE) );
		if( ret ){
			DbgOut( "CD3DDisp : SetBIM : SetInfElemDefault error !!!\n" );
			return 1;
		}
	}

	return 0;
}

CBoneInfo* CD3DDisp::FindNearestBone( CShdElem* srcelem, int srcchildno )
{
	CPart* curpart = srcelem->part;
	_ASSERT( curpart );

	int bonenum = curpart->bonenum;
	int i;
	CBoneInfo* curbi = 0;
	CBoneInfo* nearestbi = 0;

	for( i = 0; i < bonenum; i++ ){
		curbi = *(curpart->ppBI + i);

		if( curbi->childno == srcchildno ){
			nearestbi = curbi;
			break;
		}
	}

	return nearestbi;
}

int CD3DDisp::GetDispScreenPos( int vertno, D3DXVECTOR3* posptr, int tlmode )
{
	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : GetDispScreenPos : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( m_scv );
	*posptr = *( m_scv + vertno );
	
	return 0;
}
int CD3DDisp::CalcScreenPos( int bbflag, CMotHandler* srclpmh, int vertno, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int tlmode, D3DXVECTOR3* dstpos )
{
	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : CalcScreenPos : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DTLVERTEX dsttlv;
	float x, y, z;
	int curboneno;//, curparno;
	CInfElem* curie;
	
	x = (m_skinv + vertno)->pos[0];
	y = (m_skinv + vertno)->pos[1];
	z = (m_skinv + vertno)->pos[2];

	if( m_IEptr ){
		curie = (m_IEptr + vertno);
	}else{
		curie = 0;
	}

//	curie = 0;


	D3DXMATRIX mat;
	if( bbflag == 0 ){
		mat = m_scalemat * matWorld * matView;
	}else{
		mat = m_scalemat * m_matWorld * matView;
	}

	float width = (float)dwClipWidth * 2.0f;
	float height = (float)dwClipHeight * 2.0f;

	float flClipWidth = (float)dwClipWidth;
	float flClipHeight = (float)dwClipHeight;

	float projfar, divprojfar;
	projfar = -matProj._43 / ( matProj._33 - 1.0f );
	divprojfar = -( matProj._33 - 1.0f ) / matProj._43;

	CMatrix2 inimat;
	inimat.Identity();

	CMatrix2 bonemat;//, parmat;
	D3DXMATRIX bonexmat;//, parxmat;
	D3DXMATRIX* matSet1;
	//D3DXMATRIX* matSet2;
	D3DXMATRIX savemat1;//, savemat2;


	//if( curboneno >= 0 ){
	//	bonemat = (*srclpmh)( curboneno )->curmat;
	//	MatConvD3DX( &bonexmat, bonemat );
	//	savemat1 = bonexmat * mat;
	//	matSet1 = &savemat1;
	//}else{
	//	//bonemat = inimat;
	//	matSet1 = &mat;
	//}

	//if( curparno >= 0 ){
	//	parmat = (*srclpmh)( curparno )->curmat;
	//	MatConvD3DX( &parxmat, parmat );
	//	savemat2 = parxmat * mat;
	//	matSet2 = &savemat2;
	//}else{
	//	//parmat = inimat;
	//	matSet2 = &mat;
	//}


	float xp = 0.0f;
	float yp = 0.0f;
	float zp = 0.0f;//, wp;


	if( curie ){


		int infno;
		for( infno = 0; infno < curie->infnum; infno++ ){
			INFELEM* curIE;
			curIE = curie->ie + infno;
			curboneno = curIE->bonematno;
			if( curboneno >= 0 ){
				bonemat = (*srclpmh)( curboneno )->curmat;
				MatConvD3DX( &bonexmat, bonemat );
				savemat1 = bonexmat * mat;
				matSet1 = &savemat1;
			}else{
				//bonemat = inimat;
				matSet1 = &mat;
			}

			float xpa, ypa, zpa;//, wpa;

			xpa = matSet1->_11*x + matSet1->_21*y + matSet1->_31*z + matSet1->_41;
			ypa = matSet1->_12*x + matSet1->_22*y + matSet1->_32*z + matSet1->_42;
			zpa = matSet1->_13*x + matSet1->_23*y + matSet1->_33*z + matSet1->_43;
			//wpa = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
						
			xp += xpa * curIE->dispinf;
			yp += ypa * curIE->dispinf;
			zp += zpa * curIE->dispinf;

		}
	}else{

		matSet1 = &mat;
		
		xp = matSet1->_11*x + matSet1->_21*y + matSet1->_31*z + matSet1->_41;
		yp = matSet1->_12*x + matSet1->_22*y + matSet1->_32*z + matSet1->_42;
		zp = matSet1->_13*x + matSet1->_23*y + matSet1->_33*z + matSet1->_43;
		//wp = matSet1->_14*x + matSet1->_24*y + matSet1->_34*z + matSet1->_44;
	}

	float xp2, yp2, zp2, wp2;


//	if( matProj._43 < -1.0f ){ 			
//		xp2 = xp * matProj._11;
//		yp2 = yp * matProj._22;
//		zp2 = zp * matProj._33 + matProj._43;
//		wp2 = zp;
//	}else{
		//正射影

		xp2 = matProj._11*xp + matProj._21*yp + matProj._31*zp + matProj._41;
		yp2 = matProj._12*xp + matProj._22*yp + matProj._32*zp + matProj._42;
		zp2 = matProj._13*xp + matProj._23*yp + matProj._33*zp + matProj._43;
		wp2 = matProj._14*xp + matProj._24*yp + matProj._34*zp + matProj._44;
//	}


	//xp2 = xp * matProj._11;
	//yp2 = yp * matProj._22;
	//zp2 = zp * matProj._33 + matProj._43;

	ConvScreenPos( PROJ_NORMAL, xp2, yp2, zp2, wp2, flClipHeight, flClipWidth, aspect, &dsttlv );

	/***
	switch( projmode ){
	case PROJ_NORMAL:

		ConvScreenPos( PROJ_NORMAL, xp2, yp2, zp2, zp, flClipHeight, flClipWidth, aspect, dsttlv );
		break;

	case PROJ_LENS:

		ConvScreenPos( PROJ_LENS, xp2, yp2, zp2, zp, flClipHeight, flClipWidth, aspect, dsttlv );
		break;
	case PROJ_PREINIT:
		break;
	}
	***/


	dstpos->x = dsttlv.sx;
	dstpos->y = dsttlv.sy;
	dstpos->z = dsttlv.sz;

	return 0;
}



int CD3DDisp::PickVert( int srctype, int srcseri, int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr )
{
	int ret;

//	if( !m_dispTLV ){
//		DbgOut( "d3ddisp : PickVert : dispTLV not exist error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


	D3DXVECTOR3* curscv;
	DWORD tlvno;

//DbgOut( "check!!!: d3ddisp : pickvert\r\n" );

	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		
		curscv = m_scv + tlvno;
		ret = pickptr->PickVert( srcseri, (int)tlvno, posx, posy, rangex, rangey, curscv, maxgetnum, getnumptr );
		if( ret ){
			DbgOut( "d3ddisp : PickVert : GPU : pickptr PickVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//DbgOut( "\r\n" );


	return 0;
}

int CD3DDisp::GetRevCullingFlag( int srctype, int vertno, int* revcullptr )
{
	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : GetRevCullingFlag : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	*revcullptr = 0;

	return 0;
}
int CD3DDisp::GetVertNoOfFace( int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr )
{

	if( m_linekind != 0 ){
		DbgOut( "d3ddisp : GetVertNoOfFace : line object is not support warning !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!
	}

	if( (faceno < 0) || (faceno >= m_vnum) ){
		DbgOut( "d3ddisp : GetVertNoOfFace : faceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*vert1ptr = *(m_dispIndices + faceno * 3);	
	*vert2ptr = *(m_dispIndices + faceno * 3 + 1);
	*vert3ptr = *(m_dispIndices + faceno * 3 + 2);

	return 0;
}

int CD3DDisp::GetSamePosVert( int vertno, int* sameptr, int arrayleng, int* samenumptr )
{

	*samenumptr = 0;

	if( m_linekind != 0 ){
		DbgOut( "d3ddisp : GetSamePosVert : line object is not support warning !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!
	}

	if( (vertno < 0) || (vertno >= (int)m_numTLV) ){
		DbgOut( "d3ddisp : GetSamePosVert : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 srcv;
	srcv.x = (m_skinv + vertno)->pos[0];
	srcv.y = (m_skinv + vertno)->pos[1];
	srcv.z = (m_skinv + vertno)->pos[2];


	DWORD tlvno;
	D3DXVECTOR3 curv;


	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		
		curv.x = (m_skinv + tlvno)->pos[0];
		curv.y = (m_skinv + tlvno)->pos[1];
		curv.z = (m_skinv + tlvno)->pos[2];
	
		if( (curv == srcv) && ((int)tlvno != vertno) ){
			if( arrayleng <= *samenumptr ){
				DbgOut( "d3ddisp : GetSamePosVert : arrayleng too short warning !!!\n" );
				_ASSERT( 0 );
				break;
			}

			*( sameptr + *samenumptr ) = tlvno;
			(*samenumptr)++;
		}
	}


	return 0;
}

int CD3DDisp::CalcMeshWorldv( int* seri2boneno, CMotHandler* srclpmh, int serialno, D3DXMATRIX* firstscale, D3DXMATRIX* matWorldptr )
{

	if( !m_worldv ){
		DbgOut( "d3ddisp : CalcMeshWorldv : worldv NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !matWorldptr ){
		DbgOut( "d3ddisp : CalcMeshWorldv : matworldptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	DWORD tlvno;

	D3DXMATRIX matSet1;


	SKINVERTEX* skinv = m_skinv;
	D3DXMATRIX swmat = m_scalemat * *matWorldptr;
	D3DXVECTOR3* curwv = m_worldv;

	CInfElem* curie = m_IEptr;

	//m_matView = matView;// !!! InitColor での、カリングに使用。
	//m_matProj = matProj;


	CMotionCtrl* melem = (*srclpmh)( serialno );
	CMatrix2 inimat;
	int curboneno;
	inimat.Identity();


	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		
		float x, y,z;
		x = skinv->pos[0];
		y = skinv->pos[1];
		z = skinv->pos[2];

		//float invwp;

		float xp = 0.0f;
		float yp = 0.0f;
		float zp = 0.0f;//, wp;

		if( seri2boneno != 0 ){				
			int infno;
			for( infno = 0; infno < curie->infnum; infno++ ){
				INFELEM* curIE;
				curIE = curie->ie + infno;
				curboneno = max( curIE->bonematno, 0 );
				if( curboneno > 0 ){
					CMotionCtrl* curmc;
					CMatrix2* curmatptr;
					D3DXMATRIX curxmat;
					curmc = (*srclpmh)( curboneno );
					curmatptr = &(curmc->curmat);
					MatConvD3DX( &curxmat, curmatptr );
					matSet1 = curxmat * swmat;
				}else{
					matSet1 = swmat;
				}

				float xpa, ypa, zpa;//, wpa;

				xpa = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
				ypa = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
				zpa = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;
				//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
							
				xp += xpa * curIE->dispinf;
				yp += ypa * curIE->dispinf;
				zp += zpa * curIE->dispinf;
			}
		}else{

			//float xpa, ypa, zpa;//, wpa;

			xp = swmat._11*x + swmat._21*y + swmat._31*z + swmat._41;
			yp = swmat._12*x + swmat._22*y + swmat._32*z + swmat._42;
			zp = swmat._13*x + swmat._23*y + swmat._33*z + swmat._43;
			//wp = swmat._14*x + swmat._24*y + swmat._34*z + swmat._44;

		}
		curwv->x = xp;//!!!!!!!!
		curwv->y = yp;
		curwv->z = zp;


		skinv++;
		
		if( curie ){
			curie++; // !!!!
		}
		curwv++;//!!!!!

	}

	return 0;
}

int CD3DDisp::FillDispColor( DWORD srccol, CPolyMesh2* pm2 )
{
	int ret;
	DWORD tlvno;

	SKINMATERIAL* curskinv;
	if( m_smaterial ){
		for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
			curskinv = m_smaterial + tlvno;
			curskinv->diffuse = srccol;
		}

		if( !pm2 ){
			ret = Copy2VertexBuffer( 0 );
			if( ret ){
				DbgOut( "d3ddisp : FillDispColor : Copy2VertexColor error !!!\n" );
				return 1;
			}
		}else{
			ret = Copy2VertexBufferMorph( pm2, 1 );
			if( ret ){
				DbgOut( "d3ddisp : FillDispColor : Copy2VertexBufferMorph error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		//_ASSERT( 0 );
	}

	return 0;
}

int CD3DDisp::GetInitialPartPos( D3DXVECTOR3* partpos )
{
	DWORD vno;
	D3DXVECTOR3 temppos( 0.0f, 0.0f, 0.0f );

	SKINVERTEX* curskinv;

	for( vno = 0; vno < m_numTLV; vno++ ){
		curskinv = m_skinv + vno;

		temppos.x += curskinv->pos[0];
		temppos.y += curskinv->pos[1];
		temppos.z += curskinv->pos[2];
	}

	if( m_numTLV != 0 ){
		*partpos = temppos / (float)m_numTLV;
	}else{
		*partpos = temppos;
	}

	return 0;
}

int CD3DDisp::GetNormal( D3DXVECTOR3* normalptr )
{
	if( m_orgNormal && normalptr ){
		memcpy( normalptr, m_orgNormal, sizeof( D3DXVECTOR3 ) * m_numTLV );
	}

	return 0;
}

int CD3DDisp::CalcInitialNormal( CPolyMesh2* pm2 )
{


	D3DXVECTOR3* tmpw;
	tmpw = new D3DXVECTOR3 [m_numTLV];
	if( !tmpw ){
		DbgOut( "d3ddisp : CalcInitialNormal : tmpw alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	DWORD tlvno;
	for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
		( tmpw + tlvno )->x = ( m_skinv + tlvno )->pos[0];
		( tmpw + tlvno )->y = ( m_skinv + tlvno )->pos[1];
		( tmpw + tlvno )->z = ( m_skinv + tlvno )->pos[2];
	}

	int ret;
	if( pm2 ){
		ret = SetTriNormalPM2_nolight( tmpw, pm2 );
		if( ret ){
			DbgOut( "d3ddisp : CalcInitialNormal : SetTriNormalPM2_nolight error !!!\n" );
			_ASSERT( 0 );
			free( tmpw );
			return 1;
		}
	}else{
		ret = SetSkinNormalPM();
		if( ret ){
			DbgOut( "d3ddisp : CalcInitialNormal : SetSkinNormalPM error !!!\n" );
			_ASSERT( 0 );
			free( tmpw );
			return 1;
		}
	}

	free( tmpw );

	//// 裏面
	if( m_revNormal ){
		int vno;
		for( vno = 0; vno < (int)m_numTLV; vno++ ){
			*(m_revNormal + vno) = -*(m_orgNormal + vno);
		}
	}


	return 0;
}

int CD3DDisp::CalcNormalByWorldV( CPolyMesh2* pm2 )
{
	if( !m_orgNormal ){
		DbgOut( "d3ddisp : CalcNormalByWorldV : object NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = SetTriNormalPM2_nolight( m_worldv, pm2 );
	if( ret ){
		DbgOut( "d3ddisp : CalcInitialNormal : SetTriNormalPM2_nolight error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//// 裏面
	if( m_revNormal ){
		int vno;
		for( vno = 0; vno < (int)m_numTLV; vno++ ){
			*(m_revNormal + vno) = -*(m_orgNormal + vno);
		}
	}

	return 0;
}

int CD3DDisp::GetRenderPrimNum( int* numptr )
{
	*numptr = 0;


	*numptr = m_numPrim;

	if( m_revIndices ){
		(*numptr) += m_numPrim;
	}


	return 0;
}



int CD3DDisp::SetIndexBuffer( int* srcindex, int srcclockwise )
{
	if( !m_dispIndices ){
		DbgOut( "d3ddisp : SetAndUpdateIndexBuffer : dispindices NULL error !!!\n");
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	if( srcclockwise != 3 ){
		ret = SetTriIndices( m_dispIndices, srcclockwise, m_vnum, srcindex );
		if( ret ){
			DbgOut( "CD3DDisp : SetAndUpdateIndexBuffer : SetTriIndices error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( srcclockwise == 1 ){
			ret = SetTriIndices( m_revIndices, 2, m_vnum, srcindex );
			if( ret ){
				DbgOut( "CD3DDisp : SetAndUpdateIndexBuffer : SetTriIndices error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = SetTriIndices( m_revIndices, 1, m_vnum, srcindex );
			if( ret ){
				DbgOut( "CD3DDisp : SetAndUpdateIndexBuffer : SetTriIndices error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		ret = SetTriIndices( m_dispIndices, 2, m_vnum, srcindex );
		if( ret ){
			DbgOut( "CD3DDisp : SetAndUpdateIndexBuffer : SetTriIndices 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = SetTriIndices( m_revIndices, 1, m_vnum, srcindex );
		if( ret ){
			DbgOut( "CD3DDisp : SetAndUpdateIndexBuffer : SetTriIndices 2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CD3DDisp::GetSkinMeshHeader( int leng, int* maxpervert, int* maxperface )
{
	int* dirtyflag;
	dirtyflag = (int*)malloc( sizeof( int ) * leng );
	if( !dirtyflag ){
		DbgOut( "d3ddisp : GetSkinMeshHeader ; dirtyflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int pervert = 0;
	int perface = 0;

	int faceno;
	int i[3];
	for( faceno = 0; faceno < m_vnum; faceno++ ){
		i[0] = *(m_dispIndices + faceno * 3);
		i[1] = *(m_dispIndices + faceno * 3 + 1);
		i[2] = *(m_dispIndices + faceno * 3 + 2);

		ZeroMemory( dirtyflag, sizeof( int ) * leng );

		int index;
		for( index = 0; index < 3; index++ ){
			CInfElem* curie;
			curie = m_IEptr + i[index];

			int infnum;
			infnum = curie->infnum;

			if( infnum >= pervert ){
				pervert = infnum;
			}


			int infno;
			for( infno = 0; infno < infnum; infno++ ){
				INFELEM* curIE;
				curIE = curie->ie + infno;
				
				if( curIE->bonematno > 0 ){
					*(dirtyflag + curIE->bonematno) = 1;
				}
			}

		}

		int tmpcnt = 0;
		int seri;
		for( seri = 0; seri < leng; seri++ ){
			if( *(dirtyflag + seri) == 1 ){
				tmpcnt++;
			}
		}

		if( tmpcnt > perface ){
			perface = tmpcnt;
		}

	}

	*maxpervert = pervert;
	*maxperface = perface;

	free( dirtyflag );

	return 0;
}

int CD3DDisp::SaveToDispTempDiffuse()
{

	_ASSERT( m_tempdiffuse );
	_ASSERT( m_smaterial );

	DWORD tlvno;
	SKINMATERIAL* curskinv;
	if( m_tempdiffuse && m_smaterial){
		for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
			curskinv = m_smaterial + tlvno;
			*( m_tempdiffuse + tlvno ) = curskinv->diffuse;
		}
	}

	return 0;
}
int CD3DDisp::RestoreDispTempDiffuse( CPolyMesh2* pm2 )
{
	
	_ASSERT( m_tempdiffuse );
	_ASSERT( m_smaterial );

	DWORD tlvno;
	SKINMATERIAL* curskinv;
	if( m_tempdiffuse && m_smaterial){
		for( tlvno = 0; tlvno < m_numTLV; tlvno++ ){
			curskinv = m_smaterial + tlvno;
			curskinv->diffuse = *( m_tempdiffuse + tlvno );
		}
	}

	int ret;
	if( !pm2 ){
		ret = Copy2VertexBuffer( 0 );
		if( ret ){
			DbgOut( "d3ddisp : RestoreDispTempDiffuse : Copy2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		ret = Copy2VertexBufferMorph( pm2, 1 );
		if( ret ){
			DbgOut( "d3ddisp : RestoreDispTempDiffuse : Copy2VertexBufferMorph error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CD3DDisp::GetFaceNormal( int faceno, D3DXMATRIX matWorld, CMotHandler* lpmh, D3DXVECTOR3* dstn )
{
	if( (faceno < 0) || (faceno >= m_vnum) ){
		DbgOut( "d3ddisp : GetFaceNormal : faceno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int i1, i2, i3;
	i1 = *( m_dispIndices + faceno * 3 );
	i2 = *( m_dispIndices + faceno * 3 + 1);
	i3 = *( m_dispIndices + faceno * 3 + 2);

	int kindnum = lpmh->m_kindnum;
	int motid = lpmh->m_curmotkind;
	int frameno = lpmh->m_curframeno;
	
	int ret;
	D3DXVECTOR3 v1, v2, v3;

	if( (kindnum >= 1) && (motid >= 0) ){
		ret = TransformOnlyWorld1Vert( lpmh, matWorld, motid, frameno, i1, &v1, 1, 0 );
		_ASSERT( !ret );
		ret = TransformOnlyWorld1Vert( lpmh, matWorld, motid, frameno, i2, &v2, 1, 0 );
		_ASSERT( !ret );
		ret = TransformOnlyWorld1Vert( lpmh, matWorld, motid, frameno, i3, &v3, 1, 0 );
		_ASSERT( !ret );
	}else{

		//モーションがない場合

		D3DXMATRIX matSet1;
		matSet1 = m_scalemat * matWorld;

		SKINVERTEX* skinv;

		float x, y, z;

		skinv = m_skinv + i1;		
		x = skinv->pos[0];
		y = skinv->pos[1];
		z = skinv->pos[2];
		v1.x = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
		v1.y = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
		v1.z = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;

		skinv = m_skinv + i2;		
		x = skinv->pos[0];
		y = skinv->pos[1];
		z = skinv->pos[2];
		v2.x = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
		v2.y = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
		v2.z = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;

		skinv = m_skinv + i3;		
		x = skinv->pos[0];
		y = skinv->pos[1];
		z = skinv->pos[2];
		v3.x = matSet1._11*x + matSet1._21*y + matSet1._31*z + matSet1._41;
		v3.y = matSet1._12*x + matSet1._22*y + matSet1._32*z + matSet1._42;
		v3.z = matSet1._13*x + matSet1._23*y + matSet1._33*z + matSet1._43;
	}

	ret = CalcNormal( dstn, &v1, &v2, &v3 );
	_ASSERT( !ret );

//	DbgOut( "check !!! : d3ddisp : GetFaceNormal : motid %d, frameno %d, (%f, %f, %f)\n",
//		motid, frameno, dstn->x, dstn->y, dstn->z );


	return 0;
}


CLightData* CD3DDisp::GetLightData( int lid )
{
	CLightData* retlight = 0;
	CLightData* curlight = g_lighthead;

	while( curlight ){
		if( curlight->lightid == lid ){
			retlight = curlight;
			break;
		}
		curlight = curlight->nextlight;
	}

	return retlight;
}

//int CD3DDisp::CreateDispData( LPDIRECT3DDEVICE9 pd3dDevice, CPolyMesh2* polymesh2, 
//	int clockwise, int srcrepx, int srcrepy, int tlmode, float facet )
int CD3DDisp::CreateDispDataMPM2( LPDIRECT3DDEVICE9 pd3dDevice, CMorph* morph, int tlmode )
{
	int ret;

	m_pdev = pd3dDevice;
	m_clockwise = morph->m_baseelem->clockwise;
	float facet = morph->m_baseelem->facet;
	int clockwise = morph->m_baseelem->clockwise;

	//int adjustuvflag = polymesh2->groundflag;
	int adjustuvflag = 0;//<---- この変数の使用は止める。polymesh2->adjustuvflagを使う。
	m_TLmode = tlmode;

//	ret = polymesh2->CreateOptDataBySamePointBuf( adjustuvflag, facet );
//	if( ret ){
//		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : CreateOptDataBySamePointBuf error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}
	
	int pointnum, facenum;
	
	//pointnum = mi->n * 3;
	pointnum = morph->m_facenum * 3;//!!!
	facenum = morph->m_facenum;

	m_unum = 3;
	m_vnum = facenum;

	m_numTLV = pointnum;
	m_numIndices = facenum * 3;
	
	ret = CreateVertexAndIndex( clockwise );
	if( ret ){
		DbgOut( "d3ddisp : CreateDispData : pm2 : CreateVertexAndIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	ret = polymesh2->CreateInfElemIfNot( (int)m_numTLV );
//	if( ret ){
//		DbgOut( "d3ddisp : CreateDispData : polymesh2 : pm2 CreateInfElemIfNot error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}
	m_IEptr = morph->m_baseelem->polymesh2->m_IE;
	_ASSERT( m_IEptr );


	ret = SetSkinDecl( pd3dDevice );
	_ASSERT( !ret );

	m_worldv = new D3DXVECTOR3 [ m_numTLV ];
	if( !m_worldv ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_worldv alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_worldv, sizeof( D3DXVECTOR3 ) * m_numTLV );


	m_scv = new D3DXVECTOR3 [ m_numTLV ];
	if( !m_worldv ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_scv alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_scv, sizeof( D3DXVECTOR3 ) * m_numTLV );

	m_tempdiffuse = new DWORD [ m_numTLV ];
	if( !m_tempdiffuse ){
		DbgOut( "D3DDisp : CraeteDispData[ polymesh2 ] : m_tempdiffuse alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_tempdiffuse, sizeof( DWORD ) * m_numTLV );


	m_faceNormal = new D3DXVECTOR3 [ m_vnum ];
	if( !m_faceNormal ){
		DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_faceNormal alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_faceNormal, sizeof( D3DXVECTOR3 ) * m_vnum );


	m_faceBinormal = new D3DXVECTOR3[ m_vnum ];
	if( !m_faceBinormal ){
		DbgOut( "d3ddisp : CreateDispData pm2 : faceBinormal alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_faceBinormal, sizeof( D3DXVECTOR3 ) * m_vnum );

	if( clockwise == 3 ){
		m_revfaceBinormal = new D3DXVECTOR3[ m_vnum ];
		if( !m_revfaceBinormal ){
			DbgOut( "d3ddisp : CreateDispData pm2 : revfaceBinormal alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ZeroMemory( m_revfaceBinormal, sizeof( D3DXVECTOR3 ) * m_vnum );
	}


	if( morph->m_baseelem->polymesh2->groundflag == 1 ){

		m_bs = new CBSphere [ m_vnum ];
		if( !m_bs ){
			DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_bs alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_pc = new CPCoef [ m_vnum ];
		if( !m_pc ){
			DbgOut( "D3DDisp : CreateDispData[ polymesh2 ] : m_pc alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	ret = SetPolyMesh2SkinVMorph( morph->m_baseelem->polymesh2 );
	if( ret ){
		DbgOut( "CD3DDisp : CreateDispData[ polymesh2 ] : SetPolyMesh2TLV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_numPrim = facenum;

	if( clockwise != 3 )
		totalPrim += m_numPrim;
	else
		totalPrim += m_numPrim * 2;
	pracPrim = totalPrim - invalidPrim;
	DbgOut( "CD3DDisp : CreateDispData : totalPrim %d, invalidPrim %d, pracPrim %d\n", 
		totalPrim, invalidPrim, pracPrim );

//	_ASSERT( 0 );

	return 0;
}

int CD3DDisp::CreateMorphSkinMat( CShdHandler* lpsh, CPolyMesh2* pm2 )
{
	if( !m_IEptr || !m_skinv || !m_dispIndices ){
		DbgOut( "d3ddisp : CreateSkinMat : data NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_numTLV < 3 ){
		DbgOut( "d3ddisp : CreateSkinMat : tlvnum less than 3 ; skip\n" );
		return 0;//!!!!!!!!!
	}

	int limitnum;
	if( m_blurmode == BLUR_NONE ){
		limitnum = MAXSKINMATRIX;
	}else{
		limitnum = MAXBLURMATRIX;
	}

//DbgOut( "d3ddisp : CreateSkinMat : 0\n" );

	DestroySkinMat();

	int leng = lpsh->s2shd_leng;

	int ifaceno, startfaceno;;
	int index[3];
	int optindex[3];
	int blockno = 0;

	int* bonemat2skinmat;
	int* skinmat2bonemat;
	int* dirtyflag;
	D3DXMATRIX* skinmat;

	int hasjointflag;


	dirtyflag = (int*)malloc( sizeof( int ) * leng );
	if( !dirtyflag ){
		DbgOut( "d3ddisp : CreateSkinMat : dirtyflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	startfaceno = 0;
	while( startfaceno < m_vnum ){
		hasjointflag = 0;

		mskinblock = (SKINBLOCK*)realloc( mskinblock, sizeof( SKINBLOCK ) * (blockno + 1) );
		if( !mskinblock ){
			DbgOut( "d3ddisp : CreateSkinMat : skinblock alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		(mskinblock + blockno)->startface = startfaceno;

		bonemat2skinmat = (int*)malloc( sizeof( int ) * leng );
		if( !bonemat2skinmat ){
			DbgOut( "d3ddisp : CreateSkinMat : bonemat2skinmat alloc error !!!\n" );
			_ASSERT( 0 );
			free( dirtyflag );
			return 1;
		}
		ZeroMemory( dirtyflag, sizeof( int ) * leng );
		ZeroMemory( bonemat2skinmat, sizeof( int ) * leng );

		int skinmatnum;
		skinmatnum = 0;

		for( ifaceno = startfaceno; ifaceno < m_vnum; ifaceno++ ){
			index[0] = *(m_dispIndices + ifaceno * 3);
			index[1] = *(m_dispIndices + ifaceno * 3 + 1);
			index[2] = *(m_dispIndices + ifaceno * 3 + 2);
			optindex[0] = *( pm2->oldpno2optpno + index[0] );
			optindex[1] = *( pm2->oldpno2optpno + index[1] );
			optindex[2] = *( pm2->oldpno2optpno + index[2] );

			int ino;
			int ieno;
			int addmatnum = 0;
			int addboneno[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			for( ino = 0; ino < 3; ino++ ){
				CInfElem* curie = m_IEptr + optindex[ino];
				
				if( curie->infnum > 4 ){
					char messtr[1024];
					sprintf_s( messtr, 1024, "ひとつの頂点に作用できるボーンの数は、４個までです。\n影響ボーンの数が多すぎるので処理できません。エラー終了します。" );
					::MessageBox( NULL, messtr, "エラーです", MB_OK );
					free( dirtyflag );
					free( bonemat2skinmat );
					_ASSERT( 0 );
					return 1;
				}

				for( ieno = 0; ieno < curie->infnum; ieno++ ){
					INFELEM* curIE = curie->ie + ieno;

					if( (curIE->bonematno > 0) && (*(dirtyflag + curIE->bonematno) == 0) ){

						int findsame = 0;
						int ano;
						for( ano = 0; ano < addmatnum; ano++ ){
							if( addboneno[ano] == curIE->bonematno ){
								findsame = 1;
								break;
							}
						}
						if( findsame == 0 ){
							addboneno[addmatnum] = curIE->bonematno;
							addmatnum++;
						}
					}
				}
			}
			if( (skinmatnum + addmatnum + 1) >= limitnum ){
				hasjointflag = ifaceno;
				break;
			}else{
				skinmatnum += addmatnum;
				int addno;
				for( addno = 0; addno < addmatnum; addno++ ){
					if( *(dirtyflag + addboneno[addno]) == 1 ){
						DbgOut( "d3ddisp : CreateSkinMat : dirtyflag error %d\r\n", addboneno[addno] );
						_ASSERT( 0 );
						free( dirtyflag );
						free( bonemat2skinmat );
						return 1;
					}
					*(dirtyflag + addboneno[addno]) = 1;
				}
			}
		}
		(mskinblock + blockno)->endface = ifaceno;//!!!!!!!!! 次の最初の番号

		startfaceno = ifaceno;
		skinmatnum += 1;// index 0 の分。
		*(dirtyflag + 0) = 1;
////////////////
		skinmat2bonemat = (int*)malloc( sizeof( int ) * skinmatnum );
		if( !skinmat2bonemat ){
			DbgOut( "d3ddisp : CreateSkinMat : skinmat2bonemat alloc error !!!\n" );
			_ASSERT( 0 );
			free( dirtyflag );
			free( bonemat2skinmat );
			return 1;
		}
		ZeroMemory( skinmat2bonemat, sizeof( int ) * skinmatnum );


		int elemno;
		*(skinmat2bonemat + 0) = 0;
		int setno = 1;

		for( elemno = 1; elemno < leng; elemno++ ){
			if( *(dirtyflag + elemno) == 1 ){
				*(bonemat2skinmat + elemno) = setno;
				*(skinmat2bonemat + setno) = elemno;
				setno++;
			}
		}
		if( setno != skinmatnum ){
			DbgOut( "d3ddisp : CreateSkinMat : setno error %d, %d, %d!!!\n", setno, skinmatnum, hasjointflag );

//int dbgcnt;
//for( dbgcnt = 0; dbgcnt < leng; dbgcnt++ ){
//	DbgOut( "d3ddisp : CreateSkinMat : error dirty : %d, %d\r\n", dbgcnt, *(dirtyflag + dbgcnt) );
//}
			_ASSERT( 0 );
			free( dirtyflag );
			free( bonemat2skinmat );
			free( skinmat2bonemat );
			return 1;
		}

		if( m_blurmode == BLUR_NONE ){
			skinmat = new D3DXMATRIX[ skinmatnum ];
			if( !skinmat ){
				DbgOut( "d3ddisp : CreateSkinMat : skinmat alloc error !!!\n" );
				_ASSERT( 0 );
				free( dirtyflag );
				free( bonemat2skinmat );
				free( skinmat2bonemat );
				return 1;
			}
			int mno;
			for( mno = 0; mno < skinmatnum; mno++ ){
				D3DXMatrixIdentity( skinmat + mno );
			}

		}else{
			skinmat = new D3DXMATRIX[ MAXSKINMATRIX + 1 ];
			if( !skinmat ){
				DbgOut( "d3ddisp : CreateSkinMat : skinmat alloc error !!!\n" );
				_ASSERT( 0 );
				free( dirtyflag );
				free( bonemat2skinmat );
				free( skinmat2bonemat );
				return 1;
			}
			int mno;
			for( mno = 0; mno < (MAXSKINMATRIX + 1); mno++ ){
				D3DXMatrixIdentity( skinmat + mno );
			}
		}

		/////////////////
		mskinmat2bonemat = (int**)realloc( mskinmat2bonemat, sizeof( int* ) * (blockno + 1) );
		if( !mskinmat2bonemat ){
			DbgOut( "d3ddisp : CreateSkinMat : mskinmat2bonemat alloc error %d!!!\n", blockno );
			_ASSERT( 0 );
			return 1;
		}
		*(mskinmat2bonemat + blockno) = skinmat2bonemat;
	
		mbonemat2skinmat = (int**)realloc( mbonemat2skinmat, sizeof( int* ) * (blockno + 1) );
		if( !mbonemat2skinmat ){
			DbgOut( "d3ddisp : CreateSkinMat : mbonemat2skinmat alloc error %d !!!\n", blockno );
			_ASSERT( 0 );
			return 1;
		}
		*(mbonemat2skinmat + blockno) = bonemat2skinmat;

		mskinmat = (D3DXMATRIX**)realloc( mskinmat, sizeof( D3DXMATRIX*) * (blockno + 1) );
		if( !mskinmat ){
			DbgOut( "d3ddisp : CreateSkinMat : mskinmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		*(mskinmat + blockno) = skinmat;

		mskinmatnum = (int*)realloc( mskinmatnum, sizeof( int ) * (blockno + 1) );
		if( !mskinmatnum ){
			DbgOut( "d3ddisp : CreateSkinMat : mskinmatnum alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		*(mskinmatnum + blockno) = skinmatnum;

		blockno++;
	}

	mskinblocknum = blockno;
	free( dirtyflag );
	DbgOut( "d3ddisp : CreateSkinMat : %d\r\n", mskinblocknum );
/////////////
	int ret;
	ret = CreateBlurMat();
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMat : CreateBlurMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////////
	ret = Create3DBuffersSkin();
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMat : Create3DBuffersSkin error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "d3ddisp : CreateSkinMat : 1\n" );


	return 0;
}


int CD3DDisp::CreateMorphSkinMatTex( CShdHandler* lpsh, CPolyMesh2* pm2, TEXTUREBLOCK* texblock, int texblocknum )
{
	int ret;

	if( m_numTLV < 3 ){
		DbgOut( "d3ddisp : CreateSkinMatTex : tlvnum less than 3 ; skip\n" );
		return 0;//!!!!!!!!!
	}

	ret = CreateMorphSkinMat( lpsh, pm2 );
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMatTex : CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////
	int rblocknum = 0;
	int skinno, curskinstart, curskinend;
	TEXTUREBLOCK* curtexblock;
	RENDERBLOCK* currenderblock;
	int curstart;
	int texblockno = 0;


	curtexblock = texblock;

	for( skinno = 0; skinno < mskinblocknum; skinno++ ){


		curskinstart = (mskinblock + skinno)->startface;
		curskinend = (mskinblock + skinno)->endface;
		
		curstart = curskinstart;

		while( curskinend >= curtexblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatTex : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			//currenderblock->materialno = curtexblock->materialno;
			currenderblock->materialno = 0;
			currenderblock->startface = curstart;
			currenderblock->endface = curtexblock->endface;
			currenderblock->mqomat = curtexblock->mqomat;

			texblockno++;
			if( texblockno < texblocknum ){
				curtexblock = texblock + texblockno;
				curstart = curtexblock->startface;
			}else{
				break;
			}

		}
		if( curskinend < curtexblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatTex : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			//currenderblock->materialno = curtexblock->materialno;
			currenderblock->materialno = 0;
			currenderblock->startface = curstart;
			currenderblock->endface = curskinend;//!!!
			currenderblock->mqomat = curtexblock->mqomat;

		}

	}

	mrenderblocknum = rblocknum;

	/***
// for dbg
	int blno;
	for( blno = 0; blno < mrenderblocknum; blno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + blno;

		DbgOut( "check renderblock : no %d : materialno %d, skinno %d, texname %s, startface %d, endface %d\r\n",
			blno, currb->mqomat->materialno, currb->skinno, currb->mqomat->tex, currb->startface, currb->endface );
	}
	DbgOut( "\r\n" );
	***/

//	_ASSERT( 0 );

	return 0;
}
int CD3DDisp::CreateMorphSkinMatToon1( CShdHandler* lpsh, CPolyMesh2* pm2, MATERIALBLOCK* materialblock, int materialnum )
{
	int ret;

	if( m_numTLV < 3 ){
		DbgOut( "d3ddisp : CreateSkinMatToon1 : tlvnum less than 3 ; skip\n" );
		return 0;//!!!!!!!!!
	}

	ret = CreateMorphSkinMat( lpsh, pm2 );
	if( ret ){
		DbgOut( "d3ddisp : CreateSkinMatToon1 : CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////
	int rblocknum = 0;
	int skinno, curskinstart, curskinend;
	MATERIALBLOCK* curmaterialblock;
	RENDERBLOCK* currenderblock;
	int curstart;
	int materialno = 0;
	CMQOMaterial* curmqomat = 0;


	curmaterialblock = materialblock;

	for( skinno = 0; skinno < mskinblocknum; skinno++ ){


		curskinstart = (mskinblock + skinno)->startface;
		curskinend = (mskinblock + skinno)->endface;

		curstart = curskinstart;

		while( curskinend >= curmaterialblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatToon1 : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			currenderblock->materialno = curmaterialblock->materialno;
			currenderblock->startface = curstart;
			currenderblock->endface = curmaterialblock->endface;
			currenderblock->mqomat = curmaterialblock->mqomat;//!!!!!!!!!!!!
			materialno++;
			if( materialno < materialnum ){
				curmaterialblock = materialblock + materialno;
				curstart = curmaterialblock->startface;
			}else{
				break;
			}

		}
		if( curskinend < curmaterialblock->endface ){
			rblocknum++;
			mrenderblock = (RENDERBLOCK*)realloc( mrenderblock, sizeof( RENDERBLOCK ) * rblocknum );
			if( !mrenderblock ){
				DbgOut( "d3ddisp : CreateSkinMatToon1 : renderblock alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			currenderblock = mrenderblock + rblocknum - 1;
			currenderblock->skinno = skinno;
			currenderblock->materialno = curmaterialblock->materialno;
			currenderblock->startface = curstart;
			currenderblock->endface = curskinend;//!!!
			currenderblock->mqomat = curmaterialblock->mqomat;
		}

	}

	mrenderblocknum = rblocknum;

	/***
// for dbg
	int blno;
	for( blno = 0; blno < mrenderblocknum; blno++ ){
		RENDERBLOCK* currb;
		currb = mrenderblock + blno;

		DbgOut( "check renderblock : no %d : materialno %d, skinno %d, startface %d, endface %d\r\n",
			blno, currb->materialno, currb->skinno, currb->startface, currb->endface );
	}
	DbgOut( "\r\n" );
	***/

	return 0;
}

int CD3DDisp::Copy2VertexBufferMorph( CPolyMesh2* pm2, int cpmaterial )
{
	if( !m_skinv && !m_lv ){
		DbgOut( "d3ddisp : Copy2VertexBuffer : skinv not exist skip !!!\n" );
		return 0;
	}

	if( m_linekind == 0 ){

		int blno;
		for( blno = 0; blno < mskinblocknum; blno++ ){

			LPDIRECT3DVERTEXBUFFER9 curvb;
			curvb = *(m_ArrayVB + blno);
			SKINVERTEX* pVertices;
			if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
				_ASSERT( 0 );
				return 1;
			}

			int* curbone2skin = *(mbonemat2skinmat + blno);
			int* curskin2bone = *(mskinmat2bonemat + blno);
			SKINBLOCK* curskinblock = mskinblock + blno;

			int faceno;
			int index[3];
			int optindex[3];
			for( faceno = curskinblock->startface; faceno < curskinblock->endface; faceno++ ){
				index[0] = *(m_dispIndices + faceno * 3);
				index[1] = *(m_dispIndices + faceno * 3 + 1);
				index[2] = *(m_dispIndices + faceno * 3 + 2);
				optindex[0] = *(pm2->oldpno2optpno + index[0] );
				optindex[1] = *(pm2->oldpno2optpno + index[1] );
				optindex[2] = *(pm2->oldpno2optpno + index[2] );

				int ino;
				for( ino = 0; ino < 3; ino++ ){
					SKINVERTEX* curskinv = m_skinv + index[ino];
					SKINVERTEX* dstv = pVertices + index[ino];

					*dstv = *curskinv;

					CInfElem* curie = m_IEptr + optindex[ino];
					int ieno;
					int setno = 0;
					for( ieno = 0; ieno < curie->infnum; ieno++ ){
						INFELEM* curIE = curie->ie + ieno;
							
						int setskinmatno;
						setskinmatno = *(curbone2skin + curIE->bonematno);
						if( setskinmatno > 0 ){
							dstv->boneindex[setno] = (float)setskinmatno;
							dstv->weight[setno] = curIE->dispinf;
							setno++;
						}
					}
						
				}

			}
			curvb->Unlock();
		}


////////////////////
		if( m_revskinv && m_revArrayVB ){

			int blno;
			for( blno = 0; blno < mskinblocknum; blno++ ){

				LPDIRECT3DVERTEXBUFFER9 curvb;
				curvb = *(m_revArrayVB + blno);
				SKINVERTEX* pVertices;
				if( FAILED( curvb->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
					_ASSERT( 0 );
					return 1;
				}

				int* curbone2skin = *(mbonemat2skinmat + blno);
				int* curskin2bone = *(mskinmat2bonemat + blno);
				SKINBLOCK* curskinblock = mskinblock + blno;

				int faceno;
				int index[3];
				int optindex[3];
				for( faceno = curskinblock->startface; faceno < curskinblock->endface; faceno++ ){
					index[0] = *(m_dispIndices + faceno * 3);
					index[1] = *(m_dispIndices + faceno * 3 + 2);// 裏面なので、index反転
					index[2] = *(m_dispIndices + faceno * 3 + 1);
					optindex[0] = *(pm2->oldpno2optpno + index[0] );
					optindex[1] = *(pm2->oldpno2optpno + index[1] );
					optindex[2] = *(pm2->oldpno2optpno + index[2] );


					int ino;
					for( ino = 0; ino < 3; ino++ ){
						SKINVERTEX* curskinv = m_revskinv + index[ino];
						SKINVERTEX* dstv = pVertices + index[ino];

						*dstv = *curskinv;

						CInfElem* curie = m_IEptr + optindex[ino];
						int ieno;
						int setno = 0;
						for( ieno = 0; ieno < curie->infnum; ieno++ ){
							INFELEM* curIE = curie->ie + ieno;
								
							int setskinmatno;
							setskinmatno = *(curbone2skin + curIE->bonematno);
							if( setskinmatno > 0 ){
								dstv->boneindex[setno] = (float)setskinmatno;
								dstv->weight[setno] = curIE->dispinf;
								setno++;
							}
						}
							
					}

				}
				curvb->Unlock();
			}
		}


		if( m_smaterial && m_VBmaterial && (cpmaterial != 0) ){
			SKINMATERIAL* pv;
			if( FAILED( m_VBmaterial->Lock( 0, sizeof( SKINMATERIAL ) * m_numTLV, (void**)&pv, 0 ) ) ){
				DbgOut( "d3ddisp : m_VBmaterial lock error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			MoveMemory( pv, m_smaterial, m_numTLV * sizeof( SKINMATERIAL ) );
			m_VBmaterial->Unlock();
		}


	}else{
		D3DLVERTEX* pVertices;
		if( FAILED( m_VB->Lock( 0, 0, (void**)&pVertices, NULL ) ) ){
			DbgOut( "d3ddisp : m_VB lock 10  : error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		MoveMemory( pVertices, m_lv, m_numTLV * sizeof( D3DLVERTEX ) );
					
		m_VB->Unlock();
	}

	if( m_skintan && m_VBtan ){
		SKINTANGENT* pv;
		if( FAILED( m_VBtan->Lock( 0, sizeof( SKINTANGENT ) * m_numTLV, (void**)&pv, 0 ) ) ){
			DbgOut( "d3ddisp : m_VBtan lock error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( pv, m_skintan, m_numTLV * sizeof( SKINTANGENT ) );
		m_VBtan->Unlock();
	}

	if( m_revskintan && m_revVBtan ){
		SKINTANGENT* pv;
		if( FAILED( m_revVBtan->Lock( 0, sizeof( SKINTANGENT ) * m_numTLV, (void**)&pv, NULL ) ) ){
			DbgOut( "d3ddisp : m_VBtan lock error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( pv, m_revskintan, m_numTLV * sizeof( SKINTANGENT ) );
		m_revVBtan->Unlock();
	}

	return 0;
}

int CD3DDisp::InitBeforeBlur()
{
	m_blurpos = 0;

	int blno;
	int blurcnt;
	for( blurcnt = 0; blurcnt < m_blurtime; blurcnt++ ){

		//現在の姿勢をリングバッファ全てに保存
		for( blno = 0; blno < mskinblocknum; blno++ ){
			int curskinnum = *(mskinmatnum + blno);
			D3DXMATRIX* curskinmat = *(mskinmat + blno);
			
			D3DXMATRIX* savemat = *( m_blurmat + blno );
			int skno;
			for( skno = 0; skno < curskinnum; skno++ ){
				*( savemat + ( blurcnt * curskinnum ) + skno ) = *( curskinmat + skno );
			}
		}

		*( m_blurViewMat + blurcnt ) = m_matView;
	}


	//リングバッファをブラー用のskinmatに格納		
	int pickpos = 0;

	for( blno = 0; blno < mskinblocknum; blno++ ){
		int curskinnum = *(mskinmatnum + blno);
		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		D3DXMATRIX* bmat = *( m_blurmat + blno );

		if( (curskinnum + MAXBLURMATRIX) >= MAXSKINMATRIX ){
			_ASSERT( 0 );
			return 1;
		}

		int skno;
		for( skno = 0; skno < curskinnum; skno++ ){
			*( curskinmat + skno + MAXBLURMATRIX + 1 ) = *( bmat + ( pickpos * curskinnum ) + skno );
		}
	}

	m_oldView = *( m_blurViewMat + pickpos );

	return 0;
}

int CD3DDisp::SetBeforeBlur( int bbflag )
{

//現在の姿勢をリングバッファに保存
	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){
		int curskinnum = *(mskinmatnum + blno);
		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		
		D3DXMATRIX* savemat = *( m_blurmat + blno );
		int skno;
		for( skno = 0; skno < curskinnum; skno++ ){
			*( savemat + ( m_blurpos * curskinnum ) + skno ) = *( curskinmat + skno );
		}
	}

	*( m_blurViewMat + m_blurpos ) = m_matView;

//m_blurtime - 1 回だけ前のリングバッファをブラー用のskinmatに格納
	int pickpos;
	pickpos = m_blurpos - ( m_blurtime - 1 );
	if( pickpos < 0 ){
		pickpos += m_blurtime;
	}
	_ASSERT( (pickpos >= 0) && (pickpos < m_blurtime) );
	
	for( blno = 0; blno < mskinblocknum; blno++ ){
		int curskinnum = *(mskinmatnum + blno);
		D3DXMATRIX* curskinmat = *(mskinmat + blno);
		D3DXMATRIX* bmat = *( m_blurmat + blno );

		if( (curskinnum + MAXBLURMATRIX) >= MAXSKINMATRIX ){
			_ASSERT( 0 );
			return 1;
		}

		int skno;
		for( skno = 0; skno < curskinnum; skno++ ){
			*( curskinmat + skno + MAXBLURMATRIX + 1 ) = *( bmat + ( pickpos * curskinnum ) + skno );
		}
	}

	m_oldView = *( m_blurViewMat + pickpos );

//リングバッファの保存場所の更新
	m_blurpos++;
	if( m_blurpos >= m_blurtime ){
		m_blurpos = 0;
	}

	return 0;
}


/***
int CD3DDisp::SetBeforeBlur( int bbflag )
{
	m_oldView = m_matView;

	if( bbflag == 0 ){
		int blno;
		for( blno = 0; blno < mskinblocknum; blno++ ){
			int curskinnum = *(mskinmatnum + blno);
			D3DXMATRIX* curskinmat = *(mskinmat + blno);

			if( (curskinnum + MAXBLURMATRIX) >= MAXSKINMATRIX ){
				_ASSERT( 0 );
				return 1;
			}

			*( curskinmat + 0 + MAXBLURMATRIX + 1 ) = *( curskinmat + 0 );

			int skno;
			for( skno = 1; skno < curskinnum; skno++ ){
				*( curskinmat + skno + MAXBLURMATRIX + 1 ) = *( curskinmat + skno );
			}
		}
	}else{
		//billboard
		int skno;
		int curskinnum = *mskinmatnum;
		D3DXMATRIX* curskinmat = *mskinmat;

		if( (curskinnum + MAXBLURMATRIX) >= MAXSKINMATRIX ){
			_ASSERT( 0 );
			return 1;
		}

		*( curskinmat + 0 + MAXBLURMATRIX + 1 ) = *( curskinmat + 0 );

		for( skno = 1; skno < curskinnum; skno++ ){
			*( curskinmat + skno + MAXBLURMATRIX + 1 ) = *( curskinmat + skno );
		}
	}

	return 0;
}
***/
int CD3DDisp::CreateBlurMat()
{
	if( m_blurmat ){
		_ASSERT( 0 );
		return 1;
	}

	if( m_blurtime <= 0 ){
		return 0;
	}

	m_blurmat = (D3DXMATRIX**)malloc( sizeof( D3DXMATRIX* ) * mskinblocknum );
	if( !m_blurmat ){
		DbgOut( "d3ddisp : CreateBlurMat : blurmat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( m_blurmat, sizeof( D3DXMATRIX* ) * mskinblocknum );

	int blno;
	for( blno = 0; blno < mskinblocknum; blno++ ){
		int matnum;
		matnum = *( mskinmatnum + blno );

		D3DXMATRIX* bmat;
		int matlen;
		matlen = matnum * m_blurtime;
		bmat = new D3DXMATRIX[ matlen ];
		if( !bmat ){
			DbgOut( "d3ddisp : CreateBlurMat : bmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int matno;
		for( matno = 0; matno < matlen; matno++ ){
			D3DXMatrixIdentity( bmat + matno );
		}

		*( m_blurmat + blno ) = bmat;
	}

	m_blurViewMat = new D3DXMATRIX[ m_blurtime ];
	if( !m_blurViewMat ){
		DbgOut( "d3ddisp : CreateBlurMat : blurViewMat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int mno;
	for( mno = 0; mno < m_blurtime; mno++ ){
		D3DXMatrixIdentity( m_blurViewMat + mno );
	}

	return 0;
}

