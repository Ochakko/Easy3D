#ifndef MkmTraH
#define MkmTraH

#include <coef.h>
#include <d3dx9.h>

class CTraElem;
class CTreeHandler2;
class CShdHandler;
class CMotHandler;

class CMkmTra
{
public:
	CMkmTra();
	~CMkmTra();

	int LoadTra( MKMBUF* mkmbuf );
	int SetMotion( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, int motid, int curkey, float tramult );
	int SetSerialNo( CTreeHandler2* lpth, CShdHandler* lpsh );

private:
	int InitParams();
	int DestroyObjs();

	int GetLine( MKMBUF* mkmbuf );
	int GetAttachName();
	int GetEndFrame();

	int GetTraNum( MKMBUF* mkmbuf, int* numptr );
	int GetTraElem( CTraElem* dsttra );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );


public:
	int trastart;//ì«Ç›çûÇ›pos
	int traend;

	int serialno;
	char attach[256];
	int endframe;
	CTraElem* traarray;
	int elemnum;
private:
	char m_linechar[MKMLINELENG];

};

#endif
