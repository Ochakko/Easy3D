#ifndef LWOPOLSELEMH
#define LWOPOLSELEMH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

class CLWOpolselem
{
public:
	CLWOpolselem();
	~CLWOpolselem();

	int CreateData( int srcvertnum );
	int SetVert( unsigned int* srcvert, int srcvertnum );

private:
	int InitParams();
	int DestroyObjs();


public:
	int vertnum;
	unsigned int* vertptr;

};

#endif