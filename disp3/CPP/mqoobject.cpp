#include <stdafx.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <mqoobject.h>
#include <mqoface.h>
#include <mqomaterial.h>

#include <treehandler2.h>
#include <treeelem2.h>
#include <shdhandler.h>
#include <mothandler.h>
#include <motionctrl.h>

#include <extlineio.h>
#include <extline.h>


extern int g_index32bit;


typedef struct tag_latheelem
{
	float height;
	float dist;
} LATHEELEM;


CMQOObject::CMQOObject()
{
	InitParams();
}

CMQOObject::~CMQOObject()
{
	if( pointbuf ){
		free( pointbuf );
		pointbuf = 0;
	}

	if( facebuf ){
		delete [] facebuf;
		facebuf = 0;
	}

	if( colorbuf ){
		free( colorbuf );
		colorbuf = 0;
	}

	if( pointbuf2 ){
		free( pointbuf2 );
		pointbuf2 = 0;
	}

	if( facebuf2 ){
		delete [] facebuf2;
		facebuf2 = 0;
	}

	if( colorbuf2 ){
		free( colorbuf2 );
		colorbuf2 = 0;
	}

	if( connectface ){
		delete [] connectface;
		connectface = 0;
	}

}

void CMQOObject::InitParams()
{
	objectno = -1;
	ZeroMemory( name, 256 );

	patch = 0;
	segment = 1;

	visible = 15;
	locking = 0;

	shading	= 1;
	facet = 59.5f;
	color.r = 1.0f;
	color.g = 1.0f;
	color.b = 1.0f;
	color.a = 1.0f;
	color_type = 0;
	mirror = 0;
	mirror_axis = 1;
	issetmirror_dis = 0;
	mirror_dis = 0.0f;
	lathe = 0;
	lathe_axis = 0;
	lathe_seg = 3;
	vertex = 0;
	//BVertex;
	face = 0;

	pointbuf = 0;
	facebuf = 0;

	vertex2 = 0;
	face2 = 0;
	pointbuf2 = 0;
	facebuf2 = 0;

	hascolor = 0;
	colorbuf = 0;
	colorbuf2 = 0;

	connectnum = 0;
	connectface = 0;


	next = 0;

}


int CMQOObject::SetParams( char* srcchar, int srcleng )
{
	char pat[15][20] = 
	{
		"Object",
		"patch",
		"segment",
		"shading",
		"facet",
		"color_type",
		"color",
		"mirror_axis",
		"mirror_dis",
		"mirror",
		"lathe_axis",
		"lathe_seg",
		"lathe",

		"visible",
		"locking"
	};

	int patno, patleng;
	int pos = 0;
	int cmp;
	int ret;
	int stepnum;
	for( patno = 0; patno < 15; patno++ ){
		while( (pos < srcleng) && ( (*(srcchar + pos) == ' ') || (*(srcchar + pos) == '\t') ) ){
			pos++;
		}
		
		patleng = (int)strlen( pat[patno] );
		cmp = strncmp( pat[patno], srcchar + pos, patleng );
		if( cmp == 0 ){

			pos += patleng;//!!!

			switch( patno ){
			case 0:
				ret = GetName( name, 256, srcchar, pos, srcleng );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetName error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 1:
				ret = GetInt( &patch, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 2:
				ret = GetInt( &segment, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 3:
				ret = GetInt( &shading, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 4:
				ret = GetFloat( &facet, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetFloat error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 5:
				ret = GetInt( &color_type, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 6:
				ret = GetFloat( &color.r, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetFloat error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				pos += stepnum;
				if( pos >= srcleng ){
					DbgOut( "MQOObject : SetParams : GetFloat : pos error !!!" );
					_ASSERT( 0 );
					return 1;
				}


				ret = GetFloat( &color.g, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetFloat error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				pos += stepnum;
				if( pos >= srcleng ){
					DbgOut( "MQOObject : SetParams : GetFloat : pos error !!!" );
					_ASSERT( 0 );
					return 1;
				}

				ret = GetFloat( &color.b, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetFloat error !!!" );
					_ASSERT( 0 );
					return 1;
				}

				break;
			case 7:
				ret = GetInt( &mirror_axis, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 8:
				ret = GetFloat( &mirror_dis, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetFloat error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				issetmirror_dis = 1;//!!!!
				break;
			case 9:
				ret = GetInt( &mirror, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 10:
				ret = GetInt( &lathe_axis, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 11:
				ret = GetInt( &lathe_seg, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 12:
				ret = GetInt( &lathe, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 13:
				ret = GetInt( &visible, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 14:
				ret = GetInt( &locking, srcchar, pos, srcleng, &stepnum );
				if( ret ){
					DbgOut( "MQOObject : SetParams ; GetInt error !!!" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			default:
				_ASSERT( 0 );
				break;
			}

			break;
		}
	}
	


	return 0;

}

int CMQOObject::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
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
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}
int CMQOObject::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
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
		*stepnum = endpos - pos;
		return 1;
	}


	return 0;
}

int CMQOObject::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
{
	int startpos, endpos;
	startpos = pos;

	while( (startpos < srcleng) && 
		( ( *(srcchar + startpos) == ' ' ) || ( *(srcchar + startpos) == '\t' ) || ( *(srcchar + startpos) == '\"' ) ) 
	){
		startpos++;
	}

	endpos = startpos;
	while( (endpos < srcleng) && 
		( ( *(srcchar + endpos) != ' ' ) && ( *(srcchar + endpos) != '\t' ) && (*(srcchar + endpos) != '\r') && (*(srcchar + endpos) != '\n') && (*(srcchar + endpos) != '\"') )
	){
		endpos++;
	}

	if( (endpos - startpos < dstleng) && (endpos - startpos > 0) ){
		strncpy_s( dstchar, dstleng, srcchar + startpos, endpos - startpos );
		*(dstchar + endpos - startpos) = 0;

	}else{
		_ASSERT( 0 );
	}


	return 0;
}

int CMQOObject::SetVertex( int* vertnum, char* srcchar, int srcleng )
{
	//vertex, または、　BVertexを含む文字列を受け取る。
	if( pointbuf ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	char headerchar1[] = "vertex";
	int headerleng1 = (int)strlen( headerchar1 );
	char* find1;
	find1 = strstr( srcchar, headerchar1 );


	char headerchar2[] = "Vector";
	int headerleng2 = (int)strlen( headerchar2 );
	char* find2;
	find2 = strstr( srcchar, headerchar2 );

	int pos;
	if( find1 != NULL ){
		pos = (int)(find1 + headerleng1 - srcchar);
	}else if( find2 != NULL ){
		pos = (int)(find2 + headerleng2 - srcchar);
	}else{

	}
	int stepnum;
	ret = GetInt( &vertex, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	*vertnum = vertex;
	DbgOut( "MQOObject : SetVertex : vertex %d\n", vertex );


	pointbuf = (VEC3F*)malloc( sizeof( VEC3F ) * vertex );
	if( !pointbuf ){
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( pointbuf, sizeof( VEC3F ) * vertex );


	return 0;
}
int CMQOObject::SetPointBuf( unsigned char* srcptr, int srcnum )
{
	if( !pointbuf || (srcnum > vertex) ){
		_ASSERT( 0 );
		return 1;
	}

	MoveMemory( pointbuf, srcptr, srcnum * sizeof( VEC3F ) );

	return 0;

}


int CMQOObject::SetPointBuf( int vertno, char* srcchar, int srcleng )
{
	if( !pointbuf || (vertno >= vertex) ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	VEC3F* dstvec;
	dstvec = pointbuf + vertno;
	int pos, stepnum;
	pos = 0;

	ret = GetFloat( &dstvec->x, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MQOObject : SetPointBuf ; GetFloat error !!!" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;
	if( pos >= srcleng ){
		DbgOut( "MQOObject : SePointBuf : GetFloat : pos error !!!" );
		_ASSERT( 0 );
		return 1;
	}


	ret = GetFloat( &dstvec->y, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MQOObject : SetPointBuf ; GetFloat error !!!" );
		_ASSERT( 0 );
		return 1;
	}
	pos += stepnum;
	if( pos >= srcleng ){
		DbgOut( "MQOObject : SePointBuf : GetFloat : pos error !!!" );
		_ASSERT( 0 );
		return 1;
	}


	ret = GetFloat( &dstvec->z, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MQOObject : SetPointBuf ; GetFloat error !!!" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CMQOObject::SetFace( int* facenum, char* srcchar, int srcleng )
{
	if( facebuf ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	char headerchar[] = "face";
	int headerleng = (int)strlen( headerchar );

	char* find;
	find = strstr( srcchar, headerchar );
	if( find == NULL ){
		_ASSERT( 0 );
		return 1;
	}

	int pos;
	pos = (int)(find + headerleng - srcchar);
	int stepnum;
	ret = GetInt( &face, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	*facenum = face;
	DbgOut( "MQOObject : SetFace : face %d\n", face );

	
	facebuf = new CMQOFace[ face ];
	if( !facebuf ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

int CMQOObject::SetFaceBuf( int faceno, char* srcchar, int srcleng, int setmatno )
{
	if( !facebuf || (faceno >= face) ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	CMQOFace* dstface;
	dstface = facebuf + faceno;

	_ASSERT( dstface );

	ret = dstface->SetParams( srcchar, srcleng, setmatno );
	if( ret ){
		DbgOut( "MQOObject : SetFaceBuf : SetParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//	dstface->materialno += materialoffset;


	return 0;
}

int CMQOObject::Dump()
{
	DbgOut( "Ojbect : %s\n", name );
	DbgOut( "\tpatch %d\n", patch );
	DbgOut( "\tsegment %d\n", segment );

	DbgOut( "\tvisible %d\n", visible );
	DbgOut( "\tlocking %d\n", locking );

	DbgOut( "\tshading %d\n", shading );
	DbgOut( "\tfacet %f\n", facet );
	DbgOut( "\tcolor %f %f %f\n", color.r, color.g, color.b );
	DbgOut( "\tcolor_type %d\n", color_type );
	DbgOut( "\tmirror %d\n", mirror );
	DbgOut( "\tmirror_axis %d\n", mirror_axis );
	DbgOut( "\tmirror_dis %f\n", mirror_dis );
	DbgOut( "\tlathe %d\n", lathe );
	DbgOut( "\tlathe_axis %d\n", lathe_axis );
	DbgOut( "\tlathe_seg %d\n", lathe_seg );

	DbgOut( "\tvertex %d\n", vertex );
	int vertno;
	VEC3F* curvec = pointbuf;
	for( vertno = 0; vertno < vertex; vertno++ ){
		DbgOut( "\t\t%f %f %f\n", curvec->x, curvec->y, curvec->z );
		curvec++;
	}

	if( hascolor && colorbuf ){
		DbgOut( "\tcolor\n" );
		for( vertno = 0; vertno < vertex; vertno++ ){
			DbgOut( "\t\t%d : %f %f %f\n", vertno, colorbuf[vertno].r, colorbuf[vertno].g, colorbuf[vertno].b );
		}
	}

	int ret;
	DbgOut( "\tface %d\n", face );
	int faceno;
	CMQOFace* curface = facebuf;
	for( faceno = 0; faceno < face; faceno++ ){
		ret = curface->Dump();
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		curface++;
	}

	return 0;
}

int CMQOObject::CreateColor()
{
	if( vertex <= 0 ){
		DbgOut( "MQOObject : CreateColor : vertex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	colorbuf = (ARGBF*)malloc( sizeof( ARGBF ) * vertex );
	if( !colorbuf ){
		DbgOut( "MQOObject : CreateColor : colorbuf alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ZeroMemory( colorbuf, sizeof( ARGBF ) * vertex );

	hascolor = 1;

	return 0;
}
int CMQOObject::SetColor( char* srcchar, int srcleng )
{
	int pos, stepnum;
	int ret;
	int pointno;
	
	pos = 0;
	ret = GetInt( &pointno, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MQOObject : SetColor : GetInt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (pointno < 0) || (pointno >= vertex) ){
		DbgOut( "MQOObject : SetColor : pointno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	pos += stepnum;
	DWORD col;
	ret = GetInt( (int*)&col, srcchar, pos, srcleng, &stepnum );
	if( ret ){
		DbgOut( "MQOObject : SetColor : GetInt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DWORD dwr, dwg, dwb;
	dwr = col & 0x000000FF;
	dwg = (col & 0x0000FF00) >> 8;
	dwb = (col & 0x00FF0000) >> 16;

	ARGBF* dstcol = colorbuf + pointno;
	dstcol->r = (float)dwr / 255.0f;
	dstcol->g = (float)dwg / 255.0f;
	dstcol->b = (float)dwb / 255.0f;
	dstcol->a = 1.0f;
	

	//DbgOut( "MQOObject : SetColor : pointno %d, col %d, %f %f %f\n",
	//	pointno, col, dstcol->r, dstcol->g, dstcol->b );


	return 0;
}

int CMQOObject::MakeExtLine( CShdHandler* shandler, int cur_seri, CMQOMaterial* mathead )
{

	int shdtype;

	shdtype = SHDEXTLINE;

	if( !pointbuf || !facebuf )
		return 0;

	int ret;
	CExtLineIO* lineio;
	lineio = new CExtLineIO();
	if( !lineio ){
		DbgOut( "mqoobject : MakeExtLine : lineio alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int pointnum = face * 2;//!!!!!!!!!
	int maxpointnum = face * 2 + 20;
	int linekind = 2;//LINELIST

	CMeshInfo tempinfo;
	ret = lineio->SetMeshInfo( &tempinfo, SHDEXTLINE, pointnum, maxpointnum, linekind );
	if( ret ){
		_ASSERT( 0 );
		delete lineio;
		return 1;
	}
	delete lineio;
//// 色のセット	

	float ambient = 0.5f;
	float specular = 0.1f;

	CShdElem* curelem;
	curelem = (*shandler)( cur_seri );
	_ASSERT( curelem );

	tempinfo.SetMem( 1, MESHI_HASCOLOR );
	CVec3f* midiffuse = tempinfo.GetMaterial( MAT_DIFFUSE );
	CVec3f* miambient = tempinfo.GetMaterial( MAT_AMBIENT );
	CVec3f* mispecular = tempinfo.GetMaterial( MAT_SPECULAR );

	midiffuse->x = color.r;
	midiffuse->y = color.g;
	midiffuse->z = color.b;

	miambient->x = color.r * ambient;
	miambient->y = color.g * ambient;
	miambient->z = color.b * ambient;

	mispecular->x = specular;
	mispecular->y = specular;
	mispecular->z = specular;

	ret = curelem->SetAlpha( color.a );
	_ASSERT( !ret );


DbgOut( "mqoobject : MakeExtLine : Color %f %f %f\r\n", color.r, color.g, color.b );


/////////
	ret = shandler->Init3DObj( cur_seri, &tempinfo );
	if( ret ){
		DbgOut( "mqoobject : MakeExtLine : Init3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

////////
	CExtLine* curline;
	CShdElem* curselem;

	curselem = (*shandler)( cur_seri );
	if( !curselem ){
		_ASSERT( 0 );
		return 1;
	}

	curline = curselem->extline;
	if( !curline ){
		_ASSERT( 0 );
		return 1;
	}

	VEC3F* posptr;
	D3DXVECTOR3 setv;
	int addpnum = 0;
	int face_index;
	int epid;
	for( face_index = 0; face_index < face; face_index++ )
	{
		CMQOFace* curface = facebuf + face_index;
		if( lathe == 0 ){
			switch( curface->pointnum ){
			case 2:
				if( addpnum > pointnum ){
					DbgOut( "mqoobject : MakeExtLine : addpnum error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				posptr = pointbuf + curface->index[0];
				setv.x = posptr->x;
				setv.y = posptr->y;
				setv.z = posptr->z;
				ret = curline->AddExtPoint( -1, 1, &epid );
				if( ret || (epid < 0) ){
					DbgOut( "mqoobject : MakeExtLine : curline AddExtPoint error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = curline->SetExtPointPos( epid, &setv );
				if( ret ){
					DbgOut( "mqoobject : MakeExtLine : curline SetExtPointPos error !!!\n ");
					_ASSERT( 0 );
					return 1;
				}

				addpnum++;
		///////////////////
				if( addpnum > pointnum ){
					DbgOut( "mqoobject : MakeExtLine : addpnum error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				posptr = pointbuf + curface->index[1];
				setv.x = posptr->x;
				setv.y = posptr->y;
				setv.z = posptr->z;

				ret = curline->AddExtPoint( -1, 1, &epid );
				if( ret || (epid < 0) ){
					DbgOut( "mqoobject : MakeExtLine : curline AddExtPoint error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = curline->SetExtPointPos( epid, &setv );
				if( ret ){
					DbgOut( "mqoobject : MakeExtLine : curline SetExtPointPos error !!!\n ");
					_ASSERT( 0 );
					return 1;
				}
				addpnum++;

				break;
			default:
				break;
			}
		}
	}

////////////


	return 0;
}


int CMQOObject::MakePolymesh2( CShdHandler* shandler, int cur_seri, CMQOMaterial* mathead, int groundflag, int adjustuvflag )
{
	CMeshInfo tempinfo;
	int shdtype;

	shdtype = SHDPOLYMESH2;

	if( !pointbuf || !facebuf )
		return 0;

	int ret;
	
	
	if( lathe != 0 ){
		ret = MakeLatheBuf();
		if( ret ){
			DbgOut( "MQOObject : MakePolymesh2 : MakeLatheBuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( mirror != 0 ){
		ret = MakeMirrorBuf();
		if( ret ){
			DbgOut( "MQOObject : MakePolymesh2 : MakeMirrorBuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	

////////
	int face_index;
	CShdElem* curelem;

	// 面と頂点の数を取得
	int face_count;
	int vert_count;
	VEC3F* pointptr;
	CMQOFace* faceptr;
	ARGBF* colorptr;

	if( face2 > 0 ){
		face_count = face2;
		faceptr = facebuf2;
	}else{
		face_count = face;
		faceptr = facebuf;
	}

	if( vertex2 > 0 ){
		vert_count = vertex2;
		pointptr = pointbuf2;
	}else{
		vert_count = vertex;
		pointptr = pointbuf;
	}

	if( colorbuf2 ){
		colorptr = colorbuf2;
	}else{
		colorptr = colorbuf;
	}


	int includereverseface = 0;//!!!!!!!!!

	int* dirtyface;
	dirtyface = (int*)malloc( sizeof( int ) * face_count );
	if( !dirtyface ){
		_ASSERT( 0 );
		return FALSE;
	}
	ZeroMemory( dirtyface, sizeof( int ) * face_count );

	int triangle_count = 0;
	for( face_index = 0; face_index < face_count; face_index++ )
	{
		
		int findsameface = 0;
		int chkindex;
		CMQOFace* chkface;
		for( chkindex = 0; chkindex < face_count; chkindex++ ){
			chkface = faceptr + chkindex;

			if( (face_index != chkindex) && ( *(dirtyface + chkindex) != 0 ) ){
				int issame;
				issame = IsSameFaceIndex( faceptr + face_index, chkface );
				if( issame ){
					findsameface = 1;
					includereverseface++;//!!!!!!!!
					//_ASSERT( 0 );
					break;
				}

				if( issame == 0 ){
					int issamev;
					issamev = IsSameFacePos( faceptr + face_index, chkface, pointptr );
					if( issamev ){
						findsameface = 1;
						includereverseface++;
						break;
					}
				}

			}
		}


		if( findsameface == 0 ){
			switch( (faceptr + face_index)->pointnum ){
			case 2:
				*(dirtyface + face_index) = 2;
				break;
			case 3:
				*(dirtyface + face_index) = 3;
				triangle_count++;
				break;
			case 4: 
				*(dirtyface + face_index) = 4;
				triangle_count+=2; 
				break;
			default:
				*(dirtyface + face_index) = 0;
				break;
			}
		}else{
			*(dirtyface + face_index) = -1;//!!!!!!!!!!!!!
		}
	}
	
	if( g_index32bit ){
		if( triangle_count > 715827882 ){
			::MessageBox( NULL, "一つのオブジェ内の、三角形の数が、\n715827882 個を超えたため、出力できません。", "出力エラー", MB_OK );			
			free( dirtyface );
			return 1;
		}
	}else{
		if( triangle_count > (65535 / 3) ){
			::MessageBox( NULL, "一つのオブジェ内の、三角形の数が、\n65535 / 3 個を超えたため、出力できません。", "出力エラー", MB_OK );			
			free( dirtyface );
			return 1;
		}
	}

	if( triangle_count > 0 ){

		ret = begin_polymesh( triangle_count, shandler, cur_seri );

		curelem = (*shandler)( cur_seri );
		_ASSERT( curelem );

		if( !curelem->polymesh2 ){
			DbgOut( "mqoobject : MakePolymesh2 : polymesh2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		curelem->polymesh2->groundflag = groundflag;//!!!!!!!!!!!!

		curelem->polymesh2->m_anchormaterialno = facebuf->materialno;//!!!!!!!!!!!!!!!!

		curelem->polymesh2->adjustuvflag = adjustuvflag;//!!!!!!!!!!!!!!!!!!!

		if( includereverseface > 0 )
			curelem->clockwise = 3;//!!!!!!!!!!!!!!!!!!!!!!!!!!

		if( lathe != 0 ){
			ret = curelem->SetClockwise( 3 );
			if( ret ){
				DbgOut( "MQOObject : MakePolymesh2 : SetClockwise error !!!\n" );
				return 1;
			}
		}

		int datano = 0;
		int issettexture = 0;
		int issetalpha = 0;
		int settrino = 0;
		int issetcolor = 0;
		int hasvcol = 0;

		ARGBF whitecol;
		whitecol.a = 1.0f;
		whitecol.r = 1.0f;
		whitecol.g = 1.0f;
		whitecol.b = 1.0f;

		for( face_index = 0; face_index < face_count; face_index++ )
		{
			CMQOFace* curface = faceptr + face_index;
			int curmatno;
			CMQOMaterial* curmat;
			ARGBF color;
			float diffuse, ambient, specular, power, emi;
			COORDINATE uv[4];
			curmatno = curface->materialno;
			if( curmatno >= -1 ){
				curmat = GetMaterialFromNo( mathead, curmatno );
				_ASSERT( curmat );
				color = curmat->col;
				diffuse = curmat->dif;
				ambient = curmat->amb;
				specular = curmat->spc;
				power = curmat->power;
				emi = curmat->emi;
				MoveMemory( uv, curface->uv, sizeof( COORDINATE ) * 4 );
				hasvcol = curmat->vcolflag;

				whitecol.a = color.a;
			}else{
				//ダミーマテリアルはmaterialno == -1で作成されているはずなので、ここに来たらエラー
				DbgOut( "mqoobject : makepolymesh2 : materialno error !!!\n" );
				_ASSERT( 0 );
				return 1;

				/***
				curmat = 0;
				color.a = 1.0f;
				color.r = 1.0f;
				color.g = 1.0f;
				color.b = 1.0f;
				diffuse = 1.0f;
				ambient = 0.25f;
				specular = 0.0f;
				//power = 5.0f;
				power = 0.0f;
				emi = 0.0f;
				ZeroMemory( uv, sizeof( COORDINATE ) * 4 );
				hasvcol = 0;

				whitecol.a = 1.0f;
				***/
			}

			if( (issetcolor == 0) && curmat ){
				CMeshInfo* mi = curelem->GetMeshInfo();
				mi->SetMem( 1, MESHI_HASCOLOR );
				CVec3f* midiffuse = mi->GetMaterial( MAT_DIFFUSE );
				CVec3f* miambient = mi->GetMaterial( MAT_AMBIENT );
				CVec3f* mispecular = mi->GetMaterial( MAT_SPECULAR );

				midiffuse->x = color.r;
				midiffuse->y = color.g;
				midiffuse->z = color.b;

				miambient->x = color.r * ambient;
				miambient->y = color.g * ambient;
				miambient->z = color.b * ambient;

				mispecular->x = specular;
				mispecular->y = specular;
				mispecular->z = specular;
					
				issetcolor = 1;

			}

			if( issetalpha == 0 ){
				ret = curelem->SetAlpha( color.a );
				_ASSERT( !ret );
				issetalpha = 1;
			}

			if( (issettexture == 0) && curmat && (curmat->tex[0] != 0) ){
				ret = curelem->SetTexName( curmat->tex );
				_ASSERT( !ret );
				ret = curelem->SetTexRepetition( 1, 1 );
				_ASSERT( !ret );

				ret = curelem->SetTexRule( TEXRULE_MQ );
				_ASSERT( !ret );

				issettexture = 1;

			}

			// 三角形面は１つ、四角形面は２つの面を書き込む
			switch( *(dirtyface + face_index) ){
			case 3:
				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[0])->x, (pointptr + curface->index[0])->y, (pointptr + curface->index[0])->z, curface->index[0] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[1])->x, (pointptr + curface->index[1])->y, (pointptr + curface->index[1])->z, curface->index[1] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[2])->x, (pointptr + curface->index[2])->y, (pointptr + curface->index[2])->z, curface->index[2] );
				datano++;
				_ASSERT( !ret );
	
				ret = curelem->SetPolyMesh2Attrib0( settrino, curmatno );
				_ASSERT( !ret );


				if( colorptr == 0 ){

					if( hasvcol == 0 ){
						ret = curelem->SetPolyMesh2Material( settrino, diffuse, ambient, specular, power, emi, color, shandler->m_scene_ambient, uv, 3 );
						_ASSERT( !ret );

					}else{
						ret = curelem->SetPolyMesh2Material( settrino, diffuse, ambient, specular, power, emi, whitecol, shandler->m_scene_ambient, uv, 3 );
						_ASSERT( !ret );

						if( curface->vcolsetflag ){
							//頂点カラー
							ret = curelem->SetPolyMesh2VCol( datano - 3, curface->col[0] );
							_ASSERT( !ret );

							ret = curelem->SetPolyMesh2VCol( datano - 2, curface->col[1] );
							_ASSERT( !ret );

							ret = curelem->SetPolyMesh2VCol( datano - 1, curface->col[2] );
							_ASSERT( !ret );
						}
					}
					_ASSERT( !ret );
				}else{
					ret = curelem->SetPolyMesh2Color( datano - 3, colorptr + curface->index[0] );
					_ASSERT( !ret );
					ret = curelem->SetPolyMesh2Color( datano - 2, colorptr + curface->index[1] );
					_ASSERT( !ret );
					ret = curelem->SetPolyMesh2Color( datano - 1, colorptr + curface->index[2] );
					_ASSERT( !ret );
				}

				settrino++;

				break;
			case 4:
				// (0-1-2)を頂点インデックスとする三角形の出力
				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[0])->x, (pointptr + curface->index[0])->y, (pointptr + curface->index[0])->z, curface->index[0] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[1])->x, (pointptr + curface->index[1])->y, (pointptr + curface->index[1])->z, curface->index[1] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[2])->x, (pointptr + curface->index[2])->y, (pointptr + curface->index[2])->z, curface->index[2] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Attrib0( settrino, curmatno );
				_ASSERT( !ret );


				if( colorptr == 0 ){
					
					if( hasvcol == 0 ){
						ret = curelem->SetPolyMesh2Material( settrino, diffuse, ambient, specular, power, emi, color, shandler->m_scene_ambient, uv, 3 );
						_ASSERT( !ret );
//DbgOut( "check !!!, mqoobject, SetPolyMesh2Material ambient %f\r\n", ambient );
					}else{
						ret = curelem->SetPolyMesh2Material( settrino, diffuse, ambient, specular, power, emi, whitecol, shandler->m_scene_ambient, uv, 3 );
						_ASSERT( !ret );

						if( curface->vcolsetflag ){
							//頂点カラー
							ret = curelem->SetPolyMesh2VCol( datano - 3, curface->col[0] );
							_ASSERT( !ret );

							ret = curelem->SetPolyMesh2VCol( datano - 2, curface->col[1] );
							_ASSERT( !ret );

							ret = curelem->SetPolyMesh2VCol( datano - 1, curface->col[2] );
							_ASSERT( !ret );

						}
					}
				}else{
					ret = curelem->SetPolyMesh2Color( datano - 3, colorptr + curface->index[0] );
					_ASSERT( !ret );
					ret = curelem->SetPolyMesh2Color( datano - 2, colorptr + curface->index[1] );
					_ASSERT( !ret );
					ret = curelem->SetPolyMesh2Color( datano - 1, colorptr + curface->index[2] );
					_ASSERT( !ret );
				}
					
				settrino++;

				// (0-2-3)を頂点インデックスとする三角形の出力
				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[0])->x, (pointptr + curface->index[0])->y, (pointptr + curface->index[0])->z, curface->index[0] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[2])->x, (pointptr + curface->index[2])->y, (pointptr + curface->index[2])->z, curface->index[2] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Point( datano, (pointptr + curface->index[3])->x, (pointptr + curface->index[3])->y, (pointptr + curface->index[3])->z, curface->index[3] );
				datano++;
				_ASSERT( !ret );

				ret = curelem->SetPolyMesh2Attrib0( settrino, curmatno );
				_ASSERT( !ret );


				if( colorptr == 0 ){
					if( hasvcol == 0 ){
						ret = curelem->SetPolyMesh2Material( settrino, diffuse, ambient, specular, power, emi, color, shandler->m_scene_ambient, uv, 4 );
						_ASSERT( !ret );
//DbgOut( "check !!!, mqoobject, SetPolyMesh2Material ambient %f\r\n", ambient );

					}else{
						ret = curelem->SetPolyMesh2Material( settrino, diffuse, ambient, specular, power, emi, whitecol, shandler->m_scene_ambient, uv, 4 );
						_ASSERT( !ret );

						if( curface->vcolsetflag ){

							//頂点カラー
							ret = curelem->SetPolyMesh2VCol( datano - 3, curface->col[0] );
							_ASSERT( !ret );

							ret = curelem->SetPolyMesh2VCol( datano - 2, curface->col[2] );
							_ASSERT( !ret );

							ret = curelem->SetPolyMesh2VCol( datano - 1, curface->col[3] );
							_ASSERT( !ret );

						}
					}
				}else{
					ret = curelem->SetPolyMesh2Color( datano - 3, colorptr + curface->index[0] );
					_ASSERT( !ret );
					ret = curelem->SetPolyMesh2Color( datano - 2, colorptr + curface->index[2] );
					_ASSERT( !ret );
					ret = curelem->SetPolyMesh2Color( datano - 1, colorptr + curface->index[3] );
					_ASSERT( !ret );
				}

				settrino++;

				break;
			default:
				break;
			}
				
		}
			
		ret = end_polymesh( curelem );
		if( ret ){
			DbgOut( "MQOObject : MakePolymesh2 : end_polymesh error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		



	}

	free( dirtyface );


	return 0;
}

int CMQOObject::begin_polymesh( int facenum, CShdHandler* shandler, int cur_seri ) 
{

	int ret = 0;
	int shdtype = SHDPOLYMESH2;
	CMeshInfo tempinfo;

	ret = tempinfo.ResetParams();
	_ASSERT( !ret );


	//ret += SetMaterials( &tempinfo );
	//_ASSERT( !ret ); 
	ret += tempinfo.SetMem( &shdtype, BASE_TYPE );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( facenum * 3, MESHI_M );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( facenum, MESHI_N );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( 0, MESHI_TOTAL );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( 1, MESHI_MCLOSED );
	_ASSERT( !ret );
	ret += tempinfo.SetMem( 0, MESHI_NCLOSED );
	_ASSERT( !ret );

	ret += shandler->Init3DObj( cur_seri, &tempinfo );
	if( ret ){
		DbgOut( "begin_polymesh_vertex : Init3DObj error !!!\n" );
		_ASSERT( !ret );
		return 1;
	}
	return 0;
}

int CMQOObject::end_polymesh( CShdElem* curelem )
{
	int ret;

	ret = curelem->SetPolyMesh2SamePoint();
	_ASSERT( !ret );



	return ret;
}


int CMQOObject::HasPolygon()
{
	int face2 = 0;
	int face3 = 0;
	int face4 = 0;

	int faceno;

	for( faceno = 0; faceno < face; faceno++ ){
		CMQOFace* curface;
		curface = facebuf + faceno;
		switch( curface->pointnum ){
			case 2:
				face2++;
				break;
			case 3:
				face3++;
				break;
			case 4:
				face4++;
			default:
				break;
		}
	}
	
	if( lathe != 0 ){
		return (face2 + face3 + face4);
	}else{
		return (face3 + face4);
	}
}

int CMQOObject::IsMikoBone()
{
	int cmp;
	char pattern[20] = "bone:";
	int patleng = (int)strlen( pattern );

	cmp = strncmp( name, pattern, patleng );
	if( cmp == 0 )
		return 1;
	else
		return 0;
}

int CMQOObject::HasLine()
{
	int face2 = 0;

	int faceno;

	for( faceno = 0; faceno < face; faceno++ ){
		CMQOFace* curface;
		curface = facebuf + faceno;
		switch( curface->pointnum ){
			case 2:
				face2++;
				break;
			case 3:
			case 4:
			default:
				break;
		}
	}
	
	return face2;
}


int CMQOObject::MakeLatheBuf()
{
	int linenum = 0;
	int faceno;
	CMQOFace* curface;
	for( faceno = 0; faceno < face; faceno++ ){
		curface = facebuf + faceno;
		if( curface->pointnum == 2 ){
			linenum++;
		}
	}

	if( linenum <= 0 ) //!!!!
		return 0;


	int* lineno2faceno;
	lineno2faceno = (int*)malloc( sizeof( int ) * linenum );
	if( !lineno2faceno ){
		DbgOut( "MQOObject : MakeLatheBuf : lineno2faceno alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( lineno2faceno, sizeof( int ) * linenum );

	int lineno = 0;
	for( faceno = 0; faceno < face; faceno++ ){
		curface = facebuf + faceno;
		if( curface->pointnum == 2 ){
			_ASSERT( lineno < linenum );
			*(lineno2faceno + lineno) = faceno; 
			lineno++;
		}
	}

	if( pointbuf2 ){
		_ASSERT( 0 );
		return 1;
	}
	vertex2 = lathe_seg * 2 * linenum;
	pointbuf2 = (VEC3F*)malloc( sizeof( VEC3F ) * vertex2 );
	if( !pointbuf2 ){
		DbgOut( "MQOObject : MakeLatheBuf : pointbuf2 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;			
	}
	ZeroMemory( pointbuf2, sizeof( VEC3F ) * vertex2 );

	if( colorbuf ){
		if( colorbuf2 ){
			_ASSERT( 0 );
			return 1;
		}
		colorbuf2 = (ARGBF*)malloc( sizeof( ARGBF ) * vertex2 );
		if( !colorbuf2 ){
			DbgOut( "MQOObject : MakeLatheBuf : colorbuf2 alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;			
		}
		//ZeroMemory( colorbuf2, sizeof( ARGBF ) * vertex2 );
		MoveMemory( colorbuf2, colorbuf, sizeof( ARGBF ) * vertex );
	}

	if( facebuf2 ){
		_ASSERT( 0 );
		return 1;
	}
	face2 = lathe_seg * linenum;
	facebuf2 = new CMQOFace[ face2 ];
	if( !facebuf2 ){
		DbgOut( "MQOObject : MakeLatheBuf : facebuf2 alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;			
	}
	

	LATHEELEM elem[2];
	float mag, rad;
	int segno;
	rad = 2.0f * PI / lathe_seg;

	for( lineno = 0; lineno < linenum; lineno++ ){
		faceno = *(lineno2faceno + lineno);
		int v0, v1;
		v0 = (facebuf + faceno)->index[0];
		v1 = (facebuf + faceno)->index[1];
		VEC3F* src0 = pointbuf + v0;
		VEC3F* src1 = pointbuf + v1;
		switch( lathe_axis ){
		case 0://X
			elem[0].height = src0->x;
			mag = src0->y * src0->y + src0->z * src0->z;
			if( mag > 0.0f ){
				elem[0].dist = (float)sqrt( mag );
			}else{
				elem[0].dist = 0.0f;
			}

			elem[1].height = src1->x;
			mag = src1->y * src1->y + src1->z * src1->z;
			if( mag > 0.0f ){
				elem[1].dist = (float)sqrt( mag );
			}else{
				elem[1].dist = 0.0f;
			}

			for( segno = 0; segno < lathe_seg; segno++ ){
				VEC3F* dst0 = pointbuf2 + lineno * 2 * lathe_seg + segno;
				VEC3F* dst1 = dst0 + lathe_seg;

				dst0->x = elem[0].height;
				dst0->y = elem[0].dist * (float)cos( rad * segno );
				dst0->z = elem[0].dist * (float)sin( rad * segno );

				dst1->x = elem[1].height;
				dst1->y = elem[1].dist * (float)cos( rad * segno );
				dst1->z = elem[1].dist * (float)sin( rad * segno );

				if( colorbuf2 ){
					ARGBF* dstcol0 = colorbuf2 + lineno * 2 * lathe_seg + segno;
					ARGBF* dstcol1 = dstcol0 + lathe_seg;

					*dstcol0 = *(colorbuf + v0);
					*dstcol1 = *(colorbuf + v1);
				}
			}

			break;
		case 1://Y
			elem[0].height = src0->y;
			mag = src0->x * src0->x + src0->z * src0->z;
			if( mag > 0.0f ){
				elem[0].dist = (float)sqrt( mag );
			}else{
				elem[0].dist = 0.0f;
			}

			elem[1].height = src1->y;
			mag = src1->x * src1->x + src1->z * src1->z;
			if( mag > 0.0f ){
				elem[1].dist = (float)sqrt( mag );
			}else{
				elem[1].dist = 0.0f;
			}

			for( segno = 0; segno < lathe_seg; segno++ ){
				VEC3F* dst0 = pointbuf2 + lineno * 2 * lathe_seg + segno;
				VEC3F* dst1 = dst0 + lathe_seg;

				dst0->x = elem[0].dist * (float)cos( rad * segno );
				dst0->y = elem[0].height;
				dst0->z = elem[0].dist * (float)sin( rad * segno );

				dst1->x = elem[1].dist * (float)cos( rad * segno );
				dst1->y = elem[1].height;
				dst1->z = elem[1].dist * (float)sin( rad * segno );

				if( colorbuf2 ){
					ARGBF* dstcol0 = colorbuf2 + lineno * 2 * lathe_seg + segno;
					ARGBF* dstcol1 = dstcol0 + lathe_seg;

					*dstcol0 = *(colorbuf + v0);
					*dstcol1 = *(colorbuf + v1);
				}
			}
			break;
		case 2://Z
			elem[0].height = src0->z;
			mag = src0->x * src0->x + src0->y * src0->y;
			if( mag > 0.0f ){
				elem[0].dist = (float)sqrt( mag );
			}else{
				elem[0].dist = 0.0f;
			}

			elem[1].height = src1->z;
			mag = src1->x * src1->x + src1->y * src1->y;
			if( mag > 0.0f ){
				elem[1].dist = (float)sqrt( mag );
			}else{
				elem[1].dist = 0.0f;
			}

			for( segno = 0; segno < lathe_seg; segno++ ){
				VEC3F* dst0 = pointbuf2 + lineno * 2 * lathe_seg + segno;
				VEC3F* dst1 = dst0 + lathe_seg;

				dst0->x = elem[0].dist * (float)cos( rad * segno );
				dst0->y = elem[0].dist * (float)sin( rad * segno );
				dst0->z = elem[0].height;

				dst1->x = elem[1].dist * (float)cos( rad * segno );
				dst1->y = elem[1].dist * (float)sin( rad * segno );
				dst1->z = elem[1].height;

				if( colorbuf2 ){
					ARGBF* dstcol0 = colorbuf2 + lineno * 2 * lathe_seg + segno;
					ARGBF* dstcol1 = dstcol0 + lathe_seg;

					*dstcol0 = *(colorbuf + v0);
					*dstcol1 = *(colorbuf + v1);
				}
			}
			break;
		default:
			_ASSERT( 0 );
			break;
		}
	}


	for( lineno = 0; lineno < linenum; lineno++ ){ 
		for( segno = 0; segno < lathe_seg; segno++ ){
			CMQOFace* dstface = facebuf2 + lathe_seg * lineno + segno;
			dstface->pointnum = 4;

			if( segno != lathe_seg - 1 ){
				dstface->index[0] = lineno * 2 * lathe_seg + segno;
				dstface->index[1] = lineno * 2 * lathe_seg + lathe_seg + segno;
				dstface->index[2] = lineno * 2 * lathe_seg + lathe_seg + 1 + segno;
				dstface->index[3] = lineno * 2 * lathe_seg + 1 + segno;
			}else{
				dstface->index[0] = lineno * 2 * lathe_seg + segno;
				dstface->index[1] = lineno * 2 * lathe_seg + lathe_seg + segno;
				dstface->index[2] = lineno * 2 * lathe_seg + lathe_seg;
				dstface->index[3] = lineno * 2 * lathe_seg;
			}
			dstface->hasuv = 0;

		}
	}


	free( lineno2faceno );

	return 0;
}

int CMQOObject::MakeMirrorBuf()
{
	int ret;
	
	if( mirror == 2 ){
		ret = FindConnectFace( 0 );//connectnumのセット
		if( ret ){
			DbgOut( "MQOObject : MakeMirrorBuf : FindConnectFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		if( connectnum > 0 ){
			_ASSERT( connectface == 0 );
			connectface = new CMQOFace[ connectnum ];
			if( !connectface ){
				DbgOut( "MQOObject : MakeMirrorBuf : connectface alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = FindConnectFace( 1 );//connectfaceのセット
			if( ret ){
				DbgOut( "MQOObject : MakeMirrorBuf : FindConnectFace error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}else{
		connectnum = 0;
		connectface = 0;
	}
	

	int doconnect = 0;
	int axis;
	if( axis = (mirror_axis & 0x01) ){
		ret = MakeMirrorPointAndFace( axis, doconnect );
		if( ret ){
			DbgOut( "MQOObject : MakeMirrorBuf : x : error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		doconnect = 1;
	}

	if( axis = (mirror_axis & 0x02) ){
		ret = MakeMirrorPointAndFace( axis, doconnect );
		if( ret ){
			DbgOut( "MQOObject : MakeMirrorBuf : y : error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		doconnect = 1;
	}

	
	if( axis = (mirror_axis & 0x04) ){
		ret = MakeMirrorPointAndFace( axis, doconnect );
		if( ret ){
			DbgOut( "MQOObject : MakeMirrorBuf : z : error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		doconnect = 1;
	}



	return 0;
}

int CMQOObject::MakeMirrorPointAndFace( int axis, int doconnect )
{
	int ret;
	int befvertex, befface;

	if( vertex2 > 0 )
		befvertex = vertex2;
	else
		befvertex = vertex;
	
	if( face2 > 0 )
		befface = face2;
	else
		befface = face;

	vertex2 = befvertex * 2;
	
	if( doconnect == 0 )
		face2 = befface * 2 + connectnum;
	else
		face2 = befface * 2;

	if( pointbuf2 ){
		pointbuf2 = (VEC3F*)realloc( pointbuf2, sizeof( VEC3F ) * vertex2 );
		if( !pointbuf2 ){
			DbgOut( "MQOObject : MakeMirrorPointAndFace : pointbuf2 realloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{
		pointbuf2 = (VEC3F*)malloc( sizeof( VEC3F ) * vertex2 );
		if( !pointbuf2 ){
			DbgOut( "MQOObject : MakeMirrorPointAndFace : pointbuf2 alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( pointbuf2, pointbuf, sizeof( VEC3F ) * befvertex );
	}

	if( colorbuf ){
		if( colorbuf2 ){
			colorbuf2 = (ARGBF*)realloc( colorbuf2, sizeof( ARGBF ) * vertex2 );
			if( !colorbuf2 ){
				DbgOut( "MQOObject : MakeMirrorPointAndFace : colorbuf2 realloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			colorbuf2 = (ARGBF*)malloc( sizeof( ARGBF ) * vertex2 );
			if( !colorbuf2 ){
				DbgOut( "MQOObject : MakeMirrorPointAndFace : colorbuf2 alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			MoveMemory( colorbuf2, colorbuf, sizeof( ARGBF ) * befvertex );
		}
	}

	if( facebuf2 ){
		CMQOFace* newface;
		newface = new CMQOFace[ face2 ];
		if( !newface ){
			DbgOut( "MQOObject : MakeMirrorPointAndFace : newface alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( newface, facebuf2, sizeof( CMQOFace ) * befface );
		delete [] facebuf2;
		facebuf2 = newface;

	}else{
		facebuf2 = new CMQOFace[ face2 ];
		if( !facebuf2 ){
			DbgOut( "MQOObject : MakeMirrorPointAndFace : facebuf2 alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( facebuf2, facebuf, sizeof( CMQOFace ) * befface );
	}


	int vertno;
	VEC3F* srcvec;
	VEC3F* dstvec;
	switch( axis ){
	case 1://X
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srcvec = pointbuf2 + vertno;
			dstvec = pointbuf2 + vertno + befvertex;
			
			dstvec->x = -srcvec->x;
			dstvec->y = srcvec->y;
			dstvec->z = srcvec->z;
		}
		break;
	case 2://Y
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srcvec = pointbuf2 + vertno;
			dstvec = pointbuf2 + vertno + befvertex;
			
			dstvec->x = srcvec->x;
			dstvec->y = -srcvec->y;
			dstvec->z = srcvec->z;
		}
		break;
	case 4://Z
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srcvec = pointbuf2 + vertno;
			dstvec = pointbuf2 + vertno + befvertex;
			
			dstvec->x = srcvec->x;
			dstvec->y = srcvec->y;
			dstvec->z = -srcvec->z;
		}

		break;
	default:
		_ASSERT( 0 );
		break;
	}

	if( colorbuf2 ){
		ARGBF* srccol;
		ARGBF* dstcol;
		for( vertno = 0; vertno < befvertex; vertno++ ){
			srccol = colorbuf2 + vertno;
			dstcol = colorbuf2 + vertno + befvertex;
			
			*dstcol = *srccol;
		}
	}


	int faceno;
	CMQOFace* srcface;
	CMQOFace* dstface;
	for( faceno = 0; faceno < befface; faceno++ ){
		srcface = facebuf2 + faceno;
		dstface = facebuf2 + faceno + befface;

		ret = dstface->SetInvFace( srcface, befvertex );
		if( ret ){
			DbgOut( "MQOObject : MakeMirrorPointAndFace : SetInvFace error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( (doconnect == 0) && connectface ){
		int srcno = 0;
		for( faceno = befface * 2; faceno < face2; faceno++ ){
			srcface = connectface + srcno;
			dstface = facebuf2 + faceno;
			
			dstface->pointnum = 4;
			dstface->materialno = srcface->materialno;
			
			dstface->index[0] = srcface->index[0];//反対向きに描画するようにセット。
			dstface->index[1] = srcface->index[0] + befvertex;
			dstface->index[2] = srcface->index[1] + befvertex;
			dstface->index[3] = srcface->index[1];

			dstface->hasuv = srcface->hasuv;

			dstface->uv[0] = srcface->uv[0];
			dstface->uv[1] = srcface->uv[0];
			dstface->uv[2] = srcface->uv[1];
			dstface->uv[3] = srcface->uv[1];

			dstface->col[0] = srcface->col[0];
			dstface->col[1] = srcface->col[0];
			dstface->col[2] = srcface->col[1];
			dstface->col[3] = srcface->col[1];

			srcno++;
		}
	}


	return 0;
}


int CMQOObject::FindConnectFace( int issetface )
{
	int ret;
	int conno = 0;
	int faceno, chkno;
	int pointnum;
	int findflag[4];
	int findall;
	CMQOFace* curface;
	CMQOFace* chkface;
	for( faceno = 0; faceno < face; faceno++ ){
		curface = facebuf + faceno;
		pointnum = curface->pointnum;
		if( pointnum <= 2 )
			continue;

		ZeroMemory( findflag, sizeof( int ) * 4 );

		findall = 0;
		for( chkno = 0; chkno < face; chkno++ ){
			if( chkno == faceno )
				continue;
			chkface = facebuf + chkno;

			if( chkface->pointnum <= 2 )
				continue;

			ret = curface->CheckSameLine( chkface, findflag );
			if( ret ){
				DbgOut( "MQOObject : FindConnectFace : CheckSameLine error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			int flagsum = 0;
			int sumno;
			for( sumno = 0; sumno < pointnum; sumno++ ){
				flagsum += findflag[sumno];
			}
			if( flagsum == pointnum ){
				findall = 1;
				break;
			}
		}

		if( findall == 0 ){
			int i;
			for( i = 0; i < pointnum; i++ ){
				if( findflag[i] == 0 ){//共有していない辺
					int distok = 1;
					distok = CheckMirrorDis( pointbuf, curface, i, pointnum );
					if( distok ){
						if( issetface ){
							(connectface + conno)->pointnum = 2;
							(connectface + conno)->materialno = curface->materialno;
							(connectface + conno)->hasuv = curface->hasuv;

							(connectface + conno)->index[0] = curface->index[i];
							(connectface + conno)->uv[0] = curface->uv[i];
							if( i != (pointnum - 1) ){
								(connectface + conno)->index[1] = curface->index[i+1];
								(connectface + conno)->uv[1] = curface->uv[i+1];
							}else{
								(connectface + conno)->index[1] = curface->index[0];
								(connectface + conno)->uv[1] = curface->uv[0];
							}

						}
						conno++;
					}
				}
			}

		}

	}

	if( issetface == 0 ){
		connectnum = conno;
	}


	return 0;
}

CMQOMaterial* CMQOObject::GetMaterialFromNo( CMQOMaterial* mathead, int matno )
{
	CMQOMaterial* retmat = 0;
	CMQOMaterial* chkmat = mathead;

	while( chkmat ){
		if( chkmat->materialno == matno ){
			retmat = chkmat;
			break;
		}
		chkmat = chkmat->next;
	}
	
	return retmat;
}

int CMQOObject::CheckMirrorDis( VEC3F* pbuf, CMQOFace* fbuf, int lno, int pnum )
{
	if( issetmirror_dis == 0 ){
		return 1;
	}else{
		int axisx, axisy, axisz;
		axisx = mirror_axis & 0x01;
		axisy = mirror_axis & 0x02;
		axisz = mirror_axis & 0x04;

		int i0, i1;
		i0 = lno;
		if( lno == pnum - 1 ){
			i1 = 0;
		}else{
			i1 = lno + 1;
		}
		VEC3F* v0;
		VEC3F* v1;
		v0 = pbuf + fbuf->index[i0];
		v1 = pbuf + fbuf->index[i1];

		float distx0, distx1;
		int chkx = 1;
		if( axisx ){
			distx0 = v0->x * 2.0f;
			distx1 = v1->x * 2.0f;

			if( (distx0 > mirror_dis) || (distx1 > mirror_dis) ){
				chkx = 0;
			}
		}


		float disty0, disty1;
		int chky = 1;
		if( axisy ){
			disty0 = v0->y * 2.0f;
			disty1 = v1->y * 2.0f;

			if( (disty0 > mirror_dis) || (disty1 > mirror_dis) ){
				chky = 0;
			}
		}


		float distz0, distz1;
		int chkz = 1;
		if( axisz ){
			distz0 = v0->z * 2.0f;
			distz1 = v1->z * 2.0f;

			if( (distz0 > mirror_dis) || (distz1 > mirror_dis) ){
				chkz = 0;
			}
		}


		if( (chkx == 1) && (chky == 1) && (chkz == 1) ){
			return 1;
		}else{
			return 0;
		}

	}

}

int CMQOObject::MultMat( D3DXMATRIX multmat )
{
	VEC3F* curv;
	int vno;

	for( vno = 0; vno < vertex; vno++ ){
		curv = pointbuf + vno;


		D3DXVECTOR3 befv, aftv;
		befv.x = curv->x;
		befv.y = curv->y;
		befv.z = curv->z;

		D3DXVec3TransformCoord( &aftv, &befv, &multmat );
		curv->x = aftv.x;
		curv->y = aftv.y;
		curv->z = aftv.z;

	}

	return 0;

}


int CMQOObject::Shift( D3DXVECTOR3 shiftvec )
{

	VEC3F* curv;
	int vno;

	for( vno = 0; vno < vertex; vno++ ){
		curv = pointbuf + vno;

		curv->x += shiftvec.x;
		curv->y += shiftvec.y;
		curv->z += shiftvec.z;
	}

	return 0;
}



int CMQOObject::Multiple( float multiple )
{
	VEC3F* curv;
	int vno;

	for( vno = 0; vno < vertex; vno++ ){
		curv = pointbuf + vno;

		curv->x *= multiple;
		curv->y *= multiple;
		curv->z *= -multiple;// Zは、反転

	}

	return 0;
}

int CMQOObject::SetMikoBoneIndex3()
{
	int ret;

	int faceno;
	CMQOFace* curface;
	for( faceno = 0; faceno < face; faceno++ ){
		curface = facebuf + faceno;

		if( curface->pointnum == 3 ){
			ret = curface->SetMikoBoneIndex3( pointbuf );
			if( ret ){
				DbgOut( "mqoobject : SetMikoBoneIndex3 : face SetMikoBoneIndex3 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	return 0;
}

int CMQOObject::SetMikoBoneIndex2()
{

	int ret;

	int faceno;
	CMQOFace* curface;

	CMQOFace* findface = 0;
	int findi = -1;

	for( faceno = 0; faceno < face; faceno++ ){
		curface = facebuf + faceno;

		if( curface->pointnum == 2 ){

			int i;
			int lineindex;
			for( i = 0; i < 2; i++ ){
				lineindex = curface->index[i];

				ret = CheckFaceSameChildIndex( curface, lineindex, &findface );
				if( ret ){
					DbgOut( "mqoobject : SetMikoBoneIndex2 : CheckFaceSameChildIndex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				if( findface && (findface->mikobonetype == MIKOBONE_NORMAL) ){

					if( i == 0 ){
						int otherindex = curface->index[1];
						int j;
						for( j = 0; j < findface->pointnum; j++ ){
							if( otherindex == findface->index[j] ){
								//3角内に、線が含まれる場合は、ゴミデータと見なす。
								findface = 0;
								findi = -1;
								break;
							}
						}
					}else{
						int otherindex = curface->index[0];
						int j;
						for( j = 0; j < findface->pointnum; j++ ){
							if( otherindex == findface->index[j] ){
								//3角内に、線が含まれる場合は、ゴミデータと見なす。
								findface = 0;
								findi = -1;
								break;
							}
						}						
					}

					if( findface ){
						findi = i;
						break;
					}

				}

			}


			if( findface && (findi >= 0) ){
				if( findi == 0 ){
					curface->parentindex = curface->index[0];
					curface->childindex = curface->index[1];
				}else{
					curface->parentindex = curface->index[1];
					curface->childindex = curface->index[0];
				}
				curface->mikobonetype = MIKOBONE_FLOAT;

			}else{
				curface->mikobonetype = MIKOBONE_NONE;//!!!!!!!!!!!!!!!!!
			}


		}

	}
	
	return 0;
}


/***
int CMQOObject::SetMikoBoneIndex2()
{

	int ret;

	int faceno;
	CMQOFace* curface;
	for( faceno = 0; faceno < face; faceno++ ){
		curface = facebuf + faceno;

		if( curface->pointnum == 2 ){
			ret = curface->SetMikoBoneIndex2( facebuf, face );
			if( ret ){
				DbgOut( "mqoobject : SetMikoBoneIndex3 : face SetMikoBoneIndex3 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}
	
	return 0;
}
***/

int CMQOObject::GetTopLevelMikoBone( CMQOFace** pptopface, int* topnumptr, int maxnum )
{
	*topnumptr = 0;


	int ret;
	int fno;
	CMQOFace* curface;
	CMQOFace* findface;

	for( fno = 0; fno < face; fno++ ){
		curface = facebuf + fno;

		if( curface->mikobonetype == MIKOBONE_NORMAL ){
			ret = CheckFaceSameChildIndex( curface, curface->parentindex, &findface );
			if( ret ){
				DbgOut( "mqoobject : GetTopLevelMikoBone : CheckFaceSameChildIndex error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		
			if( !findface ){
				if( *topnumptr >= maxnum ){
					DbgOut( "mqoobject : GetTopLevelMikoBone : too many top level bone num error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				*( pptopface + *topnumptr ) = curface;
				(*topnumptr)++;
			}

		}
	}

	return 0;
}


int CMQOObject::CheckFaceSameChildIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface )
{
	*ppfindface = 0;

	int fno;
	CMQOFace* curface;

	for( fno = 0; fno < face; fno++ ){
		curface = facebuf + fno;

		if( (curface->mikobonetype == MIKOBONE_NORMAL) || (curface->mikobonetype == MIKOBONE_FLOAT) ){
			if( (curface->childindex == chkno) && (curface != srcface) ){
				*ppfindface = curface;
				break;
			}
		}

	}

	return 0;
}

int CMQOObject::FindFaceSameParentIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface, int* findnum, int maxnum )
{
	*findnum = 0;

	int fno;
	CMQOFace* curface;

	for( fno = 0; fno < face; fno++ ){
		curface = facebuf + fno;

		if( (curface->mikobonetype == MIKOBONE_NORMAL) || (curface->mikobonetype == MIKOBONE_FLOAT) ){
			if( (chkno == curface->parentindex) && (curface != srcface) ){
				
				if( *findnum >= maxnum ){
					DbgOut( "mqoobject : FindFaceSameParentIndex : maxnum too short error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				*( ppfindface + *findnum ) = curface;
				(*findnum)++;
			}

		}

	}


	return 0;
}

int CMQOObject::InitFaceDirtyFlag()
{
	int fno;
	CMQOFace* curface;
	for( fno = 0; fno < face; fno++ ){
		curface = facebuf + fno;

		curface->dirtyflag = 0;
	}

	return 0;
}


int CMQOObject::SetTreeMikoBone( CMQOFace* srctopface, CMQOMaterial* mathead )
{
	int ret;
	int childno;
	childno = srctopface->childindex;

	srctopface->dirtyflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	

	CMQOFace* findface[MAXBONENUM];
	int findnum = 0;
	ret = FindFaceSameParentIndex( srctopface, childno, findface, &findnum, MAXBONENUM );
	if( ret ){
		DbgOut( "mqoobject : SetTreeMikoBone : FindFaceSameParentIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int dbgcnt = 0;
	int findno;

/***
char* dbgname;

mathead->GetName( srctopface->materialno, &dbgname );
DbgOut( "\r\nmqoobject : SetTreeMikoBone : topface %d, %s  : findnum %d\r\n", srctopface->serialno, dbgname, findnum );
if( srctopface->materialno == 29 ){
	if( srctopface->pointnum == 3 ){
		DbgOut( "\tpointnum %d, index %d %d %d\r\n", srctopface->pointnum, srctopface->index[0], srctopface->index[1], srctopface->index[2] );
	}else if( srctopface->pointnum == 2 ){
		DbgOut( "\tpointnum %d, index %d %d\r\n", srctopface->pointnum, srctopface->index[0], srctopface->index[1] );
	}
}
for( findno = 0; findno < findnum; findno++ ){
	mathead->GetName( findface[findno]->materialno, &dbgname );
	DbgOut( "mqoobject : SetTreeMikoBone : findface %d, %s\r\n", findface[findno]->serialno, dbgname );
	if( findface[findno]->materialno == 29 ){
		if( findface[findno]->pointnum == 3 ){
			DbgOut( "\tpointnum %d, index %d %d %d\r\n", findface[findno]->pointnum, findface[findno]->index[0], findface[findno]->index[1], findface[findno]->index[2] );
		}else if( findface[findno]->pointnum == 2 ){
			DbgOut( "\tpointnum %d, index %d %d\r\n", findface[findno]->pointnum, findface[findno]->index[0], findface[findno]->index[1] );
		}
	}
}
***/


	for( findno = 0; findno < findnum; findno++ ){

		if( findno == 0 ){
			srctopface->child = findface[findno];
		}else{
			CMQOFace* lastbro = srctopface->child;
			CMQOFace* chkface = srctopface->child;
			while( chkface ){

				lastbro = chkface;
				chkface = chkface->brother;
		
				dbgcnt++;

				if( dbgcnt > 10000 ){
					DbgOut( "mqoobject : SetTreeMikoBone : loop count error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}

			if( lastbro ){
				lastbro->brother = findface[findno];
			}
		}

		findface[findno]->parent = srctopface;

	}

///////////////
	for( findno = 0; findno < findnum; findno++ ){

		if( (findface[findno])->dirtyflag == 0 ){
			ret = SetTreeMikoBone( findface[findno], mathead );
			if( ret ){
				DbgOut( "mqoobject : SetTreeMikoBone : find SetTreeMikoBone error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CMQOObject::CheckSameMikoBone()
{

	int fno1, fno2;
	CMQOFace* face1;
	CMQOFace* face2;
	int issame;
	for( fno1 = 0; fno1 < face; fno1++ ){
		face1 = facebuf + fno1;

		for( fno2 = 0; fno2 < face; fno2++ ){
			face2 = facebuf + fno2;

			if( (fno1 != fno2) && ((face1->mikobonetype != MIKOBONE_NONE) && (face1->mikobonetype == face2->mikobonetype))){
				issame = IsSameMikoBone( face1, face2 );
				if( issame ){
					face2->mikobonetype = MIKOBONE_ILLEAGAL;
				}
			}else if( (face1->mikobonetype == MIKOBONE_NORMAL) && (face2->mikobonetype == MIKOBONE_FLOAT) ){
				int includecnt = 0;
				int i, j;
				for( i = 0; i < 2; i++ ){
					for( j = 0; j < 3; j++ ){
						if( face2->index[i] == face1->index[j] ){
							includecnt++;
						}
					}
				}
				if( includecnt >= 2 ){
					face2->mikobonetype = MIKOBONE_ILLEAGAL;
				}
			}
		}

	}

	return 0;
}

int CMQOObject::IsSameFacePos( CMQOFace* face1, CMQOFace* face2, VEC3F* pointptr )
{
	if( face1->pointnum != face2->pointnum )
		return 0;

	int pointnum;
	pointnum = face1->pointnum;

	int samecnt = 0;
	int i, j;
	VEC3F* v1;
	VEC3F* v2;

	for( i = 0; i < pointnum; i++ ){
		v1 = pointptr + face1->index[i];

		for( j = 0; j < pointnum; j++ ){
			v2 = pointptr + face2->index[j];
			
			if( (v1->x == v2->x) && (v1->y == v2->y) && (v1->z == v2->z) ){
				samecnt++;
			}
		}
	}

	if( samecnt >= pointnum )
		return 1;
	else
		return 0;

}



int CMQOObject::IsSameFaceIndex( CMQOFace* face1, CMQOFace* face2 )
{

	if( face1->pointnum != face2->pointnum )
		return 0;


	int pointnum;
	pointnum = face1->pointnum;

	int samecnt = 0;
	int i, j;
	int index1, index2;
	for( i = 0; i < pointnum; i++ ){
		index1 = face1->index[i];

		for( j = 0; j < pointnum; j++ ){
			index2 = face2->index[j];

			if( index1 == index2 ){
				samecnt++;
			}
		}
	}

	if( samecnt >= pointnum )
		return 1;
	else
		return 0;

}

int CMQOObject::IsSameMikoBone( CMQOFace* face1, CMQOFace* face2 )
{
	if( face1->mikobonetype != face2->mikobonetype )
		return 0;

	int sameindex;
	sameindex = IsSameFaceIndex( face1, face2 );

	return sameindex;

}

int CMQOObject::CheckLoopedMikoBoneReq( CMQOMaterial* mathead, CMQOFace* faceptr, int* isloopedptr, int* jointnumptr )
{

	if( (faceptr->mikobonetype == MIKOBONE_NORMAL) || (faceptr->mikobonetype == MIKOBONE_FLOAT) ){
		if( faceptr->dirtyflag == 1 ){
			(*isloopedptr)++;
		}

		faceptr->dirtyflag = 1;
		(*jointnumptr)++;
	}

	/***
	char* dbgname;
	mathead->GetName( faceptr->materialno, &dbgname );
	if( faceptr->pointnum == 3 ){
		int parno;
		if( faceptr->parent )
			parno = faceptr->parent->serialno;
		else
			parno = -1;
		DbgOut( "mqoobject : check : face %d %s, parent %d, index %d %d %d\r\n", 
			faceptr->serialno, dbgname, parno, faceptr->index[0], faceptr->index[1], faceptr->index[2] );
	}else{
		int parno;
		if( faceptr->parent )
			parno = faceptr->parent->serialno;
		else
			parno = -1;
		DbgOut( "mqoobject : check : face %d %s, parent %d, index %d %d\r\n", 
			faceptr->serialno, dbgname, parno, faceptr->index[0], faceptr->index[1] );
	}
	***/


	// error
	if( *isloopedptr > 0 ){
		//DbgOut( "find loop !!!!\r\n" );
		return 0;
	}
	if( *jointnumptr > MAXBONENUM ){
		//DbgOut( "bonenum overflow !!!!\r\n" );
		return 0;
	}
	

////////
	if( faceptr->child ){
		DbgOut( "%d's child : ", faceptr->serialno );
		CheckLoopedMikoBoneReq( mathead, faceptr->child, isloopedptr, jointnumptr );
	}

	if( faceptr->brother ){
		DbgOut( "%d's brother : ", faceptr->serialno );
		CheckLoopedMikoBoneReq( mathead, faceptr->brother, isloopedptr, jointnumptr );
	}

	return 0;
}


int CMQOObject::SetMikoBoneName( CMQOMaterial* mathead )
{
	int ret;
	int fno;
	CMQOFace* curface;
	int nameflag;

	char* nameptr;
	char* convnameptr;
	int lrflag;

	CMQOMaterial* curmat;

	//char tempname[1024];

	for( fno = 0; fno < face; fno++ ){
		curface = facebuf + fno;

		if( curface->mikobonetype == MIKOBONE_NORMAL ){
			ret = CheckMaterialSameName( curface->materialno, mathead, &nameflag );
			if( ret ){
				DbgOut( "mqoobject : SetMikoBoneName : CheckMaterialSameName error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			

			mathead->GetName( curface->materialno, &nameptr );

			switch( nameflag ){
			case 0:
				//重複無し
				ret = curface->SetMikoBoneName( nameptr, 0 );
				if( ret ){
					DbgOut( "mqoobject : SetMikoBoneName : face SetMikoBoneName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 1:
				//[]のＬＲ
				ret = curface->CheckLRFlag( pointbuf, &lrflag );
				if( ret ){
					DbgOut( "mqoobject : SetMikoBoneName : face CheckLRFlag error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				ret = curface->SetMikoBoneName( nameptr, lrflag );
				if( ret ){
					DbgOut( "mqoobject : SetMikoBoneName : face SetMikoBoneName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				break;
			case 2:
				//重複有り、[]以外
				curmat = GetMaterialFromNo( mathead, curface->materialno );
				if( !curmat ){
					DbgOut( "mqoobject : SetMikoBoneName : GetMaterialFromNo error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = curmat->AddConvName( &convnameptr );
				if( ret || !convnameptr ){
					DbgOut( "mqoobject : SetMikoBoneName : curmat AddConvName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ret = curface->SetMikoBoneName( convnameptr, 0 );
				if( ret ){
					DbgOut( "mqoobject : SetMikoBoneName : face SetMikoBoneName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				break;
			case 3:
				DbgOut( "mqoobject : SetMikoBoneName : unknown nameflag error !!!\n" );
				_ASSERT( 0 );
				return 1;
				break;
			}

		}
		
		//else if( curface->mikobonetype == MIKOBONE_FLOAT ){
		//	sprintf( tempname, "float_%d", curface->serialno );
//
//			ret = curface->SetMikoBoneName( tempname, 0 );
//			if( ret ){
//				DbgOut( "mqoobject : SetMikoBoneName : face SetMikoBoneName error !!!\n" );
//				_ASSERT( 0 );
//				return 1;
//			}
//
//		}

	}

	return 0;
}

int CMQOObject::SetMikoFloatBoneName()
{

	int ret;
	int fno;
	CMQOFace* curface;

	char tempname[1024];

	for( fno = 0; fno < face; fno++ ){
		curface = facebuf + fno;

		if( curface->mikobonetype == MIKOBONE_FLOAT ){
			CMQOFace* chilface;
			chilface = curface->child;

			if( chilface ){
				sprintf_s( tempname, 1024, "FLOAT_%s", chilface->bonename );

				ret = curface->SetMikoBoneName( tempname, 0 );
				if( ret ){
					DbgOut( "mqoobject : SetMikoFloatBoneName : face SetMikoBoneName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

			}else{
				sprintf_s( tempname, 1024, "FLOAT_%d", curface->serialno );

				ret = curface->SetMikoBoneName( tempname, 0 );
				if( ret ){
					DbgOut( "mqoobject : SetMikoFloatBoneName : face SetMikoBoneName error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}


	return 0;
}




int CMQOObject::CheckMaterialSameName( int srcmatno, CMQOMaterial* mathead, int* nameflag )
{
	int fno;
	CMQOFace* curface;

	int samecnt = 0;
	int lrcnt = 0;

	char* nameptr;
	int leng;
	int cmp;

	for( fno = 0; fno < face; fno++ ){
		curface = facebuf + fno;

		if( curface->mikobonetype == MIKOBONE_NORMAL ){
			if( curface->materialno == srcmatno ){
				samecnt++;

				mathead->GetName( srcmatno, &nameptr );
				if( !nameptr ){
					DbgOut( "mqoobject : CheckMaterkalSameName : material %d, name NULL error !!!\n", srcmatno );
					_ASSERT( 0 );
					return 1;
				}

				leng = (int)strlen( nameptr );
				if( leng > 2 ){
					cmp = strcmp( nameptr + leng - 2, "[]" );
					if( cmp == 0 ){
						lrcnt++;
					}
				}

			}
		}
	}

	if( samecnt == 1 ){
		*nameflag = 0;
	}else if( (samecnt == 2) && (lrcnt == 2) ){
		*nameflag = 1;
	}else if( samecnt >= 2 ){
		*nameflag = 2;
	}else{
		*nameflag = 3;
	}

	return 0;
}


void CMQOObject::DumpMikoBoneReq( CMQOFace* srcface, int depth )
{

	if( (srcface->mikobonetype == MIKOBONE_NORMAL) || (srcface->mikobonetype == MIKOBONE_FLOAT) ){
		int dcnt;
		for( dcnt = 0; dcnt < depth; dcnt++ ){
			DbgOut( "\t" );
		}

		DbgOut( "%s\r\n", srcface->bonename );

	}

//////////
	if( srcface->child ){
		DumpMikoBoneReq( srcface->child, depth + 1 );
	}

	if( srcface->brother ){
		DumpMikoBoneReq( srcface->brother, depth );
	}

}

int CMQOObject::GetParentLock( CMQOFace* srcface, CVec3f* locptr )
{
	VEC3F* posptr;
	posptr = pointbuf + srcface->parentindex;

	locptr->x = posptr->x;
	locptr->y = posptr->y;
	locptr->z = posptr->z;

	return 0;
}
int CMQOObject::GetChildLock( CMQOFace* srcface, CVec3f* locptr )
{
	VEC3F* posptr;
	posptr = pointbuf + srcface->childindex;

	locptr->x = posptr->x;
	locptr->y = posptr->y;
	locptr->z = posptr->z;

	return 0;
}

int CMQOObject::GetHLock( CMQOFace* srcface, CVec3f* locptr )
{
	VEC3F* posptr;
	if( srcface->hindex >= 0 ){
		posptr = pointbuf + srcface->hindex;

		locptr->x = posptr->x;
		locptr->y = posptr->y;
		locptr->z = posptr->z;
	}else{
		locptr->x = 0.0f;
		locptr->y = 0.0f;
		locptr->z = 0.0f;
	}
	return 0;
}
