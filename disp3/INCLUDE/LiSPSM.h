//------------------------------------------------------------------------------
/*!
\file LiSPSM.h
\brief Light Space Perspective Shadow Map
*/
//------------------------------------------------------------------------------

#ifndef		__LISPSM_H__
#define		__LISPSM_H__

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//
#include	<d3d9.h>
#include	<d3dx9.h>
//#include	<vector>
//#include	"DXMain.h"
//using namespace std;

class CVec3List;


//------------------------------------------------------------------------------
//! LiSPSM�X�e�[�^�X
typedef struct _SLispStatus{
	D3DXMATRIX		matProj;				//!< ���e�s��
	D3DXMATRIX		matView;				//!< �r���[�s��
	D3DXVECTOR3		vLightDir;				//!< ���C�g�x�N�g��
	D3DXVECTOR3		vViewDir;				//!< �r���[�x�N�g��
	D3DXVECTOR3		vEyePos;				//!< ���_�ʒu
	float			fNearDist;				//!< �r���[�̃j�A�N���b�v�ʂ̋���
	D3DXVECTOR3		newpos;
//	D3DXMATRIX		tmpmatView;
} SLispStatus;

/***
class	CVec3List{
private:
	vector<D3DXVECTOR3>		m_cPointList;

public:
	//! �R���X�g���N�^
	CVec3List(){}
	//! �f�X�g���N�^
	~CVec3List(){ m_cPointList.clear(); }

	//! �|�C���g��ǉ�����
	void	Add(D3DXVECTOR3& vec)
	{
		m_cPointList.push_back(vec);
	}

	//! �|�C���g���N���A����
	void	Clear(void)
	{
		m_cPointList.clear();
	}

	//! �|�C���g��ϊ�����
	void	Transform(D3DXMATRIX* pMat)
	{
		int		num = m_cPointList.size();
		for(int i=0; i<num; i++){
			D3DXVECTOR3		vec = m_cPointList[i];
			D3DXVec3TransformCoord(&vec, &vec, pMat);
			m_cPointList[i] = vec;
		}
	}

	//! AABB���擾����
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
