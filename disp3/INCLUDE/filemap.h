#define	FILEMAPH

#include <tchar.h>

enum	{
	ACT_READ, ACT_WRITE
};

typedef struct CUSFILEINFO
{
	DWORD	hashval;
	DWORD	dwcookie;

	HANDLE	hfile;
	HANDLE	hfilemapping;
	PBYTE	pbyte;
	//char*	filename;
	//wchar_t*	filename;
	LPTSTR filename;
	int		acttype;
	DWORD	sizehigh;
	DWORD	sizelow;

	CUSFILEINFO*	nextinfo;

} CUSFILEINFO;


class	AFX_EXT_CLASS CFileMap
{
public:
	CFileMap();
	~CFileMap();

	//CUSFILEINFO*	AddNewFileInfo( char* fname, 
	//	int acttype, DWORD sizehigh, DWORD sizelow );
	CUSFILEINFO* AddNewFileInfo( LPCTSTR fname,
		int acttype, DWORD sizehigh, DWORD sizelow );

	BOOL	DelFileInfo( DWORD dwcookie );
	
	CUSFILEINFO*	GetFileInfo( DWORD dwcookie );

	BOOL	FlushAndSetEnd( DWORD dwcookie, DWORD	curpos = -1 );

private:
	CUSFILEINFO*		m_firstFileInfo;

		
	//DWORD	HashFName( char* fname );
	DWORD HashFName( LPCTSTR fname );

		// hashval, fname の順で　チェック
	//CUSFILEINFO*	FindChain( DWORD hashval, char* fname );
	CUSFILEINFO*	FindChain( DWORD hashval, LPCTSTR fname );

		// pinfo->hashval で　ソート
	BOOL	AddChain( CUSFILEINFO*	pinfo );

	
};


//CUSFILEINFO*	FindFileInfo( DWORD	dwcookie );
