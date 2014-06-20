#ifndef WRITEFILEH
#define WRITEFILEH

class CWriteFile
{
public:
	CWriteFile( char* filename );
	~CWriteFile();

	int Write2File( char* lpFormat, ... );

private:
	int InitParams();
	int DestroyObjs();
	int SetBuffer();

private:

	//fileëÄçÏóp
	HANDLE m_hfile;
	char* m_buf;
	DWORD m_pos;
	DWORD m_bufleng;

	char m_filename[MAX_PATH];
};

#endif