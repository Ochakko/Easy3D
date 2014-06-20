#ifndef PICKDATAH
#define PICKDATAH

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#define PICKDATAMAX		128


class CPickData
{

public:
	CPickData();
	~CPickData();

	int PickVert( int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DTLVERTEX* curtlv, int getmaxnum, int* getnumptr );
	//int PickVert3( D3DXVECTOR3* worldv, D3DXVECTOR3* campos, int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DTLVERTEX* curtlv, int getmaxnum, int* getnumptr );

	int PickVert( int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DXVECTOR3* curtlv, int getmaxnum, int* getnumptr );

private:
	int InitParams();


public:
	int setflag;
	int partno[ PICKDATAMAX ];
	int vertno[ PICKDATAMAX ];
	D3DXVECTOR3 scpos;
	float dist2;

};

#endif