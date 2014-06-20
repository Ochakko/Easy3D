#ifndef DIBFILEH
#define DIBFILEH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>


#define DBGH
#include <dbg.h>


// �p���b�g�Ȃ�dib��p�B
// 24bpp�B

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
	//BITMAPINFO bi;//<------ �p���b�g�L��̏ꍇ�̂݁A�g�p����B
	BITMAPINFOHEADER biheader;
	
	RGBDAT* rgbdat;

	DWORD linewidth;
	DWORD sizeimage;

	unsigned char* m_pbuf;
	int m_bufsize;

};

#endif