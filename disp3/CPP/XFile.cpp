#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <D3DX9.h>

#include <XFile.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

#include <coef.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <mothandler.h>
#include <treehandler2.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <d3ddisp.h>
#include <polymesh.h>
#include <polymesh2.h>
#include <mqomaterial.h>

#include "c:\pgfile9\rokdebone2DX\motdlg.h"
#include "c:\pgfile9\rokdebone2DX\motparamdlg.h"

#include "c:\pgfile9\rokdebone2DX\AllocateHierarchy.h"
#include "c:\pgfile9\rokdebone2DX\dxutil.h"

#include "c:\pgfile9\rokdebone2DX\SigEditWnd.h"


#define XFILELINELEN 4098
#define MAXKEYFRAMENUM 36001

/////////////

LPD3DXFRAME	g_pFrameRoot = NULL;
ID3DXAnimationController* g_pAnimController = NULL;


/// extern
extern CMotDlg* g_motdlg;
extern int g_toon1matcnt;


static int s_engmotno = 0;
static int s_topboneno = 0;

static char xfileheader1[XFILELINELEN] = "xof 0303txt 0032\r\n\
template AnimTicksPerSecond {\r\n\
 <9e415a43-7ba6-4a73-8743-b73d47e88476>\r\n\
 DWORD AnimTicksPerSecond;\r\n\
}\r\n\
\r\n\
template Frame {\r\n\
 <3d82ab46-62da-11cf-ab39-0020af71e433>\r\n\
 [...]\r\n\
}\r\n\
\r\n\
template Matrix4x4 {\r\n\
 <f6f23f45-7686-11cf-8f52-0040333594a3>\r\n\
 array FLOAT matrix[16];\r\n\
}\r\n\
\r\n\
template FrameTransformMatrix {\r\n\
 <f6f23f41-7686-11cf-8f52-0040333594a3>\r\n\
 Matrix4x4 frameMatrix;\r\n\
}\r\n\
\r\n\
template Vector {\r\n\
 <3d82ab5e-62da-11cf-ab39-0020af71e433>\r\n\
 FLOAT x;\r\n\
 FLOAT y;\r\n\
 FLOAT z;\r\n\
}\r\n\
\r\n\
template MeshFace {\r\n\
 <3d82ab5f-62da-11cf-ab39-0020af71e433>\r\n\
 DWORD nFaceVertexIndices;\r\n\
 array DWORD faceVertexIndices[nFaceVertexIndices];\r\n\
}\r\n\
\r\n\
template Mesh {\r\n\
 <3d82ab44-62da-11cf-ab39-0020af71e433>\r\n\
 DWORD nVertices;\r\n\
 array Vector vertices[nVertices];\r\n\
 DWORD nFaces;\r\n\
 array MeshFace faces[nFaces];\r\n\
 [...]\r\n\
}\r\n\
\r\n";

static char xfileheader2[XFILELINELEN] = "template MeshNormals {\r\n\
 <f6f23f43-7686-11cf-8f52-0040333594a3>\r\n\
 DWORD nNormals;\r\n\
 array Vector normals[nNormals];\r\n\
 DWORD nFaceNormals;\r\n\
 array MeshFace faceNormals[nFaceNormals];\r\n\
}\r\n\
\r\n\
template Coords2d {\r\n\
 <f6f23f44-7686-11cf-8f52-0040333594a3>\r\n\
 FLOAT u;\r\n\
 FLOAT v;\r\n\
}\r\n\
\r\n\
template MeshTextureCoords {\r\n\
 <f6f23f40-7686-11cf-8f52-0040333594a3>\r\n\
 DWORD nTextureCoords;\r\n\
 array Coords2d textureCoords[nTextureCoords];\r\n\
}\r\n\
\r\n\
template ColorRGBA {\r\n\
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>\r\n\
 FLOAT red;\r\n\
 FLOAT green;\r\n\
 FLOAT blue;\r\n\
 FLOAT alpha;\r\n\
}\r\n\
\r\n\
template IndexedColor {\r\n\
 <1630b820-7842-11cf-8f52-0040333594a3>\r\n\
 DWORD index;\r\n\
 ColorRGBA indexColor;\r\n\
}\r\n\
\r\n\
template MeshVertexColors {\r\n\
 <1630b821-7842-11cf-8f52-0040333594a3>\r\n\
 DWORD nVertexColors;\r\n\
 array IndexedColor vertexColors[nVertexColors];\r\n\
}\r\n\
\r\n\
template ColorRGB {\r\n\
 <d3e16e81-7835-11cf-8f52-0040333594a3>\r\n\
 FLOAT red;\r\n\
 FLOAT green;\r\n\
 FLOAT blue;\r\n\
}\r\n\
\r\n\
template Material {\r\n\
 <3d82ab4d-62da-11cf-ab39-0020af71e433>\r\n\
 ColorRGBA faceColor;\r\n\
 FLOAT power;\r\n\
 ColorRGB specularColor;\r\n\
 ColorRGB emissiveColor;\r\n\
 [...]\r\n\
}\r\n\
\r\n";

static char xfileheader3[XFILELINELEN] = "template MeshMaterialList {\r\n\
 <f6f23f42-7686-11cf-8f52-0040333594a3>\r\n\
 DWORD nMaterials;\r\n\
 DWORD nFaceIndexes;\r\n\
 array DWORD faceIndexes[nFaceIndexes];\r\n\
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]\r\n\
}\r\n\
\r\n\
template VertexDuplicationIndices {\r\n\
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>\r\n\
 DWORD nIndices;\r\n\
 DWORD nOriginalVertices;\r\n\
 array DWORD indices[nIndices];\r\n\
}\r\n\
\r\n\
template XSkinMeshHeader {\r\n\
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>\r\n\
 WORD nMaxSkinWeightsPerVertex;\r\n\
 WORD nMaxSkinWeightsPerFace;\r\n\
 WORD nBones;\r\n\
}\r\n\
\r\n\
template SkinWeights {\r\n\
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>\r\n\
 STRING transformNodeName;\r\n\
 DWORD nWeights;\r\n\
 array DWORD vertexIndices[nWeights];\r\n\
 array FLOAT weights[nWeights];\r\n\
 Matrix4x4 matrixOffset;\r\n\
}\r\n\
\r\n\
template RDB2ExtInfo1 {\r\n\
 <10b97487-12b6-41f9-896f-af119248bd3c>\r\n\
 DWORD StringNum;\r\n\
 array STRING InfoString[StringNum];\r\n\
}\r\n\
\r\n\
template TextureFilename {\r\n\
 <a42790e1-7810-11cf-8f52-0040333594a3>\r\n\
 STRING filename;\r\n\
}\r\n\
\r\n";

static char xfileheader4[XFILELINELEN] = "template Animation {\r\n\
 <3d82ab4f-62da-11cf-ab39-0020af71e433>\r\n\
 [...]\r\n\
}\r\n\
\r\n\
template AnimationSet {\r\n\
 <3d82ab50-62da-11cf-ab39-0020af71e433>\r\n\
 [Animation <3d82ab4f-62da-11cf-ab39-0020af71e433>]\r\n\
}\r\n\
\r\n\
template FloatKeys {\r\n\
 <10dd46a9-775b-11cf-8f52-0040333594a3>\r\n\
 DWORD nValues;\r\n\
 array FLOAT values[nValues];\r\n\
}\r\n\
\r\n\
template TimedFloatKeys {\r\n\
 <f406b180-7b3b-11cf-8f52-0040333594a3>\r\n\
 DWORD time;\r\n\
 FloatKeys tfkeys;\r\n\
}\r\n\
\r\n\
template AnimationKey {\r\n\
 <10dd46a8-775b-11cf-8f52-0040333594a3>\r\n\
 DWORD keyType;\r\n\
 DWORD nKeys;\r\n\
 array TimedFloatKeys keys[nKeys];\r\n\
}\r\n\
\r\n\
template RDB2TimedDSKeys {\r\n\
 <2f286f31-2b2c-4bbc-b75e-1623aaa4861e>\r\n\
 DWORD StringNum;\r\n\
 array STRING KeyString[StringNum];\r\n\
}\r\n\
\r\n\
template RDB2DSAnime {\r\n\
 <cf1812c0-060d-498f-8a23-4aca594ac560>\r\n\
 STRING AnimName;\r\n\
 DWORD nKeys;\r\n\
 array RDB2TimedDSKeys keys[nKeys];\r\n\
}\r\n\
\r\n\
\r\n\
AnimTicksPerSecond {\r\n\
 60;\r\n\
}\r\n";



CXFile::CXFile()
{
	m_thandler = 0;
	m_shandler = 0;
	m_mhandler = 0;
	
	m_mult = 1.0f;
	m_convface = 0;

	m_hfile = INVALID_HANDLE_VALUE;

	m_offset = 0;
	curseri = 0;
	int  ret;
	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	m_firstmaterialno = 1;

	m_sigeditwnd = 0;

	m_wnum = 0;
	m_ppni = 0;

	m_fullbone = 1;
}

CXFile::~CXFile()
{
	CloseFile();
}

int CXFile::CloseFile()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( m_hfile );
		SetEndOfFile( m_hfile );
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	return 0;
}

int CXFile::Write2File( int spnum, char* lpFormat, ... )
{
	if( !m_hfile ){
		//_ASSERT( 0 );
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[XFILELINELEN];
			
	ZeroMemory( outchar, XFILELINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, XFILELINELEN, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	char printchar[XFILELINELEN];
	ZeroMemory( printchar, XFILELINELEN );


	if( (spnum + 2 + (int)strlen( outchar )) >= XFILELINELEN ){
		DbgOut( "xfile : Write2File : write length too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int spno;
	for( spno = 0; spno < spnum; spno++ ){
		strcat_s( printchar, XFILELINELEN, " " );
	}
	strcat_s( printchar, XFILELINELEN, outchar );
	strcat_s( printchar, XFILELINELEN, "\r\n" );


	wleng = (unsigned long)strlen( printchar );
	WriteFile( m_hfile, printchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}

	return 0;
}

int CXFile::WriteXFile( int srcfullbone, CSigEditWnd* sewnd, int srcwnum, NAMEID** srcppni, char* filename, float mult, 
		int convface, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh )
{
	int ret = 0;

	m_thandler = lpth;
	m_shandler = lpsh;
	m_mhandler = lpmh;
	m_mult = mult;
	m_convface = convface;
	m_sigeditwnd = sewnd;
	m_wnum = srcwnum;
	m_ppni = srcppni;
	m_fullbone = srcfullbone;

	if( !m_thandler || !m_shandler || !m_mhandler || !m_sigeditwnd ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!
	}

///////
	m_hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( m_hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "XFile : WriteXFile : file open error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto wxfileexit;
	}

	
	ret = WriteXFile_aft();
	if( ret ){
		DbgOut( "XFile : WriteXFile : WriteXFile_aft error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto wxfileexit;
	}

	
	goto wxfileexit;
wxfileexit:
	CloseFile();

	return ret;
}

int CXFile::WriteXFile_aft()
{

	int ret;

	ret = m_thandler->ReplaceToEnglishName();
	if( ret ){
		DbgOut( "xfile : WriteXFile_aft : th ReplaceToEnglishName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	s_topboneno = 0;
	SetTopBoneNoReq( 0, &s_topboneno );
DbgOut( "xfile : topboneno %d\r\n", s_topboneno );

	CShdElem* topelem;
	topelem = (*m_shandler)( 0 );
	D3DXMATRIX inimat;
	D3DXMatrixIdentity( &inimat );
	m_shandler->CalcXTransformationMatrixReq( topelem, inimat, m_mult, m_mhandler->m_standard );


	Write2File( 0, xfileheader1 );
	Write2File( 0, xfileheader2 );
	Write2File( 0, xfileheader3 );
	Write2File( 0, xfileheader4 );

	int errcnt = 0;
	WriteShdElemReq( 0, topelem, &errcnt, 0 );
	if( errcnt != 0 ){
		DbgOut( "xfile : WriteXFile_aft : WriteShdElemReq error %d !!!\n", errcnt );
		_ASSERT( 0 );
		return 1;
	}

	//アニメーション
//	int motnum;
//	motnum = g_motdlg->GetMotionNum();
//	int motkind;
//	for( motkind = 0; motkind < motnum; motkind++ ){
//		ret = WriteAnimationSet( motkind );
//		if( ret ){
//			DbgOut( "xfile : WriteXFile_aft : WriteAnimationSet error !!!\n" );
//			_ASSERT( 0 );
//			return 1;
//		}
//	}
	int mno;
	for( mno = 0; mno < m_wnum; mno++ ){
		NAMEID* curni = *( m_ppni + mno );
		int motkind = curni->id;
		ret = WriteAnimationSet( motkind );
		if( ret ){
			DbgOut( "xfile : WriteXFile_aft : WriteAnimationSet error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

void CXFile::WriteShdElemReq( int spnum, CShdElem* curselem, int* errcnt, int revflag )
{
	int ret;
	CTreeElem2* telem;
	telem = (*m_thandler)( curselem->serialno );

//frame name
	if( revflag == 0 ){
		ret = Write2File( spnum, "Frame %s {\r\n\r\n", telem->engname );
		_ASSERT( !ret );
	}else{
		ret = Write2File( spnum, "Frame %s_Reverse {\r\n\r\n", telem->engname );
		_ASSERT( !ret );
	}


//transformation matrix
	ret = WriteTransformationMatrix( spnum + 1, curselem );
	_ASSERT( !ret );


//Mesh
	if( curselem->type == SHDPOLYMESH ){
		ret = WritePolyMesh( spnum + 1, curselem, revflag );
		if( ret ){
			DbgOut( "xfile : WriteShdElemReq : WritePolyMesh error !!!\n" );
			_ASSERT( 0 );
			(*errcnt)++;
			return;
		}
	}else if( curselem->type == SHDPOLYMESH2 ){
		ret = WritePolyMesh2( spnum + 1, curselem, revflag );
		if( ret ){
			DbgOut( "xfile : WriteShdElemReq : WritePolyMesh2 error !!!\n" );
			_ASSERT( 0 );
			(*errcnt)++;
			return;
		}
	}
	
/////////// 再帰
	if( (revflag == 0) && curselem->child ){

		CShdElem* curchil;
		curchil = m_shandler->GetValidChild( curselem->child );
		if( curchil ){
			WriteShdElemReq( spnum + 1, curchil, errcnt, revflag );
			
		}
	}

	Write2File( spnum, "}" );

	if( (curselem->clockwise == 3) && (revflag == 0) ){
		// 両面オブジェクトは、兄弟に追加
		WriteShdElemReq( spnum, curselem, errcnt, 1 );
	}


	if( (revflag == 0) && curselem->brother ){

		CShdElem* curbro;
		curbro = m_shandler->GetValidBrother( curselem->brother );
		if( curbro ){
			WriteShdElemReq( spnum, curbro, errcnt, revflag );
		}
	}

}

int CXFile::WriteTransformationMatrix( int spnum, CShdElem* curselem )
{
	int ret;
	ret = Write2File( spnum, "FrameTransformMatrix {" );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f;;",
		curselem->TransformationMatrix._11, curselem->TransformationMatrix._12, curselem->TransformationMatrix._13, curselem->TransformationMatrix._14,
		curselem->TransformationMatrix._21, curselem->TransformationMatrix._22, curselem->TransformationMatrix._23, curselem->TransformationMatrix._24,
		curselem->TransformationMatrix._31, curselem->TransformationMatrix._32, curselem->TransformationMatrix._33, curselem->TransformationMatrix._34,
		curselem->TransformationMatrix._41, curselem->TransformationMatrix._42, curselem->TransformationMatrix._43, curselem->TransformationMatrix._44		
		);
	_ASSERT( !ret );

	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return ret;
}


int CXFile::WritePolyMesh( int spnum, CShdElem* curselem, int revflag )
{
	int ret;

	if( revflag == 0 ){
		ret = Write2File( spnum, "Mesh %s_PM {", ((*m_thandler)( curselem->serialno ))->engname );
		_ASSERT( !ret );
	}else{
		ret = Write2File( spnum, "Mesh %s_PM_Reverse {", ((*m_thandler)( curselem->serialno ))->engname );
		_ASSERT( !ret );
	}

	CD3DDisp* d3ddisp;
	d3ddisp = curselem->d3ddisp;
	if( !d3ddisp ){
		DbgOut( "xfile : WritePolyMesh : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//position
	ret = WritePosition( spnum + 1, d3ddisp );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WritePosition error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//face
	CPolyMesh* pm;
	pm = curselem->polymesh;
	_ASSERT( pm );

	int reversenormal;
	int secindex_pm;
	int secindex_disp;
	secindex_pm = *( pm->indexbuf + 1 );
	secindex_disp = *( d3ddisp->m_dispIndices + 1 );
	if( secindex_pm == secindex_disp ){
		reversenormal = 0;
	}else{
		reversenormal = 1;
	}


	int indexcw;
	if( curselem->clockwise == 3 ){
		if( reversenormal == 0 ){
			if( revflag == 0 )
				indexcw = 2;
			else
				indexcw = 1;
		}else{
			if( revflag == 0 )
				indexcw = 1;
			else
				indexcw = 2;
		}
	}else{
		indexcw = 2;
	}


	ret = WriteFaceIndex( spnum + 1, d3ddisp, indexcw, pm, 0 );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteFaceIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( 0, "\r\n" );
	_ASSERT( !ret );

//normal
	ret = curselem->CalcInitialNormal();
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : selem CalcInitialNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteNormal( spnum + 1, curselem, indexcw, pm, 0 );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//UV
	ret = WriteUV( spnum + 1, d3ddisp );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//vertex color
	if( m_convface == 0 ){
		ret = WriteVertexColor( spnum + 1, d3ddisp, curselem->alpha );
		if( ret ){
			DbgOut( "xfile : WritePolyMesh ; WriteVertexColor error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//material list
	ret = WriteMaterialList( spnum + 1, curselem );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteMaterialList error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//duplication indices
	ret = WriteDuplicationIndices( spnum + 1, d3ddisp );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteDuplicationIndices error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//skinmesh header
	int infbonenum = 0;
	int* seri2infbno;

	seri2infbno = (int*)malloc( sizeof( int ) * m_shandler->s2shd_leng );
	if( !seri2infbno ){
		DbgOut( "xfile : WritePolyMesh : seri2infbno alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curselem->MakeXSeri2InfBno( m_shandler->s2shd_leng, seri2infbno, &infbonenum );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : MakeXSeri2InfBno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteSkinMeshHeader( spnum + 1, d3ddisp, infbonenum );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteSkinMeshHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//skin weights
	BONEINFLUENCE* boneinf;
	boneinf = (BONEINFLUENCE*)malloc( sizeof( BONEINFLUENCE ) * m_shandler->s2shd_leng );
	ZeroMemory( boneinf, sizeof( BONEINFLUENCE ) * m_shandler->s2shd_leng );

	ret = curselem->MakeXBoneInfluence( m_shandler->s2shd_leng, infbonenum, seri2infbno, boneinf );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : MakeXBoneInfluence error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteSkinWeights( spnum + 1, boneinf, infbonenum, curselem->serialno );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteSkinWeights error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// free
	free( seri2infbno );

	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		BONEINFLUENCE* curbi;
		curbi = boneinf + seri;

		if( curbi->vertices ){
			free( curbi->vertices );
		}
		if( curbi->weights ){
			free( curbi->weights );
		}
	}
	free( boneinf );

//RDB2ExtInfo1
	ret = WriteRDB2ExtInfo1( spnum + 1, curselem );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh : WriteRDB2ExtInfo1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//
	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return 0;
}
int CXFile::WritePolyMesh2( int spnum, CShdElem* curselem, int revflag )
{

	int ret;

	if( revflag == 0 ){
		ret = Write2File( spnum, "Mesh %s_PM2 {", ((*m_thandler)( curselem->serialno ))->engname );
		_ASSERT( !ret );
	}else{
		ret = Write2File( spnum, "Mesh %s_PM2_Reverse {", ((*m_thandler)( curselem->serialno ))->engname );
		_ASSERT( !ret );
	}

	CD3DDisp* d3ddisp;
	d3ddisp = curselem->d3ddisp;
	if( !d3ddisp ){
		DbgOut( "xfile : WritePolyMesh2 : d3ddisp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//position
	ret = WritePosition( spnum + 1, d3ddisp );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WritePosition error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//face
	int secindex_pm2;
	int secindex_disp;
	int reversenormal;

	CPolyMesh2* pm2;
	pm2 = curselem->polymesh2;
	_ASSERT( pm2 );

	secindex_pm2 = *( pm2->optindexbuf + 1 );
	secindex_disp = *( d3ddisp->m_dispIndices + 1 );
	if( secindex_pm2 == secindex_disp )
		reversenormal = 0;// polymeshと逆
	else
		reversenormal = 1;

	int indexcw;
	if( curselem->clockwise == 3 ){
		if( reversenormal == 0 ){
			if( revflag == 0 )
				indexcw = 2;
			else
				indexcw = 1;
		}else{
			if( revflag == 0 )
				indexcw = 1;
			else
				indexcw = 2;
		}
	}else{
		indexcw = 2;
	}


	ret = WriteFaceIndex( spnum + 1, d3ddisp, indexcw, 0, pm2 );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteFaceIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( 0, "\r\n" );
	_ASSERT( !ret );

//normal
	
	ret = curselem->CalcInitialNormal();
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : selem CalcInitialNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteNormal( spnum + 1, curselem, indexcw, 0, pm2 );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteNormal error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//UV
	ret = WriteUV( spnum + 1, d3ddisp );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteUV error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//vertex color
	if( m_convface == 0 ){
		ret = WriteVertexColor( spnum + 1, d3ddisp, curselem->alpha );
		if( ret ){
			DbgOut( "xfile : WritePolyMesh2 ; WriteVertexColor error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

//material list
	ret = WriteMaterialList( spnum + 1, curselem );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteMaterialList error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//duplication indices
	ret = WriteDuplicationIndices( spnum + 1, d3ddisp );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteDuplicationIndices error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//skinmesh header
	int infbonenum = 0;
	int* seri2infbno;

	seri2infbno = (int*)malloc( sizeof( int ) * m_shandler->s2shd_leng );
	if( !seri2infbno ){
		DbgOut( "xfile : WritePolyMesh2 : seri2infbno alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = curselem->MakeXSeri2InfBno( m_shandler->s2shd_leng, seri2infbno, &infbonenum );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : MakeXSeri2InfBno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteSkinMeshHeader( spnum + 1, d3ddisp, infbonenum );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteSkinMeshHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//skin weights
	BONEINFLUENCE* boneinf;
	boneinf = (BONEINFLUENCE*)malloc( sizeof( BONEINFLUENCE ) * m_shandler->s2shd_leng );
	ZeroMemory( boneinf, sizeof( BONEINFLUENCE ) * m_shandler->s2shd_leng );

	ret = curselem->MakeXBoneInfluence( m_shandler->s2shd_leng, infbonenum, seri2infbno, boneinf );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : MakeXBoneInfluence error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteSkinWeights( spnum + 1, boneinf, infbonenum, curselem->serialno );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteSkinWeights error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

// free
	free( seri2infbno );

	int seri;
	for( seri = 0; seri < m_shandler->s2shd_leng; seri++ ){
		BONEINFLUENCE* curbi;
		curbi = boneinf + seri;

		if( curbi->vertices ){
			free( curbi->vertices );
		}
		if( curbi->weights ){
			free( curbi->weights );
		}
	}
	free( boneinf );

//RDB2ExtInfo1
	ret = WriteRDB2ExtInfo1( spnum + 1, curselem );
	if( ret ){
		DbgOut( "xfile : WritePolyMesh2 : WriteRDB2ExtInfo1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



//
	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return 0;
}

int CXFile::WritePosition( int spnum, CD3DDisp* d3ddisp )
{
	int ret;
	int vertnum;
	vertnum = (int)d3ddisp->m_numTLV;

	ret = Write2File( spnum, "%d;", vertnum );
	_ASSERT( !ret );

	SKINVERTEX* curtlv;
	int vertno;
	for( vertno = 0; vertno < vertnum; vertno++ ){
		curtlv = d3ddisp->m_skinv + vertno;

		if( vertno != (vertnum - 1) ){
			ret = Write2File( spnum, "%f;%f;%f;,", curtlv->pos[0] * m_mult, curtlv->pos[1] * m_mult, curtlv->pos[2] * m_mult );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum, "%f;%f;%f;;", curtlv->pos[0] * m_mult, curtlv->pos[1] * m_mult, curtlv->pos[2] * m_mult );
			_ASSERT( !ret );
		}
	}

	return 0;
}

int CXFile::WriteFaceIndex( int spnum, CD3DDisp* d3ddisp, int indexcw, CPolyMesh* pm, CPolyMesh2* pm2 )
{
	int ret;
	int facenum;
	facenum = d3ddisp->m_vnum;

	int* windices = 0;
	/***
	if( pm ){
		windices = pm->m_optindexbuftex;
	}else if( pm2 ){
		windices = pm2->m_optindexbuftex;
	}else{
		_ASSERT( 0 );
		return 1;
	}
	***/
	if( pm ){
		windices = pm->indexbuf;
	}else if( pm2 ){
		windices = pm2->optindexbuf;
	}else{
		_ASSERT( 0 );
		return 1;
	}
	_ASSERT( windices );

	ret = Write2File( spnum, "%d;", facenum );
	_ASSERT( !ret );

	int i1, i2, i3;
	int faceno;
	for( faceno = 0; faceno < facenum; faceno++ ){
		if( indexcw == 2 ){
			i1 = *(windices + faceno * 3);
			i2 = *(windices + faceno * 3 + 1);
			i3 = *(windices + faceno * 3 + 2);
		}else if( indexcw == 1 ){
			i1 = *(windices + faceno * 3);
			i2 = *(windices + faceno * 3 + 2);
			i3 = *(windices + faceno * 3 + 1);
		}else{
			_ASSERT( 0 );
			i1 = *(windices + faceno * 3);
			i2 = *(windices + faceno * 3 + 1);
			i3 = *(windices + faceno * 3 + 2);
		}
		if( faceno != (facenum - 1) ){
			ret = Write2File( spnum, "3;%d,%d,%d;,", i1, i2, i3 );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum, "3;%d,%d,%d;;", i1, i2, i3 );
			_ASSERT( !ret );

		}

	}

	return 0;
}

int CXFile::WriteNormal( int spnum, CShdElem* curselem, int indexcw, CPolyMesh* pm, CPolyMesh2* pm2 )
{
	int ret;
	CD3DDisp* d3ddisp;
	d3ddisp = curselem->d3ddisp;
	if( !d3ddisp ){
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3* curnormal;
	if( indexcw == 2 ){
		curnormal = d3ddisp->m_orgNormal;
	}else{
		curnormal = d3ddisp->m_revNormal;
	}

	ret = Write2File( spnum, "MeshNormals {" );	
	_ASSERT( !ret );

	int normnum;
	normnum = (int)d3ddisp->m_numTLV;

	ret = Write2File( spnum + 1, "%d;", normnum );
	_ASSERT( !ret );

	D3DXVECTOR3* nptr;
	int normno;
	for( normno = 0; normno < normnum; normno++ ){
		nptr = curnormal + normno;
		
		if( normno != (normnum - 1) ){
			ret = Write2File( spnum + 1, "%f;%f;%f;,", nptr->x, nptr->y, nptr->z );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum + 1, "%f;%f;%f;;", nptr->x, nptr->y, nptr->z );
			_ASSERT( !ret );
		}
	}
	
	ret = WriteFaceIndex( spnum + 1, d3ddisp, indexcw, pm, pm2 );
	_ASSERT( !ret );


	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return 0;
}

int CXFile::WriteUV( int spnum, CD3DDisp* d3ddisp )
{
	int ret;

	ret = Write2File( spnum, "MeshTextureCoords {" );
	_ASSERT( !ret );

	int vertnum = (int)d3ddisp->m_numTLV;
	ret = Write2File( spnum + 1, "%d;", vertnum );
	_ASSERT( !ret );


	SKINVERTEX* curtlv;
	int vertno;
	for( vertno = 0; vertno < vertnum; vertno++ ){
		curtlv = d3ddisp->m_skinv + vertno;

		if( vertno != (vertnum - 1) ){
			ret = Write2File( spnum + 1, "%f;%f;,", curtlv->tex1[0], curtlv->tex1[1] );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum + 1, "%f;%f;;", curtlv->tex1[0], curtlv->tex1[1] );
			_ASSERT( !ret );
		}
	}


	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return 0;
}

int CXFile::WriteVertexColor( int spnum, CD3DDisp* d3ddisp, float alpha )
{
	int ret;

	ret = Write2File( spnum, "MeshVertexColors {" );
	_ASSERT( !ret );


	int vertnum;
	vertnum = (int)d3ddisp->m_numTLV;
	ret = Write2File( spnum + 1, "%d;", vertnum );
	_ASSERT( !ret );

	SKINMATERIAL* curmat;
	int vertno;
	for( vertno = 0; vertno < vertnum; vertno++ ){
		curmat = d3ddisp->m_smaterial + vertno;
		int r, g, b;
		r = (curmat->diffuse & 0x00FF0000) >> 16;
		g = (curmat->diffuse & 0x0000FF00) >> 8;
		b = curmat->diffuse & 0x000000FF;

		float fr, fg, fb;
		fr = (float)r / 255.0f;
		min( fr, 1.0f );
		max( fr, 0.0f );

		fg = (float)g / 255.0f;
		min( fg, 1.0f );
		max( fg, 0.0f );

		fb = (float)b / 255.0f;
		min( fb, 1.0f );
		max( fb, 0.0f );

		if( vertno != (vertnum - 1) ){
			ret = Write2File( spnum + 1, "%d;%f;%f;%f;%f;,", vertno, fr, fg, fb, alpha );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum + 1, "%d;%f;%f;%f;%f;;", vertno, fr, fg, fb, alpha );
			_ASSERT( !ret );
		}
	}

	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return 0;
}


int CXFile::WriteMaterialList( int spnum, CShdElem* curselem )
{
	int ret;

	ret = Write2File( spnum, "MeshMaterialList {" );
	_ASSERT( !ret );

	int facenum;
	facenum = curselem->d3ddisp->m_vnum;

	D3DXMATERIAL* matptr = 0;
	DWORD* attrptr = 0;
	int matnum = 0;

	if( curselem->type == SHDPOLYMESH ){
		CPolyMesh* pm;
		pm = curselem->polymesh;
		_ASSERT( pm );

		ret = pm->ConvColor2XMaterial( m_shandler->m_mathead, &matptr, &attrptr, &matnum );
		if( ret ){
			DbgOut( "xfile ; WriteMaterialList : pm ConvColor2XMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else if( curselem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2;
		pm2 = curselem->polymesh2;
		_ASSERT( pm2 );

		ret = pm2->ConvColor2XMaterial( m_shandler->m_mathead, &matptr, &attrptr, &matnum );
		if( ret ){
			DbgOut( "xfile ; WriteMaterialList : pm2 ConvColor2XMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

		
	ret = Write2File( spnum + 1, "%d;", matnum );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "%d;", facenum );
	_ASSERT( !ret );

	int faceno;
	for( faceno = 0; faceno < facenum; faceno++ ){
		if( faceno != (facenum - 1) ){
			ret = Write2File( spnum + 1, "%d,", *(attrptr + faceno) );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum + 1, "%d;", *(attrptr + faceno) );
			_ASSERT( !ret );
		}
	}


	int matno;
	D3DXMATERIAL* curmat; 
	for( matno = 0; matno < matnum; matno++ ){
		curmat = matptr + matno;

		ret = Write2File( spnum + 1, "Material {" );
		_ASSERT( !ret );
		ret = Write2File( spnum + 2, "%f;%f;%f;%f;;", curmat->MatD3D.Diffuse.r, curmat->MatD3D.Diffuse.g, curmat->MatD3D.Diffuse.b, curmat->MatD3D.Diffuse.a );
		_ASSERT( !ret );
		ret = Write2File( spnum + 2, "%f;", curmat->MatD3D.Power );
		_ASSERT( !ret );
		ret = Write2File( spnum + 2, "%f;%f;%f;;", curmat->MatD3D.Specular.r, curmat->MatD3D.Specular.g, curmat->MatD3D.Specular.b );
		_ASSERT( !ret );
		ret = Write2File( spnum + 2, "%f;%f;%f;;", curmat->MatD3D.Emissive.r, curmat->MatD3D.Emissive.g, curmat->MatD3D.Emissive.b );
		_ASSERT( !ret );


		if( curmat->pTextureFilename && *(curmat->pTextureFilename) ){
			ret = Write2File( spnum + 2, "TextureFilename {" );
			_ASSERT( !ret );
			ret = Write2File( spnum + 3, "\"%s\";", curmat->pTextureFilename );
			_ASSERT( !ret );
			ret = Write2File( spnum + 2, "}" );
			_ASSERT( !ret );

			ret = CheckMultiBytesChar( curmat->pTextureFilename );
			if( ret ){
				::MessageBox( NULL, "テクスチャファイル名に、日本語が含まれています。\n英語に直さないと正常に読み込めないXファイルになります。", "エラー", MB_OK );
			}
		}

		ret = Write2File( spnum + 1, "}\r\n" );
		_ASSERT( !ret );

	}

	// free !!!
	if( attrptr ){
		delete [] attrptr;
	}
	if( matptr ){
		for( matno = 0; matno < matnum; matno++ ){
			curmat = matptr + matno;
			if( curmat->pTextureFilename ){
				delete [] curmat->pTextureFilename;
			}
		}
		delete [] matptr;
	}
	

	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return 0;
}

int CXFile::WriteDuplicationIndices( int spnum, CD3DDisp* d3ddisp )
{
	int ret;

	ret = Write2File( spnum, "VertexDuplicationIndices {" );
	_ASSERT( !ret );

	int vertnum = (int)d3ddisp->m_numTLV;

	ret = Write2File( spnum + 1, "%d;", vertnum );
	_ASSERT( !ret );
	ret = Write2File( spnum + 1, "%d;", vertnum );
	_ASSERT( !ret );


	int vertno;
	for( vertno = 0; vertno < vertnum; vertno++ ){
		if( vertno != (vertnum - 1) ){
			ret = Write2File( spnum + 1, "%d,", vertno );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum + 1, "%d;", vertno );
			_ASSERT( !ret );
		}
	}

	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

	return 0;
}

int CXFile::WriteSkinMeshHeader( int spnum, CD3DDisp* d3ddisp, int bonenum )
{
	int ret;

	ret = Write2File( spnum, "XSkinMeshHeader {" );
	_ASSERT( !ret );


	int maxpervert = 0;
	int maxperface = 0;
	ret = d3ddisp->GetSkinMeshHeader( m_shandler->s2shd_leng, &maxpervert, &maxperface );
	if( ret ){
		DbgOut( "xfile : WriteSkinMeshHeader : d3ddisp GetSkinMeshHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = Write2File( spnum + 1, "%d;", maxpervert );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "%d;", maxperface );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "%d;", bonenum );
	_ASSERT( !ret );


	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );


	return 0;
}

int CXFile::WriteSkinWeights( int spnum, BONEINFLUENCE* boneinf, int infbonenum, int curseri )
{
	int ret;



/// １個目のweightは、不動のためのダミーデータ。
	ret = Write2File( spnum, "SkinWeights {" );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"%s\";", (*m_thandler)( s_topboneno )->engname );
	_ASSERT( !ret );
	ret = Write2File( spnum + 1, "0;" );
	_ASSERT( !ret );
	ret = Write2File( spnum + 1, ";" );
	_ASSERT( !ret );
	ret = Write2File( spnum + 1, ";" );
	_ASSERT( !ret );
	ret = Write2File( spnum + 1, "1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;" );
	_ASSERT( !ret );

	ret = Write2File( spnum, "}\r\n" );
	_ASSERT( !ret );

/// 
	int seri;
	for( seri = 1; seri < m_shandler->s2shd_leng; seri++ ){
		BONEINFLUENCE* curbi;
		curbi = boneinf + seri;

		if( (curbi->Bone >= 0) && (curbi->numInfluences > 0) ){
			ret = Write2File( spnum, "SkinWeights {" );
			_ASSERT( !ret );
			
			ret = Write2File( spnum + 1, "\"%s\";", (*m_thandler)( seri )->engname );
			_ASSERT( !ret );

			ret = Write2File( spnum + 1, "%d;", curbi->numInfluences );
			_ASSERT( !ret );

			int infno;
			for( infno = 0; infno < (int)curbi->numInfluences; infno++ ){
				if( infno != (curbi->numInfluences - 1) ){
					ret = Write2File( spnum + 1, "%d,", *(curbi->vertices + infno) );
					_ASSERT( !ret );
				}else{
					ret = Write2File( spnum + 1, "%d;", *(curbi->vertices + infno) );
					_ASSERT( !ret );
				}
			}

			for( infno = 0; infno < (int)curbi->numInfluences; infno++ ){
				if( infno != (curbi->numInfluences - 1) ){
					ret = Write2File( spnum + 1, "%f,", *(curbi->weights + infno) );
					_ASSERT( !ret );
				}else{
					ret = Write2File( spnum + 1, "%f;", *(curbi->weights + infno) );
					_ASSERT( !ret );
				}
			}

			if( m_mhandler->m_standard == 0 ){
				D3DXMATRIX offsetmat;
				ret = m_shandler->HuGetOffsetMatrix( seri, &offsetmat, m_mult );
				if( ret ){
					DbgOut( "xfile : WriteSkinWeights : sh HuGetOffsetMatrix error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}


				ret = Write2File( spnum + 1, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f;;",
					offsetmat._11, offsetmat._12, offsetmat._13, offsetmat._14,
					offsetmat._21, offsetmat._22, offsetmat._23, offsetmat._24,
					offsetmat._31, offsetmat._32, offsetmat._33, offsetmat._34,
					offsetmat._41, offsetmat._42, offsetmat._43, offsetmat._44
					);
				_ASSERT( !ret );
			}else{

				D3DXMATRIX inimat;
				D3DXMatrixIdentity( &inimat );
				CShdElem* wse = (*m_shandler)( seri );
				_ASSERT( wse );
				if( wse->IsJoint() ){
					D3DXVECTOR3 jpos( 0.0f, 0.0f, 0.0f );
					ret = m_shandler->GetBonePos( seri, 0, -1, 0, &jpos, m_mhandler, inimat, 1 );
					if( ret ){
						DbgOut( "xfile : WriteTransformMatrix : sh GetBonePos error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					ret = Write2File( spnum + 1, "1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,%f,%f,%f,1.0;;",
						-jpos.x, -jpos.y, -jpos.z
					);
					_ASSERT( !ret );
				}else{
					ret = Write2File( spnum + 1, "1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0;;" );
					_ASSERT( !ret );
				}
			}

			ret = Write2File( spnum, "}\r\n" );
			_ASSERT( !ret );

		}
	}

	return 0;
}

int CXFile::WriteAnimationSet( int motkind )
{
	int ret;
//	int motkind;
//	motkind = g_motdlg->GetMotCookie();
//	if( motkind < 0 ){
//		return 0;//!!!!!!!!
//	}

	int totaljointnum = 0;
	int samplejointno = -1;
	CShdElem* selem;
	int serino;
	for( serino = 0; serino < m_shandler->s2shd_leng; serino++ ){
		selem = (*m_shandler)( serino );

		if( selem->IsJoint() ){
			totaljointnum++;

			if( selem->child && (samplejointno == -1) ){
				samplejointno = serino;
			}
		}
	}
	totaljointnum += 1;//先頭フレームの分（boneno 0）
	
	if( samplejointno < 0 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!
	}

	//モーションポイントが存在するかをチェック
	CMotionCtrl* samplemc;
	samplemc = (*m_mhandler)( samplejointno );
//	CMotionPoint2* samplemp;
//	samplemp = samplemc->GetMotionPoint( motkind );
//	if( !samplemp ){
//		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	}

//////////////
	CMotionInfo* mi;
	mi = samplemc->motinfo;
	if( !mi ){
		_ASSERT( 0 );
		return 0;
	}
	if( !mi->motname ){
		_ASSERT( 0 );
		return 0;
	}
	char* nameptr;
	nameptr = *(mi->motname + motkind);


	char engmotname[256];
	ret = ConvName2English( nameptr, engmotname, 255 );
	if( ret ){
		DbgOut( "xfile : WriteAnimationSet : ConvName2English error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


//	ret = Write2File( 0, "AnimationSet %s {", nameptr );
		ret = Write2File( 0, "AnimationSet %s {", engmotname );
	_ASSERT( !ret );

//	int keyframe[MAXKEYFRAMENUM];
//	KEYFRAMENO keyframearray[MAXKEYFRAMENUM];
//	int keyframenum;
//
//	D3DXQUATERNION rotarray[MAXKEYFRAMENUM];
//	D3DXVECTOR3 scalearray[MAXKEYFRAMENUM];
//	D3DXVECTOR3 traarray[MAXKEYFRAMENUM];

	for( serino = 0; serino < m_shandler->s2shd_leng; serino++ ){

		int* keyframe = 0;
		KEYFRAMENO* keyframearray = 0;
		int keyframenum;

		D3DXQUATERNION* rotarray = 0;
		D3DXVECTOR3* scalearray = 0;
		D3DXVECTOR3* traarray = 0;


		CShdElem* curselem;
		curselem = (*m_shandler)( serino );
		if( curselem->IsJoint() && (curselem->type != SHDMORPH) ){
			CMotionCtrl* curmc;
			curmc = (*m_mhandler)( serino );
			int forbidflag;
			forbidflag = m_sigeditwnd->ForbidSelect( serino );
			if( (m_fullbone == 1) || (forbidflag == 0) ){
				keyframenum = 0;

				if( m_fullbone == 0 ){
					ret = m_mhandler->HuGetKeyframeNo( motkind, serino, 0, 0, &keyframenum );
					if( ret ){
						DbgOut( "xfile : WriteAnimationSet : mh HuGetKeyframeNo 0 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					keyframe = (int*)malloc( sizeof( int ) * (keyframenum + 1) );
					if( !keyframe ){
						DbgOut( "xfile : WriteAnimationSet : keyframe alloc  error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ZeroMemory( keyframe, sizeof( int ) * (keyframenum + 1) );

					ret = m_mhandler->HuGetKeyframeNo( motkind, serino, keyframe, keyframenum, &keyframenum );
					if( ret ){
						DbgOut( "xfile : WriteAnimationSet : mh HuGetKeyframeNo 1 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}else{
					ret = m_mhandler->GetKeyframeNo( motkind, serino, 0, 0, &keyframenum );
					if( ret ){
						DbgOut( "xfile : WriteAnimationSet : mh GetKeyframeNo 0 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					keyframe = (int*)malloc( sizeof( int ) * (keyframenum + 1) );
					if( !keyframe ){
						DbgOut( "xfile : WriteAnimationSet : keyframe alloc  error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ZeroMemory( keyframe, sizeof( int ) * (keyframenum + 1) );

					keyframearray = (KEYFRAMENO*)malloc( sizeof( KEYFRAMENO ) * (keyframenum + 1) );
					if( !keyframearray ){
						DbgOut( "xfile : WriteAnimationSet : keyframearray alloc  error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ZeroMemory( keyframearray, sizeof( KEYFRAMENO ) * (keyframenum + 1) );

					ret = m_mhandler->GetKeyframeNo( motkind, serino, keyframearray, keyframenum, &keyframenum );
					if( ret ){
						DbgOut( "xfile : WriteAnimationSet : mh GetKeyframeNo 1 error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					int kno0;
					for( kno0 = 0; kno0 < keyframenum; kno0++ ){
						KEYFRAMENO* curkar;
						curkar = keyframearray + kno0;
						*(keyframe + kno0) = curkar->frameno;
					}

				}


				//frame 0 には必ずキーフレームを作る。（これをしないとMeshViewerでモーションがおかしくなることがある。）
				if( (keyframenum == 0) || (*(keyframe + 0) != 0) ){
					int kno;
					for( kno = keyframenum - 1; kno >= 0; kno-- ){
						*(keyframe + kno + 1) = *(keyframe + kno);
					}
					*(keyframe + 0) = 0;
					keyframenum++;
				}

				scalearray = (D3DXVECTOR3*)malloc( sizeof( D3DXVECTOR3 ) * keyframenum );
				if( !scalearray ){
					DbgOut( "xfile : WriteAnimationSet : scalearray alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ZeroMemory( scalearray, sizeof( D3DXVECTOR3 ) * keyframenum );

				traarray = (D3DXVECTOR3*)malloc( sizeof( D3DXVECTOR3 ) * keyframenum );
				if( !traarray ){
					DbgOut( "xfile : WriteAnimationSet : traarray alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ZeroMemory( traarray, sizeof( D3DXVECTOR3 ) * keyframenum );
				
				rotarray = (D3DXQUATERNION*)malloc( sizeof( D3DXQUATERNION ) * keyframenum );
				if( !rotarray ){
					DbgOut( "xfile : WriteAnimationSet : rotarray alloc error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ZeroMemory( rotarray, sizeof( D3DXQUATERNION ) * keyframenum );

				if( keyframenum > 0 ){
					int keycnt;
					int keyframeno;
					//取得
					for( keycnt = 0; keycnt < keyframenum; keycnt++ ){
						keyframeno = *(keyframe + keycnt);

						ret = m_mhandler->HuGetKeyframeSRT( m_shandler, motkind, serino, keyframeno, 
							scalearray + keycnt, rotarray + keycnt, traarray + keycnt, m_mhandler->m_standard );
						if( ret ){
							DbgOut( "xfile : WriteAnimationSet : mh HuGetKeyframeSRT error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
					}

					//書き出し
					ret = Write2File( 1, "Animation {\r\n" );
					_ASSERT( !ret );

					ret = Write2File( 2, "AnimationKey {\r\n" );
					_ASSERT( !ret );
					ret = Write2File( 3, "0;" );// rot
					_ASSERT( !ret );
					ret = Write2File( 3, "%d;", keyframenum );
					for( keycnt = 0; keycnt < keyframenum; keycnt++ ){
						keyframeno = *(keyframe + keycnt);
						D3DXQUATERNION* currot;
						currot = rotarray + keycnt;
						if( keycnt != (keyframenum - 1) ){
							ret = Write2File( 3, "%d;4;%f,%f,%f,%f;;,", keyframeno, currot->w, currot->x, currot->y, currot->z );
							_ASSERT( !ret );
						}else{
							ret = Write2File( 3, "%d;4;%f,%f,%f,%f;;;", keyframeno, currot->w, currot->x, currot->y, currot->z );
							_ASSERT( !ret );
						}
					}
					ret = Write2File( 2, "}\r\n" );
					_ASSERT( !ret );


					ret = Write2File( 2, "AnimationKey {\r\n" );
					_ASSERT( !ret );
					ret = Write2File( 3, "1;" );// scale
					_ASSERT( !ret );
					ret = Write2File( 3, "%d;", keyframenum );
					for( keycnt = 0; keycnt < keyframenum; keycnt++ ){
						keyframeno = *(keyframe + keycnt);
						D3DXVECTOR3* curscale;
						curscale = scalearray + keycnt;
						if( keycnt != (keyframenum - 1) ){
							ret = Write2File( 3, "%d;3;%f,%f,%f;;,", keyframeno, curscale->x, curscale->y, curscale->z );
							_ASSERT( !ret );
						}else{
							ret = Write2File( 3, "%d;3;%f,%f,%f;;;", keyframeno, curscale->x, curscale->y, curscale->z );
							_ASSERT( !ret );
						}
					}
					ret = Write2File( 2, "}\r\n" );
					_ASSERT( !ret );


					ret = Write2File( 2, "AnimationKey {\r\n" );
					_ASSERT( !ret );
					ret = Write2File( 3, "2;" );// tra
					_ASSERT( !ret );
					ret = Write2File( 3, "%d;", keyframenum );
					for( keycnt = 0; keycnt < keyframenum; keycnt++ ){
						keyframeno = *(keyframe + keycnt);
						D3DXVECTOR3* curtra;
						curtra = traarray + keycnt;
						if( keycnt != (keyframenum - 1) ){
							ret = Write2File( 3, "%d;3;%f,%f,%f;;,", keyframeno, curtra->x * m_mult, curtra->y * m_mult, curtra->z * m_mult );
							_ASSERT( !ret );
						}else{
							ret = Write2File( 3, "%d;3;%f,%f,%f;;;", keyframeno, curtra->x * m_mult, curtra->y * m_mult, curtra->z * m_mult );
							_ASSERT( !ret );
						}
					}
					ret = Write2File( 2, "}\r\n" );
					_ASSERT( !ret );

					
					ret = Write2File( 2, "{ %s }", (*m_thandler)( serino )->engname );
					_ASSERT( !ret );


					ret = Write2File( 1, "}\r\n" );
					_ASSERT( !ret );

				}
			}
		}

		if( keyframe ){
			free( keyframe );
			keyframe = 0;
		}
		if( keyframearray ){
			free( keyframearray );
			keyframearray = 0;
		}
		if( rotarray ){
			free( rotarray );
			rotarray = 0;
		}
		if( scalearray ){
			free( scalearray );
			scalearray = 0;
		}
		if( traarray ){
			free( traarray );
			traarray = 0;
		}

	}

	ret = Write2File( 0, "}\r\n" );
	_ASSERT( !ret );


///// DSAnim
/***
	keyframenum = 0;
	ret = m_mhandler->HuGetKeyframeNo( motkind, samplejointno, keyframe, MAXKEYFRAMENUM, &keyframenum );
	if( ret ){
		DbgOut( "xfile : WriteAnimationSet : mh HuGetKeyframeNo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( keyframenum > 0 ){

		ret = Write2File( 0, "RDB2DSAnime {" );
		_ASSERT( !ret );

		//ret = Write2File( 1, "\"%s\";", g_motdlg->m_motparamdlg->m_motionname );
		ret = Write2File( 1, "\"%s\";", engmotname );
		_ASSERT( !ret );

		ret = Write2File( 1, "%d;", keyframenum );
		_ASSERT( !ret );


		int keycnt;
		int keyframeno;
		DWORD dispswitch;

		for( keycnt = 0; keycnt < keyframenum; keycnt++ ){
			keyframeno = keyframe[keycnt];
			ret =  m_mhandler->GetDispSwitch( m_shandler, motkind, keyframeno, &dispswitch );
			if( ret ){
				DbgOut( "xfile : WriteAnimationSet : GetDispSwitch error !!!\n" );
				_ASSERT( !ret );
				return 1;
			}

			if( keycnt != (keyframenum - 1) ){
				ret = Write2File( 1, "2;\"time=%d\",\"displayswitch=%d\";,", keyframeno, dispswitch );
				_ASSERT( !ret );
			}else{
				ret = Write2File( 1, "2;\"time=%d\",\"displayswitch=%d\";;", keyframeno, dispswitch );
				_ASSERT( !ret );
			}
		}

		ret = Write2File( 0, "}\r\n" );
		_ASSERT( !ret );
	}
***/

	return 0;
}

int CXFile::WriteRDB2ExtInfo1( int spnum, CShdElem* selem )
{
	int ret;

	ret = Write2File( spnum, "RDB2ExtInfo1 {" );
	_ASSERT( !ret );

	int stringnum;
	stringnum = 8 + selem->exttexnum;

	ret = Write2File( spnum + 1, "%d;", stringnum );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"dispswitchno=%d\",", selem->dispswitchno );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"transparent=%d\",", selem->transparent );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"uanime=%f\",", selem->uanime );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"vanime=%f\",", selem->vanime );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"exttexnum=%d\",", selem->exttexnum );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"exttexmode=%d\",", selem->exttexmode );
	_ASSERT( !ret );

	ret = Write2File( spnum + 1, "\"exttexrep=%d\",", selem->exttexrep );
	_ASSERT( !ret );

	if( stringnum != 8 ){
		ret = Write2File( spnum + 1, "\"exttexstep=%d\",", selem->exttexstep );
		_ASSERT( !ret );
	}else{
		ret = Write2File( spnum + 1, "\"exttexstep=%d\";", selem->exttexstep );
		_ASSERT( !ret );
	}

	int texno;
	for( texno = 0; texno < selem->exttexnum; texno++ ){
		if( texno != (selem->exttexnum - 1) ){
			ret = Write2File( spnum + 1, "\"exttexname=%s\",", *( selem->exttexname + texno ) );
			_ASSERT( !ret );
		}else{
			ret = Write2File( spnum + 1, "\"exttexname=%s\";", *( selem->exttexname + texno ) );
			_ASSERT( !ret );
		}

		ret = CheckMultiBytesChar( *(selem->exttexname + texno) );
		if( ret ){
			::MessageBox( NULL, "テクスチャ名(exttexname)に、日本語が含まれています。\n英語に直さないと正常に読み込めないXファイルになります。", "エラー", MB_OK );
		}

	}


	ret = Write2File( spnum, "}" );
	_ASSERT( !ret );

	return 0;
}
int CXFile::CheckMultiBytesChar( char* srcname )
{
	int leng;
	leng = (int)strlen( srcname );

	int no;
	unsigned char curuc;
	int findw = 0;
	for( no = 0; no < leng; no++ ){
		curuc = *(srcname + no);

		if( curuc >= 0x80 ){
			findw = 1;
			break;
		}
	}
	
	return findw;

}

int CXFile::LoadXFile( LPDIRECT3DDEVICE9 pdev, char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult )
{

	g_toon1matcnt = 0;//!!!!!!!!!!!!!!!!!!!!!!!!


	m_shandler = srclpsh;
	m_thandler = srclpth;
	m_mhandler = srclpmh;
	m_mult = datamult;
	m_offset = offset;	

	////////////
	int ret;
	HRESULT hr;
    CAllocateHierarchy Alloc;

	hr = D3DXLoadMeshHierarchyFromX( filename, D3DXMESH_MANAGED, pdev, &Alloc, NULL, &g_pFrameRoot, &g_pAnimController );
	if( hr != D3D_OK ){
		DbgOut( "xfile : LoadXFile : D3DXLoadMeshHierarchyFromX error %x!!!\n", hr );
		_ASSERT( 0 );
		return 1;
	}


	ret = m_shandler->InitMQOMaterial();
	if( ret ){
		DbgOut( "xfile : LoadXFile : sh InitMQOMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = MakeRDB2Obj();
	if( ret ){
		DbgOut( "xfile : LoadXFile : MakeRDB2Obj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//////////////

	SAFE_RELEASE( g_pAnimController );
    D3DXFrameDestroy( g_pFrameRoot, &Alloc );
	
	return 0;
}

int CXFile::MakeRDB2Obj()
{

	int ret;

	m_shandler->m_im2enableflag = 1;
	m_shandler->m_bonetype = BONETYPE_RDB2;
	m_shandler->m_mikoblendtype = MIKOBLEND_SKINNING;

	if( m_offset == 0 )
		curseri = 0;
	else
		curseri = m_offset - 1;


	ret = m_thandler->Start( m_offset );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//dispobjまとめのパーツ

	ret = m_thandler->Begin();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = AddShape2Tree( SHDPART, "パーツ" );
	if( ret ){
		DbgOut( "xfile : MakeRDB2Obj : AddShape2Tree error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->Init3DObj( curseri, &tempinfo );
	if( ret ){
		DbgOut( "xfile : MakeRDB2Obj Init3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int errcnt = 0;
	MakeRDB2ObjRec( g_pFrameRoot, &errcnt );
	if( errcnt > 0 ){
		DbgOut( "xfile : MakeRDB2Obj : MakeRDB2Obj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = m_thandler->End();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	// jointまとめのパーツ
	ret = m_thandler->Begin();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = AddShape2Tree( SHDPART, "Joint Part" );
	if( ret ){
		DbgOut( "xfile : MakeRDB2Obj : AddShape2Tree error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->Init3DObj( curseri, &tempinfo );
	if( ret ){
		DbgOut( "xfile : MakeRDB2Obj Init3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_thandler->End();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	

////////
//////////
	ret = m_shandler->SetChain( m_offset );
	if( ret ){
		DbgOut( "xfile : sh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_shandler->SetColors();
	if( ret ){
		DbgOut( "xfile : sh SetColors error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->SetChain( m_offset );
	if( ret ){
		DbgOut( "xfile : mh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//ret = lpsh->InfScopeConvTempApplyChild();
	//if( ret ){
	//	DbgOut( "sigfile : LoadSigFile : sh InfScopeConvTempApplyChild error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

/***
	ret = m_shandler->CalcDepth();
	if( ret ){
		DbgOut( "viewer : CreateBone : shandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = m_mhandler->CalcDepth();
	if( ret ){
		DbgOut( "viewer : CreateBone : mhandler CalcDepth error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
***/


	ret = m_shandler->SetMikoAnchorApply( m_thandler );
	if( ret ){
		DbgOut( "xfile : sh SetMikoAnchorApply error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = m_shandler->Anchor2InfScope( m_thandler, m_mhandler );
	if( ret ){
		DbgOut( "xfile : sh Anchor2InfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	return 0;

}

int CXFile::ConvContainer( D3DXMESHCONTAINER* pMeshCon )
{
	int ret;
	ret = m_thandler->Begin();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( pMeshCon && (pMeshCon->MeshData.Type == D3DXMESHTYPE_MESH) ){

		ret = AddShape2Tree( SHDPOLYMESH, pMeshCon->Name );
		if( ret ){
			DbgOut( "xfile : MakeRDB2ObjRec : AddShape2Tree error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = XMaterial2MQOMaterial( pMeshCon );
		if( ret ){
			DbgOut( "xfile : MakeRDB2ObjRec : XMaterial2MQOMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}



		ret = MakePolyMesh( pMeshCon );
		if( ret ){
			DbgOut( "xfile : MakeRDB2ObjRec : MakePolyMesh error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	ret = m_thandler->End();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}


void CXFile::MakeRDB2ObjRec( D3DXFRAME* pFrame, int* errcnt )
{
	int ret;
	D3DXMESHCONTAINER* pMeshCon;
	pMeshCon = pFrame->pMeshContainer;


	while( pMeshCon ){

		ret = ConvContainer( pMeshCon );
		if( ret ){
			DbgOut( "xfile : MakeRDB2ObjRec : ConvContainer error !!!\n" );
			_ASSERT( 0 );
			(*errcnt)++;
			return;
		}

		pMeshCon = pMeshCon->pNextMeshContainer;
	}

	///////////
	if( pFrame->pFrameFirstChild ){
DbgOut( "xfile : firstChild : %s\r\n", pFrame->pFrameFirstChild->Name );
		MakeRDB2ObjRec( pFrame->pFrameFirstChild, errcnt );
	}

	if( pFrame->pFrameSibling ){
DbgOut( "xfile : sibling : %s\r\n", pFrame->pFrameSibling->Name );
		MakeRDB2ObjRec( pFrame->pFrameSibling, errcnt );
	}

	return;
}

int CXFile::AddShape2Tree( int shdtype, char* srcname )
{

	DbgOut( "xfile : AddShape2Tree : %d, %s\r\n", shdtype, srcname );


	int ret;
	int curdepth;

	curseri++;//!!!!!!!!!!!!!

	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	ret = tempinfo.SetMem( shdtype, BASE_TYPE );
	if( ret ){
		DbgOut( "xfile : AddShape2Tree : tempinfo.SetMem : BASE_TYPE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	


	//chkdepth = m_thandler->GetCurDepth();
	//if( chkdepth != curdepth ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	curdepth = m_thandler->GetCurDepth();

	ret = m_thandler->AddTree( srcname, curseri );
	if( curseri != ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = m_thandler->SetElemType( curseri, shdtype );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	
// shdhandler
	ret = m_shandler->AddShdElem( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = m_shandler->SetClockwise( curseri, 2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//ret = m_shandler->SetBDivUV( curseri, sheader.bdivu, sheader.bdivv );
	//if( ret ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	//int cmp;
	//cmp = strcmp( sheader.texname, "0" );
	//if( cmp ){
	//	ret = m_shandler->SetTexName( curseri, sheader.texname );
	//	if( ret ){
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//}

	//ret = (*m_shandler)( curseri )->SetTexRepetition( sheader.texrepx, sheader.texrepy );
	//_ASSERT( !ret );

	//ret = (*m_shandler)( curseri )->SetTexTransparent( sheader.transparent );
	//_ASSERT( !ret );

	//ret = (*m_shandler)( curseri )->SetUVAnime( sheader.uanime, sheader.vanime );
	//_ASSERT( !ret );

	//ret = (*m_shandler)( curseri )->SetAlpha( sheader.alpha );
	//_ASSERT( !ret );

	ret = (*m_shandler)( curseri )->SetTexRule( TEXRULE_MQ );
	_ASSERT( !ret );

	//ret = (*m_shandler)( curseri )->SetNotUse( sheader.notuse );
	//_ASSERT( !ret );


	//if( sheader.flags & FLAGS_NOSKINNING ){
	//	ret = (*m_shandler)( curseri )->SetNoSkinning( 1 );
	//	_ASSERT( !ret );
	//}else{
		ret = (*m_shandler)( curseri )->SetNoSkinning( 0 );
		_ASSERT( !ret );
	//}

	//if( curshdtype != SHDINFSCOPE ){
	//	ret = (*m_shandler)( curseri )->SetDispSwitchNo( sheader.dispswitchno );
	//	_ASSERT( !ret );
	//}else{
	//	ret = (*m_shandler)( curseri )->SetDispSwitchNo( 0 );
	//	_ASSERT( !ret );
//
//		(*m_shandler)( curseri )->scopenum = 0;//!!!!!!!!!!!!!
//	}


//	if( sheader.flags & FLAGS_NOTRANSIK ){
//		(*m_shandler)( curseri )->m_notransik = 1;
//	}else{
//		(*m_shandler)( curseri )->m_notransik = 0;
//	}

//	if( sheader.flags & FLAGS_ANCHORFLAG ){
//		(*m_shandler)( curseri )->m_anchorflag = 1;
//	}else{
		(*m_shandler)( curseri )->m_anchorflag = 0;
//	}
//	(*m_shandler)( curseri )->m_mikodef = sheader.mikodef;


//	(*m_shandler)( curseri )->m_userint1 = sheader.userint1;//!!!!!!!!!!!!

//	if( sheader.iklevel > 0 ){
//		(*m_shandler)( curseri )->m_iklevel = sheader.iklevel;
//	}else{
		(*m_shandler)( curseri )->m_iklevel = 5;
//	}

//	if( sheader.facetm180 != 0.0f ){
//		(*m_shandler)( curseri )->facet = sheader.facetm180 + 180.0f;
//	}else{
		(*m_shandler)( curseri )->facet = 180.0f;
//	}

//	if( curshdtype == SHDINFSCOPE ){
//		(*m_shandler)( curseri )->alpha = 0.5f;
//	}


// mothandler
	ret = m_mhandler->AddMotionCtrl( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( shdtype == SHDBALLJOINT ){
		ret = m_mhandler->SetHasMotion( curseri, 1 );
		_ASSERT( !ret );
	}else{
		ret = m_mhandler->SetHasMotion( curseri, 0 );
		_ASSERT( !ret );
	}

	return 0;

}

int CXFile::MakePolyMesh( D3DXMESHCONTAINER* pMeshCon )
{
	int ret;
	HRESULT hr;
	LPD3DXMESH pMesh;
	pMesh = pMeshCon->MeshData.pMesh;
	DWORD vertnum;

	vertnum = pMesh->GetNumVertices();


	//init3dobj
	CVec3f tempvec;
	ret = tempvec.SetMem( 1.0f, 1.0f, 1.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = tempinfo.SetMem( &tempvec, MAT_DIFFUSE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = tempvec.SetMem( 0.25f, 0.25f, 0.25f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = tempinfo.SetMem( &tempvec, MAT_AMBIENT );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = tempvec.SetMem( 0.0f, 0.0f, 0.0f );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = tempinfo.SetMem( &tempvec, MAT_SPECULAR );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = tempinfo.SetMem( vertnum, MESHI_M );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 0, MESHI_N );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 0, MESHI_TOTAL );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 1, MESHI_MCLOSED );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 0, MESHI_NCLOSED );
	_ASSERT( !ret );
	ret = tempinfo.SetMem( 1, MESHI_HASCOLOR );
	_ASSERT( !ret );

	ret = m_shandler->Init3DObj( curseri, &tempinfo );
	if( ret ){
		DbgOut( "xfile : MakePolyMesh error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* curselem;
	curselem = (*m_shandler)( curseri );
	if( !curseri ){
		DbgOut( "xfile : MakePolyMesh : curselem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CPolyMesh* pm;
	pm = curselem->polymesh;
	if( !pm ){
		DbgOut( "xfile : MakePolyMesh : pm NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	pm->m_texorgflag = 1;//!!!!!!!!!!!!!!!!!!uvをいじらない

	ret = pm->CreateTextureBuffer();
	if( ret ){
		DbgOut( "xfile : MakePolyMesh : CreateTextureBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


////// vertex, uv

	
	DWORD vertFVF;
	vertFVF = pMesh->GetFVF();
	if( vertFVF == 0 ){
		DbgOut( "xfile : MakePolyMesh : FVF 0 not supported error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	

	DWORD bpv;
	bpv = pMesh->GetNumBytesPerVertex();


    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 pDeclCur;
    hr = pMesh->GetDeclaration(pDecl);
	if(FAILED(hr)){
		DbgOut( "xfile : MakePolyMesh : mesh GetDeclaration error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD texoffset = -1;
    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff){
		if( pDeclCur->Usage == D3DDECLUSAGE_TEXCOORD ){
			texoffset = pDeclCur->Offset;
			break;
		}
		pDeclCur++;
    }

	DWORD vertoffset = -1;
    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff){
		if( pDeclCur->Usage == D3DDECLUSAGE_POSITION ){
			vertoffset = pDeclCur->Offset;
			break;
		}
		pDeclCur++;
    }
	if( vertoffset == -1 ){
		DbgOut( "xfile : MakePolyMesh : vertoffset error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD vcoloffset = -1;
    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff){
		if( (pDeclCur->Usage == D3DDECLUSAGE_COLOR) && (pDeclCur->UsageIndex == 0) ){
			vcoloffset = pDeclCur->Offset;
			break;
		}
		pDeclCur++;
    }




	DWORD i;
	LPVOID pVB;
	hr = pMesh->LockVertexBuffer( D3DLOCK_READONLY, &pVB );
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : LockVertexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	for (i = 0; i < vertnum; i++){

		VEC3F* vertptr;
		vertptr = (VEC3F*)( (BYTE*)pVB + i * bpv + vertoffset );

		ret = curselem->SetPolyMeshPoint( i, vertptr->x * m_mult, vertptr->y * m_mult, vertptr->z * m_mult, i );
		if( ret ){
			DbgOut( "xfile : MakePolyMesh : SetPolyMeshPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( texoffset >= 0 ){
			COORDINATE* texptr;
			texptr = (COORDINATE*)( (BYTE*)pVB + i * bpv + texoffset );

			ret = pm->SetUV( i, texptr->u, texptr->v, 0, 0 );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : pm SetUV error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	
//	if( texoffset >= 0 ){
//		ret = pm->AdjustUVBuf();
//		_ASSERT( !ret );
//	}

	hr = pMesh->UnlockVertexBuffer();
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : UnlockVertexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////////////////
	/// face index

	DWORD facenum;
	facenum = pMesh->GetNumFaces();

	ret = curselem->CreatePolyMeshIndex( facenum );
	if( ret ){
		DbgOut( "xfile : MakePolyMesh : CreatePolyMeshIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	LPDIRECT3DINDEXBUFFER9 pIB;
	hr = pMesh->GetIndexBuffer( &pIB );
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : GetIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	D3DINDEXBUFFER_DESC ibdesc;
	hr = pIB->GetDesc( &ibdesc );
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : ib GetDesc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	D3DFORMAT ibformat;
	ibformat = ibdesc.Format;

	LPVOID pIndex;
	hr = pMesh->LockIndexBuffer( D3DLOCK_READONLY, &pIndex );
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : LockIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( ibformat == D3DFMT_INDEX16 ){
		for( i = 0; i < facenum; i++ ){
_ASSERT( 0 );
			WORD* indexptr;
			indexptr = (WORD*)( (BYTE*)pIndex + i * 3 * sizeof( WORD ) );

			int list[3];
			list[0] = *indexptr;
			list[1] = *(indexptr + 1);
			list[2] = *(indexptr + 2);

			ret = curselem->SetPolyMeshIndex( i, list );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : SetPolyMeshIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else if( ibformat == D3DFMT_INDEX32 ){
_ASSERT( 0 );
		for( i = 0; i < facenum; i++ ){			
			DWORD* indexptr;
			indexptr = (DWORD*)( (BYTE*)pIndex + i * 3 * sizeof( DWORD ) );

			int list[3];
			list[0] = (int)(*indexptr);
			list[1] = (int)(*(indexptr + 1));
			list[2] = (int)(*(indexptr + 2));

			ret = curselem->SetPolyMeshIndex( i, list );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : SetPolyMeshIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}else{
		DbgOut( "xfile : MakePolyMesh : ibformat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	hr = pMesh->UnlockIndexBuffer();
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : UnlockIndexBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	////////
	// material texture
	DWORD* pattrib;
	hr = pMesh->LockAttributeBuffer( D3DLOCK_READONLY, &pattrib );
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : LockAttributeBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXMATERIAL* firstxmat;
	firstxmat = pMeshCon->pMaterials + *pattrib;

	/***
	if( firstxmat ){

		float alpha;
		alpha = firstxmat->MatD3D.Diffuse.a;
		ret = curselem->SetAlpha( alpha );
		if( ret ){
			DbgOut( "xfile : MakePolyMesh : se SetAlpha error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( firstxmat->pTextureFilename && *(firstxmat->pTextureFilename) ){
DbgOut( "xfile : MakePolyMesh : SetTexture %s\r\n", firstxmat->pTextureFilename );
			ret = curselem->SetTexName( firstxmat->pTextureFilename );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : se SetTexName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}
	***/


	int* pmindex = pm->indexbuf;
	_ASSERT( pmindex );

	int materialnum;
	materialnum = pMeshCon->NumMaterials;


	for( i = 0; i < facenum; i++ ){
		DWORD curattrib;
		curattrib = *(pattrib + i);
		if( (curattrib < 0) || (curattrib >= (DWORD)materialnum) ){
			DbgOut( "xfile : MakePolyMesh : curattrib out of range error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		*( pm->m_attrib0 + i ) = m_firstmaterialno + curattrib;//!!!!!!!!!!!!!!!!!

		D3DXMATERIAL* curxmat;
		curxmat = pMeshCon->pMaterials + curattrib;

		int vno[3];
		vno[0] = *(pmindex + i * 3);
		vno[1] = *(pmindex + i * 3 + 1);
		vno[2] = *(pmindex + i * 3 + 2);


		int j;
		for( j = 0; j < 3; j++ ){
			ARGBF curargb;
			curargb.a = curxmat->MatD3D.Diffuse.a;
			curargb.r = curxmat->MatD3D.Diffuse.r;
			curargb.g = curxmat->MatD3D.Diffuse.g;
			curargb.b = curxmat->MatD3D.Diffuse.b;
			ret = pm->SetDiffuse( vno[j], 0, curargb, 0, TLMODE_ORG );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : diffuse pm SetDifffuse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			curargb.a = curxmat->MatD3D.Ambient.a;
			curargb.r = curxmat->MatD3D.Ambient.r;
			curargb.g = curxmat->MatD3D.Ambient.g;
			curargb.b = curxmat->MatD3D.Ambient.b;
			ret = pm->SetAmbient( vno[j], 0, curargb );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : pm SetAmbient error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			curargb.a = curxmat->MatD3D.Specular.a;
			curargb.r = curxmat->MatD3D.Specular.r;
			curargb.g = curxmat->MatD3D.Specular.g;
			curargb.b = curxmat->MatD3D.Specular.b;
			ret = pm->SetSpecular( vno[j], 0, curargb, 0, TLMODE_ORG );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : pm SetSpecular error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			curargb.a = curxmat->MatD3D.Emissive.a;
			curargb.r = curxmat->MatD3D.Emissive.r;
			curargb.g = curxmat->MatD3D.Emissive.g;
			curargb.b = curxmat->MatD3D.Emissive.b;
			ret = pm->SetEmissive( vno[j], 0, curargb, 0, TLMODE_ORG );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : pm SetEmissive error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm->SetSpecularPower( vno[j], 0, curxmat->MatD3D.Power, 0, TLMODE_ORG );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh : pm SetSpecularPower error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
				
	}


	hr = pMesh->UnlockAttributeBuffer();
	if( hr != D3D_OK ){
		DbgOut( "xfile : MakePolyMesh : UnlockAttributeBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	////////
	// vertex color
	if( (vcoloffset >= 0) && (vertFVF & D3DFVF_DIFFUSE) ){
		hr = pMesh->LockVertexBuffer( D3DLOCK_READONLY, &pVB );
		if( hr != D3D_OK ){
			DbgOut( "xfile : MakePolyMesh : LockVertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		for (i = 0; i < vertnum; i++){

			DWORD* colptr;
			colptr = (DWORD*)( (BYTE*)pVB + i * bpv + vcoloffset );

			ARGBF curargb;
			curargb.a = (float)((*colptr & 0xFF000000) >> 24) / 255.0f;
			curargb.r = (float)((*colptr & 0x00FF0000) >> 16) / 255.0f;
			curargb.g = (float)((*colptr & 0x0000FF00) >> 8) / 255.0f;
			curargb.b = (float)(*colptr & 0x000000FF) / 255.0f;
			ret = pm->SetDiffuse( i, 0, curargb, 0, TLMODE_ORG );
			if( ret ){
				DbgOut( "xfile : MakePolyMesh vertcol : pm SetDifffuse error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		
		hr = pMesh->UnlockVertexBuffer();
		if( hr != D3D_OK ){
			DbgOut( "xfile : MakePolyMesh : UnlockVertexBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	return 0;
}

int CXFile::ConvName2English( char* srcptr, char* dstptr, int dstleng )
{
	
	int textleng;
	textleng = (int)strlen( srcptr );
	if( (textleng <= 0) || (textleng > dstleng) ){
		sprintf_s( dstptr, dstleng + 1, "EnglishMotName%d", s_engmotno );
		s_engmotno++;

		return 0;//!!!!!!!!!!!!!!!!
	}


	char chkchar;
	int no;
	int finderr = 0;
	for( no = 0; no < textleng; no++ ){
		chkchar = *(srcptr + no);
		if( !((chkchar >= 'a') && (chkchar <= 'z')) && !((chkchar >= 'A') && (chkchar <= 'Z')) && 
			!((chkchar >= '0') && (chkchar <= '9')) && (chkchar != '_' ) ){
			finderr = 1;
			break;
		}
	}

	if( finderr ){
		sprintf_s( dstptr, dstleng + 1, "EnglishMotName%d", s_engmotno );
		s_engmotno++;
		DbgOut( "xfile : ConvName2English : find illeagal name : convert to English %s-->%s\n", srcptr, dstptr );
	}else{
		strcpy_s( dstptr, dstleng + 1, srcptr );
	}

	return 0;
}


int CXFile::XMaterial2MQOMaterial( D3DXMESHCONTAINER* pMeshCon )
{
	int matnum;
	matnum = pMeshCon->NumMaterials;

	CMQOMaterial* curmat;
	curmat = m_shandler->GetLastMaterial();
	if( !curmat ){
		DbgOut( "xfile : XMaterial2MQOMaterial curmat NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int matno;
	for( matno = 0; matno < matnum; matno++ ){
		D3DXMATERIAL* curxmat;
		curxmat = pMeshCon->pMaterials + matno;

		CMQOMaterial* newmat;
		newmat = new CMQOMaterial();
		if( !newmat ){
			DbgOut( "xfile : XMaterial2MQOMaterial : newmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		curmat->next = newmat;
		curmat = newmat;

		newmat->materialno = m_shandler->m_materialcnt - 1;
		if( matno == 0 ){
			m_firstmaterialno = newmat->materialno;//!!!!!!!!!!!!!!!!!!!!!!!!!!! pm->attrib0は　m_firstmaterialno + matno
		}
		(m_shandler->m_materialcnt)++;

		sprintf_s( newmat->name, 256, "%s_%d", pMeshCon->Name, newmat->materialno );
		
		newmat->col.a = curxmat->MatD3D.Diffuse.a;
		newmat->col.r = curxmat->MatD3D.Diffuse.r;
		newmat->col.g = curxmat->MatD3D.Diffuse.g;
		newmat->col.b = curxmat->MatD3D.Diffuse.b;
		
		newmat->dif4f.a = curxmat->MatD3D.Diffuse.a;
		newmat->dif4f.r = curxmat->MatD3D.Diffuse.r;
		newmat->dif4f.g = curxmat->MatD3D.Diffuse.g;
		newmat->dif4f.b = curxmat->MatD3D.Diffuse.b;
		newmat->dif = 1.0f;

		newmat->amb3f.r = curxmat->MatD3D.Ambient.r;
		newmat->amb3f.g = curxmat->MatD3D.Ambient.g;
		newmat->amb3f.b = curxmat->MatD3D.Ambient.b;
		if( newmat->col.r != 0.0f )
			newmat->amb = newmat->amb3f.r / newmat->col.r;
		else
			newmat->amb = newmat->amb3f.r;
		
		newmat->emi3f.r = curxmat->MatD3D.Emissive.r;
		newmat->emi3f.g = curxmat->MatD3D.Emissive.g;
		newmat->emi3f.b = curxmat->MatD3D.Emissive.b;
		if( newmat->col.r != 0.0f )
			newmat->emi = newmat->emi3f.r / newmat->col.r;
		else
			newmat->emi = newmat->emi3f.r;

		newmat->spc3f.r = curxmat->MatD3D.Specular.r;
		newmat->spc3f.g = curxmat->MatD3D.Specular.g;
		newmat->spc3f.b = curxmat->MatD3D.Specular.b;
		if( newmat->col.r != 0.0f )
			newmat->spc = newmat->spc3f.r / newmat->col.r;
		else
			newmat->spc = newmat->spc3f.r;

		newmat->power = curxmat->MatD3D.Power;

		if( curxmat->pTextureFilename && *(curxmat->pTextureFilename) ){
			strcpy_s( newmat->tex, 256, curxmat->pTextureFilename );
		}

	}

	return 0;
}

int CXFile::SetTopBoneNoReq( int curseri, int* topnoptr )
{
	CShdElem* selem;
	selem = (*m_shandler)( curseri );
	if( (selem->IsJoint()) && (*topnoptr <= 0) ){
		*topnoptr = curseri;
		return 0;
	}

	if( selem->child ){
		SetTopBoneNoReq( selem->child->serialno, topnoptr );
	}
	if( selem->brother ){
		SetTopBoneNoReq( selem->brother->serialno, topnoptr );
	}

	return 0;
}

