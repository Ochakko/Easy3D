//-----------------------------------------------------------------------------
//
// (c) 2002-2003 Ç®ÇøÇ¡Ç±Å@http://www5d.biglobe.ne.jp/~ochikko
//
//-----------------------------------------------------------------------------

#ifndef MYSPRITEH
#define MYSPRITEH

class CMySprite
{
public:
	CMySprite( int* idptr );
	~CMySprite();

	int SetSpriteParams( char* fname, int srctpflag, int srctpr, int srctpg, int srctpb, UINT miplevels, DWORD mipfilter );
	int SetSpriteParams2( BITMAPINFOHEADER* pbi, BYTE* pBit, int srctpflag, int srctpr, int srctpg, int srctpb, UINT miplevels, DWORD mipfilter );

	int RestoreTexture( LPDIRECT3DDEVICE9 pd3dDevice );
	//int InvalidateTexture();
	int Invalidate( int resetflag );

	//int Render( LPD3DXSPRITE pd3dxsprite, int point1, int point2, int pointmax, DWORD resttime, int endflag, CD3DFont* pfont, CD3DFont* pfontbig, HWND apphwnd );
	int Render( LPDIRECT3DDEVICE9 pd3ddev, LPD3DXSPRITE pd3dxsprite, D3DXVECTOR2 scale, D3DXVECTOR3 tra );

	int SetTextureAlpha();

	int SetRotation( D3DXVECTOR2 srccen, float srcdeg );
	int SetARGB( int srca, int srcr, int srcg, int srcb );

	int SetTextureDirtyFlag();


	int GetTextureFromDispObj( int* texidptr );
	int SetTextureToDispObj( int texid );


	int SetBlendingMode( int bmode );


	int SetDefaultRenderState();
	int SetRenderState( int statetype, int value );

	int SetSpriteUV( float srcstartu, float srcendu, float srcstartv, float srcendv );

	int LoadSpriteFromBuf();

private:	
	int SetRenderState( LPDIRECT3DDEVICE9 pdev );


public:
	int serialno;
	char spritename[1024];
	LPDIRECT3DTEXTURE9 pd3dtex;

	int texWidth;
	int texHeight;

	int transparentflag;
	int texbanktransparent;

	int tpr;
	int tpg;
	int tpb;

	CMySprite* next;

	D3DXVECTOR2 rotcen;
	float		rotdeg;
	D3DCOLOR	multcol;

	UINT		m_miplevels;
	DWORD		m_mipfilter;


	int m_renderstate[D3DRS_BLENDOP + 1];
	DWORD m_minfilter;
	DWORD m_magfilter;

	int m_blendmode;

	float startu, endu;
	float startv, endv;

	//LPDIRECT3DTEXTURE9 ptexPB;
	//int texPBWidth;
	//int texPBHeight;

	BITMAPINFOHEADER* m_pbi;
	BYTE*	m_pBit;

};

#endif