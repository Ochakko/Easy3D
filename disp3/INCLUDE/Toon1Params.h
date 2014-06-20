#ifndef TOON1PARAMSH
#define TOON1PARAMSH

#include <D3DX9.h>

class CToon1Params
{
public:
	CToon1Params();
	~CToon1Params();

	int SetDiffuse( D3DCOLORVALUE srcdiff, int overlayflag );
	int CreateTexture( LPDIRECT3DDEVICE9 pdev );

private:
	int InitParams();

public:
	char name[32];

	D3DCOLORVALUE diffuse;
	D3DCOLORVALUE ambient;
	D3DCOLORVALUE specular;

	float darkh;//bmpの塗りつぶし範囲
	float brighth;

	float ambientv;//UVのVのサンプリング位置
	float diffusev;
	float specularv;
	
	float darknl;//法線とライトの内積の閾値
	float brightnl;

	LPDIRECT3DTEXTURE9 tex;

	D3DCOLORVALUE edgecol0;
	char edgevalid0;
	char edgeinv0;
	char reserved1, reserved2;

	float toon0dnl;
	float toon0bnl;

};

#endif