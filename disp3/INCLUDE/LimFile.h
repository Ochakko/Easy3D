#ifndef LIMFILEH
#define LIMFILEH

#include <coef.h>
#include <basedat.h>

#include <d3dx9.h>

class CShdHandler;
class CTreeHandler2;
class CShdElem;
class CTreeElem2;


typedef struct tag_limbuf
{
	HANDLE hfile;
	char* buf;
	int bufleng;
	int startpos;
	int endpos;
	int isend;
} LIMBUF;


class CLimFile
{
public:
	CLimFile();
	~CLimFile();

	int SaveLimFile( CShdHandler* lpsh, CTreeHandler2* lpth, char* filename );
	int LoadLimFile( HWND srchwnd, CShdHandler* lpsh, CTreeHandler2* lpth, char* filename );

private:
	int InitParams();
	int DestroyObjs();

	int SetBuffer( char* filename );
	int CheckFileHeader();
	int FindLimElem();
	int ReadLimElem();
	
	int GetName( char* srcptr, char* dstptr, int dstleng );
	int GetDouble( char* srcptr, double* dstptr );
	int GetInt( char* srcptr, int* dstptr );

	int WriteLimElem( CShdElem* selem, CTreeElem2* telem );
	int Write2File( char* lpFormat, ... );

private:
	CShdHandler* m_lpsh;
	CTreeHandler2* m_lpth;

	HANDLE m_hwfile;//書き出し用ファイルハンドル

	LIMBUF m_buf;

};

#endif