#ifndef XFONTH
#define	XFONTH


#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>

#include <D3DX9.h>

#include "D3DFont.h" // !!!! 注意、disp0\includeにコピーした、d3dfont.hを使用する。

#define DBGH
#include <dbg.h>

class CXFont
{
public:
	CXFont();
	~CXFont();

	int CreateFont( LPDIRECT3DDEVICE9 pdev, LPD3DXSPRITE psprite, int height, int width, int weight, 
		int bItalic, int bUnderline, int bStrikeout,
		char* fontname );

	int DrawText( char* textstr, int posx, int posy, D3DCOLOR col, int blendmode = -1 );
	int DrawTextWithCnt( char* textstr, int posx, int posy, D3DCOLOR col, int eachcnt, int curcnt );
	int DrawTextWithCnt2( char* textstr, int posx, int posy, D3DCOLOR col, int* cntarray, int arrayleng, int curcnt );

	int DestroyFont();

	int AddToPrev( CXFont* addfont );
	int AddToNext( CXFont* addfont );
	int LeaveFromChain();


	int Invalidate();
	int Restore( LPDIRECT3DDEVICE9 pdev );

	int CalcStrSize( char* textstr, SIZE* sizeptr );

private:
	int InitParams();
	int DestroyObjs();


	int GetStrideArray( char* textstr, int maxlen, int* stridearray, int* mcnum );
	int CopyTextWithCnt( char* textptr, int textleng, char* orgptr, int* cntarray, int arrayleng, int curcnt );

public:
	LPDIRECT3DDEVICE9 m_pdev;
	LPD3DXSPRITE m_psprite;
	int m_serialno;

	int m_height;
	int m_width;
	int m_weight;
	int m_Italic;
	int m_Underline;
	int m_Strikeout;

	char m_fontname[1024];

	
	LPD3DXFONT	m_lpd3dxFont;
	RECT	m_rect;


	CXFont* prev;
	CXFont* next;
	int ishead;

};

#endif