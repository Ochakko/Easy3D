#ifndef BBox2H
#define BBox2H

#include <D3DX9.h>
#include <PCoef.h>
#include <Line.h>

#include <BBox.h>

class CShdHandler;
class CShdElem;
class CPolyMesh2;
class CD3DDisp;

class CMotHandler;


class CBBox2
{
public:
	CBBox2();
	~CBBox2();

	int InvalidateDispObj();
	int ResetPrimNum();
	int DestroyObjs();
	int CalcInitialInfElem();

	//int SetPolyMesh2( CPolyMesh2* pm2, CD3DDisp* srcd3ddisp, float srcfacet );

	int CheckVertInScope( D3DXVECTOR3 srcv, int* insideptr );

	////////////
	
	int SetBBox( CShdHandler* srclpsh, CShdElem* srctarget, CShdElem* srcjoint, int* vertarray, int vertnum );
	int TransformOnlyWorld( CMotHandler* srclpmh, D3DXMATRIX matWorld );

	int ChkConflict( CBBox2* chkbbx );

private:
	int InitParams();

	int ChkConfVecAndFace2( CLine* srcline, CPCoef* curpc, D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3, int* resultptr );

//	BOOL IntersectTriangle( const D3DXVECTOR3& orig,
//                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
//                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
//                                       FLOAT* t, FLOAT* u, FLOAT* v );

__inline BOOL CBBox2::IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* t, FLOAT* u, FLOAT* v )
{
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

	if( (*t >= 0.0f) && (*t <= 1.0f) ){
		return TRUE;
	}else{
		return FALSE;
	}

    //return TRUE;
};




public:
	int dispflag;
	CShdElem* applychild;
	CShdElem* target;
	CPolyMesh2* polymesh2;
	CD3DDisp* d3ddisp;
	float pm2facet;


	D3DXVECTOR3 R0, S0, T0;//単位長の３軸ベクトル
	D3DXVECTOR3 Q;
	float lengR0, lengS0, lengT0;//R0, S0, T0の長さ

	int invindex;
	int pm2index[12][3];

	D3DXVECTOR3 m_vert[8];//８頂点
	D3DXVECTOR3 m_transvert[8];//ボーン変形後の８頂点

	CPCoef m_pcoef[6];//ボーン変形後の６平面
	CLine m_line[12];//ボーン変形後の12辺	

	CBBox m_bbx1;

};

#endif
