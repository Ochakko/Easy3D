#ifndef CAMERAKEYH
#define CAMERAKEYH

#include <coef.h>

class CCameraKey
{
public:
	CCameraKey();
	~CCameraKey();

	int SetCameraKey( CAMERAELEM* srccelem );
	int SetInterp( int srcinterp );

	int FramenoCmp( CCameraKey* cmpck );
	int FramenoCmp( int cmpno );

	int AddToPrev( CCameraKey* addck );
	int AddToNext( CCameraKey* addck );
	int LeaveFromChain();

	int CopyCameraElem( CCameraKey* srcck );

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_frameno;
	CAMERAELEM m_celem;
	int m_interp;

	CCameraKey* prev;
	CCameraKey* next;
};

#endif
