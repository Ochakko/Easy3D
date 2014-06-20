#ifndef SHDDATAH
#define SHDDATAH

#include <d3drmwin.h>
#include <charpos.h>

#include <basedat.h>
#include <vecmesh.h>
#include <planes.h>

#include <motionctrl.h>


typedef struct loadopt
{
	unsigned int optnotrans;
	unsigned int optmatrix;
	unsigned int optmaterial;
	unsigned int optvertex;

	unsigned int optvline;
	unsigned int optbmesh;
	unsigned int optsphere;
	unsigned int optdisk;

} LOADOPT;


// 形状データの管理以外は、なるべく別クラスにする。

class CShdTree : public CBaseDat {
public:
	CShdTree();
	~CShdTree();

	void	InitParams();
	void	ResetParams();

// 表示用データ
		// セット
	int	Init3DObj( CMeshInfo* srcmeshinfo );	// 初期化用データ
	int CreateDispObj();
	int CheckDispData( int srctype ); // disp data の　allocate チェック
	
// 3dobj class の関数を呼び出すだけの関数群
	int SetDepth( int srcdepth );

	int SetMem( int* srcint, __int64 setflag );
	int SetMem( char* srcchar, __int64 setflag );
	
	int SetVecLine( int* srcint, __int64 setflag );
	int SetVecLine( char* srcchar, __int64 setflag );
	int SetVecLine( CVec3f* srcvec, int datano, __int64 setflag );

	int SetPolygon( int* srcint, __int64 setflag );
	int SetPolygon( char* srcchar, __int64 setflag );
	int SetPolygon( CVec3f* srcvec, int lineno, int vecno, __int64 setflag );
	int Add2CurPolygon( CMeshInfo* srclineinfo ); // lineの頂点数が確定するたびにallocのために呼ぶ。

	int SetSphere( int* srcint, __int64 setflag );
	int SetSphere( char* srcchar, __int64 setflag );
	int SetSphere( CMatrix2* srcmatrix, __int64 setflag );

	int SetBMesh( int* srcint, __int64 setflag );
	int SetBMesh( int srcint, int vno, __int64 setflag );
	int SetBMesh( char* srcchar, __int64 setflag );
	int SetBMesh( char* srcchar, int vno, __int64 setflag );
	int SetBMesh( CBezData* srcbez, int bezno, __int64 setflag );
	int SetBMesh( CVec3f* srcvec, int lineno, int vecno, __int64 setflag );

	int SetRevolved( int* srcint, __int64 setflag );
	int SetRevolved( char* srcchar, __int64 setflag );
	int SetRevolved( float* srcfloat, __int64 setflag );
	int SetRevolved( CVec3f* srcvec, int lineno, int vecno, __int64 setflag );
	int SetRevolved( CMatrix2* axis, __int64 setflag );

	int SetDisk( int* srcint, __int64 setflag );
	int SetDisk( char* srcchar, __int64 setflag );
	int SetDisk( float* srcfloat, __int64 setflag );
	int SetDisk( CMatrix2* srcmatrix, __int64 setflag );


	// 変換、加工
	/////////////////////////////////////////
	//v' = v * partmat * shapemat * local2world
	//		 (modeling時の変換)
	//					(ポーズの変換)
	//								(parentからの影響)
	/////////////////////////////////////////
	//BEZDATA*	DivBezMesh( BEZDATA* bezptr, MESHINFO* orgmi, int levelu, int levelv );
	//D3DVERTEX*	Bez2Poly( BEZDATA* bezptr );

	
// treeの操作
		// alloc, destroy
	CShdTree*	AddTree( int srcdepth, char* nameptr ); // beftree、curtreeのdepthを比較して、brotherまたはchildを作る
	CShdTree*	MakeBrother( char* broname );
	CShdTree*	MakeChild( char* childname );
	void	DeleteAllTree( int level );
		// set
	void	SetChild( int* isset, CShdTree* setchild ); // すでにchild がある場合には、内部で child->SetBrotherを呼び出す。
	void	SetBrother( int* isset, CShdTree* setbro );
	void DoSetSister( CShdTree* sisptr );
	void DoSetParent( CShdTree* parentptr );
		//get
	CShdTree	*GetParent();
	CShdTree	*GetChild();
	CShdTree	*GetBrother();
	CShdTree	*GetSister();

	CMotionCtrl*	GetMotionCtrl();


		// 検索、チェック
	CShdTree	*FindTree( char* findname, int srcdepth );
	int	IsSameTree( char* cmpname, int srcdepth ); 
	int IsDispObj( int dispflag );
	int IsJoint();

	int	DumpTree( char* filename, int dumpflag );
	int DumpMatrix( HANDLE hfile, CMatrix2& srcmat, char* header, int level, int dumpflag );


	/// 呼び出しが　うざいので　表引き。　でもセットもうざい
	void InitObjFunc();
	int (CShdTree::*CreateObjFunc[SHDTYPEMAX])( CMeshInfo* srcmeshinfo );
	int (CShdTree::*DestroyObjFunc[SHDTYPEMAX])(); // 初期化データ、dispデータ片方だけ使うことはまず無いので両方destroy

	int (CShdTree::*CreateDObjFunc[SHDTYPEMAX])();

private:

	int	CreateObjs();
	void	DestroyObjs();
	
	void	SetMotionChainReq( int* errcnt );
	void	DestroyMotionObj();


	void CreateDispObjReq( int* errcnt );// matrix演算後データを格納するobjを作る。
	int CreateCurDispObj();
	

	//int Destroy3DObj( int srctype );
	int DestroyVLine();
	int DestroyPolygon();
	int DestroySphere();
	int DestroyBMesh();
	int DestroyRevolved();
	int DestroyDisk();
	int DestroyDummy();

	//int Create3DObj( CMeshInfo* srcmeshinfo );
	int CreateVLine( CMeshInfo* srcmeshinfo );
	int CreatePolygon( CMeshInfo* srcmeshinfo );
	int CreateSphere( CMeshInfo* srcmeshinfo );
	int CreateBMesh( CMeshInfo* srcmeshinfo );
	int CreateRevolved( CMeshInfo* srcmeshinfo );
	int CreateDisk( CMeshInfo* srcmeshinfo );
	int CreateDummy( CMeshInfo* srcmeshinfo );

	//int CreateDObjFunc[i] 
	int CreateDVLine();
	int CreateDPolygon();
	int CreateDSphere();
	int CreateDBMesh();
	int CreateDRevolved();
	int CreateDDisk();
	int CreateDDummy();



	int GetKindNum( int objtype );
	CMeshInfo* AdjustMeshInfo( CMeshInfo* srcmeshinfo );
	CMeshInfo* CreateLineInfo( CMeshInfo* srcmeshinfo );

	// 同階層のものだけ検索
	void	FindBro( CShdTree** findtree, char* findname );
	
	void	FindReq( CShdTree** findtree, char* findname, int srcdepth );

		// DumpTree から　呼ぶ
	int	DumpTreeReq( HANDLE hfile, int level, int dumpflag );
	int DumpObj( HANDLE hfile, int level, int dumpflag );
	int DumpDObj( HANDLE hfile, int level, int dumpflag );

public:
	int	indexno; // 通し番号、生成順
	static int createno; // indexno に　入れる 
	int depth;

	CMotionCtrl* motctrl;

	// Init3DObj( meshinfo )で作成
		// １つのTreeに１種、１個だけ。
	CVecLine* vline; // SHDPOLYLINE
	CPolygon* polygon; // SHDPOLYGON
	CSphere* sphere; // SHDSPHERE
	CBezMesh* bmesh; // SHDBEZIERSURF
	CRevolved* revolved; // SHDMESHES
	CDisk* disk; // SHDDISK

		// for disp
	CVecLine* d_vline; // SHDPOLYLINE
	CPolygon* d_polygon; // SHDPOLYGON
	CSphere* d_sphere; // SHDSPHERE
	CBezMesh* d_bmesh; // SHDBEZIERSURF
	CRevolved* d_revolved; // SHDMESHES
	CDisk* d_disk; // SHDDISK

	CShdTree	*child;
	CShdTree	*brother;
	CShdTree	*sister;

private:
	// parent は　長男にのみセットする(データ構造上)ので、取得する場合には、必ず GetParent() を使う。
	CShdTree	*parent;


};

#endif

