#ifndef LWOVMADELEMH
#define LWOVMADELEMH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>


class CLWOvmadelem
{
public:
	CLWOvmadelem();
	~CLWOvmadelem();	


	int CreateData( int srcdimension );
	int SetParams( int srcvertindex, int srcpolyindex, float* srcvalueptr, int valuenum );

private:

	int DestroyData();
	int InitParams();


public:
	int dimension;

	int vertindex;
	int polyindex;
	float* valueptr;

};



#endif