#include <windows.h>

#include <map>

typedef struct tag_pndprop
{
	char path[MAX_PATH];
	char directory[MAX_PATH];
	char filename[MAX_PATH];
	unsigned long sourcesize;
}PNDPROP;

class _PmCipher;

class CPanda {
public:
	CPanda();
	~CPanda();

	int Init( unsigned char* keyptr, unsigned int keyleng, int* pndidptr );
	int Destroy( int pndid );

	int Encrypt( int pndid, const char* orgdirectory, const char* pndpath );

	int ParseCipherFile( int pndid, const char* pndpath );
	int GetRootPath( int pndid, char* dstpath, int dstleng );
	int GetPropertyNum( int pndid );
	int GetProperty( int pndid, int propno, PNDPROP* dstprop );
	int GetProperty( int pndid, char* sourcepath, PNDPROP* dstprop );
	int Decrypt( int pndid, const char* filepath, unsigned char* dstbuf, int dstsize, int* getsize );


private:
	_PmCipher* GetCipher( int pndid );

private:
	std::map<int, _PmCipher*> m_cipher;

};

