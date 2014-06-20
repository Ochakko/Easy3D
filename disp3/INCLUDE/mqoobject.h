#ifndef MQOOBJECTH
#define MQOOBJECTH

#include <stdio.h>
#include <stdarg.h>

#include <coef.h>
#include <basedat.h>

#define DBGH
#include <dbg.h>

class CMQOFace;
class CShdHandler;
//class CMQOMaterial;

class CShdElem;

class CMQOObject
{
public:
	CMQOObject();
	~CMQOObject();

	int SetParams( char* srcchar, int srcleng );
	
	int SetVertex( int* vertnum, char* srcchar, int srcleng );
	int SetPointBuf( int vertno, char* srcchar, int srcleng );
	int SetPointBuf( unsigned char* srcptr, int srcnum );

	int SetFace( int* facenum, char* srcchar, int srcleng );
	int SetFaceBuf( int faceno, char* srcchar, int srcleng, int setmatno );

	int CreateColor();
	int SetColor( char* srcchar, int srcleng );

	int Dump();

	int MakePolymesh2( CShdHandler* shandler, int cur_seri, CMQOMaterial* mathead, int groundflag, int adjustuvflag );
	int MakeExtLine( CShdHandler* shandler, int cur_seri, CMQOMaterial* mathead );

	int HasPolygon();
	int HasLine();

	int MultMat( D3DXMATRIX multmat );
	int Shift( D3DXVECTOR3 shiftvec );
	int Multiple( float multiple );

	int IsMikoBone();
	int SetMikoBoneIndex3();
	int SetMikoBoneIndex2();
	int GetTopLevelMikoBone( CMQOFace** pptopface, int* topnumptr, int maxnum );
	int SetTreeMikoBone( CMQOFace* srctopface, CMQOMaterial* mathead );
	int InitFaceDirtyFlag();
	int CheckSameMikoBone();
	int CheckLoopedMikoBoneReq( CMQOMaterial* mathead, CMQOFace* faceptr, int* isloopedptr, int* jointnumptr );
	int SetMikoBoneName( CMQOMaterial* mathead );
	int SetMikoFloatBoneName();

	int GetParentLock( CMQOFace* srcface, CVec3f* locptr );
	int GetChildLock( CMQOFace* srcface, CVec3f* locptr );
	int GetHLock( CMQOFace* srcface, CVec3f* lockptr );


	void DumpMikoBoneReq( CMQOFace* srcface, int depth );

private:
	void InitParams();

	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

	int MakeLatheBuf();
	int MakeMirrorBuf();

	int FindConnectFace( int issetface );
	int MakeMirrorPointAndFace( int axis, int doconnect );

	CMQOMaterial* GetMaterialFromNo( CMQOMaterial* mathead, int matno );

	int begin_polymesh( int facenum, CShdHandler* shandler, int cur_seri );
	int end_polymesh( CShdElem* curelem );

	int CheckMirrorDis( VEC3F* pbuf, CMQOFace* fbuf, int lno, int pnum );

	int CheckFaceSameChildIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface );
	int FindFaceSameParentIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface, int* findnum, int maxnum );
	int IsSameMikoBone( CMQOFace* face1, CMQOFace* face2 );
	int IsSameFaceIndex( CMQOFace* face1, CMQOFace* face2 );
	int IsSameFacePos( CMQOFace* face1, CMQOFace* face2, VEC3F* pointptr );

	int CheckMaterialSameName( int srcmatno, CMQOMaterial* mathead, int* nameflag );


public:
	int objectno;
	char name[256];

	int patch;
	int segment;

	int visible;
	int locking;

	int shading;
	float facet;
	ARGBF color;
	int color_type;
	int mirror;
	int mirror_axis;
	int issetmirror_dis;
	float mirror_dis;

	int lathe;
	int lathe_axis;
	int lathe_seg;
	int vertex;
	//BVertex;
	int face;

	VEC3F* pointbuf;
	CMQOFace* facebuf;

	int hascolor;
	ARGBF* colorbuf;

	int vertex2;
	int face2;
	VEC3F* pointbuf2;
	CMQOFace* facebuf2;
	ARGBF* colorbuf2;

	int connectnum;
	CMQOFace* connectface;

	CMQOObject* next;

};

#endif