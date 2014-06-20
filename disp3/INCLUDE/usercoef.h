#ifndef USERCOEFH
#define USERCOEFH

#ifndef COEFH
#include <d3dx9.h>
#endif

#define BLURTIMEMAX	20
#define DISPSWITCHNUM	100
#define CAMERANUMMAX	3

#define MOELISTLENG	15
#define MAXEXTTEXNUM	100

#define PI                  3.14159265358979323846f
#define	PAI			3.14159265359
#define	PAIDIV2		1.570796326795
#define	PAIDIV4		0.7853981633974
#define	PAI2DEG		57.29577951308
#define	DEG2PAI		0.01745329251994
#define D2R			0.01745329251994

#define	DEGXZINITVAL	0.0f
#define DEGYINITVAL		0.0f
#define	EYEYINITVAL	750.0f
#define	CAMDISTINITVAL	2900.0f

enum {
	INDX,
	INDY,
	INDZ,
	INDMAX
};

typedef struct tag_spctrl
{
	float spt;
	float spdist;
	float distmax;
	D3DXVECTOR2 sppoint;
	float tmin;
	float tmax;
	int reserved[2];
}SPCTRL;

typedef struct tag_spparam
{
	SPCTRL ctrlS[INDMAX];
	SPCTRL ctrlE[INDMAX];
	int syncflag;
	int lockflag;
	int reserved[3];
}SPPARAM;

typedef struct tag_quaspparam
{
	SPPARAM m_rotparam;
	SPPARAM m_mvparam;
	SPPARAM m_scparam;
	int reserved[4];
} QUASPPARAM;


typedef struct tag_dvec3
{
	double x;
	double y;
	double z;
} DVEC3;

typedef struct tag_cameraelem
{
	int looktype;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;
	float dist;

	int hsid;
	int boneno;

	float nearz;
	float farz;
	float fov;
	
	int ortho;
	float orthosize;

}CAMERAELEM;

enum {
	CAMB_HEN,
	CAMB_0,
	CAMB_1,
	CAMB_2,
	CAMB_KAE,
	CAMB_MAX
};


enum {
	SKEL_TOPOFJOINT,
	SKEL_TORSO,
	SKEL_LEFT_HIP,
	SKEL_LEFT_KNEE,
	SKEL_LEFT_FOOT,
	SKEL_RIGHT_HIP,
	SKEL_RIGHT_KNEE,
	SKEL_RIGHT_FOOT,
	SKEL_NECK,
	SKEL_HEAD,
	SKEL_LEFT_SHOULDER,
	SKEL_LEFT_ELBOW,
	SKEL_LEFT_HAND,
	SKEL_RIGHT_SHOULDER,
	SKEL_RIGHT_ELBOW,
	SKEL_RIGHT_HAND,
	SKEL_MAX
};


static char strskel[ SKEL_MAX ][30] = {
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

static char strconvskel[ SKEL_MAX ][30] = {
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

typedef struct tag_tpos
{// -1����+1�̒l
	float ftop;
	float fleft;
	float fbottom;
	float fright;
}TPOS;

enum {
	CAPMODE_ALL,
	CAPMODE_ONE,
	CAPMODE_MAX
};
enum {
	POSMODE_TOPOFJOINT,
	POSMODE_GLOBAL,
	POSMODE_ZERO,
	POSMODE_MAX
};


typedef struct tag_rpselem
{
	int framecnt;
	int skelno;
	float confidence;
	D3DXVECTOR3 pos;
	int twistflag;
}RPSELEM;

typedef struct tag_tselem
{
	int skelno;
	char jointname[256];
	int jointno;
	int twistflag;
}TSELEM;


typedef struct tag_e3dssinfo
{
	char setname[32];
	int setid;	
}E3DSSINFO;

typedef struct tag_animtiming
{
	float haku;
	float sec;
	float frame;
}ANIMTIMING;

typedef struct tag_texchange
{
	int materialno;
	char texname[256];
}TEXCHANGE;

typedef struct tag_dschange
{
	int switchno;
	int state;
}DSCHANGE;


typedef struct tag_idchange
{
	int befid;
	int newid;
} IDCHANGE;

typedef struct tag_eulerlimit
{
	int ignoreflag;
	D3DXVECTOR3 lim0;
	D3DXVECTOR3 lim1;
} EULERLIMIT;

typedef struct tag_e3dq
{
	float x;
	float y;
	float z;
	float w;
} E3DQ;

typedef struct tag_za4elem
{
	int zatype;
	int rotaxis;
	E3DQ za4q;
}ZA4ELEM;

enum {
	ZA_1,
	ZA_2,
	ZA_3,
	ZA_4,
	ZA_MAX
};

enum {
	ROTAXIS_X,
	ROTAXIS_Y,
	ROTAXIS_Z,
	ROTAXIS_MAX
};


typedef struct tag_MOEELEM
{
	int listnum;
	int list[MOELISTLENG];
	int notlistnum;
	int notlist[MOELISTLENG];
	int eventno;
}MOEELEM;

typedef struct tag_e3dgpstate
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
	int	ongmode;
	float	rayy;
	float	rayleng;
	float	offsety;
	int interp;
}E3DGPSTATE;

enum {//GP ongmode
	GROUND_NONE,
	GROUND_ONE,
	GROUND_ON,
	GROUND_MAX
};

enum {//CAMERA looktype�̒l
	CAML_NORMAL,//�L���������Ȃ�
	CAML_LOOKONCE,//�L�[�̏u�Ԃ�������
	CAML_LOCK,//���̃L�[�܂Œ���
	CAML_MAX
};

typedef struct tag_e3dcamerastate
{
	int looktype;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;
	float dist;

	int hsid;
	int boneno;

	float nearz;
	float farz;
	float fov;
	int ortho;
	float orthosize;

	int interp;
}E3DCAMERASTATE;

enum {
	//hsp�p E3DGetMorphBaseInfo
	MBI_DISPNO,
	MBI_BONENO,
	MBI_TARGETNUM,
	MBI_MAX
};

typedef struct tag_e3dmorphbase
{
	char name[256];
	int dispno;
	int boneno;
	int targetnum;
}RDBMORPHBASE, E3DMORPHBASE;

typedef struct tag_e3dmorphtarget
{
	char name[256];
	int dispno;
}RDBMORPHTARGET, E3DMORPHTARGET;

typedef struct tag_e3dmorphmotion
{
	int frameno;
	float blendrate;
}RDBMORPHMOTION, E3DMORPHMOTION;


enum {
	BLUR_NONE,
	BLUR_WORLD,
	BLUR_CAMERA,
	BLUR_MAX
};

enum {
	GLOW_NONE,
	GLOW_NORMAL,
	GLOW_ALPHA,
	GLOW_MAX
};

typedef struct tag_e3dcolor4uc
{
//public:
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
//	inline tag_e3dcolor4uc( unsigned char srca, unsigned char srcr, unsigned char srcg, unsigned char srcb ){ 
//		a = srca; r = srcr; g = srcg; b = srcb; 
//	};
} E3DCOLOR4UC;

typedef struct tag_e3dcolor3uc
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} E3DCOLOR3UC;


typedef struct tag_e3dcolor4f
{
	float r;
	float g;
	float b;
	float a;
} E3DCOLOR4F;

typedef struct RDBColor3f
{
public:
	float r;
	float g;
	float b;
} RDBColor3f;

typedef struct tag_e3dtoon1material
{
	char name[32];

	RDBColor3f diffuse;
	RDBColor3f ambient;
	RDBColor3f specular;

	float darknl;//�@���ƃ��C�g�̓��ς�臒l
	float brightnl;

	RDBColor3f edgecol0;
	int edgevalid0;
	int edgeinv0;
	float edgewidth0;
	
}E3DTOON1MATERIAL;


typedef struct tag_bbinfo
{
	int bbid;
	D3DXVECTOR3 pos;
	char texname[256];
	int transparent;
	float width;
	float height;
} BBINFO;


enum {
	MPI2_MPID,
	MPI2_QUA,
	MPI2_TRA,
	MPI2_FRAMENO,
	MPI2_INTERP,
	MPI2_SCALE,
	MPI2_USERINT1,
	MPI2_MAX
};
typedef struct tag_MPINFO2
{
	int mpid;
	int qid;
	D3DXVECTOR3 tra;
	int frameno;
	int interpolation;
	D3DXVECTOR3 scale;
	int userint1;
} MPINFO2;


typedef struct tag_jointinfo
{
	char name[256];
	int serialno;
	int notuse;
	int parent;
	int child;
	int brother;
	int sister;
} JOINTINFO;


typedef struct tag_dispobjinfo
{
	char name[256];
	int serialno;
	int notuse;
	int dispswitch;
	int invisible;
} DISPOBJINFO;

typedef struct tag_fullscreenparams
{
	int validflag;
	int bits;
	SIZE scsize;
} FULLSCPARAMS;

typedef struct tag_trivno
{
	int vert1;
	int vert2;
	int vert3;
} TRIVNO;

typedef struct tag_bsphere
{
	D3DXVECTOR3 center;
	float r;
} BSPHERE;

typedef struct tag_uv
{
	float u;
	float v;
} UV;

typedef struct tag_textureinfo
{
	int texid;
	int width;
	int height;
	int pool;
	int format;
	int transparent;
	char name[256];//���g�p
} TEXTUREINFO;

//ShaderType
enum {
	COL_OLD,
	COL_PHONG,
	COL_BLINN,
	COL_SCHLICK,
	COL_MQCLASSIC,
	COL_TOON0,
	COL_TOON1,
	COL_PPHONG,
	COL_MAX
};

enum tag_calcmode
{
	CALCMODE_NOSKIN0,//�X�L�j���O�Ȃ�
	CALCMODE_ONESKIN0,//�����Ɠ��ς���v�Z
	CALCMODE_ONESKIN1,//��������v�Z
	CALCMODE_DIRECT0,//�l�̒��ڎw��
	CALCMODE_SYM,//�Ώ̐ݒ�
	CALCMODE_MAX
};

//E3DSetInfElem
enum {
	PAINT_NOR,//�m�[�}��
	PAINT_EXC,//�r��
	PAINT_ADD,//���Z
	PAINT_SUB,//���Z
	PAINT_ERA,//����
	PAINT_MAX
};

typedef struct tag_e3dinfelem
{
	int infno;
	int childjointno;//�e���{�[���A�q���̔ԍ��B�G���h�W���C���g������B
	int calcmode;//CALCMODE_*
	float userrate;//��
	float orginf;//CALCMODE_*�Ōv�Z�����l�B
	float dispinf;//�@orginf[] * userrate[]�Anormalizeflag���P�̂Ƃ��́A���K������B
} E3DINFELEM;

typedef struct tag_toon0params
{
	float darkrate;
	float brightrate;
	float darkdeg;
	float brightdeg;
} TOON0PARAMS;

typedef struct tag_moainfo
{
	char name[256];
	int id;
} MOAINFO;

typedef struct tag_moatrunkinfo
{
	int idling;
	int ev0idle;
	int comid;
	int notcomnum;
	int* notcomid;
	int branchnum;
} MOATRUNKINFO;

typedef struct tag_moabranchinfo
{
	int motid;
	int eventno;
	int frameno1;
	int frameno2;
	int notfu;
} MOABRANCHINFO;

enum {
	SHADOWIN_PROJ,		//������Ɉꕔ���ł������Ă�����V���h�E�}�b�v�ɓ����
	SHADOWIN_ALWAYS,	//��ɃV���h�E�}�b�v�ɓ����
	SHADOWIN_NOT,		//���䂵�Ȃ��i�n�ʂȂǂ̑傫���I�u�W�F�N�g�p�j
	SHADOWIN_EXCLUDE	//�V���h�E�}�b�v���珜�O����B
};

enum {
	BONETYPE_RDB2,
	BONETYPE_MIKO,
	BONETYPE_MAX
};

typedef struct tag_materialinfo
{
	int materialno;
	char name[256];
	float alpha;
	E3DCOLOR4UC diffuse;
	E3DCOLOR4UC specular;
	E3DCOLOR4UC ambient;
	E3DCOLOR4UC emissive;
	float power;
} MATERIALINFO;

typedef struct tag_toon1materialinfo
{
	char name[32];
	E3DCOLOR4UC ambient;
	E3DCOLOR4UC diffuse;
	E3DCOLOR4UC specular;
	int edgevalid;
	E3DCOLOR4UC edgecol0;
	float edgewidth;
} TOON1MATERIALINFO;


// E3DSetSymInfElem�Ŏg�p
enum {
	SYMAXIS_NONE,
	SYMAXIS_X,
	SYMAXIS_Y,
	SYMAXIS_Z,
	SYMAXIS_MAX
};

enum {
	QUATYPE_NUM,
	QUATYPE_NAME,
	QUATYPE_MAX
};

enum {
	TEXRULE_X,
	TEXRULE_Y,
	TEXRULE_Z,
	TEXRULE_CYLINDER, 
	TEXRULE_SPHERE,
	TEXRULE_MQ,
	TEXRULE_RAP,//������
	TEXRULE_MAX
};

enum {
	EXTTEXMODE_NORMAL,
	EXTTEXMODE_ANIM,
	EXTTEXMODE_MULTI,//������
	EXTTEXMODE_GLOW,
	EXTTEXMODE_GLOWALPHA,
	EXTTEXMODE_MAX
};

enum {
	TEXANIM_WRAP,
	TEXANIM_NOLIMIT,
	TEXANIM_MAX
};

typedef struct tag_materialtex
{
	char tex[256];
	char bump[256];
	
	int exttexnum;
	char exttex[MAXEXTTEXNUM][256];
	int exttexmode;//EXTTEXMODE_*

	int transparent;//0�s����,1������,2�e�N�X�`���A���t�@
	float uanim;
	float vanim;
	int texrule;//TEXRULE_*
	int texanimtype;//TEXANIM_*

	RDBColor3f glowmult;	
}MATERIALTEX;

typedef struct tag_materialtex1dim
{
	char tex[256];
	char bump[256];
	
	int exttexnum;
	char exttex[256 * MAXEXTTEXNUM];
	int exttexmode;//EXTTEXMODE_*

	int transparent;//0�s����,1������,2�e�N�X�`���A���t�@
	float uanim;
	float vanim;
	int texrule;//TEXRULE_*
	int texanimtype;//TEXANIM_*

	RDBColor3f glowmult;	
}MATERIALTEX1DIM;

typedef struct tag_isinfo
{
	int isid;
	int boneno;
	int dispno;
} ISINFO;

typedef struct tag_isvert
{
	int isid;
	int vertno;
	D3DXVECTOR3 pos;
} ISVERT;

#endif

