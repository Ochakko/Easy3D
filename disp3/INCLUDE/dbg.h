// platform sdk \ d3dim \ include ‚ÖˆÚ“®

#ifndef SHDTREEVIEWH
#ifndef NOTWINDOWSH
#include <windows.h>
#endif
#endif

#include <coef.h>
#ifndef DBGH
#define	DBGH
	HANDLE dbgfile = 0;
	char tabchar[TREEDEPTHMAX + 1];

	int DbgOut( char* lpFormat, ... );
	int DbgOutB( void* lpvoid, int datsize );
	int Write2File( HANDLE hfile, char* lpFormat, ... );
	int SetTabChar( int tabnum );		
	void ErrorMessage(char* szMessage, HRESULT hr);
	int OpenDbgFile();
	int CloseDbgFile();
#else
	extern HANDLE dbgfile;
	extern char tabchar[TREEDEPTHMAX + 1];

	extern int DbgOut( char* lpFormat, ... );
	extern int DbgOutB( void* lpvoid, int datsize );
	extern int Write2File( HANDLE hfile, char* lpFormat, ... );
	extern int SetTabChar( int tabnum );
	extern void ErrorMessage(char* szMessage, HRESULT hr);
	extern int OpenDbgFile();
	extern int CloseDbgFile();
#endif

