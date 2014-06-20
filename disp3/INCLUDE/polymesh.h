#ifndef POLYMESHH
#define POLYMESHH

#include <coef.h>
#include <D3DX9.h>

#include <matrix2.h> // shade plugin sdk からmat4をもらいマチタ。(CMatrixとは掛ける方向が逆、あとで変える)
#include <crtdbg.h>

#include <basedat.h>
#include <infelem.h>


class CD3DDisp;
class CMQOMaterial;
class CMotHandler;
class CShdHandler;
class CShdElem;
class CQuaternion;
class CToon1Params;


class CPolyMesh : public CBaseDat
{
public:
	CPolyMesh();
	~CPolyMesh();

	int CreatePointBuf( CMeshInfo* srcmeshinfo, float srcalpha );
	int CreateIndexBuf( int srcfacenum );
	int SetPointBuf( int srcpno, float srcx, float srcy, float srcz, int orgno );
	int SetPointBuf( int srcpnum, VEC3F* srcvec );

	int SetIndexBuf( int srcfaceno, int* srclist );
	int SetIndexBuf( int srcdatanum, int* srclist, int srcflag );
	int InvIndexBuf( int srcfaceno );
	int SetDefaultIndex( int srcxnum, int srcznum );//四角の格子状のデータと仮定して、indexをセットする。
	int SetInvisibleWallIndex( int pmfnum );


	int SetGroundFlag( int srcflag );
	int CreateTextureBuffer();
	int SetTextureKind( int srcpno, int srckind );
	int SetTex4UV( int srcxnum, int srcznum );
	int SetUV( int vertno, float fu, float fv, int setflag, int clampflag = 0 );
	int SetUVTile( int texrule, int unum, int vnum, int tileno );
	int CopyUVFromD3DDisp( CD3DDisp* d3ddispptr );
	int AdjustUVBuf();


	int GetPointBuf( int srcpno, D3DXVECTOR3* dstvec );


	int SetBillboardPoints( float width, float height, int orgflag );
	int SetBillboardUV( int unum, int vnum, int texno, int revuflag );
	

	int MakeFaceno2Materialno( int srcshader, int sigmagicno, int** arrayptr, CMQOMaterial* newmathead, CMQOMaterial* shmathead, int srcseri );
	int WriteMQOObject( HANDLE hfile, int* matnoindex, CShdElem* selem );
	int WriteMQOObjectOnFrame( HANDLE hfile, int* matnoindex, int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag = 1 );
	int WriteMQOObjectOnFrameMorph( HANDLE hfile, int* matnoindex, 
		int motid, int frameno, int serialno, CShdHandler* lpsh, CMotHandler* lpmh, float srcmult, int writeRevFlag = 1 );
	int CreateInfElemIfNot( int leng );

	int SetDiffuse( int srcvertno, int setflag, ARGBF srcdiffuse, CD3DDisp* d3dptr, int tlmode );
	int SetAmbient( int srcvertno, int setflag, ARGBF srcambient );
	int SetSpecular( int srcvertno, int setflag, ARGBF srcspecular, CD3DDisp* d3dptr, int tlmode );
	int SetEmissive( int srcvertno, int setflag, ARGBF srcspecular, CD3DDisp* d3dptr, int tlmode );
	int SetSpecularPower( int srcvertno, int setflag, float srcpow, CD3DDisp* d3dptr, int tlmode );

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


	int GetDiffuse( int srcvertno, ARGBF* dstdiffuse );
	int GetAmbient( int srcvertno , ARGBF* dstambient );
	int GetSpecular( int srcvertno, ARGBF* dstspecular );
	int GetEmissive( int srcvertno, ARGBF* dstemissive );
	int GetSpecularPower( int srcvertno, float* powerptr );

	int GetOrgVertNo( int vertno, int* orgnoarray, int arrayleng, int* getnumptr );

	int SetCurrentPose2OrgData( CMotHandler* srcmh, CShdElem* selem, CQuaternion* multq );
	int ConvSymmXShape();

	int ChangeColor( CPolyMesh* srcpm );

	int CalcInitialInfElem( int srcmatno, int srcchildno );
	int CheckVertInShape( D3DXVECTOR3 srcv, int clockwise, int* insideptr );
	int CalcAnchorCenter();

	int CreateAndSaveTempMaterial();
	int RestoreAndDestroyTempMaterial();
	int SetMaterialFromInfElem( int srcchildno, int srcvert );

	int GetNearVert( D3DXVECTOR3 findpos, float symdist, int* findvert, float* finddist );

//	int ConvColor2XMaterial( float srcalpha, char* srctexname, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr );
	int ConvColor2XMaterial( CMQOMaterial* mqomat, D3DXMATERIAL** ppmat, DWORD** ppattr, int* matnumptr );
	int CreateToon1Buffer( LPDIRECT3DDEVICE9 pdev, D3DXMATERIAL* pmat, DWORD* pattrib, int matnum, CMQOMaterial* mqomat );
	int CreateToon1Buffer2( LPDIRECT3DDEVICE9 pdev, CMQOMaterial* mqomat );

	int GetInfluenceVertNo( int srcjointno, int* vertarray, int* vertnum );

	int CopyNearestInfElem( CPolyMesh* srcpm, SERICONV* sericonv, int serinum );


	int CreateSortTexBuffer( CMQOMaterial* mathead );
	int ChkAlphaNum( CMQOMaterial* mathead );

	int* GetDirtyMaterial( int matno );
	CMQOMaterial* GetMaterialFromNo( CMQOMaterial* mathead, int matno );
	int SetInfCol( E3DCOLOR3UC* srcinfcol );
	int CloneDispObj( CPolyMesh* srcpm, float srcalpha );

private:
	void InitParams();
	void DestroyObjs();
	void DestroyPolyMesh();
	int DestroyTempMaterial();
	int DestroyToon1Buffer();

	int WriteChar( HANDLE hfile, char* writechar, int addreturn );

	int SetUV_X( float startu, float endu, float startv, float endv );
	int SetUV_Y( float startu, float endu, float startv, float endv );
	int SetUV_Z( float startu, float endu, float startv, float endv );
	int SetUV_Cylinder( float startu, float endu, float startv, float endv );
	int SetUV_Sphere( float startu, float endu, float startv, float endv );

	int IsSameMaterial( D3DMATERIAL9* mat1, D3DMATERIAL9* mat2 );
	int IsSameXMaterial( D3DXMATERIAL* xmat1, D3DMATERIAL9* mat2, char* texname2 );

	int IsSameD3DColorValue( D3DCOLORVALUE* col1, D3DCOLORVALUE* col2 );

	int DestroySortTexBuffer();

	int MakeColor( float minval, float maxval, float val, E3DCOLOR3UC target_col, RDBColor3f* dstcol );

public:
	CMeshInfo* meshinfo; // mは頂点数、nは面数、totalは使わない0
	VEC3F* pointbuf;// leng : meshinfo->m
	int* indexbuf;// leng : meshinfo->n ×3


	int* orgnobuf;// leng : meshinfo->m

	int groundflag;
	int billboardflag;
	COORDINATE* uvbuf;
	int*	texkindbuf;

//	D3DMATERIAL9 m_material;	
	CInfElem* m_IE;


	//2004/06/17
	ARGBF* diffusebuf;
	ARGBF* ambientbuf;
	ARGBF* specularbuf;

	float* powerbuf;
	ARGBF* emissivebuf;

	CShdElem* m_mikoapplychild;
	CShdElem* m_mikotarget;

	int tempapplychild;
	int temptarget;

	D3DXVECTOR3 m_center;

	ARGBF* tempdiffuse;
	ARGBF* tempambient;
	ARGBF* tempspecular;
	ARGBF* tempemissive;
	float* temppower;


	D3DXMATERIAL* m_material;
	int m_materialnum;
	DWORD* m_attrib;// meshinfo->n
	int* m_indexbuf2;// meshinfo->n * 3, materialで並び替えたインデックス
	MATERIALBLOCK* m_materialblock;// m_materialnum
	//LPDIRECT3DTEXTURE9* m_toon1tex;
	CToon1Params* m_toon1;
	int* m_toonface2oldface;


	int* m_attrib0;//meshinfo->n rok読み込み時にsh->m_matheadのマテリアル番号を面ごとに保持する。

	SORTTEXTURE* m_sorttex;// meshinfo->n 面ごとのテクスチャ名、テクスチャ名でソートする
	int* m_optindexbuftex;// meshinfo->n * 3, sorttexの順番で並び替えたインデックス
	int m_texblocknum;
	TEXTUREBLOCK* m_texblock;// texblocknum個のレンダー単位

	CHKALPHA chkalpha;

	DIRTYMAT* m_dirtymat;

	E3DCOLOR3UC infcol[CALCMODE_MAX];

	int m_texorgflag;
};



#endif