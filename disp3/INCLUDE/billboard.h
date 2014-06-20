#ifndef BILLBOARDH
#define BILLBOARDH


#include <coef.h>
#include <D3DX9.h>

#include <crtdbg.h>
#include <basedat.h>

class CShdHandler;
class CShdElem;
class CBillboardElem;
class CBSphere;

class CBillboard : public CBaseDat
{
public:
	CBillboard();
	~CBillboard();

	int CreateBuffers( CMeshInfo* srcmeshinfo );
	
	int SetSElemPtr( CShdElem* srcselem, float srcR );

	int SortElem( D3DXVECTOR3 vDir );
	int SetBillboardPos( int bbid, float posx, float posy, float posz );
	int RotateBillboard( int bbid, float fdeg, int rotkind );
	int SetBillboardDispFlag( int bbid, int flag );
	int SetBillboardSize( int bbid, float srcw, float srch, int dirmode, int orgflag );
	int DestroyBillboard( int bbid, CTreeHandler2* lpth, CMotHandler* lpmh );

	int SetBillboardUV( int bbid, int unum, int vnum, int texno, int revuflag );

	int SetParticlePos( D3DXVECTOR3 srcpos );
	int SetParticleGravity( float srcgravity );
	int SetParticleLife( float srclife );
	int SetParticleEmitNum( float srcemitnum );
	int SetParticleVel0( D3DXVECTOR3 srcminvel, D3DXVECTOR3 srcmaxvel );

	int UpdateParticle( CShdHandler* lpsh, int srcfps );

	int SetParticleAlpha( CShdHandler* lpsh, float srcmintime, float srcmaxtime, float srcalpha );
	int SetParticleUVTile( CShdHandler* lpsh, float srcmintime, float srcmaxtime, int srcunum, int srcvnum, int srctileno );

	void InitParticleParams0();
	int InitParticle();

	int ChkConfParticle( CBSphere* chkbs, float srcrate );

private:
	void InitParams();
	void DestroyObjs();

	int FindUnusedElem();
	CBillboardElem* FindBillboardByID( int bbid );	
	CBillboardElem* GetNotUseElem();

public:
	int dispnum;
	CMeshInfo* meshinfo;
	CBillboardElem* bbarray;
	CBillboardElem** disparray;
	
	static D3DXVECTOR3 s_campos;

///////////
	// for particle
	float emitnum;
	float femittotal;
	int	iemittotal;

	D3DXVECTOR3 particlepos;

	float gravity;

	float life;//[sec]
	float time;//[sec]

	D3DXVECTOR3 minvel;
	D3DXVECTOR3 maxvel;

	float time0alpha;
	int time0uv_unum;
	int time0uv_vnum;
	int time0uvtile;

	int cmpalways;
};


#endif