#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <mqoface.h>

typedef struct tag_mqotriline
{
	int index[2];
	float leng;
	int sortno;
} MQOTRILINE;


// static 

static int SortTriLine( const VOID* arg1, const VOID* arg2 );
static int s_allocno = 0;

////////////////////////////////


CMQOFace::CMQOFace()
{
	InitParams();
	serialno = s_allocno++;

}
CMQOFace::~CMQOFace()
{


}

void CMQOFace::InitParams()
{
	pointnum = 0;
	ZeroMemory( index, sizeof( int ) * 4 );
	materialno = -1;

	hasuv = 0;
	ZeroMemory( uv, sizeof( COORDINATE ) * 4 );


	mikobonetype = MIKOBONE_NONE;
	parentindex = -1;
	childindex = -1;
	hindex = -1;

	bonename[0] = 0;
	parent = 0;
	child = 0;
	brother = 0;

	next = 0;

	dirtyflag = 0;


	int i;
	for( i = 0; i < 4; i++ ){
		col[i] = 0xFFFFFFFF;
	}

	vcolsetflag = 0;
}

int CMQOFace::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstint = atoi( tempchar );

		*stepnum = endpos - pos;

	}else{
		_ASSERT( 0 );
	}


	return 0;
}

int CMQOFace::GetI64( __int64* dsti64, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		//*dstuint = (unsigned int)atol( tempchar );
		//*dstlong = atol( tempchar );
		*dsti64 = _atoi64( tempchar );

		*stepnum = endpos - pos;

//DbgOut( "check !!! : mqoface : GetUInt : tempchar %s, dsti64 %d\r\n", tempchar, *dsti64 );


	}else{
		_ASSERT( 0 );
	}

	return 0;
}


int CMQOFace::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) &&  
		( ( isdigit( *(srcchar + startpos) ) == 0 ) && (*(srcchar + startpos) != '-') && (*(srcchar + startpos) != '.') ) 
	
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( (isdigit( *(srcchar + endpos) ) != 0) || ( *(srcchar + endpos) == '-' ) || (*(srcchar + endpos) == '.') )
	){
		endpos++;
	}

	char tempchar[256];
	if( (endpos - startpos < 256) && (endpos - startpos > 0) ){
		strncpy_s( tempchar, 256, srcchar + startpos, endpos - startpos );
		tempchar[endpos - startpos] = 0;

		*dstfloat = (float)atof( tempchar );

		*stepnum = endpos - pos;
	}else{
		_ASSERT( 0 );
	}


	return 0;
}

int CMQOFace::SetParams( char* srcchar, int srcleng, int setmatno )
{
	int ret;
	int pos, stepnum;
	char* find;

//pointnum
	ret = GetInt( &pointnum, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MQOFace : SetParams : GetInt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	if( (pointnum < 0) || (pointnum > 4) ){
		DbgOut( "MQOFace : SetParams : pointnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//index
	char patv[] = "V(";
	int patvleng = (int)strlen( patv );
	find = strstr( srcchar, patv );
	if( !find ){
		DbgOut( "MQOFace : SetParams : find patv error !!!\n" );
		_ASSERT( 0 );
		return 1;		
	}
	pos = (int)( find + patvleng - srcchar );

	int i;
	for( i = 0; i < pointnum; i++ ){
		ret = GetInt( index + i, srcchar, pos, srcleng, &stepnum );
		if( ret ){
			DbgOut( "MQOFace : SetParams : GetInt error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		pos += stepnum;
		if( (i != pointnum - 1) && (pos >= srcleng) ){
			DbgOut( "MQOFace : SetParams : pos error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
/***
//materialno
	char patm[] = "M(";
	int patmleng = (int)strlen( patm );
	find = strstr( srcchar, patm );
	if( find ){	
		pos = (int)( find + patmleng - srcchar );

		ret = GetInt( &materialno, srcchar, pos, srcleng, &stepnum );
		if( ret ){
			DbgOut( "MQOFace : SetParams : GetInt error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
***/
	materialno = setmatno;

//uv
	char patuv[] = "UV(";
	int patuvleng = (int)strlen( patuv );
	find = strstr( srcchar, patuv );
	if( find ){
		hasuv = 1;//!!!!!!!!!

		pos = (int)( find + patuvleng - srcchar );
		
		for( i = 0; i < pointnum; i++ ){
			ret = GetFloat( &(uv[i].u), srcchar, pos, srcleng, &stepnum );
			if( ret ){
				DbgOut( "MQOFace : SetParams : GetInt error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;
			if( pos >= srcleng ){
				DbgOut( "MQOFace : SetParams : pos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = GetFloat( &(uv[i].v), srcchar, pos, srcleng, &stepnum );
			if( ret ){
				DbgOut( "MQOFace : SetParams : GetInt error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;
			if( (i != pointnum - 1) && (pos >= srcleng) ){
				DbgOut( "MQOFace : SetParams : pos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}
	}

//col
	char patcol[] = "COL(";
	int patcolleng = (int)strlen( patcol );
	find = strstr( srcchar, patcol );

	if( find ){
		pos = (int)( find + patcolleng - srcchar );

		int i;
		for( i = 0; i < pointnum; i++ ){
			ret = GetI64( col + i, srcchar, pos, srcleng, &stepnum );
			if( ret ){
				DbgOut( "MQOFace : SetParams : GetInt error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			pos += stepnum;
			if( (i != pointnum - 1) && (pos >= srcleng) ){
				DbgOut( "MQOFace : SetParams : pos error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

//DbgOut( "check !!! mqoface : %d, %d\r\n", i, col[i] );


		}
		vcolsetflag = 1;

	}else{
		vcolsetflag = 0;
	}

	return 0;
}


int CMQOFace::Dump()
{
	DbgOut( "\t\t%d V(", pointnum );
	int i;
	for( i = 0; i < pointnum; i++ ){
		DbgOut( " %d ", index[i] );
	}
	DbgOut( ")" );

	if( materialno >= 0 ){
		DbgOut( " M(%d) ", materialno );
	}

	if( hasuv == 1 ){
		DbgOut( "UV( " );

		for( i = 0; i < pointnum; i++ ){
			DbgOut( "%f %f ", uv[i].u, uv[i].v );
		}

		DbgOut( ")" );
	}

	DbgOut( "\n" );

	return 0;
}

int CMQOFace::CheckSameLine( CMQOFace* chkface, int* findflag )
{
	int pno;
	for( pno = 0; pno < pointnum; pno++ ){
		int v0, v1;
		v0 = index[pno];
		if( pno != pointnum - 1 )
			v1 = index[pno + 1];
		else
			v1 = index[0];

		int chknum = chkface->pointnum;
		int chkno;
		for( chkno = 0; chkno < chknum; chkno++ ){
			int chkv0, chkv1;
			chkv0 = chkface->index[chkno];
			if( chkno != chknum - 1 )
				chkv1 = chkface->index[chkno + 1];
			else
				chkv1 = chkface->index[0];

			if( ((v0 == chkv0) && (v1 == chkv1))
			|| ((v0 == chkv1) && (v1 == chkv0)) ){
				*(findflag + pno) = 1;
				break;
			}
		}
	}

	return 0;
}

int CMQOFace::SetInvFace( CMQOFace* srcface, int offset )
{
	materialno = srcface->materialno;
	pointnum = srcface->pointnum;
	int i;
	for( i = 0; i < pointnum; i++ ){
		index[i] = srcface->index[ pointnum - 1 - i ] + offset;
	}
	hasuv = srcface->hasuv;
	for( i = 0; i < pointnum; i++ ){
		uv[i] = srcface->uv[ pointnum - 1 - i ];
	}

	return 0;
}


int SortTriLine( const VOID* arg1, const VOID* arg2 )
{
    MQOTRILINE* p1 = (MQOTRILINE*)arg1;
    MQOTRILINE* p2 = (MQOTRILINE*)arg2;
    
    if( p1->leng < p2->leng )
        return -1;
	else if( p1->leng == p2->leng )
		return 0;
	else
		return 1;
}


int CMQOFace::SetMikoBoneIndex3( VEC3F* pbuf )
{
	if( pointnum != 3 ){
		DbgOut( "mqoface : SetMikoBoneIndex3 : pointnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	mikobonetype = MIKOBONE_NORMAL;

	MQOTRILINE triline[3];
	triline[0].index[0] = index[0];
	triline[0].index[1] = index[1];

	triline[1].index[0] = index[1];
	triline[1].index[1] = index[2];

	triline[2].index[0] = index[2];
	triline[2].index[1] = index[0];

	int lineno;
	VEC3F* vec0;
	VEC3F* vec1;
	float mag;
	for( lineno = 0; lineno < 3; lineno++ ){
		vec0 = ( pbuf + triline[lineno].index[0] );
		vec1 = ( pbuf + triline[lineno].index[1] );

		mag = ( vec0->x - vec1->x ) * ( vec0->x - vec1->x ) +
			( vec0->y - vec1->y ) * ( vec0->y - vec1->y ) +
			( vec0->z - vec1->z ) * ( vec0->z - vec1->z );

		if( mag != 0.0f ){
			triline[lineno].leng = (float)sqrt( mag );
		}else{
			triline[lineno].leng = 0.0f;
		}
	}

	qsort( triline, 3, sizeof( MQOTRILINE ), SortTriLine );

//DbgOut( "mqoface : triline leng %f %f %f\r\n", triline[0].leng, triline[1].leng, triline[2].leng );


	if( (triline[0].index[0] == triline[1].index[0]) || (triline[0].index[0] == triline[1].index[1]) ){
		parentindex = triline[0].index[0];
	}else{
		parentindex = triline[0].index[1];
	}

	if( (triline[1].index[0] == triline[2].index[0]) || (triline[1].index[0] == triline[2].index[1]) ){
		childindex = triline[1].index[0];
	}else{
		childindex = triline[1].index[1];
	}

	if( (triline[2].index[0] == triline[0].index[0]) || (triline[2].index[0] == triline[0].index[1]) ){
		hindex = triline[2].index[0];
	}else{
		hindex = triline[2].index[1];
	}


	return 0;
}

/***
// mqoobjectに、修正関数。

int CMQOFace::SetMikoBoneIndex2( CMQOFace* srcface, int srcfacenum )
{
	if( pointnum != 2 ){
		DbgOut( "mqoface : SetMikoBoneIndex2 : pointnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int curindex;
	curindex = index[0];

	CMQOFace* findface = 0;
	int fno;
	for( fno = 0; fno < srcfacenum; fno++ ){
		if( (srcface + fno)->pointnum == 3 ){
			int i;
			for( i = 0; i < 3; i++ ){
				if( curindex == (srcface + fno)->index[i] ){
					findface = srcface + fno;
				}
			}


			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//2点とも3角形に含まれている場合は、ゴミのラインの可能性が、非常に高いので、無効にする。
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			int find2 = 0;
			for( i = 0; i < 3; i++ ){
				if( index[1] == (srcface + fno)->index[i] ){
					find2 = 1;
				}
			}
			if( find2 )
				findface = 0;//!!!!!!!!!!!!!!!!!

			if( findface )
				break;


			}
			
		}
	}

	if( findface ){
		if( curindex == findface->parentindex ){
			parentindex = index[1];
			childindex = index[0];
		}else if( curindex == findface->childindex ){
			parentindex = index[0];
			childindex = index[1];
		}else{
			_ASSERT( 0 );
			mikobonetype = MIKOBONE_NONE;//!!!!!!!!!!
		}

		mikobonetype = MIKOBONE_FLOAT;

	}else{
		mikobonetype = MIKOBONE_NONE;//!!!!!!!!!!!!!!!!!
	}

	return 0;
}
***/


int CMQOFace::SetMikoBoneName( char* srcname, int lrflag )
{
	int leng1;

	char LRpat[20] = "[]";
	char Lpat[20] = "[L]_X+";
	char Rpat[20] = "[R]_X-";

	if( lrflag == 0 ){
		leng1 = (int)strlen( srcname );
		if( leng1 >= MIKONAMELENG ){
			DbgOut( "mqoface : SetMikoBoneName : name leng too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		strcpy_s( bonename, MIKONAMELENG, srcname );

	}else if( lrflag == 1 ){
		leng1 = (int)strlen( srcname );
		
		if( (leng1 - 2 + 6) >= MIKONAMELENG ){
			DbgOut( "mqoface : SetMikoBoneName : name leng too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		strncpy_s( bonename, MIKONAMELENG, srcname, leng1 - 2 );
		bonename[leng1 - 2] = 0;
		strcat_s( bonename, MIKONAMELENG, Lpat );


	}else if( lrflag == 2 ){
		leng1 = (int)strlen( srcname );
		
		if( (leng1 - 2 + 6) >= MIKONAMELENG ){
			DbgOut( "mqoface : SetMikoBoneName : name leng too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		strncpy_s( bonename, MIKONAMELENG, srcname, leng1 - 2 );
		bonename[leng1 - 2] = 0;
		strcat_s( bonename, MIKONAMELENG, Rpat );

	}else{
		_ASSERT( 0 );

		leng1 = (int)strlen( srcname );
		if( leng1 >= MIKONAMELENG ){
			DbgOut( "mqoface : SetMikoBoneName : name leng too long error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		strcpy_s( bonename, MIKONAMELENG, srcname );
	}


	return 0;
}

int CMQOFace::CheckLRFlag( VEC3F* pointptr, int* lrflagptr )
{
	if( (mikobonetype != MIKOBONE_NORMAL) && (mikobonetype != MIKOBONE_FLOAT) ){
		DbgOut( "mqoface : CheckLRFlag : mikobonetype error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (parentindex < 0) || (childindex < 0) ){
		DbgOut( "mqoface : CheckLRFlag : index not set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	VEC3F* parp;
	VEC3F* chilp;
	float centerx;

	parp = pointptr + parentindex;
	chilp = pointptr + childindex;

	centerx = ( parp->x + chilp->x ) * 0.5f;

	if( centerx == 0.0f ){
		*lrflagptr = 0;
	}else if( centerx > 0.0f ){
		*lrflagptr = 1;
	}else{
		*lrflagptr = 2;
	}

	return 0;
}


