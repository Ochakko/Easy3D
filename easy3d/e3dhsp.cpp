

//
//		HSP DLL test ( for ver2.6 )
//				onion software/onitama 2002/12
//
#define _CRTDBG_MAP_ALLOC

#include <windows.h>

#include <process.h>
#include <advwin32.h>


//#include <windowsx.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//#include <D3DX9.h>

#include "hspdll.h"
#include "hspsdk.h"

#include <crtdbg.h>
#include <coef.h>
#include <basedat.h>

#define DBGH
#include <dbg.h>

#include "d3dapp.h"
#include "handlerset.h"
#include "d3dfont.h"

#include <lightdata.h>
#include <quaternion.h>
#include <quaternion2.h>

#include <groundbmp.h>

#include <bgdisp2.h>
#include "mysprite.h"

#include <commctrl.h> //for progress bar


#include <naviline.h>
#include <navipoint.h>

#include <qhandler.h>

#include "npcleardata.h"

#include <dibfile.h>
#include <avidata.h>
#include <xfont.h>

#include "bsphere.h"
#include <texbank.h>

#include "HuSound.h"

#include <cpuid.h>
#include <rtsc.h>
#include <asmfunc.h>

#define E3DHSPVERSION	1045

#define POSITIONMIN	-5000.0f
#define POSITIONMAX 5000.0f

//////////

/***	
static HANDLE s_hThread1 = NULL;
static DWORD s_dwMainId1;
static HANDLE s_hThread2 = NULL;
static DWORD s_dwMainId2;
static HANDLE s_hThread3 = NULL;
static DWORD s_dwMainId3;

DWORD WINAPI ThreadFunc( LPVOID	lpThreadParam );
***/


typedef struct tag_threadtable
{
	int threadid;
	HANDLE hThread;
	DWORD dwId;

	int returnval;
	int returnval2;

	char* filename;
	char* filename2;
	char* filename3;
	char* filename4;
	int mult100;
	int adjustuvflag;

	char* bufptr;
	int bufsize;

	int maxx;
	int maxz;
	int divx;
	int divz;
	int maxheight;

}THREADTABLE;


#define THMAX	4098
static THREADTABLE s_thtable[THMAX];
static int s_threadnum = 0;



/////////////////


////////////////
// global

//float g_specular = 1.0f;
//float g_ambient = 1.0f;

CRITICAL_SECTION g_crit_hshead;
//CRITICAL_SECTION g_crit_tempmp; 
CRITICAL_SECTION g_crit_restore;

int g_proj_near = 100;
int g_proj_far = 10000;
int g_proj_fov = 45;
float g_fovindex[ FOVINDEXSIZE + 1 ];

int g_renderstate[ D3DRS_BLENDOP + 1 ];
int g_rsindex[80] = {
    D3DRS_ZENABLE, D3DRS_FILLMODE, D3DRS_SHADEMODE, D3DRS_LINEPATTERN, D3DRS_ZWRITEENABLE, 
	D3DRS_ALPHATESTENABLE, D3DRS_LASTPIXEL, D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_CULLMODE,

    D3DRS_ZFUNC, D3DRS_ALPHAREF, D3DRS_ALPHAFUNC, D3DRS_DITHERENABLE, D3DRS_ALPHABLENDENABLE,
	D3DRS_FOGENABLE, D3DRS_SPECULARENABLE, D3DRS_ZVISIBLE, D3DRS_FOGCOLOR, D3DRS_FOGTABLEMODE,
    
	D3DRS_FOGSTART, D3DRS_FOGEND, D3DRS_FOGDENSITY, D3DRS_EDGEANTIALIAS, D3DRS_ZBIAS,
	D3DRS_RANGEFOGENABLE, D3DRS_STENCILENABLE, D3DRS_STENCILFAIL, D3DRS_STENCILZFAIL, D3DRS_STENCILPASS,

    D3DRS_STENCILFUNC, D3DRS_STENCILREF, D3DRS_STENCILMASK, D3DRS_STENCILWRITEMASK, D3DRS_TEXTUREFACTOR,
	D3DRS_WRAP0, D3DRS_WRAP1, D3DRS_WRAP2, D3DRS_WRAP3, D3DRS_WRAP4,
    
	D3DRS_WRAP5, D3DRS_WRAP6, D3DRS_WRAP7, D3DRS_CLIPPING, D3DRS_LIGHTING,
	D3DRS_AMBIENT, D3DRS_FOGVERTEXMODE, D3DRS_COLORVERTEX, D3DRS_LOCALVIEWER, D3DRS_NORMALIZENORMALS,

    D3DRS_DIFFUSEMATERIALSOURCE, D3DRS_SPECULARMATERIALSOURCE, D3DRS_AMBIENTMATERIALSOURCE, D3DRS_EMISSIVEMATERIALSOURCE, D3DRS_VERTEXBLEND,
	D3DRS_CLIPPLANEENABLE, D3DRS_SOFTWAREVERTEXPROCESSING, D3DRS_POINTSIZE, D3DRS_POINTSIZE_MIN, D3DRS_POINTSPRITEENABLE,

    D3DRS_POINTSCALEENABLE, D3DRS_POINTSCALE_A, D3DRS_POINTSCALE_B, D3DRS_POINTSCALE_C, D3DRS_MULTISAMPLEANTIALIAS,
	D3DRS_MULTISAMPLEMASK, D3DRS_PATCHEDGESTYLE, D3DRS_PATCHSEGMENTS, D3DRS_DEBUGMONITORTOKEN, D3DRS_POINTSIZE_MAX,

    D3DRS_INDEXEDVERTEXBLENDENABLE, D3DRS_COLORWRITEENABLE, D3DRS_TWEENFACTOR, D3DRS_BLENDOP, -1,
	-1,-1,-1,-1,-1
};

static int s_partialrs[ 22 ] = {
	D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_LIGHTING, D3DRS_CULLMODE, D3DRS_SHADEMODE,
	D3DRS_WRAP0, D3DRS_WRAP1, D3DRS_DITHERENABLE, D3DRS_SPECULARENABLE, D3DRS_ZENABLE, 
	D3DRS_AMBIENT, D3DRS_NORMALIZENORMALS, D3DRS_VERTEXBLEND, D3DRS_ZWRITEENABLE, D3DRS_ZFUNC, 
	D3DRS_ALPHAREF, D3DRS_ALPHAFUNC, D3DRS_ALPHABLENDENABLE, D3DRS_MULTISAMPLEANTIALIAS, D3DRS_MULTISAMPLEMASK,
	D3DRS_BLENDOP, D3DRS_TEXTUREFACTOR
	
};



CNaviLine* g_nlhead = 0;


UINT g_miplevels = 1;//!!!!!!!!!!!!!!!!
DWORD g_mipfilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR;

CCpuid g_cpuinfo;

RTSC g_rtsc;

int e3dRestoreDeviceObject();
int e3dInvalidateDeviceObject();


///////////////////
// extern
extern CD3DApplication* g_pD3DApp = 0;
extern CLightData* g_lighthead;// d3ddisp.cppで定義、使用。

extern CHandlerSet* g_bbhs;

extern CTexBank* g_texbnk;// shdhandler.cpp

//////////////////
// static
static CHandlerSet* hshead = 0;
static CAviData* avihead = 0;
static CXFont* xfonthead = 0;

static HWND s_hwnd = 0;
static HWND s_PBwnd = 0;
static HINSTANCE s_inst = 0;


static CBgDisp2* s_bgdisp = 0;
static DWORD s_clientWidth = 0;
static DWORD s_clientHeight = 0;

static D3DXMATRIX s_matView;

static LPD3DXSPRITE	s_d3dxsprite = 0;
static CMySprite* s_spritehead = 0;

static CQHandler* s_qh = 0;
static CQuaternion2 s_cameraq( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );

static CHuSound* s_HS = 0;

static int s_multisample = 0;

static CHandlerSet* GetHandlerSet( int hsid );
static CAviData* GetAviData( int aviid );
static CXFont* GetXFont( int fontid );
static CLightData* GetLightData( int lid );
static CMySprite* GetSprite( int spriteid );

static CNaviLine* GetNaviLine( int nlid );

static int ChainNewHandlerSet( CHandlerSet* newhs );

static int Restore( CHandlerSet* hsptr, int billboardid );
static int FreeHandlerSet();
static int DirToTheOtherXZ( E3DCHARACTERSET* e3d1, E3DCHARACTERSET e3d2 );
static int DirToTheOther( E3DCHARACTERSET* e3d1, E3DCHARACTERSET e3d2 );
static int SeparateFrom( int hsid1, int hsid2, float dist );
static int CheckOutOfRange( E3DCHARACTERSET* charaptr );
static int CreateAfterImage( int hsid, LPDIRECT3DDEVICE9 pdev, int imagenum );
static int MoveGirlXZ( int hsid, int keyid );

static int SetModuleDir();
static int SetResDir0();
static int SetResDir( char* filename, CHandlerSet* hsptr );

static int Bye( HWND srcwnd );
static int DestroySoundObject();

static int SetFovIndex();

static int SetDefaultRenderState();
static int SetPartialRenderState( LPDIRECT3DDEVICE9 pdev );

static int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec );
static int DestroyHandlerSet( int hsid );
static int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag );


static int CatmullRomPoint( D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* p3, D3DXVECTOR3* p4, float t, D3DXVECTOR3* resp );
static int GetScreenPos( D3DXVECTOR3 vec3d, int* x2dptr, int* y2dptr, int* validptr );

static int CalcMatView();
static int ScreenToObj( D3DXVECTOR3* objpos, D3DXVECTOR3 scpos );
static int GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir );

static int DestroyTexture( int hsid, int bbid, int spriteid );


static int s_cameratype = 0;
static D3DXVECTOR3 s_camerapos( 0.0f, 0.0f, 4200.0f );
static float s_camera_degxz = 0.0f;
static float s_camera_degy = 0.0f;
static D3DXVECTOR3 s_cameratarget( 0.0f, 0.0f, 0.0f );
static D3DXVECTOR3 s_cameraupvec( 0.0f, 1.0f, 0.0f );
static float s_camera_twist = 0.0f;

static int byeflag = 0;




static int LoadMQOFileAsGround_F( char* tempname1, int mult100, HWND hwnd, int adjustuvflag, int* hsidptr );
static DWORD WINAPI ThreadLoadMQOFileAsGround( LPVOID lpThreadParam );

static int SigLoad_F( char* tempname, int mult100, int adjustuvflag, int* hsidptr );
static DWORD WINAPI ThreadSigLoad( LPVOID lpThreadParam );

static int SigLoadFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, int mult100, int* hsidptr );
static DWORD WINAPI ThreadSigLoadFromBuf( LPVOID lpThreadParam );

static int LoadMQOFileAsMovableArea_F( char* tempname, int mult100, int* hsidptr );
static DWORD WINAPI ThreadLoadMQOFileAsMovableArea( LPVOID lpThreadParam );

static int LoadMQOFileAsMovableAreaFromBuf_F( char* bufptr, int bufsize, int mult100, int* hsidptr );
static DWORD WINAPI ThreadLoadMQOFileAsMovableAreaFromBuf( LPVOID lpThreadParam );

static int LoadGroundBMP_F( char* tempname1, char* tempname2, char* tempname3, char* tempname4, int maxx, int maxz, int divx, int divz, int maxheight, int TLmode, int* hsidptr );
static DWORD WINAPI ThreadLoadGroundBMP( LPVOID lpThreadParam );

static int LoadGNDFile_F( char* tempname, int adjustuvflag, int mult100, int* hsidptr );
static DWORD WINAPI ThreadLoadGNDFile( LPVOID lpThreadParam );

static int LoadGNDFileFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, int* hsidptr );
static DWORD WINAPI ThreadLoadGNDFileFromBuf( LPVOID lpThreadParam );

static int AddMotion_F( int hsid, char* tempname, int* cookieptr, int* maxnumptr );
static DWORD WINAPI ThreadAddMotion( LPVOID lpThreadParam );

static int AddMotionFromBuf_F( int hsid, int datakind, char* bufptr, int bufsize, int* cookieptr, int* maxnumptr );
static DWORD WINAPI ThreadAddMotionFromBuf( LPVOID lpThreadParam );



//static D3DXVECTOR3 s_cameradir( 0.0f, 0.0f, -1.0f );
static int s_afterimagenum = 5;

static float s_movestep = 100.0f;

static LONGLONG s_timerfreq = 0;

static int s_usertexcnt = 0;


int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
 {
	
	switch( fdwReason ){
	case DLL_PROCESS_ATTACH:
		//OpenDbgFile();	
		
		//SetModuleDir();
		//SetResDir();
		break;
	
	case DLL_THREAD_ATTACH:
		//OpenDbgFile();
		
		break;
	case DLL_THREAD_DETACH:
		
		//CloseDbgFile();
		break;
	default:
		break;
	
	}

	//DbgOut( "DllMain : fdwReason %d\n", fdwReason );

	return TRUE ;
}



EXPORT BOOL WINAPI E3DInit( BMSCR* bm, int p2, int p3, int p4 )
{
	//type 2
	
	int thno;
	for( thno = 0; thno < THMAX; thno++ ){
		s_thtable[thno].threadid = -1;
		s_thtable[thno].hThread = NULL;
		s_thtable[thno].dwId = -1;
		s_thtable[thno].returnval = -1;
		s_thtable[thno].returnval2 = -1;

		s_thtable[thno].filename = 0;
		s_thtable[thno].filename2 = 0;
		s_thtable[thno].filename3 = 0;
		s_thtable[thno].filename4 = 0;
		s_thtable[thno].mult100 = 100;
		s_thtable[thno].adjustuvflag = 0;

		s_thtable[thno].bufptr = 0;
		s_thtable[thno].bufsize = 0;

		s_thtable[thno].maxx = 0;
		s_thtable[thno].maxz = 0;
		s_thtable[thno].divx = 0;
		s_thtable[thno].divz = 0;
		s_thtable[thno].maxheight = 0;
	}
	s_threadnum = 0;


	InitializeCriticalSection( &g_crit_hshead );
	//InitializeCriticalSection( &g_crit_tempmp );
	InitializeCriticalSection( &g_crit_restore );

	HRESULT hr;

	int fullscflag = p2;
	int bits = p3;
	int multisamplenum = p4;

	if( bits <= 16 ){
		bits = 16;
	}else if( (bits > 16) && (bits < 32) ){
		bits = 32;
	}else if( bits >= 32 ){
		bits = 32;
	}

	if( multisamplenum < 2 )
		multisamplenum = 0;
	if( multisamplenum > 16 )
		multisamplenum = 16;


	s_multisample = multisamplenum;


	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	g_cpuinfo.CheckCPU();
	DbgOut( 
		"CPU Vender: \"%s\"\n"
		"Family: %d  Model: %d  Stepping ID: %d\n"
		"Supported CPUID: %d\n"
		"Supported MMX: %d\n"
		"Supported SSE: %d\n"
		"Supported SSE2: %d\n"
		"Supported 3DNow!: %d\n"
		"Supported Enhanced 3DNow!: %d\n"
		, g_cpuinfo.vd.id
		, g_cpuinfo.dwFamily, g_cpuinfo.dwModel, g_cpuinfo.dwSteppingId
		, g_cpuinfo.bCPUID, g_cpuinfo.bMMX, g_cpuinfo.bSSE, g_cpuinfo.bSSE2, g_cpuinfo.b3DNow, g_cpuinfo.bE3DNow
	);

	/***
	if( g_cpuinfo.bSSE ){
		AsmSetMXCSR( -1 );
		AsmSetMXCSR( 1 );
	}
	***/

	//OpenDbgFile();	
		
	SetModuleDir();
	SetResDir( 0, 0 );

	LARGE_INTEGER timerfreq;
	QueryPerformanceFrequency( &timerfreq );
	s_timerfreq = timerfreq.QuadPart;

	if( g_pD3DApp ){
		//すでに作成済なので、return
		return 0;
	}

	g_pD3DApp = new CD3DApplication();
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	

	s_hwnd = bm->hwnd;
	s_inst = (HINSTANCE)(bm->hInst);


	//InitCommonControls(); 	// for progress bar
	INITCOMMONCONTROLSEX cominfo;
	cominfo.dwSize = sizeof( INITCOMMONCONTROLSEX );
	cominfo.dwICC = ICC_PROGRESS_CLASS;

	InitCommonControlsEx( &cominfo ); 	// for progress bar

	s_PBwnd = 0;



    if( FAILED( g_pD3DApp->Create( (HINSTANCE)bm->hInst, s_hwnd, fullscflag, bits, s_multisample ) ) ){
		_ASSERT( 0 );
        return 1;
	}	
		

	RECT clirect;
	int clientWidth, clientHeight;
	GetClientRect( s_hwnd, &clirect );
	clientWidth = clirect.right - clirect.left;
	clientHeight = clirect.bottom - clirect.top;

	DbgOut( "E3DInit : Width %d, Height %d\n", clientWidth, clientHeight );


	//初回のresizeは必要？？？？
    //return d3dApp.Run();


//d3dxsprite
	
	int ret;
	//HRESULT hr;


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( !s_d3dxsprite ){

		hr = D3DXCreateSprite( pdev, &s_d3dxsprite );
		if( (hr != D3D_OK) || !s_d3dxsprite ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
//billboard
	//g_bbhs
	if( !g_bbhs ){
		g_bbhs = new CHandlerSet();
		if( !g_bbhs ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->CreateHandler();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		g_bbhs->m_billboardflag = 1;// !!!!! ver1036 2004/3/14

		ret = g_bbhs->CreateBillboard();
		if( ret ){
			DbgOut( "E3DInit : g_bbhs CreateBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}

// directmusic

	DestroySoundObject();
    
    // Initialize COM <---- for direct music
	HRESULT hr1;
    hr1 = CoInitialize(NULL);

	if( hr1 == S_OK ){
		s_HS = new CHuSound();
		if( !s_HS ){
			DbgOut( "E3DInit : HuSound alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int ret;
		ret = s_HS->Init( s_hwnd );
		if( ret ){
			DbgOut( "E3DInit : HS Init error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		::MessageBox( s_hwnd, "COMの初期化に失敗しました。\n音は鳴らせません。", "エラーです。", MB_OK );
	}

////
	ret = SetFovIndex();
	_ASSERT( !ret );


////
	SetDefaultRenderState();

    hr = pdev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MINFILTER : error !!!\r\n" );
	}
    hr = pdev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MAGFILTER : error !!!\r\n" );
	}
    hr = pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_COLOROP : error !!!\r\n" );
	}
    hr = pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_ALPHAOP : error !!!\r\n" );
	}

	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_COLORARG1 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_COLORARG2 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_ADDRESSU : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_ADDRESSV : error !!!\r\n" );
	}

////

	if( !s_qh ){
		s_qh = new CQHandler();
		if( !s_qh ){
			DbgOut( "E3DInit : s_qh alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = s_qh->InitHandler();
		if( ret ){
			DbgOut( "E3DInit : s_qh InitHandler error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

/// texbank
	if( !g_texbnk ){
		g_texbnk = new CTexBank( g_miplevels, g_mipfilter );
		if( !g_texbnk ){
			DbgOut( "E3DInit : new CTexBank error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


///
	byeflag = 0;



	/***
//////// test thread

	s_hThread1 = BEGINTHREADEX(NULL, 0, ThreadFunc, 
		//(void*)this,
		(void*)&s_hThread1,
		0, &s_dwMainId1 );

	s_hThread2 = BEGINTHREADEX(NULL, 0, ThreadFunc, 
		//(void*)this,
		(void*)&s_hThread2,
		0, &s_dwMainId2 );

	s_hThread3 = BEGINTHREADEX(NULL, 0, ThreadFunc, 
		//(void*)this,
		(void*)&s_hThread3,
		0, &s_dwMainId3 );
	***/



	return 0;
}

/***
DWORD WINAPI ThreadFunc( LPVOID	lpThreadParam )
{

	HANDLE* thp = (HANDLE*)lpThreadParam;

	DbgOut( "E3D : ThreadFunc : Start %x\r\n", thp );

	DWORD cnt;
	for( cnt = 0; cnt < 1000; cnt++ )
	{
		DbgOut( "E3D ThreadFunc : %x, %d\r\n", thp, cnt );
		Sleep( 0 );
	}

	DbgOut( "E3D : ThreadFunc : End %x\r\n", thp );

	//_endthreadex( 0 );//<----ThreadFuncがreturnするときに、自動的に呼ばれる。

	return 0;
}
***/

EXPORT BOOL WINAPI E3DBye( BMSCR *bm, int p2, int p3, int p4 )
{
	//type $2

	/***
	if( g_cpuinfo.bSSE ){
		AsmSetMXCSR( -1 );
	}
	***/

	if( bm )
		Bye( bm->hwnd );
	else
		Bye( 0 );

	return 0;
}

int Bye( HWND srcwnd )
{
	if( byeflag == 1 )
		return 0;

	if( s_threadnum > 0 ){
		int tableno;
		for( tableno = 0; tableno < s_threadnum; tableno++ ){
			
			if( s_thtable[tableno].hThread ){
				WaitForSingleObject( s_thtable[tableno].hThread, 600000 );
				CloseHandle( s_thtable[tableno].hThread );
				s_thtable[tableno].hThread = NULL;

			
				if( s_thtable[tableno].filename ){
					free( s_thtable[tableno].filename );
					s_thtable[tableno].filename = 0;
				}
				if( s_thtable[tableno].filename2 ){
					free( s_thtable[tableno].filename2 );
					s_thtable[tableno].filename2 = 0;
				}
				if( s_thtable[tableno].filename3 ){
					free( s_thtable[tableno].filename3 );
					s_thtable[tableno].filename3 = 0;
				}
				if( s_thtable[tableno].filename4 ){
					free( s_thtable[tableno].filename4 );
					s_thtable[tableno].filename4 = 0;
				}
			}
		}

	}

	if( g_pD3DApp ){

		//_ASSERT( s_hwnd == srcwnd );

		//clean up
		g_pD3DApp->Cleanup3DEnvironment();
		//DestroyMenu( GetMenu(hWnd) );

		delete g_pD3DApp;
		g_pD3DApp = 0;
	
	}

	if( srcwnd )
		DestroyWindow( srcwnd );

	FreeHandlerSet();


	CAviData* curavi;
	CAviData* nextavi;
	curavi = avihead;
	while( curavi ){
		nextavi = curavi->next;

		delete curavi;
		curavi = nextavi;
	}
	avihead = 0;


	CXFont* curfont;
	CXFont* nextfont;
	curfont = xfonthead;
	while( curfont ){
		nextfont = curfont->next;

		delete curfont;
		curfont = nextfont;
	}
	xfonthead = 0;


	CLightData* curlight = g_lighthead;
	CLightData* nextlight;
	while( curlight ){
		nextlight = curlight->nextlight;

		delete curlight;
		curlight = nextlight;
	}
	g_lighthead = 0;


	if( s_bgdisp ){
		delete s_bgdisp;
		s_bgdisp = 0;
	}


	if( s_d3dxsprite ){
		s_d3dxsprite->Release();
		s_d3dxsprite = 0;
	}


	CMySprite* cursprite = s_spritehead;
	CMySprite* nextsprite;
	while( cursprite ){
		nextsprite = cursprite->next;

		delete cursprite;
		cursprite = nextsprite;
	}
	s_spritehead = 0;



	CNaviLine* curnl = g_nlhead;
	CNaviLine* nextnl;
	while( curnl ){
		nextnl = curnl->next;

		delete curnl;
		curnl = nextnl;
	}
	g_nlhead = 0;



	if( s_PBwnd ){
		DestroyWindow( s_PBwnd );
		s_PBwnd = 0;
	}

	if( s_qh ){
		delete s_qh;
		s_qh = 0;
	}

	if( g_texbnk ){
		delete g_texbnk;
		g_texbnk = 0;
	}


	DestroySoundObject();

	//PostQuitMessage(0);//<----- DestroyWindowの後では、たぶん意味無し。

	CloseDbgFile();

	
    // Close down COM
    CoUninitialize();

	DeleteCriticalSection( &g_crit_hshead );
	//DeleteCriticalSection( &g_crit_tempmp );
	DeleteCriticalSection( &g_crit_restore );

	byeflag = 1;

	return 0;
}

int DestroySoundObject()
{
	if( s_HS ){
		delete s_HS;
		s_HS = 0;
	}

	return 0;
}

int ChainNewHandlerSet( CHandlerSet* newhs )
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit


	CHandlerSet* curhs = hshead;
	CHandlerSet* largerhs = 0;
	CHandlerSet* smallerhs = 0;
	int ret, cmp;
	while( curhs ){
		cmp = newhs->SerialnoCmp( curhs );
		if( cmp < 0 ){
			largerhs = curhs;
			break;
		}else{
			smallerhs = curhs;
		}
		curhs = curhs->next;
	}

	if( !hshead ){
		//先頭に追加
		newhs->ishead = 1;
		hshead = newhs;
	}else{
		if( largerhs ){
			//largermpの前に追加。
			ret = largerhs->AddToPrev( newhs );
			if( ret ){
				_ASSERT( 0 );
				LeaveCriticalSection( &g_crit_hshead );//###### end crit
				return -1;
			}
			if( newhs->ishead )
				hshead = newhs;
		}else{
			//最後に追加。(smallermp の後に追加)
			_ASSERT( smallerhs );
			ret = smallerhs->AddToNext( newhs );
			if( ret ){
				_ASSERT( 0 );
				LeaveCriticalSection( &g_crit_hshead );//###### end crit
				return -1;
			}
		}
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit

	return 0;

}

EXPORT BOOL WINAPI E3DSigLoad( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// first : sig file name
	// second : pointer of handlerset id.
	// 3 adjustuvflag
	// 4 mult100
//first
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSigLoad : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DSigLoad : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : SigLoad : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//second	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "e3dhsp : SigLoad : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSigLoad : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
// 3:
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

// 4:
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	int ret;
	int temphsid = -1;
	ret = SigLoad_F( tempname, mult100, adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoad : SigLoad_F error %s !!!\n", tempname );
		_ASSERT( 0 );
		*hsidptr = -1;
		return 1;
	}
	*hsidptr = temphsid;

	
	return 0;
}

int SigLoad_F( char* tempname, int mult100, int adjustuvflag, int* hsidptr )
{
	EnterCriticalSection( &g_crit_hshead );//######## start crit

	int ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	float srcmult;
	srcmult = (float)mult100 / 100.0f;


	ret = newhs->LoadSigFile( tempname, adjustuvflag, srcmult );
	if( ret ){
		DbgOut( "easy3d : E3DCreateHandler : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3DSigLoad : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	
//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;


	return 0;
}


EXPORT BOOL WINAPI E3DRender( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//type $202
	// first : int handlerset no.
	// second : int withalpha
	// third : int framecnt <-- for afterimage
	// 4th : projmode
	// lastparent
	// siglightflag

	int hsid, withalpha, framecnt, projmode, lastparent;


	hsid = hei->HspFunc_prm_getdi(0);
		
	withalpha = hei->HspFunc_prm_getdi(0);

	framecnt = hei->HspFunc_prm_getdi(0);

	projmode = hei->HspFunc_prm_getdi(0);

	lastparent = hei->HspFunc_prm_getdi(0);

	siglightflag = hei->HspFunc_prm_getdi(1);

	//divmode = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック

//DbgOut( "e3dhsp : E3DRender : hsid %d, withalpha %d, framecnt %d, projmode %d\n",
//	   hsid, withalpha, framecnt, projmode );

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRender : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
	
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	/***
	s_matViewのセットは、E3DBeginSceneに移動！！！
	***/

	//LARGE_INTEGER lbeftime1;
	//LARGE_INTEGER lafttime1;

	//LARGE_INTEGER lbeftime2;
	//LARGE_INTEGER lafttime2;


	//QueryPerformanceCounter( &lbeftime1 );
	int* bonearray;
	if( lastparent != 0 ){
		curhs->SetBoneArray( lastparent );
		bonearray = curhs->m_bonearray;
	}else{
		bonearray = 0;
	}

	ret = curhs->Transform( s_camerapos, pdev, s_matView, framecnt, siglightflag, projmode, bonearray, 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//QueryPerformanceCounter( &lafttime1 );
	

	//QueryPerformanceCounter( &lbeftime2 );
	ret = curhs->Render( pdev, withalpha, s_matView );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//QueryPerformanceCounter( &lafttime2 );


	/***
	LONGLONG difftime1, difftime2;
	difftime1 = lafttime1.QuadPart - lbeftime1.QuadPart;
	difftime2 = lafttime2.QuadPart - lbeftime2.QuadPart;

	float time1, time2;
	time1 = (float)difftime1 / (float)s_timerfreq;
	time2 = (float)difftime2 / (float)s_timerfreq;


	unsigned char tempstate[256];
	GetKeyboardState( tempstate );
	if( (tempstate[VK_SHIFT] & 0x80) && (tempstate['D'] & 0x80) ){
		DbgOut( "Check Time : Transform Time %f, Render Time %f\n", time1, time2 );
	}
	***/

    //if( pdev && g_pD3DApp->m_bReady ){
	//	pdev->Present( NULL, NULL, NULL, NULL );
	//}

	/***
	{
		LARGE_INTEGER lbeftime1;
		LARGE_INTEGER lafttime1;

		LARGE_INTEGER lbeftime2;
		LARGE_INTEGER lafttime2;

		QueryPerformanceCounter( &lbeftime1 );
		Sleep( 0 );
		QueryPerformanceCounter( &lafttime1 );

		QueryPerformanceCounter( &lbeftime2 );
		Sleep( 1 );
		QueryPerformanceCounter( &lafttime2 );

		LONGLONG difftime1, difftime2;
		difftime1 = lafttime1.QuadPart - lbeftime1.QuadPart;
		difftime2 = lafttime2.QuadPart - lbeftime2.QuadPart;

		float time1, time2;
		time1 = (float)difftime1 / (float)s_timerfreq;
		time2 = (float)difftime2 / (float)s_timerfreq;

		DbgOut( "Check Time : time1 %f, time2 %f\n", time1, time2 );

	}
	***/


	return 0;
}

EXPORT BOOL WINAPI E3DPresent( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

    if( pdev && g_pD3DApp->m_bReady ){
//DbgOut( "E3DPresent\n" );
		pdev->Present( NULL, NULL, NULL, NULL );
	}

	
	return 0;
}

EXPORT BOOL WINAPI E3DBeginScene( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int skipflag;
	skipflag = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
//////////


	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}
    
	ret = g_pD3DApp->TestCooperative();
	_ASSERT( !ret );

	
   if( FAILED( pdev->BeginScene() ) ){
		_ASSERT( 0 );
        return 1;
	}

    // Clear the viewport
	HRESULT hres;

	if( skipflag == 0 ){
		hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		//hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET, 
							 0x000000ff,
							 //0x00ffffff,
							 1.0f, 0L );
	   if( hres != D3D_OK ){
			//_ASSERT( 0 );
		   DbgOut( "E3DBegin : Clear error !!!\n" );
			return 1;
	   }
	}
	CalcMatView();

//!!!!!!!!!!!!!!!!
//render bgdisp
//!!!!!!!!!!!!!!!!

	if( (skipflag == 0) && s_bgdisp ){
		ret = s_bgdisp->SetBgSize( s_clientWidth, s_clientHeight );
		if( ret ){
			return 1;
		}
		ret = s_bgdisp->MoveTexUV( s_matView );
		_ASSERT( !ret );

		ret = s_bgdisp->UpdateVertexBuffer();
		_ASSERT( !ret );

		ret = s_bgdisp->Render( pdev );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		} 
	}
	return 0;
}

EXPORT BOOL WINAPI E3DEndScene( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return -1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return -1;
	}


    pdev->EndScene();

	return 0;
}
///////////////

EXPORT BOOL WINAPI E3DAddMotion( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	//1 : handlerset id
	//2 : filename
	//3 : [out] motcookie
	//4 : [out] maxnum
	int curtype;

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);


	char tempname[MAX_PATH];
	char* nameptr = 0;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();			// パラメータ2:文字列
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DAddMotion : parameter type error !!!\n" );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DAddMotion : parameter error !!!\n" );
		return 1;
	}

	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : AddMotion : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

	
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMotion : parameter type error !!!\n" );
		return 1;
	}
	int* cookieptr;
	cookieptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数
	

	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMotion : parameter type error !!!\n" );
		return 1;
	}
	int* maxnumptr;
	maxnumptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック

////////

	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotion_F( hsid, tempname, &tempmotid, &tempnum );
	if( ret ){
		DbgOut( "E3DAddMotion : AddMotoin_F error %s !!!\n", tempname );
		_ASSERT( 0 );
		*cookieptr = -1;
		*maxnumptr = 0;
		return 1;
	}
	*cookieptr = tempmotid;
	*maxnumptr = tempnum;
	return 0;

}

int AddMotion_F( int hsid, char* tempname, int* cookieptr, int* maxnumptr )
{

	char* ext;
	int ch = '.';
	int filekind = -1;
	ext = strrchr( tempname, ch );
	if( !ext ){
		DbgOut( "E3DAddMotion : file not supported error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int cmp1, cmp2;
	cmp1 = strcmp( ext + 1, "qua" );
	cmp2 = strcmp( ext + 1, "mot" );
	if( (cmp1 != 0) && (cmp2 != 0) ){
		DbgOut( "E3DAddMotion : file not supported error !!!\n" );
		_ASSERT( 0 );
		return 1;		
	}

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


EnterCriticalSection( &(curhs->m_crit_addmotion) );//######## start crit
	
	int ret = 0;
	if( cmp1 == 0 ){
		ret = curhs->LoadQuaFile( tempname, cookieptr, maxnumptr );	
	}else if( cmp2 == 0 ){
		ret = curhs->LoadMotFile( tempname, cookieptr, maxnumptr );
	}

LeaveCriticalSection( &(curhs->m_crit_addmotion) );//###### end crit


	if( ret ){
		DbgOut( "E3DAddMotion : Load***File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DSetMotionKind( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	//1 : handlerset id
	//2 : motkind
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int motkind;
	motkind = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionKind : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetMotionKind( motkind );
	if( ret ){
		DbgOut( "E3DSetMotionKind : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DGetMotionKind( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	//1 : handlerset id
	//2 : motkindptr
	int curtype;
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int* motkindptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetMotionKind : parameter type error !!!\n" );
		return 1;
	}
	motkindptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionKind : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	ret = curhs->GetMotionKind( motkindptr );
	if( ret ){
		DbgOut( "E3DGetMotionKind : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	/***
	ret = curhs->SetMotionKind( *motkindptr );
	if( ret ){
		DbgOut( "E3DSetMotionKind : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/
	return 0;
}

EXPORT BOOL WINAPI E3DSetNewPause( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
	if ( *hei->er ) return *hei->er;		// エラーチェック
///////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPause : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nextno;
	nextno = curhs->SetNewPose();
	_ASSERT( nextno >= 0 );
	return -nextno;

}
EXPORT BOOL WINAPI E3DSetNewPose( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
	if ( *hei->er ) return *hei->er;		// エラーチェック
///////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPose : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nextno;
	nextno = curhs->SetNewPose();
	_ASSERT( nextno >= 0 );
	return -nextno;

}


EXPORT BOOL WINAPI E3DSetMotionStep( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	//1 : handlerset id
	//2 : motkindptr
	//3 : stepptr
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int motkind;
	motkind = hei->HspFunc_prm_getdi(0);

	int step;
	step = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionStep : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret = 0;
	ret = curhs->SetMotionStep( motkind, step );
	if( ret ){
		DbgOut( "E3DSetMotionStep : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DChkConflict( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : src handlerset id
	// 2 : chk handlerset id
	// (3,4 : rate 後で、拡張するかも？)
	int curtype;	
	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	int chkhsid;
	chkhsid = hei->HspFunc_prm_getdi(0);

	int* confflagptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConflict : parameter type error !!!\n" );
		return 1;
	}
	confflagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflict : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflict : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict( chkhs, fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflict : srchs ChkConflict error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	*confflagptr = conf;

	return -inviewflag;

}

//E3DChkConflict2 hsid1, partno1, hsid2, partno2, confflag
EXPORT BOOL WINAPI E3DChkConflict2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : hsid1
	// 2 : partno1
	// 3 : hsid2
	// 4 : partno2

	int curtype;	
	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	int srcpartno;
	srcpartno = hei->HspFunc_prm_getdi(0);

	int chkhsid;
	chkhsid = hei->HspFunc_prm_getdi(0);

	int chkpartno;
	chkpartno = hei->HspFunc_prm_getdi(0);


	int* confflagptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConflict2 : parameter type error !!!\n" );
		return 1;
	}
	confflagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflict2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflict2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict2( srcpartno, chkhs, chkpartno, fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflict2 : srchs ChkConflict2 error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	*confflagptr = conf;

	return -inviewflag;

}





EXPORT BOOL WINAPI E3DCreateAfterImage( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : imagenumptr
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int imagenum;
	imagenum = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( !ret );

	CreateAfterImage( hsid, pdev, imagenum );

	return 0;
}

int CreateAfterImage( int hsid, LPDIRECT3DDEVICE9 pdev, int imagenum )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "CreateAfterImage : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->CreateAfterImage( pdev, imagenum );
	if( ret ){
		DbgOut( "E3DCreateAfterImage : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;

}


EXPORT BOOL WINAPI E3DDestroyAfterImage( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	//1 : handlerset id

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
///////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyAfterImage : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret = 0;
	ret = curhs->DestroyAfterImage();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSetAlpha( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : usealphaflag

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int usealphaflag;
	usealphaflag = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float alpha;
	if( usealphaflag == 0 ){
		alpha = 1.0f;
	}else{
		alpha = 0.5f;
	}
	int ret = 0;
	ret = curhs->SetAlpha( alpha );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

EXPORT BOOL WINAPI E3DSetAlpha2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : alphaval
	int alphaval;
	alphaval = hei->HspFunc_prm_getdi(0);

	// 3 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

	// 4 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi( -1 );//!!!!


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DSetAlpha2 : hsid error !!!\n" );
			DbgOut( "e3dhsp : SetAlpha2 : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetAlpha2 : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	float alpha;
	if( mult != 0 ){
		alpha = (float)alphaval / (float)mult;
	}else{
		alpha = 0.0f;
	}

	if( alpha < 0.0f )
		alpha = 0.0f;
	if( alpha > 1.0f )
		alpha = 1.0f;

	
	int ret = 0;
	ret = curhs->SetAlpha( alpha, partno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}



EXPORT BOOL WINAPI E3DSetPos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : posxptr
	// 3 : posyptr
	// 4 : poszptr

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int posx;
	posx = hei->HspFunc_prm_getdi(0);

	int posy;
	posy = hei->HspFunc_prm_getdi(0);

	int posz;
	posz = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.position.x = (float)posx;
	curhs->m_charaset.position.y = (float)posy;
	curhs->m_charaset.position.z = (float)posz;
	
	curhs->SetPosition( curhs->m_charaset.position );

	return 0;
}
/////////////
EXPORT BOOL WINAPI E3DGetPos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : posxptr
	// 3 : posyptr
	// 4 : poszptr
	int curtype;
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int* posxptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetPos : parameter type error !!!\n" );
		return 1;
	}
	posxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	int* posyptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetPos : parameter type error !!!\n" );
		return 1;
	}
	posyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	int* poszptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetPos : parameter type error !!!\n" );
		return 1;
	}
	poszptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posxptr = (int)curhs->m_charaset.position.x;		
	*posyptr = (int)curhs->m_charaset.position.y;		
	*poszptr = (int)curhs->m_charaset.position.z;		

	return 0;

}


EXPORT BOOL WINAPI E3DSetDir( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : dirxptr (deg)
	// 3 : diryptr (deg)
	// 4 : dirzptr (deg)
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int dirx;
	dirx = hei->HspFunc_prm_getdi(0);

	int diry;
	diry = hei->HspFunc_prm_getdi(0);

	int dirz;
	dirz = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDir : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.rotate.x = (float)dirx;
	curhs->m_charaset.rotate.y = (float)diry;
	curhs->m_charaset.rotate.z = (float)dirz;

	int ret;
	ret = curhs->m_charaset.dirq.SetRotation( curhs->m_charaset.rotate.x, curhs->m_charaset.rotate.y, curhs->m_charaset.rotate.z );
	_ASSERT( !ret );
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();


	curhs->SetRotate( curhs->m_charaset.dirm );

	return 0;
}

EXPORT BOOL WINAPI E3DRotateInit( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateInit : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.rotate.x = 0.0f;
	curhs->m_charaset.rotate.y = 0.0f;
	curhs->m_charaset.rotate.z = 0.0f;

	int ret;
	ret = curhs->m_charaset.dirq.SetRotation( curhs->m_charaset.rotate.x, curhs->m_charaset.rotate.y, curhs->m_charaset.rotate.z );
	_ASSERT( !ret );
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();

	curhs->SetRotate( curhs->m_charaset.dirm );

	return 0;
}

EXPORT BOOL WINAPI E3DRotateX( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : dirxptr (deg)
	// 3 : diryptr (deg)
	// 4 : dirzptr (deg)

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int dirx;
	dirx = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(1);


	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateX : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.rotate.x = 0.0f;
	curhs->m_charaset.rotate.y = 0.0f;
	curhs->m_charaset.rotate.z = 0.0f;

	D3DXVECTOR3 axisx( 1.0f, 0.0f, 0.0f );
	CQuaternion2 qx, newq;
	qx.SetAxisAndRot( axisx, (float)dirx * (float)DEG2PAI / (float)mult );
	newq = qx * curhs->m_charaset.dirq;

	float savetwist = curhs->m_charaset.dirq.twist;
	curhs->m_charaset.dirq = newq;
	curhs->m_charaset.dirq.twist = savetwist;

	//int ret;
	//ret = curhs->m_charaset.dirq.SetRotation( curhs->m_charaset.rotate.x, curhs->m_charaset.rotate.y, curhs->m_charaset.rotate.z );
	//_ASSERT( !ret );
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();


	curhs->SetRotate( curhs->m_charaset.dirm );

	return 0;
}
EXPORT BOOL WINAPI E3DRotateY( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : diryptr (deg)
	// 3 : mult

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int diry;
	diry = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(1);


	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateY : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.rotate.x = 0.0f;
	curhs->m_charaset.rotate.y = 0.0f;
	curhs->m_charaset.rotate.z = 0.0f;

	D3DXVECTOR3 axisy( 0.0f, 1.0f, 0.0f );
	CQuaternion2 qy, newq;
	qy.SetAxisAndRot( axisy, (float)diry * (float)DEG2PAI / (float)mult );
	newq = qy * curhs->m_charaset.dirq;

	float savetwist = curhs->m_charaset.dirq.twist;
	curhs->m_charaset.dirq = newq;
	curhs->m_charaset.dirq.twist = savetwist;

	//int ret;
	//ret = curhs->m_charaset.dirq.SetRotation( curhs->m_charaset.rotate.x, curhs->m_charaset.rotate.y, curhs->m_charaset.rotate.z );
	//_ASSERT( !ret );
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();


	curhs->SetRotate( curhs->m_charaset.dirm );

	return 0;
}

EXPORT BOOL WINAPI E3DRotateZ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : dirzptr (deg)

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int dirz;
	dirz = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(1);


	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateZ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.rotate.x = 0.0f;
	curhs->m_charaset.rotate.y = 0.0f;
	curhs->m_charaset.rotate.z = 0.0f;

	D3DXVECTOR3 axisz( 0.0f, 0.0f, 1.0f );
	CQuaternion2 qz, newq;
	qz.SetAxisAndRot( axisz, (float)dirz * (float)DEG2PAI / (float)mult );
	newq = qz * curhs->m_charaset.dirq;

	float savetwist = curhs->m_charaset.dirq.twist;
	curhs->m_charaset.dirq = newq;
	curhs->m_charaset.dirq.twist = savetwist;

	//int ret;
	//ret = curhs->m_charaset.dirq.SetRotation( curhs->m_charaset.rotate.x, curhs->m_charaset.rotate.y, curhs->m_charaset.rotate.z );
	//_ASSERT( !ret );
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();


	curhs->SetRotate( curhs->m_charaset.dirm );

	return 0;
}


EXPORT BOOL WINAPI E3DTwist( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : stepptr
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int step;
	step = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DTwist : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.rotate.x = 0.0f;
	curhs->m_charaset.rotate.y = 0.0f;
	curhs->m_charaset.rotate.z = 0.0f;
	
	int ret;
	float steprad;
	D3DXVECTOR3 axis;
	D3DXVECTOR3 orgvec( 0.0f, 0.0f, -1.0f );

	ret = curhs->m_charaset.dirq.Rotate( &axis, orgvec );
	_ASSERT( !ret );
	DXVec3Normalize( &axis, &axis );


	float ftwist, settwist;
	ftwist = curhs->m_charaset.dirq.twist + (float)step;

	if( ftwist >= 360.0f ){
		while( ftwist >= 360.0f )
			ftwist -= 360.0f;
		settwist = ftwist;
	}else if( ftwist < 0.0f ){
		while( ftwist < 0.0f )
			ftwist += 360.0f;
		settwist = ftwist;
	}else{
		settwist = ftwist;
	}


	steprad = (float)step * (float)DEG2PAI;//変化分
	CQuaternion2 newq, multq;

	multq.SetAxisAndRot( axis, steprad );
	newq = multq * curhs->m_charaset.dirq;
	curhs->m_charaset.dirq = newq;

	//curhs->m_charaset.dirq.twist += steprad;//!!!!!!!!!!!!!!
	curhs->m_charaset.dirq.twist = settwist;

	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();

	curhs->SetRotate( curhs->m_charaset.dirm );

	return 0;
}



/***
EXPORT BOOL WINAPI E3DGetDir( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : dirxptr
	// 3 : diryptr
	// 4 : dirzptr
	
	int* hsidptr;
	hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ1:変数
	int hsid;
	hsid = *hsidptr;

	int* dirxptr;
	dirxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	int* diryptr;
	diryptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	int* dirzptr;
	dirzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		_ASSERT( 0 );
		return 1;
	}
	
	
	// !!!! 以下の方法は、X,Y,Z軸の順に回転した場合のみに、有効。
	// !!!! βが、-90～90　ということにも、注意。

	//	   　xx xy xz 
	//	R =  yx yy yz 
	//	   　zx zy zz 

	//	α β γ   γの角度     βの角度(-90～90) αの角度 
	//	Ｘ Ｙ Ｚ   atan2(xy,xx) -asin(xz)         asin(yz/cos(β)) if(zz<0)α=180-α  

	//	cos(β)がゼロになるときα＝０、β＝９０or－９０としてγを計算する方法
	//	（γをゼロにするとベータの符号によって式を変える必要がある）

	//	α β γ   γの角度      βの角度(-90or90) αの角度 
	//	Ｘ Ｙ Ｚ   atan2(-yx,yy) -asin(xz)         0 
	
	
	float xx, xy, xz, yx, yy, yz, zx, zy, zz;
	xx = curhs->m_charaset.dirm._11;
	xy = curhs->m_charaset.dirm._12;
	xz = curhs->m_charaset.dirm._13;

	yx = curhs->m_charaset.dirm._21;
	yy = curhs->m_charaset.dirm._22;
	yz = curhs->m_charaset.dirm._23;

	zx = curhs->m_charaset.dirm._31;
	zy = curhs->m_charaset.dirm._32;
	zz = curhs->m_charaset.dirm._33;
	
	float ganma, beta, alpha, tempbeta;
	float chkxz, chkcos;

	tempbeta = (float)-asin( xz );
	

///////////////
	beta = tempbeta;
	
/////////////

	float cosbeta;
	cosbeta = (float)cos( beta );//!!!!!

	if( cosbeta == 0.0f ){
		ganma = (float)atan2( -yx, yy );
		alpha = 0.0f;
	}else{
		ganma = (float)atan2( xy, xx );
		alpha = (float)asin( yz / cosbeta );
		if( zz < 0.0f )
			alpha = PI - alpha;
	}

	float fRad2Deg = (float)PAI2DEG;
	curhs->m_charaset.rotate.x = alpha * fRad2Deg;
	curhs->m_charaset.rotate.y = beta * fRad2Deg;
	curhs->m_charaset.rotate.z = ganma * fRad2Deg;

	*dirxptr = (int)curhs->m_charaset.rotate.x;
	*diryptr = (int)curhs->m_charaset.rotate.y;
	*dirzptr = (int)curhs->m_charaset.rotate.z;

	return 0;
}

EXPORT BOOL WINAPI E3DSetDir( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : dirxptr (deg)
	// 3 : diryptr (deg)
	// 4 : dirzptr (deg)
	
	int* hsidptr;
	hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ1:変数
	int hsid;
	hsid = *hsidptr;

	int* dirxptr;
	dirxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	int* diryptr;
	diryptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	int* dirzptr;
	dirzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.rotate.x = (float)*dirxptr;
	curhs->m_charaset.rotate.y = (float)*diryptr;
	curhs->m_charaset.rotate.z = (float)*dirzptr;

	int ret;
	ret = curhs->m_charaset.dirq.SetRotation( curhs->m_charaset.rotate.x, curhs->m_charaset.rotate.y, curhs->m_charaset.rotate.z );
	_ASSERT( !ret );
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();


	curhs->SetRotate( curhs->m_charaset.dirm );

	return 0;
}
***/


EXPORT BOOL WINAPI E3DPosForward( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : stepptr
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int step;
	step = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPosForward : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	/***
	CQuaternion q, qx, qy, qz;
	float cosx, sinx, cosy, siny, cosz, sinz;
	float fDeg2Pai = (float)DEG2PAI;
	cosx = (float)cos( curhs->m_charaset.rotate.x * 0.5f * fDeg2Pai );
	sinx = (float)sin( curhs->m_charaset.rotate.x * 0.5f * fDeg2Pai );
	cosy = (float)cos( curhs->m_charaset.rotate.y * 0.5f * fDeg2Pai );
	siny = (float)sin( curhs->m_charaset.rotate.y * 0.5f * fDeg2Pai );
	cosz = (float)cos( curhs->m_charaset.rotate.z * 0.5f * fDeg2Pai );
	sinz = (float)sin( curhs->m_charaset.rotate.z * 0.5f * fDeg2Pai );

	qx.SetParams( cosx, sinx, 0.0f, 0.0f );
	qy.SetParams( cosy, 0.0f, siny, 0.0f );
	qz.SetParams( cosz, 0.0f, 0.0f, sinz );

	q = qx * qy * qz;

	//CMatrix2 rotmat;
	//rotmat = q.MakeRotMat();
	***/
	int ret;
	D3DXVECTOR3 orgvec( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 stepvec( 0.0f, 0.0f, 1.0f );
	
	//ret = q.Rotate( &stepvec, orgvec );
	//_ASSERT( !ret );

	ret = curhs->m_charaset.dirq.Rotate( &stepvec, orgvec );
	_ASSERT( !ret );

	curhs->m_charaset.position.x += (float)step * stepvec.x;
	curhs->m_charaset.position.y += (float)step * stepvec.y;
	curhs->m_charaset.position.z += (float)step * stepvec.z;


	curhs->SetPosition( curhs->m_charaset.position );

	return 0;
}

////////////////
EXPORT BOOL WINAPI E3DCloseTo( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : change handlerset id
	// 2 : src handlerset id
	int chghsid;
	chghsid = hei->HspFunc_prm_getdi(0);

	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	CHandlerSet* chghs = GetHandlerSet( chghsid );
	if( !chghs ){
		DbgOut( "E3DCloseTo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DCloseTo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// position
	float distmag;
	int zeroflag = 0;
	D3DXVECTOR3 vec3, nvec3;
	vec3.x = srchs->m_charaset.position.x - chghs->m_charaset.position.x;
	vec3.y = srchs->m_charaset.position.y - chghs->m_charaset.position.y;
	vec3.z = srchs->m_charaset.position.z - chghs->m_charaset.position.z;
	if( (vec3.x != 0.0f) || (vec3.y != 0.0f) && (vec3.z != 0.0f) )
		D3DXVec3Normalize( &nvec3, &vec3 );
	
	distmag = ( vec3.x * vec3.x + vec3.y * vec3.y + vec3.z * vec3.z );
	if( distmag > (s_movestep * s_movestep) ){
		chghs->m_charaset.position.x += s_movestep * nvec3.x;
		chghs->m_charaset.position.y += s_movestep * nvec3.y;
		chghs->m_charaset.position.z += s_movestep * nvec3.z;		
	}else{
		zeroflag = 1;//!!!!!!!!!!
		chghs->m_charaset.position.x = srchs->m_charaset.position.x;
		chghs->m_charaset.position.y = srchs->m_charaset.position.y;
		chghs->m_charaset.position.z = srchs->m_charaset.position.z;				
	}

	/***
	D3DXVECTOR2 vec2;
	D3DXVECTOR2 nvec2;

	vec2.x = srchs->m_charaset.position.x - chghs->m_charaset.position.x;
	vec2.y = srchs->m_charaset.position.z - chghs->m_charaset.position.z;


	D3DXVec2Normalize( &nvec2, &vec2 );
	chghs->m_charaset.position.x += s_movestep * nvec2.x;
	chghs->m_charaset.position.z += s_movestep * nvec2.y;
	***/


	int ret;
	ret = chghs->SetPosition( chghs->m_charaset.position );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( zeroflag == 0 ){
		ret = DirToTheOther( &(chghs->m_charaset), srchs->m_charaset );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		//ret = chghs->SetRotate( chghs->m_charaset.rotate );
		ret = chghs->SetRotate( chghs->m_charaset.dirm );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
EXPORT BOOL WINAPI E3DDirToTheOtherXZ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : change handlerset id
	// 2 : src handlerset id

	int chghsid;
	chghsid = hei->HspFunc_prm_getdi(0);

	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	CHandlerSet* chghs = GetHandlerSet( chghsid );
	if( !chghs ){
		DbgOut( "E3DDirToTheOtherXZ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DDirToTheOtherXZ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = DirToTheOtherXZ( &(chghs->m_charaset), srchs->m_charaset );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//ret = chghs->SetRotate( chghs->m_charaset.rotate );
	ret = chghs->SetRotate( chghs->m_charaset.dirm );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EXPORT BOOL WINAPI E3DDirToTheOther( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : change handlerset id
	// 2 : src handlerset id
	int chghsid;
	chghsid = hei->HspFunc_prm_getdi(0);

	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	CHandlerSet* chghs = GetHandlerSet( chghsid );
	if( !chghs ){
		DbgOut( "E3DDirToTheOther : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DDirToTheOther : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = DirToTheOther( &(chghs->m_charaset), srchs->m_charaset );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	//ret = chghs->SetRotate( chghs->m_charaset.rotate );
	ret = chghs->SetRotate( chghs->m_charaset.dirm );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSeparateFrom( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : chg handlerset id
	// 2 : src handlerset id

	int chghsid;
	chghsid = hei->HspFunc_prm_getdi(0);

	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	int dist;
	dist = hei->HspFunc_prm_getdi(100);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	int ret;
	ret = SeparateFrom( chghsid, srchsid, (float)dist );
	_ASSERT( !ret );

	/***
	CHandlerSet* chghs = GetHandlerSet( chghsid );
	if( !chghs ){
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		_ASSERT( 0 );
		return 1;
	}

	

	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;

	vec.x = srchs->m_charaset.position.x - chghs->m_charaset.position.x;
	vec.y = srchs->m_charaset.position.z - chghs->m_charaset.position.z;

	D3DXVec2Normalize( &nvec, &vec );
	chghs->m_charaset.position.x -= s_movestep * nvec.x;
	chghs->m_charaset.position.z -= s_movestep * nvec.y;
	***/

	return 0;
}

int SeparateFrom( int hsid1, int hsid2, float dist ){

	CHandlerSet* chghs = GetHandlerSet( hsid1 );
	if( !chghs ){
		DbgOut( "E3D SeparateFrom : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* srchs = GetHandlerSet( hsid2 );
	if( !srchs ){
		DbgOut( "E3D SeparateFrom : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;

	vec.x = srchs->m_charaset.position.x - chghs->m_charaset.position.x;
	vec.y = srchs->m_charaset.position.z - chghs->m_charaset.position.z;

	D3DXVec2Normalize( &nvec, &vec );
	chghs->m_charaset.position.x -= dist * nvec.x;
	chghs->m_charaset.position.z -= dist * nvec.y;


	chghs->SetPosition( chghs->m_charaset.position );

	return 0;
}

/////////////////

EXPORT BOOL WINAPI E3DGetCameraPos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : posxptr
	// 2 : posyptr
	// 3 : poszptr
	int curtype;
	int* posxptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCameraPos : parameter type error !!!\n" );
		return 1;
	}
	posxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ1:変数

	int* posyptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCameraPos : parameter type error !!!\n" );
		return 1;
	}
	posyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	int* poszptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCameraPos : parameter type error !!!\n" );
		return 1;
	}
	poszptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////
	
	*posxptr = (int)s_camerapos.x;
	*posyptr = (int)s_camerapos.y;
	*poszptr = (int)s_camerapos.z;
	

	return 0;
}

EXPORT BOOL WINAPI E3DSetCameraPos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : posxptr
	// 2 : posyptr
	// 3 : poszptr
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

	int posy;
	posy = hei->HspFunc_prm_getdi(0);

	int posz;
	posz = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	s_camerapos.x = (float)posx;
	s_camerapos.y = (float)posy;
	s_camerapos.z = (float)posz;

	return 0;
}
EXPORT BOOL WINAPI E3DGetCameraDeg( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : degxzptr
	// 2 : degyptr
	
	int curtype;
	int* degxzptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCameraDeg : parameter type error !!!\n" );
		return 1;
	}
	degxzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ1:変数

	int* degyptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCameraDeg : parameter type error !!!\n" );
		return 1;
	}
	degyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	*degxzptr = (int)s_camera_degxz;
	*degyptr = (int)s_camera_degy;
	

	return 0;
}

EXPORT BOOL WINAPI E3DSetCameraDeg( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : degxzptr
	// 2 : degyptr
	int degxz;
	degxz = hei->HspFunc_prm_getdi(0);

	int degy;
	degy = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	s_camera_degxz = (float)degxz;
	s_camera_degy = (float)degy;

	s_cameratype = 0;
	
	return 0;
}

EXPORT BOOL WINAPI E3DSetCameraTarget( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : targetxptr
	// 2 : targetyptr
	// 3 : targetzptr
	// 4 : upvecxptr
	// 5 : upvecyptr
	// 6 : upveczptr
	
	int targetx;
	targetx = hei->HspFunc_prm_getdi(0);

	int targety;
	targety = hei->HspFunc_prm_getdi(0);

	int targetz;
	targetz = hei->HspFunc_prm_getdi(0);


	int upvecx;
	upvecx = hei->HspFunc_prm_getdi(0);

	int upvecy;
	upvecy = hei->HspFunc_prm_getdi(0);

	int upvecz;
	upvecz = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	s_cameratarget.x = (float)targetx;
	s_cameratarget.y = (float)targety;
	s_cameratarget.z = (float)targetz;

	s_cameraupvec.x = (float)upvecx;//一時的に。後でツイストする。
	s_cameraupvec.y = (float)upvecy;
	s_cameraupvec.z = (float)upvecz;

	D3DXVec3Normalize( &s_cameraupvec, &s_cameraupvec );

	s_cameratype = 1;


	CalcMatView();//ツイスト前のmatView


// twistをしたs_cameraupvecを求める。
	D3DXMATRIX tempmatView;
	float cx, cy, cz;

	tempmatView = s_matView;
	cx = tempmatView._41;
	cy = tempmatView._42;
	cz = tempmatView._43;
	tempmatView._41 = 0.0f;
	tempmatView._42 = 0.0f;
	tempmatView._43 = 0.0f;



	D3DXVECTOR3 axisvec;
	CQuaternion2 addrot;
	axisvec = s_cameratarget - s_camerapos;
	DXVec3Normalize( &axisvec, &axisvec );
	addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
	
	
	D3DXQUATERNION viewqx;
	D3DXQuaternionRotationMatrix( &viewqx, &tempmatView );

	CQuaternion2 viewq;
	viewq.x = viewqx.x;
	viewq.y = viewqx.y;
	viewq.z = viewqx.z;
	viewq.w = viewqx.w;
	
	CQuaternion2 invview;
	viewq.inv( &invview );

	CQuaternion2 multq;
	multq = addrot * invview;

	D3DXVECTOR3 upv( 0.0f, 1.0f, 0.0f );
	multq.Rotate( &s_cameraupvec, upv );

	DXVec3Normalize( &s_cameraupvec, &s_cameraupvec );

	return 0;
}

///// static func
CHandlerSet* GetHandlerSet( int hsid )
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* curhs = hshead;
	while( curhs ){
		int cmp;
		cmp = curhs->SerialnoCmp( hsid );
		if( cmp == 0 ){
			LeaveCriticalSection( &g_crit_hshead );//###### end crit
			return curhs; // find
		}
		curhs = curhs->next;
	}
	LeaveCriticalSection( &g_crit_hshead );//###### end crit

	return 0;
}

CAviData* GetAviData( int aviid )
{
	CAviData* curavi = avihead;
	while( curavi ){
		if( curavi->serialno == aviid ) 
			return curavi; // find
		curavi = curavi->next;
	}
	return 0;
}

CXFont* GetXFont( int fontid )
{
	CXFont* curfont = xfonthead;
	while( curfont ){
		if( curfont->m_serialno == fontid ) 
			return curfont; // find
		curfont = curfont->next;
	}
	return 0;
}

CNaviLine* GetNaviLine( int nlid )
{
	CNaviLine* curnl = g_nlhead;
	while( curnl ){
		if( curnl->lineid == nlid ){
			return curnl; // find !!!
		}
		curnl = curnl->next;
	}

	return 0; // not found
}

int Restore( CHandlerSet* hsptr, int billboardid )
{

	EnterCriticalSection( &g_crit_restore );//######## start crit

	int ret;
	HRESULT hr;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

	/***
	ret = E3DRestore( m_girl1.handlerid, m_pd3dDevice, m_hWnd );
	if( ret ){
		_ASSERT( 0 );
		return -1;
	}

	if( m_girl1.afterimage ){
		ret = E3DCreateAfterImage( m_girl1.handlerid, m_pd3dDevice, m_girl1.afterimage );
		if( ret ){
			_ASSERT( 0 );
			return -1;
		}
	}
	***/
	

	_ASSERT( s_hwnd );

	RECT clirect;
	GetClientRect( s_hwnd, &clirect );
	s_clientWidth = clirect.right - clirect.left;
	s_clientHeight = clirect.bottom - clirect.top;

	ret = hsptr->Restore( pdev, s_hwnd, billboardid );
	if( ret ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}


	
    hr = pdev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_MINFILTER : error !!!\r\n" );
	}
    hr = pdev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_MAGFILTER : error !!!\r\n" );
	}
    hr = pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_COLOROP : error !!!\r\n" );
	}
    hr = pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_ALPHAOP : error !!!\r\n" );
	}

	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_COLORARG1 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_COLORARG2 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_ADDRESSU : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_ADDRESSV : error !!!\r\n" );
	}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	SetPartialRenderState( pdev );


/// lightのセット

    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );


	CLightData* curlight = g_lighthead;
	while( curlight ){
		switch( curlight->Type ){
		case D3DLIGHT_DIRECTIONAL:

			light.Type = D3DLIGHT_DIRECTIONAL;
			light.Direction = curlight->orgDirection;
			
			light.Diffuse.r = curlight->Diffuse.x;
			light.Diffuse.g = curlight->Diffuse.y;
			light.Diffuse.b = curlight->Diffuse.z;
			light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

			pdev->SetLight( curlight->lightid, &light );
			pdev->LightEnable( curlight->lightid, TRUE );

//DbgOut( "e3dhsp : Restore : directionallight : dir %f %f %f, rgb %f %f %f\n", 
//	   light.Direction.x, light.Direction.y, light.Direction.z,
//	   light.Diffuse.r, light.Diffuse.g, light.Diffuse.b );

			break;
		case D3DLIGHT_POINT:
			light.Type = D3DLIGHT_POINT;
			
			light.Diffuse.r = curlight->Diffuse.x;
			light.Diffuse.g = curlight->Diffuse.y;
			light.Diffuse.b = curlight->Diffuse.z;
			light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

			light.Position = curlight->orgPosition;

			if( curlight->Range != 0.0f )
				light.Range = sqrtf( curlight->Range );

			light.Attenuation0 = 0.4f;

			pdev->SetLight( curlight->lightid, &light );
			pdev->LightEnable( curlight->lightid, TRUE );

//DbgOut( "e3dhsp : Restore : pointlight : col %f %f %f, pos %f %f %f, range %f\n",
//	   light.Diffuse.r, light.Diffuse.g, light.Diffuse.b,
//	   light.Position.x, light.Position.y, light.Position.z,
//	   light.Range );

			break;
		case D3DLIGHT_SPOT:
			light.Type = D3DLIGHT_SPOT;
			
			light.Diffuse.r = curlight->Diffuse.x;
			light.Diffuse.g = curlight->Diffuse.y;
			light.Diffuse.b = curlight->Diffuse.z;
			light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;
			
			light.Position = curlight->orgPosition;

			light.Direction = curlight->orgDirection;

			light.Range = sqrtf( curlight->Range );

			light.Falloff = 1.0f;
			light.Attenuation0 = 1.0f;
    
			light.Theta        = curlight->Phi;
			light.Phi          = curlight->Phi;

			pdev->SetLight( curlight->lightid, &light );
			pdev->LightEnable( curlight->lightid, TRUE );

			
			break;
		default:
			DbgOut( "e3dhsp : Restore : invalid light type warning %d\n", curlight->Type );

			pdev->LightEnable( curlight->lightid, FALSE );

			break;
		}

		curlight = curlight->nextlight;
	}


	LeaveCriticalSection( &g_crit_restore );//###### end crit

    
	return 0;
}

int FreeHandlerSet()
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* curhs;
	CHandlerSet* nexths;
	curhs = hshead;
	while( curhs ){
		nexths = curhs->next;

		delete curhs;
		curhs = nexths;
	}
	hshead = 0;

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	if( g_bbhs ){
		delete g_bbhs;
		g_bbhs = 0;
	}


	return 0;
}

static int DirToTheOtherXZ(  E3DCHARACTERSET* set1, E3DCHARACTERSET set2 )
{
	D3DXVECTOR2 basevec;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	
	basevec.x = 0.0f;
	basevec.y = -1.0f;

	vec.x = set2.position.x - set1->position.x;
	vec.y = set2.position.z - set1->position.z;

	D3DXVec2Normalize( &nvec, &vec );
	float dot;
	dot = D3DXVec2Dot( &basevec, &nvec );
	float rad;
	if( dot <= -1.0f )
		dot = -1.0f;
	else if( dot >= 1.0f )
		dot = 1.0f;

	rad = (float)acos( dot );
	float ccw;
	ccw = D3DXVec2CCW( &basevec, &nvec );

	float roty;
	if( ccw >= 0.0f ){
		roty = -rad;
	}else{
		roty = rad;
	}

	set1->rotate.x = 0.0f;
	set1->rotate.y = roty * (float)PAI2DEG;
	set1->rotate.z = 0.0f;

	int ret;
	ret = set1->dirq.SetRotation( set1->rotate.x, set1->rotate.y, set1->rotate.z );
	_ASSERT( !ret );
	set1->dirm = set1->dirq.MakeRotMatX();
	
	/***
	DbgOut( "e3dhsp : TheOtherXZ : deg %f : dirq : %f %f %f %f\n", set1->rotate.y, set1->dirq.w, set1->dirq.x, set1->dirq.y, set1->dirq.z );
	DbgOut( "e3dhsp : TheOtherXZ : dirm : \n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n",
		set1->dirm._11, set1->dirm._12, set1->dirm._13, set1->dirm._14,
		set1->dirm._21, set1->dirm._22, set1->dirm._23, set1->dirm._24,
		set1->dirm._31, set1->dirm._32, set1->dirm._33, set1->dirm._34,
		set1->dirm._41, set1->dirm._42, set1->dirm._43, set1->dirm._44 );
	***/

	return 0;
}

static int DirToTheOther(  E3DCHARACTERSET* set1, E3DCHARACTERSET set2 )
{
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 vec;
	
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = -1.0f;

	vec.x = set2.position.x - set1->position.x;
	vec.y = set2.position.y - set1->position.y;
	vec.z = set2.position.z - set1->position.z;

	D3DXVec3Normalize( &vec, &vec );

	D3DXVECTOR3 axis;

	D3DXVec3Cross( &axis, &basevec, &vec );
	D3DXVec3Normalize( &axis, &axis );

	float dot, rad;
	dot = D3DXVec3Dot( &basevec, &vec );
	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;
	rad = (float)acos( dot );

	CQuaternion2 q0, q1;
	q0.SetAxisAndRot( axis, rad );
	q1.SetAxisAndRot( axis, -rad );

	D3DXVECTOR3 testvec0, testvec1;
	q0.Rotate( &testvec0, basevec );
	q1.Rotate( &testvec1, basevec );

	float testdot0, testdot1;
	testdot0 = D3DXVec3Dot( &vec, &testvec0 );
	testdot1 = D3DXVec3Dot( &vec, &testvec1 );

	CQuaternion2 q;
	if( testdot0 >= testdot1 )
		q = q0;
	else
		q = q1;

	set1->rotate.x = 0.0f;
	set1->rotate.y = 0.0f;
	set1->rotate.z = 0.0f;

	float savetwist = set1->dirq.twist;
	set1->dirq = q;
	set1->dirq.twist = savetwist;

	set1->dirm = set1->dirq.MakeRotMatX();
	
	/***
	DbgOut( "e3dhsp : TheOtherXZ : deg %f : dirq : %f %f %f %f\n", set1->rotate.y, set1->dirq.w, set1->dirq.x, set1->dirq.y, set1->dirq.z );
	DbgOut( "e3dhsp : TheOtherXZ : dirm : \n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n",
		set1->dirm._11, set1->dirm._12, set1->dirm._13, set1->dirm._14,
		set1->dirm._21, set1->dirm._22, set1->dirm._23, set1->dirm._24,
		set1->dirm._31, set1->dirm._32, set1->dirm._33, set1->dirm._34,
		set1->dirm._41, set1->dirm._42, set1->dirm._43, set1->dirm._44 );
	***/

	return 0;
}


EXPORT BOOL WINAPI E3DCos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : degptr
	// 2 : multptr
	// 3 : retptr
	// 4 : degmult

	int curtype;
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(0);

	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCos : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	int degmult;
	degmult = hei->HspFunc_prm_getdi(1);



	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	float fdeg;
	if( degmult != 0 ){
		fdeg = (float)deg / (float)degmult;
	}else{
		fdeg = 0.0f;
	}


	float rad = fdeg * (float)DEG2PAI;
	
	*retptr = (int)( cosf( rad ) * (float)mult );

	return 0;

}

EXPORT BOOL WINAPI E3DSin( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : degptr
	// 2 : multptr
	// 3 : retptr
	// 4 : degmult

	int curtype;
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(0);


	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSin : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	int degmult;
	degmult = hei->HspFunc_prm_getdi(1);


	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	float fdeg;
	if( degmult != 0 ){
		fdeg = (float)deg / (float)degmult;
	}else{
		fdeg = 0.0f;
	}

	float rad = fdeg * (float)DEG2PAI;
	
	*retptr = (int)( sinf( rad ) * (float)mult );

	return 0;

}


EXPORT BOOL WINAPI E3DACos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : degptr
	// 2 : multptr
	// 3 : retptr
	// 4 : degmult
	int curtype;
	int dot;
	dot = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(0);

	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DACos : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


	int degmult;
	degmult = hei->HspFunc_prm_getdi(1);


	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	if( mult == 0 ){
		mult = 1;
	}

	float fdot = (float)dot / (float)mult;
	if( fdot > 1.0f )
		fdot = 1.0f;
	else if( fdot < -1.0f )
		fdot = -1.0f;

	float rad;
	rad = (float)acos( fdot );

	float degree;
	degree = rad * (float)PAI2DEG;

	if( degmult == 0 )
		degmult = 1;

	*retptr = (int)( degree * (float)degmult );

	return 0;

}



EXPORT BOOL WINAPI E3DDot( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 7 : multptr

	// 8 : retptr

	int curtype;
	int x1;
	x1 = hei->HspFunc_prm_getdi(0);

	int y1;
	y1 = hei->HspFunc_prm_getdi(0);

	int z1;
	z1 = hei->HspFunc_prm_getdi(0);

	int x2;
	x2 = hei->HspFunc_prm_getdi(0);

	int y2;
	y2 = hei->HspFunc_prm_getdi(0);

	int z2;
	z2 = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(0);


	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DDot : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ8:変数
	
	
	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	D3DXVECTOR3 vec1, vec2;
	vec1.x = (float)x1;
	vec1.y = (float)y1;
	vec1.z = (float)z1;

	vec2.x = (float)x2;
	vec2.y = (float)y2;
	vec2.z = (float)z2;

	D3DXVec3Normalize( &vec1, &vec1 );
	D3DXVec3Normalize( &vec2, &vec2 );

	float dot;
	dot = D3DXVec3Dot( &vec1, &vec2 );

	*retptr = (int)( dot * (float)mult );

	return 0;
}

EXPORT BOOL WINAPI E3DCross( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 7 : multptr

	// 8 : retxptr
	// 9 : retyptr
	// 10 : retzptr

	int curtype;
	int x1;
	x1 = hei->HspFunc_prm_getdi(0);

	int y1;
	y1 = hei->HspFunc_prm_getdi(0);

	int z1;
	z1 = hei->HspFunc_prm_getdi(0);

	
	int x2;
	x2 = hei->HspFunc_prm_getdi(0);

	int y2;
	y2 = hei->HspFunc_prm_getdi(0);

	int z2;
	z2 = hei->HspFunc_prm_getdi(0);


	int mult;
	mult = hei->HspFunc_prm_getdi(0);


	int* retxptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCross : parameter type error !!!\n" );
		return 1;
	}
	retxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ8:変数
	
	int* retyptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCross : parameter type error !!!\n" );
		return 1;
	}
	retyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ9:変数

	int* retzptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCross : parameter type error !!!\n" );
		return 1;
	}
	retzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ10:変数

	
	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	//！！！本来の、外積とは、異なる。！！！
	// vec1, vec2の両方に垂直で、大きさが*multptrなベクトルを計算する。

	D3DXVECTOR3 vec1, vec2;
	vec1.x = (float)x1;
	vec1.y = (float)y1;
	vec1.z = (float)z1;

	vec2.x = (float)x2;
	vec2.y = (float)y2;
	vec2.z = (float)z2;

	D3DXVec3Normalize( &vec1, &vec1 );
	D3DXVec3Normalize( &vec2, &vec2 );

	D3DXVECTOR3 crossvec;
	D3DXVec3Cross( &crossvec, &vec1, &vec2 );

	*retxptr = (int)( (float)mult * crossvec.x );
	*retyptr = (int)( (float)mult * crossvec.y );
	*retzptr = (int)( (float)mult * crossvec.z );

	//DbgOut( "E3DCross : mult %d : %f %f %f, %f %f %f\n",
	//	*multptr,
	//	crossvec.x, crossvec.y, crossvec.z,
	//	*retxptr, *retyptr, *retzptr );

	return 0;
}

EXPORT BOOL WINAPI E3DAtan( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : valptr
	// 2 : multptr
	// 3 : retptr

	int curtype;
	int val;
	val = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(0);

	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAtan : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	if( mult == 0 ){
		_ASSERT( 0 );
		return 1;
	}

	//E3DAtan val, mult, ret
	//	atan ( val / mult ) のdegree
	float fval = (float)val / (float)mult;
	float retrad;
	retrad = (float)atan( fval );

	*retptr = (int)( retrad * (float)PAI2DEG );

	//DbgOut( "E3DHSP : E3DAtan : val %f, retrad %f, result %f, retptr %d\n", val, retrad, retrad * (float)PAI2DEG, *retptr );

	return 0;

}

EXPORT BOOL WINAPI E3DAtan2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : yptr
	// 2 : xptr
	// 3 : retptr
	int curtype;
	int y;
	y = hei->HspFunc_prm_getdi(0);

	int x;
	x = hei->HspFunc_prm_getdi(0);

	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAtan2 : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	float retrad;
	retrad = (float)atan2( (float)y, (float)x );

	*retptr = (int)( retrad * (float)PAI2DEG );

	return 0;

}

EXPORT BOOL WINAPI E3DSqrt( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : valptr
	// 2 : multptr
	// 3 : retptr

	int curtype;
	int val;
	val = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(0);

	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSqrt : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	//E3DSqrt val, mult, ret
	//	sqrt( val ) * mult
	
	if( val <= 0 ){
		*retptr = 0;
	}else{
		*retptr = (int)( (float)sqrt( (float)val ) * (float)mult );
	}

	return 0;

}


EXPORT BOOL WINAPI E3DDrawText( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : posxptr
	// 2 : posyptr
	
	// 3 : scaleptr

	// 4 : rptr
	// 5 : gptr
	// 6 : bptr

	// 7 : strptr

	int curtype;
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

	int posy;
	posy = hei->HspFunc_prm_getdi(0);

	int scale;
	scale = hei->HspFunc_prm_getdi(0);

	int r;
	r = hei->HspFunc_prm_getdi(0);

	int g;
	g = hei->HspFunc_prm_getdi(0);

	int b;
	b = hei->HspFunc_prm_getdi(0);


	char tempname[2049];
	char* nameptr;
	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DDrawText : parameter type error !!!\n" );
		return 1;
		break;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= 2049 ){
		DbgOut( "e3dhsp : E3DDrawText : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );	
	
	if ( *hei->er ) return *hei->er;		// エラーチェック
////////
	
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pD3DApp->m_pFont ){
		_ASSERT( 0 );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	(g_pD3DApp->m_pFont)->m_fTextScale = (float)scale;
	(g_pD3DApp->m_pFont)->DrawText( (float)posx, (float)posy, D3DCOLOR_ARGB(255, r, g, b), tempname );
	
	return 0;
}

EXPORT BOOL WINAPI E3DDrawBigText( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : posxptr
	// 2 : posyptr
	
	// 3 : scaleptr

	// 4 : rptr
	// 5 : gptr
	// 6 : bptr

	// 7 : strptr

	int curtype;
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

	int posy;
	posy = hei->HspFunc_prm_getdi(0);



	int scale;
	scale = hei->HspFunc_prm_getdi(0);

	int r;
	r = hei->HspFunc_prm_getdi(0);

	int g;
	g = hei->HspFunc_prm_getdi(0);

	int b;
	b = hei->HspFunc_prm_getdi(0);

	
	char tempname[2049];
	char* nameptr;
	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DDrawBigText : parameter type error !!!\n" );
		return 1;
		break;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= 2049 ){
		DbgOut( "e3dhsp : E3DDrawBigText : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );	

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////
	
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pD3DApp->m_pFontBig ){
		_ASSERT( 0 );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	(g_pD3DApp->m_pFontBig)->m_fTextScale = (float)scale;
	(g_pD3DApp->m_pFontBig)->DrawText( (float)posx, (float)posy, D3DCOLOR_ARGB(255, r, g, b), tempname );
	
	return 0;
}

int SetResDir0()
{
	char filename[1024];
	char* endptr = 0;

	ZeroMemory( filename, 1024 );
	int ret;

	ret = GetModuleFileName( NULL, filename, 1024 );
	if( ret == 0 ){
		_ASSERT( 0 );
		DbgOut( "SetResDir : GetModuleFileName error !!!\n" );
		return 1;
	}

	int ch = '\\';
	char* lasten;
	lasten = strrchr( filename, ch );
	if( !lasten ){
		_ASSERT( 0 );
		DbgOut( "SetResDir : strrchr error !!!\n" );
		return 1;
	}

	*lasten = 0;

	char* last2en;
	char* last3en;
	last2en = strrchr( filename, ch );
	if( last2en ){
		*last2en = 0;
		last3en = strrchr( filename, ch );
		if( last3en ){
			if( (strcmp( last2en + 1, "Debug" ) == 0) ||
				(strcmp( last2en + 1, "DEBUG" ) == 0) ||
				(strcmp( last2en + 1, "Release" ) == 0) ||
				(strcmp( last2en + 1, "RELEASE" ) == 0)
				){

				endptr = last2en;
			}else{
				endptr = lasten;
			}
		}else{
			endptr = lasten;
		}
	}else{
		endptr = lasten;
	}

	*lasten = '\\';
	if( last2en )
		*last2en = '\\';
	if( last3en )
		*last3en = '\\';

	int leng;
	char resdir[1024];
	ZeroMemory( resdir, 1024 );
	leng = endptr - filename + 1;
	if( leng >= 1024 )
		leng = 1023;
	strncpy( resdir, filename, leng );

	::SetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPCTSTR)resdir );

	DbgOut( "SetResDir : %s\n", resdir );

	return 0;
}

int SetResDir( char* pathname, CHandlerSet* hsptr )
{

	if( pathname ){
		char* lastmark;
		lastmark = strrchr( pathname, '\\' );
		if( !lastmark )
			return 1;

		int dirleng;
		char dirname[_MAX_PATH];
		dirleng = (int)(lastmark - pathname + 1);
		ZeroMemory( dirname, _MAX_PATH );
		strncpy( dirname, pathname, dirleng );

		::SetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPCTSTR)dirname );
		DbgOut( "CMyD3DApplication : SetRESDIR : %s\n", dirname );
	}else{
		SetResDir0();
	}

	if( hsptr ){
		int namelen;
		namelen = (int)strlen( pathname );
		if( namelen < MAX_PATH ){
			strcpy( hsptr->m_resdir, pathname );
		}else{
			DbgOut( "e3d : SetResDir : name too long warning !!! %s \n", pathname );
		}
	}

	return 0;
}

int SetModuleDir()
{
	char filename[1024];
	ZeroMemory( filename, 1024 );
	int ret;

	ret = GetModuleFileName( NULL, filename, 1024 );
	if( ret == 0 ){
		_ASSERT( 0 );
		DbgOut( "SetModuleDir : GetModuleFileName error !!!\n" );
		return 1;
	}
	int ch = '\\';
	char* lasten;
	lasten = strrchr( filename, ch );
	if( !lasten ){
		_ASSERT( 0 );
		DbgOut( "SetModuleDir : strrchr error !!!\n" );
		return 1;
	}
	int leng;
	char moduledir[1024];
	ZeroMemory( moduledir, 1024 );
	leng = lasten - filename + 1;
	if( leng >= 1024 )
		leng = 1023;
	strncpy( moduledir, filename, leng );

	::SetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPCTSTR)moduledir );
	
	DbgOut( "SetModuleDir : %s\n", moduledir );
	return 0;
}

EXPORT BOOL WINAPI E3DSetProjection( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : nearptr
	// 2 : farptr
	// 3 : fovptr

	int nearclip;
	nearclip = hei->HspFunc_prm_getdi(0);

	int farclip;
	farclip = hei->HspFunc_prm_getdi(0);

	int fov;
	fov = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	g_proj_near = nearclip;
	g_proj_far = farclip;
	g_proj_fov = fov;

	if( g_proj_fov == 0 )
		g_proj_fov = 45; //default値
	
	if( g_proj_fov < 0 )
		g_proj_fov *= -1;

	if( g_proj_fov >= 180 )
		g_proj_fov = 179; // fov / 2 が　90以上にならないようにする。90は発散するので禁止。

	if( g_proj_near >= g_proj_far ){
		g_proj_near = 100; //default
		g_proj_far = 10000;//default
	}

	int ret;
	ret = SetFovIndex();
	_ASSERT( !ret );

	return 0;
}

EXPORT BOOL WINAPI E3DRand( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : rangeptr
	// 2 : retptr
	int curtype;
	int range;
	range = hei->HspFunc_prm_getdi(0);

	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DRand : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////
			
	int tempval;
	tempval = rand();

	int rangeval;
	rangeval = range;
	if( rangeval < 0 ){
		rangeval *= -1;
	}else if( rangeval == 0 ){
		rangeval = 1;
	}

	*retptr = tempval % rangeval;

	return 0;
}

EXPORT BOOL WINAPI E3DSeed( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : seedptr

	int seed;
	seed = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
////////


	srand( (unsigned)seed );


	return 0;
}


EXPORT BOOL WINAPI E3DEnableDbgFile( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	OpenDbgFile();	
	return 0;

}

EXPORT BOOL WINAPI E3DGetFPS( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;
	int* retptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetFPS : parameter type error !!!\n" );
		return 1;
	}
	retptr = (int*)hei->HspFunc_prm_getv();

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
	static DWORD beftime = 0;
	DWORD curtime;
	DWORD difftime;

	static LARGE_INTEGER lbeftime = {0, 0};
	LARGE_INTEGER lcurtime;
	LONGLONG lldifftime;
	
	DWORD retfps;

	if( s_timerfreq != 0 ){
		QueryPerformanceCounter( &lcurtime );

		//HighPart の　diff は、０と仮定！！！
		lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
		
		retfps = (DWORD)( 1.0 / ((float)lldifftime / (float)s_timerfreq) );

		lbeftime.LowPart = lcurtime.LowPart;
		lbeftime.HighPart = lcurtime.HighPart;
	}else{
		timeBeginPeriod( 5 );
		curtime = timeGetTime();
		timeEndPeriod( 5 );

		difftime = curtime - beftime;

		retfps = (DWORD)( 1.0 / ((float)difftime / 1000.0) );
		
		beftime = curtime;
	}

	*retptr = retfps;

	return 0;

}

EXPORT BOOL WINAPI E3DWaitbyFPS( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int srcfps;
	srcfps = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////
	DWORD srcmsec;
	if( srcfps <= 0 ){
		srcmsec = 0;
	}else{
		srcmsec = 1000 / srcfps;
	}

	DWORD curmsec = 0;

	static DWORD beftime = 0;
	DWORD curtime;
	DWORD difftime = 0;

	static LARGE_INTEGER lbeftime = {0, 0};
	LARGE_INTEGER lcurtime;
	LONGLONG lldifftime = 0;
	
	int retfps;


	if( s_timerfreq != 0 ){

		QueryPerformanceCounter( &lcurtime );

		//HighPart の　diff は、０と仮定！！！
		if( lcurtime.QuadPart > lbeftime.QuadPart )
			lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
		else
			lldifftime = lbeftime.QuadPart;

		curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);

		if( srcmsec >= curmsec ){
			timeBeginPeriod( 1 );
			::MsgWaitForMultipleObjects( 0, NULL, FALSE, srcmsec - curmsec, 0 );
			timeEndPeriod( 1 );

			/***
			QueryPerformanceCounter( &lcurtime );
			//HighPart の　diff は、０と仮定！！！
			if( lcurtime.QuadPart > lbeftime.QuadPart )
				lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
			else
				lldifftime = lbeftime.QuadPart;
			curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);
			***/
		}

//////////
		QueryPerformanceCounter( &lcurtime );

		//HighPart の　diff は、０と仮定！！！
		if( lcurtime.QuadPart > lbeftime.QuadPart )
			lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
		else
			lldifftime = lbeftime.QuadPart;

		curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);
		if( lldifftime != 0 )
			retfps = (int)( 1.0 / ((float)lldifftime / (float)s_timerfreq) );
		else
			retfps = 0;

//DbgOut( "E3DWaitbyFPS : qpc : retfps %d\n", retfps );		

		lbeftime.LowPart = lcurtime.LowPart;
		lbeftime.HighPart = lcurtime.HighPart;

	}else{
		timeBeginPeriod( 1 );
		curtime = timeGetTime();
		timeEndPeriod( 1 );

		if( curtime > beftime )
			difftime = curtime - beftime;
		else
			difftime = beftime;
		
		if( srcmsec >= difftime ){
			timeBeginPeriod( 1 );
			::MsgWaitForMultipleObjects( 0, NULL, FALSE, srcmsec - difftime, 0 );
			curtime = timeGetTime();
			timeEndPeriod( 1 );

			/***
			if( curtime > beftime )
				difftime = curtime - beftime;
			else
				difftime = beftime;
			***/
		}
//////////////
		timeBeginPeriod( 1 );
		curtime = timeGetTime();
		timeEndPeriod( 1 );
		
		if( curtime > beftime )
			difftime = curtime - beftime;
		else
			difftime = beftime;


		if( difftime != 0 )
			retfps = (int)( 1.0 / ((float)difftime / 1000.0) );
		else
			retfps = 0;

//DbgOut( "E3DWaitbyFPS : tgt : retfps %d\n", retfps );		

		beftime = curtime;
	}

	return -retfps;
}

/***
EXPORT BOOL WINAPI E3DWaitbyFPS( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int srcfps;
	srcfps = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////
	DWORD srcmsec;
	if( srcfps <= 0 ){
		srcmsec = 0;
	}else{
		srcmsec = 1000 / srcfps;
	}

	DWORD curmsec = 0;

	static DWORD beftime = 0;
	DWORD curtime;
	DWORD difftime = 0;

	static LARGE_INTEGER lbeftime = {0, 0};
	LARGE_INTEGER lcurtime;
	LONGLONG lldifftime = 0;
	
	int retfps;

	if( s_timerfreq != 0 ){

		while( curmsec <= srcmsec ){// srcmsec == 0 のときのために、curmsec == srcmsecの時も、ループする。
			QueryPerformanceCounter( &lcurtime );

			//HighPart の　diff は、０と仮定！！！
			if( lcurtime.QuadPart > lbeftime.QuadPart )
				lldifftime = lcurtime.QuadPart - lbeftime.QuadPart;
			else
				lldifftime = lbeftime.QuadPart;

			curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);

			Sleep( 0 );
		}
		
		if( lldifftime != 0 )
			retfps = (int)( 1.0 / ((float)lldifftime / (float)s_timerfreq) );
		else
			retfps = 0;

//DbgOut( "E3DWaitbyFPS : qpc : retfps %d\n", retfps );		

		lbeftime.LowPart = lcurtime.LowPart;
		lbeftime.HighPart = lcurtime.HighPart;
	}else{

		timeBeginPeriod( 5 );
		while( curmsec <= srcmsec ){// srcmsec == 0 のときのために、curmsec == srcmsecの時も、ループする。
			curtime = timeGetTime();
			if( curtime > beftime )
				difftime = curtime - beftime;
			else
				difftime = beftime;

			curmsec = difftime;

			Sleep( 0 );
		}
		timeEndPeriod( 5 );

		if( difftime != 0 )
			retfps = (int)( 1.0 / ((float)difftime / 1000.0) );
		else
			retfps = 0;

//DbgOut( "E3DWaitbyFPS : tgt : retfps %d\n", retfps );		

		beftime = curtime;
	}
	
	return -retfps;
}
***/


EXPORT BOOL WINAPI E3DCreateLight( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int curtype;
	int* retptr;

	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCreateLight : parameter type error !!!\n" );
		return 1;
	}

	retptr = (int*)hei->HspFunc_prm_getv();
//////
	
	int lid = -1;
	CLightData* newlight = 0;

	newlight = new CLightData( &lid );
	if( !newlight ){
		DbgOut( "E3DCreateLight : newlight alloc error !!!\n" );
		return 1;
	}
	
	newlight->nextlight = g_lighthead;
	g_lighthead = newlight;

	*retptr = lid;

	return 0;
}

CLightData* GetLightData( int lid )
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

CMySprite* GetSprite( int spriteid )
{
	CMySprite* retsprite = 0;
	CMySprite* cursprite = s_spritehead;

	while( cursprite ){
		if( cursprite->serialno == spriteid ){
			retsprite = cursprite;
			break;
		}
		cursprite = cursprite->next;
	}

	return retsprite;
}



EXPORT BOOL WINAPI E3DSetDirectionalLight( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : light id

	// 2 : dir.x
	// 3 : dir.y
	// 4 : dir.z
	
	// 5 : r
	// 6 : g
	// 7 : b


	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int dirx;
	dirx = hei->HspFunc_prm_getdi(0);

	int diry;
	diry = hei->HspFunc_prm_getdi(0);

	int dirz;
	dirz = hei->HspFunc_prm_getdi(0);

	int r;
	r = hei->HspFunc_prm_getdi(0);

	int g;
	g = hei->HspFunc_prm_getdi(0);

	int b;
	b = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////
	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetDirectionalLight : lightID not found error !!!\n" );
		return 1;
	}

	D3DXVECTOR3 dir;
	dir.x = (float)dirx;
	dir.y = (float)diry;
	dir.z = (float)dirz;
	
	D3DXVec3Normalize( &dir, &dir );

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f difcol;
	difcol.x = (float)r / 255.0f;
	difcol.y = (float)g / 255.0f;
	difcol.z = (float)b / 255.0f;

	int ret;
	ret = curlight->SetDirectionalLight( dir, difcol );
	if( ret ){
		_ASSERT( !ret );
		return 1;
	}
////////

	//resotreでも実行する

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetDirectionalLight : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );
    light.Type = D3DLIGHT_DIRECTIONAL;

    light.Direction = dir;
	
	light.Diffuse.r = difcol.x;
	light.Diffuse.g = difcol.y;
	light.Diffuse.b = difcol.z;
	light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );


	//DbgOut( "e3dhsp : SetDirectionalLight : lid %d, col %f %f %f, dir %f %f %f\n",
	//	lid, difcol.x, difcol.y, difcol.z, dir.x, dir.y, dir.z );

	return 0;
}


EXPORT BOOL WINAPI E3DSetPointLight( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int posx;
	posx = hei->HspFunc_prm_getdi(0);

	int posy;
	posy = hei->HspFunc_prm_getdi(0);

	int posz;
	posz = hei->HspFunc_prm_getdi(0);

	int dist;
	dist = hei->HspFunc_prm_getdi(0);

	int r;
	r = hei->HspFunc_prm_getdi(0);

	int g;
	g = hei->HspFunc_prm_getdi(0);

	int b;
	b = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	CLightData* curlight;
	curlight = GetLightData( lid );
	if( !curlight ){
		DbgOut( "E3DSetPointLight : lightID not found error !!!\n" );
		return 1;
	}

	D3DXVECTOR3 pos;
	pos.x = (float)posx;
	pos.y = (float)posy;
	pos.z = (float)posz;
	

	float fdist;
	fdist = (float)dist;

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f difcol;
	difcol.x = (float)r / 255.0f;
	difcol.y = (float)g / 255.0f;
	difcol.z = (float)b / 255.0f;

	int ret;
	ret = curlight->SetPointLight( pos, fdist, difcol );
	if( ret ){
		_ASSERT( 0 );
		DbgOut( "E3DSetPointLight : SetPointLight error !!!\n" );
		return 1;
	}

////////

	//resotreでも実行する

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetDirectionalLight : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );
    light.Type = D3DLIGHT_POINT;
	
	light.Diffuse.r = difcol.x;
	light.Diffuse.g = difcol.y;
	light.Diffuse.b = difcol.z;
	light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

	light.Position = pos;

	light.Range = fdist;

	light.Attenuation0 = 0.4f;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );


//DbgOut( "e3dhsp : SetPointLight : pointlight : col %f %f %f, pos %f %f %f, range %f\n",
//	   light.Diffuse.r, light.Diffuse.g, light.Diffuse.b,
//	   light.Position.x, light.Position.y, light.Position.z,
//	   light.Range );


	return 0;
}

EXPORT BOOL WINAPI E3DSetSpotLight( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int posx;
	posx = hei->HspFunc_prm_getdi(0);

	int posy;
	posy = hei->HspFunc_prm_getdi(0);

	int posz;
	posz = hei->HspFunc_prm_getdi(0);

	int dirx;
	dirx = hei->HspFunc_prm_getdi(0);

	int diry;
	diry = hei->HspFunc_prm_getdi(0);

	int dirz;
	dirz = hei->HspFunc_prm_getdi(0);


	int dist;
	dist = hei->HspFunc_prm_getdi(0);

	int angle;
	angle = hei->HspFunc_prm_getdi(0);

	int r;
	r = hei->HspFunc_prm_getdi(0);

	int g;
	g = hei->HspFunc_prm_getdi(0);

	int b;
	b = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	CLightData* curlight;
	curlight = GetLightData( lid );
	if( !curlight ){
		DbgOut( "E3DSetPointLight : lightID not found error !!!\n" );
		return 1;
	}

	D3DXVECTOR3 pos;
	pos.x = (float)posx;
	pos.y = (float)posy;
	pos.z = (float)posz;
	
	D3DXVECTOR3 dir;
	dir.x = (float)dirx;
	dir.y = (float)diry;
	dir.z = (float)dirz;

	D3DXVec3Normalize( &dir, &dir );


	float fdist;
	fdist = (float)dist;

	float fangle;
	fangle = (float)angle;

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f difcol;
	difcol.x = (float)r / 255.0f;
	difcol.y = (float)g / 255.0f;
	difcol.z = (float)b / 255.0f;

	int ret;
	ret = curlight->SetSpotLight( pos, dir, fdist, fangle, difcol );
	if( ret ){
		DbgOut( "E3DSetSpotLight : SetSpotLight error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////


	//resotreでも実行する

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetDirectionalLight : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;                 // Description of the D3D light
    ZeroMemory( &light, sizeof(light) );
    light.Type = D3DLIGHT_SPOT;
	
	light.Diffuse.r = difcol.x;
	light.Diffuse.g = difcol.y;
	light.Diffuse.b = difcol.z;
	light.Diffuse.a = 1.0f;

			light.Specular.r = curlight->Specular.x;
			light.Specular.g = curlight->Specular.y;
			light.Specular.b = curlight->Specular.z;
			light.Specular.a = 1.0f;

			light.Ambient.r = curlight->Ambient.x;
			light.Ambient.g = curlight->Ambient.y;
			light.Ambient.b = curlight->Ambient.z;
			light.Ambient.a = 1.0f;

	light.Position = pos;

	light.Direction = dir;

	light.Range = fdist;

	light.Falloff = 1.0f;

    light.Attenuation0 = 1.0f;
    
	light.Theta        = fangle * (float)DEG2PAI;
    light.Phi          = fangle * (float)DEG2PAI;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );


	return 0;
}


EXPORT BOOL WINAPI E3DDestroyLight( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
///////
	
	CLightData* beflight = 0;
	CLightData* curlight = g_lighthead;
	CLightData* findlight = 0;

	while( curlight ){
		if( curlight->lightid == lid ){
			findlight = curlight;
			break;
		}
		beflight = curlight;
		curlight = curlight->nextlight;
	}

	if( findlight ){
		if( beflight ){
			beflight->nextlight = findlight->nextlight;
			delete findlight;
		}else{
			g_lighthead = findlight->nextlight;
			delete findlight;
		}

		int ret;
		LPDIRECT3DDEVICE9 pdev;
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		if( ret || !pdev ){
			DbgOut( "e3dhsp : E3DDestroyLight : GetD3DDevice error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pdev->LightEnable( lid, FALSE );
	
	}else{
		return 0;
	}

	return 0;
}



EXPORT BOOL WINAPI E3DClearZ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}
	
    // Clear Z buffer
	HRESULT hres;
	hres = pdev->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 
                         0x000000ff,
						 //0x00ffffff,
						 1.0f, 0L );
   if( hres != D3D_OK ){
		//_ASSERT( 0 );
	   DbgOut( "E3DClearZ : Clear error !!!\n" );
		return 1;
   }

	return 0;
}

EXPORT BOOL WINAPI E3DDestroyHandlerSet( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////

	int ret;
	ret = DestroyHandlerSet( hsid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyHandlerSet : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	CHandlerSet* befhs = 0;
	CHandlerSet* curhs = hshead;
	CHandlerSet* findhs = 0;

	while( curhs && (findhs == 0) ){
		if( curhs->serialno == hsid ){
			findhs = curhs;
			break;
		}
		befhs = curhs;
		curhs = curhs->next;
	}

	if( findhs ){
		if( befhs ){
			befhs->next = findhs->next;
			delete findhs;
		}else{
			hshead = findhs->next;
			delete findhs;
		}
	}else{
		return 0;
	}
	***/

	return 0;
}

EXPORT BOOL WINAPI E3DCreateBG( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int curtype;
	char tempname1[MAX_PATH];
	char* nameptr1 = 0;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DCreateBG : parameter type error !!!\n" );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DCreateBG : parameter error !!!\n" );
		return 1;
	}

	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "E3DCreateBG : namelen1 too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );



	char tempname2[MAX_PATH];
	char* nameptr2 = 0;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr2 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
		break;
	case 4:
		nameptr2 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DCreateBG : parameter type error !!!\n" );
		return 1;
		break;
	}

	if( !nameptr2 ){
		DbgOut( "E3DCreateBG : parameter error !!!\n" );
		return 1;
	}

	int namelen2;
	namelen2 = (int)strlen( nameptr2 );
	if( namelen2 >= MAX_PATH ){
		DbgOut( "E3DCreateBG : namelen1 too large error !!!\n" );
		return 1;
	}
	strcpy( tempname2, nameptr2 );

	int uanim;
	uanim = hei->HspFunc_prm_getdi(0);

	int vanim;
	vanim = hei->HspFunc_prm_getdi(0);


	int isround;
	isround = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
///////////

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( s_bgdisp != 0 ){
		delete s_bgdisp;
		s_bgdisp = 0;
	}

	float fuanim, fvanim;
	fuanim = (float)uanim / 1000.0f;
	if( fuanim > 1.0f ){
		fuanim = 1.0f;
	}else if( fuanim < -1.0f ){
		fuanim = -1.0f;
	}

	fvanim = (float)vanim / 1000.0f;
	if( fvanim > 1.0f ){
		fvanim = 1.0f;
	}else if( fvanim < -1.0f ){
		fvanim = -1.0f;
	}


	CBgDisp2* newbg;
	newbg = new CBgDisp2( pdev, tempname1, tempname2, fuanim, fvanim, 0xFFFFFFFF, isround );
	if( !newbg ){
		DbgOut( "E3DSetBG : newbg alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_bgdisp = newbg;

/////////

	_ASSERT( s_hwnd );

	RECT clirect;
	GetClientRect( s_hwnd, &clirect );
	s_clientWidth = clirect.right - clirect.left;
	s_clientHeight = clirect.bottom - clirect.top;



	return 0;
}

//E3DSetBGU startu10000, endu10000
EXPORT BOOL WINAPI E3DSetBGU( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

//1
	int startu10000;
	startu10000 = hei->HspFunc_prm_getdi(0);

//2
	int endu10000;
	endu10000 = hei->HspFunc_prm_getdi(0);

///
	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////

	if( !s_bgdisp )
		return 0;

	float startu;
	float endu;

	startu = (float)startu10000 * 0.0001f; 
	endu = (float)endu10000 * 0.0001f;


	int ret;
	ret = s_bgdisp->SetU( startu, endu );
	if( ret ){
		DbgOut( "E3DSetBGU : bgdisp SetU error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSetBGV( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

//1
	int startv10000;
	startv10000 = hei->HspFunc_prm_getdi(0);

//2
	int endv10000;
	endv10000 = hei->HspFunc_prm_getdi(0);

///
	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////

	if( !s_bgdisp )
		return 0;

	float startv;
	float endv;

	startv = (float)startv10000 * 0.0001f; 
	endv = (float)endv10000 * 0.0001f;


	int ret;
	ret = s_bgdisp->SetV( startv, endv );
	if( ret ){
		DbgOut( "E3DSetBGV : bgdisp SetV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DDestroyBG( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	if( s_bgdisp != 0 ){
		delete s_bgdisp;
		s_bgdisp = 0;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSetDispSwitch( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int mk;
	mk = hei->HspFunc_prm_getdi(0);

	int swid;
	swid = hei->HspFunc_prm_getdi(0);

	int srcframeno;
	srcframeno = hei->HspFunc_prm_getdi(0);

	int swflag;
	swflag = hei->HspFunc_prm_getdi(0);
	
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDispSwitch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SetDispSwitch( mk, swid, srcframeno, swflag );
	if( ret ){
		DbgOut( "E3DSetDispSwitch : SetDispSwitch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EXPORT BOOL WINAPI E3DSetMotionFrameNo( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int mk;
	mk = hei->HspFunc_prm_getdi(0);

	int srcframeno;
	srcframeno = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMotionFrameNo( mk, srcframeno );
	if( ret ){
		DbgOut( "E3DSetMotionFrameNo : SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DCreateSprite( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;
//1 : name
	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DCreateSprite : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DCreateSprite : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : CreateSprite : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//2 : transparent
	int transparentflag;
	transparentflag = hei->HspFunc_prm_getdi(0);

//3 : spriteid
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCreateSprite : parameter type error !!!\n" );
		return 1;
	}
	int* spriteidptr;
	spriteidptr = (int *)hei->HspFunc_prm_getv();

//4 : tpr
	int tpr = hei->HspFunc_prm_getdi(0);
//4 : tpg
	int tpg = hei->HspFunc_prm_getdi(0);
//4 : tpb
	int tpb = hei->HspFunc_prm_getdi(0);



	if ( *hei->er ) return *hei->er;		// エラーチェック
////////
	CMySprite* newsprite;
	newsprite = new CMySprite( spriteidptr );
	if( !newsprite ){
		DbgOut( "E3DCreateSprite : newsprite alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newsprite->next = s_spritehead;
	s_spritehead = newsprite;

	int ret;
	ret = newsprite->SetSpriteParams( tempname, transparentflag, tpr, tpg, tpb, g_miplevels, g_mipfilter );
	if( ret ){
		DbgOut( "E3DCreateSprite : newsprite SetSpriteParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newsprite->RestoreTexture( pdev );
	if( ret ){
		DbgOut( "E3DCreateSprite : newsprite RestoreTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	return 0;
}

EXPORT BOOL WINAPI E3DBeginSprite( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	if( !s_d3dxsprite ){
		DbgOut( "E3DBeginSprite : d3dxsprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	HRESULT hr;
	hr = s_d3dxsprite->Begin();
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EXPORT BOOL WINAPI E3DEndSprite( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	if( !s_d3dxsprite ){
		DbgOut( "E3DEndSprite : d3dxsprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	hr = s_d3dxsprite->End();
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EXPORT BOOL WINAPI E3DRenderSprite( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int spid;
	spid = hei->HspFunc_prm_getdi(0);

	int scalex;
	scalex = hei->HspFunc_prm_getdi(0);

	int scaley;
	scaley = hei->HspFunc_prm_getdi(0);

	int trax;
	trax = hei->HspFunc_prm_getdi(0);

	int tray;
	tray = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	if( !s_d3dxsprite ){
		DbgOut( "E3DRenderSprite : d3dxsprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}



	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DRenderSprite : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR2 scale;
	scale.x = (float)scalex / 100.0f; //!!!!!!!
	scale.y = (float)scaley / 100.0f; //!!!!!!!

	D3DXVECTOR2 tra;
	tra.x = (float)trax;
	tra.y = (float)tray;

	ret = cursprite->Render( pdev, s_d3dxsprite, scale, tra );
	if( ret ){
		DbgOut( "E3DRenderSprite : Render error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EXPORT BOOL WINAPI E3DGetSpriteSize( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int curtype;

	int spid;
	spid = hei->HspFunc_prm_getdi(0);

	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSpriteSizez : parameter type error !!!\n" );
		return 1;
	}
	int* sizexptr;
	sizexptr = (int *)hei->HspFunc_prm_getv();

	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSpriteSizez : parameter type error !!!\n" );
		return 1;
	}
	int* sizeyptr;
	sizeyptr = (int *)hei->HspFunc_prm_getv();

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DGetSpriteSize : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*sizexptr = cursprite->texWidth;
	*sizeyptr = cursprite->texHeight;


	return 0;
}

/***
	E3DSetSpriteRotation spriteid, centerx, centery, rotdeg, mult

	E3DSetSpriteARGB spriteid, alpha, r, g, b
***/
EXPORT BOOL WINAPI E3DSetSpriteRotation( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int spid;
	spid = hei->HspFunc_prm_getdi(0);

	int centerx;
	centerx = hei->HspFunc_prm_getdi(0);

	int centery;
	centery = hei->HspFunc_prm_getdi(0);

	int rotdeg;
	rotdeg = hei->HspFunc_prm_getdi(0);

	int mult;
	mult = hei->HspFunc_prm_getdi(1);
	
	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteRotation : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR2 cenvec;
	cenvec.x = (float)centerx;
	cenvec.y = (float)centery;

	float fdeg;
	if( mult != 0 ){
		fdeg = (float)rotdeg / (float)mult;
	}else{
		fdeg = 0.0f;
	}

	ret = cursprite->SetRotation( cenvec, fdeg );
	if( ret ){
		DbgOut( "E3DSetSpriteRotation : sprite SetRotation error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSetSpriteARGB( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int spid;
	spid = hei->HspFunc_prm_getdi(0);

	int alpha;
	alpha = hei->HspFunc_prm_getdi(0);

	int r;
	r = hei->HspFunc_prm_getdi(0);

	int g;
	g = hei->HspFunc_prm_getdi(0);

	int b;
	b = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteARGB : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( alpha < 0 ){
		alpha = 0;
	}else if( alpha > 255 ){
		alpha = 255;
	}

	if( r < 0 ){
		r = 0;
	}else if( r > 255 ){
		r = 255;
	}

	if( g < 0 ){
		g = 0;
	}else if( g > 255 ){
		g = 255;
	}

	if( b < 0 ){
		b = 0;
	}else if( b > 255 ){
		b = 255;
	}

	int ret;

	ret = cursprite->SetARGB( alpha, r, g, b );
	if( ret ){
		DbgOut( "E3DSetSpriteARGB : sprite SetARGB error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



EXPORT BOOL WINAPI E3DDestroySprite( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int spid;
	spid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////

	int ret;
	ret = DestroyTexture( -2, -2, spid );
	_ASSERT( !ret );
///////


	CMySprite* befsprite = 0;
	CMySprite* cursprite = s_spritehead;
	CMySprite* findsprite = 0;

	while( cursprite ){
		if( cursprite->serialno == spid ){
			findsprite = cursprite;
			break;
		}
		befsprite = cursprite;
		cursprite = cursprite->next;
	}

	if( findsprite ){
		if( befsprite ){
			befsprite->next = findsprite->next;
			delete findsprite;
		}else{
			s_spritehead = findsprite->next;
			delete findsprite;
		}
	}else{
		return 0;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DChkInView( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkInView : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	float fDeg2Pai;
	fDeg2Pai = (float)DEG2PAI;

	D3DXVECTOR3 viewvec( 0.0f, 0.0f, 0.0f );
	
	if( s_cameratype == 0 ){
		D3DXVECTOR3 targetpos( 0.0f, 0.0f, 0.0f );
		targetpos.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
		targetpos.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
		targetpos.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

		viewvec.x = targetpos.x - s_camerapos.x;
		viewvec.y = targetpos.y - s_camerapos.y;
		viewvec.z = targetpos.z - s_camerapos.z;

		D3DXVec3Normalize( &viewvec, &viewvec );
	}else{
		viewvec.x = s_cameratarget.x - s_camerapos.x;
		viewvec.y = s_cameratarget.y - s_camerapos.y;
		viewvec.z = s_cameratarget.z - s_camerapos.z;
	
		D3DXVec3Normalize( &viewvec, &viewvec );
	}
	***/

	/***
	float fDeg2Pai = (float)DEG2PAI;
	
	if( s_cameratype == 0 ){
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

		s_cameratarget.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
		s_cameratarget.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
		s_cameratarget.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

		D3DXMatrixLookAtLH( &s_matView, &s_camerapos, &s_cameratarget, &vUpVec );
	}else{

		D3DXMatrixLookAtLH( &s_matView, &s_camerapos, &s_cameratarget, &s_cameraupvec );
	}
	***/
	CalcMatView();

	int ret;
	//ret = curhs->ChkInView( s_camerapos, viewvec, (float)g_proj_near, (float)g_proj_far, (float)g_proj_fov * fDeg2Pai );
	ret = curhs->ChkInView( s_matView );
	if( ret ){
		DbgOut( "E3DRender : ChkInView error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DChkConfBillboard2 centerx, centery, centerz, srcr, confrate, result
EXPORT BOOL WINAPI E3DChkConfBillboard2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;	
// 1 : centerx
	int centerx;
	centerx = hei->HspFunc_prm_getdi(0);

// 2 : centery
	int centery;
	centery = hei->HspFunc_prm_getdi(0);

// 3 : centerz
	int centerz;
	centerz = hei->HspFunc_prm_getdi(0);

// 4 : srcr
	int srcr;
	srcr = hei->HspFunc_prm_getdi(0);

// 5 : confrate100
	int confrate100;
	confrate100 = hei->HspFunc_prm_getdi(0);

// 6 : confflagptr
	int* confflagptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBillboard2 : parameter type error !!!\n" );
		return 1;
	}
	confflagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


// 7 : confbbid
	int* confbbid;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBillboard : parameter type error !!!\n" );
		return 1;
	}
	confbbid = (int *)hei->HspFunc_prm_getv();

// 8 : arrayleng
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);

// 9 : confnumptr
	int* confnumptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBillboard : parameter type error !!!\n" );
		return 1;
	}
	confnumptr = (int *)hei->HspFunc_prm_getv();


//



/////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////////

	if( g_bbhs ){
		int ret;
		ret = g_bbhs->ChkConfBillboard2( centerx, centery, centerz, srcr, confflagptr, confrate100, confbbid, arrayleng, confnumptr );
		if( ret ){
			DbgOut( "E3DChkConfBillboard2 : bbhs ChkConfBillboard2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	
	}else{
		DbgOut( "E3DChkConfBillboard2 : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		*confflagptr = 0;
		return 1;
	}

	return 0;
}


//E3DChkConfBillboard hsid, confrate, result, confbbid, arrayleng, confnum
EXPORT BOOL WINAPI E3DChkConfBillboard( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;	
// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

// 2 : confrate
	int confrate;
	confrate = hei->HspFunc_prm_getdi(0);

// 3 : confflagptr
	int* confflagptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBillboard : parameter type error !!!\n" );
		return 1;
	}
	confflagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 4 : confbbid
	int* confbbid;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBillboard : parameter type error !!!\n" );
		return 1;
	}
	confbbid = (int *)hei->HspFunc_prm_getv();

// 5 : arrayleng
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);

// 6 : confnumptr
	int* confnumptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBillboard : parameter type error !!!\n" );
		return 1;
	}
	confnumptr = (int *)hei->HspFunc_prm_getv();

/////////////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	*confnumptr = 0;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfBillboard : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	if( g_bbhs ){
		int ret;
		ret = g_bbhs->ChkConfBillboard( curhs, confflagptr, confrate, confbbid, arrayleng, confnumptr );
		if( ret ){
			DbgOut( "E3DChkConfBySphere : bbhs ChkConfBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	
	}else{
		DbgOut( "E3DChkConfBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		*confflagptr = 0;
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DChkConfBySphere( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : src handlerset id
	// 2 : chk handlerset id
	// (3,4 : rate 後で、拡張するかも？)
	int curtype;	
	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	int chkhsid;
	chkhsid = hei->HspFunc_prm_getdi(0);

	int conflevel;
	conflevel = hei->HspFunc_prm_getdi(0);

	int* confflagptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBySphere : parameter type error !!!\n" );
		return 1;
	}
	confflagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConfBySphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConfBySphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int ret;
	ret = srchs->ChkConfBySphere( chkhs, conflevel, confflagptr );
	if( ret ){
		DbgOut( "E3DChkConfBySphere : srchs ChkConfBySphere error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DChkConfBySphere2 hsid1, partno1, hsid2, partno2, confflag
EXPORT BOOL WINAPI E3DChkConfBySphere2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : src handlerset id
	// 2 : srcpartno
	// 3 : chk handlerset id
	// 4 : chkpartno
	// 5 : confflag

	int curtype;	
	
	int srchsid;
	srchsid = hei->HspFunc_prm_getdi(0);

	int srcpartno;
	srcpartno = hei->HspFunc_prm_getdi(0);

	int chkhsid;
	chkhsid = hei->HspFunc_prm_getdi(0);

	int chkpartno;
	chkpartno = hei->HspFunc_prm_getdi(0);

	int* confflagptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfBySphere2 : parameter type error !!!\n" );
		return 1;
	}
	confflagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConfBySphere2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConfBySphere2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int ret;
	ret = srchs->ChkConfBySphere2( srcpartno, chkhs, chkpartno, confflagptr );
	if( ret ){
		DbgOut( "E3DChkConfBySphere : srchs ChkConfBySphere2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



EXPORT BOOL WINAPI E3DCreateProgressBar( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
    RECT rcClient;  // Client area of parent window 
    int cyVScroll;  // Height of scroll bar arrow 
 
 
	if( !s_hwnd || !s_inst ){
		DbgOut( "E3DCreateProgressBar : parent wnd NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s_PBwnd ){
		DbgOut( "E3DCreateProgressBar : progress bar already exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

    // Ensure that the common control DLL is loaded
    // and create a progress bar along the bottom of
    // the client area of the parent window. 
    // Base the height of the progress bar on the
    // height of a scroll bar arrow. 
    GetClientRect(s_hwnd, &rcClient); 
    cyVScroll = GetSystemMetrics(SM_CYVSCROLL); 
    s_PBwnd = CreateWindowEx(0, PROGRESS_CLASS,
	         (LPSTR)NULL, WS_CHILD | WS_VISIBLE,
             rcClient.left, rcClient.bottom - cyVScroll,
             rcClient.right - rcClient.left, cyVScroll, 
             s_hwnd, (HMENU) 0, s_inst, NULL); 

	if( !s_PBwnd ){
		_ASSERT( 0 );
		return 1;
	}

    //SendMessage(hwndPB, PBM_SETRANGE, 0,
    //            MAKELPARAM(0, cb / 2048)); 
    //SendMessage(hwndPB, PBM_SETSTEP, (WPARAM) 1, 0); 


    


	return 0;
}

EXPORT BOOL WINAPI E3DSetProgressBar( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int newpos;
	newpos = hei->HspFunc_prm_getdi(0);
	if( newpos < 0 )
		newpos = 0;
	if( newpos > 100 )
		newpos = 100;

	if ( *hei->er ) return *hei->er;		// エラーチェック
//////////


	if( !s_PBwnd ){
		DbgOut( "E3DSetProgressBar : Progress bar still not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SendMessage( s_PBwnd, PBM_SETPOS, newpos, 0 );


	return 0;
}

EXPORT BOOL WINAPI E3DDestroyProgressBar( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	if( s_PBwnd ){
		DestroyWindow( s_PBwnd );
		s_PBwnd = 0;
	}

	return 0;
}


//E3DLoadGroundBMP( char* bmpname1, char* bmpname2, char* bmpname3, char* texturename, int rgbflag, int maxx, int maxz, int divx, int divz, float maxheight, int* retid );

EXPORT BOOL WINAPI E3DLoadGroundBMP( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : bmp file name1
	// 2 : bmp file name2
	// 3 : bmp file name3
	// 4 : texture file name
	// 5 : maxx
	// 6 : maxz
	// 7 : divx
	// 8 : divz
	// 9 : maxheight
	// 10 : pointer of handlerset id.

	// 11 : TLmode , ver1036

//1
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2
	char tempname2[MAX_PATH];
	char* nameptr2;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr2 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr2 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr2 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen2;
	namelen2 = (int)strlen( nameptr2 );
	if( namelen2 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname2, nameptr2 );
//3
	char tempname3[MAX_PATH];
	char* nameptr3;


	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr3 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr3 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr3 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen3;
	namelen3 = (int)strlen( nameptr3 );
	if( namelen3 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname3, nameptr3 );
	
	
//4
	char tempname4[MAX_PATH];
	char* nameptr4;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr4 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr4 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr4 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen4;
	namelen4 = (int)strlen( nameptr4 );
	if( namelen4 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname4, nameptr4 );
//5
	int maxx;
	maxx = hei->HspFunc_prm_getdi(0);
//6
	int maxz;
	maxz = hei->HspFunc_prm_getdi(0);

//7	
	int divx;
	divx = hei->HspFunc_prm_getdi(0);
	if( divx < 2 )
		divx = 2;
//8
	int divz;
	divz = hei->HspFunc_prm_getdi(0);
	if( divz < 2 )
		divz = 2;
//9
	int maxheight;
	maxheight = hei->HspFunc_prm_getdi(0);
	if( maxheight < 0 )
		maxheight = 0;
//10	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "e3dhsp : LoadGroundBMP : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGroundBMP : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//11
	int TLmode;
	TLmode = hei->HspFunc_prm_getdi(2);
	
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	int ret;
	int temphsid = -1;
	ret = LoadGroundBMP_F( tempname1, tempname2, tempname3, tempname4, maxx, maxz, divx, divz, maxheight, TLmode, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : LoadGroundBMP_F error !!!\n" );
		_ASSERT( 0 );
		*hsidptr = -1;
		return 1;
	}
	*hsidptr = temphsid;
	
	
	return 0;
}

int LoadGroundBMP_F( char* tempname1, char* tempname2, char* tempname3, char* tempname4, int maxx, int maxz, int divx, int divz, int maxheight, int TLmode, int* hsidptr )
{
	EnterCriticalSection( &g_crit_hshead );//######## start crit

	int ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	SetResDir( tempname1, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	newhs->m_TLmode = TLmode;//!!!!!!! ver1036
	newhs->m_groundflag = 1;//!!!!!!!! ver1036

	ret = newhs->LoadGroundBMP( tempname1, tempname2, tempname3, tempname4, (float)maxx, (float)maxz, divx, divz, (float)maxheight );
	if( ret ){
		DbgOut( "easy3d : E3DCreateHandler : LoadGroundBMP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;

	return 0;
}


EXPORT BOOL WINAPI E3DCameraPosForward( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int step;
	step = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////
	
	
	float fDeg2Pai = (float)DEG2PAI;
	
	if( s_cameratype == 0 ){
		D3DXVECTOR3 dir( 0.0f, 0.0f, 0.0f );

		dir.x = sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
		dir.y = sinf( s_camera_degy * fDeg2Pai );
		dir.z = -cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

		s_camerapos.x += step * dir.x;
		s_camerapos.y += step * dir.y;
		s_camerapos.z += step * dir.z;

	}else{
		D3DXVECTOR3 dir( 0.0f, 0.0f, 0.0f );
		dir.x = s_cameratarget.x - s_camerapos.x;
		dir.y = s_cameratarget.y - s_camerapos.y;
		dir.z = s_cameratarget.z - s_camerapos.z;

		D3DXVec3Normalize( &dir, &dir );

		s_camerapos.x += step * dir.x;
		s_camerapos.y += step * dir.y;
		s_camerapos.z += step * dir.z;

		s_cameratarget.x += step * dir.x;
		s_cameratarget.y += step * dir.y;
		s_cameratarget.z += step * dir.z;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSetBeforePos( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBeforePos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SetBeforePos();
	if( ret ){
		DbgOut( "E3DSetBeforePos : curhs SetBeforePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DChkConfGround charahsid, groundhsid, mode, diffmaxy, result, adjustx, adjusty, adjustz, nx, ny, nz
EXPORT BOOL WINAPI E3DChkConfGround( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : charahsid
	// 2 : groundhsid
	// 3 : mode
	// 4 : diffmaxy
	// 5 : mapminy
	// 6 : result
	// 7 : adjustx
	// 8 : adjusty
	// 9 : adjustz
	// 10 : nx10000
	// 11 : ny10000
	// 12 : nz10000

	int curtype;

// 1 : charahsid
	int charahsid;
	charahsid = hei->HspFunc_prm_getdi(0);

// 2 : groundhsid
	int groundhsid;
	groundhsid = hei->HspFunc_prm_getdi(0);

// 3 : mode
	int mode;
	mode = hei->HspFunc_prm_getdi(0);

// 4 : diffmaxy
	int diffmaxy;
	diffmaxy = hei->HspFunc_prm_getdi(0);

// 5 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

// 6 : result
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround : parameter type error !!!\n" );
		return 1;
	}
	int* resultptr;
	resultptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 7 : adjustx
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround : parameter type error !!!\n" );
		return 1;
	}
	int* adxptr;
	adxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 8 : adjusty
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround : parameter type error !!!\n" );
		return 1;
	}
	int* adyptr;
	adyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 9 : adjustz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround : parameter type error !!!\n" );
		return 1;
	}
	int* adzptr;
	adzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 10 : nx10000
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround : parameter type error !!!\n" );
		return 1;
	}
	int* nx10000ptr;
	nx10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 11 : ny
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround : parameter type error !!!\n" );
		return 1;
	}
	int* ny10000ptr;
	ny10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 12 : nz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround : parameter type error !!!\n" );
		return 1;
	}
	int* nz10000ptr;
	nz10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->ChkConfGround( charahs, mode, diffmaxy, mapminy, resultptr, adxptr, adyptr, adzptr, nx10000ptr, ny10000ptr, nz10000ptr );
	if( ret ){
		DbgOut( "E3DChkConfGround : groundhs ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DChkConfGround2 befposx, befposy, befposz, newposx, newposy, newposz, groundhsid, mode, diffmaxy, result, adjustx, adjusty, adjustz, nx, ny, nz
EXPORT BOOL WINAPI E3DChkConfGround2( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : befposx
	// 2 : befposy
	// 3 : befposz

	// 4 : newposx
	// 5 : newposy
	// 6 : newposz

	// 7 : groundhsid
	// 8 : mode
	// 9 : diffmaxy
	// 10 : mapminy
	// 11 : result
	// 12 : adjustx
	// 13 : adjusty
	// 14 : adjustz
	// 15 : nx10000
	// 16 : ny10000
	// 17 : nz10000

	int curtype;

// 1 : befposx
	int befposx;
	befposx = hei->HspFunc_prm_getdi(0);

// 2 : befposy
	int befposy;
	befposy = hei->HspFunc_prm_getdi(0);

// 3 : befposz
	int befposz;
	befposz = hei->HspFunc_prm_getdi(0);

// 4 : newposx
	int newposx;
	newposx = hei->HspFunc_prm_getdi(0);

// 5 : newposy
	int newposy;
	newposy = hei->HspFunc_prm_getdi(0);

// 6 : newposz
	int newposz;
	newposz = hei->HspFunc_prm_getdi(0);

// 7 : groundhsid
	int groundhsid;
	groundhsid = hei->HspFunc_prm_getdi(0);

// 8 : mode
	int mode;
	mode = hei->HspFunc_prm_getdi(0);

// 9 : diffmaxy
	int diffmaxy;
	diffmaxy = hei->HspFunc_prm_getdi(0);

// 10 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

// 11 : result
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround2 : parameter type error !!!\n" );
		return 1;
	}
	int* resultptr;
	resultptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 12 : adjustx
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround2 : parameter type error !!!\n" );
		return 1;
	}
	int* adxptr;
	adxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 13 : adjusty
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround2 : parameter type error !!!\n" );
		return 1;
	}
	int* adyptr;
	adyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 14 : adjustz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround2 : parameter type error !!!\n" );
		return 1;
	}
	int* adzptr;
	adzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 15 : nx10000
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround2 : parameter type error !!!\n" );
		return 1;
	}
	int* nx10000ptr;
	nx10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 16 : ny
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround2 : parameter type error !!!\n" );
		return 1;
	}
	int* ny10000ptr;
	ny10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 17 : nz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGround2 : parameter type error !!!\n" );
		return 1;
	}
	int* nz10000ptr;
	nz10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGround2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 befv, newv;
	befv.x = (float)befposx;
	befv.y = (float)befposy;
	befv.z = (float)befposz;

	newv.x = (float)newposx;
	newv.y = (float)newposy;
	newv.z = (float)newposz;


	ret = groundhs->ChkConfGround2( befv, newv, mode, diffmaxy, mapminy, resultptr, adxptr, adyptr, adzptr, nx10000ptr, ny10000ptr, nz10000ptr );
	if( ret ){
		DbgOut( "E3DChkConfGround : groundhs ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetPartNoByName hsid, partname, partno
EXPORT BOOL WINAPI E3DGetPartNoByName( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	char tempname[MAX_PATH];
	char* nameptr;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DGetPartNoByName : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DGetPartNoByName : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : GetPartNoByName : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );
//3

	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetPartNoByName : parameter type error !!!\n" );
		return 1;
	}
	int* partnoptr;
	partnoptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPartNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetPartNoByName( tempname, partnoptr );
	if( ret ){
		DbgOut( "E3DGetPartNoByName : curhs GetPartNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetVertNumOfPart hsid, partno, vertnum
EXPORT BOOL WINAPI E3DGetVertNumOfPart( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);


//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);


//3 : vertnumptr
	int curtype;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVertNumOfPart : parameter type error !!!\n" );
		return 1;
	}
	int* vertnumptr;
	vertnumptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertNumOfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetVertNumOfPart( partno, vertnumptr );
	if( ret ){
		DbgOut( "E3DGetVertNumOfPart : curhs GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	

	return 0;
}

//E3DGetVertPos hsid, partno, vertno, mult, vertx, verty, vertz
EXPORT BOOL WINAPI E3DGetVertPos( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
//1 : hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(0);

//5 : vxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVertPos : parameter type error !!!\n" );
		return 1;
	}
	int* vxptr;
	vxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//6 : vyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVertPos : parameter type error !!!\n" );
		return 1;
	}
	int* vyptr;
	vyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//7 : vzptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVertPos : parameter type error !!!\n" );
		return 1;
	}
	int* vzptr;
	vzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetVertPos( partno, vertno, mult, vxptr, vyptr, vzptr );
	if( ret ){
		DbgOut( "E3DGetVertPos : curhs GetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}

//E3DSetVertPos hsid, partno, vertno, mult, vertx, verty, vertz
EXPORT BOOL WINAPI E3DSetVertPos( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

//5 : vertx
	int vertx;
	vertx = hei->HspFunc_prm_getdi(0);

//6 : verty
	int verty;
	verty = hei->HspFunc_prm_getdi(0);

//7 : vertz
	int vertz;
	vertz = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetVertPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetVertPos( partno, vertno, mult, vertx, verty, vertz );
	if( ret ){
		DbgOut( "E3DSetVertPos : curhs SetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DSetPosOnGround hsid, groundid, mapmaxy, mapminy, posx, posz

EXPORT BOOL WINAPI E3DSetPosOnGround( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : groundid
	int groundid;
	groundid = hei->HspFunc_prm_getdi(0);

//3 : mapmaxy
	int mapmaxy;
	mapmaxy = hei->HspFunc_prm_getdi(0);

//4 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

//5 : posx
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//6 : posz
	int posz;
	posz = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPosOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetPosOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->SetPosOnGround( curhs, mapmaxy, mapminy, posx, posz );
	if( ret ){
		DbgOut( "E3DSetPosOnGround : groundhs SetPosOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetPosOnGroundPart hsid, groundid, mapmaxy, mapminy, posx, posz
EXPORT BOOL WINAPI E3DSetPosOnGroundPart( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : groundid
	int groundid;
	groundid = hei->HspFunc_prm_getdi(0);

//3 : groundpart
	int groundpart;
	groundpart = hei->HspFunc_prm_getdi(0);

//4 : mapmaxy
	int mapmaxy;
	mapmaxy = hei->HspFunc_prm_getdi(0);

//5 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

//6 : posx
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//7 : posz
	int posz;
	posz = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPosOnGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetPosOnGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->SetPosOnGroundPart( groundpart, curhs, mapmaxy, mapminy, posx, posz );
	if( ret ){
		DbgOut( "E3DSetPosOnGroundPart : groundhs SetPosOnGroundPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DCreateBillboard bmpname, width, height, transparentflag, billboardid, dirmode
EXPORT BOOL WINAPI E3DCreateBillboard( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : bmpname
	int curtype;
	char tempname1[MAX_PATH];
	char* nameptr1 = 0;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DCreateBG : parameter type error !!!\n" );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DCreateBillboard : parameter error !!!\n" );
		return 1;
	}

	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "E3DCreateBillboard : namelen1 too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2 : widht
	int width;
	width = hei->HspFunc_prm_getdi(0);

//3 : height
	int height;
	height = hei->HspFunc_prm_getdi(0);

//4 : transparentflag
	int transparentflag;
	transparentflag = hei->HspFunc_prm_getdi(0);


//5 : billboardidptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCreateBillboard : parameter type error !!!\n" );
		return 1;
	}
	int* bbidptr;
	bbidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//6 : dirmode
	int dirmode;
	dirmode = hei->HspFunc_prm_getdi( 0 );

//7 : orgflag
	int orgflag;
	orgflag = hei->HspFunc_prm_getdi( 0 );


//
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////
	
	int ret;

	if( !g_bbhs ){
		DbgOut( "E3DCreateBillboard : g_bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	SetResDir( tempname1, g_bbhs );//!!!!!!

	ret = g_bbhs->AddBillboard( tempname1, width, height, transparentflag, dirmode, orgflag, bbidptr );
	if( ret || (*bbidptr < 0) ){
		DbgOut( "E3DCreateBillboard : g_bbhs AddBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( *bbidptr >= 0 ){
	//Create 3D Object
		ret = Restore( g_bbhs, *bbidptr );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}


	/////
		ret = g_bbhs->SetCurrentBSphereData();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}



	return 0;
}


//E3DRenderBillboard <--- 引数無し。dispflagの付いているBillboard全てを、適切な順番に（ｚの奥から）描画する
EXPORT BOOL WINAPI E3DRenderBillboard( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//////

	int ret;
	if( g_bbhs ){
		
		if( !g_pD3DApp ){
			_ASSERT( 0 );
			return 1;
		}

		LPDIRECT3DDEVICE9 pdev;
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

//ChkInView
		/***
		float fDeg2Pai;
		fDeg2Pai = (float)DEG2PAI;

		D3DXVECTOR3 viewvec( 0.0f, 0.0f, 0.0f );
		
		if( s_cameratype == 0 ){
			D3DXVECTOR3 targetpos( 0.0f, 0.0f, 0.0f );
			targetpos.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
			targetpos.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
			targetpos.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

			viewvec.x = targetpos.x - s_camerapos.x;
			viewvec.y = targetpos.y - s_camerapos.y;
			viewvec.z = targetpos.z - s_camerapos.z;

			D3DXVec3Normalize( &viewvec, &viewvec );
		}else{
			viewvec.x = s_cameratarget.x - s_camerapos.x;
			viewvec.y = s_cameratarget.y - s_camerapos.y;
			viewvec.z = s_cameratarget.z - s_camerapos.z;
		
			D3DXVec3Normalize( &viewvec, &viewvec );
		}
		***/
		/***
		float fDeg2Pai = (float)DEG2PAI;
	
		if( s_cameratype == 0 ){
			D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

			s_cameratarget.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
			s_cameratarget.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
			s_cameratarget.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

			D3DXMatrixLookAtLH( &s_matView, &s_camerapos, &s_cameratarget, &vUpVec );
		}else{

			D3DXMatrixLookAtLH( &s_matView, &s_camerapos, &s_cameratarget, &s_cameraupvec );
		}
		
		if( s_cameratype == 0 ){
			D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

			s_cameratarget.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
			s_cameratarget.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
			s_cameratarget.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

			D3DXMATRIX tempmatView;
			float cx, cy, cz;

			D3DXMatrixLookAtLH( &tempmatView, &s_camerapos, &s_cameratarget, &vUpVec );
			cx = tempmatView._41;
			cy = tempmatView._42;
			cz = tempmatView._43;
			tempmatView._41 = 0.0f;
			tempmatView._42 = 0.0f;
			tempmatView._43 = 0.0f;

			D3DXVECTOR3 axisvec;
			CQuaternion2 addrot;
			axisvec = s_cameratarget - s_camerapos;
			DXVec3Normalize( &axisvec, &axisvec );
			addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
			
			D3DXMATRIX addrotmat;
			addrotmat = addrot.MakeRotMatX();

			s_matView = tempmatView * addrotmat;
			s_matView._41 = cx;
			s_matView._42 = cy; 
			s_matView._43 = cz;

		}else{

			D3DXMatrixLookAtLH( &s_matView, &s_camerapos, &s_cameratarget, &s_cameraupvec );
		}
		***/

		CalcMatView();

		int ret;
		ret = g_bbhs->ChkInView( s_matView );
		if( ret ){
			DbgOut( "E3DRenderBillboard : ChkInView error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
//Transform

		ret = g_bbhs->TransformBillboard( pdev, s_matView, &s_camerapos, &s_cameratarget );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

//Render		
		//ret = g_bbhs->Render( pdev, 0 );
		ret = g_bbhs->Render( pdev, 1, s_matView );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DRenderBillboard : bbhs not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetBillboardPos billboardid, posx, posy, posz
EXPORT BOOL WINAPI E3DSetBillboardPos( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : bbid
	int bbid;
	bbid = hei->HspFunc_prm_getdi(0);
//2 : posx
	int posx;
	posx = hei->HspFunc_prm_getdi(0);
//3 : posy
	int posy;
	posy = hei->HspFunc_prm_getdi(0);
//4 : posz
	int posz;
	posz = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	int ret;
	if( g_bbhs ){

		ret = g_bbhs->SetBillboardPos( bbid, posx, posy, posz );
		if( ret ){
			DbgOut( "E3DSetBillboardPos : bbhs SetBillboardPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//ret = g_bbhs->SetCurrentBSphereData();
		//if( ret ){
		//	_ASSERT( 0 );
		//	return 1;
		//}

	}else{
		DbgOut( "E3DSetBillboardPos : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


/***
//E3DSetBillboardDispFlag billboardid, flag
EXPORT BOOL WINAPI E3DSetBillboardDispFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : bbid
	int bbid;
	bbid = hei->HspFunc_prm_getdi(0);

//2 : flag
	int flag;
	flag = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	int ret;
	if( g_bbhs ){

		ret = g_bbhs->SetBillboardDispFlag( bbid, flag );
		if( ret ){
			DbgOut( "E3DSetBillboardDispFlag : bbhs SetBillboardDispFlag error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DSetBillboardDispFlag : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
***/


//E3DSetBillboardOnGround billboardid, groundid, mapmaxy, mapminy, posx, posz	 
EXPORT BOOL WINAPI E3DSetBillboardOnGround( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : bbid
	int bbid;
	bbid = hei->HspFunc_prm_getdi(0);

//2 : groundid
	int groundid;
	groundid = hei->HspFunc_prm_getdi(0);

//3 : mapmaxy
	int mapmaxy;
	mapmaxy = hei->HspFunc_prm_getdi(0);

//4 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

//5 : posx
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//6 : posz
	int posz;
	posz = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetBillboardOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_bbhs ){

		ret = g_bbhs->SetBillboardOnGround( bbid, groundhs, mapmaxy, mapminy, posx, posz );
		if( ret ){
			DbgOut( "E3DSetBillboardOnGround : bbhs SetBillboardOnGround error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DSetBillboardOnGround : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DDestroyBillboard billboardid <---- freeは行わない、使い回せるように、未使用のフラグを立てる。
EXPORT BOOL WINAPI E3DDestroyBillboard( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : hsid
	int bbid;
	bbid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	int ret;

	ret = DestroyTexture( -2, bbid, -2 );
	_ASSERT( !ret );

//////////

	if( g_bbhs ){

		ret = g_bbhs->DestroyBillboard( bbid );
		if( ret ){
			DbgOut( "E3DDestroyBillboard : bbhs DestroyBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DDestroyBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DDbgOutBMPHeader bmpname
EXPORT BOOL WINAPI E3DDbgOutBMPHeader( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DDbgOutBMPHeader : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DDbgOutBMPHeader : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DDbgOutBMPHeader : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CGroundBMP* gbmp;
	
	gbmp = new CGroundBMP();
	if( !gbmp ){
		DbgOut( "E3DDbgOutBMPHeader : gbmp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = gbmp->DbgOutBMPHeader( tempname1 );
	if( ret ){
		DbgOut( "E3DDbgOutBMPHeader : gbmp DbgOutBMPHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delete gbmp;

	return 0;
}

//E3DLoadMQOFileAsGround( char* filename, int mult100, int* hsidptr );
EXPORT BOOL WINAPI E3DLoadMQOFileAsGround( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : mqo file name1
	// 2 : mult100
	// 3 : pointer of handlerset id.

	// 4 : adjustuvflag

					// 4 : TLmode : ver1036<----- ver1087で削除

//1
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadMQOFileAsGround : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DLoadMQOFileAsGround : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadMQOFileAsGround : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2 : mult100
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(0);


//3	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "e3dhsp : LoadMQOFileAsGround : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DMQOFileAsGround : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//4
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);


	//int TLmode;
	//TLmode = hei->HspFunc_prm_getdi(2);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////	

	*hsidptr = -1;	
	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsGround_F( tempname1, mult100, s_hwnd, adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : LoadMQOFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		*hsidptr = -1;
		return 1;
	}
	*hsidptr = temphsid;
	return 0;

}

int LoadMQOFileAsGround_F( char* tempname1, int mult100, HWND hwnd, int adjustuvflag, int* hsidptr )
{
	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	SetResDir( tempname1, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	newhs->m_TLmode = 2;//!!!!!!!!!!!! ver1036
	newhs->m_groundflag = 1;//!!!!!!!! ver1036

	ret = newhs->LoadMQOFileAsGround( tempname1, mult100, hwnd, adjustuvflag, hsidptr );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : curhs LoadMQOFileAsGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;
	return 0;

}



//E3DSaveMQOFile( int hsid, char* filename );
EXPORT BOOL WINAPI E3DSaveMQOFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : hsid
	// 2 : file name1

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSaveMQOFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DSaveMQOFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DSaveMQOFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );
////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMQOFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SaveMQOFile( tempname1, s_hwnd );
	if( ret ){
		DbgOut( "E3DSaveMQOFile : curhs SaveMQOFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DGetBillboardInfo( bbid, posxptr, posyptr, poszptr, texname<-- 256, transparentptr, widthptr, heightptr );
EXPORT BOOL WINAPI E3DGetBillboardInfo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1: srcbbid
	int srcbbid;
	srcbbid = hei->HspFunc_prm_getdi(0);

// 2: posxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBillboardInfo : parameter type error !!!\n" );
		return 1;
	}
	int* posxptr;
	posxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

// 3: posyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBillboardInfo : parameter type error !!!\n" );
		return 1;
	}
	int* posyptr;
	posyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 4: poszptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBillboardInfo : parameter type error !!!\n" );
		return 1;
	}
	int* poszptr;
	poszptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数


// 5: texnameptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBillboardInfo : parameter type error !!!\n" );
		return 1;
	}
	char* texnameptr;
	texnameptr = (char *)hei->HspFunc_prm_getv();	// パラメータ5:変数

// 6: transparentptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBillboardInfo : parameter type error !!!\n" );
		return 1;
	}
	int* transparentptr;
	transparentptr = (int *)hei->HspFunc_prm_getv();	// パラメータ6:変数

// 7: widthptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBillboardInfo : parameter type error !!!\n" );
		return 1;
	}
	int* widthptr;
	widthptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 8: heightptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBillboardInfo : parameter type error !!!\n" );
		return 1;
	}
	int* heightptr;
	heightptr = (int *)hei->HspFunc_prm_getv();	// パラメータ8:変数


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////////
////////
	int ret;

	if( g_bbhs ){
		ret = g_bbhs->GetBillboardInfo( srcbbid, posxptr, posyptr, poszptr, texnameptr, transparentptr, widthptr, heightptr );
		if( ret ){
			DbgOut( "E3DGetBillboardInfo : bbhs GetBillboardInfo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DGetBillboardInfo : bbhs not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetNearBillboard( srcposx, srcposy, srcposz, 500.0f, &m_nearbbid );
EXPORT BOOL WINAPI E3DGetNearBillboard( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1: srcposx
	int srcposx;
	srcposx = hei->HspFunc_prm_getdi(0);

// 2: srcposy
	int srcposy;
	srcposy = hei->HspFunc_prm_getdi(0);

// 3: srcposz
	int srcposz;
	srcposz = hei->HspFunc_prm_getdi(0);

// 4: maxdist
	int maxdist;
	maxdist = hei->HspFunc_prm_getdi(0);

// 5: bbidptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetNearBillboard : parameter type error !!!\n" );
		return 1;
	}
	int* bbidptr;
	bbidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////

	int ret;

	if( g_bbhs ){
		ret = g_bbhs->GetNearBillboard( srcposx, srcposy, srcposz, maxdist, bbidptr );
		if( ret ){
			DbgOut( "E3DGetNearBillboard : bbhs GetNearBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		DbgOut( "E3DGetNearBillboard : bbhs not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetInvisibleFlag( -1, m_nearbbid, &invisibleflag );
EXPORT BOOL WINAPI E3DGetInvisibleFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

// 2: partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

// 3: flagptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetNearBillboard : parameter type error !!!\n" );
		return 1;
	}
	int* flagptr;
	flagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////

	int ret;
	CHandlerSet* curhs;
	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DGetInvisibleFlag : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			_ASSERT( 0 );
			return 1;
		}
	}
	

	ret = curhs->GetInvisibleFlag( partno, flagptr );
	if( ret ){
		DbgOut( "E3DGetInvisibleFlag : curhs GetInvisibleFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetInvisibleFlag( -1, m_nearbbid, 1 );
EXPORT BOOL WINAPI E3DSetInvisibleFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

// 1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

// 2: partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

// 3: srcflag
	int srcflag;
	srcflag = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////

	int ret;
	CHandlerSet* curhs;
	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DSetInvisibleFlag : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
	ret = curhs->SetInvisibleFlag( partno, srcflag );
	if( ret ){
		DbgOut( "E3DSetInvisibleFlag : curhs GetInvisibleFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetKeyboardState
EXPORT BOOL WINAPI E3DGetKeyboardState( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetKeyboardState : parameter type error !!!\n" );
		return 1;
	}
	int* stateptr;
	stateptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////
	unsigned char tempstate[256];


	GetKeyboardState( tempstate );

	int cno;
	for( cno = 0; cno < 256; cno++ ){
		if( tempstate[cno] & 0x80 )
			*( stateptr + cno ) = 1;
		else
			*( stateptr + cno ) = 0;
	}

	return 0;
}

//E3DSetMovableArea bmpname, maxx, maxz, divx, divz, wallheight, rethsid
EXPORT BOOL WINAPI E3DSetMovableArea( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1: bmpname
	char tempname1[MAX_PATH];
	char* nameptr1;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSetMovableArea : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DSetMovableArea : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : SetMovableArea : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

// 2: maxx
	int maxx;
	maxx = hei->HspFunc_prm_getdi(0);

// 3: maxz
	int maxz;
	maxz = hei->HspFunc_prm_getdi(0);

// 4: divx
	int divx;
	divx = hei->HspFunc_prm_getdi(0);

// 5: divz
	int divz;
	divz = hei->HspFunc_prm_getdi(0);

// 6: wallheight
	int wallheight;
	wallheight = hei->HspFunc_prm_getdi(0);

// 7: rethsid
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSetMovableArea : parameter type error !!!\n" );
		return 1;
	}
	int* hsidptr;
	hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////


	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	SetResDir( tempname1, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit



	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetMovableArea( tempname1, maxx, maxz, divx, divz, wallheight );
	if( ret ){
		DbgOut( "easy3d : E3DSetMovableArea : SetMovableArea error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;

	return 0;
}

//E3DChkConfWall( int charahsid, int groundhsid, float dist, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv );
EXPORT BOOL WINAPI E3DChkConfWall( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int ret;
	int curtype;

// 1: charahsid
	int charahsid;
	charahsid = hei->HspFunc_prm_getdi(0);

// 2: groundhsid
	int groundhsid;
	groundhsid = hei->HspFunc_prm_getdi(0);

// 3: dist
	int dist;
	dist = hei->HspFunc_prm_getdi(0);

// 4: resultptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall : parameter type error !!!\n" );
		return 1;
	}
	int* resultptr;
	resultptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数


// 5: adjustxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall : parameter type error !!!\n" );
		return 1;
	}
	int* adjustxptr;
	adjustxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 6: adjustyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall : parameter type error !!!\n" );
		return 1;
	}
	int* adjustyptr;
	adjustyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 7: adjustzptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall : parameter type error !!!\n" );
		return 1;
	}
	int* adjustzptr;
	adjustzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 8: nx10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall : parameter type error !!!\n" );
		return 1;
	}
	int* nx10000ptr;
	nx10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 9: ny10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall : parameter type error !!!\n" );
		return 1;
	}
	int* ny10000ptr;
	ny10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 10: nz10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall : parameter type error !!!\n" );
		return 1;
	}
	int* nz10000ptr;
	nz10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfWall : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfWall : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	ret = groundhs->ChkConfWall( charahs, (float)dist, resultptr, adjustxptr, adjustyptr, adjustzptr, nx10000ptr, ny10000ptr, nz10000ptr );
	if( ret ){
		DbgOut( "E3DChkConfWall : groundhs ChkConfWall error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DChkConfWall2( D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int groundhsid, float dist, int* resultptr, D3DXVECTOR3* adjustv, D3DXVECTOR3* nv );
EXPORT BOOL WINAPI E3DChkConfWall2( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int ret;
	int curtype;

// 1: befposx
	int befposx;
	befposx = hei->HspFunc_prm_getdi(0);

// 2: befposy
	int befposy;
	befposy = hei->HspFunc_prm_getdi(0);

// 3: befposz
	int befposz;
	befposz = hei->HspFunc_prm_getdi(0);

// 4: newposx
	int newposx;
	newposx = hei->HspFunc_prm_getdi(0);

// 5: newposy
	int newposy;
	newposy = hei->HspFunc_prm_getdi(0);

// 6: newposz
	int newposz;
	newposz = hei->HspFunc_prm_getdi(0);

// 7: groundhsid
	int groundhsid;
	groundhsid = hei->HspFunc_prm_getdi(0);

// 8: dist
	int dist;
	dist = hei->HspFunc_prm_getdi(0);

// 9: resultptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall2 : parameter type error !!!\n" );
		return 1;
	}
	int* resultptr;
	resultptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 10: adjustxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall2 : parameter type error !!!\n" );
		return 1;
	}
	int* adjustxptr;
	adjustxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 11: adjustyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall2 : parameter type error !!!\n" );
		return 1;
	}
	int* adjustyptr;
	adjustyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 12: adjustzptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall2 : parameter type error !!!\n" );
		return 1;
	}
	int* adjustzptr;
	adjustzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 13: nx10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall2 : parameter type error !!!\n" );
		return 1;
	}
	int* nx10000ptr;
	nx10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 14: ny10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall2 : parameter type error !!!\n" );
		return 1;
	}
	int* ny10000ptr;
	ny10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 15: nz10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfWall2 : parameter type error !!!\n" );
		return 1;
	}
	int* nz10000ptr;
	nz10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfWall2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->ChkConfWall2( (float)befposx, (float)befposy, (float)befposz, (float)newposx, (float)newposy, (float)newposz, (float)dist, resultptr, adjustxptr, adjustyptr, adjustzptr, nx10000ptr, ny10000ptr, nz10000ptr );
	if( ret ){
		DbgOut( "E3DChkConfWall2 : groundhs ChkConfWall2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	return 0;
}


EXPORT BOOL WINAPI E3DVec3Normalize( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1: befposx
	int befposx;
	befposx = hei->HspFunc_prm_getdi(0);

// 2: befposy
	int befposy;
	befposy = hei->HspFunc_prm_getdi(0);

// 3: befposz
	int befposz;
	befposz = hei->HspFunc_prm_getdi(0);

// 4: mult
	int mult;
	mult = hei->HspFunc_prm_getdi(0);

// 5: newposxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DVec3Normalize : parameter type error !!!\n" );
		return 1;
	}
	int* newposxptr;
	newposxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 6: newposyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DVec3Normalize : parameter type error !!!\n" );
		return 1;
	}
	int* newposyptr;
	newposyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 7: newposzptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DVec3Normalize : parameter type error !!!\n" );
		return 1;
	}
	int* newposzptr;
	newposzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////

	D3DXVECTOR3 vec;
	vec.x = (float)befposx;
	vec.y = (float)befposy;
	vec.z = (float)befposz;

	D3DXVec3Normalize( &vec, &vec );

	*newposxptr = (int)( (float)mult * vec.x );
	*newposyptr = (int)( (float)mult * vec.y );
	*newposzptr = (int)( (float)mult * vec.z );

	return 0;
}

//E3DVec2CCW nbasevecx, nbasevecz, nvecx, nvecz, ccw
EXPORT BOOL WINAPI E3DVec2CCW( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1: posx1
	int posx1;
	posx1 = hei->HspFunc_prm_getdi(0);

// 2: posy1
	int posy1;
	posy1 = hei->HspFunc_prm_getdi(0);

// 3: posx2
	int posx2;
	posx2 = hei->HspFunc_prm_getdi(0);

// 4: posy2
	int posy2;
	posy2 = hei->HspFunc_prm_getdi(0);

// 5: ccwptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DVec2CCW : parameter type error !!!\n" );
		return 1;
	}
	int* ccwptr;
	ccwptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////

	D3DXVECTOR2 vec1, vec2;

	vec1.x = (float)posx1;
	vec1.y = (float)posy1;

	vec2.x = (float)posx2;
	vec2.y = (float)posy2;

	float ccw;
	ccw = D3DXVec2CCW( &vec1, &vec2 );
	
	if( ccw >= 0.0f )
		*ccwptr = 1;
	else
		*ccwptr = -1;


	return 0;
}

//E3DVec3RotateY nx1, ny1, nz1, 90, kabex1, kabey1, kabez1, degmult
EXPORT BOOL WINAPI E3DVec3RotateY( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1: vecx
	int vecx;
	vecx = hei->HspFunc_prm_getdi(0);

// 2: vexy
	int vecy;
	vecy = hei->HspFunc_prm_getdi(0);

// 3: vecz
	int vecz;
	vecz = hei->HspFunc_prm_getdi(0);

// 4: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

// 5; xptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DVec3RotateY : parameter type error !!!\n" );
		return 1;
	}
	int* xptr;
	xptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 6: yptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DVec3RotateY : parameter type error !!!\n" );
		return 1;
	}
	int* yptr;
	yptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 7: zptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DVec3RotateY : parameter type error !!!\n" );
		return 1;
	}
	int* zptr;
	zptr = (int *)hei->HspFunc_prm_getv();	// パラメータ7:変数

// 8: degmult
	int degmult;
	degmult = hei->HspFunc_prm_getdi(1);


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////
/////////
	int ret;
	CQuaternion2 dirq;
	D3DXMATRIX	dirm;

	float fdeg;
	if( degmult != 0 )
		fdeg = (float)deg / (float)degmult;
	else
		fdeg = (float)deg;


	ret = dirq.SetRotation( 0, fdeg, 0 );
	_ASSERT( !ret );
	dirm = dirq.MakeRotMatX();

	float fvecx, fvecy, fvecz;
	fvecx = (float)vecx;
	fvecy = (float)vecy;
	fvecz = (float)vecz;


	float aftx, afty, aftz;
	aftx = dirm._11 * fvecx + dirm._21 * fvecy + dirm._31 * fvecz + dirm._41;
	afty = dirm._12 * fvecx + dirm._22 * fvecy + dirm._32 * fvecz + dirm._42;
	aftz = dirm._13 * fvecx + dirm._23 * fvecy + dirm._33 * fvecz + dirm._43;
	//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
	
	*xptr = (int)aftx;
	*yptr = (int)afty;
	*zptr = (int)aftz;


	return 0;
}

//E3DLoadMQOFileAsMovableArea mqofile, mult100, rethsid
EXPORT BOOL WINAPI E3DLoadMQOFileAsMovableArea( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : mqo file name1
	// 2 : mult100
	// 3 : pointer of handlerset id.

//1
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadMQOFileAsMovableArea : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadMQOFileAsMovableArea : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2 : mult100
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);


//3	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "e3dhsp : LoadMQOFileAsMovableArea : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableArea: hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////	

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableArea_F( tempname1, mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : LoadMQOFileAsMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		*hsidptr = -1;
	}
	*hsidptr = temphsid;
	return 0;

	
	return 0;
}

int LoadMQOFileAsMovableArea_F( char* tempname1, int mult100, int* hsidptr )
{
	*hsidptr = -1;

	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	SetResDir( tempname1, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	
	ret = newhs->LoadMQOFileAsMovableArea( tempname1, mult100, s_hwnd, hsidptr );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : curhs LoadMQOFileAsMovableArea error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;

	return 0;
}



//E3DLoadSound filename, soundid, use3dflag, bufnum
EXPORT BOOL WINAPI E3DLoadSound( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : file name1
	// 2 : pointer of sound id.
	// 3 : use3dflag
	// 4 : bufnum

//1
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadSound : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DLoadSound : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadSound : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2
	curtype = *hei->nptype;
	int* soundidptr;
	if( curtype == 4 ){
		soundidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !soundidptr ){
			DbgOut( "e3dhsp : LoadSound : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadSound : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//3
	int use3dflag;
	use3dflag = hei->HspFunc_prm_getdi(0);


//4
	int bufnum;
	bufnum = hei->HspFunc_prm_getdi(1);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////	

	int ret;

	if( !s_HS ){
		DbgOut( "E3DLoadSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



// directoryのセット
	char dirname[ MAX_PATH ];
	strcpy( dirname, tempname1 );
	int ch = '\\';
	char* lasten;
	lasten = strrchr( dirname, ch );
	int dirleng = 0;
	if( lasten ){
		*(lasten + 1) = 0;//\\は残す。
		
		dirleng = strlen( dirname );//

		ret = s_HS->SetSearchDirectory( dirname );
		if( ret ){
			DbgOut( "E3DLoadSound : hs SetSearchDirectory error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


// 読み込み

	if( lasten ){
		ret = s_HS->LoadSoundFile( tempname1 + dirleng, use3dflag, bufnum, soundidptr );
	}else{
		ret = s_HS->LoadSoundFile( tempname1, use3dflag, bufnum, soundidptr );
	}
	if( ret ){
		DbgOut( "E3DLoadSound : hs LoadSoundFile error %s !!!\n", tempname1 );
		_ASSERT( 0 );
		return 1;
	}

	DbgOut( "E3DLoadSound : success : %s\n", tempname1 );

	return 0;
}



//E3DPlaySound soundid, isprimary
EXPORT BOOL WINAPI E3DPlaySound( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

	int isprimary;
	isprimary = hei->HspFunc_prm_getdi(1);

	int boundaryflag;
	boundaryflag = hei->HspFunc_prm_getdi(1);


///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DPlaySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD dwboundary = (DWORD)boundaryflag;

	int ret;
	ret = s_HS->PlaySound( soundid, isprimary, dwboundary );
	if( ret ){
		DbgOut( "E3DPlaySound : hs PlaySound error %d!!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DStopSound soundid
EXPORT BOOL WINAPI E3DStopSound( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

	int flag;
	flag = hei->HspFunc_prm_getdi(0);


///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DStopSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->StopSound( soundid, flag );
	if( ret ){
		DbgOut( "E3DStopSound : hs StopSound error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetSoundLoop soundid, loopflag
EXPORT BOOL WINAPI E3DSetSoundLoop( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

	int loopflag;
	loopflag = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundLoop : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetRepeats( soundid, loopflag );
	if( ret ){
		DbgOut( "E3DSetSoundLoop : hs SetRepeats error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetSoundVolume volume
EXPORT BOOL WINAPI E3DSetSoundVolume( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int volume;
	volume = hei->HspFunc_prm_getdi(0);

//2
	int soundid;
	soundid = hei->HspFunc_prm_getdi(-1);//!!!!! default は　-1 


///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundVolume : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = s_HS->SetVolume( soundid, volume );
	if( ret ){
		DbgOut( "E3DSetSoundVolume : hs SetVolume error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

//E3DSetSoundTempo tempo
EXPORT BOOL WINAPI E3DSetSoundTempo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int tempo100;
	tempo100 = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundTempo : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	float fTempo;
	fTempo = (float)tempo100 / 100.0f;

	ret = s_HS->SetMasterTempo( fTempo );
	if( ret ){
		DbgOut( "E3DSetSoundTempo : hs SetMasterTempo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DDestroySound( int soundid );
EXPORT BOOL WINAPI E3DDestroySound( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DDestroySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->DestroySound( soundid );
	if( ret ){
		DbgOut( "E3DDestroySound : hs DestroySound error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetSoundFrequency( int soundid, DWORD freq );
EXPORT BOOL WINAPI E3DSetSoundFrequency( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);
//2
	int freq;
	freq = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DSetSoundFrequency : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetFrequency( soundid, (DWORD)freq );
	if( ret ){
		DbgOut( "E3DSetSoundFrequency : hs SetFrequency error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetSoundVolume( int soundid, LONG* pvol );
EXPORT BOOL WINAPI E3DGetSoundVolume( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

//2
	int curtype;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSoundVolume : parameter type error !!!\n" );
		return 1;
	}
	int* pvol;
	pvol = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DGetSoundVolume : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LONG vol = 0;
	ret = s_HS->GetVolume( soundid, &vol );
	if( ret ){
		DbgOut( "E3DGetSoundVolume : hs GetVolume error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}
	*pvol = (int)vol;


	return 0;
}


//E3DGetSoundFrequency( int soundid, DWORD* pfreq );
EXPORT BOOL WINAPI E3DGetSoundFrequency( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

//2
	int curtype;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSoundFrequency : parameter type error !!!\n" );
		return 1;
	}
	int* pfreq;
	pfreq = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DGetSoundFrequency : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	DWORD freq = 0;
	ret = s_HS->GetFrequency( soundid, &freq );
	if( ret ){
		DbgOut( "E3DGetSoundFrequency : hs GetFrequency error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}
	*pfreq = (int)freq;

	return 0;
}
	


//E3DSet3DSoundListener( float fDopplerFactor, float fRolloffFactor );
EXPORT BOOL WINAPI E3DSet3DSoundListener( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int doppler100;
	doppler100 = hei->HspFunc_prm_getdi(0);

//2
	int rolloff100;
	rolloff100 = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundListener : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fdoppler;
	float frolloff;

	fdoppler = (float)doppler100 / 100.0f;
	frolloff = (float)rolloff100 / 100.0f;

	int ret;
	ret = s_HS->Set3DListener( fdoppler, frolloff );
	if( ret ){
		DbgOut( "E3DSet3DSoundListener : hs Set3DListener error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSet3DSoundListenerMovement( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DPlaySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vel( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 dir( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 updir( 0.0f, 1.0f, 0.0f );

	D3DXVECTOR3 aftdir, aftupdir;

	if( hsid > 0 ){
		CHandlerSet* curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DRender : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		pos.x = curhs->m_charaset.position.x;
		pos.y = curhs->m_charaset.position.y;
		pos.z = curhs->m_charaset.position.z;

		curhs->m_charaset.dirq.Rotate( &aftdir, dir );
		curhs->m_charaset.dirq.Rotate( &aftupdir, updir );

		
		ret = s_HS->Set3DListenerMovement( &pos, &vel, &aftdir, &aftupdir );
		if( ret ){
			DbgOut( "E3DSet3DSoundListenerMovement : hs Set3DListenerMovement error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		pos.x = s_camerapos.x;
		pos.y = s_camerapos.y;
		pos.z = s_camerapos.z;
		

		float fDeg2Pai = (float)DEG2PAI;	
		if( s_cameratype == 0 ){
			D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

			s_cameratarget.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
			s_cameratarget.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
			s_cameratarget.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

			aftdir = s_cameratarget - s_camerapos;
			D3DXVec3Normalize( &aftdir, &aftdir );

			//aftupdir = vUpVec;


			s_cameraupvec = vUpVec;
			CalcMatView();//ツイスト前のmatView

			/***
		// twistをしたs_cameraupvecを求める。
			D3DXMATRIX tempmatView;
			float cx, cy, cz;

			tempmatView = s_matView;
			cx = tempmatView._41;
			cy = tempmatView._42;
			cz = tempmatView._43;
			tempmatView._41 = 0.0f;
			tempmatView._42 = 0.0f;
			tempmatView._43 = 0.0f;

			D3DXVECTOR3 axisvec;
			CQuaternion2 addrot;
			axisvec = s_cameratarget - s_camerapos;
			DXVec3Normalize( &axisvec, &axisvec );
			addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
				
			D3DXMATRIX addrotmat;
			addrotmat = addrot.MakeRotMatX();

			s_matView = tempmatView * addrotmat;
			//D3DXMATRIX invview;
			//D3DXMatrixInverse( &invview, NULL, &s_matView );

			D3DXVECTOR3 upv( 0.0f, 1.0f, 0.0f );
			D3DXVec3TransformCoord( &s_cameraupvec, &upv, &s_matView );

			aftupdir = s_cameraupvec;
			***/
		// twistをしたs_cameraupvecを求める。
			D3DXMATRIX tempmatView;
			float cx, cy, cz;

			tempmatView = s_matView;
			cx = tempmatView._41;
			cy = tempmatView._42;
			cz = tempmatView._43;
			tempmatView._41 = 0.0f;
			tempmatView._42 = 0.0f;
			tempmatView._43 = 0.0f;



			D3DXVECTOR3 axisvec;
			CQuaternion2 addrot;
			axisvec = s_cameratarget - s_camerapos;
			DXVec3Normalize( &axisvec, &axisvec );
			addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
			
			
			D3DXQUATERNION viewqx;
			D3DXQuaternionRotationMatrix( &viewqx, &tempmatView );

			CQuaternion2 viewq;
			viewq.x = viewqx.x;
			viewq.y = viewqx.y;
			viewq.z = viewqx.z;
			viewq.w = viewqx.w;
			
			CQuaternion2 invview;
			viewq.inv( &invview );

			CQuaternion2 multq;
			multq = addrot * invview;

			D3DXVECTOR3 upv( 0.0f, 1.0f, 0.0f );
			multq.Rotate( &s_cameraupvec, upv );

			DXVec3Normalize( &s_cameraupvec, &s_cameraupvec );
			aftdir = s_cameraupvec;

		}else{

			aftdir = s_cameratarget - s_camerapos;
			D3DXVec3Normalize( &aftdir, &aftdir );

			aftupdir = s_cameraupvec;
		}




		ret = s_HS->Set3DListenerMovement( &pos, &vel, &aftdir, &aftupdir );
		if( ret ){
			DbgOut( "E3DSet3DSoundListenerMovement : hs Set3DListenerMovement error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

//E3DSet3DSoundDistance( int soundid, float fMinDistance, float fMaxDistance );
EXPORT BOOL WINAPI E3DSet3DSoundDistance( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

//2
	int min100;
	min100 = hei->HspFunc_prm_getdi(0);

//3
	int max100;
	max100 = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundDistance : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fmin, fmax;
	fmin = (float)min100 / 100.0f;
	fmax = (float)max100 / 100.0f;

	int ret;
	ret = s_HS->Set3DDistance( soundid, fmin, fmax );
	if( ret ){
		DbgOut( "E3DSet3DSoundDistance : hs Set3DDistance error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
//E3DSet3DSoundMovement( int soundid, D3DXVECTOR3* pvPosition, D3DXVECTOR3* pvVelocity );
EXPORT BOOL WINAPI E3DSet3DSoundMovement( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202


//1
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

//2
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//3
	int posy;
	posy = hei->HspFunc_prm_getdi(0);

//4
	int posz;
	posz = hei->HspFunc_prm_getdi(0);

//5
	int vx;
	vx = hei->HspFunc_prm_getdi(0);

//6
	int vy;
	vy = hei->HspFunc_prm_getdi(0);

//7
	int vz;
	vz = hei->HspFunc_prm_getdi(0);

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundMovement : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 vel;

	pos.x = (float)posx;
	pos.y = (float)posy;
	pos.z = (float)posz;

	vel.x = (float)vx;
	vel.y = (float)vy;
	vel.z = (float)vz;

	int ret;
	ret = s_HS->Set3DMovement( soundid, &pos, &vel );
	if( ret ){
		DbgOut( "E3DSet3DSoundMovement : hs Set3DMovement error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}





/***
int SetFovIndex()
{
	D3DXMATRIX matProj;
    D3DXMatrixIdentity( &matProj );
	float fovrad2, h, w, Q;

	fovrad2 = (float)g_proj_fov * 0.5f * (float)DEG2PAI;
	w = 1.0f / tanf( fovrad2 );
	h = w;
	Q = (float)g_proj_far / ( (float)g_proj_far - (float)g_proj_near );
	
    matProj._11 = w;
    matProj._22 = h;
    matProj._33 = Q;
    matProj._34 = 1.0f;
    //matProj._43 = -Q * near_plane;//101.01
    matProj._43 = -Q * (float)g_proj_near;//101.01

//////////
	float curdist;
	float diststep;
	float radarray[FOVINDEXSIZE + 1];

	diststep = ((float)g_proj_far - (float)g_proj_near) / (float)FOVINDEXSIZE;
	int i;
	for( i = 0; i <= FOVINDEXSIZE; i++ ){
		
		curdist = (float)g_proj_near + diststep * (float)i;


		D3DXVECTOR3 orgv;
		orgv.z = curdist;
		orgv.x = tanf( fovrad2 ) * orgv.z;
		orgv.y = 0.0f;


		float xp, yp, zp, wp;
		xp = matProj._11 * orgv.x + matProj._21 * orgv.y + matProj._31 * orgv.z + matProj._41;
		yp = matProj._12 * orgv.x + matProj._22 * orgv.y + matProj._32 * orgv.z + matProj._42;
		zp = matProj._13 * orgv.x + matProj._23 * orgv.y + matProj._33 * orgv.z + matProj._43;
		wp = matProj._14 * orgv.x + matProj._24 * orgv.y + matProj._34 * orgv.z + matProj._44;
				
		float orgrad, transrad;
		orgrad = (float)atan2( orgv.x, orgv.z );
		transrad = (float)atan2( xp, zp );

		radarray[ i ] = transrad;
	}

	float fovrate, newfov;
	float maxfov = 179.0f * (float)DEG2PAI;

	for( i = 0; i <= FOVINDEXSIZE; i++ ){
		fovrate = radarray[ 0 ] / radarray[ i ];
		newfov = (float)g_proj_fov * (float)DEG2PAI * fovrate;
		

		if( newfov <= maxfov ){
			g_fovindex[ i ] = newfov;
		}else{
			g_fovindex[ i ] = maxfov;
		}

		//DbgOut( "SetFovIndex %d : fovrate %f\n", i, fovrate );

	}

	return 0;
}
***/
int SetFovIndex()
{
	int i;
	float minrate = 1.0f;
	float maxrate = 1.5f;

	float ratestep;
	ratestep = ( maxrate - minrate ) / (float)FOVINDEXSIZE;

	for( i = 0; i <= FOVINDEXSIZE; i++ ){
		float currate = minrate + ratestep * (float)i;
		g_fovindex[ i ] = (float)g_proj_fov * (float)DEG2PAI * currate;

		//DbgOut( "SetFovIndex %d : currate %f\n", i, currate );
	}

	return 0;
}

EXPORT BOOL WINAPI E3DGetVersion( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

	curtype = *hei->nptype;
	int* verptr;
	if( curtype == 4 ){
		verptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !verptr ){
			DbgOut( "e3dhsp : GetVersion : verptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetVersion : verptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////
	
	*verptr = E3DHSPVERSION;

	return 0;
}


EXPORT BOOL WINAPI E3DGetCopyRight( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
	curtype = *hei->nptype;
	char* cpptr;
	if( curtype == 4 ){
		cpptr = (char *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !cpptr ){
			DbgOut( "e3dhsp : GetCopyRight : cpptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetCopyRight : cpptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	strcpy( cpptr, "Copyright(C)ochikko LAB, http://www5d.biglobe.ne.jp/~ochikko" );

	return 0;
}



//E3DCreateNaviLine &lineid 
EXPORT BOOL WINAPI E3DCreateNaviLine( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
	curtype = *hei->nptype;
	int* nlidptr;
	if( curtype == 4 ){
		nlidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !nlidptr ){
			DbgOut( "e3dhsp : CreateNaviLine : nlidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCreateNaviLine : nlidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

///////
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	*nlidptr = -1;	

	CNaviLine* newnl;

	newnl = new CNaviLine();
	if( !newnl ){
		DbgOut( "E3DCreateNaviLine : newnl alloc error !!!\n" );
		return 1;
	}

	newnl->next = g_nlhead;
	g_nlhead = newnl;

	*nlidptr = newnl->lineid;

	return 0;
}



//E3DDestroyNaviLine lineid
EXPORT BOOL WINAPI E3DDestroyNaviLine( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int nlid = hei->HspFunc_prm_getdi(0);

/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////

	CNaviLine* delnl = 0;
	CNaviLine* chknl = g_nlhead;
	CNaviLine* befnl = 0;
	int isfound = 0;

	while( (isfound == 0) && chknl ){
		if( chknl->lineid == nlid ){
			delnl = chknl;
			isfound = 1;
			break;
		}
		befnl = chknl;
		chknl = chknl->next;
	}
	
	if( isfound ){
		if( befnl ){
			befnl->next = delnl->next;
		}else{
			g_nlhead = delnl->next;
		}
		delete delnl;

	}else{
		DbgOut( "E3DDestroyNaviLine : isfound 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DAddNaviPoint lineid, befpointid, &pointid // befpointidの次にチェインする。bef==-1のとき最後に、bef==-2のとき最初にチェインする
EXPORT BOOL WINAPI E3DAddNaviPoint( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int befnpid = hei->HspFunc_prm_getdi(0);
///
	int curtype;
	curtype = *hei->nptype;
	int* npidptr;
	if( curtype == 4 ){
		npidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !npidptr ){
			DbgOut( "e3dhsp : AddNaviPoint : npidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DAddNaviPoint : npidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
	

/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DAddNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->AddNaviPoint( befnpid, 1, npidptr );
	if( ret ){
		DbgOut( "E3DAddNaviPoint : curnl AddNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DRemoveNaviPoint lineid, pointid
EXPORT BOOL WINAPI E3DRemoveNaviPoint( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int npid = hei->HspFunc_prm_getdi(0);
	

/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DRemoveNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curnl->RemoveNaviPoint( npid );
	if( ret ){
		DbgOut( "E3DRemoveNaviPoint : curnl RemoveNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}


//E3DGetNaviPointPos lineid, pointid, &pos	// lineid, pointid を指定して、pointの座標を取得する。
EXPORT BOOL WINAPI E3DGetNaviPointPos( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int npid = hei->HspFunc_prm_getdi(0);
///
	int curtype;

	curtype = *hei->nptype;
	int* posxptr;
	if( curtype == 4 ){
		posxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !posxptr ){
			DbgOut( "e3dhsp : GetNaviPointPos : posxptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNaviPointPos : posxptr parameter type error %d!!!\n", curtype );
		return 1;
	}
	
///
	int* posyptr;
	if( curtype == 4 ){
		posyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !posyptr ){
			DbgOut( "e3dhsp : GetNaviPointPos : posyptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNaviPointPos : posyptr parameter type error %d!!!\n", curtype );
		return 1;
	}
///
	int* poszptr;
	if( curtype == 4 ){
		poszptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !poszptr ){
			DbgOut( "e3dhsp : GetNaviPointPos : poszptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNaviPointPos : poszptr parameter type error %d!!!\n", curtype );
		return 1;
	}

/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNaviPointPos : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	D3DXVECTOR3 dstpos;
	ret = curnl->GetNaviPointPos( npid, &dstpos );
	if( ret ){
		DbgOut( "E3DGetNaviPointPos : curnl GetNaviPointPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posxptr = (int)(dstpos.x);
	*posyptr = (int)(dstpos.y);
	*poszptr = (int)(dstpos.z);


	return 0;
}
//E3DSetNaviPointPos lineid, pointid, srcpos
EXPORT BOOL WINAPI E3DSetNaviPointPos( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int npid = hei->HspFunc_prm_getdi(0);
///
	int posx = hei->HspFunc_prm_getdi(0);
///
	int posy = hei->HspFunc_prm_getdi(0);
///
	int posz = hei->HspFunc_prm_getdi(0);
/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DSetNaviPointPos : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 srcpos;
	srcpos.x = (float)posx;
	srcpos.y = (float)posy;
	srcpos.z = (float)posz;

	ret = curnl->SetNaviPointPos( npid, &srcpos );
	if( ret ){
		DbgOut( "E3DSetNaviPointPos : curnl SetNaviPointPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	

	return 0;
}


//E3DGetNaviPointOwnerID lineid, pointid, &ownerid
EXPORT BOOL WINAPI E3DGetNaviPointOwnerID( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int npid = hei->HspFunc_prm_getdi(0);
///
	int curtype;

	curtype = *hei->nptype;
	int* oidptr;
	if( curtype == 4 ){
		oidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !oidptr ){
			DbgOut( "e3dhsp : GetNaviPointOwnerID : oidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNaviPointOwnerID : oidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
	
/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNaviPointOwnerID : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->GetNaviPointOwnerID( npid, oidptr );
	if( ret ){
		DbgOut( "E3DGetNaviPointOwnerID : curnl GetNaviPointOwnerID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetNaviPointOwnerID lineid, pointid, ownerid
EXPORT BOOL WINAPI E3DSetNaviPointOwnerID( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int npid = hei->HspFunc_prm_getdi(0);
///
	int oid = hei->HspFunc_prm_getdi(0);
/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DSetNaviPointOwnerID : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->SetNaviPointOwnerID( npid, oid );
	if( ret ){
		DbgOut( "E3DSetNaviPointOwnerID : curnl SetNaviPointOwnerID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}

//E3DGetNextNaviPoint lineid, pointid, &nextid 
//pointid のnext チェインのpointidを返す。pointid == -1 のとき、先頭pointを返すnextid == -1のときは、nextなし、
EXPORT BOOL WINAPI E3DGetNextNaviPoint( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int npid = hei->HspFunc_prm_getdi(0);
///
	int curtype;

	curtype = *hei->nptype;
	int* nextptr;
	if( curtype == 4 ){
		nextptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !nextptr ){
			DbgOut( "e3dhsp : GetNextNaviPoint : nextptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNextNaviPoint : nextptr parameter type error %d!!!\n", curtype );
		return 1;
	}
	
/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNextNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->GetNextNaviPoint( npid, nextptr );
	if( ret ){
		DbgOut( "E3DGetNextNaviPoint : curnl GetNextNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetPrevNaviPoint lineid, pointid, &befid　// befid == -1のときは、befなし
EXPORT BOOL WINAPI E3DGetPrevNaviPoint( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int npid = hei->HspFunc_prm_getdi(0);
///
	int curtype;

	curtype = *hei->nptype;
	int* prevptr;
	if( curtype == 4 ){
		prevptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !prevptr ){
			DbgOut( "e3dhsp : GetPrevNaviPoint : prevptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetPrevNaviPoint : prevptr parameter type error %d!!!\n", curtype );
		return 1;
	}
	
/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetPrevNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->GetPrevNaviPoint( npid, prevptr );
	if( ret ){
		DbgOut( "E3DGetPrevNaviPoint : curnl GetPrevNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetNearestNaviPoint lineid, srcpos, &nearid, &previd, &nextid // 一番近いNaviPointと、その前後のNaviPointのpointidを取得する
EXPORT BOOL WINAPI E3DGetNearestNaviPoint( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int posx = hei->HspFunc_prm_getdi(0);
///
	int posy = hei->HspFunc_prm_getdi(0);
///
	int posz = hei->HspFunc_prm_getdi(0);
///
	int curtype;

	curtype = *hei->nptype;
	int* nearptr;
	if( curtype == 4 ){
		nearptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !nearptr ){
			DbgOut( "e3dhsp : GetNearestNaviPoint : nearptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNearestNaviPoint : nearptr parameter type error %d!!!\n", curtype );
		return 1;
	}
///
	curtype = *hei->nptype;
	int* prevptr;
	if( curtype == 4 ){
		prevptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !prevptr ){
			DbgOut( "e3dhsp : GetNearestNaviPoint : prevptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNearestNaviPoint : prevptr parameter type error %d!!!\n", curtype );
		return 1;
	}
///
	curtype = *hei->nptype;
	int* nextptr;
	if( curtype == 4 ){
		nextptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !nextptr ){
			DbgOut( "e3dhsp : GetNearestNaviPoint : nextptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetNearestNaviPoint : nextptr parameter type error %d!!!\n", curtype );
		return 1;
	}
/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetPrevNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 srcpos;
	srcpos.x = (float)posx;
	srcpos.y = (float)posy;
	srcpos.z = (float)posz;

	ret = curnl->GetNearestNaviPoint( &srcpos, nearptr, prevptr, nextptr );
	if( ret ){
		DbgOut( "E3DGetNearestNaviPoint : curnl GetNearestNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DFillUpNaviLine lineid, div, flag // 3つ以上点がないときは、何もしない。pointid を付け替えるかどうかのフラグも？？
EXPORT BOOL WINAPI E3DFillUpNaviLine( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
///
	int nlid = hei->HspFunc_prm_getdi(0);
///
	int div = hei->HspFunc_prm_getdi(0);
///
	int flag = hei->HspFunc_prm_getdi(0);

/////////
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////

	if( div < 2 ){
		DbgOut( "E3DFillUpNaviLine : div too small warning : exit !!!\n" );
		return 0;
	}


	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DFillUpNaviLine : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->FillUpNaviLine( div, flag );
	if( ret ){
		DbgOut( "E3DFillUpNaviLine : curnl FillUpNaviLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DSetNaviLineOnGround lineid, groundid, mapmaxy, mapminy //全てのnavipointを地面の高さにセットする。
EXPORT BOOL WINAPI E3DSetNaviLineOnGround( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 : hsid
	int nlid;
	nlid = hei->HspFunc_prm_getdi(0);

//2 : groundid
	int groundid;
	groundid = hei->HspFunc_prm_getdi(0);

//3 : mapmaxy
	int mapmaxy;
	mapmaxy = hei->HspFunc_prm_getdi(0);

//4 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DSetNaviLineOnGround : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundid );
	if( !groundhs ){
		DbgOut( "E3DSetNaviLineOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->SetNaviLineOnGround( curnl, (float)mapmaxy, (float)mapminy );
	if( ret ){
		DbgOut( "E3DSetNaviLineOnGround : groundhs SetNaviLineOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DControlByNaviLine hsid, lineid, ctrlmode, roundflag, reverseflag, maxdist, posstep, dirstep100, &newpos, &newdirQ, &targetpointid
EXPORT BOOL WINAPI E3DControlByNaviLine( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1 : hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

// 2 : nlid
	int nlid;
	nlid = hei->HspFunc_prm_getdi(0);

// 3 : ctrlmode
	int ctrlmode;
	ctrlmode = hei->HspFunc_prm_getdi(0);

// 4 : roundflag
	int roundflag;
	roundflag = hei->HspFunc_prm_getdi(0);

// 5 : reverseflag
	int reverseflag;
	reverseflag = hei->HspFunc_prm_getdi(0);

// 6 : maxdist
	int maxdist;
	maxdist = hei->HspFunc_prm_getdi(0);

// 7 : posstep
	int posstep;
	posstep = hei->HspFunc_prm_getdi(0);

// 8 : dirstep100
	int dirstep100;
	dirstep100 = hei->HspFunc_prm_getdi(0);

// 9 : newposxptr
	curtype = *hei->nptype;
	int* newposxptr;
	if( curtype == 4 ){
		newposxptr = (int *)hei->HspFunc_prm_getv();
		if( !newposxptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : newposxptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : newposxptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 10 : newposyptr
	curtype = *hei->nptype;
	int* newposyptr;
	if( curtype == 4 ){
		newposyptr = (int *)hei->HspFunc_prm_getv();
		if( !newposyptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : newposyptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : newposyptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 11 : newposzptr
	curtype = *hei->nptype;
	int* newposzptr;
	if( curtype == 4 ){
		newposzptr = (int *)hei->HspFunc_prm_getv();
		if( !newposzptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : newposzptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : newposzptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 12 : newqx10000ptr
	curtype = *hei->nptype;
	int* newqxptr;
	if( curtype == 4 ){
		newqxptr = (int *)hei->HspFunc_prm_getv();
		if( !newqxptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : newqxptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : newqxptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 13 : newqy10000ptr
	curtype = *hei->nptype;
	int* newqyptr;
	if( curtype == 4 ){
		newqyptr = (int *)hei->HspFunc_prm_getv();
		if( !newqyptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : newqyptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : newqyptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 14 : newqz10000ptr
	curtype = *hei->nptype;
	int* newqzptr;
	if( curtype == 4 ){
		newqzptr = (int *)hei->HspFunc_prm_getv();
		if( !newqzptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : newqzptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : newqzptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 15 : newqw10000ptr
	curtype = *hei->nptype;
	int* newqwptr;
	if( curtype == 4 ){
		newqwptr = (int *)hei->HspFunc_prm_getv();
		if( !newqwptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : newqwptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : newqwptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 16 : tpid
	curtype = *hei->nptype;
	int* tpidptr;
	if( curtype == 4 ){
		tpidptr = (int *)hei->HspFunc_prm_getv();
		if( !tpidptr ){
			DbgOut( "e3dhsp : ControlByNaviLine : tpidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DControlByNaviLine : tpidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DControlByNaviLine : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DControlByNaviLine : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//E3DControlByNaviLine hsid, lineid, ctrlmode, roundflag, reverseflag, posstep, dirstep, &newpos, &newdirQ, &targetpointid

	D3DXVECTOR3 offset( 0.0f, 0.0f, 0.0f );

	D3DXVECTOR3 newpos;
	CQuaternion newq;

	float fdegstep;
	fdegstep = ((float)dirstep100) * 0.01f;

	CQuaternion tempdirq;
	tempdirq.CopyFromCQuaternion2( &(curhs->m_charaset.dirq) );//!!!!!!!!!

	ret = curnl->ControlByNaviLine( curhs->m_charaset.position, offset, tempdirq, 
			ctrlmode, roundflag, reverseflag,
			(float)maxdist, 
			(float)posstep, fdegstep,
			&newpos, &newq, tpidptr );
	if( ret ){
		DbgOut( "E3DControlByNaviLine : curnl ControlByNaviLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	*newposxptr	= (int)newpos.x;
	*newposyptr	= (int)newpos.y;
	*newposzptr	= (int)newpos.z;

	*newqxptr = (int)( newq.x * 10000.0f );
	*newqyptr = (int)( newq.y * 10000.0f );
	*newqzptr = (int)( newq.z * 10000.0f );
	*newqwptr = (int)( newq.w * 10000.0f );

	return 0;
}

//E3DSetDirQ hsid qx10000, qy10000, qz10000, qw10000
//	クォータニオンによる、方向のセット
//	正規化もする。
EXPORT BOOL WINAPI E3DSetDirQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

// 1 : hsid	
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

// 2 : qx10000
	int qx10000;
	qx10000 = hei->HspFunc_prm_getdi(0);

// 3 : qy10000
	int qy10000;
	qy10000 = hei->HspFunc_prm_getdi(0);

// 4 : qz10000
	int qz10000;
	qz10000 = hei->HspFunc_prm_getdi(0);

// 5 : qw10000
	int qw10000;
	qw10000 = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDirQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	float qx, qy, qz, qw;

	qx = ((float)qx10000) * 0.0001f;
	qy = ((float)qy10000) * 0.0001f;
	qz = ((float)qz10000) * 0.0001f;
	qw = ((float)qw10000) * 0.0001f;

	float savetwist = curhs->m_charaset.dirq.twist;
	ret = curhs->m_charaset.dirq.SetParams( qw, qx, qy, qz, savetwist );
	if( ret ){
		DbgOut( "E3DSetDirQ : dirq SetParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.dirq = curhs->m_charaset.dirq.normalize();
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();

	ret = curhs->SetRotate( curhs->m_charaset.dirm );
	if( ret ){
		DbgOut( "E3DSetDirQ : curhs SetRotate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




//E3DGetDirQ hsid qx10000ptr, qy10000ptr, qz10000ptr, qw10000ptr
//	姿勢を示す、クォータニオンの取得
EXPORT BOOL WINAPI E3DGetDirQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

// 1 : hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

// 2 : qx10000ptr
	curtype = *hei->nptype;
	int* qx10000ptr;
	if( curtype == 4 ){
		qx10000ptr = (int *)hei->HspFunc_prm_getv();
		if( !qx10000ptr ){
			DbgOut( "e3dhsp : GetDirQ : qx10000ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDirQ : qx10000ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 3 : qy10000ptr
	curtype = *hei->nptype;
	int* qy10000ptr;
	if( curtype == 4 ){
		qy10000ptr = (int *)hei->HspFunc_prm_getv();
		if( !qy10000ptr ){
			DbgOut( "e3dhsp : GetDirQ : qy10000ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDirQ : qy10000ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 4 : qz10000ptr
	curtype = *hei->nptype;
	int* qz10000ptr;
	if( curtype == 4 ){
		qz10000ptr = (int *)hei->HspFunc_prm_getv();
		if( !qz10000ptr ){
			DbgOut( "e3dhsp : GetDirQ : qz10000ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDirQ : qz10000ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 5 : qw10000ptr
	curtype = *hei->nptype;
	int* qw10000ptr;
	if( curtype == 4 ){
		qw10000ptr = (int *)hei->HspFunc_prm_getv();
		if( !qw10000ptr ){
			DbgOut( "e3dhsp : GetDirQ : qw10000ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDirQ : qw10000ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDirQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	*qx10000ptr = (int)(curhs->m_charaset.dirq.x * 10000.0f);
	*qy10000ptr = (int)(curhs->m_charaset.dirq.y * 10000.0f);
	*qz10000ptr = (int)(curhs->m_charaset.dirq.z * 10000.0f);
	*qw10000ptr = (int)(curhs->m_charaset.dirq.w * 10000.0f);

	return 0;
}



//E3DSetWallOnGround wallhsid, groundhsid, mapmaxy, mapminy, wallheight
EXPORT BOOL WINAPI E3DSetWallOnGround( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202


//
	int whsid;
	whsid = hei->HspFunc_prm_getdi(0);
//
	int ghsid;
	ghsid = hei->HspFunc_prm_getdi(0);
//
	int mapmaxy;
	mapmaxy = hei->HspFunc_prm_getdi(0);
//
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);
//
	int wheight;
	wheight = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	CHandlerSet* whs = GetHandlerSet( whsid );
	if( !whs ){
		DbgOut( "E3DSetWallOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* ghs = GetHandlerSet( ghsid );
	if( !ghs ){
		DbgOut( "E3DSetWallOnGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = ghs->SetWallOnGround( whs, (float)mapmaxy, (float)mapminy, (float)wheight );
	if( ret ){
		DbgOut( "E3DSetWallOnGround : ghs SetWallOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// Invalidate
	ret = whs->Invalidate();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	ret = Restore( whs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

/////
	ret = whs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DMagnetPosition hsid, dist
EXPORT BOOL WINAPI E3DMagnetPosition( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
//
	int dist;
	dist = hei->HspFunc_prm_getdi(0);
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DMagnetPosition : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->MagnetPosition( (float)dist );
	if( ret ){
		DbgOut( "E3DMagnetPosition : curhs MagnetPosition error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}


//E3DCreateNaviPointClearFlag hsid, lineno, roundnum
EXPORT BOOL WINAPI E3DCreateNaviPointClearFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: lineno
	int nlid;
	nlid = hei->HspFunc_prm_getdi(0);

//3: roundnum
	int roundnum;
	roundnum = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	if( roundnum <= 0 ){
		DbgOut( "E3DCreateNaviPointClearFlag : roundnum out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DCreateNaviPointClearFlag : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	ret = curhs->CreateNaviPointClearFlag( curnl, roundnum );
	if( ret ){
		DbgOut( "E3DCreateNaviPointClearFlag : curhs CreateNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DDestroyNaviPointClearFlag hsid
EXPORT BOOL WINAPI E3DDestroyNaviPointClearFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyNaviPointClearFlag();
	if( ret ){
		DbgOut( "E3DDestroyNaviPointClearFlag : curhs DestroyNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DInitNaviPointClearFlag hsid
EXPORT BOOL WINAPI E3DInitNaviPointClearFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DInitNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->InitNaviPointClearFlag();
	if( ret ){
		DbgOut( "E3DInitNaviPointClearFlag : curhs InitNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DSetNaviPointClearFlag hsid, srcposx, srcposy, srcposz, maxdist, &curp, &curr, &curdist
EXPORT BOOL WINAPI E3DSetNaviPointClearFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
//2: srcposx
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//3: srcposy
	int posy;
	posy = hei->HspFunc_prm_getdi(0);

//4: srcposz
	int posz;
	posz = hei->HspFunc_prm_getdi(0);

//5: maxdist
	int maxdist;
	maxdist = hei->HspFunc_prm_getdi(0);

//6: npidptr
	curtype = *hei->nptype;
	int* npidptr;
	if( curtype == 4 ){
		npidptr = (int *)hei->HspFunc_prm_getv();
		if( !npidptr ){
			DbgOut( "e3dhsp : E3DSetNaviPointClearFlag : npidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSetNaviPointClearFlag : npidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//7: roundptr
	curtype = *hei->nptype;
	int* roundptr;
	if( curtype == 4 ){
		roundptr = (int *)hei->HspFunc_prm_getv();
		if( !roundptr ){
			DbgOut( "e3dhsp : E3DSetNaviPointClearFlag : roundptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSetNaviPointClearFlag : roundptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//8: distptr
	curtype = *hei->nptype;
	int* distptr;
	if( curtype == 4 ){
		distptr = (int *)hei->HspFunc_prm_getv();
		if( !distptr ){
			DbgOut( "e3dhsp : E3DSetNaviPointClearFlag : distptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSetNaviPointClearFlag : distptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	D3DXVECTOR3 srcpos;
	srcpos.x = (float)posx;
	srcpos.y = (float)posy;
	srcpos.z = (float)posz;
	
	float fdist;

	ret = curhs->SetNaviPointClearFlag( srcpos, (float)maxdist, npidptr, roundptr, &fdist );
	if( ret ){
		DbgOut( "E3DSetNaviPointClearFlag : curhs SetNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*distptr = (int)fdist;

	return 0;
}


int SortNPCD( const VOID* arg1, const VOID* arg2 )
{
    CNPClearData** pp1 = (CNPClearData**)arg1;
    CNPClearData** pp2 = (CNPClearData**)arg2;
    
    CNPClearData* p1 = *pp1;
    CNPClearData* p2 = *pp2;


	int arno1, arno2;

	arno1 = p1->pointnum * p1->lastround + p1->lastpointarno;
	arno2 = p2->pointnum * p2->lastround + p2->lastpointarno;

	if( arno1 < arno2 ){
		return 1;
	}else if( arno1 > arno2 ){
		return -1;
	}else{
		// arno1 == arno2
		if( p1->lastdist > p2->lastdist ){
			return 1;
		}else{
			return -1;
		}
	}

    //float d1 = p1->pos.x * s_vDir.x + p1->pos.z * s_vDir.z;
    //float d2 = p2->pos.x * s_vDir.x + p2->pos.z * s_vDir.z;
    //if (d1 < d2)
    //  return +1;
    //return -1;
}


		
//E3DGetOrder hsidarray[], arrayleng, dstorder[]
EXPORT BOOL WINAPI E3DGetOrder( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

//1: hsidptr
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();
		if( !hsidptr ){
			DbgOut( "e3dhsp : E3DGetOrder : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetOrder : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//2: arrayleng
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);

//3: dstorder
	curtype = *hei->nptype;
	int* orderptr;
	if( curtype == 4 ){
		orderptr = (int *)hei->HspFunc_prm_getv();
		if( !orderptr ){
			DbgOut( "e3dhsp : E3DGetOrder : orderptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetOrder : orderptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//4: clearnoptr
	curtype = *hei->nptype;
	int* clearnoptr;
	if( curtype == 4 ){
		clearnoptr = (int *)hei->HspFunc_prm_getv();
		if( !clearnoptr ){
			DbgOut( "e3dhsp : E3DGetOrder : clearnoptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetOrder : clearnoptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	if( arrayleng >= 500 ){
		DbgOut( "E3DSetOrder : arrayleng too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* hsarray[500];
	CNPClearData* npcdarray[500];

//npcdのせっと	
	int hsno;
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		hsarray[hsno] = GetHandlerSet( *(hsidptr + hsno) );
		if( !hsarray[hsno] ){
			DbgOut( "E3DSetOrder : hsarray : hsid not found error %d !!!\n", hsarray[hsno] );
			_ASSERT( 0 );
			return 1;
		}

		npcdarray[hsno] = &(hsarray[hsno]->m_npcd);
	}

//sort
	
	qsort( npcdarray, arrayleng, sizeof( CNPClearData* ), SortNPCD );

// order, clearno
	int curhsid;
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		
		curhsid = npcdarray[hsno]->ownerhsid;

		*( orderptr + hsno ) = curhsid;
		*( clearnoptr + hsno ) = npcdarray[hsno]->pointnum * npcdarray[hsno]->lastround + npcdarray[hsno]->lastpointarno;

	}

	return 0;

}

EXPORT BOOL WINAPI E3DDestroyAllBillboard( HSPEXINFO *hei, int p2, int p3, int p4 )
{

	int ret;

	ret = DestroyTexture( -2, -1, -2 );
	_ASSERT( !ret );

///////////

	if( g_bbhs ){
		delete g_bbhs;
		g_bbhs = 0;
	}

	if( !g_bbhs ){
		g_bbhs = new CHandlerSet();
		if( !g_bbhs ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->CreateHandler();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->CreateBillboard();
		if( ret ){
			DbgOut( "E3DInit : g_bbhs CreateBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}	
		g_bbhs->m_billboardflag = 1;// !!!!! ver1036 2004/3/14
		
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSetValidFlag( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi( -1 );//!!!!

	// 3 : flag
	int flag;
	flag = hei->HspFunc_prm_getdi( 1 );//!!!!

	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetValidFlag : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetValidFlag : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	ret = curhs->SetValidFlag( partno, flag );
	if( ret ){
		DbgOut( "e3dhsp : SetValidFlag : curhs SetValidFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EXPORT BOOL WINAPI E3DSetDiffuse( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : r
	int r;
	r = hei->HspFunc_prm_getdi(0);

	// 4 : g
	int g;
	g = hei->HspFunc_prm_getdi(0);

	// 5 : b
	int b;
	b = hei->HspFunc_prm_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = hei->HspFunc_prm_getdi(0);

	//7 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetDiffuse : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetDiffuse : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	int ret;
	ret = curhs->SetDispDiffuse( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetDiffuse : curhs SetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EXPORT BOOL WINAPI E3DSetSpecular( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : r
	int r;
	r = hei->HspFunc_prm_getdi(0);

	// 4 : g
	int g;
	g = hei->HspFunc_prm_getdi(0);

	// 5 : b
	int b;
	b = hei->HspFunc_prm_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = hei->HspFunc_prm_getdi(0);


	//7 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecular : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecular : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;


	int ret;
	ret = curhs->SetDispSpecular( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecular : curhs SetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////////

	/***
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}

    D3DMATERIAL9 mtrl;
    //D3DUtil_InitMaterial( mtrl, 0.1f, 0.1f, 0.1f );
	D3DCOLORVALUE diff, amb, spec, emi;
	diff.r = 1.0f;
	diff.g = 1.0f;
	diff.b = 1.0f;
	diff.a = 1.0f;

	amb.r = 0.3f;
	amb.g = 0.3f;
	amb.b = 0.3f;
	amb.a = 1.0f;

	spec.r = r / 255.0f;
	spec.g = g / 255.0f;
	spec.b = b / 255.0f;
	spec.a = 1.0f;

	emi.r = 0.0f;
	emi.g = 0.0f;
	emi.b = 0.0f;
	emi.a = 1.0f;

	mtrl.Diffuse = diff;
	mtrl.Ambient = amb;
	mtrl.Specular = spec;
	mtrl.Emissive = emi;
	mtrl.Power = 100.0f;
    pdev->SetMaterial( &mtrl );
	***/
	
	return 0;

}

EXPORT BOOL WINAPI E3DSetAmbient( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : r
	int r;
	r = hei->HspFunc_prm_getdi(0);

	// 4 : g
	int g;
	g = hei->HspFunc_prm_getdi(0);

	// 5 : b
	int b;
	b = hei->HspFunc_prm_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = hei->HspFunc_prm_getdi(0);



	//7 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);



	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetAmbient : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetAmbient : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;


	int ret;
	ret = curhs->SetDispAmbient( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetAmbient : curhs SetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EXPORT BOOL WINAPI E3DSetBlendingMode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : bmode
	int bmode;
	bmode = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetBlendingMode : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetBlendingMode : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;

	if( curhs ){
		ret = curhs->SetBlendingMode( partno, bmode );
		if( ret ){
			DbgOut( "e3dhsp : SetBlendingMode : curhs SetBlendingMode error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		
		if( partno >= 0 ){
			CMySprite* cursprite;
			cursprite = GetSprite( partno );
			if( !cursprite ){
				DbgOut( "E3DSetBlendingMode : sprite id error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = cursprite->SetBlendingMode( bmode );
			if( ret ){
				DbgOut( "E3DSetBlendingMode : sprite SetBlendingMode error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			CMySprite* cursprite = s_spritehead;
			while( cursprite ){

				ret = cursprite->SetBlendingMode( bmode );
				if( ret ){
					DbgOut( "E3DSetBlendingMode : sprite SetBlendingMode error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				cursprite = cursprite->next;
			}

		}
	}

	
	return 0;

}

int SetPartialRenderState( LPDIRECT3DDEVICE9 pdev )
{

	HRESULT hr;
	int pno;
	int prs;
	for( pno = 0; pno < 22; pno++ ){
		prs = s_partialrs[ pno ];

		if( prs >= 0 ){
			hr = pdev->SetRenderState( (D3DRENDERSTATETYPE)prs, g_renderstate[ prs ] );
			if( hr != D3D_OK ){
				DbgOut( "e3d : SetPartialRenderState : SetRenderState : %d error !!!\r\n", prs );
			}
		}
	}

	return 0;
}


int SetDefaultRenderState()
{
	ZeroMemory( g_renderstate, sizeof( int ) * (D3DRS_BLENDOP + 1) );


	float fogdensity = 1.0f;
	float pointsize = 1.0f;
	float pointscale = 1.0f;
	float pointscaleb = 0.0f;
	float pointscalec = 0.0f;
	float numsegments = 0.0f;
	float tweenfactor = 1.0f;

	g_renderstate[D3DRS_ZENABLE] = D3DZB_TRUE;
	g_renderstate[D3DRS_FILLMODE] =	D3DFILL_SOLID;
	g_renderstate[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
	g_renderstate[D3DRS_LINEPATTERN] = 0;
	g_renderstate[D3DRS_ZWRITEENABLE] = TRUE;
	g_renderstate[D3DRS_ALPHATESTENABLE] = TRUE;
	g_renderstate[D3DRS_LASTPIXEL] = TRUE;
	//g_renderstate[D3DRS_SRCBLEND] = D3DBLEND_ONE;
	//g_renderstate[D3DRS_DESTBLEND] = D3DBLEND_ZERO ;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
g_renderstate[D3DRS_SRCBLEND] = D3DBLEND_SRCALPHA;
g_renderstate[D3DRS_DESTBLEND] = D3DBLEND_INVSRCALPHA;
	//g_renderstate[D3DRS_SRCBLEND] = D3DBLEND_ONE;
	//g_renderstate[D3DRS_DESTBLEND] = D3DBLEND_ONE;


	g_renderstate[D3DRS_CULLMODE] = D3DCULL_CCW;

	g_renderstate[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL;
	g_renderstate[D3DRS_ALPHAREF] = 0x08;
	g_renderstate[D3DRS_ALPHAFUNC] = D3DCMP_GREATEREQUAL;
	g_renderstate[D3DRS_DITHERENABLE] = FALSE;//<------------
	g_renderstate[D3DRS_ALPHABLENDENABLE] = FALSE;
	g_renderstate[D3DRS_FOGENABLE] = FALSE;
	g_renderstate[D3DRS_SPECULARENABLE] = TRUE;//<----------
	g_renderstate[D3DRS_ZVISIBLE] = 0;
	g_renderstate[D3DRS_FOGCOLOR] = 0;
	g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;

	g_renderstate[D3DRS_FOGSTART] = 0;
	g_renderstate[D3DRS_FOGEND] = 0;
	g_renderstate[D3DRS_FOGDENSITY ] = *((DWORD*)&fogdensity);
	g_renderstate[D3DRS_EDGEANTIALIAS] = FALSE;
	g_renderstate[D3DRS_ZBIAS] = 0;
	g_renderstate[D3DRS_RANGEFOGENABLE] = FALSE;
	g_renderstate[D3DRS_STENCILENABLE] = FALSE;
	g_renderstate[D3DRS_STENCILFAIL] = D3DSTENCILOP_KEEP;
	g_renderstate[D3DRS_STENCILZFAIL] = D3DSTENCILOP_KEEP;
	g_renderstate[D3DRS_STENCILPASS] = D3DSTENCILOP_KEEP;

	g_renderstate[D3DRS_STENCILFUNC] = D3DCMP_ALWAYS;
	g_renderstate[D3DRS_STENCILREF] = 0;
	g_renderstate[D3DRS_STENCILMASK] = 0xFFFFFFFF;
	g_renderstate[D3DRS_STENCILWRITEMASK] = 0xFFFFFFFF;
	g_renderstate[D3DRS_TEXTUREFACTOR] = 0xFFFFFFFF;//!!!
	g_renderstate[D3DRS_WRAP0] = 0;
	g_renderstate[D3DRS_WRAP1] = 0;
	g_renderstate[D3DRS_WRAP2] = 0;
	g_renderstate[D3DRS_WRAP3] = 0;
	g_renderstate[D3DRS_WRAP4] = 0;


	g_renderstate[D3DRS_WRAP5] = 0;
	g_renderstate[D3DRS_WRAP6] = 0;
	g_renderstate[D3DRS_WRAP7] = 0;
	g_renderstate[D3DRS_CLIPPING] = TRUE;
	//g_renderstate[D3DRS_LIGHTING] = FALSE;
	g_renderstate[D3DRS_LIGHTING] = TRUE;
	//g_renderstate[D3DRS_AMBIENT] = 0x40404040;
	//g_renderstate[D3DRS_AMBIENT] = D3DCOLOR_COLORVALUE( 0.25, 0.25, 0.25, 1.0 );
	g_renderstate[D3DRS_AMBIENT] = D3DCOLOR_COLORVALUE( 0.40, 0.40, 0.40, 1.0 );
	g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_NONE;
	g_renderstate[D3DRS_COLORVERTEX] = TRUE;
	g_renderstate[D3DRS_LOCALVIEWER] = TRUE;
	g_renderstate[D3DRS_NORMALIZENORMALS] = FALSE;

	g_renderstate[D3DRS_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
	g_renderstate[D3DRS_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
	g_renderstate[D3DRS_AMBIENTMATERIALSOURCE] = D3DMCS_COLOR2;
	g_renderstate[D3DRS_EMISSIVEMATERIALSOURCE] = D3DMCS_MATERIAL;
	g_renderstate[D3DRS_VERTEXBLEND] = D3DVBF_DISABLE;
	g_renderstate[D3DRS_CLIPPLANEENABLE] = 0;
	g_renderstate[D3DRS_SOFTWAREVERTEXPROCESSING] = FALSE;//!!!
	g_renderstate[D3DRS_POINTSIZE] = *((DWORD*)&pointsize);
	g_renderstate[D3DRS_POINTSIZE_MIN] = *((DWORD*)&pointsize);
	g_renderstate[D3DRS_POINTSPRITEENABLE] = FALSE;


	g_renderstate[D3DRS_POINTSCALEENABLE] = FALSE;
	g_renderstate[D3DRS_POINTSCALE_A] = *((DWORD*)&pointscale);
	g_renderstate[D3DRS_POINTSCALE_B] = *((DWORD*)&pointscaleb);
	g_renderstate[D3DRS_POINTSCALE_C] = *((DWORD*)&pointscalec);
	//g_renderstate[D3DRS_MULTISAMPLEANTIALIAS] = FALSE;//!!!	

	if( s_multisample > 0 ){
		g_renderstate[D3DRS_MULTISAMPLEANTIALIAS] = TRUE;
	}else{
		g_renderstate[D3DRS_MULTISAMPLEANTIALIAS] = FALSE;//!!!
	}
	
	g_renderstate[D3DRS_MULTISAMPLEMASK] = 0xFFFFFFFF;
	g_renderstate[D3DRS_PATCHEDGESTYLE] = 0;
	g_renderstate[D3DRS_PATCHSEGMENTS] = *((DWORD*)&numsegments);
	g_renderstate[D3DRS_DEBUGMONITORTOKEN] = D3DDMT_ENABLE;
	g_renderstate[D3DRS_POINTSIZE_MAX] = *((DWORD*)&pointsize);


	g_renderstate[D3DRS_INDEXEDVERTEXBLENDENABLE] = TRUE;//!!!
	g_renderstate[D3DRS_COLORWRITEENABLE] = 0x0000000F;
	g_renderstate[D3DRS_TWEENFACTOR] = *((DWORD*)&tweenfactor);
	g_renderstate[D3DRS_BLENDOP] = D3DBLENDOP_ADD;

////////


	return 0;

}

/***
static int s_partialrs[ 22 ] = {
	D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_LIGHTING, D3DRS_CULLMODE, D3DRS_SHADEMODE,
	D3DRS_WRAP0, D3DRS_WRAP1, D3DRS_DITHERENABLE, D3DRS_SPECULARENABLE, D3DRS_ZENABLE, 
	D3DRS_AMBIENT, D3DRS_NORMALIZENORMALS, D3DRS_VERTEXBLEND, D3DRS_ZWRITEENABLE, D3DRS_ZFUNC, 
	D3DRS_ALPHAREF, D3DRS_ALPHAFUNC, D3DRS_ALPHABLENDENABLE, D3DRS_MULTISAMPLEANTIALIAS, D3DRS_MULTISAMPLEMASK,
	D3DRS_BLENDOP, D3DRS_TEXTUREFACTOR
	
};
***/


EXPORT BOOL WINAPI E3DSetRenderState( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : statetype
	int statetype;
	statetype = hei->HspFunc_prm_getdi(0);

	// 4 : value
	int value;
	value = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetRenderState : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetRenderState : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	int ret;
	if( curhs ){

		ret = curhs->SetRenderState( partno, statetype, value );
		if( ret ){
			DbgOut( "e3dhsp : SetRenderState : curhs SetRenderState error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		
		CMySprite* cursprite;
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DSetRenderState : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = cursprite->SetRenderState( statetype, value );
		if( ret ){
			DbgOut( "E3DSetRenderState : sprite SetRenderState error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	
	return 0;

}

//E3DSetScale hsid, partsno, scalex, scaley, scalez, mult, centerflag
EXPORT BOOL WINAPI E3DSetScale( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : scalex
	int scalex;
	scalex = hei->HspFunc_prm_getdi(1);

	// 4 : scaley
	int scaley;
	scaley = hei->HspFunc_prm_getdi(1);

	// 5 : scalez
	int scalez;
	scalez = hei->HspFunc_prm_getdi(1);

	// 6 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

	// 7 : centerflag
	int centerflag;
	centerflag = hei->HspFunc_prm_getdi(0);



	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	D3DXVECTOR3 scalevec;
	if( mult != 0 ){
		scalevec.x = (float)scalex / (float)mult;
		scalevec.y = (float)scaley / (float)mult;
		scalevec.z = (float)scalez / (float)mult;
	}else{
		scalevec.x = 1.0f;
		scalevec.y = 1.0f;
		scalevec.z = 1.0f;
	}


	int ret;
	ret = curhs->SetScale( partno, scalevec, centerflag );
	if( ret ){
		DbgOut( "e3dhsp : SetScale : curhs SetScale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EXPORT BOOL WINAPI E3DTimeGetTime31bit( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;
	curtype = *hei->nptype;
	int* timeptr;
	if( curtype == 4 ){
		timeptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !timeptr ){
			DbgOut( "e3dhsp : TimeGetTime31bit : timeptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DTimeGetTime31bit : timeptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	DWORD curtime;
	curtime = timeGetTime();

	*timeptr = curtime & 0x7FFFFFFF;

	return 0;
}

//E3DGetScreenPos hsid, partno, &scx, &scy

EXPORT BOOL WINAPI E3DGetScreenPos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;
	
	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);
	
	//3 : scx
	curtype = *hei->nptype;
	int* scxptr;
	if( curtype == 4 ){
		scxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !scxptr ){
			DbgOut( "e3dhsp : GetScreenPos : scxptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetScreenPos : scxptr parameter type error %d!!!\n", curtype );
		return 1;
	}

	//4 : scy
	curtype = *hei->nptype;
	int* scyptr;
	if( curtype == 4 ){
		scyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !scyptr ){
			DbgOut( "e3dhsp : GetScreenPos : scyptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetScreenPos : scyptr parameter type error %d!!!\n", curtype );
		return 1;
	}


	//5 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);


	//6 : calcmode
	int calcmode;
	calcmode = hei->HspFunc_prm_getdi(0);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetScale : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	//!!!!!! モデル全体の２ｄ座標取得時は、vertnoに-1 !!!!!!!
	if( partno < 0 ){
		vertno = -1;
	}


	int ret;

	ret = curhs->GetScreenPos( partno, scxptr, scyptr, s_matView, vertno, calcmode );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPos : curhs GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetScreenPos2 x3d, y3d, z3d, x2dptr, y2dptr, validptr
EXPORT BOOL WINAPI E3DGetScreenPos2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;
	
	// 1
	int x3d;
	x3d = hei->HspFunc_prm_getdi(0);

	// 2
	int y3d;
	y3d = hei->HspFunc_prm_getdi(0);

	// 3
	int z3d;
	z3d = hei->HspFunc_prm_getdi(0);

	
	//4
	curtype = *hei->nptype;
	int* x2dptr;
	if( curtype == 4 ){
		x2dptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !x2dptr ){
			DbgOut( "e3dhsp : GetScreenPos2 : x2dptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetScreenPos2 : x2dptr parameter type error %d!!!\n", curtype );
		return 1;
	}

	//5
	curtype = *hei->nptype;
	int* y2dptr;
	if( curtype == 4 ){
		y2dptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !y2dptr ){
			DbgOut( "e3dhsp : GetScreenPos2 : y2dptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetScreenPos2 : y2dptr parameter type error %d!!!\n", curtype );
		return 1;
	}

	//6
	curtype = *hei->nptype;
	int* validptr;
	if( curtype == 4 ){
		validptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !validptr ){
			DbgOut( "e3dhsp : GetScreenPos2 : validptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetScreenPos2 : validptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////
	
	D3DXVECTOR3 vec3d;
	vec3d.x = (float)x3d;
	vec3d.y = (float)y3d;
	vec3d.z = (float)z3d;

	int ret;

	ret = GetScreenPos( vec3d, x2dptr, y2dptr, validptr );
	if( ret ){
		DbgOut( "E3DGetScreenPos2 : GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DCreateQ &qid
EXPORT BOOL WINAPI E3DCreateQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;
		
	curtype = *hei->nptype;
	int* qidptr;
	if( curtype == 4 ){
		qidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !qidptr ){
			DbgOut( "e3dhsp : CreateQ : qidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCreateQ : qidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DCreateQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_qh->CreateQ( qidptr );
	if( ret ){
		DbgOut( "E3DCreateQ : qh CreateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;	
}

//E3DDestroyQ qid
EXPORT BOOL WINAPI E3DDestroyQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DDestroyQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_qh->DestroyQ( qid );
	if( ret ){
		DbgOut( "E3DDestroyQ : qh DestroyQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DInitQ qid
EXPORT BOOL WINAPI E3DInitQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DInitQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_qh->InitQ( qid );
	if( ret ){
		DbgOut( "E3DInitQ : qh InitQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetQAxisAndDeg qid, axisx, axisy, axisz, deg, degmult
EXPORT BOOL WINAPI E3DSetQAxisAndDeg( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//1 ; qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

	//2 : axisx
	int axisx;
	axisx = hei->HspFunc_prm_getdi(0);
	
	//3 : axisy
	int axisy;
	axisy = hei->HspFunc_prm_getdi(0);

	//4 : axisz
	int axisz;
	axisz = hei->HspFunc_prm_getdi(0);

	//5 : deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

	//5 : degmult
	int degmult;
	degmult = hei->HspFunc_prm_getdi(1);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DSetQAxisAndDeg : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;
	axisvec.x = (float)axisx;
	axisvec.y = (float)axisy;
	axisvec.z = (float)axisz;

	float fdeg;
	if( degmult != 0 ){
		fdeg = (float)deg / (float)degmult;
	}else{
		fdeg = (float)deg;
	}

	int ret;
	ret = s_qh->SetQAxisAndDeg( qid, axisvec, fdeg );
	if( ret ){
		DbgOut( "E3DSetQAxisAndDeg : qh SetQAxisAndDeg error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetQAxisAndDeg qid, &axisx10000, &axisy10000, &axisy10000, &deg10000
EXPORT BOOL WINAPI E3DGetQAxisAndDeg( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;


	//1 ; qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

	//2 : axisx1e4ptr		
	curtype = *hei->nptype;
	int* axisx1e4ptr;
	if( curtype == 4 ){
		axisx1e4ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !axisx1e4ptr ){
			DbgOut( "e3dhsp : GetQAxisAndDeg : axisx1e4ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetQAxisAndDeg : axisx1e4ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

	//3 : axisy1e4ptr
	curtype = *hei->nptype;
	int* axisy1e4ptr;
	if( curtype == 4 ){
		axisy1e4ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !axisy1e4ptr ){
			DbgOut( "e3dhsp : GetQAxisAndDeg : axisy1e4ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetQAxisAndDeg : axisy1e4ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

	//4 : axisz1e4ptr
	curtype = *hei->nptype;
	int* axisz1e4ptr;
	if( curtype == 4 ){
		axisz1e4ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !axisz1e4ptr ){
			DbgOut( "e3dhsp : GetQAxisAndDeg : axisz1e4ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetQAxisAndDeg : axisz1e4ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

	//5 : deg1e4ptr
	curtype = *hei->nptype;
	int* deg1e4ptr;
	if( curtype == 4 ){
		deg1e4ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !deg1e4ptr ){
			DbgOut( "e3dhsp : GetQAxisAndDeg : deg1e4ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetQAxisAndDeg : deg1e4ptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DGetQAxisAndDeg : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;
	float deg;

	int ret;
	ret = s_qh->GetQAxisAndDeg( qid, &axisvec, &deg );
	if( ret ){
		DbgOut( "E3DGetQAxisAndDeg : qh GetQAxisAndDeg error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//!!! axis * 10000
	*axisx1e4ptr = (int)( axisvec.x * 10000.0f );
	*axisy1e4ptr = (int)( axisvec.y * 10000.0f );
	*axisz1e4ptr = (int)( axisvec.z * 10000.0f );

	//!!! deg * 10000
	*deg1e4ptr = (int)( deg * 10000.0f );

	return 0;
}


//E3DRotateQX qid, degx, mult
EXPORT BOOL WINAPI E3DRotateQX( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//1 ; qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

	//2 : degx
	int degx;
	degx = hei->HspFunc_prm_getdi(0);

	//3 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DRotateQX : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;
	float fdeg;

	axisvec.x = 1.0f;
	axisvec.y = 0.0f;
	axisvec.z = 0.0f;

	if( mult != 0 ){
		fdeg = (float)degx / (float)mult;
	}else{
		fdeg = (float)degx;
	}

	int ret;

	ret = s_qh->RotateQ( qid, axisvec, fdeg );
	if( ret ){
		DbgOut( "E3DRotateQX : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DRotateQY qid, degy, mult
EXPORT BOOL WINAPI E3DRotateQY( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//1 ; qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

	//2 : degx
	int degy;
	degy = hei->HspFunc_prm_getdi(0);

	//3 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DRotateQY : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;
	float fdeg;

	axisvec.x = 0.0f;
	axisvec.y = 1.0f;
	axisvec.z = 0.0f;

	if( mult != 0 ){
		fdeg = (float)degy / (float)mult;
	}else{
		fdeg = (float)degy;
	}

	int ret;

	ret = s_qh->RotateQ( qid, axisvec, fdeg );
	if( ret ){
		DbgOut( "E3DRotateQY : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
//E3DRotateQZ qid, degz, mult
EXPORT BOOL WINAPI E3DRotateQZ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//1 ; qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

	//2 : degz
	int degz;
	degz = hei->HspFunc_prm_getdi(0);

	//3 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DRotateQZ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;
	float fdeg;

	axisvec.x = 0.0f;
	axisvec.y = 0.0f;
	axisvec.z = 1.0f;

	if( mult != 0 ){
		fdeg = (float)degz / (float)mult;
	}else{
		fdeg = (float)degz;
	}

	int ret;

	ret = s_qh->RotateQ( qid, axisvec, fdeg );
	if( ret ){
		DbgOut( "E3DRotateQZ : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DMultQ resultqid, befqid, aftqid
EXPORT BOOL WINAPI E3DMultQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//1 ; resqid
	int resqid;
	resqid = hei->HspFunc_prm_getdi(0);

	//2 ; befqid
	int befqid;
	befqid = hei->HspFunc_prm_getdi(0);

	//3 ; aftqid
	int aftqid;
	aftqid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DMultQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_qh->MultQ( resqid, befqid, aftqid );
	if( ret ){
		DbgOut( "E3DMultQ : qh MultQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DNormalizeQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//1 ; qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DNormalizeQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_qh->NormalizeQ( qid );
	if( ret ){
		DbgOut( "E3DNormalizeQ : qh NormalizeQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DCopyQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//1 ; dstqid
	int dstqid;
	dstqid = hei->HspFunc_prm_getdi(0);

	//2 ; srcqid
	int srcqid;
	srcqid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	if( !s_qh ){
		DbgOut( "E3DCopyQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_qh->CopyQ( dstqid, srcqid );
	if( ret ){
		DbgOut( "E3DCopyQ : qh CopyQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetBoneNoByName hsid, name, &boneno
	//該当しない場合は　-1
//!!! エンドジョイントは、操作不能にするために、取得できないようにする。

EXPORT BOOL WINAPI E3DGetBoneNoByName( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	char tempname[MAX_PATH];
	char* nameptr;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DGetBoneNoByName : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DGetBoneNoByName : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : GetBoneNoByName : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );
//3

	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBoneNoByName : parameter type error !!!\n" );
		return 1;
	}
	int* bonenoptr;
	bonenoptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : GetBoneNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetBoneNoByName( tempname, bonenoptr );
	if( ret ){
		DbgOut( "E3DGetBoneNoByName : curhs GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetNextMP hsid, motionid, boneno, prevmpid, &mpid
	//prevmpid == -1 の時は、最初のmp
	//prevmpid >= 0	で、そのmpが存在しない場合は、エラー。
	//無い場合は　-1
EXPORT BOOL WINAPI E3DGetNextMP( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

//4: prevmpid
	int prevmpid;
	prevmpid = hei->HspFunc_prm_getdi(0);

//5: mpidptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetNextMP : parameter type error !!!\n" );
		return 1;
	}
	int* mpidptr;
	mpidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : GetNextMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetNextMP( motid, boneno, prevmpid, mpidptr );
	if( ret ){
		DbgOut( "e3dhsp : GetNextMP : curhs GetNextMP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetMPInfo hsid, motionid, boneno, mpid, infoptr
	//infoptr.0 : qid(呼び出し前に準備)
	//infoptr.1 : trax
	//infoptr.2 : tray
	//infoptr.3 : traz
	//infoptr.4 : frameno
	//infoptr.5 : displayswitch
	//infoptr.6 : interpolation
	
	//infoptr.7 : scalex
	//infoptr.8 : scaley
	//infoptr.9 : scalez
	//infoptr.10: userint1

EXPORT BOOL WINAPI E3DGetMPInfo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

//4: mpid
	int mpid;
	mpid = hei->HspFunc_prm_getdi(0);

//5: infoptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetMPInfo : parameter type error !!!\n" );
		return 1;
	}
	int* infoptr;
	infoptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetMPInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion dstq;
	D3DXVECTOR3 dsttra;
	int dstframeno;
	DWORD dstds;
	int dstinterp;

	D3DXVECTOR3 dstsc;
	int dstuserint1;

	ret = curhs->GetMPInfo( motid, boneno, mpid, &dstq, &dsttra, &dstframeno, &dstds, &dstinterp, &dstsc, &dstuserint1 );
	//ret = curhs->GetMPInfo( motid, boneno, mpid, &dstq, &dsttra, &dstframeno, &dstds, &dstinterp );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetMPInfo : curhs GetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_qh ){
		DbgOut( "E3DGetMPInfo : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2 setq;
	setq.CopyFromCQuaternion( &dstq );//!!!!!!!!

	ret = s_qh->SetQ( *(infoptr + MPI_QUA), &setq );
	if( ret ){
		DbgOut( "E3DGetMPInfo : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "E3DGetMPInfo : %f %f %f\r\n", dstsc.x, dstsc.y, dstsc.z );

	*(infoptr + MPI_TRAX) = (int)dsttra.x;
	*(infoptr + MPI_TRAY) = (int)dsttra.y;
	*(infoptr + MPI_TRAZ) = (int)dsttra.z;
	*(infoptr + MPI_FRAMENO) = dstframeno;
	*(infoptr + MPI_DISPSWITCH) = *((int*)(&dstds));
	*(infoptr + MPI_INTERP) = dstinterp;

	*(infoptr + MPI_SCALEX) = (int)(dstsc.x * 1000.0f);
	*(infoptr + MPI_SCALEY) = (int)(dstsc.y * 1000.0f);
	*(infoptr + MPI_SCALEZ) = (int)(dstsc.z * 1000.0f);
	*(infoptr + MPI_USERINT1) = dstuserint1;
	*(infoptr + MPI_SCALEDIV) = 1000;//!!!!!!!!!!!!
	
	return 0;
}



//E3DSetMPInfo hsid, motionid, boneno, mpid, infoptr, infoflagptr ----> 適切なタイミングで、E3DFillUpMP必要。
	//infoptr.0 : qid(呼び出し前に準備)
	//infoptr.1 : trax
	//infoptr.2 : tray
	//infoptr.3 : traz
	//infoptr.4 : frameno
	//infoptr.5 : displayswitch
	//infoptr.6 : interpolation

	//infoflagptr.0 が１のときは、infoptr.0 のqidを有効に
	//infoflagptr.1 が１のときは、infoptr.1のtraxを有効に
	//....


	//!!!! frameno が不適切な値でないかチェックも！！！！
	// 既にframenoにモーションポイントがある場合にも、エラー

EXPORT BOOL WINAPI E3DSetMPInfo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

//4: mpid
	int mpid;
	mpid = hei->HspFunc_prm_getdi(0);

//5: infoptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSetMPInfo : parameter type error !!!\n" );
		return 1;
	}
	int* infoptr;
	infoptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//6: infoflagptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSetMPInfo : parameter type error !!!\n" );
		return 1;
	}
	int* infoflagptr;
	infoflagptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////


	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetMPInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	D3DXVECTOR3 srctra;
	int srcframeno;
	DWORD srcds;
	int srcinterp;

	D3DXVECTOR3 srcscale;
	int srcuserint1;

	if( !s_qh ){
		DbgOut( "E3DSetMPInfo : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( *infoptr, &srcqptr );
	if( ret ){
		DbgOut( "E3DSetMPInfo : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float scalediv;

	if( *(infoflagptr + MPI_SCALEDIV) != 0 ){
		scalediv = (float)(*(infoptr + MPI_SCALEDIV));
		if( scalediv == 0.0f )
			scalediv = 1.0f;
	}else{
		scalediv = 1.0f;
	}

	srctra.x = (float)(*(infoptr + MPI_TRAX));
	srctra.y = (float)(*(infoptr + MPI_TRAY));
	srctra.z = (float)(*(infoptr + MPI_TRAZ));
	srcframeno = *(infoptr + MPI_FRAMENO);
	srcds = *((DWORD*)(infoptr + MPI_DISPSWITCH));
	srcinterp = *(infoptr + MPI_INTERP);

	srcscale.x = (float)(*(infoptr + MPI_SCALEX)) / scalediv;
	srcscale.y = (float)(*(infoptr + MPI_SCALEY)) / scalediv;
	srcscale.z = (float)(*(infoptr + MPI_SCALEZ)) / scalediv;
	srcuserint1 = *(infoptr + MPI_USERINT1);


//DbgOut( "E3DSetMPInfo : %f, %f, %f, %f : %d %d %d %d\r\n", srcscale.x, srcscale.y, srcscale.z, scalediv, *(infoflagptr + MPI_SCALEX), *(infoflagptr + MPI_SCALEY), *(infoflagptr + MPI_SCALEZ), *(infoflagptr + MPI_SCALEDIV));

	CQuaternion setq;
	setq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!

	ret = curhs->SetMPInfo( motid, boneno, mpid, &setq, &srctra, srcframeno, srcds, srcinterp, &srcscale, srcuserint1, infoflagptr );
	//ret = curhs->SetMPInfo( motid, boneno, mpid, &setq, &srctra, srcframeno, srcds, srcinterp, infoflagptr );
	if( ret ){
		DbgOut( "e3dhsp : E3DSetMPInfo : curhs SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DIsExistMP hsid, motionid, boneno, frameno, &mpid
		//framenoに、既にモーションポイントがあれば、そのidを代入する。
		//無いときは、mpid に　-1を代入する
EXPORT BOOL WINAPI E3DIsExistMP( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

//4: frameno
	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);

//5: mpidptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIsExistMP : parameter type error !!!\n" );
		return 1;
	}
	int* mpidptr;
	mpidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DIsExistMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->IsExistMP( motid, boneno, frameno, mpidptr );
	if( ret ){
		DbgOut( "e3dhsp : E3DIsExistMP : curhs SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetMotionFrameLength hsid, motionid, &frameleng

		//フレーム番号の最大値は、leng - 1なので注意

EXPORT BOOL WINAPI E3DGetMotionFrameLength( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: lengptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetMotionFrameLength : parameter type error !!!\n" );
		return 1;
	}
	int* lengptr;
	lengptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetMotionFrameLength : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMotionFrameLength( motid, lengptr );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetMotionFrameLength : curhs GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetMotionFrameLength hsid, motionid, frameleng, initflag ----> 適切なタイミングで、E3DFillUpMP必要。
	//フレーム数を縮小する場合は、範囲外になるフレーム番号を持つモーションポイントは、削除される。
EXPORT BOOL WINAPI E3DSetMotionFrameLength( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: frameleng
	int frameleng;
	frameleng = hei->HspFunc_prm_getdi(0);

//4; initflag
	int initflag;
	initflag = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetMotionFrameLength : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionFrameLength( motid, frameleng, initflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DSetMotionFrameLength : curhs SetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DAddMP hsid, motionid, boneno, infoptr, &mpid ----> 適切なタイミングで、E3DFillUpMP必要。
		//既にフレーム番号にモーションポイントがあった場合は、エラー
EXPORT BOOL WINAPI E3DAddMP( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;
//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

//4: infoptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMP : parameter type error !!!\n" );
		return 1;
	}
	int* infoptr;
	infoptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//5: mpidptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMP : parameter type error !!!\n" );
		return 1;
	}
	int* mpidptr;
	mpidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DAddMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	D3DXVECTOR3 srctra;
	int srcframeno;
	DWORD srcds;
	int srcinterp;

	D3DXVECTOR3 srcsc;
	int srcuserint1;

	if( !s_qh ){
		DbgOut( "E3DAddMP : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( *infoptr, &srcqptr );
	if( ret ){
		DbgOut( "E3DAddMP : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	srctra.x = (float)(*(infoptr + 1));
	srctra.y = (float)(*(infoptr + 2));
	srctra.z = (float)(*(infoptr + 3));
	srcframeno = *(infoptr + 4);
	srcds = *((DWORD*)(infoptr + 5));
	srcinterp = *(infoptr + 6);

	srcsc.x = (float)(*(infoptr + 7));
	srcsc.y = (float)(*(infoptr + 8));
	srcsc.z = (float)(*(infoptr + 9));
	srcuserint1 = *(infoptr + 10);

	CQuaternion addq;
	addq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!!

	//ret = curhs->AddMotionPoint( motid, boneno, &addq, &(infoptr->tra), infoptr->frameno, infoptr->dispswitch, infoptr->interpolation, &(infoptr->scale), infoptr->userint1, mpidptr );

	ret = curhs->AddMotionPoint( motid, boneno, &addq, &srctra, srcframeno, srcds, srcinterp, &srcsc, srcuserint1, mpidptr );
	if( ret ){
		DbgOut( "e3dhsp : E3DAddMP : curhs AddMotioinPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DDeleteMP hsid, motionid, boneno, mpid ----> 適切なタイミングで、E3DFillUpMP必要。
EXPORT BOOL WINAPI E3DDeleteMP( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;
//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

//4: mpid
	int mpid;
	mpid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DDeleteMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DeleteMotionPoint( motid, boneno, mpid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDeleteMP : curhs DeleteMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DFillUpMP hsid, motionid, boneno, startmp, endmp, initflag
		//boneno指定の場合は、boneno以下のtreeのみ更新。
		//boneno == -1のときは、全てのボーンを更新。
		//endmp == -1 のときは、最後のフレームまでを計算。
		//initflag == 1 ---> あたり判定情報を更新
EXPORT BOOL WINAPI E3DFillUpMotion( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;
//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3: boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

//4: startframe
	int startframe;
	startframe = hei->HspFunc_prm_getdi(0);

//5: endframe
	int endframe;
	endframe = hei->HspFunc_prm_getdi(0);

//6: initflag
	int initflag;
	initflag = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DFillUpMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->FillUpMotion( motid, boneno, startframe, endframe, initflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DFillUpMotion : curhs E3DFillUpMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCopyMotionFrame hsid, srcmotid, srcframeno, dstmotid, dstframeno
		// E3DFillUpMotionしないと、結果は反映されない。
		//srcframeに、モーションポイントが無い場合は、補間計算が生じる
		//dstframe側に、モーションポイントが無い場合は、新規に作成する。
EXPORT BOOL WINAPI E3DCopyMotionFrame( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;
//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: srcmotid
	int srcmotid;
	srcmotid = hei->HspFunc_prm_getdi(0);

//3: srcframe
	int srcframe;
	srcframe = hei->HspFunc_prm_getdi(0);

//4: dstmotid
	int dstmotid;
	dstmotid = hei->HspFunc_prm_getdi(0);

//5: dstframe
	int dstframe;
	dstframe = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DCopyMotionFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->CopyMotionFrame( srcmotid, srcframe, dstmotid, dstframe );
	if( ret ){
		DbgOut( "e3dhsp : E3DCopyMotionFrame : curhs CopyMotionFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//	E3DGetDirQ2 hsid, dstqid
EXPORT BOOL WINAPI E3DGetDirQ2( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;
//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: dstqid
	int dstqid;
	dstqid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetDirQ2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_qh ){
		DbgOut( "E3DGetDirQ2 : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->SetQ( dstqid, &(curhs->m_charaset.dirq) );
	if( ret ){
		DbgOut( "E3DGetDirQ2 : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//	E3DSetDirQ2 hsid, srcqid
EXPORT BOOL WINAPI E3DSetDirQ2( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;
//1: hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2: srcqid
	int srcqid;
	srcqid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetDirQ2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_qh ){
		DbgOut( "E3DSetDirQ2 : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( srcqid, &srcqptr );
	if( ret ){
		DbgOut( "E3DSetDirQ2 : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !srcqptr ){
		DbgOut( "E3DSetDirQ2 : srcqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->m_charaset.dirq.SetParams( srcqptr->w, srcqptr->x, srcqptr->y, srcqptr->z, srcqptr->twist );
	if( ret ){
		DbgOut( "E3DSetDirQ2 : dirq SetParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_charaset.dirq = curhs->m_charaset.dirq.normalize();
	curhs->m_charaset.dirm = curhs->m_charaset.dirq.MakeRotMatX();

	ret = curhs->SetRotate( curhs->m_charaset.dirm );
	if( ret ){
		DbgOut( "E3DSetDirQ2 : curhs SetRotate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DLookAtQ qid, vecx, vecy, vecz, basex, basey, basez, upflag, divnum

	// ！！！！　qidには、一回前の情報が入っていることを前提とする。

	//upflag == 0 --> 上向き
	//upflag == 1 --> 下向き
	//upflag == 2 --> 自動制御、宙返りモード
	//upflag == 3 --> 上向き保持なしモード

EXPORT BOOL WINAPI E3DLookAtQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

//1: dstqid
	int dstqid;
	dstqid = hei->HspFunc_prm_getdi(0);

//2: vecx
	int vecx;
	vecx = hei->HspFunc_prm_getdi(0);

//3: vecx
	int vecy;
	vecy = hei->HspFunc_prm_getdi(0);

//4: vecz
	int vecz;
	vecz = hei->HspFunc_prm_getdi(1);

//5: basevecx
	int basevecx;
	basevecx = hei->HspFunc_prm_getdi(0);

//6: basevecx
	int basevecy;
	basevecy = hei->HspFunc_prm_getdi(0);

//7: basevecz
	int basevecz;
	basevecz = hei->HspFunc_prm_getdi(-1);


//8: upflag
	int upflag;
	upflag = hei->HspFunc_prm_getdi(2);

//9: divnum
	int divnum;
	divnum = hei->HspFunc_prm_getdi(1);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////


	int ret;

	if( !s_qh ){
		DbgOut( "E3DLookAtQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* dstqptr;
	ret = s_qh->GetQ( dstqid, &dstqptr );
	if( ret ){
		DbgOut( "E3DLookAtQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !dstqptr ){
		DbgOut( "E3DLookAtQ : dstqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//DbgOut( "e3dhsp : E3DLookAtQ : qid %d, savetwist %f\n", dstqid, savetwist );

	D3DXVECTOR3 basevec;
	basevec.x = (float)basevecx;
	basevec.y = (float)basevecy;
	basevec.z = (float)basevecz;
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}
	D3DXVECTOR3 tarvec;
	tarvec.x = (float)vecx;
	tarvec.y = (float)vecy;
	tarvec.z = (float)vecz;
	DXVec3Normalize( &tarvec, &tarvec );


	ret = LookAtQ( dstqptr, tarvec, basevec, divnum, upflag );
	if( ret ){
		DbgOut( "E3DLookAtQ : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}

int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag )
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
			DbgOut( "e3dhsp : LookAtQ : CheckSault error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		// tarvec Ｙシフト
		int shiftflag = 0;
		D3DXVECTOR3 shifttarvec;
		ret = dstqptr->ShiftVecNearY( basevec, tarvec, &shiftflag, &shifttarvec );
		if( ret ){
			DbgOut( "e3dhsp : LookAtQ : ShiftVecNearY error !!!\n" );
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
			DbgOut( "e3dhsp : LookAtQ : ShiftVecNearY  2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( shiftflag2 == 1 )
			toppos = shifttarvec2;

//DbgOut( "e3dhsp : E3DLookAtQ : saultflag %d, shift tarvec %f %f %f, toppos %f %f %f\n",
//	   saultflag, tarvec.x, tarvec.y, tarvec.z, toppos.x, toppos.y, toppos.z );


		if( (saultflag == 0) || (saultflag == 2) ){

			// 移動角度が大きい場合は、targetを近くに設定し直す。分割数も、設定し直す。

			// 直接、tarvecを目指して、回転する。

			
			
			ret = dstqptr->GetDividedTarget( basevec, tarvec, tarvec, divnum, &newtarget, &newdivnum );
			if( ret ){
				DbgOut( "e3dhsp : LookAtQ : GetDividedTarget error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			tarvec = newtarget;
			divnum = newdivnum;
			
			if( tarvec == toppos ){
				_ASSERT( 0 );
			}


//DbgOut( "e3dhsp : E3DLookAtQ : saultflag %d, tarvec %f %f %f, divnum %d\n",
//	   saultflag, tarvec.x, tarvec.y, tarvec.z, divnum );


		}else if( saultflag == 1 ){
			
				
			// 移動角度が大きい場合は、targetを近くに設定し直す。分割数も、設定し直す。

			// まず、topposをめざして、回転する。

			ret = dstqptr->GetDividedTarget( basevec, toppos, tarvec, divnum, &newtarget, &newdivnum );
			if( ret ){
				DbgOut( "e3dhsp : LookAtQ : GetDividedTarget error !!!\n" );
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
		
	//DbgOut( "e3dhsp : LookAtQ : saultflag %d\n", saultflag );
	//DbgOut( "e3dhsp : LookAtQ : 0 : divnum %d\n", divnum );

	// targetへの回転処理
	CQuaternion2 finalq;
	CQuaternion2 rot1, rot2;
	D3DXVECTOR3 upvec;


	CQuaternion2 saveq;
	saveq = *dstqptr;


	finalq = *dstqptr;

	ret = finalq.LookAt( tarvec, basevec, 10000.0f, upflag, 0, 0, &upvec, &rot1, &rot2 );
	if( ret ){
		DbgOut( "e3dhsp : LookAtQ : fianlq LookAt error !!!\n" );
	}

	//
	//if( *dstqptr != finalq ){
	//	ret = finalq.GradUp( *dstqptr, basevec, upvec, rot1, rot2 );
	//	if( ret ){
	//		DbgOut( "e3dhsp : E3DLookAtQ : GradUp error !!!\n" );
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//}
	



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

		//if( finalvec == trabase ){
		//	DbgOut( "e3dhsp : E3DLookAtQ : not move warning %f %f %f!!!\n", finalvec.x, finalvec.y, finalvec.z );
		//}
		
		//DbgOut( "e3dhsp : E3DLookAtQ : saultflag %d, tarvec %f %f %f, newtarget %f %f %f\n", saultflag, tarvec.x, tarvec.y, tarvec.z, newtarget.x, newtarget.y, newtarget.z );
	}


	dstqptr->twist = savetwist;


	//DbgOut( "e3dhsp : E3DLookAtQ : upflag %d, divnum %d\n", upflag, divnum );


	//*dstqptr = finalq;

	//CQuaternion2 firstq;
	//firstq = *dstqptr;
	//*dstqptr = firstq.Slerp( finalq, divnum, 1 );

	dstqptr->twist = savetwist;

	return 0;
}



//E3DMultQVec qid, befvecx, befvecy, befvecz, aftvecx, aftvecy, aftvecz
	// 初期状態のベクトルをbefvecに渡せば、現在向いている向きベクトルを求めることも出来ます。

EXPORT BOOL WINAPI E3DMultQVec( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

	int curtype;

//1: qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//2: befvecx
	int befvecx;
	befvecx = hei->HspFunc_prm_getdi(0);

//3: befvecy
	int befvecy;
	befvecy = hei->HspFunc_prm_getdi(0);

//4: befvecz
	int befvecz;
	befvecz = hei->HspFunc_prm_getdi(0);


//5: aftvecx 
	int* aftvecx;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DMultQVec : parameter type error !!!\n" );
		return 1;
	}
	aftvecx = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

//6: aftvecy 
	int* aftvecy;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DMultQVec : parameter type error !!!\n" );
		return 1;
	}
	aftvecy = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

//7: aftvecz 
	int* aftvecz;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DMultQVec : parameter type error !!!\n" );
		return 1;
	}
	aftvecz = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DMultQVec : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DMultQVec : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DMultQVec : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 befvec, aftvec;
	befvec.x = (float)befvecx;
	befvec.y = (float)befvecy;
	befvec.z = (float)befvecz;

	qptr->Rotate( &aftvec, befvec );

	*aftvecx = (int)aftvec.x;
	*aftvecy = (int)aftvec.y;
	*aftvecz = (int)aftvec.z;

	return 0;
}


//E3DTwistQ qid, twistdeg, degmult, basex, basey, basez
EXPORT BOOL WINAPI E3DTwistQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//2: twistdeg
	int twistdeg;
	twistdeg = hei->HspFunc_prm_getdi(0);

//3: degmult
	int degmult;
	degmult = hei->HspFunc_prm_getdi(0);

//4: basex
	int basex;
	basex = hei->HspFunc_prm_getdi(0);

//5: basey
	int basey;
	basey = hei->HspFunc_prm_getdi(0);

//6: basez
	int basez;
	basez = hei->HspFunc_prm_getdi(0);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DTwistQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DTwistQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DTwistQ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float ftwist;
	if( degmult != 0 ){
		ftwist = (float)twistdeg / (float)degmult;
	}else{
		ftwist = 0.0f;
	}
	
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 axis;

	basevec.x = (float)basex;
	basevec.y = (float)basey;
	basevec.z = (float)basez;
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}
	D3DXVECTOR3 upvec( 0.0f, 1.0f, 0.0f );
	CQuaternion2 basecheckq;
	basecheckq.RotationArc( basevec, upvec );
	D3DXVECTOR3 chkbaseaxis;
	float chkbaserad;
	basecheckq.GetAxisAndRot( &chkbaseaxis, &chkbaserad );
	if( fabs( chkbaserad ) < 0.1f * (float)DEG2PAI ){
		D3DXVECTOR3 chgaxis( 1.0f, 0.0f, 0.0f );
		CQuaternion2 chgq;
		chgq.SetAxisAndRot( chgaxis, 0.1f * (float)DEG2PAI );
		chgq.Rotate( &basevec, basevec );

		DXVec3Normalize( &basevec, &basevec );
	}



	qptr->Rotate( &axis, basevec );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 twistq;
	twistq.SetAxisAndRot( axis, ftwist * (float)DEG2PAI );

	float savetwist;
	savetwist = qptr->twist;

	*qptr = twistq * *qptr;
	//qptr->twist = savetwist + ftwist;


	// 0 - 360
	float finaltwist, settwist;
	finaltwist = savetwist + (float)ftwist;

	if( finaltwist >= 360.0f ){
		while( finaltwist >= 360.0f )
			finaltwist -= 360.0f;
		settwist = finaltwist;
	}else if( finaltwist < 0.0f ){
		while( finaltwist < 0.0f )
			finaltwist += 360.0f;
		settwist = finaltwist;
	}else{
		settwist = finaltwist;
	}


	qptr->twist = settwist;


	//DbgOut( "e3dhsp : E3DTwistQ : ftwist %f, qptr->twist %f\n", ftwist, qptr->twist );
		return 0;
}


//E3DInitTwistQ qid, basex, basey, basez
EXPORT BOOL WINAPI E3DInitTwistQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//2: basex
	int basex;
	basex = hei->HspFunc_prm_getdi(0);

//3: basey
	int basey;
	basey = hei->HspFunc_prm_getdi(0);

//4: basez
	int basez;
	basez = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DInitTwistQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DInitTwistQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DInitTwistQ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 axis;

	basevec.x = (float)basex;
	basevec.y = (float)basey;
	basevec.z = (float)basez;
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}
	D3DXVECTOR3 upvec( 0.0f, 1.0f, 0.0f );
	CQuaternion2 basecheckq;
	basecheckq.RotationArc( basevec, upvec );
	D3DXVECTOR3 chkbaseaxis;
	float chkbaserad;
	basecheckq.GetAxisAndRot( &chkbaseaxis, &chkbaserad );
	if( fabs( chkbaserad ) < 0.1f * (float)DEG2PAI ){
		D3DXVECTOR3 chgaxis( 1.0f, 0.0f, 0.0f );
		CQuaternion2 chgq;
		chgq.SetAxisAndRot( chgaxis, 0.1f * (float)DEG2PAI );
		chgq.Rotate( &basevec, basevec );

		DXVec3Normalize( &basevec, &basevec );
	}


	qptr->Rotate( &axis, basevec );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 twistq;
	float invtwist;
	invtwist = qptr->twist * (float)DEG2PAI * -1.0f;
	twistq.SetAxisAndRot( axis, invtwist );

//DbgOut( "e3dhsp : E3DInitTwistQ : qid %d, axis %f %f %f, rad of invtwist %f twist %f\n", qid, axis.x, axis.y, axis.z, invtwist, qptr->twist );

	*qptr = twistq * *qptr;
	
	qptr->twist = 0.0f;

//DbgOut( "e3dhsp : E3DInitTwistQ : afttwist %f\n", qptr->twist );

	return 0;
}


//E3DGetTwistQ qid, twist10000
EXPORT BOOL WINAPI E3DGetTwistQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);


//2: twist10000 
	int curtype;
	int* twist10000;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetTwistQ : parameter type error !!!\n" );
		return 1;
	}
	twist10000 = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DGetTwistQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DGetTwistQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DGetTwistQ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*twist10000 = (int)(qptr->twist * 10000.0f); 

	return 0;
}

//E3DRotateQLocalX qid, deg, degmult
EXPORT BOOL WINAPI E3DRotateQLocalX( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//2: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

//3: degmult
	int degmult;
	degmult = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DRotateQLocalX : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DRotateQLocalX : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DRotateQLocalX : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		
	float fdeg;
	if( degmult != 0 ){
		fdeg = (float)deg / (float)degmult;
	}else{
		fdeg = (float)deg;
	}


	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 1.0f, 0.0f, 0.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, fdeg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}


//E3DRotateQLocalY qid, deg, degmult
EXPORT BOOL WINAPI E3DRotateQLocalY( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//2: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

//3: degmult
	int degmult;
	degmult = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DRotateQLocalY : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DRotateQLocalY : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DRotateQLocalY : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		
	float fdeg;
	if( degmult != 0 ){
		fdeg = (float)deg / (float)degmult;
	}else{
		fdeg = (float)deg;
	}


	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 0.0f, 1.0f, 0.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, fdeg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}



//E3DRotateQLocalZ qid, deg, degmult
EXPORT BOOL WINAPI E3DRotateQLocalZ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1: qid
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

//2: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

//3: degmult
	int degmult;
	degmult = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	int ret;

	if( !s_qh ){
		DbgOut( "E3DRotateQLocalZ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DRotateQLocalZ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DRotateQLocalZ : qid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

		
	float fdeg;
	if( degmult != 0 ){
		fdeg = (float)deg / (float)degmult;
	}else{
		fdeg = (float)deg;
	}


	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 0.0f, 0.0f, 1.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, fdeg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}

//E3DGetBonePos hsid, boneno, poskind, motid, frameno, posx, posy, posz, scaleflag
//	local座標とグローバル座標の2種類
EXPORT BOOL WINAPI E3DGetBonePos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id
	// 2 : boneno
	// 3 : poskind
	// 4 : motid
	// 5 : frameno

	// 6 : posxptr
	// 7 : posyptr
	// 8 : poszptr

	// 9 : scaleflag

	int curtype;
	
	
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

	int poskind;
	poskind = hei->HspFunc_prm_getdi(0);

	int motid;
	motid = hei->HspFunc_prm_getdi(0);

	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);

	int* posxptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBonePos : parameter type error !!!\n" );
		return 1;
	}
	posxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	int* posyptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBonePos : parameter type error !!!\n" );
		return 1;
	}
	posyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	int* poszptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBonePos : parameter type error !!!\n" );
		return 1;
	}
	poszptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック

	
	int scaleflag;
	scaleflag = hei->HspFunc_prm_getdi(0);


///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetBonePos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 dstpos;

	ret = curhs->GetBonePos( boneno, poskind, motid, frameno, scaleflag, &dstpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetBonePos : curhs GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posxptr = (int)dstpos.x;
	*posyptr = (int)dstpos.y;
	*poszptr = (int)dstpos.z;


	return 0;

}


int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec )
{
	float mag;
	mag = srcvec->x * srcvec->x + srcvec->y * srcvec->y + srcvec->z * srcvec->z;

	float sqmag;
	sqmag = (float)sqrt( mag );

	if( sqmag != 0.0f ){
		float magdiv;
		magdiv = 1.0f / sqmag;
		dstvec->x *= magdiv;
		dstvec->y *= magdiv;
		dstvec->z *= magdiv;

	}else{
		DbgOut( "q2 : DXVec3Normalize : zero warning !!!\n" );
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
		_ASSERT( 0 );
	}

	return 0;
}

//E3DCreateLine pointarray, pointnum, maxpointnum, linekind, &lineid
		//p[pointnum][3]
		//videomemoryは、maxpointnumでアロケートする
		//linekind --> LINELIST = 0, LINESTRIP = 1
EXPORT BOOL WINAPI E3DCreateLine( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int curtype;

	int* posptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCreateLine : parameter type error !!!\n" );
		return 1;
	}
	posptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
	

	int pointnum;
	pointnum = hei->HspFunc_prm_getdi(0);

	int maxpointnum;
	maxpointnum = hei->HspFunc_prm_getdi(0);

	int linekind;
	linekind = hei->HspFunc_prm_getdi(0);


	int* lineidptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DCreateLine : parameter type error !!!\n" );
		return 1;
	}
	lineidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////

	if( (linekind != 2) && (linekind != 3) ){
		DbgOut( "e3dhsp : E3DCreateLine : invalid linekind warning !!! : set default value \n" );
		_ASSERT( 0 );

		linekind = 3;//!!!!!!!
	}

	
	if( pointnum < 2 ){
		DbgOut( "e3dhsp : E3DCreateLine : pointnum too small error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !posptr ){
		DbgOut( "e3dhsp : E3DCreateLine : posptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( pointnum < 2 ){
		DbgOut( "e3dhsp : E3DCreateLine : pointnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( maxpointnum < pointnum ){
		maxpointnum = pointnum;//!!!!!
	}
	
	if( pointnum > 65535 ){
		DbgOut( "e3dhsp : E3DCreateLine : pointnum too large : fixed to 65535 : warning !!!\n" );
		pointnum = 65535;
	}
	
	if( maxpointnum > 65535 ){
		DbgOut( "e3dhsp : E3DCreateLine : maxpointnum too large : fixed to 65535 : warning !!!\n" );
		maxpointnum = 65535;
	}

	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	newhs->m_extlineflag = 1;//!!!!!!!!

	ret = newhs->CreateLine( posptr, pointnum, maxpointnum, linekind );
	if( ret ){
		DbgOut( "e3dhsp : E3DCreateLine : newhs CreateLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


/////
	//ret = newhs->SetCurrentBSphereData();
	//if( ret ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	*lineidptr = newhs->serialno;


	return 0;
}
	

	//E3DDestroyLine lineid
EXPORT BOOL WINAPI E3DDestroyLine( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	// 1 : handlerset id

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
	
///////

	int ret;
	ret = DestroyHandlerSet( hsid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyLine : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int DestroyHandlerSet( int hsid )
{

///// textureの破棄

	EnterCriticalSection( &g_crit_restore );//######## start crit
	
	int ret;
	ret = g_texbnk->ResetDirtyFlag();

	ret = DestroyTexture( hsid, -2, -2 );
	if( ret ){
		DbgOut( "DestroyHandlerSet : DestroyTexture error !!!\n" );
		_ASSERT( 0 );
		//return 1;
	}

	LeaveCriticalSection( &g_crit_restore );//###### end crit


/////////////
	EnterCriticalSection( &g_crit_hshead );//######## start crit

	
	CHandlerSet* befhs = 0;
	CHandlerSet* curhs = hshead;
	CHandlerSet* findhs = 0;

	while( curhs && (findhs == 0) ){
		if( curhs->serialno == hsid ){
			findhs = curhs;
			break;
		}
		befhs = curhs;
		curhs = curhs->next;
	}

	if( findhs ){
		if( befhs ){
			befhs->next = findhs->next;
			if( findhs->next )
				findhs->next->prev = befhs;

			delete findhs;
		}else{
			hshead = findhs->next;
			if( findhs->next ){
				findhs->next->prev = 0;
				findhs->next->ishead = 1;
			}
			delete findhs;
		}
	}else{
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 0;
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	return 0;
}

//E3DSetLineColor lineid, a, r, g, b
EXPORT BOOL WINAPI E3DSetLineColor( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int a;
	a = hei->HspFunc_prm_getdi(0);

	int r;
	r = hei->HspFunc_prm_getdi(0);

	int g;
	g = hei->HspFunc_prm_getdi(0);

	int b;
	b = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////

	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : SetLineColor : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( a < 0 )
		a = 0;
	else if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;


	int ret;
	ret = curhs->SetExtLineColor( a, r, g, b );
	if( ret ){
		DbgOut( "e3dhsp : SetLineColor : curhs SetExtLineColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DAddPoint2Line lineid, prevpid, &newpid
		//previd == -2のとき先頭、-1のとき最後
EXPORT BOOL WINAPI E3DAddPoint2Line( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int curtype;	

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int previd;
	previd = hei->HspFunc_prm_getdi(0);


	int* newidptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddPoint2Line : parameter type error !!!\n" );
		return 1;
	}
	newidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : AddPoint2Line : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->AddPoint2ExtLine( previd, newidptr );
	if( ret ){
		DbgOut( "e3dhsp : AddPoint2Line : curhs AddPoint2ExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DDeletePointOfLine lineid, pid
EXPORT BOOL WINAPI E3DDeletePointOfLine( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int pid;
	pid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : DeletePointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeletePointOfExtLine( pid );
	if( ret ){
		DbgOut( "e3dhsp : DeletePointOfLine : curhs DeletePointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetPointPosOfLine lineid, pid, pos[3]
EXPORT BOOL WINAPI E3DSetPointPosOfLine( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int curtype;	

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int pid;
	pid = hei->HspFunc_prm_getdi(0);


	int* posptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSetPointPosOfLine : parameter type error !!!\n" );
		return 1;
	}
	posptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetPointPosOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 srcpos;
	srcpos.x = (float)( *posptr );
	srcpos.y = (float)( *(posptr + 1) );
	srcpos.z = (float)( *(posptr + 2) );

	int ret;
	ret = curhs->SetPointPosOfExtLine( pid, srcpos );
	if( ret ){
		DbgOut( "e3dhsp : SetPointPosOfLine : curhs SetPointPosOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetPointPosOfLine lineid, pid, pos[3]
EXPORT BOOL WINAPI E3DGetPointPosOfLine( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int curtype;	

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int pid;
	pid = hei->HspFunc_prm_getdi(0);


	int* posptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetPointPosOfLine : parameter type error !!!\n" );
		return 1;
	}
	posptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetPointPosOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 dstpos;

	int ret;
	ret = curhs->GetPointPosOfExtLine( pid, &dstpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetPointPosOfLine : curhs GetPointPosOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = (int)dstpos.x;
	*(posptr + 1) = (int)dstpos.y;
	*(posptr + 2) = (int)dstpos.z;

	return 0;
}

//E3DGetNextPointOfLine lineid, prevpid, &nextpid
	//pointid のnext チェインのpointidを返す。pointid == -1 のとき、先頭pointを返すnextid == -1のときは、nextなし、
EXPORT BOOL WINAPI E3DGetNextPointOfLine( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int curtype;	

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int previd;
	previd = hei->HspFunc_prm_getdi(0);


	int* nextptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetNextPointOfLine : parameter type error !!!\n" );
		return 1;
	}
	nextptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetNextPointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextPointOfExtLine( previd, nextptr );
	if( ret ){
		DbgOut( "e3dhsp : GetNextPointOfLine : curhs GetNextPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetPrevPointOfLine lineid, pid, &prevpid
	// befid == -1のときは、befなし
EXPORT BOOL WINAPI E3DGetPrevPointOfLine( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	int curtype;	

	int lid;
	lid = hei->HspFunc_prm_getdi(0);

	int pid;
	pid = hei->HspFunc_prm_getdi(0);


	int* prevptr;
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetPrevPointOfLine : parameter type error !!!\n" );
		return 1;
	}
	prevptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetPrevPointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetPrevPointOfExtLine( pid, prevptr );
	if( ret ){
		DbgOut( "e3dhsp : GetPrevPointOfLine : curhs GetPrevPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//WriteDisplay2BMP bmpname
EXPORT BOOL WINAPI E3DWriteDisplay2BMP( HSPEXINFO *hei, int p1, int p2, int p3 )
{

	char bmpname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSigLoad : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DSigLoad : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( bmpname, nameptr );


	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////////////

	
	
	//アンチがオンの時は、エラーにする。（どっちみちLockでエラーになる）
	if( s_multisample != 0 ){
		DbgOut( "E3DWriteDisplay2BMP : cant run because Antialias is ON error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	LPDIRECT3DDEVICE9 pd3dDevice;
	ret = g_pD3DApp->GetD3DDevice( &pd3dDevice );
	if( ret || !pd3dDevice ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : d3dDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	LPDIRECT3DSURFACE9 pBack;
	HRESULT hr;
	hr = pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBack );
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : GetBackBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DSURFACE_DESC desc;
	hr = pBack->GetDesc( &desc );
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : GetDesc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int backformat;
	UINT backWidth, backHeight;
	backformat = desc.Format;
	backWidth = desc.Width;
	backHeight = desc.Height;

	D3DLOCKED_RECT lockedrect;
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = backWidth;
	rect.bottom = backHeight;

////
	hr = pBack->LockRect( &lockedrect, &rect, 0 );
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : pback LockRect error !!!\n" );		
		_ASSERT( 0 );
		pBack->Release();
		return 1;
	}

		//DbgOut( "MotDlg:WriteBmp : width %d, height %d, pitch %d\n",
		//	backWidth, backHeight, lockedrect.Pitch );

	int bmpwidth, bmpheight;
	bmpheight = backHeight;
	int wcnt;
	wcnt = backWidth / 4;
	bmpwidth = wcnt * 4;

	int enmark = '\\';
	char* enpos = 0;
	enpos = strchr( bmpname, enmark );

	char outputname[2048];//
	ZeroMemory( outputname, 2048 );

	if( !enpos ){

		char moduledir[2048];
		int leng;
		ZeroMemory( moduledir, 2048 );
		leng = GetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPTSTR)moduledir, 2048 );
		if( (leng <= 0) || (leng >= 2048) ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : GetEnvironmentVariable error !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}


		int chkleng;
		chkleng = leng + (int)strlen( bmpname );

		if( chkleng >= 2048 ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : chkleng error !!! file name too long !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}

		strcpy( outputname, moduledir );
		strcat( outputname, bmpname );

		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : outputname0 %s\n", outputname );
	}else{
		int chkleng;
		chkleng = (int)strlen( bmpname );
		if( chkleng >= 2048 ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : chkleng error !!! file name too long !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}
		strcpy( outputname, bmpname );
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : outputname1 %s\n", outputname );
	
	}

	CDibFile* dibfile;
	dibfile = new CDibFile( outputname, bmpwidth, bmpheight, 24 );
	if( !dibfile ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : dibfile alloc error !!!\n" );
		_ASSERT( 0 );
		pBack->UnlockRect();
		pBack->Release();
		return 1;
	}


	BYTE* bitshead;
	int hno;
	for( hno = 0; hno < bmpheight; hno++ ){
		//bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno;
		//上下反転する。
		bitshead = (BYTE*)(lockedrect.pBits) + lockedrect.Pitch * hno;
		ret = dibfile->SetRGB( bmpheight - 1 - hno, bitshead, backformat );
		if( ret ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : dibfile SetRGB error !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			delete dibfile;
			return 1;
		}
	}

	ret = dibfile->CreateDibFile();
	if( ret ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : dibfile CreateFile error !!!\n" );
		_ASSERT( 0 );
		pBack->UnlockRect();
		pBack->Release();
		delete dibfile;
		return 1;
	}
	delete dibfile;

	hr = pBack->UnlockRect();
	if( hr != D3D_OK ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : pback UnlockRect error !!!\n" );
		_ASSERT( 0 );
		pBack->Release();
		return 1;
	}
	//pd3dDevice->Present( NULL, NULL, NULL, NULL );


	pBack->Release();


	switch( backformat ){
	case D3DFMT_X1R5G5B5:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format X1R5G5B5\n" );
		break;
	case D3DFMT_R5G6B5:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format R5G6B5\n" );
		break;
	case D3DFMT_X8R8G8B8:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format X8R8G8B8\n" );
		break;
	case D3DFMT_A8R8G8B8:
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : format A8R8G8B8\n" );
		break;
	default:
		break;
	}

	return 0;
}

//E3DCreateAVIFile filename, datatype, compkind, framerate, frameleng, aviid
		// ファイル名は、拡張子なし
		// 現バージョンでは、datatype は１：videoのみ
		// compkind
		//	0 --> 圧縮無し
		//	1 --> cinepak Codec by Radius
		//	2 --> Microsoft Video 1
		//終了時にcompleteしていないaviは、自動的にcompleteする。(ただしpdevが有効な間にしないとだめ！！！)

EXPORT BOOL WINAPI E3DCreateAVIFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	char aviname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DCreateAVIFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DCreateAVIFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DCreateAVIFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( aviname, nameptr );

// 2:
	int datatype;
	datatype = hei->HspFunc_prm_getdi(1);

// 3:
	int compkind;
	compkind = hei->HspFunc_prm_getdi(1);

// 4:
	int framerate;
	framerate = hei->HspFunc_prm_getdi(0);

// 5:
	int frameleng;
	frameleng = hei->HspFunc_prm_getdi(0);

// 6:
	curtype = *hei->nptype;
	int* aviidptr;
	if( curtype == 4 ){
		aviidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !aviidptr ){
			DbgOut( "e3dhsp : E3DCreateAVIFile : aviidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCreateAVIFile : aviidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*aviidptr = -1;

	//アンチがオンの時は、エラーにする。（どっちみちLockでエラーになる）
	if( s_multisample != 0 ){
		DbgOut( "E3DCreateAVIFile : cant run because Antialias is ON error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateAVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAviData* newavi;
	newavi = new CAviData();
	if( !newavi ){
		DbgOut( "E3DCreateAVIFile : newavi alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( datatype != 1 ){
		DbgOut( "E3DCreateAVIFile : this datatype not supported warning !!! set default value 1\n" );
		_ASSERT( 0 );
		datatype = 1;//!!!!!!!
	}

	if( (framerate < 0) || (framerate > 200) ){
		DbgOut( "E3DCreateAVIFile : framerate out of range warning !!! set default value 30 \n" );
		_ASSERT( 0 );
		framerate = 30;
	}

	if( frameleng < 0 ){
		DbgOut( "E3DCreateAVIFile : invalid frameleng warning !!! set default value 60 \n" );
		_ASSERT( 0 );
		frameleng = 60;
	}

	ret = newavi->CreateAVIFile( pdev, aviname, datatype, compkind, framerate, frameleng );
	if( ret ){
		DbgOut( "E3DCreateAVIFile : newavi CreateAVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	CAviData* curavi = avihead;
	CAviData* largeravi = 0;
	CAviData* smalleravi = 0;

	while( curavi ){
		if( newavi->serialno < curavi->serialno ){
			largeravi = curavi;
			break;
		}else{
			smalleravi = curavi;
		}
		curavi = curavi->next;
	}

	if( !avihead ){
		//先頭に追加
		newavi->ishead = 1;
		avihead = newavi;
	}else{
		if( largeravi ){
			//largermpの前に追加。
			ret = largeravi->AddToPrev( newavi );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
			if( newavi->ishead )
				avihead = newavi;
		}else{
			//最後に追加。(smallermp の後に追加)
			_ASSERT( smalleravi );
			ret = smalleravi->AddToNext( newavi );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
		}
	}

	*aviidptr = newavi->serialno;//!!!!

	return 0;

}
//E3DWriteData2AVIFile aviid, datatype
		//completeflagが立っている場合や、データ数がframelengに達している場合は、return する。
EXPORT BOOL WINAPI E3DWriteData2AVIFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
// 1:
	int aviid;
	aviid = hei->HspFunc_prm_getdi(0);

// 2:
	int datatype;
	datatype = hei->HspFunc_prm_getdi(1);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DWriteData2AVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CAviData* curavi;
	curavi = GetAviData( aviid );
	if( !curavi ){
		DbgOut( "E3DWriteData2AVIFile : aviid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( datatype != 1 ){
		DbgOut( "E3DWriteData2AVIFile : this datatype not supported warning !!! set default value 1 \n" );
		_ASSERT( 0 );
		datatype = 1;
	}


	ret = curavi->Write2AVIFile( pdev, datatype );
	if( ret ){
		DbgOut( "E3DWriteData2AVIFile : curavi Write2AVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}

//E3DCompleteAVIFile aviid
		//データ数が足りない場合は、ダミーデータを付加してcompleteする。
EXPORT BOOL WINAPI E3DCompleteAVIFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
// 1:
	int aviid;
	aviid = hei->HspFunc_prm_getdi(0);

// 2:
	int datatype;
	datatype = hei->HspFunc_prm_getdi(1);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	int ret;


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCompleteAVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CAviData* curavi;
	curavi = GetAviData( aviid );
	if( !curavi ){
		DbgOut( "E3DCompleteAVIFile : aviid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( datatype != 1 ){
		DbgOut( "E3DCompleteAVIFile : this datatype not supported warning !!! set default value 1 \n" );
		_ASSERT( 0 );
		datatype = 1;
	}


	ret = curavi->CompleteAVIFile( pdev, datatype );
	if( ret ){
		DbgOut( "E3DCompleteAVIFile : curavi CompleteAVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DCameraLookAt
//	E3DCameraLookAt vecx, vecy, vecz, upflag, divnum
EXPORT BOOL WINAPI E3DCameraLookAt( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

//1: vecx
	int vecx;
	vecx = hei->HspFunc_prm_getdi(0);

//2: vecy
	int vecy;
	vecy = hei->HspFunc_prm_getdi(0);

//3: vecz
	int vecz;
	vecz = hei->HspFunc_prm_getdi(1);

//4: upflag
	int upflag;
	upflag = hei->HspFunc_prm_getdi(2);

//5: divnum
	int divnum;
	divnum = hei->HspFunc_prm_getdi(1);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////




	D3DXVECTOR3 basevec;
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = 1.0f;
	DXVec3Normalize( &basevec, &basevec );

	D3DXVECTOR3 tarvec;
	tarvec.x = (float)vecx;
	tarvec.y = (float)vecy;
	tarvec.z = (float)vecz;
	DXVec3Normalize( &tarvec, &tarvec );

	int ret;
	CQuaternion2 invq;

	s_cameraq.inv( &invq );//!!!!!
	invq.twist = s_camera_twist;//!!!!!!!

	ret = LookAtQ( &invq, tarvec, basevec, divnum, upflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraLookAt : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	invq.inv( &s_cameraq );//!!!!!


	D3DXVECTOR3 dirvec;
	invq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

	//s_camerapos =
	s_cameratarget = s_camerapos + dirvec;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	return 0;
}


//E3DCameraOnNaviLine nlid, mode, roundflag, reverseflag, offx, offy, offz, posstep, dirdivnum, upflag, tpid
//	mode = 0 -> 位置のみ
//	mode = 1 -> 位置と向き
//	posstep : マイナスで、分割数指定。
//	dirdivnum : 分割数
EXPORT BOOL WINAPI E3DCameraOnNaviLine( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

//1: nlid
	int nlid;
	nlid = hei->HspFunc_prm_getdi(0);

//2: mode
	int mode;
	mode = hei->HspFunc_prm_getdi(1);

//3: roundflag
	int roundflag;
	roundflag = hei->HspFunc_prm_getdi(0);

//4: reverseflag
	int reverseflag;
	reverseflag = hei->HspFunc_prm_getdi(0);

//5: offx
	int offx;
	offx = hei->HspFunc_prm_getdi(0);

//6: offy
	int offy;
	offy = hei->HspFunc_prm_getdi(0);

//7: offz
	int offz;
	offz = hei->HspFunc_prm_getdi(0);


//8: posstep
	int posstep;
	posstep = hei->HspFunc_prm_getdi(100);

//9: dirdivnum
	int dirdivnum;
	dirdivnum = hei->HspFunc_prm_getdi(10);

//10: upflag
	int upflag;
	upflag = hei->HspFunc_prm_getdi(2);

//11: tpidptr
	int curtype;
	curtype = *hei->nptype;
	int* tpidptr;
	if( curtype == 4 ){
		tpidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !tpidptr ){
			DbgOut( "e3dhsp : E3DCameraOnNaviLine : tpidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCameraOnNaviLine : tpidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////


//DbgOut( "e3dhsp : camonvl : cameraq %f %f %f %f\n", s_cameraq.x, s_cameraq.y, s_cameraq.z, s_cameraq.w );


	int ret;

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DCameraOnNaviLine : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 offset;
	offset.x = (float)offx;
	offset.y = (float)offy;
	offset.z = (float)offz;


	D3DXVECTOR3 newpos;
	CQuaternion newq;

	float fdegstep = 360.0f;

	CQuaternion tempdirq;
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	tempdirq.CopyFromCQuaternion2( &invq );//!!!!!!!!!

	ret = curnl->ControlByNaviLine( s_camerapos, offset, tempdirq, 
			1, roundflag, reverseflag,
			1000.0f, 
			(float)posstep, fdegstep,
			&newpos, &newq, tpidptr );
	if( ret ){
		DbgOut( "E3DCameraOnNaviLine : curnl ControlByNaviLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaternion2 invq2;
	invq2.w = invq.w;
	invq2.x = invq.x;
	invq2.y = invq.y;
	invq2.z = invq.z;
	//invq2.twist = 0.0f;
	invq2.twist = s_camera_twist;//!!!!!!!!!

	D3DXVECTOR3 basevec;
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = 1.0f;


	if( mode == 1 ){
		D3DXVECTOR3 tarvec;
		tarvec = newpos - s_camerapos;
		float tarmag;
		tarmag = tarvec.x * tarvec.x + tarvec.y * tarvec.y + tarvec.z * tarvec.z;
		if( tarmag >= 0.001f ){
			DXVec3Normalize( &tarvec, &tarvec );
			ret = LookAtQ( &invq2, tarvec, basevec, dirdivnum, upflag );
			if( ret ){
				DbgOut( "e3dhsp : E3DCameraOnNaviLine : LookAtQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		invq2.inv( &s_cameraq );//!!!!!
	}

	D3DXVECTOR3 dirvec;
	invq2.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

	s_cameratarget = newpos + dirvec;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq2.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;

	s_camerapos = newpos;//!!!!!
	
	return 0;
}


//	E3DCameraUp deg, divnum
EXPORT BOOL WINAPI E3DCameraDirUp( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

//1: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);

//2: divnum
	int divnum;
	divnum = hei->HspFunc_prm_getdi(10);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = (float)deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldy( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newz, newy;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newy, oldy );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newz, &newy );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////　上方向を調整する。
	
	invq.twist = s_camera_twist;//!!!!!!!

	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );

	return 0;
}


//	E3DCameraDown deg, divnum
EXPORT BOOL WINAPI E3DCameraDirDown( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

//1: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);
//2: divnum
	int divnum;
	divnum = hei->HspFunc_prm_getdi(10);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = (float)deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldy( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newz, newy;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newy, oldy );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newz, &newy );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, -rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////　上方向を調整する。
	
	invq.twist = s_camera_twist;//!!!!!!!!!

	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );


	return 0;
}
//	E3DCameraRight deg, divnum
EXPORT BOOL WINAPI E3DCameraDirRight( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

//1: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);
//2: divnum
	int divnum;
	divnum = hei->HspFunc_prm_getdi(10);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = (float)deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldx( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 newz, newx;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newx, oldx );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newx, &newz );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, -rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////　上方向を調整する。
	
	invq.twist = s_camera_twist;//!!!!!

	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );

	return 0;
}
//	E3DCameraLeft deg, divnum
EXPORT BOOL WINAPI E3DCameraDirLeft( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	//int curtype;

//1: deg
	int deg;
	deg = hei->HspFunc_prm_getdi(0);
//2: divnum
	int divnum;
	divnum = hei->HspFunc_prm_getdi(10);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = (float)deg * (float)DEG2PAI; 

	D3DXVECTOR3 oldz( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 oldx( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 newz, newx;

	invq.Rotate( &newz, oldz );
	invq.Rotate( &newx, oldx );


	D3DXVECTOR3 axis;
	D3DXVec3Cross( &axis, &newx, &newz );
	DXVec3Normalize( &axis, &axis );

	CQuaternion2 rotq, newq;
	rotq.SetAxisAndRot( axis, rad );


	newq = rotq * invq;



	D3DXVECTOR3 basevec( 0.0f, 0.0f, 1.0f );
	D3DXVECTOR3 dirvec;
	newq.Rotate( &dirvec, basevec );
	DXVec3Normalize( &dirvec, &dirvec );

/////　上方向を調整する。
	
	invq.twist = s_camera_twist;//!!!!!!!!

	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 finaldir;
	invq.Rotate( &finaldir, basevec );
	DXVec3Normalize( &finaldir, &finaldir );

	s_cameratarget = s_camerapos + finaldir;

	D3DXVECTOR3 up0( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 newup;
	invq.Rotate( &newup, up0 );

	s_cameraupvec = newup;
	s_cameratype = 2;


	invq.inv( &s_cameraq );


	return 0;
}

EXPORT BOOL WINAPI E3DLoadBIMFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//ver1.1.1.0で、サポート中止

	return 0;
}

/***
EXPORT BOOL WINAPI E3DLoadBIMFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : hsid
	// 2 : sig file name

//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadBIMFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DLoadBIMFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadBIMFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : LoadBIMFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->LoadBIMFile( tempname );
	if( ret ){
		DbgOut( "e3dhsp : LoadBIMFile : curhs LoadBIMFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
***/


//E3DCreateFont height, width, weight, bItalic, bUnderline, bStrikeout, fontname, &fontidptr
EXPORT BOOL WINAPI E3DCreateFont( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int height;
	height = hei->HspFunc_prm_getdi(0);

//2
	int width;
	width = hei->HspFunc_prm_getdi(0);

//3
	int weight;
	weight = hei->HspFunc_prm_getdi(0);

//4
	int bItalic;
	bItalic = hei->HspFunc_prm_getdi(0);

//5
	int bUnderline;
	bUnderline = hei->HspFunc_prm_getdi(0);

//6
	int bStrikeout;
	bStrikeout = hei->HspFunc_prm_getdi(0);

//7
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DCreateFont : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DCreateFont : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadBIMFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//8
	curtype = *hei->nptype;
	int* fontidptr;
	if( curtype == 4 ){
		fontidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !fontidptr ){
			DbgOut( "e3dhsp : E3DCreateFont : fontidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCreateFont : fontidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////
	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateFont : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CXFont* newfont;
	newfont = new CXFont();
	if( !newfont ){
		DbgOut( "E3DCreateFont : newfont alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newfont->CreateFont( pdev, height, width, weight, bItalic, bUnderline, bStrikeout, tempname );
	if( ret ){
		DbgOut( "E3DCreateFont : newfont CreateFont error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


/////////
	CXFont* curfont = xfonthead;
	CXFont* largerfont = 0;
	CXFont* smallerfont = 0;

	while( curfont ){
		if( newfont->m_serialno < curfont->m_serialno ){
			largerfont = curfont;
			break;
		}else{
			smallerfont = curfont;
		}
		curfont = curfont->next;
	}

	if( !xfonthead ){
		//先頭に追加
		newfont->ishead = 1;
		xfonthead = newfont;
	}else{
		if( largerfont ){
			//largermpの前に追加。
			ret = largerfont->AddToPrev( newfont );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
			if( newfont->ishead )
				xfonthead = newfont;
		}else{
			//最後に追加。(smallermp の後に追加)
			_ASSERT( smallerfont );
			ret = smallerfont->AddToNext( newfont );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
		}
	}

	*fontidptr = newfont->m_serialno;//!!!!

	return 0;
}

//E3DDrawTextByFontID fontid, posx, posy, textstr, a, r, g, b
EXPORT BOOL WINAPI E3DDrawTextByFontID( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int fontid;
	fontid = hei->HspFunc_prm_getdi(0);

//2
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//3
	int posy;
	posy = hei->HspFunc_prm_getdi(0);

//4
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DDrawTextByFontID : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DDrawTextByFontID : parameter error !!!\n" );
		return 1;
	}
	
//5
	int a;
	a = hei->HspFunc_prm_getdi(0);

//6
	int r;
	r = hei->HspFunc_prm_getdi(0);

//7
	int g;
	g = hei->HspFunc_prm_getdi(0);

//8
	int b;
	b = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontID : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_hwnd ){
		DbgOut( "E3DDrawTextByFontID : application HWND not initialized error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}


	if( a < 0 )
		a = 0;
	if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	D3DCOLOR col;
	col = D3DCOLOR_ARGB( a, r, g, b );

	int ret;
	ret = curfont->DrawText( s_hwnd, nameptr, posx, posy, col );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID : curfont DrawText error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DDrawTextByFontIDWithCnt fontid, posx, posy, textstr, a, r, g, b, eachcnt, curcnt
EXPORT BOOL WINAPI E3DDrawTextByFontIDWithCnt( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int fontid;
	fontid = hei->HspFunc_prm_getdi(0);

//2
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//3
	int posy;
	posy = hei->HspFunc_prm_getdi(0);

//4
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DDrawTextByFontIDWithCnt : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : parameter error !!!\n" );
		return 1;
	}
	
//5
	int a;
	a = hei->HspFunc_prm_getdi(0);

//6
	int r;
	r = hei->HspFunc_prm_getdi(0);

//7
	int g;
	g = hei->HspFunc_prm_getdi(0);

//8
	int b;
	b = hei->HspFunc_prm_getdi(0);

//9
	int eachcnt;
	eachcnt = hei->HspFunc_prm_getdi(0);

//10
	int curcnt;
	curcnt = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_hwnd ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : application HWND not initialized error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}


	if( a < 0 )
		a = 0;
	if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	D3DCOLOR col;
	col = D3DCOLOR_ARGB( a, r, g, b );


	int ret;
	ret = curfont->DrawTextWithCnt( s_hwnd, nameptr, posx, posy, col, eachcnt, curcnt );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID : curfont DrawText error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DDrawTextByFontIDWithCnt2 fontid, posx, posy, textstr, a, r, g, b, cntarray, arrayleng, curcnt
EXPORT BOOL WINAPI E3DDrawTextByFontIDWithCnt2( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int fontid;
	fontid = hei->HspFunc_prm_getdi(0);

//2
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//3
	int posy;
	posy = hei->HspFunc_prm_getdi(0);

//4
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DDrawTextByFontIDWithCnt : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : parameter error !!!\n" );
		return 1;
	}
	
//5
	int a;
	a = hei->HspFunc_prm_getdi(0);

//6
	int r;
	r = hei->HspFunc_prm_getdi(0);

//7
	int g;
	g = hei->HspFunc_prm_getdi(0);

//8
	int b;
	b = hei->HspFunc_prm_getdi(0);

//9
	curtype = *hei->nptype;
	int* cntarray;
	if( curtype == 4 ){
		cntarray = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !cntarray ){
			DbgOut( "e3dhsp : E3DDrawTextByFontIDWithCnt2 : cntarray error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : cntarray parameter type error %d!!!\n", curtype );
		return 1;
	}

//10
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);

//11
	int curcnt;
	curcnt = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_hwnd ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : application HWND not initialized error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}


	if( a < 0 )
		a = 0;
	if( a > 255 )
		a = 255;

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;

	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	D3DCOLOR col;
	col = D3DCOLOR_ARGB( a, r, g, b );


	int ret;
	ret = curfont->DrawTextWithCnt2( s_hwnd, nameptr, posx, posy, col, cntarray, arrayleng, curcnt );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID2 : curfont DrawText error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




//E3DGetCharacterNum textstr, numptr
EXPORT BOOL WINAPI E3DGetCharacterNum( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DGetCharacterNum : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DGetCharacterNum : parameter error !!!\n" );
		return 1;
	}

//2	
	curtype = *hei->nptype;
	int* numptr;
	if( curtype == 4 ){
		numptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !numptr ){
			DbgOut( "e3dhsp : E3DGetCharacterNum : numptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetCharacterNum : numptr parameter type error %d!!!\n", curtype );
		return 1;
	}
	
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*numptr = 0;

	if( !nameptr )
		return 0;

	unsigned char curc;
	int totalnum = 0;
	int curpos = 0;
	
	int orgleng;
	orgleng = (int)strlen( nameptr );
	if( orgleng <= 0 )
		return 0;

	while( curpos < orgleng ){
		curc = *( nameptr + curpos );

		if( curc == 0 )
			break;

		if( curc < 0x80 ){
			curpos++;
			//DbgOut( "GetCharaNum : 1byte : %c, %d\n", curc, curc );
		}else{
			curpos += 2;
			//DbgOut( "GetCharaNum : 2byte : %c, %d\n", curc, curc );
		}
		totalnum++;
	}


	//DbgOut( "GetCharaNum : \\r %d, \\n %d\n", '\r', '\n' ); 

	*numptr = totalnum;


	return 0;
}

//E3DDestroyFont fontid
EXPORT BOOL WINAPI E3DDestroyFont( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int fontid;
	fontid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	CXFont* beffont = 0;
	CXFont* curfont = xfonthead;
	CXFont* findfont = 0;

	while( curfont && (findfont == 0) ){
		if( curfont->m_serialno == fontid ){
			findfont = curfont;
			break;
		}
		beffont = curfont;
		curfont = curfont->next;
	}

	if( findfont ){
		if( beffont ){
			beffont->next = findfont->next;
			if( findfont->next )
				findfont->next->prev = beffont;
			delete findfont;
		}else{
			xfonthead = findfont->next;
			if( findfont->next ){
				findfont->next->prev = 0;
				findfont->next->ishead = 1;
			}
			delete findfont;
		}
	}else{
		return 0;
	}

	return 0;
}


//E3DSlerpQ startqid, endqid, t, tdiv, resqid
EXPORT BOOL WINAPI E3DSlerpQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int startqid;
	startqid = hei->HspFunc_prm_getdi(0);

//2
	int endqid;
	endqid = hei->HspFunc_prm_getdi(0);

//3
	int t;
	t = hei->HspFunc_prm_getdi(0);

//4
	int tdiv;
	tdiv = hei->HspFunc_prm_getdi(0);

//5
	int resqid;
	resqid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	if( !s_qh ){
		DbgOut( "E3DSlerpQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* startq2;
	CQuaternion2* endq2;
	CQuaternion2* resq2;

	int ret;
	ret = s_qh->GetQ( startqid, &startq2 );
	if( ret ){
		DbgOut( "E3DSlerpQ : qh GetQ startq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( endqid, &endq2 );
	if( ret ){
		DbgOut( "E3DSlerpQ : qh GetQ endq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( resqid, &resq2 );
	if( ret ){
		DbgOut( "E3DSlerpQ : qh GetQ resq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float ft;
	if( tdiv != 0 ){
		ft = (float)t / (float)tdiv;
		if( ft > 1.0f )
			ft = 1.0f;
		else if( ft < 0.0f )
			ft = 0.0f;
	}else{
		ft = 0.0f;
	}

	*resq2 = startq2->Slerp( *endq2, 10000, (int)( ft * 10000.0f ) );
	

	return 0;
}


//E3DSquadQ befqid, startqid, endqid, aftqid, t, tdiv, resqid
EXPORT BOOL WINAPI E3DSquadQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int befqid;
	befqid = hei->HspFunc_prm_getdi(0);

//2
	int startqid;
	startqid = hei->HspFunc_prm_getdi(0);

//3
	int endqid;
	endqid = hei->HspFunc_prm_getdi(0);

//4
	int aftqid;
	aftqid = hei->HspFunc_prm_getdi(0);

//5
	int t;
	t = hei->HspFunc_prm_getdi(0);

//6
	int tdiv;
	tdiv = hei->HspFunc_prm_getdi(0);

//7
	int resqid;
	resqid = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	if( !s_qh ){
		DbgOut( "E3DSquadQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* befq2;
	CQuaternion2* startq2;
	CQuaternion2* endq2;
	CQuaternion2* aftq2;
	CQuaternion2* resq2;

	int ret;
	ret = s_qh->GetQ( befqid, &befq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ befq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( startqid, &startq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ startq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( endqid, &endq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ endq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( aftqid, &aftq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ aftq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( resqid, &resq2 );
	if( ret ){
		DbgOut( "E3DSquadQ : qh GetQ resq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float ft;
	if( tdiv != 0 ){
		ft = (float)t / (float)tdiv;
		if( ft > 1.0f )
			ft = 1.0f;
		else if( ft < 0.0f )
			ft = 0.0f;
	}else{
		ft = 0.0f;
	}



	ret = resq2->Squad( *befq2, *startq2, *endq2, *aftq2, ft );
	if( ret ){
		DbgOut( "E3DSquadQ : resq Squad error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSplineVec pos0, pos1, pos2, pos3, t, tdiv, respos
EXPORT BOOL WINAPI E3DSplineVec( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1
	curtype = *hei->nptype;
	int* pos0ptr;
	if( curtype == 4 ){
		pos0ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !pos0ptr ){
			DbgOut( "e3dhsp : E3DSplineVec : pos0ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DsplineVec : pos0ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//2
	curtype = *hei->nptype;
	int* pos1ptr;
	if( curtype == 4 ){
		pos1ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !pos1ptr ){
			DbgOut( "e3dhsp : E3DsplineVec : pos1ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DsplineVec : pos1ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//3
	curtype = *hei->nptype;
	int* pos2ptr;
	if( curtype == 4 ){
		pos2ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !pos2ptr ){
			DbgOut( "e3dhsp : E3DsplineVec : pos2ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DsplineVec : pos2ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//4
	curtype = *hei->nptype;
	int* pos3ptr;
	if( curtype == 4 ){
		pos3ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !pos3ptr ){
			DbgOut( "e3dhsp : E3DsplineVec : pos3ptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DsplineVec : pos3ptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//5
	int t;
	t = hei->HspFunc_prm_getdi(0);

//6
	int tdiv;
	tdiv = hei->HspFunc_prm_getdi(0);

//7
	curtype = *hei->nptype;
	int* resposptr;
	if( curtype == 4 ){
		resposptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !resposptr ){
			DbgOut( "e3dhsp : E3DsplineVec : resposptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DsplineVec : resposptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	float ft;
	if( tdiv != 0 ){
		ft = (float)t / (float)tdiv;
		if( ft > 1.0f )
			ft = 1.0f;
		else if( ft < 0.0f )
			ft = 0.0f;
	}else{
		ft = 0.0f;
	}

	D3DXVECTOR3 pos0, pos1, pos2, pos3, retpos;
	pos0.x = (float)( *pos0ptr );
	pos0.y = (float)( *(pos0ptr + 1) );
	pos0.z = (float)( *(pos0ptr + 2) );

	pos1.x = (float)( *pos1ptr );
	pos1.y = (float)( *(pos1ptr + 1) );
	pos1.z = (float)( *(pos1ptr + 2) );

	pos2.x = (float)( *pos2ptr );
	pos2.y = (float)( *(pos2ptr + 1) );
	pos2.z = (float)( *(pos2ptr + 2) );

	pos3.x = (float)( *pos3ptr );
	pos3.y = (float)( *(pos3ptr + 1) );
	pos3.z = (float)( *(pos3ptr + 2) );

	int ret;
	ret = CatmullRomPoint( &pos0, &pos1, &pos2, &pos3, ft, &retpos );
	if( ret ){
		DbgOut( "E3DSplineVec : CatmullRomPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	*resposptr = (int)retpos.x;
	*(resposptr + 1) = (int)retpos.y;
	*(resposptr + 2) = (int)retpos.z;


	return 0;
}

int CatmullRomPoint( D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* p3, D3DXVECTOR3* p4, float u, D3DXVECTOR3* resp )
{
	float coef1, coef2, coef3, coef4;



	coef1 = -0.5f * u * u * u + u * u - 0.5f * u;
	coef2 = 1.5f * u * u * u - 2.5f * u * u + 1.0f;
	coef3 = -1.5f * u * u * u + 2.0f * u * u + 0.5f * u;
	coef4 = 0.5f * u * u * u - 0.5f * u * u;

	*resp = *p1 * coef1 + *p2 * coef2 + *p3 * coef3 + *p4 * coef4;

	return 0;
}

EXPORT BOOL WINAPI E3DDbgOut( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	char tempname[4098];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DDbgOut : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DDbgOut : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= (4098 - 2) ){
		DbgOut( "e3dhsp : DbgOut : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );
	strcat( tempname, "\r\n" );
	
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////////

	DbgOut( tempname );

	return 0;

}

EXPORT BOOL WINAPI E3DGetDiffuse( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);

	//4 : rptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetDiffuse : parameter type error !!!\n" );
		return 1;
	}
	int* rptr;
	rptr = (int *)hei->HspFunc_prm_getv();

	//5 : gptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetDiffuse : parameter type error !!!\n" );
		return 1;
	}
	int* gptr;
	gptr = (int *)hei->HspFunc_prm_getv();

	//6 : bptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetDiffuse : parameter type error !!!\n" );
		return 1;
	}
	int* bptr;
	bptr = (int *)hei->HspFunc_prm_getv();


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetDiffuse : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetDiffuse : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	ret = curhs->GetDiffuse( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "e3dhsp : GetDiffuse : curhs GetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DGetAmbient( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);

	//4 : rptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetAmbient : parameter type error !!!\n" );
		return 1;
	}
	int* rptr;
	rptr = (int *)hei->HspFunc_prm_getv();

	//5 : gptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetAmbient : parameter type error !!!\n" );
		return 1;
	}
	int* gptr;
	gptr = (int *)hei->HspFunc_prm_getv();

	//6 : bptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetAmbient : parameter type error !!!\n" );
		return 1;
	}
	int* bptr;
	bptr = (int *)hei->HspFunc_prm_getv();


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetAmbient : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetAmbient : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	ret = curhs->GetAmbient( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "e3dhsp : GetAmbient : curhs GetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DGetSpecular( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);

	//4 : rptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSpecular : parameter type error !!!\n" );
		return 1;
	}
	int* rptr;
	rptr = (int *)hei->HspFunc_prm_getv();

	//5 : gptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSpecular : parameter type error !!!\n" );
		return 1;
	}
	int* gptr;
	gptr = (int *)hei->HspFunc_prm_getv();

	//6 : bptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSpecular : parameter type error !!!\n" );
		return 1;
	}
	int* bptr;
	bptr = (int *)hei->HspFunc_prm_getv();


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecular : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecular : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	ret = curhs->GetSpecular( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "e3dhsp : GetSpecular : curhs GetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DGetAlpha( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);

	//4 : aptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetAlpha : parameter type error !!!\n" );
		return 1;
	}
	int* aptr;
	aptr = (int *)hei->HspFunc_prm_getv();


	if ( *hei->er ) return *hei->er;		// エラーチェック

/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetAlpha : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetAlpha : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;

	ret = curhs->GetAlpha( partno, vertno, aptr );
	if( ret ){
		DbgOut( "e3dhsp : GetAlpha : curhs GetAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSaveQuaFile hsid, mkid, filename 
EXPORT BOOL WINAPI E3DSaveQuaFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : hsid
	// 2 : mkid
	// 3 : file name1

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

// 2
	int mkid;
	mkid = hei->HspFunc_prm_getdi(0);
	
// 3
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSaveQuaFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DSaveQuaFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DSaveQuaFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );
////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveQuaFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SaveQuaFile( mkid, tempname1 );
	if( ret ){
		DbgOut( "E3DSaveQuaFile : curhs SaveQuaFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DSaveSigFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : hsid
	// 2 : file name1

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSaveSigFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DSaveSigFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DSaveSigFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );
////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMQOFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SaveSigFile( tempname1 );
	if( ret ){
		DbgOut( "E3DSaveSigFile : curhs SaveSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetMipMapParams miplevels, mipfilter
//UINT g_miplevels = 0;
//DWORD g_mipfilter = D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR;
EXPORT BOOL WINAPI E3DSetMipMapParams( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

// 1
	int miplevels;
	miplevels = hei->HspFunc_prm_getdi(0);
	
//2
	int mipfilter;
	mipfilter = hei->HspFunc_prm_getdi(-1);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////


	g_miplevels = (UINT)miplevels;

	if( mipfilter != -1 ){
		g_mipfilter = (DWORD)mipfilter;
	}else{
		g_mipfilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR;
	}


	return 0;
}

//E3DPickVert hsid, posx, posy, rangex, rangey, pickpartarray, pickvertarray, arrayleng, getnum
EXPORT BOOL WINAPI E3DPickVert( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

//3
	int posy;
	posy = hei->HspFunc_prm_getdi(0);

//4
	int rangex;
	rangex = hei->HspFunc_prm_getdi(0);

//5
	int rangey;
	rangey = hei->HspFunc_prm_getdi(0);

//6 : partptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickVert : parameter type error !!!\n" );
		return 1;
	}
	int* partptr;
	partptr = (int *)hei->HspFunc_prm_getv();

//7 : vertptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickVert : parameter type error !!!\n" );
		return 1;
	}
	int* vertptr;
	vertptr = (int *)hei->HspFunc_prm_getv();

//8
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);


//9 : getnumptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickVert : parameter type error !!!\n" );
		return 1;
	}
	int* getnumptr;
	getnumptr = (int *)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->PickVert( posx, posy, rangex, rangey, partptr, vertptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DPickVert : curhs PickVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetCullingFlag hsid, partno, vertno, viewcullflag, revcullflag
EXPORT BOOL WINAPI E3DGetCullingFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : viewcullptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCullingFlag : parameter type error !!!\n" );
		return 1;
	}
	int* viewcullptr;
	viewcullptr = (int *)hei->HspFunc_prm_getv();

//7 : revcullptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCullingFlag : parameter type error !!!\n" );
		return 1;
	}
	int* revcullptr;
	revcullptr = (int *)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCullingFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	ret = curhs->GetCullingFlag( partno, vertno, viewcullptr, revcullptr );
	if( ret ){
		DbgOut( "E3DGetCullingFlag : curhs GetCullingFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetOrgVertNo hsid, partno, vertno, orgnoarray, arrayleng, getnum
EXPORT BOOL WINAPI E3DGetOrgVertNo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : orgnoarray
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCullingFlag : parameter type error !!!\n" );
		return 1;
	}
	int* orgnoarray;
	orgnoarray = (int *)hei->HspFunc_prm_getv();

//5
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);


//6 : getnum
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetCullingFlag : parameter type error !!!\n" );
		return 1;
	}
	int* getnumptr;
	getnumptr = (int *)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCullingFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->GetOrgVertNo( partno, vertno, orgnoarray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetOrgVertNo : curhs GetOrgVertNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



EXPORT BOOL WINAPI E3DChkIM2Status( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2 : statusptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkIM2Status : parameter type error !!!\n" );
		return 1;
	}
	int* statusptr;
	statusptr = (int *)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkIM2Status : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->ChkIM2Status( statusptr );
	if( ret ){
		DbgOut( "E3DChkIM2Status : curhs ChkIM2Status error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DLoadIM2File( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : hsid
	// 2 : im2 file name

//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadIM2File : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DLoadIM2File : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadIM2File : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : LoadIM2File : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->LoadIM2File( tempname );
	if( ret ){
		DbgOut( "e3dhsp : LoadIM2File : curhs LoadBIMFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DSaveIM2File( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : hsid
	// 2 : file name1

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSaveIM2File : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DSaveIM2File : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DSaveIM2File : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );
////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveIM2File : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SaveIM2File( tempname1 );
	if( ret ){
		DbgOut( "E3DSaveIM2File : curhs SaveIM2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EXPORT BOOL WINAPI E3DGetJointNum( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2
	curtype = *hei->nptype;
	int* numptr;
	if( curtype == 4 ){
		numptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !numptr ){
			DbgOut( "e3dhsp : E3DGetJointNum : numptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetJointNum : numptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	*numptr = 0;//!!!!!!!!

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetJointNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->GetJointNum( numptr );
	if( ret ){
		DbgOut( "E3DGetJointNum : curhs GetJointNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



EXPORT BOOL WINAPI E3DGetJointInfo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
	
// 2
	int maxnum;
	maxnum = hei->HspFunc_prm_getdi(0);

// 3
	curtype = *hei->nptype;
	char* strptr;
	if( curtype == 4 ){
		strptr = (char *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !strptr ){
			DbgOut( "e3dhsp : E3DGetJointInfo : strptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetJointInfo: strptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 4
	curtype = *hei->nptype;
	int* seriptr;
	if( curtype == 4 ){
		seriptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !seriptr ){
			DbgOut( "e3dhsp : E3DGetJointInfo : seriptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetJointInfo : seriptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 5
	curtype = *hei->nptype;
	int* getnumptr;
	if( curtype == 4 ){
		getnumptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !getnumptr ){
			DbgOut( "e3dhsp : E3DGetJointInfo : getnumptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetJointInfo : getnumptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////


	//strcpy( strptr, "あいうえおかきくけこ0123" );
	//strcpy( strptr + 256, "さしすせそ" );
	//strcpy( strptr + 512, "abcdefghijkたちつてと4567_" );
	//*getnumptr = 3;

	*getnumptr = 0;//!!!!!!!!

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetJointInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetJointInfo( maxnum, strptr, seriptr, getnumptr );
	if( ret ){
		DbgOut( "E3DGetJointInfo : curhs GetJointInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DGetFirstJointNo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2
	curtype = *hei->nptype;
	int* noptr;
	if( curtype == 4 ){
		noptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !noptr ){
			DbgOut( "e3dhsp : E3DGetFirstJointNo : noptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetFirstJointNo : noptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	*noptr = -1;//!!!!!!!!

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetFirstJointNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->GetFirstJointNo( noptr );
	if( ret ){
		DbgOut( "E3DGetFirstJointNo : curhs GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//GetDispObjNum
EXPORT BOOL WINAPI E3DGetDispObjNum( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2
	curtype = *hei->nptype;
	int* numptr;
	if( curtype == 4 ){
		numptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !numptr ){
			DbgOut( "e3dhsp : E3DGetDispObjNum : numptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDispObjNum : numptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	*numptr = 0;//!!!!!!!!

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispObjNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->GetDispObjNum( numptr );
	if( ret ){
		DbgOut( "E3DGetDispObjNum : curhs GetDispObjNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//GetDispObjInfo
EXPORT BOOL WINAPI E3DGetDispObjInfo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
	
// 2
	int maxnum;
	maxnum = hei->HspFunc_prm_getdi(0);

// 3
	curtype = *hei->nptype;
	char* strptr;
	if( curtype == 4 ){
		strptr = (char *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !strptr ){
			DbgOut( "e3dhsp : E3DGetDispObjInfo : strptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDispObjInfo : strptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 4
	curtype = *hei->nptype;
	int* seriptr;
	if( curtype == 4 ){
		seriptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !seriptr ){
			DbgOut( "e3dhsp : E3DGetDispObjInfo : seriptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDispObjInfo : seriptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 5
	curtype = *hei->nptype;
	int* getnumptr;
	if( curtype == 4 ){
		getnumptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !getnumptr ){
			DbgOut( "e3dhsp : E3DGetDispObjInfo : getnumptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetDispObjInfo : getnumptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	*getnumptr = 0;//!!!!!!!!

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispObjInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetDispObjInfo( maxnum, strptr, seriptr, getnumptr );
	if( ret ){
		DbgOut( "E3DGetDispObjInfo : curhs GetDispObjInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetIM2Params hsid, partno, optvno, childjoint, parentjoint, calcmode
EXPORT BOOL WINAPI E3DSetIM2Params( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

// 3
	int optvno;
	optvno = hei->HspFunc_prm_getdi(0);

// 4
	int childjoint;
	childjoint = hei->HspFunc_prm_getdi(0);

// 5
	int parentjoint;
	parentjoint = hei->HspFunc_prm_getdi(0);

// 6
	int calcmode;
	calcmode = hei->HspFunc_prm_getdi(0);


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////



	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIM2Params : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetIM2Params( partno, optvno, childjoint, parentjoint, calcmode );
	if( ret ){
		DbgOut( "E3DSetIM2Params : curhs SetIM2Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DGetIM2Params hsid, partno, optvno, childjointptr, parentjointptr, calcmodeptr
EXPORT BOOL WINAPI E3DGetIM2Params( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
	
// 2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

// 3
	int optvno;
	optvno = hei->HspFunc_prm_getdi(0);

// 4
	curtype = *hei->nptype;
	int* childjointptr;
	if( curtype == 4 ){
		childjointptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !childjointptr ){
			DbgOut( "e3dhsp : E3DGetIM2Params : childjointptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetIM2Params : childjointptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 5
	curtype = *hei->nptype;
	int* parentjointptr;
	if( curtype == 4 ){
		parentjointptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !parentjointptr ){
			DbgOut( "e3dhsp : E3DGetIM2Params : parentjointptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetIM2Params : parentjointptr parameter type error %d!!!\n", curtype );
		return 1;
	}

// 6
	curtype = *hei->nptype;
	int* calcmodeptr;
	if( curtype == 4 ){
		calcmodeptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !calcmodeptr ){
			DbgOut( "e3dhsp : E3DGetIM2Params : calcmodeptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetIM2Params : calcmodeptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////


	DbgOut( "E3DGetIM2Params : this func is out of support now !!!\n" );


	/***

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIM2Params : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetIM2Params( partno, optvno, childjointptr, parentjointptr, calcmodeptr );
	if( ret ){
		DbgOut( "E3DGetIM2Params : curhs GetIM2Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/

	return 0;
}

//E3DEnableTexture hsid, partno, enableflag
EXPORT BOOL WINAPI E3DEnableTexture( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202


// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

// 3
	int enableflag;
	enableflag = hei->HspFunc_prm_getdi(0);


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIM2Params : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->EnableTexture( partno, enableflag );
	if( ret ){
		DbgOut( "E3DEnableTexture : curhs EnableTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int GetScreenPos( D3DXVECTOR3 vec3d, int* x2dptr, int* y2dptr, int* validptr )
{

	D3DXMATRIX matProj;
    D3DXMatrixIdentity( &matProj );

	float fovrad2, h, w, Q;

	fovrad2 = (float)g_proj_fov * 0.5f * (float)DEG2PAI;
	w = 1.0f / tanf( fovrad2 );
	h = w;
	Q = (float)g_proj_far / ( (float)g_proj_far - (float)g_proj_near );

    matProj._11 = w;
    matProj._22 = h;
    matProj._33 = Q;
    matProj._34 = 1.0f;
    //matProj._43 = -Q * near_plane;//101.01
    matProj._43 = -Q * (float)g_proj_near;//101.01


	if( !s_hwnd ){
		DbgOut( "e3dhsp : GetScreenPos : hwnd NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RECT clirect;
	GetClientRect( s_hwnd, &clirect );
	DWORD dwClipWidth = (clirect.right - clirect.left) / 2;
	DWORD dwClipHeight = (clirect.bottom - clirect.top) / 2;


	CBSphere bs;
	bs.tracenter = vec3d;

	float aspect = 1.0f;//!!!!!!!!!

	int ret;
	ret = bs.Transform2ScreenPos( dwClipWidth, dwClipHeight, s_matView, matProj, aspect, x2dptr, y2dptr );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPos : bs Transform2ScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (*x2dptr == 0xFFFFFFFF) && (*y2dptr == 0xFFFFFFFF) ){
		*validptr = 0;
	}else{
		*validptr = 1;
	}

	return 0;
}


//E3DJointAddToTree hsid, parentjoint, childjoint, lastflag
EXPORT BOOL WINAPI E3DJointAddToTree( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2
	int parentjoint;
	parentjoint = hei->HspFunc_prm_getdi(0);

// 3
	int childjoint;
	childjoint = hei->HspFunc_prm_getdi(0);

// 4
	int lastflag;
	lastflag = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DJointAddToTree : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->JointAddToTree( parentjoint, childjoint, lastflag );
	if( ret ){
		DbgOut( "E3DJointAddToTree : hs JointAddToTree error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DJointRemoveFromParent hsid, rmjoint
//	　　子供ジョイントはそのまま
EXPORT BOOL WINAPI E3DJointRemoveFromParent( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
// 2
	int rmjoint;
	rmjoint = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DJointRemoveFromParent : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->JointRemoveFromParent( rmjoint );
	if( ret ){
		DbgOut( "E3DjointRemoveFromParent : hs JointRemoveFromParent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

//E3DJointRemake hsid
EXPORT BOOL WINAPI E3DJointRemake( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DJointRemake : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->JointRemake();
	if( ret ){
		DbgOut( "E3DJointRemake : hs JointRemake error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}


EXPORT BOOL WINAPI E3DSigImport( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSigImport : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DSigImport : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : SigLoad : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//3
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

//4
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);



	int offsetx;
	offsetx = hei->HspFunc_prm_getdi(0);
	int offsety;
	offsety = hei->HspFunc_prm_getdi(0);
	int offsetz;
	offsetz = hei->HspFunc_prm_getdi(0);


	int rotx;
	rotx = hei->HspFunc_prm_getdi(0);
	int roty;
	roty = hei->HspFunc_prm_getdi(0);
	int rotz;
	rotz = hei->HspFunc_prm_getdi(0);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////


	float fmult;
	fmult = (float)mult100 / 100.0f;


	D3DXVECTOR3 offset;
	offset.x = (float)offsetx;
	offset.y = (float)offsety;
	offset.z = (float)offsetz;

	D3DXVECTOR3 rot;
	rot.x = (float)rotx;
	rot.y = (float)roty;
	rot.z = (float)rotz;



	int ret = 0;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSigImport : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetResDir( tempname, curhs );//!!!!!!

	ret = curhs->ImportSigFile( tempname, adjustuvflag, fmult, offset, rot );
	if( ret ){
		DbgOut( "E3DSigImport : hs ImportSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object
	ret = Restore( curhs, -1 );
	if( ret ){
		DbgOut( "E3DSigImport : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////
	ret = curhs->SetCurrentBSphereData();
	if( ret ){
		DbgOut( "E3DSigImport : hs SetCurrentBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}


//E3DSigLoadFromBuf resdir, buf, bufsize, hsidptr, adjustuvflag
	//resdirは最後に\\を忘れずに。
EXPORT BOOL WINAPI E3DSigLoadFromBuf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSigLoadFromBuf : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DSigLoadFromBuf : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : SigLoad : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//2
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "e3dhsp : SigLoad : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSigLoadFromBuf : bufptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//3
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//4	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "e3dhsp : SigLoad : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSigLoadFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//5
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

//6
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	int ret;
	int temphsid = -1;
	ret = SigLoadFromBuf_F( tempname, bufptr, bufsize, adjustuvflag, mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoadFromBuf : SigLoadFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		*hsidptr = -1;
		return 1;
	}
	*hsidptr = temphsid;


	return 0;
}

int SigLoadFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, int mult100, int* hsidptr )
{

	float fmult;
	fmult = (float)mult100 / 100.0f;

	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	SetResDir( tempname, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit
	
	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->LoadSigFileFromBuf( bufptr, bufsize, adjustuvflag, fmult );
	if( ret ){
		DbgOut( "easy3d : E3DSigLoadFromBuf : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object

	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;
	
	return 0;
}



//E3DSigImportFromBuf hsid, resdir, buf, bufsize, adjustuvflag
EXPORT BOOL WINAPI E3DSigImportFromBuf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSigImportFromBuf : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DSigImportFromBuf : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DSigImportFromBuf : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//3
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "e3dhsp : E3DSigImportFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSigImportFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//4
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);


//5
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

//6
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);


	int offsetx;
	offsetx = hei->HspFunc_prm_getdi(0);
	int offsety;
	offsety = hei->HspFunc_prm_getdi(0);
	int offsetz;
	offsetz = hei->HspFunc_prm_getdi(0);


	int rotx;
	rotx = hei->HspFunc_prm_getdi(0);
	int roty;
	roty = hei->HspFunc_prm_getdi(0);
	int rotz;
	rotz = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	float fmult;
	fmult = (float)mult100 / 100.0f;

	D3DXVECTOR3 offset;
	offset.x = (float)offsetx;
	offset.y = (float)offsety;
	offset.z = (float)offsetz;

	D3DXVECTOR3 rot;
	rot.x = (float)rotx;
	rot.y = (float)roty;
	rot.z = (float)rotz;

	int ret = 0;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSigImportFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetResDir( tempname, curhs );//!!!!!!


//DWORD dwtime1, dwtime2, dwtime3;
//g_rtsc.Start();

	ret = curhs->ImportSigFileFromBuf( bufptr, bufsize, adjustuvflag, fmult, offset, rot );
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : hs ImportSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//dwtime1 = g_rtsc.Stop();

//g_rtsc.Start();

//Create 3D Object
	ret = Restore( curhs, -1 );
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//dwtime2 = g_rtsc.Stop();

//g_rtsc.Start();

/////
	ret = curhs->SetCurrentBSphereData();
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : hs SetCurrentBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//dwtime3 = g_rtsc.Stop();

//DbgOut( "e3dimportsig from buf : time1 %d, time2 %d, time3 %d\r\n", dwtime1, dwtime2, dwtime3 );
	
	return 0;
}


//E3DAddMotionFromBuf hsid, datakind, bufptr, bufsize, motcookie, maxnum
EXPORT BOOL WINAPI E3DAddMotionFromBuf( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	int datakind;
	datakind = hei->HspFunc_prm_getdi(0);

//3
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "e3dhsp : E3DAddMotionFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DAddMotionFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//4
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//5	
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMotionFromBuf : parameter type error !!!\n" );
		return 1;
	}
	int* cookieptr;
	cookieptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数
	
//6
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMotionFromBuf : parameter type error !!!\n" );
		return 1;
	}
	int* maxnumptr;
	maxnumptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	
	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotionFromBuf_F( hsid, datakind, bufptr, bufsize, &tempmotid, &tempnum );
	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : AddMotionFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		*cookieptr = -1;
		*maxnumptr = 0;
		return 1;
	}
	*cookieptr = tempmotid;
	*maxnumptr = tempnum;

	return 0;
}

int AddMotionFromBuf_F( int hsid, int datakind, char* bufptr, int bufsize, int* cookieptr, int* maxnumptr )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMotionFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
	if( datakind == 0 ){
		ret = curhs->LoadQuaFileFromBuf( bufptr, bufsize, cookieptr, maxnumptr );	
	}else{
		ret = curhs->LoadMotFileFromBuf( bufptr, bufsize, cookieptr, maxnumptr );
	}

	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : Load***File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DCheckFullScreenParams w, h, bits, validflag, validw, validh, validbits
EXPORT BOOL WINAPI E3DCheckFullScreenParams( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

//1
	int w;
	w = hei->HspFunc_prm_getdi(640);

//2
	int h;
	h = hei->HspFunc_prm_getdi(480);

//3
	int bits;
	bits = hei->HspFunc_prm_getdi(16);

//4
	curtype = *hei->nptype;
	int* validflag;
	if( curtype == 4 ){
		validflag = (int*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !validflag ){
			DbgOut( "e3dhsp : E3DCheckFullScreenParams : validflag error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCheckFullScreenParams : validflag parameter type error %d!!!\n", curtype );
		return 1;
	}

//5
	curtype = *hei->nptype;
	int* validw;
	if( curtype == 4 ){
		validw = (int*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !validw ){
			DbgOut( "e3dhsp : E3DCheckFullScreenParams : validw error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCheckFullScreenParams : validw parameter type error %d!!!\n", curtype );
		return 1;
	}

//6
	curtype = *hei->nptype;
	int* validh;
	if( curtype == 4 ){
		validh = (int*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !validh ){
			DbgOut( "e3dhsp : E3DCheckFullScreenParams : validh error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCheckFullScreenParams : validh parameter type error %d!!!\n", curtype );
		return 1;
	}


//7
	curtype = *hei->nptype;
	int* validbits;
	if( curtype == 4 ){
		validbits = (int*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !validbits ){
			DbgOut( "e3dhsp : E3DCheckFullScreenParams : validbits error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCheckFullScreenParams : validbits parameter type error %d!!!\n", curtype );
		return 1;
	}

////////	
	
	if ( *hei->er ) return *hei->er;		// エラーチェック

////////

	if( bits <= 16 ){
		bits = 16;
	}else if( (bits > 16) && (bits < 32) ){
		bits = 32;
	}else if( bits >= 32 ){
		bits = 32;
	}

	*validflag = 0;//!!!!!!!

	CD3DApplication* tmpapp;

	tmpapp = new CD3DApplication();
	if( !tmpapp ){
		DbgOut( "E3DCheckFullScreenParams : tmpapp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = tmpapp->CheckFullScreenParams( (DWORD)w, (DWORD)h, bits, validflag, validw, validh, validbits );
	if( ret ){
		DbgOut( "E3DCheckFullScreenParams : tmpapp CheckFullScreenParams error !!!\n" );
		_ASSERT( 0 );
		delete tmpapp;
		return 1;
	}

	delete tmpapp;

	return 0;
}

//static LPD3DXSPRITE	s_d3dxsprite = 0;
//static CMySprite* s_spritehead = 0;


int e3dInvalidateDeviceObject()
{
	int ret;

	
//handlerset	
	CHandlerSet* curhs = hshead;
	while( curhs ){
		ret = curhs->Invalidate();
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : curhs Invalidate error!!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curhs = curhs->next;
	}

//sprite
	if( s_d3dxsprite != NULL ){
		s_d3dxsprite->Release();
		s_d3dxsprite = NULL;
	}
	
	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		ret = cursprite->Invalidate();
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : cursprite InvalidateTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		cursprite = cursprite->next;
	}

//billboard
	if( g_bbhs ){
		ret = g_bbhs->Invalidate();
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : bbhs Invalidate error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//bgdisp restoreで壊す。


//font english
	//g_pD3DApp->InvalidateDeviceObjects();


//font fontid
	CXFont* curfont;
	curfont = xfonthead;
	while( curfont ){
		ret = curfont->Invalidate();
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : curfont Invalidate error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curfont = curfont->next;
	}

	return 0;
}


int e3dRestoreDeviceObject()
{
	int ret;
	HRESULT hr;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

//handlerset	
	CHandlerSet* curhs = hshead;
	while( curhs ){
		ret = SetResDir( curhs->m_resdir, curhs );
		_ASSERT( !ret );

		ret = Restore( curhs, -1 );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curhs = curhs->next;
	}
	
//sprite
	if( s_d3dxsprite == NULL ){
		hr = D3DXCreateSprite( pdev, &s_d3dxsprite );
		if( hr != D3D_OK ){
			DbgOut( "e3dRestoreDeviceObject : D3DXCreateSprite error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		ret = cursprite->RestoreTexture( pdev );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : cursprite RestoreTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		cursprite = cursprite->next;
	}

//billboard
	if( g_bbhs ){
		ret = SetResDir( g_bbhs->m_resdir, g_bbhs );
		_ASSERT( !ret );
		
		ret = Restore( g_bbhs, -1 );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : bbhs Restore error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//bgdisp  invalidateも行う。
	if( s_bgdisp ){
		CBgDisp2* newbg;
		newbg = new CBgDisp2( pdev, s_bgdisp->texname, s_bgdisp->texname2, s_bgdisp->uanime, s_bgdisp->vanime, s_bgdisp->col, s_bgdisp->isround );
		if( !newbg ){
			DbgOut( "e3dRestoreDeviceObject : newbg alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		delete s_bgdisp;//!!!
		s_bgdisp = newbg;

	/////////

		_ASSERT( s_hwnd );

		RECT clirect;
		GetClientRect( s_hwnd, &clirect );
		s_clientWidth = clirect.right - clirect.left;
		s_clientHeight = clirect.bottom - clirect.top;
	
	}

//font english
	//g_pD3DApp->RestoreDeviceObjects();


//font fontid

	CXFont* curfont;
	curfont = xfonthead;
	while( curfont ){
		ret = curfont->Restore( pdev );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : curfont Restore error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curfont = curfont->next;
	}

//renderstate
	SetDefaultRenderState();

	SetPartialRenderState( pdev );

	/***
	int i;
	int rsno;
	int curvalue;
	for( i = 0; i < 80; i++ ){
		rsno = g_rsindex[i];

		if( rsno >= 0 ){
			curvalue = g_renderstate[ rsno ];
			hr = pdev->SetRenderState( (D3DRENDERSTATETYPE)rsno, curvalue );
			if( hr != D3D_OK ){
				DbgOut( "e3d : RestoreDeviceObject : SetRenderState : %d error !!!\r\n", rsno );
			}

		}
	}
	***/

	return 0;
}

//E3DGetMaxMultiSampleNum bits, iswindowmode, multisamplenumptr
EXPORT BOOL WINAPI E3DGetMaxMultiSampleNum( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

//1
	int bits;
	bits = hei->HspFunc_prm_getdi(16);

//2
	int iswindowmode;
	iswindowmode = hei->HspFunc_prm_getdi(16);

//4
	curtype = *hei->nptype;
	int* numptr;
	if( curtype == 4 ){
		numptr = (int*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !numptr ){
			DbgOut( "e3dhsp : E3DGetMaxMultiSampleNum : numptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetMaxMultiSampleNum : numptr parameter type error %d!!!\n", curtype );
		return 1;
	}


////////	
	
	if ( *hei->er ) return *hei->er;		// エラーチェック

////////

	*numptr = 0;


	if( bits <= 16 ){
		bits = 16;
	}else if( (bits > 16) && (bits < 32) ){
		bits = 32;
	}else if( bits >= 32 ){
		bits = 32;
	}

	if( iswindowmode != 0 )
		iswindowmode = 1;


	CD3DApplication* tmpapp;
	tmpapp = new CD3DApplication();
	if( !tmpapp ){
		DbgOut( "E3DGetMaxMultiSampleNum : tmpapp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = tmpapp->GetMaxMultiSampleNum( bits, iswindowmode, numptr );
	if( ret ){
		DbgOut( "E3DGetMaxMultiSampleNum : tmpapp GetMaxMultiSampleNum error !!!\n" );
		_ASSERT( 0 );
		delete tmpapp;
		return 1;
	}

	delete tmpapp;

	return 0;
}

//E3DChkConfLineAndFace posx1, posy1, posz1, posx2, posy2, posz2, hsid, needtrans, 
//	partno, faceno, confx, confy, confz, nx10000, ny10000, nz10000, revfaceflag
EXPORT BOOL WINAPI E3DChkConfLineAndFace( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

// 1
	int posx1;
	posx1 = hei->HspFunc_prm_getdi(0);
// 2
	int posy1;
	posy1 = hei->HspFunc_prm_getdi(0);
// 3
	int posz1;
	posz1 = hei->HspFunc_prm_getdi(0);


// 4
	int posx2;
	posx2 = hei->HspFunc_prm_getdi(0);
// 5
	int posy2;
	posy2 = hei->HspFunc_prm_getdi(0);
// 6
	int posz2;
	posz2 = hei->HspFunc_prm_getdi(0);


// 7
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
// 8
	int needtrans;
	needtrans = hei->HspFunc_prm_getdi(0);

// 9
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* partptr;
	partptr = (int *)hei->HspFunc_prm_getv();
// 10
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* faceptr;
	faceptr = (int *)hei->HspFunc_prm_getv();


// 11
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* confxptr;
	confxptr = (int *)hei->HspFunc_prm_getv();
// 12
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* confyptr;
	confyptr = (int *)hei->HspFunc_prm_getv();
// 13
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* confzptr;
	confzptr = (int *)hei->HspFunc_prm_getv();



// 14
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* nxptr;
	nxptr = (int *)hei->HspFunc_prm_getv();
// 15
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* nyptr;
	nyptr = (int *)hei->HspFunc_prm_getv();
// 16
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* nzptr;
	nzptr = (int *)hei->HspFunc_prm_getv();




// 17
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfLineAndFace : parameter type error !!!\n" );
		return 1;
	}
	int* revfaceptr;
	revfaceptr = (int *)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfLineAndFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 srcpos1, srcpos2;
	D3DXVECTOR3 dstpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 dstn( 0.0f, 0.0f, 0.0f );

	srcpos1.x = (float)posx1;
	srcpos1.y = (float)posy1;
	srcpos1.z = (float)posz1;

	srcpos2.x = (float)posx2;
	srcpos2.y = (float)posy2;
	srcpos2.z = (float)posz2;

	D3DXVECTOR3 camdir;
	camdir = s_cameratarget - s_camerapos;

	int ret;
	ret = curhs->ChkConfLineAndFace( camdir, s_matView, srcpos1, srcpos2, needtrans, partptr, faceptr, &dstpos, &dstn, revfaceptr );
	if( ret ){
		DbgOut( "E3DChkConfLineAndFace : curhs PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confxptr = (int)dstpos.x;
	*confyptr = (int)dstpos.y;
	*confzptr = (int)dstpos.z;

	if( *partptr >= 0 ){
		D3DXVec3Normalize( &dstn, &dstn );
	}

	*nxptr = (int)(10000.0f * dstn.x);
	*nyptr = (int)(10000.0f * dstn.y);
	*nzptr = (int)(10000.0f * dstn.z);

	return 0;
}

//E3DPickFace hsid, pos2x, pos2y, maxdist, partnoptr, facenoptr, pos3xptr, pos3yptr, pos3zptr, nxptr, nyptr, nzptr, distptr, calcmode
EXPORT BOOL WINAPI E3DPickFace( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	int pos2x;
	pos2x = hei->HspFunc_prm_getdi(0);

//3
	int pos2y;
	pos2y = hei->HspFunc_prm_getdi(0);

//4
	int maxdist;
	maxdist = hei->HspFunc_prm_getdi(10000);

//5 : partptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* partptr;
	partptr = (int *)hei->HspFunc_prm_getv();

//6 : faceptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* faceptr;
	faceptr = (int *)hei->HspFunc_prm_getv();


//7 : pos3xptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* pos3xptr;
	pos3xptr = (int *)hei->HspFunc_prm_getv();

//8 : pos3yptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* pos3yptr;
	pos3yptr = (int *)hei->HspFunc_prm_getv();

//9 : pos3zptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* pos3zptr;
	pos3zptr = (int *)hei->HspFunc_prm_getv();


//10 : nxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* nxptr;
	nxptr = (int *)hei->HspFunc_prm_getv();

//11 : nyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* nyptr;
	nyptr = (int *)hei->HspFunc_prm_getv();

//12 : nzptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* nzptr;
	nzptr = (int *)hei->HspFunc_prm_getv();

//13 : distptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickFace : parameter type error !!!\n" );
		return 1;
	}
	int* distptr;
	distptr = (int *)hei->HspFunc_prm_getv();

//14
	int calcmode;
	calcmode = hei->HspFunc_prm_getdi(0);


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 pos3;
	D3DXVECTOR3 nvec;
	float fmaxdist;

	if( maxdist < 0 ){
		fmaxdist = 10000.0f;
	}else{
		fmaxdist = (float)maxdist;
	}

	float fdist;

	if( calcmode == 1 ){
		CalcMatView();//!!!!!!!
	}

	int ret;
	ret = curhs->PickFace( s_matView, fmaxdist, pos2x, pos2y, partptr, faceptr, &pos3, &nvec, &fdist, calcmode );
	if( ret ){
		DbgOut( "E3DPickFace : curhs PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*pos3xptr = (int)pos3.x;
	*pos3yptr = (int)pos3.y;
	*pos3zptr = (int)pos3.z;

	if( *partptr >= 0 ){
		D3DXVec3Normalize( &nvec, &nvec );
	}

	*nxptr = (int)(10000.0f * nvec.x);
	*nyptr = (int)(10000.0f * nvec.y);
	*nzptr = (int)(10000.0f * nvec.z);

	if( *partptr < 0 ){
		*distptr = 900000000;
	}else{
		*distptr = (int)fdist;
	}

	return 0;
}


//E3DGetBBox hsid, partno, mode, minx, maxx, miny, maxy, minz, maxz
EXPORT BOOL WINAPI E3DGetBBox( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3
	int mode;
	mode = hei->HspFunc_prm_getdi(0);

//4 : minxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBBox : parameter type error !!!\n" );
		return 1;
	}
	int* minxptr;
	minxptr = (int *)hei->HspFunc_prm_getv();

//5 : maxxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBBox : parameter type error !!!\n" );
		return 1;
	}
	int* maxxptr;
	maxxptr = (int *)hei->HspFunc_prm_getv();

//6 : minyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBBox : parameter type error !!!\n" );
		return 1;
	}
	int* minyptr;
	minyptr = (int *)hei->HspFunc_prm_getv();

//7 : maxyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBBox : parameter type error !!!\n" );
		return 1;
	}
	int* maxyptr;
	maxyptr = (int *)hei->HspFunc_prm_getv();

//8 : minxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBBox : parameter type error !!!\n" );
		return 1;
	}
	int* minzptr;
	minzptr = (int *)hei->HspFunc_prm_getv();

//9 : maxxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBBox : parameter type error !!!\n" );
		return 1;
	}
	int* maxzptr;
	maxzptr = (int *)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBBox : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fminx, fmaxx, fminy, fmaxy, fminz, fmaxz;
	int ret;

	ret = curhs->GetBBox( partno, mode, &fminx, &fmaxx, &fminy, &fmaxy, &fminz, &fmaxz );
	if( ret ){
		DbgOut( "E3DGetBBox : curhs GetBBox error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*minxptr = (int)fminx;
	*maxxptr = (int)fmaxx;

	*minyptr = (int)fminy;
	*maxyptr = (int)fmaxy;

	*minzptr = (int)fminz;
	*maxzptr = (int)fmaxz;

	return 0;
}

//E3DGetVertNoOfFace hsid, partno, faceno, vert1, vert2, vert3
EXPORT BOOL WINAPI E3DGetVertNoOfFace( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3
	int faceno;
	faceno = hei->HspFunc_prm_getdi(0);

//4 : vert1ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVertNoOfFace : parameter type error !!!\n" );
		return 1;
	}
	int* vert1ptr;
	vert1ptr = (int *)hei->HspFunc_prm_getv();

//5 : vert2ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVertNoOfFace : parameter type error !!!\n" );
		return 1;
	}
	int* vert2ptr;
	vert2ptr = (int *)hei->HspFunc_prm_getv();


//6 : vert3ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVertNoOfFace : parameter type error !!!\n" );
		return 1;
	}
	int* vert3ptr;
	vert3ptr = (int *)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertNoOfFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*vert1ptr = -1;
	*vert2ptr = -1;
	*vert3ptr = -1;

	int ret;
	ret = curhs->GetVertNoOfFace( partno, faceno, vert1ptr, vert2ptr, vert3ptr );
	if( ret ){
		DbgOut( "E3DGetVertNoOfFace : hs GetVertNoOfFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DGetSamePosVert hsid, partno, vertno, sameptr, arrayleng, samenum
EXPORT BOOL WINAPI E3DGetSamePosVert( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : sameptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSamePosVert : parameter type error !!!\n" );
		return 1;
	}
	int* sameptr;
	sameptr = (int *)hei->HspFunc_prm_getv();

//5
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);


//6 : samenumptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSamePosVert : parameter type error !!!\n" );
		return 1;
	}
	int* samenumptr;
	samenumptr = (int *)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	*samenumptr = 0;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetSamePosVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->GetSamePosVert( partno, vertno, sameptr, arrayleng, samenumptr );
	if( ret ){
		DbgOut( "E3DGetSamePosVert : hs GetSamePosVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DRdtscStart
EXPORT BOOL WINAPI E3DRdtscStart( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	if ( *hei->er ) return *hei->er;		// エラーチェック
///////

	g_rtsc.Start();

	return 0;
}



//E3DRdtscStop time
EXPORT BOOL WINAPI E3DRdtscStop( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 : timeptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DRdtscStop : parameter type error !!!\n" );
		return 1;
	}
	int* timeptr;
	timeptr = (int *)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////


	DWORD dwret;
	dwret = g_rtsc.Stop();

	*timeptr = (int)dwret;
	return 0;
}

//E3DSaveSig2Buf hsid, buf, bufsize, writesize 
//bufsizeに０を指定した場合には、writesizeに必要なバッファ長を代入するのみ
EXPORT BOOL WINAPI E3DSaveSig2Buf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : buf
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSaveSig2Buf : parameter type error !!!\n" );
		return 1;
	}
	char* buf;
	buf = (char*)hei->HspFunc_prm_getv();

//3 : bufsize
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//4 : writesize
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSaveSig2Buf : parameter type error !!!\n" );
		return 1;
	}
	int* writesize;
	writesize = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	if( !writesize ){
		DbgOut( "E3DSaveSig2Buf : writesizeptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	*writesize = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveSig2Buf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SaveSig2Buf( buf, bufsize, writesize );
	if( ret ){
		DbgOut( "E3DSaveSig2Buf : hs SaveSig2Buf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSaveQua2Buf hsid, mkid, buf, bufsize, writesize
//bufsizeに０を指定した場合には、writesizeに必要なバッファ長を代入するのみ
EXPORT BOOL WINAPI E3DSaveQua2Buf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 mkid
	int mkid;
	mkid = hei->HspFunc_prm_getdi(0);

//3 : buf
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSaveQua2Buf : parameter type error !!!\n" );
		return 1;
	}
	char* buf;
	buf = (char*)hei->HspFunc_prm_getv();

//4 : bufsize
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//5 : writesize
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DSaveQua2Buf : parameter type error !!!\n" );
		return 1;
	}
	int* writesize;
	writesize = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	if( !writesize ){
		DbgOut( "E3DSaveQua2Buf : writesizeptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	*writesize = 0;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveSig2Buf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SaveQua2Buf( mkid, buf, bufsize, writesize );
	if( ret ){
		DbgOut( "E3DSaveSig2Buf : hs SaveSig2Buf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DCameraShiftLeft	shift
EXPORT BOOL WINAPI E3DCameraShiftLeft( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1 hsid
	int shift;
	shift = hei->HspFunc_prm_getdi(0);

////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	float fshift = (float)shift;

	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	CQuaternion2 rotq;
	D3DXVECTOR3 rotaxis( 0.0f, 1.0f, 0.0f );
	float rotrad = 90.0f * (float)DEG2PAI;
	rotq.SetAxisAndRot( rotaxis, rotrad );

	CQuaternion2 newq;
	newq = invview * rotq;
	newq.normalize();

	D3DXVECTOR3 zdir( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 shiftdir;
	

	newq.Rotate( &shiftdir, zdir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

//E3DCameraShiftRight shift
EXPORT BOOL WINAPI E3DCameraShiftRight( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1 hsid
	int shift;
	shift = hei->HspFunc_prm_getdi(0);

////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	float fshift = (float)shift;

	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	CQuaternion2 rotq;
	D3DXVECTOR3 rotaxis( 0.0f, 1.0f, 0.0f );
	float rotrad = -90.0f * (float)DEG2PAI;
	rotq.SetAxisAndRot( rotaxis, rotrad );

	CQuaternion2 newq;
	newq = invview * rotq;
	newq.normalize();

	D3DXVECTOR3 zdir( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 shiftdir;
	

	newq.Rotate( &shiftdir, zdir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}
//E3DCameraShiftUp shift
EXPORT BOOL WINAPI E3DCameraShiftUp( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1 hsid
	int shift;
	shift = hei->HspFunc_prm_getdi(0);

////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	float fshift = (float)shift;

	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	D3DXVECTOR3 ydir( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 shiftdir;
	

	invview.Rotate( &shiftdir, ydir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

//E3DCameraShiftDown shift
EXPORT BOOL WINAPI E3DCameraShiftDown( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1 hsid
	int shift;
	shift = hei->HspFunc_prm_getdi(0);

////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	float fshift = (float)shift;

	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	D3DXVECTOR3 ydir( 0.0f, -1.0f, 0.0f );
	D3DXVECTOR3 shiftdir;
	

	invview.Rotate( &shiftdir, ydir );
	s_camerapos = s_camerapos + fshift * shiftdir;
	s_cameratarget = s_cameratarget + fshift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

//E3DGetCameraQ dstqid
EXPORT BOOL WINAPI E3DGetCameraQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1 hsid
	int dstqid;
	dstqid = hei->HspFunc_prm_getdi(0);

////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DGetCameraQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( dstqid, &qptr );
	if( ret ){
		DbgOut( "E3DGetCameraQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	D3DXMATRIX vm;
	vm = s_matView;
	vm._41 = 0.0f;
	vm._42 = 0.0f;
	vm._43 = 0.0f;

	D3DXQUATERNION vmq;
	D3DXQuaternionRotationMatrix( &vmq, &vm );

	qptr->x = vmq.x;
	qptr->y = vmq.y;
	qptr->z = vmq.z;
	qptr->w = vmq.w;

	return 0;
}


//E3DInvQ srcqid, dstqid
EXPORT BOOL WINAPI E3DInvQ( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int srcqid;
	srcqid = hei->HspFunc_prm_getdi(0);
//2
	int dstqid;
	dstqid = hei->HspFunc_prm_getdi(0);

////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DInvQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( srcqid, &srcqptr );
	if( ret ){
		DbgOut( "E3DInvQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CQuaternion2* dstqptr;
	ret = s_qh->GetQ( dstqid, &dstqptr );
	if( ret ){
		DbgOut( "E3DInvQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	srcqptr->inv( dstqptr );

	return 0;
}

EXPORT BOOL WINAPI E3DSetCameraTwist( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//1
	int twistdeg;
	twistdeg = hei->HspFunc_prm_getdi(0);
//2
	int mult;
	mult = hei->HspFunc_prm_getdi(0);

////
	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	if( mult != 0 ){
		s_camera_twist = (float)twistdeg / (float)mult;
	}else{
		s_camera_twist = (float)twistdeg;
	}

	return 0;
}

int CalcMatView()
{
	float fDeg2Pai = (float)DEG2PAI;
	
	if( s_cameratype == 0 ){
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

		s_cameratarget.x = s_camerapos.x + sinf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );
		s_cameratarget.y = s_camerapos.y + sinf( s_camera_degy * fDeg2Pai );
		s_cameratarget.z = s_camerapos.z - cosf( s_camera_degxz * fDeg2Pai ) * cosf( s_camera_degy * fDeg2Pai );

		D3DXMATRIX tempmatView;
		float cx, cy, cz;

		D3DXMatrixLookAtLH( &tempmatView, &s_camerapos, &s_cameratarget, &vUpVec );
		cx = tempmatView._41;
		cy = tempmatView._42;
		cz = tempmatView._43;
		tempmatView._41 = 0.0f;
		tempmatView._42 = 0.0f;
		tempmatView._43 = 0.0f;

		D3DXVECTOR3 axisvec;
		CQuaternion2 addrot;
		axisvec = s_cameratarget - s_camerapos;
		DXVec3Normalize( &axisvec, &axisvec );
		addrot.SetAxisAndRot( axisvec, s_camera_twist * (float)DEG2PAI );
		
		D3DXMATRIX addrotmat;
		addrotmat = addrot.MakeRotMatX();

		s_matView = tempmatView * addrotmat;
		s_matView._41 = cx;
		s_matView._42 = cy; 
		s_matView._43 = cz;

	}else{

		D3DXMatrixLookAtLH( &s_matView, &s_camerapos, &s_cameratarget, &s_cameraupvec );
	}

	return 0;

}

//E3DIKRotateBeta hsid, jointno, motid, frameno, iklevel, axiskind, axisx, axisy, axisz, calclevel, targetx, targety, targetz, resx, resy, resz, lastparent
//	iklevel : 階層数
//	axiskind : 0-->視線に平行な軸
//	　　　　　　1-->指定した軸
//	　　　　　　2-->Auto選択
//	axisx, y, z : axiskindに１を指定したときのみ有効
//	calclevel : 計算の細かさ、数が大きいほど細かい。
//	targetx, y, z : ボーンの目標地点。
//	resx, y, z : 計算後のジョイントの位置。		
//	lastparent : 計算で変更のあった一番上の階層のジョイントの番号。E3DRenderに渡すと高速化出来る。
EXPORT BOOL WINAPI E3DIKRotateBeta( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 jointno
	int jointno;
	jointno = hei->HspFunc_prm_getdi(0);

//3 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//4 frameno
	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);

//5 iklevel
	int iklevel;
	iklevel = hei->HspFunc_prm_getdi(0);

//6 axiskind
	int axiskind;
	axiskind = hei->HspFunc_prm_getdi(0);

//7 axisx
	int axisx;
	axisx = hei->HspFunc_prm_getdi(0);
//8 axisy
	int axisy;
	axisy = hei->HspFunc_prm_getdi(0);
//9 axisz
	int axisz;
	axisz = hei->HspFunc_prm_getdi(0);

//10 calclevel
	int calclevel;
	calclevel = hei->HspFunc_prm_getdi(0);

//11 targetx
	int targetx;
	targetx = hei->HspFunc_prm_getdi(0);
//12 targety
	int targety;
	targety = hei->HspFunc_prm_getdi(0);
//13 targetz
	int targetz;
	targetz = hei->HspFunc_prm_getdi(0);

//14 : resx
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta : parameter type error !!!\n" );
		return 1;
	}
	int* resx;
	resx = (int*)hei->HspFunc_prm_getv();
//15 : resy
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta : parameter type error !!!\n" );
		return 1;
	}
	int* resy;
	resy = (int*)hei->HspFunc_prm_getv();
//16 : resz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta : parameter type error !!!\n" );
		return 1;
	}
	int* resz;
	resz = (int*)hei->HspFunc_prm_getv();

//17 : lastparent
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta : parameter type error !!!\n" );
		return 1;
	}
	int* lastparent;
	lastparent = (int*)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	D3DXVECTOR3 axisvec;
	switch( axiskind ){
	case 0:
		axisvec = s_cameratarget - s_camerapos;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 1:
		axisvec.x = (float)axisx;
		axisvec.y = (float)axisy;
		axisvec.z = (float)axisz;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 2:
		axisvec.x = 0.0f;//ダミーデータ
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	default:
		axiskind = 2;//!!!!!!
		axisvec.x = 0.0f;//ダミーデータ
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	}
	
	D3DXVECTOR3 targetpos;
	targetpos.x = (float)targetx;
	targetpos.y = (float)targety;
	targetpos.z = (float)targetz;

	D3DXVECTOR3 respos(0.0f, 0.0f, 0.0f);

//DbgOut( "E3DIKRotateBeta : axisvec %f %f %f\r\n", axisvec.x, axisvec.y, axisvec.z );

	ret = curhs->IKRotateBeta( s_camerapos, pdev, s_matView, jointno, motid, frameno, iklevel, axiskind, axisvec, calclevel, targetpos, &respos, lastparent );
	if( ret ){
		DbgOut( "E3DIKRotateBeta : hs IKRotateBeta error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resx = (int)respos.x;
	*resy = (int)respos.y;
	*resz = (int)respos.z;

	return 0;
}

//E3DIKRotateBeta2D hsid, jointno, motid, frameno, iklevel, axiskind, axisx, axisy, axisz, calclevel, 
//		target2dx, target2dy, resx, resy, resz, lastparent
EXPORT BOOL WINAPI E3DIKRotateBeta2D( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 jointno
	int jointno;
	jointno = hei->HspFunc_prm_getdi(0);

//3 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//4 frameno
	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);

//5 iklevel
	int iklevel;
	iklevel = hei->HspFunc_prm_getdi(0);

//6 axiskind
	int axiskind;
	axiskind = hei->HspFunc_prm_getdi(0);

//7 axisx
	int axisx;
	axisx = hei->HspFunc_prm_getdi(0);
//8 axisy
	int axisy;
	axisy = hei->HspFunc_prm_getdi(0);
//9 axisz
	int axisz;
	axisz = hei->HspFunc_prm_getdi(0);

//10 calclevel
	int calclevel;
	calclevel = hei->HspFunc_prm_getdi(0);

//11 target2dx
	int target2dx;
	target2dx = hei->HspFunc_prm_getdi(0);
//12 target2dy
	int target2dy;
	target2dy = hei->HspFunc_prm_getdi(0);

//13 : resx
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta2D : parameter type error !!!\n" );
		return 1;
	}
	int* resx;
	resx = (int*)hei->HspFunc_prm_getv();
//14 : resy
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta2D : parameter type error !!!\n" );
		return 1;
	}
	int* resy;
	resy = (int*)hei->HspFunc_prm_getv();
//15 : resz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta2D : parameter type error !!!\n" );
		return 1;
	}
	int* resz;
	resz = (int*)hei->HspFunc_prm_getv();

//16 : lastparent
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKRotateBeta2D : parameter type error !!!\n" );
		return 1;
	}
	int* lastparent;
	lastparent = (int*)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta2D : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	D3DXVECTOR3 axisvec;
	switch( axiskind ){
	case 0:
		axisvec = s_cameratarget - s_camerapos;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 1:
		axisvec.x = (float)axisx;
		axisvec.y = (float)axisy;
		axisvec.z = (float)axisz;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 2:
		axisvec.x = 0.0f;//ダミーデータ
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	default:
		axiskind = 2;//!!!!!!
		axisvec.x = 0.0f;//ダミーデータ
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	}
	
	D3DXVECTOR2 targetpos2d;
	targetpos2d.x = (float)target2dx;
	targetpos2d.y = (float)target2dy;

	D3DXVECTOR3 respos(0.0f, 0.0f, 0.0f);

/////////////////

	D3DXVECTOR3 mvobj;
	ret = curhs->GetBonePos( jointno, 1, motid, frameno, 1, &mvobj );
	if( ret ){
		DbgOut( "E3DIKRotateBeta2D : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	D3DXVECTOR3 targetsc, targetobj, targetobj0, targetobj1;
	
	targetsc.x = targetpos2d.x;
	targetsc.y = targetpos2d.y;
	targetsc.z = 0.0f;
	ret = ScreenToObj( &targetobj0, targetsc );
	if( ret )
		return 0;

	targetsc.x = targetpos2d.x;
	targetsc.y = targetpos2d.y;
	targetsc.z = 1.0f;
	ret = ScreenToObj( &targetobj1, targetsc );
	if( ret )
		return 0;

	D3DXVECTOR3 targetdir;
	targetdir = targetobj1 - targetobj0;
	DXVec3Normalize( &targetdir, &targetdir );


	D3DXVECTOR3 planeaxis;
	planeaxis = s_cameratarget - s_camerapos;	
	DXVec3Normalize( &planeaxis, &planeaxis );


	D3DXVECTOR3 temptarget;
	ret = GetShadow( &temptarget, planeaxis, mvobj, targetobj0, targetdir );
	if( ret )
		return 0;

	//targetobj = mvobj + ( temptarget - mvobj ) / (float)calclevel;


/////////////////



	ret = curhs->IKRotateBeta( s_camerapos, pdev, s_matView, jointno, motid, frameno, iklevel, axiskind, axisvec, calclevel, temptarget, &respos, lastparent );
	if( ret ){
		DbgOut( "E3DIKRotateBeta2D : hs IKRotateBeta error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resx = (int)respos.x;
	*resy = (int)respos.y;
	*resz = (int)respos.z;

	return 0;
}



//E3DGetMotiontype hsid, motid, typeptr
EXPORT BOOL WINAPI E3DGetMotionType( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3 : typeptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetMotionType : parameter type error !!!\n" );
		return 1;
	}
	int* typeptr;
	typeptr = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMotionType( motid, typeptr );
	if( ret ){
		DbgOut( "E3DGetMotionType : hs GetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

//E3DSetMotionType hsid, motid, type
EXPORT BOOL WINAPI E3DSetMotionType( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202



//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3 type
	int type;
	type = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->SetMotionType( motid, type );
	if( ret ){
		DbgOut( "E3DSetMotionType : hs SetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

//E3DGetIKTransFlag hsid, jointno, flagptr
EXPORT BOOL WINAPI E3DGetIKTransFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 jointno
	int jointno;
	jointno = hei->HspFunc_prm_getdi(0);

//3 : flagptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DIKTransFlag : parameter type error !!!\n" );
		return 1;
	}
	int* flagptr;
	flagptr = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIKTransFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetIKTransFlag( jointno, flagptr );
	if( ret ){
		DbgOut( "E3DGetIKTransFlag : hs GetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DSetIKTransFlag hsid, jointno, flag
EXPORT BOOL WINAPI E3DSetIKTransFlag( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202



//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 jointno
	int jointno;
	jointno = hei->HspFunc_prm_getdi(0);

//3 flag
	int flag;
	flag = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKTransFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetIKTransFlag( jointno, flag );
	if( ret ){
		DbgOut( "E3DSetIKTransFlag : hs SetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DDestroyAllMotion hsid
EXPORT BOOL WINAPI E3DDestroyAllMotion( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyAllMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->DestroyAllMotion();
	if( ret ){
		DbgOut( "E3DDestroyAllMotion : hs DestroyAllMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;
}


	
//E3DGetUserInt1OfPart hsid, partno, userintptr
EXPORT BOOL WINAPI E3DGetUserInt1OfPart( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : userint1ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetUserInt1OfPart : parameter type error !!!\n" );
		return 1;
	}
	int* userint1ptr;
	userint1ptr = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetUserInt1OfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetUserInt1OfPart( partno, userint1ptr );
	if( ret ){
		DbgOut( "E3DGetUserInt1OfPart : hs GetUserInt1OfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetUserInt1OfPart hsid, partno, userint
EXPORT BOOL WINAPI E3DSetUserInt1OfPart( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202



//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 userint1
	int userint1;
	userint1 = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetUserInt1OfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUserInt1OfPart( partno, userint1 );
	if( ret ){
		DbgOut( "E3DSetUserInt1OfPart : hs SetUserInt1OfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetBSphere hsid, partno, cenx, ceny, cenz, r
EXPORT BOOL WINAPI E3DGetBSphere( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : cenxptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBSphere : parameter type error !!!\n" );
		return 1;
	}
	int* cenxptr;
	cenxptr = (int*)hei->HspFunc_prm_getv();

//4 : cenyptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBSphere : parameter type error !!!\n" );
		return 1;
	}
	int* cenyptr;
	cenyptr = (int*)hei->HspFunc_prm_getv();

//5 : cenzptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBSphere : parameter type error !!!\n" );
		return 1;
	}
	int* cenzptr;
	cenzptr = (int*)hei->HspFunc_prm_getv();

//6 : rptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetBSphere : parameter type error !!!\n" );
		return 1;
	}
	int* rptr;
	rptr = (int*)hei->HspFunc_prm_getv();
	
////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBSphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 dstcenter;
	float dstr;

	ret = curhs->GetBSphere( partno, &dstcenter, &dstr );
	if( ret ){
		DbgOut( "E3DGetBSphere : hs GetBSphere error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*cenxptr = (int)dstcenter.x;
	*cenyptr = (int)dstcenter.y;
	*cenzptr = (int)dstcenter.z;

	*rptr = (int)dstr;

	return 0;
}


//GetChildJoint hsid, parentno, arrayleng, childarray, childnum
EXPORT BOOL WINAPI E3DGetChildJoint( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 parentno
	int parentno;
	parentno = hei->HspFunc_prm_getdi(0);

//3 arrayleng
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);


//4 : childarray
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetChildJoint : parameter type error !!!\n" );
		return 1;
	}
	int* childarray;
	childarray = (int*)hei->HspFunc_prm_getv();


//5 : childnum
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetChildJoint : parameter type error !!!\n" );
		return 1;
	}
	int* childnum;
	childnum = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBSphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetChildJoint( parentno, arrayleng, childarray, childnum );
	if( ret ){
		DbgOut( "E3DGetChildJoint : hs GetChildJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DDestroyMotionFrame hsid, motid, frameno
EXPORT BOOL WINAPI E3DDestroyMotionFrame( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202


//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3 frameno
	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBSphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DestroyMotionFrame( motid, frameno );
	if( ret ){
		DbgOut( "E3DDestroyMotionFrame : hs DestroyMotionFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetKeyFrameNo( int no, int hsid, int motid, int boneno, int* framearray, int arrayleng, int* framenumptr );
EXPORT BOOL WINAPI E3DGetKeyFrameNo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);

//3 boneno
	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);


//4 : framearray
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetChildJoint : parameter type error !!!\n" );
		return 1;
	}
	int* framearray;
	framearray = (int*)hei->HspFunc_prm_getv();


//5 arrayleng
	int arrayleng;
	arrayleng = hei->HspFunc_prm_getdi(0);


//6 : framenumptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetChildJoint : parameter type error !!!\n" );
		return 1;
	}
	int* framenumptr;
	framenumptr = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetKeyFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->HuGetKeyFrameNo( motid, boneno, framearray, arrayleng, framenumptr );
	if( ret ){
		DbgOut( "E3DGetKeyframeNo : hs HuGetKeyFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	return 0;
}


//E3DConvScreenTo3D x2d, y2d, z100000, x3d, y3d, z3d
EXPORT BOOL WINAPI E3DConvScreenTo3D( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 x2d
	int x2d;
	x2d = hei->HspFunc_prm_getdi(0);
//2 y2d
	int y2d;
	y2d = hei->HspFunc_prm_getdi(0);
//3 z100000
	int z100000;
	z100000 = hei->HspFunc_prm_getdi(0);




//4 : x3d
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DConvScreenTo3D : parameter type error !!!\n" );
		return 1;
	}
	int* x3d;
	x3d = (int*)hei->HspFunc_prm_getv();

//5 : y3d
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DConvScreenTo3D : parameter type error !!!\n" );
		return 1;
	}
	int* y3d;
	y3d = (int*)hei->HspFunc_prm_getv();

//6 : z3d
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DConvScreenTo3D : parameter type error !!!\n" );
		return 1;
	}
	int* z3d;
	z3d = (int*)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	D3DXVECTOR3 scpos;
	D3DXVECTOR3 objpos( 0.0f, 0.0f, 0.0f );

	scpos.x = (float)x2d;
	scpos.y = (float)y2d;
	scpos.z = (float)z100000 / 100000.0f;

	ret = ScreenToObj( &objpos, scpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DConvScreenTo3D ; ScreenToObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

//DbgOut( "e3dhsp : E3DConvScreenTo3D : scpos %f %f %f, objpos %f %f %f\r\n",
//	   scpos.x, scpos.y, scpos.z,
//	   objpos.x, objpos.y, objpos.z );


	*x3d = (int)objpos.x;
	*y3d = (int)objpos.y;
	*z3d = (int)objpos.z;

//DbgOut( "e3dhsp : E3DConvScreenTo3D : scpos %f %f %f, objpos %f %f %f, ret %d %d %d\r\n",
//	   scpos.x, scpos.y, scpos.z,
//	   objpos.x, objpos.y, objpos.z,
//	   *x3d, *y3d, *z3d );


	return 0;
}


int ScreenToObj( D3DXVECTOR3* objpos, D3DXVECTOR3 scpos )
{

	float clipw, cliph, asp;

	RECT clirect;
	GetClientRect( s_hwnd, &clirect );
	clipw = ( clirect.right - clirect.left ) / 2.0f;
	cliph = ( clirect.bottom - clirect.top ) / 2.0f;

	/***
	clipw = (float)s_clientWidth / 2.0f;
	cliph = (float)s_clientHeight / 2.0f;
	//Resotreをまだ実行していない可能性があるので、s_clientWidthは、使えない。
	***/
  
	asp = 1.0f;//!!!!!!!!!!!!



	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 正射影を有効にする場合には、aspを以下のように設定する必要有り！！！
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/***
	//float calaspect;
	if( m_matProj._43 < -1.0f ){
		asp = m_shandler->m_aspect;
	}else{
		//正射影
		asp = clipw / cliph;
	}
	***/



	D3DXMATRIX mat, invmat;
	D3DXMATRIX* retmat;

	//mat = m_matWorld * m_matView * m_matProj;

	CalcMatView();


	D3DXMATRIX matProj;
    D3DXMatrixIdentity( &matProj );
	float fovrad2, h, w, Q;
	fovrad2 = (float)g_proj_fov * 0.5f * (float)DEG2PAI;
	w = 1.0f / tanf( fovrad2 );
	h = w;
	Q = (float)g_proj_far / ( (float)g_proj_far - (float)g_proj_near );
	
    matProj._11 = w;
    matProj._22 = h;
    matProj._33 = Q;
    matProj._34 = 1.0f;
    //matProj._43 = -Q * near_plane;//101.01
    matProj._43 = -Q * (float)g_proj_near;//101.01
		

	mat = s_matView * matProj;//world == ini

	retmat = D3DXMatrixInverse( &invmat, NULL, &mat );
	if( retmat == NULL ){
		_ASSERT( 0 );
		return 1;
	}
	

	float res1;
	D3DXVECTOR3 screenpos;

	screenpos.x = ( scpos.x - clipw ) / ( asp * cliph );
	screenpos.y = ( cliph - scpos.y ) / cliph;
	screenpos.z = scpos.z;

	objpos->x = screenpos.x * invmat._11 + screenpos.y * invmat._21 + screenpos.z * invmat._31 + invmat._41;
	objpos->y = screenpos.x * invmat._12 + screenpos.y * invmat._22 + screenpos.z * invmat._32 + invmat._42;
	objpos->z = screenpos.x * invmat._13 + screenpos.y * invmat._23 + screenpos.z * invmat._33 + invmat._43;
	res1 =		screenpos.x * invmat._14 + screenpos.y * invmat._24 + screenpos.z * invmat._34 + invmat._44;

	
	if( res1 == 0.0f ){
		_ASSERT( 0 );
		return 1;
	}

	objpos->x /= res1;
	objpos->y /= res1;
	objpos->z /= res1;


	return 0;
}


EXPORT BOOL WINAPI E3DVec3Length( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 vecx
	int vecx;
	vecx = hei->HspFunc_prm_getdi(0);
//2 vecy
	int vecy;
	vecy = hei->HspFunc_prm_getdi(0);
//3 vecz
	int vecz;
	vecz = hei->HspFunc_prm_getdi(0);

//4 : lengptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DConvScreenTo3D : parameter type error !!!\n" );
		return 1;
	}
	int* lengptr;
	lengptr = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	D3DXVECTOR3 srcvec;

	srcvec.x = (float)vecx;
	srcvec.y = (float)vecy;
	srcvec.z = (float)vecz;

	float fleng;
	fleng = D3DXVec3Length( &srcvec );

	*lengptr = (int)fleng;
	
	return 0;
}

//E3DSetUV hsid, partno, vertno, u10000, v10000, setflag
EXPORT BOOL WINAPI E3DSetUV( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	//int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
//2 partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);
//3 vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);


//4 u10000
	int u10000;
	u10000 = hei->HspFunc_prm_getdi(0);
//5 v10000
	int v10000;
	v10000 = hei->HspFunc_prm_getdi(0);

//6 setflag
	int setflag;
	setflag = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetUV : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fu, fv;
	fu = (float)u10000 / 10000.0f;
	fv = (float)v10000 / 10000.0f;


	ret = curhs->SetUV( partno, vertno, fu, fv, setflag );
	if( ret ){
		DbgOut( "E3DSetUV : hs SetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	return 0;
}


//E3DGetUV hsid, partno, vertno, u10000, v10000
EXPORT BOOL WINAPI E3DGetUV( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
//2 partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);
//3 vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);



//4 : u10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetUV : parameter type error !!!\n" );
		return 1;
	}
	int* u10000ptr;
	u10000ptr = (int*)hei->HspFunc_prm_getv();


//5 : v10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetUV : parameter type error !!!\n" );
		return 1;
	}
	int* v10000ptr;
	v10000ptr = (int*)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetUV : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fu, fv;

	ret = curhs->GetUV( partno, vertno, &fu, &fv );
	if( ret ){
		DbgOut( "E3DGetUV : hs GetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*u10000ptr = (int)( fu * 10000.0f );
	*v10000ptr = (int)( fv * 10000.0f );

	return 0;
}


int GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir )
{

		//planepを通り、planedirを法線ベクトルとする平面：ax + by + cz + d = 0;
	float a, b, c, d;
	a = planedir.x; b = planedir.y; c = planedir.z;
	d = -a * planep.x - b * planep.y - c * planep.z;

		//平面ax+by+cz+d=0と、点pobj1を通り方向ベクトルdirectionの直線との交点、targetを求める。
		//OpenGL赤本p402参照
	D3DXMATRIX mat;
	mat._11 = b * srcdir.y + c * srcdir.z;
	mat._12 = -a * srcdir.y;
	mat._13 = -a * srcdir.z;
	mat._14 = 0.0f;

	mat._21 = -b * srcdir.x;
	mat._22 = a * srcdir.x + c * srcdir.z;
	mat._23 = -b * srcdir.z;
	mat._24 = 0.0f;

	mat._31 = -c * srcdir.x;
	mat._32 = -c * srcdir.y;
	mat._33 = a * srcdir.x + b * srcdir.y;
	mat._34 = 0.0f;

	mat._41 = -d * srcdir.x;
	mat._42 = -d * srcdir.y;
	mat._43 = -d * srcdir.z;
	mat._44 = a * srcdir.x + b * srcdir.y + c * srcdir.z;

	if( mat._44 == 0.0f )
		return 1;

	dstshadow->x = (srcp.x * mat._11 + srcp.y * mat._21 + srcp.z * mat._31 + mat._41) / mat._44;
	dstshadow->y = (srcp.x * mat._12 + srcp.y * mat._22 + srcp.z * mat._32 + mat._42) / mat._44;
	dstshadow->z = (srcp.x * mat._13 + srcp.y * mat._23 + srcp.z * mat._33 + mat._43) / mat._44;


	return 0;
}


//E3DPickBone hsid, posx, posy, boneno
EXPORT BOOL WINAPI E3DPickBone( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);


//2 posx
	int posx;
	posx = hei->HspFunc_prm_getdi(0);
//3 posy
	int posy;
	posy = hei->HspFunc_prm_getdi(0);



//4 : bonenoptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DPickBone : parameter type error !!!\n" );
		return 1;
	}
	int* bonenoptr;
	bonenoptr = (int*)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->PickBone( posx, posy, bonenoptr );
	if( ret ){
		DbgOut( "E3DPickBone : hs PickBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DShiftBoneTree2D hsid, jointno, motid, frameno, target2dx, target2dy 
EXPORT BOOL WINAPI E3DShiftBoneTree2D( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 jointno
	int jointno;
	jointno = hei->HspFunc_prm_getdi(0);

//3 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);
//4 frameno
	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);


//5 target2dx
	int target2dx;
	target2dx = hei->HspFunc_prm_getdi(0);
//6 target2dy
	int target2dy;
	target2dy = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DShiftBoneTree2D : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	D3DXVECTOR3 mvobj;
	ret = curhs->GetBonePos( jointno, 1, motid, frameno, 1, &mvobj );
	if( ret ){
		DbgOut( "E3DShiftBoneTree2D : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	D3DXVECTOR3 targetsc, targetobj, targetobj0, targetobj1;
	
	targetsc.x = (float)target2dx;
	targetsc.y = (float)target2dy;
	targetsc.z = 0.0f;
	ret = ScreenToObj( &targetobj0, targetsc );
	if( ret )
		return 0;

	targetsc.x = (float)target2dx;
	targetsc.y = (float)target2dy;
	targetsc.z = 1.0f;
	ret = ScreenToObj( &targetobj1, targetsc );
	if( ret )
		return 0;

	D3DXVECTOR3 targetdir;
	targetdir = targetobj1 - targetobj0;
	DXVec3Normalize( &targetdir, &targetdir );


	D3DXVECTOR3 planeaxis;
	planeaxis = s_cameratarget - s_camerapos;	
	DXVec3Normalize( &planeaxis, &planeaxis );


	D3DXVECTOR3 temptarget;
	ret = GetShadow( &temptarget, planeaxis, mvobj, targetobj0, targetdir );
	if( ret )
		return 0;

	D3DXVECTOR3 mvdiff;
	mvdiff = temptarget - mvobj;

/////////////
	ret = curhs->ShiftBoneTreeDiff( jointno, mvdiff, motid, frameno );
	if( ret ){
		DbgOut( "E3DShiftBoneTree2D : hs ShiftBoneTreeDiff error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetDispSwitch hsid, motid, frameno, dswitch
EXPORT BOOL WINAPI E3DGetDispSwitch( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);


//2 motid
	int motid;
	motid = hei->HspFunc_prm_getdi(0);
//3 frameno
	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);


//4 : dswitch
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetDispSwitch : parameter type error !!!\n" );
		return 1;
	}
	int* dswitch;
	dswitch = (int*)hei->HspFunc_prm_getv();


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispSwitch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetDispSwitch( motid, frameno, dswitch );
	if( ret ){
		DbgOut( "E3DGetDispSwitch : hs GetDispSwitch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DRotateBillboard bbid, rotdeg, mult, rotkind
EXPORT BOOL WINAPI E3DRotateBillboard( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 bbid
	int bbid;
	bbid = hei->HspFunc_prm_getdi(0);

//2 rotdeg
	int rotdeg;
	rotdeg = hei->HspFunc_prm_getdi(0);

//3 mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

//4 rotkind
	int rotkind;
	rotkind = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////


	int ret;
	if( !g_bbhs ){
		DbgOut( "E3DRotateBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fdeg;
	if( mult != 0 )
		fdeg = (float)rotdeg / (float)mult;
	else
		fdeg = (float)rotdeg;


	ret = g_bbhs->RotateBillboard( bbid, fdeg, rotkind );
	if( ret ){
		DbgOut( "E3DRotateBillboard : bbhs RotateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetBillboardUV bbid, unum, vnum, texno
EXPORT BOOL WINAPI E3DSetBillboardUV( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 bbid
	int bbid;
	bbid = hei->HspFunc_prm_getdi(0);

//2 unum
	int unum;
	unum = hei->HspFunc_prm_getdi(1);

//3 vnum
	int vnum;
	vnum = hei->HspFunc_prm_getdi(1);

//4 texno
	int texno;
	texno = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////

	if( unum == 0 )
		unum = 1;
	if( vnum == 0 )
		vnum = 1;

	int ret;
	if( !g_bbhs ){
		DbgOut( "E3DSetBillboardUV : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_bbhs->SetBillboardUV( bbid, unum, vnum, texno );
	if( ret ){
		DbgOut( "E3DSetBillboardUV : bbhs SetBillboardUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int DestroyTexture( int hsid, int bbid, int spriteid )
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//bbid == -1 ＢＢ全て削除、bbid == -2 ＢＢは削除しない
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	int ret;

	CHandlerSet* delhs;
	if( hsid >= 0 ){
		delhs = GetHandlerSet( hsid );
		_ASSERT( delhs );
	}else{
		delhs = 0;
	}

	CMySprite* delsprite;
	if( spriteid >= 0 ){
		delsprite = GetSprite( spriteid );
		_ASSERT( delsprite );
	}else{
		delsprite = 0;
	}
	
//////////

EnterCriticalSection( &g_crit_restore );//######## start crit

	CHandlerSet* curhs;
	curhs = hshead;
	while( curhs ){
		if( curhs != delhs ){
			ret = curhs->SetTextureDirtyFlag( -1 );//全てにdirtyflagセット　
			_ASSERT( !ret );
		}
		curhs = curhs->next;
	}

	if( g_bbhs ){
		if( bbid == -2 ){
			ret = g_bbhs->SetTextureDirtyFlag( -1 );//全てにdirtyflagセット
			_ASSERT( !ret );
		}else if( bbid == -1 ){
			//何もしない
		}else if( bbid >= 0 ){
			ret = g_bbhs->SetTextureDirtyFlag( bbid );//bbidにだけdirtyflagセット
			_ASSERT( !ret );
		}
	}

	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		if( cursprite != delsprite ){
			ret = cursprite->SetTextureDirtyFlag();
		}
		cursprite = cursprite->next;
	}

	//////////


	ret = g_texbnk->DestroyNotDirtyTexture( 1 );
	_ASSERT( !ret );

LeaveCriticalSection( &g_crit_restore );//###### end crit


	return 0;
}

//E3DCreateTexture　filename, pool, transparent, texid
EXPORT BOOL WINAPI E3DCreateTexture( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DCreateTexture : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}
	if( !nameptr ){
		DbgOut( "E3DCreateTexture : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "E3DCreateTexture : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//2 pool
	int pool;
	pool = hei->HspFunc_prm_getdi( D3DPOOL_MANAGED );


//3 transparent
	int transparent;
	transparent = hei->HspFunc_prm_getdi( 0 );


//4 texidptr
	curtype = *hei->nptype;
	int* texidptr;
	if( curtype == 4 ){
		texidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !texidptr ){
			DbgOut( "e3dhsp : CreateTexture : texidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCreateTexture : texidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	int ret;


	if( (pool != D3DPOOL_DEFAULT) && (pool != D3DPOOL_MANAGED) && (pool != D3DPOOL_SYSTEMMEM) ){
		pool = D3DPOOL_DEFAULT;
	}
	if( (transparent < 0) || (transparent >= 3) ){
		transparent = 0;
	}


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateTexture : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DCreateTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////
	int newserial = -1;
	int newindex = -1;
	char usertexname[ MAX_PATH + 256 ];
	s_usertexcnt++;


	int cmp1;
	cmp1 = strcmp( tempname, "MainWindow" );
	if( cmp1 != 0 ){
		
		sprintf( usertexname, "%s_TEXID%d", tempname, s_usertexcnt );		
	}else{
		pool = D3DPOOL_SYSTEMMEM;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!強制設定
		if( transparent == 1 ){
			transparent = 2;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!強制設定
		}
		sprintf( usertexname, "%s_%d", tempname, s_usertexcnt );
	}

EnterCriticalSection( &g_crit_restore );//######## start crit

	newserial = g_texbnk->AddName( usertexname, transparent, pool );
	if( newserial <= 0 ){
		DbgOut( "E3DCreateTexture : texbnk AddName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}
		
	newindex = g_texbnk->FindName( usertexname, transparent );
	if( newindex < 0 ){
		DbgOut( "E3DCreateTexture : texbnk FindName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}

	ret = g_texbnk->CreateTexData( newindex, pdev );
	if( ret ){
		DbgOut( "E3DCreateTexture : texbnk CreateTexData error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}


	*texidptr = newserial;//!!!!!!!

LeaveCriticalSection( &g_crit_restore );//###### end crit
	
	
	return 0;

/***
		filenameに、"MainWindow"を指定すると、バックバッファと同じ内容で、同じ大きさのテクスチャを作成。
			ただし、このときは、poolに何を指定しても、システムメモリに作られる。
			（サイズ制限のため）

		テクスチャ内部名は、"filename_TEXID%d", texid　にする。
		(または、"MainWindow_TEXID%d")

		D3DPOOL_DEFAULT                 = 0,
		D3DPOOL_MANAGED                 = 1,
		D3DPOOL_SYSTEMMEM               = 2,
***/

}

//E3DGetTextureInfo texid, width, height, pool, transparent, format
EXPORT BOOL WINAPI E3DGetTextureInfo( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
//1 texid
	int texid;
	texid = hei->HspFunc_prm_getdi( 0 );

//2 widthptr
	curtype = *hei->nptype;
	int* widthptr;
	if( curtype == 4 ){
		widthptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !widthptr ){
			DbgOut( "e3dhsp : GetTextureSize : widthptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetTextureInfo : widthptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//3 heightptr
	curtype = *hei->nptype;
	int* heightptr;
	if( curtype == 4 ){
		heightptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !heightptr ){
			DbgOut( "e3dhsp : GetTextureSize : heightptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetTextureInfo : heightptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//4 poolptr
	curtype = *hei->nptype;
	int* poolptr;
	if( curtype == 4 ){
		poolptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !poolptr ){
			DbgOut( "e3dhsp : GetTextureSize : poolptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetTextureInfo : poolptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//5 transparentptr
	curtype = *hei->nptype;
	int* transparentptr;
	if( curtype == 4 ){
		transparentptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !transparentptr ){
			DbgOut( "e3dhsp : GetTextureSize : transparentptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetTextureInfo : transparentptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//6 formatptr
	curtype = *hei->nptype;
	int* formatptr;
	if( curtype == 4 ){
		formatptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !formatptr ){
			DbgOut( "e3dhsp : GetTextureSize : formatptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetTextureInfo : formatptr parameter type error %d!!!\n", curtype );
		return 1;
	}


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DGetTextureInfo : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->GetTextureInfoBySerial( texid, widthptr, heightptr, poolptr, transparentptr, formatptr );
	if( ret ){
		DbgOut( "E3DGetTextureInfo : texbank GetTextureSizeBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DCopyTextureToTexture srctexid, desttexid
EXPORT BOOL WINAPI E3DCopyTextureToTexture( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 srctexid
	int srctexid;
	srctexid = hei->HspFunc_prm_getdi( 0 );

//2 dsttexid
	int dsttexid;
	dsttexid = hei->HspFunc_prm_getdi( 0 );

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DCopyTextureToTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CopyTexture2Texture( srctexid, dsttexid );
	if( ret ){
		DbgOut( "E3DCopyTextureToTexture : texbank : CopyTexture2Texture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetTextureFromDispObj hsid, partno, texid
EXPORT BOOL WINAPI E3DGetTextureFromDispObj( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi( 0 );

//2 partno
	int partno;
	partno = hei->HspFunc_prm_getdi( 0 );

//3 texidptr
	curtype = *hei->nptype;
	int* texidptr;
	if( curtype == 4 ){
		texidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !texidptr ){
			DbgOut( "E3DGetTextureFromDispObj : texidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DGetTextureFromDispObj : texidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	*texidptr = -1;
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DGetTextureFromDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	CMySprite* cursprite = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DGetTextureFromDispObj : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DGetTextureFromDispObj : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( curhs ){
		ret = curhs->GetTextureFromDispObj( partno, texidptr );
		if( ret ){
			DbgOut( "E3DGetTextureFromDispObj : hs GetTextureFromDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( cursprite ){
		ret = cursprite->GetTextureFromDispObj( texidptr );
		if( ret ){
			DbgOut( "E3DGetTextureFromDispObj : sprite GetTextureFromDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

//E3DSetTextureToDispObj hsid, partno, texid, transparent				
EXPORT BOOL WINAPI E3DSetTextureToDispObj( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi( 0 );

//2 partno
	int partno;
	partno = hei->HspFunc_prm_getdi( 0 );

//3 texid
	int texid;
	texid = hei->HspFunc_prm_getdi( 0 );


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DSetTextureToDispObj : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	CMySprite* cursprite = 0;

	if( hsid >= 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DSetTextureToDispObj : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DGetTextureFromDispObj : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

////////
	
	int texindex;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DSetTextureToDispObj : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int textype;
	textype = g_texbnk->GetType( texindex );
	if( textype != TEXTYPE_TEXTURE ){
		DbgOut( "E3DSetTextureToDispObj : this is not texture %d error !!!\n", textype );
		_ASSERT( 0 );
		return 1;
	}

	if( curhs ){
		ret = curhs->SetTextureToDispObj( partno, texid );
		if( ret ){
			DbgOut( "E3DSetTextureToDispObj : hs SetTextureToDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( cursprite ){
		ret = cursprite->SetTextureToDispObj( texid );
		if( ret ){
			DbgOut( "E3DSetTextureToDispObj : cursprite SetTextureToDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

//E3DRasterScrollTexture srctexid, desttexid, t, param1, param1div, param2, param2div
EXPORT BOOL WINAPI E3DRasterScrollTexture( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 srctexid
	int srctexid;
	srctexid = hei->HspFunc_prm_getdi( 0 );

//2 desttexid
	int desttexid;
	desttexid = hei->HspFunc_prm_getdi( 0 );

//3 t
	int t;
	t = hei->HspFunc_prm_getdi( 0 );

//4 param1
	int param1;
	param1 = hei->HspFunc_prm_getdi( 0 );

//5 param1div
	int param1div;
	param1div = hei->HspFunc_prm_getdi( 1 );


//6 param2
	int param2;
	param2 = hei->HspFunc_prm_getdi( 0 );


//7 param2div
	int param2div;
	param2div = hei->HspFunc_prm_getdi( 1 );


////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DRasterScrollTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DRasterScrollTexture : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	float fparam1, fparam2;

	if( param1div != 0 ){
		fparam1 = (float)param1 / (float)param1div;
	}else{
		fparam1 = 0.0f;
	}
	if( param2div != 0 ){
		fparam2 = (float)param2 / (float)param2div;
	}else{
		fparam2 = 0.0f;
	}

	ret = g_texbnk->RasterScrollTexture( pdev, srctexid, desttexid, t, fparam1, fparam2 );
	if( ret ){
		DbgOut( "E3DRasterScrollTexture : texbank RasterScrollTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCopyTextureToBackBuffer texid
EXPORT BOOL WINAPI E3DCopyTextureToBackBuffer( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 texid
	int texid;
	texid = hei->HspFunc_prm_getdi( 0 );

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DCopyTextureToBackBuffer : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCopyTextureToBackBuffer : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CopyTextureToBackBuffer( pdev, texid );
	if( ret ){
		DbgOut( "E3DCopyTextureToBackBuffer : texbank CopyTextureToBackBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DDestroyTexture texid
EXPORT BOOL WINAPI E3DDestroyTexture( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 texid
	int texid;
	texid = hei->HspFunc_prm_getdi( 0 );

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DDestroyTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "E3DDestroyTexture : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "E3DDestroyTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texindex;
	texindex = g_texbnk->FindName( texname, transparent );
	if( texindex < 0 ){
		DbgOut( "E3DDestroyTexture : texbank FindName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////

	ret = g_texbnk->DestroyTexture( texindex );
	_ASSERT( !ret );

/***	
	CHandlerSet* curhs;
	curhs = hshead;
	while( curhs ){
		ret = curhs->SetTextureDirtyFlag( -1 );//全てにdirtyflagセット　
		_ASSERT( !ret );
		curhs = curhs->next;
	}

	if( g_bbhs ){
		ret = g_bbhs->SetTextureDirtyFlag( -1 );//全てにdirtyflagセット
		_ASSERT( !ret );
	}

	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		ret = cursprite->SetTextureDirtyFlag();
		cursprite = cursprite->next;
	}

////////////
	int dirtyflag;

	dirtyflag = g_texbnk->GetDirtyFlag( texindex );

	if( dirtyflag == 0 ){
		ret = g_texbnk->DestroyTexture( texindex );
		_ASSERT( !ret );
	}
***/

	return 0;
}

//E3DSetLightAmbient lightid, r, g, b
EXPORT BOOL WINAPI E3DSetLightAmbient( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 lid
	int lid;
	lid = hei->HspFunc_prm_getdi( 0 );

//2 r
	int r;
	r = hei->HspFunc_prm_getdi( 0 );

//3 g
	int g;
	g = hei->HspFunc_prm_getdi( 0 );

//4 b
	int b;
	b = hei->HspFunc_prm_getdi( 0 );

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetLightAmbient : lightID not found error !!!\n" );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f ambcol;
	ambcol.x = (float)r / 255.0f;
	ambcol.y = (float)g / 255.0f;
	ambcol.z = (float)b / 255.0f;

	curlight->Ambient = ambcol;

////////

	//resotreでも実行する

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetLightAmbient : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(light) );


	switch( curlight->Type ){
	case D3DLIGHT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = curlight->orgDirection;
		break;

	case D3DLIGHT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		light.Position = curlight->orgPosition;
		light.Direction = curlight->orgDirection;
		light.Range = sqrtf( curlight->Range );
		light.Falloff = 1.0f;
		light.Attenuation0 = 1.0f;
		light.Theta        = curlight->Theta;
		light.Phi          = curlight->Theta;
		break;

	case D3DLIGHT_POINT:
		light.Type = D3DLIGHT_POINT;
		light.Position = curlight->orgPosition;
		light.Range = sqrtf( curlight->Range );
		light.Attenuation0 = 0.4f;
		break;

	default:
		break;

	}

	light.Diffuse.r = curlight->Diffuse.x;
	light.Diffuse.g = curlight->Diffuse.y;
	light.Diffuse.b = curlight->Diffuse.z;
	light.Diffuse.a = 1.0f;

	light.Ambient.r = curlight->Ambient.x;
	light.Ambient.g = curlight->Ambient.y;
	light.Ambient.b = curlight->Ambient.z;
	light.Ambient.a = 1.0f;

	light.Specular.r = curlight->Specular.x;
	light.Specular.g = curlight->Specular.y;
	light.Specular.b = curlight->Specular.z;
	light.Specular.a = 1.0f;


    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );

	return 0;
}


//E3DSetLightSpecular lightid, r, g, b
EXPORT BOOL WINAPI E3DSetLightSpecular( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 lid
	int lid;
	lid = hei->HspFunc_prm_getdi( 0 );

//2 r
	int r;
	r = hei->HspFunc_prm_getdi( 0 );

//3 g
	int g;
	g = hei->HspFunc_prm_getdi( 0 );

//4 b
	int b;
	b = hei->HspFunc_prm_getdi( 0 );

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetLightSpecular : lightID not found error !!!\n" );
		return 1;
	}

	if( r < 0 )
		r = 0;
	else if( r > 255 )
		r = 255;
	
	if( g < 0 )
		g = 0;
	else if( g > 255 )
		g = 255;

	if( b < 0 )
		b = 0;
	else if( b > 255 )
		b = 255;

	CVec3f speccol;
	speccol.x = (float)r / 255.0f;
	speccol.y = (float)g / 255.0f;
	speccol.z = (float)b / 255.0f;

	curlight->Specular = speccol;

////////

	//resotreでも実行する

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : SetLightSpecular : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(light) );


	switch( curlight->Type ){
	case D3DLIGHT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = curlight->orgDirection;
		break;

	case D3DLIGHT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		light.Position = curlight->orgPosition;
		light.Direction = curlight->orgDirection;
		light.Range = sqrtf( curlight->Range );
		light.Falloff = 1.0f;
		light.Attenuation0 = 1.0f;
		light.Theta        = curlight->Theta;
		light.Phi          = curlight->Theta;
		break;

	case D3DLIGHT_POINT:
		light.Type = D3DLIGHT_POINT;
		light.Position = curlight->orgPosition;
		light.Range = sqrtf( curlight->Range );
		light.Attenuation0 = 0.4f;
		break;

	default:
		break;

	}

	light.Diffuse.r = curlight->Diffuse.x;
	light.Diffuse.g = curlight->Diffuse.y;
	light.Diffuse.b = curlight->Diffuse.z;
	light.Diffuse.a = 1.0f;

	light.Ambient.r = curlight->Ambient.x;
	light.Ambient.g = curlight->Ambient.y;
	light.Ambient.b = curlight->Ambient.z;
	light.Ambient.a = 1.0f;

	light.Specular.r = curlight->Specular.x;
	light.Specular.g = curlight->Specular.y;
	light.Specular.b = curlight->Specular.z;
	light.Specular.a = 1.0f;


    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );

	return 0;
}

//E3DInvColTexture texid
EXPORT BOOL WINAPI E3DInvColTexture( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 lid
	int texid;
	texid = hei->HspFunc_prm_getdi( 0 );

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DInvColTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DInvColTexture : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->InvColTexture( pdev, texid );
	if( ret ){
		DbgOut( "E3DInvColTexture : texbank InvColTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DSaveGndFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : hsid
	// 2 : file name1

// 1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);
	
//2
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSaveGNDFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DSaveGNDFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : E3DSaveGNDFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );
////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveGNDFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SaveGNDFile( tempname1 );
	if( ret ){
		DbgOut( "E3DSaveGNDFile : curhs SaveGNDFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EXPORT BOOL WINAPI E3DLoadGndFile( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// first : sig file name
	// second : pointer of handlerset id.
	// 3 adjustuvflag
	// 4 mult100
//first
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGNDFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DLoadGNDFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "E3DLoadGNDFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//second	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "E3DLoadGNDFile : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGNDFile : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
// 3:
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

// 4:
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////


	int ret;
	int temphsid = -1;
	ret = LoadGNDFile_F( tempname, adjustuvflag, mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFile : LoadGNDFile_F error !!!\n" );
		_ASSERT( 0 );
		*hsidptr = -1;
		return 1;
	}
	*hsidptr = temphsid;
	return 0;


	
	return 0;
}

int LoadGNDFile_F( char* tempname, int adjustuvflag, int mult100, int* hsidptr )
{

	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	SetResDir( tempname, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!


	float srcmult;
	srcmult = (float)mult100 / 100.0f;


	ret = newhs->LoadGNDFile( tempname, adjustuvflag, srcmult );
	if( ret ){
		DbgOut( "easy3d : E3DLoadGNDFile : LoadGNDFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;

	return 0;
}



//E3DLoadGNDFileFromBuf resdir, buf, bufsize, hsidptr, adjustuvflag
	//resdirは最後に\\を忘れずに。
EXPORT BOOL WINAPI E3DLoadGndFileFromBuf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGNDFileFromBuf : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DLoadGNDFileFromBuf : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "E3DLoadGNDFileFromBuf : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//2
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "E3DLoadGNDFileFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGNDFileFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//3
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//4	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "E3DLoadGNDFileFromBuf : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGNDFileFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//5
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	int ret;
	int temphsid = -1;
	ret = LoadGNDFileFromBuf_F( tempname, bufptr, bufsize, adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFileFromBuf : LoadGNDFileFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		*hsidptr = -1;
		return 1;
	}
	*hsidptr = temphsid;
	
	return 0;
}

int LoadGNDFileFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, int* hsidptr )
{

	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	SetResDir( tempname, newhs );//!!!!!!

	LeaveCriticalSection( &g_crit_hshead );//###### end crit
	

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!


	ret = newhs->LoadGNDFileFromBuf( bufptr, bufsize, adjustuvflag );
	if( ret ){
		DbgOut( "easy3d : E3DLoadGNDFileFromBuf : LoadGNDFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;


	return 0;
}



//E3DCreateTextureFromBuf buf, bufleng, pool, transparent, texid
EXPORT BOOL WINAPI E3DCreateTextureFromBuf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

//1
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "E3DCreateTextureFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCreateTextureFromBuf : bufptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//2
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//3 pool
	int pool;
	pool = hei->HspFunc_prm_getdi( D3DPOOL_MANAGED );


//4 transparent
	int transparent;
	transparent = hei->HspFunc_prm_getdi( 0 );


//5 texidptr
	curtype = *hei->nptype;
	int* texidptr;
	if( curtype == 4 ){
		texidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !texidptr ){
			DbgOut( "E3DCreateTextureFromBuf : texidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DCreateTextureFromBuf : texidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

////

	if ( *hei->er ) return *hei->er;		// エラーチェック
////
	int ret;


	if( bufsize <= 0 ){
		DbgOut( "E3DCreateTextureFromBuf : bufsize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( (pool != D3DPOOL_DEFAULT) && (pool != D3DPOOL_MANAGED) && (pool != D3DPOOL_SYSTEMMEM) ){
		pool = D3DPOOL_DEFAULT;
	}
	if( (transparent < 0) || (transparent >= 3) ){
		transparent = 0;
	}


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateTextureFromBuf : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DCreateTextureFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////
	int newserial = -1;
	int newindex = -1;
	char usertexname[ MAX_PATH + 256 ];
	s_usertexcnt++;


	sprintf( usertexname, "FromBuf_TEXID%d", s_usertexcnt );


	newserial = g_texbnk->AddName( usertexname, transparent, pool );
	if( newserial <= 0 ){
		DbgOut( "E3DCreateTextureFromBuf : texbnk AddName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	newindex = g_texbnk->FindName( usertexname, transparent );
	if( newindex < 0 ){
		DbgOut( "E3DCreateTextureFromBuf : texbnk FindName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->SetBufData( newindex, bufptr, bufsize );
	if( ret ){
		DbgOut( "E3DCreateTextureFromBuf : texbank SetBufData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CreateTexData( newindex, pdev );
	if( ret ){
		DbgOut( "E3DCreateTextureFromBuf : texbnk CreateTexData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	*texidptr = newserial;//!!!!!!!
	return 0;

}

//E3DLoadSoundFromBuf buf, bufleng, type, use3dflag, bufnum, soundid
//type 0 --> wav, type 1 --> midi
EXPORT BOOL WINAPI E3DLoadSoundFromBuf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;
//1
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "E3DLoadSoundFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadSoundFromBuf : bufptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//2
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//3 
	int type;
	type = hei->HspFunc_prm_getdi(0);

//4
	int use3dflag;
	use3dflag = hei->HspFunc_prm_getdi(0);

//5
	int bufnum;
	bufnum = hei->HspFunc_prm_getdi(1);


//6
	curtype = *hei->nptype;
	int* soundidptr;
	if( curtype == 4 ){
		soundidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !soundidptr ){
			DbgOut( "E3DLoadSoundFromBuf : soundidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadSoundFromBuf : soundidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////	

	int ret;

	if( !s_HS ){
		DbgOut( "E3DLoadSoundFromBuf : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (type != 0) && (type != 1) ){
		type = 0;
	}


// 読み込み

	ret = s_HS->LoadSoundFileFromBuf( bufptr, bufsize, type, use3dflag, bufnum, soundidptr );
	if( ret ){
		DbgOut( "E3DLoadSoundFromBuf : hs LoadSoundFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

//E3DTwistBone hsid, motid, frameno, boneno, twistdeg, divdeg
EXPORT BOOL WINAPI E3DTwistBone( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int motid;
	motid = hei->HspFunc_prm_getdi(0);

	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);

	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

	int twistdeg;
	twistdeg = hei->HspFunc_prm_getdi(0);

	int divdeg;
	divdeg = hei->HspFunc_prm_getdi(1);
	

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DTwistBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fdeg;
	if( divdeg != 0 ){
		fdeg = (float)twistdeg / (float)divdeg;
	}else{
		fdeg = (float)twistdeg;
	}


	int ret;
	ret = curhs->TwistBone( motid, frameno, boneno, fdeg );
	if( ret ){
		DbgOut( "E3DTwistBone : hs TwistBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DSetStartPointOfSound soundid, sec1000
EXPORT BOOL WINAPI E3DSetStartPointOfSound( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

	int sec1000;
	sec1000 = hei->HspFunc_prm_getdi(0);	

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////


	if( sec1000 < 0 ){
		sec1000 = 0;//!!!!!!!!!!!!!!!!!!!
	}


	if( !s_HS ){
		DbgOut( "E3DSetStartPointOfSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetStartPointOfSound( soundid, sec1000 );
	if( ret ){
		DbgOut( "E3DSetStartPointOfSound : hs SetTartPointOfSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetBoneQ hsid, boneno, motid, frameno, kind, qid
//	kind 0 --> 親の影響のないクォータニオン
//	kind 1 --> 親の影響を考慮したクォータニオン
//  kind 2 --> 親の影響＋モデル全体のクォータニオンの影響
EXPORT BOOL WINAPI E3DGetBoneQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

	int motid;
	motid = hei->HspFunc_prm_getdi(0);

	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);

	int kind;
	kind = hei->HspFunc_prm_getdi(0);
	
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBoneQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !s_qh ){
		DbgOut( "E3DGetBoneQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion dstq;
	ret = curhs->GetBoneQ( boneno, motid, frameno, kind, &dstq );
	if( ret ){
		DbgOut( "E3DGetBoneQ : hs GetBoneQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaternion2 setq;
	setq.CopyFromCQuaternion( &dstq );//!!!!!!!!

	ret = s_qh->SetQ( qid, &setq );
	if( ret ){
		DbgOut( "E3DGetBoneQ : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}




//E3DSetBoneQ hsid, boneno, motid, frameno, qid
EXPORT BOOL WINAPI E3DSetBoneQ( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

	int motid;
	motid = hei->HspFunc_prm_getdi(0);

	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);
	
	int qid;
	qid = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBoneQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !s_qh ){
		DbgOut( "E3DSetBoneQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DSetBoneQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CQuaternion setq;
	setq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!


	ret = curhs->SetQ( boneno, motid, frameno, setq );
	if( ret ){
		DbgOut( "E3DSetBoneQ : hs SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DIsSoundPlaying soundid, playing
//	再生中　playing 1
//	停止中　playing 0
EXPORT BOOL WINAPI E3DIsSoundPlaying( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;

	// type $202
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);


	curtype = *hei->nptype;
	int* playingptr;
	if( curtype == 4 ){
		playingptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !playingptr ){
			DbgOut( "E3DIsSoundPlaying : playingptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DIsSoundPlaying playingptr parameter type error %d!!!\n", curtype );
		return 1;
	}


	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	int ret;

	if( !s_HS ){
		DbgOut( "E3DIsSoundPlaying : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->IsSoundPlaying( soundid, playingptr );
	if( ret ){
		DbgOut( "E3DIsSoundPlaying : hs IsSoundPlaying error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DIKTranslate hsid, boneno, motid, frameno, posx, posy, posz
EXPORT BOOL WINAPI E3DIKTranslate( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int boneno;
	boneno = hei->HspFunc_prm_getdi(0);

	int motid;
	motid = hei->HspFunc_prm_getdi(0);

	int frameno;
	frameno = hei->HspFunc_prm_getdi(0);
	
	int posx;
	posx = hei->HspFunc_prm_getdi(0);

	int posy;
	posy = hei->HspFunc_prm_getdi(0);

	int posz;
	posz = hei->HspFunc_prm_getdi(0);
		
	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKTranslate : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 srcpos;
	srcpos.x = (float)posx;
	srcpos.y = (float)posy;
	srcpos.z = (float)posz;


	ret = curhs->IKTranslate( boneno, motid, frameno, srcpos );
	if( ret ){
		DbgOut( "E3DIKTranslate : hs IKTranslate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetUVTile hsid, partno, texrule, unum, vnum, tileno
//	texrule 0 --> Ｘ軸投影
//		 1 --> Ｙ軸投影
//		 2 --> Ｚ軸投影
//		 3 --> 円筒貼り付け
//		 4 --> 球貼り付け
EXPORT BOOL WINAPI E3DSetUVTile( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int partno;
	partno = hei->HspFunc_prm_getdi(0);

	int texrule;
	texrule = hei->HspFunc_prm_getdi(0);

	int unum;
	unum = hei->HspFunc_prm_getdi(1);
	
	int vnum;
	vnum = hei->HspFunc_prm_getdi(1);

	int tileno;
	tileno = hei->HspFunc_prm_getdi(0);

	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	int ret;

	if( unum == 0 )
		unum = 1;
	if( vnum == 0 )
		vnum = 1;

	if( (texrule < TEXRULE_X) || (texrule > TEXRULE_SPHERE) ){
		DbgOut( "E3DSetUVTile : illeagal texrule : set default value TEXRULE_Z warning !!!\n" );
		texrule = TEXRULE_Z;//!!!!!
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetUVTile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUVTile( partno, texrule, unum, vnum, tileno );
	if( ret ){
		DbgOut( "E3DSetUVTile : hs SetUVTile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DImportMQOFileAsGround hsid, filename, adjustuvflagg, mult100, offsetx, offsety, offsetz
EXPORT BOOL WINAPI E3DImportMQOFileAsGround( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int curtype;
	
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

/////////
	char tempname[MAX_PATH];
	char* nameptr = 0;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();			// パラメータ2:文字列
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DImportMQOFileAsGround : parameter type error !!!\n" );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DImportMQOFileAsGround : parameter error !!!\n" );
		return 1;
	}

	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "E3DImportMQOFileAsGround : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

/////////
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);


	int offsetx;
	offsetx = hei->HspFunc_prm_getdi(0);
	int offsety;
	offsety = hei->HspFunc_prm_getdi(0);
	int offsetz;
	offsetz = hei->HspFunc_prm_getdi(0);


	int rotx;
	rotx = hei->HspFunc_prm_getdi(0);
	int roty;
	roty = hei->HspFunc_prm_getdi(0);
	int rotz;
	rotz = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DImportMQOFileAsGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fmult;
	fmult = (float)mult100 / 100.0f;

	D3DXVECTOR3 offset;
	offset.x = (float)offsetx;
	offset.y = (float)offsety;
	offset.z = (float)offsetz;

	D3DXVECTOR3 rot;
	rot.x = (float)rotx;
	rot.y = (float)roty;
	rot.z = (float)rotz;

	ret = curhs->ImportMQOFileAsGround( tempname, s_hwnd, adjustuvflag, fmult, offset, rot );
	if( ret ){
		DbgOut( "E3DImportMQOFileAsGround : hs ImportMOQFileAsGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////////

//Create 3D Object
	ret = Restore( curhs, -1 );
	if( ret ){
		DbgOut( "E3DImportMQOFileAsGround : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////
	ret = curhs->SetCurrentBSphereData();
	if( ret ){
		DbgOut( "E3DImportMQOFileAsGround : hs SetCurrentBSphereData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DLoadMQOFileAsMovableAreaFromBuf buf, bufsize, mult100, hsidptr 
EXPORT BOOL WINAPI E3DLoadMQOFileAsMovableAreaFromBuf( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

//1
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : bufptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//2
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//3
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);

//4	
	curtype = *hei->nptype;
	int* hsidptr;
	if( curtype == 4 ){
		hsidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !hsidptr ){
			DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : hsidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableAreaFromBuf_F( bufptr, bufsize, mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : LoadMQOFileAsMovableAreaFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		*hsidptr = temphsid;
	}
	*hsidptr = temphsid;
	return 0;

}

int LoadMQOFileAsMovableAreaFromBuf_F( char* bufptr, int bufsize, int mult100, int* hsidptr )
{

	float fmult;
	fmult = (float)mult100 / 100.0f;


	int ret = 0;

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->LoadMQOFileAsMovableAreaFromBuf( s_hwnd, bufptr, bufsize, fmult );
	if( ret ){
		DbgOut( "LoadMQOFileAsMovableAreaFromBuf : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}



/////
	ret = newhs->SetCurrentBSphereData();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;

	return 0;
}


//E3DChkThreadWorking threadid, working, returnval
EXPORT BOOL WINAPI E3DChkThreadWorking( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

//1
	int threadid;
	threadid = hei->HspFunc_prm_getdi(0);


//2
	curtype = *hei->nptype;
	int* workingptr;
	if( curtype == 4 ){
		workingptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !workingptr ){
			DbgOut( "E3DChkThreadWorking : workingptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DChkThreadWorking : workingptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//3
	curtype = *hei->nptype;
	int* returnvalptr;
	if( curtype == 4 ){
		returnvalptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !returnvalptr ){
			DbgOut( "E3DChkThreadWorking : returnvalptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DChkThreadWorking : returnvalptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//4
	curtype = *hei->nptype;
	int* returnvalptr2;
	if( curtype == 4 ){
		returnvalptr2 = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !returnvalptr2 ){
			DbgOut( "E3DChkThreadWorking : returnvalptr2 error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DChkThreadWorking : returnvalptr2 parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

////////////////

	if( (threadid < 0) || (threadid >= s_threadnum) ){
		DbgOut( "E3DChkThreadWorking : threadid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( s_thtable[threadid].hThread ){
		DWORD dwret;
		dwret = WaitForSingleObject( s_thtable[threadid].hThread, 0 );

		switch( dwret ){
		case WAIT_OBJECT_0:
			*workingptr = 0;
			*returnvalptr = s_thtable[threadid].returnval;
			*returnvalptr2 = s_thtable[threadid].returnval2;
			break;
		case WAIT_TIMEOUT:
			*workingptr = 1;
			*returnvalptr = -1;
			*returnvalptr2 = -1;
			break;
		case WAIT_FAILED:
			DbgOut( "E3DChkWorkingThread : WaitForSigleObject WAIT_FAILED warning !!!\n" );
			_ASSERT( 0 );
			*workingptr = 0;
			*returnvalptr = -1;
			*returnvalptr2 = -1;
			break;
		default:
			_ASSERT( 0 );
			*workingptr = 0;
			*returnvalptr = -1;
			*returnvalptr2 = -1;
			break;
		}
	}else{
		*workingptr = 0;
		*returnvalptr = s_thtable[threadid].returnval;
		*returnvalptr2 = s_thtable[threadid].returnval2;
	}

	return 0;
}


//E3DLoadMQOFileAsGroundThread( char* filename, int mult100, int adjustuvflag, int* threadid );
EXPORT BOOL WINAPI E3DLoadMQOFileAsGroundThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadMQOFileAsGround : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DLoadMQOFileAsGround : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadMQOFileAsGround : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2 : mult100
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(0);

//3
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);


//4	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "e3dhsp : LoadMQOFileAsGroundThread : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DMQOFileAsGroundThread : threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////	

	*threadidptr = -1;


	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DMQOFileAsGroundThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;

	int leng;
	leng = (int)strlen( tempname1 );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DLoadMQOFileAsGroundThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname1 );


	s_thtable[s_threadnum - 1].mult100 = mult100;
	s_thtable[s_threadnum - 1].adjustuvflag = adjustuvflag;

	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsGround, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DLoadMQOFileAsGroundThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 0;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;

	return 0;
}

DWORD WINAPI ThreadLoadMQOFileAsGround( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = LoadMQOFileAsGround_F( curtable->filename, curtable->mult100, s_hwnd, curtable->adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : LoadMQOFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;

}

EXPORT BOOL WINAPI E3DSigLoadThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// first : sig file name
	// second : pointer of handlerset id.
	// 3 adjustuvflag
	// 4 mult100
//first
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSigLoad : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DSigLoad : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : SigLoad : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

// 2:
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

// 3:
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);

//4	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "e3dhsp : SigLoad : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSigLoad : threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*threadidptr = -1;


	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DSigLoadThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DSigLoadThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname );

	s_thtable[s_threadnum - 1].mult100 = mult100;
	s_thtable[s_threadnum - 1].adjustuvflag = adjustuvflag;

	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadSigLoad, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DSigLoadThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;

	return 0;

}

DWORD WINAPI ThreadSigLoad( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = SigLoad_F( curtable->filename, curtable->mult100, curtable->adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoad : SigLoad_F error %s !!!\n", curtable->filename );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EXPORT BOOL WINAPI E3DSigLoadFromBufThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DSigLoadFromBuf : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DSigLoadFromBuf : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : SigLoad : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//2
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "e3dhsp : SigLoad : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSigLoadFromBuf : bufptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//3
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//4
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

//5
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);

//6	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "e3dhsp : SigLoad : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DSigLoadFromBuf : threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*threadidptr = -1;


	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DSigLoadFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DSigLoadFromBufThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname );

	s_thtable[s_threadnum - 1].mult100 = mult100;
	s_thtable[s_threadnum - 1].adjustuvflag = adjustuvflag;

	s_thtable[s_threadnum - 1].bufptr = bufptr;
	s_thtable[s_threadnum - 1].bufsize = bufsize;

	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadSigLoadFromBuf, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DSigLoadFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;

	return 0;
}

DWORD WINAPI ThreadSigLoadFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = SigLoadFromBuf_F( curtable->filename, curtable->bufptr, curtable->bufsize, curtable->adjustuvflag, curtable->mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoadFromBuf : SigLoadFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EXPORT BOOL WINAPI E3DLoadMQOFileAsMovableAreaThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : mqo file name1
	// 2 : mult100
	// 3 : pointer of handlerset id.

//1
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadMQOFileAsMovableArea : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadMQOFileAsMovableArea : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2 : mult100
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);


//3	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "e3dhsp : LoadMQOFileAsMovableArea : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableArea: threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////	

	*threadidptr = -1;


	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;

	int leng;
	leng = (int)strlen( tempname1 );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname1 );

	s_thtable[s_threadnum - 1].mult100 = mult100;


	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsMovableArea, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;
	
	return 0;
}

DWORD WINAPI ThreadLoadMQOFileAsMovableArea( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableArea_F( curtable->filename, curtable->mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : LoadMQOFileAsMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EXPORT BOOL WINAPI E3DLoadMQOFileAsMovableAreaFromBufThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

//1
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : bufptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//2
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//3
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);

//4	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*threadidptr = -1;


	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;


	s_thtable[s_threadnum - 1].mult100 = mult100;
	s_thtable[s_threadnum - 1].bufptr = bufptr;
	s_thtable[s_threadnum - 1].bufsize = bufsize;

	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsMovableAreaFromBuf, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;


	return 0;

}

DWORD WINAPI ThreadLoadMQOFileAsMovableAreaFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableAreaFromBuf_F( curtable->bufptr, curtable->bufsize, curtable->mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : LoadMQOFileAsMovableAreaFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = temphsid;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EXPORT BOOL WINAPI E3DLoadGroundBMPThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : bmp file name1
	// 2 : bmp file name2
	// 3 : bmp file name3
	// 4 : texture file name
	// 5 : maxx
	// 6 : maxz
	// 7 : divx
	// 8 : divz
	// 9 : maxheight
	// 10 : pointer of handlerset id.

	// 11 : TLmode , ver1036

//1
	char tempname1[MAX_PATH];
	char* nameptr1;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr1 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr1 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr1 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen1;
	namelen1 = (int)strlen( nameptr1 );
	if( namelen1 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname1, nameptr1 );

//2
	char tempname2[MAX_PATH];
	char* nameptr2;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr2 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr2 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr2 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen2;
	namelen2 = (int)strlen( nameptr2 );
	if( namelen2 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname2, nameptr2 );
//3
	char tempname3[MAX_PATH];
	char* nameptr3;


	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr3 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr3 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr3 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen3;
	namelen3 = (int)strlen( nameptr3 );
	if( namelen3 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname3, nameptr3 );
	
	
//4
	char tempname4[MAX_PATH];
	char* nameptr4;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr4 = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr4 = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGroundBMP : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr4 ){
		DbgOut( "E3DLoadGroundBMP : parameter error !!!\n" );
		return 1;
	}
	
	int namelen4;
	namelen4 = (int)strlen( nameptr4 );
	if( namelen4 >= MAX_PATH ){
		DbgOut( "e3dhsp : LoadGroundBMP : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname4, nameptr4 );
//5
	int maxx;
	maxx = hei->HspFunc_prm_getdi(0);
//6
	int maxz;
	maxz = hei->HspFunc_prm_getdi(0);

//7	
	int divx;
	divx = hei->HspFunc_prm_getdi(0);
	if( divx < 2 )
		divx = 2;
//8
	int divz;
	divz = hei->HspFunc_prm_getdi(0);
	if( divz < 2 )
		divz = 2;
//9
	int maxheight;
	maxheight = hei->HspFunc_prm_getdi(0);
	if( maxheight < 0 )
		maxheight = 0;
//10	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "e3dhsp : LoadGroundBMP : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGroundBMP : threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

	
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*threadidptr = -1;


	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DLoadGroundBMPThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;


	int leng;
	leng = (int)strlen( tempname1 );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname1 );


	leng = (int)strlen( tempname2 );
	s_thtable[s_threadnum - 1].filename2 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename2) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename2, tempname2 );


	leng = (int)strlen( tempname3 );
	s_thtable[s_threadnum - 1].filename3 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename3) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename3, tempname3 );

	leng = (int)strlen( tempname4 );
	s_thtable[s_threadnum - 1].filename4 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename4) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename4, tempname4 );


	s_thtable[s_threadnum - 1].maxx = maxx;
	s_thtable[s_threadnum - 1].maxz = maxz;
	s_thtable[s_threadnum - 1].divx = divx;
	s_thtable[s_threadnum - 1].divz = divz;
	s_thtable[s_threadnum - 1].maxheight = maxheight;

	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadGroundBMP, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DLoadGroundBMPThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;
		
	return 0;
}

DWORD WINAPI ThreadLoadGroundBMP( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadGroundBMP_F( curtable->filename, curtable->filename2, curtable->filename3, curtable->filename4, curtable->maxx, curtable->maxz, curtable->divx, curtable->divz, curtable->maxheight, 2, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : LoadGroundBMP_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EXPORT BOOL WINAPI E3DLoadGndFileThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
//first
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGNDFile : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DLoadGNDFile : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "E3DLoadGNDFile : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

// 2:
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

// 3:
	int mult100;
	mult100 = hei->HspFunc_prm_getdi(100);

//4	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "E3DLoadGNDFile : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGNDFile : threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*threadidptr = -1;

	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DLoadGNDFileThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DLoadGNDFileThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname );

	s_thtable[s_threadnum - 1].mult100 = mult100;
	s_thtable[s_threadnum - 1].adjustuvflag = adjustuvflag;

	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadGNDFile, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DLoadGNDFileThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;
	
	return 0;
}

DWORD WINAPI ThreadLoadGNDFile( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadGNDFile_F( curtable->filename, curtable->adjustuvflag, curtable->mult100, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFile : LoadGNDFile_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}
	curtable->returnval = temphsid;

	return 0;
}

EXPORT BOOL WINAPI E3DLoadGndFileFromBufThread( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1
	char tempname[MAX_PATH];
	char* nameptr;
	int curtype;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DLoadGNDFileFromBuf : parameter type error %d!!!\n", curtype );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DLoadGNDFileFromBuf : parameter error !!!\n" );
		return 1;
	}
	
	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "E3DLoadGNDFileFromBuf : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//2
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "E3DLoadGNDFileFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGNDFileFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//3
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//4
	int adjustuvflag;
	adjustuvflag = hei->HspFunc_prm_getdi(0);

//5	
	curtype = *hei->nptype;
	int* threadidptr;
	if( curtype == 4 ){
		threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !threadidptr ){
			DbgOut( "E3DLoadGNDFileFromBuf : threadidptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DLoadGNDFileFromBuf : threadidptr parameter type error %d!!!\n", curtype );
		return 1;
	}


//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////

	*threadidptr = -1;

	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DLoadGNDFileFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DLoadGNDFileFromBufThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname );

	//s_thtable[s_threadnum - 1].mult100 = mult100;
	s_thtable[s_threadnum - 1].adjustuvflag = adjustuvflag;

	s_thtable[s_threadnum - 1].bufptr = bufptr;
	s_thtable[s_threadnum - 1].bufsize = bufsize;


	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadGNDFileFromBuf, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DLoadGNDFileFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[s_threadnum - 1].threadid;
	
	return 0;
}

DWORD WINAPI ThreadLoadGNDFileFromBuf( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadGNDFileFromBuf_F( curtable->filename, curtable->bufptr, curtable->bufsize, curtable->adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGNDFileFromBuf : LoadGNDFileFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EXPORT BOOL WINAPI E3DAddMotionThread( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int curtype;

	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//////////
	char tempname[MAX_PATH];
	char* nameptr = 0;

	curtype = *hei->nptype;
	switch( curtype ){
	case 2:
	case 7:
		nameptr = hei->HspFunc_prm_gets();			// パラメータ2:文字列
		break;
	case 4:
		nameptr = (char*)hei->HspFunc_prm_getv();
		break;
	default:
		DbgOut( "E3DAddMotion : parameter type error !!!\n" );
		return 1;
		break;
	}

	if( !nameptr ){
		DbgOut( "E3DAddMotion : parameter error !!!\n" );
		return 1;
	}

	int namelen;
	namelen = (int)strlen( nameptr );
	if( namelen >= MAX_PATH ){
		DbgOut( "e3dhsp : AddMotion : namelen too large error !!!\n" );
		return 1;
	}
	strcpy( tempname, nameptr );

//////////	
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMotion : parameter type error !!!\n" );
		return 1;
	}
	int* threadidptr;
	threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

	if ( *hei->er ) return *hei->er;		// エラーチェック

////////

	*threadidptr = -1;

	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DAddMotionThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;
	s_thtable[s_threadnum - 1].returnval2 = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[s_threadnum - 1].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[s_threadnum - 1].filename) ){
		DbgOut( "E3DAddMotionThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		return 1;
	}
	strcpy( s_thtable[s_threadnum - 1].filename, tempname );

	s_thtable[s_threadnum - 1].mult100 = hsid;//!!!!!!!!!!!!!!!!!!!!!!

	//s_thtable[s_threadnum - 1].adjustuvflag = adjustuvflag;

	//s_thtable[s_threadnum - 1].bufptr = bufptr;
	//s_thtable[s_threadnum - 1].bufsize = bufsize;


	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadAddMotion, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DAddMotionThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		s_thtable[s_threadnum - 1].returnval2 = -1;
		return 1;
	}

	*threadidptr = s_thtable[s_threadnum - 1].threadid;

	return 0;

}

DWORD WINAPI ThreadAddMotion( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotion_F( curtable->mult100, curtable->filename, &tempmotid, &tempnum );
	if( ret ){
		DbgOut( "E3DAddMotion : AddMotoin_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		curtable->returnval2 = 0;
		return 0;
	}
	curtable->returnval = tempmotid;
	curtable->returnval2 = tempnum;

	return 0;
}

EXPORT BOOL WINAPI E3DAddMotionFromBufThread( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

//1
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2
	int datakind;
	datakind = hei->HspFunc_prm_getdi(0);

//3
	curtype = *hei->nptype;
	char* bufptr;
	if( curtype == 4 ){
		bufptr = (char*)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !bufptr ){
			DbgOut( "e3dhsp : E3DAddMotionFromBuf : bufptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DAddMotionFromBuf : hsidptr parameter type error %d!!!\n", curtype );
		return 1;
	}

//4
	int bufsize;
	bufsize = hei->HspFunc_prm_getdi(0);

//5	
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DAddMotionFromBuf : parameter type error !!!\n" );
		return 1;
	}
	int* threadidptr;
	threadidptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数



	if ( *hei->er ) return *hei->er;		// エラーチェック

////////
	
	*threadidptr = -1;

	s_threadnum++;
	if( s_threadnum > THMAX ){
		DbgOut( "E3DAddMotionFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_thtable[s_threadnum - 1].threadid = s_threadnum - 1;
	s_thtable[s_threadnum - 1].hThread = NULL;
	s_thtable[s_threadnum - 1].returnval = -1;
	s_thtable[s_threadnum - 1].returnval2 = -1;

	s_thtable[s_threadnum - 1].mult100 = hsid;//!!!!!!!!!!!!!!!!!!!!!!
	s_thtable[s_threadnum - 1].adjustuvflag = datakind;

	s_thtable[s_threadnum - 1].bufptr = bufptr;
	s_thtable[s_threadnum - 1].bufsize = bufsize;


	s_thtable[s_threadnum - 1].hThread = BEGINTHREADEX(NULL, 0, ThreadAddMotionFromBuf, 
		(void*)(s_thtable + s_threadnum - 1),
		0, &(s_thtable[s_threadnum - 1].dwId) );


	if( !s_thtable[s_threadnum - 1].hThread ){
		DbgOut( "E3DAddMotionFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[s_threadnum - 1].returnval = -1;
		s_thtable[s_threadnum - 1].returnval2 = -1;
		return 1;
	}

	*threadidptr = s_thtable[s_threadnum - 1].threadid;

	return 0;
}

DWORD WINAPI ThreadAddMotionFromBuf( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotionFromBuf_F( curtable->mult100, curtable->adjustuvflag, curtable->bufptr, curtable->bufsize, &tempmotid, &tempnum );
	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : AddMotionFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		curtable->returnval2 = 0;
		return 0;
	}

	curtable->returnval = tempmotid;
	curtable->returnval2 = tempnum;

	return 0;
}

//E3DGetShaderType hsid, shader, overflow
EXPORT BOOL WINAPI E3DGetShaderType( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);


//2 : shader
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetShaderType : parameter type error !!!\n" );
		return 1;
	}
	int* shaderptr;
	shaderptr = (int*)hei->HspFunc_prm_getv();

//3 : overflow
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetShaderType : parameter type error !!!\n" );
		return 1;
	}
	int* overflowptr;
	overflowptr = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetShaderType( shaderptr, overflowptr );
	if( ret ){
		DbgOut( "E3DGetShaderType : hs GetShaderType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetShaderType hsid, shader, overflow
EXPORT BOOL WINAPI E3DSetShaderType( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202



//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);


//2 : shader
	int shader;
	shader = hei->HspFunc_prm_getdi(0);

//3 : overflow
	int overflow;
	overflow = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetShaderType( shader, overflow );
	if( ret ){
		DbgOut( "E3DSetShaderType : hs SetShaderType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetLightBlendMode lid, mode
EXPORT BOOL WINAPI E3DSetLightBlendMode( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 lid
	int lid;
	lid = hei->HspFunc_prm_getdi(0);

//2 : mode
	int mode;
	mode = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////


	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetLightBlendMode : lightID not found error !!!\n" );
		return 1;
	}

	if( (mode >= LBLEND_MULT) && (mode < LBLEND_MAX) ){
		curlight->lblend = mode;
	}

	return 0;
}

//E3DGetLightBlendMode lid, mode
EXPORT BOOL WINAPI E3DGetLightBlendMode( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

	int curtype;
//1 lid
	int lid;
	lid = hei->HspFunc_prm_getdi(0);

//2 : mode
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetLightBlendMode : parameter type error !!!\n" );
		return 1;
	}
	int* mode;
	mode = (int*)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DGetLightBlendMode : lightID not found error !!!\n" );
		return 1;
	}

	if( mode )
		*mode = curlight->lblend;

	return 0;
}

//E3DSetEmissive hsid, partno, r, g, b, setflag, vertno
EXPORT BOOL WINAPI E3DSetEmissive( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : r
	int r;
	r = hei->HspFunc_prm_getdi(0);

	// 4 : g
	int g;
	g = hei->HspFunc_prm_getdi(0);

	// 5 : b
	int b;
	b = hei->HspFunc_prm_getdi(0);

	// 6 : setflag
	int setflag;
	setflag = hei->HspFunc_prm_getdi(0);

	//7 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetEmissive : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetEmissive : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( r < 0 )
		r = 0;
	if( r > 255 )
		r = 255;
	if( g < 0 )
		g = 0;
	if( g > 255 )
		g = 255;
	if( b < 0 )
		b = 0;
	if( b > 255 )
		b = 255;

	int ret;
	ret = curhs->SetDispEmissive( partno, r, g, b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetEmissive : curhs SetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}
//E3DSetSpecularPower hsid, partno, pow, mult, setflag, vertno
EXPORT BOOL WINAPI E3DSetSpecularPower( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	// 3 : pow
	int pow;
	pow = hei->HspFunc_prm_getdi(0);

	// 4 : mult
	int mult;
	mult = hei->HspFunc_prm_getdi(1);

	// 5 : setflag
	int setflag;
	setflag = hei->HspFunc_prm_getdi(0);

	// 6 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecularPower : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetSpecularPower : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	float fpow;
	if( mult != 0 ){
		fpow = (float)pow / (float)mult;
	}else{
		fpow = (float)pow;
	}

	if( fpow < 0.0f )
		fpow = 0.0f;
	if( fpow > 100.0f )
		fpow = 100.0f;


	int ret;
	ret = curhs->SetDispSpecularPower( partno, fpow, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecularPower : curhs SetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}
//E3DGetEmissive hsid, partno, vertno, r, g, b
EXPORT BOOL WINAPI E3DGetEmissive( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);

	//4 : rptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetEmissive : parameter type error !!!\n" );
		return 1;
	}
	int* rptr;
	rptr = (int *)hei->HspFunc_prm_getv();

	//5 : gptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetEmissive : parameter type error !!!\n" );
		return 1;
	}
	int* gptr;
	gptr = (int *)hei->HspFunc_prm_getv();

	//6 : bptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetEmissive : parameter type error !!!\n" );
		return 1;
	}
	int* bptr;
	bptr = (int *)hei->HspFunc_prm_getv();


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetEmissive : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetEmissive : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	int ret;
	ret = curhs->GetEmissive( partno, vertno, rptr, gptr, bptr );
	if( ret ){
		DbgOut( "e3dhsp : GetEmissive : curhs GetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DGetSpecularPower hsid, partno, vertno, pow100
EXPORT BOOL WINAPI E3DGetSpecularPower( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202

	int curtype;

	// 1 : handlerset id
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	// 2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(-1);

	//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(-1);

	//4 : powptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetSpecularPower : parameter type error !!!\n" );
		return 1;
	}
	int* powptr;
	powptr = (int *)hei->HspFunc_prm_getv();


	if ( *hei->er ) return *hei->er;		// エラーチェック


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecularPower : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetSpecularPower : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	float fpow = 0.0f;

	int ret;
	ret = curhs->GetSpecularPower( partno, vertno, &fpow );
	if( ret ){
		DbgOut( "e3dhsp : GetSpecularPower : curhs GetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*powptr = (int)( fpow * 100.0f );

	return 0;
}


//E3DGetInfElemNum hsid, partno, vertno, num
EXPORT BOOL WINAPI E3DGetInfElemNum( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;
//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : numptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetInfElemNum : parameter type error !!!\n" );
		return 1;
	}
	int* numptr;
	numptr = (int *)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfElemNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetInfElemNum( partno, vertno, numptr );
	if( ret ){
		DbgOut( "E3DGetInfElemNum : curhs GetInfElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

//E3DGetInfElem hsid, partno, vertno, infno, bonenoptr, calcmodeptr, userrate10000ptr, orginf10000ptr, dispinf10000ptr
EXPORT BOOL WINAPI E3DGetInfElem( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;
//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : infno
	int infno;
	infno = hei->HspFunc_prm_getdi(0);

//5 : childjointptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetInfElem : parameter type error !!!\n" );
		return 1;
	}
	int* childjointptr;
	childjointptr = (int *)hei->HspFunc_prm_getv();

//6 : calcmodeptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetInfElem : parameter type error !!!\n" );
		return 1;
	}
	int* calcmodeptr;
	calcmodeptr = (int *)hei->HspFunc_prm_getv();


//7 : userrate10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetInfElem : parameter type error !!!\n" );
		return 1;
	}
	int* userrate10000ptr;
	userrate10000ptr = (int *)hei->HspFunc_prm_getv();

//8 : orginf10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetInfElem : parameter type error !!!\n" );
		return 1;
	}
	int* orginf10000ptr;
	orginf10000ptr = (int *)hei->HspFunc_prm_getv();

//9 : dispinf10000ptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetInfElem : parameter type error !!!\n" );
		return 1;
	}
	int* dispinf10000ptr;
	dispinf10000ptr = (int *)hei->HspFunc_prm_getv();

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float userrate, orginf, dispinf;
	ret = curhs->GetInfElem( partno, vertno, infno, childjointptr, calcmodeptr, &userrate, &orginf, &dispinf );
	if( ret ){
		DbgOut( "E3DGetInfElem : curhs GetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*userrate10000ptr = (int)( userrate * 100.0f * 10000.0f );
	*orginf10000ptr = (int)( orginf * 10000.0f );
	*dispinf10000ptr = (int)( dispinf * 10000.0f );

	return 0;

}


//E3DSetInfElem hsid, partno, vertno, childjointno, calcmode, paintmode, normalizeflag, userrate10000, directval10000
EXPORT BOOL WINAPI E3DSetInfElem( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : childjointno
	int childjointno;
	childjointno = hei->HspFunc_prm_getdi(0);

//5 : calcmode
	int calcmode;
	calcmode = hei->HspFunc_prm_getdi(0);

//6 : paintmode
	int paintmode;
	paintmode = hei->HspFunc_prm_getdi(0);

//7 : normalizeflag
	int normalizeflag;
	normalizeflag = hei->HspFunc_prm_getdi(0);

//8 : userrate10000
	int userrate10000;
	userrate10000 = hei->HspFunc_prm_getdi(0);

//9 : directval10000
	int directval10000;
	directval10000 = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fuserrate, fdirectval;
	fuserrate = (float)userrate10000 / 100.0f / 10000.0f;
	fdirectval = (float)directval10000 / 10000.0f;

	if( fuserrate < 0.0f ){
		_ASSERT( 0 );
		fuserrate = 0.0f;
	}

	if( fdirectval < 0.0f ){
		_ASSERT( 0 );
		fdirectval = 0.0f;
	}


	ret = curhs->SetInfElem( partno, vertno, childjointno, calcmode, paintmode, normalizeflag, fuserrate, fdirectval );
	if( ret ){
		DbgOut( "E3DSetInfElem : curhs SetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DDeleteInfElem hsid, partno, vertno, childjointno, normalizeflag
EXPORT BOOL WINAPI E3DDeleteInfElem( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

//4 : childjointno
	int childjointno;
	childjointno = hei->HspFunc_prm_getdi(0);

//5 : normalizeflag
	int normalizeflag;
	normalizeflag = hei->HspFunc_prm_getdi(0);


////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DeleteInfElem( partno, vertno, childjointno, normalizeflag );
	if( ret ){
		DbgOut( "E3DDeleteInfElem : curhs DeleteInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


//E3DSetInfElemDefault hsid, partno, vertno
EXPORT BOOL WINAPI E3DSetInfElemDefault( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfElemDefault : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetInfElemDefault( partno, vertno );
	if( ret ){
		DbgOut( "E3DSetInfElemDefault : curhs SetInfElemDefault error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DNormalizeInfElem hsid, partno, vertno
EXPORT BOOL WINAPI E3DNormalizeInfElem( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 : partno
	int partno;
	partno = hei->HspFunc_prm_getdi(0);

//3 : vertno
	int vertno;
	vertno = hei->HspFunc_prm_getdi(0);

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DNormalizeInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->NormalizeInfElem( partno, vertno );
	if( ret ){
		DbgOut( "E3DNormalizeInfElem : curhs NormalizeInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DGetVisiblePolygonNum hsid, num1, num2
EXPORT BOOL WINAPI E3DGetVisiblePolygonNum( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

//1 hsid
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

//2 num1
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVisiblePolygonNum : parameter type error !!!\n" );
		return 1;
	}
	int* num1ptr;
	num1ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

//3 num2
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetVisiblePolygonNum : parameter type error !!!\n" );
		return 1;
	}
	int* num2ptr;
	num2ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ4:変数

////

	if ( *hei->er ) return *hei->er;		// エラーチェック

////

	*num1ptr = 0;//!!!!!!!
	*num2ptr = 0;//!!!!!!!

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVisiblePolygonNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetVisiblePolygonNum( num1ptr, num2ptr );
	if( ret ){
		DbgOut( "E3DGetVisiblePolygonNum : curhs GetVisiblePolygonNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DChkConfGroundPart charaid, groundid, groundpart, mode, diffmaxy, mapminy, result, adjustx, adjusty, adjustz, nx10000, ny10000, nz10000
EXPORT BOOL WINAPI E3DChkConfGroundPart( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : charahsid
	// 2 : groundhsid
	// 3 : groundpart
	// 4 : mode
	// 5 : diffmaxy
	// 6 : mapminy
	// 7 : result
	// 8 : adjustx
	// 9 : adjusty
	// 10 : adjustz
	// 11 : nx10000
	// 12 : ny10000
	// 13 : nz10000

	int curtype;

// 1 : charahsid
	int charahsid;
	charahsid = hei->HspFunc_prm_getdi(0);

// 2 : groundhsid
	int groundhsid;
	groundhsid = hei->HspFunc_prm_getdi(0);

// 3 : groundpart
	int groundpart;
	groundpart = hei->HspFunc_prm_getdi(0);

// 4 : mode
	int mode;
	mode = hei->HspFunc_prm_getdi(0);

// 5 : diffmaxy
	int diffmaxy;
	diffmaxy = hei->HspFunc_prm_getdi(0);

// 6 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

// 7 : result
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart : parameter type error !!!\n" );
		return 1;
	}
	int* resultptr;
	resultptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 8 : adjustx
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart : parameter type error !!!\n" );
		return 1;
	}
	int* adxptr;
	adxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 9 : adjusty
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart : parameter type error !!!\n" );
		return 1;
	}
	int* adyptr;
	adyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 10 : adjustz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart : parameter type error !!!\n" );
		return 1;
	}
	int* adzptr;
	adzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 11 : nx10000
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart : parameter type error !!!\n" );
		return 1;
	}
	int* nx10000ptr;
	nx10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 12 : ny
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart : parameter type error !!!\n" );
		return 1;
	}
	int* ny10000ptr;
	ny10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 13 : nz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart : parameter type error !!!\n" );
		return 1;
	}
	int* nz10000ptr;
	nz10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGroundPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = groundhs->ChkConfGroundPart( charahs, groundpart, mode, diffmaxy, mapminy, resultptr, adxptr, adyptr, adzptr, nx10000ptr, ny10000ptr, nz10000ptr );
	if( ret ){
		DbgOut( "E3DChkConfGroundPart : groundhs ChkConfGroundPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DChkConfGroundPart2 befposx, befposy, befposz, newposx, newposy, newposz, groundhsid, groundpart, mode, diffmaxy, result, adjustx, adjusty, adjustz, nx, ny, nz
EXPORT BOOL WINAPI E3DChkConfGroundPart2( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	// 1 : befposx
	// 2 : befposy
	// 3 : befposz

	// 4 : newposx
	// 5 : newposy
	// 6 : newposz

	// 7 : groundhsid
	// 8 : groundpart

	// 9 : mode
	// 10 : diffmaxy
	// 11 : mapminy
	// 12 : result
	// 13 : adjustx
	// 14 : adjusty
	// 15 : adjustz
	// 16 : nx10000
	// 17 : ny10000
	// 18 : nz10000

	int curtype;

// 1 : befposx
	int befposx;
	befposx = hei->HspFunc_prm_getdi(0);

// 2 : befposy
	int befposy;
	befposy = hei->HspFunc_prm_getdi(0);

// 3 : befposz
	int befposz;
	befposz = hei->HspFunc_prm_getdi(0);

// 4 : newposx
	int newposx;
	newposx = hei->HspFunc_prm_getdi(0);

// 5 : newposy
	int newposy;
	newposy = hei->HspFunc_prm_getdi(0);

// 6 : newposz
	int newposz;
	newposz = hei->HspFunc_prm_getdi(0);

// 7 : groundhsid
	int groundhsid;
	groundhsid = hei->HspFunc_prm_getdi(0);

// 8 : groundpart
	int groundpart;
	groundpart = hei->HspFunc_prm_getdi(0);

// 9 : mode
	int mode;
	mode = hei->HspFunc_prm_getdi(0);

// 10 : diffmaxy
	int diffmaxy;
	diffmaxy = hei->HspFunc_prm_getdi(0);

// 11 : mapminy
	int mapminy;
	mapminy = hei->HspFunc_prm_getdi(0);

// 12 : result
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart2 : parameter type error !!!\n" );
		return 1;
	}
	int* resultptr;
	resultptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 13 : adjustx
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart2 : parameter type error !!!\n" );
		return 1;
	}
	int* adxptr;
	adxptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 14 : adjusty
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart2 : parameter type error !!!\n" );
		return 1;
	}
	int* adyptr;
	adyptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 15 : adjustz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart2 : parameter type error !!!\n" );
		return 1;
	}
	int* adzptr;
	adzptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 16 : nx10000
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart2 : parameter type error !!!\n" );
		return 1;
	}
	int* nx10000ptr;
	nx10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 17 : ny
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart2 : parameter type error !!!\n" );
		return 1;
	}
	int* ny10000ptr;
	ny10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

// 18 : nz
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DChkConfGroundPart2 : parameter type error !!!\n" );
		return 1;
	}
	int* nz10000ptr;
	nz10000ptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGroundPart2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 befv, newv;
	befv.x = (float)befposx;
	befv.y = (float)befposy;
	befv.z = (float)befposz;

	newv.x = (float)newposx;
	newv.y = (float)newposy;
	newv.z = (float)newposz;


	ret = groundhs->ChkConfGroundPart2( groundpart, befv, newv, mode, diffmaxy, mapminy, resultptr, adxptr, adyptr, adzptr, nx10000ptr, ny10000ptr, nz10000ptr );
	if( ret ){
		DbgOut( "E3DChkConfGround : groundhs ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EXPORT BOOL WINAPI E3DGetMidiMusicTime( HSPEXINFO *hei, int p2, int p3, int p4 )
{
	//type $202
	int curtype;

// 1 : soundid
	int soundid;
	soundid = hei->HspFunc_prm_getdi(0);

// 2 : mtimeptr
	curtype = *hei->nptype;
	if( curtype != 4 ){
		DbgOut( "E3DGetMidiMusicTime : parameter type error !!!\n" );
		return 1;
	}
	int* mtimeptr;
	mtimeptr = (int *)hei->HspFunc_prm_getv();	// パラメータ3:変数

//
	if ( *hei->er ) return *hei->er;		// エラーチェック

//////////

	if( !s_HS ){
		DbgOut( "E3DGetMidiMusicTime : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->GetMusicTime( soundid, mtimeptr );
	if( ret ){
		DbgOut( "E3DGetMidiMusicTime : hs GetMusicTime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;
}

EXPORT BOOL WINAPI E3DSetNextMotionFrameNo( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	// type $202
	int hsid;
	hsid = hei->HspFunc_prm_getdi(0);

	int mk;
	mk = hei->HspFunc_prm_getdi(0);

	int nextmk;
	nextmk = hei->HspFunc_prm_getdi(0);

	int nextframeno;
	nextframeno = hei->HspFunc_prm_getdi(0);


	if ( *hei->er ) return *hei->er;		// エラーチェック
/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNextMotionFrameNo( mk, nextmk, nextframeno );
	if( ret ){
		DbgOut( "E3DSetNextMotionFrameNo : hs SetNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

