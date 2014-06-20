#ifndef TEXBANKH
#define TEXBANKH


#include <stdio.h>
#include <D3DX9.h>
#include <crtdbg.h>

#include <namebank.h>

typedef struct tag_DataFromBuf
{
	char* pdata;
	int leng;
} DATAFROMBUF;

class CPanda;

class CTexBank : public CNameBank
{
public:
	CTexBank( UINT miplevels, DWORD mipfilter );
	~CTexBank();

	int CreateTexData( int datano, LPDIRECT3DDEVICE9 pdev, int srcsizex = 0, int srcsizey = 0 );
	int CreateTexDataFromMainWindow( int datano, LPDIRECT3DDEVICE9 pdev );
	int LoadTextureFromBuf( int srcindex, D3DCOLOR colkey );


	int SetTexData( int datano, LPDIRECT3DTEXTURE9 lptex );
	int SetBufData( int srcindex, char* bufptr, int bufleng );
	int SetBufData2( int srcindex, BITMAPINFOHEADER* pbi, BYTE* pBit );
	int SetBufDataFromPnd( int srcindex, CPanda* panda, int pndid, char* texpath );


	LPDIRECT3DTEXTURE9 GetTexData( char* srcname, int srctransparent );
	LPDIRECT3DSURFACE9 GetSurface( int srcindex, int* textype );

	//LPDIRECT3DTEXTURE9 GetTexDataBySerial( int srcserial );

	int AddName( char* dirname, char* srcname, int srctransparent = 0, int srcpool = D3DPOOL_MANAGED );

	int InvalidateTexture( char* srcname, int srctrans, int resetflag = 0 );
	int InvalidateTextureIfNotDirty( char* srcname, int srctrans, int resetflag );
	int InvalidatePoolDefault();
	int RestorePoolDefault( LPDIRECT3DDEVICE9 pdev );


	int DestroyNotDirtyTexture( int saveusers );
	int DestroyTexture( int destroyindex, int renameflag = 1 );
	int CopyTexture2Texture( int srctexid, int dsttexid );
	int CopyTextureToBackBuffer( LPDIRECT3DDEVICE9 pdev, int texid );
	int RasterScrollTexture( LPDIRECT3DDEVICE9 pdev, int srctexid, int desttexid, int t, float fparam1, float fparam2 );
							 
	int InvColTexture( LPDIRECT3DDEVICE9 pdev, int srctexid );
	
	int GetTexNoByName( char* srcname, int srctrans );
	int GetTextureInfoBySerial( int texid, int* widthptr, int* heightptr, int* poolptr, int* transparentptr, int* formatptr );
	int GetTexDataBySerial( int texid, LPDIRECT3DTEXTURE9* pptex );

	int GetTexSize( int srcindex, int* dstx, int* dsty );
	int GetTexOrgSize( int srcindex, int* dstx, int* dsty );

	int SetDirtyFlag( char* srcname, int srcint );

	int SetAppTexture( char* dstname, int dsttransparent, int srctexid );


private:
	int DestroyTexData();
	int InvalidateTexture( int delindex );
	
	int GetBackSurfaceDesc( LPDIRECT3DDEVICE9 pdev, D3DSURFACE_DESC* pdesc );
	//int CopySurface2Surface( RECT cprect, D3DLOCKED_RECT srcrect, D3DFORMAT srcformat, D3DLOCKED_RECT dstrect, D3DFORMAT dstformat );
	//int CopySurface2BackBuffer( RECT cprect, D3DLOCKED_RECT srcrect, D3DFORMAT srcformat, D3DLOCKED_RECT dstrect, D3DFORMAT dstformat );


	int SetTextureAlpha( LPDIRECT3DTEXTURE9 pd3dtex );

	__inline int ConvR5G6B5toA8R8G8B8( WORD* srchead16, DWORD* dsthead32 )
	{
		unsigned char srcr, srcg, srcb;
		unsigned char dsta, dstr, dstg, dstb;

		srcr = (*srchead16 & 0xF800) >> 11;
		srcg = (*srchead16 & 0x07E0) >> 5;
		srcb = (*srchead16 & 0x001F);

		dsta = 0xFF;
		dstr = srcr << 3;
		dstg = srcg << 2;
		dstb = srcb << 3;

		*dsthead32 = (dsta << 24) | (dstr << 16) | (dstg << 8) | dstb;

		return 0;
	};
	__inline int Conv1R5G5B5toA8R8G8B8( WORD* srchead16, DWORD* dsthead32 )
	{
		unsigned char srca, srcr, srcg, srcb;
		unsigned char dsta, dstr, dstg, dstb;

		static unsigned char alpha[2] = { 0, 255 };

		srca = (*srchead16 & 0x8000) >> 15;
		srcr = (*srchead16 & 0x7C00) >> 10;
		srcg = (*srchead16 & 0x03E0) >> 5;
		srcb = (*srchead16 & 0x001F);

		dsta = alpha[ srca ];
		dstr = srcr << 3;
		dstg = srcg << 3;
		dstb = srcb << 3;

		*dsthead32 = (dsta << 24) | (dstr << 16) | (dstg << 8) | dstb;

		return 0;
	};


	__inline int ConvA8R8G8B8toR5G6B5( DWORD* srchead32, WORD* dsthead16 )
	{
		unsigned char srcr, srcg, srcb;
		WORD dstr, dstg, dstb;

		//srca = (*srchead32 & 0xFF000000) >> 24;
		srcr = (unsigned char)(*srchead32 & 0x00FF0000) >> 16;
		srcg = (unsigned char)(*srchead32 & 0x0000FF00) >> 8;
		srcb = (unsigned char)(*srchead32 & 0x000000FF);

		dstr = (srcr >> 3) << 11;
		dstg = (srcg >> 2) << 5;
		dstb = srcb >> 3;

		*dsthead16 = dstr | dstg | dstb;

		return 0;
	}
	__inline int ConvA8R8G8B8to1R5G5B5( DWORD* srchead32, WORD* dsthead16 )
	{
		unsigned char srca, srcr, srcg, srcb;
		WORD dsta, dstr, dstg, dstb;

		srca = (unsigned char)(*srchead32 & 0xFF000000) >> 24;
		srcr = (unsigned char)(*srchead32 & 0x00FF0000) >> 16;
		srcg = (unsigned char)(*srchead32 & 0x0000FF00) >> 8;
		srcb = (unsigned char)(*srchead32 & 0x000000FF);

		dsta = (srca != 0) << 15;
		dstr = (srcr >> 3) << 10;
		dstg = (srcg >> 3) << 5;
		dstb = srcb >> 3;

		*dsthead16 = dsta | dstr | dstg | dstb;

		return 0;
	}


public:
	LPDIRECT3DTEXTURE9* pptexdata;
	LPDIRECT3DSURFACE9* ppsurfdata;
	DATAFROMBUF* ppbufdata;
	char** ppdirname;

	int* ptexapplyindex;

	UINT m_miplevels;
	DWORD m_mipfilter;

};

#endif