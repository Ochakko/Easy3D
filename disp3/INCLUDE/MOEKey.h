#ifndef MOEKeyH
#define MOEKeyH

#include <coef.h>

class CMOEKey
{
public:
	CMOEKey();
	~CMOEKey();

	int SetIdlingName( char* srcname );
	int AddMOEE( MOEELEM* srcmoee );
	int DeleteMOEE( int srcindex );
	int GetIndex( MOEELEM* srcmoee, int* indexptr );

	int FramenoCmp( CMOEKey* cmpmoek );
	int FramenoCmp( int cmpno );

	int AddToPrev( CMOEKey* addmoek );
	int AddToNext( CMOEKey* addmoek );
	int LeaveFromChain();

	int CopyMOEElem( CMOEKey* srcmoek );

private:
	int InitParams();
	int DestroyObjs();

	int IsSameMOEE( MOEELEM* moee1, MOEELEM* moee2 );


public:
	int m_frameno;
	char m_idlingname[256];
	int m_goonflag;

	int m_moeenum;
	MOEELEM* m_pmoee;

	CMOEKey* prev;
	CMOEKey* next;
};

#endif
