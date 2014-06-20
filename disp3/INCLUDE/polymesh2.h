#ifndef POLYMESH2H
#define POLYMESH2H


//�P�̒��_�����L����ʂ̐��̍ő吔
#define SHAREFACEMAX 100


//#include "charpos.h"
//#include <d3drmwin.h>
#include <coef.h>
#include <D3DX9.h>

#include <matrix2.h> // shade plugin sdk ����mat4�����炢�}�`�^�B(CMatrix�Ƃ͊|����������t�A���Ƃŕς���)
#include <crtdbg.h>

#include <basedat.h>
#include <infelem.h>

//class CMQOMaterial;
class CD3DDisp;
class CMotHandler;
class CShdHandler;
class CShdElem;
class CQuaternion;
class CToon1Params;
class CMQOMaterial;

typedef struct _tag_smface
{
	int faceno;
	int smoothflag;
} SMFACE;



class CPolyMesh2 : public CBaseDat
{
public:
	CPolyMesh2();
	~CPolyMesh2();

	//int CreatePointBuf( CMeshInfo* srcmeshinfo );
	//int CreateIndexBuf( int srcfacenum );

	int CreateBuffers( CMeshInfo* srcmeshinfo );

	int SetPointBuf( int srcpno, float srcx, float srcy, float srcz, int ono );
	int SetPointBuf( int srcpnum, VEC3F* srcvec );
	
	int SetIndexBuf( int srcfaceno, int* srclist );	
	int SetIndexBuf( int srcdatanum, int* srclist, int srcflag );
	int InvIndexBuf( int srcfaceno );
	int SetDefaultIndex( int srcxnum, int srcznum );

	//int SetMaterial( int faceno, float srcdiffuse, float srcambient, float srcspecular, float srcalpha, float srcr, float srcg, float srcb, COORDINATE* srcuv, int srcvnum );
	int SetMaterial( int faceno, float srcdiffuse, float srcambient, float srcspecular, float power, float emi, ARGBF color, ARGBF scene_ambient, COORDINATE* srcuv, int srcvnum );
	int SetVCol( int vertno, __int64 vcol );
	int GetVCol2( int srcoldvno, __int64* dstcol, int* dstsetflag );

	int SetDefaultMaterial();

	int SetColor( int srcpno, ARGBF* srccol );
	int SetSamePoint();

	//int SetGroundFlag( int srcflag );

	int MItoMaterial();

	
	int CreateOptDataBySamePointBuf( int adjustuvflag, float srcfacet );
	int CreateOptDataBySamePointBufLegacy( int adjustuvflag, float srcfacet );
	int CreateOptDataByOld2Opt( int adjustuvflag, float srcfacet, int calcsmooth );
	int DestroyOptData();
	int DestroyVColData();

	int GetOptPos( int vertno, D3DXVECTOR3* vpos );
	int SetOptPos( int vertno, D3DXVECTOR3 vpos );

	int MakeFaceno2Materialno( ARGBF* sceneamb, int srcshader, int sigmagicno, int** arrayptr, CMQOMaterial* newmathead, CMQOMaterial* shmathead, int srcseri );

	int WriteMQOObject( HANDLE hfile, int* matnoindex, char* srctexname, CShdElem* selem );
	int WriteMQOObjectOnFrame( HANDLE hfile, int* matnoindex, char* srctexname, int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag = 1 );
	int WriteMQOObjectOnFrameMorph( HANDLE hfile, int* matnoindex, char* srctexname, 
		int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag = 1 );

	//int SetOptDiffuse( int srcr, int srcg, int srcb, int setflag, CD3DDisp* d3dptr, int tlmode );
	//int SetOptSpecular( int srcr, int srcg, int srcb, int setflag, CD3DDisp* d3dptr, int tlmode );
	//int SetOptAmbient( int srcr, int srcg, int srcb, int setflag );
	
	int SetDiffuse( int optvertno, int setflag, ARGBF srccol, CD3DDisp* d3dptr, int tlmode );
	int SetAmbient( int optvertno, int setflag, ARGBF srccol );
	int SetSpecular( int optvertno, int setflag, ARGBF srccol, CD3DDisp* d3dptr, int tlmode );
	int SetEmissive( int optvertno, int setflag, ARGBF srccol, CD3DDisp* d3dptr, int tlmode );
	int SetSpecularPower( int optvertno, int setflag, float srcpow, CD3DDisp* d3dptr, int tlmode );

	int GetDiffuse( int optvertno, ARGBF* dstcol );
	int GetAmbient( int optvertno, ARGBF* dstcol );
	int GetSpecular( int optvertno, ARGBF* dstcol );
	int GetEmissive( int optvertno, ARGBF* dstcol );
	int GetSpecularPower( int optvertno, float* powerptr );

	int SetMaterialDiffuse( int matno, int setflag, ARGBF srcdiffuse );
	int SetMaterialSpecular( int matno, int setflag, ARGBF srcspecular );
	int SetMaterialAmbient( int matno, int setflag, ARGBF srcambient );
	int SetMaterialEmissive( int matno, int setflag, ARGBF srcemissive );
	int SetMaterialPower( int matno, int setflag, float srcpow );

	int GetMaterialDiffuse( int matno, ARGBF* dstdiffuse );
	int GetMaterialSpecular( int matno, ARGBF* dstspecular );
	int GetMaterialAmbient( int matno, ARGBF* dstambient );
	int GetMaterialEmissive( int matno, ARGBF* dstemissive );
	int GetMaterialPower( int matno, float* dstpower );



	int CreateInfElemIfNot( int leng );
	int GetOrgVertNo( int vertno, int* orgnoarray, int arrayleng, int* getnumptr );

	int SetCurrentPose2OrgData( CMotHandler* srclpmh, CShdElem* selem, CQuaternion* multq );
	int ConvSymmXShape();

	int CalcAnchorCenter();

	int CheckVertInShape( D3DXVECTOR3 srcv, int clockwise, int* insideptr );

	int SetUV( int vertno, float fu, float fv, int setflag, int clampflag = 0 );
	int SetUVTile( int texrule, int unum, int vnum, int tileno );

	int MultMat( D3DXMATRIX multmat );

	int ChangeColor( int seri, CPolyMesh2* srcpm2 );
	int CalcInitialInfElem( int srcmatno, int srcchildno );

	int CreateAndSaveTempMaterial();
	int RestoreAndDestroyTempMaterial();
	int SetMaterialFromInfElem( int srcchildno, int srcvert );
	int GetNearVert( D3DXVECTOR3 findpos, float symdist, int* findvert, float* finddist );
	int GetValidVColNum();

//	int ConvColor2XMaterial( float srcalpha, char* srctexname, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr );
	int ConvColor2XMaterial( CMQOMaterial* mqomat, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr );
	int CreateToon1Buffer(  LPDIRECT3DDEVICE9 pdev, D3DXMATERIAL* pmat, DWORD* pattrib, int matnum, CMQOMaterial* mqomat );
	int CreateToon1Buffer2(  LPDIRECT3DDEVICE9 pdev, CMQOMaterial* mqomat );
	int CreateSortTexBuffer( CMQOMaterial* mathead );

	int GetInfluenceVertNo( int srcjointno, int* vertarray, int* vertnum );

	int CopyNearestInfElem( CPolyMesh2* srcpm2, SERICONV* sericonv, int serinum );

	int SetAttrib0( int faceno, int srcmaterialno );
	int ChkAlphaNum( CMQOMaterial* mathead );
	CMQOMaterial* GetMaterialFromNo( CMQOMaterial* mathead, int matno );
	int* GetDirtyMaterial( int matno );
	int SetInfCol( E3DCOLOR3UC* srcinfcol );
	int CloneDispObj( CPolyMesh2* srcpm2, float srcfacet );

private:
	void InitParams();
	void DestroyObjs();
	void DestroyPolyMesh2();
	int	DestroyTempMaterial();
	int DestroyToon1Buffer();
	int DestroySortTexBuffer();

	int SetOptData( int setflag, int* lengptr, int* srcsamepoint, float facet );
	int SetOptDataLegacy( int setflag, int* lengptr, int* srcsamepoint, float facet );
	int SetOptDataByOld2Opt( int* lengptr, int* chkflag );
	int AdjustUVBuf( int aduv );

	int WriteChar( HANDLE hfile, char* writechar, int addreturn );

	int SetUV_X( float startu, float endu, float startv, float endv );
	int SetUV_Y( float startu, float endu, float startv, float endv );
	int SetUV_Z( float startu, float endu, float startv, float endv );
	int SetUV_Cylinder( float startu, float endu, float startv, float endv );
	int SetUV_Sphere( float startu, float endu, float startv, float endv );

	int CalcOrgNormal();
	int CalcNormal( VEC3F* newn, VEC3F* curp, VEC3F* aftp1, VEC3F* aftp2 );
	int Vec3Cross( VEC3F* pOut, VEC3F* pV1, VEC3F* pV2 );
	int Vec3Normalize( VEC3F* retvec, VEC3F* srcvec );

	int SetSmoothBuf( float facet );
	int SetOptSamePointBuf();

	__int64 GetVCol( int vertno );
	VCOLDATA* GetFreeVColData();

	int IsSameMaterial( D3DMATERIAL9* mat1, D3DMATERIAL9* mat2 );
	int IsSameXMaterial( D3DXMATERIAL* xmat1, D3DMATERIAL9* mat2, char* texname2 );
	int IsSameD3DColorValue( D3DCOLORVALUE* col1, D3DCOLORVALUE* col2 );

	int SetSMFace( float srcfacet );
	int SearchSamePointBuf( int srcvno );

	int MakeColor( float minval, float maxval, float val, E3DCOLOR3UC target_col, E3DCOLOR3UC* dstcol );


public:
	int m_loadorgnormalflag;//CalcOrgNormal���X�L�b�v���邽�߂̃t���O�isig�ǂݍ��ݎ��ɃZ�b�g�j

	CMeshInfo* meshinfo; // m�͒��_���An�͖ʐ��Atotal�͎g��Ȃ�0<---- material���́A�g�p���Ȃ��B
	VEC3F* pointbuf;// leng : meshinfo->n * 3
	//int* indexbuf;// leng : meshinfo->n �~3

	VEC3F* orgnormal;// leng : meshinfo->n * 3


	//-1�ŏ������B
	//��O�̓������W�̔ԍ���ێ�����B
	//�������W�����A�擪��samepointbuf�́A-2
	int* samepointbuf;

	//samepoint���A�X���[�W���O���钸�_�̔ԍ����Z�b�g�B
	int* smoothbuf;// leng : meshinfo->n * 3


	//���_�����L����ʂ̔ԍ��̃��X�g
	SMFACE** smface;// leng : meshinfo->n * 3


	COORDINATE*	uvbuf; // leng : meshinfo->n * 3
	ARGBF* diffusebuf; // leng : meshinfo->n
	ARGBF* ambientbuf; // leng : meshinfo->n
	float* specularbuf; // leng : meshinfo->n

	float* powerbuf; // leng : meshinfo->n
	float* emissivebuf; // leng : meshinfo->n
	float* diffbuf; // leng : meshinfo->n


	ARGBF* colorbuf; //leng : meshinfo->n * 3, ���^�Z�R�A���_�J���[

	char* dirtypoint; // leng : meshinfo->n * 3, save, load�͂��Ȃ��B//�Œ���ł͂Ȃ��A�v�Z���ɕK�v�ɉ����āA����Ɏg�p�\�B


	int* orgnobuf;// 2004/07/14  leng : meshinfo->n * 3

///////
	int optflag;
	int optpleng;
	D3DTLVERTEX* opttlv;//leng : optpleng
	ARGBF* optambient;//leng : optpleng
	ARGBF* optcolorbuf;//leng : optpleng
	
	float* optpowerbuf;//leng : optpleng
	ARGBF* optemissivebuf;//leng : optpleng

	DWORD* tempdiffuse;
	DWORD* tempspecular;
	ARGBF* tempambient;
	ARGBF* tempemissive;
	float* temppower;


	int* oldpno2optpno;//leng : meshinfo->n * 3
	int* optindexbuf;// leng : meshinfo->n * 3

	int* optpno2oldpno;//leng : optleng	//�������W�����A���W�̂����ŁA��ԑ傫��oldpno���Z�b�g����B

	int* optsamepointbuf;// leng : optpleng : samepointbuf��opt�o�[�W����


	int groundflag;

	int adjustuvflag;

	//int groundflag;

	/***
	int optflag;
	int	optpleng;
	VEC3F* optpointbuf; //�d�����_���Ȃ������o�b�t�@�Bleng : meshinfo->n * 3 : �g�p�͈� optpleng
	int* optindexbuf; // leng : meshinfo->n * 3
	int* optpno2oldface; //leng : meshinfo->n * 3 , �g�p�͈́@optpleng 

	int* oldpno2optpno;//leng : meshinfo->n * 3
	int* optpno2oldpno;//leng : meshinfo->n * 3

	//int* optsamepointbuf; //leng : optpleng ���W�̂ݓ���optpoint�̏��B-1�������A��O�̓������W�̔ԍ���ێ��A�������W�����擪��optsamepointbuf�́A-2
	***/

	CInfElem* m_IE;

	int createoptflag;

	D3DXVECTOR3 m_center;

	int m_anchormaterialno;
	CShdElem* m_mikoapplychild;
	CShdElem* m_mikotarget;

	int tempapplychild;
	int temptarget;

	VCOLDATA** vcoldata;
	int vcolnum;


	D3DXMATERIAL* m_material;
	int m_materialnum;
	DWORD* m_attrib;// meshinfo->n
	int* m_optindexbuf2;// meshinfo->n * 3, material�ŕ��ёւ����C���f�b�N�X
	int* m_optindexbuf2m;//morph�p��optindexbuf2
	MATERIALBLOCK* m_materialblock;// m_materialnum
//	LPDIRECT3DTEXTURE9* m_toon1tex;
	CToon1Params* m_toon1;
	int* m_toonface2oldface;

	int* m_attrib0;//meshinfo->n mqo�ǂݍ��ݎ���sh->m_mathead�̃}�e���A���ԍ���ʂ��Ƃɕێ�����B

	SORTTEXTURE* m_sorttex;// meshinfo->n �ʂ��Ƃ̃e�N�X�`�����A�e�N�X�`�����Ń\�[�g����
	int* m_optindexbuftex;// meshinfo->n * 3, sorttex�̏��Ԃŕ��ёւ����C���f�b�N�X
	int* m_optindexbuftexm;//morph�p��m_optindexbuftex
	int m_texblocknum;
	TEXTUREBLOCK* m_texblock;// texblocknum�̃����_�[�P��

	CHKALPHA chkalpha;

	DIRTYMAT* m_dirtymat;

	E3DCOLOR3UC infcol[CALCMODE_MAX];

};



#endif