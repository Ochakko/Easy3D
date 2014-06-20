#ifndef MORPHH
#define MORPHH

#include <coef.h>
#include <D3DX9.h>

#include <crtdbg.h>

#include <basedat.h>

class CShdHandler;
class CShdElem;
class CMotHandler;
class CMotionCtrl;

class CMorph
{
public:
	CMorph();
	~CMorph();

	int SetBaseParams( CShdHandler* lpsh, CMotHandler* lpmh, CShdElem* srcbaseelem, CMotionCtrl* srcboneelem );
	int SetBaseParams( CShdHandler* lpsh, CMotHandler* lpmh, int srcbaseseri, int srcboneseri );

	void DestroyObjs();
	int AddMorphTarget( CShdElem* addelem );
	int AddMorphTarget( int addseri );
	int DeleteMorphTarget( CShdElem* delelem );
	int DeleteAllTarget();

	int CheckTargetOK( CShdElem* chkelem );

	int ConvSeriToElem();//sigì«Ç›çûÇ›óp

private:
	int InitParams();
	int FindTarget( CShdElem* findelem );
	int FindTarget( int findseri );

public:
	int m_baseseri;
	CShdElem* m_baseelem;
	int m_boneseri;
	CMotionCtrl* m_boneelem;
	int m_objtype;
	int m_vertnum;
	int m_facenum;


	int m_targetnum;
	//CShdElem** m_pptarget;
	TARGETPRIM* m_ptarget;

private:
	CShdHandler* m_shandler;
	CMotHandler* m_mhandler;
};

#endif