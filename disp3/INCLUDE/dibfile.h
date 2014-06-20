#ifndef DIBFILEH
#define DIBFILEH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>


#define DBGH
#include <dbg.h>


// パレットなしdib専用。
// 24bpp。

class CDibFile
{
public:
	CDibFile( char* fname, int sizewidth, int sizeheight, int srcbpp );
	CDibFile( unsigned char* pbuf, int bufsize, int* dstflag );
	~CDibFile();

	int SetRGB( int lineno, BYTE* srcptr, int type );
	int CreateDibFile();

	BITMAPINFOHEADER* GetBMPInfoHeader();
	BYTE* GetBMPData();

	int GetRGBOfBuf( int w, int h, RGBDAT* dstrgb );

public:
	char filename[1024];
	BITMAPFILEHEADER hdr;
	//BITMAPINFO bi;//<------ パレット有りの場合のみ、使用する。
	BITMAPINFOHEADER biheader;
	
	RGBDAT* rgbdat;

	DWORD linewidth;
	DWORD sizeimage;

	unsigned char* m_pbuf;
	int m_bufsize;

};

#endif