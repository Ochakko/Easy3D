#ifndef LWOLAYRH
#define LWOLAYRH


#include <D3DX9.h>

#define DBGH
#include <dbg.h>

#include <crtdbg.h>

class CLWOvmap;
class CLWOvmad;
class CLWOpols;
class CLWOptag;

class CLWOlayr
{
public:
	CLWOlayr();
	~CLWOlayr();

	int SetLayerParams( unsigned int srcnumber, unsigned int srcflag, D3DXVECTOR3 srcpivot, char* srcname, unsigned int srcparent );

	int CreatePNTS( int srcnum );
	int SetPNTS( int srcno, D3DXVECTOR3 srcvec );

	int SetBBOX( D3DXVECTOR3 srcmin, D3DXVECTOR3 srcmax );

	CLWOvmap* AddVmap();
	CLWOvmad* AddVmad();
	CLWOpols* AddPols();
	CLWOptag* AddPtag();

private:
	int InitParams();
	int DestroyObjs();


public:
	unsigned int number;
	char* name;
	unsigned int parent;
	unsigned int flag;
	D3DXVECTOR3 pivot;

	int pntsnum;
	D3DXVECTOR3* PNTS;

	D3DXVECTOR3 BBOXmin;
	D3DXVECTOR3 BBOXmax;

	int vmapnum;
	CLWOvmap** ppvmap;
	
	int vmadnum;
	CLWOvmad** ppvmad;

	int polsnum;
	CLWOpols** pppols;

	int ptagnum;
	CLWOptag** ppptag;


	//CLWOlayr* next;

};

#endif