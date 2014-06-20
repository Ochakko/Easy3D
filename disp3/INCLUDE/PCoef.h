#ifndef CPCOEFH
#define CPCOEFH

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

class CPCoef
{

public:
	CPCoef();
	~CPCoef();

	int InitParams();

	int SetPCFrom3V( D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3 );
	int SetPC( D3DXVECTOR3 srcn, float srcd );

	int OnPlane( D3DXVECTOR3 srcv, float* dstval );
	int Inverse();

public:
	float m_a;
	float m_b;
	float m_c;
	float m_d;

	char m_invalidflag;
};


#endif
