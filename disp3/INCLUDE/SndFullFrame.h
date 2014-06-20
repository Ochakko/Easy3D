#ifndef SNDFULLFRAMEH
#define SNDFULLFRAMEH

#include <coef.h>

class CSndKey;
class CSndFrame;

class CSndFullFrame
{
public:
	CSndFullFrame();
	~CSndFullFrame();

	int CreateFullFrame( CSndKey* firstkey, int framenum, int range );
	CSndFrame* GetFrame( int frameno );


private:
	int InitParams();
	int DestroyObjs();

public:
	int m_framenum;
	int m_range;
	CSndFrame* m_pframe;
};


#endif