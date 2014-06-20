#ifndef BBOXH
#define BBOXH

#include <D3DX9.h>

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

class CPolyMesh;
class CPolyMesh2;
class CBBox2;

//typedef struct tag_boundarybox
//{
//	int setflag;
//	float minx, maxx;
//	float miny, maxy;
//	float minz, maxz;
//} BOUNDARYBOX;


class CBBox
{

public:
	CBBox();
	~CBBox();

	int InitParams();
	int SetBBox( float srcminx, float srcmaxx, float srcminy, float srcmaxy, float srcminz, float srcmaxz );
	int SetBBox( CPolyMesh* srcpm );
	int SetBBox( CPolyMesh2* srcpm2 );
	int SetBBox( CBBox* srcbbx );
	int SetBBox( CBBox2* srcbbx2 );

	int ChkConflict( CBBox chkbb, D3DXVECTOR3 rate1, D3DXVECTOR3 rate2 );

	int JoinBBox( CBBox* addbb );//bboxの合成。
	int JoinBBox( D3DXVECTOR3* addv );//bboxの更新。
	int JoinBBox( CBBox2* srcbbx2 );

	int TransformOnlyWorld( CBBox* srcbbx, D3DXMATRIX* matWorld );


public:
	int setflag;
	float minx, maxx;
	float miny, maxy;
	float minz, maxz;
};


#endif



