//------------------------------------------------------------------------------
/*!
\file LiSPSM.cpp
\brief Light Space Perspective Shadow Map
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// インクルードファイル
//

#include	<stdafx.h>
#include	<LiSPSM.h>

#include <Vec3List.h>


//------------------------------------------------------------------------------
//! UPベクトルを求める
void	CalcUpVector(D3DXVECTOR3* pUp, const D3DXVECTOR3* pView, const D3DXVECTOR3 *pLight)
{
	D3DXVECTOR3		left;

	D3DXVec3Cross(&left, pView, pLight);
	D3DXVec3Cross(pUp, pLight, &left);
	D3DXVec3Normalize(pUp, pUp);
}

//------------------------------------------------------------------------------
//! 行列を (-1, -1, -1) ～ (1, 1, 1) の範囲にあわせる
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
//! LiSPSMの変換行列を求める
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

	// アップベクトルを求める
	CalcUpVector(&up, &(pStatus->vViewDir), &(pStatus->vLightDir));

	// カメラ視点、ライトベクトル、アップベクトルからビュー行列を求める
	D3DXMatrixLookAtLH(&(pStatus->matView), &(pStatus->vEyePos), &(pStatus->vEyePos + pStatus->vLightDir), &up);
//	pStatus->tmpmatView = pStatus->matView;

	// ポイントリストを変換する
	ListCopy.Transform(&(pStatus->matView));

	// 変換したポイントリストを内包するAABBを取得する
	ListCopy.GetAABB(&max, &min);

	// ライトスペースに垂直なビューのニアクリップ面とファークリップ面を求める
	// AABB はライトのビュー行列で変換されているので y が上記ビューの奥行きになる
#if		0
	float			factor = 1.0f / sinGamma;
	float			z_n = factor * pStatus->fNearDist;
	float			d = max.y - min.y;
	float			z_f = z_n + d * sinGamma;
	float			n = (z_n + sqrtf(z_f * z_n)) * factor;
	float			f = n + d;
	D3DXVECTOR3		pos;

	// 新しい視点を求める
	pos = pStatus->vEyePos - (up * (n - pStatus->fNearDist));

	pStatus->newpos = pos;


	// 新しいライトビュー行列を求める
	D3DXMatrixLookAtLH(&(pStatus->matView), &pos, &(pos + pStatus->vLightDir), &up);

	// ニアクリップ面とファークリップ面からY方向への透視変換行列を求める
	D3DXMatrixIdentity(&matLisp);
	matLisp._22 = f / (f - n);
	matLisp._42 = -f * n / (f - n);
	matLisp._24 = 1.0f;
	matLisp._44 = 0.0f;

	// 現在のライト視点からの変換行列を求める
	D3DXMATRIX	matLVP;
	D3DXMatrixMultiply(&matLVP, &(pStatus->matView), &matLisp);

	// この行列で元のポイントリストを変換する
	ListCopy = *pList;
	ListCopy.Transform(&matLVP);
#else
	float	d = max.y - min.y;
	float	n = (min.y > 0.0f) ? (min.y + d * 0.1f) : (d * 0.25f);
	float	f = n + d;
	float	p = min.y - n;

	// ニアクリップ面とファークリップ面からY方向への透視変換行列を求める
	D3DXMatrixIdentity(&matLisp);
	matLisp._22 = f / (f - n);
	matLisp._42 = -f * n / (f - n);
	matLisp._24 = 1.0f;
	matLisp._44 = 0.0f;

	// 現在のライト視点からの変換行列を求める
	D3DXMATRIX	matLVP;
	D3DXMatrixTranslation(&matLVP, 0.0f, -p, 0.0f);
	D3DXMatrixMultiply(&(pStatus->matView), &(pStatus->matView), &matLVP);
	D3DXMatrixMultiply(&matLVP, &matLVP, &matLisp);

	// この行列でポイントリストを変換する
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
	// AABBを取得する
	ListCopy.GetAABB(&max, &min);

	// 上記のAABBが (-1, -1, 0) ～ (1, 1, 1) の範囲になるような行列を作成する
	// 位置とサイズを正確なものにするため
	ScaleMatrixToFit(&(pStatus->matProj), &max, &min);

	// 最終的な透視変換行列を求める
	D3DXMatrixMultiply(&(pStatus->matProj), &matLisp, &(pStatus->matProj));
}

//------------------------------------------------------------------------------
//! 視錐台の8点を求める
/*!
\param	pList		out : 視錐台の8点のリスト
\param	matView		in : ビュー行列
\param	fAngle		in : ビューアングル
\param	fAspect		in : アスペクト比
\param	fNear		in : ニアプレーン距離
\param	fFar		in : ファープレーン距離
*/
void	CalcPointsOfViewBolume(CVec3List* pList, D3DXMATRIX& matView, float fAngle, float fAspect, float fNear, float fFar)
{

	float	t = tanf(fAngle / 2.0f);
	float	fNLen = t * fNear;
	float	fFLen = t * fFar;
	D3DXVECTOR3		vPos;

	// ニアプレーンの点を求める
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

	// ファープレーンの点を求める
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

	// ビュー行列の逆行列で変換する
	D3DXMATRIX	matInvView;
	D3DXMatrixInverse(&matInvView, NULL, &matView);
	pList->Transform(&matInvView);
}




//	EOF
