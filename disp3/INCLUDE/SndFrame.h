#ifndef SNDFRAMEH
#define SHDFRAMEH

#include <coef.h>

class CSndKey;

class CSndFrame
{
public:
	CSndFrame();
	~CSndFrame();

	int AddSndKey( CSndKey* psndkey );

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_frameno;
	int m_keynum;
	CSndKey** m_ppkey;
	int m_keyflag;
};


#endif