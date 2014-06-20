#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <D3DX9.h>

#include <shdelem.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <coef.h>

#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <d3ddisp.h>

#include <texbank.h>

#include <billboardelem.h>
#include <mqomaterial.h>
#include <extline.h>
#include <InfScope.h>

#include <MCache.h>
#include <pickdata.h>
#include <BoneInfo.h>

#include <Toon1Params.h>

#include <BBox2.h>
#include <DispSwitch.h>

#include <MMotElem.h>

#include <rtsc.h>

////////
/***
HANDLE	CShdElem::s_hHeap = NULL;
unsigned int	CShdElem::s_uNumAllocsInHeap = 0;

unsigned int	CShdElem::s_DataNo = 0;
void*	CShdElem::s_DataHead = 0;

int		CShdElem::s_HeadNum = 0;
BYTE*	CShdElem::s_HeadIndex[2048];

int	CShdElem::s_useflag[2048][SE_BLOCKLENG];
***/

/// extern 
extern DWORD g_renderstate[ D3DRS_BLENDOP + 1 ];
extern int g_rsindex[ 80 ];
extern DWORD g_minfilter;
extern DWORD g_magfilter;
extern RTSC g_rtsc;
extern int g_useGPU;

extern int g_fogenable;
extern D3DCOLOR g_fogcolor;
extern float g_fogstart;
extern float g_fogend;	
extern float g_fogtype;

extern LPDIRECT3DTEXTURE9 g_curtex0;
extern LPDIRECT3DTEXTURE9 g_curtex1;

extern DWORD g_cop0;
extern DWORD g_cop1;
extern DWORD g_aop0;
extern DWORD g_aop1;

extern CTexBank* g_texbnk;
extern int MAXBLURMATRIX;

/***
void	*CShdElem::operator new ( size_t size )
{

	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
			
		if( s_hHeap == NULL )
			return NULL;
	}

	void	*p;
	p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size );
			
	if( p != NULL ){
		s_uNumAllocsInHeap++;
	}

	return (p);
}

void	CShdElem::operator delete ( void *p )
{
		
	if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
		s_uNumAllocsInHeap--;
	}

	if( s_uNumAllocsInHeap == 0 ){
		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
	}
}
***/

/***
void	*CShdElem::operator new ( size_t size )
{

	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
		if( s_hHeap == NULL )
			return NULL;
		ZeroMemory( s_HeadIndex, sizeof( BYTE* ) * 2048 );
		ZeroMemory( s_useflag, sizeof( int ) * 2048 * SE_BLOCKLENG );
	}


	if( !s_DataHead || (s_DataNo >= SE_BLOCKLENG) ){
		if( s_HeadNum >= 2048 ){
			_ASSERT( 0 );
			DbgOut( "CShdElem : operator new : s_HeadNum too large error !!!\n" );
			return NULL;
		}

		s_DataHead = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size * SE_BLOCKLENG );
		if( !s_DataHead ){
			_ASSERT( 0 );
			return NULL;
		}
		s_DataNo = 0;

		s_HeadIndex[s_HeadNum] = (BYTE*)s_DataHead;
		s_HeadNum++;
	}

	void* p;
	p = (void*)((BYTE*)s_DataHead + size * s_DataNo);
	if( p != NULL ){
		s_uNumAllocsInHeap++;
		s_DataNo++;
		s_useflag[s_HeadNum - 1][s_DataNo - 1] = 1;
	}

	return (p);
}

void	CShdElem::operator delete ( void *p )
{
		
	s_uNumAllocsInHeap--;

	int headno;
	void* delhead = 0;
	int delno = -1;
	//pより小さい中で、一番大きいheadindexを探す。
	for( headno = 0; headno < s_HeadNum; headno++ ){
		void* headptr = (void*)(s_HeadIndex[headno]);
		if( ((unsigned long)p >= (unsigned long)headptr) 
			&& ((unsigned long)delhead < (unsigned long)headptr) ){
			delhead = headptr;
			delno = headno;
		}
	}
	if( delno < 0 ){
		_ASSERT( 0 );
		DbgOut( "ShdElem : operator delete invalid address error !!!\n" );
		return;
	}
	int datano;
	datano = ((unsigned long)p - (unsigned long)delhead) / sizeof( CShdElem );
	_ASSERT( (datano >= 0) && (datano < SE_BLOCKLENG) ); 
	s_useflag[delno][datano] = 0;
	
	int usecnt = 0;
	int chkno;
	for( chkno = 0; chkno < SE_BLOCKLENG; chkno++ ){
		if( s_useflag[delno][chkno] == 1 )
			usecnt++;
	}
	if( usecnt == 0 ){
		HeapFree( s_hHeap, HEAP_NO_SERIALIZE, delhead );
		s_HeadIndex[delno] = 0;

		DbgOut( "ShdElem : operator delete : HeapFree %d\n", delno );

		if( delno == (s_HeadNum - 1) ){
			//最後のHeadIndexを消す場合の処理。
			s_DataHead = 0;
			DbgOut( "ShdElem : operator delete : last heap\n" );
		}
	}


	//if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
	//	s_uNumAllocsInHeap--;
	//}

	if( s_uNumAllocsInHeap == 0 ){		
		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
		DbgOut( "ShdElem : operator delete : HeapDestroy : %d\n", s_HeadNum );
	}
}
***/


CShdElem::CShdElem( int srcserino )
{
	int ret;

	InitObjFunc();

	InitParams( srcserino );

	ret = CreateObjs();
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree ::CShdTree : CreateObjs error  !!!\n" );
		isinit = 0;
	}

}
CShdElem::~CShdElem()
{
	DestroyObjs();
}

int CShdElem::InitParams( int srcserino )
{
	CBaseDat::InitParams();

	setalphaflag = 0;

	serialno = srcserino;
	brono = -1;
	bronum = 0;
	depth = 0;
	type = SHDTYPENONE;

	// chain は、CTreeElem2から取得してセット。
	parent = 0;
	child = 0;
	brother = 0;
	sister = 0;

	clockwise = 0;
	invface = 0;
	invfacecnt = 0;

	bdivU = 0;
	bdivV = 0;

	uanime = 0.0f;
	vanime = 0.0f;

	alpha = 1.0f;

	texname = 0;
	curtexname = 0;

	texrepx = 0;
	texrepy = 0;

	transparent = 0;

	texrule = 0;
	noskinning = 0;

	blendmode = 0;

	dispflag = 0;
	isselected = 0;

	notuse = 0;

	influencebone = 0;
	influencebonenum = 0;

	ignorebone = 0;
	ignorebonenum = 0;

	dispswitchno = 0;

	invisibleflag = 0;

	exttexnum = 0;
	exttexmode = EXTTEXMODE_NORMAL; //重ねるか、パラパラアニメするかどうか。
	exttexrep = EXTTEXREP_ROUND; //繰り返しかどうか
	exttexstep = 1; //何フレームごとに、変化するか。
	exttexname = 0;


	// Init3DObj( meshinfo )で作成
		// １つのTreeに１種、１個だけ。
	vline = 0; // SHDPOLYLINE
	polygon = 0; // SHDPOLYGON
	sphere = 0; // SHDSPHERE
	bmesh = 0; // SHDBEZIERSURF
	revolved = 0; // SHDMESHES
	disk = 0; // SHDDISK
	bline = 0;
	morph = 0;
	part = 0;

	extrude = 0;// SHDEXTRUDE

	polymesh = 0;//SHDPOLYMESH
	polymesh2 = 0;

	grounddata = 0;
	billboard = 0;

	extline = 0;

	bdiv_bmesh = 0;
	bdiv_bmesh2 = 0;
		// for disp
	d3ddisp = 0;

	neededconvert = 0;
	neededconvert_ig = 0;

	firstbs = 0;
	curbs.InitParams();
	m_bbox.InitParams();
	m_bbx0.InitParams();
	m_bbx1.InitParams();

	m_loadbimflag = 0;

	m_enabletexture = 1;

	ZeroMemory( m_renderstate, sizeof( int ) * (D3DRS_BLENDOP + 1) );

	SetDefaultRenderState();

	m_tempscalemat = 0;

	m_notransik = 0;
	m_ikskip = 0;

	m_userint1 = 0;

	m_iklevel = 5;
	m_bonelinedisp = 1;

	m_bbdirmode = 0;

	m_mqono = 0;

	m_anchorflag = 0;

	m_mikodef = MIKODEF_NONE;

	scopenum = 0;
	ppscope = 0;

	bboxnum = 0;
	ppbbox = 0;

	m_iktarget = 0;
	m_iktargetlevel = 5;
	m_iktargetcnt = 1;
	m_iktargetpos.x = 0.0f;
	m_iktargetpos.y = 0.0f;
	m_iktargetpos.z = 0.0f;


	facet = 180.0f;

	m_toonparams[0] = 0.0f;
	m_toonparams[1] = 0.9f;
	m_toonparams[2] = 0.4f;
	m_toonparams[3] = 0.9f;

	D3DXMatrixIdentity( &TransformationMatrix );
	D3DXMatrixIdentity( &combinedtra );

	m_lightflag = 1;
	m_particleflag = 0;

	m_tempfogenable = 0;

	m_shadowinflag = SHADOWIN_PROJ;

	m_notsel = 0;

	m_mikobonepos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_setmikobonepos = 0;

	m_mtype = M_NONE;
	m_blurbefrender = 0;
	m_mbluralpha[0] = 0.2f;
	m_mbluralpha[1] = 1.0f;

	m_bbexttexmode = 0;
	m_bbglowmult[0] = 1.0f;
	m_bbglowmult[1] = 1.0f;
	m_bbglowmult[2] = 1.0f;

	m_befdispswitch = 0;

	return 0;
}

int	CShdElem::Init3DObj( CMeshInfo* srcmeshinfo )
{
// 初期化用データ
	int ret;

	if( !srcmeshinfo ){
		DbgOut( "shddata.cpp : CShdTree : Init3DObj : srcmeshinfo NULL !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	DestroyBSphereData();


	type = srcmeshinfo->type;
	ret = (this->*DestroyObjFunc[ type ])();
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : Init3DObj : Destroy3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	DbgOut( "shddata.cpp : Destroy3DObj\n" );

	ret = (this->*CreateObjFunc[ type ])( srcmeshinfo );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : Create3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DbgOut( "shddata.cpp : Create3DObj\n" );

	return 0;
		
}

int CShdElem::InvalidateDispObj()
{
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	CInfScope* curis;
	int isno;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		curis->InvalidateDispObj();
	}

	CBBox2* curbbx;
	int bbxno;
	for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
		curbbx = *( ppbbox + bbxno );
		curbbx->InvalidateDispObj();
	}

	return 0;
}
/***
int CShdElem::SetDirectionalLight( D3DXVECTOR3 dir )
{
	if( d3ddisp ){
		d3ddisp->m_orglightvec = dir;
	}
	return 0;
}
***/
int CShdElem::ResetTotalPrim()
{
	if( d3ddisp ){
		d3ddisp->ResetPrimNum();
	}

	CInfScope* curis;
	int isno;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		curis->ResetPrimNum();
	}


	CBBox2* curbbx;
	int bbxno;
	for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
		curbbx = *( ppbbox + bbxno );
		curbbx->ResetPrimNum();
	}


	return 0;
}

int CShdElem::CreateAfterImage( LPDIRECT3DDEVICE9 pd3dDevice, int imagenum )
{
	if( !d3ddisp )
		return 0;
	if( IsJoint() )
		return 0;

	//int ret = 0;
	//ret = d3ddisp->CreateAfterImage( pd3dDevice, imagenum );
	//if( ret ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	return 0;
}
int CShdElem::DestroyAfterImage()
{
	if( !d3ddisp )
		return 0;
	if( IsJoint() )
		return 0;

	//int ret = 0;
	//ret = d3ddisp->DestroyAfterImage();
	//if( ret ){
	//	_ASSERT( 0 );
//		return 1;
//	}
	return 0;
}

int CShdElem::CreateDispObj( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{

	int ret;
	ret = CheckDispData();
	if( ret ){
		DbgOut( "CShdTree : warning !!! DispObj exist !!!\n" );
		return 0;
	}

	// 3dobjのないtree は　ret == -1, create失敗は　ret == 1
	ret = (this->*CreateDObjFunc[ type ])( seri2boneno, pd3dDevice, lpmh, lpsh, tlmode, needcalcbe );
	_ASSERT( ret <= 0 ); // なんか逆なんだけど、あいかわらず　ねがてぃぶ
	return ret;
	
}
int CShdElem::CheckDispData()
{
	int ret = 0;

	if( d3ddisp )
		return 1;
	else
		return 0;
}
int CShdElem::SetUV( CMQOMaterial* mathead )
{
	if( !d3ddisp ){
		DbgOut( "CShdElem : SetUV : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	if( polymesh && polymesh->billboardflag ){
		ret = d3ddisp->SetUV( type, texrule, polymesh2, polymesh, -1 );
		if( ret ){
			DbgOut( "CShdElem : SetUV : d3ddisp->SetUV error !!!\n" );
			return 1;
		}
	}else{
		if( type == SHDPOLYMESH ){
			int dmno;
			for( dmno = 0; dmno < polymesh->m_texblocknum; dmno++ ){
				DIRTYMAT* curdm;
				curdm = polymesh->m_dirtymat + dmno;

				int matno;
				matno = curdm->materialno;
				CMQOMaterial* curmat;
				curmat = polymesh->GetMaterialFromNo( mathead, matno );
				if( !curmat ){
					DbgOut( "se : SetUV : pm GetMaterialFromNo NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = d3ddisp->SetUVArray( curdm->dirtyflag, type, curmat->texrule, polymesh2, polymesh );
				if( ret ){
					DbgOut( "se : SetUV : d3ddisp SetUVArray error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		}else if( type == SHDPOLYMESH2 ){
			int dmno;
			for( dmno = 0; dmno < polymesh2->m_texblocknum; dmno++ ){
				DIRTYMAT* curdm;
				curdm = polymesh2->m_dirtymat + dmno;

				int matno;
				matno = curdm->materialno;
				CMQOMaterial* curmat;
				curmat = polymesh2->GetMaterialFromNo( mathead, matno );
				if( !curmat ){
					DbgOut( "se : SetUV : pm2 GetMaterialFromNo NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = d3ddisp->SetUVArray( curdm->dirtyflag, type, curmat->texrule, polymesh2, polymesh );
				if( ret ){
					DbgOut( "se : SetUV : d3ddisp SetUVArray error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		}
	}
	return 0;

}

int CShdElem::CalcInfElem( CShdHandler* srclpsh )
{
	int ret;

	if( d3ddisp ){
		if( !morph ){
			ret = d3ddisp->CalcInfElem( srclpsh, serialno );
			if( ret ){
				DbgOut( "CShdElem : CalcInfElem : d3ddisp->CalcInfElem error !!!\n" );
				return 1;
			}
		}//morphはbaseのIEを使うので計算しない。


		if( !morph ){
			if( srclpsh->m_shader != COL_TOON1 ){
				if( polymesh ){
					if( polymesh->billboardflag == 0 ){
						ret = d3ddisp->CreateSkinMatTex( srclpsh, polymesh->m_texblock, polymesh->m_texblocknum );
						if( ret ){
							DbgOut( "CShdElem : CalcInfElem : d3ddisp CreateSkinMatTex error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						ret = d3ddisp->CreateSkinMat( srclpsh );
						if( ret ){
							DbgOut( "ShdElem : CalcInfElem : d3ddisp CreateSkinMat error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
				if( polymesh2 ){
					ret = d3ddisp->CreateSkinMatTex( srclpsh, polymesh2->m_texblock, polymesh2->m_texblocknum );
					if( ret ){
						DbgOut( "CShdElem : CalcInfElem : d3ddisp CreateSkinMatTex error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

			}else{
				if( polymesh && polymesh->m_materialblock ){
					ret = d3ddisp->CreateSkinMatToon1( srclpsh, polymesh->m_materialblock, polymesh->m_materialnum );
					if( ret ){
						DbgOut( "shdelem : CalcInfElem : d3ddisp CreateSkinMatToon1 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				if( polymesh2 && polymesh2->m_materialblock ){
					ret = d3ddisp->CreateSkinMatToon1( srclpsh, polymesh2->m_materialblock, polymesh2->m_materialnum );
					if( ret ){
						DbgOut( "shdelem : CalcInfElem : d3ddisp CreateSkinMatToon1 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}else{
			if( srclpsh->m_shader != COL_TOON1 ){
				if( morph->m_objtype == SHDPOLYMESH ){
					ret = d3ddisp->CreateSkinMatTex( srclpsh, 
						morph->m_baseelem->polymesh->m_texblock, morph->m_baseelem->polymesh->m_texblocknum );
					if( ret ){
						DbgOut( "CShdElem : CalcInfElem : d3ddisp CreateSkinMatTex error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				if( morph->m_objtype == SHDPOLYMESH2 ){
					ret = d3ddisp->CreateMorphSkinMatTex( srclpsh, morph->m_baseelem->polymesh2,
						morph->m_baseelem->polymesh2->m_texblock, morph->m_baseelem->polymesh2->m_texblocknum );
					if( ret ){
						DbgOut( "CShdElem : CalcInfElem : d3ddisp CreateSkinMatTex error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

			}else{
				if( (morph->m_objtype == SHDPOLYMESH) && morph->m_baseelem->polymesh->m_materialblock ){
					ret = d3ddisp->CreateSkinMatToon1( srclpsh, 
						morph->m_baseelem->polymesh->m_materialblock, morph->m_baseelem->polymesh->m_materialnum );
					if( ret ){
						DbgOut( "shdelem : CalcInfElem : d3ddisp CreateSkinMatToon1 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				if( (morph->m_objtype == SHDPOLYMESH2) && morph->m_baseelem->polymesh2->m_materialblock ){
					ret = d3ddisp->CreateMorphSkinMatToon1( srclpsh, morph->m_baseelem->polymesh2,
						morph->m_baseelem->polymesh2->m_materialblock, morph->m_baseelem->polymesh2->m_materialnum );
					if( ret ){
						DbgOut( "shdelem : CalcInfElem : d3ddisp CreateSkinMatToon1 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}

		ret = UpdateVertexBuffer( 0, TLMODE_ORG, 0 );
		if( ret ){
			DbgOut( "selem : CalcInfElem : UpdateVertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDINFSCOPE ){
		CInfScope* curis;
		int isno;
		for( isno = 0; isno < scopenum; isno++ ){
			curis = *( ppscope + isno );
			_ASSERT( curis );
			
			ret = curis->CalcInitialInfElem();
			if( ret ){
				DbgOut( "shdelem : CalcInfElem : IS CreateInitialInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( g_useGPU != 0 ){

				ret = curis->d3ddisp->CreateSkinMat( srclpsh );
				if( ret ){
					DbgOut( "selem : CalcInfElem : IS : d3ddisp CreateSkinMat error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = curis->d3ddisp->Copy2VertexBuffer( 0 );
				if( ret ){
					DbgOut( "selem : CalcInfElem : IS : d3ddisp UpdateVertexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = curis->d3ddisp->Copy2IndexBufferNotCulling();
				if( ret ){
					DbgOut( "shdelem : CalcInfElem : curis->d3ddisp  Copy2IndexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


			}
		}
	}else if( type == SHDBBOX ){
		CBBox2* curbbx;
		int bbxno;
		for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
			curbbx = *( ppbbox + bbxno );
			_ASSERT( curbbx );

			ret = curbbx->CalcInitialInfElem();
			if( ret ){
				DbgOut( "shdelem : CalcInfElem : bbx CalcInitialInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( g_useGPU != 0 ){
				ret = curbbx->d3ddisp->CreateSkinMat( srclpsh );
				if( ret ){
					DbgOut( "selem : CalcInfElem : bbx d3ddisp CreateSkinMat error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				//if( curbbx->polymesh2 ){
				//	ret = curbbx->d3ddisp->CreateSkinMatTex( srclpsh, curbbx->polymesh2->m_texblock, curbbx->polymesh2->m_texblocknum );
				//	if( ret ){
				//		DbgOut( "CShdElem : CalcInfElem : bbx : d3ddisp CreateSkinMatTex error !!!\n" );
				//		_ASSERT( 0 );
				//		return 1;
				//	}
				//}

				ret = curbbx->d3ddisp->Copy2VertexBuffer( 0 );
				if( ret ){
					DbgOut( "selem : CalcInfElem : bbx d3ddisp UpdateVertexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = curbbx->d3ddisp->Copy2IndexBufferNotCulling();
				_ASSERT( !ret );

			}
		}
	}
	return 0;
}
int CShdElem::CalcInfElem1Vert( CShdHandler* srclpsh, int srcvert )
{
	if( !d3ddisp ){
		DbgOut( "se : CalcInfElem1Vert : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcvert < 0) || (srcvert >= (int)d3ddisp->m_numTLV) ){
		DbgOut( "selem : CalcInfElem1Vert : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->CalcInfElem1Vert( srclpsh, serialno, srcvert );
	if( ret ){
		DbgOut( "selem : CalcInfElem1Vert : d3ddisp CalcInfElem1Vert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( srclpsh->m_shader != COL_TOON1 ){
		if( polymesh2 ){
			ret = d3ddisp->CreateSkinMatTex( srclpsh, polymesh2->m_texblock, polymesh2->m_texblocknum );
			if( ret ){
				DbgOut( "CShdElem : CalcInfElem1Vert : d3ddisp CreateSkinMatTex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( polymesh ){
			if( polymesh->billboardflag == 0 ){
				ret = d3ddisp->CreateSkinMatTex( srclpsh, polymesh->m_texblock, polymesh->m_texblocknum );
				if( ret ){
					DbgOut( "CShdElem : CalcInfElem1Vert : d3ddisp CreateSkinMatTex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				ret = d3ddisp->CreateSkinMat( srclpsh );
				if( ret ){
					DbgOut( "shdelem : CalcInfElem1Vert : d3ddisp CreateSkinMat error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}else{
		if( polymesh2 ){
			ret = d3ddisp->CreateSkinMatToon1( srclpsh, polymesh2->m_materialblock, polymesh2->m_materialnum );
			if( ret ){
				DbgOut( "shdelem : CalcInfElem1Vert : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( polymesh ){
			ret = d3ddisp->CreateSkinMatToon1( srclpsh, polymesh->m_materialblock, polymesh->m_materialnum );
			if( ret ){
				DbgOut( "shdelem : CalcInfElem1Vert : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdElem::CopyChain( CShdHandler* shdhandler, CTreeElem2* curte )
{
	int parno, chilno, brono, sisno;
	//CTreeLevel* curchild = 0;
	
	CTreeElem2* parte = curte->parent;
	if( parte )
		parno = parte->serialno;
	else
		parno = -1;

	CTreeElem2* chilte = curte->child;
	if( chilte ){
		chilno = chilte->serialno;
	}else{
		chilno = -1;
	}

	CTreeElem2* brote = curte->brother;
	if( brote )
		brono = brote->serialno;
	else
		brono = -1;

	CTreeElem2* siste = curte->sister;
	if( siste )
		sisno = siste->serialno;
	else
		sisno = -1;

	////
	if( parno > 0 )
		parent = (*shdhandler)( parno );
	else
		parent = 0;

	if( chilno > 0 )
		child = (*shdhandler)( chilno );
	else
		child = 0;

	if( brono > 0 )
		brother = (*shdhandler)( brono );
	else
		brother = 0;

	if( sisno > 0 )
		sister = (*shdhandler)( sisno );
	else
		sister = 0;


	return 0;
}

		// 3dobj の　set
int CShdElem::SetChild( CShdElem* chilptr )
{
	if( !chilptr )
		return 1;

	child = chilptr;
	chilptr->parent = this;
	return 0;
}
int CShdElem::SetBrother( CShdElem* broptr )
{
	if( !broptr )
		return 1;

	brother = broptr;
	broptr->sister = this;
	return 0;
}

int CShdElem::SetDepth( int srcdepth )
{
	depth = srcdepth;
	return 0;

}

int CShdElem::SetClockwise( int cw )
{
	clockwise = cw;
	return 0;
}

int CShdElem::ClearInvFaceCnt()
{
	invfacecnt = 0;
	return 0;
}
void CShdElem::SetClockwiseReq( int addcnt )
{
	int newaddcnt = 0;

	newaddcnt = addcnt + invface;
	invfacecnt = newaddcnt;
	CalcClockwise();
	
	if( child ){
		child->SetClockwiseReq( newaddcnt );
	}

	if( brother ){
		brother->SetClockwiseReq( addcnt );
	}
}

int CShdElem::SetBDivUV( int srcdivU, int srcdivV )
{
	bdivU = srcdivU;
	bdivV = srcdivV;
	return 0;
}

int CShdElem::SetTexName( char* srctexname )
{
	int leng;

	if( !srctexname )
		return 1;

	leng = (int)strlen( srctexname );
	if( leng >= 256 ){
		DbgOut( "selem : SetTexName : texname too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//texname = (char*)malloc( leng + 1 );
	//!!! 複数回呼び出される可能性有り。
	texname = (char*)realloc( texname, leng + 1 );
	if( !texname )
		return 1;

	ZeroMemory( texname, leng + 1 );
	strncpy_s( texname, leng + 1, srctexname, leng );

	return 0;
}
int CShdElem::SetExtTex( unsigned char srctexnum, unsigned char srctexmode, unsigned char srctexrep, unsigned char srctexstep, char** srctexname )
{
	int texno;

	//以前のexttexを削除
	for( texno = 0; texno < exttexnum; texno++ ){
		char* delname = *( exttexname + texno );
		
//DbgOut( "ShdElem : SetExtTex : delname %s--\n", delname );

		if( delname )
			free( delname );
	}
	if( exttexname ){
		free( exttexname );
	}
	exttexname = 0;
	exttexnum = 0;


	if( srctexnum > MAXEXTTEXNUM ){
		DbgOut( "ShdElem : SetExtTex : texnum too large error !!!\n" );
		srctexnum = MAXEXTTEXNUM;//!!!!!
	}


	if( srctexnum > 0 ){
		//exttexnameの作り直し。
		exttexname = (char**)malloc( sizeof( char* ) * srctexnum );
		if( !exttexname ){
			DbgOut( "ShdElem : SetExtTex : exttexname alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		for( texno = 0; texno < srctexnum; texno++ ){
			char* curname;
			int curleng;
			curname = *( srctexname + texno );
			curleng = (int)strlen( curname );
			if( (curleng >= 256) || (curleng < 0) ){
				DbgOut( "ShdElem : SetExtTex : texname too long error !!! %d, %s\n", texno, curname );
				_ASSERT( 0 );
				curleng = 255;//途中までしか保存しない。shandler::CreateTexture時にエラーになる。
				//return 1;
			}

			char* newname;
			newname = (char*)malloc( curleng + 1 );//!!!!!!
			if( !newname ){
				DbgOut( "ShdElem : SetExtTex : newname alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			strncpy_s( newname, curleng + 1, curname, curleng );
			*( newname + curleng ) = 0;

			*( exttexname + texno ) = newname;

//DbgOut( "shdelem : SetExtTex : loop : texno %d, curname %s, curleng %d, exttexname %s\n",
//	texno, curname, curleng, *( exttexname + texno ) );

		}
	}

	exttexnum = srctexnum;
	exttexmode = srctexmode;
	exttexrep = srctexrep;
	exttexstep = srctexstep;


//DbgOut( "shdelem : SetExtTex : Exit\n" );

	return 0;
}


int CShdElem::SetTexRepetition( int srcrepx, int srcrepy )
{
	texrepx = srcrepx;
	texrepy = srcrepy;
	return 0;
}

int CShdElem::SetTexTransparent( int srctp )
{
	transparent = srctp;
	return 0;
}

int CShdElem::SetAlpha( float srcalpha )
{
	alpha = srcalpha;

	/***
	int ialpha;
	ialpha = (int)( alpha * 255.0f );
	if( ialpha > 255 ) 
		ialpha = 255;
	if( ialpha < 0 )
		ialpha = 0;

	int ret;
	if( d3ddisp ){
		ret = d3ddisp->SetDispTlvAlpha( ialpha );
		if( ret ){
			DbgOut( "selem : SetAlpha : SetDispTlvAlpha error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	***/
	int ret;
	if( d3ddisp ){
		ret = d3ddisp->SetDispTlvAlpha( srcalpha );
		if( ret ){
			DbgOut( "selem : SetAlpha : SetDispTlvAlpha error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

int CShdElem::SetUVAnime( float srcuanime, float srcvanime )
{
	uanime = srcuanime;
	vanime = srcvanime;
	return 0;
}

int CShdElem::SetTexRule( int srcrule )
{
	texrule = srcrule;
	return 0;
}
int CShdElem::SetNoSkinning( int srcint )
{
	noskinning = srcint;
	return 0;
}
int CShdElem::SetDispSwitchNo( DWORD srcdw )
{
	dispswitchno = srcdw;
	return 0;
}

int CShdElem::SetNotUse( int srcint )
{
	notuse = srcint;
	return 0;
}

int CShdElem::SetDispFlag( int srcdflag )
{
	dispflag = srcdflag;
	return 0;
}

int CShdElem::SetMem( int* srcint, __int64 setflag )
{
	int ret;
	ret = CBaseDat::SetMem( srcint, setflag );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : SetMem : basedat::SetMem srcint error !!!\n" );
		return 1;
	}
	return 0;

}
	
int CShdElem::SetVecLine( int* srcint, __int64 setflag )
{
	int ret;
	if( vline ){
		ret = vline->SetMem( srcint, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetVecLine : vline->SetMem srcint error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetVecLine : vline NULL !!!\n" );
		return 1;
	}
}

int CShdElem::SetVecLine( CVec3f* srcvec, int datano, __int64 setflag )
{
	int ret;
	if( vline ){
		ret = vline->SetMem( srcvec, datano, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetVecLine : vline->SetMem srcvec error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetVecLine : vline NULL !!!\n" );
		return 1;
	}	

}
int CShdElem::SetVecLine( VEC3F* srcvec, int vecnum )
{
	int ret;
	if( vline ){
		ret = vline->SetMem( srcvec, vecnum );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetPolygon( int* srcint, __int64 setflag )
{
	int ret;
	if( polygon ){
		ret = ((CVecMesh*)polygon)->SetMem( srcint, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetVecLine : polygon->SetMem srcint error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetVecLine : polygon NULL !!!\n" );
		return 1;
	}

}

int CShdElem::SetPolygon( CVec3f* srcvec, int lineno, int linekind, int vecno, __int64 setflag )
{
	int ret;
	if( polygon ){
		ret = ((CVecMesh*)polygon)->SetMem( srcvec, lineno, linekind, vecno, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetVecLine : polygon->SetMem srcvec error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetVecLine : polygon NULL !!!\n" );
		return 1;
	}	

}
int CShdElem::SetPolygon( VEC3F* srcvec, int linevno, int linekind, int vnum )
{
	int ret;
	if( polygon ){
		ret = ((CVecMesh*)polygon)->SetMem( srcvec, linevno, linekind, vnum );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::Add2CurPolygon( CMeshInfo* srclineinfo )
{
	int ret;

	CMeshInfo lineinfo[2];
	lineinfo[0].CopyData( srclineinfo );
	lineinfo[1].CopyData( srclineinfo );

	if( polygon ){
		ret = polygon->AddVecLine( lineinfo, 2 );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : Add2CurPolygon : AddVecLine error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;

}
int CShdElem::SetSphere( int* srcint, __int64 setflag )
{
	int ret;
	if( sphere ){
		ret = ((CVecMesh*)sphere)->SetMem( srcint, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetVecLine : sphere->SetMem srcint error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetVecLine : sphere NULL !!!\n" );
		return 1;
	}

}

int CShdElem::SetSphere( CMatrix2* srcdefmat, __int64 setflag )
{
	int ret;
	if( sphere ){
		ret = sphere->SetMem( srcdefmat, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetVecLine : sphere->SetMem srcvec error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetVecLine : sphere NULL !!!\n" );
		return 1;
	}		

}

int CShdElem::SetBMesh( int* srcint, __int64 setflag )
{
	int ret;
	if( bmesh ){
		ret = ((CVecMesh*)bmesh)->SetMem( srcint, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetBMesh : bmesh->SetMem srcint error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetBMesh : bmesh NULL !!!\n" );
		return 1;
	}

}

int CShdElem::SetBMesh( CShdElem* blelem, int brono, int blseri )
{
	CBezLine* blptr = 0;
	int ret = 0;

	if( (type == SHDBEZIERSURF) && bmesh &&
		(blelem->type == SHDBEZLINE) && blelem && (blptr = blelem->bline) ){
		
		ret = bmesh->SetMem( blptr, brono, blseri, SHDBEZLINE );
		_ASSERT( !ret );

		return ret;
	}else{
		return 1;
	}

}

int CShdElem::SetBLine( int srcint, __int64 setflag )
{
	// skip
	int ret, i;


	if( bline && (setflag == MESHI_SKIP) ){

		ret = ((CVecMesh*)bline)->SetMem( &srcint, MESHI_SKIP );
		if( ret ){
			DbgOut( "CShdElem : SetBLine : bline->SetMem error !!!\n" );
			return 1;
		}
		for( i = 0; i < LIM_MAX; i++ ){		
			ret = ((CVecMesh*)bline)->SetMem( srcint, i, setflag );
			if( ret ){
				DbgOut( "shddata.cpp : CShdTree : SetBMesh : bline->SetMem srcint MESHI_SKIP %d error !!!\n", i );
				return 1;
			}
		}

		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetBLine : bline NULL !!!\n" );
		return 1;
	}

}
int CShdElem::SetBLine( CBezData* srcbez, int bezno, __int64 setflag )
{
	int ret;
	if( bline ){
		ret = bline->SetMem( srcbez, bezno, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetBLine : bline->SetMem srcbez error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetBLine : bline NULL !!!\n" );
		return 1;
	}

}

int CShdElem::SetBLine( CVec3f* srcvec, int srclineno, int srcvecno )
{
	int ret = 0;

	if( bline ){
		ret = ((CVecMesh*)bline)->SetMem( srcvec, srclineno, srcvecno, VEC_ALL );
		if( ret ){
			DbgOut( "CShdElem : SetBLine : SetMem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "CShdElem : SetBLine : bline NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetBLine( VEC3F* srcvec, int srclineno, int srcvnum )
{
	int ret;
	if( bline ){
		ret = ((CVecMesh*)bline)->SetMem( srcvec, srclineno, srcvnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		return 0;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetBLine( CVec3f* srcvec, int srcvecleng, int srcbezno, __int64 setflag )
{
	int ret = 0;
	int pointkind;	

	_ASSERT( srcvecleng == 5 ); // bezpoint leng

	if( bline ){
		for( pointkind = 0; pointkind < srcvecleng; pointkind++ ){
			// bezline 中では、lineno がpointkind, pointno が bezno。
			ret += ((CVecMesh*)bline)->SetMem( srcvec + pointkind, pointkind, srcbezno, VEC_ALL );
			_ASSERT( !ret );
		}
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetBLine : bline->SetMem srcvec error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetBLine : bline NULL !!!\n" );
		return 1;
	}

}


int CShdElem::SetExtrude( VEC3F srcexvec )
{
	int ret;

	if( extrude ){
		ret = extrude->SetExVec( srcexvec );
		_ASSERT( !ret );
		return ret;
	}else{
		DbgOut( "CShdElem : SetExtrude VEC3F : extrude NULL !!!\n" );
		return 1;
	}
}
int CShdElem::SetExtrude( CVec3f* srcvec, int pno, __int64 setflag )
{
	int ret;

	if( extrude ){
		ret = ((CVecMesh*)extrude)->SetMem( srcvec, 0, pno, setflag );
		_ASSERT( !ret );
		return ret;
	}else{
		DbgOut( "CShdElem : SetExtrude Cvec3f : extrude NULL !!!\n" );
		return 1;
	}
}
int CShdElem::SetExtrude( VEC3F* srcvec, int lineno, int vnum )
{
	int ret;

	if( extrude ){
		ret = ((CVecMesh*)extrude)->SetMem( srcvec, lineno, vnum );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetPolyMesh2MItoMaterial()
{
	int ret;
	if( polymesh2 ){
		ret = polymesh2->MItoMaterial();
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetPolyMesh2SamePoint()
{
	int ret;
	if( polymesh2 ){
		ret = polymesh2->SetSamePoint();
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetPolyMesh2Point( int srcpno, float srcx, float srcy, float srcz, int ono )
{
	int ret;
	if( polymesh2 ){
		ret = polymesh2->SetPointBuf( srcpno, srcx, srcy, srcz, ono );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetPolyMesh2Material( int faceno, float srcdiffuse, float srcambient, float srcspecular, float power, float emi, ARGBF color, ARGBF scene_ambient, COORDINATE* srcuv, int srcvnum )
{
	int ret;
	if( polymesh2 ){
		ret = polymesh2->SetMaterial( faceno, srcdiffuse, srcambient, srcspecular, power, emi, color, scene_ambient, srcuv, srcvnum );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetPolyMesh2Attrib0( int faceno, int srcmaterialno )
{
	int ret;
	if( polymesh2 ){
		ret = polymesh2->SetAttrib0( faceno, srcmaterialno );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}


int CShdElem::SetPolyMesh2VCol( int vertno, __int64 vcol )
{
	int ret;
	if( polymesh2 ){
		ret = polymesh2->SetVCol( vertno, vcol );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}


int CShdElem::SetPolyMesh2Color( int srcpno, ARGBF* srccol )
{
	int ret;
	if( polymesh2 ){
		ret = polymesh2->SetColor( srcpno, srccol );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}


int CShdElem::SetPolyMeshPoint( int srcpno, float srcx, float srcy, float srcz, int orgno )
{
	int ret;
	if( polymesh ){
		ret = polymesh->SetPointBuf( srcpno, srcx, srcy, srcz, orgno );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}
int CShdElem::SetPolyMeshPoint( int srcpnum, VEC3F* srcvec )
{
	int ret;
	if( polymesh ){
		ret = polymesh->SetPointBuf( srcpnum, srcvec );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::CreatePolyMeshIndex( int srcfacenum )
{
	int ret;
	if( polymesh ){
		ret = polymesh->CreateIndexBuf( srcfacenum );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::InvPolyMeshIndex( int srcfaceno )
{
	int ret;
	if( polymesh ){
		ret = polymesh->InvIndexBuf( srcfaceno );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetPolyMeshIndex( int srcfaceno, int* srclist )
{
	int ret;
	if( polymesh ){
		ret = polymesh->SetIndexBuf( srcfaceno, srclist );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}
int CShdElem::SetPolyMeshIndex( int srcdatanum, int* srclist, int srcflag )
{
	int ret;
	if( polymesh ){
		ret = polymesh->SetIndexBuf( srcdatanum, srclist, srcflag );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CShdElem::SetRevolved( int* srcint, __int64 setflag )
{
	int ret;
	if( revolved ){
		if( setflag & REV_NSET ){
			ret = revolved->SetMem( srcint, setflag );
		}else{
			ret = ((CVecMesh*)revolved)->SetMem( srcint, setflag );
		}
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved->SetMem srcint error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved NULL !!!\n" );
		return 1;
	}

}

int CShdElem::SetRevolved( CVec3f* srcvec, int lineno, int vecno, __int64 setflag )
{
	int ret;
	if( revolved ){
		ret = ((CVecMesh*)revolved)->SetMem( srcvec, lineno, vecno, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved->SetMem srcvec error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved NULL !!!\n" );
		return 1;
	}	

}

int CShdElem::SetRevolved( VEC3F* srcvec, int lineno, int vnum )
{
	int ret;
	if( revolved ){
		ret = ((CVecMesh*)revolved)->SetMem( srcvec, lineno, vnum );
		_ASSERT( !ret );
		return ret;
	}else{
		_ASSERT( 0 );
		return 1;
	}

}

int CShdElem::SetRevolved( CMatrix2* srcaxis, __int64 setflag )
{
	int ret;
	if( revolved ){
		ret = revolved->SetMem( srcaxis, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved->SetMem srcaxis error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved NULL !!!\n" );
		return 1;
	}		

}
int CShdElem::SetRevolved( float* srcfloat, __int64 setflag )
{
	int ret;
	if( revolved ){
		ret = revolved->SetMem( srcfloat, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved->SetMem srcfloat error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetRevolved : revolved NULL !!!\n" );
		return 1;
	}		
}

int CShdElem::SetDisk( int* srcint, __int64 setflag )
{
	int ret;
	if( disk ){
		ret = disk->SetMem( srcint, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetDisk : disk->SetMem srcint error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetDisk : disk NULL !!!\n" );
		return 1;
	}

}

int CShdElem::SetDisk( float* srcfloat, __int64 setflag )
{
	int ret;
	if( disk ){
		ret = disk->SetMem( srcfloat, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetDisk : disk->SetMem srcfloat error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetDisk : disk NULL !!!\n" );
		return 1;
	}

}
int CShdElem::SetDisk( CMatrix2* srcdefmat, __int64 setflag )
{
	int ret;
	if( disk ){
		ret = disk->SetMem( srcdefmat, setflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : SetDisk : disk->SetMem srcvec error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		DbgOut( "shddata.cpp : CShdTree : SetDisk : disk NULL !!!\n" );
		return 1;
	}		

}

int CShdElem::SetJointLoc( CVec3f* srcvec )
{
	int ret;
	if( part ){
		ret = part->SetJointLoc( srcvec );
		return ret;
	}else{
		DbgOut( "shdelem : SetJointLoc : part NULL error !!!\n" );
		return 1;
	}
}

int CShdElem::AddBoneInfo( CShdElem* chielem, CShdElem* parelem )
{
	CPart* chipart = chielem->part;
	if( !part || !chipart ){
		_ASSERT( 0 );
		return 1;
	}

	CVec3f chiloc;
	chiloc.CopyData( &(chipart->jointloc) );

	int parno;
	if( parelem ){
		parno = parelem->serialno;
		CPart* parpart = parelem->part;
		if( !parpart ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		parno = -1;
	}

	int ret;
	ret = part->AddBoneInfo( serialno, chielem->serialno, chiloc, parno );
	if( ret ){
		DbgOut( "CShdElem : AddBoneInfo : part->AddBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::DumpBoneInfo( CTreeHandler2* srclpth )
{
	if( !part ){
		_ASSERT( 0 );
		return 1;
	}

	int bonenum = part->bonenum;
	if( bonenum ){
		for( int boneno = 0; boneno < bonenum; boneno++ ){
			CBoneInfo* curbi;
			curbi = *(part->ppBI + boneno);
			_ASSERT( curbi );

			DbgOut( "CShdElem : DumpBoneInfo : %s : serial %d : child %f,%f,%f, parent %d\n",
				(*srclpth)(serialno)->name,
				curbi->jointno,
				curbi->boneend.x, curbi->boneend.y, curbi->boneend.z,
				curbi->parentjoint
			);
		}
	}
	return 0;
}

	// check
/***
int CShdElem::IsDispObj( int dispflag )
{
	int ret = 0;
	switch( type ){
		//case SHDJOINT:
		case SHDROTATE:
		case SHDSLIDER:
		case SHDSCALE:
		case SHDUNISCALE:
		case SHDPATH:
			if( dispflag & DISP_JOINT )
				ret = 1;
			break;
		case SHDPOLYLINE:
			if( dispflag & DISP_VLINE )
				ret = 1;
			break;
		case SHDPOLYGON:
			if( dispflag & DISP_POLYGON )
				ret = 1;
			break;
		case SHDSPHERE:
			if( dispflag & DISP_SPHERE )
				ret = 1;
			break;
		case SHDBEZIERSURF:
			if( dispflag & DISP_BMESH )
				ret = 1;
			break;
		case SHDMESHES:
			if( dispflag & DISP_REVOLVED )
				ret = 1;
			break;
		case SHDDISK:
			if( dispflag & DISP_DISK )
				ret = 1;
			break;
		case SHDBEZLINE:
			if( dispflag & DISP_BLINE )
				ret = 1;
			break;
		default:
			break;
	}
	return ret;

}
***/

int CShdElem::IsJoint()
{
	//if( (type >= SHDROTATE) && (type <= SHDMORPH) )
	if( (type >= SHDROTATE) && (type <= SHDBALLJOINT) && (type != SHDMORPH) )
		return 1;
	else
		return 0;

}

int CShdElem::Dbg_Dump( HANDLE hfile, CTreeHandler2* thandler )
{
	int ret = 0;
	int brono, childno, parno;
	CTreeElem2* curte = 0;
	CTreeElem2* brote = 0;
	CTreeElem2* childte = 0;
	CTreeElem2* parte = 0;

	char* treename = 0;
	char* broname = 0;
	char* childname = 0;
	char* parname = 0;

	curte = (*thandler)( serialno );
	treename = curte->name;

	if( parent ){
		parno = parent->serialno;
		parte = (*thandler)( parno );
		parname = parte->name;
	}

	if( brother ){
		brono = brother->serialno;
		brote = (*thandler)( brono );
		broname = brote->name;
	}

	if( child ){
		childno = child->serialno;
		childte = (*thandler)( childno );
		childname = childte->name;
	}

	ret = Write2File( hfile, "serialno %d, name %s, parname %s, broname %s, childname %s\n",
		serialno, treename, parname, broname, childname );
	_ASSERT( !ret );
	
	return ret;
}

void CShdElem::DumpReq( HANDLE hfile, CTreeHandler2* thandler, CMotHandler* lpmh, int* errcnt )
{
	int ret = 0;
	
	if( *errcnt ){
		_ASSERT( 0 );
		return;
	}

	ret = DumpText( hfile, thandler, lpmh );
	*errcnt = ret;


	if( child )
		child->DumpReq( hfile, thandler, lpmh, errcnt );

	if( brother )
		brother->DumpReq( hfile, thandler, lpmh, errcnt );

}

int CShdElem::DumpText( HANDLE hfile, CTreeHandler2* thandler, CMotHandler* lpmh )
{
	int ret = 0;
	int level = depth;

	// dump CTreeElem2
	ret = (*thandler)( serialno )->DumpText( hfile );	
	if( ret ){
		DbgOut( "CShdElem : DumpText : dump CTreeElem2 error %d !!!\n", serialno );
		_ASSERT( 0 );
		return 1;
	}
	
	// dump CShdElem

	
	level++;
	ret = DumpElem( hfile, level, lpmh );
	if( ret ){
		DbgOut( "CShdElem : DumpText : dump CShdElem error %d !!!\n", serialno );
		_ASSERT( 0 );
		return 1;
	}
	
	return ret;
}



int CShdElem::DumpElem( HANDLE hfile, int level, CMotHandler* lpmh )
{
	int	ret = 0;
	char tempchar[1025];
	int hasmotion;

	hasmotion = (*lpmh)( serialno )->hasmotion;

	SetTabChar( level );

	ret = Write2File( hfile, "%sshdelem depth : %d, clockwise %d, BDivU %d, BDivV %d, hasmotion %d,\n", 
		tabchar, depth, clockwise, bdivU, bdivV, hasmotion );
	_ASSERT( !ret );

	// BaseDat
	ZeroMemory( tempchar, 1025 );

	if( !texname ){
		sprintf_s( tempchar, 1025, 
			"texname 0, TexRepX 0, TexRepY 0, TRANSPARENTBLACK 0, UANIME 0.0, VANIME 0.0, ALPHA %f,",
			alpha );
	}else{
		sprintf_s( tempchar, 1025,
			"texname %s, TexRepX %d, TexRepY %d, TRANSPARENTBLACK 0, UANIME 0.0, VANIME 0.0, ALPHA %f,", 
			texname, texrepx, texrepy, alpha );
	}

	ret = CBaseDat::DumpMem( hfile, level, tempchar );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : DumpElem : basedat::DumpMem error !!!\n" );
		return 1;
	}

	
	// 初期化用データのダンプ
	ret = DumpObj( hfile, level, DISP_ALL );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : DumpElem : DumpObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	} 
	

	return ret;
}

/***
int CShdElem::DumpElem( HANDLE hfile, int dumpflag )
{
	int	ret, level;
	char tempchar[20];

	level = depth;

	if( IsDispObj( dumpflag ) ){ // 3Dobjの選択
		// BaseDat
		ZeroMemory( tempchar, 20 );
		sprintf( tempchar, "TREE%d", depth );
		ret = CBaseDat::DumpMem( hfile, level, tempchar );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : basedat::DumpMem error !!!\n" );
			return 1;
		}

		// 3DObj
		if( dumpflag & DISP_TRANS ){
			// d_* のダンプ
			ret = DumpDObj( hfile, level, dumpflag );
		}else{
			// 初期化用データのダンプ
			ret = DumpObj( hfile, level, dumpflag );
		}
	}

	return ret;
}
***/

/***
int CShdElem::DumpObj( HANDLE hfile, int level, int dumpflag )
{
	int ret = 0;
	if( vline ){
		level++;
		ret = vline->DumpMem( hfile, level, dumpflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : vline->DumpMem error !!!\n" );
			return 1;
		}
	}	
	if( polygon ){
		level++;
		ret = polygon->DumpMem( hfile, level, dumpflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : polygon->DumpMem error !!!\n" );
			return 1;
		}
	}
	if( sphere ){
		level++;
		ret = sphere->DumpMem( hfile, level, dumpflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : sphere->DumpMem error !!!\n" );
			return 1;
		}
	}
	if( bmesh ){
		level++;
		ret = bmesh->DumpMem( hfile, level, dumpflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : bmesh->DumpMem error !!!\n" );
			return 1;
		}
	}
	if( revolved ){
		level++;
		ret = revolved->DumpMem( hfile, level, dumpflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : revolved->DumpMem error !!!\n" );
			return 1;
		}
	}
	if( disk ){
		level++;
		ret = disk->DumpMem( hfile, level, dumpflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : disk->DumpMem error !!!\n" );
			return 1;
		}
	}
	if( bline ){
		level++;
		ret = bline->DumpMem( hfile, level, dumpflag );
		if( ret ){
			DbgOut( "shddata.cpp : CShdTree : DumpTreeStruc : bline->DumpMem error !!!\n" );
			return 1;		
		}
	}
	return 0;
}
***/

int CShdElem::DumpObj( HANDLE hfile, int level, int dumpflag )
{
	/***
	int ret = 0;

	switch( type ){

		//case SHDROOT:
		case SHDROTATE:
		case SHDSLIDER:
		case SHDSCALE:
		case SHDUNISCALE:
		case SHDPATH:		
		case SHDPART:
			_ASSERT( part );
			level++;
			ret = part->DumpMem( hfile, level, dumpflag );
			break;

		case SHDMORPH:
			_ASSERT( morph );
			level++;
			ret = morph->DumpMem( hfile, level, dumpflag );
			break;

		case SHDPOLYLINE:
			_ASSERT( vline );
			level++;
			ret = vline->DumpMem( hfile, level, dumpflag );
			break;
		case SHDPOLYGON:
			_ASSERT( polygon );
			level++;
			ret = polygon->DumpMem( hfile, level, dumpflag );
			break;
		case SHDSPHERE:
			_ASSERT( sphere );
			level++;
			ret = sphere->DumpMem( hfile, level, dumpflag );
			break;
		case SHDBEZIERSURF:
			_ASSERT( bmesh );
			level++;
			ret = bmesh->DumpMem( hfile, level, dumpflag );
			break;
		case SHDMESHES:
			_ASSERT( revolved );
			level++;
			ret = revolved->DumpMem( hfile, level, dumpflag );
			break;
		case SHDDISK:
			_ASSERT( disk );
			level++;
			ret = disk->DumpMem( hfile, level, dumpflag );
			break;
		case SHDBEZLINE:	// CBezLine
			_ASSERT( bline );
			level++;
			ret = bline->DumpMem( hfile, level, dumpflag );
			break;
		case SHDEXTRUDE:
			_ASSERT( extrude );
			level++;
			ret = extrude->DumpMem( hfile, level, dumpflag );
			break;
		case SHDTYPENONE:
		case SHDTYPEERROR:
		case SHDTYPEMAX:
			// error
			DbgOut( "CShdElem : DumpObj : type error %d !!!", type );
			_ASSERT( 0 );
			ret = 1;
			break;

		default:
			// 未対応
			ret = 0;
			break;
	}

	return ret;
	***/
	return 0;
}



	/// 呼び出しが　うざいので　表引き。　でもセットもうざい
void CShdElem::InitObjFunc()
{
	int i;

	for( i = 0; i < SHDTYPEMAX; i++ ){
		switch( i ){
		case SHDPOLYLINE:
			CreateObjFunc[i] = &CShdElem::CreateVLine;
			DestroyObjFunc[i] = &CShdElem::DestroyVLine;
			//CreateDObjFunc[i] = &CShdElem::CreateDVLine;	
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;
		case SHDPOLYGON:
			CreateObjFunc[i] = &CShdElem::CreatePolygon;
			DestroyObjFunc[i] = &CShdElem::DestroyPolygon;
//			CreateDObjFunc[i] = &CShdElem::CreateDPolygon;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;
		case SHDSPHERE:
			CreateObjFunc[i] = &CShdElem::CreateSphere;
			DestroyObjFunc[i] = &CShdElem::DestroySphere;
//			CreateDObjFunc[i] = &CShdElem::CreateDSphere;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;
		case SHDBEZIERSURF:
			CreateObjFunc[i] = &CShdElem::CreateBMesh;
			DestroyObjFunc[i] = &CShdElem::DestroyBMesh;
//			CreateDObjFunc[i] = &CShdElem::CreateDBMesh;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;
		case SHDMESHES:
			CreateObjFunc[i] = &CShdElem::CreateRevolved;
			DestroyObjFunc[i] = &CShdElem::DestroyRevolved;
//			CreateDObjFunc[i] = &CShdElem::CreateDRevolved;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;
		case SHDDISK:
			CreateObjFunc[i] = &CShdElem::CreateDisk;
			DestroyObjFunc[i] = &CShdElem::DestroyDisk;
//			CreateDObjFunc[i] = &CShdElem::CreateDDisk;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;
		case SHDBEZLINE:
			CreateObjFunc[i] = &CShdElem::CreateBLine;
			DestroyObjFunc[i] = &CShdElem::DestroyBLine;
//			CreateDObjFunc[i] = &CShdElem::CreateDBLine;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;

		case SHDMORPH:
			CreateObjFunc[i] = &CShdElem::CreateMorph;
			DestroyObjFunc[i] = &CShdElem::DestroyMorph;
//			CreateDObjFunc[i] = &CShdElem::CreateDMorph;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;

		//case SHDROOT:
		case SHDROTATE:
		case SHDSLIDER:
		case SHDSCALE:
		case SHDUNISCALE:
		case SHDPATH:		
		case SHDPART:
		case SHDBALLJOINT:
			CreateObjFunc[i] = &CShdElem::CreatePart;
			DestroyObjFunc[i] = &CShdElem::DestroyPart;
			CreateDObjFunc[i] = &CShdElem::CreateDPart;
			break;	
		
		case SHDEXTRUDE:
			CreateObjFunc[i] = &CShdElem::CreateExtrude;
			DestroyObjFunc[i] = &CShdElem::DestroyExtrude;
//			CreateDObjFunc[i] = &CShdElem::CreateDExtrude;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;	
			break;
		case SHDPOLYMESH:
			CreateObjFunc[i] = &CShdElem::CreatePolyMesh;
			DestroyObjFunc[i] = &CShdElem::DestroyPolyMesh;
			CreateDObjFunc[i] = &CShdElem::CreateDPolyMesh;
			break;
		case SHDPOLYMESH2:
			CreateObjFunc[i] = &CShdElem::CreatePolyMesh2;
			DestroyObjFunc[i] = &CShdElem::DestroyPolyMesh2;
			CreateDObjFunc[i] = &CShdElem::CreateDPolyMesh2;
			break;
		case SHDGROUNDDATA:
			CreateObjFunc[i] = &CShdElem::CreateGroundData;
			DestroyObjFunc[i] = &CShdElem::DestroyGroundData;
			CreateDObjFunc[i] = &CShdElem::CreateDGroundData;
			break;
		case SHDBILLBOARD:
			CreateObjFunc[i] = &CShdElem::CreateBillboard;
			DestroyObjFunc[i] = &CShdElem::DestroyBillboard;
			CreateDObjFunc[i] = &CShdElem::CreateDBillboard;
			break;
		case SHDEXTLINE:
			CreateObjFunc[i] = &CShdElem::CreateExtLine;
			DestroyObjFunc[i] = &CShdElem::DestroyExtLine;
			CreateDObjFunc[i] = &CShdElem::CreateDExtLine;
			break;
		case SHDINFSCOPE:
			CreateObjFunc[i] = &CShdElem::CreateDummy;
			DestroyObjFunc[i] = &CShdElem::DestroyInfScope;
			CreateDObjFunc[i] = &CShdElem::CreateDInfScope;
			break;
		case SHDBBOX:
			CreateObjFunc[i] = &CShdElem::CreateDummy;
			DestroyObjFunc[i] = &CShdElem::DestroyBBox;
			CreateDObjFunc[i] = &CShdElem::CreateDBBox;
			break;

		case SHDDESTROYED:
		default:
			CreateObjFunc[i] = &CShdElem::CreateDummy;
			DestroyObjFunc[i] = &CShdElem::DestroyDummy;
			CreateDObjFunc[i] = &CShdElem::CreateDDummy;
			break;
		}
	}
}

int	CShdElem::CreateObjs()
{
	return 0;
}
void	CShdElem::DestroyObjs()
{
	CBaseDat::DestroyObjs();
	(this->*DestroyObjFunc[type])();

	if( texname ){
		free( texname );
		texname = 0;
	}

	if( exttexnum > 0 ){
		int i;
		for( i = 0; i < exttexnum; i++ ){
			char* delname = *( exttexname + i );
			
			if( delname )
				free( delname );
		}
		if( exttexname )
			free( exttexname );

		exttexname = 0;
		exttexnum = 0;
	}


	if( influencebone ){
		free( influencebone );
		influencebone = 0;
	}
	influencebonenum = 0;

	if( ignorebone ){
		free( ignorebone );
		ignorebone = 0;
	}
	ignorebonenum = 0;

	DestroyBSphereData();


}

	//int Destroy3DObj( int srctype )
int CShdElem::DestroyVLine()
{
	if( vline ){
		delete vline;
		vline = 0;
		DbgOut( "shddata.cpp : DestroyVLine\n" );
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	return 0;

}
int CShdElem::DestroyPolygon()
{
	if( polygon ){
		delete polygon;
		polygon = 0;
	}

	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;

}
int CShdElem::DestroySphere()
{
	if( sphere ){
		delete sphere;
		sphere = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	return 0;

}
int CShdElem::DestroyBMesh()
{
	CBezLine* bl = 0;
	int lno, lnum;

	if( bmesh ){
		delete bmesh;
		bmesh = 0;
		DbgOut( "shddata.cpp : DestroyBMesh\n" );
	}
	if( bdiv_bmesh ){
		// bezdivしたbmesh, blineは、shdhandlerの管理下にないので、ここで後処理する。
		lnum = bdiv_bmesh->meshinfo->n;
		for( lno = 0; lno < lnum; lno++ ){
			bl = (*bdiv_bmesh)( lno );
			delete bl;
		}

		delete bdiv_bmesh;
		bdiv_bmesh = 0;
	}

	if( bdiv_bmesh2 ){
		// bezdivしたbmesh, blineは、shdhandlerの管理下にないので、ここで後処理する。
		lnum = bdiv_bmesh2->meshinfo->n;
		for( lno = 0; lno < lnum; lno++ ){
			bl = (*bdiv_bmesh2)( lno );
			delete bl;
		}

		delete bdiv_bmesh2;
		bdiv_bmesh2 = 0;
	}

	
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;

}
int CShdElem::DestroyRevolved()
{
	if( revolved ){
		delete revolved;
		revolved = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;

}
int CShdElem::DestroyDisk()
{
	if( disk ){
		delete disk;
		disk = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;

}
int CShdElem::DestroyBLine()
{
	if( bline ){
		delete bline;
		bline = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;
}

int CShdElem::DestroyMorph()
{
	if( morph ){
		delete morph;
		morph = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	return 0;
}
int CShdElem::DestroyPart()
{
	if( part ){
		delete part;
		part = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;
}

int CShdElem::DestroyExtrude()
{
	if( extrude ){
		delete extrude;
		extrude = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;
}

int CShdElem::DestroyBillboard()
{
	if( billboard ){
		delete billboard;
		billboard = 0;
	}

	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}

	return 0;
}
int CShdElem::DestroyExtLine()
{
	if( extline ){
		delete extline;
		extline = 0;
	}

	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;

}
int CShdElem::DestroyInfScope()
{
	CInfScope* curis;
	int isno;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		_ASSERT( curis );

		delete curis;
	}

	if( ppscope ){
		free( ppscope );
		ppscope = 0;
	}
	scopenum = 0;

	return 0;
}

int CShdElem::DestroyBBox()
{

	CBBox2* curbbx;
	int bbxno;
	for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
		curbbx = *( ppbbox + bbxno );
		_ASSERT( curbbx );

		delete curbbx;
	}

	if( ppbbox ){
		free( ppbbox );
		ppbbox = 0;
	}
	bboxnum = 0;

	return 0;
}

int CShdElem::DestroyPolyMesh()
{
	if( polymesh ){
		delete polymesh;
		polymesh = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;
}

int CShdElem::DestroyPolyMesh2()
{
	if( polymesh2 ){
		delete polymesh2;
		polymesh2 = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;
}

int CShdElem::DestroyGroundData()
{
	if( grounddata ){
		delete grounddata;
		grounddata = 0;
	}
	if( d3ddisp ){
		delete d3ddisp;
		d3ddisp = 0;
	}
	return 0;
}

int CShdElem::DestroyDummy()
{
	//DbgOut( "shddata.cpp : CShdTree : invalid DestroyObjFunc call error !!!\n" );
	return 0;

}

	//int Create3DObj( CMeshInfo* srcmeshinfo )
int CShdElem::CreateVLine( CMeshInfo* srcmeshinfo )
{
	int ret;
	CVecLine* newvline = 0;

	newvline = new CVecLine();
	if( !newvline ){
		DbgOut( "shddata.cpp : CShdTree : CreateVLine : newvline alloc error !!!\n" );
		return 1;
	}

	// lineのmeshinfoは　lexのmeshinfoと同じ
	ret = newvline->InitVecLine( srcmeshinfo );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : newvline->InitVLine error !!!\n" );
		return 1;
	}


	vline = newvline;

	return 0;

}
int CShdElem::CreatePolygon( CMeshInfo* srcmeshinfo )
{
	
	int ret;
	CPolygon* newpolygon = 0;
	CMeshInfo* curinfo = 0;

	newpolygon = new CPolygon();
	if( !newpolygon ){
		DbgOut( "shddata.cpp : CShdTree : CreatePolygon : newpolygon alloc error !!!\n" );
		return 1;
	}

	curinfo = AdjustMeshInfo( srcmeshinfo );
	if( !curinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreatePolygon : AdjustMeshInfo error !!!\n" );
		return 1;
	}

	DbgOut( "CShdtree : CreatePolygon : Call CVecMesh::InitVecMesh\n" );
	ret = ((CVecMesh*)newpolygon)->InitVecMesh( curinfo, 0 ); // lineinfo に無効値を入れて、必要時にAddVecLineでデータをalloc
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : CreatePolygon : newpolygon->InitVecMesh error !!!\n" );
		delete curinfo;
		return 1;
	}

	polygon = newpolygon;
	delete curinfo;

	return 0;
	

}
int CShdElem::CreateSphere( CMeshInfo* srcmeshinfo )
{
	int ret;
	CSphere* newsphere = 0;
	CMeshInfo* curinfo = 0;

	newsphere = new CSphere();
	if( !newsphere ){
		DbgOut( "shddata.cpp : CShdTree : CreateSphere : newsphere alloc error !!!\n" );
		return 1;
	}

	curinfo = AdjustMeshInfo( srcmeshinfo );
	if( !curinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreateSphere : AdjustMeshInfo error !!!\n" );
		return 1;
	}
	ret = ((CVecMesh*)newsphere)->InitVecMesh( curinfo, 0 );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : CreateSphere : newsphere->InitVecMesh error !!!\n" );
		delete curinfo;
		return 1;
	}

	sphere = newsphere;
	delete curinfo;

	return 0;

}

CBezMesh* CShdElem::NewBMesh( CMeshInfo* srcmeshinfo )
{
	int ret;
	CBezMesh*	retbmesh = 0;
	CBezMesh*	newbmesh = 0;
	CMeshInfo* curinfo = 0;
	CMeshInfo* lineinfo = 0;

	
	newbmesh = new CBezMesh();
	if( !newbmesh ){
		DbgOut( "shddata.cpp : CShdTree : NewBMesh : newbmesh alloc error !!!\n" );
		goto newbmexit;
	}

	// lexからの　meshinfo を　調整
	curinfo = AdjustMeshInfo( srcmeshinfo );
	if( !curinfo ){
		DbgOut( "shddata.cpp : CShdTree : NewBMesh : AdjustMeshInfo error !!!\n" );
		goto newbmexit;
	}

	// bezmeshの作成
	ret = newbmesh->InitBezMesh( curinfo, curinfo->n );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : NewBMesh : newbmesh->InitBezMesh error !!!\n" );
		goto newbmexit;
	}

	retbmesh = newbmesh;
	
	goto newbmexit;
newbmexit:
	if( lineinfo )
		delete [] lineinfo;
	if( curinfo )
		delete curinfo;
	if( !retbmesh && newbmesh ) // retbmeshにセットする前に　こけたとき
		delete [] newbmesh;

	return retbmesh;

}

int CShdElem::CreateBMesh( CMeshInfo* srcmeshinfo )
{
	bmesh = NewBMesh( srcmeshinfo );
	if( !bmesh ){
		DbgOut( "CShdElem : CreateBMesh : NewBMesh error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

CBezLine* CShdElem::NewBLine( CMeshInfo* srcmeshinfo )
{
	int ret;
	CBezLine*	retbline = 0;
	CBezLine*	newbline = 0;
	CMeshInfo* curinfo = 0;
	CMeshInfo* lineinfo = 0;
	
	
	newbline = new CBezLine();
	if( !newbline ){
		DbgOut( "shddata.cpp : CShdTree : NewBLine : newbline alloc error !!!\n" );
		goto newblexit;
	}

	// lexからの　meshinfo を　調整
	curinfo = AdjustMeshInfo( srcmeshinfo );
	if( !curinfo ){
		DbgOut( "shddata.cpp : CShdTree : NewBLine : AdjustMeshInfo error !!!\n" );
		goto newblexit;
	}

	// lineinfoの作成
	lineinfo = CreateLineInfo( srcmeshinfo );
	if( !lineinfo ){
		DbgOut( "shddata.cpp : CShdTree : NewBLine : CreateLineInfo error !!!\n" );
		goto newblexit;
	}

	// bezmeshの作成
	ret = ((CVecMesh*)newbline)->InitVecMesh( curinfo, lineinfo );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : NewBLine : newbline->InitVecMesh error !!!\n" );
		goto newblexit;
	}

	retbline = newbline;

	goto newblexit;
newblexit:
	if( lineinfo )
		delete [] lineinfo;
	if( curinfo )
		delete curinfo;
	if( !retbline && newbline ) // bmeshにセットする前に　こけたとき
		delete [] newbline;

	return retbline;


}

int CShdElem::CreateBLine( CMeshInfo* srcmeshinfo )
{
	bline = NewBLine( srcmeshinfo );
	if( !bline ){
		DbgOut( "CShdElem : CreateBLine : NewBLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CShdElem::CreateRevolved( CMeshInfo* srcmeshinfo )
{
	int ret, retval;
	CRevolved*	newrev = 0;
	CMeshInfo* curinfo = 0;
	CMeshInfo* lineinfo = 0;
	

	
	newrev = new CRevolved();
	if( !newrev ){
		DbgOut( "shddata.cpp : CShdTree : CreateRevolved : newrev alloc error !!!\n" );
		retval = 1;
		goto createrevexit;
	}

	// lexからの　meshinfo を　調整
	curinfo = AdjustMeshInfo( srcmeshinfo );
	if( !curinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreateRevolved : AdjustMeshInfo error !!!\n" );
		retval = 1;
		goto createrevexit;
	}

	// lineinfoの作成
	lineinfo = CreateLineInfo( srcmeshinfo );
	if( !lineinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreateRevolved : CreateLineInfo error !!!\n" );
		retval = 1;
		goto createrevexit;
	}

	// bezmeshの作成
	ret = ((CVecMesh*)newrev)->InitVecMesh( curinfo, lineinfo );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : CreateRevolved : newrev->InitVecMesh error !!!\n" );
		retval = 1;
		goto createrevexit;
	}

	revolved = newrev;

	retval = 0;
	goto createrevexit;
createrevexit:
	if( lineinfo )
		delete [] lineinfo;
	if( curinfo )
		delete curinfo;
	if( !revolved && newrev ) // revolvedにセットする前に　こけたとき
		delete [] newrev;

	return retval;	
	

}
int CShdElem::CreateDisk( CMeshInfo* srcmeshinfo )
{
	int ret;
	CDisk* newdisk = 0;
	CMeshInfo* curinfo = 0;

	newdisk = new CDisk();
	if( !newdisk ){
		DbgOut( "shddata.cpp : CShdTree : CreateDisk : newdisk alloc error !!!\n" );
		return 1;
	}

	curinfo = AdjustMeshInfo( srcmeshinfo );
	if( !curinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreateDisk : AdjustMeshInfo error !!!\n" );
		return 1;
	}
	ret = ((CVecMesh*)newdisk)->InitVecMesh( curinfo, 0 );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : CreateDisk : newdisk->InitVecMesh error !!!\n" );
		delete curinfo;
		return 1;
	}

	disk = newdisk;
	delete curinfo;

	return 0;

}

int CShdElem::CreateMorph( CMeshInfo* srcmeshinfo )
{
	CMorph* newmorph = 0;
	int ret = 0;

	newmorph = new CMorph();
	if( !newmorph ){
		_ASSERT( 0 );
		return 1;
	}

	morph = newmorph;
	return 0;
}

int CShdElem::CreatePart( CMeshInfo* srcmeshinfo )
{
	CPart* newpart = 0;
	int ret = 0;

	newpart = new CPart();
	if( !newpart ){
		DbgOut( "CShdElem : CreatePart : part alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newpart->InitPart( srcmeshinfo );
	if( ret ){
		DbgOut( "CShdElem : CreatePart : newpart->InitPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part = newpart;

	return 0;
}

int CShdElem::CreateExtrude( CMeshInfo* srcmeshinfo )
{
	int ret = 0;
	CMeshInfo* curinfo = 0;
	CMeshInfo* lineinfo = 0;
	
	
	extrude = new CExtrude();
	if( !extrude ){
		DbgOut( "shddata.cpp : CShdTree : CreateExtrude : extrude alloc error !!!\n" );
		ret = 1;
		goto createexexit;
	}

	// lexからの　meshinfo を　調整
	curinfo = AdjustMeshInfo( srcmeshinfo );
	if( !curinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreateExtrude : AdjustMeshInfo error !!!\n" );
		ret = 1;
		goto createexexit;
	}

	// lineinfoの作成
	lineinfo = CreateLineInfo( srcmeshinfo );
	if( !lineinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreateExtrude : CreateLineInfo error !!!\n" );
		ret = 1;
		goto createexexit;
	}

	// bezmeshの作成
	ret = ((CVecMesh*)extrude)->InitVecMesh( curinfo, lineinfo );
	if( ret ){
		DbgOut( "shddata.cpp : CShdTree : CreateExtrude : exturde->InitVecMesh error !!!\n" );
		ret = 1;
		goto createexexit;
	}

	goto createexexit;
createexexit:
	if( lineinfo )
		delete [] lineinfo;
	if( curinfo )
		delete curinfo;

	return ret;
}

int CShdElem::CreateBillboard( CMeshInfo* srcmeshinfo )
{
	billboard = new CBillboard();
	if( !billboard ){
		DbgOut( "shdelem : CreateBillboard : billboard alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = billboard->CreateBuffers( srcmeshinfo );
	if( ret ){
		DbgOut( "shdelem : CreateBillboard : billboard CreateBuffers error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::CreateExtLine( CMeshInfo* srcmeshinfo )
{
	extline = new CExtLine();
	if( !extline ){
		DbgOut( "shdelem : CreateExtLine : extline alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = extline->SetMeshInfo( srcmeshinfo );
	if( ret ){
		DbgOut( "shdelem : CreateExtLine : extline SetMeshInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdElem::CreatePolyMesh( CMeshInfo* srcmeshinfo )
{
	polymesh = new CPolyMesh();
	if( !polymesh ){
		DbgOut( "shdelem : CreatePolyMesh : polymesh alloc error !!!\n" );
		return 1;
	}

	int ret;
	ret = polymesh->CreatePointBuf( srcmeshinfo, alpha );
	if( ret ){
		DbgOut( "shdelem : CreatePolyMesh : CreatePointBuf error !!!\n" );
		return 1;
	}


	return 0;
}

int CShdElem::CreatePolyMesh2( CMeshInfo* srcmeshinfo )
{
	polymesh2 = new CPolyMesh2();
	if( !polymesh2 ){
		DbgOut( "shdelem : CreatePolyMesh2 : polymesh alloc error !!!\n" );
		return 1;
	}

	int ret;
	ret = polymesh2->CreateBuffers( srcmeshinfo );
	if( ret ){
		DbgOut( "shdelem : CreatePolyMesh2 : CreateBuffers error !!!\n" );
		return 1;
	}

	return 0;
}

int CShdElem::CreateGroundData( CMeshInfo* srcmeshinfo )
{
	grounddata = new CGroundData();
	if( !grounddata ){
		DbgOut( "shdelem : CreateGroundData : grounddata alloc error !!!\n" );
		return 1;
	}

	int ret;
	ret = grounddata->CreateBuffers( srcmeshinfo );
	if( ret ){
		DbgOut( "shdelem : CreateGroundData : CreateBuffers error !!!\n" );
		return 1;
	}


	return 0;
}

int CShdElem::CreateDummy( CMeshInfo* srcmeshinfo )
{
	// 未対応のSHDTYPE
	//DbgOut( "shddata.cpp : CShdTree : invalid CreateObjFunc call error !!!\n" );
	DbgOut( "shdelem : CreateDummy\n" );
	return 1;

}

int CShdElem::CreateDPart( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	int ret1 = 0;

	if( !part )
		return 1;

	d3ddisp = new CD3DDisp();
	if( !d3ddisp ){
		DbgOut( "CShdElem : CreateDPart : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "selem : CreateDPart : 0 \n" );
	ret1 += d3ddisp->CreateDispData( pd3dDevice, part, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDPart : CreateDispData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//DbgOut( "selem : CreateDPart : 1 \n" );

	if( needcalcbe ){
		ret1 += CalcInfElem( lpsh );
		_ASSERT( !ret1 );
	}
//DbgOut( "selem : CreateDPart : 2 \n" );


	return ret1;
}

int CShdElem::CreateDBillboard( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{

	if( !billboard )
		return 1;

	return 0;
}

int CShdElem::CreateDExtLine( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	
	int ret1 = 0;

	if( !extline )
		return 1;

	d3ddisp = new CD3DDisp();
	if( !d3ddisp ){
		DbgOut( "CShdElem : CreateDExtLine : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->CreateDispData( pd3dDevice, extline, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDExtLine : CreateDispData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//ret1 = InitColor( &(lpsh->m_mc), lpmh, tlmode );
	//if( ret1 ){
	//	DbgOut( "CShdElem : CreateDExtLine : InitColor error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}


	if( tlmode == TLMODE_D3D ){
		ret1 = d3ddisp->Copy2VertexBuffer( 0 );
		if( ret1 ){
			DbgOut( "shdelem : CreateDExtLine : Copy2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret1 = d3ddisp->Copy2IndexBuffer( 0 );
		if( ret1 ){
			DbgOut( "shdelem : CreateDExtLine : Copy2IndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//DbgOut( "shdelem : CreateDExtLine : Copy2Buffer \n" );

	}

/***
		if( g_useGPU == 0 ){
		if( tlmode == TLMODE_D3D ){
			ret1 = d3ddisp->Copy2VertexBuffer( 0 );
			if( ret1 ){
				DbgOut( "shdelem : CreateDPolyMesh2 : Copy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret1 = d3ddisp->Copy2IndexBuffer( 0 );
			if( ret1 ){
				DbgOut( "shdelem : CreateDPolyMesh2 : Copy2IndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		ret1 = d3ddisp->Copy2VertexBuffer( 0 );
		if( ret1 ){
			DbgOut( "shdelem : CreateDPolyMesh2 : Copy2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret1 = d3ddisp->Copy2IndexBufferNotCulling();
		if( ret1 ){
			DbgOut( "shdelem : CreateDPolyMesh2 : Copy2IndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
***/

	return 0;
}

int CShdElem::CreateDInfScope( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	int ret1 = 0;

	CInfScope* curis;
	int isno;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		_ASSERT( curis );
	
		switch( curis->type ){
		case SHDPOLYMESH:

			if( !curis->d3ddisp ){
				curis->d3ddisp = new CD3DDisp();
				if( !curis->d3ddisp ){
					DbgOut( "CShdElem : CreateDInfScope pm : d3ddisp NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret1 = curis->d3ddisp->CreateDispData( pd3dDevice, curis->polymesh, clockwise, texrepx, texrepy, tlmode );
				//ret1 = curis->d3ddisp->CreateDispData( pd3dDevice, curis->polymesh, clockwise, texrepx, texrepy, TLMODE_ORG );
				if( ret1 ){
					DbgOut( "CShdElem : CreateDInfScope pm : CreateDispData error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				//ret1 = curis->polymesh->CreateSortTexBuffer( lpsh->m_mathead );
				//if( ret1 ){
				//	DbgOut( "se : CreateDInfScope : pm CreateSortTexBuffer error !!!\n" );
				//	_ASSERT( 0 );
				//	return 1;
				//}

			}

			break;

		case SHDPOLYMESH2:
			if( !curis->d3ddisp ){
				curis->d3ddisp = new CD3DDisp();
				if( !curis->d3ddisp ){
					DbgOut( "CShdElem : CreateDInfScope pm2 : d3ddisp NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret1 = curis->d3ddisp->CreateDispData( pd3dDevice, curis->polymesh2, clockwise, texrepx, texrepy, tlmode, curis->pm2facet );
				//ret1 = curis->d3ddisp->CreateDispData( pd3dDevice, curis->polymesh2, clockwise, texrepx, texrepy, TLMODE_ORG, curis->pm2facet );
				if( ret1 ){
					DbgOut( "CShdElem : CreateDInfScope pm2 : CreateDispData error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				//ret1 = curis->polymesh2->CreateSortTexBuffer( lpsh->m_mathead );
				//if( ret1 ){
				//	DbgOut( "se : CreateDInfScope : pm2 CreateSortTexBuffer error !!!\n" );
				//	_ASSERT( 0 );
				//	return 1;
				//}

			}
			break;
		default:
			_ASSERT( 0 );
			break;
		}

	}


	if( needcalcbe ){
		ret1 += CalcInfElem( lpsh );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDInfScope : CalcINfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		_ASSERT( curis );
		if( curis->d3ddisp ){
			ret1 = curis->d3ddisp->CreateSkinMat( lpsh );
			if( ret1 ){
				DbgOut( "CShdElem : CreateDInfScope : d3ddisp CreateSkinMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}



	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );

	ret1 = InitColor( seri2boneno, vEyePt, lpsh->m_shader, lpsh->m_overflow, &(lpsh->m_mc), lpmh, tlmode );
	//ret1 = InitColor( seri2boneno, vEyePt, lpsh->m_shader, lpsh->m_overflow, &(lpsh->m_mc), lpmh, TLMODE_ORG );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDInfScope : InitColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_useGPU != 0 ){
		for( isno = 0; isno < scopenum; isno++ ){
			curis = *( ppscope + isno );
			_ASSERT( curis );

			ret1 = curis->d3ddisp->Copy2VertexBuffer( 0 );
			if( ret1 ){
				DbgOut( "shdelem : CreateDInfScope : curis->d3ddisp Copy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret1 = curis->d3ddisp->Copy2IndexBufferNotCulling();
			if( ret1 ){
				DbgOut( "shdelem : CreateDInfScope : curis->d3ddisp  Copy2IndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}


int CShdElem::CreateDBBox( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	
	int ret;

	DestroyBBox();//!!!!!!!!!!!!!


	CShdElem* curobj;
	int objno;
	
	CShdElem* curjoint;
	int jointno;

	for( objno = 0; objno < lpsh->s2shd_leng; objno++ ){
		curobj = (*lpsh)( objno );

		if( (curobj->type == SHDPOLYMESH) || (curobj->type == SHDPOLYMESH2) ){
			
			for( jointno = 0; jointno < lpsh->s2shd_leng; jointno++ ){
				curjoint = (*lpsh)( jointno );

				if( curjoint->IsJoint() && (curjoint->type != SHDMORPH) ){
					
					int* vertarray = 0;
					int vertnum = 0;

					if( curobj->type == SHDPOLYMESH ){
						if( !curobj->polymesh ){
							DbgOut( "selem : CreateDBBox : polymesh NULL error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						ret = curobj->polymesh->GetInfluenceVertNo( curjoint->serialno, 0, &vertnum );
						if( ret ){
							DbgOut( "selem : CreateDBBox : polymesh GetInfluenceVertNo error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						if( vertnum > 2 ){
							vertarray = (int*)malloc( sizeof( int ) * vertnum );
							if( !vertarray ){
								DbgOut( "selem : CreateDBBox : vertarray alloc error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
							ZeroMemory( vertarray, sizeof( int ) * vertnum );

							int tmpvertnum = 0;
							ret = curobj->polymesh->GetInfluenceVertNo( curjoint->serialno, vertarray, &tmpvertnum );
							if( ret ){
								DbgOut( "selem : CreateDBBox : polymesh GetInfluenceVertNo error !!!\n" );
								_ASSERT( 0 );
								free( vertarray );
								return 1;
							}

							if( vertnum != tmpvertnum ){
								DbgOut( "selem : CreateDBBox : vertnum error !!!\n" );
								_ASSERT( 0 );
								free( vertarray );
								return 1;
							}

						}


						ret = curobj->polymesh->GetInfluenceVertNo( curjoint->serialno, vertarray, &vertnum );
						if( ret ){
							DbgOut( "selem : CreateDBBox : polymesh GetInfluenceVertNo error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

					}else if( curobj->type == SHDPOLYMESH2 ){

						if( !curobj->polymesh2 ){
							DbgOut( "selem : CreateDBBox : polymesh2 NULL error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
						ret = curobj->polymesh2->GetInfluenceVertNo( curjoint->serialno, 0, &vertnum );
						if( ret ){
							DbgOut( "selem : CreateDBBox : polymesh2 GetInfluenceVertNo error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}

						if( vertnum > 2 ){
							vertarray = (int*)malloc( sizeof( int ) * vertnum );
							if( !vertarray ){
								DbgOut( "selem : CreateDBBox : vertarray alloc error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
							ZeroMemory( vertarray, sizeof( int ) * vertnum );

							int tmpvertnum = 0;
							ret = curobj->polymesh2->GetInfluenceVertNo( curjoint->serialno, vertarray, &tmpvertnum );
							if( ret ){
								DbgOut( "selem : CreateDBBox : polymesh2 GetInfluenceVertNo error !!!\n" );
								_ASSERT( 0 );
								free( vertarray );
								return 1;
							}

							if( vertnum != tmpvertnum ){
								DbgOut( "selem : CreateDBBox : vertnum error !!!\n" );
								_ASSERT( 0 );
								free( vertarray );
								return 1;
							}
						}
					}

					if( vertarray ){
						ret = AddBBox( lpsh, curobj, curjoint, vertarray, vertnum );
						if( ret ){
							DbgOut( "selem : CreateDBBox : AddBBox error !!!\n" );
							_ASSERT( 0 );
							free( vertarray );
							return 1;
						}

						free( vertarray );
						vertarray = 0;
					}


				}
			}
		}
	}

/////////////////
/////////////////
	int ret1 = 0;


	CBBox2* curbbx;
	int bbxno;
	for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
		curbbx = *( ppbbox + bbxno );
		_ASSERT( curbbx );
	
		if( !curbbx->d3ddisp ){
			curbbx->d3ddisp = new CD3DDisp();
			if( !curbbx->d3ddisp ){
				DbgOut( "CShdElem : CreateDBBox pm2 : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret1 = curbbx->d3ddisp->CreateDispData( pd3dDevice, curbbx->polymesh2, clockwise, texrepx, texrepy, tlmode, curbbx->pm2facet );
			if( ret1 ){
				DbgOut( "CShdElem : CreateDBBox pm2 : CreateDispData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( needcalcbe ){
		ret1 += CalcInfElem( lpsh );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDBBox : CreateINfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );

	ret1 = InitColor( seri2boneno, vEyePt, lpsh->m_shader, lpsh->m_overflow, &(lpsh->m_mc), lpmh, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDBBox : InitColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

///////////////
	if( g_useGPU != 0 ){

		for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
			curbbx = *( ppbbox + bbxno );
			_ASSERT( curbbx );

			ret1 = curbbx->d3ddisp->Copy2VertexBuffer( 0 );
			if( ret1 ){
				DbgOut( "shdelem : CreateDBBox : bbx->d3ddisp : Copy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret1 = curbbx->d3ddisp->Copy2IndexBufferNotCulling();
			if( ret1 ){
				DbgOut( "shdelem : CreateDBBox : bbx->d3ddisp : Copy2IndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CShdElem::CreateDPolyMesh( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	int ret1 = 0;

	if( !polymesh )
		return 1;

	ret1 = polymesh->CalcAnchorCenter();
	if( ret1 ){
		DbgOut( "se : pm : CalcAnchorCenter\n" );
		_ASSERT( 0 );
		return 1;
	}


	d3ddisp = new CD3DDisp();
	if( !d3ddisp ){
		DbgOut( "CShdElem : CreateDPolyMesh : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->CreateDispData( pd3dDevice, polymesh, clockwise, texrepx, texrepy, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDPolyMesh : CreateDispData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( polymesh->billboardflag == 0 ){
		ret1 = polymesh->CreateSortTexBuffer( lpsh->m_mathead );
		if( ret1 ){
			DbgOut( "se : CreateDPolyMesh : pm CreateSortTexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret1 = d3ddisp->SetIndexBuffer( polymesh->m_optindexbuftex, clockwise );
		if( ret1 ){
			DbgOut( "selem : CreateDPolyMesh : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



//DbgOut( "selem : CreateDisp : bef SetUV\n" );
	ret1 = SetUV( lpsh->m_mathead );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDPolyMesh : SetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "selem : CreateDisp : bef CalcInfElem\n" );

	if( needcalcbe ){
		ret1 += CalcInfElem( lpsh );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDPolyMesh : CreateINfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	if( g_useGPU != 0 ){
		if( lpsh->m_shader != COL_TOON1 ){
			if( (type == SHDPOLYMESH) && (polymesh->billboardflag != 0) ){
				ret1 = d3ddisp->CreateSkinMat( lpsh );
				if( ret1 ){
					DbgOut( "CShdElem : CreateDPolyMesh : d3ddisp CreateSkinMat error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				ret1 = d3ddisp->CreateSkinMatTex( lpsh, polymesh->m_texblock, polymesh->m_texblocknum );
				//ret1 = d3ddisp->CreateSkinMat( lpsh );
				if( ret1 ){
					DbgOut( "CShdElem : CreateDPolyMesh : d3ddisp CreateSkinMatTex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}else if( polymesh->m_materialblock ){
			ret1 = d3ddisp->CreateSkinMatToon1( lpsh, polymesh->m_materialblock, polymesh->m_materialnum );
			if( ret1 ){
				DbgOut( "shdelem : CreateDPolyMesh : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

//DbgOut( "selem : CreateDisp : bef InitColor\n" );

	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );

	ret1 = InitColor( seri2boneno, vEyePt, lpsh->m_shader, lpsh->m_overflow, &(lpsh->m_mc), lpmh, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDPolyMesh : InitColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "selem : CreateDisp : bef SetGroundObj\n" );


	if( polymesh->groundflag ){
		D3DXMATRIX inimat;
		D3DXMatrixIdentity( &inimat );

		lpsh->m_mc.InitCache( &inimat );
		
		ret1 = d3ddisp->SetGroundObj( &(lpsh->m_mc), lpmh, inimat );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDPolyMesh : SetGroundObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

//DbgOut( "selem : CreateDisp : bef SetRevVertex\n" );

	if( (clockwise == 3) && ( (tlmode == TLMODE_D3D) || g_useGPU )){
		ret1 = d3ddisp->SetRevVertex( COPYVERTEX | COPYNORMAL | COPYUV | COPYCOLOR | COPYSPECULAR | COPYBLEND );
		if( ret1 ){
			DbgOut( "shdelem : CreateDPolyMesh : SetRevVertex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//DbgOut( "selem : CreateDisp : bef Copy2VertexBuffer\n" );
	ret1 = d3ddisp->Copy2VertexBuffer( 0 );
	if( ret1 ){
		DbgOut( "shdelem : CreateDPolyMesh : Copy2VertexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->Copy2IndexBufferNotCulling();
	if( ret1 ){
		DbgOut( "shdelem : CreateDPolyMesh : Copy2IndexBufferNotCulling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( polymesh->billboardflag == 0 ){
		ret1 = polymesh->ChkAlphaNum( lpsh->m_mathead );
		if( ret1 ){
			DbgOut( "selem : CreateDPolyMesh : pm ChkAlphaNum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


//DbgOut( "selem : CreateDisp : end\n" );

	return 0;
}

int CShdElem::CreateDMorphPM( CPolyMesh* pm, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	int ret1 = 0;

	if( !pm )
		return 1;

//	ret1 = polymesh->CalcAnchorCenter();
//	if( ret1 ){
//		DbgOut( "se : pm : CalcAnchorCenter\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


	d3ddisp = new CD3DDisp();
	if( !d3ddisp ){
		DbgOut( "CShdElem : CreateDMorphPM : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->CreateDispData( pd3dDevice, morph->m_baseelem->polymesh, morph->m_baseelem->clockwise,
		morph->m_baseelem->texrepx, morph->m_baseelem->texrepy, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDMorphPM : CreateDispData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//	ret1 = polymesh->CreateSortTexBuffer( lpsh->m_mathead );
//	if( ret1 ){
//		DbgOut( "se : CreateDPolyMesh : pm CreateSortTexBuffer error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	ret1 = d3ddisp->SetIndexBuffer( pm->m_optindexbuftex, morph->m_baseelem->clockwise );
	if( ret1 ){
		DbgOut( "selem : CreateDMorphPM : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



//	ret1 = SetUV( lpsh->m_mathead );
//	if( ret1 ){
//		DbgOut( "CShdElem : CreateDPolyMesh : SetUV error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	if( needcalcbe ){
		ret1 += CalcInfElem( lpsh );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDMorphPM : CreateINfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	if( g_useGPU != 0 ){
		if( lpsh->m_shader != COL_TOON1 ){
			ret1 = d3ddisp->CreateSkinMatTex( lpsh, pm->m_texblock, pm->m_texblocknum );
			if( ret1 ){
				DbgOut( "CShdElem : CreateDMorphPM : d3ddisp CreateSkinMatTex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( polymesh->m_materialblock ){
			ret1 = d3ddisp->CreateSkinMatToon1( lpsh, pm->m_materialblock, pm->m_materialnum );
			if( ret1 ){
				DbgOut( "shdelem : CreateDMorphPM : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


//	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );
//
//	ret1 = InitColor( seri2boneno, vEyePt, lpsh->m_shader, lpsh->m_overflow, &(lpsh->m_mc), lpmh, tlmode );
//	if( ret1 ){
//		DbgOut( "CShdElem : CreateDPolyMesh : InitColor error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}


//	if( polymesh->groundflag ){
//		D3DXMATRIX inimat;
//		D3DXMatrixIdentity( &inimat );
//
//		lpsh->m_mc.InitCache( &inimat );
//		
//		ret1 = d3ddisp->SetGroundObj( &(lpsh->m_mc), lpmh, inimat );
//		if( ret1 ){
//			DbgOut( "CShdElem : CreateDPolyMesh : SetGroundObj error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//
//	}

//	if( (clockwise == 3) && ( (tlmode == TLMODE_D3D) || g_useGPU )){
//		ret1 = d3ddisp->SetRevVertex( COPYVERTEX | COPYNORMAL | COPYUV | COPYCOLOR | COPYSPECULAR | COPYBLEND );
//		if( ret1 ){
//			DbgOut( "shdelem : CreateDPolyMesh : SetRevVertex error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}

//	ret1 = d3ddisp->Copy2VertexBuffer( 0 );
//	if( ret1 ){
//		DbgOut( "shdelem : CreateDPolyMesh : Copy2VertexBuffer error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	ret1 = d3ddisp->Copy2IndexBufferNotCulling();
	if( ret1 ){
		DbgOut( "shdelem : CreateDMorphPM : Copy2IndexBufferNotCulling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	if( polymesh->billboardflag == 0 ){
//		ret1 = polymesh->ChkAlphaNum( lpsh->m_mathead );
//		if( ret1 ){
//			DbgOut( "selem : CreateDPolyMesh : pm ChkAlphaNum error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}


	return 0;
}


int CShdElem::CreateDMorph( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	_ASSERT( morph );

	int ret;
	ret = CheckDispData();
	if( ret ){
		DbgOut( "CShdTree : warning !!! DispObj exist !!!\n" );
		return 0;
//		delete d3ddisp;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		d3ddisp = 0;
	}

	if( morph->m_objtype == SHDPOLYMESH ){
		ret = CreateDMorphPM( morph->m_baseelem->polymesh, pd3dDevice, lpmh, lpsh, tlmode, needcalcbe );
		if( ret ){
			DbgOut( "se : CreateDMorph : CreateDMorphPM error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( morph->m_objtype == SHDPOLYMESH2 ){
		ret = CreateDMorphPM2( morph->m_baseelem->polymesh2, pd3dDevice, lpmh, lpsh, tlmode, needcalcbe );
		if( ret ){
			DbgOut( "se : CreateDMorph : CreateDMorphPM2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CShdElem::CreateDPolyMesh2( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	int ret1 = 0;

	if( !polymesh2 )
		return 1;

	ret1 = polymesh2->CalcAnchorCenter();
	if( ret1 ){
		DbgOut( "se : pm2 : CalcAnchorCenter\n" );
		_ASSERT( 0 );
		return 1;
	}

	d3ddisp = new CD3DDisp();
	if( !d3ddisp ){
		DbgOut( "CShdElem : CreateDPolyMesh : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->CreateDispData( pd3dDevice, polymesh2, clockwise, texrepx, texrepy, tlmode, facet );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDPolyMesh2 : CreateDispData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = polymesh2->CreateSortTexBuffer( lpsh->m_mathead );
	if( ret1 ){
		DbgOut( "se : CreateDPolyMesh2 : pm2 CreateSortTexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->SetIndexBuffer( polymesh2->m_optindexbuftex, clockwise );
	if( ret1 ){
		DbgOut( "selem : CreateDPolyMesh2 : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret1 = SetUV( lpsh->m_mathead );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDPolyMesh2 : SetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( needcalcbe ){
		ret1 += CalcInfElem( lpsh );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDPolyMesh2 : CreateInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( g_useGPU != 0 ){
		if( lpsh->m_shader != COL_TOON1 ){
			ret1 = d3ddisp->CreateSkinMatTex( lpsh, polymesh2->m_texblock, polymesh2->m_texblocknum );
			if( ret1 ){
				DbgOut( "CShdElem : CreateDPolyMesh2 : d3ddisp CreateSkinMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( polymesh2->m_materialblock ){
			ret1 = d3ddisp->CreateSkinMatToon1( lpsh, polymesh2->m_materialblock, polymesh2->m_materialnum );
			if( ret1 ){
				DbgOut( "shdelem : CreateDPolyMesh2 : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );

	ret1 = InitColor( seri2boneno, vEyePt, lpsh->m_shader, lpsh->m_overflow, &(lpsh->m_mc), lpmh, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : CreateDPolyMesh2 : InitColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( polymesh2->groundflag ){
		D3DXMATRIX inimat;
		D3DXMatrixIdentity( &inimat );

		lpsh->m_mc.InitCache( &inimat );

		ret1 = d3ddisp->SetGroundObj( &(lpsh->m_mc), lpmh, inimat );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDPolyMesh2 : SetGroundObj error !!!\n" );
			_ASSERT( 0 );
		}

	}

	if( (clockwise == 3) && ( (tlmode == TLMODE_D3D) || g_useGPU ) ){
		ret1 = d3ddisp->SetRevVertex( COPYVERTEX | COPYNORMAL | COPYUV | COPYCOLOR | COPYSPECULAR | COPYBLEND );
		if( ret1 ){
			DbgOut( "shdelem : CreateDPolyMesh2 : SetRevVertex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret1 = d3ddisp->Copy2VertexBuffer( 0 );
	if( ret1 ){
		DbgOut( "shdelem : CreateDPolyMesh2 : Copy2VertexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->Copy2IndexBufferNotCulling();
	if( ret1 ){
		DbgOut( "shdelem : CreateDPolyMesh2 : Copy2IndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret1 = polymesh2->ChkAlphaNum( lpsh->m_mathead );
	if( ret1 ){
		DbgOut( "selem : CreateDPolyMesh2 : pm2 ChkAlphaNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CShdElem::CreateDMorphPM2( CPolyMesh2* pm2, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	int ret1 = 0;

	if( !pm2 )
		return 1;

//	ret1 = pm2->CalcAnchorCenter();
//	if( ret1 ){
//		DbgOut( "se : CreateDMorphPM2 : pm2 : CalcAnchorCenter\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	d3ddisp = new CD3DDisp();
	if( !d3ddisp ){
		DbgOut( "CShdElem : :CreateDMorphPM2 : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret1 = d3ddisp->CreateDispDataMPM2( pd3dDevice, morph, tlmode );
	if( ret1 ){
		DbgOut( "CShdElem : :CreateDMorphPM2 : CreateDispData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	ret1 = pm2->CreateSortTexBuffer( lpsh->m_mathead );
//	if( ret1 ){
//		DbgOut( "se : :CreateDMorphPM2 : pm2 CreateSortTexBuffer error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	ret1 = d3ddisp->SetIndexBuffer( pm2->m_optindexbuftexm, morph->m_baseelem->clockwise );
	if( ret1 ){
		DbgOut( "selem : :CreateDMorphPM2 : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//	ret1 = SetUV( lpsh->m_mathead );
//	if( ret1 ){
//		DbgOut( "CShdElem : CreateDPolyMesh2 : SetUV error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	if( needcalcbe ){
		ret1 += CalcInfElem( lpsh );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDMorphPM2 : CreateInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( lpsh->m_shader != COL_TOON1 ){
		ret1 = d3ddisp->CreateMorphSkinMatTex( lpsh, pm2, pm2->m_texblock, pm2->m_texblocknum );
		if( ret1 ){
			DbgOut( "CShdElem : CreateDMorphPM2 : d3ddisp CreateSkinMat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( pm2->m_materialblock ){
		ret1 = d3ddisp->CreateMorphSkinMatToon1( lpsh, pm2, pm2->m_materialblock, pm2->m_materialnum );
		if( ret1 ){
			DbgOut( "shdelem : CreateDMorphPM2 : d3ddisp CreateSkinMatToon1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );

//	ret1 = InitColor( 0, vEyePt, lpsh->m_shader, lpsh->m_overflow, &(lpsh->m_mc), lpmh, tlmode );
//	if( ret1 ){
//		DbgOut( "CShdElem : CreateDPolyMesh2 : InitColor error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

//	if( polymesh2->groundflag ){
//		D3DXMATRIX inimat;
//		D3DXMatrixIdentity( &inimat );
//
//		lpsh->m_mc.InitCache( &inimat );
//
//		ret1 = d3ddisp->SetGroundObj( &(lpsh->m_mc), lpmh, inimat );
//		if( ret1 ){
//			DbgOut( "CShdElem : CreateDPolyMesh2 : SetGroundObj error !!!\n" );
//			_ASSERT( 0 );
//		}
//
//	}

//	if( morph->m_baseelem->clockwise == 3 ){
//		ret1 = d3ddisp->SetRevVertex( COPYVERTEX | COPYNORMAL | COPYUV | COPYCOLOR | COPYSPECULAR | COPYBLEND );
//		if( ret1 ){
//			DbgOut( "shdelem : CreateDPolyMesh2 : SetRevVertex error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}

//	ret1 = d3ddisp->Copy2VertexBuffer( 0 );
//	if( ret1 ){
//		DbgOut( "shdelem : CreateDPolyMesh2 : Copy2VertexBuffer error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	ret1 = d3ddisp->Copy2IndexBufferNotCulling();
	if( ret1 ){
		DbgOut( "shdelem : CreateDMorphPM2 : Copy2IndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
//	ret1 = pm2->ChkAlphaNum( lpsh->m_mathead );
//	if( ret1 ){
//		DbgOut( "selem : CreateDPolyMesh2 : pm2 ChkAlphaNum error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	return 0;
}

int CShdElem::CreateDGroundData( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{

	if( !grounddata )
		return 1;
	return 0;
}

int CShdElem::CreateDDummy( int* seri2boneno, LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, CShdHandler* lpsh, int tlmode, int needcalcbe )
{
	//DbgOut( "CShdTree : CreateDObjFunc called with invalid objtype %d : warning !!!\n", type );
	//return -1;

	DbgOut( "selem : CreateDDummy %d\n", type );
	return 0;
}

int CShdElem::GetKindNum( int objtype )
{
	int kindnum = 0;

	switch( objtype ){
	case SHDPOLYLINE:
	case SHDBEZIERSURF: // CBezMesh memberはCBezLine
	case SHDEXTRUDE:
		kindnum = 1;
		break;
	case SHDPOLYGON: // CPolygon
	case SHDSPHERE:	// CSphere
	case SHDMESHES: // CRevolved 
	case SHDDISK:	// CDisk
		kindnum = LIM2_MAX;
		break;
	case SHDBEZLINE: // CBezLine
		kindnum = LIM_MAX;
		break;
	default:
		kindnum = 0;
		break;
	}

	return kindnum;

}
CMeshInfo* CShdElem::AdjustMeshInfo( CMeshInfo* srcmeshinfo )
{
	// lex からの　meshinfo を　それぞれの3D obj用に調整
	int objtype, mnum, nnum, linenum, kindnum, mclosed, nclosed, hascolor, skip;

	int ret0, ret1, ret2, ret3, ret4, ret5, ret6, i;
	CMeshInfo* newmeshinfo = 0;
	CVec3f* tempmat = 0;

	objtype = srcmeshinfo->type;
	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	mclosed = srcmeshinfo->mclosed;
	nclosed = srcmeshinfo->nclosed;
	hascolor = srcmeshinfo->HasColor();
	skip = srcmeshinfo->skip;

	_ASSERT( (objtype > SHDTYPENONE) && (objtype < SHDTYPEMAX) );
	if( (objtype == SHDSPHERE) || (objtype == SHDDISK) ){
		// sphere, disk は　defmatで形状のみの指定なので、load時には MESHI_Nは　空。
		if( nnum == 0 )
			nnum = 1;
	}

	kindnum = GetKindNum( objtype );
	if( !kindnum ){
		DbgOut( "shddata.cpp : CShdTree : AdjustMeshInfo : kindnum error !!!\n" );
		return 0;		
	}

	linenum = nnum * kindnum;


	// CMeshInfo::NewMeshInfo は　メンバのチェックがあるので　ここでは使えない
	newmeshinfo = new CMeshInfo();
	if( !newmeshinfo )
	{
	DbgOut( "shddata.cpp : CShdTree : AdjustMeshInfo : newmeshinfo alloc error !!!\n" );
		return 0;
	}

	ret0 = newmeshinfo->SetMem( objtype, BASE_TYPE );
	ret1 = newmeshinfo->SetMem( kindnum, MESHI_M );
	ret2 = newmeshinfo->SetMem( nnum, MESHI_N );
	ret3 = newmeshinfo->SetMem( linenum, MESHI_TOTAL );
	ret4 = newmeshinfo->SetMem( mclosed, MESHI_MCLOSED );
	ret5 = newmeshinfo->SetMem( nclosed, MESHI_NCLOSED );
	ret6 = newmeshinfo->SetMem( hascolor, MESHI_HASCOLOR );
	ret6 += newmeshinfo->SetMem( skip, MESHI_SKIP );
	for( i = 0; i < 3; i++ ){
		tempmat = srcmeshinfo->GetMaterial( chkmaterial[i] );
		ret6 += newmeshinfo->SetMem( tempmat, chkmaterial[i] );
	}


	if( ret0 + ret1 + ret2 + ret3 + ret4 + ret5 + ret6 ){
		DbgOut( "shddata.cpp : CShdTree : AdjustMeshInfo : newmeshinfo->SetMem error !!!\n" );
		delete newmeshinfo;
		return 0;
	}

	return newmeshinfo;

}
CMeshInfo* CShdElem::CreateLineInfo( CMeshInfo* srcmeshinfo )
{
	// lex　からの　meshinfo から　lineinfo を作る
		// lineinfo には、hascolor をせっとしない。

	int mnum, nnum, objtype, i, j, mclosed, nclosed, skip;//, hascolor;
	int linenum, kindnum;
	int ret1, ret2, ret3, ret4, ret5, ret6;
	CMeshInfo* lineinfo = 0;
	CMeshInfo* curlineinfo = 0;


	objtype = srcmeshinfo->type;
	kindnum = GetKindNum( objtype );
	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	mclosed = srcmeshinfo->mclosed;
	nclosed = srcmeshinfo->nclosed;
	skip = srcmeshinfo->skip;
	//hascolor = srcmeshinfo->hascolor;

	linenum = kindnum * nnum;
	
	lineinfo = new CMeshInfo[ linenum ];
	if( !lineinfo ){
		DbgOut( "shddata.cpp : CShdTree : CreateLineInfo : lineinfo alloc error !!!\n" );
		return 0;
	}

	curlineinfo = lineinfo;
	for( i = 0; i < nnum; i++ ){
		for( j = 0; j < kindnum; j++ ){
			ret1 = curlineinfo->SetMem( SHDLINE, BASE_TYPE );
			ret2 = curlineinfo->SetMem( mnum, MESHI_M );
			ret3 = curlineinfo->SetMem( 1, MESHI_N );
			ret4 = curlineinfo->SetMem( mnum, MESHI_TOTAL );
			ret5 = curlineinfo->SetMem( mclosed, MESHI_MCLOSED );
			ret6 = curlineinfo->SetMem( nclosed, MESHI_NCLOSED );
			//ret7 = curlineinfo->SetMem( hascolor, MESHI_HASCOLOR );
			ret6 += curlineinfo->SetMem( skip, MESHI_SKIP );

			if( ret1 || ret2 || ret3 || ret4 || ret5 || ret6 ){
				DbgOut( "shddata.cpp : CShdTree : CreateLineInfo : lineinfo->SetMem error !!!\n" );
				return 0;
			}
			curlineinfo++;
		}
	}

	return lineinfo;

}

CMeshInfo* CShdElem::GetMeshInfo()
{
	CMeshInfo* retmi = 0;

		switch( type ){

		//case SHDROOT:
		case SHDROTATE:
		case SHDSLIDER:
		case SHDSCALE:
		case SHDUNISCALE:
		case SHDPATH:		
		case SHDPART:
		case SHDBALLJOINT:
			_ASSERT( part );
			retmi = part->meshinfo;
			break;

		case SHDPOLYLINE:
			_ASSERT( vline );
			retmi = vline->meshinfo;
			break;
		case SHDPOLYGON:
			_ASSERT( polygon );
			retmi = polygon->meshinfo;
			break;
		case SHDSPHERE:
			_ASSERT( sphere );
			retmi = sphere->meshinfo;
			break;
		case SHDBEZIERSURF:
			_ASSERT( bmesh );
			retmi = bmesh->meshinfo;
			break;
		case SHDMESHES:
			_ASSERT( revolved );
			retmi = revolved->meshinfo;
			break;
		case SHDDISK:
			_ASSERT( disk );
			retmi = disk->meshinfo;
			break;
		case SHDBEZLINE:	// CBezLine
			_ASSERT( bline );
			retmi = bline->meshinfo;
			break;
		case SHDEXTRUDE:
			_ASSERT( extrude );
			retmi = extrude->meshinfo;
			break;
		case SHDPOLYMESH:
			_ASSERT( polymesh );
			retmi = polymesh->meshinfo;
			break;
		case SHDPOLYMESH2:
			_ASSERT( polymesh2 );
			retmi = polymesh2->meshinfo;
			break;
		case SHDGROUNDDATA:
			_ASSERT( grounddata );
			retmi = grounddata->meshinfo;
			break;
		case SHDBILLBOARD:
			_ASSERT( billboard );
			retmi = billboard->meshinfo;
			break;
		case SHDEXTLINE:
			_ASSERT( extline );
			retmi = extline->meshinfo;
			break;
		case SHDTYPENONE:
		case SHDTYPEERROR:
		case SHDTYPEMAX:
			// error
			DbgOut( "CShdElem : DumpObj : type error %d !!!", type );
			_ASSERT( 0 );
			retmi = 0;
			break;

		case SHDDESTROYED:
		case SHDINFSCOPE:
		case SHDBBOX:
		case SHDMORPH:
			m_dummymeshinfo.type = type;
			retmi = &m_dummymeshinfo;
			break;

		default:
			// 未対応
			_ASSERT( 0 );
			//retmi = 0;
			m_dummymeshinfo.type = type;
			retmi = &m_dummymeshinfo;
			break;
	}

	return retmi;
}


int CShdElem::GetBoneOrgPos( float* dstx, float* dsty, float* dstz )
{
	if( (IsJoint() && (type != SHDMORPH)) && (notuse != 1) ){
		if( !d3ddisp  ){
			_ASSERT( 0 );
			return 1;
		}

		SKINVERTEX* skinv;
		skinv = d3ddisp->m_skinv;

		*dstx = skinv->pos[0];
		*dsty = skinv->pos[1];
		*dstz = skinv->pos[2];


		return 0;
	}else{
		return 1;
	}

}

int CShdElem::UpdateVertexBuffer( int framecnt, int tlmode, int srcshader )
{
	int ret = 0;

	if( (type != SHDINFSCOPE) && (type != SHDBBOX) && (type != SHDMORPH) ){
		if( !d3ddisp ){
			_ASSERT( 0 );
			return 1;
		}

		//if( tlmode == TLMODE_ORG ){
			ret = d3ddisp->Copy2VertexBuffer( framecnt );
			_ASSERT( !ret );
			ret = d3ddisp->Copy2IndexBufferNotCulling();
			_ASSERT( !ret );

		//}
		return ret;
	}else if( type == SHDINFSCOPE ){
		CInfScope* curis;
		int isno;
		for( isno = 0; isno < scopenum; isno++ ){
			curis = *( ppscope + isno );
			_ASSERT( curis );
			_ASSERT( curis->d3ddisp );
			
			if( curis->dispflag != 0 ){
				ret = curis->d3ddisp->Copy2VertexBuffer( framecnt );
				_ASSERT( !ret );

				ret = curis->d3ddisp->Copy2IndexBufferNotCulling();
				_ASSERT( !ret );
			}
		}
		return ret;
	}else if( type == SHDBBOX ){
		CBBox2* curbbx;
		int bbxno;
		for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
			curbbx = *( ppbbox + bbxno );
			_ASSERT( curbbx );
			_ASSERT( curbbx->d3ddisp );

			if( curbbx->dispflag != 0 ){
				ret = curbbx->d3ddisp->Copy2VertexBuffer( framecnt );
				_ASSERT( !ret );

				ret = curbbx->d3ddisp->Copy2IndexBufferNotCulling();
				_ASSERT( !ret );
			}
		}
		return ret;

	}else if( type == SHDMORPH ){
		if( morph->m_objtype == SHDPOLYMESH ){
			ret = d3ddisp->Copy2VertexBuffer( framecnt );
			_ASSERT( !ret );
		}else if( morph->m_objtype == SHDPOLYMESH2 ){
			ret = d3ddisp->Copy2VertexBufferMorph( morph->m_baseelem->polymesh2, 1 );
			_ASSERT( !ret );

		}
		ret = d3ddisp->Copy2IndexBufferNotCulling();
		_ASSERT( !ret );

		return 0;
	}else{
		_ASSERT( 0 );
		return 0;
	}
}


int CShdElem::TransformDispData( int* seri2boneno, D3DXVECTOR3 vEyePt, D3DXMATRIX* firstscale, 
	CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, int lightflag )
{
	int ret = 0;

	m_lightflag = lightflag;

	m_tempscalemat = firstscale;//!!!!!!!

	CInfScope* curis;
	int isno;

	switch( type ){
	case SHDBEZIERSURF:
		//ret = TransformDBMesh( 1, mcache, dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, gband, aspect, bonearray );
		break;
	case SHDMORPH:
		ret = TransformDMorph( &vEyePt, lpmh, matWorld, matView, matProj );
		break;
	case SHDMESHES:
		//ret = TransformDMeshes( mcache, dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, gband, aspect, bonearray );
		break;
	case SHDSPHERE:
		//ret = TransformDSphere( mcache, dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, gband, aspect, bonearray );
		break;
	case SHDEXTRUDE:
		//ret = TransformDExtrude( mcache, dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, gband, aspect, bonearray );
		break;
	case SHDPOLYGON:
		//ret = TransformDPolygon( mcache, dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, gband, aspect, bonearray );
		break;
	case SHDPOLYMESH:
		ret = TransformDPolyMesh( seri2boneno, firstscale, &vEyePt, lpmh, matWorld, matView, matProj );
		break;
	case SHDPOLYMESH2:
		ret = TransformDPolyMesh2( seri2boneno, firstscale, &vEyePt, lpmh, matWorld, matView, matProj );
		break;
	case SHDINFSCOPE:

		for( isno = 0; isno < scopenum; isno++ ){
			curis = *( ppscope + isno );
			_ASSERT( curis );
			_ASSERT( curis->d3ddisp );

			if( curis->dispflag ){
				switch( curis->type ){
				case SHDPOLYMESH:

					
					ret = curis->d3ddisp->TransformSkinMat( serialno, seri2boneno, firstscale, lpmh, matWorld, matView, matProj, &vEyePt );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}
					
					break;
				case SHDPOLYMESH2:
					
					ret = curis->d3ddisp->TransformSkinMat( serialno, seri2boneno, firstscale, lpmh, matWorld, matView, matProj, &vEyePt );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}
					

					break;
				default:
					break;

				}
			}
		}

		break;
	
	case SHDBBOX:

		CBBox2* curbbx;
		int bbxno;
		for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
			curbbx = *( ppbbox + bbxno );
			_ASSERT( curbbx );
			_ASSERT( curbbx->d3ddisp );

			if( curbbx->dispflag ){	
				
				if( curbbx->target && curbbx->target->d3ddisp ){
					curbbx->d3ddisp->m_scalemat = curbbx->target->d3ddisp->m_scalemat;
				}

				ret = curbbx->d3ddisp->TransformSkinMat( serialno, seri2boneno, firstscale, lpmh, matWorld, matView, matProj, &vEyePt );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
				
			}


			ret = curbbx->TransformOnlyWorld( lpmh, *matWorld );
			if( ret ){
				DbgOut( "se : TransformDispData : bbx Transform error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

		break;

	case SHDGROUNDDATA:
		//ret = TransformDGroundData( mcache, dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, gband, aspect, bonearray );
		break;
	case SHDBILLBOARD:


		//TransformDBillboard は、shdhandlerから、直接呼ばれる。他のものより、引数が多い。
		// ここでは、何もしない。


		//ret = TransformDBillboard( dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, gband, aspect, bonearray );
		break;

	case SHDEXTLINE:

		/***
		_ASSERT( d3ddisp );
		d3ddisp->m_matView = matView;
		d3ddisp->m_matProj = matProj;
		***/

		break;
	case SHDDESTROYED:
		break;

	default:
		//if( IsJoint() && (type != SHDMORPH) ){
		//	ret = TransformDPart( seri2boneno, mcache, dwClipWidth, dwClipHeight, lpmh, matWorld, matView, matProj, projmode, lightflag, aspect, bonearray );
		//}else{
		//	ret = 1;
		//}
		break;
	}
	return ret;
}

int CShdElem::InitColor( int* seri2boneno, D3DXVECTOR3 vEyePt, int srcshader,  int srcoverflow, CMCache* mcache, CMotHandler* lpmh, int tlmode )
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat );

	GUARDBAND dummyband;
	dummyband.left = -2048.0f;
	dummyband.top = -2048.0f;
	dummyband.right = 2047.0f;
	dummyband.bottom = 2047.0f;

	D3DXMATRIX firstscale;
	D3DXMatrixIdentity( &firstscale );

	int ret = 0;


	CInfScope* curis;
	int isno;

	CBBox2* curbbx;
	int bbxno;

	switch( type ){
	case SHDBEZIERSURF:
		//ret += d3ddisp->InitColor( bmesh, alpha, mat );
		//_ASSERT( !ret );
		break;
	case SHDMORPH:

		break;
	case SHDMESHES:
		//ret += d3ddisp->InitColor( revolved, alpha, mat );
		//_ASSERT( !ret );
		break;
	case SHDSPHERE:
		//ret += d3ddisp->InitColor( sphere, alpha, mat );
		//_ASSERT( !ret );
		break;
	case SHDEXTRUDE:
		//ret = d3ddisp->InitColor( extrude, alpha, mat );
		//_ASSERT( !ret );
		break;
	case SHDPOLYGON:
		//ret += d3ddisp->InitColor( polygon, alpha, mat );
		//_ASSERT( !ret );
		break;
	case SHDPOLYMESH:
		ret += d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh, alpha, mat );
		_ASSERT( !ret );
		break;
	case SHDPOLYMESH2:
		ret += d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh2, alpha, mat );
		_ASSERT( !ret );
		break;
	case SHDINFSCOPE:

		for( isno = 0; isno < scopenum; isno++ ){
			curis = *( ppscope + isno );
			_ASSERT( curis );
			_ASSERT( curis->d3ddisp );

			switch( curis->type ){
			case SHDPOLYMESH:
				ret = curis->d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, curis->polymesh, alpha, mat );
				_ASSERT( !ret );
				break;
			case SHDPOLYMESH2:
				ret = curis->d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, curis->polymesh2, alpha, mat );
				_ASSERT( !ret );
				break;
			default:
				break;
			}
		}

		break;

	case SHDBBOX:
		for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
			curbbx = *( ppbbox + bbxno );
			_ASSERT( curbbx );
			_ASSERT( curbbx->d3ddisp );

			ret = curbbx->d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, curbbx->polymesh2, alpha, mat );
			_ASSERT( !ret );
		}
		break;

	case SHDEXTLINE:
		ret += d3ddisp->InitColor( extline, alpha, mat );
		_ASSERT( !ret );
		break;
	default:
		break;
	}

	return ret;
}


int CShdElem::TransformDExtLine( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, DWORD curdispswitch, int* bonearray )
{
	// TLMODE_D3Dのみなので、なにもしない。

	return 0;
}

int CShdElem::TransformDPolyMesh( int* seri2boneno, D3DXMATRIX* firstscale, D3DXVECTOR3* vEyePt, 
	CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj )
{
	int ret;

	if( !polymesh || !d3ddisp ){
		DbgOut( "CShdElem : TransformDPolyMesh : error %x %x!!!\n", polymesh, d3ddisp );
		_ASSERT( 0 );
		return 1;
	}
	

	if( polymesh->billboardflag == 1 ){
		//polymeshでは、処理しない。
		//SHDBILLBOARDから、呼ばれて、処理する。
		return 0;
	}

	
	ret = d3ddisp->TransformSkinMat( serialno, seri2boneno, firstscale, lpmh, matWorld, matView, matProj, vEyePt );
	if( ret ){
		DbgOut( "shdelem : TransformDPolyMesh : d3ddisp TransformSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;

}

int CShdElem::TransformDPolyMesh2( int* seri2boneno, D3DXMATRIX* firstscale, D3DXVECTOR3* vEyePt, 
	CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj )
{
	int ret;

	if( !polymesh2 || !d3ddisp ){
		DbgOut( "CShdElem : TransformDPolyMesh2 : error %x %x!!!\n", polymesh2, d3ddisp );
		_ASSERT( 0 );
		return 1;
	}
	

	
	ret = d3ddisp->TransformSkinMat( serialno, seri2boneno, firstscale, lpmh, matWorld, matView, matProj, vEyePt );
	if( ret ){
		DbgOut( "selem : TransformDPolyMesh2 : d3ddisp TransformSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	


	return 0;

}

int CShdElem::TransformDGroundData( CMCache* mcache, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int projmode, int lightflag, GUARDBAND gband, float aspect, int* bonearray )
{
	return 0;
}

int CShdElem::RenderBone( LPD3DXSPRITE pd3dxsprite, LPDIRECT3DTEXTURE9 ptexture, float jointsize, int jointalpha )
{
	int ret;

	if( !d3ddisp )
		return 1;

	ret = d3ddisp->RenderBone( pd3dxsprite, ptexture, isselected, jointsize, jointalpha );
	_ASSERT( !ret );

	return ret;
}


int CShdElem::RenderShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh )
{

	int ret = 0;

	if( (type != SHDBILLBOARD) && (type != SHDINFSCOPE) && (type != SHDBBOX) && (type != SHDMORPH) && !d3ddisp )
		return 1;
	
//	float fogparams[2];
//	fogparams[0] = *((float*)(&m_renderstate[D3DRS_FOGSTART]));
//	fogparams[1] = *((float*)(&m_renderstate[D3DRS_FOGEND]));


	if( type == SHDPOLYMESH ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh );
		if( polymesh->billboardflag == 0 ){

			ret = d3ddisp->RenderPhongShadowMap0( pd3dDevice, lpsh, m_enabletexture );
			_ASSERT( !ret );
		}

		
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh2 );
		//ret = pd3dDevice->SetMaterial( &(polymesh2->m_material) );//!!!!!!
		//ret = d3ddisp->RenderTri( pd3dDevice, lpsh, m_toonparams, lptex, fogparams, polymesh2->m_toon1, m_lightflag );
		//_ASSERT( !ret );

		ret = d3ddisp->RenderPhongShadowMap0( pd3dDevice, lpsh, m_enabletexture );
		_ASSERT( !ret );
	}else if( type == SHDMORPH ){
		_ASSERT( d3ddisp );
		_ASSERT( morph );

		ret = d3ddisp->RenderPhongShadowMap0( pd3dDevice, lpsh, m_enabletexture );
		_ASSERT( !ret );
	}

	if( ret )
		return 1;

	return 0;
}

int CShdElem::RenderShadowMap1( int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh )

{
	int ret = 0;

	if( (type != SHDBILLBOARD) && (type != SHDINFSCOPE) && (type != SHDBBOX) && (type != SHDMORPH) && !d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}
	
	float fogparams[3];
	fogparams[0] = *((float*)(&m_renderstate[D3DRS_FOGSTART]));
	fogparams[1] = *((float*)(&m_renderstate[D3DRS_FOGEND]));
	fogparams[2] = g_fogtype;

	if( type == SHDPOLYMESH ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh );
		if( polymesh->billboardflag == 0 ){
			if( lpsh->m_shader != COL_TOON1 ){
				ret = d3ddisp->RenderPhongShadowMap1( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, m_toonparams, fogparams, m_enabletexture, &(polymesh->chkalpha) );
				_ASSERT( !ret );
			}else{
				ret = d3ddisp->RenderToon1ShadowMap1( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, fogparams, polymesh->m_toon1, m_lightflag,
					m_enabletexture, &(polymesh->chkalpha) );
				_ASSERT( !ret );
			}
		}
		
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh2 );
		//ret = pd3dDevice->SetMaterial( &(polymesh2->m_material) );//!!!!!!
		//ret = d3ddisp->RenderTri( pd3dDevice, lpsh, m_toonparams, lptex, fogparams, polymesh2->m_toon1, m_lightflag );
		//_ASSERT( !ret );
		if( lpsh->m_shader != COL_TOON1 ){
			ret = d3ddisp->RenderPhongShadowMap1( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, m_toonparams, fogparams, m_enabletexture, &(polymesh2->chkalpha) );
			_ASSERT( !ret );
		}else{
			ret = d3ddisp->RenderToon1ShadowMap1( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, fogparams, polymesh2->m_toon1, m_lightflag,
				m_enabletexture, &(polymesh2->chkalpha) );
			_ASSERT( !ret );
		}
	}else if( type == SHDMORPH ){
		_ASSERT( d3ddisp );
		_ASSERT( morph );
		_ASSERT( morph );
		CShdElem* baseelem = morph->m_baseelem;
		_ASSERT( baseelem );
		if( morph->m_objtype == SHDPOLYMESH2 ){
			CPolyMesh2* pm2 = baseelem->polymesh2;
			_ASSERT( pm2 );

			if( lpsh->m_shader != COL_TOON1 ){

				ret = d3ddisp->RenderPhongShadowMap1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams,
					baseelem->m_enabletexture, &(pm2->chkalpha) );
				_ASSERT( !ret );

				//_ASSERT( 0 );

			}else{
				ret = d3ddisp->RenderToon1ShadowMap1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, fogparams, pm2->m_toon1, 
					baseelem->m_lightflag, baseelem->m_enabletexture, &(pm2->chkalpha) );
				_ASSERT( !ret );
			}
			
		}else if( morph->m_objtype == SHDPOLYMESH ){
			CPolyMesh* pm = baseelem->polymesh;
			_ASSERT( pm );

			if( lpsh->m_shader != COL_TOON1 ){
				ret = d3ddisp->RenderPhongShadowMap1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams,
					baseelem->m_enabletexture, &(pm->chkalpha) );
				_ASSERT( !ret );

			}else{
				ret = d3ddisp->RenderToon1ShadowMap1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, fogparams, pm->m_toon1, 
					baseelem->m_lightflag, baseelem->m_enabletexture, &(pm->chkalpha) );
				_ASSERT( !ret );
			}

		}

	}else if( type == SHDEXTLINE ){
		
		_ASSERT( d3ddisp );
		ret = d3ddisp->RenderLine( pd3dDevice, withalpha, alpha );
		_ASSERT( !ret );
	}

	if( ret )
		return 1;

	return 0;
}

int CShdElem::Render( int znflag, int withalpha, LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, int glowflag )

{
	int ret = 0;

	if( (type != SHDBILLBOARD) && (type != SHDINFSCOPE) && (type != SHDBBOX) && (type != SHDMORPH) && !d3ddisp )
		return 1;
	
	float fogparams[3];
	fogparams[0] = *((float*)(&m_renderstate[D3DRS_FOGSTART]));
	fogparams[1] = *((float*)(&m_renderstate[D3DRS_FOGEND]));
	fogparams[2] = g_fogtype;

	if( type == SHDPOLYMESH ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh );
		if( polymesh->billboardflag == 0 ){

			if( (znflag == 0) || (znflag == 104) ){
				if( lpsh->m_shader != COL_TOON1 ){
					ret = d3ddisp->RenderTriPhong( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, m_toonparams, fogparams, m_lightflag,
						m_enabletexture, &(polymesh->chkalpha), m_mbluralpha, 0, glowflag );
					_ASSERT( !ret );

				}else{
					ret = d3ddisp->RenderTriToon1( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, m_toonparams, fogparams, polymesh->m_toon1, m_lightflag,
						m_enabletexture, &(polymesh->chkalpha), m_mbluralpha, 0, glowflag );
					_ASSERT( !ret );
				}
			}else if( znflag >= 100 ){
				ret = d3ddisp->RenderZNPhong( znflag, alpha, setalphaflag, withalpha, pd3dDevice, 
					&(polymesh->chkalpha) );
				_ASSERT( !ret );
			}else{
				if( lpsh->m_shader != COL_TOON1 ){
					ret = d3ddisp->RenderZNPhong( znflag, alpha, setalphaflag, withalpha, pd3dDevice, 
						&(polymesh->chkalpha) );
					_ASSERT( !ret );

				}else{
					ret = d3ddisp->RenderZNToon1( alpha, setalphaflag, withalpha, pd3dDevice, 
						&(polymesh->chkalpha) );
					_ASSERT( !ret );
				}
			}
		}

		_ASSERT( !ret );
		
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh2 );
		if( (znflag == 0) || (znflag == 104) ){
			if( lpsh->m_shader != COL_TOON1 ){

				ret = d3ddisp->RenderTriPhong( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, m_toonparams, fogparams, m_lightflag,
					m_enabletexture, &(polymesh2->chkalpha), m_mbluralpha, 0, glowflag );
				_ASSERT( !ret );

			}else{
				ret = d3ddisp->RenderTriToon1( alpha, setalphaflag, withalpha, pd3dDevice, lpsh, m_toonparams, fogparams, polymesh2->m_toon1, m_lightflag,
					m_enabletexture, &(polymesh2->chkalpha), m_mbluralpha, 0, glowflag );
				_ASSERT( !ret );
			}
		}else if( znflag >= 100 ){
			ret = d3ddisp->RenderZNPhong( znflag, alpha, setalphaflag, withalpha, pd3dDevice, 
				&(polymesh2->chkalpha) );
			_ASSERT( !ret );

		}else{
//_ASSERT( 0 );
			if( lpsh->m_shader != COL_TOON1 ){
				ret = d3ddisp->RenderZNPhong( znflag, alpha, setalphaflag, withalpha, pd3dDevice, 
					&(polymesh2->chkalpha) );
				_ASSERT( !ret );

			}else{
				ret = d3ddisp->RenderZNToon1( alpha, setalphaflag, withalpha, pd3dDevice, 
					&(polymesh2->chkalpha) );
				_ASSERT( !ret );
			}

		}
	}else if( type == SHDMORPH ){
		_ASSERT( morph );
		CShdElem* baseelem = morph->m_baseelem;
		_ASSERT( baseelem );
		if( morph->m_objtype == SHDPOLYMESH2 ){
			CPolyMesh2* pm2 = baseelem->polymesh2;
			_ASSERT( pm2 );

			if( (znflag == 0) || (znflag == 104) ){
				if( lpsh->m_shader != COL_TOON1 ){
					ret = d3ddisp->RenderTriPhong( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, baseelem->m_lightflag,
						baseelem->m_enabletexture, &(pm2->chkalpha), m_mbluralpha, 0, glowflag );
					_ASSERT( !ret );

					//_ASSERT( 0 );

				}else{
					ret = d3ddisp->RenderTriToon1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, pm2->m_toon1, 
						baseelem->m_lightflag, baseelem->m_enabletexture, &(pm2->chkalpha), m_mbluralpha, 0, glowflag );
					_ASSERT( !ret );
				}
			}else if( znflag >= 100 ){
				ret = d3ddisp->RenderZNPhong( znflag, baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, 
					&(baseelem->polymesh2->chkalpha) );
				_ASSERT( !ret );

			}else{
				if( lpsh->m_shader != COL_TOON1 ){
					ret = d3ddisp->RenderZNPhong( znflag, baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, 
						&(baseelem->polymesh2->chkalpha) );
					_ASSERT( !ret );

				}else{
					ret = d3ddisp->RenderZNToon1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, 
						&(baseelem->polymesh2->chkalpha) );
					_ASSERT( !ret );
				}
			}
			
		}else if( morph->m_objtype == SHDPOLYMESH ){
			CPolyMesh* pm = baseelem->polymesh;
			_ASSERT( pm );

			if( (znflag == 0) || (znflag == 104) ){
				if( lpsh->m_shader != COL_TOON1 ){
					ret = d3ddisp->RenderTriPhong( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, baseelem->m_lightflag,
						baseelem->m_enabletexture, &(pm->chkalpha), m_mbluralpha, 0, glowflag );
					_ASSERT( !ret );

				}else{
					ret = d3ddisp->RenderTriToon1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, pm->m_toon1, 
						baseelem->m_lightflag, baseelem->m_enabletexture, &(pm->chkalpha), m_mbluralpha, 0, glowflag );
					_ASSERT( !ret );
				}
			}else if( znflag >= 100 ){
				ret = d3ddisp->RenderZNPhong( znflag, baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, 
					&(baseelem->polymesh->chkalpha) );
				_ASSERT( !ret );

			}else{
				if( lpsh->m_shader != COL_TOON1 ){
					ret = d3ddisp->RenderZNPhong( znflag, baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, 
						&(baseelem->polymesh->chkalpha) );
					_ASSERT( !ret );

				}else{
					ret = d3ddisp->RenderZNToon1( baseelem->alpha, baseelem->setalphaflag, withalpha, pd3dDevice, 
						&(baseelem->polymesh->chkalpha) );
					_ASSERT( !ret );
				}
			}

		}
	}else if( type == SHDINFSCOPE ){

		if( withalpha == 0 ){
			return 0;//!!!!!!!!!!
		}

		if( g_curtex0 != NULL ){
			pd3dDevice->SetTexture( 0, NULL );
			g_curtex0 = NULL;
		}
		if( g_curtex1 != NULL ){
			pd3dDevice->SetTexture( 1, NULL );
			g_curtex1 = NULL;
		}
		if( g_cop0 != D3DTOP_MODULATE ){
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_cop0 = D3DTOP_MODULATE;
		}
		if( g_cop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
			g_cop1 = D3DTOP_DISABLE;
		}
		if( g_aop0 != D3DTOP_MODULATE ){
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			g_aop0 = D3DTOP_MODULATE;
		}
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		if( g_aop1 != D3DTOP_DISABLE ){
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
			g_aop1 = D3DTOP_DISABLE;
		}

		CInfScope* curis;
		int isno;
		for( isno = 0; isno < scopenum; isno++ ){
			curis = *( ppscope + isno );
			_ASSERT( curis );
			_ASSERT( curis->d3ddisp );
			if( curis->dispflag ){
				//ret = curis->d3ddisp->RenderTri( pd3dDevice, lpsh, m_toonparams, lptex, fogparams, 0 );


				// infscopeは、toonでは表示しない！！！！！！！
				curis->d3ddisp->m_alpha = 0.5f;
				ret = curis->d3ddisp->RenderTri( pd3dDevice, lpsh, 0, 0, fogparams, 0, m_lightflag, m_mbluralpha, 0, NOGLOW );


				_ASSERT( !ret );
			}
		}

//unsigned char KeyBuffer[ 256 ];
//GetKeyboardState( KeyBuffer );
//if( KeyBuffer['1'] & 0x80 ){
//	::MessageBox( NULL, "debug print", "debug", MB_OK );
//	DbgOut( "check!!!: d3ddisp : scopenum %d\r\n", scopenum );
//	for( isno = 0; isno < scopenum; isno++ ){
//		curis = *( ppscope + isno );
//		DbgOut( "check!!!: d3ddisp : isno %d, dispflag %d\r\n", isno, curis->dispflag );
//	}
//}


	}else if( type == SHDBBOX ){


		CBBox2* curbbx;
		int bbxno;
		for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
			curbbx = *( ppbbox + bbxno );
			_ASSERT( curbbx );
			_ASSERT( curbbx->d3ddisp );

			if( curbbx->dispflag ){
				ret = curbbx->d3ddisp->RenderTri( pd3dDevice, lpsh, 0, 0, fogparams, 0, m_lightflag, m_mbluralpha, 0, NOGLOW );
				_ASSERT( !ret );
			}
		}



	}else if( type == SHDPOLYGON ){
		//ret = d3ddisp->RenderFan( pd3dDevice );
		//_ASSERT( !ret );

	}else if( type == SHDGROUNDDATA ){

		//今のところ、何もしない
	}else if( type == SHDBILLBOARD ){
		
		//RenderBillboard は、shdhandlerから、直接呼ばれる。
		// ここでは、何もしない。


		//ret = RenderBillboard( pd3dDevice );
		//if( ret ){
		//	DbgOut( "shdelem : Render : RenderBillboard error !!!\n" );
		//	_ASSERT( 0 );
		//}
	}else if( type == SHDEXTLINE ){
		
		if( glowflag == 0 ){
			_ASSERT( d3ddisp );
			ret = d3ddisp->RenderLine( pd3dDevice, withalpha, alpha );
			_ASSERT( !ret );
		}
		
	}else if( type == SHDDESTROYED ){
		//何もしない

	}else{
		//ret = d3ddisp->Render( pd3dDevice );
		//_ASSERT( !ret );
	}

	if( ret )
		return 1;

	return 0;
}

int CShdElem::Render( LPDIRECT3DDEVICE9 pd3dDevice, int faceno )
{
	int ret = 0;

	if( !d3ddisp && (type != SHDBILLBOARD) )
		return 1;
	
	/***
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		if( polymesh->billboardflag == 0 )
			//ret = pd3dDevice->SetMaterial( &(polymesh->m_material) );//!!!!!!
			ret = d3ddisp->RenderTri( pd3dDevice, faceno );

		_ASSERT( !ret );
		
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		//ret = pd3dDevice->SetMaterial( &(polymesh2->m_material) );//!!!!!!
		ret = d3ddisp->RenderTri( pd3dDevice, faceno );
		_ASSERT( !ret );

	}

	if( ret )
		return 1;
	***/

	return 0;
}

CBezMesh* CShdElem::BezDiv( CBezMesh* srcbmesh )
{
	CBezMesh* newbm;
	CMeshInfo dstinfo;
	CMeshInfo* srcbminfo;
	CMeshInfo* srcvlinfo;
	CBezLine* bl;
	CBezLine* newbl;
	CVecLine* vl;
	int ret, linenum, pointnum, mclosed, nclosed;
	int newlinenum, newpointnum;
	int lno, pno, beflno, aftlno;
	int lstep, pstep;

	srcbminfo = srcbmesh->meshinfo;
	linenum = srcbminfo->n;
	mclosed = srcbminfo->mclosed;
	nclosed = srcbminfo->nclosed;

	bl = (*srcbmesh)( 0 );
	vl = (*bl)( 0 );
	srcvlinfo = vl->meshinfo;
	pointnum = srcvlinfo->m;

	_ASSERT( nclosed == 0 );

	if( bdivV ){
		newlinenum = linenum * 2 - 1;
		lstep = 2;
	}else{
		newlinenum = linenum;
		lstep = 1;
	}

	if( bdivU ){
		if( mclosed == 0 ){
			newpointnum = pointnum * 2 - 1;
		}else{
			newpointnum = pointnum * 2;
		}
		pstep = 2;
	}else{
		newpointnum = pointnum;
		pstep = 1;
	}

	ret = dstinfo.CopyData( srcbminfo );
	_ASSERT( !ret );
	dstinfo.n = newlinenum;
	dstinfo.m = 1;
	dstinfo.total = newlinenum;

	newbm = NewBMesh( &dstinfo );
	if( !newbm ){
		DbgOut( "CShdElem : BezDiv : NewBMesh error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	dstinfo.m = newpointnum;
	dstinfo.n = 1;
	dstinfo.total = newpointnum;
	dstinfo.type = SHDBEZLINE;
	for( lno = 0; lno < newlinenum; lno++ ){
		newbl = NewBLine( &dstinfo );		
		if( !newbl ){
			DbgOut( "CShdElem : BezDiv : NewBLine error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}
		ret = newbm->SetMem( newbl, lno, 0, SHDBEZLINE );// blseriはダミー。　bm->hblseriは使えない。
		_ASSERT( !ret );

	}


	ret = newbm->SetEvenData( srcbmesh, bdivU, bdivV );
	_ASSERT( !ret );

	if( bdivU ){
		for( lno = 0; lno < newlinenum; lno+= lstep ){
			for( pno = 1; pno < newpointnum - 1; pno+= pstep ){
				ret = newbm->CalcBezDiv_U( lno, pno, pno - 1, pno + 1 );
				if( ret ){
					_ASSERT( 0 );
					return 0;
				}
			}
			if( mclosed == 1 ){
				ret = newbm->CalcBezDiv_U( lno, newpointnum - 1, newpointnum - 2, 0 );
				if( ret ){
					_ASSERT( 0 );
					return 0;
				}
			}
		}
	}

	if( bdivV ){
		if( bdivU ){
			for( lno = 0; lno < newlinenum; lno+= lstep ){
				if( lno == 0 ){
					beflno = lno;
					aftlno = lno + 2;
				}else if( lno == (newlinenum - 1) ){
					beflno = lno - 2;
					aftlno = lno;
				}else{
					beflno = lno - 2;
					aftlno = lno + 2;
				}
				for( pno = 1; pno < newpointnum - 1; pno+= pstep ){
					ret = newbm->CalcNewpLat( lno, beflno, aftlno, pno );
					if( ret ){
						_ASSERT( 0 );
						return 0;
					}
				}
				if( mclosed == 1 ){
					ret = newbm->CalcNewpLat( lno, beflno, aftlno, newpointnum - 1 );
					if( ret ){
						_ASSERT( 0 );
						return 0;
					}
				}
			}
		}

		for( lno = 1; lno < newlinenum - 1; lno+= lstep ){
			for( pno = 0; pno < newpointnum; pno++ ){
				ret = newbm->CalcBezDiv_V( pno, lno, lno - 1, lno + 1 );
				if( ret ){
					_ASSERT( 0 );
					return 0;
				}
			}
		}
	}

	return newbm;

}

int CShdElem::MoveTexUV( int tlmode )
{
	int ret;

	if( (type != SHDINFSCOPE) && (type != SHDBBOX) && (type != SHDMORPH) ){
		if( !d3ddisp )
			return 1;

		ret = d3ddisp->MoveTexUV( uanime, vanime, type );
		if( ret ){
			DbgOut( "shdelem : MoveTexUV : d3ddisp MoveTexUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//if( (tlmode == TLMODE_D3D) && (clockwise == 3) ){
		if( clockwise == 3 ){
			ret = d3ddisp->SetRevVertex( COPYUV );
			if( ret ){
				DbgOut( "shdelem : MoveTexUV : d3ddisp : SetRevVertex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		if( g_useGPU ){
			ret = d3ddisp->Copy2VertexBuffer( 0 );
			if( ret ){
				DbgOut( "shdelem : MoveTexUV : d3ddisp : Copy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}
	return 0;
}

int CShdElem::MoveMaterialUV( CMQOMaterial* curmat )
{
	int ret;
	int* dirtyptr = 0;
	if( type == SHDPOLYMESH ){
		dirtyptr = polymesh->GetDirtyMaterial( curmat->materialno );
	}else if( type == SHDPOLYMESH2 ){
		dirtyptr = polymesh2->GetDirtyMaterial( curmat->materialno );
	}

	if( d3ddisp && dirtyptr ){
		ret = d3ddisp->MoveUVArray( curmat->texanimtype, dirtyptr, curmat->uanime, curmat->vanime, type );
		if( ret ){
			DbgOut( "se : MoveMaterialUV : d3ddisp MoveUVArray error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CShdElem::SetInvFace( int srcif )
{
	invface = srcif;
	//clockwise の初期化
	clockwise = 2;
	return 0;
}
int CShdElem::AddInvFaceCnt( int srccnt )
{
	invfacecnt += srccnt;
	return 0;
}
int CShdElem::CalcClockwise()
{
	int rest;

	if( clockwise != 3 ){
		rest = invfacecnt % 2;
		if( rest == 0 ){
			clockwise = 2;
		}else{
			clockwise = 1;
		}
	}

	return 0;
}

//int CShdElem::LeaveFromChain( int delchildflag )
int CShdElem::LeaveFromChain()
{
	//childのchainは保ったまま、他のリンクから、はずす。

	CShdElem *saveparent, *savechild, *savebrother, *savesister;

	saveparent = parent;
	savechild = child;
	savebrother = brother;
	savesister = sister;

	if( savesister ){
		//長男ではない
		savesister->brother = savebrother;
		if( savebrother )
			savebrother->sister = savesister;

		parent = 0;
		sister = 0;
		brother = 0;

	}else{
		//長男(parentからchildでリンクされている)
		if( saveparent )
			saveparent->child = savebrother;
		
		//if( savebrother )
		//	savebrother->sister = 0;
		if( savebrother )
			savebrother->sister = savesister;

		parent = 0;
		sister = 0;
		brother = 0;
	}

	//if( delchildflag != 0 ){
	//	child = 0;
	//}


	return 0;
}
int CShdElem::PutToChain( CShdElem* newparent, int add2lastflag )
{
	//newparentの子として、chainにputする。


	if( !newparent )
		return 1;
	
	if( add2lastflag == 0 ){
		CShdElem* savechild;
		savechild = newparent->child;

		if( savechild ){
			//newparentの新しい長男としてput
			newparent->child = this;
			
			parent = newparent;
			sister = 0;


			
			CShdElem* lastbro = brother;
			CShdElem* chkbro = brother;
			while( chkbro ){
				lastbro = chkbro;
				chkbro->parent = newparent;
				chkbro = chkbro->brother;
			}
			
			if( lastbro ){
				lastbro->brother = savechild;
				savechild->sister = lastbro;
			}else{
				brother = savechild;
				savechild->sister = this;
			}

			//brother = savechild;
			//if( savechild->sister ){
			//	_ASSERT( 0 );			
			//	return 1; //savechildが長男でない場合。バグ。
			//}
			//savechild->sister = this;


			//savechild->parent はそのまま。

		}else{
			newparent->child = this;
			parent = newparent;
			sister = 0;
			//brother = 0;

			CShdElem* chkbro = brother;
			while( chkbro ){
				chkbro->parent = newparent;
				chkbro = chkbro->brother;
			}

			//brotherはそのまま！！！

		}
	}else{
		CShdElem* childelem;
		childelem = newparent->child;

		if( !childelem ){
			newparent->child = this;
			
			parent = newparent;
			//child = 0;//<---------- childは、そのまま
			sister = 0;
			//brother = 0;
			//brotherはそのまま
			
			CShdElem* chkbro = brother;
			while( chkbro ){
				chkbro->parent = newparent;
				chkbro = chkbro->brother;
			}

		}else{
			CShdElem* lastbro = 0;
			CShdElem* curelem = childelem;

			while( curelem ){
				lastbro = curelem;
				curelem = curelem->brother;
			}
			
			_ASSERT( !lastbro->brother );

			lastbro->brother = this;
			sister = lastbro;
			parent = newparent;
			//brother = 0;//brotherはそのまま
			//child = 0;//<---------- childは、そのまま

			CShdElem* chkbro = brother;
			while( chkbro ){
				chkbro->parent = newparent;
				chkbro = chkbro->brother;
			}

		}

	}
	return 0;
}

void CShdElem::CalcDepthReq( int srcdepth )
{
	depth = srcdepth;

	if( brother )
		brother->CalcDepthReq( srcdepth );

	if( child )
		child->CalcDepthReq( srcdepth + 1 );
}

/***
CBezMesh* CShdElem::BezDiv( CBezMesh* srcbmesh )
{
	CBezMesh* newbm;
	CMeshInfo dstinfo;
	CMeshInfo* srcbminfo;
	CMeshInfo* srcvlinfo;
	CBezLine* bl;
	CBezLine* newbl;
	CVecLine* vl;
	int ret, linenum, pointnum, mclosed, nclosed;
	int newlinenum, newpointnum;
	int lno, pno, beflno, aftlno;

	srcbminfo = srcbmesh->meshinfo;
	linenum = srcbminfo->n;
	mclosed = srcbminfo->mclosed;
	nclosed = srcbminfo->nclosed;

	bl = (*srcbmesh)( 0 );
	vl = (*bl)( 0 );
	srcvlinfo = vl->meshinfo;
	pointnum = srcvlinfo->m;

	_ASSERT( nclosed == 0 );
	newlinenum = linenum * 2 - 1;

	if( mclosed == 0 ){
		newpointnum = pointnum * 2 - 1;
	}else{
		newpointnum = pointnum * 2;
	}

	ret = dstinfo.CopyData( srcbminfo );
	_ASSERT( !ret );
	dstinfo.n = newlinenum;
	dstinfo.m = 1;
	dstinfo.total = newlinenum;

	newbm = NewBMesh( &dstinfo );
	if( !newbm ){
		DbgOut( "CShdElem : BezDiv : NewBMesh error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	dstinfo.m = newpointnum;
	dstinfo.n = 1;
	dstinfo.total = newpointnum;
	dstinfo.type = SHDBEZLINE;
	for( lno = 0; lno < newlinenum; lno++ ){
		newbl = NewBLine( &dstinfo );		
		if( !newbl ){
			DbgOut( "CShdElem : BezDiv : NewBLine error !!!\n" );
			_ASSERT( 0 );
			return 0;
		}
		ret = newbm->SetMem( newbl, lno, 0, SHDBEZLINE );// blseriはダミー。　bm->hblseriは使えない。
		_ASSERT( !ret );

	}

	ret = newbm->SetEvenData( srcbmesh );
	_ASSERT( !ret );

	for( lno = 0; lno < newlinenum; lno+= 2 ){
		for( pno = 1; pno < newpointnum - 1; pno+= 2 ){
			ret = newbm->CalcBezDiv_U( lno, pno, pno - 1, pno + 1 );
			_ASSERT( !ret );
		}
		if( mclosed == 1 ){
			ret = newbm->CalcBezDiv_U( lno, newpointnum - 1, newpointnum - 2, 0 );
			_ASSERT( !ret );
		}
	}

	for( lno = 0; lno < newlinenum; lno+= 2 ){
		if( lno == 0 ){
			beflno = lno;
			aftlno = lno + 2;
		}else if( lno == (newlinenum - 1) ){
			beflno = lno - 2;
			aftlno = lno;
		}else{
			beflno = lno - 2;
			aftlno = lno + 2;
		}
		for( pno = 1; pno < newpointnum - 1; pno+= 2 ){
			ret = newbm->CalcNewpLat( lno, beflno, aftlno, pno );
			_ASSERT( !ret );
		}
		if( mclosed == 1 ){
			ret = newbm->CalcNewpLat( lno, beflno, aftlno, newpointnum - 1 );
			_ASSERT( !ret );
		}
	}

	for( lno = 1; lno < newlinenum - 1; lno+= 2 ){
		for( pno = 0; pno < newpointnum; pno++ ){
			ret = newbm->CalcBezDiv_V( pno, lno, lno - 1, lno + 1 );
			_ASSERT( !ret );
		}
	}

	return newbm;
}
***/


/***
	switch( type ){
		case SHDTYPENONE:
		case SHDMESHINFO: 
		case SHDMATERIAL: 
		case SHDMATRIX:

		case SHDROOT:

		case SHDROTATE:
		case SHDSLIDER:
		case SHDSCALE:
		case SHDUNISCALE:
		case SHDPATH:
		case SHDBALLJOINT:
		
		case SHDPART:
		case SHDPOLYLINE:
		case SHDPOLYGON:
		case SHDSPHERE:
		case SHDBEZIERSURF:
		case SHDMESHES:
		case SHDDISK:
			
		case SHDLINE:
		case SHDMESH:
		case SHDBEZPOINT: // CBezData
		case SHDBEZLINE:	// CBezLine
		case SHDPLANES:

		case SHDEXTRUDE:

		case SHDTYPEERROR:
		case SHDTYPEMAX
	
	}

***/

int CShdElem::DebugPolymeshM()
{
	if( !polymesh ){
		_ASSERT( 0 );
		return 1;
	}

	CMeshInfo* mi;
	mi = polymesh->meshinfo;
	if( !mi ){
		_ASSERT( 0 );
		return 0;
	}

	mi->m -= 1;

	return 0;
}

int CShdElem::CreateBSphereData( int motkindnum, int framenum )
{
	//motkindnumのちぇっく。
	if( firstbs ){
		int bsnum = 0;
		CBSphere* chkbs = *firstbs;

		while( chkbs ){
			chkbs = *(firstbs + bsnum);
			if( chkbs )
				bsnum++;
		}

		if( bsnum >= motkindnum ){
			DbgOut( "shdelem : CreateBSphereData : bsnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	//dataの作成
	CBSphere* newbs;
	newbs = new CBSphere[ framenum ];
	if( !newbs ){
		DbgOut( "ShdElem : CreateBSphereData : newbs alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	
	//！！！データの終わりの印に、０を入れる！！！
	firstbs = (CBSphere**)realloc( firstbs, sizeof( CBSphere* ) * (motkindnum + 1) );
	if( !firstbs ){
		DbgOut( "ShdElem : CreateBSphereData : firstbs alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	*(firstbs + motkindnum - 1) = newbs;
	*(firstbs + motkindnum) = 0;


	return 0;
}

int CShdElem::RemakeBSphereData( int motid, int framenum )
{

	//motidのちぇっく。
	if( firstbs ){
		int bsnum = 0;
		CBSphere* chkbs = *firstbs;

		while( chkbs ){
			chkbs = *(firstbs + bsnum);
			if( chkbs )
				bsnum++;
		}

		if( (motid < 0) || (bsnum <= motid) ){
			DbgOut( "shdelem : RemakeBSphereData : motid error %d, %d!!!\n", bsnum, motid );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		DbgOut( "shdelem : RemakeBSphereData : firstbs error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//dataの作成
	CBSphere* newbs;
	newbs = new CBSphere[ framenum ];
	if( !newbs ){
		DbgOut( "ShdElem : RemakeBSphereData : newbs alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	
	delete [] *(firstbs + motid );//!!!!!!!!!!!!!

	*(firstbs + motid) = newbs;

	return 0;
}


int CShdElem::DestroyBSphereData()
{
	if( firstbs ){
		int i = 0;
		CBSphere* delbs = *firstbs;
		while( delbs ){
			i++;
			delete [] delbs;
			delbs = *(firstbs + i);
		}

		free( firstbs );
		firstbs = 0;
	}

	return 0;
}

int CShdElem::TransformOnlyWorld( CMCache* mcache, CMotHandler* mhptr, int motcookie, int frameno, D3DXMATRIX matWorld, int calcflag, CBSphere* totalbs )
{
	if( !d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}

	CBSphere* setbs;
	if( motcookie >= 0 ){
		if( firstbs && *(firstbs + motcookie) ){
			setbs = *(firstbs + motcookie) + frameno;
		}else{
			setbs = 0;
		}
	}else{
		setbs = &curbs;
	}

	int ret;
	ret = d3ddisp->TransformOnlyWorld( mcache, mhptr, matWorld, calcflag, setbs, totalbs );
	if( ret ){
		DbgOut( "shdelem : TransformOnlyWorld : d3ddisp->TransformOnlyWorld error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CShdElem::TransformOnlyWorld3( CMotHandler* mhptr, D3DXMATRIX matWorld, D3DXVECTOR3* dstvec )
{
	if( !d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = d3ddisp->TransformOnlyWorld3( mhptr, matWorld, dstvec, 0 );
	if( ret ){
		DbgOut( "shdelem : TransformOnlyWorld : d3ddisp->TransformOnlyWorld3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}


int CShdElem::SetCurrentBSphere( int motkind, int frameno )
{
	if( !firstbs ){
		DbgOut( "shdelem : SetCurrentBSphere : firstbs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curbs = *( *(firstbs + motkind) + frameno );

	return 0;
}


int CShdElem::SetGroundObj( CMCache* mcache, CMotHandler* lpmh, D3DXMATRIX matWorld )
{
	if( !d3ddisp || ( !polymesh && !polymesh2 ) ){
		DbgOut( "shdelem : SetGroundObj : d3ddisp polymesh polymesh2 NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->SetGroundObj( mcache, lpmh, matWorld );
	if( ret ){
		DbgOut( "shdelem : SetGroundObj : d3ddisp SetGroundObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::ChkConfVecAndFaceInfScope( int isindex, D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : ChkConfVecAndFaceInfScope : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	if( !curis->d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curis->d3ddisp->ChkConfVecAndFace( befpos, newpos, confpointnum, confdata, confmaxnum, ds );
	if( ret ){
		DbgOut( "shdelem : ChkConfVecAndFaceInfScope : d3ddisp ChkConfVecAndFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int confno;
	for( confno = 0; confno < *confpointnum; confno++ ){
		(confdata + confno)->partsno = serialno;
	}

	return 0;
}


int CShdElem::ChkConfVecAndFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds )
{

	if( !d3ddisp || (!polymesh && !polymesh2) ){
		DbgOut( "shdelem : ChkConfVecAndFace : d3ddisp polymesh polymesh2 NULL error !!!\n" );
		_ASSERT( 0 );
		*confpointnum = 0;
		return 1;
	}

	int ret;
	ret = d3ddisp->ChkConfVecAndFace( befpos, newpos, confpointnum, confdata, confmaxnum, ds );
	if( ret ){
		DbgOut( "shdelem : ChkConfVecAndFace : d3ddisp ChkConfVecAndFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int confno;
	for( confno = 0; confno < *confpointnum; confno++ ){
		(confdata + confno)->partsno = serialno;
	}


	return 0;
}

int CShdElem::ChkConfVecAndGroundFace( D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpointnum, CONFDATA* confdata, int confmaxnum, DISTSAMPLE* ds )
{

	if( !d3ddisp || (!polymesh && !polymesh2) ){
		DbgOut( "shdelem : ChkConfVecAndGroundFace : d3ddisp polymesh polymesh2 NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->ChkConfVecAndGroundFace( befpos, newpos, confpointnum, confdata, confmaxnum, ds );
	if( ret ){
		DbgOut( "shdelem : ChkConfVecAndGroundFace : d3ddisp ChkConfVecAndGroundFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int confno;
	for( confno = 0; confno < *confpointnum; confno++ ){
		(confdata + confno)->partsno = serialno;
	}

	return 0;
}

int CShdElem::RenderBillboardShadowMap0( CShdHandler* lpsh, LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, D3DXVECTOR3 vEyePt )
{
	D3DXMATRIX firstscale;
	lpsh->GetFirstScale( &firstscale );

	int ret;
	int pmno;
	CBillboardElem* curbbelem;
	CShdElem* curselem;
	CD3DDisp* curd3ddisp;

	char* beftexname = NULL;
	char* curtexname = NULL;
	int cmp;

	LPDIRECT3DTEXTURE9 curtex = NULL;

	int befblendmode = -1;
	//HRESULT hres;

	for( pmno = 0; pmno < billboard->dispnum; pmno++ ){
		curbbelem = *(billboard->disparray + pmno);

		if( curbbelem && (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) && (curbbelem->selem->invisibleflag == 0) ){
			curselem = curbbelem->selem;
			if( !curselem ){
				_ASSERT( 0 );
				return 1;
			}

			curd3ddisp = curselem->d3ddisp;
			if( !curd3ddisp ){
				_ASSERT( 0 );
				return 1;
			}

			int vflag;

			vflag = curselem->curbs.visibleflag;
			DWORD dispswitchno = curselem->dispswitchno;
			if( vflag && (curselem->notuse != 1) && 
				((lpsh->m_curds + dispswitchno)->state != 0)
			){

				lpsh->SetCurTexname( curselem->serialno );

				curtexname = curselem->curtexname;
				if( curtexname == 0 )
					curtexname = NULL;
				
				
				if( beftexname && curtexname ){
					cmp = strcmp( beftexname, curtexname );
					if( cmp != 0 ){
						curtex = g_texbnk->GetTexData( curtexname, curselem->transparent );
					}
				}else{
					if( beftexname != curtexname ){
						curtex = g_texbnk->GetTexData( curtexname, curselem->transparent );
						//pd3dDevice->SetTexture( 0, curtex );
					}
				}
				
				SetRenderState( pd3dDevice, curselem );

				D3DXMATRIX newmatW;
				if( curselem->m_bbdirmode == 0 ){
					newmatW = *matWorld1;
//DbgOut( "selem : rednerbb : %d dirmode 0\r\n", serialno );
				}else{
					newmatW = *matWorld2;//!!!!!!!!!!!
//DbgOut( "selem : rednerbb : %d dirmode 1\r\n", serialno );
				}

				newmatW = curd3ddisp->m_scalemat * curbbelem->rotmat * newmatW;//!!!!!!!!!!!!!!!!!!!!!!!!!

				newmatW._41 += curbbelem->pos.x;
				newmatW._42 += curbbelem->pos.y;
				newmatW._43 += curbbelem->pos.z;

				D3DXMATRIX finalmatW;
				finalmatW = newmatW * *matWorld;


				ret = curd3ddisp->TransformSkinMat( serialno, lpsh->m_seri2boneno, &firstscale, 0, &finalmatW, matView, matProj, &vEyePt );
				if( ret ){
					DbgOut( "selem : RenderBillboardShadowMap0 : d3ddisp TransformSkinMat error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = curd3ddisp->RenderBillboardShadowMap0( pd3dDevice, lpsh, curtex );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				beftexname = curtexname;

			}

		}
	}

	return 0;
}

int CShdElem::RenderBillboardShadowMap1( CShdHandler* lpsh, LPDIRECT3DDEVICE9 pd3dDevice,
	D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, D3DXVECTOR3 vEyePt )
{

	D3DXMATRIX firstscale;
	lpsh->GetFirstScale( &firstscale );

	int ret;
	int pmno;
	CBillboardElem* curbbelem;
	CShdElem* curselem;
	CD3DDisp* curd3ddisp;

	char* beftexname = NULL;
	char* curtexname = NULL;
	//int cmp;

	LPDIRECT3DTEXTURE9 curtex = NULL;

	int befblendmode = -1;

	float fogparams[3];

	
	if( m_particleflag == 0 ){

		for( pmno = 0; pmno < billboard->dispnum; pmno++ ){
			curbbelem = *(billboard->disparray + pmno);

			if( curbbelem && (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) && (curbbelem->selem->invisibleflag == 0) ){
				curselem = curbbelem->selem;
				if( !curselem ){
					_ASSERT( 0 );
					return 1;
				}

				fogparams[0] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGSTART])));
				fogparams[1] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGEND])));
				fogparams[2] = g_fogtype;

				curd3ddisp = curselem->d3ddisp;
				if( !curd3ddisp ){
					_ASSERT( 0 );
					return 1;
				}

				int vflag;

				vflag = curselem->curbs.visibleflag;
				DWORD dispswitchno = curselem->dispswitchno;
				if( vflag && (curselem->notuse != 1) && 
					((lpsh->m_curds + dispswitchno)->state != 0)
				){

					lpsh->SetCurTexname( curselem->serialno );

					curtexname = curselem->curtexname;
					if( curtexname == 0 )
						curtexname = NULL;
					
					if( curtexname ){
						curtex = g_texbnk->GetTexData( curtexname, curselem->transparent );
					}

					SetRenderState( pd3dDevice, curselem );

					if( curselem->blendmode != befblendmode ){
						if( curselem->blendmode == 0 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
							//SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, FALSE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 1 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_ONE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_ONE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x0 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 2 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_ONE );
							//SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, FALSE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x0 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 103 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ZERO );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 104 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ZERO );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 105 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 106 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 107 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}else if( curselem->blendmode == 108 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						}
					}
					befblendmode = curselem->blendmode;


	//DbgOut( "shdelem : RenderBillboard : RenderTri\n" );				

					//SetRenderState( pd3dDevice, curselem );

					D3DXMATRIX newmatW;
					if( curselem->m_bbdirmode == 0 ){
						newmatW = *matWorld1;
//DbgOut( "selem : rednerbb : %d dirmode 0\r\n", serialno );
					}else{
						newmatW = *matWorld2;//!!!!!!!!!!!
//DbgOut( "selem : rednerbb : %d dirmode 1\r\n", serialno );
					}

					newmatW = curd3ddisp->m_scalemat * curbbelem->rotmat * newmatW;//!!!!!!!!!!!!!!!!!!!!!!!!!

					newmatW._41 += curbbelem->pos.x;
					newmatW._42 += curbbelem->pos.y;
					newmatW._43 += curbbelem->pos.z;

					D3DXMATRIX finalmatW;
					finalmatW = newmatW * *matWorld;


					ret = curd3ddisp->TransformSkinMat( serialno, lpsh->m_seri2boneno, &firstscale, 0, &finalmatW, matView, matProj, &vEyePt );
					if( ret ){
						DbgOut( "selem : RenderBillboard : d3ddisp TransformSkinMat error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					ret = curd3ddisp->RenderBillboardShadowMap1( pd3dDevice, lpsh, curtex, fogparams );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}

					beftexname = curtexname;

				}

	//DbgOut( "shdelem : RenderBillboard : curselem %d\n", curselem->serialno );

			}
		}
	}

	return 0;
}


int CShdElem::TransformBillboard( CShdHandler* lpsh, D3DXMATRIX matWorld, D3DXMATRIX matWorld1, D3DXMATRIX matWorld2, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3 vEyePt )
{
	D3DXMATRIX firstscale;
	lpsh->GetFirstScale( &firstscale );

	int ret;
	int pmno;
	CBillboardElem* curbbelem;
	CShdElem* curselem;
	CD3DDisp* curd3ddisp;

	for( pmno = 0; pmno < billboard->dispnum; pmno++ ){
		curbbelem = *(billboard->disparray + pmno);

		if( curbbelem && (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) && (curbbelem->selem->invisibleflag == 0) ){
			curselem = curbbelem->selem;
			if( !curselem ){
				_ASSERT( 0 );
				return 1;
			}

			curd3ddisp = curselem->d3ddisp;
			if( !curd3ddisp ){
				_ASSERT( 0 );
				return 1;
			}

			int vflag;

			vflag = curselem->curbs.visibleflag;
			DWORD dispswitchno = curselem->dispswitchno;
			if( vflag && (curselem->notuse != 1) && 
				((lpsh->m_curds + dispswitchno)->state != 0)
			){

				D3DXMATRIX newmatW;
				if( curselem->m_bbdirmode == 0 ){
					newmatW = matWorld1;
				}else{
					newmatW = matWorld2;//!!!!!!!!!!!
				}

				newmatW = curd3ddisp->m_scalemat * curbbelem->rotmat * newmatW;//!!!!!!!!!!!!!!!!!!!!!!!!!

				newmatW._41 += curbbelem->pos.x;
				newmatW._42 += curbbelem->pos.y;
				newmatW._43 += curbbelem->pos.z;

				D3DXMATRIX finalmatW;
				finalmatW = newmatW * matWorld;


				//pd3dDevice->SetTransform( D3DTS_WORLD,  &newmatW );//!!!!!!!!
				//pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
				//pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );							

				ret = curd3ddisp->TransformSkinMat( serialno, lpsh->m_seri2boneno, &firstscale, 0, &finalmatW, &matView, &matProj, &vEyePt );
				if( ret ){
					DbgOut( "selem : TransformBillboard : d3ddisp TransformSkinMat error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}


	return 0;
}


int CShdElem::RenderBillboard( int transskip, CShdHandler* lpsh, LPDIRECT3DDEVICE9 pd3dDevice, CTexBank* texbnk, int tlmode, 
	D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, 
	D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, D3DXVECTOR3 vEyePt, int glowflag )
{

	D3DXMATRIX firstscale;
	lpsh->GetFirstScale( &firstscale );



	int ret;
	int pmno;
	CBillboardElem* curbbelem;
	CShdElem* curselem;
	CD3DDisp* curd3ddisp;

	char* beftexname = NULL;
	char* curtexname = NULL;
	int cmp;

	LPDIRECT3DTEXTURE9 curtex = NULL;

	int befblendmode = -1;

	float fogparams[3];

	
	if( m_particleflag == 0 ){

		for( pmno = 0; pmno < billboard->dispnum; pmno++ ){
			curbbelem = *(billboard->disparray + pmno);

			if( curbbelem && (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) && (curbbelem->selem->invisibleflag == 0) ){
				curselem = curbbelem->selem;
				if( !curselem ){
					_ASSERT( 0 );
					return 1;
				}

				fogparams[0] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGSTART])));
				fogparams[1] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGEND])));
				fogparams[2] = g_fogtype;

				curd3ddisp = curselem->d3ddisp;
				if( !curd3ddisp ){
					_ASSERT( 0 );
					return 1;
				}

				int vflag;

				vflag = curselem->curbs.visibleflag;
				DWORD dispswitchno = curselem->dispswitchno;
				if( vflag && (curselem->notuse != 1) && 
					((lpsh->m_curds + dispswitchno)->state != 0)
				){

					lpsh->SetCurTexname( curselem->serialno );

					curtexname = curselem->curtexname;
					if( curtexname == 0 )
						curtexname = NULL;
					
					
					if( beftexname && curtexname ){
						cmp = strcmp( beftexname, curtexname );
						if( cmp != 0 ){
							curtex = texbnk->GetTexData( curtexname, curselem->transparent );
							if( g_curtex0 != curtex ){
								pd3dDevice->SetTexture( 0, curtex );
								g_curtex0 = curtex;
							}

						}
					}else{
						if( beftexname != curtexname ){
							curtex = texbnk->GetTexData( curtexname, curselem->transparent );
							//pd3dDevice->SetTexture( 0, curtex );
							if( g_curtex0 != curtex ){
								pd3dDevice->SetTexture( 0, curtex );
								g_curtex0 = curtex;
							}

						}
					}

	//DbgOut( "shdelem : RenderBillboard : seri %d, bmode %d, befblendmode %d\n", curselem->serialno, curselem->blendmode, befblendmode );

					SetRenderState( pd3dDevice, curselem );
					
					//if( curselem->blendmode != befblendmode ){
						if( curselem->blendmode == 0 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
							if( glowflag == 0 ){
//								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, FALSE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 1 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_ONE );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_ONE );
							if( glowflag == 0 ){
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 2 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_ONE );
							if( glowflag == 0 ){
//								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, FALSE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 103 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ZERO );
							if( glowflag == 0 ){
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 104 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_ZERO );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
							if( glowflag == 0 ){
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 105 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_ONE );
							if( glowflag == 0 ){
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 106 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
							if( glowflag == 0 ){
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 107 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
							if( glowflag == 0 ){
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}else if( curselem->blendmode == 108 ){
							SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
							SetRenderStateIfNotSame( pd3dDevice,  D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
							if( glowflag == 0 ){
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x00 );
								SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
							}
						}
					//}

					befblendmode = curselem->blendmode;


	//DbgOut( "shdelem : RenderBillboard : RenderTri\n" );				

					//SetRenderState( pd3dDevice, curselem );

					if( transskip == 0 ){
						D3DXMATRIX newmatW;
						if( curselem->m_bbdirmode == 0 ){
							newmatW = *matWorld1;
	//DbgOut( "selem : rednerbb : %d dirmode 0\r\n", serialno );
						}else{
							newmatW = *matWorld2;//!!!!!!!!!!!
	//DbgOut( "selem : rednerbb : %d dirmode 1\r\n", serialno );
						}

						
						newmatW = curd3ddisp->m_scalemat * curbbelem->rotmat * newmatW;//!!!!!!!!!!!!!!!!!!!!!!!!!

						newmatW._41 += curbbelem->pos.x;
						newmatW._42 += curbbelem->pos.y;
						newmatW._43 += curbbelem->pos.z;

						D3DXMATRIX finalmatW;
						finalmatW = newmatW * matWorld;


						pd3dDevice->SetTransform( D3DTS_WORLD,  &finalmatW );//!!!!!!!!
						pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
						pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );							

						ret = curd3ddisp->TransformSkinMat( serialno, lpsh->m_seri2boneno, &firstscale, 0, &finalmatW, &matView, &matProj, &vEyePt );
						if( ret ){
							DbgOut( "selem : RenderBillboard : d3ddisp TransformSkinMat error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}

					int glowmode;
					if( glowflag == 0 ){
						glowmode = 0;
					}else{
						glowmode = curselem->m_bbexttexmode;
					}
					ret = curd3ddisp->RenderTri( pd3dDevice, lpsh, m_toonparams, curtex, fogparams, curselem->polymesh->m_toon1, m_lightflag, m_mbluralpha, 0, glowmode, curselem->m_bbglowmult );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}

					beftexname = curtexname;

				}

	//DbgOut( "shdelem : RenderBillboard : curselem %d\n", curselem->serialno );

			}
		}
	}else{
		// particleのrender

		//fog
		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

		g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, g_fogcolor );

		g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );

		g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
		pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

		g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
		pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );

		g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
		pd3dDevice->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );


		/////////////////
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
		g_renderstate[ D3DRS_ZWRITEENABLE ] = FALSE;

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
		g_renderstate[ D3DRS_LIGHTING ] = FALSE;

		if( billboard->cmpalways == 0 ){
			pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
			g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;
		}else{
			pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );//!!!!!!!!!!!!!!!
			g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_ALWAYS;
		}


		if( blendmode == 0 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;

			if( glowflag == 0 ){
//				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE );
//				g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		//DbgOut( "shdelem : RenderBillboard : blend src\n" );

//_ASSERT( 0 );
		}else if( blendmode == 1 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ONE;

			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

			if( glowflag == 0 ){
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		//DbgOut( "shdelem : RenderBillboard : blend one\n" );

		}else if( blendmode == 2 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

			if( glowflag == 0 ){
//				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE );
//				g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
//_ASSERT( 0 );

		}else if( blendmode == 103 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_DESTCOLOR;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ZERO;

			if( glowflag == 0 ){
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		}else if( blendmode == 104 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_REVSUBTRACT;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ZERO;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;

			if( glowflag == 0 ){
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		}else if( blendmode == 105 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

			if( glowflag == 0 ){
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		}else if( blendmode == 106 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;

			if( glowflag == 0 ){
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		}else if( blendmode == 107 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;

			if( glowflag == 0 ){
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		}else if( blendmode == 108 ){
			pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
			g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;

			pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;

			if( glowflag == 0 ){
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}
		}



//DbgOut( "\r\nRenderBillBoard / time %f\r\n", billboard->time );		

		int elemno;
		for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
			curbbelem = billboard->bbarray + elemno;
			if( curbbelem && (curbbelem->useflag == 1) ){
				curselem = curbbelem->selem;
				if( curselem && (curselem->notuse == 0) ){

					fogparams[0] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGSTART])));
					fogparams[1] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGEND])));
					fogparams[2] = g_fogtype;

					curd3ddisp = curselem->d3ddisp;
					if( !curd3ddisp ){
						_ASSERT( 0 );
						return 1;
					}
//DbgOut( "check!!!, se RenderBillboard, elemno %d, seri %d, m_alpha %f\r\n", elemno, curselem->serialno, curd3ddisp->m_alpha );

					int vflag;

					vflag = curselem->curbs.visibleflag;
					DWORD dispswitchno = curselem->dispswitchno;
					if( vflag && (curselem->notuse != 1) && 
						((lpsh->m_curds + dispswitchno)->state != 0)
					){

						lpsh->SetCurTexname( curselem->serialno );

						curtexname = curselem->curtexname;
						if( curtexname == 0 )
							curtexname = NULL;
						
						
						if( beftexname && curtexname ){
							cmp = strcmp( beftexname, curtexname );
							if( cmp != 0 ){
								curtex = texbnk->GetTexData( curtexname, curselem->transparent );
								if( g_curtex0 != curtex ){
									pd3dDevice->SetTexture( 0, curtex );
									g_curtex0 = curtex;
								}

							}
						}else{
							if( beftexname != curtexname ){
								curtex = texbnk->GetTexData( curtexname, curselem->transparent );
								if( g_curtex0 != curtex ){
									pd3dDevice->SetTexture( 0, curtex );
									g_curtex0 = curtex;
								}
							}
						}

		//DbgOut( "shdelem : RenderBillboard : RenderTri\n" );				

						//SetRenderState( pd3dDevice, curselem );

						//if( tlmode == TLMODE_D3D ){
							D3DXMATRIX newmatW;
							if( curselem->m_bbdirmode == 0 ){
								newmatW = *matWorld1;
		//DbgOut( "selem : rednerbb : %d dirmode 0\r\n", serialno );
							}else{
								newmatW = *matWorld2;//!!!!!!!!!!!
		//DbgOut( "selem : rednerbb : %d dirmode 1\r\n", serialno );
							}

							newmatW = curd3ddisp->m_scalemat * curbbelem->rotmat * newmatW;//!!!!!!!!!!!!!!!!!!!!!!!!!

							newmatW._41 += curbbelem->pos.x;
							newmatW._42 += curbbelem->pos.y;
							newmatW._43 += curbbelem->pos.z;

							D3DXMATRIX finalmatW;
							finalmatW = newmatW * matWorld;


							pd3dDevice->SetTransform( D3DTS_WORLD,  &finalmatW );//!!!!!!!!
							pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
							pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );							
						//}


//DbgOut( "RenderBillboard / serial %d, pos %f, %f, %f\r\n", curselem->serialno, curbbelem->pos.x, curbbelem->pos.y, curbbelem->pos.z );
//DbgOut( "RenderBillboard / createtime %f, alpha %f, tileno %d\r\n", curbbelem->createtime, curselem->alpha, curbbelem->m_textileno );

						if( g_useGPU ){

							ret = curd3ddisp->TransformSkinMat( serialno, lpsh->m_seri2boneno, &firstscale, 0, &finalmatW, &matView, &matProj, &vEyePt );
							if( ret ){
								DbgOut( "selem : RenderBillboard : d3ddisp TransformSkinMat error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}

						int glowmode;
						if( glowflag == 0 ){
							glowmode = 0;
						}else{
							glowmode = curselem->m_bbexttexmode;
						}
						ret = curd3ddisp->RenderTri( pd3dDevice, lpsh, m_toonparams, curtex, fogparams, curselem->polymesh->m_toon1, m_lightflag, m_mbluralpha, 0, glowmode, curselem->m_bbglowmult );
						if( ret ){
							_ASSERT( 0 );
							return 1;
						}

						beftexname = curtexname;

					}


				}
			}

		}


	}

	return 0;
}

int CShdElem::MakeFaceno2Materialno( ARGBF* sceneamb, int srcshader, int sigmagicno, int** arrayptr, CMQOMaterial* newmathead, CMQOMaterial* shmathead )
{
	int ret;

	switch( type ){
	case SHDPOLYMESH:
		ret = polymesh->MakeFaceno2Materialno( srcshader, sigmagicno, arrayptr, newmathead, shmathead, serialno );
		if( ret ){
			DbgOut( "shdelem : MakeFaceno2Materialno : polymesh error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		ret = polymesh2->MakeFaceno2Materialno( sceneamb, srcshader, sigmagicno, arrayptr, newmathead, shmathead, serialno );
		if( ret ){
			DbgOut( "shdelem : MakeFaceno2Materialno : polymesh error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		break;
	}
	return 0;
}


int CShdElem::WriteMQOObjectOnFrame( HANDLE hfile, int* matnoindex, int motid, int frameno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag )
{
	int ret;

	switch( type ){
	case SHDPOLYMESH:
		ret = polymesh->WriteMQOObjectOnFrame( hfile, matnoindex, motid, frameno, serialno, lpsh, lpmh, srcmult, writeRevFlag );
		if( ret ){
			DbgOut( "shdelem : WriteMQOObject : polymesh WriteMQOObject error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;

	case SHDPOLYMESH2:
		ret = polymesh2->WriteMQOObjectOnFrame( hfile, matnoindex, texname, motid, frameno, serialno, lpsh, lpmh, srcmult, writeRevFlag );
		if( ret ){
			DbgOut( "shdelem : WriteMQOObject : polymesh WriteMQOObject error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDMORPH:

		if( morph->m_objtype == SHDPOLYMESH ){

			ret = morph->m_baseelem->polymesh->WriteMQOObjectOnFrameMorph( hfile, matnoindex, 
				motid, frameno, serialno, lpsh, lpmh, srcmult, writeRevFlag );
			if( ret ){
				DbgOut( "selem : WriteMQOObjectOnFrame : pm WriteMQOObjectOnFrameMorph error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( morph->m_objtype == SHDPOLYMESH2 ){
			ret = morph->m_baseelem->polymesh2->WriteMQOObjectOnFrameMorph( hfile, matnoindex, 
				texname, motid, frameno, serialno, lpsh, lpmh, srcmult, writeRevFlag );
			if( ret ){
				DbgOut( "selem : WriteMQOObjectOnFrame : pm2 WriteMQOObjectOnFrameMorph error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	default:
		break;
	}

	return 0;
}



int CShdElem::WriteMQOObject( HANDLE hfile, int* matnoindex )
{
	int ret;

	switch( type ){
	case SHDPOLYMESH:
		ret = polymesh->WriteMQOObject( hfile, matnoindex, this );
		if( ret ){
			DbgOut( "shdelem : WriteMQOObject : polymesh WriteMQOObject error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;

	case SHDPOLYMESH2:
		ret = polymesh2->WriteMQOObject( hfile, matnoindex, texname, this );
		if( ret ){
			DbgOut( "shdelem : WriteMQOObject : polymesh WriteMQOObject error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;

	default:
		break;
	}

	return 0;
}

int CShdElem::GetBillboardNum( int* bbnumptr )
{
	if( (type != SHDBILLBOARD) || !billboard ){
		*bbnumptr = 0;
		return 0;
	}

	CBillboardElem* curbbelem;
	int bbelemno;
	int bbelemcnt = 0;
	for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
		curbbelem = billboard->bbarray + bbelemno;

		if( (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) ){
			bbelemcnt++;
		}
	}

	*bbnumptr = bbelemcnt;

	return 0;
}
int CShdElem::GetBillboardInfo( int srcbbid, D3DXVECTOR3* bbposptr, char* texnameptr, int* transparentptr, float* widthptr, float* heightptr )
{
	if( (type != SHDBILLBOARD) || !billboard ){
		DbgOut( "shdelem : GetBillboardInfo : billboard not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBillboardElem* curbbelem;
	int bbelemno;
	int setflag = 0;
	for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
		curbbelem = billboard->bbarray + bbelemno;

		if( (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) ){

			if( curbbelem->selem->serialno == srcbbid ){

				*bbposptr = curbbelem->pos;
				
				if( curbbelem->selem->texname ){
					int nameleng;
					nameleng = (int)strlen( curbbelem->selem->texname );
					if( nameleng >= 256 ){
						DbgOut( "shdelem : GetBillboardInfo : name leng too long error !!! %s\n", curbbelem->selem->texname );
						_ASSERT( 0 );
						return 1;
					}
					strcpy_s( texnameptr, 256, curbbelem->selem->texname );
				}else{
					*texnameptr = 0;
				}

				*transparentptr = curbbelem->selem->transparent;

				VEC3F* pbuf0;
				VEC3F* pbuf1;
				VEC3F* pbuf2;
				if( curbbelem->selem->polymesh && curbbelem->selem->polymesh->pointbuf ){
					pbuf0 = curbbelem->selem->polymesh->pointbuf;
					pbuf1 = curbbelem->selem->polymesh->pointbuf + 1;
					pbuf2 = curbbelem->selem->polymesh->pointbuf + 2;

					*widthptr = pbuf2->x - pbuf0->x;
					*heightptr = pbuf1->y - pbuf0->y;
				}else{
					DbgOut( "shdelem : GetBillboardInfo : polymesh not exist error !!!\n" );
					_ASSERT( 0 );

					*widthptr = 0.0f;
					*heightptr = 0.0f;
				}
				setflag = 1;
				break;
			}
		}
	}

	if( setflag == 0 ){
		DbgOut( "shdelem : GetBillboardInfo : bbid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetAllBillboardInfo( int getnum, int* bbnoptr, D3DXVECTOR3* bbposptr, char** texnameptr, int* transparentptr, float* widthptr, float* heightptr )
{
	if( (type != SHDBILLBOARD) || !billboard ){
		DbgOut( "shdelem : GetAllBillboardInfo : billboard not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBillboardElem* curbbelem;
	int bbelemno;
	int bbelemcnt = 0;
	for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
		curbbelem = billboard->bbarray + bbelemno;

		if( (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) ){
			if( getnum <= bbelemcnt ){
				DbgOut( "shdelem : GetAllBillboardInfo : getnum error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			*( bbnoptr + bbelemcnt ) = curbbelem->selem->serialno;
			*( bbposptr + bbelemcnt ) = curbbelem->pos;
			

			*( texnameptr + bbelemcnt ) = curbbelem->selem->texname;
			*( transparentptr + bbelemcnt ) = curbbelem->selem->transparent;

			VEC3F* pbuf0;
			VEC3F* pbuf1;
			VEC3F* pbuf2;
			if( curbbelem->selem->polymesh && curbbelem->selem->polymesh->pointbuf ){
				pbuf0 = curbbelem->selem->polymesh->pointbuf;
				pbuf1 = curbbelem->selem->polymesh->pointbuf + 1;
				pbuf2 = curbbelem->selem->polymesh->pointbuf + 2;

				*( widthptr + bbelemcnt ) = pbuf2->x - pbuf0->x;
				*( heightptr + bbelemcnt ) = pbuf1->y - pbuf0->y;
			}else{
				DbgOut( "shdelem : GetAllBillboardInfo : polymesh not exist error !!!\n" );
				_ASSERT( 0 );

				*( widthptr + bbelemcnt ) = 0.0f;
				*( heightptr + bbelemcnt ) = 0.0f;
			}

			bbelemcnt++;
		}
	}

	if( getnum > bbelemcnt ){
		int fillno;
		for( fillno = bbelemcnt; fillno < getnum; fillno++ ){
			*( bbnoptr + fillno ) = -1;
		}
	}


	return 0;
}

int CShdElem::GetNearBillboard( D3DXVECTOR3 srcpos, float maxdist, int* nearbbid )
{
	if( (type != SHDBILLBOARD) || !billboard ){
		DbgOut( "shdelem : GetNearBillboard : billboard not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBillboardElem* curbbelem;
	int bbelemno;
	float mindist = 1e10;
	int nearestid = -1;

	for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
		curbbelem = billboard->bbarray + bbelemno;

		if( (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) ){
			float curdist;
			curdist = ( curbbelem->pos.x - srcpos.x ) * ( curbbelem->pos.x - srcpos.x )
				+ ( curbbelem->pos.y - srcpos.y ) * ( curbbelem->pos.y - srcpos.y )
				+ ( curbbelem->pos.z - srcpos.z ) * ( curbbelem->pos.z - srcpos.z );

			if( (curdist < mindist) && (curdist < (maxdist * maxdist)) ){
				nearestid = curbbelem->selem->serialno;
				mindist = curdist;
			}
		}

	}

	*nearbbid = nearestid;

	return 0;
}

int CShdElem::GetDiffuse( int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;

	ARGBF dstcol;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : GetDiffuse : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->GetDiffuse( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetDiffuse : polymesh GetDiffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : GetDiffuse : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->GetDiffuse( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetDiffuse : polymesh2 GetDiffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	*rptr = (int)( dstcol.r * 255.0f );
	*gptr = (int)( dstcol.g * 255.0f );
	*bptr = (int)( dstcol.b * 255.0f );

	return 0;
}
int CShdElem::GetAmbient( int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;

	ARGBF dstcol;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : GetAmbient : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->GetAmbient( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetAmbient : polymesh GetAmbient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : GetAmbient : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->GetAmbient( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetAmbient : polymesh2 GetAmbient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	*rptr = (int)( dstcol.r * 255.0f );
	*gptr = (int)( dstcol.g * 255.0f );
	*bptr = (int)( dstcol.b * 255.0f );

	return 0;
}
int CShdElem::GetSpecularPower( int vertno, float* powerptr )
{
	int ret;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : GetSpecularPower : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->GetSpecularPower( vertno, powerptr );
		if( ret ){
			DbgOut( "shdelem : GetSpecularPower : polymesh GetSpecularPower error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : GetSpecularPower : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->GetSpecularPower( vertno, powerptr );
		if( ret ){
			DbgOut( "shdelem : GetSpecularPower : polymesh2 GetSpecularPower error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;

}


int CShdElem::GetEmissive( int vertno, int* rptr, int* gptr, int* bptr )
{

	int ret;

	ARGBF dstcol;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : GetEmissive : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->GetEmissive( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetEmissive : polymesh GetEmissive error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : GetEmissive : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->GetEmissive( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetEmissive : polymesh2 GetEmissive error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	*rptr = (int)( dstcol.r * 255.0f );
	*gptr = (int)( dstcol.g * 255.0f );
	*bptr = (int)( dstcol.b * 255.0f );

	return 0;

}


int CShdElem::GetSpecular( int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;

	ARGBF dstcol;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : GetSpecular : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->GetSpecular( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetSpecular : polymesh GetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : GetSpecular : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->GetSpecular( vertno, &dstcol );
		if( ret ){
			DbgOut( "shdelem : GetSpecular : polymesh2 GetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	*rptr = (int)( dstcol.r * 255.0f );
	*gptr = (int)( dstcol.g * 255.0f );
	*bptr = (int)( dstcol.b * 255.0f );

	return 0;
}
int CShdElem::SetMaterialDiffuse( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	int* dirtyptr = 0;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetMaterialDiffuse : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = polymesh->SetMaterialDiffuse( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialDiffuse : polymesh SetDiffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh->GetDirtyMaterial( matno );

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetMaterialDiffuse : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetMaterialDiffuse( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialDiffuse : polymesh2 SetDiffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh2->GetDirtyMaterial( matno );
	}

	if( d3ddisp && dirtyptr ){
		ARGBF setcol;
		if( polymesh ){
			ret = polymesh->GetMaterialDiffuse( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialDiffuse : pm GetDiffuse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}			
		}else if( polymesh2 ){
			ret = polymesh2->GetMaterialDiffuse( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialDiffuse : pm2 GetDiffuse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		int setr, setg, setb;
		setr = (int)( setcol.r * 255.0f );
		setr = min( 255, setr );
		setr = max( 0, setr );
		setg = (int)( setcol.g * 255.0f );
		setg = min( 255, setg );
		setg = max( 0, setg );
		setb = (int)( setcol.b * 255.0f );
		setb = min( 255, setb );
		setb = max( 0, setb );

		int ialpha = 255;
			
		DWORD dwcol;
		dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

		ret = d3ddisp->SetOrgTlvColorArray( dirtyptr, dwcol, MATERIAL_DIFFUSE );
		if( ret ){
			DbgOut( "shdelem : SetMaterialDiffuse : d3ddisp SetOrgTlvColorArray error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdElem::SetMaterialSpecular( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	int* dirtyptr = 0;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetMaterialSpecular : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = polymesh->SetMaterialSpecular( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialSpecular : polymesh SetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh->GetDirtyMaterial( matno );

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetMaterialSpecular : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetMaterialSpecular( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialSpecular : polymesh2 SetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh2->GetDirtyMaterial( matno );
	}

	if( d3ddisp && dirtyptr ){
		ARGBF setcol;
		if( polymesh ){
			ret = polymesh->GetMaterialSpecular( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialSpecular : pm GetSpecular error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}			
		}else if( polymesh2 ){
			ret = polymesh2->GetMaterialSpecular( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialSpecular : pm2 GetSpecular error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		int setr, setg, setb;
		setr = (int)( setcol.r * 255.0f );
		setr = min( 255, setr );
		setr = max( 0, setr );
		setg = (int)( setcol.g * 255.0f );
		setg = min( 255, setg );
		setg = max( 0, setg );
		setb = (int)( setcol.b * 255.0f );
		setb = min( 255, setb );
		setb = max( 0, setb );

		int ialpha = 255;
			
		DWORD dwcol;
		dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

		ret = d3ddisp->SetOrgTlvColorArray( dirtyptr, dwcol, MATERIAL_SPECULAR );
		if( ret ){
			DbgOut( "shdelem : SetMaterialDiffuse : d3ddisp SetOrgTlvColorArray error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CShdElem::SetMaterialAmbient( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	int* dirtyptr = 0;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetMaterialAmbient : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = polymesh->SetMaterialAmbient( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialAmbient : polymesh SetAmbient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh->GetDirtyMaterial( matno );

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetMaterialAmbient : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetMaterialAmbient( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialAmbient : polymesh2 SetAmbient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh2->GetDirtyMaterial( matno );
	}

	if( d3ddisp && dirtyptr ){
		ARGBF setcol;
		if( polymesh ){
			ret = polymesh->GetMaterialAmbient( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialAmbient : pm GetAmbient error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}			
		}else if( polymesh2 ){
			ret = polymesh2->GetMaterialAmbient( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialAmbient : pm2 GetAmbient error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		int setr, setg, setb;
		setr = (int)( setcol.r * 255.0f );
		setr = min( 255, setr );
		setr = max( 0, setr );
		setg = (int)( setcol.g * 255.0f );
		setg = min( 255, setg );
		setg = max( 0, setg );
		setb = (int)( setcol.b * 255.0f );
		setb = min( 255, setb );
		setb = max( 0, setb );

		int ialpha = 255;
			
		DWORD dwcol;
		dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

		ret = d3ddisp->SetOrgTlvColorArray( dirtyptr, dwcol, MATERIAL_AMBIENT );
		if( ret ){
			DbgOut( "shdelem : SetMaterialDiffuse : d3ddisp SetOrgTlvColorArray error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CShdElem::SetMaterialEmissive( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	int* dirtyptr = 0;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetMaterialEmissive : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = polymesh->SetMaterialEmissive( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialEmissive : polymesh SetEmissive error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh->GetDirtyMaterial( matno );

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetMaterialEmissive : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetMaterialEmissive( matno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetMaterialEmissive : polymesh2 SetEmissive error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh2->GetDirtyMaterial( matno );
	}

	if( d3ddisp && dirtyptr ){
		ARGBF setcol;
		if( polymesh ){
			ret = polymesh->GetMaterialEmissive( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialEmissive : pm GetEmissive error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}			
		}else if( polymesh2 ){
			ret = polymesh2->GetMaterialEmissive( matno, &setcol );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialEmissive : pm2 GetEmissive error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		int setr, setg, setb;
		setr = (int)( setcol.r * 255.0f );
		setr = min( 255, setr );
		setr = max( 0, setr );
		setg = (int)( setcol.g * 255.0f );
		setg = min( 255, setg );
		setg = max( 0, setg );
		setb = (int)( setcol.b * 255.0f );
		setb = min( 255, setb );
		setb = max( 0, setb );

		int ialpha = 255;
			
		DWORD dwcol;
		dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

		ret = d3ddisp->SetOrgTlvColorArray( dirtyptr, dwcol, MATERIAL_EMISSIVE );
		if( ret ){
			DbgOut( "shdelem : SetMaterialDiffuse : d3ddisp SetOrgTlvColorArray error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CShdElem::SetMaterialPower( int matno, int setflag, float srcpow )
{
	int ret;

	int* dirtyptr = 0;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetMaterialPower : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = polymesh->SetMaterialPower( matno, setflag, srcpow );
		if( ret ){
			DbgOut( "shdelem : SetMaterialPower : polymesh SetPower error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh->GetDirtyMaterial( matno );

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetMaterialPower : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetMaterialPower( matno, setflag, srcpow );
		if( ret ){
			DbgOut( "shdelem : SetMaterialPower : polymesh2 SetPower error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		dirtyptr = polymesh2->GetDirtyMaterial( matno );
	}

	if( d3ddisp && dirtyptr ){
		float newpow;
		if( polymesh ){
			ret = polymesh->GetMaterialPower( matno, &newpow );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialPower : pm GetPower error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}			
		}else if( polymesh2 ){
			ret = polymesh2->GetMaterialPower( matno, &newpow );// setflagにより引数の色とは異なる。
			if( ret ){
				DbgOut( "se : SetMaterialPower : pm2 GetPower error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = d3ddisp->SetOrgTlvPowerArray( dirtyptr, newpow );
		if( ret ){
			DbgOut( "shdelem : SetMaterialDiffuse : d3ddisp SetOrgTlvPowerArray error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CShdElem::SetDispDiffuse( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno, int updateflag )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetDispDiffuse : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//ret = polymesh->SetDiffuse( vertno, setflag, srccol, d3ddisp, tlmode );
		ret = polymesh->SetDiffuse( vertno, setflag, srccol, 0, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispDiffuse : polymesh SetDiffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetDispDiffuse : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//ret = polymesh2->SetDiffuse( vertno, setflag, srccol, d3ddisp, tlmode );
		ret = polymesh2->SetDiffuse( vertno, setflag, srccol, 0, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispDiffuse : polymesh2 SetDiffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( updateflag && d3ddisp ){
		if( vertno >= 0 ){
			ARGBF setcol;
			if( polymesh ){
				ret = polymesh->GetDiffuse( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispDiffuse : pm GetDiffuse error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}			
			}else if( polymesh2 ){
				ret = polymesh2->GetDiffuse( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispDiffuse : pm2 GetDiffuse error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			int setr, setg, setb;
			setr = (int)( setcol.r * 255.0f );
			setr = min( 255, setr );
			setr = max( 0, setr );
			setg = (int)( setcol.g * 255.0f );
			setg = min( 255, setg );
			setg = max( 0, setg );
			setb = (int)( setcol.b * 255.0f );
			setb = min( 255, setb );
			setb = max( 0, setb );

			int ialpha = (int)(255.0f * alpha);
			ialpha = min( 255, ialpha );
			ialpha = max( 0, ialpha );
			
			DWORD dwcol;
			dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

			ret = d3ddisp->SetOrgTlvColor( vertno, dwcol, MATERIAL_DIFFUSE );
			if( ret ){
				DbgOut( "shdelem : SetDispDiffuse : d3ddisp SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{

			DWORD setvno;
			for( setvno = 0; setvno < d3ddisp->m_numTLV; setvno++ ){

				ARGBF setcol;
				if( polymesh ){
					ret = polymesh->GetDiffuse( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispDiffuse : pm GetDiffuse error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}			
				}else if( polymesh2 ){
					ret = polymesh2->GetDiffuse( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispDiffuse : pm2 GetDiffuse error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				int setr, setg, setb;
				setr = (int)( setcol.r * 255.0f );
				setr = min( 255, setr );
				setr = max( 0, setr );
				setg = (int)( setcol.g * 255.0f );
				setg = min( 255, setg );
				setg = max( 0, setg );
				setb = (int)( setcol.b * 255.0f );
				setb = min( 255, setb );
				setb = max( 0, setb );

				int ialpha = (int)(255.0f * alpha);
				ialpha = min( 255, ialpha );
				ialpha = max( 0, ialpha );
				
				DWORD dwcol;
				dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

				ret = d3ddisp->SetOrgTlvColor( setvno, dwcol, MATERIAL_DIFFUSE );
				if( ret ){
					DbgOut( "shdelem : SetDispDiffuse : d3ddisp SetOrgTlvColor error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;
}

int CShdElem::SetDispAmbient( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetDispAmbient : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->SetAmbient( vertno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetDispAmbient : polymesh SetAmbient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetDispAmbient : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetAmbient( vertno, setflag, srccol );
		if( ret ){
			DbgOut( "shdelem : SetDispAmbient : polymesh2 SetAmbient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}		

	}

	if( d3ddisp ){
		if( vertno >= 0 ){
			ARGBF setcol;
			if( polymesh ){
				ret = polymesh->GetAmbient( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispAmbient : pm GetAmbient error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}			
			}else if( polymesh2 ){
				ret = polymesh2->GetAmbient( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispAmbient : pm2 GetAmbient error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			int setr, setg, setb;
			setr = (int)( setcol.r * 255.0f );
			setr = min( 255, setr );
			setr = max( 0, setr );
			setg = (int)( setcol.g * 255.0f );
			setg = min( 255, setg );
			setg = max( 0, setg );
			setb = (int)( setcol.b * 255.0f );
			setb = min( 255, setb );
			setb = max( 0, setb );

			int ialpha = (int)(255.0f * alpha);
			ialpha = min( 255, ialpha );
			ialpha = max( 0, ialpha );
			
			DWORD dwcol;
			dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

			ret = d3ddisp->SetOrgTlvColor( vertno, dwcol, MATERIAL_AMBIENT );
			if( ret ){
				DbgOut( "shdelem : SetDispAmbient : d3ddisp SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{

			DWORD setvno;
			for( setvno = 0; setvno < d3ddisp->m_numTLV; setvno++ ){

				ARGBF setcol;
				if( polymesh ){
					ret = polymesh->GetAmbient( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispAmbient : pm GetAmbient error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}			
				}else if( polymesh2 ){
					ret = polymesh2->GetAmbient( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispAmbient : pm2 GetAmbient error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				int setr, setg, setb;
				setr = (int)( setcol.r * 255.0f );
				setr = min( 255, setr );
				setr = max( 0, setr );
				setg = (int)( setcol.g * 255.0f );
				setg = min( 255, setg );
				setg = max( 0, setg );
				setb = (int)( setcol.b * 255.0f );
				setb = min( 255, setb );
				setb = max( 0, setb );

				int ialpha = (int)(255.0f * alpha);
				ialpha = min( 255, ialpha );
				ialpha = max( 0, ialpha );
				
				DWORD dwcol;
				dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

				ret = d3ddisp->SetOrgTlvColor( setvno, dwcol, MATERIAL_AMBIENT );
				if( ret ){
					DbgOut( "shdelem : SetDispAmbient : d3ddisp SetOrgTlvColor error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}


	return 0;
}

int CShdElem::SetDispEmissive( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetDispEmissive : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->SetEmissive( vertno, setflag, srccol, d3ddisp, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispEmissive : polymesh SetEmissive error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetDispEmissive : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetEmissive( vertno, setflag, srccol, d3ddisp, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispEmissive : polymesh2 SetEmissive error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( d3ddisp ){
		if( vertno >= 0 ){
			ARGBF setcol;
			if( polymesh ){
				ret = polymesh->GetEmissive( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispEmissive : pm GetEmissive error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}			
			}else if( polymesh2 ){
				ret = polymesh2->GetEmissive( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispEmissive : pm2 GetEmissive error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			int setr, setg, setb;
			setr = (int)( setcol.r * 255.0f );
			setr = min( 255, setr );
			setr = max( 0, setr );
			setg = (int)( setcol.g * 255.0f );
			setg = min( 255, setg );
			setg = max( 0, setg );
			setb = (int)( setcol.b * 255.0f );
			setb = min( 255, setb );
			setb = max( 0, setb );

			int ialpha = (int)(255.0f * alpha);
			ialpha = min( 255, ialpha );
			ialpha = max( 0, ialpha );
			
			DWORD dwcol;
			dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

			ret = d3ddisp->SetOrgTlvColor( vertno, dwcol, MATERIAL_EMISSIVE );
			if( ret ){
				DbgOut( "shdelem : SetDispEmissive : d3ddisp SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{

			DWORD setvno;
			for( setvno = 0; setvno < d3ddisp->m_numTLV; setvno++ ){

				ARGBF setcol;
				if( polymesh ){
					ret = polymesh->GetEmissive( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispEmissive : pm GetEmissive error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}			
				}else if( polymesh2 ){
					ret = polymesh2->GetEmissive( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispEmissive : pm2 GetEmissive error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				int setr, setg, setb;
				setr = (int)( setcol.r * 255.0f );
				setr = min( 255, setr );
				setr = max( 0, setr );
				setg = (int)( setcol.g * 255.0f );
				setg = min( 255, setg );
				setg = max( 0, setg );
				setb = (int)( setcol.b * 255.0f );
				setb = min( 255, setb );
				setb = max( 0, setb );

				int ialpha = (int)(255.0f * alpha);
				ialpha = min( 255, ialpha );
				ialpha = max( 0, ialpha );
				
				DWORD dwcol;
				dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

				ret = d3ddisp->SetOrgTlvColor( setvno, dwcol, MATERIAL_EMISSIVE );
				if( ret ){
					DbgOut( "shdelem : SetDispEmissive : d3ddisp SetOrgTlvColor error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;

}
int CShdElem::SetDispSpecularPower( float srcpow, int setflag, int tlmode, int vertno )
{
	int ret;
	
	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetDispSpecularPower : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->SetSpecularPower( vertno, setflag, srcpow, d3ddisp, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispSpecularPower : polymesh SetSpecularPower error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetDispSpecularPower : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetSpecularPower( vertno, setflag, srcpow, d3ddisp, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispSpecularPower : polymesh2 SetSpecularPower error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( d3ddisp ){
		if( vertno >= 0 ){
			float setcol;
			if( polymesh ){
				ret = polymesh->GetSpecularPower( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispSpecularPower : pm GetSpecularPower error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}			
			}else if( polymesh2 ){
				ret = polymesh2->GetSpecularPower( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispSpecularPower : pm2 GetSpecularPower error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			ret = d3ddisp->SetOrgTlvPower( vertno, setcol );
			if( ret ){
				DbgOut( "shdelem : SetDispSpecularPower : d3ddisp SetOrgTlvPower error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{

			DWORD setvno;
			for( setvno = 0; setvno < d3ddisp->m_numTLV; setvno++ ){

				float setcol;
				if( polymesh ){
					ret = polymesh->GetSpecularPower( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispSpecularPower : pm GetSpecularPower error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}			
				}else if( polymesh2 ){
					ret = polymesh2->GetSpecularPower( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispSpecularPower : pm2 GetSpecularPower error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				ret = d3ddisp->SetOrgTlvPower( setvno, setcol );
				if( ret ){
					DbgOut( "shdelem : SetDispSpecularPower : d3ddisp SetOrgTlvPower error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;

}


int CShdElem::SetDispSpecular( int srcr, int srcg, int srcb, int setflag, int tlmode, int vertno )
{
	int ret;

	ARGBF srccol;
	srccol.a = 1.0f;
	srccol.r = srcr / 255.0f;
	srccol.g = srcg / 255.0f;
	srccol.b = srcb / 255.0f;

	if( type == SHDPOLYMESH ){
		if( !polymesh ){
			DbgOut( "shdelem : SetDispSpecular : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->SetSpecular( vertno, setflag, srccol, d3ddisp, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispSpecular : polymesh SetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : SetDispSpecular : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->SetSpecular( vertno, setflag, srccol, d3ddisp, tlmode );
		if( ret ){
			DbgOut( "shdelem : SetDispSpecular : polymesh2 SetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( d3ddisp ){
		if( vertno >= 0 ){
			ARGBF setcol;
			if( polymesh ){
				ret = polymesh->GetSpecular( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispSpecular : pm GetSpecular error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}			
			}else if( polymesh2 ){
				ret = polymesh2->GetSpecular( vertno, &setcol );// setflagにより引数の色とは異なる。
				if( ret ){
					DbgOut( "se : SetDispSpecular : pm2 GetSpecular error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			int setr, setg, setb;
			setr = (int)( setcol.r * 255.0f );
			setr = min( 255, setr );
			setr = max( 0, setr );
			setg = (int)( setcol.g * 255.0f );
			setg = min( 255, setg );
			setg = max( 0, setg );
			setb = (int)( setcol.b * 255.0f );
			setb = min( 255, setb );
			setb = max( 0, setb );

			int ialpha = (int)(255.0f * alpha);
			ialpha = min( 255, ialpha );
			ialpha = max( 0, ialpha );
			
			DWORD dwcol;
			dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

			ret = d3ddisp->SetOrgTlvColor( vertno, dwcol, MATERIAL_SPECULAR );
			if( ret ){
				DbgOut( "shdelem : SetDispDiffuse : d3ddisp SetOrgTlvColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{

			DWORD setvno;
			for( setvno = 0; setvno < d3ddisp->m_numTLV; setvno++ ){

				ARGBF setcol;
				if( polymesh ){
					ret = polymesh->GetSpecular( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispSpecular : pm GetSpecular error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}			
				}else if( polymesh2 ){
					ret = polymesh2->GetSpecular( setvno, &setcol );// setflagにより引数の色とは異なる。
					if( ret ){
						DbgOut( "se : SetDispSpecular : pm2 GetSpecular error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
				int setr, setg, setb;
				setr = (int)( setcol.r * 255.0f );
				setr = min( 255, setr );
				setr = max( 0, setr );
				setg = (int)( setcol.g * 255.0f );
				setg = min( 255, setg );
				setg = max( 0, setg );
				setb = (int)( setcol.b * 255.0f );
				setb = min( 255, setb );
				setb = max( 0, setb );

				int ialpha = (int)(255.0f * alpha);
				ialpha = min( 255, ialpha );
				ialpha = max( 0, ialpha );
				
				DWORD dwcol;
				dwcol = (ialpha << 24) | (setr << 16) | (setg << 8) | setb;

				ret = d3ddisp->SetOrgTlvColor( setvno, dwcol, MATERIAL_SPECULAR );
				if( ret ){
					DbgOut( "shdelem : SetDispSpecular : d3ddisp SetOrgTlvColor error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;
}


int CShdElem::SetDefaultRenderState()
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
m_renderstate[D3DRS_FOGENABLE] = g_fogenable;
	m_renderstate[D3DRS_SPECULARENABLE] = TRUE;//<----------
//	m_renderstate[D3DRS_ZVISIBLE] = 0;
m_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
	m_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;

m_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
m_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
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
//	m_renderstate[D3DRS_WRAP0] = D3DWRAP_U | D3DWRAP_V;
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
m_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
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


int CShdElem::SetRenderState( LPDIRECT3DDEVICE9 pdev, CShdElem* selem )
{
	HRESULT hr;
	int i;
	int rsno;
	int curvalue;
	for( i = 0; i < 80; i++ ){
		rsno = g_rsindex[i];

		if( (rsno >= 0) && (rsno != D3DRS_ALPHABLENDENABLE) && (rsno != D3DRS_ALPHATESTENABLE) &&
			(rsno != D3DRS_BLENDOP) && (rsno != D3DRS_SRCBLEND) && (rsno != D3DRS_DESTBLEND) ){
			curvalue = selem->m_renderstate[ rsno ];
			if( g_renderstate[ rsno ] != curvalue ){
				hr = pdev->SetRenderState( (D3DRENDERSTATETYPE)rsno, curvalue );
				if( hr != D3D_OK ){
					DbgOut( "selem : SetRenderState : %d error !!!\r\n", rsno );
				}
				g_renderstate[ rsno ] = curvalue;
			}
		}
	}


	HRESULT hr1, hr2;
	DWORD curfilter;
	curfilter = selem->m_minfilter;
	if( g_minfilter != curfilter ){
		hr1 = pdev->SetSamplerState( 0, D3DSAMP_MINFILTER, curfilter );
		if( hr1 != D3D_OK ){
			DbgOut( "sh : SetRenderState : minfilter 0 error !!!\n" );
		}
		hr2 = pdev->SetSamplerState( 1, D3DSAMP_MINFILTER, curfilter );
		if( hr2 != D3D_OK ){
			DbgOut( "sh : SetRenderState : minfilter 1 error !!!\n" );
		}
		g_minfilter = curfilter;
	}

	curfilter = selem->m_magfilter;
	if( g_magfilter != curfilter ){
		hr1 = pdev->SetSamplerState( 0, D3DSAMP_MAGFILTER, curfilter );
		if( hr1 != D3D_OK ){
			DbgOut( "sh : SetRenderState : magfilter 0 error !!!\n" );
		}
		hr2 = pdev->SetSamplerState( 1, D3DSAMP_MAGFILTER, curfilter );
		if( hr2 != D3D_OK ){
			DbgOut( "sh : SetRenderState : magfilter 1 error !!!\n" );
		}
		g_magfilter = curfilter;
	}


	return 0;
}

int CShdElem::SetRenderStateIfNotSame( LPDIRECT3DDEVICE9 pd3dDevice, int srcstate, DWORD srcval )
{
	_ASSERT( pd3dDevice );

	if( (g_renderstate[ srcstate ] != srcval) && (srcstate >= 0) && (srcstate <= D3DRS_BLENDOP) ){
		pd3dDevice->SetRenderState( (D3DRENDERSTATETYPE)srcstate, srcval );
		g_renderstate[ srcstate ] = srcval;
	}

	return 0;
}


int CShdElem::SetDispScale( D3DXVECTOR3 scalevec, D3DXVECTOR3 centervec )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : SetDispScale : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = d3ddisp->SetOrgScale( scalevec, centervec );
	if( ret ){
		DbgOut( "shdelem : SetDispScale : d3ddisp SetOrgScale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
	
int CShdElem::SetBIM( CShdHandler* lpsh, int srcvertno, int srcchildno, int srcparentno, int srccalcmode )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : SetBIM : d3ddisp not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( (srcchildno <= 0) || (srcchildno >= lpsh->s2shd_leng) ){
		DbgOut( "shdelem : SetBIM : child boneno error %d !!!\n", srcchildno );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* childselem;
	childselem = (*lpsh)( srcchildno );
	if( !childselem || (childselem->IsJoint() == 0) ){
		DbgOut( "shdelem : SetBIM : child elem is not bone : invalid child boneno %d error !!!\n", srcchildno );
		_ASSERT( 0 );
	}


	if( srcparentno > 0 ){
		if( srcparentno >= lpsh->s2shd_leng ){
			DbgOut( "shdelem : SetBIM : parent boneno error %d !!!\n", srcparentno );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* parentselem;
		parentselem = (*lpsh)( srcparentno );
		if( !parentselem || (parentselem->IsJoint() == 0) ){
			DbgOut( "shdelem : SetBIM : parent elem is not bone : invalid parent boneno %d error !!!\n", srcparentno );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* upperjoint;
		upperjoint = lpsh->FindUpperJoint( childselem );
		if( upperjoint != parentselem ){
			DbgOut( "shdelem : SetBIM : invalid relation of child and parent error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		srcparentno = -1;
	}

	int ret;

	ret = d3ddisp->SetBIM( lpsh, serialno, srcvertno, srcchildno, srcparentno, srccalcmode );
	if( ret ){
		DbgOut( "shdelem : SetBIM : d3ddisp SetBIM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetDispScreenPosInfScope( int isindex, int vertno, D3DXVECTOR2* posptr, int tlmode )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetDispScreenPosInfScope : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	if( !curis->d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 dstpos;

	int ret;
	ret = curis->d3ddisp->GetDispScreenPos( vertno, &dstpos, tlmode );
	if( ret ){
		DbgOut( "shdelem : GetDispScreenPosInfScope : d3ddisp GetDispScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	posptr->x = dstpos.x;
	posptr->y = dstpos.y;

	return 0;
}

int CShdElem::GetDispScreenPos( int vertno, D3DXVECTOR3* posptr, int tlmode )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : GetDispScreenPos : d3ddisp not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	if( (type == SHDPOLYMESH) || (type == SHDPOLYMESH2) ){

		ret = d3ddisp->GetDispScreenPos( vertno, posptr, tlmode );
		if( ret ){
			DbgOut( "shdelem : GetDispScreenPos : d3ddisp GetDispScreenPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{

		DbgOut( "shdelem : GetDispScreenPos : no 3d object warning !!!\n" );
		_ASSERT( 0 );

		posptr->x = -1.0f;
		posptr->y = -1.0f;
	}

	return 0;
}
int CShdElem::CalcScreenPos( int bbflag, CMotHandler* srclpmh, int vertno, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int tlmode, D3DXVECTOR3* posptr )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : CalcScreenPos : d3ddisp not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->CalcScreenPos( bbflag, srclpmh, vertno, dwClipWidth, dwClipHeight, matWorld, matView, matProj, aspect, tlmode, posptr );
	if( ret ){
		DbgOut( "shdelem : CalcScreenPos : d3ddisp CalcScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	if( (type == SHDPOLYMESH) || (type == SHDPOLYMESH2) ){

		ret = d3ddisp->CalcScreenPos( srclpmh, vertno, dwClipWidth, dwClipHeight, matWorld, matView, matProj, aspect, tlmode, posptr );
		if( ret ){
			DbgOut( "shdelem : CalcScreenPos : d3ddisp CalcScreenPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{

		DbgOut( "shdelem : ClalcScreenPos : no 3d object warning !!!\n" );
		_ASSERT( 0 );

		posptr->x = -1.0f;
		posptr->y = -1.0f;
	}
	***/


	return 0;
}

int CShdElem::PickVertInfScope( int isindex, int* seri2boneno, D3DXMATRIX firstscale, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh,
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float srcaspect,
		int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : PickVert3InfScope : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	if( !curis->d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_useGPU != 0 ){
		ret = curis->d3ddisp->TransformDispDataSCV( seri2boneno, serialno, &firstscale, dwClipWidth, dwClipHeight, srclpmh, 
			matWorld, matView, matProj, srcaspect );
		if( ret ){
			DbgOut( "se : PickVertInfScope : d3ddisp TransformDispDataSCV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	ret = curis->d3ddisp->PickVert( curis->type, serialno, posx, posy, rangex, rangey, pickptr, maxgetnum, getnumptr );
	if( ret ){
		DbgOut( "selem : PickVert3InfScope : d3ddisp PickVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



int CShdElem::PickVert( int posx, int posy, int rangex, int rangey, CPickData* pickptr, int maxgetnum, int* getnumptr )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : PickVert : d3ddisp not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	if( (type == SHDPOLYMESH) || (type == SHDPOLYMESH2) ){
		ret = d3ddisp->PickVert( type, serialno, posx, posy, rangex, rangey, pickptr, maxgetnum, getnumptr );
		if( ret ){
			DbgOut( "shdelem : PickVert : d3ddisp PickVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}

	return 0;
}
int CShdElem::PickVertWorld( D3DXVECTOR3 pickpos, float srcdist, int* dstvno, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;
	if( !d3ddisp )
		return 0;

	if( !d3ddisp->m_worldv )
		return 0;

	int setno = 0;
	DWORD vno;
	for( vno = 0; vno < d3ddisp->m_numTLV; vno++ ){
		D3DXVECTOR3* curwv;
		curwv = d3ddisp->m_worldv + vno;

		float mag;
		mag = ( pickpos.x - curwv->x ) * ( pickpos.x - curwv->x ) + 
			  ( pickpos.y - curwv->y ) * ( pickpos.y - curwv->y ) +
			  ( pickpos.z - curwv->z ) * ( pickpos.z - curwv->z );

		float dist;
		if( mag > 0.0f ){
			dist = (float)sqrt( mag );
		}else{
			dist = 0.0f;
		}

		if( dist <= srcdist ){
			if( setno < arrayleng ){
				*(dstvno + setno) = vno;
				setno++;
			}else{
				_ASSERT( 0 );
			}
		}
	}

	*getnumptr = setno;

	return 0;
}


int CShdElem::GetRevCullingFlag( int vertno, int* revcullptr )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : GetRevCullingFlag : d3ddisp not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	if( (type == SHDPOLYMESH) || (type == SHDPOLYMESH2) ){
		ret = d3ddisp->GetRevCullingFlag( type, vertno, revcullptr );
		if( ret ){
			DbgOut( "shdelem : GetRevCullingFlag : d3ddisp GetRevCullingFlag error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdElem::GetOrgVertNo( int vertno, int* orgnoarray, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	int ret;
	if( type == SHDPOLYMESH ){		
		if( !polymesh ){
			DbgOut( "shdelem : GetOrgVertNo : polymesh not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->GetOrgVertNo( vertno, orgnoarray, arrayleng, getnumptr );
		if( ret ){
			DbgOut( "shdelem : polymesh GetOrgVertNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "shdelem : GetOrgVertNo : polymesh2 not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->GetOrgVertNo( vertno, orgnoarray, arrayleng, getnumptr );
		if( ret ){
			DbgOut( "shdelem : polymesh GetOrgVertNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

/***
int CShdElem::GetBBox( int mode, CBBox* dstbb )
{
	int ret;

	if( !d3ddisp ){
		//DbgOut( "shdelem : GetBBox : d3ddisp NULL error !!!\n" );
		//_ASSERT( 0 );
		//return 1;
		dstbb->setflag = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!
	}

	ret = d3ddisp->GetBBox( mode, dstbb );
	if( ret ){
		DbgOut( "shdelem : GetBBox : d3ddisp GetBBox error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/

int CShdElem::GetVertNoOfFaceInfScope( int isindex, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetVertNoOfFaceInfScope : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	if( !curis->d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curis->d3ddisp->GetVertNoOfFace( faceno, vert1ptr, vert2ptr, vert3ptr );
	if( ret ){
		DbgOut( "selem : GetVertNoOfFaceInfScope : d3ddisp GetVertNoOfFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdElem::GetVertNoOfFace( int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr )
{
	if( !d3ddisp ){
		DbgOut( "selem : GetVertNoOfFace : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->GetVertNoOfFace( faceno, vert1ptr, vert2ptr, vert3ptr );
	if( ret ){
		DbgOut( "selem : GetVertNoOfFace : d3ddisp GetVertNoOfFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetSamePosVert( int vertno, int* sameptr, int arrayleng, int* samenumptr )
{

	if( !d3ddisp ){
		DbgOut( "selem : GetSamePos : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->GetSamePosVert( vertno, sameptr, arrayleng, samenumptr );
	if( ret ){
		DbgOut( "selem : GetSamePosVert : d3ddisp GetSamePosVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetTextureInfo( char* nameptr, int* repxptr, int* repyptr, float* animuptr, float* animvptr, int* transparentptr )
{
	if( nameptr ){
		if( texname ){
			strcpy_s( nameptr, 256, texname );
		}else{
			*nameptr = 0;
		}
	}
	if( repxptr ){
		*repxptr = texrepx;
	}
	if( repyptr ){
		*repyptr = texrepy;
	}
	if( animuptr ){
		*animuptr = uanime;
	}
	if( animvptr ){
		*animvptr = vanime;
	}
	if( transparentptr ){
		*transparentptr = transparent;
	}

	return 0;
}
int CShdElem::GetExtTextureInfo( char* extnameptr, int* texnumptr, int* texmodeptr, int* repflagptr, int* stepptr )
{
	if( extnameptr ){
		ZeroMemory( extnameptr, sizeof( char ) * 256 * 100 );
		int extno;
		char* dstname = extnameptr;
		for( extno = 0; extno < exttexnum; extno++ ){
			if( *(exttexname + extno ) ){
				strcpy_s( dstname, 256, *(exttexname + extno ) );
			}else{
				*dstname = 0;
			}
			dstname += 256;
		}
	}

	if( texnumptr ){
		*texnumptr = exttexnum;
	}
	if( texmodeptr ){
		*texmodeptr = exttexmode;
	}
	if( repflagptr ){
		*repflagptr = exttexrep;
	}
	if( stepptr ){
		*stepptr = exttexstep;
	}

	return 0;
}

/***
int CShdElem::HuGetBoneWeight( int vertno, int* boneno1, int* boneno2, float* rate1 )
{
	if( (type != SHDPOLYMESH) && (type != SHDPOLYMESH2) && ( !IsJoint() ) ){
		DbgOut( "selem : HuGetBoneWeight : this element has not bone weight error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !d3ddisp ){
		DbgOut( "selem : HuGetBoneWeight : d3ddisp not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CInfElem* ieptr = 0;
	switch( type ){
	case SHDPOLYMESH:
		if( !polymesh ){
			DbgOut( "selem : HuGetBoneWeight : pm NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = polymesh->m_IE;
		break;
	case SHDPOLYMESH2:
		if( !polymesh2 ){
			DbgOut( "selem : HuGetBoneWeight : pm2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = polymesh2->m_IE;
		break;
	default:
		if( !part ){
			DbgOut( "selem : HuGetBoneWeight : part NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = part->m_IE;
		break;
	}

	if( !ieptr ){
		DbgOut( "selem : HuGetBoneWeight : ie NULL error %d !!!\n", type );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= (int)d3ddisp->m_numTLV) ){
		DbgOut( "selem : HuGetBoneWeight : vertno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfElem* curie;
	curie = ieptr + vertno;

	if( curie->infnum != 2 ){
		DbgOut( "selem : HuGetBoneWeight : infnum error !!!\n" );
		_ASSERT( 0 );
		*boneno1 = -1;
		*boneno2 = -1;
		*rate1 = 0.0f;
		return 1;
	}


	if( boneno1 ){
		*boneno1 = curie->ie->bonematno;
	}
	if( boneno2 ){
		*boneno2 =(curie->ie + 1)->bonematno;
	}
	if( rate1 ){
		*rate1 = curie->ie->dispinf;
	}

	return 0;
}
***/

int CShdElem::GetTriIndicesInt( int* buf, int intnum, int* setintnum, int* revflag )
{
	if( (type != SHDPOLYMESH) && (type != SHDPOLYMESH2) ){
		DbgOut( "selem : GetTriIndices : type %d not supported error !!!\n", type );
		_ASSERT( 0 );
		return 1;
	}

	if( type == SHDPOLYMESH ){
		*setintnum = polymesh->meshinfo->n * 3;

		if( buf && (intnum > 0) ){

			if( intnum < ( polymesh->meshinfo->n * 3 ) ){
				DbgOut( "se : GetTriIndices : wordnum too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int i;
			for( i = 0; i < polymesh->meshinfo->n * 3; i++ ){
				*( buf + i ) = *( polymesh->indexbuf + i );
			}
		}

	}else if( type == SHDPOLYMESH2 ){
		*setintnum = polymesh2->meshinfo->n * 3;

		if( buf && (intnum > 0) ){

			if( intnum < ( polymesh2->meshinfo->n * 3 ) ){
				DbgOut( "se : GetTriIndices : wordnum too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int i;
			for( i = 0; i < polymesh2->meshinfo->n * 3; i++ ){
				*( buf + i ) = *( polymesh2->optindexbuf + i );
			}
		}


	}else{
		_ASSERT( 0 );
		return 1;
	}

	if( revflag ){
		if( clockwise == 3 )
			*revflag = 1;
		else
			*revflag = 0;
	}


	return 0;
}



int CShdElem::GetTriIndices( WORD* buf, int wordnum, int* setwordnum, int* revflag )
{
	if( (type != SHDPOLYMESH) && (type != SHDPOLYMESH2) ){
		DbgOut( "selem : GetTriIndices : type %d not supported error !!!\n", type );
		_ASSERT( 0 );
		return 1;
	}

	if( type == SHDPOLYMESH ){
		*setwordnum = polymesh->meshinfo->n * 3;

		if( buf && (wordnum > 0) ){

			if( wordnum < ( polymesh->meshinfo->n * 3 ) ){
				DbgOut( "se : GetTriIndices : wordnum too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int i;
			for( i = 0; i < polymesh->meshinfo->n * 3; i++ ){
				*( buf + i ) = (WORD)(*( polymesh->indexbuf + i ));
			}
		}

	}else if( type == SHDPOLYMESH2 ){
		*setwordnum = polymesh2->meshinfo->n * 3;

		if( buf && (wordnum > 0) ){

			if( wordnum < ( polymesh2->meshinfo->n * 3 ) ){
				DbgOut( "se : GetTriIndices : wordnum too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int i;
			for( i = 0; i < polymesh2->meshinfo->n * 3; i++ ){
				*( buf + i ) = (WORD)(*( polymesh2->optindexbuf + i ));
			}
		}


	}else{
		_ASSERT( 0 );
		return 1;
	}

	if( revflag ){
		if( clockwise == 3 )
			*revflag = 1;
		else
			*revflag = 0;
	}


	return 0;
}



int CShdElem::CalcMeshWorldv( int* seri2boneno, CMotHandler* srclpmh, D3DXMATRIX* firstscale, D3DXMATRIX* matWorldptr )
{
	int ret;

	if( (type == SHDPOLYMESH) || (type == SHDPOLYMESH2) ){
		if( !d3ddisp ){
			DbgOut( "selem : CalcMeshWorldv : d3ddisp NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = d3ddisp->CalcMeshWorldv( seri2boneno, srclpmh, serialno, firstscale, matWorldptr );
		if( ret ){
			DbgOut( "selem : CalcMeshWorldv : d3ddisp CalcMeshWorldv error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//if( serialno == 8 ){
//	_ASSERT( 0 );
//}


	}else if( type == SHDINFSCOPE ){
		CInfScope* curis;
		int isno;
		for( isno = 0; isno < scopenum; isno++ ){
			curis = *( ppscope + isno );
			_ASSERT( curis->d3ddisp );

			ret = curis->d3ddisp->CalcMeshWorldv( seri2boneno, srclpmh, serialno, firstscale, matWorldptr );
			if( ret ){
				DbgOut( "selem : CalcMeshWorldv : d3ddisp CalcMeshWorldv error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

void CShdElem::SetBoneLineDispReq( int srclinedisp, int broflag )
{
	m_bonelinedisp = srclinedisp;

	if( broflag && brother ){
		brother->SetBoneLineDispReq( srclinedisp, 1 );
	}

	if( child ){
		child->SetBoneLineDispReq( srclinedisp, 1 );
	}

}

int CShdElem::RestoreBoneInfo( CShdHandler* lpsh )
{
	int ret;

	if( !part ){
		return 0;
	}

	ret = part->RestoreBoneInfo( lpsh );
	if( ret ){
		DbgOut( "selem : part RestoreBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::FillDispColor( DWORD srccol )
{
	int ret;

	if( (type != SHDINFSCOPE) && (type != SHDBBOX) && (type != SHDMORPH) ){
		if( !d3ddisp )
			return 0;//!!!!!

		ret = d3ddisp->FillDispColor( srccol, 0 );
		if( ret ){
			DbgOut( "selem : d3ddisp FillDispColor error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDINFSCOPE ){
		int scno;
		CInfScope* curis;
		for( scno = 0; scno < scopenum; scno++ ){
			curis = *( ppscope + scno );
			_ASSERT( curis );

			if( curis->d3ddisp ){
				ret = curis->d3ddisp->FillDispColor( srccol, 0 );
				if( ret ){
					DbgOut( "selem : is d3ddisp FillDispColor error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}else if( type == SHDBBOX ){
		int bbxno;
		CBBox2* curbbx;
		for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
			curbbx = *( ppbbox + bbxno );
			_ASSERT( curbbx );

			if( curbbx->d3ddisp ){
				ret = curbbx->d3ddisp->FillDispColor( srccol, 0 );
				if( ret ){
					DbgOut( "selem : bbx d3ddisp FillUpDispColor error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}else if( type == SHDMORPH ){
		if( !d3ddisp )
			return 0;

		CPolyMesh2* pm2;
		if( morph->m_objtype == SHDPOLYMESH2 ){
			pm2 = morph->m_baseelem->polymesh2;
		}else{
			pm2 = 0;
		}
		ret = d3ddisp->FillDispColor( srccol, pm2 );
		if( ret ){
			DbgOut( "se : FillDispColor : morph FillDispColor error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdElem::GetInitialInfScopeCenter( int isindex, D3DXVECTOR3* partpos )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetInitialInfScopeCenter : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );
	int ret;
	switch( curis->type ){
	case SHDPOLYMESH:
		if( curis->polymesh ){
			ret = curis->polymesh->CalcAnchorCenter();
			if( ret ){
				DbgOut( "selem : GetInitialInfScopeCenter : pm CalcAnchorCenter error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			*partpos = curis->polymesh->m_center;
		}else{
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		if( curis->polymesh2 ){
			ret = curis->polymesh2->CalcAnchorCenter();
			if( ret ){
				DbgOut( "selem : GetInitialInfScopeCenter : pm2 CalcAnchorCenter error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			*partpos = curis->polymesh2->m_center;
		}else{
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		_ASSERT( 0 );
		return 1;
		break;
	}

	return 0;
}

int CShdElem::GetInitialPartPos( D3DXVECTOR3* partpos )
{
	int ret;

	if( !d3ddisp ){
		DbgOut( "shdelem : GetInitialPartPos : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = d3ddisp->GetInitialPartPos( partpos );
	if( ret ){
		DbgOut( "selem : GetInitialPartPos : d3ddisp GetInitialPartPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetVertPos2InfScope( int isindex, CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetVertPos2InfScope : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	if( !curis->d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}

	CInfElem* ieptr = 0;
	switch( curis->type ){
	case SHDPOLYMESH:
		_ASSERT( curis->polymesh );
		ieptr = curis->polymesh->m_IE;
		break;
	case SHDPOLYMESH2:
		_ASSERT( curis->polymesh2 );
		ieptr = curis->polymesh2->m_IE;
		break;
	default:
		_ASSERT( 0 );
		return 1;
		break;
	}

	if( !ieptr ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curis->d3ddisp->TransformOnlyWorld1Vert( lpmh, matWorld, motid, frameno, vertno, dstpos, scaleflag, ieptr );
	if( ret ){
		DbgOut( "shdelem : GetVertPos2 : d3ddisp TransfromOnlyWorld1Vert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdElem::GetVertPos2( CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : GetVertPos2 : d3ddisp NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->TransformOnlyWorld1Vert( lpmh, matWorld, motid, frameno, vertno, dstpos, scaleflag, 0 );
	if( ret ){
		DbgOut( "shdelem : GetVertPos2 : d3ddisp TransfromOnlyWorld1Vert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CShdElem::GetVertPos3( CMotHandler* lpmh, D3DXMATRIX matWorld, int vertno, D3DXVECTOR3* dstpos )
{
	if( !d3ddisp ){
		DbgOut( "shdelem : GetVertPos3 : d3ddisp NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->TransformOnlyWorld1VertCurrent( lpmh, matWorld, vertno, dstpos, 1, 0 );

	if( ret ){
		DbgOut( "shdelem : GetVertPos3 : d3ddisp TransfromOnlyWorld1VertCurrent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}





int CShdElem::Copy2IndexBuffer( int frameno )
{
	if( !d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->Copy2IndexBuffer( frameno );
	if( ret ){
		DbgOut( "shdelem : Copy2IndexBuffer : d3ddisp Copy2IndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CShdElem::Copy2IndexBufferNotCulling()
{
	if( !d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->Copy2IndexBufferNotCulling();
	if( ret ){
		DbgOut( "shdelem : Copy2IndexBufferNotCulling : d3ddisp Copy2IndexBufferNotCulling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetFaceNum( int* facenum )
{
	if( (type != SHDPOLYMESH) && (type != SHDPOLYMESH2) ){
		_ASSERT( 0 );
		*facenum = 0;
		return 0;
	}

	if( !d3ddisp ){
		DbgOut( "selem : GetFaceNum : d3ddisp NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	*facenum = d3ddisp->m_vnum;

	return 0;
}

int CShdElem::GetNormal( D3DXVECTOR3* normalptr )
{

	if( !d3ddisp ){
		DbgOut( "selem : GetNormal : d3ddisp NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->GetNormal( normalptr );
	if( ret ){
		DbgOut( "selem : GetNormal : d3ddisp GetNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::CalcInitialNormal()
{
	if( !d3ddisp ){
		DbgOut( "selem : CalcInitialNormal : d3ddisp NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	if( (type != SHDPOLYMESH) && (type != SHDPOLYMESH2) ){
		DbgOut( "selem : CalcInitialNormal : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->CalcInitialNormal( polymesh2 );
	if( ret ){
		DbgOut( "selem : CalcInitialNormal : d3ddisp CalcInitialNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CShdElem::CalcNormalByWorldV()
{
	if( !d3ddisp ){
		DbgOut( "selem : CalcNormalByWorldV : d3ddisp NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	if( (type != SHDPOLYMESH) && (type != SHDPOLYMESH2) ){
		DbgOut( "selem : CalcNormalByWorldV : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = d3ddisp->CalcNormalByWorldV( polymesh2 );
	if( ret ){
		DbgOut( "selem : CalcNormalByWorldV : d3ddisp CalcInitialNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CShdElem::ChangeColor( CShdElem* srcse )
{
	int ret;

	if( type != srcse->type ){
		DbgOut( "se : ChangeColor : type not equal return %d\r\n", serialno );
		return 0;
	}

	CPolyMesh* srcpm;
	CPolyMesh2* srcpm2;

	switch( type ){
	case SHDPOLYMESH:
		srcpm = srcse->polymesh;
		if( !srcpm || !polymesh ){
			DbgOut( "se : ChangeColor : polymesh not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
			
		ret = polymesh->ChangeColor( srcpm );
		if( ret ){
			DbgOut( "se : ChangeColor : pm ChangeColor error !!!\n" );
			_ASSERT( 0 );
			return ret;
		}

		break;
	case SHDPOLYMESH2:
		srcpm2 = srcse->polymesh2;
		if( !srcpm2 || !polymesh2 ){
			DbgOut( "se : ChangeColor : polymesh2 not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
			
		ret = polymesh2->ChangeColor( serialno, srcpm2 );
		if( ret ){
			DbgOut( "se : ChangeColor : pm2 ChangeColor error !!!\n" );
			_ASSERT( 0 );
			return ret;
		}
		break;
	default:
		break;
	}

	return 0;

}

int CShdElem::RemoveInfluenceNo( int delno )
{
	int find = 0;
	int no;
	for( no = 0; no < influencebonenum; no++ ){
		if( *( influencebone + no ) == delno ){
			find = 1;
			break;
		}
	}

	if( find ){
		if( influencebonenum > 1 ){
			int* newinf;
			newinf = (int*)malloc( sizeof( int ) * ( influencebonenum - 1 ) );
			if( !newinf ){
				DbgOut( "selem : RemoveInfluenceNo : newinf alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int setno = 0;
			for( no = 0; no < influencebonenum; no++ ){
				int curno;
				curno = *( influencebone + no );
				if( curno != delno ){
					*( newinf + setno ) = curno;
					setno++;
				}
			}

			if( setno != (influencebonenum - 1) ){
				DbgOut( "selem : RemoveInfluenceNo : setno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			influencebonenum--;
			free( influencebone );
			influencebone = newinf;

		}else{
			influencebonenum = 0;
			free( influencebone );
			influencebone = 0;
		}
	}
/////////////////
	find = 0;
	for( no = 0; no < ignorebonenum; no++ ){
		if( *( ignorebone + no ) == delno ){
			find = 1;
			break;
		}
	}

	if( find ){
		if( ignorebonenum > 1 ){
			int* newinf;
			newinf = (int*)malloc( sizeof( int ) * ( ignorebonenum - 1 ) );
			if( !newinf ){
				DbgOut( "selem : RemoveInfluenceNo : newinf alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int setno = 0;
			for( no = 0; no < ignorebonenum; no++ ){
				int curno;
				curno = *( ignorebone + no );
				if( curno != delno ){
					*( newinf + setno ) = curno;
					setno++;
				}
			}

			if( setno != (ignorebonenum - 1) ){
				DbgOut( "selem : RemoveInfluenceNo : setno error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			ignorebonenum--;
			free( ignorebone );
			ignorebone = newinf;

		}else{
			ignorebonenum = 0;
			free( ignorebone );
			ignorebone = 0;
		}
	}

	return 0;

}


int CShdElem::DeleteBBoxByTarget( CShdElem* srctarget )
{
	if( bboxnum == 0 ){
		return 0;
	}

	int delnum = 0;
	int* deldirty = (int*)malloc( sizeof( int ) * bboxnum );
	if( !deldirty ){
		DbgOut( "se : DeleteBBoxByTarget : deldirty alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( deldirty, sizeof( int ) * bboxnum );

	int bbxno;
	for( bbxno = 0; bbxno < bboxnum; bbxno++ ){
		CBBox2* curbbx2 = *( ppbbox + bbxno );
		if( curbbx2->target == srctarget ){
			*( deldirty + bbxno ) = 1;
			delnum++;
		}
	}

	if( delnum > 0 ){
		CBBox2** newppbbox = (CBBox2**)malloc( sizeof( CBBox2* ) * (bboxnum - delnum) );
		if( !newppbbox ){
			DbgOut( "se : DeleteBBoxByTarget : newppbbox alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int bno;
		int setno = 0;
		for( bno = 0; bno < bboxnum; bno++ ){
			if( *(deldirty + bno) == 0 ){
				*( newppbbox + setno ) = *( ppbbox + bno );
				setno++;
			}else{
				delete *( ppbbox + bno );//!!!!!!!!!!
			}
		}

//_ASSERT( 0 );

		free( ppbbox );
		ppbbox = newppbbox;
		bboxnum = setno;
	}

	if( deldirty ){
		free( deldirty );
	}

	return 0;
}


int CShdElem::AddBBox( CShdHandler* srclpsh, CShdElem* curobj, CShdElem* curjoint, int* vertarray, int vertnum )
{

	bboxnum++;

	ppbbox = (CBBox2**)realloc( ppbbox, sizeof( CBBox2* ) * bboxnum );
	if( !ppbbox ){
		DbgOut( "selem : AddBBox : ppbbox alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBBox2* newbbx;
	newbbx = new CBBox2();
	if( !newbbx ){
		DbgOut( "selem : AddBBox : newbbx alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*( ppbbox + bboxnum - 1 ) = newbbx;

	int ret;
	ret = newbbx->SetBBox( srclpsh, curobj, curjoint, vertarray, vertnum );
	if( ret ){
		DbgOut( "selem : AddBBox : bbx SetBBox error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CShdElem::AddInfScope( CPolyMesh* pm, CD3DDisp* srcd3ddisp )
{
	if( !pm ){
		_ASSERT( 0 );
		return 1;
	}

	scopenum++;

	ppscope = (CInfScope**)realloc( ppscope, sizeof( CInfScope* ) * scopenum );
	if( !ppscope ){
		DbgOut( "selem : AddInfScope : ppscope alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* newis;
	newis = new CInfScope();
	if( !newis ){
		DbgOut( "selem : AddInfScope : is alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( ppscope + scopenum - 1 ) = newis;

	int ret;
	ret = newis->SetPolyMesh( pm, srcd3ddisp );
	if( ret ){
		DbgOut( "selem : AddInfScope : is SetPolyMesh error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}


int CShdElem::AddInfScope( CPolyMesh2* pm2, CD3DDisp* srcd3ddisp, float srcfacet )
{
	if( !pm2 ){
		_ASSERT( 0 );
		return 1;
	}

	scopenum++;

	ppscope = (CInfScope**)realloc( ppscope, sizeof( CInfScope* ) * scopenum );
	if( !ppscope ){
		DbgOut( "selem : AddInfScope : ppscope alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* newis;
	newis = new CInfScope();
	if( !newis ){
		DbgOut( "selem : AddInfScope : is alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( ppscope + scopenum - 1 ) = newis;

	int ret;
	ret = newis->SetPolyMesh2( pm2, srcd3ddisp, srcfacet );
	if( ret ){
		DbgOut( "selem : AddInfScope : is SetPolyMesh2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::DeleteInfScopeElemByIndex( int isindex )
{

	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : DeleteInfScopeElemByIndex : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope** newppscope;
	newppscope = (CInfScope**)malloc( sizeof( CInfScope* ) * (scopenum - 1) );
	if( !newppscope ){
		DbgOut( "selem : DeleteInfScopeElemByIndex : newppscope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int isno2;
	int setno = 0;
	for( isno2 = 0; isno2 < scopenum; isno2++ ){
		if( isno2 != isindex ){
			*( newppscope + setno ) = *( ppscope + isno2 );
			setno++;
		}
	}

	_ASSERT( setno == (scopenum - 1) );

	delete *( ppscope + isindex );
	free( ppscope );

	ppscope = newppscope;
	scopenum--;

	return 0;
}

int CShdElem::DeleteInfScopeElemByTarget( int targetno )
{

	/***
	CInfScope* curis;
	int delindex;
	int isno;
	int ret;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		_ASSERT( curis );
		if( curis->target && ( curis->target->serialno == targetno ) ){
			delindex = isno;

			ret = DeleteInfScopeElemByIndex( delindex );
			if( ret ){
				DbgOut( "selem : DeleteInfScopeElemByTarget : DeleteInfScopeElemByIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}
	***/
	int ret;
	int delindex = 1;
	while( delindex >= 0 ){
		delindex = ExistInfScopeByTarget( targetno );
		if( delindex >= 0 ){
			ret = DeleteInfScopeElemByIndex( delindex );
			if( ret ){
				DbgOut( "selem : DeleteInfScopeElemByTarget : DeleteInfScopeElemByIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdElem::ExistInfScopeByTarget( int targetno )
{
	CInfScope* curis;
	int delindex = -1;
	int isno;
	//int ret;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		_ASSERT( curis );
		if( curis->target && ( curis->target->serialno == targetno ) ){
			delindex = isno;
			break;
		}
	}
	return delindex;	
}

int CShdElem::DeleteInfScopeElemByApplyChild( int srcchild )
{
	/***
	CInfScope* curis;
	int delindex;
	int isno;
	int ret;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		_ASSERT( curis );
		if( curis->applychild && ( curis->applychild->serialno == srcchild ) ){
			delindex = isno;

			ret = DeleteInfScopeElemByIndex( delindex );
			if( ret ){
				DbgOut( "selem : DeleteInfScopeElemByApplyChild : DeleteInfScopeElemByIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}
	***/
	int ret;
	int delindex = 1;
	while( delindex >= 0 ){
		delindex = ExistInfScopeByApplyChild( srcchild );
		if( delindex >= 0 ){
			ret = DeleteInfScopeElemByIndex( delindex );
			if( ret ){
				DbgOut( "selem : DeleteInfScopeElemByTarget : DeleteInfScopeElemByIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdElem::ExistInfScopeByApplyChild( int srcchild )
{
	CInfScope* curis;
	int delindex = -1;
	int isno;
	//int ret;
	for( isno = 0; isno < scopenum; isno++ ){
		curis = *( ppscope + isno );
		_ASSERT( curis );
		if( curis->applychild && ( curis->applychild->serialno == srcchild ) ){
			delindex = isno;
			break;
		}
	}
	return delindex;	
}


int CShdElem::GetInfScopePointNum( int isindex, int* pointnumptr )
{

	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetInfScopePointNum : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	int ret;
	ret = curis->GetPointNum( pointnumptr );
	if( ret ){
		DbgOut( "selem : GetInfScopePointNum : is GetPointNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CShdElem::GetInfScopePointBuf( int isindex, D3DXVECTOR3* pbuf )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetInfScopePointBuf : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	int ret;
	ret = curis->GetPointBuf( pbuf );
	if( ret ){
		DbgOut( "selem : GetInfScopePointBuf : GetPointBuf error !!!\n" ) ;
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetInfScopeCenter( int isindex, D3DXVECTOR3* centerptr )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetInfScopeCenter : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	int ret;
	ret = curis->GetCenter( centerptr );
	if( ret ){
		DbgOut( "selem : GetInfScopeCenter : GetCenter error !!!\n" ) ;
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::SetInfScopePointBuf( int isindex, D3DXVECTOR3* newp, int vertno )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : SetInfScopePointBuf : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	int ret;
	ret = curis->SetPointBuf( newp, vertno );
	if( ret ){
		DbgOut( "selem : SetInfScopePointBuf : SetPointBuf error !!!\n" ) ;
		_ASSERT( 0 );
		return 1;
	}

	if( g_useGPU != 0 ){
		ret = curis->d3ddisp->Copy2VertexBuffer( 0 );
		_ASSERT( !ret );
		
	}


	return 0;
}

int CShdElem::GetInfScopeDiffuse( int isindex, COLORREF* colptr )
{
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : GetInfScopeDiffuse : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	CPolyMesh* pm;
	CPolyMesh2* pm2;
	DWORD col;

	int r, g, b;
	switch( curis->type ){
	case SHDPOLYMESH:
		pm = curis->polymesh;
		if( pm ){
			_ASSERT( pm->diffusebuf );
			r = (int)( pm->diffusebuf->r * 255.0f );
			g = (int)( pm->diffusebuf->g * 255.0f );
			b = (int)( pm->diffusebuf->b * 255.0f );
			*colptr = RGB( r, g, b );
		}
		break;
	case SHDPOLYMESH2:
		pm2 = curis->polymesh2;
		if( pm2 ){
			_ASSERT( pm2->opttlv );
			col = pm2->opttlv->color;
			r = (col & 0x00FF0000) >> 16; 
			g = (col & 0x0000FF00) >> 8;
			b = col & 0x000000FF;

			*colptr = RGB( r, g, b );
		}
		break;
	default:
		_ASSERT( 0 );
		break;
	}

	return 0;
}

int CShdElem::SetInfScopeDiffuseAmbient( int isindex, COLORREF srccol )
{
	int ret;
	if( (isindex < 0) || (isindex >= scopenum) ){
		DbgOut( "selem : SetInfScopeDiffuse : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( ppscope + isindex );
	_ASSERT( curis );

	CPolyMesh* pm;
	CPolyMesh2* pm2;
	
	ARGBF diffuse;
	diffuse.r = (float)GetRValue( srccol ) / 255.0f;
	diffuse.g = (float)GetGValue( srccol ) / 255.0f;
	diffuse.b = (float)GetBValue( srccol ) / 255.0f;
	ARGBF ambient;
	ambient.r = diffuse.r * 0.25f;
	ambient.g = diffuse.g * 0.25f;
	ambient.b = diffuse.b * 0.25f;


	switch( curis->type ){
	case SHDPOLYMESH:
		pm = curis->polymesh;
		if( pm ){
			ret = pm->SetDiffuse( -1, 0, diffuse, curis->d3ddisp, TLMODE_ORG );
			if( ret ){
				DbgOut( "shdelem : SetInfScopeDiffuse : polymesh SetDiffuse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = pm->SetAmbient( -1, 0, ambient );
			if( ret ){
				DbgOut( "shdelem : SetInfScopeAmbient : polymesh SetAmbient error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( g_useGPU != 0 ){
				D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );
				int srcshader = 0;
				int srcoverflow = 0;
				D3DXMATRIX matworld;
				D3DXMatrixIdentity( &matworld );
				ret = curis->d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, pm, alpha, matworld );
				_ASSERT( !ret );

				ret = curis->d3ddisp->Copy2VertexBuffer( 0 );
				_ASSERT( !ret );
			}

		}
		break;
	case SHDPOLYMESH2:
		pm2 = curis->polymesh2;
		if( pm2 ){
			ret = pm2->SetDiffuse( -1, 0, diffuse, curis->d3ddisp, TLMODE_ORG );
			if( ret ){
				DbgOut( "shdelem : SetInfScopeDiffuse : polymesh2 SetDiffuse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = pm2->SetAmbient( -1, 0, ambient );
			if( ret ){
				DbgOut( "shdelem : SetInfScopeAmbient : polymesh2 SetAmbient error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}		

			if( g_useGPU != 0 ){
				D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );
				int srcshader = 0;
				int srcoverflow = 0;
				D3DXMATRIX matworld;
				D3DXMatrixIdentity( &matworld );
				ret = curis->d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, pm2, alpha, matworld );
				_ASSERT( !ret );

				ret = curis->d3ddisp->Copy2VertexBuffer( 0 );
				_ASSERT( !ret );
			}

		}
		break;
	default:
		_ASSERT( 0 );
		break;
	}

	return 0;
}

int CShdElem::CreateAndSaveTempMaterial()
{
	int ret;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		ret = polymesh->CreateAndSaveTempMaterial();
		if( ret ){
			DbgOut( "selem : CreateAndSaveTempMaterial : pm CreateAndSaveTempMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		ret = polymesh2->CreateAndSaveTempMaterial();
		if( ret ){
			DbgOut( "selem : CreateAndSaveTempMaterial : pm2 CreateAndSaveTempMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

/***
		ret = d3ddisp->SetIndexBuffer( polymesh2->optindexbuf, clockwise );
		if( ret ){
			DbgOut( "selem : CreateAndSaveTempMaterial : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = d3ddisp->Copy2IndexBufferNotCulling();
		if( ret ){
			DbgOut( "selem : CreateAndSaveTempMaterial : Copy2IndexBufferNotCulling error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
***/

	}

	if( (g_useGPU != 0) && d3ddisp ){

		D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );
		int srcshader = 0;
		int srcoverflow = 0;
		D3DXMATRIX mat;
		D3DXMatrixIdentity( &mat );

		switch( type ){
		case SHDPOLYMESH:
			ret = d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh, alpha, mat );
			_ASSERT( !ret );
			break;
		case SHDPOLYMESH2:
			ret = d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh2, alpha, mat );
			_ASSERT( !ret );
			break;
		default:
			break;
		}
		ret = d3ddisp->Copy2VertexBuffer( 0 );
		_ASSERT( !ret );
	}

	return 0;
}
int CShdElem::RestoreAndDestroyTempMaterial()
{
	int ret;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		ret = polymesh->RestoreAndDestroyTempMaterial();
		if( ret ){
			DbgOut( "selem : RestoreAndDestroyTempMaterial : pm RestoreAndDestroyTempMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		ret = polymesh2->RestoreAndDestroyTempMaterial();
		if( ret ){
			DbgOut( "selem : RestoreAndDestroyTempMaterial : pm2 RestoreAndDestroyTempMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( (g_useGPU != 0) && d3ddisp ){
		D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );
		int srcshader = 0;
		int srcoverflow = 0;
		D3DXMATRIX mat;
		D3DXMatrixIdentity( &mat );

		switch( type ){
		case SHDPOLYMESH:
			ret = d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh, alpha, mat );
			_ASSERT( !ret );
			break;
		case SHDPOLYMESH2:
			ret = d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh2, alpha, mat );
			_ASSERT( !ret );
			break;
		default:
			break;
		}
		ret = d3ddisp->Copy2VertexBuffer( 0 );
		_ASSERT( !ret );
	}

	return 0;
}

int CShdElem::SetMaterialFromInfElem( int srcchildno, int srcvert, E3DCOLOR3UC* infcolarray )
{
	int ret;

	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		ret = polymesh->SetInfCol( infcolarray );
		_ASSERT( !ret );
		ret = polymesh->SetMaterialFromInfElem( srcchildno, srcvert );
		if( ret ){
			DbgOut( "selem : SetMaterialFromInfElem : pm SetMaterialFromInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		ret = polymesh2->SetInfCol( infcolarray );
		_ASSERT( !ret );
		ret = polymesh2->SetMaterialFromInfElem( srcchildno, srcvert );
		if( ret ){
			DbgOut( "selem : SetMaterialFromInfElem : pm2 SetMaterialFromInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( (g_useGPU != 0) && d3ddisp ){
		D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );
		int srcshader = 0;
		int srcoverflow = 0;
		D3DXMATRIX mat;
		D3DXMatrixIdentity( &mat );

		switch( type ){
		case SHDPOLYMESH:
			ret = d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh, alpha, mat );
			_ASSERT( !ret );
			break;
		case SHDPOLYMESH2:
			ret = d3ddisp->InitColor( vEyePt, srcshader, srcoverflow, polymesh2, alpha, mat );
			_ASSERT( !ret );
			break;
		default:
			break;
		}
		ret = d3ddisp->Copy2VertexBuffer( 0 );
		_ASSERT( !ret );
	}


	return 0;
}

int CShdElem::SetInfElem( CShdHandler* lpsh, int srcvert, CBoneInfo* biptr, CShdElem* parselem, int paintmode, int calcmode, float rate, int normflag, float directval, int updateflag )
{
	int ret;

	CInfElem* ieptr;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		if( (srcvert < 0) || (srcvert >= polymesh->meshinfo->m) ){
			DbgOut( "selem : SetInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh->m_IE );
		ieptr = polymesh->m_IE + srcvert;

		D3DXVECTOR3 vec;
		vec.x = (polymesh->pointbuf + srcvert)->x;
		vec.y = (polymesh->pointbuf + srcvert)->y;
		vec.z = (polymesh->pointbuf + srcvert)->z;


		ret = ieptr->SetInfElem( biptr, parselem, vec, paintmode, calcmode, rate, normflag, directval );
		if( ret ){//ret 2 は、５個以上の影響度設定をしようとして失敗したとき。
			if( ret != 2 ){
				DbgOut( "selem : SetInfElem : pm : ie SetInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}else{
				return 2;//!!!!!!!!!!!!!!
			}
		}
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		if( (srcvert < 0) || (srcvert >= polymesh2->optpleng) ){
			DbgOut( "selem : SetInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh2->m_IE );
		ieptr = polymesh2->m_IE + srcvert;

		D3DXVECTOR3 vec;
		vec.x = (polymesh2->opttlv + srcvert)->sx;
		vec.y = (polymesh2->opttlv + srcvert)->sy;
		vec.z = (polymesh2->opttlv + srcvert)->sz;


		ret = ieptr->SetInfElem( biptr, parselem, vec, paintmode, calcmode, rate, normflag, directval );
		if( ret ){//ret 2 は、５個以上の影響度設定をしようとして失敗したとき。
			if( ret != 2 ){
				DbgOut( "selem : SetInfElem : pm2 : ie SetInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}else{
				return 2;//!!!!!!!!!!!!!!!!!!!!!!
			}
		}
		
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}

int CShdElem::DeleteInfElem( int srcvert, int srcchildno, int normflag, int updateflag )
{
	int ret;

	CInfElem* ieptr;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		if( (srcvert < 0) || (srcvert >= polymesh->meshinfo->m) ){
			DbgOut( "selem : DeleteInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh->m_IE );
		ieptr = polymesh->m_IE + srcvert;

		ret = ieptr->DeleteInfElem( srcchildno, normflag );
		if( ret ){
			DbgOut( "selem : DeleteInfElem pm : ie DeleteInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		if( (srcvert < 0) || (srcvert >= polymesh2->optpleng) ){
			DbgOut( "selem : DeleteInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh2->m_IE );
		ieptr = polymesh2->m_IE + srcvert;

		ret = ieptr->DeleteInfElem( srcchildno, normflag );
		if( ret ){
			DbgOut( "selem : DeleteInfElem pm2 : ie DeleteInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}

int CShdElem::DestroyIE( int srcvert )
{

	int ret;
	if( srcvert < 0 ){
		int vno;
		if( type == SHDPOLYMESH ){
			for( vno = 0; vno < polymesh->meshinfo->m; vno++ ){
				ret = DestroyIE( vno );
				if( ret ){
					DbgOut( "se : DestroyIE error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}else if( type == SHDPOLYMESH2 ){
			for( vno = 0; vno < polymesh2->optpleng; vno++ ){
				ret = DestroyIE( vno );
				if( ret ){
					DbgOut( "se : DestroyIE error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

		return 0;
	}


////////////////

	CInfElem* ieptr;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		if( (srcvert < 0) || (srcvert >= polymesh->meshinfo->m) ){
			DbgOut( "selem : DestroyIE : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh->m_IE );
		ieptr = polymesh->m_IE + srcvert;

		ret = ieptr->DestroyIE();
		if( ret ){
			DbgOut( "selem : DestroyIE pm : ie DestroyIE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		if( (srcvert < 0) || (srcvert >= polymesh2->optpleng) ){
			DbgOut( "selem : DestroyIE : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh2->m_IE );
		ieptr = polymesh2->m_IE + srcvert;

		ret = ieptr->DestroyIE();
		if( ret ){
			DbgOut( "selem : DestroyIE pm2 : ie DestroyIE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}

	return 0;
}

int CShdElem::AddInfElem( int srcvert, INFELEM srcIE )
{
	int ret;

	CInfElem* ieptr;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		if( (srcvert < 0) || (srcvert >= polymesh->meshinfo->m) ){
			DbgOut( "selem : AddInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh->m_IE );
		ieptr = polymesh->m_IE + srcvert;

		ret = ieptr->AddInfElem( srcIE );
		if( ret ){
			DbgOut( "selem : AddInfElem pm : ie AddInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		if( (srcvert < 0) || (srcvert >= polymesh2->optpleng) ){
			DbgOut( "selem : AddInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh2->m_IE );
		ieptr = polymesh2->m_IE + srcvert;

		ret = ieptr->AddInfElem( srcIE );
		if( ret ){
			DbgOut( "selem : AddInfElem pm2 : ie AddInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}else{
		_ASSERT( 0 );
		return 0;
	}


	return 0;

}
int CShdElem::NormalizeInfElem( int srcvert )
{
	int ret;

	if( srcvert < 0 ){
		int vnum, vno;
		if( type == SHDPOLYMESH ){
			_ASSERT( polymesh );
			vnum = polymesh->meshinfo->m;
		}else if( type == SHDPOLYMESH2 ){
			_ASSERT( polymesh2 );
			vnum = polymesh2->optpleng;
		}else{
			_ASSERT( 0 );
			return 0;
		}

		for( vno = 0; vno < vnum; vno++ ){
			ret = NormalizeInfElem( vno );
			if( ret ){
				DbgOut( "selem : NormalizeInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;
	}



	CInfElem* ieptr;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		if( (srcvert < 0) || (srcvert >= polymesh->meshinfo->m) ){
			DbgOut( "selem : NormalizeInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh->m_IE );
		ieptr = polymesh->m_IE + srcvert;

		ret = ieptr->Normalize();
		if( ret ){
			DbgOut( "selem : NormalizeInfElem pm : ie Normalize error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		if( (srcvert < 0) || (srcvert >= polymesh2->optpleng) ){
			DbgOut( "selem : NormalizeInfElem : srcvert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( polymesh2->m_IE );
		ieptr = polymesh2->m_IE + srcvert;

		ret = ieptr->Normalize();
		if( ret ){
			DbgOut( "selem : NormalizeInfElem pm2 : ie Normalize error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}else{
		_ASSERT( 0 );
		return 0;
	}


	return 0;


}

int CShdElem::GetNearVert( D3DXVECTOR3 findpos, float symdist, int* findvert, float* finddist )
{
	*findvert = -1;
	*finddist = 1e8;

	int ret;
	switch( type ){
	case SHDPOLYMESH:
		_ASSERT( polymesh );
		ret = polymesh->GetNearVert( findpos, symdist, findvert, finddist );
		if( ret ){
			DbgOut( "selem : GetNearVert : pm GetNearVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SHDPOLYMESH2:
		_ASSERT( polymesh2 );
		ret = polymesh2->GetNearVert( findpos, symdist, findvert, finddist );
		if( ret ){
			DbgOut( "selem : GetNearVert : pm GetNearVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		break;
	default:
		break;
	}

	return 0;
}

/***
int CShdElem::ChkMorphElemToAdd( CShdElem* addelem )
{

	if( (type != SHDMORPH) || !morph )
		return 0;//NG

	if( morph->morphnum == 0 ){

		if( (addelem->type == SHDPOLYMESH) || (addelem->type == SHDPOLYMESH2) ){
			return 1;//ok
		}else{
			return 0;//NG
		}

	}else{
		CShdElem* firstelem;
		firstelem = *( morph->hselem );

		if( !firstelem )
			return 0;//NG

		if( firstelem->type != addelem->type ){
			return 0;//NG
		}

		CPolyMesh* firstpm;
		CPolyMesh* addpm;
		CPolyMesh2* firstpm2;
		CPolyMesh2* addpm2;

		switch( firstelem->type ){
		case SHDPOLYMESH:
			firstpm = firstelem->polymesh;
			_ASSERT( firstpm );
			addpm = addelem->polymesh;
			_ASSERT( addpm );

			if( firstpm->meshinfo->m == addpm->meshinfo->m ){
				return 1;//OK
			}else{
				return 0;//NG
			}

			break;
		case SHDPOLYMESH2:
			firstpm2 = firstelem->polymesh2;
			_ASSERT( firstpm2 );
			addpm2 = addelem->polymesh2;
			_ASSERT( addpm2 );

			if( firstpm2->meshinfo->n == addpm2->meshinfo->n ){
				return 1;//OK
			}else{
				return 0;//NG
			}
			break;
		default:
			return 0;//NG
			break;
		}

	}	
}
int CShdElem::AddMorphElem( CShdElem* addelem )
{
	int addok;

	addok = ChkMorphElemToAdd( addelem );
	if( addok == 0 ){
		DbgOut( "selem : AddMorphElem : ChkMorphElemToAdd error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( morph );

	int ret;
	ret = morph->AddMorphElem( addelem );
	if( ret ){
		DbgOut( "selem : AddMorphElem : morph AddMorphElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CShdElem::DeleteMorphElem( CShdElem* delelem )
{

	if( !morph ){
		DbgOut( "selem : DeleteMorphElem : morph NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = morph->DeleteMorphElem( delelem );
	if( ret ){
		DbgOut( "selem : DeleteMorphElem : morph DeleteMorphElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/

int CShdElem::CreateMaterialBlock( LPDIRECT3DDEVICE9 pdev, CShdHandler* lpsh )
{
	int ret;
	D3DXMATERIAL* pmat = 0;
	DWORD* pattrib = 0;
	int matnum = 0;

	if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "selem : CreateToon1Buffer : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->ConvColor2XMaterial( lpsh->m_mathead, &pmat, &pattrib, &matnum );
		if( ret ){
			DbgOut( "selem : CreateToon1Buffer : pm2 ConvColor2XMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh2->CreateToon1Buffer( pdev, pmat, pattrib, matnum, lpsh->m_mathead );
		if( ret ){
			DbgOut( "selem : CreateToon1Buffer : pm2 CreateToon1Buffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH ){

		if( !polymesh ){
			DbgOut( "selem : CreateToon1Buffer : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->ConvColor2XMaterial( lpsh->m_mathead, &pmat, &pattrib, &matnum );
		if( ret ){
			DbgOut( "selem : CreateToon1Buffer : pm ConvColor2XMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = polymesh->CreateToon1Buffer( pdev, pmat, pattrib, matnum, lpsh->m_mathead );
		if( ret ){
			DbgOut( "selem : CreateToon1Buffer : pm CreateToon1Buffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	}else if( type == SHDMORPH ){
		if( morph->m_objtype == SHDPOLYMESH2 ){
			CPolyMesh2* pm2;
			pm2 = morph->m_baseelem->polymesh2;
			_ASSERT( pm2 );

		}else if( morph->m_objtype == SHDPOLYMESH ){
			CPolyMesh* pm;
			pm = morph->m_baseelem->polymesh;
			_ASSERT( pm );

		}
	}


	return 0;
}

int CShdElem::CreateToon1Buffer( CShdHandler* lpsh, LPDIRECT3DDEVICE9 pdev, int forceRemakeflag )
{
	int ret;
	D3DXMATERIAL* pmat = 0;
	DWORD* pattrib = 0;
	int matnum = 0;

	if( type == SHDPOLYMESH2 ){
		if( !polymesh2 ){
			DbgOut( "selem : CreateToon1Buffer : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (polymesh2->m_toon1 == 0) || forceRemakeflag ){
			ret = polymesh2->ConvColor2XMaterial( lpsh->m_mathead, &pmat, &pattrib, &matnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : pm2 ConvColor2XMaterial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = polymesh2->CreateToon1Buffer( pdev, pmat, pattrib, matnum, lpsh->m_mathead );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : pm2 CreateToon1Buffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( !d3ddisp ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->SetIndexBuffer( polymesh2->m_optindexbuf2, clockwise );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->CreateSkinMatToon1( lpsh, polymesh2->m_materialblock, polymesh2->m_materialnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = polymesh2->CreateToon1Buffer2( pdev, lpsh->m_mathead );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : pm2 CreateToon1Buffer2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( !d3ddisp ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->SetIndexBuffer( polymesh2->m_optindexbuf2, clockwise );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->CreateSkinMatToon1( lpsh, polymesh2->m_materialblock, polymesh2->m_materialnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = d3ddisp->Copy2VertexBuffer( 0 );
		if( ret ){
			DbgOut( "shdelem : CreateToon1Buffer : Copy2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = d3ddisp->Copy2IndexBufferNotCulling();
		if( ret ){
			DbgOut( "shdelem : CreateToon1Buffer : Copy2IndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	}else if( type == SHDPOLYMESH ){

		if( !polymesh ){
			DbgOut( "selem : CreateToon1Buffer : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (polymesh->m_toon1 == 0) || forceRemakeflag ){
			ret = polymesh->ConvColor2XMaterial( lpsh->m_mathead, &pmat, &pattrib, &matnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : pm ConvColor2XMaterial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = polymesh->CreateToon1Buffer( pdev, pmat, pattrib, matnum, lpsh->m_mathead );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : pm CreateToon1Buffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( !d3ddisp ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->SetIndexBuffer( polymesh->m_indexbuf2, clockwise );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->CreateSkinMatToon1( lpsh, polymesh->m_materialblock, polymesh->m_materialnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = polymesh->CreateToon1Buffer2( pdev, lpsh->m_mathead );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : pm CreateToon1Buffer2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( !d3ddisp ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->SetIndexBuffer( polymesh->m_indexbuf2, clockwise );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->CreateSkinMatToon1( lpsh, polymesh->m_materialblock, polymesh->m_materialnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = d3ddisp->Copy2VertexBuffer( 0 );
		if( ret ){
			DbgOut( "shdelem : CreateToon1Buffer : Copy2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = d3ddisp->Copy2IndexBufferNotCulling();
		if( ret ){
			DbgOut( "shdelem : CreateToon1Buffer : Copy2IndexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDMORPH ){
		if( morph->m_objtype == SHDPOLYMESH2 ){
			CPolyMesh2* pm2;
			pm2 = morph->m_baseelem->polymesh2;
			_ASSERT( pm2 );

			ret = d3ddisp->SetIndexBuffer( pm2->m_optindexbuf2m, morph->m_baseelem->clockwise );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->CreateMorphSkinMatToon1( lpsh, pm2, pm2->m_materialblock, pm2->m_materialnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->Copy2VertexBufferMorph( pm2, 1 );
			if( ret ){
				DbgOut( "shdelem : CreateToon1Buffer : Copy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->Copy2IndexBufferNotCulling();
			if( ret ){
				DbgOut( "shdelem : CreateToon1Buffer : Copy2IndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


		}else if( morph->m_objtype == SHDPOLYMESH ){
			CPolyMesh* pm;
			pm = morph->m_baseelem->polymesh;
			_ASSERT( pm );

			ret = d3ddisp->SetIndexBuffer( pm->m_indexbuf2, morph->m_baseelem->clockwise );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp SetAndUpdateIndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->CreateSkinMatToon1( lpsh, pm->m_materialblock, pm->m_materialnum );
			if( ret ){
				DbgOut( "selem : CreateToon1Buffer : d3ddisp CreateSkinMatToon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->Copy2VertexBuffer( 0 );
			if( ret ){
				DbgOut( "shdelem : CreateToon1Buffer : Copy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3ddisp->Copy2IndexBufferNotCulling();
			if( ret ){
				DbgOut( "shdelem : CreateToon1Buffer : Copy2IndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}


	return 0;
}


int CShdElem::MakeXSeri2InfBno( int serinum, int* seri2infbno, int* bonenumptr )
{
	CInfElem* ieptr;
	int meshvnum;

	switch( type ){
	case SHDPOLYMESH:
		if( !polymesh ){
			DbgOut( "selem : MakeXSeri2InfBno : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = polymesh->m_IE;
		meshvnum = polymesh->meshinfo->m;
		break;
	case SHDPOLYMESH2:
		if( !polymesh2 ){
			DbgOut( "selem : MakeXSeri2InfBno : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = polymesh2->m_IE;
		meshvnum = polymesh2->optpleng;
		break;
	default:
		ieptr = 0;
		break;
	}

	if( !ieptr ){
		DbgOut( "selem : MakeXSeri2InfBno : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	int* dirtyflag;
	dirtyflag = (int*)malloc( sizeof( int ) * serinum );
	if( !dirtyflag ){
		DbgOut( "selem : MakeXSeri2InfBno : dirtyrflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( dirtyflag, sizeof( int ) * serinum );


	int vno, infno, matno;
	CInfElem* curie;
	INFELEM* IE;
	for( vno = 0; vno < meshvnum; vno++ ){
		curie = ieptr + vno;
		
		for( infno = 0; infno < curie->infnum; infno++ ){
			IE = curie->ie + infno;
			matno = IE->bonematno;
			if( matno >= 0 )//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 0は入れるかどうか！！！
				*(dirtyflag + matno) = 1;
		}
	}

	// infbno == 0 は、先頭フレームを予約！！！！
	*(dirtyflag + 0) = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	int setno = 0;
	int serino;
	for( serino = 0; serino < serinum; serino++ ){
		if( *(dirtyflag + serino) == 1 ){

			*(seri2infbno + serino) = setno;
			setno++;
		}else{
			*(seri2infbno + serino) = -1;
		}
	}

	*bonenumptr = setno;

	free( dirtyflag );

	return 0;
}

int CShdElem::MakeXBoneInfluence( int serinum, int bonenum, int* seri2infbno, BONEINFLUENCE* biptr )
{
	CInfElem* ieptr;
	int meshvnum;
	int ret;

	switch( type ){
	case SHDPOLYMESH:
		if( !polymesh ){
			DbgOut( "selem : MakeXBoneInfluence : polymesh NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = polymesh->m_IE;
		meshvnum = polymesh->meshinfo->m;
		break;
	case SHDPOLYMESH2:
		if( !polymesh2 ){
			DbgOut( "selem : MakeXBoneInfluence : polymesh2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = polymesh2->m_IE;
		meshvnum = polymesh2->optpleng;
		break;
	default:
		ieptr = 0;
		break;
	}

	if( !ieptr ){
		DbgOut( "selem : MakeXBoneInfluence : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int serino, curserino;
	int curboneno;
	BONEINFLUENCE* curbi;
	DWORD infnum;
	for( serino = 0; serino < serinum; serino++ ){
		
		curbi = biptr + serino;

		curboneno = *(seri2infbno + serino);


		if( serino == 0 )
			curserino = -1;//!!!!!!!!!!!!!
		else
			curserino = serino;

		if( curboneno >= 0 ){
			

	//// bonenoの影響を受ける頂点の数を取得。
			ret = SetXInfluenceArray( ieptr, meshvnum, curserino, 0, 0, 0, &infnum );
			if( ret ){
				DbgOut( "selem : MakeXBoneInfluence : SetXInfluenceArray error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			DWORD* vertices;
			float* weights;

			if( infnum > 0 ){

			//// 配列作成


				vertices = (DWORD*)malloc( sizeof( DWORD ) * infnum );
				if( !vertices ){
					DbgOut( "selem : MakeXBoneInfluence : vertices alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				weights = (float*)malloc( sizeof( float ) * infnum );
				if( !weights ){
					DbgOut( "selem : MakeXBoneInfluence : weights alloc error !!!\n" );
					_ASSERT( 0 );
					free( vertices );
					return 1;
				}

				//// 情報セット
				DWORD setnum;
				ret = SetXInfluenceArray( ieptr, meshvnum, curserino, vertices, weights, infnum, &setnum );
				if( ret ){
					DbgOut( "selem : MakeXBoneInfluence : SetXInfluenceArray 1 error !!!\n" );
					_ASSERT( 0 );
					free( vertices );
					free( weights );
					return 1;
				}

			}else{

				vertices = (DWORD*)malloc( sizeof( DWORD ) * 1 );
				if( !vertices ){
					DbgOut( "selem : MakeXBoneInfluence : vertices alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				weights = (float*)malloc( sizeof( float ) * 1 );
				if( !weights ){
					DbgOut( "selem : MakeXBoneInfluence : weights alloc error !!!\n" );
					_ASSERT( 0 );
					free( vertices );
					return 1;
				}

				*vertices = 0;
				*weights = 0;
			}


		/// boneinfluenceにセット
			curbi->Bone = curboneno;

			curbi->numInfluences = infnum;

			curbi->vertices = vertices;
			curbi->weights = weights;

	
		}else{
			curbi->Bone = -1;
			curbi->numInfluences = 0;
			curbi->vertices = NULL;
			curbi->weights = NULL;
		}
	}	

	return 0;
}


int CShdElem::SetXInfluenceArray( CInfElem* ieptr, int vnum, int boneserino, DWORD* vertices, float* weights, int infnum, DWORD* setnumptr )
{
	int mode;

	if( vertices ){
		mode = 1;// 情報をセットする。
	}else{
		mode = 0;// setnumをカウントするだけ。
	}

	*setnumptr = 0;


	int setno = 0;
	int vno, infno;
	CInfElem* curie = ieptr;
	INFELEM* IE;
	for( vno = 0; vno < vnum; vno++ ){
		for( infno = 0; infno < curie->infnum; infno++ ){
			IE = curie->ie + infno;

			if( IE->bonematno == boneserino ){
				if( mode == 1 ){
					if( setno >= infnum ){
						DbgOut( "selem : SetXInfluenceArray : infnum error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					*(vertices + setno) = (DWORD)vno;
					*(weights + setno) = IE->dispinf;
				}
				setno++;
			}

		}

		curie++;
	}

	*setnumptr = setno;

	return 0;
}

int CShdElem::RepairInfElem( CShdHandler* srclpsh, LPDIRECT3DDEVICE9 pdev )
{

/***
	int ret;
	CInfElem* ieptr = 0;
	int vertnum;

	if( type == SHDPOLYMESH ){
		if( polymesh ){
			ieptr = polymesh->m_IE;
			vertnum = polymesh->meshinfo->m;
		}
	}else if( type == SHDPOLYMESH2 ){
		if( polymesh2 ){
			ieptr = polymesh2->m_IE;
			vertnum = polymesh2->optpleng;
		}
	}


	if( ieptr ){
		int vertno;
		CInfElem* curie;
		for( vertno = 0; vertno < vertnum; vertno++ ){
			curie = ieptr + vertno;

			int infno;
			INFELEM* curIE;
			for( infno = 0; infno < curie->infnum; infno++ ){
				curIE = curie->ie + infno;

				if( (curIE->childno > 0) && (curIE->bonematno != curIE->childno) ){
					curIE->bonematno = curIE->childno;
				}
			}
		}
	

		if( srclpsh->m_shader == COL_TOON1 ){
			ret = CreateToon1Buffer( srclpsh, pdev );
			if( ret ){
				DbgOut( "se : RepairInfElem : CreateToon1Buffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			if( (g_useGPU != 0) && d3ddisp ){
				ret = d3ddisp->CreateSkinMat( srclpsh );
				if( ret ){
					DbgOut( "se : RepairInfElem : d3ddisp CreateSkinMat error !!!\n" );
					return 1;
				}
			}
		}
	}
***/
	return 0;
}

int CShdElem::InitBBox()
{
	
	int ret;
	if( type == SHDPOLYMESH ){
		ret = m_bbx0.SetBBox( polymesh );
		if( ret ){
			DbgOut( "se : InitBBox : bbx SetBBox pm error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( SHDPOLYMESH2 ){
		ret = m_bbx0.SetBBox( polymesh2 );
		if( ret ){
			DbgOut( "se : InitBBox : bbx SetBBox pm2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int CShdElem::SetBBox( CShdElem* srcbbxelem, D3DXMATRIX matWorld )
{
	
	m_bbx1.InitParams();

	int isfirst = 1;

	if( srcbbxelem ){
		int bbxno;
		CBBox2* curbbx;
		for( bbxno = 0; bbxno < srcbbxelem->bboxnum; bbxno++ ){
			curbbx = *( srcbbxelem->ppbbox + bbxno );
			_ASSERT( curbbx );


			if( curbbx->target == this ){
				if( isfirst ){
					m_bbx1.SetBBox( curbbx );
					isfirst = 0;
				}else{
					m_bbx1.JoinBBox( curbbx );
				}
			}
		}
	}


	if( isfirst == 1 ){
		//ボーンが無い場合は（対応するCBBox2が無い場合は）、
		//m_bbx0にmatWorldを施したものを、m_bbx1とする。

		D3DXMATRIX swmat;
		if( d3ddisp ){
			swmat = d3ddisp->m_scalemat * matWorld;
		}else{
			swmat = matWorld;
		}

		m_bbx1.TransformOnlyWorld( &m_bbx0, &swmat );

	}


	return 0;
}


int CShdElem::CreateSkinMat( CShdHandler* lpsh, int updateflag )
{
	if( g_useGPU == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!
	}

	if( !d3ddisp ){
		return 0;//!!!!!!!!!
	}


	int ret;

	if( (type == SHDPOLYMESH) || (type == SHDPOLYMESH2) ){
		if( lpsh->m_shader != COL_TOON1 ){
			if( polymesh2 ){
				ret = d3ddisp->CreateSkinMatTex( lpsh, polymesh2->m_texblock, polymesh2->m_texblocknum );
				if( ret ){
					DbgOut( "CShdElem : CreateSkinMat : d3ddisp CreateSkinMatTex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( polymesh ){
				if( polymesh->billboardflag != 0 ){
					ret = d3ddisp->CreateSkinMat( lpsh );
					if( ret ){
						DbgOut( "CShdElem : CreateDPolyMesh : d3ddisp CreateSkinMat error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					ret = d3ddisp->CreateSkinMatTex( lpsh, polymesh->m_texblock, polymesh->m_texblocknum );
					//ret1 = d3ddisp->CreateSkinMat( lpsh );
					if( ret ){
						DbgOut( "CShdElem : CreateDPolyMesh : d3ddisp CreateSkinMatTex error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}else{
			if( polymesh2 ){
				ret = d3ddisp->CreateSkinMatToon1( lpsh, polymesh2->m_materialblock, polymesh2->m_materialnum );
				if( ret ){
					DbgOut( "shdelem : CreateSkinMat : d3ddisp CreateSkinMatToon1 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( polymesh ){
				ret = d3ddisp->CreateSkinMatToon1( lpsh, polymesh->m_materialblock, polymesh->m_materialnum );
				if( ret ){
					DbgOut( "shdelem : CreateSkinMat : d3ddisp CreateSkinMatToon1 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}



		if( updateflag != 0 ){
			ret = d3ddisp->Copy2VertexBuffer( 0 );
			if( ret ){
				DbgOut( "selem : CreateSkinMat : d3ddisp Copy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = d3ddisp->Copy2IndexBufferNotCulling();
			if( ret ){
				DbgOut( "selem : CreateSkinMat : d3ddisp Copy2IndexBUfferNotCulling error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else if( type == SHDMORPH ){
		if( morph->m_objtype == SHDPOLYMESH ){
			CPolyMesh* pm;
			pm = morph->m_baseelem->polymesh;
			_ASSERT( pm );

			if( lpsh->m_shader != COL_TOON1 ){
				ret = d3ddisp->CreateSkinMatTex( lpsh, pm->m_texblock, pm->m_texblocknum );
				if( ret ){
					DbgOut( "CShdElem : CreateDPolyMesh : d3ddisp CreateSkinMatTex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				ret = d3ddisp->CreateSkinMatToon1( lpsh, pm->m_materialblock, pm->m_materialnum );
				if( ret ){
					DbgOut( "shdelem : CreateSkinMat : d3ddisp CreateSkinMatToon1 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( updateflag != 0 ){
				ret = d3ddisp->Copy2VertexBuffer( 0 );
				if( ret ){
					DbgOut( "selem : CreateSkinMat : d3ddisp Copy2VertexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = d3ddisp->Copy2IndexBufferNotCulling();
				if( ret ){
					DbgOut( "selem : CreateSkinMat : d3ddisp Copy2IndexBUfferNotCulling error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		}else if( morph->m_objtype == SHDPOLYMESH2 ){
			CPolyMesh2* pm2;
			pm2 = morph->m_baseelem->polymesh2;
			_ASSERT( pm2 );

			if( lpsh->m_shader != COL_TOON1 ){
				ret = d3ddisp->CreateMorphSkinMatTex( lpsh, pm2, pm2->m_texblock, pm2->m_texblocknum );
				if( ret ){
					DbgOut( "CShdElem : CreateSkinMat : d3ddisp CreateSkinMatTex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				ret = d3ddisp->CreateMorphSkinMatToon1( lpsh, pm2, pm2->m_materialblock, pm2->m_materialnum );
				if( ret ){
					DbgOut( "shdelem : CreateSkinMat : d3ddisp CreateSkinMatToon1 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
			if( updateflag != 0 ){
				ret = d3ddisp->Copy2VertexBufferMorph( pm2, 1 );
				if( ret ){
					DbgOut( "selem : CreateSkinMat : d3ddisp Copy2VertexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = d3ddisp->Copy2IndexBufferNotCulling();
				if( ret ){
					DbgOut( "selem : CreateSkinMat : d3ddisp Copy2IndexBUfferNotCulling error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		}
	}

	return 0;
}

int CShdElem::GetToon1Ptr( int matno, CToon1Params** pptoon1 )
{
	int toon1num = 0;
	CToon1Params* toon1ptr = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( !toon1ptr ){
		*pptoon1 = 0;
		return 0;
	}

	if( (matno < 0) || (matno >= toon1num) ){
		*pptoon1 = 0;
		return 0;
	}

	*pptoon1 = toon1ptr + matno;

	return 0;
}
int CShdElem::GetToon1Ptr( char* matname, CToon1Params** pptoon1 )
{
	*pptoon1 = 0;

	int ret;
	int matno = -1;
	ret = GetToon1MaterialNoByName( matname, &matno );
	if( ret || (matno < 0) ){
		return 0;
	}

	ret = GetToon1Ptr( matno, pptoon1 );
	_ASSERT( !ret );

	return 0;
}


int CShdElem::GetToon1MaterialNoByName( char* matname, int* matnoptr )
{
	*matnoptr = -1;//!!!!!!!!!!

	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( !toon1ptr || (toon1num <= 0) ){
		return 0;
	}

	int findno = -1;
	int matno;
	for( matno = 0; matno < toon1num; matno++ ){
		CToon1Params* curtoon1;
		curtoon1 = toon1ptr + matno;
		int cmp;
		cmp = strcmp( matname, curtoon1->name );
		if( cmp == 0 ){
			findno = matno;
			break;
		}
	}

	*matnoptr = findno;

	return 0;
}

int CShdElem::SetToon1Name( char* oldname, char* newname )
{
	if( !oldname || !newname ){
		return 0;
	}

	int ret;
	CToon1Params* toon1;
	ret = GetToon1Ptr( oldname, &toon1 );
	if( ret || !toon1 ){
		_ASSERT( 0 );
		return 1;
	}

	int newleng;
	newleng = (int)strlen( newname );
	if( newleng >= 32 ){
		DbgOut( "se : SetToon1Name : newname too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( toon1->name, 32, newname );
	
	return 0;
}


int CShdElem::SetToon1Diffuse( char* matname, RDBColor3f diffuse )
{
	int ret;
	CToon1Params* toon1;
	ret = GetToon1Ptr( matname, &toon1 );
	if( ret || !toon1 ){
		_ASSERT( 0 );
		return 0;
	}

	toon1->diffuse.r = diffuse.r;
	toon1->diffuse.g = diffuse.g;
	toon1->diffuse.b = diffuse.b;


	return 0;
}
int CShdElem::SetToon1Specular( char* matname, RDBColor3f specular )
{
	int ret;
	CToon1Params* toon1;
	ret = GetToon1Ptr( matname, &toon1 );
	if( ret || !toon1 ){
		_ASSERT( 0 );
		return 0;
	}

	toon1->specular.r = specular.r;
	toon1->specular.g = specular.g;
	toon1->specular.b = specular.b;

	return 0;
}
int CShdElem::SetToon1Ambient( char* matname, RDBColor3f ambient )
{
	int ret;
	CToon1Params* toon1;
	ret = GetToon1Ptr( matname, &toon1 );
	if( ret || !toon1 ){
		_ASSERT( 0 );
		return 0;
	}

	toon1->ambient.r = ambient.r;
	toon1->ambient.g = ambient.g;
	toon1->ambient.b = ambient.b;


	return 0;
}
int CShdElem::SetToon1NL( char* matname, float darknl, float brightnl )
{
	int ret;
	CToon1Params* toon1;
	ret = GetToon1Ptr( matname, &toon1 );
	if( ret || !toon1 ){
		_ASSERT( 0 );
		return 0;
	}

	toon1->toon0dnl = darknl;
	toon1->toon0bnl = brightnl;

	return 0;
}
int CShdElem::SetToon1Edge0( char* matname, RDBColor3f col, int validflag, int invflag, float width )
{
	int ret;
	CToon1Params* toon1;
	ret = GetToon1Ptr( matname, &toon1 );
	if( ret || !toon1 ){
		_ASSERT( 0 );
		return 0;
	}

	toon1->edgecol0.r = col.r;
	toon1->edgecol0.g = col.g;
	toon1->edgecol0.b = col.b;

	toon1->edgevalid0 = (char)validflag;
	toon1->edgeinv0 = (char)invflag;

	toon1->edgecol0.a = width;

	return 0;
}


int CShdElem::GetToon1MaterialNum( int* numptr )
{

	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( toon1ptr && (toon1num > 0) ){
		*numptr = toon1num;
	}else{
		*numptr = 0;
	}

	return 0;
}

int CShdElem::GetToon1Material( E3DTOON1MATERIAL* infoptr, int arrayleng, int* getnumptr )
{
	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	int setno = 0;

	if( toon1ptr && (toon1num > 0) ){

		int no;
		for( no = 0; no < toon1num; no++ ){
			if( setno >= arrayleng ){
				DbgOut( "se : GetToon1Material : array too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			strcpy_s( (infoptr + setno)->name, 32, (toon1ptr + no)->name );

			(infoptr + setno)->ambient.r = (toon1ptr + no)->ambient.r;
			(infoptr + setno)->ambient.g = (toon1ptr + no)->ambient.g;
			(infoptr + setno)->ambient.b = (toon1ptr + no)->ambient.b;

			(infoptr + setno)->diffuse.r = (toon1ptr + no)->diffuse.r;
			(infoptr + setno)->diffuse.g = (toon1ptr + no)->diffuse.g;
			(infoptr + setno)->diffuse.b = (toon1ptr + no)->diffuse.b;

			(infoptr + setno)->specular.r = (toon1ptr + no)->specular.r;
			(infoptr + setno)->specular.g = (toon1ptr + no)->specular.g;
			(infoptr + setno)->specular.b = (toon1ptr + no)->specular.b;

			(infoptr + setno)->darknl = (toon1ptr + no)->toon0dnl;
			(infoptr + setno)->brightnl = (toon1ptr + no)->toon0bnl;

			(infoptr + setno)->edgecol0.r = (toon1ptr + no)->edgecol0.r;
			(infoptr + setno)->edgecol0.g = (toon1ptr + no)->edgecol0.g;
			(infoptr + setno)->edgecol0.b = (toon1ptr + no)->edgecol0.b;

			(infoptr + setno)->edgevalid0 = (toon1ptr + no)->edgevalid0;
			(infoptr + setno)->edgeinv0 = (toon1ptr + no)->edgeinv0;
			(infoptr + setno)->edgewidth0 = (toon1ptr + no)->edgecol0.a;

			setno++;
		}	
	}

	*getnumptr = setno;


	return 0;
}


int CShdElem::GetToon1MaterialInfo( TOON1MATERIALINFO* infoptr, int arrayleng, int* getnumptr )
{
	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	int setno = 0;

	if( toon1ptr && (toon1num > 0) ){

		int no;
		for( no = 0; no < toon1num; no++ ){
			if( setno >= arrayleng ){
				DbgOut( "se : GetToon1MaterialInfo : array too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			strcpy_s( (infoptr + setno)->name, 32, (toon1ptr + no)->name );

			(infoptr + setno)->ambient.a = 255;
			(infoptr + setno)->ambient.r = (unsigned char)( (toon1ptr + no)->ambient.r * 255.0f );
			(infoptr + setno)->ambient.g = (unsigned char)( (toon1ptr + no)->ambient.g * 255.0f );
			(infoptr + setno)->ambient.b = (unsigned char)( (toon1ptr + no)->ambient.b * 255.0f );

			(infoptr + setno)->diffuse.a = 255;
			(infoptr + setno)->diffuse.r = (unsigned char)( (toon1ptr + no)->diffuse.r * 255.0f );
			(infoptr + setno)->diffuse.g = (unsigned char)( (toon1ptr + no)->diffuse.g * 255.0f );
			(infoptr + setno)->diffuse.b = (unsigned char)( (toon1ptr + no)->diffuse.b * 255.0f );

			(infoptr + setno)->specular.a = 255;
			(infoptr + setno)->specular.r = (unsigned char)( (toon1ptr + no)->specular.r * 255.0f );
			(infoptr + setno)->specular.g = (unsigned char)( (toon1ptr + no)->specular.g * 255.0f );
			(infoptr + setno)->specular.b = (unsigned char)( (toon1ptr + no)->specular.b * 255.0f );

			(infoptr + setno)->edgevalid = (toon1ptr + no)->edgevalid0;

			(infoptr + setno)->edgecol0.a = 255;
			(infoptr + setno)->edgecol0.r = (unsigned char)( (toon1ptr + no)->edgecol0.r * 255.0f );
			(infoptr + setno)->edgecol0.g = (unsigned char)( (toon1ptr + no)->edgecol0.g * 255.0f );
			(infoptr + setno)->edgecol0.b = (unsigned char)( (toon1ptr + no)->edgecol0.b * 255.0f );
			
			(infoptr + setno)->edgewidth = (toon1ptr + no)->edgecol0.a;

			setno++;
		}	
	}

	*getnumptr = setno;


	return 0;
}

int CShdElem::EnableToonEdge( int srctype, int srcflag )
{

	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( toon1ptr && (toon1num > 0) ){
		int no;
		for( no = 0; no < toon1num; no++ ){
			if( srcflag ){
				(toon1ptr + no)->edgevalid0 = 1;	
			}else{
				(toon1ptr + no)->edgevalid0 = 0;
			}
		}
	}

	return 0;
}

int CShdElem::SetToonEdge0Color( char* srcname, int srcr, int srcg, int srcb )
{
	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( toon1ptr && (toon1num > 0) ){
		int no;
		CToon1Params* curtoon1;
		for( no = 0; no < toon1num; no++ ){
			curtoon1 = toon1ptr + no;
			
			int cmp0;
			if( srcname ){
				cmp0 = strcmp( curtoon1->name, srcname );
			}else{
				cmp0 = 0;
			}
			
			if( cmp0 == 0 ){
				float fr, fg, fb;
				fr = (float)srcr / 255.0f;
				max( fr, 0.0f );
				min( fr, 1.0f );

				fg = (float)srcg / 255.0f;
				max( fg, 0.0f );
				min( fg, 1.0f );

				fb = (float)srcb / 255.0f;
				max( fb, 0.0f );
				min( fb, 1.0f );
				
				curtoon1->edgecol0.r = fr;
				curtoon1->edgecol0.g = fg;
				curtoon1->edgecol0.b = fb;
			}

		}
	}

	return 0;
}
int CShdElem::SetToonEdge0Width( char* srcname, float srcwidth )
{
	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( toon1ptr && (toon1num > 0) ){
		int no;
		CToon1Params* curtoon1;
		for( no = 0; no < toon1num; no++ ){
			curtoon1 = toon1ptr + no;
			
			int cmp0;
			if( srcname ){
				cmp0 = strcmp( curtoon1->name, srcname );
			}else{
				cmp0 = 0;
			}
			
			if( cmp0 == 0 ){				
				curtoon1->edgecol0.a = srcwidth;
			}

		}
	}

	return 0;
}

int CShdElem::GetToonEdge0Color( char* srcname, float* dstr, float* dstg, float* dstb )
{
	_ASSERT( srcname );

	*dstr = 0.0f;
	*dstg = 0.0f;
	*dstb = 0.0f;

	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( toon1ptr && (toon1num > 0) ){
		int no;
		CToon1Params* curtoon1;
		for( no = 0; no < toon1num; no++ ){
			curtoon1 = toon1ptr + no;
			int cmp0;			
			cmp0 = strcmp( curtoon1->name, srcname );
			
			if( cmp0 == 0 ){
				*dstr = curtoon1->edgecol0.r;
				*dstg = curtoon1->edgecol0.g;
				*dstb = curtoon1->edgecol0.b;
				break;
			}

		}
	}

	return 0;
}
int CShdElem::GetToonEdge0Width( char* srcname, float* dstwidth )
{
	_ASSERT( srcname );

	*dstwidth = 0.0f;

	CToon1Params* toon1ptr = 0;
	int toon1num = 0;
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		toon1ptr = polymesh->m_toon1;
		toon1num = polymesh->m_materialnum;
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		toon1ptr = polymesh2->m_toon1;
		toon1num = polymesh2->m_materialnum;
	}

	if( toon1ptr && (toon1num > 0) ){
		int no;
		CToon1Params* curtoon1;
		for( no = 0; no < toon1num; no++ ){
			curtoon1 = toon1ptr + no;
			
			int cmp0;
			cmp0 = strcmp( curtoon1->name, srcname );
			
			if( cmp0 == 0 ){
				*dstwidth = curtoon1->edgecol0.a;
				break;
			}

		}
	}

	return 0;
}

int CShdElem::InitParticleParams()
{
	if( (type != SHDBILLBOARD) || !billboard ){
		DbgOut( "shdelem : InitParticleParams : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_particleflag = 1;//!!!!!!

	billboard->InitParticleParams0();

	return 0;
}

int CShdElem::SetIndexBufColP()
{

	int ret;

	if( d3ddisp ){
		if( type == SHDPOLYMESH ){

			if( polymesh->billboardflag == 0 ){
				ret = d3ddisp->SetIndexBuffer( polymesh->m_optindexbuftex, clockwise );
				if( ret ){
					DbgOut( "selem : SetIndexBufColP : d3ddisp SetIndexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				ret = d3ddisp->SetIndexBuffer( polymesh->indexbuf, clockwise );
				if( ret ){
					DbgOut( "selem : SetIndexBufColP : d3ddisp SetIndexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		}else if( type == SHDPOLYMESH2 ){
			ret = d3ddisp->SetIndexBuffer( polymesh2->m_optindexbuftex, clockwise );
			if( ret ){
				DbgOut( "selem : SetIndexBufColP : d3ddisp SetIndexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( type == SHDMORPH ){
			if( morph->m_objtype == SHDPOLYMESH ){
				ret = d3ddisp->SetIndexBuffer( morph->m_baseelem->polymesh->m_optindexbuftex, morph->m_baseelem->clockwise );
				if( ret ){
					DbgOut( "selem : SetIndexBufColP : d3ddisp SetIndexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else if( morph->m_objtype == SHDPOLYMESH2 ){
				ret = d3ddisp->SetIndexBuffer( morph->m_baseelem->polymesh2->m_optindexbuftexm, morph->m_baseelem->clockwise );
				if( ret ){
					DbgOut( "selem : SetIndexBufColP : d3ddisp SetIndexBuffer error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

		ret = d3ddisp->Copy2IndexBufferNotCulling();
		if( ret ){
			DbgOut( "selem : SetIndexBufColP : Copy2IndexBufferNotCulling error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		//_ASSERT( 0 );
	}

	return 0;
}

int CShdElem::GetOrgCenter( D3DXVECTOR3* dstcenter )
{
	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		*dstcenter = polymesh->m_center;

	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		*dstcenter = polymesh2->m_center;

	}else{
		dstcenter->x = 0.0f;
		dstcenter->y = 0.0f;
		dstcenter->z = 0.0f;
	}

	return 0;
}

int CShdElem::GetFaceNormal( int faceno, D3DXMATRIX matWorld, CMotHandler* lpmh, D3DXVECTOR3* dstn )
{

	if( !d3ddisp ){
		DbgOut( "se : GetFaceNormal : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = d3ddisp->GetFaceNormal( faceno, matWorld, lpmh, dstn );
	if( ret ){
		DbgOut( "se : GetFaceNormal : d3ddisp GetFaceNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetScaleMat( D3DXMATRIX* dstmat )
{

	if( d3ddisp ){
		*dstmat = d3ddisp->m_scalemat;
	}else{
		D3DXMatrixIdentity( dstmat );
	}

	return 0;
}

int CShdElem::ChkAlphaNum( CMQOMaterial* srcmat )
{
	int ret;
	if( polymesh ){
		ret = polymesh->ChkAlphaNum( srcmat );
		_ASSERT( !ret );
	}else if( polymesh2 ){
		ret = polymesh2->ChkAlphaNum( srcmat );
		_ASSERT( !ret );
	}

	return 0;
}

int CShdElem::GetFirstMaterialNo( int* matnoptr )
{
	*matnoptr = -3;

	if( type == SHDPOLYMESH ){
		*matnoptr = *polymesh->m_attrib0;
	}else if( type == SHDPOLYMESH2 ){
		*matnoptr = *polymesh2->m_attrib0;
	}

	return 0;
}

int CShdElem::GetMaterialNo( int faceno, int* matnoptr )
{
	int facenum;

	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		facenum = polymesh->meshinfo->n;
		if( (faceno < 0) || (faceno >= facenum) ){
			DbgOut( "se : GetMaterialNo : faceno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*matnoptr = *( polymesh->m_attrib0 + faceno );

	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		facenum = polymesh2->meshinfo->n;
		if( (faceno < 0) || (faceno >= facenum) ){
			DbgOut( "se : GetMaterialNo : faceno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*matnoptr = *( polymesh2->m_attrib0 + faceno );

	}else{
		DbgOut( "se : GetMaterialNo : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::GetMaterialNo2( int srcshader, int faceno, int* matnoptr )
{
	int facenum;

	if( type == SHDPOLYMESH ){
		_ASSERT( polymesh );
		facenum = polymesh->meshinfo->n;
		if( (faceno < 0) || (faceno >= facenum) ){
			DbgOut( "se : GetMaterialNo2 : faceno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( srcshader != COL_TOON1 ){
			SORTTEXTURE* curst;
			curst = polymesh->m_sorttex + faceno;
			int oldfaceno;
			oldfaceno = curst->faceno;
			*matnoptr = *( polymesh->m_attrib0 + oldfaceno );
		}else{
			int oldfaceno;
			oldfaceno = *( polymesh->m_toonface2oldface + faceno );
			*matnoptr = *( polymesh->m_attrib0 + oldfaceno );
		}

	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( polymesh2 );
		facenum = polymesh2->meshinfo->n;
		if( (faceno < 0) || (faceno >= facenum) ){
			DbgOut( "se : GetMaterialNo2 : faceno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( srcshader != COL_TOON1 ){
			SORTTEXTURE* curst;
			curst = polymesh2->m_sorttex + faceno;
			int oldfaceno;
			oldfaceno = curst->faceno;
			*matnoptr = *( polymesh2->m_attrib0 + oldfaceno );
		}else{
			int oldfaceno;
			oldfaceno = *( polymesh2->m_toonface2oldface + faceno );
			*matnoptr = *( polymesh2->m_attrib0 + oldfaceno );
		}

	}else{
		DbgOut( "se : GetMaterialNo2 : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::SetBoneLim01( DVEC3* srclim0, DVEC3* srclim1 )
{
	if( !part ){
		DbgOut( "se : SetBoneLim01 : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part->m_lim0 = *srclim0;
	part->m_lim1 = *srclim1;

	return 0;
}
int CShdElem::SetIgnoreLim( int srcignore )
{
	if( !part ){
		DbgOut( "se : SetIgonoreLim : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part->m_ignorelim01 = srcignore;

	return 0;
}
int CShdElem::GetBoneLim01( DVEC3* dstlim0, DVEC3* dstlim1 )
{
	if( !part ){
		DbgOut( "se : GetBoneLim01 : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstlim0 = part->m_lim0;
	*dstlim1 = part->m_lim1;

	return 0;
}
int CShdElem::GetIgnoreLim( int* dstignore )
{
	if( !part ){
		DbgOut( "se : GetIgnoreLim : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstignore = part->m_ignorelim01;

	return 0;
}

int CShdElem::SetBoneAxisQ( CQuaternion srcq )
{
	if( !part ){
		DbgOut( "se : SetBoneAxisQ : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part->m_axisq = srcq;

	return 0;
}

int CShdElem::GetBoneAxisQ( CQuaternion* dstq )
{
	if( !part ){
		DbgOut( "se : GetBoneAxisQ : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstq = part->m_axisq;

	return 0;
}

int CShdElem::SetZa4Q( CQuaternion srcq )
{
	if( !part ){
		DbgOut( "se : SetZa4Q : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part->m_za4q = srcq;

	return 0;
}
int CShdElem::GetZa4Q( CQuaternion* dstq )
{
	if( !part ){
		DbgOut( "se : GetZa4Q : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstq = part->m_za4q;

	return 0;
}
int CShdElem::SetZa4LocalQ( CQuaternion srcq )
{
	if( !part ){
		DbgOut( "se : SetZa4LocalQ : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part->m_za4localq = srcq;

	return 0;
}
int CShdElem::GetZa4LocalQ( CQuaternion* dstq )
{
	if( !part ){
		DbgOut( "se : GetZa4LocalQ : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstq = part->m_za4localq;

	return 0;
}

int CShdElem::SetZa4Type( int srctype )
{
	if( !part ){
		DbgOut( "se : SetZa4Type : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part->m_za4_type = srctype;

	return 0;
}
int CShdElem::GetZa4Type( int* dsttype )
{
	if( !part ){
		DbgOut( "se : GetZa4Type : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	*dsttype = part->m_za4_type;

	return 0;
}
int CShdElem::SetZa4RotAxis( int srcaxis )
{
	if( !part ){
		DbgOut( "se : SetZa4RotAxis : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	part->m_za4_rotaxis = srcaxis;

	return 0;
}
int CShdElem::GetZa4RotAxis( int* dstaxis )
{
	if( !part ){
		DbgOut( "se : GetZa4RotAxis : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstaxis = part->m_za4_rotaxis;

	return 0;
}

int CShdElem::TransformDMorph( D3DXVECTOR3* vEyePt, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj )
{
	int ret;
	_ASSERT( morph );
	if( morph->m_objtype == SHDPOLYMESH ){
		ret = TransformDMorphPM();
		if( ret ){
			DbgOut( "se : TransformDMorph : TransformDMorphPM error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = d3ddisp->TransformSkinMatMorph( lpmh, matWorld, matView, matProj, vEyePt );
		if( ret ){
			DbgOut( "se : TransformDMorph : d3ddisp TransformSkinMatMorph pm error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( morph->m_objtype == SHDPOLYMESH2 ){
		ret = TransformDMorphPM2();
		if( ret ){
			DbgOut( "se : TransformDMorph : TransformDMorphPM2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = d3ddisp->TransformSkinMatMorph( lpmh, matWorld, matView, matProj, vEyePt );
		if( ret ){
			DbgOut( "se : TransformDMorph : d3ddisp TransformSkinMatMorph pm2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}
int CShdElem::TransformDMorphPM()
{
	int ret;

	CShdElem* baseelem;
	baseelem = morph->m_baseelem;
	if( !baseelem ){
		_ASSERT( 0 );
		return 1;
	}

	CD3DDisp* basedisp;
	basedisp = baseelem->d3ddisp;
	if( !basedisp ){
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = morph->m_boneelem;
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}

	CMMotElem* mmeptr;
	mmeptr = mcptr->GetCurMMotElem( baseelem );
	if( !mmeptr ){
		_ASSERT( 0 );
		return 1;
	}

	int oldv;
	for( oldv = 0; oldv < (int)d3ddisp->m_numTLV; oldv++ ){
		D3DXVECTOR3 diffpos( 0.0f, 0.0f, 0.0f );
		SKINVERTEX* bsv = basedisp->m_skinv + oldv;
		D3DXVECTOR3 bpos;
		bpos.x = bsv->pos[0];
		bpos.y = bsv->pos[1];
		bpos.z = bsv->pos[2];

		D3DXVECTOR3 diffnormal( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 bnormal;
		bnormal.x = bsv->normal[0];
		bnormal.y = bsv->normal[1];
		bnormal.z = bsv->normal[2];

		D3DXVECTOR2 diffuv( 0.0f, 0.0f );
		D3DXVECTOR2 buv;
		buv.x = bsv->tex1[0];
		buv.y = bsv->tex1[1];

		SKINMATERIAL* bsm = basedisp->m_smaterial + oldv;
		D3DCOLORVALUE diffdiff = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bdiff;
		DwcolToD3dcolv( bsm->diffuse, &bdiff );

		D3DCOLORVALUE diffspc = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bspc;
		DwcolToD3dcolv( bsm->specular, &bspc );

		D3DCOLORVALUE diffamb = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bamb;
		DwcolToD3dcolv( bsm->ambient, &bamb );

		D3DCOLORVALUE diffemi = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bemi;
		DwcolToD3dcolv( bsm->emissive, &bemi );

		float diffpow = 0.0f;
		float bpow;
		bpow = bsm->power;

		SKINTANGENT* bst = basedisp->m_skintan + oldv;
		D3DXVECTOR3 difftan( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 btan;
		btan.x = bst->tangent[0];
		btan.y = bst->tangent[1];
		btan.z = bst->tangent[2];

		D3DXVECTOR3 diffrevtan( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 brevtan;
		if( baseelem->clockwise == 3 ){
			SKINTANGENT* brevst = basedisp->m_revskintan + oldv;
			brevtan.x = brevst->tangent[0];
			brevtan.y = brevst->tangent[1];
			brevtan.z = brevst->tangent[2];
		}


		int tno;
		for( tno = 0; tno < morph->m_targetnum; tno++ ){
			CShdElem* tarselem;
			tarselem = ( morph->m_ptarget + tno )->selem;
			_ASSERT( tarselem );

			CD3DDisp* tardisp;
			tardisp = tarselem->d3ddisp;
			_ASSERT( tardisp );

			float curvalue;
			curvalue = mmeptr->GetValue( tarselem );

			if( curvalue != 0.0f ){
				SKINVERTEX* tsv = tardisp->m_skinv + oldv;
				D3DXVECTOR3 tpos;
				tpos.x = tsv->pos[0];
				tpos.y = tsv->pos[1];
				tpos.z = tsv->pos[2];
				diffpos += (tpos - bpos) * curvalue;

				D3DXVECTOR3 tnormal;
				tnormal.x = tsv->normal[0];
				tnormal.y = tsv->normal[1];
				tnormal.z = tsv->normal[2];
				diffnormal += (tnormal - bnormal) * curvalue;

				D3DXVECTOR2 tuv;
				tuv.x = tsv->tex1[0];
				tuv.y = tsv->tex1[1];
				diffuv += (tuv - buv) * curvalue;

				SKINMATERIAL* tsm = tardisp->m_smaterial + oldv;
				D3DCOLORVALUE tdiff;
				DwcolToD3dcolv( tsm->diffuse, &tdiff );
				diffdiff.a += (tdiff.a - bdiff.a) * curvalue;
				diffdiff.r += (tdiff.r - bdiff.r) * curvalue;
				diffdiff.g += (tdiff.g - bdiff.g) * curvalue;
				diffdiff.b += (tdiff.b - bdiff.b) * curvalue;

				D3DCOLORVALUE tspc;
				DwcolToD3dcolv( tsm->specular, &tspc );
				diffspc.a += (tspc.a - bspc.a) * curvalue;
				diffspc.r += (tspc.r - bspc.r) * curvalue;
				diffspc.g += (tspc.g - bspc.g) * curvalue;
				diffspc.b += (tspc.b - bspc.b) * curvalue;

				D3DCOLORVALUE tamb;
				DwcolToD3dcolv( tsm->ambient, &tamb );
				diffamb.a += (tamb.a - bamb.a) * curvalue;
				diffamb.r += (tamb.r - bamb.r) * curvalue;
				diffamb.g += (tamb.g - bamb.g) * curvalue;
				diffamb.b += (tamb.b - bamb.b) * curvalue;

				D3DCOLORVALUE temi;
				DwcolToD3dcolv( tsm->emissive, &temi );
				diffemi.a += (temi.a - bemi.a) * curvalue;
				diffemi.r += (temi.r - bemi.r) * curvalue;
				diffemi.g += (temi.g - bemi.g) * curvalue;
				diffemi.b += (temi.b - bemi.b) * curvalue;

				float tpow;
				tpow = tsm->power;
				diffpow += (tpow - bpow) * curvalue;

				SKINTANGENT* tst = tardisp->m_skintan + oldv;
				D3DXVECTOR3 ttan;
				ttan.x = tst->tangent[0];
				ttan.y = tst->tangent[1];
				ttan.z = tst->tangent[2];
				difftan += (ttan - btan) * curvalue;

				if( baseelem->clockwise == 3 ){
					SKINTANGENT* trevst = tardisp->m_revskintan + oldv;
					D3DXVECTOR3 trevtan;
					trevtan.x = trevst->tangent[0];
					trevtan.y = trevst->tangent[1];
					trevtan.z = trevst->tangent[2];
					diffrevtan += (trevtan - brevtan) * curvalue;
				}
			}
		}

		SKINVERTEX* msv = d3ddisp->m_skinv + oldv;
		
		msv->pos[0] = bpos.x + diffpos.x;
		msv->pos[1] = bpos.y + diffpos.y;
		msv->pos[2] = bpos.z + diffpos.z;
		msv->pos[3] = 1.0f;

		D3DXVECTOR3 mnormal;
		mnormal = bnormal + diffnormal;
		D3DXVec3Normalize( &mnormal, &mnormal );
		msv->normal[0] = mnormal.x;
		msv->normal[1] = mnormal.y;
		msv->normal[2] = mnormal.z;
		msv->normal[3] = 0.0f;

		msv->tex1[0] = buv.x + diffuv.x;
		msv->tex1[1] = buv.y + diffuv.y;

		SKINMATERIAL* msm = d3ddisp->m_smaterial + oldv;
		D3DCOLORVALUE mdiff;
		mdiff.a = bdiff.a + diffdiff.a;
		mdiff.r = bdiff.r + diffdiff.r;
		mdiff.g = bdiff.g + diffdiff.g;
		mdiff.b = bdiff.b + diffdiff.b;
		D3dcolvToDwcol( mdiff, &(msm->diffuse) );

		D3DCOLORVALUE mspc;
		mspc.a = bdiff.a + diffspc.a;
		mspc.r = bdiff.r + diffspc.r;
		mspc.g = bdiff.g + diffspc.g;
		mspc.b = bdiff.b + diffspc.b;
		D3dcolvToDwcol( mspc, &(msm->specular) );

		D3DCOLORVALUE mamb;
		mamb.a = bdiff.a + diffamb.a;
		mamb.r = bdiff.r + diffamb.r;
		mamb.g = bdiff.g + diffamb.g;
		mamb.b = bdiff.b + diffamb.b;
		D3dcolvToDwcol( mamb, &(msm->ambient) );

		D3DCOLORVALUE memi;
		memi.a = bdiff.a + diffemi.a;
		memi.r = bdiff.r + diffemi.r;
		memi.g = bdiff.g + diffemi.g;
		memi.b = bdiff.b + diffemi.b;
		D3dcolvToDwcol( memi, &(msm->emissive) );

		msm->power = bpow + diffpow;

		SKINTANGENT* mst = d3ddisp->m_skintan + oldv;
		D3DXVECTOR3 mtan;
		mtan = btan + difftan;
		D3DXVec3Normalize( &mtan, &mtan );
		mst->tangent[0] = mtan.x;
		mst->tangent[1] = mtan.y;
		mst->tangent[2] = mtan.z;
		mst->tangent[3] = 0.0f;

		if( baseelem->clockwise == 3 ){
			SKINTANGENT* mrevst = d3ddisp->m_revskintan + oldv;
			D3DXVECTOR3 mrevtan;
			mrevtan = brevtan + diffrevtan;
			D3DXVec3Normalize( &mrevtan, &mrevtan );
			mrevst->tangent[0] = mrevtan.x;
			mrevst->tangent[1] = mrevtan.y;
			mrevst->tangent[2] = mrevtan.z;
			mrevst->tangent[3] = 0.0f;
		}

	}

	if( baseelem->clockwise == 3 ){
		ret = d3ddisp->SetRevVertex( COPYVERTEX | COPYUV | COPYNORMAL );
		if( ret ){
			DbgOut( "se : TransformDMorphPM2 : d3ddisp SetRevVertex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = d3ddisp->Copy2VertexBuffer( 0, 1 );
	if( ret ){
		DbgOut( "se : TransformDMorphPM2 : d3ddisp Copy2VertexBufferMorph error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CShdElem::TransformDMorphPM2()
{
	int ret;

	CShdElem* baseelem;
	baseelem = morph->m_baseelem;
	if( !baseelem ){
		_ASSERT( 0 );
		return 1;
	}

	CD3DDisp* basedisp;
	basedisp = baseelem->d3ddisp;
	if( !basedisp ){
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = morph->m_boneelem;
	if( !mcptr ){
		_ASSERT( 0 );
		return 1;
	}

	CMMotElem* mmeptr;
	mmeptr = mcptr->GetCurMMotElem( baseelem );
	if( !mmeptr ){
		_ASSERT( 0 );
		return 1;
	}

	int oldv, boptv, toptv;
	for( oldv = 0; oldv < (int)d3ddisp->m_numTLV; oldv++ ){
		D3DXVECTOR3 diffpos( 0.0f, 0.0f, 0.0f );
		boptv = *( baseelem->polymesh2->oldpno2optpno + oldv );
		SKINVERTEX* bsv = basedisp->m_skinv + boptv;
		D3DXVECTOR3 bpos;
		bpos.x = bsv->pos[0];
		bpos.y = bsv->pos[1];
		bpos.z = bsv->pos[2];

		D3DXVECTOR3 diffnormal( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 bnormal;
		bnormal.x = bsv->normal[0];
		bnormal.y = bsv->normal[1];
		bnormal.z = bsv->normal[2];

		D3DXVECTOR2 diffuv( 0.0f, 0.0f );
		D3DXVECTOR2 buv;
		buv.x = bsv->tex1[0];
		buv.y = bsv->tex1[1];

		SKINMATERIAL* bsm = basedisp->m_smaterial + boptv;
		D3DCOLORVALUE diffdiff = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bdiff;
		DwcolToD3dcolv( bsm->diffuse, &bdiff );

		D3DCOLORVALUE diffspc = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bspc;
		DwcolToD3dcolv( bsm->specular, &bspc );

		D3DCOLORVALUE diffamb = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bamb;
		DwcolToD3dcolv( bsm->ambient, &bamb );

		D3DCOLORVALUE diffemi = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3DCOLORVALUE bemi;
		DwcolToD3dcolv( bsm->emissive, &bemi );

		float diffpow = 0.0f;
		float bpow;
		bpow = bsm->power;

		SKINTANGENT* bst = basedisp->m_skintan + boptv;
		D3DXVECTOR3 difftan( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 btan;
		btan.x = bst->tangent[0];
		btan.y = bst->tangent[1];
		btan.z = bst->tangent[2];

		D3DXVECTOR3 diffrevtan( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 brevtan;
		if( baseelem->clockwise == 3 ){
			SKINTANGENT* brevst = basedisp->m_revskintan + boptv;
			brevtan.x = brevst->tangent[0];
			brevtan.y = brevst->tangent[1];
			brevtan.z = brevst->tangent[2];
		}


		int tno;
		for( tno = 0; tno < morph->m_targetnum; tno++ ){
			CShdElem* tarselem;
			tarselem = ( morph->m_ptarget + tno )->selem;
			_ASSERT( tarselem );

			CD3DDisp* tardisp;
			tardisp = tarselem->d3ddisp;
			_ASSERT( tardisp );

			float curvalue;
			curvalue = mmeptr->GetValue( tarselem );

			if( curvalue != 0.0f ){
				toptv = *( tarselem->polymesh2->oldpno2optpno + oldv );
				SKINVERTEX* tsv = tardisp->m_skinv + toptv;
				D3DXVECTOR3 tpos;
				tpos.x = tsv->pos[0];
				tpos.y = tsv->pos[1];
				tpos.z = tsv->pos[2];
				diffpos += (tpos - bpos) * curvalue;

				D3DXVECTOR3 tnormal;
				tnormal.x = tsv->normal[0];
				tnormal.y = tsv->normal[1];
				tnormal.z = tsv->normal[2];
				diffnormal += (tnormal - bnormal) * curvalue;

				D3DXVECTOR2 tuv;
				tuv.x = tsv->tex1[0];
				tuv.y = tsv->tex1[1];
				diffuv += (tuv - buv) * curvalue;

				SKINMATERIAL* tsm = tardisp->m_smaterial + toptv;
				D3DCOLORVALUE tdiff;
				DwcolToD3dcolv( tsm->diffuse, &tdiff );
				diffdiff.a += (tdiff.a - bdiff.a) * curvalue;
				diffdiff.r += (tdiff.r - bdiff.r) * curvalue;
				diffdiff.g += (tdiff.g - bdiff.g) * curvalue;
				diffdiff.b += (tdiff.b - bdiff.b) * curvalue;

				D3DCOLORVALUE tspc;
				DwcolToD3dcolv( tsm->specular, &tspc );
				diffspc.a += (tspc.a - bspc.a) * curvalue;
				diffspc.r += (tspc.r - bspc.r) * curvalue;
				diffspc.g += (tspc.g - bspc.g) * curvalue;
				diffspc.b += (tspc.b - bspc.b) * curvalue;

				D3DCOLORVALUE tamb;
				DwcolToD3dcolv( tsm->ambient, &tamb );
				diffamb.a += (tamb.a - bamb.a) * curvalue;
				diffamb.r += (tamb.r - bamb.r) * curvalue;
				diffamb.g += (tamb.g - bamb.g) * curvalue;
				diffamb.b += (tamb.b - bamb.b) * curvalue;

				D3DCOLORVALUE temi;
				DwcolToD3dcolv( tsm->emissive, &temi );
				diffemi.a += (temi.a - bemi.a) * curvalue;
				diffemi.r += (temi.r - bemi.r) * curvalue;
				diffemi.g += (temi.g - bemi.g) * curvalue;
				diffemi.b += (temi.b - bemi.b) * curvalue;

				float tpow;
				tpow = tsm->power;
				diffpow += (tpow - bpow) * curvalue;

				SKINTANGENT* tst = tardisp->m_skintan + toptv;
				D3DXVECTOR3 ttan;
				ttan.x = tst->tangent[0];
				ttan.y = tst->tangent[1];
				ttan.z = tst->tangent[2];
				difftan += (ttan - btan) * curvalue;

				if( baseelem->clockwise == 3 ){
					SKINTANGENT* trevst = tardisp->m_revskintan + toptv;
					D3DXVECTOR3 trevtan;
					trevtan.x = trevst->tangent[0];
					trevtan.y = trevst->tangent[1];
					trevtan.z = trevst->tangent[2];
					diffrevtan += (trevtan - brevtan) * curvalue;
				}
			}
		}

		SKINVERTEX* msv = d3ddisp->m_skinv + oldv;
		
		msv->pos[0] = bpos.x + diffpos.x;
		msv->pos[1] = bpos.y + diffpos.y;
		msv->pos[2] = bpos.z + diffpos.z;
		msv->pos[3] = 1.0f;

		D3DXVECTOR3 mnormal;
		mnormal = bnormal + diffnormal;
		D3DXVec3Normalize( &mnormal, &mnormal );
		msv->normal[0] = mnormal.x;
		msv->normal[1] = mnormal.y;
		msv->normal[2] = mnormal.z;
		msv->normal[3] = 0.0f;

		msv->tex1[0] = buv.x + diffuv.x;
		msv->tex1[1] = buv.y + diffuv.y;

		SKINMATERIAL* msm = d3ddisp->m_smaterial + oldv;
		D3DCOLORVALUE mdiff;
		mdiff.a = bdiff.a + diffdiff.a;
		mdiff.r = bdiff.r + diffdiff.r;
		mdiff.g = bdiff.g + diffdiff.g;
		mdiff.b = bdiff.b + diffdiff.b;
		D3dcolvToDwcol( mdiff, &(msm->diffuse) );

		D3DCOLORVALUE mspc;
		mspc.a = bspc.a + diffspc.a;
		mspc.r = bspc.r + diffspc.r;
		mspc.g = bspc.g + diffspc.g;
		mspc.b = bspc.b + diffspc.b;
		D3dcolvToDwcol( mspc, &(msm->specular) );

		D3DCOLORVALUE mamb;
		mamb.a = bamb.a + diffamb.a;
		mamb.r = bamb.r + diffamb.r;
		mamb.g = bamb.g + diffamb.g;
		mamb.b = bamb.b + diffamb.b;
		D3dcolvToDwcol( mamb, &(msm->ambient) );

		D3DCOLORVALUE memi;
		memi.a = bemi.a + diffemi.a;
		memi.r = bemi.r + diffemi.r;
		memi.g = bemi.g + diffemi.g;
		memi.b = bemi.b + diffemi.b;
		D3dcolvToDwcol( memi, &(msm->emissive) );

		msm->power = bpow + diffpow;

		SKINTANGENT* mst = d3ddisp->m_skintan + oldv;
		D3DXVECTOR3 mtan;
		mtan = btan + difftan;
		D3DXVec3Normalize( &mtan, &mtan );
		mst->tangent[0] = mtan.x;
		mst->tangent[1] = mtan.y;
		mst->tangent[2] = mtan.z;
		mst->tangent[3] = 0.0f;

		if( baseelem->clockwise == 3 ){
			SKINTANGENT* mrevst = d3ddisp->m_revskintan + oldv;
			D3DXVECTOR3 mrevtan;
			mrevtan = brevtan + diffrevtan;
			D3DXVec3Normalize( &mrevtan, &mrevtan );
			mrevst->tangent[0] = mrevtan.x;
			mrevst->tangent[1] = mrevtan.y;
			mrevst->tangent[2] = mrevtan.z;
			mrevst->tangent[3] = 0.0f;
		}

	}

	if( baseelem->clockwise == 3 ){
		ret = d3ddisp->SetRevVertex( COPYVERTEX | COPYUV | COPYNORMAL );
		if( ret ){
			DbgOut( "se : TransformDMorphPM2 : d3ddisp SetRevVertex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = d3ddisp->Copy2VertexBufferMorph( baseelem->polymesh2, 1 );
	if( ret ){
		DbgOut( "se : TransformDMorphPM2 : d3ddisp Copy2VertexBufferMorph error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//	_ASSERT( 0 );


	return 0;
}

int CShdElem::DwcolToD3dcolv( DWORD srccol, D3DCOLORVALUE* dstcol )
{
	int srcr, srcg, srcb, srca;
	srca = (srccol & 0xFF000000) >> 24;
	srcr = (srccol & 0x00FF0000) >> 16;
	srcg = (srccol & 0x0000FF00) >> 8;
	srcb = srccol & 0x000000FF;

	dstcol->a = (float)srca / 255.0f;
	dstcol->r = (float)srcr / 255.0f;
	dstcol->g = (float)srcg / 255.0f;
	dstcol->b = (float)srcb / 255.0f;

	return 0;
}
int CShdElem::D3dcolvToDwcol( D3DCOLORVALUE srccol, DWORD* dstcol )
{
	int srcr, srcg, srcb, srca;
	srca = (int)( srccol.a * 255.0f );
	srca = min( 255, srca );
	srca = max( 0, srca );

	srcr = (int)( srccol.r * 255.0f );
	srcr = min( 255, srcr );
	srcr = max( 0, srcr );

	srcg = (int)( srccol.g * 255.0f );
	srcg = min( 255, srcg );
	srcg = max( 0, srcg );

	srcb = (int)( srccol.b * 255.0f );
	srcb = min( 255, srcb );
	srcb = max( 0, srcb );

	*dstcol = (srca << 24) | (srcr << 16) | (srcg << 8) | srcb;

	return 0;
}

int CShdElem::SetMotionBlur( CShdHandler* lpsh, int mode, int blurtime )
{
	if( !d3ddisp ){
		return 0;
	}
	int ret;
	d3ddisp->m_blurmode = mode;
	d3ddisp->m_blurtime = blurtime;
	d3ddisp->m_blurpos = 0;

	if( mode != BLUR_NONE ){
		ret = CreateSkinMat( lpsh, 1 );
		if( ret ){
			DbgOut( "se : SetMotionBlur : CreateSkinMat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdElem::SetBeforeBlur( int bbflag )
{
	if( !d3ddisp ){
		return 0;
	}
	if( (d3ddisp->m_blurmode == BLUR_NONE) || (d3ddisp->m_blurtime <= 0) ){
		return 0;
	}

	int ret;
	ret = d3ddisp->SetBeforeBlur( bbflag );
	if( ret ){
		DbgOut( "se : SetBeforeBlur : d3ddisp SetBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdElem::InitBeforeBlur()
{
	if( !d3ddisp ){
		return 0;
	}
	if( (d3ddisp->m_blurmode == BLUR_NONE) || (d3ddisp->m_blurtime <= 0) ){
		return 0;
	}

	int ret;
	ret = d3ddisp->InitBeforeBlur();
	if( ret ){
		DbgOut( "se : InitBeforeBlur : d3ddisp InitBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdElem::RenderBillboardMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, CTexBank* texbnk )
{
	int ret;
	int pmno;
	CBillboardElem* curbbelem;
	CShdElem* curselem;
	CD3DDisp* curd3ddisp;

	char* beftexname = NULL;
	char* curtexname = NULL;
	int cmp;

	LPDIRECT3DTEXTURE9 curtex = NULL;

	int befblendmode = -1;

	float fogparams[3];

	
	if( m_particleflag == 0 ){

		for( pmno = 0; pmno < billboard->dispnum; pmno++ ){
			curbbelem = *(billboard->disparray + pmno);

			if( curbbelem && (curbbelem->useflag == 1) && (curbbelem->dispflag == 1) && (curbbelem->selem) && (curbbelem->selem->invisibleflag == 0) ){
				curselem = curbbelem->selem;
				if( !curselem ){
					_ASSERT( 0 );
					return 1;
				}

				fogparams[0] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGSTART])));
				fogparams[1] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGEND])));
				fogparams[2] = g_fogtype;

				curd3ddisp = curselem->d3ddisp;
				if( !curd3ddisp ){
					_ASSERT( 0 );
					return 1;
				}

				if( (curd3ddisp->m_blurmode == BLUR_NONE) || (curd3ddisp->m_blurtime <= 0) ){
					curselem->m_blurbefrender = 0;
					continue;
				}

				int vflag;

				vflag = curselem->curbs.visibleflag;
				DWORD dispswitchno = curselem->dispswitchno;
				if( vflag && (curselem->notuse != 1) && 
					((lpsh->m_curds + dispswitchno)->state != 0)
				){

					lpsh->SetCurTexname( curselem->serialno );

					curtexname = curselem->curtexname;
					if( curtexname == 0 )
						curtexname = NULL;
					
					
					if( beftexname && curtexname ){
						cmp = strcmp( beftexname, curtexname );
						if( cmp != 0 ){
							curtex = texbnk->GetTexData( curtexname, curselem->transparent );
							if( g_curtex0 != curtex ){
								pd3dDevice->SetTexture( 0, curtex );
								g_curtex0 = curtex;
							}
						}
					}else{
						if( beftexname != curtexname ){
							curtex = texbnk->GetTexData( curtexname, curselem->transparent );
							if( g_curtex0 != curtex ){
								pd3dDevice->SetTexture( 0, curtex );
								g_curtex0 = curtex;
							}
						}
					}

					SetRenderState( pd3dDevice, curselem );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
					SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

					if( curselem->m_blurbefrender == 0 ){
						InitBeforeBlur();
					}
					curselem->m_blurbefrender = 1;

					ret = curd3ddisp->RenderTri( pd3dDevice, lpsh, m_toonparams, curtex, fogparams, curselem->polymesh->m_toon1, m_lightflag, m_mbluralpha, 1 );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}

					beftexname = curtexname;

				}else{
					curselem->m_blurbefrender = 0;
				}
			}else{
				curselem->m_blurbefrender = 0;
			}
		}
	}else{
		// particleのrender

		//fog
		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

		g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, g_fogcolor );

		g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );

		g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
		pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

		g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
		pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );

		g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
		pd3dDevice->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );


		/////////////////
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
		g_renderstate[ D3DRS_ZWRITEENABLE ] = FALSE;

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
		g_renderstate[ D3DRS_LIGHTING ] = FALSE;

		if( billboard->cmpalways == 0 ){
			pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
			g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;
		}else{
			pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );//!!!!!!!!!!!!!!!
			g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_ALWAYS;
		}

		SetRenderStateIfNotSame( pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD );
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHATESTENABLE, TRUE );
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAREF, 0x08 );
		SetRenderStateIfNotSame( pd3dDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );


		int elemno;
		for( elemno = 0; elemno < MAX_BILLBOARD_NUM; elemno++ ){
			curbbelem = billboard->bbarray + elemno;
			if( curbbelem && (curbbelem->useflag == 1) ){
				curselem = curbbelem->selem;
				if( curselem && (curselem->notuse == 0) ){

					fogparams[0] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGSTART])));
					fogparams[1] = *((float*)(&(curselem->m_renderstate[D3DRS_FOGEND])));
					fogparams[2] = g_fogtype;

					curd3ddisp = curselem->d3ddisp;
					if( !curd3ddisp ){
						_ASSERT( 0 );
						return 1;
					}

					if( (curd3ddisp->m_blurmode == BLUR_NONE) || (curd3ddisp->m_blurtime <= 0) ){
						curselem->m_blurbefrender = 0;
						continue;
					}


					int vflag;

					vflag = curselem->curbs.visibleflag;
					DWORD dispswitchno = curselem->dispswitchno;
					if( vflag && (curselem->notuse != 1) && 
						((lpsh->m_curds + dispswitchno)->state != 0)
					){

						lpsh->SetCurTexname( curselem->serialno );

						curtexname = curselem->curtexname;
						if( curtexname == 0 )
							curtexname = NULL;
						
						
						if( beftexname && curtexname ){
							cmp = strcmp( beftexname, curtexname );
							if( cmp != 0 ){
								curtex = texbnk->GetTexData( curtexname, curselem->transparent );
								if( g_curtex0 != curtex ){
									pd3dDevice->SetTexture( 0, curtex );
									g_curtex0 = curtex;
								}
							}
						}else{
							if( beftexname != curtexname ){
								curtex = texbnk->GetTexData( curtexname, curselem->transparent );
								if( g_curtex0 != curtex ){
									pd3dDevice->SetTexture( 0, curtex );
									g_curtex0 = curtex;
								}
							}
						}

						if( curselem->m_blurbefrender == 0 ){
							InitBeforeBlur();
						}
						curselem->m_blurbefrender = 1;

						ret = curd3ddisp->RenderTri( pd3dDevice, lpsh, m_toonparams, curtex, fogparams, curselem->polymesh->m_toon1, m_lightflag, m_mbluralpha, 1 );
						if( ret ){
							_ASSERT( 0 );
							return 1;
						}

						beftexname = curtexname;
					}else{
						curselem->m_blurbefrender = 0;
					}
				}else{
					curselem->m_blurbefrender = 0;
				}
			}else{
				curselem->m_blurbefrender = 0;
			}
		}
	}

	return 0;

}
int CShdElem::RenderMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh )
{
	int ret = 0;

	if( (type != SHDBILLBOARD) && (type != SHDINFSCOPE) && (type != SHDBBOX) && (type != SHDMORPH) && !d3ddisp )
		return 1;
	
	float fogparams[3];
	fogparams[0] = *((float*)(&m_renderstate[D3DRS_FOGSTART]));
	fogparams[1] = *((float*)(&m_renderstate[D3DRS_FOGEND]));
	fogparams[2] = g_fogtype;

	if( type == SHDPOLYMESH ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh );
		if( polymesh->billboardflag == 0 ){

			if( lpsh->m_shader != COL_TOON1 ){
				ret = d3ddisp->RenderTriPhong( alpha, setalphaflag, 1, pd3dDevice, lpsh, m_toonparams, fogparams, m_lightflag,
					m_enabletexture, &(polymesh->chkalpha), m_mbluralpha, 1 );
				_ASSERT( !ret );

			}else{
				ret = d3ddisp->RenderTriToon1( alpha, setalphaflag, 1, pd3dDevice, lpsh, m_toonparams, fogparams, polymesh->m_toon1, m_lightflag,
					m_enabletexture, &(polymesh->chkalpha), m_mbluralpha, 1 );
				_ASSERT( !ret );
			}
		}

		_ASSERT( !ret );
		
	}else if( type == SHDPOLYMESH2 ){
		_ASSERT( d3ddisp );
		_ASSERT( polymesh2 );

		if( lpsh->m_shader != COL_TOON1 ){

			ret = d3ddisp->RenderTriPhong( alpha, setalphaflag, 1, pd3dDevice, lpsh, m_toonparams, fogparams, m_lightflag,
				m_enabletexture, &(polymesh2->chkalpha), m_mbluralpha, 1 );
			_ASSERT( !ret );

		}else{
			ret = d3ddisp->RenderTriToon1( alpha, setalphaflag, 1, pd3dDevice, lpsh, m_toonparams, fogparams, polymesh2->m_toon1, m_lightflag,
				m_enabletexture, &(polymesh2->chkalpha), m_mbluralpha, 1 );
			_ASSERT( !ret );
		}
	}else if( type == SHDMORPH ){
		_ASSERT( morph );
		CShdElem* baseelem = morph->m_baseelem;
		_ASSERT( baseelem );
		if( morph->m_objtype == SHDPOLYMESH2 ){
			CPolyMesh2* pm2 = baseelem->polymesh2;
			_ASSERT( pm2 );

			if( lpsh->m_shader != COL_TOON1 ){

				ret = d3ddisp->RenderTriPhong( alpha, setalphaflag, 1, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, baseelem->m_lightflag,
					baseelem->m_enabletexture, &(pm2->chkalpha), baseelem->m_mbluralpha, 1 );
				_ASSERT( !ret );

				//_ASSERT( 0 );

			}else{
				ret = d3ddisp->RenderTriToon1( alpha, setalphaflag, 1, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, pm2->m_toon1, 
					baseelem->m_lightflag, baseelem->m_enabletexture, &(pm2->chkalpha), baseelem->m_mbluralpha, 1 );
				_ASSERT( !ret );
			}
			
		}else if( morph->m_objtype == SHDPOLYMESH ){
			CPolyMesh* pm = baseelem->polymesh;
			_ASSERT( pm );

			if( lpsh->m_shader != COL_TOON1 ){
				ret = d3ddisp->RenderTriPhong( alpha, setalphaflag, 1, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, baseelem->m_lightflag,
					baseelem->m_enabletexture, &(pm->chkalpha), baseelem->m_mbluralpha, 1 );
				_ASSERT( !ret );

			}else{
				ret = d3ddisp->RenderTriToon1( alpha, setalphaflag, 1, pd3dDevice, lpsh, baseelem->m_toonparams, fogparams, pm->m_toon1, 
					baseelem->m_lightflag, baseelem->m_enabletexture, &(pm->chkalpha), baseelem->m_mbluralpha, 1 );
				_ASSERT( !ret );
			}

		}
	}

	return 0;
}

int CShdElem::GetMorphCenterPos( D3DXVECTOR3* dstcenter )
{
	*dstcenter = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	if( !morph ){
		_ASSERT( 0 );
		return 1;
	}

	if( morph->m_objtype == SHDPOLYMESH ){
		_ASSERT( morph->m_baseelem );
		CPolyMesh* pm;
		pm = morph->m_baseelem->polymesh;
		if( pm ){
			*dstcenter = pm->m_center;
		}
	}else if( morph->m_objtype == SHDPOLYMESH2 ){
		_ASSERT( morph->m_baseelem );
		CPolyMesh2* pm2;
		pm2 = morph->m_baseelem->polymesh2;
		if( pm2 ){
			*dstcenter = pm2->m_center;
		}
	}

	return 0;
}

int CShdElem::CloneDispObj( CShdElem* srcelem )
{
	int ret;
	if( type == SHDPOLYMESH ){
		if( !srcelem->polymesh ){
			_ASSERT( 0 );
			return 1;
		}
		polymesh = new CPolyMesh();
		if( !polymesh ){
			DbgOut( "se : CloneDispObj : pm alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = polymesh->CloneDispObj( srcelem->polymesh, alpha );
		if( ret ){
			DbgOut( "se : CloneDispObj : pm CloneDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( type == SHDPOLYMESH2 ){
		if( !srcelem->polymesh2 ){
			_ASSERT( 0 );
			return 1;
		}
		polymesh2 = new CPolyMesh2();
		if( !polymesh2 ){
			DbgOut( "se : CloneDispObj : pm2 alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = polymesh2->CloneDispObj( srcelem->polymesh2, facet );
		if( ret ){
			DbgOut( "se : CloneDispObj : pm2 CloneDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
