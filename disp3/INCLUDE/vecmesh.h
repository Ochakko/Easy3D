#ifndef	VECMESHH
#define VECMESHH

//#include "charpos.h"
//#include <d3drmwin.h>
#include <coef.h>
#include <D3DX9.h>


#include <basedat.h>
#include <crtdbg.h>


class CVecMesh : public CBaseDat
{
	// SHDPOLYGON, m�@*�@n line
	// line�ւ̃A�N�Z�X��index�͈̔͂̌��؂́Aoperator()�ɔC����B
	// line���̒��_�ԍ��́ACVecLine�ɔC���āA�`�F�b�N���Ȃ��B

public:
	CVecMesh();
	~CVecMesh();

	CVecLine* operator() (int i) { 
		#ifdef _DEBUG
			int ret;
			ret = CheckNo( i );
			_ASSERT( !ret );
		#endif
		return (CVecLine *)( *(hline + i) );
	};


	CVecLine* operator() (int vno, int uno) { 
		int datano;
		#ifdef _DEBUG
			int ret;
			ret = ConvUV2No( &datano, uno, vno );
			_ASSERT( !ret );
		#else
			datano = meshinfo->m * vno + uno;
		#endif

		return (CVecLine *)( *(hline + datano) );
	};
	
	int CheckNo( int datano )
	{
		if( (datano >= 0) && (datano < allocleng) )
			return 0;
		else
			return 1;
	};


	int ConvUV2No( int* datano, int uno, int vno )
	{
		int mnum, nnum;
		if( !meshinfo )
			return 0;

		mnum = meshinfo->m;
		nnum = meshinfo->n;
		if( (uno >= 0) && (uno < mnum) && (vno >= 0) && (vno < nnum) ){
			*datano = vno * mnum + uno;
			if( *datano < allocleng )
				return 0;
			else
				return 1;
		}else
			return 1;
	};
	
	// ���ׂď�����
	virtual void	InitParams();

	void	ResetParams();
	int	CreateObjs();
	void	DestroyObjs();
		
	// line��alloc���Ȃ��Ƃ��́Alineinfo �Ɂ@NULL
	int InitVecMesh( CMeshInfo* srcmeshinfo, CMeshInfo* lineinfo, int setleng = -1 );
	int InitVecMesh( CVecMesh* srcmesh );
	int	CreateVecMesh( CMeshInfo* srcmeshinfo, CMeshInfo* lineinfo, int setleng );
	int DestroyVecMesh();

	// line�̌�t��
	int AddVecLine( CMeshInfo* lineinfo, int linenum );


		// meshinfo �͈͓̔��ŃZ�b�g
	int SetMem( int* srcint, __int64 setflag );
	int SetMem( int srcint, int lineno, __int64 setflag ); // meshinfo->skip

	
	int SetMem( CVecMesh* srcbmesh, __int64 setflag );
	
	int SetMem( CVecLine* srcline, int lineno, __int64 setflag );
	int SetMem( CVecLine* srcline, int linevno, int linekind, __int64 setflag );

	int SetMem( CVec3f* srcmat, __int64 setflag );
	

	int SetMem( CVec3f* srcvec, int datano, __int64 setflag );
	int SetMem( CVec3f* srcvec, int lineno, int vecno, __int64 setflag );
	int SetMem( CVec3f* srcvec, int linevno, int linekind, int vecno, __int64 setflag );
	int SetMem( VEC3F* srcvec, int linevno, int linekind, int vnum );
	int SetMem( VEC3F* srcvec, int lineno, int vnum );

	int SetMem( float* srcfloat, int datano, __int64 setflag );
	int SetMem( float* srcfloat, int lineno, int vecno, __int64 setflag );
	int SetMem( float* srcfloat, int linevno, int linekind, int vecno, __int64 setflag );

	int	CopyData( CVecMesh* srcdata );

	int	DumpMem( HANDLE hfile, int tabnum, int dumpflag );

	
	// !!!!!! line���̒��_�����ς��class�ł́@�㏑���K�v�@!!!!!!!
	virtual int ConvDataNo( int* linenoptr, int* vecno, int datano );


	// �h���N���X�́@InitVecMesh(meshinfo, meshinfo) �������������ɁACheckMeshInfo �Ɓ@CheckLineInfo���@����������B
	virtual int CheckMeshInfo( CMeshInfo* srcmeshinfo );
	virtual int CheckLineInfo( CMeshInfo* lineinfo, int infonum );


		// ���ׂĂ�line���v�Z
	virtual int Transform( CMatrix2& transmat, CVecMesh* srcvmesh );


	CMeshInfo* meshinfo; // m �� line�̎�ށAn�͌��������̖{��
	//CVecLine* lineptr; // vecmesh[i][j] �ŃA�N�Z�X, line�̐��͐���i*j, line�̒��̒��_���́A�΂�΂���B
	

	unsigned long** hline;
	int allocleng; // allocate �����@CVecLine�̐��@�F�@AddVecLine �ɑΉ����邽�߁B

};


class CBezData : public CVecMesh
{
public:
	CBezData();
	~CBezData();

	int SetMem( CBezData* srcbez, __int64 setflag );
	int SetMem( CVec3f* srcvec, int srckind, __int64 setflag );

	int	CopyData( CBezData* srcdata );
	int InitVecMesh( CBezData* srcmesh );// InitVecMesh( meshinfo, lineinfo )���Ăԏ��������邾��

	virtual int CheckMeshInfo( CMeshInfo* srcmeshinfo ); //meshinfo->m = 5; meshinfo->n = 1;
	virtual int CheckLineInfo( CMeshInfo* lineinfo, int infonum ); //eachline->meshinfo->m = 1; eachline->meshinfo->n = 1;

};


class CBezLine : public CVecMesh
{
public:
	CBezLine();
	~CBezLine();

	int SetMem( CBezData* srcbezdat, int srcdatno, __int64 setflag ); //
	int SetMem( CBezLine* srcbezline, __int64 setflag );
	int	SetMem( CVec3f* srcvec, int srcvecleng, int bezno, __int64 setflag );
	

	int	CopyData( CBezLine* srcdata );

	int InitVecMesh( CBezLine* srcbline );

	virtual int CheckMeshInfo( CMeshInfo* srcmeshinfo ); //meshinfo->m = 5; meshinfo->n = 1;
	virtual int CheckLineInfo( CMeshInfo* lineinfo, int infonum ); //eachline->meshinfo->m = �C��(���); eachline->meshinfo->n = 1;
};

#endif