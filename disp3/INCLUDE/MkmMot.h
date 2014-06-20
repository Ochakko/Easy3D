#ifndef MkmMotH
#define MkmMotH

#include <coef.h>
#include <d3dx9.h>

#include <quaternion.h>

class CMkmRot;
class CMkmTra;
class CTreeHandler2;
class CShdHandler;
class CMotHandler;


class CMkmMot
{
public:
	CMkmMot();
	~CMkmMot();

	int LoadMotion( MKMBUF* mkmbuf );
	int SetKeyFrame();

	int SetMotion( CMkmMot* inimot, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, float tramult, LONG* motidptr );

	int GetInitInvQ( CTreeHandler2* lpth, CShdHandler* lpsh, int rotno, CQuaternion* invq, CQuaternion* accumq, CQuaternion* accuminvq );
	int FindRot( int srcseri, CMkmRot** pprot );

private:
	int InitParams();
	int DestroyObjs();

	int GetLine( MKMBUF* mkmbuf );
	int GetMotionName();
	int GetEndFrame();

	int FindRotation( MKMBUF* mkmbuf, int setflag, int* numptr );
	int FindTrans( MKMBUF* mkmbuf, int setflag, int* numptr );

	int CreateMotionPoints( CShdHandler* lpsh, CMotHandler* lpmh, int motcookie, int frameno );

public:
	int motstart;//ì«Ç›çûÇ›pos
	int motend;

	char motname[256];
	int endframe;

	int frameleng;
	int keynum;
	int* keyframe;

	int rotnum;
	CMkmRot* mkmrot;

	int tranum;
	CMkmTra* mkmtra;
private:
	char m_linechar[ MKMLINELENG ];


};

#endif
