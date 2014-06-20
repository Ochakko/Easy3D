#ifndef MKMFILEH
#define MKMFILEH

#include <coef.h>
#include <d3dx9.h>

enum {
	MKM_FINDMOTNAME,
	MKM_READMOTNAME,
	MKM_FINDBONEELEM,
	MKM_READQ,
	MKM_READTRA,
	MKM_EOF
};


class CMkmMot;
class CTreeHandler2;
class CShdHandler;
class CMotHandler;

class CMKMFile
{
public:
	CMKMFile();
	~CMKMFile();

	int LoadMKMFile( char* filename );

	int SetMotion( CMKMFile* inimkm, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, int motno, float tramult, LONG* motidptr );


private:
	int InitParams();
	int DestroyObjs();


	int SetBuffer( char* srcfilename );
	int CheckFileHeader();
	int FindMotionBlock( int setflag, int* numptr );


	int GetLine( int* getlen );
//	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng, int* stepnum );
//	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
//	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

public:
	int m_motnum;
	CMkmMot* m_mkmmot;

private:
	//int m_state;
	MKMBUF m_mkmbuf;	

};



#endif