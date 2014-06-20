
#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

//#include <shddata.h>
#include <shdhandler.h>
#include <shdelem.h>
#include <motionctrl.h>
#include <motfile.h>
#include <motionpoint2.h>
#include <treeelem2.h>

#include <d3ddisp.h>
#include <mqomaterial.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>


// loadmotion.l
//extern int LoadMotionData_Lex( CMotionCtrl* rootmctrl, LPCTSTR fname );

/***
HANDLE	CMotionCtrl::s_hHeap = NULL;
unsigned int	CMotionCtrl::s_uNumAllocsInHeap = 0;


unsigned int	CMotionCtrl::s_DataNo = 0;
void*	CMotionCtrl::s_DataHead = 0;

int		CMotionCtrl::s_HeadNum = 0;
BYTE*	CMotionCtrl::s_HeadIndex[2048];

int	CMotionCtrl::s_useflag[2048][MC_BLOCKLENG];
***/


/***
void	*CMotionCtrl::operator new ( size_t size )
{

	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
			
		if( s_hHeap == NULL )
			return NULL;
	}

	void	*p;
	p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size );
			
	if( p != NULL ){
		s_uNumAllocsInHeap++;
	}

	return (p);
}

void	CMotionCtrl::operator delete ( void *p )
{
		
	if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
		s_uNumAllocsInHeap--;
	}

	if( s_uNumAllocsInHeap == 0 ){
		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
	}
}
***/

/***
void	*CMotionCtrl::operator new ( size_t size )
{

	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
		if( s_hHeap == NULL )
			return NULL;
		ZeroMemory( s_HeadIndex, sizeof( BYTE* ) * 2048 );
		ZeroMemory( s_useflag, sizeof( int ) * 2048 * MC_BLOCKLENG );
	}


	if( !s_DataHead || (s_DataNo >= MC_BLOCKLENG) ){
		if( s_HeadNum >= 2048 ){
			_ASSERT( 0 );
			DbgOut( "MotionCtrl : operator new : s_HeadNum too large error !!!\n" );
			return NULL;
		}

		s_DataHead = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size * MC_BLOCKLENG );
		if( !s_DataHead ){
			_ASSERT( 0 );
			return NULL;
		}
		s_DataNo = 0;

		s_HeadIndex[s_HeadNum] = (BYTE*)s_DataHead;
		s_HeadNum++;
	}

	void* p;
	p = (void*)((BYTE*)s_DataHead + size * s_DataNo);
	if( p != NULL ){
		s_uNumAllocsInHeap++;
		s_DataNo++;
		s_useflag[s_HeadNum - 1][s_DataNo - 1] = 1;
	}

	return (p);
}

void	CMotionCtrl::operator delete ( void *p )
{
		
	s_uNumAllocsInHeap--;

	int headno;
	void* delhead = 0;
	int delno = -1;
	//pより小さい中で、一番大きいheadindexを探す。
	for( headno = 0; headno < s_HeadNum; headno++ ){
		void* headptr = (void*)(s_HeadIndex[headno]);
		if( ((unsigned long)p >= (unsigned long)headptr) 
			&& ((unsigned long)delhead < (unsigned long)headptr) ){
			delhead = headptr;
			delno = headno;
		}
	}
	if( delno < 0 ){
		_ASSERT( 0 );
		DbgOut( "MotionCtrl : operator delete invalid address error !!!\n" );
		return;
	}
	int datano;
	datano = ((unsigned long)p - (unsigned long)delhead) / sizeof( CMotionCtrl );
	_ASSERT( (datano >= 0) && (datano < MC_BLOCKLENG) ); 
	s_useflag[delno][datano] = 0;
	
	int usecnt = 0;
	int chkno;
	for( chkno = 0; chkno < MC_BLOCKLENG; chkno++ ){
		if( s_useflag[delno][chkno] == 1 )
			usecnt++;
	}
	if( usecnt == 0 ){
		HeapFree( s_hHeap, HEAP_NO_SERIALIZE, delhead );
		s_HeadIndex[delno] = 0;

		DbgOut( "MotionCtrl : operator delete : HeapFree %d\n", delno );

		if( delno == (s_HeadNum - 1) ){
			//最後のHeadIndexを消す場合の処理。
			s_DataHead = 0;
			DbgOut( "MotionCtrl : operator delete : last heap\n" );
		}
	}


	//if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
	//	s_uNumAllocsInHeap--;
	//}

	if( s_uNumAllocsInHeap == 0 ){		
		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
		DbgOut( "MotionCtrl : operator delete : HeapDestroy : %d\n", s_HeadNum );
	}
}
***/

CMotionCtrl::CMotionCtrl( CTreeHandler2* lphandler, int srcserino )
{
	int ret;

	serialno = srcserino;

	ret = InitParams( lphandler );
	if( ret ){
		isinit = 0;
		return;
	}

	ret = CreateObjs();
	if( ret ){
		isinit = 0;
		return;
	}
}

CMotionCtrl::~CMotionCtrl()
{
	DestroyObjs();
}
int CMotionCtrl::InitParams( CTreeHandler2* lphandler )
{
	//int ret;
	CBaseDat::InitParams();

	if( !lphandler ){
		return 1;
	}
	lpthandler = lphandler;


	//serialno = srcserino;
	brono = -1;
	bronum = 0;
	depth = 0;
	type = SHDTYPENONE;

	// chain は、CTreeElem2から取得してセット。
	parent = 0;
	child = 0;
	brother = 0;
	sister = 0;

	hasmotion = 0;// 対応するshdelemが、motion_interfaceを持っているとき　１
	haschain = 0;
	//motcookie = 0;

	leap = 0;

	InitMatrix();

	curval = 0.0f;

		// 複数のモーションのjointmat 格納、管理用
	motinfo = 0;

	boneno = -1;
	oldboneno = -1;
	morphno = -1;

	curdispswitch = 0;

	curframeno = 0;

	curmmenum = 0;
	curmmotelem = 0;

	m_nottoidle = 0;
	m_standard = 0;

	m_lpsh = 0;

	return 0;
}

int CMotionCtrl::InitMatrix()
{
	curmat.Identity();
	partmat.Identity();
	return 0;
}

int CMotionCtrl::CopyChain( CMotHandler* mothandler, CTreeElem2* curte )
{
	int parno, chilno, brono, sisno;
	//CTreeLevel* curchild = 0;
	
	CTreeElem2* parte = curte->parent;
	if( parte )
		parno = parte->serialno;
	else
		parno = -1;	

	CTreeElem2* chilte = curte->child;
	if( chilte ){
		chilno = chilte->serialno;
	}else{
		chilno = -1;
	}

	CTreeElem2* brote = curte->brother;
	if( brote )
		brono = brote->serialno;
	else
		brono = -1;

	CTreeElem2* siste = curte->sister;
	if( siste )
		sisno = siste->serialno;
	else
		sisno = -1;

	////
	if( parno > 0 )
		parent = (*mothandler)( parno );
	else
		parent = 0;

	if( chilno > 0 )
		child = (*mothandler)( chilno );
	else
		child = 0;

	if( brono > 0 )
		brother = (*mothandler)( brono );
	else
		brother = 0;

	if( sisno > 0 )
		sister = (*mothandler)( sisno );
	else
		sister = 0;

	haschain = 1;// 

	return 0;
}

void CMotionCtrl::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	if( motinfo ){
		delete motinfo;
		motinfo = 0;
	}

	if( curmmotelem ){
		int mmeno;
		for( mmeno = 0; mmeno < curmmenum; mmeno++ ){
			CMMotElem* delmme;
			delmme = *( curmmotelem + mmeno );
			if( delmme ){
				delete delmme;
			}
		}
		free( curmmotelem );
		curmmotelem = 0;
	}
	curmmenum = 0;

	hasmotion = 0;

}

int CMotionCtrl::SetMotionFrameNo( CMQOMaterial* mqohead, int mcookie, int srcframeno, int srcisfirst )
{
	int ret;

	if( !motinfo )
		return 1;

	curframeno = srcframeno;//!!!!!!!!!!!!!!!!!!!!

	ret = motinfo->SetMotionFrameNo( mcookie, srcframeno, srcisfirst );
	if( !ret ){
		CMatrix2* nextmat;
		nextmat = (*motinfo)( mcookie, srcframeno );
		if( nextmat ){
			curmat = *nextmat;
		}
	}

	motinfo->SetTexAnim();
	motinfo->SetAlpAnim( mqohead );
	motinfo->SetDSAnim( m_lpsh );
	motinfo->SetMMotAnim( this );

	return ret;


}

int CMotionCtrl::SetNextMotionFrameNo( int mcookie, int nextmk, int nextframeno, int befframeno )
{
	int ret;

	if( !motinfo )
		return 1;

	ret = motinfo->SetNextMotionFrameNo( mcookie, nextmk, nextframeno, befframeno );
	return ret;

}

int CMotionCtrl::SetMotionKind( int mcookie )
{
	int ret;

	if( !motinfo )
		return 1;

	ret = motinfo->SetMotionKind( mcookie );
	return ret;

}
int CMotionCtrl::SetMotionJump( int srcmotkind, int srcmotjump )
{
	int ret;

	if( !motinfo )
		return 1;

	ret = motinfo->SetMotionJump( srcmotkind, srcmotjump );
	return ret;	
}

int CMotionCtrl::SetMotionType( int srcmotkind, int srcmottype )
{
	int ret;

	if( !motinfo )
		return 1;

	ret = motinfo->SetMotionType( srcmotkind, srcmottype );
	return ret;	
}

int CMotionCtrl::SetInterpolationAll( int srcmotkind, int srcinterpolation )
{
	int ret;

	if( !motinfo )
		return 0;

	ret = motinfo->SetInterpolationAll( srcmotkind, srcinterpolation );
	return ret;	
}

int CMotionCtrl::PutQuaternionInOrder( int srcmotkind )
{
	int ret;

	if( !motinfo )
		return 1;

	ret = motinfo->PutQuaternionInOrder( srcmotkind );
	return ret;	

}

int CMotionCtrl::SetMotionStep( int srcmotkind, int srcmotstep )
{
	int ret;

	if( !motinfo )
		return 1;

	ret = motinfo->SetMotionStep( srcmotkind, srcmotstep );
	return ret;
}

/***
int CMotionCtrl::GetNextNo()
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 0;
	}

	return motinfo->GetNextNo();
}
***/
int CMotionCtrl::GetNextMotion( NEXTMOTION* nextmot, int getonly )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 0;
	}
	
	return motinfo->GetNextMotion( nextmot, getonly );
}


int CMotionCtrl::GetDispSwitch( DWORD* dstdw, int srcmotkind, int srcframeno )
{
	if( !motinfo ){
		_ASSERT( 0 );
		*dstdw = 0;
		return 1;
	}
	int ret;
	ret = motinfo->GetDispSwitch( dstdw, srcmotkind, srcframeno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotionCtrl::GetInterpolation( int* dstinterp, int srcmotkind, int srcframeno )
{
	if( !motinfo ){
		_ASSERT( 0 );
		*dstinterp = 0;
		return 1;
	}
	int ret;
	ret = motinfo->GetInterpolation( dstinterp, srcmotkind, srcframeno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


CMotionPoint2* CMotionCtrl::GetMotionPoint( int srcmotkind )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 0;
	}
	return motinfo->GetMotionPoint( srcmotkind );
}

CMotionPoint2* CMotionCtrl::GetMotionPoint2( int srcmotkind, int mpid )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 0;
	}
	return motinfo->GetMotionPoint2( srcmotkind, mpid );
}


int CMotionCtrl::StepMotion( CMQOMaterial* mqohead, int nextno )
{
	CMatrix2* retmat;
	
	if( !motinfo )
		return 1;

	curframeno = nextno;//!!!!

	retmat = motinfo->StepMotion( nextno );
	if( !retmat )
		return 1;

	curmat = *retmat;

//	if( type == SHDMORPH ){
//		curval = motinfo->StepJoint( nextno );
//		//DbgOut( "MotionCtrl : SetMotion : SHDMORPH : curval %f\n", curval );
//	}

	curdispswitch = motinfo->StepDispSwitch( nextno );
	
	motinfo->SetTexAnim();
	motinfo->SetAlpAnim( mqohead );
	motinfo->SetDSAnim( m_lpsh );
	motinfo->SetMMotAnim( this );

	return 0;
}

int CMotionCtrl::SetMatrix( CMatrix2* srcmat, __int64 setflag )
{
	 // jointmat 以外。
	CMatrix2* setmat = 0;

	if( setflag & MATRIX_ALL ){
		switch( setflag ){
		case MAT_CURRENT:
			setmat = &curmat;
			break;
		case MAT_PART:
			setmat = &partmat;
			break;
		default:
			setmat = 0;
			break;
		}
		if( !setmat )
			return 1;

		*setmat = *srcmat;
		return 0;

	}
	return 1;

}

int CMotionCtrl::RemakeMotionMatrix( int srcmotkind, int newtotal )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = motinfo->RemakeMotionMatrix( type, srcmotkind, newtotal );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return ret;
}

int CMotionCtrl::SetMotionName( int motid, char* srcname )
{
	if( !motinfo ){
		DbgOut( "mc : SetMotionName : motinfo NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = motinfo->SetMotionName( motid, srcname );
	if( ret ){
		DbgOut( "mc : SetMotionName : motinfo SetMotionName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMotionCtrl::AddMotion( int srcstandard, char* srcmotionname, int srcmottype, int srcmatnum, int srcmotjump )
{
	// 返り値は、motionnameに対応するcookie。
	// エラー時は -1
	m_standard = srcstandard;

	int motcookie;

	if( !srcmotionname ){
		_ASSERT( 0 );
		return -1;
	}

	motcookie = motinfo->AddMotion( srcstandard, type, srcmotionname, srcmottype, srcmatnum, srcmotjump );


	return motcookie;

}

int CMotionCtrl::SetMotionMatrix( CMatrix2* transmat, int motcookie, int frameno )
{
	int ret;
	
	if( !motinfo ){
		DbgOut( "CMotionCtrl : SetMotionMatrix : motinfo NULL error !!!\n" );
		return 1;
	}
	ret = motinfo->SetMotionMatrix( transmat, motcookie, frameno );
	return ret;
}
//int CMotionCtrl::SetJointValue( float srcval, int motcookie, int frameno )
//{
//	int ret;
//
//	if( !motinfo ){
//		_ASSERT( 0 );
//		return 1;
//	}
//	ret = motinfo->SetJointValue( srcval, motcookie, frameno );
//	return ret;
//}


int CMotionCtrl::CreateObjs()
{
	// ここでは　skipmatrixは作らない。
	// SHDTYPE を見て必要時に、CreateSkipMatrixを呼ぶ。

	motinfo = new CMotionInfo();
	if( !motinfo )
		return 1;

	//motcookie = (int*)malloc( sizeof( int ) * MOT_LAB_MAX );
	//if( !motcookie )
	//	return 1;
	//ZeroMemory( motcookie, sizeof( int ) * MOT_LAB_MAX );

	return 0;
}


int CMotionCtrl::DumpCurMat( HANDLE hfile )
{
	int ret = 0;
	int i;


	ret = Write2File( hfile, "serial %d : %f, %f, %f, %f\n", 
		serialno, curmat[0][0], curmat[0][1], curmat[0][2], curmat[0][3] );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	for( i = 1; i < 4; i++ ){
		ret = Write2File( hfile, "\t%f, %f, %f, %f\n", 
			curmat[i][0], curmat[i][1], curmat[i][2], curmat[i][3] );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
	return 0;
}

int CMotionCtrl::DumpPartMat( HANDLE hfile )
{
	int ret = 0;
	int i;


	ret = Write2File( hfile, "serial %d : %f, %f, %f, %f\n", 
		serialno, partmat[0][0], partmat[0][1], partmat[0][2], partmat[0][3] );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	for( i = 1; i < 4; i++ ){
		ret = Write2File( hfile, "\t%f, %f, %f, %f\n", 
			partmat[i][0], partmat[i][1], partmat[i][2], partmat[i][3] );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
	return 0;
}
/***
int CMotionCtrl::DumpJointVal( CMotFile* mfile, int mcookie )
{
	int ret = 0;

	ret = motinfo->DumpJointVal( mfile, mcookie, serialno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMotionCtrl::DumpMotion( CMotFile* mfile, int mcookie )
{
	int ret = 0;

	ret = motinfo->DumpMem( mfile, mcookie );
	if( ret ){
		DbgOut( "CMotionCtrl : DumpMotion : motinfo->DumpMem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;

}

int CMotionCtrl::DumpMotion( HANDLE hfile, int mcookie )
{
	int ret = 0;

	ret = Write2File( hfile, "serial %d\n", serialno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = motinfo->DumpMem( hfile, mcookie );
	if( ret ){
		DbgOut( "CMotionCtrl : DumpMotion : motinfo->DumpMem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
***/
int CMotionCtrl::SetHasMotion( int srchasmot )
{
	hasmotion = srchasmot;
	return 0;
}
int CMotionCtrl::GetHasMotion()
{
	return hasmotion;
}

int CMotionCtrl::CalcNoJointMatrix( CMotionCtrl* srcctrl, int mcookie )
{
	int ret;
	CMotionInfo* srcmotinfo;

	srcmotinfo = srcctrl->motinfo;
	if( !srcmotinfo )
		return 1;

	ret = motinfo->MultMotionMatrix( srcmotinfo, mcookie, partmat );
	if( ret ){
		DbgOut( "CMotionCtrl : CalcNoJointMatrix : motinfo->CalcNoJointMatrix error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;	
}

int CMotionCtrl::MultCopyMotionMatrix( CMotionCtrl* srcctrl, CMatrix2 multmat, int mcookie )
{
	int ret;
	CMotionInfo* srcmotinfo;

	srcmotinfo = srcctrl->motinfo;
	if( !motinfo || !srcmotinfo )
		return 1;

	ret = motinfo->MultCopyMotionMatrix( srcmotinfo, multmat, mcookie );
	if( ret ){
		DbgOut( "CMotionCtrl : MultCopyMotionMatrix : motinfo->MultCopyMotionMatrix error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMotionCtrl::CopyMotionMatrix( CMotionCtrl* srcctrl, int mcookie )
{
	int ret;
	CMotionInfo* srcmotinfo;

	srcmotinfo = srcctrl->motinfo;
	if( !motinfo || !srcmotinfo )
		return 1;

	ret = motinfo->CopyMotionMatrix( srcmotinfo, mcookie );
	if( ret ){
		DbgOut( "CMotionCtrl : CopyMotionMatrix : motinfo->CopyMotionMatrix error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMotionCtrl::CopyMotionMatrix( CMatrix2* srcmat, int mcookie, int matnum )
{
	int ret;

	if( !motinfo )
		return 1;

	ret = motinfo->CopyMotionMatrix( srcmat, mcookie, matnum );
	if( ret ){
		DbgOut( "CMotionCtrl : CopyMotionMatrix srcmat : motinfo->CopyMotionMatrix error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;

}

//int CMotionCtrl::CopyJointVal( float* srcval, int mcookie, int valnum )
//{
//	int ret;
//	if( !motinfo )
//		return 1;
//	ret = motinfo->CopyJointVal( srcval, mcookie, valnum );
//	if( ret ){
//		_ASSERT( 0 );
//		return 1;
//	}
//	return 0;
//}

void CMotionCtrl::CalcDepthReq( int srcdepth )
{
	depth = srcdepth;

	if( brother )
		brother->CalcDepthReq( srcdepth );
	if( child )
		child->CalcDepthReq( srcdepth + 1 );

}
CMotionPoint2* CMotionCtrl::IsExistMotionPoint( int srcmotkind, int srcframeno )
{
	CMotionPoint2* retmp;
	if( !motinfo ){
		_ASSERT( 0 );
		return 0;
	}
	retmp = motinfo->IsExistMotionPoint( srcmotkind, srcframeno );

	return retmp;
}
int CMotionCtrl::DestroyMotionObj( int delcookie )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	ret = motinfo->DestroyMotionObj( delcookie );
	if( ret ){
		DbgOut( "mc : DestroyMotionObj : mi DestroyMotionObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return ret;
}



CMotionPoint2* CMotionCtrl::AddMotionPoint( int srcmotkind, CMotionPoint* srcmp, int srcinterp )
{
	//_ASSERT( 0 );
	//互換性のために、とっておく。
	return AddMotionPoint( srcmotkind, srcmp->m_frameno,
		srcmp->m_q,
		srcmp->m_mvx, srcmp->m_mvy, srcmp->m_mvz, 0, srcinterp, 1.0f, 1.0f, 1.0f, 0 );//!!!! dispswitch は　０をセット。
}
CMotionPoint2* CMotionCtrl::AddMotionPoint( int srcmotkind, CMotionPoint2* srcmp, int srcinterp )
{
	return AddMotionPoint( srcmotkind, srcmp->m_frameno,
		srcmp->m_q,
		srcmp->m_mvx, srcmp->m_mvy, srcmp->m_mvz, srcmp->dispswitch, srcinterp, srcmp->m_scalex, srcmp->m_scaley, srcmp->m_scalez, srcmp->m_userint1, srcmp->m_eul );
}



CMotionPoint2* CMotionCtrl::AddMotionPoint( int srcmotkind, int srcframeno,
	CQuaternion srcq,
	float srcmvx, float srcmvy, float srcmvz, DWORD srcdswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1 )
{
	CMotionPoint2* retmp;
	if( !motinfo ){
		_ASSERT( 0 );
		return 0;
	}
	retmp = motinfo->AddMotionPoint( srcmotkind, srcframeno,
		srcq,
		srcmvx, srcmvy, srcmvz, srcdswitch, srcinterp, scalex, scaley, scalez, userint1 );
	_ASSERT( retmp );
	return retmp;
}

CMotionPoint2* CMotionCtrl::AddMotionPoint( int srcmotkind, int srcframeno,
	CQuaternion srcq,
	float srcmvx, float srcmvy, float srcmvz, DWORD srcdswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1, D3DXVECTOR3 srceul )
{
	if( type == SHDDESTROYED )
		return 0;

	CMotionPoint2* retmp;
	if( !motinfo ){
		_ASSERT( 0 );
		return 0;
	}
	retmp = motinfo->AddMotionPoint( srcmotkind, srcframeno,
		srcq,
		srcmvx, srcmvy, srcmvz, srcdswitch, srcinterp, scalex, scaley, scalez, userint1, srceul );
	_ASSERT( retmp );
	return retmp;
}
int CMotionCtrl::DeleteMpOutOfRange( int srcmotkind, int maxno )
{
	int ret = 0;
	CMotionPoint2* mpptr = GetMotionPoint( srcmotkind );
	CMotionPoint2* nextmp = 0;
	while( mpptr ){
		nextmp = mpptr->next;
		int frameno = mpptr->m_frameno;
		if( frameno > maxno ){
			ret = DeleteMotionPoint( srcmotkind, frameno );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		mpptr = nextmp;
	}
	return 0;
}

int CMotionCtrl::DeleteMotionPoint( int srcmotkind, int srcframeno )
{
	int ret;
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	ret = motinfo->DeleteMotionPoint( srcmotkind, srcframeno );
	_ASSERT( !ret );
	return ret;
}
int CMotionCtrl::LeaveFromChainMP( int srcmotkind, int srcframeno )
{
	CMotionPoint2* retmp;
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	retmp = motinfo->LeaveFromChainMP( srcmotkind, srcframeno );
	_ASSERT( retmp );
	if( retmp ){
		return 0;
	}else{
		return 1;
	}
}

int CMotionCtrl::CalcMLMat( CMotionCtrl* parmc )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}

	CMatrix2* dstmat = &(motinfo->mlmat);
	if( motinfo->kindno < 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!
	}
	CMatrix2* srcraw = *(motinfo->rawmat + motinfo->kindno);

	if( curframeno < 0 ){
		_ASSERT( 0 );
		return 0;//!!!!!!!!!!!!!!!!!!
	}

	if( parmc && parmc->motinfo ){
		*dstmat = *( srcraw + curframeno ) * parmc->motinfo->mlmat;
		curmat = *dstmat;
	}else{
		*dstmat = *( srcraw + curframeno );
		curmat = *dstmat;
	}


	return 0;
}

int CMotionCtrl::RawmatToMotionmat( int mcookie, CMotionCtrl* parmc, int srcstart, int srcend )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	CMatrix2* dstmat = *(motinfo->firstmot + mcookie);
	if( !dstmat ){
		_ASSERT( 0 );
		return 1;
	}
	CMatrix2* srcraw = *(motinfo->rawmat + mcookie);
	if( !srcraw ){
		_ASSERT( 0 );
		return 1;
	}
	int matnum = *(motinfo->motnum + mcookie);
	if( matnum <= 0 ){
		_ASSERT( 0 );
		return 1;
	}
	if( parmc ){
		CMotionInfo* parmi = parmc->motinfo;
		if( !parmi ){
			_ASSERT( 0 );
			return 1;
		}
		CMatrix2* parmat = *(parmi->firstmot + mcookie);
		if( !parmat ){
			_ASSERT( 0 );
			return 1;
		}

		int matno;
		for( matno = srcstart; matno <= srcend; matno++ ){
			*(dstmat + matno) = *(srcraw + matno) * *(parmat + matno);
		}
	}else{
		int matno;
		for( matno = srcstart; matno <= srcend; matno++ ){
			*(dstmat + matno) = *(srcraw + matno);
		}	
	}
	return 0;
}

int CMotionCtrl::RawmatToMotionmatCap( int mcookie, CMotionCtrl* parmc, int srcframe )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	CMatrix2* dstmat = *(motinfo->firstmot + mcookie);
	if( !dstmat ){
		_ASSERT( 0 );
		return 1;
	}
	CMatrix2* srcraw = *(motinfo->rawmat + mcookie);
	if( !srcraw ){
		_ASSERT( 0 );
		return 1;
	}
	int matnum = *(motinfo->motnum + mcookie);
	if( matnum <= 0 ){
		_ASSERT( 0 );
		return 1;
	}
	if( parmc ){
		CMotionInfo* parmi = parmc->motinfo;
		if( !parmi ){
			_ASSERT( 0 );
			return 1;
		}
		CMatrix2* parmat = *(parmi->firstmot + mcookie);
		if( !parmat ){
			_ASSERT( 0 );
			return 1;
		}

		int matno = srcframe;
		*(dstmat + matno) = *(srcraw + matno) * *(parmat + matno);
	}else{
		int matno = srcframe;
		*(dstmat + matno) = *(srcraw + matno);
	}
	return 0;
}




int CMotionCtrl::MakeRawmat( int mcookie, CShdElem* lpselem, int srcstart, int srcend, CShdHandler* lpsh, CMotHandler* lpmh, int calceulflag )
{
	int ret;
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	ret = motinfo->MakeRawmat( mcookie, lpselem, srcstart, srcend, lpsh, lpmh, calceulflag );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return ret;
}

int CMotionCtrl::MakeRawmatCap( int mcookie, CShdElem* lpselem, int srcframe, CShdHandler* lpsh, CMotHandler* lpmh, CMotionPoint2* srcmp )
{
	int ret;
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	ret = motinfo->MakeRawmatCap( mcookie, lpselem, srcframe, lpsh, lpmh, srcmp );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return ret;
}



int CMotionCtrl::IsJoint()
{
	if( (type >= SHDROTATE) && (type <= SHDBALLJOINT) && (type != SHDMORPH) )
		return 1;
	else
		return 0;

}

int CMotionCtrl::SetCurrentMotion( CMQOMaterial* mqohead, int mcookie, int srcframeno )
{
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}

	curframeno = srcframeno;//!!!!

	CMatrix2* matptr = (*motinfo)( mcookie, srcframeno );
	if( !matptr )
		return 1;
	curmat = *matptr;

	int ret;
	ret = motinfo->GetDispSwitch( &curdispswitch, mcookie, srcframeno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//　2003/5/29で追加。
	motinfo->kindno = mcookie;
	motinfo->motno = srcframeno;


	motinfo->SetTexAnim();
	motinfo->SetAlpAnim( mqohead );
	motinfo->SetDSAnim( m_lpsh );
	motinfo->SetMMotAnim( this );

	return 0;
}

int CMotionCtrl::GetMotionJump( int motid, int* motjumpptr )
{
	if( !motinfo ){
		*motjumpptr = 0;
		return 0;
	}

	if( (motid < 0) || (motid >= motinfo->kindnum) ){
		DbgOut( "motionctrl : GetMotionJump : motid error !!!\n" );
		_ASSERT( 0 );
		*motjumpptr = 0;
		return 1;
	}
	
	if( motinfo->motjump ){
		*motjumpptr = *(motinfo->motjump + motid);
	}else{
		*motjumpptr = 0;
	}

	return 0;
}

/***
int CMotionCtrl::GetMotionFrameNum( int motid, int* framenum )
{
	if( !motinfo ){
		*framenum = 0;
		return 0;
	}

	if( (motid < 0) || (motid >= motinfo->kindnum) ){
		DbgOut( "motionctrl : GetMotionFrameNum : motid error !!!\n" );
		_ASSERT( 0 );
		*framenum = 0;
		return 1;
	}
	
	if( motinfo->motnum ){
		*framenum = *(motinfo->motnum + motid);
	}else{
		*framenum = 0;
	}

	return 0;
}
***/

int CMotionCtrl::CopyMotionFrame( CShdElem* selem, int srcmotid, int srcframe, int dstmotid, int dstframe,
	CShdHandler* lpsh, CMotHandler* lpmh )
{
	int ret;
	if( !motinfo ){
		_ASSERT( 0 );
		return 1;
	}
	ret = motinfo->CopyMotionFrame( selem, srcmotid, srcframe, dstmotid, dstframe, lpsh, lpmh );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return ret;
}



int CMotionCtrl::CalcMotionPointOnFrame( CShdElem* selem, CMotionPoint2* dstmp, 
	int srcmotid, int srcframe, int* hasmpflag, CShdHandler* lpsh, CMotHandler* lpmh )
{
	int ret;

	if( motinfo && (srcmotid < motinfo->kindnum) ){
		ret = lpmh->SetBoneAxisQ( lpsh, selem->serialno, lpmh->GetZaType( srcmotid ), srcmotid, srcframe );
		_ASSERT( !ret );
		CQuaternion axisq;
		ret = selem->GetBoneAxisQ( &axisq );
		_ASSERT( !ret );

		ret = motinfo->CalcMotionPointOnFrame( &axisq, selem, dstmp, srcmotid, srcframe, hasmpflag );
		if( ret ){
			DbgOut( "mc : CalcMotionPointOnFrame : mi CalcMotionPointOnFrame error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		*hasmpflag = 0;
		CMotionPoint2 inimp;
		*dstmp = inimp;
	}

	return 0;
}

int CMotionCtrl::AddCurMMotElem( CShdElem* srcbaseelem )
{
	CMMotElem* findmme;
	findmme = GetCurMMotElem( srcbaseelem );
	if( findmme ){
		return 0;
	}

	curmmotelem = (CMMotElem**)realloc( curmmotelem, sizeof( CMMotElem* ) * ( curmmenum + 1 ) );
	if( !curmmotelem ){
		DbgOut( "mc : AddCurMMotElem : curmmotelem alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMMotElem* newmme;
	newmme = new CMMotElem( srcbaseelem );
	if( !newmme ){
		DbgOut( "mc : AddCurMMotElem : newmme alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( curmmotelem + curmmenum ) = newmme;
	curmmenum++;


	return 0;
}

int CMotionCtrl::DeleteCurMMotElem( CShdElem* srcbaseelem )
{
	CMMotElem* delmme;
	delmme = GetCurMMotElem( srcbaseelem );
	if( !delmme ){
		return 0;
	}

	CMMotElem** newcurmme;
	newcurmme = (CMMotElem**)malloc( sizeof( CMMotElem* ) * ( curmmenum - 1 ) );
	if( !newcurmme ){
		DbgOut( "mc : DeleteCurMMotElem : newcurmme alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int setno = 0;
	int mmeno;
	for( mmeno = 0; mmeno < curmmenum; mmeno++ ){
		CMMotElem* cpmme;
		cpmme = *( curmmotelem + mmeno );
		if( cpmme != delmme ){
			*( newcurmme + setno ) = cpmme;
			setno++;
		}
	}

	if( setno != (curmmenum - 1) ){
		DbgOut( "mc : DeleteCurMMotElem : setno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delete delmme;
	free( curmmotelem );
	curmmotelem = newcurmme;
	curmmenum--;

	return 0;
}

CMMotElem* CMotionCtrl::GetCurMMotElem( CShdElem* srcbaseelem )
{
	CMMotElem* retmme = 0;

	int mmeno;
	for( mmeno = 0; mmeno < curmmenum; mmeno++ ){
		CMMotElem* chkmme;
		chkmme = *( curmmotelem + mmeno );
		if( chkmme->m_baseelem == srcbaseelem ){
			retmme = chkmme;
			break;
		}
	}
	return retmme;
}

int CMotionCtrl::ScaleSplineMv( int motid, D3DXVECTOR3 srcmult )
{
	if( !motinfo ){
		return 0;
	}
	int ret;
	ret = motinfo->ScaleSplineMv( motid, srcmult );
	if( ret ){
		DbgOut( "mc : ScaleSplineMv : mi ScaleSplineMv error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}