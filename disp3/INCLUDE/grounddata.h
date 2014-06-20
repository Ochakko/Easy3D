#ifndef GROUNDDATAH
#define GROUNDDATAH

#include <coef.h>
#include <D3DX9.h>

#include <matrix2.h>
#include <crtdbg.h>

#include <basedat.h>

#include <polymesh.h>
#include <borderface.h>

class CGroundData : public CBaseDat
{
public:
	CGroundData();
	~CGroundData();

	int CreateBuffers( CMeshInfo* srcmeshinfo );
	int SetPolyMeshPtr( int pmno, CPolyMesh* srcpm );

	//int LoadGroundBMP( char* bmpname, int divx, int divy, float maxheight );
	//int SetGroundData( RGBDAT* rgb1, int rgbflag1, RGBDAT* rgb2, int rgbflag2, RGBDAT* rgb3, int rgbflag3,
	//	int bmpw, int bmph, 
	//	float maxx, float maxz,
	//	int divx, int divz, 
	//	float maxheight );

	int SetGroundData( 
		float* ntscy1, float minntscy1, float maxntscy1,
		float* ntscy2, float minntscy2, float maxntscy2,
		float* ntscy3, float minntscy3, float maxntscy3,
		
		int bmpw, int bmph, 
		float maxx, float maxz,
		int divx, int divz, 
		float maxheight );

	int SetGroundDataTS( 
		float* ntscy1, float minntscy1, float maxntscy1,
		int bmpw, int bmph, 
		float maxx, float maxz,
		int divx, int divz, 
		float maxheight );

////// for Invisible Wall
	int CreateInvisibleWallObj( float* ntscy, float miny, float maxy, int bmpw, int bmph, float maxx, float maxz );
	int DestroyInvisibleWallObj();

	int GetBorderFaceNum( int pmno, int* bfnum );
	int SetInvisibleWallPM( int pmno, int pmvnum, int pmfnum, int divx, int divz, float maxx, float maxz, float wallheight );


///////


private:
	void InitParams();
	void DestroyObjs();

	int FindSame2Point( int* indexbuf, int facenum, int p1, int p2, int srcother, int* dstother );
	int FindSame1EdgeFromBF( int srcp1, int srcp2, int srcp3, CBorderFace** samebf );

	int Index2Pos( int srcindex, int divx, int divz, float maxx, float maxz, D3DXVECTOR3* dstv );
	int Index2PolymeshNo( int srcindex, int divx, int divz, int* areanoptr );

	int IsEdgeOfMap( int srcindex, int divx, int divz );

public:
	CMeshInfo* meshinfo; // mはdivx、nはdivz、totalは使わない0
	char m_gbmpname[MAX_PATH];
	int m_divx; //分割数。0から始まるナンバー。線の本数 - 1
	int m_divz; //分割数。0から始まるナンバー。線の本数 - 1

	int m_bulkx;
	int m_bulkz;

	int m_pmnum;
	CPolyMesh** m_pmarray;// ポインタを保持するだけで、内容のalloc, freeは行わない。！！！
	
	RECT* m_rect;//各polymesh2のための、格子分けの情報。


///// for Invisible Wall
	CBorderFace* m_bf;
	int* m_hitpoint;
	int* m_indexbuf;

};

#endif