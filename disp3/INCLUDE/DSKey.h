#ifndef DSKEYH
#define DSKEYH

class CDSChange;
class CDispSwitch;

class CDSKey
{
public:
	CDSKey();
	~CDSKey();

	int SetDSChange( CDispSwitch* srcdsptr, int srcstate );
	int DeleteDSChange( CDispSwitch* srcdsptr );

	int ExistDSChange( CDispSwitch* srcdsptr, CDSChange** ppdsc );

	int FramenoCmp( CDSKey* cmpdsk );
	int FramenoCmp( int cmpno );

	int AddToPrev( CDSKey* adddsk );
	int AddToNext( CDSKey* adddsk );
	int LeaveFromChain();

	int CopyDSChange( CDSKey* srcdsk );

private:
	int InitParams();
	int DestroyObjs();

	CDSChange* FindDSChange( CDispSwitch* srcdsptr, int* pindex );

public:
	int frameno;

	int changenum;
	CDSChange** dschange;

	CDSKey* prev;
	CDSKey* next;
};




#endif
