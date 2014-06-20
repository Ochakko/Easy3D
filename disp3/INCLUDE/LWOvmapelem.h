#ifndef LWOVMAPELEMH
#define LWOVMAPELEMH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>


class CLWOvmapelem
{
public:
	CLWOvmapelem();
	~CLWOvmapelem();	


	int CreateData( int srcdimension );
	int SetParams( int srcvertindex, float* srcvalueptr, int valuenum );

private:

	int DestroyData();
	int InitParams();


public:
	int dimension;

	int vertindex;
	float* valueptr;

};



#endif