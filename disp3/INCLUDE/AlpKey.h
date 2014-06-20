#ifndef ALPKEYH
#define ALPKEYH

class CMQOMaterial;
class CAlpChange;

class CAlpKey
{
public:
	CAlpKey();
	~CAlpKey();

	int SetAlpChange( CMQOMaterial* srcmat, float srcalpha );
	int DeleteAlpChange( CMQOMaterial* srcmat );
	int DeleteAllChange();


	int ExistAlpChange( CMQOMaterial* srcmat, CAlpChange** ppalpc );

	int FramenoCmp( CAlpKey* cmpalpk );
	int FramenoCmp( int cmpno );

	int AddToPrev( CAlpKey* addalpk );
	int AddToNext( CAlpKey* addalpk );
	int LeaveFromChain();

	int CopyAlpChange( CAlpKey* srcalpk );

private:
	int InitParams();
	int DestroyObjs();

	CAlpChange* FindAlpChange( CMQOMaterial* srcmat, int* pindex );

public:
	int frameno;
	
	int changenum;
	CAlpChange** alpchange;
	
	CAlpKey* prev;
	CAlpKey* next;
};

#endif