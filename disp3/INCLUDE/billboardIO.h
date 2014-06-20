#ifndef BILLBOARDIOH
#define BILLBOARDIOH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>


#define DBGH
#include <dbg.h>


class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;



class CBillboardIO
{
public:
	CBillboardIO();
	~CBillboardIO();

	int CreateBillboard( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );
	int AddBillboard( char* bmpname, float width, float height, int transparentflag, int dirmode, int orgflag, int* retseri );


private:
	void InitLoadParams();
	int AddShape2Tree( char* elemname, char* texname, int transparentflag );
	int SetMeshInfo( CMeshInfo* dstmi, int mitype, int mim, int min );
	int Init3DObj();

public:
	CTreeHandler2* m_lpth;
	CShdHandler* m_lpsh;
	CMotHandler* m_lpmh;


	CMeshInfo tempinfo;
	int curseri;
	int befseri;
	int curdepth;
	int befdepth;
	int curshdtype;
	int befshdtype;

	int m_pmnum;
};

#endif