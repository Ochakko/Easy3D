#ifndef NAVIPOINTH
#define NAVIPOINTH

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


class CNaviPoint
{

public:
	CNaviPoint();
	CNaviPoint( int ctrlflag );
	~CNaviPoint();


public:
	int AddToPrev( CNaviPoint* newnp );
	int AddToNext( CNaviPoint* newnp );
	int LeaveFromChain( CNaviPoint** pphead );


private:
	void InitParams( int ctrlflag );


public:
	int pointid; // line中で、一意の番号
	int controlflag; // ユーザー指定の点の場合１、補間で作成した場合０
	D3DXVECTOR3 point;
	int ownerid; // pointを占有する？？場合に使うかも
	CNaviPoint* next;
	CNaviPoint* prev;

	int ishead;
};


#endif