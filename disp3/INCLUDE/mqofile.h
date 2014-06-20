#ifndef MQOFILEH
#define MQOFILEH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>

#define DBGH
#include <dbg.h>

#include <map>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;
//class CMQOMaterial;
class CMQOObject;
class CMQOFace;
class CBVHElem;

#define NAMELENG	1024

/***
typedef struct tag_mqobuf
{
	HANDLE hfile;
	unsigned char* buf;
	DWORD bufleng;
	DWORD pos;
	int isend;
} MQOBUF;
***/

typedef enum tag_mqostate {
	BEGIN_NONE,
	BEGIN_FINDCHUNK,
	BEGIN_TRIALNOISE,
	BEGIN_SCENE,
	BEGIN_BACKIMAGE,
	BEGIN_MATERIAL,
	BEGIN_OBJECT,//子チャンクをもつ
	BEGIN_VERTEX,
	BEGIN_VERTEXATTR,//子チャンクをもつ
	BEGIN_BVERTEX,//子チャンクをもつ
	BEGIN_VECTOR,
	BEGIN_WEIT,
	BEGIN_COLOR,
	BEGIN_FACE,
	BEGIN_BLOB,
	BEGIN_FINISH,
	BEGIN_UNKCHUNK,
	BEGIN_MAX
} MQOSTATE;

#define LINECHARLENG 2048

class CMQOFile
{
public:
	CMQOFile( HWND apphwnd );
	~CMQOFile();

	int LoadMQOFile( float multiple, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, 
		int offset, int groundflag, int bonetype, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot );
	int LoadMQOFileFromBuf( float multiple, char* bufptr, int bufsize, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, int groundflag, int bonetype, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot );


	int SaveMQOFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh );
	int SaveMQOFileOnFrame( int chkmorph, int motid, int frameno, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, float srcmult, int srcoutbone, HANDLE srchfile = INVALID_HANDLE_VALUE );

	int SaveMQOFileFromBVH( int srcjointnum, int srcbonenum, CBVHElem* srcbe, char* filename );


private:
/// for load

	int LoadMQOFile_aft( float multiple, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, int groundflag, int bonetype, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot );
	
	void InitLoadParams( HWND apphwnd );
	//int AddShape2Tree( SIGHEADER sh );
	int Init3DObj();

	int CheckFileVersion();
	int FindChunk( MQOSTATE* nextstate );
	int GetChunkType( MQOSTATE* type, char* chunkname, int nameleng );
	int SkipChunk();

	int CountJointNum( int* jointnumptr, int* bonenumptr, int* onlyjointnumptr );


	//int GetLine( char* dstchar, int dstlen, int* getlen );
	int GetLine( int* getlen );
	int GetBytes( unsigned char** dstuc, int getlen );
	int GrowUpMQOBuf();

	int ReadScene( MQOSTATE* nextstate );
	int ReadMaterial( MQOSTATE* nextstate );
	int ReadObject( MQOSTATE* nextstate );
	int ReadVertex( MQOSTATE* nextstate );
	int ReadFace( MQOSTATE* nextstate );
	int ReadBVertex( MQOSTATE* nextstate );
	int ReadColor( MQOSTATE* nextstate );

	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	CMQOMaterial* GetMaterialFromNo( CMQOMaterial* mathead, int matno );


/// for load
	int MakeTree();
	int start();
	int begin();
	int end();
	int AddShape2Tree();

	int make_first_part();
	int end_first_part();
		
	int do_transform_mat();
	int IsJoint();

	int make_polymesh2();
	int make_balljoints();
	int make_mikobone();
	void make_balljoints_from_mikoboneReq( CMQOObject* srcobj, CMQOFace* srcface, int* errorcnt );

	int make_extline();

	int finish();
	int Multiple();

	int SetMikoAnchorName();
	int ReplaceLRName();
	int ReplaceBdefSdef();
	int SetMikoAnchorApply();

//////////////////////
/// for save
	int WriteMQOHeader( HANDLE hfile );
	int CreateMaterial( int** matnoarray );
	int WriteMQOMaterial( HANDLE hfile, CMQOMaterial* mathead );

	int WriteMQOObject( HANDLE hfile, int** matnoarray );
	int WriteMQOObjectOnFrame( HANDLE hfile, int** matnoarray, int motid, int frameno, float srcmult, int chkmorph = 0 );
	int WriteMQOBoneOnFrame( HANDLE hfile, int motid, int frameno, float srcmult );
	void WriteMQOVertexFromBVHReq( HANDLE hfile, CBVHElem* srcbe, int* mqonoptr );
	void WriteMQOFaceFromBVHReq( HANDLE hfile, CBVHElem* srcbe );
	void WriteMQOFaceFromSelemReq( HANDLE hfile, CShdElem* srcse );


	int WriteObjectHeader( HANDLE hfile, char* nameptr, float srcfacet );
	int WriteLinechar( HANDLE hfile, int addreturn );

private:
	int m_objcnt;

	char m_linechar[ LINECHARLENG ];

	MQOSTATE m_state;
	HWND m_apphwnd;
	MQOBUF mqobuf;	

	CMQOMaterial** ppmathead;
	CMQOObject* objhead;
	CMQOObject* currentobj;

	int m_offset;
	int m_groundflag;
////
	int depth;
	CTreeHandler2* thandler;
	CShdHandler* shandler;
	CMotHandler* mhandler;
	CMeshInfo tempinfo;

	int cur_seri;// AddTree　時に、更新。

	int shdtype;
	//bool inmorph;
	//int morphdepth;
	//int morphno;
	//int cur_pm_face;
	//int pm_vertnum;
	//int pm_facenum;
	int totalpoly;
	char tempname[NAMELENG + 1];

	float m_multiple;

	//int m_writeno;
	int m_includeextline;

	int m_bonetype;
	int m_adjustuvflag;

	//D3DXVECTOR3 m_offsetpos;

	D3DXMATRIX m_offsetmat;

	int m_legacymatflag;
	CMQOMaterial* m_tempmathead;
	int m_materialoffset;
	std::map<int, int> m_newmatindex;

};


#endif
