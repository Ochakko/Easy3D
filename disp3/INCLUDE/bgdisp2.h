#ifndef BGDISP2H
#define BGDISP2H

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

class CTexBank;

#define	BGUNUM	3
#define BGVNUM	3

#define BGTLVNUM	(BGUNUM * BGVNUM)
#define BGINDEXNUM	((BGUNUM - 1) * 2 * (BGVNUM - 1) * 3)

class CBgDisp2
{
public:
	//CBgDisp2( char* srctexname );
	CBgDisp2( int resetflag, LPDIRECT3DDEVICE9 lpd3dDevice, char* name1, int trans1, char* name2, int trans2, float mvu, float mvv, 
		COLORREF srccolor, int srcisround, float fogdist, UV startuv, UV enduv, int srcfixsize );
	~CBgDisp2();

	int SetBgSize( DWORD dwWidth, DWORD dwHeight );
	int MoveTexUV( D3DXMATRIX matView );
	int Render( LPDIRECT3DDEVICE9 pd3dDevice );

	int SetUVAnime( float srcuanime, float srcvanime );

	int UpdateVertexBuffer();
	int SetU( float startu, float endu );
	int SetV( float startv, float endv );
	int SetTextureDirtyFlag();
	int CreateTexture( LPDIRECT3DDEVICE9 lpd3dDevice );

private:
	int Create3DBuffers( LPDIRECT3DDEVICE9 lpd3dDevice );
	int Copy2IndexBuffer();
	int Copy2VertexBuffer();

	int CheckMultiTexOk( LPDIRECT3DDEVICE9 pd3ddev );

public:
	D3DTLVERTEX m_tlv1[BGTLVNUM];// multitexok == 0 ‚Ì‚Æ‚«
	D3DTLVERTEX2 m_tlv2[BGTLVNUM];// multitexok == 1 ‚Ì‚Æ‚«

	WORD	m_Indices[BGINDEXNUM];
    LPDIRECT3DVERTEXBUFFER9 m_VB;
	LPDIRECT3DINDEXBUFFER9 m_IB;
	CTexBank* m_texbank;	

	char texname[_MAX_PATH];
	char texname2[_MAX_PATH];

	float uanime;
	float vanime;

	int isround;

	int multitexok;
	
	COLORREF col;

	float m_fogdist;

	DWORD m_minfilter;
	DWORD m_magfilter;


	UV m_startuv;
	UV m_enduv;

	int fixsize;
	int fixx, fixy;

	int transparent1;
	int transparent2;
};
#endif
