#ifndef NAVILINEH
#define NAVILINEH

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


class CNaviPoint;

class CNaviLine
{
public:
	CNaviLine();
	~CNaviLine();

public:
	CNaviPoint* GetNaviPoint( int npid );
	
	int AddNaviPoint( int befnpid, int ctrlflag, int* npidptr );
	int RemoveNaviPoint( int npid );

	int GetNaviPointPos( int npid, D3DXVECTOR3* posptr );
	int SetNaviPointPos( int npid, D3DXVECTOR3* srcpos );

	int GetNaviPointOwnerID( int npid, int* oidptr );
	int SetNaviPointOwnerID( int npid, int oid );

	int GetNextNaviPoint( int npid, int* nextptr );
	int GetPrevNaviPoint( int npid, int* prevptr );

	int GetNearestNaviPoint( D3DXVECTOR3* srcpos, int* nearptr, int* prevptr, int* nextptr );

	int FillUpNaviLine( int div, int flag );
	int ReorderNaviPoint( int* oldnoptr, int ptrlen, int* setlenptr );

	int ControlByNaviLine( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, CQuaternion srcq, 
			int ctrlmode, int roundflag, int reverseflag,
			float maxdist,
			float posstep, float dirstep,
			D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr );

private:
	void InitParams();
	CNaviPoint* GetLastNaviPoint();

	CNaviPoint* GetTargetNaviPoint( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, int roundflag, int reverseflag, int ctrlmode );
	CNaviPoint* GetTargetNaviPoint_regular( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, int roundflag, int ctrlmode );
	CNaviPoint* GetTargetNaviPoint_reverse( D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, int roundflag, int ctrlmode );

	int DirToXZ( D3DXVECTOR3 newdir, CQuaternion* dstq );
	int DirTo( D3DXVECTOR3 newdir, CQuaternion* dstq );
	
	int LimitedDirToXZ( CQuaternion srcq, float maxdeg, D3DXVECTOR3 newdir, CQuaternion* dstq, D3DXVECTOR3* setdir );
	int LimitedDirTo( CQuaternion srcq, float maxdeg, D3DXVECTOR3 newdir, CQuaternion* dstq, D3DXVECTOR3* setdir );


	int CreateCatmullRomPoint( CNaviPoint* np1, CNaviPoint* np2, CNaviPoint* np3, CNaviPoint* np4, int div );
	//int CreateCatmullRomPoint( D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4, div );


	int ControlByNL_Pos( CNaviPoint* targetnp, int xzflag, D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, CQuaternion srcq, 
			int roundflag, int reverseflag,
			float posstep, float dirstep,
			D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr );

	int ControlByNL_Dir( CNaviPoint* targetnp, int xzflag, D3DXVECTOR3 srcpos, D3DXVECTOR3 offset, CQuaternion srcq, 
			int roundflag, int reverseflag,
			float maxdist,
			float posstep, float dirstep,
			D3DXVECTOR3* newposptr, CQuaternion* newqptr, int* tpidptr );

	int GetVerticalPos( D3DXVECTOR3* pos1, D3DXVECTOR3* pos2, D3DXVECTOR3* pos3, int clampflag, D3DXVECTOR3* dstp );

public:
	int lineid;//e3dì‡Ç≈ÅAàÍà”ÇÃî‘çÜ
	int pnum;
	CNaviPoint* nphead;
	
	CNaviLine* next;

};


#endif
