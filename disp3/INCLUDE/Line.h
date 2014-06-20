#ifndef CLINEH
#define CLINEH

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

class CLine
{

public:
	CLine();
	~CLine();

	int InitParams();

	int SetLineFrom2V( D3DXVECTOR3* srcp1, D3DXVECTOR3* srcp2 );

public:
	D3DXVECTOR3 P0;
	D3DXVECTOR3 P1;
	D3DXVECTOR3 V;
};


#endif
