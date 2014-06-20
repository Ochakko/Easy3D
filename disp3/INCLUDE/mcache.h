#ifndef MCACHEH
#define MCACHEH

#include <D3DX9.h>

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

#define MCNUM	6

class CMCache
{
public:
	CMCache();
	~CMCache();

public:
	__inline int IsChangeMatrix( D3DXMATRIX* srcmat )
	{
		if( sno[0] != -1 )
			return 1;

		if( *(matptr[0]) == *srcmat )
			return 0;
		else
			return 1;
	};

	__inline int InitCache( D3DXMATRIX* srcmat )
	{
		//top óvëfÇÕÅAserialno == -1êÍóp, world * viewÇäiî[Ç∑ÇÈÅB
		sno[0] = -1;
		*(matptr[0]) = *srcmat;
	
		int i;
		for( i = 1; i < MCNUM; i++ ){
			sno[i] = -2;
			//D3DXMatrixIdentity( &(mat[i]) );//è»ó™
		}

		spos = MCNUM - 1;

		return 0;

	};

	__inline int AddToCache( int srcno, D3DXMATRIX* srcmat )
	{
		/***
		int nextpos;
		nextpos = spos - 1;
		if( nextpos >= 1 )
			spos = nextpos;
		else
			spos = MCNUM - 1;

		sno[spos] = srcno;
		*(matptr[spos]) = *srcmat;
		***/
		

		int i;

		D3DXMATRIX* saveptr = matptr[MCNUM - 1];

		for( i = MCNUM - 2; i >= 1; i-- ){
			sno[i+1] = sno[i];
			matptr[i + 1] = matptr[i];
		}
		matptr[1] = saveptr;

		sno[1] = srcno;
		*(matptr[1]) = *srcmat;
		

		return 0;
	};

	__inline D3DXMATRIX* GetCache( int srcno )
	{
		int i;

		for( i = 0; i < MCNUM; i++ ){
			if( srcno == sno[i] )
				return *(matptr + i);
		}

		return 0;
	};


public:
	int spos;
	int sno[MCNUM];
	D3DXMATRIX* matptr[MCNUM];
	D3DXMATRIX	realm[MCNUM];
};

#endif