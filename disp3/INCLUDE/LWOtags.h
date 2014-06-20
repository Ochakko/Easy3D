#ifndef LWOTAGSH
#define LWOTAGSH


#include <D3DX9.h>
#include <math.h>

#define DBGH
#include <dbg.h>

#include <coef.h>


class CLWOtags
{

public:
	CLWOtags();
	~CLWOtags();

	int SetParams( char* srcname, int srcindex );

	//int GetIndexByName( char* srcname, int* dstindex );
	//int GetNameByIndex( int srcindex, char* dstname, int dstleng );

private:
	int InitParams();
	int DestroyObjs();

public:
	char* nameptr;
	int index;

	//CLWOtags* next;
};

#endif

