#ifndef MkmRotH
#define MkmRotH

#include <coef.h>
#include <d3dx9.h>

class CQElem;
class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CQuaternion;

class CMkmRot
{
public:
	CMkmRot();
	~CMkmRot();

	int LoadRot( MKMBUF* mkmbuf );

	int SetMotion( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, int motid, int curkey,
		CQuaternion* invq, CQuaternion* accumq, CQuaternion* accuminvq );

	int SetSerialNo( CTreeHandler2* lpth, CShdHandler* lpsh );

private:
	int InitParams();
	int DestroyObjs();

	int GetLine( MKMBUF* mkmbuf );
	int GetAttachName();
	int GetEndFrame();

	int GetQNum( MKMBUF* mkmbuf, int* numptr );
	int GetQElem( CQElem* dstq );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

public:
	int rotstart;//ì«Ç›çûÇ›pos
	int rotend;

	int serialno;
	char attach[256];
	int endframe;
	CQElem* qarray;
	int elemnum;

private:
	char m_linechar[MKMLINELENG];

};

#endif
