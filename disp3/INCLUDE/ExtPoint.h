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
	int pointid; // line���ŁA��ӂ̔ԍ�
	int controlflag; // ���[�U�[�w��̓_�̏ꍇ�P�A��Ԃō쐬�����ꍇ�O
	D3DXVECTOR3 point;
	//int ownerid; // point���L����H�H�ꍇ�Ɏg������
	CExtPoint* next;
	CExtPoint* prev;

	int ishead;
};


#endif