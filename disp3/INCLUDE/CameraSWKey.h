#ifndef CAMERASWKEYH
#define CAMERASWKEYH

#include <coef.h>

class CCameraSWKey
{
public:
	CCameraSWKey();
	~CCameraSWKey();

	int SetCameraSWKey( int srccamno );

	int FramenoCmp( CCameraSWKey* cmpcswk );
	int FramenoCmp( int cmpno );

	int AddToPrev( CCameraSWKey* addcswk );
	int AddToNext( CCameraSWKey* addcswk );
	int LeaveFromChain();

	int CopyCameraSW( CCameraSWKey* srccswk );

private:
	int InitParams();
	int DestroyObjs();


public:
	int m_frameno;
	int m_camerano;

	CCameraSWKey* prev;
	CCameraSWKey* next;
};

#endif
