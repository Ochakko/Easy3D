#include <vecmesh.h>


class CPlanes : public CVecMesh
{
public:
	CPlanes();
	~CPlanes();

	void InitParams();

	int SetMem( CPlanes* srcbmesh, __int64 setflag );
	int	CopyData( CPlanes* srcdata );
	
	int InitVecMesh( CPlanes* srcbmesh );

	virtual int CheckMeshInfo( CMeshInfo* srcmeshinfo ); //meshinfo->m = 2; meshinfo->n = �C��;
	virtual int CheckLineInfo( CMeshInfo* lineinfo, int infonum ); //eachline->meshinfo->m = �C��(���); eachline->meshinfo->n = 1;

		// pos line , normal (���ׂ�)�@�v�Z
	virtual int Transform( CMatrix2& transmat, CVecMesh* srcvmesh );
};

class CRevolved : public CPlanes
{
public:
	CRevolved();
	~CRevolved();

	void InitParams();

	int SetMem( int* srcint, __int64 setflag );
	int SetMem( CRevolved* srcrev, __int64 setflag );

	int SetMem( float* srcfloat, __int64 setflag );
	int SetMem( CMatrix2* srcmatrix, __int64 setflag );
	
	int	CopyData( CRevolved* srcdata );
	
	int InitVecMesh( CRevolved* srcbmesh );
	int DumpMem( HANDLE hfile, int tabnum, int dumpflag );

	//int Revolve();

public:
	int isnormalset;
	float radfrom;
	float radto;
	CMatrix2 axis;
};

class CSphere : public CPlanes
{
public:
	CSphere();
	~CSphere();

	void InitParams();

	int SetMem( CSphere* srcrev, __int64 setflag );
	int SetMem( CMatrix2* srcdefmat, __int64 setflag );

	int	CopyData( CSphere* srcdata );
	
	int InitVecMesh( CSphere* srcbmesh );
	int DumpMem( HANDLE hfile, int tabnum, int dumpflag );

public:
	CMatrix2 defmat;
	//CVecLine* orgsphere; // �P�ʋ�
};

class CDisk : public CPlanes
{
public:
	CDisk();
	~CDisk();

	void InitParams();

	int SetMem( float* srcfloat, __int64 setflag );
	int SetMem( int* srcint, __int64 setflag );
	int SetMem( CDisk* srcrev, __int64 setflag );
	int SetMem( CMatrix2* srcdefmat, __int64 setflag );

	int	CopyData( CDisk* srcdata );
	
	int InitVecMesh( CDisk* srcbmesh );
	int DumpMem( HANDLE hfile, int tabnum, int dumpflag );

public:
	float radfrom;
	float radto;
	int	isfilled;
	CMatrix2 defmat;
	//CVecLine* orgdisk; // �P�ʉ~
};


class CPolygon : public CPlanes
{
public:
	CPolygon();
	~CPolygon();

	int SetMem( CPolygon* srcbmesh, __int64 setflag );
	int	CopyData( CPolygon* srcdata );
	
	int InitVecMesh( CPolygon* srcbmesh );

	virtual int ConvDataNo( int* linenoptr, int* vecno, int datano );

	virtual int CheckMeshInfo( CMeshInfo* srcmeshinfo ); //meshinfo->m = 2; meshinfo->n = �C��;
	virtual int CheckLineInfo( CMeshInfo* lineinfo, int infonum ); //eachline->meshinfo->m = �C��(��); eachline->meshinfo->n = 1;

};

class CExtrude : public CVecMesh
{
	// �|����
	// line�͏�ʂ̒��_�̂݁B

public:
	CExtrude();
	~CExtrude();

	void InitParams();

	int SetExVec( VEC3F srcvec );

	virtual int CheckMeshInfo( CMeshInfo* srcmeshinfo ); //meshinfo->m = 1; meshinfo->n = 1;
	virtual int CheckLineInfo( CMeshInfo* lineinfo, int infonum ); //eachline->meshinfo->m = �C��; eachline->meshinfo->n = 1; infonum = 1

	int	DumpMem( HANDLE hfile, int tabnum, int dumpflag );	

public:
	VEC3F exvec; // �|���x�N�g��
};
