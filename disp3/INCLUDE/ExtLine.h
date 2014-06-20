#ifndef EXTLINEH
#define EXTLINEH

#include <D3DX9.h>

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>
#include <quaternion.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>


class CExtPoint;

class CExtLine : public CBaseDat
{
public:
	CExtLine();
	~CExtLine();

public:
	int SetMeshInfo( CMeshInfo* srcmeshinfo );

	CExtPoint* GetExtPoint( int epid );
	
	int AddExtPoint( int befepid, int ctrlflag, int* epidptr );
	int RemoveExtPoint( int epid );

	int GetExtPointPos( int epid, D3DXVECTOR3* posptr );
	int SetExtPointPos( int epid, D3DXVECTOR3* srcpos );

	//int GetExtPointOwnerID( int epid, int* oidptr );
	//int SetExtPointOwnerID( int epid, int oid );

	int GetNextExtPoint( int epid, int* nextptr );
	int GetPrevExtPoint( int epid, int* prevptr );

	//int GetNearestExtPoint( D3DXVECTOR3* srcpos, int* nearptr, int* prevptr, int* nextptr );

	//int FillUpExtLine( int div, int flag );
	//int ReorderExtPoint( int* oldnoptr, int ptrlen, int* setleeptr );

	//int ControlByExtLine( D3DXVECTOR3 srcpos, CQuaternion srcq, 
	//		int ctrlmode, int roundflag, int reverseflag,
	//		float maxdist,
	//		float posstep, float dirstep,
	//		D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr );

private:
	void InitParams();
	CExtPoint* GetLastExtPoint();

	//CExtPoint* GetTargetExtPoint( D3DXVECTOR3 srcpos, int roundflag, int reverseflag, int ctrlmode );
	//CExtPoint* GetTargetExtPoint_regular( D3DXVECTOR3 srcpos, int roundflag, int ctrlmode );
	//CExtPoint* GetTargetExtPoint_reverse( D3DXVECTOR3 srcpos, int roundflag, int ctrlmode );

	//int DirToXZ( D3DXVECTOR3 newdir, CQuaternion* dstq );
	//int DirTo( D3DXVECTOR3 newdir, CQuaternion* dstq );
	
	//int LimitedDirToXZ( CQuaternion srcq, float maxdeg, D3DXVECTOR3 newdir, CQuaternion* dstq, D3DXVECTOR3* setdir );
	//int LimitedDirTo( CQuaternion srcq, float maxdeg, D3DXVECTOR3 newdir, CQuaternion* dstq, D3DXVECTOR3* setdir );


	//int CreateCatmullRomPoint( CExtPoint* ep1, CExtPoint* ep2, CExtPoint* ep3, CExtPoint* ep4, int div );
	////int CreateCatmullRomPoint( D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4, div );


	//int ControlByNL_Pos( CExtPoint* targetep, int xzflag, D3DXVECTOR3 srcpos, CQuaternion srcq, 
	//		int roundflag, int reverseflag,
	//		float posstep, float dirstep,
	//		D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr );

	//int ControlByNL_Dir( CExtPoint* targetep, int xzflag, D3DXVECTOR3 srcpos, CQuaternion srcq, 
	//		int roundflag, int reverseflag,
	//		float maxdist,
	//		float posstep, float dirstep,
	//		D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr );

	//int GetVerticalPos( D3DXVECTOR3* pos1, D3DXVECTOR3* pos2, D3DXVECTOR3* pos3, int clampflag, D3DXVECTOR3* dstp );

public:
	int lineid;//e3d内で、一意の番号
	int pnum;
	CExtPoint* ephead;
	

	CMeshInfo* meshinfo; // mは、描画頂点数、nは、拡張可能な最大頂点数、totalは使わない。

	//CExtLine* next;

};


#endif
