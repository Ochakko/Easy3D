#ifndef VEC3LISTH
#define VEC3LISTH

#define VEC3LISTMAX	8000

#include <d3dx9.h>


class CBSphere;
class CBBox;

//------------------------------------------------------------------------------
//! ポイントリストクラス
class	CVec3List
{
public:
	D3DXVECTOR3		m_cPointList[VEC3LISTMAX];
	int				m_pointnum;

public:
	CVec3List();
	~CVec3List();

	//! ポイントを変換する
	void	Transform(D3DXMATRIX* pMat);
	//! AABBを取得する
	void	GetAABB(D3DXVECTOR3* pMax, D3DXVECTOR3* pMin);

	int AddPoint( D3DXVECTOR3* srcv );

	int AddBSphere( CBSphere* srcbs, float srcscale );
	int AddBBox( CBBox* srcbbx );



	CVec3List&	operator=(const CVec3List& b)
	{
		for( int i = 0; i < m_pointnum; i++ ){
			m_cPointList[i] = b.m_cPointList[i];
		}
		m_pointnum = b.m_pointnum;
		return *this;
	}

	D3DXVECTOR3&	operator[](int n)
	{
		return m_cPointList[n];
	}
};


#endif