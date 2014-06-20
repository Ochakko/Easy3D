#ifndef CHARPOSH
#define	CHARPOSH

#include	"Coef.h"
//#include	<d3drmwin.h>
#include <D3DX9.h>

typedef struct grand_data {
	unsigned char	height;
	unsigned char	color;
	unsigned char	dirxz;
	unsigned char	diry;
	unsigned char	start;
	unsigned char	end;
	unsigned char	walkflag;// 進入禁止方向 全禁止 ： ９
} GRANDDATA;

typedef	struct	walk_data {
	float			height; // granddata.height * HSTEP
	unsigned char	walkflag; // granddata.walkflag
} WALKDATA;

typedef struct range_data {
	int		start;
	int		end;
	int		num;
} RANGEDATA;

typedef struct range_data2 {
	int		start0;
	int		end0;
	int		start1;
	int		end1;
} RANGEDATA2;

typedef struct tlvertexplus {
	D3DTLVERTEX	vert;
	int			ten;
	float		zvalue;
} TLVERTEXP;

typedef struct charsize {
	float	widthx;
	float	widthz;
	float	height;
} CHARSIZE;

typedef struct location {
	float	x;
	float	y;
	float	z;
} VEC3F, LOCATION, ROTATE;


typedef struct location2 {
	float	x;
	float	y;
	float	z;
	int		chkflag;
} LOCATION2;


typedef struct rgbdata {
	//unsigned char	r;
	//unsigned char	g;
	//unsigned char	b;

		// 大量に このデータを 作ることは ないと思うので
		// 計算時の range が 楽な int に しとく
		// が。。  高速化のため 0 ～ 500 以外は サポートしない ( calc.cpp )
	int		r;
	int		g;
	int		b;
} RGBDATA;

typedef struct rgb_data_f {
	float	rf;
	float	gf;
	float	bf;
} RGBDATA_F;

typedef struct indexl {
	int		x;
	int		y;
	int		z;
} INDEX_L;

typedef struct charindexl {
	char x;
	char y;
	char z;
} CHAR_INDEX_L;

typedef struct indexrect {
	INDEX_L	leftbottom;
	INDEX_L	righttop;
} INDEX_RECT;


typedef struct charpos {
	D3DVECTOR	loc;
	D3DVECTOR	dir;// xz平面での dir
	D3DVECTOR	updir; // 上方向を 示す dir
	ROTATE		rot;
	int			Move;
	int			dirindex;
	//int			patternno;
	INDEX_L		index;
} CHARPOS;

typedef struct obj_flag{
	unsigned char	flag;
	unsigned char	objno;
} OBJFLAG;

typedef struct inter_flag{
	int			interx; 
	int			interz;
	//for mychar
	int			interkick;
	int			interpush;
	int			interstop; // enemy 共用
	INDEX_L		interindex;// push 時の damyindex
	//for enemy
	//int			interenmpush[MAXENEMY + 1];// [enmno]
	//int			interenmsit[MAXENEMY + 1]; // set chairno + 1
	//INDEX_L		interenmindex[MAXENEMY + 1];
	//for chair
	BOOL		intervib;
	BOOL		interremove;
	unsigned char interyenobjno;
} INTERFLAG;

typedef struct _tlvdataheader
{
	char	filetype[TYPENAMELENG];// "GRANDDATA"
	DWORD	tlvwidth;
	DWORD	tlvheight;
	DWORD	tlvsize;

	DWORD	hdatawidth;
	DWORD	hdataheight;
	DWORD	hdatasize;
} TLVDATAHEADER;



typedef struct _chardataheader
{
	char	filetype[TYPENAMELENG]; // "CHARDATA"
	char	orgfile[TYPENAMELENG];  // 変換元　TLV.TXT FILE NAME
	int		tlvdim; // ディメンジョン
	int		tlvwidth; // point
	int		tlvheight; // plane
	DWORD	tlvnum;
	DWORD	trinum;
	D3DTLVERTEX	centervert;
	int		memnum[FANNUMMAX];	
} CHARDATAHEADER;

typedef struct _matrixdataheader
{
	char	filetype[TYPENAMELENG]; // "MATRIXDATA"
	int		ctrlnum;
	int		movenum;
	DWORD	datasize;
} MATRIXDATAHEADER;


// ３D空間での　バウンダリー
typedef struct bound3d {
	float	minx;
	float	maxx;
	float	miny;
	float	maxy;
	float	minz;
	float	maxz;
}BOUND3D;

// display座標での　バウンダリー
typedef struct bound2d {
	int		minx;
	int		maxx;
	int		miny;
	int		maxy;
}BOUND2D;


#endif