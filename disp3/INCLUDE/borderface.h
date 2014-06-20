#ifndef BORDERFACEH
#define BORDERFACEH

#include <coef.h>
#include <D3DX9.h>

#include <matrix2.h>
#include <crtdbg.h>

#include <basedat.h>

class CBorderFace
{
public:
	CBorderFace();
	~CBorderFace();

	int SetParams( int srcp1, int srcp2, int srcother, int divx, int divz );

	float CalcDot( CBorderFace* srcbf );
	int SetClockwise( D3DXVECTOR3 srcnvec );// srcnvec との内積がの符号により、clockwiseをセットする。

private:
	void InitParams();
	void DestroyObjs();

	int Index2Position( int pno, int divx, int divz, D3DXVECTOR3* dstv );
	int CalcNormal( D3DXVECTOR3* newn, D3DXVECTOR3* curp, D3DXVECTOR3* aftp1, D3DXVECTOR3* aftp2 );

public:
	int serialno;
	int p1;
	int p2;
	int other;
	int clockwise;
	int useflag;
	D3DXVECTOR3 nvec;
	CBorderFace* next;

	int areano;
};

#endif