#ifndef GPKEYH
#define GPKEYH

#include <coef.h>

class CGPKey
{
public:
	CGPKey();
	~CGPKey();

	int SetGPKey( GPELEM* srcgpelem );
	int SetInterp( int srcinterp );

	int FramenoCmp( CGPKey* cmpgpk );
	int FramenoCmp( int cmpno );

	int AddToPrev( CGPKey* addgpk );
	int AddToNext( CGPKey* addgpk );
	int LeaveFromChain();

	int CopyGPElem( CGPKey* srcgpk );

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_frameno;
	GPELEM m_gpe;
	int m_interp;

	int m_keyflag;

	CGPKey* prev;
	CGPKey* next;
};

#endif
