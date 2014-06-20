#ifndef TOON1FILEH
#define TOON1FILEH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>

class CTreeHandler2;
class CShdHandler;
class CShdElem;
class CToon1Params;

#define MATBUFLENG		3000
#define TO1LINELENG		2048

// mode
enum {
	TO1_SAVE,
	TO1_LOAD,
	TO1_MAX
};

typedef struct tag_dispobjbuf
{
	char* buf;
	int bufleng;
	int pos;
	int isend;
} TMPBUF;


class CToon1File
{
public:
	CToon1File( int srcmode );
	~CToon1File();

	int SaveToon1File( char* filename, CTreeHandler2* lpth, CShdHandler* lpsh );
	int LoadToon1File( HWND srchwnd, char* filename, CTreeHandler2* lpth, CShdHandler* lpsh );

private:
	int InitParams();
	int DestroyObjs();
	int DestroyDispBuf();
	int DestroyMatBuf();

	int WriteHeader();
	int WriteDispObj( CShdElem* selem );
	int WriteMaterial( CToon1Params* toon1param );
	int WriteLinechar( int addreturn );


	int SetBuffer( char* filename );
	int CheckFileHeader();
	int SetDispBuffer( int* seriptr, float* darknl, float* brightnl );
	int SetNL( int seri, float darknl, float brightnl );
	int SetMaterialBuffer( int seri, int* matnoptr );
	int ReadMaterialBuffer( int seri, int matno );

	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );



private:
	TO1BUF m_to1buf;
	TMPBUF m_dispbuf;
	TMPBUF m_matbuf;
	char m_linechar[ TO1LINELENG ];

	int m_mode;
	CTreeHandler2* m_thandler;
	CShdHandler* m_shandler;

	int m_loadversion;
};

#endif