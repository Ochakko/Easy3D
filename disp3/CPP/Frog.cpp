// ver1_5 dbg : TIME : 8 月 4 日 17 時 2 分 26 秒
//tlv 3 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 8 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 9 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 10 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 14 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 15 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 16 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 17 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 18 : param is not definded
	 // 1 1 1 1 0 0 0
//tlv 21 : param is not definded
	 // 1 1 0 0 0 0 0
//tlv 23 : param is not definded
	 // 1 1 0 0 0 0 0

#include	"stdafx.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#include <d3drmwin.h>
#ifndef	INITDDH
	#include "initdd.h"
#endif

#ifndef	TEXTUREH
	#include "texture.h"
#endif

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
#ifndef	PLAYAREAH
	#include	"PlayArea.h"
#endif

#ifndef DISPMODEH
	#include "dispmode.h"
#endif

#ifndef LOADMOTH
	#include "loadmot.h"
#endif
#ifndef	MAPDATAH
	#include	"mapdata.h"
#endif

#ifndef	FROGH
	#include	"frog.h"
#endif
#include	"d3dmacs.h" // execute buffer マクロ

// extern
//extern void GetParam( FILE*, D3DTLVERTEX*, int, int, int ); // getparam.cpp
extern int LoadMotion( CHARPARAM* ldmot, int ldcnt, char* ldname, int ldtype, int ldnum ); // loadmot.cpp

extern CMapData*	MapData;

extern LONG			g_lThreadFr0;

extern D3Zvars D3Z;
extern HANDLE		g_hMuteProjM;
extern HANDLE		g_hMuteCampos;
extern HANDLE		g_hMuteKaerupos;
extern HANDLE		g_hMuteVRotMat;

extern HANDLE		g_hEventFr0;
extern HANDLE		g_hMuteFlTemp0;

extern CMatrix		g_VRotMat;

//extern float		g_viewdeg;

extern LONG			gprojmode;
extern CCalc*		Calc;
extern CPlayArea*	gplayarea;
extern CRand*		Rand;
extern CHARPOS		kaerupos;
extern CHARPOS		campos;
extern float		kaeru_army;

//extern WALKDATA		gwalkdata[MAXINDEXZ][MAXINDEXX];
extern LOCATION		gdirxz[9], gdiry[7], gcharmovedir[10];
extern LOCATION		lightdir;
extern CMatrix		gdir_cmatrix[9];

extern CMatrix			gworld_cmatrix, gview_cmatrix, gproj_cmatrix, gtemp_cmatrix;
extern CMatrix			gscreen_cmatrix, gproj_screen_cmatrix;

extern INDEX_L		gcharmoveindex[10];
extern CMatrix		viewcenrot; // backgcpp
extern int			gcamdist; // mainfrmcpp : kaeruindexz - camindexz
extern int			gcamheight; // camindexy - kaeruindexy

////////////////////////
//static



////////////////////////

CFrog::CFrog()
{
	bfinishConstruct = FALSE;
	//EDITPOINT!!!	EBNUM(INIT)

	/*** VERTNUM FOR EXECUTE BUFFER ***/
	num_tlvctlall = PNUMCTLALL * VNUMCTLALL;
	num_trictlall = ( PNUMCTLALL - 1 ) * VNUMCTLALL * 2;

	num_tlvhead0 = PNUMHEAD0 * VNUMHEAD0;
	num_trihead0 = ( PNUMHEAD0 - 1 ) * VNUMHEAD0 * 2;

	num_tlvhead00 = VNUMHEAD00;
	num_trihead00 = 0; // InitTlv で初期化 

	num_tlvhead02 = VNUMHEAD02;
	num_trihead02 = 0; // InitTlv で初期化 

	num_tlvarml0 = PNUMARML0 * VNUMARML0;
	num_triarml0 = ( PNUMARML0 - 1 ) * VNUMARML0 * 2;

	num_tlvarml1 = PNUMARML1 * VNUMARML1;
	num_triarml1 = PNUMARML1 * VNUMARML1 * 2; // JOINT TRIANGLE を 含む

	num_tlvarml2 = VNUMARML2;
	num_triarml2 = 0; // InitTlv で初期化 

	num_tlvlegl0 = PNUMLEGL0 * VNUMLEGL0;
	num_trilegl0 = ( PNUMLEGL0 - 1 ) * VNUMLEGL0 * 2;

	num_tlvlegl1 = PNUMLEGL1 * VNUMLEGL1;
	num_trilegl1 = PNUMLEGL1 * VNUMLEGL1 * 2; // JOINT TRIANGLE を 含む

	num_tlvlegl2 = VNUMLEGL2;
	num_trilegl2 = 0; // InitTlv で初期化 

	num_tlvbody1 = PNUMBODY1 * VNUMBODY1;
	num_tribody1 = ( PNUMBODY1 - 1 ) * ( VNUMBODY1 - 1 ) * 2;

	num_tlvktlvheadl = PNUMKTLVHEADL * VNUMKTLVHEADL;
	num_triktlvheadl = ( PNUMKTLVHEADL - 1 ) * VNUMKTLVHEADL * 2;

	num_tlvktlveyel = VNUMKTLVEYEL;
	num_triktlveyel = 0; // InitTlv で初期化 

	num_tlvhead01 = VNUMHEAD01;
	num_trihead01 = 0; // InitTlv で初期化 

	num_tlvarmr0 = PNUMARMR0 * VNUMARMR0;
	num_triarmr0 = ( PNUMARMR0 - 1 ) * VNUMARMR0 * 2;

	num_tlvarmr1 = PNUMARMR1 * VNUMARMR1;
	num_triarmr1 = PNUMARMR1 * VNUMARMR1 * 2; // JOINT TRIANGLE を 含む

	num_tlvarmr2 = VNUMARMR2;
	num_triarmr2 = 0; // InitTlv で初期化 

	num_tlvlegr0 = PNUMLEGR0 * VNUMLEGR0;
	num_trilegr0 = ( PNUMLEGR0 - 1 ) * VNUMLEGR0 * 2;

	num_tlvlegr1 = PNUMLEGR1 * VNUMLEGR1;
	num_trilegr1 = PNUMLEGR1 * VNUMLEGR1 * 2; // JOINT TRIANGLE を 含む

	num_tlvlegr2 = VNUMLEGR2;
	num_trilegr2 = 0; // InitTlv で初期化 

	num_tlvktlvheadr = PNUMKTLVHEADR * VNUMKTLVHEADR;
	num_triktlvheadr = ( PNUMKTLVHEADR - 1 ) * VNUMKTLVHEADR * 2;

	num_tlvktlveyer = VNUMKTLVEYER;
	num_triktlveyer = 0; // InitTlv で初期化 

	/** total num **/
	num_tlvkaeru1 = 0; // !!! InitKaeruVertex で 初期化
	num_trikaeru1 = 0;

	num_kaeruinst1 = 3;// exit の分も忘れずに
	num_kaerustate1 = 0;
	num_kaeruprov1 = 1;

	num_ini_kinst = 3;// exit の分も忘れずに
	num_ini_kstate = 5;

	num_bld_kinst = 3;// exit の分も忘れずに
	num_bld_kstate = 5;


	m_p2mat = new CParam2Mat();
	if( !m_p2mat ){
		TRACE( "FROG : CParam2Mat Create error!!!\n" );
		return;
	}

	m_armat = new CArrayMatrix( MYMOVEMAX * CTLMAX );
	if( !m_armat || !(m_armat->bfinishConstruct) ){
		TRACE( "FROG : m_armat Create error!!!\n" );
		return;
	}

	m_arpause = new CArrayMatrix( CTLMAX );
	if( !m_arpause || !(m_arpause->bfinishConstruct) ){
		TRACE( "FROG : m_arpause Create error!!!\n" );
		return;
	}
	
	InitMultVars();

	BOOL	ret;
	ret = InitKaeruVisual();
	if( !ret ){
		TRACE( "FROGCPP : InitKaeruVisual error!!\n" );
		return;
	}

	ret = InitEBVars(); // InitKaeruVertex より 後で
	if( !ret ){
		TRACE( "FROGCPP : InitEBVars error!!\n" );
		return;
	}

	InitDeviceVars();

	bfinishConstruct = TRUE;
}

CFrog::~CFrog()
{
	DestroyKaeru();
}


HANDLE	CFrog::s_hHeap = NULL;
unsigned int	CFrog::s_uNumAllocsInHeap = 0;


void	*CFrog::operator new ( size_t size )
{
	TRACE( "CFROG CREATE size %d\n", size );

	unsigned int	commitsize;
	commitsize = size + sizeof( FrogVisual ) * 2;

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


void	CFrog::operator delete ( void *p )
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


BOOL CFrog::InitEBVars()
{
	//DWORD	dwVertexSize, dwInstructionSize, dwExecuteBufferSize;

	lpKaeruEB1 = 0;
	memset( &kaeruExeBuffDesc1, 0, sizeof( D3DEXECUTEBUFFERDESC ) );
	memset( &kaeruExecuteData1, 0, sizeof( D3DEXECUTEDATA ) );
	
	lpIKaeruEB = 0;
	memset( &ikaeruExeBuffDesc, 0, sizeof( D3DEXECUTEBUFFERDESC ) );
	memset( &ikaeruExecuteData, 0, sizeof( D3DEXECUTEDATA ) );

	lpBKaeruEB = 0;
	memset( &bkaeruExeBuffDesc, 0, sizeof( D3DEXECUTEBUFFERDESC ) );
	memset( &bkaeruExecuteData, 0, sizeof( D3DEXECUTEDATA ) );

	m_dwVertSize = ( num_tlvkaeru1 * sizeof( D3DTLVERTEX) );//
	m_dwInstSize = ( num_kaeruinst1 * sizeof( D3DINSTRUCTION ) 
		+ ( num_kaerustate1 * sizeof( D3DSTATE ) )
		+ ( num_kaeruprov1 * sizeof( D3DPROCESSVERTICES) )
		+ ( num_trikaeru1 * sizeof( D3DTRIANGLE ) ) );
	m_dwEBSize = m_dwVertSize + m_dwInstSize;
	
	//kaerubaseEB = (void*)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, m_dwEBSize );
	calcEB = (void*)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, m_dwEBSize );
	tempEB = (void*)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, m_dwEBSize );

	if( !calcEB || !tempEB ){
		TRACE( "FROGCPP : InitEBVars : HeapAlloc error !!!!!!!!!!!!\n" );
		return FALSE;
	}

	//memset( kaerubaseEB, 0, m_dwEBSize );
	memset( calcEB, 0, m_dwEBSize );
	memset( tempEB, 0, m_dwEBSize );

	return TRUE;
}

void CFrog::InitDeviceVars()
{
	lpD3DDev = NULL;
	lpD3DView = NULL;
}


void CFrog::RenderKaeru()
{


	
	//HRESULT	hres;

	if( invalidFrog ){
		CpTemp2Disp();

		InterlockedExchange( &invalidFrog, 0 );
	}
	
	lpD3DDev->Execute( lpIKaeruEB, lpD3DView, D3DEXECUTE_UNCLIPPED );
	//lpD3DDev->Execute( lpBKaeruEB, lpD3DView, D3DEXECUTE_UNCLIPPED );
	lpD3DDev->Execute( lpKaeruEB1, lpD3DView, D3DEXECUTE_UNCLIPPED );

}

BOOL CFrog::PreKaeruRender()
{

	if( m_Visual->dev != D3Z.lpD3DDevice ){
		SetUpKaeruRes();
		
		m_Visual->dev = D3Z.lpD3DDevice;

		RELEASE( lpD3DView );
		RELEASE( lpD3DDev );

		lpD3DDev = D3Z.lpD3DDevice;
		lpD3DView = D3Z.oldviewport;
		
		D3Z.lpD3DDevice->AddRef();// backgcpp には 入れてないが ここにないと InitDD::EndApp で Release したときに
		D3Z.oldviewport->AddRef();// バグるのは なぜ？？？  参照数が よれている ？？？？
								

// cmatrix

		proj_screen_cmatrix.SetD3DMatrix( gproj_screen_cmatrix );
		temp_cmatrix.SetD3DMatrix( gtemp_cmatrix );
		world_cmatrix.SetD3DMatrix( gworld_cmatrix );
		view_cmatrix.SetD3DMatrix( gview_cmatrix );

		BOOL	ret;

		ret = InitIniKaeruEB();
		if( !ret ){
			TRACE( "UVKAERUCPP : PreKaeruRender : InitIniKaeruEB error!!!!\n" );
			return FALSE;
		}
		ret = InitBldKaeruEB();
		if( !ret ){
			TRACE( "UVKAERUCPP : PreKaeruRender : InitBldKaeruEB error!!!!\n" );
			return FALSE;
		}
		ret = InitKaeruEB();
		if( !ret ){
			TRACE( "UVKAERUCPP : PreKaeruRender : InitKaeruEB error!!!!\n" );
			return FALSE;
		}


		KaeruMove( SCROLL_XZ );
		MultKaeru( MODE_NORMAL );
		
		return TRUE;
	}
	return FALSE;
}

void CFrog::DestroyKaeru()
{
	RELEASE( lpKaeruEB1 );
	RELEASE( lpIKaeruEB );

	if( m_Visual->htex ){
		m_Visual->texture->Release();
		m_Visual->htex = 0;
	}

	RELEASE(m_Visual->mat0);

	TRACE( "UVKAERUCPP : DESTROY : free Myvis\n" );

    memset(m_Visual, '0', sizeof(FrogVisual));
	//free( (FrogVisual*)m_Visual );
	HeapFree( s_hHeap, HEAP_NO_SERIALIZE, m_Visual );

	if( calcmat )
		HeapFree( s_hHeap, HEAP_NO_SERIALIZE, calcmat );


	TRACE( "UVKAERUCPP : DESTROY : release lpD3dView\n" );
	RELEASE( lpD3DView );
	TRACE( "UVKAERUCPP : DESTROY : release lpD3DDev\n" );
	RELEASE( lpD3DDev );



	if( calcEB ){
		HeapFree( s_hHeap, HEAP_NO_SERIALIZE, calcEB );
		calcEB = 0;
	}
	if( tempEB ){
		HeapFree( s_hHeap, HEAP_NO_SERIALIZE, tempEB );
		tempEB = 0;
	}

	if( m_p2mat ){
		delete m_p2mat;
		m_p2mat = 0;
	}

	if( m_armat ){
		delete m_armat;
		m_armat = 0;
	}

	if( m_arpause ){
		delete m_arpause;
		m_arpause = 0;
	}

}


void CFrog::SetUpKaeruRes()
{

    D3DMATERIAL mat0;
	int	findleng;
	char	texdir[50];
	char	filename[300];

	memset( texdir, 0, 50 );
	memset( filename, 0, 300 );

	findleng = GetEnvironmentVariable( "TEXDIR", texdir, 50 );
	if( !findleng ){
		TRACE( "FROGCPP : TEXDIR  directory var not found !!! error !!!\n" );
	}
	strcpy( filename, texdir );
	strcat( filename, "matkaeru.bmp" );

	OutputDebugString("setup vukaeru resources\n");

	//m_Visual->texture->Load(D3Z.lpD3DDevice,"kaerudata\\matkaeru.bmp");	
	m_Visual->texture->Load( D3Z.lpD3DDevice, filename );
	m_Visual->htex = m_Visual->texture->GetHandle();

	//m_Visual->texture1.Load( D3Z.lpD3DDevice, "rmfire3.bmp" );
	//m_Visual->htex1 = m_Visual->texture1.GetHandle();
	//m_Visual->htex1 = 0;

    if (D3Z.lpD3D->CreateMaterial( &m_Visual->mat0, NULL) != D3D_OK) {
		OutputDebugString("uvis: Failed to CreateMaterial\n");
    }

    if (D3Z.lpD3D->CreateMaterial( &calcmat, NULL) != D3D_OK) {
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
    mat0.ambient.r = (D3DVALUE)0.5;
    mat0.ambient.g = (D3DVALUE)0.5;
    mat0.ambient.b = (D3DVALUE)0.5;
    mat0.ambient.a = (D3DVALUE)1.0;
    mat0.specular.r = (D3DVALUE)0.0;
    mat0.specular.g = (D3DVALUE)0.0;
    mat0.specular.b = (D3DVALUE)0.0;
	mat0.specular.a = (D3DVALUE)0.0;
    mat0.power = (float)0.0;
    mat0.hTexture = m_Visual->htex;
    mat0.dwRampSize = 16;

	m_Visual->mat0->SetMaterial( &mat0 );
    m_Visual->mat0->GetHandle( D3Z.lpD3DDevice, &m_Visual->hmat0 );

}

BOOL CFrog::InitMyVisual()
{
	//m_Visual = (FrogVisual*)malloc(sizeof(FrogVisual));
	unsigned int	vissize = sizeof( FrogVisual );
	m_Visual = (FrogVisual*)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, vissize );
	if (!m_Visual){
		TRACE( "UVKAERUCPP : InitMyVisual : m_Visual alloc err\n" );
		return FALSE;
	}
	memset(m_Visual, 0, sizeof(FrogVisual));

	m_Visual->texture = new D3DTexture;


	m_Visual->dev = 0;

	m_Visual->hmat0 = 0;
	m_Visual->htex = 0;

	// for calc
	unsigned int hsize;
	hsize = sizeof( LPDIRECT3DMATERIAL );
	calcmat = (LPDIRECT3DMATERIAL)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, hsize );
	if( !calcmat ){
		TRACE( "UVKAERUCPP : InitMyVisual : calcmat alloc err\n" );
		return FALSE;
	}
	calchmat = 0;

	return TRUE;
}

//EDITPOINT!!!	InitKaeruVisual
/*** INIT KAERU VISUAL ***/
BOOL CFrog::InitKaeruVisual()
{
	BOOL	ret;
	ret = InitMyVisual();
	if( !ret ){
		TRACE( "FROG : InitMyVisual error!!\n" );
		return FALSE;
	}

	memset( tlvdatadir, 0, STRMAX );
	strcpy( tlvdatadir, "c:\\disp0\\tlvdata\\" );
	memset( motiondir, 0, STRMAX );
	strcpy( motiondir, "c:\\disp0\\motiondata\\" );
	
	sizeInstruction = sizeof( D3DINSTRUCTION );
	sizeState = sizeof( D3DSTATE );
	sizeProcessv = sizeof( D3DPROCESSVERTICES );
	sizeTlv = sizeof( D3DTLVERTEX );
	sizeTriangle = sizeof( D3DTRIANGLE );

	curdirindex = 1;

	clipstatus.dwFlags = D3DCLIPSTATUS_EXTENTS2;
	clipstatus.dwStatus = 0;
	clipstatus.minx = 300.0f;
	clipstatus.maxx = 500.0f;
	clipstatus.miny = 350.0f;
	clipstatus.maxy = 500.0f;
	clipstatus.minz = 0.0f;
	clipstatus.maxz = 0.0f;

	invalidFrog = 1;
	
	//m_curmove = NONE;

	memset( pkaerutlv, 0, sizeof( LPD3DTLVERTEX ) * (KTLVMAX + 1) );
	memset( ikaerutlv, 0, sizeof( int ) * (KTLVMAX + 1) );
	memset( ikaerutri, 0, sizeof( int ) * (KTLVMAX + 1) );

	coldiffuse.r = 200;
	coldiffuse.g = 200;
	coldiffuse.b = 200;

	colrate = (200 - 100) * 0.5f;

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

	charrect[0].sx = cenCTLALL.sx - width;
	charrect[0].sy = cenCTLALL.sy + height;
	charrect[0].sz = cenCTLALL.sz;
	charrect[1].sx = cenCTLALL.sx + width;
	charrect[1].sy = cenCTLALL.sy - height;
	charrect[1].sz = cenCTLALL.sz;

	testcharrect[1].sx = cenCTLALL.sx - width;
	testcharrect[1].sy = cenCTLALL.sy + height;
	testcharrect[1].sz = cenCTLALL.sz;

	testcharrect[0].sx = cenCTLALL.sx - width;
	testcharrect[0].sy = cenCTLALL.sy - height;
	testcharrect[0].sz = cenCTLALL.sz;

	testcharrect[3].sx = cenCTLALL.sx + width;
	testcharrect[3].sy = cenCTLALL.sy + height;
	testcharrect[3].sz = cenCTLALL.sz;

	testcharrect[2].sx = cenCTLALL.sx + width;
	testcharrect[2].sy = cenCTLALL.sy - height;
	testcharrect[2].sz = cenCTLALL.sz;


	ret = LoadKaeruVertex();
	if( !ret ){
		TRACE( "FROG : InitKaeruVisual : LoadKaeruVertex error!!!!!\n" );
		return FALSE;
	}

	ret = LoadKaeruMatrix();
	if( !ret ){
		TRACE( "FROG : InitKaeruVisual : LoadKaeruMatrix error!!!!!\n" );
		return FALSE;
	}

	return TRUE;
}

void	CFrog::CpCalc2Temp()
{
	// 頂点数が 変化するときは ExecuteBufferData も コピーする

	WaitForSingleObject( g_hMuteFlTemp0, INFINITE );

	memcpy( tempEB, calcEB, m_dwEBSize );


	ReleaseMutex( g_hMuteFlTemp0 );

}
void	CFrog::CpTemp2Disp()
{

	// 頂点数が 変化するときは ExecuteBufferData も コピーする
		// そのときは disp用 の EB は SetExecuteData も 必要になる
	LPD3DTLVERTEX	ebptr;


	WaitForSingleObject( g_hMuteFlTemp0, INFINITE );

	ebptr = (LPD3DTLVERTEX)kaeruExeBuffDesc1.lpData;
	memcpy( ebptr, tempEB, m_dwEBSize );

	ReleaseMutex( g_hMuteFlTemp0 );

}


DWORD	WINAPI	CFrog::CalcFrThread( LPVOID	lpThreadParam )
{
	static CFrog*	pfrog = (CFrog*)lpThreadParam;
	DWORD	dwwait;
	//int	curmove;

	TRACE( "CALCFRTHREAD RESUME\n" );

	while( g_lThreadFr0 ){
		dwwait = WaitForSingleObject( g_hEventFr0, INFINITE );

		pfrog->MultKaeru( MODE_NORMAL );


		ResetEvent( g_hEventFr0 );
	}

	TRACE( "FROGCPP : CalcFrThread : _endthreadex\n" );
	_endthreadex( 0 );

	return 0;
}


void CFrog::MultKaeru( int dispmode )
{
	int	curmove;

	switch( dispmode ){
	case MODE_NORMAL:
		SetKaeruClipStatus();
		//法線ベクトルは 回転していない

		WaitForSingleObject( g_hMuteKaerupos, INFINITE );
		curmove = kaerupos.Move; // 予期しないタイミングでの変更に 耐えるように コピー
		ReleaseMutex( g_hMuteKaerupos );

		//curcharparam = m_param[ m_curmove ]; // pis , fire で 参照する

		m_armat->SetMatrixFix30( curmove * CTLMAX, &trans_cmatrix );

		CpCalc2Temp();

		InterlockedExchange( &invalidFrog, 1 );

		break;
	default:
		break;
	}
}

BOOL CFrog::InitIniKaeruEB()
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
	//d3dExecuteBufferDesc.dwFlags = D3DDEB_BUFSIZE;
	d3dExecuteBufferDesc.dwFlags = D3DDEB_CAPS | D3DDEB_BUFSIZE;
	d3dExecuteBufferDesc.dwCaps = D3DDEBCAPS_VIDEOMEMORY;	
	d3dExecuteBufferDesc.dwBufferSize = dwExecuteBufferSize;

	hres = D3Z.lpD3DDevice->CreateExecuteBuffer( &d3dExecuteBufferDesc, &lpIKaeruEB, NULL );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitIniKaeruEB : CreateEB err\n" );
		return FALSE;
	}

	BOOL	ret;

	ret = FillIniKaeruEB();//mode, triangle set
	if( !ret ){
		return FALSE;
	}

	ZeroMemory( &ikaeruExecuteData, sizeof( ikaeruExecuteData ) );
	ikaeruExecuteData.dwSize = sizeof( ikaeruExecuteData );
	ikaeruExecuteData.dwVertexCount = 0;
	ikaeruExecuteData.dwInstructionOffset = 0;//
	ikaeruExecuteData.dwInstructionLength = dwInstructionSize;

	hres = lpIKaeruEB->SetExecuteData( &ikaeruExecuteData );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitIniKaeruEB : SetExecuteData err \n" );
		return FALSE;
	}

	return TRUE;

}

BOOL CFrog::FillIniKaeruEB()
{
	HRESULT	hres;


	void*	curptr;


	ZeroMemory( &ikaeruExeBuffDesc, sizeof( ikaeruExeBuffDesc ) );
	ikaeruExeBuffDesc.dwSize = sizeof( ikaeruExeBuffDesc );
	ikaeruExeBuffDesc.dwCaps = D3DDEBCAPS_VIDEOMEMORY;
	ikaeruExeBuffDesc.dwFlags = D3DDEB_CAPS | D3DDEB_LPDATA;
	hres = lpIKaeruEB->Lock( &ikaeruExeBuffDesc );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : FillIniGrandEB : EBLock err\n" );
		return FALSE;
	}

	curptr = (void*)ikaeruExeBuffDesc.lpData;


	// light, render state
	PUTD3DINSTRUCTION( D3DOP_STATELIGHT, sizeof(D3DSTATE), 1, curptr );
	STATE_DATA( D3DLIGHTSTATE_MATERIAL, m_Visual->hmat0, curptr );

	PUTD3DINSTRUCTION( D3DOP_STATERENDER, sizeof(D3DSTATE), 4, curptr );
	STATE_DATA( D3DRENDERSTATE_TEXTUREHANDLE, m_Visual->htex, curptr );
	//STATE_DATA( D3DRENDERSTATE_ZWRITEENABLE, FALSE, curptr );
	STATE_DATA( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE, curptr );

	STATE_DATA( D3DRENDERSTATE_ZWRITEENABLE, TRUE, curptr );
	//STATE_DATA( D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS, curptr );
	STATE_DATA( D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL, curptr );

	// exit
	OP_EXIT( curptr );
	lpIKaeruEB->Unlock();
	return TRUE;
}

BOOL CFrog::InitBldKaeruEB()
{
	HRESULT	hres;
	DWORD	dwVertexSize;
	DWORD	dwInstructionSize;
	DWORD	dwExecuteBufferSize;
	D3DEXECUTEBUFFERDESC	d3dExecuteBufferDesc;


	dwVertexSize = 0;
	dwInstructionSize = ( num_bld_kinst * sizeof( D3DINSTRUCTION ) ) 
		+ ( num_bld_kstate * sizeof( D3DSTATE ) );

	dwExecuteBufferSize = dwVertexSize + dwInstructionSize;
	ZeroMemory( &d3dExecuteBufferDesc, sizeof(d3dExecuteBufferDesc) );
	d3dExecuteBufferDesc.dwSize = sizeof( d3dExecuteBufferDesc );
	d3dExecuteBufferDesc.dwFlags = D3DDEB_CAPS | D3DDEB_BUFSIZE;
	d3dExecuteBufferDesc.dwCaps = D3DDEBCAPS_VIDEOMEMORY;	
	d3dExecuteBufferDesc.dwBufferSize = dwExecuteBufferSize;

	hres = D3Z.lpD3DDevice->CreateExecuteBuffer( &d3dExecuteBufferDesc, &lpBKaeruEB, NULL );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitBldKaeruEB : CreateEB err\n" );
		return FALSE;
	}

	BOOL	ret;
	ret = FillBldKaeruEB();//mode, triangle set
	if( !ret ){
		TRACE( "UVKAERUCPP : InitBldKaeruEB : FillBldkaeruEB error!!!!!!\n" );
		return FALSE;
	}

	ZeroMemory( &bkaeruExecuteData, sizeof( bkaeruExecuteData ) );
	bkaeruExecuteData.dwSize = sizeof( bkaeruExecuteData );
	bkaeruExecuteData.dwVertexCount = 0;
	bkaeruExecuteData.dwInstructionOffset = 0;//
	bkaeruExecuteData.dwInstructionLength = dwInstructionSize;

	hres = lpBKaeruEB->SetExecuteData( &bkaeruExecuteData );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitBldKaeruEB : SetExecuteData err \n" );
		return FALSE;
	}
	return TRUE;

}
BOOL CFrog::FillBldKaeruEB()
{
	HRESULT	hres;


	void*	curptr;


	ZeroMemory( &bkaeruExeBuffDesc, sizeof( bkaeruExeBuffDesc ) );
	bkaeruExeBuffDesc.dwSize = sizeof( bkaeruExeBuffDesc );
	bkaeruExeBuffDesc.dwCaps = D3DDEBCAPS_VIDEOMEMORY;
	bkaeruExeBuffDesc.dwFlags = D3DDEB_CAPS | D3DDEB_LPDATA;
	hres = lpBKaeruEB->Lock( &bkaeruExeBuffDesc );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : FillIniGrandEB : EBLock err\n" );
		return FALSE;
	}

	curptr = (void*)bkaeruExeBuffDesc.lpData;


	// light, render state
	PUTD3DINSTRUCTION( D3DOP_STATELIGHT, sizeof(D3DSTATE), 1, curptr );
	STATE_DATA( D3DLIGHTSTATE_MATERIAL, m_Visual->hmat0, curptr );

	PUTD3DINSTRUCTION( D3DOP_STATERENDER, sizeof(D3DSTATE), 4, curptr );
	STATE_DATA( D3DRENDERSTATE_TEXTUREHANDLE, m_Visual->htex, curptr );
	STATE_DATA( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE, curptr );

	STATE_DATA( D3DRENDERSTATE_ZWRITEENABLE, TRUE, curptr );
	STATE_DATA( D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL, curptr );

	// exit
	OP_EXIT( curptr );
	lpBKaeruEB->Unlock();

	return TRUE;
}



BOOL CFrog::InitKaeruEB()
{
	HRESULT	hres;
	//DWORD	vertsize;
	//DWORD	instleng;
	D3DEXECUTEBUFFERDESC	d3dExecuteBufferDesc;


	ZeroMemory( &d3dExecuteBufferDesc, sizeof(d3dExecuteBufferDesc) );
	d3dExecuteBufferDesc.dwSize = sizeof( d3dExecuteBufferDesc );
	d3dExecuteBufferDesc.dwFlags = D3DDEB_CAPS | D3DDEB_BUFSIZE;
	d3dExecuteBufferDesc.dwCaps = D3DDEBCAPS_VIDEOMEMORY;
	d3dExecuteBufferDesc.dwBufferSize = m_dwEBSize;

	hres = D3Z.lpD3DDevice->CreateExecuteBuffer( &d3dExecuteBufferDesc, &lpKaeruEB1, NULL );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitKaeruEB : CreateEB1 err\n" );
		return FALSE;
	}
		
	BOOL	ret;
	ret = FillKaeruEB();
	if( !ret ){
		TRACE( "UVKAERUCPP : InitKaeruEB : FillKaeruEB err\n" );
		return FALSE;
	}

///
	ZeroMemory( &kaeruExecuteData1, sizeof( kaeruExecuteData1 ) );
	kaeruExecuteData1.dwSize = sizeof( kaeruExecuteData1 );
	kaeruExecuteData1.dwVertexCount = num_tlvkaeru1;//!!!!!!!!!
	//vertsize  = ( num_tlvkaeru1 * sizeof( D3DTLVERTEX) );
	//kaeruExecuteData1.dwInstructionOffset = vertsize;//!!!!!!!
	kaeruExecuteData1.dwInstructionOffset = m_dwVertSize;
	//instleng = ( num_kaeruinst1 * sizeof( D3DINSTRUCTION ) 
	//	+ ( num_kaerustate1 * sizeof( D3DSTATE ) )
	//	+ ( num_kaeruprov1 * sizeof( D3DPROCESSVERTICES) )
	//	+ ( num_trikaeru1 * sizeof( D3DTRIANGLE ) ) );
	//kaeruExecuteData1.dwInstructionLength = instleng;//!!!!!!!
	kaeruExecuteData1.dwInstructionLength = m_dwInstSize;

////
	hres = lpKaeruEB1->SetExecuteData( &kaeruExecuteData1 );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : InitKaeruEB : SetExecuteData err \n" );
		return FALSE;
	}
	//TRACE( "UVKAERUCP : InitGrandEB : num_tlvkaeru1 %d num_trikaeru1 %d\n",
	//	num_tlvkaeru1, num_trikaeru1 );
	
	return TRUE;
}

BOOL CFrog::FillKaeruEB()
{
	HRESULT	hres;


	ZeroMemory( &kaeruExeBuffDesc1, sizeof( kaeruExeBuffDesc1 ) );
	kaeruExeBuffDesc1.dwSize = sizeof( kaeruExeBuffDesc1 );
	kaeruExeBuffDesc1.dwCaps = D3DDEBCAPS_VIDEOMEMORY;
	kaeruExeBuffDesc1.dwFlags = D3DDEB_CAPS | D3DDEB_LPDATA;
	hres = lpKaeruEB1->Lock( &kaeruExeBuffDesc1 );
	if( FAILED( hres ) ){
		TRACE( "UVKAERUCPP : FillGrandEB : EBLock err\n" );
		return FALSE;
	}
	
	InitPkaeruTlv();
	SetEBInst();

	lpKaeruEB1->Unlock();
	return TRUE;
}

//EDITPOINT!!!	InitPkaeruTlv
/*** TLV HEAD POSITION FOR EB ***/
void CFrog::InitPkaeruTlv()
{

	int	partsno;
	LPD3DTLVERTEX	lptlv;

	for( partsno = 0; partsno < KTLVMAX; partsno++ ){
		ikaerutlv[partsno] = 0;
		ikaerutri[partsno] = 0;
		pkaerutlv[partsno] = NULL;
	}


	lptlv = (LPD3DTLVERTEX)calcEB;

	ikaerutlv[ 0 ] = 0;
	ikaerutri[ 0 ] = 0;
	pkaerutlv[ 0 ] = lptlv;
	m_matindex[0] = CTLALL;
	m_ebtlvptr[0] = pkaerutlv[0];
	m_stlvptr[0] = tlvCTLALL[0];
	m_pnum[0] = PNUMCTLALL;
	m_vnum[0] = VNUMCTLALL;

	ikaerutlv[ 1 ] = ikaerutlv[ 0 ] + num_tlvctlall; // 直前までの総数  ！！！
	ikaerutri[ 1 ] = ikaerutri[ 0 ] + num_trictlall;
	pkaerutlv[ 1 ] = pkaerutlv[ 0 ] + ikaerutlv[ 1 ];
	m_matindex[1] = HEAD0;
	m_ebtlvptr[1] = pkaerutlv[1];
	m_stlvptr[1] = tlvHEAD0[0];
	m_pnum[1] = PNUMHEAD0;
	m_vnum[1] = VNUMHEAD0;

	ikaerutlv[ 2 ] = ikaerutlv[ 1 ] + num_tlvhead0; // 直前までの総数  ！！！
	ikaerutri[ 2 ] = ikaerutri[ 1 ] + num_trihead0;
	pkaerutlv[ 2 ] = pkaerutlv[ 0 ] + ikaerutlv[ 2 ];
	m_matindex[2] = HEAD00;
	m_ebtlvptr[2] = pkaerutlv[2];
	m_stlvptr[2] = tlvHEAD00;
	m_pnum[2] = 1;//
	m_vnum[2] = VNUMHEAD00;

	ikaerutlv[ 4 ] = ikaerutlv[ 2 ] + num_tlvhead00; // 直前までの総数  ！！！
	ikaerutri[ 4 ] = ikaerutri[ 2 ] + num_trihead00;
	pkaerutlv[ 4 ] = pkaerutlv[ 0 ] + ikaerutlv[ 4 ];
	m_matindex[3] = HEAD02;
	m_ebtlvptr[3] = pkaerutlv[4];
	m_stlvptr[3] = tlvHEAD02;
	m_pnum[3] = 1;//
	m_vnum[3] = VNUMHEAD02;

	ikaerutlv[ 5 ] = ikaerutlv[ 4 ] + num_tlvhead02; // 直前までの総数  ！！！
	ikaerutri[ 5 ] = ikaerutri[ 4 ] + num_trihead02;
	pkaerutlv[ 5 ] = pkaerutlv[ 0 ] + ikaerutlv[ 5 ];
	m_matindex[4] = ARML0;
	m_ebtlvptr[4] = pkaerutlv[5];
	m_stlvptr[4] = tlvARML0[0];
	m_pnum[4] = PNUMARML0;
	m_vnum[4] = VNUMARML0;

	ikaerutlv[ 6 ] = ikaerutlv[ 5 ] + num_tlvarml0; // 直前までの総数  ！！！
	ikaerutri[ 6 ] = ikaerutri[ 5 ] + num_triarml0;
	pkaerutlv[ 6 ] = pkaerutlv[ 0 ] + ikaerutlv[ 6 ];
	m_matindex[5] = ARML1;
	m_ebtlvptr[5] = pkaerutlv[6];
	m_stlvptr[5] = tlvARML1[0];
	m_pnum[5] = PNUMARML1;
	m_vnum[5] = VNUMARML1;

	ikaerutlv[ 7 ] = ikaerutlv[ 6 ] + num_tlvarml1; // 直前までの総数  ！！！
	ikaerutri[ 7 ] = ikaerutri[ 6 ] + num_triarml1;
	pkaerutlv[ 7 ] = pkaerutlv[ 0 ] + ikaerutlv[ 7 ];
	m_matindex[6] = ARML2;
	m_ebtlvptr[6] = pkaerutlv[7];
	m_stlvptr[6] = tlvARML2;
	m_pnum[6] = 1;//
	m_vnum[6] = VNUMARML2;

	ikaerutlv[ 11 ] = ikaerutlv[ 7 ] + num_tlvarml2; // 直前までの総数  ！！！
	ikaerutri[ 11 ] = ikaerutri[ 7 ] + num_triarml2;
	pkaerutlv[ 11 ] = pkaerutlv[ 0 ] + ikaerutlv[ 11 ];
	m_matindex[7] = LEGL0;
	m_ebtlvptr[7] = pkaerutlv[11];
	m_stlvptr[7] = tlvLEGL0[0];
	m_pnum[7] = PNUMLEGL0;
	m_vnum[7] = VNUMLEGL0;

	ikaerutlv[ 12 ] = ikaerutlv[ 11 ] + num_tlvlegl0; // 直前までの総数  ！！！
	ikaerutri[ 12 ] = ikaerutri[ 11 ] + num_trilegl0;
	pkaerutlv[ 12 ] = pkaerutlv[ 0 ] + ikaerutlv[ 12 ];
	m_matindex[8] = LEGL1;
	m_ebtlvptr[8] = pkaerutlv[12];
	m_stlvptr[8] = tlvLEGL1[0];
	m_pnum[8] = PNUMLEGL1;
	m_vnum[8] = VNUMLEGL1;

	ikaerutlv[ 13 ] = ikaerutlv[ 12 ] + num_tlvlegl1; // 直前までの総数  ！！！
	ikaerutri[ 13 ] = ikaerutri[ 12 ] + num_trilegl1;
	pkaerutlv[ 13 ] = pkaerutlv[ 0 ] + ikaerutlv[ 13 ];
	m_matindex[9] = LEGL2;
	m_ebtlvptr[9] = pkaerutlv[13];
	m_stlvptr[9] = tlvLEGL2;
	m_pnum[9] = 1;//
	m_vnum[9] = VNUMLEGL2;

	ikaerutlv[ 19 ] = ikaerutlv[ 13 ] + num_tlvlegl2; // 直前までの総数  ！！！
	ikaerutri[ 19 ] = ikaerutri[ 13 ] + num_trilegl2;
	pkaerutlv[ 19 ] = pkaerutlv[ 0 ] + ikaerutlv[ 19 ];
	m_matindex[10] = CTLALL;
	m_ebtlvptr[10] = pkaerutlv[19];
	m_stlvptr[10] = tlvBody1[0];
	m_pnum[10] = PNUMBODY1;
	m_vnum[10] = VNUMBODY1;

	ikaerutlv[ 20 ] = ikaerutlv[ 19 ] + num_tlvbody1; // 直前までの総数  ！！！
	ikaerutri[ 20 ] = ikaerutri[ 19 ] + num_tribody1;
	pkaerutlv[ 20 ] = pkaerutlv[ 0 ] + ikaerutlv[ 20 ];
	m_matindex[11] = HEAD0;
	m_ebtlvptr[11] = pkaerutlv[20];
	m_stlvptr[11] = tlvKtlvheadl[0];
	m_pnum[11] = PNUMKTLVHEADL;
	m_vnum[11] = VNUMKTLVHEADL;

	ikaerutlv[ 22 ] = ikaerutlv[ 20 ] + num_tlvktlvheadl; // 直前までの総数  ！！！
	ikaerutri[ 22 ] = ikaerutri[ 20 ] + num_triktlvheadl;
	pkaerutlv[ 22 ] = pkaerutlv[ 0 ] + ikaerutlv[ 22 ];
	m_matindex[12] = HEAD0;
	m_ebtlvptr[12] = pkaerutlv[22];
	m_stlvptr[12] = tlvKtlveyel;
	m_pnum[12] = 1;//
	m_vnum[12] = VNUMKTLVEYEL;

	ikaerutlv[ 3 ] = ikaerutlv[ 22 ] + num_tlvktlveyel; // 直前までの総数  ！！！
	ikaerutri[ 3 ] = ikaerutri[ 22 ] + num_triktlveyel;
	pkaerutlv[ 3 ] = pkaerutlv[ 0 ] + ikaerutlv[ 3 ];
	m_matindex[13] = HEAD01;
	m_ebtlvptr[13] = pkaerutlv[3];
	m_stlvptr[13] = tlvHEAD01;
	m_pnum[13] = 1;//
	m_vnum[13] = VNUMHEAD01;

	ikaerutlv[ 8 ] = ikaerutlv[ 3 ] + num_tlvhead01; // 直前までの総数  ！！！
	ikaerutri[ 8 ] = ikaerutri[ 3 ] + num_trihead01;
	pkaerutlv[ 8 ] = pkaerutlv[ 0 ] + ikaerutlv[ 8 ];
	m_matindex[14] = ARMR0;
	m_ebtlvptr[14] = pkaerutlv[8];
	m_stlvptr[14] = tlvARMR0[0];
	m_pnum[14] = PNUMARMR0;
	m_vnum[14] = VNUMARMR0;

	ikaerutlv[ 9 ] = ikaerutlv[ 8 ] + num_tlvarmr0; // 直前までの総数  ！！！
	ikaerutri[ 9 ] = ikaerutri[ 8 ] + num_triarmr0;
	pkaerutlv[ 9 ] = pkaerutlv[ 0 ] + ikaerutlv[ 9 ];
	m_matindex[15] = ARMR1;
	m_ebtlvptr[15] = pkaerutlv[9];
	m_stlvptr[15] = tlvARMR1[0];
	m_pnum[15] = PNUMARMR1;
	m_vnum[15] = VNUMARMR1;

	ikaerutlv[ 10 ] = ikaerutlv[ 9 ] + num_tlvarmr1; // 直前までの総数  ！！！
	ikaerutri[ 10 ] = ikaerutri[ 9 ] + num_triarmr1;
	pkaerutlv[ 10 ] = pkaerutlv[ 0 ] + ikaerutlv[ 10 ];
	m_matindex[16] = ARMR2;
	m_ebtlvptr[16] = pkaerutlv[10];
	m_stlvptr[16] = tlvARMR2;
	m_pnum[16] = 1;//
	m_vnum[16] = VNUMARMR2;

	ikaerutlv[ 14 ] = ikaerutlv[ 10 ] + num_tlvarmr2; // 直前までの総数  ！！！
	ikaerutri[ 14 ] = ikaerutri[ 10 ] + num_triarmr2;
	pkaerutlv[ 14 ] = pkaerutlv[ 0 ] + ikaerutlv[ 14 ];
	m_matindex[17] = LEGR0;
	m_ebtlvptr[17] = pkaerutlv[14];
	m_stlvptr[17] = tlvLEGR0[0];
	m_pnum[17] = PNUMLEGR0;
	m_vnum[17] = VNUMLEGR0;

	ikaerutlv[ 15 ] = ikaerutlv[ 14 ] + num_tlvlegr0; // 直前までの総数  ！！！
	ikaerutri[ 15 ] = ikaerutri[ 14 ] + num_trilegr0;
	pkaerutlv[ 15 ] = pkaerutlv[ 0 ] + ikaerutlv[ 15 ];
	m_matindex[18] = LEGR1;
	m_ebtlvptr[18] = pkaerutlv[15];
	m_stlvptr[18] = tlvLEGR1[0];
	m_pnum[18] = PNUMLEGR1;
	m_vnum[18] = VNUMLEGR1;

	ikaerutlv[ 16 ] = ikaerutlv[ 15 ] + num_tlvlegr1; // 直前までの総数  ！！！
	ikaerutri[ 16 ] = ikaerutri[ 15 ] + num_trilegr1;
	pkaerutlv[ 16 ] = pkaerutlv[ 0 ] + ikaerutlv[ 16 ];
	m_matindex[19] = LEGR2;
	m_ebtlvptr[19] = pkaerutlv[16];
	m_stlvptr[19] = tlvLEGR2;
	m_pnum[19] = 1;//
	m_vnum[19] = VNUMLEGR2;

	ikaerutlv[ 21 ] = ikaerutlv[ 16 ] + num_tlvlegr2; // 直前までの総数  ！！！
	ikaerutri[ 21 ] = ikaerutri[ 16 ] + num_trilegr2;
	pkaerutlv[ 21 ] = pkaerutlv[ 0 ] + ikaerutlv[ 21 ];
	m_matindex[20] = HEAD0;
	m_ebtlvptr[20] = pkaerutlv[21];
	m_stlvptr[20] = tlvKtlvheadr[0];
	m_pnum[20] = PNUMKTLVHEADR;
	m_vnum[20] = VNUMKTLVHEADR;

	ikaerutlv[ 23 ] = ikaerutlv[ 21 ] + num_tlvktlvheadr; // 直前までの総数  ！！！
	ikaerutri[ 23 ] = ikaerutri[ 21 ] + num_triktlvheadr;
	pkaerutlv[ 23 ] = pkaerutlv[ 0 ] + ikaerutlv[ 23 ];
	m_matindex[21] = HEAD0;
	m_ebtlvptr[21] = pkaerutlv[23];
	m_stlvptr[21] = tlvKtlveyer;
	m_pnum[21] = 1;//
	m_vnum[21] = VNUMKTLVEYER;

	m_ctrlnum = 22; ///////////////////////////!!!!!!!!!! 
	

	m_armat->SetMultVars( m_matindex, m_ebtlvptr, m_stlvptr, m_pnum, m_vnum, m_ctrlnum );


	/*** JOINT TLV INDEX ***/

	JindexARML1 = ikaerutlv[ 5 ] + ( PNUMARML0 - 1 ) * VNUMARML1;
	JindexLEGL1 = ikaerutlv[ 11 ] + ( PNUMLEGL0 - 1 ) * VNUMLEGL1;
	JindexARMR1 = ikaerutlv[ 8 ] + ( PNUMARMR0 - 1 ) * VNUMARMR1;
	JindexLEGR1 = ikaerutlv[ 14 ] + ( PNUMLEGR0 - 1 ) * VNUMLEGR1;
}



//EDITPOINT!!!	SetEBInst
/*** SET KAERU EB INSTRUCTION ***/
void CFrog::SetEBInst()
{

	LPD3DTLVERTEX	lptlv;
	LPD3DINSTRUCTION	lpinst;

	LPD3DINSTRUCTION	lpInstruction;
	LPD3DTRIANGLE		lpTriangle;
	LPD3DPROCESSVERTICES	lpProcessV;

	LPD3DTLVERTEX	lplasttlv;
	LPD3DTRIANGLE	lplasttri;

	//lptlv = (LPD3DTLVERTEX)kaeruExeBuffDesc1.lpData;
	lptlv = pkaerutlv[0];
	lplasttlv = lptlv + num_tlvkaeru1;
	
	lpinst = (LPD3DINSTRUCTION)( lplasttlv );

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

	DWORD	tlvsize = sizeof( D3DTLVERTEX );

	// eb に　tex　UV(　multvert でも不変　) と　color を　セットするために tlv を　コピー　
	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[0], PNUMCTLALL, VNUMCTLALL, ikaerutlv[ 0 ] );
	memcpy(  pkaerutlv[ 0 ], tlvCTLALL[0], num_tlvctlall * tlvsize);

	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[1], PNUMHEAD0, VNUMHEAD0, ikaerutlv[ 1 ] );
	memcpy( pkaerutlv[ 1 ], tlvHEAD0[0], num_tlvhead0 * tlvsize );

	tra_cmatrix.SetTridataFan( lpTriangle + ikaerutri[2], PNUMHEAD00, VNUMHEAD00, ikaerutlv[ 2 ], MemNumHEAD00 );
	memcpy( pkaerutlv[ 2 ], tlvHEAD00, num_tlvhead00 * tlvsize );

	tra_cmatrix.SetTridataFan( lpTriangle + ikaerutri[4], PNUMHEAD02, VNUMHEAD02, ikaerutlv[ 4 ], MemNumHEAD02 );
	memcpy( pkaerutlv[ 4 ], tlvHEAD02, num_tlvhead02 * tlvsize );

	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[5], PNUMARML0, VNUMARML0, ikaerutlv[ 5 ] );
	memcpy( pkaerutlv[ 5 ], tlvARML0[0], num_tlvarml0 * tlvsize );

	tra_cmatrix.SetTriYokoJ( lpTriangle + ikaerutri[6], JindexARML1, PNUMARML1, VNUMARML1, ikaerutlv[ 6 ] );
	memcpy( pkaerutlv[ 6 ], tlvARML1[0], num_tlvarml1 * tlvsize );

	tra_cmatrix.SetTridataBFan( lpTriangle + ikaerutri[7], PNUMARML2, VNUMARML2, ikaerutlv[ 7 ], MemNumARML2 );
	memcpy( pkaerutlv[ 7 ], tlvARML2, num_tlvarml2 * tlvsize );

	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[11], PNUMLEGL0, VNUMLEGL0, ikaerutlv[ 11 ] );
	memcpy( pkaerutlv[ 11 ], tlvLEGL0[0], num_tlvlegl0 * tlvsize );

	tra_cmatrix.SetTriYokoJ( lpTriangle + ikaerutri[12], JindexLEGL1, PNUMLEGL1, VNUMLEGL1, ikaerutlv[ 12 ] );
	memcpy( pkaerutlv[ 12 ], tlvLEGL1[0], num_tlvlegl1 * tlvsize );

	tra_cmatrix.SetTridataBFan( lpTriangle + ikaerutri[13], PNUMLEGL2, VNUMLEGL2, ikaerutlv[ 13 ], MemNumLEGL2 );
	memcpy( pkaerutlv[ 13 ], tlvLEGL2, num_tlvlegl2 * tlvsize );

	tra_cmatrix.SetTridataYLine( lpTriangle + ikaerutri[19], PNUMBODY1, VNUMBODY1, ikaerutlv[ 19 ] );
	memcpy(  pkaerutlv[ 19 ], tlvBody1[0], num_tlvbody1 * tlvsize );

	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[20], PNUMKTLVHEADL, VNUMKTLVHEADL, ikaerutlv[ 20 ] );
	memcpy( pkaerutlv[ 20 ], tlvKtlvheadl[0], num_tlvktlvheadl * tlvsize );

	tra_cmatrix.SetTridataFan( lpTriangle + ikaerutri[22], PNUMKTLVEYEL, VNUMKTLVEYEL, ikaerutlv[ 22 ], MemNumKtlveyel );
	memcpy( pkaerutlv[ 22 ], tlvKtlveyel, num_tlvktlveyel * tlvsize );

	tra_cmatrix.SetTridataFan( lpTriangle + ikaerutri[3], PNUMHEAD01, VNUMHEAD01, ikaerutlv[ 3 ], MemNumHEAD01 );
	memcpy( pkaerutlv[ 3 ], tlvHEAD01, num_tlvhead01 * tlvsize );

	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[8], PNUMARMR0, VNUMARMR0, ikaerutlv[ 8 ] );
	memcpy( pkaerutlv[ 8 ], tlvARMR0[0], num_tlvarmr0 * tlvsize );

	tra_cmatrix.SetTriYokoJ( lpTriangle + ikaerutri[9], JindexARMR1, PNUMARMR1, VNUMARMR1, ikaerutlv[ 9 ] );
	memcpy( pkaerutlv[ 9 ], tlvARMR1[0], num_tlvarmr1 * tlvsize );

	tra_cmatrix.SetTridataBFan( lpTriangle + ikaerutri[10], PNUMARMR2, VNUMARMR2, ikaerutlv[ 10 ], MemNumARMR2 );
	memcpy( pkaerutlv[ 10 ], tlvARMR2, num_tlvarmr2 * tlvsize );

	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[14], PNUMLEGR0, VNUMLEGR0, ikaerutlv[ 14 ] );
	memcpy( pkaerutlv[ 14 ], tlvLEGR0[0], num_tlvlegr0 * tlvsize );

	tra_cmatrix.SetTriYokoJ( lpTriangle + ikaerutri[15], JindexLEGR1, PNUMLEGR1, VNUMLEGR1, ikaerutlv[ 15 ] );
	memcpy( pkaerutlv[ 15 ], tlvLEGR1[0], num_tlvlegr1 * tlvsize );

	tra_cmatrix.SetTridataBFan( lpTriangle + ikaerutri[16], PNUMLEGR2, VNUMLEGR2, ikaerutlv[ 16 ], MemNumLEGR2 );
	memcpy( pkaerutlv[ 16 ], tlvLEGR2, num_tlvlegr2 * tlvsize );

	tra_cmatrix.SetTridataYoko( lpTriangle + ikaerutri[21], PNUMKTLVHEADR, VNUMKTLVHEADR, ikaerutlv[ 21 ] );
	memcpy( pkaerutlv[ 21 ], tlvKtlvheadr[0], num_tlvktlvheadr * tlvsize );

	tra_cmatrix.SetTridataFan( lpTriangle + ikaerutri[23], PNUMKTLVEYER, VNUMKTLVEYER, ikaerutlv[ 23 ], MemNumKtlveyer );
	memcpy( pkaerutlv[ 23 ], tlvKtlveyer, num_tlvktlveyer * tlvsize );



	lplasttri = lpTriangle + num_trikaeru1;

	lpInstruction = (LPD3DINSTRUCTION)( lplasttri );
	lpInstruction->bOpcode = D3DOP_EXIT;
	lpInstruction->bSize = 0;
	lpInstruction->wCount = 0U;

}



//EDITPOINT!!!	InitKaeruVertex
/*** KAERU INIT VERTEX ***/
BOOL CFrog::LoadKaeruVertex()
{
	BOOL	retCTLALL, retHEAD0, retHEAD00, retHEAD02;
	BOOL	retARML0, retARML1, retARML2;
	BOOL	retLEGL0, retLEGL1, retLEGL2;
	BOOL	retBody1, retKtlvheadl, retKtlveyel, retHEAD01;
	BOOL	retARMR0, retARMR1, retARMR2;
	BOOL	retLEGR0, retLEGR1, retLEGR2;
	BOOL	retKtlvheadr, retKtlveyer;
	BOOL	retALL;

	char	prename[500];
	int		findleng;
	char	tlvdir[50];

	memset( tlvdir, 0, 50 );
	memset( prename, 0, 500 );
	findleng = GetEnvironmentVariable( "TLVDATA", tlvdir, 50 );
	if( !findleng ){
		TRACE( " tlvdir  directory var not found !!! error !!!\n" );
		return FALSE;
	}
	strcpy( prename, tlvdir );
	strcat( prename, "kaeru" ); // prename : directory\\kaeru
	

	retCTLALL = LoadTlvCTLALL( prename );
	retHEAD0 = LoadTlvHEAD0( prename );
	retHEAD00 = LoadTlvHEAD00( prename );
	retHEAD02 = LoadTlvHEAD02( prename );
	retARML0 = LoadTlvARML0( prename );
	retARML1 = LoadTlvARML1( prename );
	retARML2 = LoadTlvARML2( prename );
	retLEGL0 = LoadTlvLEGL0( prename );
	retLEGL1 = LoadTlvLEGL1( prename );
	retLEGL2 = LoadTlvLEGL2( prename );
	retBody1 = LoadTlvBody1( prename );
	retKtlvheadl = LoadTlvKtlvheadl( prename );
	retKtlveyel = LoadTlvKtlveyel( prename );
	retHEAD01 = LoadTlvHEAD01( prename );
	retARMR0 = LoadTlvARMR0( prename );
	retARMR1 = LoadTlvARMR1( prename );
	retARMR2 = LoadTlvARMR2( prename );
	retLEGR0 = LoadTlvLEGR0( prename );
	retLEGR1 = LoadTlvLEGR1( prename );
	retLEGR2 = LoadTlvLEGR2( prename );
	retKtlvheadr = LoadTlvKtlvheadr( prename );
	retKtlveyer = LoadTlvKtlveyer( prename );

/** total num for eb **/
	num_tlvkaeru1 = num_tlvctlall + num_tlvhead0 + num_tlvhead00 + num_tlvhead02 + num_tlvarml0 + num_tlvarml1 + num_tlvarml2 + num_tlvlegl0 + num_tlvlegl1 + num_tlvlegl2 + num_tlvbody1 + num_tlvktlvheadl + num_tlvktlveyel + num_tlvhead01 + num_tlvarmr0 + num_tlvarmr1 + num_tlvarmr2 + num_tlvlegr0 + num_tlvlegr1 + num_tlvlegr2 + num_tlvktlvheadr + num_tlvktlveyer;

	num_trikaeru1 = num_trictlall + num_trihead0 + num_trihead00 + num_trihead02 + num_triarml0 + num_triarml1 + num_triarml2 + num_trilegl0 + num_trilegl1 + num_trilegl2 + num_tribody1 + num_triktlvheadl + num_triktlveyel + num_trihead01 + num_triarmr0 + num_triarmr1 + num_triarmr2 + num_trilegr0 + num_trilegr1 + num_trilegr2 + num_triktlvheadr + num_triktlveyer;

	retALL = 
		retCTLALL && retHEAD0 && retHEAD00 && retHEAD02 &&
		retARML0 && retARML1 && retARML2 &&
		retLEGL0 && retLEGL1 && retLEGL2 &&
		retBody1 && retKtlvheadl && retKtlveyel && retHEAD01 &&
		retARMR0 && retARMR1 && retARMR2 &&
		retLEGR0 && retLEGR1 && retLEGR2 &&
		retKtlvheadr && retKtlveyer;

	return retALL;

}


BOOL	CFrog::LoadCharTlvFile( char* filename,
	int dim, int planenum, int pointnum,
	int*	tlvnum, int* trinum, D3DTLVERTEX* cenptr, int* memnumptr, 
	D3DTLVERTEX*	dataptr )
{
	HANDLE hfile;
	CHARDATAHEADER	cheader;
	static DWORD	headersize = sizeof( CHARDATAHEADER );
	DWORD	datasize, readsize;

	// open file
	hfile = CreateFile( filename, GENERIC_READ,
		0, NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		TRACE( "FROG : LoadCharTlvFile : open error !!!!! %s\n", filename );
		return FALSE;
	}

	// read header
	ReadFile( hfile, &cheader, headersize, &readsize, NULL );
	if( readsize != headersize ){
		TRACE( "FROG : LoadCharTlvFile : header read error !!!! %s\n", filename );
		CloseHandle( hfile );
		return FALSE;
	}

	// check header
	if( strncmp( cheader.filetype, "CHARDATA", 8 ) != 0 ){
		TRACE( "FROG : LoadCharTlvFile : filetype error !!!! %s\n", filename );
		CloseHandle( hfile );
		return FALSE;
	}
	if( cheader.tlvdim != dim || cheader.tlvwidth != pointnum || cheader.tlvheight != planenum ){
		TRACE( "FROG : LoadCharTlvFile : header param error !!! %s\n", filename );
		CloseHandle( hfile );
		return FALSE;
	}

	// setparam
	*tlvnum = cheader.tlvnum; *trinum = cheader.trinum;
	*cenptr = cheader.centervert;
	if( dim == 1 ){ // fan data
		if( memnumptr ){
			memcpy( memnumptr, cheader.memnum, sizeof( int ) * cheader.tlvheight );
		}else{
			TRACE( "FROG : LoadCharTlvFile : set memnum : memnumptr NULL error !! %s\n", filename );
			return FALSE;
		}
	}


	// read data
	datasize = sizeof( D3DTLVERTEX ) * cheader.tlvnum;
	ReadFile( hfile, dataptr, datasize, &readsize, NULL );
	if( readsize != datasize ){
		TRACE( "FROG : LoadCharTlvFile : data read error !!!!! %s\n", filename );
		CloseHandle( hfile );
		return FALSE;
	}

	// close
	CloseHandle( hfile );
	return TRUE;
	
}


//EDITPOINT!!!	INIT PARTS TLV
/*** INIT VERTEX ***/
BOOL CFrog::LoadTlvCTLALL( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "CTLALL.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMCTLALL, VNUMCTLALL, 
		&num_tlvctlall, &num_trictlall, &cenCTLALL, 0, 
		tlvCTLALL[0]
		);

	return ret;
}

BOOL CFrog::LoadTlvHEAD0( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "HEAD0.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMHEAD0, VNUMHEAD0, 
		&num_tlvhead0, &num_trihead0, &cenHEAD0, 0, 
		tlvHEAD0[0]
		);

	return ret;

}

BOOL CFrog::LoadTlvHEAD00( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "HEAD00.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMHEAD00, 0, 
		&num_tlvhead00, &num_trihead00, &cenHEAD00, MemNumHEAD00,
		tlvHEAD00
		);


	return ret;
}

BOOL CFrog::LoadTlvHEAD02( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "HEAD02.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMHEAD02, 0, 
		&num_tlvhead02, &num_trihead02, &cenHEAD02, MemNumHEAD02, 
		tlvHEAD02
		);

	return ret;

}

BOOL CFrog::LoadTlvARML0( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "ARML0.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMARML0, VNUMARML0, 
		&num_tlvarml0, &num_triarml0, &cenARML0, 0, 
		tlvARML0[0]
		);

	return ret;

}

BOOL CFrog::LoadTlvARML1( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "ARML1.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMARML1, VNUMARML1, 
		&num_tlvarml1, &num_triarml1, &cenARML1, 0, 
		tlvARML1[0]
		);

	return ret;

	
}

BOOL CFrog::LoadTlvARML2( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "ARML2.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMARML2, 0,
		&num_tlvarml2, &num_triarml2, &cenARML2, MemNumARML2, 
		tlvARML2
		);

	return ret;
}

BOOL CFrog::LoadTlvLEGL0( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "LEGL0.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMLEGL0, VNUMLEGL0, 
		&num_tlvlegl0, &num_trilegl0, &cenLEGL0, 0,
		tlvLEGL0[0]
		);

	return ret;

}

BOOL CFrog::LoadTlvLEGL1( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "LEGL1.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMLEGL1, VNUMLEGL1, 
		&num_tlvlegl1, &num_trilegl1, &cenLEGL1, 0,
		tlvLEGL1[0]
		);

	return ret;


}

BOOL CFrog::LoadTlvLEGL2( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "LEGL2.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMLEGL2, 0,
		&num_tlvlegl2, &num_trilegl2, &cenLEGL2, MemNumLEGL2, 
		tlvLEGL2
		);

	return ret;
}

BOOL CFrog::LoadTlvBody1( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "Body1.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMBODY1, VNUMBODY1, 
		&num_tlvbody1, &num_tribody1, &cenBody1, 0,
		tlvBody1[0]
		);

	return ret;

}

BOOL CFrog::LoadTlvKtlvheadl( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "Ktlvheadl.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMKTLVHEADL, VNUMKTLVHEADL, 
		&num_tlvktlvheadl, &num_triktlvheadl, &cenKtlvheadl, 0,
		tlvKtlvheadl[0]
		);

	return ret;

}

BOOL CFrog::LoadTlvKtlveyel( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "Ktlveyel.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMKTLVEYEL, 0,
		&num_tlvktlveyel, &num_triktlveyel, &cenKtlveyel, MemNumKtlveyel,
		tlvKtlveyel
		);

	return ret;

}

BOOL CFrog::LoadTlvHEAD01( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "HEAD01.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMHEAD01, 0, 
		&num_tlvhead01, &num_trihead01, &cenHEAD01, MemNumHEAD01, 
		tlvHEAD01
		);

	return ret;

}

BOOL CFrog::LoadTlvARMR0( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "ARMR0.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMARMR0, VNUMARMR0, 
		&num_tlvarmr0, &num_triarmr0, &cenARMR0, 0,
		tlvARMR0[0]
		);

	return ret;


}

BOOL CFrog::LoadTlvARMR1( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "ARMR1.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMARMR1, VNUMARMR1, 
		&num_tlvarmr1, &num_triarmr1, &cenARMR1, 0,
		tlvARMR1[0]
		);

	return ret;
	

}

BOOL CFrog::LoadTlvARMR2( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "ARMR2.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMARMR2, 0,
		&num_tlvarmr2, &num_triarmr2, &cenARMR2, MemNumARMR2, 
		tlvARMR2
		);

	return ret;
	
}

BOOL CFrog::LoadTlvLEGR0( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "LEGR0.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMLEGR0, VNUMLEGR0, 
		&num_tlvlegr0, &num_trilegr0, &cenLEGR0, 0,
		tlvLEGR0[0]
		);

	return ret;

}

BOOL CFrog::LoadTlvLEGR1( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "LEGR1.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMLEGR1, VNUMLEGR1, 
		&num_tlvlegr1, &num_trilegr1, &cenLEGR1, 0,
		tlvLEGR1[0]
		);

	return ret;

}

BOOL CFrog::LoadTlvLEGR2( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "LEGR2.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMLEGR2, 0,
		&num_tlvlegr2, &num_trilegr2, &cenLEGR2, MemNumLEGR2,
		tlvLEGR2
		);

	return ret;

}

BOOL CFrog::LoadTlvKtlvheadr( char* prefilename )
{
	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "Ktlvheadr.tlv" );

	ret = LoadCharTlvFile( filename, 
		2, PNUMKTLVHEADR, VNUMKTLVHEADR, 
		&num_tlvktlvheadr, &num_triktlvheadr, &cenKtlvheadr, 0,
		tlvKtlvheadr[0]
		);

	return ret;


}

BOOL CFrog::LoadTlvKtlveyer( char* prefilename )
{

	char filename[500];
	BOOL	ret;

	memset( filename, 0, 500 );
	strcpy( filename, prefilename );
	strcat( filename, "Ktlveyer.tlv" );

	ret = LoadCharTlvFile( filename, 
		1, PNUMKTLVEYER, 0, 
		&num_tlvktlveyer, &num_triktlveyer, &cenKtlveyer, MemNumKtlveyer, 
		tlvKtlveyer
		);

	return ret;
}




void CFrog::InitMultVars()
{
	memset( m_matindex, 0, sizeof( int ) * CTLMAX );
	memset( m_ebtlvptr, 0, sizeof( D3DTLVERTEX* ) * CTLMAX );
	memset( m_stlvptr, 0, sizeof( D3DTLVERTEX* ) * CTLMAX );
	memset( m_pnum, 0, sizeof( int ) * CTLMAX );
	memset( m_vnum, 0, sizeof( int ) * CTLMAX );
	m_ctrlnum = 0;
}



BOOL CFrog::LoadKaeruMatrix()
{
	m_p2mat->SetCenterVert( 
			cenCTLALL, 
			cenHEAD0, 
			cenHEAD00, 
			cenHEAD02, 
			cenARML0, 
			cenARML1, 
			cenARML2, 
			cenLEGL0, 
			cenLEGL1, 
			cenLEGL2, 
			cenBody1, 
			cenKtlvheadl, 
			cenKtlveyel, 
			cenHEAD01, 
			cenARMR0, 
			cenARMR1, 
			cenARMR2, 
			cenLEGR0, 
			cenLEGR1, 
			cenLEGR2, 
			cenKtlvheadr, 
			cenKtlveyer
	);

	// matrix の　load
	char	tlvdir[50];
	char	filename[500];
	int		findleng;
	HANDLE	hfile;

	memset( tlvdir, 0, 50 );
	memset( filename, 0, 500 );
	findleng = GetEnvironmentVariable( "TLVDATA", tlvdir, 50 );
	if( !findleng ){
		TRACE( " tlvdir  directory var not found !!! error !!!\n" );
		return FALSE;
	}
	strcpy( filename, tlvdir );
	strcat( filename, "initmatrix.matrix" );
	hfile = CreateFile( filename, GENERIC_READ,
		0, NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		TRACE( "FROG : LoadKaeruMatrix : open error !!!!! %s\n", filename );
		return FALSE;
	}

	MATRIXDATAHEADER	matheader;
	DWORD	readsize;
	DWORD	headersize = sizeof( matheader );
	ReadFile( hfile, &matheader, headersize, &readsize, NULL );
	if( readsize != headersize ){
		TRACE( "FROG : LoadKaeruMatrix : read header error!!! %s\n", filename );
		return FALSE;
	}

	DWORD	sized3dmat = sizeof( D3DMATRIX );
	DWORD	ctrlnum, movenum, datanum, datasize;
	D3DMATRIX*	readmat;

	ctrlnum = matheader.ctrlnum; movenum = matheader.movenum;
	datanum = ctrlnum * movenum;
	datasize = sized3dmat * datanum;

	readmat = (D3DMATRIX*)malloc( datasize );
	if( !readmat ){
		TRACE( "FROG : LoadKaeruMatrix : d3dmatrix malloc error!!!!!!\n" );
		return FALSE;
	}
	
	ReadFile( hfile, readmat, datasize, &readsize, NULL );
	if( readsize != datasize ){
		TRACE( "FROG : LoadKaeruMatrix : read data error!!! %s\n", filename );
		return FALSE;
	}
	
	BOOL	ret;
	ret = m_armat->SetD3DMatrix( readmat, datanum );
	if( !ret ){
		TRACE( "FROG : LoadKaeruMatrix : m_armat->SetD3DMatrix error!!! %s\n", filename );
		return FALSE;
	}

	if( readmat ){
		free( readmat );
	}
	//////////

	InitBodyDirMat();

	return TRUE;
}

/*** CURRENTPAUSE2MATRIX ***/
void CFrog::CurrentPause2Matrix( CHARPARAM& curparam )
{
	D3DMATRIX*	firstmat;

	firstmat = m_arpause->GetMatPtr( 0 );
	m_p2mat->SetKaeruMatrix( &curparam, firstmat );
	MultKaeru( MODE_NORMAL );


	/***
	m_p2mat->SetKaeruMatrix( &curparam, m_mat4pause );
	// eb vertex の 変更
	KaeruMultMatrix( m_mat4pause );
	CpCalc2Temp();
	InterlockedExchange( &invalidFrog, 1 );
	***/
}


//EDITPOINT!!!   MAKEBINDMOVE
/*** MAKE BIND MOVE ***/
void CFrog::MakeBindMove( char* motfname, int pausecnt )
{
	int	moveno;
	CHARPARAM*	setparam;

	DWORD	setpsize = sizeof( CHARPARAM ) * pausecnt;
	setparam = (CHARPARAM*)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, setpsize );
	if( !setparam ){
		TRACE( "uvkaeru : MakeBindMove : setparam malloc error!!!!!\n" );
		exit(0);
	}

	for( moveno = 0; moveno < pausecnt; moveno++ ){
		Calc->InitCharParam( setparam + moveno );
	}
	
	LoadMotion( setparam, 0, motfname, LDM_MAX, -1 );

	D3DMATRIX*	firstmat = m_armat->GetMatPtr( CTLMAX * BINDSTARTNO );
	D3DMATRIX*	curmat;
	for( moveno = 0; moveno < pausecnt; moveno++ ){
		curmat = firstmat + CTLMAX * moveno;
		m_p2mat->SetKaeruMatrix( (setparam + moveno), curmat );
	}

	HeapFree( s_hHeap, HEAP_NO_SERIALIZE, setparam );
}



//EDITPOINT!!!	InitBodyDirMat
/*** INIT BODY DIR MATRIX ***/
void CFrog::InitBodyDirMat()
{

	int	dirno;
	float deg[9] = { 0.0f, 0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 225.0f, 270.0f, 315.0f };

	for( dirno = 0; dirno <= 8; dirno++ ){
		bodydir_cmat[dirno].SetMatrixCenterRotXYZ( 0.0f, deg[dirno], 0.0f, cenCTLALL );
	}
}

void CFrog::KaeruMove( int sckind )
{
	KaeruPosSet();
	CamPosSet( sckind );
}
void CFrog::KaeruPosSet()
{
	//int	dirno = kaerupos.dirindex;
	//int	walkflag;
	CHARPOS kpos;
	INDEX_L	nextindex, nextgindex;
	int	dirno;
	static int	grandh = 0; // dbg 用に　値を保存 (範囲外でも　set できるように)
	DWORD	dwwait;

	WaitForSingleObject( g_hMuteKaerupos, INFINITE );
	dirno = kaerupos.dirindex;
	kpos = kaerupos;
	ReleaseMutex( g_hMuteKaerupos );


	if( gplayarea->isMyMoving[ kpos.Move ] ){
		//gplayarea->CharPosMove( &kpos, gcharmoveindex[dirno] );// subindex で 動かす

		nextindex.x = kpos.index.x + gcharmoveindex[dirno].x;
		nextindex.y = kpos.index.y + gcharmoveindex[dirno].y;
		nextindex.z = kpos.index.z + gcharmoveindex[dirno].z;

		gplayarea->Index2GIndex( &nextgindex, nextindex );

		//walkflag = gwalkdata[nextindex.z][nextindex.x].walkflag;
		//if( gplayarea->isWalk[walkflag][dirno] ){
		//if( gplayarea->ChkWalk( nextindex, dirno ) ){

			if( nextgindex.x >= 0 && nextgindex.x < GMAXINDEXX
				&& nextgindex.z >= 0 && nextgindex.z < GMAXINDEXZ ){
				grandh = MapData->GetGrandH( nextgindex );
			}
			nextindex.y = grandh;
			
			kpos.index = nextindex;
			gplayarea->Index2Loc( &kpos.loc, &kpos.index );

		//}

		//gplayarea->Index2Loc( &kaerupos );
	}
	curdirindex = dirno;


	dwwait = WaitForSingleObject( g_hMuteKaerupos, 1000 );
	if( dwwait == WAIT_TIMEOUT ){
		TRACE( "FROGCPP : Mutex kaerupos wait timeout !!!!!!!!!!!!\n" );
	}
	kaerupos = kpos;
	ReleaseMutex( g_hMuteKaerupos );


	m_curdirmatrix.SetD3DMatrix( bodydir_cmat[curdirindex] );

}

void CFrog::CamPosSet( int sckind )
{
	INDEX_L	gindex;
	static int grandh = 0;
	int	kaeruh;
	CHARPOS	kpos;
	INDEX_L	camindex;
	D3DVECTOR	camloc;

	WaitForSingleObject( g_hMuteKaerupos, INFINITE );
	kpos = kaerupos;
	ReleaseMutex( g_hMuteKaerupos );


	switch( sckind ){
	case SCROLL_NONE:
		break;
	case SCROLL_X:
		camindex.x = kpos.index.x;
		break;
	case SCROLL_Z:
		camindex.y = kpos.index.y + gcamheight;
		break;
	case SCROLL_XZ:
		camindex.x = kpos.index.x;
		camindex.z = kpos.index.z - gcamdist;
		break;
	default:
		break;
	}

	gplayarea->Index2GIndex( &gindex, camindex );
	if( gindex.x >= 0 && gindex.x < GMAXINDEXX
		&& gindex.z >= 0 && gindex.z < GMAXINDEXZ ){
		grandh = MapData->GetGrandH( gindex ) + gcamheight;
	}
	kaeruh = kpos.index.y + gcamheight;

	if( grandh > kaeruh ){
		camindex.y = grandh;
	}else{
		camindex.y = kaeruh;
	}

	gplayarea->Index2Loc( &camloc, &camindex );

	WaitForSingleObject( g_hMuteCampos, INFINITE );
	campos.index = camindex;
	campos.loc = camloc;
	ReleaseMutex( g_hMuteCampos );

	float		cam2kY, cam2kZ;
	float	degx;

	cam2kY = kpos.loc.y - camloc.y;
	cam2kZ = kpos.loc.z - camloc.z;
	degx = Calc->DEGAtan( cam2kY, cam2kZ );

	m_vrotmat.SetMatrixCenterRotXYZ( degx, 0, 0, camloc );

	WaitForSingleObject( g_hMuteVRotMat, INFINITE );
	g_VRotMat = m_vrotmat;
	ReleaseMutex( g_hMuteVRotMat );


//////////////
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

void CFrog::SetKaeruClipStatus()
{

/////// camera は　キャラを　XYZ平行移動と　X軸回転でのみ　追う

	D3DVECTOR	kloc;
	D3DVECTOR	camloc;
	CMatrix		vrotmat;

	WaitForSingleObject( g_hMuteKaerupos, INFINITE );
		kloc = kaerupos.loc;
	ReleaseMutex( g_hMuteKaerupos );

	WaitForSingleObject( g_hMuteCampos, INFINITE );
		camloc = campos.loc;
	ReleaseMutex( g_hMuteCampos );

	

	tra_cmatrix.SetInitMatrix();
		// world * view 平行移動のみの場合は  trax = worldx - viewx !!!!
	//tra_cmatrix._41 = kloc.x - camloc.x;
	//tra_cmatrix._42 = kloc.y - camloc.y;
	//tra_cmatrix._43 = kloc.z - camloc.z;	
	tra_cmatrix._41 = kloc.x;
	tra_cmatrix._42 = kloc.y;
	tra_cmatrix._43 = kloc.z;	

	vrotmat = m_vrotmat;
	vrotmat._41 -= camloc.x;
	vrotmat._42 -= camloc.y;
	vrotmat._43 -= camloc.z;

	// m_vrotmat は　backg で　MultKaeru よりも　先に参照されるので
	// campos の　セット時に　計算しておく
	trans_cmatrix.MultMatrix3( &vrotmat, &tra_cmatrix, &m_curdirmatrix );


}


void CFrog::MirrorTlv2D( D3DTLVERTEX* retvert, D3DTLVERTEX* svert, int planenum, int pointnum )
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

void CFrog::MirrorTlv1D( D3DTLVERTEX* retvert, int* retmemnum, D3DTLVERTEX* svert, int* smemnum, 
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

