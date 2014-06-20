#ifndef BVHELEMH
#define BVHELEMH

#include <coef.h>
#include <D3DX9.h>

class CQuaternion;

class CBVHElem
{
public:
	CBVHElem();
	~CBVHElem();

	int SetName( char* srcname );
	int SetIsRoot( int srcisroot );
	int SetOffset( char* srcchar );
	int SetChanels( char* srcchar );

	int CreateMotionObjs( int srcframes );

	int SetMotionParams( int srcframeno, float* srcfloat );

	int DbgOutBVHElem( int srcdepth, int outmotionflag );
	int SetPosition();

	int Mult( float srcmult );
	int ConvertRotate2Q();
	int CheckNotAlNumName( char** ppdstname );


private:
	int InitParams();
	int DestroyObjs();

	char* GetDigit( char* srcstr, int* dstint, int* dstsetflag );
	char* GetFloat( char* srcstr, float* dstfloat, int* dstsetflag );
	char* GetChanelType( char* srcstr, int* dstint, int* dstsetflag );

public:
	int isroot;

	int describeno;
	int serialno;
	int mqono;

	char name[ PATH_LENG ];
	D3DXVECTOR3 offset;
	int chanelnum;
	int chanels[ CHANEL_MAX ];

	int framenum;
	float frametime;

	D3DXVECTOR3* trans;
	D3DXVECTOR3* rotate;
	CQuaternion* qptr;
	CQuaternion* transpose;//転置後のクォータニオン
	CQuaternion* treeq;//親の影響を考慮したクォータニオン

	D3DXVECTOR3* partransptr;

	D3DXVECTOR3 position;

	CBVHElem* parent;
	CBVHElem* child;
	CBVHElem* brother;

	int samenameboneseri;//sigに、同じ名前のボーンがあった場合にselem->serialnoをセット、無かったら-1。

	int rotordercnt;
	int rotorder[ROTAXIS_MAX];
};

#endif