#ifndef CPUIDH
#define CPUIDH


typedef union vender {
	char id[12+1];
	struct {
		DWORD id0;
		DWORD id1;
		DWORD id2;
		char nul;
	};
} vender;


class CCpuid
{

public:
	CCpuid();
	~CCpuid();

public:
	BOOL OSCheckCPUID();
	BOOL OSCheckMMX();
	BOOL OSCheckSSE();
	BOOL OSCheckSSE2();
	BOOL OSCheck3DNow();
	BOOL OSCheckE3DNow();
	int CheckCPU();

public:
	BOOL bCPUID, bMMX, bSSE, bSSE2, b3DNow, bE3DNow;
	DWORD dwStandard, dwFeature;
	DWORD dwFamily, dwModel, dwSteppingId;
	vender vd;

};


#endif