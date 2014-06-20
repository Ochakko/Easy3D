#ifndef PARTH
#define PARTH

//#include "charpos.h"
#include <coef.h>

#include <D3DX9.h>
#include <matrix2.h> // shade plugin sdk からmat4をもらいマチタ。(CMatrixとは掛ける方向が逆、あとで変える)
#include <crtdbg.h>

#include <basedat.h>

#include <boneinfo.h>
#include <infelem.h>

#include <quaternion.h>

class CD3DDisp;
class CShdHandler;
class CMotHandler;
class CShdElem;
//class CQuaternion;

class CPart : public CBaseDat
{

public:
	CPart();
	~CPart();

	virtual void	InitParams();

	void	ResetParams();
	int	CreateObjs();
	void	DestroyObjs();

	int InitPart( CMeshInfo* srcmeshinfo );

	int CreatePart( CMeshInfo* srcmeshinfo );
	int DestroyPart();

	int SetMaterial( CVec3f* srcvec );
	int SetDiffuse( CVec3f* srcvec );
	int SetSpecular( CVec3f* srcvec );
	int SetAmbient( CVec3f* srcvec );

	int SetJointLoc( CVec3f* srcvec );
	
	int AddBoneInfo( int srcjointno, int srcchilno, CVec3f endloc, int parno );
	int DestroyBoneInfo();

	int CopyData( CPart* srcdata );

	int	DumpMem( HANDLE hfile, int tabnum, int dumpflag );

	int CreateInfElemIfNot( int leng );

	int GetBoneMarkMatrix( int bno, CShdHandler* lpsh, CMotHandler* lpmh, int motid, int frameno, D3DXMATRIX matWorld, float baseleng, D3DXVECTOR3 basevec, D3DXMATRIX* bmmat );

	CShdElem* GetChildSelem( CShdHandler* lpsh, int boneno );

	int SetCurrentPose2OrgData( CMotHandler* srclpmh, CShdElem* selem, CQuaternion* multq );
	int ConvSymmXShape();

	int RestoreBoneInfo( CShdHandler* lpsh );

	int GetBoneInfo( int childno, CBoneInfo** dstppbi );

public:
	CMeshInfo* meshinfo;
	
	CVec3f	jointloc;
	int	bonenum;
	CBoneInfo** ppBI;

	CInfElem* m_IE;

	DVEC3 m_lim0;
	DVEC3 m_lim1;
	int m_ignorelim01;

	CQuaternion m_axisq;

	int m_za4_type;
	int m_za4_rotaxis;
	CQuaternion m_za4q;
	CQuaternion m_za4localq;

};

#endif