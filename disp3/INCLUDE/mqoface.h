#ifndef MQOFACEH
#define MQOFACEH

#include <stdio.h>
#include <stdarg.h>

#include <coef.h>
#include <basedat.h>

#define DBGH
#include <dbg.h>


#ifndef MIKONAMELENG
	#define MIKONAMELENG 1024
#endif


class CMQOFace
{
public:
	CMQOFace();
	~CMQOFace();

	int SetParams( char* srcchar, int srcleng, int setmatno );

	int Dump();

	int CheckSameLine( CMQOFace* chkface, int* findflag );
	int SetInvFace( CMQOFace* srcface, int offset );

	int SetMikoBoneIndex3( VEC3F* pbuf );
	int SetMikoBoneIndex2( CMQOFace* srcface, int srcfacenum );

	int SetMikoBoneName( char* srcname, int lrflag );
	int CheckLRFlag( VEC3F* pointptr, int* lrflagptr );

private:
	void InitParams();

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetI64( __int64* dsti64, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

public:
	int pointnum;
	int index[4];
	int materialno;
	
	int hasuv;
	COORDINATE uv[4];

	__int64 col[4];
	int vcolsetflag;


	CMQOFace* next;

// for mikoto bone
	int mikobonetype; //MIKOBONE_NONE, MIKOBONE_NORMAL, MIKOBONE_float
	int parentindex;
	int childindex;
	int hindex;

	char bonename[MIKONAMELENG];
	CMQOFace* parent;
	CMQOFace* child;
	CMQOFace* brother;

	int dirtyflag;
	int serialno;

};

#endif
