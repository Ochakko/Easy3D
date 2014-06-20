// ver1_4dbg : TIME : 6 月 26 日 16 時 0 分 14 秒
//tlv 3 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 5 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 7 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 12 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 13 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 14 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 18 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 19 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 20 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 21 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 22 : param is not definded
	 // 1 1 1 1 0 0 0

#include	"stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <d3d.h>
#include <d3drmwin.h>
#ifndef	INITDDH
	#include "initdd.h"
#endif

#ifndef	TEXTUREH
	#include "texture.h"
#endif
#include "MakeVertex.h"

#include "MapData.h"
#ifndef CHARPARAMH
	#include "CharParam.h"
#endif
#ifndef CALCH
	#include "Calc.h"
#endif
#ifndef	RANDH
	#include	"Rand.h"
#endif
#ifndef	MYMOVEFLAGH
	#include	"MyMoveFlag.h"
#endif
#ifndef	MATRIXH
	#include	"matrix.h"
#endif
#ifndef	PLAYAREAH
	#include	"PlayArea.h"
#endif
#include	"kaeruparts.h"

#include	"d3dmacs.h" // execute buffer マクロ

#ifndef	COLORNAMEH
	#include	"colorname.h"
#endif

#ifndef	KAERUCOEFH
	#include	"kaerucoef.h"
#endif

//#ifndef	UVKAERUH
//	#include	"uvkaeru.h"
//#endif
//#define	u_long	unsigned long

#define	STRMAX	100

//EDITPOINT!!!	KTLVMAX
/*** DEFINE ***/
#define	KTLVMAX	24




// ユーザ定義したビジュアルのインスタンス毎が持つ情報の構造体。
typedef struct _MyVisual{
    LPDIRECT3DDEVICE dev;

	// mat は 一つ   tex を tu で切って使う
    LPDIRECT3DMATERIAL mat0;
	D3DMATERIALHANDLE hmat0;

	D3DTEXTUREHANDLE htex;
	D3DTexture texture;

}MyVisual;

static MyVisual*	gmyVisual;


// extern
extern void GetParam( FILE*, D3DTLVERTEX*, int, int, int ); // getparam.cpp
extern void DestLexBuf();// getparam.cpp  

extern int LoadMotion( CHARPARAM*, int, char* ); // loadmot.cpp
extern void FreeLDMBuff(); // loadmot.cpp

//extern float	divtz2[300];// -50 から 249 まで  matrix.cpp

extern CInitDD*		gInitDD;
extern CMakeVertex* MVertex;
extern CMapData*	MapData;
extern CCalc*		Calc;
extern CPlayArea*	gplayarea;
extern CRand*		Rand;
extern CHARPOS		kaerupos;
extern CHARPOS		campos;
extern float		kaeru_army;
extern WALKDATA		gwalkdata[MAXINDEXZ][MAXINDEXX];
extern LOCATION		gdirxz[9], gdiry[7], gcharmovedir[10];
extern LOCATION		lightdir;
extern CMatrix		gdir_cmatrix[9];

extern CMatrix			gworld_cmatrix, gview_cmatrix, gproj_cmatrix, gtemp_cmatrix;
extern CMatrix			gscreen_cmatrix, gproj_screen_cmatrix;

extern INDEX_L		gcharmoveindex[10];
extern CMatrix		viewcenrot; // backgcpp
extern int			gcamdist; // mainfrmcpp : kaeruindexz - camindexz
extern int			gcamheight; // camindexy - kaeruindexy


static char	tlvdatadir[STRMAX];
static char motiondir[STRMAX];


static LOCATION		kaeruloc, camloc;
static LOCATION		cam2char;
static int			curdirindex = 1;

D3DCLIPSTATUS		clipstatus = {D3DCLIPSTATUS_EXTENTS2, 0, 300.0f, 500.0f, 350.0f, 500.0f, 0.0f, 0.0f};
static D3DTLVERTEX	charrect[2]; // 左上と右下 ｚ は 真ん中
static D3DTLVERTEX	clipstatus2d[2]; // charrect に trans_cmatrix を かけたもの 

	// clip debug用
static D3DTLVERTEX	testcharrect[4];
static D3DTLVERTEX	testclipvert[4];

static BOOL			ischgvert = FALSE;

static int			kaeruswitch = 0; 
static int			next_kaeruswitch[2] = {1, 0};


static CHARPARAM	m_param[ MYFLAGMAX ];
static CHARPARAM	curcharparam = m_param[ IDLE0 ];

static int			m_curmove;
static CMatrix		m_matrix[ MYFLAGMAX ][ CTLMAX ];
static CMatrix		m_curmatrix[ CTLMAX ];
static CMatrix		m_curtransmatrix( Calc );
static CMatrix		m_clmatrix( Calc );

static CMatrix		bodydir_cmat[9]; // gdir_cmatrix の前後に cenBodyvert の 平行移動
static CMatrix		m_curdirmatrix( Calc );

static CMatrix		m_Nmatrix[9][ MYFLAGMAX ];// [dirindex][ moveno ]


static CMatrix		proj_screen_cmatrix( Calc ), temp_cmatrix( Calc ); // extern を コピーして持っておく
static CMatrix		l_viewcenrot( Calc ), trans_cmatrix( Calc );
static CMatrix		world_cmatrix( Calc ), tra_world_cmatrix( Calc ), view_cmatrix( Calc );
static CMatrix		tra_cmatrix( Calc );

static float	PLANEY0[8][3] = {
	{1.0f, 0.0f, 0.0f},{(float)PY, 0.0f, (float)PZ},
	{0.0f, 0.0f, 1.0f},{(float)-PY, 0.0f, (float)PZ},
	{-1.0f, 0.0f, 0.0f},{(float)-PY, 0.0f, (float)-PZ},
	{0.0f, 0.0f, -1.0f},{(float)PY, 0.0f, (float)-PZ}
};


// rgbdata
static RGBDATA		coldiffuse;
static float		colrate;

//EDITPOINT!!!	TLVNAME
/*** TLVERTEX ***/
static D3DTLVERTEX	tlvCtlAll[PNUMCTLALL][VNUMCTLALL];
static D3DTLVERTEX	tlvHead0[PNUMHEAD0][VNUMHEAD0];
static D3DTLVERTEX	tlvHead00[VNUMHEAD00];
static int	MemNumHead00[PNUMHEAD00];
static D3DTLVERTEX	tlvHead02[VNUMHEAD02];
static int	MemNumHead02[PNUMHEAD02];
static D3DTLVERTEX	tlvArmL0[PNUMARML0][VNUMARML0];
static D3DTLVERTEX	tlvArmL1[PNUMARML1][VNUMARML1];
static D3DTLVERTEX	tlvArmL2[VNUMARML2];
static int	MemNumArmL2[PNUMARML2];
static D3DTLVERTEX	tlvLegL0[PNUMLEGL0][VNUMLEGL0];
static D3DTLVERTEX	tlvLegL1[PNUMLEGL1][VNUMLEGL1];
static D3DTLVERTEX	tlvLegL2[VNUMLEGL2];
static int	MemNumLegL2[PNUMLEGL2];
static D3DTLVERTEX	tlvKtlvheadl[PNUMKTLVHEADL][VNUMKTLVHEADL];
static D3DTLVERTEX	tlvKtlveyel[VNUMKTLVEYEL];
static int	MemNumKtlveyel[PNUMKTLVEYEL];
static D3DTLVERTEX	tlvBody1[PNUMBODY1][VNUMBODY1];
static D3DTLVERTEX	tlvKtlvheadr[PNUMKTLVHEADR][VNUMKTLVHEADR];
static D3DTLVERTEX	tlvKtlveyer[VNUMKTLVEYER];
static int	MemNumKtlveyer[PNUMKTLVEYER];
static D3DTLVERTEX	tlvHead01[VNUMHEAD01];
static int	MemNumHead01[PNUMHEAD01];
static D3DTLVERTEX	tlvArmR0[PNUMARMR0][VNUMARMR0];
static D3DTLVERTEX	tlvArmR1[PNUMARMR1][VNUMARMR1];
static D3DTLVERTEX	tlvArmR2[VNUMARMR2];
static int	MemNumArmR2[PNUMARMR2];
static D3DTLVERTEX	tlvLegR0[PNUMLEGR0][VNUMLEGR0];
static D3DTLVERTEX	tlvLegR1[PNUMLEGR1][VNUMLEGR1];
static D3DTLVERTEX	tlvLegR2[VNUMLEGR2];
static int	MemNumLegR2[PNUMLEGR2];

/*** JOINT TLV ***/
static int	JindexArmL1;
static int	JindexLegL1;
static int	JindexArmR1;
static int	JindexLegR1;


//EDITPOINT!!!	OBJCENTER
/*** OBJ 中心座標 ***/
static D3DTLVERTEX	cenCtlAll;
static D3DTLVERTEX	cenHead0;
static D3DTLVERTEX	cenHead00;
static D3DTLVERTEX	cenHead02;
static D3DTLVERTEX	cenArmL0;
static D3DTLVERTEX	cenArmL1;
static D3DTLVERTEX	cenArmL2;
static D3DTLVERTEX	cenLegL0;
static D3DTLVERTEX	cenLegL1;
static D3DTLVERTEX	cenLegL2;
static D3DTLVERTEX	cenKtlvheadl;
static D3DTLVERTEX	cenKtlveyel;
static D3DTLVERTEX	cenBody1;
static D3DTLVERTEX	cenKtlvheadr;
static D3DTLVERTEX	cenKtlveyer;
static D3DTLVERTEX	cenHead01;
static D3DTLVERTEX	cenArmR0;
static D3DTLVERTEX	cenArmR1;
static D3DTLVERTEX	cenArmR2;
static D3DTLVERTEX	cenLegR0;
static D3DTLVERTEX	cenLegR1;
static D3DTLVERTEX	cenLegR2;



//***************
//Execute Buffer
//***************
static D3DTLVERTEX	zerotlv;

static LPD3DTLVERTEX	pkaerutlv[ KTLVMAX + 1 ]; //それぞれのparts tlvの 先頭のpointer
static int	ikaerutlv[ KTLVMAX + 1 ];	// それぞれのparts tlvの 先頭のindex
static int	ikaerutri[ KTLVMAX + 1 ];

//EDITPOINT!!!	EBNUM

/*** VERTNUM FOR EXECUTE BUFFER ***/
static int num_tlvctlall = PNUMCTLALL * VNUMCTLALL;
static int num_trictlall = ( PNUMCTLALL - 1 ) * VNUMCTLALL * 2;

static int num_tlvhead0 = PNUMHEAD0 * VNUMHEAD0;
static int num_trihead0 = ( PNUMHEAD0 - 1 ) * VNUMHEAD0 * 2;

static int num_tlvhead00 = VNUMHEAD00;
static int num_trihead00 = 0; // InitTlv で初期化 

static int num_tlvhead02 = VNUMHEAD02;
static int num_trihead02 = 0; // InitTlv で初期化 

static int num_tlvarml0 = PNUMARML0 * VNUMARML0;
static int num_triarml0 = ( PNUMARML0 - 1 ) * VNUMARML0 * 2;

static int num_tlvarml1 = PNUMARML1 * VNUMARML1;
static int num_triarml1 = PNUMARML1 * VNUMARML1 * 2; // JOINT TRIANGLE を 含む

static int num_tlvarml2 = VNUMARML2;
static int num_triarml2 = 0; // InitTlv で初期化 

static int num_tlvlegl0 = PNUMLEGL0 * VNUMLEGL0;
static int num_trilegl0 = ( PNUMLEGL0 - 1 ) * VNUMLEGL0 * 2;

static int num_tlvlegl1 = PNUMLEGL1 * VNUMLEGL1;
static int num_trilegl1 = PNUMLEGL1 * VNUMLEGL1 * 2; // JOINT TRIANGLE を 含む

static int num_tlvlegl2 = VNUMLEGL2;
static int num_trilegl2 = 0; // InitTlv で初期化 

static int num_tlvktlvheadl = PNUMKTLVHEADL * VNUMKTLVHEADL;
static int num_triktlvheadl = ( PNUMKTLVHEADL - 1 ) * VNUMKTLVHEADL * 2;

static int num_tlvktlveyel = VNUMKTLVEYEL;
static int num_triktlveyel = 0; // InitTlv で初期化 

static int num_tlvbody1 = PNUMBODY1 * VNUMBODY1;
static int num_tribody1 = ( PNUMBODY1 - 1 ) * ( VNUMBODY1 - 1 ) * 2;

static int num_tlvktlvheadr = PNUMKTLVHEADR * VNUMKTLVHEADR;
static int num_triktlvheadr = ( PNUMKTLVHEADR - 1 ) * VNUMKTLVHEADR * 2;

static int num_tlvktlveyer = VNUMKTLVEYER;
static int num_triktlveyer = 0; // InitTlv で初期化 

static int num_tlvhead01 = VNUMHEAD01;
static int num_trihead01 = 0; // InitTlv で初期化 

static int num_tlvarmr0 = PNUMARMR0 * VNUMARMR0;
static int num_triarmr0 = ( PNUMARMR0 - 1 ) * VNUMARMR0 * 2;

static int num_tlvarmr1 = PNUMARMR1 * VNUMARMR1;
static int num_triarmr1 = PNUMARMR1 * VNUMARMR1 * 2; // JOINT TRIANGLE を 含む

static int num_tlvarmr2 = VNUMARMR2;
static int num_triarmr2 = 0; // InitTlv で初期化 

static int num_tlvlegr0 = PNUMLEGR0 * VNUMLEGR0;
static int num_trilegr0 = ( PNUMLEGR0 - 1 ) * VNUMLEGR0 * 2;

static int num_tlvlegr1 = PNUMLEGR1 * VNUMLEGR1;
static int num_trilegr1 = PNUMLEGR1 * VNUMLEGR1 * 2; // JOINT TRIANGLE を 含む

static int num_tlvlegr2 = VNUMLEGR2;
static int num_trilegr2 = 0; // InitTlv で初期化 

/** total num **/
static int num_tlvkaeru1 = 0; // !!! InitKaeruVertex で 初期化
static int num_trikaeru1 = 0;



static int	num_kaeruinst1 = 3;// exit の分も忘れずに
static int	num_kaerustate1 = 0;
static int	num_kaeruprov1 = 1;

static int	num_ini_kinst = 3;// exit の分も忘れずに
static int	num_ini_kstate = 4;

static int	sizeInstruction = sizeof( D3DINSTRUCTION );
static int	sizeState = sizeof( D3DSTATE );
static int	sizeProcessv = sizeof( D3DPROCESSVERTICES );
static int	sizeTlv = sizeof( D3DTLVERTEX );
static int	sizeTriangle = sizeof( D3DTRIANGLE );


static LPDIRECT3DEXECUTEBUFFER	lpKaeruEB1 = 0;
static D3DEXECUTEBUFFERDESC	kaeruExeBuffDesc1;
static D3DEXECUTEDATA	kaeruExecuteData1;

	// renderstate, lightstate 用(基本的に データ更新無し)
	// 頂点と いっしょにすると メモリ位置を 複数の場所で管理することになり バグのもと 
		// なので 最初から 逃げ。。。
static LPDIRECT3DEXECUTEBUFFER	lpIKaeruEB = 0;
static D3DEXECUTEBUFFERDESC	ikaeruExeBuffDesc;
static D3DEXECUTEDATA	ikaeruExecuteData;


//********
// device
//********
static LPDIRECT3DDEVICE lpD3DDev = NULL;
static LPDIRECT3DVIEWPORT lpD3DView = NULL;

//****************
// func prototype 
//****************

//EDITPOINT!!!	FUNC INIT TLV
/*** INIT TLVERTEX ***/
static void	InitTlvCtlAll();
static void	InitTlvHead0();
static void	InitTlvHead00();
static void	InitTlvHead02();
static void	InitTlvArmL0();
static void	InitTlvArmL1();
static void	InitTlvArmL2();
static void	InitTlvLegL0();
static void	InitTlvLegL1();
static void	InitTlvLegL2();
static void	InitTlvKtlvheadl();
static void	InitTlvKtlveyel();
static void	InitTlvBody1();
static void	InitTlvKtlvheadr();
static void	InitTlvKtlveyer();
static void	InitTlvHead01();
static void	InitTlvArmR0();
static void	InitTlvArmR1();
static void	InitTlvArmR2();
static void	InitTlvLegR0();
static void	InitTlvLegR1();
static void	InitTlvLegR2();



static void InitKaeruVertex();
static void	SetKaeruClipStatus();

static void InitIniKaeruEB();// executebuffer
static void FillIniKaeruEB();
static void InitKaeruEB();
static void FillKaeruEB();
static void InitPkaeruTlv();
static void	SetEBInst(); // triflag などの instruction の set 

	//svert の yz 平面に関して 対称な 点を retvert に格納
	// ccw で clip するため  inverse round する
static void MirrorTlv2D( D3DTLVERTEX* retvert, D3DTLVERTEX* svert, int planenum, int pointnum );
static void MirrorTlv1D( D3DTLVERTEX* retvert, int* retmemnum,  
						D3DTLVERTEX* svert, int* smemnum, 
						int fannum, int pointnum );


void	KaeruMove( int sckind );
void	CamPosSet( int sckind );

void	SetViewcenrot( CMatrix& ); // global ： backgcpp から 呼ばれる
void	MultKaeru();// mainfrmcpp から
void	KaeruMultMatrix();

void	InitKaeruVisual();
void	PreKaeruRender();
void	RenderKaeru();
void	SetUpKaeruRes();
void	DestroyKaeru();


static void	KaeruPosSet();

	// tiranglelist 型 内積が 正のものを indexから はじく
		// normal 配列数は ３角形の数 ！！！！ 頂点の数ではない ！！！
static void	ClipInversFace( LOCATION* snormal, WORD* sindex, int trinum, 
						   WORD* retindex, DWORD* retdrawnum );

static void	ClipInversBody();
static void InitMyVisual();

// 通常の move 用
//EDITPOINT!!!	FUNC FOR MOVE
static void	InitMatrixCtlAll();
static void	MultMatrixCtlAll();


static void	InitMatrixHead0();
static void	MultMatrixHead0();


static void	InitMatrixHead00();
static void	MultMatrixHead00();


static void	InitMatrixHead02();
static void	MultMatrixHead02();


static void	InitMatrixArmL0();
static void	MultMatrixArmL0();


static void	InitMatrixArmL1();
static void	MultMatrixArmL1();


static void	InitMatrixArmL2();
static void	MultMatrixArmL2();


static void	InitMatrixLegL0();
static void	MultMatrixLegL0();


static void	InitMatrixLegL1();
static void	MultMatrixLegL1();


static void	InitMatrixLegL2();
static void	MultMatrixLegL2();


static void	MultMatrixKtlvheadl();


static void	MultMatrixKtlveyel();


static void	MultMatrixBody1();


static void	MultMatrixKtlvheadr();


static void	MultMatrixKtlveyer();


static void	InitMatrixHead01();
static void	MultMatrixHead01();


static void	InitMatrixArmR0();
static void	MultMatrixArmR0();


static void	InitMatrixArmR1();
static void	MultMatrixArmR1();


static void	InitMatrixArmR2();
static void	MultMatrixArmR2();


static void	InitMatrixLegR0();
static void	MultMatrixLegR0();


static void	InitMatrixLegR1();
static void	MultMatrixLegR1();


static void	InitMatrixLegR2();
static void	MultMatrixLegR2();



static void	InitKaeruMatrix();
static void InitBodyDirMat();

void KaeruMultMatrix();

static void SetCharParam();



void RenderKaeru()
{

	if( ischgvert )
		return;

	//isrendering = TRUE;

	lpD3DDev->Execute( lpIKaeruEB, lpD3DView, D3DEXECUTE_UNCLIPPED );
	lpD3DDev->Execute( lpKaeruEB1, lpD3DView, D3DEXECUTE_UNCLIPPED );
	//isrendering = FALSE;
	

}

void PreKaeruRender()
{


	if( gmyVisual->dev != D3Z.lpD3DDevice ){
		SetUpKaeruRes();
		
		gmyVisual->dev = D3Z.lpD3DDevice;

		RELEASE( lpD3DView );
		RELEASE( lpD3DDev );

		lpD3DDev = D3Z.lpD3DDevice;
		lpD3DView = D3Z.oldviewport;
		
		D3Z.lpD3DDevice->AddRef();// backgcpp には 入れてないが ここにないと InitDD::EndApp で Release したときに
		D3Z.oldviewport->AddRef();// バグるのは なぜ？？？  参照数が よれている ？？？？
								

// cmatrix

		proj_screen_cmatrix.SetD3DMatrix( Calc, gproj_screen_cmatrix );
		temp_cmatrix.SetD3DMatrix( Calc, gtemp_cmatrix );
		world_cmatrix.SetD3DMatrix( Calc, gworld_cmatrix );
		view_cmatrix.SetD3DMatrix( Calc, gview_cmatrix );

		InitIniKaeruEB();
		InitKaeruEB();

		//KaeruMove();
		//KaeruMultMatrix();
	}
}

void DestroyKaeru()
{
	RELEASE( lpKaeruEB1 );
	RELEASE( lpIKaeruEB );

	if( gmyVisual->htex ){
		gmyVisual->texture.Release();
		gmyVisual->htex = 0;
	}

	RELEASE(gmyVisual->mat0);

	TRACE( "UVKAERUCPP : DESTROY : free Myvis\n" );

    memset(gmyVisual, '0', sizeof(MyVisual));
	free( (MyVisual*)gmyVisual );

	TRACE( "UVKAERUCPP : DESTROY : release lpD3dView\n" );
	RELEASE( lpD3DView );
	TRACE( "UVKAERUCPP : DESTROY : release lpD3DDev\n" );
	RELEASE( lpD3DDev );

}


void SetUpKaeruRes()
{

    D3DMATERIAL mat0;

	OutputDebugString("setup vukaeru resources\n");

	gmyVisual->texture.Load(D3Z.lpD3DDevice,"kaerudata\\matkaeru.bmp");	
	gmyVisual->htex = gmyVisual->texture.GetHandle();

	//gmyVisual->texture1.Load( D3Z.lpD3DDevice, "rmfire3.bmp" );
	//gmyVisual->htex1 = gmyVisual->texture1.GetHandle();
	//gmyVisual->htex1 = 0;

    if (D3Z.lpD3D->CreateMaterial( &gmyVisual->mat0, NULL) != D3D_OK) {
		OutputDebugString("uvis: Failed to CreateMaterial\n");
    }

	//color 0 R 0.849020 G 0.849020 B 0.849020
	//color 1 R 0.313726 G 0.749020 B 0.313726

    memset(&mat0, 0, sizeof(D3DMATERIAL));
    mat0.dwSize = sizeof(D3DMATERIAL);
    mat0.diffuse.r = (D3DVALUE)1.0;
    mat0.diffuse.g = (D3DVALUE)1.0;
    mat0.diffuse.b = (D3DVALUE)1.0;
    mat0.diffuse.a = (D3DVALUE)1.0;
    mat0.ambient.r = (D3DVALUE)0.3;
    mat0.ambient.g = (D3DVALUE)0.3;
    mat0.ambient.b = (D3DVALUE)0.3;
    mat0.ambient.a = (D3DVALUE)1.0;
    mat0.specular.r = (D3DVALUE)0.0;
    mat0.specular.g = (D3DVALUE)0.0;
    mat0.specular.b = (D3DVALUE)0.0;
    mat0.power = (float)0.0;
    mat0.hTexture = gmyVisual->htex;
    mat0.dwRampSize = 16;

	gmyVisual->mat0->SetMaterial( &mat0 );
    gmyVisual->mat0->GetHandle( D3Z.lpD3DDevice, &gmyVisual->hmat0 );

}

void InitMyVisual()
{
	gmyVisual = (MyVisual*)malloc(sizeof(MyVisual));
	if (!gmyVisual){
		TRACE( "UVKAERUCPP : InitMyVisual : gmyVisual alloc err\n" );
		exit(0);
	}
	memset(gmyVisual, 0, sizeof(MyVisual));

    
	gmyVisual->dev = 0;

	gmyVisual->hmat0 = 0;
	gmyVisual->htex = 0;
}

//EDITPOINT!!!	InitKaeruVisual
/*** INIT KAERU VISUAL ***/
void	InitKaeruVisual()
{
	InitMyVisual();

	memset( tlvdatadir, 0, STRMAX );
	strcpy( tlvdatadir, "c:\\disp0\\tlvdata\\" );
	memset( motiondir, 0, STRMAX );
	strcpy( motiondir, "c:\\disp0\\motiondata\\" );
	coldiffuse.r = 200;
	coldiffuse.g = 200;
	coldiffuse.b = 200;

	colrate = (200 - 50) * 0.5f;

	kaeruloc.x = kaerupos.loc.x;
	kaeruloc.y = kaerupos.loc.y;
	kaeruloc.z = kaerupos.loc.z;

	camloc.x = campos.loc.x;
	camloc.y = campos.loc.y;
	camloc.z = campos.loc.z;

	cam2char.x = kaerupos.loc.x - campos.loc.x;
	cam2char.y = kaerupos.loc.y - campos.loc.y;
	cam2char.z = kaerupos.loc.z - campos.loc.z;

	Calc->VecNormalizeXYZ( &cam2char );

	float	height, width;
	height = 2.8f; width = 1.9f;

	charrect[0].sx = cenCtlAll.sx - width;
	charrect[0].sy = cenCtlAll.sy + height;
	charrect[0].sz = cenCtlAll.sz;
	charrect[1].sx = cenCtlAll.sx + width;
	charrect[1].sy = cenCtlAll.sy - height;
	charrect[1].sz = cenCtlAll.sz;

	testcharrect[1].sx = cenCtlAll.sx - width;
	testcharrect[1].sy = cenCtlAll.sy + height;
	testcharrect[1].sz = cenCtlAll.sz;

	testcharrect[0].sx = cenCtlAll.sx - width;
	testcharrect[0].sy = cenCtlAll.sy - height;
	testcharrect[0].sz = cenCtlAll.sz;

	testcharrect[3].sx = cenCtlAll.sx + width;
	testcharrect[3].sy = cenCtlAll.sy + height;
	testcharrect[3].sz = cenCtlAll.sz;

	testcharrect[2].sx = cenCtlAll.sx + width;
	testcharrect[2].sy = cenCtlAll.sy - height;
	testcharrect[2].sz = cenCtlAll.sz;

	SetCharParam();
	InitKaeruVertex();
	InitKaeruMatrix();
}



void MultKaeru()
{
	HRESULT	hres;

	ischgvert = TRUE;

	KaeruMultMatrix();

			//頂点数などが 変化しないときは 呼ばなくても大丈夫
	//hres = lpKaeruEB1->SetExecuteData( &kaeruExecuteData1 );
	//if( FAILED( hres ) ){
	//	TRACE( "UVKAERUCPP : UpdateKaeru : SetExecuteData err \n" );
	//	exit(0);
	//}
	//TRACE( "UVKAERUCP : InitGrandEB : num_tlvkaeru1 %d num_trikaeru1 %d\n",
	//	num_tlvkaeru1, num_trikaeru1 );

	ischgvert = FALSE;

}

void InitIniKaeruEB()
{
	HRESULT	hres;
	DWORD	dwVertexSize;
	DWORD	dwInstructionSize;
	DWORD	dwExecuteBufferSize;
	D3DEXECUTEBUFFERDESC	d3dExecuteBufferDesc;


	dwVertexSize = 0;
	dwInstructionSize = ( num_ini_kinst * sizeof( D3DINSTRUCTION ) ) 
		+ ( num_ini_kstate * sizeof( D3DSTATE ) );

	dwExecuteBufferSize = dwVertexSize + dwInstructionSize;
	ZeroMemory( &d3dExecuteBufferDesc, sizeof(d3dExecuteBufferDesc) );
	d3dExecuteBufferDesc.dwSize = sizeof( d3dExecuteBufferDesc );
	d3dExecuteBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	d3dExecuteBufferDesc.dwBufferSize = dwExecuteBufferSize;

	hres = D3Z.lpD3DDevice->CreateExecuteBuffer( &d3dExecuteBufferDesc, &lpIKaeruEB, NULL );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitIniKaeruEB : CreateEB err\n" );
		exit(0);
	}

	FillIniKaeruEB();//mode, triangle set

	ZeroMemory( &ikaeruExecuteData, sizeof( ikaeruExecuteData ) );
	ikaeruExecuteData.dwSize = sizeof( ikaeruExecuteData );
	ikaeruExecuteData.dwVertexCount = 0;
	ikaeruExecuteData.dwInstructionOffset = 0;//
	ikaeruExecuteData.dwInstructionLength = dwInstructionSize;

	hres = lpIKaeruEB->SetExecuteData( &ikaeruExecuteData );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitIniKaeruEB : SetExecuteData err \n" );
		exit(0);
	}

}
void FillIniKaeruEB()
{
	HRESULT	hres;


	void*	curptr;


	ZeroMemory( &ikaeruExeBuffDesc, sizeof( ikaeruExeBuffDesc ) );
	ikaeruExeBuffDesc.dwSize = sizeof( ikaeruExeBuffDesc );

	hres = lpIKaeruEB->Lock( &ikaeruExeBuffDesc );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : FillIniGrandEB : EBLock err\n" );
		exit(0);
	}

	curptr = (void*)ikaeruExeBuffDesc.lpData;


	// light, render state
	PUTD3DINSTRUCTION( D3DOP_STATELIGHT, sizeof(D3DSTATE), 1, curptr );
	STATE_DATA( D3DLIGHTSTATE_MATERIAL, gmyVisual->hmat0, curptr );

	PUTD3DINSTRUCTION( D3DOP_STATERENDER, sizeof(D3DSTATE), 3, curptr );
	STATE_DATA( D3DRENDERSTATE_TEXTUREHANDLE, gmyVisual->htex, curptr );
	//STATE_DATA( D3DRENDERSTATE_ZWRITEENABLE, FALSE, curptr );
	STATE_DATA( D3DRENDERSTATE_ZWRITEENABLE, TRUE, curptr );
	//STATE_DATA( D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS, curptr );
	STATE_DATA( D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL, curptr );

	// exit
	OP_EXIT( curptr );
	lpIKaeruEB->Unlock();

}

void InitKaeruEB()
{
	HRESULT	hres;
	DWORD	dwVertexSize;
	DWORD	dwInstructionSize;
	DWORD	dwExecuteBufferSize;
	DWORD	vertsize;
	DWORD	instleng;
	D3DEXECUTEBUFFERDESC	d3dExecuteBufferDesc;


	dwVertexSize = ( num_tlvkaeru1 * sizeof( D3DTLVERTEX) );//
	dwInstructionSize = ( num_kaeruinst1 * sizeof( D3DINSTRUCTION ) 
		+ ( num_kaerustate1 * sizeof( D3DSTATE ) )
		+ ( num_kaeruprov1 * sizeof( D3DPROCESSVERTICES) )
		+ ( num_trikaeru1 * sizeof( D3DTRIANGLE ) ) );
	dwExecuteBufferSize = dwVertexSize + dwInstructionSize;
	ZeroMemory( &d3dExecuteBufferDesc, sizeof(d3dExecuteBufferDesc) );
	d3dExecuteBufferDesc.dwSize = sizeof( d3dExecuteBufferDesc );
	d3dExecuteBufferDesc.dwFlags = D3DDEB_CAPS | D3DDEB_BUFSIZE;
	d3dExecuteBufferDesc.dwCaps = D3DDEBCAPS_VIDEOMEMORY;
	d3dExecuteBufferDesc.dwBufferSize = dwExecuteBufferSize;

	hres = D3Z.lpD3DDevice->CreateExecuteBuffer( &d3dExecuteBufferDesc, &lpKaeruEB1, NULL );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitKaeruEB : CreateEB1 err\n" );
		exit(0);
	}
		
	FillKaeruEB();
///
	ZeroMemory( &kaeruExecuteData1, sizeof( kaeruExecuteData1 ) );
	kaeruExecuteData1.dwSize = sizeof( kaeruExecuteData1 );
	kaeruExecuteData1.dwVertexCount = num_tlvkaeru1;//!!!!!!!!!
	vertsize  = ( num_tlvkaeru1 * sizeof( D3DTLVERTEX) );
	kaeruExecuteData1.dwInstructionOffset = vertsize;//!!!!!!!
	instleng = ( num_kaeruinst1 * sizeof( D3DINSTRUCTION ) 
		+ ( num_kaerustate1 * sizeof( D3DSTATE ) )
		+ ( num_kaeruprov1 * sizeof( D3DPROCESSVERTICES) )
		+ ( num_trikaeru1 * sizeof( D3DTRIANGLE ) ) );
	kaeruExecuteData1.dwInstructionLength = instleng;//!!!!!!!
	

////
	hres = lpKaeruEB1->SetExecuteData( &kaeruExecuteData1 );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitGrandEB : SetExecuteData err \n" );
		exit(0);
	}
	//TRACE( "UVKAERUCP : InitGrandEB : num_tlvkaeru1 %d num_trikaeru1 %d\n",
	//	num_tlvkaeru1, num_trikaeru1 );
	
}

void FillKaeruEB()
{
	HRESULT	hres;


	ZeroMemory( &kaeruExeBuffDesc1, sizeof( kaeruExeBuffDesc1 ) );
	kaeruExeBuffDesc1.dwSize = sizeof( kaeruExeBuffDesc1 );

	hres = lpKaeruEB1->Lock( &kaeruExeBuffDesc1 );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : FillGrandEB : EBLock err\n" );
		exit(0);
	}
	
	
	//GrandMove();
	InitPkaeruTlv();

	KaeruMove( SCROLL_XZ );
	KaeruMultMatrix();

	SetEBInst();

	lpKaeruEB1->Unlock();

}

//EDITPOINT!!!	InitPkaeruTlv
/*** TLV HEAD POSITION FOR EB ***/
void	InitPkaeruTlv()
{

	int	partsno;
	LPD3DTLVERTEX	lptlv;

	for( partsno = 0; partsno < KTLVMAX; partsno++ ){
		ikaerutlv[partsno] = 0;
		ikaerutri[partsno] = 0;
		pkaerutlv[partsno] = NULL;
	}

	lptlv = (LPD3DTLVERTEX)kaeruExeBuffDesc1.lpData;
	ikaerutlv[ 0 ] = 0;
	ikaerutri[ 0 ] = 0;
	pkaerutlv[ 0 ] = lptlv;

	ikaerutlv[ 1 ] = ikaerutlv[ 0 ] + num_tlvctlall; // 直前までの総数  ！！！
	ikaerutri[ 1 ] = ikaerutri[ 0 ] + num_trictlall;
	pkaerutlv[ 1 ] = pkaerutlv[ 0 ] + ikaerutlv[ 1 ];

	ikaerutlv[ 6 ] = ikaerutlv[ 1 ] + num_tlvhead0; // 直前までの総数  ！！！
	ikaerutri[ 6 ] = ikaerutri[ 1 ] + num_trihead0;
	pkaerutlv[ 6 ] = pkaerutlv[ 0 ] + ikaerutlv[ 6 ];

	ikaerutlv[ 8 ] = ikaerutlv[ 6 ] + num_tlvhead00; // 直前までの総数  ！！！
	ikaerutri[ 8 ] = ikaerutri[ 6 ] + num_trihead00;
	pkaerutlv[ 8 ] = pkaerutlv[ 0 ] + ikaerutlv[ 8 ];

	ikaerutlv[ 9 ] = ikaerutlv[ 8 ] + num_tlvhead02; // 直前までの総数  ！！！
	ikaerutri[ 9 ] = ikaerutri[ 8 ] + num_trihead02;
	pkaerutlv[ 9 ] = pkaerutlv[ 0 ] + ikaerutlv[ 9 ];

	ikaerutlv[ 10 ] = ikaerutlv[ 9 ] + num_tlvarml0; // 直前までの総数  ！！！
	ikaerutri[ 10 ] = ikaerutri[ 9 ] + num_triarml0;
	pkaerutlv[ 10 ] = pkaerutlv[ 0 ] + ikaerutlv[ 10 ];

	ikaerutlv[ 11 ] = ikaerutlv[ 10 ] + num_tlvarml1; // 直前までの総数  ！！！
	ikaerutri[ 11 ] = ikaerutri[ 10 ] + num_triarml1;
	pkaerutlv[ 11 ] = pkaerutlv[ 0 ] + ikaerutlv[ 11 ];

	ikaerutlv[ 15 ] = ikaerutlv[ 11 ] + num_tlvarml2; // 直前までの総数  ！！！
	ikaerutri[ 15 ] = ikaerutri[ 11 ] + num_triarml2;
	pkaerutlv[ 15 ] = pkaerutlv[ 0 ] + ikaerutlv[ 15 ];

	ikaerutlv[ 16 ] = ikaerutlv[ 15 ] + num_tlvlegl0; // 直前までの総数  ！！！
	ikaerutri[ 16 ] = ikaerutri[ 15 ] + num_trilegl0;
	pkaerutlv[ 16 ] = pkaerutlv[ 0 ] + ikaerutlv[ 16 ];

	ikaerutlv[ 17 ] = ikaerutlv[ 16 ] + num_tlvlegl1; // 直前までの総数  ！！！
	ikaerutri[ 17 ] = ikaerutri[ 16 ] + num_trilegl1;
	pkaerutlv[ 17 ] = pkaerutlv[ 0 ] + ikaerutlv[ 17 ];

	ikaerutlv[ 2 ] = ikaerutlv[ 17 ] + num_tlvlegl2; // 直前までの総数  ！！！
	ikaerutri[ 2 ] = ikaerutri[ 17 ] + num_trilegl2;
	pkaerutlv[ 2 ] = pkaerutlv[ 0 ] + ikaerutlv[ 2 ];

	ikaerutlv[ 4 ] = ikaerutlv[ 2 ] + num_tlvktlvheadl; // 直前までの総数  ！！！
	ikaerutri[ 4 ] = ikaerutri[ 2 ] + num_triktlvheadl;
	pkaerutlv[ 4 ] = pkaerutlv[ 0 ] + ikaerutlv[ 4 ];

	ikaerutlv[ 23 ] = ikaerutlv[ 4 ] + num_tlvktlveyel; // 直前までの総数  ！！！
	ikaerutri[ 23 ] = ikaerutri[ 4 ] + num_triktlveyel;
	pkaerutlv[ 23 ] = pkaerutlv[ 0 ] + ikaerutlv[ 23 ];

	ikaerutlv[ 3 ] = ikaerutlv[ 23 ] + num_tlvbody1; // 直前までの総数  ！！！
	ikaerutri[ 3 ] = ikaerutri[ 23 ] + num_tribody1;
	pkaerutlv[ 3 ] = pkaerutlv[ 0 ] + ikaerutlv[ 3 ];

	ikaerutlv[ 5 ] = ikaerutlv[ 3 ] + num_tlvktlvheadr; // 直前までの総数  ！！！
	ikaerutri[ 5 ] = ikaerutri[ 3 ] + num_triktlvheadr;
	pkaerutlv[ 5 ] = pkaerutlv[ 0 ] + ikaerutlv[ 5 ];

	ikaerutlv[ 7 ] = ikaerutlv[ 5 ] + num_tlvktlveyer; // 直前までの総数  ！！！
	ikaerutri[ 7 ] = ikaerutri[ 5 ] + num_triktlveyer;
	pkaerutlv[ 7 ] = pkaerutlv[ 0 ] + ikaerutlv[ 7 ];

	ikaerutlv[ 12 ] = ikaerutlv[ 7 ] + num_tlvhead01; // 直前までの総数  ！！！
	ikaerutri[ 12 ] = ikaerutri[ 7 ] + num_trihead01;
	pkaerutlv[ 12 ] = pkaerutlv[ 0 ] + ikaerutlv[ 12 ];

	ikaerutlv[ 13 ] = ikaerutlv[ 12 ] + num_tlvarmr0; // 直前までの総数  ！！！
	ikaerutri[ 13 ] = ikaerutri[ 12 ] + num_triarmr0;
	pkaerutlv[ 13 ] = pkaerutlv[ 0 ] + ikaerutlv[ 13 ];

	ikaerutlv[ 14 ] = ikaerutlv[ 13 ] + num_tlvarmr1; // 直前までの総数  ！！！
	ikaerutri[ 14 ] = ikaerutri[ 13 ] + num_triarmr1;
	pkaerutlv[ 14 ] = pkaerutlv[ 0 ] + ikaerutlv[ 14 ];

	ikaerutlv[ 18 ] = ikaerutlv[ 14 ] + num_tlvarmr2; // 直前までの総数  ！！！
	ikaerutri[ 18 ] = ikaerutri[ 14 ] + num_triarmr2;
	pkaerutlv[ 18 ] = pkaerutlv[ 0 ] + ikaerutlv[ 18 ];

	ikaerutlv[ 19 ] = ikaerutlv[ 18 ] + num_tlvlegr0; // 直前までの総数  ！！！
	ikaerutri[ 19 ] = ikaerutri[ 18 ] + num_trilegr0;
	pkaerutlv[ 19 ] = pkaerutlv[ 0 ] + ikaerutlv[ 19 ];

	ikaerutlv[ 20 ] = ikaerutlv[ 19 ] + num_tlvlegr1; // 直前までの総数  ！！！
	ikaerutri[ 20 ] = ikaerutri[ 19 ] + num_trilegr1;
	pkaerutlv[ 20 ] = pkaerutlv[ 0 ] + ikaerutlv[ 20 ];

	/*** JOINT TLV INDEX ***/

	JindexArmL1 = ikaerutlv[ 9 ] + ( PNUMARML0 - 1 ) * VNUMARML1;
	JindexLegL1 = ikaerutlv[ 15 ] + ( PNUMLEGL0 - 1 ) * VNUMLEGL1;
	JindexArmR1 = ikaerutlv[ 12 ] + ( PNUMARMR0 - 1 ) * VNUMARMR1;
	JindexLegR1 = ikaerutlv[ 18 ] + ( PNUMLEGR0 - 1 ) * VNUMLEGR1;
}



//EDITPOINT!!!	SetEBInst
/*** SET KAERU EB INSTRUCTION ***/
void	SetEBInst()
{

	LPD3DTLVERTEX	lptlv;
	LPD3DINSTRUCTION	lpinst;

	LPD3DINSTRUCTION	lpInstruction;
	LPD3DTRIANGLE		lpTriangle;
	LPD3DPROCESSVERTICES	lpProcessV;

	//lptlv = (LPD3DTLVERTEX)kaeruExeBuffDesc1.lpData;
	lptlv = pkaerutlv[0];

	lpinst = (LPD3DINSTRUCTION)( lptlv + num_tlvkaeru1 );

	lpInstruction = lpinst;
	lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
	lpInstruction->bSize = sizeof( D3DPROCESSVERTICES );
	lpInstruction->wCount = 1U;
	lpInstruction++;

	lpProcessV = (LPD3DPROCESSVERTICES)lpInstruction;
	lpProcessV->dwFlags = D3DPROCESSVERTICES_COPY;
	lpProcessV->wStart = 0U;
	lpProcessV->wDest = 0U;
	lpProcessV->dwCount = num_tlvkaeru1;//
	lpProcessV->dwReserved = 0;
	lpProcessV++;

	// triangle
	lpInstruction = (LPD3DINSTRUCTION)lpProcessV;
	lpInstruction->bOpcode = D3DOP_TRIANGLE;
	lpInstruction->bSize = sizeof( D3DTRIANGLE );
	lpInstruction->wCount = num_trikaeru1;//!!!!!!!!!!!!!!!!!
	lpInstruction++;

	lpTriangle = (LPD3DTRIANGLE)lpInstruction;

	m_curmatrix[ CTLALL ].SetTridataYoko( lpTriangle + ikaerutri[0], PNUMCTLALL, VNUMCTLALL, ikaerutlv[ 0 ] );
	m_curmatrix[ CTLALL ].SetUV2D( pkaerutlv[ 0 ], PNUMCTLALL, VNUMCTLALL, KAERUGREEN );

	m_curmatrix[ HEAD0 ].SetTridataYoko( lpTriangle + ikaerutri[1], PNUMHEAD0, VNUMHEAD0, ikaerutlv[ 1 ] );
	m_curmatrix[ HEAD0 ].SetUV2D( pkaerutlv[ 1 ], PNUMHEAD0, VNUMHEAD0, KAERUGREEN );

	m_curmatrix[ HEAD00 ].SetTridataFan( lpTriangle + ikaerutri[6], PNUMHEAD00, VNUMHEAD00, ikaerutlv[ 6 ], MemNumHead00 );
	m_curmatrix[ HEAD00 ].SetUV1D( pkaerutlv[ 6 ], MemNumHead00, PNUMHEAD00, KAERUBLACK );

	m_curmatrix[ HEAD0 ].SetTridataFan( lpTriangle + ikaerutri[8], PNUMHEAD02, VNUMHEAD02, ikaerutlv[ 8 ], MemNumHead02 );
	m_curmatrix[ HEAD0 ].SetUV1D( pkaerutlv[ 8 ], MemNumHead02, PNUMHEAD02, KAERURED );

	m_curmatrix[ ARML0 ].SetTridataYoko( lpTriangle + ikaerutri[9], PNUMARML0, VNUMARML0, ikaerutlv[ 9 ] );
	m_curmatrix[ ARML0 ].SetUV2D( pkaerutlv[ 9 ], PNUMARML0, VNUMARML0, KAERUGREEN );

	m_curmatrix[ ARML1 ].SetTriYokoJ( lpTriangle + ikaerutri[10], JindexArmL1, PNUMARML1, VNUMARML1, ikaerutlv[ 10 ] );
	m_curmatrix[ ARML1 ].SetUV2D( pkaerutlv[ 10 ], PNUMARML1, VNUMARML1, KAERUGREEN );

	m_curmatrix[ ARML2 ].SetTridataBFan( lpTriangle + ikaerutri[11], PNUMARML2, VNUMARML2, ikaerutlv[ 11 ], MemNumArmL2 );
	m_curmatrix[ ARML2 ].SetUV1D( pkaerutlv[ 11 ], MemNumArmL2, PNUMARML2, KAERUGREEN );

	m_curmatrix[ LEGL0 ].SetTridataYoko( lpTriangle + ikaerutri[15], PNUMLEGL0, VNUMLEGL0, ikaerutlv[ 15 ] );
	m_curmatrix[ LEGL0 ].SetUV2D( pkaerutlv[ 15 ], PNUMLEGL0, VNUMLEGL0, KAERUGREEN );

	m_curmatrix[ LEGL1 ].SetTriYokoJ( lpTriangle + ikaerutri[16], JindexLegL1, PNUMLEGL1, VNUMLEGL1, ikaerutlv[ 16 ] );
	m_curmatrix[ LEGL1 ].SetUV2D( pkaerutlv[ 16 ], PNUMLEGL1, VNUMLEGL1, KAERUGREEN );

	m_curmatrix[ LEGL2 ].SetTridataBFan( lpTriangle + ikaerutri[17], PNUMLEGL2, VNUMLEGL2, ikaerutlv[ 17 ], MemNumLegL2 );
	m_curmatrix[ LEGL2 ].SetUV1D( pkaerutlv[ 17 ], MemNumLegL2, PNUMLEGL2, KAERUGREEN );

	m_curmatrix[ HEAD0 ].SetTridataYoko( lpTriangle + ikaerutri[2], PNUMKTLVHEADL, VNUMKTLVHEADL, ikaerutlv[ 2 ] );
	m_curmatrix[ HEAD0 ].SetUV2D( pkaerutlv[ 2 ], PNUMKTLVHEADL, VNUMKTLVHEADL, KAERUGREEN );

	m_curmatrix[ HEAD0 ].SetTridataFan( lpTriangle + ikaerutri[4], PNUMKTLVEYEL, VNUMKTLVEYEL, ikaerutlv[ 4 ], MemNumKtlveyel );
	m_curmatrix[ HEAD0 ].SetUV1D( pkaerutlv[ 4 ], MemNumKtlveyel, PNUMKTLVEYEL, KAERUWHITE );

	m_curmatrix[ CTLALL ].SetTridataYLine( lpTriangle + ikaerutri[23], PNUMBODY1, VNUMBODY1, ikaerutlv[ 23 ] );
	m_curmatrix[ CTLALL ].SetUV2D( pkaerutlv[ 23 ], PNUMBODY1, VNUMBODY1, KAERUWHITE );

	m_curmatrix[ HEAD0 ].SetTridataYoko( lpTriangle + ikaerutri[3], PNUMKTLVHEADR, VNUMKTLVHEADR, ikaerutlv[ 3 ] );
	m_curmatrix[ HEAD0 ].SetUV2D( pkaerutlv[ 3 ], PNUMKTLVHEADR, VNUMKTLVHEADR, KAERUGREEN );

	m_curmatrix[ HEAD0 ].SetTridataFan( lpTriangle + ikaerutri[5], PNUMKTLVEYER, VNUMKTLVEYER, ikaerutlv[ 5 ], MemNumKtlveyer );
	m_curmatrix[ HEAD0 ].SetUV1D( pkaerutlv[ 5 ], MemNumKtlveyer, PNUMKTLVEYER, KAERUWHITE );

	m_curmatrix[ HEAD01 ].SetTridataFan( lpTriangle + ikaerutri[7], PNUMHEAD01, VNUMHEAD01, ikaerutlv[ 7 ], MemNumHead01 );
	m_curmatrix[ HEAD01 ].SetUV1D( pkaerutlv[ 7 ], MemNumHead01, PNUMHEAD01, KAERUBLACK );

	m_curmatrix[ ARMR0 ].SetTridataYoko( lpTriangle + ikaerutri[12], PNUMARMR0, VNUMARMR0, ikaerutlv[ 12 ] );
	m_curmatrix[ ARMR0 ].SetUV2D( pkaerutlv[ 12 ], PNUMARMR0, VNUMARMR0, KAERUGREEN );

	m_curmatrix[ ARMR1 ].SetTriYokoJ( lpTriangle + ikaerutri[13], JindexArmR1, PNUMARMR1, VNUMARMR1, ikaerutlv[ 13 ] );
	m_curmatrix[ ARMR1 ].SetUV2D( pkaerutlv[ 13 ], PNUMARMR1, VNUMARMR1, KAERUGREEN );

	m_curmatrix[ ARMR2 ].SetTridataBFan( lpTriangle + ikaerutri[14], PNUMARMR2, VNUMARMR2, ikaerutlv[ 14 ], MemNumArmR2 );
	m_curmatrix[ ARMR2 ].SetUV1D( pkaerutlv[ 14 ], MemNumArmR2, PNUMARMR2, KAERUGREEN );

	m_curmatrix[ LEGR0 ].SetTridataYoko( lpTriangle + ikaerutri[18], PNUMLEGR0, VNUMLEGR0, ikaerutlv[ 18 ] );
	m_curmatrix[ LEGR0 ].SetUV2D( pkaerutlv[ 18 ], PNUMLEGR0, VNUMLEGR0, KAERUGREEN );

	m_curmatrix[ LEGR1 ].SetTriYokoJ( lpTriangle + ikaerutri[19], JindexLegR1, PNUMLEGR1, VNUMLEGR1, ikaerutlv[ 19 ] );
	m_curmatrix[ LEGR1 ].SetUV2D( pkaerutlv[ 19 ], PNUMLEGR1, VNUMLEGR1, KAERUGREEN );

	m_curmatrix[ LEGR2 ].SetTridataBFan( lpTriangle + ikaerutri[20], PNUMLEGR2, VNUMLEGR2, ikaerutlv[ 20 ], MemNumLegR2 );
	m_curmatrix[ LEGR2 ].SetUV1D( pkaerutlv[ 20 ], MemNumLegR2, PNUMLEGR2, KAERUGREEN );


	lpInstruction = (LPD3DINSTRUCTION)(lpTriangle + num_trikaeru1);
	lpInstruction->bOpcode = D3DOP_EXIT;
	lpInstruction->bSize = 0;
	lpInstruction->wCount = 0U;
}



//EDITPOINT!!!	InitKaeruVertex
/*** KAERU INIT VERTEX ***/
void	InitKaeruVertex()
{
	InitTlvCtlAll();
	InitTlvHead0();
	InitTlvHead00();
	InitTlvHead02();
	InitTlvArmL0();
	InitTlvArmL1();
	InitTlvArmL2();
	InitTlvLegL0();
	InitTlvLegL1();
	InitTlvLegL2();
	InitTlvKtlvheadl();
	InitTlvKtlveyel();
	InitTlvBody1();
	InitTlvKtlvheadr();
	InitTlvKtlveyer();
	InitTlvHead01();
	InitTlvArmR0();
	InitTlvArmR1();
	InitTlvArmR2();
	InitTlvLegR0();
	InitTlvLegR1();
	InitTlvLegR2();
	DestLexBuf();


/** total num for eb **/
	num_tlvkaeru1 = num_tlvctlall + num_tlvhead0 + num_tlvhead00 + num_tlvhead02 + num_tlvarml0 + num_tlvarml1 + num_tlvarml2 + num_tlvlegl0 + num_tlvlegl1 + num_tlvlegl2 + num_tlvktlvheadl + num_tlvktlveyel + num_tlvbody1 + num_tlvktlvheadr + num_tlvktlveyer + num_tlvhead01 + num_tlvarmr0 + num_tlvarmr1 + num_tlvarmr2 + num_tlvlegr0 + num_tlvlegr1 + num_tlvlegr2;

	num_trikaeru1 = num_trictlall + num_trihead0 + num_trihead00 + num_trihead02 + num_triarml0 + num_triarml1 + num_triarml2 + num_trilegl0 + num_trilegl1 + num_trilegl2 + num_triktlvheadl + num_triktlveyel + num_tribody1 + num_triktlvheadr + num_triktlveyer + num_trihead01 + num_triarmr0 + num_triarmr1 + num_triarmr2 + num_trilegr0 + num_trilegr1 + num_trilegr2;

}



//EDITPOINT!!!	INIT PARTS TLV
/*** INIT VERTEX ***/
void	InitTlvCtlAll()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaerubody0.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvCtlAll : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvCtlAll[0], PNUMCTLALL, VNUMCTLALL, 0 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	Calc->CenterLoc( &cennormal, tlvCtlAll[ (int)( PNUMCTLALL / 2 ) ], VNUMCTLALL );

	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenCtlAll.sx = cennormal.x; cenCtlAll.sy = cennormal.y + shifty; cenCtlAll.sz = cennormal.z;
	for( planeno = 0; planeno < PNUMCTLALL; planeno++ ){
		for( pointno = 0; pointno < VNUMCTLALL; pointno++ ){
			tlvCtlAll[planeno][pointno].sx *= scalex;
			tlvCtlAll[planeno][pointno].sy *= scaley;
			tlvCtlAll[planeno][pointno].sy += shifty;
			tlvCtlAll[planeno][pointno].sz *= scalez;
			tlvCtlAll[planeno][pointno].sz += shiftz;
			tlvCtlAll[planeno][pointno].rhw = 0.95f;

			normal.x = tlvCtlAll[planeno][pointno].sx - cenCtlAll.sx;
			normal.y = tlvCtlAll[planeno][pointno].sy - cenCtlAll.sy;
			normal.z = tlvCtlAll[planeno][pointno].sz - cenCtlAll.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvCtlAll[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvCtlAll[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvHead0()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruhead0.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvHead0 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvHead0[0], PNUMHEAD0, VNUMHEAD0, 0 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	Calc->CenterLoc( &cennormal, tlvHead0[ (int)( PNUMHEAD0 / 2 ) ], VNUMHEAD0 );

	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenHead0.sx = cennormal.x; cenHead0.sy = cennormal.y + shifty; cenHead0.sz = cennormal.z;
	for( planeno = 0; planeno < PNUMHEAD0; planeno++ ){
		for( pointno = 0; pointno < VNUMHEAD0; pointno++ ){
			tlvHead0[planeno][pointno].sx *= scalex;
			tlvHead0[planeno][pointno].sy *= scaley;
			tlvHead0[planeno][pointno].sy += shifty;
			tlvHead0[planeno][pointno].sz *= scalez;
			tlvHead0[planeno][pointno].sz += shiftz;
			tlvHead0[planeno][pointno].rhw = 0.95f;

			normal.x = tlvHead0[planeno][pointno].sx - cenHead0.sx;
			normal.y = tlvHead0[planeno][pointno].sy - cenHead0.sy;
			normal.z = tlvHead0[planeno][pointno].sz - cenHead0.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvHead0[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvHead0[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvHead00()
{
	int	fanno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaerueye1.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvHead00 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvHead00, 1, VNUMHEAD00, 0 );
 // fan は 1次元配列
	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	cennormal.x =  0.063323f;
	cennormal.y =  -0.244556f;
	cennormal.z =  -0.010954f;
	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenHead00.sx = cennormal.x; cenHead00.sy = cennormal.y + shifty; cenHead00.sz = cennormal.z;
	for( pointno = 0; pointno < VNUMHEAD00; pointno++ ){
			tlvHead00[pointno].sx *= scalex;
			tlvHead00[pointno].sy *= scaley;
			tlvHead00[pointno].sy += shifty;
			tlvHead00[pointno].sz *= scalez;
			tlvHead00[pointno].sz += shiftz;
			tlvHead00[pointno].rhw = 0.95f;

			normal.x = tlvHead00[pointno].sx - cenHead00.sx;
			normal.y = tlvHead00[pointno].sy - cenHead00.sy;
			normal.z = tlvHead00[pointno].sz - cenHead00.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvHead00[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvHead00[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	//fan mem num
	MemNumHead00[0] = 11;

	num_trihead00 = 0;

	for( fanno = 0; fanno < PNUMHEAD00; fanno++ ){
		num_trihead00 += (MemNumHead00[fanno] - 2);
	}
}

void	InitTlvHead02()
{
	int	fanno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaerumouth.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvHead02 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvHead02, 1, VNUMHEAD02, 0 );
 // fan は 1次元配列
	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.05f;
	cennormal.x =  0.000000f;
	cennormal.y =  -0.157812f;
	cennormal.z =  -0.009370f;
	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenHead02.sx = cennormal.x; cenHead02.sy = cennormal.y + shifty; cenHead02.sz = cennormal.z;
	for( pointno = 0; pointno < VNUMHEAD02; pointno++ ){
			tlvHead02[pointno].sx *= scalex;
			tlvHead02[pointno].sy *= scaley;
			tlvHead02[pointno].sy += shifty;
			tlvHead02[pointno].sz *= scalez;
			tlvHead02[pointno].sz += shiftz;
			tlvHead02[pointno].rhw = 0.95f;

			normal.x = tlvHead02[pointno].sx - cenHead02.sx;
			normal.y = tlvHead02[pointno].sy - cenHead02.sy;
			normal.z = tlvHead02[pointno].sz - cenHead02.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvHead02[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvHead02[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	//fan mem num
	MemNumHead02[0] = 8;

	num_trihead02 = 0;

	for( fanno = 0; fanno < PNUMHEAD02; fanno++ ){
		num_trihead02 += (MemNumHead02[fanno] - 2);
	}
}

void	InitTlvArmL0()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruarm.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvArmL0 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvArmL0[0], PNUMARML0, VNUMARML0, 0 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.3f; shiftz = 0.0f;
	Calc->CenterLoc( &cennormal, tlvArmL0[ 0 ], VNUMARML0 );

	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenArmL0.sx = cennormal.x; cenArmL0.sy = cennormal.y + shifty; cenArmL0.sz = cennormal.z;
	for( planeno = 0; planeno < PNUMARML0; planeno++ ){
		for( pointno = 0; pointno < VNUMARML0; pointno++ ){
			tlvArmL0[planeno][pointno].sx *= scalex;
			tlvArmL0[planeno][pointno].sy *= scaley;
			tlvArmL0[planeno][pointno].sy += shifty;
			tlvArmL0[planeno][pointno].sz *= scalez;
			tlvArmL0[planeno][pointno].sz += shiftz;
			tlvArmL0[planeno][pointno].rhw = 0.95f;

			normal.x = tlvArmL0[planeno][pointno].sx - cenArmL0.sx;
			normal.y = tlvArmL0[planeno][pointno].sy - cenArmL0.sy;
			normal.z = tlvArmL0[planeno][pointno].sz - cenArmL0.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvArmL0[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvArmL0[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvArmL1()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruarm.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvArmL1 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvArmL1[0], PNUMARML1, VNUMARML1, 3 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.3f; shiftz = 0.0f;
	Calc->CenterLoc( &cennormal, tlvArmL1[ 0 ], VNUMARML1 );

	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenArmL1.sx = cennormal.x; cenArmL1.sy = cennormal.y + shifty; cenArmL1.sz = cennormal.z;
	for( planeno = 0; planeno < PNUMARML1; planeno++ ){
		for( pointno = 0; pointno < VNUMARML1; pointno++ ){
			tlvArmL1[planeno][pointno].sx *= scalex;
			tlvArmL1[planeno][pointno].sy *= scaley;
			tlvArmL1[planeno][pointno].sy += shifty;
			tlvArmL1[planeno][pointno].sz *= scalez;
			tlvArmL1[planeno][pointno].sz += shiftz;
			tlvArmL1[planeno][pointno].rhw = 0.95f;

			normal.x = tlvArmL1[planeno][pointno].sx - cenArmL1.sx;
			normal.y = tlvArmL1[planeno][pointno].sy - cenArmL1.sy;
			normal.z = tlvArmL1[planeno][pointno].sz - cenArmL1.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvArmL1[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvArmL1[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvArmL2()
{
	int	fanno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruhand0.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvArmL2 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvArmL2, 1, VNUMARML2, 0 );
 // fan は 1次元配列
	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.3f; shiftz = 0.0f;
	cennormal.x =  0.169781f;
	cennormal.y =  0.081826f;
	cennormal.z =  0.026685f;
	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenArmL2.sx = cennormal.x; cenArmL2.sy = cennormal.y + shifty; cenArmL2.sz = cennormal.z;
	for( pointno = 0; pointno < VNUMARML2; pointno++ ){
			tlvArmL2[pointno].sx *= scalex;
			tlvArmL2[pointno].sy *= scaley;
			tlvArmL2[pointno].sy += shifty;
			tlvArmL2[pointno].sz *= scalez;
			tlvArmL2[pointno].sz += shiftz;
			tlvArmL2[pointno].rhw = 0.95f;

			normal.x = tlvArmL2[pointno].sx - cenArmL2.sx;
			normal.y = tlvArmL2[pointno].sy - cenArmL2.sy;
			normal.z = tlvArmL2[pointno].sz - cenArmL2.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvArmL2[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvArmL2[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	//fan mem num
	MemNumArmL2[0] = 9;
	MemNumArmL2[1] = 8;
	MemNumArmL2[2] = 8;
	MemNumArmL2[3] = 8;
	MemNumArmL2[4] = 7;
	MemNumArmL2[5] = 7;

	num_triarml2 = 0;

	for( fanno = 0; fanno < PNUMARML2; fanno++ ){
		num_triarml2 += (MemNumArmL2[fanno] - 2);
	}

	num_triarml2 *= 2; // 裏面
}

void	InitTlvLegL0()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruleg0.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvLegL0 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvLegL0[0], PNUMLEGL0, VNUMLEGL0, 0 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	Calc->CenterLoc( &cennormal, tlvLegL0[ 0 ], VNUMLEGL0 );

	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenLegL0.sx = cennormal.x; cenLegL0.sy = cennormal.y + shifty; cenLegL0.sz = cennormal.z;
	for( planeno = 0; planeno < PNUMLEGL0; planeno++ ){
		for( pointno = 0; pointno < VNUMLEGL0; pointno++ ){
			tlvLegL0[planeno][pointno].sx *= scalex;
			tlvLegL0[planeno][pointno].sy *= scaley;
			tlvLegL0[planeno][pointno].sy += shifty;
			tlvLegL0[planeno][pointno].sz *= scalez;
			tlvLegL0[planeno][pointno].sz += shiftz;
			tlvLegL0[planeno][pointno].rhw = 0.95f;

			normal.x = tlvLegL0[planeno][pointno].sx - cenLegL0.sx;
			normal.y = tlvLegL0[planeno][pointno].sy - cenLegL0.sy;
			normal.z = tlvLegL0[planeno][pointno].sz - cenLegL0.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvLegL0[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvLegL0[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvLegL1()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruleg0.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvLegL1 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvLegL1[0], PNUMLEGL1, VNUMLEGL1, 3 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	Calc->CenterLoc( &cennormal, tlvLegL1[ 0 ], VNUMLEGL1 );

	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenLegL1.sx = cennormal.x; cenLegL1.sy = cennormal.y + shifty; cenLegL1.sz = cennormal.z;
	for( planeno = 0; planeno < PNUMLEGL1; planeno++ ){
		for( pointno = 0; pointno < VNUMLEGL1; pointno++ ){
			tlvLegL1[planeno][pointno].sx *= scalex;
			tlvLegL1[planeno][pointno].sy *= scaley;
			tlvLegL1[planeno][pointno].sy += shifty;
			tlvLegL1[planeno][pointno].sz *= scalez;
			tlvLegL1[planeno][pointno].sz += shiftz;
			tlvLegL1[planeno][pointno].rhw = 0.95f;

			normal.x = tlvLegL1[planeno][pointno].sx - cenLegL1.sx;
			normal.y = tlvLegL1[planeno][pointno].sy - cenLegL1.sy;
			normal.z = tlvLegL1[planeno][pointno].sz - cenLegL1.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvLegL1[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvLegL1[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvLegL2()
{
	int	fanno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruleg1.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvLegL2 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvLegL2, 1, VNUMLEGL2, 0 );
 // fan は 1次元配列
	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	cennormal.x =  0.121980f;
	cennormal.y =  0.246005f;
	cennormal.z =  0.044510f;
	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenLegL2.sx = cennormal.x; cenLegL2.sy = cennormal.y + shifty; cenLegL2.sz = cennormal.z;
	for( pointno = 0; pointno < VNUMLEGL2; pointno++ ){
			tlvLegL2[pointno].sx *= scalex;
			tlvLegL2[pointno].sy *= scaley;
			tlvLegL2[pointno].sy += shifty;
			tlvLegL2[pointno].sz *= scalez;
			tlvLegL2[pointno].sz += shiftz;
			tlvLegL2[pointno].rhw = 0.95f;

			normal.x = tlvLegL2[pointno].sx - cenLegL2.sx;
			normal.y = tlvLegL2[pointno].sy - cenLegL2.sy;
			normal.z = tlvLegL2[pointno].sz - cenLegL2.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvLegL2[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvLegL2[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	//fan mem num
	MemNumLegL2[0] = 9;
	MemNumLegL2[1] = 8;
	MemNumLegL2[2] = 8;
	MemNumLegL2[3] = 8;
	MemNumLegL2[4] = 7;
	MemNumLegL2[5] = 7;

	num_trilegl2 = 0;

	for( fanno = 0; fanno < PNUMLEGL2; fanno++ ){
		num_trilegl2 += (MemNumLegL2[fanno] - 2);
	}

	num_trilegl2 *= 2; // 裏面
}

void	InitTlvKtlvheadl()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaeruhead1.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvKtlvheadl : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvKtlvheadl[0], PNUMKTLVHEADL, VNUMKTLVHEADL, 0 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	cennormal.x = cenHead0.sx; cennormal.y = cenHead0.sy; cennormal.z = cenHead0.sz;
	cenKtlvheadl = cenHead0;
	for( planeno = 0; planeno < PNUMKTLVHEADL; planeno++ ){
		for( pointno = 0; pointno < VNUMKTLVHEADL; pointno++ ){
			tlvKtlvheadl[planeno][pointno].sx *= scalex;
			tlvKtlvheadl[planeno][pointno].sy *= scaley;
			tlvKtlvheadl[planeno][pointno].sy += shifty;
			tlvKtlvheadl[planeno][pointno].sz *= scalez;
			tlvKtlvheadl[planeno][pointno].sz += shiftz;
			tlvKtlvheadl[planeno][pointno].rhw = 0.95f;

			normal.x = tlvKtlvheadl[planeno][pointno].sx - cenKtlvheadl.sx;
			normal.y = tlvKtlvheadl[planeno][pointno].sy - cenKtlvheadl.sy;
			normal.z = tlvKtlvheadl[planeno][pointno].sz - cenKtlvheadl.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvKtlvheadl[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvKtlvheadl[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvKtlveyel()
{
	int	fanno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaerueye0.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvKtlveyel : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvKtlveyel, 1, VNUMKTLVEYEL, 0 );
 // fan は 1次元配列
	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	cennormal.x =  0.070294f;
	cennormal.y =  -0.241568f;
	cennormal.z =  0.005975f;
	cennormal.x *= scalex;
	cennormal.y *= scaley; cennormal.y += shifty;
	cennormal.z *= scalez; cennormal.z += shiftz;
	cenKtlveyel.sx = cennormal.x; cenKtlveyel.sy = cennormal.y + shifty; cenKtlveyel.sz = cennormal.z;
	for( pointno = 0; pointno < VNUMKTLVEYEL; pointno++ ){
			tlvKtlveyel[pointno].sx *= scalex;
			tlvKtlveyel[pointno].sy *= scaley;
			tlvKtlveyel[pointno].sy += shifty;
			tlvKtlveyel[pointno].sz *= scalez;
			tlvKtlveyel[pointno].sz += shiftz;
			tlvKtlveyel[pointno].rhw = 0.95f;

			normal.x = tlvKtlveyel[pointno].sx - cenKtlveyel.sx;
			normal.y = tlvKtlveyel[pointno].sy - cenKtlveyel.sy;
			normal.z = tlvKtlveyel[pointno].sz - cenKtlveyel.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvKtlveyel[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvKtlveyel[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	//fan mem num
	MemNumKtlveyel[0] = 14;

	num_triktlveyel = 0;

	for( fanno = 0; fanno < PNUMKTLVEYEL; fanno++ ){
		num_triktlveyel += (MemNumKtlveyel[fanno] - 2);
	}
}

void	InitTlvBody1()
{
	int	planeno, pointno;
	float	scalex, scaley, scalez, shifty, shiftz;
	LOCATION	normal, cennormal;
	RGBDATA	setcol;
	float	seki;
	FILE*	partsfile = NULL;

	partsfile = fopen( "c:\\disp0\\tlvdata\\kaerubody1.txt", "r" );
	if( !partsfile ){
		TRACE( "UVKAERUCPP : InitTlvBody1 : file open error !!!!! \n" );
		return;
	}
	GetParam( partsfile, tlvBody1[0], PNUMBODY1, VNUMBODY1, 0 );	fclose( partsfile );
	scalex = 8.0f; scaley = -8.0f; scalez = -8.0f; shifty = 0.0f; shiftz = 0.0f;
	cennormal.x = cenCtlAll.sx; cennormal.y = cenCtlAll.sy; cennormal.z = cenCtlAll.sz;
	cenBody1 = cenCtlAll;
	for( planeno = 0; planeno < PNUMBODY1; planeno++ ){
		for( pointno = 0; pointno < VNUMBODY1; pointno++ ){
			tlvBody1[planeno][pointno].sx *= scalex;
			tlvBody1[planeno][pointno].sy *= scaley;
			tlvBody1[planeno][pointno].sy += shifty;
			tlvBody1[planeno][pointno].sz *= scalez;
			tlvBody1[planeno][pointno].sz += shiftz;
			tlvBody1[planeno][pointno].rhw = 0.95f;

			normal.x = tlvBody1[planeno][pointno].sx - cenBody1.sx;
			normal.y = tlvBody1[planeno][pointno].sy - cenBody1.sy;
			normal.z = tlvBody1[planeno][pointno].sz - cenBody1.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvBody1[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvBody1[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvKtlvheadr()
{
	int	planeno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv2D( tlvKtlvheadr[0], tlvKtlvheadl[0], PNUMKTLVHEADR, VNUMKTLVHEADR );
	cenKtlvheadr.sx = -cenKtlvheadl.sx; cenKtlvheadr.sy = cenKtlvheadl.sy; cenKtlvheadr.sz = cenKtlvheadl.sz;
	for( planeno = 0; planeno < PNUMKTLVHEADR; planeno++ ){
		for( pointno = 0; pointno < VNUMKTLVHEADR; pointno++ ){
			normal.x = tlvKtlvheadr[planeno][pointno].sx - cenKtlvheadr.sx;
			normal.y = tlvKtlvheadr[planeno][pointno].sy - cenKtlvheadr.sy;
			normal.z = tlvKtlvheadr[planeno][pointno].sz - cenKtlvheadr.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvKtlvheadr[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvKtlvheadr[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvKtlveyer()
{
	int	fanno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv1D( tlvKtlveyer, MemNumKtlveyer, tlvKtlveyel, MemNumKtlveyel, PNUMKTLVEYER, VNUMKTLVEYER );
	cenKtlveyer.sx = -cenKtlveyel.sx; cenKtlveyer.sy = cenKtlveyel.sy; cenKtlveyer.sz = cenKtlveyel.sz;
	for( pointno = 0; pointno < VNUMKTLVEYER; pointno++ ){
			normal.x = tlvKtlveyer[pointno].sx - cenKtlveyer.sx;
			normal.y = tlvKtlveyer[pointno].sy - cenKtlveyer.sy;
			normal.z = tlvKtlveyer[pointno].sz - cenKtlveyer.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvKtlveyer[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvKtlveyer[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	num_triktlveyer = num_triktlveyel;
}

void	InitTlvHead01()
{
	int	fanno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv1D( tlvHead01, MemNumHead01, tlvHead00, MemNumHead00, PNUMHEAD01, VNUMHEAD01 );
	cenHead01.sx = -cenHead00.sx; cenHead01.sy = cenHead00.sy; cenHead01.sz = cenHead00.sz;
	for( pointno = 0; pointno < VNUMHEAD01; pointno++ ){
			normal.x = tlvHead01[pointno].sx - cenHead01.sx;
			normal.y = tlvHead01[pointno].sy - cenHead01.sy;
			normal.z = tlvHead01[pointno].sz - cenHead01.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvHead01[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvHead01[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	num_trihead01 = num_trihead00;
}

void	InitTlvArmR0()
{
	int	planeno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv2D( tlvArmR0[0], tlvArmL0[0], PNUMARMR0, VNUMARMR0 );
	cenArmR0.sx = -cenArmL0.sx; cenArmR0.sy = cenArmL0.sy; cenArmR0.sz = cenArmL0.sz;
	for( planeno = 0; planeno < PNUMARMR0; planeno++ ){
		for( pointno = 0; pointno < VNUMARMR0; pointno++ ){
			normal.x = tlvArmR0[planeno][pointno].sx - cenArmR0.sx;
			normal.y = tlvArmR0[planeno][pointno].sy - cenArmR0.sy;
			normal.z = tlvArmR0[planeno][pointno].sz - cenArmR0.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvArmR0[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvArmR0[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvArmR1()
{
	int	planeno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv2D( tlvArmR1[0], tlvArmL1[0], PNUMARMR1, VNUMARMR1 );
	cenArmR1.sx = -cenArmL1.sx; cenArmR1.sy = cenArmL1.sy; cenArmR1.sz = cenArmL1.sz;
	for( planeno = 0; planeno < PNUMARMR1; planeno++ ){
		for( pointno = 0; pointno < VNUMARMR1; pointno++ ){
			normal.x = tlvArmR1[planeno][pointno].sx - cenArmR1.sx;
			normal.y = tlvArmR1[planeno][pointno].sy - cenArmR1.sy;
			normal.z = tlvArmR1[planeno][pointno].sz - cenArmR1.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvArmR1[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvArmR1[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvArmR2()
{
	int	fanno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv1D( tlvArmR2, MemNumArmR2, tlvArmL2, MemNumArmL2, PNUMARMR2, VNUMARMR2 );
	cenArmR2.sx = -cenArmL2.sx; cenArmR2.sy = cenArmL2.sy; cenArmR2.sz = cenArmL2.sz;
	for( pointno = 0; pointno < VNUMARMR2; pointno++ ){
			normal.x = tlvArmR2[pointno].sx - cenArmR2.sx;
			normal.y = tlvArmR2[pointno].sy - cenArmR2.sy;
			normal.z = tlvArmR2[pointno].sz - cenArmR2.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvArmR2[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvArmR2[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	num_triarmr2 = num_triarml2;
}

void	InitTlvLegR0()
{
	int	planeno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv2D( tlvLegR0[0], tlvLegL0[0], PNUMLEGR0, VNUMLEGR0 );
	cenLegR0.sx = -cenLegL0.sx; cenLegR0.sy = cenLegL0.sy; cenLegR0.sz = cenLegL0.sz;
	for( planeno = 0; planeno < PNUMLEGR0; planeno++ ){
		for( pointno = 0; pointno < VNUMLEGR0; pointno++ ){
			normal.x = tlvLegR0[planeno][pointno].sx - cenLegR0.sx;
			normal.y = tlvLegR0[planeno][pointno].sy - cenLegR0.sy;
			normal.z = tlvLegR0[planeno][pointno].sz - cenLegR0.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvLegR0[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvLegR0[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvLegR1()
{
	int	planeno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv2D( tlvLegR1[0], tlvLegL1[0], PNUMLEGR1, VNUMLEGR1 );
	cenLegR1.sx = -cenLegL1.sx; cenLegR1.sy = cenLegL1.sy; cenLegR1.sz = cenLegL1.sz;
	for( planeno = 0; planeno < PNUMLEGR1; planeno++ ){
		for( pointno = 0; pointno < VNUMLEGR1; pointno++ ){
			normal.x = tlvLegR1[planeno][pointno].sx - cenLegR1.sx;
			normal.y = tlvLegR1[planeno][pointno].sy - cenLegR1.sy;
			normal.z = tlvLegR1[planeno][pointno].sz - cenLegR1.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvLegR1[planeno][pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvLegR1[planeno][pointno].specular = RGB_MAKE( 0, 0, 0 );
		}
	}

}

void	InitTlvLegR2()
{
	int	fanno, pointno;
	LOCATION	normal;
	RGBDATA	setcol;
	float	seki;
	MirrorTlv1D( tlvLegR2, MemNumLegR2, tlvLegL2, MemNumLegL2, PNUMLEGR2, VNUMLEGR2 );
	cenLegR2.sx = -cenLegL2.sx; cenLegR2.sy = cenLegL2.sy; cenLegR2.sz = cenLegL2.sz;
	for( pointno = 0; pointno < VNUMLEGR2; pointno++ ){
			normal.x = tlvLegR2[pointno].sx - cenLegR2.sx;
			normal.y = tlvLegR2[pointno].sy - cenLegR2.sy;
			normal.z = tlvLegR2[pointno].sz - cenLegR2.sz;

			Calc->VecNormalizeXYZ( &normal );
			seki = normal.x * lightdir.x + normal.y * lightdir.y + normal.z * lightdir.z + 1.0f;
			setcol.r = (int)(coldiffuse.r - seki * colrate);
			setcol.g = setcol.r;
			setcol.b = setcol.r;
			Calc->ClampRGB( &setcol );
			tlvLegR2[pointno].color = RGB_MAKE( setcol.r, setcol.g, setcol.b );
			tlvLegR2[pointno].specular = RGB_MAKE( 0, 0, 0 );
	}


	num_trilegr2 = num_trilegl2;
}



//EDITPOINT!!!	KaeruMultMatrix
/*** KAERU MULT MATRIX ***/
void	KaeruMultMatrix()
{

	SetKaeruClipStatus();
	//法線ベクトルは 回転していない
	m_curmove = kaerupos.Move; // 予期しないタイミングでの変更に 耐えるように コピー
	curcharparam = m_param[ m_curmove ]; // pis , fire で 参照する
	MultMatrixCtlAll();
	MultMatrixHead0();
	MultMatrixHead00();
	MultMatrixHead02();
	MultMatrixArmL0();
	MultMatrixArmL1();
	MultMatrixArmL2();
	MultMatrixLegL0();
	MultMatrixLegL1();
	MultMatrixLegL2();
	MultMatrixKtlvheadl();
	MultMatrixKtlveyel();
	MultMatrixBody1();
	MultMatrixKtlvheadr();
	MultMatrixKtlveyer();
	MultMatrixHead01();
	MultMatrixArmR0();
	MultMatrixArmR1();
	MultMatrixArmR2();
	MultMatrixLegR0();
	MultMatrixLegR1();
	MultMatrixLegR2();

}



//EDITPOINT!!!	MULT PARTS MATRIX
/*** MULT PARTS MATRIX ***/
void	MultMatrixCtlAll()
{
	m_curmatrix[ CTLALL ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + CTLALL );
	m_curmatrix[ CTLALL ].MultVertEBFix3( pkaerutlv[0], tlvCtlAll[0], PNUMCTLALL, VNUMCTLALL );
}

void	MultMatrixHead0()
{
	m_curmatrix[ HEAD0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD0 );
	m_curmatrix[ HEAD0 ].MultVertEBFix3( pkaerutlv[1], tlvHead0[0], PNUMHEAD0, VNUMHEAD0 );
}

void	MultMatrixHead00()
{
	m_curmatrix[ HEAD00 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD00 );
	m_curmatrix[ HEAD00 ].MultVertEBFix3( pkaerutlv[6], tlvHead00, 1, VNUMHEAD00 );
}

void	MultMatrixHead02()
{
	m_curmatrix[ HEAD0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD0 );
	m_curmatrix[ HEAD0 ].MultVertEBFix3( pkaerutlv[8], tlvHead02, 1, VNUMHEAD02 );
}

void	MultMatrixArmL0()
{
	m_curmatrix[ ARML0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + ARML0 );
	m_curmatrix[ ARML0 ].MultVertEBFix3( pkaerutlv[9], tlvArmL0[0], PNUMARML0, VNUMARML0 );
}

void	MultMatrixArmL1()
{
	m_curmatrix[ ARML1 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + ARML1 );
	m_curmatrix[ ARML1 ].MultVertEBFix3( pkaerutlv[10], tlvArmL1[0], PNUMARML1, VNUMARML1 );
}

void	MultMatrixArmL2()
{
	m_curmatrix[ ARML2 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + ARML2 );
	m_curmatrix[ ARML2 ].MultVertEBFix3( pkaerutlv[11], tlvArmL2, 1, VNUMARML2 );
}

void	MultMatrixLegL0()
{
	m_curmatrix[ LEGL0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + LEGL0 );
	m_curmatrix[ LEGL0 ].MultVertEBFix3( pkaerutlv[15], tlvLegL0[0], PNUMLEGL0, VNUMLEGL0 );
}

void	MultMatrixLegL1()
{
	m_curmatrix[ LEGL1 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + LEGL1 );
	m_curmatrix[ LEGL1 ].MultVertEBFix3( pkaerutlv[16], tlvLegL1[0], PNUMLEGL1, VNUMLEGL1 );
}

void	MultMatrixLegL2()
{
	m_curmatrix[ LEGL2 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + LEGL2 );
	m_curmatrix[ LEGL2 ].MultVertEBFix3( pkaerutlv[17], tlvLegL2, 1, VNUMLEGL2 );
}

void	MultMatrixKtlvheadl()
{
	m_curmatrix[ HEAD0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD0 );
	m_curmatrix[ HEAD0 ].MultVertEBFix3( pkaerutlv[2], tlvKtlvheadl[0], PNUMKTLVHEADL, VNUMKTLVHEADL );
}

void	MultMatrixKtlveyel()
{
	m_curmatrix[ HEAD0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD0 );
	m_curmatrix[ HEAD0 ].MultVertEBFix3( pkaerutlv[4], tlvKtlveyel, 1, VNUMKTLVEYEL );
}

void	MultMatrixBody1()
{
	m_curmatrix[ CTLALL ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + CTLALL );
	m_curmatrix[ CTLALL ].MultVertEBFix3( pkaerutlv[23], tlvBody1[0], PNUMBODY1, VNUMBODY1 );
}

void	MultMatrixKtlvheadr()
{
	m_curmatrix[ HEAD0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD0 );
	m_curmatrix[ HEAD0 ].MultVertEBFix3( pkaerutlv[3], tlvKtlvheadr[0], PNUMKTLVHEADR, VNUMKTLVHEADR );
}

void	MultMatrixKtlveyer()
{
	m_curmatrix[ HEAD0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD0 );
	m_curmatrix[ HEAD0 ].MultVertEBFix3( pkaerutlv[5], tlvKtlveyer, 1, VNUMKTLVEYER );
}

void	MultMatrixHead01()
{
	m_curmatrix[ HEAD01 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + HEAD01 );
	m_curmatrix[ HEAD01 ].MultVertEBFix3( pkaerutlv[7], tlvHead01, 1, VNUMHEAD01 );
}

void	MultMatrixArmR0()
{
	m_curmatrix[ ARMR0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + ARMR0 );
	m_curmatrix[ ARMR0 ].MultVertEBFix3( pkaerutlv[12], tlvArmR0[0], PNUMARMR0, VNUMARMR0 );
}

void	MultMatrixArmR1()
{
	m_curmatrix[ ARMR1 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + ARMR1 );
	m_curmatrix[ ARMR1 ].MultVertEBFix3( pkaerutlv[13], tlvArmR1[0], PNUMARMR1, VNUMARMR1 );
}

void	MultMatrixArmR2()
{
	m_curmatrix[ ARMR2 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + ARMR2 );
	m_curmatrix[ ARMR2 ].MultVertEBFix3( pkaerutlv[14], tlvArmR2, 1, VNUMARMR2 );
}

void	MultMatrixLegR0()
{
	m_curmatrix[ LEGR0 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + LEGR0 );
	m_curmatrix[ LEGR0 ].MultVertEBFix3( pkaerutlv[18], tlvLegR0[0], PNUMLEGR0, VNUMLEGR0 );
}

void	MultMatrixLegR1()
{
	m_curmatrix[ LEGR1 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + LEGR1 );
	m_curmatrix[ LEGR1 ].MultVertEBFix3( pkaerutlv[19], tlvLegR1[0], PNUMLEGR1, VNUMLEGR1 );
}

void	MultMatrixLegR2()
{
	m_curmatrix[ LEGR2 ].MultMatrix2( &trans_cmatrix, m_matrix[m_curmove] + LEGR2 );
	m_curmatrix[ LEGR2 ].MultVertEBFix3( pkaerutlv[20], tlvLegR2, 1, VNUMLEGR2 );
}



//EDITPOINT!!!	InitKaeruMatrix
/*** INIT KAERU MATRIX ***/
void	InitKaeruMatrix()
{

	int	partsno, moveno;

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
		for( partsno = 0; partsno < CTLMAX; partsno++ ){
			m_matrix[ moveno ][ partsno ].SetCalcPt( Calc );
		}
	}

	for( partsno = 0; partsno < CTLMAX; partsno++ ){
		m_curmatrix[ partsno ].SetCalcPt( Calc );
	}

	InitMatrixCtlAll(); // 必ず 最初 (全parts に 掛けられる)
	InitMatrixHead0();
	InitMatrixHead00();
	InitMatrixHead02();
	InitMatrixArmL0();
	InitMatrixArmL1();
	InitMatrixArmL2();
	InitMatrixLegL0();
	InitMatrixLegL1();
	InitMatrixLegL2();
	InitMatrixHead01();
	InitMatrixArmR0();
	InitMatrixArmR1();
	InitMatrixArmR2();
	InitMatrixLegR0();
	InitMatrixLegR1();
	InitMatrixLegR2();
	InitBodyDirMat();

}



//EDITPOINT!!!	InitBodyDirMat
/*** INIT BODY DIR MATRIX ***/
void InitBodyDirMat()
{

	int	dirno;
	float deg[9] = { 0.0f, 0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 225.0f, 270.0f, 315.0f };

	for( dirno = 0; dirno <= 8; dirno++ ){
		bodydir_cmat[dirno].SetCalcPt( Calc );
		bodydir_cmat[dirno].SetMatrixCenterRotXYZ( 0.0f, deg[dirno], 0.0f, cenCtlAll );
	}
}



//EDITPOINT!!!	INIT PARTS MATRIX
/*** INIT PARTS MATRIX ***/
void	InitMatrixCtlAll()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//CtlAll Matrix
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXctlall, cparam.rotYctlall, cparam.rotZctlall, cenCtlAll );
		partsshift.SetMatrixTraXYZ( cparam.shiftXctlall, cparam.shiftYctlall, cparam.shiftZctlall );

		m_matrix[moveno][ CTLALL ] = partsshift * partsrot;
	}
}

void	InitMatrixHead0()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXhead0, cparam.rotYhead0, cparam.rotZhead0, cenHead0 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXhead0, cparam.shiftYhead0, cparam.shiftZhead0 );

		m_matrix[moveno][ HEAD0 ] = m_matrix[moveno][ CTLALL ] * partsshift * partsrot;
	}
}

void	InitMatrixHead00()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXhead00, cparam.rotYhead00, cparam.rotZhead00, cenHead00 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXhead00, cparam.shiftYhead00, cparam.shiftZhead00 );

		m_matrix[moveno][ HEAD00 ] = m_matrix[moveno][ HEAD0 ] * partsshift * partsrot;
	}
}

void	InitMatrixHead02()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
		//matrix の 使いまわし ！！！
		m_matrix[moveno][ HEAD02 ] = m_matrix[moveno][ HEAD0 ];
	}
}

void	InitMatrixArmL0()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXarml0, cparam.rotYarml0, cparam.rotZarml0, cenArmL0 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXarml0, cparam.shiftYarml0, cparam.shiftZarml0 );

		m_matrix[moveno][ ARML0 ] = m_matrix[moveno][ CTLALL ] * partsshift * partsrot;
	}
}

void	InitMatrixArmL1()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXarml1, cparam.rotYarml1, cparam.rotZarml1, cenArmL1 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXarml1, cparam.shiftYarml1, cparam.shiftZarml1 );

		m_matrix[moveno][ ARML1 ] = m_matrix[moveno][ ARML0 ] * partsshift * partsrot;
	}
}

void	InitMatrixArmL2()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXarml2, cparam.rotYarml2, cparam.rotZarml2, cenArmL2 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXarml2, cparam.shiftYarml2, cparam.shiftZarml2 );

		m_matrix[moveno][ ARML2 ] = m_matrix[moveno][ ARML1 ] * partsshift * partsrot;
	}
}

void	InitMatrixLegL0()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXlegl0, cparam.rotYlegl0, cparam.rotZlegl0, cenLegL0 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXlegl0, cparam.shiftYlegl0, cparam.shiftZlegl0 );

		m_matrix[moveno][ LEGL0 ] = m_matrix[moveno][ CTLALL ] * partsshift * partsrot;
	}
}

void	InitMatrixLegL1()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXlegl1, cparam.rotYlegl1, cparam.rotZlegl1, cenLegL1 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXlegl1, cparam.shiftYlegl1, cparam.shiftZlegl1 );

		m_matrix[moveno][ LEGL1 ] = m_matrix[moveno][ LEGL0 ] * partsshift * partsrot;
	}
}

void	InitMatrixLegL2()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXlegl2, cparam.rotYlegl2, cparam.rotZlegl2, cenLegL2 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXlegl2, cparam.shiftYlegl2, cparam.shiftZlegl2 );

		m_matrix[moveno][ LEGL2 ] = m_matrix[moveno][ LEGL1 ] * partsshift * partsrot;
	}
}

void	InitMatrixHead01()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXhead01, cparam.rotYhead01, cparam.rotZhead01, cenHead01 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXhead01, cparam.shiftYhead01, cparam.shiftZhead01 );

		m_matrix[moveno][ HEAD01 ] = m_matrix[moveno][ HEAD0 ] * partsshift * partsrot;
	}
}

void	InitMatrixArmR0()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXarmr0, cparam.rotYarmr0, cparam.rotZarmr0, cenArmR0 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXarmr0, cparam.shiftYarmr0, cparam.shiftZarmr0 );

		m_matrix[moveno][ ARMR0 ] = m_matrix[moveno][ CTLALL ] * partsshift * partsrot;
	}
}

void	InitMatrixArmR1()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXarmr1, cparam.rotYarmr1, cparam.rotZarmr1, cenArmR1 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXarmr1, cparam.shiftYarmr1, cparam.shiftZarmr1 );

		m_matrix[moveno][ ARMR1 ] = m_matrix[moveno][ ARMR0 ] * partsshift * partsrot;
	}
}

void	InitMatrixArmR2()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXarmr2, cparam.rotYarmr2, cparam.rotZarmr2, cenArmR2 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXarmr2, cparam.shiftYarmr2, cparam.shiftZarmr2 );

		m_matrix[moveno][ ARMR2 ] = m_matrix[moveno][ ARMR1 ] * partsshift * partsrot;
	}
}

void	InitMatrixLegR0()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXlegr0, cparam.rotYlegr0, cparam.rotZlegr0, cenLegR0 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXlegr0, cparam.shiftYlegr0, cparam.shiftZlegr0 );

		m_matrix[moveno][ LEGR0 ] = m_matrix[moveno][ CTLALL ] * partsshift * partsrot;
	}
}

void	InitMatrixLegR1()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXlegr1, cparam.rotYlegr1, cparam.rotZlegr1, cenLegR1 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXlegr1, cparam.shiftYlegr1, cparam.shiftZlegr1 );

		m_matrix[moveno][ LEGR1 ] = m_matrix[moveno][ LEGR0 ] * partsshift * partsrot;
	}
}

void	InitMatrixLegR2()
{
	int moveno;
	CHARPARAM cparam;
	CMatrix	partsrot( Calc ), partsshift( Calc );
	CMatrix rotonly( Calc );

	for( moveno = 0; moveno < MYFLAGMAX; moveno++ ){
	//親の matrix を 掛ける
		cparam = m_param[moveno];

		partsrot.SetMatrixCenterRotXYZ( cparam.rotXlegr2, cparam.rotYlegr2, cparam.rotZlegr2, cenLegR2 );
		partsshift.SetMatrixTraXYZ( cparam.shiftXlegr2, cparam.shiftYlegr2, cparam.shiftZlegr2 );

		m_matrix[moveno][ LEGR2 ] = m_matrix[moveno][ LEGR1 ] * partsshift * partsrot;
	}
}





void SetCharParam()
{
	int	viewcnt;
	
	for( viewcnt = 0; viewcnt < MYFLAGMAX; viewcnt++ ){
		Calc->InitCharParam( &m_param[viewcnt] );
	}

	//EDITPOINT!!!LOADMOTION
	LoadMotion( m_param, IDLE0, "c:\\disp0\\motiondata\\idle.motion" );
	LoadMotion( m_param, WALK0, "c:\\disp0\\motiondata\\walk.motion" );
	LoadMotion( m_param, JUMP0, "c:\\disp0\\motiondata\\jump.motion" );
	LoadMotion( m_param, KICK0, "c:\\disp0\\motiondata\\kick.motion" );
	LoadMotion( m_param, RIDE_C0, "c:\\disp0\\motiondata\\ride_c.motion" );
	LoadMotion( m_param, PUSHOBJ0, "c:\\disp0\\motiondata\\pushobj.motion" );
	LoadMotion( m_param, PUSHED0, "c:\\disp0\\motiondata\\pushed.motion" );
	LoadMotion( m_param, INTERSECT0, "c:\\disp0\\motiondata\\intersect.motion" );
	LoadMotion( m_param, RIDEONIDLE0, "c:\\disp0\\motiondata\\rideonidle.motion" );
	LoadMotion( m_param, RIDEON0, "c:\\disp0\\motiondata\\rideon.motion" );
	LoadMotion( m_param, KICKED0, "c:\\disp0\\motiondata\\kicked.motion" );
	LoadMotion( m_param, MPUSHED0, "c:\\disp0\\motiondata\\mpushed.motion" );
	LoadMotion( m_param, ZROTL0, "c:\\disp0\\motiondata\\zrotl.motion" );
	LoadMotion( m_param, ZROTR0, "c:\\disp0\\motiondata\\zrotr.motion" );
	LoadMotion( m_param, KANIL0, "c:\\disp0\\motiondata\\kanil.motion" );
	LoadMotion( m_param, KANIR0, "c:\\disp0\\motiondata\\kanir.motion" );
	LoadMotion( m_param, DROPHOLE0, "c:\\disp0\\motiondata\\drophole.motion" );
	LoadMotion( m_param, UPFLOOR0, "c:\\disp0\\motiondata\\upfloor.motion" );
	LoadMotion( m_param, DOWNFLOOR0, "c:\\disp0\\motiondata\\downfloor.motion" );
	LoadMotion( m_param, UMAIDLE0, "c:\\disp0\\motiondata\\umaidle.motion" );
	LoadMotion( m_param, UMA0, "c:\\disp0\\motiondata\\uma.motion" );
	LoadMotion( m_param, PIS0, "c:\\disp0\\motiondata\\pis.motion" );
	LoadMotion( m_param, FIRE0, "c:\\disp0\\motiondata\\fire.motion" );
	LoadMotion( m_param, DETH, "c:\\disp0\\motiondata\\deth.motion" );
//ENDEDIT











































	FreeLDMBuff();
}


void KaeruMove( int sckind )
{
	KaeruPosSet();
	CamPosSet( sckind );
}
void	KaeruPosSet()
{
	int	dirno = kaerupos.dirindex;
	//int	walkflag;
	INDEX_L	nextindex;

	if( gplayarea->isMyMoving[ kaerupos.Move ] ){
		nextindex.x = kaerupos.index.x + gcharmoveindex[dirno].x;
		nextindex.y = kaerupos.index.y + gcharmoveindex[dirno].y;
		nextindex.z = kaerupos.index.z + gcharmoveindex[dirno].z;

		//walkflag = gwalkdata[nextindex.z][nextindex.x].walkflag;
		//if( gplayarea->isWalk[walkflag][dirno] ){
			kaerupos.index = nextindex;
			gplayarea->Index2Loc( &kaerupos.loc, &kaerupos.index );
		//}

	}

	//cam2char.x = kaerupos.loc.x - campos.loc.x;
	//cam2char.y = kaerupos.loc.y - campos.loc.y;
	//cam2char.z = kaerupos.loc.z - campos.loc.z;
	//Calc->VecNormalizeXYZ( &cam2char );

	curdirindex = kaerupos.dirindex;
	//m_curdirmatrix.SetD3DMatrix( Calc, gdir_cmatrix[curdirindex] );
	m_curdirmatrix.SetD3DMatrix( Calc, bodydir_cmat[curdirindex] );


	//TRACE( "UVAKERUCPP : dirmatrix %d\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n",
	//	curdirindex,
	//	m_curdirmatrix._11, m_curdirmatrix._21, m_curdirmatrix._31, m_curdirmatrix._41, 
	//	m_curdirmatrix._12, m_curdirmatrix._22, m_curdirmatrix._32, m_curdirmatrix._42, 
	//	m_curdirmatrix._13, m_curdirmatrix._23, m_curdirmatrix._33, m_curdirmatrix._43, 
	//	m_curdirmatrix._14, m_curdirmatrix._24, m_curdirmatrix._34, m_curdirmatrix._44 );


	//TRACE( "UVKAERUCPP : KaeruPosSet : curdirindex %d\n", curdirindex );

}

void	CamPosSet( int sckind )
{
	//float	calcSinX, calcCosX, calcSinY, calcCosY, calcSinZ, calcCosZ;

	switch( sckind ){
	case SCROLL_NONE:
		break;
	case SCROLL_X:
		campos.index.x = kaerupos.index.x;
		campos.index.y = kaerupos.index.y + gcamheight;
		gplayarea->Index2Loc( &campos.loc, &campos.index );
		break;
	case SCROLL_Z:
		campos.index.y = kaerupos.index.y + gcamheight;
		campos.index.z = kaerupos.index.z - gcamdist;
		gplayarea->Index2Loc( &campos.loc, &campos.index );
		break;
	case SCROLL_XZ:
		campos.index.x = kaerupos.index.x;
		campos.index.y = kaerupos.index.y + gcamheight;
		campos.index.z = kaerupos.index.z - gcamdist;
		gplayarea->Index2Loc( &campos.loc, &campos.index );
		break;
	default:
		break;
	}

	//calcSinX = Calc->DEGSin( campos.rot.x );
	//calcCosX = Calc->DEGCos( campos.rot.x );
	//calcSinY = Calc->DEGSin( campos.rot.y );
	//calcCosY = Calc->DEGCos( campos.rot.y );
	//calcSinZ = Calc->DEGSin( campos.rot.z );
	//calcCosZ = Calc->DEGCos( campos.rot.z );

	//campos.dir.x = calcSinY;
	//campos.dir.y = -calcSinX * calcCosY;
	//campos.dir.z = calcCosX * calcCosY;
}

void	SetKaeruClipStatus()
{
	D3DVECTOR	kloc = kaerupos.loc;
	D3DVECTOR	camloc = campos.loc;

	tra_cmatrix.SetInitMatrix();
		// world * view 平行移動のみの場合は  trax = worldx - viewx !!!!
	tra_cmatrix._41 = kloc.x - camloc.x;
	tra_cmatrix._42 = kloc.y + kaeru_army - camloc.y;
	tra_cmatrix._43 = kloc.z - camloc.z;	

	//trans_cmatrix.SetD3DMatrix( tra_cmatrix );/////////////////////////////////
	trans_cmatrix.MultMatrix2( &tra_cmatrix, &m_curdirmatrix );

}

void SetViewcenrot( CMatrix& gviewcenrot )
{
	l_viewcenrot.SetD3DMatrix( gviewcenrot );
}


void MirrorTlv2D( D3DTLVERTEX* retvert, D3DTLVERTEX* svert, int planenum, int pointnum )
{
	int	planeno, pointno;
	int	retdatano, sdatano;

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			retdatano = pointnum * planeno + pointno;
			sdatano = pointnum * (planeno + 1) - 1 - pointno;
			
			*(retvert + retdatano) = *(svert + sdatano); 	
			(retvert + retdatano)->sx *= -1.0f; // !!!!!!!!
		}
	}


}

void MirrorTlv1D( D3DTLVERTEX* retvert, int* retmemnum, D3DTLVERTEX* svert, int* smemnum, 
				int fannum, int pointnum )
{
	int	fanno, pointno;
	int	retdatano, sdatano;
	int	centerno;

	retdatano = 0;
	sdatano = 0;
	for( fanno = 0; fanno < fannum; fanno++ ){
		// fan の開始点
		centerno = retdatano;
		*(retvert + retdatano) = *(svert + centerno);
		(retvert + retdatano)->sx *= -1.0f; // !!!!!!!!
		retdatano++; 

		// ２点目 以降
		for( pointno = 1; pointno < *(smemnum + fanno); pointno++ ){
			sdatano = centerno + *(smemnum + fanno) - pointno;
			*(retvert + retdatano) = *(svert + sdatano);
			(retvert + retdatano)->sx *= -1.0f; // !!!!!!!!
			retdatano++;
		}		

		// memnum の copy
		*(retmemnum + fanno) = *(smemnum + fanno);
	}

}



