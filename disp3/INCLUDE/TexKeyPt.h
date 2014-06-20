#ifndef TEXKEYPTH
#define TEXKEYPTH

class CTexChangePt;
class CMQOMaterial;

class CTexKeyPt
{
public:
	CTexKeyPt();
	~CTexKeyPt();

	int SetTexChangePt( CMQOMaterial* srcmqomat, char* srcname );
	int DeleteTexChangePt( CMQOMaterial* srcmqomat );
	int DeleteAllChangePt();

	int ExistTexChangePt( CMQOMaterial* srcmat, CTexChangePt** pptc );

	int CopyTexChangePt( CTexKeyPt* srctk );

private:
	int InitParams();
	int DestroyObjs();

	CTexChangePt* FindTexChangePt( CMQOMaterial* srcmqomat, int* pindex );

public:
	int changenum;
	CTexChangePt** texchange;
};




#endif
