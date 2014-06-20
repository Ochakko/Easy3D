#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>
//#include <motfile.h>

#include <motionpoint2.h>
#include <shdhandler.h>
#include <shdelem.h>
#include <part.h>

#include <BoneProp.h>

#include <TexKey.h>
#include <TexChange.h>
#include <AlpKey.h>
#include <AlpChange.h>
#include <mqomaterial.h>


#include <DSKey.h>
#include <DSChange.h>
#include <DispSwitch.h>

#include <MMotKey.h>
#include <MMotElem.h>
#include <morph.h>

#define	DBGH
#include <dbg.h>


#define TEMPMPLENG	100

//////////////////
// static 
static CMotionPoint2* s_tempmp = 0;

//////////////////

// extern 
//extern CRITICAL_SECTION g_crit_tempmp; 
//extern CBonePropDlg* g_bonepropdlg;


CMotionInfo::CMotionInfo()
{
	InitParams();

	isinit = 1;
}

CMotionInfo::~CMotionInfo()
{
	DestroyObjs();
}


int CMotionInfo::CreateTempMP()
{
	if( !s_tempmp ){
		s_tempmp = new CMotionPoint2[ TEMPMPLENG ];
		if( !s_tempmp ){
			DbgOut( "CMotionInfo : CreateTempMP : s_tempmp alloc error !!!\n ");
			_ASSERT( 0 );
			return 1;
		}
	}
	return 0;
}
int CMotionInfo::DestroyTempMP()
{
	if( s_tempmp ){
		delete [] s_tempmp;
		s_tempmp = 0;
	}

	return 0;
}



int CMotionInfo::Name2Index( char* srcmotname ) 
{
	// motionname ---> motkindno この関数自体は高速化しないが、
	// 返り値をcookie のように使うことによって、CMatrix2 へのアクセスをスムーズにする。
	// 登録されていない名前に対しては、-1 を返す。
	int retno = -1;
	int i;
	char* curname = 0;

	for( i = 0; i < kindnum; i++ ){
		curname = *(motname + i);
		if( strcmp( srcmotname, curname ) == 0 ){
			retno = i;
			break;
		}
	}
	return retno;
}

int CMotionInfo::RemakeMotionMatrix( int srctype, int srcmotkind, int newtotal )
{
	int ret = CheckIndex( srcmotkind );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( kindno == srcmotkind ){
		//motnoの初期化。
		SetMotionKind( srcmotkind );
	}

	*(motnum + srcmotkind) = newtotal;

	////
	CMatrix2* newmat = new CMatrix2[newtotal];
	if( !newmat ){
		_ASSERT( 0 );
		return 1;
	}
	CMatrix2* oldmat = 	*(firstmot + srcmotkind);
	delete [] oldmat;
	*(firstmot + srcmotkind) = newmat;
	////
	CMatrix2* newrawmat = new CMatrix2[newtotal];
	if( !newrawmat ){
		_ASSERT( 0 );
		return 1;
	}
	CMatrix2* oldrawmat = *(rawmat + srcmotkind);
	delete [] oldrawmat;
	*(rawmat + srcmotkind) = newrawmat;
	////
	return 0;
}

int CMotionInfo::AddMotion( int srcstandard, int srctype, char* srcname, int srcmottype, int srcmotnum, int srcmotjump )
{
	//名前の登録、allocate
	//int indexno
	m_standard = srcstandard;

	int newkindnum, nameleng;
	char* nameptr = 0;
	CMatrix2* newmot = 0;
	float* newval = 0;

	/***
	indexno = Name2Index( srcname );
	if( indexno >= 0 ){
		DbgOut( "CMotionInfo : AddMotion : Name2Index : same name exist error !!!\n" );
		return -1;
	}
	***/


	if( !s_tempmp ){
		DbgOut( "CMotionInfo : AddMotion : s_tempmp alloc error !!!\n ");
		_ASSERT( 0 );
		return -1;
	}


	newkindnum = kindnum + 1;
	motname = (char**)realloc( motname, sizeof( char* ) * newkindnum );
	if( !motname ){
		return -1;
	}
	nameleng = (int)strlen( srcname );
	nameptr = (char*)malloc( nameleng + 1 );
	if( !nameptr ){
		return -1;
	}
	ZeroMemory( nameptr, nameleng + 1 );
	strncpy_s( nameptr, nameleng + 1, srcname, nameleng );
	*(motname + newkindnum - 1) = nameptr;
	

	motnum = (int*)realloc( motnum, sizeof( int ) * newkindnum );
	if( !motnum ){
		return -1;
	}
	*(motnum + newkindnum - 1) = srcmotnum;

	mottype = (int*)realloc( mottype, sizeof( int ) * newkindnum );
	if( !mottype ){
		return -1;
	}
	*(mottype + newkindnum - 1) = srcmottype;
	//*(mottype + newkindnum - 1) = MOTION_CLAMP; // default値

	motjump = (int*)realloc( motjump, sizeof( int ) * newkindnum );
	if( !motjump ){
		return -1;
	}
	*(motjump + newkindnum - 1) = srcmotjump;

	nextmot = (NEXTMOTION*)realloc( nextmot, sizeof( NEXTMOTION ) * newkindnum );
	(nextmot + newkindnum - 1)->mk = -1;
	(nextmot + newkindnum - 1)->befframeno = -1;
	(nextmot + newkindnum - 1)->aftframeno = 0;
	

	motstep = (int*)realloc( motstep, sizeof( int ) * newkindnum );
	if( !motstep ){
		return -1;
	}
	*(motstep + newkindnum - 1) = 1; // default値


	///////
	firstmot = (CMatrix2**)realloc( firstmot, sizeof( CMatrix2* ) * newkindnum );
	if( !firstmot ){
		return -1;
	}
	newmot = new CMatrix2[ srcmotnum ];
	if( !newmot ){
		return -1;
	}
	*(firstmot + newkindnum - 1) = newmot;
	///////
	rawmat = (CMatrix2**)realloc( rawmat, sizeof( CMatrix2* ) * newkindnum );
	if( !rawmat ){
		return -1;
	}
	CMatrix2* newrawmat;
	newrawmat = new CMatrix2[ srcmotnum ];
	if( !newrawmat ){
		return -1;
	}
	*(rawmat + newkindnum - 1) = newrawmat;
	////

	////
	firstmp = (CMotionPoint2**)realloc( firstmp, sizeof( CMotionPoint2* ) * newkindnum );
	if( !firstmp ){
		return -1;
	}
	*(firstmp + newkindnum - 1) = 0;

	firsttex = (CTexKey**)realloc( firsttex, sizeof( CTexKey* ) * newkindnum );
	if( !firsttex ){
		_ASSERT( 0 );
		return -1;
	}
	*(firsttex + newkindnum - 1) = 0;

	firstalp = (CAlpKey**)realloc( firstalp, sizeof( CAlpKey* ) * newkindnum );
	if( !firstalp ){
		_ASSERT( 0 );
		return -1;
	}
	*(firstalp + newkindnum - 1) = 0;


	firstds = (CDSKey**)realloc( firstds, sizeof( CDSKey* ) * newkindnum );
	if( !firstds ){
		_ASSERT( 0 );
		return -1;
	}
	*(firstds + newkindnum - 1) = 0;

	firstmorph = (CMMotKey**)realloc( firstmorph, sizeof( CMMotKey* ) * newkindnum );
	if( !firstmorph ){
		_ASSERT( 0 );
		return -1;
	}
	*(firstmorph + newkindnum - 1) = 0;


	/////

	kindnum = newkindnum; // 途中で失敗したときのために最後にセット

	//DbgOut( "CMotionInfo : AddMotion : %s %d %d\n", srcname, kindnum, *(motnum + newkindnum - 1) );

	
	return kindnum - 1;
}

int CMotionInfo::SetMotionMatrix( CMatrix2* transmat, int mcookie, int frameno )
{
	CMatrix2* motptr = 0;

	//DbgOut( "CMotionInfo : SetMotionMatrix : %d %d  %d %d\n", mcookie, frameno, kindnum, *(motnum + mcookie) );
	//DbgOut( "%f %f %f %f\n", (*transmat)[0][0], (*transmat)[0][1], (*transmat)[0][2], (*transmat)[0][3] );

	motptr = (*this)( mcookie, frameno );
	*motptr = *transmat;

	//DbgOut( "%f %f %f %f\n", (*motptr)[0][0], (*motptr)[0][1], (*motptr)[0][2], (*motptr)[0][3] );
	
	return 0;

}

//int CMotionInfo::SetJointValue( float srcval, int mcookie, int frameno )
//{
//
//	return 0;
//}



int CMotionInfo::SetMotionType( int srcmotkind, int srcmottype )
{
		// motionのindexno の増え方を指定。
#ifdef _DEBUG
	int ret = CheckIndex( srcmotkind );
	if( !ret ){
		*(mottype + srcmotkind) = srcmottype;
		return 0;
	}else{
		return 1;
	}
#else
	*(mottype + srcmotkind) = srcmottype;
	return 0;
#endif

}
int CMotionInfo::SetMotionJump( int srcmotkind, int srcmotjump )
{
#ifdef _DEBUG
	int ret = CheckIndex( srcmotkind );
	if( !ret ){
		if( (srcmotjump < 0) || (srcmotjump >= *(motnum + srcmotkind)) ){
			_ASSERT( 0 );
			return 1;
		}
		*(motjump + srcmotkind) = srcmotjump;
		return 0;
	}else{
		return 1;
	}
#else
	*(motjump + srcmotkind) = srcmotjump;
	return 0;
#endif
}



int CMotionInfo::SetMotionStep( int srcmotkind, int srcmotstep )
{
#ifdef _DEBUG
	int ret = CheckIndex( srcmotkind );
	if( !ret ){
		*(motstep + srcmotkind) = srcmotstep;
		return 0;
	}else{
		return 1;
	}
#else
	*(motstep + srcmotkind) = srcmotstep;
	return 0;
#endif

}

int CMotionInfo::SetInterpolationAll( int srcmotkind, int srcinterpolation )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		//DbgOut( "MotionInfo : SetInterpolation : srcmotkind error !!!return 0\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( !firstmp )
		return 0;//!!!!!!!!!!!!!!!!

	
	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	while( curmp ){
		curmp->interp = srcinterpolation;
		curmp = curmp->next;
	}

	return 0;

}


// currentデータを指定。
int CMotionInfo::SetMotionKind( int srcmotkind )
{
	// isfirst, motno もセットする。

#ifdef _DEBUG
	int ret = CheckIndex( srcmotkind );
	if( !ret ){
		kindno = srcmotkind;
		isfirst = 1;
		motno = 0;
		return 0;
	}else{
		return 1;
	}
#else
	kindno = srcmotkind;
	isfirst = 1;
	motno = 0;
	return 0;
#endif
}
int CMotionInfo::SetMotionNo( int srcmotno )
{
	int motmax;

	if( !motnum ){
		_ASSERT( 0 );
		return 1;
	}

	motmax = *(motnum + kindno);

	if( (srcmotno >= 0) && (srcmotno < motmax) ){
		motno = srcmotno;
		return 0;
	}else{
		_ASSERT( 0 );
		return 1;
	}
}

int CMotionInfo::SetMotionFrameNo( int srcmotkind, int srcframeno, int srcisfirst )
{
	int ret;

	ret = SetMotionKind( srcmotkind );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

isfirst = srcisfirst;//!!!!!!!!!!!!!!!!!!!!!

	ret = SetMotionNo( srcframeno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotionInfo::SetNextMotionFrameNo( int srcmotkind, int nextmk, int nextframeno, int befframeno )
{
	
	int chk1;
	chk1 = CheckIndex( srcmotkind );
	if( chk1 ){
		DbgOut( "mc : SetNextMotionFrameNo : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	chk1 = CheckIndex( nextmk, nextframeno );
	if( chk1 ){
		DbgOut( "mc : SetNextMotionFrameNo : nextmk or nextframeno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	NEXTMOTION* curnm;
	curnm = nextmot + srcmotkind;

	curnm->mk = nextmk;
	curnm->befframeno = befframeno;
	curnm->aftframeno = nextframeno;

	return 0;
}


int CMotionInfo::GetMotionKindNum()
{
	return kindnum;
}
int CMotionInfo::GetMotionNo()
{
	return motno;	
}
int CMotionInfo::GetMotionNum( int srcmotkind )
{
#ifdef _DEBUG
	int ret;
	ret = CheckIndex( srcmotkind );
	if( !ret ){
		return *(motnum + srcmotkind);
	}else{
		return -1;
	}
#else
	return *(motnum + srcmotkind);
#endif

}
int CMotionInfo::GetMotionType( int srcmotkind )
{
#ifdef _DEBUG
	int ret;
	ret = CheckIndex( srcmotkind );
	if( !ret ){
		return *(mottype + srcmotkind);
	}else{
		return -1;
	}	
#else
	return *(mottype + srcmotkind);
#endif
}
int CMotionInfo::GetMotionJump( int srcmotkind )
{
#ifdef _DEBUG
	int ret;
	ret = CheckIndex( srcmotkind );
	if( !ret ){
		return *(motjump + srcmotkind);
	}else{
		return -1;
	}	
#else
	return *(motjump + srcmotkind);
#endif
}


int CMotionInfo::GetMotionStep( int srcmotkind )
{
#ifdef _DEBUG
	int ret;
	ret = CheckIndex( srcmotkind );
	if( !ret ){
		return *(motstep + srcmotkind);
	}else{
		return -1;
	}
#else
	return *(motstep + srcmotkind);
#endif
}


char* CMotionInfo::Index2Name( int srcmotkind )
{
 // for debug
	int ret;
	ret = CheckIndex( srcmotkind );
	if( !ret ){
		return *(motname + srcmotkind);
	}else{
		return 0;
	}
}

		// currentデータを取得
CMatrix2* CMotionInfo::GetCurMotion()
{
	return (*this)( kindno, motno );
}
/***
int CMotionInfo::GetNextNo()
{
	if( kindno < 0 ){
		return 0;
	}

	int nextno = 0;
	int curtype;

	if( kindnum <= kindno ){
		nextno = 0;
		return nextno;
	}

	if( isfirst == 0 ){
		curtype = *(mottype + kindno);
		nextno = (this->*StepFunc[curtype])();
	}else{
		isfirst = 0;
		nextno = motno;
	}
	return nextno;
}
***/
int CMotionInfo::GetNextMotion( NEXTMOTION* pnm, int getonly )
{
	if( kindno < 0 ){
		pnm->mk = -1;
		pnm->aftframeno = 0;
		return 0;
	}

	//int nextno = 0;
	int curtype;
	int ret;
	if( kindnum <= kindno ){
		pnm->mk = -1;
		pnm->aftframeno = 0;
		return 0;
	}

	if( isfirst == 0 ){
		curtype = *(mottype + kindno);
		ret = (this->*StepFunc[curtype])( pnm );
		_ASSERT( !ret );
	}else{
		if( getonly == 0 ){
			isfirst = 0;
		}
		pnm->mk = kindno;
		pnm->aftframeno = motno;
	}
	
	return 0;
}


		// motno を　進める
CMatrix2* CMotionInfo::StepMotion( int nextno )// isfirst == 1 のときは、stepしないで、isfirst を０にセットするだけ。
{
	if( kindnum <= kindno ){
		motno = 0;
		return 0;
	}

	motno = nextno;
	return (*this)( kindno, motno );
}
DWORD CMotionInfo::StepDispSwitch( int nextno )
{
	if( kindnum <= kindno ){
		return 0xFFFFFFFF;
	}

	int ret;
	DWORD retdw;
	ret = GetDispSwitch( &retdw, kindno, nextno );
	if ( ret ){
		_ASSERT( 0 );
		return 0;
	}

	return retdw;
}


int CMotionInfo::MultMotionMatrix( CMotionInfo* srcmotinfo, int mcookie, CMatrix2 multmat )
{
	CMatrix2* srcmat;
	CMatrix2* dstmat;
	int i, srcleng, dstleng;

	srcmat = (*srcmotinfo)( mcookie );
	dstmat = (*this)( mcookie );
	srcleng = *(motnum + mcookie);
	dstleng = *( srcmotinfo->motnum + mcookie );

	if( !srcmat || !dstmat || ( srcleng != dstleng ) )
		return 1;

	for( i = 0; i < srcleng; i++ ){
		*(dstmat + i) = multmat * *(srcmat + i);
	}

	return 0;

}

int CMotionInfo::MultCopyMotionMatrix( CMotionInfo* srcmotinfo, CMatrix2 multmat, int mcookie )
{
	CMatrix2* srcmat;
	CMatrix2* dstmat;
	int i, srcleng, dstleng;

	srcmat = (*srcmotinfo)( mcookie );
	dstmat = (*this)( mcookie );
	srcleng = *(motnum + mcookie);
	dstleng = *( srcmotinfo->motnum + mcookie );

	if( !srcmat || !dstmat || ( srcleng != dstleng ) )
		return 1;


	for( i = 0; i < srcleng; i++ ){
		*(dstmat + i) = multmat * *(srcmat + i);
	}

	return 0;

}

int CMotionInfo::CopyMotionMatrix( CMotionInfo* srcmotinfo, int mcookie )
{
	CMatrix2* srcmat;
	CMatrix2* dstmat;
	int srcleng, dstleng;
	HRESULT hres;

	srcmat = (*srcmotinfo)( mcookie );
	dstmat = (*this)( mcookie );
	srcleng = *( srcmotinfo->motnum + mcookie );
	dstleng = *(motnum + mcookie);

	if( !srcmat || !dstmat || ( srcleng != dstleng ) )
		return 1;

	SetLastError( 0 );
	MoveMemory( (void*)dstmat, (void*)srcmat, sizeof( CMatrix2 ) * srcleng );
	hres = GetLastError();
	if( hres ){
		DbgOut( "CMotionInfo : CopyMotionMatrix : MoveMemory error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotionInfo::CopyMotionMatrix( CMatrix2* srcmat, int mcookie, int matnum )
{
	CMatrix2* dstmat;
	int dstleng;
	HRESULT hres;

	dstmat = (*this)( mcookie );
	dstleng = *( motnum + mcookie );

	if( !srcmat || !dstmat || (matnum > dstleng) )
		return 1;

	SetLastError( 0 );
	MoveMemory( (void*)dstmat, (void*)srcmat, sizeof( CMatrix2 ) * matnum );
	hres = GetLastError();
	if( hres ){
		DbgOut( "CMotionInfo : CopyMotionMatrix srcmat : MoveMemory error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
		// 名前以外のメンバをコピー
int CMotionInfo::CopyMotion( int dstmotkind, int srcmotkind )
{
	int ret1, ret2, oldmotnum, newmotnum;
	CMatrix2* dstmot = 0;
	CMatrix2* newmot = 0;
	CMatrix2* srcmot = 0;
	

	ret1 = CheckIndex( dstmotkind );
	ret2 = CheckIndex( srcmotkind );
	if( ret1 + ret2 )
		return 1;

	oldmotnum = *(motnum + dstmotkind);
	newmotnum = *(motnum + srcmotkind);

	*(motnum + dstmotkind) = newmotnum;
	*(mottype + dstmotkind) = *(mottype + srcmotkind);
	*(motjump + dstmotkind) = *(motjump + srcmotkind);
	*(nextmot + dstmotkind) = *(nextmot + srcmotkind);
	*(motstep + dstmotkind) = *(motstep + srcmotkind);

	srcmot = (*this)( srcmotkind, 0 );
	dstmot = (*this)( dstmotkind, 0 );
	
	
	if( oldmotnum != newmotnum ){
		// motion数が異なるときは、作り直し。
		delete [] dstmot;
		*(firstmot + dstmotkind) = 0;

		newmot = new CMatrix2[newmotnum];
		if( !newmot )
			return 1;
	}else{
		newmot = dstmot;
	}

	MoveMemory( newmot, srcmot, sizeof( CMatrix2 ) * newmotnum );

	*(firstmot + dstmotkind) = newmot;

	return 0;
}


void CMotionInfo::InitParams()
{
	CBaseDat::InitParams();

	InitMotParams();

	InitStepFunc();

	m_standard = 0;

}

void CMotionInfo::InitMotParams()
{
	kindnum = 0;// motion の種類の数
	kindno = -1;	// motion の種類の　current値
	motno = 0;	// current motion No.
	isfirst = 0;

	motname = 0;// 各motkind の motionname。 
	motnum = 0;// 各motkindno に対するmotno の最大値。
	mottype = 0;// 各motkind の motiontype
	motjump = 0;
	nextmot = 0;
	motstep = 0;// 各motkind の motno の変化幅

	firstmot = 0;// 各motkindの先頭のCMatrix2* を格納する。
	rawmat = 0;

	firstmp = 0;
	firsttex = 0;
	firstalp = 0;
	firstds = 0;
	firstmorph = 0;

	m_stepdir = 1;

	m_initmp.InitParams();

	mlmat.Identity();

	tick = 0;

	m_capmp = 0;
}
void CMotionInfo::DestroyObjs()
{

	CBaseDat::DestroyObjs();

	DestroyMotionObj();

}



int CMotionInfo::DumpMem( HANDLE hfile, int mcookie )
{
	int ret, i, j, k, matnum;
	CMatrix2* curmat = 0;

	matnum = *(motnum + mcookie);

	for( i = 0; i < matnum; i++ ){
		curmat = (*this)( mcookie, i );
		ret = Write2File( hfile, "matrixno %d\n", i );
		_ASSERT( !ret );


		for( j = 0; j < 4; j++ ){
			for( k = 0; k < 4; k++ ){
				ret += Write2File( hfile, "%f,", (*curmat)[j][k] );
				if( k == 3 )
					ret += Write2File( hfile, "\n" );
				_ASSERT( !ret );
			}
		}

		/***
		for( j = 0; j < 4; j++ ){
			ret += Write2File( hfile, "%f, %f, %f, %f\n",
				curmat[j][0], curmat[j][1], curmat[j][2], curmat[j][3] );
			_ASSERT( !ret );
		}
		***/
	}

	return ret;
}


/***
int CMotionInfo::DumpMem( HANDLE hfile, int tabnum, char* headerptr )
{
	int i, j;
	CMatrix2* curmat = 0;

	CBaseDat::DumpMem( hfile, tabnum, headerptr );

	for( i = 0; i < kindnum; i++ ){
		SetTabChar( tabnum );
		Write2File( hfile, "%s name : %s, motnum : %d, mottype : %d, motstep : %d\n",
			tabchar, *(motname + i), *(motnum + i), *(mottype + i), *(motstep + i) );

		SetTabChar( tabnum + 1 );
	
		Write2File( hfile, "%s matrix\n", tabchar );
		for( j = 0; j < *(motnum + i); j++ ){
			curmat = (*this)( i, j );
			Write2File( hfile, "%s %f, %f, %f, %f\n%s %f, %f, %f, %f\n%s %f, %f, %f, %f\n%s %f, %f, %f, %f\n",
				tabchar, curmat[0][0], curmat[0][1], curmat[0][2], curmat[0][3],
				tabchar, curmat[1][0], curmat[1][1], curmat[1][2], curmat[1][3],
				tabchar, curmat[2][0], curmat[2][1], curmat[2][2], curmat[2][3],
				tabchar, curmat[3][0], curmat[3][1], curmat[3][2], curmat[3][3]
			);
		}

	}
	return 0;
}
***/


		// motno 制御
void CMotionInfo::InitStepFunc()
{
	int i;
	for( i = 0; i < MOTIONTYPEMAX; i++ ){

		switch( i ){
		case MOTION_STOP:
			StepFunc[i] = &CMotionInfo::StepStop;
			break;
		case MOTION_CLAMP:
			StepFunc[i] = &CMotionInfo::StepClamp;
			break;
		case MOTION_ROUND:
			StepFunc[i] = &CMotionInfo::StepRound;
			break;
		case MOTION_INV:
			StepFunc[i] = &CMotionInfo::StepInv;
			break;
		case MOTION_JUMP:
			StepFunc[i] = &CMotionInfo::StepJump;
			break;
		default:
			StepFunc[i] = &CMotionInfo::StepDummy;
			break;
		}

	}
}

CMotionPoint2* CMotionInfo::GetMotionPoint( int srcmotkind )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) )
		return 0;

	return *(firstmp + srcmotkind);
}

CMotionPoint2* CMotionInfo::GetMotionPoint2( int srcmotkind, int srcmpid )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) )
		return 0;

	CMotionPoint2* curmp;
	curmp = *(firstmp + srcmotkind);

	CMotionPoint2* findmp = 0;
	while( curmp ){
		if( curmp->serialno == srcmpid ){
			findmp = curmp;
			break;
		}
		curmp = curmp->next;
	}

	return findmp;
}


CMotionPoint2* CMotionInfo::IsExistMotionPoint( int srcmotkind, int srcframeno )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : IsExistMotionPoint : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	
	if( !firstmp )
		return 0;

	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	while( curmp ){
		int cmp;
		cmp = curmp->FramenoCmp( srcframeno );
		if( cmp == 0 ){
			return curmp; // find
		}
		if( curmp->m_frameno >= srcframeno ){
			return 0;
		}
		curmp = curmp->next;
	}

	return 0;
}

CMotionPoint2* CMotionInfo::GetSmallMotionPoint( int srcmotkind, int srcframeno )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : GetSmallMotionPoint : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	
	if( !firstmp )
		return 0;

	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	CMotionPoint2* befmp = 0;
	while( curmp ){
		int cmp;
		cmp = curmp->FramenoCmp( srcframeno );
		if( cmp > 0 )
			break;
		befmp = curmp;
		curmp = curmp->next;
	}
	
	return befmp;
}
CMotionPoint2* CMotionInfo::GetNearMotionPoint( int srcmotkind, int srcframeno )
{

	CMotionPoint2* smallmp;
	CMotionPoint2* largemp;


	smallmp = GetSmallMotionPoint( srcmotkind, srcframeno );

	largemp = GetLargeMotionPoint( srcmotkind, srcframeno );

	int diffs, diffl;

	if( smallmp ){
		diffs = abs( srcframeno - smallmp->m_frameno );
	}else{
		diffs = 100000;
	}

	if( largemp ){
		diffl = abs( srcframeno - largemp->m_frameno );
	}else{
		diffl = 100000;
	}

	if( diffs <= diffl ){
		return smallmp;
	}else{
		return largemp;
	}

	return 0;
}
CMotionPoint2* CMotionInfo::GetLargeMotionPoint( int srcmotkind, int srcframeno )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : GetLargeMotionPoint : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	
	if( !firstmp )
		return 0;

	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	CMotionPoint2* findmp = 0;
	while( curmp ){
		int cmp;
		cmp = curmp->FramenoCmp( srcframeno );
		if( cmp >= 0 ){
			findmp = curmp;
			break;
		}
		curmp = curmp->next;
	}
	
	return findmp;

}


int CMotionInfo::CalcMotionPointOnFrame( CQuaternion* axisq, CShdElem* selem, CMotionPoint2* dstmp, 
	int srcmotid, int srcframeno, int* hasmpflag )
{
	// motid, frameno は、チェック済とする

	int ret;

	if( !firstmp ){
		*hasmpflag = 0;
		*dstmp = m_initmp;
		dstmp->m_spp = 0;
		return 0;
	}

	CMotionPoint2* mphead = *(firstmp + srcmotid);
	if( !mphead ){
		*hasmpflag = 0;
		*dstmp = m_initmp;
		dstmp->m_spp = 0;
		return 0;
	}

	CMotionPoint2* chkmp = mphead;
	CMotionPoint2* findmp = 0;
	CMotionPoint2* befmp = 0;
	while( chkmp ){
		if( chkmp->m_frameno == srcframeno ){
			findmp = chkmp;
			break;
		}else if( chkmp->m_frameno > srcframeno ){
			break;
		}
		befmp = chkmp;

		chkmp = chkmp->next;
	}

	if( findmp ){
		//存在するmpを返す
		*hasmpflag = 1;
		*dstmp = *findmp;
		dstmp->m_spp = 0;
	}else{
		if( befmp ){
			//befmp, befmp->next間の補間
			*hasmpflag = 2;//!!!!!!!!!!!!!!!!!!!!
			ret = FillUpMotionPointOnFrame( axisq, befmp, befmp->next, dstmp, srcframeno );
			if( ret ){
				DbgOut( "motioninfo : CalcMotionPointOnFrame : FillUpMotionPointOnFrame error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			ret = qToEuler( selem, &dstmp->m_q, &dstmp->m_eul );			
			_ASSERT( !ret );
			ret = modifyEuler( &dstmp->m_eul, &befmp->m_eul );
			_ASSERT( !ret );
		}else{
			//init
			//_ASSERT( 0 );

			*hasmpflag = 0;
			*dstmp = m_initmp;
		}
		dstmp->m_spp = 0;
	}

	return 0;
}


int CMotionInfo::GetDispSwitch( DWORD* dstdw, int srcmotkind, int srcframeno )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : GetDispSwitch : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !firstmp ){
		*dstdw = 0;
		return 0;
	}
		

	CMotionPoint2* mphead;
	mphead = *(firstmp + srcmotkind);

	*dstdw = 0;

	CMotionPoint2* chkmp = mphead;
	CMotionPoint2* findmp = 0;
	CMotionPoint2* befmp = 0;
	while( chkmp ){
		if( chkmp->m_frameno == srcframeno ){
			findmp = chkmp;
			break;
		}else if( chkmp->m_frameno > srcframeno ){
			break;
		}
		befmp = chkmp;

		chkmp = chkmp->next;
	}

	if( findmp ){
		*dstdw = findmp->dispswitch;
	}else{
		if( befmp ){
			*dstdw = befmp->dispswitch;
		}else{
			*dstdw = 0;
		}
	}


	return 0;
}

int CMotionInfo::GetInterpolation( int* dstinterp, int srcmotkind, int srcframeno )
{
//mpからinterpを取得
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : GetInterpolation : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !firstmp ){
		*dstinterp = 0;
		return 0;
	}

	CMotionPoint2* mphead;
	mphead = *(firstmp + srcmotkind);

	*dstinterp = 0;

	CMotionPoint2* chkmp = mphead;
	CMotionPoint2* findmp = 0;
	CMotionPoint2* befmp = 0;
	while( chkmp ){
		if( chkmp->m_frameno == srcframeno ){
			findmp = chkmp;
			break;
		}else if( chkmp->m_frameno > srcframeno ){
			break;
		}
		befmp = chkmp;

		chkmp = chkmp->next;
	}

	if( findmp ){
		*dstinterp = findmp->interp;
	}else{
		if( befmp ){
			*dstinterp = befmp->interp;
		}else{
			*dstinterp = 0;
		}
	}

	return 0;
}



int CMotionInfo::DestroyMotionObj( int delcookie )
{
	int kno;
	CMatrix2* curmat = 0;
	float* curval = 0;
	char* curname = 0;

//DbgOut( "mi : DestroyMotionObj %d, kindnum %d\r\n", delcookie, kindnum );


	if( delcookie == - 1 ){		
		for( kno = 0; kno < kindnum; kno++ ){
			if( motname ){
				curname = *(motname + kno);
				if( curname ){
					free( curname );
					*(motname + kno) = 0;
				}
			}
			if( rawmat ){
				curmat = *(rawmat + kno);
				if( curmat ){
					delete [] curmat;
					*(rawmat + kno) = 0;
				}
			}
			if( firstmot ){
				curmat = *(firstmot + kno);
				if( curmat ){
					delete [] curmat;
					*(firstmot + kno) = 0;
				}
			}

			if( firstmp ){
				CMotionPoint2* mphead = *(firstmp + kno);
				CMotionPoint2* delmp = mphead;
				CMotionPoint2* nextmp = 0;
				while( delmp ){
					nextmp = delmp->next;
					delete delmp;
					delmp = nextmp;
				}
				*(firstmp + kno) = 0;
			}
			if( firsttex ){
				CTexKey* texhead = *(firsttex + kno);
				CTexKey* deltex = texhead;
				CTexKey* nexttex = 0;
				while( deltex ){
					nexttex = deltex->next;
					delete deltex;
					deltex = nexttex;
				}
				*(firsttex + kno) = 0;
			}
			if( firstalp ){
				CAlpKey* alphead = *(firstalp + kno);
				CAlpKey* delalp = alphead;
				CAlpKey* nextalp = 0;
				while( delalp ){
					nextalp = delalp->next;
					delete delalp;
					delalp = nextalp;
				}
				*(firstalp + kno) = 0;
			}
			if( firstds ){
				CDSKey* dshead = *(firstds + kno);
				CDSKey* delds = dshead;
				CDSKey* nextds = 0;
				while( delds ){
					nextds = delds->next;
					delete delds;
					delds = nextds;
				}
				*(firstds + kno) = 0;
			}
			if( firstmorph ){
				CMMotKey* mmkhead = *(firstmorph + kno);
				CMMotKey* delmmk = mmkhead;
				CMMotKey* nextmmk = 0;
				while( delmmk ){
					nextmmk = delmmk->next;
					delete delmmk;
					delmmk = nextmmk;
				}
				*(firstmorph + kno) = 0;
			}
		}
		if( firstmot ){
			free( firstmot );
			firstmot = 0;
		}
		if( rawmat ){
			free( rawmat );
			rawmat = 0;
		}
		if( motname ){
			free( motname );
			motname = 0;
		}

		if( motnum ){
			free( motnum );
			motnum = 0;
		}
		if( mottype ){
			free( mottype );
			mottype = 0;
		}
		if( motjump ){
			free( motjump );
			motjump = 0;
		}
		if( nextmot ){
			free( nextmot );
			nextmot = 0;
		}
		if( motstep ){
			free( motstep );
			motstep = 0;
		}

		if( firstmp ){
			free( firstmp );
			firstmp = 0;
		}
		if( firsttex ){
			free( firsttex );
			firsttex = 0;
		}
		if( firstalp ){
			free( firstalp );
			firstalp = 0;
		}
		if( firstds ){
			free( firstds );
			firstds = 0;
		}
		if( firstmorph ){
			free( firstmorph );
			firstmorph = 0;
		}

		InitMotParams(); //！！！kindnumなどを初期化

	}else{
		if( delcookie >= kindnum ){
			//DbgOut( "mi : DestroyMotionObj : delcookie error !!!\n" );
			//_ASSERT( 0 );
			return 0;
		}

		int newkindnum = kindnum - 1;
		int mindex, setno;

		char** newname;
		newname = (char**)malloc( sizeof( char* ) * newkindnum );
		if( !newname ){
			DbgOut( "mi : DestroyMotionObj : newname alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newname + setno) = *(motname + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
		free( *(motname + delcookie) );
		free( motname );
		motname = newname;

		/////////
		int* newmotnum;
		newmotnum = (int*)malloc( sizeof( int ) * newkindnum );
		if( !newmotnum ){
			DbgOut( "mi : DestroyMotionObj : newmotnum alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newmotnum + setno) = *(motnum + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
	
		free( motnum );
		motnum = newmotnum;
		//////////
		int* newmottype;
		newmottype = (int*)malloc( sizeof( int ) * newkindnum );
		if( !newmottype ){
			DbgOut( "mi : DestroyMotionObj : newmottype alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newmottype + setno) = *(mottype + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
	
		free( mottype );
		mottype = newmottype;
		/////////////
		int* newmotstep;
		newmotstep = (int*)malloc( sizeof( int ) * newkindnum );
		if( !newmotstep ){
			DbgOut( "mi : DestroyMotionObj : newmotstep alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newmotstep + setno) = *(motstep + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
	
		free( motstep );
		motstep = newmotstep;
		///////////////
		int* newmotjump;
		newmotjump = (int*)malloc( sizeof( int ) * newkindnum );
		if( !newmotjump ){
			DbgOut( "mi : DestroyMotionObj : newmotjump alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newmotjump + setno) = *(motjump + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
	
		free( motjump );
		motjump = newmotjump;
		////////////////
		NEXTMOTION* newnextmot;
		newnextmot = (NEXTMOTION*)malloc( sizeof( NEXTMOTION ) * newkindnum );
		if( !newnextmot ){
			DbgOut( "mi : DestroyMotionObj : newnextmot alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newnextmot + setno) = *(nextmot + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );
	
		free( nextmot );
		nextmot = newnextmot;
		/////////////////
		CMatrix2** newfirstmot;
		newfirstmot = (CMatrix2**)malloc( sizeof( CMatrix2* ) * newkindnum );
		if( !newfirstmot ){
			DbgOut( "mi : DestroyMotionObj : newfirstmot alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newfirstmot + setno) = *(firstmot + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );

		delete [] *(firstmot + delcookie );
		free( firstmot );
		firstmot = newfirstmot;
		///////////////
		CMatrix2** newrawmat;
		newrawmat = (CMatrix2**)malloc( sizeof( CMatrix2* ) * newkindnum );
		if( !newrawmat ){
			DbgOut( "mi : DestroyMotionObj : newrawmat alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		setno = 0;
		for( mindex = 0; mindex < kindnum; mindex++ ){
			if( mindex != delcookie ){
				*(newrawmat + setno) = *(rawmat + mindex);
				setno++;
			}
		}
		_ASSERT( setno == newkindnum );

		delete [] *(rawmat + delcookie );
		free( rawmat );
		rawmat = newrawmat;
		////////////////
		if( firstmp ){
			CMotionPoint2** newfirstmp;
			newfirstmp = (CMotionPoint2**)malloc( sizeof( CMotionPoint2* ) * newkindnum );
			if( !newfirstmp ){
				DbgOut( "mi : DestroyMotionObj : newfirstmp alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			setno = 0;
			for( mindex = 0; mindex < kindnum; mindex++ ){
				if( mindex != delcookie ){
					*(newfirstmp + setno) = *(firstmp + mindex);
					setno++;
				}
			}
			_ASSERT( setno == newkindnum );

			CMotionPoint2* delmp;
			CMotionPoint2* nextmp;
			delmp = *(firstmp + delcookie);
			while( delmp ){
				nextmp = delmp->next;
				delete delmp;
				delmp = nextmp;
			}
			
			free( firstmp );
			firstmp = newfirstmp;
		}

		////////
		if( firsttex ){
			CTexKey** newfirsttex;
			newfirsttex = (CTexKey**)malloc( sizeof( CTexKey* ) * newkindnum );
			if( !newfirsttex ){
				DbgOut( "mi : DestroyMotionObj : newfirsttex alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			setno = 0;
			for( mindex = 0; mindex < kindnum; mindex++ ){
				if( mindex != delcookie ){
					*(newfirsttex + setno) = *(firsttex + mindex);
					setno++;
				}
			}
			_ASSERT( setno == newkindnum );

			CTexKey* deltex;
			CTexKey* nexttex;
			deltex = *(firsttex + delcookie);
			while( deltex ){
				nexttex = deltex->next;
				delete deltex;
				deltex = nexttex;
			}
			
			free( firsttex );
			firsttex = newfirsttex;
		}

		////////
		if( firstalp ){
			CAlpKey** newfirstalp;
			newfirstalp = (CAlpKey**)malloc( sizeof( CAlpKey* ) * newkindnum );
			if( !newfirstalp ){
				DbgOut( "mi : DestroyMotionObj : newfirstalp alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			setno = 0;
			for( mindex = 0; mindex < kindnum; mindex++ ){
				if( mindex != delcookie ){
					*(newfirstalp + setno) = *(firstalp + mindex);
					setno++;
				}
			}
			_ASSERT( setno == newkindnum );

			CAlpKey* delalp;
			CAlpKey* nextalp;
			delalp = *(firstalp + delcookie);
			while( delalp ){
				nextalp = delalp->next;
				delete delalp;
				delalp = nextalp;
			}
			
			free( firstalp );
			firstalp = newfirstalp;
		}

		////////

		if( firstds ){
			CDSKey** newfirstds;
			newfirstds = (CDSKey**)malloc( sizeof( CDSKey* ) * newkindnum );
			if( !newfirstds ){
				DbgOut( "mi : DestroyMotionObj : newfirstds alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			setno = 0;
			for( mindex = 0; mindex < kindnum; mindex++ ){
				if( mindex != delcookie ){
					*(newfirstds + setno) = *(firstds + mindex);
					setno++;
				}
			}
			_ASSERT( setno == newkindnum );

			CDSKey* delds;
			CDSKey* nextds;
			delds = *(firstds + delcookie);
			while( delds ){
				nextds = delds->next;
				delete delds;
				delds = nextds;
			}
			
			free( firstds );
			firstds = newfirstds;
		}

		////////
		if( firstmorph ){
			CMMotKey** newfirstmmk;
			newfirstmmk = (CMMotKey**)malloc( sizeof( CMMotKey* ) * newkindnum );
			if( !newfirstmmk ){
				DbgOut( "mi : DestroyMotionObj : newfirstmorph alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			setno = 0;
			for( mindex = 0; mindex < kindnum; mindex++ ){
				if( mindex != delcookie ){
					*(newfirstmmk + setno) = *(firstmorph + mindex);
					setno++;
				}
			}
			_ASSERT( setno == newkindnum );

			CMMotKey* delmmk;
			CMMotKey* nextmmk;
			delmmk = *(firstmorph + delcookie);
			while( delmmk ){
				nextmmk = delmmk->next;
				delete delmmk;
				delmmk = nextmmk;
			}
			
			free( firstmorph );
			firstmorph = newfirstmmk;
		}


		/////////
		kindnum = newkindnum;



		/***
	char** motname; // 各motkind の motionname。 
	int* motnum; // 各motkindno に対するmotno の最大値 + 1(要素数)。
	int* mottype; // 各motkind の motiontype
	int* motstep; // 各motkind の motno の変化幅
	int* motjump;// 各motkind の ジャンプ先フレーム番号（MOTION_JUMP時）

	NEXTMOTION* nextmot;//各motkindの最終フレーム番号の後のモーション


	//unsigned long* firstmot; // 各motkindの先頭のCMatrix2* を格納する。
	CMatrix2** firstmot;
	CMatrix2** rawmat; //親行列の掛かっていないmatrix

	float** firstval; //SHDMORPH時のjointの値

	CMotionPoint2** firstmp;
		***/


	}
	return 0;
}

CMotionPoint2* CMotionInfo::AddMotionPoint( int srcmotkind, int srcframeno,
	CQuaternion srcq,
	float srcmvx, float srcmvy, float srcmvz, DWORD srcdswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1 )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : AddMotionPoint : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + srcmotkind);
	if( (srcframeno < 0) || (srcframeno >= frameleng) ){
		DbgOut( "motinfo : AddMotionPoint : srcframeno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	CMotionPoint2* newmp;
	newmp = new CMotionPoint2();
	if( !newmp ){
		DbgOut( "MotionInfo : AddMotionPoint : newmp alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	int ret;
	ret = newmp->SetParams( srcframeno, srcq, srcmvx, srcmvy, srcmvz, srcdswitch, srcinterp, scalex, scaley, scalez, userint1 );
	if( ret ){
		DbgOut( "MotionInfo : AddMotionPoint : SetParams error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	
	ret = newmp->CreateSppIfNot();
	if( ret ){
		DbgOut( "mi : AddMotionPoint : mp CreateSppIfNot error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	CMotionPoint2* largermp = 0;
	CMotionPoint2* smallermp = 0;
	int cmp;
	while( curmp ){
		cmp = newmp->FramenoCmp( curmp );
		if( cmp < 0 ){
			largermp = curmp;
			break;
		}else{
			smallermp = curmp;
		}
		curmp = curmp->next;
	}

	if( !mphead ){
		//先頭に追加
		newmp->ishead = 1;
		*(firstmp + srcmotkind) = newmp;
	}else{
		if( largermp ){
			//largermpの前に追加。
			ret = largermp->AddToPrev( newmp );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( newmp->ishead )
				*(firstmp + srcmotkind) = newmp;
		}else{
			//最後に追加。(smallermp の後に追加)
			_ASSERT( smallermp );
			ret = smallermp->AddToNext( newmp );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}
	return newmp;
}

CMotionPoint2* CMotionInfo::AddMotionPoint( int srcmotkind, int srcframeno,
	CQuaternion srcq,
	float srcmvx, float srcmvy, float srcmvz, DWORD srcdswitch, int srcinterp, float scalex, float scaley, float scalez, int userint1,
	D3DXVECTOR3 srceul )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : AddMotionPoint : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + srcmotkind);
	if( (srcframeno < 0) || (srcframeno >= frameleng) ){
		DbgOut( "motinfo : AddMotionPoint : srcframeno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	CMotionPoint2* newmp;
	newmp = new CMotionPoint2();
	if( !newmp ){
		DbgOut( "MotionInfo : AddMotionPoint : newmp alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	int ret;
	ret = newmp->SetParams( srcframeno, srcq, srcmvx, srcmvy, srcmvz, srcdswitch, srcinterp, scalex, scaley, scalez, userint1 );
	if( ret ){
		DbgOut( "MotionInfo : AddMotionPoint : SetParams error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	
	ret = newmp->CreateSppIfNot();
	if( ret ){
		DbgOut( "mi : AddMotionPoint : mp CreateSppIfNot error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	newmp->m_eul = srceul;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	CMotionPoint2* largermp = 0;
	CMotionPoint2* smallermp = 0;
	int cmp;
	while( curmp ){
		cmp = newmp->FramenoCmp( curmp );
		if( cmp < 0 ){
			largermp = curmp;
			break;
		}else{
			smallermp = curmp;
		}
		curmp = curmp->next;
	}

	if( !mphead ){
		//先頭に追加
		newmp->ishead = 1;
		*(firstmp + srcmotkind) = newmp;
	}else{
		if( largermp ){
			//largermpの前に追加。
			ret = largermp->AddToPrev( newmp );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( newmp->ishead )
				*(firstmp + srcmotkind) = newmp;
		}else{
			//最後に追加。(smallermp の後に追加)
			_ASSERT( smallermp );
			ret = smallermp->AddToNext( newmp );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}
	return newmp;
}


/***
CMotionPoint2* CMotionInfo::AddMotionPoint( int srcmotkind, int srcframeno,
	float srcrotx, float srcroty, float srcrotz,
	float srcmvx, float srcmvy, float srcmvz )
{
	
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : AddMotionPoint : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	CMotionPoint2* newmp;
	newmp = new CMotionPoint2();
	if( !newmp ){
		DbgOut( "MotionInfo : AddMotionPoint : newmp alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	int ret;
	ret = newmp->SetParams( srcframeno, srcrotx, srcroty, srcrotz, srcmvx, srcmvy, srcmvz );
	if( ret ){
		DbgOut( "MotionInfo : AddMotionPoint : SetParams error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	
	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	CMotionPoint2* largermp = 0;
	CMotionPoint2* smallermp = 0;
	int cmp;
	while( curmp ){
		cmp = newmp->FramenoCmp( curmp );
		if( cmp < 0 ){
			largermp = curmp;
			break;
		}else{
			smallermp = curmp;
		}
		curmp = curmp->next;
	}

	if( !mphead ){
		//先頭に追加
		newmp->ishead = 1;
		*(firstmp + srcmotkind) = newmp;
	}else{
		if( largermp ){
			//largermpの前に追加。
			ret = largermp->AddToPrev( newmp );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( newmp->ishead )
				*(firstmp + srcmotkind) = newmp;
		}else{
			//最後に追加。(smallermp の後に追加)
			_ASSERT( smallermp );
			ret = smallermp->AddToNext( newmp );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}
	return newmp;
}
***/
CMotionPoint2* CMotionInfo::LeaveFromChainMP( int srcmotkind, int srcframeno )
{
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : LeaveFromChainMP : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	CMotionPoint2* curmp = mphead;
	CMotionPoint2* delmp = 0;
	int cmp;
	while( curmp ){
		cmp = curmp->FramenoCmp( srcframeno );
		if( cmp == 0 ){
			delmp = curmp;
			break;
		}
		curmp = curmp->next;
	}
	if( !delmp ){
		DbgOut( "MotionInfo : LeaveFromChainMP : delmp NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//DbgOut( "MotionInfo : DeleteMotionPoint : delmp : frame %d, prev %x, next %x\n",
//	   delmp->m_frameno, delmp->prev, delmp->next );

	if( delmp->ishead ){
		CMotionPoint2* newhead = delmp->next;
		*(firstmp + srcmotkind) = newhead;
	}

	int ret;
	ret = delmp->LeaveFromChain();
	if( ret ){
		_ASSERT( 0 );
		return 0;
	}

	return delmp;
}

int CMotionInfo::DeleteMotionPoint( int srcmotkind, int srcframeno )
{
	CMotionPoint2* delmp;
	delmp = LeaveFromChainMP( srcmotkind, srcframeno );
	if( delmp )
		delete delmp;

	return 0;
}


int CMotionInfo::MakeRawmat( int srcmotkind, CShdElem* lpselem, int srcstart, int srcend, CShdHandler* lpsh, CMotHandler* lpmh, int calceulflag )
{

	int ret;

	//MotionPointのチェインをもとに、rawmatをセット。
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : MakeRawmat : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int matnum = *(motnum + srcmotkind );
	if( matnum <= 0 ){
		_ASSERT( 0 );	
		return 0;
	}

	CMatrix2* rmhead = *(rawmat + srcmotkind);
	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	if( !mphead ){
		for( int i = srcstart; i <= srcend; i++ ){
			(rmhead + i)->Identity();
		}
		return 0;
	}
	


	CMotionPoint2* tmpmp;
	int allocflag = 0;
	if( (matnum <= TEMPMPLENG) && s_tempmp ){
		tmpmp = s_tempmp;
		int mpno;
		for( mpno = srcstart; mpno <= srcend; mpno++ ){
			(tmpmp + mpno)->InitParams();
		}

		allocflag = 0;
	}else{
		tmpmp = new CMotionPoint2[ matnum ];
		if( !tmpmp ){
			DbgOut( "motioninfo : MakeRawmat : tmpmp alloc error !!!\n" );
			_ASSERT( 0 );
			return -1;
		}
		allocflag = 1;
	}
	

//if( matnum != (srcend - srcstart + 1) ){
//	DbgOut( "motioninfo : MakeRawmat : alert : matnum %d, srcstart %d, srcend %d\n", matnum, srcstart, srcend );
//}


	if( calceulflag == 1 ){
		ret = PutQuaternionInOrder( srcmotkind );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	

	//CMotionPoint2* beginmp;
	CMotionPoint2* finishmp;

	CMotionPoint2* startmp;
	CMotionPoint2* endmp;

	CMotionPoint2* smallmp;
	CMotionPoint2* nearmp;

	smallmp = GetSmallMotionPoint( srcmotkind, srcstart );
	nearmp = GetNearMotionPoint( srcmotkind, srcstart );
	finishmp = GetLargeMotionPoint( srcmotkind, srcend );

	if( smallmp ){
		startmp = smallmp;
	}else{
		if( !nearmp ){
			startmp = 0;
		}else{
			startmp = nearmp;
		}
	}

	if( calceulflag == 1 ){
		ret = CalcMPEuler( lpmh, lpsh, lpselem, srcmotkind, lpmh->GetZaType( srcmotkind ) );
		_ASSERT( !ret );
	}
		
	CQuaternion axisq;
	int isfirst = 1;
	int calcframe;
	while( startmp ){
		if( isfirst && (srcstart < startmp->m_frameno) ){
			for( calcframe = srcstart; calcframe <= startmp->m_frameno; calcframe++ ){

				ret = lpmh->SetBoneAxisQ( lpsh, lpselem->serialno, lpmh->GetZaType( srcmotkind ), srcmotkind, calcframe );
				_ASSERT( !ret );
				ret = lpselem->GetBoneAxisQ( &axisq );
				_ASSERT( !ret );

				ret = FillUpMotionPointOnFrame( &axisq, 0, startmp, tmpmp + calcframe, calcframe );
				if( ret ){
					DbgOut( "motioninfo : MakeRawmat : FillUpMotionPointOnFrame error !!!\n" );
					_ASSERT( 0 );
					if( allocflag ){
						delete [] tmpmp;
					}
					return 1;
				}
			}
		}

		isfirst = 0;
		endmp = startmp->next;
		int finframe;
		if( endmp )
			finframe = endmp->m_frameno;
		else
			finframe = srcend;
		
		for( calcframe = startmp->m_frameno; calcframe <= finframe; calcframe++ ){

			ret = lpmh->SetBoneAxisQ( lpsh, lpselem->serialno, lpmh->GetZaType( srcmotkind ), srcmotkind, calcframe );
			_ASSERT( !ret );
			ret = lpselem->GetBoneAxisQ( &axisq );
			_ASSERT( !ret );

			ret = FillUpMotionPointOnFrame( &axisq, startmp, endmp, tmpmp + calcframe, calcframe );
			if( ret ){
				DbgOut( "motioninfo : MakeRawmat : FillUpMotionPointOnFrame error !!!\n" );
				_ASSERT( 0 );
				if( allocflag ){
					delete [] tmpmp;
				}
				return 1;
			}
		}

		if( startmp == finishmp ){
			//srcstart == srcend
			break;
		}
		startmp = startmp->next;

		if( startmp == finishmp ){
		//if( startmp->m_frameno >= finishmp->m_frameno ){
			break;//!!!!!!!!!!!!!!!
		}
	}


	//ret = MPtoRawmat( srcmotkind, tmpmp, matnum, lpselem );
	ret = MPtoRawmat( srcmotkind, tmpmp, srcstart, srcend, lpselem );
	if( ret ){
		_ASSERT( 0 );
		if( allocflag ){
			delete [] tmpmp;
		}
		return 1;
	}

	if( allocflag ){
		delete [] tmpmp;
	}



	return 0;
}

int CMotionInfo::MakeRawmatCap( int srcmotkind, CShdElem* lpselem, int srcframe, CShdHandler* lpsh, CMotHandler* lpmh, CMotionPoint2* srcmp )
{


	m_capmp = srcmp;

	//MotionPointのチェインをもとに、rawmatをセット。
	if( (srcmotkind >= kindnum) || (srcmotkind < 0) ){
		DbgOut( "MotionInfo : MakeRawmatCap : srcmotkind error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int matnum = *(motnum + srcmotkind );
	if( matnum <= 0 ){
		_ASSERT( 0 );	
		return 0;
	}

	CMatrix2* rawhead = *(rawmat + srcmotkind);
	CMotionPoint2* mphead = *(firstmp + srcmotkind);
	if( !mphead ){
		(rawhead + srcframe)->Identity();
		return 0;
	}

//////////

	float centerx, centery, centerz;
	if( m_standard == 1 ){
		CPart* partptr = lpselem->part;
		centerx = partptr->jointloc.x;
		centery = partptr->jointloc.y;
		centerz = partptr->jointloc.z;
	}else{
		CShdElem* parselem;
		parselem = lpselem->parent;
		if( parselem && ( parselem->IsJoint() && parselem->type != SHDMORPH) ){
			CPart* partptr = parselem->part;
			if( !partptr ){
				_ASSERT( 0 );
				return 1;
			}
			centerx = partptr->jointloc.x;
			centery = partptr->jointloc.y;
			centerz = partptr->jointloc.z;
		
		}else{
			centerx = 0.0f;
			centery = 0.0f;
			centerz = 0.0f;
		}
	}

	CMatrix2 befrotmat, aftrotmat;
	befrotmat.SetMoveMat( -centerx, -centery, -centerz );
	aftrotmat.SetMoveMat( centerx, centery, centerz );

	int matno = srcmp->m_frameno;
	CMatrix2 rotmat;
	CQuaternion curq = srcmp->m_q;
	rotmat = curq.MakeRotMat();

	CMatrix2 mvmat;
	mvmat.SetMoveMat( srcmp->m_mvx, srcmp->m_mvy, srcmp->m_mvz );

	CMatrix2 scmat;
	scmat.SetScaleMat( srcmp->m_scalex, srcmp->m_scaley, srcmp->m_scalez );


	CMatrix2* dstmat = rawhead + matno;
	*dstmat = befrotmat * scmat * rotmat * aftrotmat * mvmat;

	return 0;
}


int CMotionInfo::PutQuaternionInOrder( int srcmotkind )
{
	CMotionPoint2* startmp = *(firstmp + srcmotkind);

	if( !startmp )
		return 0;

	CMotionPoint2* curmp = startmp;
	CMotionPoint2* nextmp = startmp->next;

	while( nextmp ){

		float kaku;
		kaku = (curmp->m_q).CalcRad( nextmp->m_q );
		if( kaku > (PI * 0.5f) ){
			//片方を-qにすれば、(PI * 0.5f)より小さくなる。（最短コースをたどれる）
			nextmp->m_q = -(nextmp->m_q);
			//kaku = this->CalcRad( endq );
			//_ASSERT( kaku <= (PI * 0.5f) );
		}

		curmp = nextmp;
		nextmp = nextmp->next;
	}


	return 0;
}

int CMotionInfo::FillUpMotionPointOnFrame( CQuaternion* axisq, CMotionPoint2* srcstartmp, CMotionPoint2* srcendmp, CMotionPoint2* dstmp, int srcframe )
{
//	PutQuaternionInOrder( srcmotkind );

	int startframe, endframe, framenum;
	CQuaternion startq, endq;
	
	if( !srcstartmp && !srcendmp )
		return 1;

	if( srcstartmp ){
		startq = srcstartmp->m_q;	
		startframe = srcstartmp->m_frameno;
	}else{
		//純虚数
		_ASSERT( srcendmp );
		startframe = 0;
		endframe = srcendmp->m_frameno;

		CQuaternion tempq;
		tempq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
			
		dstmp->m_q = tempq;

		dstmp->m_mvx = 0.0f;
		dstmp->m_mvy = 0.0f;
		dstmp->m_mvz = 0.0f;		

		dstmp->dispswitch = 0;
		dstmp->interp = 0;

		dstmp->m_scalex = 1.0f;
		dstmp->m_scaley = 1.0f;
		dstmp->m_scalez = 1.0f;
		return 0;
	}

	if( srcendmp ){
		endq = srcendmp->m_q;
		endframe = srcendmp->m_frameno;
		framenum = endframe - startframe;
	}else{
		//startqと同じ
		_ASSERT( srcstartmp );
		//endframe = srcmpnum - 1;

		dstmp->m_q = startq;

		dstmp->m_mvx = srcstartmp->m_mvx;
		dstmp->m_mvy = srcstartmp->m_mvy;
		dstmp->m_mvz = srcstartmp->m_mvz;

		dstmp->dispswitch = srcstartmp->dispswitch;
		dstmp->interp = srcstartmp->interp;

		dstmp->m_scalex = srcstartmp->m_scalex;
		dstmp->m_scaley = srcstartmp->m_scaley;
		dstmp->m_scalez = srcstartmp->m_scalez;
		return 0;
	}


	if( (srcframe < 0) || (srcframe > endframe) ){
		DbgOut( "motioninfo : FillUpMotionPointOnFrame : srcframe error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	if( srcframe == startframe ){
		dstmp->CopyMotion( srcstartmp );
	}else if( srcframe == endframe ){
		dstmp->CopyMotion( srcendmp );
	}else{
		int framecnt;
		framecnt = srcframe - startframe;
		ret = dstmp->FillUpMotionPoint( axisq, srcstartmp, srcendmp, framenum, framecnt, srcstartmp->interp );
		if( ret ){
			_ASSERT( 0 );
			return 0;
		}
	}

	return 0;

}


int CMotionInfo::FillUpMotionPoint( CQuaternion* axisq, CMotionPoint2* srcstartmp, CMotionPoint2* srcendmp, 
	CMotionPoint2* dstmp, int srcmpnum, int interp )
{
//	PutQuaternionInOrder( srcmotkind );

	int startframe, endframe, framenum, frameno;
	CQuaternion startq, endq;
	
	if( !srcstartmp && !srcendmp )
		return 1;

	if( srcstartmp ){
		startq = srcstartmp->m_q;	
		startframe = srcstartmp->m_frameno;
	}else{
		//startからendまで、純虚数
		_ASSERT( srcendmp );
		startframe = 0;
		endframe = srcendmp->m_frameno;

		CQuaternion tempq;
		tempq.SetParams( 1.0f, 0.0f, 0.0f, 0.0f );
		for( frameno = startframe; frameno <= endframe; frameno++ ){
			CMotionPoint2* dstptr = dstmp + frameno;
			dstptr->m_q = tempq;

			dstptr->m_mvx = 0.0f;
			dstptr->m_mvy = 0.0f;
			dstptr->m_mvz = 0.0f;
			
			dstptr->dispswitch = 0;
			dstptr->interp = 0;
		}
		return 0;
	}

	if( srcendmp ){
		endq = srcendmp->m_q;
		endframe = srcendmp->m_frameno;
		framenum = endframe - startframe;
	}else{
		//startから最後まで、startqと同じ
		_ASSERT( srcstartmp );
		endframe = srcmpnum - 1;
		for( frameno = startframe; frameno <= endframe; frameno++ ){
			CMotionPoint2* dstptr = dstmp + frameno;
			dstptr->m_q = startq;

//DbgOut( "MotionInfo : FillUpMotionPoint 1: frame %d, %f, %f, %f, %f\n",
//	   frameno, startq.w, startq.x, startq.y, startq.z );

			dstptr->m_mvx = srcstartmp->m_mvx;
			dstptr->m_mvy = srcstartmp->m_mvy;
			dstptr->m_mvz = srcstartmp->m_mvz;

			dstptr->dispswitch = srcstartmp->dispswitch;
			dstptr->interp = srcstartmp->interp;
		}
		return 0;
	}


	int ret;
	int framecnt = 0;
	for( frameno = startframe; frameno <= endframe; frameno++ ){
		CMotionPoint2* dstptr = dstmp + frameno;

		if( frameno == startframe ){
			dstptr->CopyMotion( srcstartmp );
		}else if( frameno == endframe ){
			dstptr->CopyMotion( srcendmp );
		}else{
			ret = dstptr->FillUpMotionPoint( axisq, srcstartmp, srcendmp, framenum, framecnt, interp );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}

		framecnt++;
	}



	return 0;
}


int CMotionInfo::MPtoRawmat( int srcmotkind, CMotionPoint2* srcmp, int srcstart, int srcend, CShdElem* lpselem )
{
	CMatrix2* rawhead = *(rawmat + srcmotkind);
	if( !rawhead ){
		_ASSERT( 0 );
		return 1;
	}

	for( int i = srcstart; i <= srcend; i++ ){
		(rawhead + i)->Identity();
	}
	

	float centerx, centery, centerz;
	if( m_standard == 1 ){
		CPart* partptr = lpselem->part;
		centerx = partptr->jointloc.x;
		centery = partptr->jointloc.y;
		centerz = partptr->jointloc.z;
	}else{
		CShdElem* parselem;
		parselem = lpselem->parent;
		if( parselem && ( parselem->IsJoint() && parselem->type != SHDMORPH) ){
			CPart* partptr = parselem->part;
			if( !partptr ){
				_ASSERT( 0 );
				return 1;
			}
			centerx = partptr->jointloc.x;
			centery = partptr->jointloc.y;
			centerz = partptr->jointloc.z;
		
		}else{
			centerx = 0.0f;
			centery = 0.0f;
			centerz = 0.0f;
		}
	}

	CMatrix2 befrotmat, aftrotmat;
	befrotmat.SetMoveMat( -centerx, -centery, -centerz );
	aftrotmat.SetMoveMat( centerx, centery, centerz );

	int matno;
	for( matno = srcstart; matno <= srcend; matno++ ){
		CMatrix2 rotmat;
		CMotionPoint2* curmp = srcmp + matno;
		CQuaternion curq = curmp->m_q;
		rotmat = curq.MakeRotMat();

		CMatrix2 mvmat;
		mvmat.SetMoveMat( curmp->m_mvx, curmp->m_mvy, curmp->m_mvz );

		CMatrix2 scmat;
		scmat.SetScaleMat( curmp->m_scalex, curmp->m_scaley, curmp->m_scalez );


		CMatrix2* dstmat = rawhead + matno;
		//*dstmat = befrotmat * rotmat * aftrotmat * mvmat;

		*dstmat = befrotmat * scmat * rotmat * aftrotmat * mvmat;//2005/04/22


		//invmvmat * invaftrotmat * invrotmat * invscmat * invbefrotmat * befrotmat * rotmat * aftrotmat * mvmat
		
	}
	
	return 0;
}

int CMotionInfo::CopyMMotAnimFrame( CMotionCtrl* mcptr, int srcmotid, int srcframe, int dstmotid, int dstframe )
{
	int ret;
	CMMotKey* srcfmmk;
	srcfmmk = GetFirstMMotKey( srcmotid );

	CMMotKey* dstfmmk;
	dstfmmk = GetFirstMMotKey( dstmotid );

	if( !srcfmmk && !dstfmmk ){
		return 0;
	}

	int baseno;
	for( baseno = 0; baseno < mcptr->curmmenum; baseno++ ){
		CMMotElem* mcmme;
		mcmme = *( mcptr->curmmotelem + baseno );

		int targetno;
		for( targetno = 0; targetno < mcmme->m_primnum; targetno++ ){
			MPRIM* mcprim;
			mcprim = mcmme->m_primarray + targetno;

			int existflag;
			float srcvalue;
			ret = GetMMotAnim( &srcvalue, srcmotid, srcframe, mcmme->m_baseelem, mcprim->selem, &existflag );
			if( ret ){
				DbgOut( "mi : CopyMMotAnimFrame : GetMMotAnim error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = SetMMotValue( dstmotid, dstframe, mcmme->m_baseelem, mcprim->selem, srcvalue );
			if( ret ){
				DbgOut( "mi : CopyMMotAnimFrame : SetMMotValue error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}


int CMotionInfo::CopyMotionFrame( CShdElem* selem, int srcmotid, int srcframe, int dstmotid, int dstframe, CShdHandler* lpsh, CMotHandler* lpmh )
{
	// motid, frameno は、チェック済とする
	
	int ret;

	ret = lpmh->SetBoneAxisQ( lpsh, selem->serialno, lpmh->GetZaType( srcmotid ), srcmotid, srcframe );
	_ASSERT( !ret );
	CQuaternion axisq;
	ret = selem->GetBoneAxisQ( &axisq );
	_ASSERT( !ret );


	int hasmpflag = 0;
	CMotionPoint2 calcmp;
	ret = CalcMotionPointOnFrame( &axisq, selem, &calcmp, srcmotid, srcframe, &hasmpflag );
	if( ret ){
		DbgOut( "motioninfo : CopyMotionFrame : CalcMotionPointOnFrame error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	if( hasmpflag ){//これを有効にするとモーションキーのない所の補間がうまくいかない。
		CMotionPoint2* isexist;
		isexist = IsExistMotionPoint( dstmotid, dstframe );
		if( isexist ){
			//チェイン以外をコピー
			ret = isexist->CopyMotion( &calcmp );
			if( ret ){
				DbgOut( "motioninfo : CopyMotionFrame : isexist CopyMotion error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			CMotionPoint2* newmp;
			newmp = AddMotionPoint( dstmotid, dstframe,
				calcmp.m_q, calcmp.m_mvx, calcmp.m_mvy, calcmp.m_mvz, calcmp.dispswitch, calcmp.interp, calcmp.m_scalex, calcmp.m_scaley, calcmp.m_scalez, calcmp.m_userint1, calcmp.m_eul );
			if( !newmp ){
				DbgOut( "motioninfo : CopyMotionFrame : AddMotionPoint error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
//	}

	return 0;

}

int CMotionInfo::GetCurrentRawMat( CMatrix2* rawmptr )
{

	CMatrix2* rawhead = *(rawmat + kindno);
	if( !rawhead ){
		DbgOut( "motinfo : GetCurrentRawMat : rawmat NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*rawmptr = *( rawhead + motno );

	return 0;
}

int CMotionInfo::SetMotionName( int motid, char* srcname )
{
	if( (motid < 0) || (motid >= kindnum) ){
		DbgOut( "mi : SetMotionName : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( !motname ){
		DbgOut( "mi : SetMotionName : motname NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int leng;
	leng = (int)strlen( srcname );

	char* newname;
	newname = (char*)malloc( sizeof( char ) * (leng + 1) );
	if( !newname ){
		DbgOut( "mi : SetMotionName : newname alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	strcpy_s( newname, leng + 1, srcname );

	free( *(motname + motid) );
	*(motname + motid) = newname;

	return 0;
}

int CMotionInfo::CalcMPEuler( CMotHandler* lpmh, CShdHandler* lpsh, CShdElem* selem, int motid, int zakind )
{

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : CalcMPEuler motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( !firstmp )
		return 0;//!!!!!!!!!!!!!!!!
	
	int ret;
	CMotionPoint2* curmp = *( firstmp + motid );
	D3DXVECTOR3 befeul( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 neweul;
	while( curmp ){

		ret = lpmh->SetBoneAxisQ( lpsh, selem->serialno, zakind, motid, curmp->m_frameno );
		_ASSERT( !ret );

		ret = qToEuler( selem, &curmp->m_q, &neweul );
		_ASSERT( !ret );

		ret = modifyEuler( &neweul, &befeul );
		_ASSERT( !ret );

		curmp->m_eul = neweul;

		curmp = curmp->next;
		befeul = neweul;
	}

	return 0;
}

int CMotionInfo::ExistTexKey( int motid, int frameno, CTexKey** pptexkey )
{
	*pptexkey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ExistTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : ExistTexKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CTexKey* curtk = *( firsttex + motid );
	while( curtk && (curtk->frameno <= frameno) ){
		if( curtk->frameno == frameno ){
			*pptexkey = curtk;
			break;
		}

		curtk = curtk->next;
	}

	return 0;
}

int CMotionInfo::CreateTexKey( int motid, int frameno, CTexKey** pptexkey )
{
	*pptexkey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : CreateTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : CreateTexKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CTexKey* newtk = 0;
	newtk = new CTexKey();
	if( !newtk ){
		DbgOut( "mi : CreateTexKey : newtk alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newtk->frameno = frameno;


//////////////////
	int ret;
	CTexKey* tkhead = *(firsttex + motid);
	CTexKey* curtk = tkhead;
	CTexKey* largertk = 0;
	CTexKey* smallertk = 0;
	int cmp;
	while( curtk ){
		cmp = newtk->FramenoCmp( curtk );
		if( cmp < 0 ){
			largertk = curtk;
			break;
		}else{
			smallertk = curtk;
		}
		curtk = curtk->next;
	}

	if( !tkhead ){
		//先頭に追加
		*(firsttex + motid) = newtk;
	}else{
		if( largertk ){
			//largertkの前に追加。
			ret = largertk->AddToPrev( newtk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newtk->prev )
				*(firsttex + motid) = newtk;
		}else{
			//最後に追加。(smallertk の後に追加)
			_ASSERT( smallertk );
			ret = smallertk->AddToNext( newtk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*pptexkey = newtk;

	return 0;
}

CTexKey* CMotionInfo::GetFirstTexKey( int motid )
{

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : GetFirstTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( !firsttex ){
		DbgOut( "mi : GetFirstTexKey : firsttex NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	return *( firsttex + motid );
}

int CMotionInfo::DeleteTexKey( int motid, int frameno )
{
	CTexKey* deltk = 0;
	deltk = LeaveFromChainTexKey( motid, frameno );
	if( deltk ){
		delete deltk;
	}

	return 0;
}

CTexKey* CMotionInfo::LeaveFromChainTexKey( int motid, int frameno )
{

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : LeaveFromChainTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : LeaveFromChainTexKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
////////////////

	int ret;
	CTexKey* curtk = 0;
	ret = ExistTexKey( motid, frameno, &curtk );
	if( ret ){
//		DbgOut( "mi : LeaveFromChainTexKey : ExistTexKey error !!!\n" );
//		_ASSERT( 0 );
		return 0;
	}

	if( curtk ){
		CTexKey* firsttk;
		firsttk = *( firsttex + motid );
		if( firsttk == curtk ){
			*( firsttex + motid ) = curtk->next;
		}

		curtk->LeaveFromChain();
	}

	return curtk;
}

int CMotionInfo::ChainTexKey( int motid, CTexKey* addtk )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ChainTexKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (addtk->frameno < 0) || (addtk->frameno >= frameleng) ){
		DbgOut( "motinfo : ChainTexKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////////


	int ret;
	CTexKey* tkhead = *(firsttex + motid);
	CTexKey* curtk = tkhead;
	CTexKey* largertk = 0;
	CTexKey* smallertk = 0;
	int cmp;
	while( curtk ){
		cmp = addtk->FramenoCmp( curtk );
		if( cmp < 0 ){
			largertk = curtk;
			break;
		}else{
			smallertk = curtk;
		}
		curtk = curtk->next;
	}

	if( !tkhead ){
		//先頭に追加
		*(firsttex + motid) = addtk;
	}else{
		if( largertk ){
			//largertkの前に追加。
			ret = largertk->AddToPrev( addtk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addtk->prev )
				*(firsttex + motid) = addtk;
		}else{
			//最後に追加。(smallertk の後に追加)
			_ASSERT( smallertk );
			ret = smallertk->AddToNext( addtk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CMotionInfo::DeleteTexKeyOutOfRange( int motid, int srcmaxframe )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : DeleteTexKeyOutOfRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////

	int ret;
	CTexKey* tkptr = *( firsttex + motid );
	CTexKey* nexttk = 0;
	while( tkptr ){
		nexttk = tkptr->next;
		int frameno = tkptr->frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteTexKey( motid, frameno );
			if( ret ){
				DbgOut( "mi : DeleteTexKeyOutOfRange : DeleteTexKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		tkptr = nexttk;
	}

	return 0;
}

int CMotionInfo::SetTexAnim()
{
//kindno, motno

	CTexKey* tkptr;
	tkptr = *( firsttex + kindno );

	while( tkptr ){
		if( tkptr->frameno <= motno ){
			int tcno;
			for( tcno = 0; tcno < tkptr->changenum; tcno++ ){
				CTexChange* tcptr;
				tcptr = *( tkptr->texchange + tcno );
				tcptr->m_mqomat->curtexname = tcptr->m_change;
			}
		}else{
			break;
		}
		tkptr = tkptr->next;
	}

	return 0;
}

int CMotionInfo::GetTexAnim( int motid, int frameno )
{

	CTexKey* tkptr;
	tkptr = *( firsttex + motid );

	while( tkptr ){
//_ASSERT( 0 );
		if( tkptr->frameno <= frameno ){
			int tcno;
			for( tcno = 0; tcno < tkptr->changenum; tcno++ ){
				CTexChange* tcptr;
				tcptr = *( tkptr->texchange + tcno );
				tcptr->m_mqomat->curtexname = tcptr->m_change;
			}
		}else{
			break;
		}
		tkptr = tkptr->next;
	}

	return 0;


}


int CMotionInfo::ExistDSKey( int motid, int frameno, CDSKey** ppdskey )
{
	*ppdskey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ExistDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : ExistDSKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CDSKey* curdsk = *( firstds + motid );
	while( curdsk && (curdsk->frameno <= frameno) ){
		if( curdsk->frameno == frameno ){
			*ppdskey = curdsk;
			break;
		}

		curdsk = curdsk->next;
	}

	return 0;
}

int CMotionInfo::CreateDSKey( int motid, int frameno, CDSKey** ppdskey )
{
	*ppdskey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : CreateDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : CreateDSKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CDSKey* newdsk = 0;
	newdsk = new CDSKey();
	if( !newdsk ){
		DbgOut( "mi : CreateDSKey : newdsk alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newdsk->frameno = frameno;


//////////////////
	int ret;
	CDSKey* dskhead = *(firstds + motid);
	CDSKey* curdsk = dskhead;
	CDSKey* largerdsk = 0;
	CDSKey* smallerdsk = 0;
	int cmp;
	while( curdsk ){
		cmp = newdsk->FramenoCmp( curdsk );
		if( cmp < 0 ){
			largerdsk = curdsk;
			break;
		}else{
			smallerdsk = curdsk;
		}
		curdsk = curdsk->next;
	}

	if( !dskhead ){
		//先頭に追加
		*(firstds + motid) = newdsk;
	}else{
		if( largerdsk ){
			//largerdskの前に追加。
			ret = largerdsk->AddToPrev( newdsk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newdsk->prev )
				*(firstds + motid) = newdsk;
		}else{
			//最後に追加。(smallertk の後に追加)
			_ASSERT( smallerdsk );
			ret = smallerdsk->AddToNext( newdsk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppdskey = newdsk;

	return 0;
}

CDSKey* CMotionInfo::GetFirstDSKey( int motid )
{

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : GetFirstDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( !firstds ){
		DbgOut( "mi : GetFirstDSKey : firstds NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	return *( firstds + motid );
}

int CMotionInfo::DeleteDSKey( int motid, int frameno )
{
	CDSKey* deldsk = 0;
	deldsk = LeaveFromChainDSKey( motid, frameno );
	if( deldsk ){
		delete deldsk;
	}

	return 0;
}

CDSKey* CMotionInfo::LeaveFromChainDSKey( int motid, int frameno )
{

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : LeaveFromChainDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : LeaveFromChainDSKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
////////////////

	int ret;
	CDSKey* curdsk = 0;
	ret = ExistDSKey( motid, frameno, &curdsk );
	if( ret ){
		//DbgOut( "mi : LeaveFromChainDSKey : ExistDSKey error !!!\n" );
		//_ASSERT( 0 );
		return 0;
	}

	if( curdsk ){
		CDSKey* firstdsk;
		firstdsk = *( firstds + motid );
		if( firstdsk == curdsk ){
			*( firstds + motid ) = curdsk->next;
		}

		curdsk->LeaveFromChain();
	}

	return curdsk;
}

int CMotionInfo::ChainDSKey( int motid, CDSKey* adddsk )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ChainDSKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (adddsk->frameno < 0) || (adddsk->frameno >= frameleng) ){
		DbgOut( "motinfo : ChainDSKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////////


	int ret;
	CDSKey* dskhead = *(firstds + motid);
	CDSKey* curdsk = dskhead;
	CDSKey* largerdsk = 0;
	CDSKey* smallerdsk = 0;
	int cmp;
	while( curdsk ){
		cmp = adddsk->FramenoCmp( curdsk );
		if( cmp < 0 ){
			largerdsk = curdsk;
			break;
		}else{
			smallerdsk = curdsk;
		}
		curdsk = curdsk->next;
	}

	if( !dskhead ){
		//先頭に追加
		*(firstds + motid) = adddsk;
	}else{
		if( largerdsk ){
			//largerdskの前に追加。
			ret = largerdsk->AddToPrev( adddsk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !adddsk->prev )
				*(firstds + motid) = adddsk;
		}else{
			//最後に追加。(smallerdsk の後に追加)
			_ASSERT( smallerdsk );
			ret = smallerdsk->AddToNext( adddsk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CMotionInfo::DeleteDSKeyOutOfRange( int motid, int srcmaxframe )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : DeleteDSKeyOutOfRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////

	int ret;
	CDSKey* dskptr = *( firstds + motid );
	CDSKey* nextdsk = 0;
	while( dskptr ){
		nextdsk = dskptr->next;
		int frameno = dskptr->frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteDSKey( motid, frameno );
			if( ret ){
				DbgOut( "mi : DeleteDSKeyOutOfRange : DeleteDSKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		dskptr = nextdsk;
	}

	return 0;
}

int CMotionInfo::SetDSAnim( CShdHandler* lpsh )
{
//kindno, motno

	CDSKey* dskptr;
	dskptr = *( firstds + kindno );

	while( dskptr ){
		if( dskptr->frameno <= motno ){
			int dscno;
			for( dscno = 0; dscno < dskptr->changenum; dscno++ ){
				CDSChange* dscptr;
				dscptr = *( dskptr->dschange + dscno );
				dscptr->m_dsptr->state = dscptr->m_change;
			}
		}else{
			break;
		}
		dskptr = dskptr->next;
	}

	if( lpsh ){
		CDispSwitch* dsf = lpsh->m_dsF;
		if( dsf ){
			int dsno;
			for( dsno = 0; dsno < DISPSWITCHNUM; dsno++ ){
				CDispSwitch* curdsf = dsf + dsno;
				if( curdsf->state != 2 ){
					(lpsh->m_ds + dsno)->state = curdsf->state;
				}
			}
		}
	}

	return 0;
}

int CMotionInfo::GetDSAnim( CDispSwitch* dsptr, int motid, int frameno )
{
	CDSKey* dskptr;
	dskptr = *( firstds + motid );

	while( dskptr ){
		if( dskptr->frameno <= frameno ){
			int dscno;
			for( dscno = 0; dscno < dskptr->changenum; dscno++ ){
				CDSChange* dscptr;
				dscptr = *( dskptr->dschange + dscno );

				int switchno;
				switchno = dscptr->m_dsptr->switchno;
				( dsptr + switchno )->state = dscptr->m_change;				
			}
		}else{
			break;
		}
		dskptr = dskptr->next;
	}

	return 0;
}

int CMotionInfo::ExistMMotKey( int motid, int frameno, CMMotKey** ppmmkey )
{
	*ppmmkey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ExistMMotKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : ExistMMotKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CMMotKey* curmmk = *( firstmorph + motid );
	while( curmmk && (curmmk->frameno <= frameno) ){
		if( curmmk->frameno == frameno ){
			*ppmmkey = curmmk;
			break;
		}

		curmmk = curmmk->next;
	}

	return 0;
}
int CMotionInfo::CreateMMotKey( int motid, int frameno, CMMotKey** ppmmkey )
{
	*ppmmkey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : CreateMMotKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : CreateMMotKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CMMotKey* newmmk = 0;
	newmmk = new CMMotKey();
	if( !newmmk ){
		DbgOut( "mi : CreateMMotKey : newmmk alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newmmk->frameno = frameno;


//////////////////
	int ret;
	CMMotKey* mmkhead = *(firstmorph + motid);
	CMMotKey* curmmk = mmkhead;
	CMMotKey* largermmk = 0;
	CMMotKey* smallermmk = 0;
	int cmp;
	while( curmmk ){
		cmp = newmmk->FramenoCmp( curmmk );
		if( cmp < 0 ){
			largermmk = curmmk;
			break;
		}else{
			smallermmk = curmmk;
		}
		curmmk = curmmk->next;
	}

	if( !mmkhead ){
		//先頭に追加
		*(firstmorph + motid) = newmmk;
	}else{
		if( largermmk ){
			//largerdskの前に追加。
			ret = largermmk->AddToPrev( newmmk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newmmk->prev )
				*(firstmorph + motid) = newmmk;
		}else{
			//最後に追加。(smallermmk の後に追加)
			_ASSERT( smallermmk );
			ret = smallermmk->AddToNext( newmmk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppmmkey = newmmk;

	return 0;
}
CMMotKey* CMotionInfo::GetFirstMMotKey( int motid )
{
	if( (motid >= kindnum) || (motid < 0) ){
		//DbgOut( "MotionInfo : GetFirstMMotKey : motid error !!!\n" );
		//_ASSERT( 0 );
		return 0;
	}

	if( !firstmorph ){
		//DbgOut( "mi : GetFirstMMotKey : firstmorph NULL error !!!\n" );
		//_ASSERT( 0 );
		return 0;
	}

	return *( firstmorph + motid );
}
int CMotionInfo::DeleteMMotKey( int motid, int frameno )
{
	CMMotKey* delmmk = 0;
	delmmk = LeaveFromChainMMotKey( motid, frameno );
	if( delmmk ){
		delete delmmk;
	}

	return 0;
}
int CMotionInfo::DeleteMMotKey( int motid, int frameno, CShdElem* srcbase )
{
	CMMotKey* curmmk = 0;
	int ret;
	ret = ExistMMotKey( motid, frameno, &curmmk );
	_ASSERT( !ret );
	if( !curmmk ){
		return 0;
	}

	ret = curmmk->DeleteBase( srcbase );
	if( ret ){
		DbgOut( "mi : DeleteMMotKey : mmk DeleteBase error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curmmk->mmenum <= 0 ){
		//baseが１個も無くなったらキーごと消す。
		DeleteMMotKey( motid, frameno );
	}

	return 0;
}


CMMotKey* CMotionInfo::LeaveFromChainMMotKey( int motid, int frameno )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : LeaveFromChainMMotKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : LeaveFromChainMMotKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
////////////////

	int ret;
	CMMotKey* curmmk = 0;
	ret = ExistMMotKey( motid, frameno, &curmmk );
	if( ret ){
		//DbgOut( "mi : LeaveFromChainMMotKey : ExistMMotKey error !!!\n" );
		//_ASSERT( 0 );
		return 0;
	}

	if( curmmk ){
		CMMotKey* firstmmk;
		firstmmk = *( firstmorph + motid );
		if( firstmmk == curmmk ){
			*( firstmorph + motid ) = curmmk->next;
		}

		curmmk->LeaveFromChain();
	}

	return curmmk;
}
int CMotionInfo::ChainMMotKey( int motid, CMMotKey* addmmk )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ChainMMotKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (addmmk->frameno < 0) || (addmmk->frameno >= frameleng) ){
		DbgOut( "motinfo : ChainMMotKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////////


	int ret;
	CMMotKey* mmkhead = *(firstmorph + motid);
	CMMotKey* curmmk = mmkhead;
	CMMotKey* largermmk = 0;
	CMMotKey* smallermmk = 0;
	int cmp;
	while( curmmk ){
		cmp = addmmk->FramenoCmp( curmmk );
		if( cmp < 0 ){
			largermmk = curmmk;
			break;
		}else{
			smallermmk = curmmk;
		}
		curmmk = curmmk->next;
	}

	if( !mmkhead ){
		//先頭に追加
		*(firstmorph + motid) = addmmk;
	}else{
		if( largermmk ){
			//largermmkの前に追加。
			ret = largermmk->AddToPrev( addmmk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addmmk->prev )
				*(firstmorph + motid) = addmmk;
		}else{
			//最後に追加。(smallerdsk の後に追加)
			_ASSERT( smallermmk );
			ret = smallermmk->AddToNext( addmmk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
int CMotionInfo::DeleteMMotKeyOutOfRange( int motid, int srcmaxframe )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : DeleteMMotKeyOutOfRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////

	int ret;
	CMMotKey* mmkptr = *( firstmorph + motid );
	CMMotKey* nextmmk = 0;
	while( mmkptr ){
		nextmmk = mmkptr->next;
		int frameno = mmkptr->frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteMMotKey( motid, frameno );
			if( ret ){
				DbgOut( "mi : DeleteMMotKeyOutOfRange : DeleteMMotKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		mmkptr = nextmmk;
	}

	return 0;
}
int CMotionInfo::SetMMotAnim( CMotionCtrl* mcptr )
{
	int ret;
	int mmeno;
	for( mmeno = 0; mmeno < mcptr->curmmenum; mmeno++ ){
		CMMotElem* curmme;
		curmme = *( mcptr->curmmotelem + mmeno );

		CShdElem* curbase;
		CShdElem* curtarget;
		curbase = curmme->m_baseelem;

		int tno;
		for( tno = 0; tno < curmme->m_primnum; tno++ ){
			MPRIM* curmp;
			curmp = curmme->m_primarray + tno;
			curtarget = curmp->selem;

			int existflag = 0;
			ret = GetMMotAnim( &curmp->value, kindno, motno, curbase, curtarget, &existflag );
			if( ret ){
				DbgOut( "mi : SetMMotAnim : GetMMotAnim error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}
int CMotionInfo::GetMMotAnim( float* valptr, int srcmotid, int srcframeno, CShdElem* srcbase, CShdElem* srctarget, int* existflag )
{
	if( (srcmotid >= kindnum) || (srcmotid < 0) ){
		DbgOut( "MotionInfo : GetMMotAnim : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + srcmotid);
	if( (srcframeno < 0) || (srcframeno >= frameleng) ){
		DbgOut( "motinfo : GetMMotAnim : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////////



	int ret;

	if( !firstmorph ){
		*valptr = 0.0f;
		return 0;
	}

	CMMotKey* mmkhead = *(firstmorph + srcmotid);
	if( !mmkhead ){
		*valptr = 0.0f;
		return 0;
	}

	CMMotKey* chkmmk = mmkhead;
	CMMotKey* findmmk = 0;
	CMMotKey* befmmk = 0;
	while( chkmmk ){
		CMMotElem* chkmme;
		chkmme = chkmmk->ExistBase( srcbase );
		if( chkmme ){
			MPRIM* chkmp;
			chkmp = chkmme->GetPrim( srctarget );
			if( chkmp ){
				if( chkmmk->frameno == srcframeno ){
					findmmk = chkmmk;
				}else if( chkmmk->frameno > srcframeno ){
					break;
				}
				befmmk = chkmmk;

			}
		}
		chkmmk = chkmmk->next;
	}

	if( findmmk ){
		*valptr = findmmk->GetValue( srcbase, srctarget, existflag ); 
	}else{
		*existflag = 0;//!!!!

		if( befmmk ){
			CMMotKey* nextmmk;
			nextmmk = befmmk->next;
			while( nextmmk ){
				CMMotElem* nextmme;
				nextmme = nextmmk->ExistBase( srcbase );
				if( nextmme ){
					MPRIM* nextmp;
					nextmp = nextmme->GetPrim( srctarget );
					if( nextmp ){
						break;
					}
				}
				nextmmk = nextmmk->next;
			}

			//befmmk, befmmk->next間の補間
			ret = FillUpMMotOnFrame( befmmk, nextmmk, valptr, srcframeno, srcbase, srctarget );
			if( ret ){
				DbgOut( "mi : GetMMotAnim : FillUpMMotOnFrame error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			//init
			//_ASSERT( 0 );
			*valptr = 0.0f;
		}
	}

	return 0;
}

int CMotionInfo::FillUpMMotOnFrame( CMMotKey* srcstartmmk, CMMotKey* srcendmmk, float* valptr, int srcframe, 
	CShdElem* srcbase, CShdElem* srctarget )
{
	_ASSERT( srcstartmmk );

	MPRIM* startmp;
	float startval;
	int startframe;
	startmp = srcstartmmk->ExistTarget( srcbase, srctarget );
	if( startmp ){
		startval = startmp->value;
		startframe = srcstartmmk->frameno;
	}else{
		_ASSERT( 0 );
		*valptr = 0.0f;
		return 1;
	}

	if( !srcendmmk ){
		*valptr = startval;
		return 0;
	}

	MPRIM* endmp;
	float endval;
	int endframe;
	endmp = srcendmmk->ExistTarget( srcbase, srctarget );
	if( endmp ){
		endval = endmp->value;
		endframe = srcendmmk->frameno;
	}else{
		_ASSERT( 0 );
		*valptr = startval;
		return 1;
	}

	float t;
	t = (float)( srcframe - startframe ) / (float)( endframe - startframe );

	*valptr = startval + ( endval - startval ) * t;

	return 0;
}

int CMotionInfo::SetMMotValue( int motid, int frameno, CShdElem* srcbase, CShdElem* srctarget, float srcval )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : SetMMotValue : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : SetMMotValue : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////////
	int ret;
	CMMotKey* setmmk = 0;
	ret = ExistMMotKey( motid, frameno, &setmmk );
	_ASSERT( !ret );
	if( !setmmk ){
		CMMotKey* newmmk = 0;
		ret = CreateMMotKey( motid, frameno, &newmmk );
		if( ret || !newmmk ){
			DbgOut( "mi : SetMMotValue : CreateMMotKey error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		setmmk = newmmk;
	}

	ret = setmmk->SetValue( srcbase, srctarget, srcval );
	if( ret ){
		DbgOut( "mi : SetMMotValue : mmk SetValue error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CMotionInfo::GetMMotAnimKeyframeNoRange( int motid, int startframe, int endframe, int* keyframeptr, int arrayleng, int* getnum )
{
	*getnum = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : GetMMotAnimKeyframeNoRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMMotKey* mmkptr;
	mmkptr = GetFirstMMotKey( motid );

	int setno = 0;
	while( mmkptr ){
		if( (mmkptr->frameno >= startframe) && (mmkptr->frameno <= endframe) ){
			if( keyframeptr ){
				if( setno >= arrayleng ){
					DbgOut( "mi : GetMMotAnimKeyframeNoRange : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				*( keyframeptr + setno ) = mmkptr->frameno;
			}
			setno++;
		}
		mmkptr = mmkptr->next;
	}

	*getnum = setno;

	return 0;
}

int CMotionInfo::GetMMotAnimKeyAll( int motid, CShdElem* baseelem, CShdElem* targetelem,
		E3DMORPHMOTION* motionptr, int arrayleng, int* getnum )
{
	*getnum = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : GetMMotAnimKeyAll : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMMotKey* mmkptr;
	mmkptr = GetFirstMMotKey( motid );


	int setno = 0;
	while( mmkptr ){
		MPRIM* curmprim;
		curmprim = mmkptr->ExistTarget( baseelem, targetelem );
		if( curmprim ){
			if( motionptr ){
				if( setno >= arrayleng ){
					DbgOut( "mi : GetMMotAnimKeyAll : arrayleng too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				( motionptr + setno )->frameno = mmkptr->frameno;
				( motionptr + setno )->blendrate = curmprim->value;
			}
			setno++;
		}
		mmkptr = mmkptr->next;
	}

	*getnum = setno;

	return 0;
}

int CMotionInfo::ExistAlpKey( int motid, int frameno, CAlpKey** ppalpkey )
{
	*ppalpkey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ExistAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : ExistAlpKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

//////////
	CAlpKey* curalpk = *( firstalp + motid );
	while( curalpk && (curalpk->frameno <= frameno) ){
		if( curalpk->frameno == frameno ){
			*ppalpkey = curalpk;
			break;
		}

		curalpk = curalpk->next;
	}

	return 0;
}

int CMotionInfo::CreateAlpKey( int motid, int frameno, CAlpKey** ppalpkey )
{
	*ppalpkey = 0;

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : CreateAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : CreateAlpKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
///////////////////
	CAlpKey* newalpk = 0;
	newalpk = new CAlpKey();
	if( !newalpk ){
		DbgOut( "mi : CreateAlpKey : newalpk alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	newalpk->frameno = frameno;


//////////////////
	int ret;
	CAlpKey* alpkhead = *(firstalp + motid);
	CAlpKey* curalpk = alpkhead;
	CAlpKey* largeralpk = 0;
	CAlpKey* smalleralpk = 0;
	int cmp;
	while( curalpk ){
		cmp = newalpk->FramenoCmp( curalpk );
		if( cmp < 0 ){
			largeralpk = curalpk;
			break;
		}else{
			smalleralpk = curalpk;
		}
		curalpk = curalpk->next;
	}

	if( !alpkhead ){
		//先頭に追加
		*(firstalp + motid) = newalpk;
	}else{
		if( largeralpk ){
			//largeralpkの前に追加。
			ret = largeralpk->AddToPrev( newalpk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
			if( !newalpk->prev )
				*(firstalp + motid) = newalpk;
		}else{
			//最後に追加。(smalleralpk の後に追加)
			_ASSERT( smalleralpk );
			ret = smalleralpk->AddToNext( newalpk );
			if( ret ){
				_ASSERT( 0 );
				return 0;
			}
		}
	}

	*ppalpkey = newalpk;

	return 0;
}

CAlpKey* CMotionInfo::GetFirstAlpKey( int motid )
{

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : GetFirstAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	if( !firstalp ){
		DbgOut( "mi : GetFirstAlpKey : firstalp NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	return *( firstalp + motid );
}

int CMotionInfo::DeleteAlpKey( int motid, int frameno )
{
	CAlpKey* delalpk = 0;
	delalpk = LeaveFromChainAlpKey( motid, frameno );
	if( delalpk ){
		delete delalpk;
	}

	return 0;
}

CAlpKey* CMotionInfo::LeaveFromChainAlpKey( int motid, int frameno )
{

	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : LeaveFromChainAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (frameno < 0) || (frameno >= frameleng) ){
		DbgOut( "motinfo : LeaveFromChainAlpKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
////////////////

	int ret;
	CAlpKey* curalpk = 0;
	ret = ExistAlpKey( motid, frameno, &curalpk );
	if( ret ){
//		DbgOut( "mi : LeaveFromChainAlpKey : ExistAlpKey error !!!\n" );
//		_ASSERT( 0 );
		return 0;
	}

	if( curalpk ){
		CAlpKey* firstalpk;
		firstalpk = *( firstalp + motid );
		if( firstalpk == curalpk ){
			*( firstalp + motid ) = curalpk->next;
		}

		curalpk->LeaveFromChain();
	}

	return curalpk;
}

int CMotionInfo::ChainAlpKey( int motid, CAlpKey* addalpk )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : ChainAlpKey : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int frameleng;
	frameleng = *(motnum + motid);
	if( (addalpk->frameno < 0) || (addalpk->frameno >= frameleng) ){
		DbgOut( "motinfo : ChainAlpKey : frameno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////////


	int ret;
	CAlpKey* alpkhead = *(firstalp + motid);
	CAlpKey* curalpk = alpkhead;
	CAlpKey* largeralpk = 0;
	CAlpKey* smalleralpk = 0;
	int cmp;
	while( curalpk ){
		cmp = addalpk->FramenoCmp( curalpk );
		if( cmp < 0 ){
			largeralpk = curalpk;
			break;
		}else{
			smalleralpk = curalpk;
		}
		curalpk = curalpk->next;
	}

	if( !alpkhead ){
		//先頭に追加
		*(firstalp + motid) = addalpk;
	}else{
		if( largeralpk ){
			//largeralpkの前に追加。
			ret = largeralpk->AddToPrev( addalpk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			if( !addalpk->prev )
				*(firstalp + motid) = addalpk;
		}else{
			//最後に追加。(smalleralpk の後に追加)
			_ASSERT( smalleralpk );
			ret = smalleralpk->AddToNext( addalpk );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CMotionInfo::DeleteAlpKeyOutOfRange( int motid, int srcmaxframe )
{
	if( (motid >= kindnum) || (motid < 0) ){
		DbgOut( "MotionInfo : DeleteAlpKeyOutOfRange : motid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
////////////

	int ret;
	CAlpKey* alpkptr = *( firstalp + motid );
	CAlpKey* nextalpk = 0;
	while( alpkptr ){
		nextalpk = alpkptr->next;
		int frameno = alpkptr->frameno;
		if( frameno > srcmaxframe ){
			ret = DeleteAlpKey( motid, frameno );
			if( ret ){
				DbgOut( "mi : DeleteAlpKeyOutOfRange : DeleteAlpKey error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		alpkptr = nextalpk;
	}

	return 0;
}

int CMotionInfo::SetAlpAnim( CMQOMaterial* srcmathead )
{
	int ret;
	CMQOMaterial* curmat = srcmathead;
	while( curmat ){
		float curalpha = 1.0f;
		int existflag = 0;
		ret = GetAlpAnim( kindno, motno, curmat, &curalpha, &existflag );
		if( ret == 1){
			DbgOut( "mi : SetAlpAnim : GetAlpAnim error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}else if( ret == 0 ){
			curmat->col.a = curalpha;
		}//ret 2のときは何もしない

		curmat = curmat->next;
	}
	return 0;
}

int CMotionInfo::GetAlpAnim( int motid, int frameno, CMQOMaterial* srcmat, float* alphaptr, int* existflag )
{
	int ret;

//	*alphaptr = srcmat->orgalpha;
	*existflag = 0;

	if( !firstalp ){
		return 2;
	}

	CAlpKey* alpkptr;
	alpkptr = *( firstalp + motid );
	if( !alpkptr ){
//		*alphaptr = srcmat->orgalpha;
		return 2;
	}


//	CAlpKey* befkey = 0;
//	CAlpChange* befchange = 0;
	CAlpKey* curkey = 0;
	CAlpChange* curchange = 0;
	CAlpKey* nextkey = 0;
	CAlpChange* nextchange = 0;

	while( alpkptr ){
		if( alpkptr->frameno <= frameno ){
			int alpno;
			for( alpno = 0; alpno < alpkptr->changenum; alpno++ ){
				CAlpChange* alpptr;
				alpptr = *( alpkptr->alpchange + alpno );
				if( alpptr->mqomat == srcmat ){
//					befkey = curkey;
//					befchange = curchange;

					curkey = alpkptr;
					curchange = alpptr;
				}
			}
		}else{
			if( !nextkey ){
				int alpno;
				for( alpno = 0; alpno < alpkptr->changenum; alpno++ ){
					CAlpChange* alpptr;
					alpptr = *( alpkptr->alpchange + alpno );
					if( alpptr->mqomat == srcmat ){
						nextkey = alpkptr;
						nextchange = alpptr;
						break;
					}
				}
			}else{
				break;
			}
		}
		alpkptr = alpkptr->next;
	}

	if( curkey && curchange && (curkey->frameno == frameno) ){
		*alphaptr = curchange->change;
		*existflag = 1;//!!!!!!!!!!!!
	}else{
		if( curkey ){
			if( nextkey ){
				ret = FillUpAlpha( curkey, curchange, nextkey, nextchange, frameno, alphaptr );
				if( ret ){
					DbgOut( "mi : GetAlphaAnim : FillUpAlpha error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}else{
				*alphaptr = curchange->change;
			}
		}else{
			*alphaptr = srcmat->orgalpha;
		}
	}

	return 0;

}

int CMotionInfo::FillUpAlpha( CAlpKey* befkey, CAlpChange* befchange, CAlpKey* nextkey, CAlpChange* nextchange, 
	int frameno, float* alphaptr )
{
	//bef, nextともNULLでないことが保障されて呼び出される。
	if( !befkey || !befchange || !nextkey || !nextchange ){
		_ASSERT( 0 );
		return 1;
	}


	if( befkey->frameno == frameno ){
		*alphaptr = befchange->change;
	}else if( nextkey->frameno == frameno ){
		*alphaptr = nextchange->change;
	}else{
		int framenum = nextkey->frameno - befkey->frameno;
		int framecnt = frameno - befkey->frameno;

		float startalp;
		float stepalp;
		startalp = befchange->change;
		if( framenum != 0 ){
			stepalp = (nextchange->change - startalp) / (float)framenum;
		}else{
			stepalp = 0.0f;
		}
		
		*alphaptr = startalp + stepalp * (float)framecnt;
	}

	return 0;
}

int CMotionInfo::ScaleSplineMv( int motid, D3DXVECTOR3 srcmult )
{
	if( (motid >= kindnum) || (motid < 0) ){
		_ASSERT( 0 );
		return 0;
	}

	if( !firstmp )
		return 0;//!!!!!!!!!!!!!!!!
	
	int ret;
	CMotionPoint2* curmp = *( firstmp + motid );
	while( curmp ){
		CMotionPoint2* nextmp = curmp->next;

		if( curmp && nextmp ){
			ret = curmp->ScaleSplineMv( nextmp, srcmult );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}

		curmp = nextmp;
	}

	return 0;
}
