#include <stdafx.h>
#include <stdio.h>
#include <stdarg.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <math.h>

#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <basedat.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>

// CBaseDat, CMeshInfo, CVecLine


///////////////////////
// class CBaseDat
//////////////////////

CBaseDat::CBaseDat()
{
	InitParams();
}

CBaseDat::~CBaseDat()
{
	//isinit = 0;
	DestroyObjs();
}

void CBaseDat::InitParams()
{
	chkflag = 0;
	type = 0;
	state = 0;
	isinit = 1;
}


int CBaseDat::IsSameData( CBaseDat* cmpbasedat, __int64* cmpflag, int flagnum )
{
	int  i;
	int ret;
	int res = 0;

	for( i = 0; i < flagnum; i++ ){
		ret = IsSameData( cmpbasedat, *(cmpflag + i) );
		if( !ret ){
			return 0;
		}else{
			res += ret;
		}
	}
	return res;	
}

int CBaseDat::IsSameData( CBaseDat* cmpbasedat, __int64 cmpflag )
{
	// 同じ時　0 以外
	int ret = 0;

	int cmptype;

	if( !cmpbasedat )
	{
		return 0;
	}

	if( cmpflag & BASE_TYPE ){
		cmptype = cmpbasedat->type;
		if( type == cmptype )
			ret++;
		else
			return 0;

	}

	return ret;
}

int CBaseDat::AssertPointer( void* voidptr, char* mes )
{
	if( !voidptr ){
		DbgOut( "AssertPointer error" );
		DbgOut( mes );
		return 1;
	}
	return 0;
}

void CBaseDat::DestroyObjs()
{
}
int	CBaseDat::CopyData( CBaseDat* srcdata )
{
	HRESULT hres;

	if( !srcdata ){
		DbgOut( "charpos2.cpp : CBaseDat: CopyData : srcdata NULL !!!\n" );
		return 1;
	}

	SetLastError( 0 );
	MoveMemory( (void*)this, (void*)srcdata, sizeof( CBaseDat ) );
	hres = GetLastError();

	if( hres ){
		ErrorMessage( "CBaseDat : CopyData  error !!!\n", hres );
		return 1;
	}

	return 0;
}


int CBaseDat::DumpMem( HANDLE hfile, int tabnum, char* headerptr )
{
	int ret;
	SetTabChar( tabnum );
	if( (type < SHDTYPENONE) || (type > SHDTYPEMAX) ){
		DbgOut( "CBaseDat : DumpMem : type %d error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//_ASSERT( (type > SHDTYPENONE) && (type < SHDTYPEMAX) );
	
	ret = Write2File( hfile, "%s%s : type %s,\n", tabchar, headerptr, typechar[type] );
	_ASSERT( !ret );

	return ret;
}

void CBaseDat::ResetFlag()
{
	chkflag = 0;
}

int	CBaseDat::IsInit()
{
	return isinit;
}

int CBaseDat::SetType( int num )
{
	type = num;
	chkflag |= BASE_TYPE;
	return 0;
}
int CBaseDat::SetType( char* typechar )
{
	int typenum;
	int ret;

	typenum = atoi( typechar );
	
	ret = SetType( typenum );
	
	return ret;
}


int CBaseDat::SetMem( int* srcint, __int64 setflag )
{
	int ret;

	if( setflag == BASE_TYPE ){
		ret = SetType( *srcint );
		if( ret ){
			DbgOut( "charpos2.cpp : CBaseDat : SetMem : SetType error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}

int CBaseDat::IsSetMem( __int64* flagptr, int num )
{
	for( int i = 0; i < num; i++ ){
		if( !( chkflag & *(flagptr + i) ) ){
			return 0;
		}
	}

	return 1;
}

int CBaseDat::IsSetMem( __int64 flag )
{
	if( chkflag & flag )
		return 1;
	else
		return 0;

}


//////////////////////
// class CVec3f
//////////////////////

CVec3f::CVec3f()
{
	InitParams();
	isinit = 1;
}

CVec3f::CVec3f( float datax, float datay, float dataz )
{
	SetMem( datax, datay, dataz );
}

CVec3f::CVec3f( float datax, float datay, float dataz, __int64 setflag )
{
	SetMem( datax, datay, dataz, setflag );
}



CVec3f::~CVec3f()
{
	DestroyObjs();
}

void CVec3f::DestroyObjs()
{
	CBaseDat::DestroyObjs();
}

float CVec3f::Length()
{
	float mag, sqmag;
	mag = x * x + y * y + z * z;
	if( mag != 0.0f )
		sqmag = (float)sqrt( mag );
	else
		sqmag = 0.0f;

	return sqmag;
}

int CVec3f::Normalize()
{
	float mag, sqmag;
	mag = x * x + y * y + z * z;
	if( mag != 0.0f )
		sqmag = (float)sqrt( mag );
	else
		sqmag = 0.0f;

	if( mag != 0.0f ){
		x /= sqmag;
		y /= sqmag;
		z /= sqmag;
	}else{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	return 0;
}

float CVec3f::DotProduct( CVec3f srcvec )
{
	float dot;
	dot = x * srcvec.x + y * srcvec.y + z * srcvec.z;
	return dot;
}

int	CVec3f::SetMem( float datax, float datay, float dataz )
{
	x = datax;
	y = datay;
	z = dataz;

	chkflag = VEC_ALL;
	//type = 0;
	//state = 0;
	
	return 0;
}

int CVec3f::SetMem( float datax, float datay, float dataz, __int64 setflag )
{
	type = 0;

	if( setflag & VEC_X ){
		x = datax;
		chkflag |= VEC_X;
	}else{
		x = 0.0f;
	}

	if( setflag & VEC_Y ){
		y = datay;
		chkflag |= VEC_Y;
	}else{
		y = 0.0f;
	}

	if( setflag & VEC_Z ){
		z = dataz;
		chkflag |= VEC_Z;
	}else{
		z = 0.0f;
	}
	state = 0;
	return 0;

}

int CVec3f::SetMem( int* srcint, __int64 setflag )
{
	int ret;
	if( setflag & BASE_TYPE ){
		ret = CBaseDat::SetMem( srcint, BASE_TYPE );
		if( ret ){
			DbgOut( "charpos2.cpp : CVec3f : SetMem : basedat::SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;
}

int CVec3f::SetMem( CVec3f* srcvec, __int64 setflag )
{
	int i;
	__int64 isset;
	float *setfloatptr[3] = {&x, &y, &z};
	float srcfloat[3];

	if( setflag == VEC_ALL ){
// CVec3f*, name, typeなし。　すべてセットしたいときは　CopyData()
		x = srcvec->x;
		y = srcvec->y;
		z = srcvec->z;
		chkflag |= VEC_ALL;
		return 0;
	}else{

		srcfloat[0] = srcvec->x;
		srcfloat[1] = srcvec->y;
		srcfloat[2] = srcvec->z;
		for( i = 0; i < 3; i++ ){
			isset = setflag & chkvec[i];
			if( isset ){
				*setfloatptr[i] = srcfloat[i];
				chkflag |= chkvec[i];
				return 0;
			}
		}
		return 1;
	}

}
int CVec3f::SetMem( VEC3F srcvec, __int64 setflag )
{
	int i;
	__int64 isset;
	float *setfloatptr[3] = {&x, &y, &z};
	float srcfloat[3];

	if( setflag == VEC_ALL ){
// CVec3f*, name, typeなし。　すべてセットしたいときは　CopyData()
		x = srcvec.x;
		y = srcvec.y;
		z = srcvec.z;
		chkflag |= VEC_ALL;
		return 0;
	}else{

		srcfloat[0] = srcvec.x;
		srcfloat[1] = srcvec.y;
		srcfloat[2] = srcvec.z;
		for( i = 0; i < 3; i++ ){
			isset = setflag & chkvec[i];
			if( isset ){
				*setfloatptr[i] = srcfloat[i];
				chkflag |= chkvec[i];
				return 0;
			}
		}
		return 1;
	}

}

int CVec3f::SetMem( float* srcfloat, __int64 setflag )
{
	int i;
	__int64 isset;
	float *setfloatptr[3] = {&x, &y, &z};

	if( setflag & VEC_ALL ){
		for( i = 0; i < 3; i++ ){
			isset = setflag & chkvec[i];
			if( isset ){				
				*(setfloatptr[i]) = *srcfloat;
				chkflag |= chkvec[i];
				return 0;
			}
		}
	}

	return 1;
}



void CVec3f::InitParams()
{
	CBaseDat::InitParams();

	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

int	CVec3f::CopyData( CVec3f* srcdata )
{
	HRESULT	hres;

	if( !srcdata ){
		DbgOut( "charpos2.cpp : CVec3f : CopyData : srcdata NULL !!!\n" );
		return 1;
	}
	
	SetLastError( 0 );
	MoveMemory( (void*)this, (const void*)srcdata, sizeof( CVec3f ) );
	hres = GetLastError();
	if( hres ){
		ErrorMessage( "CVec3f : CopyData error !!!\n", hres );
		return 1;
	}

	return 0;

}

int CVec3f::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{

	int	ret;

	SetTabChar( tabnum );

	ret = CBaseDat::DumpMem( hfile, tabnum, "3DOBJ" );
	if( ret ){
		DbgOut( "charpos2.cpp : CVec3f : DumpMem : basedat dump error !!!\n" );
		return 1;
	}

	ret = Write2File( hfile, "%s X : %f, Y : %f, Z : %f\n", tabchar, x, y, z );
	_ASSERT( !ret );

	return ret;
}

////////////////////////
// CMeshInfo
////////////////////////

CMeshInfo::CMeshInfo()
{
	int	ret;
	InitParams();
	ret = CreateObjs();
	if( ret )
		isinit = 0;
}

CMeshInfo::~CMeshInfo()
{
	DestroyObjs();
}

void	CMeshInfo::InitParams()
{
	CBaseDat::InitParams();
	m = 0;
	n = 0;
	total = 0;
	mclosed = 0;
	nclosed = 0;
	skip = 0;
	hascolor = 0;
}

int CMeshInfo::IsSameData( CMeshInfo* cmpmeshinfo, __int64* cmpflag, int flagnum )
{
	int ret, i;
	int res = 0;

	for( i = 0; i < flagnum; i++ ){
		ret = IsSameData( cmpmeshinfo, *(cmpflag + i) );
		if( !ret ){
			return 0;
		}else{
			res += ret;
		}
	}
	return res;
}

int CMeshInfo::IsSameData( CMeshInfo* cmpmeshinfo, __int64 cmpflag )
{
	int cmpdata;
	int ret = 0;

	if( !cmpmeshinfo )
		return 0;

	if( cmpflag & MESHI_M ){
		cmpdata = cmpmeshinfo->m;
		if( m == cmpdata )
			ret++;
		else
			return 0;
	}

	if( cmpflag & MESHI_N ){
		cmpdata = cmpmeshinfo->n;
		if( n == cmpdata )
			ret++;
		else
			return 0;
	}

	if( cmpflag & MESHI_TOTAL ){
		cmpdata = cmpmeshinfo->total;
		if( total == cmpdata )
			ret++;
		else
			return 0;
	}

	if( cmpflag & MESHI_MCLOSED ){
		cmpdata = cmpmeshinfo->mclosed;
		if( mclosed == cmpdata )
			ret++;
		else
			return 0;
	}

	if( cmpflag & MESHI_NCLOSED ){
		cmpdata = cmpmeshinfo->nclosed;
		if( nclosed == cmpdata )
			ret++;
		else
			return 0;
	}

	if( cmpflag & MESHI_SKIP ){
		cmpdata = cmpmeshinfo->skip;
		if( skip == cmpdata )
			ret++;
		else
			return 0;
	}

	if( cmpflag & MESHI_HASCOLOR ){
		cmpdata = cmpmeshinfo->hascolor;
		if( hascolor == cmpdata )
			ret++;
		else
			return 0;
	}

	return ret;
}

int	CMeshInfo::CreateObjs()
{
	int ret1;
	CVec3f* newvec;
	CVec3f** vecarray[3] = { &diffuse, &specular, &ambient };
	//char*	vecname[3] = { "diffuse", "specular", "ambient" };
	int		vectype = SHDMATERIAL;

	int i;

	for( i = 0; i < 3; i++ ){
	// alloc
		newvec = new CVec3f();
		if( !newvec ){
			DbgOut( "charpos2.cpp : CMeshInfo : CreateObj : material alloc error !!!\n" );
			return 1;
		}
	// settype
		ret1 = newvec->SetType( vectype );
		if( ret1 ){
			DbgOut( "charpos2.cpp : CMeshInfo : CreateObj : material SetType error !!!\n" );
			return 1;
		}
	// set pointer
		*(vecarray[i]) = newvec;
	}

	return 0;
}

int CMeshInfo::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	int	i;
	int	ret;
	char	setchar[3][10] = { "diffuse", "specular", "ambient" };
	CVec3f*	setvec[3] = { diffuse, specular, ambient };
	
	ret = CBaseDat::DumpMem( hfile, tabnum, "MESHINFO" );
	if( ret ){
		DbgOut( "charpos2.cpp : CMeshInfo : DumpMem : basedat dump error !!!\n" );
		return 1;
	}

	tabnum++;
	SetTabChar( tabnum );

	ret = Write2File( hfile, "%sM : %d, N : %d, Total : %d, M_CLOSED : %d, N_CLOSED : %d, skip : %d\n",
		tabchar, m, n, total, mclosed, nclosed, skip );
	_ASSERT( !ret );


	if( (dumpflag & DISP_MATERIAL) && (chkflag & MESHI_MATERIAL) ){
		ret = Write2File( hfile, "%s hascolor %d\n", tabchar, hascolor );
		if( ret ){
			DbgOut( "CMeshInfo : DumpMem : hascolor dump error !!!\n" );
			return 1;
		}

		for( i = 0; i < 3; i++ ){
			ret = Write2File( hfile, "%s %s\n", tabchar, setchar[i] );
			if( ret ){
				DbgOut( "charpos2.cpp : MeshInfo:DumpMem : material dump error !!!\n" );
				return 1;
			}
			ret = setvec[i]->DumpMem( hfile, tabnum, dumpflag );
			if( ret ){
				DbgOut( "charpos2.cpp : MeshInfo:DumpMem : material dump error !!!\n" );
				return 1;
			}
		}
	}
	return ret;
}

int CMeshInfo::NewMeshInfo( CMeshInfo** hnewinfo )
{
	CMeshInfo* newmeshinfo = 0;
	int ret;
	

	newmeshinfo = new CMeshInfo();
	if( !newmeshinfo ){
		DbgOut( "charpos2.cpp : CMeshInfo : NewMeshInfo : newmeshinfo alloc error !!!\n" );
		goto newmesherr;
	}



	ret = newmeshinfo->CopyData( this );
	if( ret ){
		DbgOut( "charpos2.cpp : CMeshInfo : NewMeshInfo : newmeshinfo->CopyData error !!!\n" );
		goto newmesherr;
	}


	_ASSERT( (newmeshinfo->type > SHDTYPENONE) && (newmeshinfo->type < SHDTYPEMAX) );

	// totalnum のチェック
	//DbgOut( "charpos2.cpp : CMeshInfo : NewMeshInfo : bef check : m %d, n %d, total %d\n", m, n, total );
	total = m * n;
	DbgOut( "charpos2.cpp : CMeshInfo : NewMeshInfo : m %d, n %d, total %d\n", m, n, total );
	
	if( total <= 0 ){
		if( ((type < SHDROTATE) || (type > SHDPART)) && (type != SHDPOLYMESH) && (type != SHDPOLYMESH2) && (type != SHDGROUNDDATA) ){ // part, joint, polymesh, polymesh2, grounddata の場合はエラーではない。
			DbgOut( "charpos2.cpp : CMeshInfo : NewMeshInfo : totalno error !!!\n" );
			_ASSERT( 0 );
			goto newmesherr;
		}
	}

	
	newmeshinfo->total = total;

	newmeshinfo->mclosed = mclosed;// 2004/4/1
	newmeshinfo->nclosed = nclosed;// 2004/4/1
	newmeshinfo->skip = skip;

	*hnewinfo = newmeshinfo;
	return 0;

newmesherr:
	if( newmeshinfo )
		delete newmeshinfo;
	DbgOut( "charpos2.cpp : CMeshInfo : NewMeshInfo error !!!\n"  );
	return 1;
}

int	CMeshInfo::CopyData( CMeshInfo* srcdata )
{
	int ret = 0;
	HRESULT hres;
	CVec3f* save_diff = 0;
	CVec3f*	save_spec = 0;
	CVec3f*	save_amb = 0;


	if( !srcdata ){
		DbgOut( "charpos2.cpp : CMeshInfo : CopyData : srcdata NULL !!!\n" );
		return 1;
	}

	// pointer メンバーの退避
	save_diff = diffuse;
	save_spec = specular;
	save_amb = ambient;

	// ベタコピー
	SetLastError( 0 );
	MoveMemory( (void*)this, (const void*)srcdata, sizeof( CMeshInfo ) );
	hres = GetLastError();
	if( hres ){
		ErrorMessage( "CMeshInfo : CopyData error ", hres );
		return 1;
	}

	// pointerメンバの復元と、内容コピー
	diffuse = save_diff;
	specular = save_spec;
	ambient = save_amb;
	
	ret = CopyMaterial( srcdata );
	if( ret ){
		DbgOut( "CMeshInfo : CopyData : CopyMaterial error !!!\n" );
		return 1;
	}

	//ret1 = diffuse->CopyData( srcdata->diffuse );
	//ret2 = specular->CopyData( srcdata->specular );
	//ret3 = ambient->CopyData( srcdata->ambient );
	//if( ret1 || ret2 || ret3 ){
	//	DbgOut( "charpos2.cpp : MeshInfo : CopyData : material CopyData error !!!\n" );
	//	return 1;
	//}

	return 0;
}

int CMeshInfo::CopyMaterial( CMeshInfo* srcmi )
{
	int ret1, ret2, ret3;

	ret1 = diffuse->CopyData( srcmi->diffuse );
	ret2 = specular->CopyData( srcmi->specular );
	ret3 = ambient->CopyData( srcmi->ambient );
	if( ret1 || ret2 || ret3 ){
		DbgOut( "CMeshInfo : CopyMaterial : CVec3f->CopyData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	chkflag |= MESHI_MATERIAL;

	return 0;
}

int CMeshInfo::SetDefaultMaterial()
{
	int ret1, ret2, ret3, ret4;
	CVec3f defaultmat;
	CVec3f zerovec;
	zerovec.x = 0.0f;
	zerovec.y = 0.0f;
	zerovec.z = 0.0f;

	ret1 = defaultmat.SetMem( 1.0f, 1.0f, 1.0f, VEC_ALL );
	ret2 = diffuse->CopyData( &defaultmat );
	ret3 = specular->CopyData( &zerovec );
	ret4 = ambient->CopyData( &defaultmat );

	if( ret1 || ret2 || ret3 || ret4 ){
		DbgOut( "CMeshInfo : SetDefaultMaterial : CVec3f->CopyData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	chkflag |= MESHI_MATERIAL;
	
	return 0;
}


int CMeshInfo::ResetParams()
{
	int ret;
	DestroyObjs();
	InitParams();
	ret = CreateObjs();
	return ret;
}

void	CMeshInfo::DestroyObjs()
{
	CBaseDat::DestroyObjs();

	if( diffuse ){
		delete diffuse;
		diffuse = 0;
	}

	if( specular ){
		delete specular;
		specular = 0;
	}

	if( ambient ){
		delete ambient;
		ambient = 0;
	}
}

int CMeshInfo::HasColor()
{
	return hascolor;
}

CVec3f* CMeshInfo::GetMaterial( __int64 getflag )
{
	CVec3f* srcmat[3] = { diffuse, specular, ambient };
	CVec3f* retmat = 0;
	int i;

	for( i = 0; i < 3; i++ ){
		if( getflag == chkmaterial[i] ){
			retmat = srcmat[i];	
			break;
		}
	}
	return retmat;
}

int	CMeshInfo::SetMem( int idata, __int64 setflag )
{

	if( setflag & BASE_TYPE ){
		type = idata;
		chkflag |= BASE_TYPE;
	}else if( setflag & MESHI_M ){
		m = idata;
		chkflag |= MESHI_M;
	}else if( setflag & MESHI_N ){
		n = idata;
		chkflag |= MESHI_N;
	}else if( setflag & MESHI_TOTAL ){
		total = idata;
		chkflag |= MESHI_TOTAL;
	}else if( setflag & MESHI_MCLOSED ){
		mclosed = idata;
		chkflag |= MESHI_MCLOSED;
	}else if( setflag & MESHI_NCLOSED ){
		nclosed = idata;
		chkflag |= MESHI_NCLOSED;
	}else if( setflag & MESHI_SKIP ){
		skip = idata;
		chkflag |= MESHI_SKIP;
	}else if( setflag & MESHI_HASCOLOR ){
		hascolor = idata;
		chkflag |= MESHI_HASCOLOR;
	}else{
		DbgOut( "charpos2.cpp : MeshInfo : SetMem : setflag not implemented !!!\n" );
		state = 1;
		return 1;
	}

	return 0;
}

int CMeshInfo::SetMem( int* srcint, __int64 setflag )
{
	int ret, i;
	__int64 isset;
	int *setintptr[7] = {&m, &n, &total, &mclosed, &nclosed, &skip, &hascolor};


	if( setflag & BASE_TYPE ){
		ret = CBaseDat::SetMem( srcint, BASE_TYPE );
		if( ret ){
			DbgOut( "charpos2.cpp : MeshInfo : SetMem : basedat::SetMem error !!!\n" );
			return 1;
		}
		return 0;
	}else{
		for( i = 0; i < 7; i++ ){
			isset = setflag & chkmeshi[i];
			if( isset ){
				*(setintptr[i]) = *srcint;
				chkflag |= chkmeshi[i];
				return 0;
			}
		}
		return 1;
	}
}

int CMeshInfo::SetMem( CVec3f* srcvec, __int64 setflag )
{
	int ret, i, chk2;
	__int64 isset;
	CVec3f*	setarray[3] = {diffuse, specular, ambient};

	// MAT_*
	if( setflag & MATERIAL_ALL ){
		for( i = 0; i < 3; i++ ){
			isset = setflag & chkmaterial[i];
			if( isset ){
				ret = setarray[i]->SetMem( srcvec, VEC_ALL );
				if( ret ){
					DbgOut( "charpos2.cpp : MeshInfo : SetMem : srcvec SetMem error !!!\n" );
					return 1;
				}
				chkflag |= chkmaterial[i];
			}
		}

		chk2 = IsSetMem( chkmaterial, 3 ); 
		if( chk2 ){
			// diffuse, specular, ambient の　すべてのメンバがセットされたら
			// MESHI_MATERIAL を　セットする
			chkflag |= MESHI_MATERIAL; 
		}
		return 0;

	}

	DbgOut( "basedat : CMeshInfo : setflag error !!!\n" );
	return 1;
}

int CMeshInfo::SetMem( VEC3F srcvec, __int64 setflag )
{
	int ret, i, chk2;
	__int64 isset;
	CVec3f*	setarray[3] = {diffuse, specular, ambient};

	// MAT_*
	if( setflag & MATERIAL_ALL ){
		for( i = 0; i < 3; i++ ){
			isset = setflag & chkmaterial[i];
			if( isset ){
				ret = setarray[i]->SetMem( srcvec, VEC_ALL );
				if( ret ){
					DbgOut( "charpos2.cpp : MeshInfo : SetMem : srcvec SetMem error !!!\n" );
					return 1;
				}
				chkflag |= chkmaterial[i];
			}
		}

		chk2 = IsSetMem( chkmaterial, 3 ); 
		if( chk2 ){
			// diffuse, specular, ambient の　すべてのメンバがセットされたら
			// MESHI_MATERIAL を　セットする
			chkflag |= MESHI_MATERIAL; 
		}
		return 0;

	}
	DbgOut( "basedat : CMeshInfo : setflag error !!!\n" );
	return 1;
}

int CMeshInfo::SetMem( float* srcfloat, __int64 setflag )
{
	int ret, i;
	__int64	isset, tempflag, chk1, chk2;
	CVec3f*	setarray[3] = {diffuse, specular, ambient};

	// MESHI_MATERIAL | MAT_* | VEC_*
	if( setflag & MESHI_MATERIAL ){
		for( i = 0; i < 3; i++ ){
			isset = setflag & chkmaterial[i];
			if( isset ){
				tempflag = setflag & VEC_ALL; // VEC_*　だけにする
				ret = setarray[i]->SetMem( srcfloat, tempflag );
				if( ret ){
					DbgOut( "charpos2.cpp : MeshInfo : SetMem : srcfloat SetMem error !!!\n" );
					return 1;
				}
				chk1 = setarray[i]->IsSetMem( chkvec, 3 );
				if( chk1 ){
					chkflag |= chkmaterial[i];
					chk2 = IsSetMem( chkmaterial, 3 ); 
					if( chk2 ){
						// diffuse, specular, ambient の　すべてのメンバがセットされたら
						// MESHI_MATERIAL を　セットする
						chkflag |= MESHI_MATERIAL; 
					}
				}
				return 0;
			}
		}		
	}
	return 1;
}


///////////////////
// CVecLine
///////////////////
CVecLine::CVecLine()
{
	int ret;
	InitParams();
	ret = CreateObjs();
	if( ret ){
		DbgOut( "charpos2.cpp : CVecLine : CreateObjs error !!!\n" );
		isinit = 0;
	}
}

CVecLine::~CVecLine()
{
	DestroyObjs();
}


void	CVecLine::InitParams()
{
	CBaseDat::InitParams();

	vec = 0;
	meshinfo = 0;	
}
int	CVecLine::InitVecLine( CMeshInfo* srcmeshinfo )
{
	int ret;

	DestroyLine(); 	
	ret = CreateLine( srcmeshinfo );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecLine : InitVecLine : CreateLine error !!!\n" );
		return 1;
	}

	return 0;
}

VEC3F* CVecLine::GetVec( int vecno )
{
	int totalnum;

	if( !meshinfo || !vec )
		return 0;
	
	totalnum = meshinfo->total;
	if( (vecno >= 0) && (vecno < totalnum) )
		return (vec + vecno);
	else
		return 0;
}


int	CVecLine::CopyData( CVecLine* srcdata )
{
	int ret;
	int totalnum;
	unsigned long	leng, lret;
	//float* setptr[3] = { x, y, z };
	//float* srcptr[3];


	if( !srcdata ){
		DbgOut( "charpos2.cpp : CVecLine : CopyData : srcdata NULL !!!\n" );
		return 1;
	}

	ret = CBaseDat::CopyData( (CBaseDat*)srcdata );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecLine : CopyData : CBaseDat::CopyData error !!!\n" );
		return 1;
	}

	ret = CheckMeshInfo( srcdata->meshinfo );
	assert( !ret );

	ret = InitVecLine( srcdata->meshinfo );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecLine : CopyData : InitVecLine error !!!\n" );
		return 1;
	}

	totalnum = meshinfo->total;
	leng = sizeof(VEC3F) * totalnum;

	SetLastError( 0 );
	MoveMemory( (void*)vec, (const void*)srcdata->vec, leng );
	lret = GetLastError();
	if( lret ){
		DbgOut( "charpos2.cpp : CVecLine : CopyData : MoveMemory error !!!\n" );
		return 1;
	}

	chkflag = srcdata->chkflag;

	return 0;
}

int	CVecLine::DumpMem( HANDLE hfile, int tabnum, int dumpflag )
{
	int i;
	int ret;
	int totalnum;
	VEC3F* curvec = 0;
	float tempx, tempy, tempz;

	static int dbgcnt = 0;

	ret = CBaseDat::DumpMem( hfile, tabnum, "3DOBJ" );
	if( ret ){
		DbgOut( "charpos2.cpp : CVecLine : DumpMem : CBaseDat : DumpMem error !!!\n" );
		return 1;
	}

	tabnum++;
	SetTabChar( tabnum );

	if( meshinfo ){
		ret = meshinfo->DumpMem( hfile, tabnum, dumpflag );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine : DumpMem : meshinfo->DumpMem error !!!\n" );
			return 1;
		}
			
		if( dumpflag & DISP_VERTEX ){
			// line header
			totalnum = meshinfo->total;
			ret = Write2File( hfile, "%s LINE VEC\n", tabchar );
			_ASSERT( !ret );

			// line vec
			for( i = 0; i < totalnum; i++ ){
				curvec = vec + i;
				tempx = curvec->x;
				tempy = curvec->y;
				tempz = curvec->z;
			
				ret = Write2File( hfile, "%s X : %f, Y : %f, Z : %f\n",
					tabchar, tempx, tempy, tempz );
				_ASSERT( !ret );
			}
		}			
	}else{
		ret = Write2File( hfile, "%s VecLine not created !!! skip dump\n", tabchar );
		_ASSERT( !ret );
	}

	return ret;
}

int CVecLine::SetMem( int* srcint, __int64 setflag )
{
	int ret;
	if( setflag & BASE_TYPE ){
		ret = CBaseDat::SetMem( srcint, BASE_TYPE );
		if( ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : BASE_TYPE error !!!\n" );
			return 1;
		}
		return 0;
	}
	return 1;	
}

int CVecLine::SetMem( CVecLine* srcline, __int64 setflag )
{
	int ret, totalnum;
	unsigned long leng;
	HRESULT	hres;
	// name, type, meshinfo はそのままで、　x,y,z をすべてセットする
	// datano は無関係
	if( setflag & VEC_LINE ){
		ret = CheckMeshInfo( srcline->meshinfo );
		assert( !ret );

		ret = meshinfo->IsSameData( srcline->meshinfo, chkmeshi, 5 );
		// material, skip は　違っても可
		if( !ret ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVecLine : meshinfo->IsSameData error !!!\n" );
			return 1;
		}
		totalnum = meshinfo->total;
		leng = sizeof(VEC3F) * totalnum;
		SetLastError(0);
		MoveMemory( (void*)vec, (const void*)srcline->vec, leng );
		hres = GetLastError();
		if( hres ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : type CVecLine : MoveMemory error !!!\n" );
			return 1;
		}
		
		return 0;
	}
	return 1;
}

int CVecLine::SetMem( VEC3F* srcvec, int datano, __int64 setflag )
{
	int totalnum;

	// vec memberの　セットには　meshinfo が必要
	if( setflag & VEC_ALL ){
		if( !meshinfo || !vec ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : meshinfo NULL !!!\n" );
			return 1;
		}
		totalnum = meshinfo->total;
		if( (datano < 0) || (datano >= totalnum) ){
			// datano は　0 から total - 1
			DbgOut( "charpos2.cpp : CVecLine : SetMem : datano error !!!\n" );
			return 1;
		}

		if( setflag == VEC_ALL ){
			*(vec + datano) = *srcvec;
			return 0;
		}
	}

	return 1;
}

int CVecLine::SetMem( VEC3F* srcvec, int vecnum )
{
	int total;

	total = meshinfo->total;
	if( vecnum > total ){
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( vec, srcvec, sizeof( VEC3F ) * vecnum );

	return 0;
}

int CVecLine::SetMem( CVec3f* srcvec, int datano, __int64 setflag )
{
	int i, totalnum;
	__int64 isset;
	//int chkvec[3] = {VEC_X, VEC_Y, VEC_Z};
	float *setvecptr[3];
	float srcvecval[3];

	// vec memberの　セットには　meshinfo が必要
	if( setflag & VEC_ALL ){
		if( !meshinfo || !vec ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : meshinfo NULL !!!\n" );
			return 1;
		}
		totalnum = meshinfo->total;
		if( (datano < 0) || (datano >= totalnum) ){
			// datano は　0 から total - 1
			DbgOut( "charpos2.cpp : CVecLine : SetMem : datano error !!!\n" );
			return 1;
		}

		if( setflag == VEC_ALL ){
			// VEC3F <------ CVec3f
			(vec + datano)->x = srcvec->x;
			(vec + datano)->y = srcvec->y;
			(vec + datano)->z = srcvec->z;

			return 0;
		}else{
			srcvecval[0] = srcvec->x;
			srcvecval[1] = srcvec->y;
			srcvecval[2] = srcvec->z;

			setvecptr[0] = &((vec + datano)->x);
			setvecptr[1] = &((vec + datano)->y);
			setvecptr[2] = &((vec + datano)->z);
			for( i = 0; i < 3; i++ ){
				isset = setflag & chkvec[i];
				if( isset ){
					*(setvecptr[i]) = srcvecval[i];
					return 0;
				}
			}
			return 1;
		}
	}

	return 1;
}
int CVecLine::SetMem( float* srcfloat, int datano, __int64 setflag )
{
	int i, totalnum;
	__int64 isset;
	//int chkvec[3] = {VEC_X, VEC_Y, VEC_Z};
	float *setvecptr[3]; // = {x, y, z};


	// vec memberの　セットには　meshinfo が必要
	if( setflag & VEC_ALL ){
		if( !meshinfo || !vec ){
			DbgOut( "charpos2.cpp : CVecLine : SetMem : meshinfo NULL !!!\n" );
			return 1;
		}
		totalnum = meshinfo->total;
		if( (datano < 0) || (datano >= totalnum) ){
			// datano は　0 から total - 1
			DbgOut( "charpos2.cpp : CVecLine : SetMem : datano error !!!\n" );
			return 1;
		}

		setvecptr[0] = &((vec + datano)->x);
		setvecptr[1] = &((vec + datano)->y);
		setvecptr[2] = &((vec + datano)->z);
		for( i = 0; i < 3; i++ ){
			isset = setflag & chkvec[i];
			if( isset ){
				*(setvecptr[i]) = *srcfloat;
				return 0;
			}
		}
		return 1;
	}

	return 1;
}

void	CVecLine::ResetParams()
{
	// destroy
	DestroyObjs();

	// init
	InitParams();

}

void	CVecLine::DestroyObjs()
{
	// meshinfo, x, y, z 以外のpointer memberの　destroy
	CBaseDat::DestroyObjs();
	
	// meshinfo, x, y, z のdestroy
	DestroyLine();

}
int	CVecLine::CreateObjs()
{
	// meshinfo, x, y, z 以外のpointer memberの　alloc
	return 0;
}

int	CVecLine::CreateLine( CMeshInfo* srcmeshinfo )
{
	int ret1;
	int totalnum;
	HRESULT hres;
// meshinfo

	if( !srcmeshinfo ){
		DbgOut( "charpos2.cpp : CVecLine : CreateLine : srcmeshinfo NULL !!!\n" );
		return 1;			
	}

	ret1 = CheckMeshInfo( srcmeshinfo );
	_ASSERT( !ret1 );

	ret1 = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret1 || !meshinfo ){
		DbgOut( "charpos2.cpp : CVecLine : CreateLine ; srcmeshinfo->NewMeshinfo error !!!\n" );
		return 1;
	}

	// type
	if( meshinfo->type ){
		ret1 = CBaseDat::SetType( meshinfo->type );
		if( ret1 ){
			DbgOut( "charpos2.cpp : CVecLine : CreateLine : base::SetType error !!!\n" );
			return 1;
		}
	}
	totalnum = meshinfo->total;

	//DbgOut( "charpos2.cpp : CVevLine : CreateLine : bef vec alloc : totalnum %d\n", totalnum );

	// VEC3F の　Create
	vec = (VEC3F*)realloc( vec, sizeof( VEC3F ) * totalnum );
	if( !vec ){
		DbgOut( "charpos2.cpp : CVecLine : CreateLine : vec alloc error !!!\n" );
		return 1;
	}
	SetLastError( 0 );
	ZeroMemory( vec, sizeof( VEC3F ) * totalnum );
	hres = GetLastError();
	if( hres ){
		DbgOut( "charpos2.cpp : CVecLine : CreateLine : vec ZeroMemory error !!!\n" );
		return 1;
	}

	//DbgOut( "charpos2.cpp : CVevLine : CreateLine : aft vec alloc\n" );
	return 0;
}

int CVecLine::DestroyLine()
{
	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	if( vec ){
		free( vec );
		vec = 0;
	}

	return 0;
}

int CVecLine::CheckMeshInfo( CMeshInfo* srcmeshinfo )
{
	int mnum, nnum, totalnum;
	assert( srcmeshinfo );

	mnum = srcmeshinfo->m;
	nnum = srcmeshinfo->n;
	totalnum = srcmeshinfo->total;

	if( (nnum == 1) && (mnum == totalnum) )
		return 0;
	else
		return 1;	
}

int CVecLine::Transform( CMatrix2& t, CVecLine* srcline )
{
	// 
	int i, totalnum, srctotal;
	VEC3F* curvec = vec;
	VEC3F* cursrc = srcline->vec;

	float srcx, srcy, srcz, w, invw;
	float dstx, dsty, dstz;

	totalnum = meshinfo->total;
	srctotal = srcline->meshinfo->total;

	w = t[3][3];
	if( (totalnum != srctotal) || (w == 0.0f) )
		return 1;

	invw = 1.0f / w;

	for( i = 0; i < totalnum; i++ ){
		srcx = cursrc->x;
		srcy = cursrc->y;
		srcz = cursrc->z;
		dstx = (srcx * t[0][0] + srcy * t[1][0] + srcz * t[2][0] + t[3][0]) * invw;
		dsty = (srcx * t[0][1] + srcy * t[1][1] + srcz * t[2][1] + t[3][1]) * invw;
		curvec->x = dstx;
		dstz = (srcx * t[0][2] + srcy * t[1][2] + srcz * t[2][2] + t[3][2]) * invw;
		curvec->y = dsty;
		curvec->z = dstz;

		curvec++; cursrc++;
	}

	return 0;
}

/***

inline static CVecLine operator/ (const CVecLine &p, const float f ) {
	CVecLine q;
	VEC3F* curvec = vec;
	int i, totalnum;

	totalnum = p.meshinfo->total;
	for( i = 0; i < totalnum; i++ ){
		curvec->x 

		curvec++;
	}


	_ASSERT( f );
	q.x = p.x / f;
	q.y = p.y / f;
	q.z = p.z / f;
	return q;
}

inline static CVecLine operator* (const CVecLine &p, const CMatrix2 &t) {
	CVecLine q;
	q.x = p[0]*t[0][0] + p[1]*t[1][0] + p[2]*t[2][0] + t[3][0];
	q.y = p[0]*t[0][1] + p[1]*t[1][1] + p[2]*t[2][1] + t[3][1];
	q.z = p[0]*t[0][2] + p[1]*t[1][2] + p[2]*t[2][2] + t[3][2];
	if (t[3][3] != 1.0) q /= t[3][3];
	//if (t[3][3] != 0.0) q /= t[3][3];
	return q;
}

inline static CVecLine &operator*= (CVecLine &a, const CMatrix2 &b) {
	a = a * b;
	return a;
}

***/