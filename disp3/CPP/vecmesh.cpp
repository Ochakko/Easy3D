#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <vecmesh.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

// CVecMesh, CBezData, CBezLine


//////////////////////////
// CVecMesh
//////////////////////////

CVecMesh::CVecMesh()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CVecMesh::CVecMesh : CreateObj error !!!\n" );
		isinit = 0;
	}
}

CVecMesh::~CVecMesh()
{
	DestroyObjs();
}


void	CVecMesh::InitParams()
{
	CBaseDat::InitParams();

	meshinfo = 0;
	hline = 0;	
	allocleng = 0;
}

int CVecMesh::InitVecMesh( CVecMesh* srcmesh )
{
	int i, ret, setleng;
	CMeshInfo* tempinfo = 0;

	if( !srcmesh )
		return 1;

	setleng = srcmesh->allocleng;

	if( setleng ){
		tempinfo = new CMeshInfo[setleng];
		if( !tempinfo ){
			DbgOut( "charpos2.cpp : CVecMesh : InitVecMesh : tempinfo alloc error !!!\n" );
			return 1;
		}

		for( i = 0; i < setleng; i++ ){
			ret = (tempinfo + i)->CopyData( (*srcmesh)( i )->meshinfo );
			if( ret ){
				DbgOut( "charpos2.cpp : CVecMesh : InitVecMesh : tempinfo->CopyData error !!!\n" );
				delete [] tempinfo;
				return 1;
			}
		}
	}

	ret = InitVecMesh( srcmesh->meshinfo, tempinfo, setleng );
	delete [] tempinfo;
	return ret;
}


int	CVecMesh::InitVecMesh( CMeshInfo* srcmeshinfo, CMeshInfo* lineinfo, int setleng )
{
	// srcmeshinfo は　一個、lineinfo は　srcmeshinfo->total 個
	int ret;


#ifdef _DEBUG
	int checkleng;
	ret = CheckMeshInfo( srcmeshinfo );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecMesh : InitVecMesh : CheckMeshInfo error !!!\n" );
		return 1;
	}

	if( setleng >= 0 ){
		checkleng = setleng;
	}else if( srcmeshinfo->type == SHDBEZLINE ){
		checkleng = 1;
	}else{
		checkleng = srcmeshinfo->total;
	}

	
	if( lineinfo && checkleng ){
		ret = CheckLineInfo( lineinfo, checkleng );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecMesh : InitVecMesh : CheckLineInfo error !!!\n" );
			return 1;
		}
	}
#endif

	DestroyVecMesh();
	
	ret = CreateVecMesh( srcmeshinfo, lineinfo, setleng );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecMesh : InitVecMesh : CreateVecMesh error !!!\n" );
		return 1;
	}

	DbgOut( "charpos2.cpp : CVecMesh : InitVecMesh : CreateVecMesh\n" );

	return 0;	
}

int CVecMesh::SetMem( int* srcint, __int64 setflag )
{
	int ret;
	if( setflag & BASE_TYPE ){
		ret = CBaseDat::SetMem( srcint, BASE_TYPE );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecMesh : SetMem : BASE_TYPE error !!!\n" );
			return 1;
		}
		return 0;
	}else if( setflag & MESHI_SKIP ){
		ret = meshinfo->SetMem( srcint, MESHI_SKIP );
		if( ret ){
			DbgOut( "CVecMesh : SetMem : meshinfo->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}
int CVecMesh::SetMem( int srcint, int lineno, __int64 setflag )
{
	int ret;
	if( setflag & MESHI_SKIP ){
		ret = (*this)( lineno )->meshinfo->SetMem( srcint, setflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecMesh : SetMem : MESHI_SKIP error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}

int CVecMesh::SetMem( CVecMesh* srcmesh, __int64 setflag )
{
	int ret, setleng, i;
	CVecLine* srcline = 0;

	// setflag は　VEC_MESH
	// srcdata は　CVecMesh*
	if( setflag == VEC_MESH ){
		ret = CheckMeshInfo( srcmesh->meshinfo );
		if( ret || !meshinfo ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVecMesh : meshinfo NULL !!!\n" );
			return 1;
		}

		ret = meshinfo->IsSameData( srcmesh->meshinfo, chkmeshi, 5 );
		if( !ret ){
			DbgOut( "charpos2.cpp : CVecMesh : SetMem : type CVecMesh : meshinfo->IsSameData error !!!\n" );
			return 1;
		}

		setleng = srcmesh->allocleng;
		if( allocleng < setleng ){
			DbgOut( "charpos2.cpp : CVecMesh : SetMem : type CVecMesh : allocleng error %d %d !!!\n",
				allocleng, setleng );
			return 1;
		}

		for( i = 0; i < setleng; i++ ){
			srcline = (*srcmesh)(i);
			ret = CheckLineInfo( srcline->meshinfo, 1 );
			if( ret ){
				DbgOut( "charpos2.cpp : CVecMesh : SetMem : type CVecMesh : line->meshinfo error !!!\n" );
				return 1;
			}

			ret = (*this)( i )->SetMem( srcline, VEC_LINE );
			if( ret ){
				DbgOut( "charpos2.cpp : CVecMesh : SetMem : type CVecMesh : line->SetMem error !!!\n" );
				return 1;
			}
		}
		return 0;
	}
	return 1;
}

int CVecMesh::SetMem( CVecLine* srcline, int lineno, __int64 setflag )
{
	int ret;
	// setflag は　VEC_LINE

	if( setflag == VEC_LINE ){
		ret = CheckLineInfo( srcline->meshinfo, 1 );
		if( ret || !meshinfo ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVecLine : meshinfo NULL !!!\n" );
			return 1;
		}

		ret = meshinfo->IsSameData( srcline->meshinfo, MESHI_M );
		if( !ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVecLine : meshinfo->IsSameData error !!!\n" );
			return 1;
		}

		ret = (*this)( lineno )->SetMem( srcline, VEC_LINE );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine: SetMem : type CVecLine : srcbline->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}

int CVecMesh::SetMem( CVecLine* srcline, int linevno, int linekind, __int64 setflag )
{
	int ret;
	// setflag は　VEC_LINE

	if( setflag == VEC_LINE ){
		ret = CheckLineInfo( srcline->meshinfo, 1 );
		if( ret || !meshinfo ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVecLine : meshinfo NULL !!!\n" );
			return 1;
		}

		ret = meshinfo->IsSameData( srcline->meshinfo, MESHI_M );
		if( !ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVecLine : meshinfo->IsSameData error !!!\n" );
			return 1;
		}

		ret = (*this)( linevno, linekind )->SetMem( srcline, VEC_LINE );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine: SetMem : type CVecLine : srcbline->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;

}

int CVecMesh::SetMem( CVec3f* srcmat, __int64 setflag )
{
	int ret;
	__int64 maskflag;

	if( setflag & MESHI_MATERIAL ){
	// meshinfo->m, n, total は　meshlineにも影響するので、単体ではセットさせない。
	// material は　可

	// srcdata は　CVec3f*, setflag は　MESHI_MATERIAL | VEC_*

		if( !meshinfo ){
			DbgOut( "charpos2.cpp : CVecMesh : SetMem : material : meshinfo NULL !!!\n" );
			return 1;
		}
		maskflag = MESHI_MATERIAL | VEC_ALL;
		setflag &= maskflag; // material 以外はセットさせない

		ret = meshinfo->SetMem( srcmat, setflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecMesh : SetMem : material : meshinfo->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}

int CVecMesh::ConvDataNo( int* linenoptr, int* vecno, int datano )
{
	int kindnum, vnum, pointnum, lineno;
	CMeshInfo* lineinfo = 0;

	if( !meshinfo ){
		return 1;
	}

	lineinfo = (*this)( 0 )->meshinfo; // 一定の場合
	if( !lineinfo )
		return 1;
	pointnum = lineinfo->m;


	vnum = meshinfo->n;
	kindnum = meshinfo->m;
	lineno = datano / pointnum; // 全データ数 / 1 line分のデータ数

	//lineno = datano / (kindnum * pointnum); // 全データ数 / kindnum line分のデータ数

	*linenoptr = lineno;
	*vecno = datano - lineno * pointnum; // 総数　- linevno * １line分のデータ数

	return 0;
}

/***
int CVecMesh::SetMem( CVec3f* srcvec, int datano, __int64 setflag )
{
	int ret, lineno, vecno;

	ret = ConvDataNo( &lineno, &vecno, datano );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecMesh : SetMem : CVec3f : ConvDataNo error !!!\n" );
		return 1;
	}

	ret = SetMem( srcvec, lineno, vecno, setflag );
	if( ret ){
		return 1;
	}
	return 0;
}
***/

int CVecMesh::SetMem( CVec3f* srcvec, int lineno, int vecno, __int64 setflag )
{
	int ret;
	__int64 tempflag;

	// setflag は　VEC_*

	if( setflag & VEC_ALL ){
		if( !meshinfo ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVec3f,float : meshinfo NULL !!!\n" );
			return 1;
		}
		
		tempflag = setflag & VEC_ALL; // VEC_* のみ抽出
		ret = (*this)( lineno )->SetMem( srcvec, vecno, tempflag );
		//ret = (line + vno)->SetMem( srcvec, uno, tempflag ); // uno を渡す
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type vec : line->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}

	return 1;
}
int CVecMesh::SetMem( CVec3f* srcvec, int linevno, int linekind, int vecno, __int64 setflag )
{
	int ret;
	__int64 tempflag;

	// setflag は　VEC_*

	if( setflag & VEC_ALL ){
		if( !meshinfo ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVec3f,float : meshinfo NULL !!!\n" );
			return 1;
		}
		
		tempflag = setflag & VEC_ALL; // VEC_* のみ抽出
		ret = (*this)( linevno, linekind )->SetMem( srcvec, vecno, tempflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type vec : line->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}

int CVecMesh::SetMem( VEC3F* srcvec, int lineno, int vnum )
{
	int ret;
	ret = (*this)( lineno )->SetMem( srcvec, vnum );
	_ASSERT( !ret );
	return ret;
}
int CVecMesh::SetMem( VEC3F* srcvec, int linevno, int linekind, int vnum )
{
	int ret;
	ret = (*this)( linevno, linekind )->SetMem( srcvec, vnum );
	_ASSERT( !ret );
	return ret;
}

int CVecMesh::SetMem( float* srcfloat, int lineno, int vecno, __int64 setflag )
{
	int ret;
	__int64 tempflag;


	// setflag は　VEC_*

	if( setflag & VEC_ALL ){
		if( !meshinfo ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type float : meshinfo NULL !!!\n" );
			return 1;
		}
		
		tempflag = setflag & VEC_ALL; // VEC_* のみ抽出
		ret = (*this)( lineno )->SetMem( srcfloat, vecno, tempflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type float : line->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}

	return 1;

}
int CVecMesh::SetMem( float* srcfloat, int linevno, int linekind, int vecno, __int64 setflag )
{
	int ret;
	__int64 tempflag;


	// setflag は　VEC_*

	if( setflag & VEC_ALL ){
		if( !meshinfo ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type float : meshinfo NULL !!!\n" );
			return 1;
		}
		
		tempflag = setflag & VEC_ALL; // VEC_* のみ抽出
		ret = (*this)( linevno, linekind )->SetMem( srcfloat, vecno, tempflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type float : line->SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}

	return 1;
}

int	CVecMesh::CopyData( CVecMesh* srcdata )
{
	int ret1;
	int setleng;
	int i;

	if( !srcdata ){
		DbgOut( "charpos2.cpp : CVecMesh : CopyData : srcdata NULL !!!\n" );
		return 1;
	}

	ret1 = CheckMeshInfo( srcdata->meshinfo );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CVecMesh : CopyData : srcdata->meshinfo invalid !!!\n" );
		return 1;
	}


	ret1 = CBaseDat::CopyData( (CBaseDat*)srcdata );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CVecMesh : CopyData : basedat->CopyData error !!!\n" );
		return 1;
	}

	ret1 = InitVecMesh( srcdata );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CVecMesh : CopyData1 : InitMesh error !!!\n" );
		return 1;
	}

	setleng = srcdata->allocleng;
	if( allocleng != setleng ){
		DbgOut( "charpos2.cpp : CVecMesh : CopyData : allocleng %d %d error !!!\n", allocleng, setleng );
		return 1;
	}

	for( i = 0; i < setleng; i++ ){
		ret1 = CheckLineInfo( (*srcdata)( i )->meshinfo, 1 );
		if( ret1 ){
			DbgOut( "charpos2.cpp : CVecMesh : CopyData : srcdata->lineptr->meshinfo invalid !!!\n" );
			return 1;
		}

		ret1 = (*this)( i )->CopyData( (*srcdata)( i ) );
		if( ret1 ){
			DbgOut( "charpos2.cpp : CVecMesh : CopyData : AddLine, line->CopyData error !!!\n" );
			return 1;
		}
	}

	chkflag = srcdata->chkflag;

	return 0;
}


int	CVecMesh::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	int ret;
	int i;
	int totalnum;


	ret = CBaseDat::DumpMem( hfile, tabnum, "3DOBJ" );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecMesh : DumpMem : basedat::DumpMem error !!!\n" );
		return 1;
	}


	tabnum++;
	ret = meshinfo->DumpMem( hfile, tabnum, dumpflag );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecMesh : DumpMem : meshinfo->DumpMem error !!!\n" );
		return 1;
	}

	totalnum = meshinfo->total;
	if( totalnum != allocleng ){
		SetTabChar( tabnum );			

		ret = Write2File( hfile, "%s allocleng %d\n", tabchar, allocleng );
		_ASSERT( !ret );
	}

	for( i = 0; i < allocleng; i++ ){
		ret = (*this)( i )->DumpMem( hfile, tabnum, dumpflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecMesh : DumpMem : line->DumpMem error !!!\n" );
			return 1;
		}
	}
	return 0;
}


void	CVecMesh::ResetParams()
{
	DestroyObjs();
	InitParams();
}

void	CVecMesh::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	DestroyVecMesh();

}

int	CVecMesh::CreateObjs()
{
	return 0;
}
int	CVecMesh::CreateVecMesh( CMeshInfo* srcmeshinfo, CMeshInfo* srclineinfo, int setleng )
{
	// meshinfo のチェックをしてから呼ぶ

	int ret1, addleng;


	// meshinfo
	ret1 = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CVecMesh : CreateVecMesh ; srcmeshinfo->NewMeshinfo error !!!\n" );
		return 1;
	}

	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );
	//if( (meshinfo->type <= SHDTYPENONE) || (meshinfo->type >= SHDTYPEMAX) ){
	//	DbgOut( "charpos2.cpp : CVecMesh : CreateVecMesh : meshinfo->type error !!!\n" );
	//	return 1;
	//}
	ret1 = CBaseDat::SetType( meshinfo->type );
	if( ret1 ){
		DbgOut( "charpos2.cpp : CVecMesh : CreateVecMesh : base::SetType error !!!\n" );
		return 1;
	}


	// Line data の　作成
	if( srclineinfo ){
		if( setleng >= 0 )
			addleng = setleng;
		else
			addleng = meshinfo->total;

		if( addleng ){
			ret1 = AddVecLine( srclineinfo, addleng );
			_ASSERT( !ret1 );
			if( ret1 ){
				DbgOut( "charpos2.cpp : CVecMesh : CreateVecMesh : AddVecLine error !!!\n" );
				return 1;
			}
		}
	}

	return 0;

}

int CVecMesh::AddVecLine( CMeshInfo* srclineinfo, int linenum )
{
	// この関数はpublicだが、lineinfoのチェックをしない。

	// linenum が　１以上でも、SHDBEZLINEのときは、srclineinfo は一個のみ。

	int totalnum, edgenum, i, ret1;
	CVecLine* newline = 0;
	CMeshInfo* curmi = 0;

	totalnum = meshinfo->total;
	edgenum = allocleng + linenum;

	if( meshinfo->type != SHDPOLYGON ){
		if( edgenum > totalnum ){
			DbgOut( "chharpos2.cpp : CVecMesh : AddVecLine : allocleng error !!!\n" );
			return 1;
		}
	}

	_ASSERT( (srclineinfo->type > SHDTYPENONE) && (srclineinfo->type < SHDTYPEMAX) );

	hline = (unsigned long**)realloc( hline, sizeof(unsigned long*) * edgenum );
	if( !hline ){
		DbgOut( "chharpos2.cpp : CVecMesh : AddVecLine : hline realloc error !!!\n" );
		return 1;
	}

	curmi = srclineinfo;
	for( i = allocleng; i < edgenum; i++ ){
		if( type != SHDBEZLINE )
			curmi = srclineinfo + i;

		newline = new CVecLine();
		if( !newline ){
			DbgOut( "charpos2.cpp : CVecMesh : AddVecLine ; linedata alloc error !!!\n" );
			return 1;
		}

		*(hline + i) = (unsigned long*)newline;
		allocleng++;

		ret1 = (*this)( i )->InitVecLine( curmi );
		if( ret1 ){
			DbgOut( "charpos2.cpp : CVecMesh : AddVecLine ; newline InitLine error !!!\n" );
			return 1;
		}
	}
	DbgOut( "charpos2.cpp : CVecMesh : AddVecLine : init newline : newline->InitLine\n" );


	//!!!!!!!!
	if( meshinfo->type == SHDPOLYGON ){
		meshinfo->n = allocleng;
		meshinfo->total = meshinfo->m * meshinfo->n;
	}

	return 0;

}


int	CVecMesh::DestroyVecMesh()
{
	int i;

	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}


	for( i = 0; i < allocleng; i++ ){
		delete (CVecLine*)( *(hline + i) );
	}
	if( hline ){
		//hline = (unsigned long**)realloc( hline, 0 );
		free( hline );
		hline = 0;
	}
	allocleng = 0;


	return 0;
}

int CVecMesh::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	int mnum, nnum, totalnum;

	if( !srcmeshinfo )
		return 1;

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( totalnum && ( totalnum == (mnum * nnum) ) )
		return 0;
	else
		return 1;
}

int CVecMesh::CheckLineInfo( CMeshInfo* lineinfo, int infonum )
{
	int i, mnum, nnum, totalnum, mnum0;

	_ASSERT( lineinfo );
	mnum0 = lineinfo->m;

	for( i = 0; i < infonum; i++ ){
		_ASSERT( lineinfo + i );

		mnum = (lineinfo + i)->m;
		nnum = (lineinfo + i)->n;
		totalnum = (lineinfo + i)->total;

		if( (mnum == mnum0) && (nnum == 1) && (totalnum == mnum) )
			continue;
		else
			return 1;
	}
	return 0;
}

int CVecMesh::Transform( CMatrix2& transmat, CVecMesh* srcvmesh )
{
	// すべてのlineを計算
	int ret = 0;
	int srclnum, i;
	CVecLine* srcline = 0;
	CVecLine* dstline = 0;

	srclnum = srcvmesh->allocleng;

	if( srclnum != allocleng )
		return 1;

	for( i = 0; i < allocleng; i++ ){
		dstline = (*this)( i );
		srcline = (*srcvmesh)( i );
		ret += dstline->Transform( transmat, srcline );
		_ASSERT( !ret );
	}

	return ret;
}


////////////////////////
// CBezData
////////////////////////

CBezData::CBezData()
{
	int	ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		isinit = 0;
		DbgOut( "charpos2.cpp : CBezData::CBezData : CreateObjs error !!!\n" );
	}
}

CBezData::~CBezData()
{
	DestroyObjs();
}

int CBezData::InitVecMesh( CBezData* srcdata )
{
	int ret;

	ret = CVecMesh::InitVecMesh( (CVecMesh*)srcdata );

	return ret;
}

int CBezData::CopyData( CBezData* srcdata )
{
	int ret;
	ret = CVecMesh::CopyData( (CVecMesh*)srcdata );


	chkflag = srcdata->chkflag;

	return ret;
}

int CBezData::SetMem( CBezData* srcbez, __int64 setflag )
{
	int	ret = 0;
	int	i;

	if( setflag == BEZPOINT_ALL ){
		for( i = 0; i < LIM_MAX; i++ ){
			ret = CheckLineInfo( (*srcbez)( i )->meshinfo, 1 );
			if( ret ){
				DbgOut( "charpos2.cpp : CBezData : SetMem : CheckLineInfo error !!!\n" );
				return 1;
			}
			ret = (*this)( i )->SetMem( (*srcbez)( i ), VEC_LINE );
			if( ret ){
				DbgOut( "charpos2.cpp : CBezData : SetMem : line->SetMem error !!!\n" );
				return 1;
			}
			chkflag |= chkbez[i];
		}
		return 0;
	}
	return 1;
}

int CBezData::SetMem( CVec3f* srcvec, int srckind, __int64 setflag )
{
	int ret = 0;

	if( setflag == SHDBEZPOINT ){
		ret = (*this)( srckind )->SetMem( srcvec, 0, VEC_ALL );
		_ASSERT( !ret );
		return ret;
	}else{
		return 1;
	}
}

int CBezData::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	//meshinfo->m = 5; meshinfo->n = 1;
	int mnum, nnum, totalnum;

	if( !srcmeshinfo )
		return 1;

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( (mnum == LIM_MAX) && (nnum == 1) && (totalnum == mnum * nnum) )
		return 0;
	else
		return 1;
}

int CBezData::CheckLineInfo( CMeshInfo* lineinfo, int infonum )
{
	// BezData中のLine は　一点。
	//eachline->meshinfo->m = 1; eachline->meshinfo->n = 1;
	int mnum, nnum, totalnum, i;

	for( i = 0; i < infonum; i++ ){
		_ASSERT( lineinfo + i );
		mnum = (lineinfo + i)->m;
		nnum = (lineinfo + i)->n;
		totalnum = (lineinfo + i)->total;
		if( (mnum == 1) && (nnum == 1) && (totalnum == 1) )
			continue;
		else
			return 1;
	}
	return 0;
}


////////////////////
// CBezLine
////////////////////

CBezLine::CBezLine()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CBezLine::CBezLine : CreateObjs error !!!\n" );
		isinit = 0;
	}
}
CBezLine::~CBezLine()
{
	DestroyObjs();
}

int	CBezLine::CopyData( CBezLine* srcdata )
{
	int ret;
	ret = CVecMesh::CopyData( (CVecMesh*)srcdata );
	chkflag = srcdata->chkflag;
	return ret;
}

int CBezLine::InitVecMesh( CBezLine* srcdata )
{
	int ret;
	ret = CVecMesh::InitVecMesh( (CVecMesh*)srcdata );
	return ret;
}

int CBezLine::SetMem( CBezData* srcbezdat, int srcdatno, __int64 setflag )
{
	int ret, i;
	CVecLine* dstvl = 0;
	VEC3F* srcbp = 0;

	if( setflag == BEZPOINT_ALL ){
		for( i = 0; i < LIM_MAX; i++ ){
			dstvl = (*this)( 0, i );
			srcbp = (*srcbezdat)( 0, i )->GetVec( 0 );
			ret = dstvl->SetMem( srcbp, srcdatno, VEC_ALL );
			if( ret ){
				DbgOut( "CBezLine : SetMem : type CBezData : SetMem error !!!\n" );
				return 1;
			}
		}
		return 0;
	}
	return 1;
}

int CBezLine::SetMem( CBezLine* srcbezline, __int64 setflag )
{
	int ret, i;

	if( setflag == BEZLINE_ALL ){
		if( !meshinfo ){
			DbgOut( "charpos2.cpp : CBezLine : SetMem : type CBezLine : meshinfo NULL !!!\n" );
			return 1;
		}

		ret = CheckMeshInfo( srcbezline->meshinfo );
		if( !ret ){
			DbgOut( "charpos2.cpp : CBezLine : SetMem : type CBezLine : CheckMeshInfo error !!!\n" );
			return 1;
		}

		ret = meshinfo->IsSameData( srcbezline->meshinfo, chkmeshi, 5 );
		if( !ret ){
			DbgOut( "charpos2.cpp : CBezLine : SetMem : type CBezLine : meshinfo->IsSameData error !!!\n" );
			return 1;
		}

		for( i = 0; i < LIM_MAX; i++ ){
			ret = CheckLineInfo( (*srcbezline)( i )->meshinfo, 1 );
			if( ret ){
				DbgOut( "charpos2.cpp : CBezLine: SetMem : type CBezLine : CheckLineInfo error !!!\n" );
				return 1;
			}

			ret = (*this)( i )->SetMem( (*srcbezline)( i ), VEC_LINE );
			if( ret ){
				DbgOut( "charpos2.cpp : CBezLine: SetMem : type CBezLine : lineptr->SetMem error !!!\n" );
				return 1;
			}
		}

		chkflag |= BEZLINE_ALL; // 1点ごとにセットした場合には、flagをチェックしないので、意味無いけど。

		return 0;
	}
	return 1;
}

int CBezLine::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	//meshinfo->m = 5; meshinfo->n = 1;
	int mnum, nnum, totalnum;

	if( !srcmeshinfo )
		return 1;

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( (mnum == LIM_MAX) && (nnum == 1) && (totalnum == mnum * nnum) )
		return 0;
	else
		return 1;
	
}

int CBezLine::CheckLineInfo( CMeshInfo* lineinfo, int infonum )
{
	//eachline->meshinfo->m = 任意(一定); eachline->meshinfo->n = 1;
	int i, mnum0;

	if( !lineinfo )
		return 1;

	mnum0 = lineinfo->m;
	for( i = 0; i < infonum; i++ ){
		if( !(lineinfo + i) )
			return 1;

		if( (mnum0 != (lineinfo + i)->m) || ((lineinfo + i)->n != 1) )
			return 1;
	}
	return 0;
	
}

