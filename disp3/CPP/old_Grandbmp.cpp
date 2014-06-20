#include	"stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#include	"grandbmp.h"
//#ifndef		CALCH
//	#include	"calc.h"
//#endif
#ifndef		PLAYAREAH
	#include	"playarea.h"
#endif

//extern
extern CCalc*	Calc;
extern CPlayArea*	gplayarea;



//////////////////////////////////

CGrandBMP::CGrandBMP( int bmpw, int bmph, int bmpno, char* bmpname )
{
	CFile	bmpfile;
	CFileException e;
	BOOL	ret;

	m_bmpcreated = FALSE;


	m_bmpnow = bmpw; m_bmpnoh = bmph; m_bmpno = bmpno;

	m_pBMI = NULL;
	m_pBits = NULL;
	m_bitdepth = 0;
	m_palnum = 0;


	int	findleng;
	char	texdir[50];
	char	tempchar[500];

	//int	bmpw, bmph, bmpno;

	memset( texdir, 0, 50 );
	memset( m_filename, 0, 500 );

	findleng = GetEnvironmentVariable( "TEXDIR", texdir, 50 );
	if( !findleng ){
		TRACE( "GRANDBMP : TEXDIR  directory var not found !!! error !!!\n" );
		return;
	}

	memset( tempchar, 0, 500 );
	sprintf( tempchar, "%s%d_%d.bmp", bmpname, m_bmpnow, m_bmpnoh );
	strcpy( m_filename, texdir );
	strcat( m_filename, tempchar );
	

	if( !bmpfile.Open( m_filename, CFile::modeRead, &e ) )
	{
		TRACE( "CGrandBMP : file open err\n" );
		return;
	}

	ret = Load( &bmpfile );
	if( !ret ){
		TRACE( "CGrandBMP : Load error!!!!!! \n" );
		bmpfile.Close();
		return;
	}else{
		m_bmpcreated = TRUE;
	}


	bmpfile.Close();

	//TRACE( "CGrandBMP : width %d  height %d\n", DibWidth(), DibHeight() );


// for debug
	RGBQUAD*	palptr;
	palptr = GetClrTabAddress();
	int	palno;
	int	ired, igreen, iblue;
	for( palno = 0; palno < m_palnum; palno++ ){
		ired = (palptr + palno)->rgbRed;		
		igreen = (palptr + palno)->rgbGreen;
		iblue = (palptr + palno)->rgbBlue;

		TRACE( "GrandBMP %d : palno %d : r %d g %d b %d\n", 
			m_bmpno, palno, ired, igreen, iblue );
	}

}


CGrandBMP::~CGrandBMP()
{

	if( m_pBMI ){
		free( m_pBMI );
		m_pBMI = 0;
	}

	if( m_pBits ){
		free( m_pBits );
		m_pBits = 0;
	}
}

BOOL	CGrandBMP::Load( CFile*	fp )
{
	BOOL	bIsPM = FALSE;
	BITMAPINFO* pBmpInfo = NULL;
	BYTE*	pBits = NULL;
	int iBytes;

	DWORD	dwFileStart = fp->GetPosition();

	BITMAPFILEHEADER BmpFileHdr;
	iBytes = fp->Read( &BmpFileHdr, sizeof( BmpFileHdr ) );

	if( BmpFileHdr.bfType != 0x4D42 ){
		TRACE( "MAGIC NUMBER IS NOT BM\n" );
		goto $abort;
	}

	BITMAPINFOHEADER	BmpInfoHdr;
	iBytes = fp->Read( &BmpInfoHdr, sizeof( BmpInfoHdr ) );

	if( BmpInfoHdr.biSize != sizeof( BITMAPINFOHEADER ) ){
		// convert PM-> WIN DIB

		bIsPM = TRUE;
		TRACE( "file format : PM!!!\n" );

		fp->Seek( dwFileStart + sizeof(BITMAPFILEHEADER), CFile::begin );
		BITMAPCOREHEADER BmpCoreHdr;
		iBytes = fp->Read( &BmpCoreHdr, sizeof( BmpCoreHdr ) );

		BmpInfoHdr.biSize = sizeof( BITMAPINFOHEADER );
		BmpInfoHdr.biWidth = ( int ) BmpCoreHdr.bcWidth;
		BmpInfoHdr.biHeight = ( int ) BmpCoreHdr.bcHeight;
		BmpInfoHdr.biPlanes = BmpCoreHdr.bcPlanes;
		BmpInfoHdr.biCompression = BI_RGB;
		BmpInfoHdr.biSizeImage = 0;
		BmpInfoHdr.biXPelsPerMeter = 0;
		BmpInfoHdr.biYPelsPerMeter = 0;
		BmpInfoHdr.biClrUsed = 0;
		BmpInfoHdr.biClrImportant = 0;
	}

	int iColors;
	int iColorTableSize;
	iColors = NumDIBColorEntries( (LPBITMAPINFO) &BmpInfoHdr );
	iColorTableSize = 256 * sizeof( RGBQUAD );
	int iBitsSize;
	int iBISize;
	iBISize = sizeof( BITMAPINFOHEADER ) + iColorTableSize;
	iBitsSize = BmpFileHdr.bfSize - BmpFileHdr.bfOffBits;

	pBmpInfo = ( LPBITMAPINFO ) malloc( iBISize );

	memcpy( pBmpInfo, &BmpInfoHdr, sizeof( BITMAPINFOHEADER ) );

	//check bmpsize
	if( BmpInfoHdr.biWidth != BMPW || BmpInfoHdr.biHeight != BMPH ){
		TRACE( "GRANDBMP : LOAD : BMPSIZE error !!!!!\n"
			"	: BMPW %d  biWidth %d : BMPH %d  biHeight %d\n",
			BMPW, BmpInfoHdr.biWidth, BMPH, BmpInfoHdr.biHeight );
		goto $abort;
	}
	//m_sizew = BmpInfoHdr.biWidth;
	//m_sizeh = BmpInfoHdr.biHeight;


	if( bIsPM == FALSE ){
		iBytes = fp->Read( (LPBYTE) pBmpInfo + sizeof( BITMAPINFOHEADER ),
			iColorTableSize );
	}else{
		LPRGBQUAD lpRGB;
		lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof( BITMAPINFOHEADER ) );

		int i;
		RGBTRIPLE rgbt;
		for( i = 0; i < iColors; i++ ){
			iBytes = fp->Read( &rgbt, sizeof( RGBTRIPLE ) );
			lpRGB->rgbBlue = rgbt.rgbtBlue;
			lpRGB->rgbGreen = rgbt.rgbtGreen;
			lpRGB->rgbRed = rgbt.rgbtRed;
			lpRGB->rgbReserved = 0;
			lpRGB++;
		}
	}

	pBits = (BYTE*)malloc( iBitsSize );
	if( !pBits ){
		TRACE( "pBits malloc error\n" );
		goto $abort;
	}

	fp->Seek( dwFileStart + BmpFileHdr.bfOffBits, CFile::begin );

	iBytes = fp->Read( pBits, iBitsSize );

	if( m_pBMI != NULL )	free( m_pBMI );
	m_pBMI = pBmpInfo;
	if( m_pBits != NULL )	free( m_pBits );
	m_pBits = pBits;

	return TRUE;

$abort:
	if( pBmpInfo ) free( pBmpInfo );
	if( pBits ) free( pBits );
	return FALSE;

}

BOOL CGrandBMP::IsWinDIB( BITMAPINFOHEADER* pBIH)
{
	ASSERT( pBIH );
	if(((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof( BITMAPCOREHEADER ) ){
		return FALSE;
	}
	return TRUE;
}

int	CGrandBMP::NumDIBColorEntries( LPBITMAPINFO pBmpInfo )
{
	BITMAPINFOHEADER* pBIH;
	BITMAPCOREHEADER* pBCH;
	int	iColors, iBitCount;

	ASSERT( pBmpInfo );

	pBIH = &(pBmpInfo->bmiHeader);
	pBCH = (BITMAPCOREHEADER*) pBIH;

	if( IsWinDIB( pBIH ) ){
		iBitCount = pBIH->biBitCount;
	}else{
		iBitCount = pBCH->bcBitCount;
	}

	switch( iBitCount ){
	case 1:
		iColors = 2;
		break;
	case 4:
		iColors = 16;
		break;
	case 8:
		iColors = 256;
		break;
	default:
		iColors = 0;
		break;
	}

	if( IsWinDIB( pBIH ) && ( pBIH->biClrUsed != 0 ) ){
		iColors = pBIH->biClrUsed;
	}

	m_bitdepth = iBitCount;
	m_palnum = iColors;

	TRACE( "GrandBMP : iBitCount %d iColors %d\n", iBitCount, iColors );

	return iColors;

}

BOOL	CGrandBMP::SetGrandVertex( D3DTLVERTEX* lpvert, int* lph, int* lptexkind )
{
	int	startw, starth;
	D3DTLVERTEX*	setptr;
	int*			sethptr;
	int*			setkind = lptexkind;


	int	wno, hno;
	DWORD	bmpdatano, setdatano;

	BYTE*	bitptr;
	RGBQUAD*	tabptr;
	int	tableno, igreen, midgreen, maxgreen;
	float	midgreendiv, maxgreendiv;
	INDEX_L	index, gindex;
	LOCATION	loc;

	int	hpoint0, hpoint1, hpoint2;

	tabptr = GetClrTabAddress();
	bitptr = GetBitsAddress();

	maxgreen = 1 << m_bitdepth;
	midgreen = maxgreen >> 1; // max の　半分
	if( maxgreen == 1 ){
		TRACE( "GRANDBMP : midgreen error!!!!!!!\n" );
		return FALSE;
	}else{
		maxgreendiv = (float)(1.0 / (double)maxgreen);
		midgreendiv = (float)(1.0 / (double)midgreen);
	}

	//texkind 境界設定
	//hpoint0 = GMAXINDEXY / 2;
	hpoint0 = (int)((float)GMAXINDEXY / 2.0 * maxgreendiv) - 2; 
	hpoint1 = hpoint0 + 1;
	hpoint2 = hpoint1 + 2;

	TRACE( "GRANDBMP : tex height 0-%d 1-%d 2-%d\n", hpoint0, hpoint1, hpoint2 );

	startw = m_bmpnow * BMPW;
	starth = m_bmpnoh * BMPH;

	// green 成分が　vertex の　y 座標
	// w : x, h : z
	for( hno = 0; hno < BMPH; hno++ ){
		for( wno = 0; wno < BMPW; wno++ ){
			bmpdatano = hno * BMPW + wno;

			tableno = *( bitptr	+ bmpdatano );
			if( tableno < 0 || tableno >= m_palnum ){
				TRACE( "GRANDCPP : SetGrandVertex : palno error!!! %d\n", tableno );
			}

			setdatano = GMAXINDEXX * ( starth + hno ) + startw + wno;
			setptr = lpvert + setdatano;
			sethptr = lph + setdatano;


			igreen = ( tabptr + tableno )->rgbGreen;
			gindex.x = startw + wno;
			gindex.y = (int)((float)GMAXINDEXY * maxgreendiv * (float)( igreen - midgreen ) );
			gindex.z = starth + hno;

			//set tex kind
			if( gindex.y <= hpoint0 ){
				*setkind = 3;
			}else if( gindex.y <= hpoint1 ){
				*setkind = 2;
			}else if( gindex.y <= hpoint2 ){
				*setkind = 1;
			}else{
				*setkind = 0;
			}

			gplayarea->GIndex2Index( &index, gindex );
			*sethptr = index.y;
			

			gplayarea->Index2Loc( &loc, &index );		

			setptr->sx = loc.x;
			setptr->sy = loc.y;
			setptr->sz = loc.z;

			setkind++;
		}
	}

	return TRUE;
	// 法線ベクトルの計算は　BMP の　境界値も使うので　MAPDATA で行う
	//return SetNVector( lpvert );

}






