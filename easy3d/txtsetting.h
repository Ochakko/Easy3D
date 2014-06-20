#ifndef TxtSettingH
#define TxtSettingH

#include <d3dx9.h>
#include <usercoef.h>

#define TXTLINELEN	2048

class CHandlerSet;

class CTxtSetting
{
public:
	CTxtSetting( CHandlerSet* hsptr );
	~CTxtSetting();

	int LoadParams( char* filename );
	int SaveKstFile( char* filename );

private:
	int InitParams();
	int DestroyObjs();

	int SetBuffer( char* filename );

	int GetElemLine( char* strpat );

	char* GetName( char* srcstr, char*dststr, int maxleng, int* dstsetflag );

	int Write2File( char* lpFormat, ... );

public:
	TSELEM m_elem[ SKEL_MAX ];

private:
	CHandlerSet* m_hs;

	//fileëÄçÏóp
	HANDLE m_hfile;
	char* m_buf;
	DWORD m_pos;
	DWORD m_bufleng;

	int m_filemode;

	char m_line[ TXTLINELEN ];
};

#endif