#ifndef LWOCLIPH
#define LWOCLIPH

#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

class CLWOclip
{
public:
	CLWOclip();
	~CLWOclip();

	int SetIndex( unsigned int srcindex );
	int SetStil( char* srcname );
	int SetIseq( unsigned char srcnumdig, unsigned char srcflag, short srcoff, unsigned short srcres,
		short srcstart, short srcend, char* srcpre, char* srcsuff );
	int SetXref( unsigned int srcindex, char* srcname );


private:
	int InitParams();
	int DestroyObjs();


public:
	unsigned int index;
	char typestr[5];

	char* nameptr;

	unsigned char numdigits;
	unsigned char flag;
	short offset;
	unsigned short reserved;
	short start;
	short end;
	char* prefixptr;
	char* suffixptr;

	int xrefindex;

	//CLWOclip* next;

};

#endif