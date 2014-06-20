#ifndef INFSCOPEH
#define INFSCOPEH

class CShdElem;
class CPolyMesh;
class CPolyMesh2;
class CD3DDisp;


class CInfScope
{
public:
	CInfScope();
	~CInfScope();

	int InvalidateDispObj();
	int ResetPrimNum();
	int DestroyObjs();
	int CalcInitialInfElem();

	int SetPolyMesh2( CPolyMesh2* pm2, CD3DDisp* srcd3ddisp, float srcfacet );
	int SetPolyMesh( CPolyMesh* pm, CD3DDisp* srcd3ddisp );

	int CheckVertInScope( D3DXVECTOR3 srcv, int clockwise, int* insideptr );

	int GetPointNum( int* pointnumptr );
	int GetPointBuf( D3DXVECTOR3* pbuf );
	int GetCenter( D3DXVECTOR3* centerptr );
	int SetPointBuf( D3DXVECTOR3* newp, int vertno );

private:
	int InitParams();

public:
	int type;//SHDPOLYMESH or SHDPOLYMESH2
	int dispflag;
	CShdElem* applychild;
	CShdElem* target;
	int anchormaterialno;// for miko
	CPolyMesh2* polymesh2;
	CPolyMesh* polymesh;
	CD3DDisp* d3ddisp;
	float pm2facet;

};

#endif
