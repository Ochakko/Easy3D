#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>
#include <InfScope.h>
#include <InfElem.h>
#include <Toon1Params.h>

#include <crtdbg.h>

#include <FBXFile.h>


#define DBGH
#include <dbg.h>

#include "c:\pgfile9\rokdebone2\motdlg.h"
#include "c:\pgfile9\rokdebone2\motparamdlg.h"

//#include <fbxsdk.h>
#include <Common.h>
//#include <fbxfilesdk_nsuse.h>

// global
extern int g_toon1matcnt;


CFBXFile::CFBXFile()
{
	lpth = 0;
	lpsh = 0;
	lpmh = 0;

	m_SdkManager = NULL;
	m_Scene = NULL;

	int  ret;
	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	curseri = 0;
	befseri = 0;
	curdepth = 0;
	befdepth = 0;
	curshdtype = 0;
	befshdtype = 0;

	m_datamult = 1.0f;
}

CFBXFile::~CFBXFile()
{
}


int CFBXFile::LoadFBXFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult )
{

	g_toon1matcnt = 0;//!!!!!!!!!!!!!!


	if( !filename || !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "fbx : LoadFBXFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;
	m_offset = offset;
	m_datamult = datamult;
//////////

	KFbxSdkManager* lSdkManager = NULL;
	KFbxScene* lScene = NULL;
	bool lResult;
 
	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	m_SdkManager = lSdkManager;
	m_Scene = lScene;


	// Load the scene.

	// The example can take a FBX file as an argument.
	lResult = LoadScene(lSdkManager, lScene, filename);
	if(lResult == false)
	{
		DbgOut( "FBX : LoadFBXFile : LoadScene error !!! %s\n", filename );
		_ASSERT( 0 );
		DestroySdkObjects(lSdkManager);
		return 1;
	}

	ConvertNurbsAndPatch(lSdkManager, lScene);
	
/////////
	int ret;
	ret = MakeRDB2Obj( lSdkManager, lScene );
	if( ret ){
		DbgOut( "fbx : LoadFBXFile : MakeRDB2Obj error !!!\n" );
		_ASSERT( 0 );
		DestroySdkObjects(lSdkManager);
		return 1;
	}

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager);


	return 0;
}

void CFBXFile::ConvertNurbsAndPatch(KFbxSdkManager* pSdkManager, KFbxScene* pScene)
{
    ConvertNurbsAndPatchRecursive(pSdkManager, pScene->GetRootNode());
}


void CFBXFile::ConvertNurbsAndPatchRecursive(KFbxSdkManager* pSdkManager, KFbxNode* pNode)
{
    KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

    if (lNodeAttribute)
    {
        if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eNURB ||
            lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::ePATCH)
        {
            KFbxGeometryConverter* lConverter = pSdkManager->CreateKFbxGeometryConverter();

            lConverter->TriangulateInPlace(pNode);

            pSdkManager->DestroyKFbxGeometryConverter(lConverter);
        }
    }

    int i, lCount = pNode->GetChildCount();

    for (i = 0; i < lCount; i++)
    {
        ConvertNurbsAndPatchRecursive(pSdkManager, pNode->GetChild(i));
    }
}

int CFBXFile::MakeRDB2Obj( KFbxSdkManager* pSdkManager, KFbxScene* pScene )
{
	int ret;

	lpsh->m_im2enableflag = 1;
	lpsh->m_bonetype = BONETYPE_RDB2;
	lpsh->m_mikoblendtype = MIKOBLEND_SKINNING;

	if( m_offset == 0 )
		curseri = 0;
	else
		curseri = m_offset - 1;


	ret = lpth->Start( m_offset );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

/////////
	int errcnt = 0;
	MakeRDB2ObjRec( pSdkManager, pScene->GetRootNode(), &errcnt );
	if( errcnt > 0 ){
		DbgOut( "fbx : MakeRDB2Obj : MakeRDB2ObjRec error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////
	ret = lpsh->SetChain( m_offset );
	if( ret ){
		DbgOut( "fbx : sh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = lpsh->SetColors();
	if( ret ){
		DbgOut( "fbx : sh SetColors error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = lpmh->SetChain( m_offset );
	if( ret ){
		DbgOut( "fbx : mh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//ret = lpsh->InfScopeConvTempApplyChild();
	//if( ret ){
	//	DbgOut( "sigfile : LoadSigFile : sh InfScopeConvTempApplyChild error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}


	ret = lpsh->SetMikoAnchorApply( lpth );
	if( ret ){
		DbgOut( "fbx : sh SetMikoAnchorApply error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->Anchor2InfScope( lpth, lpmh );
	if( ret ){
		DbgOut( "fbx : sh Anchor2InfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////////////
	SetJointPos( pScene );

	SetInfElem( pScene );


	return 0;
}


void CFBXFile::MakeRDB2ObjRec( KFbxSdkManager* pSdkManager, KFbxNode* pNode, int* errcnt )
{
	KString lString;
	int ret, i;

	lString = pNode->GetName();
	DbgOut( "fbx : MakeRDB2ObjRec : name %s, %d\r\n", lString.Buffer(), pNode->GetChildCount() );

	
	ret = lpth->Begin();
	if( ret ){
		_ASSERT( 0 );
		(*errcnt)++;
		return;
	}


	KFbxNodeAttribute::EAttributeType lAttributeType;
	if(pNode->GetNodeAttribute()){
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
			case KFbxNodeAttribute::eSKELETON:
				ret = AddShape2Tree( SHDBALLJOINT, lString.Buffer() );
				if( ret ){
					DbgOut( "fbx : MakeRDB2ObjRec : AddShape2Tree joint error !!!\n" );
					_ASSERT( 0 );
					(*errcnt)++;
					return;
				}
				ret = MakeJoint( pNode );
				if( ret ){
					DbgOut( "fbx : MakeRDB2ObjRec : MakeJoint error !!!\n" );
					_ASSERT( 0 );
					(*errcnt)++;
					return;
				}

				break;
	
			case KFbxNodeAttribute::eMESH:
				ret = AddShape2Tree( SHDPOLYMESH, lString.Buffer() );
				if( ret ){
					DbgOut( "fbx : MakeRDB2ObjRec : AddShape2Tree pm2 error !!!\n" );
					_ASSERT( 0 );
					(*errcnt)++;
					return;
				}
				ret = MakePolyMesh( pNode );
				if( ret ){
					DbgOut( "fbx : MakeRDB2ObjRec : MakePolyMesh error !!!\n" );
					_ASSERT( 0 );
					(*errcnt)++;
					return;
				}
				break;

			default:
				ret = AddShape2Tree( SHDPART, lString.Buffer() );
				if( ret ){
					DbgOut( "fbx : MakeRDB2ObjRec : AddShape2Tree part error !!!\n" );
					_ASSERT( 0 );
					(*errcnt)++;
					return;
				}
				ret = MakePart( pNode );
				if( ret ){
					DbgOut( "fbx : MakeRDB2ObjRec : MakePart error !!!\n" );
					_ASSERT( 0 );
					(*errcnt)++;
					return;
				}
				break;
	    }   
	}else{
//		DbgOut( "fbx : MakeRDB2ObjRec : no attribute error !!!\n" );
//		_ASSERT( 0 );
//		(*errcnt)++;


		ret = AddShape2Tree( SHDPART, lString.Buffer() );
		if( ret ){
			DbgOut( "fbx : MakeRDB2ObjRec : AddShape2Tree part error !!!\n" );
			_ASSERT( 0 );
			(*errcnt)++;
			return;
		}
		ret = MakePart( pNode );
		if( ret ){
			DbgOut( "fbx : MakeRDB2ObjRec : MakePart error !!!\n" );
			_ASSERT( 0 );
			(*errcnt)++;
			return;
		}
	}

	for(i = 0; i < pNode->GetChildCount(); i++)
	{
		MakeRDB2ObjRec(pSdkManager, pNode->GetChild(i), errcnt );
	}

	ret = lpth->End();
	if( ret ){
		_ASSERT( 0 );
		(*errcnt)++;
		return;
	}

}


int CFBXFile::AddShape2Tree( int shdtype, char* srcname )
{

	DbgOut( "fbx : AddShape2Tree : %d, %s\r\n", shdtype, srcname );


	int ret;
	int curdepth;

	curseri++;//!!!!!!!!!!!!!

	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	ret = tempinfo.SetMem( shdtype, BASE_TYPE );
	if( ret ){
		DbgOut( "fbx : AddShape2Tree : tempinfo.SetMem : BASE_TYPE error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}	


	//chkdepth = lpth->GetCurDepth();
	//if( chkdepth != curdepth ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	curdepth = lpth->GetCurDepth();

	ret = lpth->AddTree( srcname, curseri );
	if( curseri != ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = lpth->SetElemType( curseri, shdtype );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	
// shdhandler
	ret = lpsh->AddShdElem( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = lpsh->SetClockwise( curseri, 2 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//ret = lpsh->SetBDivUV( curseri, sheader.bdivu, sheader.bdivv );
	//if( ret ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	//int cmp;
	//cmp = strcmp( sheader.texname, "0" );
	//if( cmp ){
	//	ret = lpsh->SetTexName( curseri, sheader.texname );
	//	if( ret ){
	//		_ASSERT( 0 );
	//		return 1;
	//	}
	//}

	//ret = (*lpsh)( curseri )->SetTexRepetition( sheader.texrepx, sheader.texrepy );
	//_ASSERT( !ret );

	//ret = (*lpsh)( curseri )->SetTexTransparent( sheader.transparent );
	//_ASSERT( !ret );

	//ret = (*lpsh)( curseri )->SetUVAnime( sheader.uanime, sheader.vanime );
	//_ASSERT( !ret );

	//ret = (*lpsh)( curseri )->SetAlpha( sheader.alpha );
	//_ASSERT( !ret );

	ret = (*lpsh)( curseri )->SetTexRule( TEXRULE_MQ );
	_ASSERT( !ret );

	//ret = (*lpsh)( curseri )->SetNotUse( sheader.notuse );
	//_ASSERT( !ret );


	//if( sheader.flags & FLAGS_NOSKINNING ){
	//	ret = (*lpsh)( curseri )->SetNoSkinning( 1 );
	//	_ASSERT( !ret );
	//}else{
		ret = (*lpsh)( curseri )->SetNoSkinning( 0 );
		_ASSERT( !ret );
	//}

	//if( curshdtype != SHDINFSCOPE ){
	//	ret = (*lpsh)( curseri )->SetDispSwitchNo( sheader.dispswitchno );
	//	_ASSERT( !ret );
	//}else{
	//	ret = (*lpsh)( curseri )->SetDispSwitchNo( 0 );
	//	_ASSERT( !ret );
//
//		(*lpsh)( curseri )->scopenum = 0;//!!!!!!!!!!!!!
//	}


//	if( sheader.flags & FLAGS_NOTRANSIK ){
//		(*lpsh)( curseri )->m_notransik = 1;
//	}else{
//		(*lpsh)( curseri )->m_notransik = 0;
//	}

//	if( sheader.flags & FLAGS_ANCHORFLAG ){
//		(*lpsh)( curseri )->m_anchorflag = 1;
//	}else{
		(*lpsh)( curseri )->m_anchorflag = 0;
//	}
//	(*lpsh)( curseri )->m_mikodef = sheader.mikodef;


//	(*lpsh)( curseri )->m_userint1 = sheader.userint1;//!!!!!!!!!!!!

//	if( sheader.iklevel > 0 ){
//		(*lpsh)( curseri )->m_iklevel = sheader.iklevel;
//	}else{
		(*lpsh)( curseri )->m_iklevel = 5;
//	}

//	if( sheader.facetm180 != 0.0f ){
//		(*lpsh)( curseri )->facet = sheader.facetm180 + 180.0f;
//	}else{
		(*lpsh)( curseri )->facet = 180.0f;
//	}

//	if( curshdtype == SHDINFSCOPE ){
//		(*lpsh)( curseri )->alpha = 0.5f;
//	}


// mothandler
	ret = lpmh->AddMotionCtrl( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( shdtype == SHDBALLJOINT ){
		ret = lpmh->SetHasMotion( curseri, 1 );
		_ASSERT( !ret );
	}else{
		ret = lpmh->SetHasMotion( curseri, 0 );
		_ASSERT( !ret );
	}

	return 0;

}

int CFBXFile::MakePart( KFbxNode* pNode )
{
	int ret;
	ret = lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		DbgOut( "fbx : MakePart : Init3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CFBXFile::MakePolyMesh( KFbxNode* pNode )
{

	KFbxMesh* pMesh = (KFbxMesh*) pNode->GetNodeAttribute();
	_ASSERT( pMesh );

	int ret;
	int i, j;
	int lControlPointsCount = pMesh->GetControlPointsCount();
	int lPolygonCount = pMesh->GetPolygonCount();

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

	ret = tempinfo.SetMem( lControlPointsCount, MESHI_M );
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

	ret = lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		DbgOut( "fbx : MakePolyMesh error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* curselem;
	curselem = (*lpsh)( curseri );
	if( !curseri ){
		DbgOut( "fbx : MakePolyMesh : curselem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CPolyMesh* pm;
	pm = curselem->polymesh;
	if( !pm ){
		DbgOut( "fbx : MakePolyMesh : pm NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//vertex, uv
	{
		KFbxVector4* lControlPoints = pMesh->GetControlPoints();

		for (i = 0; i < lControlPointsCount; i++)
		{
			ret = curselem->SetPolyMeshPoint( i, (float)lControlPoints[i][0], (float)lControlPoints[i][1], (float)lControlPoints[i][2], i );
			if( ret ){
				DbgOut( "fbx : MakePolyMesh : SetPolyMeshPoint error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}
	//face
	ret = curselem->CreatePolyMeshIndex( lPolygonCount );
	if( ret ){
		DbgOut( "fbx : MakePolyMesh : CreatePolyMeshIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = pm->CreateTextureBuffer();
	if( ret ){
		DbgOut( "fbx : MakePolyMesh : CreateTextureBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	{
		int l;
		KFbxVector4* lControlPoints = pMesh->GetControlPoints(); 

		int vertexId = 0;
		for (i = 0; i < lPolygonCount; i++)
		{			
			int lPolygonSize = pMesh->GetPolygonSize(i);
			if( lPolygonSize == 3 ){
				int list[3];
				list[0] = pMesh->GetPolygonVertex( i, 0 );
				list[1] = pMesh->GetPolygonVertex( i, 1 );
				list[2] = pMesh->GetPolygonVertex( i, 2 );
			
				ret = curselem->SetPolyMeshIndex( i, list );
				if( ret ){
					DbgOut( "fbx : MakePolyMesh : SetPolyMeshIndex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				_ASSERT( 0 );
			}


			for (j = 0; j < lPolygonSize; j++)
			{
				int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

				
				if( pMesh->GetLayerCount() >= 1 ){
					l = 0;
				//for (l = 0; l < pMesh->GetLayerCount(); l++)
				//{
					/***
					KFbxLayerElementVertexColor* leVtxc = pMesh->GetLayer(l)->GetVertexColors();
					if (leVtxc)
					{
						sprintf(header, "            Color vertex (on layer %d): ", l); 

						switch (leVtxc->GetMappingMode())
						{
						case KFbxLayerElement::eBY_CONTROL_POINT:
							switch (leVtxc->GetReferenceMode())
							{
							case KFbxLayerElement::eDIRECT:
								DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
								break;
							case KFbxLayerElement::eINDEX_TO_DIRECT:
								{
									int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
									DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
								}
								break;
							default:
								break; // other reference modes not shown here!
							}
							break;

						case KFbxLayerElement::eBY_POLYGON_VERTEX:
							{
							switch (leVtxc->GetReferenceMode())
							{
							case KFbxLayerElement::eDIRECT:
								DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
								break;
							case KFbxLayerElement::eINDEX_TO_DIRECT:
								{
									int id = leVtxc->GetIndexArray().GetAt(vertexId);
									DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
								}
								break;
							default:
								break; // other reference modes not shown here!
							}
							}
							break;

						case KFbxLayerElement::eBY_POLYGON: // doesn't make much sense for UVs
						case KFbxLayerElement::eALL_SAME:   // doesn't make much sense for UVs
						case KFbxLayerElement::eNONE:       // doesn't make much sense for UVs
							break;
						}
					}
					***/

					KFbxLayerElementUV* leUV = pMesh->GetLayer(l)->GetUVs();
					if (leUV)
					{
						//sprintf(header, "            Texture UV (on layer %d): ", l); 

						switch (leUV->GetMappingMode())
						{
						case KFbxLayerElement::eBY_CONTROL_POINT:
							switch (leUV->GetReferenceMode())
							{
							case KFbxLayerElement::eDIRECT:
								ret = pm->SetUV( lControlPointIndex, 
									(float)leUV->GetDirectArray().GetAt(lControlPointIndex)[0], 
									(float)leUV->GetDirectArray().GetAt(lControlPointIndex)[1], 
									0 );
								if( ret ){
									DbgOut( "fbx : MakePolyMesh : pm SetUV error !!!\n" );
									_ASSERT( 0 );
									return 1;
								}
								//Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
								break;
							case KFbxLayerElement::eINDEX_TO_DIRECT:
								{
									int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
									//Display2DVector(header, leUV->GetDirectArray().GetAt(id));
									ret = pm->SetUV( lControlPointIndex,
										(float)leUV->GetDirectArray().GetAt(id)[0],
										(float)leUV->GetDirectArray().GetAt(id)[1],
										0 );
									if( ret ){
										DbgOut( "fbx : MakePolyMesh : pm SetUV error !!!\n" );
										_ASSERT( 0 );
										return 1;
									}
								}
								break;
							default:
								break; // other reference modes not shown here!
							}
							break;

						case KFbxLayerElement::eBY_POLYGON_VERTEX:
							{
							int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
							switch (leUV->GetReferenceMode())
							{
							case KFbxLayerElement::eDIRECT:
								//Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
								ret = pm->SetUV( lControlPointIndex,
									(float)leUV->GetDirectArray().GetAt(lTextureUVIndex)[0], 
									(float)leUV->GetDirectArray().GetAt(lTextureUVIndex)[1],
									0 );
								if( ret ){
									DbgOut( "fbx : MakePolyMesh : pm SetUV error !!!\n" );
									_ASSERT( 0 );
									return 1;
								}
								break;
							case KFbxLayerElement::eINDEX_TO_DIRECT:
								{
									int id = leUV->GetIndexArray().GetAt(lTextureUVIndex);
									//Display2DVector(header, leUV->GetDirectArray().GetAt(id));
									ret = pm->SetUV( lControlPointIndex,
										(float)leUV->GetDirectArray().GetAt(id)[0],
										(float)leUV->GetDirectArray().GetAt(id)[1],
										0 );
									if( ret ){
										DbgOut( "fbx : MakePolyMesh : pm SetUV error !!!\n" );
										_ASSERT( 0 );
										return 1;
									}
								}
								break;
							default:
								break; // other reference modes not shown here!
							}
							}
							break;

						case KFbxLayerElement::eBY_POLYGON: // doesn't make much sense for UVs
						case KFbxLayerElement::eALL_SAME:   // doesn't make much sense for UVs
						case KFbxLayerElement::eNONE:       // doesn't make much sense for UVs
							break;
						}
					}

				} // for layer
				vertexId++;
			} // for polygonSize
		} // for polygonCount
	}


	//material
	
	
	
	//texture




	return 0;
}

int CFBXFile::MakeJoint( KFbxNode* pNode )
{

	int ret;
	ret = lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		DbgOut( "fbx : MakeJoint : Init3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/***
	CVec3f jointloc;
	//jointloc.x = (pts + vertno)->x;
	//jointloc.y = (pts + vertno)->y;
	//jointloc.z = (pts + vertno)->z;
////////////

    KFbxTakeNode* pTakeNode = pNode->GetDefaultTakeNode();
	//printf("    Default Animation\n");
	if(pTakeNode)
	{
		KFCurve* lCurve = NULL;
		
		lCurve = pTakeNode->GetTranslationX();
		//printf("        TX\n");
		//DisplayCurve(lCurve);
		jointloc.x = static_cast<float> (lCurve->GetValue());

		lCurve = pTakeNode->GetTranslationY();
		//printf("        TY\n");
		//DisplayCurve(lCurve);
		jointloc.y = static_cast<float> (lCurve->GetValue());

		lCurve = pTakeNode->GetTranslationZ();
		//printf("        TZ\n");
		//DisplayCurve(lCurve);
		jointloc.z = static_cast<float> (lCurve->GetValue());


	}else{
		jointloc.x = 0.0f;
		jointloc.y = 0.0f;
		jointloc.z = 0.0f;
	}
	
///////////
	CShdElem* curselem;
	curselem = (*lpsh)(curseri);
	ret = curselem->SetJointLoc( &jointloc );
	if( ret ){
		DbgOut( "fbx : MakeJoint : SetJointLoc error !!!\n" );
		return 1;
	}
***/

	return 0;
}



void CFBXFile::SetJointPos( KFbxScene* pScene )
{

	int i, lCount = pScene->GetRootNode()->GetChildCount();

    for (i = 0; i < lCount; i++)
    {
        SetJointPosRec( pScene->GetRootNode()->GetChild(i) );
    }
}

KFbxXMatrix CFBXFile::GetGlobalDefaultPosition(KFbxNode* pNode)
{	
	KFbxXMatrix lLocalPosition;
	KFbxXMatrix lGlobalPosition;
	KFbxXMatrix lParentGlobalPosition;

	KFbxVector4 lT, lR, lS;
	lLocalPosition.SetT(pNode->GetDefaultT(lT));
	lLocalPosition.SetR(pNode->GetDefaultR(lR));
	lLocalPosition.SetS(pNode->GetDefaultS(lS));

	if (pNode->GetParent())
	{
		lParentGlobalPosition = GetGlobalDefaultPosition(pNode->GetParent());
		lGlobalPosition = lParentGlobalPosition * lLocalPosition;
	}
	else
	{
		lGlobalPosition = lLocalPosition;
	}

	return lGlobalPosition;
}


void CFBXFile::SetJointPosRec( KFbxNode* pNode )
{

    KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute){

		if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON){


			/***
			KFbxGeometry* pGeometry = (KFbxGeometry*)lNodeAttribute;
			KFbxXMatrix lGlobalOffPosition;
			pGeometry->GetPivot( lGlobalOffPosition );
			***/


		
			KFbxXMatrix lGlobalOffPosition = GetGlobalDefaultPosition( pNode );
		

			/***
			// Compute the node's global position.
			KFbxXMatrix lGlobalPosition = pNode->GetGlobalFromDefaultTake();
//			KFbxXMatrix lGlobalPosition = pNode->GetGlobalFromCurrentTake( 0.0 );

			// Geometry offset.
			// it is not inherited by the children.
			//KFbxXMatrix lGeometryOffset = GetGeometry(pNode);

			KFbxVector4 lT, lR, lS;
			KFbxXMatrix lGeometryOffset;
			lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
			lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
			lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);
			lGeometryOffset.SetT(lT);
			lGeometryOffset.SetR(lR);
			lGeometryOffset.SetS(lS);

			KFbxXMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
			***/

			/***
			KFbxVector4 lPos;
			lPos = pNode->GetGeometricTranslation( KFbxNode::eSOURCE_SET );
			//pNode->GetGeometricTranslation( KFbxNode::EPivotSet )
			***/

			/***
			KFbxGeometry* pGeo;
			pGeo = pNode->GetGeometry();
			_ASSERT( pGeo );

			KFbxXMatrix lGlobalOffPosition;
			pGeo->GetPivot( lGlobalOffPosition );
			***/

			/***
			KFbxVector4 lPos;
			pNode->GetDefaultT( lPos );
			***/


			/***
			KFbxVector4 lT, lR, lS;
			KFbxXMatrix lGlobalOffPosition;
			pNode->GetDefaultT( lT );
			pNode->GetDefaultR( lR );
			pNode->GetDefaultS( lS );
			lGlobalOffPosition.SetT(lT);
			lGlobalOffPosition.SetR(lR);
			lGlobalOffPosition.SetS(lS);
			***/

			/***
			KFbxVector4 lT, lR, lS;
			KFbxXMatrix lGlobalOffPosition;
			lT = pNode->GetLocalTFromDefaultTake();
			lR = pNode->GetLocalRFromDefaultTake();
			lS = pNode->GetLocalSFromDefaultTake();
			lGlobalOffPosition.SetT(lT);
			lGlobalOffPosition.SetR(lR);
			lGlobalOffPosition.SetS(lS);
			***/

			/***
			KFbxVector4 lPos;
			lPos = pNode->GetRotationPivot( KFbxNode::eSOURCE_SET );
			***/

			/***
			KFbxVector4 lPos;
			lPos = pNode->GetRotationOffset( KFbxNode::eSOURCE_SET );
			***/



		//////////
			KString lString;
			lString = pNode->GetName();
			int serino = 0;
			int ret;
			ret = lpth->GetBoneNoByName( lString.Buffer(), &serino, lpsh, 0 );
			if( ret || (serino < 0) ){
				DbgOut( "fbx : SetJointPosRec : GetBoneNoByName error %d!!!\n", serino );
				_ASSERT( 0 );
				return;
			}

			CShdElem* selem;
			selem = (*lpsh)( serino );
			_ASSERT( selem );

			CPart* part;
			part = selem->part;
			if( !part ){
				DbgOut( "fbx : SetJointPosRec : part NULL error !!!\n" );
				_ASSERT( 0 );
				return;
			}

			part->jointloc.x = (float)lGlobalOffPosition.GetT()[0];
			part->jointloc.y = (float)lGlobalOffPosition.GetT()[1];
			part->jointloc.z = (float)lGlobalOffPosition.GetT()[2];


//			part->jointloc.x = (float)lPos[0];
//			part->jointloc.y = (float)lPos[1];
//			part->jointloc.z = (float)lPos[2];

		}
	}
//////////

    int i, lCount = pNode->GetChildCount();

    for (i = 0; i < lCount; i++)
    {
        SetJointPosRec( pNode->GetChild(i) );
    }
}

int CFBXFile::SetInfElem( KFbxScene* pScene )
{

	int i, lCount = pScene->GetRootNode()->GetChildCount();

	int errcnt = 0;
    for (i = 0; i < lCount; i++)
    {
        SetInfElemRec( pScene->GetRootNode()->GetChild(i), &errcnt );
    }

	if( errcnt > 0 ){
		DbgOut( "fbx : SetInfElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

void CFBXFile::SetInfElemRec( KFbxNode* pNode, int* errcnt )
{
	int ret;
    int i, lLinkCount;
	KFbxLink* lLink;


    KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
    if (lNodeAttribute){

		if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH){


			KFbxMesh* pMesh = (KFbxMesh*) pNode->GetNodeAttribute();
			_ASSERT( pMesh );


			KString lDispName;
			lDispName = pNode->GetName();

			int dispno;
			ret = lpth->GetDispObjNoByName( lDispName.Buffer(), &dispno, lpsh, 0 );
			if( ret || (dispno < 0) ){
				DbgOut( "fbx : SetInfElemRec : th GetDispObjNoByName error %d %s\n", dispno, lDispName.Buffer() );
				_ASSERT( 0 );
				(*errcnt)++;
				return;
			}
			CShdElem* dispelem;
			dispelem = (*lpsh)( dispno );
			_ASSERT( dispelem );

			dispelem->m_loadbimflag = 2;//!!!!!!!!!!!

			CPolyMesh* pm;
			pm = dispelem->polymesh;
			_ASSERT( pm );

			int lControlPointsCount = pMesh->GetControlPointsCount();
			ret = pm->CreateInfElemIfNot( lControlPointsCount );
			if( ret ){
				DbgOut( "fbx : SetInfElemRec : pm CreateInfElemIfNot error !!!\n" );
				_ASSERT( 0 );
				(*errcnt)++;
				return;
			}



			KFbxGeometry* pGeometry = (KFbxGeometry*)pNode->GetNodeAttribute();
			lLinkCount = pGeometry->GetLinkCount();

			for (i = 0; i < lLinkCount; i++)
			{
			    lLink = pGeometry->GetLink(i);    
		
				if(lLink->GetLink() == NULL)
				{
					continue;//!!!!!!!!!!!!!!!!!!!!!!!
				}
					
				KString lString;
				lString = lLink->GetLink()->GetName();

				int boneno;
				ret = lpth->GetBoneNoByName( lString.Buffer(), &boneno, lpsh, 0 );
				if( ret || (dispno < 0) ){
					DbgOut( "fbx : SetInfElemRec : th GetBoneNoByName error %d %s\n", boneno, lString.Buffer() );
					_ASSERT( 0 );
					(*errcnt)++;
					return;
				}


				CShdElem* boneelem;
				boneelem = (*lpsh)( boneno );
				_ASSERT( boneelem );


				/***
				int childnum = 0;
				int childno[50];
				CShdElem* childelem = boneelem->child;
				while( childelem ){
					if( childnum >= 50 ){
						DbgOut( "fbx : SetInfElemRec : childnum overflow error !!!\n" );
						_ASSERT( 0 );
						(*errcnt)++;
						return;
					}
					childno[childnum] = childelem->serialno;
					childnum++;

					childelem = childelem->brother;
				}
				***/

				
				int j, lIndexCount = lLink->GetControlPointIndicesCount();
				int* lIndices = lLink->GetControlPointIndices();
				double* lWeights = lLink->GetControlPointWeights();


				for(j = 0; j < lIndexCount; j++)
				{
					CInfElem* ieptr;
					_ASSERT( pm->m_IE );
					ieptr = pm->m_IE + lIndices[j];
					ieptr->normalizeflag = 1;

					
					INFELEM addie;

					addie.bonematno = boneno;
					addie.childno = boneno;

					addie.kind = CALCMODE_DIRECT0;
					addie.userrate = 1.0f;
					addie.orginf = (float)lWeights[j];
					addie.dispinf = (float)lWeights[j];

					ret = ieptr->AddInfElem( addie );
					if( ret ){
						DbgOut( "fbx : SetInfElemRec : ie AddInfElem error !!!\n" );
						_ASSERT( 0 );
						(*errcnt)++;
						return;
					}
					

					/***
					int cno;
					for( cno = 0; cno < childnum; cno++ ){
						INFELEM addie;

						//addie.bonematno = boneno;
						//addie.childno = boneno;

						addie.bonematno = childno[cno];
						addie.childno = childno[cno];


						addie.kind = CALCMODE_DIRECT0;
						addie.userrate = 1.0f;
						addie.orginf = (float)lWeights[j];
						addie.dispinf = (float)lWeights[j];

						ret = ieptr->AddInfElem( addie );
						if( ret ){
							DbgOut( "fbx : SetInfElemRec : ie AddInfElem error !!!\n" );
							_ASSERT( 0 );
							(*errcnt)++;
							return;
						}
					}
					
					ret = ieptr->Normalize();
					if( ret ){
						DbgOut( "fbx : SetInfElemRec : ie Normalize error !!!\n" );
						_ASSERT( 0 );
						(*errcnt)++;
						return;
					}
					***/
				}

			}

		}

	}
}

/***
int CFBXFile::SetInfElem()
{
	int ret = 0;
	if( m_Scene ){
		ret = SetInfElem( m_Scene );
		_ASSERT( !ret );
	}
	return ret;
}

int CFBXFile::UnLoad()
{
	if( m_SdkManager ){
		DestroySdkObjects( m_SdkManager );
	}
	return 0;
}
***/

