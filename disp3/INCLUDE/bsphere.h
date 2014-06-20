#ifndef CBSPHEREH
#define CBSPHEREH

#include <D3DX9.h>

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

class CBBox;


class CBSphere
{
public:
	CBSphere();
	~CBSphere();

	int InitParams();

	//int ChkInView( D3DXMATRIX matWorld, D3DXVECTOR3 viewpos, D3DXVECTOR3 viewvec, float projnear, float projfar, float* projindex );


	int ChkInView( FRUSTUMINFO* frinfo );
	int ChkInView( FRUSTUMINFO* frinfo, D3DXMATRIX matWorld );
	int GetFootOnPlane( D3DXVECTOR3* pfoot, D3DXVECTOR3* ppos, D3DXPLANE* pplane );


	//int ChkConflict( CBSphere* chkbs, int* resultptr, float srcrate = 1.0f, float chkrate = 1.0f );
	//int ChkConflict( D3DXVECTOR3* chkcenter, float chkr, int* resultptr, float srcrate = 1.0f, float chkrate = 1.0f );


	int SetBSFrom3V( D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3 );
	int SetBSphere( CBBox* srcbbx );
	int SetBSFromVEC3F( VEC3F* srcv, int srcvnum );

	int Transform2ScreenPos( DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int* scxptr, int* scyptr, int outflag = 0 );
	int Transform2ScreenPos( DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, int* scxptr, int* scyptr, float* sczptr, int outflag = 0 );
	int Transform2ScreenPos( DWORD dwClipWidth, DWORD dwClipHeight, D3DXMATRIX matView, D3DXMATRIX matProj, float aspect, D3DXVECTOR3* scpos );

	int ConvScreenPos( int projmode, float xp, float yp, float zp, float wp, float h, float w, float aspect, D3DTLVERTEX* dsttlv );

	int ChkConfMoveSphere( D3DXVECTOR3 Q1, D3DXVECTOR3 Q2, float rq, float srcrate );

	__inline int ChkConflict( CBSphere* chkbs, int* resultptr, float srcrate = 1.0f, float chkrate = 1.0f )
	{
		float diffx, diffy, diffz;
		diffx = tracenter.x - chkbs->tracenter.x;
		diffy = tracenter.y - chkbs->tracenter.y;
		diffz = tracenter.z - chkbs->tracenter.z;

		float dist;
		dist = diffx * diffx + diffy * diffy + diffz * diffz;	

		float maxdist;
		//maxdist = (rmag + chkbs->rmag) * (rmag + chkbs->rmag);
		maxdist = (rmag * srcrate + chkbs->rmag * chkrate) * (rmag * srcrate + chkbs->rmag * chkrate);

		if( dist <= maxdist ){
			*resultptr = 1;
		}else{
			*resultptr = 0;
		}

		return 0;
	};

	__inline int ChkConflict( D3DXVECTOR3* chkcenter, float chkr, int* resultptr, float srcrate = 1.0f, float chkrate = 1.0f )
	{
		float diffx, diffy, diffz;
		diffx = tracenter.x - chkcenter->x;
		diffy = tracenter.y - chkcenter->y;
		diffz = tracenter.z - chkcenter->z;

		float dist;
		dist = diffx * diffx + diffy * diffy + diffz * diffz;	

		float maxdist;
		maxdist = (rmag * srcrate + chkr * chkrate) * (rmag * srcrate + chkr * chkrate);

		if( dist <= maxdist ){
			*resultptr = 1;
		}else{
			*resultptr = 0;
		}


		return 0;
	};



private:

public:
	D3DXVECTOR3 befcenter; //bonematのみの変換後の中心
	D3DXVECTOR3 tracenter; //befcenter * worldMat の中心
	float	rmag; //パーツの中心からの半径
	//float   totalrmag; //モデル全体の中心からの半径
	int		visibleflag; //視野内か外かの判定結果フラグ
};


#endif