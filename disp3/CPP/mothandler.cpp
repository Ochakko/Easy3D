#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <treehandler2.h>
#include <treeelem2.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <part.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>

#include <motfile.h>

#include <motionpoint2.h>
#include <quaternion.h>

#include <DispSwitch.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>
#include <asmfunc.h>
#include <cpuid.h>

#include <morph.h>

#include <TexKey.h>
#include <TexChange.h>
#include <TexKeyPt.h>
#include <TexChangePt.h>
#include <DSKey.h>
#include <AlpKey.h>
#include <AlpChange.h>
#include <MMotKey.h>
#include <MMotElem.h>
#include <mqomaterial.h>

#include <MState.h>

//////////////
// extern
extern CCpuid g_cpuinfo;


/////////////
// static 
HANDLE	CMotHandler::s_hHeap = NULL;
unsigned int	CMotHandler::s_uNumAllocsInHeap = 0;



////////////////


/***
#ifdef _DEBUG
	void	*CMotHandler::operator new ( size_t size )
	{

		if( s_hHeap == NULL ){
			s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
			//s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, commitsize, 0 );
			
			if( s_hHeap == NULL )
				return NULL;
		}

		void	*p;
		p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, size );
			
		if( p != NULL ){
			s_uNumAllocsInHeap++;
		}

		return (p);
	}


	void	CMotHandler::operator delete ( void *p )
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
#endif
***/


CMotHandler::CMotHandler( CTreeHandler2* srcthandler )
{
	int ret;
	int	roottype = SHDROOT;

	ret = InitParams( srcthandler );
	if( ret ){
		DbgOut( "CMotHandler : constructor : InitParams error !!!\n" );
		return;
	}

	// root elem : serial 0
	ret = AddMotionCtrl( 0, 0 );
	if( ret ){
		DbgOut( "CMotHandler : constructor : make root elem error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	// type == SHDROOT のセット
	ret += (*this)( 0 )->SetMem( &roottype, BASE_TYPE );
	if( ret ){
		DbgOut( "CMotHandler : constructor : set root elem type error !!!\n" );
		_ASSERT( 0 );
		return;
	}

	//DbgOut( "mothandler : sizeof mp2 %d\n", sizeof( CMotionPoint2 ) );


DbgOut( "CMotHandler : constructor : check : s2mot_leng %d\n", s2mot_leng );

}

CMotHandler::~CMotHandler()
{

	int i;
	CMotionCtrl* del_ctrl = 0;
	CMotionCtrl** motarray = 0;

	if( s2mot ){
		for( i = 0; i < s2mot_leng; i++ ){
			del_ctrl = *(s2mot + i);
			delete del_ctrl;
		}

		free( s2mot );
		s2mot = 0;
	}


	if( m_motnum ){
		free( m_motnum );
		m_motnum = 0;
	}
	if( m_zatype ){
		free( m_zatype );
		m_zatype = 0;
	}
	if( m_definterp ){
		free( m_definterp );
		m_definterp = 0;
	}
	if( m_firstbs ){
		for( i = 0; i < m_kindnum; i++ ){
			CBSphere* delbs;
			delbs = *(m_firstbs + i);
			delete [] delbs;
		}

		free( m_firstbs );
		m_firstbs = 0;
	}



	DbgOut( "CMotHandler : destructor : delete elem\n" );
}


int CMotHandler::InitParams( CTreeHandler2* srcthandler )
{
	isfirst = 1;

	thandler = srcthandler;

	s2mot_allocleng = 0;
	s2mot_leng = 0;
	s2mot = 0;

	m_kindnum = 0; // motion の種類の数
	m_curframeno = 0;

	//m_kindno = 0;	// motion の種類の　current値
	//m_motno = 0; // current motion No.
	m_motnum = 0; // 各motkindno に対するmotno の最大値。
	m_zatype = 0;
	m_definterp = 0;

	m_firstbs = 0;
	m_curbs.InitParams();

	m_curmotkind = 0;

	m_fuid = 0;
	m_fuleng = 10;

	m_ek.InitParams();

	m_nottoidle = 0;

	m_standard = 0;

	return 0;
}

int CMotHandler::AddMotionCtrl( int srcserino, int srcdepth )
{
	CTreeElem2* curtelem = 0;
	int ret, brono, type;

	if( !thandler )
		return 1;
	
	curtelem = (*thandler)( srcserino );
	brono = curtelem->brono;
	type = curtelem->type;

	DbgOut( "CMotHandler : AddMotionCtrl : srcseri %d, brono %d, srcdepth %d\n", 
	   srcserino, brono, srcdepth );

	ret = AddS2Mot( srcserino, brono, srcdepth, type );
	if( ret ){
		DbgOut( "CMotHandler : AddS2Mot error !!!\n" );
		return 1;
	}

	return 0;
}

int CMotHandler::AddS2Mot( int srcserino, int srcbrono, int srcdepth, int srctype )
{


	//int curmotno;
	int saveleng = s2mot_leng;
	int ret = 0;
	CMotionCtrl* newmot = 0;


	newmot = new CMotionCtrl( thandler, srcserino );
	if( !newmot ){
		DbgOut( "CMotHandler : AddS2Mot : newmot alloc error !!!\n" );
		return 1;
	}

	if( srcserino >= s2mot_leng )
		s2mot_leng = srcserino + 1;

	if( s2mot_leng > s2mot_allocleng ){
		while( s2mot_leng > s2mot_allocleng )
			s2mot_allocleng += 1024;

		s2mot = (CMotionCtrl**)realloc( s2mot, sizeof( CMotionCtrl* ) * s2mot_allocleng );
		if( !s2mot ){
			DbgOut( "CMotHandler : AddS2Mot : s2mot alloc error !!!\n" );
			return 1;
		}
	}
	*(s2mot + srcserino) = newmot;


	/***
	// set2index
	if( srcserino >= s2mot_leng ){
		s2mot_leng = srcserino + 1;	
		s2mot = (CMotionCtrl**)realloc( s2mot, sizeof( CMotionCtrl* ) * s2mot_leng );
		if( !s2mot ){
			DbgOut( "CMotHandler : AddS2Mot : s2mot alloc error !!!\n" );
			return 1;
		}
	}
	*(s2mot + srcserino) = newmot;
	***/

	newmot->depth = srcdepth;
	newmot->brono = srcbrono;
	newmot->type = srctype;

	//ret = SetChain( newmot );
	_ASSERT( !ret );

	return ret;
}


int CMotHandler::SetChain( int offset )
{
	int ret = 0;
	int serino, treeleng;
	CTreeElem2* curte = 0;
	CMotionCtrl* curmot = 0;

	treeleng = thandler->s2e_leng;

	if( offset == 0 ){
		for( serino = 0; serino < treeleng; serino++ ){
			curte = (*thandler)( serino );
			curmot = (*this)( serino );

			ret = curmot->CopyChain( this, curte );		
			if( ret ){
				_ASSERT( 0 );
				return ret;			
			}
		}
	}else{
		CTreeElem2* te;
		CMotionCtrl* headmc;
		te = (*thandler)( offset );
		headmc = (*this)( offset );
		ret = headmc->CopyChain( this, te );		
		if( ret ){
			_ASSERT( 0 );
			return ret;
		}
		//serialno 1 のbrotherに先頭データをchain
		CMotionCtrl* broelem;
		CMotionCtrl* lastbro = 0;
		broelem = (*this)( 1 );
		while( broelem ){
			lastbro = broelem;
			broelem = broelem->brother;			
		}
		if( lastbro ){
			lastbro->brother = headmc;
		}else{
			_ASSERT( 0 );
			return 1;
		}
		headmc->sister = lastbro;

		for( serino = offset + 1; serino < treeleng; serino++ ){
			curte = (*thandler)( serino );
			curmot = (*this)( serino );

			ret = curmot->CopyChain( this, curte );		
			if( ret ){
				_ASSERT( 0 );
				return ret;			
			}
		}
	}
	return ret;
}

int CMotHandler::InitMatrix( int srcseri )
{
	int ret;

	ret = (*this)( srcseri )->InitMatrix();
	_ASSERT( !ret );

	return ret;
}

int CMotHandler::SetTransMat( int srcseri, CMatrix2* srcmat )
{
	int ret;

	ret = (*this)( srcseri )->SetMatrix( srcmat, MAT_CURRENT );
	_ASSERT( !ret );
	return ret;
}
int CMotHandler::SetPartMat( int srcseri, CMatrix2* srcmat )
{
	int ret;

	ret = (*this)( srcseri )->SetMatrix( srcmat, MAT_PART );
	_ASSERT( !ret );
	return ret;
}

int CMotHandler::SetHasMotion( int srcseri, int srchasmot )
{
	int ret;
	ret = (*this)( srcseri )->SetHasMotion( srchasmot );
	if( ret ){
		DbgOut( "CMotHandler : SetHasMotion error !!!\n" );
		_ASSERT( 0 );
	}
	return ret;
}

int CMotHandler::SetNoJointMotion( int mcookie )
{
	int i, ret;
	CMotionCtrl* melem;
	CMotionCtrl* parmelem;

	// motioninterfaceを持たないelem (名前の先頭に記号のないパートなど)は、
	// (自分のpartmat * 親のelemのmotionmatrix) を計算して格納。

		// motioninterfaceを持たないelemは、melem->hasmotion == 0
	for( i = 1; i < s2mot_leng; i++ ){
		melem = (*this)( i );
		if( (melem->hasmotion == 0) && (melem->depth > 1) && (melem->type != SHDMORPH) ){
			parmelem = melem->parent;
			if( !parmelem ){
				DbgOut( "CMotHandler : SetNoJointMotion : parmelem NULL error %d\n", i );
				_ASSERT( 0 );
				return 1;
			}

			//ret = melem->CopyMotionMatrix( parmelem, mcookie );
			ret = melem->CalcNoJointMatrix( parmelem, mcookie );
			if( ret ){
				DbgOut( "CMotHandler : SetNoJointMotion : melem->CalcNoJointMatrix error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	return 0;
}

int CMotHandler::SetSkipMotion( CShdHandler* lpsh, int mcookie )
{
	// !!!!!!!!!!!!!
	// leapはhasmotionを考慮に入れたスキップ数。
	// !!!!!!!!!!!!!

	int ret, i, j, k, leng;
	int linenum, bmseri, blseri;
	int leap, leapmax;
	//, skip, skipmax;
	CShdElem* selem = 0;
	CMeshInfo* mi = 0;
	CBezMesh* bm = 0;
	CBezLine* bl = 0;

	int leaproute[SKIPMAX]; 
	CMatrix2 partmat[SKIPMAX];
	CMatrix2 multmat;

	leng = lpsh->s2shd_leng;
	for( i = 1; i < leng; i++ ){
		selem = (*lpsh)( i );

		if( selem->type == SHDBEZIERSURF ){
			bmseri = selem->serialno;
			bm = selem->bmesh;
			if( !bm ){
				DbgOut( "CMotHandler : SetSkipMotion : bm NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			mi = selem->GetMeshInfo();
			linenum = mi->n;
			
			leapmax = (*this)( bmseri )->leap;

			if( leapmax > 0 ){
				ZeroMemory( leaproute, sizeof(int) * SKIPMAX );

				ret = GetLeapRoute( bmseri, leaproute, leapmax );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
				
				ZeroMemory( partmat, sizeof( CMatrix2 ) * SKIPMAX );
				ret = GetPartMatrix( partmat, leaproute, leapmax );
				_ASSERT( !ret );

				for( j = 0; j < linenum; j++ ){
					bl = (*bm)( j );
					if( !bl ){
						DbgOut( "CMotHandler : SetSkipMotion : bl NULL error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					//skip = bl->meshinfo->skip;
					blseri = *(bm->hblseri + j);
					leap = (*this)( blseri )->leap;
					
					multmat.Identity();
					for( k = 0; k < leap; k++ ){
						multmat *= partmat[k];
					}
					//multmat *= skipmat[skip + 1];

					//ret = CopyMotionMatrix( blseri, skiproute[skip + 1], mcookie );
					ret = MultCopyMotionMatrix( blseri, leaproute[leap], multmat, mcookie );
					if( ret ){
						DbgOut( "CMotHandler : SetSkipMotion : MultCopyMotionMatrix error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}
	}
	return 0;

}

int CMotHandler::MultCopyMotionMatrix( int dstseri, int srcseri, CMatrix2 multmat, int mcookie )
{
	int ret;
	CMotionCtrl* dstctrl;
	CMotionCtrl* srcctrl;
	int chkdst, chksrc;

	dstctrl = (*this)( dstseri );
	srcctrl = (*this)( srcseri );

	if( !dstctrl || !srcctrl )
		return 1;

	chkdst = dstctrl->serialno;
	chksrc = srcctrl->serialno;

	if( (chkdst != dstseri) || (chksrc != srcseri) ){
		DbgOut( "CMotHandler : MultCopyMotionMatrix : check serialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = dstctrl->MultCopyMotionMatrix( srcctrl, multmat, mcookie );
	if( ret ){
		DbgOut( "CMotHandler : MultCopyMotionMatrix : dstctrl->MultCopyMotionMatrix error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}



int CMotHandler::CopyMotionMatrix( int dstseri, int srcseri, int mcookie )
{
	int ret;
	CMotionCtrl* dstctrl;
	CMotionCtrl* srcctrl;
	int chkdst, chksrc;

	dstctrl = (*this)( dstseri );
	srcctrl = (*this)( srcseri );

	if( !dstctrl || !srcctrl )
		return 1;

	chkdst = dstctrl->serialno;
	chksrc = srcctrl->serialno;

	if( (chkdst != dstseri) || (chksrc != srcseri) ){
		DbgOut( "CMotHandler : CopyMotionMatrix : check serialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = dstctrl->CopyMotionMatrix( srcctrl, mcookie );
	if( ret ){
		DbgOut( "CMotHandler : CopyMotionMatrix : dstctrl->CopyMotionMatrix error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CMotHandler::DumpCurMat( int dbgflag, char* fname )
{
	int ret = 0;
	CMotionCtrl* curmot = 0;
	int serino;
	HANDLE hfile;
	DWORD startpoint = 0;
	DWORD dwerror = 0;

	if( !fname ){
		hfile = CreateFile( (LPCTSTR)"dumpshd.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	}else{
		hfile = CreateFile( (LPCTSTR)fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	}
	if( hfile == INVALID_HANDLE_VALUE ){
		ret = 1;
		DbgOut( "CMotHandler : DumpCurMat : CreateFile error !!!\n" );
		_ASSERT( 0 );
		goto dumpexit;
	}	
	
	SetLastError( 0 );
	startpoint = GetFileSize( hfile, NULL );
	dwerror = GetLastError();
	if( dwerror ){
		ret = 1;
		DbgOut( "CMotHandler : DumpCurMat : GetFileSize error !!!\n" );
		_ASSERT( 0 );
		goto dumpexit;
	}

	SetLastError( 0 );
	SetFilePointer( hfile, startpoint, NULL, FILE_BEGIN );
	dwerror = GetLastError();
	if( dwerror ){
		ret = 1;
		DbgOut( "CMotHandler : DumpCurMat : SetFilePointer error !!!\n" );
		_ASSERT( 0 );
		goto dumpexit;
	}

	ret = Write2File( hfile, "\n\n\nTransformMatrix\n" );
	if( ret ){
		DbgOut( "CMotHandler : DumpCurMat : Write2File error !!!\n" );
		_ASSERT( 0 );
		goto dumpexit;
	}

	for( serino = 0; serino < s2mot_leng; serino++ ){
		curmot = (*this)( serino );
		ret = curmot->DumpCurMat( hfile );
		if( ret ){
			DbgOut( "CMotHandler : DumpCurMat : curmot->DumpCurMat error !!!\n" );
			_ASSERT( 0 );
			break;
		}
	}
	goto dumpexit;

dumpexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
	}
	
	return ret;
}

int CMotHandler::DumpPartMat( int dbgflag, char* fname )
{
	int ret = 0;
	CMotionCtrl* curmot = 0;
	int serino;
	HANDLE hfile;
	DWORD startpoint = 0;
	DWORD dwerror = 0;

	if( !fname ){
		hfile = CreateFile( (LPCTSTR)"dumpshd.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	}else{
		hfile = CreateFile( (LPCTSTR)fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	}
	if( hfile == INVALID_HANDLE_VALUE ){
		ret = 1;
		DbgOut( "CMotHandler : DumpPartMat : CreateFile error !!!\n" );
		_ASSERT( 0 );
		goto dumppexit;
	}	
	
	SetLastError( 0 );
	startpoint = GetFileSize( hfile, NULL );
	dwerror = GetLastError();
	if( dwerror ){
		ret = 1;
		DbgOut( "CMotHandler : DumpPartMat : GetFileSize error !!!\n" );
		_ASSERT( 0 );
		goto dumppexit;
	}

	SetLastError( 0 );
	SetFilePointer( hfile, startpoint, NULL, FILE_BEGIN );
	dwerror = GetLastError();
	if( dwerror ){
		ret = 1;
		DbgOut( "CMotHandler : DumpPartMat : SetFilePointer error !!!\n" );
		_ASSERT( 0 );
		goto dumppexit;
	}

	ret = Write2File( hfile, "\n\n\nPartMatrix\n" );
	if( ret ){
		DbgOut( "CMotHandler : DumpPartMat : Write2File error !!!\n" );
		_ASSERT( 0 );
		goto dumppexit;
	}

	for( serino = 0; serino < s2mot_leng; serino++ ){
		curmot = (*this)( serino );
		ret = curmot->DumpPartMat( hfile );
		if( ret ){
			DbgOut( "CMotHandler : DumpPartMat : curmot->DumpPartMat error !!!\n" );
			_ASSERT( 0 );
			break;
		}
	}
	goto dumppexit;

dumppexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
	}
	
	return ret;
}

/***
int CMotHandler::DumpMotion( char* mname, int mcookie, int framemax, int mottype )
{
	int ret = 0;
	CMotionCtrl* curmot = 0;
	int serino, srctype;
	CMotFile* mfile;
	MOTFILEMARKER mmarker;

	mfile = new CMotFile();
	if( !mfile ){
		DbgOut( "CMotHandler : DumpMotion : alloc CMotFile error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto dumpmexit;
	}

	ret = mfile->CreateMotFile( mname, mcookie, framemax, mottype, s2mot_leng - 1 );// 1～s2mot_leng
	if( ret ){
		DbgOut( "CMotHandler : DumpMotion : CreateMotFile error !!!\n" );
		_ASSERT( 0 );
		goto dumpmexit;
	}	

	//matrix dataの書き出し
	for( serino = 1; serino < s2mot_leng; serino++ ){
		curmot = (*this)( serino );
		ret = curmot->DumpMotion( mfile, mcookie );
		if( ret ){
			DbgOut( "CMotHandler : DumpMotion : curmot->DumpMotion error !!!\n" );
			_ASSERT( 0 );
			break;
		}
	}

	//morph joint val の書き出し
	for( serino = 1; serino < s2mot_leng; serino++ ){
		curmot = (*this)( serino );
		srctype = curmot->type;
		if( srctype == SHDMORPH ){
			ret = curmot->DumpJointVal( mfile, mcookie );
			if( ret ){
				_ASSERT( 0 );
				break;
			}
		}
	}

	//mot file の終わりの目印の書き出し
	ZeroMemory( &mmarker, sizeof( MOTFILEMARKER ) );
	mmarker.magicno = MOTMARKER_END;
	ret = mfile->WriteMarker( &mmarker );
	_ASSERT( !ret );

	//file close はdeleteで行われる。

	goto dumpmexit;

dumpmexit:
	if( mfile )
		delete mfile;

	return ret;

}
***/
/***
int CMotHandler::DumpMotion( char* mname, int mcookie, int framemax, int mottype )
{
	int ret = 0;
	int ret1;
	CMotionCtrl* curmot = 0;
	int serino;
	HANDLE hfile;
	char header[1024];

	hfile = CreateFile( "dumpmotion.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		ret = 1;
		DbgOut( "CMotHandler : DumpMotion : CreateFile error !!!\n" );
		_ASSERT( 0 );
		goto dumpmexit;
	}	

	ZeroMemory( header, 1024 );
	sprintf( header, "motionname : %s,\nframemax : %d\nmotiontype : %d\n",
		mname, framemax, mottype );

	ret1 = Write2File( hfile, header );
	if( ret1 ){
		DbgOut( "CMotHandler : DumpMotion : Write2File error !!!\n" );
		_ASSERT( 0 );
		goto dumpmexit;
	}

	for( serino = 1; serino < s2mot_leng; serino++ ){
		curmot = (*this)( serino );
		ret = curmot->DumpMotion( hfile, mcookie );
		if( ret ){
			DbgOut( "CMotHandler : DumpMotion : curmot->DumpMotion error !!!\n" );
			_ASSERT( 0 );
			break;
		}
	}
	goto dumpmexit;

dumpmexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
	}
	
	return ret;

}
***/
int CMotHandler::GetPartMatrix( CMatrix2* dstmat, int dstseri )
{
	CMotionCtrl* mctrl = (*this)( dstseri );
	if( !mctrl )
		return 1;
	*dstmat = mctrl->partmat;
	return 0;
}

int CMotHandler::GetPartMatrix( CMatrix2* dstmat, int* skiproute, int skipmax )
{
	int i, seri;
	CMotionCtrl* mctrl = 0;
	
	for( i = 0; i <= skipmax; i++ ){
		seri = *(skiproute + i);
		mctrl = (*this)( seri );
		if( !mctrl )
			return 1;
		*(dstmat + i) = mctrl->partmat;
	}

	return 0;
}


int CMotHandler::GetCurMatrix( CMatrix2* dstmat, int srcseri )
{
	CMotionCtrl* mctrl = (*this)( srcseri );
	if( !mctrl )
		return 1;
	*dstmat = mctrl->curmat;
	return 0;
}

int CMotHandler::GetCurMatrix( CMatrix2* dstmat, int* skiproute, int skipmax )
{
	int i, seri;
	CMotionCtrl* mctrl = 0;
	
	for( i = 0; i <= skipmax; i++ ){
		seri = *(skiproute + i);
		mctrl = (*this)( seri );
		if( !mctrl )
			return 1;
		*(dstmat + i) = mctrl->curmat;
	}

	return 0;
}

int CMotHandler::SetCurMatrix( int dstseri, CMatrix2 srcmat )
{
	int ret;
	CMotionCtrl* mctrl = 0;

	mctrl = (*this)( dstseri );
	if( !mctrl )
		return 1;
	
	ret = mctrl->SetMatrix( &srcmat, MAT_CURRENT );

	return ret;
}

int CMotHandler::SetMotionName( int motid, char* srcname )
{
	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mh : SetMotionName : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int i, ret;
	for( i = 1; i < s2mot_leng; i++ ){
		CMotionCtrl* mc = (*this)( i );
		int mctype = mc->type;
		if( (i == 1) || mc->IsJoint() ){
			ret = mc->SetMotionName( motid, srcname );
			if( ret ){
				DbgOut( "mh : SetMotionName : mc SetMotionName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


int CMotHandler::AddMotion( int zatype, char* motionname, int motiontype, int framenum, int srcinterpolation, int srcmotjump )
{
	DbgOut( "MotHandler : AddMotion %s, framenum %d\n", motionname, framenum );

	//int m_kindnum; // motion の種類の数
	//int m_kindno;	// motion の種類の　current値
	//int m_motno; // current motion No.
	//int* m_motnum; // 各motkindno に対するmotno の最大値+1。


	m_kindnum++;
	m_motnum = (int*)realloc( m_motnum, sizeof( int ) * m_kindnum );
	if( !m_motnum ){
		DbgOut( "MotHandler : AddMotion : m_motnum alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	*(m_motnum + m_kindnum - 1) = framenum;

	m_zatype = (int*)realloc( m_zatype, sizeof( int ) * m_kindnum );
	if( !m_zatype ){
		DbgOut( "MotHandler : AddMotion : zatype alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	*(m_zatype + m_kindnum - 1) = zatype;


	m_definterp = (int*)realloc( m_definterp, sizeof( int ) * m_kindnum );
	if( !m_definterp ){
		DbgOut( "mh : AddMotion : m_definterp alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*(m_definterp + m_kindnum - 1) = srcinterpolation;
///
	CBSphere* newbs;
	newbs = new CBSphere[ framenum ];
	if( !newbs ){
		DbgOut( "MotHandler : AddMotion : newbs alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	
	m_firstbs = (CBSphere**)realloc( m_firstbs, sizeof( CBSphere* ) * m_kindnum );
	if( !m_firstbs ){
		DbgOut( "MotHandler : AddMotion : m_firstbs alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	*(m_firstbs + m_kindnum - 1) = newbs;

	
///
	int i;

	int retcookie = -1;
	int befcookie = -1;

	int addno = 0;
	for( i = 1; i < s2mot_leng; i++ ){
		CMotionCtrl* mc = (*this)( i );
		int mctype = mc->type;
		if( (i == 1) || mc->IsJoint() ){

//_ASSERT( m_standard == 1 );		
			retcookie = (*this)( i )->AddMotion( m_standard, motionname, motiontype, framenum, srcmotjump );
			if( (retcookie) < 0 || ( (addno != 0) && (befcookie != retcookie) ) ){
				DbgOut( "CMotHandler : AddMotion : error !!!\n" );
				_ASSERT( 0 );
				return -1;
			}
			addno++;
			befcookie = retcookie;
		}
	}
	return retcookie;
}

int	CMotHandler::SetMotionMatrix( CMatrix2* transmat, int shapeno, int motcookie, int frameno )
{
	int ret;

	ret = (*this)( shapeno )->SetMotionMatrix( transmat, motcookie, frameno );
	if( ret ){
		DbgOut( "CMotHandler : SetMotionMatrix : error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}


int CMotHandler::SetMotionJump( int srcmotkind, int srcmotjump )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : SetMotionJump : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcmotkind < 0) || (srcmotkind >= m_kindnum) ){
		DbgOut( "mothandler : SetMotionJump : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int i, ret;

	CMotionCtrl* mc;
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){ 
			ret = mc->SetMotionJump( srcmotkind, srcmotjump );
			if( ret ){
				DbgOut( "CMotHandler : SetMotionJump error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	return 0;
}

int CMotHandler::SetMotionType( int srcmotkind, int srcmottype )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : SetMotiontype : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcmotkind < 0) || (srcmotkind >= m_kindnum) ){
		DbgOut( "mothandler : SetMotiontype : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int i, ret;

	CMotionCtrl* mc;
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){ 
			ret = mc->SetMotionType( srcmotkind, srcmottype );
			if( ret ){
				DbgOut( "CMotHandler : SetMotionType error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	return 0;
}

int CMotHandler::GetMotionType( int srcmotkind, int* mottypeptr )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetMotiontype : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcmotkind < 0) || (srcmotkind >= m_kindnum) ){
		DbgOut( "mothandler : GetMotiontype : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( s2mot_leng <= 1 ){
		DbgOut( "mothandler : GetMotionType : mot_leng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}




	CMotionCtrl* firstmc;
	firstmc = (*this)( 1 );//!!!!!!!!!!!!!!

	if( !firstmc ){
		DbgOut( "mothandler : GetMotionType : firstmc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionInfo* firstmi;
	firstmi = firstmc->motinfo;

	if( !firstmi ){
		DbgOut( "mothandler : GetMotionType : firstmi error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*mottypeptr = *(firstmi->mottype + srcmotkind);

	return 0;
}

int CMotHandler::GetDefInterp( int srcmotkind, int* dstinterp )
{
	if( (srcmotkind < 0) || (srcmotkind >= m_kindnum) ){
		DbgOut( "mothandler : GetDefInterp : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstinterp = *( m_definterp + srcmotkind );
	return 0;
}


int CMotHandler::SetDefInterp( int srcmotkind, int srcinterpolation )
{
	if( (srcmotkind < 0) || (srcmotkind >= m_kindnum) ){
		DbgOut( "mothandler : SetDefInterp : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( m_definterp + srcmotkind ) = srcinterpolation;

	return 0;
}

int CMotHandler::SetInterpolationAll( int srcmotkind, int srcinterpolation )
{
	if( (srcmotkind < 0) || (srcmotkind >= m_kindnum) ){
		DbgOut( "mothandler : SetInterpolationAll : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int seri;
	CMotionCtrl* mcptr;
	for( seri = 0; seri < s2mot_leng; seri++ ){
		mcptr = (*this)( seri );
		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			ret = mcptr->SetInterpolationAll( srcmotkind, srcinterpolation );
			_ASSERT( !ret );
		}
	}

	return 0;
}


int CMotHandler::ChangeTotalFrame( int srcmotkind, int newtotal, int expandflag )
{
	int ret;
	int i;
	CMotionCtrl* mc;
	//範囲外のmotion pointを削除。

	int deleteflag = 0;

	/***
	if( expandflag == -1 ){
		int oldleng;
		ret = GetMotionFrameLength( srcmotkind, &oldleng );
		if( ret ){
			DbgOut( "mothandler : ChangeTotalFrame : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( oldleng > newtotal ){
			deleteflag = 1;
		}else{
			deleteflag = 0;
		}
		
	}else{
		deleteflag = expandflag;
	}
	
	if( deleteflag == 0 ){
		for( i = 1; i < s2mot_leng; i++ ){
			mc = (*this)( i );
			if( (i == 1) || (mc->IsJoint() && (mc->type != SHDMORPH)) ){
				ret = mc->DeleteMpOutOfRange( srcmotkind, newtotal - 1 );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}
	***/



	if( expandflag == -1 ){
		int oldleng;
		ret = GetMotionFrameLength( srcmotkind, &oldleng );
		if( ret ){
			DbgOut( "mothandler : ChangeTotalFrame : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( oldleng > newtotal ){
			deleteflag = 1;
		}else{
			deleteflag = 0;
		}
		
	}else{
		//deleteflag = expandflag;
		if( expandflag == 0 ){
			deleteflag = 1;
		}else{
			deleteflag = 0;
		}
	}
	
	if( deleteflag == 1 ){
		for( i = 1; i < s2mot_leng; i++ ){
			mc = (*this)( i );
			if( (i == 1) || (mc->IsJoint() && (mc->type != SHDMORPH)) ){
				ret = mc->DeleteMpOutOfRange( srcmotkind, newtotal - 1 );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}

				CMotionInfo* mi;
				mi = mc->motinfo;
				if( mi ){
					ret = mi->DeleteTexKeyOutOfRange( srcmotkind, newtotal - 1 );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}

					ret = mi->DeleteDSKeyOutOfRange( srcmotkind, newtotal - 1 );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}

					ret = mi->DeleteMMotKeyOutOfRange( srcmotkind, newtotal - 1 );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}

					ret = mi->DeleteAlpKeyOutOfRange( srcmotkind, newtotal - 1 );
					if( ret ){
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}
	}


	//matrixの作り直し。
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){
			ret = mc->RemakeMotionMatrix( srcmotkind, newtotal );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	// mothandler :: m_motnum の更新
	*( m_motnum + srcmotkind ) = newtotal;


	//motjump
	int oldmotjump;
	mc = (*this)( 1 );
	ret = mc->GetMotionJump( srcmotkind, &oldmotjump );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	if( oldmotjump >= newtotal ){
		ret = SetMotionJump( srcmotkind, 0 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

int CMotHandler::SetMotionFrameNoML( CMQOMaterial* mqohead, int seri, int mcookie, int srcframeno, int srcisfirst )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : SetMotionFrameNoML : no motion error !!!\n" );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : SetMotionFrameNoML : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcframeno < 0) || (srcframeno >= *(m_motnum + mcookie)) ){
		DbgOut( "mothandler : SetMotionFrameNoML : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (seri < 0) || (seri >= s2mot_leng) ){
		DbgOut( "mh : SetMotionKindML : serialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	CMotionCtrl* mc;
	mc = (*this)( seri );
	if( mc->IsJoint() ){ 
		ret = mc->SetMotionFrameNo( mqohead, mcookie, srcframeno, srcisfirst );
		if( ret ){
			DbgOut( "CMotHandler : SetMotionFrameNoML error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}


int CMotHandler::SetMotionFrameNo( CShdHandler* lpsh, int mcookie, int srcframeno, int srcisfirst )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : SetMotionFrameNo : no motion error !!!\n" );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : SetMotionFrameNo : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcframeno < 0) || (srcframeno >= *(m_motnum + mcookie)) ){
		DbgOut( "mothandler : SetMotionFrameNo : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int i, ret;

	CMotionCtrl* mc;
	//CShdElem* selem;
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){ 
			ret = mc->SetMotionFrameNo( lpsh->m_mathead, mcookie, srcframeno, srcisfirst );
			if( ret ){
				DbgOut( "CMotHandler : SetMotionFrameNo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	m_curmotkind = mcookie;
	m_curframeno = srcframeno;//!!!!!!

	return 0;
}

int CMotHandler::SetNextMotionFrameNoML( int seri, int mcookie, int nextmk, int nextframeno, int befframeno )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : SetNextMotionFrameNoML : no motion error !!!\n" );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : SetNextMotionFrameNoML : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (nextmk < 0) || (nextmk >= m_kindnum) ){
		DbgOut( "mothandler : SetNextMotionFrameNoML : nextmk error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( (nextframeno < 0) || (nextframeno >= *(m_motnum + nextmk)) ){
		DbgOut( "mothandler : SetNextMotionFrameNoML : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (seri < 0) || (seri >= s2mot_leng) ){
		DbgOut( "mh : SetNextMotionFrameNoML : serialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	CMotionCtrl* mc;
	mc = (*this)( seri );
	if( mc->IsJoint() ){ 
		ret = mc->SetNextMotionFrameNo( mcookie, nextmk, nextframeno, befframeno );
		if( ret ){
			DbgOut( "CMotHandler : SetNextMotionFrameNoML : mc SetNextMotionFrameNo error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;


}


int CMotHandler::SetNextMotionFrameNo( CShdHandler* lpsh, int mcookie, int nextmk, int nextframeno, int befframeno )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : SetNextMotionFrameNo : no motion error !!!\n" );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : SetNextMotionFrameNo : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (nextmk < 0) || (nextmk >= m_kindnum) ){
		DbgOut( "mothandler : SetNextMotionFrameNo : nextmk error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( (nextframeno < 0) || (nextframeno >= *(m_motnum + nextmk)) ){
		DbgOut( "mothandler : SetNextMotionFrameNo : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int i, ret;

	CMotionCtrl* mc;
	//CShdElem* selem;
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){ 
			ret = mc->SetNextMotionFrameNo( mcookie, nextmk, nextframeno, befframeno );
			if( ret ){
				DbgOut( "CMotHandler : SetNextMotionFrameNo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int CMotHandler::GetNextMotionFrameNoML( int boneno, int mcookie, int* nextmkptr, int* nextframenoptr )
{
	*nextmkptr = -1;
	*nextframenoptr = 0;

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetNextMotionFrameNoML : no motion error !!!\n" );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : GetNextMotionFrameNoML : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetNextMotionFrameNoML : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////

	CMotionCtrl* mc;
	mc = (*this)( boneno );

	CMotionInfo* mi;
	mi = mc->motinfo;

	if( mi ){
		*nextmkptr = (mi->nextmot + mcookie)->mk;
		*nextframenoptr = (mi->nextmot + mcookie)->aftframeno;

	}else{
		_ASSERT( 0 );
		*nextmkptr = -1;
		*nextframenoptr = 0;
		return 1;
	}

	return 0;

}


int CMotHandler::GetNextMotionFrameNo( int mcookie, int* nextmkptr, int* nextframenoptr )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetNextMotionFrameNo : no motion error !!!\n" );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : GetNextMotionFrameNo : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////////
	int i;
	CMotionCtrl* firstjoint = 0;
	CMotionCtrl* curmc;
	for( i = 1; i < s2mot_leng; i++ ){
		curmc = (*this)( i );
		if( curmc->IsJoint() && (curmc->type != SHDMORPH) ){
			firstjoint = curmc;
			break;
		}
	}

	if( firstjoint ){
		CMotionInfo* mi;
		mi = firstjoint->motinfo;
		_ASSERT( mi );

		*nextmkptr = (mi->nextmot + mcookie)->mk;
		*nextframenoptr = (mi->nextmot + mcookie)->aftframeno;

	}else{
		_ASSERT( 0 );
		*nextmkptr = -1;
		*nextframenoptr = 0;
		return 1;
	}

	return 0;
}

int CMotHandler::SetMotionKindML( int seri, int mcookie )
{
	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : SetMotionKindML : mcookie error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (seri < 0) || (seri >= s2mot_leng) ){
		DbgOut( "mothandler : SetMotionKindML : serialno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	CMotionCtrl* mc;
	mc = (*this)( seri );
	if( mc->IsJoint() ){
		ret = mc->SetMotionKind( mcookie );
		if( ret ){
			DbgOut( "CMotHandler : SetMotionKindML : mc SetMotionKind error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CMotHandler::SetMotionKind( int mcookie )
{
	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}

	if( (mcookie < 0) || (mcookie >= m_kindnum) ){
		DbgOut( "mothandler : SetMotionKind : mcookie error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int i, ret;

	CMotionCtrl* mc;
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){ 
			ret = mc->SetMotionKind( mcookie );
			if( ret ){
				DbgOut( "CMotHandler : SetMotionKind error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	m_curmotkind = mcookie;
	m_curframeno = 0;//!!!!!!


	return 0;
}
int CMotHandler::GetMotionKind( int* motkindptr )
{
	int i;

	CMotionCtrl* firstjoint = 0;
	CMotionCtrl* curmc;
	for( i = 1; i < s2mot_leng; i++ ){
		curmc = (*this)( i );
		if( curmc->IsJoint() && (curmc->type != SHDMORPH) && (curmc->motinfo)){
			firstjoint = curmc;
			break;
		}
	}

	if( firstjoint ){
		CMotionInfo* mi;
		mi = firstjoint->motinfo;
		if( mi ){
			*motkindptr = mi->kindno;
		}else{
			*motkindptr = -1;
		}

	}else{
		*motkindptr = -1;
	}

	return 0;
}

int CMotHandler::GetMotionFrameNoML( int boneno, int* motkindptr, int* framenoptr )
{
	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetMotionFrameNoML : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		*motkindptr = mi->kindno;
		*framenoptr = mc->curframeno;
	}else{
		*motkindptr = -1;
		*framenoptr = 0;
		_ASSERT( 0 );
	}

	return 0;

}


int CMotHandler::GetMotionFrameNo( int* motkindptr, int* framenoptr )
{

	int i;

	CMotionCtrl* firstjoint = 0;
	CMotionCtrl* curmc;
	for( i = 1; i < s2mot_leng; i++ ){
		curmc = (*this)( i );
		if( curmc->IsJoint() && (curmc->type != SHDMORPH) && (curmc->motinfo) ){
			firstjoint = curmc;
			break;
		}
	}

	if( firstjoint ){
		CMotionInfo* mi;
		mi = firstjoint->motinfo;
		if( mi ){
			*motkindptr = mi->kindno;
			*framenoptr = firstjoint->curframeno;
			//if( *motkindptr >= 0 ){
			//	*framemaxptr = *( mi->motnum + *motkindptr );
			//}else{
			//	_ASSERT( 0 );
			//	*framemaxptr = 0;
			//}
		}else{
			*motkindptr = -1;
			*framenoptr = 0;
			//*framemaxptr = 0;
		}

	}else{
		*motkindptr = -1;
		*framenoptr = 0;
		//*framemaxptr = 0;
	}

	return 0;
}




int CMotHandler::SetMotionStep( int srcmotkind, int srcmotstep )
{
	int i, ret;
	CMotionCtrl* mc;
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){
			ret = mc->SetMotionStep( srcmotkind, srcmotstep );
			if( ret ){
				DbgOut( "CMotHandler : SetMotionStep error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	return 0;
}

int CMotHandler::SetCurrentMotion( CShdHandler* lpsh, int mcookie, int srcframeno )
{
	if( m_kindnum <= 0 ){
		return 0;
	}

	InitTexAnim( lpsh );
	InitAlpAnim( lpsh );
	InitDSAnim( lpsh );

	int i;
	int ret = 0;
	CMotionCtrl* mcptr;
	for( i = 0; i < s2mot_leng; i++ ){
		mcptr = (*this)( i );
		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			ret += mcptr->SetCurrentMotion( lpsh->m_mathead, mcookie, srcframeno );
			_ASSERT( !ret );
		}
	}

	m_curmotkind = mcookie;
	m_curframeno = srcframeno;

	return ret;
}

void CMotHandler::SetCurrentMotionReq( CMQOMaterial* mqohead, CMotionCtrl* mcptr, int mcookie, int srcframeno, int broflag, int* errorflag )
{
	int ret;

	m_curmotkind = mcookie;
	m_curframeno = srcframeno;

	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		ret = mcptr->SetCurrentMotion( mqohead, mcookie, srcframeno );
		if( ret ){
			_ASSERT( 0 );
			(*errorflag)++;
			return;
		}
	}

	if( broflag ){
		CMotionCtrl* broelem;
		broelem = mcptr->brother;
		if( broelem ){
			SetCurrentMotionReq( mqohead, broelem, mcookie, srcframeno, 1, errorflag );
		}
	}

	CMotionCtrl* chilelem;
	chilelem = mcptr->child;
	if( chilelem ){
		SetCurrentMotionReq( mqohead, chilelem, mcookie, srcframeno, 1, errorflag );
	}
}

int CMotHandler::SetNewPoseML( CShdHandler* lpsh )
{

	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}


	static int busyflag = 0;
	if( busyflag ){
		DbgOut( "mothandler : SetNewPose : busyflag error !!!\n" );
		_ASSERT( 0 );
		busyflag = 0;
		return 0;
	}

	busyflag = 1;

	int i, ret;//, nextno;

	InitTexAnim( lpsh );
	InitAlpAnim( lpsh );
	InitDSAnim( lpsh );


	CMotionCtrl* mc;
	for( i = 0; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){
			CMotionInfo* mi;
			mi = mc->motinfo;
			if( mi ){
				int curmotkind;
				curmotkind = mi->kindno;
				if( (mi->kindnum <= 0 ) || (curmotkind < 0) ){
					busyflag = 0;
					return 0;
				}

				NEXTMOTION nextmot;
				mc->GetNextMotion( &nextmot );
				if( nextmot.mk != curmotkind ){
					//モーションの切り替え
					ret = SetMotionFrameNoML( lpsh->m_mathead, mc->serialno, nextmot.mk, nextmot.aftframeno, 0 );// !!!!!!!!! srcisfirst は　０　！！！！
					if( ret ){
						DbgOut( "mothandler : SetNewPoseML : SetMotionFrameNoML error !!!\n" );
						_ASSERT( 0 );
						busyflag = 0;
						return 1;
					}
				}else{
					ret = mc->StepMotion( lpsh->m_mathead, nextmot.aftframeno );
					if( ret ){
						DbgOut( "CMotHandler : SetNewPose : StepMotion error !!!\n" );
						_ASSERT( 0 );
						busyflag = 0;
						return 0;
					}
				}
			}
		}
	}

	CalcMLMotion( lpsh );

	busyflag = 0;

	return 0;
}

int CMotHandler::CalcMLMotion( CShdHandler* lpsh )
{
	int firstseri = 1;
	int broflag = 1;
	InitMLMatReq( firstseri, broflag );

	CalcMLMatReq( lpsh, firstseri, broflag );

	return 0;
}


void CMotHandler::InitMLMatReq( int seri, int broflag )
{

	CMotionCtrl* mc;
	mc = (*this)( seri );

	if( mc->IsJoint() && (mc->type != SHDMORPH) ){
		CMotionInfo* mi;
		mi = mc->motinfo;
		if( mi ){
			mi->mlmat.Identity();
		}
	}

	if( broflag && mc->brother ){
		InitMLMatReq( mc->brother->serialno, 1 );
	}
	if( mc->child ){
		InitMLMatReq( mc->child->serialno, 1 );
	}
}

void CMotHandler::CalcMLMatReq( CShdHandler* lpsh, int seri, int broflag )
{
	int ret;
	CMotionCtrl* mc;
	mc = (*this)( seri );

	if( mc->IsJoint() && (mc->type != SHDMORPH) ){
		CShdElem* curse;
		curse = (*lpsh)( seri );
		_ASSERT( curse );

		CShdElem* parse;
		parse = lpsh->FindUpperJoint( curse, 0 );

		CMotionCtrl* parmc;
		if( parse ){
			parmc = (*this)( parse->serialno );
		}else{
			parmc = 0;
		}

		ret = mc->CalcMLMat( parmc );
		if( ret ){
			DbgOut( "mh : CalcMLMatReq : mc CalcMLMat error !!!\n" );
			_ASSERT( 0 );
			//return;
		}
	}

	if( broflag && mc->brother ){
		CalcMLMatReq( lpsh, mc->brother->serialno, 1 );
	}
	if( mc->child ){
		CalcMLMatReq( lpsh, mc->child->serialno, 1 );
	}
}




int CMotHandler::SetNewPose( CShdHandler* lpsh )
{
	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}


	static int busyflag = 0;
	static int savenextno = 0;

	if( busyflag ){
		DbgOut( "mothandler : SetNewPose : busyflag error !!!\n" );
		_ASSERT( 0 );
		//return -1;
		busyflag = 0;
		return savenextno;
	}

	busyflag = 1;

	int i, ret;//, nextno;

	InitTexAnim( lpsh );
	InitAlpAnim( lpsh );
	InitDSAnim( lpsh );


	CMotionCtrl* mc;
	CMotionCtrl* firstjoint = 0;
	for( i = 0; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( mc->IsJoint() && (mc->type != SHDMORPH) ){
			firstjoint = mc;
			break;
		}
	}

	if( !firstjoint ){
		busyflag = 0;
		return 0;
	}

	int curmotkind;
	CMotionInfo* minfo;
	minfo = firstjoint->motinfo;
	if( !minfo ){
		DbgOut( "MotHandler : SetNewPose : minfo NULL error !!!\n" );
		_ASSERT( 0 );
		busyflag = 0;
		return 0;
	}
	curmotkind = minfo->kindno;
	if( (minfo->kindnum <= 0 ) || (curmotkind < 0) ){
		busyflag = 0;
		return 0;
	}

	//nextno = firstjoint->GetNextNo();
	NEXTMOTION nextmot;
	firstjoint->GetNextMotion( &nextmot );
	if( nextmot.mk != curmotkind ){
//モーションの切り替え
		ret = SetMotionFrameNo( lpsh, nextmot.mk, nextmot.aftframeno, 0 );// !!!!!!!!! srcisfirst は　０　！！！！
		if( ret ){
			DbgOut( "mothandler : SetNewPose : SetMotionFrameNo error !!!\n" );
			_ASSERT( 0 );
			busyflag = 0;
			return 1;
		}
		busyflag = 0;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}


	//CShdElem* selem;
	for( i = 1; i < s2mot_leng; i++ ){
		mc = (*this)( i );
		if( (i == 1) || mc->IsJoint() ){
			ret = mc->StepMotion( lpsh->m_mathead, nextmot.aftframeno );
			
			//ret = mc->SetCurrentMotion( m_curmotkind, nextno );

			if( ret ){
				DbgOut( "CMotHandler : SetNewPose : StepMotion error !!!\n" );
				_ASSERT( 0 );
				busyflag = 0;
				return 0;
			}
		}
		//////
		/***
		if( lpsh->m_inRDBflag == 0 ){
			selem = (*lpsh)( i );
			if( (selem->type == SHDPOLYMESH) || (selem->type == SHDPOLYMESH2) ){
				ret = selem->SetCurrentBSphere( nextmot.mk, nextmot.frameno );
				if( ret ){
					DbgOut( "CMotHandler : SetNewPose : SetCurrentBSphere error !!!\n" );
					_ASSERT( 0 );
					busyflag = 0;
					return 0;
				}
			}
		}
		***/
	}

	/***
	if( lpsh->m_inRDBflag == 0 ){
		ret = SetCurrentBSphere( nextmot.mk, nextmot.frameno );
		if( ret ){
			DbgOut( "CMotHandler : SetNewPose : mothandler : SetCurrentBSphere error !!!\n" );
			_ASSERT( 0 );
			busyflag = 0;
			return 0;
		}
	}
	***/

	savenextno = nextmot.aftframeno;

	busyflag = 0;

	m_curframeno = nextmot.aftframeno;//!!!!!!

	return nextmot.aftframeno;
}
/***
int CMotHandler::LoadMotData( char* fname, char* motname, int* framemax, int* mottype )
{
	CMotFile* mfile = 0;
	MOTFILEHDR header;
	int ret, i;
	int retcookie = -1;
	CMatrix2* matptr = 0;
	CMotionCtrl* mctrl = 0;

	MOTFILEMARKER mmarker;
	int findend = 0;
	float* valptr = 0;
	int serino, valnum;

	mfile = new CMotFile();
	if( !mfile ){
		DbgOut( "CMotHandler : LoadMotData : alloc CMotFile error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	ret = mfile->OpenMotFile( fname );
	if( ret ){
		DbgOut( "CMotHandler : LoadMotData : mfile->OpenMotFile error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	
	ret = mfile->ReadHeader( &header );
	if( ret ){
		DbgOut( "CMotHandler : LoadMotData : mfile->ReadHeader error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	if( header.magicno != MOTFILEMAGICNO ){
		DbgOut( "CMotHandler : LoadMotData : magicno error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	*framemax = header.framenum;
	*mottype = header.mottype;
	strcpy( motname, header.motname );

	retcookie = AddMotion( header.motname, header.mottype, header.framenum );
	if( retcookie < 0 ){
		DbgOut( "CMotHandler : LoadMotData : AddMotion error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	matptr = new CMatrix2[ header.framenum ];
	if( !matptr ){
		DbgOut( "CMotHandler : LoadMotData : alloc matptr error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}

	
	for( i = 1; i < s2mot_leng; i++ ){
		ZeroMemory( matptr, sizeof( CMatrix2 ) * header.framenum );
		ret = mfile->ReadData( matptr, header.framenum );
		if( ret ){
			DbgOut( "CMotHandler : LoadMotData : mfile->ReadData error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}

		mctrl = (*this)( i );
		if( !mctrl )
			return -1;

		ret = mctrl->CopyMotionMatrix( matptr, retcookie, header.framenum );
		if( ret ){
			DbgOut( "CMotHandler : LoadMotData : mctrl->CopyMotionMatrix error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}
	}
	if( matptr )
		delete [] matptr;

// MORPH JOINT
	while( !findend ){
		ZeroMemory( &mmarker, sizeof( MOTFILEMARKER ) );
		ret = mfile->ReadMarker( &mmarker );
		switch( mmarker.magicno ){
		case MOTMARKER_END:
			findend = 1;
			break;
		case MOTMARKER_VAL:
			serino = mmarker.serialno;
			valnum = mmarker.valnum;
			break;
		default:
			_ASSERT( 0 );
			break;
		}

		if( !findend ){
			mctrl = (*this)( serino );
			valptr = (float*)malloc( sizeof( float ) * valnum );
			if( !valptr ){
				_ASSERT( 0 );
				return -1;
			}
			ret = mfile->ReadData( valptr, valnum );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}

			ret = mctrl->CopyJointVal( valptr, retcookie, valnum );
			if( ret ){
				_ASSERT( 0 );
				return -1;
			}
			free( valptr );
			valptr = 0;
		}
	}


	if( mfile )
		delete mfile;

	return retcookie;
}
***/

int CMotHandler::GetLeapRoute( int serialno, int* leaproute, int leapmax )
{
	int leapcnt = 0;
	int curseri, parseri;
	CMotionCtrl* curmctrl = 0;
	CMotionCtrl* parmctrl = 0;

	curseri = serialno;
	*leaproute = serialno;

	while( leapcnt < leapmax ){
		curmctrl = (*this)( curseri );
		parmctrl = curmctrl->parent;
		parseri = parmctrl->serialno;

		leapcnt++;
		*(leaproute + leapcnt) = parseri;

		curseri = parmctrl->serialno;
	}
	return 0;
}

int CMotHandler::MakeLeapRoute( int serialno, int* skip2leap, int* leaproute, int skipmax )
{
	int skipcnt = 0;
	int leapcnt = 0;
	int curseri, parseri;
	CMotionCtrl* curmctrl = 0;
	CMotionCtrl* parmctrl = 0;

	curseri = serialno;
	*leaproute = serialno;
	*skip2leap = 0;
	while( skipcnt < skipmax ){
		curmctrl = (*this)( curseri );
		parmctrl = curmctrl->parent;
		parseri = parmctrl->serialno;

		leapcnt++;
		*(leaproute + leapcnt) = parseri;

		if( curmctrl->hasmotion ){
			skipcnt++;
			*(skip2leap + skipcnt) = leapcnt;
		}

		curseri = parmctrl->serialno;
	}


	return 0;
}

int CMotHandler::PrepMotionCtrl( CShdHandler* lpsh )
{
	int ret, i, j, k, leng;
	int linenum, bmseri, blseri, skip, skipmax, leap, leapmax;
	CShdElem* selem = 0;
	CMeshInfo* mi = 0;
	CBezMesh* bm = 0;
	CBezLine* bl = 0;

	// hasmotion == 1 のelemのみskip数に数える。

	int skip2leap[SKIPMAX]; // skip--->leap への変換表
	int leaproute[SKIPMAX]; // leaproute[i] は i個 motionを持つelemをskipした時のserialno
	CMatrix2 leapmat[SKIPMAX];// leaprouteに対応するcurmatを入れる。
	CMatrix2 partmat[SKIPMAX];

	CMatrix2 multmat;

	leng = lpsh->s2shd_leng;
	for( i = 1; i < leng; i++ ){
		selem = (*lpsh)( i );
		mi = selem->GetMeshInfo();
		if( !mi ){
			_ASSERT( 0 );
			return 1;
		}

		if( mi->type == SHDBEZIERSURF ){
			bmseri = selem->serialno;
			bm = selem->bmesh;
			_ASSERT( bm );
			linenum = mi->n;
			
			skipmax = bm->GetSkipMax();
			_ASSERT( skipmax >= 0 );

			if( skipmax > 0 ){
				ZeroMemory( skip2leap, sizeof(int) * SKIPMAX );
				ZeroMemory( leaproute, sizeof(int) * SKIPMAX );

				ret = MakeLeapRoute( bmseri, skip2leap, leaproute, skipmax );
				_ASSERT( !ret );
				leapmax = skip2leap[skipmax];


				ret = GetCurMatrix( leapmat, leaproute, leapmax );
				_ASSERT( !ret );

				ret = GetPartMatrix( partmat, leaproute, leapmax );
				_ASSERT( !ret );

				(*this)( bmseri )->leap = leapmax;//

				for( j = 0; j < linenum; j++ ){
					bl = (*bm)( j );
					skip = bl->meshinfo->skip;
					blseri = *(bm->hblseri + j);
					
					if( skip > 0 ){
						leap = skip2leap[skip];
						(*this)( blseri )->leap = leap;//

						multmat.Identity();
						for( k = 0; k < leap; k++ ){
							multmat *= partmat[k];
						}
						multmat *= leapmat[leap];

						ret = SetCurMatrix( blseri, multmat );
						_ASSERT( !ret );
					}
				}
			}
		}
	}
	return 0;
}

int CMotHandler::CopyChainFromShd( CShdHandler* lpsh )
{
	int ctrlno;
	
	if( lpsh->s2shd_leng != s2mot_leng )
		return 1;

	for( ctrlno = 0; ctrlno < s2mot_leng; ctrlno++ )
	{
		CShdElem* curshd = (*lpsh)( ctrlno );
		CMotionCtrl* curctrl = (*this)( ctrlno );

		CShdElem *parshd, *chilshd, *broshd, *sisshd;
		int parno, chilno, brono, sisno;

		parshd = curshd->parent;
		if( parshd )
			parno = parshd->serialno;
		else
			parno = -1;
		chilshd = curshd->child;
		if( chilshd )
			chilno = chilshd->serialno;
		else
			chilno = -1;
		broshd = curshd->brother;
		if( broshd )
			brono = broshd->serialno;
		else
			brono = -1;
		sisshd = curshd->sister;
		if( sisshd )
			sisno = sisshd->serialno;
		else
			sisno = -1;
		
		////////
		if( parno >= 0 )
			curctrl->parent = (*this)( parno );
		else
			curctrl->parent = 0;

		if( chilno >= 0 )
			curctrl->child = (*this)( chilno );
		else
			curctrl->child = 0;

		if( brono >= 0 )
			curctrl->brother = (*this)( brono );
		else
			curctrl->brother = 0;

		if( sisno >= 0 )
			curctrl->sister = (*this)( sisno );
		else
			curctrl->sister = 0;

	}
	return 0;
}

int CMotHandler::CalcDepth()
{
	CMotionCtrl* startctrl;
	startctrl = (*this)( 1 );

	startctrl->CalcDepthReq( 1 );

	return 0;
}

int CMotHandler::MakeRawmat( int mcookie, CShdHandler* lpsh, int startframe, int endframe, int calceulflag )
{
	int ret;
	int frameleng;
	ret = GetMotionFrameLength( mcookie, &frameleng );
	if( ret ){
		DbgOut( "mothandler : MakeRawmat : GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( frameleng <= 0 ){
		DbgOut( "mothandler : MakeRawmat : frameleng 0 warning exit !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!
	}

	if( endframe < 0 ){
		endframe = frameleng - 1;
	}
	if( endframe >= frameleng ){
		endframe = frameleng - 1;
	}

	if( startframe < 0 ){
		startframe = 0;
	}
	if( startframe > endframe ){
		startframe = endframe;
	}

//DbgOut( "mothandler : makerawmat : frameleng %d, startframe %d, endframe %d\n", frameleng, startframe, endframe );

	//全boneのrawmatセット
	int i;
	for( i = 1; i < s2mot_leng; i++ ){
		CMotionCtrl* mcptr = (*this)( i );
		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			CShdElem* selem = (*lpsh)( i );
			CPart* partptr = selem->part;
			if( !partptr ){
				_ASSERT( 0 );
				return 1;
			}
			//int bonenum = partptr->bonenum;
			//if( bonenum > 0 ){
				int ret;
				ret = mcptr->MakeRawmat( mcookie, selem, startframe, endframe, lpsh, this, calceulflag );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			//}
		}
	}
	return 0;
}

void CMotHandler::MakeRawmatReq( int mcookie, CShdElem* lpselem, int srcstart, int srcend, int broflag, CShdHandler* lpsh, int calceulflag )
{
	int ret;
	int serino = lpselem->serialno;
	CMotionCtrl* mcptr = (*this)( serino );
	if( (mcptr->IsJoint()) && (mcptr->type != SHDMORPH) ){

		ret = MakeRawmat( mcookie, lpselem, srcstart, srcend, lpsh, calceulflag );
		if( ret ){
			DbgOut( "mothandler : MakeRawmatReq : MakeRawmat error !!!\n" );
			_ASSERT( 0 );
			return;
		}
	}

	if( lpselem->child ){
		MakeRawmatReq( mcookie, lpselem->child, srcstart, srcend, 1, lpsh, calceulflag );
	}

	if( broflag && lpselem->brother ){
		MakeRawmatReq( mcookie, lpselem->brother, srcstart, srcend, 1, lpsh, calceulflag );
	}

}

int CMotHandler::MakeRawmat( int mcookie, CShdElem* lpselem, int startframe, int endframe, CShdHandler* lpsh, int calceulflag )
{
	//指定boneのrawmatセット
	int serino = lpselem->serialno;
	CMotionCtrl* mcptr = (*this)( serino );
	if( !(mcptr->IsJoint()) || (mcptr->type == SHDMORPH) ){
		DbgOut( "MotHandler : MakeRawmat : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//CPart* partptr = lpselem->part;
	//if( !partptr ){
	//	_ASSERT( 0 );
	//	return 1;
	//}
	
	//int bonenum = partptr->bonenum;
	//if( bonenum <= 0 ){
	//	DbgOut( "MotHandler : MakeRawmat : bonenum errorr !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

	int ret;
	int frameleng;
	ret = GetMotionFrameLength( mcookie, &frameleng );
	if( ret ){
		DbgOut( "mothandler : MakeRawmat : GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( frameleng <= 0 ){
		DbgOut( "mothandler : MakeRawmat : frameleng 0 warning exit !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!
	}

	if( endframe < 0 ){
		endframe = frameleng - 1;
	}
	if( endframe >= frameleng ){
		endframe = frameleng - 1;
	}

	if( startframe < 0 ){
		startframe = 0;
	}
	if( startframe > endframe ){
		startframe = endframe;
	}


	ret = mcptr->MakeRawmat( mcookie, lpselem, startframe, endframe, lpsh, this, calceulflag );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
/***
	if( m_zakind != ZA_1 ){
		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			CMotionInfo* mi;
			mi = mcptr->motinfo;
			if( mi ){
				ret = mi->CalcMPEuler( this, lpsh, lpselem, mcookie, m_zakind );
				if( ret ){
					DbgOut( "mh MakeRawmat : mi CalcMPEuler error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}
	}
***/
	return ret;
}

int CMotHandler::MakeRawmatCap( int mcookie, CShdElem* lpselem, int srcframe, CShdHandler* lpsh, CMotionPoint2* srcmp )
{
	//指定boneのrawmatセット
	int serino = lpselem->serialno;
	CMotionCtrl* mcptr = (*this)( serino );
	if( !(mcptr->IsJoint()) || (mcptr->type == SHDMORPH) ){
		DbgOut( "MotHandler : MakeRawmat : type error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int frameleng;
	ret = GetMotionFrameLength( mcookie, &frameleng );
	if( ret ){
		DbgOut( "mothandler : MakeRawmat : GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameleng <= 0) || (frameleng <= srcframe) ){
		DbgOut( "mothandler : MakeRawmat : frameleng 0 warning exit !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!
	}

	ret = mcptr->MakeRawmatCap( mcookie, lpselem, srcframe, lpsh, this, srcmp );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	return 0;
}



int CMotHandler::RawmatToMotionmat( int mcookie, CShdHandler* lpsh, int startframe, int endframe )
{
	int ret;
	int frameleng;
	ret = GetMotionFrameLength( mcookie, &frameleng );
	if( ret ){
		DbgOut( "mothandler : RawmatToMotionmat : GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( frameleng <= 0 ){
		DbgOut( "mothandler : RawmatToMotionmat : frameleng 0 warning !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!!!
	}

	if( endframe < 0 ){
		endframe = frameleng - 1;
	}
	if( endframe >= frameleng ){
		endframe = frameleng - 1;
	}

	if( startframe < 0 ){
		startframe = 0;
	}
	if( startframe > endframe ){
		startframe = endframe;
	}

	int errorflag = 0;
	RawmatToMotionmatReq( mcookie, lpsh, 1, &errorflag, startframe, endframe );

	return errorflag;
}

void CMotHandler::RawmatToMotionmatReq( int mcookie, CShdHandler* lpsh, int srcseri, int* errorflag, int startframe, int endframe, int addbroflag )
{
	//2004/4/23

	CMotionCtrl* mcptr = (*this)( srcseri );

	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){				
		CShdElem* selem;
		selem = (*lpsh)( srcseri );
		_ASSERT( selem );

		CShdElem* parselem;
		parselem = lpsh->FindUpperJoint( selem, 0 );

		CMotionCtrl* parmc;

		if( parselem ){
			parmc = (*this)( parselem->serialno );
			_ASSERT( parmc );
		}else{
			parmc = 0;
		}

		int ret;
		int frameleng;
		ret = GetMotionFrameLength( mcookie, &frameleng );
		if( ret ){
			DbgOut( "mothandler : RawmatToMotionmatReq : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			(*errorflag)++;
			return;
		}

		if( frameleng <= 0 ){
			DbgOut( "mothandler : RawmatToMotionmatReq : frameleng 0 warning !!!\n" );
			_ASSERT( 0 );
		}else{

			if( endframe < 0 ){
				endframe = frameleng - 1;
			}
			if( endframe >= frameleng ){
				endframe = frameleng - 1;
			}

			if( startframe < 0 ){
				startframe = 0;
			}
			if( startframe > endframe ){
				startframe = endframe;
			}

				
			ret = mcptr->RawmatToMotionmat( mcookie, parmc, startframe, endframe );
			if( ret ){
				(*errorflag)++;
				_ASSERT( 0 );
				return;
			}
		}
	}

	if( addbroflag == 1 ){
		CMotionCtrl* bromc = mcptr->brother;
		if( bromc ){
			RawmatToMotionmatReq( mcookie, lpsh, bromc->serialno, errorflag, startframe, endframe );
		}
	}

	CMotionCtrl* chilmc = mcptr->child;
	if( chilmc ){
		RawmatToMotionmatReq( mcookie, lpsh, chilmc->serialno, errorflag, startframe, endframe );
	}
}
void CMotHandler::RawmatToMotionmatReqCap( int mcookie, CShdHandler* lpsh, int srcseri, int* errorflag, int srcframe, int addbroflag )
{
	CMotionCtrl* mcptr = (*this)( srcseri );

	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){				
		CShdElem* selem;
		selem = (*lpsh)( srcseri );
		_ASSERT( selem );

		CShdElem* parselem;
		parselem = lpsh->FindUpperJoint( selem, 0 );

		CMotionCtrl* parmc;

		if( parselem ){
			parmc = (*this)( parselem->serialno );
			_ASSERT( parmc );
		}else{
			parmc = 0;
		}

		int ret;
		int frameleng;
		ret = GetMotionFrameLength( mcookie, &frameleng );
		if( ret ){
			DbgOut( "mothandler : RawmatToMotionmatReq : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			(*errorflag)++;
			return;
		}

		if( frameleng <= 0 ){
			DbgOut( "mothandler : RawmatToMotionmatReq : frameleng 0 warning !!!\n" );
			_ASSERT( 0 );
		}else{
			ret = mcptr->RawmatToMotionmatCap( mcookie, parmc, srcframe );
			if( ret ){
				(*errorflag)++;
				_ASSERT( 0 );
				return;
			}
		}
	}

	if( addbroflag == 1 ){
		CMotionCtrl* bromc = mcptr->brother;
		if( bromc ){
			RawmatToMotionmatReqCap( mcookie, lpsh, bromc->serialno, errorflag, srcframe );
		}
	}

	CMotionCtrl* chilmc = mcptr->child;
	if( chilmc ){
		RawmatToMotionmatReqCap( mcookie, lpsh, chilmc->serialno, errorflag, srcframe );
	}
}


int CMotHandler::DestroyMotionObj( int delcookie )
{
	int ret = 0;
	int i;
	for( i = 0; i < s2mot_leng; i++ ){
		CMotionCtrl* mcptr = (*this)( i );
		ret = mcptr->DestroyMotionObj( delcookie );
		if( ret ){
			DbgOut( "mh : DestroyMotionObj : mc DestroyMotionObj error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

////////

	if( delcookie == -1 ){
		if( m_motnum ){
			free( m_motnum );
			m_motnum = 0;
		}
		if( m_zatype ){
			free( m_zatype );
			m_zatype = 0;
		}
		if( m_definterp ){
			free( m_definterp );
			m_definterp = 0;
		}

		if( m_firstbs ){
			int bsno;
			for( bsno = 0; bsno < m_kindnum; bsno++ ){
				CBSphere* delbs;
				delbs = *( m_firstbs + bsno );
				delete [] delbs;
			}
			free( m_firstbs );
			m_firstbs = 0;
		}
		m_kindnum = 0;
		m_fuid = 0;
	}else{
		if( delcookie >= m_kindnum ){
			DbgOut( "mh : DestroyMotionObj select : delcookie error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		int newkindnum = m_kindnum - 1;

		int* newmotnum;
		newmotnum = (int*)malloc( sizeof( int ) * newkindnum );
		if( !newmotnum ){
			DbgOut( "mh : DestroyMotionObj : newmotnum alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int mindex;
		int setno = 0;
		for( mindex = 0; mindex < m_kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newmotnum + setno) = *(m_motnum + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
	
		free( m_motnum );
		m_motnum = newmotnum;
		////////////
		int* newzatype;
		newzatype = (int*)malloc( sizeof( int ) * newkindnum );
		if( !newzatype ){
			DbgOut( "mh : DestroyMotionObj : newzatype alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < m_kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newzatype + setno) = *(m_zatype + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
	
		free( m_zatype );
		m_zatype = newzatype;

		////////////
		int* newinterp;
		newinterp = (int*)malloc( sizeof( int ) * newkindnum );
		if( !newinterp ){
			DbgOut( "mh : DestroyMotionObj : newinterp alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		setno = 0;
		for( mindex = 0; mindex < m_kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newinterp + setno) = *(m_definterp + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );

		free( m_definterp );
		m_definterp = newinterp;


		///////////
		CBSphere** newbs;
		newbs = (CBSphere**)malloc( sizeof( CBSphere* ) * newkindnum );
		if( !newbs ){
			DbgOut( "mh : DestroyMotionObj : newbs alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < m_kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newbs + setno) = *(m_firstbs + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );

		delete [] *(m_firstbs + delcookie);
		free( m_firstbs );
		m_firstbs = newbs;


		////////////
		m_kindnum = newkindnum;

	}

///

	return ret;
}

int CMotHandler::SetCurrentBSphere( int motkind, int frameno )
{
	if( !m_firstbs ){
		DbgOut( "mothandler : SetCurrentBSphere : firstbs NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( motkind >= m_kindnum ){
		DbgOut( "mothandler : SetCurrentBSphere : motkind out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( frameno >= *(m_motnum + motkind) ){
		DbgOut( "mothandler : SetCurrentBSphere : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_curbs = *( *( m_firstbs + motkind ) + frameno );


	return 0;
}

int CMotHandler::ChkConfTotalBySphere( CMotHandler* chkmh, int* confflag )
{
	CBSphere* chktotalbs;
	chktotalbs = &(chkmh->m_curbs);

	int ret;
	ret = m_curbs.ChkConflict( chktotalbs, confflag );
	if( ret ){
		DbgOut( "mothandler : ChkConfTotalBySphere : totalbs ChkConflict error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CMotHandler::GetNextMP( int motid, int boneno, int prevmpid, int* mpidptr )
{
	//CMotionPoint2* curmp = mcptr->IsExistMotionPoint( m_mot_cookie, m_current );
	//CMotionPoint2* mpptr = mcptr->GetMotionPoint( m_mot_cookie );

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetNextMP : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : GetNextMP : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( prevmpid >= 0 ){

		CMotionPoint2* mpptr;
		mpptr = mcptr->GetMotionPoint2( motid, prevmpid );
		if( !mpptr ){
			DbgOut( "mothandler : GetNextMP : prevmpid error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMotionPoint2* nextmp;
		nextmp = mpptr->next;

		if( nextmp ){
			*mpidptr = nextmp->serialno;
		}else{
			*mpidptr = -1;
		}
	}else{
		CMotionPoint2* mpptr;
		mpptr = mcptr->GetMotionPoint( motid );
		
		if( mpptr ){
			*mpidptr = mpptr->serialno;
		}else{
			*mpidptr = -1;
		}

	}

	return 0;
}
int CMotHandler::GetCurrentMotionPoint( CShdHandler* srclpsh, int boneno, CMotionPoint2* dstmp, int* hasmpflag )
{
	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetCurrentMotionPoint : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////
	int ret;
	CMotionCtrl* curmc;
	CMotionInfo* curmi;

	curmc = (*this)( boneno );
	curmi = curmc->motinfo;
	if( !curmi ){
		DbgOut( "mothandler : GetCurrentMotionPoint : curmi NULL error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}
		
	CShdElem* selem;
	selem = (*srclpsh)( boneno );
	_ASSERT( selem );

	ret = SetBoneAxisQ( srclpsh, selem->serialno, GetZaType( curmi->kindno ), curmi->kindno, curmi->motno );
	_ASSERT( !ret );
	CQuaternion axisq;
	ret = selem->GetBoneAxisQ( &axisq );
	_ASSERT( !ret );


	ret = curmi->CalcMotionPointOnFrame( &axisq, selem, dstmp, curmi->kindno, curmi->motno, hasmpflag );
	if( ret ){
		DbgOut( "mothandler : GetCurrentMotionPoint : CalcMotionPointOnFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



int CMotHandler::GetCurrentBoneQ( CShdHandler* lpsh, int boneno, int kind, CQuaternion* dstq )
{
	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetCurrentBoneQ : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////

	int ret;
	if( kind != 0 ){
		ret = GetTotalQOnFrame( lpsh, -1, 0, boneno, dstq );
		if( ret ){
			DbgOut( "mothandler : GetBoneQ : GetTotalQOnFrame error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		CMotionCtrl* curmc;
		CMotionInfo* curmi;

		curmc = (*this)( boneno );
		curmi = curmc->motinfo;

		if( !curmi ){
			DbgOut( "mothandler : GetBoneQ : curmi NULL error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}
		

		CShdElem* selem;
		selem = (*lpsh)( boneno );
		_ASSERT( selem );

		ret = SetBoneAxisQ( lpsh, selem->serialno, GetZaType( curmi->kindno ), curmi->kindno, curmi->motno );
		_ASSERT( !ret );
		CQuaternion axisq;
		ret = selem->GetBoneAxisQ( &axisq );
		_ASSERT( !ret );


		int hasmpflag = 0;
		CMotionPoint2 calcmp;
		ret = curmi->CalcMotionPointOnFrame( &axisq, selem, &calcmp, curmi->kindno, curmi->motno, &hasmpflag );
		if( ret ){
			DbgOut( "mothandler : GetBoneQ : CalcMotionPointOnFrame error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*dstq = calcmp.m_q;
	}

	return 0;

}


int CMotHandler::GetBoneQ( CShdHandler* lpsh, int boneno, int motid, int frameno, int kind, CQuaternion* dstq )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetBoneQ : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetBoneQ : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetBoneQ : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : GetBoneQ : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////

	int ret;
	if( kind != 0 ){
		ret = GetTotalQOnFrame( lpsh, motid, frameno, boneno, dstq );
		if( ret ){
			DbgOut( "mothandler : GetBoneQ : GetTotalQOnFrame error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		CMotionCtrl* curmc;
		CMotionInfo* curmi;

		curmc = (*this)( boneno );
		curmi = curmc->motinfo;

		if( !curmi ){
			DbgOut( "mothandler : GetBoneQ : curmi NULL error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}
		
		CShdElem* selem;
		selem = (*lpsh)( boneno );
		_ASSERT( selem );

		ret = SetBoneAxisQ( lpsh, selem->serialno, GetZaType( curmi->kindno ), curmi->kindno, curmi->motno );
		_ASSERT( !ret );
		CQuaternion axisq;
		ret = selem->GetBoneAxisQ( &axisq );
		_ASSERT( !ret );

		int hasmpflag = 0;
		CMotionPoint2 calcmp;
		ret = curmi->CalcMotionPointOnFrame( &axisq, selem, &calcmp, motid, frameno, &hasmpflag );
		if( ret ){
			DbgOut( "mothandler : GetBoneQ : CalcMotionPointOnFrame error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*dstq = calcmp.m_q;
	}

	return 0;
}


int CMotHandler::GetMPInfo( int motid, int boneno, int mpid, CQuaternion* dstq, D3DXVECTOR3* dsttra, int* dstframeno, DWORD* dstds, int* dstinterp, D3DXVECTOR3* dstscale, int* dstuserint1 )
{
	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetMPInfo : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : GetMPInfo : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* mpptr;
	mpptr = mcptr->GetMotionPoint2( motid, mpid );
	if( !mpptr ){
		DbgOut( "mothandler : GetMPInfo : mpid error %d!!!\n", mpid );
		_ASSERT( 0 );
		return 1;
	}

	*dstq = mpptr->m_q;
	dsttra->x = mpptr->m_mvx;
	dsttra->y = mpptr->m_mvy;
	dsttra->z = mpptr->m_mvz;

	*dstframeno = mpptr->m_frameno;
	*dstds = mpptr->dispswitch;
	*dstinterp = mpptr->interp;

	dstscale->x = mpptr->m_scalex;
	dstscale->y = mpptr->m_scaley;
	dstscale->z = mpptr->m_scalez;

	*dstuserint1 = mpptr->m_userint1;

	return 0;
}

int CMotHandler::SetMPInfo( int motid, int boneno, CQuaternion* srcqptr, MPINFO2* mpiptr, int* flagptr )
{
	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : SetMPInfo : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : SetMPInfo : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* mpptr;
	mpptr = mcptr->GetMotionPoint2( motid, mpiptr->mpid );
	if( !mpptr ){
		DbgOut( "mothandler : SetMPInfo : prevmpid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	if( (*( flagptr + MPI2_FRAMENO) == 1) && (mpptr->m_frameno != mpiptr->frameno) ){
		//frameno
		int framenum;
		ret = GetMotionFrameLength( motid, &framenum );
		if( ret ){
			DbgOut( "mothandler : SetMPInfo : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}	

		if( (mpiptr->frameno < 0) || (mpiptr->frameno >= framenum) ){
			DbgOut( "mothandler : SetMPInfo : srcframeno range error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		CMotionPoint2 tempmp;
		tempmp = *mpptr;

		ret = mcptr->DeleteMotionPoint( motid, mpptr->m_frameno );
		if( ret ){
			DbgOut( "mothandler : SetMPInfo : DeleteMotionPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMotionPoint2* newmp;
		newmp = mcptr->AddMotionPoint( motid, mpiptr->frameno,
			tempmp.m_q,
			tempmp.m_mvx, tempmp.m_mvy, tempmp.m_mvz, tempmp.dispswitch, tempmp.interp, tempmp.m_scalex, tempmp.m_scaley, tempmp.m_scalez, tempmp.m_userint1 );
		if( !newmp ){
			DbgOut( "mothandler : SetMPInfo : AddMotionPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		newmp->serialno = mpiptr->mpid;//!!!!!!!!!!!!

		mpptr = newmp;//!!!!!!!!!!!!!!!!!!!!!!

	}

	if( *(flagptr + MPI2_QUA) == 1 ){
		//q
		mpptr->m_q = *srcqptr;
	}

	if( *( flagptr + MPI2_TRA) == 1 ){
		mpptr->m_mvx = mpiptr->tra.x;
		mpptr->m_mvy = mpiptr->tra.y;
		mpptr->m_mvz = mpiptr->tra.z;

	}

	if( *( flagptr + MPI2_INTERP) == 1 ){
		mpptr->interp = mpiptr->interpolation;
	}

	if( *( flagptr + MPI2_SCALE) == 1 ){
		mpptr->m_scalex = mpiptr->scale.x;
		mpptr->m_scaley = mpiptr->scale.y;
		mpptr->m_scalez = mpiptr->scale.z;
	}
	if( *( flagptr + MPI2_USERINT1) == 1 ){
		mpptr->m_userint1 = mpiptr->userint1;
	}

	return 0;
}


int CMotHandler::SetMPInfo( int motid, int boneno, int mpid, CQuaternion* srcqptr, D3DXVECTOR3* srctra, int srcframeno, DWORD srcds, int srcinterp, D3DXVECTOR3* srcscale, int srcuserint1, int* infoflagptr )
{
	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : SetMPInfo : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : SetMPInfo : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* mpptr;
	mpptr = mcptr->GetMotionPoint2( motid, mpid );
	if( !mpptr ){
		DbgOut( "mothandler : SetMPInfo : prevmpid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	if( (*( infoflagptr + MPI_FRAMENO) == 1) && (mpptr->m_frameno != srcframeno) ){
		//frameno
		int framenum;
		ret = GetMotionFrameLength( motid, &framenum );
		if( ret ){
			DbgOut( "mothandler : SetMPInfo : GetMotionFrameLength error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}	

		if( (srcframeno < 0) || (srcframeno >= framenum) ){
			DbgOut( "mothandler : SetMPInfo : srcframeno range error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		CMotionPoint2 tempmp;
		tempmp = *mpptr;

		ret = mcptr->DeleteMotionPoint( motid, mpptr->m_frameno );
		if( ret ){
			DbgOut( "mothandler : SetMPInfo : DeleteMotionPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMotionPoint2* newmp;
		newmp = mcptr->AddMotionPoint( motid, srcframeno,
			tempmp.m_q,
			tempmp.m_mvx, tempmp.m_mvy, tempmp.m_mvz, tempmp.dispswitch, tempmp.interp, tempmp.m_scalex, tempmp.m_scaley, tempmp.m_scalez, tempmp.m_userint1 );
		if( !newmp ){
			DbgOut( "mothandler : SetMPInfo : AddMotionPoint error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		newmp->serialno = mpid;//!!!!!!!!!!!!

		mpptr = newmp;//!!!!!!!!!!!!!!!!!!!!!!

	}

/***
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
	MPI_MAX
};
***/

	if( *(infoflagptr + MPI_QUA) == 1 ){
		//q
		mpptr->m_q = *srcqptr;
	}

	if( *( infoflagptr + MPI_TRAX) == 1 ){
		//trax
		mpptr->m_mvx = srctra->x;
	}

	if( *( infoflagptr + MPI_TRAY) == 1 ){
		//tray
		mpptr->m_mvy = srctra->y;

	}

	if( *( infoflagptr + MPI_TRAZ) == 1 ){
		//traz
		mpptr->m_mvz = srctra->z;
	}

	if( *( infoflagptr + MPI_DISPSWITCH) == 1 ){
		//dispswitch
		mpptr->dispswitch = srcds;
	}

	if( *( infoflagptr + MPI_INTERP) == 1 ){
		mpptr->interp = srcinterp;
	}
//////
	if( *( infoflagptr + MPI_SCALEX) == 1 ){
		mpptr->m_scalex = srcscale->x;
	}
	if( *( infoflagptr + MPI_SCALEY) == 1 ){
		mpptr->m_scaley = srcscale->y;
	}
	if( *( infoflagptr + MPI_SCALEZ) == 1 ){
		mpptr->m_scalez = srcscale->z;
	}

//DbgOut( "mothandler : SetMPInfo : %f %f %f\r\n", mpptr->m_scalex, mpptr->m_scaley, mpptr->m_scalez );

	if( *( infoflagptr + MPI_USERINT1) == 1 ){
		mpptr->m_userint1 = srcuserint1;
	}


	return 0;
}

int CMotHandler::IsExistCurrentMotionPoint( int boneno, CMotionPoint2** ppmp )
{
	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : IsExistCurrentMotionPoint : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );
	if( !mcptr ){
		DbgOut( "mothandler : IsExistCurrentMotionPoint : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionInfo* mi;
	mi = mcptr->motinfo;
	if( !mi ){
		DbgOut( "mothandler : IsExistCurrentMotionPoint : mi NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* chkmp = mcptr->IsExistMotionPoint( mi->kindno, mi->motno );

	*ppmp = chkmp;

	return 0;
}


int CMotHandler::IsExistMotionPoint( int motid, int boneno, int frameno, CMotionPoint2** ppmp )
{

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : IsExistMotionPoint : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : IsExistMotionPoint : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* chkmp = mcptr->IsExistMotionPoint( motid, frameno );
	if( chkmp ){
		*ppmp = chkmp;
	}else{
		*ppmp = 0;
	}


	return 0;
}



int CMotHandler::IsExistMotionPoint( int motid, int boneno, int frameno, int* mpidptr )
{
	//CMotionPoint2* curmp = mcptr->IsExistMotionPoint( m_mot_cookie, m_current );
	//CMotionPoint2* mpptr = mcptr->GetMotionPoint( m_mot_cookie );

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : IsExistMotionPoint : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : IsExistMotionPoint : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* chkmp = mcptr->IsExistMotionPoint( motid, frameno );
	if( chkmp ){
		*mpidptr = chkmp->serialno;
	}else{
		*mpidptr = -1;
	}

	return 0;
}


int CMotHandler::GetMotionFrameLength( int motid, int* lengptr )
{

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetMotionFrameLength : motid error !!!\n" );
		_ASSERT( 0 );
		*lengptr = 0;
		return 1;
	}

	*lengptr = *( m_motnum + motid );

	/***
	int i, ret;
	CMotionCtrl* firstjoint = 0;
	CMotionCtrl* curmc;
	for( i = 1; i < s2mot_leng; i++ ){
		curmc = (*this)( i );
		if( curmc->IsJoint() && (curmc->type != SHDMORPH) ){
			firstjoint = curmc;
			break;
		}
	}

	if( firstjoint ){
		ret = firstjoint->GetMotionFrameNum( motid, lengptr );
		if( ret ){
			DbgOut( "mothandler : GetMotionFrameLength : mcptr GetMotionFrameNum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		*lengptr = 0;
	}
	***/

	return 0;
}

int CMotHandler::AddMotionPoint( int motid, int boneno, CQuaternion* srcqptr, D3DXVECTOR3* srctra, D3DXVECTOR3* srcscale, int srcframeno, DWORD srcds, int srcinterp, int srcuserint1, int* mpidptr )
{
	*mpidptr = -1;

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : AddMotionPoint : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : AddMotionPoint : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* chkmp = mcptr->IsExistMotionPoint( motid, srcframeno );
	if( chkmp ){
		DbgOut( "mothandler : AddMotionPoint : motion point is already exist in this frameno error %d!!!\n", srcframeno );
		_ASSERT( 0 );
		return 1;
	}


	CMotionPoint2* newmp;
	newmp = mcptr->AddMotionPoint( motid, srcframeno, *srcqptr, srctra->x, srctra->y, srctra->z, srcds, srcinterp, srcscale->x, srcscale->y, srcscale->z, srcuserint1 );
	if( !newmp ){
		DbgOut( "mothandler : AddMotionPoint : mcptr AddMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*mpidptr = newmp->serialno;

	return 0;
}

int CMotHandler::DeleteMotionPoint( int motid, int boneno, int mpid )
{
	
	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : AddMotionPoint : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );

	if( !mcptr ){
		DbgOut( "mothandler : AddMotionPoint : mcptr error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionPoint2* mpptr;
	mpptr = mcptr->GetMotionPoint2( motid, mpid );
	if( !mpptr ){
		DbgOut( "mothandler : DeleteMotionPoint : mpid not found warning !!!\n" );
		//_ASSERT( 0 );
		//return 1;
		return 0;
	}

	int ret;

	ret = mcptr->DeleteMotionPoint( motid, mpptr->m_frameno );
	if( ret ){
		DbgOut( "mothandler : DeleteMotionPoint : mcptr DeleteMotionPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotHandler::FillUpMotionML( CShdHandler* lpsh, int motid, int boneno, int startframe, int endframe )
{
	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}


	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : FillUpMotionML : motid error %d!!!\n", motid );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int frameleng;
	ret = GetMotionFrameLength( motid, &frameleng );
	if( ret ){
		DbgOut( "mothandler : FillUpMotionML : GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( frameleng <= 0 ){
		DbgOut( "mothandler : FillUpMotionML : frameleng 0 warning exit !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!
	}

	if( endframe < 0 ){
		endframe = frameleng - 1;
	}
	if( endframe >= frameleng ){
		endframe = frameleng - 1;
	}

	if( startframe < 0 ){
		startframe = 0;
	}
	if( startframe > endframe ){
		startframe = endframe;
	}

	if( boneno >= 0 ){
		if( (boneno < 0) || (boneno >= s2mot_leng) ){
			DbgOut( "mothandler : FillUpMotionML : boneno error %d!!!\n", boneno );
			_ASSERT( 0 );
			return 1;
		}

		CMotionCtrl* mcptr;
		mcptr = (*this)( boneno );
		if( !mcptr ){
			DbgOut( "mothandler : FillUpMotionML : mcptr error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*lpsh)( boneno );
		if( !selem ){
			DbgOut( "mothandler : FillUpMotionML : selem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//MakeRawmatReq( motid, selem, startframe, endframe, 0 );

		if( (mcptr->IsJoint()) && (mcptr->type != SHDMORPH) ){
			ret = MakeRawmat( motid, selem, startframe, endframe, lpsh );
			if( ret ){
				DbgOut( "mothandler : FillUpMotionML : MakeRawmat error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}



		//int errorflag = 0;
		//RawmatToMotionmatReq( motid, lpsh, boneno, &errorflag, startframe, endframe, 0 );
		//if( errorflag ){
		//	DbgOut( "mothandler : FillUpMotion : RawmatToMotionmatReq error !!!\n" );
		//	_ASSERT( 0 );
		//	return 1;
		//}
		
	}else{

		ret = MakeRawmat( motid, lpsh, startframe, endframe );
		if( ret ){
			DbgOut( "mothandler : FillUpMotionML : MakeRawmat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//ret = RawmatToMotionmat( motid, lpsh, startframe, endframe );
		//if( ret ){
		//	DbgOut( "mothandler : FillUpMotion : RawmatToMotionmat error !!!\n" );
		//	_ASSERT( 0 );
		//	return 1;
		//}
	}

	return 0;

}


int CMotHandler::FillUpMotion( CShdHandler* lpsh, int motid, int boneno, int startframe, int endframe, int initflag, int calceulflag )
{
	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}


	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : FillUpMotion : motid error %d!!!\n", motid );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int frameleng;
	ret = GetMotionFrameLength( motid, &frameleng );
	if( ret ){
		DbgOut( "mothandler : FillUpMotion : GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( frameleng <= 0 ){
		DbgOut( "mothandler : FillUpMotion : frameleng 0 warning exit !!!\n" );
		_ASSERT( 0 );
		return 0;//!!!!!
	}

	if( endframe < 0 ){
		endframe = frameleng - 1;
	}
	if( endframe >= frameleng ){
		endframe = frameleng - 1;
	}

	if( startframe < 0 ){
		startframe = 0;
	}
	if( startframe > endframe ){
		startframe = endframe;
	}

	if( boneno >= 0 ){
		if( (boneno < 0) || (boneno >= s2mot_leng) ){
			DbgOut( "mothandler : FillUpMotion : boneno error %d!!!\n", boneno );
			_ASSERT( 0 );
			return 1;
		}

		CMotionCtrl* mcptr;
		mcptr = (*this)( boneno );
		if( !mcptr ){
			DbgOut( "mothandler : FillUpMotion : mcptr error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* selem;
		selem = (*lpsh)( boneno );
		if( !selem ){
			DbgOut( "mothandler : FillUpMotion : selem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		MakeRawmatReq( motid, selem, startframe, endframe, 0, lpsh, calceulflag );

		int errorflag = 0;
		RawmatToMotionmatReq( motid, lpsh, boneno, &errorflag, startframe, endframe, 0 );
		if( errorflag ){
			DbgOut( "mothandler : FillUpMotion : RawmatToMotionmatReq error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
	}else{

		ret = MakeRawmat( motid, lpsh, startframe, endframe, calceulflag );
		if( ret ){
			DbgOut( "mothandler : FillUpMotion : MakeRawmat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = RawmatToMotionmat( motid, lpsh, startframe, endframe );
		if( ret ){
			DbgOut( "mothandler : FillUpMotion : RawmatToMotionmat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

/***
	//bsphere
	if( initflag == 1 ){
		ret = lpsh->SetBSphereData( this, motid, startframe, endframe );
		if( ret ){
			DbgOut( "mothandler : FillUpMotion : shandler SetBSphereData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
***/

	return 0;
}

int CMotHandler::CopyMotionFrameML( CShdHandler* lpsh, int seri, int srcmotid, int srcframe, int dstmotid, int dstframe )
{

	int ret;
////	
	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}


	if( (srcmotid < 0) || (srcmotid >= m_kindnum) ){
		DbgOut( "mothandler : CopyMotionFrameML : srcmotid error %d!!!\n", srcmotid );
		_ASSERT( 0 );
		return 1;
	}

	int srcframeleng;
	ret = GetMotionFrameLength( srcmotid, &srcframeleng );
	if( ret ){
		DbgOut( "mothandler : CopyMotionFrameML : GetMotionFrameLength src error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcframe < 0) || (srcframe >= srcframeleng) ){
		DbgOut( "mothandler : CopyMotionFrameML : srcframe error %d, %d!!!\n", srcframe, srcframeleng );
		_ASSERT( 0 );
		return 1;
	}
////
	if( (dstmotid < 0) || (dstmotid >= m_kindnum) ){
		DbgOut( "mothandler : CopyMotionFrameML : dstmotid error %d!!!\n", dstmotid );
		_ASSERT( 0 );
		return 1;
	}

	int dstframeleng;
	ret = GetMotionFrameLength( dstmotid, &dstframeleng );
	if( ret ){
		DbgOut( "mothandler : CopyMotionFrameML : GetMotionFrameLength dst error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (dstframe < 0) || (dstframe >= dstframeleng) ){
		DbgOut( "mothandler : CopyMotionFrameML : dstframe error %d!!!\n", dstframe );
		_ASSERT( 0 );
		return 1;
	}
////
	CMotionCtrl* mcptr = (*this)( seri );
	CShdElem* selem;
	selem = (*lpsh)( seri );
	_ASSERT( selem );
	if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
		int ret;
		ret = mcptr->CopyMotionFrame( selem, srcmotid, srcframe, dstmotid, dstframe, lpsh, this );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;


}


int CMotHandler::CopyMotionFrame( CShdHandler* lpsh, int srcmotid, int srcframe, int dstmotid, int dstframe )
{
	int ret;
////	
	if( m_kindnum <= 0 ){
		//_ASSERT( 0 );
		return 0;
	}


	if( (srcmotid < 0) || (srcmotid >= m_kindnum) ){
		DbgOut( "mothandler : CopyMotionFrame : srcmotid error %d!!!\n", srcmotid );
		_ASSERT( 0 );
		return 1;
	}

	int srcframeleng;
	ret = GetMotionFrameLength( srcmotid, &srcframeleng );
	if( ret ){
		DbgOut( "mothandler : CopyMotionFrame : GetMotionFrameLength src error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcframe < 0) || (srcframe >= srcframeleng) ){
		DbgOut( "mothandler : CopyMotionFrame : srcframe error %d!!!\n", srcframe );
		_ASSERT( 0 );
		return 1;
	}
////
	if( (dstmotid < 0) || (dstmotid >= m_kindnum) ){
		DbgOut( "mothandler : CopyMotionFrame : dstmotid error %d!!!\n", dstmotid );
		_ASSERT( 0 );
		return 1;
	}

	int dstframeleng;
	ret = GetMotionFrameLength( dstmotid, &dstframeleng );
	if( ret ){
		DbgOut( "mothandler : CopyMotionFrame : GetMotionFrameLength dst error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (dstframe < 0) || (dstframe >= dstframeleng) ){
		DbgOut( "mothandler : CopyMotionFrame : dstframe error %d!!!\n", dstframe );
		_ASSERT( 0 );
		return 1;
	}
////
	int i;	
	for( i = 1; i < s2mot_leng; i++ ){
		CMotionCtrl* mcptr = (*this)( i );
		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			CShdElem* selem = (*lpsh)( i );
			//CPart* partptr = selem->part;
			//if( !partptr ){
			//	_ASSERT( 0 );
			//	return 1;
			//}
			//int bonenum = partptr->bonenum;
			//if( bonenum > 0 ){
				int ret;
				ret = mcptr->CopyMotionFrame( selem, srcmotid, srcframe, dstmotid, dstframe, lpsh, this );
				if( ret ){
					_ASSERT( 0 );
					return 1;
				}
			//}
		}
	}


	return 0;
}

int CMotHandler::CopyMMotAnimFrame( int srcseri, CShdHandler* lpsh, int srcmotid, int srcframe, int dstmotid, int dstframe )
{
	int ret;
	if( m_kindnum <= 0 ){
		return 0;
	}

////
	if( srcseri < 0 ){
		int seri;
		for( seri = 0; seri < s2mot_leng; seri++ ){
			ret = CopyMMotAnimFrame( seri, lpsh, srcmotid, srcframe, dstmotid, dstframe );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		return 0;//!!!!!!!!!!!!!!
	}

////	


	if( (srcmotid < 0) || (srcmotid >= m_kindnum) ){
		DbgOut( "mothandler : CopyMMotAnimFrame : srcmotid error %d!!!\n", srcmotid );
		_ASSERT( 0 );
		return 1;
	}

	int srcframeleng;
	ret = GetMotionFrameLength( srcmotid, &srcframeleng );
	if( ret ){
		DbgOut( "mothandler : CopyMMotAnimFrame : GetMotionFrameLength src error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (srcframe < 0) || (srcframe >= srcframeleng) ){
		DbgOut( "mothandler : CopyMMotAnimFrame : srcframe error %d!!!\n", srcframe );
		_ASSERT( 0 );
		return 1;
	}
////
	if( (dstmotid < 0) || (dstmotid >= m_kindnum) ){
		DbgOut( "mothandler : CopyMMotAnimFrame : dstmotid error %d!!!\n", dstmotid );
		_ASSERT( 0 );
		return 1;
	}

	int dstframeleng;
	ret = GetMotionFrameLength( dstmotid, &dstframeleng );
	if( ret ){
		DbgOut( "mothandler : CopyMMotAnimFrame : GetMotionFrameLength dst error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (dstframe < 0) || (dstframe >= dstframeleng) ){
		DbgOut( "mothandler : CopyMMotAnimFrame : dstframe error %d!!!\n", dstframe );
		_ASSERT( 0 );
		return 1;
	}
////

	if( srcseri >= s2mot_leng ){
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr;
	mcptr = (*this)( srcseri );
	if( mcptr->IsJoint() ){
		CMotionInfo* motinfo;
		motinfo = mcptr->motinfo;
		if( motinfo ){
			ret = motinfo->CopyMMotAnimFrame( mcptr, srcmotid, srcframe, dstmotid, dstframe );
			if( ret ){
				DbgOut( "mh : CopyMMotAnimFrame : mi CopyMMotAnimFrame error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


int CMotHandler::RemakeBSphereData( int motid, int framenum )
{
	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : RemakeBSphereData : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	if( framenum != *(m_motnum + motid) ){
		DbgOut( "mothandler : RemakeBSphereData : framenum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CBSphere* newbs;
	newbs = new CBSphere[ framenum ];
	if( !newbs ){
		DbgOut( "MotHandler : RemakeBSphereData : newbs alloc error !!!\n" );
		_ASSERT( 0 );
		return -1;
	}
	
	delete [] *(m_firstbs + motid);

	*(m_firstbs + motid) = newbs;

	return 0;
}



int CMotHandler::HuGetBoneMatrix( int boneno, int motid, int frameno, D3DXMATRIX* bonematptr )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : HuGetBoneMatrix : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : HuGetBoneMatrix : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : HuGetBoneMatrix : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : HuGetBoneMatrix : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//////////

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );
	if( !mcptr ){
		DbgOut( "mothandler : HuGetBoneMatrix : mc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( bonematptr ){
		//MatConvD3DX( bonematptr, mcptr->curmat );
		
		CMotionInfo* miptr;
		miptr = mcptr->motinfo;
		if( !miptr ){
			DbgOut( "mh : HuGetBoneMatrix : miptr NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMatrix2* mothead;
		mothead = *(miptr->firstmot + motid);
		CMatrix2 mat;
		mat = *(mothead + frameno);
		
		MatConvD3DX( bonematptr, mat );
	}
	return 0;
}

int CMotHandler::HuGetRawMatrix( int boneno, int motid, int frameno, D3DXMATRIX* rawmatptr )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : HuGetRawMatrix : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : HuGetRawMatrix : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : HuGetRawMatrix : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : HuGetRawMatrix : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );
	if( !mcptr ){
		DbgOut( "mothandler : HuGetRawMatrix : mc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( rawmatptr ){
		//MatConvD3DX( bonematptr, mcptr->curmat );
		
		CMotionInfo* miptr;
		miptr = mcptr->motinfo;
		if( !miptr ){
			DbgOut( "mh : HuGetRawMatrix : miptr NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMatrix2* mothead;
		mothead = *(miptr->rawmat + motid);
		CMatrix2 mat;
		mat = *(mothead + frameno);
		
		MatConvD3DX( rawmatptr, mat );
	}

	/***
	if( rawmatptr ){
		CMotionInfo* motinfo;
		motinfo = mcptr->motinfo;
		if( !motinfo ){
			DbgOut( "mothandler : HuGetRawMatrix : motinfo NULL error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int ret;
		CMatrix2 rawm;
		ret = motinfo->GetCurrentRawMat( &rawm );
		if( ret ){
			DbgOut( "mothandle : HuGetRawMatarix : minfo GetCurrentRawMat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		MatConvD3DX( rawmatptr, rawm );

	}
	***/
	return 0;
}



int CMotHandler::GetKeyframeNo( int motid, int boneno, KEYFRAMENO* framearray, int arrayleng, int* framenumptr )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetKeyframeNo : motion not loaded error !!!\n" );
		//_ASSERT( 0 );
		*framenumptr = 0;
		return 0;//!!!!!!!!!!!
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetKeyframeNo : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( boneno >= s2mot_leng ){
		DbgOut( "mothandler : GetKeyframeNo : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int frameleng;
	ret = GetMotionFrameLength( motid, &frameleng );
	if( ret ){
		DbgOut( "mothandler : GetKeyframeNo : GetMotionFrameLength error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int* dirtyflag;
	dirtyflag = (int*)malloc( sizeof( int ) * (frameleng + 1) );
	if( !dirtyflag ){
		DbgOut( "mh : GetKeyframeNo : dirtyflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( dirtyflag, sizeof( int ) * (frameleng + 1) );

	/***
	int frameno;
	for( frameno = 0; frameno < frameleng; frameno++ ){

		int seri;
		for( seri = 0; seri < s2mot_leng; seri++ ){
			CMotionCtrl* mcptr;
			mcptr = (*this)( seri );
			_ASSERT( mcptr );

			if( mcptr->IsJoint() ){
				CMotionPoint2* existptr;
				existptr = mcptr->IsExistMotionPoint( motid, frameno );
				if( existptr ){
					if( seri != boneno ){
						if( *( dirtyflag + frameno ) == 0 ){
							*( dirtyflag + frameno ) = 1;
						}
					}else{
						*( dirtyflag + frameno ) = 2;
						break;//!!!!!!!!!!!!!!!!!!!!!!!!!!
					}
				}
			}
		}
	}
	***/
	
	int seri;
	for( seri = 0; seri < s2mot_leng; seri++ ){
		CMotionCtrl* mcptr;
		mcptr = (*this)( seri );
		_ASSERT( mcptr );

		if( mcptr->IsJoint() ){
			CMotionPoint2* mpptr;
			mpptr = mcptr->GetMotionPoint( motid );
			while( mpptr ){
				
				if( seri != boneno ){
					if( *( dirtyflag + mpptr->m_frameno ) == 0 ){
						*( dirtyflag + mpptr->m_frameno ) = 1;
					}
				}else{
					*( dirtyflag + mpptr->m_frameno ) = 2;
				}

				mpptr = mpptr->next;
			}
		}
	}


	int dirtynum = 0;
	int frameno;
	for( frameno = 0; frameno < frameleng; frameno++ ){
		if( *( dirtyflag + frameno ) == 1 ){
			if( framearray ){
				if( dirtynum >= arrayleng ){
					DbgOut( "mh : GetKeyframeNo : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					free( dirtyflag );
					return 1;
				}

				( framearray + dirtynum )->frameno = frameno;
				( framearray + dirtynum )->kind = 1;
			}
			dirtynum++;
		}else if( *( dirtyflag + frameno ) == 2 ){

			if( framearray ){
				if( dirtynum >= arrayleng ){
					DbgOut( "mh : GetKeyframeNo : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					free( dirtyflag );
					return 1;
				}
				
				( framearray + dirtynum )->frameno = frameno;
				( framearray + dirtynum )->kind = 2;
			}
			dirtynum++;
		}
	}

	*framenumptr = dirtynum;

	free( dirtyflag );

	return 0;
}

int CMotHandler::GetKeyframeNoRange( int motid, int boneno, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetKeyframeNoRange : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetKeyframeNoRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetKeyframeNoRange : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int prevmpid = -1;
	int curmpid = 0;

	CQuaternion curq;
	D3DXVECTOR3 curtra;
	int curframe;
	DWORD curds;
	int curinterp;
	D3DXVECTOR3 curscale;
	int curuserint1;
	int setno = 0;
	while( curmpid >= 0 ){
		ret = GetNextMP( motid, boneno, prevmpid, &curmpid );
		if( ret ){
			DbgOut( "mothandler : GetKeyframeNoRange : GetNextMP error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curmpid >= 0 ){
			ret = GetMPInfo( motid, boneno, curmpid, &curq, &curtra, &curframe, &curds, &curinterp, &curscale, &curuserint1 );
			if( ret ){
				DbgOut( "mothandler : GetKeyframeNoRange : GetMPInfo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( (curframe >= srcstart) && (curframe <= srcend) ){

				if( setno >= arrayleng ){
					DbgOut( "mothandler : GetKeyframeNoRange : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				*( framearray + setno ) = curframe;
				setno++;

			}

			prevmpid = curmpid;

		}
	}

	*framenumptr = setno;


	return 0;
}


int CMotHandler::HuGetKeyframeNo( int motid, int boneno, int* framearray, int arrayleng, int* framenumptr )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : HuGetKeyframeNo : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : HuGetKeyframeNo : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : HuGetKeyframeNo : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	int prevmpid = -1;
	int curmpid = 0;

	CQuaternion curq;
	D3DXVECTOR3 curtra;
	int curframe;
	DWORD curds;
	int curinterp;
	D3DXVECTOR3 curscale;
	int curuserint1;
	int setno = 0;
	while( curmpid >= 0 ){
		ret = GetNextMP( motid, boneno, prevmpid, &curmpid );
		if( ret ){
			DbgOut( "mothandler : HuGetKeyframeNo : GetNextMP error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curmpid >= 0 ){
			ret = GetMPInfo( motid, boneno, curmpid, &curq, &curtra, &curframe, &curds, &curinterp, &curscale, &curuserint1 );
			if( ret ){
				DbgOut( "mothandler : HuGetKeyframeNo : GetMPInfo error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( framearray ){
				if( setno >= arrayleng ){
					DbgOut( "mothandler : HuGetKeyframeNo : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				*( framearray + setno ) = curframe;
			}
			setno++;

			prevmpid = curmpid;

		}
	}

	*framenumptr = setno;

	return 0;
}


int CMotHandler::GetTotalQOnFrame( CShdHandler* lpsh, int motid, int frameno, int boneno, CQuaternion* dstq )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetTotalQOnFrame : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( motid >= m_kindnum ){// -1指定も有り。
		DbgOut( "mothandler : GetTotalQOnFrame : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetTotalQOnFrame : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || ( (motid >= 0) && (frameno >= *(m_motnum + motid)) ) ){
		DbgOut( "mothandler : GetTotalQOnFrame : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	CShdElem* curselem;
	curselem = (*lpsh)( boneno );
	CQuaternion totalq;
	totalq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );

	CMotionCtrl* curmc;
	CMotionInfo* curmi;


	while( curselem ){
		curmc = (*this)( curselem->serialno );
		curmi = curmc->motinfo;

		if( !curmi ){
			DbgOut( "mothandler : GetTotalQOnFrame : curmi NULL error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}
		
		int hasmpflag = 0;
		CMotionPoint2 calcmp;

		int calcmotid, calcframeno;
		if( motid >= 0 ){
			calcmotid = motid;
			calcframeno = frameno;
		}else{
			calcmotid = curmi->kindno;
			calcframeno = curmi->motno;
		}

//		ret = SetBoneAxisQ( lpsh, curselem->serialno, m_zakind, calcmotid, calcframeno );
//		_ASSERT( !ret );
		CQuaternion axisq;
		ret = curselem->GetBoneAxisQ( &axisq );
		_ASSERT( !ret );

		ret = curmi->CalcMotionPointOnFrame( &axisq, curselem, &calcmp, calcmotid, calcframeno, &hasmpflag );
		if( ret ){
			DbgOut( "mothandler : GetTotalQOnFrame : CalcMotionPointOnFrame error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		totalq = calcmp.m_q * totalq;

		curselem = lpsh->FindUpperJoint( curselem, 0 );
	}

	totalq.normalize();

	*dstq = totalq;

	return 0;
}


int CMotHandler::HuGetKeyframeSRT( CShdHandler* lpsh, int motid, int boneno, int frameno, 
	D3DXVECTOR3* pscale, D3DXQUATERNION* prot, D3DXVECTOR3* ptrans, int isstandard )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : HuGetKeyframeSRT : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : HuGetKeyframeSRT : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : HuGetKeyframeSRT : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	int mpid = -2;

	CMotionCtrl* mcptr;
	mcptr = (*this)( boneno );
	_ASSERT( mcptr );
	if( !mcptr->IsJoint() ){
		DbgOut( "mothandler : HuGetKeyFrameSRT : this is not bone object error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CShdElem* selem;
	selem = (*lpsh)( boneno );
	_ASSERT( selem );


	CMotionPoint2 mp;
	int hasmpflag = 0;
	ret = mcptr->CalcMotionPointOnFrame( selem, &mp, motid, frameno, &hasmpflag, lpsh, this );
	if( ret ){
		DbgOut( "mothandler : HuGetKeyFrameSRT : mc CalcMotionPointOnFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	D3DXVECTOR3 bonepos;
	D3DXMATRIX inimat;
	D3DXMatrixIdentity( &inimat );
	ret = lpsh->GetBonePos( boneno, 0, -1, 0, &bonepos, this, inimat, 0 );
	if( ret ){
		DbgOut( "mothandler : HuGetKeyframeSRT : sh GetBonePos error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	CShdElem* curselem;
	curselem = (*lpsh)( boneno );
	CShdElem* parselem;

	parselem = lpsh->FindUpperJoint( curselem );

	D3DXVECTOR3 parpos;
	D3DXVECTOR3 gparpos;
	if( parselem ){
		ret = lpsh->GetBonePos( parselem->serialno, 0, -1, 0, &parpos, this, inimat, 0 );
		if( ret ){
			DbgOut( "mothandler : HuGetKeyframeSRT : sh GetBonePos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CShdElem* gparelem;
		gparelem = lpsh->FindUpperJoint( parselem );
		if( gparelem ){
			ret = lpsh->GetBonePos( gparelem->serialno, 0, -1, 0, &gparpos, this, inimat, 0 );
			if( ret ){
				DbgOut( "mothandler : HuGetKeyframeSRT : sh GetBonePos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			gparpos.x = 0.0f;
			gparpos.y = 0.0f;
			gparpos.z = 0.0f;
		}

	}else{
		parpos.x = 0.0f;
		parpos.y = 0.0f;
		parpos.z = 0.0f;

		gparpos.x = 0.0f;
		gparpos.y = 0.0f;
		gparpos.z = 0.0f;

	}

//	pscale->x = scale.x; pscale->y = scale.y; pscale->z = scale.z;
	pscale->x = mp.m_scalex;
	pscale->y = mp.m_scaley;
	pscale->z = mp.m_scalez;

	CQuaternion invq;
	mp.m_q.inv( &invq );
	invq.CQuaternion2X( prot );


	//*ptrans = tra + bonepos - parpos;
	//*ptrans = tra + parpos - gparpos;
	if( isstandard == 0 ){
		ptrans->x = mp.m_mvx + parpos.x - gparpos.x;
		ptrans->y = mp.m_mvy + parpos.y - gparpos.y;
		ptrans->z = mp.m_mvz + parpos.z - gparpos.z;
	}else{
		ptrans->x = mp.m_mvx + bonepos.x - parpos.x;
		ptrans->y = mp.m_mvy + bonepos.y - parpos.y;
		ptrans->z = mp.m_mvz + bonepos.z - parpos.z;
	}
	return 0;
}

int CMotHandler::GetKeyframeSRT2( CShdHandler* lpsh, int motid, int boneno, int frameno, D3DXVECTOR3* pscale, D3DXQUATERNION* prot, D3DXVECTOR3* ptrans )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetKeyframeSRT2 : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetKeyframeSRT2 : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno <= 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mothandler : GetKeyframeSRT2 : boneno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int ret;
	int mpid = -2;

	ret = IsExistMotionPoint( motid, boneno, frameno, &mpid );
	if( ret || (mpid < 0) ){
		DbgOut( "mothandler : GetKeyframeSRT2 : IsExistMotionPoint error %d !!!\n", mpid );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion q;
	D3DXVECTOR3 tra;
	int dstframeno;
	DWORD ds;
	int interp;
	D3DXVECTOR3 scale;
	int userint1;
	ret = GetMPInfo( motid, boneno, mpid, &q, &tra, &dstframeno, &ds, &interp, &scale, &userint1 );
	if( ret ){
		DbgOut( "mothandler : GetKeyframeSRT2 : GetMPInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( frameno != dstframeno ){
		DbgOut( "mothandler : GetKeyframeSRT2 : GetMPInfo frameno error %d %d\n", frameno, dstframeno );
		_ASSERT( 0 );
		return 1;
	}

	pscale->x = scale.x; pscale->y = scale.y; pscale->z = scale.z;

	CQuaternion invq;
	q.inv( &invq );
	invq.CQuaternion2X( prot );

	ptrans->x = tra.x; ptrans->y = tra.y; ptrans->z = tra.z;

	return 0;
}



int CMotHandler::DestroyMotionFrame( CShdHandler* lpsh, int motid, int frameno )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : DestroyMotionFrame : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : DestroyMotionFrame : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	int ret;
	if( frameno < 0 ){
		int framenomax;
		framenomax = *( m_motnum + motid ) - 1;
		int delframe;
		for( delframe = 0; delframe <= framenomax; delframe++ ){
			ret = DestroyMotionFrameOne( lpsh, motid, delframe );
			if( ret ){
				DbgOut( "mothandler : DestroyMotionFrame : One error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		ret = DestroyMotionFrameOne( lpsh, motid, frameno );
		if( ret ){
			DbgOut( "mothandler : DestroyMotionFrame : One error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}



int CMotHandler::DestroyMotionFrameOne( CShdHandler* lpsh, int motid, int frameno )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : DestroyMotionFrame : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : DestroyMotionFrame : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int serino;
	int shdnum = lpsh->s2shd_leng;
	int* delbuf;
	delbuf = (int*)malloc( sizeof( int ) * shdnum );
	if( !delbuf ){
		_ASSERT( 0 );
		DbgOut( "mothandler : DestroyMotionFrame delbuf : malloc error!!!\n" );
		return 1;
	}
	ZeroMemory( delbuf, sizeof( int ) * shdnum );

	CShdElem* curselem;
	for( serino = 1; serino < shdnum; serino++ ){
		curselem = (*lpsh)( serino );
		if( curselem->IsJoint() && (curselem->type != SHDMORPH) ){
			CMotionCtrl* mcptr = (*this)( serino );
			_ASSERT( mcptr );
			CMotionPoint2* isexist = mcptr->IsExistMotionPoint( motid, frameno );
			if( isexist ){
				*(delbuf + serino) = 1;
			}else{
				*(delbuf + serino) = 0;
			}
		}
	}


	//deleteする。
	int ret = 0;
	for( serino = 1; serino < shdnum; serino++ ){
		if( *(delbuf + serino) == 1 ){
			CMotionCtrl* mcptr = (*this)( serino );
			_ASSERT( mcptr );
			ret = mcptr->DeleteMotionPoint( motid, frameno );
			if( ret ){
				DbgOut( "mh : DestroyMotionFrame : mc : DeleteMotionPoint error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}
	free( delbuf );

	//ret = MakeRawmat( motid, lpsh, -1, -1 );
	//_ASSERT( !ret );
	//ret = RawmatToMotionmat( motid, lpsh, -1, -1 );
	//_ASSERT( !ret );


	//ret = SetCurrentMotion( motid, m_curframeno );


	return 0;
}

int CMotHandler::GetDispSwitch2( CShdHandler* lpsh, int* swptr, int leng )
{
	
	int swno;
	for( swno = 0; swno < DISPSWITCHNUM; swno++ ){
		if( swno < leng ){
			*( swptr + swno ) = (lpsh->m_curds + swno)->state;
		}
	}

	return 0;
}


int CMotHandler::GetDispSwitch( CShdHandler* lpsh, int motid, int frameno, DWORD* dswitch )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetDispSwitch : motion not loaded error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
/////////
	if( motid < 0 ){
		motid = m_curmotkind;
	}

	if( frameno < 0 ){
		frameno = m_curframeno;
	}

////////
	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetDispSwitch : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( frameno >= *(m_motnum + motid) ){
		DbgOut( "mothandler : GetDispSwitch : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( s2mot_leng <= 1 ){
		*dswitch = 0;
		return 0;//!!!!!!!!!!!
	}


	/***
	CMotionCtrl* firstjoint = 0;
	CMotionCtrl* mc;
	int seri;
	for( seri = 0; seri < s2mot_leng; seri++ ){
		mc = (*this)( seri );
		if( mc->IsJoint() && (mc->type != SHDMORPH) ){
			firstjoint = mc;
			break;
		}
	}
	***/
	int ret;

	CMotionCtrl* firstjoint = 0;
	ret = lpsh->GetFirstJoint( &firstjoint, this, 1 );
	if( ret || !firstjoint ){
		*dswitch = 0;
		return 0;//!!!!!!!!!
	}


	ret = firstjoint->GetDispSwitch( dswitch, motid, frameno );
	if( ret ){
		DbgOut( "mothandler : GetDispSwitch : mc GetDispSwitch error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "mothandler : GetDispSwitch : motid %d, frameno %d, ds %d\r\n", motid, frameno, *dswitch );

	return 0;
}

int CMotHandler::GetMotionIDByName( char* srcname )
{
	int retid = -1;

	if( s2mot_leng <= 1 )
		return -1;//!!!!!!!!!!

	CMotionCtrl* mcptr = (*this)( 1 );
	if( !mcptr ){
		_ASSERT( 0 );
		return -1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		_ASSERT( 0 );
		return -1;
	}
	if( !(miptr->motname) ){
		_ASSERT( 0 );
		return -1;
	}

	int mno;
	for( mno = 0; mno < m_kindnum; mno++ ){

		char* curname;
		curname = *( miptr->motname + mno );

		int cmp0;
		cmp0 = strcmp( curname, srcname );
		if( cmp0 == 0 ){
			retid = mno;
			break;
		}
	}

	return retid;
}

int CMotHandler::GetMotionName( int motid, char* motname )
{
	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mh : GetMotionName : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMotionCtrl* mcptr = (*this)( 1 );
	if( !mcptr ){
		DbgOut( "mh : GetMotionName : mcptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	CMotionInfo* miptr = mcptr->motinfo;
	if( !miptr ){
		DbgOut( "mh : GetMotinName : miptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !(miptr->motname) ){
		DbgOut( "mh : GetMotionName : motname NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	char* nameptr = *(miptr->motname + motid);
	strcpy_s( motname, 256, nameptr );

	return 0;
}

int CMotHandler::SetTickCount( int seri, DWORD srctick )
{
	int ret;
	if( seri < 0 ){
		int i;
		for( i = 0; i < s2mot_leng; i++ ){
			ret = SetTickCount( i, srctick );
			_ASSERT( !ret );
		}
	}else{
		if( seri >= s2mot_leng ){
			DbgOut( "mh : SetTickCount : seri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMotionCtrl* mc;
		mc = (*this)( seri );

		if( mc->IsJoint() && (mc->type != SHDMORPH) ){
			CMotionInfo* mi;
			mi = mc->motinfo;
			if( mi ){
				mi->tick = srctick;
			}
		}
	}

	return 0;
}

int CMotHandler::GetFirstTickJoint( int nextflag, int srcmotid, int* dstseri, int* dstframeno )
{
	*dstseri = -1;
	*dstframeno = 0;
	DWORD mintick = 0xFFFFFFFF;

	int seri;
	for( seri = 0; seri < s2mot_leng; seri++ ){
		CMotionCtrl* mc;
		mc = (*this)( seri );
		if( mc->IsJoint() && (mc->type != SHDMORPH) ){
			CMotionInfo* mi;
			mi = mc->motinfo;
			if( mi ){
				if( nextflag ){
					NEXTMOTION nextmot;
					int getonly = 1;
					mc->GetNextMotion( &nextmot, getonly );
					if( nextmot.mk == srcmotid ){
						if( mi->tick < mintick ){
							mintick = mi->tick;
							*dstseri = seri;
							*dstframeno = nextmot.aftframeno;
						}
					}
				}else{
					if( mi->kindno == srcmotid ){
						if( mi->tick < mintick ){
							mintick = mi->tick;
							*dstseri = seri;
							*dstframeno = mc->curframeno;
						}
					}
				}
			}
		}
	}

	return 0;
}

int CMotHandler::CalcMPEuler( CShdHandler* lpsh, int motid, int zakind )
{
	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mh : CalcMPEuler : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	int ret;
	int seri;
	for( seri = 0; seri < s2mot_leng; seri++ ){
		CMotionCtrl* mcptr;
		mcptr = (*this)( seri );
		CShdElem* selem;
		selem = (*lpsh)( seri );

		if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
			CMotionInfo* mi;
			mi = mcptr->motinfo;

			if( mi ){
				ret = mi->CalcMPEuler( this, lpsh, selem, motid, zakind );
				if( ret ){
					DbgOut( "mh CalcMPEuler : mi CalcMPEuler error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

		}
	}



	return 0;
}
int CMotHandler::SetBoneAxisQ( CShdHandler* srclpsh, int srcseri, int srczakind, int srcmotid, int srcframeno )
{
	int ret;

	if( srcseri < 0 ){
		int seri;
		for( seri = 0; seri < s2mot_leng; seri++ ){
			ret = SetBoneAxisQ( srclpsh, seri, srczakind, srcmotid, srcframeno );
			if( ret ){
				DbgOut( "mh SetBoneAxisQ : SetBoneAxisQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		return 0;

	}else{
		if( (srcmotid >= 0) && (srcmotid < m_kindnum) ){
			*(m_zatype + srcmotid) = srczakind;
		}else{
			_ASSERT( 0 );
		}

		CShdElem* selem;
		CQuaternion iniq( 1.0f, 0.0f, 0.0f, 0.0f );
		CQuaternion tmpq;
		CQuaternion tmpq2;
		CQuaternion tmpq4;
		CQuaternion setq;

		selem = (*srclpsh)( srcseri );
		_ASSERT( selem );
		if( selem->IsJoint() && (selem->type != SHDMORPH) ){
			if( srczakind == ZA_1 ){
				setq = iniq;
			}else if( srczakind == ZA_2 ){
				ret = GetTotalQOnFrame( srclpsh, srcmotid, srcframeno, srcseri, &tmpq );
				if( ret ){
					DbgOut( "mh SetBoneAxisQ : GetTotalQOnFrame error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = tmpq.inv( &tmpq2 );
				_ASSERT( !ret );

				setq = tmpq2;

			}else if( srczakind == ZA_3 ){

				ret = srclpsh->GetInitialBoneQ( srcseri, &tmpq );
				if( ret ){
					DbgOut( "mh SetBoneAxisQ : sh GetInitialBoneQ error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				setq = tmpq;

			}else if( srczakind == ZA_4 ){

				int za4type;
				ret = selem->GetZa4Type( &za4type );
				_ASSERT( !ret );

				ret = selem->GetZa4Q( &tmpq4 );
				if( ret ){
					DbgOut( "mh SetBoneAxisQ : se GetZa4Q error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( za4type == ZA_1 ){
					setq = tmpq4;

				}else if( za4type == ZA_2 ){
					ret = GetTotalQOnFrame( srclpsh, srcmotid, srcframeno, srcseri, &tmpq );
					if( ret ){
						DbgOut( "mh SetBoneAxisQ : GetTotalQOnFrame error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
					ret = tmpq.inv( &tmpq2 );
					_ASSERT( !ret );

					setq = tmpq2 * tmpq4;

				}else if( za4type == ZA_3 ){

					ret = srclpsh->GetInitialBoneQ( srcseri, &tmpq );
					if( ret ){
						DbgOut( "mh SetBoneAxisQ : sh GetInitialBoneQ error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}

					setq = tmpq * tmpq4;

				}else{
					_ASSERT( 0 );
					return 1;
				}

			}else{
				DbgOut( "mh SetBoneAxisQ : zakind error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = selem->SetBoneAxisQ( setq );
			if( ret ){
				DbgOut( "mh SetBoneAxisQ : se SetBoneAxisQ error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		
		return 0;

	}

}

int CMotHandler::GetTexAnimKeyframeNoRange( int boneno, int motid, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetTexAnimKeyframeNoRange : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetTexAnimKeyframeNoRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetTexAnimKeyframeNoRange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////


	CTexKey* tkptr = 0;
	tkptr = GetFirstTexKey( boneno, motid );

	if( !tkptr ){
		*framenumptr = 0;
		return 0;
	}

	int setno = 0;
	while( tkptr ){
		int curframeno;
		curframeno = tkptr->frameno;
		if( (curframeno >= srcstart) && (curframeno <= srcend) ){
			if( setno >= arrayleng ){
				DbgOut( "mh : GetTexAnimKeyframeNoRange : arrayleng too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			*( framearray + setno ) = curframeno;
			setno++;
		}
		tkptr = tkptr->next;
	}
	*framenumptr = setno;

	return 0;
}


int CMotHandler::ExistTexKey( int boneno, int motid, int frameno, CTexKey** pptexkey )
{

	*pptexkey = 0;


	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : ExistTexKey : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : ExistTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : ExistTexKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : ExistTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->ExistTexKey( motid, frameno, pptexkey );
		if( ret ){
			DbgOut( "mh : ExistTexKey : mi ExistTexKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMotHandler::CreateTexKey( int boneno, int motid, int frameno, CTexKey** pptexkey )
{

	*pptexkey = 0;


	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : CreateTexKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : CreateTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : CreateTexKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : CreateTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->CreateTexKey( motid, frameno, pptexkey );
		if( ret ){
			DbgOut( "mh : CreateTexKey : mi CreateTexKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
CMotionPoint2* CMotHandler::GetFirstMotionPoint( int boneno, int motid )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetFirstMotionPoint : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetFirstMotionPoint : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetFirstMotionPoint : boneno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
//////////////
	//int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		//_ASSERT( 0 );
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		return mi->GetMotionPoint( motid );
	}else{
		return 0;
	}
}

CTexKey* CMotHandler::GetFirstTexKey( int boneno, int motid )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetFirstTexKey : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetFirstTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetFirstTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
//////////////
	//int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		//_ASSERT( 0 );
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		return mi->GetFirstTexKey( motid );
	}else{
		//_ASSERT( 0 );
		return 0;
	}
}

int CMotHandler::DeleteTexKey( int boneno, int motid, int frameno )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : DeleteTexKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : DeleteTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : DeleteTexKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : DeleteTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->DeleteTexKey( motid, frameno );
		if( ret ){
			DbgOut( "mh : DeleteTexKey : mi DeleteTexKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}

int CMotHandler::LeaveFromChainTexKey( int boneno, int motid, int frameno )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : LeaveFromChainTexKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : LeaveFromChainTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : LeaveFromChainTexKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : LeaveFromChainTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	//int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		CTexKey* tkptr = 0;
		tkptr = mi->LeaveFromChainTexKey( motid, frameno );
		if( !tkptr ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CMotHandler::ChainTexKey( int boneno, int motid, CTexKey* tkptr )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : ChainTexKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : ChainTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : ChainTexKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->ChainTexKey( motid, tkptr );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CMotHandler::InitTexAnim( CShdHandler* lpsh )
{
	CMQOMaterial* curmat;
	curmat = lpsh->m_mathead;
	while( curmat ){
		curmat->curtexname = 0;

		curmat = curmat->next;
	}
	return 0;
}

int CMotHandler::InitAlpAnim( CShdHandler* lpsh )
{

//DbgOut( "\r\n" );

	CMQOMaterial* curmat;
	curmat = lpsh->m_mathead;
	while( curmat ){
		curmat->col.a = curmat->orgalpha;

//DbgOut( "checkalp!!! : mh InitAlpAnim : %s, %f\r\n", curmat->name, curmat->orgalpha );

		curmat = curmat->next;
	}
	return 0;
}

int CMotHandler::GetTexAnim( int boneno, int motid, int frameno )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetTexAnim : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetTexAnim : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : GetTexAnim : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetTexAnim : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;
	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->GetTexAnim( motid, frameno );
		if( ret ){
			DbgOut( "mh : GetTexAnim : mi GetTexAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}


///////////


int CMotHandler::GetDSAnim( CDispSwitch* dsptr, int boneno, int motid, int frameno )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetDSAnim : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetDSAnim : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : GetDSAnim : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetDSAnim : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;
	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->GetDSAnim( dsptr, motid, frameno );
		if( ret ){
			DbgOut( "mh : GetDSAnim : mi GetDSAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	return 0;
}


int CMotHandler::GetDSAnimKeyframeNoRange( int boneno, int motid, int srcstart, int srcend, int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetDSAnimKeyframeNoRange : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetDSAnimKeyframeNoRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetDSAnimKeyframeNoRange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////


	CDSKey* dskptr = 0;
	dskptr = GetFirstDSKey( boneno, motid );

	if( !dskptr ){
		*framenumptr = 0;
		return 0;
	}

	int setno = 0;
	while( dskptr ){
		int curframeno;
		curframeno = dskptr->frameno;
		if( (curframeno >= srcstart) && (curframeno <= srcend) ){
			if( setno >= arrayleng ){
				DbgOut( "mh : GetTexDSKeyframeNoRange : arrayleng too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			*( framearray + setno ) = curframeno;
			setno++;
		}
		dskptr = dskptr->next;
	}
	*framenumptr = setno;

	return 0;
}


int CMotHandler::ExistDSKey( int boneno, int motid, int frameno, CDSKey** ppdskey )
{

	*ppdskey = 0;


	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : ExistDSKey : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : ExistDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : ExistDSKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : ExistDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->ExistDSKey( motid, frameno, ppdskey );
		if( ret ){
			DbgOut( "mh : ExistDSKey : mi ExistDSKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMotHandler::CreateDSKey( int boneno, int motid, int frameno, CDSKey** ppdskey )
{

	*ppdskey = 0;


	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : CreateDSKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : CreateDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : CreateDSKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : CreateDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->CreateDSKey( motid, frameno, ppdskey );
		if( ret ){
			DbgOut( "mh : CreateDSKey : mi CreateDSKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

CDSKey* CMotHandler::GetFirstDSKey( int boneno, int motid )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetFirstDSKey : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetFirstDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetFirstDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
//////////////
	//int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		//_ASSERT( 0 );
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		return mi->GetFirstDSKey( motid );
	}else{
		//_ASSERT( 0 );
		return 0;
	}
}

int CMotHandler::DeleteDSKey( int boneno, int motid, int frameno )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : DeleteDSKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : DeleteDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : DeleteDSKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : DeleteDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->DeleteDSKey( motid, frameno );
		if( ret ){
			DbgOut( "mh : DeleteDSKey : mi DeleteDSKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotHandler::LeaveFromChainDSKey( int boneno, int motid, int frameno )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : LeaveFromChainDSKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : LeaveFromChainDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : LeaveFromChainDSKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : LeaveFromChainDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	//int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		CDSKey* dskptr = 0;
		dskptr = mi->LeaveFromChainDSKey( motid, frameno );
		if( !dskptr ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CMotHandler::ChainDSKey( int boneno, int motid, CDSKey* dskptr )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : ChainDSKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : ChainDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : ChainDSKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->ChainDSKey( motid, dskptr );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CMotHandler::InitDSAnim( CShdHandler* lpsh )
{
	int dsno;
	for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
		( lpsh->m_ds + dsno )->state = 1;
	}

	int seri;
	for( seri = 0; seri < s2mot_leng; seri++ ){
		CMotionCtrl* mcptr = (*this)( seri );
		mcptr->m_lpsh = lpsh;
	}

	return 0;
}

int CMotHandler::SetMState( CShdHandler* lpsh, CMState* dstms, int bonenum, int* boneno2seri )
{
	int boneno, seri;
	int ret;
	for( boneno = 0; boneno < bonenum; boneno++ ){
		seri = *( boneno2seri + boneno );
		if( (seri < 0) || (seri >= s2mot_leng) ){
			DbgOut( "mh : SetMState : seri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMotionCtrl* mcptr;
		mcptr = (*this)( seri );
		_ASSERT( mcptr );

		ret = dstms->SetMSElem( boneno, &( mcptr->curmat ) );
		if( ret ){
			DbgOut( "mh : SetMState : ms SetMSElem error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( bonenum > 0 ){
		ret = dstms->SetMSElem( lpsh );
		if( ret ){
			DbgOut( "mh : SetMState : ms SetMSElem lpsh error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CMotHandler::SetCurMatFromMState( CShdHandler* lpsh, CMState* srcms, int bonenum, int* boneno2seri )
{
	if( !srcms ){
		return 0;
	}

	int boneno, seri;
	//int ret;
	for( boneno = 0; boneno < bonenum; boneno++ ){
		seri = *( boneno2seri + boneno );
		if( (seri < 0) || (seri >= s2mot_leng) ){
			DbgOut( "mh : SetCurMatFromMState : seri error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		CMotionCtrl* mcptr;
		mcptr = (*this)( seri );
		_ASSERT( mcptr );

		MSELEM* srcmse = srcms->m_mselem + boneno;

		mcptr->curmat = srcmse->bonemat;
	}

	int dsno;
	for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
		(lpsh->m_curds + dsno)->state = srcms->m_ds[dsno];
	}


	int ret;
	CMQOMaterial* curmat = lpsh->m_mathead;
	while( curmat ){
		CTexChangePt* tcptr = 0;
		ret = srcms->m_tk.ExistTexChangePt( curmat, &tcptr );
		if( ret ){
			DbgOut( "mh : SetCurMatFromMState : tk ExistTexChangePt error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( tcptr ){
			curmat->curtexname = tcptr->m_change;
		}else{
			curmat->curtexname = 0;
		}
//////////////
		CAlpChange* acptr = 0;
		ret = srcms->m_ak.ExistAlpChange( curmat, &acptr );
		if( ret ){
			DbgOut( "mh : SetCurMatFromMState : ak ExistAlpChange error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( acptr ){
			curmat->col.a = acptr->change;
		}else{
			curmat->col.a = curmat->orgalpha;
		}


		curmat = curmat->next;
	}

//DbgOut( "!!!!!!checkm!!! : mh SetCurMatFromMState : lpsh->m_mnum %d\r\n", lpsh->m_mnum );

	int mno;
	for( mno = 0; mno < lpsh->m_mnum; mno++ ){
		CShdElem* melem = *(lpsh->m_ppm + mno);
		CMorph* curmorph = melem->morph;
		if( curmorph ){
			CShdElem* baseelem = curmorph->m_baseelem;
			_ASSERT( baseelem );
			CMotionCtrl* boneelem = curmorph->m_boneelem;
			_ASSERT( boneelem );

			CMMotElem* mmeptr = 0;
			mmeptr = boneelem->GetCurMMotElem( baseelem );

			if( mmeptr ){
				int tno;
				for( tno = 0; tno < curmorph->m_targetnum; tno++ ){
					TARGETPRIM* curtp = curmorph->m_ptarget + tno;
					CShdElem* targetelem = curtp->selem;
					_ASSERT( targetelem );

					MPRIM* curmp = srcms->m_mk.ExistTarget( baseelem, targetelem );
					if( curmp ){
						ret = mmeptr->SetValue( curmp->selem, curmp->value );
						if( ret ){
							DbgOut( "mh : SetCurMatFromMState : mme SetValue 0 error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
//DbgOut( "!!!checkm!!! : mh SetCurMatFromMState : mno %d, tno %d, value %f\r\n", mno, tno, curmp->value );
					}else{
						ret = mmeptr->SetValue( targetelem, 0.0f );
						if( ret ){
							DbgOut( "mh : SetCurMatFromMState : mme SetValue 1 error !!!\n" );
							_ASSERT( 0 );
							return 1;
						}
						_ASSERT( !ret );
					}
				}
			}else{
				_ASSERT( 0 );
			}
		}else{
			_ASSERT( 0 );
		}
	}


	return 0;
}

int CMotHandler::ExistAlpKey( int boneno, int motid, int frameno, CAlpKey** ppalpkey )
{

	*ppalpkey = 0;


	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : ExistAlpKey : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : ExistAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : ExistAlpKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : ExistAlpKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->ExistAlpKey( motid, frameno, ppalpkey );
		if( ret ){
			DbgOut( "mh : ExistAlpKey : mi ExistAlpKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CMotHandler::CreateAlpKey( int boneno, int motid, int frameno, CAlpKey** ppalpkey )
{

	*ppalpkey = 0;


	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : CreateAlpKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : CreateAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : CreateAlpKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : CreateAlpKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->CreateAlpKey( motid, frameno, ppalpkey );
		if( ret ){
			DbgOut( "mh : CreateAlpKey : mi CreateAlpKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

CAlpKey* CMotHandler::GetFirstAlpKey( int boneno, int motid )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetFirstAlpKey : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetFirstAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetFirstAlpKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
//////////////
	//int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		//_ASSERT( 0 );
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		return mi->GetFirstAlpKey( motid );
	}else{
		//_ASSERT( 0 );
		return 0;
	}
}

int CMotHandler::DeleteAlpKey( int boneno, int motid, int frameno )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : DeleteAlpKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : DeleteAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : DeleteAlpKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : DeleteAlpKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->DeleteAlpKey( motid, frameno );
		if( ret ){
			DbgOut( "mh : DeleteAlpKey : mi DeleteAlpKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 0;
	}

	return 0;
}

int CMotHandler::LeaveFromChainAlpKey( int boneno, int motid, int frameno )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : LeaveFromChainAlpKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : LeaveFromChainAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : LeaveFromChainAlpKey : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : LeaveFromChainAlpKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	//int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		CAlpKey* alpkptr = 0;
		alpkptr = mi->LeaveFromChainAlpKey( motid, frameno );
		if( !alpkptr ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CMotHandler::ChainAlpKey( int boneno, int motid, CAlpKey* addalpk )
{

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : ChainAlpKey : no motion error !!!\n" );
		return 1;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : ChainAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : ChainAlpKey : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////////
	int ret;

	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		_ASSERT( 0 );
		return 1;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->ChainAlpKey( motid, addalpk );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}

int CMotHandler::GetAlpAnim( int boneno, int motid, int frameno, CMQOMaterial* srcmat, float* alphaptr, int* existflag )
{
	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetAlpAnim : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetAlpAnim : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (frameno < 0) || (frameno >= *(m_motnum + motid)) ){
		DbgOut( "mothandler : GetAlpAnim : frameno out of range error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetAlpAnim : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////
	int ret;
	CMotionCtrl* mc;
	mc = (*this)( boneno );
	_ASSERT( mc );

	if( !mc->IsJoint() || (mc->type == SHDMORPH) ){
		return 0;//!!!!!!!!!!!
	}

	CMotionInfo* mi;
	mi = mc->motinfo;
	if( mi ){
		ret = mi->GetAlpAnim( motid, frameno, srcmat, alphaptr, existflag );
		if( ret == 1 ){
			DbgOut( "mh : GetAlpAnim : mi GetAlpAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}else if( ret == 2 ){
			*alphaptr = srcmat->orgalpha;
		}
	}


	return 0;
}
int CMotHandler::GetAlpAnimKeyframeNoRange( int boneno, int motid, int srcstart, int srcend, 
	int* framearray, int arrayleng, int* framenumptr )
{
	*framenumptr = 0;

	if( m_kindnum <= 0 ){
		DbgOut( "mothandler : GetAlpAnimKeyframeNoRange : no motion error !!!\n" );
		return 0;
	}

	if( (motid < 0) || (motid >= m_kindnum) ){
		DbgOut( "mothandler : GetAlpAnimKeyframeNoRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (boneno < 0) || (boneno >= s2mot_leng) ){
		DbgOut( "mh : GetAlpAnimKeyframeNoRange : boneno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

/////////


	CAlpKey* alpkptr = 0;
	alpkptr = GetFirstAlpKey( boneno, motid );

	if( !alpkptr ){
		*framenumptr = 0;
		return 0;
	}

	int setno = 0;
	while( alpkptr ){
		int curframeno;
		curframeno = alpkptr->frameno;
		if( (curframeno >= srcstart) && (curframeno <= srcend) ){
			if( setno >= arrayleng ){
				DbgOut( "mh : GetAlpAnimKeyframeNoRange : arrayleng too short error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			*( framearray + setno ) = curframeno;
			setno++;
		}
		alpkptr = alpkptr->next;
	}
	*framenumptr = setno;

	return 0;
}

int CMotHandler::GetNotToIdle( int boneno, int* flagptr )
{
	if( boneno < 0 ){
		*flagptr = m_nottoidle;
	}else{
		if( boneno < s2mot_leng ){
			CMotionCtrl* mcptr;
			mcptr = (*this)( boneno );
			_ASSERT( mcptr );
			*flagptr = mcptr->m_nottoidle;
		}else{
			_ASSERT( 0 );
			*flagptr = 0;
		}
	}

	return 0;
}

int CMotHandler::SetNotToIdle( int boneno, int srcflag )
{
	if( boneno < 0 ){
		m_nottoidle = srcflag;
	}else{
		if( boneno < s2mot_leng ){
			CMotionCtrl* mcptr;
			mcptr = (*this)( boneno );
			_ASSERT( mcptr );
			mcptr->m_nottoidle = srcflag;
		}else{
			_ASSERT( 0 );
		}
	}
		
	return 0;
}

int CMotHandler::GetZaType( int motid )
{
	int retza = ZA_1;

	if( (motid >= 0) && (motid < m_kindnum) ){
		retza = *(m_zatype + motid);
	}else{
		if( m_curmotkind >= 0 ){
			retza = *(m_zatype + m_curmotkind);
		}else{
			retza = ZA_1;
		}
	}

	return retza;
}

int CMotHandler::HasKey( int motid, int frameno, int* flagptr )
{
	if( (motid < 0) || (motid >= m_kindnum) ){
		_ASSERT( 0 );
		return 1;
	}

	int haskey = 0;
	int seri;
	CMotionCtrl* mcptr;
	CMotionPoint2* mpptr;
	for( seri = 0; seri < s2mot_leng; seri++ ){
		mcptr = (*this)( seri );
		_ASSERT( mcptr );

		mpptr = mcptr->GetMotionPoint( motid );
		while( mpptr ){
			if( mpptr->m_frameno > frameno ){
				break;
			}
			if( mpptr->m_frameno == frameno ){
				haskey = 1;
				break;
			}
			mpptr = mpptr->next;
		}

		if( haskey == 1 ){
			break;
		}
	}

	*flagptr = haskey;

	return 0;
}

