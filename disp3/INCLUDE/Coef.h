#ifndef COEFH
#define		COEFH

#include <D3DX9.h>
#include <usercoef.h>

class CTreeHandler2;
class CShdHandler;
class CShdElem;
class CMotHandler;
class CMQOMaterial;

#define ALIGNED		_declspec(align(16))
#define	u_long	unsigned long

#define BONELISTNUM	100

#define PICKVERTMAX	100
#define VERTNUMMAX 21845

#define GBANDVAL	4000.0f

#define	TYPENAMELENG	500

#define PATH_LENG	2048

#define MOVEBONETIMER	1
#define MOTPARAMDLGTIMER	2
#define TexKeyDlgTIMER	3
#define DSKeyDlgTIMER	4
#define GraphRotDlgTIMER	5
#define MorphKeyDlgTIMER	6
#define CamKeyDlgTIMER		7
#define GPKeyDlgTIMER		8
#define MOEKeyDlgTIMER		9
#define SndKeyDlgTIMER		10
#define AlpKeyDlgTIMER		11

//#define TIMERINTERVAL	10
//#define TIMERINTERVAL	100
//#define TIMERINTERVAL	200

#define MAXBONENUM	2048
#define MAXMOTIONNUM 300
#define MAXUNDONUM	40

//#define MAXDISPSWITCHNO		15	//<--31より大きくする場合は、Bitno2Mask　も拡張必要！！



#define MAX_BILLBOARD_NUM	2000
//#define MAX_BILLBOARD_NUM	500

#define FOVINDEXSIZE	100

//#define MAXSKINMATRIX	14
//#define MAXSKINMATRIX	43 //<---- d3dapp.cppに移動。グローバル変数に。
//#define MAXJOINTSKIN	20

//#define MCEVENTNUM		16
#define MCEVENTNUM		30

#define FILLUPMOTIONID	0

#define KFPINUM		8
#define KFPDNUM		8

#define MKMLINELENG 2048

#define MAXFORBIDNUM	256

#define SIGEDIT_MSGMAP	2
#define MAIN3D_MSGMAP	3

#define NOGLOW	0
#define WITHGLOW	1

///// materialinfo userocef.h
#define MII_MATNO	0
#define MII_DIFFR	1
#define MII_DIFFG	2
#define MII_DIFFB	3
#define MII_SPCR	4
#define MII_SPCG	5
#define MII_SPCB	6
#define MII_AMBR	7
#define MII_AMBG	8
#define MII_AMBB	9
#define MII_EMIR	10
#define MII_EMIG	11
#define MII_EMIB	12
#define MII_MAX		13

#define MID_ALPHA	0
#define MID_POWER	1
#define MID_MAX		2

enum {
	FB_NORMAL,
	FB_BUNKI,
	FB_ROOT,
	FB_ENDJOINT,
	FB_MAX
};

enum {
	OPTAR_CUR,
	OPTAR_ALL,
	OPTAR_FWD,
	OPTAR_SEL,
	OPTAR_MAX
};

enum {
	CHKDLG_NES,
	CHKDLG_YES,
	CHKDLG_NO,
	CHKDLG_MAX
};


enum {
	MKIND_ROT,
	MKIND_MV,
	MKIND_SC,
	MKIND_MAX
};



enum {
	FUGOU_ZERO,
	FUGOU_PLUS,
	FUGOU_MINUS
};

typedef struct tag_texv
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 uv;
}TEXV;

typedef struct tag_calcsp
{
	int fugouS;
	int fugouE;
	float yvalS;
	float yvalE;
}CALCSP;



enum {
	TAB_BASE,//0
	TAB_AXIS,//1
	TAB_VIK,//2
	TAB_TEX,//3
	TAB_DS,//4
	TAB_M,//5
	TAB_ALP,
	TAB_CAM,//6-->7
	TAB_GP,//7-->8
	TAB_MOE,//8-->9
	TAB_SND,//9-->10
	TAB_MAX
};

typedef struct tag_framesave
{
	int flag;
	int frameno;
	int startframe;
	int endframe;
}FRAMESAVE;

enum {
	SAVEKEY_0,
	SAVEKEY_1,
	SAVEKEY_2,
	SAVEKEY_MAX
};


typedef struct tag_bld
{
	char name[MAX_PATH];
	float mult;
	int flag;
	int mode;
}BLD;

typedef struct tag_SNDELEM
{
	char sndsetname[32];
	int sndsetid;
	char sndname[256];
	int sndid;
}SNDELEM;


//xdlg
typedef struct tag_nameid
{
	char name[256];
	int id;
}NAMEID;


//HSP MOE Anim
enum {
	MOEI_LISTNUM,
	MOEI_NOTLISTNUM,
	MOEI_EVENTNO,
	MOEI_MAX
};

typedef struct tag_ekey
{
	int eventno;
	int key;
	int combono;
	int validflag;
	int singleevent;
}EKEY;


//HSP GP Anim
enum {
	GPI_ONGMODE,
	GPI_INTERP,
	GPI_MAX
};
//HSP GP Anim
enum {
	GPD_POSX,
	GPD_POSY,
	GPD_POSZ,
	GPD_ROTX,
	GPD_ROTY,
	GPD_ROTZ,
	GPD_RAYY,
	GPD_RAYLENG,
	GPD_OFFSETY,
	GPD_MAX
};

enum {
	GP_HEN,
	GP_ANIM,
	GP_MAX
};


typedef struct tag_gpelem
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
	int	ongmode;
	float	rayy;
	float	rayleng;
	float	offsety;
	D3DXVECTOR3 e3dpos;
	D3DXVECTOR3 e3drot;
}GPELEM;


enum {//gpkeydlg m_gtype
	GPG_ROT,
	GPG_POS,
	GPG_MAX
};


enum {//HSP
	CAMI_LOOKTYPE,
	CAMI_HSID,
	CAMI_BONENO,
	CAMI_ORTHO,
	CAMI_INTERP,
	CAMI_MAX
};
enum {//HSP
	CAMD_POSX,
	CAMD_POSY,
	CAMD_POSZ,
	CAMD_TARX,
	CAMD_TARY,
	CAMD_TARZ,
	CAMD_UPX,
	CAMD_UPY,
	CAMD_UPZ,
	CAMD_DIST,
	CAMD_NEARZ,
	CAMD_FARZ,
	CAMD_FOV,
	CAMD_ORTHOSIZE,
	CAMD_MAX
};



enum {
	G_TARGET,
	G_POS,
	G_PROJ,
	G_MAX
};



//ToonMateiralDouble
enum {
	TMD_DIFFUSE_R,
	TMD_DIFFUSE_G,
	TMD_DIFFUSE_B,
	TMD_SPECULAR_R,
	TMD_SPECULAR_G,
	TMD_SPECULAR_B,
	TMD_AMBIENT_R,
	TMD_AMBIENT_G,
	TMD_AMBIENT_B,
	TMD_DARKNL,
	TMD_BRIGHTNL,
	TMD_EDGECOL0_R,
	TMD_EDGECOL0_G,
	TMD_EDGECOL0_B,
	TMD_EDGEWIDTH0,
	TMD_MAX
};
//ToonMaterialInt
enum {
	TMI_VALID0,
	TMI_INV0,
	TMI_MAX
};


enum {
	CREATE_MINUS,
	CREATE_PLUS,
	CREATE_MAX
};

enum {
	M_NONE,
	M_BASE,
	M_TARGET,
	M_MAX
};

enum {
	SHAPEF_SIG,
	SHAPEF_MQO,
	SHAPEF_ROK,
	SHAPEF_X,
	SHAPEF_MAX
};


enum {
	//２次元配列のデータの並び方
	ORDER_HSP,
	ORDER_C,
	ORDER_MAX
};

// material mode
enum {
	MMODE_VERTEX,
	MMODE_FACE,
	MMODE_MAX
};


enum {
	MOAT_IDLING,
	MOAT_EV0IDLE,
	MOAT_COMID,
	MOAT_NOTCOMID,
	MOAT_BRANCHNUM,
	MOAT_MAX
};

enum {
	MOAB_MOTID,
	MOAB_EVENTID,
	MOAB_FRAME1,
	MOAB_FRAME2,
	MOAB_NOTFU,
	MOAB_MAX
};

enum {
	ROT_ZXY,
	ROT_YXZ,
	ROT_MAX
};

enum {
	OW_YES,
	OW_NO,
	OW_ALLYES,
	OW_ALLNO
};

typedef struct tag_rttexelem
{
	LPDIRECT3DTEXTURE9 ptex;
	LPDIRECT3DSURFACE9 psurf;
	LPDIRECT3DSURFACE9 pZsurf;
	int width;
	int height;
	int texid;
} RTTEXELEM;

typedef struct tag_targetprim
{
	int seri;
	CShdElem* selem;
} TARGETPRIM;


typedef struct tag_mprim
{
	CShdElem* selem;
	float value;
} MPRIM;


typedef struct tag_keyframeno
{
	int frameno;
	int kind;
} KEYFRAMENO;


typedef struct tag_skintangent
{
	float tangent[4];
} SKINTANGENT;

typedef struct tag_chkalpha
{
	int alphanum;
	int notalphanum;
} CHKALPHA;

typedef struct tag_sericonv
{
	int seri;
	int extseri;
} SERICONV;

typedef struct tag_savepos
{
	char name[256];
	D3DXVECTOR3 pos;
} SAVEPOS;


enum {
	CLONE_NONE,
	CLONE_PARENT,
	CLONE_CHILD,
	CLONE_MAX
};

typedef struct tag_mqobuf
{
	HANDLE hfile;
	unsigned char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} MQOBUF;

typedef struct tag_mabuf
{
	HANDLE hfile;
	char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} MABUF, TO1BUF;

typedef struct tag_mkmbuf
{
	HANDLE hfile;
	char* buf;
	int bufleng;
	int pos;
	int isend;
} MKMBUF;

#define MAXMCCOPYNUM	100

typedef struct MCELEM
{
	int setno;//lParam
	int id;//motcookie
	int idling;
	int ev0idle;//eventno 0 でアイドリングに戻す
	int commonid;//共通分岐イベント番号

	int forbidnum;//共通分岐禁止イベント番号の要素数。
	int* forbidid;//共通分岐禁止イベント番号

	int childnum;
	MCELEM* childmc;
	
	int frameno1;
	int frameno2;//拡張用
	int eventno1;
	int eventno2;//拡張用
	int notfu;//補間なし

	int closetree;
	int nottoidle;
} MCELEM;

typedef struct tag_motid
{
	int id;
	char filename[256];
	int ev0idle;
	int commonid;
	int forbidnum;
	int* forbidid;
	int notfu;

	LONG startframe;
	LONG endframe;
	LONG framerange;
} MOTID;

typedef struct tag_boneinfluence
{
	DWORD Bone;
    DWORD numInfluences;
    DWORD *vertices;
    FLOAT *weights;
} BONEINFLUENCE;


enum
{
	MATERIAL_DIFFUSE,
	MATERIAL_SPECULAR,
	MATERIAL_AMBIENT,
	MATERIAL_EMISSIVE,
	MATERIAL_POWER,
	MATERIAL_MAX
};

//paintdlgの表のSUBID
enum
{
	SUBID_NAME,
	SUBID_CALC,
	SUBID_RATE,
	SUBID_ORGINF,
	SUBID_DISPINF,
	SUBID_MAX
};

//AlpKeyDlgの表のSUBID
enum
{
	ALPSUB_MATNAME,
	ALPSUB_ALPHA,
	ALPSUB_MAX
};

//TexKeyDlgの表のSUBID
enum
{
	TEXSUB_MATNAME,
	TEXSUB_TEXNAME,
	TEXSUB_MAX
};

//DSKeyDlgの表のSUBID
enum
{
	DSSUB_SWNO,
	DSSUB_STATE,
	DSSUB_MAX
};

//MorphKeyDlgの表のSUBID
enum
{
	MSUB_TARGET,
	MSUB_VALUE,
	MSUB_MAX
};

typedef struct tag_skinblock
{
	int startface;
	int endface;
} SKINBLOCK;

typedef struct tag_materialblock
{
	int materialno;
	int startface;
	int endface;
	CMQOMaterial* mqomat;
} MATERIALBLOCK;

typedef struct tag_textureblock
{
	CMQOMaterial* mqomat;
	int startface;//startfaceを含み、endfaceを含まない！！！
	int endface;
} TEXTUREBLOCK;

typedef struct tag_sorttexture
{
	CMQOMaterial* mqomat;
	int faceno;
} SORTTEXTURE;

typedef struct tag_dirtymat
{
	int materialno;
	int* dirtyflag;
} DIRTYMAT;


typedef struct tag_renderblock
{
	int skinno;
	int materialno;
	int startface;//startfaceを含み、endfaceを含まない！！！
	int endface;
	CMQOMaterial* mqomat;
} RENDERBLOCK;

/***
typedef struct tag_skinvertex
{
	float pos[4];
	float normal[4];
	float weight[4];
	float boneindex[4];

	float tex1[2];//tex0

	DWORD diffuse;
	DWORD specular;
	DWORD ambient;
	float power;
	DWORD emissive;
//	float diffuse[4];
//	float ambient[3];
//	float specular[3];
//	float power;
//	float emissive[3];
} SKINVERTEX;
***/


typedef struct tag_skinvertex
{
	float pos[4];
	float normal[4];
	float weight[4];
	float boneindex[4];

	float tex1[2];//tex0
} SKINVERTEX;

typedef struct tag_skinmaterial
{
	DWORD diffuse;
	DWORD specular;
	DWORD ambient;
	float power;
	DWORD emissive;
} SKINMATERIAL;


/***
#define MOTION_IDLE	1
#define MOTION_KICK 2
#define MOTION_PUNCH	4
#define MOTION_ACT1	8
#define MOTION_DEFENSE 0x10
#define MOTION_DEATH	0x20
#define MOTION_WIN		0x40
//#define MOTION_MAX		
***/

#define IDTBB_R		501
#define IDTBB_T		502
#define IDTBB_T2	503
#define IDTBB_S		504
#define IDTBB_ITI	505
#define IDTBB_IKT	506
#define IDTBB_COL	507
#define IDTBB_BONE	508
#define IDTBB_EI1	509
#define IDTBB_EI2	510
#define IDTBB_EI3	511
#define IDTBB_TOON1	512
#define IDTBB_MO	513
#define IDTBB_FACE	514
#define IDTBB_LINE	515
#define IDTBB_POINT	516
#define IDTBB_TEN2	517
#define IDTBB_ZA1	518
#define IDTBB_ZA2	519
#define IDTBB_ZA3	520
//#define IDTBB_COLR	520
#define IDTBB_COLP	521
//#define IDTBB_COLB	522
//#define IDTBB_COLS	523
//#define IDTBB_COLM	524
#define IDTBB_COLT	525
//#define IDTBB_OFC	526
//#define IDTBB_OFS	527
//#define IDTBB_OFO	528
#define IDTBB_SHA	529

#define IDTBB_4			530
#define IDTBB_ZA4		531
#define IDTBB_GLOW		532
#define IDTBB_COLPP		533


typedef struct tag_nextmotion
{
	int mk;
	int befframeno;
	int aftframeno;
} NEXTMOTION;

typedef struct tag_vcoldata
{
	int vertno;
	__int64 vcol;
} VCOLDATA;

enum {
	SYMMTYPE_NONE,
	SYMMTYPE_XP,
	SYMMTYPE_XM,
	SYMMTYPE_YP,
	SYMMTYPE_YM,
	SYMMTYPE_ZP,
	SYMMTYPE_ZM,

	SYMMTYPE_X,
	SYMMTYPE_Y,
	SYMMTYPE_Z,


	SYMMTYPE_MAX
};

typedef struct tag_sigtoon1
{
	char name[32];

	D3DCOLORVALUE diffuse;
	D3DCOLORVALUE ambient;
	D3DCOLORVALUE specular;

	float darkh;//bmpの塗りつぶし範囲
	float brighth;

	float ambientv;//UVのVのサンプリング位置
	float diffusev;
	float specularv;
	
	float darknl;//法線とライトの内積の閾値
	float brightnl;

//	LPDIRECT3DTEXTURE9 tex;

	//float reserved1, reserved2, reserved3, reserved4, reserved5;
	D3DCOLORVALUE edgecol0;
	char edgevalid0;
	char edgeinv0;
	char reserved1, reserved2;

}SIGTOON1;

typedef struct tag_sigtoon1ex
{
	SIGTOON1 toon1;
	float	toon0dnl;
	float	toon0bnl;
	float	reserved[12];
}SIGTOON1EX;


typedef struct tag_siginfelem
{
	int childno;//影響ボーン、子供の番号。エンドジョイントもあり。
	int bonematno;//マトリックス格納ボーンの番号、親の番号。bonematno, parmatno。子供を持つジョイントの番号しか入らない。
	int kind;//CALC_*
	float userrate;//％
	float orginf;//CALC_*で計算した値。
	float dispinf;//　orginf[] * userrate[]、normalizeflagが１のときは、正規化する。

	float reserved1, reserved2, reserved3;
}SIGINFELEM;

typedef struct tag_infelemheader
{
	int infnum;
	int normalizeflag;
	int symaxis;
	float symdist;
}INFELEMHEADER;

typedef struct tag_im2header
{
	char name[256];
	int vertnum;
}IM2HEADER;



enum {
	PAINTOPE_SETINF,
	PAINTOPE_BONE,
	PAINTOPE_MAX
};

enum {
	BONEOPE_CREATE,
	BONEOPE_POS,
	BONEOPE_MAX
};

#define INFNUMMAX	4


typedef struct tag_infelem
{
	int childno;//影響ボーン、子供の番号。エンドジョイントもあり。
	int bonematno;//マトリックス格納ボーンの番号、親の番号。bonematno, parmatno。子供を持つジョイントの番号しか入らない。
	int kind;//CALC_*
	float userrate;//％
	float orginf;//CALC_*で計算した値。
	float dispinf;//　orginf[] * userrate[]、normalizeflagが１のときは、正規化する。
}INFELEM;


enum {
	MOUSEOPE_NONE,
	MOUSEOPE_CAMROT,
	MOUSEOPE_CAMMOVE,
	MOUSEOPE_CAMDIST,
	MOUSEOPE_BONETWIST,
	MOUSEOPE_MAX
};


#define MPALETTENUM	16

typedef struct tag_material0
{
	COLORREF	diffuse;
	COLORREF	specular;
	COLORREF	ambient;
	COLORREF	emissive;
	float		power;
}MATERIAL0;

typedef struct tag_mpalette
{
	char name[64];
	MATERIAL0 mat;
}MPALETTE;


enum {
	LBLEND_MULT,
	LBLEND_SCREEN,
	LBLEND_OVERLAY,
	LBLEND_HARDLIGHT,
	LBLEND_DODGE,
	LBLEND_MAX
};


enum {
	OVERFLOW_CLAMP,
	OVERFLOW_SCALE,
	OVERFLOW_ORG,
	OVERFLOW_MAX
};

enum {
	MOVEMODE_RELATIVE,
	MOVEMODE_ABSOLUTE,
	MOVEMODE_MAX
};
enum {
	ITIOBJ_VERTEX,
	ITIOBJ_FACE,
	ITIOBJ_PART,
	ITIOBJ_JOINT,
	ITIOBJ_JOINTROT,
	ITIOBJ_MAX
};



//texbankで使用（namebank Type）
enum {
	TEXTYPE_NONE,
	TEXTYPE_TEXTURE,
	TEXTYPE_SURFACE,
	TEXTYPE_MAX
};



enum {
	MIKOBONE_NONE,
	MIKOBONE_NORMAL,
	MIKOBONE_FLOAT,
	MIKOBONE_ILLEAGAL,
	MIKOBONE_MAX
};

enum {
	MIKODEF_NONE,
	MIKODEF_SDEF,
	MIKODEF_BDEF,
	MIKODEF_NODEF,
	MIKODEF_MAX
};

enum {
	MIKOBLEND_SKINNING,
	MIKOBLEND_MIX,
	MIKOBLEND_MAX
};


//WM_USER_IMPORTMQO, WM_USER_LOADBONで使用
typedef struct _tag_loadfileinfo
{
	char* filename;
	float mult;
	int needsetresdir;
	CTreeHandler2* lpth;
	CShdHandler* lpsh;
	CMotHandler* lpmh;
} LOADFILEINFO;


//for bvhelem
enum {
	CHANEL_XPOS,
	CHANEL_YPOS,
	CHANEL_ZPOS,
	CHANEL_ZROT,
	CHANEL_XROT,
	CHANEL_YROT,
	CHANEL_MAX
};


enum {
	IKMODE_JOINT,
	IKMODE_OPE,
	IKMODE_MAX
};

enum {//SetRenderTargetするたびに設定する。
	TARGET_DEFAULT,
	TARGET_MAIN,
	TARGET_SUB1,
	TARGET_SUB2,
	TARGET_SHADOWMAP,
	
	TARGET_GLOWVIEW,
	TARGET_GLOWORG,
	TARGET_GLOWSMALL,
	TARGET_BLURWORK,
	TARGET_BLURRESULT,

	TARGET_MAX
};

enum {
	PARSMODE_PARS,
	PARSMODE_ORTHO,
	PARSMODE_MAX
};


//d3ddisp SetRevVertex : e3dhsp ver1036 2004/3/13
#define COPYVERTEX		1
#define COPYNORMAL		2
#define COPYUV			4
#define COPYCOLOR		8
#define COPYSPECULAR	16
#define COPYBLEND		32

typedef struct tag_viewparam
{
	float degxz;
	float degy;
	float eye_y;
	float camdist;
	D3DXVECTOR3 targetpos;
} VIEWPARAM;


typedef struct tag_frustuminfo
{
    D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
    D3DXPLANE planeFrustum[6];    // planes of the view frustum
} FRUSTUMINFO;



/***
typedef struct tag_mpinfo
{
	int qid;
	D3DXVECTOR3 tra;
	int frameno;
	int dispswitch;
	int interpolation;
	int reserved1;
	int reserved2;
	int reserved3;
	int reserved4;
} MPINFO;
***/
//2005/05/02
typedef struct tag_mpinfo
{
	int qid;
	D3DXVECTOR3 tra;
	int frameno;
	int dispswitch;
	int interpolation;
	D3DXVECTOR3 scale;
	int userint1;
} MPINFO;
//MPInfo 2005/05/02

enum {
	MPI_QUA = 0,
	MPI_TRAX,
	MPI_TRAY,
	MPI_TRAZ,
	MPI_FRAMENO,
	MPI_DISPSWITCH,
	MPI_INTERP,
	MPI_SCALEX,
	MPI_SCALEY,
	MPI_SCALEZ,
	MPI_USERINT1,
	MPI_SCALEDIV,
	MPI_MAX
};


typedef struct tag_guardband
{
	float left;
	float top;
	float right;
	float bottom;
	float maxrhw;
}GUARDBAND;


typedef struct location {
	float	x;
	float	y;
	float	z;
} VEC3F, LOCATION, ROTATE;


typedef struct tag_tvertex {
	float pos[4]; 
	float tex[2];
} TVERTEX;

typedef struct tag_tlvertex {
	float pos[4];
	float diffuse[4];
	float tex[2];
} TLVERTEX;

typedef struct _TRANSLITVERTEX {
    float sx, sy;      // スクリーン座標
    float sz;         // Z バッファ深度
    float rhw;       // 同次 W の逆数
    DWORD color;   // ディフューズ色
    DWORD specular;  // スペキュラ色
    float tu, tv;  // テクスチャ座標
	float tu2, tv2;
} D3DTLVERTEX, *LPD3DTLVERTEX;

typedef struct _TRANSLITVERTEX2 {
	// texture ２枚
    float sx, sy;      // スクリーン座標
    float sz;         // Z バッファ深度
    float rhw;       // 同次 W の逆数
    DWORD color;   // ディフューズ色
    DWORD specular;  // スペキュラ色
    float tu1, tv1;  // テクスチャ座標
	float tu2, tv2;
} D3DTLVERTEX2, *LPD3DTLVERTEX2;

typedef struct tag_effecttlv {
	float pos[4];
	float diffuse[4];
	float tex[2];
} EFFECTTLV;

//e3dhsp ver1036 2004/3/12
typedef struct _D3DVERTEX {
	float x, y, z;
	float nx, ny, nz;
	DWORD color;
	DWORD specular;
	float tu, tv;
} D3DVERTEX, *LPD3DVERTEX;

typedef struct _D3DLVERTEX {
	float x, y, z;
	DWORD color;
} D3DLVERTEX, *LPD3DLVERTEX;


typedef struct tag_pm3optv {
	D3DTLVERTEX opttlv;
	D3DXVECTOR4 optambient;
	D3DXVECTOR4 optcolorbuf;
	
	float optpowerbuf;
	D3DXVECTOR4 optemissivebuf;
	int orgvno;
	int faceno;
}PM3OPTV;

typedef struct tag_pm3n3param {	
	D3DXVECTOR2 uv[3];
	D3DXVECTOR3 vcol[3];
	int			vcolflag[3];
	int			createflag[3];
	int			smoothflag[3];
}PM3N3PARAM;

typedef struct tag_pm3faceparam {
	int faceno;
	int materialno;

	int vno[3];

	D3DXVECTOR4 basecol;
	D3DXVECTOR3 sceneamb;
	float diffuse;
	float ambient;
	float specular;
	float power;
	float emissive;
	D3DXVECTOR3 orgnormal;

	PM3N3PARAM n3param;
}PM3FACEPARAM;

typedef struct tag_pm3chkv
{
	int vertno;
	PM3FACEPARAM* faceptr;	
}PM3CHKV;

//e3dhsp ver1036 2004/3/12
enum {
	TLMODE_ORG,
	TLMODE_D3D,
	TLMODE_SEL,
	TLMODE_MAX
};


enum {
	MOTION_IDLE,
	MOTION_KICK,
	MOTION_PUNCH, //MOTION_CHOPPU,
	MOTION_ACT1,  //MOTION_UPPER,
	MOTION_DEFENSE,
	MOTION_DEATH,
	MOTION_WIN,
	MOTION_MAX
};


#define FIGHTINGTIME		120
//#define FIGHTINGTIME		10

enum {
	ENDFLAG_NONE,
	ENDFLAG_KO,
	ENDFLAG_TIMEUP,
	ENDFLAG_OUTOFRANGE,
	ENDFLAG_MAX
};

enum {
	RGBFLAG_R,
	RGBFLAG_G,
	RGBFLAG_B,
	RGBFLAG_MAX
};

enum {
	JI_SERIAL = 0, 
	JI_NOTUSE,
	JI_PARENT,
	JI_CHILD,
	JI_BROTHER,
	JI_SISTER,
	JI_MAX
};


//DispObjInfo 2004/9/28
enum {
	DOI_SERIAL = 0,
	DOI_NOTUSE,
	DOI_DISPSWITCH,
	DOI_INVISIBLE,
	DOI_MAX
};



// error code
// d3dapp.h から移動。

#ifndef D3DAPPERR_NODIRECT3D
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESIZEFAILED        0x8200000c

#endif

typedef struct tag_errormes {
	int errorcode;
	DWORD type;
	char* mesptr;
} ERRORMES;


/////////////////////////
// viewer m_polydispmode
enum {
	POLYDISP_ALL,
	POLYDISP_SEL,
	POLYDISP_MAX
};


//////////////////////

enum {
	EXTTEXREP_STOP,
	EXTTEXREP_CLAMP,
	EXTTEXREP_ROUND,
	EXTTEXREP_MAX
};


//////////////////////
//interpolation
enum {
	INTERPOLATION_SLERP,
	INTERPOLATION_SQUAD,
	INTERPOLATION_MAX
};

///////////////////////
//light

//D3DLIGHT_* を使うようにする

//enum {
//	LIGHT_DIRECTIONAL,
//	LIGHT_POINT,
//	LIGHT_SPOT,
//	LIGHT_SEARCH,
//	LIGHT_MAX
//};


//////////////////////
//user defined window message

#define WM_USER_DISPLAY		WM_USER
#define WM_USER_KEY			(WM_USER + 1)
#define WM_USER_MOVE		(WM_USER + 2)

#define WM_USER_REMAKE_TREE	(WM_USER + 3)
#define WM_USER_REMAKE_TEXTURE	(WM_USER + 4)
#define WM_USER_RENAME		(WM_USER + 5)

#define WM_USER_REFUGE_MOTDLG	(WM_USER + 6)
#define WM_USER_PUTBACK_MOTDLG	(WM_USER + 7)
#define WM_USER_ENDDIALOG		(WM_USER + 8)

#define WM_USER_REMAKE_DISPOBJ	(WM_USER + 9)
#define WM_USER_REMAKE_UV		(WM_USER + 10)

#define WM_USER_SELCHANGE		(WM_USER + 11)
#define WM_USER_MOVING			(WM_USER + 12)

#define WM_USER_CHANGE_COLOR	(WM_USER + 13)

#define WM_USER_CHANGE_NOTUSE	(WM_USER + 14)
#define WM_USER_ENABLE_MENU		(WM_USER + 15)

#define WM_USER_ERROR			(WM_USER + 16)

#define WM_USER_CHANGE_JOINTLOC	(WM_USER + 17)
#define WM_USER_NEW_JOINT		(WM_USER + 18)

#define WM_USER_INFLUENCE		(WM_USER + 19)

#define WM_USER_PREVIEW			(WM_USER + 20)
#define WM_USER_SELLOCK			(WM_USER + 21)
#define WM_USER_INIT_UNDOBUF	(WM_USER + 22)

#define WM_USER_CREATE_PB		(WM_USER + 23)
#define WM_USER_SET_PB			(WM_USER + 24)
#define WM_USER_DESTROY_PB		(WM_USER + 25)

#define WM_USER_BONEDISP		(WM_USER + 26)

#define WM_USER_IKDISPLAY		(WM_USER + 27)

#define WM_USER_IMPORTMQO		(WM_USER + 28)
#define WM_USER_LOADBON			(WM_USER + 29)
#define WM_USER_DEL_JOINT		(WM_USER + 30)
#define WM_USER_CONV2SCOPE		(WM_USER + 31)
#define WM_USER_MAKEMORPH		(WM_USER + 32)
#define WM_USER_RESTOREHANDLER	(WM_USER + 33)

#define WM_USER_ANIMMENU		(WM_USER + 34)
#define WM_USER_CALCEUL			(WM_USER + 35)

#define WM_USER_CAMERAMENU		(WM_USER + 36)
#define WM_USER_GPMENU			(WM_USER + 37)
#define WM_USER_MOEMENU			(WM_USER + 38)


//WM_USER_KEY の WPARAM
#define ID_KEY_LEFT		1
#define ID_KEY_RIGHT	2
#define ID_KEY_UP		3
#define ID_KEY_DOWN		4
#define ID_KEY_ZOOMIN	5
#define ID_KEY_ZOOMOUT	6
#define	ID_KEY_TARGET_LEFT	7
#define	ID_KEY_TARGET_RIGHT	8
#define ID_KEY_RAD2_UP	9
#define ID_KEY_RAD2_DOWN	10

#define ID_KEY_90LEFT	11
#define ID_KEY_90RIGHT	12
#define ID_KEY_90UP		13
#define ID_KEY_90DOWN	14


#define ID_KEY_UD_RESET	20
#define ID_KEY_ROT_RESET	21
#define ID_KEY_ZOOM_RESET	22
#define ID_KEY_TARGET_RESET	23
#define ID_KEY_ALL_RESET	24

#define ID_KEY_RAD2_RESET	25

#define ID_KEY_TARGET_UP	26
#define ID_KEY_TARGET_DOWN	27

#define ID_KEY_SYMMX		28
#define ID_KEY_JOINTTARGET	29

//WM_USER_MOVEのWPARAM
	//messageの送信元を表す。
enum {
	ID_MSG_FROM_CAMERADLG,
	ID_MSG_FROM_SIGDLG,
	ID_MSG_FROM_MOTDLG,
	ID_MSG_FROM_BONEDLG,

	ID_MSG_FROM_INFLUENCEDLG,
	ID_MSG_FROM_INFSCOPEDLG,
	ID_MSG_FROM_PAINTDLG,
	ID_MSG_FROM_MAX
};

////////////////////////////////
#define BITMASK_0	0x00000001
#define BITMASK_1	0x00000002
#define BITMASK_2	0x00000004
#define BITMASK_3	0x00000008

#define BITMASK_4	0x00000010
#define BITMASK_5	0x00000020
#define BITMASK_6	0x00000040
#define BITMASK_7	0x00000080

#define BITMASK_8	0x00000100
#define BITMASK_9	0x00000200
#define BITMASK_10	0x00000400
#define BITMASK_11	0x00000800

#define BITMASK_12	0x00001000
#define BITMASK_13	0x00002000
#define BITMASK_14	0x00004000
#define BITMASK_15	0x00008000

#define BITMASK_16	0x00010000
#define BITMASK_17	0x00020000
#define BITMASK_18	0x00040000
#define BITMASK_19	0x00080000

#define BITMASK_20	0x00100000
#define BITMASK_21	0x00200000
#define BITMASK_22	0x00400000
#define BITMASK_23	0x00800000

#define BITMASK_24	0x01000000
#define BITMASK_25	0x02000000
#define BITMASK_26	0x04000000
#define BITMASK_27	0x08000000

#define BITMASK_28	0x10000000
#define BITMASK_29	0x20000000
#define BITMASK_30	0x40000000
#define BITMASK_31	0x80000000



////////////////////////////////
enum _projmode {
	PROJ_NORMAL,
	PROJ_LENS,
	PROJ_PREINIT,// init colorする前の、transform用
	PROJ_MAX
};

////////////////////////////////
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)
#define D3DFVF_TLVERTEX2 (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)

#define D3DFVF_VERTEX	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

#define D3DFVF_LVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

////////////////////////////////
//  handler の種類
#define TREEHANDLER	0x01
#define SHDHANDLER	0x02
#define HANDLERMAX	0x04

///////////////////////////////
// CTreeHandler

#define	TMODE_EXPORT		0x01
#define	TMODE_IMPORT		0x02
#define	TMODE_ONCE			0x04// 同じ名前不可
#define	TMODE_LEVEL_ONCE	0x08// level, name で一意なら可能
#define	TMODE_MULT			0x10// 同じ名前が何個あっても可能

enum _addt_type {
	ADDT_NONE, // 引数の矛盾　エラー
	ADDT_UP, // depth--
	ADDT_DOWN, // depth++
	ADDT_LEVEL, // 同じdepth
	ADDT_MAX
};

/////////////////////////////
// CSigFile
//#define SIGFILEMAGICNO 10203

	//2002/6/12に変更。
#define SIGFILEMAGICNO_1	10204

	//2004/6/19に追加。
#define SIGFILEMAGICNO_2	( SIGFILEMAGICNO_1 + 1 )
#define SIGFILEMAGICNO_4	( SIGFILEMAGICNO_1 + 3 )
#define SIGFILEMAGICNO_5	( SIGFILEMAGICNO_1 + 4 )
#define SIGFILEMAGICNO_6	( SIGFILEMAGICNO_1 + 5 )
	//rokdebone2 ver5029
#define SIGFILEMAGICNO_7	( SIGFILEMAGICNO_1 + 6 )
#define SIGFILEMAGICNO_8	( SIGFILEMAGICNO_1 + 7 )
#define SIGFILEMAGICNO_9	( SIGFILEMAGICNO_1 + 8 )
	//rokdebone2 ver5042
#define SIGFILEMAGICNO_10	( SIGFILEMAGICNO_1 + 9 )
	//rokdebone2 ver5200
#define SIGFILEMAGICNO_11	( SIGFILEMAGICNO_1 + 10 )
	//rokdebone2 ver5208
#define SIGFILEMAGICNO_12	( SIGFILEMAGICNO_1 + 11 )
	//rokdebone2 ver5216
#define SIGFILEMAGICNO_13	( SIGFILEMAGICNO_1 + 12 )
	//rokdebone2 ver5315
#define SIGFILEMAGICNO_14	( SIGFILEMAGICNO_1 + 13 )
	// ！！！！更新するときは、shandler m_sigmagicno も忘れずに！！！！！



#define GNDFILEMAGICNO_1	10500
#define GNDFILEMAGICNO_2	( GNDFILEMAGICNO_1 + 1 )
#define GNDFILEMAGICNO_3	( GNDFILEMAGICNO_1 + 2 )
#define GNDFILEMAGICNO_4	( GNDFILEMAGICNO_1 + 3 )
#define GNDFILEMAGICNO_6	( GNDFILEMAGICNO_1 + 5 )



/////////////////////////////
// CQuaFile
#define QUAFILEMAGICNO	11223

	//RokDeBone2 ver1008, easy3d ver2005以降
#define QUAFILEMAGICNO2	( QUAFILEMAGICNO + 1 )

	//RokDeBone2 ver1.0.1.0, easy3d ver2007以降
#define QUAFILEMAGICNO3	( QUAFILEMAGICNO + 2 ) 

	//RokDeBone2 ver1.1.1.9以降
#define QUAFILEMAGICNO4	( QUAFILEMAGICNO + 3 ) 

	//RokDeBone2 ver4.0.1.9以降
#define QUAFILEMAGICNO5 ( QUAFILEMAGICNO + 4 )

	//RokDeBone2 ver4.0.2.6以降
#define QUAFILEMAGICNO6 ( QUAFILEMAGICNO + 5 )

	//RokDeBone2 ver4.1.0.2以降
#define QUAFILEMAGICNO7 ( QUAFILEMAGICNO + 6 )

	//RokDeBone2 ver5.0.3.4以降
#define QUAFILEMAGICNO8 ( QUAFILEMAGICNO + 7 )

	//RokDeBone2 ver5.0.5.0以降
#define QUAFILEMAGICNO9 ( QUAFILEMAGICNO + 8 )

	//RokDeBone2 ver5.2.0.0以降
#define QUAFILEMAGICNO10 ( QUAFILEMAGICNO + 9 )

	//RokDeBone2 ver5.3.1.5以降
#define QUAFILEMAGICNO11 ( QUAFILEMAGICNO + 10 )

	//RokDeBone2 ver5.3.3.7以降
#define QUAFILEMAGICNO12 ( QUAFILEMAGICNO + 11 )

	//RokDeBone2 ver5.3.4.1以降
#define QUAFILEMAGICNO13 ( QUAFILEMAGICNO + 12 )


///////////////////////////////////
// PoseFile
#define POSEFILEMAGICNO		33221




/////////////////////////////
// CMotFile
/////////////////////////////

#define MOTFILEMAGICNO	12345
//#define MOTMARKER_VAL	33333
#define MOTMARKER_BONE	22222
#define MOTMARKER_MORPH	44444
#define MOTMARKER_END	-1

/////////////////////////////
// CMotionInfo
//		mottype : motno の制御方法。

enum _motiontype {
	MOTIONTYPENONE,

	MOTION_STOP, // motionno 固定。
	MOTION_CLAMP, // motionno が　maxに達したら、そのままそこで固定。
	MOTION_ROUND, //		最初に戻る
	MOTION_INV,	//			逆方向に進む。
	MOTION_JUMP,
	//MOTION_BIV, // 落ち着いたら、バイブレーションも追加。 

	MOTIONTYPEMAX
};


////////////////////

	// SKIPMAXは、CShdTree::l2wskipmat の　セットフラグのビット数に影響
#define SKIPMAX	31

// dumpflag, dispflag兼用
#define DISP_TRANS	0x01
#define DISP_MATRIX	0x02
#define	DISP_MATERIAL	0x04
#define DISP_VERTEX	0x08

#define DISP_JOINT	0x0100
#define	DISP_VLINE	0x0200
#define	DISP_BMESH	0x0400
#define	DISP_SPHERE	0x0800
#define DISP_DISK	0x1000
#define DISP_POLYGON	0x2000
#define DISP_REVOLVED	0x4000
#define DISP_BLINE	0x8000

#define DISP_MOTCTRL	0x10000

#define DUMP_TEXT		0x20000
#define DUMP_BINARY		0x40000


#define DISP_HEADER	( DISP_TRANS | DISP_MATRIX | DISP_MATERIAL )
#define DISP_ALLOBJ ( DISP_JOINT | DISP_VLINE | DISP_BMESH | DISP_SPHERE | DISP_DISK | DISP_POLYGON | DISP_REVOLVED | DISP_BLINE )
#define DISP_ALL	( DISP_HEADER | DISP_VERTEX | DISP_ALLOBJ | DISP_MOTCTRL )

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// objtypes を　変更した場合には、
// shade plugin : simpleexporter.cpp : simple_exporter_debug.dll に影響するので注意
//				  motionexporter.cpp	
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//namespace dispcoef {
	enum objtypes {
		SHDTYPENONE,
		SHDMESHINFO, 
		SHDMATERIAL, 
		SHDMATRIX,

		SHDROOT, // depth の数(start, begin, end)とtreeのかずを合わせるためのダミー

			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// !!!!!  SHDROTATE から　SHDPART の間をいじるときは、
			// !!!!!  basedat.cpp : NewMeshInfo : if( total <=0 も変更
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 　
		SHDROTATE, // joint 変更時は　IsJoint() も変更
		SHDSLIDER,
		SHDSCALE,
		SHDUNISCALE,
		SHDPATH, // path joint から見て　sister の関係にある線形状に対応して、translateする		
		SHDMORPH,
		
		SHDBALLJOINT,
		SHDPART, // objを束ねる。自分自身は3dobjを持たない。


		SHDPOLYLINE, // // CVecLine
		SHDPOLYGON, // CPolygon
		SHDSPHERE,	// CSphere
		SHDBEZIERSURF, // CBezMesh
		SHDMESHES, // CRevolved 
		SHDDISK,	// CDisk
		
		SHDEXTRUDE, // CExtrude
		
			// 以下は shadeのtypeじゃない
		SHDLINE,		// (MESHの構成LINE)CVecLine, shadeのpolylineは開いているものだけのようなので、こだわるようなので一応別にしとく。
		
		SHDMESH,
		SHDBEZPOINT, // CBezData
		SHDBEZLINE,	// CBezLine
		SHDPLANES,

		SHDPOLYMESH,//CPolyMesh

		SHDPOLYMESH2,//2003/2/8追加

		SHDGROUNDDATA,//2003/6/19追加

		SHDBILLBOARD,//2003/7/4追加

		SHDEXTLINE, //2004/3/30
		
		SHDDESTROYED, //2005/12/01

		SHDINFSCOPE, //2005/12/08

		SHDBBOX, //2007/03/22

		SHDTYPEERROR,
		SHDTYPEMAX
	};

	// line の添え字に使う
	enum line_index_m {
		LIM_POS, LIM_INH, LIM_OUTH, LIM_LATINH, LIM_LATOUTH, LIM_MAX
	};
	enum line_index_m2 {
		LIM2_POS, LIM2_NORMAL, LIM2_MAX
	};

//}

	enum jointsubtype { // jointの軸
		JOINTTYPENONE, 
		JOINTX, 
		JOINTY, 
		JOINTZ, 
		JOINTUNI, 
		JOINTCOMPLEX,
		JOINTTYPEMAX
	};



#define TREEDEPTHMAX	50


//#define BASE_NAME	0x01
#define BASE_TYPE	0x02

	// LOCATION
#define	VEC_X	0x04
#define	VEC_Y	0x08
#define	VEC_Z	0x10

	// MESHINFO
//#define MESHI_TYPE	0x10
#define	MESHI_M		0x20
#define	MESHI_N		0x40
#define	MESHI_TOTAL	0x80
#define	MESHI_MATERIAL	0x100
#define MESHI_MCLOSED	0x200
#define MESHI_NCLOSED	0x400
#define MESHI_SKIP		0x800
#define MESHI_HASCOLOR	0x1000
	
	// VECLINE
#define	VEC_LINE	0x2000

	// BEZDATA
//#define	BEZ_SUBNAME	0x001000
#define	BEZ_POS		0x010000
#define	BEZ_INH		0x020000
#define	BEZ_OUTH	0x040000
#define	BEZ_LATINH	0x080000
#define	BEZ_LATOUTH	0x100000

	// BEZLINE
#define	LINE_POS	0x0200000
#define	LINE_INH	0x0400000
#define	LINE_OUTH	0x0800000
#define	LINE_LATINH	0x1000000
#define	LINE_LATOUTH	0x2000000

	// BEZMESH
#define	BEZ_MESH	0x4000000

	// VECMESH
#define VEC_MESH	0x8000000

	// MATERIAL
#define	MAT_DIFFUSE		0x010000000
#define	MAT_SPECULAR	0x020000000
#define	MAT_AMBIENT		0x040000000

	// !!!! __int64 !!!!

	// PLANES
#define PLANES_LINE		0x0100000000
#define	PLANES_NORMAL	0x0200000000

	// SPHERE
#define SPH_DEFMAT	0x0400000000

	// REVOLVED
#define	REV_FROM	0x020000000000
#define	REV_TO		0x040000000000
#define	REV_AXIS	0x080000000000
#define REV_NSET	0x100000000000

	// DISK
#define	DISK_DEFMAT	0x10000000000000
#define DISK_FROM	0x20000000000000
#define DISK_TO		0x40000000000000
#define DISK_ISFILLED	0x80000000000000

	// MATRIX
#define	MAT_CURRENT		0x0100000000000000
#define MAT_LOCAL2WORLD	0x0200000000000000
#define MAT_SHAPE0		0x0400000000000000
#define MAT_SHAPE		0x0800000000000000
#define MAT_PART		0x1000000000000000

#define BASE_ALL ( BASE_TYPE )
#define VEC_ALL	( VEC_X | VEC_Y | VEC_Z )
#define	MESHI_ALL	( MESHI_M | MESHI_N | MESHI_TOTAL | MESHI_MCLOSED | MESHI_NCLOSED | MESHI_MATERIAL | MESHI_SKIP )
#define	MATERIAL_ALL	( MAT_DIFFUSE | MAT_SPECULAR | MAT_AMBIENT )
#define BEZPOINT_ALL	( BEZ_POS | BEZ_INH | BEZ_OUTH | BEZ_LATINH | BEZ_LATOUTH )
#define	BEZLINE_ALL ( LINE_POS | LINE_INH | LINE_OUTH | LINE_LATINH | LINE_LATOUTH )
#define MATRIX_ALL ( MAT_CURRENT | MAT_LOCAL2WORLD | MAT_SHAPE | MAT_SHAPE0 | MAT_PART )

#define REVOLVED_ALL	( PLANES_LINE | PLANES_NORMAL | REV_FROM | REV_TO | REV_AXIS )
#define SPH_ALL	( PLANES_LINE | PLANES_NORMAL | SPH_DEFMAT )
#define DISK_ALL ( PLANES_LINE | PLANES_NORMAL | DISK_DEFMAT | DISK_FROM | DISK_TO | DISK_ISFILLED )
#define POLY_ALL ( PLANES_LINE | PLANES_NORMAL )



#define SPHERE_M	7
#define SPHERE_N	7


//shdhandler::SetSphereData
#define CALC_CENTER_FLAG 1
#define CALC_R_FLAG		2
#define CALC_WORLDV		4





#ifndef COEFHSTRING
//	extern char strskel[ SKEL_MAX ][ 30 ];
//	extern char strconvskel[ SKEL_MAX ][30];
	extern char strgtype[ G_MAX ][ 10 ];
	extern char strsymmtype[SYMMTYPE_MAX + 1][4];
	extern char strsymmtype2[SYMMTYPE_XM + 1][10];
	extern char mouseopestr[6][20];
	extern char itiobjname[ ITIOBJ_MAX ][ 30 ];

	//for bvhelem
	extern char chanelstr[ CHANEL_MAX ][ 20 ];
	extern char calcmodestr[CALCMODE_MAX + 1][20];

	//////resource file name
	//extern char s_bonemarkbmp[256];
	extern char s_bonemarkdds[256];
	extern char s_bonemarkdds2[256];
	extern char s_cam_kaku[256];
	extern char s_cam_i[256];
	extern char s_cam_kai[256];
	extern char s_defpalletrok[256];
	//extern char s_groundsig[256];
	extern char s_groundmqo[256];
	extern char s_selectmqo[256];
	extern char s_bonemarkmqo[256];
	extern char s_iktmqo[256];
	extern char s_sphmqo[256];
	extern char s_ei3mqo[256];
	extern char s_selvertbmp[256];
	extern char s_selvbmp[256];
	//extern char s_groundsig[256];

	extern char texrule_str[TEXRULE_MAX + 1][30];
	extern char exttexrep_str[EXTTEXREP_MAX + 1][30];

//	extern DWORD Bitno2Mask[32];

	extern char strmotiontype[MOTIONTYPEMAX + 1][20];

	extern char typechar[SHDTYPEMAX + 1][20];

	extern char subtypechar[JOINTTYPEMAX + 1][20];

	extern int chkhandler[3];

	extern __int64 chkvec[3];

		// chkmeshi は　cmpの都合上、int 型でないデータフラグは、最後に付け足す。
	extern __int64 chkmeshi[8];

	extern __int64 chkbez[5];
	extern __int64 chkbline[5];
	extern __int64 chkmaterial[3];
	extern __int64 chkrevolved[3];
	extern __int64 chkmatrix[4];
	extern __int64 chkdisk[6];

	extern int mceventmask[MCEVENTNUM];
	extern char strcommid[MCEVENTNUM][15];

#else

/***
	char strskel[ SKEL_MAX ][30] = {
		"TOPOFJOINT",
		"TORSO",
		"LEFT_HIP",
		"LEFT_KNEE",
		"LEFT_FOOT",
		"RIGHT_HIP",
		"RIGHT_KNEE",
		"RIGHT_FOOT",
		"NECK",
		"HEAD",
		"LEFT_SHOULDER",
		"LEFT_ELBOW",
		"LEFT_HAND",
		"RIGHT_SHOULDER",
		"RIGHT_ELBOW",
		"RIGHT_HAND"
	};

	char strconvskel[ SKEL_MAX ][30] = {
		"00TOP_OF_JOINT",
		"01SKEL_TORSO",
		"02SKEL_LEFT_HIP",
		"03SKEL_LEFT_KNEE",
		"04SKEL_LEFT_FOOT",
		"05SKEL_RIGHT_HIP",
		"06SKEL_RIGHT_KNEE",
		"07SKEL_RIGHT_FOOT",
		"08SKEL_NECK",
		"09SKEL_HEAD",
		"10SKEL_LEFT_SHOULDER",
		"11SKEL_LEFT_ELBOW",
		"12SKEL_LEFT_HAND",
		"13SKEL_RIGHT_SHOULDER",
		"14SKEL_RIGHT_ELBOW",
		"15SKEL_RIGHT_HAND"
	};
***/
	char strgtype[ G_MAX ][10] = {
		"target",
		"pos",
		"proj"
	};

	int mceventmask[MCEVENTNUM] =
	{
		1,
		2,
		4,
		8,
		16,

		32,
		64,
		128,
		256,
		512,

		1024,
		2048,
		4096,
		8192,
		16384,

		32768,
		65536,
		131072,
		262144,
		524288,

		1048576,
		2097152,
		4194304,
		8388608,
		16777216,

		33554432,
		67108864,
		134217728,
		268435456,
		536870912
	};

	char strcommid[MCEVENTNUM][15] = 
	{
		"1(0)",
		"2(1)",
		"4(2)",
		"8(3)",
		"16(4)",

		"32(5)",
		"64(6)",
		"128(7)",
		"256(8)",
		"512(9)",

		"1024(10)",
		"2048(11)",
		"4096(12)",
		"8192(13)",
		"16384(14)",

		"32768(15)",
		"65536(16)",
		"131072(17)",
		"262144(18)",
		"524288(19)",

		"1048576(20)",
		"2097152(21)",
		"4194304(22)",
		"8388608(23)",
		"16777216(24)",

		"33554432(25)",
		"67108864(26)",
		"134217728(27)",
		"268435456(28)",
		"536870912(29)"
	};


	char strsymmtype[SYMMTYPE_MAX + 1][4] =
	{
		"___",
		
		"_X+",
		"_X-",

		"_Y+",
		"_Y-",

		"_Z+",
		"_Z-",


		"_X",
		"_Y",
		"_Z",


		"___"
	};

	char strsymmtype2[SYMMTYPE_XM + 1][10] =
	{
		"___",
		
		"[L]_X+",
		"[R]_X-"
	};

	char mouseopestr[6][20] =
	{
		"None",
		"CameraRot",
		"CameraMove",
		"CameraDist",
		"BoneTwist",
		"max"
	};

	char itiobjname[ ITIOBJ_MAX ][ 30 ] = 
	{
		"頂点",
		"面",
		"パーツ",
		"ジョイント初期位置",
	};

	//for bvhelem
	char chanelstr[ CHANEL_MAX ][ 20 ] = 
	{
		"Xposition",
		"Yposition",
		"Zposition",
		"Zrotation",
		"Xrotation",
		"Yrotation"
	};

	char calcmodestr[CALCMODE_MAX + 1][20] = {
		"スキニング無し",
		"距離と角度",
		"距離のみ",
		"数値指定",
		"対称コピー",
		"ＭＡＸ"
	};

	//////resource file name
	//char s_bonemarkbmp[256] = "bonemark1.bmp";
	char s_bonemarkdds[256] = "bonemark1.dds";
	char s_bonemarkdds2[256] = "bonemark2.tga";
	
	char s_cam_kaku[256] = "cam_kaku.png";
	char s_cam_i[256] = "cam_i.png";
	char s_cam_kai[256] = "cam_kai.png";

	char s_defpalletrok[256] = "defpallet.rok";
	//char s_groundsig[256] = "groundplane1_p_small.sig";
	char s_groundmqo[256] = "ground2.mqo";
	char s_selectmqo[256] = "select.mqo";
	char s_bonemarkmqo[256] = "bonemark.mqo";
	char s_iktmqo[256] = "iktarget.mqo";
	char s_sphmqo[256] = "sphere0.mqo";
	char s_ei3mqo[256] = "ei3.mqo";
	char s_selvertbmp[256] = "selvert2b1.bmp";
	char s_selvbmp[256] = "selv.bmp";
	//char s_groundsig[256] = "groundplane1_p.sig";

	char texrule_str[TEXRULE_MAX + 1][30] = {
		"X軸平行投影",
		"Y軸平行投影",
		"Z軸平行投影",
		"円筒貼り付け",
		"球貼り付け",
		"メタセコイアでの指定",
		"ラップ",
		""
	};
	char exttexrep_str[EXTTEXREP_MAX + 1][30] = {
		"停止（アニメしない）",
		"最大値で停止（繰り返さない）",
		"最初に戻る（繰り返す）",
		"END TAG"
	};

/***
	DWORD Bitno2Mask[32] =
	{
		BITMASK_0,
		BITMASK_1,
		BITMASK_2,
		BITMASK_3,

		BITMASK_4,
		BITMASK_5,
		BITMASK_6,
		BITMASK_7,

		BITMASK_8,
		BITMASK_9,
		BITMASK_10,
		BITMASK_11,

		BITMASK_12,
		BITMASK_13,
		BITMASK_14,
		BITMASK_15,

		BITMASK_16,
		BITMASK_17,
		BITMASK_18,
		BITMASK_19,

		BITMASK_20,
		BITMASK_21,
		BITMASK_22,
		BITMASK_23,

		BITMASK_24,
		BITMASK_25,
		BITMASK_26,
		BITMASK_27,

		BITMASK_28,
		BITMASK_29,
		BITMASK_30,
		BITMASK_31
	};
***/

	char strmotiontype[MOTIONTYPEMAX + 1][20] = {
		"MOTIONTYPENONE",

		"MOTION_STOP", // motionno 固定。
		"MOTION_CLAMP", // motionno が　maxに達したら、そのままそこで固定。
		"MOTION_ROUND", //		最初に戻る
		"MOTION_INV",	//			逆方向に進む。
		//"MOTION_BIV", // 落ち着いたら、バイブレーションも追加。 

		"MOTIONTYPEMAX"
	};

	char typechar[SHDTYPEMAX + 1][20] = {
		"SHDTYPENONE",
		"SHDMESHINFO", 
		"SHDMATERIAL", 
		"SHDMATRIX",

		"SHDROOT", // depth の数(start, begin, end)とtreeのかずを合わせるためのダミー

		"SHDROTATE", // joint 変更時は　IsJoint() も変更
		"SHDSLIDER",
		"SHDSCALE",
		"SHDUNISCALE",
		"SHDPATH", // path joint から見て　sister の関係にある線形状に対応して、translateする
		
		"SHDMORPH",

		"SHDBALLJOINT",

		"SHDPART", // objを束ねる。自分自身は3dobjを持たない。
		"SHDPOLYLINE", // // CVecLine
		"SHDPOLYGON", // CPolygon
		"SHDSPHERE",	// CSphere
		"SHDBEZIERSURF", // CBezMesh
		"SHDMESHES", // CRevolved 
		"SHDDISK",	// CDisk
			
		"SHDEXTRUDE",

			// 以下は shadeのtypeじゃない
		"SHDLINE",		// (MESHの構成LINE)CVecLine, shadeのpolylineは開いているものだけのようなので、こだわるようなので一応別にしとく。
		"SHDMESH",
		"SHDBEZPOINT", // CBezData
		"SHDBEZLINE",	// CBezLine
		"SHDPLANES",

		"SHDPOLYMESH",

		"SHDPOLYMESH2",

		"SHDGROUNDDATA",

		"SHDBILLBOARD", 

		"SHDEXTLINE",

		"SHDDESTROYED",

		"SHDINFSCOPE",

		"SHDBBOX",

		"SHDTYPEERROR",
		"SHDTYPEMAX"
	};

	char subtypechar[JOINTTYPEMAX + 1][20] = {
		"JOINTTYPENONE", 
		"JOINTX", 
		"JOINTY", 
		"JOINTZ", 
		"JOINTUNI", 
		"JOINTCOMPLEX",
		"JOINTTYPEMAX"
	};

	int chkhandler[3] = {TREEHANDLER, SHDHANDLER, HANDLERMAX};

	__int64 chkvec[3] = { VEC_X, VEC_Y, VEC_Z };

		// chkmeshi は　cmpの都合上、int 型でないデータフラグは、最後に付け足す。
	__int64 chkmeshi[8] = { MESHI_M, MESHI_N, MESHI_TOTAL, MESHI_MCLOSED, MESHI_NCLOSED, MESHI_SKIP, MESHI_HASCOLOR, MESHI_MATERIAL };

	__int64 chkbez[5] = { BEZ_POS, BEZ_INH, BEZ_OUTH, BEZ_LATINH, BEZ_LATOUTH };
	__int64 chkbline[5] = { LINE_POS, LINE_INH, LINE_OUTH, LINE_LATINH, LINE_LATOUTH };
	__int64 chkmaterial[3] = { MAT_DIFFUSE, MAT_SPECULAR, MAT_AMBIENT };
	__int64 chkrevolved[3] = { REV_FROM, REV_TO, REV_AXIS };
	__int64 chkmatrix[4] = { MAT_CURRENT, MAT_LOCAL2WORLD, MAT_SHAPE, MAT_SHAPE0 };
	__int64 chkdisk[6] = { PLANES_LINE, PLANES_NORMAL, DISK_DEFMAT, DISK_FROM, DISK_TO, DISK_ISFILLED };

#endif

#endif