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
	int pointid; // line���ŁA��ӂ̔ԍ�
	int controlflag; // ���[�U�[�w��̓_�̏ꍇ�P�A��Ԃō쐬�����ꍇ�O
	D3DXVECTOR3 point;
	int ownerid; // point���L����H�H�ꍇ�Ɏg������
	CNaviPoint* next;
	CNaviPoint* prev;

	int ishead;
};


#endif