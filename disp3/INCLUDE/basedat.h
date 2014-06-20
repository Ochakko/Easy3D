#ifndef BASEDATH
#define BASEDATH

//#include "charpos.h"
//#include <d3drmwin.h>
#include <D3DX9.h>

#include <matrix2.h> // shade plugin sdk からmat4をもらいマチタ。(CMatrixとは掛ける方向が逆、あとで変える)

/***
typedef struct rgbdat
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RGBDAT;
***/
typedef struct tag_distsample
{
	int setflag;
	float dist;
	D3DXVECTOR3 nv;
} DISTSAMPLE;


typedef struct tag_confdata
{
	int partsno;
	int faceno;
	D3DXVECTOR3 adjustv;
	D3DXVECTOR3 nv;
} CONFDATA;


typedef struct rgbdat
{
	//unsigned char a;
	unsigned char b;
	unsigned char g;
	unsigned char r;
} RGBDAT;

typedef struct rgb3f
{
	float r;
	float g;
	float b;
} RGB3F;

typedef struct tag_argbfdat
{
	float a;
	float r;
	float g;
	float b;
} ARGBF;

typedef struct tag_coordinate
{
	float u;
	float v;
} COORDINATE;


class	CBaseDat
{
public:
	CBaseDat();
	~CBaseDat();

	void	InitParams();
	void	ResetFlag();
	int	IsInit();

	int	SetType( int num );
	int	SetType( char* typechar );

	int SetMem( int* srcint, __int64 setflag );


	int	IsSetMem( __int64* flagptr, int num );
	int	IsSetMem( __int64 flag );

	int IsSameData( CBaseDat* cmpbasedat, __int64 cmpflag );	
	int IsSameData( CBaseDat* cmpbasedat, __int64* cmpflag, int flagnum );

	int AssertPointer( void* voidptr, char* mes );
	
	int	CopyData( CBaseDat* srcdata ); // 子クラスから呼ぶ
	int DumpMem( HANDLE hfile, int tabnum, char* headerptr );

	void DestroyObjs();


	//char* name;
	__int64	chkflag;
	int	type;
	int	isinit;
	int state; // for debug

};


class CVec3f : public CBaseDat
{
public:
	CVec3f();
	CVec3f( float x, float y, float z );
	CVec3f( float x, float y, float z, __int64 setflag );

	~CVec3f();
	void	InitParams();
	int	CopyData( CVec3f* srcdata );
	int DumpMem( HANDLE hfile, int tabnum, int dumpflag );

	int Normalize();
	float Length();
	float DotProduct( CVec3f srcvec );

		// 通常 : setflag で　data type も可変
	//int SetMem( void* dataptr, __int64 setflag );
	int	SetMem( float x, float y, float z );
	int	SetMem( float x, float y, float z, __int64 setflag );
	
	int SetMem( int* srcint, __int64 setflag );
	int SetMem( CVec3f* srcvec, __int64 setflag );
	int SetMem( VEC3F srcvec, __int64 setflag );
	int SetMem( float* srcfloat, __int64 setflag );

	void DestroyObjs();

	float x, y, z;

};

class CMeshInfo : public CBaseDat
{
public:
	CMeshInfo();
	~CMeshInfo();


	//int SetMem( void* srcdata, __int64 setflag );
	int	SetMem( int idata, __int64 setflag );
	int SetMem( int* srcint, __int64 setflag );
	int SetMem( CVec3f* srcvec, __int64 setflag );
	int SetMem( VEC3F srcvec, __int64 setflag );
	int SetMem( float* srcfloat, __int64 setflag );

	int HasColor();
	CVec3f* GetMaterial( __int64 getflag );

	void	InitParams();
	int	ResetParams();

	int	CopyData( CMeshInfo* srcdata );
	int NewMeshInfo( CMeshInfo** hnewinfo );

	int CopyMaterial( CMeshInfo* srcmi );
	int SetDefaultMaterial();

	int	DumpMem( HANDLE hfile, int tabnum, int dumpflag );

	int IsSameData( CMeshInfo* cmpmeshinfo, __int64 cmpflag );	
	int IsSameData( CMeshInfo* cmpmeshinfo, __int64* cmpflag, int flagnum );

	int	m;
	int n;
	int	total;
	int	mclosed; // meshを構成するlineがすべて閉じているか
	int nclosed; // meshが交差方向に閉じているか
	int skip; // スキップ数

	CVec3f*	diffuse;
	CVec3f*	specular;
	CVec3f*	ambient;

private:
	void	DestroyObjs();
	int	CreateObjs();

	//int type; // SHDMESH, SHDLINE, SHDPOLYMESH ...
	int hascolor;// diffuse, specular, ambientに有効な値がセットされているとき１。

};

class CVecLine : public CBaseDat
{
public:
	CVecLine();
	~CVecLine();
	
	// すべて初期化	
	void	InitParams();
	
		// meshinfo と　arrayの長さが矛盾しないように、いっしょに初期化
	int	InitVecLine( CMeshInfo* meshinfo );

	int	CopyData( CVecLine* srcdata );
	int	DumpMem( HANDLE hfile, int tabnum, int dumpflag );

	int SetMem( int* srcint, __int64 setflag );
	int SetMem( CVecLine* srcline, __int64 setflag );
	int SetMem( CVec3f* srcvec, int datano, __int64 setflag );
	int SetMem( VEC3F* srcvec, int datano, __int64 setflag );
	int SetMem( VEC3F* srcvec, int vecnum );
	int SetMem( float* srcfloat, int datano, __int64 setflag );

	VEC3F* GetVec( int vecno );

	int CheckMeshInfo( CMeshInfo* srcmeshinfo );

	int Transform( CMatrix2& transmat, CVecLine* srcline );

	VEC3F* vec;
	CMeshInfo* meshinfo;

private:
	void	ResetParams();
	void	DestroyObjs();

	int	CreateObjs();
	int	CreateLine( CMeshInfo* srcmeshinfo );
	int DestroyLine();

};


#endif