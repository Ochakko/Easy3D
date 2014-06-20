#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <coef.h>
#include <treehandler2.h>
#include <treeelem2.h>

#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <texbank.h>

#include <d3ddisp.h>

#include <motionctrl.h>
#include <motionpoint2.h>

#include <billboard.h>
#include <billboardelem.h>

#include <motionpoint2.h>
#include <ExtLine.h>

#include <pickdata.h>
#include <quaternion.h>
#include <quaternion2.h>

#include <InfScope.h>
#include <polymesh2.h>
#include <part.h>
#include <BoneInfo.h>

#include <BBox2.h>

#include <mqomaterial.h>
//#if !plugin
	//#include "D3DTextr.h"
//#endif

#include <DispSwitch.h>
#include <Vec3List.h>

#include <morph.h>
#include <Panda.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


HANDLE	CShdHandler::s_hHeap = NULL;
unsigned int	CShdHandler::s_uNumAllocsInHeap = 0;


/// extern
extern int g_useGPU;
extern DWORD g_renderstate[ D3DRS_BLENDOP + 1 ];
extern int g_rsindex[ 80 ];
extern DWORD g_minfilter;
extern DWORD g_magfilter;

extern LPDIRECT3DTEXTURE9 g_curtex0;
extern LPDIRECT3DTEXTURE9 g_curtex1;

extern DWORD g_cop0;
extern DWORD g_cop1;
extern DWORD g_aop0;
extern DWORD g_aop1;
extern int g_rendercnt;//main.cpp


/// global
CTexBank* g_texbnk = 0;


//// 定数　：　ChkConfGroundで使用
#define CONFPARTSMAX	1024
#define CONFPOINTMAX	2048


/***
#ifdef _DEBUG
	void	*CShdHandler::operator new ( size_t size )
	{

		if( s_hHeap == NULL ){
			s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
			//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
			
			if( s_hHeap == NULL )
				return NULL;
		}

		void	*p;
		p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, size );
			
		if( p != NULL ){
			s_uNumAllocsInHeap++;
		}

		return (p);
	}


	void	CShdHandler::operator delete ( void *p )
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
#endif
***/


CShdHandler::CShdHandler( CTreeHandler2* srcthandler )
{
	int ret;
	int	roottype = SHDROOT;

	ret = InitParams( srcthandler );
	if( ret ){
		DbgOut( "CShdHandler : constructor : InitParams error !!!\n" );
		return;
	}

	// root elem : serial 0
	ret = AddShdElem( 0, 0 );
	if( ret ){
		DbgOut( "CShdHandler : constructor : make root elem error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	// type == SHDROOT のセット
	ret += (*this)( 0 )->SetMem( &roottype, BASE_TYPE );
	if( ret ){
		DbgOut( "CShdHandler : constructor : set root elem type error !!!\n" );
		_ASSERT( 0 );
		return;
	}


	m_ds = new CDispSwitch[ DISPSWITCHNUM ];
	if( !m_ds ){
		DbgOut( "sh : constructor : m_ds alloc error !!!\n" );
		_ASSERT( 0 );
		return;
	}
	m_ds2 = new CDispSwitch[ DISPSWITCHNUM ];
	if( !m_ds2 ){
		DbgOut( "sh : constructor : m_ds2 alloc error !!!\n" );
		_ASSERT( 0 );
		return;
	}
	m_dsF = new CDispSwitch[ DISPSWITCHNUM ];
	if( !m_dsF ){
		_ASSERT( 0 );
		return;
	}
	m_tempds = new CDispSwitch[ DISPSWITCHNUM ];
	if( !m_tempds ){
		DbgOut( "sh : constructor : m_tempds alloc error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	int dsno;
	for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
		( m_ds + dsno )->switchno = dsno;
		( m_ds2 + dsno )->switchno = dsno;
		( m_tempds + dsno )->switchno = dsno;
		( m_ds + dsno )->state = 1;
//		( m_ds2 + dsno )->state = 0;//!!!!!!!
		( m_ds2 + dsno )->state = 1;//!!!!!!!
		( m_tempds + dsno )->state = 1;
		( m_dsF + dsno )->switchno = dsno;
		( m_dsF + dsno )->state = 2;
	}

	m_curds = m_ds;

DbgOut( "CShdHandler : constructor : check : s2shd_leng %d\n", s2shd_leng );

}

CShdHandler::~CShdHandler()
{

	if( m_seri2boneno ){
		free( m_seri2boneno );
		m_seri2boneno = 0;
	}
	if( m_boneno2seri ){
		free( m_boneno2seri );
		m_boneno2seri = 0;
	}

	if( m_bbxelem ){
		delete m_bbxelem;
		m_bbxelem = 0;
	}

	DestroyMQOMaterial();

	if( m_ds ){
		delete [] m_ds;
		m_ds = 0;
	}
	if( m_ds2 ){
		delete [] m_ds2;
		m_ds2 = 0;
	}
	if( m_dsF ){
		delete [] m_dsF;
		m_dsF = 0;
	}
	if( m_tempds ){
		delete [] m_tempds;
		m_tempds = 0;
	}

	if( m_ppm ){
		free( m_ppm );
		m_ppm = 0;
	}
	m_mnum = 0;


	int i;
	CShdElem* del_elem = 0;
	CShdElem** shdarray = 0;

#if !plugin
	//DestroyTexture();
	//InvalidateTexture();
#endif

	if( s2shd ){
		for( i = 0; i < s2shd_leng; i++ ){
			del_elem = *(s2shd + i);
			delete del_elem;
		}

		free( s2shd );
		s2shd = 0;
	}

	DbgOut( "CShdHandler : destructor : delete elem\n" );

}

void CShdHandler::DestroyMQOMaterial()
{
	if( m_mathead ){
		CMQOMaterial* delmat = m_mathead;
		CMQOMaterial* nextmat;
		while( delmat ){
			nextmat = delmat->next;
			delete delmat;
			delmat = nextmat;
		}
		m_mathead = 0;
	}
}

int CShdHandler::InitMQOMaterial()
{

	//int ret;

	DestroyMQOMaterial();

	CMQOMaterial* newmat;
	newmat = new CMQOMaterial();
	if( !newmat ){
		DbgOut( "sh : InitMQOMaterial : newmat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	newmat->materialno = -1;//ダミー用
	strcpy_s( newmat->name, 256, "DummyMaterial" );

	newmat->next = 0;
	m_mathead = newmat;

	m_materialcnt = 1;

	return 0;
}

int CShdHandler::InitGroundBMPMaterial( char* texname )
{
	DestroyMQOMaterial();

	static int setcnt = 0;

	CMQOMaterial* newmat;
	newmat = new CMQOMaterial();
	if( !newmat ){
		DbgOut( "sh : InitMQOMaterial : newmat alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newmat->materialno = -1;//ダミー用
	strcpy_s( newmat->name, 256, "DummyMaterial" );

	newmat->next = 0;
	m_mathead = newmat;

///////////
	CMQOMaterial* newmat2;
	newmat2 = new CMQOMaterial();
	if( !newmat2 ){
		DbgOut( "sh : InitMQOMaterial : newmat2 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newmat2->materialno = 0;
	newmat2->next = 0;
	newmat->next = newmat2;

	sprintf_s( newmat2->name, 256, "GroundMat%d", setcnt );
	if( texname ){
		strcpy_s( newmat2->tex, 256, texname );
	}

	m_materialcnt = 2;
	setcnt++;

	return 0;



}

CMQOMaterial* CShdHandler::GetMaterialFromNo( CMQOMaterial* mathead, int matno )
{
	CMQOMaterial* retmat = 0;
	CMQOMaterial* chkmat = mathead;

	while( chkmat ){
		if( chkmat->materialno == matno ){
			retmat = chkmat;
			break;
		}
		chkmat = chkmat->next;
	}
	
	return retmat;
}

CMQOMaterial* CShdHandler::GetLastMaterial()
{
	CMQOMaterial* retmat = m_mathead;
	CMQOMaterial* curmat = m_mathead;

	while( curmat ){
		retmat = curmat;
		curmat = curmat->next;
	}
	return retmat;
}

int CShdHandler::GetMaterialNoByName( char* srcname, int* matnoptr )
{
	*matnoptr = -3;

	CMQOMaterial* curmat = m_mathead;
	while( curmat ){
		int cmp0;
		cmp0 = strcmp( curmat->name, srcname );
		if( cmp0 == 0 ){
			*matnoptr = curmat->materialno;
			break;
		}
		curmat = curmat->next;
	}

	return 0;
}


int CShdHandler::InitParams( CTreeHandler2* srcthandler )
{
	isfirst = 1;

	thandler = srcthandler;

	s2shd_allocleng = 0;
	s2shd_leng = 0;
	s2shd = 0;

	m_bbxelem = 0;

	//texbnk = 0;

	m_lastboneno = 0;
	m_lastoldboneno = 0;

	m_aspect = 1.0f;

	m_inRDBflag = 0;

	m_center.x = 0.0f;
	m_center.y = 0.0f;
	m_center.z = 0.0f;

	m_orgcenter.x = 0.0f;
	m_orgcenter.y = 0.0f;
	m_orgcenter.z = 0.0f;


	m_TLmode = TLMODE_ORG;

	m_im2enableflag = 0;

	m_bbox.InitParams();
	m_bbx.InitParams();

	m_bonetype = BONETYPE_RDB2;

	m_curdispswitch = 0;

	m_mikoblendtype = MIKOBLEND_MIX;


	m_shader = COL_OLD;
	m_overflow = OVERFLOW_ORG;

	m_scene_ambient.a = 1.0f;
	m_scene_ambient.r = 0.25f;
	m_scene_ambient.g = 0.25f;
	m_scene_ambient.b = 0.25f;

	m_sigmagicno = SIGFILEMAGICNO_14;

	m_seri2boneno = 0;
	m_boneno2seri = 0;

	m_mathead = 0;
	m_materialcnt = 0;

	m_ds = 0;
	m_ds2 = 0;
	m_dsF = 0;
	m_curds = 0;
	m_tempds = 0;

	m_phongedge0enable = 0;
	m_phongedge0[0] = 0.0f;
	m_phongedge0[1] = 0.0f;
	m_phongedge0[2] = 0.0f;
	m_phongedge0[3] = 8.0f;
	m_phongedge0Blend = 3;
	m_phongedge0Alpha = 1.0f;

	ZeroMemory( m_dirname, sizeof( char ) * PATH_LENG );

	m_bbtransskip = 0;

	m_gpdata.InitParams();

	m_ppm = 0;
	m_mnum = 0;

	return 0;
}

int CShdHandler::AddShdElem( int srcserino, int srcdepth )
{
	CTreeElem2* curtelem = 0;
	int ret, brono, type;


	if( !thandler )
		return 1;
	
	curtelem = (*thandler)( srcserino );
	brono = curtelem->brono;
	type = curtelem->type;

DbgOut( "CShdHandler : AddShdElem : srcseri %d, brono %d, srcdepth %d\n", 
	   srcserino, brono, srcdepth );

	ret = AddS2Shd( srcserino, brono, srcdepth, type );
	if( ret ){
		DbgOut( "CShdHandler : AddS2Shd error !!!\n" );
		return 1;
	}

//DbgOut( "###CShdHandler : AddShdElem : s2shd_leng %d\n", s2shd_leng );

	return 0;
}

	// 各shdelemの初期化
int	CShdHandler::Init3DObj( int srcserino, CMeshInfo* srcmeshinfo )
{
	int ret;
	ret = CheckNo( srcserino );
	_ASSERT( !ret );

	if( !ret ){
		ret = (*this)( srcserino )->Init3DObj( srcmeshinfo );
		_ASSERT( !ret );
		return ret;
	}else
		return 1;
}

int CShdHandler::SetClockwise( int srcseri, int cw )
{
	CShdElem* selem;
	int ret;

	selem = (*this)( srcseri );
	if( !selem )
		return 1;

	ret = selem->SetClockwise( cw );
	if( ret ){
		DbgOut( "CShdHandler : SetClockwise : selem->SetClockwise error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CShdHandler::SetInvFace( int srcseri, int srcif )
{
	CShdElem* selem;
	int ret;

	selem = (*this)( srcseri );
	if( !selem )
		return 1;

	ret = selem->SetInvFace( srcif );
	if( ret ){
		DbgOut( "CShdHandler : SetInvFace : selem->SetInvFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CShdHandler::SetBDivUV( int srcseri, int srcbdivU, int srcbdivV )
{
	CShdElem* selem;
	int ret;

	selem = (*this)( srcseri );
	if( !selem )
		return 1;
	ret = selem->SetBDivUV( srcbdivU, srcbdivV );
	if( ret ){
		DbgOut( "CShdHandler : SetBDivUV : selem->SetBDivUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}


int CShdHandler::SetBMesh( int bmseri, int blseri, int brono )
{
	CShdElem* bmelem = 0;
	CShdElem* blelem = 0;
	int ret = 0;

	bmelem = (*this)( bmseri );
	blelem = (*this)( blseri );

	if( !bmelem || !blelem )
		return 1;

	ret = bmelem->SetBMesh( blelem, brono, blseri );
	_ASSERT( !ret );

	return ret;

}

#if !plugin
	int CShdHandler::InvalidateTexture( int resetflag )
	{
		//DestroyTexture();

		int ret;
		int i;
		char* texname;
		int transparent;
		
		for( i = 1; i < s2shd_leng; i++ ){
			texname = (*this)( i )->texname;
			transparent = (*this)( i )->transparent;
			if( texname && (*texname != 0) ){

				ret = g_texbnk->InvalidateTexture( texname, transparent, resetflag );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}

			int texno;
			int extnum;
			extnum = (*this)( i )->exttexnum;
			for( texno = 0; texno < extnum; texno++ ){
				char* extname;
				extname = *( (*this)( i )->exttexname + texno );
				if( extname && (*extname != 0) ){

					ret = g_texbnk->InvalidateTexture( extname, transparent, resetflag );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}
				}

			}

		}

		//rokdebone2用のボーンマークもinvalidateする。
		if( m_inRDBflag == 1 ){
			ret = g_texbnk->InvalidateTexture( s_bonemarkdds, 0, resetflag );
			_ASSERT( !ret );
			ret = g_texbnk->InvalidateTexture( s_bonemarkdds2, 0, resetflag );
			_ASSERT( !ret );
			ret = g_texbnk->InvalidateTexture( s_selvertbmp, 1, resetflag );
			_ASSERT( !ret );
			ret = g_texbnk->InvalidateTexture( s_selvbmp, 0, resetflag );
			_ASSERT( !ret );

			ret = g_texbnk->InvalidateTexture( s_cam_kaku, 0, resetflag );
			_ASSERT( !ret );
			ret = g_texbnk->InvalidateTexture( s_cam_i, 0, resetflag );
			_ASSERT( !ret );
			ret = g_texbnk->InvalidateTexture( s_cam_kai, 0, resetflag );
			_ASSERT( !ret );

			int texno;
			for( texno = 0; texno <= 9; texno++ ){
				char tname[MAX_PATH];
				sprintf_s( tname, MAX_PATH, "%d.png", texno );
				ret = g_texbnk->InvalidateTexture( tname, 0, resetflag );
				_ASSERT( !ret );
			}
		}


		CMQOMaterial* curmat = m_mathead;
		while( curmat ){
			if( curmat->name[0] != 0 ){
				g_texbnk->InvalidateTexture( curmat->name, curmat->transparent, resetflag );
			}
			if( curmat->bump[0] != 0 ){
				g_texbnk->InvalidateTexture( curmat->bump, curmat->transparent, resetflag );
			}
			curmat = curmat->next;
		}

		return 0;
	}


	int CShdHandler::AddTexture( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int srcseri, UINT miplevels, DWORD mipfilter )
	{
		int ret;
		int transparent;
		char* texname;

		if( !g_texbnk ){
			g_texbnk = new CTexBank( miplevels, mipfilter );
			if( !g_texbnk ){
				DbgOut( "CShdHandler :	AddTexture : new CTexBank error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


		int findno;

		texname = (*this)( srcseri )->texname;
		transparent = (*this)( srcseri )->transparent;
		if( texname && (*texname != 0) ){
			ret = g_texbnk->AddName( 0, texname, transparent, D3DPOOL_MANAGED );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}

			//if( (ret > 0) || onlyrestore ){
				//追加する
				findno = g_texbnk->GetTexNoByName( texname, transparent );
				if( (findno < 0) || (findno >= g_texbnk->namenum) ){
					DbgOut( "shandler : AddTexture : GetTexNoByName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = g_texbnk->CreateTexData( findno, pdev );

//DbgOut( "shandler : AddTexture : CreateTexData : name %s, trans %d, ret %d\r\n", texname, transparent, ret );

				if( ret && (ret != -2) ){
					DbgOut( "ShdHandler : AddTexture : CreateTexData : media not found %s error !!!\n",
						(*g_texbnk)( findno ) );

					int ret2;
					ret2 = g_texbnk->SetTexData( findno, NULL );
					if( ret2 ){
						_ASSERT( 0 );
						return 1;
					}
				}
				
				if( (ret == 0) && (transparent == 1) ){
					LPDIRECT3DTEXTURE9 lptexture;
					lptexture = g_texbnk->GetTexData( texname, transparent );
					if( lptexture != NULL ){
						ret = SetTextureAlpha( lptexture, 0 );
						if( ret ){
							DbgOut( "shdhandler : AddTexture : SetTextureAlpha error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}

				texname = (*g_texbnk)( findno ); //for debug
				DbgOut( "CShdHandler : AddTexture : D3DTextr_CreateTextureFromFile : %s\n", texname );
			//}

		}
		
		int texno;
		int extnum;
		extnum = (*this)( srcseri )->exttexnum;
		for( texno = 0; texno < extnum; texno++ ){
			char* extname;
			extname = *( (*this)( srcseri )->exttexname + texno );
			if( extname && (*extname != 0) ){
				ret = g_texbnk->AddName( 0, extname, transparent, D3DPOOL_MANAGED );
				if( ret < 0 ){
					_ASSERT( 0 );
					return 1;
				}

				//if( (ret > 0) || onlyrestore ){
					//追加する
					findno = g_texbnk->GetTexNoByName( texname, transparent );
					if( (findno < 0) || (findno >=g_texbnk->namenum) ){
						DbgOut( "shandler : AddTexture : GetTexNoByName error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ret = g_texbnk->CreateTexData( findno, pdev );
					if( ret && (ret != -2) ){
						DbgOut( "ShdHandler : AddTexture : CreateTexData : media not found %s error !!!\n",
							(*g_texbnk)( findno ) );

						int ret2;
						ret2 = g_texbnk->SetTexData( findno, NULL );
						if( ret2 ){
							_ASSERT( 0 );
							return 1;
						}
					}

					if( (ret == 0) && (transparent == 1) ){
						LPDIRECT3DTEXTURE9 lptexture;
						lptexture = g_texbnk->GetTexData( texname, transparent );
						if( lptexture != NULL ){
							ret = SetTextureAlpha( lptexture, 0 );
							if( ret ){
								DbgOut( "shdhandler : AddTexture ext : SetTextureAlpha error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
					}

					texname = (*g_texbnk)( findno ); //for debug

					DbgOut( "CShdHandler : AddTexture : D3DTextr_CreateTextureFromFile : %s\n", texname );

				//}
			
			}

		}
	
		return 0;
	}


	int CShdHandler::CreateTextureFromPnd( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int neededbone, UINT miplevels, DWORD mipfilter, int dispwarnflag, CPanda* panda, int pndid )
	{
		int j, ret;
		int transparent;
		char* texname;

		if( !g_texbnk ){
			g_texbnk = new CTexBank( miplevels, mipfilter );
			if( !g_texbnk ){
				DbgOut( "CShdHandler : CreateTexture : new CTexBank error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		if( neededbone ){
			//bone用
			//MEDIADIR
			DWORD dirleng;
			char dirname[MAX_PATH];
			ZeroMemory( dirname, MAX_PATH );
			dirleng = GetEnvironmentVariable( (LPCTSTR)"MEDIADIR", (LPTSTR)dirname, MAX_PATH );
			if( (dirleng == 0) || (dirleng >= MAX_PATH) ){
				DbgOut( "sh : CreateTexture : MEDIADIR too long error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}



			//ret = texbnk->AddName( s_bonemarkbmp, 1 );
			ret = g_texbnk->AddName( dirname, s_bonemarkdds, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_bonemarkdds2, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_selvertbmp, 1 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_selvbmp, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
	

			ret = g_texbnk->AddName( dirname, s_cam_kaku, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_cam_i, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_cam_kai, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}


			int texno;
			for( texno = 0; texno <= 9; texno++ ){
				char tname[MAX_PATH];
				sprintf_s( tname, MAX_PATH, "%d.png", texno );
				ret = g_texbnk->AddName( dirname, tname, 0 );
				//_ASSERT( !ret );
			}

		}

		// material
		CMQOMaterial* curmqomat = m_mathead;
		while( curmqomat ){

			texname = curmqomat->tex;
			transparent = curmqomat->transparent;
			if( texname && (*texname != 0) ){
				ret = g_texbnk->AddName( m_dirname, texname, transparent, D3DPOOL_MANAGED );
				//if( ret <= 0 ){
				if( ret < 0 ){
					_ASSERT( 0 );
					return 1;
				}
			}

			// bump
			if( curmqomat->bump[0] != 0 ){
				//ret = g_texbnk->AddName( m_dirname, curmqomat->bump, transparent, D3DPOOL_MANAGED );
				ret = g_texbnk->AddName( m_dirname, curmqomat->bump, 0, D3DPOOL_MANAGED );
				if( ret < 0 ){
					_ASSERT( 0 );
					return 1;
				}
			}

			int texno;
			int extnum;
			extnum = curmqomat->exttexnum;
			for( texno = 0; texno < extnum; texno++ ){
				char* extname;
				extname = *( curmqomat->exttexname + texno );
				if( extname && (*extname != 0) ){
					ret = g_texbnk->AddName( m_dirname, extname, curmqomat->transparent, D3DPOOL_MANAGED );
					//if( ret <= 0 ){
					if( ret < 0 ){
						_ASSERT( 0 );
						return 1;
					}
				}

			}

			curmqomat = curmqomat->next;
		}



		for( j = 0; j < g_texbnk->namenum; j++ ){
			char* curtexname = (*g_texbnk)( j );
			char curtexpath[MAX_PATH] = {0};
			sprintf_s( curtexpath, MAX_PATH, "%s%s", m_dirname, curtexname );

			ret = g_texbnk->SetBufDataFromPnd( j, panda, pndid, curtexpath );
			if( ret ){
				continue;
			}

			ret = g_texbnk->CreateTexData( j, pdev );

//DbgOut( "shandler : CreateTexture : CreateTexData : index %d, ret %d\r\n", j, ret );

			if( ret && (ret != -2) ){
				ERRORMES errormes;
				if( appwnd && (dispwarnflag != 0) ){
					errormes.errorcode = D3DAPPERR_MEDIANOTFOUND;
					errormes.type = MSG_NONE;
					errormes.mesptr = (*g_texbnk)( j );
					//SendMessage( appwnd, WM_USER_ERROR, 0, (LPARAM)&errormes );
					
					DbgOut( "ShdHandler : CreateTexData : media not found %s error !!!\n",
						(*g_texbnk)( j ) );
				}

				int ret2;
				ret2 = g_texbnk->SetTexData( j, NULL );
				if( ret2 ){
					_ASSERT( 0 );
					return 1;
				}
			}
			transparent = g_texbnk->GetIntData( j );
			if( (ret == 0) && (transparent == 1) ){
				texname = (*g_texbnk)( j );
				LPDIRECT3DTEXTURE9 lptexture;
				lptexture = g_texbnk->GetTexData( texname, transparent );
				if( lptexture != NULL ){
					ret = SetTextureAlpha( lptexture, 0 );
					if( ret ){
						DbgOut( "shdhandler : CreateTexture : SetTextureAlpha error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			texname = (*g_texbnk)( j ); //for debug
			DbgOut( "CShdHandler : CreateTexture : D3DTextr_CreateTextureFromFile : %s\n", texname );
		}
		
		if( neededbone ){
			LPDIRECT3DTEXTURE9 lptexture;
			lptexture = g_texbnk->GetTexData( s_bonemarkdds, 0 );
			if( lptexture != NULL ){

				HRESULT hr;
				D3DSURFACE_DESC	sdesc;
				hr = lptexture->GetLevelDesc( 0, &sdesc );
				if( hr != D3D_OK ){
					_ASSERT( 0 );
					return 1;
				}
			
				CD3DDisp::s_bonebmpsizex = sdesc.Width / 2;
				CD3DDisp::s_bonebmpsizey = sdesc.Height / 2;
			}
		}

		return 0;
	}


	int CShdHandler::CreateTexture( LPDIRECT3DDEVICE9 pdev, HWND appwnd, int neededbone, UINT miplevels, DWORD mipfilter, int dispwarnflag, int billboardflag )
	{
		int i, j, ret;
		int transparent;
		char* texname;

		//if( texbnk )
		//	return 1;
		

		if( !g_texbnk ){
			g_texbnk = new CTexBank( miplevels, mipfilter );
			if( !g_texbnk ){
				DbgOut( "CShdHandler : CreateTexture : new CTexBank error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


		if( neededbone ){
			//bone用

			//MEDIADIR
			DWORD dirleng;
			char dirname[MAX_PATH];
			ZeroMemory( dirname, MAX_PATH );
			dirleng = GetEnvironmentVariable( (LPCTSTR)"MEDIADIR", (LPTSTR)dirname, MAX_PATH );
			if( (dirleng == 0) || (dirleng >= MAX_PATH) ){
				DbgOut( "sh : CreateTexture : MEDIADIR too long error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}



			//ret = texbnk->AddName( s_bonemarkbmp, 1 );
			ret = g_texbnk->AddName( dirname, s_bonemarkdds, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_bonemarkdds2, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_selvertbmp, 1 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_selvbmp, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
	

			ret = g_texbnk->AddName( dirname, s_cam_kaku, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_cam_i, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}
			ret = g_texbnk->AddName( dirname, s_cam_kai, 0 );
			if( ret < 0 ){
				_ASSERT( 0 );
				return 1;
			}


			int texno;
			for( texno = 0; texno <= 9; texno++ ){
				char tname[MAX_PATH];
				sprintf_s( tname, MAX_PATH, "%d.png", texno );
				ret = g_texbnk->AddName( dirname, tname, 0 );
				//_ASSERT( !ret );
			}

		}

		// billboard, particle
		if( billboardflag == 1 ){
			for( i = 1; i < s2shd_leng; i++ ){
				texname = (*this)( i )->texname;
				transparent = (*this)( i )->transparent;
				if( texname && (*texname != 0) ){
					ret = g_texbnk->AddName( m_dirname, texname, transparent, D3DPOOL_MANAGED );
					//if( ret <= 0 ){
					if( ret < 0 ){
						_ASSERT( 0 );
						return 1;
					}
				}


				int texno;
				int extnum;
				extnum = (*this)( i )->exttexnum;
				for( texno = 0; texno < extnum; texno++ ){
					char* extname;
					extname = *( (*this)( i )->exttexname + texno );
					if( extname && (*extname != 0) ){
						ret = g_texbnk->AddName( m_dirname, extname, transparent, D3DPOOL_MANAGED );
						//if( ret <= 0 ){
						if( ret < 0 ){
							_ASSERT( 0 );
							return 1;
						}
					}

				}
			}
		}

		// material
		CMQOMaterial* curmqomat = m_mathead;
		while( curmqomat ){

			texname = curmqomat->tex;
			transparent = curmqomat->transparent;
			if( texname && (*texname != 0) ){
				ret = g_texbnk->AddName( m_dirname, texname, transparent, D3DPOOL_MANAGED );
				//if( ret <= 0 ){
				if( ret < 0 ){
					_ASSERT( 0 );
					return 1;
				}
			}

			// bump
			if( curmqomat->bump[0] != 0 ){
				//ret = g_texbnk->AddName( m_dirname, curmqomat->bump, transparent, D3DPOOL_MANAGED );
				ret = g_texbnk->AddName( m_dirname, curmqomat->bump, 0, D3DPOOL_MANAGED );
				if( ret < 0 ){
					_ASSERT( 0 );
					return 1;
				}
			}

			int texno;
			int extnum;
			extnum = curmqomat->exttexnum;
			for( texno = 0; texno < extnum; texno++ ){
				char* extname;
				extname = *( curmqomat->exttexname + texno );
				if( extname && (*extname != 0) ){
					ret = g_texbnk->AddName( m_dirname, extname, curmqomat->transparent, D3DPOOL_MANAGED );
					//if( ret <= 0 ){
					if( ret < 0 ){
						_ASSERT( 0 );
						return 1;
					}
				}

			}

			curmqomat = curmqomat->next;
		}


		for( j = 0; j < g_texbnk->namenum; j++ ){
			//transparent = texbnk->GetIntData( j );
			ret = g_texbnk->CreateTexData( j, pdev );

//DbgOut( "shandler : CreateTexture : CreateTexData : index %d, ret %d\r\n", j, ret );

			if( ret && (ret != -2) ){
				ERRORMES errormes;
				if( appwnd && (dispwarnflag != 0) ){
					errormes.errorcode = D3DAPPERR_MEDIANOTFOUND;
					errormes.type = MSG_NONE;
					errormes.mesptr = (*g_texbnk)( j );
					//SendMessage( appwnd, WM_USER_ERROR, 0, (LPARAM)&errormes );
					
					DbgOut( "ShdHandler : CreateTexData : media not found %s error !!!\n",
						(*g_texbnk)( j ) );
				}

				int ret2;
				ret2 = g_texbnk->SetTexData( j, NULL );
				if( ret2 ){
					_ASSERT( 0 );
					return 1;
				}
			}
			transparent = g_texbnk->GetIntData( j );
			if( (ret == 0) && (transparent == 1) ){
				texname = (*g_texbnk)( j );
				LPDIRECT3DTEXTURE9 lptexture;
				lptexture = g_texbnk->GetTexData( texname, transparent );
				if( lptexture != NULL ){
					ret = SetTextureAlpha( lptexture, 0 );
					if( ret ){
						DbgOut( "shdhandler : CreateTexture : SetTextureAlpha error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			/***
			if( transparent == 0 ){
				D3DTextr_CreateTextureFromFile( texname );
			}else{
				DbgOut( "shdhandler : CreateTexture : TRANSPARENTBLACK %s\n", texname );
				D3DTextr_CreateTextureFromFile( texname );
				//D3DTextr_CreateTextureFromFile( texname, 0, D3DTEXTR_TRANSPARENTBLACK );
			}
			***/
			texname = (*g_texbnk)( j ); //for debug
			DbgOut( "CShdHandler : CreateTexture : D3DTextr_CreateTextureFromFile : %s\n", texname );
		}

		
		
		if( neededbone ){

			LPDIRECT3DTEXTURE9 lptexture;
			lptexture = g_texbnk->GetTexData( s_bonemarkdds, 0 );
			if( lptexture != NULL ){

				HRESULT hr;
				D3DSURFACE_DESC	sdesc;
				hr = lptexture->GetLevelDesc( 0, &sdesc );
				if( hr != D3D_OK ){
					_ASSERT( 0 );
					return 1;
				}
			
				CD3DDisp::s_bonebmpsizex = sdesc.Width / 2;
				CD3DDisp::s_bonebmpsizey = sdesc.Height / 2;
			}
		}



		/***
		if( neededbone ){
			//bonebmpのalphaのセット。
			LPDIRECT3DTEXTURE9 lptexture;
			lptexture = texbnk->GetTexData( s_bonemarkbmp );
			if( lptexture != NULL ){
				ret = SetTextureAlpha( lptexture, 1 );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}
		***/

		return 0;
	}
	int CShdHandler::SetTextureAlpha( LPDIRECT3DTEXTURE9 lptexture, int isbone )
	{
		if( lptexture == NULL ){
			DbgOut( "shdhandler : SetTextureAlpha : lptexture NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		HRESULT hr;
		D3DSURFACE_DESC	sdesc;
		hr = lptexture->GetLevelDesc( 0, &sdesc );
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
			DbgOut( "ShdHandler : SetTextureAlpha : d3dformat %d is not supported error !!!\n",
				fmt );
			_ASSERT( 0 );
			return 1;
		}

		//if( (fmt != D3DFMT_A8R8G8B8) && (fmt != D3DFMT_A1R5G5B5) ){
		//	DbgOut( "ShdHandler : SetTextureAlpha : d3dformat %d is not supported error !!!\n",
		//		fmt );
		//	_ASSERT( 0 );
		//	return 1;
		//}

		int w = sdesc.Width;
		int h = sdesc.Height;
		D3DLOCKED_RECT lockedrect;
		RECT rect;


DbgOut( "shandler : SetTextureAlpha : width %d, height %d\n", w, h );


		rect.left = 0; rect.right = w; rect.top = 0; rect.bottom = h;
		hr = lptexture->LockRect( 0, &lockedrect, &rect, 0 );
		if( hr != D3D_OK ){
			DbgOut( "shdhandler : SetTextureAlpha : LockRect error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		BYTE*	bitshead;
		DWORD* dwbits;
		WORD* wbits;
		//INT                 Pitch;
		//void*               pBits;
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
					if( col == 0 ){
					//if( (colr < 30) && (colg < 30) && (colb < 30) ){
						//alpha 0
						*dwbits = 0;
					}else{
						//alpha 255
						*dwbits = col | 0xFF000000;
						//*dwbits = col;
					}
				}else if( (fmt == D3DFMT_A1R5G5B5) || (fmt == D3DFMT_X1R5G5B5) ){
					wbits = (WORD*)bitshead + width;
					WORD col = *wbits & 0x7FFF;
					if( col == 0 ){
						*wbits = 0;
					}else{
						*wbits = col | 0x8000;
						//*wbits = col;
					}
				}

			}
	
		}

		hr = lptexture->UnlockRect( 0 );
		if( hr != D3D_OK ){
			DbgOut( "shdhandler : SetTextureAlpha : UnlockRect error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		/***
		if( isbone ){
			CD3DDisp::s_bonebmpsizex = sdesc.Width / 2;
			CD3DDisp::s_bonebmpsizey = sdesc.Height / 2;
		}
		***/

		return 0;
	}


	int CShdHandler::DestroyTexture()
	{


		if( g_texbnk ){
			delete g_texbnk;
			g_texbnk = 0;
		}
		return 0;
	}
	int CShdHandler::SetTexTransparent()
	{
		//int j, transparent;
		//char* texname;

		DbgOut( "CShdHandler : SetTexTransParent\n" );

		/***
		for( j = 0; j < texbnk->namenum; j++ ){
			texname = (*texbnk)( j );
			
			transparent = texbnk->GetIntData( j );
			if( transparent )
			{
				DbgOut( "shdhandler : SetTexTransparent : TRANSPARENTBLACK %s\n", texname );

				LPDIRECTDRAWSURFACE7 dds7;
				DDCOLORKEY	colkey;
				HRESULT hres;
				
				colkey.dwColorSpaceLowValue = RGB_MAKE( 0, 0, 0 );
				colkey.dwColorSpaceHighValue = RGB_MAKE( 0, 0, 0 );

				dds7 = D3DTextr_GetSurface( texname );
				_ASSERT( dds7 );

				hres = dds7->SetColorKey( DDCKEY_SRCBLT, &colkey );
				ErrorMessage( "dds7->SetColorKey", hres );

			}
		}
		***/
		return 0;
	}

#endif


int CShdHandler::InvalidateDispObj()
{
	int i, ret;
	int etype;
	CShdElem* selem;
	int isfirst = 1;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;

		int isbone = ( selem->IsJoint() && (etype != SHDMORPH) );

		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || (etype == SHDMORPH) ||
			(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) ||
			(etype == SHDINFSCOPE) || 
			isbone ){

			if( isfirst ){
				(*this)( i )->ResetTotalPrim();
				isfirst = 0;
			}

			ret = (*this)( i )->InvalidateDispObj();
			if( ret )
				return 1;

		}
	}	


	CShdElem* bbxelem;
	bbxelem = GetBBoxElem();
	if( bbxelem ){
		bbxelem->ResetTotalPrim();
		ret = bbxelem->InvalidateDispObj();
		_ASSERT( !ret );
	}


	return 0;
}
int CShdHandler::CreateAfterImage( LPDIRECT3DDEVICE9 pd3dDevice, int imagenum )
{
	int i, ret;
	int etype;
	CShdElem* selem;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;

		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || 
			(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
			ret = selem->CreateAfterImage( pd3dDevice, imagenum );
			if( ret )
				return 1;
		}
	}

	return 0;
}
int CShdHandler::DestroyAfterImage()
{
	int i, ret;
	int etype;
	CShdElem* selem;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;

		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || 
			(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
			ret = selem->DestroyAfterImage();
			if( ret )
				return 1;
		}
	}

	return 0;
}

int CShdHandler::SetAlpha( float alpha, int srcseri, int updateflag )
{
	// 古い関数　マテリアル対応前
	int i;
	int etype;
	CShdElem* selem;
	int ret;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			ret = SetAlpha( alpha, i, updateflag );
			if( ret ){
				DbgOut( "sh : SetAlpha : %d error !!!\n", i );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetAlpha : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetAlpha : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;


//DbgOut( "shdhandler : SetAlpha : seri %d, alpha %f, etype %d\n", srcseri, alpha, etype );


		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			if( selem->polymesh && selem->polymesh->billboardflag ){
				selem->alpha = alpha;

				if( selem->d3ddisp ){
					ret = selem->d3ddisp->SetDispTlvAlpha( alpha );
					if( ret ){
						DbgOut( "shandler : SetAlpha : SetDispTlvAlpha error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
//DbgOut( "check!!!, sh SetAlpha , %d, %f\r\n", selem->serialno, alpha );
				}
//_ASSERT( 0 );
			}else{
				/***
				int matno = -3;
				ret = selem->GetFirstMaterialNo( &matno );
				_ASSERT( !ret );

				CMQOMaterial* curmat;
				curmat = GetMaterialFromNo( m_mathead, matno );
				if( curmat ){
					curmat->col.a = alpha;
					curmat->dif4f.a = alpha;
					ret = ChkAlphaNum();
					_ASSERT( !ret );
				}
				***/
				selem->SetAlpha( alpha );
				selem->setalphaflag = 1;
//_ASSERT( 0 );
			}
		}
	}

	return 0;
}

int CShdHandler::SetBlendingMode( int srcseri, int bmode )
{
	int ret;
	int i;
	int etype;
	CShdElem* selem;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){

			ret = SetBlendingMode( i, bmode );
			if( ret ){
				DbgOut( "sh : SetBlendingMode : %d error !!!\n", i );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetBlendingMode : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetBlendingMode : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;


		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			if( selem->polymesh && selem->polymesh->billboardflag ){
				selem->blendmode = bmode;

				if( bmode == 0 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
					//selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
					selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
					selem->m_renderstate[ D3DRS_ALPHAREF ] = 0x08;
					selem->m_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
							
				}else if( bmode == 1 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ONE;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;
					selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
					selem->m_renderstate[ D3DRS_ALPHAREF ] = 0x0;
					selem->m_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
				}else if( bmode == 2 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;
					selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
					selem->m_renderstate[ D3DRS_ALPHAREF ] = 0x0;
					selem->m_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
					//selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
					//selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
					//selem->m_renderstate[ D3DRS_ALPHAREF ] = 0x08;
					//selem->m_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
				}else if( bmode == 103 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_DESTCOLOR;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ZERO;

				}else if( bmode == 104 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_REVSUBTRACT;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ZERO;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;

				}else if( bmode == 105 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

				}else if( bmode == 106 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;
				}else if( bmode == 107 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_DESTCOLOR;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_DESTCOLOR;
				}else if( bmode == 108 ){
					selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
					selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
					selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_DESTCOLOR;
				}
//_ASSERT( 0 );
			}else{
				int matno = -3;
				ret = selem->GetFirstMaterialNo( &matno );
				_ASSERT( !ret );

				CMQOMaterial* curmat;
				curmat = GetMaterialFromNo( m_mathead, matno );
				if( curmat ){
					curmat->blendmode = bmode;
				}

//_ASSERT( 0 );
			}
		}else if( (etype == SHDBILLBOARD) || (etype == SHDEXTLINE) ){
			selem->blendmode = bmode;

			if( bmode == 0 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
				//selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
				selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
				selem->m_renderstate[ D3DRS_ALPHAREF ] = 0x08;
				selem->m_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

							
			}else if( bmode == 1 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ONE;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;
				selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
				selem->m_renderstate[ D3DRS_ALPHAREF ] = 0x0;
				selem->m_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}else if( bmode == 2 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;
				//selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
				selem->m_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
				selem->m_renderstate[ D3DRS_ALPHAREF ] = 0x0;
				selem->m_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;
			}else if( bmode == 103 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_DESTCOLOR;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ZERO;

			}else if( bmode == 104 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_REVSUBTRACT;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ZERO;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;

			}else if( bmode == 105 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

			}else if( bmode == 106 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_INVDESTCOLOR;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCCOLOR;
			}else if( bmode == 107 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_DESTCOLOR;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_DESTCOLOR;
			}else if( bmode == 108 ){
				selem->m_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				selem->m_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
				selem->m_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_DESTCOLOR;
			}
		}
	}

	return 0;
}

int CShdHandler::SetElemRenderState( int srcseri, int statetype, DWORD value )
{

	int i;
	int etype;
	CShdElem* selem;

	if( (statetype < 0) || (statetype > D3DRS_BLENDOP) ){
		DbgOut( "shdhandler : SetElemRenderState : statetype error !!!\n" );
		return 1;
	}


	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
				(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
				(etype == SHDPOLYGON) || 
				(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) || (etype == SHDBILLBOARD) ||
				(etype == SHDMORPH) ){

				selem->m_renderstate[ statetype ] = value;
			
			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetElemRenderState : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetElemRenderState : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || 
			(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) || (etype == SHDBILLBOARD) || 
			(etype == SHDMORPH) ){

			selem->m_renderstate[ statetype ] = value;
		}
	}

	return 0;
}

int CShdHandler::SetElemTextureMinMagFilter( int srcseri, DWORD minfilter, DWORD magfilter )
{
	int i;
	int etype;
	CShdElem* selem;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
				(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
				(etype == SHDPOLYGON) || 
				(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) || (etype == SHDBILLBOARD) ){

				selem->m_minfilter = minfilter;
				selem->m_magfilter = magfilter;

			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetElemTextureMinMagFilter : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetElemTextureMinMagFilter : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || 
			(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) || (etype == SHDBILLBOARD) ){

			selem->m_minfilter = minfilter;
			selem->m_magfilter = magfilter;
		}
	}


	return 0;
}



/***
int CShdHandler::SetDirectionalLight( D3DXVECTOR3 dir )
{
	int i;
	int etype;
	CShdElem* selem;
	int ret = 0;

	D3DXVECTOR3 lightvec;
	D3DXVec3Normalize( &lightvec, &dir );

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;

		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || 
			(etype == SHDPOLYMESH)|| (etype == SHDPOLYMESH2) ){

			ret = selem->SetDirectionalLight( lightvec );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	return 0;
}
***/

int CShdHandler::CreateDispObj( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh, int needcalcbe )
{
	int i, ret;
	int etype;
	CShdElem* selem;

	ZeroMemory( &m_orgcenter, sizeof( D3DXVECTOR3 ) );

	int addcnt = 0;
	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;
		int isbone = ( selem->IsJoint() && (etype != SHDMORPH) );

		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || 
			(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) || 
			(etype == SHDINFSCOPE) ||		
			isbone ){

//DbgOut( "check!!!sh :CreateDispObj : start %d\n", i );
			ret = selem->CreateDispObj( m_seri2boneno, pd3dDevice, lpmh, this, m_TLmode, needcalcbe );
//			ret = selem->CreateDispObj( m_seri2boneno, pd3dDevice, lpmh, this, TLMODE_ORG, needcalcbe );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
//DbgOut( "check!!!sh :CreateDispObj : end %d\n", i );

			D3DXVECTOR3 tmpcenter;
			ret = selem->GetOrgCenter( &tmpcenter );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
//DbgOut( "check!!!sh :GetOrgCenter end %d\n", i );

			m_orgcenter += tmpcenter;
			addcnt++;
		}
	}

	if( addcnt != 0 ){
		m_orgcenter /= (float)addcnt;
	}


	// bboxは、他のオブジェクトの作成が全て終わってから（表示オブジェクトのCalcInfElemの後で）

	CShdElem* bbxelem;
	bbxelem = GetBBoxElem();
	if( bbxelem ){
		ret = bbxelem->CreateDispObj( m_seri2boneno, pd3dDevice, lpmh, this, m_TLmode, needcalcbe );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
				
				ret = selem->InitBBox();
				if( ret ){
					DbgOut( "sh : CreateDispObj : selem InitBBox error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}
	
	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;
		
		if( etype == SHDMORPH ){
			// morphのCreateはbaseとtargetのCreateが済んでから行う。
			ret = selem->CreateDMorph( pd3dDevice, lpmh, this, m_TLmode, needcalcbe );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

/***
int CShdHandler::CalcInfElem()
{
	int i, ret;

	for( i = 0; i < s2shd_leng; i++ ){
		ret = CalcInfElem( i );
	}

	return 0;
}
***/

int CShdHandler::CalcInfElem( int srcseri, int forceflag, int excludeflag, int onlymfolder  )
{
	int ret = 0;

	if( srcseri < 0 ){
		int seri;
		if( onlymfolder == 0 ){
			for( seri = 0; seri < s2shd_leng; seri++ ){
				CShdElem* tmpse = (*this)( seri );
				if( tmpse->type != SHDMORPH ){
					ret = CalcInfElem( seri, forceflag, excludeflag );
					_ASSERT( !ret );
				}
			}
		}
		//base, targetを計算してからmorphの計算
		for( seri = 0; seri < s2shd_leng; seri++ ){
			CShdElem* tmpse = (*this)( seri );
			if( tmpse->type == SHDMORPH ){
				ret = CalcInfElem( seri, forceflag, excludeflag );
				_ASSERT( !ret );
			}
		}

		return ret;
	}


	int etype;
	CShdElem* selem = (*this)( srcseri );

	if( forceflag == 1 )
		selem->m_loadbimflag = 0;//!!!!!!!!!!!!!!!!!!!!


	etype = selem->type;
	int isbone = ( selem->IsJoint() && (etype != SHDMORPH) );
	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) ||
	//	(etype == SHDPOLYGON) ||
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2)
	//	|| isbone ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDMORPH) || 
		(etype == SHDINFSCOPE) || (etype == SHDBBOX) || isbone ){

		if( (excludeflag == 0) || (selem->notuse != 1) ){
			ret = selem->CalcInfElem( this );
			if( ret ){
				DbgOut( "ShdHandler : CalcInfElem : selem->CalcInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdHandler::CalcInfElem1Vert( int srcseri, int srcvert )
{
	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh : CalcInfElem1Vert : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* selem = (*this)( srcseri );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

		ret = selem->CalcInfElem1Vert( this, srcvert );
		if( ret ){
			DbgOut( "ShdHandler : CalcInfElem1Vert : selem->CalcInfElem1Vert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CShdHandler::CreateBoneInfo( CMotHandler* lpmh )
{
	int i, ret;
	int etype;
	CShdElem* selem;

	CShdElem* parent_j;
	CShdElem* granpa_j;



	// 親から子へは枝分かれが難しいので、子から親を探す。

	for( i = s2shd_leng - 1; i >= 0; i-- ){
		selem = (*this)( i );
		etype = selem->type;
		int isend = 0;

		if( selem->IsJoint() && (selem->type != SHDMORPH) && (selem->notuse != 1) ){
			parent_j = FindUpperJoint( selem );
			if( parent_j ){
				granpa_j = FindUpperJoint( parent_j );

				ret = parent_j->AddBoneInfo( selem, granpa_j );
				if( ret ){
					DbgOut( "shdhandler : CreateBoneInfo : AddBoneInfo error !!!\n" );
					return 1;
				}
			}
		}		
	}

	//bonenoのセット。
	CMotionCtrl* mcptr;
	for( i = 1; i < lpmh->s2mot_leng; i++ ){
		mcptr = (*lpmh)( i );
		_ASSERT( mcptr );
		mcptr->boneno = -1;
		mcptr->morphno = -1;
	}

	CShdElem* firstelem;
	int boneno = 0;
	firstelem = (*this)( 1 );
	SetBoneNoReq( lpmh, firstelem, &boneno );
	m_lastboneno = boneno;


	int oldboneno = 0;
	firstelem = (*this)( 1 );
	SetOldBoneNoReq( lpmh, firstelem, &oldboneno );
	m_lastoldboneno = oldboneno;


	//morphnoのセット。
	int morphno = 0;
	firstelem = (*this)( 1 );
	SetMorphNoReq( lpmh, firstelem, &morphno );


	ret = ConvertInfluenceList( lpmh );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	/////// SWVmat (！！！bonenoのセットより後で！！！)

	if( m_seri2boneno )
		free( m_seri2boneno );
	m_seri2boneno = (int*)malloc( sizeof( int ) * s2shd_leng );
	if( !m_seri2boneno ){
		DbgOut( "sh : CreateBoneInfo : seri2boneno alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( m_boneno2seri )
		free( m_boneno2seri );
	m_boneno2seri = (int*)malloc( sizeof( int ) * m_lastboneno );
	if( !m_boneno2seri ){
		DbgOut( "sh : CreateBoneInfo : boneno2seri alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	for( i = 0; i < s2shd_leng; i++ ){
		CMotionCtrl* curmc;
		curmc = (*lpmh)( i );
		_ASSERT( curmc );

		if( curmc->boneno >= 0 ){
			*( m_seri2boneno + i ) = curmc->boneno;
			*( m_boneno2seri + curmc->boneno ) = i;
		}else{
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			*( m_seri2boneno + i ) = m_lastboneno;

			// SWVMatのインデックスとして使用するため、
			// 存在しない場合は、	SWVMatの最後の要素を参照するようにする。
		}
	}

	//DbgOut( "sh : aft CreateBoneInfo\n" );

	return 0;
}
int CShdHandler::ConvertInfluenceList( CMotHandler* lpmh )
{

//DbgOut( "ShdHandler : ConvertInfluenceList\n" );

	int* boneno2serino;
	boneno2serino = (int*)malloc( sizeof( int ) * lpmh->s2mot_leng );
	if( !boneno2serino ){
		_ASSERT( 0 );
		return 1;
	}
	int elemno;
	for( elemno = 0; elemno < lpmh->s2mot_leng; elemno++ ){
		*(boneno2serino + elemno) = -1;
	}
	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		CMotionCtrl* mc = (*lpmh)( elemno );

		int bno;
		if( m_sigmagicno >= SIGFILEMAGICNO_5 ){
			bno = mc->boneno;
		}else{
			bno = mc->oldboneno;//!!!!!!!!!!!!!!!!!!!!
		}

		if( bno >= 0 ){
			if( bno < lpmh->s2mot_leng ){
				*(boneno2serino + bno) = mc->serialno;
			}else{
				_ASSERT( 0 );
				return -1;
			}
		}
	}

	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		CShdElem* selem = (*this)( elemno );
		
		
		if( (selem->influencebonenum > 0) && (selem->neededconvert == 1) ){
			int listno;
			int templeng = 0;
			for( listno = 0; listno < selem->influencebonenum; listno++ ){
				int bno = *(selem->influencebone + listno);
				if( (bno >= 0) && (bno < s2shd_leng) ){

					/***
					int serino;
					int tmpseri = *(boneno2serino + bno);
					if( (tmpseri > 0) && (tmpseri < s2shd_leng) ){
						if( m_sigmagicno >= SIGFILEMAGICNO_5 ){
							serino = tmpseri;
						}else{
							CShdElem* tmpelem;
							tmpelem = (*this)( tmpseri );
							CShdElem* chilelem;
							chilelem = tmpelem->child;
							if( chilelem ){
								serino = chilelem->serialno;
							}else{
								serino = tmpseri;
							}
						}
						//int serino = *(boneno2serino + bno);
						if( (serino > 0) && (serino < s2shd_leng) ){
							*(selem->influencebone + templeng) = serino;

							templeng++;
						}
					}
					***/
					int serino = *(boneno2serino + bno);
					if( (serino > 0) && (serino < s2shd_leng) ){
						*(selem->influencebone + templeng) = serino;

						templeng++;
					}

				}
			}
			if( templeng != selem->influencebonenum ){

				//DbgOut( "ShdHandler : ConvertInfluenceList : realloc \n" );
				selem->influencebone = (int*)realloc( selem->influencebone, sizeof( int ) * templeng );
				if( (templeng != 0) && !(selem->influencebone) ){
					_ASSERT( 0 );
					return 1;
				}
				selem->influencebonenum = templeng;
			}
			selem->neededconvert = 0;
		}

		/////////////

		if( (selem->ignorebonenum > 0) && (selem->neededconvert_ig == 1) ){
			int listno;
			int templeng = 0;
			for( listno = 0; listno < selem->ignorebonenum; listno++ ){
				int bno = *(selem->ignorebone + listno);
				if( (bno >= 0) && (bno < s2shd_leng) ){
					/***
					int serino;
					int tmpseri = *(boneno2serino + bno);
					if( (tmpseri > 0) && (tmpseri < s2shd_leng) ){
						if( m_sigmagicno >= SIGFILEMAGICNO_5 ){
							serino = tmpseri;
						}else{
							CShdElem* tmpelem;
							tmpelem = (*this)( tmpseri );
							CShdElem* chilelem;
							chilelem = tmpelem->child;
							if( chilelem ){
								serino = chilelem->serialno;
							}else{
								serino = tmpseri;
							}
						}
						//int serino = *(boneno2serino + bno);
						if( (serino > 0) && (serino < s2shd_leng) ){
							*(selem->ignorebone + templeng) = serino;

							templeng++;
						}
					}
					***/
					int serino = *(boneno2serino + bno);
					if( (serino > 0) && (serino < s2shd_leng) ){
						*(selem->ignorebone + templeng) = serino;

						templeng++;
					}

				}
			}
			if( templeng != selem->ignorebonenum ){

				//DbgOut( "ShdHandler : ConvertInfluenceList : realloc \n" );
				selem->ignorebone = (int*)realloc( selem->ignorebone, sizeof( int ) * templeng );
				if( (templeng != 0) && !(selem->ignorebone) ){
					_ASSERT( 0 );
					return 1;
				}
				selem->ignorebonenum = templeng;
			}
			selem->neededconvert_ig = 0;
		}

	}

	free( boneno2serino );

	return 0;
}

/***
int CShdHandler::ConvertInfluenceList( CMotHandler* lpmh )
{

//DbgOut( "ShdHandler : ConvertInfluenceList\n" );

	int* boneno2serino;
	boneno2serino = (int*)malloc( sizeof( int ) * lpmh->s2mot_leng );
	if( !boneno2serino ){
		_ASSERT( 0 );
		return 1;
	}
	int elemno;
	for( elemno = 0; elemno < lpmh->s2mot_leng; elemno++ ){
		*(boneno2serino + elemno) = -1;
	}
	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		CMotionCtrl* mc = (*lpmh)( elemno );
		int bno = mc->boneno;
		if( bno >= 0 ){
			if( bno < lpmh->s2mot_leng ){
				*(boneno2serino + bno) = mc->serialno;
			}else{
				_ASSERT( 0 );
				return -1;
			}
		}
	}

	//int templeng = 0;
	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		CShdElem* selem = (*this)( elemno );
		if( (selem->influencebonenum > 0) && (selem->neededconvert == 1) ){
			int listno;
			for( listno = 0; listno < selem->influencebonenum; listno++ ){
				int bno = *(selem->influencebone + listno);
				if( (bno >= 0) && (bno < s2shd_leng) ){
					int serino = *(boneno2serino + bno);
					if( (serino > 0) && (serino < s2shd_leng) ){
						*(selem->influencebone + listno) = serino;
						//templeng++;
					}else{
						*(selem->influencebone + listno) = -1;
					}
				}else{
					*(selem->influencebone + listno) = -1;
				}
			}
			selem->neededconvert = 0;
		}
	}

	free( boneno2serino );

	return 0;
}
***/
/***
int CShdHandler::ConvertInfluenceList( CMotHandler* lpmh )
{

//DbgOut( "ShdHandler : ConvertInfluenceList\n" );

	int* boneno2serino;
	boneno2serino = (int*)malloc( sizeof( int ) * lpmh->s2mot_leng );
	if( !boneno2serino ){
		_ASSERT( 0 );
		return 1;
	}
	int elemno;
	for( elemno = 0; elemno < lpmh->s2mot_leng; elemno++ ){
		*(boneno2serino + elemno) = -1;
	}
	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		CMotionCtrl* mc = (*lpmh)( elemno );
		int bno = mc->boneno;
		if( bno >= 0 ){
			if( bno < lpmh->s2mot_leng ){
				*(boneno2serino + bno) = mc->serialno;
			}else{
				_ASSERT( 0 );
				return -1;
			}
		}
	}

	int templeng = 0;
	for( elemno = 1; elemno < lpmh->s2mot_leng; elemno++ ){
		CShdElem* selem = (*this)( elemno );
		if( (selem->influencebonenum > 0) && (selem->neededconvert == 1) ){
			int listno;
			for( listno = 0; listno < selem->influencebonenum; listno++ ){
				int bno = *(selem->influencebone + listno);
				if( (bno >= 0) && (bno < s2shd_leng) ){
					int serino = *(boneno2serino + bno);
					if( (serino > 0) && (serino < s2shd_leng) ){
						*(selem->influencebone + templeng) = serino;

//DbgOut( "ShdHandler : ConvertInfluenceList : list%d - seri %d\n", templeng, serino );

						templeng++;
					}
				}
			}
			if( templeng != selem->influencebonenum ){
				selem->influencebone = (int*)realloc( selem->influencebone, sizeof( int ) * templeng );
				if( (templeng != 0) && !(selem->influencebone) ){
					_ASSERT( 0 );
					return 1;
				}
				selem->influencebonenum = templeng;
			}
			selem->neededconvert = 0;
		}
	}

	free( boneno2serino );

	return 0;
}
***/


void CShdHandler::SetMorphNoReq( CMotHandler* lpmh, CShdElem* selem, int* morphnoptr )
{
	int etype = selem->type;
	if( etype == SHDMORPH ){
		int elemno = selem->serialno;
		CMotionCtrl* mcptr = (*lpmh)( elemno );
		mcptr->morphno = *morphnoptr;
		(*morphnoptr)++;
	}

	CShdElem* chilelem = selem->child;
	if( chilelem ){
		SetMorphNoReq( lpmh, chilelem, morphnoptr );
	}

	CShdElem* broelem = selem->brother;
	if( broelem ){
		SetMorphNoReq( lpmh, broelem, morphnoptr );
	}
}

void CShdHandler::SetBoneNoReq( CMotHandler* lpmh, CShdElem* selem, int* bonenoptr )
{
	int elemtype = selem->type;
	if( selem->IsJoint() && (elemtype != SHDMORPH) && (selem->notuse == 0) ){
		CPart* partptr = selem->part;
		//if( partptr && (partptr->bonenum >= 1) ){
		if( partptr ){
			int elemno = selem->serialno;
			CMotionCtrl* mcptr = (*lpmh)( elemno );
			mcptr->boneno = *bonenoptr;
			(*bonenoptr)++;
		}
	}

	CShdElem* chilelem = selem->child;
	if( chilelem ){
		SetBoneNoReq( lpmh, chilelem, bonenoptr );
	}

	CShdElem* broelem = selem->brother;
	if( broelem ){
		SetBoneNoReq( lpmh, broelem, bonenoptr );
	}
}

void CShdHandler::SetOldBoneNoReq( CMotHandler* lpmh, CShdElem* selem, int* bonenoptr )
{
	int elemtype = selem->type;
	if( selem->IsJoint() && (elemtype != SHDMORPH) && (selem->notuse == 0) ){
		CPart* partptr = selem->part;
		if( partptr && (partptr->bonenum >= 1) ){
			int elemno = selem->serialno;
			CMotionCtrl* mcptr = (*lpmh)( elemno );
			mcptr->oldboneno = *bonenoptr;
			(*bonenoptr)++;
		}
	}

	CShdElem* chilelem = selem->child;
	if( chilelem ){
		SetOldBoneNoReq( lpmh, chilelem, bonenoptr );
	}

	CShdElem* broelem = selem->brother;
	if( broelem ){
		SetOldBoneNoReq( lpmh, broelem, bonenoptr );
	}
}


int CShdHandler::DestroyBoneInfo( CMotHandler* lpmh )
{
	int i;
	int ret = 0;
	CShdElem* selem;
	//CMotionCtrl* melem;
	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			CPart* partptr;
			partptr = selem->part;
			if( !partptr ){
				_ASSERT( 0 );
				return 1;
			}
			ret += partptr->DestroyBoneInfo();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	return ret;
}
int CShdHandler::DumpBoneInfo( CTreeHandler2* srclpth )
{
	int i, ret;
	CShdElem* selem;
	int etype;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;

		if( selem->IsJoint() && (selem->type != SHDMORPH)){
			ret = selem->DumpBoneInfo( srclpth );
			if( ret ){
				DbgOut( "CShdHandler : DumpBoneInfo : selem->DumpBoneInfo error !!!\n" );
				return 1;
			}
		}
	}

	return 0;
}

CShdElem* CShdHandler::FindUpperJoint( CShdElem* curelem, int findnotuse )
{
	CShdElem* retelem = 0;
	CShdElem* parelem = 0;

	int useflag;

	while( 1 ){
		parelem = curelem->parent;
		if( !parelem ){
			retelem = 0;
			break;
		}else{

			if( findnotuse ){
				useflag = 1;
			}else{
				if( parelem->notuse == 0 ){
					useflag = 1;
				}else{
					useflag = 0;
				}
			}

			//if( parelem->IsJoint() && (parelem->type != SHDMORPH) && (parelem->notuse != 1) ){
			if( parelem->IsJoint() && (parelem->type != SHDMORPH) && (useflag == 1) ){
				retelem = parelem;
				break;
			}else{
				curelem = parelem;
			}
		}
	}
	return retelem;
}



int CShdHandler::DumpShd( CMotHandler* lpmh, int dumpflag, char* fname )
{
	int ret = 0;
	//int serino;
	CShdElem* rootelem = (*this)( 0 );


	HANDLE hfile;

	if( !fname ){
		hfile = CreateFile( (LPCTSTR)"dumpshd.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	}else{
		hfile = CreateFile( (LPCTSTR)fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	}
	if( hfile == INVALID_HANDLE_VALUE ){
		ret = 1;
		goto dumpexit;
	}	

	/***
	// bro, child のチェック
	for( serino = 0; serino < s2shd_leng; serino++ ){
		ret = ((*this)(serino))->Dbg_Dump( hfile, thandler );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	***/
	
	if( rootelem ){
		rootelem->DumpReq( hfile, thandler, lpmh, &ret );
		if( ret ){
			DbgOut( "CShdHandler : DumpShd : DumpReq error %d\n", ret );
			_ASSERT( 0 );

		}
	}
	

	goto dumpexit;

dumpexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
	}
	
	return ret;
}

/***
int CShdHandler::DumpText( HANDLE hfile, int srcserino, int srctype )
{
	int ret = 0;

DbgOut( "CShdHandler : DumpText : srcserino %d, srctype %d\n", srcserino, srctype );

	CShdElem* dumpelem = (*this)( srcserino );
	
	ret = dumpelem->DumpElem( hfile, srctype );

	_ASSERT( !ret );

	return ret;

}
***/
int CShdHandler::AddS2Shd( int srcserino, int srcbrono, int srcdepth, int srctype )
{


	//int curshdno;
	int saveleng = s2shd_leng;
	int ret = 0;
	CShdElem* newshd = 0;
	CShdElem* curshd = 0;
	CShdElem* parshd = 0;

	newshd = new CShdElem( srcserino );
	if( !newshd ){
		DbgOut( "CShdHandler : AddS2Shd : newshd alloc error !!!\n" );
		return 1;
	}

	if( srcserino >= s2shd_leng )
		s2shd_leng = srcserino + 1;

	if( s2shd_leng > s2shd_allocleng ){
		while( s2shd_leng > s2shd_allocleng )
			s2shd_allocleng += 1024;

		s2shd = (CShdElem**)realloc( s2shd, sizeof( CShdElem* ) * s2shd_allocleng );
		if( !s2shd ){
			DbgOut( "CShdHandler : AddS2Shd : s2shd alloc error !!!\n" );
			return 1;
		}
	}
	*(s2shd + srcserino) = newshd;

	/***
	if( srcserino >= s2shd_leng ){
		s2shd_leng = srcserino + 1;
		// set2index
		s2shd = (CShdElem**)realloc( s2shd, sizeof( CShdElem* ) * s2shd_leng );
		if( !s2shd ){
			DbgOut( "CShdHandler : AddS2Shd : s2shd alloc error !!!\n" );
			return 1;
		}
	}
	*(s2shd + srcserino) = newshd;
	***/

	newshd->depth = srcdepth;
	newshd->brono = srcbrono;
	newshd->type = srctype;

	return 0;
}
int CShdHandler::SetChain( int offset )
{
	int ret = 0;
	int serino, treeleng;
	CTreeElem2* curte = 0;
	CShdElem* curshd = 0;

	treeleng = thandler->s2e_leng;

	if( offset == 0 ){
		for( serino = 0; serino < treeleng; serino++ ){
			curte = (*thandler)( serino );
			curshd = (*this)( serino );

			ret = curshd->CopyChain( this, curte );		
			if( ret ){
				_ASSERT( 0 );
				return ret;
			}
		}
	}else{
		CTreeElem2* te;
		CShdElem* headelem;
		te = (*thandler)( offset );
		headelem = (*this)( offset );
		ret = headelem->CopyChain( this, te );		
		if( ret ){
			_ASSERT( 0 );
			return ret;			
		}

		//serialno 1 のbrotherに先頭データをchain
		CShdElem* broelem;
		CShdElem* lastbro = 0;
		broelem = (*this)( 1 );
		while( broelem ){
			lastbro = broelem;
			broelem = broelem->brother;			
		}
		if( lastbro ){
			lastbro->brother = headelem;
		}else{
			_ASSERT( 0 );
			return 1;
		}
		headelem->sister = lastbro;

DbgOut( "ShdHandler : SetChain offset %d : head %s, lastbro %s\n",
	   offset,
	   ((*thandler)(headelem->serialno))->name,
	   ((*thandler)(lastbro->serialno))->name );

		//先頭データ以下のデータ
		for( serino = offset + 1; serino < treeleng; serino++ ){
			curte = (*thandler)( serino );
			curshd = (*this)( serino );

			ret = curshd->CopyChain( this, curte );		
			if( ret ){
				_ASSERT( 0 );
				break;			
			}
		}
	}
	return ret;
}

int CShdHandler::SetClockwise()
{
	CShdElem* curshd;

	curshd = (*this)( 1 );
	curshd->SetClockwiseReq( 0 );

	return 0;
}
int CShdHandler::ClearInvFaceCnt()
{
	CShdElem* curshd;
	int shdno;
	int ret;
	for( shdno = 0; shdno < s2shd_leng; shdno++ ){
		curshd = (*this)( shdno );
		ret = curshd->ClearInvFaceCnt();
		_ASSERT( !ret );
	}
	return 0;
}

int CShdHandler::SetColors()
{
	int ret = 0;
	int curseri;
	int hascolor = 0;
	int parcolor = 0;
	int skip;

	CShdElem* curshd = 0;
	CShdElem* parshd = 0;

	CMeshInfo* parmi = 0;
	CMeshInfo* curmi = 0;

	DbgOut( "CShdColor : SetColors \n" );

	for( curseri = 1; curseri < s2shd_leng; curseri++ ){
		curshd = (*this)( curseri );
		
		if( (curshd->type == SHDINFSCOPE) || (curshd->type == SHDDESTROYED) ){
			continue;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		skip = 0;

		curmi = curshd->GetMeshInfo();
		if( curmi ){
			hascolor = curmi->HasColor();

//if( curseri == 56 ){
//	DbgOut( "SetColor : CURSERI 56 : hascolor %d\n", hascolor );	
//}
		
		}else{
			hascolor = 0;
			skip = 1; // curmiがないときは、なにもしない。
		}
		
		if( (hascolor == 0) && !skip ){
			parshd = curshd->parent;

			if( parshd ){
				parmi = parshd->GetMeshInfo();
				if( parmi ){
					parcolor = parmi->HasColor();
				}else{
					parcolor = 0;
				}

				if( parcolor ){
					ret = curmi->CopyMaterial( parmi );
					if( ret ){
						DbgOut( "CShdHandler : SetColors : CopyMaterial error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ret = curmi->SetMem( 1, MESHI_HASCOLOR );
					if( ret ){
						DbgOut( "CShdHandler : SetColors : curmi->SetMem error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					ret = curmi->SetDefaultMaterial();
					if( ret ){
						DbgOut( "CShdHandler : SetColors : SetDefaultMaterial error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}					
				}
			}else{
				ret = curmi->SetDefaultMaterial();
				if( ret ){
					DbgOut( "CShdHandler : SetColors : SetDefaultMaterial error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}					
			}
		}
	}
	return 0;
}

int CShdHandler::SetInfluenceList( int srcserino, int* listptr, int listleng )
{
	CShdElem* curelem;

	curelem = (*this)( srcserino );
	if( !curelem ){
		_ASSERT( 0 );
		return 1;
	}

	if( curelem->influencebonenum != listleng ){
		curelem->influencebone = (int*)realloc( curelem->influencebone, sizeof( int ) * listleng );
		if( (listleng != 0) && !(curelem->influencebone) ){
			_ASSERT( 0 );
			return 1;
		}
		curelem->influencebonenum = listleng;
	}

	if( listleng > 0 )
		MoveMemory( curelem->influencebone, listptr, sizeof( int ) * listleng );
	
	return 0;
}

int CShdHandler::SetIgnoreList( int srcserino, int* listptr, int listleng )
{
	CShdElem* curelem;

	curelem = (*this)( srcserino );
	if( !curelem ){
		_ASSERT( 0 );
		return 1;
	}

	if( curelem->ignorebonenum != listleng ){
		curelem->ignorebone = (int*)realloc( curelem->ignorebone, sizeof( int ) * listleng );
		if( (listleng != 0) && !(curelem->ignorebone) ){
			_ASSERT( 0 );
			return 1;
		}
		curelem->ignorebonenum = listleng;
	}

	if( listleng > 0 )
		MoveMemory( curelem->ignorebone, listptr, sizeof( int ) * listleng );
	
	return 0;
}

int CShdHandler::SetTexRule( int srcserino , int srcrule )
{
	CShdElem* curelem;
	int ret;

	curelem = (*this)( srcserino );
	if( !curelem )
		return 1;

	ret = curelem->SetTexRule( srcrule );
	_ASSERT( !ret );
	return ret;
}

int CShdHandler::SetNoSkinning( int srcserino, int srcint )
{
	CShdElem* curelem;
	int ret;

	curelem = (*this)( srcserino );
	if( !curelem )
		return 1;

	ret = curelem->SetNoSkinning( srcint );
	_ASSERT( !ret );
	return ret;
}

int CShdHandler::SetAllNoSkinning( int srcint )
{
	CShdElem* curelem;
	int ret = 0;

	int i;
	for( i = 0; i < s2shd_leng; i++ ){

		curelem = (*this)( i );
		if( !curelem ){
			_ASSERT( 0 );
			return 1;
		}

		ret = curelem->SetNoSkinning( srcint );
		_ASSERT( !ret );

	}

	return ret;
}


int CShdHandler::SetTexName( int srcserino, char* srctexname )
{
	CShdElem* curelem;
	int ret;

	curelem = (*this)( srcserino );
	if( !curelem )
		return 1;
	
	ret = curelem->SetTexName( srctexname );
	_ASSERT( !ret );
	return ret;
}

int CShdHandler::SetExtTex( int srcserino, unsigned char srctexnum, unsigned char srctexmode, unsigned char srctexrep, unsigned char srctexstep, char** srctexname )
{
	CShdElem* curelem;
	int ret;

	curelem = (*this)( srcserino );
	if( !curelem )
		return 1;
	
	if( srctexnum > MAXEXTTEXNUM ){
		DbgOut( "ShdHandler : SetExtTex : texnum too large error !!!\n" );
		srctexnum = MAXEXTTEXNUM;//!!!!!
	}

	ret = curelem->SetExtTex( srctexnum, srctexmode, srctexrep, srctexstep, srctexname );
	_ASSERT( !ret );
	return ret;

}

int CShdHandler::SetTexName()
{
	int i;
	int ret = 0;
	CShdElem* parelem = 0;
	CShdElem* curelem = 0;
	char* curtex = 0;
	char* partex = 0;
	int parrepx, parrepy;
	int partrans;
	float paruanime, parvanime;
	float paralpha;

	for( i = 1; i < s2shd_leng; i++ ){
		curelem = (*this)( i );
		parelem = curelem->parent;
		curtex = curelem->texname;
		if( !curtex && parelem ){
			partex = parelem->texname;
			parrepx = parelem->texrepx;
			parrepy = parelem->texrepy;
			partrans = parelem->transparent;
			paruanime = parelem->uanime;
			parvanime = parelem->vanime;
			paralpha = parelem->alpha;
			if( partex ){
				ret += curelem->SetTexName( partex );
				ret += curelem->SetTexRepetition( parrepx, parrepy );
				ret += curelem->SetTexTransparent( partrans );
				ret += curelem->SetUVAnime( paruanime, parvanime );
				ret += curelem->SetAlpha( paralpha );
				if( ret ){
					_ASSERT( 0 );
					return ret;
				}
			}
		}
	}
	return 0;
}

int CShdHandler::InitColor( D3DXVECTOR3 vEyePt, CMotHandler* lpmh, int serino )
{
	int ret = 0;
	CShdElem* selem = (*this)( serino );
	int etype = selem->type;

	if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
		(etype == SHDSPHERE) || (etype == SHDEXTRUDE) ||
		(etype == SHDPOLYGON) ||
		(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDINFSCOPE) || (etype == SHDBBOX) ){
		
		ret = selem->InitColor( m_seri2boneno, vEyePt, m_shader, m_overflow, &m_mc, lpmh, m_TLmode );
		_ASSERT( !ret );
	}
	return ret;
}

int CShdHandler::SetDispFlag( int boneflag )
{
	CShdElem* selem;
	CShdElem* parelem;
	int i, etype;
	int ret = 0;
	int inmorph = 0;

	for( i = 1; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;
		int isbone;
		if( boneflag )
			isbone = ( selem->IsJoint() && (etype != SHDMORPH) );
		else
			isbone = 0;

		parelem = selem->parent;
		if( parelem && (parelem->type == SHDMORPH) ){
			//if( selem->m_mtype == M_TARGET ){
				inmorph = 1;
			//}else{
			//	inmorph = 0;
			//}
		}else{
			inmorph = 0;
		}


		if( 
			(inmorph == 0) && 
			
			( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ||
			(etype == SHDBILLBOARD) || (etype == SHDEXTLINE) || 
			(etype == SHDINFSCOPE) || (etype == SHDMORPH) || (etype == SHDBBOX) ||
			isbone )
		){

			//if( selem->m_mtype != M_TARGET ){
				ret += selem->SetDispFlag( 1 );
			//}else{
			//	ret += selem->SetDispFlag( 0 );
			//}
			_ASSERT( !ret );
		}else{
			ret += selem->SetDispFlag( 0 );
			_ASSERT( !ret );
		}
	}

	//ret += SetMorphDispFlag();
	//_ASSERT( !ret );

	return ret;
}

/***
int CShdHandler::SetMorphDispFlag()
{
	int i;
	int etype;
	CShdElem* selem;
	CShdElem* chilelem;
	CShdElem* broelem;
	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;

		if( etype == SHDMORPH ){
			selem->dispflag = 1;//!!!!!
			chilelem = selem->child;
			if( chilelem ){
				chilelem->dispflag = 0;//!!!!!				
				broelem = chilelem->brother;
				while( broelem ){
					broelem->dispflag = 0;//!!!!!
					broelem = broelem->brother;
				}
			}
		}	
	}
	return 0;
}
***/

int CShdHandler::SetIsSelected( int selno, int infscopevisible, int pmsonomama, int curboneonly )
{
	int i;
	CShdElem* selem;

	//clear 
	for( i = 1; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		if( ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) && pmsonomama ){
			//いじらない
		}else{
			selem->isselected = 0;
		}
	}

	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( iselem && infscopevisible ){
		iselem->isselected = 1;
	}


	//set
	if( (selno < 0) || (selno >= s2shd_leng) ){
		return 0;//!!!!!!!!!!
	}
	selem = (*this)( selno );
	if( curboneonly == 0 ){
		SetIsSelectedReq( selem, 0 );
	}else{
		selem->isselected = 1;		
	}

	return 0;
}

void CShdHandler::SetIsSelectedReq( CShdElem* srcselem, int setbroflag )
{
	srcselem->isselected = 1;

	CShdElem* chilelem;
	chilelem = srcselem->child;
	if( chilelem ){
		SetIsSelectedReq( chilelem, 1 );
	}

	if( setbroflag ){
		CShdElem* broelem;
		broelem = srcselem->brother;
		if( broelem ){
			SetIsSelectedReq( broelem, 1 );
		}
	}
}


int CShdHandler::GetFirstScale( D3DXMATRIX* firstscale )
{
	int i;
	CShdElem* selem;
	CD3DDisp* dispptr;

	D3DXMATRIX* retmat = 0;

	for( i = 1; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		
		if( (selem->dispflag || (selem->m_mtype == M_BASE)) && !selem->IsJoint() && (dispptr = selem->d3ddisp) ){
			retmat = &(dispptr->m_scalemat);
			break;
		}

	}

	if( retmat ){
		*firstscale = *retmat;
	}else{
		D3DXMatrixIdentity( firstscale );
	}

	return 0;
}


int CShdHandler::TransformAndRenderIKT( D3DXVECTOR3 vEyePt, LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* lpmh,
			CShdHandler* lpmodelsh, CMotHandler* lpmodelmh,
			CShdHandler* lpbmsh,
			int motid, int frameno,
			D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXMATRIX matProjX, GUARDBAND gband )
{
	int ret;
	HRESULT hr;


	int i;
	CShdElem* curelem;
	CShdElem* bonemarkelem = 0;
	for( i = 0; i < lpbmsh->s2shd_leng; i++ ){
		curelem = (*lpbmsh)( i );
		if( curelem->type == SHDEXTLINE ){
			bonemarkelem = curelem;
			break;
		}
	}
	if( !bonemarkelem ){
		DbgOut( "shandler : TransformAndRenderIKT : EXTLINE object not found in %s error !!!\n", s_bonemarkmqo );
		_ASSERT( 0 );
		return 1;
	}
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjX );							
	if( g_curtex0 != NULL ){
		pd3dDevice->SetTexture( 0, NULL );
		g_curtex0 = NULL;
	}
	lpbmsh->SetExtLineColor( -1, 255, 255, 255, 0 );


	hr = pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );


	int modelseri;
	CShdElem* modelse;
	for( modelseri = 0; modelseri < lpmodelsh->s2shd_leng; modelseri++ ){
		modelse = (*lpmodelsh)( modelseri );

		if( modelse->IsJoint() && (modelse->type != SHDMORPH) && (modelse->m_iktarget != 0) ){

			if( modelse->m_iktarget == 1 ){
				ret = SetDispDiffuse( -1, 20, 255, 251, 0, -1, 1 );
				_ASSERT( !ret );
			}else if( modelse->m_iktarget == 2 ){
				ret = SetDispDiffuse( -1, 100, 100, 100, 0, -1, 1 );
				_ASSERT( !ret );
			}

/// iktarget sphere
			D3DXMATRIX iktworld;
			D3DXMatrixIdentity( &iktworld );
			iktworld._41 = modelse->m_iktargetpos.x;
			iktworld._42 = modelse->m_iktargetpos.y;
			iktworld._43 = modelse->m_iktargetpos.z;

			ret = ChkInView( lpmh, &iktworld, &matView, &matProjX );
			if( ret ){
				DbgOut( "shandler : TransformAndRenderIKT : ChkInView error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//D3DXVECTOR3 vEyePt( 0.0f, 0.0f, 0.0f );


			ret = TransformDispData( vEyePt, lpmh, &iktworld, &matView, &matProjX, 1, 0 );
			if( ret ){
				DbgOut( "shandler : TransformAndRenderIKT : TransformDispData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			//ret = UpdateVertexBuffer( 0 );
			//if( ret ){
			//	DbgOut( "shandler : TransformAndRenderIKT : UpdateVertexBuffer error !!!\n" );
			//	_ASSERT( 0 );
			//	return 1;
			//}

			ret = Render( 0, 0, pd3dDevice, 0, POLYDISP_ALL, lpmh, motid, iktworld, matView, matProjX, vEyePt, 0, 0, NOGLOW );
			if( ret ){
				DbgOut( "shandler : TransformAndRenderIKT : Render error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

/// bone mark
			D3DXVECTOR3 parpos;
			ret = lpmodelsh->GetBonePos( modelseri, 1, motid, frameno, &parpos, lpmodelmh, matWorld, 1 );
			if( ret ){
				DbgOut( "shandler : TransformAndRenderIKT : GetBonePos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			D3DXVECTOR3 chilpos;
			chilpos = modelse->m_iktargetpos;


			D3DXMATRIX bmmat;
			D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
			float baseleng = 50.0f;


			D3DXVECTOR3 par2chil;
			par2chil = chilpos - parpos;
			float dist;
			dist = D3DXVec3Length( &par2chil );
			D3DXVec3Normalize( &par2chil, &par2chil );

			CQuaternion rotq;
			rotq.RotationArc( basevec, par2chil );

			D3DXMATRIX rotm;
			rotm = rotq.MakeRotMatX();


			D3DXMATRIX scalem;
			float scale;
			scale = dist / baseleng;
			D3DXMatrixScaling( &scalem, scale, scale, scale );

			bmmat = scalem * rotm;

			bmmat._41 = parpos.x;
			bmmat._42 = parpos.y;
			bmmat._43 = parpos.z;

			pd3dDevice->SetTransform( D3DTS_WORLD,  &bmmat );

			ret = bonemarkelem->Render( 0, 0, pd3dDevice, this, NOGLOW );
			if( ret ){
				DbgOut( "CShdHandler : TransformAndRenderIKT : bmelem Render error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


		}

	}

	hr = pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

	return 0;
}


int CShdHandler::SetPointListForShadow( CVec3List* plist )
{
	int i;
	CShdElem* selem;
	int dispflag;



	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
		int type;
		type = selem->type;
		
		if( (dispflag || (selem->m_mtype == M_BASE) )&& (selem->notuse != 1) && ((type == SHDPOLYMESH) || (type == SHDPOLYMESH2) || (type == SHDBILLBOARD)) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){ 
				
				if( (type == SHDPOLYMESH) && (selem->polymesh) && (selem->polymesh->billboardflag == 1) ){
					
					//処理をスキップ
					//SHDBILLBOARDに処理を任せる。

				}else if( type == SHDBILLBOARD ){
					CBillboard* bb;
					bb = selem->billboard;
					_ASSERT( bb );

					int bbelemno;
					for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
						CBillboardElem* bbelem;
						bbelem = bb->bbarray + bbelemno;

						if( (bbelem->useflag == 1) && (bbelem->dispflag == 1) && bbelem->selem ){

							int addflag = 0;
							if( bbelem->selem->m_shadowinflag == SHADOWIN_PROJ ){
								if( bbelem->selem->curbs.visibleflag != 0 ){
									addflag = 1;
								}else{
									addflag = 0;
								}
							}else if( bbelem->selem->m_shadowinflag == SHADOWIN_ALWAYS ){
								addflag = 1;
							}else if( bbelem->selem->m_shadowinflag == SHADOWIN_NOT ){
								addflag = 0;
							}else if( bbelem->selem->m_shadowinflag == SHADOWIN_EXCLUDE ){
								addflag = 0;
							}
						
							if( addflag == 1 ){
								plist->AddBSphere( &(bbelem->selem->curbs), 1.5f );
							}
						}

					}
				}else{

					int addflag = 0;
					if( selem->m_shadowinflag == SHADOWIN_PROJ ){
						if( selem->curbs.visibleflag != 0 ){
							addflag = 1;
						}else{
							addflag = 0;
						}
					}else if( selem->m_shadowinflag == SHADOWIN_ALWAYS ){
						addflag = 1;
					}else if( selem->m_shadowinflag == SHADOWIN_NOT ){
						addflag = 0;
					}else if( selem->m_shadowinflag == SHADOWIN_EXCLUDE ){
						addflag = 0;
					}

					if( addflag == 1 ){
						plist->AddBSphere( &(selem->curbs), 1.0f );
					}
				}
			}
		}
	}

	return 0;
}


int CShdHandler::TransformDispData( D3DXVECTOR3 vEyePt, CMotHandler* lpmh, 
	D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, 
	int lightflag, int tra_boneonly )
{
	int i, ret;
	int dispflag;
	CShdElem* selem;
	//float uanime, vanime;
	int transcnt = 0;


	D3DXMATRIX mat;
	mat = *matWorld * *matView;
	m_mc.InitCache( &mat );

	D3DXMATRIX firstscale;
	GetFirstScale( &firstscale );

	/***
	ret = InitBoundaryBox();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	***/


	SetCurDS();

//	float calaspect;
//	calaspect = (float)dwClipWidth / (float)dwClipHeight;

/***
	//ChkInViewに移動

	// SHDBBOXの変換
***/

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;

		
		int isbone;
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			isbone = 1;
		}else{
			isbone = 0;
		}


		int tra_checkflag;
		if( tra_boneonly == 1 ){
			if( isbone )
				tra_checkflag = 1;
			else
				tra_checkflag = 0;
		}else{
			tra_checkflag = 1;
		}
		
		//transformdispdata
		int vflag;
		if( m_inRDBflag == 0 ){
			if( selem->curbs.visibleflag || isbone || (selem->type == SHDBILLBOARD) || (selem->type == SHDINFSCOPE) )
				vflag = 1;
			else
				vflag = 0;

			if( selem->type == SHDMORPH ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}
		}else{
			vflag = 1;
		}
//{
//	if( selem->type == SHDBILLBOARD ){
//		DbgOut( "shdhandler : TransformDispData : check billboard : vflag %d, dispflag %d, notuse %d, tra_checkflag %d, dispswitchno %d\n",
//			vflag, dispflag, selem->notuse, tra_checkflag, selem->dispswitchno );
//	}
//}


		if( vflag && (dispflag || (selem->m_mtype == M_BASE))&& (selem->notuse != 1) && tra_checkflag ){
		//if( dispflag && (selem->notuse != 1) ){
			//！！！更新するときは、TransformDispDataReqも！！！

			
			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){  

				if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) || 
					(selem->type == SHDINFSCOPE) || (selem->type == SHDMORPH) ){
					ret = selem->TransformDispData( m_seri2boneno, vEyePt, &firstscale, 
						lpmh, matWorld, matView, matProj, lightflag );
					if( ret ){
						DbgOut( "CShdHandler : TransformDispData error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					transcnt++;
				}
			}
			
//			uanime = selem->uanime;
//			vanime = selem->vanime;
//			if( (uanime != 0.0f) || (vanime != 0.0f) ){
//				ret = selem->MoveTexUV( m_TLmode );
//				_ASSERT( !ret );
//			}

			CMQOMaterial* curmat;
			curmat = m_mathead;
			while( curmat ){
				if( (curmat->uanime != 0.0f) || (curmat->vanime != 0.0f) ){
					if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
						ret = selem->MoveMaterialUV( curmat );
						_ASSERT( !ret );
					}
				}
				curmat = curmat->next;
			}
		}
	}

	return 0;
}
int CShdHandler::UpdateVertexBuffer( int framecnt )
{
	int i, ret;
	int dispflag;
	CShdElem* selem;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );


		int vflag;
/***
		if( (g_useGPU == 0) && (m_inRDBflag == 0) ){
			if( selem->curbs.visibleflag  )
				vflag = 1;
			else
				vflag = 0;

			if( selem->type == SHDMORPH ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}

		}else{
			vflag = 1;
		}
***/
		vflag = 1;
		
		dispflag = selem->dispflag;
		if( vflag && ( dispflag || (selem->m_mtype == M_BASE) ) && !( selem->IsJoint() ) && (selem->type != SHDBILLBOARD) && (selem->type != SHDEXTLINE)){
			ret = selem->UpdateVertexBuffer( framecnt, m_TLmode, m_shader );
			if( ret ){
				DbgOut( "CShdHandler : UpdateVertexBuffer error %d!!!\n", m_TLmode );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	CShdElem* bbxelem;
	bbxelem = GetBBoxElem();
	if( bbxelem ){
		ret = bbxelem->UpdateVertexBuffer( framecnt, m_TLmode, m_shader );
		if( ret ){
			DbgOut( "sh : UpdateVertexBuffer bbx error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}




#if !plugin

	int CShdHandler::RenderShadowMap0( LPDIRECT3DDEVICE9 pd3dDevice, int polydispmode, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 vEyePt, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2 )
	{

		int i, ret;
		int dispflag;
		CShdElem* selem;
		int isfirstrender = 1;
		CMeshInfo* mi = 0;
		int polydisp;
		
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if( (m_inRDBflag == 0) && (lpmh->m_curbs.visibleflag == 0) ){
			//_ASSERT( 0 );
			return 0;
		}

		int vflag;
		SetCurDS();


		// extline用
		if( m_TLmode == TLMODE_D3D ){
			//pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );
			D3DXMATRIX fscale, newwmat;
			GetFirstScale( &fscale );
			newwmat = fscale * *matWorld;
			//pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );
			pd3dDevice->SetTransform( D3DTS_WORLD,  &newwmat );
			pd3dDevice->SetTransform( D3DTS_VIEW, matView );
			pd3dDevice->SetTransform( D3DTS_PROJECTION, matProj );							
		}


		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			dispflag = selem->dispflag;

			if( polydispmode == POLYDISP_ALL )
				polydisp = 1;
			else
				polydisp = selem->isselected;

			if( (m_inRDBflag == 0) && (selem->type != SHDBILLBOARD) && (selem->type != SHDEXTLINE) && (selem->type != SHDINFSCOPE) )
				vflag = selem->curbs.visibleflag;
			else
				vflag = 1;
			if( selem->type == SHDMORPH ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}


			if( vflag && dispflag && polydisp 
				&& !( selem->IsJoint() && (selem->type != SHDMORPH) ) 
				&& (selem->notuse != 1) 
				&& (selem->invisibleflag == 0) && (selem->m_mtype == M_NONE) ){										

				if( selem->type == SHDBILLBOARD ){	
					ret = selem->RenderBillboardShadowMap0( this, pd3dDevice, matWorld, matView, matProj, matWorld1, matWorld2, vEyePt );
					if( ret ){
						DbgOut( "shdhandler : RenderShadowMap0 : RenderBillboardShadowMap0 error !!1\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					DWORD dispswitchno = selem->dispswitchno;
					if( (m_curds + dispswitchno)->state != 0 ){  
						if( selem->m_shadowinflag != SHADOWIN_EXCLUDE ){

							SetRenderState( pd3dDevice, selem );

							ret = selem->RenderShadowMap0( pd3dDevice, this );
							if( ret ){
								DbgOut( "CShdHandler : Render error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
					}
				}
			}
		}

		return 0;
	}

	int CShdHandler::RenderShadowMap1( LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, int polydispmode, CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, D3DXVECTOR3 vEyePt, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2 )
	{

		int i, ret;
		int dispflag;
		CShdElem* selem;
		int isfirstrender = 1;
		CMeshInfo* mi = 0;
		int polydisp;
		
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if( (m_inRDBflag == 0) && (lpmh->m_curbs.visibleflag == 0) ){
			//_ASSERT( 0 );
			return 0;
		}


		// extline用
		if( m_TLmode == TLMODE_D3D ){
			//pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );
			D3DXMATRIX fscale, newwmat;
			GetFirstScale( &fscale );
			newwmat = fscale * *matWorld;
			//pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );
			pd3dDevice->SetTransform( D3DTS_WORLD,  &newwmat );
			pd3dDevice->SetTransform( D3DTS_VIEW, matView );
			pd3dDevice->SetTransform( D3DTS_PROJECTION, matProj );							
		}


//		if( g_rendercnt == 0 ){
			//スプライト描画でステートが不定になるのでここでの呼び出しが必要
//			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//			g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
//			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//			g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
//			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
//			g_renderstate[ D3DRS_ALPHATESTENABLE ] = 0;
//		}
//		g_rendercnt++;

		int vflag;

		if( withalpha == 0 ){
		// 不透明objectのrender
			//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			//g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;

			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
			g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
			g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

////////////////////////////
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
			g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
			g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
			g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

		}else{
		//半透明object、transparent == 1 のrender
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;

//			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
//			g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;

			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
			g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
			g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

////////////////////////////
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
			g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
			g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
			g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

		}

		SetCurDS();

		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			dispflag = selem->dispflag;

			if( polydispmode == POLYDISP_ALL )
				polydisp = 1;
			else
				polydisp = selem->isselected;

			if( (m_inRDBflag == 0) && (selem->type != SHDBILLBOARD) && (selem->type != SHDEXTLINE) && (selem->type != SHDINFSCOPE) )
				vflag = selem->curbs.visibleflag;
			else
				vflag = 1;
			if( (m_inRDBflag == 0) && (selem->type == SHDMORPH) ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}


			if( vflag && dispflag && polydisp 
				&& !( selem->IsJoint() && (selem->type != SHDMORPH) ) 
				&& (selem->notuse != 1) 
				&& (selem->invisibleflag == 0) && (selem->m_mtype == M_NONE) ){										

				if( selem->type == SHDBILLBOARD ){
						
					// Set diffuse blending for alpha set in vertices.
					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
					g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;


					ret = selem->RenderBillboardShadowMap1( this, pd3dDevice, matWorld, matView, matProj, matWorld1, matWorld2, vEyePt );
					if( ret ){
						DbgOut( "shdhandler : RenderShadowMap1 : RenderBillboardShadowMap1 error !!1\n" );
						_ASSERT( 0 );
						return 1;
					}

				}else{
					DWORD dispswitchno = selem->dispswitchno;
					if( (m_curds + dispswitchno)->state != 0 ){  

						SetRenderState( pd3dDevice, selem );

						
						ret = selem->RenderShadowMap1( withalpha, pd3dDevice, this );
						if( ret ){
							DbgOut( "CShdHandler : RenderShadowMap1 : RenderShadowMap1 error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
			}
		}


		return 0;
	}

	int CShdHandler::TransformBillboardDispData( D3DXMATRIX matWorld, D3DXMATRIX matWorld1, D3DXMATRIX matWorld2, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3 camerapos )
	{

		int ret;

		CShdElem* bbselem;
		bbselem = (*this)( 1 );
		if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
			DbgOut( "sh : TransformBillboardDispData : type error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = bbselem->TransformBillboard( this, matWorld, matWorld1, matWorld2, matView, matProj, camerapos );
		if( ret ){
			DbgOut( "shdhandler : TransformBillboardDispData : se TransformBillboard error !!1\n" );
			_ASSERT( 0 );
			return 1;
		}

		return 0;
	}


	int CShdHandler::Render( int znflag, int inbatch, LPDIRECT3DDEVICE9 pd3dDevice, int withalpha, int polydispmode, CMotHandler* lpmh, int srcmotcookie, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3 vEyePt, D3DXMATRIX* matWorld1, D3DXMATRIX* matWorld2, int glowflag )
	{

		int i, ret;
		int dispflag;
		CShdElem* selem;
		int isfirstrender = 1;
		CMeshInfo* mi = 0;
		int polydisp;
		
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if( (m_inRDBflag == 0) && (lpmh->m_curbs.visibleflag == 0) ){
			//_ASSERT( 0 );
			return 0;
		}



		int vflag;

		if( inbatch == 0 ){

			if( g_rendercnt == 0 ){
				//スプライト描画でステートが不定になるのでここでの呼び出しが必要
				pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
				g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
				//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
				//g_renderstate[ D3DRS_ALPHATESTENABLE ] = 0;
				pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;
				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;


				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				g_cop0 = D3DTOP_MODULATE;
				pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
				g_cop1 = D3DTOP_DISABLE;
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
				g_aop0 = D3DTOP_MODULATE;
				pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
				g_aop1 = D3DTOP_DISABLE;

			}
			g_rendercnt++;


			if( znflag >= 100 ){
				if( znflag != 104 ){
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
				}else{
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
					g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;
				}
				pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
				g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

				pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


				pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
				g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
				g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
				g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

			}else if( withalpha == 0 ){
			// 不透明objectのrender
				//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
				//g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;

				if( glowflag == 0 ){
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
					g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

					pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
					g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

					pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
					g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
					g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;
				}else{
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
					g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

					pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
					g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

					pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
					g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
					g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;
				}
	////////////////////////////
				pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
				g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
				g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
				g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

			}else{
			//半透明object、transparent == 1 のrender
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;

				if( glowflag == 0 ){
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;
				}else{
					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
					g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

					pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
					g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

					pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
					g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
					g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;
				}
	////////////////////////////
				pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
				g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
				g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
				g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

			}

		}

		// extline用
		if( m_TLmode == TLMODE_D3D ){
			//pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );
			D3DXMATRIX fscale, newwmat;
			GetFirstScale( &fscale );
			newwmat = fscale * matWorld;
			//pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );
			pd3dDevice->SetTransform( D3DTS_WORLD,  &newwmat );
			pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
			pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );							
		}


		SetCurDS();


		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			dispflag = selem->dispflag;

			if( polydispmode == POLYDISP_ALL )
				polydisp = 1;
			else
				polydisp = selem->isselected;

			if( (m_inRDBflag == 0) && (selem->type != SHDBILLBOARD) && (selem->type != SHDEXTLINE) && (selem->type != SHDINFSCOPE) )
				vflag = selem->curbs.visibleflag;
			else
				vflag = 1;
			if( selem->type == SHDMORPH ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}

			if( vflag && dispflag && polydisp 
				&& !( selem->IsJoint() && (selem->type != SHDMORPH) ) 
				&& (selem->notuse != 1) 
				&& (selem->invisibleflag == 0) ){										

				if( selem->type == SHDBILLBOARD ){
						
					if( withalpha != 0 ){
						// Set diffuse blending for alpha set in vertices.
						pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
						g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;

						pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
						g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

						pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
						g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;

						pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
						g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;

						pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
						g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

						pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
						g_renderstate[ D3DRS_ALPHAREF ] = 0;

						pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
						g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;


						//pd3dDevice->SetTexture( 0, curtex );
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
						//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
						if( g_aop0 != D3DTOP_MODULATE ){
							pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
							g_aop0 = D3DTOP_MODULATE;
						}
						if( g_aop1 != D3DTOP_DISABLE ){
							pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
							g_aop1 = D3DTOP_DISABLE;
						}

						if( znflag == 0 ){
							ret = selem->RenderBillboard( m_bbtransskip, this, pd3dDevice, g_texbnk, m_TLmode, matWorld, matView, matProj, matWorld1, matWorld2, vEyePt, glowflag );
							if( ret ){
								DbgOut( "shdhandler : Render : RenderBillboard error !!1\n" );
								_ASSERT( 0 );
								return 1;
							}
						}


						//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE );
						//g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;

						//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
						//g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

						if( g_cop0 != D3DTOP_MODULATE ){
							pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
							g_cop0 = D3DTOP_MODULATE;
						}
						if( g_aop0 != D3DTOP_MODULATE ){
							pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
							g_aop0 = D3DTOP_MODULATE;
						}

					}
				}else if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) || 
					(selem->type == SHDEXTLINE) || (selem->type == SHDMORPH) ){
					DWORD dispswitchno = selem->dispswitchno;
					if( (m_curds + dispswitchno)->state != 0 ){  

						SetRenderState( pd3dDevice, selem );

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
						//if( g_aop0 != D3DTOP_SELECTARG1 ){
						//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
						//	g_aop0 = D3DTOP_SELECTARG1;
						//}
						if( g_aop0 != D3DTOP_MODULATE ){
							pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
							g_aop0 = D3DTOP_MODULATE;
						}
						if( g_aop1 != D3DTOP_DISABLE ){
							pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
							g_aop1 = D3DTOP_DISABLE;
						}

						ret = selem->Render( znflag, withalpha, pd3dDevice, this, glowflag );
						if( ret ){
							DbgOut( "CShdHandler : Render error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}else if( ((m_inRDBflag != 0) && (selem->type != SHDDESTROYED)) || (selem->type == SHDINFSCOPE) ){
					DWORD dispswitchno = selem->dispswitchno;
					if( (m_curds + dispswitchno)->state != 0 ){  

						SetRenderState( pd3dDevice, selem );


						//pd3dDevice->SetTexture( 0, curtex );
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
//						if( g_aop0 != D3DTOP_SELECTARG1 ){
//							pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//							g_aop0 = D3DTOP_SELECTARG1;
//						}
						if( g_aop0 != D3DTOP_MODULATE ){
							pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
							g_aop0 = D3DTOP_MODULATE;
						}
						if( g_aop1 != D3DTOP_DISABLE ){
							pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
							g_aop1 = D3DTOP_DISABLE;
						}

						ret = selem->Render( znflag, withalpha, pd3dDevice, this, NOGLOW );
						if( ret ){
							DbgOut( "CShdHandler : Render error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}else{

//if( g_curtex0 != NULL ){
//	pd3dDevice->SetTexture( 0, NULL );
//	g_curtex0 = NULL;
//}
//	pd3dDevice->SetTexture( 1, NULL );
//	g_curtex1 = NULL;
				}
			}
		}

		if( m_inRDBflag != 0 ){
			if( withalpha != 0 ){

				CShdElem* bbxelem;
				bbxelem = GetBBoxElem();
				if( bbxelem ){

					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;

					pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;

					pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
					g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;

					pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
					g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;

					if( g_curtex0 != NULL ){
						pd3dDevice->SetTexture( 0, NULL );
						g_curtex0 = NULL;
					}
					SetRenderState( pd3dDevice, bbxelem );

					if( znflag == 0 ){
						ret = bbxelem->Render( 0, 1, pd3dDevice, this, NOGLOW );
						if( ret ){
							DbgOut( "CShdHandler : Render bbx error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
			}
		}

		return 0;
	}


	int CShdHandler::SetCurTexname( int srcseri )
	{


		//int ret;
		CShdElem* curselem = (*this)( srcseri );

	//enabletextureが０の場合
		if( curselem->m_enabletexture == 0 ){
			curselem->curtexname = NULL;
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		curselem->curtexname = curselem->texname;

		return 0;
	}

	int CShdHandler::RenderSelVert( CMotHandler* lpmh, LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pd3dxsprite, 
		int selpart, int* pselvert, int selvertnum, int brushtype,
		int markpart, int markvert,
		DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj  )
	{
		if( brushtype == 0 ){
			if( (selpart <= 0) || (selvertnum <= 0) ){
				return 0;
			}
		}else{
			if( ( (selpart <= 0) || (selvertnum <= 0) ) && 
				( (markpart <= 0) || (markvert < 0) ) ){
				return 0;
			}			
		}

		HRESULT hr;
		hr = pd3dxsprite->Begin( D3DXSPRITE_ALPHABLEND );
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		int savelighting;
		savelighting = g_renderstate[ D3DRS_LIGHTING ];

 		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		LPDIRECT3DTEXTURE9 ptexture1;
		LPDIRECT3DTEXTURE9 ptexture2;
		ptexture1 = g_texbnk->GetTexData( s_selvertbmp, 1 );
		ptexture2 = g_texbnk->GetTexData( s_selvbmp, 0 );

		D3DXVECTOR2	scl1, scl2;
		int sizex1, sizey1, sizex2, sizey2;
		if( ptexture1 != NULL ){
			HRESULT hr;
			D3DSURFACE_DESC	sdesc;
			hr = ptexture1->GetLevelDesc( 0, &sdesc );
			if( hr != D3D_OK ){
				_ASSERT( 0 );
				return 1;
			}
			
			sizex1 = sdesc.Width / 2;
			sizey1 = sdesc.Height / 2;

			scl1.x = 16.0f / sizex1;
			scl1.y = 16.0f / sizey1;
		}else{
			_ASSERT( 0 );
			return 1;
		}
		if( ptexture2 != NULL ){
			HRESULT hr;
			D3DSURFACE_DESC	sdesc;
			hr = ptexture2->GetLevelDesc( 0, &sdesc );
			if( hr != D3D_OK ){
				_ASSERT( 0 );
				return 1;
			}
			
			sizex2 = sdesc.Width / 2;
			sizey2 = sdesc.Height / 2;

			scl2.x = 4.0f / ( sizex2 * 2 );
			scl2.y = 4.0f / ( sizey2 * 2 );
		}else{
			_ASSERT( 0 );
			return 1;
		}

///////////
		if( (brushtype == 1) && (markpart > 0) && (markvert >= 0) ){

			float scx, scy, scz;
			int ret;
			ret = GetScreenPos4( 0, lpmh, markpart, markvert, 0,
				&scx, &scy, &scz,
				dwClipWidth, dwClipHeight, matWorld, matView, matProj );
			if( ret ){
				DbgOut( "sh : RenderSelVert : GetScreenPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			D3DXVECTOR3	tra;
			D3DCOLOR col;

			col = D3DCOLOR_ARGB( 128, 255, 255, 255 );
			tra.x = scx - sizex1 * scl1.x;
			tra.y = scy - sizey1 * scl1.y;
			tra.z = 0.0;


			D3DXMATRIX transmat;
			D3DXMATRIX scalemat;
			D3DXMatrixIdentity( &scalemat );

			D3DXMATRIX posmat;
			D3DXMatrixIdentity( &posmat );
			posmat._41 = tra.x;
			posmat._42 = tra.y;

			scalemat._11 = scl1.x;
			scalemat._22 = scl1.y;

			transmat = scalemat * posmat;
			pd3dxsprite->SetTransform( &transmat );

			hr = pd3dxsprite->Draw( ptexture1, NULL, NULL, NULL, col );
			_ASSERT( hr == D3D_OK );
		}


////////////
		int drawvnum;
		if( brushtype == 0 ){
			drawvnum = 1;
		}else{
			drawvnum = selvertnum;
		}

		int vno;
		for( vno = 0; vno < drawvnum; vno++ ){

			int selvert;
			selvert = *( pselvert + vno );

			float scx, scy, scz;
			int ret;
			//ret = GetScreenPos( 0, lpmh, selpart, selvert, 0, &scx, &scy,
			//	dwClipWidth, dwClipHeight, matWorld, matView, matProj );
			ret = GetScreenPos4( 0, lpmh, selpart, selvert, 0,
				&scx, &scy, &scz,
				dwClipWidth, dwClipHeight, matWorld, matView, matProj );
			if( ret ){
				DbgOut( "sh : RenderSelVert : GetScreenPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			D3DXVECTOR3	tra;
			D3DCOLOR col;

			if( brushtype == 0 ){
				col = D3DCOLOR_ARGB( 128, 255, 255, 255 );
				tra.x = scx - sizex1 * scl1.x;
				tra.y = scy - sizey1 * scl1.y;
				tra.z = 0.0;
			}else{
				col = D3DCOLOR_ARGB( 255, 255, 255, 255 );
				tra.x = scx - sizex2 * scl2.x;
				tra.y = scy - sizey2 * scl2.y;
				tra.z = 0.0;
			}


			D3DXMATRIX transmat;
			D3DXMATRIX scalemat;
			D3DXMatrixIdentity( &scalemat );

			D3DXMATRIX posmat;
			D3DXMatrixIdentity( &posmat );
			posmat._41 = tra.x;
			posmat._42 = tra.y;

			if( brushtype == 0 ){
				scalemat._11 = scl1.x;
				scalemat._22 = scl1.y;
			}else{
				scalemat._11 = scl2.x;
				scalemat._22 = scl2.y;
			}

			transmat = scalemat * posmat;
			pd3dxsprite->SetTransform( &transmat );

			if( brushtype == 0 ){
				hr = pd3dxsprite->Draw( ptexture1, NULL, NULL, NULL, col );
			}else{
				hr = pd3dxsprite->Draw( ptexture2, NULL, NULL, NULL, col );
				//_ASSERT( 0 );
			}
			_ASSERT( hr == D3D_OK );
		}



		hr = pd3dxsprite->End();
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, savelighting );

		return 0;		
	}

/***
	int CShdHandler::RenderSelVert( CMotHandler* lpmh, LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pd3dxsprite, 
		int selpart, int* pselvert, int selvertnum, int brushtype,
		DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj  )
	{
		if( (selpart <= 0) || (selvertnum <= 0) ){
			_ASSERT( 0 );
			return 0;
		}


		HRESULT hr;
		hr = pd3dxsprite->Begin( D3DXSPRITE_ALPHABLEND );
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		int savelighting;
		savelighting = g_renderstate[ D3DRS_LIGHTING ];

 		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		LPDIRECT3DTEXTURE9 ptexture;
		if( brushtype == 0 ){
			ptexture = g_texbnk->GetTexData( s_selvertbmp, 1 );
		}else{
			ptexture = g_texbnk->GetTexData( s_selvbmp, 0 );
		}

		D3DXVECTOR2	scl;
		int sizex, sizey;
		if( ptexture != NULL ){

			HRESULT hr;
			D3DSURFACE_DESC	sdesc;
			hr = ptexture->GetLevelDesc( 0, &sdesc );
			if( hr != D3D_OK ){
				_ASSERT( 0 );
				return 1;
			}
			
			sizex = sdesc.Width / 2;
			sizey = sdesc.Height / 2;

			if( brushtype == 0 ){
				scl.x = 16.0f / sizex;
				scl.y = 16.0f / sizey;
			}else{
				scl.x = 4.0f / ( sizex * 2 );
				scl.y = 4.0f / ( sizey * 2 );
			}

		}else{
			_ASSERT( 0 );
			return 1;
		}

		int drawvnum;
		int cola;
		if( brushtype == 0 ){
			drawvnum = 1;
			cola = 128;
		}else{
			drawvnum = selvertnum;
			cola = 255;
		}

		int vno;
		for( vno = 0; vno < drawvnum; vno++ ){

			int selvert;
			selvert = *( pselvert + vno );

			float scx, scy, scz;
			int ret;
			//ret = GetScreenPos( 0, lpmh, selpart, selvert, 0, &scx, &scy,
			//	dwClipWidth, dwClipHeight, matWorld, matView, matProj );
			ret = GetScreenPos4( 0, lpmh, selpart, selvert, 0,
				&scx, &scy, &scz,
				dwClipWidth, dwClipHeight, matWorld, matView, matProj );
			if( ret ){
				DbgOut( "sh : RenderSelVert : GetScreenPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			D3DXVECTOR3	tra;
			D3DCOLOR col;

			col = D3DCOLOR_ARGB( cola, 255, 255, 255 );


			tra.x = scx - sizex * scl.x;
			tra.y = scy - sizey * scl.y;
			tra.z = 0.0;

			D3DXMATRIX transmat;
			D3DXMATRIX scalemat;
			D3DXMatrixIdentity( &scalemat );

			D3DXMATRIX posmat;
			D3DXMatrixIdentity( &posmat );
			posmat._41 = tra.x;
			posmat._42 = tra.y;

			scalemat._11 = scl.x;
			scalemat._22 = scl.y;

			transmat = scalemat * posmat;
			pd3dxsprite->SetTransform( &transmat );

			hr = pd3dxsprite->Draw( ptexture, NULL, NULL, NULL, col );

			_ASSERT( hr == D3D_OK );
		}

		hr = pd3dxsprite->End();
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, savelighting );

		return 0;
	}
***/

	int CShdHandler::RenderBone( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pd3dxsprite, float jointsize, int jointalpha,
		DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh, 
		D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj )
	{
		HRESULT hr;		
		hr = pd3dxsprite->Begin( D3DXSPRITE_ALPHABLEND );
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		//pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		//pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
 		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );

/***
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
		g_renderstate[ D3DRS_ZWRITEENABLE ] = FALSE;

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
		g_renderstate[ D3DRS_LIGHTING ] = FALSE;

		pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );//!!!!!!!!!!!!!!!
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_ALWAYS;
***/


		LPDIRECT3DTEXTURE9 ptexture;
		LPDIRECT3DTEXTURE9 ptexture1, ptexture2;
		ptexture1 = g_texbnk->GetTexData( s_bonemarkdds, 0 );
		ptexture2 = g_texbnk->GetTexData( s_bonemarkdds2, 0 );

		int i, ret;
		CShdElem* selem;
		int etype, dispflag;


		D3DXMATRIX firstscale;
		GetFirstScale( &firstscale );

		float calaspect;
		calaspect = (float)dwClipWidth / (float)dwClipHeight;

		for( i = 1; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;
			dispflag = selem->dispflag;
			
			if( dispflag && (selem->IsJoint() && (etype != SHDMORPH)) 
				&& (selem->notuse != 1) ){

				if( selem->m_notransik == 0 ){
					ptexture = ptexture1;
				}else{
					ptexture = ptexture2;
				}

				ret = selem->d3ddisp->TransformDispDataSCV( m_seri2boneno, i, &firstscale, dwClipWidth, dwClipHeight, srclpmh,
					matWorld, matView, matProj, calaspect );
				if( ret ){
					DbgOut( "sh : RenderBone : d3ddisp TransformDispDataSCV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = selem->RenderBone( pd3dxsprite, ptexture, jointsize, jointalpha );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}

		hr = pd3dxsprite->End();
		if( hr != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		return 0;
	}

	int CShdHandler::RenderBoneMark( LPDIRECT3DDEVICE9 pd3dDevice, CShdHandler* lpsh, CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, COLORREF srcselcol, COLORREF srcunselcol, int srcselectno )
	{

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// lpsh, lpmh は、モデルデータのハンドラーなので、注意！！！
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


		pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );
		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );							

		if( g_curtex0 != NULL ){
			pd3dDevice->SetTexture( 0, NULL );
			g_curtex0 = NULL;
		}


		int i, ret;
		CShdElem* selem;
		int etype;//, dispflag;

		CShdElem* curelem;
		CShdElem* bonemarkelem = 0;
		for( i = 0; i < s2shd_leng; i++ ){
			curelem = (*this)( i );
			if( curelem->type == SHDEXTLINE ){
				bonemarkelem = curelem;
				break;
			}
		}
		if( !bonemarkelem ){
			DbgOut( "shandler : RenderBoneMark : EXTLINE object not found in %s error !!!\n", s_bonemarkmqo );
			_ASSERT( 0 );
			return 1;
		}



		for( i = 1; i < lpsh->s2shd_leng; i++ ){
			selem = (*lpsh)( i );
			etype = selem->type;
			//dispflag = selem->dispflag;
			
			if( (selem->IsJoint() && (etype != SHDMORPH)) && (selem->notuse != 1) ){
				CPart* partptr;
				partptr = selem->part;
				if( !partptr ){
					DbgOut( "shandler : RenderBoneMark : partptr NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( partptr->bonenum > 0 ){
					int bno;
					for( bno = 0; bno < partptr->bonenum; bno++ ){

						CShdElem* belem = 0;
						belem = partptr->GetChildSelem( lpsh, bno );
						if( belem && belem->m_bonelinedisp && (belem->m_notsel == 0) ){
							D3DXMATRIX bmmat;
							D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
							float baseleng = 50.0f;
							ret = partptr->GetBoneMarkMatrix( bno, lpsh, lpmh, lpmh->m_curmotkind, lpmh->m_curframeno, matWorld, baseleng, basevec, &bmmat );
							if( ret ){
								DbgOut( "shandler : RenderBoneMark : part GetBoneMarkMatrix error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}

							pd3dDevice->SetTransform( D3DTS_WORLD,  &bmmat );

							D3DXVECTOR3 dummyeye( 0.0f, 0.0f, 0.0f );
							bonemarkelem->d3ddisp->TransformSkinMat( bonemarkelem->serialno, 0, 0, lpmh, 
								&matWorld, &matView, &matProj, &dummyeye );

							/***
							int isselected = 0;
							int bano;
							for( bano = 0; bano < MAXBONENUM; bano++ ){
								if( *(barray + bano) == 0 )
									break;

								if( *(barray + bano) == belem->serialno ){
									isselected = 1;
									break;
								}
							}
							***/

							if( srcselectno == belem->serialno ){
								SetExtLineColor( -1, 255, GetRValue( srcselcol ), GetGValue( srcselcol ), GetBValue( srcselcol ) );
							}else{
								SetExtLineColor( -1, 255, GetRValue( srcunselcol ), GetGValue( srcunselcol ), GetBValue( srcunselcol ) );
							}

							ret = bonemarkelem->Render( 0, 0, pd3dDevice, this, NOGLOW );
							if( ret ){
								DbgOut( "CShdHandler : RenderBoneMark : bmelem Render error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
					}
				}
			}
		}


		return 0;
	}


#endif

int CShdHandler::CalcDepth()
{
	CShdElem* startelem;

	startelem = (*this)( 1 );
	
	startelem->CalcDepthReq( 1 );

	return 0;
}

int CShdHandler::InitBoundaryBox()
{
	CD3DDisp::s_minx = 1e6;
	CD3DDisp::s_maxx = -1e6;
	CD3DDisp::s_miny = 1e6;
	CD3DDisp::s_maxy = -1e6;
	CD3DDisp::s_minz = 1e6;
	CD3DDisp::s_maxz = -1e6;

	/***
	m_bbox.minx = 1e6;
	m_bbox.maxx = -1e6;
	m_bbox.miny = 1e6;
	m_bbox.maxy = -1e6;
	m_bbox.minz = 1e6;
	m_bbox.maxz = -1e6;

	m_bbox.setflag = 0;
	***/

	m_bbox.InitParams();

	return 0;
}

/***
int CShdHandler::SetBoundaryBox()
{
	m_bbox.setflag = 1;

	m_bbox.minx = CD3DDisp::s_minx;
	m_bbox.maxx = CD3DDisp::s_maxx;
	m_bbox.miny = CD3DDisp::s_miny;
	m_bbox.maxy = CD3DDisp::s_maxy;
	m_bbox.minz = CD3DDisp::s_minz;
	m_bbox.maxz = CD3DDisp::s_maxz;

	return 0;
}
***/


int CShdHandler::SetTBSBBox()
{
	CD3DDisp::s_tbsminx = 1e6;
	CD3DDisp::s_tbsmaxx = -1e6;
	CD3DDisp::s_tbsminy = 1e6;
	CD3DDisp::s_tbsmaxy = -1e6;
	CD3DDisp::s_tbsminz = 1e6;
	CD3DDisp::s_tbsmaxz = -1e6;

	return 0;
}

int CShdHandler::ChangeJointLoc( int serino, CVec3f* newloc )
{
	CShdElem* selem;
	selem = (*this)( serino );
	if( !selem ){
		_ASSERT( 0 );
		return 1;
	}
	CPart* partptr;
	partptr = selem->part;
	if( !partptr ){
		_ASSERT( 0 );
		return 1;
	}
	
	CD3DDisp* dispptr;
	dispptr = selem->d3ddisp;
	if( !dispptr ){
		_ASSERT( 0 );
		return 1;
	}
	SKINVERTEX* dstskinv;
	dstskinv = dispptr->m_skinv;
	_ASSERT( dstskinv );

	partptr->jointloc = *newloc;

	dstskinv->pos[0] = newloc->x;
	dstskinv->pos[1] = newloc->y;
	dstskinv->pos[2] = newloc->z;

	return 0;
}

/***
////////////////
int CShdHandler::CheckMinZObj( int* dstno, D3DXVECTOR3* dstvec )
{
	CShdElem* selem;
	int dispflag;
	int i;

	float minz = 1e6;

	for( i = 1; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
		if( dispflag && (selem->notuse != 1) && !(selem->IsJoint()) && (selem->type != SHDBILLBOARD) ){
			CD3DDisp* d3ddisp = selem->d3ddisp;
			if( !d3ddisp ){
				_ASSERT( 0 );
				return 1;
			}
			int findflag;
			findflag = d3ddisp->FindMinZVert( &minz, dstvec );
			if( findflag ){
				*dstno = i;
			}
		}
	}

	return 0;
}
***/

int CShdHandler::CheckRestVert()
{
	CShdElem* selem;
	int dispflag;
	int i;

	for( i = 1; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
		if( dispflag && (selem->notuse != 1) && !(selem->IsJoint()) && (selem->type != SHDBILLBOARD) ){
			CD3DDisp* d3ddisp = selem->d3ddisp;
			if( !d3ddisp ){
				_ASSERT( 0 );
				return 1;
			}
			DbgOut( "shandler : CheckRestVert : serial %d\n", i );
			d3ddisp->CheckRestVert();
		}
	}

	return 0;
}

int CShdHandler::DebugPolymeshM()
{
	CShdElem* selem;
	int i;

	for( i = 1; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		if( selem->type == SHDPOLYMESH ){
			selem->DebugPolymeshM();
		}
	}

	return 0;
}

int CShdHandler::PickBone( CMotHandler* srclpmh, int srcx, int srcy, DWORD dwWidth, DWORD dwHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float srcaspect )
{
	float flx, fly;
	flx = (float)srcx;
	fly = (float)srcy;

	float pickrange = 100.0;
	float mag = 1e6;
	int pickno = 0;


	int i, ret;
	CShdElem* selem;
	int etype;
	for( i = 1; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;
		if( (selem->IsJoint() && (etype != SHDMORPH)) && (selem->notuse != 1) ){
			CD3DDisp* d3ddisp = selem->d3ddisp;
			if( !d3ddisp ){
				_ASSERT( 0 );
				return -1;
			}

			float tempmag;

			int motid, frameno, scaleflag;
			motid = srclpmh->m_curmotkind;
			frameno = srclpmh->m_curframeno;
			scaleflag = 1;

			D3DXVECTOR3 dstpos;
			ret = d3ddisp->TransformOnlyWorld1Vert( srclpmh, matWorld, motid, frameno, 0, &dstpos, scaleflag, 0 );
			if( ret ){
				DbgOut( "shdhandler : PickBone : d3ddisp TransformOnlyWorld1Vert error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			CBSphere tempbs;
			tempbs.tracenter = dstpos;
			int scx, scy;
			ret = tempbs.Transform2ScreenPos( dwWidth, dwHeight, matView, matProj, srcaspect, &scx, &scy );
			if( ret ){
				DbgOut( "sh : PickBone : bs Transform2ScreenPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			tempmag = ( (float)scx - flx ) * ( (float)scx - flx ) + ( (float)scy - fly ) * ( (float)scy - fly );


//DbgOut( "shd PickBone : elem %d : %d %d, %f\r\n", i, srcx, srcy, tempmag );

			if( (tempmag < mag) && (tempmag < pickrange) ){
				mag = tempmag;
				pickno = i;
			}
			
		}
	}

	return pickno;
}

int CShdHandler::GetBoneVec( D3DXVECTOR3* dstvec, int srcseri, int srcparseri )
{
	CShdElem* curelem;
	CShdElem* parelem;

	curelem = (*this)( srcseri );
	parelem = (*this)( srcparseri );
	_ASSERT( curelem );
	_ASSERT( parelem );

	D3DXVECTOR3 curvec, parvec, diffvec;
	int ret;
	ret = curelem->GetBoneOrgPos( &curvec.x, &curvec.y, &curvec.z );
	_ASSERT( !ret );
	ret = parelem->GetBoneOrgPos( &parvec.x, &parvec.y, &parvec.z );
	_ASSERT( !ret );

	diffvec = curvec - parvec;
	D3DXVec3Normalize( dstvec, &diffvec );

	return 0;

	/***
	int i;
	CShdElem* curelem;
	CBoneInfo* findbi = 0;
	int isfind = 0;
	for( i = 0; i < s2shd_leng; i++ ){
		if( isfind == 1 )
			break;
		curelem = (*this)( i );
		if( curelem->IsJoint() && (curelem->type != SHDMORPH) ){
			CPart* partptr;
			partptr = curelem->part;
			int bino;
			for( bino = 0; bino < partptr->bonenum; bino++ ){
				CBoneInfo* curbi;
				curbi = *(partptr->ppBI + bino);
				if( (curbi->jointno == srcseri) && (curbi->parentjoint == srcparseri) ){
					findbi = curbi;
					isfind = 1;
					break;
				}
			}
		}
	}

	if( findbi ){
		dstvec->x = findbi->bonevec.x;
		dstvec->y = findbi->bonevec.y;
		dstvec->z = findbi->bonevec.z;
		return 0;

	}else{
		_ASSERT( 0 );
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
		return 1;
	}
	***/

}

int CShdHandler::CreateBSphereData( int motkindnum, int framenum )
{
	int i, ret;
	CShdElem* curelem;
	for( i = 0; i < s2shd_leng; i++ ){
		curelem = (*this)( i );
		if( (curelem->type == SHDPOLYMESH) || (curelem->type == SHDPOLYMESH2) ){
			ret = curelem->CreateBSphereData( motkindnum, framenum );
			if( ret ){
				DbgOut( "shdhandler : CreateBSphereData : elem->CreateBSphereData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdHandler::RemakeBSphereData( CMotHandler* lpmh, int motid, int framenum )
{
	int i, ret;
	CShdElem* curelem;
	for( i = 0; i < s2shd_leng; i++ ){
		curelem = (*this)( i );
		if( (curelem->type == SHDPOLYMESH) || (curelem->type == SHDPOLYMESH2) ){
			ret = curelem->RemakeBSphereData( motid, framenum );
			if( ret ){
				DbgOut( "shdhandler : RemakeBSphereData : selem RemakeBSphereData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	ret = lpmh->RemakeBSphereData( motid, framenum );
	if( ret ){
		DbgOut( "shdhandler : RemakeBSphereData : mh RemakeBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdHandler::SetBSphereData( CMotHandler* mhptr, int motcookie, int startframe, int endframe )
{

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// startframe != 0のときは、SetNewPoseの前にSetMotionFrameNoが必要！！！！！
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	int i, ret;
	
	CMotionCtrl* firstjoint = 0;

	if( motcookie >= 0 ){
		int frameleng;
		ret = mhptr->GetMotionFrameLength( motcookie, &frameleng );
		if( ret ){
			DbgOut( "shdhandler : SetBSphereData : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( frameleng <= 0 ){
			DbgOut( "shdhandler : SetBSphereData : frameleng 0 warning !!!\n" );
			_ASSERT( 0 );
			return 0;//!!!!!!!
		}
		if( endframe < 0 ){
			endframe = frameleng - 1;
		}
		if( endframe >= frameleng ){
			endframe = frameleng - 1;
		}
		if( startframe < 0 ){
			startframe = 0;
		}
		if( startframe > endframe ){
			startframe = endframe;
		}
		////

		GetFirstJoint( &firstjoint, mhptr, 1 );


		if( !firstjoint ){
			DbgOut( "shdhandler : SetBSphereData : firstjoint NULL exit !!!\n" );
			_ASSERT( 0 );
			return 0;
		}

		ret = mhptr->SetMotionKind( motcookie );
		if( ret ){
			DbgOut( "ShdHandler : SetBSphereData : mhandler->SetMotionKind error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		CMotionInfo* minfo;
		minfo = firstjoint->motinfo;
		if( !minfo ){
			DbgOut( "shdhandler : SetBSphereData : minfo NULL exit !!!\n" );
			_ASSERT( 0 );
			return 0;
		}
		int saveframestep;
		saveframestep = *(minfo->motstep + motcookie); 
		ret = mhptr->SetMotionStep( motcookie, 1 );
		if( ret ){
			DbgOut( "shdhandler : SetBSphereData : mhandler->SetMotionStep error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		int frameno;
		for( frameno = startframe; frameno <= endframe; frameno++ ){

			mhptr->SetNewPose( this );

			DWORD curdispswitch;
			if( m_curdispswitch == 0 ){
				if( firstjoint ){
					curdispswitch = firstjoint->curdispswitch;
				}else{
					curdispswitch = 0xFFFFFFFF;
				}
			}else{
				curdispswitch = m_curdispswitch;
			}

			CBSphere* dstbs;
			dstbs = *(mhptr->m_firstbs + motcookie) + frameno;


			ret = SetBSphereDataOnFrame( mhptr, motcookie, frameno, curdispswitch, dstbs );
			if( ret ){
				DbgOut( "shdhandler : SetBSphereData : SetBSphereDataOnFrame error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


		}

		ret = mhptr->SetMotionStep( motcookie, saveframestep );
		if( ret ){
			DbgOut( "shdhandler : SetBSphereData : mhandler->SetMotionStep error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{

		CBSphere* dstbs;
		dstbs = &(mhptr->m_curbs);

		ret = SetBSphereDataOnFrame( mhptr, -1, 0, 0xFFFFFFFF, dstbs );
		if( ret ){
			DbgOut( "shdhandler : SetBSphereData : motcookie < 0 : SetBSphereDataOnFrame error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

// モデル、パーツの中心座標のセット
		
		m_center = dstbs->befcenter;
		CShdElem* selem;
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				selem->m_center = selem->curbs.befcenter;			
			}
		}
	}

	return 0;
}


int CShdHandler::SetBSphereDataOnFrame( CMotHandler* mhptr, int motcookie, int frameno, DWORD curdispswitch, CBSphere* dsttotalbs )
{
	int i, ret;
	int dispflag;
	CShdElem* selem;
	int calccnt;

	D3DXMATRIX iniworld;
	D3DXMatrixIdentity( &iniworld );

	m_mc.InitCache( &iniworld );


	SetTBSBBox();


	// !!!!!!!!!! 2003/12/8 ver2023+
	//プログラム側で、任意のタイミングで、displayswitchを操作できるようにしたので、
	//bs の初期化は、スイッチオフのものについても、しておく必要がある。
	// notuse についても、同様に。

	//パーツの中心と半径を求める
	calccnt = 0;
	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
				
		if( (dispflag || (selem->m_mtype == M_BASE)) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){					
			ret = selem->TransformOnlyWorld( &m_mc, mhptr, motcookie, frameno, iniworld, CALC_CENTER_FLAG, dsttotalbs );
			if( ret ){
				DbgOut( "shandler : SetBSphereData : TransformOnlyWorld : CALC_CENTER error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			calccnt++;
		}
	}

	//オブジェクトの中心と半径を求める
	if( calccnt > 0 ){
		dsttotalbs->befcenter.x = ( CD3DDisp::s_tbsminx + CD3DDisp::s_tbsmaxx ) * 0.5f; 
		dsttotalbs->befcenter.y = ( CD3DDisp::s_tbsminy + CD3DDisp::s_tbsmaxy ) * 0.5f; 
		dsttotalbs->befcenter.z = ( CD3DDisp::s_tbsminz + CD3DDisp::s_tbsmaxz ) * 0.5f;
				
		float diffx, diffy, diffz;
		diffx = ( CD3DDisp::s_tbsmaxx - CD3DDisp::s_tbsminx ) * 0.5f;
		diffy = ( CD3DDisp::s_tbsmaxy - CD3DDisp::s_tbsminy ) * 0.5f;
		diffz = ( CD3DDisp::s_tbsmaxz - CD3DDisp::s_tbsminz ) * 0.5f;

		dsttotalbs->rmag = (float)sqrt( diffx * diffx + diffy * diffy + diffz * diffz );
		
	}else{
		dsttotalbs->befcenter.x = 0.0f; 
		dsttotalbs->befcenter.y = 0.0f; 
		dsttotalbs->befcenter.z = 0.0f;

		dsttotalbs->rmag = 0.0f;

	}

	return 0;
}

int CShdHandler::ChkInView( CMotHandler* lpmh, D3DXMATRIX* matWorld, D3DXMATRIX* matView, D3DXMATRIX* matProj, int* retptr )
{
	int ret;
	int dispflag;

	// SHDBBOXの変換
	D3DXVECTOR3 dummyEyePt( 0.0f, 0.0f, 0.0f );
	D3DXMATRIX dummyScale;
	D3DXMatrixIdentity( &dummyScale );

	CShdElem* bbxelem;
	bbxelem = GetBBoxElem();
	if( bbxelem ){
		// eyept, gband, Width, Heightがダミーなので、g_useGPU == 0 かつTLMODE_ORGの時には、結果がおかしくなる。（たぶん、描画されない）
		// g_useGPU != 0のときは、これでも、大丈夫。

		ret = bbxelem->TransformDispData( m_seri2boneno, dummyEyePt, &dummyScale, 
			lpmh, matWorld, matView, matProj, 1 );
		if( ret ){
			DbgOut( "CShdHandler : bbx TransformDispData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	// SetBBoxは、視野外でも計算する。
	ret = InitBoundaryBox();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
		
	SetCurDS();

	int setbbno = 0;
	CShdElem* selem;
	int i;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;

		if( (dispflag || (selem->m_mtype == M_BASE) ) && (selem->notuse != 1) ){
			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){  

				if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

					if( (selem->m_mtype == M_NONE) || (selem->m_mtype == M_BASE) ){ 
						if( selem->polymesh && selem->polymesh->billboardflag ){
							//ここでは、何もしない。						
						}else{
							ret = selem->SetBBox( bbxelem, *matWorld );
							if( ret ){
								DbgOut( "sh : TransformDispData : se SetBBox error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}

							ret = selem->curbs.SetBSphere( &(selem->m_bbx1) );
							_ASSERT( !ret );

							selem->m_center = selem->curbs.tracenter;
						}

						if( setbbno == 0 ){
							m_bbox.SetBBox( &(selem->m_bbx1) );
						}else{
							m_bbox.JoinBBox( &(selem->m_bbx1) );
						}
						setbbno++;
					}
				}
			}
		}	
	}


	ret = lpmh->m_curbs.SetBSphere( &m_bbox );
	_ASSERT( !ret );
	m_center = lpmh->m_curbs.tracenter;

	/////////////

	ret = UpdateFrustumInfo( matView, matProj );
	if( ret ){
		DbgOut( "shdhandler : ChkInView : UpdateFrustumInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//モデル全体が視野内かどうかチェック。
	CBSphere* totalbs;
	totalbs = &(lpmh->m_curbs);
	ret = totalbs->ChkInView( &m_frinfo );
	if( ret ){
		DbgOut( "shdhandler : ChkInView : totalbs ChkInView error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//_ASSERT( totalbs->visibleflag );

	

	//！！！！
	//！！！SHDGROUNDDATA 又は、SHDBILLBOARDがある場合は、totalbsのinvisibleを１にセットする。！！！！
	//！！！！
	CShdElem* chkelem;
	chkelem = (*this)( 1 );
	if( (chkelem->type == SHDGROUNDDATA) || (chkelem->type == SHDBILLBOARD) ){
		totalbs->visibleflag = 1;
	}


	//////


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//あたり判定で、パーツのｂｓ情報を使用するため、視野外でも、パーツごとに計算する。！！！！
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//パーツごとに、視野内かどうかチェック。
	//int i;

	int inviewcnt = 0;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
		int type;
		type = selem->type;
		
		if( (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ((type == SHDPOLYMESH) || (type == SHDPOLYMESH2) || (type == SHDBILLBOARD)) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){

				//if( (selem->m_mtype == M_NONE) || (selem->m_mtype == M_BASE) ){

					if( (type == SHDPOLYMESH) && (selem->polymesh) && (selem->polymesh->billboardflag == 1) ){
						
						//処理をスキップ
						//SHDBILLBOARDに処理を任せる。

					}else if( type == SHDBILLBOARD ){
						CBillboard* bb;
						bb = selem->billboard;
						_ASSERT( bb );

						int bbelemno;
						for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
							CBillboardElem* bbelem;
							bbelem = bb->bbarray + bbelemno;

							if( (bbelem->useflag == 1) && (bbelem->dispflag == 1) ){
								D3DXMATRIX newmatW;
								//D3DXMatrixIdentity( &newmatW );
								newmatW = *matWorld;
								newmatW._41 += bbelem->pos.x;
								newmatW._42 += bbelem->pos.y;
								newmatW._43 += bbelem->pos.z;

								ret = bbelem->selem->curbs.ChkInView( &m_frinfo, newmatW );
								if( ret ){
									DbgOut( "shdhandler : ChkInView : Billboard selem curbs ChkInView error !!!\n" );
									_ASSERT( 0 );
									return 1;
								}

								if( bbelem->selem->curbs.visibleflag > 0 ){
									inviewcnt++;
								}

							}

						}

						
					}else{
						ret = selem->curbs.ChkInView( &m_frinfo );
						if( ret ){
							DbgOut( "shdhandler : ChkInView : selem curbs ChkInView error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
						if( selem->curbs.visibleflag > 0 ){
							inviewcnt++;
						}
					}
				//}
			}
		}
	}

	//totalbs->visibleflag = 1;

	if( retptr ){
		*retptr = inviewcnt;
	}



	return 0;
}


/***
int CShdHandler::ChkInView( CMotHandler* lpmh, D3DMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int* retptr )
{
	int setbbno = 0;
	CShdElem* selem;
	int i;

	CBSphere* totalbs;
	totalbs = &(lpmh->m_curbs);
	totalbs->visibleflag = 1;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		//dispflag = selem->dispflag;
		int type;
		type = selem->type;
		
		if( ((type == SHDPOLYMESH) || (type == SHDPOLYMESH2) || (type == SHDBILLBOARD)) ){

				if( (type == SHDPOLYMESH) && (selem->polymesh) && (selem->polymesh->billboardflag == 1) ){
					
					//処理をスキップ
					//SHDBILLBOARDに処理を任せる。

				}else if( type == SHDBILLBOARD ){
					CBillboard* bb;
					bb = selem->billboard;
					_ASSERT( bb );

					int bbelemno;
					for( bbelemno = 0; bbelemno < MAX_BILLBOARD_NUM; bbelemno++ ){
						CBillboardElem* bbelem;
						bbelem = bb->bbarray + bbelemno;

						if( (bbelem->useflag == 1) && (bbelem->dispflag == 1) ){
							bbelem->selem->curbs.visibleflag = 1;
						}
					}

					
				}else{
					selem->curbs.visibleflag = 1;
				}
		}
	}

	if( retptr ){
		*retptr = 1;
	}

	return 0;
}
***/

int CShdHandler::ChkConfBBX( int srcpartno, CShdHandler* chksh, int chkpartno, int* confflag )
{
	*confflag = 0;

	if( (srcpartno < 0) || (srcpartno >= s2shd_leng) ){
		DbgOut( "sh : ChkConfBBX : srcpartno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !chksh ){
		DbgOut( "sh : ChkConfBBX : chkhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (chkpartno < 0) || (chkpartno >= chksh->s2shd_leng) ){
		DbgOut( "sh : ChkConfBBX : srcpartno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* srcelem;
	srcelem = (*this)( srcpartno );
	_ASSERT( srcelem );

	CShdElem* chkelem;
	chkelem = (*chksh)( chkpartno );
	_ASSERT( chkelem );

	D3DXVECTOR3 rate1( 1.0f, 1.0f, 1.0f );
	int result1 = 0;
	int result2 = 0;

	result1 = srcelem->m_bbx1.ChkConflict( chkelem->m_bbx1, rate1, rate1 );
	if( result1 == 0 ){
		*confflag = 0;
		return 0;
	}else{
		
		*confflag = result1;//!!!!!!!!

		CShdElem* srcbbxelem;
		CShdElem* chkbbxelem;
		srcbbxelem = GetBBoxElem();
		chkbbxelem = chksh->GetBBoxElem();

		if( !srcbbxelem || !chkbbxelem ){
			_ASSERT( 0 );
			return 0;//!!!!!!!!!!!!!!!!
		}

		if( (srcbbxelem->bboxnum == 0) || (chkbbxelem->bboxnum == 0) ){
			_ASSERT( 0 );
			return 0;//!!!!!!!!!!!!!!!!
		}

		CBBox2* srcbbx;
		CBBox2* chkbbx;
		int srcbbxno, chkbbxno;
	
		int checkcnt = 0;


		for( srcbbxno = 0; srcbbxno < srcbbxelem->bboxnum; srcbbxno++ ){
			srcbbx = *( srcbbxelem->ppbbox + srcbbxno );
			_ASSERT( srcbbx );

			if( srcbbx->target == srcelem ){
				
				for( chkbbxno = 0; chkbbxno < chkbbxelem->bboxnum; chkbbxno++ ){
					chkbbx = *( chkbbxelem->ppbbox + chkbbxno );
					_ASSERT( chkbbx );

					if( chkbbx->target == chkelem ){
						result2 = srcbbx->ChkConflict( chkbbx );
						checkcnt++;
						if( result2 != 0 ){
							*confflag = result2;
							return 0;//!!!!!!!!!!!!!!!!!!!!
						}
					}

				}

			}

		}

		if( (checkcnt > 0) && (result2 == 0) ){
			*confflag = 0;
		}

		if( checkcnt == 0 ){
			_ASSERT( 0 );
		}
	}

	return 0;
}


int CShdHandler::ChkConfParts( CMotHandler* srcmh, CShdHandler* chksh, CMotHandler* chkmh, int chkno, int* confflagptr )
{

	if( (chkno < 0) || (chkno >= chksh->s2shd_leng) ){
		DbgOut( "shandler : ChkConfParts : chkno (partno2) error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetCurDS();

//////
	chksh->SetCurDS();

	CShdElem* chkselem;
	chkselem = (*chksh)( chkno );
	_ASSERT( chkselem );

	//////////////////////

	int srcno;
	CShdElem* selem;
	int confflag = 0;
	int ret;

	D3DXVECTOR3 onevec( 1.0f, 1.0f, 1.0f );


	if( (chkselem->dispflag || (chkselem->m_mtype == M_BASE)) && (chkselem->notuse != 1) && ((chkselem->type == SHDPOLYMESH) || (chkselem->type == SHDPOLYMESH2)) ){

		DWORD chkdispswitchno = chkselem->dispswitchno;
		if( (chksh->m_curds + chkdispswitchno)->state != 0 ){  


			for( srcno = 1; srcno < s2shd_leng; srcno++ ){
				selem = (*this)( srcno );

				if( (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){

					DWORD dispswitchno = selem->dispswitchno;
					if( (m_curds + dispswitchno)->state != 0 ){  

						ret = ChkConfBBX( srcno, chksh, chkno, &confflag );
						if( ret ){
							DbgOut( "sh : ChkConfParts : ChkConfBBX error !!!\n" );
							_ASSERT( 0 );
							*confflagptr = 0;
							return 1;
						}
						if( confflag > 0 )
							break;//
					}
				}
			}
		}
	}

	*confflagptr = confflag;

	return 0;
}


int CShdHandler::ChkConfPartsBySphere( CMotHandler* srcmh, CShdHandler* chksh, CMotHandler* chkmh, int chkpartno, int* confflag, float srcrate, float chkrate, int* confidptr, int arrayleng, int* confnumptr )
{
	//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	//！！！引数の異なる同じ関数が、もう２つあるので、注意！！！！
	//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

	*confnumptr = 0;


	int ret;
	int dispflag;

	int i;

	SetCurDS();

//////
	chksh->SetCurDS();

////////

	//////

	CShdElem* selem;


	CShdElem* chkselem;
	int chkdispflag;
	int chkleng;
	CBSphere* srcbs;
	CBSphere* chkbs;

	chkleng = chksh->s2shd_leng;
	chkselem = (*chksh)( chkpartno );
	chkdispflag = chkselem->dispflag;

	*confflag = 0;//!!!

	int tempflag;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
				
		if( (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){  
	
				srcbs = &(selem->curbs);
			/////////
				//for( chkno = 0; chkno < chkleng; chkno++ ){
					//chkselem = (*chksh)( chkno );
					//chkdispflag = chkselem->dispflag;

					if( (chkdispflag || (chkselem->m_mtype == M_BASE)) && (chkselem->notuse != 1) && ((chkselem->type == SHDPOLYMESH) || (chkselem->type == SHDPOLYMESH2)) ){

						DWORD chkdispswitchno = chkselem->dispswitchno;
						if( (chksh->m_curds + chkdispswitchno)->state != 0 ){  
					///////
							chkbs = &(chkselem->curbs);

							ret = srcbs->ChkConflict( chkbs, &tempflag, srcrate, chkrate );
							if( ret ){
								DbgOut( "shdhandler : ChkConfPartsBySphere : srcbs ChkConflict error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}

							if( tempflag != 0 ){
								*confflag = 1;
								//return 0;//
							}
							/***
							if( tempflag && confidptr ){
								if( *confnumptr >= arrayleng ){
									DbgOut( "shandler : ChkConfPartsBySphere : arrayleng too short error !!!\n" );
									_ASSERT( 0 );
									return 1;
								}
								*( confidptr + *confnumptr ) = i;
							}
							if( tempflag )
								(*confnumptr)++;
							***/


							if( tempflag ){
								int chksame = 0;
								
								if( confidptr ){
									int confno;
									for( confno = 0; confno < *confnumptr; confno++ ){
										//同じＩＤがすでに格納されているかをチェック
										if( i == *(confidptr + confno) ){
											chksame = 1;
											break;
										}
									}
									if( chksame == 0 ){
										if( *confnumptr >= arrayleng ){
											DbgOut( "shandler : ChkConfPartsBySphere : arrayleng too short error !!!\n" );
											_ASSERT( 0 );
											return 1;
										}
										*( confidptr + *confnumptr ) = i;
									}

								}

								if( chksame == 0 ){
									(*confnumptr)++;
								}
							}

						}
					}
				//}
			}		
		}
	}


	return 0;
}


int CShdHandler::ChkConfPartsBySphere( CMotHandler* srcmh, CShdHandler* chksh, CMotHandler* chkmh, int* confflag, float srcrate, float chkrate, int* confidptr, int arrayleng, int* confnumptr )
{
	//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	//！！！引数の異なる同じ関数が、もう２つあるので、注意！！！！
	//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

	*confnumptr = 0;


	int ret;
	int dispflag;

	int i;

	SetCurDS();


//////
	chksh->SetCurDS();

////////

	//////

	CShdElem* selem;

	int chkno;
	CShdElem* chkselem;
	int chkdispflag;
	int chkleng;
	CBSphere* srcbs;
	CBSphere* chkbs;

	chkleng = chksh->s2shd_leng;

	*confflag = 0;//!!!
	int tempflag;

	//int findsrchitflag;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
				
		if( (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){  
	
				srcbs = &(selem->curbs);
			/////////

				//findsrchitflag = 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				for( chkno = 0; chkno < chkleng; chkno++ ){
					chkselem = (*chksh)( chkno );
					chkdispflag = chkselem->dispflag;

					if( (chkdispflag || (chkselem->m_mtype == M_BASE)) && (chkselem->notuse != 1) && ((chkselem->type == SHDPOLYMESH) || (chkselem->type == SHDPOLYMESH2)) ){

						DWORD chkdispswitchno = chkselem->dispswitchno;
						if( (chksh->m_curds + chkdispswitchno)->state != 0 ){  
					///////
							chkbs = &(chkselem->curbs);

							ret = srcbs->ChkConflict( chkbs, &tempflag, srcrate, chkrate );
							if( ret ){
								DbgOut( "shdhandler : ChkConfPartsBySphere : srcbs ChkConflict error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}

//if( chkselem->m_mtype == M_BASE ){
//	_ASSERT( 0 );
//}
							if( tempflag != 0 ){
								*confflag = 1;
								//return 0;
							}

							/***
							if( tempflag && confidptr && (findsrchitflag == 0) ){
								if( *confnumptr >= arrayleng ){
									DbgOut( "shandler : ChkConfPartsBySphere : arrayleng too short error !!!\n" );
									_ASSERT( 0 );
									return 1;
								}
								*( confidptr + *confnumptr ) = i;
							}
							if( tempflag ){
								(*confnumptr)++;
								findsrchitflag++;
							}
							***/

							if( tempflag ){
								int chksame = 0;
								
								if( confidptr ){
									int confno;
									for( confno = 0; confno < *confnumptr; confno++ ){
										//同じＩＤがすでに格納されているかをチェック
										if( i == *(confidptr + confno) ){
											chksame = 1;
											break;
										}
									}
									if( chksame == 0 ){
										if( *confnumptr >= arrayleng ){
											DbgOut( "shandler : ChkConfPartsBySphere : arrayleng too short error !!!\n" );
											_ASSERT( 0 );
											return 1;
										}
										*( confidptr + *confnumptr ) = i;
									}

								}

								if( chksame == 0 ){
									(*confnumptr)++;
								}
							}

						}
					}
				}
			}		
		}
	}


	return 0;
}

int CShdHandler::ChkConfPartsBySphere( CMotHandler* srcmh, D3DXVECTOR3 srccenter, float srcr, int* confflag, float srcrate, float chkrate, int* confidptr, int arrayleng, int* confnumptr )
{
	//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	//！！！引数の異なる同じ関数が、もう２つあるので、注意！！！！
	//！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

	//一つのＢＳと、srcの全てのBSとの判定。

	*confnumptr = 0;

	int ret;
	int dispflag;

	int i;

	SetCurDS();

////////

	//////

	CShdElem* selem;
	CBSphere* srcbs;
	CBSphere chkbs;

	
	chkbs.befcenter = srccenter;
	chkbs.tracenter = srccenter;//!!!!
	chkbs.rmag = srcr;//!!!!
	//chkbs.totalrmag = srcr;

	*confflag = 0;//!!!
	int tempflag;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;
				
		if( (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ((selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2)) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){  
	
				srcbs = &(selem->curbs);
			/////////
				

				ret = srcbs->ChkConflict( &chkbs, &tempflag, srcrate, chkrate );
				if( ret ){
					DbgOut( "shdhandler : ChkConfPartsBySphere : srcbs ChkConflict error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				//衝突が見つかったら、直ちに、return する。
				if( tempflag != 0 ){
					*confflag = 1;
					//return 0;
				}

				if( tempflag && confidptr ){
					if( *confnumptr >= arrayleng ){
						DbgOut( "shandler : ChkConfPartsByShere : arrayleng too short error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					*( confidptr + *confnumptr ) = i;
				}
				if( tempflag )
					(*confnumptr)++;

			}		
		}
	}

	return 0;

}


int CShdHandler::SetGroundObj( CMotHandler* lpmh, D3DXMATRIX matWorld )
{
	int i, ret;
	int dispflag;
	CShdElem* selem;



	SetCurDS();
	m_mc.InitCache( &matWorld );

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;

		int isg;
		if( ((selem->type == SHDPOLYMESH) && (selem->polymesh->groundflag == 1)) ||
			((selem->type == SHDPOLYMESH2) && (selem->polymesh2->groundflag == 1))
		){
			isg = 1;
		}else{
			isg = 0;
		}

		if( isg && (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){  

				ret = selem->SetGroundObj( &m_mc, lpmh, matWorld );
				if( ret ){
					DbgOut( "CShdHandler : SetGroundObj : selem SetGroundObj error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;
}


int CShdHandler::ChkConfWall( int* partarray, int partnum, D3DXMATRIX* matWorld, D3DXVECTOR3 bpos, D3DXVECTOR3 npos, CMotHandler* lpmh, float srcdist, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv )
{
	//ぶつかった場合は、ぶつかった座標に、面の法線の定数倍を足して、補正する。
	//補正後の座標も、壁とぶつかっていないか、チェックする。
	//補正後の座標と、ぶつかった場合は、壁との衝突点を返す。

	int ret;
	int result1;
	D3DXVECTOR3 adjustv1, nv1;
	DISTSAMPLE ds;
	ds.setflag = 0;
	ds.dist = 1e10;
	ds.nv.x = 0.0f;
	ds.nv.y = 0.0f;
	ds.nv.z = 0.0f;

	ret = ChkConfGround( partarray, partnum, matWorld, bpos, npos, lpmh, 0, 200.0f, -100.0f, &result1, &adjustv1, &nv1, &ds );
	if( ret ){
		DbgOut( "shdhandler : ChkConfWall : check1 : ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( result1 == 0 ){
		if( (ds.setflag == 0) || (ds.dist >= srcdist) ){ 
			*resultptr = 0;
			*adjustv = npos;
			nv->x = 0.0f;
			nv->y = 0.0f;
			nv->z = 0.0f;
		}else{
			// 近すぎるポリゴンが存在。srcdist分離す。
			float diffdist;
			diffdist = srcdist - ds.dist;

			*resultptr = 2;
			*adjustv = npos + diffdist * ds.nv;
			*nv = ds.nv;

		}
	}else{
		int result2;
		D3DXVECTOR3 adjustv2, nv2;
		D3DXVECTOR3 bpos2, npos2;
		bpos2 = adjustv1 + 0.010f * srcdist * nv1;
		npos2 = adjustv1 + srcdist * nv1;


		D3DXVECTOR3 zerovec( 0.0f, 0.0f, 0.0f );
		if( nv1 == zerovec ){
			DbgOut( "shdhandler : ChkConfWall : check2 : nv1 error !!!\n" );
			_ASSERT( 0 );
		}

		//新しい位置が、他の面にも、ぶつかっていないかをチェックする。
		ret = ChkConfGround( partarray, partnum, matWorld, bpos2, npos2, lpmh, 0, 200.0f, -100.0f, &result2, &adjustv2, &nv2 );
		if( ret ){
			DbgOut( "shdhandler : ChkConfWall : check2 : ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( result2 == 0 ){
			*resultptr = 1;
			*adjustv = npos2;
			*nv = nv1;
		}else{
			*resultptr = 1;
			*adjustv = bpos2;
			*nv = nv1;
		}
	}

	return 0;
}

int CShdHandler::ChkConfFace( int* partarray, int partnum, D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, D3DXVECTOR3 befposwv, D3DXVECTOR3 newposwv, CMotHandler* lpmh, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv, CONFDATA* retcf, DISTSAMPLE* ds )
{
	int ret;
	*resultptr = 0;

	SetCurDS();

	int sameflag1;

	if( befpos != newpos ){
		sameflag1 = 0;
	}else{
		sameflag1 = 1;
	}


//bef-->newとパーツ単位の球判定

	CShdElem* confparts[CONFPARTSMAX];
	int confpartsnum = 0;

	if( sameflag1 == 0 ){
		if( partarray == 0 ){
			ret = ChkConfVecAndGroundParts( 0, &befpos, &newpos, &confpartsnum, &(confparts[0]), CONFPARTSMAX );
			if( ret ){
				DbgOut( "shdhandler : ChkConfFace : ChkConfVecAndParts error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			int pno;
			for( pno = 0; pno < partnum; pno++ ){
				CShdElem* selem;
				selem = (*this)( *( partarray + pno ) );
				_ASSERT( selem );
				confparts[ pno ] = selem;
			}
			confpartsnum = partnum;
		}
	}

//bef-->newとポリゴン単位の球判定
// &
//bef-->newとポリゴンの詳細判定


	CONFDATA confdata[CONFPOINTMAX];
	int confpointnum = 0;
	DISTSAMPLE neards;
	neards.setflag = 0;
	neards.dist = 1e10;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;

	if( (sameflag1 == 0) && confpartsnum ){
		int partsno;
		CShdElem* partsptr;

		int curpointnum = 0;
		
		for( partsno = 0; partsno < confpartsnum; partsno++ ){
			partsptr = confparts[partsno];

			curpointnum = 0;
			if( (partsptr->notuse != 0) || (partsptr->type == SHDEXTLINE) ){
				continue;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}


			if( confpointnum >= CONFPOINTMAX ){
				DbgOut( "shdhandler : ChkConfFace : confpointnum error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = partsptr->ChkConfVecAndFace( &befposwv, &newposwv, &curpointnum, &(confdata[confpointnum]), CONFPOINTMAX - confpointnum, ds );
			if( ret ){
				DbgOut( "shdhandler : ChkConfFace : ChkConfVecAndFace error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			confpointnum += curpointnum;

			if( curpointnum > 0 ){
				if( ds ){
					if( ds->setflag && (ds->dist < neards.dist) ){
						neards = *ds;
					}
				}
			}
//_ASSERT( 0 );
		}
	}

	if( confpointnum > 0 ){

		CONFDATA nearconf;
		D3DXVECTOR3* curadv;
		float minmag = (float)1e10;

		int confno;
		for( confno = 0; confno < confpointnum; confno++ ){
			float diffx, diffy, diffz;
			float curmag;
			curadv = &(confdata[confno].adjustv);

			//diffx = befpos.x - curadv->x;
			//diffy = befpos.y - curadv->y;
			//diffz = befpos.z - curadv->z;

			diffx = befposwv.x - curadv->x;
			diffy = befposwv.y - curadv->y;
			diffz = befposwv.z - curadv->z;


			curmag = diffx * diffx + diffy * diffy + diffz * diffz;
			if( curmag < minmag ){
				nearconf = confdata[confno];
				minmag = curmag;
			}
		}

		*resultptr = 1;//!!!!!!!
		*adjustv = nearconf.adjustv;
		*nv = nearconf.nv;

		*retcf = nearconf;

		*ds = neards;

//_ASSERT( 0 );


	}else{
		*resultptr = 0;//!!!!!!!!!
		*adjustv = newpos;
		nv->x = 0.0f;
		nv->y = 0.0f;
		nv->z = 0.0f;

//_ASSERT( 0 );

	}


	return 0;
}



int CShdHandler::ChkConfGround( int* partarray, int partnum, D3DXMATRIX* matWorld, D3DXVECTOR3 bpos, D3DXVECTOR3 npos, CMotHandler* lpmh, int mode, float diffmaxy, float mapminy, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv, DISTSAMPLE* ds, CONFDATA* retcf )
{//mode == 0 は飛ぶモード、mode == 1 は、這うモード
	int ret;



	SetCurDS();

////
	D3DXVECTOR3 befpos, newpos;
	if( mode == 0 ){
		befpos = bpos;
		newpos = npos;
	}else{
		befpos.x = bpos.x;
		befpos.y = bpos.y + diffmaxy;
		befpos.z = bpos.z;

		newpos.x = npos.x;
		newpos.y = npos.y + diffmaxy;
		newpos.z = npos.z;
	}

	D3DXVECTOR3 befinvpos, newinvpos;
	D3DXMATRIX mats, matsw, matinvsw;


//	D3DXMatrixInverse( &matinvw, NULL, matWorld );
//	D3DXVec3TransformCoord( &befinvpos, &befpos, &matinvw );
//	D3DXVec3TransformCoord( &newinvpos, &newpos, &matinvw );
	

	int sameflag1;
	if( befpos != newpos ){
		sameflag1 = 0;
	}else{
		sameflag1 = 1;
	}

//bef-->newとパーツ単位の球判定

	CShdElem* confparts[CONFPARTSMAX];
	int confpartsnum = 0;

	if( sameflag1 == 0 ){
		if( partarray == 0 ){
			ret = ChkConfVecAndGroundParts( 1, &befpos, &newpos, &confpartsnum, &(confparts[0]), CONFPARTSMAX );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndParts error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			int pno;
			for( pno = 0; pno < partnum; pno++ ){
				CShdElem* selem;
				selem = (*this)( *( partarray + pno ) );
				_ASSERT( selem );
				confparts[ pno ] = selem;
			}
			confpartsnum = partnum;
		}
	}

//bef-->newとポリゴン単位の球判定
// &
//bef-->newとポリゴンの詳細判定


	CONFDATA confdata[CONFPOINTMAX];
	int confpointnum = 0;
	DISTSAMPLE neards;
	neards.setflag = 0;
	neards.dist = (float)1e13;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;

	if( (sameflag1 == 0) && confpartsnum ){
		int partsno;
		CShdElem* partsptr;

		int curpointnum;
		
		for( partsno = 0; partsno < confpartsnum; partsno++ ){
			partsptr = confparts[partsno];
	
			if( partsptr->notuse != 0 ){
				continue;
			}

			if( confpointnum >= CONFPOINTMAX ){
				DbgOut( "shdhandler : ChkConfGround : confpointnum error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = partsptr->GetScaleMat( &mats );
			_ASSERT( !ret );
			matsw = mats * *matWorld;
			D3DXMatrixInverse( &matinvsw, NULL, &matsw );
			D3DXVec3TransformCoord( &befinvpos, &befpos, &matinvsw );
			D3DXVec3TransformCoord( &newinvpos, &newpos, &matinvsw );

			curpointnum = 0;
			ret = partsptr->ChkConfVecAndGroundFace( &befinvpos, &newinvpos, &curpointnum, &(confdata[confpointnum]), CONFPOINTMAX - confpointnum, ds );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndGroundFace error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			confpointnum += curpointnum;

			if( curpointnum > 0 ){
				if( ds ){
					if( ds->setflag && (ds->dist < neards.dist) ){
						neards = *ds;
					}
				}
			}
		}
	}


//// 結果その１

	if( confpointnum > 0 ){
		// mode 0, mode 1 どちらの場合も、returnする。

		CONFDATA nearconf;
		D3DXVECTOR3* curadv;
		float minmag = (float)1e13;

		int confno;
		for( confno = 0; confno < confpointnum; confno++ ){
			float diffx, diffy, diffz;
			float curmag;
			curadv = &(confdata[confno].adjustv);

			diffx = befinvpos.x - curadv->x;
			diffy = befinvpos.y - curadv->y;
			diffz = befinvpos.z - curadv->z;

			curmag = diffx * diffx + diffy * diffy + diffz * diffz;
			if( curmag < minmag ){
				nearconf = confdata[confno];
				minmag = curmag;
			}
		}

		CShdElem* confselem;
		D3DXMATRIX confmats, confmatsw;
		if( nearconf.partsno > 0 ){
			confselem = (*this)( nearconf.partsno );
			confselem->GetScaleMat( &confmats );
			confmatsw = confmats * *matWorld;
		}else{
			confmatsw = *matWorld;
		}

		D3DXVECTOR3 hitv, hitn;
		D3DXVec3TransformCoord( &hitv, &(nearconf.adjustv), &confmatsw );
		D3DXMATRIX matN;
		matN = confmatsw;
		matN._41 = 0.0f;
		matN._42 = 0.0f;
		matN._43 = 0.0f;
		D3DXVec3TransformCoord( &hitn, &(nearconf.nv), &matN );
		DXVec3Normalize( &hitn, &hitn );

		*resultptr = 1;//!!!!!!!
		*adjustv = hitv;
		*nv = hitn;

		nearconf.adjustv = hitv;
		nearconf.nv = hitn;

		if( retcf ){
			*retcf = nearconf;
		}

		neards.nv = hitn;
		D3DXVECTOR3 dv;
		dv = hitv - bpos;
		neards.dist = D3DXVec3Length( &dv );

		if( ds ){
			*ds = neards;
		}


//_ASSERT( 0 );

		return 0;

	}else{
		// mode 0 のときのみ、returnする。
		if( ds ){
			*ds = neards;
		}


		if( mode == 0 ){

			*resultptr = 0;//!!!!!!!!!
			*adjustv = npos;
			nv->x = 0.0f;
			nv->y = 0.0f;
			nv->z = 0.0f;

			return 0;
		}
	}


//mode == 1かつ、bef-->newとあたりが無い場合
	// mapminy から　downposを生成。
	D3DXVECTOR3 downpos;
	downpos.x = newpos.x;
	downpos.y = mapminy;
	downpos.z = newpos.z;

	int sameflag2;
	if( newpos != downpos ){
		sameflag2 = 0;
	}else{
		sameflag2 = 1;
	}

	D3DXVECTOR3 downinvpos;
//	D3DXVec3TransformCoord( &downinvpos, &downpos, &matinvw );

	//new--->downposとポリゴンの交点を求める。
	
		//new--->downposとパーツ単位の球判定。
	CShdElem* confparts2[CONFPARTSMAX];
	int confpartsnum2 = 0;

	if( sameflag2 == 0 ){
		if( partarray == 0 ){
			ret = ChkConfVecAndGroundParts( 1, &newpos, &downpos, &confpartsnum2, &(confparts2[0]), CONFPARTSMAX );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndParts 2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			int pno;
			for( pno = 0; pno < partnum; pno++ ){
				CShdElem* selem;
				selem = (*this)( *( partarray + pno ) );
				_ASSERT( selem );
				confparts2[ pno ] = selem;
			}
			confpartsnum2 = partnum;
		}
	}
		//new--->downposとポリゴン単位の球判定。
		// &
		//new--->downposとポリゴンの詳細判定。

	CONFDATA confdata2[CONFPOINTMAX];
	int confpointnum2 = 0;

	neards.setflag = 0;
	neards.dist = (float)1e13;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;

	if( ds ){
		*ds = neards;
	}
	
	if( (sameflag2 == 0) && confpartsnum2 ){
		int partsno;
		CShdElem* partsptr;

		int curpointnum;

		for( partsno = 0; partsno < confpartsnum2; partsno++ ){
			partsptr = confparts2[partsno];

			if( confpointnum2 >= CONFPOINTMAX ){
				DbgOut( "shdhandler : ChkConfGround 2 : confpointnum2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = partsptr->GetScaleMat( &mats );
			_ASSERT( !ret );
			matsw = mats * *matWorld;
			D3DXMatrixInverse( &matinvsw, NULL, &matsw );
			D3DXVec3TransformCoord( &downinvpos, &downpos, &matinvsw );
			D3DXVec3TransformCoord( &newinvpos, &newpos, &matinvsw );

			curpointnum = 0;
			ret = partsptr->ChkConfVecAndGroundFace( &newinvpos, &downinvpos, &curpointnum, &(confdata2[confpointnum2]), CONFPOINTMAX - confpointnum2, ds );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndGroundFace 2 error !!!\n" );
				_ASSERT( 0 );
				return 0;
			}

			confpointnum2 += curpointnum;


			if( curpointnum > 0 ){
				if( ds ){
					if( ds->setflag && (ds->dist < neards.dist) ){
						neards = *ds;
					}
				}
			}

		}
	}
		
//// 結果その２

	if( confpointnum2 > 0 ){
		// mode 1 

		CONFDATA nearconf2;
		D3DXVECTOR3* curadv2;
		nearconf2.adjustv = newinvpos;//!!!!!!!!!!!!
		float minmag2 = (float)1e13;

		int confno2;
		for( confno2 = 0; confno2 < confpointnum2; confno2++ ){
			float diffx, diffy, diffz;
			float curmag;
			curadv2 = &(confdata2[confno2].adjustv);

			diffx = newinvpos.x - curadv2->x;
			diffy = newinvpos.y - curadv2->y;
			diffz = newinvpos.z - curadv2->z;

			curmag = diffx * diffx + diffy * diffy + diffz * diffz;
			if( curmag < minmag2 ){
				nearconf2 = confdata2[confno2];
				minmag2 = curmag;
			}
		}

		CShdElem* confselem;
		D3DXMATRIX confmats, confmatsw;
		if( nearconf2.partsno > 0 ){
			confselem = (*this)( nearconf2.partsno );
			confselem->GetScaleMat( &confmats );
			confmatsw = confmats * *matWorld;
		}else{
			confmatsw = *matWorld;
		}

		D3DXVECTOR3 hitv, hitn;
		D3DXVec3TransformCoord( &hitv, &(nearconf2.adjustv), &confmatsw );
		D3DXMATRIX matN;
		matN = confmatsw;
		matN._41 = 0.0f;
		matN._42 = 0.0f;
		matN._43 = 0.0f;
		D3DXVec3TransformCoord( &hitn, &(nearconf2.nv), &matN );
		DXVec3Normalize( &hitn, &hitn );

		*resultptr = 2;//!!!!!!!
		*adjustv = hitv;
		*nv = hitn;

		nearconf2.adjustv = hitv;
		nearconf2.nv = hitn;

		if( retcf ){
			*retcf = nearconf2;
		}

		neards.nv = hitn;
		D3DXVECTOR3 dv;
		dv = hitv - bpos;
		neards.dist = D3DXVec3Length( &dv );

		if( ds ){
			*ds = neards;
		}

		return 0;

	}else{
		// mode 1

//_ASSERT( 0 );

		*resultptr = 0;//!!!!!!!!!
		*adjustv = npos;
		nv->x = 0.0f;
		nv->y = 0.0f;
		nv->z = 0.0f;

		return 0;
	}
	

	return 0;
}

int CShdHandler::ChkConfGroundPart( D3DXMATRIX* matWorld, int groundpart, D3DXVECTOR3 bpos, D3DXVECTOR3 npos, CMotHandler* lpmh, int mode, float diffmaxy, float mapminy, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv, DISTSAMPLE* ds, CONFDATA* retcf )
{//mode == 0 は飛ぶモード、mode == 1 は、這うモード
	int ret;

	if( (groundpart < 0) || (groundpart >= s2shd_leng) ){
		DbgOut( "sh : ChkConfGroundPart : groundpart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////
	D3DXVECTOR3 befpos, newpos;
	if( mode == 0 ){
		befpos = bpos;
		newpos = npos;
	}else{
		befpos.x = bpos.x;
		befpos.y = bpos.y + diffmaxy;
		befpos.z = bpos.z;

		newpos.x = npos.x;
		newpos.y = npos.y + diffmaxy;
		newpos.z = npos.z;
	}
	int sameflag1;

	if( befpos != newpos ){
		sameflag1 = 0;
	}else{
		sameflag1 = 1;
	}

	D3DXVECTOR3 befinvpos, newinvpos;
	D3DXMATRIX mats, matsw, matinvsw;
//	D3DXMatrixInverse( &matinvw, NULL, matWorld );
//	D3DXVec3TransformCoord( &befinvpos, &befpos, &matinvw );
//	D3DXVec3TransformCoord( &newinvpos, &newpos, &matinvw );

//bef-->newとパーツ単位の球判定

	CShdElem* confparts[CONFPARTSMAX];
	int confpartsnum = 0;

	if( sameflag1 == 0 ){
		//ret = ChkConfVecAndGroundParts( 1, &befpos, &newpos, curdispswitch, &confpartsnum, &(confparts[0]), CONFPARTSMAX );
		//if( ret ){
		//	DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndParts error !!!\n" );
		//	_ASSERT( 0 );
		//	return 1;
		//}
		CShdElem* selem;
		float dist;
		float diffx, diffy, diffz;

		diffx = newpos.x - befpos.x;
		diffy = newpos.y - befpos.y;
		diffz = newpos.z - befpos.z;

		dist = sqrtf( diffx * diffx + diffy * diffy + diffz * diffz ) * 0.50f;
		
		D3DXVECTOR3 srccenter;
		srccenter.x = ( befpos.x + newpos.x ) * 0.50f;
		srccenter.y = ( befpos.y + newpos.y ) * 0.50f;
		srccenter.z = ( befpos.z + newpos.z ) * 0.50f;

		int confno = 0;

			
		selem = (*this)( groundpart );
		_ASSERT( selem );
		
		if( (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) )){

			int result;				
			ret = selem->curbs.ChkConflict( &srccenter, dist, &result );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGroundPart : curbs ChkConflict error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( result != 0 ){
				confparts[0] = selem;					
				confpartsnum = 1;
			}
		}
	}

//bef-->newとポリゴン単位の球判定
// &
//bef-->newとポリゴンの詳細判定


	CONFDATA confdata[CONFPOINTMAX];
	int confpointnum = 0;
	DISTSAMPLE neards;
	neards.setflag = 0;
	neards.dist = (float)1e13;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;

	if( (sameflag1 == 0) && confpartsnum ){
		int partsno;
		CShdElem* partsptr;

		int curpointnum = 0;
		
		for( partsno = 0; partsno < confpartsnum; partsno++ ){
			partsptr = confparts[partsno];

			if( confpointnum >= CONFPOINTMAX ){
				DbgOut( "shdhandler : ChkConfGround : confpointnum error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = partsptr->GetScaleMat( &mats );
			_ASSERT( !ret );
			matsw = mats * *matWorld;
			D3DXMatrixInverse( &matinvsw, NULL, &matsw );
			D3DXVec3TransformCoord( &befinvpos, &befpos, &matinvsw );
			D3DXVec3TransformCoord( &newinvpos, &newpos, &matinvsw );

			ret = partsptr->ChkConfVecAndGroundFace( &befinvpos, &newinvpos, &curpointnum, &(confdata[confpointnum]), CONFPOINTMAX - confpointnum, ds );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndGroundFace error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			confpointnum += curpointnum;

			if( curpointnum > 0 ){
				if( ds ){
					if( ds->setflag && (ds->dist < neards.dist) ){
						neards = *ds;
					}
				}
			}
		}
	}


//// 結果その１

	if( confpointnum > 0 ){
		// mode 0, mode 1 どちらの場合も、returnする。

		CONFDATA nearconf;
		D3DXVECTOR3* curadv;
		float minmag = (float)1e13;

		int confno;
		for( confno = 0; confno < confpointnum; confno++ ){
			float diffx, diffy, diffz;
			float curmag;
			curadv = &(confdata[confno].adjustv);

			diffx = befinvpos.x - curadv->x;
			diffy = befinvpos.y - curadv->y;
			diffz = befinvpos.z - curadv->z;

			curmag = diffx * diffx + diffy * diffy + diffz * diffz;
			if( curmag < minmag ){
				nearconf = confdata[confno];
				minmag = curmag;
			}
		}

		CShdElem* confselem;
		D3DXMATRIX confmats, confmatsw;
		if( nearconf.partsno > 0 ){
			confselem = (*this)( nearconf.partsno );
			confselem->GetScaleMat( &confmats );
			confmatsw = confmats * *matWorld;
		}else{
			confmatsw = *matWorld;
		}

		D3DXVECTOR3 hitv, hitn;
		D3DXVec3TransformCoord( &hitv, &(nearconf.adjustv), &confmatsw );
		D3DXMATRIX matN;
		matN = confmatsw;
		matN._41 = 0.0f;
		matN._42 = 0.0f;
		matN._43 = 0.0f;
		D3DXVec3TransformCoord( &hitn, &(nearconf.nv), &matN );
		DXVec3Normalize( &hitn, &hitn );

		*resultptr = 1;//!!!!!!!

//		if( hitn.y < 0.1736481f ){
//			*adjustv = bpos;
//		}else{
			*adjustv = hitv;
//		}
		*nv = hitn;

		nearconf.adjustv = hitv;
		nearconf.nv = hitn;

		if( retcf ){
			*retcf = nearconf;
		}

		neards.nv = hitn;
		D3DXVECTOR3 dv;
		//dv = hitv - bpos;
		dv = *adjustv - bpos;
		neards.dist = D3DXVec3Length( &dv );

		if( ds ){
			*ds = neards;
		}

//_ASSERT( 0 );

		return 0;

	}else{
		// mode 0 のときのみ、returnする。

		if( ds ){
			*ds = neards;
		}

		if( mode == 0 ){
			*resultptr = 0;//!!!!!!!!!
			*adjustv = npos;
			nv->x = 0.0f;
			nv->y = 0.0f;
			nv->z = 0.0f;

			return 0;
		}
	}


//mode == 1かつ、bef-->newとあたりが無い場合
	// mapminy から　downposを生成。
	D3DXVECTOR3 downpos;
	downpos.x = newpos.x;
	downpos.y = mapminy;
	downpos.z = newpos.z;

	int sameflag2;
	if( newpos != downpos ){
		sameflag2 = 0;
	}else{
		sameflag2 = 1;
	}

	D3DXVECTOR3 downinvpos;
//	D3DXVec3TransformCoord( &downinvpos, &downpos, &matinvw );


	//new--->downposとポリゴンの交点を求める。
	
		//new--->downposとパーツ単位の球判定。
	CShdElem* confparts2[CONFPARTSMAX];
	int confpartsnum2 = 0;

	if( sameflag2 == 0 ){
		//DWORD curdispswitch = 0xFFFFFFFF;
		//ret = ChkConfVecAndGroundParts( 1, &newpos, &downpos, curdispswitch, &confpartsnum2, &(confparts2[0]), CONFPARTSMAX );
		//if( ret ){
		//	DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndParts 2 error !!!\n" );
		//	_ASSERT( 0 );
		//	return 1;
		//}

		CShdElem* selem;
		float dist;
		float diffx, diffy, diffz;

		diffx = downpos.x - newpos.x;
		diffy = downpos.y - newpos.y;
		diffz = downpos.z - newpos.z;

		dist = sqrtf( diffx * diffx + diffy * diffy + diffz * diffz ) * 0.50f;

		D3DXVECTOR3 srccenter;
		srccenter.x = ( downpos.x + newpos.x ) * 0.50f;
		
		srccenter.y = ( downpos.y + newpos.y ) * 0.50f;

		srccenter.z = ( downpos.z + newpos.z ) * 0.50f;

		int confno = 0;

			
		selem = (*this)( groundpart );
		_ASSERT( selem );
		
		if( (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) )){

			int result;				
			ret = selem->curbs.ChkConflict( &srccenter, dist, &result );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGroundPart : curbs ChkConflict error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( result != 0 ){
				confparts2[0] = selem;					
				confpartsnum2 = 1;
			}
		}



	}
		//new--->downposとポリゴン単位の球判定。
		// &
		//new--->downposとポリゴンの詳細判定。

	CONFDATA confdata2[CONFPOINTMAX];
	int confpointnum2 = 0;

	neards.setflag = 0;
	neards.dist = (float)1e13;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;
	if( (sameflag2 == 0) && confpartsnum2 ){
		int partsno;
		CShdElem* partsptr;

		int curpointnum = 0;

		for( partsno = 0; partsno < confpartsnum2; partsno++ ){
			partsptr = confparts2[partsno];

			if( confpointnum2 >= CONFPOINTMAX ){
				DbgOut( "shdhandler : ChkConfGround 2 : confpointnum2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = partsptr->GetScaleMat( &mats );
			_ASSERT( !ret );
			matsw = mats * *matWorld;
			D3DXMatrixInverse( &matinvsw, NULL, &matsw );
			D3DXVec3TransformCoord( &downinvpos, &downpos, &matinvsw );
			D3DXVec3TransformCoord( &newinvpos, &newpos, &matinvsw );

			ret = partsptr->ChkConfVecAndGroundFace( &newinvpos, &downinvpos, &curpointnum, &(confdata2[confpointnum2]), CONFPOINTMAX - confpointnum2, ds );
			if( ret ){
				DbgOut( "shdhandler : ChkConfGround : ChkConfVecAndGroundFace 2 error !!!\n" );
				_ASSERT( 0 );
				return 0;
			}

			confpointnum2 += curpointnum;


			if( curpointnum > 0 ){
				if( ds ){
					if( ds->setflag && (ds->dist < neards.dist) ){
						neards = *ds;
					}
				}
			}

		}
	}
		
//// 結果その２

	if( confpointnum2 > 0 ){
		// mode 1 

		CONFDATA nearconf;
		D3DXVECTOR3* curadv;
		float minmag = (float)1e13;

		int confno;
		for( confno = 0; confno < confpointnum2; confno++ ){
			float diffx, diffy, diffz;
			float curmag;
			curadv = &(confdata2[confno].adjustv);

			diffx = newinvpos.x - curadv->x;
			diffy = newinvpos.y - curadv->y;
			diffz = newinvpos.z - curadv->z;

			curmag = diffx * diffx + diffy * diffy + diffz * diffz;
			if( curmag < minmag ){
				nearconf = confdata2[confno];
				minmag = curmag;
			}
		}


		CShdElem* confselem;
		D3DXMATRIX confmats, confmatsw;
		if( nearconf.partsno > 0 ){
			confselem = (*this)( nearconf.partsno );
			confselem->GetScaleMat( &confmats );
			confmatsw = confmats * *matWorld;
		}else{
			confmatsw = *matWorld;
		}

		D3DXVECTOR3 hitv, hitn;
		D3DXVec3TransformCoord( &hitv, &(nearconf.adjustv), &confmatsw );
		D3DXMATRIX matN;
		matN = confmatsw;
		matN._41 = 0.0f;
		matN._42 = 0.0f;
		matN._43 = 0.0f;
		D3DXVec3TransformCoord( &hitn, &(nearconf.nv), &matN );
		DXVec3Normalize( &hitn, &hitn );

		*resultptr = 2;//!!!!!!!
		*adjustv = hitv;
		*nv = hitn;

		nearconf.adjustv = hitv;
		nearconf.nv = hitn;

		if( retcf ){
			*retcf = nearconf;
		}

		neards.nv = hitn;
		D3DXVECTOR3 dv;
		dv = hitv - bpos;
		neards.dist = D3DXVec3Length( &dv );

		if( ds ){
			*ds = neards;
		}

		return 0;

	}else{
		// mode 1

//_ASSERT( 0 );

		*resultptr = 0;//!!!!!!!!!
		*adjustv = npos;
		nv->x = 0.0f;
		nv->y = 0.0f;
		nv->z = 0.0f;

		return 0;
	}
	

	return 0;
}

int CShdHandler::ChkConfVecAndGroundParts( int groundonly, D3DXVECTOR3* befpos, D3DXVECTOR3* newpos, int* confpartsnum, CShdElem** confparts, int confmaxnum )
{

	SetCurDS();


	CShdElem* selem;
	int dispflag;
	int i, ret;

	float dist;
	float diffx, diffy, diffz;

	diffx = newpos->x - befpos->x;
	diffy = newpos->y - befpos->y;
	diffz = newpos->z - befpos->z;

	dist = sqrtf( diffx * diffx + diffy * diffy + diffz * diffz ) * 0.50f;
	
	D3DXVECTOR3 srccenter;
	srccenter.x = ( befpos->x + newpos->x ) * 0.50f;
	srccenter.y = ( befpos->y + newpos->y ) * 0.50f;
	srccenter.z = ( befpos->z + newpos->z ) * 0.50f;

	int confno = 0;

	for( i = 0; i < s2shd_leng; i++ ){
		
		selem = (*this)( i );
		dispflag = selem->dispflag;

		int isg;

		if( groundonly == 1 ){
			if( ((selem->type == SHDPOLYMESH) && (selem->polymesh->groundflag == 1)) ||
				((selem->type == SHDPOLYMESH2) && (selem->polymesh2->groundflag == 1))
			){
				isg = 1;
			}else{
				isg = 0;
			}
		}else{
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) )
			{
				isg = 1;
			}else{
				isg = 0;
			}
		}

		if( isg && (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){
				int result;

			
				ret = selem->curbs.ChkConflict( &srccenter, dist, &result );
				if( ret ){
					DbgOut( "shdhandler : ChkConfVecAndGroundParts : curbs ChkConflict error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( result != 0 ){
					if( confno >= confmaxnum ){
						DbgOut( "shdhandler : ChkConfVecAndGroundParts : confno error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					*(confparts + confno) = selem;					
					confno++;
				}
				
				
				/***
				if( confno >= confmaxnum ){
					DbgOut( "shdhandler : ChkConfVecAndGroundParts : confno error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				*(confparts + confno) = selem;
				confno++;
				***/
			}
		}
	}

	*confpartsnum = confno;

	return 0;
}

int CShdHandler::GetVertNumOfPartInfScope( int isindex, int* vertnumptr )
{
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}

	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "sh : GetVertNumOfPartInfScope : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( iselem->ppscope + isindex );
	_ASSERT( curis );


	CD3DDisp* d3ddispptr;
	d3ddispptr = curis->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : GetVertNumOfPartInfScope : 3dobject not exist error !!!\n" );

		*vertnumptr = 0;

		_ASSERT( 0 );
		return 0;//!!!
	}

	*vertnumptr = d3ddispptr->m_numTLV;

	return 0;
}


int CShdHandler::GetVertNumOfPart( int partno, int* vertnumptr )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetVertNumOfPart : partno error !!!\n" );

		*vertnumptr = 0;

		_ASSERT( 0 );
		return 0;//!!!
	}

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : GetVertNumOfPart : 3dobject not exist error !!!\n" );

		*vertnumptr = 0;

		_ASSERT( 0 );
		return 0;//!!!
	}

	*vertnumptr = d3ddispptr->m_numTLV;

	return 0;
}

int CShdHandler::GetInitialFacePos( int partno, int faceno, D3DXVECTOR3* facepos, D3DXMATRIX matWorld )
{
	int ret;
	int v1, v2, v3;

	ret = GetVertNoOfFace( partno, faceno, &v1, &v2, &v3 );
	if( ret ){
		DbgOut( "shandler : GetInitialFacePos : GetVertNoOfFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 pos1, pos2, pos3;
	ret = GetVertPos( partno, v1, &pos1 );
	if( ret ){
		DbgOut( "shandler : GetInitialFacePos : GetVertPos 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetVertPos( partno, v2, &pos2 );
	if( ret ){
		DbgOut( "shandler : GetInitialFacePos : GetVertPos 2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetVertPos( partno, v3, &pos3 );
	if( ret ){
		DbgOut( "shandler : GetInitialFacePos : GetVertPos 3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 avpos;
	avpos = ( pos1 + pos2 + pos3 ) / 3.0f;

	D3DXVec3TransformCoord( facepos, &avpos, &matWorld );

	return 0;
}

int CShdHandler::GetInitialInfScopeCenter( int isindex, D3DXVECTOR3* partpos, D3DXMATRIX matWorld )
{
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		DbgOut( "sh : GetInitialInfScopeCenter : iselem NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 temppos;
	ret = iselem->GetInitialInfScopeCenter( isindex, &temppos );
	if( ret ){
		DbgOut( "shandler : GetInitialInfScopeCenter : selem GetInitialInfScopeCenter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVec3TransformCoord( partpos, &temppos, &matWorld );

	return 0;

}

int CShdHandler::GetInitialPartPos( int partno, D3DXVECTOR3* partpos, D3DXMATRIX matWorld )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetInitialPartPos : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;
	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	D3DXVECTOR3 temppos;
	ret = selem->GetInitialPartPos( &temppos );
	if( ret ){
		DbgOut( "shandler : GetInitialPartPos : selem GetInitialPartPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVec3TransformCoord( partpos, &temppos, &matWorld );

	return 0;
}




int CShdHandler::GetVertPos( int partno, int vertno, D3DXVECTOR3* vpos )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetVertPos : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	//int ret;

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : GetVertPos : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	if( (vertno < 0) || (vertno >= vertnum) ){
		DbgOut( "shdhandler : GetVertPos : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!		
	}

	vpos->x = ( d3ddispptr->m_skinv + vertno )->pos[0];
	vpos->y = ( d3ddispptr->m_skinv + vertno )->pos[1];
	vpos->z = ( d3ddispptr->m_skinv + vertno )->pos[2];
	//DbgOut( "shdhandler : GetVertPos : %f %f %f\n", vpos->x, vpos->y, vpos->z );


/***
	int ret;
	CPolyMesh* pm;
	CPolyMesh2* pm2;

	int elemtype;
	elemtype = selem->type;
	switch( elemtype ){
	case SHDPOLYMESH:
		pm = selem->polymesh;
		if( !pm ){
			DbgOut( "shandler : GetVertPos : polymesh not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = pm->GetPointBuf( vertno, vpos );
		if( ret ){
			DbgOut( "shdhandler : GetVertPos : pm GetPointBuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SHDPOLYMESH2:
		pm2 = selem->polymesh2;
		if( !pm2 ){
			DbgOut( "shandler : GetVertPos : polymesh2 not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = pm2->GetOptPos( vertno, vpos );
		if( ret ){
			DbgOut( "shandler : GetVertPos : pm2 GetOptPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		break;
	default:
		DbgOut( "shandler : GetVertPos : this elem has not 3d object error !!!\n" );
		_ASSERT( 0 );
		return 1;

		break;
	}
***/	
	//DbgOut( "shdhandler : GetVertPos 2 : %f %f %f\n", vpos->x, vpos->y, vpos->z );

	return 0;
}
int CShdHandler::MagnetPosition( float dist )
{
	int partno;

	int* dirtyflag;
	
	for( partno = 0; partno < s2shd_leng; partno++ ){
		CShdElem* selem;
		selem = (*this)( partno );		


		int elemtype;
		elemtype = selem->type;
		if( (elemtype != SHDPOLYMESH) && (elemtype != SHDPOLYMESH2) )
			continue;
		
		int vertno, cmpvno;
		int vertnum;
		D3DXVECTOR3 srcpos, cmppos, diffvec;
		float distmag;
		
		VEC3F* srcptr;
		VEC3F* cmpptr;

		CPolyMesh* pm;
		CPolyMesh2* pm2;

		switch( elemtype ){
		case SHDPOLYMESH:
			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "shandler : SetWallOnGround : polymesh not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			vertnum = pm->meshinfo->m;//!!!
			dirtyflag = (int*)malloc( sizeof( int ) * vertnum );
			ZeroMemory( dirtyflag, sizeof( int ) * vertnum );


			for( vertno = 0; vertno < vertnum; vertno++ ){
				srcptr = pm->pointbuf + vertno;
				srcpos.x = srcptr->x;
				srcpos.y = srcptr->y;
				srcpos.z = srcptr->z;
				
				for( cmpvno = 0; cmpvno < vertnum; cmpvno++ ){
					if( (vertno != cmpvno) && (*(dirtyflag + cmpvno) == 0) ){
						cmpptr = pm->pointbuf + cmpvno;
						cmppos.x = cmpptr->x;
						cmppos.y = cmpptr->y;
						cmppos.z = cmpptr->z;

						diffvec = srcpos - cmppos;
						distmag = diffvec.x * diffvec.x + diffvec.y * diffvec.y + diffvec.z * diffvec.z;

						if( distmag < (dist * dist) ){
							*cmpptr = *srcptr;
							*( dirtyflag + cmpvno ) = 1;
						}
					}
				}
				*( dirtyflag + vertno ) = 1;

			}
			free( dirtyflag );
			break;
		case SHDPOLYMESH2:
			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "shandler : SetWallOnGround : polymesh2 not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			vertnum = pm2->meshinfo->m;
			dirtyflag = (int*)malloc( sizeof( int ) * vertnum );
			ZeroMemory( dirtyflag, sizeof( int ) * vertnum );


			for( vertno = 0; vertno < vertnum; vertno++ ){
				srcptr = pm2->pointbuf + vertno;
				srcpos.x = srcptr->x;
				srcpos.y = srcptr->y;
				srcpos.z = srcptr->z;
				
				for( cmpvno = 0; cmpvno < vertnum; cmpvno++ ){
					if( (vertno != cmpvno) && (*(dirtyflag + cmpvno) == 0) ){
						cmpptr = pm2->pointbuf + cmpvno;
						cmppos.x = cmpptr->x;
						cmppos.y = cmpptr->y;
						cmppos.z = cmpptr->z;

						diffvec = srcpos - cmppos;
						distmag = diffvec.x * diffvec.x + diffvec.y * diffvec.y + diffvec.z * diffvec.z;

						if( distmag < (dist * dist) ){
							*cmpptr = *srcptr;
							*( dirtyflag + cmpvno ) = 1;
						}
					}
				}
				*( dirtyflag + vertno ) = 1;

			}
			free( dirtyflag );
			break;
		default:
			DbgOut( "shandler : SetWallOnGround : this elem has not 3d object error !!!\n" );
			_ASSERT( 0 );
			return 1;

			break;
		}

	}


	return 0;
}

int CShdHandler::SetWallOnGround( D3DXMATRIX* wallmatWorld, D3DXMATRIX* gmatWorld, CShdHandler* gsh, CMotHandler* gmh, float mapmaxy, float mapminy, float wheight )
{
	int notoncnt = 0;

	int partno;
	int ret;
	
	for( partno = 0; partno < s2shd_leng; partno++ ){
		CShdElem* selem;
		selem = (*this)( partno );


		int elemtype;
		elemtype = selem->type;
		if( (elemtype != SHDPOLYMESH) && (elemtype != SHDPOLYMESH2) )
			continue;
		
		int vertno;
		int vertnum;


		CPolyMesh* pm;
		CPolyMesh2* pm2;

		switch( elemtype ){
		case SHDPOLYMESH:
			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "shandler : SetWallOnGround : polymesh not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			vertnum = pm->meshinfo->m;//!!!

			for( vertno = 0; vertno < vertnum; vertno++ ){
				D3DXVECTOR3 srcpos;
				VEC3F* srcptr;

				srcptr = pm->pointbuf + vertno;
				srcpos.x = srcptr->x;
				srcpos.y = srcptr->y;
				srcpos.z = srcptr->z;


				D3DXVECTOR3 uppos, downpos, newpos, nv;
				uppos.x = srcpos.x;
				uppos.y = mapmaxy;
				uppos.z = srcpos.z;
				downpos.x = srcpos.x;
				downpos.y = mapminy;
				downpos.z = srcpos.z;

				D3DXVECTOR3 upwpos, downwpos;
				D3DXVec3TransformCoord( &upwpos, &uppos, wallmatWorld );
				D3DXVec3TransformCoord( &downwpos, &downpos, wallmatWorld );

				int result;
				ret = gsh->ChkConfGround( 0, 0, gmatWorld, upwpos, downwpos, gmh, 0, 200.0f, mapminy, &result, &newpos, &nv );
				if( ret ){
					DbgOut( "shdhandler : SetWallOnGround : pm : gsh ChkConfGround error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( result ){
					D3DXMATRIX invw;
					D3DXVECTOR3 neworgpos;
					D3DXMatrixInverse( &invw, NULL, wallmatWorld );
					D3DXVec3TransformCoord( &neworgpos, &newpos, &invw );

					if( srcpos.y <= 0.0f ){
						srcptr->y = neworgpos.y - 500.0f;
					}else{
						srcptr->y = neworgpos.y + wheight;
					}
				}else{
					//そのまま
					_ASSERT( 0 );
					DbgOut( "shandler : SetWallOnGround : Not On Ground %f %f %f\n", srcptr->x, srcptr->y, srcptr->z );
					notoncnt++;
				}

			}
			break;
		case SHDPOLYMESH2:
			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "shandler : SetWallOnGround : polymesh2 not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			vertnum = pm2->meshinfo->m;
			for( vertno = 0; vertno < vertnum; vertno++ ){
				D3DXVECTOR3 srcpos;

				VEC3F* srcptr;
				srcptr = pm2->pointbuf + vertno;
				srcpos.x = srcptr->x;
				srcpos.y = srcptr->y;
				srcpos.z = srcptr->z;


				D3DXVECTOR3 uppos, downpos, newpos, nv;
				uppos.x = srcpos.x;
				uppos.y = mapmaxy;
				uppos.z = srcpos.z;
				downpos.x = srcpos.x;
				downpos.y = mapminy;
				downpos.z = srcpos.z;

				D3DXVECTOR3 upwpos, downwpos;
				D3DXVec3TransformCoord( &upwpos, &uppos, wallmatWorld );
				D3DXVec3TransformCoord( &downwpos, &downpos, wallmatWorld );

				int result;
				ret = gsh->ChkConfGround( 0, 0, gmatWorld, upwpos, downwpos, gmh, 0, 200.0f, mapminy, &result, &newpos, &nv );
				if( ret ){
					DbgOut( "shdhandler : SetWallOnGround : pm : gsh ChkConfGround error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( result ){
					D3DXMATRIX invw;
					D3DXVECTOR3 neworgpos;
					D3DXMatrixInverse( &invw, NULL, wallmatWorld );
					D3DXVec3TransformCoord( &neworgpos, &newpos, &invw );

					if( srcpos.y <= 0.0f ){
						srcptr->y = neworgpos.y - 500.0f;
					}else{
						srcptr->y = neworgpos.y + wheight;
					}
				}else{
					//そのまま
					_ASSERT( 0 );
					DbgOut( "shandler : SetWallOnGround : pm2 : Not On Ground %f %f %f\n", srcptr->x, srcptr->y, srcptr->z );
					notoncnt++;

				}
			}			
			break;
		default:
			DbgOut( "shandler : SetWallOnGround : this elem has not 3d object error !!!\n" );
			_ASSERT( 0 );
			return 1;

			break;
		}


	}

	DbgOut( "shandler : SetWallOnGround : notoncnt %d\n", notoncnt );

	return 0;
}

int CShdHandler::SetVertPos2InfScope( int isindex, CMotHandler* lpmh, int motid, int frameno, int vertno, D3DXVECTOR3 targetobj )
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// targetobjは、ボーン変形を考慮した、ローカル座標！！！！！
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if( motid >= lpmh->m_kindnum ){
		DbgOut( "shandler : SetVertPos2InfScope : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid >= 0) && ( (frameno < 0) || (frameno >= *(lpmh->m_motnum + motid)) ) ){
		DbgOut( "shandler : SetVertPos2InfScope : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( vertno < 0 ){
		DbgOut( "shandler : SetVertPos2InfScope : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////

	int ret;

	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}

	if( (isindex < 0) || (isindex >= iselem->scopenum) ){
		DbgOut( "sh : SetVertPos2InfScope : index error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CInfScope* curis;
	curis = *( iselem->ppscope + isindex );
	_ASSERT( curis );

	CPolyMesh* pm;
	CPolyMesh2* pm2;
	CInfElem* ieptr;
	switch( curis->type ){
	case SHDPOLYMESH:
		pm = curis->polymesh;
		_ASSERT( pm );
		if( vertno >= pm->meshinfo->m ){
			_ASSERT( 0 );
			return 1;
		}
		ieptr = pm->m_IE;
		break;
	case SHDPOLYMESH2:
		pm2 = curis->polymesh2;
		_ASSERT( pm2 );
		if( vertno >= pm2->optpleng ){
			_ASSERT( 0 );
			return 1;
		}
		ieptr = pm2->m_IE;
		break;
	default:
		_ASSERT( 0 );
		return 1;
		break;
	}
	_ASSERT( ieptr );


	D3DXVECTOR3 orgpos;

	if( motid >= 0 ){

		D3DXMATRIX blendmat;
		ZeroMemory( &blendmat, sizeof( D3DXMATRIX ) );
			
		int infno;
		for( infno = 0; infno < (ieptr + vertno)->infnum; infno++ ){
			D3DXMATRIX curmat;
			INFELEM* curIE;
			curIE = (ieptr + vertno)->ie + infno;

			int matno;
			matno = curIE->bonematno;
			if( matno > 0 ){
				ret = lpmh->HuGetBoneMatrix( matno, motid, frameno, &curmat );
				if( ret ){
					DbgOut( "shandler : SetVertPos2 : mh HuGetBoneMatrix error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				D3DXMatrixIdentity( &curmat );
			}

			blendmat += curIE->dispinf * curmat;

		}


		D3DXMATRIX invblend;
		D3DXMatrixInverse( &invblend, NULL, &blendmat );

		D3DXVec3TransformCoord( &orgpos, &targetobj, &invblend );

	}else{
		orgpos = targetobj;
	}

	ret = SetVertPosInfScope( curis, lpmh, vertno, orgpos );
	if( ret ){
		DbgOut( "shandler : SetVertPos2 : SetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdHandler::SetVertPos2( CMotHandler* lpmh, int motid, int frameno, int partno, int vertno, D3DXVECTOR3 targetobj )
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// targetobjは、ボーン変形を考慮した、ローカル座標！！！！！
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : SetVertPos2 : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= lpmh->m_kindnum) ){
		DbgOut( "shandler : SetVertPos2 : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(lpmh->m_motnum + motid)) ){
		DbgOut( "shandler : SetVertPos2 : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( vertno < 0 ){
		DbgOut( "shandler : SetVertPos2 : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////

	int ret;
	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		CPolyMesh* pm;
		pm = selem->polymesh;
		if( vertno >= pm->meshinfo->m ){
			DbgOut( "shandler : SetVertPos2 : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = pm->m_IE + vertno;

	}else if( selem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2;
		pm2 = selem->polymesh2;
		if( vertno >= pm2->optpleng ){
			DbgOut( "shandler : SetVertPos2 : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ieptr = pm2->m_IE + vertno;

	}else{
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	_ASSERT( ieptr );


	D3DXMATRIX blendmat;
	ZeroMemory( &blendmat, sizeof( D3DXMATRIX ) );

	int infno;
	for( infno = 0; infno < ieptr->infnum; infno++ ){
		D3DXMATRIX curmat;
		INFELEM* curIE;
		curIE = ieptr->ie + infno;

		int matno;
		matno = curIE->bonematno;
		if( matno > 0 ){
			ret = lpmh->HuGetBoneMatrix( matno, motid, frameno, &curmat );
			if( ret ){
				DbgOut( "shandler : SetVertPos2 : mh HuGetBoneMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			D3DXMatrixIdentity( &curmat );
		}

		blendmat += curIE->dispinf * curmat;

	}
	if( ieptr->infnum == 0 ){
		D3DXMatrixIdentity( &blendmat );
	}


	D3DXMATRIX invblend;
	D3DXMatrixInverse( &invblend, NULL, &blendmat );

	D3DXVECTOR3 orgpos;
	D3DXVec3TransformCoord( &orgpos, &targetobj, &invblend );

	ret = SetVertPos( lpmh, partno, vertno, orgpos );
	if( ret ){
		DbgOut( "shandler : SetVertPos2 : SetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::SetVertPosInfScope( CInfScope* curis, CMotHandler* mhptr, int vertno, D3DXVECTOR3 vpos )
{
	int ret;

	CD3DDisp* d3ddispptr;
	d3ddispptr = curis->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetVertPosInfScope : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	if( (vertno < 0) || (vertno >= vertnum) ){
		DbgOut( "shdhandler : SetVertPosInfScope : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!		
	}

	
	_ASSERT( d3ddispptr->m_skinv );
	SKINVERTEX* skinv;
	skinv = d3ddispptr->m_skinv + vertno;
	skinv->pos[0] = vpos.x;
	skinv->pos[1] = vpos.y;
	skinv->pos[2] = vpos.z;
		
	int blno;
	for( blno = 0; blno < d3ddispptr->mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb = 0;			
		curvb = *( d3ddispptr->m_ArrayVB + blno );

		SKINVERTEX* pSkinv = 0;
		if( FAILED( curvb->Lock( 0, 0, (void**)&pSkinv, NULL ) ) ){
			_ASSERT( 0 );
			return 1;
		}			

		( pSkinv + vertno )->pos[0] = vpos.x;
		( pSkinv + vertno )->pos[1] = vpos.y;
		( pSkinv + vertno )->pos[2] = vpos.z;

		curvb->Unlock();
	}
	
	D3DXMATRIX iniworld;
	D3DXMatrixIdentity( &iniworld );


	CPolyMesh* pm;
	CPolyMesh2* pm2;

	switch( curis->type ){
	case SHDPOLYMESH:
		pm = curis->polymesh;
		if( !pm ){
			DbgOut( "shandler : SetVertPosInfScope : polymesh not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = pm->SetPointBuf( vertno, vpos.x, vpos.y, vpos.z, -999 );
		if( ret ){
			DbgOut( "shdhandler : SetVertPosInfScope : pm SetPointBuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SHDPOLYMESH2:
		pm2 = curis->polymesh2;
		if( !pm2 ){
			DbgOut( "shandler : SetVertPosInfScope : polymesh2 not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = pm2->SetOptPos( vertno, vpos );
		if( ret ){
			DbgOut( "shandler : SetVertPosInfScope : pm2 SetOptPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		break;
	default:
		DbgOut( "shandler : SetVertPosInfScope : this elem has not 3d object error !!!\n" );
		_ASSERT( 0 );
		return 1;

		break;
	}

	return 0;

}

int CShdHandler::SetVertPosBatchDouble( CMotHandler* mhptr, int partno, int* vnoarray, int vnum, double* varray, int aorder )
{
	// HSPの２次元配列（並び方が違う）

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : SetVertPos : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetVertPos : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	D3DXMATRIX iniworld;
	D3DXMatrixIdentity( &iniworld );


	int i;
	int vertno;
	for( i = 0; i < vnum; i++ ){
		vertno = *( vnoarray + i );


		if( (vertno < 0) || (vertno >= vertnum) ){
			DbgOut( "shdhandler : SetVertPos : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;//!!!		
		}


		D3DXVECTOR3 vpos;
		if( aorder == ORDER_HSP ){	
			// HSPの２次元配列（並び方が違う）
			vpos.x = (float)*( varray + 0 * vnum + i );
			vpos.y = (float)*( varray + 1 * vnum + i );
			vpos.z = (float)*( varray + 2 * vnum + i );
		}else{
			vpos.x = (float)(*( varray + i * 3 ));
			vpos.y = (float)(*( varray + i * 3 + 1 ));
			vpos.z = (float)(*( varray + i * 3 + 2 ));
		}



		( d3ddispptr->m_skinv + vertno )->pos[0] = vpos.x;
		( d3ddispptr->m_skinv + vertno )->pos[1] = vpos.y;
		( d3ddispptr->m_skinv + vertno )->pos[2] = vpos.z;

/////////////////
		CPolyMesh* pm;
		CPolyMesh2* pm2;

		int elemtype;
		elemtype = selem->type;
		switch( elemtype ){
		case SHDPOLYMESH:
			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "shandler : SetVertPos : polymesh not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			ret = pm->SetPointBuf( vertno, vpos.x, vpos.y, vpos.z, -999 );
			if( ret ){
				DbgOut( "shdhandler : SetVertPos : pm SetPointBuf error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

//			if( pm->groundflag ){
//				ret = d3ddispptr->SetGroundObj2( mhptr, iniworld, vertno );
//				if( ret ){
//					DbgOut( "shdhandler : SetVertPos : d3ddisp SetGroundObj error !!!\n" );
//					_ASSERT( 0 );
//					return 1;
//				}
//			}


			break;
		case SHDPOLYMESH2:
			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "shandler : SetVertPos : polymesh2 not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm2->SetOptPos( vertno, vpos );
			if( ret ){
				DbgOut( "shandler : SetVertPos : pm2 SetOptPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

//			if( pm2->groundflag ){
//				ret = d3ddispptr->SetGroundObj2( mhptr, iniworld, vertno );
//				if( ret ){
//					DbgOut( "shdhandler : SetVertPos : d3ddisp SetGroundObj error !!!\n" );
//					_ASSERT( 0 );
//					return 1;
//				}
//			}
			
			
			break;
		default:
			DbgOut( "shandler : SetVertPos : this elem has not 3d object error !!!\n" );
			_ASSERT( 0 );
			return 1;

			break;
		}
	}

	int blno;
	for( blno = 0; blno < d3ddispptr->mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb = 0;
		LPDIRECT3DVERTEXBUFFER9 currevvb = 0;
			
		curvb = *( d3ddispptr->m_ArrayVB + blno );
		if( d3ddispptr->m_revArrayVB ){
			currevvb = *( d3ddispptr->m_revArrayVB + blno );
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


		int i2, vertno2;
		for( i2 = 0; i2 < vnum; i2++ ){
			vertno2 = *( vnoarray + i2 );
			if( (vertno2 >= 0) && (vertno2 < vertnum) ){

				D3DXVECTOR3 vpos2;
				if( aorder == ORDER_HSP ){	
					// HSPの２次元配列（並び方が違う）
					vpos2.x = (float)*( varray + 0 * vnum + i2 );
					vpos2.y = (float)*( varray + 1 * vnum + i2 );
					vpos2.z = (float)*( varray + 2 * vnum + i2 );
				}else{
					vpos2.x = (float)(*( varray + i2 * 3 ));
					vpos2.y = (float)(*( varray + i2 * 3 + 1 ));
					vpos2.z = (float)(*( varray + i2 * 3 + 2 ));
				}


				(pSkinv + vertno2)->pos[0] = vpos2.x;
				(pSkinv + vertno2)->pos[1] = vpos2.y;
				(pSkinv + vertno2)->pos[2] = vpos2.z;

				if( prevSkinv ){
					(prevSkinv + vertno2)->pos[0] = vpos2.x;
					(prevSkinv + vertno2)->pos[1] = vpos2.y;
					(prevSkinv + vertno2)->pos[2] = vpos2.z;
				}
			}
		}

		curvb->Unlock();
		if( currevvb ){
			currevvb->Unlock();
		}
	}
		
	/***
	//BSphere　curbs のセット
	CBSphere* dsttotalbs = &(mhptr->m_curbs);

	
	//まず、パーツの中心と半径を求める。
	m_mc.InitCache( &iniworld );

	ret = selem->TransformOnlyWorld( &m_mc, mhptr, -1, 0, iniworld, CALC_CENTER_FLAG, dsttotalbs );
	if( ret ){
		DbgOut( "shandler : SetVertPos : TransformOnlyWorld : CALC_CENTER error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//オブジェクト全体の中心を求める。
		
		//!!!!!!!!!!!!!!!!!!!!!!!
		//省略　変化しないとみなす。
		//!!!!!!!!!!!!!!!!!!!!!!!


	//オブジェクト全体の半径を求める。
		//もし、変更したパーツにより、半径が大きくなるようなら、変更、小さくなる場合は、関知しない。


	CBSphere* srcbs;
	srcbs = &(selem->curbs);
	
	float cmpdist;
	float diffx, diffy, diffz;
	diffx = dsttotalbs->befcenter.x - srcbs->befcenter.x;
	diffy = dsttotalbs->befcenter.y - srcbs->befcenter.y;
	diffz = dsttotalbs->befcenter.z - srcbs->befcenter.z;

	cmpdist = (float)sqrt( diffx * diffx + diffy * diffy + diffz * diffz ) + srcbs->rmag;

	if( cmpdist > dsttotalbs->rmag )
		dsttotalbs->rmag = cmpdist;
	***/

	return 0;

}
int CShdHandler::SetVertPosBatchVec3( CMotHandler* mhptr, int partno, int* vnoarray, int vnum, D3DXVECTOR3* varray )
{
	// HSPの２次元配列（並び方が違う）

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : SetVertPos : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetVertPos : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	D3DXMATRIX iniworld;
	D3DXMatrixIdentity( &iniworld );


	int i;
	int vertno;
	for( i = 0; i < vnum; i++ ){
		vertno = *( vnoarray + i );


		if( (vertno < 0) || (vertno >= vertnum) ){
			DbgOut( "shdhandler : SetVertPos : vertno error !!!\n" );
			_ASSERT( 0 );
			return 1;//!!!		
		}


		D3DXVECTOR3 vpos = *( varray + i );

		( d3ddispptr->m_skinv + vertno )->pos[0] = vpos.x;
		( d3ddispptr->m_skinv + vertno )->pos[1] = vpos.y;
		( d3ddispptr->m_skinv + vertno )->pos[2] = vpos.z;

/////////////////
		CPolyMesh* pm;
		CPolyMesh2* pm2;

		int elemtype;
		elemtype = selem->type;
		switch( elemtype ){
		case SHDPOLYMESH:
			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "shandler : SetVertPos : polymesh not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			ret = pm->SetPointBuf( vertno, vpos.x, vpos.y, vpos.z, -999 );
			if( ret ){
				DbgOut( "shdhandler : SetVertPos : pm SetPointBuf error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

//			if( pm->groundflag ){
//				ret = d3ddispptr->SetGroundObj2( mhptr, iniworld, vertno );
//				if( ret ){
//					DbgOut( "shdhandler : SetVertPos : d3ddisp SetGroundObj error !!!\n" );
//					_ASSERT( 0 );
//					return 1;
//				}
//			}


			break;
		case SHDPOLYMESH2:
			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "shandler : SetVertPos : polymesh2 not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm2->SetOptPos( vertno, vpos );
			if( ret ){
				DbgOut( "shandler : SetVertPos : pm2 SetOptPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

//			if( pm2->groundflag ){
//				ret = d3ddispptr->SetGroundObj2( mhptr, iniworld, vertno );
//				if( ret ){
//					DbgOut( "shdhandler : SetVertPos : d3ddisp SetGroundObj error !!!\n" );
//					_ASSERT( 0 );
//					return 1;
//				}
//			}
			
			
			break;
		default:
			DbgOut( "shandler : SetVertPos : this elem has not 3d object error !!!\n" );
			_ASSERT( 0 );
			return 1;

			break;
		}
	}

	int blno;
	for( blno = 0; blno < d3ddispptr->mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb = 0;
		LPDIRECT3DVERTEXBUFFER9 currevvb = 0;
			
		curvb = *( d3ddispptr->m_ArrayVB + blno );
		if( d3ddispptr->m_revArrayVB ){
			currevvb = *( d3ddispptr->m_revArrayVB + blno );
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


		int i2, vertno2;
		for( i2 = 0; i2 < vnum; i2++ ){
			vertno2 = *( vnoarray + i2 );
			if( (vertno2 >= 0) && (vertno2 < vertnum) ){

				D3DXVECTOR3 vpos2 = *( varray + i2 );

				(pSkinv + vertno2)->pos[0] = vpos2.x;
				(pSkinv + vertno2)->pos[1] = vpos2.y;
				(pSkinv + vertno2)->pos[2] = vpos2.z;

				if( prevSkinv ){
					(prevSkinv + vertno2)->pos[0] = vpos2.x;
					(prevSkinv + vertno2)->pos[1] = vpos2.y;
					(prevSkinv + vertno2)->pos[2] = vpos2.z;
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

int CShdHandler::SetVertPos( CMotHandler* mhptr, int partno, int vertno, D3DXVECTOR3 vpos )
{

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : SetVertPos : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetVertPos : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	if( (vertno < 0) || (vertno >= vertnum) ){
		DbgOut( "shdhandler : SetVertPos : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!		
	}

	
	( d3ddispptr->m_skinv + vertno )->pos[0] = vpos.x;
	( d3ddispptr->m_skinv + vertno )->pos[1] = vpos.y;
	( d3ddispptr->m_skinv + vertno )->pos[2] = vpos.z;

	int blno;
	for( blno = 0; blno < d3ddispptr->mskinblocknum; blno++ ){

		LPDIRECT3DVERTEXBUFFER9 curvb = 0;
		LPDIRECT3DVERTEXBUFFER9 currevvb = 0;
			
		curvb = *( d3ddispptr->m_ArrayVB + blno );
		if( d3ddispptr->m_revArrayVB ){
			currevvb = *( d3ddispptr->m_revArrayVB + blno );
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

		(pSkinv + vertno)->pos[0] = vpos.x;
		(pSkinv + vertno)->pos[1] = vpos.y;
		(pSkinv + vertno)->pos[2] = vpos.z;

		if( prevSkinv ){
			(prevSkinv + vertno)->pos[0] = vpos.x;
			(prevSkinv + vertno)->pos[1] = vpos.y;
			(prevSkinv + vertno)->pos[2] = vpos.z;
		}

		curvb->Unlock();
		if( currevvb ){
			currevvb->Unlock();
		}
	}
		
	

	D3DXMATRIX iniworld;
	D3DXMatrixIdentity( &iniworld );


	CPolyMesh* pm;
	CPolyMesh2* pm2;

	int elemtype;
	elemtype = selem->type;
	switch( elemtype ){
	case SHDPOLYMESH:
		pm = selem->polymesh;
		if( !pm ){
			DbgOut( "shandler : SetVertPos : polymesh not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = pm->SetPointBuf( vertno, vpos.x, vpos.y, vpos.z, -999 );
		if( ret ){
			DbgOut( "shdhandler : SetVertPos : pm SetPointBuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( pm->groundflag ){
			ret = d3ddispptr->SetGroundObj2( mhptr, iniworld, vertno );
			if( ret ){
				DbgOut( "shdhandler : SetVertPos : d3ddisp SetGroundObj error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


		break;
	case SHDPOLYMESH2:
		pm2 = selem->polymesh2;
		if( !pm2 ){
			DbgOut( "shandler : SetVertPos : polymesh2 not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = pm2->SetOptPos( vertno, vpos );
		if( ret ){
			DbgOut( "shandler : SetVertPos : pm2 SetOptPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( pm2->groundflag ){
			ret = d3ddispptr->SetGroundObj2( mhptr, iniworld, vertno );
			if( ret ){
				DbgOut( "shdhandler : SetVertPos : d3ddisp SetGroundObj error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		
		
		break;
	default:
		DbgOut( "shandler : SetVertPos : this elem has not 3d object error !!!\n" );
		_ASSERT( 0 );
		return 1;

		break;
	}


	//BSphere　curbs のセット
	CBSphere* dsttotalbs = &(mhptr->m_curbs);

	
	//まず、パーツの中心と半径を求める。
	m_mc.InitCache( &iniworld );

	ret = selem->TransformOnlyWorld( &m_mc, mhptr, -1, 0, iniworld, CALC_CENTER_FLAG, dsttotalbs );
	if( ret ){
		DbgOut( "shandler : SetVertPos : TransformOnlyWorld : CALC_CENTER error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//オブジェクト全体の中心を求める。
		
		//!!!!!!!!!!!!!!!!!!!!!!!
		//省略　変化しないとみなす。
		//!!!!!!!!!!!!!!!!!!!!!!!


	//オブジェクト全体の半径を求める。
		//もし、変更したパーツにより、半径が大きくなるようなら、変更、小さくなる場合は、関知しない。


	CBSphere* srcbs;
	srcbs = &(selem->curbs);
	
	float cmpdist;
	float diffx, diffy, diffz;
	diffx = dsttotalbs->befcenter.x - srcbs->befcenter.x;
	diffy = dsttotalbs->befcenter.y - srcbs->befcenter.y;
	diffz = dsttotalbs->befcenter.z - srcbs->befcenter.z;

	cmpdist = (float)sqrt( diffx * diffx + diffy * diffy + diffz * diffz ) + srcbs->rmag;

	if( cmpdist > dsttotalbs->rmag )
		dsttotalbs->rmag = cmpdist;


	/***
	CBSphere* srcbs;
	srcbs = &(selem->curbs);

	if( srcbs->totalrmag > dsttotalbs->rmag ){
		dsttotalbs->rmag = srcbs->totalrmag;
	}
	
	dsttotalbs->totalrmag = dsttotalbs->rmag;
	***/


//DbgOut( "shdhandler : SetVertPos : %f %f %f\n", vpos.x, vpos.y, vpos.z );

	return 0;
}

int CShdHandler::SortBillboard( D3DXVECTOR3 vDir )
{
	int ret;
	CBillboard* bb;
	CShdElem* bbselem;
	bbselem = (*this)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "sh : SortBillboard : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bb = bbselem->billboard;
	if( bb ){
		ret = bb->SortElem( vDir );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::SetBillboardPos( int bbid, float posx, float posy, float posz )
{
	int ret;
	CBillboard* bb;
	CShdElem* bbselem;
	bbselem = (*this)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "sh : SetBillboardPos : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bb = bbselem->billboard;
	if( bb ){
		ret = bb->SetBillboardPos( bbid, posx, posy, posz );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::RotateBillboard( int bbid, float fdeg, int rotkind )
{
	int ret;
	CBillboard* bb;
	CShdElem* bbselem;
	bbselem = (*this)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "sh : RotateBillboard : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bb = bbselem->billboard;
	if( bb ){
		ret = bb->RotateBillboard( bbid, fdeg, rotkind );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::SetBillboardSize( int bbid, float width, float height, int dirmode, int orgflag )
{
	int ret;
	CBillboard* bb;
	CShdElem* bbselem;
	bbselem = (*this)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "sh : SetBillboardPos : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bb = bbselem->billboard;
	if( bb ){
		ret = bb->SetBillboardSize( bbid, width, height, dirmode, orgflag );
		if( ret ){
			DbgOut( "sh : SetBillboardSize : bb SetBillboardSize error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::SetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag )
{
	int ret;
	CBillboard* bb;
	CShdElem* bbselem;
	bbselem = (*this)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "sh : SetBillboardUV : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bb = bbselem->billboard;
	if( bb ){
		ret = bb->SetBillboardUV( bbid, unum, vnum, texno, revuflag );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::SetBillboardDispFlag( int bbid, int flag )
{

	int ret;
	CBillboard* bb;
	CShdElem* bbselem;
	bbselem = (*this)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "sh : SetBillboardDispFlag : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bb = bbselem->billboard;
	if( bb ){
		ret = bb->SetBillboardDispFlag( bbid, flag );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}		
	}

	return 0;
}
int CShdHandler::DestroyBillboard( int bbid, CTreeHandler2* lpth, CMotHandler* lpmh )
{
	int ret;
	CBillboard* bb;
	CShdElem* bbselem;
	bbselem = (*this)( 1 );
	if( (bbselem->type != SHDBILLBOARD) || !bbselem->billboard ){
		DbgOut( "sh : DestroyBillboard : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	bb = bbselem->billboard;
	if( bb ){
		ret = bb->DestroyBillboard( bbid, lpth, lpmh );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CShdHandler::SetValidFlag( int srcseri, int srcflag )
{


	int i;
	int etype;
	CShdElem* selem;

	int isjoint;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			isjoint = selem->IsJoint();

			if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
				(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
				(etype == SHDPOLYGON) || 
				(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) ||
				isjoint ){

				selem->notuse = !srcflag;			
			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetValidFlag : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetValidFlag : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;
		isjoint = selem->IsJoint();


		if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			(etype == SHDPOLYGON) || 
			(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) ||
			isjoint ){

			selem->notuse = !srcflag;
		}
	}



	return 0;
}

int CShdHandler::GetDiffuse( int srcseri, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;
	int etype;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shdhandler : GetDiffuse : srcseri out of range warning !!!\n" );
		_ASSERT( 0 );
		return 0;//
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	if( !selem ){
		DbgOut( "shdhandler : GetDiffuse : selem NULL warning !!!\n" );
		_ASSERT( 0 );
		return 0;//			
	}

	etype = selem->type;

	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
	//	(etype == SHDPOLYGON) || 
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
		ret = selem->GetDiffuse( vertno, rptr, gptr, bptr );
		if( ret ){
			DbgOut( "shandler : GetDiffuse : selem GetDiffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CShdHandler::GetAmbient( int srcseri, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;
	int etype;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shdhandler : GetAmbient : srcseri out of range warning !!!\n" );
		_ASSERT( 0 );
		return 0;//
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	if( !selem ){
		DbgOut( "shdhandler : GetAmbient : selem NULL warning !!!\n" );
		_ASSERT( 0 );
		return 0;//			
	}

	etype = selem->type;

	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
	//	(etype == SHDPOLYGON) || 
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
		ret = selem->GetAmbient( vertno, rptr, gptr, bptr );
		if( ret ){
			DbgOut( "shandler : GetAmbient : selem GetAmbient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::GetSpecularPower( int srcseri, int vertno, float* powerptr )
{
	int ret;
	int etype;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shdhandler : GetSpecularPower : srcseri out of range warning !!!\n" );
		_ASSERT( 0 );
		return 0;//
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	if( !selem ){
		DbgOut( "shdhandler : GetSpecularPower : selem NULL warning !!!\n" );
		_ASSERT( 0 );
		return 0;//			
	}

	etype = selem->type;

	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
	//	(etype == SHDPOLYGON) || 
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
		ret = selem->GetSpecularPower( vertno, powerptr );
		if( ret ){
			DbgOut( "shandler : GetSpecularPower : selem GetSpecularPower error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CShdHandler::GetEmissive( int srcseri, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;
	int etype;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shdhandler : GetEmissive : srcseri out of range warning !!!\n" );
		_ASSERT( 0 );
		return 0;//
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	if( !selem ){
		DbgOut( "shdhandler : GetEmissive : selem NULL warning !!!\n" );
		_ASSERT( 0 );
		return 0;//			
	}

	etype = selem->type;

	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
	//	(etype == SHDPOLYGON) || 
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
		ret = selem->GetEmissive( vertno, rptr, gptr, bptr );
		if( ret ){
			DbgOut( "shandler : GetSpecular : selem GetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CShdHandler::GetSpecular( int srcseri, int vertno, int* rptr, int* gptr, int* bptr )
{
	int ret;
	int etype;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shdhandler : GetSpecular : srcseri out of range warning !!!\n" );
		_ASSERT( 0 );
		return 0;//
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	if( !selem ){
		DbgOut( "shdhandler : GetSpecular : selem NULL warning !!!\n" );
		_ASSERT( 0 );
		return 0;//			
	}

	etype = selem->type;

	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
	//	(etype == SHDPOLYGON) || 
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
		ret = selem->GetSpecular( vertno, rptr, gptr, bptr );
		if( ret ){
			DbgOut( "shandler : GetSpecular : selem GetSpecular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::GetAlphaF( int srcseri, int vertno, float* aptr )
{

	int etype;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shdhandler : GetAlpha : srcseri out of range warning !!!\n" );
		_ASSERT( 0 );
		return 0;//
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	if( !selem ){
		DbgOut( "shdhandler : GetAlpha : selem NULL warning !!!\n" );
		_ASSERT( 0 );
		return 0;//			
	}

	etype = selem->type;

	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
	//	(etype == SHDPOLYGON) || 
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
		*aptr = selem->alpha;
	}

	return 0;
}

int CShdHandler::GetAlpha( int srcseri, int vertno, int* aptr )
{

	int etype;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shdhandler : GetAlpha : srcseri out of range warning !!!\n" );
		_ASSERT( 0 );
		return 0;//
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	if( !selem ){
		DbgOut( "shdhandler : GetAlpha : selem NULL warning !!!\n" );
		_ASSERT( 0 );
		return 0;//			
	}

	etype = selem->type;

	//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
	//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
	//	(etype == SHDPOLYGON) || 
	//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){
		*aptr = (int)( selem->alpha * 255.0f );
	}

	return 0;
}

int CShdHandler::SetDispDiffuse( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno, int updateflag )
{

	int ret;
	int i;
	int etype;
	CShdElem* selem;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			//	(etype == SHDPOLYGON) || 
			//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

				ret = selem->SetDispDiffuse( srcr, srcg, srcb, setflag, m_TLmode, -1, updateflag );
				if( ret ){
					DbgOut( "shandler : SetDiffuse : selem SetRGB error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}

	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetDispDiffuse : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetDispDiffuse : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
		//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
		//	(etype == SHDPOLYGON) || 
		//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			ret = selem->SetDispDiffuse( srcr, srcg, srcb, setflag, m_TLmode, vertno, updateflag );
			if( ret ){
				DbgOut( "shandler : SetDiffuse : selem SetRGB error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}



	return 0;
}

int CShdHandler::SetDispEmissive( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno )
{
	int ret;
	int i;
	int etype;
	CShdElem* selem;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			//	(etype == SHDPOLYGON) || 
			//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

				ret = selem->SetDispEmissive( srcr, srcg, srcb, setflag, m_TLmode, -1 );
				if( ret ){
					DbgOut( "shandler : SetEmissive : selem SetEmissive error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetDispEmissive : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetDispEmissive : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
		//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
		//	(etype == SHDPOLYGON) || 
		//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			ret = selem->SetDispEmissive( srcr, srcg, srcb, setflag, m_TLmode, vertno );
			if( ret ){
				DbgOut( "shandler : SetEmissive : selem SetEmissive error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}



	return 0;
}
int CShdHandler::SetDispSpecularPower( int srcseri, float srcpow, int setflag, int vertno )
{
	int ret;
	int i;
	int etype;
	CShdElem* selem;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			//	(etype == SHDPOLYGON) || 
			//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

				ret = selem->SetDispSpecularPower( srcpow, setflag, m_TLmode, -1 );
				if( ret ){
					DbgOut( "shandler : SetSpecularPower : selem SetSpecularPower error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetDispSpecularPower : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetDispSpecularPower : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
		//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
		//	(etype == SHDPOLYGON) || 
		//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			ret = selem->SetDispSpecularPower( srcpow, setflag, m_TLmode, vertno );
			if( ret ){
				DbgOut( "shandler : SetSpecularPower : selem SetSpecularPower error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;

}


int CShdHandler::SetDispSpecular( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno )
{

	int ret;
	int i;
	int etype;
	CShdElem* selem;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			//	(etype == SHDPOLYGON) || 
			//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

				ret = selem->SetDispSpecular( srcr, srcg, srcb, setflag, m_TLmode, -1 );
				if( ret ){
					DbgOut( "shandler : SetSpecular : selem SetSpecular error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetDispSpecular : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetDispSpecular : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
		//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
		//	(etype == SHDPOLYGON) || 
		//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			ret = selem->SetDispSpecular( srcr, srcg, srcb, setflag, m_TLmode, vertno );
			if( ret ){
				DbgOut( "shandler : SetSpecular : selem SetSpecular error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}



	return 0;
}

int CShdHandler::SetDispAmbient( int srcseri, int srcr, int srcg, int srcb, int setflag, int vertno )
{

	int ret;
	int i;
	int etype;
	CShdElem* selem;

	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
			//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
			//	(etype == SHDPOLYGON) || 
			//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

				ret = selem->SetDispAmbient( srcr, srcg, srcb, setflag, m_TLmode, -1 );
				if( ret ){
					DbgOut( "shandler : SetAmbient : selem SetAmbient error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetDispAmbient : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetDispAmbient : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		//if( (etype == SHDBEZIERSURF) || (etype == SHDMESHES) || 
		//	(etype == SHDSPHERE) || (etype == SHDEXTRUDE) || 
		//	(etype == SHDPOLYGON) || 
		//	(etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) ){

			ret = selem->SetDispAmbient( srcr, srcg, srcb, setflag, m_TLmode, vertno );
			if( ret ){
				DbgOut( "shandler : SetAmbient : selem SetAmbient error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}



	return 0;
}

int CShdHandler::SetMaterialBlendingMode( int matno, int mode )
{

	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialBlendingMode( mqomat->materialno, mode );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}

		curmat->blendmode = mode;
	}
	return 0;

}

int CShdHandler::SetMaterialAlphaTest( int matno, int alphatest0, int alphaval0, int alphatest1, int alphaval1 )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialAlphaTest( mqomat->materialno, alphatest0, alphaval0, alphatest1, alphaval1 );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}

		curmat->alphatest0 = alphatest0;
		curmat->alphaval0 = alphaval0;

		curmat->alphatest1 = alphatest1;
		curmat->alphaval1 = alphaval1;
	}


	return 0;
}
int CShdHandler::GetMaterialAlphaTest( int materialno, int* test0ptr, int* val0ptr, int* test1ptr, int* val1ptr )
{



	if( !m_mathead ){
		return 1;//!!!!!!!!!!!!!!!!
	}

	CMQOMaterial* curmat;
	curmat = GetMaterialFromNo( m_mathead, materialno );
	if( !curmat ){
		return 1;//!!!!!!!!!!!!!!!!
	}

	*test0ptr = (int)curmat->alphatest0;
	*val0ptr = (int)curmat->alphaval0;

	*test1ptr = (int)curmat->alphatest1;
	*val1ptr = (int)curmat->alphaval1;

	return 0;
}


int CShdHandler::SetMaterialAlpha( int matno, float srcalpha )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialAlpha( mqomat->materialno, srcalpha );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}

		curmat->col.a = srcalpha;
		curmat->dif4f.a = srcalpha;
		curmat->orgalpha = srcalpha;

		ret = ChkAlphaNum();
		_ASSERT( !ret );
	}

	return ret;
}

int CShdHandler::Material2Color()
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	CMQOMaterial* curmat;
	curmat = m_mathead;

	int setflag = 0;
	int tmpr, tmpg, tmpb;

	while( curmat ){
		tmpr = (int)(curmat->dif4f.r * 255.0f);
		tmpg = (int)(curmat->dif4f.g * 255.0f);
		tmpb = (int)(curmat->dif4f.b * 255.0f);
		ret = SetMaterialDiffuse( curmat->materialno, setflag, tmpr, tmpg, tmpb );
		_ASSERT( !ret );

		tmpr = (int)(curmat->spc3f.r * 255.0f);
		tmpg = (int)(curmat->spc3f.g * 255.0f);
		tmpb = (int)(curmat->spc3f.b * 255.0f);
		ret = SetMaterialSpecular( curmat->materialno, setflag, tmpr, tmpg, tmpb );
		_ASSERT( !ret );

		tmpr = (int)(curmat->amb3f.r * 255.0f);
		tmpg = (int)(curmat->amb3f.g * 255.0f);
		tmpb = (int)(curmat->amb3f.b * 255.0f);
		ret = SetMaterialAmbient( curmat->materialno, setflag, tmpr, tmpg, tmpb );
		_ASSERT( !ret );

		tmpr = (int)(curmat->emi3f.r * 255.0f);
		tmpg = (int)(curmat->emi3f.g * 255.0f);
		tmpb = (int)(curmat->emi3f.b * 255.0f);
		ret = SetMaterialEmissive( curmat->materialno, setflag, tmpr, tmpg, tmpb );
		_ASSERT( !ret );

		ret = SetMaterialPower( curmat->materialno, setflag, curmat->power );
		_ASSERT( !ret );

		curmat = curmat->next;
	}


	return 0;

}


int CShdHandler::SetMaterialDiffuse( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialDiffuse( mqomat->materialno, setflag, srcr, srcg, srcb );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}
		curmat->dif4f.r = (float)srcr / 255.0f;
		curmat->dif4f.g = (float)srcg / 255.0f;
		curmat->dif4f.b = (float)srcb / 255.0f;


		int seri;
		CShdElem* selem;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			selem = (*this)( seri );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				ret = selem->SetMaterialDiffuse( matno, setflag, srcr, srcg, srcb );
				if( ret ){
					DbgOut( "sh : SetMaterialDiffuse : se SetMaterialDiffuse error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

	return 0;
}
int CShdHandler::SetMaterialSpecular( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialSpecular( mqomat->materialno, setflag, srcr, srcg, srcb );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}
		curmat->spc3f.r = (float)srcr / 255.0f;
		curmat->spc3f.g = (float)srcg / 255.0f;
		curmat->spc3f.b = (float)srcb / 255.0f;


		int seri;
		CShdElem* selem;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			selem = (*this)( seri );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				ret = selem->SetMaterialSpecular( matno, setflag, srcr, srcg, srcb );
				if( ret ){
					DbgOut( "sh : SetMaterialSpecular : se SetMaterialSpecular error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}
	return 0;
}
int CShdHandler::SetMaterialAmbient( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialAmbient( mqomat->materialno, setflag, srcr, srcg, srcb );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}
		curmat->amb3f.r = (float)srcr / 255.0f;
		curmat->amb3f.g = (float)srcg / 255.0f;
		curmat->amb3f.b = (float)srcb / 255.0f;

		int seri;
		CShdElem* selem;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			selem = (*this)( seri );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				ret = selem->SetMaterialAmbient( matno, setflag, srcr, srcg, srcb );
				if( ret ){
					DbgOut( "sh : SetMaterialAmbient : se SetMaterialAmbient error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}
	return 0;
}
int CShdHandler::SetMaterialEmissive( int matno, int setflag, int srcr, int srcg, int srcb )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialEmissive( mqomat->materialno, setflag, srcr, srcg, srcb );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}
		curmat->emi3f.r = (float)srcr / 255.0f;
		curmat->emi3f.g = (float)srcg / 255.0f;
		curmat->emi3f.b = (float)srcb / 255.0f;


		int seri;
		CShdElem* selem;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			selem = (*this)( seri );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				ret = selem->SetMaterialEmissive( matno, setflag, srcr, srcg, srcb );
				if( ret ){
					DbgOut( "sh : SetMaterialEmissive : se SetMaterialEmissive error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}
	return 0;
}
int CShdHandler::SetMaterialPower( int matno, int setflag, float srcpow )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialPower( mqomat->materialno, setflag, srcpow );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}
		curmat->power = srcpow;


		int seri;
		CShdElem* selem;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			selem = (*this)( seri );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				ret = selem->SetMaterialPower( matno, setflag, srcpow );
				if( ret ){
					DbgOut( "sh : SetMaterialPower : se SetMaterialPower error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}
	return 0;
}



int CShdHandler::SetDispScale( int srcseri, D3DXVECTOR3 scalevec, int centerflag, D3DXMATRIX matWorld )
{
	int ret;
	int i;
	int etype;
	CShdElem* selem;

	D3DXVECTOR3 centervec;



	if( srcseri == -1 ){
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			etype = selem->type;

			int isbone;
			isbone = (selem->IsJoint() && (etype != SHDMORPH));

			if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) || isbone || (etype == SHDMORPH) ){
				//E3DGetBonePosで、ボーンのスケールも必要！！

				int isbillboard = 0;
				if( (etype == SHDPOLYMESH) && selem->polymesh && selem->polymesh->billboardflag ){
					isbillboard = 1;
				}


				D3DXMATRIX invw;
				D3DXMatrixInverse( &invw, NULL, &matWorld );

				switch( centerflag ){
				case 0:
					if( isbillboard == 0 ){
						centervec = m_orgcenter;
					}else{
						centervec = m_center;
					}
					break;
				case 1:
					if( isbillboard == 0 ){
						//D3DXVec3TransformCoord( &centervec, &selem->m_center, &invw )
						if( etype == SHDPOLYMESH ){
							centervec = selem->polymesh->m_center;
						}else if( etype == SHDPOLYMESH2 ){
							centervec = selem->polymesh2->m_center;
						}else if( etype == SHDMORPH ){
							ret = selem->GetMorphCenterPos( &centervec );
							_ASSERT( !ret );
						}
					}else{
						centervec = selem->polymesh->m_center;
					}
					break;
				case 2:
					centervec.x = 0.0f;
					centervec.y = 0.0f;
					centervec.z = 0.0f;
					break;
				default:
					centervec.x = 0.0f;
					centervec.y = 0.0f;
					centervec.z = 0.0f;
					break;
				}

				ret = selem->SetDispScale( scalevec, centervec );
				if( ret ){
					DbgOut( "shandler : SetDispScale : selem SetDispScale error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}else{


		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : SetDispScale : srcseri out of range warning !!!\n" );
			_ASSERT( 0 );
			return 0;//
		}

		selem = (*this)( srcseri );
		if( !selem ){
			DbgOut( "shdhandler : SetDispScale : selem NULL warning !!!\n" );
			_ASSERT( 0 );
			return 0;//			
		}

		etype = selem->type;

		int isbone;
		isbone = (selem->IsJoint() && (etype != SHDMORPH));

		if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDEXTLINE) || isbone || (etype == SHDMORPH) ){

			int isbillboard = 0;
			if( (etype == SHDPOLYMESH) && selem->polymesh && selem->polymesh->billboardflag ){
				isbillboard = 1;
			}

			D3DXMATRIX invw;
			D3DXMatrixInverse( &invw, NULL, &matWorld );

			switch( centerflag ){
			case 0:
				if( isbillboard == 0 ){
					//D3DXVec3TransformCoord( &centervec, &m_center, &invw );
					centervec = m_orgcenter;
				}else{
					centervec = m_center;
				}
				break;
			case 1:
				if( isbillboard == 0 ){
					//D3DXVec3TransformCoord( &centervec, &selem->m_center, &invw )
					if( etype == SHDPOLYMESH ){
						centervec = selem->polymesh->m_center;
					}else if( etype == SHDPOLYMESH2 ){
						centervec = selem->polymesh2->m_center;
					}else if( etype == SHDMORPH ){
						ret = selem->GetMorphCenterPos( &centervec );
						_ASSERT( !ret );
					}
				}else{
					centervec = selem->polymesh->m_center;
				}
				break;
			case 2:
				centervec.x = 0.0f;
				centervec.y = 0.0f;
				centervec.z = 0.0f;
				break;
			default:
				centervec.x = 0.0f;
				centervec.y = 0.0f;
				centervec.z = 0.0f;
				break;
			}


			ret = selem->SetDispScale( scalevec, centervec );
			if( ret ){
				DbgOut( "shandler : SetDispScale : selem SetDispScale error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdHandler::GetScreenPosInfScope( CMotHandler* lpmh, int isindex, int vertno, int* scxptr, int* scyptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj )
{
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	float calaspect;
//	if( matProj._43 < -1.0f ){
//		calaspect = m_aspect;
//	}else{
		//正射影
		calaspect = (float)dwClipWidth / (float)dwClipHeight;
//	}

	//dispdataから取得
	D3DXVECTOR2 dstpos;
	ret = iselem->GetDispScreenPosInfScope( isindex, vertno, &dstpos, m_TLmode );
	if( ret ){
		DbgOut( "shdhandler : GetScreenPosInfScope : selem GetDispScreenPosInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int outx = 0;
	int outy = 0;

	int posx, posy;
	posx = (int)dstpos.x;
	posy = (int)dstpos.y;
	if( (posx < 0) || (posx > (int)(dwClipWidth * 2)) ){
		outx = 1;
	}
	if( (posy < 0) || (posy > (int)(dwClipHeight * 2)) ){
		outy = 1;
	}
	if( (outx == 0) && (outy == 0) ){
		*scxptr = posx;
		*scyptr = posy;
	}else{
		*scxptr = -1;
		*scyptr = -1;
	}

	return 0;
}


int CShdHandler::GetScreenPos( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, int* scxptr, int* scyptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj )
{
	int ret;

	SetCurDS();

	*scxptr = 0xFFFFFFFF;
	*scyptr = 0xFFFFFFFF;


	CShdElem* selem;
	int dispflag;


	if( srcseri < 0 )
		vertno = -1;//!!!!!!


	float calaspect;
//	if( matProj._43 < -1.0f ){
//		calaspect = m_aspect;
//	}else{
		//正射影
		calaspect = (float)dwClipWidth / (float)dwClipHeight;
//	}


	if( vertno < 0 ){
		if( (srcseri >= 0) && (srcseri < s2shd_leng) ){
			
			selem = (*this)( srcseri );
			dispflag = selem->dispflag;
			
			int vflag;
			if( (m_inRDBflag == 0) && (g_useGPU == 0) ){
				if( selem->curbs.visibleflag )
					vflag = 1;
				else
					vflag = 0;
				if( selem->type == SHDMORPH ){
					vflag = selem->morph->m_baseelem->curbs.visibleflag;
				}

			}else{
				vflag = 1;
			}



			if( vflag && (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ) ){

				DWORD dispswitchno = selem->dispswitchno;
				if( (m_curds + dispswitchno)->state != 0 ){
	

//DbgOut( "shandler : GetScreenPos : srcseri %d : curbs.befcenter %f %f %f, tracenter %f %f %f\r\n",
//	srcseri,
//	selem->curbs.befcenter.x, selem->curbs.befcenter.y, selem->curbs.befcenter.z,
//	selem->curbs.tracenter.x, selem->curbs.tracenter.y, selem->curbs.tracenter.z );

					ret = selem->curbs.Transform2ScreenPos( dwClipWidth, dwClipHeight, matView, matProj, calaspect, scxptr, scyptr );
					if( ret ){
						DbgOut( "CShdHandler : GetScreenPos : curbs Transform2ScreenPos error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}else if( srcseri == -1 ){
			int vflag;
			if( (m_inRDBflag == 0) && (g_useGPU == 0) ){
				if( lpmh->m_curbs.visibleflag )
					vflag = 1;
				else
					vflag = 0;
			}else{
				vflag = 1;
			}
			
			if( vflag ){				
				ret = lpmh->m_curbs.Transform2ScreenPos( dwClipWidth, dwClipHeight, matView, matProj, calaspect, scxptr, scyptr );
				if( ret ){
					DbgOut( "CShdHandler : GetScreenPos : total curbs Transform2ScreenPos error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}else{

		D3DXVECTOR3 dstpos;
		
		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : GetScreenPos : srcseri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		selem = (*this)( srcseri );

			//計算し直し

		ret = selem->CalcScreenPos( bbflag, lpmh, vertno, dwClipWidth, dwClipHeight, matWorld, matView, matProj, calaspect, m_TLmode, &dstpos );
		if( ret ){
			DbgOut( "shdhandler : GetScreenPos : selem CalcScreenPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int outx = 0;
		int outy = 0;

		int posx, posy;
		posx = (int)dstpos.x;
		posy = (int)dstpos.y;
		if( (posx < 0) || (posx > (int)(dwClipWidth * 2)) ){
			outx = 1;
		}
		if( (posy < 0) || (posy > (int)(dwClipHeight * 2)) ){
			outy = 1;
		}

		if( (outx == 0) && (outy == 0) ){
			*scxptr = posx;
			*scyptr = posy;
		}else{
			*scxptr = -1;
			*scyptr = -1;
		}
	}

	return 0;
}

int CShdHandler::GetScreenPos3( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, int* scxptr, int* scyptr, float* sczptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int outflag )
{
	int ret;

	SetCurDS();

	*scxptr = 0xFFFFFFFF;
	*scyptr = 0xFFFFFFFF;


	CShdElem* selem;
	int dispflag;


	if( srcseri < 0 )
		vertno = -1;//!!!!!!


	float calaspect;
	//if( matProj._43 < -1.0f ){
	//	calaspect = m_aspect;
	//}else{
		//正射影
	//	calaspect = (float)dwClipWidth / (float)dwClipHeight;
	//}
	calaspect = (float)dwClipWidth / (float)dwClipHeight;

	if( vertno < 0 ){
		if( (srcseri >= 0) && (srcseri < s2shd_leng) ){
			
			selem = (*this)( srcseri );
			dispflag = selem->dispflag;
			
			int vflag;
			if( (m_inRDBflag == 0) && (g_useGPU == 0) ){
				if( selem->curbs.visibleflag )
					vflag = 1;
				else
					vflag = 0;
				if( selem->type == SHDMORPH ){
					vflag = selem->morph->m_baseelem->curbs.visibleflag;
				}

			}else{
				vflag = 1;
			}

			if( vflag && (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ) ){

				DWORD dispswitchno = selem->dispswitchno;
				if( (m_curds + dispswitchno)->state != 0 ){
	

//DbgOut( "shandler : GetScreenPos : srcseri %d : curbs.befcenter %f %f %f, tracenter %f %f %f\r\n",
//	srcseri,
//	selem->curbs.befcenter.x, selem->curbs.befcenter.y, selem->curbs.befcenter.z,
//	selem->curbs.tracenter.x, selem->curbs.tracenter.y, selem->curbs.tracenter.z );

					ret = selem->curbs.Transform2ScreenPos( dwClipWidth, dwClipHeight, matView, matProj, calaspect, scxptr, scyptr, sczptr, outflag );
					if( ret ){
						DbgOut( "CShdHandler : GetScreenPos3 : curbs Transform2ScreenPos error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}else if( srcseri == -1 ){
			int vflag;
			if( (m_inRDBflag == 0) && (g_useGPU == 0) ){
				if( lpmh->m_curbs.visibleflag )
					vflag = 1;
				else
					vflag = 0;
			}else{
				vflag = 1;
			}
			
			if( vflag ){				
				ret = lpmh->m_curbs.Transform2ScreenPos( dwClipWidth, dwClipHeight, matView, matProj, calaspect, scxptr, scyptr, sczptr, outflag );
				if( ret ){
					DbgOut( "CShdHandler : GetScreenPos3 : total curbs Transform2ScreenPos error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}else{

		D3DXVECTOR3 dstpos;
		
		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : GetScreenPos3 : srcseri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		selem = (*this)( srcseri );

			//計算し直し

		ret = selem->CalcScreenPos( bbflag, lpmh, vertno, dwClipWidth, dwClipHeight, matWorld, matView, matProj, calaspect, m_TLmode, &dstpos );
		if( ret ){
			DbgOut( "shdhandler : GetScreenPos3 : selem CalcScreenPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//DbgOut( "check !!! : sh : GetScreenPos3 : %f %f %f\r\n", dstpos.x, dstpos.y, dstpos.z );


		int outx = 0;
		int outy = 0;

		int posx, posy;
		posx = (int)dstpos.x;
		posy = (int)dstpos.y;
		if( (posx < 0) || (posx > (int)(dwClipWidth * 2)) ){
			outx = 1;
		}
		if( (posy < 0) || (posy > (int)(dwClipHeight * 2)) ){
			outy = 1;
		}

//outflag = 1;

		if( outflag == 0 ){
			if( (outx == 0) && (outy == 0) ){
				*scxptr = posx;
				*scyptr = posy;
				*sczptr = dstpos.z;
			}else{
				*scxptr = -1;
				*scyptr = -1;
				*sczptr = 1.0f;
			}
		}else{
			//画面外でも数値を格納
			*scxptr = posx;
			*scyptr = posy;
			*sczptr = dstpos.z;
		}
	}

	return 0;
}

int CShdHandler::GetScreenPos3F( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, DWORD dwClipWidth, DWORD dwClipHeight,
	D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, D3DXVECTOR3* scpos )
{
	int outflag = 1;


	int ret;

	SetCurDS();

	CShdElem* selem;
	//int dispflag;

	if( srcseri < 0 )
		vertno = -1;//!!!!!!

	float calaspect;
	calaspect = (float)dwClipWidth / (float)dwClipHeight;

	if( vertno < 0 ){
		if( (srcseri >= 0) && (srcseri < s2shd_leng) ){
			selem = (*this)( srcseri );
			if( (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ) ){
				ret = selem->curbs.Transform2ScreenPos( dwClipWidth, dwClipHeight, matView, matProj, calaspect, scpos );
				if( ret ){
					DbgOut( "CShdHandler : GetScreenPos3F : curbs Transform2ScreenPos error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}else if( srcseri == -1 ){
			ret = lpmh->m_curbs.Transform2ScreenPos( dwClipWidth, dwClipHeight, matView, matProj, calaspect, scpos );
			if( ret ){
				DbgOut( "CShdHandler : GetScreenPos3F : total curbs Transform2ScreenPos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		
		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : GetScreenPos3 : srcseri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		selem = (*this)( srcseri );

			//計算し直し
		ret = selem->CalcScreenPos( bbflag, lpmh, vertno, dwClipWidth, dwClipHeight, matWorld, matView, matProj, calaspect, m_TLmode, scpos );
		if( ret ){
			DbgOut( "shdhandler : GetScreenPos3 : selem CalcScreenPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CShdHandler::GetScreenPos4( int bbflag, CMotHandler* lpmh, int srcseri, int vertno, int calcmode, float* scxptr, float* scyptr, float* sczptr, DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj )
{
	int ret;

	*scxptr = -1.0f;
	*scyptr = -1.0f;


	CShdElem* selem;
	//int dispflag;


	if( srcseri < 0 )
		vertno = -1;//!!!!!!


	float calaspect;
	//if( matProj._43 < -1.0f ){
	//	calaspect = m_aspect;
	//}else{
		//正射影
	//	calaspect = (float)dwClipWidth / (float)dwClipHeight;
	//}
	calaspect = (float)dwClipWidth / (float)dwClipHeight;

	if( vertno < 0 ){
		_ASSERT( 0 );
		return 1;
	}else{

		D3DXVECTOR3 dstpos;
		
		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shdhandler : GetScreenPos4 : srcseri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		selem = (*this)( srcseri );

			//計算し直し

		ret = selem->CalcScreenPos( bbflag, lpmh, vertno, dwClipWidth, dwClipHeight, matWorld, matView, matProj, calaspect, m_TLmode, &dstpos );
		if( ret ){
			DbgOut( "shdhandler : GetScreenPos4 : selem CalcScreenPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//DbgOut( "check !!! : sh : GetScreenPos3 : %f %f %f\r\n", dstpos.x, dstpos.y, dstpos.z );


		int outx = 0;
		int outy = 0;

		int posx, posy;
		posx = (int)dstpos.x;
		posy = (int)dstpos.y;
		if( (posx < 0) || (posx > (int)(dwClipWidth * 2)) ){
			outx = 1;
		}
		if( (posy < 0) || (posy > (int)(dwClipHeight * 2)) ){
			outy = 1;
		}

		if( (outx == 0) && (outy == 0) ){
			*scxptr = dstpos.x;//!!!!!!!!!!!!3と違うところは、ここがfloat
			*scyptr = dstpos.y;//!!!!!!!!!!!! 
			*sczptr = dstpos.z;
		}else{
			*scxptr = -1;
			*scyptr = -1;
			*sczptr = 1.0f;
		}
	}

	return 0;
}

int CShdHandler::SetRenderState( LPDIRECT3DDEVICE9 pdev, CShdElem* selem )
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
					DbgOut( "sh : SetRenderState : %d error !!!\r\n", rsno );
				}
				g_renderstate[ rsno ] = curvalue;
			}
		}
	}

	if( selem->type == SHDEXTLINE ){
		if( g_renderstate[ D3DRS_BLENDOP ] != selem->m_renderstate[ D3DRS_BLENDOP ] ){
			pdev->SetRenderState( D3DRS_BLENDOP, selem->m_renderstate[ D3DRS_BLENDOP ] );
			g_renderstate[ D3DRS_BLENDOP ] = selem->m_renderstate[ D3DRS_BLENDOP ];
		}
		if( g_renderstate[ D3DRS_SRCBLEND ] != selem->m_renderstate[ D3DRS_SRCBLEND ] ){
			pdev->SetRenderState( D3DRS_SRCBLEND, selem->m_renderstate[ D3DRS_SRCBLEND ] );
			g_renderstate[ D3DRS_SRCBLEND ] = selem->m_renderstate[ D3DRS_SRCBLEND ];
		}
		if( g_renderstate[ D3DRS_DESTBLEND ] != selem->m_renderstate[ D3DRS_DESTBLEND ] ){
			pdev->SetRenderState( D3DRS_DESTBLEND, selem->m_renderstate[ D3DRS_DESTBLEND ] );
			g_renderstate[ D3DRS_DESTBLEND ] = selem->m_renderstate[ D3DRS_DESTBLEND ];
		}
	}

////////////
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

int CShdHandler::GetFirstJoint( CMotionCtrl** dstjoint, CMotHandler* lpmh, int needbone )
{

	int i;
	CMotionCtrl* curmc;
	CMotionCtrl* firstjoint = 0;
	CShdElem* curse;
	for( i = 0; i < s2shd_leng; i++ ){
		curmc = (*lpmh)( i );
		curse = (*this)( i );
		
		if( curmc->IsJoint() && (curmc->type != SHDMORPH) ){

			if( needbone == 1 ){
				if( curse->part && (curse->part->bonenum >= 1) ){
					firstjoint = curmc;
					break;
				}
			}else{
				firstjoint = curmc;
				break;
			}
		}
	}

	*dstjoint = firstjoint;

	return 0;
}

int CShdHandler::GetCurrentBonePos( int boneno, int poskind, D3DXVECTOR3* dstpos, CMotHandler* lpmh, D3DXMATRIX matWorld )
{
	if( (boneno < 0) || (boneno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetCurrentBonePos : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* selem;
	selem = (*this)( boneno );

	if( !selem ){
		DbgOut( "shdhandler : GetCurrentBonePos : selem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( !(selem->IsJoint()) || (selem->type == SHDMORPH) ){
		DbgOut( "shdhandler : GetCurrentBonePos : this element is not bone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	CPart* partptr;
	partptr = selem->part;
	if( !partptr ){
		DbgOut( "shdhandler : GetCurrentBonePos : partptr NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}


	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	if( !d3ddispptr ){
		DbgOut( "shdhandler : GetCurrentBonePos : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( poskind == 0 ){
		// local座標

		D3DXMATRIX mscale;
		mscale = d3ddispptr->m_scalemat;

		float x, y, z;
		x = partptr->jointloc.x;
		y = partptr->jointloc.y;
		z = partptr->jointloc.z;

		dstpos->x = mscale._11*x + mscale._21*y + mscale._31*z + mscale._41;
		dstpos->y = mscale._12*x + mscale._22*y + mscale._32*z + mscale._42;
		dstpos->z = mscale._13*x + mscale._23*y + mscale._33*z + mscale._43;

	}else{
		D3DXMATRIX tmpmatWorld;
		if( poskind == 2 ){
			D3DXMatrixIdentity( &tmpmatWorld );//ローカルボーン変形
		}else{
			tmpmatWorld = matWorld;//グローバル座標
		}
		
		int scaleflag = 1;
		ret = d3ddispptr->TransformOnlyWorld1VertCurrent( lpmh, tmpmatWorld, 0, dstpos, scaleflag, 0 );
		if( ret ){
			DbgOut( "shdhandler : GetCurrentBonePos : d3ddisp TransformOnlyWorld1VertCurrent error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CShdHandler::GetBonePos( int boneno, int poskind, int motid, int frameno, D3DXVECTOR3* dstpos, CMotHandler* lpmh, D3DXMATRIX matWorld, int scaleflag )
{
	if( (boneno < 0) || (boneno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetBonePos : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* selem;
	selem = (*this)( boneno );

	if( !selem ){
		DbgOut( "shdhandler : GetBonePos : selem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( !(selem->IsJoint()) || (selem->type == SHDMORPH) ){
		DbgOut( "shdhandler : GetBonePos : this element is not bone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( motid >= 0 ){
		if( (motid < 0) || (motid >= lpmh->m_kindnum) ){
			DbgOut( "shdhandler : GetBonePos : motid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int frameleng;
		ret = lpmh->GetMotionFrameLength( motid, &frameleng );
		if( ret ){
			DbgOut( "shdhandler : GetBonePos : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (frameno < 0 ) || (frameno >= frameleng) ){
			DbgOut( "shdhandler : GetBonePos : frameno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		poskind = 0;//!!!!!!!!!!!

	}

	CPart* partptr;
	partptr = selem->part;
	if( !partptr ){
		DbgOut( "shdhandler : GetBonePos : partptr NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}


	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	if( !d3ddispptr ){
		DbgOut( "shdhandler : GetBonePos : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( poskind == 0 ){
		// local座標

		if( scaleflag == 0 ){
			dstpos->x = partptr->jointloc.x;
			dstpos->y = partptr->jointloc.y;
			dstpos->z = partptr->jointloc.z;
		}else{
			D3DXMATRIX mscale;
			mscale = d3ddispptr->m_scalemat;

			float x, y, z;
			x = partptr->jointloc.x;
			y = partptr->jointloc.y;
			z = partptr->jointloc.z;

			dstpos->x = mscale._11*x + mscale._21*y + mscale._31*z + mscale._41;
			dstpos->y = mscale._12*x + mscale._22*y + mscale._32*z + mscale._42;
			dstpos->z = mscale._13*x + mscale._23*y + mscale._33*z + mscale._43;

		}
	}else{
		D3DXMATRIX tmpmatWorld;
		if( poskind == 2 ){
			D3DXMatrixIdentity( &tmpmatWorld );//ローカルボーン変形
		}else{
			tmpmatWorld = matWorld;//グローバル座標
		}
		
		ret = d3ddispptr->TransformOnlyWorld1Vert( lpmh, tmpmatWorld, motid, frameno, 0, dstpos, scaleflag, 0 );
		if( ret ){
			DbgOut( "shdhandler : GetBonePos : d3ddisp TransformOnlyWorld1Vert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::RestoreExtLineColor()
{
	int ret;
	CShdElem* selem;
	int elemno;
	for( elemno = 1; elemno < s2shd_leng; elemno++ ){
		selem = (*this)( elemno );
		if( selem->type == SHDEXTLINE ){
	
			CMeshInfo* mi;
			mi = selem->GetMeshInfo();
			_ASSERT( mi );
			CVec3f* midiff;
			midiff = mi->GetMaterial( MAT_DIFFUSE );

//DbgOut( "shandler : RestoreExtLineColor : micolor %f %f %f\r\n", midiff->x, midiff->y, midiff->z );

			CD3DDisp* d3dptr;
			d3dptr = selem->d3ddisp;
			if( !d3dptr ){
				_ASSERT( 0 );
				continue;
			}

			D3DXMATRIX inimat;
			D3DXMatrixIdentity( &inimat );

			_ASSERT( selem->extline );
			ret = d3dptr->InitColor( selem->extline, selem->alpha, inimat );
			if( ret ){
				DbgOut( "shdhandler : RestoreExtLineColor : d3ddisp InitColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3dptr->Copy2VertexBuffer( 0 );
			if( ret ){
				DbgOut( "shdhandler : RestoreExtLineColor : d3ddisp Copoy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


int CShdHandler::SetExtLineColor( int srcseri, int a, int r, int g, int b )
{

	int elemno;
	CShdElem* selem;

	VEC3F setcol;
	setcol.x = (float)( r / 255.0f );
	setcol.y = (float)( g / 255.0f );
	setcol.z = (float)( b / 255.0f );

	float alpha = (float)( a / 255.0f );

	int setno = 0;
	int ret;

//DbgOut( "shdhandler : SetExtLineColor %d %d %d %d\n", a, r, g, b );

	if( srcseri < 0 ){
		for( elemno = 0; elemno < s2shd_leng; elemno++ ){
			ret = SetExtLineColor( elemno, a, r, g, b );
			if( ret ){
				DbgOut( "shandler : SetExtLineColor : error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		if( srcseri >= s2shd_leng )
			return 1;

		selem = (*this)( srcseri );
		_ASSERT( selem );

		if( selem->type == SHDEXTLINE ){
			CMeshInfo* miptr;
			miptr = selem->GetMeshInfo();
			if( !miptr ){
				_ASSERT( 0 );
				return 1;//!!!!!!!!!
			}

			ret = miptr->SetMem( setcol, MAT_DIFFUSE | VEC_ALL );
			if( ret ){
				DbgOut( "shdhandler : SetExtLineColor : mi SetMem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			selem->alpha = alpha;
			
			CD3DDisp* d3dptr;
			d3dptr = selem->d3ddisp;
			if( !d3dptr ){
				_ASSERT( 0 );
				return 1;
			}

			D3DXMATRIX inimat;
			D3DXMatrixIdentity( &inimat );

			_ASSERT( selem->extline );
			ret = d3dptr->InitColor( selem->extline, alpha, inimat );
			if( ret ){
				DbgOut( "shdhandler : SetExtLineColor : d3ddisp InitColor error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = d3dptr->Copy2VertexBuffer( 0 );
			if( ret ){
				DbgOut( "shdhandler : SetExtLineColor : d3ddisp Copoy2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}

	return 0;
}

int CShdHandler::AddPoint2ExtLine( int previd, int* newidptr )
{
	int elemno;
	CShdElem* selem;
	CShdElem* lineselem = 0;
	CExtLine* extline;

	for( elemno = 0; elemno < s2shd_leng; elemno++ ){
		selem = (*this)( elemno );
		_ASSERT( selem );

		if( selem->type == SHDEXTLINE ){
			lineselem = selem;
			break;
		}

	}

	if( lineselem == 0 ){
		DbgOut( "shdhandler : AddPoint2ExtLine : EXTLINE not found : skip : error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	extline = lineselem->extline;
	if( !extline ){
		DbgOut( "shdhandler : AddPoint2ExtLine : extline not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	CMeshInfo* miptr;
	miptr = lineselem->GetMeshInfo();
	if( !miptr ){
		DbgOut( "shdhandler : AddPoint2ExtLine : miptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( extline->pnum >= miptr->n ){
		DbgOut( "shdhandler : AddPoint2ExtLine : pointnum over maxpointnum : skip : error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;
	ret = extline->AddExtPoint( previd, 1, newidptr );
	if( ret ){
		DbgOut( "shdhandler : AddPoint2ExtLine : extline AddExtPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////////
	CD3DDisp* d3dptr;
	d3dptr = lineselem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "shdhandler : AddPoint2ExtLine : d3dptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!!
	}

	miptr->m = extline->pnum;
	d3dptr->m_unum = extline->pnum;

	if( d3dptr->m_linekind == D3DPT_LINELIST ){
		d3dptr->m_numPrim = d3dptr->m_unum / 2;
	}else if( d3dptr->m_linekind == D3DPT_LINESTRIP ){
		d3dptr->m_numPrim = d3dptr->m_unum - 1;
	}

	return 0;
}

int CShdHandler::DeletePointOfExtLine( int pid )
{
	int elemno;
	CShdElem* selem;
	CShdElem* lineselem = 0;
	CExtLine* extline;

	for( elemno = 0; elemno < s2shd_leng; elemno++ ){
		selem = (*this)( elemno );
		_ASSERT( selem );

		if( selem->type == SHDEXTLINE ){
			lineselem = selem;
			break;
		}

	}

	if( lineselem == 0 ){
		DbgOut( "shdhandler : DeletePointOfExtLine : EXTLINE not found : skip : error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	extline = lineselem->extline;
	if( !extline ){
		DbgOut( "shdhandler : DeletePointOfExtLine : extline not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;
	ret = extline->RemoveExtPoint( pid );
	if( ret ){
		DbgOut( "shdhandler : DeletePointOfExtLine : extline RemoveExtPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////


	CD3DDisp* d3dptr;
	d3dptr = lineselem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "shdhandler : DeletePointOfExtLine : d3dptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!!
	}

	CMeshInfo* miptr;
	miptr = lineselem->GetMeshInfo();
	if( !miptr ){
		DbgOut( "shdhandler : DeletePointOfExtLine : miptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	miptr->m = extline->pnum;//!!!!!

	ret = d3dptr->UpdateExtLine( extline, lineselem->alpha );
	if( ret ){
		DbgOut( "shdhandler : DeletePointOfExtLine : d3dptr UpdateExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::SetPointPosOfExtLine( int pid, D3DXVECTOR3 srcpos )
{
	int elemno;
	CShdElem* selem;
	CShdElem* lineselem = 0;
	CExtLine* extline;

	for( elemno = 0; elemno < s2shd_leng; elemno++ ){
		selem = (*this)( elemno );
		_ASSERT( selem );

		if( selem->type == SHDEXTLINE ){
			lineselem = selem;
			break;
		}

	}

	if( lineselem == 0 ){
		DbgOut( "shdhandler : SetPointPosOfExtLine : EXTLINE not found : skip : error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	extline = lineselem->extline;
	if( !extline ){
		DbgOut( "shdhandler : SetPointPosOfExtLine : extline not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;
	ret = extline->SetExtPointPos( pid, &srcpos );
	if( ret ){
		DbgOut( "shdhandler : SetPointPosOfExtLine : extline SetExtPointPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////


	CD3DDisp* d3dptr;
	d3dptr = lineselem->d3ddisp;
	if( !d3dptr ){
		DbgOut( "shdhandler : SetPointPosOfExtLine : d3dptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!!
	}

	ret = d3dptr->UpdateExtLine( extline, lineselem->alpha );
	if( ret ){
		DbgOut( "shdhandler : SetPointPosOfExtLine : d3dptr UpdateExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

int CShdHandler::GetPointPosOfExtLine( int pid, D3DXVECTOR3* dstpos )
{
	int elemno;
	CShdElem* selem;
	CShdElem* lineselem = 0;
	CExtLine* extline;

	for( elemno = 0; elemno < s2shd_leng; elemno++ ){
		selem = (*this)( elemno );
		_ASSERT( selem );

		if( selem->type == SHDEXTLINE ){
			lineselem = selem;
			break;
		}

	}

	if( lineselem == 0 ){
		DbgOut( "shdhandler : GetPointPosOfExtLine : EXTLINE not found : skip : error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	extline = lineselem->extline;
	if( !extline ){
		DbgOut( "shdhandler : GetPointPosOfExtLine : extline not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;
	ret = extline->GetExtPointPos( pid, dstpos );
	if( ret ){
		DbgOut( "shdhandler : GetPointPosOfExtLine : extline GetExtPointPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::GetNextPointOfExtLine( int previd, int* nextptr )
{
	int elemno;
	CShdElem* selem;
	CShdElem* lineselem = 0;
	CExtLine* extline;

	for( elemno = 0; elemno < s2shd_leng; elemno++ ){
		selem = (*this)( elemno );
		_ASSERT( selem );

		if( selem->type == SHDEXTLINE ){
			lineselem = selem;
			break;
		}

	}

	if( lineselem == 0 ){
		DbgOut( "shdhandler : GetNextPointOfExtLine : EXTLINE not found : skip : error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	extline = lineselem->extline;
	if( !extline ){
		DbgOut( "shdhandler : GetNextPointOfExtLine : extline not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;
	ret = extline->GetNextExtPoint( previd, nextptr );
	if( ret ){
		DbgOut( "shdhandler : GetNextPointOfExtLine : extline GetNextExtPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::GetPrevPointOfExtLine( int pid, int* prevptr )
{
	int elemno;
	CShdElem* selem;
	CShdElem* lineselem = 0;
	CExtLine* extline;

	for( elemno = 0; elemno < s2shd_leng; elemno++ ){
		selem = (*this)( elemno );
		_ASSERT( selem );

		if( selem->type == SHDEXTLINE ){
			lineselem = selem;
			break;
		}

	}

	if( lineselem == 0 ){
		DbgOut( "shdhandler : GetPrevPointOfExtLine : EXTLINE not found : skip : error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	extline = lineselem->extline;
	if( !extline ){
		DbgOut( "shdhandler : GetPrevPointOfExtLine : extline not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	int ret;
	ret = extline->GetPrevExtPoint( pid, prevptr );
	if( ret ){
		DbgOut( "shdhandler : GetPrevPointOfExtLine : extline GetPrevExtPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CShdHandler::PickVertInfScope( int isindex, DWORD dwClipWidth, DWORD dwClipHeight, CMotHandler* srclpmh,
	D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, float srcaspect,
	int posx, int posy, int rangex, int rangey, int* vertptr, int arrayleng, int* getnumptr )
{
	int ret;
	*getnumptr = 0;

	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}


	//if( m_TLmode != TLMODE_ORG ){
	//	DbgOut( "shdhandler : PickVert3InfScope : tlmode error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}


	CPickData nearpick;
	int maxgetnum;
	maxgetnum = min( PICKDATAMAX, arrayleng );//!!!!!

	D3DXMATRIX firstScale;
	GetFirstScale( &firstScale );

	ret = iselem->PickVertInfScope( isindex, m_seri2boneno, firstScale, dwClipWidth, dwClipHeight, srclpmh, matWorld, matView, matProj, srcaspect,
		posx, posy, rangex, rangey, &nearpick, maxgetnum, getnumptr );
	if( ret ){
		DbgOut( "sh : PickVertInfScope : se PickVertInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( nearpick.setflag != 0 ){
		int datano;
		for( datano = 0; datano < *getnumptr; datano++ ){
			//*( partptr + datano ) = nearpick.partno[datano];
			*( vertptr + datano ) = nearpick.vertno[datano];
		}

	}else{
		*getnumptr = 0;
		//*partptr = -1;
		*vertptr = -1;
	}

	return 0;

}

int CShdHandler::PickVertSelDisp( int seldisp, CMotHandler* srclpmh, DWORD dwClipW, DWORD dwClipH, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj,
	float srcaspect, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr )
{
	int ret;

	*partptr = 0;
	*getnumptr = 0;

	D3DXMATRIX firstscale;
	GetFirstScale( &firstscale );

	SetCurDS();

	//int seri;
	CShdElem* selem;
	CPickData nearpick;

	int maxgetnum;
	maxgetnum = min( PICKDATAMAX, arrayleng );//!!!!!

	//for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seldisp );
		_ASSERT( selem );

		int vflag;
		if( (m_inRDBflag == 0) && (g_useGPU == 0) ){
			if( selem->curbs.visibleflag )
				vflag = 1;
			else
				vflag = 0;
			if( selem->type == SHDMORPH ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}

		}else{
			vflag = 1;
		}

		if( vflag && (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){

				_ASSERT( selem->d3ddisp );
				ret = selem->d3ddisp->TransformDispDataSCV( m_seri2boneno, seldisp, &firstscale, dwClipW, dwClipH, srclpmh, matWorld, matView, matProj, srcaspect );
				if( ret ){
					DbgOut( "sh : PickVert : se TransformDispDataSCV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = selem->PickVert( posx, posy, rangex, rangey, &nearpick, maxgetnum, getnumptr );
				if( ret ){
					DbgOut( "shdhandler : PickVert : selem PickVert error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	//}

	if( nearpick.setflag != 0 ){
		int datano;
		for( datano = 0; datano < *getnumptr; datano++ ){
			*( partptr + datano ) = nearpick.partno[datano];
			*( vertptr + datano ) = nearpick.vertno[datano];
		}

	}else{
		*partptr = 0;
		*getnumptr = 0;
		*partptr = -1;
		*vertptr = -1;
	}

	return 0;
}


int CShdHandler::PickVert( CMotHandler* srclpmh, DWORD dwClipW, DWORD dwClipH, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj,
	float srcaspect, int posx, int posy, int rangex, int rangey, int* partptr, int* vertptr, int arrayleng, int* getnumptr )
{
	int ret;

	*getnumptr = 0;

	//if( m_TLmode != TLMODE_ORG ){
	//	DbgOut( "shdhandler : PickVert : tlmode error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

	D3DXMATRIX firstscale;
	GetFirstScale( &firstscale );

	SetCurDS();

	int seri;
	CShdElem* selem;
	CPickData nearpick;

	int maxgetnum;
	maxgetnum = min( PICKDATAMAX, arrayleng );//!!!!!

	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		_ASSERT( selem );

		int vflag;
		if( (m_inRDBflag == 0) && (g_useGPU == 0) ){
			if( selem->curbs.visibleflag )
				vflag = 1;
			else
				vflag = 0;
			if( selem->type == SHDMORPH ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}

		}else{
			vflag = 1;
		}

		if( vflag && (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){

				_ASSERT( selem->d3ddisp );
				ret = selem->d3ddisp->TransformDispDataSCV( m_seri2boneno, seri, &firstscale, dwClipW, dwClipH, srclpmh, matWorld, matView, matProj, srcaspect );
				if( ret ){
					DbgOut( "sh : PickVert : se TransformDispDataSCV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = selem->PickVert( posx, posy, rangex, rangey, &nearpick, maxgetnum, getnumptr );
				if( ret ){
					DbgOut( "shdhandler : PickVert : selem PickVert error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}
		}
	}

	if( nearpick.setflag != 0 ){
		int datano;
		for( datano = 0; datano < *getnumptr; datano++ ){
			*( partptr + datano ) = nearpick.partno[datano];
			*( vertptr + datano ) = nearpick.vertno[datano];
		}

	}else{
		*getnumptr = 0;
		*partptr = -1;
		*vertptr = -1;
	}

	return 0;
}

int CShdHandler::ChkConfLineAndFaceInfScope( int isindex, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj,
	D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr )
{

	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}


	int ret;
		
		//matworld, matViewが掛かっている頂点データを検索する。
        // Compute the vector of the pick ray in screen space
        // Compute the vector of the pick ray in screen space
 

// start3dとend3dは、world座標系
		
//　startwvとendwvは、matWorldが掛かったもの。（ground以外の、d3ddispのm_worldvと同じ） 
	D3DXMATRIX matwv;
	matwv = matWorld;// * matView;
		
		
	D3DXVECTOR3 startwv, endwv;
	startwv = srcpos1;
	endwv = srcpos2;


/////////
	CONFDATA cf;		
	int result = 0;
	DISTSAMPLE ds;
	ds.setflag = 0;
	ds.dist = 1e10;
	ds.nv.x = 0.0f;
	ds.nv.y = 0.0f;
	ds.nv.z = 0.0f;
	//ret = ChkConfFace( start3d, end3d, startwv, endwv, srclpmh, &result, dstpos, dstn, &cf, &ds );


	CONFDATA confdata[CONFPOINTMAX];
	int confpointnum = 0;
	DISTSAMPLE neards;
	neards.setflag = 0;
	neards.dist = 1e10;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;

	D3DXMATRIX firstscale;
	GetFirstScale( &firstscale );

	ret = iselem->CalcMeshWorldv( m_seri2boneno, srclpmh, &firstscale, &matWorld );
	if( ret ){
		DbgOut( "sh : ChkConfLineAndFaceInfScope : se CalcMeshWorldv error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int curpointnum = 0;
	ret = iselem->ChkConfVecAndFaceInfScope( isindex, &startwv, &endwv, &curpointnum, &(confdata[confpointnum]), CONFPOINTMAX - confpointnum, &ds );
	if( ret ){
		DbgOut( "shdhandler : ChkConfLineAndFaceInfScope : ChkConfVecAndFaceInfScope error !!!\n" );
		_ASSERT( 0 );
		*faceptr = -1;
		return 1;
	}		
	confpointnum += curpointnum;
	if( curpointnum > 0 ){
		//if( ds ){
			if( ds.setflag && (ds.dist < neards.dist) ){
				neards = ds;
			}
		//}
	}

	if( confpointnum > 0 ){

		CONFDATA nearconf;
		D3DXVECTOR3* curadv;
		float minmag = (float)1e10;

		int confno;
		for( confno = 0; confno < confpointnum; confno++ ){
			float diffx, diffy, diffz;
			float curmag;
			curadv = &(confdata[confno].adjustv);

			diffx = startwv.x - curadv->x;
			diffy = startwv.y - curadv->y;
			diffz = startwv.z - curadv->z;


			curmag = diffx * diffx + diffy * diffy + diffz * diffz;
			if( curmag < minmag ){
				nearconf = confdata[confno];
				minmag = curmag;
			}
		}

		cf = nearconf;
		ds = neards;
		*faceptr = cf.faceno;
		*dstn = nearconf.nv;
		*dstpos = nearconf.adjustv;

	}else{
		// mode 0 のときのみ、returnする。
		*faceptr = -1;
	}


	return 0;

}


int CShdHandler::ChkConfLineAndFaceSelDisp( int seldisp, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView,
	D3DXVECTOR3 start3d, D3DXVECTOR3 end3d, int needtrans, int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr )
{


	int ret;

	if( groundflag == 1 ){
		// 地面データを検索処理する。

		CONFDATA cf;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;

		int result = 0;
		ret = ChkConfGround( &seldisp, 1, &matWorld, start3d, end3d, srclpmh, 0, 200.0f, -100.0f, &result, dstpos, dstn, &ds, &cf );
		if( ret ){
			DbgOut( "shandler : ChkConfLineAndFace : ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;

		}
		return 0;

	}else{

		if( needtrans == 1 ){

			//D3DXMATRIX matWV;
			//matWV = matWorld;// * matView;

			D3DXMATRIX firstScale;
			GetFirstScale( &firstScale );

			ret = CalcMeshWorldv( srclpmh, matWorld, matView, &firstScale );
			if( ret ){
				DbgOut( "shandler : ChkConfLineAndFace : CalcMeshWMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

// start3dとend3dは、world座標系
		
//　startwvとendwvは、matWorldが掛かったもの。（ground以外の、d3ddispのm_worldvと同じ） 
		D3DXMATRIX matwv;
		matwv = matWorld;// * matView;
		
		D3DXVECTOR3 startwv, endwv;
		startwv = start3d;
		endwv = end3d;


/////////
		CONFDATA cf;		
		int result = 0;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;
		ret = ChkConfFace( &seldisp, 1, start3d, end3d, startwv, endwv, srclpmh, &result, dstpos, dstn, &cf, &ds );
		if( ret ){
			DbgOut( "shandler : ChkConfLineAndFace : ChkConfFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;

		}

	}


	return 0;
}

int CShdHandler::ChkConfLineAndFacePart( int chkpartno, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView,
	D3DXVECTOR3 start3d, D3DXVECTOR3 end3d, int needtrans, int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr )
{

	int ret;

	if( groundflag == 1 ){
		// 地面データを検索処理する。

		CONFDATA cf;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;

		int result = 0;
		ret = ChkConfGround( &chkpartno, 1, &matWorld, start3d, end3d, srclpmh, 0, 200.0f, -100.0f, &result, dstpos, dstn, &ds, &cf );
		if( ret ){
			DbgOut( "shandler : ChkConfLineAndFace : ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;

		}
		return 0;

	}else{

		if( needtrans == 1 ){

			//D3DXMATRIX matWV;
			//matWV = matWorld;// * matView;

			D3DXMATRIX firstScale;
			GetFirstScale( &firstScale );

			ret = CalcMeshWorldv( srclpmh, matWorld, matView, &firstScale );
			if( ret ){
				DbgOut( "shandler : ChkConfLineAndFace : CalcMeshWMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	
// start3dとend3dは、world座標系
//　startwvとendwvは、matWorldが掛かったもの。（ground以外の、d3ddispのm_worldvと同じ） 
		D3DXMATRIX matwv;
		matwv = matWorld;// * matView;
				
		D3DXVECTOR3 startwv, endwv;
		startwv = start3d;
		endwv = end3d;

/////////
		CONFDATA cf;		
		int result = 0;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;
		ret = ChkConfFace( &chkpartno, 1, start3d, end3d, startwv, endwv, srclpmh, &result, dstpos, dstn, &cf, &ds );
		if( ret ){
			DbgOut( "shandler : ChkConfLineAndFace : ChkConfFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;
		}

	}


	return 0;
}



int CShdHandler::ChkConfLineAndFace( DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView,
	D3DXVECTOR3 start3d, D3DXVECTOR3 end3d, int needtrans, int* partptr, int* faceptr, D3DXVECTOR3* dstpos, D3DXVECTOR3* dstn, int* revfaceptr )
{


	int ret;

	if( groundflag == 1 ){
		// 地面データを検索処理する。

		CONFDATA cf;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;

		int result = 0;
		ret = ChkConfGround( 0, 0, &matWorld, start3d, end3d, srclpmh, 0, 200.0f, -100.0f, &result, dstpos, dstn, &ds, &cf );
		if( ret ){
			DbgOut( "shandler : ChkConfLineAndFace : ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;

		}
		return 0;

	}else{

		if( needtrans == 1 ){

			//D3DXMATRIX matWV;
			//matWV = matWorld;// * matView;

			D3DXMATRIX firstScale;
			GetFirstScale( &firstScale );

			ret = CalcMeshWorldv( srclpmh, matWorld, matView, &firstScale );
			if( ret ){
				DbgOut( "shandler : ChkConfLineAndFace : CalcMeshWMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	
		
		//matworld, matViewが掛かっている頂点データを検索する。
        // Compute the vector of the pick ray in screen space
        // Compute the vector of the pick ray in screen space
 

// start3dとend3dは、world座標系
		


//　startwvとendwvは、matWorldが掛かったもの。（ground以外の、d3ddispのm_worldvと同じ） 
		D3DXMATRIX matwv;
		matwv = matWorld;// * matView;
		
		
		D3DXVECTOR3 startwv, endwv;

		/***
		float x, y, z;
		x = start3d.x;
		y = start3d.y;
		z = start3d.z;
		startwv.x = matwv._11*x + matwv._21*y + matwv._31*z + matwv._41;
		startwv.y = matwv._12*x + matwv._22*y + matwv._32*z + matwv._42;
		startwv.z = matwv._13*x + matwv._23*y + matwv._33*z + matwv._43;
		

		x = end3d.x;
		y = end3d.y;
		z = end3d.z;
		endwv.x = matwv._11*x + matwv._21*y + matwv._31*z + matwv._41;
		endwv.y = matwv._12*x + matwv._22*y + matwv._32*z + matwv._42;
		endwv.z = matwv._13*x + matwv._23*y + matwv._33*z + matwv._43;
		***/

		//D3DXVec3TransformCoord( &startwv, &start3d, &matwv );
		//D3DXVec3TransformCoord( &endwv, &end3d, &matwv );

		startwv = start3d;
		endwv = end3d;


/////////
		CONFDATA cf;		
		int result = 0;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;
		ret = ChkConfFace( 0, 0, start3d, end3d, startwv, endwv, srclpmh, &result, dstpos, dstn, &cf, &ds );
		if( ret ){
			DbgOut( "shandler : ChkConfLineAndFace : ChkConfFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;

			/***
			//invviewmatを掛ける
			D3DXMATRIX invview;
			D3DXMatrixInverse( &invview, NULL, &matView );

			D3DXVECTOR3 newpos3;
			D3DXVec3TransformCoord( &newpos3, dstpos, &invview );
			*dstpos = newpos3;

			D3DXVECTOR3 newn;
			newn.x  = dstn->x*invview._11 + dstn->y*invview._21 + dstn->z*invview._31;
			newn.y  = dstn->x*invview._12 + dstn->y*invview._22 + dstn->z*invview._32;
			newn.z  = dstn->x*invview._13 + dstn->y*invview._23 + dstn->z*invview._33;
			*dstn = newn;
			***/
		}

	}


	return 0;
}

int CShdHandler::PickInfScopeFace( int isindex, float maxdist, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, int pos2x, int pos2y, int* faceptr, D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr )
{
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		*faceptr = -1;
		return 0;//!!!!!!!!!!!!!!!!!
	}
	
	
	D3DXVECTOR3 start2d, end2d, start3d, end3d;

	start2d.x = (float)pos2x;
	start2d.y = (float)pos2y;
	start2d.z = 0.0f;

	end2d.x = (float)pos2x;
	end2d.y = (float)pos2y;
	end2d.z = 1.0f;


	int ret;
	
		
	//matworldが掛かっている頂点データを検索する。
    // Compute the vector of the pick ray in screen space
    // Compute the vector of the pick ray in screen space
 
// start3dとend3dは、world座標系
		
	/***
	D3DXVECTOR3 v;
    v.x =  ( ( ( 2.0f * (float)pos2x ) / (float)dwWidth  ) - 1.0f ) / matProj._11;
    v.y = -( ( ( 2.0f * (float)pos2y ) / (float)dwHeight ) - 1.0f ) / matProj._22;
	v.z =  1.0f;

    // Get the inverse view matrix
    D3DXMATRIX wv, m;
	wv = matView;
    D3DXMatrixInverse( &m, NULL, &wv );

	D3DXVECTOR3 vPickRayDir;

    // Transform the screen space pick ray into 3D space
    vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

	D3DXVec3Normalize( &vPickRayDir, &vPickRayDir );

    start3d.x = m._41;
    start3d.y = m._42;
    start3d.z = m._43;
		
	end3d = start3d + maxdist * vPickRayDir;
	***/
	float clipw, cliph, asp;
	clipw = (float)dwWidth / 2.0f;
	cliph = (float)dwHeight / 2.0f;
	asp = clipw / cliph;

    D3DXMATRIX wv, m;
	wv = matView * matProj;
	D3DXMatrixInverse( &m, NULL, &wv );
	
	D3DXVECTOR3 v0, v1;
	v0.x = ( (float)pos2x - clipw ) / ( asp * cliph );
	v0.y = ( cliph - (float)pos2y ) / cliph;
	v0.z = 0.0f;

	v1 = v0;
	v1.z = 1.0f;

	D3DXVECTOR3 vStart, vEnd;
	D3DXVECTOR3 vPickRayDir;
	float res1;
    // Transform the screen space pick ray into 3D space
    vStart.x  = v0.x*m._11 + v0.y*m._21 + v0.z*m._31 + m._41;
    vStart.y  = v0.x*m._12 + v0.y*m._22 + v0.z*m._32 + m._42;
    vStart.z  = v0.x*m._13 + v0.y*m._23 + v0.z*m._33 + m._43;
	res1 =		v0.x*m._14 + v0.y*m._24 + v0.z*m._34 + m._44;
	if( res1 != 0.0f ){
		vStart /= res1;
	}

    vEnd.x  = v1.x*m._11 + v1.y*m._21 + v1.z*m._31 + m._41;
    vEnd.y  = v1.x*m._12 + v1.y*m._22 + v1.z*m._32 + m._42;
    vEnd.z  = v1.x*m._13 + v1.y*m._23 + v1.z*m._33 + m._43;
	res1 =	  v1.x*m._14 + v1.y*m._24 + v1.z*m._34 + m._44;
	if( res1 != 0.0f ){
		vEnd /= res1;
	}
	vPickRayDir = vEnd - vStart;

	D3DXVec3Normalize( &vPickRayDir, &vPickRayDir );

	start3d = vStart;
	end3d = start3d + maxdist * vPickRayDir;

//　startwvとendwvは、matWorldが掛かったもの。（ground以外の、d3ddispのm_worldvと同じ）<--- 間違い　！！！start3dと同じ 
	D3DXVECTOR3 startwv, endwv, raywv;

	raywv = vPickRayDir;
	startwv = start3d;

	endwv = startwv + maxdist * raywv;
	
/////////
	CONFDATA cf;		
	DISTSAMPLE ds;
	ds.setflag = 0;
	ds.dist = 1e10;
	ds.nv.x = 0.0f;
	ds.nv.y = 0.0f;
	ds.nv.z = 0.0f;
	//ret = ChkConfFace( start3d, end3d, startwv, endwv, srclpmh, &result, pos3ptr, nptr, &cf, &ds );
	
	CONFDATA confdata[CONFPOINTMAX];
	int confpointnum = 0;
	DISTSAMPLE neards;
	neards.setflag = 0;
	neards.dist = 1e10;
	neards.nv.x = 0.0f;
	neards.nv.y = 0.0f;
	neards.nv.z = 0.0f;


	D3DXMATRIX firstscale;
	GetFirstScale( &firstscale );

	ret = iselem->CalcMeshWorldv( m_seri2boneno, srclpmh, &firstscale, &matWorld );
	if( ret ){
		DbgOut( "sh : PickInfScopeFace : se CalcMeshWorldv error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int curpointnum = 0;
	ret = iselem->ChkConfVecAndFaceInfScope( isindex, &startwv, &endwv, &curpointnum, &(confdata[confpointnum]), CONFPOINTMAX - confpointnum, &ds );
	if( ret ){
		DbgOut( "shdhandler : PickInfScopeFace : ChkConfVecAndFaceInfScope error !!!\n" );
		_ASSERT( 0 );
		*faceptr = -1;
		return 1;
	}		
	confpointnum += curpointnum;
	if( curpointnum > 0 ){
		//if( ds ){
			if( ds.setflag && (ds.dist < neards.dist) ){
				neards = ds;
			}
		//}
	}

	if( confpointnum > 0 ){

		CONFDATA nearconf;
		D3DXVECTOR3* curadv;
		float minmag = (float)1e10;

		int confno;
		for( confno = 0; confno < confpointnum; confno++ ){
			float diffx, diffy, diffz;
			float curmag;
			curadv = &(confdata[confno].adjustv);

			diffx = startwv.x - curadv->x;
			diffy = startwv.y - curadv->y;
			diffz = startwv.z - curadv->z;


			curmag = diffx * diffx + diffy * diffy + diffz * diffz;
			if( curmag < minmag ){
				nearconf = confdata[confno];
				minmag = curmag;
			}
		}

		cf = nearconf;
		ds = neards;
		*faceptr = cf.faceno;
		*nptr = nearconf.nv;
		*pos3ptr = nearconf.adjustv;

		D3DXVECTOR3 diffv;
		//diffv = start3d - newpos3;
		diffv = start3d - *pos3ptr;
		*distptr = D3DXVec3Length( &diffv );


	}else{
		// mode 0 のときのみ、returnする。
		*faceptr = -1;
	}

	return 0;
}


int CShdHandler::PickFace( float maxdist, DWORD dwWidth, DWORD dwHeight, int groundflag, CMotHandler* srclpmh, 
	D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX matProj, 
	int* partarray, int partnum, int pos2x, int pos2y, int* partptr, int* faceptr, 
	D3DXVECTOR3* pos3ptr, D3DXVECTOR3* nptr, float* distptr, int calcmode )
{
	D3DXVECTOR3 start2d, end2d, start3d, end3d;

	start2d.x = (float)pos2x;
	start2d.y = (float)pos2y;
	start2d.z = 0.0f;

	end2d.x = (float)pos2x;
	end2d.y = (float)pos2y;
	end2d.z = 1.0f;


	int ret;

	if( groundflag == 1 ){
		// 地面データを検索処理する。

        // Compute the vector of the pick ray in screen space
		/***
		D3DXVECTOR3 v;
        v.x =  ( ( ( 2.0f * (float)pos2x ) / (float)dwWidth  ) - 1.0f ) / matProj._11;
        v.y = -( ( ( 2.0f * (float)pos2y ) / (float)dwHeight ) - 1.0f ) / matProj._22;
		v.z =  1.0f;

        // Get the inverse view matrix
        D3DXMATRIX m;
        D3DXMatrixInverse( &m, NULL, &matView );

		D3DXVECTOR3 vPickRayDir;

        // Transform the screen space pick ray into 3D space
        vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
        vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
        vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

		D3DXVec3Normalize( &vPickRayDir, &vPickRayDir );

        start3d.x = m._41;
        start3d.y = m._42;
        start3d.z = m._43;
		
		end3d = start3d + maxdist * vPickRayDir;
		***/

		float clipw, cliph, asp;
		clipw = (float)dwWidth / 2.0f;
		cliph = (float)dwHeight / 2.0f;
		asp = clipw / cliph;

        D3DXMATRIX wv, m;
		wv = matView * matProj;
		D3DXMatrixInverse( &m, NULL, &wv );
		
		D3DXVECTOR3 v0, v1;
		v0.x = ( (float)pos2x - clipw ) / ( asp * cliph );
		v0.y = ( cliph - (float)pos2y ) / cliph;
		v0.z = 0.0f;

		v1 = v0;
		v1.z = 1.0f;

		D3DXVECTOR3 vStart, vEnd;
		D3DXVECTOR3 vPickRayDir;
		float res1;
        // Transform the screen space pick ray into 3D space
        vStart.x  = v0.x*m._11 + v0.y*m._21 + v0.z*m._31 + m._41;
        vStart.y  = v0.x*m._12 + v0.y*m._22 + v0.z*m._32 + m._42;
        vStart.z  = v0.x*m._13 + v0.y*m._23 + v0.z*m._33 + m._43;
		res1 =		v0.x*m._14 + v0.y*m._24 + v0.z*m._34 + m._44;
		if( res1 != 0.0f ){
			vStart /= res1;
		}

        vEnd.x  = v1.x*m._11 + v1.y*m._21 + v1.z*m._31 + m._41;
        vEnd.y  = v1.x*m._12 + v1.y*m._22 + v1.z*m._32 + m._42;
        vEnd.z  = v1.x*m._13 + v1.y*m._23 + v1.z*m._33 + m._43;
		res1 =	  v1.x*m._14 + v1.y*m._24 + v1.z*m._34 + m._44;
		if( res1 != 0.0f ){
			vEnd /= res1;
		}
		vPickRayDir = vEnd - vStart;

		D3DXVec3Normalize( &vPickRayDir, &vPickRayDir );

        //start3d.x = m._41;
        //start3d.y = m._42;
        //start3d.z = m._43;
		start3d = vStart;


		end3d = start3d + maxdist * vPickRayDir;



//DbgOut( "shandler : PickFace : pos2x %d, pos2y %d, start3d ( %f %f %f ), end3d ( %f %f %f )\n",
//	   pos2x, pos2y,
//	   start3d.x, start3d.y, start3d.z,
//	   end3d.x, end3d.y, end3d.z );


		CONFDATA cf;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;

		int result = 0;
		ret = ChkConfGround( partarray, partnum, &matWorld, start3d, end3d, srclpmh, 0, 200.0f, -100.0f, &result, pos3ptr, nptr, &ds, &cf );
		if( ret ){
			DbgOut( "shandler : PickFace : ChkConfGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;
			*distptr = ds.dist;
		}


	}else{

		if( calcmode == 1 ){

//			D3DXMATRIX matWV;
//			matWV = matWorld;// * matView;

			D3DXMATRIX firstScale;
			GetFirstScale( &firstScale );

			ret = CalcMeshWorldv( srclpmh, matWorld, matView, &firstScale );
			if( ret ){
				DbgOut( "shandler : PickFace : CalcMeshWMat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	
		
		//matworldが掛かっている頂点データを検索する。
        // Compute the vector of the pick ray in screen space
        // Compute the vector of the pick ray in screen space
 
// start3dとend3dは、world座標系
		
		/***
		D3DXVECTOR3 v;
        v.x =  ( ( ( 2.0f * (float)pos2x ) / (float)dwWidth  ) - 1.0f ) / matProj._11;
        v.y = -( ( ( 2.0f * (float)pos2y ) / (float)dwHeight ) - 1.0f ) / matProj._22;
		v.z =  1.0f;

        // Get the inverse view matrix
        D3DXMATRIX wv, m;
		wv = matView;
        D3DXMatrixInverse( &m, NULL, &wv );
		***/
		float clipw, cliph, asp;
		clipw = (float)dwWidth / 2.0f;
		cliph = (float)dwHeight / 2.0f;
		asp = clipw / cliph;

        D3DXMATRIX wv, m;
		wv = matView * matProj;
		D3DXMatrixInverse( &m, NULL, &wv );
		
		D3DXVECTOR3 v0, v1;
		v0.x = ( (float)pos2x - clipw ) / ( asp * cliph );
		v0.y = ( cliph - (float)pos2y ) / cliph;
		v0.z = 0.0f;

		v1 = v0;
		v1.z = 1.0f;

		D3DXVECTOR3 vStart, vEnd;
		D3DXVECTOR3 vPickRayDir;
		float res1;
        // Transform the screen space pick ray into 3D space
        vStart.x  = v0.x*m._11 + v0.y*m._21 + v0.z*m._31 + m._41;
        vStart.y  = v0.x*m._12 + v0.y*m._22 + v0.z*m._32 + m._42;
        vStart.z  = v0.x*m._13 + v0.y*m._23 + v0.z*m._33 + m._43;
		res1 =		v0.x*m._14 + v0.y*m._24 + v0.z*m._34 + m._44;
		if( res1 != 0.0f ){
			vStart /= res1;
		}

        vEnd.x  = v1.x*m._11 + v1.y*m._21 + v1.z*m._31 + m._41;
        vEnd.y  = v1.x*m._12 + v1.y*m._22 + v1.z*m._32 + m._42;
        vEnd.z  = v1.x*m._13 + v1.y*m._23 + v1.z*m._33 + m._43;
		res1 =	  v1.x*m._14 + v1.y*m._24 + v1.z*m._34 + m._44;
		if( res1 != 0.0f ){
			vEnd /= res1;
		}
		vPickRayDir = vEnd - vStart;

		D3DXVec3Normalize( &vPickRayDir, &vPickRayDir );

        //start3d.x = m._41;
        //start3d.y = m._42;
        //start3d.z = m._43;
		start3d = vStart;


		end3d = start3d + maxdist * vPickRayDir;

//　startwvとendwvは、matWorldが掛かったもの。（ground以外の、d3ddispのm_worldvと同じ）<--- 間違い　！！！start3dと同じ 
		D3DXVECTOR3 startwv, endwv, raywv;

		raywv = vPickRayDir;
		startwv = start3d;

		endwv = startwv + maxdist * raywv;

/////////
		CONFDATA cf;		
		int result = 0;
		DISTSAMPLE ds;
		ds.setflag = 0;
		ds.dist = 1e10;
		ds.nv.x = 0.0f;
		ds.nv.y = 0.0f;
		ds.nv.z = 0.0f;
		ret = ChkConfFace( partarray, partnum, start3d, end3d, startwv, endwv, srclpmh, &result, pos3ptr, nptr, &cf, &ds );
		if( ret ){
			DbgOut( "shandler : PickFace : ChkConfFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( result == 0 ){
			*partptr = -1;
			*faceptr = -1;
		}else{
			*partptr = cf.partsno;
			*faceptr = cf.faceno;

			//*distptr = ds.dist;
		
			D3DXVECTOR3 diffv;
			//diffv = start3d - newpos3;
			diffv = start3d - *pos3ptr;
			*distptr = D3DXVec3Length( &diffv );

		}


	
	}

	return 0;
}

int CShdHandler::GetCullingFlag( CMotHandler* srclpmh, int srcseri, int vertno, int* viewcullptr, int* revcullptr )
{
	*viewcullptr = 1;
	*revcullptr = 1;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shandler : GetCullingFlag : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetCurDS();

	CShdElem* selem;
	selem = (*this)( srcseri );
	_ASSERT( selem );

	int vflag;
	if( m_inRDBflag == 0 ){
		if( selem->curbs.visibleflag )
			vflag = 1;
		else
			vflag = 0;
		if( selem->type == SHDMORPH ){
			vflag = selem->morph->m_baseelem->curbs.visibleflag;
		}

	}else{
		vflag = 1;
	}

	if( vflag && (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ) ){

		DWORD dispswitchno = selem->dispswitchno;
		if( (m_curds + dispswitchno)->state != 0 ){
			*viewcullptr = 0;
		}
	}

	int ret;
	if( *viewcullptr == 0 ){
		ret = selem->GetRevCullingFlag( vertno, revcullptr );
		if( ret ){
			DbgOut( "shandler : GetCullingFlag : selem GetRevCullingFlag error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CShdHandler::GetOrgVertNo( int srcseri, int vertno, int* orgnoarray, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "shandler : GetOrgVertNo : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	CShdElem* selem;
	selem = (*this)( srcseri );
	_ASSERT( selem );

	
	int ret;

	ret = selem->GetOrgVertNo( vertno, orgnoarray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "shandler : GetOrgVertNo : selem GetOrgVertNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::CheckOrgNoBuf( int* nocntptr )
{
	*nocntptr = 0;

	int seri;
	CShdElem* selem;

	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		_ASSERT( selem );

		if( selem->type == SHDPOLYMESH2 ){
			CPolyMesh2* pm2;

			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "shandler : CheckOrgNoBuf : pm2 not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( !(pm2->orgnobuf) ){
				(*nocntptr)++;
			}

		}else if( selem->type == SHDPOLYMESH ){
			CPolyMesh* pm;

			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "shandler : CheckOrgNoBuf : pm not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( !(pm->orgnobuf) ){
				(*nocntptr)++;
			}
		}
	}

	return 0;
}

int CShdHandler::GetJointNum( int* numptr )
{

	*numptr = 0;

	int findcnt = 0;
	CShdElem* selem;
	int seri;

	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			findcnt++;
		}
	}

	*numptr = findcnt;

	return 0;
}


int CShdHandler::CheckBoneNum( int* bonenumptr )
{

	*bonenumptr = 0;//!!!!!


	int i;
	int etype;
	CShdElem* selem;
	CPart* partptr;
	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		etype = selem->type;

		int isbone = ( selem->IsJoint() && (etype != SHDMORPH) );

		if( isbone ){
			partptr = selem->part;
			if( !partptr ){
				continue;
			}

			(*bonenumptr) += partptr->bonenum;
		}
	}


	return 0;
}


int CShdHandler::EnableTexture( int partno, int enableflag )
{	
	
	if( partno < 0 ){
		int seri;
		CShdElem* selem;
		for( seri = 1; seri < s2shd_leng; seri++ ){
			selem = (*this)( seri );
			_ASSERT( selem );

			selem->m_enabletexture = enableflag;

		}

	}else{
		if( (partno <= 0) || (partno >= s2shd_leng) ){
			DbgOut( "shandler : EnableTexture : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*this)( partno );
		_ASSERT( selem );

		selem->m_enabletexture = enableflag;

	}

	return 0;
}


int CShdHandler::UpdateFrustumInfo( D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj )
{
    D3DXMATRIX mat;

    D3DXMatrixMultiply( &mat, pMatView, pMatProj );
    D3DXMatrixInverse( &mat, NULL, &mat );

    m_frinfo.vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
    m_frinfo.vecFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
    m_frinfo.vecFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
    m_frinfo.vecFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
    m_frinfo.vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
    m_frinfo.vecFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
    m_frinfo.vecFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
    m_frinfo.vecFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

    for( INT i = 0; i < 8; i++ )
        D3DXVec3TransformCoord( &m_frinfo.vecFrustum[i], &m_frinfo.vecFrustum[i], &mat );

    D3DXPlaneFromPoints( &m_frinfo.planeFrustum[0], &m_frinfo.vecFrustum[0], 
        &m_frinfo.vecFrustum[1], &m_frinfo.vecFrustum[2] ); // Near
    D3DXPlaneFromPoints( &m_frinfo.planeFrustum[1], &m_frinfo.vecFrustum[6], 
        &m_frinfo.vecFrustum[7], &m_frinfo.vecFrustum[5] ); // Far
    D3DXPlaneFromPoints( &m_frinfo.planeFrustum[2], &m_frinfo.vecFrustum[2], 
        &m_frinfo.vecFrustum[6], &m_frinfo.vecFrustum[4] ); // Left
    D3DXPlaneFromPoints( &m_frinfo.planeFrustum[3], &m_frinfo.vecFrustum[7], 
        &m_frinfo.vecFrustum[3], &m_frinfo.vecFrustum[5] ); // Right
    D3DXPlaneFromPoints( &m_frinfo.planeFrustum[4], &m_frinfo.vecFrustum[2], 
        &m_frinfo.vecFrustum[3], &m_frinfo.vecFrustum[6] ); // Top
    D3DXPlaneFromPoints( &m_frinfo.planeFrustum[5], &m_frinfo.vecFrustum[1], 
        &m_frinfo.vecFrustum[0], &m_frinfo.vecFrustum[4] ); // Bottom

	return 0;
}

int CShdHandler::IsValidJoint( int jno )
{
	if( (jno < 0) || (jno >= s2shd_leng) ){
		return 0;
	}

	CShdElem* selem;
	selem = (*this)( jno );
	_ASSERT( selem );

	if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		return 1;
	}else{
		return 0;
	}
}

int CShdHandler::GetBBox( int partno, int mode, CBBox* dstbb )
{
	if( partno < 0 ){
		*dstbb = m_bbox;

	}else{
		if( (partno < 0) || (partno >= s2shd_leng) ){
			DbgOut( "shandler : GetBBox : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		CShdElem* selem;
		selem = (*this)( partno );
		if( selem->m_bbx1.setflag != 0 ){
			*dstbb = selem->m_bbx1;
		}else{
			//billboardはこっちを通る
			dstbb->minx = selem->curbs.tracenter.x - selem->curbs.rmag;
			dstbb->maxx = selem->curbs.tracenter.x + selem->curbs.rmag;

			dstbb->miny = selem->curbs.tracenter.y - selem->curbs.rmag;
			dstbb->maxy = selem->curbs.tracenter.y + selem->curbs.rmag;

			dstbb->minz = selem->curbs.tracenter.z - selem->curbs.rmag;
			dstbb->maxz = selem->curbs.tracenter.z + selem->curbs.rmag;
		}
	}


/***
	CShdElem* selem;
	int ret;
	dstbb->InitParams();

	if( partno < 0 ){
		CBBox curbb;
		int i;
		for( i = 0; i < s2shd_leng; i++ ){
			selem = (*this)( i );
			if( selem->dispflag ){

				curbb.InitParams();

				ret = selem->GetBBox( mode, &curbb );
				if( ret ){
					DbgOut( "shandler : selem GetBBox error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = dstbb->JoinBBox( &curbb );
				if( ret ){
					DbgOut( "shandler : dstbb JoinBB error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		}

	}else{
		if( (partno < 0) || (partno >= s2shd_leng) ){
			DbgOut( "shandler : GetBBox : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		selem = (*this)( partno );
		if( selem->dispflag ){
			ret = selem->GetBBox( mode, dstbb );
			if( ret ){
				DbgOut( "shandler : selem GetBBox error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
***/
	return 0;
}

int CShdHandler::GetVertNoOfFaceInfScope( int isindex, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr )
{
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = iselem->GetVertNoOfFaceInfScope( isindex, faceno, vert1ptr, vert2ptr, vert3ptr );
	if( ret ){
		DbgOut( "shandler : GetVertNoOfFaceInfScope : selem GetVertNoOfFaceInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;

}


int CShdHandler::GetVertNoOfFace( int partno, int faceno, int* vert1ptr, int* vert2ptr, int* vert3ptr )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetVertNoOfFace : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	int ret;
	if( (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->IsJoint() == 0) ){
		ret = selem->GetVertNoOfFace( faceno, vert1ptr, vert2ptr, vert3ptr );
		if( ret ){
			DbgOut( "shandler : GetVertNoOfFace : selem GetVertNoOfFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

int CShdHandler::GetSamePosVert( int partno, int vertno, int* sameptr, int arrayleng, int* samenumptr )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetSamePosVert : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	int ret;
	if( (selem->dispflag || (selem->m_mtype == M_BASE)) && (selem->IsJoint() == 0) ){
		ret = selem->GetSamePosVert( vertno, sameptr, arrayleng, samenumptr );
		if( ret ){
			DbgOut( "shandler : GetSamePosVert : selem GetSamePosVert error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

/***
int CShdHandler::HuGetBoneWeight( int partno, int vertno, int* boneno1, int* boneno2, float* rate1 )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : HuGetBoneWeight : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	if( !selem ){
		DbgOut( "shandler : HuGetBoneWeight selem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = selem->HuGetBoneWeight( vertno, boneno1, boneno2, rate1 );
	if( ret ){
		DbgOut( "shandler : HuGetBoneWeight : selem HuGetBoneWeight error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/

int CShdHandler::GetTriIndicesInt( int partno, int* buf, int intnum, int* setintnum, int* revflag )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetTriIndices : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	if( !selem ){
		DbgOut( "shandler : GetTriIndices selem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = selem->GetTriIndicesInt( buf, intnum ,setintnum, revflag );
	if( ret ){
		DbgOut( "shandler : GetTriIndices : selem GetTriIndices error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdHandler::GetTriIndices( int partno, WORD* buf, int wordnum, int* setwordnum, int* revflag )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetTriIndices : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	if( !selem ){
		DbgOut( "shandler : GetTriIndices selem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = selem->GetTriIndices( buf, wordnum ,setwordnum, revflag );
	if( ret ){
		DbgOut( "shandler : GetTriIndices : selem GetTriIndices error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

/***
int CShdHandler::GetUV( int partno, int vertno, float* uptr, float* vptr )
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetUV : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	if( !selem ){
		DbgOut( "shandler : GetUV selem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = selem->GetUV( vertno, uptr, vptr );
	if( ret ){
		DbgOut( "shandler : GetUV : selem GetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/

int CShdHandler::CalcMeshWorldv( CMotHandler* lpmh, D3DXMATRIX matWorld, D3DXMATRIX matView, D3DXMATRIX* firstscale )
{
	int ret;

	SetCurDS();

	int i, dispflag;
	CShdElem* selem;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;

	
		int vflag;
		/***
		if( (m_inRDBflag == 0) && (g_useGPU == 0) ){
			//if( selem->curbs.visibleflag || (projmode == PROJ_PREINIT) || isbone || (selem->type == SHDBILLBOARD))
			if( selem->curbs.visibleflag )
				vflag = 1;
			else
				vflag = 0;
		}else{
			vflag = 1;
		}
		***/
		vflag = 1;

		int ismesh;
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ismesh = 1;
		}else{
			ismesh = 0;
		}

//if( i == 8 ){
//	_ASSERT( 0 );
//}

		if( vflag && (dispflag || (selem->m_mtype == M_BASE)) && (selem->notuse != 1) && ismesh ){
			//if( m_TLmode == TLMODE_ORG ){
				DWORD dispswitchno = selem->dispswitchno;

				if( (m_curds + dispswitchno)->state != 0 ){
					if( m_lastboneno != 0 ){
						ret = selem->CalcMeshWorldv( m_seri2boneno, lpmh, firstscale, &matWorld );
						if( ret ){
							DbgOut( "shandler : CalcMeshWorldv : selem CalcMeshWorldv error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						//ボーンなし
						ret = selem->CalcMeshWorldv( 0, lpmh, firstscale, &matWorld );
						if( ret ){
							DbgOut( "shandler : CalcMeshWorldv : selem CalcMeshWorldv error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
				}
			//}
		}


	}
	
	return 0;
}

int CShdHandler::HuGetOffsetMatrix( int boneno, D3DXMATRIX* offsetptr, float mult, int isstandard )
{
	if( (boneno < 0) || (boneno >= s2shd_leng) ){
		DbgOut( "shandler : HuGetOffsetMatrix : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXMATRIX inimat;
	D3DXMatrixIdentity( &inimat );

	CShdElem* selem;
	selem = (*this)( boneno );

	if( (selem->IsJoint()) && (selem->type != SHDMORPH) ){

		if( isstandard == 0 ){
			CShdElem* parelem;
			parelem = FindUpperJoint( selem );
			if( parelem ){
				CPart* part;
				part = parelem->part;
				if( !part ){
					DbgOut( "shandler : HuGetOffsetMatrix : part NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				inimat._41 = part->jointloc.x * mult;
				inimat._42 = part->jointloc.y * mult;
				inimat._43 = part->jointloc.z * mult;

				D3DXMATRIX invmat;
				D3DXMatrixInverse( &invmat, NULL, &inimat );

				*offsetptr = invmat;
			}else{
				*offsetptr = inimat;
			}
		}else{
			CPart* part;
			part = selem->part;
			if( !part ){
				DbgOut( "shandler : HuGetOffsetMatrix : part NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			inimat._41 = part->jointloc.x * mult;
			inimat._42 = part->jointloc.y * mult;
			inimat._43 = part->jointloc.z * mult;

			D3DXMATRIX invmat;
			D3DXMatrixInverse( &invmat, NULL, &inimat );

			*offsetptr = invmat;
		}
	}else{
		*offsetptr = inimat;
	}

	return 0;
}


int CShdHandler::GetIKTransFlag( int jointno, int* flagptr )
{
	if( (jointno <= 0) || (jointno >= s2shd_leng) ){
		DbgOut( "shandler : GetIKTransFlag : jointno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( jointno );

	*flagptr = selem->m_notransik;


	return 0;
}
int CShdHandler::SetIKTransFlag( int jointno, int flag )
{
	if( (jointno <= 0) || (jointno >= s2shd_leng) ){
		DbgOut( "shandler : SetIKTransFlag : jointno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( jointno );

	selem->m_notransik = flag;	

	return 0;
}

int CShdHandler::GetUserInt1OfPart( int partno, int* userint1ptr )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetUserInt1OfPart : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );

	*userint1ptr = selem->m_userint1;

	return 0;

}
int CShdHandler::SetUserInt1OfPart( int partno, int userint1 )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : SetUserInt1OfPart : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );

	selem->m_userint1 = userint1;

	return 0;
}

int CShdHandler::GetBSphere( int partno, D3DXVECTOR3* dstcenter, float* dstr )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetBSphere : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );

	CBSphere bs;
	bs = selem->curbs;

	*dstcenter = bs.tracenter;
	*dstr = bs.rmag;

	return 0;
}


int CShdHandler::GetParent( int partno, int* partnoptr )
{
	*partnoptr = -1;

	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetParent : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CShdElem* parelem;
	parelem = selem->parent;

	if( parelem ){
		*partnoptr = parelem->serialno;
	}else{
		*partnoptr = -1;
	}

	return 0;
}

int CShdHandler::GetChild( int parentno, int arrayleng, int* childnoptr, int* childnumptr )
{
	*childnumptr = 0;

	if( (parentno <= 0) || (parentno >= s2shd_leng) ){
		DbgOut( "shandler : GetChild : parentno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( parentno );

	CShdElem* broelem;
	broelem = selem->child;

	while( broelem ){

		if( *childnumptr >= arrayleng ){
			DbgOut( "shandler : GetChild : arrayleng too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*( childnoptr + *childnumptr ) = broelem->serialno;
		(*childnumptr)++;

		broelem = broelem->brother;
	}

	return 0;
}


int CShdHandler::GetChildJoint( int parentno, int arrayleng, int* childarray, int* childnum )
{
	if( (parentno <= 0) || (parentno >= s2shd_leng) ){
		DbgOut( "shandler : GetChildJoint : parentno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( parentno );

	if( (selem->IsJoint() == 0) || (selem->type == SHDMORPH) ){
		*childnum = 0;
		return 0;
	}

	CPart* partptr;
	partptr = selem->part;
	if( !partptr ){
		*childnum = 0;
		return 0;
	}
	
	*childnum = partptr->bonenum;//!!!!!!!!!

	if( childarray ){

		int setno = 0;
		int childno;
		CBoneInfo* biptr;
		for( childno = 0; childno < partptr->bonenum; childno++ ){
			if( setno >= arrayleng ){
				DbgOut( "shandler : GetChildJoint : arrayleng too short warning !!!\r\n" );
				_ASSERT( 0 );
				break;
			}

			biptr = *(partptr->ppBI + childno);
			if( !biptr ){
				DbgOut( "shandler : GetChildJoint : biptr NULL warning !!!\r\n" );
				_ASSERT( 0 );
				break;
			}

			*( childarray + setno ) = biptr->childno;
//DbgOut( "shandler : GetChildJoint : %x %d\r\n", biptr, biptr->childno );
			setno++;
		}

	}

	
	return 0;
}


int CShdHandler::SetCurrentPose2OrgData( CMotHandler* srclpmh, CQuaternion* multq )
{
	int ret;
	int serino;
	CShdElem* selem;

	CPolyMesh* pm;
	CPolyMesh2* pm2;
	CPart* partptr;


	int scno;
	CInfScope* curis;

	D3DXVECTOR3* wvec;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	int vno;
	VEC3F* curdst;
	D3DTLVERTEX* curdsttlv;
	D3DXVECTOR3* cursrc;

	int ono;
	VEC3F* dstorg;
	D3DTLVERTEX* srctlv;
	int opno;


	for( serino = 0; serino < s2shd_leng; serino++ ){
		selem = (*this)( serino );

		switch( selem->type ){
		case SHDPOLYMESH:
			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "sh : SetCurrentPose2OrgData : pm NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = pm->SetCurrentPose2OrgData( srclpmh, selem, multq );
			if( ret ){
				DbgOut( "sh : SetCurrentPose2OrgData : pm SetCurrentPose2OrgData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case SHDPOLYMESH2:
			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "sh : SetCurrentPose2OrgData : pm2 NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = pm2->SetCurrentPose2OrgData( srclpmh, selem, multq );
			if( ret ){
				DbgOut( "shd : SetCurrentPose2OrgData : pm2 SetCurrentPose2OrgData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			break;
		case SHDINFSCOPE:
			for( scno = 0; scno < selem->scopenum; scno++ ){
				curis = *( selem->ppscope + scno );
				_ASSERT( curis );

				switch( curis->type ){
				case SHDPOLYMESH:
					pm = curis->polymesh;
					if( pm ){

						wvec = new D3DXVECTOR3[ pm->meshinfo->m ];
						if( !wvec ){
							DbgOut( "pm : WriteMQOObjectOnFrame : wvec alloc error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
						ret = curis->d3ddisp->TransformOnlyWorld3( srclpmh, matWorld, wvec, pm->m_IE );
						if( ret ){
							DbgOut( "shdelem : TransformOnlyWorld : d3ddisp->TransformOnlyWorld3 error !!!\n" );
							_ASSERT( 0 );
							delete [] wvec;
							return 1;
						}

						for( vno = 0; vno < pm->meshinfo->m; vno++ ){
							curdst = pm->pointbuf + vno;
							cursrc = wvec + vno;

							if( multq ){
								multq->Rotate( cursrc, *cursrc );
							}

							curdst->x = cursrc->x;
							curdst->y = cursrc->y;
							curdst->z = cursrc->z;
						}

						delete [] wvec;

					}					
					break;
				case SHDPOLYMESH2:
					pm2 = curis->polymesh2;
					if( pm2 ){

						
						wvec = new D3DXVECTOR3[ pm2->optpleng ];
						if( !wvec ){
							DbgOut( "pm2 : SetCurrentPose2OrgData : wvec alloc error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
						ret = curis->d3ddisp->TransformOnlyWorld3( srclpmh, matWorld, wvec, pm2->m_IE );
						if( ret ){
							DbgOut( "shdelem : TransformOnlyWorld : d3ddisp->TransformOnlyWorld3 error !!!\n" );
							_ASSERT( 0 );
							delete [] wvec;
							return 1;
						}
						
						for( vno = 0; vno < pm2->optpleng; vno++ ){
							curdsttlv = pm2->opttlv + vno;
							cursrc = wvec + vno;

							if( multq ){
								multq->Rotate( cursrc, *cursrc );
							}

							curdsttlv->sx = cursrc->x;
							curdsttlv->sy = cursrc->y;
							curdsttlv->sz = cursrc->z;
						}



						delete [] wvec;
					/////////
						for( ono = 0; ono < pm2->meshinfo->n * 3; ono++ ){
							dstorg = pm2->pointbuf + ono;
							
							opno = *( pm2->oldpno2optpno + ono );
							if( opno >= 0 ){
								srctlv = pm2->opttlv + opno;

								dstorg->x = srctlv->sx;
								dstorg->y = srctlv->sy;
								dstorg->z = srctlv->sz;
							}
						}
					
					}
					break;
				default:
					break;
				}

			}


			break;

		default:
			break;
		}


		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			partptr = selem->part;
			if( !partptr ){
				DbgOut( "sh : SetCurrentPose2OrgData : partptr NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = partptr->SetCurrentPose2OrgData( srclpmh, selem, multq );
			if( ret ){
				DbgOut( "sh : SetCurrenetPose2OrgData : part SetCurrentPose2OrgData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int CShdHandler::ConvSymmXShape()
{
	int ret;
	int serino;
	CShdElem* selem;

	CPolyMesh* pm;
	CPolyMesh2* pm2;
	CPart* partptr;

	int scno;
	CInfScope* curis;

//	D3DXVECTOR3* wvec;
//	D3DXMATRIX matWorld;
//	D3DXMatrixIdentity( &matWorld );

	//int vno;
	//VEC3F* curdst;
	//D3DTLVERTEX* curdsttlv;
	//D3DXVECTOR3* cursrc;

	//int ono;
	//VEC3F* dstorg;
	//D3DTLVERTEX* srctlv;
	//int opno;

	int newcw = 0;

	for( serino = 0; serino < s2shd_leng; serino++ ){
		selem = (*this)( serino );

		switch( selem->type ){
		case SHDPOLYMESH:
			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "sh : ConvSymmXShape : pm NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = pm->ConvSymmXShape();
			if( ret ){
				DbgOut( "sh : ConvSymmXShape : pm ConvSymmXShape error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			switch( selem->clockwise ){
			case 1:
				newcw = 2;
				break;
			case 2:
				newcw = 1;
				break;
			default:
				newcw = selem->clockwise;
				break;
			}
			selem->clockwise = newcw;

			break;
		case SHDPOLYMESH2:
			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "sh : ConvSymmXShape : pm2 NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = pm2->ConvSymmXShape();
			if( ret ){
				DbgOut( "shd : ConvSymmXShape : pm2 ConvSymmXShape error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			switch( selem->clockwise ){
			case 1:
				newcw = 2;
				break;
			case 2:
				newcw = 1;
				break;
			default:
				newcw = selem->clockwise;
				break;
			}
			selem->clockwise = newcw;

			break;
		case SHDINFSCOPE:
			for( scno = 0; scno < selem->scopenum; scno++ ){
				curis = *( selem->ppscope + scno );
				_ASSERT( curis );

				switch( curis->type ){
				case SHDPOLYMESH:
					pm = curis->polymesh;
					if( pm ){
						ret = pm->ConvSymmXShape();
						if( ret ){
							DbgOut( "sh : ConvSymmXShape : pm ConvSymmXShape error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}					
					break;
				case SHDPOLYMESH2:
					pm2 = curis->polymesh2;
					if( pm2 ){
						ret = pm2->ConvSymmXShape();
						if( ret ){
							DbgOut( "sh : ConvSymmXShape : pm2 ConvSymmXShape error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}
					break;
				default:
					break;
				}
			}

			switch( selem->clockwise ){
			case 1:
				newcw = 2;
				break;
			case 2:
				newcw = 1;
				break;
			default:
				newcw = selem->clockwise;
				break;
			}
			selem->clockwise = newcw;

			break;

		default:
			break;
		}


		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			partptr = selem->part;
			if( !partptr ){
				DbgOut( "sh : ConvSymmXShape : partptr NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = partptr->ConvSymmXShape();
			if( ret ){
				DbgOut( "sh : ConvSymmXShape : part ConvSymmXShape error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}
	
	return 0;
}

int CShdHandler::ChangeJointInitialPos( int jointno, D3DXVECTOR3 target )
{
	int ret;

	if( jointno < 0 || jointno >= s2shd_leng ){
		DbgOut( "sh : ChangeJointInitialPos : jointno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* jointelem;
	jointelem = (*this)( jointno );
	_ASSERT( jointelem );


	if( !jointelem->IsJoint() || (jointelem->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 0;
	}

//jointlocのセット
	CPart* partptr;
	partptr = jointelem->part;
	if( !partptr ){
		DbgOut( "sh ChangeJointInitialPos : partptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	partptr->jointloc.x = target.x;
	partptr->jointloc.y = target.y;
	partptr->jointloc.z = target.z;

//d3ddispの更新
	CD3DDisp* dispptr;
	dispptr = jointelem->d3ddisp;
	if( !dispptr ){
		DbgOut( "sh ChangeJointInitialPos : dispptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	_ASSERT( dispptr->m_skinv );
	dispptr->m_skinv->pos[0] = target.x;
	dispptr->m_skinv->pos[1] = target.y;
	dispptr->m_skinv->pos[2] = target.z;

//ppBIの更新
	int serino;
	CShdElem* selem;
	for( serino = 0; serino < s2shd_leng; serino++ ){
		selem = (*this)( serino );
		_ASSERT( selem );

		if( selem->IsJoint() || (selem->type != SHDMORPH) ){
			ret = selem->RestoreBoneInfo( this );
			if( ret ){
				DbgOut( "sh ChangeJointInitialPos : selem RestoreBoneInfo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}


int CShdHandler::FindNearestBoneFromAnchor( CShdElem* srcanchor, CShdElem** ppnearbone )
{
	*ppnearbone = 0;

	if( (srcanchor->type != SHDPOLYMESH2) || (srcanchor->m_anchorflag == 0) ){
		DbgOut( "sh : FindNearestBoneFromAnchor : anchortype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	CPolyMesh2* pm2;
	pm2 = srcanchor->polymesh2;
	if( !pm2 ){
		DbgOut( "sh : FindNearestBoneFromAnchor : pm2 NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 anccenter = pm2->m_center;

	float curdist;
	float mindist = 1e6;
	CShdElem* nearbone = 0;

	int serino;

	CShdElem* selem;
	CPart* part;
	CVec3f loc;

	CShdElem* parelem;
	CPart* parpart;
	CVec3f parloc;

	D3DXVECTOR3 bonecenter;
	D3DXVECTOR3 diffvec;

	for( serino = 0; serino < s2shd_leng; serino++ ){
		selem = (*this)( serino );

		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			parelem = FindUpperJoint( selem, 0 );
			if( parelem ){
				
				part = selem->part;
				parpart = parelem->part;
				if( !part || !parpart ){
					DbgOut( "sh : FindNearearBoneFromAnchor : part NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				loc = part->jointloc;
				parloc = parpart->jointloc;

				bonecenter.x = ( loc.x + parloc.x ) * 0.5f;
				bonecenter.y = ( loc.y + parloc.y ) * 0.5f;
				bonecenter.z = ( loc.z + parloc.z ) * 0.5f;

				diffvec = bonecenter - anccenter;

				curdist = D3DXVec3Length( &diffvec );

				if( curdist <= mindist ){
					nearbone = selem;//!!!!!!!!!!
					mindist = curdist;
				}
			}
		}
	}

	*ppnearbone = nearbone;

	return 0;
}



int CShdHandler::SetMikoAnchorApply( CTreeHandler2* thandler )
{

	//if( m_bonetype != BONETYPE_MIKO ){
	//	return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//}

	int ret;
	int serino;
	CShdElem* selem;
	CTreeElem2* telem;
	CPolyMesh2* pm2;
	CPolyMesh* pm;
	int isancname;

	char targetname[1024];
	char applyname[1024];

	int targetseri;
	int applyseri;

	CShdElem* targetelem;
	CShdElem* applyelem;

	for( serino = 0; serino < s2shd_leng; serino++ ){
		telem = (*thandler)( serino );
		selem = (*this)( serino );

		if( (selem->m_anchorflag) && (selem->type == SHDPOLYMESH2) ){

			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "sh  : SetMikoAnchorApply : pm2 NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			isancname = telem->IsAnchorName( targetname, applyname );
			
			if( isancname && *targetname && *applyname ){

				targetseri = -1;
				ret = thandler->GetPartNoByName( targetname, &targetseri );
				if( ret ){
					DbgOut( "sh  : SetMikoAnchorApply : th GetPartNoByName target error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( targetseri >= 0 ){
					targetelem = (*this)( targetseri );
					if( targetelem ){
						pm2->m_mikotarget = targetelem;
					}else{
						pm2->m_mikotarget = 0;
						selem->m_anchorflag = 0;//!!!!!!!!!!!!!! アンカー取り消し。
					}
				}else{
					pm2->m_mikotarget = 0;
					selem->m_anchorflag = 0;//!!!!!!!!!!!!!! アンカー取り消し。
				}


				applyseri = -1;
				ret = thandler->GetBoneNoByName( applyname, &applyseri, this, 0 );
				if( ret ){
					DbgOut( "sh  : SetMikoAnchorApply : th GetBoneNoByName apply error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( applyseri >= 0 ){
					applyelem = (*this)( applyseri );
					if( applyelem && applyelem->IsJoint() && (applyelem->type != SHDMORPH) ){
						pm2->m_mikoapplychild = applyelem;
					}else{
						pm2->m_mikoapplychild = 0;
						selem->m_anchorflag = 0;//!!!!!!!!!!!!!! アンカー取り消し。
					}
				}else{
					pm2->m_mikoapplychild = 0;
					selem->m_anchorflag = 0;//!!!!!!!!!!!!!! アンカー取り消し。
				}


if( (pm2->m_mikotarget) && (pm2->m_mikoapplychild) ){
	DbgOut( "sh  : SetMikoAnchorApply : anc %s, target %s, apply %s\r\n", 
		telem->name,
		(*thandler)( targetseri )->name,
		(*thandler)( applyseri )->name );
}else{
	DbgOut( "sh  : SetMikoAnchorApply : anc %s, no target %x or no apply %x warning !!!\n", 
		telem->name, (pm2->m_mikotarget == 0), (pm2->m_mikoapplychild == 0) );
	//DbgOut( "targetname %s\r\n", targetname );
}

			}
		}else if( (selem->m_anchorflag == 0) && (selem->type == SHDPOLYMESH2) ){
			ret = telem->GetDispObjApplyName( applyname );
			if( ret ){
				DbgOut( "sh  : SetMikoAnchorApply : te GetDispObjApplyName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			pm2 = selem->polymesh2;
			if( !pm2 ){
				DbgOut( "sh  : SetMikoAnchorApply : pm2 NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			if( *applyname ){

				applyseri = -1;
				ret = thandler->GetBoneNoByName( applyname, &applyseri, this, 0 );
				if( ret ){
					DbgOut( "sh  : SetMikoAnchorApply : th GetBoneNoByName apply error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( applyseri >= 0 ){
					applyelem = (*this)( applyseri );
					if( applyelem && applyelem->IsJoint() && (applyelem->type != SHDMORPH) ){
						pm2->m_mikoapplychild = applyelem;
						selem->noskinning = 1;//!!!!!!!!!!!!!!!
					}else{
						pm2->m_mikoapplychild = 0;
						selem->noskinning = 0;//!!!!!!!!!!!!!!!
					}
				}else{
					pm2->m_mikoapplychild = 0;
					selem->noskinning = 0;//!!!!!!!!!!!!!!!
				}
				
			}else{
				pm2->m_mikoapplychild = 0;

				if( m_bonetype == BONETYPE_MIKO ){
					if( (selem->m_mikodef == MIKODEF_SDEF) || (selem->m_mikodef == MIKODEF_BDEF) )
						selem->noskinning = 0;//!!!!!!!!!!!!!!!
					else
						selem->noskinning = 1;//!!!!!!!!!!!!!!!
				}else{
					//そのまま
				}

			}

if( pm2->m_mikoapplychild ){
	DbgOut( "sh  : SetMikoAnchorApply : disp obj %s, apply %s\r\n", telem->name, (*thandler)( applyseri )->name );
}else{
	DbgOut( "sh  : SetMikoAnchorApply : disp obj %s, no fixed apply\r\n", telem->name );
}

		}else if( (selem->m_anchorflag == 0) && (selem->type == SHDPOLYMESH) ){
			ret = telem->GetDispObjApplyName( applyname );
			if( ret ){
				DbgOut( "sh  : SetMikoAnchorApply : te GetDispObjApplyName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			pm = selem->polymesh;
			if( !pm ){
				DbgOut( "sh  : SetMikoAnchorApply : pm NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			if( *applyname ){

				applyseri = -1;
				ret = thandler->GetBoneNoByName( applyname, &applyseri, this, 0 );
				if( ret ){
					DbgOut( "sh  : SetMikoAnchorApply : th GetBoneNoByName apply error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				if( applyseri >= 0 ){
					applyelem = (*this)( applyseri );
					if( applyelem && applyelem->IsJoint() && (applyelem->type != SHDMORPH) ){
						pm->m_mikoapplychild = applyelem;
						selem->noskinning = 1;//!!!!!!!!!!!!!!!
					}else{
						pm->m_mikoapplychild = 0;
						selem->noskinning = 0;//!!!!!!!!!!!!!!!
					}
				}else{
					pm->m_mikoapplychild = 0;
					selem->noskinning = 0;//!!!!!!!!!!!!!!!
				}
				
			}else{
				pm->m_mikoapplychild = 0;

				//if( (m_bonetype != BONETYPE_MIKO) || (selem->m_mikodef == MIKODEF_SDEF) || (selem->m_mikodef == MIKODEF_BDEF) )
				//	selem->noskinning = 0;//!!!!!!!!!!!!!!!
				//else
				//	selem->noskinning = 1;//!!!!!!!!!!!!!!!
			}

if( pm->m_mikoapplychild ){
	DbgOut( "sh  : SetMikoAnchorApply : disp obj %s, apply %s\r\n", telem->name, (*thandler)( applyseri )->name );
}else{
	DbgOut( "sh  : SetMikoAnchorApply : disp obj %s, no fixed apply\r\n", telem->name );
}

		}


	}

	return 0;
}

int CShdHandler::SetUVTile( int partno, int texrule, int unum, int vnum, int tileno )
{
	int ret;

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : SetUVTile : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetUVTile : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	

	CPolyMesh* pm;
	CPolyMesh2* pm2;

	pm = selem->polymesh;
	pm2 = selem->polymesh2;

	if( selem->type == SHDPOLYMESH ){
		if( !pm ){
			DbgOut( "shandler : SetUVTile : pm NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( pm->uvbuf ){
			ret = pm->SetUVTile( texrule, unum, vnum, tileno );
			if( ret ){
				DbgOut( "shandler : SetUVTile : pm SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!

		}else{
			ret = pm->CreateTextureBuffer();
			if( ret ){
				DbgOut( "shandler : SetUVTile ; pm CreateTextureBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm->CopyUVFromD3DDisp( d3ddispptr );
			if( ret ){
				DbgOut( "shandler : SetUVTile : pm CopyUVFromD3DDisp error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!


			ret = pm->SetUVTile( texrule, unum, vnum, tileno );
			if( ret ){
				DbgOut( "shandler : SetUVTile : pm SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}else if( selem->type == SHDPOLYMESH2 ){
		if( !pm2 ){
			DbgOut( "shandler : SetUVTile : pm2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = pm2->SetUVTile( texrule, unum, vnum, tileno );
		if( ret ){
			DbgOut( "shandler : SetUVTile : pm2 SetUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!

	}

////////
	
	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

		ret = d3ddispptr->SetUV( selem->type, selem->texrule, pm2, pm, -1 );
		if( ret ){
			DbgOut( "sh : SetUVTile : d3ddisp SetUVTile error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = d3ddispptr->CopyUV2VertexBuffer( -1 );
		if( ret ){
			DbgOut( "sh : SetUVTile : d3ddisp CopyUV2VertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CShdHandler::SetUVBatchUV( int partno, int* vertnoptr, int setnum, UV* uvptr, int setflag, int clampflag )
{
	int ret;

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : SetUVBatchUV : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetUV : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	int vertindex;
	int vertno;
	float fu, fv;
	for( vertindex = 0; vertindex < setnum; vertindex++ ){
		vertno = *( vertnoptr + vertindex );
		fu = ( uvptr + vertindex )->u;
		fv = ( uvptr + vertindex )->v;

		if( (vertno >= 0) && (vertno < vertnum) ){
			CPolyMesh* pm;
			CPolyMesh2* pm2;

			pm = selem->polymesh;
			pm2 = selem->polymesh2;

			if( selem->type == SHDPOLYMESH ){
				if( !pm ){
					DbgOut( "shandler : SetUV : pm NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( pm->uvbuf ){
					ret = pm->SetUV( vertno, fu, fv, setflag, clampflag );
					if( ret ){
						DbgOut( "shandler : SetUV : pm SetUV error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!

				}else{
					ret = pm->CreateTextureBuffer();
					if( ret ){
						DbgOut( "shandler : SetUV ; pm CreateTextureBuffer error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					ret = pm->CopyUVFromD3DDisp( d3ddispptr );
					if( ret ){
						DbgOut( "shandler : SetUV : pm CopyUVFromD3DDisp error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!


					ret = pm->SetUV( vertno, fu, fv, setflag, clampflag );
					if( ret ){
						DbgOut( "shandler : SetUV : pm SetUV error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

				}

				ret = d3ddispptr->SetUV( selem->type, selem->texrule, pm2, pm, vertno );
				if( ret ){
					DbgOut( "sh : SetUV : d3ddisp SetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


			}else if( selem->type == SHDPOLYMESH2 ){
				if( !pm2 ){
					DbgOut( "shandler : SetUV : pm2 NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = pm2->SetUV( vertno, fu, fv, setflag, clampflag );
				if( ret ){
					DbgOut( "shandler : SetUV : pm2 SetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!

				ret = d3ddispptr->SetUV( selem->type, selem->texrule, pm2, pm, vertno );
				if( ret ){
					DbgOut( "sh : SetUV : d3ddisp SetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

////////
	
	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = d3ddispptr->CopyUV2VertexBufferBatch( vertnoptr, setnum );
		if( ret ){
			DbgOut( "sh : SetUVBatch : d3ddisp CopyUV2VertexBufferBatch error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;

}


int CShdHandler::SetUVBatchDouble( int partno, int* vertnoptr, int setnum, double* uvptr, int setflag, int clampflag )
{

	int ret;

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : SetUVBatchDouble : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetUV : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	int vertindex;
	int vertno;
	float fu, fv;
	for( vertindex = 0; vertindex < setnum; vertindex++ ){
		vertno = *( vertnoptr + vertindex );
		fu = (float)*( uvptr + vertindex * 2 );
		fv = (float)*( uvptr + vertindex * 2 + 1 );

		if( (vertno >= 0) && (vertno < vertnum) ){
			CPolyMesh* pm;
			CPolyMesh2* pm2;

			pm = selem->polymesh;
			pm2 = selem->polymesh2;

			if( selem->type == SHDPOLYMESH ){
				if( !pm ){
					DbgOut( "shandler : SetUV : pm NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( pm->uvbuf ){
					ret = pm->SetUV( vertno, fu, fv, setflag, clampflag );
					if( ret ){
						DbgOut( "shandler : SetUV : pm SetUV error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!

				}else{
					ret = pm->CreateTextureBuffer();
					if( ret ){
						DbgOut( "shandler : SetUV ; pm CreateTextureBuffer error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					ret = pm->CopyUVFromD3DDisp( d3ddispptr );
					if( ret ){
						DbgOut( "shandler : SetUV : pm CopyUVFromD3DDisp error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!


					ret = pm->SetUV( vertno, fu, fv, setflag, clampflag );
					if( ret ){
						DbgOut( "shandler : SetUV : pm SetUV error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

				}

				ret = d3ddispptr->SetUV( selem->type, selem->texrule, pm2, pm, vertno );
				if( ret ){
					DbgOut( "sh : SetUV : d3ddisp SetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


			}else if( selem->type == SHDPOLYMESH2 ){
				if( !pm2 ){
					DbgOut( "shandler : SetUV : pm2 NULL error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = pm2->SetUV( vertno, fu, fv, setflag, clampflag );
				if( ret ){
					DbgOut( "shandler : SetUV : pm2 SetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!

				ret = d3ddispptr->SetUV( selem->type, selem->texrule, pm2, pm, vertno );
				if( ret ){
					DbgOut( "sh : SetUV : d3ddisp SetUV error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}

////////
	
	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = d3ddispptr->CopyUV2VertexBufferBatch( vertnoptr, setnum );
		if( ret ){
			DbgOut( "sh : SetUVBatch : d3ddisp CopyUV2VertexBufferBatch error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}



int CShdHandler::SetUV( int partno, int vertno, float fu, float fv, int setflag, int clampflag, int srcupdateflag )
{
	int ret;

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : SetUV : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : SetUV : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	if( vertno < 0 ){
		int vno;
		for( vno = 0; vno < vertnum; vno++ ){
			int updateflag;
			if( vno != (vertnum - 1) ){
				updateflag = 0;
			}else{
				updateflag = 2;
			}
			ret = SetUV( partno, vno, fu, fv, setflag, clampflag, updateflag );
			if( ret ){
				DbgOut( "shandler : SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;//!!!!!!!!!
	}


//	if( (vertno < 0) || (vertno >= vertnum) ){
	if( vertno >= vertnum ){
		DbgOut( "shdhandler : SetUV : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!		
	}

	CPolyMesh* pm;
	CPolyMesh2* pm2;

	pm = selem->polymesh;
	pm2 = selem->polymesh2;

	if( selem->type == SHDPOLYMESH ){
		if( !pm ){
			DbgOut( "shandler : SetUV : pm NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( pm->uvbuf ){
			ret = pm->SetUV( vertno, fu, fv, setflag, clampflag );
			if( ret ){
				DbgOut( "shandler : SetUV : pm SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!

		}else{
			ret = pm->CreateTextureBuffer();
			if( ret ){
				DbgOut( "shandler : SetUV ; pm CreateTextureBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm->CopyUVFromD3DDisp( d3ddispptr );
			if( ret ){
				DbgOut( "shandler : SetUV : pm CopyUVFromD3DDisp error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!


			ret = pm->SetUV( vertno, fu, fv, setflag, clampflag );
			if( ret ){
				DbgOut( "shandler : SetUV : pm SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}else if( selem->type == SHDPOLYMESH2 ){
		if( !pm2 ){
			DbgOut( "shandler : SetUV : pm2 NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = pm2->SetUV( vertno, fu, fv, setflag, clampflag );
		if( ret ){
			DbgOut( "shandler : SetUV : pm2 SetUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		selem->texrule = TEXRULE_MQ;//!!!!!!!!!!!!!!!!!!!!!!!

	}

////////
	
	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

		ret = d3ddispptr->SetUV( selem->type, selem->texrule, pm2, pm, vertno );
		if( ret ){
			DbgOut( "sh : SetUV : d3ddisp SetUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( srcupdateflag == 1 ){
			ret = d3ddispptr->CopyUV2VertexBuffer( vertno );
			if( ret ){
				DbgOut( "sh : SetUV : d3ddisp CopyUV2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else if( srcupdateflag == 2 ){
			ret = d3ddispptr->CopyUV2VertexBuffer( -1 );
			if( ret ){
				DbgOut( "sh : SetUV : d3ddisp CopyUV2VertexBuffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdHandler::GetUV( int partno, int vertno, float* uptr, float* vptr )
{

	int ret;

	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "shdhandler : GetUV : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}

	CShdElem* selem;
	selem = (*this)( partno );

	CD3DDisp* d3ddispptr;
	d3ddispptr = selem->d3ddisp;
	
	if( !d3ddispptr ){
		DbgOut( "shdhandler : GetUV : 3dobject not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!
	}
	
	int vertnum;
	vertnum = d3ddispptr->m_numTLV;

	if( (vertno < 0) || (vertno >= vertnum) ){
		DbgOut( "shdhandler : GetUV : vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;//!!!		
	}

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){

		ret = d3ddispptr->GetUV( vertno, uptr, vptr );
		if( ret ){
			DbgOut( "sh : SetUV : d3ddisp SetUV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	
	return 0;
}

int CShdHandler::SetTextureDirtyFlag( int srcseri )
{
	int ret;

	if( srcseri >= 0 ){
		if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
			DbgOut( "shandler : SetTextureDirtyFlag : srcseri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*this)( srcseri );

		ret = g_texbnk->SetDirtyFlag( selem->texname, selem->transparent );	
		if( ret ){
			DbgOut( "sh : SetTextureDirtyFlag : texbnk SetDirtyFlag texname error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int texno;
		int extnum;
		extnum = (*this)( srcseri )->exttexnum;
		for( texno = 0; texno < extnum; texno++ ){
			char* extname;
			extname = *( (*this)( srcseri )->exttexname + texno );
			if( extname && (*extname != 0) ){
				ret = g_texbnk->SetDirtyFlag( extname, selem->transparent );
				if( ret ){
					DbgOut( "sh : SetDirtyFlag : texbnk SetDirtyFlag extname error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}else{
		//material
		CMQOMaterial* curmat;
		curmat = m_mathead;
		while( curmat ){

			ret = g_texbnk->SetDirtyFlag( curmat->tex, curmat->transparent );	
			if( ret ){
				DbgOut( "sh : SetTextureDirtyFlag : texbnk SetDirtyFlag texname error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int texno;
			int extnum;
			extnum = curmat->exttexnum;
			for( texno = 0; texno < extnum; texno++ ){
				char* extname;
				extname = *( curmat->exttexname + texno );
				if( extname && (*extname != 0) ){
					ret = g_texbnk->SetDirtyFlag( extname, curmat->transparent );
					if( ret ){
						DbgOut( "sh : SetDirtyFlag : texbnk SetDirtyFlag extname error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			if( curmat->bump[0] != 0 ){
				ret = g_texbnk->SetDirtyFlag( curmat->bump, curmat->transparent );
				if( ret ){
					DbgOut( "sh : SetDirtyFlag : texbnk SetDirtyFlag bump error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			curmat = curmat->next;
		}


		//旧形式
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = SetTextureDirtyFlag( seri );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}

	}
	return 0;
}


int CShdHandler::FillDispColor( int srcseri, DWORD srccol )
{
	int ret;

	if( srcseri < 0 ){
		int serino;
		for( serino = 0; serino < s2shd_leng; serino++ ){
			ret = FillDispColor( serino, srccol );
			if( ret ){
				DbgOut( "shandler : FillDispColor : error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;

	}
////

	if( srcseri >= s2shd_leng ){
		DbgOut( "shandler : FillDispColor : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	_ASSERT( selem );

	int etype = selem->type;
	if( (etype == SHDPOLYMESH) || (etype == SHDPOLYMESH2) || (etype == SHDINFSCOPE) || (etype == SHDBBOX) ){
		ret = selem->FillDispColor( srccol );
		if( ret ){
			DbgOut( "shandler : selem FillDispColor error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::GetVertPos2InfScope( int isindex, CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int vertno, D3DXVECTOR3* dstpos, int scaleflag )
{
	if( (motid < 0) || (motid >= lpmh->m_kindnum) ){
		DbgOut( "shandler : GetVertPos2 : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(lpmh->m_motnum + motid)) ){
		DbgOut( "shandler : GetVertPos2 : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
///////////

	int ret;
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}

	ret = iselem->GetVertPos2InfScope( isindex, lpmh, matWorld, motid, frameno, vertno, dstpos, scaleflag );
	if( ret ){
		DbgOut( "shandler : GetVertPos2InfScope : selem GetVertPos2InfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}


int CShdHandler::GetVertPos2( CMotHandler* lpmh, D3DXMATRIX matWorld, int motid, int frameno, int partno, int vertno, D3DXVECTOR3* dstpos, int scaleflag )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetVertPos2 : partno error %d!!!\n", partno );
		_ASSERT( 0 );
		return 1;
	}

	/***
	if( (motid < 0) || (motid >= lpmh->m_kindnum) ){
		DbgOut( "shandler : GetVertPos2 : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(lpmh->m_motnum + motid)) ){
		DbgOut( "shandler : GetVertPos2 : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

///////////

	int ret;
	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	ret = selem->GetVertPos2( lpmh, matWorld, motid, frameno, vertno, dstpos, scaleflag );
	if( ret ){
		DbgOut( "shandler : GetVertPos2 : selem GetVertPos2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CShdHandler::GetVertPos3( int kind, CMotHandler* lpmh, D3DXMATRIX matWorld, int partno, int vertno, D3DXVECTOR3* vpos )
{
	int ret;

	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetVertPos3 : partno error %d!!!\n", partno );
		_ASSERT( 0 );
		return 1;
	}


	if( kind == 0 ){
		ret = GetVertPos( partno, vertno, vpos );
		if( ret ){
			DbgOut( "sh : GetVertPos3 : GetVertPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{

		CShdElem* selem;
		selem = (*this)( partno );
		_ASSERT( selem );

		ret = selem->GetVertPos3( lpmh, matWorld, vertno, vpos );
		if( ret ){
			DbgOut( "shandler : GetVertPos3 : selem GetVertPos3 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CShdHandler::GetFaceNum( int partno, int* facenum )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetFaceNum : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	ret = selem->GetFaceNum( facenum );
	if( ret ){
		DbgOut( "shandler : GetFaceNum : selem GetFaceNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::GetInfElemNum( int partno, int vertno, int* infnumptr )
{

	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetInfElemNum : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh GetInfElemNum : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : GetInfElemNum vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( (selem->IsJoint()) && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : GetInfElemNum : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		*infnumptr = 0;
		return 1;
	}

	*infnumptr = (ieptr + vertno)->infnum;

	return 0;
}

int CShdHandler::GetInfElemCalcmode( int partno, int vertno, int infno, int* dstcalcmode )
{

	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetInfElemCalcmode : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh GetInfElemCalcmode : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : GetInfElemCalcmode vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : GetInfElemCalcmode : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (infno < 0) || (infno >= (ieptr + vertno)->infnum) ){
		DbgOut( "shandler : GetInfElemCalcmode : infno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	INFELEM* curIE;
	curIE = (ieptr + vertno)->ie + infno;

	*dstcalcmode = curIE->kind;

	return 0;
}

int CShdHandler::SetInfElemCalcmode( int partno, int vertno, int infno, int srccalcmode )
{

	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : SetInfElemCalcmode : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh SetInfElemCalcmode : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : SetInfElemCalcmode vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : SetInfElemCalcmode : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (infno < 0) || (infno >= (ieptr + vertno)->infnum) ){
		DbgOut( "shandler : SetInfElemCalcmode : infno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	INFELEM* curIE;
	curIE = (ieptr + vertno)->ie + infno;

	curIE->kind = srccalcmode;

	return 0;
}


int CShdHandler::GetInfElemParam( int partno, int vertno, int infno, int* bonenoptr, float* rateptr )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetInfElemParam : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh GetInfElemParam : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : GetInfElemParam vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : GetInfElemParam : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (infno < 0) || (infno >= (ieptr + vertno)->infnum) ){
		DbgOut( "shandler : GetInfElemParam : infno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	INFELEM* curIE;
	curIE = (ieptr + vertno)->ie + infno;

	*bonenoptr = curIE->bonematno;
	*rateptr = curIE->dispinf;

	
	return 0;
}

int CShdHandler::GetInfElem( int partno, int vertno, int infno, INFELEM** ppIE )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetInfElem : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh GetInfElem : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : GetInfElem vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : GetInfElem : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (infno < 0) || (infno >= (ieptr + vertno)->infnum) ){
		DbgOut( "shandler : GetInfElem : infno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*ppIE = (ieptr + vertno)->ie + infno;

	return 0;
}

int CShdHandler::GetInfElemParam2( int partno, int vertno, int infno, int* bonenoptr, int* childnoptr, float* rateptr )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetInfElemParam2 : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh GetInfElemParam2 : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : GetInfElemParam2 vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : GetInfElemParam2 : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (infno < 0) || (infno >= (ieptr + vertno)->infnum) ){
		DbgOut( "shandler : GetInfElemParam2 : infno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	INFELEM* curIE;
	curIE = (ieptr + vertno)->ie + infno;

	*bonenoptr = curIE->bonematno;
	*childnoptr = curIE->childno;
	*rateptr = curIE->dispinf;

	
	return 0;
}


int CShdHandler::SetInfElemSymParams( int partno, int vertno, int srcsymaxis, float srcsymdist )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : SetInfElemSymParams : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh SetInfElemSymParams : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : SetInfElemSymParams vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : SetInfElemSymParams : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	(ieptr + vertno)->symaxis = srcsymaxis;
	(ieptr + vertno)->symdist = srcsymdist;


	return 0;
}
int CShdHandler::GetInfElemSymParams( int partno, int vertno, int* dstsymaxis, float* dstsymdist )
{
	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "shandler : GetInfElemSymParams : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int vnum;
	ret = GetVertNumOfPart( partno, &vnum );
	if( ret ){
		DbgOut( "sh GetInfElemSymParams : GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= vnum) ){
		DbgOut( "shandler : GetInfElemSymParams vertno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	CInfElem* ieptr = 0;
	if( selem->type == SHDPOLYMESH ){
		if( selem->polymesh ){
			ieptr = selem->polymesh->m_IE;
		}
	}else if( selem->type == SHDPOLYMESH2 ){
		if( selem->polymesh2 ){
			ieptr = selem->polymesh2->m_IE;
		}
	}else if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->part ){
			ieptr = selem->part->m_IE;
		}
	}else{
		_ASSERT( 0 );
	}

	if( !ieptr ){
		DbgOut( "shandler : GetInfElemSymParams : ieptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstsymaxis = (ieptr + vertno)->symaxis;
	*dstsymdist = (ieptr + vertno)->symdist;

	return 0;
}


int CShdHandler::ChangeColor( CTreeHandler2* dstth, CShdHandler* srcsh, CTreeHandler2* srcth )
{
	int ret;
	int dstseri;
	CShdElem* dstse;
	CShdElem* srcse;
	CTreeElem2* dstte;

	char orgname[4098];
	char orgKname[4098];
	char sdefname[4098];
	char sdefKname[4098];
	char bdefname[4098];
	char bdefKname[4098];

	int leng;
	
	int findseri1;

	int sccnt = 0;

	for( dstseri = 1; dstseri < s2shd_leng; dstseri++ ){
		dstse = (*this)( dstseri );
		_ASSERT( dstse );

		if( (dstse->type == SHDPOLYMESH) || (dstse->type == SHDPOLYMESH2) ){
			dstte = (*dstth)( dstseri );
			_ASSERT( dstte );
			
			leng = (int)strlen( dstte->name );
			if( leng < 4095 ){
				strcpy_s( orgname, 4096, dstte->name );

				strcpy_s( orgKname, 4096, "\"" );
				strcat_s( orgKname, 4096, dstte->name );
				strcat_s( orgKname, 4096, "\"" );

				strcpy_s( sdefname, 4096, "sdef:" );
				strcat_s( sdefname, 4096, dstte->name );
				
				strcpy_s( sdefKname, 4096, "\"sdef:" );
				strcat_s( sdefKname, 4096, dstte->name );
				strcat_s( sdefKname, 4096, "\"" );

				strcpy_s( bdefname, 4096, "bdef:" );
				strcat_s( bdefname, 4096, dstte->name );

				strcpy_s( bdefKname, 4096, "\"bdef:" );
				strcat_s( bdefKname, 4096, dstte->name );
				strcat_s( bdefKname, 4096, "\"" );

				ret = srcth->GetDispObjNoByName( orgname, &findseri1, srcsh, 0 );
				_ASSERT( !ret );

				if( findseri1 < 0 ){
					ret = srcth->GetDispObjNoByName( orgKname, &findseri1, srcsh, 0 );
					_ASSERT( !ret );

					if( findseri1 < 0 ){
						ret = srcth->GetDispObjNoByName( sdefname, &findseri1, srcsh, 0 );
						_ASSERT( !ret );

						if( findseri1 < 0 ){
							ret = srcth->GetDispObjNoByName( sdefKname, &findseri1, srcsh, 0 );
							_ASSERT( !ret );

							if( findseri1 < 0 ){
								ret = srcth->GetDispObjNoByName( bdefname, &findseri1, srcsh, 0 );
								_ASSERT( !ret );

								if( findseri1 < 0 ){
									ret = srcth->GetDispObjNoByName( bdefKname, &findseri1, srcsh, 0 );
									_ASSERT( !ret );
								}
							}
						}
					}
				}

				if( findseri1 >= 0 ){

					DbgOut( "sh : ChangeColor : find same name %s\r\n", dstte->name );

					srcse = (*srcsh)( findseri1 );
					_ASSERT( srcse );

					ret = dstse->ChangeColor( srcse );
					if( ret == 1 ){
						DbgOut( "sh : ChangeColor : se ChangeColor error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}else if( ret == 2 ){
						DbgOut( "sh : ChangeColor : se ChangeColor : params not equal %s\r\n", dstte->name );
					}else if( ret == 0 ){
						DbgOut( "sh : ChangeColor : se ChangeColor : color change is success %s\r\n", dstte->name );
						sccnt++;
					}

				}

			}

		}
	}

	if( sccnt > 0 ){
		m_shader = srcsh->m_shader;
		m_overflow = srcsh->m_overflow;
	}


	return 0;
}
CShdElem* CShdHandler::GetInfScopeElem()
{
	CShdElem* findelem = 0;
	CShdElem* curelem;
	int seri;
	for( seri = 1; seri < s2shd_leng; seri++ ){
		curelem = (*this)( seri );
		_ASSERT( curelem );
		if( curelem->type == SHDINFSCOPE ){
			findelem = curelem;
			break;
		}
	}

	return findelem;
}
CShdElem* CShdHandler::GetInfScopeElem( int offset )
{
	CShdElem* findelem = 0;
	CShdElem* curelem;
	int seri;
	for( seri = 1; seri <= offset; seri++ ){
		curelem = (*this)( seri );
		_ASSERT( curelem );
		if( curelem->type == SHDINFSCOPE ){
			findelem = curelem;
			break;
		}
	}

	return findelem;
}
CShdElem* CShdHandler::GetBBoxElem()
{
	/***
	CShdElem* findelem = 0;
	CShdElem* curelem;
	int seri;
	for( seri = 1; seri < s2shd_leng; seri++ ){
		curelem = (*this)( seri );
		_ASSERT( curelem );
		if( curelem->type == SHDBBOX ){
			findelem = curelem;
			break;
		}
	}
	return findelem;
	***/

	return m_bbxelem;

}

CShdElem* CShdHandler::GetBBoxElem( int offset )
{
	/***
	CShdElem* findelem = 0;
	CShdElem* curelem;
	int seri;
	for( seri = 1; seri <= offset; seri++ ){
		curelem = (*this)( seri );
		_ASSERT( curelem );
		if( curelem->type == SHDBBOX ){
			findelem = curelem;
			break;
		}
	}
	return findelem;
	***/
	return m_bbxelem;
}


int CShdHandler::MeshConv2InfScope( CTreeHandler2* lpth, CMotHandler* lpmh, int srcboneno, int srcdispno, int srcconvno )
{
	int ret;
	if( (srcboneno <= 0) || (srcboneno >= s2shd_leng) || (srcdispno <= 0) || (srcdispno >= s2shd_leng) || (srcconvno <= 0) || (srcconvno >= s2shd_leng) ){
		DbgOut( "sh : MeshConv2InfScope : parameter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		DbgOut( "sh : MeshConv2InfScope : infscope not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* convelem;
	convelem = (*this)( srcconvno );
	_ASSERT( convelem );

	CShdElem* applychild;
	applychild = (*this)( srcboneno );
	_ASSERT( applychild );

	CShdElem* target;
	target = (*this)( srcdispno );
	_ASSERT( srcdispno );

	if( (convelem->type != SHDPOLYMESH) && (convelem->type != SHDPOLYMESH2) ){
		DbgOut( "sh : MeshConv2InfScope : convelem type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !applychild->IsJoint() || (applychild->type == SHDMORPH) ){
		DbgOut( "sh : MeshConv2InfScope : boneelem type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (target->type != SHDPOLYMESH) && (target->type != SHDPOLYMESH2) ){
		DbgOut( "sh : MeshConv2InfScope : target type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//既に存在していたら、削除
	int isindex;
	isindex = ExistInfScope( srcboneno, srcdispno );
	if( isindex >= 0 ){
		ret = iselem->DeleteInfScopeElemByIndex( isindex );
		if( ret ){
			DbgOut( "sh : MeshConv2InfScope : DeleteInfScopeElemByIndex error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	//変換するメッシュをtargetとしているInfScopeを削除する
	ret = iselem->DeleteInfScopeElemByTarget( srcconvno );
	if( ret ){
		DbgOut( "sh : MeshConv2InfScope : DeleteInfScopeElemByTarget error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	switch( convelem->type ){
	case SHDPOLYMESH:
		convelem->polymesh->m_mikoapplychild = applychild;//!!!!!!!!
		convelem->polymesh->m_mikotarget = target;//!!!!!!!!!!!!

		ret = iselem->AddInfScope( convelem->polymesh, 0 );
		if( ret ){
			DbgOut( "sh : MeshConv2InfScope : pm iselem AddInfScope error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		convelem->polymesh = 0;// freeされないように０セット。
		//convelem->d3ddisp = 0;// freeされないように０セット。

		break;

	case SHDPOLYMESH2:

		convelem->polymesh2->m_mikoapplychild = applychild;//!!!!!!!!
		convelem->polymesh2->m_mikotarget = target;//!!!!!!!!!!!!

		ret = iselem->AddInfScope( convelem->polymesh2, 0, convelem->facet );
		if( ret ){
			DbgOut( "sh : MeshConv2InfScope : pm2 iselem AddInfScope error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		convelem->polymesh2 = 0;// freeされないように０セット。
		//convelem->d3ddisp = 0;// freeされないように０セット。


		break;
	default:
		DbgOut( "sh : MeshConv2InfScope : conv type error !!!\n" );
		_ASSERT( 0 );
		return 1;

		break;
	}

	ret = LeaveOneElemFromChain( srcconvno, lpmh );
	if( ret ){
		DbgOut( "sh : MeshConv2InfScope : pm : LeaveOneElemFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	convelem->DestroyObjs();
	convelem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!


	CMotionCtrl* delmc2;
	delmc2 = (*lpmh)( srcconvno );
	_ASSERT( delmc2 );
	delmc2->type = SHDDESTROYED;//!!!!!!!!!!!!!!


	CTreeElem2* delte2;
	delte2 = (*lpth)( srcconvno );
	_ASSERT( delte2 );
	delte2->type = SHDDESTROYED;//!!!!!!!!!!!!!!!


	return 0;

}


int CShdHandler::Anchor2InfScope( CTreeHandler2* lpth, CMotHandler* lpmh )
{
//	DbgOut( "sh : bef Anchor2InfScope\n" );
	int ret = 0;
	int retseri;

	CShdElem* iselem = 0;
	iselem = GetInfScopeElem();
	if( !iselem ){
		//SHDINFSCOPEがない場合は、作る。

		int serino = s2shd_leng;
		int srctype = SHDINFSCOPE;

		int depth;	
		lpth->Start( serino - 1 );
		lpth->Begin();
		depth = lpth->GetCurDepth();

		////////
		retseri = lpth->AddTree( "InfScope", serino );
		if( retseri != serino ){
			_ASSERT( 0 );
			return 1;
		}

		ret = lpth->SetElemType( serino, srctype );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		//////////////
		ret = AddShdElem( serino, depth );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = SetClockwise( serino, 2 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		//////////////
		ret = lpmh->AddMotionCtrl( serino, depth );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}	
		ret = lpmh->SetHasMotion( serino, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		////////
		lpth->End();

		///////
		CShdElem* curelem;
		curelem = (*this)( serino );
		_ASSERT( curelem );

		CShdElem* topelem;
		topelem = (*this)( 1 );
		_ASSERT( topelem );
		ret = curelem->PutToChain( topelem, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = CalcDepth();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = lpmh->CopyChainFromShd( this );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = lpmh->CalcDepth();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		iselem = curelem;
	}

/////// anchorを見つけて、InfScopeにadd。
	int seri;
	for( seri = 1; seri < s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*this)( seri );
		_ASSERT( selem );

		if( selem->m_anchorflag == 1 ){

			switch( selem->type ){
			case SHDPOLYMESH:
				_ASSERT( 0 );
				break;
			case SHDPOLYMESH2:
					
				if( (selem->polymesh2->m_mikotarget != 0) && (selem->polymesh2->m_mikoapplychild != 0) ){	
					ret = iselem->AddInfScope( selem->polymesh2, 0, selem->facet );
					if( ret ){
						DbgOut( "Anchor2InfScope : pm2 iselem AddInfScope error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					selem->polymesh2 = 0;// freeされないように０セット。
					//selem->d3ddisp = 0;// freeされないように０セット。
				}else{
					//影響ボーン指定１００％の時は、m_mikotargetがみつかっていない（名前に余分な-があるから）
					//このときは、infscopeは作らない。

				}


				/////////////////////
				ret = LeaveOneElemFromChain( seri, lpmh );
				if( ret ){
					DbgOut( "Anchor2InfScope : pm2 : LeaveOneElemFromChain error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				selem->DestroyObjs();
				selem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!
				selem->m_anchorflag = 0;//!!!!!!!!!!!!!!


				CMotionCtrl* delmc2;
				delmc2 = (*lpmh)( seri );
				_ASSERT( delmc2 );
				delmc2->type = SHDDESTROYED;//!!!!!!!!!!!!!!


				CTreeElem2* delte2;
				delte2 = (*lpth)( seri );
				_ASSERT( delte2 );
				delte2->type = SHDDESTROYED;//!!!!!!!!!!!!!!!
				/////////////////////////////

				break;
			default:
				break;
			}

			selem->m_anchorflag = 0;//!!!!!!
		}

	}


	iselem->alpha = 0.5f;//!!!!!!!!!!!!!!!!!!!

//	DbgOut( "sh : aft Anchor2InfScope\n" );

	return 0;
}

int CShdHandler::CreateShdBBoxIfNot( CTreeHandler2* lpth, CMotHandler* lpmh )
{

	if( !m_bbxelem ){
		m_bbxelem = new CShdElem( -1 );// !!! serialno -1 !!!!!
		if( !m_bbxelem ){
			DbgOut( "sh : CreateShdBBoxIfNot : bbxelem alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		m_bbxelem->type = SHDBBOX;
		m_bbxelem->alpha = 0.5f;
		m_bbxelem->clockwise = 2;
	}


	return 0;
}

int CShdHandler::DeletePart( int delno, CTreeHandler2* lpth, CMotHandler* lpmh )
{
	int ret;

	if( (delno <= 1) || (delno >= s2shd_leng) ){
		DbgOut( "sh : DeletePart : delno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* delelem;
	delelem = (*this)( delno );
	_ASSERT( delelem );

	if( delelem->type != SHDPART ){
		DbgOut( "sh : DeletePart : this is not part object return !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!
	}

///////////
	ret = LeaveOneElemFromChain( delno, lpmh );
	if( ret ){
		DbgOut( "sh : DeletePart : LeaveOneElemFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delelem->DestroyObjs();
	delelem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!

////////////
	CMotionCtrl* delmc;
	delmc = (*lpmh)( delno );
	_ASSERT( delmc );
	delmc->type = SHDDESTROYED;//!!!!!!!!!!!!!!

//////////////
	CTreeElem2* delte;
	delte = (*lpth)( delno );
	_ASSERT( delte );
	delte->type = SHDDESTROYED;//!!!!!!!!!!!!!!!

	return 0;
}


int CShdHandler::DeleteDispObj( int delno, CTreeHandler2* lpth, CMotHandler* lpmh )
{
	int ret;

	if( (delno <= 0) || (delno >= s2shd_leng) ){
		DbgOut( "sh : DeleteDispObj : delno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* delelem;
	delelem = (*this)( delno );
	_ASSERT( delelem );

	if( ( delelem->type != SHDPOLYMESH ) && (delelem->type != SHDPOLYMESH2) ){
		DbgOut( "sh : DeleteDispObj : this is not disp object return !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!
	}

///////////
	ret = LeaveOneElemFromChain( delno, lpmh );
	if( ret ){
		DbgOut( "sh : DeleteDispObj : LeaveOneElemFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delelem->DestroyObjs();
	delelem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!

////////////
	CMotionCtrl* delmc;
	delmc = (*lpmh)( delno );
	_ASSERT( delmc );
	delmc->type = SHDDESTROYED;//!!!!!!!!!!!!!!

//////////////
	CTreeElem2* delte;
	delte = (*lpth)( delno );
	_ASSERT( delte );
	delte->type = SHDDESTROYED;//!!!!!!!!!!!!!!!


/////////// InfScopeの要素削除
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( iselem ){
		ret = iselem->DeleteInfScopeElemByTarget( delno );
		if( ret ){
			DbgOut( "sh : DeleteDispObj : is DeleteInfScopeElemByTarget error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
/////////////BBoxの要素削除
	CShdElem* bbxelem;
	bbxelem = GetBBoxElem();
	if( bbxelem ){
		ret = bbxelem->DeleteBBoxByTarget( delelem );
		if( ret ){
			DbgOut( "sh : DeleteDispObj : bbxelem DeleteBBoxByTarget error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}


int CShdHandler::DeleteJoint( int delno, CTreeHandler2* lpth, CMotHandler* lpmh )
{
	int ret;

	if( (delno <= 0) || (delno >= s2shd_leng) ){
		DbgOut( "sh : DeleteJoint : delno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* delelem;
	delelem = (*this)( delno );
	_ASSERT( delelem );

	if( !( delelem->IsJoint() ) || (delelem->type == SHDMORPH) ){
		DbgOut( "sh : DeleteJoint : this is not joint object return !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!
	}

///////////
	ret = LeaveOneElemFromChain( delno, lpmh );
	if( ret ){
		DbgOut( "sh : DeleteJoint : LeaveOneElemFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delelem->DestroyObjs();
	delelem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!

////////////
	CMotionCtrl* delmc;
	delmc = (*lpmh)( delno );
	_ASSERT( delmc );
	delmc->type = SHDDESTROYED;//!!!!!!!!!!!!!!

//////////////
	CTreeElem2* delte;
	delte = (*lpth)( delno );
	_ASSERT( delte );
	delte->type = SHDDESTROYED;//!!!!!!!!!!!!!!!


//////////// 対応アンカーの削除と、NOSKINの影響ボーン番号のリセット。
	
	int seri;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		CShdElem* chkelem;
		chkelem = (*this)( seri );
		_ASSERT( chkelem );

		if( (chkelem->type == SHDPOLYMESH2) || (chkelem->type == SHDPOLYMESH) ){
			CPolyMesh2* pm2;
			pm2 = chkelem->polymesh2;
			CPolyMesh* pm;
			pm = chkelem->polymesh;


			int applychild;
			if( pm2 ){
				if( pm2->m_mikoapplychild ){
					applychild = pm2->m_mikoapplychild->serialno;
				}else{
					applychild = -1;
				}
					
				if( applychild == delno ){
					pm2->m_mikoapplychild = 0;//!!!!!!!!!!!!!!!  影響ボーンのリセット
				}
			}else if( pm ){
				if( pm->m_mikoapplychild ){
					applychild = pm->m_mikoapplychild->serialno;
				}else{
					applychild = -1;
				}
					
				if( applychild == delno ){
					pm->m_mikoapplychild = 0;//!!!!!!!!!!!!!!!  影響ボーンのリセット
				}
			}

			if( (chkelem->m_anchorflag == 1) && (delno == applychild) ){
		
				ret = LeaveOneElemFromChain( seri, lpmh );
				if( ret ){
					DbgOut( "sh : DeleteJoint : LeaveOneElemFromChain error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				chkelem->DestroyObjs();
				chkelem->type = SHDDESTROYED;//!!!!!!!!!!!!!!!!!!
				chkelem->m_anchorflag = 0;//!!!!!!!!!!!!!!


				CMotionCtrl* delmc2;
				delmc2 = (*lpmh)( seri );
				_ASSERT( delmc2 );
				delmc2->type = SHDDESTROYED;//!!!!!!!!!!!!!!


				CTreeElem2* delte2;
				delte2 = (*lpth)( seri );
				_ASSERT( delte2 );
				delte2->type = SHDDESTROYED;//!!!!!!!!!!!!!!!

			}

		}
	}

/////////// InfScopeの要素削除
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( iselem ){
		ret = iselem->DeleteInfScopeElemByApplyChild( delno );
		if( ret ){
			DbgOut( "sh : DeleteJoint : is DeleteInfScopeElemByApplyChild error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


//////////// 影響ボーン番号の削除と作り直し。
	for( seri = 0; seri < s2shd_leng; seri++ ){
		CShdElem* chkelem2;
		chkelem2 = (*this)( seri );
		_ASSERT( chkelem2 );

		if( (chkelem2->type == SHDPOLYMESH) || (chkelem2->type == SHDPOLYMESH2) ){
			ret = chkelem2->RemoveInfluenceNo( delno );
			if( ret ){
				DbgOut( "sh : DeleteJoint : RemoveInfluenceNo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}


	}


	return 0;
}

int CShdHandler::LeaveOneElemFromChain( int delno, CMotHandler* lpmh )
{

	int ret;

	if( (delno <= 0) || (delno >= s2shd_leng) ){
		DbgOut( "sh : LeaveOneElemFromChain : delno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* delelem;
	delelem = (*this)( delno );
	_ASSERT( delelem );


	CShdElem* topelem;
	topelem = (*this)( 1 );
	_ASSERT( topelem );

	CShdElem* parelem;
	parelem = delelem->parent;
	if( !parelem ){
		parelem = topelem;
	}

	CShdElem* chilelem;
	chilelem = delelem->child;


	// chainし直し。

	ret = delelem->LeaveFromChain();
	if( ret ){
		DbgOut( "sh : LeaveOneElemFromChain : selem LeaveFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	delelem->child = 0;

	ret = delelem->PutToChain( topelem, 1 );
	if( ret ){
		DbgOut( "sh : LeaveOneElemFromChain : delelem PutToChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( parelem && chilelem ){
		ret = chilelem->PutToChain( parelem, 1 );
		if( ret ){
			DbgOut( "sh : LeaveOneElemFromChain : selem PutToChain error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = CalcDepth();
	if( ret ){
		DbgOut( "sh : LeaveOneElemFromChain : shandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = lpmh->CopyChainFromShd( this );
	if( ret ){
		DbgOut( "sh : LeaveOneElemFromChain : mhandler CopyChainFromShd error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpmh->CalcDepth();
	if( ret ){
		DbgOut( "sh : LeaveOneElemFromChain : mhandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CShdHandler::InfScopeConvTempApplyChild()
{
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( iselem ){
		int scno;
		for( scno = 0; scno < iselem->scopenum; scno++ ){
			CInfScope* curis;
			curis = *( iselem->ppscope + scno );
			_ASSERT( curis );

			CPolyMesh* pm;
			CPolyMesh2* pm2;

			switch( curis->type ){
			case SHDPOLYMESH:
				pm = curis->polymesh;
				_ASSERT( pm );
				if( pm->tempapplychild ){
					curis->applychild = (*this)( pm->tempapplychild );
					pm->tempapplychild = 0;
				}
				if( pm->temptarget ){
					curis->target = (*this)( pm->temptarget );
					pm->temptarget = 0;
				}
				break;
			case SHDPOLYMESH2:
				pm2 = curis->polymesh2;
				_ASSERT( pm2 );
				if( pm2->tempapplychild ){
					curis->applychild = (*this)( pm2->tempapplychild );
					pm2->tempapplychild = 0;
				}
				if( pm2->temptarget ){
					curis->target = (*this)( pm2->temptarget );
					pm2->temptarget = 0;
				}
				break;
			default:
				break;
			}
		}
	}

	return 0;
}

int CShdHandler::ExistInfScope( int srcboneno, int srcdispno )
{
	int retno = -1;

	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return -1;
	}

	int scno;
	for( scno = 0; scno < iselem->scopenum; scno++ ){
		CInfScope* curis;
		curis = *( iselem->ppscope + scno );
		_ASSERT( curis );

		if( (curis->applychild) && (curis->applychild->serialno == srcboneno) 
			&& (curis->target) && (curis->target->serialno == srcdispno) ){
			
			retno = scno;
			break;
		}
	}

	return retno;
}

int CShdHandler::SetInfScopeDispFlag( int srcindex, int srcflag, int exclusiveflag )
{
	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		_ASSERT( 0 );
		return 1;
	}


	if( srcindex < 0  ){

		int isno;
		for( isno = 0; isno < iselem->scopenum; isno++ ){
			SetInfScopeDispFlag( isno, srcflag, exclusiveflag );
		}

	}else{

		if( srcindex >= iselem->scopenum ){
			DbgOut( "sh : SetInfScopeDispFlag : index error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		int invflag;
		invflag = !srcflag;

		CInfScope* curis;
		if( exclusiveflag == 0 ){
			curis = *( iselem->ppscope + srcindex );
			curis->dispflag = srcflag;
		}else{
			int isno;
			for( isno = 0; isno < iselem->scopenum; isno++ ){
				curis = *( iselem->ppscope + isno );

				if( isno == srcindex ){
					curis->dispflag = srcflag;
				}else{
					curis->dispflag = invflag;
				}
			}
		}

	}
	return 0;
}

int CShdHandler::SetMikoBonePos( int boneno, D3DXVECTOR3 bdir )
{
	int ret;

	if( (boneno <= 0) || (boneno >= s2shd_leng) ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* childjoint;
	childjoint = (*this)( boneno );
	if( !childjoint->IsJoint() || (childjoint->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* parjoint;
	parjoint = FindUpperJoint( childjoint );
	if( !parjoint ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}


	CPart* partptr;
	partptr = parjoint->part;
	if( !partptr ){
		_ASSERT( 0 );
		return 1;
	}

	CBoneInfo* biptr = 0;
	ret = partptr->GetBoneInfo( boneno, &biptr );
	if( ret || !biptr ){
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 startpos;
	startpos.x = biptr->bonestart.x;
	startpos.y = biptr->bonestart.y;
	startpos.z = biptr->bonestart.z;
	

	childjoint->m_setmikobonepos = 2;
	//mikoto format
	childjoint->m_mikobonepos = bdir;

	return 0;
}




int CShdHandler::GetInitialBoneQ( int childno, CQuaternion* dstq )
{

	int ret;

	if( (childno <= 0) || (childno >= s2shd_leng) ){
		DbgOut( "sh : GetInitialBoneQ : childno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* childjoint;
	childjoint = (*this)( childno );
	if( !childjoint->IsJoint() || (childjoint->type == SHDMORPH) ){
		DbgOut( "sh : GetInitialBoneQ : applychild type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* parjoint;
	parjoint = FindUpperJoint( childjoint );
	if( !parjoint ){
		//DbgOut( "sh : GetBone3Vec : parjoint NULL error !!!\n" );
		//_ASSERT( 0 );
		//return 1;

		dstq->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}


	CPart* partptr;
	partptr = parjoint->part;
	if( !partptr ){
		DbgOut( "sh : GetInitialBoneQ : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneInfo* biptr = 0;
	ret = partptr->GetBoneInfo( childno, &biptr );
	if( ret || !biptr ){
		DbgOut( "sh : GetInitialBoneQ : GetBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXMATRIX matview, invmatview;
	D3DXVECTOR3 startpos, endpos, upvec;

	D3DXVECTOR3 vecx0, vecy0, vecz0;
	D3DXVECTOR3 vecx1, vecy1, vecz1;


	startpos.x = biptr->bonestart.x;
	startpos.y = biptr->bonestart.y;
	startpos.z = biptr->bonestart.z;
	
	endpos.x = biptr->boneend.x;
	endpos.y = biptr->boneend.y;
	endpos.z = biptr->boneend.z;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	D3DXVECTOR3 bonevec;
	bonevec.x = biptr->bonevec.x;
	bonevec.y = biptr->bonevec.y;
	bonevec.z = biptr->bonevec.z;

	
//zaxis = normal(At - Eye)
//xaxis = normal(cross(Up, zaxis))
//yaxis = cross(zaxis, xaxis)


	if( childjoint->m_setmikobonepos == 0 ){

		if( (bonevec.x != 0.0f) || (bonevec.y != 0.0f) ){
			upvec.x = 0.0f;
			upvec.y = 0.0f;
			upvec.z = 1.0f;
		}else{
			upvec.x = 1.0f;
			upvec.y = 0.0f;
			upvec.z = 0.0f;
		}

		vecz1 = bonevec;
		
		D3DXVec3Cross( &vecx1, &upvec, &vecz1 );
		D3DXVec3Normalize( &vecx1, &vecx1 );

		D3DXVec3Cross( &vecy1, &vecz1, &vecx1 );
		D3DXVec3Normalize( &vecy1, &vecy1 );


	}else if( childjoint->m_setmikobonepos == 1 ){
		//mikoto format

		vecz1 = bonevec;

		D3DXVECTOR3 hvec;
		hvec = childjoint->m_mikobonepos - startpos;
		D3DXVec3Cross( &vecy1, &bonevec, &hvec );
		D3DXVec3Normalize( &vecy1, &vecy1 );

		D3DXVec3Cross( &vecx1, &vecy1, &vecz1 );
		D3DXVec3Normalize( &vecx1, &vecx1 );
	}else{
		vecz1 = bonevec;

		vecy1 = childjoint->m_mikobonepos;

		D3DXVec3Cross( &vecx1, &vecy1, &vecz1 );
		D3DXVec3Normalize( &vecx1, &vecx1 );

		D3DXVec3Cross( &vecy1, &vecz1, &vecx1 );
		D3DXVec3Normalize( &vecy1, &vecy1 );
	}


	D3DXMATRIX tmpxmat;
	D3DXQUATERNION tmpxq;

	D3DXMatrixIdentity( &tmpxmat );
	tmpxmat._11 = vecx1.x;
	tmpxmat._12 = vecx1.y;
	tmpxmat._13 = vecx1.z;

	tmpxmat._21 = vecy1.x;
	tmpxmat._22 = vecy1.y;
	tmpxmat._23 = vecy1.z;

	tmpxmat._31 = vecz1.x;
	tmpxmat._32 = vecz1.y;
	tmpxmat._33 = vecz1.z;

	D3DXQuaternionRotationMatrix( &tmpxq, &tmpxmat );

	dstq->x = tmpxq.x;
	dstq->y = tmpxq.y;
	dstq->z = tmpxq.z;
	dstq->w = tmpxq.w;

	return 0;
}


/***
int CShdHandler::GetInitialBoneQ( int childno, int motid, int frameno, CMotHandler* lpmh, D3DXMATRIX matWorld, CQuaternion* dstq )
{

	int ret;

	if( (childno <= 0) || (childno >= s2shd_leng) ){
		DbgOut( "sh : GetInitialBoneQ : childno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* childjoint;
	childjoint = (*this)( childno );
	if( !childjoint->IsJoint() || (childjoint->type == SHDMORPH) ){
		DbgOut( "sh : GetInitialBoneQ : applychild type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* parjoint;
	parjoint = FindUpperJoint( childjoint );
	if( !parjoint ){
		//DbgOut( "sh : GetBone3Vec : parjoint NULL error !!!\n" );
		//_ASSERT( 0 );
		//return 1;

		dstq->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	D3DXMATRIX matview, invmatview;
	D3DXVECTOR3 startpos, endpos, upvec;

	D3DXVECTOR3 vecx0, vecy0, vecz0;
	D3DXVECTOR3 vecx1, vecy1, vecz1;

	ret = GetBonePos( parjoint->serialno, 1, motid, frameno, &startpos, lpmh, matWorld, 1 );
	if( ret ){
		DbgOut( "sh : GetInitialBoneQ : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = GetBonePos( childjoint->serialno, 1, motid, frameno, &endpos, lpmh, matWorld, 1 );
	if( ret ){
		DbgOut( "sh : GetInitialBoneQ : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	D3DXVECTOR3 bonevec;
	bonevec = endpos - startpos;
	D3DXVec3Normalize( &bonevec, &bonevec );
	
//zaxis = normal(At - Eye)
//xaxis = normal(cross(Up, zaxis))
//yaxis = cross(zaxis, xaxis)

	if( (bonevec.x != 0.0f) || (bonevec.y != 0.0f) ){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;
	}else{
		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;
	}

	vecz1 = bonevec;
	
	D3DXVec3Cross( &vecx1, &upvec, &vecz1 );
	D3DXVec3Normalize( &vecx1, &vecx1 );

	D3DXVec3Cross( &vecy1, &vecz1, &vecx1 );
	D3DXVec3Normalize( &vecy1, &vecy1 );

	D3DXMATRIX tmpxmat;
	D3DXQUATERNION tmpxq;

	D3DXMatrixIdentity( &tmpxmat );
	tmpxmat._11 = vecx1.x;
	tmpxmat._12 = vecx1.y;
	tmpxmat._13 = vecx1.z;

	tmpxmat._21 = vecy1.x;
	tmpxmat._22 = vecy1.y;
	tmpxmat._23 = vecy1.z;

	tmpxmat._31 = vecz1.x;
	tmpxmat._32 = vecz1.y;
	tmpxmat._33 = vecz1.z;

	D3DXQuaternionRotationMatrix( &tmpxq, &tmpxmat );

	dstq->x = tmpxq.x;
	dstq->y = tmpxq.y;
	dstq->z = tmpxq.z;
	dstq->w = tmpxq.w;

	return 0;
}
***/


/***
int CShdHandler::GetInitialBoneQ( int childno, CQuaternion* dstq )
{

	int ret;

	if( (childno <= 0) || (childno >= s2shd_leng) ){
		DbgOut( "sh : GetInitialBoneQ : childno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* childjoint;
	childjoint = (*this)( childno );
	if( !childjoint->IsJoint() || (childjoint->type == SHDMORPH) ){
		DbgOut( "sh : GetInitialBoneQ : applychild type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* parjoint;
	parjoint = FindUpperJoint( childjoint );
	if( !parjoint ){
		//DbgOut( "sh : GetBone3Vec : parjoint NULL error !!!\n" );
		//_ASSERT( 0 );
		//return 1;

		dstq->SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	CPart* partptr;
	partptr = parjoint->part;
	if( !partptr ){
		DbgOut( "sh : GetInitialBoneQ : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneInfo* biptr = 0;
	ret = partptr->GetBoneInfo( childno, &biptr );
	if( ret || !biptr ){
		DbgOut( "sh : GetInitialBoneQ : GetBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXMATRIX matview, invmatview;
	D3DXVECTOR3 startpos, endpos, upvec;

	D3DXVECTOR3 vecx0, vecy0, vecz0;
	D3DXVECTOR3 vecx1, vecy1, vecz1;


	startpos.x = biptr->bonestart.x;
	startpos.y = biptr->bonestart.y;
	startpos.z = biptr->bonestart.z;
	
	endpos.x = biptr->boneend.x;
	endpos.y = biptr->boneend.y;
	endpos.z = biptr->boneend.z;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	D3DXVECTOR3 xbonevec;
	xbonevec.x = biptr->bonevec.x;
	xbonevec.y = biptr->bonevec.y;
	xbonevec.z = biptr->bonevec.z;

	if( (biptr->bonevec.x != 0.0f) || (biptr->bonevec.y != 0.0f) ){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;

		D3DXMatrixLookAtLH( &matview, &startpos, &endpos, &upvec );
		matview._41 = 0.0f;
		matview._42 = 0.0f;
		matview._43 = 0.0f;
		D3DXMatrixInverse( &invmatview, NULL, &matview );


		D3DXVec3TransformCoord( &vecx1, &vecx0, &invmatview );
		D3DXVec3TransformCoord( &vecy1, &vecy0, &invmatview );
		D3DXVec3TransformCoord( &vecz1, &vecz0, &invmatview );

		D3DXVec3Normalize( &vecx1, &vecx1 );
		D3DXVec3Normalize( &vecy1, &vecy1 );
		D3DXVec3Normalize( &vecz1, &vecz1 );

	}else{
		//ボーンがZ軸と平行

		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;

		D3DXMatrixLookAtLH( &matview, &startpos, &endpos, &upvec );
		matview._41 = 0.0f;
		matview._42 = 0.0f;
		matview._43 = 0.0f;
		D3DXMatrixInverse( &invmatview, NULL, &matview );


		D3DXVec3TransformCoord( &vecx1, &vecx0, &invmatview );
		D3DXVec3TransformCoord( &vecy1, &vecy0, &invmatview );
		D3DXVec3TransformCoord( &vecz1, &vecz0, &invmatview );

		D3DXVec3Normalize( &vecx1, &vecx1 );
		D3DXVec3Normalize( &vecy1, &vecy1 );
		D3DXVec3Normalize( &vecz1, &vecz1 );

	}

	D3DXMATRIX tmpxmat;
	D3DXQUATERNION tmpxq;

	D3DXMatrixIdentity( &tmpxmat );
	tmpxmat._11 = vecx1.x;
	tmpxmat._12 = vecx1.y;
	tmpxmat._13 = vecx1.z;

	tmpxmat._21 = vecy1.x;
	tmpxmat._22 = vecy1.y;
	tmpxmat._23 = vecy1.z;

	tmpxmat._31 = vecz1.x;
	tmpxmat._32 = vecz1.y;
	tmpxmat._33 = vecz1.z;

	D3DXQuaternionRotationMatrix( &tmpxq, &tmpxmat );

	dstq->x = -tmpxq.x;
	dstq->y = -tmpxq.y;
	dstq->z = -tmpxq.z;
	dstq->w = tmpxq.w;

	return 0;
}
***/

/***
int CShdHandler::GetInitialBoneQ( int childno, CQuaternion* dstq )
{

	int ret;

	if( (childno <= 0) || (childno >= s2shd_leng) ){
		DbgOut( "sh : GetInitialBoneQ : childno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 vecx, vecy, vecz;
	ret = GetBone3Vec( childno, &vecx, &vecy, &vecz );
	if( ret ){
		DbgOut( "sh : GetInitialBoneQ : GetBone3Vec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion q1( 1.0f, 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vecz0( 0.0f, 0.0f, 1.0f );
	float chkdot;
	chkdot = vecz0.x * vecz.x + vecz0.y * vecz.y + vecz0.z * vecz.z; 
	if( chkdot <= -0.999999 ){
		q1.SetAxisAndRot( vecx, (float)PAI );
	}else{
		q1.RotationArc( vecz0, vecz );
	}
	q1.normalize();


	D3DXVECTOR3 vecy0( 0.0f, 1.0, 0.0 );
	D3DXVECTOR3 vecy1;
	q1.Rotate( &vecy1, vecy0 );


	float chkdot2;
	CQuaternion q2( 1.0f, 0.0f, 0.0f, 0.0f );
	chkdot2 = vecy1.x * vecy.x + vecy1.y * vecy.y + vecy1.z * vecy.z; 
	if( chkdot <= -0.999999 ){
		q2.SetAxisAndRot( vecz0, (float)PAI );
	}else{
		q2.RotationArc( vecy1, vecy );
	}
	q2.normalize();

	*dstq = q2 * q1;
	dstq->normalize();


	return 0;
}
***/


int CShdHandler::GetBone3Vec( int childno, D3DXVECTOR3* vecx, D3DXVECTOR3* vecy, D3DXVECTOR3* vecz )
{
	int ret;
	CShdElem* childjoint;
	childjoint = (*this)( childno );
	if( !childjoint->IsJoint() || (childjoint->type == SHDMORPH) ){
		DbgOut( "sh : GetBone3Vec : applychild type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* parjoint;
	parjoint = FindUpperJoint( childjoint );
	if( !parjoint ){
		//DbgOut( "sh : GetBone3Vec : parjoint NULL error !!!\n" );
		//_ASSERT( 0 );
		//return 1;

		vecx->x = 1.0f;
		vecx->y = 0.0f;
		vecx->z = 0.0f;

		vecy->x = 0.0f;
		vecy->y = 1.0f;
		vecy->z = 0.0f;

		vecz->x = 0.0f;
		vecz->y = 0.0f;
		vecz->z = 1.0f;

		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	CPart* partptr;
	partptr = parjoint->part;
	if( !partptr ){
		DbgOut( "sh : GetBone3Vec : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneInfo* biptr = 0;
	ret = partptr->GetBoneInfo( childno, &biptr );
	if( ret || !biptr ){
		DbgOut( "sh : GetBone3Vec : GetBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXMATRIX matview, invmatview;
	D3DXVECTOR3 startpos, endpos, upvec;

	D3DXVECTOR3 vecx0, vecy0, vecz0;
	D3DXVECTOR3 vecx1, vecy1, vecz1;


	startpos.x = biptr->bonestart.x;
	startpos.y = biptr->bonestart.y;
	startpos.z = biptr->bonestart.z;
	
	endpos.x = biptr->boneend.x;
	endpos.y = biptr->boneend.y;
	endpos.z = biptr->boneend.z;

	vecx0.x = 1.0;
	vecx0.y = 0.0;
	vecx0.z = 0.0;

	vecy0.x = 0.0;
	vecy0.y = 1.0;
	vecy0.z = 0.0;

	vecz0.x = 0.0;
	vecz0.y = 0.0;
	vecz0.z = 1.0;

	D3DXVECTOR3 xbonevec;
	xbonevec.x = biptr->bonevec.x;
	xbonevec.y = biptr->bonevec.y;
	xbonevec.z = biptr->bonevec.z;

	/***
	CQuaternion2 rotq( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	int lcnt;
	for( lcnt = 0; lcnt < 20; lcnt++ ){
		ret = LookAtQ( &rotq, xbonevec, vecz0, 1, 2 );
		_ASSERT( !ret );
	}

	rotq.Rotate( &vecx1, vecx0 );
	rotq.Rotate( &vecy1, vecy0 );
	rotq.Rotate( &vecz1, vecz0 );
	
	D3DXVec3Normalize( &vecx1, &vecx1 );
	D3DXVec3Normalize( &vecy1, &vecy1 );
	D3DXVec3Normalize( &vecz1, &vecz1 );

	vecx1 *= (biptr->boneleng * 0.5f);
	vecy1 *= (biptr->boneleng * 0.5f);
	vecz1 *= (biptr->boneleng * 0.5f);
	***/

	if( (biptr->bonevec.x != 0.0f) || (biptr->bonevec.y != 0.0f) ){
		upvec.x = 0.0f;
		upvec.y = 0.0f;
		upvec.z = 1.0f;

		D3DXMatrixLookAtLH( &matview, &startpos, &endpos, &upvec );
		matview._41 = 0.0f;
		matview._42 = 0.0f;
		matview._43 = 0.0f;
		D3DXMatrixInverse( &invmatview, NULL, &matview );


		D3DXVec3TransformCoord( &vecx1, &vecx0, &invmatview );
		D3DXVec3TransformCoord( &vecy1, &vecy0, &invmatview );
		D3DXVec3TransformCoord( &vecz1, &vecz0, &invmatview );

		D3DXVec3Normalize( &vecx1, &vecx1 );
		D3DXVec3Normalize( &vecy1, &vecy1 );
		D3DXVec3Normalize( &vecz1, &vecz1 );

	}else{
		//ボーンがZ軸と平行

		upvec.x = 1.0f;
		upvec.y = 0.0f;
		upvec.z = 0.0f;

		D3DXMatrixLookAtLH( &matview, &startpos, &endpos, &upvec );
		matview._41 = 0.0f;
		matview._42 = 0.0f;
		matview._43 = 0.0f;
		D3DXMatrixInverse( &invmatview, NULL, &matview );


		D3DXVec3TransformCoord( &vecx1, &vecx0, &invmatview );
		D3DXVec3TransformCoord( &vecy1, &vecy0, &invmatview );
		D3DXVec3TransformCoord( &vecz1, &vecz0, &invmatview );

		D3DXVec3Normalize( &vecx1, &vecx1 );
		D3DXVec3Normalize( &vecy1, &vecy1 );
		D3DXVec3Normalize( &vecz1, &vecz1 );

	}

	*vecx = vecx1;
	*vecy = vecy1;
	*vecz = vecz1;

	return 0;
}


int CShdHandler::CreateInfScopePM2( int srcapplychild, int srctarget )
{
	int ret;
	if( (srcapplychild <= 0) || (srcapplychild >= s2shd_leng) ){
		DbgOut( "sh : CreateInfScopePM2 : srcapplychild error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srctarget <= 0) || (srctarget >= s2shd_leng) ){
		DbgOut( "sh : CreateInfScopePM2 : srctarget error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* iselem;
	iselem = GetInfScopeElem();
	if( !iselem ){
		DbgOut( "sh : CreateInfScopePM2 : IS not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* childjoint;
	childjoint = (*this)( srcapplychild );
	if( !childjoint->IsJoint() || (childjoint->type == SHDMORPH) ){
		DbgOut( "sh : CreateInfScopePM2 : applychild type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* parjoint;
	parjoint = FindUpperJoint( childjoint );
	if( !parjoint ){
		DbgOut( "sh : CreateInfScopePM2 : parjoint NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CPart* partptr;
	partptr = parjoint->part;
	if( !partptr ){
		DbgOut( "sh : CreateInfScopePM2 : part NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CBoneInfo* biptr = 0;
	ret = partptr->GetBoneInfo( srcapplychild, &biptr );
	if( ret || !biptr ){
		DbgOut( "sh : CreateInfScopePM2 : GetBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



//////////////
	int facenum = 12;
	int shdtype = SHDPOLYMESH2;
	CMeshInfo tempinfo;
	ret = tempinfo.ResetParams();
	_ASSERT( !ret );
	ret = tempinfo.SetMem( &shdtype, BASE_TYPE );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( facenum * 3, MESHI_M );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( facenum, MESHI_N );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 0, MESHI_TOTAL );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 1, MESHI_MCLOSED );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 0, MESHI_NCLOSED );
	_ASSERT( !ret );

	CPolyMesh2* polymesh2;
	polymesh2 = new CPolyMesh2();
	if( !polymesh2 ){
		DbgOut( "sh : CreateInfScopePM2 : polymesh2 alloc error !!!\n" );
		return 1;
	}

	ret = polymesh2->CreateBuffers( &tempinfo );
	if( ret ){
		DbgOut( "sh : CreateInfScopePM2 : CreateBuffers error !!!\n" );
		return 1;
	}
///////////////

	D3DXVECTOR3 vecx1, vecy1, vecz1;
	ret = GetBone3Vec( srcapplychild, &vecx1, &vecy1, &vecz1 );
	if( ret ){
		DbgOut( "sh : CreateInfScopePM2 : GetBone3Vec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	vecx1 *= (biptr->boneleng * 0.5f);
	vecy1 *= (biptr->boneleng * 0.5f);
	vecz1 *= (biptr->boneleng * 0.5f);


///////////////
	D3DXVECTOR3 center;
	center.x = biptr->bonecenter.x;
	center.y = biptr->bonecenter.y;
	center.z = biptr->bonecenter.z;

	D3DXVECTOR3 xpbuf[8];
	xpbuf[0] = center - vecx1 + vecy1 + vecz1;
	xpbuf[1] = center + vecx1 + vecy1 + vecz1;
	xpbuf[2] = center - vecx1 - vecy1 + vecz1;
	xpbuf[3] = center + vecx1 - vecy1 + vecz1;
	xpbuf[4] = center + vecx1 + vecy1 - vecz1;
	xpbuf[5] = center - vecx1 + vecy1 - vecz1;
	xpbuf[6] = center + vecx1 - vecy1 - vecz1;
	xpbuf[7] = center - vecx1 - vecy1 - vecz1;



//DbgOut( "check !!! : sh : CreateInfScopePM2 : boneleng %f, center %f %f %f\r\n, vecx %f %f %f, vecy %f %f %f, vecz %f %f %f\r\n",
//	biptr->boneleng, center.x, center.y, center.z,
//	vecx1.x, vecx1.y, vecx1.z,
//	vecy1.x, vecy1.y, vecy1.z,
//	vecz1.x, vecz1.y, vecz1.z );



//// 面の向きをチェック
	D3DXVECTOR3 vec1, vec2, facenormal, facecenter, center2src;
	float dot;
	int insideflag;

	vec1.x = xpbuf[3].x - xpbuf[0].x;
	vec1.y = xpbuf[3].y - xpbuf[0].y;
	vec1.z = xpbuf[3].z - xpbuf[0].z;

	vec2.x = xpbuf[2].x - xpbuf[0].x;
	vec2.y = xpbuf[2].y - xpbuf[0].y;
	vec2.z = xpbuf[2].z - xpbuf[0].z;

	D3DXVec3Cross( &facenormal, &vec1, &vec2 );

	facecenter = ( xpbuf[0] + xpbuf[3] + xpbuf[2] ) / 3.0f;

	center2src = center - facecenter;

	dot = D3DXVec3Dot( &facenormal, &center2src );
	if( dot > 0.0f ){
		insideflag = 0;
	}else{
		insideflag = 1;
	}


///////////////////

	int pm2index[12][3];
	if( insideflag == 1 ){
		pm2index[0][0] = 0;
		pm2index[0][1] = 3;
		pm2index[0][2] = 2;
		pm2index[1][0] = 3;
		pm2index[1][1] = 0;
		pm2index[1][2] = 1;
		pm2index[2][0] = 1;
		pm2index[2][1] = 6;
		pm2index[2][2] = 3;
		pm2index[3][0] = 6;
		pm2index[3][1] = 1;
		pm2index[3][2] = 4;
		pm2index[4][0] = 4;
		pm2index[4][1] = 7;
		pm2index[4][2] = 6;
		pm2index[5][0] = 7;
		pm2index[5][1] = 4;
		pm2index[5][2] = 5;
		pm2index[6][0] = 5;
		pm2index[6][1] = 2;
		pm2index[6][2] = 7;
		pm2index[7][0] = 2;
		pm2index[7][1] = 5;
		pm2index[7][2] = 0;
		pm2index[8][0] = 0;
		pm2index[8][1] = 4;
		pm2index[8][2] = 1;
		pm2index[9][0] = 4;
		pm2index[9][1] = 0;
		pm2index[9][2] = 5;
		pm2index[10][0] = 3;
		pm2index[10][1] = 7;
		pm2index[10][2] = 2;
		pm2index[11][0] = 7;
		pm2index[11][1] = 3;
		pm2index[11][2] = 6;
	}else{
		pm2index[0][0] = 0;
		pm2index[0][1] = 2;
		pm2index[0][2] = 3;

		pm2index[1][0] = 3;
		pm2index[1][1] = 1;
		pm2index[1][2] = 0;
		
		pm2index[2][0] = 1;
		pm2index[2][1] = 3;
		pm2index[2][2] = 6;
		
		pm2index[3][0] = 6;
		pm2index[3][1] = 4;
		pm2index[3][2] = 1;
		
		pm2index[4][0] = 4;
		pm2index[4][1] = 6;
		pm2index[4][2] = 7;
		
		pm2index[5][0] = 7;
		pm2index[5][1] = 5;
		pm2index[5][2] = 4;
		
		pm2index[6][0] = 5;
		pm2index[6][1] = 7;
		pm2index[6][2] = 2;
		
		pm2index[7][0] = 2;
		pm2index[7][1] = 0;
		pm2index[7][2] = 5;
		
		pm2index[8][0] = 0;
		pm2index[8][1] = 1;
		pm2index[8][2] = 4;
		
		pm2index[9][0] = 4;
		pm2index[9][1] = 5;
		pm2index[9][2] = 0;
		
		pm2index[10][0] = 3;
		pm2index[10][1] = 2;
		pm2index[10][2] = 7;
		
		pm2index[11][0] = 7;
		pm2index[11][1] = 6;
		pm2index[11][2] = 3;
	}

	VEC3F pbuf[36];
	int fno, pno;
	int setno = 0;
	for( fno = 0; fno < 12; fno++ ){
		for( pno = 0; pno < 3; pno++ ){
			pbuf[setno].x = xpbuf[ pm2index[fno][pno] ].x;
			pbuf[setno].y = xpbuf[ pm2index[fno][pno] ].y;
			pbuf[setno].z = xpbuf[ pm2index[fno][pno] ].z;

			setno++;
		}
	}
	MoveMemory( polymesh2->pointbuf, pbuf, sizeof( VEC3F ) * facenum * 3 );

	ARGBF diffuse[12];
	ARGBF ambient[12];
	for( fno = 0; fno < 12; fno++ ){
		diffuse[fno].a = 0.5f;
		diffuse[fno].r = 0.0f;
		diffuse[fno].g = 0.0f;
		diffuse[fno].b = 1.0f;

		ambient[fno].a = 0.5f;
		ambient[fno].r = 0.0f;
		ambient[fno].g = 0.0f;
		ambient[fno].b = 0.25f;
	}
	MoveMemory( polymesh2->diffusebuf, diffuse, sizeof( ARGBF ) * 12 );
	MoveMemory( polymesh2->ambientbuf, ambient, sizeof( ARGBF ) * 12 );

///////////////
	ret = polymesh2->SetSamePoint();
	_ASSERT( !ret );

	for( fno = 0; fno < facenum; fno++ ){
		*( polymesh2->m_attrib0 + fno ) = -1;//!!!!!! materialは使わないが一応デフォルトをセット。
	}


///////////////
	CShdElem* targetelem;
	targetelem = (*this)( srctarget );
	_ASSERT( targetelem );

	polymesh2->m_mikoapplychild = childjoint;//!!!!!!!!
	polymesh2->m_mikotarget = targetelem;//!!!!!!!!!!!!

	ret = iselem->AddInfScope( polymesh2, 0, 59.5f );
	if( ret ){
		DbgOut( "sh : MeshConv2InfScope : pm2 iselem AddInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		


	return 0;
}

int CShdHandler::LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag )
{
	int ret;

	D3DXVECTOR3 newtarget;
	int newdivnum;

	float savetwist = dstqptr->twist;

	int saultflag = 0;

	if( upflag == 2 ){

		D3DXVECTOR3 toppos;
		if( tarvec.y > 0.0f ){
			toppos.x = 0.0f;
			toppos.y = 1.0f;
			toppos.z = 0.0f;
		}else if( tarvec.y < 0.0f ){
			toppos.x = 0.0f;
			toppos.y = -1.0f;
			toppos.z = 0.0f;
		}else{
			toppos.x = 0.0f;//!!!!!!!!!!!! 2004/4/16
			toppos.y = 1.0f;
			toppos.z = 0.0f;
		}


		//saultチェック
		ret = dstqptr->CheckSault( basevec, tarvec, upflag, &saultflag );
		if( ret ){
			DbgOut( "sh : LookAtQ : CheckSault error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		// tarvec Ｙシフト
		int shiftflag = 0;
		D3DXVECTOR3 shifttarvec;
		ret = dstqptr->ShiftVecNearY( basevec, tarvec, &shiftflag, &shifttarvec );
		if( ret ){
			DbgOut( "sh : LookAtQ : ShiftVecNearY error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( shiftflag == 1 )
			tarvec = shifttarvec;


		// toppos Yシフト
		int shiftflag2 = 0;
		D3DXVECTOR3 shifttarvec2;
		ret = dstqptr->ShiftVecNearY( basevec, toppos, &shiftflag2, &shifttarvec2 );
		if( ret ){
			DbgOut( "sh : LookAtQ : ShiftVecNearY  2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( shiftflag2 == 1 )
			toppos = shifttarvec2;

//DbgOut( "sh : E3DLookAtQ : saultflag %d, shift tarvec %f %f %f, toppos %f %f %f\n",
//	   saultflag, tarvec.x, tarvec.y, tarvec.z, toppos.x, toppos.y, toppos.z );


		if( (saultflag == 0) || (saultflag == 2) ){

			// 移動角度が大きい場合は、targetを近くに設定し直す。分割数も、設定し直す。

			// 直接、tarvecを目指して、回転する。

			
			
			ret = dstqptr->GetDividedTarget( basevec, tarvec, tarvec, divnum, &newtarget, &newdivnum );
			if( ret ){
				DbgOut( "sh : LookAtQ : GetDividedTarget error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			tarvec = newtarget;
			divnum = newdivnum;
			
			if( tarvec == toppos ){
				_ASSERT( 0 );
			}


//DbgOut( "sh : E3DLookAtQ : saultflag %d, tarvec %f %f %f, divnum %d\n",
//	   saultflag, tarvec.x, tarvec.y, tarvec.z, divnum );


		}else if( saultflag == 1 ){
			
				
			// 移動角度が大きい場合は、targetを近くに設定し直す。分割数も、設定し直す。

			// まず、topposをめざして、回転する。

			ret = dstqptr->GetDividedTarget( basevec, toppos, tarvec, divnum, &newtarget, &newdivnum );
			if( ret ){
				DbgOut( "sh : LookAtQ : GetDividedTarget error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			tarvec = newtarget;
			divnum = newdivnum;

			if( tarvec == toppos ){
				_ASSERT( 0 );
			}
			
		}
		
	}
		
	//DbgOut( "sh : LookAtQ : saultflag %d\n", saultflag );
	//DbgOut( "sh : LookAtQ : 0 : divnum %d\n", divnum );

	// targetへの回転処理
	CQuaternion2 finalq;
	CQuaternion2 rot1, rot2;
	D3DXVECTOR3 upvec;


	CQuaternion2 saveq;
	saveq = *dstqptr;


	finalq = *dstqptr;

	ret = finalq.LookAt( tarvec, basevec, 10000.0f, upflag, 0, 0, &upvec, &rot1, &rot2 );
	if( ret ){
		DbgOut( "sh : LookAtQ : fianlq LookAt error !!!\n" );
	}

	CQuaternion2 firstq;
	firstq = *dstqptr;

	if( divnum >= 1 )
		*dstqptr = firstq.Slerp( finalq, divnum, 1 );
	else
		*dstqptr = finalq;

	{
		D3DXVECTOR3 finalvec;
		dstqptr->Rotate( &finalvec, basevec );
		DXVec3Normalize( &finalvec, &finalvec );

		D3DXVECTOR3 trabase;
		saveq.Rotate( &trabase, basevec );
		DXVec3Normalize( &trabase, &trabase );
	}


	dstqptr->twist = savetwist;


	dstqptr->twist = savetwist;

	return 0;
}

int CShdHandler::DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec )
{
	float mag;
	mag = srcvec->x * srcvec->x + srcvec->y * srcvec->y + srcvec->z * srcvec->z;

	float sqmag;
	sqmag = (float)sqrt( mag );

	if( sqmag != 0.0f ){
		float magdiv;
		magdiv = 1.0f / sqmag;
//		dstvec->x *= magdiv;
//		dstvec->y *= magdiv;
//		dstvec->z *= magdiv;
		dstvec->x = srcvec->x * magdiv;
		dstvec->y = srcvec->y * magdiv;
		dstvec->z = srcvec->z * magdiv;

	}else{
		DbgOut( "sh : DXVec3Normalize : zero warning !!!\n" );
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
		_ASSERT( 0 );
	}

	return 0;
}

int CShdHandler::CreateAndSaveTempMaterial()
{
	int ret;
	int seri;
	CShdElem* selem;
	for( seri = 1; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		_ASSERT( selem );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->CreateAndSaveTempMaterial();
			if( ret ){
				DbgOut( "sh CreateAndSaveTempMaterial : selem CreateAndSaveTempMaterial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
int CShdHandler::RestoreAndDestroyTempMaterial()
{
	if( s2shd_leng <= 1 )
		return 0;

	int ret;
	int seri;
	CShdElem* selem;
	for( seri = 1; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		_ASSERT( selem );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->RestoreAndDestroyTempMaterial();
			if( ret ){
				DbgOut( "sh RestoreAndDestroyTempMaterial : selem RestoreAndDestroyTempMaterial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdHandler::SetMaterialFromInfElem( int srcchildno, int srcseri, int srcvert, E3DCOLOR3UC* infcolarray )
{
	int ret;
	if( srcseri < 0 ){
		int seri;
		for( seri = 1; seri < s2shd_leng; seri++ ){
			ret = SetMaterialFromInfElem( srcchildno, seri, -1, infcolarray );
			if( ret ){
				DbgOut( "sh SetMaterialFromInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;
	}

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh SetMaterialFromInfElem : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( srcseri );
	_ASSERT( selem );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = selem->SetMaterialFromInfElem( srcchildno, srcvert, infcolarray );
		if( ret ){
			DbgOut( "sh SetMaterialFromInfElem : se SetMaterialFromInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::SetInfElem( int srcseri, int srcvert, int srcchildno, int paintmode, int calcmode, float rate, int normflag, float directval, int updateflag )
{
	if( (srcseri <= 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh : SetInfElem : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srcchildno <= 0) || (srcchildno >= s2shd_leng) ){
		DbgOut( "sh : SetInfElem : srcchildno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* childelem;
	childelem = (*this)( srcchildno );
	_ASSERT( childelem );

	if( (!childelem->IsJoint()) || (childelem->type == SHDMORPH) ){
		DbgOut( "sh : SetInfElem : child not joint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* parelem;
	parelem = FindUpperJoint( childelem );
	if( !parelem ){
		DbgOut( "sh : SetInfElem : parent not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CPart* partptr;
	partptr = parelem->part;
	_ASSERT( partptr );
	CBoneInfo* biptr;
	ret = partptr->GetBoneInfo( srcchildno, &biptr );
	if( ret || !biptr ){
		DbgOut( "sh : SetInfElem : part GetBoneInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* gparselem;
	int gparno;
	if( biptr )
		gparno = biptr->parentjoint;
	else
		gparno = -1;
		
	if( gparno > 0 ){
		gparselem = (*this)( gparno );
	}else{
		gparselem = 0;
	}


	CShdElem* selem;
	selem = (*this)( srcseri );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = selem->SetInfElem( this, srcvert, biptr, gparselem, paintmode, calcmode, rate, normflag, directval, updateflag );
		if( ret ){
			if( ret == 2 ){
				//ret 2 は、５個以上の影響度設定をしようとして失敗したとき。
				DbgOut( "sh : SetInfElem : se SetInfElem : over 5 influence warning !!!\n" );
				
				return 2;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			}else{
				DbgOut( "sh : SetInfElem : se SetInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}

int CShdHandler::DeleteInfElem( int srcseri, int srcvert, int srcchildno, int normflag, int updateflag )
{
	int ret;
	if( (srcseri <= 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh : DeleteInfElem : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (srcchildno <= 0) || (srcchildno >= s2shd_leng) ){
		DbgOut( "sh : DeleteInfElem : srcchildno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* childelem;
	childelem = (*this)( srcchildno );
	_ASSERT( childelem );

	if( (!childelem->IsJoint()) || (childelem->type == SHDMORPH) ){
		DbgOut( "sh : DeleteInfElem : child not joint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( srcseri );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = selem->DeleteInfElem( srcvert, srcchildno, normflag );
		if( ret ){
			DbgOut( "sh : DeleteInfElem : se DeleteInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}

int CShdHandler::DestroyIE( int srcseri, int srcvert )
{
	int ret;

	if( srcseri < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = DestroyIE( seri, -1 );
			if( ret ){
				DbgOut( "sh : DestroyIE error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		return 0;
	}

//////////////////

	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh : DestroyIE : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( srcseri );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = selem->DestroyIE( srcvert );
		if( ret ){
			DbgOut( "sh : DestroyIE : se DestroyIE error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		return 0;
	}

	return 0;
}

int CShdHandler::AddInfElem( int srcseri, int srcvert, INFELEM srcIE )
{
	int ret;
	if( (srcseri <= 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh : AddInfElem : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( srcseri );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = selem->AddInfElem( srcvert, srcIE );
		if( ret ){
			DbgOut( "sh : AddInfElem : se AddInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}

int CShdHandler::NormalizeInfElem( int srcseri, int srcvert )
{
	int ret;
	if( (srcseri <= 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh : NormalizeInfElem : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( srcseri );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = selem->NormalizeInfElem( srcvert );
		if( ret ){
			DbgOut( "sh : NormalizeInfElem : se NormalizeInfElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;

}

int CShdHandler::GetSymVert( int srcseri, int srcvert, int symaxis, float symdist, int* sympart, int* symvert )
{
	*sympart = -1;
	*symvert = -1;


	int ret;
	if( (srcseri < 0) || (srcseri >= s2shd_leng) ){
		DbgOut( "sh : GetSymVert : srcseri error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( srcseri );


	D3DXVECTOR3 orgpos;
	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		ret = GetVertPos( srcseri, srcvert, &orgpos );
		if( ret ){
			DbgOut( "sh GetSymVert : GetVertPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		*sympart = -1;
		*symvert = -1;
		return 0;
	}

	D3DXVECTOR3 findpos;
	switch( symaxis ){
	case SYMAXIS_X:
		findpos.x = -orgpos.x;
		findpos.y = orgpos.y;
		findpos.z = orgpos.z;
		break;
	case SYMAXIS_Y:
		findpos.x = orgpos.x;
		findpos.y = -orgpos.y;
		findpos.z = orgpos.z;
		break;
	case SYMAXIS_Z:
		findpos.x = orgpos.x;
		findpos.y = orgpos.y;
		findpos.z = -orgpos.z;
		break;
	default:
		_ASSERT( 0 );
		*sympart = -1;
		*symvert = -1;
		return 0;
		break;
	}


	float finddist = 1e8;

	int chkseri;
	CShdElem* chkelem;
	for( chkseri = 0; chkseri < s2shd_leng; chkseri++ ){
		chkelem = (*this)( chkseri );

		if( (chkelem->type == SHDPOLYMESH) || (chkelem->type == SHDPOLYMESH2) ){

			float tempdist = 1e8;
			int tempvert = -1;
			ret = chkelem->GetNearVert( findpos, symdist, &tempvert, &tempdist );
			if( ret ){
				DbgOut( "sh : GetSymVert : selem GetNearVert error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( (tempvert >= 0) && (tempdist < finddist) ){
				*sympart = chkseri;
				*symvert = tempvert;
				finddist = tempdist;
			}
		}
	}

	return 0;

}

int CShdHandler::GetVisiblePolygonNum( CMotHandler* lpmh, int* num1ptr, int* num2ptr )
{

	*num1ptr = 0;//!!!!!!!!!!
	*num2ptr = 0;

	int i;
	int dispflag;
	CShdElem* selem;

	int transcnt = 0;

	if( (m_inRDBflag == 0) && (lpmh->m_curbs.visibleflag == 0)){
		*num1ptr = 0;
		*num2ptr = 0;
		return 0;//!!!!!!!!!!!!
	}


	SetCurDS();

	int curnum1, curnum2;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;

		
		int isbone;
		//if( selem->IsJoint() && (selem->type != SHDMORPH) ){
		if( selem->IsJoint() ){
			isbone = 1;
		}else{
			isbone = 0;
		}


		int vflag;
		if( m_inRDBflag == 0 ){
			//if( selem->curbs.visibleflag || (projmode == PROJ_PREINIT) || isbone || (selem->type == SHDBILLBOARD))
			if( selem->curbs.visibleflag || (selem->type == SHDBILLBOARD))
				vflag = 1;
			else
				vflag = 0;
			if( selem->type == SHDMORPH ){
				vflag = selem->morph->m_baseelem->curbs.visibleflag;
			}

		}else{
			vflag = 1;
		}


		if( vflag && dispflag && (selem->notuse != 1) ){

			DWORD dispswitchno = selem->dispswitchno;
			if( (m_curds + dispswitchno)->state != 0 ){  
				
				CD3DDisp* d3ddispptr = selem->d3ddisp;
				if( d3ddispptr ){
					curnum1 = d3ddispptr->m_vnum;
					(*num1ptr) += curnum1;

					d3ddispptr->GetRenderPrimNum( &curnum2 );
					(*num2ptr) += curnum2;

				}
			}
		}
	}


	//DbgOut( "sh : GetVisiblePolygonNum : %d %d\n", *num1ptr, *num2ptr );

	return 0;
}

int CShdHandler::CreateMaterialBlock( LPDIRECT3DDEVICE9 pdev )
{
	int i;
	CShdElem* selem;
	int ret;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		_ASSERT( selem );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->CreateMaterialBlock( pdev, this );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		_ASSERT( selem );

		if( selem->type == SHDMORPH ){
			ret = selem->CreateMaterialBlock( pdev, this );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CShdHandler::CreateToon1Buffer( LPDIRECT3DDEVICE9 pdev, int forceRemakeflag )
{
	int i;
	CShdElem* selem;
	int ret;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		_ASSERT( selem );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->CreateToon1Buffer( this, pdev, forceRemakeflag );
			if( ret ){
				DbgOut( "sh : CreateToon1Buffer : se CreateToon1Buffer error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		_ASSERT( selem );

		if( selem->type == SHDMORPH ){
			ret = selem->CreateToon1Buffer( this, pdev, forceRemakeflag );
			if( ret ){
				DbgOut( "sh : CreateToon1Buffer : se CreateToon1Buffer morph error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

void CShdHandler::CalcXTransformationMatrixReq( CShdElem* curselem, D3DXMATRIX parenttra, float mult, int isstandard )
{
	int ret;

	//if( curselem->IsJoint() || (curselem->type != SHDMORPH) ){
		D3DXMATRIX offsetmat;
		D3DXMatrixIdentity( &offsetmat );
		ret = HuGetOffsetMatrix( curselem->serialno, &offsetmat, mult, isstandard );
		if( ret ){
			DbgOut( "sh : CalcXTransformationMatrixReq : HuGetOffsetMatrix error !!!\n" );
			_ASSERT( 0 );
		}
		D3DXMATRIX offsetinv;
		D3DXMatrixInverse( &offsetinv, NULL, &offsetmat );

		D3DXMATRIX parinv;
		D3DXMatrixInverse( &parinv, NULL, &parenttra );

		curselem->TransformationMatrix = offsetinv * parinv;
		curselem->combinedtra = curselem->TransformationMatrix * parenttra;

	//}


	if( curselem->brother ){

		CShdElem* curbro;
		curbro = GetValidBrother( curselem->brother );
		if( curbro ){
			CalcXTransformationMatrixReq( curbro, parenttra, mult, isstandard );
		}
	}

	if( curselem->child ){

		CShdElem* curchil;
		curchil = GetValidChild( curselem->child );
		if( curchil ){
			CalcXTransformationMatrixReq( curchil, curselem->combinedtra, mult, isstandard );
		}
	}

}

CShdElem* CShdHandler::GetValidBrother( CShdElem* selem )
{
	CShdElem* retelem = selem;

	while( retelem ){
		if( (retelem->type != SHDDESTROYED) && (retelem->type != SHDINFSCOPE) && (retelem->type != SHDBBOX) && (retelem->notuse == 0) ){
			break;
		}else{
			retelem = retelem->brother;
		}
	}

	return retelem;

}

CShdElem* CShdHandler::GetValidChild( CShdElem* selem )
{
	CShdElem* retelem = selem;

	while( retelem ){
		if( (retelem->type != SHDDESTROYED) && (retelem->type != SHDINFSCOPE) && (retelem->type != SHDBBOX) && (retelem->notuse == 0) ){
			break;
		}else{
			//retelem = retelem->child;
			retelem = retelem->brother;
		}
	}

	return retelem;

}


int CShdHandler::RepairInfElem( LPDIRECT3DDEVICE9 pdev )
{
	int ret;
	int seri;
	CShdElem* selem;

	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->RepairInfElem( this, pdev );
			if( ret ){
				DbgOut( "sh : RepairInfElem : se RepairInfElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
	return 0;
}

int CShdHandler::SaveToDispTempDiffuse()
{
	int ret, i;
	CShdElem* selem;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			if( selem->d3ddisp ){
				ret = selem->d3ddisp->SaveToDispTempDiffuse();
				if( ret ){
					DbgOut( "sh : SaveToDispTempDiffuse : d3ddisp SaveToDispTempDiffuse error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}
	return 0;
}

int CShdHandler::RestoreDispTempDiffuse()
{
	int ret, i;
	CShdElem* selem;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			if( selem->d3ddisp ){
				ret = selem->d3ddisp->RestoreDispTempDiffuse( 0 );
				if( ret ){
					DbgOut( "sh : RestoreDispTempDiffuse : d3ddisp RestoreDispTempDiffuse error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}else if( selem->type == SHDMORPH ){
			if( selem->d3ddisp ){
				CPolyMesh2* pm2;
				if( selem->morph->m_objtype == SHDPOLYMESH2 ){
					pm2 = selem->morph->m_baseelem->polymesh2;
				}else{
					pm2 = 0;
				}
				ret = selem->d3ddisp->RestoreDispTempDiffuse( pm2 );
				if( ret ){
					DbgOut( "sh : RestoreDispTempDiffuse : d3ddisp RestoreDispTempDiffuse morph error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}
	return 0;
}

int CShdHandler::CreateSkinMat( int srcseri, int updateflag )
{
	int ret;

	if( srcseri < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = CreateSkinMat( seri, updateflag );
			_ASSERT( !ret );
			if( ret )
				return ret;
		}
	}else{

		if( srcseri >= s2shd_leng ){
			DbgOut( "sh : CreateSkinMat : srcseri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*this)( srcseri );

		ret = selem->CreateSkinMat( this, updateflag );
		if( ret ){
			DbgOut( "sh : CreateSkinMat : selem CreateSkinMat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::EnableToonEdge( int srcpartno, int srctype, int srcflag )
{
	int ret;

	if( srcpartno < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = EnableToonEdge( seri, srctype, srcflag );
			_ASSERT( !ret );
			if( ret )
				return ret;
		}
	}else{
		if( srcpartno >= s2shd_leng ){
			DbgOut( "sh : EnableToonEdge : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*this)( srcpartno );

		ret = selem->EnableToonEdge( srctype, srcflag );
		if( ret ){
			DbgOut( "sh : EnableToonEdge : se EnableToonEdge error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::SetToonEdge0Color( int srcpartno, char* srcname, int srcr, int srcg, int srcb )
{
	int ret;
	if( srcpartno < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = SetToonEdge0Color( seri, 0, srcr, srcg, srcb );
			_ASSERT( !ret );
			if( ret )
				return ret;
		}
	}else{
		if( srcpartno >= s2shd_leng ){
			DbgOut( "sh : SetToonEdge0Color : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*this)( srcpartno );

		ret = selem->SetToonEdge0Color( srcname, srcr, srcg, srcb );
		if( ret ){
			DbgOut( "sh : SetToonEdge0Color : se SetToonEdge0Color error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CShdHandler::SetToonEdge0Width( int srcpartno, char* srcname, float srcwidth )
{
	int ret;
	if( srcpartno < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = SetToonEdge0Width( seri, 0, srcwidth );
			_ASSERT( !ret );
			if( ret )
				return ret;
		}
	}else{
		if( srcpartno >= s2shd_leng ){
			DbgOut( "sh : SetToonEdge0Width : partno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*this)( srcpartno );

		ret = selem->SetToonEdge0Width( srcname, srcwidth );
		if( ret ){
			DbgOut( "sh : SetToonEdge0Color : se SetToonEdge0Color error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::GetToonEdge0Color( int srcpartno, char* srcname, float* dstr, float* dstg, float* dstb )
{
	if( (srcpartno < 0) || (srcpartno >= s2shd_leng) ){
		DbgOut( "sh : GetToonEdge0Color partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcname ){
		DbgOut( "sh : GetToonEdge0Color : name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* selem;
	selem = (*this)( srcpartno );

	ret = selem->GetToonEdge0Color( srcname, dstr, dstg, dstb );
	if( ret ){
		DbgOut( "sh : GetToonEdge0Color : se GetToonEdge0Color error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CShdHandler::GetToonEdge0Width( int srcpartno, char* srcname, float* dstwidth )
{
	if( (srcpartno < 0) || (srcpartno >= s2shd_leng) ){
		DbgOut( "sh : GetToonEdge0Width partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcname ){
		DbgOut( "sh : GetToonEdge0Width : name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CShdElem* selem;
	selem = (*this)( srcpartno );

	ret = selem->GetToonEdge0Width( srcname, dstwidth );
	if( ret ){
		DbgOut( "sh : GetToonEdge0Width : se GetToonEdge0Width error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::SetIndexBufColP()
{
	int ret;

	int seri;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*this)( seri );
		_ASSERT( selem );

		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->SetIndexBufColP();
			if( ret ){
				DbgOut( "sh : SetIndexBufColP : se SetIndexBufColP error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	for( seri = 0; seri < s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*this)( seri );
		_ASSERT( selem );

		if( selem->type == SHDMORPH ){
			ret = selem->SetIndexBufColP();
			if( ret ){
				DbgOut( "sh : SetIndexBufColP : se SetIndexBufColP morph error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	ret = CreateSkinMat( -1, 1 );
	if( ret ){
		DbgOut( "sh : SetIndexBufColP : CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CShdHandler::GetFaceNormal( int partno, int faceno, D3DXMATRIX matWorld, CMotHandler* lpmh, D3DXVECTOR3* dstn )
{
	int ret;

	if( (partno <= 0) || (partno >= s2shd_leng) ){
		DbgOut( "sh : GetFaceNormal : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	ret = selem->GetFaceNormal( faceno, matWorld, lpmh, dstn );
	if( ret ){
		DbgOut( "sh : GetFaceNormal : se GetFaceNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::ChkAlphaNum()
{
	int ret;
	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
			ret = selem->ChkAlphaNum( m_mathead );
			_ASSERT( !ret );
		}
	}

	return 0;
}


int CShdHandler::GetMaterialNo( int partno, int faceno, int* matnoptr )// optindexbufを元にセット RDB Plugin用
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "sh : GetMaterialNo : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );

	int ret;
	ret = selem->GetMaterialNo( faceno, matnoptr );
	if( ret ){
		DbgOut( "sh : GetMaterialNo : se GetMaterialNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CShdHandler::GetMaterialNo2( int partno, int faceno, int* matnoptr )// d3ddispのindexを元にセット　E3D用
{
	if( (partno < 0) || (partno >= s2shd_leng) ){
		DbgOut( "sh : GetMaterialNo2 : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );

	int ret;
	ret = selem->GetMaterialNo2( m_shader, faceno, matnoptr );
	if( ret ){
		DbgOut( "sh : GetMaterialNo2 : se GetMaterialNo2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CShdHandler::SetCurDS()
{
	int findflag = 0;
	int dsno;
	for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
		if( (m_ds2 + dsno)->state == 0 ){
			findflag = 1;
			break;
		}
	}

	if( findflag == 0 ){
		m_curds = m_ds;
	}else{
		m_curds = m_ds2;
	}
	return 1;
}

int CShdHandler::EnablePhongEdge0( int flag )
{
	m_phongedge0enable = flag;

	return 0;
}
int CShdHandler::SetPhongEdge0Params( int r, int g, int b, float width, int srcblendmode, float srcalpha )
{

	float fr, fg, fb;
	fr = (float)r / 255.0f;
	fr = max( 0.0f, fr );
	fr = min( 1.0f, fr );

	fg = (float)g / 255.0f;
	fg = max( 0.0f, fg );
	fg = min( 1.0f, fg );

	fb = (float)b / 255.0f;
	fb = max( 0.0f, fb );
	fb = min( 1.0f, fb );

	m_phongedge0[0] = fr;
	m_phongedge0[1] = fg;
	m_phongedge0[2] = fb;
	m_phongedge0[3] = width;


	m_phongedge0Blend = srcblendmode;

	if( m_phongedge0Blend == 3 ){
		m_phongedge0Alpha = 1.0f;
	}else{
		m_phongedge0Alpha = srcalpha;
		m_phongedge0Alpha = max( 0.0f, m_phongedge0Alpha );
		m_phongedge0Alpha = min( 1.0f, m_phongedge0Alpha );
	}

	return 0;
}

int CShdHandler::SetDirName( char* pathname )
{
//DbgOut( "sh : SetDirName : %s \r\n", pathname );

	if( pathname ){
		char* lastmark;
		lastmark = strrchr( pathname, '\\' );
		if( !lastmark ){
			_ASSERT( 0 );
			return 1;
		}

		int dirleng;
		//char dirname[_MAX_PATH];
		dirleng = (int)(lastmark - pathname + 1);
		if( dirleng >= MAX_PATH ){
			DbgOut( "sh : SetDirName : dirname too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
//DbgOut( "sh : SetDirName : dirleng %d\r\n", dirleng );

		ZeroMemory( m_dirname, sizeof( char ) * PATH_LENG );
//DbgOut( "sh : SetDirName : 1 \r\n" );
		strncpy_s( m_dirname, PATH_LENG, pathname, dirleng );
//DbgOut( "sh : SetDirName : 2 \r\n" );
	}else{
		ZeroMemory( m_dirname, sizeof( char ) * PATH_LENG );
	}
DbgOut( "sh : SetDirName : m_dirname %s\r\n", m_dirname );
	return 0;
}

int CShdHandler::InitTempDS()
{
	int dsno;
	for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
		( m_tempds + dsno )->switchno = dsno;
		( m_tempds + dsno )->state = 1;
	}

	return 0;
}

int CShdHandler::SetShadowInterFlag( int partno, int srcflag )
{
	if( partno < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			SetShadowInterFlag( seri, srcflag );
		}
		return 0;

	}else{
		if( (partno < 0) || (partno >= s2shd_leng) ){
			DbgOut( "sh : SetShadowInterFlag : seri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*this)( partno );

		selem->m_shadowinflag = srcflag;

		return 0;
	}
}

int CShdHandler::CalcZa4LocalQ( CMotHandler* srcmh, int srcmotid, int srcframeno )
{
	int ret;
	int seri;
	CShdElem* selem;

	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		_ASSERT( selem );

		if( selem->IsJoint() && (selem->type != SHDMORPH) ){

			CQuaternion selq;
			ret = srcmh->GetTotalQOnFrame( this, srcmotid, srcframeno, seri, &selq );
			if( ret ){
				DbgOut( "sh : CalcZa4LocalQ : mh GetTotalQOnFrame error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int za4type = ZA_1;
			ret = selem->GetZa4Type( &za4type );
			_ASSERT( !ret );

			CQuaternion za4q;
			ret = selem->GetZa4Q( &za4q );
			_ASSERT( !ret );

			CQuaternion localq;
			if( za4type == ZA_1 ){
				localq = selq * za4q;
				//localq = za4q * selq;
			}else if( za4type == ZA_2 ){
				localq = za4q;
			}else if( za4type == ZA_3 ){
				CQuaternion iniq;
				ret = GetInitialBoneQ( seri, &iniq );
				if( ret ){
					DbgOut( "sh : CalcZa4LocalQ : sh GetInitialBoneQ error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				//localq = selq * iniq * za4q;
				localq = za4q * iniq * selq;
			}else{
				_ASSERT( 0 );
				localq.SetParams( 1.0f, 0.0, 0.0f, 0.0f );
			}

			ret = selem->SetZa4LocalQ( localq );
			_ASSERT( !ret );

		}
	}

	return 0;
}

int CShdHandler::ConvMorphElem()
{
	int ret;
	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( selem->type == SHDMORPH ){
			CMorph* morph;
			morph = selem->morph;
			if( morph ){
				ret = morph->ConvSeriToElem();
				if( ret ){
					DbgOut( "sh : ConvMorphElem : morph ConvSeriToElem error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				_ASSERT( 0 );
			}
		}
	}

	return 0;
}

int CShdHandler::GetMorphObj( CShdElem* srcbase, CMorph** dstmorph )
{
	CMorph* retmorph = 0;

	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( selem->type == SHDMORPH ){
			CMorph* chkmorph = selem->morph;
			_ASSERT( chkmorph );
			if( chkmorph->m_baseelem == srcbase ){
				retmorph = chkmorph;
				break;
			}
		}
	}

	*dstmorph = retmorph;
	return 0;
}

int CShdHandler::GetMorphElem( CShdElem** ppselem, int* numptr, int arrayleng )
{
	*numptr = 0;

	int setno = 0;
	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( selem->type == SHDMORPH ){
			if( ppselem ){
				if( setno >= arrayleng ){
					DbgOut( "sh : GetMorphElem : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( ppselem + setno ) = selem;
			}
			setno++;
		}
	}

	*numptr = setno;

	return 0;
}

int CShdHandler::GetMorphElem( CShdElem** ppselem, int baseid )
{
	*ppselem = 0;

	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( selem->type == SHDMORPH ){
			CMorph* morphptr;
			morphptr = selem->morph;
			if( morphptr && (morphptr->m_baseelem->serialno == baseid) ){
				*ppselem = selem;
				break;
			}
		}
	}

	return 0;
}

int CShdHandler::SetMorphDispFlag( int flag )
{
	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( selem->type == SHDMORPH ){
			selem->dispflag = flag;
		}
	}

	return 0;
}

int CShdHandler::SetMotionBlur( int mode, int blurtime )
{

	int ret;
	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) || (selem->type == SHDMORPH) ){
			ret = selem->SetMotionBlur( this, mode, blurtime );
			if( ret ){
				DbgOut( "sh : SetMotionBlur : se SetMotionBlur error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CShdHandler::SetBeforeBlur( int bbflag )
{
	int ret;
	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) || (selem->type == SHDMORPH) ){
			ret = selem->SetBeforeBlur( bbflag );
			if( ret ){
				DbgOut( "sh : SetBeforeBlur : se SetBeforeBlur error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}

int CShdHandler::InitBeforeBlur()
{
	int ret;
	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) || (selem->type == SHDMORPH) ){
			ret = selem->InitBeforeBlur();
			if( ret ){
				DbgOut( "sh : InitBeforeBlur : se InitBeforeBlur error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}


	return 0;
}


int CShdHandler::RenderMotionBlur( LPDIRECT3DDEVICE9 pd3dDevice, CMotHandler* lpmh )
{
	int i, ret;
	int dispflag;
	int vflag;
	CShdElem* selem;
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	if( (m_inRDBflag == 0) && (lpmh->m_curbs.visibleflag == 0) ){
//		//_ASSERT( 0 );
//		return 0;
//	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;

	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
	g_renderstate[ D3DRS_ALPHAREF ] = 0x01;

	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
	g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
	g_renderstate[ D3DRS_ZWRITEENABLE ] = FALSE;
	g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

	for( i = 0; i < s2shd_leng; i++ ){
		selem = (*this)( i );
		dispflag = selem->dispflag;

		if( (m_inRDBflag == 0) && (selem->type != SHDBILLBOARD) && (selem->type != SHDEXTLINE) && (selem->type != SHDINFSCOPE) )
			vflag = selem->curbs.visibleflag;
		else
			vflag = 1;
		if( selem->type == SHDMORPH ){
			vflag = selem->morph->m_baseelem->curbs.visibleflag;
		}


		if( vflag && dispflag  
			&& !( selem->IsJoint() && (selem->type != SHDMORPH) ) 
			&& (selem->notuse != 1) 
			&& (selem->invisibleflag == 0) ){										

			if( selem->type == SHDBILLBOARD ){
					
				// Set diffuse blending for alpha set in vertices.
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
				g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;


				//pd3dDevice->SetTexture( 0, curtex );
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
				//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
				if( g_aop0 != D3DTOP_MODULATE ){
					pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
					g_aop0 = D3DTOP_MODULATE;
				}
				if( g_aop1 != D3DTOP_DISABLE ){
					pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
					g_aop1 = D3DTOP_DISABLE;
				}

				ret = selem->RenderBillboardMotionBlur( pd3dDevice, this, g_texbnk );
				if( ret ){
					DbgOut( "shdhandler : Render : se RenderBillboardMotionBlur error !!1\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( g_cop0 != D3DTOP_MODULATE ){
					pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
					g_cop0 = D3DTOP_MODULATE;
				}
				if( g_aop0 != D3DTOP_MODULATE ){
					pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
					g_aop0 = D3DTOP_MODULATE;
				}
			}else if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) || 
				(selem->type == SHDMORPH) ){
				DWORD dispswitchno = selem->dispswitchno;
				int dsstate = (m_curds + dispswitchno)->state;
				if( dsstate != 0 ){  

					SetRenderState( pd3dDevice, selem );

					pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
					pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
					pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
					g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
					g_renderstate[ D3DRS_ZWRITEENABLE ] = FALSE;
					g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

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
//					if( g_aop0 != D3DTOP_SELECTARG1 ){
//						pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//						g_aop0 = D3DTOP_SELECTARG1;
//					}
					if( g_aop0 != D3DTOP_MODULATE ){
						pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
						g_aop0 = D3DTOP_MODULATE;
					}
					if( g_aop1 != D3DTOP_DISABLE ){
						pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
						g_aop1 = D3DTOP_DISABLE;
					}

					if( (selem->d3ddisp->m_blurmode != BLUR_NONE) && (selem->d3ddisp->m_blurtime >= 1) ){
						if( (selem->m_blurbefrender == 0) || ((selem->m_befdispswitch == 0) && (dsstate != 0)) ){
							ret = selem->InitBeforeBlur();
							if( ret ){
								DbgOut( "sh : RenderMotionBlur : InitBeforeBlur error !!!\n" );
								_ASSERT( 0 );
								return 1;
							}
						}
						selem->m_blurbefrender = 1;
						selem->m_befdispswitch = dsstate;

						ret = selem->RenderMotionBlur( pd3dDevice, this );
						if( ret ){
							DbgOut( "CShdHandler : RenderMotionBlur error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}else{
						selem->m_blurbefrender = 0;
						selem->m_befdispswitch = 0;
					}
				}else{
					selem->m_blurbefrender = 0;
					selem->m_befdispswitch = 0;
				}
			}
		}else{
			selem->m_blurbefrender = 0;
			selem->m_befdispswitch = 0;
		}
	}


	return 0;
}

int CShdHandler::SetMotionBlurMinAlpha( int partno, float minalpha )
{
	int ret;
	if( partno < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = SetMotionBlurMinAlpha( seri, minalpha );
			if( ret ){
				DbgOut( "sh SetMotionBlurMinAlpha error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;//!!!!!!!!
	}

	if( partno >= s2shd_leng ){
		DbgOut( "sh SetMotionBlurMinAlpha : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		selem->m_mbluralpha[0] = minalpha;
	}

	return 0;
}

int CShdHandler::SetMotionBlurMaxAlpha( int partno, float maxalpha )
{
	int ret;
	if( partno < 0 ){
		int seri;
		for( seri = 0; seri < s2shd_leng; seri++ ){
			ret = SetMotionBlurMaxAlpha( seri, maxalpha );
			if( ret ){
				DbgOut( "sh SetMotionBlurMaxAlpha error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;//!!!!!!!!
	}

	if( partno >= s2shd_leng ){
		DbgOut( "sh SetMotionBlurMaxAlpha : partno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* selem;
	selem = (*this)( partno );
	_ASSERT( selem );

	if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
		selem->m_mbluralpha[1] = maxalpha;
	}

	return 0;
}


int CShdHandler::SetMaterialGlowParams( int matno, int glowmode, int setcolflag, E3DCOLOR4F* colptr )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialGlowParams( mqomat->materialno, glowmode, setcolflag, colptr );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}

		if( glowmode == 0 ){
			curmat->exttexmode = EXTTEXMODE_NORMAL;
		}else if( glowmode == GLOW_NORMAL ){
			curmat->exttexmode = EXTTEXMODE_GLOW;
		}else if( glowmode == GLOW_ALPHA ){
			curmat->exttexmode = EXTTEXMODE_GLOWALPHA;
		}

		if( setcolflag ){
			curmat->glowmult[0] = colptr->r;
			curmat->glowmult[1] = colptr->g;
			curmat->glowmult[2] = colptr->b;
		}
	}


	return 0;
}
int CShdHandler::GetMaterialGlowParams( int matno, int* glowmodeptr, E3DCOLOR4F* colptr )
{


	if( !m_mathead ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!!!!!!
	}

	CMQOMaterial* curmat;
	curmat = GetMaterialFromNo( m_mathead, matno );
	if( !curmat ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!!!!!!
	}

	if( curmat->exttexmode == EXTTEXMODE_GLOW ){
		*glowmodeptr = GLOW_NORMAL;
	}else if( curmat->exttexmode == EXTTEXMODE_GLOWALPHA ){
		*glowmodeptr = GLOW_ALPHA;
	}else{
		*glowmodeptr = GLOW_NONE;
	}

	colptr->r = curmat->glowmult[0];
	colptr->g = curmat->glowmult[1];
	colptr->b = curmat->glowmult[2];

	return 0;
}

int CShdHandler::SetMaterialTransparent( int matno, int transparent )
{
	int ret;

	if( !m_mathead ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( matno < 0 ){
		CMQOMaterial* mqomat = m_mathead;
		while( mqomat ){
			if( mqomat->materialno >= 0 ){
				ret = SetMaterialTransparent( mqomat->materialno, transparent );
				_ASSERT( !ret );
			}
			mqomat = mqomat->next;
		}
	}else{

		CMQOMaterial* curmat;
		curmat = GetMaterialFromNo( m_mathead, matno );
		if( !curmat ){
			return 0;//!!!!!!!!!!!!!!!!
		}

		if( (transparent >= 0) && (transparent <=2) ){
			int texindex;
			texindex = g_texbnk->FindName( curmat->tex, curmat->transparent );
			curmat->transparent = transparent;

			if( texindex >= 0 ){
				*( g_texbnk->pidata + texindex ) = transparent;
			}

		}else{
			_ASSERT( 0 );
		}
	}

	ret = ChkAlphaNum();
	_ASSERT( !ret );

	return 0;
}
int CShdHandler::GetMaterialTransparent( int matno, int* transptr )
{

	if( !m_mathead ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!!!!!!
	}

	CMQOMaterial* curmat;
	curmat = GetMaterialFromNo( m_mathead, matno );
	if( !curmat ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!!!!!!
	}

	*transptr = curmat->transparent;

	return 0;
}

int CShdHandler::GetDestroyedSeri( int* seriptr )
{
	*seriptr = -1;

	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		_ASSERT( selem );
		if( selem->type == SHDDESTROYED ){
			*seriptr = seri;
			break;
		}
	}

	return 0;
}

int CShdHandler::GetDestroyedNum( int* numptr )
{
	*numptr = 0;
	int num = 0;

	int seri;
	CShdElem* selem;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		selem = (*this)( seri );
		_ASSERT( selem );
		if( selem->type == SHDDESTROYED ){
			num++;
		}
	}

	*numptr = num;

	return 0;
}


int CShdHandler::GetFirstParentNo( int* parnoptr )
{
	int firstparentno;
	firstparentno = 1;//!!!!!
	int seri;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		CShdElem* selem;
		selem = (*this)( seri );
		if( selem->type == SHDPART ){
			if( seri > firstparentno ){
				firstparentno = seri;
			}
		}
	}
	*parnoptr = firstparentno;

	return 0;
}

int CShdHandler::CreateMorphTable()
{
	int ret;
	if( m_ppm ){
		free( m_ppm );
		m_ppm = 0;
	}
	m_mnum = 0;
	
	ret = GetMorphElem( 0, &m_mnum, 0 );
	if( ret ){
		DbgOut( "sh : CreateMorphTable : sh GetMorphElem 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( m_mnum > 0 ){
		m_ppm = (CShdElem**)malloc( sizeof(CShdElem*) * m_mnum );
		if( !m_ppm ){
			DbgOut( "sh : CreateMorphTable m_ppm alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int getnum = 0;
		ret = GetMorphElem( m_ppm, &getnum, m_mnum );
		if( ret || (getnum != m_mnum) ){
			DbgOut( "sh : CreateMorphTable : sh GetMorphElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CShdHandler::GetVertPosInfScope( CInfScope* curis, int vertno, D3DXVECTOR3* posptr )
{


	CD3DDisp* d3dptr = curis->d3ddisp;
	if( !d3dptr ){
		_ASSERT( 0 );
		return 1;
	}

	if( (vertno < 0) || (vertno >= (int)d3dptr->m_numTLV) ){
		_ASSERT( 0 );
		return 1;
	}

	SKINVERTEX* skinv;	
	_ASSERT( d3dptr->m_skinv );
	skinv = d3dptr->m_skinv + vertno;
		
	posptr->x = skinv->pos[0];
	posptr->y = skinv->pos[1];
	posptr->z = skinv->pos[2];

	return 0;
}

int CShdHandler::GetTopJoints( int arrayleng, CShdElem** dstarray, int* getnum )
{
	int setno = 0;
	int seri;
	for( seri = 0; seri < s2shd_leng; seri++ ){
		CShdElem* curse = (*this)( seri );
		_ASSERT( curse );

		if( curse->IsJoint() ){
			int topflag = 0;
			CShdElem* parse = curse->parent;
			if( !parse ){
				topflag = 1;
			}else{
				if( !parse->IsJoint() ){
					topflag = 1;
				}
			}

			if( topflag ){
				if( arrayleng <= setno ){
					_ASSERT( 0 );
					return 1;
				}
				*( dstarray + setno ) = curse;
				setno++;
			}
		}
	}

	*getnum = setno;

	return 0;
}

