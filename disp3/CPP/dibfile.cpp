#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <wingdi.h>
#include <dibfile.h>
#include <D3DX9.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


#include <vfw.h>


CDibFile::CDibFile( char* fname, int sizewidth, int sizeheight, int srcbpp )
{
	if( srcbpp != 24 ){
		_ASSERT( 0 );
	}

	//DWORD linewidth;
	//DWORD sizeimage;

	linewidth = (( sizewidth * 24 + 31 ) & ~31 ) / 8;
	sizeimage = linewidth * sizeheight;


	ZeroMemory( &hdr, sizeof( BITMAPFILEHEADER ) );
	ZeroMemory( &biheader, sizeof( BITMAPINFOHEADER ) );
	ZeroMemory( filename, 1024 );

	//
	hdr.bfType = 0x4D42; // BM
	//hdr.bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + 
	//	sizeof( RGBDAT ) * sizewidth * sizeheight;
	hdr.bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + 
		sizeimage;


	hdr.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	//
	/***
	//hdr.bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFO ) + 
	//	sizeof( RGBDAT ) * sizewidth * sizeheight;
	//hdr.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFO );
	bi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bi.bmiHeader.biWidth = sizewidth;
	bi.bmiHeader.biHeight = sizeheight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = 0;
	bi.bmiHeader.biSizeImage = sizeof( RGBDAT ) * sizewidth * sizeheight;//uncompressedÇ»èÍçáÇÕÅAÇOÇ‡â¬ÅB
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	***/
	//bi.bmiColors = NULL;

/***
#define LINE ((((WIDTH)*24+31)&~31)/8)
#define SIZEIMAGE (LINE*(HEIGHT))
BITMAPINFOHEADER bmih={sizeof(BITMAPINFOHEADER),WIDTH,HEIGHT,1,24,BI_RGB,
                                                            SIZEIMAGE,0,0,0,0};
BYTE bBit[SIZEIMAGE];
***/
	biheader.biSize = sizeof( BITMAPINFOHEADER );
	biheader.biWidth = sizewidth;
	biheader.biHeight = sizeheight;
	biheader.biPlanes = 1;
	biheader.biBitCount = 24;
	biheader.biCompression = BI_RGB;
	biheader.biSizeImage = sizeimage;//uncompressedÇ»èÍçáÇÕÅAÇOÇ‡â¬ÅB
	biheader.biXPelsPerMeter = 0;
	biheader.biYPelsPerMeter = 0;
	biheader.biClrUsed = 0;
	biheader.biClrImportant = 0;

	int leng;
	leng = (int)strlen( fname );

	if( leng < 1019 ){
		strncpy_s( filename, 1020, fname, leng );
		strcat_s( filename, 1024, ".bmp" );
	}else{
		strncpy_s( filename, fname, 1019 );
		strcat_s( filename, 1024, ".bmp" );
	}
	rgbdat = (RGBDAT*)malloc( sizeimage );
	if( !rgbdat )
		return;
	ZeroMemory( rgbdat, sizeimage );

	//MoveMemory( rgbdat, rgbptr, sizeof( RGBDAT ) * sizewidth * sizeheight );

	m_pbuf = 0;
	m_bufsize = 0;

}

CDibFile::CDibFile( unsigned char* pbuf, int bufsize, int* dstflag )
{
	*dstflag = 0;

	m_pbuf = pbuf;
	m_bufsize = bufsize;

	int hleng1, hleng2;
	hleng1 = sizeof( BITMAPFILEHEADER );
	hleng2 = sizeof( BITMAPINFOHEADER );
	if( m_bufsize <= (hleng1 + hleng2) ){
		*dstflag = 0;
		return;
	}
	int pos = 0;
	MoveMemory( &hdr, m_pbuf, hleng1 );
	MoveMemory( &biheader, m_pbuf + hleng1, hleng2 );


	linewidth = biheader.biWidth;
	sizeimage = biheader.biSizeImage;

	*dstflag = 1;
}


CDibFile::~CDibFile()
{
	if( rgbdat ){
		free( rgbdat );
		rgbdat = 0;
	}
}
int CDibFile::SetRGB( int lineno, BYTE* srcptr, int type )
{
	DWORD* dwbits;
	WORD* wbits;
	//int linemax = bi.bmiHeader.biHeight;
	int linemax = biheader.biHeight;
	if( lineno >= linemax ){
		_ASSERT( 0 );
		return 1;
	}
	//int width = bi.bmiHeader.biWidth;
	int width = biheader.biWidth;
	int wno;
	RGBDAT* dstrgb;
	float rate, rate1, rate2;
	DWORD srcr, srcg, srcb;
	DWORD dstr, dstg, dstb;
	for( wno = 0; wno < width; wno++ ){
		//dstrgb = rgbdat + width * lineno + wno;

		dstrgb = (RGBDAT*)((BYTE*)rgbdat + linewidth * lineno) + wno;

		switch( type ){
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
			//DbgOut( "DibFile : SetRGB : format X1R5G5B5\n" );
			wbits = (WORD*)srcptr + wno;
			srcr = (*wbits & 0x7C00) >> 10;
			srcg = (*wbits & 0x03E0) >> 5;
			srcb = (*wbits & 0x001F);

			rate = 255.0f / 31.0f;
			dstr = (DWORD)((float)srcr * rate);
			if( dstr > 255 )
				dstr = 255;
			dstg = (DWORD)((float)srcg * rate);
			if( dstg > 255 )
				dstg = 255;
			dstb = (DWORD)((float)srcb * rate);
			if( dstb > 255 )
				dstb = 255;
			dstrgb->r = (unsigned char)dstr;
			dstrgb->g = (unsigned char)dstg;
			dstrgb->b = (unsigned char)dstb;
			break;

		case D3DFMT_R5G6B5:
			//DbgOut( "DibFile : SetRGB : format R5G6B5\n" );
			wbits = (WORD*)srcptr + wno;
			srcr = (*wbits & 0xF800) >> 11;
			srcg = (*wbits & 0x07E0) >> 5;
			srcb = (*wbits & 0x001F);
			rate1 = 255.0f / 31.0f;
			rate2 = 255.0f / 63.0f;
			dstr = (DWORD)((float)srcr * rate1);
			if( dstr > 255 )
				dstr = 255;
			dstg = (DWORD)((float)srcg * rate2);
			if( dstg > 255 )
				dstg = 255;
			dstb = (DWORD)((float)srcb * rate1);
			if( dstb > 255 )
				dstb = 255;
			dstrgb->r = (unsigned char)dstr;
			dstrgb->g = (unsigned char)dstg;
			dstrgb->b = (unsigned char)dstb;
			
			break;
		case D3DFMT_X8R8G8B8:
			//DbgOut( "DibFile : SetRGB : format X8R8G8B8\n" );
		case D3DFMT_A8R8G8B8:
			//DbgOut( "DibFile : SetRGB : format A8R8G8B8\n" );
		case D3DFMT_R8G8B8:

			dwbits = (DWORD*)srcptr + wno;
			srcr = (*dwbits & 0x00FF0000) >> 16;
			srcg = (*dwbits & 0x0000FF00) >> 8;
			srcb = (*dwbits & 0x000000FF);
			dstrgb->r = (unsigned char)srcr;
			dstrgb->g = (unsigned char)srcg;
			dstrgb->b = (unsigned char)srcb;

			break;
		default:
			_ASSERT( 0 );
			return 1;
			break;
		}
	}

	return 0;
}

int CDibFile::GetRGBOfBuf( int w, int h, RGBDAT* dstrgb )
{
	if( !m_pbuf ){
		_ASSERT( 0 );
		return 1;
	}

	int hleng1, hleng2;
	hleng1 = sizeof( BITMAPFILEHEADER );
	hleng2 = sizeof( BITMAPINFOHEADER );
	if( m_bufsize <= (hleng1 + hleng2) ){
		_ASSERT( 0 );
		return 1;
	}

	int datapos = 0;
	datapos = hleng1 + hleng2 + ( h * biheader.biWidth + w ) * sizeof( RGBDAT );
	if( m_bufsize < (datapos + sizeof( RGBDAT )) ){
		_ASSERT( 0 );
		return 1;
	}

	*dstrgb = *( (RGBDAT*)( m_pbuf + datapos ) );

	return 0;
}

/***
int CDibFile::SetRGB( int lineno, BYTE* srcptr, int type )
{
	DWORD* dwbits;
	WORD* wbits;
	//int linemax = bi.bmiHeader.biHeight;
	int linemax = biheader.biHeight;
	if( lineno >= linemax ){
		_ASSERT( 0 );
		return 1;
	}
	//int width = bi.bmiHeader.biWidth;
	int width = biheader.biWidth;
	int wno;
	RGBDAT* dstrgb;
	float rate, rate1, rate2;
	DWORD srcr, srcg, srcb;
	DWORD dstr, dstg, dstb;
	for( wno = 0; wno < width; wno++ ){
		dstrgb = rgbdat + width * lineno + wno;

		switch( type ){
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
			//DbgOut( "DibFile : SetRGB : format X1R5G5B5\n" );
			wbits = (WORD*)srcptr + wno;
			srcr = (*wbits & 0x7C00) >> 10;
			srcg = (*wbits & 0x03E0) >> 5;
			srcb = (*wbits & 0x001F);

			rate = 255.0f / 31.0f;
			dstr = (DWORD)((float)srcr * rate);
			if( dstr > 255 )
				dstr = 255;
			dstg = (DWORD)((float)srcg * rate);
			if( dstg > 255 )
				dstg = 255;
			dstb = (DWORD)((float)srcb * rate);
			if( dstb > 255 )
				dstb = 255;
			dstrgb->r = (unsigned char)dstr;
			dstrgb->g = (unsigned char)dstg;
			dstrgb->b = (unsigned char)dstb;
			break;

		case D3DFMT_R5G6B5:
			//DbgOut( "DibFile : SetRGB : format R5G6B5\n" );
			wbits = (WORD*)srcptr + wno;
			srcr = (*wbits & 0xF800) >> 11;
			srcg = (*wbits & 0x07E0) >> 5;
			srcb = (*wbits & 0x001F);
			rate1 = 255.0f / 31.0f;
			rate2 = 255.0f / 63.0f;
			dstr = (DWORD)((float)srcr * rate1);
			if( dstr > 255 )
				dstr = 255;
			dstg = (DWORD)((float)srcg * rate2);
			if( dstg > 255 )
				dstg = 255;
			dstb = (DWORD)((float)srcb * rate1);
			if( dstb > 255 )
				dstb = 255;
			dstrgb->r = (unsigned char)dstr;
			dstrgb->g = (unsigned char)dstg;
			dstrgb->b = (unsigned char)dstb;
			
			break;
		case D3DFMT_X8R8G8B8:
			//DbgOut( "DibFile : SetRGB : format X8R8G8B8\n" );
		case D3DFMT_A8R8G8B8:
			//DbgOut( "DibFile : SetRGB : format A8R8G8B8\n" );
		case D3DFMT_R8G8B8:

			dwbits = (DWORD*)srcptr + wno;
			srcr = (*dwbits & 0x00FF0000) >> 16;
			srcg = (*dwbits & 0x0000FF00) >> 8;
			srcb = (*dwbits & 0x000000FF);
			dstrgb->r = (unsigned char)srcr;
			dstrgb->g = (unsigned char)srcg;
			dstrgb->b = (unsigned char)srcb;

			break;
		default:
			_ASSERT( 0 );
			return 1;
			break;
		}
	}

	return 0;
}
***/

int CDibFile::CreateDibFile()
{
	HANDLE hfile;
	unsigned long wleng, writeleng;

	hfile = CreateFile( filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "CDibFile : CreateDibFile : CreateFile error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	

	wleng = sizeof( BITMAPFILEHEADER );
	WriteFile( hfile, &hdr, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CDibFile : CreateDibFile : WriteFile hdr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//wleng = sizeof( BITMAPINFO );
	//WriteFile( hfile, &bi, wleng, &writeleng, NULL );
	//if( wleng != writeleng ){
	//	DbgOut( "CDibFile : CreateDibFile : WriteFile bi error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}
	wleng = sizeof( BITMAPINFOHEADER );
	WriteFile( hfile, &biheader, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CDibFile : CreateDibFile : WriteFile bi error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//wleng = sizeof( RGBDAT ) * bi.bmiHeader.biWidth * bi.bmiHeader.biHeight;
	wleng = biheader.biSizeImage;
	WriteFile( hfile, rgbdat, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		DbgOut( "CDibFile : CreateDibFile : WriteFile rgbdat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	FlushFileBuffers( hfile );
	SetEndOfFile( hfile );
	CloseHandle( hfile );
	
	return 0;
}
BITMAPINFOHEADER* CDibFile::GetBMPInfoHeader()
{
	return &biheader;
}

BYTE* CDibFile::GetBMPData()
{
	return (BYTE*)rgbdat;
}


