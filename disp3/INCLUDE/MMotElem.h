#ifndef MMOTELEMH
#define MMOTELEMH

#include <coef.h>

class CShdElem;

class CMMotElem
{
public:
	CMMotElem( CShdElem* srcbaseelem );
	~CMMotElem();

	int AddPrim( CShdElem* addelem );
	int DeletePrim( CShdElem* delelem );
	int FindPrim( CShdElem* findelem );
	MPRIM* GetPrim( CShdElem* findelem );

	float GetValue( CShdElem* selem );
	int SetValue( CShdElem* selem, float srcvalue );

	int InitParams();
	int DestroyObjs();

	int CopyTarget( CMMotElem* srcmme );

public:
	CShdElem* m_baseelem;
	int m_primnum;
	MPRIM* m_primarray;
};

#endif