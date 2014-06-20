//------------------------------------------------------------------------------
/*!
\file LiSPSM.h
\brief Light Space Perspective Shadow Map
*/
//------------------------------------------------------------------------------

#ifndef		__LISPSM_H__
#define		__LISPSM_H__

//------------------------------------------------------------------------------
// インクルードファイル
//
#include	<d3d9.h>
#include	<d3dx9.h>
//#include	<vector>
//#include	"DXMain.h"
//using namespace std;

class CVec3List;


//------------------------------------------------------------------------------
//! LiSPSMステータス
typedef struct _SLispStatus{
	D3DXMATRIX		matProj;				//!< 投影行列
	D3DXMATRIX		matView;				//!< ビュー行列
	D3DXVECTOR3		vLightDir;				//!< ライトベクトル
	D3DXVECTOR3		vViewDir;				//!< ビューベクトル
	D3DXVECTOR3		vEyePos;				//!< 視点位置
	float			fNearDist;				//!< ビューのニアクリップ面の距離
	D3DXVECTOR3		newpos;
//	D3DXMATRIX		tmpmatView;
} SLispStatus;

/***
class	CVec3List{
private:
	vector<D3DXVECTOR3>		m_cPointList;

public:
	//! コンストラクタ
	CVec3List(){}
	//! デストラクタ
	~CVec3List(){ m_cPointList.clear(); }

	//! ポイントを追加する
	void	Add(D3DXVECTOR3& vec)
	{
		m_cPointList.push_back(vec);
	}

	//! ポイントをクリアする
	void	Clear(void)
	{
		m_cPointList.clear();
	}

	//! ポイントを変換する
	void	Transform(D3DXMATRIX* pMat)
	{
		int		num = m_cPointList.size();
		for(int i=0; i<num; i++){
			D3DXVECTOR3		vec = m_cPointList[i];
			D3DXVec3TransformCoord(&vec, &vec, pMat);
			m_cPointList[i] = vec;
		}
	}

	//! AABBを取得する
	void	GetAABB(D3DXVECTOR3* pMax, D3DXVECTOR3* pMin)
	{
		int		num = m_cPointList.size();
		if(num > 0){
			*pMax = *pMin = m_cPointList[0];
			for(int i=1; i<num; i++){
				if(pMax->x < m_cPointList[i].x)			{ pMax->x = m_cPointList[i].x; }
				else if(pMin->x > m_cPointList[i].x)	{ pMin->x = m_cPointList[i].x; }
				if(pMax->y < m_cPointList[i].y)			{ pMax->y = m_cPointList[i].y; }
				else if(pMin->y > m_cPointList[i].y)	{ pMin->y = m_cPointList[i].y; }
				if(pMax->z < m_cPointList[i].z)			{ pMax->z = m_cPointList[i].z; }
				else if(pMin->z > m_cPointList[i].z)	{ pMin->z = m_cPointList[i].z; }
			}
		}
	}

	CVec3List&	operator=(const CVec3List& b)
	{
		Clear();
		int		num = b.m_cPointList.size();
		for(int i=0; i<num; i++){
			m_cPointList.push_back(b.m_cPointList[i]);
		}
		return *this;
	}

	D3DXVECTOR3&	operator[](int n)
	{
		return m_cPointList[n];
	}
};
***/


void	CalcLispMatrix(SLispStatus* pStatus, CVec3List* pList);
void	CalcPointsOfViewBolume(CVec3List* pList, D3DXMATRIX& matView, float fAngle, float fAspect, float fNear, float fFar);




#endif

//	EOF
