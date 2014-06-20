//------------------------------------------------------------------------------
/*!
\file LiSPSM.cpp
\brief Light Space Perspective Shadow Map
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//

#include	<stdafx.h>
#include	<LiSPSM.h>

#include <Vec3List.h>


//------------------------------------------------------------------------------
//! UP�x�N�g�������߂�
void	CalcUpVector(D3DXVECTOR3* pUp, const D3DXVECTOR3* pView, const D3DXVECTOR3 *pLight)
{
	D3DXVECTOR3		left;

	D3DXVec3Cross(&left, pView, pLight);
	D3DXVec3Cross(pUp, pLight, &left);
	D3DXVec3Normalize(pUp, pUp);
}

//------------------------------------------------------------------------------
//! �s��� (-1, -1, -1) �` (1, 1, 1) �͈̔͂ɂ��킹��
void	ScaleMatrixToFit(D3DXMATRIX* pOut, const D3DXVECTOR3* pMax, const D3DXVECTOR3* pMin)
{
	pOut->_11 = 2.0f / (pMax->x - pMin->x);
	pOut->_21 = 0.0f;
	pOut->_31 = 0.0f;
	pOut->_41 = -(pMax->x + pMin->x) / (pMax->x - pMin->x);

	pOut->_12 = 0.0f;
	pOut->_22 = 2.0f / (pMax->y - pMin->y);
	pOut->_32 = 0.0f;
	pOut->_42 = -(pMax->y + pMin->y) / (pMax->y - pMin->y);

	pOut->_13 = 0.0f;
	pOut->_23 = 0.0f;
	pOut->_33 = 1.0f / (pMax->z - pMin->z);
	pOut->_43 = -pMin->z / (pMax->z - pMin->z);

	pOut->_14 = 0.0f;
	pOut->_24 = 0.0f;
	pOut->_34 = 0.0f;
	pOut->_44 = 1.0f;
}

//------------------------------------------------------------------------------
//! LiSPSM�̕ϊ��s������߂�
/*!
*/
void	CalcLispMatrix(SLispStatus* pStatus, CVec3List* pList)
{
	D3DXVECTOR3		min, max;
	D3DXVECTOR3		up;
	D3DXMATRIX		matLisp;
	CVec3List		ListCopy = *pList;
	float			cosGamma = D3DXVec3Dot(&(pStatus->vViewDir), &(pStatus->vLightDir));
	float			sinGamma = sqrtf(1.0f - cosGamma * cosGamma);

	// �A�b�v�x�N�g�������߂�
	CalcUpVector(&up, &(pStatus->vViewDir), &(pStatus->vLightDir));

	// �J�������_�A���C�g�x�N�g���A�A�b�v�x�N�g������r���[�s������߂�
	D3DXMatrixLookAtLH(&(pStatus->matView), &(pStatus->vEyePos), &(pStatus->vEyePos + pStatus->vLightDir), &up);
//	pStatus->tmpmatView = pStatus->matView;

	// �|�C���g���X�g��ϊ�����
	ListCopy.Transform(&(pStatus->matView));

	// �ϊ������|�C���g���X�g������AABB���擾����
	ListCopy.GetAABB(&max, &min);

	// ���C�g�X�y�[�X�ɐ����ȃr���[�̃j�A�N���b�v�ʂƃt�@�[�N���b�v�ʂ����߂�
	// AABB �̓��C�g�̃r���[�s��ŕϊ�����Ă���̂� y ����L�r���[�̉��s���ɂȂ�
#if		0
	float			factor = 1.0f / sinGamma;
	float			z_n = factor * pStatus->fNearDist;
	float			d = max.y - min.y;
	float			z_f = z_n + d * sinGamma;
	float			n = (z_n + sqrtf(z_f * z_n)) * factor;
	float			f = n + d;
	D3DXVECTOR3		pos;

	// �V�������_�����߂�
	pos = pStatus->vEyePos - (up * (n - pStatus->fNearDist));

	pStatus->newpos = pos;


	// �V�������C�g�r���[�s������߂�
	D3DXMatrixLookAtLH(&(pStatus->matView), &pos, &(pos + pStatus->vLightDir), &up);

	// �j�A�N���b�v�ʂƃt�@�[�N���b�v�ʂ���Y�����ւ̓����ϊ��s������߂�
	D3DXMatrixIdentity(&matLisp);
	matLisp._22 = f / (f - n);
	matLisp._42 = -f * n / (f - n);
	matLisp._24 = 1.0f;
	matLisp._44 = 0.0f;

	// ���݂̃��C�g���_����̕ϊ��s������߂�
	D3DXMATRIX	matLVP;
	D3DXMatrixMultiply(&matLVP, &(pStatus->matView), &matLisp);

	// ���̍s��Ō��̃|�C���g���X�g��ϊ�����
	ListCopy = *pList;
	ListCopy.Transform(&matLVP);
#else
	float	d = max.y - min.y;
	float	n = (min.y > 0.0f) ? (min.y + d * 0.1f) : (d * 0.25f);
	float	f = n + d;
	float	p = min.y - n;

	// �j�A�N���b�v�ʂƃt�@�[�N���b�v�ʂ���Y�����ւ̓����ϊ��s������߂�
	D3DXMatrixIdentity(&matLisp);
	matLisp._22 = f / (f - n);
	matLisp._42 = -f * n / (f - n);
	matLisp._24 = 1.0f;
	matLisp._44 = 0.0f;

	// ���݂̃��C�g���_����̕ϊ��s������߂�
	D3DXMATRIX	matLVP;
	D3DXMatrixTranslation(&matLVP, 0.0f, -p, 0.0f);
	D3DXMatrixMultiply(&(pStatus->matView), &(pStatus->matView), &matLVP);
	D3DXMatrixMultiply(&matLVP, &matLVP, &matLisp);

	// ���̍s��Ń|�C���g���X�g��ϊ�����
	ListCopy.Transform(&matLVP);


	D3DXVECTOR3 camview( 0.0f, 0.0f, 0.0f );
	D3DXMATRIX invview;
	D3DXMatrixInverse( &invview, NULL, &(pStatus->matView) );
	D3DXVec3TransformCoord( &(pStatus->newpos), &camview, &invview );


//	D3DXVECTOR3 camview0( 0.0f, 0.0f, 0.0f );
//	D3DXVECTOR3 camview1( 0.0f, 0.0f, 1.0f );
//	
//	D3DXMATRIX invview;
//	D3DXMatrixInverse( &invview, NULL, &(pStatus->matView) );
//
//	D3DXVECTOR3 camp0, camp1;
//	D3DXVec3TransformCoord( &camp0, &camview0, &invview );
//	D3DXVec3TransformCoord( &camp1, &camview1, &invview );
//
//	D3DXVECTOR3 vec10;
//	vec10 = camp0 - camp1;
//	D3DXVec3Normalize( &vec10, &vec10 );
//
//	pStatus->newpos = camp0 + pStatus->fNearDist * vec10;


#endif
	// AABB���擾����
	ListCopy.GetAABB(&max, &min);

	// ��L��AABB�� (-1, -1, 0) �` (1, 1, 1) �͈̔͂ɂȂ�悤�ȍs����쐬����
	// �ʒu�ƃT�C�Y�𐳊m�Ȃ��̂ɂ��邽��
	ScaleMatrixToFit(&(pStatus->matProj), &max, &min);

	// �ŏI�I�ȓ����ϊ��s������߂�
	D3DXMatrixMultiply(&(pStatus->matProj), &matLisp, &(pStatus->matProj));
}

//------------------------------------------------------------------------------
//! �������8�_�����߂�
/*!
\param	pList		out : �������8�_�̃��X�g
\param	matView		in : �r���[�s��
\param	fAngle		in : �r���[�A���O��
\param	fAspect		in : �A�X�y�N�g��
\param	fNear		in : �j�A�v���[������
\param	fFar		in : �t�@�[�v���[������
*/
void	CalcPointsOfViewBolume(CVec3List* pList, D3DXMATRIX& matView, float fAngle, float fAspect, float fNear, float fFar)
{

	float	t = tanf(fAngle / 2.0f);
	float	fNLen = t * fNear;
	float	fFLen = t * fFar;
	D3DXVECTOR3		vPos;

	// �j�A�v���[���̓_�����߂�
	vPos.x = fNLen * fAspect;
	vPos.y = fNLen;
	vPos.z = fNear;
	pList->AddPoint( &vPos );
	vPos.x = -vPos.x;
	pList->AddPoint( &vPos );
	vPos.y = -vPos.y;
	pList->AddPoint( &vPos );
	vPos.x = -vPos.x;
	pList->AddPoint( &vPos );

	// �t�@�[�v���[���̓_�����߂�
	vPos.x = fFLen * fAspect;
	vPos.y = fFLen;
	vPos.z = fFar;
	pList->AddPoint( &vPos );
	vPos.x = -vPos.x;
	pList->AddPoint( &vPos );
	vPos.y = -vPos.y;
	pList->AddPoint( &vPos );
	vPos.x = -vPos.x;
	pList->AddPoint( &vPos );

	// �r���[�s��̋t�s��ŕϊ�����
	D3DXMATRIX	matInvView;
	D3DXMatrixInverse(&matInvView, NULL, &matView);
	pList->Transform(&matInvView);
}




//	EOF
