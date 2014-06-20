#ifndef MAOPEH
#define MAOPEH

class CMCHandler;
class CMotHandler;
class CHandlerSet;

class MAOpe
{
public:
	MAOpe( CMotHandler* srcmh );
	~MAOpe();

	int LoadMAFile( char* srcname, HWND srchwnd );

	//int SetNewPose( int srcevent, int* motidptr, int* framenoptr );


private:
	int InitParams();
	int DestroyObjs();

private:
	CMotHandler* m_mh;
	CMCHandler* m_mch;

};

#endif