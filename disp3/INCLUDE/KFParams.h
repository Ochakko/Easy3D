#ifndef KFPARAMSH
#define KFPARAMSH

#include <coef.h>


class CKFParams
{
public:
	CKFParams();
	~CKFParams();

	int SetIParam( int srcindex, int srcival );
	int SetDParam( int srcindex, double srcdval );

private:
	int InitParams();
	int DestroyObjs();


public:
	int m_frameno;
	int m_iparams[ KFPINUM ];
	double m_dparams[ KFPDNUM ];

//	CKFParams* prev;
//	CKFParams* next;

private:




};

#endif