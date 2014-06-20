#ifndef EXTLINEIOH
#define EXTLINEIOH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <basedat.h>
#include <D3DX9.h>


#define DBGH
#include <dbg.h>


class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;

class CExtLineIO
{
public:
	CExtLineIO();
	~CExtLineIO();

	int CreateLine( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, D3DXVECTOR3* pptr, int pointnum, int maxpointnum, int linekind );
	int SetMeshInfo( CMeshInfo* dstmi, int mitype, int mim, int min, int linekind );

private:
	void InitLoadParams();
	int AddShape2Tree( char* elemname );
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

};


#endif