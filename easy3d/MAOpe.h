#ifndef MAOPEH
#define MAOPEH

class CMCHandler;
class CMotHandler;
class CHandlerSet;

class MAOpe
{
public:
	MAOpe( CHandlerSet* srchs );
	~MAOpe();

	int LoadMAFile( char* srcname, HWND srchwnd );

	//int SetNewPose( int srcevent, int* motidptr, int* framenoptr );


private:
	int InitParams();
	int DestroyObjs();

private:
	CHandlerSet* m_hs;
	CMCHandler* m_mch;

};

#endif