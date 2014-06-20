#include	"stdafx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <d3d.h>
#include <d3drmwin.h>
//#ifndef CALCH
//	#include "Calc.h"
//#endif
#include	"matrix.h"
#include	"playarea.h"

#ifndef		INTBOOL
	extern int		intbool_true[2];// 0 のとき 0, 1のとき1
	extern int		intbool_false[2]; // 0 のとき 1, 1 のとき 0
	extern int		inv_intbool[2]; // 0, 1 トグル
#endif


#ifndef	COLORNAMEH
	#include	"colorname.h"
#endif

// extern !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
extern CPlayArea*	gplayarea;
extern LONG		gprojmode;
extern HANDLE	g_hMuteCampos;
extern CHARPOS campos;


	// PROJ_DOOR 係数　BACKG用に　全画面分、INDEX刻み
static float	proj_d[2 * MAXINDEXX][2 * MAXINDEXY];

	// PROJ_DOOR 係数　CHARCTER用に　狭い範囲(画面中央)のみ、細かく
static float	doorproj[100][100];// PROJ_DOOR ( charcter 用 )


	// tex4 用 割り算表
static float	divtz2[ 4 * VISIBLERANGEZ * INT_POSSTEPZ ]; 





// texture UV セット用
static int		texpcnt = 0;
static int		texvcnt = 0;

static int		next_texpcnt[2] = {1, 0};
static int		next_texvcnt[2] = {1, 0};

static int		texkind = 0;

static D3DVALUE	texu4[4][2][2] = { { {0.0f, 0.5f}, {0.0f, 0.5f} }, { {0.5f, 1.0f}, {0.5f, 1.0f} }, 
								   { {0.0f, 0.5f}, {0.0f, 0.5f} }, { {0.5f, 1.0f}, {0.5f, 1.0f} } };// [texkind][texpcnt][texvcnt]

static D3DVALUE	texv4[4][2][2] = { { {0.0f, 0.0f}, {0.5f, 0.5f} }, { {0.0f, 0.0f}, {0.5f, 0.5f} }, 
								   { {0.5f, 0.5f}, {1.0f, 1.0f} }, { {0.5f, 0.5f}, {1.0f, 1.0f} }  };

	// 境界線で 他色が 出ないように 境界線の値を 避ける
static D3DVALUE	texu42[4][2][2] = { { {0.0f, 0.4f}, {0.0f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} }, 
								   { {0.0f, 0.4f}, {0.0f, 0.4f} }, { {0.6f, 0.9f}, {0.6f, 0.9f} } };// [texkind][texpcnt][texvcnt]

static D3DVALUE	texv42[4][2][2] = { { {0.0f, 0.0f}, {0.4f, 0.4f} }, { {0.0f, 0.0f}, {0.4f, 0.4f} }, 
								   { {0.6f, 0.6f}, {0.9f, 0.9f} }, { {0.6f, 0.6f}, {0.9f, 0.9f} }  };


// EB instruction
static int		instsizetex4 = 0;
static void*	insttex4 = 0;
static int		insttlvoff = 0;
static int		insttrioff = 0;

static int		sized3dinst	= sizeof( D3DINSTRUCTION );
static void*	instexit = 0;

///////////////////////////////////
// init  static 

HANDLE	CMatrix::s_hHeap = NULL;
DWORD	CMatrix::s_uNumAllocsInHeap = 0;
HWND	CMatrix::s_mainhwnd = 0;

///////////////////////////////////


void	*CMatrix::operator new ( size_t size )
{
	//TRACE( "CMatrix CREATE size %d\n", size );


	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		
		if( s_hHeap == NULL )
			return NULL;

		InitDivTz2(); // tex4 で使うdivtz2[] 初期化用の CMatrix
		InitDoorProj();
		InitInstTex4();
	}

	void	*p;
	p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size );
		
	if( p != NULL ){
		s_uNumAllocsInHeap++;
	}

	return (p);
}


void	CMatrix::operator delete ( void *p )
{
	if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
		s_uNumAllocsInHeap--;
	}

	if( s_uNumAllocsInHeap == 0 ){
		if( insttex4 ){
			HeapFree( s_hHeap, HEAP_NO_SERIALIZE, insttex4 );
			insttex4 = 0;
		}
		if( instexit ){
			HeapFree( s_hHeap, HEAP_NO_SERIALIZE, instexit );
			instexit = 0;
		}

		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
	}
}


void	CMatrix::SetDbgHwnd( HWND hwnd )
{
	s_mainhwnd = hwnd;
}
///////////////////////////////////
CMatrix::~CMatrix()
{

}

void CMatrix::InitDivTz2()
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// backg用(z 変化が　index で表せるobject 用)　の　proj 係数表
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// NEARCLIP / ( sz - camz ) の 表
	//	sz - camz < NEARCLIP のとき CLIPPING するので
		// 判定用に 0.0 を格納

	int	indexno, zindex;

	for( indexno = 0; indexno < 4 * VISIBLERANGEZ * INT_POSSTEPZ; indexno++ ){
		zindex = indexno - 2 * VISIBLERANGEZ * INT_POSSTEPZ;
		if( zindex == 0 ){
			divtz2[indexno] = 0.0f;
		}else{
			divtz2[indexno] = (float)( 1.0 / (double)zindex );
		}
	}
}

void CMatrix::InitInstTex4()
{
	LPD3DINSTRUCTION	lpInstruction;
	LPD3DPROCESSVERTICES	lpProcessV;
	DWORD*	tlvptr;
	WORD*	triptr;

	instsizetex4 = sizeof(D3DPROCESSVERTICES) + 2 * sizeof(D3DINSTRUCTION);
	insttex4 = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, instsizetex4 );
	if( !insttex4 ){
		::MessageBox(
			s_mainhwnd,
			"InitInstTex4 : alloc error!!!!\n",
			"MATRIXCPP",
			MB_OK | MB_TASKMODAL
		);
		return;
	}

	instexit = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, sized3dinst );
	if( !instexit ){
		::MessageBox(
			s_mainhwnd,
			"InstExit : alloc error!!!!\n",
			"MATRIXCPP",
			MB_OK | MB_TASKMODAL
		);
		return;
	}

	
	lpInstruction = (LPD3DINSTRUCTION)insttex4;
	lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
	lpInstruction->bSize = sizeof( D3DPROCESSVERTICES );
	lpInstruction->wCount = 1U;
	lpInstruction++;

	lpProcessV = (LPD3DPROCESSVERTICES)lpInstruction;
	lpProcessV->dwFlags = D3DPROCESSVERTICES_COPY;
	lpProcessV->wStart = 0U;
	lpProcessV->wDest = 0U;
	lpProcessV->dwCount = 0;// DWORD
	tlvptr = &(lpProcessV->dwCount);//
	lpProcessV->dwReserved = 0;
	lpProcessV++;

	// triangle
	lpInstruction = (LPD3DINSTRUCTION)lpProcessV;
	lpInstruction->bOpcode = D3DOP_TRIANGLE;
	lpInstruction->bSize = sizeof( D3DTRIANGLE );
	lpInstruction->wCount = 0; // WORD
	triptr = &(lpInstruction->wCount);

	insttlvoff = (BYTE*)tlvptr - (BYTE*)insttex4;
	insttrioff = (BYTE*)triptr - (BYTE*)insttex4;

	TRACE( "MATRIXCPP : InitInstTex4 : insttlvoff %d insttrioff %d\n",
		insttlvoff, insttrioff );

	lpInstruction = (LPD3DINSTRUCTION)instexit;
	lpInstruction->bOpcode = D3DOP_EXIT;
	lpInstruction->bSize = 0;
	lpInstruction->wCount = 0U;

}

void CMatrix::InitDoorProj()
{
	InitDoorProj_char();
	InitDoorProj_back();
}


void CMatrix::InitDoorProj_char()
{
	// charcter 用　の　proj係数表

	int	indexx, indexy;
	int	pindexx, pindexy;
	int	indexabsx, indexabsy;
	double	diff;
	double	projx, projy, deg;

	diff = 2.5 / 100.0;

	for( indexx = 0; indexx < 100; indexx++ ){

		pindexx = indexx - 50;		
		indexabsx = abs( pindexx );
		projx = (5.0 + indexabsx * indexabsx * diff ) * DEG2PAI;
		
		for( indexy = 0; indexy < 100; indexy++ ){

			pindexy = indexy - 50;		
			indexabsy = abs( pindexy );
			projy = (5.0 + indexabsy * indexabsy * diff ) * DEG2PAI;
	
			deg = projx + projy;
			
			if( deg > PAIDIV2 )
				deg = PAIDIV2;

			//(NEARCLIP * SCWIDTH2 ) / TAN( DEG )
			doorproj[indexx][indexy] = (NEARCLIP * SCWIDTH2) / (float)tan( deg );
		}
	}
}

void CMatrix::InitDoorProj_back()
{
	// backg 用　の　proj係数表

	int	indexx, indexy;
	int	pindexx, pindexy;
	int	indexabsx, indexabsy;
	double	diff;
	double	projx, projy, deg;

	//diff = 2.5 / 100.0;
	diff = 0.02;
	for( indexx = 0; indexx < 2 * MAXINDEXX; indexx++ ){

		pindexx = indexx - MAXINDEXX;
		indexabsx = abs( pindexx );
		projx = (10.0 + indexabsx * indexabsx * diff ) * DEG2PAI;
		
		for( indexy = 0; indexy < 2 * MAXINDEXY; indexy++ ){

			pindexy = indexy - MAXINDEXY;		
			indexabsy = abs( pindexy );
			projy = (10.0 + 0.5 * indexabsy * indexabsy * diff ) * DEG2PAI;
			//projy = 0.0;

			deg = projx + projy;
			
			if( deg > PAIDIV2 )
				deg = PAIDIV2;

			proj_d[indexx][indexy] = (NEARCLIP * SCWIDTH2) / (float)tan( deg );
		}
	}

}








void	CMatrix::SetUVTex4( D3DTLVERTEX* svert,
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int* texkind4, 
			int	planenum, int pointnum )
{

	int	headno;
	int	startno, endno;
	int	planeno, pointno;

	LPD3DTLVERTEX	setvert;

	texpcnt = 0;


	for( planeno = 0; planeno < planenum; planeno++ ){
		startno = (calcrange + planeno)->start;
		endno = (calcrange + planeno)->end;
		
		headno = pointnum * planeno;
		texvcnt = 0;

		for( pointno = startno; pointno <= endno; pointno++ ){
			setvert = svert + headno + pointno;
			
			texkind = *(texkind4 + pointnum * planeno + pointno);

			setvert->tu = texu4[texkind][texpcnt][texvcnt];
			setvert->tv = texv4[texkind][texpcnt][texvcnt];

			texvcnt = next_texvcnt[texvcnt];
		}
		texpcnt = next_texpcnt[texpcnt];
	}

}
void	CMatrix::SetUVTex4( D3DTLVERTEX* svert,
			int* texkind4, 
			int	planenum, int pointnum )
{

	int	headno;
	int	startno, endno;
	int	planeno, pointno;

	LPD3DTLVERTEX	setvert;

	texpcnt = 0;


	for( planeno = 0; planeno < planenum; planeno++ ){
		startno = 0;
		endno = pointnum - 1;
		
		headno = pointnum * planeno;
		texvcnt = 0;

		for( pointno = startno; pointno <= endno; pointno++ ){
			setvert = svert + headno + pointno;
			
			texkind = *(texkind4 + pointnum * planeno + pointno);

			setvert->tu = texu4[texkind][texpcnt][texvcnt];
			setvert->tv = texv4[texkind][texpcnt][texvcnt];

			texvcnt = next_texvcnt[texvcnt];
		}
		texpcnt = next_texpcnt[texpcnt];
	}

}

void	CMatrix::SetUVTex4( D3DTLVERTEX* svert,
			int bmpnoh, int bmpnow,
			int* texkind4,
			int planenum, int pointnum )
{

	//int	headno;
	//int	startno, endno;
	DWORD	headno;
	DWORD	startw, starth;
	DWORD	planeno, pointno;

	LPD3DTLVERTEX	setvert;

	texpcnt = 0;

	// GrandBMP 用であることを　チェック
	if( planenum != BMPH || pointnum != BMPW ){
		TRACE( "MATRIXCPP : PARAM error !!!\n" );
		return;
	}


	// svert は　map 全範囲、　texkind は　bmpblock　内

	starth = bmpnoh * BMPH;
	startw = bmpnow * BMPW;
	for( planeno = 0; planeno < BMPH; planeno++ ){
		//startno = 0;
		//endno = pointnum - 1;
		//headno = pointnum * planeno;

		headno = GMAXINDEXX * ( starth + planeno ) + startw;
		texvcnt = 0;

		for( pointno = 0; pointno < BMPW; pointno++ ){
			setvert = svert + headno + pointno;
			
			texkind = *(texkind4 + pointnum * planeno + pointno);

			setvert->tu = texu4[texkind][texpcnt][texvcnt];
			setvert->tv = texv4[texkind][texpcnt][texvcnt];

			texvcnt = next_texvcnt[texvcnt];
		}
		texpcnt = next_texpcnt[texpcnt];
	}

}




void	CMatrix::MultVertEBTex40( D3DTLVERTEX* lptlv, 
			LPD3DTLVERTEX* lplpsvert, int setnum, 
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int tlvoffset, int* rettrinum,  
			int	planenum, int startz, CMatrix vrotmat )
{
	// PROJ_NORMAL : 画角一定


	// static な 表 divtz2 を 初期化するために
	//	InitDivTz2() を 呼んだ後で 実行

	float	s12 = vrotmat._12;
	float	s22 = vrotmat._22;
	float	s32 = vrotmat._32;
	float	s42 = vrotmat._42;
	float	s13 = vrotmat._13;
	float	s23 = vrotmat._23;
	float	s33 = vrotmat._33;
	float	s43 = vrotmat._43;
	
	float	tx, ty, txold, deltax;
	float	tx0, ty0, tz0;
	float	settx, setty, settz;

	int		iz;
	float	tz, tzold, deltaz;
	BOOL	*clipptr;
	float	camx, camy, camz;
	float	projsc;
	int	vertnum;
	int	planeno, pointno;
	int	setno, vertsize, boolsize;


	LPD3DTRIANGLE		lpTriangle;

	LPD3DTLVERTEX		lpfirsttlv;


	LPD3DTLVERTEX		setvert;
	LPD3DTLVERTEX		headptr;
	
	BYTE*	blptlv;
	DWORD*	insttlvptr;
	WORD*	insttriptr;


	BOOL	cliprender[ (VISIBLERANGEZ + 1) * GMAXINDEXX ];
	static float	normalpsc = (float)( ( (double)NEARCLIP * (double)SCWIDTH2 ) / (double)TANFOV );

	
	int		itriflag = 0;
	int		skipflag = 0;

	vertsize = sizeof( D3DTLVERTEX );
	boolsize = sizeof( BOOL );

	static float	befsettx = 0.0f;
	static float	befsetty = 0.0f;

	static int testcnt = 0;

	testcnt++;

	lpfirsttlv = lptlv;
	clipptr = cliprender;


	WaitForSingleObject( g_hMuteCampos, INFINITE );
	camx = campos.loc.x;
	camy = campos.loc.y;
	camz = campos.loc.z;
	ReleaseMutex( g_hMuteCampos );


	deltax = POSSTEPX * INDEX2LOC; // LOC
	deltaz = (float)(-POSSTEPZ * INDEX2LOC);

	gplayarea->GIndex2Index_Z( &iz, startz );
	gplayarea->Index2Loc_Z( &tzold, iz );
	tzold = tzold - deltaz;
	
	setno = 0;

	// svert は　calcrange で　既に　切り抜いてある！！！！！！！( ver. 10_9 ～ ) 

	for( planeno = 0; planeno < planenum; planeno++ ){
		vertnum = (calcrange + planeno)->num;
		
		headptr = *( lplpsvert + planeno );

		tz0 = tzold + deltaz;
		tzold = tz0;

		texvcnt = 0;
		txold = headptr->sx - deltax; // 初回も deltax を 足すため


		// 座標以外は まとめて COPY !!!!!!!!!!!!
		memcpy( lptlv, headptr, vertsize * vertnum );

		//setvert = headvert;
		setvert = headptr;
		for( pointno = 0; pointno < vertnum; pointno++ ){

				// 回転は　x 軸に関してしか　行わないので　tx は　vrotmat の影響を受けない
				// x と z は　参照 せずに　差分で
			tx0 = txold + deltax; 
			txold = tx0;

			tz = s13 * tx0 + s23 * ty0 + s33 * tz0 + s43 - camz;

			*clipptr = FALSE; ////////

			if( tz >= NEARCLIP ){

				ty0 = setvert->sy;
				projsc = normalpsc / tz; /////

				tx = tx0 - camx;
				ty = s12 * tx0 + s22 * ty0 + s32 * tz0 + s42 - camy; 

				settx = tx * projsc + SCWIDTH2;
				setty = -ty * projsc + SCHEIGHT2;
				settz = ( tz - NEARCLIP ) * INV_ZDIST;
			}else{

				settx = befsettx;
				setty = befsetty;
				settz = -1.0f;//
				*clipptr = TRUE;
			}


			if( settx < 0.0 ){
				*clipptr = TRUE;
				settx = 0.0f;
			}else if( settx >= SCWIDTH ){
				*clipptr = TRUE;
				settx = CLPWIDTH;
			}

			if( setty < 0.0 ){
				*clipptr = TRUE;
				setty = 0.0f;
			}else if( setty >= SCHEIGHT ){
				*clipptr = TRUE;
				setty = CLPHEIGHT;
			}

			befsettx = settx;
			befsetty = setty;
			
			lptlv->sx = settx;
			lptlv->sy = setty;
			lptlv->sz = settz;

			lptlv++; clipptr++;
			setno++; setvert++;
		}
	}

	blptlv = (BYTE*)lptlv;// inst の　先頭
	lpTriangle = (LPD3DTRIANGLE)(blptlv + instsizetex4);

	//if( setno == setnum ){
	//	TRACE( "MATRIXCPP : TEX40 : setno error !!!!!! : setno %d setnum %d\n",
	//		setno, setnum );
	//}

	int	startno0, startno1, endno0, endno1;
	int	pointmax;
	int	settrinum = 0;
	int	v1, v2, v3;

		// indexrange は　3分割していないので 初期値で引く
			// tlvoffset : make0 - calc0

	for( planeno = 0; planeno < planenum - 1; planeno++ ){
		startno0 = (indexrange + planeno)->start0 - tlvoffset;
		endno0 = (indexrange + planeno)->end0 - tlvoffset;
		startno1 = (indexrange + planeno)->start1 - tlvoffset;
		endno1 = (indexrange + planeno)->end1 - tlvoffset;
		pointmax = endno0 - startno0; // endno1 - startno1 と 同じはず
		
		itriflag = 0;
		//skipflag = 0;
		skipflag = 0;

		for( pointno = 0; pointno <= pointmax - 1; pointno++ ){
			
			v1 = startno1 + pointno;
			v2 = startno0 + pointno;
			v3 = startno1 + pointno + 1;
			
			if( *(cliprender + v1) && *(cliprender + v2) && *(cliprender + v3) ){
			//if( *(cliprender + v1) || *(cliprender + v2) || *(cliprender + v3) ){

				//itriflag = nextitriflag[itriflag];
				skipflag = 0;
				goto secren;
			}

			lpTriangle->wV1 = v1;
			lpTriangle->wV2 = v2;
			lpTriangle->wV3 = v3;
			
			lpTriangle->wFlags = D3DTRIFLAG_EVEN * skipflag; // 0 は　D3DTRIFLAG_START
			lpTriangle++;
			settrinum++;

			skipflag = 1;

secren :
			
			v1 = startno1 + pointno + 1;
			v2 = startno0 + pointno;
			v3 = startno0 + pointno + 1;
			
			if( *(cliprender + v1) && *(cliprender + v2) && *(cliprender + v3) ){
			//if( *(cliprender + v1) || *(cliprender + v2) || *(cliprender + v3) ){
				skipflag = 0;
				continue;
			}

			lpTriangle->wV1 = v1;
			lpTriangle->wV2 = v2;
			lpTriangle->wV3 = v3;
			lpTriangle->wFlags = D3DTRIFLAG_ODD * skipflag; // 0 は　D3DTRIFLAG_START
			lpTriangle++;
			settrinum++;
			skipflag = 1;

		}
	}
	

	// set Instruction
	memcpy( lptlv, insttex4, instsizetex4 );
	memcpy( lpTriangle, instexit, sized3dinst );

	insttlvptr = (DWORD*)( blptlv + insttlvoff );
	insttriptr = (WORD*)( blptlv + insttrioff );
	
	*insttlvptr = setnum;
	*insttriptr = settrinum;
	*rettrinum = settrinum;
		
}


/***
void	CMatrix::MultVertEBTex41( D3DTLVERTEX* lptlv, 
			D3DTLVERTEX* svert,  INDEX_L*	sindex,
			RANGEDATA* calcrange, RANGEDATA2* indexrange, 
			int* rettlvnum, int tlvoffset, int* rettrinum,  
			int	planenum, int pointnum, int startz )
{
	// PROJ_DOOR

	// static な 表 divtz2 を 初期化するために
	//	InitDivTz2() を 呼んだ後で 実行

	int	headno;
	float	tx, ty, txold, deltax;
	float	settx, setty, settz;
	

	int		 tz, tz2, tzold, deltaz;
	BOOL	*clipptr;
	float	camx, camy;
	int		icamx, icamy, icamz;
	int		igx, igy, igz;
	int		datano;
	int		v1, v2, v3;

	float	projsc, dproj, projsc_d;
	int	startno, endno;
	int	planeno, pointno;
	int	setno, vertsize, boolsize;
	LPD3DINSTRUCTION	lpInstruction;
	LPD3DINSTRUCTION	lpInst_SetNum;
	LPD3DTRIANGLE		lpTriangle;
	LPD3DPROCESSVERTICES	lpProcessV;
	LPD3DTLVERTEX		lpfirsttlv;

	LPD3DTLVERTEX		setvert;
	INDEX_L*			setindex;

	BOOL	cliprender[ (VISIBLERANGEZ + 1) * GMAXINDEXX ];

	static int dbgcnt = 0;


	int		itriflag = 0;
	int		skipflag = 0;

	vertsize = sizeof( D3DTLVERTEX );
	boolsize = sizeof( BOOL );

	static float	befsettx = 0.0f;
	static float	befsetty = 0.0f;

	static int testcnt = 0;

	testcnt++;

	lpfirsttlv = lptlv;
	clipptr = cliprender;

	setno = 0;

	WaitForSingleObject( g_hMuteCampos, INFINITE );
	camx = campos.loc.x;
	camy = campos.loc.y;
	icamx = campos.index.x;
	icamy = campos.index.y;
	icamz = campos.index.z;// z は index 
	ReleaseMutex( g_hMuteCampos );


	deltax = POSSTEPX * INDEX2LOC; // LOC
	deltaz = -INT_POSSTEPZ * INT_INDEX2LOC; // (int)LOC

	//tzold = (startz * INT_POSSTEPZ) * INT_INDEX2LOC - deltaz;
	tzold = sindex->z - deltaz - icamz;

	dbgcnt++;

	if( dbgcnt == 1 ){
		TRACE( "MATRIX : icamx %d\n", icamx );
	}
	for( planeno = 0; planeno < planenum; planeno++ ){
		startno = (calcrange + planeno)->start;
		endno = (calcrange + planeno)->end;
		
		if( dbgcnt == 1 ){
			TRACE( "MATRIX : plane %d start %d end %d\n", planeno, startno, endno );
		}

		headno = pointnum * planeno;

		tz2 = tzold + deltaz;
		tzold = tz2;

		// + MAXINDEXZ は 負の数の場合の 保険
		projsc = divtz2[ tz2 + MAXINDEXZ ]; 

		texvcnt = 0;
		txold = ((svert + headno + startno)->sx - camx) - deltax; // 初回も deltax を 足すため


		// 座標以外は まとめて COPY !!!!!!!!!!!!
		memcpy( lptlv, svert + headno + startno, vertsize * (endno - startno + 1) );

		for( pointno = startno; pointno <= endno; pointno++ ){
			datano = headno + pointno;
			setvert = svert + datano;
			setindex = sindex + datano;
			igx = setindex->x - icamx + MAXINDEXX;
			igy = setindex->y - icamy + MAXINDEXY;

			dproj = proj_d[ igx ][ igy ];
			projsc_d = projsc * dproj;

			tx = txold + deltax; // x の 変化量は 一定なので 差分で 計算
			ty = setvert->sy - camy;

			*clipptr = FALSE; ////////

			if( tz2 >= NEARCLIP ){
				settx = tx * projsc_d + SCWIDTH2;
				setty = -ty * projsc_d + SCHEIGHT2;
				settz = ( tz2 - NEARCLIP ) * INV_ZDIST;
			}else{
				settx = befsettx;
				setty = befsetty;
				settz = -1.0f;//
				*clipptr = TRUE;
			}
			befsettx = settx;
			befsetty = setty;


			if( settx < 0.0 ){
				*clipptr = TRUE;
				settx = 0.0f;
			}else if( settx >= SCWIDTH ){
				*clipptr = TRUE;
				settx = CLPWIDTH;
			}

			if( setty < 0.0 ){
				*clipptr = TRUE;
				setty = 0.0f;
			}else if( setty >= SCHEIGHT ){
				*clipptr = TRUE;
				setty = CLPHEIGHT;
			}
				
			lptlv->sx = settx;
			lptlv->sy = setty;
			lptlv->sz = settz;

			txold = tx;

			lptlv++; clipptr++;
			setno++;
		}
	}
	*rettlvnum = setno;

	lpInstruction = (LPD3DINSTRUCTION)lptlv;
	lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
	lpInstruction->bSize = sizeof( D3DPROCESSVERTICES );
	lpInstruction->wCount = 1U;
	lpInstruction++;

	lpProcessV = (LPD3DPROCESSVERTICES)lpInstruction;
	lpProcessV->dwFlags = D3DPROCESSVERTICES_COPY;
	lpProcessV->wStart = 0U;
	lpProcessV->wDest = 0U;
	lpProcessV->dwCount = setno;//
	lpProcessV->dwReserved = 0;
	lpProcessV++;

	// triangle
	lpInstruction = (LPD3DINSTRUCTION)lpProcessV;
	lpInst_SetNum = lpInstruction;//!!!!!!!!!!!!!!!!!!!
	lpInstruction->bOpcode = D3DOP_TRIANGLE;
	lpInstruction->bSize = sizeof( D3DTRIANGLE );
	lpInstruction->wCount = 0U;//!!!!!!!!!!!!!!!!! 確定後に セット
	lpInstruction++;

	lpTriangle = (LPD3DTRIANGLE)lpInstruction;
	

	int	startno0, startno1, endno0, endno1;
	int	pointmax;
	int	firstno;
	int	settrinum = 0;
	
	firstno = indexrange->start0 - tlvoffset;


	for( planeno = 0; planeno < planenum - 1; planeno++ ){
		startno0 = (indexrange + planeno)->start0 - firstno;
		endno0 = (indexrange + planeno)->end0 - firstno;
		startno1 = (indexrange + planeno)->start1 - firstno;
		endno1 = (indexrange + planeno)->end1 - firstno;
		pointmax = endno0 - startno0; // endno1 - startno1 と 同じはず
		
		itriflag = 0;
		//skipflag = 0;
		skipflag = 0;

		for( pointno = 0; pointno < pointmax; pointno++ ){

			v1 = startno1 + pointno;
			v2 = startno0 + pointno;
			v3 = startno1 + pointno + 1;

			if( *(cliprender + v1) && *(cliprender + v2) && *(cliprender + v3) ){

				//itriflag = nextitriflag[itriflag];
				skipflag = 0;
				goto secren;
			}


			lpTriangle->wV1 = v1;
			lpTriangle->wV2 = v2;
			lpTriangle->wV3 = v3;
			
			lpTriangle->wFlags = D3DTRIFLAG_EVEN * skipflag; // 0 は　D3DTRIFLAG_START
			lpTriangle++;
			settrinum++;

			skipflag = 1;

secren :
			v1 = startno1 + pointno + 1;
			v2 = startno0 + pointno;
			v3 = startno0 + pointno + 1;

			if( *(cliprender + v1) && *(cliprender + v2) && *(cliprender + v3) ){
				skipflag = 0;
				continue;
			}

			lpTriangle->wV1 = v1;
			lpTriangle->wV2 = v2;
			lpTriangle->wV3 = v3;
			lpTriangle->wFlags = D3DTRIFLAG_ODD * skipflag; // 0 は　D3DTRIFLAG_START
			lpTriangle++;
			settrinum++;
			skipflag = 1;

		}
	}
	
	lpInst_SetNum->wCount = settrinum;//!!!!!!!!!!!!!!!

	lpInstruction = (LPD3DINSTRUCTION)lpTriangle;
	lpInstruction->bOpcode = D3DOP_EXIT;
	lpInstruction->bSize = 0;
	lpInstruction->wCount = 0U;
	
	*rettrinum = settrinum;

}
***/


void	CMatrix::MultVertEBFix30( D3DTLVERTEX* lptlv, D3DTLVERTEX* svert,
				int planenum, int pointnum )
{
	// tan( fov ) == FROGTANFOV の 円錐 VIEWPORT

	int	datano;
	float	tx, ty, tz, aw, rhw;


	int	datamaxno;
	LPD3DTLVERTEX		lpfirsttlv, curtlv;
	LPD3DTLVERTEX		sourcetlv;
	static	float	projsc = ( NEARCLIP * SCWIDTH2 ) / ( (float)FROGTANFOV );

	curtlv = lptlv;
	lpfirsttlv = lptlv;


	datamaxno = planenum * pointnum;

	for( datano = 0; datano < datamaxno; datano++ ){
		sourcetlv = svert + datano;

		tx = sourcetlv->sx;
		ty = sourcetlv->sy;
		tz = sourcetlv->sz;			

		
		aw = _13 * tx + _23 * ty + _33 * tz + _43;

		if( aw )
			rhw = projsc / aw;
		else
			rhw = 0.0f;

		curtlv->rhw = rhw;
		curtlv->sx = (_11 * tx + _21 * ty + _31 * tz + _41) * rhw + SCWIDTH2;
		curtlv->sy = (_12 * tx + _22 * ty + _32 * tz + _42) * -rhw + SCHEIGHT2; 
		curtlv->sz = ( aw - NEARCLIP ) * INV_ZDIST;

		curtlv->color = sourcetlv->color;
		curtlv->specular = sourcetlv->specular;

		curtlv++;
	}

}


void	CMatrix::MultVertEBFix31( D3DTLVERTEX* lptlv, D3DTLVERTEX* svert,
				int planenum, int pointnum )
{
	// DOOR SCOPE
	// 視点中央から 外にかけて FOV  ( projsc ) を 大きくする
	// static な 表を 初期化するために InitDoorProj() を 呼んだ後で 実行

	int	datano;
	float	tx, ty, tz, aw, rhw;


	int	datamaxno;
	LPD3DTLVERTEX		lpfirsttlv, curtlv;
	LPD3DTLVERTEX		sourcetlv;

	float	basex, basey;
	int	int_basex, int_basey;
	float	projsc;
	static int	testcnt = 0;

	testcnt++;

	curtlv = lptlv;
	lpfirsttlv = lptlv;

	datamaxno = planenum * pointnum;

	for( datano = 0; datano < datamaxno; datano++ ){
		sourcetlv = svert + datano;

		tx = sourcetlv->sx;
		ty = sourcetlv->sy;
		tz = sourcetlv->sz;			
		
		basex = _11 * tx + _21 * ty + _31 * tz + _41;
		int_basex = (int)(basex * 10.0f) + 50;
		if( int_basex < 0 )
			int_basex = 0;
		else if( int_basex >= 100 )
			int_basex = 99;

		basey = _12 * tx + _22 * ty + _32 * tz + _42;
		int_basey = (int)(basey * 10.0f) + 50;
		if( int_basey < 0 )
			int_basey = 0;
		else if( int_basey >= 100 )
			int_basey = 99;
		

		//projsc = (NEARCLIP * SCWIDTH2 ) / doorproj[ int_basex ][ int_basey ] ;
		projsc = doorproj[ int_basex ][ int_basey ];

		aw = _13 * tx + _23 * ty + _33 * tz + _43;
		if( aw )
			rhw = projsc / aw;
		else
			rhw = 0.0f;

		curtlv->rhw = rhw;
		curtlv->sx = basex * rhw + SCWIDTH2;
		curtlv->sy = basey * -rhw + SCHEIGHT2; 
		curtlv->sz = ( aw - NEARCLIP ) * INV_ZDIST;


		curtlv->color = sourcetlv->color;
		curtlv->specular = sourcetlv->specular;

		curtlv++;
	}
}


				//uv set のみ
void	CMatrix::SetUV2D( D3DTLVERTEX* lptlv, 
				int planenum, int pointnum, int textype )
{

	int	planeno, pointno;
	LPD3DTLVERTEX	curtlv;

	static int testcnt = 0;

	curtlv = lptlv;

	texpcnt = 0;

	for( planeno = 0; planeno < planenum; planeno++ ){		
		
		texvcnt = 0;
		
		for( pointno = 0; pointno < pointnum; pointno++ ){
			
			curtlv->tu = texu42[textype][texpcnt][texvcnt];
			curtlv->tv = texv42[textype][texpcnt][texvcnt];

			curtlv++;
			texvcnt = next_texvcnt[texvcnt];
		}
		texpcnt = next_texpcnt[texpcnt];
	}

}


void	CMatrix::SetUV1D( D3DTLVERTEX* lptlv, int* memnum, 
				int fannum, int textype )
{
	int	fanno, pointno;
	LPD3DTLVERTEX	curtlv;

	static int testcnt = 0;

	curtlv = lptlv;

	for( fanno = 0; fanno < fannum; fanno++ ){				
		texvcnt = 0;
		for( pointno = 0; pointno < *(memnum + fanno); pointno++ ){

			if( pointno != 0 )
				texpcnt = 1;
			else
				texpcnt = 0;
			
			curtlv->tu = texu42[textype][texpcnt][texvcnt];
			curtlv->tv = texv42[textype][texpcnt][texvcnt];

			curtlv++;
			texvcnt = next_texvcnt[texvcnt];
		}
	}
}


void	CMatrix::SetTridataYoko( D3DTRIANGLE* lptri, int planenum, int pointnum, int poffset )
{
	int	headno0, headno1;
	int	setno = 0;

	int	planeno, pointno;
	int		itriflag = 0;
	int		skipflag = 0;
	int	nextp;
	int	flatnum[ FLATMAX ];
	int	flatcalc, flatno, flatcnt = 0;
	int	setflat, vInPlane;

	//LPD3DINSTRUCTION	lpInstruction;
	LPD3DTRIANGLE		lpTriangle;

	lpTriangle = lptri;

	flatcalc = pointnum * 2;
	flatno = 0;
	while( flatcalc > 30 ){
		flatnum[flatno] = 30;
		flatcalc -= 30;
		flatno++;
	}
	flatnum[flatno] = flatcalc;



	for( planeno = 0; planeno < planenum - 1; planeno++ ){//
		
		headno0 = pointnum * planeno + poffset;
		headno1 = headno0 + pointnum;	

		itriflag = 0;// plane の 始めは D3DTRIFLAG_START
		skipflag = 0;

		flatcnt = 0;
		vInPlane = 0;
		for( pointno = 0; pointno < pointnum; pointno++ ){
			if( pointno != pointnum - 1 )
				nextp = pointno + 1;
			else
				nextp = 0;


			lpTriangle->wV1 = headno0 + pointno;
			lpTriangle->wV2 = headno1 + pointno;
			lpTriangle->wV3 = headno0 + nextp;

			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_ODD;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
				//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
			}
			lpTriangle++; setno++; vInPlane++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}
				
			lpTriangle->wV1 = headno0 + nextp;
			lpTriangle->wV2 = headno1 + pointno;
			lpTriangle->wV3 = headno1 + nextp;
			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_EVEN;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
			}
			lpTriangle++; setno++; vInPlane++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}

		}
	}
	
}


void	CMatrix::SetTriYokoJ(  D3DTRIANGLE* lptri, int jindex, int planenum, int pointnum, int poffset )
{
	int	headno0, headno1;
	int	setno = 0;

	int	planeno, pointno;
	int		itriflag = 0;
	int	nextp;
	int	skipflag = 0;


	int	flatnum[ FLATMAX ];
	int	flatcalc, flatno, flatcnt = 0;
	int	setflat, vInPlane;

	LPD3DTRIANGLE		lpTriangle;

	lpTriangle = lptri;

	flatcalc = pointnum * 2;
	flatno = 0;
	while( flatcalc > 30 ){
		flatnum[flatno] = 30;
		flatcalc -= 30;
		flatno++;
	}
	flatnum[flatno] = flatcalc;


	// joint triangle	
	headno0 = jindex;
	headno1 = poffset;


	//TRACE( "MATRIXCPP : SetTriYokoJ : jindex %d poffset %d\n",
	//	jindex, poffset );

	itriflag = 0;
	skipflag = 0;
	flatcnt = 0;
	vInPlane = 0;
	for( pointno = 0; pointno < pointnum; pointno++ ){
		if( pointno != pointnum - 1 )
			nextp = pointno + 1;
		else
			nextp = 0;

		lpTriangle->wV1 = headno0 + pointno;
		lpTriangle->wV2 = headno1 + pointno;
		lpTriangle->wV3 = headno0 + nextp;

		if( skipflag ){
			lpTriangle->wFlags = D3DTRIFLAG_ODD;
		}else{
			setflat = flatnum[ flatcnt ];
			lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
			//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
		}
		lpTriangle++; setno++; vInPlane++;
		if( vInPlane % 30 == 0 ){
			skipflag = 0;
			flatcnt++;
		}else{
			skipflag = 1;
		}
				
		lpTriangle->wV1 = headno0 + nextp;
		lpTriangle->wV2 = headno1 + pointno;
		lpTriangle->wV3 = headno1 + nextp;
		if( skipflag ){
			lpTriangle->wFlags = D3DTRIFLAG_EVEN;
		}else{
			setflat = flatnum[ flatcnt ];
			lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
		}
		lpTriangle++; setno++; vInPlane++;
		if( vInPlane % 30 == 0 ){
			skipflag = 0;
			flatcnt++;
		}else{
			skipflag = 1;
		}

	}

	//TRACE( "MATRIXCPP : SetTriYokoJ : setno %d\n",
	//	setno );
	
	// normal triangle
	for( planeno = 0; planeno < planenum - 1; planeno++ ){//
		
		headno0 = pointnum * planeno + poffset;
		headno1 = headno0 + pointnum;	

		itriflag = 0;// plane の 始めは D3DTRIFLAG_START
		skipflag = 0;
		flatcnt = 0;
		vInPlane = 0;

		for( pointno = 0; pointno < pointnum; pointno++ ){
			if( pointno != pointnum - 1 )
				nextp = pointno + 1;
			else
				nextp = 0;

			lpTriangle->wV1 = headno0 + pointno;
			lpTriangle->wV2 = headno1 + pointno;
			lpTriangle->wV3 = headno0 + nextp;

			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_ODD;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
				//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
			}
			lpTriangle++; setno++; vInPlane++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}
					
			lpTriangle->wV1 = headno0 + nextp;
			lpTriangle->wV2 = headno1 + pointno;
			lpTriangle->wV3 = headno1 + nextp;
			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_EVEN;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
			}
			lpTriangle++; setno++; vInPlane++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}

		}
	}
	//TRACE( "MATRIXCPP : SetTriYokoJ : setno %d\n\n\n",
	//	setno );
	
}


void	CMatrix::SetTridataYLine( D3DTRIANGLE* lptri, int planenum, int pointnum, int poffset )
{
	int	headno0, headno1;
	int	setno = 0;

	int	planeno, pointno;
	int		itriflag = 0;
	int	nextp;
	int	skipflag = 0;

	int	flatnum[ FLATMAX ];
	int	flatcalc, flatno, flatcnt = 0;
	int	setflat, vInPlane;

	LPD3DTRIANGLE		lpTriangle;

	lpTriangle = lptri;
	
	flatcalc = (pointnum - 1) * 2;
	flatno = 0;
	while( flatcalc > 30 ){
		flatnum[flatno] = 30;
		flatcalc -= 30;
		flatno++;
	}
	flatnum[flatno] = flatcalc;

	
	for( planeno = 0; planeno < planenum - 1; planeno++ ){//
		
		headno0 = pointnum * planeno + poffset;
		headno1 = headno0 + pointnum;	

		itriflag = 0;// plane の 始めは D3DTRIFLAG_START
		skipflag = 0;
		flatcnt = 0;
		vInPlane = 0;

		for( pointno = 0; pointno < pointnum - 1; pointno++ ){
			nextp = pointno + 1;

			lpTriangle->wV1 = headno0 + pointno;
			lpTriangle->wV2 = headno1 + pointno;
			lpTriangle->wV3 = headno0 + nextp;

			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_ODD;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
				//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
			}
			lpTriangle++; setno++; vInPlane++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}
					
			lpTriangle->wV1 = headno0 + nextp;
			lpTriangle->wV2 = headno1 + pointno;
			lpTriangle->wV3 = headno1 + nextp;
			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_EVEN;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
			}
			lpTriangle++; setno++; vInPlane++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}
		}
	}
}


void	CMatrix::SetTridataFan( D3DTRIANGLE* lptri, 
			int fannum, int pointnum, int poffset, int* memnum )
{
	int	setno = 0;
	int	fanno, pointno, centerno;
	int	membernum;
	int	trino = 0;

	int	flatnum[ FLATMAX ];
	int	flatcalc, flatno, flatcnt = 0;
	int	setflat, vInPlane;
	int	skipflag = 0;

	LPD3DTRIANGLE		lpTriangle;

	lpTriangle = lptri;

	setno = 0;
	for( fanno = 0; fanno < fannum; fanno++ ){		
		if( (membernum = *(memnum + fanno)) < 3 ){
			TRACE( "SetTridataFan : memnum under 3 !!!!! : exit !!! \n" );
			return;
		}
		
		flatcalc = membernum;
		flatno = 0;
		while( flatcalc > 30 ){
			flatnum[flatno] = 30;
			flatcalc -= 30;
			flatno++;
		}
		flatnum[flatno] = flatcalc;
		
		flatcnt = 0;
		vInPlane = 0;


		centerno = poffset + setno;
		//TRACE( "\nMATRIXCPP : SetTridataFan : poffset %d  centerno %d\n", poffset, centerno );

		// 最初の ３点		
		lpTriangle->wV1 = centerno;
		lpTriangle->wV2 = centerno + 1;
		lpTriangle->wV3 = centerno + 2;
		setflat = flatnum[ flatcnt ];
		lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
		//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
		skipflag = 1;


		lpTriangle++; trino++; vInPlane++;
		setno += 3;

		// 4点目以降
		for( pointno = 3; pointno < membernum; pointno++ ){
			lpTriangle->wV1 = centerno;
			lpTriangle->wV2 = poffset + setno - 1;
			lpTriangle->wV3 = poffset + setno;

			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_EVEN;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
				//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
			}

			lpTriangle++; trino++;  vInPlane++;
			setno++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}
			
		}
	}

	//TRACE( "MATRIXCPP : SetTridataFan : setno %d  trino %d\n", setno, trino );
}

void	CMatrix::SetTridataBFan( D3DTRIANGLE* lptri, 
			int fannum, int pointnum, int poffset, int* memnum )
{
	int	setno = 0;
	int	fanno, pointno, centerno;
	int	membernum;
	int	trino = 0;

	int	flatnum[ FLATMAX ];
	int	flatcalc, flatno, flatcnt = 0;
	int	setflat, vInPlane;
	int	skipflag;

	LPD3DTRIANGLE		lpTriangle;


	lpTriangle = lptri;

	setno = 0;
	for( fanno = 0; fanno < fannum; fanno++ ){		
		if( (membernum = *(memnum + fanno)) < 3 ){
			TRACE( "SetTridataFan : memnum under 3 !!!!! : exit !!! \n" );
			return;
		}
		

		flatcalc = membernum;
		flatno = 0;
		while( flatcalc > 30 ){
			flatnum[flatno] = 30;
			flatcalc -= 30;
			flatno++;
		}
		flatnum[flatno] = flatcalc;
		
		flatcnt = 0;
		vInPlane = 0;

		centerno = poffset + setno;
		//TRACE( "\nMATRIXCPP : SetTridataFan : poffset %d  centerno %d\n", poffset, centerno );

		// 最初の ３点		
		lpTriangle->wV1 = centerno;
		lpTriangle->wV2 = centerno + 1;
		lpTriangle->wV3 = centerno + 2;
		setflat = flatnum[ flatcnt ];
		lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
		//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
		skipflag = 1;


		lpTriangle++; trino++; vInPlane++;
		setno += 3;

		// 4点目以降
		for( pointno = 3; pointno < membernum; pointno++ ){
			lpTriangle->wV1 = centerno;
			lpTriangle->wV2 = centerno + pointno - 1;
			lpTriangle->wV3 = centerno + pointno;

			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_EVEN;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
				//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
			}

			lpTriangle++; trino++;  vInPlane++;
			setno++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}
		}
	}

	//TRACE( "MATRIXCPP : SetTridataBFan : 表 : setno %d  trino %d\n", setno, trino );

	setno = 0; trino = 0;
	for( fanno = 0; fanno < fannum; fanno++ ){
		
		if( (membernum = *(memnum + fanno)) < 3 ){
			TRACE( "SetTridataFan : memnum under 3 !!!!! : exit !!! \n" );
			return;
		}

		flatcalc = membernum;
		flatno = 0;
		while( flatcalc > 30 ){
			flatnum[flatno] = 30;
			flatcalc -= 30;
			flatno++;
		}
		flatnum[flatno] = flatcalc;
		
		flatcnt = 0;
		vInPlane = 0;
		
		centerno = poffset + setno;
		//TRACE( "\nMATRIXCPP : SetTridataFan : poffset %d  centerno %d\n", poffset, centerno );

		// 最初の ３点		
		lpTriangle->wV1 = centerno;
		lpTriangle->wV2 = centerno + membernum - 1;
		lpTriangle->wV3 = centerno + membernum - 2;
		setflat = flatnum[ flatcnt ];
		lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
		//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
		skipflag = 1;

		lpTriangle++; trino++; vInPlane++;
		setno += 3;

		// 4点目以降
		for( pointno = 2; pointno < membernum - 1; pointno++ ){
			lpTriangle->wV1 = centerno;
			lpTriangle->wV2 = centerno + membernum - pointno;
			lpTriangle->wV3 = centerno + membernum - pointno - 1;
			if( skipflag ){
				lpTriangle->wFlags = D3DTRIFLAG_EVEN;
			}else{
				setflat = flatnum[ flatcnt ];
				lpTriangle->wFlags = D3DTRIFLAG_STARTFLAT(setflat);
				//TRACE( "MATRIXCPP : flatcnt %d  : setflat %d\n", flatcnt, setflat );
			}

			lpTriangle++; trino++;  vInPlane++;
			setno++;
			if( vInPlane % 30 == 0 ){
				skipflag = 0;
				flatcnt++;
			}else{
				skipflag = 1;
			}
		}
	}

}




