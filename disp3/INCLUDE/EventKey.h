#ifndef EVENTKEYH
#define EVENTKEYH

#include <coef.h>

class CEventKey
{
public:
	CEventKey();
	~CEventKey();

	int InitParams();
	int AddEKey( EKEY srcek );
	int DelEKeyByIndex( int srcindex );

	int GetEventNo( int srckey, int srccnt );

	int CheckSameKey( int srckey, int* indexptr );

private:
	int DestroyObjs();

public:
	EKEY m_ekey[256];
	int m_keynum;

	int m_savemotid;
	int m_saveframeno;
};

#endif
