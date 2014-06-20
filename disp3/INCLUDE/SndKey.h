#ifndef SNDKEYH
#define SNDKEYH

#include <coef.h>

class CSndKey
{
public:
	CSndKey();
	~CSndKey();
	int AddSndE( SNDELEM* srcsnde );
	int DeleteSndE( int srcindex );
	int GetIndex( SNDELEM* srcsnde, int* indexptr );
	int FramenoCmp( CSndKey* cmpsndk );
	int FramenoCmp( int cmpno );
	int AddToPrev( CSndKey* addsndk );
	int AddToNext( CSndKey* addsndk );
	int LeaveFromChain();
	int CopySndElem( CSndKey* srcsndk );

private:
	int InitParams();
	int DestroyObjs();
	int IsSameSndE( SNDELEM* snde1, SNDELEM* snde2 );

public:
	int m_frameno;
	int m_sndenum;
	SNDELEM* m_psnde;
	int m_doneflag;
	CSndKey* prev;
	CSndKey* next;
};


#endif