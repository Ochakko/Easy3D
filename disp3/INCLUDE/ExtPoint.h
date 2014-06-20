#ifndef EXTPOINTH
#define EXTPOINTH

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


class CExtPoint
{

public:
	CExtPoint();
	CExtPoint( int ctrlflag );
	~CExtPoint();


public:
	int AddToPrev( CExtPoint* newnp );
	int AddToNext( CExtPoint* newnp );
	int LeaveFromChain( CExtPoint** pphead );


private:
	void InitParams( int ctrlflag );


public:
	int pointid; // line中で、一意の番号
	int controlflag; // ユーザー指定の点の場合１、補間で作成した場合０
	D3DXVECTOR3 point;
	//int ownerid; // pointを占有する？？場合に使うかも
	CExtPoint* next;
	CExtPoint* prev;

	int ishead;
};


#endif