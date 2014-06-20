#ifndef QUATERNIONH

#define QUATERNIONH

#include <D3DX9.h>
#include <coef.h>

class CMatrix2;
class CQuaternion2;

class CQuaternion
{
public:
	CQuaternion();
	CQuaternion( float srcw, float srcx, float srcy, float srcz );
	~CQuaternion();
	int SetParams( float srcw, float srcx, float srcy, float srcz );

	int SetAxisAndRot( D3DXVECTOR3 srcaxis, float phai );
	int SetAxisAndRot( D3DXVECTOR3 srcaxis, double phai );

	int SetRotation( float degx, float degy, float degz );
	int SetRotation( double degx, double degy, double degz );

	int SetRotationGlobal( float degx, float degy, float degz );

	int GetAxisAndRot( D3DXVECTOR3* axisvecptr, float* frad );

	int Q2Eul2( CQuaternion* axisq, D3DXVECTOR3 befeul, D3DXVECTOR3* reteul );
	int ModifyEuler2( D3DXVECTOR3* eulerA, D3DXVECTOR3* eulerB );


	int QuaternionToAxisAngle( D3DXVECTOR3* dstaxis, float* dstrad );

	CQuaternion operator* (float srcw) const;
	CQuaternion &operator*= (float srcw);
	CQuaternion operator/ (float srcw) const;
	CQuaternion &operator/= (float srcw);
	CQuaternion operator+ (const CQuaternion &q) const;
	CQuaternion &operator+= (const CQuaternion &q);
	CQuaternion operator- (const CQuaternion &q) const;
	CQuaternion &operator-= (const CQuaternion &q);
	CQuaternion operator* (const CQuaternion &q) const;
	CQuaternion &operator*= (const CQuaternion &q);
	CQuaternion operator/ (const CQuaternion &q) const;
	CQuaternion &operator/= (const CQuaternion &q);
	CQuaternion operator- () const;
	//CQuaternion inv () const;
	CQuaternion normalize () const;
	
	int CopyFromCQuaternion2( CQuaternion2* q2 );
	
	int CQuaternion::inv( CQuaternion* dstq ); 
	
	bool CQuaternion::operator== (const CQuaternion &q) const { return w == q.w && x == q.x && y == q.y && z == q.z; }
	bool CQuaternion::operator!= (const CQuaternion &q) const { return !(*this == q); }

	float DotProduct( CQuaternion srcq );
	float CalcRad( CQuaternion srcq );

	CQuaternion Slerp( CQuaternion endq, int framenum, int frameno );

	int Squad( CQuaternion q0, CQuaternion q1, CQuaternion q2, CQuaternion q3, float t );
	
	CMatrix2 MakeRotMat();
	D3DXMATRIX MakeRotMatX();

	int RotationArc( D3DXVECTOR3 srcvec0, D3DXVECTOR3 srcvec1 );
	int Rotate( D3DXVECTOR3* dstvec, D3DXVECTOR3 srcvec );
	int Rotate( DVEC3* dstvec, DVEC3 srcvec );

	int CQuaternion2X( D3DXQUATERNION* dstx );
	int CQuaternion2X( D3DXQUATERNION* dstx, CQuaternion srcq );

	int transpose( CQuaternion* dstq );
	CQuaternion CalcFBXEul( CQuaternion befq, D3DXVECTOR3 befeul, D3DXVECTOR3* reteul );

private:


public:
	float w, x, y, z;
};
#endif