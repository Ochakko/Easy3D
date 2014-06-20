#ifndef MOTFILEH
#define MOTFILEH


#define DBGH
#include <dbg.h>

#include <matrix2.h>

typedef struct motfilehdr
{
	int magicno;
	char motname[256];
	int framenum;
	int mottype;
	int elemnum;
} MOTFILEHDR;

typedef struct motfilemarker
{
	int magicno;
	int serialno;//実際には、bonenoや、morphnoが入る。
	int valnum;
} MOTFILEMARKER;

typedef struct tag_motbuf
{
	char* buf;
	int bufsize;
	int pos;
} MOTBUF;


class CMotHandler;

class CMotFile
{
public:
	CMotFile();
	~CMotFile();
	
	int SaveMotFile( CMotHandler* lpmh, int mcookie, char* fname );
	int LoadMotData( CMotHandler* lpmh, char* fname, char* motname, int* framenum, int* mottype );
	int LoadMotDataFromBuf( CMotHandler* lpmh, char* bufptr, int bufsize, char* motname, int* framenum, int* mottype );


private:
	void DestroyObjs();
	int CreateMotFile( char* fname, char* mname, int mcookie, int framenum, int mottype, int elemnum );

	int WriteData( CMatrix2* wmat );// matrix１個ずつ書き出す。

	int OpenMotFile( char* fname );
	int ReadHeader( MOTFILEHDR* hdr );
	int ReadData( CMatrix2* rmat, int matnum );// header.framenum個ずつ読み込む。

	int WriteMarker( MOTFILEMARKER* wmarker );
	int WriteData( float* wval, int valnum );
	int ReadMarker( MOTFILEMARKER* rmarker );
	int ReadData( float* rval, int valnum );

	int ReadBoneMatrix( CMotHandler* lpmh, int mcookie, MOTFILEMARKER mmarker, int framenum, int* bno2serino );
	int ReadMorphVal( CMotHandler* lpmh, int mcookie, MOTFILEMARKER mmarker, int framenum, int* mno2serino );

	int LoadMotData_aft( CMotHandler* lpmh, char* motname, int* framenum, int* mottype );


public:
	MOTFILEHDR header;
	CMatrix2* matptr;
	HANDLE hfile;	
	int m_writemode;


	int m_frombuf;
	MOTBUF m_buf;

};


#endif