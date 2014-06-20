#ifndef XFILEH
#define XFILEH

#include <D3DX9.h>

#include <coef.h>
#include <basedat.h>

class CShdHandler;
class CMotHandler;
class CTreeHandler2;
class CShdElem;
class CD3DDisp;
class CPolyMesh;
class CPolyMesh2;
class CSigEditWnd;

class CXFile
{
public:
	CXFile();
	~CXFile();

	int WriteXFile( int srcfullbone, CSigEditWnd* sewnd, int srcwnum, NAMEID** srcppni, char* name, float mult, 
		int convface, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh );

	int LoadXFile( LPDIRECT3DDEVICE9 pdev, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult );

private:
	int WriteXFile_aft();

	int Write2File( int spnum, char* lpFormat, ... );
	int CloseFile();
	
	void WriteShdElemReq( int spnum, CShdElem* curselem, int* errcnt, int revflag );

	int WriteTransformationMatrix( int spnum, CShdElem* curselem );
	int WritePolyMesh( int spnum, CShdElem* curselem, int revflag );
	int WritePolyMesh2( int spnum, CShdElem* curselem, int revflag );

	int WritePosition( int spnum, CD3DDisp* d3ddisp );
	int WriteFaceIndex( int spnum, CD3DDisp* d3ddisp, int indexcw, CPolyMesh* pm, CPolyMesh2* pm2 );
	int WriteNormal( int spnum, CShdElem* curselem, int indexcw, CPolyMesh* pm, CPolyMesh2* pm2 );
	int WriteUV( int spnum, CD3DDisp* d3ddisp );
	int WriteVertexColor( int spnum, CD3DDisp* d3ddisp, float alpha );
	int WriteMaterialList( int spnum, CShdElem* curselem );
	int WriteDuplicationIndices( int spnum, CD3DDisp* d3ddisp );
	int WriteSkinMeshHeader( int spnum, CD3DDisp* d3ddisp, int bonenum );
	int WriteSkinWeights( int spnum, BONEINFLUENCE* boneinf, int infbonenum, int curseri );

	int WriteAnimationSet( int motkind );
	int WriteRDB2ExtInfo1( int spnum, CShdElem* selem );

	int CheckMultiBytesChar( char* srcname );

	///// load
	int MakeRDB2Obj();
	void MakeRDB2ObjRec( D3DXFRAME* pFrame, int* errcnt );
	int ConvContainer( D3DXMESHCONTAINER* pMeshCon );

	int MakePolyMesh( D3DXMESHCONTAINER* pMeshCon );

	int AddShape2Tree( int shdtype, char* srcname );

	int ConvName2English( char* srcptr, char* dstptr, int dstleng );

	int XMaterial2MQOMaterial( D3DXMESHCONTAINER* pMeshCon );
	int SetTopBoneNoReq( int curseri, int* topnoptr );

private:
	CTreeHandler2* m_thandler;
	CShdHandler* m_shandler;
	CMotHandler* m_mhandler;
	CSigEditWnd* m_sigeditwnd;

	float m_mult;
	int m_convface;

	HANDLE m_hfile;

	int m_offset;

	int curseri;
	CMeshInfo tempinfo;

	int m_firstmaterialno;

	int m_wnum;
	NAMEID** m_ppni;

	int m_fullbone;
};



#endif