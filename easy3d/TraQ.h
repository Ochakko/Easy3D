#ifndef TRAQH
#define TRAQH

#include <d3dx9.h>
#include <usercoef.h>
#include "quaternion.h"

class CShdHandler;

class CTraQ
{
public:
	CTraQ();
	~CTraQ();

	int InitParams();
	int CalcQ( CShdHandler* lpsh, TSELEM* tsptr, CTraQ* traqptr, RPSELEM* rpsptr, int frameno, int pivotskel, int skelno, int skipflag );
	int CalcTorso( CTraQ* traqptr, RPSELEM* rpsptr, int frameno, int skipflag );
	int CalcNeck(  CShdHandler* lpsh, TSELEM* tsptr, CTraQ* traqptr, RPSELEM* rpsptr, int frameno, int skipflag );
	//int QtoEuler( D3DXVECTOR3* eulptr, D3DXVECTOR3 befeul );

	int QtoEul( CShdHandler* lpsh, CQuaternion srcq, D3DXVECTOR3 befeul, int axisflag, int boneno, D3DXVECTOR3* eulptr, CQuaternion* axisqptr );

private:
	int DCalcDiffQ( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2, CQuaternion* dstq ); 
	int DCalcDot( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2, double* dstdot );
	int DVec3Cross( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2, D3DXVECTOR3* dstvec );

	int DVec3Normalize( D3DXVECTOR3* dstvec, D3DXVECTOR3 srcvec );

	int LimitQ( CQuaternion srcq, int hsid, int jointno, CQuaternion za3q, 
		D3DXVECTOR3 befeul, CQuaternion* limitq, D3DXVECTOR3* limiteul );


public:
	D3DXVECTOR3 m_tra;
	CQuaternion m_q;
	CQuaternion m_totalq;
	D3DXVECTOR3 m_cureul;//ZA3
	D3DXVECTOR3 m_befeul;//ZA3
	D3DXVECTOR3 m_neckbefeul;
	D3DXVECTOR3 m_finaltra;		//平滑処理後の位置
	CQuaternion m_finalq;		//平滑処理後のクォータニオン
	D3DXVECTOR3 m_finaleul;//ZA3

	CQuaternion m_orgq;//最初の計算結果(フィルターを掛けていない状態)
	D3DXVECTOR3 m_orgtra;
	D3DXVECTOR3 m_orgeul;

	int m_outputflag;

};

#endif