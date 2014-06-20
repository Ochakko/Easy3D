#ifndef MStateH
#define MStateH

#include <coef.h>

#include <TexKeyPt.h>
#include <MMotKey.h>
#include <AlpKey.h>
#include <matrix2.h>

//class CMatrix2;
class CShdHandler;
class CMotHandler;

enum {
	MSLOCAL_ANIM,
	MSLOCAL_FRAME,
	MSLOCAL_NEXTANIM,
	MSLOCAL_NEXTFRAME,
	MSLOCAL_MAX
};

typedef struct tag_mselem
{
	CMatrix2 bonemat;
	int localinf[ MSLOCAL_MAX ];
}MSELEM;


class CMState
{
public:
	CMState();
	~CMState();

	int CreateMatrix( CShdHandler* lpsh, int bonenum );
	int SetMSElem( int boneno, CMatrix2* srcmat );
	int SetMSElem( CShdHandler* lpsh );
	int SetMSLocalInf( int boneno, int srcanimno, int srcframeno, int srcnextanimno, int srcnextframeno );

	int InitMatrix();

private:
	int InitParams();
	int DestroyObjs();

public:
	int m_bonenum;
	int m_keyflag;

	int m_ds[DISPSWITCHNUM];
	CTexKeyPt m_tk;
	CMMotKey m_mk;
	CAlpKey m_ak;

	MSELEM* m_mselem;

//	CMatrix2* m_bonemat;	
//	int* m_plocalanimno;
//	int* m_plocalframeno;
//	int* m_plocalnextanimno;
//	int* m_plocalnextframeno;
};

#endif