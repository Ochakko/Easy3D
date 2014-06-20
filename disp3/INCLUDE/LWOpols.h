#ifndef LWOPOLSH
#define LWOPOLSH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <LWOpolselem.h>

class CLWOpols
{
public:
	CLWOpols();
	~CLWOpols();

	int CreateData( int srcdatanum, char* srctype );
	int SetPols( int srcdatano, unsigned int* srcdata, int srcdatalen );

private:
	int InitParams();
	int DestroyObjs();


public:
	char typestr[5];
	int datanum;
	CLWOpolselem* data;

};

#endif