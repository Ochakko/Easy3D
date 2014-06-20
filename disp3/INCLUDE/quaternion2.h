#ifndef QUATERNION2H

#define QUATERNION2H

#include <D3DX9.h>

#include <quaternion.h>

class CMatrix2;

class CQuaternion2
{
public:
	CQuaternion2();
	CQuaternion2( float srcw, float srcx, float srcy, float srcz, float srctwist );
	~CQuaternion2();
	int SetParams( float srcw, float srcx, float srcy, float srcz, float srctwist );

	int SetAxisAndRot( D3DXVECTOR3 srcaxis, float phai );
	int SetRotation( float degx, float degy, float degz );
	int SetRotationGlobal( float degx, float degy, float degz );

	int GetAxisAndRot( D3DXVECTOR3* axisvecptr, float* frad );

	float GetTwist()
	{
		return twist;
	}
	int SetTwist( float srctwist )
	{
		twist = srctwist;
		return 0;
	}

	int QuaternionToAxisAngle( D3DXVECTOR3* dstaxis, float* dstrad );


	CQuaternion2 operator* (float srcw) const;
	CQuaternion2 &operator*= (float srcw);
	CQuaternion2 operator/ (float srcw) const;
	CQuaternion2 &operator/= (float srcw);
	CQuaternion2 operator+ (const CQuaternion2 &q) const;
	CQuaternion2 &operator+= (const CQuaternion2 &q);
	CQuaternion2 operator- (const CQuaternion2 &q) const;
	CQuaternion2 &operator-= (const CQuaternion2 &q);
	CQuaternion2 operator* (const CQuaternion2 &q) const;
	CQuaternion2 &operator*= (const CQuaternion2 &q);
	CQuaternion2 operator/ (const CQuaternion2 &q) const;
	CQuaternion2 &operator/= (const CQuaternion2 &q);
	CQuaternion2 operator- () const;

	//CQuaternion2 operator= (const CQuaternion &q);
	//CQuaternion operator= (const CQuaternion2 &q2 );

	int CopyFromCQuaternion( CQuaternion* srcq );


	//CQuaternion2 inv () const;
	CQuaternion2 normalize () const;
	
	int CQuaternion2::inv( CQuaternion2* dstq ); 
	
	bool CQuaternion2::operator== (const CQuaternion2 &q) const { return w == q.w && x == q.x && y == q.y && z == q.z && twist == q.twist; }
	bool CQuaternion2::operator!= (const CQuaternion2 &q) const { return !(*this == q); }

	float DotProduct( CQuaternion2 srcq );
	float CalcRad( CQuaternion2 srcq );

	CQuaternion2 Slerp( CQuaternion2 endq, int framenum, int frameno );
	CQuaternion2 Slerp( CQuaternion2 endq, float t );

	int Squad( CQuaternion2 q0, CQuaternion2 q1, CQuaternion2 q2, CQuaternion2 q3, float t );
	
	CMatrix2 MakeRotMat();
	D3DXMATRIX MakeRotMatX();

	int RotationArc( D3DXVECTOR3 srcvec0, D3DXVECTOR3 srcvec1 );
	int Rotate( D3DXVECTOR3* dstvec, D3DXVECTOR3 srcvec );

	int CQuaternion22X( D3DXQUATERNION* dstx );
	int CQuaternion22X( D3DXQUATERNION* dstx, CQuaternion2 srcq );

	int LookAt( D3DXVECTOR3 tarvec, D3DXVECTOR3 basevec, float fmindeg, int upflag, int gradyflag, CQuaternion2* finalq, D3DXVECTOR3* dstup, CQuaternion2* dstrot1, CQuaternion2* dstrot2 );
	
	int InitTempQ();
	int DestroyTempQ();

	int CheckSault( D3DXVECTOR3 basevec, D3DXVECTOR3 tarvec, int upflag, int* resptr );
	int Get180Axis( D3DXVECTOR3 tarvec, D3DXVECTOR3* axisptr );
	int ShiftVecNearY( D3DXVECTOR3 basevec, D3DXVECTOR3 tarvec, int* shiftflag, D3DXVECTOR3* shifttarvec );

	int GradUp( CQuaternion2 befq, D3DXVECTOR3 basevec, D3DXVECTOR3 upvec, CQuaternion2 rot1, CQuaternion2 rot2 );
	int GetDividedTarget( D3DXVECTOR3 basevec, D3DXVECTOR3 target, D3DXVECTOR3 lasttarget, int olddivnum, D3DXVECTOR3* newtargetptr, int* newdivnumptr );


private:
	int DXVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3* srcvec );
	int DXVec3Cross( D3DXVECTOR3* dstvec, D3DXVECTOR3* pV1, D3DXVECTOR3* pV2 );
	int GetShadow( D3DXVECTOR3* dstshadow, D3DXVECTOR3 planedir, D3DXVECTOR3 planep, D3DXVECTOR3 srcp, D3DXVECTOR3 srcdir );
	int LookAtQ( CQuaternion2* dstqptr, CQuaternion2 befq, D3DXVECTOR3 targvec, D3DXVECTOR3 upvec, D3DXVECTOR3 basevec, float fmindeg, int keepupflag, int gradyflag, CQuaternion2* finalq, CQuaternion2* dstrot1, CQuaternion2* dstrot2 );
	int GetFootPos2D( D3DXVECTOR2 o0, D3DXVECTOR2 p1, D3DXVECTOR2 p2, D3DXVECTOR2* footpos2d, int* betweenptr, float* rett );


public:
	float w, x, y, z;
	float twist;
};
#endif