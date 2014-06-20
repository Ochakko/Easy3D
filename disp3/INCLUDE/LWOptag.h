#ifndef LWOPTAGH
#define LWOPTAGH


#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>


typedef struct tag_lwoptag
{
	int polyindex;
	int tag;
} LWOPTAG;



class CLWOptag
{

public:
	CLWOptag();
	~CLWOptag();

	int CreateData( char* srctype, int srcdatanum );
	int SetPtag( int srcdatano, int srcpolyindex, int srctag );

private:
	int InitParams();
	int DestroyObjs();


public:
	char typestr[5];
	int datanum;
	LWOPTAG* dataptr;

};

#endif