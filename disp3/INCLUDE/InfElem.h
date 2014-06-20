#ifndef INFELEMH
#define INFELEMH

class CShdHandler;
class CBoneInfo;
class CShdElem;

class CInfElem
{
public:
	CInfElem();
	~CInfElem();

	int ClearMikoParams();
	int AddMikoApChild( int srcchildseri );
	int SetNearestBI( CShdHandler* srclpsh, D3DXVECTOR3 srcv );

	int SetInfElemDefault( CBoneInfo* srcbi, CShdElem* parselem, D3DXVECTOR3* srcxv, int mikoflag );
	int SetInfElemDefaultNoSkin( int srcmatno, int srcchildno );
	int SetInfElemInitial();

	int AddInfElem( INFELEM srcie );
	int SetInfElem( CBoneInfo* srcbi, CShdElem* parselem, D3DXVECTOR3 srcxv, int paintmode, int calcmode, float rate, int normflag, float directval );
	int DeleteInfElem( int srcchildno, int normflag );

	int DestroyIE();
	int Normalize();

	int CheckInfluence( int srcjointno, float srccmpval );

	int CopyIE( CInfElem* srcie, SERICONV* sericonv, int serinum );

	int InitParams();

	int GetInfElemByBone( int srcboneno );


private:
	int DestroyObjs();
	int Replace2FloatBone( CShdHandler* srclpsh );

	int InitIE( INFELEM* dstie );

	INFELEM* ExistINFELEM( int srcchildno );

	int CalcOneSkin0( CBoneInfo* srcbi, CShdElem* parselem, int mikoflag, D3DXVECTOR3* srcxv, float* rateptr );
	int CalcOneSkin1( CBoneInfo* srcbi, D3DXVECTOR3* srcxv, float* rateptr );

	int DeleteInfElemByIndex( int delindex );
	int DeleteDummyINFELEM();

public:
	int infnum;
	int normalizeflag;
	int symaxis;
	float symdist;
	INFELEM* ie;

	//////// for miko
	int mikoapchildnum;
	int* mikoapchildseri;
	CBoneInfo* nearestbi;
	CShdElem* nearestbipar;

};

/***
typedef struct tag_infelem
{
	int childno;//�e���{�[���A�q���̔ԍ��B�G���h�W���C���g������B
	int bonematno;//�}�g���b�N�X�i�[�{�[���̔ԍ��A�e�̔ԍ��Bbonematno, parmatno�B�q�������W���C���g�̔ԍ���������Ȃ��B
	int kind;//CALC_*
	float userrate;//��
	float orginf;//CALC_*�Ōv�Z�����l�B
	float dispinf;//�@orginf[] * userrate[]�Anormalizeflag���P�̂Ƃ��́A���K������B
}INFELEM;
***/

#endif