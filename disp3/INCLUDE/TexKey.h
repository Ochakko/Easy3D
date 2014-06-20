#ifndef TEXKEYH
#define TEXKEYH

class CTexChange;
class CMQOMaterial;

class CTexKey
{
public:
	CTexKey();
	~CTexKey();

	int SetTexChange( CMQOMaterial* srcmqomat, char* srcname );
	int DeleteTexChange( CMQOMaterial* srcmqomat );
	int DeleteAllChange();

	int ExistTexChange( CMQOMaterial* srcmat, CTexChange** pptc );

	int FramenoCmp( CTexKey* cmptk );
	int FramenoCmp( int cmpno );

	int AddToPrev( CTexKey* addtk );
	int AddToNext( CTexKey* addtk );
	int LeaveFromChain();

	int CopyTexChange( CTexKey* srctk );

private:
	int InitParams();
	int DestroyObjs();

	CTexChange* FindTexChange( CMQOMaterial* srcmqomat, int* pindex );

public:
	int frameno;

	int changenum;
	CTexChange** texchange;

	CTexKey* prev;
	CTexKey* next;
};




#endif
