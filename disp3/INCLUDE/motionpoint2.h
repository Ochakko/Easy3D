#ifndef MOTIONPOINT2H
#define MOTIONPOINT2H

#include <coef.h>
#include <Quaternion.h>
#include <D3DX9.h>

#include <MotionPoint.h>

#include <SPParam.h>

class CMotionPoint2 : public CMotionPoint
{
public:
	CMotionPoint2();
	~CMotionPoint2();

	int InitParams();
	int CreateSppIfNot();

	int SetParams( int srcframe, CQuaternion srcq, 
		float srcmvx, float srcmvy, float srcmvz, DWORD srcdispswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1 );

	int SetDispSwitch( DWORD srcdswitch );

	int FillUpMotionPoint( CQuaternion* axisq, CMotionPoint2* srcstartmp, CMotionPoint2* srcendmp, int framenum, int framecnt, int interp );

	int CopyMotion( CMotionPoint2* srcmp );


	int AddToPrev( CMotionPoint2* addmp );
	int AddToNext( CMotionPoint2* addmp );
	int LeaveFromChain();

	int MvHermiteSpline( CMotionPoint2* mp0, CMotionPoint2* mp1, CMotionPoint2* mp2, CMotionPoint2* mp3, float t );
	int ScaleHermiteSpline( CMotionPoint2* mp0, CMotionPoint2* mp1, CMotionPoint2* mp2, CMotionPoint2* mp3, float t );
	float HermiteSpline( float val1, float val2, float val3, float val4, float t );

	int IsInitMP();
	int IsSameMP( CMotionPoint2* cmpmp );

	int RotBSpline( CQuaternion* axisq, CMotionPoint2* startmp, CMotionPoint2* endmp, int srcx );
	int CalcSPPoint( SPPARAM* spp, D3DXVECTOR2* startp, D3DXVECTOR2* endp );
	int CalcSPPointScale( SPPARAM* spp, D3DXVECTOR2* startp, D3DXVECTOR2* endp );
	float CalcBSpline( float x, D3DXVECTOR2 pos1, D3DXVECTOR2 pos2, D3DXVECTOR2 pos3 );

	int MvBSpline( CMotionPoint2* startmp, CMotionPoint2* endmp, int srcx );
	int ScBSpline( CMotionPoint2* startmp, CMotionPoint2* endmp, int srcx );

	int ScaleSplineMv( CMotionPoint2* endmp, D3DXVECTOR3 srcmult );
	int CalcFBXEul( CQuaternion* srcq, D3DXVECTOR3 befeul, D3DXVECTOR3* reteul );

public:

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// クラスのインスタンス丸ごと、quaファイルに書き出すので、メンバーを増やす場合は、必ず、reserved変数を使うこと。
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	DWORD dispswitch;


	CMotionPoint2* prev;
	CMotionPoint2* next;

	//拡張用
	//int serialno, interp, reservedint3, reservedint4, reservedint5;
	
	int serialno, interp;
	float m_scalex, m_scaley, m_scalez;//2005/04/22
	
	int m_userint1;
	D3DXVECTOR3 m_eul;
	//float reservedfl2, reservedfl3, reservedfl4;

	
	CSPParam* m_spp;
	//float reservedfl5;
	//float reservedfl1, reservedfl2, reservedfl3, reservedfl4, reservedfl5;
};

#endif