#ifndef FBXFILEH
#define FBXFILEH

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <coef.h>
#include <basedat.h>
#include <matrix2.h>


class CTreeHandler2;
class CShdHandler;
class CMotHandler;
class CShdElem;

class CPolyMesh;
class CPolyMesh2;

#include <fbxsdk.h>
#include <fbxfilesdk_nsuse.h>

class CFBXFile
{
public:
	CFBXFile();
	~CFBXFile();

	int LoadFBXFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult );
	int SetInfElem();
	int UnLoad();

private:
	void ConvertNurbsAndPatch(KFbxSdkManager* pSdkManager, KFbxScene* pScene);
	void ConvertNurbsAndPatchRecursive(KFbxSdkManager* pSdkManager, KFbxNode* pNode);

	int MakeRDB2Obj( KFbxSdkManager* pSdkManager, KFbxScene* pScene );
	void MakeRDB2ObjRec( KFbxSdkManager* pSdkManager, KFbxNode* pNode, int* errcnt );

	int AddShape2Tree( int shdtype, char* srcname );

	int MakePart( KFbxNode* pNode );
	int MakePolyMesh( KFbxNode* pNode );
	int MakeJoint( KFbxNode* pNode );

	void SetJointPos( KFbxScene* pScene );
	void SetJointPosRec( KFbxNode* pNode );

	KFbxXMatrix GetGlobalDefaultPosition(KFbxNode* pNode);

	int SetInfElem( KFbxScene* pScene );
	void SetInfElemRec( KFbxNode* pNode, int* errcnt );


private:
	CTreeHandler2* lpth;
	CShdHandler* lpsh;
	CMotHandler* lpmh;

	KFbxSdkManager* m_SdkManager;
	KFbxScene* m_Scene;


	///
	int curseri, befseri;
	int curdepth, befdepth;
	int curshdtype, befshdtype;
	CMeshInfo	tempinfo;

	int m_writeno;
	int m_offset;

	char* exttexlist[MAXEXTTEXNUM];
	char exttexbuf[ MAXEXTTEXNUM ][ 256 ];

	float m_datamult;

};


#endif