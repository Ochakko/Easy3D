// easy3d.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//



#include "stdafx.h"
#include "easy3d.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// これは、エクスポートされた変数の例です。
//EASY3D_API int neasy3d=0;

// これは、エクスポートされた関数の例です。
//EASY3D_API int fneasy3d(void)
//{
//	return 42;
//}

// これは、エクスポートされたクラスのコンストラクタです。
// クラス定義に関しては easy3d.h を参照してください。
//Ceasy3d::Ceasy3d()
//{
//	return;
//}
#define E3DVERSIONNO	5518

#define _CRTDBG_MAP_ALLOC

#include <windows.h>

#include <process.h>
#include <advwin32.h>


//#include <windowsx.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <io.h>

#include <crtdbg.h>

#define COEFHSTRING
#include <coef.h>


#include <basedat.h>

#define DBGH
#include <dbg.h>

#include "d3dapp.h"
#include "handlerset.h"
#include "d3dfont.h"

#include <shdhandler.h>
#include <mothandler.h>
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


#include <BoneProp.h>
#include <LiSPSM.h>
#include <Vec3List.h>

#include <cpuid.h>
#include <rtsc.h>
#include <asmfunc.h>

#include "resource.h"

#include "IncludeManager.h"

#include <CameraAnimHandler.h>
#include <CameraAnim.h>
#include <CameraFile.h>

#include "SoundBank.h"
#include "SoundSet.h"
#include "SoundElem.h"
#include <SndAnimHandler.h>
#include <SndAnim.h>
#include <SndSetFile.h>
#include <SndFile.h>
#include "HuSound.h"
#include <SndConv.h>

#include <motioninfo.h>

#include "PluginKinect.h"

#include <DShow.h>

#include <Panda.h>


//#define E3DHSPVERSION	1045

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

static CPanda* s_panda = 0;


#define RGNMAXNUM	25000
static int rgnpoint[RGNMAXNUM][4];
static int rgnpoly[RGNMAXNUM];


static int s_SelectLightType = 0;
static CCameraAnimHandler* m_cah = 0;

//static float s_shadowbias = 0.005f;
static int s_shadowmode = 0;
static float s_shadowcoef[2] = { 0.005f, 1.0f };// [0]: bias, [1]: darkness
static int s_keyboardcnt[256];
static int s_initcomflag = 1;

typedef struct tag_tonemapelem
{
	int tonemapid;
	int tonenum;
	int* pscid;
	int* ptexid;
} TONEMAPELEM;
#define TONEMAPMAX	10
static TONEMAPELEM s_tonemapelem[ TONEMAPMAX ];
static int s_usertonemapcnt = 0;


typedef struct tag_hsidelem
{
	int hsid;
	int validflag;
	CHandlerSet* hsptr;
} HSIDELEM;
#define HSIDMAX	5001
static HSIDELEM s_hsidelem[ HSIDMAX ];


static int s_swapchaincnt = 0;
typedef struct tag_swapchainelem
{
	int scid;
	HWND hwnd;
	IDirect3DSwapChain9* pSwapChain;
	IDirect3DSurface9* pZSurf;
	IDirect3DSurface9* pBackBuffer;
	int texid;
	POINT texsize;
	D3DCOLOR clearcol;
	int needz;
} SWAPCHAINELEM;

#define SCMAX	256
static SWAPCHAINELEM s_sctable[ SCMAX ];


typedef struct tag_bgdispelem
{
	int scid;
	CBgDisp2* bgdisp;
} BGDISPELEM;
static BGDISPELEM s_bgdisp[ SCMAX ];



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
	//int mult100;
	float mult;
	int adjustuvflag;

	char* bufptr;
	int bufsize;

	float maxx;
	float maxz;
	int divx;
	int divz;
	float maxheight;

	int hsid;
	int useflag;
}THREADTABLE;


#define THMAX	4098
static THREADTABLE s_thtable[THMAX];
//static int s_threadnum = 0;

static int s_callinitflag = 0;
static HMODULE s_hMod = 0;

static CSoundBank* s_soundbank = 0;
static CSndAnimHandler* s_sndah = 0;
static int s_sndstartframe = 0;

static LARGE_INTEGER	m_freq;
static LARGE_INTEGER	m_rendercnt;
static LARGE_INTEGER	m_curcnt;
static double			m_dfreq;
static double			m_drenderfreq;
static double			m_drendercnt;
static double			m_dcurcnt;

/////////////////
#define	WM_GRAPH_NOTIFY		(WM_APP + 1)

static IGraphBuilder *s_pGraphBuilder = 0;
static IMediaControl *s_pMediaControl = 0;
static IMediaEventEx *s_pMediaEventEx = 0;
static IVideoWindow *s_pVideoWindow = 0;
static IBasicAudio *s_pBasicAudio = 0;
static int s_playingmovieflag = 0;

static HWND s_movienotifywnd = 0;
LRESULT CALLBACK MovieWndProc(HWND, UINT, WPARAM, LPARAM);
static LONG s_movieoldfunc = 0;

////////////////
// global

//float g_specular = 1.0f;
//float g_ambient = 1.0f;

CPluginKinect* g_kinect = 0;
int g_index32bit = 0;

SLispStatus	g_sStatus;//!< LiSPSMステータス

int g_activateapp = 0;

ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
ID3DXEffect*			g_pPostEffect = NULL;

CRITICAL_SECTION g_crit_hshead;
//CRITICAL_SECTION g_crit_tempmp; 
CRITICAL_SECTION g_crit_restore;

float g_proj_near;// = 100.0f;
float g_proj_far;// = 10000.0f;
float g_proj_fov;// = 45.0f;
float g_fovindex[ FOVINDEXSIZE + 1 ];
int	g_ortho;
float g_orthosize;

int g_fogenable;// = 0;
D3DCOLOR g_fogcolor;// = 0x00FFFFFF;
float g_fogstart;// = 0.0f;
float g_fogend;// = 10000.0f;	
float g_fogtype = 0.0f;
static int s_savefogenable;// = g_fogenable;

int g_rendercnt = 0;
int g_presentinterval = D3DPRESENT_INTERVAL_IMMEDIATE;

D3DXMATRIX g_matProj;
POINT		g_scsize;

DWORD g_renderstate[ D3DRS_BLENDOP + 1 ];
int g_rsindex[80] = {
    D3DRS_ZENABLE, D3DRS_FILLMODE, D3DRS_SHADEMODE, -1, D3DRS_ZWRITEENABLE, 
	D3DRS_ALPHATESTENABLE, D3DRS_LASTPIXEL, D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_CULLMODE,

    D3DRS_ZFUNC, D3DRS_ALPHAREF, D3DRS_ALPHAFUNC, D3DRS_DITHERENABLE, D3DRS_ALPHABLENDENABLE,
	D3DRS_FOGENABLE, D3DRS_SPECULARENABLE, -1, D3DRS_FOGCOLOR, D3DRS_FOGTABLEMODE,
    
	D3DRS_FOGSTART, D3DRS_FOGEND, D3DRS_FOGDENSITY, -1, -1,
	D3DRS_RANGEFOGENABLE, D3DRS_STENCILENABLE, D3DRS_STENCILFAIL, D3DRS_STENCILZFAIL, D3DRS_STENCILPASS,

    D3DRS_STENCILFUNC, D3DRS_STENCILREF, D3DRS_STENCILMASK, D3DRS_STENCILWRITEMASK, D3DRS_TEXTUREFACTOR,
	D3DRS_WRAP0, D3DRS_WRAP1, D3DRS_WRAP2, D3DRS_WRAP3, D3DRS_WRAP4,
    
	D3DRS_WRAP5, D3DRS_WRAP6, D3DRS_WRAP7, D3DRS_CLIPPING, D3DRS_LIGHTING,
	D3DRS_AMBIENT, D3DRS_FOGVERTEXMODE, D3DRS_COLORVERTEX, D3DRS_LOCALVIEWER, D3DRS_NORMALIZENORMALS,

    D3DRS_DIFFUSEMATERIALSOURCE, D3DRS_SPECULARMATERIALSOURCE, D3DRS_AMBIENTMATERIALSOURCE, D3DRS_EMISSIVEMATERIALSOURCE, D3DRS_VERTEXBLEND,
	D3DRS_CLIPPLANEENABLE, -1, D3DRS_POINTSIZE, D3DRS_POINTSIZE_MIN, D3DRS_POINTSPRITEENABLE,

    D3DRS_POINTSCALEENABLE, D3DRS_POINTSCALE_A, D3DRS_POINTSCALE_B, D3DRS_POINTSCALE_C, D3DRS_MULTISAMPLEANTIALIAS,
	D3DRS_MULTISAMPLEMASK, D3DRS_PATCHEDGESTYLE, -1, D3DRS_DEBUGMONITORTOKEN, D3DRS_POINTSIZE_MAX,

    D3DRS_INDEXEDVERTEXBLENDENABLE, D3DRS_COLORWRITEENABLE, D3DRS_TWEENFACTOR, D3DRS_BLENDOP, -1,
	-1,-1,-1,-1,-1
};

static int s_partialrs[ 28 ] = {
	D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_LIGHTING, D3DRS_CULLMODE, D3DRS_SHADEMODE,
	D3DRS_WRAP0, D3DRS_WRAP1, D3DRS_DITHERENABLE, D3DRS_SPECULARENABLE, D3DRS_ZENABLE, 
	D3DRS_AMBIENT, D3DRS_NORMALIZENORMALS, D3DRS_VERTEXBLEND, D3DRS_ZWRITEENABLE, D3DRS_ZFUNC, 
	D3DRS_ALPHAREF, D3DRS_ALPHAFUNC, D3DRS_ALPHABLENDENABLE, D3DRS_MULTISAMPLEANTIALIAS, D3DRS_MULTISAMPLEMASK,
	D3DRS_BLENDOP, D3DRS_TEXTUREFACTOR, D3DRS_FOGENABLE, D3DRS_FOGCOLOR, D3DRS_FOGSTART,
	D3DRS_FOGEND, D3DRS_FOGDENSITY, D3DRS_FOGVERTEXMODE
	
};

DWORD g_minfilter = D3DTEXF_LINEAR;
DWORD g_magfilter = D3DTEXF_LINEAR;

CNaviLine* g_nlhead = 0;


UINT g_miplevels = 1;//!!!!!!!!!!!!!!!!
DWORD g_mipfilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR;

CCpuid g_cpuinfo;

RTSC g_rtsc;

int g_LightOfBump[3] = { 0, -1, -1 };
int g_lightnum = 0;
int g_bumplightnum = 0;

int g_usePostEffect = 1;

//------------------------
//shader
//------------------------
D3DXHANDLE g_hRenderSceneBone4 = 0;
D3DXHANDLE g_hRenderSceneBone1 = 0;
D3DXHANDLE g_hRenderSceneBone0 = 0;
D3DXHANDLE g_hRenderSceneBone4Toon = 0;
D3DXHANDLE g_hRenderSceneBone1Toon = 0;
D3DXHANDLE g_hRenderSceneBone0Toon = 0;
D3DXHANDLE g_hRenderSceneBone4ToonNoTex = 0;
D3DXHANDLE g_hRenderSceneBone1ToonNoTex = 0;
D3DXHANDLE g_hRenderSceneBone0ToonNoTex = 0;
D3DXHANDLE g_hRenderSceneBone4Toon1 = 0;
D3DXHANDLE g_hRenderSceneBone1Toon1 = 0;
D3DXHANDLE g_hRenderSceneBone0Toon1 = 0;
D3DXHANDLE g_hRenderSceneBone4Bump = 0;//vs2
D3DXHANDLE g_hRenderSceneBone1Bump = 0;//vs2
D3DXHANDLE g_hRenderSceneBone0Bump = 0;//vs2
D3DXHANDLE g_hRenderSceneBone4Edge0 = 0;
D3DXHANDLE g_hRenderSceneBone1Edge0 = 0;
D3DXHANDLE g_hRenderSceneBone0Edge0 = 0;
D3DXHANDLE g_hRenderSceneNormalShadow = 0;//vs2
D3DXHANDLE g_hRenderSceneBumpShadow = 0;//vs2
D3DXHANDLE g_hRenderSceneToon1Shadow = 0;//vs2
D3DXHANDLE g_hRenderSceneToon0Shadow = 0;//vs2
D3DXHANDLE g_hRenderSceneBone4PP = 0;
D3DXHANDLE g_hRenderSceneBone0PP = 0;
D3DXHANDLE g_hRenderScenePPShadow = 0;
D3DXHANDLE g_hRenderZN_2path_NormalBone = 0;
D3DXHANDLE g_hRenderZN_2path_NormalNoBone = 0;


D3DXHANDLE g_hmWorldMatrixArray = 0;
D3DXHANDLE g_hmView = 0;
D3DXHANDLE g_hmoldView = 0;
D3DXHANDLE g_hmProj = 0;
D3DXHANDLE g_hmEyePos = 0;
D3DXHANDLE g_hmLightParams = 0;
D3DXHANDLE g_hmBumpLight = 0;//vs2
D3DXHANDLE g_hmFogParams = 0;
D3DXHANDLE g_hmToonParams = 0;
D3DXHANDLE g_hmLightNum = 0;
D3DXHANDLE g_hmEdgeCol0 = 0;
D3DXHANDLE g_hmAlpha = 0;
D3DXHANDLE g_hNormalMap = 0;//vs2
D3DXHANDLE g_hDecaleTex = 0;//vs2
D3DXHANDLE g_hDecaleTex1 = 0;//vs2
D3DXHANDLE g_hmLP = 0;//vs2
D3DXHANDLE g_hmLPB = 0;//vs2
D3DXHANDLE g_hShadowMap = 0;//vs2
D3DXHANDLE g_hmShadowCoef = 0;//vs2
D3DXHANDLE g_hmMinAlpha = 0;
D3DXHANDLE g_hmGlowMult = 0;
D3DXHANDLE g_hmToonDiffuse = 0;
D3DXHANDLE g_hmToonAmbient = 0;
D3DXHANDLE g_hmToonSpecular = 0;
D3DXHANDLE g_hmZNUser1 = 0;
D3DXHANDLE g_hmZNUser2 = 0;
/////
D3DXHANDLE g_hPostEffectBlur0 = 0;
D3DXHANDLE g_hPostEffectGauss = 0;
D3DXHANDLE g_hPostEffectTLV = 0;
D3DXHANDLE g_hPostEffectDownScale = 0;
D3DXHANDLE g_hPostEffectToneMap = 0;
D3DXHANDLE g_hPostEffectBrightPass = 0;
D3DXHANDLE g_hPostEffectHDRFinal = 0;
D3DXHANDLE g_hPostEffectMono = 0;
D3DXHANDLE g_hPostEffectCbCr = 0;
D3DXHANDLE g_hPostEffectUser[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


D3DXHANDLE g_hpeWidth0 = 0;
D3DXHANDLE g_hpeHeight0 = 0;
D3DXHANDLE g_hpeWidth1 = 0;
D3DXHANDLE g_hpeHeight1 = 0;
D3DXHANDLE g_hpeTexture0 = 0;
D3DXHANDLE g_hpeTexture1 = 0;
D3DXHANDLE g_hpeTexture2 = 0;
D3DXHANDLE g_hpeCbCr = 0;


D3DXHANDLE g_hpeWeight = 0;
D3DXHANDLE g_hpeOffsetX = 0;
D3DXHANDLE g_hpeOffsetY = 0;
D3DXHANDLE g_hpeSampleWeight = 0;
D3DXHANDLE g_hpeSampleOffset = 0;

D3DXHANDLE g_hpeMiddleGray = 0;
D3DXHANDLE g_hpeBrightPassThreshold = 0;
D3DXHANDLE g_hpeBrightPassOffset = 0;
D3DXHANDLE g_hpeColMult = 0;
D3DXHANDLE g_hpeUserFl4[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
D3DXHANDLE g_hpeUserTex[5] = {0, 0, 0, 0, 0};


/////// global func

int e3dRestoreDeviceObject( int resetflag );
int e3dInvalidateDeviceObject( int resetflag );
CHandlerSet* GetHandlerSet( int hsid );

///////////////////
// extern

extern int g_useGPU;
extern int g_chkVS;
extern CD3DApplication* g_pD3DApp = 0;
extern CLightData* g_lighthead;// d3ddisp.cppで定義、使用。

extern CHandlerSet* g_bbhs;

extern CTexBank* g_texbnk;// shdhandler.cpp


extern DWORD g_cop0;
extern DWORD g_cop1;
extern DWORD g_aop0;
extern DWORD g_aop1;

extern int g_bumpEnable;
extern int g_bumpUse;
extern int g_shadowEnable;
extern int g_shadowUse;


//////////////////
// static
//static CHandlerSet* hshead = 0;
static CAviData* avihead = 0;
static CXFont* xfonthead = 0;

static HWND s_hwnd = 0;
static HWND s_PBwnd = 0;
static HINSTANCE s_inst = 0;


static DWORD s_clientWidth = 0;
static DWORD s_clientHeight = 0;

static D3DXMATRIX s_matView;
static D3DXMATRIX s_matViewShadow;
static D3DXMATRIX s_matProjShadow;
static D3DXMATRIX s_matBiasShadow;
static LPDIRECT3DTEXTURE9 s_ShadowMapTex = NULL;


static LPD3DXSPRITE	s_d3dxsprite = 0;
static CMySprite* s_spritehead = 0;

static CQHandler* s_qh = 0;
static CQuaternion2 s_cameraq( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );

static CHuSound* s_HS = 0;

static int s_multisample = 0;

////////////////////
////////////////////
static int CameraElemToParams( CAMERAELEM ce );

static CAviData* GetAviData( int aviid );
static CXFont* GetXFont( int fontid );
static CLightData* GetLightData( int lid );
static CMySprite* GetSprite( int spriteid );

static CNaviLine* GetNaviLine( int nlid );

static int ChainNewHandlerSet( CHandlerSet* newhs );

static int Restore( CHandlerSet* hsptr, int billboardid, CPanda* panda = 0, int pndid = 0 );
static int FreeHandlerSet();
//static int DirToTheOtherXZ( E3DCHARACTERSET* e3d1, E3DCHARACTERSET e3d2 );
//static int DirToTheOther( E3DCHARACTERSET* e3d1, E3DCHARACTERSET e3d2 );
static int DirToTheOtherXZ( CGPData* e3d1, CGPData e3d2 );
static int DirToTheOther( CGPData* e3d1, CGPData e3d2 );

static int SeparateFrom( int hsid1, int hsid2, float dist );
//static int CheckOutOfRange( E3DCHARACTERSET* charaptr );
static int CheckOutOfRange( CGPData* charaptr );
static int CreateAfterImage( int hsid, LPDIRECT3DDEVICE9 pdev, int imagenum );
static int MoveGirlXZ( int hsid, int keyid );

static int SetModuleDir();
static int SetResDir0();
static int SetResDir( char* filename, CHandlerSet* hsptr );

static int DestroySoundObject();

static int SetFovIndex();

static int SetDefaultRenderState();
static int SetPartialRenderState( LPDIRECT3DDEVICE9 pdev );

static int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec );
static int DestroyHandlerSet( int hsid );
static int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag );


static int CatmullRomPoint( D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* p3, D3DXVECTOR3* p4, float t, D3DXVECTOR3* resp );
static int GetScreenPos( D3DXVECTOR3 vec3d, D3DXVECTOR3* scposptr );

static int CalcMatView();
static int ScreenToObj( D3DXVECTOR3* objpos, D3DXVECTOR3 scpos );
static int GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir );

static int DestroyTexture( int hsid, int bbid, int spriteid, int dirtyreset, int bgno );


static int s_cameratype = 0;
static D3DXVECTOR3 s_camerapos( 0.0f, 0.0f, 4200.0f );
static float s_camera_degxz = 0.0f;
static float s_camera_degy = 0.0f;
static D3DXVECTOR3 s_cameratarget( 0.0f, 0.0f, 0.0f );
static D3DXVECTOR3 s_cameraupvec( 0.0f, 1.0f, 0.0f );
static float s_camera_twist = 0.0f;

static D3DXVECTOR3 s_camoffbef( 0.0f, 0.0f, 0.0f );
static D3DXVECTOR3 s_camoffaft( 0.0f, 0.0f, 0.0f );
static CQuaternion2 s_camoffq( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );

static D3DXVECTOR3 s_camposshadow( 0.0f, 10000.0f, 10000.0f );
static D3DXVECTOR3 s_camtarshadow( 0.0f, 0.0f, 0.0f );
static D3DXVECTOR3 s_camupshadow( 0.0f, 1.0f, 0.0f );

static D3DXVECTOR3 s_lightdirshadow( 0.0f, -1.0f, 1.0f );

static int byeflag = 0;


static int SetMovableArea_F( char* tempname1, float maxx, float maxz, int divx, int divz, float wallheight, int* hsidptr );
static DWORD WINAPI ThreadSetMovableArea( LPVOID lpThreadParam );

static int LoadMQOFileAsGround_F( char* tempname1, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype );
static DWORD WINAPI ThreadLoadMQOFileAsGround( LPVOID lpThreadParam );

static int LoadMQOFileAsChara_F( char* tempname1, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype );

static int SigLoad_F( char* tempname, float mult, int adjustuvflag, int* hsidptr );
static DWORD WINAPI ThreadSigLoad( LPVOID lpThreadParam );

static int SigLoadFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadSigLoadFromBuf( LPVOID lpThreadParam );

static int LoadMQOFileAsMovableArea_F( char* tempname, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadMQOFileAsMovableArea( LPVOID lpThreadParam );

static int LoadMQOFileAsMovableAreaFromBuf_F( char* bufptr, int bufsize, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadMQOFileAsMovableAreaFromBuf( LPVOID lpThreadParam );

static int LoadGroundBMP_F( char* tempname1, char* tempname2, char* tempname3, char* tempname4, float maxx, float maxz, int divx, int divz, float maxheight, int TLmode, int* hsidptr );
static DWORD WINAPI ThreadLoadGroundBMP( LPVOID lpThreadParam );

static int LoadGNDFile_F( char* tempname, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadGNDFile( LPVOID lpThreadParam );

static int LoadGNDFileFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, int* hsidptr );
static DWORD WINAPI ThreadLoadGNDFileFromBuf( LPVOID lpThreadParam );

static int LoadSigFileAsGround_F( char* tempname, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadSigFileAsGround( LPVOID lpThreadParam );

static int LoadSigFileAsGroundFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr );
static DWORD WINAPI ThreadLoadSigFileAsGroundFromBuf( LPVOID lpThreadParam );

static int AddMotion_F( int hsid, char* tempname, int* cookieptr, int* maxnumptr, float mvmult );
static DWORD WINAPI ThreadAddMotion( LPVOID lpThreadParam );

static int AddMotionFromBuf_F( int hsid, int datakind, char* bufptr, int bufsize, int* cookieptr, int* maxnumptr, float mvmult );
static DWORD WINAPI ThreadAddMotionFromBuf( LPVOID lpThreadParam );

static int LoadMQOFileAsGroundFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr );


static SWAPCHAINELEM* GetFreeSwapChainElem();
static SWAPCHAINELEM* GetSwapChainElem( int scid );
static int DestroySwapChainElem( SWAPCHAINELEM* cursc, int initflag );
static int CreateSwapChainElem( int scid, SWAPCHAINELEM* cursc, HWND srchwnd, int texid, D3DCOLOR* clearcol, int needz );

static BGDISPELEM* GetFreeBGDispElem();
static BGDISPELEM* GetBGDispElem( int scid );
int GetName( char* dstname, int srcleng );

static int DisableFogState();
static int RollbackFogState();

//static D3DXVECTOR3 s_cameradir( 0.0f, 0.0f, -1.0f );
static int s_afterimagenum = 5;

static float s_movestep = 100.0f;

static LONGLONG s_timerfreq = 0;

static int s_usertexcnt = 0;


static int XORFuncBeta( unsigned char* newbuf, int bufleng, int* xorptr );

static int InitThTableElem( int thno );
static int GetFreeThTable();
static int FreeThTableElem( int tableno );
static int SetShaderConst( int shadowflag );
static int GetShaderHandle();

static TONEMAPELEM* GetFreeToneMapElem();
static TONEMAPELEM* GetToneMapElem( int tonemapid );
static int ToneMapFirst( int scid, int texid );
static int ToneMapSecond( int scid, int texid );
static int ToneMapFinal( int scid, int texid );


static int SetMatProj( SWAPCHAINELEM* cursc );

static int SetPointListForShadow( CVec3List* plist, float aspect, int* hsidptr, int num );
static int ChkInViewAndTransformBatch( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int passno, int needchkinview, int needtrans );
static int SetRenderTarget( LPDIRECT3DDEVICE9 pdev, int scid, int shadowmapflag, int skipflag );
static int RenderShadowMap0( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num );
static int RenderShadowMap1( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int withalpha );
static int SetTempFogEnable( LPDIRECT3DDEVICE9 pdev, int srcenable );
static int RestoreTempFogEnable();
static int RenderNormalBatch( int scid, int* hsidptr, int num, int needchkinview, int skipflag, int* modeptr );
static int RenderBatch( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int withalpha, int* modeptr );

static int SetEffectDecl( LPDIRECT3DDEVICE9 pdev );
static IDirect3DVertexDeclaration9* s_effectdecl = 0;
static IDirect3DVertexDeclaration9* s_tlvdecl = 0;


static int RTBeginScene( LPDIRECT3DDEVICE9 pdev, int scid, int skipbeginscene, int needz );
static int SetRTRenderState( LPDIRECT3DDEVICE9 pdev, int needz );
static int SetGaussWeight( float dispersion );
static int SetGauss5x5Weight( float fw, float fh, float dispersion );
static int SetDownScale4x4Offset( float fw, float fh );
static int SetDownScale2x2Offset( float fw, float fh );
static int SetToneMapFirstOffset( float fw, float fh );

static int RestoreBG( LPDIRECT3DDEVICE9 pdev, int resetflag );

static int CalcSndFrameData( int animno );


////////////////////////
/// ** static funcの実装
int e3dInvalidateDeviceObject( int resetflag )
{
	int ret;

	
//handlerset
	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		HSIDELEM* curhse = s_hsidelem + hsidno;
		if( curhse->validflag && curhse->hsptr ){
			ret = curhse->hsptr->Invalidate( resetflag );
			if( ret ){
				DbgOut( "e3dInvalidateDeviceObject : curhs Invalidate error!!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

//sprite
	if( s_d3dxsprite != NULL ){
		s_d3dxsprite->Release();
		s_d3dxsprite = NULL;
	}
	
	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}
	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		ret = cursprite->Invalidate( resetflag );
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : cursprite InvalidateTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		cursprite = cursprite->next;
	}

//billboard
	if( g_bbhs ){
		ret = g_bbhs->Invalidate( resetflag );
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

//swapchain
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid > 0 ){
			DestroySwapChainElem( &(s_sctable[scno]), 0 );
		}
	}

// texbank RenderTarget
	if( g_texbnk ){
		ret = g_texbnk->InvalidatePoolDefault();
		if( ret ){
			DbgOut( "e3dInvalidateDeviceObject : texbank InvalidatePoolDefault error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



//effect
	if( g_pEffect ){
		HRESULT hres;
		hres = g_pEffect->OnLostDevice();
		_ASSERT( hres == D3D_OK );
	}
	if( g_pPostEffect ){
		HRESULT hres;
		hres = g_pPostEffect->OnLostDevice();
		_ASSERT( hres == D3D_OK );
	}

	return 0;
}


int e3dRestoreDeviceObject( int resetflag )
{
	int ret;
	HRESULT hr;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

//effect
	HRESULT hres;
	if( g_pEffect ){
		hres = g_pEffect->OnResetDevice();
		_ASSERT( hres == D3D_OK );
	}
	if( g_pPostEffect ){
		hres = g_pPostEffect->OnResetDevice();
		_ASSERT( hres == D3D_OK );
	}

	ret = SetEffectDecl( pdev );
	_ASSERT( !ret );



//handlerset	
	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			CHandlerSet* curhs = s_hsidelem[hsidno].hsptr;
			ret = SetResDir( curhs->GetResDir(), curhs );
			_ASSERT( !ret );

			ret = Restore( curhs, -1 );
			if( ret ){
				DbgOut( "e3dRestoreDeviceObject error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
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
		ret = SetResDir( g_bbhs->GetResDir(), g_bbhs );
		_ASSERT( !ret );
		
		ret = Restore( g_bbhs, -2 );// -2はビルボード全部
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : bbhs Restore error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
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

//texbank RenderTarget
	if( g_texbnk ){
		ret = g_texbnk->RestorePoolDefault( pdev );
		if( ret ){
			DbgOut( "e3dRestoreDeviceObject : texbank RestorePoolDefault error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
//swapchain
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid > 0 ){
			ret = CreateSwapChainElem( s_sctable[scno].scid, &(s_sctable[scno]), 
				s_sctable[scno].hwnd, s_sctable[scno].texid, &(s_sctable[scno].clearcol), s_sctable[scno].needz );
			if( ret ){
				DbgOut( "RestoreDeviceObject : CreateSwapChainElem error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	s_sctable[0].pSwapChain = NULL;
	s_sctable[0].pZSurf = g_pD3DApp->m_pZSurf;
	s_sctable[0].pBackBuffer = g_pD3DApp->m_pBackBuffer;


//bgdisp  invalidateも行う。
	ret = RestoreBG( pdev, resetflag );
	_ASSERT( !ret );


	return 0;
}

int RestoreBG( LPDIRECT3DDEVICE9 pdev0, int resetflag )
{
	int ret;
	int scno = 0;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	for( scno = 0; scno < SCMAX; scno++ ){
		BGDISPELEM* curbgelem;
		curbgelem = &(s_bgdisp[scno]);

		if( curbgelem && (curbgelem->scid >= 0) && (curbgelem->bgdisp) ){

			int curscid = curbgelem->scid;
			char name1[MAX_PATH];
			char name2[MAX_PATH];
			ZeroMemory( name1, sizeof( char ) * MAX_PATH );
			ZeroMemory( name2, sizeof( char ) * MAX_PATH );
			strcpy_s( name1, MAX_PATH, curbgelem->bgdisp->texname );
			strcpy_s( name2, MAX_PATH, curbgelem->bgdisp->texname2 );
			int trans1, trans2;
			trans1 = curbgelem->bgdisp->transparent1;
			trans2 = curbgelem->bgdisp->transparent2;
			float uanime, vanime;
			uanime = curbgelem->bgdisp->uanime;
			vanime = curbgelem->bgdisp->vanime;
			COLORREF col;
			col = curbgelem->bgdisp->col;
			int isround;
			isround = curbgelem->bgdisp->isround;
			float fogdist;
			fogdist = curbgelem->bgdisp->m_fogdist;
			UV startuv;
			UV enduv;
			startuv = curbgelem->bgdisp->m_startuv;
			enduv = curbgelem->bgdisp->m_enduv;
			int srcfixsize = 0;

			delete curbgelem->bgdisp;
			curbgelem->bgdisp = 0;
			curbgelem->scid = -1;

			BGDISPELEM* newbgelem;
			if( curbgelem ){
				newbgelem = curbgelem;
				newbgelem->scid = curscid;
			}else{
				newbgelem = GetFreeBGDispElem();
			}
			if( !newbgelem ){
				DbgOut( "e3dRestoreDeviceObject : CreateBG : newbg NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			CBgDisp2* newbg;
			newbg = new CBgDisp2( 0, pdev, name1, 0, name2, 0, uanime, vanime, col, isround, fogdist, startuv, enduv, 0 );
			if( !newbg ){
				DbgOut( "e3dRestoreDeviceObject : newbg alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

DbgOut( "RestoreBG : %s, %s, uani %f, vani %f, col %x, round %d, fogdist %f, start(%f,%f), end(%f,%f)\r\n",
	name1, name2, uanime, vanime, col, isround, fogdist, startuv.u, startuv.v, enduv.u, enduv.v );


			newbgelem->bgdisp = newbg;
			newbgelem->scid = curscid;


		}

	}

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
	char* lasten = 0;
	lasten = strrchr( filename, ch );
	if( !lasten ){
		_ASSERT( 0 );
		DbgOut( "SetResDir : strrchr error !!!\n" );
		return 1;
	}

	*lasten = 0;

	char* last2en = 0;
	char* last3en = 0;
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
	leng = (int)( endptr - filename + 1 );
	if( leng >= 1024 )
		leng = 1023;
	strncpy_s( resdir, 1024, filename, leng );

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
		strncpy_s( dirname, _MAX_PATH, pathname, dirleng );

		::SetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPCTSTR)dirname );
		DbgOut( "e3d : SetResDir : RESDIR : %s\n", dirname );
	}else{
		SetResDir0();
	}

	if( hsptr && pathname ){
		//int namelen;
		//namelen = (int)strlen( pathname );
		//if( namelen < MAX_PATH ){
		//	strcpy( hsptr->m_resdir, pathname );
		//}else{
		//	DbgOut( "e3d : SetResDir : name too long warning !!! %s \n", pathname );
		//}
		int ret;
		ret = hsptr->SetResDir( pathname );
		if( ret ){
			DbgOut( "e3d : SetResDir : hs SetResDir error !!!\n" );
			_ASSERT( 0 );
			return 1;
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
	leng = (int)( lasten - filename + 1 );
	if( leng >= 1024 )
		leng = 1023;
	strncpy_s( moduledir, 1024, filename, leng );

	::SetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPCTSTR)moduledir );
	
	DbgOut( "SetModuleDir : %s\n", moduledir );
	return 0;
}

int DestroySoundObject()
{
	if( s_sndah ){
		delete s_sndah;
		s_sndah = 0;
	}

	if( s_soundbank ){
		delete s_soundbank;
		s_soundbank = 0;
	}

	if( s_HS ){
		delete s_HS;
		s_HS = 0;
	}

	return 0;
}

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

int SetPartialRenderState( LPDIRECT3DDEVICE9 pdev )
{

	HRESULT hr;
	int pno;
	int prs;
	for( pno = 0; pno < 28; pno++ ){
		prs = s_partialrs[ pno ];

		if( prs >= 0 ){
			hr = pdev->SetRenderState( (D3DRENDERSTATETYPE)prs, g_renderstate[ prs ] );
			if( hr != D3D_OK ){
				DbgOut( "e3d : SetPartialRenderState : SetRenderState : %d error !!!\r\n", prs );
			}
		}
	}

	///////////////////

    hr = pdev->SetSamplerState( 0, D3DSAMP_MINFILTER, g_minfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MINFILTER 0 : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 0, D3DSAMP_MAGFILTER, g_magfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MAGFILTER 0 : error !!!\r\n" );
	}

    hr = pdev->SetSamplerState( 1, D3DSAMP_MINFILTER, g_minfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MINFILTER 1 : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 1, D3DSAMP_MAGFILTER, g_magfilter );
	if( hr != D3D_OK ){
		DbgOut( "e3d : SetPartialRenderState : SetSamplerState : D3DSAMP_MAGFILTER 1 : error !!!\r\n" );
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
	//g_renderstate[D3DRS_LINEPATTERN] = 0;
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
	//g_renderstate[D3DRS_FOGENABLE] = FALSE;
g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
	g_renderstate[D3DRS_SPECULARENABLE] = TRUE;//<----------
	//g_renderstate[D3DRS_ZVISIBLE] = 0;
g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
	g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;


g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
	g_renderstate[D3DRS_FOGDENSITY ] = *((DWORD*)&fogdensity);
	//g_renderstate[D3DRS_EDGEANTIALIAS] = FALSE;
	//g_renderstate[D3DRS_ZBIAS] = 0;
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
g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
	g_renderstate[D3DRS_COLORVERTEX] = TRUE;
	g_renderstate[D3DRS_LOCALVIEWER] = TRUE;
	g_renderstate[D3DRS_NORMALIZENORMALS] = FALSE;

	g_renderstate[D3DRS_DIFFUSEMATERIALSOURCE] = D3DMCS_COLOR1;
	g_renderstate[D3DRS_SPECULARMATERIALSOURCE] = D3DMCS_COLOR2;
	g_renderstate[D3DRS_AMBIENTMATERIALSOURCE] = D3DMCS_COLOR2;
	g_renderstate[D3DRS_EMISSIVEMATERIALSOURCE] = D3DMCS_MATERIAL;
	g_renderstate[D3DRS_VERTEXBLEND] = D3DVBF_DISABLE;
	g_renderstate[D3DRS_CLIPPLANEENABLE] = 0;
	//g_renderstate[D3DRS_SOFTWAREVERTEXPROCESSING] = FALSE;//!!!
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
	//g_renderstate[D3DRS_PATCHSEGMENTS] = *((DWORD*)&numsegments);
	g_renderstate[D3DRS_DEBUGMONITORTOKEN] = D3DDMT_ENABLE;
	g_renderstate[D3DRS_POINTSIZE_MAX] = *((DWORD*)&pointsize);


	g_renderstate[D3DRS_INDEXEDVERTEXBLENDENABLE] = TRUE;//!!!
	g_renderstate[D3DRS_COLORWRITEENABLE] = 0x0000000F;
	g_renderstate[D3DRS_TWEENFACTOR] = *((DWORD*)&tweenfactor);
	g_renderstate[D3DRS_BLENDOP] = D3DBLENDOP_ADD;

////////

	g_minfilter = D3DTEXF_LINEAR;
	g_magfilter = D3DTEXF_LINEAR;

	return 0;

}

int InitThTableElem( int thno )
{
	if( (thno < 0) || (thno >= THMAX) ){
		DbgOut( "e3d : InitThTableElem : thno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	s_thtable[thno].threadid = -1;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].dwId = -1;
	s_thtable[thno].returnval = -1;
	s_thtable[thno].returnval2 = -1;

	s_thtable[thno].filename = 0;
	s_thtable[thno].filename2 = 0;
	s_thtable[thno].filename3 = 0;
	s_thtable[thno].filename4 = 0;
	s_thtable[thno].mult = 1.0f;
	s_thtable[thno].adjustuvflag = 0;

	s_thtable[thno].bufptr = 0;
	s_thtable[thno].bufsize = 0;

	s_thtable[thno].maxx = 0;
	s_thtable[thno].maxz = 0;
	s_thtable[thno].divx = 0;
	s_thtable[thno].divz = 0;
	s_thtable[thno].maxheight = 0;

	s_thtable[thno].hsid = -1;

	s_thtable[thno].useflag = 0;

	return 0;
}
int GetFreeThTable()
{
	int retth = -1;

	int thno;
	for( thno = 0; thno < THMAX; thno++ ){
		if( s_thtable[thno].useflag == 0 ){
			retth = thno;
			InitThTableElem( thno );//!!!!!!!!!!!!!!
			break;
		}
	}

	return retth;
}
int FreeThTableElem( int tableno )
{
	if( (tableno < 0) || (tableno >= THMAX) ){
		DbgOut( "e3d : FreeThTableElem : thno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s_thtable[tableno].hThread ){
		WaitForSingleObject( s_thtable[tableno].hThread, 600000 );
		CloseHandle( s_thtable[tableno].hThread );
		s_thtable[tableno].hThread = NULL;
	}
	
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

	s_thtable[tableno].useflag = 0;//!!!!!!!!!!!!!!!

	return 0;
}

int FreeHandlerSet()
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		HSIDELEM* curhse = s_hsidelem + hsidno;
		if( curhse->validflag && curhse->hsptr ){
			delete curhse->hsptr;
		}
		curhse->hsid = hsidno;
		curhse->validflag = 0;
		curhse->hsptr = 0;
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	if( g_bbhs ){
		delete g_bbhs;
		g_bbhs = 0;
	}


	return 0;
}

int ChainNewHandlerSet( CHandlerSet* newhs )
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	int freehsid = -1;
	int hsidno;
	for( hsidno = 1; hsidno < HSIDMAX; hsidno++ ){// 1から！！！　０は使わない。
		if( s_hsidelem[hsidno].validflag == 0 ){
			freehsid = hsidno;
			break;
		}
	}

	if( freehsid <= 0 ){
		DbgOut( "ChainNewHandlerSet : freehsid error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 1;
	}

	s_hsidelem[freehsid].hsptr = newhs;
	s_hsidelem[freehsid].validflag = 1;
	newhs->serialno = s_hsidelem[freehsid].hsid;//!!!!!!!!!!!!!


	LeaveCriticalSection( &g_crit_hshead );//###### end crit

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
int SetShaderConst( int shadowflag )
{
	HRESULT hres;


	if( shadowflag == 0 ){

		hres = g_pEffect->SetMatrix( g_hmView, &s_matView );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : view set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pEffect->SetMatrix( g_hmProj, &g_matProj );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : proj set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&s_camerapos), sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : eyepos set error !!!\n" );
			_ASSERT( 0 );
			return 1;	
		}


	//float3 mLightParams[5][7];
	//0: x=validflag, y=type, z=divRange
	//1: x=cosPhi, y=divCosPhi
	//2: xyz=diffuse
	//3: xyz=specular
	//4: xyz=position
	//5: xyz=direction

		float tmplparam[5][6][4];
		ZeroMemory( tmplparam, sizeof( float ) * 5 * 6 * 4 );

		CLightData* curl;
		curl = g_lighthead;
		int setlno = 0;
		int lno;
		for( lno = 0; lno < 5; lno++ ){
			if( curl ){
				tmplparam[lno][0][0] = 1.0f;

				tmplparam[lno][2][0] = curl->Diffuse.x;
				tmplparam[lno][2][1] = curl->Diffuse.y;
				tmplparam[lno][2][2] = curl->Diffuse.z;

				tmplparam[lno][3][0] = curl->Specular.x;
				tmplparam[lno][3][1] = curl->Specular.y;
				tmplparam[lno][3][2] = curl->Specular.z;

				switch( curl->Type ){
				case D3DLIGHT_DIRECTIONAL:
					tmplparam[lno][0][1] = 0.0f;

					tmplparam[lno][5][0] = curl->orgDirection.x;
					tmplparam[lno][5][1] = curl->orgDirection.y;
					tmplparam[lno][5][2] = curl->orgDirection.z;

					break;
				case D3DLIGHT_POINT:
					tmplparam[lno][0][1] = 1.0f;

					tmplparam[lno][0][2] = curl->divRange;


					tmplparam[lno][4][0] = curl->orgPosition.x;
					tmplparam[lno][4][1] = curl->orgPosition.y;
					tmplparam[lno][4][2] = curl->orgPosition.z;

					break;
				case D3DLIGHT_SPOT:
					tmplparam[lno][0][1] = 2.0f;

					tmplparam[lno][0][2] = curl->divRange;
					tmplparam[lno][1][0] = curl->cosPhi;
					tmplparam[lno][1][1] = curl->divcosPhi;

					tmplparam[lno][4][0] = curl->orgPosition.x;
					tmplparam[lno][4][1] = curl->orgPosition.y;
					tmplparam[lno][4][2] = curl->orgPosition.z;

					tmplparam[lno][5][0] = curl->orgDirection.x;
					tmplparam[lno][5][1] = curl->orgDirection.y;
					tmplparam[lno][5][2] = curl->orgDirection.z;

					break;
				}
					
				curl = curl->nextlight;
				setlno++;

			}else{
				tmplparam[lno][0][0] = 0.0f;
				tmplparam[lno][0][1] = -1.0f;
			}
		}

		float lbump[3][6][4];
		ZeroMemory( lbump, sizeof( float ) * 3 * 6 * 4 );

		int bulno;
		int bulsetno = 0;
		for( bulno = 0; bulno < 3; bulno++ ){
			CLightData* lbptr;
			lbptr = GetLightData( g_LightOfBump[ bulno ] );
			if( (bulno == 0) && !lbptr && g_lighthead ){
				lbptr = g_lighthead;//存在しなかったら先頭ライト
			}

			if( lbptr ){
				lbump[bulsetno][0][0] = 1.0f;

				lbump[bulsetno][2][0] = lbptr->Diffuse.x;
				lbump[bulsetno][2][1] = lbptr->Diffuse.y;
				lbump[bulsetno][2][2] = lbptr->Diffuse.z;

				lbump[bulsetno][3][0] = lbptr->Specular.x;
				lbump[bulsetno][3][1] = lbptr->Specular.y;
				lbump[bulsetno][3][2] = lbptr->Specular.z;

				switch( lbptr->Type ){
				case D3DLIGHT_DIRECTIONAL:
					lbump[bulsetno][0][1] = 0.0f;

					lbump[bulsetno][5][0] = lbptr->orgDirection.x;
					lbump[bulsetno][5][1] = lbptr->orgDirection.y;
					lbump[bulsetno][5][2] = lbptr->orgDirection.z;

					break;
				case D3DLIGHT_POINT:
					lbump[bulsetno][0][1] = 1.0f;

					lbump[bulsetno][0][2] = lbptr->divRange;


					lbump[bulsetno][4][0] = lbptr->orgPosition.x;
					lbump[bulsetno][4][1] = lbptr->orgPosition.y;
					lbump[bulsetno][4][2] = lbptr->orgPosition.z;

					break;
				case D3DLIGHT_SPOT:
					lbump[bulsetno][0][1] = 2.0f;

					lbump[bulsetno][0][2] = lbptr->divRange;
					lbump[bulsetno][1][0] = lbptr->cosPhi;
					lbump[bulsetno][1][1] = lbptr->divcosPhi;

					lbump[bulsetno][4][0] = lbptr->orgPosition.x;
					lbump[bulsetno][4][1] = lbptr->orgPosition.y;
					lbump[bulsetno][4][2] = lbptr->orgPosition.z;

					lbump[bulsetno][5][0] = lbptr->orgDirection.x;
					lbump[bulsetno][5][1] = lbptr->orgDirection.y;
					lbump[bulsetno][5][2] = lbptr->orgDirection.z;

					break;
				}

				bulsetno++;
			}
		}
		g_bumplightnum = bulsetno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if( g_bumplightnum == 0 ){

			//dummy data
			lbump[0][0][0] = 1.0f;

			lbump[0][2][0] = 0.0f;
			lbump[0][2][1] = 0.0f;
			lbump[0][2][2] = 0.0f;

			lbump[0][3][0] = 0.0f;
			lbump[0][3][1] = 0.0f;
			lbump[0][3][2] = 0.0f;

			///////directional
			lbump[0][0][1] = 0.0f;

			lbump[0][5][0] = 0.0f;
			lbump[0][5][1] = 0.0f;
			lbump[0][5][2] = 1.0f;

			g_bumplightnum = 1;
		}



		if( (g_chkVS >= 20) || (g_chkVS == 0) ){
			hres = g_pEffect->SetValue( g_hmLightParams, &(tmplparam[0][0][0]), sizeof( float ) * 3 * 6 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLightParam SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->SetValue( g_hmBumpLight, &(lbump[0][0][0]), sizeof( float ) * 3 * 6 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mBumpLight SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}else{
			hres = g_pEffect->SetValue( g_hmLightParams, &(tmplparam[0][0][0]), sizeof( float ) * 1 * 6 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLightParam SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		g_lightnum = setlno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	}else{
		//shadowmap
		if( (g_chkVS >= 20) || (g_chkVS == 0) ){
			D3DXMATRIX matLP;
			matLP = s_matViewShadow * s_matProjShadow;
			hres = g_pEffect->SetMatrix( g_hmLP, &matLP );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLP SetMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			D3DXMATRIX matLPB;
			matLPB = matLP * s_matBiasShadow;
			hres = g_pEffect->SetMatrix( g_hmLPB, &matLPB );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLPB SetMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->SetValue( g_hmShadowCoef, &(s_shadowcoef[0]), sizeof( float ) * 2 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mShadowCoef SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			hres = g_pEffect->SetTexture( g_hShadowMap, s_ShadowMapTex );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : ShadowMap SetTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

/***
int SetShaderConst( int shadowflag )
{
	HRESULT hres;


	if( shadowflag == 0 ){

		hres = g_pEffect->SetMatrix( g_hmView, &s_matView );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : view set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pEffect->SetMatrix( g_hmProj, &g_matProj );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : proj set error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->SetValue( g_hmEyePos, (void*)(&s_camerapos), sizeof( float ) * 3 );
		if( hres != D3D_OK ){
			DbgOut( "SetShaderConst : eyepos set error !!!\n" );
			_ASSERT( 0 );
			return 1;	
		}


	//float3 mLightParams[5][7];
	//0: x=validflag, y=type, z=divRange
	//1: x=cosPhi, y=divCosPhi
	//2: xyz=diffuse
	//3: xyz=specular
	//4: xyz=ambient
	//5: xyz=position
	//6: xyz=direction

		float tmplparam[5][7][4];
		ZeroMemory( tmplparam, sizeof( float ) * 5 * 7 * 4 );

		CLightData* curl;
		curl = g_lighthead;
		int setlno = 0;
		int lno;
		for( lno = 0; lno < 5; lno++ ){
			if( curl ){
				tmplparam[lno][0][0] = 1.0f;

				tmplparam[lno][2][0] = curl->Diffuse.x;
				tmplparam[lno][2][1] = curl->Diffuse.y;
				tmplparam[lno][2][2] = curl->Diffuse.z;

				tmplparam[lno][3][0] = curl->Specular.x;
				tmplparam[lno][3][1] = curl->Specular.y;
				tmplparam[lno][3][2] = curl->Specular.z;

				tmplparam[lno][4][0] = curl->Ambient.x;
				tmplparam[lno][4][1] = curl->Ambient.y;
				tmplparam[lno][4][2] = curl->Ambient.z;

				switch( curl->Type ){
				case D3DLIGHT_DIRECTIONAL:
					tmplparam[lno][0][1] = 0.0f;

					tmplparam[lno][6][0] = curl->orgDirection.x;
					tmplparam[lno][6][1] = curl->orgDirection.y;
					tmplparam[lno][6][2] = curl->orgDirection.z;

					break;
				case D3DLIGHT_POINT:
					tmplparam[lno][0][1] = 1.0f;

					tmplparam[lno][0][2] = curl->divRange;


					tmplparam[lno][5][0] = curl->orgPosition.x;
					tmplparam[lno][5][1] = curl->orgPosition.y;
					tmplparam[lno][5][2] = curl->orgPosition.z;

					break;
				case D3DLIGHT_SPOT:
					tmplparam[lno][0][1] = 2.0f;

					tmplparam[lno][0][2] = curl->divRange;
					tmplparam[lno][1][0] = curl->cosPhi;
					tmplparam[lno][1][1] = curl->divcosPhi;

					tmplparam[lno][5][0] = curl->orgPosition.x;
					tmplparam[lno][5][1] = curl->orgPosition.y;
					tmplparam[lno][5][2] = curl->orgPosition.z;

					tmplparam[lno][6][0] = curl->orgDirection.x;
					tmplparam[lno][6][1] = curl->orgDirection.y;
					tmplparam[lno][6][2] = curl->orgDirection.z;

					break;
				}
					
				curl = curl->nextlight;
				setlno++;

			}else{
				tmplparam[lno][0][0] = 0.0f;
				tmplparam[lno][0][1] = -1.0f;
			}
		}

		float lbump[7][4];
		ZeroMemory( lbump, sizeof( float ) * 7 * 4 );
		CLightData* lbptr;
		lbptr = GetLightData( g_LightOfBump );
		if( !lbptr && g_lighthead ){
			lbptr = g_lighthead;//存在しなかったら先頭ライト
		}

		if( lbptr ){
			lbump[0][0] = 1.0f;

			lbump[2][0] = lbptr->Diffuse.x;
			lbump[2][1] = lbptr->Diffuse.y;
			lbump[2][2] = lbptr->Diffuse.z;

			lbump[3][0] = lbptr->Specular.x;
			lbump[3][1] = lbptr->Specular.y;
			lbump[3][2] = lbptr->Specular.z;

			lbump[4][0] = lbptr->Ambient.x;
			lbump[4][1] = lbptr->Ambient.y;
			lbump[4][2] = lbptr->Ambient.z;

			switch( lbptr->Type ){
			case D3DLIGHT_DIRECTIONAL:
				lbump[0][1] = 0.0f;

				lbump[6][0] = lbptr->orgDirection.x;
				lbump[6][1] = lbptr->orgDirection.y;
				lbump[6][2] = lbptr->orgDirection.z;

				break;
			case D3DLIGHT_POINT:
				lbump[0][1] = 1.0f;

				lbump[0][2] = lbptr->divRange;


				lbump[5][0] = lbptr->orgPosition.x;
				lbump[5][1] = lbptr->orgPosition.y;
				lbump[5][2] = lbptr->orgPosition.z;

				break;
			case D3DLIGHT_SPOT:
				lbump[0][1] = 2.0f;

				lbump[0][2] = lbptr->divRange;
				lbump[1][0] = lbptr->cosPhi;
				lbump[1][1] = lbptr->divcosPhi;

				lbump[5][0] = lbptr->orgPosition.x;
				lbump[5][1] = lbptr->orgPosition.y;
				lbump[5][2] = lbptr->orgPosition.z;

				lbump[6][0] = lbptr->orgDirection.x;
				lbump[6][1] = lbptr->orgDirection.y;
				lbump[6][2] = lbptr->orgDirection.z;

				break;
			}
		}else{
			//dummy data
			lbump[0][0] = 1.0f;

			lbump[2][0] = 0.0f;
			lbump[2][1] = 0.0f;
			lbump[2][2] = 0.0f;

			lbump[3][0] = 0.0f;
			lbump[3][1] = 0.0f;
			lbump[3][2] = 0.0f;

			lbump[4][0] = 0.0f;
			lbump[4][1] = 0.0f;
			lbump[4][2] = 0.0f;
			///////directional
			lbump[0][1] = 0.0f;

			lbump[6][0] = 0.0f;
			lbump[6][1] = 0.0f;
			lbump[6][2] = 1.0f;

		}

		if( (g_chkVS >= 20) || (g_chkVS == 0) ){
			hres = g_pEffect->SetValue( g_hmLightParams, &(tmplparam[0][0][0]), sizeof( float ) * 3 * 7 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLightParam SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->SetValue( g_hmBumpLight, &(lbump[0][0]), sizeof( float ) * 7 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mBumpLight SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}else{
			hres = g_pEffect->SetValue( g_hmLightParams, &(tmplparam[0][0][0]), sizeof( float ) * 1 * 7 * 4 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLightParam SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		g_lightnum = setlno;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	}else{
		//shadowmap
		if( (g_chkVS >= 20) || (g_chkVS == 0) ){
			D3DXMATRIX matLP;
			matLP = s_matViewShadow * s_matProjShadow;
			hres = g_pEffect->SetMatrix( g_hmLP, &matLP );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLP SetMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			D3DXMATRIX matLPB;
			matLPB = matLP * s_matBiasShadow;
			hres = g_pEffect->SetMatrix( g_hmLPB, &matLPB );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mLPB SetMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			hres = g_pEffect->SetValue( g_hmShadowCoef, &(s_shadowcoef[0]), sizeof( float ) * 2 );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : mShadowCoef SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			hres = g_pEffect->SetTexture( g_hShadowMap, s_ShadowMapTex );
			if( hres != D3D_OK ){
				DbgOut( "SetShaderConst : ShadowMap SetTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
***/

int GetShaderHandle()
{
	if( !g_pEffect ){
		_ASSERT( 0 );
		return 1;
	}

	g_hRenderSceneBone4 = g_pEffect->GetTechniqueByName( "RenderSceneBone4" );
	g_hRenderSceneBone1 = g_pEffect->GetTechniqueByName( "RenderSceneBone1" );
	g_hRenderSceneBone0 = g_pEffect->GetTechniqueByName( "RenderSceneBone0" );
	g_hRenderSceneBone4Toon = g_pEffect->GetTechniqueByName( "RenderSceneBone4Toon" );
	g_hRenderSceneBone1Toon = g_pEffect->GetTechniqueByName( "RenderSceneBone1Toon" );
	g_hRenderSceneBone0Toon = g_pEffect->GetTechniqueByName( "RenderSceneBone0Toon" );
	g_hRenderSceneBone4ToonNoTex = g_pEffect->GetTechniqueByName( "RenderSceneBone4ToonNoTex" );
	g_hRenderSceneBone1ToonNoTex = g_pEffect->GetTechniqueByName( "RenderSceneBone1ToonNoTex" );
	g_hRenderSceneBone0ToonNoTex = g_pEffect->GetTechniqueByName( "RenderSceneBone0ToonNoTex" );
	g_hRenderSceneBone4Toon1 = g_pEffect->GetTechniqueByName( "RenderSceneBone4Toon1" );
	g_hRenderSceneBone1Toon1 = g_pEffect->GetTechniqueByName( "RenderSceneBone1Toon1" );
	g_hRenderSceneBone0Toon1 = g_pEffect->GetTechniqueByName( "RenderSceneBone0Toon1" );
	g_hRenderSceneBone4Edge0 = g_pEffect->GetTechniqueByName( "RenderSceneBone4Edge0" );
	g_hRenderSceneBone1Edge0 = g_pEffect->GetTechniqueByName( "RenderSceneBone1Edge0" );
	g_hRenderSceneBone0Edge0 = g_pEffect->GetTechniqueByName( "RenderSceneBone0Edge0" );
	g_hRenderSceneBone4Bump = g_pEffect->GetTechniqueByName( "RenderSceneBone4Bump" );//vs2
	g_hRenderSceneBone1Bump = g_pEffect->GetTechniqueByName( "RenderSceneBone1Bump" );//vs2
	g_hRenderSceneBone0Bump = g_pEffect->GetTechniqueByName( "RenderSceneBone0Bump" );//vs2
	g_hRenderSceneNormalShadow = g_pEffect->GetTechniqueByName( "RenderSceneNormalShadow" );//vs2
	g_hRenderSceneBumpShadow = g_pEffect->GetTechniqueByName( "RenderSceneBumpShadow" );//vs2
	g_hRenderSceneToon1Shadow = g_pEffect->GetTechniqueByName( "RenderSceneToon1Shadow" );//vs2
	g_hRenderSceneToon0Shadow = g_pEffect->GetTechniqueByName( "RenderSceneToon0Shadow" );//vs2
	g_hRenderSceneBone4PP = g_pEffect->GetTechniqueByName( "RenderSceneBone4PP" );
	g_hRenderSceneBone0PP = g_pEffect->GetTechniqueByName( "RenderSceneBone0PP" );
	g_hRenderScenePPShadow = g_pEffect->GetTechniqueByName( "RenderScenePPShadow" );

	g_hRenderZN_2path_NormalBone = g_pEffect->GetTechniqueByName( "RenderZN_2path_NormalBone" );
	g_hRenderZN_2path_NormalNoBone = g_pEffect->GetTechniqueByName( "RenderZN_2path_NormalNoBone" );


	g_hmWorldMatrixArray = g_pEffect->GetParameterByName( NULL, "mWorldMatrixArray" );
	g_hmView = g_pEffect->GetParameterByName( NULL, "mView" );
	g_hmProj = g_pEffect->GetParameterByName( NULL, "mProj" );
	g_hmEyePos = g_pEffect->GetParameterByName( NULL, "mEyePos" );
	g_hmLightParams = g_pEffect->GetParameterByName( NULL, "mLightParams" );
	g_hmFogParams = g_pEffect->GetParameterByName( NULL, "mFogParams" );
	g_hmToonParams = g_pEffect->GetParameterByName( NULL, "mToonParams" );
	g_hmLightNum = g_pEffect->GetParameterByName( NULL, "mLightNum" );
	g_hmEdgeCol0 = g_pEffect->GetParameterByName( NULL, "mEdgeCol0" );
	g_hmAlpha = g_pEffect->GetParameterByName( NULL, "mAlpha" );
	g_hmBumpLight = g_pEffect->GetParameterByName( NULL, "mBumpLight" );//vs2
	g_hNormalMap = g_pEffect->GetParameterByName( NULL, "NormalMap" );//vs2
	g_hDecaleTex = g_pEffect->GetParameterByName( NULL, "DecaleTex" );//vs2
	g_hDecaleTex1 = g_pEffect->GetParameterByName( NULL, "DecaleTex1" );//vs2
	g_hmLP = g_pEffect->GetParameterByName( NULL, "mLP" );//vs2
	g_hmLPB = g_pEffect->GetParameterByName( NULL, "mLPB" );//vs2
	g_hShadowMap = g_pEffect->GetParameterByName( NULL, "ShadowMap" );//vs2
	g_hmShadowCoef = g_pEffect->GetParameterByName( NULL, "mShadowCoef" );//vs2
	g_hmoldView = g_pEffect->GetParameterByName( NULL, "moldView" );//vs2
	g_hmMinAlpha = g_pEffect->GetParameterByName( NULL, "mMinAlpha" );//vs2
	g_hmGlowMult = g_pEffect->GetParameterByName( NULL, "mGlowMult" );
	g_hmToonDiffuse = g_pEffect->GetParameterByName( NULL, "mToonDiffuse" );
	g_hmToonAmbient = g_pEffect->GetParameterByName( NULL, "mToonAmbient" );
	g_hmToonSpecular = g_pEffect->GetParameterByName( NULL, "mToonSpecular" );
	g_hmZNUser1 = g_pEffect->GetParameterByName( NULL, "mZNUser1" );
	g_hmZNUser2 = g_pEffect->GetParameterByName( NULL, "mZNUser2" );

/////////////// PostEffect
	if( g_pPostEffect ){
		g_hPostEffectBlur0 = g_pPostEffect->GetTechniqueByName( "PostEffectBlur0" );
		g_hPostEffectGauss = g_pPostEffect->GetTechniqueByName( "PostEffectBlurGauss" );
		g_hPostEffectTLV = g_pPostEffect->GetTechniqueByName( "PostEffectTLV" );
		g_hPostEffectDownScale = g_pPostEffect->GetTechniqueByName( "PostEffectDownScale" );
		g_hPostEffectToneMap = g_pPostEffect->GetTechniqueByName( "PostEffectToneMap" );
		g_hPostEffectBrightPass = g_pPostEffect->GetTechniqueByName( "PostEffectBrightPass" );
		g_hPostEffectHDRFinal = g_pPostEffect->GetTechniqueByName( "PostEffectHDRFinal" );
		g_hPostEffectMono = g_pPostEffect->GetTechniqueByName( "PostEffectMono" );
		g_hPostEffectCbCr = g_pPostEffect->GetTechniqueByName( "PostEffectCbCr" );

		char username[256];
		int userno;
		for( userno = 0; userno < 20; userno++ ){
			sprintf_s( username, 256, "PostEffectUser%d", userno );
			g_hPostEffectUser[userno] = g_pPostEffect->GetTechniqueByName( username );
			_ASSERT( g_hPostEffectUser[userno] );
		}

		g_hpeWidth0 = g_pPostEffect->GetParameterByName( NULL, "WIDTH0" );
		g_hpeHeight0 = g_pPostEffect->GetParameterByName( NULL, "HEIGHT0" );
		g_hpeWidth1 = g_pPostEffect->GetParameterByName( NULL, "WIDTH1" );
		g_hpeHeight1 = g_pPostEffect->GetParameterByName( NULL, "HEIGHT1" );
		g_hpeTexture0 = g_pPostEffect->GetParameterByName( NULL, "Texture0" );
		g_hpeTexture1 = g_pPostEffect->GetParameterByName( NULL, "Texture1" );
		g_hpeTexture2 = g_pPostEffect->GetParameterByName( NULL, "Texture2" );
		g_hpeWeight = g_pPostEffect->GetParameterByName( NULL, "weight" );
		g_hpeOffsetX = g_pPostEffect->GetParameterByName( NULL, "offsetX" );
		g_hpeOffsetY = g_pPostEffect->GetParameterByName( NULL, "offsetY" );
		g_hpeSampleWeight = g_pPostEffect->GetParameterByName( NULL, "avSampleWeights" );
		g_hpeSampleOffset = g_pPostEffect->GetParameterByName( NULL, "avSampleOffsets" );
		g_hpeMiddleGray = g_pPostEffect->GetParameterByName( NULL, "fMiddleGray" );
		g_hpeBrightPassThreshold = g_pPostEffect->GetParameterByName( NULL, "fBrightPassThreshold" );
		g_hpeBrightPassOffset = g_pPostEffect->GetParameterByName( NULL, "fBrightPassOffset" );
		g_hpeColMult = g_pPostEffect->GetParameterByName( NULL, "colmult" );
		g_hpeCbCr = g_pPostEffect->GetParameterByName( NULL, "CbCr" );
		g_hpeUserFl4[0] = g_pPostEffect->GetParameterByName( NULL, "userFL4_0" );
		g_hpeUserFl4[1] = g_pPostEffect->GetParameterByName( NULL, "userFL4_1" );
		g_hpeUserFl4[2] = g_pPostEffect->GetParameterByName( NULL, "userFL4_2" );
		g_hpeUserFl4[3] = g_pPostEffect->GetParameterByName( NULL, "userFL4_3" );
		g_hpeUserFl4[4] = g_pPostEffect->GetParameterByName( NULL, "userFL4_4" );
		g_hpeUserFl4[5] = g_pPostEffect->GetParameterByName( NULL, "userFL4_5" );
		g_hpeUserFl4[6] = g_pPostEffect->GetParameterByName( NULL, "userFL4_6" );
		g_hpeUserFl4[7] = g_pPostEffect->GetParameterByName( NULL, "userFL4_7" );
		g_hpeUserFl4[8] = g_pPostEffect->GetParameterByName( NULL, "userFL4_8" );
		g_hpeUserFl4[9] = g_pPostEffect->GetParameterByName( NULL, "userFL4_9" );
		g_hpeUserTex[0] = g_pPostEffect->GetParameterByName( NULL, "usertex0" );
		g_hpeUserTex[1] = g_pPostEffect->GetParameterByName( NULL, "usertex1" );
		g_hpeUserTex[2] = g_pPostEffect->GetParameterByName( NULL, "usertex2" );
		g_hpeUserTex[3] = g_pPostEffect->GetParameterByName( NULL, "usertex3" );
		g_hpeUserTex[4] = g_pPostEffect->GetParameterByName( NULL, "usertex4" );
	}

	return 0;
}

int CreateSwapChainElem( int scid, SWAPCHAINELEM* cursc, HWND srchwnd, int texid, D3DCOLOR* clearcol, int needz )
{
	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "CreateSwapChainElem : GetD3DDevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//HWND hwnd = 0;

	if( texid <= 0 ){
		cursc->hwnd = srchwnd;

		RECT rc;
		GetClientRect( srchwnd, &rc );

		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp = g_pD3DApp->m_d3dpp;
		d3dpp.BackBufferWidth = rc.right - rc.left;
		d3dpp.BackBufferHeight = rc.bottom - rc.top;
		d3dpp.hDeviceWindow = (HWND)srchwnd;
		d3dpp.EnableAutoDepthStencil = FALSE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.PresentationInterval = g_presentinterval;
		d3dpp.BackBufferCount = 1;//!!!!

		HRESULT hres;
		hres = pdev->CreateAdditionalSwapChain( &d3dpp, &(cursc->pSwapChain) );
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : dev CreateAdditionalSwapChain error %x !!!\n", hres );
			_ASSERT( 0 );
			return 1;
		}


		hres = cursc->pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &(cursc->pBackBuffer) );
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : swapchain GetBackBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( needz == 1 ){
			D3DFORMAT fmt;
			fmt = D3DFMT_D16;
	//		fmt = D3DFMT_D24S8;
	//		fmt = D3DFMT_D32;
			//hres = pdev->CreateDepthStencilSurface( d3dpp.BackBufferWidth, d3dpp.BackBufferHeight, fmt, d3dpp.MultiSampleType, &(cursc->pZSurf), NULL );
			hres = pdev->CreateDepthStencilSurface( d3dpp.BackBufferWidth, d3dpp.BackBufferHeight, fmt, 
				d3dpp.MultiSampleType,
				//max( 0, d3dpp.MultiSampleQuality - 1 ), 
				d3dpp.MultiSampleQuality,
				FALSE, &(cursc->pZSurf), NULL ); 
			if( hres != D3D_OK ){
				DbgOut( "CreateSwapChainElem : dev CreateDepthStencilSurface error %x !!!\n", hres );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			if( cursc->pZSurf ){
				cursc->pZSurf->Release();
				cursc->pZSurf = 0;
			}
		}

		cursc->texid = -1;
		cursc->texsize.x = 0;
		cursc->texsize.y = 0;

		if( clearcol ){
			cursc->clearcol = *clearcol;
		}else{
			cursc->clearcol = D3DCOLOR_ARGB( 0, 0, 0, 255 );
		}


	}else{

		cursc->hwnd = 0;//!!!!!!!!!!!

		if( !g_texbnk ){
			DbgOut( "CreateSwapChainElem : texbank NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		LPDIRECT3DTEXTURE9 curtex = 0;
		ret = g_texbnk->GetTexDataBySerial( texid, &curtex );
		if( ret || !curtex ){
			DbgOut( "CreateSwapChainElem : texbank GetTexDataBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = curtex->GetSurfaceLevel( 0, &(cursc->pBackBuffer) );
		if( hres != D3D_OK ){
			DbgOut( "CreateSwapChainElem : tex GetSrufaceLevel error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int width, height, pool, transparent, format;
		ret = g_texbnk->GetTextureInfoBySerial( texid, &width, &height, &pool, &transparent, &format );
		if( ret ){
			DbgOut( "CreateSwapChainElem : texbank GetTextureSizeBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( needz == 1 ){
			D3DFORMAT fmt;
			fmt = D3DFMT_D16;
	//		fmt = D3DFMT_D24S8;
	//		fmt = D3DFMT_D32;
			hres = pdev->CreateDepthStencilSurface( width, height, fmt, 
				D3DMULTISAMPLE_NONE,
				0,
				FALSE, &(cursc->pZSurf), NULL ); 
			if( hres != D3D_OK ){
				DbgOut( "CreateSwapChainElem : dev CreateDepthStencilSurface error %x !!!\n", hres );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			if( cursc->pZSurf ){
				cursc->pZSurf->Release();
				cursc->pZSurf = 0;
			}
		}
		cursc->texid = texid;
		cursc->texsize.x = width;
		cursc->texsize.y = height;

		if( clearcol ){
			cursc->clearcol = *clearcol;
		}else{
			cursc->clearcol = D3DCOLOR_ARGB( 0, 0, 0, 0 );
		}

	}
	
	//cursc->scid = s_swapchaincnt;
	cursc->scid = scid;
	cursc->needz = needz;

	return 0;
}
SWAPCHAINELEM* GetFreeSwapChainElem()
{
	SWAPCHAINELEM* retelem = 0;

	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid == -1 ){
			retelem = &(s_sctable[scno]);
			break;
		}
	}
	return retelem;
}


SWAPCHAINELEM* GetSwapChainElem( int scid )
{


	SWAPCHAINELEM* retelem = 0;
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid == scid ){
			retelem = &(s_sctable[scno]);
			break;
		}

	}

	if( retelem ){
		SetMatProj( retelem );//!!!!!!!!!!!!!!
	}


	return retelem;
}

int DestroySwapChainElem( SWAPCHAINELEM* cursc, int initflag )
{

	if( initflag ){
		cursc->scid = -1;
		cursc->hwnd = 0;
		cursc->texid = -1;
		cursc->texsize.x = 0;
		cursc->texsize.y = 0;
	}

	if( cursc->pZSurf ){
		cursc->pZSurf->Release();
		cursc->pZSurf = 0;
	}

	if( cursc->pBackBuffer ){
		cursc->pBackBuffer->Release();
		cursc->pBackBuffer = 0;
	}

	if( cursc->pSwapChain ){
		cursc->pSwapChain->Release();
		cursc->pSwapChain = 0;
	}

	return 0;

}

int Restore( CHandlerSet* hsptr, int billboardid, CPanda* panda, int pndid )
{

	EnterCriticalSection( &g_crit_restore );//######## start crit

//	SetResDir( hsptr->m_resdir, hsptr );//!!!!!!!!!!!!!!!

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

	ret = hsptr->Restore( pdev, s_hwnd, billboardid, 1, panda, pndid );
	if( ret ){
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 1;
	}


	if( g_cop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_aop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}

	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_COLORARG1 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_COLORARG2 : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "e3d : Restore : SetTextureStageState : D3DTSS_ADDRESSU : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
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

CHandlerSet* GetHandlerSet( int hsid )
{

	EnterCriticalSection( &g_crit_hshead );//######## start crit

	if( (hsid < 0) || (hsid >= HSIDMAX) ){
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 0;
	}

	if( s_hsidelem[hsid].validflag ){
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return s_hsidelem[hsid].hsptr;
	}else{
		LeaveCriticalSection( &g_crit_hshead );//###### end crit
		return 0;
	}

}
BGDISPELEM* GetFreeBGDispElem()
{
	BGDISPELEM* retelem = 0;
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_bgdisp[scno].scid == -1 ){
			retelem = &(s_bgdisp[scno]);
			break;
		}
	}
	return retelem;
}
BGDISPELEM* GetBGDispElem( int scid )
{
	BGDISPELEM* retelem = 0;
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_bgdisp[scno].scid == scid ){
			retelem = &(s_bgdisp[scno]);
			break;
		}
	}
	return retelem;

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

//////////
	D3DXMATRIX offbef, offrot, offaft;
	D3DXMatrixIdentity( &offbef );
	D3DXMatrixIdentity( &offrot );
	D3DXMatrixIdentity( &offaft );
	offbef._41 = s_camoffbef.x;
	offbef._42 = s_camoffbef.y;
	offbef._43 = s_camoffbef.z;

	offaft._41 = s_camoffaft.x;
	offaft._42 = s_camoffaft.y;
	offaft._43 = s_camoffaft.z;

	offrot = s_camoffq.MakeRotMatX();

	D3DXMATRIX offmat, offinv;
	offmat = offbef * offrot * offaft;

//	D3DXMatrixInverse( &offinv, NULL, &offmat );
//	s_matView = offinv * s_matView;

	s_matView = offmat * s_matView;

/////////
	D3DXMATRIX vm;
	vm = s_matView;
	vm._41 = 0.0f;
	vm._42 = 0.0f;
	vm._43 = 0.0f;

	D3DXQUATERNION vmq;
	D3DXQuaternionRotationMatrix( &vmq, &vm );
	s_cameraq.x = vmq.x;
	s_cameraq.y = vmq.y;
	s_cameraq.z = vmq.z;
	s_cameraq.w = vmq.w;


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
		//dstvec->x *= magdiv;
		//dstvec->y *= magdiv;
		//dstvec->z *= magdiv;
		dstvec->x = srcvec->x * magdiv;
		dstvec->y = srcvec->y * magdiv;
		dstvec->z = srcvec->z * magdiv;
	}else{
//		DbgOut( "q2 : DXVec3Normalize : zero warning !!!\n" );
		dstvec->x = 0.0f;
		dstvec->y = 0.0f;
		dstvec->z = 0.0f;
//		_ASSERT( 0 );
	}

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
int DisableFogState()
{

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	s_savefogenable = g_fogenable;

	if( g_fogenable != 0 ){
		g_renderstate[D3DRS_FOGENABLE] = 0;
		pdev->SetRenderState( D3DRS_FOGENABLE, 0 );
	}
	return 0;
}
int RollbackFogState()
{
	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	if( s_savefogenable != g_fogenable ){
		g_fogenable = s_savefogenable;

		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pdev->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

	}
	return 0;
}

int DestroyHandlerSet( int hsid )
{

///// textureの破棄

//	EnterCriticalSection( &g_crit_restore );//######## start crit
	
	int ret;

	if( (hsid < 0) || (hsid >= HSIDMAX) ){
		DbgOut( "DestroyHandlerSet : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( s_hsidelem[hsid].validflag == 0 ){
		return 0;
	}


	ret = g_texbnk->ResetDirtyFlag();

	ret = DestroyTexture( hsid, -2, -2, 1, -2 );
	if( ret ){
		DbgOut( "DestroyHandlerSet : DestroyTexture error !!!\n" );
		_ASSERT( 0 );
		//return 1;
	}

//	LeaveCriticalSection( &g_crit_restore );//###### end crit


/////////////
	EnterCriticalSection( &g_crit_hshead );//######## start crit

	
//	if( (hsid < 0) || (hsid >= HSIDMAX) ){
//		DbgOut( "DestroyHandlerSet : hsid error !!!\n" );
//		_ASSERT( 0 );
//		return 1;
//	}

	s_hsidelem[hsid].validflag = 0;
	if( s_hsidelem[hsid].hsptr ){
		delete s_hsidelem[hsid].hsptr;
		s_hsidelem[hsid].hsptr = 0;
	}

	LeaveCriticalSection( &g_crit_hshead );//###### end crit


	return 0;
}

int DestroyTexture( int hsid, int bbid, int spriteid, int dirtyreset, int bgno )
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//bbid == -1 ＢＢ全て削除、bbid == -2 ＢＢは削除しない
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	int ret;

	CHandlerSet* delhs = 0;
	if( hsid > 0 ){
		delhs = GetHandlerSet( hsid );
		_ASSERT( delhs );
	}else{
		delhs = 0;
	}

	CMySprite* delsprite = 0;
	if( spriteid >= 0 ){
		delsprite = GetSprite( spriteid );
		_ASSERT( delsprite );
	}else{
		delsprite = 0;
	}

	BGDISPELEM* delbgelem = 0;
	if( bgno > 0 ){
		delbgelem = GetBGDispElem( bgno );
	}else{
		delbgelem = 0;
	}

//////////

EnterCriticalSection( &g_crit_restore );//######## start crit

	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		BGDISPELEM* curbgelem;
		curbgelem = &(s_bgdisp[scno]);
		if( (curbgelem->scid >= 0) && (curbgelem->bgdisp) && (curbgelem != delbgelem) ){
			ret = curbgelem->bgdisp->SetTextureDirtyFlag();
			_ASSERT( !ret );
		}
	}


	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			if( delhs != s_hsidelem[hsidno].hsptr ){
				ret = s_hsidelem[hsidno].hsptr->SetTextureDirtyFlag( -999999 );//全てにdirtyflagセット　
				_ASSERT( !ret );
			}
		}
	}


	if( g_bbhs ){
		if( bbid == -2 ){
			ret = g_bbhs->SetTextureDirtyFlag( -999999 );//全てにdirtyflagセット
			_ASSERT( !ret );
		}else if( bbid == -1 ){
			//何もしない
		}else if( bbid >= 0 ){
			//ret = g_bbhs->SetTextureDirtyFlag( bbid );       //----> bug ---> //bbidにだけdirtyflagセット
															

			ret = g_bbhs->SetTextureDirtyFlag( -bbid );// bbid以外に、dirtyflagセット


			_ASSERT( !ret );
		}
	}


//DbgOut( "!!! DestroyTexture : spriteid %d\r\n", spriteid );


	CMySprite* cursprite = s_spritehead;
	while( cursprite ){
		if( cursprite != delsprite ){
			ret = cursprite->SetTextureDirtyFlag();

//DbgOut( "!!! DestroyTexture : SetTextureDirtyFlag %d\r\n", cursprite->serialno );			
		}
		cursprite = cursprite->next;
	}

	//////////


	ret = g_texbnk->DestroyNotDirtyTexture( 1 );
	_ASSERT( !ret );


	if( dirtyreset ){
		if( g_texbnk ){
			g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
		}
	}

LeaveCriticalSection( &g_crit_restore );//###### end crit


	return 0;
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

int GetScreenPos( D3DXVECTOR3 vec3d, D3DXVECTOR3* scposptr )
{

	DWORD dwClipWidth = g_scsize.x / 2;
	DWORD dwClipHeight = g_scsize.y / 2;

	CBSphere bs;
	bs.tracenter = vec3d;

	float aspect;
	aspect = (float)dwClipWidth / (float)dwClipHeight;
	int ret;
	ret = bs.Transform2ScreenPos( dwClipWidth, dwClipHeight, s_matView, g_matProj, aspect, scposptr );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPos : bs Transform2ScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int ScreenToObj( D3DXVECTOR3* objpos, D3DXVECTOR3 scpos )
{

	float clipw, cliph, asp;

	clipw = (float)g_scsize.x / 2.0f;
	cliph = (float)g_scsize.y / 2.0f;

	/***
	clipw = (float)s_clientWidth / 2.0f;
	cliph = (float)s_clientHeight / 2.0f;
	//Resotreをまだ実行していない可能性があるので、s_clientWidthは、使えない。
	***/
  
	//asp = 1.0f;//!!!!!!!!!!!!
	asp = clipw / cliph;


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

	mat = s_matView * g_matProj;//world == ini

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
int LookAtQ( CQuaternion2* dstqptr, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int divnum, int upflag )
{
	int ret;
	
	D3DXVECTOR3 newtarget;
	int newdivnum;

	float savetwist = dstqptr->twist;

	int saultflag = 0;

	if( ((tarvec.x == 0.0f) || (tarvec.x == -0.0f)) && 
		((tarvec.y == 0.0f) || (tarvec.y == -0.0f)) &&
		((tarvec.z == 0.0f) || (tarvec.z == -0.0f)) ){

		//zero vecがターゲットのときは、すぐリターン
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}


	CQuaternion2 saveq0;
	saveq0 = *dstqptr;


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

	//dstqptr->twist = savetwist;


	dstqptr->normalize();
//!!!!! validate !!!!!!!!!!!!!
	if( (dstqptr->x >= -1.0f) && (dstqptr->x <= 1.0f) &&
		(dstqptr->y >= -1.0f) && (dstqptr->y <= 1.0f) &&
		(dstqptr->z >= -1.0f) && (dstqptr->z <= 1.0f) &&
		(dstqptr->w >= -1.0f) && (dstqptr->w <= 1.0f) ){

		// 数値が入っている
	}else{
		//数値以外のものが入っている可能性大

		*dstqptr = saveq0;

	}

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
int SetMatProj( SWAPCHAINELEM* cursc )
{
	RECT clirect;

	if( cursc->texid <= 0 ){
		GetClientRect( cursc->hwnd, &clirect );
		if( (clirect.bottom <= 0) || (clirect.right <= 0) ){
			//_ASSERT( 0 );
			return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 変更しない
		}

	}else{
		clirect.top = 0;
		clirect.left = 0;
		clirect.bottom = cursc->texsize.y;
		clirect.right = cursc->texsize.x;
	}

    D3DXMatrixIdentity( &g_matProj );

	if( g_ortho == 0 ){
		DWORD dwClipWidth = (clirect.right - clirect.left) / 2;
		DWORD dwClipHeight = (clirect.bottom - clirect.top) / 2;
		float fov = (float)g_proj_fov * (float)DEG2PAI;
		float aspect;
		aspect = (float)dwClipWidth / (float)dwClipHeight;
		D3DXMatrixPerspectiveFovLH( &g_matProj, fov, aspect, g_proj_near, g_proj_far );
	}else{
		float vw, vh;
		vw = g_orthosize;
		vh = g_orthosize * ((float)(clirect.bottom - clirect.top) / (float)(clirect.right - clirect.left));
		D3DXMatrixOrthoLH( &g_matProj, vw, vh, g_proj_near, g_proj_far );		
	}


	g_scsize.x = clirect.right - clirect.left;
	g_scsize.y = clirect.bottom - clirect.top;

	return 0;
}




////////////////////////
/// ** plugin func

EASY3D_API int E3DInit( HINSTANCE srcinst, HWND srchwnd, int fullscflag, int bits, int multisamplenum, int gpuflag, int enablereverb, int initcomflag, int index32bit, int* scidptr )
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc( 13981 );

	
	s_callinitflag = 1;

	s_initcomflag = initcomflag;//!!!!!!!!!!!!!!!

	//DbgOut( "e3dinit : WM_ACTIVATEAPP %d, WM_SYSCOMMAND %d\r\n", WM_ACTIVATEAPP, WM_SYSCOMMAND );

	DbgOut( "E3DInit : Easy3D Version %d\r\n", E3DVERSIONNO );

	s_pGraphBuilder = 0;
	s_pMediaControl = 0;
	s_pMediaEventEx = 0;
	s_pVideoWindow = 0;
	s_pBasicAudio = 0;
	s_playingmovieflag = 0;



	m_freq.QuadPart = 0;
	m_rendercnt.QuadPart = 0;
	m_curcnt.QuadPart = 0;
	m_dfreq = 0.0;
	m_drenderfreq = 0.0;
	m_drendercnt = 0.0;
	m_dcurcnt = 0.0;

	QueryPerformanceFrequency( &m_freq );
	m_dfreq = (double)m_freq.QuadPart;

	LARGE_INTEGER tmpcnt;
	QueryPerformanceCounter( &tmpcnt );
	if( tmpcnt.QuadPart == 0 ){
		::MessageBox( s_hwnd, "マシンが高精度タイマーに対応していないため\nこのソフトは実行できません。", "終了します", MB_OK );
		return 1;
	}

	s_camoffbef = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	s_camoffaft = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	s_camoffq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f );


	g_index32bit = index32bit;

	g_proj_near = 100.0f;
	g_proj_far = 10000.0f;
	g_proj_fov = 45.0f;
	g_ortho = 0;
	g_orthosize = 3000.0f;

	g_fogenable = 0;
	g_fogcolor = 0x00FFFFFF;
	g_fogstart = 0.0f;
	g_fogend = 10000.0f;	
	g_fogtype = 0.0f;
	s_savefogenable = g_fogenable;

	if( m_cah ){
		delete m_cah;
		m_cah = 0;
	}

	m_cah = new CCameraAnimHandler();
	if( !m_cah ){
		DbgOut( "E3DInit : cah alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int thno;
	for( thno = 0; thno < THMAX; thno++ ){
		InitThTableElem( thno );
	}
	//s_threadnum = 0;

	int tmno;
	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;
		
		curtm->tonemapid = -1;
		curtm->tonenum = 0;
		curtm->pscid = 0;
		curtm->ptexid = 0;
	}
	s_usertonemapcnt = 0;



	InitializeCriticalSection( &g_crit_hshead );
	//InitializeCriticalSection( &g_crit_tempmp );
	InitializeCriticalSection( &g_crit_restore );

	ZeroMemory( s_keyboardcnt, sizeof( int ) * 256 );

	HRESULT hr;


	*scidptr = 0;//!!!!!!!

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

	s_inst = srcinst;
	s_hwnd = srchwnd;

	if( IsWindow( s_hwnd ) == 0 ){
		DbgOut( "e3dinit : hwnd not window error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//InitCommonControls(); 	// for progress bar
	INITCOMMONCONTROLSEX cominfo;
	cominfo.dwSize = sizeof( INITCOMMONCONTROLSEX );
	cominfo.dwICC = ICC_PROGRESS_CLASS;

	InitCommonControlsEx( &cominfo ); 	// for progress bar

	s_PBwnd = 0;

    if( FAILED( g_pD3DApp->Create( s_inst, s_hwnd, fullscflag, bits, s_multisample, gpuflag ) ) ){
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

	int ret;
	//HRESULT hr;


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		_ASSERT( 0 );
		return 1;
	}


//effect



	char moduledir[2048];
	char fxname[2048];
	ZeroMemory( moduledir, 2048 );
	ZeroMemory( fxname, 2048 );
	int mleng = GetEnvironmentVariable( (LPCTSTR)"MODULEDIR", (LPTSTR)moduledir, 2048 );
	if( (mleng <= 0) || (mleng >= 2048) ){
		DbgOut( "e3dhsp : E3DInit : GetEnvironmentVariable error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	char* endptr = 0;
	int ch = '\\';
	char* lasten = 0;
	lasten = strrchr( moduledir, ch );
	if( !lasten ){
		_ASSERT( 0 );
		return 1;
	}
	*lasten = 0;
	char* last2en = 0;
	char* last3en = 0;
	last2en = strrchr( moduledir, ch );
	if( last2en ){
		*last2en = 0;
		last3en = strrchr( moduledir, ch );
		if( last3en ){
			if( (strcmp( last2en + 1, "debug" ) == 0) ||
				(strcmp( last2en + 1, "Debug" ) == 0) ||
				(strcmp( last2en + 1, "DEBUG" ) == 0) ||
				(strcmp( last2en + 1, "release" ) == 0) ||
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

	int cpleng;
	cpleng = (int)( endptr - moduledir + 1 );
	strncpy_s( fxname, 2048, moduledir, cpleng );
	strcat_s( fxname, 2048, "E3D_EXT\\posteffect.fx" );


	s_hMod = GetModuleHandle( "easy3d.dll" );//!!!!!!!!!! LoadMOAFileでも使用する。
	DbgOut( "\r\nE3DInit : ModuleName : e3dhsp3.dll\r\n\r\n" );
	_ASSERT( s_hMod );

	if( g_useGPU == 1 ){
		HRESULT hr;


		DWORD dwShaderFlags = 0;
		#ifdef DEBUG_VS
			dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
		#endif
		#ifdef DEBUG_PS
			dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
		#endif

        //dwShaderFlags |= D3DXSHADER_NO_PRESHADER;

		//if( !g_bEnablePreshader )
		//	dwShaderFlags |= D3DXSHADER_NO_PRESHADER;

		
		if( s_hMod == NULL ){
			::MessageBox( NULL, "モジュール名が違います。終了します。", "エラー", MB_OK );
			DbgOut( "E3DInit : GetModuleHandle error !!!\n" );
			_ASSERT( 0 );
			g_useGPU = 0;
			return 1;
		}else{

			if( (g_chkVS >= 20) || (g_chkVS == 0) ){
//DbgOut( "E3DInit : 000\r\n" );

				LPD3DXBUFFER pxbuff = 0;
				//hr = D3DXCreateBuffer( 20000, &pxbuff );
				//if( hr != D3D_OK ){
				//	DbgOut( "E3DInit : D3DXCreateBuffer error !!!\n" );
				//	_ASSERT( 0 );
				//	return 1;
				//}

				if( s_SelectLightType == 0 ){
					hr = D3DXCreateEffectFromResource( pdev, s_hMod, MAKEINTRESOURCE(IDR_RCDATA1), NULL, NULL, dwShaderFlags, NULL, &g_pEffect, NULL );
				}else{
					hr = D3DXCreateEffectFromResource( pdev, s_hMod, MAKEINTRESOURCE(IDR_RCDATA4), NULL, NULL, dwShaderFlags, NULL, &g_pEffect, NULL );
				}
				if( hr != D3D_OK ){
					::MessageBox( NULL, "エフェクトファイルにエラーがあります。\n終了します。", "確認", MB_OK );
					if( pxbuff && pxbuff->GetBufferPointer() ){
						DbgOut( "effecterror!!!\r\n%s\r\n", (LPCTSTR)pxbuff->GetBufferPointer() );
					}
					DbgOut( "\n\nEffect File error %x !!!\n", hr );
					_ASSERT( 0 );
					g_useGPU = 0;
					return 1;
				}

				if( pxbuff ){
					pxbuff->Release();
				}

//DbgOut( "E3DInit : 001\r\n" );

				if( g_usePostEffect ){
					int existflag;
					existflag = _access( fxname, 0 );
					if( existflag != 0 ){
						::MessageBox( NULL, "fxファイルが見つかりません。\ndllと同じ場所にE3D_EXTフォルダをコピーしてください。",
							"エラー", MB_OK );
						_ASSERT( 0 );
						return 1;
					}
//DbgOut( "E3DInit : 002\r\n" );
//_ASSERT( 0 );


					WCHAR widename[MAX_PATH];
					ZeroMemory( widename, sizeof( WCHAR ) * MAX_PATH );
					ret = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, fxname, (int)strlen( fxname ), widename, MAX_PATH );
					_ASSERT( ret != 0 );
					LPD3DXBUFFER pErr;
					hr = D3DXCreateEffectFromFileW(
						pdev, widename, NULL, NULL, 
						D3DXSHADER_DEBUG , NULL, &g_pPostEffect, &pErr );
					if( hr != D3D_OK ){
						::MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
						DbgOut( "\n\nPostEffect File error !!! %x\n", hr );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					g_pPostEffect = 0;
				}

			}else{
				::MessageBox( NULL, "ハードウェアが要件を満たさないので実行できません。\n終了します。", "確認", MB_OK );
				DbgOut( "\n\nHardware too old error !!!\n" );
				_ASSERT( 0 );
				g_useGPU = 0;
				return 1;
			}
			
			ret = GetShaderHandle();
			if( ret ){
				DbgOut( "GetShaderHandle error !!!\n" );
				_ASSERT( 0 );
				g_useGPU = 0;
				return 1;
			}
		}
	}

	if( !g_kinect ){
		char kinectdir[2048];
		ZeroMemory( kinectdir, sizeof( char ) * 2048 );
		strncpy_s( kinectdir, 2048, moduledir, cpleng );
		strcat_s( kinectdir, 2048, "E3D_EXT\\Kinect" );

		g_kinect = new CPluginKinect( fullscflag );
		if( !g_kinect ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_kinect->SetFilePath( kinectdir );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = CMotionInfo::CreateTempMP();
	_ASSERT( !ret );


//d3dxsprite
	

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
	HRESULT hr1 = S_OK;
	if( s_initcomflag != 0 ){
		hr1 = CoInitialize(NULL);
	}
	//hr1 = S_OK;

	if( hr1 == S_OK ){
		s_HS = new CHuSound();
		if( !s_HS ){
			DbgOut( "E3DInit : HuSound alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int ret;
		ret = s_HS->Init( s_hwnd, enablereverb );
		if( ret ){
			DbgOut( "E3DInit : HS Init error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( !s_soundbank ){
			s_soundbank = new CSoundBank();
			if( !s_soundbank ){
				DbgOut( "E3DInit : new SoundBank error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		if( !s_sndah ){
			s_sndah = new CSndAnimHandler();
			if( !s_sndah ){
				DbgOut( "E3DInit : new SndAH error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		::MessageBox( s_hwnd, "COMの初期化に失敗しました。\n音は鳴らせません。", "エラーです。", MB_OK );
	}

////
	ret = SetFovIndex();
	_ASSERT( !ret );


////
	SetDefaultRenderState();

    hr = pdev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MINFILTER : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MAGFILTER : error !!!\r\n" );
	}

/***
    hr = pdev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MINFILTER : error !!!\r\n" );
	}
    hr = pdev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_MAGFILTER : error !!!\r\n" );
	}
***/
	if( g_cop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_aop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}

	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_COLORARG1 : error !!!\r\n" );
	}
	hr = pdev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_COLORARG2 : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	if( hr != D3D_OK ){
		DbgOut( "E3DInit : SetTextureStageState : D3DTSS_ADDRESSU : error !!!\r\n" );
	}
	hr = pdev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
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

// swapchain
	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		s_sctable[scno].scid = -1;
		s_sctable[scno].hwnd = 0;
		s_sctable[scno].texid = -1;
		s_sctable[scno].texsize.x = 0;
		s_sctable[scno].texsize.y = 0;
		s_sctable[scno].pSwapChain = NULL;
		s_sctable[scno].pZSurf = NULL;
		s_sctable[scno].pBackBuffer = NULL;
		s_sctable[scno].clearcol = D3DCOLOR_ARGB( 0, 0, 0, 0 );
		s_sctable[scno].needz = 1;
	}

	s_sctable[0].scid = 0;
	s_sctable[0].hwnd = s_hwnd;
	s_sctable[0].pSwapChain = NULL;
	s_sctable[0].pZSurf = g_pD3DApp->m_pZSurf;
	s_sctable[0].pBackBuffer = g_pD3DApp->m_pBackBuffer;
	s_sctable[0].clearcol = D3DCOLOR_ARGB( 0, 0, 0, 255 );
	s_sctable[0].needz = 1;

	s_swapchaincnt++;

//hsidelem
	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		s_hsidelem[hsidno].hsid = hsidno;
		s_hsidelem[hsidno].validflag = 0;
		s_hsidelem[hsidno].hsptr = 0;
	}

//bg
	for( scno = 0; scno < SCMAX; scno++ ){
		s_bgdisp[scno].scid = -1;
		s_bgdisp[scno].bgdisp = 0;
	}

//bumplight
	g_LightOfBump[0] = 0;
	g_LightOfBump[1] = -1;
	g_LightOfBump[2] = -1;


//affinity
//	HANDLE hproc;
//	hproc = GetCurrentProcess();
//	SetProcessAffinityMask( hproc, 1 );

	s_panda = new CPanda();
	if( !s_panda ){
		_ASSERT( 0 );
		return 1;
	}

	s_movienotifywnd = CreateWindow( "STATIC", "mvntfy", WS_OVERLAPPED, 0, 0, 10, 10, NULL, NULL, s_inst, NULL );
	if( !s_movienotifywnd ){
		_ASSERT( 0 );
		return 1;
	}
	s_movieoldfunc = ::GetWindowLong( s_movienotifywnd, GWL_WNDPROC );
	if( !s_movieoldfunc ){
		_ASSERT( 0 );
		return 1;
	}
	LONG lret;
	lret = ::SetWindowLong( s_movienotifywnd, GWL_WNDPROC, (LONG)MovieWndProc );
	if( lret == 0 ){
		_ASSERT( 0 );
		return 1;
	}


///
	byeflag = 0;
	g_activateapp = 1;


	return 0;
}


EASY3D_API int E3DBye()
{
	if( s_callinitflag == 0 )
		return 0;//!!!!!!!!!!!!!!!!

	if( byeflag == 1 )
		return 0;//!!!!!!!!!!!!!!!!

	if( s_playingmovieflag == 1 ){
		if( s_pMediaControl ){
			s_pMediaControl->Stop();
		}
		s_playingmovieflag = 0;
	}
	if( s_pMediaEventEx ){
		s_pMediaEventEx->Release();
		s_pMediaEventEx = 0;
	}
	if( s_pVideoWindow ){
		s_pVideoWindow->Release();
		s_pVideoWindow = 0;
	}
	if( s_pMediaControl ){
		s_pMediaControl->Release();
		s_pMediaControl = 0;
	}
	if( s_pGraphBuilder ){
		s_pGraphBuilder->Release();
		s_pGraphBuilder = 0;
	}
	if( s_pBasicAudio ){
		s_pBasicAudio->Release();
		s_pBasicAudio = 0;
	}
	if( s_movienotifywnd ){
		DestroyWindow( s_movienotifywnd );
		s_movienotifywnd = 0;
	}


	//if( s_threadnum > 0 ){
	int tableno;
	for( tableno = 0; tableno < THMAX; tableno++ ){

		FreeThTableElem( tableno );		
	}

	//}
	if( m_cah ){
		delete m_cah;
		m_cah = 0;
	}

	int tmno;
	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;

		curtm->tonemapid = -1;
		curtm->tonenum = 0;
		if( curtm->pscid ){
			free( curtm->pscid );
			curtm->pscid = 0;
		}
		if( curtm->ptexid ){
			free( curtm->ptexid );
			curtm->ptexid = 0;
		}
	}
	s_usertonemapcnt = 0;

	//if( srcwnd )
	//	DestroyWindow( srcwnd );

	FreeHandlerSet();

	CMotionInfo::DestroyTempMP();

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


	int scno;
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_bgdisp[scno].scid >= 0 ){
			delete s_bgdisp[scno].bgdisp;
			s_bgdisp[scno].bgdisp = 0;
			s_bgdisp[scno].scid = -1;
		}
	}
	

	if( s_d3dxsprite ){
		s_d3dxsprite->Release();
		s_d3dxsprite = 0;
	}

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
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

	if( s_panda ){
		delete s_panda;
		s_panda = 0;
	}

	//PostQuitMessage(0);//<----- DestroyWindowの後では、たぶん意味無し。
	if( s_effectdecl ){
		s_effectdecl->Release();
		s_effectdecl = 0;
	}
	if( s_tlvdecl ){
		s_tlvdecl->Release();
		s_tlvdecl = 0;
	}


	if( g_pEffect ){
		g_pEffect->Release();
		g_pEffect = 0;
	}
	if( g_pPostEffect ){
		g_pPostEffect->Release();
		g_pPostEffect = 0;
	}

//swapchain
	for( scno = 0; scno < SCMAX; scno++ ){
		if( s_sctable[scno].scid > 0 ){
			DestroySwapChainElem( &(s_sctable[scno]), 1 );
		}
	}

	if( g_kinect ){
		delete g_kinect;
		g_kinect = 0;
	}


	if( g_pD3DApp ){

		//_ASSERT( s_hwnd == srcwnd );

		//clean up
		g_pD3DApp->Cleanup3DEnvironment();
		//DestroyMenu( GetMenu(hWnd) );

		delete g_pD3DApp;
		g_pD3DApp = 0;
	
	}

	CloseDbgFile();

	if( s_initcomflag != 0 ){
		// Close down COM
		CoUninitialize();
	}

	DeleteCriticalSection( &g_crit_hshead );
	//DeleteCriticalSection( &g_crit_tempmp );
	DeleteCriticalSection( &g_crit_restore );

	byeflag = 1;

	return 0;
}


EASY3D_API int E3DCreateSwapChain( HWND srchwnd, int* scidptr )
{

	SWAPCHAINELEM* cursc = 0;
	cursc = GetFreeSwapChainElem();
	if( !cursc ){
		DbgOut( "CreateSwapChainElem : GetFreeSwapChainElem NULL error : swap chain num overflow error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = CreateSwapChainElem( s_swapchaincnt, cursc, srchwnd, 0, 0, 1 );
	if( ret ){
		DbgOut( "E3DCreateSwapChain : CreateSwapChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*scidptr = cursc->scid;
	s_swapchaincnt++;

	return 0;
}


EASY3D_API int E3DDestroySwapChain( int scid )
{

	if( scid <= 0 ){
		DbgOut( "E3DDestroySwapChain : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		return 0;
	}

	int ret;
	ret = DestroySwapChainElem( cursc, 1 );
	if( ret ){
		DbgOut( "E3DDestroySwapChain : DestroySwapChainElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSigLoad( const char* tempname, int adjustuvflag, float mult, int* hsidptr )
{
	char fname[MAX_PATH];
	if( tempname ){
		strcpy_s( fname, MAX_PATH, tempname );
	}else{
		DbgOut( "E3DSigLoad : name NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////
	int ret;
	int temphsid = -1;
	ret = SigLoad_F( fname, mult, adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoad : SigLoad_F error %s !!!\n", tempname );
		_ASSERT( 0 );
		*hsidptr = -1;
		return 1;
	}

	DbgOut( "E3DSigLoad : temphsid %d\r\n", temphsid );

	*hsidptr = temphsid;
	
	return 0;
}

int SigLoad_F( char* tempname, float mult, int adjustuvflag, int* hsidptr )
{
	int ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( (char*)tempname, newhs );//!!!!!!



	ret = newhs->LoadSigFile( tempname, adjustuvflag, mult );
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

	*hsidptr = newhs->serialno;


	return 0;
}

EASY3D_API int E3DRender( int scid, int hsid, int withalpha, int lightflag, int transskip, int lastparent )
{
	if( scid < 0 ){
		DbgOut( "E3DRender : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRender : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
	
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
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

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRender : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	/***
	s_matViewのセットは、E3DBeginSceneに移動！！！
	***/

	int* bonearray;
	if( lastparent != 0 ){
		curhs->SetBoneArray( lastparent );
		bonearray = curhs->m_bonearray;
	}else{
		bonearray = 0;
	}

	if( transskip == 0 ){
		int traboneonly = 0;
		ret = curhs->Transform( s_camerapos, pdev, s_matView, lightflag, traboneonly );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = curhs->Render( 0, pdev, withalpha, s_matView, s_camerapos );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DPresent( int scid )
{
	if( scid < 0 ){
		DbgOut( "E3DPresent : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPresent : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

    if( pdev && g_pD3DApp->m_bReady ){
		if( (scid == 0) && cursc->hwnd ){
			pdev->Present( NULL, NULL, cursc->hwnd, NULL );
		}else if( cursc->pSwapChain ){
			cursc->pSwapChain->Present( NULL, NULL, cursc->hwnd, NULL, D3DPRESENT_DONOTWAIT );
		}
	}

	g_rendercnt = 0;

	return 0;
}
EASY3D_API int E3DBeginScene( int scid, int skipflag, int zscid )
{
	if( scid < 0 ){
		DbgOut( "E3DBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DBeginScene : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* zsc = 0;
	int zsizex, zsizey;
	if( zscid >= 0 ){
		zsc = GetSwapChainElem( zscid );
		zsizex = g_scsize.x;
		zsizey = g_scsize.y;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int rendersizex, rendersizey;
	rendersizex = g_scsize.x;
	rendersizey = g_scsize.y;

	int clearzflag = 1;
	IDirect3DSurface9* zsurf = 0;
	if( zsc ){
		if( (zsizex == rendersizex) && (zsizey == rendersizey) ){
			zsurf = zsc->pZSurf;
			clearzflag = 0;
		}else{
			zsurf = cursc->pZSurf;
			clearzflag = 1;
		}
	}else{
		zsurf = cursc->pZSurf;
		clearzflag = 1;
	}


	HRESULT hres;
	hres = pdev->SetRenderTarget( 0, cursc->pBackBuffer );
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : 3ddev SetRenderTarget error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}
	//hres = pdev->SetDepthStencilSurface( cursc->pZSurf );
	hres = pdev->SetDepthStencilSurface( zsurf );
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}

	D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Height = g_scsize.y;
    viewport.Width =  g_scsize.x;
    viewport.MaxZ = 1.0f;
    viewport.MinZ = 0.0f;

    hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (skipflag & 2) == 0 ){
		CalcMatView();

		ret = SetShaderConst( 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

   if( FAILED( pdev->BeginScene() ) ){
		_ASSERT( 0 );
        return 1;
	}

    // Clear the viewport

	if( (skipflag & 1) == 0 ){
		if( clearzflag ){
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
								 cursc->clearcol, 1.0f, 0L );
		}else{
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET, 
								 cursc->clearcol, 1.0f, 0L );
		}
	   if( hres != D3D_OK ){
			//_ASSERT( 0 );
		   DbgOut( "E3DBegin : Clear error !!!\n" );
			return 1;
	   }
	}

//!!!!!!!!!!!!!!!!
//render bgdisp
//!!!!!!!!!!!!!!!!

	if( ((skipflag & 1) == 0) && ((skipflag & 4) == 0) ){

		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}

	return 0;
}

EASY3D_API int E3DEndScene()
{
	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return -1;
	}

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
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

EASY3D_API int E3DCreateBG( int scid, const char* tempname1, const char* tempname2, 
	float uanim, float vanim, int isround, float fogdist )
{
	int ret;
	if( scid < 0 ){
		DbgOut( "E3DCreateBG : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname1[MAX_PATH];
	char fname2[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DCreateBG : file name1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( tempname2 ){
		strcpy_s( fname2, MAX_PATH, tempname2 );
	}else{
		DbgOut( "E3DCreateBG : file name2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( fogdist <= 0.0f ){
		fogdist = g_proj_far;//!!!!!!!!!!!
	}

///////////

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	//_ASSERT( pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	BGDISPELEM* oldbgelem;
	oldbgelem = GetBGDispElem( scid );
	if( oldbgelem ){
		if( oldbgelem->bgdisp != 0 ){
			ret = g_texbnk->ResetDirtyFlag();
			ret = DestroyTexture( -2, -2, -2, 1, scid );
			if( ret ){
				DbgOut( "E3DCreateBG : DestroyTexture error !!!\n" );
				_ASSERT( 0 );
				//return 1;
			}

			delete oldbgelem->bgdisp;
			oldbgelem->bgdisp = 0;
			oldbgelem->scid = -1;
		}
	}

	BGDISPELEM* newbgelem;
	if( oldbgelem ){
		newbgelem = oldbgelem;
		newbgelem->scid = scid;
	}else{
		newbgelem = GetFreeBGDispElem();
	}
	if( !newbgelem ){
		DbgOut( "E3DCreateBG : newbg NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	UV startuv = { 0.0f, 0.0f };
	UV enduv = { 1.0f, 1.0f };

	CBgDisp2* newbg;
	newbg = new CBgDisp2( 0, pdev, fname1, 0, fname2, 0, uanim, vanim, 0xFFFFFFFF, isround, fogdist, startuv, enduv, 0 );
	if( !newbg ){
		DbgOut( "E3DSetBG : newbg alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	newbgelem->bgdisp = newbg;
	newbgelem->scid = scid;

/////////

	return 0;
}

EASY3D_API int E3DSetBGU( int scid, float startu, float endu )
{
	if( scid < 0 ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	BGDISPELEM* curbgelem;
	curbgelem = GetBGDispElem( scid );
	if( !curbgelem ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	if( curbgelem->bgdisp ){
		ret = curbgelem->bgdisp->SetU( startu, endu );
		if( ret ){
			DbgOut( "E3DSetBGU : bgdisp SetU error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}

EASY3D_API int E3DSetBGV( int scid, float startv, float endv )
{
	if( scid < 0 ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////

	BGDISPELEM* curbgelem;
	curbgelem = GetBGDispElem( scid );
	if( !curbgelem ){
		DbgOut( "E3DSetBGU : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	if( curbgelem->bgdisp ){
		ret = curbgelem->bgdisp->SetV( startv, endv );
		if( ret ){
			DbgOut( "E3DSetBGV : bgdisp SetV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}


EASY3D_API int E3DDestroyBG( int scid )
{
	int ret;
	if( scid < 0 ){
		DbgOut( "E3DDestroyBG : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	BGDISPELEM* delbgelem;
	delbgelem = GetBGDispElem( scid );
	if( delbgelem ){
		ret = g_texbnk->ResetDirtyFlag();
		ret = DestroyTexture( -2, -2, -2, 1, scid );
		if( ret ){
			DbgOut( "E3DDestroyBG : DestroyTexture error !!!\n" );
			_ASSERT( 0 );
			//return 1;
		}

		delbgelem->scid = -1;
		if( delbgelem->bgdisp != 0 ){
			delete delbgelem->bgdisp;
			delbgelem->bgdisp = 0;
		}
	}
	return 0;
}

EASY3D_API int E3DAddMotion( int hsid, const char* tempname, float mvmult, int* motidptr, int* maxframeptr )
{
	int ret;
////////

	char fname[MAX_PATH];
	if( tempname ){
		strcpy_s( fname, MAX_PATH, tempname );
	}else{
		DbgOut( "E3DAddMotion : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int tempmotid = -1;
	int tempmax = 0;
	ret = AddMotion_F( hsid, fname, &tempmotid, &tempmax, mvmult );
	if( ret ){
		DbgOut( "E3DAddMotion : AddMotoin_F error %s !!!\n", tempname );
		_ASSERT( 0 );

		*motidptr = -1;
		*maxframeptr = 0;
		return 1;
	}

	*motidptr = tempmotid;
	*maxframeptr = tempmax;

	return 0;

}

int AddMotion_F( int hsid, char* tempname, int* cookieptr, int* maxnumptr, float mvmult )
{
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


EnterCriticalSection( &(curhs->m_crit_addmotion) );//######## start crit
	
	int ret = 0;
	ret = curhs->LoadQuaFile( tempname, cookieptr, maxnumptr, mvmult );	

LeaveCriticalSection( &(curhs->m_crit_addmotion) );//###### end crit


	if( ret ){
		DbgOut( "E3DAddMotion : Load***File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetMotionKind( int hsid, int motid )
{	
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionKind : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetMotionKind( motid );
	if( ret ){
		DbgOut( "E3DSetMotionKind : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMotionKind( int hsid, int* motidptr )
{
///////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionKind : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	int dstmotkind = -1;
	ret = curhs->GetMotionKind( &dstmotkind );
	if( ret ){
		DbgOut( "E3DGetMotionKind : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*motidptr = dstmotkind;


	return 0;
}

EASY3D_API int E3DSetNewPose( int hsid, int* frameptr)
{	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPose : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int nextno;
	nextno = curhs->SetNewPose();
	_ASSERT( nextno >= 0 );

	*frameptr = nextno;

	return 0;
}

EASY3D_API int E3DSetMotionStep( int hsid, int motid, int step )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionStep : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret = 0;
	ret = curhs->SetMotionStep( motid, step );
	if( ret ){
		DbgOut( "E3DSetMotionStep : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetPos( int hsid, D3DXVECTOR3 pos )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_gpd.m_gpe.e3dpos = pos;
	curhs->m_gpd.CalcMatWorld();

	return 0;
}

EASY3D_API int E3DGetPos( int hsid, D3DXVECTOR3* posptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = curhs->m_gpd.m_gpe.e3dpos;

	return 0;

}


EASY3D_API int E3DSetDir( int hsid, D3DXVECTOR3 dir )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDir : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_gpd.m_gpe.e3drot = dir;
	curhs->m_gpd.CalcMatWorld();

	return 0;
}

EASY3D_API int E3DRotateInit( int hsid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateInit : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curhs->m_gpd.m_gpe.e3drot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	curhs->m_gpd.CalcMatWorld();

	return 0;
}

EASY3D_API int E3DRotateX( int hsid, float dirx )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateX : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisx( 1.0f, 0.0f, 0.0f );
	CQuaternion qx, newq;
	qx.SetAxisAndRot( axisx, dirx * (float)DEG2PAI );
	newq = qx * curhs->m_gpd.m_q;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );

	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.e3drot );

	curhs->m_gpd.m_gpe.e3drot = neweul;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}
EASY3D_API int E3DRotateY( int hsid, float diry )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateY : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisy( 0.0f, 1.0f, 0.0f );
	CQuaternion qy, newq;
	qy.SetAxisAndRot( axisy, diry * (float)DEG2PAI );
	newq = qy * curhs->m_gpd.m_q;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.e3drot );
	curhs->m_gpd.m_gpe.e3drot = neweul;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}

EASY3D_API int E3DRotateZ( int hsid, float dirz )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRotateZ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisz( 0.0f, 0.0f, 1.0f );
	CQuaternion qz, newq;
	qz.SetAxisAndRot( axisz, dirz * (float)DEG2PAI );
	newq = qz * curhs->m_gpd.m_q;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.e3drot );
	curhs->m_gpd.m_gpe.e3drot = neweul;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}


EASY3D_API int E3DTwist( int hsid, float step )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DTwist : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	float steprad;
	D3DXVECTOR3 axis;
	D3DXVECTOR3 orgvec( 0.0f, 0.0f, -1.0f );

	ret = curhs->m_gpd.m_q.Rotate( &axis, orgvec );
	_ASSERT( !ret );
	DXVec3Normalize( &axis, &axis );

	steprad = (float)step * (float)DEG2PAI;//変化分
	CQuaternion newq, multq;

	multq.SetAxisAndRot( axis, steprad );
	newq = multq * curhs->m_gpd.m_q;

	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.e3drot );
	_ASSERT( !ret );
	curhs->m_gpd.m_gpe.e3drot = neweul;

	curhs->m_gpd.CalcMatWorld();

	float savetwist;
	savetwist = curhs->m_twist;
	// 0 - 360
	float finaltwist, settwist;
	finaltwist = savetwist + step;

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
	curhs->m_twist = settwist;


	return 0;
}

EASY3D_API int E3DPosForward( int hsid, float step )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPosForward : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	D3DXVECTOR3 orgvec( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 stepvec( 0.0f, 0.0f, 1.0f );
	
	ret = curhs->m_gpd.m_q.Rotate( &stepvec, orgvec );
	_ASSERT( !ret );

	curhs->m_gpd.m_gpe.e3dpos += step * stepvec;

	curhs->m_gpd.CalcMatWorld();

	return 0;
}

EASY3D_API int E3DCloseTo( int chghsid, int srchsid, float movestep )
{
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
	vec3 = srchs->m_gpd.m_gpe.e3dpos - chghs->m_gpd.m_gpe.e3dpos;
	if( (vec3.x != 0.0f) || (vec3.y != 0.0f) && (vec3.z != 0.0f) )
		D3DXVec3Normalize( &nvec3, &vec3 );
	
	distmag = ( vec3.x * vec3.x + vec3.y * vec3.y + vec3.z * vec3.z );
	if( distmag > (movestep * movestep) ){
		chghs->m_gpd.m_gpe.e3dpos += movestep * nvec3;
	}else{
		zeroflag = 1;//!!!!!!!!!!
		chghs->m_gpd.m_gpe.e3dpos = srchs->m_gpd.m_gpe.e3dpos;
	}

	chghs->m_gpd.CalcMatWorld();
	int ret;
	if( zeroflag == 0 ){
		ret = DirToTheOther( &(chghs->m_gpd), srchs->m_gpd );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int DirToTheOtherXZ(  CGPData* set1, CGPData set2 )
{
	D3DXVECTOR2 basevec;
	D3DXVECTOR2 vec;
	D3DXVECTOR2 nvec;
	
	basevec.x = 0.0f;
	basevec.y = -1.0f;

	vec.x = set2.m_gpe.e3dpos.x - set1->m_gpe.e3dpos.x;
	vec.y = set2.m_gpe.e3dpos.z - set1->m_gpe.e3dpos.z;

	D3DXVec2Normalize( &nvec, &vec );
	float dot;
	dot = D3DXVec2Dot( &basevec, &nvec );
	//float rad;
	if( dot <= -1.0f )
		dot = -1.0f;
	else if( dot >= 1.0f )
		dot = 1.0f;

	float retrad;
	if( dot >= 0.999999619f ){
		retrad = 0.0f;
	}else if( dot <= -0.999999619f ){
		retrad = (float)PI;
	}else{
		retrad = (float)acos( dot );
	}

	float ccw;
	ccw = D3DXVec2CCW( &basevec, &nvec );

	float roty;
	if( ccw >= 0.0f ){
		roty = -retrad;
	}else{
		roty = retrad;
	}

	set1->m_gpe.e3drot.x = 0.0f;
	set1->m_gpe.e3drot.y = roty * (float)PAI2DEG;
	set1->m_gpe.e3drot.z = 0.0f;

	set1->CalcMatWorld();
	

	return 0;
}

int DirToTheOther(  CGPData* set1, CGPData set2 )
{
	D3DXVECTOR3 basevec;
	D3DXVECTOR3 vec;
	
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = -1.0f;

	vec = set2.m_gpe.e3dpos - set1->m_gpe.e3dpos;
	D3DXVec3Normalize( &vec, &vec );

	D3DXVECTOR3 axis;

	D3DXVec3Cross( &axis, &basevec, &vec );
	D3DXVec3Normalize( &axis, &axis );

	float dot, retrad;
	dot = D3DXVec3Dot( &basevec, &vec );
	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;

	if( dot >= 0.999999619f ){
		retrad = 0.0f;
	}else if( dot <= -0.999999619f ){
		retrad = (float)PI;
	}else{
		retrad = (float)acos( dot );
	}

	CQuaternion q0, q1;
	q0.SetAxisAndRot( axis, retrad );
	q1.SetAxisAndRot( axis, -retrad );

	D3DXVECTOR3 testvec0, testvec1;
	q0.Rotate( &testvec0, basevec );
	q1.Rotate( &testvec1, basevec );

	float testdot0, testdot1;
	testdot0 = D3DXVec3Dot( &vec, &testvec0 );
	testdot1 = D3DXVec3Dot( &vec, &testvec1 );

	CQuaternion q;
	if( testdot0 >= testdot1 )
		q = q0;
	else
		q = q1;

	int ret;
	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &q, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &set1->m_gpe.e3drot );
	_ASSERT( !ret );
	set1->m_gpe.e3drot = neweul;

	set1->CalcMatWorld();

	return 0;
}

EASY3D_API int E3DDirToTheOtherXZ( int chghsid, int srchsid )
{
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
	ret = DirToTheOtherXZ( &(chghs->m_gpd), srchs->m_gpd );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDirToTheOther( int chghsid, int srchsid )
{
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
	ret = DirToTheOther( &(chghs->m_gpd), srchs->m_gpd );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSeparateFrom( int chghsid, int srchsid, float dist )
{
	int ret;
	ret = SeparateFrom( chghsid, srchsid, dist );
	_ASSERT( !ret );

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

	vec.x = srchs->m_gpd.m_gpe.e3dpos.x - chghs->m_gpd.m_gpe.e3dpos.x;
	vec.y = srchs->m_gpd.m_gpe.e3dpos.z - chghs->m_gpd.m_gpe.e3dpos.z;

	D3DXVec2Normalize( &nvec, &vec );
	chghs->m_gpd.m_gpe.e3dpos.x -= dist * nvec.x;
	chghs->m_gpd.m_gpe.e3dpos.z -= dist * nvec.y;

	chghs->m_gpd.CalcMatWorld();


	return 0;
}

EASY3D_API int E3DGetCameraPos( D3DXVECTOR3* posptr )
{
	*posptr = s_camerapos;
	return 0;
}

EASY3D_API int E3DSetCameraPos( D3DXVECTOR3 pos )
{
	s_camerapos = pos;
	return 0;
}

EASY3D_API int E3DGetCameraDeg( float* degxzptr, float* degyptr )
{
	*degxzptr = s_camera_degxz;
	*degyptr = s_camera_degy;

	return 0;
}

EASY3D_API int E3DSetCameraDeg( float degxz, float degy )
{
	s_camera_degxz = degxz;
	s_camera_degy = degy;

	s_cameratype = 0;
	
	return 0;
}
EASY3D_API int E3DSetCameraTarget( D3DXVECTOR3 target, D3DXVECTOR3 upvec)
{
	s_cameratarget = target;
	s_cameraupvec = upvec;//一時的に。後でツイストする。
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

EASY3D_API int E3DChkInView( int scid, int hsid, int* statptr )
{
	if( scid < 0 ){
		DbgOut( "E3DChkInView : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkInView : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DChkInView : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int retflag = 0;
	//ret = curhs->ChkInView( s_camerapos, viewvec, (float)g_proj_near, (float)g_proj_far, (float)g_proj_fov * fDeg2Pai );
	ret = curhs->ChkInView( s_matView, &retflag, 0 );
	if( ret ){
		DbgOut( "E3DRender : ChkInView error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( retflag != 0 ){
		*statptr = 1;
	}else{
		*statptr = 0;
	}
	return 0;
}

EASY3D_API int E3DEnableDbgFile()
{

	OpenDbgFile();	
	return 0;

}

EASY3D_API int E3DSetProjection( float nearclip, float farclip, float fov )
{
	g_ortho = 0;
	g_proj_near = nearclip;
	g_proj_far = farclip;
	g_proj_fov = fov;

	if( g_proj_fov == 0.0f )
		g_proj_fov = 45.0f; //default値
	
	if( g_proj_fov < 0.0f )
		g_proj_fov *= -1.0f;

	if( g_proj_fov >= 180.0f )
		g_proj_fov = 179.0f; // fov / 2 が　90以上にならないようにする。90は発散するので禁止。

	if( g_proj_near >= g_proj_far ){
		g_proj_near = 100.0f; //default
		g_proj_far = 10000.0f;//default
	}

	int ret;
	ret = SetFovIndex();
	_ASSERT( !ret );

//// fog
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );

	g_fogend = g_proj_far;
	g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
	if( pdev ){
		pdev->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
	}

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			ret = s_hsidelem[hsidno].hsptr->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
			_ASSERT( !ret );
		}
	}

	if( g_bbhs ){
		ret = g_bbhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
		_ASSERT( !ret );
	}


	return 0;
}

EASY3D_API int E3DGetKeyboardState( int* stateptr )
{
	unsigned char tempstate[256];

	GetKeyboardState( tempstate );

	int cno;
	for( cno = 0; cno < 256; cno++ ){
		if( tempstate[cno] & 0x80 ){
			*( stateptr + cno ) = 1;
		}else{
			*( stateptr + cno ) = 0;
		}
	}

	return 0;
}

EASY3D_API int E3DGetKeyboardCnt( int* cntptr )
{
	unsigned char tempstate[256];
	GetKeyboardState( tempstate );


	int cno;
	for( cno = 0; cno < 256; cno++ ){
		if( tempstate[cno] & 0x80 ){
			(s_keyboardcnt[cno])++;
			*( cntptr + cno ) = s_keyboardcnt[cno];
		}else{
			s_keyboardcnt[cno] = 0;
			*( cntptr + cno ) = 0;
		}
	}

	return 0;
}
EASY3D_API int E3DResetKeyboardCnt()
{

	ZeroMemory( s_keyboardcnt, sizeof( int ) * 256 );

	return 0;
}

EASY3D_API int E3DChkConflictAABB( int srchsid, int chkhsid, int* confptr, int* inviewptr )
{
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflictAABB : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflictAABB : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict( chkhs, fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflictAABB : srchs ChkConflict error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confptr = conf;
	*inviewptr = inviewflag;

	return 0;
}

EASY3D_API int E3DChkConflictOBB( int hsid1, int partno1, int hsid2, int partno2, int* confptr, int* inviewptr )
{
	CHandlerSet* srchs = GetHandlerSet( hsid1 );
	if( !srchs ){
		DbgOut( "E3DChkConflictOBB : hsid1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* chkhs = GetHandlerSet( hsid2 );
	if( !chkhs ){
		DbgOut( "E3DChkConflictOBB : hsid2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict2( partno1, chkhs, partno2, fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflictOBB : srchs ChkConflict2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confptr = conf;
	*inviewptr = inviewflag;

	return 0;
}

EASY3D_API int E3DChkConflictOBBArray( int srchsid, int* srcpart, int srcpartnum, int chkhsid, int* chkpart, int chkpartnum, 
	int* confptr, int* inviewptr )
{
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflictOBBArray : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflictOBBArray : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	D3DXVECTOR3 fixedrate;
	fixedrate.x = 1.0f;
	fixedrate.y = 1.0f;
	fixedrate.z = 1.0f;

	int conf;
	int inviewflag = 0;
	conf = srchs->ChkConflict3( srcpart, srcpartnum, 
		chkhs, chkpart, chkpartnum,
		fixedrate, fixedrate, &inviewflag );
	if( conf < 0 ){
		DbgOut( "E3DChkConflictOBBArray : srchs ChkConflict3 error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	*confptr = conf;
	*inviewptr = inviewflag;

	return 0;
}

EASY3D_API int E3DChkConfBillboard( int hsid, float confrate, int* confptr, int* bbidptr, int arrayleng, int* confnumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfBillboard : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int confflag = 0;
	int confnum = 0;

	if( g_bbhs ){
		int ret;
		ret = g_bbhs->ChkConfBillboard( curhs, &confflag, confrate, bbidptr, arrayleng, &confnum );
		if( ret ){
			DbgOut( "E3DChkConfBySphere : bbhs ChkConfBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	
	}else{
		DbgOut( "E3DChkConfBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confptr = confflag;
	*confnumptr = confnum;

	return 0;
}


EASY3D_API int E3DChkConfBillboardBS( D3DXVECTOR3 center, float srcr, float confrate, 
	int* confptr, int* bbidptr, int arrayleng, int* confnumptr )
{
	int confflag = 0;
	int confnum = 0;
	if( g_bbhs ){
		int ret;
		ret = g_bbhs->ChkConfBillboard2( center.x, center.y, center.z, srcr, &confflag, confrate, bbidptr, arrayleng, &confnum );
		if( ret ){
			DbgOut( "E3DChkConfBillboardBS : bbhs ChkConfBillboard2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	
	}else{
		DbgOut( "E3DChkConfBillboardBS : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confptr = confflag;
	*confnumptr = confnum;

	return 0;
}


EASY3D_API int E3DChkConflictBS( int srchsid, int chkhsid, int conflevel, int* confptr )
{
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflictBS : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflictBS : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int confflag = 0;
	int ret;
	ret = srchs->ChkConfBySphere( chkhs, conflevel, &confflag );
	if( ret ){
		DbgOut( "E3DChkConflictBS : srchs ChkConfBySphere error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confptr = confflag;

	return 0;
}

EASY3D_API int E3DChkConflictBSPart( int srchsid, int srcpartno, int chkhsid, int chkpartno, int* confptr )
{
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflictBSPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflictBSPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int confflag = 0;
	int ret;
	ret = srchs->ChkConfBySphere2( srcpartno, chkhs, chkpartno, &confflag );
	if( ret ){
		DbgOut( "E3DChkConflictBSPart : srchs ChkConfBySphere2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confptr = confflag;

	return 0;
}
EASY3D_API int E3DChkConflictBSPartArray( int srchsid, int* srcpart, int srcpartnum, int chkhsid, int* chkpart, int chkpartnum, int* confptr )
{
	CHandlerSet* srchs = GetHandlerSet( srchsid );
	if( !srchs ){
		DbgOut( "E3DChkConflictBSPartArray : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	CHandlerSet* chkhs = GetHandlerSet( chkhsid );
	if( !chkhs ){
		DbgOut( "E3DChkConflictBSPartArray : hsid error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	int confflag = 0;
	int ret;
	ret = srchs->ChkConfBySphere3( srcpart, srcpartnum, 
		chkhs, chkpart, chkpartnum, &confflag );
	if( ret ){
		DbgOut( "E3DChkConflictBSPartArray : srchs ChkConfBySphere3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*confptr = confflag;

	return 0;
}

EASY3D_API int E3DChkConfGround( int charahsid, int groundhsid, int mode, float diffmaxy, float mapminy, 
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n )
{
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

	int result = 0;
	float adx, ady, adz;
	float nx, ny, nz;

	ret = groundhs->ChkConfGround( charahs, mode, diffmaxy, mapminy, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGround : groundhs ChkConfGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resultptr = result;
	adjust->x = adx;
	adjust->y = ady;
	adjust->z = adz;
	n->x = nx;
	n->y = ny;
	n->z = nz;

	return 0;
}

EASY3D_API int E3DChkConfGroundVec( D3DXVECTOR3 befv, D3DXVECTOR3 newv, int groundhsid, int mode, float diffmaxy, float mapminy,
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n )
{
	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGroundVec : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int result = 0;
	float adx, ady, adz;
	float nx, ny, nz;

	ret = groundhs->ChkConfGround2( befv, newv, mode, diffmaxy, mapminy, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGroundVec : groundhs ChkConfGround2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resultptr = result;
	adjust->x = adx;
	adjust->y = ady;
	adjust->z = adz;
	n->x = nx;
	n->y = ny;
	n->z = nz;

	return 0;
}

EASY3D_API int E3DChkConfGroundPart( int charahsid, int groundhsid, int groundpart, int mode, float diffmaxy, float mapminy,
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n )
{
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

	int result;
	float ax, ay, az, nx, ny, nz;
	ret = groundhs->ChkConfGroundPart( charahs, groundpart, mode, diffmaxy, mapminy, &result, &ax, &ay, &az, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGroundPart : groundhs ChkConfGroundPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resultptr = result;
	adjust->x = ax;
	adjust->y = ay;
	adjust->z = az;
	n->x = nx;
	n->y = ny;
	n->z = nz;

	return 0;
}


EASY3D_API int E3DChkConfGroundVecPart( D3DXVECTOR3 befv, D3DXVECTOR3 newv, int groundhsid, int groundpart, int mode, float diffmaxy, float mapminy,
	int* resultptr, D3DXVECTOR3* adjust, D3DXVECTOR3* n )
{
	int ret;

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfGroundVecPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int result;
	float ax, ay, az, nx, ny, nz;
	ret = groundhs->ChkConfGroundPart2( groundpart, befv, newv, mode, diffmaxy, mapminy, &result, &ax, &ay, &az, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfGroundVecPart : groundhs ChkConfGroundPart2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resultptr = result;
	adjust->x = ax;
	adjust->y = ay;
	adjust->z = az;
	n->x = nx;
	n->y = ny;
	n->z = nz;


	return 0;
}

EASY3D_API int E3DChkConfWall( int charahsid, int groundhsid, float dist, int* resultptr, D3DXVECTOR3* adjustptr, D3DXVECTOR3* nptr )
{
	int ret;
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
	
	int result = 0;
	float adx, ady, adz, nx, ny, nz;

	ret = groundhs->ChkConfWall( charahs, dist, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfWall : groundhs ChkConfWall error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resultptr = result;
	adjustptr->x = adx;
	adjustptr->y = ady;
	adjustptr->z = adz;
	nptr->x = nx;
	nptr->y = ny;
	nptr->z = nz;


	return 0;
}

EASY3D_API int E3DChkConfWallVec( D3DXVECTOR3 befpos, D3DXVECTOR3 newpos, int groundhsid, float dist, 
	int* resultptr, D3DXVECTOR3* adjustptr, D3DXVECTOR3* nptr )
{
	int ret;
	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfWallVec : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int result = 0;
	float adx, ady, adz, nx, ny, nz;


	ret = groundhs->ChkConfWall2( 0, 0, befpos.x, befpos.y, befpos.z, newpos.x, newpos.y, newpos.z, dist, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfWallVec : groundhs ChkConfWall2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	*resultptr = result;
	adjustptr->x = adx;
	adjustptr->y = ady;
	adjustptr->z = adz;
	nptr->x = nx;
	nptr->y = ny;
	nptr->z = nz;

	return 0;
}

EASY3D_API int E3DChkConfWallPartArray( int charahsid, int groundhsid, int* partarray, int partnum, float dist,
	int* resultptr, D3DXVECTOR3* adjustptr, D3DXVECTOR3* nptr )
{
	int ret;

	CHandlerSet* charahs = GetHandlerSet( charahsid );
	if( !charahs ){
		DbgOut( "E3DChkConfWallPartArray : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* groundhs = GetHandlerSet( groundhsid );
	if( !groundhs ){
		DbgOut( "E3DChkConfWallPartArray : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int result = 0;
	float adx, ady, adz, nx, ny, nz;

	ret = groundhs->ChkConfWall3( charahs, partarray, partnum, dist, &result, &adx, &ady, &adz, &nx, &ny, &nz );
	if( ret ){
		DbgOut( "E3DChkConfWallPartArray : groundhs ChkConfWall3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*resultptr = result;
	adjustptr->x = adx;
	adjustptr->y = ady;
	adjustptr->z = adz;
	nptr->x = nx;
	nptr->y = ny;
	nptr->z = nz;

	return 0;
}

EASY3D_API int E3DChkConfLineAndFace( D3DXVECTOR3 srcpos1, D3DXVECTOR3 srcpos2, int hsid, int needtrans,
	int* partptr, int* faceptr, D3DXVECTOR3* posptr, D3DXVECTOR3* nptr, int* revfaceptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfLineAndFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 dstpos( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 dstn( 0.0f, 0.0f, 0.0f );

	D3DXVECTOR3 camdir;
	camdir = s_cameratarget - s_camerapos;

	int ret;

	ret = curhs->ChkConfLineAndFace( camdir, s_matView, srcpos1, srcpos2, needtrans, partptr, faceptr, posptr, nptr, revfaceptr );
	if( ret ){
		DbgOut( "E3DChkConfLineAndFace : curhs ChkCnofLineAndFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( *partptr >= 0 ){
		D3DXVec3Normalize( nptr, nptr );
	}

	return 0;
}

EASY3D_API int E3DChkConfParticle( int ptclid, int hsid, float rate, int* confptr )
{
	CHandlerSet* curptcl = GetHandlerSet( ptclid );
	if( !curptcl ){
		DbgOut( "E3DChkConfParticle : ptclid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkConfParticle : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int flag = 0;
	ret = curptcl->ChkConfParticle( curhs, rate, &flag );
	if( ret ){
		DbgOut( "E3DChkConfParticle : ptcl ChkConfParticle error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*confptr = flag;

	return 0;
}

EASY3D_API int E3DDrawText( D3DXVECTOR2 scpos, float scale, E3DCOLOR4UC col, const char* strptr )
{	
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pD3DApp->m_pFont ){
		_ASSERT( 0 );
		return 1;
	}


	D3DCOLOR d3dcol;
	d3dcol = D3DCOLOR_ARGB( 255, col.r, col.g, col.b );

	(g_pD3DApp->m_pFont)->m_fTextScale = scale;
	(g_pD3DApp->m_pFont)->DrawText( scpos.x, scpos.y, d3dcol, (char*)strptr );
	
	return 0;
}

EASY3D_API int E3DDrawBigText( D3DXVECTOR2 scpos, float scale, E3DCOLOR4UC col, const char* strptr )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pD3DApp->m_pFontBig ){
		_ASSERT( 0 );
		return 1;
	}

	D3DCOLOR d3dcol;
	d3dcol = D3DCOLOR_ARGB( 255, col.r, col.g, col.b );

	(g_pD3DApp->m_pFontBig)->m_fTextScale = scale;
	(g_pD3DApp->m_pFontBig)->DrawText( scpos.x, scpos.y, d3dcol, (char*)strptr );
	
	return 0;
}

EASY3D_API int E3DCreateFont( int height, int width, int weight, int bItalic, int bUnderline, int bStrikeout, const char* tempname, int* idptr )
{

	int ret;

//////

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateFont : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	LPD3DXSPRITE fontsprite;
	hr = D3DXCreateSprite( pdev, &fontsprite );//releaseはxfontで行う。
	if( hr != D3D_OK ){
		DbgOut( "e3dRestoreDeviceObject : D3DXCreateSprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CXFont* newfont;
	newfont = new CXFont();
	if( !newfont ){
		DbgOut( "E3DCreateFont : newfont alloc error !!!\n" );
		_ASSERT( 0 );
		fontsprite->Release();
		return 1;
	}

	ret = newfont->CreateFont( pdev, fontsprite, height, width, weight, bItalic, bUnderline, bStrikeout, (char*)tempname );
	if( ret ){
		DbgOut( "E3DCreateFont : newfont CreateFont error !!!\n" );
		_ASSERT( 0 );
		fontsprite->Release();
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

	*idptr = newfont->m_serialno;

	return 0;
}

EASY3D_API int E3DDrawTextByFontID( int scid, int fontid, POINT scpos, const char* tempname, E3DCOLOR4UC col, int bmode )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!
	}

	if( scid < 0 ){
		DbgOut( "E3DDrawTextByFontID : scid error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}
////////////

	int ret;
	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontID : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DDrawTextByFontID : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DCOLOR d3dcol;
	d3dcol = D3DCOLOR_ARGB( col.a, col.r, col.g, col.b );


	DisableFogState();

	ret = curfont->DrawText( (char*)tempname, scpos.x, scpos.y, d3dcol, bmode );
	if( ret ){
		DbgOut( "E3DDrawTextByFontID : curfont DrawText error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RollbackFogState();

	return 0;
}

EASY3D_API int E3DDrawTextByFontIDWithCnt( int scid, int fontid, POINT scpos, const char* tempname, E3DCOLOR4UC col, int eachcnt, int curcnt )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!
	}


	if( scid < 0 ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////
	int ret;
	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DCOLOR d3dcol;
	d3dcol = D3DCOLOR_ARGB( col.a, col.r, col.g, col.b );

	DisableFogState();

	ret = curfont->DrawTextWithCnt( (char*)tempname, scpos.x, scpos.y, d3dcol, eachcnt, curcnt );
	if( ret ){
		DbgOut( "E3DDrawTextByFontIDWithCnt : curfont DrawTextWithCnt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RollbackFogState();

	return 0;
}

EASY3D_API int E3DDrawTextByFontIDWithCntEach( int scid, int fontid, POINT scpos, const char* tempname, E3DCOLOR4UC col,
	int* cntarray, int arrayleng, int curcnt )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!
	}

	if( scid < 0 ){
		DbgOut( "E3DDrawTextByFontIDWithCntEach : scid error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}
//////
	int ret;
	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DDrawTextByFontIDWithCnt2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DCOLOR d3dcol;
	d3dcol = D3DCOLOR_ARGB( col.a, col.r, col.g, col.b );

	DisableFogState();

	ret = curfont->DrawTextWithCnt2( (char*)tempname, scpos.x, scpos.y, d3dcol, cntarray, arrayleng, curcnt );
	if( ret ){
		DbgOut( "E3DDrawTextByFontIDEach : curfont DrawTextWithCnt2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RollbackFogState();

	return 0;
}

EASY3D_API int E3DGetCharacterNum( const char* tempname, int* numptr )
{


	if( !tempname ){
		int zeronum = 0;
		*numptr = 0;
		return 0;
	}

	unsigned char curc;
	int totalnum = 0;
	int curpos = 0;
	
	int orgleng;
	orgleng = (int)strlen( (char*)tempname );
	if( orgleng <= 0 ){
		*numptr = 0;
		return 0;
	}

	while( curpos < orgleng ){
		curc = *( tempname + curpos );

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


EASY3D_API int E3DDestroyFont( int fontid )
{

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

EASY3D_API int E3DGetFPS( int* fpsptr )
{
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
		timeBeginPeriod( 1 );
		curtime = timeGetTime();
		timeEndPeriod( 1 );

		difftime = curtime - beftime;

		retfps = (DWORD)( 1.0 / ((float)difftime / 1000.0) );
		
		beftime = curtime;
	}

	*fpsptr = retfps;

	return 0;

}

EASY3D_API int E3DWaitbyFPS( int srcfps, int* retfpsptr )
{
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
			lldifftime = 0xFFFFFFFF + lcurtime.QuadPart - lbeftime.QuadPart;
			//lldifftime = s_timerfreq * 1000;
			//lldifftime = lbeftime.QuadPart;

		if( lldifftime < 0 ){
			lldifftime = 0;
		}

		curmsec = (DWORD)((float)lldifftime / (float)s_timerfreq * 1000.0f);

		if( srcmsec >= curmsec ){
			DWORD sleepmsec;
			sleepmsec = srcmsec - curmsec;
			if( sleepmsec > 1000 ){
				sleepmsec = 1000;
			}
			timeBeginPeriod( 1 );
			//::MsgWaitForMultipleObjects( 0, NULL, FALSE, sleepmsec, 0 );
			Sleep( sleepmsec );
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
			lldifftime = s_timerfreq * 1000;
			//lldifftime = lbeftime.QuadPart;

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
			difftime = 1000;
			//difftime = beftime;
		
		if( srcmsec >= difftime ){
			DWORD sleepmsec;
			sleepmsec = srcmsec - difftime;
			if( sleepmsec > 1000 ){
				sleepmsec = 1000;
			}
			timeBeginPeriod( 1 );
			//::MsgWaitForMultipleObjects( 0, NULL, FALSE, sleepmsec, 0 );
			Sleep( sleepmsec );
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
			difftime = 1000;
			//difftime = beftime;


		if( difftime != 0 )
			retfps = (int)( 1.0 / ((float)difftime / 1000.0) );
		else
			retfps = 0;

//DbgOut( "E3DWaitbyFPS : tgt : retfps %d\n", retfps );		

		beftime = curtime;
	}

	*retfpsptr = retfps;

	return 0;

}

EASY3D_API int E3DCreateLight( int* lidptr )
{
	
	int lid = -1;
	CLightData* newlight = 0;

	newlight = new CLightData( &lid );
	if( !newlight ){
		DbgOut( "E3DCreateLight : newlight alloc error !!!\n" );
		return 1;
	}
	

//	newlight->nextlight = g_lighthead;
//	g_lighthead = newlight;


	//最後に追加する。
	CLightData* curl = g_lighthead;
	CLightData* befl = 0;

	while( curl ){
		befl = curl;
		curl = curl->nextlight;
	}

	if( befl ){
		befl->nextlight = newlight;
	}else{
		_ASSERT( !g_lighthead );
		g_lighthead = newlight;
	}

	*lidptr = lid;

	return 0;
}
EASY3D_API int E3DSetDirectionalLight( int lid, D3DXVECTOR3 dir, E3DCOLOR4UC col )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetDirectionalLight : lightID not found error !!!\n" );
		return 1;
	}
	
	DXVec3Normalize( &dir, &dir );

	CVec3f difcol;
	difcol.x = (float)col.r / 255.0f;
	difcol.y = (float)col.g / 255.0f;
	difcol.z = (float)col.b / 255.0f;

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


EASY3D_API int E3DSetPointLight( int lid, D3DXVECTOR3 pos, float dist, E3DCOLOR4UC col )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	CLightData* curlight;
	curlight = GetLightData( lid );
	if( !curlight ){
		DbgOut( "E3DSetPointLight : lightID not found error !!!\n" );
		return 1;
	}
	
	CVec3f difcol;
	difcol.x = (float)col.r / 255.0f;
	difcol.y = (float)col.g / 255.0f;
	difcol.z = (float)col.b / 255.0f;

	int ret;
	ret = curlight->SetPointLight( pos, dist, difcol );
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

	light.Range = dist;

	light.Attenuation0 = 0.4f;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );


//DbgOut( "e3dhsp : SetPointLight : pointlight : col %f %f %f, pos %f %f %f, range %f\n",
//	   light.Diffuse.r, light.Diffuse.g, light.Diffuse.b,
//	   light.Position.x, light.Position.y, light.Position.z,
//	   light.Range );


	return 0;
}

EASY3D_API int E3DDestroyLight( int lid )
{	
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

EASY3D_API int E3DClearZ()
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

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
                         0,
						 1.0f, 0L );
   if( hres != D3D_OK ){
		//_ASSERT( 0 );
	   DbgOut( "E3DClearZ : Clear error !!!\n" );
		return 1;
   }

	return 0;
}

EASY3D_API int E3DDestroyHandlerSet( int hsid )
{
	int ret;
	ret = DestroyHandlerSet( hsid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyHandlerSet : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetMotionFrameNo( int hsid, int motid, int srcframeno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMotionFrameNo( motid, srcframeno );
	if( ret ){
		DbgOut( "E3DSetMotionFrameNo : SetCurrentMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCreateSprite( const char* tempname, int transparentflag, E3DCOLOR4UC* tpcolptr, int* spriteidptr )
{
	int ret;
////////
	char fname[MAX_PATH];
	if( tempname ){
		strcpy_s( fname, MAX_PATH, tempname );
	}else{
		DbgOut( "E3DCreateSprite : file name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int spriteid;

	CMySprite* newsprite;
	newsprite = new CMySprite( &spriteid );
	if( !newsprite ){
		DbgOut( "E3DCreateSprite : newsprite alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newsprite->next = s_spritehead;
	s_spritehead = newsprite;


	int tpr, tpg, tpb;
	if( tpcolptr ){
		tpr = tpcolptr->r;
		tpg = tpcolptr->g;
		tpb = tpcolptr->b;
	}else{
		tpr = 0;
		tpg = 0;
		tpb = 0;
	}

	ret = newsprite->SetSpriteParams( fname, transparentflag, tpr, tpg, tpb, g_miplevels, g_mipfilter );
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

	*spriteidptr = spriteid;


	return 0;
}
EASY3D_API int E3DBeginSprite()
{

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	if( !s_d3dxsprite ){
		DbgOut( "E3DBeginSprite : d3dxsprite error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	HRESULT hr;
	//hr = s_d3dxsprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT );
	hr = s_d3dxsprite->Begin( D3DXSPRITE_ALPHABLEND );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}
	
	DisableFogState();

	return 0;
}
EASY3D_API int E3DEndSprite()
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


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

	RollbackFogState();

	return 0;
}
EASY3D_API int E3DRenderSprite( int spid, float scalex, float scaley, D3DXVECTOR3 tra )
{

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


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
	scale.x = scalex; //!!!!!!!
	scale.y = scaley; //!!!!!!!

	ret = cursprite->Render( pdev, s_d3dxsprite, scale, tra );
	if( ret ){
		DbgOut( "E3DRenderSprite : Render error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DGetSpriteSize( int spid, int* widthptr, int* heightptr )
{
	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DGetSpriteSize : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*widthptr = cursprite->texWidth;
	*heightptr = cursprite->texHeight;

	return 0;
}

EASY3D_API int E3DSetSpriteRotation( int spid, D3DXVECTOR2 center, float rotdeg )
{
	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteRotation : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursprite->SetRotation( center, rotdeg );
	if( ret ){
		DbgOut( "E3DSetSpriteRotation : sprite SetRotation error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetSpriteARGB( int spid, E3DCOLOR4UC col )
{
	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteARGB : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	ret = cursprite->SetARGB( col.a, col.r, col.g, col.b );
	if( ret ){
		DbgOut( "E3DSetSpriteARGB : sprite SetARGB error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetSpriteUV( int spid, float startu, float endu, float startv, float endv )// $1B8
{
	CMySprite* cursprite;
	cursprite = GetSprite( spid );
	if( !cursprite ){
		DbgOut( "E3DSetSpriteUV : sprite id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = cursprite->SetSpriteUV( startu, endu, startv, endv );
	if( ret ){
		DbgOut( "E3DSetSpriteUV : sprite SetSpriteUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDestroySprite( int spid )
{
	int ret;
	int dirtyreset = 0;
	ret = DestroyTexture( -2, -2, spid, dirtyreset, -2 );
	_ASSERT( !ret );
///////

//	if( g_texbnk ){
//		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
//	}

////////

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
		_ASSERT( 0 );
		return 0;
	}

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}

	return 0;
}

EASY3D_API int E3DCreateProgressBar()
{
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

EASY3D_API int E3DSetProgressBar( int newpos )
{
	if( newpos < 0 )
		newpos = 0;
	if( newpos > 100 )
		newpos = 100;

//////////


	if( !s_PBwnd ){
		DbgOut( "E3DSetProgressBar : Progress bar still not exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SendMessage( s_PBwnd, PBM_SETPOS, newpos, 0 );


	return 0;
}

EASY3D_API int E3DDestroyProgressBar()
{

	if( s_PBwnd ){
		DestroyWindow( s_PBwnd );
		s_PBwnd = 0;
	}

	return 0;
}

EASY3D_API int E3DLoadGroundBMP( const char* tempname1, const char* tempname2, const char* tempname3, const char* tempname4, 
	float maxx, float maxz, int divx, int divz, float maxheight, int* hsidptr )
{
	int ret;
	if( divx < 2 )
		divx = 2;

	if( divz < 2 )
		divz = 2;

	if( maxheight < 0.0f )
		maxheight = 0.0f;

	int TLmode = 2;
	
	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadGroundBMP : filename1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname2[MAX_PATH];
	if( tempname2 ){
		strcpy_s( fname2, MAX_PATH, tempname2 );
	}else{
		DbgOut( "E3DLoadGroundBMP : filename2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname3[MAX_PATH];
	if( tempname3 ){
		strcpy_s( fname3, MAX_PATH, tempname3 );
	}else{
		DbgOut( "E3DLoadGroundBMP : filename3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname4[MAX_PATH];
	if( tempname4 ){
		strcpy_s( fname4, MAX_PATH, tempname4 );
	}else{
		DbgOut( "E3DLoadGroundBMP : filename4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//////


	int temphsid = -1;
	ret = LoadGroundBMP_F( fname1, fname2, fname3, fname4, maxx, maxz, divx, divz, maxheight, TLmode, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadGroundBMP : LoadGroundBMP_F error !!!\n" );
		_ASSERT( 0 );

		return 1;
	}

	*hsidptr = temphsid;	
	
	return 0;
}

int LoadGroundBMP_F( char* tempname1, char* tempname2, char* tempname3, char* tempname4, float maxx, float maxz, int divx, int divz, float maxheight, int TLmode, int* hsidptr )
{

	int ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

	newhs->m_TLmode = TLmode;//!!!!!!! ver1036
	newhs->m_groundflag = 1;//!!!!!!!! ver1036

	ret = newhs->LoadGroundBMP( tempname1, tempname2, tempname3, tempname4, maxx, maxz, divx, divz, maxheight );
	if( ret ){
		DbgOut( "easy3d : E3DCreateHandler : LoadGroundBMP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );

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

	*hsidptr = newhs->serialno;

	return 0;
}

EASY3D_API int E3DLoadMQOFileAsGround( const char* tempname1, float mult, int adjustuvflag, int bonetype, int* hsidptr )
{

//////	
	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadMQOFileAsGround : filename1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsGround_F( fname1, mult, s_hwnd, adjustuvflag, &temphsid, bonetype );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : LoadMQOFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = temphsid;

	return 0;

}

int LoadMQOFileAsGround_F( char* tempname1, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype )
{
	int ret = 0;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

	newhs->m_TLmode = 2;//!!!!!!!!!!!! ver1036
	newhs->m_groundflag = 1;//!!!!!!!! ver1036

	ret = newhs->LoadMQOFileAsGround( tempname1, mult, hwnd, adjustuvflag, hsidptr, bonetype );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : curhs LoadMQOFileAsGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


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
	*hsidptr = newhs->serialno;
	return 0;

}

EASY3D_API int E3DLoadSigFileAsGround( const char* tempname, float mult, int adjustuvflag, int* hsidptr )
{
	char fname[MAX_PATH];
	if( tempname ){
		strcpy_s( fname, MAX_PATH, tempname );
	}else{
		DbgOut( "E3DLoadSigFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int temphsid = -1;
	ret = LoadSigFileAsGround_F( fname, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGround : LoadSigFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = temphsid;

	return 0;
}

int LoadSigFileAsGround_F( char* tempname, int adjustuvflag, float mult, int* hsidptr )
{
	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!

	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!

	ret = newhs->LoadSigFile( tempname, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : LoadSigFileAsGround_F : LoadSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : LoadSigFileAsGround_F : ChainNewHandlerSet error !!!\n" );
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
	*hsidptr = newhs->serialno;

	return 0;
}

EASY3D_API int E3DCameraPosForward( float step )
{
	
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

EASY3D_API int E3DSetBeforePos( int hsid )
{
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

EASY3D_API int E3DGetPartNoByName( int hsid, const char* tempname, int* partnoptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPartNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int partno = -1;
	ret = curhs->GetPartNoByName( (char*)tempname, &partno );
	if( ret ){
		DbgOut( "E3DGetPartNoByName : curhs GetPartNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*partnoptr = partno;

	return 0;
}

EASY3D_API int E3DGetVertNumOfPart( int hsid, int partno, int* vertnumptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertNumOfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int vertnum = 0;
	ret = curhs->GetVertNumOfPart( partno, &vertnum );
	if( ret ){
		DbgOut( "E3DGetVertNumOfPart : curhs GetVertNumOfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*vertnumptr = vertnum;

	return 0;
}

EASY3D_API int E3DGetVertPos( int hsid, int partno, int vertno, int kind, D3DXVECTOR3* posptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float vx, vy, vz;
	ret = curhs->GetVertPos( kind, partno, vertno, &vx, &vy, &vz );
	if( ret ){
		DbgOut( "E3DGetVertPos : curhs GetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	posptr->x = vx;
	posptr->y = vy;
	posptr->z = vz;

	return 0;
}

EASY3D_API int E3DSetVertPos( int hsid, int partno, int vertno, D3DXVECTOR3 pos )
{

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetVertPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetVertPos( partno, vertno, pos.x, pos.y, pos.z );
	if( ret ){
		DbgOut( "E3DSetVertPos : curhs SetVertPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DSetVertPosBatch( int hsid, int partno, int* vertnoarray, int vertnum, D3DXVECTOR3* posptr )// $1C5
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetVertPosBatch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetVertPosBatchVec3( partno, vertnoarray, vertnum, posptr );
	if( ret ){
		DbgOut( "E3DSetVertPosBatch : hs SetVertPosBatch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

EASY3D_API int E3DSetPosOnGround( int hsid, int groundid, float mapmaxy, float mapminy, float posx, float posz )
{
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

EASY3D_API int E3DSetPosOnGroundPart( int hsid, int groundid, int groundpart, float mapmaxy, float mapminy, float posx, float posz )
{
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

EASY3D_API int E3DCreateBillboard( const char* tempname1, float width, float height, int transparentflag, int dirmode, int orgflag, int* bbidptr )
{
	int ret;

	if( !g_bbhs ){
		DbgOut( "E3DCreateBillboard : g_bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DCreateBillboard : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	SetResDir( fname1, g_bbhs );//!!!!!!


	int bbid = -1;
	ret = g_bbhs->AddBillboard( fname1, width, height, transparentflag, dirmode, orgflag, &bbid );
	if( ret || (bbid < 0) ){
		DbgOut( "E3DCreateBillboard : g_bbhs AddBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( bbid >= 0 ){
	//Create 3D Object
		ret = Restore( g_bbhs, bbid );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}


	*bbidptr = bbid;

	return 0;
}

EASY3D_API int E3DRenderBillboard( int scid, int transskip )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRenderBillboard : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

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


		CalcMatView();

		int ret;

		g_bbhs->m_shandler->m_bbtransskip = transskip;//!!!!!!!!!!!!!

		if( transskip == 0 ){
			ret = g_bbhs->ChkInView( s_matView, 0, 0 );
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
		}
//Render		
		//ret = g_bbhs->Render( pdev, 0 );
		ret = g_bbhs->Render( 0, pdev, 1, s_matView, s_camerapos );
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

EASY3D_API int E3DSetBillboardPos( int bbid, D3DXVECTOR3 pos )
{

	int ret;
	if( g_bbhs ){

		ret = g_bbhs->SetBillboardPos( bbid, pos.x, pos.y, pos.z );
		if( ret ){
			DbgOut( "E3DSetBillboardPos : bbhs SetBillboardPos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		DbgOut( "E3DSetBillboardPos : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetBillboardOnGround( int bbid, int groundid, float mapmaxy, float mapminy, float posx, float posz )
{
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

EASY3D_API int E3DDestroyBillboard( int bbid )
{

	int ret;
	int dirtyreset = 0;
	ret = DestroyTexture( -2, bbid, -2, dirtyreset, -2 );
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

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}

	return 0;
}

EASY3D_API int E3DGetBillboardInfo( BBINFO* bbinfo )
{


////////
////////
	int ret;

//	float posx, posy, posz, width, height;
//	char texname[MAX_PATH];
//	int transparent;

	if( g_bbhs ){
		ret = g_bbhs->GetBillboardInfo( bbinfo->bbid, &(bbinfo->pos.x), &(bbinfo->pos.y), &(bbinfo->pos.z), 
			bbinfo->texname, &(bbinfo->transparent), &(bbinfo->width), &(bbinfo->height) );
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

EASY3D_API int E3DGetNearBillboard( D3DXVECTOR3 pos, float maxdist, int* bbidptr )
{
	int ret;
	int bbid = -1;
	if( g_bbhs ){
		ret = g_bbhs->GetNearBillboard( pos.x, pos.y, pos.z, maxdist, &bbid );
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

	*bbidptr = bbid;

	return 0;
}

EASY3D_API int E3DRotateBillboard( int bbid, float rotdeg, int rotkind )
{
	int ret;
	if( !g_bbhs ){
		DbgOut( "E3DRotateBillboard : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_bbhs->RotateBillboard( bbid, rotdeg, rotkind );
	if( ret ){
		DbgOut( "E3DRotateBillboard : bbhs RotateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag )
{
	if( unum <= 0 )
		unum = 1;
	if( vnum <= 0 )
		vnum = 1;

	int ret;
	if( !g_bbhs ){
		DbgOut( "E3DSetBillboardUV : bbhs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_bbhs->SetBillboardUV( bbid, unum, vnum, texno, revuflag );
	if( ret ){
		DbgOut( "E3DSetBillboardUV : bbhs SetBillboardUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DDestroyAllBillboard()
{

	int ret;
	int dirtyreset = 0;
	ret = DestroyTexture( -2, -1, -2, dirtyreset, -2 );
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

	if( g_texbnk ){
		g_texbnk->ResetDirtyFlag();//!!!!!!!!!!!
	}


	return 0;
}

EASY3D_API int E3DSaveMQOFile( int hsid, const char* tempname1 )
{
	int ret;

	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSaveMQOFile : filename1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMQOFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SaveMQOFile( fname1, s_hwnd );
	if( ret ){
		DbgOut( "E3DSaveMQOFile : curhs SaveMQOFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetInvisibleFlag( int hsid, int partno, int* flagptr )
{

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
	
	int flag = 0;
	ret = curhs->GetInvisibleFlag( partno, &flag );
	if( ret ){
		DbgOut( "E3DGetInvisibleFlag : curhs GetInvisibleFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*flagptr = flag;

	return 0;
}

EASY3D_API int E3DSetInvisibleFlag( int hsid, int partno, int srcflag )
{
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

EASY3D_API int E3DSetMovableArea( const char* tempname1, float maxx, float maxz, int divx, int divz, float wallheight, int* hsidptr )
{
	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSetMovableArea : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int temphsid = -1;
	ret = SetMovableArea_F( fname1, maxx, maxz, divx, divz, wallheight, &temphsid );
	if( ret ){
		DbgOut( "E3DSetMovableArea : SetMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = temphsid;

	return 0;
}

int SetMovableArea_F( char* tempname1, float maxx, float maxz, int divx, int divz, float wallheight, int* hsidptr )
{
	int ret;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}



	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

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

	*hsidptr = newhs->serialno;//!!!!!!!!!!!

	return 0;
}

EASY3D_API int E3DVec3Normalize( D3DXVECTOR3 srcvec, D3DXVECTOR3* dstvec )
{

	DXVec3Normalize( dstvec, &srcvec );

	return 0;
}

EASY3D_API int E3DVec2CCW( D3DXVECTOR2 vec1, D3DXVECTOR2 vec2, int* ccwptr )
{

	float ccw;
	ccw = D3DXVec2CCW( &vec1, &vec2 );
	
	int iccw;
	if( ccw >= 0.0f )
		iccw = 1;
	else
		iccw = -1;

	*ccwptr = iccw;

	return 0;
}

EASY3D_API int E3DVec3RotateY( D3DXVECTOR3 srcvec, float deg, D3DXVECTOR3* dstvec )
{

	int ret;
	CQuaternion2 dirq;
	D3DXMATRIX	dirm;


	ret = dirq.SetRotation( 0, deg, 0 );
	_ASSERT( !ret );
	dirm = dirq.MakeRotMatX();


	float vecx = srcvec.x;
	float vecy = srcvec.y;
	float vecz = srcvec.z;

	dstvec->x = dirm._11 * vecx + dirm._21 * vecy + dirm._31 * vecz + dirm._41;
	dstvec->y = dirm._12 * vecx + dirm._22 * vecy + dirm._32 * vecz + dirm._42;
	dstvec->z = dirm._13 * vecx + dirm._23 * vecy + dirm._33 * vecz + dirm._43;
	//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
	

	return 0;
}

EASY3D_API int E3DLoadMQOFileAsMovableArea( const char* tempname1, float mult, int* hsidptr )
{
	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableArea : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableArea_F( fname1, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : LoadMQOFileAsMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = temphsid;

	return 0;
}

int LoadMQOFileAsMovableArea_F( char* tempname1, float mult, int* hsidptr )
{
	*hsidptr = -1;

	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

	
	ret = newhs->LoadMQOFileAsMovableArea( tempname1, mult, s_hwnd, hsidptr );
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

	*hsidptr = newhs->serialno;

	return 0;
}

EASY3D_API int E3DLoadSound( const char* tempname1, int use3dflag, int reverbflag, int bufnum, int* idptr )
{
	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadSound : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !s_HS ){
		DbgOut( "E3DLoadSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// 読み込み
	int soundid = -1;
	int ret;
	ret = s_HS->LoadSoundFile( fname1, use3dflag, reverbflag, bufnum, &soundid );
	if( ret ){
		DbgOut( "E3DLoadSound : hs LoadSoundFile error %s !!!\n", fname1 );
		_ASSERT( 0 );
		return 1;
	}

	DbgOut( "E3DLoadSound : success : %s\n", fname1 );

	*idptr = soundid;

	return 0;
}

EASY3D_API int E3DPlaySound( int soundid, int startsample, int openo, int numloops )
{
	if( !s_HS ){
		DbgOut( "E3DPlaySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->PlaySound( soundid, startsample, openo, numloops );
	if( ret ){
		DbgOut( "E3DPlaySound : hs PlaySound error %d!!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EASY3D_API int E3DStopSound( int soundid )
{
	if( !s_HS ){
		DbgOut( "E3DStopSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->StopSound( soundid );
	if( ret ){
		DbgOut( "E3DStopSound : hs StopSound error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetSoundLoop( int soundid, int loopflag )
{
/***
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
***/
	return 0;
}

EASY3D_API int E3DSetSoundVolume( float volume, int soundid )
{
//	int soundid;
//	soundid = code_getdi(-1);//!!!!! default は　-1 

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

EASY3D_API int E3DSetSoundTempo( float tempo )
{
/***
	if( !s_HS ){
		DbgOut( "E3DSetSoundTempo : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = s_HS->SetMasterTempo( tempo );
	if( ret ){
		DbgOut( "E3DSetSoundTempo : hs SetMasterTempo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/
	return 0;
}

EASY3D_API int E3DDestroySound( int soundid )
{
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

EASY3D_API int E3DGetSoundVolume( int soundid, float* volptr )
{
	if( !s_HS ){
		DbgOut( "E3DGetSoundVolume : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	float vol = 0.0f;
	ret = s_HS->GetVolume( soundid, &vol );
	if( ret ){
		DbgOut( "E3DGetSoundVolume : hs GetVolume error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}
	
	*volptr = vol;

	return 0;
}


EASY3D_API int E3DSet3DSoundListener( float doppler, float rolloff )
{
/***
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundListener : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->Set3DListener( doppler, rolloff );
	if( ret ){
		DbgOut( "E3DSet3DSoundListener : hs Set3DListener error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/
	return 0;
}

EASY3D_API int E3DSet3DSoundListenerMovement( int hsid )
{

	if( !s_HS ){
		DbgOut( "E3DPlaySound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	static int s_isfirst = 1;
	static D3DXVECTOR3 s_befpos( 0.0f, 0.0f, 0.0f );


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
		
		pos = curhs->m_gpd.m_gpe.e3dpos;
		if( s_isfirst == 0 ){
			vel = pos - s_befpos;
		}else{
			vel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}

		curhs->m_gpd.m_q.Rotate( &aftdir, dir );
		curhs->m_gpd.m_q.Rotate( &aftupdir, updir );

		
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
		
		if( s_isfirst == 0 ){
			vel = pos - s_befpos;
		}else{
			vel = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}

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

EASY3D_API int E3DSet3DSoundDistance( int soundid, float min, float max )
{
/***
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundDistance : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->Set3DDistance( soundid, min, max );
	if( ret ){
		DbgOut( "E3DSet3DSoundDistance : hs Set3DDistance error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

***/
	return 0;
}

EASY3D_API int E3DSet3DSoundMovement( int soundid, D3DXVECTOR3 pos, D3DXVECTOR3 vel )
{

	if( !s_HS ){
		DbgOut( "E3DSet3DSoundMovement : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->Set3DMovement( soundid, &pos, &vel );
	if( ret ){
		DbgOut( "E3DSet3DSoundMovement : hs Set3DMovement error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DCreateNaviLine( int* nlidptr )
{	
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

EASY3D_API int E3DDestroyNaviLine( int nlid )
{
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

EASY3D_API int E3DAddNaviPoint( int nlid, int befnpid, int* npidptr )
{

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DAddNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int npid = -1;
	ret = curnl->AddNaviPoint( befnpid, 1, &npid );
	if( ret ){
		DbgOut( "E3DAddNaviPoint : curnl AddNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*npidptr = npid;

	return 0;
}

EASY3D_API int E3DRemoveNaviPoint( int nlid, int npid )
{
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

EASY3D_API int E3DGetNaviPointPos( int nlid, int npid, D3DXVECTOR3* posptr )
{

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

	*posptr = dstpos;
	return 0;
}

EASY3D_API int E3DSetNaviPointPos( int nlid, int npid, D3DXVECTOR3 pos )
{
	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DSetNaviPointPos : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curnl->SetNaviPointPos( npid, &pos );
	if( ret ){
		DbgOut( "E3DSetNaviPointPos : curnl SetNaviPointPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetNaviPointOwnerID( int nlid, int npid, int* idptr )
{

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNaviPointOwnerID : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int oid = -1;
	ret = curnl->GetNaviPointOwnerID( npid, &oid );
	if( ret ){
		DbgOut( "E3DGetNaviPointOwnerID : curnl GetNaviPointOwnerID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*idptr = oid;

	return 0;
}

EASY3D_API int E3DSetNaviPointOwnerID( int nlid, int npid, int oid )
{
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

EASY3D_API int E3DGetNextNaviPoint( int nlid, int npid, int* nextidptr )
{
//pointid のnext チェインのpointidを返す。pointid == -1 のとき、先頭pointを返すnextid == -1のときは、nextなし、

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetNextNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int next = -1;
	ret = curnl->GetNextNaviPoint( npid, &next );
	if( ret ){
		DbgOut( "E3DGetNextNaviPoint : curnl GetNextNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*nextidptr = next;
	return 0;
}

EASY3D_API int E3DGetPrevNaviPoint( int nlid, int npid, int* previdptr )
{
// previd == -1のときは、prevなし
	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetPrevNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int prev = -1;
	ret = curnl->GetPrevNaviPoint( npid, &prev );
	if( ret ){
		DbgOut( "E3DGetPrevNaviPoint : curnl GetPrevNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*previdptr = prev;

	return 0;
}

EASY3D_API int E3DGetNearestNaviPoint( int nlid, D3DXVECTOR3 pos, int* nearidptr, int* previdptr, int* nextidptr )
{
// 一番近いNaviPointと、その前後のNaviPointのpointidを取得する

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DGetPrevNaviPoint : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int nearid, prev, next;

	ret = curnl->GetNearestNaviPoint( &pos, &nearid, &prev, &next );
	if( ret ){
		DbgOut( "E3DGetNearestNaviPoint : curnl GetNearestNaviPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*nearidptr = nearid;
	*previdptr = prev;
	*nextidptr = next;

	return 0;
}


EASY3D_API int E3DFillUpNaviLine( int nlid, int div, int flag )
{
// 3つ以上点がないときは、何もしない。pointid を付け替えるかどうかのフラグも？？

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

EASY3D_API int E3DSetNaviLineOnGround( int nlid, int groundid, float mapmaxy, float mapminy )
{
//全てのnavipointを地面の高さにセットする。
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

	ret = groundhs->SetNaviLineOnGround( curnl, mapmaxy, mapminy );
	if( ret ){
		DbgOut( "E3DSetNaviLineOnGround : groundhs SetNaviLineOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DControlByNaviLine( int hsid, int nlid, int ctrlmode, int roundflag, int reverseflag,
	float maxdist, float posstep, float dirstep, D3DXVECTOR3* newposptr, D3DXQUATERNION* newqptr, int* tpidptr )
{
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

	D3DXVECTOR3 offset( 0.0f, 0.0f, 0.0f );

	D3DXVECTOR3 newpos;
	CQuaternion newq;


	CQuaternion tempdirq;
	tempdirq = curhs->m_gpd.m_q;

	int tpid;
	tpid = *tpidptr;

	ret = curnl->ControlByNaviLine( curhs->m_gpd.m_gpe.e3dpos, offset, tempdirq, 
			ctrlmode, roundflag, reverseflag,
			maxdist, 
			posstep, dirstep,
			&newpos, &newq, &tpid );
	if( ret ){
		DbgOut( "E3DControlByNaviLine : curnl ControlByNaviLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*tpidptr = tpid;
	
	*newposptr = newpos;

	newqptr->x = newq.x;
	newqptr->y = newq.y;
	newqptr->z = newq.z;
	newqptr->w = newq.w;

	return 0;
}

EASY3D_API int E3DSetDirQ( int hsid, D3DXQUATERNION q, D3DXQUATERNION* befqptr )
{
//	クォータニオンによる、方向のセット
//	正規化もする。

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDirQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;


	CQuaternion newq;
	newq.SetParams( q.w, q.x, q.y, q.z );

	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	_ASSERT( !ret );

	if( befqptr == 0 ){
		ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.e3drot );
		_ASSERT( !ret );
	}else{
		D3DXVECTOR3 befeul;
		CQuaternion befq( befqptr->w, befqptr->x, befqptr->y, befqptr->z );
		ret = qToEuler( 0, &befq, &befeul );
		_ASSERT( !ret );
		ret = modifyEuler( &neweul, &befeul );
		_ASSERT( !ret );
	}
	curhs->m_gpd.m_gpe.e3drot = neweul;

	curhs->m_gpd.CalcMatWorld();


	return 0;
}

EASY3D_API int E3DGetDirQ( int hsid, D3DXQUATERNION* qptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDirQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	qptr->x = curhs->m_gpd.m_q.x;
	qptr->y = curhs->m_gpd.m_q.y;
	qptr->z = curhs->m_gpd.m_q.z;
	qptr->w = curhs->m_gpd.m_q.w;

	return 0;
}

EASY3D_API int E3DSetWallOnGround( int whsid, int ghsid, float mapmaxy, float mapminy, float wheight )
{
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
	ret = ghs->SetWallOnGround( whs, mapmaxy, mapminy, wheight );
	if( ret ){
		DbgOut( "E3DSetWallOnGround : ghs SetWallOnGround error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


// Invalidate
	int resetflag = 1;
	ret = whs->Invalidate( resetflag );
	if( ret ){
		DbgOut( "E3DSetWallOnGround : whs Invalidate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	ret = Restore( whs, -1 );
	if( ret ){
		DbgOut( "E3DSetWallOnGround : whs Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCreateNaviPointClearFlag( int hsid, int nlid, int roundnum )
{
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

EASY3D_API int E3DDestroyNaviPointClearFlag( int hsid )
{
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

EASY3D_API int E3DInitNaviPointClearFlag( int hsid )
{
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

EASY3D_API int E3DSetNaviPointClearFlag( int hsid, D3DXVECTOR3 pos, float maxdist, int* npidptr, int* roundptr, float* distptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNaviPointClearFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;	
	int npid, round;
	float dist;

	ret = curhs->SetNaviPointClearFlag( pos, maxdist, &npid, &round, &dist );
	if( ret ){
		DbgOut( "E3DSetNaviPointClearFlag : curhs SetNaviPointClearFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*npidptr = npid;
	*roundptr = round;
	*distptr = dist;

	return 0;
}
		
EASY3D_API int E3DGetOrder( int* hsidarray, int arrayleng, int* orderarray, int* cleararray )
{
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
		int curhsid;
		curhsid = *( hsidarray + hsno );//!!!!!!!!!!!!!!!!!!

		hsarray[hsno] = GetHandlerSet( curhsid );
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
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		
		int curhsid;
		curhsid = npcdarray[hsno]->ownerhsid;

		*( orderarray + hsno ) = curhsid;
		
		int clearno;
		clearno = npcdarray[hsno]->pointnum * npcdarray[hsno]->lastround + npcdarray[hsno]->lastpointarno;

		*( cleararray + hsno ) = clearno;
	}

	return 0;

}

EASY3D_API int E3DSetValidFlag( int hsid, int partno, int flag )
{
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

EASY3D_API int E3DSetBillboardDiffuse( int bbid, E3DCOLOR4UC col, int setflag, int vertno )
{
	int hsid = -1;
	int updateflag = 1;

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

	int ret;
	ret = curhs->SetDispDiffuse( bbid, col.r, col.g, col.b, setflag, vertno, updateflag );
	if( ret ){
		DbgOut( "e3dhsp : SetDiffuse : curhs SetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

EASY3D_API int E3DSetBillboardSpecular( int bbid, E3DCOLOR4UC col, int setflag, int vertno )
{
	int hsid = -1;
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

	int ret;
	ret = curhs->SetDispSpecular( bbid, col.r, col.g, col.b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecular : curhs SetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;

}

EASY3D_API int E3DSetBillboardAmbient( int bbid, E3DCOLOR4UC col, int setflag, int vertno )
{
	int hsid = -1;
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

	int ret;
	ret = curhs->SetDispAmbient( bbid, col.r, col.g, col.b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetAmbient : curhs SetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EASY3D_API int E3DSetBillboardEmissive( int bbid, E3DCOLOR4UC col, int setflag, int vertno )
{
	int hsid = -1;

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

	int ret;
	ret = curhs->SetDispEmissive( bbid, col.r, col.g, col.b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetEmissive : curhs SetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EASY3D_API int E3DSetBillboardSpecularPower( int bbid, float power, int setflag, int vertno )
{
	int hsid = -1;	
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

	if( power < 0.0f )
		power = 0.0f;
	if( power > 100.0f )
		power = 100.0f;


	int ret;
	ret = curhs->SetDispSpecularPower( bbid, power, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecularPower : curhs SetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}
EASY3D_API int E3DSetBillboardAlpha( int bbid, float alphaval)
{
	int hsid;
	hsid = -1;

	int updateflag;
	updateflag = 1;


/////////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "E3DSetBillboardAlpha : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : SetBillboardAlpha : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( alphaval < 0.0f )
		alphaval = 0.0f;
	if( alphaval > 1.0f )
		alphaval = 1.0f;

	
	int ret = 0;
	ret = curhs->SetAlpha( alphaval, bbid, updateflag );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

EASY3D_API int E3DGetBillboardDiffuse( int bbid, int vertno, E3DCOLOR4UC* colptr )
{
	int hsid = -1;
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
	int r, g, b;
	ret = curhs->GetDiffuse( bbid, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetDiffuse : curhs GetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}

EASY3D_API int E3DGetBillboardAmbient( int bbid, int vertno, E3DCOLOR4UC* colptr )
{
	int hsid = -1;

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
	int r, g, b;
	ret = curhs->GetAmbient( bbid, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetAmbient : curhs GetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}

EASY3D_API int E3DGetBillboardSpecular( int bbid, int vertno, E3DCOLOR4UC* colptr )
{
	int hsid = -1;

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
	int r, g, b;
	ret = curhs->GetSpecular( bbid, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetSpecular : curhs GetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}

EASY3D_API int E3DGetBillboardEmissive( int bbid, int vertno, E3DCOLOR4UC* colptr )
{

	int hsid = -1;
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
	int r, g, b;
	ret = curhs->GetEmissive( bbid, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetEmissive : curhs GetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}


EASY3D_API int E3DGetBillboardSpecularPower( int bbid, int vertno, float* powerptr )
{
	int hsid = -1;

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
	ret = curhs->GetSpecularPower( bbid, vertno, &fpow );
	if( ret ){
		DbgOut( "e3dhsp : GetSpecularPower : curhs GetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*powerptr = fpow;

	return 0;
}

EASY3D_API int E3DGetBillboardAlpha( int bbid, int vertno, float* alphaptr )
{
	int hsid = -1;

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
	float a;
	ret = curhs->GetAlphaF( bbid, vertno, &a );
	if( ret ){
		DbgOut( "e3dhsp : GetAlpha : curhs GetAlphaF error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*alphaptr = a;

	return 0;
}

EASY3D_API int E3DSetBlendingMode( int hsid, int partno, int bmode )
{
	//!!!!!!! ビルボードとスプライト用

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


EASY3D_API int E3DSetRenderState( int hsid, int partno, int statetype, int value )
{
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

EASY3D_API int E3DSetScale( int hsid, int partno, D3DXVECTOR3 scale, int centerflag )
{
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

	int ret;
	ret = curhs->SetScale( partno, scale, centerflag );
	if( ret ){
		DbgOut( "e3dhsp : SetScale : curhs SetScale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EASY3D_API int E3DGetScreenPos3F( int scid, int hsid, int partno, int vertno, D3DXVECTOR3* scposptr )
{
	if( scid < 0 ){
		DbgOut( "E3DGetScreenPos3F : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int calcmode = 1;
///////

	CHandlerSet* curhs = 0;

	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : GetScreenPos3F : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : GetScreenPos3F : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	//!!!!!! モデル全体の２ｄ座標取得時は、vertnoに-1 !!!!!!!
	if( partno < 0 ){
		vertno = -1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DGetScreenPos3F : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	CalcMatView();

	ret = curhs->GetScreenPos3F( partno, vertno, calcmode, s_matView, scposptr );
	if( ret ){
		DbgOut( "e3dhsp : GetScreenPos3F : curhs GetScreenPos3F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetScreenPos3FVec( int scid, D3DXVECTOR3 pos3d, D3DXVECTOR3* scposptr )
{
	if( scid < 0 ){
		DbgOut( "E3DGetScreenPos2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DGetScreenPos2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();

	int ret;
	ret = GetScreenPos( pos3d, scposptr );
	if( ret ){
		DbgOut( "E3DGetScreenPos3FVec : GetScreenPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DConvScreenTo3D( int scid, D3DXVECTOR3 scpos, D3DXVECTOR3* pos3dptr )
{
	if( scid < 0 ){
		DbgOut( "E3DConvScreenTo3D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DConvScreenTo3D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ScreenToObj( pos3dptr, scpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DConvScreenTo3D ; ScreenToObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}


EASY3D_API int E3DCreateQ( int* qidptr )
{
	if( !s_qh ){
		DbgOut( "E3DCreateQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int qid = -1;
	ret = s_qh->CreateQ( &qid );
	if( ret ){
		DbgOut( "E3DCreateQ : qh CreateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*qidptr = qid;

	return 0;	
}

EASY3D_API int E3DDestroyQ( int qid )
{

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

EASY3D_API int E3DInitQ( int qid )
{
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

EASY3D_API int E3DSetQAxisAndDeg( int qid, D3DXVECTOR3 axis, float deg )
{
	if( !s_qh ){
		DbgOut( "E3DSetQAxisAndDeg : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_qh->SetQAxisAndDeg( qid, axis, deg );
	if( ret ){
		DbgOut( "E3DSetQAxisAndDeg : qh SetQAxisAndDeg error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetQAxisAndDeg( int qid, D3DXVECTOR3* axisptr, float* degptr )
{
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

	*axisptr = axisvec;
	*degptr = deg;

	return 0;
}


EASY3D_API int E3DRotateQX( int qid, float degx )
{
	if( !s_qh ){
		DbgOut( "E3DRotateQX : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;

	axisvec.x = 1.0f;
	axisvec.y = 0.0f;
	axisvec.z = 0.0f;


	int ret;

	ret = s_qh->RotateQ( qid, axisvec, degx );
	if( ret ){
		DbgOut( "E3DRotateQX : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DRotateQY( int qid, float degy )
{
	if( !s_qh ){
		DbgOut( "E3DRotateQY : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;

	axisvec.x = 0.0f;
	axisvec.y = 1.0f;
	axisvec.z = 0.0f;


	int ret;

	ret = s_qh->RotateQ( qid, axisvec, degy );
	if( ret ){
		DbgOut( "E3DRotateQY : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DRotateQZ( int qid, float degz )
{
	if( !s_qh ){
		DbgOut( "E3DRotateQZ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 axisvec;

	axisvec.x = 0.0f;
	axisvec.y = 0.0f;
	axisvec.z = 1.0f;


	int ret;

	ret = s_qh->RotateQ( qid, axisvec, degz );
	if( ret ){
		DbgOut( "E3DRotateQZ : qh RotateQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DMultQ( int resqid, int befqid, int aftqid )
{
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

EASY3D_API int E3DNormalizeQ( int qid )
{
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

EASY3D_API int E3DCopyQ( int dstqid, int srcqid )
{
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

EASY3D_API int E3DQtoEuler( int qid, D3DXVECTOR3* eulptr, D3DXVECTOR3 befeul )
{
	if( !s_qh ){
		DbgOut( "E3DQtoEuler : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DQtoEuler : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion tmpq;
	tmpq.x = srcqptr->x;
	tmpq.y = srcqptr->y;
	tmpq.z = srcqptr->z;
	tmpq.w = srcqptr->w;

	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &tmpq, &neweul );
	if( ret ){
		DbgOut( "E3DQtoEuler : qToEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = modifyEuler( &neweul, &befeul );
	_ASSERT( !ret );

	*eulptr = neweul;

	return 0;
}

EASY3D_API int E3DGetBoneNoByName( int hsid, const char* tempname, int* noptr )
{
	//該当しない場合は　-1
	int ret;

	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : GetBoneNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int boneno = -1;
	ret = curhs->GetBoneNoByName( (char*)tempname, &boneno );
	if( ret ){
		DbgOut( "E3DGetBoneNoByName : curhs GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*noptr = boneno;

	return 0;
}

EASY3D_API int E3DGetNextMP( int hsid, int motid, int boneno, int prevmpid, int* mpidptr )
{
	//prevmpid == -1 の時は、最初のmp
	//prevmpid >= 0	で、そのmpが存在しない場合は、エラー。
	//無い場合は　-1

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : GetNextMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int mpid = -1;
	ret = curhs->GetNextMP( motid, boneno, prevmpid, &mpid );
	if( ret ){
		DbgOut( "e3dhsp : GetNextMP : curhs GetNextMP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*mpidptr = mpid;

	return 0;
}

EASY3D_API int E3DGetMPInfo( int hsid, int motid, int boneno, MPINFO2* mpiptr )
{
	//infoptr.0 : qid(呼び出し前に準備)

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

	ret = curhs->GetMPInfo( motid, boneno, mpiptr->mpid, &dstq, &dsttra, &dstframeno, &dstds, &dstinterp, &dstsc, &dstuserint1 );
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

	ret = s_qh->SetQ( mpiptr->qid, &setq );
	if( ret ){
		DbgOut( "E3DGetMPInfo : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	mpiptr->tra = dsttra;
	mpiptr->frameno = dstframeno;
	mpiptr->interpolation = dstinterp;
	mpiptr->scale = dstsc;
	mpiptr->userint1 = dstuserint1;
	
	return 0;
}

EASY3D_API int E3DSetMPInfo( int hsid, int motid, int boneno, MPINFO2* mpiptr, int* flagptr )
{
	// frameno変更時に既にframenoにモーションポイントがある場合にも、エラー

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetMPInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	if( !s_qh ){
		DbgOut( "E3DSetMPInfo : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_qh->GetQ( mpiptr->qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DSetMPInfo : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CQuaternion setq;
	setq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!

	ret = curhs->SetMPInfo( motid, boneno, &setq, mpiptr, flagptr );
	if( ret ){
		DbgOut( "e3dhsp : E3DSetMPInfo : curhs SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DExistMP( int hsid, int motid, int boneno, int frameno, int* mpidptr )
{
		//framenoに、既にモーションポイントがあれば、そのidを代入する。
		//無いときは、mpid に　-1を代入する

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DIsExistMP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int mpid = -1;
	ret = curhs->IsExistMP( motid, boneno, frameno, &mpid );
	if( ret ){
		DbgOut( "e3dhsp : E3DIsExistMP : curhs SetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*mpidptr= mpid;

	return 0;
}

EASY3D_API int E3DGetMotionFrameLength( int hsid, int motid, int* lengptr )
{
//フレーム番号の最大値は、leng - 1なので注意

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetMotionFrameLength : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int leng = 0;
	ret = curhs->GetMotionFrameLength( motid, &leng );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetMotionFrameLength : curhs GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*lengptr = leng;

	return 0;
}

EASY3D_API int E3DSetMotionFrameLength( int hsid, int motid, int frameleng )
{
//適切なタイミングで、E3DFillUpMP必要。
//フレーム数を縮小する場合は、範囲外になるフレーム番号を持つモーションポイントは、削除される。

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetMotionFrameLength : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int initflag = 1;
	ret = curhs->SetMotionFrameLength( motid, frameleng, initflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DSetMotionFrameLength : curhs SetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DAddMP( int hsid, int motid, int boneno, MPINFO2* mpiptr )
{
//適切なタイミングで、E3DFillUpMP必要。
//既にフレーム番号にモーションポイントがあった場合は、エラー

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

	ret = s_qh->GetQ( mpiptr->qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DAddMP : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	srctra = mpiptr->tra;
	srcsc = mpiptr->scale;
	srcframeno = mpiptr->frameno;
	srcinterp = mpiptr->interpolation;
	srcuserint1 = mpiptr->userint1;
	srcds = 0xFFFFFFFF;

	CQuaternion addq;
	addq.CopyFromCQuaternion2( srcqptr );//!!!!!!!!!

	int mpid = -1;
	ret = curhs->AddMotionPoint( motid, boneno, &addq, &srctra, srcframeno, srcds, srcinterp, &srcsc, srcuserint1, &mpid );
	if( ret ){
		DbgOut( "e3dhsp : E3DAddMP : curhs AddMotioinPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	mpiptr->mpid = mpid;

	return 0;
}


EASY3D_API int E3DDeleteMP( int hsid, int motid, int boneno, int mpid )
{
//適切なタイミングで、E3DFillUpMP必要。

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

EASY3D_API int E3DFillUpMotion( int hsid, int motid, int boneno, int startframe, int endframe )
{
//boneno指定の場合は、boneno以下のtreeのみ更新。
//boneno == -1のときは、全てのボーンを更新。
//endmp == -1 のときは、最後のフレームまでを計算。

	int ret;
	CHandlerSet* curhs;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DFillUpMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int initflag = 1;
	ret = curhs->FillUpMotion( motid, boneno, startframe, endframe, initflag );
	if( ret ){
		DbgOut( "e3dhsp : E3DFillUpMotion : curhs E3DFillUpMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCopyMotionFrame( int hsid, int srcmotid, int srcframe, int dstmotid, int dstframe )
{
// E3DFillUpMotionしないと、結果は反映されない。
//srcframeに、モーションポイントが無い場合は、補間計算が生じる
//dstframe側に、モーションポイントが無い場合は、新規に作成する。

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


EASY3D_API int E3DGetDirQ2( int hsid, int dstqid )
{
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
	
	CQuaternion q = curhs->m_gpd.m_q;
	CQuaternion2 setq;
	setq.SetParams( q.w, q.x, q.y, q.z, curhs->m_twist );
	ret = s_qh->SetQ( dstqid, &setq );
	if( ret ){
		DbgOut( "E3DGetDirQ2 : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DSetDirQ2( int hsid, int srcqid )
{
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

	CQuaternion newq;
	newq.SetParams( srcqptr->w, srcqptr->x, srcqptr->y, srcqptr->z );

	D3DXVECTOR3 neweul;
	ret = qToEuler( 0, &newq, &neweul );
	_ASSERT( !ret );
	ret = modifyEuler( &neweul, &curhs->m_gpd.m_gpe.e3drot );
	_ASSERT( !ret );
	curhs->m_gpd.m_gpe.e3drot = neweul;
	curhs->m_twist = srcqptr->twist;

	curhs->m_gpd.CalcMatWorld();
	

	return 0;
}

EASY3D_API int E3DLookAtQ( int dstqid, D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, int upflag, int divnum )
{
	// ！！！！　qidには、一回前の情報が入っていることを前提とする。

	//upflag == 0 --> 上向き
	//upflag == 1 --> 下向き
	//upflag == 2 --> 自動制御、宙返りモード
	//upflag == 3 --> 上向き保持なしモード

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
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}

	DXVec3Normalize( &tarvec, &tarvec );


	ret = LookAtQ( dstqptr, tarvec, basevec, divnum, upflag );
	if( ret ){
		DbgOut( "E3DLookAtQ : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DMultQVec( int qid, D3DXVECTOR3 befvec, D3DXVECTOR3* aftvecptr)
{
	// 初期状態のベクトルをbefvecに渡せば、現在向いている向きベクトルを求めることも出来ます。

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

	qptr->Rotate( aftvecptr, befvec );

	return 0;
}


EASY3D_API int E3DTwistQ( int qid, float twistdeg, D3DXVECTOR3 basevec )
{
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
	
	D3DXVECTOR3 axis;
	DXVec3Normalize( &basevec, &basevec );
	if( (basevec.x == 0.0f) && (basevec.y == 0.0f) && (basevec.z == 0.0f) ){
		basevec.x = 0.0f;
		basevec.y = 0.0f;
		basevec.z = -1.0f;
	}

	CQuaternion2 rotq;
	rotq.SetAxisAndRot( basevec, twistdeg * (float)DEG2PAI );

	*qptr = *qptr * rotq;


	float savetwist;
	savetwist = qptr->twist;
	// 0 - 360
	float finaltwist, settwist;
	finaltwist = savetwist + twistdeg;

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


EASY3D_API int E3DInitTwistQ( int qid, D3DXVECTOR3 basevec )
{
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
	
	D3DXVECTOR3 axis;

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


EASY3D_API int E3DGetTwistQ( int qid, float* twistptr )
{
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

	*twistptr = qptr->twist;

	return 0;
}

EASY3D_API int E3DRotateQLocalX( int qid, float deg )
{
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

		

	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 1.0f, 0.0f, 0.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, deg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}


EASY3D_API int E3DRotateQLocalY( int qid, float deg )
{
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

		

	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 0.0f, 1.0f, 0.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, deg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}



EASY3D_API int E3DRotateQLocalZ( int qid, float deg )
{

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


	float savetwist = qptr->twist;
	D3DXVECTOR3 axis( 0.0f, 0.0f, 1.0f );
	
	CQuaternion2 rotq;
	rotq.SetAxisAndRot( axis, deg * (float)DEG2PAI );

	*qptr = *qptr * rotq;

	
	qptr->twist = savetwist;

	return 0;
}

EASY3D_API int E3DGetBonePos( int hsid, int boneno, int poskind, int motid, int frameno, D3DXVECTOR3* posptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetBonePos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 dstpos;
	int scaleflag = 1;

	ret = curhs->GetBonePos( boneno, poskind, motid, frameno, scaleflag, &dstpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetBonePos : curhs GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = dstpos;

	return 0;

}

EASY3D_API int E3DCreateLine( D3DXVECTOR3* posptr, int pointnum, int maxpointnum, int linekind, int* lidptr )
{
		//videomemoryは、maxpointnumでアロケートする
		//linekind --> LINELIST = 2, LINESTRIP = 3

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

	if( maxpointnum < pointnum ){
		maxpointnum = pointnum;//!!!!!
	}
	
	if( pointnum > 2147483647 ){
		DbgOut( "e3dhsp : E3DCreateLine : pointnum too large : fixed to 2147483647 : warning !!!\n" );
		pointnum = 2147483647;
	}
	
	if( maxpointnum > 2147483647 ){
		DbgOut( "e3dhsp : E3DCreateLine : maxpointnum too large : fixed to 2147483647 : warning !!!\n" );
		maxpointnum = 2147483647;
	}

	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


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

	*lidptr = newhs->serialno;



	return 0;
}
	

EASY3D_API int E3DDestroyLine( int lid )
{

	int ret;
	ret = DestroyHandlerSet( lid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyLine : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetLineColor( int lid, E3DCOLOR4UC col )
{

	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : SetLineColor : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetExtLineColor( col.a, col.r, col.g, col.b );
	if( ret ){
		DbgOut( "e3dhsp : SetLineColor : curhs SetExtLineColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DAddPoint2Line( int lid, int previd, int* pidptr )
{
//previd == -2のとき先頭、-1のとき最後

	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : AddPoint2Line : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int newid = -1;
	ret = curhs->AddPoint2ExtLine( previd, &newid );
	if( ret ){
		DbgOut( "e3dhsp : AddPoint2Line : curhs AddPoint2ExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*pidptr = newid;

	return 0;
}

EASY3D_API int E3DDeletePointOfLine( int lid, int pid )
{
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

EASY3D_API int E3DSetPointPosOfLine( int lid, int pid, D3DXVECTOR3 pos )
{
	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DSetPointPosOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetPointPosOfExtLine( pid, pos );
	if( ret ){
		DbgOut( "e3dhsp : SetPointPosOfLine : curhs SetPointPosOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetPointPosOfLine( int lid, int pid, D3DXVECTOR3* posptr )
{
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

	*posptr = dstpos;

	return 0;
}

EASY3D_API int E3DGetNextPointOfLine( int lid, int previd, int* nextidptr )
{
	//pointid のnext チェインのpointidを返す。pointid == -1 のとき、先頭pointを返すnextid == -1のときは、nextなし、


	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetNextPointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int next = -1;
	ret = curhs->GetNextPointOfExtLine( previd, &next );
	if( ret ){
		DbgOut( "e3dhsp : GetNextPointOfLine : curhs GetNextPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*nextidptr = next;

	return 0;
}


EASY3D_API int E3DGetPrevPointOfLine( int lid, int pid, int* previdptr )
{
	// befid == -1のときは、befなし

	CHandlerSet* curhs;
	curhs = GetHandlerSet( lid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetPrevPointOfLine : lid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int prev = -1;
	ret = curhs->GetPrevPointOfExtLine( pid, &prev );
	if( ret ){
		DbgOut( "e3dhsp : GetPrevPointOfLine : curhs GetPrevPointOfExtLine error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*previdptr = prev;

	return 0;
}

EASY3D_API int E3DWriteDisplay2BMP( const char* tempname1, int scid )
{
	int ret;	
	
	char fname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DWriteDisplay2BMP : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	//アンチがオンの時は、エラーにする。（どっちみちLockでエラーになる）
	if( s_multisample != 0 ){
		DbgOut( "E3DWriteDisplay2BMP : cant run because Antialias is ON error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	LPDIRECT3DDEVICE9 pd3dDevice;
	ret = g_pD3DApp->GetD3DDevice( &pd3dDevice );
	if( ret || !pd3dDevice ){
		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : d3dDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DWriteDisplay2BMP : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;

	HRESULT hr;
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
	enpos = strchr( fname, enmark );

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
		chkleng = leng + (int)strlen( fname );

		if( chkleng >= 2048 ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : chkleng error !!! file name too long !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}

		strcpy_s( outputname, 2048, moduledir );
		strcat_s( outputname, 2048, fname );

		DbgOut( "e3dhsp : E3DWriteDisplay2BMP : outputname0 %s\n", outputname );
	}else{
		int chkleng;
		chkleng = (int)strlen( fname );
		if( chkleng >= 2048 ){
			DbgOut( "e3dhsp : E3DWriteDisplay2BMP : chkleng error !!! file name too long !!!\n" );
			_ASSERT( 0 );
			pBack->UnlockRect();
			pBack->Release();
			return 1;
		}
		strcpy_s( outputname, 2048, fname );
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

EASY3D_API int E3DCreateAVIFile( int scid, const char* tempname1, int datatype, int compkind, int framerate, int frameleng, int* idptr )
{
		// ファイル名は、拡張子なし
		// 現バージョンでは、datatype は１：videoのみ
		// compkind
		//	0 --> 圧縮無し
		//	1 --> cinepak Codec by Radius
		//	2 --> Microsoft Video 1
		//終了時にcompleteしていないaviは、自動的にcompleteする。(ただしpdevが有効な間にしないとだめ！！！)

	char aviname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( aviname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DCreateAVIFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;

	//アンチがオンの時は、エラーにする。（どっちみちLockでエラーになる）
	if( s_multisample != 0 ){
		DbgOut( "E3DCreateAVIFile : cant run because Antialias is ON error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateAVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DCreateAVIFile : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;


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

	ret = newavi->CreateAVIFile( pBack, aviname, datatype, compkind, framerate, frameleng );
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
	
	*idptr = newavi->serialno;

	return 0;

}
EASY3D_API int E3DWriteData2AVIFile( int scid, int aviid, int datatype )
{
		//completeflagが立っている場合や、データ数がframelengに達している場合は、return する。
	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DWriteData2AVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DWriteData2AVIFile : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;


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


	ret = curavi->Write2AVIFile( pBack, datatype );
	if( ret ){
		DbgOut( "E3DWriteData2AVIFile : curavi Write2AVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;

}

EASY3D_API int E3DCompleteAVIFile( int scid, int aviid, int datatype )
{
		//データ数が足りない場合は、ダミーデータを付加してcompleteする。
	int ret;


	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCompleteAVIFile : d3ddevice not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DCompleteAVIFile : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 pBack;
	pBack = cursc->pBackBuffer;

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


	ret = curavi->CompleteAVIFile( pBack, datatype );
	if( ret ){
		DbgOut( "E3DCompleteAVIFile : curavi CompleteAVIFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCameraLookAt( D3DXVECTOR3 tarvec, int upflag, int divnum )
{

	D3DXVECTOR3 basevec;
	basevec.x = 0.0f;
	basevec.y = 0.0f;
	basevec.z = 1.0f;
	DXVec3Normalize( &basevec, &basevec );

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


EASY3D_API int E3DCameraOnNaviLine( int nlid, int mode, int roundflag, int reverseflag, D3DXVECTOR3 offset, float posstep, 
	int dirdivnum, int upflag, int* tpidptr )
{
//	mode = 0 -> 位置のみ
//	mode = 1 -> 位置と向き
//	dirdivnum : 分割数

	int ret;

	CNaviLine* curnl;
	curnl = GetNaviLine( nlid );
	if( !curnl ){
		DbgOut( "E3DCameraOnNaviLine : nlid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 newpos;
	CQuaternion newq;

	float fdegstep = 360.0f;

	CQuaternion tempdirq;
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	tempdirq.CopyFromCQuaternion2( &invq );//!!!!!!!!!

	int tpid = -1;
	ret = curnl->ControlByNaviLine( s_camerapos, offset, tempdirq, 
			1, roundflag, reverseflag,
			1000.0f, 
			posstep, fdegstep,
			&newpos, &newq, &tpid );
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

	*tpidptr = tpid;

	return 0;
}


EASY3D_API int E3DCameraDirUp( float deg, int divnum )
{

//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = deg * (float)DEG2PAI; 

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

EASY3D_API int E3DCameraDirDown( float deg, int divnum )
{
//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );
	
	float rad;
	rad = deg * (float)DEG2PAI; 

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

EASY3D_API int E3DCameraDirRight( float deg, int divnum )
{
//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );

	CQuaternion2 saveinvq;
	saveinvq = invq;


	float rad;
	rad = deg * (float)DEG2PAI; 

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

/***
	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/

	if( divnum >= 1 )
		invq = saveinvq.Slerp( newq, divnum, 1 );


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

EASY3D_API int E3DCameraDirLeft( float deg, int divnum )
{
//// targetを求める
	CQuaternion2 invq;
	s_cameraq.inv( &invq );

	CQuaternion2 saveinvq;
	saveinvq = invq;

	float rad;
	rad = deg * (float)DEG2PAI; 

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

	/***
	int ret;
	ret = LookAtQ( &invq, dirvec, basevec, divnum, 0 );
	if( ret ){
		DbgOut( "e3dhsp : E3DCameraDirUp : LookAtQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	***/
	if( divnum >= 1 )
		invq = saveinvq.Slerp( newq, divnum, 1 );


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

EASY3D_API int E3DSlerpQ( int startqid, int endqid, float t, int resqid )
{
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

	*resq2 = startq2->Slerp( *endq2, t );
	

	return 0;
}


EASY3D_API int E3DSquadQ( int befqid, int startqid, int endqid, int aftqid, float t, int resqid )
{
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


	ret = resq2->Squad( *befq2, *startq2, *endq2, *aftq2, t );
	if( ret ){
		DbgOut( "E3DSquadQ : resq Squad error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSplineVec( D3DXVECTOR3 pos0, D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR3 pos3, float t, D3DXVECTOR3* resptr )
{
	int ret;
	ret = CatmullRomPoint( &pos0, &pos1, &pos2, &pos3, t, resptr );
	if( ret ){
		DbgOut( "E3DSplineVec : CatmullRomPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DDbgOut( char* lpFormat, ... )
{
	int ret;
	va_list Marker;
	//unsigned long wleng, writeleng;
	char outchar[7000];
			
	ZeroMemory( outchar, 7000 );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, 7000, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	int findrep = 0;
	int curlen;
	curlen = (int)strlen( outchar );
	if( (curlen >= 1) && (outchar[curlen - 1] == '\n') ){
		if( curlen == 1 ){
			findrep = 1;
		}else{
			if( outchar[curlen - 2] != '\r' ){
				findrep = 1;
			}
		}

		if( findrep && (curlen <= (7000 - 2)) ){
			outchar[curlen - 1] = '\r';
			outchar[curlen] = '\n';
			outchar[curlen + 1] = 0;
		}
	}

//	wleng = (unsigned long)strlen( outchar );
//	WriteFile( dbgfile, outchar, wleng, &writeleng, NULL );
//	if( wleng != writeleng ){
//		return 1;
//	}

	return DbgOut( outchar );

	return 0;
	
}

EASY3D_API int E3DSaveQuaFile( int quatype, int hsid, int motid, const char* tempname1 )
{

	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSaveQuaFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveQuaFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SaveQuaFile( quatype, motid, filename );
	if( ret ){
		DbgOut( "E3DSaveQuaFile : curhs SaveQuaFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSaveSigFile( int hsid, const char* tempname1 )
{

	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSaveSigFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMQOFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SaveSigFile( filename );
	if( ret ){
		DbgOut( "E3DSaveSigFile : curhs SaveSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DPickVert( int scid, int hsid, POINT scpos, POINT range, int* partarray, int* vertarray, int arrayleng, int* getnumptr )
{
	if( scid < 0 ){
		DbgOut( "E3DPickVert : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPickVert : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->PickVert( s_matView, scpos.x, scpos.y, range.x, range.y, partarray, vertarray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DPickVert : curhs PickVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetOrgVertNo( int hsid, int partno, int vertno, int* orgnoarray, int arrayleng, int* getnumptr )
{
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

EASY3D_API int E3DChkIM2Status( int hsid, int* statusptr )
{

////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChkIM2Status : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int status = 0;
	ret = curhs->ChkIM2Status( &status );
	if( ret ){
		DbgOut( "E3DChkIM2Status : curhs ChkIM2Status error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*statusptr = status;

	return 0;
}


EASY3D_API int E3DLoadIM2File( int hsid, const char* tempname1 )
{
//////

	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadIM2File : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : LoadIM2File : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->LoadIM2File( filename );
	if( ret ){
		DbgOut( "e3dhsp : LoadIM2File : curhs LoadBIMFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSaveIM2File( int hsid, const char* tempname1 )
{
	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSaveIM2File : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveIM2File : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SaveIM2File( filename );
	if( ret ){
		DbgOut( "E3DSaveIM2File : curhs SaveIM2File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EASY3D_API int E3DGetJointNum( int hsid, int* numptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetJointNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int num = 0;
	ret = curhs->GetJointNum( &num );
	if( ret ){
		DbgOut( "E3DGetJointNum : curhs GetJointNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = num;

	return 0;
}

EASY3D_API int E3DGetJointInfo( int hsid, JOINTINFO* jiarray, int arrayleng, int* getnumptr )
{
	//strcpy( strptr, "あいうえおかきくけこ0123" );
	//strcpy( strptr + 256, "さしすせそ" );
	//strcpy( strptr + 512, "abcdefghijkたちつてと4567_" );
	//*getnumptr = 3;


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetJointInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->GetJointInfo( jiarray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetJointInfo : curhs GetJointInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetFirstJointNo( int hsid, int* jointnoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetFirstJointNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int no = 0;
	ret = curhs->GetFirstJointNo( &no );
	if( ret ){
		DbgOut( "E3DGetFirstJointNo : curhs GetFirstJointNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*jointnoptr = no;

	return 0;
}


EASY3D_API int E3DGetDispObjNum( int hsid, int* numptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispObjNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int num = 0;
	ret = curhs->GetDispObjNum( &num );
	if( ret ){
		DbgOut( "E3DGetDispObjNum : curhs GetDispObjNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = num;

	return 0;
}


EASY3D_API int E3DGetDispObjInfo( int hsid, DISPOBJINFO* doiarray, int arrayleng, int* getnumptr )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispObjInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetDispObjInfo( doiarray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetDispObjInfo : curhs GetDispObjInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DEnableTexture( int hsid, int partno, int enableflag )
{

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

EASY3D_API int E3DJointAddToTree( int hsid, int parentjoint, int childjoint, int lastflag )
{
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

EASY3D_API int E3DJointRemoveFromParent( int hsid, int rmjoint )
{
//	　　子供ジョイントはそのまま

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

EASY3D_API int E3DJointRemake( int hsid )
{
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


EASY3D_API int E3DSigImport( int hsid, const char* tempname1, int adjustuvflag, float mult, D3DXVECTOR3 offset, D3DXVECTOR3 rot )
{
	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSigImport : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSigImport : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetResDir( filename, curhs );//!!!!!!

	ret = curhs->ImportSigFile( filename, adjustuvflag, mult, offset, rot );
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

	return 0;
}


EASY3D_API int E3DSigLoadFromBuf( const char* tempname1, char* buf, int bufsize, int adjustuvflag, float mult, int* hsidptr )
{
	//resdirは最後に\\を忘れずに。

	char texdir[MAX_PATH];
	if( tempname1 ){
		strcpy_s( texdir, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSigLoadFromBuf : texdir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}




	int ret;
	int temphsid = -1;
	ret = SigLoadFromBuf_F( texdir, buf, bufsize, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoadFromBuf : SigLoadFromBuf_F error !!!\n" );
		_ASSERT( 0 );

		return 1;
	}

	*hsidptr = temphsid;

	return 0;
}

int SigLoadFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr )
{


	int ret = 0;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!

	ret = newhs->LoadSigFileFromBuf( bufptr, bufsize, adjustuvflag, mult );
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

	*hsidptr = newhs->serialno;
	
	return 0;
}



EASY3D_API int E3DSigImportFromBuf( int hsid, const char* tempname1, char* buf, int bufsize, int adjustuvflag, float mult, 
	D3DXVECTOR3 offset, D3DXVECTOR3 rot )
{
	char texdir[MAX_PATH];
	if( tempname1 ){
		strcpy_s( texdir, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSigImportFromBuf : texdir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSigImportFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetResDir( texdir, curhs );//!!!!!!


//DWORD dwtime1, dwtime2, dwtime3;
//g_rtsc.Start();

	ret = curhs->ImportSigFileFromBuf( buf, bufsize, adjustuvflag, mult, offset, rot );
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : hs ImportSigFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//Create 3D Object
	ret = Restore( curhs, -1 );
	if( ret ){
		DbgOut( "E3DSigImportFromBuf : Restore error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}


EASY3D_API int E3DAddMotionFromBuf( int hsid, char* buf, int bufsize, float mvmult, int* motidptr, int* maxframeptr )
{
	
	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	int datakind = 0;//qua
	ret = AddMotionFromBuf_F( hsid, datakind, buf, bufsize, &tempmotid, &tempnum, mvmult );
	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : AddMotionFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*motidptr = tempmotid;
	*maxframeptr = tempnum;

	return 0;
}

int AddMotionFromBuf_F( int hsid, int datakind, char* bufptr, int bufsize, int* cookieptr, int* maxnumptr, float mvmult )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMotionFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
	//if( datakind == 0 ){
		ret = curhs->LoadQuaFileFromBuf( bufptr, bufsize, cookieptr, maxnumptr, mvmult );	
	//}else{
	//	_ASSERT( 0 );
	//	ret = curhs->LoadMotFileFromBuf( bufptr, bufsize, cookieptr, maxnumptr );
	//}

	if( ret ){
		DbgOut( "E3DAddMotionFromBuf : Load***File error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCheckFullScreenParams( FULLSCPARAMS srcparams, FULLSCPARAMS* validparams, int flag )
{

	if( srcparams.bits <= 16 ){
		srcparams.bits = 16;
	}else if( (srcparams.bits > 16) && (srcparams.bits < 32) ){
		srcparams.bits = 32;
	}else if( srcparams.bits >= 32 ){
		srcparams.bits = 32;
	}


	CD3DApplication* tmpapp;

	tmpapp = new CD3DApplication();
	if( !tmpapp ){
		DbgOut( "E3DCheckFullScreenParams : tmpapp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	//int validflag, validw, validh, validbits;
	ret = tmpapp->CheckFullScreenParams( flag, srcparams.scsize, srcparams.bits, 
		&validparams->validflag, &(validparams->scsize), &validparams->bits );
	if( ret ){
		DbgOut( "E3DCheckFullScreenParams : tmpapp CheckFullScreenParams error !!!\n" );
		_ASSERT( 0 );
		delete tmpapp;
		return 1;
	}

	delete tmpapp;

	return 0;
}

EASY3D_API int E3DGetMaxMultiSampleNum( int bits, int iswindowmode, int* numptr )
{
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
	int num = 1;
	ret = tmpapp->GetMaxMultiSampleNum( bits, iswindowmode, &num );
	if( ret ){
		DbgOut( "E3DGetMaxMultiSampleNum : tmpapp GetMaxMultiSampleNum error !!!\n" );
		_ASSERT( 0 );
		delete tmpapp;
		return 1;
	}

	*numptr = num;

	delete tmpapp;

	return 0;
}

EASY3D_API int E3DPickFace( int scid, int hsid, POINT scpos, float maxdist, int* partptr, int* faceptr, 
	D3DXVECTOR3* pos3dptr, D3DXVECTOR3* nptr, float* distptr )
{
	if( scid < 0 ){
		DbgOut( "E3DPickFace : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int calcmode = 1;
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


	if( maxdist < 0 ){
		maxdist = 10000.0f;
	}

	float fdist;

	if( calcmode == 1 ){
		CalcMatView();//!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPickFace : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int part = -1;
	int face = 0;
	ret = curhs->PickFace( s_matView, maxdist, scpos.x, scpos.y, &part, &face, &pos3, &nvec, &fdist, calcmode );
	if( ret ){
		DbgOut( "E3DPickFace : curhs PickFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*partptr = part;
	*faceptr = face;
	*pos3dptr = pos3;

	if( part >= 0 ){
		D3DXVec3Normalize( &nvec, &nvec );
	}
	*nptr = nvec;


	if( part < 0 ){
		*distptr = 900000000.0f;
	}else{
		*distptr = fdist;
	}


	return 0;
}

EASY3D_API int E3DPickFacePartArray( int scid, int hsid, int* partarray, int partnum, POINT scpos, float maxdist, int* partptr, int* faceptr,
	D3DXVECTOR3* pos3dptr, D3DXVECTOR3* nptr, float* distptr )
{
	if( scid < 0 ){
		DbgOut( "E3DPickFace2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int calcmode = 1;
////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickFace2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CalcMatView();


	D3DXVECTOR3 pos3;
	D3DXVECTOR3 nvec;


	if( maxdist < 0 ){
		maxdist = 10000.0f;
	}

	float fdist;

	if( calcmode == 1 ){
		CalcMatView();//!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPickFace2 : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int part, face;
	ret = curhs->PickFace2( s_matView, partarray, partnum, maxdist, scpos.x, scpos.y, &part, &face, &pos3, &nvec, &fdist, calcmode );
	if( ret ){
		DbgOut( "E3DPickFace2 : curhs PickFace2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	*partptr = part;
	*faceptr = face;

	*pos3dptr = pos3;

	if( part >= 0 ){
		D3DXVec3Normalize( &nvec, &nvec );
	}
	*nptr = nvec;


	if( part < 0 ){
		*distptr = 900000000.0;
	}else{
		*distptr = fdist;
	}


	return 0;
}


EASY3D_API int E3DGetBBox( int hsid, int partno, int mode, D3DXVECTOR3* minptr, D3DXVECTOR3* maxptr )
{
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

	minptr->x = fminx;
	minptr->y = fminy;
	minptr->z = fminz;

	maxptr->x = fmaxx;
	maxptr->y = fmaxy;
	maxptr->z = fmaxz;

	return 0;
}

EASY3D_API int E3DGetVertNoOfFace( int hsid, int partno, int faceno, TRIVNO* vnoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetVertNoOfFace : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int v1, v2, v3;
	ret = curhs->GetVertNoOfFace( partno, faceno, &v1, &v2, &v3 );
	if( ret ){
		DbgOut( "E3DGetVertNoOfFace : hs GetVertNoOfFace error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	vnoptr->vert1 = v1;
	vnoptr->vert2 = v2;
	vnoptr->vert3 = v3;

	return 0;
}

EASY3D_API int E3DGetSamePosVert( int hsid, int partno, int vertno, int* samearray, int arrayleng, int* getnumptr )
{
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetSamePosVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int samenum = 0;
	ret = curhs->GetSamePosVert( partno, vertno, samearray, arrayleng, &samenum );
	if( ret ){
		DbgOut( "E3DGetSamePosVert : hs GetSamePosVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*getnumptr = samenum;

	return 0;
}

EASY3D_API int E3DRtscStart()
{
	g_rtsc.Start();
	return 0;
}

EASY3D_API int E3DRtscStop( DWORD* timeptr )
{
	DWORD dwret;
	dwret = g_rtsc.Stop();

	*timeptr = dwret;

	return 0;
}

EASY3D_API int E3DSaveSig2Buf( int hsid, char* buf, int bufsize, int* wsizeptr )
{
//bufsizeに０を指定した場合には、writesizeに必要なバッファ長を代入するのみ

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveSig2Buf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int writesize = 0;
	ret = curhs->SaveSig2Buf( buf, bufsize, &writesize );
	if( ret ){
		DbgOut( "E3DSaveSig2Buf : hs SaveSig2Buf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*wsizeptr = writesize;

	return 0;
}


EASY3D_API int E3DSaveQua2Buf( int quatype, int hsid, int motid, char* buf, int bufsize, int* wsizeptr )
{
//bufsizeに０を指定した場合には、writesizeに必要なバッファ長を代入するのみ

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveSig2Buf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int writesize = 0;
	ret = curhs->SaveQua2Buf( quatype, motid, buf, bufsize, &writesize );
	if( ret ){
		DbgOut( "E3DSaveSig2Buf : hs SaveSig2Buf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*wsizeptr = writesize;

	return 0;
}


EASY3D_API int E3DCameraShiftLeft( float shift )
{

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
	s_camerapos = s_camerapos + shift * shiftdir;
	s_cameratarget = s_cameratarget + shift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

EASY3D_API int E3DCameraShiftRight( float shift )
{


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
	s_camerapos = s_camerapos + shift * shiftdir;
	s_cameratarget = s_cameratarget + shift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

EASY3D_API int E3DCameraShiftUp( float shift )
{

	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	D3DXVECTOR3 ydir( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 shiftdir;
	

	invview.Rotate( &shiftdir, ydir );
	s_camerapos = s_camerapos + shift * shiftdir;
	s_cameratarget = s_cameratarget + shift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

EASY3D_API int E3DCameraShiftDown( float shift )
{

	CQuaternion2 invview;
	s_cameraq.inv( &invview );
	invview.normalize();

	D3DXVECTOR3 ydir( 0.0f, -1.0f, 0.0f );
	D3DXVECTOR3 shiftdir;
	

	invview.Rotate( &shiftdir, ydir );
	s_camerapos = s_camerapos + shift * shiftdir;
	s_cameratarget = s_cameratarget + shift * shiftdir;

	//s_cameraupvec = newup;
	s_cameratype = 2;

	return 0;
}

EASY3D_API int E3DGetCameraQ( int dstqid )
{
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


EASY3D_API int E3DInvQ( int srcqid, int dstqid )
{
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

EASY3D_API int E3DSetCameraTwist( float twistdeg )
{

	float savetwist = s_camera_twist;
	s_camera_twist = twistdeg;

	float difftwist = savetwist - s_camera_twist;


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
	addrot.SetAxisAndRot( axisvec, difftwist * (float)DEG2PAI );
	
	
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


	CalcMatView();

	return 0;
}

EASY3D_API int E3DIKRotateBeta( int scid, int hsid, int boneno, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axis,
	int calclevel, D3DXVECTOR3 target, D3DXVECTOR3* resptr, int* lastparentptr )
{
//	iklevel : 階層数
//	axiskind : 0-->視線に平行な軸
//	　　　　　　1-->指定した軸
//	　　　　　　2-->Auto選択
//	axisx, y, z : axiskindに１を指定したときのみ有効
//	calclevel : 計算の細かさ、数が大きいほど細かい。
//	targetx, y, z : ボーンの目標地点。
//	resx, y, z : 計算後のジョイントの位置。		
//	lastparent : 計算で変更のあった一番上の階層のジョイントの番号。E3DRenderに渡すと高速化出来る。

	if( scid < 0 ){
		DbgOut( "E3DIKRotateBeta : scid error !!!\n" );
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
		axisvec = axis;
		DXVec3Normalize( &axisvec, &axisvec );
		break;
	case 2:
		axisvec.x = 0.0f;//ダミーデータ
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	default:
		_ASSERT( 0 );
		axiskind = 2;//!!!!!!
		axisvec.x = 0.0f;//ダミーデータ
		axisvec.y = 0.0f;
		axisvec.z = 1.0f;
		break;
	}
	
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DIKRotateBeta : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "E3DIKRotateBeta : axisvec %f %f %f\r\n", axisvec.x, axisvec.y, axisvec.z );
	*lastparentptr = 0;
	ret = curhs->IKRotateBeta( s_camerapos, pdev, s_matView, boneno, motid, frameno, iklevel, axiskind, axisvec, calclevel, 
		target, resptr, lastparentptr );
	if( ret ){
		DbgOut( "E3DIKRotateBeta : hs IKRotateBeta error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DIKRotateBeta2D( int scid, int hsid, int boneno, int motid, int frameno, int iklevel, int axiskind, D3DXVECTOR3 axis,
	int calclevel, D3DXVECTOR2 target2d, D3DXVECTOR3* resptr, int* lastparentptr )
{
	if( scid < 0 ){
		DbgOut( "E3DIKRotateBeta2D : scid error !!!\n" );
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
		axisvec = axis;
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
	
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DIKRotateBeta2D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


/////////////////

	D3DXVECTOR3 mvobj;
	ret = curhs->GetBonePos( boneno, 1, motid, frameno, 1, &mvobj );
	if( ret ){
		DbgOut( "E3DIKRotateBeta2D : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	D3DXVECTOR3 targetsc, targetobj, targetobj0, targetobj1;
	
	targetsc.x = target2d.x;
	targetsc.y = target2d.y;
	targetsc.z = 0.0f;
	ret = ScreenToObj( &targetobj0, targetsc );
	if( ret )
		return 0;

	targetsc.x = target2d.x;
	targetsc.y = target2d.y;
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
	*lastparentptr = 0;
	ret = curhs->IKRotateBeta( s_camerapos, pdev, s_matView, boneno, motid, frameno, iklevel, axiskind, axisvec, 
		calclevel, temptarget, resptr, lastparentptr );
	if( ret ){
		DbgOut( "E3DIKRotateBeta2D : hs IKRotateBeta error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMotionType( int hsid, int motid, int* typeptr )
{

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int type = 0;
	ret = curhs->GetMotionType( motid, &type );
	if( ret ){
		DbgOut( "E3DGetMotionType : hs GetMotionType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	*typeptr = type;
	
	return 0;
}

EASY3D_API int E3DSetMotionType( int hsid, int motid, int type )
{
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

EASY3D_API int E3DGetIKTransFlag( int hsid, int boneno, int* flagptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIKTransFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int flag = 0;
	ret = curhs->GetIKTransFlag( boneno, &flag );
	if( ret ){
		DbgOut( "E3DGetIKTransFlag : hs GetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*flagptr = flag;

	return 0;
}


EASY3D_API int E3DSetIKTransFlag( int hsid, int boneno, int flag )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKTransFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetIKTransFlag( boneno, flag );
	if( ret ){
		DbgOut( "E3DSetIKTransFlag : hs SetIKTransFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DDestroyAllMotion( int hsid )
{
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


	
EASY3D_API int E3DGetUserInt1OfPart( int hsid, int partno, int* userint1ptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetUserInt1OfPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int userint1 = 0;
	ret = curhs->GetUserInt1OfPart( partno, &userint1 );
	if( ret ){
		DbgOut( "E3DGetUserInt1OfPart : hs GetUserInt1OfPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*userint1ptr = userint1;

	return 0;
}

EASY3D_API int E3DSetUserInt1OfPart( int hsid, int partno, int userint1 )
{
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


EASY3D_API int E3DGetBSphere( int hsid, int partno, BSPHERE* bsptr )
{
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

	bsptr->center = dstcenter;
	bsptr->r = dstr;

	return 0;
}



EASY3D_API int E3DGetChildJoint( int hsid, int parentno, int* childarray, int arrayleng, int* getnumptr )
{

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBSphere : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int childnum = 0;
	ret = curhs->GetChildJoint( parentno, arrayleng, childarray, getnumptr );
	if( ret ){
		DbgOut( "E3DGetChildJoint : hs GetChildJoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DDestroyMotionFrame( int hsid, int motid, int frameno )
{

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


EASY3D_API int E3DGetKeyFrameNo( int hsid, int motid, int boneno, int* framenoarray, int arrayleng, int* getnumptr )
{

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetKeyFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int framenum = 0;
	ret = curhs->HuGetKeyFrameNo( motid, boneno, framenoarray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetKeyframeNo : hs HuGetKeyFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}

EASY3D_API int E3DSetUV( int hsid, int partno, int vertno, UV uv, int setflag, int clampflag )
{
	int ret;

	CHandlerSet* curhs = 0;
	if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		curhs = GetHandlerSet( hsid );
	}
	if( !curhs ){
		DbgOut( "E3DSetUV : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUV( partno, vertno, uv.u, uv.v, setflag, clampflag );
	if( ret ){
		DbgOut( "E3DSetUV : hs SetUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
	return 0;
}

EASY3D_API int E3DSetUVBatch( int hsid, int partno, int* vertnoptr, int vertnum, UV* uvptr, int setflag, int clampflag )
{
	int ret;

	CHandlerSet* curhs = 0;
	if( hsid == -1 ){
		curhs = g_bbhs;
	}else{
		curhs = GetHandlerSet( hsid );
	}
	if( !curhs ){
		DbgOut( "E3DSetUVBatch : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetUVBatchUV( partno, vertnoptr, vertnum, uvptr, setflag, clampflag );
	if( ret ){
		DbgOut( "E3DSetUVBatch : hs SetUVBatchDouble error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



EASY3D_API int E3DGetUV( int hsid, int partno, int vertno, UV* uvptr )
{

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
	uvptr->u = fu;
	uvptr->v = fv;

	return 0;
}

EASY3D_API int E3DPickBone( int hsid, POINT scpos, int* bonenoptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int boneno = -1;
	ret = curhs->PickBone( s_matView, scpos.x, scpos.y, &boneno );
	if( ret ){
		DbgOut( "E3DPickBone : hs PickBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*bonenoptr = boneno;


	return 0;
}


EASY3D_API int E3DShiftBoneTree2D( int scid, int hsid, int boneno, int motid, int frameno, D3DXVECTOR2 target2d )
{

	if( scid < 0 ){
		DbgOut( "E3DShiftBoneTree2D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DShiftBoneTree2D : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DShiftBoneTree2D : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 mvobj;
	ret = curhs->GetBonePos( boneno, 1, motid, frameno, 1, &mvobj );
	if( ret ){
		DbgOut( "E3DShiftBoneTree2D : GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	D3DXVECTOR3 targetsc, targetobj, targetobj0, targetobj1;
	
	targetsc.x = target2d.x;
	targetsc.y = target2d.y;
	targetsc.z = 0.0f;
	ret = ScreenToObj( &targetobj0, targetsc );
	if( ret )
		return 0;

	targetsc.x = target2d.x;
	targetsc.y = target2d.y;
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
	ret = curhs->ShiftBoneTreeDiff( boneno, mvdiff, motid, frameno );
	if( ret ){
		DbgOut( "E3DShiftBoneTree2D : hs ShiftBoneTreeDiff error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetDispSwitch( int hsid, int boneno, int motid, int swid, int srcframeno, int swflag )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDispSwitch2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SetDispSwitch2( boneno, motid, swid, srcframeno, swflag );
	if( ret ){
		DbgOut( "E3DSetDispSwitch2 : SetDispSwitch2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetDispSwitch( int hsid, int* swarray, int arrayleng )
{
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDispSwitch2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetDispSwitch2( swarray, arrayleng );
	if( ret ){
		DbgOut( "E3DGetDispSwitch2 : hs GetDispSwitch2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DCreateTexture( const char* tempname1, int pool, int transparent, int* texidptr )
{
	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DCreateTexture : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


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
	cmp1 = strcmp( filename, "MainWindow" );
	if( cmp1 != 0 ){
		
		sprintf_s( usertexname, MAX_PATH + 256, "%s_TEXID%d", filename, s_usertexcnt );		
	}else{
		pool = D3DPOOL_SYSTEMMEM;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!強制設定
		if( transparent == 1 ){
			transparent = 2;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!強制設定
		}
		sprintf_s( usertexname, MAX_PATH + 256, "%s_%d", filename, s_usertexcnt );
	}

EnterCriticalSection( &g_crit_restore );//######## start crit

	newserial = g_texbnk->AddName( 0, usertexname, transparent, pool );
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

	*texidptr = newserial;

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

EASY3D_API int E3DGetTextureInfo( TEXTUREINFO* texinfoptr )
{
////
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DGetTextureInfo : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->GetTextureInfoBySerial( texinfoptr->texid, &texinfoptr->width, &texinfoptr->height, 
		&texinfoptr->pool, &texinfoptr->transparent, &texinfoptr->format );
	if( ret ){
		DbgOut( "E3DGetTextureInfo : texbank GetTextureSizeBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	texinfoptr->name[255] = 0;//!!!!!!!!!!!!!!!!!

	return 0;
}

EASY3D_API int E3DGetTextureFromDispObj( int hsid, int partno, int* texidptr )
{	
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

	int texid = -1;
	if( curhs ){
		ret = curhs->GetTextureFromDispObj( partno, &texid );
		if( ret ){
			DbgOut( "E3DGetTextureFromDispObj : hs GetTextureFromDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( cursprite ){
		ret = cursprite->GetTextureFromDispObj( &texid );
		if( ret ){
			DbgOut( "E3DGetTextureFromDispObj : sprite GetTextureFromDispObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	*texidptr = texid;


	return 0;
}

EASY3D_API int E3DSetTextureToDispObj( int hsid, int partno, int texid )
{	
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!
	}


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

EASY3D_API int E3DDestroyTexture( int texid )
{
		
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
	return 0;

}

EASY3D_API int E3DSetLightSpecular( int lid, E3DCOLOR4UC col )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetLightSpecular : lightID not found error !!!\n" );
		return 1;
	}

	CVec3f speccol;
	speccol.x = (float)col.r / 255.0f;
	speccol.y = (float)col.g / 255.0f;
	speccol.z = (float)col.b / 255.0f;

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

EASY3D_API int E3DCreateTextureFromBuf( char* buf, int bufsize, int pool, int transparent, int* texidptr )
{
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


	sprintf_s( usertexname, MAX_PATH + 256, "FromBuf_TEXID%d", s_usertexcnt );


	newserial = g_texbnk->AddName( 0, usertexname, transparent, pool );
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


	ret = g_texbnk->SetBufData( newindex, buf, bufsize );
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

	*texidptr = newserial;

	return 0;

}

EASY3D_API int E3DLoadSoundFromBuf( char* buf, int bufleng, int type, int use3dflag, int reverbflag, int bufnum, int* soundidptr )
{
//type 0 --> wav, type 1 --> midi
	int ret;

	if( !s_HS ){
		DbgOut( "E3DLoadSoundFromBuf : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	type = 0;//midi, ogg禁止

// 読み込み
	int soundid = -1;
	ret = s_HS->LoadSoundFileFromBuf( buf, bufleng, type, use3dflag, reverbflag, bufnum, &soundid );
	if( ret ){
		DbgOut( "E3DLoadSoundFromBuf : hs LoadSoundFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*soundidptr = soundid;

	return 0;
}

EASY3D_API int E3DTwistBone( int hsid, int motid, int frameno, int boneno, float twistdeg )
{
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DTwistBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->TwistBone( motid, frameno, boneno, twistdeg );
	if( ret ){
		DbgOut( "E3DTwistBone : hs TwistBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetStartPointOfSound( int soundid, int sec )
{

/***
	if( sec < 0 ){
		sec = 0;//!!!!!!!!!!!!!!!!!!!
	}

	if( !s_HS ){
		DbgOut( "E3DSetStartPointOfSound : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetStartPointOfSound( soundid, sec );
	if( ret ){
		DbgOut( "E3DSetStartPointOfSound : hs SetTartPointOfSound error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/

	return 0;
}



EASY3D_API int E3DGetBoneQ( int hsid, int boneno, int motid, int frameno, int kind, int qid )
{
//	kind 0 --> 親の影響のないクォータニオン
//	kind 1 --> 親の影響を考慮したクォータニオン
//  kind 2 --> 親の影響＋モデル全体のクォータニオンの影響

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




EASY3D_API int E3DSetBoneQ( int hsid, int boneno, int motid, int frameno, int qid, int curboneonly )
{
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


	ret = curhs->SetQ( boneno, motid, frameno, setq, curboneonly );
	if( ret ){
		DbgOut( "E3DSetBoneQ : hs SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DIsSoundPlaying( int soundid, int* playingptr )
{
//	再生中　playing 1
//	停止中　playing 0

/////////////
	int ret;

	if( !s_HS ){
		DbgOut( "E3DIsSoundPlaying : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int playing = 0;
	ret = s_HS->IsSoundPlaying( soundid, &playing );
	if( ret ){
		DbgOut( "E3DIsSoundPlaying : hs IsSoundPlaying error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*playingptr = playing;

	return 0;
}


EASY3D_API int E3DIKTranslate( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3 pos )
{
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKTranslate : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->IKTranslate( boneno, motid, frameno, pos );
	if( ret ){
		DbgOut( "E3DIKTranslate : hs IKTranslate error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetUVTile( int hsid, int partno, int texrule, int unum, int vnum, int tileno )
{
//	texrule 0 --> Ｘ軸投影
//		 1 --> Ｙ軸投影
//		 2 --> Ｚ軸投影
//		 3 --> 円筒貼り付け
//		 4 --> 球貼り付け
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

EASY3D_API int E3DImportMQOFileAsGround( int hsid, const char* tempname1, int adjustuvflag, float mult, D3DXVECTOR3 offset, D3DXVECTOR3 rot )
{
	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DImportMQOFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DImportMQOFileAsGround : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->ImportMQOFileAsGround( filename, s_hwnd, adjustuvflag, mult, offset, rot );
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

	return 0;
}


EASY3D_API int E3DLoadMQOFileAsMovableAreaFromBuf( char* buf, int bufsize, float mult, int* hsidptr )
{
//////

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableAreaFromBuf_F( buf, bufsize, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : LoadMQOFileAsMovableAreaFromBuf_F error !!!\n" );
		_ASSERT( 0 );
	}

	*hsidptr = temphsid;

	return 0;

}

int LoadMQOFileAsMovableAreaFromBuf_F( char* bufptr, int bufsize, float mult, int* hsidptr )
{


	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->LoadMQOFileAsMovableAreaFromBuf( s_hwnd, bufptr, bufsize, mult );
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
	*hsidptr = newhs->serialno;

	return 0;
}


EASY3D_API int E3DChkThreadWorking( int threadid, int* workingptr, int* ret1ptr, int* ret2ptr )
{

////////////////

	if( (threadid < 0) || (threadid >= THMAX) ){
		DbgOut( "E3DChkThreadWorking : threadid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s_thtable[threadid].useflag == 0 ){
		DbgOut( "E3DChkThreadWorking : this thread is invalid : useflag == 0 : threadid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int working, ret, ret2;

	if( s_thtable[threadid].hThread ){
		DWORD dwret;
		dwret = WaitForSingleObject( s_thtable[threadid].hThread, 0 );

		switch( dwret ){
		case WAIT_OBJECT_0:
			working = 0;
			ret = s_thtable[threadid].returnval;
			ret2 = s_thtable[threadid].returnval2;
			break;
		case WAIT_TIMEOUT:
			working = 1;
			ret = -1;
			ret2 = -1;
			break;
		case WAIT_FAILED:
			DbgOut( "E3DChkWorkingThread : WaitForSigleObject WAIT_FAILED warning !!!\n" );
			_ASSERT( 0 );
			working = 0;
			ret = -1;
			ret2 = -1;
			break;
		default:
			_ASSERT( 0 );
			working = 0;
			ret = -1;
			ret2 = -1;
			break;
		}
	}else{
		working = 0;
		ret = s_thtable[threadid].returnval;
		ret2 = s_thtable[threadid].returnval2;
	}

	*workingptr = working;
	*ret1ptr = ret;
	*ret2ptr = ret2;

	return 0;
}


EASY3D_API int E3DLoadMQOFileAsGroundThread( const char* tempname1, float mult, int adjustuvflag, int* threadidptr )
{
//1

	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadMQOFileAsGroundThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DMQOFileAsGroundThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;
	
	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( filename );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadMQOFileAsGroundThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, filename );


	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsGround, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadMQOFileAsGroundThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 0;
	}


	*threadidptr = s_thtable[thno].threadid;

	return 0;
}

DWORD WINAPI ThreadLoadMQOFileAsGround( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = LoadMQOFileAsGround_F( curtable->filename, curtable->mult, s_hwnd, curtable->adjustuvflag, &temphsid, BONETYPE_RDB2 );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGround : LoadMQOFileAsGround_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;

}

EASY3D_API int E3DSigLoadThread( const char* tempname1, int adjustuvflag, float mult, int* threadidptr )
{

	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSigLoadThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DSigLoadThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( filename );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DSigLoadThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, filename );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadSigLoad, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DSigLoadThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}

	*threadidptr = s_thtable[thno].threadid;

	return 0;

}

DWORD WINAPI ThreadSigLoad( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = SigLoad_F( curtable->filename, curtable->mult, curtable->adjustuvflag, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoad : SigLoad_F error %s !!!\n", curtable->filename );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EASY3D_API int E3DSigLoadFromBufThread( const char* tempname1, char* buf, int bufsize, int adjustuvflag, float mult, int* threadidptr )
{

//1

	char texdir[MAX_PATH];
	if( tempname1 ){
		strcpy_s( texdir, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSigLoadFromBufThread : texdir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DSigLoadFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( texdir );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DSigLoadFromBufThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, texdir );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].bufptr = buf;
	s_thtable[thno].bufsize = bufsize;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadSigLoadFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DSigLoadFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[thno].threadid;

	return 0;
}

DWORD WINAPI ThreadSigLoadFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = SigLoadFromBuf_F( curtable->filename, curtable->bufptr, curtable->bufsize, curtable->adjustuvflag, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "E3DSigLoadFromBuf : SigLoadFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EASY3D_API int E3DLoadMQOFileAsMovableAreaThread( const char* tempname1, float mult, int* threadidptr )
{

	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( filename );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, filename );

	s_thtable[thno].mult = mult;


	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsMovableArea, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[thno].threadid;

	
	return 0;
}

DWORD WINAPI ThreadLoadMQOFileAsMovableArea( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableArea_F( curtable->filename, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableArea : LoadMQOFileAsMovableArea_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EASY3D_API int E3DLoadMQOFileAsMovableAreaFromBufThread( char* buf, int bufsize, float mult, int* threadidptr )
{

//////

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;

	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;


	s_thtable[thno].mult = mult;
	s_thtable[thno].bufptr = buf;
	s_thtable[thno].bufsize = bufsize;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadMQOFileAsMovableAreaFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[thno].threadid;

	return 0;

}

DWORD WINAPI ThreadLoadMQOFileAsMovableAreaFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsMovableAreaFromBuf_F( curtable->bufptr, curtable->bufsize, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsMovableAreaFromBuf : LoadMQOFileAsMovableAreaFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = temphsid;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EASY3D_API int E3DLoadGroundBMPThread( const char* tempname1, const char* tempname2, const char* tempname3, const char* tempname4,
	float maxx, float maxz, int divx, int divz, float maxheight, int* threadidptr )
{

	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadGroundBMPThread : filename1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname2[MAX_PATH];
	if( tempname2 ){
		strcpy_s( fname2, MAX_PATH, tempname2 );
	}else{
		DbgOut( "E3DLoadGroundBMPThread : filename2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname3[MAX_PATH];
	if( tempname3 ){
		strcpy_s( fname3, MAX_PATH, tempname3 );
	}else{
		DbgOut( "E3DLoadGroundBMPThread : filename3 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char fname4[MAX_PATH];
	if( tempname4 ){
		strcpy_s( fname4, MAX_PATH, tempname4 );
	}else{
		DbgOut( "E3DLoadGroundBMPThread : filename4 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( divx < 2 )
		divx = 2;

	if( divz < 2 )
		divz = 2;

	if( maxheight < 0.0f )
		maxheight = 0.0f;


//////

	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadGroundBMPThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;


	int leng;
	leng = (int)strlen( fname1 );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, fname1 );


	leng = (int)strlen( fname2 );
	s_thtable[thno].filename2 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename2) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename2, leng + 1, fname2 );


	leng = (int)strlen( fname3 );
	s_thtable[thno].filename3 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename3) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename3, leng + 1, fname3 );

	leng = (int)strlen( fname4 );
	s_thtable[thno].filename4 = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename4) ){
		DbgOut( "E3DLoadGroundBMPThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename4, leng + 1, fname4 );


	s_thtable[thno].maxx = maxx;
	s_thtable[thno].maxz = maxz;
	s_thtable[thno].divx = divx;
	s_thtable[thno].divz = divz;
	s_thtable[thno].maxheight = maxheight;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadGroundBMP, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadGroundBMPThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[thno].threadid;

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

EASY3D_API int E3DAddMotionThread( int hsid, const char* tempname1, int* threadidptr )
{

	char tempname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( tempname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DAddMotionThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DAddMotionThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;
	s_thtable[thno].returnval2 = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DAddMotionThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, tempname );

	s_thtable[thno].hsid = hsid;//!!!!!!!!!!!!!!!!!!!!!!

	//s_thtable[thno].adjustuvflag = adjustuvflag;

	//s_thtable[thno].bufptr = bufptr;
	//s_thtable[thno].bufsize = bufsize;


	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadAddMotion, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DAddMotionThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		s_thtable[thno].returnval2 = -1;
		return 1;
	}

	*threadidptr = s_thtable[thno].threadid;

	return 0;

}

DWORD WINAPI ThreadAddMotion( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotion_F( curtable->hsid, curtable->filename, &tempmotid, &tempnum, 1.0f );
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

EASY3D_API int E3DAddMotionFromBufThread( int hsid, char* buf, int bufsize, int* threadidptr )
{

////////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DAddMotionFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;
	

	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;
	s_thtable[thno].returnval2 = -1;

	s_thtable[thno].hsid = hsid;//!!!!!!!!!!!!!!!!!!!!!!
	s_thtable[thno].adjustuvflag = 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	s_thtable[thno].bufptr = buf;
	s_thtable[thno].bufsize = bufsize;


	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadAddMotionFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DAddMotionFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		s_thtable[thno].returnval2 = -1;
		return 1;
	}

	*threadidptr = s_thtable[thno].threadid;

	return 0;
}

DWORD WINAPI ThreadAddMotionFromBuf( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int tempmotid = -1;
	int tempnum = 0;
	ret = AddMotionFromBuf_F( curtable->hsid, curtable->adjustuvflag, curtable->bufptr, curtable->bufsize, &tempmotid, &tempnum, 1.0f );
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

EASY3D_API int E3DGetShaderType( int hsid, int* shaderptr )
{

////
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int shader, overflow;
	ret = curhs->GetShaderType( &shader, &overflow );
	if( ret ){
		DbgOut( "E3DGetShaderType : hs GetShaderType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*shaderptr = shader;

	return 0;
}

EASY3D_API int E3DSetShaderType( int hsid, int shader )
{


//3 : overflow
	int overflow = OVERFLOW_ORG;

////
	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		DbgOut( "E3DSetShaderType : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DIKRotateBeta : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetShaderType( pdev, shader, overflow );
	if( ret ){
		DbgOut( "E3DSetShaderType : hs SetShaderType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetInfElemNum( int hsid, int partno, int vertno, int* numptr )
{
	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfElemNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int num = 0;
	ret = curhs->GetInfElemNum( partno, vertno, &num );
	if( ret ){
		DbgOut( "E3DGetInfElemNum : curhs GetInfElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = num;

	return 0;

}

EASY3D_API int E3DGetInfElem( int hsid, int partno, int vertno, E3DINFELEM* infelemptr )
{

	int ret;

	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int childjoint, calcmode;
	float userrate, orginf, dispinf;
	ret = curhs->GetInfElem( partno, vertno, infelemptr->infno, &childjoint, &calcmode, &userrate, &orginf, &dispinf );
	if( ret ){
		DbgOut( "E3DGetInfElem : curhs GetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	infelemptr->childjointno = childjoint;
	infelemptr->calcmode = calcmode;
	infelemptr->userrate = userrate * 100.0f;//!!!!!!!!!!
	infelemptr->orginf = orginf;
	infelemptr->dispinf = dispinf;


	return 0;

}


EASY3D_API int E3DSetInfElem( int hsid, int partno, int vertno, int childjointno, int calcmode, int paintmode, int normalizeflag,
	float userrate, float directval )
{

	int ret;
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fuserrate;
	fuserrate = userrate / 100.0f;// % --> 数値

	if( fuserrate < 0.0f ){
		_ASSERT( 0 );
		fuserrate = 0.0f;
	}

	if( directval < 0.0f ){
		_ASSERT( 0 );
		directval = 0.0f;
	}


	ret = curhs->SetInfElem( partno, vertno, childjointno, calcmode, paintmode, normalizeflag, fuserrate, directval );
	if( ret ){
		DbgOut( "E3DSetInfElem : curhs SetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EASY3D_API int E3DDeleteInfElem( int hsid, int partno, int vertno, int childjointno, int normalizeflag )
{
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


EASY3D_API int E3DSetInfElemDefault( int hsid, int partno, int vertno )
{
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


EASY3D_API int E3DNormalizeInfElem( int hsid, int partno, int vertno )
{

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

EASY3D_API int E3DGetMidiMusicTime( int soundid, int* mtimeptr )
{

/***
	if( !s_HS ){
		DbgOut( "E3DGetMidiMusicTime : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int mtime;
	double rtime;
	ret = s_HS->GetMusicTime( soundid, &mtime, &rtime );
	if( ret ){
		DbgOut( "E3DGetMidiMusicTime : hs GetMusicTime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*mtimeptr = mtime;
***/
	*mtimeptr = 0;
	return 0;
}


EASY3D_API int E3DGetReferenceTime( int soundid, int* mtimeptr, double* reftimeptr )
{
/***
	if( !s_HS ){
		DbgOut( "E3DGetReferenceTime : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int mtime;
	double rtime;
	ret = s_HS->GetMusicTime( soundid, &mtime, &rtime );
	if( ret ){
		DbgOut( "E3DGetReferenceTime : hs GetMusicTime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*mtimeptr = mtime;
	*reftimeptr = rtime;
***/
	*mtimeptr = 0;
	*reftimeptr = 0.0;

	return 0;
}

EASY3D_API int E3DSetNextMotionFrameNo( int hsid, int motid, int nextmotid, int nextframeno, int befframeno )
{
//	nextframeno = code_getdi(0);
//	befframeno = code_getdi( -1 );

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNextMotionFrameNo( motid, nextmotid, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextMotionFrameNo : hs SetNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetLinearFogParams( int enable, E3DCOLOR4UC col, float start, float end, int srchsid )
{

//	int srchsid;
//	srchsid = code_getdi( -1 );

//////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}


	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* seths = 0;
	if( srchsid >= 0 ){
		seths = GetHandlerSet( srchsid );
		if( !seths ){
			DbgOut( "E3DSetLinearFogParams : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	
	if( seths == 0 ){
		if( enable ){
			g_fogenable = 1;
		}else{
			g_fogenable = 0;
		}
		g_fogcolor = D3DCOLOR_ARGB( 0, col.r, col.g, col.b );
		g_fogstart = start;
		g_fogend = end;
		g_fogtype = 0.0f;

		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pdev->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

		g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
		pdev->SetRenderState( D3DRS_FOGCOLOR, g_fogcolor );

		g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
		pdev->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );

		g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
		pdev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

		g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
		pdev->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );

		g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
		pdev->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );

		int hsidno;
		for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
			if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
				CHandlerSet* curhs = s_hsidelem[hsidno].hsptr;
				ret = curhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
				_ASSERT( !ret );
			}
		}

		if( g_bbhs ){
			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
			_ASSERT( !ret );

		}

	}else{

		int tmpfogenable;
		D3DCOLOR tmpfogcolor;
		float tmpfogstart, tmpfogend;

		if( enable ){
			tmpfogenable = 1;
		}else{
			tmpfogenable = 0;
		}
		tmpfogcolor = D3DCOLOR_ARGB( 0, col.r, col.g, col.b );
		tmpfogstart = start;
		tmpfogend = end;
		g_fogtype = 0.0f;

		ret = seths->SetRenderState( -1, D3DRS_FOGENABLE, tmpfogenable );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGCOLOR, tmpfogcolor );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&tmpfogstart)) );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&tmpfogend)) );
		_ASSERT( !ret );

	}

	return 0;
}

EASY3D_API int E3DSetToon0Params( int hsid, int partno, TOON0PARAMS toon0 )
{
//	float darkrate;
//	darkrate = (float)code_getdd( 0.7 );

//3 : brightrate
//	float brightrate;
//	brightrate = (float)code_getdd( 1.3 );

//4 : darkdeg
//	float darkdeg;
//	darkdeg = (float)code_getdd( 66.4 );

//5 : brightdeg
//	float brightdeg;
//	brightdeg = (float)code_getdd( 25.8 );

// 6 : partno
//	int partno;
//	partno = code_getdi( -1 );

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToonParams : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float darkcos, brightcos;
	darkcos = cosf( toon0.darkdeg * (float)DEG2PAI );
	if( darkcos < 0.0f )
		darkcos = 0.0f;

	brightcos = cosf( toon0.brightdeg * (float)DEG2PAI );
	if( brightcos < 0.0f )
		brightcos = 0.0f;


	int ret;
	ret = curhs->SetToon0Params( partno, toon0.darkrate, toon0.brightrate, darkcos, brightcos );
	if( ret ){
		DbgOut( "E3DSetToon0Params : hs SetToon0Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



EASY3D_API int E3DSetMovableAreaThread( const char* tempname1, float maxx, float maxz, int divx, int divz, float wallheight, int* threadidptr )
{
	char tempname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( tempname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSetMovableAreaThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DSetMovableAreaThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( tempname );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DSigLoadThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, tempname );

	s_thtable[thno].maxx = maxx;
	s_thtable[thno].maxz = maxz;
	s_thtable[thno].divx = divx;
	s_thtable[thno].divz = divz;
	s_thtable[thno].maxheight = wallheight;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadSetMovableArea, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DSetMovableAreaThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}

	*threadidptr = s_thtable[thno].threadid;

	return 0;
}


DWORD WINAPI ThreadSetMovableArea( LPVOID lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = SetMovableArea_F( curtable->filename, curtable->maxx, curtable->maxz, curtable->divx, curtable->divz, curtable->maxheight, &temphsid );
	if( ret ){
		DbgOut( "ThreadSetMovableArea : SetMovableArea_F error %s !!!\n", curtable->filename );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EASY3D_API int E3DEncodeBeta( const char* tempname, int* xorptr )
{
	int ret;
//////////////
	HANDLE hfile;
	hfile = CreateFile( (LPCTSTR)tempname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "E3DEncodeBeta : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	
	
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "E3DEncodeBeta :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "E3DEncodeBeta :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* newbuf;
	newbuf = (unsigned char*)malloc( sizeof( char ) * bufleng + 1 );//bufleng + 1
	if( !newbuf ){
		DbgOut( "E3DEncodeBeta : newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "E3DEncodeBeta : ReadFile error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}
	
	//////////////

	ret = XORFuncBeta( newbuf, bufleng, xorptr );
	if( ret ){
		DbgOut( "E3DEncodeBeta : XORFuncBeta error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}

	//////////////

	SetFilePointer( hfile, 0, NULL, FILE_BEGIN );
	if( GetLastError() != NO_ERROR ){
		DbgOut( "E3DEncodeBeta : SetFilePointer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	unsigned long writeleng;
	WriteFile( hfile, newbuf, bufleng, &writeleng, NULL );
	if( bufleng != writeleng ){
		DbgOut( "E3DEncodeBeta : WriteFile error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}

	FlushFileBuffers( hfile );
	SetEndOfFile( hfile );
	CloseHandle( hfile );	
	free( newbuf );

	return 0;
}

int XORFuncBeta( unsigned char* newbuf, int bufleng, int* xorptr )
{

	int xorindex = 0;
	int curpos;
	for( curpos = 0; curpos < bufleng; curpos++ ){
		unsigned char curuc;
		curuc = *(newbuf + curpos);
		curuc = curuc ^ (unsigned char)(*( xorptr + xorindex ));
		*(newbuf + curpos) = curuc;

		xorindex++;
		if( xorindex >= 10 ){
			xorindex = 0;
		}
	}
	return 0;
}

EASY3D_API int E3DDecodeBeta( const char* tempname, int* xorptr, char* bufptr)
{

	int ret;
	///////////////////

	HANDLE hfile;
	hfile = CreateFile( (LPCTSTR)tempname, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "E3DDecodeBeta : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	
	
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( hfile, &sizehigh );
	if( bufleng < 0 ){
		DbgOut( "E3DDecodeBeta :  GetFileSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		DbgOut( "E3DDecodeBeta :  file size too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* newbuf;
	newbuf = (unsigned char*)malloc( sizeof( char ) * bufleng + 1 );//bufleng + 1
	if( !newbuf ){
		DbgOut( "E3DDecodeBeta : newbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		DbgOut( "E3DDecodeBeta : ReadFile error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}
	
	//////////////

	ret = XORFuncBeta( newbuf, bufleng, xorptr );
	if( ret ){
		DbgOut( "E3DDecodeBeta : XORFuncBeta error !!!\n" );
		_ASSERT( 0 );
		free( newbuf );
		CloseHandle( hfile );
		return 1;
	}

	/////////////

	memcpy( bufptr, newbuf, bufleng );

	/////////////

	CloseHandle( hfile );	
	free( newbuf );


	return 0;
}

EASY3D_API int E3DChkVertexShader( int* chkvs )
{

	*chkvs = g_chkVS;

	return 0;
}

EASY3D_API int E3DLoadMOAFile( int hsid, const char* tempname1, int fillupleng, float mvmult )
{
	char moaname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( moaname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadMOAFile : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;

	if( fillupleng == 0 ){
		fillupleng = 1;
	}else if( fillupleng >= 60 ){
		fillupleng = 60;
	}
//////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLoadMAFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = curhs->LoadMAFile( moaname, fillupleng, s_hwnd, s_hMod, mvmult );
	if( ret ){
		DbgOut( "E3DLoadMAFile : hs LoadMAFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetNewPoseByMOA( int hsid, int eventno )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPoseByMOA : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetNewPoseByMOA( eventno );
	if( ret ){
		DbgOut( "E3DSetnewPoseByMOA : hs SetNewPoseByMOA error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMotionFrameNo( int hsid, int* motidptr, int* framenoptr )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	int dstmotkind, dstmotframe;
	ret = curhs->GetMotionFrameNo( &dstmotkind, &dstmotframe );
	if( ret ){
		DbgOut( "E3DGetMotionFrameNo : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*motidptr = dstmotkind;
	*framenoptr = dstmotframe;

	return 0;
}

EASY3D_API int E3DGetMoaInfo( int hsid, int maxnum, MOAINFO* moainfoptr, int* getnumptr )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DMoaInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum;
	ret = curhs->GetMoaInfo( maxnum, moainfoptr, &getnum );
	if( ret ){
		DbgOut( "E3DGetMoaInfo : curhs GetMoaInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*getnumptr = getnum;

	return 0;
}

EASY3D_API int E3DGetNextMotionFrameNo( int hsid, int motid, int* nextmotidptr, int* nextframeptr )
{

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DNextMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextMotionFrameNo( motid, nextmotidptr, nextframeptr );
	if( ret ){
		DbgOut( "E3DGetNextMotionFrameNo : curhs GetNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EASY3D_API int E3DEnableToonEdge( int hsid, int partno, int type, int flag )
{
//	int partno;
//	partno = code_getdi( -1 );//!!!!
	int ret;

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DEnableToonEdge : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->EnableToonEdge( partno, type, flag );
	if( ret ){
		DbgOut( "E3DEnableToonEdge : hs EnableToonEdge error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetToonEdge0Color( int hsid, int partno, const char* tempname1, E3DCOLOR4UC col )
{

	char materialname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( materialname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSetToonEdge0Color : materialname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;
//	int partno;
//	partno = code_getdi( -1 );//!!!!


	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToonEdge0Color : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToonEdge0Color( partno, materialname, col.r, col.g, col.b );
	if( ret ){
		DbgOut( "E3DSetToonEdge0Color : hs SetToonEdge0Color error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DSetToonEdge0Width( int hsid, int partno, const char* tempname1, float width )
{
	char materialname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( materialname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSetToonEdge0Width : materialname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;
//	int partno;
//	partno = code_getdi( -1 );//!!!!
//	float width;
//	width = (float)code_getdd(8.0);

/////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToonEdge0Width : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToonEdge0Width( partno, materialname, width );
	if( ret ){
		DbgOut( "E3DSetToonEdge0Width : hs SetToonEdge0Width error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetToonEdge0Color( int hsid, int partno, const char* tempname1, E3DCOLOR4UC* colptr )
{
	char materialname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( materialname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DGetToonEdge0Color : materialname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

//	int partno;
//	partno = code_getdi( -1 );//!!!!

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToonEdge0Color : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fr = 0.0f;
	float fg = 0.0f;
	float fb = 0.0f;
	ret = curhs->GetToonEdge0Color( partno, materialname, &fr, &fg, &fb );
	if( ret ){
		DbgOut( "E3DGetToonEdge0Color : hs GetToonEdge0Color error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = (int)( fr * 255.0f );
	colptr->g = (int)( fg * 255.0f );
	colptr->b = (int)( fb * 255.0f );

	return 0;
}
EASY3D_API int E3DGetToonEdge0Width( int hsid, int partno, const char* tempname1, float* widthptr )
{
	char materialname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( materialname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DGetToonEdge0Width : materialname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
/////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToonEdge0Width : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float dstw = 0.0f;
	ret = curhs->GetToonEdge0Width( partno, materialname, widthptr );
	if( ret ){
		DbgOut( "E3DGetToonEdge0Width : hs GetToonEdge0Width error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCreateParticle maxnum, texname, width, height, particleid
EASY3D_API int E3DCreateParticle( int maxnum, const char* tempname1, float width, float height, int blendmode, 
	int transparentflag, int cmpalways, int* particleidptr )
{
	char texname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( texname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DCreateParticle : texname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;

//	int maxnum;
//	maxnum = code_getdi(100);

//	int blendmode;
//	blendmode = code_getdi(2);

//	int transparentflag;
//	transparentflag = code_getdi(1);

//	int cmpalways;
//	cmpalways = code_getdi(0);

///////////////////////

	if( maxnum > MAX_BILLBOARD_NUM ){
		DbgOut( "E3DCreateParticle : maxnum too big error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = 0;
	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}


	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( texname, newhs );//!!!!!!


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3DCreateParticle : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	newhs->m_billboardflag = 2;// !!!!!

	int particleflag = 1;
	ret = newhs->CreateBillboard( particleflag, cmpalways );
	if( ret ){
		DbgOut( "E3DCreateParticle : newhs CreateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////////

	//SetResDir( tempname, newhs );//!!!!!!
	//int transparentflag = 1;
	int dirmode = 1;
	int orgflag = 1;

	int bbno;
	for( bbno = 0; bbno < maxnum; bbno++ ){
		int bbid = -1;
		ret = newhs->AddBillboard( texname, width, height, transparentflag, dirmode, orgflag, &bbid );
		if( ret || (bbid < 0) ){
			DbgOut( "E3DCreateParticle : newhs AddBillboard error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( bbid >= 0 ){
			//Create 3D Object
			ret = Restore( newhs, bbid );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = newhs->SetValidFlag( bbid, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}


	ret = newhs->SetBlendingMode( -1, blendmode );
	if( ret ){
		DbgOut( "e3dhsp : E3DCreateParitcle : hs SetBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


///////////////

	*particleidptr = newhs->serialno;

	return 0;
}

EASY3D_API int E3DDestroyParticle( int particleid )
{
///////

	int ret;
	ret = DestroyHandlerSet( particleid );
	if( ret ){
		DbgOut( "e3dhsp : E3DDestroyParticle : DestroyHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetParticlePos( int particleid, D3DXVECTOR3 pos )
{
//////////////////

	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticlePos : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticlePos( pos );
	if( ret ){
		DbgOut( "E3DSetParticlePos : hs SetParticlePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetParticleGravity( int particleid, float gravity ) 
{

	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleGravity : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleGravity( gravity );
	if( ret ){
		DbgOut( "E3DSetParticleGravity : hs SetParticleGravity error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetParticleLife( int particleid, float life )
{

	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleLife : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleLife( life );
	if( ret ){
		DbgOut( "E3DSetParticleLife : hs SetParticleLife error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetParticleEmitNum( int particleid, float emitnum )
{

	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleEmitNum : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleEmitNum( emitnum );
	if( ret ){
		DbgOut( "E3DSetParticleEmitNum : hs SetParticleEmitNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DSetParticleVel0 particleid, minvelx, minvely, minvelz, maxveclx, maxvely, maxvelz
EASY3D_API int E3DSetParticleVel0( int particleid, D3DXVECTOR3 minvel, D3DXVECTOR3 maxvel )
{
	//////////////////
	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleVel0 : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = curhs->SetParticleVel0( minvel, maxvel );
	if( ret ){
		DbgOut( "E3DSetParticleVel0 : hs SetParticleVel0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetParticleRotation( int particleid, float rotation )
{
	//////////////////
	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleRotation : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int rotkind = 0;
	ret = curhs->RotateBillboard( -1, rotation, rotkind );
	if( ret ){
		DbgOut( "E3DSetParticleRotation : bbhs RotateBillboard error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetParticleDiffuse( int particleid, E3DCOLOR4UC col )
{
	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleDiffuse : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int setflag = 0;
	ret = curhs->SetDispDiffuse( -1, col.r, col.g, col.b, setflag, -1, 1 );
	if( ret ){
		DbgOut( "e3dhsp : SetDiffuse : curhs SetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DRenderParticle( int scid, int particleid, int fps, int onlyupdate )
{
//////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRenderParticle : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DRenderParticle : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
		
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


	CalcMatView();


	ret = curhs->ChkInView( s_matView, 0, 0 );
	if( ret ){
		DbgOut( "E3DRenderParticle : ChkInView error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
//Transform

	ret = curhs->UpdateParticle( s_matView, &s_camerapos, &s_cameratarget, fps );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

//Render
	if( onlyupdate == 0 ){
		ret = curhs->Render( 0, pdev, 1, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

EASY3D_API int E3DSetParticleAlpha( int particleid, float mintime, float maxtime, float alpha )
{
	//////////////
	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleAlpha : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleAlpha( mintime, maxtime, alpha );
	if( ret ){
		DbgOut( "E3DSetParticleAlpha : hs SetParticleAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetParticleUVTile( int particleid, float mintime, float maxtime, int unum, int vnum, int tileno )
{

	//////////////
	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DSetParticleUVTile : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetParticleUVTile( mintime, maxtime, unum, vnum, tileno );
	if( ret ){
		DbgOut( "E3DSetParticleUVTile : hs SetParticleUVTile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DInitParticle( int particleid )
{

	//////////////
	CHandlerSet* curhs = GetHandlerSet( particleid );
	if( !curhs ){
		DbgOut( "E3DInitParticle : particleid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->InitParticle();
	if( ret ){
		DbgOut( "E3DInitParticle : hs InitParticle error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



EASY3D_API int E3DGetMotionIDByName( int hsid, const char* tempname1, int* motidptr )
{
	
	char motname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( motname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DGetMotionIDByName : motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

//////////



	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionIDByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int motid = -1;
	ret = curhs->GetMotionIDByName( motname, &motid );
	if( ret ){
		DbgOut( "E3DGetMotinIDByName : hs GetMotionIDByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*motidptr = motid;

	return 0;
}

EASY3D_API int E3DGetMotionNum( int hsid, int* numptr )
{

	int ret;
//////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int motnum =0;
	ret = curhs->GetMotionNum( &motnum );
	if( ret ){
		DbgOut( "E3DGetMotionNum : hs GetMotionNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = motnum;


	return 0;
}
EASY3D_API int E3DLoadSigFileAsGroundFromBuf( const char* tempname1, char* buf, int bufsize, int adjustuvflag, float mult, int* hsidptr )
{
//1

	char texdir[MAX_PATH];
	if( tempname1 ){
		strcpy_s( texdir, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadSigFileAsGroundFromBuf : texdir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
//////

	int temphsid = -1;
	ret = LoadSigFileAsGroundFromBuf_F( texdir, buf, bufsize, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadSigFileAsGroundFromBuf : E3DLoadSigFileAsGroundFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = temphsid;

	return 0;
}

int LoadSigFileAsGroundFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr )
{

	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}
	

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!


	newhs->m_TLmode = TLMODE_D3D;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!


	ret = newhs->LoadSigFileFromBuf( bufptr, bufsize, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : LoadSigFileAsGroundFromBuf_F : LoadSigFileFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : LoadSigFileAsGroundFromBuf_F : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	*hsidptr = newhs->serialno;


	return 0;
}

EASY3D_API int E3DGetCenterPos( int hsid, int partno, D3DXVECTOR3* posptr )
{
//	int partno;
//	partno = code_getdi(-1);

///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCenterPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 pos( 0.0f, 0.0f, 0.0f );
	ret = curhs->GetCenterPos( partno, &pos );
	if( ret ){
		DbgOut( "E3DGetCenterPos : hs GetCenterPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = pos;

	return 0;
}

EASY3D_API int E3DGetFaceNum( int hsid, int partno, int* numptr )
{
//	int partno;
//	partno = code_getdi(-1);
//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetFaceNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int facenum = 0;
	ret = curhs->GetFaceNum( partno, &facenum );
	if( ret ){
		DbgOut( "E3DGetFaceNum : hs GetFaceNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = facenum;

	return 0;
}

EASY3D_API int E3DGetFaceNormal( int hsid, int partno, int faceno, D3DXVECTOR3* nptr )
{
//	int partno;
//	partno = code_getdi(-1);

	
///////
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetFaceNormal : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVECTOR3 n( 0.0f, 0.0f, 0.0f );
	ret = curhs->GetFaceNormal( partno, faceno, &n );
	if( ret ){
		DbgOut( "E3DGetFaceNormal : hs GetFaceNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*nptr = n;

	return 0;
}

EASY3D_API int E3DCreateEmptyMotion( int hsid, const char* tempname1, int frameleng, int* motidptr, int zatype )
{
	char motname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( motname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DCreateEmptyMotion : motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	int ret;

/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateEmptyMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

EnterCriticalSection( &(curhs->m_crit_addmotion) );//######## start crit
	
	int tempmotid = -1;
	ret = curhs->CreateEmptyMotion( motname, frameleng, &tempmotid, zatype );
	if( ret ){
		DbgOut( "E3DCreateEmptyMotion : hs CreateEmptyMotion error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &(curhs->m_crit_addmotion) );//###### end crit
		return 1;
	}

	*motidptr = tempmotid;

LeaveCriticalSection( &(curhs->m_crit_addmotion) );//###### end crit

	return 0;
}

EASY3D_API int E3DSetTextureMinMagFilter( int scid, int hsid, int partno, int minfilter, int magfilter )
{
//	int partno = code_getdi(0);

//////////////
	CHandlerSet* curhs = 0;
	int bgflag = 0;

	if( hsid > 0 ){
		curhs = GetHandlerSet( hsid );
		if( !curhs ){
			DbgOut( "e3dhsp : E3DSetTextureMinMagFilter : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == -1 ){
		curhs = g_bbhs;
		if( !curhs ){
			DbgOut( "e3dhsp : E3DSetTextureMinMagFilter : billboard not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( hsid == 0 ){
		bgflag = 1;
	}


	int ret;
	if( curhs ){

		ret = curhs->SetTextureMinMagFilter( partno, minfilter, magfilter );
		if( ret ){
			DbgOut( "e3dhsp : E3DSetTextureMinMagFilter : curhs SetTextureMinMagFilter error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else if( bgflag == 0 ){
		
		CMySprite* cursprite;
		cursprite = GetSprite( partno );
		if( !cursprite ){
			DbgOut( "E3DSetTextureMinMagFilter : sprite id error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		cursprite->m_minfilter = minfilter;
		cursprite->m_magfilter = magfilter;


	}else if( bgflag == 1 ){
		BGDISPELEM* bgelem;
		bgelem = GetBGDispElem( scid );
		if( !bgelem ){
			DbgOut( "E3DSetTextureMinMagFilter : scid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		bgelem->bgdisp->m_minfilter = minfilter;
		bgelem->bgdisp->m_magfilter = magfilter;

	}
	return 0;
}

EASY3D_API int E3DGetMotionName( int hsid, int motid, char* motname )
{
//////////////
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMotionName( motname, motid );
	if( ret ){
		DbgOut( "E3DGetMotionName : hs GetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetMotionName( int hsid, int motid, const char* tempname1 )
{

	char motname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( motname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSetMotionName : motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionName( motid, motname );
	if( ret ){
		DbgOut( "E3DSetMotionName : hs SetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetMaterialNoByName( int hsid, const char* tempname1, int* matnoptr)
{

	char matname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( matname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DGetMaterialNoByName : matname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialNoByName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int matno = -3;
	ret = curhs->GetMaterialNoByName( matname, &matno );
	if( ret ){
		DbgOut( "E3DGetMaterialNoByName : hs GetMaterialNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*matnoptr = matno;

	return 0;
}
EASY3D_API int E3DGetMaterialAlpha( int hsid, int matno, float* alphaptr )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float falpha = 1.0f;
	ret = curhs->GetMaterialAlpha( matno, &falpha );
	if( ret ){
		DbgOut( "E3DGetMaterialAlpha : hs GetMaterialAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*alphaptr = falpha;

	return 0;
}
EASY3D_API int E3DGetMaterialDiffuse( int hsid, int matno, E3DCOLOR4UC* colptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialDiffuse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialDiffuse( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialDiffuse : hs GetMaterialDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}
EASY3D_API int E3DGetMaterialSpecular( int hsid, int matno, E3DCOLOR4UC* colptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialSpecular : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialSpecular( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialSpecular : hs GetMaterialSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}
EASY3D_API int E3DGetMaterialAmbient( int hsid, int matno, E3DCOLOR4UC* colptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialAmbient : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialAmbient( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialAmbient : hs GetMaterialAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}
EASY3D_API int E3DGetMaterialEmissive( int hsid, int matno, E3DCOLOR4UC* colptr )
{

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialEmissive : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	ret = curhs->GetMaterialEmissive( matno, &r, &g, &b );
	if( ret ){
		DbgOut( "E3DGetMaterialEmissive : hs GetMaterialEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}
EASY3D_API int E3DGetMaterialPower( int hsid, int matno, float* powptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialPower : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMaterialPower( matno, powptr );
	if( ret ){
		DbgOut( "E3DGetMaterialPower : hs GetMaterialPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EASY3D_API int E3DSetMaterialAlpha( int hsid, int matno, float alpha )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialAlpha( matno, alpha );
	if( ret ){
		DbgOut( "E3DSetMaterialAlpha : hs SetMaterialAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMaterialDiffuse( int hsid, int matno, int setflag, E3DCOLOR4UC col )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialDiffuse( matno, setflag, col.r, col.g, col.b );
	if( ret ){
		DbgOut( "E3DSetMaterialDiffuse : hs SetMaterialDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMaterialSpecular( int hsid, int matno, int setflag, E3DCOLOR4UC col )
{

	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialSpecular : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialSpecular( matno, setflag, col.r, col.g, col.b );
	if( ret ){
		DbgOut( "E3DSetMaterialSpecular : hs SetMaterialSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMaterialAmbient( int hsid, int matno, int setflag, E3DCOLOR4UC col )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAmbient : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialAmbient( matno, setflag, col.r, col.g, col.b );
	if( ret ){
		DbgOut( "E3DSetMaterialAmbient : hs SetMaterialAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetMaterialEmissive( int hsid, int matno, int setflag, E3DCOLOR4UC col )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialEmissive : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialEmissive( matno, setflag, col.r, col.g, col.b );
	if( ret ){
		DbgOut( "E3DSetMaterialEmissive : hs SetMaterialEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetMaterialPower( int hsid, int matno, int setflag, float power )
{

	int ret;

///////////////////

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialPower : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialPower( matno, setflag, power );
	if( ret ){
		DbgOut( "E3DSetMaterialPower : hs SetMaterialPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DGetTextureFromMaterial( int hsid, int matno, int* texidptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetTextureFromMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	int texid = -1;
	ret = curhs->GetTextureFromMaterial( matno, &texid );
	if( ret ){
		DbgOut( "E3DGetTextureFromMaterial : hs GetTextureFromMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*texidptr = texid;

	return 0;
}
EASY3D_API int E3DSetTextureToMaterial( int hsid, int matno, int texid )
{
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DSetTextureToMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetTextureToMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	
	int texindex;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DSetTextureToMaterial : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int textype;
	textype = g_texbnk->GetType( texindex );
	if( textype != TEXTYPE_TEXTURE ){
		DbgOut( "E3DSetTextureToMaterial : this is not texture %d error !!!\n", textype );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetTextureToMaterial( matno, texid );
	if( ret ){
		DbgOut( "E3DSetTextureToMaterial : hs SetTextureToMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMaterialBlendingMode( int hsid, int matno, int* modeptr )
{
	int ret;
//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialBlendingMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int mode = 0;
	ret = curhs->GetMaterialBlendingMode( matno, &mode );
	if( ret ){
		DbgOut( "E3DGetMaterialBlendingMode : hs GetMaterialBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*modeptr = mode;
	return 0;
}
EASY3D_API int E3DSetMaterialBlendingMode( int hsid, int matno, int mode )
{
	int ret;
//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialBlendingMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialBlendingMode( matno, mode );
	if( ret ){
		DbgOut( "E3DSetMaterialBlendingMode : hs SetMaterialBlendingMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMaterialNo( int hsid, int partno, int faceno, int* matnoptr )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int matno = -1;
	ret = curhs->GetMaterialNo( partno, faceno, &matno );
	if( ret ){
		DbgOut( "E3DGetMaterialNo : hs GetMaterialNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*matnoptr = matno;

	return 0;
}


EASY3D_API int E3DSetMotionKindML( int hsid, int motid, int* listptr, int* notlistptr )
{
	int ret;
//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionKindML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionKindML( motid, listptr, notlistptr );
	if( ret ){
		DbgOut( "E3DSetMotionKindML : hs SetMotionKindML error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMotionFrameNoML( int hsid, int motid, int frameno, int* listptr, int* notlistptr )
{
	int ret;
//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMotionFrameNoML( motid, frameno, listptr, notlistptr );
	if( ret ){
		DbgOut( "E3DSetMotionFrameNoML : hs SetMotionFrameNoML error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetNewPoseML( int hsid )
{
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewPoseML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNewPoseML();
	if( ret ){
		DbgOut( "E3DSetNewPoseML : hs SetNewPoseML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOAEventNoML( int hsid, int eventno, int* listptr, int* notlistptr )
{

	int ret;
//////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOAEventNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetNewPoseByMOAML( eventno, listptr, notlistptr );
	if( ret ){
		DbgOut( "E3DSetMOAEventNoML : hs SetNewPoseByMOAML error!!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetNextMotionFrameNoML( int hsid, int motid, int nextmotid, int nextframe, int befframe, int* listptr, int* notlistptr)
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetNextMotionFrameNoML( motid, nextmotid, nextframe, befframe, listptr, notlistptr );
	if( ret ){
		DbgOut( "E3DSetNextMotionFrameNoML : hs SetNextMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMotionFrameNoML( int hsid, int boneno, int* motidptr, int* framenoptr )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;

	int dstmotkind, dstmotframe;
	ret = curhs->GetMotionFrameNoML( boneno, &dstmotkind, &dstmotframe );
	if( ret ){
		DbgOut( "E3DGetMotionFrameNoML : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*motidptr = dstmotkind;
	*framenoptr = dstmotframe;

	return 0;
}
EASY3D_API int E3DGetNextMotionFrameNoML( int hsid, int boneno, int motid, int* motidptr, int* framenoptr )
{

//////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DNextMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextMotionFrameNoML( boneno, motid, motidptr, framenoptr );
	if( ret ){
		DbgOut( "E3DGetNextMotionFrameNoML : curhs GetNextMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DGetCurrentBonePos( int hsid, int boneno, int poskind, D3DXVECTOR3* posptr )
{
///////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "e3dhsp : E3DGetCurrentBonePos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	D3DXVECTOR3 dstpos;

	ret = curhs->GetCurrentBonePos( boneno, poskind, &dstpos );
	if( ret ){
		DbgOut( "e3dhsp : E3DGetCurrentBonePos : curhs GetCurrentBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = dstpos;

	return 0;
}
EASY3D_API int E3DGetCurrentBoneQ( int hsid, int boneno, int kind, int qid )
{
/////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCurrentBoneQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !s_qh ){
		DbgOut( "E3DGetCurrentBoneQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CQuaternion dstq;
	ret = curhs->GetCurrentBoneQ( boneno, kind, &dstq );
	if( ret ){
		DbgOut( "E3DGetCurrentBoneQ : hs GetCurrentBoneQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CQuaternion2 setq;
	setq.CopyFromCQuaternion( &dstq );//!!!!!!!!

	ret = s_qh->SetQ( qid, &setq );
	if( ret ){
		DbgOut( "E3DGetCurrentBoneQ : qh SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DChkBumpMapEnable( int* flagptr )
{
	*flagptr = g_bumpEnable;

	return 0;
}

EASY3D_API int E3DEnableBumpMap( int flag )
{

	if( flag != 0 ){
		if( g_bumpEnable != 0 ){
			g_bumpUse = 1;
		}else{
			g_bumpUse = 0;
		}
	}else{
		g_bumpUse = 0;
	}

	return 0;
}

EASY3D_API int E3DConvHeight2NormalMap( const char* tempname1, const char* tempname2, float hparam )
{
	char hname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( hname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DConvHeight2NormalMap : hname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char nname[MAX_PATH];
	if( tempname2 ){
		strcpy_s( nname, MAX_PATH, tempname2 );
	}else{
		DbgOut( "E3DConvHeight2NormalMap : nname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
//	float hparam;
//	hparam = (float)code_getdd(1.0);

//////////////////////

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		DbgOut( "E3DConvHeight2NormalMap : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DTEXTURE9 phmap = NULL;
	HRESULT hr;

	hr = D3DXCreateTextureFromFileEx( pdev, (LPTSTR)hname, 
							D3DX_DEFAULT, D3DX_DEFAULT, 
							1,//!!!!!!! 
							0, D3DFMT_A8R8G8B8, 
							D3DPOOL_MANAGED, 
							D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 
							//D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
							g_mipfilter,//!!!!!!! 
							0, NULL, NULL, &phmap );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : height map texture create error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DSURFACE_DESC desc;
	hr = phmap->GetLevelDesc( 0, &desc );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : hmap GetLevelDesc error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		return 1;
	}


	LPDIRECT3DTEXTURE9 pnmap = NULL;
	hr = D3DXCreateTexture( pdev, desc.Width, desc.Height, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pnmap );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : normal map texture create error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		return 1;
	}

	_ASSERT( phmap );
	_ASSERT( pnmap );

	hr = D3DXComputeNormalMap( pnmap, phmap, NULL, 0, D3DX_CHANNEL_RED, hparam );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : normal map convert error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		pnmap->Release();
		return 1;
	}

	hr = D3DXSaveTextureToFile( (LPTSTR)nname,
			D3DXIFF_BMP,
			(LPDIRECT3DBASETEXTURE9)pnmap,
			NULL );
	if( hr != D3D_OK ){
		DbgOut( "E3DConvHeight2NormalMap : normal map write error !!!\n" );
		_ASSERT( 0 );
		phmap->Release();
		pnmap->Release();
		return 1;
	}
		

	phmap->Release();
	pnmap->Release();

	return 0;
}
EASY3D_API int E3DSetBumpMapToMaterial( int hsid, int matno, int texid )
{
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DSetBumpMapToMaterial : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBumpMapToMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	
	int texindex;
	int textype;
	if( texid >= 0 ){
		texindex = g_texbnk->GetIndexBySerial( texid );
		if( texindex < 0 ){
			DbgOut( "E3DSetBumpMapToMaterial : texid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		textype = g_texbnk->GetType( texindex );
		if( textype != TEXTYPE_TEXTURE ){
			DbgOut( "E3DSetBumpMapToMaterial : this is not texture %d error !!!\n", textype );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = curhs->SetBumpMapToMaterial( matno, texid );
	if( ret ){
		DbgOut( "E3DSetBumpMapToMaterial : hs SetBumpMapToMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMOATrunkInfo( int hsid, int motid, MOATRUNKINFO* trunkptr )
{
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOATrunkInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOATrunkInfo( motid, trunkptr );
	if( ret ){
		DbgOut( "E3DGetMOATrunkInfo : hs GetMOATrunkInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOABranchInfo( int hsid, int motid, MOABRANCHINFO* branchptr, int arrayleng, int* getnumptr )
{

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOABranchInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->GetMOABranchInfo( motid, branchptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetMOABranchInfo : hs GetMOABranchInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOABranchFrame1( int hsid, int tmotid, int bmotid, int frameno1 )
{
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOABranchFrame1 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->SetMOABranchFrame1( tmotid, bmotid, frameno1 );
	if( ret ){
		DbgOut( "E3DSetMOABranchFrame1 : hs SetMOABranchFrame1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOABranchFrame2( int hsid, int tmotid, int bmotid, int frameno2 )
{
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOABranchFrame2 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int getnum = 0;
	ret = curhs->SetMOABranchFrame2( tmotid, bmotid, frameno2 );
	if( ret ){
		DbgOut( "E3DSetMOABranchFrame2 : hs SetMOABranchFrame2 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DEnablePhongEdge( int hsid, int flag )
{
//////////
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DEnablePhongEdge : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->EnablePhongEdge0( flag );
	if( ret ){
		DbgOut( "E3DEnablePhongEdge : hs EnablePhongEdge0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetPhongEdge0Params( int hsid, E3DCOLOR4UC col, float width, int blendmode, float alpha )
{
//	int blendmode;
//	blendmode = code_getdi(3);

//	float alpha;
//	alpha = (float)code_getdd(1.0);

///////////
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPhongEdge0Params : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetPhongEdge0Params( col.r, col.g, col.b, width, blendmode, alpha );
	if( ret ){
		DbgOut( "E3DSetPhongEdge0Params : hs SetPhongEdge0Params error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DFreeThread( int threadid )
{
	if( (threadid < 0) || (threadid >= THMAX) ){
		DbgOut( "E3DFreeThread : threadid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = FreeThTableElem( threadid );
	if( ret ){
		DbgOut( "E3DFreeThread : FreeThTableElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DLoadSigFileAsGroundThread( const char* tempname1, int adjustuvflag, float mult, int* threadidptr )
{
	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadSigFileAsGroundThread : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



///////////////
	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadSigFileAsGroundThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( filename );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadSigFileAsGroundThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, filename );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadSigFileAsGround, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadSigFileAsGroundThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}

	*threadidptr = s_thtable[thno].threadid;


	return 0;
}
DWORD WINAPI ThreadLoadSigFileAsGround( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;	
	int temphsid = -1;
	ret = LoadSigFileAsGround_F( curtable->filename, curtable->adjustuvflag, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "ThreadLoadSigFileAsGround : LoadSigFileAsGround_F error %s !!!\n", curtable->filename );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}




EASY3D_API int E3DLoadSigFileAsGroundFromBufThread( const char* tempname1, char* buf, int bufsize, int adjustuvflag, float mult, int* threadidptr )
{

	char texdir[MAX_PATH];
	if( tempname1 ){
		strcpy_s( texdir, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : texdir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int thno;
	thno = GetFreeThTable();
	if( thno < 0 ){
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : cant create thread : thread num error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	s_thtable[thno].useflag = 1;


	s_thtable[thno].threadid = thno;
	s_thtable[thno].hThread = NULL;
	s_thtable[thno].returnval = -1;

	int leng;
	leng = (int)strlen( texdir );
	s_thtable[thno].filename = (char*)malloc( sizeof( char ) * ( leng + 1 ) );
	if( !(s_thtable[thno].filename) ){
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : filename alloc error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}
	strcpy_s( s_thtable[thno].filename, leng + 1, texdir );

	s_thtable[thno].mult = mult;
	s_thtable[thno].adjustuvflag = adjustuvflag;

	s_thtable[thno].bufptr = buf;
	s_thtable[thno].bufsize = bufsize;

	s_thtable[thno].hThread = BEGINTHREADEX(NULL, 0, ThreadLoadSigFileAsGroundFromBuf, 
		(void*)(s_thtable + thno),
		0, &(s_thtable[thno].dwId) );


	if( !s_thtable[thno].hThread ){
		DbgOut( "E3DLoadSigFileAsGroundFromBufThread : beginthreadex error !!!\n" );
		_ASSERT( 0 );
		s_thtable[thno].returnval = -1;
		return 1;
	}


	*threadidptr = s_thtable[thno].threadid;

	return 0;
}

DWORD WINAPI ThreadLoadSigFileAsGroundFromBuf( LPVOID	lpThreadParam )
{
	THREADTABLE* curtable;
	curtable = (THREADTABLE*)lpThreadParam;

	int ret;
	int temphsid = -1;
	ret = LoadSigFileAsGroundFromBuf_F( curtable->filename, curtable->bufptr, curtable->bufsize, curtable->adjustuvflag, curtable->mult, &temphsid );
	if( ret ){
		DbgOut( "ThreadLoadSigFileAsGroundFromBuf : LoadSigFileAsGroundFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		curtable->returnval = -1;
		return 0;
	}

	curtable->returnval = temphsid;

	return 0;
}

EASY3D_API int E3DSetLightIdOfBumpMap( int lid )
{

	CLightData* curl;
	curl = GetLightData( lid );

	if( !curl ){
		DbgOut( "E3DSetLightIdOfBumpMap : lightid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	g_LightOfBump[0] = lid;

//DbgOut( "check!!!: BumpLight %d\r\n", g_LightOfBump );

	return 0;
}

EASY3D_API int E3DSetLightId2OfBumpMap( int lid )
{
	CLightData* curl;
	curl = GetLightData( lid );

	if( !curl ){
		DbgOut( "E3DSetLightId2OfBumpMap : lightid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	g_LightOfBump[1] = lid;

//DbgOut( "check!!!: BumpLight %d\r\n", g_LightOfBump );

	return 0;
}
EASY3D_API int E3DSetLightId3OfBumpMap( int lid )
{
	CLightData* curl;
	curl = GetLightData( lid );

	if( !curl ){
		DbgOut( "E3DSetLightId3OfBumpMap : lightid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	g_LightOfBump[2] = lid;

//DbgOut( "check!!!: BumpLight %d\r\n", g_LightOfBump );

	return 0;

}



EASY3D_API int E3DCreateRenderTargetTexture( SIZE size, int srcfmt, int* scidptr, int* texidptr, int* okflagptr, int needz )
{
//	int srcfmt;
//	srcfmt = code_getdi( D3DFMT_A8R8G8B8 );

///////////////////////
	int ret;
	int okflag = 0;
	*okflagptr = okflag;
	int errorno = -1;
	*scidptr = errorno;
	*texidptr = errorno;

	///texture

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateRenderTargetTexture : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	if( !g_texbnk ){
		DbgOut( "E3DCreateRenderTargetTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


//////
	int newserial = -1;
	int newindex = -1;
	char usertexname[ MAX_PATH ];
	s_usertexcnt++;
	sprintf_s( usertexname, MAX_PATH, "RenderTarget_%d", s_usertexcnt );

EnterCriticalSection( &g_crit_restore );//######## start crit

	int transparent = 0;
	int pool = D3DPOOL_DEFAULT;

	newserial = g_texbnk->AddName( 0, usertexname, transparent, pool );
	if( newserial <= 0 ){
		DbgOut( "E3DCreateRenderTargetTexture : texbnk AddName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 0;
	}
		
	newindex = g_texbnk->FindName( usertexname, transparent );
	if( newindex < 0 ){
		DbgOut( "E3DCreateRenderTargetTexture : texbnk FindName error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 0;
	}

	ret = g_texbnk->SetFMT( newindex, srcfmt );
	_ASSERT( !ret );


	ret = g_texbnk->CreateTexData( newindex, pdev, size.cx, size.cy );
	if( ret ){
		DbgOut( "E3DCreateRenderTargetTexture : texbnk CreateTexData error !!!\n" );
		_ASSERT( 0 );
		LeaveCriticalSection( &g_crit_restore );//###### end crit
		return 0;
	}

LeaveCriticalSection( &g_crit_restore );//###### end crit


	//swapchain
	SWAPCHAINELEM* cursc = 0;
	cursc = GetFreeSwapChainElem();
	if( !cursc ){
		DbgOut( "E3DCreateRenderTargetTexture : GetFreeSwapChainElem NULL error : swap chain num overflow !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	ret = CreateSwapChainElem( s_swapchaincnt, cursc, 0, newserial, 0, needz );
	if( ret ){
		DbgOut( "E3DCreateRenderTargetTexture : CreateSwapChain error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	s_swapchaincnt++;

	///
	*okflagptr = 1;
	*texidptr = newserial;
	*scidptr = cursc->scid;

	return 0;
}
EASY3D_API int E3DDestroyRenderTargetTexture( int scid, int texid )
{

//swapchain
	if( scid <= 0 ){
		DbgOut( "E3DDestroyRenderTargetTexture : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		return 0;
	}

	int ret;
	ret = DestroySwapChainElem( cursc, 1 );
	if( ret ){
		DbgOut( "E3DDestroyRenderTargetTexture : DestroySwapChainElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//texbank
	if( !g_texbnk ){
		DbgOut( "E3DDestroyRenderTargetTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	char* texname;
	int transparent;

	ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
	if( ret ){
		DbgOut( "E3DDestroyRenderTargetTexture : texbank GetNameBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !texname ){
		DbgOut( "E3DDestroyRenderTargetTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texindex;
	texindex = g_texbnk->FindName( texname, transparent );
	if( texindex < 0 ){
		DbgOut( "E3DDestroyRenderTargetTexture : texbank FindName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->DestroyTexture( texindex );
	_ASSERT( !ret );


	return 0;
}
EASY3D_API int E3DSetDSFillUpMode( int hsid, int mode )
{
//	int mode;
//	mode = code_getdi(0);

	//////////////

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDSFillUpMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetDSFillUpMode( mode );
	if( ret ){
		DbgOut( "E3DSetDSFillUpMode : hs SetDSFillUpMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DSetTexFillUpMode( int hsid, int mode )
{

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetTexFillUpMode : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetTexFillUpMode( mode );
	if( ret ){
		DbgOut( "E3DSetTexFillUpMode : hs SetTexFillUpMode error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

EASY3D_API int E3DSetShadowMapCamera( D3DXVECTOR3 campos, D3DXVECTOR3 camtarget, D3DXVECTOR3 camupvec )
{
	D3DXMatrixLookAtLH( &s_matViewShadow, &campos, &camtarget, &camupvec );

	s_camposshadow = campos;
	s_camtarshadow = camtarget;
	s_camupshadow = camupvec;
	

	return 0;
}
EASY3D_API int E3DSetShadowMapProjOrtho( int rttexid, float pnear, float pfar, float volsize )
{
//	float pnear;
//	pnear = (float)code_getdd(1000.0);
//
//	float pfar;
//	pfar = (float)code_getdd(50000.0);
//
//	float volsize;
//	volsize = (float)code_getdd(3000.0);

//////////////////
	
	int ret;
	if( !g_texbnk ){
		DbgOut( "E3DSetShadowMapProjection : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( rttexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DSetShadowMapProjection : texbank GetTextureSizeBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	float aspect;
//	aspect = (float)width / (float)height;
//	float fovr;
//	fovr = fov * (float)DEG2PAI;
//	D3DXMatrixPerspectiveFovLH( &s_matProjShadow, fovr, aspect, pnear, pfar );


	float vw, vh;
	vw = volsize;
	vh = volsize * ((float)height / (float)width);
	D3DXMatrixOrthoLH( &s_matProjShadow, vw, vh, pnear, pfar );


	// 射影空間から、テクスチャーの空間に変換する
	float fOffsetX = 0.5f + (0.5f / (float)width);
	float fOffsetY = 0.5f + (0.5f / (float)height);
	s_matBiasShadow = D3DXMATRIX(	0.5f,     0.0f,     0.0f,   0.0f,
							0.0f,    -0.5f,     0.0f,   0.0f,
							0.0f,     0.0f,     0.0f,	0.0f,
							fOffsetX, fOffsetY, 0.0f,   1.0f );

//	s_matBiasShadow = D3DXMATRIX(	0.5f,     0.0f,     0.0f,   0.0f,
//							0.0f,    -0.5f,     0.0f,   0.0f,
//							0.0f,     0.0f,     0.0f,	0.0f,
//							0.5f,	  0.5f,     0.0f,   1.0f );
	
	return 0;
}


EASY3D_API int E3DRenderWithShadow( int scid, int rtscid, int rttexid, int* hsidarray, int num, int skipflag )
{

	int ret;

	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DRenderWithShadow : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_shadowUse == 0 ){
		ret = RenderNormalBatch( scid, hsidarray, num, 1, skipflag, 0 );
		_ASSERT( !ret );
		return ret;//!!!!!!!!!!!!!!!
	}



	if( !g_pD3DApp || !g_texbnk ){
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	ret = g_texbnk->GetTexDataBySerial( rttexid, &s_ShadowMapTex );
	if( ret || !s_ShadowMapTex ){
		DbgOut( "RenderWithShadow : texbank GetTexDataBySerial shadowmap error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hr = pdev->BeginScene();
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}


	SWAPCHAINELEM* tmpelem;
	tmpelem = GetSwapChainElem( scid );
	float aspect0 = (float)g_scsize.x / (float)g_scsize.y;


//render to shadowmap
	//rendertarget, zbuffer, viewport, clear
	ret = SetRenderTarget( pdev, rtscid, 1, 0 );
	if( ret ){
		DbgOut( "RenderWithShadow : SetRenderTarget pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();

	if( s_shadowmode == 0 ){

		ret = ChkInViewAndTransformBatch( pdev, hsidarray, num, 1, 1, 0 );
		if( ret ){
			DbgOut( "E3DRenderWithShadow : ChkInViewAndTransformBatch  shadow 0  error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CVec3List	cList;
		ret = SetPointListForShadow( &cList, aspect0, hsidarray, num );
		if( ret ){
			DbgOut( "E3DRenderWithShadow : SetPointListForShadow error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// 変換行列を求める
		g_sStatus.vEyePos = s_camerapos;
		D3DXVECTOR3 vdir = s_cameratarget - s_camerapos;
		D3DXVec3Normalize( &vdir, &vdir );
		g_sStatus.vViewDir = vdir;
		g_sStatus.fNearDist = g_proj_near;
		g_sStatus.vLightDir = s_lightdirshadow;
		CalcLispMatrix(&g_sStatus, &cList);
		s_matViewShadow = g_sStatus.matView;
		s_matProjShadow = g_sStatus.matProj;

		// 射影空間から、テクスチャーの空間に変換する
		float fOffsetX = 0.5f + (0.5f / (float)g_scsize.x);
		float fOffsetY = 0.5f + (0.5f / (float)g_scsize.y);
		s_matBiasShadow = D3DXMATRIX(	0.5f,     0.0f,     0.0f,   0.0f,
								0.0f,    -0.5f,     0.0f,   0.0f,
								0.0f,     0.0f,     0.0f,	0.0f,
								fOffsetX, fOffsetY, 0.0f,   1.0f );
	}

	//Shader Const
	ret = SetShaderConst( 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : SetShaderConst 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//chkinview & transform
	ret = ChkInViewAndTransformBatch( pdev, hsidarray, num, 0, 1, 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : ChkInViewAndTransformBatch  pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SetTempFogEnable( pdev, 0 );


	//withalpha 0, 1 まとめて。アルファテスト有り。
	ret = RenderShadowMap0( pdev, hsidarray, num );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : RenderShadowMap0  pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//render to screen
	//rendertarget, zbuffer, viewport, clear
	ret = SetRenderTarget( pdev, scid, 0, skipflag );
	if( ret ){
		DbgOut( "RenderWithShadow : SetRenderTarget pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = SetShaderConst( 0 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : SetShaderConst 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//chkinview & transform
	ret = ChkInViewAndTransformBatch( pdev, hsidarray, num, 1, 1, 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : ChkInViewAndTransformBatch  pass1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	RestoreTempFogEnable();


	//withalpha 0
	ret = RenderShadowMap1( pdev, hsidarray, num, 0 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : RenderShadowMap1 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( skipflag == 0 ){
		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}

	//withalpha 1
	ret = RenderShadowMap1( pdev, hsidarray, num, 1 );
	if( ret ){
		DbgOut( "E3DRenderWithShadow : RenderShadowMap1 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	hr = pdev->EndScene();
	_ASSERT( hr == D3D_OK );

	return 0;
}


int RenderNormalBatch( int scid, int* hsidptr, int num, int needchkinview, int skipflag, int* modeptr )
{

	int ret;
	if( !g_pD3DApp || !g_texbnk ){
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	hr = pdev->BeginScene();
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_rendercnt == 0 ){
		//スプライト描画でステートが不定になるのでここでの呼び出しが必要
		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
		pdev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = 0;
	}	
	g_rendercnt++;


	ret = SetRenderTarget( pdev, scid, 0, skipflag );
	if( ret ){
		DbgOut( "RenderNormalBatch : SetRenderTarget pass0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CalcMatView();

	ret = SetShaderConst( 0 );
	if( ret ){
		DbgOut( "RenderNormalBatch : SetShaderConst 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//chkinview & transform
	ret = ChkInViewAndTransformBatch( pdev, hsidptr, num, 1, needchkinview, 1 );
	if( ret ){
		DbgOut( "RenderNormalBatch : ChkInViewAndTransformBatch  pass1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	//withalpha 0
	ret = RenderBatch( pdev, hsidptr, num, 0, modeptr );
	if( ret ){
		DbgOut( "RenderNormalBatch : RenderShadowMap1 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( skipflag == 0 ){
		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}

	//withalpha 1
	ret = RenderBatch( pdev, hsidptr, num, 1, modeptr );
	if( ret ){
		DbgOut( "RenderNormalBatch : RenderShadowMap1 1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	hr = pdev->EndScene();
	_ASSERT( hr == D3D_OK );

	return 0;
}

int SetPointListForShadow( CVec3List* plist, float aspect, int* hsidptr, int num )
{
	// ビューボリュームを求める
	float fov = g_proj_fov * (float)DEG2PAI;
	CalcPointsOfViewBolume( plist, s_matView, fov, aspect, g_proj_near, g_proj_far );

	int ret;
	int i;
	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			curhs = g_bbhs;
			bbflag = 1;
		}else{
			curhs = GetHandlerSet( curhsid );
			if( curhs && (curhs->m_billboardflag) != 0 ){
				bbflag = 1;
			}
		}

		if( curhs ){
			ret = curhs->SetPointListForShadow( plist );
			if( ret ){
				DbgOut( "SetPointListForShadow : hs SetPointListForShadow error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int ChkInViewAndTransformBatch( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int passno, int needchkinview, int needtrans )
{
	int ret;
	int i;

	D3DXMATRIX curmatview;
	D3DXVECTOR3 curcampos;
	D3DXVECTOR3 curcamtar;
	if( passno == 0 ){
		curmatview = s_matViewShadow;
		if( s_shadowmode == 0 ){
			curcampos = g_sStatus.newpos;
			curcamtar = g_sStatus.newpos + g_sStatus.vLightDir;
		}else{
			curcampos = s_camposshadow;
			curcamtar = s_camtarshadow;
		}
	}else{
		curmatview = s_matView;
		curcampos = s_camerapos;
		curcamtar = s_cameratarget;
	}

	/***
	if( renderflag ){
		pdev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

		pdev->SetRenderState( D3DRS_ALPHAREF,        0x08 );
		g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

		pdev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

////////////////////////////
		pdev->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
		pdev->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
		pdev->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
		g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
		g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;
	}
	***/

	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			curhs = g_bbhs;
			bbflag = 1;
		}else{
			curhs = GetHandlerSet( curhsid );
			if( curhs && (curhs->m_billboardflag) != 0 ){
				bbflag = 1;
			}
		}

		if( curhs ){

			int inviewflag = 1;

			if( needchkinview ){
				if( passno == 0 ){
					ret = curhs->ChkInView( curmatview, &inviewflag, &s_matProjShadow );
				}else{
					ret = curhs->ChkInView( curmatview, &inviewflag, 0 );
				}
				if( ret ){
					DbgOut( "ChkInViewAndTransformBatch : hs ChkInView error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( needtrans && inviewflag ){
				if( bbflag == 0 ){
					ret = curhs->Transform( curcampos, pdev, curmatview, 1, 0 );
					if( ret ){
						DbgOut( "ChkInViewAndTransformBatch : hs Transform error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else if( bbflag != 0 ){
					ret = curhs->TransformBillboard( pdev, curmatview, &curcampos, &curcamtar );
					if( ret ){
						DbgOut( "ChkInViewAndTransformBatch : hs TransformBillboard  error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}		
			}

			/***
			if( renderflag && inviewflag ){
				ret = curhs->Render( 1, pdev, 0, s_matView, s_camerapos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
			***/
		}
	}

	return 0;
}

int SetRenderTarget( LPDIRECT3DDEVICE9 pdev, int scid, int shadowmapflag, int skipflag )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}


	//int ret;
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "SetRenderTarget : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//ret = g_pD3DApp->TestCooperative();
	//_ASSERT( !ret );


	HRESULT hres;
	hres = pdev->SetRenderTarget( 0, cursc->pBackBuffer );
	if( hres != D3D_OK ){
		DbgOut( "SetRenderTarget : 3ddev SetRenderTarget error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}
	hres = pdev->SetDepthStencilSurface( cursc->pZSurf );
	if( hres != D3D_OK ){
		DbgOut( "SetRenderTarget : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}

	D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Height = g_scsize.y;
    viewport.Width =  g_scsize.x;
    viewport.MaxZ = 1.0f;
    viewport.MinZ = 0.0f;

    hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "SetRenderTarget : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

    // Clear the viewport

	if( skipflag == 0 ){
		if( shadowmapflag == 0 ){
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
								 cursc->clearcol,
								 1.0f, 0L );
		}else{
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
								 0xFFFFFFFF,
								 1.0f, 0L );
		}
		if( hres != D3D_OK ){
			_ASSERT( 0 );
			DbgOut( "SetRenderTarget : Clear error !!!\n" );
			return 1;
		}
	}

	return 0;
}

int RenderShadowMap0( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num )
{
	int ret;
	int i;
	//HRESULT hres;


	if( g_rendercnt == 0 ){
		//スプライト描画でステートが不定になるのでここでの呼び出しが必要
		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;
		pdev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = 0;
	}
	g_rendercnt++;



	pdev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	g_renderstate[ D3DRS_ALPHATESTENABLE ] = TRUE;

	pdev->SetRenderState( D3DRS_ALPHAREF,        0x08 );
	g_renderstate[ D3DRS_ALPHAREF ] = 0x08;

	pdev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	g_renderstate[ D3DRS_ALPHAFUNC ] = D3DCMP_GREATEREQUAL;

	pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

////////////////////////////
	pdev->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
	pdev->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
	pdev->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
	g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
	g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
	g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			curhs = g_bbhs;
			bbflag = 1;
		}else{
			curhs = GetHandlerSet( curhsid );
		}

		if( curhs ){
			if( s_shadowmode == 0 ){
				ret = curhs->RenderShadowMap0( pdev, &s_matViewShadow, &s_matProjShadow, g_sStatus.newpos );
			}else{
				ret = curhs->RenderShadowMap0( pdev, &s_matViewShadow, &s_matProjShadow, s_camposshadow );
			}
			if( ret ){
				DbgOut( "RenderShadowMap0 : hs RenderShadowMap0 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int RenderShadowMap1( LPDIRECT3DDEVICE9 pdev, int* hsidptr, int num, int withalpha )
{
	int i;
	int ret;
	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );
		int bbflag = 0;

		CHandlerSet* curhs = 0;
		if( curhsid == -1 ){
			if( withalpha == 1 ){//!!!!!!!!!!!
				curhs = g_bbhs;
				bbflag = 1;
			}
		}else{
			curhs = GetHandlerSet( curhsid );
		}

		if( curhs ){
			ret = curhs->RenderShadowMap1( pdev, withalpha, &s_matView, &g_matProj, s_camerapos );
			if( ret ){
				DbgOut( "RenderShadowMap1 : hs RenderShadowMap1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int RenderBatch( LPDIRECT3DDEVICE9 pd3dDevice, int* hsidptr, int num, int withalpha, int* modeptr )
{

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_cop0 = D3DTOP_MODULATE;
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
	g_cop1 = D3DTOP_DISABLE;
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
	g_aop0 = D3DTOP_MODULATE;
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
	g_aop1 = D3DTOP_DISABLE;

	if( withalpha == 0 ){

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

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;

		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;

////////////////////////////
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
		g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
		g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;

	}

	int i;
	int ret;
	for( i = 0; i < num; i++ ){
		int curhsid;
		curhsid = *( hsidptr + i );

		int renderflag;
		if( modeptr == 0 ){
			renderflag = 1;
		}else{
			int curmode;
			curmode = *( modeptr + i );
			renderflag = ((withalpha == 0) && (curmode & 1)) || ((withalpha != 0) && (curmode & 2));
		}

		int bbflag = 0;
		if( renderflag ){

			CHandlerSet* curhs = 0;
			if( curhsid == -1 ){
				if( withalpha == 1 ){//!!!!!!!!!!!
					curhs = g_bbhs;
					bbflag = 1;
				}
			}else{
				curhs = GetHandlerSet( curhsid );
			}

			if( curhs ){			
				ret = curhs->Render( 1, pd3dDevice, withalpha, s_matView, s_camerapos );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}


	return 0;
}



EASY3D_API int E3DSetShadowBias( float bias )
{
	//s_shadowcoef[0] = (float)code_getdd(0.005);
	s_shadowcoef[0] = bias;

	return 0;
}

int SetTempFogEnable( LPDIRECT3DDEVICE9 pdev, int srcenable )
{
	int ret;

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			ret = s_hsidelem[hsidno].hsptr->SetTempFogEnable( srcenable );
			_ASSERT( !ret );
		}
	}

	if( g_bbhs ){
		ret = g_bbhs->SetTempFogEnable( srcenable );
		_ASSERT( !ret );
	}

	g_renderstate[D3DRS_FOGENABLE] = srcenable;
	pdev->SetRenderState( D3DRS_FOGENABLE, srcenable );


	return 0;
}
int RestoreTempFogEnable()
{
	int ret;

	int hsidno;
	for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
		if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
			ret = s_hsidelem[hsidno].hsptr->RestoreTempFogEnable();
			_ASSERT( !ret );
		}
	}

	if( g_bbhs ){
		ret = g_bbhs->RestoreTempFogEnable();
		_ASSERT( !ret );
	}

	return 0;
}


EASY3D_API int E3DCheckRTFormat( int srcfmt, int* okflagptr )
{
//	int srcfmt;
//	srcfmt = code_getdi( D3DFMT_A8R8G8B8 );
////////////


	int ret;
	int okflag = 0;
	ret = g_pD3DApp->CheckRTFormat( srcfmt, &okflag );
	if( ret ){
		DbgOut( "E3DCheckRTFormat : app CheckRTFormat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*okflagptr = okflag;

	return 0;
}

EASY3D_API int E3DChkShadowEnable( int* flagptr )
{

	*flagptr = g_shadowEnable;

	return 0;
}
EASY3D_API int E3DEnableShadow( int flag )
{
//	int flag;
//	flag = code_getdi(0);

//////////////

	if( flag != 0 ){
		if( g_shadowEnable != 0 ){
			g_shadowUse = 1;
		}else{
			g_shadowUse = 0;
		}
	}else{
		g_shadowUse = 0;
	}

	return 0;
}

EASY3D_API int E3DRenderBatch( int scid, int* hsidarray, int num, int needchkinview, int skipflag )
{

	int ret;
	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DRenderBatch : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = RenderNormalBatch( scid, hsidarray, num, needchkinview, skipflag, 0 );
	if( ret ){
		DbgOut( "E3DRenderBatch : RenderNormalBatch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetShadowDarkness( float darkness )
{
	//s_shadowcoef[1] = (float)code_getdd(1.0);
	s_shadowcoef[1] = darkness;

	return 0;
}

EASY3D_API int E3DSetShadowMapLightDir( D3DXVECTOR3 ldir )
{


	s_lightdirshadow = ldir;

	D3DXVec3Normalize( &s_lightdirshadow, &s_lightdirshadow );


	return 0;
}

EASY3D_API int E3DGlobalToLocal( int hsid, D3DXVECTOR3 srcvec, D3DXVECTOR3* dstvecptr )
{

//////////

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGlobalToLocal : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = curhs->GlobalToLocal( srcvec, dstvecptr );
	if( ret ){
		DbgOut( "E3DGlobalToLocal : hs GlobalToLocal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DLocalToGlobal( int hsid, D3DXVECTOR3 srcvec, D3DXVECTOR3* dstvecptr )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLocalToGlobal : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->LocalToGlobal( srcvec, dstvecptr );
	if( ret ){
		DbgOut( "E3DLocalToGlobal : hs LocalToGlobal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetShadowMapMode( int mode )
{

	//s_shadowmode = code_getdi(0);
	s_shadowmode = mode;

	return 0;
}

EASY3D_API int E3DRenderBatchMode( int scid, int* hsidarray, int num, int needchkinview, int skipflag, int* modeptr )
{

	int ret;
	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DRenderBatchMode : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = RenderNormalBatch( scid, hsidarray, num, needchkinview, skipflag, modeptr );
	if( ret ){
		DbgOut( "E3DRenderBatchMode : RenderNormalBatch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EASY3D_API int E3DLoadTextureFromBuf( int texid, char* buf, int bufsize, int transparent )
{
	int ret;


	if( bufsize <= 0 ){
		DbgOut( "E3DLoadTextureFromBuf : bufsize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (transparent < 0) || (transparent >= 3) ){
		transparent = 0;
	}

	if( !g_texbnk ){
		DbgOut( "E3DLoadTextureFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////
	int texindex = -1;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DLoadTextureFromBuf : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->SetBufData( texindex, buf, bufsize );
	if( ret ){
		DbgOut( "E3DLoadTextureFromBuf : texbank SetBufData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DCOLOR colkey;
	if( transparent == 1 ){
		colkey = D3DCOLOR_ARGB( 255, 0, 0, 0 );
	}else{
		colkey = 0;
	}

	ret = g_texbnk->LoadTextureFromBuf( texindex, colkey );
	if( ret ){
		DbgOut( "E3DLoadTextureFromBuf : texbank LoadTextureFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetShadowMapInterFlag( int hsid, int partno, int srcflag )
{

//	int partno;
//	partno = code_getdi(-1);

	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetShadowMpaInterFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetShadowInterFlag( partno, srcflag );
	if( ret ){
		DbgOut( "E3DSetShadowMapInterFlag : hs SetShadowInterFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DTransformBillboard()
{
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


		CalcMatView();

		ret = g_bbhs->ChkInView( s_matView, 0, 0 );
		if( ret ){
			DbgOut( "E3DRenderBillboard : ChkInView error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		ret = g_bbhs->TransformBillboard( pdev, s_matView, &s_camerapos, &s_cameratarget );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = g_bbhs->TransformBillboardDispData( s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

EASY3D_API int E3DSetMaterialAlphaTest( int hsid, int matno, int alphatest0, int alphaval0, int alphatest1, int alphaval1 )
{
//	int alphatest0;
//	alphatest0 = code_getdi(1);

//	int alphaval0;
//	alphaval0 = code_getdi(8);

//	int alphatest1;
//	alphatest1 = code_getdi(0);

//	int alphaval1;
//	alphaval1 = code_getdi(8);

/////////////////
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialAlphaTest : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	alphaval0 = min( 255, alphaval0 );
	alphaval0 = max( 0, alphaval0 );

	alphaval1 = min( 255, alphaval1 );
	alphaval1 = max( 0, alphaval1 );

	ret = curhs->SetMaterialAlphaTest( matno, alphatest0, alphaval0, alphatest1, alphaval1 );
	if( ret ){
		DbgOut( "E3DSetMaterialAlphaTest : hs SetMaterialAlphaTest error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
	
EASY3D_API int E3DGetMaterialAlphaTest( int hsid, int materialno, int* test0ptr, int* val0ptr, int* test1ptr, int* val1ptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialAlphaTest : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMaterialAlphaTest( materialno, test0ptr, val0ptr, test1ptr, val1ptr );
	if( ret ){
		DbgOut( "E3DGetMaterialAlphaTest : hs GetMaterialAlphaTest error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetVersion( int* verptr )
{

	*verptr = E3DVERSIONNO;

	return 0;
}

EASY3D_API int E3DGetMaterialNum( int hsid, int* numptr )
{
	
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMaterialNum( numptr );
	if( ret ){
		DbgOut( "E3DGetMaterialNum : hs GetMaterialNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMaterialInfo( int hsid, MATERIALINFO* infoptr, int arrayleng, int* getnumptr )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMaterialInfo( infoptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetMaterialInfo : hs GetMaterialInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetToon1MaterialNum( int hsid, int partno, int* numptr )
{
	*numptr = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToon1MaterialNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetToon1MaterialNum( partno, numptr );
	if( ret ){
		DbgOut( "E3DGetToon1MaterialNum : hs GetToon1MaterialNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetToon1MaterialInfo( int hsid, int partno, TOON1MATERIALINFO* infoptr, int arrayleng, int* getnumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToon1MaterialInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetToon1MaterialInfo( partno, infoptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetToon1MaterialInfo : hs GetToon1MaterialInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetJointInitialPos( int hsid, int jointno, D3DXVECTOR3 pos, int calcflag, int excludeflag )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetJointInitialPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetJointInitialPos( jointno, pos, calcflag, excludeflag );
	if( ret ){
		DbgOut( "E3DSetJointInitialPos : hs SetJointInitialPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


///////////////

EASY3D_API int E3DSetVertexDiffuse( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag )
{
	int updateflag = 1;

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

	int ret;
	ret = curhs->SetDispDiffuse( partno, col.r, col.g, col.b, setflag, vertno, updateflag );
	if( ret ){
		DbgOut( "e3dhsp : SetDiffuse : curhs SetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

EASY3D_API int E3DSetVertexSpecular( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag )
{

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

	int ret;
	ret = curhs->SetDispSpecular( partno, col.r, col.g, col.b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecular : curhs SetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;

}

EASY3D_API int E3DSetVertexAmbient( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag )
{

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

	int ret;
	ret = curhs->SetDispAmbient( partno, col.r, col.g, col.b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetAmbient : curhs SetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EASY3D_API int E3DSetVertexEmissive( int hsid, int partno, int vertno, E3DCOLOR4UC col, int setflag )
{

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

	int ret;
	ret = curhs->SetDispEmissive( partno, col.r, col.g, col.b, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetEmissive : curhs SetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}

EASY3D_API int E3DSetVertexSpecularPower( int hsid, int partno, int vertno, float power, int setflag )
{

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

	if( power < 0.0f )
		power = 0.0f;
	if( power > 100.0f )
		power = 100.0f;


	int ret;
	ret = curhs->SetDispSpecularPower( partno, power, setflag, vertno );
	if( ret ){
		DbgOut( "e3dhsp : SetSpecularPower : curhs SetSpecularPower error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	return 0;

}
EASY3D_API int E3DGetVertexDiffuse( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr )
{

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
	int r, g, b;
	ret = curhs->GetDiffuse( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetDiffuse : curhs GetDiffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}

EASY3D_API int E3DGetVertexAmbient( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr )
{

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
	int r, g, b;
	ret = curhs->GetAmbient( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetAmbient : curhs GetAmbient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}

EASY3D_API int E3DGetVertexSpecular( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr )
{

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
	int r, g, b;
	ret = curhs->GetSpecular( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetSpecular : curhs GetSpecular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}

EASY3D_API int E3DGetVertexEmissive( int hsid, int partno, int vertno, E3DCOLOR4UC* colptr )
{

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
	int r, g, b;
	ret = curhs->GetEmissive( partno, vertno, &r, &g, &b );
	if( ret ){
		DbgOut( "e3dhsp : GetEmissive : curhs GetEmissive error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->r = r;
	colptr->g = g;
	colptr->b = b;

	return 0;
}


EASY3D_API int E3DGetVertexSpecularPower( int hsid, int partno, int vertno, float* powerptr )
{

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

	*powerptr = fpow;

	return 0;
}


EASY3D_API int E3DCreateSkinMat( int hsid, int partno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateSkinMat : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateSkinMat( partno );
	if( ret ){
		DbgOut( "E3DCreateSkinMat : hs CreateSkinMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetSymInfElem( int hsid, int partno, int vertno, int symaxis, float symdist )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetSymInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetSymInfElem( partno, vertno, symaxis, symdist );
	if( ret ){
		DbgOut( "E3DSetSymInfElem : hs SetSymInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DUpdateSymInfElem( int hsid, int excludeflag )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DUpdateSymInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->UpdateSymInfElem( excludeflag );
	if( ret ){
		DbgOut( "E3DUpdateSymInfElem : hs UpdateSymInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCalcMLMotion( int hsid )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcMLMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcMLMotion();
	if( ret ){
		DbgOut( "E3DCalcMLMotion : hs CalcMLMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetCharaDir( int hsid, float degxz, float degy, D3DXVECTOR3* dstdir )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCharaDir : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float fDeg2Pai = (float)DEG2PAI;
	D3DXVECTOR3 orgdir;
	float vecx, vecy, vecz;
	vecx = sinf( degxz * fDeg2Pai ) * cosf( degy * fDeg2Pai );
	vecy = sinf( degy * fDeg2Pai );
	vecz = -cosf( degxz * fDeg2Pai ) * cosf( degy * fDeg2Pai );

	CalcMatView();

	D3DXMATRIX dirm;
	dirm = curhs->m_gpd.m_matWorld;
	dirm._41 = 0.0f;
	dirm._42 = 0.0f;
	dirm._43 = 0.0f;


	D3DXVECTOR3 aftvec;
	aftvec.x = dirm._11 * vecx + dirm._21 * vecy + dirm._31 * vecz;// + dirm._41;
	aftvec.y = dirm._12 * vecx + dirm._22 * vecy + dirm._32 * vecz;// + dirm._42;
	aftvec.z = dirm._13 * vecx + dirm._23 * vecy + dirm._33 * vecz;// + dirm._43;
	//wpa = matSet1._14*x + matSet1._24*y + matSet1._34*z + matSet1._44;
	
	DXVec3Normalize( dstdir, &aftvec );

	return 0;
}

EASY3D_API int E3DGetStrSizeByFontID( int fontid, const char* strmes, SIZE* sizeptr )
{
	int ret;

//////
	sizeptr->cx = 0;
	sizeptr->cy = 0;

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	CXFont* curfont;
	curfont = GetXFont( fontid );
	if( !curfont ){
		DbgOut( "E3DGetStrSizeByFontID : fontid error %d !!!\n", fontid );
		_ASSERT( 0 );
		return 1;
	}

	SIZE strsize = {0, 0};
	ret = curfont->CalcStrSize( (char*)strmes, sizeptr );
	if( ret ){
		DbgOut( "E3DGetStrSizeByFontID : curfont CalcStrSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetPresentInterval( int flag )
{
	if( flag == 0 ){
		g_presentinterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}else{
		g_presentinterval = D3DPRESENT_INTERVAL_ONE;
	}

	return 0;
}

EASY3D_API int E3DSetBillboardSize( int bbid, float width, float height, int dirmode, int orgflag )
{
	int ret;
	ret = g_bbhs->SetBillboardSize( bbid, width, height, dirmode, orgflag );
	if( ret ){
		DbgOut( "E3DSetBillboardSize : hs SetBillboardSize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetHeightFogParams( int enable, E3DCOLOR4UC col, float start, float end, int srchsid )
{

//	int srchsid;
//	srchsid = code_getdi( -1 );

//////////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!
	}


	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* seths = 0;
	if( srchsid >= 0 ){
		seths = GetHandlerSet( srchsid );
		if( !seths ){
			DbgOut( "E3DSetLinearFogParams : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	
	if( seths == 0 ){
		if( enable ){
			g_fogenable = 1;
		}else{
			g_fogenable = 0;
		}
		g_fogcolor = D3DCOLOR_ARGB( 0, col.r, col.g, col.b );
		g_fogstart = start;
		g_fogend = end;
		g_fogtype = 1.0f;

		g_renderstate[D3DRS_FOGENABLE] = g_fogenable;
		pdev->SetRenderState( D3DRS_FOGENABLE, g_fogenable );

		g_renderstate[D3DRS_FOGCOLOR] = g_fogcolor;
		pdev->SetRenderState( D3DRS_FOGCOLOR, g_fogcolor );

		g_renderstate[D3DRS_FOGVERTEXMODE] = D3DFOG_LINEAR;
		pdev->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );

		g_renderstate[D3DRS_FOGTABLEMODE] = D3DFOG_NONE;
		pdev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

		g_renderstate[D3DRS_FOGSTART] = *((DWORD*)(&g_fogstart));
		pdev->SetRenderState( D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );

		g_renderstate[D3DRS_FOGEND] = *((DWORD*)(&g_fogend));
		pdev->SetRenderState( D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );

		int hsidno;
		for( hsidno = 0; hsidno < HSIDMAX; hsidno++ ){
			if( s_hsidelem[hsidno].validflag && s_hsidelem[hsidno].hsptr ){
				CHandlerSet* curhs = s_hsidelem[hsidno].hsptr;
				ret = curhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
				_ASSERT( !ret );

				ret = curhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
				_ASSERT( !ret );
			}
		}

		if( g_bbhs ){
			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGENABLE, g_fogenable );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGCOLOR, g_fogcolor );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&g_fogstart)) );
			_ASSERT( !ret );

			ret = g_bbhs->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&g_fogend)) );
			_ASSERT( !ret );

		}

	}else{

		int tmpfogenable;
		D3DCOLOR tmpfogcolor;
		float tmpfogstart, tmpfogend;

		if( enable ){
			tmpfogenable = 1;
		}else{
			tmpfogenable = 0;
		}
		tmpfogcolor = D3DCOLOR_ARGB( 0, col.r, col.g, col.b );
		tmpfogstart = start;
		tmpfogend = end;
		g_fogtype = 1.0f;

		ret = seths->SetRenderState( -1, D3DRS_FOGENABLE, tmpfogenable );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGCOLOR, tmpfogcolor );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGSTART, *((DWORD*)(&tmpfogstart)) );
		_ASSERT( !ret );

		ret = seths->SetRenderState( -1, D3DRS_FOGEND, *((DWORD*)(&tmpfogend)) );
		_ASSERT( !ret );

	}

	return 0;
}
EASY3D_API int E3DSetTextureToBG( int scid, int stageno, int texid )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!
	}

	int ret;
	if( !g_texbnk ){
		DbgOut( "E3DSetTextureToBG : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int texindex;
	texindex = g_texbnk->GetIndexBySerial( texid );
	if( texindex < 0 ){
		DbgOut( "E3DSetTextureToBG : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int textype;
	textype = g_texbnk->GetType( texindex );
	if( textype != TEXTYPE_TEXTURE ){
		DbgOut( "E3DSetTextureToBG : this is not texture %d error !!!\n", textype );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		DbgOut( "E3DSetTextureToBG : device not set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	BGDISPELEM* bgelem;
	bgelem = GetBGDispElem( scid );
	if( bgelem ){
		if( bgelem->bgdisp != 0 ){
			char* texname;
			int transparent;
			ret = g_texbnk->GetNameBySerial( texid, &texname, &transparent );
			if( ret ){
				DbgOut( "E3DSetTextureToBG : texbank GetNameBySerial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			if( !texname ){
				DbgOut( "E3DSetTextureToBG : texid error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			
			if( stageno == 0 ){
				strcpy_s( bgelem->bgdisp->texname, _MAX_PATH, texname );
				bgelem->bgdisp->transparent1 = transparent;
			}else{
				strcpy_s( bgelem->bgdisp->texname2, _MAX_PATH, texname );
				bgelem->bgdisp->transparent2 = transparent;
			}

			ret = bgelem->bgdisp->CreateTexture( pdev );
			if( ret ){
				DbgOut( "E3DSetTextureToBG : bgdisp CreateTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}

	return 0;
}

EASY3D_API int E3DGetTextureFromBG( int scid, int stageno, int* texidptr )// $1E2
{
	*texidptr = -1;

	if( !g_texbnk ){
		DbgOut( "E3DGetTextureFromBG : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texid = -1;
	int texindex = -1;
	BGDISPELEM* bgelem;
	bgelem = GetBGDispElem( scid );
	if( bgelem && (bgelem->bgdisp != 0) ){
		if( stageno == 0 ){
			if( *(bgelem->bgdisp->texname) != 0 ){
				texindex = g_texbnk->FindName( bgelem->bgdisp->texname, bgelem->bgdisp->transparent1 );
				if( texindex >= 0 ){
					texid = g_texbnk->GetSerial( texindex );
				}else{
					texid = -1;
				}
			}else{
				texid = -1;
			}
		}else{
			if( *(bgelem->bgdisp->texname2) != 0 ){
				texindex = g_texbnk->FindName( bgelem->bgdisp->texname2, bgelem->bgdisp->transparent2 );
				if( texindex >= 0 ){
					texid = g_texbnk->GetSerial( texindex );
				}else{
					texid = -1;
				}
			}else{
				texid = -1;
			}
		}
	}else{
		texid = -1;
	}

	*texidptr = texid;

	return 0;
}

EASY3D_API int E3DChangeMoaIdling( int hsid, int motid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChangeMoaIdling : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( motid <= 0 ){
		DbgOut( "E3DChangeMoaIdling : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->ChangeMoaIdling( motid );
	if( ret ){
		DbgOut( "E3DChangeMoaIdling : hs ChangeMoaIdling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetMotionBlur( int hsid, int mode, int blurtime )
{
	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DSetMotionBlur : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( mode < 0 ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( mode >= BLUR_MAX ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( blurtime <= 0 ){
		blurtime = 1;
	}
	if( blurtime > BLURTIMEMAX ){
		blurtime = BLURTIMEMAX;
	}
	
	int ret;
	ret = curhs->SetMotionBlur( mode, blurtime );
	if( ret ){
		DbgOut( "E3DSetMotionBlur : hs SetMotionBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
EASY3D_API int E3DSetBeforeBlur( int hsid )
{
	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DSetBeforeBlur : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetBeforeBlur();
	if( ret ){
		DbgOut( "E3DSetBeforeBlur : hs SetBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DInitBeforeBlur( int hsid )
{

	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DInitBeforeBlur : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->InitBeforeBlur();
	if( ret ){
		DbgOut( "E3DInitBeforeBlur : hs InitBeforeBlur error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DRenderMotionBlur( int scid, int* hsidarray, int num )
{
	int ret;

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
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

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DRenderMotionBlur : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int hsidno;
	for( hsidno = 0; hsidno < num; hsidno++ ){
		int curhsid;
		curhsid = *( hsidarray + hsidno );

		CHandlerSet* curhs;
		if( curhsid != -1 ){
			curhs = GetHandlerSet( curhsid );
			if( !curhs ){
				DbgOut( "E3DRenderMotionBlur : hsid error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			curhs = g_bbhs;
			if( !curhs ){
				DbgOut( "E3DRenderMotionBlur : hsid bb error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = curhs->RenderMotionBlur( pdev );
		if( ret ){
			DbgOut( "E3DRenderMotionBlur : hs RenderMotionBlur error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;

}

EASY3D_API int E3DSetMotionBlurPart( int hsid, int mode, int blurtime, int* partptr, int num )
{
	CHandlerSet* curhs;
	if( hsid != -1 ){
		curhs = GetHandlerSet( hsid );
	}else{
		curhs = g_bbhs;
	}
	if( !curhs ){
		DbgOut( "E3DSetMotinBlurPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( mode < 0 ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( mode >= BLUR_MAX ){
		mode = BLUR_WORLD;//!!!!!
		_ASSERT( 0 );
	}

	if( blurtime <= 0 ){
		blurtime = 1;
	}
	if( blurtime > BLURTIMEMAX ){
		blurtime = BLURTIMEMAX;
	}

	int ret;
	ret = curhs->SetMotionBlurPart( mode, blurtime, partptr, num );
	if( ret ){
		DbgOut( "E3DSetMotionBlurPart : hs SetMotionBlurPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int RTBeginScene( LPDIRECT3DDEVICE9 pdev, int scid, int skipbeginscene, int needz )
{
	int ret;
	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "RTBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	HRESULT hres;
	hres = pdev->SetRenderTarget( 0, cursc->pBackBuffer );
	if( hres != D3D_OK ){
		DbgOut( "RTBeginScene : 3ddev SetRenderTarget error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}
	if( needz ){
		hres = pdev->SetDepthStencilSurface( cursc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		hres = pdev->SetDepthStencilSurface( NULL );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
	}

	D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Height = g_scsize.y;
    viewport.Width =  g_scsize.x;
    viewport.MaxZ = 1.0f;
    viewport.MinZ = 0.0f;

    hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "RTBeginScene : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( skipbeginscene == 0 ){
		if( FAILED( pdev->BeginScene() ) ){
			_ASSERT( 0 );
			return 1;
		}
	}

    // Clear the viewport
	if( needz ){
		hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
							 cursc->clearcol,
							 1.0f, 0L );
	}else{
		hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET,// | D3DCLEAR_ZBUFFER, 
							 cursc->clearcol,
							 1.0f, 0L );
	}
	if( hres != D3D_OK ){
		//_ASSERT( 0 );
	   DbgOut( "RTBeginScene : Clear error !!!\n" );
		return 1;
	}

	if( skipbeginscene == 0 ){
		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}

	return 0;
}

int SetRTRenderState( LPDIRECT3DDEVICE9 pdev, int needz )
{
	if( needz ){
		pdev->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
		pdev->SetRenderState( D3DRS_ZWRITEENABLE,  TRUE );
		pdev->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL );
		g_renderstate[ D3DRS_ZENABLE ] = D3DZB_TRUE;
		g_renderstate[ D3DRS_ZWRITEENABLE ] = TRUE;
		g_renderstate[ D3DRS_ZFUNC ] = D3DCMP_LESSEQUAL;
	}else{
		pdev->SetRenderState( D3DRS_ZENABLE, FALSE );
		g_renderstate[D3DRS_ZENABLE] = 0;	
	}


	if( g_cop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_cop0 = D3DTOP_MODULATE;
	}
	if( g_cop1 != D3DTOP_DISABLE ){
		pdev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
		g_cop1 = D3DTOP_DISABLE;
	}
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	if( g_aop0 != D3DTOP_MODULATE ){
		pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );//!!!!
		g_aop0 = D3DTOP_MODULATE;
	}
	if( g_aop1 != D3DTOP_DISABLE ){
		pdev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );//!!!!
		g_aop1 = D3DTOP_DISABLE;
	}

	return 0;
}


EASY3D_API int E3DBlurTexture16Box( int dstscid, int srctexid, int adjustuvflag )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTexture16Box : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlurTexture16Box : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTexture16Box : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTexture16Box : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_pPostEffect != NULL )
	{

		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBlur0 );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw;
			offsetv1 = 0.5f / screenh;
		}

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}
EASY3D_API int E3DBlurTexture9Corn( int dstscid, int srctexid, int adjustuvflag )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTexture9Corn : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;

	if( !g_texbnk ){
		DbgOut( "E3DBlurTexture9Corn : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTexture9Corn : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTexture9Corn : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBlur0 );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture9Corn :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture9Corn :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture9Corn :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------
		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw;
			offsetv1 = 0.5f / screenh;
		}

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}


int SetEffectDecl( LPDIRECT3DDEVICE9 pdev )
{
	D3DVERTEXELEMENT9 ve[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//tex1[2]//tex0
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		D3DDECL_END()

	};
	
	D3DVERTEXELEMENT9 vetlv[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//diffuse
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

		//tex0
		{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		D3DDECL_END()

	};


	if( s_effectdecl ){
		s_effectdecl->Release();
		s_effectdecl = 0;
	}
	if( s_tlvdecl ){
		s_tlvdecl->Release();
		s_tlvdecl = 0;
	}

	HRESULT hr;
	hr = pdev->CreateVertexDeclaration( ve, &s_effectdecl );
	if( hr != D3D_OK ){
		DbgOut( "SetEffectDecl : CreateVertexDeclaration effectdecl error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hr = pdev->CreateVertexDeclaration( vetlv, &s_tlvdecl );
	if( hr != D3D_OK ){
		DbgOut( "SetEffectDecl : CreateVertexDeclaration tlv error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DBlurTextureGauss5x5( int dstscid, int srctexid, float dispersion, int adjustuvflag )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss5x5 : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlurTextureGauss5x5 : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTextureGauss5x5 : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss5x5 : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBlur0 );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss5x5 :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss5x5 :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss5x5 :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetGauss5x5Weight( fw, fh, dispersion );
		if( ret ){
			DbgOut( "E3DBlurTextureGauss5x5 : SetGauss5x5Weight error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 2 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------
		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw;
			offsetv1 = 0.5f / screenh;
		}


		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}

int SetGauss5x5Weight( float fw, float fh, float dispersion )
{
	HRESULT hres;

	D3DXVECTOR4 avSampleWeight[16];
	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avSampleWeight, sizeof( D3DXVECTOR4 ) * 16 );
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
	
	float tu = 1.0f / fw;
    float tv = 1.0f / fh;

    float totalWeight = 0.0f;
    int index=0;
    for( int x = -2; x <= 2; x++ ) {
        for( int y = -2; y <= 2; y++ ) {
			// 係数が小さくなる部分は除去
            if( 2 < abs(x) + abs(y) ) continue;

            avTexCoordOffset[index] = D3DXVECTOR2( x * tu, y * tv );
			float fx = (FLOAT)x;
			float fy = (FLOAT)y;
			avSampleWeight[index].x = avSampleWeight[index].y =
			avSampleWeight[index].z = avSampleWeight[index].w
				= expf( -( fx * fx + fy * fy ) / ( 2.0f * dispersion ) );
            totalWeight += avSampleWeight[index].x;

            index++;
        }
    }

	// 重みの合計を 1.0f にする
	float divtotal = 1.0f / totalWeight;
	for( int i=0; i < index; i++ ){
		avSampleWeight[i] *= divtotal;
	}
	
	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleWeight, avSampleWeight, sizeof( D3DXVECTOR4 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetGauss5x5Weight : sampleweight SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetGauss5x5Weight : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int SetDownScale4x4Offset( float fw, float fh )
{
	HRESULT hres;
    float tU = 1.0f / fw;
    float tV = 1.0f / fh;

	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );


    // Sample from the 16 surrounding points. Since the center point will be in
    // the exact center of 16 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index=0;
    for( int y=0; y < 4; y++ )
    {
        for( int x=0; x < 4; x++ )
        {
            avTexCoordOffset[ index ].x = ( (float)x - 1.5f) * tU;
            avTexCoordOffset[ index ].y = ( (float)y - 1.5f) * tV;
                                                      
            index++;
        }
    }

	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetDownScale4x4Offset : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int SetDownScale2x2Offset( float fw, float fh )
{
	HRESULT hres;
    float tU = 1.0f / fw;
    float tV = 1.0f / fh;

	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );

    // Sample from the 4 surrounding points. Since the center point will be in
    // the exact center of 4 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index=0;
    for( int y=0; y < 2; y++ )
    {
        for( int x=0; x < 2; x++ )
        {
            avTexCoordOffset[ index ].x = ( (float)x - 0.5f ) * tU;
            avTexCoordOffset[ index ].y = ( (float)y - 0.5f ) * tV;
                                                      
            index++;
        }
    }

	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetDownScale2x2Offset : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int SetToneMapFirstOffset( float fw, float fh )
{
    // Initialize the sample offsets for the initial luminance pass.

	HRESULT hres;
	D3DXVECTOR2 avTexCoordOffset[16];
	ZeroMemory( avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		
	float tU, tV;
    tU = 1.0f / (3.0f * fw);
    tV = 1.0f / (3.0f * fh);
    
    int index = 0;
	int x, y;
    for( x = -1; x <= 1; x++ )
    {
        for( y = -1; y <= 1; y++ )
        {
            avTexCoordOffset[index].x = x * tU;
            avTexCoordOffset[index].y = y * tV;

            index++;
        }
    }

	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, avTexCoordOffset, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "SetToneMapFirstOffset : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}



int SetGaussWeight( float dispersion )
{
    DWORD i;
	float tbl[8];

    float total=0.0f;
    for( i=0; i<8; i++ ){
        float pos = 1.0f + 2.0f * (float)i;
        tbl[i] = expf( -0.5f * (float)( pos * pos ) / dispersion );
        total += 2.0f * tbl[i];
    }
    // 規格化
	for( i=0; i<8; i++ ){
		tbl[i] /= total;
	}

	if( g_pPostEffect){
		HRESULT hres;
		hres = g_pPostEffect->SetValue( g_hpeWeight, tbl, sizeof( float ) * 8 );
		if( hres != D3D_OK ){
			DbgOut( "SetGaussWeight : posteffect setvalue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

EASY3D_API int E3DBlurTextureGauss( int finalscid, int workscid, int worktexid, int srctexid, float dispersion, int adjustuvflag )
{
	int ret;
	HRESULT hres;

	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}



	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

/////////////
// １パス目
	ret = RTBeginScene( pdev, workscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss : RTBeginScene workscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw1, screenh1;
	screenw1 = (float)g_scsize.x;
	screenh1 = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlurTextureGauss : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBlurTextureGauss : srctexid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBlurTextureGauss : GetTextureInfoBySerial srctexid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;

		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectGauss );
		_ASSERT( hres == D3D_OK );

		ret = SetGaussWeight( dispersion );
		_ASSERT( !ret );


		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		D3DXVECTOR2 offsetX;
		offsetX.x = 16.0f / fw;
		offsetX.y = 0.0f / fh;
		hres = g_pPostEffect->SetValue( g_hpeOffsetX, &offsetX, sizeof( float ) * 2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :offsetX SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		float colmult = 1.0f;
		hres = g_pPostEffect->SetValue( g_hpeColMult, &colmult, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :ColMult SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );


		float offsetu1, offsetv1;
		if( adjustuvflag == 0 ){
			offsetu1 = 0.0f;
			offsetv1 = 0.0f;

		}else if( adjustuvflag == 1 ){
			offsetu1 = 0.5f / screenw1;
			offsetv1 = 0.5f / screenh1;
		}
		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 0.0f + offsetv1},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu1, 1.0f + offsetv1},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu1, 1.0f + offsetv1}
		};

		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

/////////////////////
// ２パス目
		ret = RTBeginScene( pdev, finalscid, 1, 0 );
		if( ret ){
			DbgOut( "E3DBlurTextureGauss : RTBeginScene finalscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float screenw2, screenh2;
		screenw2 = (float)g_scsize.x;
		screenh2 = (float)g_scsize.y;


		LPDIRECT3DTEXTURE9 ptex2 = 0;
		ret = g_texbnk->GetTexDataBySerial( worktexid, &ptex2 );
		if( ret || !ptex2 ){
			DbgOut( "E3DBlurTextureGauss : worktexid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		int width2, height2, pool2, transparent2, format2;
		ret = g_texbnk->GetTextureInfoBySerial( worktexid, &width2, &height2, &pool2, &transparent2, &format2 );
		if( ret ){
			DbgOut( "E3DBlurTextureGauss : GetTextureInfoBySerial worktexid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float fw2 = (float)width2;
		float fh2 = (float)height2;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw2, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :width0 2 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh2, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :height0 2 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :texture0 2 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		D3DXVECTOR2 offsetY;
		offsetY.x = 0.0f / fw;
		offsetY.y = 16.0f / fh;
		hres = g_pPostEffect->SetValue( g_hpeOffsetY, &offsetY, sizeof( float ) * 2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTextureGauss :offsetY SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );


		float offsetu2, offsetv2;
		if( adjustuvflag == 0 ){
			offsetu2 = 0.0f;
			offsetv2 = 0.0f;
		}else if( adjustuvflag == 1 ){
			offsetu2 = 0.5f / screenw2;
			offsetv2 = 0.5f / screenh2;
		}
		TVERTEX Vertex2[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu2, 0.0f + offsetv2},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu2, 0.0f + offsetv2},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu2, 1.0f + offsetv2},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu2, 1.0f + offsetv2}
		};


		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex2, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

/////////////


		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}

EASY3D_API int E3DBlendRTTexture( int finalscid, int texid1, E3DCOLOR4F col1, int texf1,
	int texid2, E3DCOLOR4F col2, int texf2, int blendop, int srcblend, int destblend, int adjustuvflag )
{

	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	int needz = 0;
	ret = RTBeginScene( pdev, finalscid, 0, needz );
	if( ret ){
		DbgOut( "E3DBlendRTTexture : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw1, screenh1;
	screenw1 = (float)g_scsize.x;
	screenh1 = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlendRTTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DTEXTURE9 ptex1 = 0;
	int width1, height1, pool1, transparent1, format1;
	if( texid1 >= 0 ){
		ret = g_texbnk->GetTexDataBySerial( texid1, &ptex1 );
		if( ret || !ptex1 ){
			DbgOut( "E3DBlendRTTexture : texid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = g_texbnk->GetTextureInfoBySerial( texid1, &width1, &height1, &pool1, &transparent1, &format1 );
		if( ret ){
			DbgOut( "E3DBlendRTTexture : GetTextureInfoBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		screenw1 = (float)width1;
//		screenh1 = (float)height1;

	}


	LPDIRECT3DTEXTURE9 ptex2 = 0;
	int width2, height2, pool2, transparent2, format2;
	if( texid2 >= 0 ){
		ret = g_texbnk->GetTexDataBySerial( texid2, &ptex2 );
		if( ret || !ptex2 ){
			DbgOut( "E3DBlendRTTexture : texid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = g_texbnk->GetTextureInfoBySerial( texid2, &width2, &height2, &pool2, &transparent2, &format2 );
		if( ret ){
			DbgOut( "E3DBlendRTTexture : GetTextureInfoBySerial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, needz );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;
		
		pdev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		g_renderstate[ D3DRS_BLENDOP ] = D3DBLENDOP_ADD;

//		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_SRCALPHA;
//
//		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_INVSRCALPHA;

		pdev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		g_renderstate[ D3DRS_SRCBLEND ] = D3DBLEND_ONE;

		pdev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		g_renderstate[ D3DRS_DESTBLEND ] = D3DBLEND_ONE;

		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectTLV );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );

	///////////
		if( texid1 >= 0 ){
			float fw1 = (float)width1;
			float fh1 = (float)height1;
			hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw1, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :width0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh1, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :height0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex1 );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :texture0 SetTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int passno1;
			if( texf1 == D3DTEXF_POINT ){
				passno1 = 0;
			}else{
				passno1 = 1;
			}
			float offsetu1, offsetv1;
			if( adjustuvflag < 0 ){
				if( texf1 == D3DTEXF_POINT ){
					offsetu1 = 0.5f / screenw1;
					offsetv1 = 0.5f / screenh1;
				}else{
					offsetu1 = 0.0f;
					offsetv1 = 0.0f;
				}
			}else if( adjustuvflag == 0 ){
				offsetu1 = 0.0f;
				offsetv1 = 0.0f;

			}else if( adjustuvflag == 1 ){
				offsetu1 = 0.5f / screenw1;
				offsetv1 = 0.5f / screenh1;
			}

			hres = g_pPostEffect->BeginPass( passno1 );
			_ASSERT( hres == D3D_OK );


			float r1, g1, b1, a1;
			r1 = col1.r;
			g1 = col1.g;
			b1 = col1.b;
			a1 = col1.a;

			EFFECTTLV Vertex1[4] = {
				//   x    y     z      w     u0 v0
				{{-1.0f, +1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {0.0f + offsetu1, 0.0f + offsetv1} },
				{{+1.0f, +1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {1.0f + offsetu1, 0.0f + offsetv1} },
				{{+1.0f, -1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {1.0f + offsetu1, 1.0f + offsetv1} },
				{{-1.0f, -1.0f, 0.2f, 1.0f}, {r1, g1, b1, a1}, {0.0f + offsetu1, 1.0f + offsetv1} }
			};

			hres = pdev->SetVertexDeclaration( s_tlvdecl );
			_ASSERT( hres == D3D_OK );

			hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex1, sizeof( EFFECTTLV ) );
			_ASSERT( hres == D3D_OK );

			hres = g_pPostEffect->EndPass();
			_ASSERT( hres == D3D_OK );

		}
////////////
		pdev->SetRenderState( D3DRS_BLENDOP, blendop );
		g_renderstate[ D3DRS_BLENDOP ] = blendop;

		pdev->SetRenderState( D3DRS_SRCBLEND, srcblend );
		g_renderstate[ D3DRS_SRCBLEND ] = srcblend;

		pdev->SetRenderState( D3DRS_DESTBLEND, destblend );
		g_renderstate[ D3DRS_DESTBLEND ] = destblend;


		if( texid2 >= 0 ){
			float fw2 = (float)width2;
			float fh2 = (float)height2;
			hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw2, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :width0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh2, sizeof( float ) );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :height0 SetValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex2 );
			if( hres != D3D_OK ){
				DbgOut( "E3DBlendRTTexture :texture0 SetTexture error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int passno2;
			if( texf2 == D3DTEXF_POINT ){
				passno2 = 0;
			}else{
				passno2 = 1;
			}
			float offsetu2, offsetv2;
			if( adjustuvflag < 0 ){
				if( texf2 == D3DTEXF_POINT ){
					offsetu2 = 0.5f / screenw1;
					offsetv2 = 0.5f / screenh1;
				}else{
					offsetu2 = 0.0f;
					offsetv2 = 0.0f;
				}
			}else if( adjustuvflag == 0 ){
				offsetu2 = 0.0f;
				offsetv2 = 0.0f;

			}else if( adjustuvflag == 1 ){
				offsetu2 = 0.5f / screenw1;
				offsetv2 = 0.5f / screenh1;
			}
			hres = g_pPostEffect->BeginPass( passno2 );
			_ASSERT( hres == D3D_OK );

			float r2, g2, b2, a2;
			r2 = col2.r;
			g2 = col2.g;
			b2 = col2.b;
			a2 = col2.a;

			EFFECTTLV Vertex2[4] = {
				//   x    y     z      w     u0 v0
				{{-1.0f, +1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {0.0f + offsetu2, 0.0f + offsetv2} },
				{{+1.0f, +1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {1.0f + offsetu2, 0.0f + offsetv2} },
				{{+1.0f, -1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {1.0f + offsetu2, 1.0f + offsetv2} },
				{{-1.0f, -1.0f, 0.2f, 1.0f}, {r2, g2, b2, a2}, {0.0f + offsetu2, 1.0f + offsetv2} }
			};

			
			hres = pdev->SetVertexDeclaration( s_tlvdecl );
			_ASSERT( hres == D3D_OK );

			hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex2, sizeof( EFFECTTLV ) );
			_ASSERT( hres == D3D_OK );

			hres = g_pPostEffect->EndPass();
			_ASSERT( hres == D3D_OK );
		}
////////////

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}


EASY3D_API int E3DBeginSceneZbuf( int scid, int skipflag, int zscid )
{

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

	SWAPCHAINELEM* zsc = 0;
	int zsizex, zsizey;
	if( zscid >= 0 ){
		zsc = GetSwapChainElem( zscid );
		zsizex = g_scsize.x;
		zsizey = g_scsize.y;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int rendersizex, rendersizey;
	rendersizex = g_scsize.x;
	rendersizey = g_scsize.y;

	int clearzflag = 1;
	IDirect3DSurface9* zsurf = 0;
	if( zsc ){
		if( (zsizex == rendersizex) && (zsizey == rendersizey) ){
			zsurf = zsc->pZSurf;
			clearzflag = 0;
		}else{
			zsurf = cursc->pZSurf;
			clearzflag = 1;
		}
	}else{
		zsurf = cursc->pZSurf;
		clearzflag = 1;
	}


	ret = g_pD3DApp->TestCooperative();
	if( ret == 2 ){
		g_activateapp = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}else if( ret == 1 ){
		DbgOut( "E3DBeginScene : TestCooperative error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	HRESULT hres;
	hres = pdev->SetRenderTarget( 0, cursc->pBackBuffer );
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : 3ddev SetRenderTarget error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}
	hres = pdev->SetDepthStencilSurface( zsurf );
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
		_ASSERT( 0 );
		return 1;
	}

	D3DVIEWPORT9 viewport;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Height = g_scsize.y;
    viewport.Width =  g_scsize.x;
    viewport.MaxZ = 1.0f;
    viewport.MinZ = 0.0f;

    hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "E3DBeginScene : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (skipflag & 2) == 0 ){
		CalcMatView();

		ret = SetShaderConst( 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

   if( FAILED( pdev->BeginScene() ) ){
		_ASSERT( 0 );
        return 1;
	}

    // Clear the viewport

	if( (skipflag & 1) == 0 ){
		if( clearzflag ){
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
								 cursc->clearcol, 1.0f, 0L );
		}else{
			hres = pdev->Clear( 0L, NULL, D3DCLEAR_TARGET, 
								 cursc->clearcol, 1.0f, 0L );
		}
	   if( hres != D3D_OK ){
			//_ASSERT( 0 );
		   DbgOut( "E3DBegin : Clear error !!!\n" );
			return 1;
	   }
	}

//!!!!!!!!!!!!!!!!
//render bgdisp
//!!!!!!!!!!!!!!!!

	if( ((skipflag & 1) == 0) && ((skipflag & 4) == 0) ){

		BGDISPELEM* curbgelem;
		curbgelem = GetBGDispElem( scid );
		if( curbgelem && curbgelem->bgdisp ){

			ret = curbgelem->bgdisp->SetBgSize( g_scsize.x, g_scsize.y );
			if( ret ){
				return 1;
			}
			ret = curbgelem->bgdisp->MoveTexUV( s_matView );
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->UpdateVertexBuffer();
			_ASSERT( !ret );

			ret = curbgelem->bgdisp->Render( pdev );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			} 
		}
	}
	return 0;
}


EASY3D_API int E3DSetClearCol( int scid, E3DCOLOR4UC col )
{
	if( scid < 0 ){
		DbgOut( "E3DSetClearCol : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DSetClearCol : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	cursc->clearcol = D3DCOLOR_ARGB( col.a, col.r, col.g, col.b );

	return 0;
}
EASY3D_API int E3DSetMotionBlurMinAlpha( int hsid, int partno, float minalpha )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionBlurMinAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMotionBlurMinAlpha( partno, minalpha );
	if( ret ){
		DbgOut( "E3DSetMotionBlurMinAlpha : hs SetMotionBlurMinAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMotionBlurMaxAlpha( int hsid, int partno, float maxalpha )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionBlurMaxAlpha : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMotionBlurMaxAlpha( partno, maxalpha );
	if( ret ){
		DbgOut( "E3DSetMotionBlurMaxAlpha : hs SetMotionBlurMaxAlpha error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DRenderGlow( int* hsidarray, int arrayleng )
{
	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

//	pdev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
//	g_renderstate[ D3DRS_ALPHATESTENABLE ] = FALSE;


	//不透明グローの描画
	int hsno;
	int withalpha;
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		int curhsid;
		curhsid = *( hsidarray + hsno );//!!!!!!!!!!!!!!!!!!

		CHandlerSet* curhs;
		curhs = GetHandlerSet( curhsid );
		if( !curhs ){
			DbgOut( "E3DRenderGlow : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		withalpha = 0;
		ret = curhs->RenderGlow( pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			DbgOut( "E3DRenderGlow : hs RenderGlow 0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	//半透明グローの描画
	for( hsno = 0; hsno < arrayleng; hsno++ ){
		int curhsid;
		curhsid = *( hsidarray + hsno );//!!!!!!!!!!!!!!!!!!

		CHandlerSet* curhs;
		curhs = GetHandlerSet( curhsid );
		if( !curhs ){
			DbgOut( "E3DRenderGlow : hsid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		withalpha = 1;
		ret = curhs->RenderGlow( pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			DbgOut( "E3DRenderGlow : hs RenderGlow 1 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}
EASY3D_API int E3DSetMaterialGlowParams( int hsid, int matno, int glowmode, int setcolflag, E3DCOLOR4F multcol )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialGlowParams : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	multcol.a = 1.0f;

	ret = curhs->SetMaterialGlowParams( matno, glowmode, setcolflag, &multcol );
	if( ret ){
		DbgOut( "E3DSetMaterialGlowParams : hs SetMaterialGlowParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}
EASY3D_API int E3DGetMaterialGlowParams( int hsid, int matno, int* modeptr, E3DCOLOR4F* colptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialGlowParams : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMaterialGlowParams( matno, modeptr, colptr );
	if( ret ){
		DbgOut( "E3DGetMaterialGlowParams : hs GetMaterialGlowParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colptr->a = 1.0f;

	return 0;
}
EASY3D_API int E3DGetMaterialTransparent( int hsid, int matno, int*transptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialTransparent : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMaterialTransparent( matno, transptr );
	if( ret ){
		DbgOut( "E3DGetMaterialTransparent : hs GetMaterialTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMaterialTransparent( int hsid, int matno, int transparent )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialTransparent : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMaterialTransparent( matno, transparent );
	if( ret ){
		DbgOut( "E3DSetMaterialTransparent : hs SetMaterialTransparent error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DChkHDR( int* okflag, int* hdrformat, int* toneformat )
{
	*okflag = 0;

	if( !g_pD3DApp ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = g_pD3DApp->CheckHDR( okflag, hdrformat, toneformat );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
EASY3D_API int E3DSetHDRLightDiffuse( int lid, E3DCOLOR4F col )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetHDRLightDiffuse : lightID not found error !!!\n" );
		return 1;
	}

	CVec3f lcol;
	lcol.x = col.r;
	lcol.y = col.g;
	lcol.z = col.b;

	curlight->Diffuse = lcol;

////////

	//resotreでも実行する

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : E3DSetHDRLightDiffuse : GetD3DDevice error !!!\n" );
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
EASY3D_API int E3DSetHDRLightSpecular( int lid, E3DCOLOR4F col )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}
	
	int ret;

	CLightData* curlight = GetLightData( lid );
	if( !curlight ){
		_ASSERT( 0 );
		DbgOut( "E3DSetHDRLightSpecular : lightID not found error !!!\n" );
		return 1;
	}

	CVec3f lcol;
	lcol.x = col.r;
	lcol.y = col.g;
	lcol.z = col.b;

	curlight->Specular = lcol;

////////

	//resotreでも実行する

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "e3dhsp : E3DSetHDRLightSpecular : GetD3DDevice error !!!\n" );
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
EASY3D_API int E3DDownScaleRTTexture4x4( int dstscid, int srctexid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture4x4 : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DDownScaleRTTexture4x4 : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DDownScaleRTTexture4x4 : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture4x4 : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectDownScale );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture4x4 :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture4x4 :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture4x4 :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale4x4Offset( fw, fh );
		if( ret ){
			DbgOut( "E3DDownScaleRTTexture4x4 : SetDownScale4x4Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;

}
EASY3D_API int E3DDownScaleRTTexture2x2( int dstscid, int srctexid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture2x2 : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DDownScaleRTTexture2x2 : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DDownScaleRTTexture2x2 : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DDownScaleRTTexture2x2 : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectDownScale );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture2x2 :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture2x2 :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DDownScaleRTTexture2x2 :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale2x2Offset( fw, fh );
		if( ret ){
			DbgOut( "E3DDownScaleRTTexture2x2 : SetDownScale2x2Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;
}
EASY3D_API int E3DCreateToneMapTexture( int tonenum, int toneformat, int* tonemapid, int* okflag )
{
	int ret;
	*okflag = 0;

	TONEMAPELEM* newtm = 0;
	newtm = GetFreeToneMapElem();
	if( !newtm ){
		DbgOut( "E3DCreateToneMapTexture : tonemap memory overflow error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (toneformat != D3DFMT_R32F) && (toneformat != D3DFMT_R16F) ){
		_ASSERT( 0 );
		toneformat = D3DFMT_R32F;
	}

	if( tonenum < 3 ){
		_ASSERT( 0 );
		tonenum = 3;
	}

	int* pscid = 0;
	pscid = (int*)malloc( sizeof( int ) * tonenum );
	if( !pscid ){
		DbgOut( "E3DCreateToneMapTexture : pscid alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	ZeroMemory( pscid, sizeof( int ) * tonenum );

	int* ptexid = 0;
	ptexid = (int*)malloc( sizeof( int ) * tonenum );
	if( !ptexid ){
		DbgOut( "E3DCreateToneMapTexture : ptexid alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	ZeroMemory( ptexid, sizeof( int ) * tonenum );

	int allocno;
	for( allocno = 0; allocno < tonenum; allocno++ ){
		int texsize = 1 << ( 2 * allocno );
		SIZE tsize;
		tsize.cx = texsize;
		tsize.cy = texsize;
		int ok1 = 0;
		ret = E3DCreateRenderTargetTexture( tsize, toneformat, pscid + allocno, ptexid + allocno, &ok1, 1 );
		if( ret || !ok1 ){
			DbgOut( "E3DCreateToneMapTexture : CreateRenderTargetTexture %d error !!!\n", allocno );
			_ASSERT( 0 );
			free( pscid );
			free( ptexid );
			return 0;
		}
	}
	
	newtm->tonenum = tonenum;
	newtm->pscid = pscid;
	newtm->ptexid = ptexid;

	*tonemapid = newtm->tonemapid;
	*okflag = 1;

	return 0;
}
EASY3D_API int E3DDestroyToneMapTexture( int tonemapid )
{
	int ret;
	if( tonemapid < 0 ){
		_ASSERT( 0 );
		return 0;
	}

	TONEMAPELEM* findelem = 0;
	findelem = GetToneMapElem( tonemapid );
	if( !findelem ){
		_ASSERT( 0 );
		return 0;
	}

	int tmno;
	for( tmno = 0; tmno < findelem->tonenum; tmno++ ){
		int curscid, curtexid;
		curscid = *( findelem->pscid + tmno );
		curtexid = *( findelem->ptexid + tmno );
		ret = E3DDestroyRenderTargetTexture( curscid, curtexid );
		if( ret ){
			DbgOut( "E3DDestroyToneMapTexture : E3DDestroyRenderTargetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( findelem->pscid ){
		free( findelem->pscid );
		findelem->pscid = 0;
	}
	if( findelem->ptexid ){
		free( findelem->ptexid );
		findelem->ptexid = 0;
	}
	findelem->tonemapid = -1;
	findelem->tonenum = 0;

	return 0;
}
EASY3D_API int E3DRenderToneMap( int tonemapid, int srctexid )
{
	int ret;

	if( !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	if( tonemapid < 0 ){
		DbgOut( "E3DRenderToneMap : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	TONEMAPELEM* tmptr = 0;
	tmptr = GetToneMapElem( tonemapid );
	if( !tmptr ){
		DbgOut( "E3DRenderToneMap : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int curtmno = tmptr->tonenum - 1;
	int curscid = *( tmptr->pscid + curtmno );
	int curtexid = srctexid;
	ret = ToneMapFirst( curscid, curtexid );
	if( ret ){
		DbgOut( "E3DRenderToneMap : ToneMapFirst error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curtmno--;
	while( curtmno > 0 ){
		curscid = *( tmptr->pscid + curtmno );
		curtexid = *( tmptr->ptexid + curtmno + 1 );
		ret = ToneMapSecond( curscid, curtexid );
		if( ret ){
			DbgOut( "E3DRenderToneMap : ToneMapSecond error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curtmno--;
	}

	curtmno = 0;
	curscid = *( tmptr->pscid + 0 );
	curtexid = *( tmptr->ptexid + 1 );
	ret = ToneMapFinal( curscid, curtexid );
	if( ret ){
		DbgOut( "E3DRenderToneMap : ToneMapFinal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DBrightPassRTTextureHDR( int dstscid, int srctexid, float middlegray, float threshold, float offset, int tonemapid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}



	if( !g_texbnk ){
		DbgOut( "E3DBrightPassRTTextureHDR : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DBrightPassRTTextureHDR : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DBrightPassRTTextureHDR : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( tonemapid < 0 ){
		DbgOut( "E3DBrightPassRTTextureHDR : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	TONEMAPELEM* tmptr = 0;
	tmptr = GetToneMapElem( tonemapid );
	if( !tmptr ){
		DbgOut( "E3DBrightPassRTTextureHDR : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( tmptr->tonenum <= 0 ){
		DbgOut( "E3DBrightPassRTTextureHDR : tonenum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex1 = 0;
	ret = g_texbnk->GetTexDataBySerial( *( tmptr->ptexid + 0 ), &ptex1 );
	if( ret || !ptex1 ){
		DbgOut( "E3DBrightPassRTTextureHDR : tone texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DBrightPassRTTextureHDR : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectBrightPass );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture1, ptex1 );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :texture1 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->SetValue( g_hpeMiddleGray, &middlegray, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :middlegray SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeBrightPassThreshold, &threshold, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :threshold SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeBrightPassOffset, &offset, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBrightPassRTTextureHDR :BrightPassOffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		ret = SetGauss5x5Weight( fw, fh, 10.0f );
		if( ret ){
			DbgOut( "E3DBrightPassRTTextureHDR : SetGauss5x5Weight error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------
		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;

}

TONEMAPELEM* GetFreeToneMapElem()
{
	int tmno;
	TONEMAPELEM* findelem = 0;

	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;

		if( curtm->tonemapid < 0 ){
			s_usertonemapcnt++;
			curtm->tonemapid = s_usertonemapcnt;
			curtm->tonenum = 0;
			if( curtm->pscid ){
				free( curtm->pscid );
				curtm->pscid = 0;
			}
			if( curtm->ptexid ){
				free( curtm->ptexid );
				curtm->ptexid = 0;
			}

			findelem = curtm;
			break;
		}
	}

	return findelem;
}
TONEMAPELEM* GetToneMapElem( int tonemapid )
{
	int tmno;
	TONEMAPELEM* findelem = 0;

	for( tmno = 0; tmno < TONEMAPMAX; tmno++ ){
		TONEMAPELEM* curtm;
		curtm = s_tonemapelem + tmno;
		if( curtm->tonemapid == tonemapid ){
			findelem = curtm;
			break;
		}
	}
	return findelem;
}
int ToneMapFirst( int dstscid, int srctexid )
{
//g_hPostEffectToneMap
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "ToneMapFirst : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "ToneMapFirst : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "ToneMapFirst : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "ToneMapFirst : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectToneMap );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFirst :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFirst :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFirst :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetToneMapFirstOffset( screenw, screenh );
		if( ret ){
			DbgOut( "ToneMapFirst : SetToneMapFirstOffset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}
int ToneMapSecond( int dstscid, int srctexid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "ToneMapSecond : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "ToneMapSecond : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "ToneSecond : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "ToneSecond : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectToneMap );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapSecond :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapSecond :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapSecond :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale4x4Offset( fw, fh );
		if( ret ){
			DbgOut( "ToneMapSecond : SetDownScale4x4Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 1 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}
int ToneMapFinal( int dstscid, int srctexid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "ToneMapFinal : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "ToneMapFinal : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "ToneMapFinal : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "ToneMapFinal : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectToneMap );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFinal :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFinal :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "ToneMapFinal :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = SetDownScale4x4Offset( fw, fh );
		if( ret ){
			DbgOut( "ToneMapFinal : SetDownScale4x4Offset error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 2 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;
}


EASY3D_API int E3DRenderHDRFinal( int dstscid, int srctexid0, int srctexid1, int tonemapid, float middlegray )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	if( !g_texbnk ){
		DbgOut( "E3DRenderHDRFinal : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex0 = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid0, &ptex0 );
	if( ret || !ptex0 ){
		DbgOut( "E3DRenderHDRFinal : texid0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex1 = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid1, &ptex1 );
	if( ret || !ptex1 ){
		DbgOut( "E3DRenderHDRFinal : texid1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( tonemapid < 0 ){
		DbgOut( "E3DRenderHDRFinal : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	TONEMAPELEM* tmptr = 0;
	tmptr = GetToneMapElem( tonemapid );
	if( !tmptr ){
		DbgOut( "E3DRenderHDRFinal : tonemapid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( tmptr->tonenum <= 0 ){
		DbgOut( "E3DRenderHDRFinal : tonenum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex2 = 0;
	ret = g_texbnk->GetTexDataBySerial( *( tmptr->ptexid + 0 ), &ptex2 );
	if( ret || !ptex2 ){
		DbgOut( "E3DRenderHDRFinal : tone texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DRenderHDRFinal : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;


	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectHDRFinal );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex0 );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture1, ptex1 );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :texture1 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture2, ptex2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :texture2 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->SetValue( g_hpeMiddleGray, &middlegray, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DRenderHDRFinal :middlegray SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------
		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;

}
EASY3D_API int E3DDisablePostEffect()
{
	g_usePostEffect = 0;

	return 0;

}
EASY3D_API int E3DCheckSpec( HINSTANCE srchinst, HWND srchwnd, int* okflag )
{
	*okflag = 0;

	_ASSERT( srchinst );
	_ASSERT( srchwnd );

	CD3DApplication* tmpapp;
	tmpapp = new CD3DApplication();
	if( !tmpapp ){
		DbgOut( "E3DCheckSpec : tmpapp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = tmpapp->CheckSpec( srchinst, srchwnd, okflag );
	if( ret ){
		DbgOut( "E3DCheckSpec : app CheckSpec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	delete tmpapp;

	return 0;
}

EASY3D_API int E3DMonoRTTexture( int dstscid, int srctexid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DMonoRTTexture : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DMonoRTTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DMonoRTTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DMonoRTTexture : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectMono );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DMonoRTTexture :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DMonoRTTexture :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DMonoRTTexture :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;

}
EASY3D_API int E3DCbCrRTTexture( int dstscid, int srctexid, float srccb, float srccr )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DCbCrRTTexture : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DCbCrRTTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DCbCrRTTexture : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int width, height, pool, transparent, format;
	ret = g_texbnk->GetTextureInfoBySerial( srctexid, &width, &height, &pool, &transparent, &format );
	if( ret ){
		DbgOut( "E3DCbCrRTTexture : GetTextureInfoBySerial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectCbCr );
		_ASSERT( hres == D3D_OK );

		float fw = (float)width;
		float fh = (float)height;
		hres = g_pPostEffect->SetValue( g_hpeWidth0, &fw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &fh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		float tmpcbcr[2];
		tmpcbcr[0] = srccb;
		tmpcbcr[1] = srccr;
		hres = g_pPostEffect->SetValue( g_hpeCbCr, tmpcbcr, sizeof( float ) * 2 );
		if( hres != D3D_OK ){
			DbgOut( "E3DCbCrRTTexture :CbCr SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();


	return 0;

}

EASY3D_API int E3DShaderConstOffset16( D3DXVECTOR2* offsetarray )
{
	HRESULT hres;
	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleOffset, offsetarray, sizeof( D3DXVECTOR2 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "E3DShaderConstOffset16 : sampleoffset SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
EASY3D_API int E3DShaderConstWeight16( D3DXVECTOR4* weightarray )
{
	HRESULT hres;
	if( g_pPostEffect ){
		hres = g_pPostEffect->SetValue( g_hpeSampleWeight, weightarray, sizeof( D3DXVECTOR4 ) * 16 );
		if( hres != D3D_OK ){
			DbgOut( "E3DShaderConstWeight16 : sampleweight SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
EASY3D_API int E3DShaderConstUserFL4( int datano, D3DXVECTOR4 fl4 )
{
	if( (datano < 0) || (datano > 9) ){
		DbgOut( "E3DShaderConstUserFL4 : datano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_pPostEffect ){
		DbgOut( "E3DShaderConstUserFL4 : posteffect NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hres;
	hres = g_pPostEffect->SetValue( g_hpeUserFl4[datano], &fl4, sizeof( D3DXVECTOR4 ) );
	if( hres != D3D_OK ){
		DbgOut( "E3DShaderConstUserFL4 : fl4 SetValue error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
EASY3D_API int E3DShaderConstUserTex( int datano, int srctexid )
{
	int ret;
	HRESULT hres;

	if( !g_pPostEffect ){
		DbgOut( "E3DShaderConstUserTex : posteffect NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (datano < 0) || (datano > 4) ){
		DbgOut( "E3DShaderConstUserTex : datano error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !g_texbnk ){
		DbgOut( "E3DShaderConstUserTex : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = 0;
	ret = g_texbnk->GetTexDataBySerial( srctexid, &ptex );
	if( ret || !ptex ){
		DbgOut( "E3DShaderConstUserTex : texid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	hres = g_pPostEffect->SetTexture( g_hpeUserTex[datano], ptex );
	if( hres != D3D_OK ){
		DbgOut( "E3DShaderConstUserTex :tex SetTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DCallUserShader( int shaderno, int passno, int dstscid )
{
	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	if( (shaderno < 0) || (shaderno >= 20) ){
		DbgOut( "E3DCallUserShader : shaderno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}


	ret = RTBeginScene( pdev, dstscid, 0, 0 );
	if( ret ){
		DbgOut( "E3DCallUserShader : RTBeginScene error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;
	float offsetu = 0.5f / screenw;
	float offsetv = 0.5f / screenh;

	if( g_pPostEffect != NULL )
	{
		ret = SetRTRenderState( pdev, 0 );
		_ASSERT( !ret );

		pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;


		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectUser[shaderno] );
		_ASSERT( hres == D3D_OK );

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		UINT passnum = 0;
		hres = g_pPostEffect->Begin( &passnum, 0 );
		_ASSERT( hres == D3D_OK );

		if( passno >= (int)passnum ){
			DbgOut( "E3DCallUserShader : passno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pPostEffect->BeginPass( passno );
		_ASSERT( hres == D3D_OK );

		//-------------------------------------------------
		// フィルタリング
		//-------------------------------------------------

		TVERTEX Vertex1[4] = {
			//   x    y     z      w     u0 v0
			{-1.0f, +1.0f, 0.1f,  1.0f, 0.0f + offsetu, 0.0f + offsetv},
			{+1.0f, +1.0f, 0.1f,  1.0f, 1.0f + offsetu, 0.0f + offsetv},
			{+1.0f, -1.0f, 0.1f,  1.0f, 1.0f + offsetu, 1.0f + offsetv},
			{-1.0f, -1.0f, 0.1f,  1.0f, 0.0f + offsetu, 1.0f + offsetv}
		};

		//hres = pdev->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		hres = pdev->SetVertexDeclaration( s_effectdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

    pdev->EndScene();

	return 0;

}

EASY3D_API int E3DCreateBone( int hsid, const char* nameptr, int parentid, int* serialptr )
{
	int ret;
	int newseri = -1;
	*serialptr = newseri;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCreateBone : pdev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( *nameptr == 0 ){
		DbgOut( "E3DCreateBone : bonename NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->CreateBone( pdev, s_hwnd, (char*)nameptr, parentid, &newseri );
	if( ret || (newseri <= 0) ){
		DbgOut( "E3DCreateBone : hs CreateBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*serialptr = newseri;

	return 0;
}
EASY3D_API int E3DDestroyBone( int hsid, int boneno )
{
	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DDestroyBone : pdev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DestroyBone( pdev, s_hwnd, boneno );
	if( ret ){
		DbgOut( "E3DDestroyBone : hs DestroyBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetMorphBaseNum( int hsid, int* basenumptr )
{
	*basenumptr = 0;

	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphBaseNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMorphBaseNum( basenumptr );
	if( ret ){
		DbgOut( "E3DGetMorphBaseNum : hs GetMorphBaseNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMorphBaseInfo( int hsid, E3DMORPHBASE* baseinfoptr, int arrayleng )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphBaseInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMorphBaseInfo( baseinfoptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMorphBaseInfo : hs GetMorphBaseInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMorphTargetInfo( int hsid, int baseid, E3DMORPHTARGET* targetinfoptr, int arrayleng )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphTargetInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMorphTargetInfo( baseid, targetinfoptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMorphTargetInfo : hs GetMorphTargetInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DGetMorphKeyNum( int hsid, int motid, int boneid, int baseid, int targetid, int* keynumptr )
{
	*keynumptr = 0;

	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphKeyNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMorphKeyNum( motid, boneid, baseid, targetid, keynumptr );
	if( ret ){
		DbgOut( "E3DGetMorphKeyNum : hs GetMorphKeyNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DGetMorphKey( int hsid, int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr, int arrayleng )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMorphKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMorphKey( motid, boneid, baseid, targetid, motioninfoptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMorphKey : hs GetMorphKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetCurMorphInfo( int hsid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetCurMorphInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetCurMorphInfo( boneid, baseid, targetid, motioninfoptr );
	if( ret ){
		DbgOut( "E3DGetCurMorphInfo : hs GetCurMorphInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DCreateMorph( int hsid, int baseid, int boneid )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateMorph : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev = 0;
	if( g_pD3DApp ){
		ret = g_pD3DApp->GetD3DDevice( &pdev );
	}
	if( !pdev ){
		DbgOut( "E3DCreateMorph : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->CreateMorph( pdev, s_hwnd, baseid, boneid );
	if( ret ){
		DbgOut( "E3DCreateMorph : hs CreateMorph error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DAddMorphTarget( int hsid, int baseid, int targetid )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMorphTarget : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev = 0;
	if( g_pD3DApp ){
		ret = g_pD3DApp->GetD3DDevice( &pdev );
	}
	if( !pdev ){
		DbgOut( "E3DAddMorphTarget : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->AddMorphTarget( baseid, targetid );
	if( ret ){
		DbgOut( "E3DAddMorphTarget : hs AddMorphTarget error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMorphKey( int hsid, int motid, int boneid, int baseid, int targetid, E3DMORPHMOTION* motioninfoptr )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMorphKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetMorphKey( motid, boneid, baseid, targetid, motioninfoptr );
	if( ret ){
		DbgOut( "E3DSetMorphKey : hs SetMorphKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDestroyMorphKey( int hsid, int motid, int boneid, int frameno, int baseid )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMorphKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->DestroyMorphKey( motid, boneid, frameno, baseid );
	if( ret ){
		DbgOut( "E3DDestroyMorphKey : hs DestroyMorphKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetToon1Material( int hsid, int partno, E3DTOON1MATERIAL* toon1ptr, int arrayleng )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetToon1Material : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetToon1Material( partno, toon1ptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetToon1Material : hs GetToon1Material error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetToon1Name( int hsid, int partno, char* oldname, char* newname )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Name : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Name( partno, oldname, newname );
	if( ret ){
		DbgOut( "E3DSetToon1Name : hs SetToon1Name error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetToon1Diffuse( int hsid, int partno, char* matname, RDBColor3f diffuse )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Diffuse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Diffuse( partno, matname, diffuse );
	if( ret ){
		DbgOut( "E3DSetToon1Diffuse : hs SetToon1Diffuse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetToon1Specular( int hsid, int partno, char* matname, RDBColor3f specular )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Specular : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Specular( partno, matname, specular );
	if( ret ){
		DbgOut( "E3DSetToon1Specular : hs SetToon1Specular error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetToon1Ambient( int hsid, int partno, char* matname, RDBColor3f ambient )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Ambient : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Ambient( partno, matname, ambient );
	if( ret ){
		DbgOut( "E3DSetToon1Ambient : hs SetToon1Ambient error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetToon1NL( int hsid, int partno, char* matname, float darknl, float brightnl )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1NL : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1NL( partno, matname, darknl, brightnl );
	if( ret ){
		DbgOut( "E3DSetToon1NL : hs SetToon1NL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetToon1Edge0( int hsid, int partno, char* matname, RDBColor3f col, int validflag, int invflag, float width )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetToon1Edge0 : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetToon1Edge0( partno, matname, col, validflag, invflag, width );
	if( ret ){
		DbgOut( "E3DSetToon1Edge0 : hs SetToon1Edge0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DLoadCameraFile( char* filename, int* animnoptr )
{
	*animnoptr = -1;

	_ASSERT( m_cah );
	int ret;
	CCameraFile camfile;
	ret = camfile.LoadCameraFile( filename, m_cah, animnoptr );
	if( ret || (*animnoptr < 0) ){
		DbgOut( "E3DLoadCameraFile : camerafile LoadCameraFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDestroyCameraAnim( int animno )
{
	_ASSERT( m_cah );
	int ret;
	ret = m_cah->DestroyAnim( animno );
	if( ret ){
		DbgOut( "E3DDestroyCameraAnim : cah DestroyAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSaveCameraFile( char* filename, int animno )
{
	_ASSERT( m_cah );
	if( m_cah->m_animnum <= 0 ){
		DbgOut( "E3DSaveCameraFile : animnum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSaveCameraFile : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	CCameraFile camfile;
	ret = camfile.WriteCameraFile( filename, anim );
	if( ret ){
		DbgOut( "E3DSaveCameraFile : camfile WriteCameraFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetCameraAnimNo( int animno )
{
	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetCurrentAnim( animno );
	if( ret ){
		DbgOut( "E3DSetCameraAnimNo : cah SetCurrentAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CAMERAELEM ce = m_cah->m_curcamera;
	ret = CameraElemToParams( ce );
	_ASSERT( !ret );

	return 0;
}
EASY3D_API int E3DGetCameraAnimNo( int* animnoptr )
{
	*animnoptr = -1;

	_ASSERT( m_cah );
	if( m_cah->m_animnum <= 0 ){
		DbgOut( "E3DGetCameraAnimNo : animnum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*animnoptr = m_cah->m_animno;

	return 0;
}
EASY3D_API int E3DSetCameraFrameNo( int animno, int camerano, int frameno, int isfirst )
{
	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetAnimFrameNo( animno, camerano, frameno, isfirst );
	if( ret ){
		DbgOut( "E3DSetCameraFrameNo : cah SetAinmFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CAMERAELEM ce = m_cah->m_curcamera;
	ret = CameraElemToParams( ce );
	_ASSERT( !ret );

	return 0;
}
EASY3D_API int E3DGetCameraFrameNo( int* animnoptr, int* camnoptr, int* framenoptr )
{
	_ASSERT( m_cah );
	if( m_cah->m_animnum <= 0 ){
		DbgOut( "E3DGetCameraFrameNo : animnum 0 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = m_cah->GetAnimFrameNo( animnoptr, camnoptr, framenoptr );
	if( ret ){
		DbgOut( "E3DGetCameraFrameNo : cah GetAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetNewCamera( int camno )
{
	_ASSERT( m_cah );
	int ret;
	if( m_cah->m_animnum <= 0 ){
		_ASSERT( 0 );
		return 0;
	}
	ret = m_cah->SetNewCamera( camno );
	if( ret ){
		DbgOut( "E3DSetNewCamera : cah SetNewCamera error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMERAELEM ce = m_cah->m_curcamera;
	ret = CameraElemToParams( ce );
	_ASSERT( !ret );
	return 0;
}
EASY3D_API int E3DSetNextCameraAnimFrameNo( int animno, int nextanimno, int nextframeno, int befframeno )
{
	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetNextAnimFrameNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextCameraAnimFrameNo : cah SetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetNextCameraAnimFrameNo( int animno, int* nextanimptr, int* nextframenoptr )
{
	_ASSERT( m_cah );
	int ret;
	ret = m_cah->GetNextAnimFrameNo( animno, nextanimptr, nextframenoptr );
	if( ret ){
		DbgOut( "E3DGetNextCameraAnimFrameNo : cah GetNextAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DCreateNewCameraAnim( char* animname, int animtype, int framenum, int* animnoptr )
{
	*animnoptr = -1;

	_ASSERT( m_cah );
	CCameraAnim* newanim;
	newanim = m_cah->AddAnim( animname, animtype, framenum, 0 );
	if( !newanim ){
		DbgOut( "E3DCreatenewCameraAnim : cah AddAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*animnoptr = newanim->m_motkind;

	return 0;
}
EASY3D_API int E3DSetCameraFrameNum( int animno, int newtotal )
{
	_ASSERT( m_cah );
	int ret;
	ret = m_cah->SetTotalFrame( animno, newtotal );
	if( ret ){
		DbgOut( "E3DSetCameraFrameNum : cah SetTotalFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetCameraFrameNum( int animno, int* framenumptr )
{
	_ASSERT( m_cah );

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraFrameNum : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*framenumptr = anim->m_maxframe + 1;

	return 0;
}
EASY3D_API int E3DSetCameraAnimName( int animno, char* srcname )
{
	_ASSERT( m_cah );
	int ret;
	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = anim->SetAnimName( srcname );
	if( ret ){
		DbgOut( "E3DSetCameraAnimName : anim SetAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}
EASY3D_API int E3DGetCameraAnimName( int animno, char* dstname, int arrayleng )
{
	_ASSERT( m_cah );

	if( !dstname ){
		_ASSERT( 0 );
		return 1;
	}

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraAnimName : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = strcpy_s( dstname, arrayleng, anim->m_animname );
	_ASSERT( !ret );

	return 0;
}
EASY3D_API int E3DSetCameraAnimType( int animno, int type )
{
	_ASSERT( m_cah );

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	anim->m_animtype = type;

	return 0;
}
EASY3D_API int E3DGetCameraAnimType( int animno, int* typeptr )
{
	*typeptr = 0;

	_ASSERT( m_cah );

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraAnimType : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*typeptr = anim->m_animtype;

	return 0;
}
EASY3D_API int E3DSetCameraKey( int animno, int camno, int frameno, E3DCAMERASTATE camstate )
{
	//camno -1禁止
	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMERAELEM ce;
	ZeroMemory( &ce, sizeof( CAMERAELEM ) );

	ce.looktype = camstate.looktype;
	ce.pos = camstate.pos;
	ce.target = camstate.target;
	ce.up = camstate.up;
	ce.dist = camstate.dist;
	ce.hsid = camstate.hsid;
	ce.boneno = camstate.boneno;
	ce.nearz = camstate.nearz;
	ce.farz = camstate.farz;
	ce.fov = camstate.fov;

	ce.ortho = camstate.ortho;
	ce.orthosize = camstate.orthosize;

	ret = anim->SetCameraKey( camno, frameno, ce, camstate.interp );
	if( ret ){
		DbgOut( "E3DSetCameraKey : anim SetCameraKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetCameraKey( int animno, int camno, int frameno, E3DCAMERASTATE* camstateptr, int* existptr )
{
	*existptr = 0;

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CAMERAELEM ce;
	ZeroMemory( &ce, sizeof( CAMERAELEM ) );
	ret = anim->GetCameraAnim( &ce, &(camstateptr->interp), camno, frameno, existptr );
	if( ret ){
		DbgOut( "E3DGetCameraKey : anim GetCameraAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	camstateptr->looktype = ce.looktype;
	camstateptr->pos = ce.pos;
	camstateptr->target = ce.target;
	camstateptr->up = ce.up;
	camstateptr->dist = ce.dist;
	camstateptr->hsid = ce.hsid;
	camstateptr->boneno = ce.boneno;
	camstateptr->nearz = ce.nearz;
	camstateptr->farz = ce.farz;
	camstateptr->fov = ce.fov;
	camstateptr->ortho = ce.ortho;
	camstateptr->orthosize = ce.orthosize;

	return 0;
}
EASY3D_API int E3DDeleteCameraKey( int animno, int camno, int frameno )
{
	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DDeleteCameraKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->DeleteCameraKey( camno, frameno );
	if( ret ){
		DbgOut( "E3DDeleteCameraKey : anim DeleteCameraKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetCameraKeyframeNoRange( int animno, int camno, int start, int end, int* framearray, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraKeyframeNoRange : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetCameraKeyframeNoRange( camno, start, end, framearray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetCameraKeyframeNoRange : anim GetCameraKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetCameraSWKey( int animno, int frameno, int camno )
{
	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraSWKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->SetSWKey( frameno, camno );
	if( ret ){
		DbgOut( "E3DSetCameraSWKey : anim SetSWKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetCameraSWKey( int animno, int frameno, int* camnoptr, int* existptr )
{
	*camnoptr = 0;
	*existptr = 0;

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraSWKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetSWAnim( camnoptr, frameno, existptr );
	if( ret ){
		DbgOut( "E3DGetCameraSWKey : anim GetSWAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDeleteCameraSWKey( int animno, int frameno )
{
	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DDeleteCameraSWKey : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->DeleteSWKey( frameno );
	if( ret ){
		DbgOut( "E3DDeleteCameraSWKey : anim DeleteSWKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetCameraSWKeyframeNoRange( int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	_ASSERT( m_cah );
	int ret;

	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetCameraSWKeyframeNoRange : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = anim->GetSWKeyframeNoRange( start, end, framearray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetCameraSWKeyframeNoRange : anim GetSWKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CameraElemToParams( CAMERAELEM ce )
{
	s_cameratype = 3;
	s_camerapos = ce.pos;
	s_cameraupvec = ce.up;
	s_camera_twist = 0.0f;


	if( ce.looktype == CAML_LOCK ){
		CHandlerSet* curhs = GetHandlerSet( ce.hsid );
		if( curhs ){
			if( (ce.boneno > 0) && (ce.boneno < curhs->m_shandler->s2shd_leng) ){
				CShdElem* curse = (*curhs->m_shandler)( ce.boneno );
				if( curse && curse->IsJoint() ){
					int ret;
					D3DXVECTOR3 bonepos( 0.0f, 0.0f, 0.0f );
					ret = curhs->m_shandler->GetBonePos( ce.boneno, 1, 
						curhs->m_mhandler->m_curmotkind, curhs->m_mhandler->m_curframeno,
						&bonepos, curhs->m_mhandler, curhs->m_gpd.m_matWorld, 1 );
					if( !ret ){
						ce.target = bonepos;
					}
				}
			}
		}
	}
	s_cameratarget = ce.target;


	D3DXMATRIX tmpview;
	D3DXMatrixLookAtLH( &tmpview, &s_camerapos, &s_cameratarget, &s_cameraupvec );
	
	tmpview._41 = 0.0f;
	tmpview._42 = 0.0f;
	tmpview._43 = 0.0f;

	D3DXQUATERNION qx;
	D3DXQuaternionRotationMatrix( &qx, &tmpview );
	s_cameraq.x = qx.x;
	s_cameraq.y = qx.y;
	s_cameraq.z = qx.z;
	s_cameraq.w = qx.w;

	g_proj_near = ce.nearz;
	g_proj_far = ce.farz;
	g_proj_fov = ce.fov;
	g_ortho = ce.ortho;
	g_orthosize = ce.orthosize;

	return 0;
}

EASY3D_API int E3DSetOrtho( float nearz, float farz, float orthosize )
{
	g_ortho = 1;//!!!!!!!!!!!!!!!
	g_proj_near = nearz;
	g_proj_far = farz;
	g_orthosize = orthosize;

	if( g_proj_fov == 0.0f )
		g_proj_fov = 45.0f; //default値
	
	if( g_orthosize < 0.0f )
		g_orthosize = 3000.0f;

	if( g_proj_near >= g_proj_far ){
		g_proj_near = 100.0f; //default
		g_proj_far = 10000.0f;//default
	}

	return 0;
}

EASY3D_API int E3DLoadGPFile( int hsid, const char* filename, int* animnoptr )
{
	char fname[MAX_PATH];
	if( filename ){
		strcpy_s( fname, MAX_PATH, filename );
	}else{
		DbgOut( "E3DLoadGPFile : name NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLoadGPFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->LoadGPFile( fname, animnoptr );
	if( ret ){
		DbgOut( "E3DLoadGPFile : hs LoadGPFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDestroyGPAnim( int hsid, int animno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyGPAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyGPAnim( animno );
	if( ret ){
		DbgOut( "E3DDestroyGPAnim : hs DestroyGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSaveGPFile( int hsid, const char* filename, int animno )
{
	char fname[MAX_PATH];
	if( filename ){
		strcpy_s( fname, MAX_PATH, filename );
	}else{
		DbgOut( "E3DSaveGPFile : name NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveGPFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SaveGPFile( fname, animno );
	if( ret ){
		DbgOut( "E3DSaveGPFile : hs SaveGPFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetGPAnimNo( int hsid, int animno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPAnimNo( animno );
	if( ret ){
		DbgOut( "E3DSetGPAnimNo : hs SetGPAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetGPAnimNo( int hsid, int* animnoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetGPAnimNo( animnoptr );
	if( ret ){
		DbgOut( "E3DGetGPAnimNo : hs GetGPAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetGPFrameNo( int hsid, int animno, int frameno, int isfirst )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPFrameNo( animno, frameno, isfirst );
	if( ret ){
		DbgOut( "E3DSetGPFrameNo : hs SetGPFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetGPFrameNo( int hsid, int* animnoptr, int* framenoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetGPFrameNo( animnoptr, framenoptr );
	if( ret ){
		DbgOut( "E3DGetGPGFrameNo : hs GetGPFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetNewGP( int hsid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewGP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNewGP();
	if( ret ){
		DbgOut( "E3DSetNewGP : hs SetNewGP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetNextGPAnimFrameNo( int hsid, int animno, int nextanimno, int nextframeno, int befframeno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextGPAnimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNextGPAnimFarmeNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextGPAnimFrameNo : hs SetNextGPAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DGetNextGPAnimFrameNo( int hsid, int animno, int* nextanimptr, int* nextframenoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetNextGPanimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextGPAnimFrameNo( animno, nextanimptr, nextframenoptr );
	if( ret ){
		DbgOut( "E3DGetNextGPAnimFrameNo : hs GetNextGPAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DCreateNewGPAnim( int hsid, const char* animname, int animtype, int framenum, int* animnoptr )
{
	char aname[256];
	if( animname ){
		strcpy_s( aname, 256, animname );
	}else{
		DbgOut( "E3DCreateNewGPAnim : animname NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateNewGPAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateNewGPAnim( aname, animtype, framenum, animnoptr );
	if( ret ){
		DbgOut( "E3DCreateNewGPAnim : hs CreateNewGPAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetGPFrameNum( int hsid, int animno, int newtotal )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetGPFrameNum( animno, newtotal );
	if( ret ){
		DbgOut( "E3DSetGPFrameNum : hs SetGPFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DGetGPFrameNum( int hsid, int animno, int* framenumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetGPFrameNum( animno, framenumptr );
	if( ret ){
		DbgOut( "E3DGetGPFrameNum : hs GetGPFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DSetGPAnimName( int hsid, int animno, char* srcname )
{
	char aname[256];
	if( srcname ){
		strcpy_s( aname, 256, srcname );
	}else{
		DbgOut( "E3DSetGPAnimName : srcname NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPAnimName( animno, aname );
	if( ret ){
		DbgOut( "E3DSetGPAnimName : hs SetGPAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetGPAnimName( int hsid, int animno, char* dstname, int arrayleng )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetGPAnimName( animno, dstname, arrayleng );
	if( ret ){
		DbgOut( "E3DGetGPAnimName : hs GetGPAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetGPAnimType( int hsid, int animno, int type )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPAnimType( animno, type );
	if( ret ){
		DbgOut( "E3DSetGPAnimType : hs SetGPAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DGetGPAnimType( int hsid, int animno, int* typeptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetGPAnimType( animno, typeptr );
	if( ret ){
		DbgOut( "E3DGetGPAnimType : hs GetGPAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DSetGPKey( int hsid, int animno, int frameno, E3DGPSTATE gpstate )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPKey( animno, frameno, gpstate );
	if( ret ){
		DbgOut( "E3DSetGPKey : hs SetGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DGetGPKey( int hsid, int animno, int frameno, E3DGPSTATE* gpstateptr, int* existptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetGPKey( animno, frameno, gpstateptr, existptr );
	if( ret ){
		DbgOut( "E3DGetGPKey : hs GetGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DDeleteGPKey( int hsid, int animno, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteGPKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeleteGPKey( animno, frameno );
	if( ret ){
		DbgOut( "E3DDeleteGPKey : hs DeleteGPKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DGetGPKeyframeNoRange( int hsid, int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPKeyframeNoRange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetGPKeyframeNoRange( animno, start, end, framearray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetGPKeyframeNoRange : hs GetGPKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DCalcGPFullFrame( int hsid, int animno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcGPFullFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcGPFullFrame( animno );
	if( ret ){
		DbgOut( "E3DCalcGPFullFrame : hs CalcGPFullFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetGPGroundHSID( int hsid, int animno, int ghsid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPGroundHSID : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetGPGroundHSID( animno, ghsid );
	if( ret ){
		DbgOut( "E3DSetGPGroundHSID : hs SetGPGroundHSID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
EASY3D_API int E3DGetGPGroundHSID( int hsid, int animno, int* ghsidptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetGPGroundHSID : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetGPGroundHSID( animno, ghsidptr );
	if( ret ){
		DbgOut( "E3DGetGPGroundHSID : hs GetGPGroundHSID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DLoadMOEFile( int hsid, const char* filename, int* animnoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLoadMOEFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->LoadMOEFile( (char*)filename, animnoptr );
	if( ret ){
		DbgOut( "E3D : hs LoadMOEFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDestroyMOEAnim( int hsid, int animno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyMOEAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyMOEAnim( animno );
	if( ret ){
		DbgOut( "E3DDestroyMOEAnim : hs DestroyMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSaveMOEFile( int hsid, const char* filename, int animno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSaveMOEFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SaveMOEFile( (char*)filename, animno );
	if( ret ){
		DbgOut( "E3DSaveMOEFile : hs SaveMOEFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOEAnimNo( int hsid, int animno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEAnimNo( animno );
	if( ret ){
		DbgOut( "E3DSetMOEAnimNo : hs SetMOEAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEAnimNo( int hsid, int* animnoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEAnimNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEAnimNo( animnoptr );
	if( ret ){
		DbgOut( "E3DGetMOEAnimNo : hs GetMOEAnimNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOEFrameNo( int hsid, int animno, int frameno, int isfirst )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEFrameNo( animno, frameno, isfirst );
	if( ret ){
		DbgOut( "E3DSetMOEFrameNo : hs  error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEFrameNo( int hsid, int* animnoptr, int* framenoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEFrameNo( animnoptr, framenoptr );
	if( ret ){
		DbgOut( "E3DGetMOEFrameNo : hs GetMOEFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetNewMOE( int hsid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNewMOE : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNewMOE();
	if( ret ){
		DbgOut( "E3DSetNewMOE : hs SetNewMOE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetNextMOEAnimFrameNo( int hsid, int animno, int nextanimno, int nextframeno, int befframeno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetNextMOEAnimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetNextMOEAnimFrameNo( animno, nextanimno, nextframeno, befframeno );
	if( ret ){
		DbgOut( "E3DSetNextMOEAnimFrameNo : hs SetNextMOEAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetNextMOEAnimFrameNo( int hsid, int animno, int* nextanimptr, int* nextframenoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetNextMOEAnimFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetNextMOEAnimFrameNo( animno, nextanimptr, nextframenoptr );
	if( ret ){
		DbgOut( "E3DGetNextMOEAnimFrameNo : hs GetNextMOEAnimFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DCreateNewMOEAnim( int hsid, char* animname, int animtype, int framenum, int* animnoptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateNewMOEAnim : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateNewMOEAnim( animname, animtype, framenum, animnoptr );
	if( ret ){
		DbgOut( "E3DCreateNewMOEAnim : hs CreateNewMOEAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOEFrameNum( int hsid, int animno, int newtotal )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEFrameNum( animno, newtotal );
	if( ret ){
		DbgOut( "E3DSetMOEFrameNum : hs SetMOEFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEFrameNum( int hsid, int animno, int* framenumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEFrameNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEFrameNum( animno, framenumptr );
	if( ret ){
		DbgOut( "E3DGetMOEFrameNum : hs GetMOEFrameNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOEAnimName( int hsid, int animno, char* srcname )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEAnimName( animno, srcname );
	if( ret ){
		DbgOut( "E3DSetMOEAnimName : hs SetMOEAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEAnimName( int hsid, int animno, char* dstname, int arrayleng )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEAnimName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEAnimName( animno, dstname, arrayleng );
	if( ret ){
		DbgOut( "E3DGetMOEAnimName : hs GetMOEAnimName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOEAnimType( int hsid, int animno, int type )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEAnimType( animno, type );
	if( ret ){
		DbgOut( "E3DSetMOEAnimType : hs SetMOEAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEAnimType( int hsid, int animno, int* typeptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEAnimType : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEAnimType( animno, typeptr );
	if( ret ){
		DbgOut( "E3DGetMOEAnimType : hs GetMOEAnimType error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEKeyframeNoRange( int hsid, int animno, int start, int end, int* framearray, int arrayleng, int* getnumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEKeyframeNoRange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEKeyframeNoRange( animno, start, end, framearray, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetMOEKeyframeNoRange : hs GetMOEKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEElemNum( int hsid, int animno, int frameno, int* moeenumptr, int* existkeyptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEElemNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEElemNum( animno, frameno, moeenumptr, existkeyptr );
	if( ret ){
		DbgOut( "E3DGetMOEElemNum : hs GetMOEElemNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEElem( int hsid, int animno, int frameno, int moeeindex, MOEELEM* moeeptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEElem( animno, frameno, moeeindex, moeeptr );
	if( ret ){
		DbgOut( "E3DGetMOEElem : hs GetMOEElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DAddMOEElem( int hsid, int animno, int frameno, MOEELEM moee )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddMOEElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->AddMOEElem( animno, frameno, moee );
	if( ret ){
		DbgOut( "E3DAddMOEElem : hs AddMOEElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDeleteMOEElem( int hsid, int animno, int frameno, int moeeindex )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteMOEElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeleteMOEElem( animno, frameno, moeeindex );
	if( ret ){
		DbgOut( "E3DDeleteMOEElem : hs DeleteMOEElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMOEKeyIdling( int hsid, int animno, int frameno, int motid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEKeyIdling : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEKeyIdling( animno, frameno, motid );
	if( ret ){
		DbgOut( "E3DSetMOEKeyIdling : hs SetMOEKeyIdling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMOEKeyIdling( int hsid, int animno, int frameno, int* motidptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEKeyIdling : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEKeyIdling( animno, frameno, motidptr );
	if( ret ){
		DbgOut( "E3DGetMOEKeyIdling : hs GetMOEKeyIdling error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDeleteMOEKey( int hsid, int animno, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDeleteMOEKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DeleteMOEKey( animno, frameno );
	if( ret ){
		DbgOut( "E3DDeleteMOEKey : hs DeleteMOEKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DCalcMOEFullFrame( int hsid, int animno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcMOEFullFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcMOEFullFrame( animno );
	if( ret ){
		DbgOut( "E3DCalcMOEFullFrame : hs CalcMOEFullFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCreateAndSaveTempMaterial( int hsid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateAndSaveTempMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateAndSaveTempMaterial();
	if( ret ){
		DbgOut( "E3DCreateAndSaveTempMaterial : hs CreateAndSaveTempMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DRestoreAndDestroyTempMaterial( int hsid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRestoreAndDestroyTempMaterial : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->RestoreAndDestroyTempMaterial();
	if( ret ){
		DbgOut( "E3DRestoreAndDestroyTempMaterial : hs RestoreAndDestroyTempMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMaterialFromInfElem(  int hsid, int jointno, int partno, int vertno, E3DCOLOR3UC* infcolarray )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialFromInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMaterialFromInfElem( jointno, partno, vertno, infcolarray );
	if( ret ){
		DbgOut( "E3DSetMaterialFromInfElem : hs SetMaterialFromInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DUpdateSound()
{
	int ret;

	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}
	ret = s_HS->Update();
	if( ret ){
		DbgOut( "E3DUpdateSound : hs Update error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetSoundSamplesPlayed( int soundid, double* psmp )
{
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	*psmp = 0.0;
	ret = s_HS->GetSoundSamplesPlayed( soundid, psmp );
	if( ret ){
		DbgOut( "E3DGetSoundSamplesPlayed : hs GetSoundSamplesPlayed error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetSoundFrequency( int soundid, float* pfreq )
{
///////
	*pfreq = 1.0f;

	if( !s_HS ){
		DbgOut( "E3DGetSoundFrequency : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->GetFrequency( soundid, pfreq );
	if( ret ){
		DbgOut( "E3DGetSoundFrequency : hs GetFrequency error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetSoundFrequency( int soundid, float freq )
{
	if( !s_HS ){
		DbgOut( "E3DSetSoundFrequency : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetFrequency( soundid, freq );
	if( ret ){
		DbgOut( "E3DSetSoundFrequency : hs SetFrequency error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSet3DSoundEmiDist( int soundid, float dist )
{
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->Set3DEmiDist( soundid, dist );
	if( ret ){
		DbgOut( "E3DSet3DSoundEmiDist : hs Set3DEmiDist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DFadeSound( int soundid, int msec, float begingain, float endgain )
{
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->Fade( soundid, msec, begingain, endgain );
	if( ret ){
		DbgOut( "E3DFadeSound : hs Fade error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

EASY3D_API int E3DGlobalToLocalParBone( int hsid, int motid, int frameno, int boneno, int flag,
	int srcqid, int dstqid, D3DXVECTOR3* srcvecptr, D3DXVECTOR3* dstvecptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGlobalToLocalBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* srcqptr = 0;
	CQuaternion2* dstqptr = 0;
	if( flag & 1 ){
		if( !s_qh ){
			DbgOut( "E3DGlobalToLocalBone : qhandler not created error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = s_qh->GetQ( srcqid, &srcqptr );
		if( ret || !srcqptr ){
			DbgOut( "E3DGlobalToLocalBone : qh GetQ src error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = s_qh->GetQ( dstqid, &dstqptr );
		if( ret || !dstqptr ){
			DbgOut( "E3DGlobalToLocalBone : qh GetQ dst error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = curhs->GlobalToLocalParBone( motid, frameno, boneno, flag, srcqptr, dstqptr, srcvecptr, dstvecptr );
	if( ret ){
		DbgOut( "E3DGlobalToLocalBone : hs GlobalToLocalBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int E3DLocalToGlobalParBone( int hsid, int motid, int frameno, int boneno, int flag,
	int srcqid, int dstqid, D3DXVECTOR3* srcvecptr, D3DXVECTOR3* dstvecptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLocalToGlobalBone : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* srcqptr = 0;
	CQuaternion2* dstqptr = 0;
	if( flag & 1 ){
		if( !s_qh ){
			DbgOut( "E3DLocalToGlobalBone : qhandler not created error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = s_qh->GetQ( srcqid, &srcqptr );
		if( ret || !srcqptr ){
			DbgOut( "E3DLocalToGlobalBone : qh GetQ src error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = s_qh->GetQ( dstqid, &dstqptr );
		if( ret || !dstqptr ){
			DbgOut( "E3DLocalToGlobalBone : qh GetQ dst error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = curhs->LocalToGlobalParBone( motid, frameno, boneno, flag, srcqptr, dstqptr, srcvecptr, dstvecptr );
	if( ret ){
		DbgOut( "E3DLocalToGlobalBone : hs LocalToGlobalBone error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetInfElemSymParams( int hsid, int partno, int vertno, int* symtypeptr, float* distptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfElemSymParams : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetInfElemSymParams( partno, vertno, symtypeptr, distptr );
	if( ret ){
		DbgOut( "E3DGetInfElemSymParams : hs GetInfElemSymParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DDestroyMotion( int hsid, int motid, IDCHANGE* idchgptr, int idnum )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DestroyMotion : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyMotion( motid, idchgptr, idnum );
	if( ret ){
		DbgOut( "E3DDestroyMotion : hs DestroyMotion error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetIKTargetPos( int hsid, int boneno, D3DXVECTOR3* posptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3GetIKTargetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetIKTargetPos( boneno, posptr );
	if( ret ){
		DbgOut( "E3DGetIKTargetPos : hs GetIKTargetPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetIKTargetState( int hsid, int boneno, int* stateptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3GetIKTargetState : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetIKTargetState( boneno, stateptr );
	if( ret ){
		DbgOut( "E3DGetIKTargetState : hs GetIKTargetState error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DGetIKTargetCnt( int hsid, int boneno, int* cntptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3GetIKTargetCnt : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetIKTargetCnt( boneno, cntptr );
	if( ret ){
		DbgOut( "E3DGetIKTargetCnt : hs GetIKTargetCnt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DGetIKTargetLevel( int hsid, int boneno, int* levelptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3GetIKTargetLevel : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetIKTargetLevel( boneno, levelptr );
	if( ret ){
		DbgOut( "E3DGetIKTargetLevel : hs GetIKTargetLevel error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetIKLevel( int hsid, int boneno, int* levelptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIKLevel : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetIKLevel( boneno, levelptr );
	if( ret ){
		DbgOut( "E3DGetIKLevel : hs GetIKLevel error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetIKTargetPos( int hsid, int boneno, D3DXVECTOR3 srcpos )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKTargetPos : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetIKTargetPos( boneno, srcpos );
	if( ret ){
		DbgOut( "E3DSetIKTargetPos : hs SetIKTargetPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetIKTargetState( int hsid, int boneno, int srcstate )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKTargetState : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetIKTargetState( boneno, srcstate );
	if( ret ){
		DbgOut( "E3DSetIKTargetState : hs SetIKTargetState error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetIKTargetCnt( int hsid, int boneno, int srccnt )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKTargetCnt : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetIKTargetCnt( boneno, srccnt );
	if( ret ){
		DbgOut( "E3DSetIKTargetCnt : hs SetIKTargetCnt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DSetIKTargetLevel( int hsid, int boneno, int srclevel )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKTargetLevel : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetIKTargetLevel( boneno, srclevel );
	if( ret ){
		DbgOut( "E3DSetIKTargetLevel : hs SetIKTargetLevel error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetIKLevel( int hsid, int boneno, int srclevel )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKLevel : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetIKLevel( boneno, srclevel );
	if( ret ){
		DbgOut( "E3DSetIKLevel : hs SetIKLevel error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DGetQVal( int qid, E3DQ* qptr )
{
	if( !s_qh ){
		DbgOut( "E3DGetQVal : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DGetQVal : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	qptr->x = srcqptr->x;
	qptr->y = srcqptr->y;
	qptr->z = srcqptr->z;
	qptr->w = srcqptr->w;

	return 0;
}
EASY3D_API int E3DSetQVal( int qid, E3DQ srcq )
{
	if( !s_qh ){
		DbgOut( "E3DSetQVal : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* dstqptr;
	ret = s_qh->GetQ( qid, &dstqptr );
	if( ret ){
		DbgOut( "E3DSetQVal : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	dstqptr->x = srcq.x;
	dstqptr->y = srcq.y;
	dstqptr->z = srcq.z;
	dstqptr->w = srcq.w;
	dstqptr->twist = 0.0f;

	return 0;
}
EASY3D_API int E3DMultQArray( int* qidarray, int qnum, int resultid )
{
	if( !s_qh ){
		DbgOut( "E3DMultQArray : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = s_qh->InitQ( resultid );
	if( ret ){
		DbgOut( "E3DMultQArray : qh InitQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( qnum <= 0 ){
		return 0;
	}

	ret = s_qh->CopyQ( resultid, *qidarray );
	if( ret ){
		DbgOut( "E3DMultQArray : qh CopyQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int calcno;
	for( calcno = 1; calcno < qnum; calcno++ ){
		int aftqid = *( qidarray + calcno );
		ret = s_qh->MultQ( resultid, resultid, aftqid );
		if( ret ){
			DbgOut( "E3DMultQArray : qh MultQ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
EASY3D_API int E3DEulerZXY2Q( int qid, D3DXVECTOR3 srceul )
{
	if( !s_qh ){
		DbgOut( "E3DEulerZXY2Q : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* dstqptr;
	ret = s_qh->GetQ( qid, &dstqptr );
	if( ret ){
		DbgOut( "E3DEulerZXY2Q : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2 qx, qy, qz;
	D3DXVECTOR3 axisX( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisY( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZ( 0.0f, 0.0f, 1.0f );
	qx.SetAxisAndRot( axisX, srceul.x * (float)DEG2PAI );
	qy.SetAxisAndRot( axisY, srceul.y * (float)DEG2PAI );
	qz.SetAxisAndRot( axisZ, srceul.z * (float)DEG2PAI );

	*dstqptr = qy * qx * qz;
	dstqptr->twist = 0.0f;

	return 0;
}

EASY3D_API int E3DGetEulerLimit( int hsid, int boneno, EULERLIMIT* elptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetEulerLimit : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetEulerLimit( boneno, elptr );
	if( ret ){
		DbgOut( "E3DGetEulerLimit : hs GetEulerLimit error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetEulerLimit( int hsid, int boneno, EULERLIMIT srcel )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetEulerLimit : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetEulerLimit( boneno, srcel );
	if( ret ){
		DbgOut( "E3DSetEulerLimit : hs SetEulerLimit error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DCalcEuler( int hsid, int motid, int zakind )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcEuler : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcEuler( motid, zakind );
	if( ret ){
		DbgOut( "E3DCalcEuler : hs CalcEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetBoneEuler( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3* eulptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBoneEuler : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetBoneEuler( boneno, motid, frameno, eulptr );
	if( ret ){
		DbgOut( "GetBoneEuler : hs GetBoneEuler error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetZa4Elem( int hsid, int boneno, ZA4ELEM* z4eptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetZa4Elem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetZa4Elem( boneno, z4eptr );
	if( ret ){
		DbgOut( "E3DGetZa4Elem : hs GetZa4Elem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetZa4Elem( int hsid, int boneno, ZA4ELEM srcz4e )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetZa4Elem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetZa4Elem( boneno, srcz4e );
	if( ret ){
		DbgOut( "E3DSetZa4Elem : hs SetZa4Elem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DCalcZa4LocalQ( int hsid, int motid, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCalcZa4LocalQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CalcZa4LocalQ( motid, frameno );
	if( ret ){
		DbgOut( "E3DSetIKLevel : hs SetIKLevel error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetZa4LocalQ( int hsid, int boneno, E3DQ* dstq )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetZa4LocalQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetZa4LocalQ( boneno, dstq );
	if( ret ){
		DbgOut( "E3DGetZa4LocalQ : hs GetZa4LocalQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DGetZa3IniQ( int hsid, int boneno, E3DQ* dstq )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetZa3IniQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetZa3IniQ( boneno, dstq );
	if( ret ){
		DbgOut( "E3DGetZa3IniQ : hs GetZa3IniQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DGetBoneAxisQ( int hsid, int boneno, E3DQ* dstq )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBoneAxisQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetBoneAxisQ( boneno, dstq );
	if( ret ){
		DbgOut( "E3DGetBoneAxisQ : hs GetBoneAxisQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetBoneAxisQ( int hsid, int boneno, int motid, int frameno, int zakind )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBoneAxisQ : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetBoneAxisQ( boneno, motid, frameno, zakind );
	if( ret ){
		DbgOut( "E3DSetBoneAxisQ : hs SetBoneAxisQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetBoneTra( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3* traptr, int* existptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBoneTra : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetBoneTra( boneno, motid, frameno, traptr, existptr );
	if( ret ){
		DbgOut( "E3DGetBoneTra : hs GetBoneTra error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetBoneScale( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3* scaleptr, int* existptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBoneScale : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetBoneScale( boneno, motid, frameno, scaleptr, existptr );
	if( ret ){
		DbgOut( "E3DGetBoneScale : hs GetBoneScale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetBoneTra( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3 srctra )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBoneTra : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetBoneTra( boneno, motid, frameno, srctra );
	if( ret ){
		DbgOut( "E3DSetBoneTra : hs SetBoneTra error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetBoneScale( int hsid, int boneno, int motid, int frameno, D3DXVECTOR3 srcscale )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetBoneScale : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetBoneScale( boneno, motid, frameno, srcscale );
	if( ret ){
		DbgOut( "E3DSetBoneScale : hs SetBoneScale error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetBoneMatrix( int hsid, int boneno, int motid, int frameno, int kind, D3DXMATRIX* matptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetBoneMatrix : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetBoneMatrix( boneno, motid, frameno, kind, matptr );
	if( ret ){
		DbgOut( "E3DGetBoneMatrix : hs GetBoneMatrix error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DCreateMotionPoint( int hsid, int boneno, int motid, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateMotionPoint : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateMotionPoint( boneno, motid, frameno );
	if( ret ){
		DbgOut( "E3DCreateMotionPoint : hs CreateMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DTransform( int hsid, int boneonly )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DTransform : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev = 0;
	if( g_pD3DApp ){
		ret = g_pD3DApp->GetD3DDevice( &pdev );
	}
	if( !pdev ){
		DbgOut( "E3DTransform : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int lightflag = 1;
	ret = curhs->Transform( s_camerapos, pdev, s_matView, lightflag, boneonly );
	if( ret ){
		DbgOut( "E3DTransform : hs Transform error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetIKSkip( int hsid, int boneno, int* flagptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetIKSkip : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetIKSkip( boneno, flagptr );
	if( ret ){
		DbgOut( "E3DGetIKSkip : hs GetIKSkip error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetIKSkip( int hsid, int boneno, int flag )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetIKSkip : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetIKSkip( boneno, flag );
	if( ret ){
		DbgOut( "E3DSetIKSkip : hs SetIKSkip error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCreateTexKey( int hsid, int boneno, int motid, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateTexKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateTexKey( boneno, motid, frameno );
	if( ret ){
		DbgOut( "E3DCreateTexKey : hs CreateTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDelTexKey( int hsid, int boneno, int motid, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDelTexKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DelTexKey( boneno, motid, frameno );
	if( ret ){
		DbgOut( "E3DDelTexKey : hs DelTexKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DAddTexChange( int hsid, int boneno, int motid, int frameno, TEXCHANGE srctc )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddTexChange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->AddTexChange( boneno, motid, frameno, srctc );
	if( ret ){
		DbgOut( "E3DAddTexChange : hs AddTexChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDelTexChange( int hsid, int boneno, int motid, int frameno, int materialno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDelTexChange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DelTexChange( boneno, motid, frameno, materialno );
	if( ret ){
		DbgOut( "E3DDelTexChange : hs DelTexChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetTexChangeNum( int hsid, int boneno, int motid, int frameno, int* tcnumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetTexChangeNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetTexChangeNum( boneno, motid, frameno, tcnumptr );
	if( ret ){
		DbgOut( "E3DGetTexChangeNum : hs GetTexChangeNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetTexChange( int hsid, int boneno, int motid, int frameno, int tcindex, TEXCHANGE* dsttc )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetTexChange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetTexChange( boneno, motid, frameno, tcindex, dsttc );
	if( ret ){
		DbgOut( "E3DGetTexChange : hs GetTexChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetTexAnimKeyframeNoRange( int hsid, int boneno, int motid, 
		int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetTexAnimKeyframeNoRange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetTexAnimKeyframeNoRange( boneno, motid, srcstart, srcend, framearray, arrayleng, framenumptr );
	if( ret ){
		DbgOut( "E3DGetTexAnimKeyframeNoRange : hs GetTexAnimKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DCreateDSKey( int hsid, int boneno, int motid, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateDSKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CreateDSKey( boneno, motid, frameno );
	if( ret ){
		DbgOut( "E3DCreateDSKey : hs CreateDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDelDSKey( int hsid, int boneno, int motid, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDelDSKey : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DelDSKey( boneno, motid, frameno );
	if( ret ){
		DbgOut( "E3DDelDSKey : hs DelDSKey error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DAddDSChange( int hsid, int boneno, int motid, int frameno, DSCHANGE srcdsc )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAddDSChange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->AddDSChange( boneno, motid, frameno, srcdsc );
	if( ret ){
		DbgOut( "E3DAddDSChange : hs AddDSChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDelDSChange( int hsid, int boneno, int motid, int frameno, int switchno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDelDSChange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DelDSChange( boneno, motid, frameno, switchno );
	if( ret ){
		DbgOut( "E3DDelDSChange : hs DelDSChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetDSChangeNum( int hsid, int boneno, int motid, int frameno, int* dscnumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDSChangeNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetDSChangeNum( boneno, motid, frameno, dscnumptr );
	if( ret ){
		DbgOut( "E3DGetDSChangeNum : hs GetDSChangeNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetDSChange( int hsid, int boneno, int motid, int frameno, int dscindex, DSCHANGE* dstdsc )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDSChange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetDSChange( boneno, motid, frameno, dscindex, dstdsc );
	if( ret ){
		DbgOut( "E3DGetDSChange : hs GetDSChange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetDSAnimKeyframeNoRange( int hsid, int boneno, int motid, 
		int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetDSAnimKeyframeNoRange : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetDSAnimKeyframeNoRange( boneno, motid, srcstart, srcend, framearray, arrayleng, framenumptr );
	if( ret ){
		DbgOut( "E3DGetDSAnimKeyframeNoRange : hs GetDSAnimKeyframeNoRange error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMaterialTexParams( int hsid, int materialno, MATERIALTEX* dstparams )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialTexParams : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !dstparams ){
		DbgOut( "E3DGetMaterialTexParams : dstparams NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetMaterialTexParams( materialno, dstparams );
	if( ret ){
		DbgOut( "E3DGetMaterialTexParams : hs GetMaterialTexParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMaterialTexParams( int hsid, int materialno, MATERIALTEX* srcparams )
{
	int ret;
	LPDIRECT3DDEVICE9 pdev = 0;
	if( g_pD3DApp ){
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		_ASSERT( !ret );
	}
	if( !pdev ){
		DbgOut( "E3DSetMaterialTexParams : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialTexParams : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcparams ){
		DbgOut( "E3DSetMaterialTexParams : srcparams NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = curhs->SetMaterialTexParams( pdev, s_hwnd, materialno, srcparams );
	if( ret ){
		DbgOut( "E3DSetMaterialTexParams : hs SetMaterialTexParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMaterialTexParams1DIM( int hsid, int materialno, MATERIALTEX1DIM* dstparams )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialTexParams1DIM : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !dstparams ){
		DbgOut( "E3DGetMaterialTexParams1DIM : dstparams NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetMaterialTexParams1DIM( materialno, dstparams );
	if( ret ){
		DbgOut( "E3DGetMaterialTexParams1DIM : hs GetMaterialTexParams1DIM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMaterialTexParams1DIM( int hsid, int materialno, MATERIALTEX1DIM* srcparams )
{
	int ret;
	LPDIRECT3DDEVICE9 pdev = 0;
	if( g_pD3DApp ){
		ret = g_pD3DApp->GetD3DDevice( &pdev );
		_ASSERT( !ret );
	}
	if( !pdev ){
		DbgOut( "E3DSetMaterialTexParams1DIM : device NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMaterialTexParams1DIM : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcparams ){
		DbgOut( "E3DSetMaterialTexParams1DIM : srcparams NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = curhs->SetMaterialTexParams1DIM( pdev, s_hwnd, materialno, srcparams );
	if( ret ){
		DbgOut( "E3DSetMaterialTexParams1DIM : hs SetMaterialTexParams1DIM error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSet3DSoundDopplerScaler( int soundid, float srcdp )
{
	if( !s_HS ){
		DbgOut( "E3DSet3DSoundDopplerScaler : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->SetDopplerScaler( soundid, srcdp );
	if( ret ){
		DbgOut( "E3DSet3DSoundDopplerScaler : hs SetDopplerScaler error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGet3DSoundDopplerScaler( int soundid, float* dpptr )
{
	if( !s_HS ){
		DbgOut( "E3DGet3DSoundDopplerScaler : Sound object not initialized error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = s_HS->GetDopplerScaler( soundid, dpptr );
	if( ret ){
		DbgOut( "E3DGet3DSoundDopplerScaler : hs GetDopplerScaler error %d !!!\n", soundid );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DLoadMQOFileAsChara( const char* tempname1, float mult, int adjustuvflag, int bonetype, int* hsidptr )
{
	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadMQOFileAsChara : filename1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsChara_F( fname1, mult, s_hwnd, adjustuvflag, &temphsid, bonetype );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsChara : LoadMQOFileAsChara_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = temphsid;

	return 0;
}

int LoadMQOFileAsChara_F( char* tempname1, float mult, HWND hwnd, int adjustuvflag, int* hsidptr, int bonetype )
{
	int ret = 0;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname1, newhs );//!!!!!!

//	newhs->m_TLmode = 2;//!!!!!!!!!!!! ver1036
//	newhs->m_groundflag = 1;//!!!!!!!! ver1036

	ret = newhs->LoadMQOFileAsChara( tempname1, mult, hwnd, adjustuvflag, hsidptr, bonetype );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsChara : curhs LoadMQOFileAsChara error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );


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
	*hsidptr = newhs->serialno;
	return 0;

}

EASY3D_API int E3DGetPartPropName( int hsid, int partno, char* nameptr, int arrayleng )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPartPropName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetPartPropName( partno, nameptr, arrayleng );
	if( ret ){
		DbgOut( "E3DGetPartPropName : hs GetPartPropName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetPartPropName( int hsid, int partno, char* srcname )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPartPropName : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetPartPropName( partno, srcname );
	if( ret ){
		DbgOut( "E3DSetPartPropName : hs SetPartPropName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetPartPropDS( int hsid, int partno, int* dsptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPartPropDS : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetPartPropDS( partno, dsptr );
	if( ret ){
		DbgOut( "E3DGetPartPropDS : hs GetPartPropDS error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetPartPropDS( int hsid, int partno, int srcds )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPartPropDS : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetPartPropDS( partno, srcds );
	if( ret ){
		DbgOut( "E3DSetPartPropDS : hs SetPartPropDS error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetPartPropNotUse( int hsid, int partno, int* flagptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetPartPropNotUse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetPartPropNotUse( partno, flagptr );
	if( ret ){
		DbgOut( "E3DGetPartPropNotUse : hs GetPartPropNotUse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetPartPropNotUse( int hsid, int partno, int srcflag )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetPartPropNotUse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetPartPropNotUse( partno, srcflag );
	if( ret ){
		DbgOut( "E3DSetPartPropNotUse : hs SetPartPropNotUse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DLoadSSF( const char* filename )
{
	if( !s_HS || !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}

	char srcfilename[MAX_PATH];
	ZeroMemory( srcfilename, sizeof( char ) * MAX_PATH );
	strcpy_s( srcfilename, MAX_PATH, filename );

	int ret;
	if( s_soundbank ){
		delete s_soundbank;
		s_soundbank = 0;

		s_soundbank = new CSoundBank();
		if( !s_soundbank ){
			DbgOut( "E3DLoadSSF : new SoundBank error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( s_sndah ){
		delete s_sndah;
		s_sndah = 0;

		s_sndah = new CSndAnimHandler();
		if( !s_sndah ){
			DbgOut( "E3DLoadSSF : new SndAH error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	CSndSetFile ssfile( s_HS, s_soundbank );
	ret = ssfile.LoadSndSetFile( srcfilename );
	if( ret ){
		DbgOut( "E3DLoadSSF : ssf LoadSndSetFile error !!!\n" );
		_ASSERT( 0 );
//		if( m_soundbank ){
//			delete m_soundbank;
//			m_soundbank = 0;
//
//			m_soundbank = new CSoundBank();
//			if( !m_soundbank ){
//				_ASSERT( 0 );
//			}
//		}
//		g_motdlg->m_snddlg->RemakeSndAH();
	
		return 1;
	}
	

	return 0;
}
EASY3D_API int E3DDestroyAllSoundSet()
{
	if( s_soundbank ){
		delete s_soundbank;
		s_soundbank = 0;

		s_soundbank = new CSoundBank();
		if( !s_soundbank ){
			DbgOut( "E3DLoadSSF : new SoundBank error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
EASY3D_API int E3DGetSoundSetNum( int* numptr )
{
	*numptr = 0;

	if( !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}

	*numptr = s_soundbank->m_setnum;

	return 0;
}
EASY3D_API int E3DGetSoundSetInfo( int ssindex, E3DSSINFO* ssinfoptr )
{
	ZeroMemory( ssinfoptr, sizeof( E3DSSINFO ) );

	if( !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}
	int ssnum = s_soundbank->m_setnum;

	if( (ssindex < 0) || (ssindex >= ssnum) ){
		DbgOut( "E3DGetSoundSetInfo : ssindex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundSet* curss;
	curss = *( s_soundbank->m_ppset + ssindex );
	_ASSERT( curss );

	int ret;
	ret = curss->GetName( ssinfoptr->setname, 32 );
	if( ret ){
		DbgOut( "E3DGetSoundSetInfo : ss GetName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	( ssinfoptr->setid ) = curss->m_setid;

	return 0;
}
EASY3D_API int E3DGetSoundSetBGM( int setid, int* bgmidptr )
{
	*bgmidptr = -1;

	if( !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}

	CSoundSet* curss = 0;
	s_soundbank->GetSoundSet( setid, &curss );
	if( !curss ){
		DbgOut( "E3DGetSoundSetBGM : sb GetSoundSet : setid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curss->m_bgm ){
		*bgmidptr = curss->m_bgm->m_serialno;
	}else{
		*bgmidptr = -1;
	}

	return 0;
}

EASY3D_API int E3DLoadSAF( const char* filename, int* animnoptr )
{
	int ret;
	*animnoptr = -1;

	if( !s_soundbank || !s_sndah ){
		_ASSERT( 0 );
		return 1;
	}

	char srcfilename[MAX_PATH];
	ZeroMemory( srcfilename, sizeof( char ) * MAX_PATH );
	strcpy_s( srcfilename, MAX_PATH, filename );

	CSndFile sndfile;
	CSndAnim* anim = 0;
	ret = sndfile.LoadSndFile( srcfilename, s_sndah, s_soundbank, &anim );
	if( ret ){
		DbgOut( "E3DLoadSAF : sndfile LoadSndFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int animno = anim->m_motkind;

	CSoundSet* curss = 0;
	ret = s_soundbank->GetSoundSet( anim->m_ssname, &curss );
	if( ret || !curss ){
		DbgOut( "E3DLoadSAF : sb GetSoundBank error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s_sndah->m_animnum > MAXMOTIONNUM ){
		DbgOut( "E3DLoadSAF : animnum overflow error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = s_sndah->SetCurrentAnim( animno );
	if( ret ){
		DbgOut( "E3DLoadSAF : cah SetCurrentAnim error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = CalcSndFrameData( animno );
	if( ret ){
		DbgOut( "E3DLoadSAF : CalcSndFrameData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*animnoptr = animno;

	return 0;
}
EASY3D_API int E3DDestroyAllSndAnim()
{
	if( s_sndah ){
		delete s_sndah;
		s_sndah = 0;

		s_sndah = new CSndAnimHandler();
		if( !s_sndah ){
			DbgOut( "E3DLoadSSF : new SndAH error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
EASY3D_API int E3DGetSndAnimFrameNum( int animno, int* numptr )
{
	*numptr = 0;

	if( !s_sndah ){
		_ASSERT( 0 );
		return 1;
	}

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetSndAnimFrameNum : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*numptr = ( anim->m_maxframe + 1 );

	return 0;
}
EASY3D_API int E3DGetSndAnimBGM( int animno, int* bgmidptr )
{
	int ret;
	*bgmidptr = -1;

	if( !s_sndah || !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetSndAnimBGM : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundSet* curss = 0;
	ret = s_soundbank->GetSoundSet( anim->m_ssname, &curss );
	if( ret || !curss ){
		DbgOut( "E3DGetSndAnimBGM : sb GetSoundSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundElem* curse = curss->m_bgm;
	if( curse ){
		*bgmidptr = curse->m_serialno;
	}else{
		*bgmidptr = -1;
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetSndAnimFPS( int animno, int* fpsptr )
{

	*fpsptr = 30;

	if( !s_sndah || !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetSndAnimFPS : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*fpsptr = anim->m_fps;

	return 0;
}
EASY3D_API int E3DGetSndAnimMode( int animno, int* modeptr )
{

	*modeptr = 0;

	if( !s_sndah || !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		DbgOut( "E3DGetSndAnimMode : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*modeptr = anim->m_bgmloop;

	return 0;
}
EASY3D_API int E3DStartSndAnim( int animno, int startframe )
{
	int ret;
	if( !s_sndah || !s_soundbank ){
		_ASSERT( 0 );
		return 1;
	}

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		DbgOut( "E3DStartSndAnim : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundSet* curss = 0;
	ret = s_soundbank->GetSoundSet( anim->m_ssname, &curss );
	if( ret || !curss ){
		DbgOut( "E3DStartSndAnim : sb GetSoundSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int maxframe;
	maxframe = anim->m_maxframe;
	if( (startframe < 0) || (startframe > maxframe) ){
		DbgOut( "E3DStartSndAnim : startframe error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_sndstartframe = startframe;


	ret = anim->ResetDoneFlag( s_sndstartframe );
	_ASSERT( !ret );

	ret = anim->SetSndAnimFrameNo( s_sndstartframe );
	_ASSERT( !ret );

	CSndFrame* curframe = anim->m_curframe;

	double cursample = 0.0;
	ret = SCConvFrame2Sample( curss, anim->m_fps, s_sndstartframe, &cursample );
	_ASSERT( !ret );

	ret = s_HS->PlayFrame( curss->m_bgm->m_serialno, (int)cursample, 0, curframe );
	_ASSERT( !ret );

	return 0;
}
EASY3D_API int E3DSetSndAnimFrameNo( int animno, int srcframeno )
{
	int ret;

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetSndAnimFrameNo : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundSet* curss = 0;
	ret = s_soundbank->GetSoundSet( anim->m_ssname, &curss );
	if( ret || !curss ){
		DbgOut( "E3DSetSndAnimFrameNo : sb GetSoundSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int maxframe;
	maxframe = anim->m_maxframe;

	int frameno;
	frameno = srcframeno;
	frameno = min( frameno, maxframe );
//	frameno = max( frameno, s_sndstartframe );
	frameno = max( frameno, 0 );

	ret = anim->SetSndAnimFrameNo( frameno );
	_ASSERT( !ret );

	CSndFrame* curframe = anim->m_curframe;

	ret = s_HS->PlayFrame( -1, 0, 0, curframe );
	_ASSERT( !ret );

	return 0;
}
EASY3D_API int E3DStopSndAnim( int animno )
{
	int ret;

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		DbgOut( "E3DStopSndAnim : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CSoundSet* curss = 0;
	ret = s_soundbank->GetSoundSet( anim->m_ssname, &curss );
	if( ret || !curss ){
		DbgOut( "E3DStopSndAnim : sb GetSoundSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->StopSound( curss->m_bgm->m_serialno );
	_ASSERT( !ret );

	//m_dlg->m_sndframe = m_startframe;

	Sleep( 50 );

	return 0;
}
EASY3D_API int E3DGetSoundTotalTime( int soundid, double* timeptr )
{
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->GetTotalTime( soundid, timeptr );
	if( ret ){
		DbgOut( "E3DGetSoundTotalTime : hs GetTotalTime error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DConvSoundSampleToFrame( int soundid, int srcfps, double srcsample, int* frameptr )
{
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	DWORD samplespersec = 0;
	ret = s_HS->GetSamplesPerSec( soundid, &samplespersec );
	if( ret || (samplespersec <= 0) ){
		*frameptr = 0;
		return 0;
	}

	double dframe;
	dframe = srcsample / (double)samplespersec * (double)srcfps;
	*frameptr = (int)( dframe + 0.5 );

	return 0;
}
EASY3D_API int E3DGetSoundStartCnt( int soundid, int* cntptr )
{
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->GetStartCnt( soundid, cntptr );
	if( ret ){
		DbgOut( "E3DGetSoundStartCnt : hs GetStartCnt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetSoundEndCnt( int soundid, int* cntptr )
{
	int ret;
	if( !s_HS ){
		_ASSERT( 0 );
		return 1;
	}

	ret = s_HS->GetStopCnt( soundid, cntptr );
	if( ret ){
		DbgOut( "E3DGetSoundEndCnt : hs GetStopCnt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DPCWait( int srcfps, double* dstfps )
{
	_ASSERT( m_dfreq > 0.0 );
	m_drenderfreq = m_dfreq / (double)srcfps;

	double ddiff;

	QueryPerformanceCounter( &m_curcnt );
	m_dcurcnt = (double)m_curcnt.QuadPart;

	ddiff = m_dcurcnt - m_drendercnt;
	while( (ddiff > 0.0) && (ddiff < m_drenderfreq) ){
		QueryPerformanceCounter( &m_curcnt );
		m_dcurcnt = (double)m_curcnt.QuadPart;
		ddiff = m_dcurcnt - m_drendercnt;

		Sleep( 0 );
	}
	m_rendercnt.QuadPart = m_curcnt.QuadPart;
	m_drendercnt = m_dcurcnt;

	if( ddiff != 0.0 )
		*dstfps = 1.0 / (ddiff / m_dfreq);
	else
		*dstfps = 0.0;

	return 0;
}

int CalcSndFrameData( int animno )
{
	if( !s_sndah || !s_soundbank ){
		return 0;
	}

	CSndAnim* anim;
	anim = s_sndah->GetSndAnim( animno );
	if( !anim ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	CSoundSet* curss = 0;
	s_soundbank->GetSoundSet( anim->m_ssname, &curss );
	if( curss ){
		ret = anim->CheckAndDelInvalid( curss );
		_ASSERT( !ret );

		ret = anim->SetFrameData();
		_ASSERT( !ret );
	}else{
		_ASSERT( 0 );
	}

	return 0;
}

EASY3D_API int E3DCloneDispObj( int hsid, int partno, char* newname, int* newseriptr )
{
	int ret;
	LPDIRECT3DDEVICE9 pdev = 0;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCloneDispObj : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = curhs->CloneDispObj( pdev, s_hwnd, partno, newname, newseriptr );
	if( ret ){
		DbgOut( "E3DCloneDispObj : hs CloneDispObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetJointNotSel( int hsid, int boneno, int* notselptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetJointNotUse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->GetJointNotSel( boneno, notselptr );
	if( ret ){
		DbgOut( "E3DGetJointNotUse : hs GetJointNotUse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetJointNotSel( int hsid, int boneno, int notsel )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetJointNotUse : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->SetJointNotSel( boneno, notsel );
	if( ret ){
		DbgOut( "E3DSetJointNotUse : hs SetJointNotUse error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DDestroyMorph( int hsid, int baseno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyMorph : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->DestroyMorph( baseno );
	if( ret ){
		DbgOut( "E3DDestroyMorph : hs DesetroyMorph error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
EASY3D_API int E3DDestroyDispObj( int hsid, int partno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyDispObj : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->DestroyDispObj( partno );
	if( ret ){
		DbgOut( "E3DDestroyDispObj : hs DestroyDispObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

EASY3D_API int E3DSetCameraOffset( D3DXVECTOR3 offbef, int offqid, D3DXVECTOR3 offaft )
{
	int ret;
	if( !s_qh ){
		DbgOut( "E3DSetCameraOffset : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcq = 0;
	ret = s_qh->GetQ( offqid, &srcq );
	if( ret ){
		DbgOut( "E3DSetCameraOffset : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_camoffq.SetParams( srcq->w, srcq->x, srcq->y, srcq->z, srcq->twist );

	s_camoffbef = offbef;
	s_camoffaft = offaft;

	CalcMatView();

	return 0;
}

EASY3D_API int E3DAllClearInfElem( int hsid, int partno, int vertno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DAllClearInfElem : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = curhs->AllClearInfElem( partno, vertno );
	if( ret ){
		DbgOut( "E3DAllClearInfElem : hs AllClearInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DSetGPOffset( int hsid, D3DXVECTOR3 tra, int qid )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetGPOffset : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	if( !s_qh ){
		DbgOut( "E3DSetGPOffset : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DSetGPOffset : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !srcqptr ){
		DbgOut( "E3DSetGPOffset : srcqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->SetGPOffset( tra, srcqptr );
	if( ret ){
		DbgOut( "E3DSetGPOffset : hs SetGPOffset error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DSelectLightType( int ltype )
{
	s_SelectLightType = ltype;
	return 0;
}

EASY3D_API int E3DSetSpotLight( int lid, D3DXVECTOR3 pos, D3DXVECTOR3 dir, float dist, float angle, E3DCOLOR4UC col )
{

///////
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	CLightData* curlight;
	curlight = GetLightData( lid );
	if( !curlight ){
		DbgOut( "E3DSetSpotLight : lightID not found error !!!\n" );
		return 1;
	}

	DXVec3Normalize( &dir, &dir );

	CVec3f difcol;
	difcol.x = (float)col.r / 255.0f;
	difcol.y = (float)col.g / 255.0f;
	difcol.z = (float)col.b / 255.0f;

	int ret;
	ret = curlight->SetSpotLight( pos, dir, dist, angle, difcol );
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

	light.Range = dist;

	light.Falloff = 1.0f;

    light.Attenuation0 = 1.0f;
    
	light.Theta        = angle * (float)DEG2PAI;
    light.Phi          = angle * (float)DEG2PAI;

    pdev->SetLight( lid, &light );
	pdev->LightEnable( lid, TRUE );

	return 0;
}

EASY3D_API int E3DRenderZandN( int outzscid, int outnscid, int inzscid, int hsid, int skipflag, int renderkind )
{
	if( outzscid < 0 ){
		DbgOut( "E3DRender : outzscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( outnscid < 0 ){
		DbgOut( "E3DRender : outnscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( inzscid < 0 ){
		DbgOut( "E3DRender : inzscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	//int skipflag;//|1-->outz_zskip,  |2-->outn_zskip, |4-->inz zskip, |8-->transskip, |16-->beginscene skip
	//int renderkind;//|1-->outz, |2-->outn, |4-->outn proj mode(2があるときのみ有効)

//////////
	if( !(renderkind & 1) && !(renderkind & 2) ){
		return 0;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRender : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
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

	CalcMatView();
	ret = SetShaderConst( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( !(skipflag & 8) ){
		int siglightflag = 0;
		ret = curhs->Transform( s_camerapos, pdev, s_matView, siglightflag, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}


	HRESULT hres;
	SWAPCHAINELEM* outzsc = 0;
	SWAPCHAINELEM* outnsc = 0;
	SWAPCHAINELEM* inzsc = 0;

	inzsc = GetSwapChainElem( inzscid );
	if( !inzsc ){
		DbgOut( "E3DRender : inzscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	int skipflag;//|1-->outz_zskip,  |2-->outn_zskip, |4-->inz zskip, |8-->transskip, |16-->beginscene skip
	if( !(skipflag & 16) ){
		if( FAILED( pdev->BeginScene() ) ){
			_ASSERT( 0 );
			return 1;
		}
	}

	D3DVIEWPORT9 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Height = g_scsize.y;
	viewport.Width =  g_scsize.x;
	viewport.MaxZ = 1.0f;
	viewport.MinZ = 0.0f;

	hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "RTBeginScene : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int znflag;
	if( renderkind & 1 ){
		outzsc = GetSwapChainElem( outzscid );
		if( !outzsc ){
			DbgOut( "E3DRender : outzscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = pdev->SetRenderTarget( 0, outzsc->pBackBuffer );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetRenderTarget outz error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

		hres = pdev->SetDepthStencilSurface( inzsc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

	
		////////////////


		// Clear the viewport
	//	int skipflag;//|1-->outz_skip,  |2-->outn_skip, |4-->inz zskip, |8-->transskip, |16-->beginscene skip
		int clcolflag =  D3DCLEAR_TARGET;
		int clzflag = D3DCLEAR_ZBUFFER;
		if( skipflag & 1 ){
			clcolflag = 0;
		}
		if( skipflag & 4 ){
			clzflag = 0;
		}

		if( clcolflag || clzflag ){
			hres = pdev->Clear( 0L, NULL, clcolflag | clzflag, 
								 outzsc->clearcol,
								 1.0f, 0L );
		}
		_ASSERT( !hres );
	////////////////
		znflag = 1;//!!!!!!!!!!!!
		int withalpha;
		withalpha = 0;
		ret = curhs->RenderZandN( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		withalpha = 1;
		ret = curhs->RenderZandN( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( renderkind & 2 ){
		outnsc = GetSwapChainElem( outnscid );
		if( !outnsc ){
			DbgOut( "E3DRender : outnscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		hres = pdev->SetRenderTarget( 0, outnsc->pBackBuffer );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetRenderTarget outn error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
		hres = pdev->SetDepthStencilSurface( inzsc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

		int clcolflag =  D3DCLEAR_TARGET;
		int clzflag = D3DCLEAR_ZBUFFER;
		if( skipflag & 2 ){
			clcolflag = 0;
		}
		if( skipflag & 4 ){
			clzflag = 0;
		}

		if( clcolflag || clzflag ){
			hres = pdev->Clear( 0L, NULL, clcolflag | clzflag, 
								 outnsc->clearcol,
								 1.0f, 0L );
		}
		_ASSERT( !hres );

		if( renderkind & 4 ){
			znflag = 3;//!!!!!!!!!!!
		}else{
			znflag = 2;//!!!!!!!!!!!
		}
		int withalpha = 0;
		ret = curhs->RenderZandN( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		withalpha = 1;
		ret = curhs->RenderZandN( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( !(skipflag & 16) ){
		if( FAILED( pdev->EndScene() ) ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

EASY3D_API int E3DCreateInfScope( int hsid, int boneno, int dispno, int* isindexptr )
{
	*isindexptr = -1;
	int ret;
	LPDIRECT3DDEVICE9 pdev = 0;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCreateInfScope : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	ret = curhs->CreateInfScope( pdev, s_hwnd, boneno, dispno, isindexptr );
	if( ret ){
		DbgOut( "E3DCreateInfScope : hs CreateInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDestroyInfScope( int hsid, int isindex )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyInfScope : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->DestroyInfScope( isindex );
	if( ret ){
		DbgOut( "E3DDestroyInfScope : hs DestroyInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DExistInfScope( int hsid, int boneno, int dispno, int* isindexptr )
{
	*isindexptr = -1;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DExistInfScope : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->ExistInfScope( boneno, dispno, isindexptr );
	if( ret ){
		DbgOut( "E3DExistInfScope : hs ExistInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetInfScopeNum( int hsid, int* numptr )
{
	*numptr = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfScopeNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetInfScopeNum( numptr );
	if( ret ){
		DbgOut( "E3DGetInfScopeNum : hs GetInfScopeNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetInfScopeInfo( int hsid, ISINFO* isiptr, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfScopeInfo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetInfScopeInfo( isiptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DGetInfScopeInfo : hs GetInfScopeInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetInfScopeVert( int hsid, ISVERT srcisv )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfScopeVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->SetInfScopeVert( srcisv );
	if( ret ){
		DbgOut( "E3DSetInfScopeVert : hs SetInfScopeVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetInfScopeVert( int hsid, ISVERT* isvptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfScopeVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetInfScopeVert( isvptr );
	if( ret ){
		DbgOut( "E3DGetInfScopeVert : hs GetInfScopeVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetInfScopeVertNum( int hsid, int isindex, int* numptr )
{
	*numptr = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfScopeVertNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetInfScopeVertNum( isindex, numptr );
	if( ret ){
		DbgOut( "E3DGetInfScopeVertNum : hs GetInfScopeVertNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DPickInfScopeVert( int scid, int hsid, POINT scpos, POINT range, int isindex, ISVERT* isvptr, int arrayleng, int* getnumptr )
{
	*getnumptr = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DPickInfScopeVert : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DPickInfScopeVert : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->PickInfScopeVert( s_matView, scpos, range, isindex, isvptr, arrayleng, getnumptr );
	if( ret ){
		DbgOut( "E3DPickInfScopeVert : hs PickInfScopeVert error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DGetInfScopeAxis( int hsid, int isindex, D3DXVECTOR3* dstvec3x3ptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfScopeAxis : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetInfScopeAxis( isindex, dstvec3x3ptr );
	if( ret ){
		DbgOut( "E3DGetInfScopeAxis : hs GetInfScopeAxis error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetInfScopeColor( int hsid, int isindex, E3DCOLOR4UC srccol )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfScopeColor : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->SetInfScopeColor( isindex, srccol );
	if( ret ){
		DbgOut( "E3DSetInfScopeColor : hs SetInfScopeColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetInfScopeColor( int hsid, int isindex, E3DCOLOR4UC* dstcolptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfScopeColor : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetInfScopeColor( isindex, dstcolptr );
	if( ret ){
		DbgOut( "E3DGetInfScopeColor : hs GetInfScopeColor error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetInfScopeDispFlag( int hsid, int isindex, int srcdispflag, int exclusiveflag )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetInfScopeDispFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->SetInfScopeDispFlag( isindex, srcdispflag, exclusiveflag );
	if( ret ){
		DbgOut( "E3DSetInfScopeDispFlag : hs SetInfScopeDispFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetInfScopeDispFlag( int hsid, int isindex, int* dstflagptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInfScopeDispFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	ret = curhs->GetInfScopeDispFlag( isindex, dstflagptr );
	if( ret ){
		DbgOut( "E3DGetInfScopeDispFlag : hs GetInfScopeDispFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DImportMQOAsInfScope( int hsid, char* filename, float mult, int boneno, int dispno, int* isindexptr )
{
	*isindexptr = -1;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DImportMQOAsInfScope : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

	_ASSERT( s_hwnd );

	ret = curhs->ImportMQOAsInfScope( pdev, s_hwnd, filename, mult, boneno, dispno, isindexptr );
	if( ret ){
		DbgOut( "E3DImportMQOAsInfScope : hs ImportMQOAsInfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMOELocalMotionFrameNo( int hsid, int moeanimno, int moeframeno, int* localanimno, int* localframeno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOELocalMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOELocalMotionFrameNo( moeanimno, moeframeno, localanimno, localframeno );
	if( ret ){
		DbgOut( "E3DMOELocalMotionFrameNo : hs GetMOELocalMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMOELocalNextMotionFrameNo( int hsid, int moeanimno, int moeframeno, int* localnextanimno, int* localnextframeno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOELocalMotionFrameNo : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOELocalNextMotionFrameNo( moeanimno, moeframeno, localnextanimno, localnextframeno );
	if( ret ){
		DbgOut( "E3DMOELocalNextMotionFrameNo : hs GetMOELocalNextMotionFrameNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DQtoEulerAxis( int qid, int axisqid, D3DXVECTOR3* eulptr, D3DXVECTOR3 befeul )
{
	if( !s_qh ){
		DbgOut( "E3DQtoEulerAxis : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	CQuaternion2* srcqptr;
	ret = s_qh->GetQ( qid, &srcqptr );
	if( ret ){
		DbgOut( "E3DQtoEulerAxis : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CQuaternion2* axisqptr;
	ret = s_qh->GetQ( axisqid, &axisqptr );
	if( ret ){
		DbgOut( "E3DQtoEulerAxis : qh GetQ axisq error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion tmpq;
	tmpq.x = srcqptr->x;
	tmpq.y = srcqptr->y;
	tmpq.z = srcqptr->z;
	tmpq.w = srcqptr->w;

	CQuaternion tmpaxis;
	tmpaxis.x = axisqptr->x;
	tmpaxis.y = axisqptr->y;
	tmpaxis.z = axisqptr->z;
	tmpaxis.w = axisqptr->w;


	D3DXVECTOR3 neweul;
	ret = qToEulerAxis( tmpaxis, &tmpq, &neweul );
	if( ret ){
		DbgOut( "E3DQtoEulerAxis : qToEulerAxis error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = modifyEuler( &neweul, &befeul );
	_ASSERT( !ret );

	*eulptr = neweul;



	return 0;
}

EASY3D_API int E3DSetMOEKeyGoOnFlag( int hsid, int animno, int frameno, int flag )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOEKeyGoOnFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetMOEKeyGoOnFlag( animno, frameno, flag );
	if( ret ){
		DbgOut( "E3DSetMOEKeyGoOnFlag : hs SetMOEKeyGoOnFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DGetMOEKeyGoOnFlag( int hsid, int animno, int frameno, int* flagptr )
{
	int flag = 0;
	*flagptr = flag;


	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOEKeyGoOnFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOEKeyGoOnFlag( animno, frameno, &flag );
	if( ret ){
		DbgOut( "E3DGetMOEKeyGoOnFlag : hs GetMOEKeyGoOnFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*flagptr = flag;


	return 0;
}

EASY3D_API int E3DSetMOATrunkNotComID( int hsid, int motid, int idnum, int* srcid )
{
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMOATrunkNotComID : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( idnum <= 0 ){
		_ASSERT( 0 );
		return 0;
	}

	ret = curhs->SetMOATrunkNotComID( motid, idnum, srcid );
	if( ret ){
		DbgOut( "E3DGetMOATrunkNotComID : hs GetMOATrunkNotComID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DGetMOELocalMotionFrameNoML( int hsid, int moeanimno, int moeframeno, int boneid, int* localanimno, int* localframeno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOELocalMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOELocalMotionFrameNoML( moeanimno, moeframeno, boneid, localanimno, localframeno );
	if( ret ){
		DbgOut( "E3DMOELocalMotionFrameNoML : hs GetMOELocalMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMOELocalNextMotionFrameNoML( int hsid, int moeanimno, int moeframeno, int boneid, int* localnextanimno, int* localnextframeno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMOELocalMotionFrameNoML : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMOELocalNextMotionFrameNoML( moeanimno, moeframeno, boneid, localnextanimno, localnextframeno );
	if( ret ){
		DbgOut( "E3DMOELocalNextMotionFrameNoML : hs GetMOELocalNextMotionFrameNoML error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetDefaultCamera( int camno, E3DCAMERASTATE camstate )
{
	//camno -1禁止
	_ASSERT( m_cah );
	int ret;

	CAMERAELEM ce;
	ZeroMemory( &ce, sizeof( CAMERAELEM ) );

	ce.looktype = camstate.looktype;
	ce.pos = camstate.pos;
	ce.target = camstate.target;
	ce.up = camstate.up;
	ce.dist = camstate.dist;
	ce.hsid = camstate.hsid;
	ce.boneno = camstate.boneno;
	ce.nearz = camstate.nearz;
	ce.farz = camstate.farz;
	ce.fov = camstate.fov;

	ce.ortho = camstate.ortho;
	ce.orthosize = camstate.orthosize;

	ret = m_cah->SetDefaultCamera( camno, ce );
	if( ret ){
		DbgOut( "E3DSetDefaultCamera : cah SetDefaultCamera error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetDefaultGP( int hsid, E3DGPSTATE gpstate )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDefaultGP : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetDefaultGP( gpstate );
	if( ret ){
		DbgOut( "E3DSetDefaultGP : hs SetDefaultGP error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DBeginSceneSimple( int skipflag )
{
	int ret = 0;
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
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

	if( (skipflag & 1) == 0 ){
		CalcMatView();

		ret = SetShaderConst( 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( FAILED( pdev->BeginScene() ) ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DEndSceneSimple()
{
	int ret = 0;
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
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


	if( FAILED( pdev->EndScene() ) ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DShaderConstZN( int hsid, unsigned char dataz, unsigned char datan )
{
	float setuser[2];

	setuser[0] = (float)dataz / 255.0f;
	setuser[1] = (float)datan / 255.0f;

	HRESULT hres;
	hres = g_pEffect->SetValue( g_hmZNUser1, setuser, sizeof( float ) * 2 );
	if( hres != D3D_OK ){
		DbgOut( "E3DShaderConstZN : SetValue error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DShaderConstZN2( int hsid, unsigned char data[4] )
{
	float setuser[4];

	int uno;
	for( uno = 0; uno < 4; uno++ ){
		setuser[uno] = (float)data[uno] / 255.0f;
	}

	HRESULT hres;
	hres = g_pEffect->SetValue( g_hmZNUser2, setuser, sizeof( float ) * 4 );
	if( hres != D3D_OK ){
		DbgOut( "E3DShaderConstZN2 : SetValue error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DRenderZN2( int outzscid, int outnscid, int outtscid, int outuscid, int inzscid, int hsid, int skipflag, int renderkind )
{
	if( outzscid < 0 ){
		DbgOut( "E3DRender : outzscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( outnscid < 0 ){
		DbgOut( "E3DRender : outnscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( outtscid < 0 ){
		DbgOut( "E3DRender : outtscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( outuscid < 0 ){
		DbgOut( "E3DRender : outuscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( inzscid < 0 ){
		DbgOut( "E3DRender : inzscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//skipflag //|1-->outz clear skip, |2-->outn clear skip, |4-->outt clear skip, 
//           |8-->outu clear skip, |16-->inz clear skip, |32-->transskip
//renderkind //|1-->outz, |2-->outn, |4-->outn camera mode(2があるときのみ有効)
//             |8-->outt, |16-->outu

//////////
	if( !(renderkind & 1) && !(renderkind & 2) && !(renderkind & 8) && !(renderkind & 16) ){
		return 0;
	}

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DRender : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret = 0;
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
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

	CalcMatView();
	ret = SetShaderConst( 0 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( !(skipflag & 32) ){
		int siglightflag = 0;
		ret = curhs->Transform( s_camerapos, pdev, s_matView, siglightflag, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}


	HRESULT hres;
	SWAPCHAINELEM* outzsc = 0;
	SWAPCHAINELEM* outnsc = 0;
	SWAPCHAINELEM* outtsc = 0;
	SWAPCHAINELEM* outusc = 0;
	SWAPCHAINELEM* inzsc = 0;

	inzsc = GetSwapChainElem( inzscid );
	if( !inzsc ){
		DbgOut( "E3DRender : inzscid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//skipflag //|1-->outz clear skip, |2-->outn clear skip, |4-->outt clear skip, 
//           |8-->outu clear skip, |16-->inz clear skip, |32-->transskip

	D3DVIEWPORT9 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Height = g_scsize.y;
	viewport.Width =  g_scsize.x;
	viewport.MaxZ = 1.0f;
	viewport.MinZ = 0.0f;

	hres = pdev->SetViewport(&viewport);
	if( hres != D3D_OK ){
		DbgOut( "RTBeginScene : dev SetViewport error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int doclearz = 0;

	int znflag;
	if( renderkind & 1 ){
		outzsc = GetSwapChainElem( outzscid );
		if( !outzsc ){
			DbgOut( "E3DRender : outzscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = pdev->SetRenderTarget( 0, outzsc->pBackBuffer );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetRenderTarget outz error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

		hres = pdev->SetDepthStencilSurface( inzsc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

	
		////////////////


		// Clear the viewport
	//	int skipflag;//|1-->outz_skip,  |2-->outn_skip, |4-->inz zskip, |8-->transskip, |16-->beginscene skip
		int clcolflag =  D3DCLEAR_TARGET;
		int clzflag = D3DCLEAR_ZBUFFER;
		if( skipflag & 1 ){
			clcolflag = 0;
		}
		if( skipflag & 16 ){
			clzflag = 0;
		}

		if( clcolflag || clzflag ){
			hres = pdev->Clear( 0L, NULL, clcolflag | clzflag, 
								 outzsc->clearcol,
								 1.0f, 0L );
		}
		_ASSERT( !hres );

		if( clzflag != 0 ){
			doclearz = 1;
		}
////////////////
		znflag = 101;//!!!!!!!!!!!!
		int withalpha;
		withalpha = 0;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		withalpha = 1;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( renderkind & 2 ){
		outnsc = GetSwapChainElem( outnscid );
		if( !outnsc ){
			DbgOut( "E3DRender : outnscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		hres = pdev->SetRenderTarget( 0, outnsc->pBackBuffer );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetRenderTarget outn error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
		hres = pdev->SetDepthStencilSurface( inzsc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

		int clcolflag =  D3DCLEAR_TARGET;
		int clzflag = D3DCLEAR_ZBUFFER;
		if( skipflag & 2 ){
			clcolflag = 0;
		}
		if( (skipflag & 16) || (doclearz == 1) ){
			clzflag = 0;
		}

		if( clcolflag || clzflag ){
			hres = pdev->Clear( 0L, NULL, clcolflag | clzflag, 
								 outnsc->clearcol,
								 1.0f, 0L );
		}
		_ASSERT( !hres );
		if( clzflag != 0 ){
			doclearz = 1;
		}


		if( renderkind & 4 ){
			znflag = 103;//!!!!!!!!!!!
		}else{
			znflag = 102;//!!!!!!!!!!!
		}
		int withalpha = 0;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		withalpha = 1;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( renderkind & 8 ){
		outtsc = GetSwapChainElem( outtscid );
		if( !outtsc ){
			DbgOut( "E3DRender : outtscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		hres = pdev->SetRenderTarget( 0, outtsc->pBackBuffer );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetRenderTarget outt error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
		hres = pdev->SetDepthStencilSurface( inzsc->pZSurf );
//		hres = pdev->SetDepthStencilSurface( outtsc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

		int clcolflag =  D3DCLEAR_TARGET;
		int clzflag = D3DCLEAR_ZBUFFER;
		if( skipflag & 4 ){
			clcolflag = 0;
		}
		if( (skipflag & 16) || (doclearz == 1) ){
			clzflag = 0;
		}

		if( clcolflag || clzflag ){
			hres = pdev->Clear( 0L, NULL, clcolflag | clzflag, 
								 outtsc->clearcol,
								 1.0f, 0L );
		}
		_ASSERT( !hres );
		if( clzflag != 0 ){
			doclearz = 1;
		}


		znflag = 104;
		int withalpha = 0;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		withalpha = 1;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( renderkind & 16 ){
		outusc = GetSwapChainElem( outuscid );
		if( !outusc ){
			DbgOut( "E3DRender : outuscid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		hres = pdev->SetRenderTarget( 0, outusc->pBackBuffer );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetRenderTarget outu error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}
		hres = pdev->SetDepthStencilSurface( inzsc->pZSurf );
		if( hres != D3D_OK ){
			DbgOut( "RTBeginScene : 3ddev SetDepthStencilSurface error %x, %x!!!\n", hres, D3DERR_INVALIDCALL );
			_ASSERT( 0 );
			return 1;
		}

		int clcolflag =  D3DCLEAR_TARGET;
		int clzflag = D3DCLEAR_ZBUFFER;
		if( skipflag & 8 ){
			clcolflag = 0;
		}
		if( skipflag & 16 ){
			clzflag = 0;
		}

		if( clcolflag || clzflag ){
			hres = pdev->Clear( 0L, NULL, clcolflag | clzflag, 
								 outusc->clearcol,
								 1.0f, 0L );
		}
		_ASSERT( !hres );

		znflag = 105;
		int withalpha = 0;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		withalpha = 1;
		ret = curhs->RenderZandN2( znflag, pdev, withalpha, s_matView, s_camerapos );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

EASY3D_API int E3DSetKinectUpVec( int hsid, int boneno, D3DXVECTOR3 upvec )
{

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetKinectUpVec : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetKinectUpVec( boneno, upvec );
	if( ret ){
		DbgOut( "E3DSetKinectUpVec : hs SetKinectUpVec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DValidHsid( int hsid, int* flagptr )
{
	int vflag = 0;

	if( (s_callinitflag == 0) || (byeflag == 1) ){
		vflag = 0;
	}else{
		CHandlerSet* hsptr;
		hsptr = GetHandlerSet( hsid );
		if( hsptr ){
			vflag = 1;
		}else{
			vflag = 0;
		}
	}
	*flagptr = vflag;

	return 0;
}


EASY3D_API int E3DKinectLoadLib( int* flagptr )
{
	int validflag = 0;
	*flagptr = validflag;

	if( !g_kinect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = g_kinect->LoadPlugin();
	if( ret ){
		DbgOut( "E3DKinectLoadLib : LoadPlugin error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( g_kinect->m_validflag == 1 ){
		if( g_kinect->m_pluginversion == 1.40f ){
			validflag = 1;
		}else{
			DbgOut( "E3DKinectLoadLib : plugin version error !!!\n" );
			_ASSERT( 0 );
			validflag = 0;
		}
	}else{
		validflag = 0;
	}

	*flagptr = validflag;

	return 0;
}
EASY3D_API int E3DKinectSetChara( int hsid, char* kstname, int motid, int capmode )
{
	int ret;
	if( !g_kinect ){
		_ASSERT( 0 );
		return 0;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	_ASSERT( pdev );

	g_kinect->m_pdev = pdev;
	g_kinect->m_hwnd = s_hwnd;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DKinectSetChara : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = g_kinect->SetChara( curhs, kstname, motid, capmode );
	if( ret ){
		DbgOut( "E3DKinectSetChara : SetChara error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DKinectStartCapture()
{
	if( !g_kinect ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = g_kinect->StartCapture();
	if( ret ){
		DbgOut( "E3DKinectStartCapture : StartCapture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DKinectEndCapture()
{
	if( !g_kinect ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = g_kinect->EndCapture();
	if( ret ){
		DbgOut( "E3DKinectEndCapture : EndCapture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
EASY3D_API int E3DKinectIsTracking( int* flagptr, int* capcntptr )
{
	*flagptr = 0;
	*capcntptr = 0;

	if( !g_kinect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = g_kinect->IsTracking( flagptr, capcntptr );
	if( ret ){
		DbgOut( "E3DKinectIsTracking : IsTracking error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DKinectGetSkelPos( D3DXVECTOR3* posptr, int* getflag )
{
	*getflag = 0;

	if( !g_kinect ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	ret = g_kinect->GetSkelPos( posptr, getflag );
	if( ret ){
		DbgOut( "E3DKinectGetSkelPos : GetSkelPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DKinectSetSkelPos( int kinectflag, int posmode, D3DXVECTOR3* srcpos, int* framenoptr )
{
	if( !g_kinect ){
		_ASSERT( 0 );
		return 0;
	}

	int ret;
	int frameno = 0;
	ret = g_kinect->SetSkelPos( kinectflag, posmode, srcpos, framenoptr );
	if( ret ){
		DbgOut( "E3DKinectSetSkelPos : SetSkelPos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DKinectRenderSilhouette( int withalpha, TPOS* tpos )
{
	if( !g_kinect ){
		_ASSERT( 0 );
		return 0;
	}

	float ftop, fleft, fbottom, fright;
	ftop = tpos->ftop;
	fleft = tpos->fleft;
	fbottom = tpos->fbottom;
	fright = tpos->fright;

	if( !g_pD3DApp || !g_pPostEffect ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	HRESULT hres;
	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( !pdev ){
		_ASSERT( 0 );
		return 1;
	}

	if( g_activateapp == 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	float screenw, screenh;
	screenw = (float)g_scsize.x;
	screenh = (float)g_scsize.y;


	if( !g_texbnk ){
		DbgOut( "E3DBlurTexture16Box : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	LPDIRECT3DTEXTURE9 ptex = g_kinect->m_ptex;
	if( g_pPostEffect != NULL )
	{

//		ret = SetRTRenderState( pdev, 0 );
//		_ASSERT( !ret );

		if( withalpha == 0 ){
			pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			g_renderstate[ D3DRS_ALPHABLENDENABLE ] = FALSE;
		}else{
			pdev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			g_renderstate[ D3DRS_ALPHABLENDENABLE ] = TRUE;
		}
		//-------------------------------------------------
		// シェーダの設定
		//-------------------------------------------------
		hres = g_pPostEffect->SetTechnique( g_hPostEffectTLV );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->SetValue( g_hpeWidth0, &screenw, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :width0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetValue( g_hpeHeight0, &screenh, sizeof( float ) );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :height0 SetValue error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		hres = g_pPostEffect->SetTexture( g_hpeTexture0, ptex );
		if( hres != D3D_OK ){
			DbgOut( "E3DBlurTexture16Box :texture0 SetTexture error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

//		hres = g_pPostEffect->CommitChanges();
//		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->Begin( NULL, 0 );
		_ASSERT( hres == D3D_OK );
		hres = g_pPostEffect->BeginPass( 0 );
		_ASSERT( hres == D3D_OK );

		float offsetu1 = 0.0f;
		float offsetv1 = 0.0f;

		TLVERTEX Vertex1[4] = {
			//   x    y     z      w     diffuse F4 u0 v0
			{fleft, ftop, 0.0001f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f,    0.0f + offsetu1, 0.0f + offsetv1},
			{fright, ftop, 0.0001f, 1.0f,  1.0f, 1.0f, 1.0f, 0.5f,    1.0f + offsetu1, 0.0f + offsetv1},
			{fright, fbottom, 0.0001f, 1.0f,  1.0f, 1.0f, 1.0f, 0.5f,    1.0f + offsetu1, 1.0f + offsetv1},
			{fleft, fbottom, 0.0001f, 1.0f,  1.0f, 1.0f, 1.0f, 0.5f,    0.0f + offsetu1, 1.0f + offsetv1}
		};

//		TLVERTEX Vertex1[4] = {
			//   x    y     z      w     diffuse F4 u0 v0
//			{0.1f, 0.1f, 0.0001f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f,    0.0f + offsetu1, 0.0f + offsetv1},
//			{+1.0f, 0.1f, 0.0001f, 1.0f,  1.0f, 1.0f, 1.0f, 0.5f,    1.0f + offsetu1, 0.0f + offsetv1},
//			{+1.0f, -1.0f, 0.0001f, 1.0f,  1.0f, 1.0f, 1.0f, 0.5f,    1.0f + offsetu1, 1.0f + offsetv1},
//			{0.1f, -1.0f, 0.0001f, 1.0f,  1.0f, 1.0f, 1.0f, 0.5f,    0.0f + offsetu1, 1.0f + offsetv1}
//		};


		hres = pdev->SetVertexDeclaration( s_tlvdecl );
		_ASSERT( hres == D3D_OK );

		hres = pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex1, sizeof( TLVERTEX ) );
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->EndPass();
		_ASSERT( hres == D3D_OK );

		hres = g_pPostEffect->End();
		_ASSERT( hres == D3D_OK );
	}else{
		_ASSERT( 0 );
	}

	return 0;
}

EASY3D_API int E3DGetInViewFlag( int hsid, int partno, int* flagptr )
{
	int ret;
	*flagptr = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetInViewFlag : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetInViewFlag( partno, flagptr );
	if( ret ){
		DbgOut( "E3DGetInViewFlag : hs GetInViewFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DWriteBmpFromSurface( char* nameptr, int scid )
{

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		return 0;
	}

	HRESULT hr;
	hr = D3DXSaveSurfaceToFile( nameptr, D3DXIFF_BMP, cursc->pBackBuffer, NULL, NULL );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


EASY3D_API int E3DGetMotionJumpFrame( int hsid, int motid, int* frameptr )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMotionJumpFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->GetMotionJumpFrame( motid, frameptr );
	if( ret ){
		DbgOut( "E3DGetMotionJumpFrame : hs GetMotionJumpFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DSetMotionJumpFrame( int hsid, int motid, int frameno )
{
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetMotionJumpFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;
	ret = curhs->SetMotionJumpFrame( motid, frameno );
	if( ret ){
		DbgOut( "E3DSetMotionJumpFrame : hs SetMotionJumpFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

EASY3D_API int E3DCopyTexAnimFrame( int hsid, int srcmotid, int srcframe, int dstmotid, int dstframe )
{
//E3DCopyTexAnimFrame hsid, srcmotid, srcframe, dstmotid, dstframe

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DCopyTexAnimFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->CopyTexAnimFrame( srcmotid, srcframe, dstmotid, dstframe );
	if( ret ){
		DbgOut( "E3DCopyTexAnimFrame : hs CopyTexAnimFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DDestroyTexAnimFrame( int hsid, int motid, int frameno )
{
//E3DDestroyTexAnimFrame hsid, motid, frameno
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyTexAnimFrame : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyTexAnimFrame( motid, frameno );
	if( ret ){
		DbgOut( "E3DDestroyTexAnimFrame : hs DestroyTexAnimFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DGetMaterialExtTextureNum( int hsid, int matno, int* numptr )
{
//E3DGetMaterialExtTextureNum hsid, matno, num
	*numptr = 0;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialExtTextureNum : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->GetMaterialExtTextureNum( matno, numptr );
	if( ret ){
		DbgOut( "E3DGetMaterialExtTextureNum : hs GetMaterialExtTextureNum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DGetMaterialExtTexture( int hsid, int matno, int extno, char* nameptr )
{
//E3DGetMaterialExtTexture hsid, matno, extno, name
	int ret;

	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DGetMaterialExtTexture : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->GetMaterialExtTexture( nameptr, matno, extno );
	if( ret ){
		DbgOut( "E3DGetMaterialExtTexture : hs GetMaterialExtTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DLoadMQOFileAsGroundFromBuf( char* filename, char* bufptr, int bufsize, int* hsidptr, int adjustuvflag, float mult )
{
//1
	int ret;
	int temphsid = -1;
	ret = LoadMQOFileAsGroundFromBuf_F( filename, bufptr, bufsize, adjustuvflag, mult, &temphsid );
	if( ret ){
		DbgOut( "E3DLoadMQOFileAsGroundFromBuf : E3DLoadMQOFileAsGroundFromBuf_F error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int LoadMQOFileAsGroundFromBuf_F( char* tempname, char* bufptr, int bufsize, int adjustuvflag, float mult, int* hsidptr )
{

	int ret = 0;


	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}
	

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( tempname, newhs );//!!!!!!


	newhs->m_TLmode = 2;//!!!!!!!
	newhs->m_groundflag = 1;//!!!!!!!!


	ret = newhs->LoadMQOFileAsGroundFromBuf( s_hwnd, bufptr, bufsize, adjustuvflag, mult );
	if( ret ){
		DbgOut( "easy3d : LoadMQOFileAsGroundFromBuf_F : LoadMQOFileAsGroundFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->SetShadowInterFlag( -1, SHADOWIN_NOT );
	_ASSERT( !ret );

	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3D : LoadMQOFileAsGroundFromBuf_F : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//Create 3D Object
	
	ret = Restore( newhs, -1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;

	return 0;
}

EASY3D_API int E3DChgMaterial1stTexFromBuf( int hsid, int matno, char* bufptr, int bufsize, int* texidptr )
{
	int ret;
	if( bufsize <= 0 ){
		DbgOut( "E3DChgMaterial1stTexFromBuf : bufsize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DChgMaterial1stTexFromBuf : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DChgMaterial1stTexFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChgMaterial1stTexFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texid = -1;
	ret = curhs->ChgMaterial1stTexFromBuf( pdev, matno, bufptr, bufsize, texidptr );
	if( ret ){
		DbgOut( "E3DChgMaterial1stTexFromBuf : hs ChgMaterial1stTexFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DChgMaterialExtTexFromBuf( int hsid, int matno, int extno, char* bufptr, int bufsize, int* texidptr )
{
	int ret;
	if( bufsize <= 0 ){
		DbgOut( "E3DChgMaterialExtTexFromBuf : bufsize error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DChgMaterialExtTexFromBuf : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DChgMaterialExtTexFromBuf : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChgMaterialExtTexFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int texid = -1;
	ret = curhs->ChgMaterialExtTexFromBuf( pdev, matno, extno, bufptr, bufsize, texidptr );
	if( ret ){
		DbgOut( "E3DChgMaterialExtTexFromBuf : hs ChgMaterialExtTexFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DChgMaterial1stTexFromID( int hsid, int matno, int texid )
{
	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DChgMaterial1stTexFromID : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DChgMaterial1stTexFromID : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChgMaterial1stTexFromID : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->ChgMaterial1stTexFromID( pdev, matno, texid );
	if( ret ){
		DbgOut( "E3DChgMaterial1stTexFromID : hs ChgMaterial1stTexFromID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DChgMaterialExtTexFromID( int hsid, int matno, int extno, int texid )
{
	int ret;

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DChgMaterialExtTexFromID : d3ddev NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !g_texbnk ){
		DbgOut( "E3DChgMaterialExtTexFromID : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DChgMaterialExtTexFromBuf : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curhs->ChgMaterialExtTexFromID( pdev, matno, extno, texid );
	if( ret ){
		DbgOut( "E3DChgMaterialExtTexFromID : hs ChgMaterialExtTexFromID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetCameraAnimTargetHsidAll( int animno, int srchsid )
{
	int ret;
	_ASSERT( m_cah );
	CCameraAnim* anim;
	anim = m_cah->GetCameraAnim( animno );
	if( !anim ){
		DbgOut( "E3DSetCameraAnimTargetHsidAll : animno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = anim->SetTargetHsidAll( srchsid );
	if( ret ){
		DbgOut( "E3DSetCameraAnimTargetHsidAll : anim SetTargetHsidAll error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetDispSwitchForce( int hsid, int swid, int state )
{
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DSetDispSwitchForce : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->SetDispSwitchForce( swid, state );
	if( ret ){
		DbgOut( "E3DSetDispSwitchForce : hs SetDispSwitchForce error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DSetWindowRgn( int deskscid, int figscid )
{
	SWAPCHAINELEM* desksc;
	desksc = GetSwapChainElem( deskscid );
	if( !desksc ){
		_ASSERT( 0 );
		return 1;
	}
	HWND deskhwnd = desksc->hwnd;


	SWAPCHAINELEM* figsc;
	figsc = GetSwapChainElem( figscid );
	if( !figsc ){
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DSURFACE9 figsurf = figsc->pBackBuffer;
	if( !figsurf ){
		DbgOut( "E3DSetWindowRgn : figsurface NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	HRESULT hres;
	LPD3DXBUFFER pbuf = 0;
	hres = D3DXSaveSurfaceToFileInMemory( &pbuf, D3DXIFF_BMP, figsurf, NULL, NULL );
	if( hres ){
		DbgOut( "E3DSetWindowRgn : D3DXSaveSurfaceToFileInMemory error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( !pbuf ){
		DbgOut( "E3DSetWindowRgn : pbuf NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	unsigned char* bufptr = (unsigned char*)pbuf->GetBufferPointer();
	int bufsize = pbuf->GetBufferSize();
	if( !bufptr || (bufsize <= 0) ){
		DbgOut( "E3DSetWindowRgn : buf param error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



//DbgOut( "E3DSetWindowRgn : bufsize %d\r\n", bufsize );

	int hleng1, hleng2;
	hleng1 = sizeof( BITMAPFILEHEADER );
	hleng2 = sizeof( BITMAPINFOHEADER );
	if( bufsize <= (hleng1 + hleng2) ){
		_ASSERT( 0 );
		return 1;
	}
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bih;
	MoveMemory( &hdr, bufptr, hleng1 );
	MoveMemory( &bih, (bufptr + hleng1), hleng2 );

	int width = bih.biWidth;
	int height = bih.biHeight;
	int bitcnt = bih.biBitCount;

//DbgOut( "E3DSetWindowRgn : width %d, height %d, bitcnt %d\r\n", width, height, bitcnt );

	ZeroMemory( rgnpoint, sizeof( int ) * RGNMAXNUM * 4 );
	ZeroMemory( rgnpoly, sizeof( int ) * RGNMAXNUM );

	int x;
	int y;
	int flg = 0;
	int count = 0;
	int pointcnt = 0;
	int polycnt = 0;

	E3DCOLOR4UC transrgb;
	transrgb = *( (E3DCOLOR4UC*)((unsigned char*)( bufptr + hleng1 + hleng2 )) );
	E3DCOLOR4UC currgb;

	for( y = height; y > 0; y-- ){
		for( x = 0; x < width; x++ ){
			currgb = *( (E3DCOLOR4UC*)( (unsigned char*)( bufptr + hleng1 + hleng2 + count * sizeof( E3DCOLOR4UC ) ) ) );
			
			//if( (width == x + 1) || ((currgb.r == transrgb.r) && (currgb.g == transrgb.g) && (currgb.b == transrgb.b)) ){
			if( (width == x + 1) || (currgb.b == 0) ){
				if( flg == 1 ){
					if( pointcnt >= RGNMAXNUM ){
						_ASSERT( 0 );
						return 1;
					}
					rgnpoint[pointcnt][0] = x;
					rgnpoint[pointcnt][1] = y - 1;
					rgnpoint[pointcnt][2] = x;
					rgnpoint[pointcnt][3] = y;
					pointcnt++;

					rgnpoly[polycnt] = 4;
					polycnt++;

					flg = 0;
				}
			}else{
				if( flg == 0 ){
					if( pointcnt >= RGNMAXNUM ){
						_ASSERT( 0 );
						return 1;
					}
					rgnpoint[pointcnt][0] = x;
					rgnpoint[pointcnt][1] = y;
					rgnpoint[pointcnt][2] = x;
					rgnpoint[pointcnt][3] = y - 1;
					pointcnt++;
					flg = 1;
				}
			}
			count++;
		}
	}
	if( (pointcnt > 0) && (polycnt > 0) ){
		HRGN hrgn;
		hrgn = CreatePolyPolygonRgn( (POINT*)rgnpoint, rgnpoly, polycnt, WINDING );
		if( !hrgn ){
			_ASSERT( 0 );
			return 1;
		}
		SetWindowRgn( deskhwnd, hrgn, 1 );
		DeleteObject( hrgn );
		UpdateWindow( deskhwnd );
	}

	DWORD colkey = 0;
	hres = D3DXLoadSurfaceFromFileInMemory( desksc->pBackBuffer, NULL, NULL,
		bufptr, bufsize, NULL,
		D3DX_FILTER_TRIANGLE,// | D3DX_FILTER_MIRROR,
		colkey, NULL );	
	if( hres != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}


	if( pbuf ){
		pbuf->Release();
	}

	return 0;
}

EASY3D_API int E3DDestroyDispPart( int hsid, int partno )
{
	CHandlerSet* curhs = 0;
	curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DDestroyDispPart : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = curhs->DestroyDispPart( partno );
	if( ret ){
		DbgOut( "E3DDestroyDispPart : hs DestroyDispPart error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

EASY3D_API int E3DPlayMovie( int scid, const char* filename )
{

	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	IGraphBuilder *pGraphBuilder;
	IMediaControl *pMediaControl;
	IMediaEvent *pMediaEvent;
	IVideoWindow *pVideoWindow;
	long eventCode;

	CoCreateInstance(CLSID_FilterGraph,
		NULL,
		CLSCTX_INPROC,
		IID_IGraphBuilder,
		(LPVOID *)&pGraphBuilder);

	pGraphBuilder->QueryInterface(IID_IMediaControl,
		(LPVOID *)&pMediaControl);

	pGraphBuilder->QueryInterface(IID_IMediaEvent,
		(LPVOID *)&pMediaEvent);

	WCHAR wname[MAX_PATH];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, filename, MAX_PATH, wname, MAX_PATH );
	pMediaControl->RenderFile( wname );

	pGraphBuilder->QueryInterface(IID_IVideoWindow,
		(LPVOID *)&pVideoWindow);
	pVideoWindow->put_Owner((OAHWND)cursc->hwnd);

	pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);

	RECT rect;
	GetClientRect( cursc->hwnd, &rect );
	pVideoWindow->SetWindowPosition(0, 0,
		rect.right - rect.left, rect.bottom - rect.top);

	pVideoWindow->SetWindowForeground(OATRUE);
	pVideoWindow->put_Visible(OATRUE);

	pMediaControl->Run();

	pMediaEvent->WaitForCompletion(-1, &eventCode);

	pMediaEvent->Release();
	pVideoWindow->Release();
	pMediaControl->Release();
	pGraphBuilder->Release();

	return 0;
}

EASY3D_API int E3DPndInit( char* key, int keyleng, int* pndidptr )
{
	int ret;
	ret = s_panda->Init( (unsigned char*)key, keyleng, pndidptr );
	if( ret || (*pndidptr <= 0) ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DPndDestroy( int pndid )
{
	int ret;
	ret = s_panda->Destroy( pndid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DPndEncrypt( int pndid, char* srcrootfolder, char* pndpath )
{
	int ret;
	ret = s_panda->Encrypt( pndid, (const char*)srcrootfolder, (const char*)pndpath );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DPndParse( int pndid, char* pndpath )
{
	int ret;
	ret = s_panda->ParseCipherFile( pndid, (const char*)pndpath );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DPndGetRootPath( int pndid, char* dstpath, int dstleng )
{
	int ret;
	ret = s_panda->GetRootPath( pndid, dstpath, dstleng );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DPndGetPropertyNum( int pndid, int* propnumptr )
{
	*propnumptr = s_panda->GetPropertyNum( pndid );

	return 0;
}
EASY3D_API int E3DPndGetProperty( int pndid, int propno, char* sourcepath, int pathleng, int* sourcesizeptr )
{
	int ret;
	PNDPROP prop;
	ZeroMemory( &prop, sizeof( PNDPROP ) );
	ret = s_panda->GetProperty( pndid, propno, &prop );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( sourcepath, pathleng, prop.path );
	*sourcesizeptr = prop.sourcesize;

	return 0;
}
EASY3D_API int E3DPndDecrypt( int pndid, char* sourcepath, char* dstbuf, int dstleng, int* getlengptr )
{
	int ret;
	ret = s_panda->Decrypt( pndid, sourcepath, (unsigned char*)dstbuf, dstleng, getlengptr );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
EASY3D_API int E3DPndLoadSigFile( int pndid, char* sourcepath, float mult, int* hsidptr )
{
	int ret;
	int sourcesize = 0;
	int propnum = s_panda->GetPropertyNum( pndid );
	if( propnum <= 0 ){
		_ASSERT( 0 );
		return 1;
	}
	int findindex = -1;
	int propno;
	for( propno = 0; propno < propnum; propno++ ){
		PNDPROP prop;
		ZeroMemory( &prop, sizeof( PNDPROP ) );
		ret = s_panda->GetProperty( pndid, propno, &prop );
		_ASSERT( !ret );
		int cmp = strcmp( prop.path, sourcepath );
		if( cmp == 0 ){
			findindex = propno;
		}
	}
	if( findindex < 0 ){
		_ASSERT( 0 );
		return 1;
	}


	int hsid = -1;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( sourcepath, newhs );//!!!!!!



	ret = newhs->LoadSigFileFromPnd( s_panda, pndid, findindex, mult );
	if( ret ){
		DbgOut( "easy3d : E3DPndLoadSigFile : hs LoadSigFileFromPnd error !!!\n" );
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

	ret = Restore( newhs, -1, s_panda, pndid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;


	return 0;
}
EASY3D_API int E3DPndLoadMoaFile( int pndid, int hsid, char* sourcepath, int fillupleng, float mvmult )
{
	int ret;
	CHandlerSet* curhs = GetHandlerSet( hsid );
	if( !curhs ){
		DbgOut( "E3DLoadMAFile : hsid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = curhs->LoadMAFile( sourcepath, fillupleng, s_hwnd, s_hMod, mvmult, s_panda, pndid );
	if( ret ){
		DbgOut( "E3DLoadMAFile : hs LoadMAFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


EASY3D_API int E3DPndLoadSigFileAsGround( int pndid, char* sourcepath, float mult, int* hsidptr )
{
	int ret;
	int sourcesize = 0;
	int propnum = s_panda->GetPropertyNum( pndid );
	if( propnum <= 0 ){
		_ASSERT( 0 );
		return 1;
	}
	int findindex = -1;
	int propno;
	for( propno = 0; propno < propnum; propno++ ){
		PNDPROP prop;
		ZeroMemory( &prop, sizeof( PNDPROP ) );
		ret = s_panda->GetProperty( pndid, propno, &prop );
		_ASSERT( !ret );
		int cmp = strcmp( prop.path, sourcepath );
		if( cmp == 0 ){
			findindex = propno;
		}
	}
	if( findindex < 0 ){
		_ASSERT( 0 );
		return 1;
	}


	int hsid = -1;

	CHandlerSet* newhs;
	newhs = new CHandlerSet();
	if( !newhs ){
		_ASSERT( 0 );
		return 1;
	}

	ret = newhs->CreateHandler();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	SetResDir( sourcepath, newhs );//!!!!!!


	newhs->m_groundflag = 1;//!!!!!!!!!!!!!!!!!!!

	ret = newhs->LoadSigFileFromPnd( s_panda, pndid, findindex, mult );
	if( ret ){
		DbgOut( "easy3d : E3DPndLoadSigFileAsGround : hs LoadSigFileFromPnd error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = ChainNewHandlerSet( newhs );
	if( ret ){
		DbgOut( "E3DPndLoadSigFileAsGround : ChainNewHandlerSet error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
		
//Create 3D Object

	ret = Restore( newhs, -1, s_panda, pndid );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	*hsidptr = newhs->serialno;

	return 0;
}

LRESULT CALLBACK MovieWndProc(
  HWND hwnd,      // handle to window
  UINT uMsg,      // message identifier
  WPARAM wParam,  // first message parameter
  LPARAM lParam   // second message parameter
){
	if( uMsg != WM_GRAPH_NOTIFY ){
		return CallWindowProc(
		  (WNDPROC)s_movieoldfunc,  // pointer to previous procedure
		  s_movienotifywnd,              // handle to window
		  uMsg,               // message
		  wParam,          // first message parameter
		  lParam           // second message parameter
		);		
	}else{
		long evCode;
		LONG param1, param2;

		// イベントを全て取得
		while (SUCCEEDED(s_pMediaEventEx->GetEvent(&evCode,
						&param1, &param2, 0))) {
			s_pMediaEventEx->FreeEventParams(evCode, param1, param2);
			switch (evCode) {
				case EC_COMPLETE:
					// 再生終了
					s_playingmovieflag = 0;
					break;
			}
		}
		return 0;	
	}
}


EASY3D_API int E3DPlayMovieEx( int scid, char* filename )
{
	if( g_activateapp == 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	E3DStopMovieEx();//!!!!!!!!!!!!!!!


	SWAPCHAINELEM* cursc;
	cursc = GetSwapChainElem( scid );
	if( !cursc ){
		DbgOut( "E3DBeginScene : scid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CoCreateInstance(CLSID_FilterGraph,
		NULL,
		CLSCTX_INPROC,
		IID_IGraphBuilder,
		(LPVOID *)&s_pGraphBuilder);

	s_pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *)&s_pMediaControl);
	s_pGraphBuilder->QueryInterface(IID_IMediaEventEx, (LPVOID *)&s_pMediaEventEx);
	s_pGraphBuilder->QueryInterface(IID_IBasicAudio, (LPVOID *)&s_pBasicAudio);

	s_pMediaEventEx->SetNotifyWindow((OAHWND)s_movienotifywnd, WM_GRAPH_NOTIFY, NULL);
	WCHAR wname[MAX_PATH];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, filename, MAX_PATH, wname, MAX_PATH );
	s_pMediaControl->RenderFile( wname );

	s_pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *)&s_pVideoWindow);
	s_pVideoWindow->put_Owner((OAHWND)cursc->hwnd);

	s_pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);

	RECT rect;
	GetClientRect( cursc->hwnd, &rect );
	s_pVideoWindow->SetWindowPosition(0, 0, rect.right - rect.left, rect.bottom - rect.top);
	s_pVideoWindow->SetWindowForeground(OATRUE);
	s_pVideoWindow->put_Visible(OATRUE);

	s_pMediaControl->Run();

	s_playingmovieflag = 1;

	return 0;

}
	
EASY3D_API int E3DIsPlayingMovieEx( int* playflagptr )
{
	*playflagptr = s_playingmovieflag;
	return 0;
}
EASY3D_API int E3DStopMovieEx()
{
	if( s_playingmovieflag == 1 ){
		if( s_pMediaControl ){
			s_pMediaControl->Stop();
		}
		s_playingmovieflag = 0;
	}
	if( s_pMediaEventEx ){
		s_pMediaEventEx->Release();
		s_pMediaEventEx = 0;
	}
	if( s_pVideoWindow ){
		s_pVideoWindow->Release();
		s_pVideoWindow = 0;
	}
	if( s_pMediaControl ){
		s_pMediaControl->Release();
		s_pMediaControl = 0;
	}
	if( s_pGraphBuilder ){
		s_pGraphBuilder->Release();
		s_pGraphBuilder = 0;
	}
	if( s_pBasicAudio ){
		s_pBasicAudio->Release();
		s_pBasicAudio = 0;
	}

	return 0;

}
EASY3D_API int E3DSetMovieVolumeEx( int srcvol )
{
	srcvol = min( 0, srcvol );
	srcvol = max( -10000, srcvol );

	if( s_pBasicAudio ){
		s_pBasicAudio->put_Volume( (long)srcvol );
	}
	return 0;
}
