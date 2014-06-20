#ifndef BGDISPH
#define BGDISPH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

class CTexBank;

class CBgDisp
{
public:
	//CBgDisp( char* srctexname );
	CBgDisp( LPDIRECT3DDEVICE9 lpd3dDevice, char* name1, char* name2, float mvu, float mvv, COLORREF srccolor );
	~CBgDisp();

	int SetBgSize( DWORD dwWidth, DWORD dwHeight );
	int MoveTexUV();
	int Render( LPDIRECT3DDEVICE9 pd3dDevice );

	int SetUVAnime( float srcuanime, float srcvanime );

	int UpdateVertexBuffer();

private:
	int Create3DBuffers( LPDIRECT3DDEVICE9 lpd3dDevice );
	int CreateTexture( LPDIRECT3DDEVICE9 lpd3dDevice );
	int Copy2IndexBuffer();
	int Copy2VertexBuffer();

public:
	D3DTLVERTEX2 m_tlv[4];
	WORD	m_Indices[4];
    LPDIRECT3DVERTEXBUFFER9 m_VB;
	LPDIRECT3DINDEXBUFFER9 m_IB;
	CTexBank* m_texbank;	

	char texname[_MAX_PATH];
	char texname2[_MAX_PATH];

	float uanime;
	float vanime;


};
#endif
