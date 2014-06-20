#ifndef MMOTKEYH
#define MMOTKEYH

#include <coef.h>

class CMMotElem;

class CMMotKey
{
public:
	CMMotKey();
	~CMMotKey();

	int AddBase( CShdElem* srcbase );
	int DeleteBase( CShdElem* delbase );
	int AddTarget( CShdElem* srcbase, CShdElem* srctarget );
	int DeleteTarget( CShdElem* srcbase, CShdElem* srctarget );
	int DeleteAllElem();

	CMMotElem* ExistBase( CShdElem* srcbase );
	MPRIM* ExistTarget( CShdElem* srcbase, CShdElem* srctarget );

	int SetValue( CShdElem* srcbase, CShdElem* srctarget, float srcvalue );
	float GetValue( CShdElem* srcbase, CShdElem* srctarget, int* existflag );



	int FramenoCmp( CMMotKey* cmpmmk );
	int FramenoCmp( int cmpno );

	int AddToPrev( CMMotKey* addmmk );
	int AddToNext( CMMotKey* addmmk );
	int LeaveFromChain();

	int CopyMMotElem( CMMotElem* srcmme );
	int CopyMMotKey( CMMotKey* srcmmk );

private:
	int InitParams();
	int DestroyObjs();

public:
	int frameno;
	int mmenum;
	CMMotElem** ppmme;//morphのバリュー。ベースの数だけの配列。
	CMMotKey* prev;
	CMMotKey* next;
};

#endif