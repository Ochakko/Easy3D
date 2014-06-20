#include <stdafx.h>

//02/2/7	serialno はWriteSigFile時に、書き出し順に割り当てる。
//02/3/11	serialnoの仕様変更。serialnoは、shdelemのものをそのまま使う。

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

#include <sigfile.h>
#include <mqomaterial.h>

#include <morph.h>
#include <Panda.h>


// global
extern int g_toon1matcnt;



///// sigheader の　flags
#define FLAGS_NOSKINNING	0x01
#define FLAGS_ORGNOBUF		0x02	//先頭のsigheader(startheader)と、パーツごとのsigheaderに記録する。
#define FLAGS_NOTRANSIK		0x04
#define FLAGS_BONETYPE		0x08
#define FLAGS_ANCHORFLAG	0x10
#define FLAGS_PMUVBUF		0x20
#define FLAGS_MIKOBLEND		0x40
#define FLAGS_INFELEM		0x80


///// sigheader の flags2
#define FLAGS2_ORGNORMAL	0x01
#define FLAGS2_VCOL			0x02
#define FLAGS2_OLD2OPT		0x04
#define FLAGS2_SMOOTHBUF	0x08
#define FLAGS2_TOON1		0x10
#define FLAGS2_EDGE0		0x20
#define FLAGS2_MQOMATERIAL	0x40
#define FLAGS2_BONELIM01	0x80

//sigheaderのflags3
#define FLAGS3_IKSKIP		0x01
#define FLAGS3_STANDARD		0x02
#define FLAGS3_INVISIBLEFLAG	0x04

//extern func
//#if !plugin
//	extern int LoadShdData( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, LPCTSTR fname );
//	extern void FreeShdLexBuff();
//#endif
///////////////



CSigFile::CSigFile()
{
	hfile = 0;
	lpth = 0;
	lpsh = 0;
	lpmh = 0;

	InitLoadParams();

}
CSigFile::~CSigFile()
{

}

int CSigFile::WriteSigFile2Buf( char* buf, int bufsize, int* writesizeptr, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh )
{
	int ret = 0;
	*writesizeptr = 0;
	
	if( bufsize > 0 ){
		m_writemode = SIGWRITEMODE_BUF;
	}else{
		m_writemode = SIGWRITEMODE_BUFSIZE;
	}

	m_buf.buf = buf;
	m_buf.bufsize = bufsize;
	m_buf.pos = 0;


	if( !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "sigfile : WriteSigFile2Buf : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;


	ret = WriteSigFile_aft();
	if( ret ){
		DbgOut( "sigfile : WriteSigFile2Buf : WriteSigFile_aft error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto write2bufexit;
	}

	goto write2bufexit;

write2bufexit:

	*writesizeptr = m_buf.pos;
	m_buf.buf = 0;
	m_buf.bufsize = 0;

	return ret;
}




int CSigFile::WriteSigFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh )
{
	int ret = 0;

	m_writemode = SIGWRITEMODE_FILE;


	if( !filename )
		return 1;

	if( !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "sigfile : WriteSigFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;

DbgOut( "sigfile : WriteSigFile : CreateFile\n" );

	hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		ret = 1;
		goto writesigexit;
	}	

	ret = WriteSigFile_aft();
	if( ret ){
		DbgOut( "sigfile : WriteSigFile : WriteSigFile_aft error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto writesigexit;
	}
	
	goto writesigexit;

writesigexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		FlushFileBuffers( hfile );
		SetEndOfFile( hfile );
		CloseHandle( hfile );
		hfile = 0;
	}
	return ret;

}

int CSigFile::WriteSigFile_aft()
{

	CShdElem* rootselem = 0;
	//CMotCtrl* mctrl = 0;
	int ret = 0;
	int errorflag = 0;
	int no_pm2orgno = 0;


	rootselem = (*lpsh)( 0 );
	if( !rootselem ){
		DbgOut( "sigfile : WriteSigFile : rootselem NULL error !!!\n" );
		return 1;
	}

	//ret = lpsh->CheckOrgNoBuf( &no_pm2orgno );
	//if( ret ){
	//	DbgOut( "sigfile : WriteSigFile : shandler : CheckOrgNoBuf error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

	writemagicno = SIGFILEMAGICNO_14;//!!!!!!!!!!!!!!!!

	SIGHEADER startheader;
	ZeroMemory( &startheader, sizeof( SIGHEADER ) );
	startheader.serialno = writemagicno;
	startheader.type = -1;
	startheader.aspect = lpsh->m_aspect;
	
	if( lpsh->m_im2enableflag == 0 ){
		startheader.flags &= ~FLAGS_ORGNOBUF;
	}else{
		startheader.flags |= FLAGS_ORGNOBUF;
	}
	
	if( lpsh->m_bonetype == BONETYPE_RDB2 ){
		startheader.flags &= ~FLAGS_BONETYPE;
	}else{
		startheader.flags |= FLAGS_BONETYPE;
	}

	if( lpsh->m_mikoblendtype == MIKOBLEND_SKINNING ){
		startheader.flags &= ~FLAGS_MIKOBLEND;
	}else{
		startheader.flags |= FLAGS_MIKOBLEND;
	}

	startheader.flags2 |= FLAGS2_MQOMATERIAL;

	if( lpmh->m_standard == 1 ){
		startheader.flags3 |= FLAGS3_STANDARD;
	}else{
		startheader.flags3 &= ~FLAGS3_STANDARD;
	}

	startheader.flags3 &= ~FLAGS3_INVISIBLEFLAG;

	ret = WriteSigHeader( startheader );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
/// scene_ambient, shader, overflowを書き出す（MAGICNO_4以降）
	ret = WriteRGBAData( &lpsh->m_scene_ambient, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = WriteIntData( &lpsh->m_shader, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = WriteIntData( &lpsh->m_overflow, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	//MQOMaterialを書き出す
	ret = WriteMQOMaterial();
	if( ret ){
		DbgOut( "sigfile : WriteSigFile_aft : WriteMQOMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	m_writeno = 0;

//DbgOut( "sigfile : WriteSigFile : start WriteShdReq\n" );

	WriteShdReq( rootselem, &errorflag );	
	if( errorflag ){
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "sigfile : WriteSigFile : end WriteShdReq\n" );

	
	SIGHEADER endheader;
	ZeroMemory( &endheader, sizeof( SIGHEADER ) );
	endheader.serialno = -1;
	endheader.type = -1;
	ret = WriteSigHeader( endheader );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



void CSigFile::WriteShdReq( CShdElem* selem, int* errorflag )
{
	CShdElem* child = 0;
	CShdElem* brother = 0;

	int ret;
	ret = WriteShdElem( selem );
	if( ret )
		(*errorflag)++;

	child = selem->child;
	if( child ){
		WriteShdReq( child, errorflag );
	}

	brother = selem->brother;
	if( brother ){
		WriteShdReq( brother, errorflag );
	}

}

int CSigFile::WriteShdElem( CShdElem* selem )
{
	SIGHEADER sigheader;
	CTreeElem2* telem = 0;
	int serialno;
	int nameleng;
	int ret = 0;
	CMotionCtrl* mctrl = 0;

	serialno = selem->serialno;
	telem = (*lpth)( serialno );
	mctrl = (*lpmh)( serialno );

	ZeroMemory( &sigheader, sizeof( SIGHEADER ) );

//!!!!!!!!!!!	
	sigheader.serialno = serialno;
	//sigheader.serialno = m_writeno;
	m_writeno++;

	sigheader.type = selem->type;
	
	nameleng = (int)strlen( telem->name );
	if( nameleng >= 256 ){
		DbgOut( "sigfile : WriteShdElem : name : nameleng too large %s!!!", telem->name );
		nameleng = 255;
	}
	strncpy_s( sigheader.name, 256, telem->name, nameleng );

	sigheader.brono = telem->brono;
	sigheader.depth = selem->depth;
	sigheader.clockwise = selem->clockwise;
	sigheader.bdivu = selem->bdivU;
	sigheader.bdivv = selem->bdivV;

	if( selem->texname ){
		nameleng = (int)strlen( selem->texname );
		if( nameleng >= 256 ){
			DbgOut( "sigfile : WriteShdElem : texname : nameleng too large %s!!!", selem->texname );
			nameleng = 255;
		}
		strncpy_s( sigheader.texname, 256, selem->texname, nameleng );
	}else{
		strcpy_s( sigheader.texname, 256, "0" );
	}

	sigheader.texrepx = selem->texrepx;
	sigheader.texrepy = selem->texrepy;
	//sigheader.transparent = 0; // !!!!!!!!!!!!!!
	//sigheader.uanime = 0.0f;// !!!!!!!!!!!!!!
	//sigheader.vanime = 0.0f;// !!!!!!!!!!!!!!
	sigheader.transparent = selem->transparent;
	sigheader.uanime = selem->uanime;
	sigheader.vanime = selem->vanime;
	sigheader.alpha = selem->alpha;

	sigheader.hasmotion = mctrl->hasmotion;

	sigheader.texrule = selem->texrule;
	sigheader.notuse = selem->notuse;

	//sigheader.flags = 0;
	if( selem->noskinning == 1 ){
		sigheader.flags |= FLAGS_NOSKINNING;
	}else{
		sigheader.flags &= ~FLAGS_NOSKINNING;
	}

	if( selem->m_anchorflag != 0 ){
		sigheader.flags |= FLAGS_ANCHORFLAG;
	}else{
		sigheader.flags &= ~FLAGS_ANCHORFLAG;
	}
	sigheader.mikodef = selem->m_mikodef;

	if( selem->facet != 180.0f ){
		sigheader.facetm180 = selem->facet - 180.0f;
	}else{
		sigheader.facetm180 = 0.0f;
	}

	sigheader.flags2 |= FLAGS2_ORGNORMAL;//infscopeのためにも、全てのelemのヘッダーにセットしておく。
	sigheader.flags2 |= FLAGS2_VCOL;
	sigheader.flags2 |= FLAGS2_OLD2OPT;
	sigheader.flags2 |= FLAGS2_SMOOTHBUF;
	sigheader.flags2 |= FLAGS2_MQOMATERIAL;
	sigheader.flags2 |= FLAGS2_BONELIM01;

	if( selem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2ptr;
		pm2ptr = selem->polymesh2;

		if( !pm2ptr ){
			DbgOut( "sigfile : WriteShdElem : pm2 not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( pm2ptr->orgnobuf ){
			sigheader.flags |= FLAGS_ORGNOBUF;
		}else{
			sigheader.flags &= ~FLAGS_ORGNOBUF;
		}

		if( pm2ptr->m_toon1 ){
			sigheader.flags2 |= FLAGS2_TOON1;
			sigheader.flags2 |= FLAGS2_EDGE0;
		}else{
			sigheader.flags2 &= ~FLAGS2_TOON1;
			sigheader.flags2 &= ~FLAGS2_EDGE0;
		}

	}
	if( selem->type == SHDPOLYMESH ){
		CPolyMesh* pmptr;
		pmptr = selem->polymesh;

		if( !pmptr ){
			DbgOut( "sigfile : WriteShdElem : pm not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( pmptr->orgnobuf ){
			sigheader.flags |= FLAGS_ORGNOBUF;
		}else{
			sigheader.flags &= ~FLAGS_ORGNOBUF;
		}

		if( pmptr->uvbuf ){
			sigheader.flags |= FLAGS_PMUVBUF;
		}else{
			sigheader.flags &= ~FLAGS_PMUVBUF;
		}

		if( pmptr->m_toon1 ){
			sigheader.flags2 |= FLAGS2_TOON1;
			sigheader.flags2 |= FLAGS2_EDGE0;
		}else{
			sigheader.flags2 &= ~FLAGS2_TOON1;
			sigheader.flags2 &= ~FLAGS2_EDGE0;
		}

	}

	if( selem->type != SHDINFSCOPE ){
		sigheader.dispswitchno = selem->dispswitchno;
	}else{
		sigheader.dispswitchno = selem->scopenum;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	if( selem->m_notransik ){
		sigheader.flags |= FLAGS_NOTRANSIK;
	}else{
		sigheader.flags &= ~FLAGS_NOTRANSIK;
	}
	if( selem->m_ikskip ){
		sigheader.flags3 |= FLAGS3_IKSKIP;
	}else{
		sigheader.flags3 &= ~FLAGS3_IKSKIP;
	}

	if( selem->invisibleflag != 0 ){
		sigheader.flags3 |= FLAGS3_INVISIBLEFLAG;
	}else{
		sigheader.flags3 &= ~FLAGS3_INVISIBLEFLAG;
	}

	sigheader.flags |= FLAGS_INFELEM;//!!!!!


	unsigned char exttexnum, exttexmode, exttexrep, exttexstep;
	exttexnum = selem->exttexnum;
	exttexmode = selem->exttexmode;
	exttexrep = selem->exttexrep;
	exttexstep = selem->exttexstep;
	sigheader.exttexmem = (exttexnum << 24) | (exttexmode << 16) | (exttexrep << 8) | (exttexstep);


	int* templist = 0;
	int templeng = 0;

	if( selem->influencebonenum > 0 ){
		templist = (int*)malloc( sizeof( int ) * selem->influencebonenum );
		ZeroMemory( templist, sizeof( int ) * selem->influencebonenum );

		int listno;
		for( listno = 0; listno < selem->influencebonenum; listno++ ){
			int serino = *( selem->influencebone + listno ); 
			CMotionCtrl* mcptr = (*lpmh)( serino );
			if( !mcptr )
				continue;
			
			if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
				CShdElem* chkelem = (*lpsh)( serino );
				CPart* partptr = chkelem->part;
				if( partptr && (partptr->bonenum >= 1) ){
					
					int bno = mcptr->boneno;
					if( (bno >= 0) && (bno < lpsh->s2shd_leng) ){
						*(templist + templeng) = bno;
						//DbgOut( "SigFile : WriteShdElem : list%d - seri %d - bno %d\n", templeng, serino, bno );
						templeng++;
					}
				}
			}
		}
	}
	sigheader.influencenum = templeng;//!!!!!!!!!!!!

	////////
	int* tempiglist = 0;
	int tempigleng = 0;

	if( selem->ignorebonenum > 0 ){
		tempiglist = (int*)malloc( sizeof( int ) * selem->ignorebonenum );
		ZeroMemory( tempiglist, sizeof( int ) * selem->ignorebonenum );

		int listno;
		for( listno = 0; listno < selem->ignorebonenum; listno++ ){
			int serino = *( selem->ignorebone + listno ); 
			CMotionCtrl* mcptr = (*lpmh)( serino );
			if( !mcptr )
				continue;
			
			if( mcptr->IsJoint() && (mcptr->type != SHDMORPH) ){
				CShdElem* chkelem = (*lpsh)( serino );
				CPart* partptr = chkelem->part;
				if( partptr && (partptr->bonenum >= 1) ){
					
					int bno = mcptr->boneno;
					if( (bno >= 0) && (bno < lpsh->s2shd_leng) ){
						*(tempiglist + tempigleng) = bno;
						//DbgOut( "SigFile : WriteShdElem : list%d - seri %d - bno %d\n", tempigleng, serino, bno );
						tempigleng++;
					}
				}
			}
		}
	}
	sigheader.ignorenum = tempigleng;//!!!!!!!!!!!!

	sigheader.userint1 = selem->m_userint1;

	sigheader.iklevel = selem->m_iklevel;

	///////
	ret = WriteSigHeader( sigheader );
	if( ret ){
		_ASSERT( 0 );
		return ret;
	}

	if( templeng > 0 ){
		ret = WriteIntData( templist, templeng );
		if( ret ){
			_ASSERT( 0 );
			return ret;
		}
	}
	if( tempigleng > 0 ){
		ret = WriteIntData( tempiglist, tempigleng );
		if( ret ){
			_ASSERT( 0 );
			return ret;
		}
	}

	if( templist )
		free( templist );
	if( tempiglist )
		free( tempiglist );

	if( exttexnum > 0 ){
		ret = WriteExtTexName( exttexnum, selem->exttexname );
		if( ret ){
			_ASSERT( 0 );
			return ret;
		}
	}

	switch( sigheader.type )
	{
		case SHDROTATE:
		case SHDSLIDER:
		case SHDSCALE:
		case SHDUNISCALE:
		case SHDPATH:		
		case SHDPART:
		case SHDBALLJOINT:
			ret = WritePart( selem );
			break;
		case SHDMORPH:
			ret = WriteMorph( selem );
			break;
		case SHDPOLYLINE:
			ret = WritePolyLine( selem );
			break;
		case SHDPOLYGON:
			ret = WritePolygon( selem );
			break;
		case SHDSPHERE:
			ret = WriteSphere( selem );
			break;
		case SHDBEZIERSURF:
			ret = WriteBezMesh( selem );
			break;
		case SHDMESHES:
			ret = WriteRevolved( selem );
			break;
		case SHDDISK:
			ret = WriteDisk( selem );
			break;
		case SHDBEZLINE:	// CBezLine
			ret = WriteBezLine( selem );
			break;
		case SHDEXTRUDE:
			ret = WriteExtrude( selem );
			break;
		case SHDPOLYMESH:
			ret = WritePolyMesh( selem->polymesh, 0 );
			break;
		case SHDPOLYMESH2:
			ret = WritePolyMesh2( selem->polymesh2, 0 );
			break;
		case SHDINFSCOPE:
			ret = WriteInfScope( selem );
			break;
		case SHDTYPENONE:
		case SHDTYPEERROR:
		case SHDTYPEMAX:
			// error
			ret = 1;
			break;

		case SHDGROUNDDATA:
			ret = WriteGroundData( selem );
			break;

		case SHDDESTROYED:
		default:
			// 未対応
			ret = 0;
			break;
	}

	_ASSERT( !ret );
	return ret;
}

int CSigFile::WriteExtTexName( int exttexnum, char** exttexname )
{
	int ret, texno;

	if( exttexnum > MAXEXTTEXNUM ){
		DbgOut( "SigFile : WriteExtTexName : exttexnum error !!!\n" );
		_ASSERT( 0 );
		exttexnum = MAXEXTTEXNUM;//!!!!
	}

	for( texno = 0; texno < exttexnum; texno++ ){
		char tempname[256];
		ZeroMemory( tempname, 256 );
		int namelen, cplen;
		namelen = (int)strlen( *( exttexname + texno ) );
		if( namelen >= 256 ){
			DbgOut( "SigFile : WriteExtTexName : namelen error !!! %s\n", *(exttexname + texno) );
			_ASSERT( 0 );
		}

		cplen = min( namelen, 255 );
		strncpy_s( tempname, 256, *( exttexname + texno ), cplen );
		
		ret = WriteUCharData( (unsigned char*)tempname, 256 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CSigFile::WriteSigHeader( SIGHEADER sigheader )
{
	unsigned long wleng, writeleng;

	wleng = sizeof( SIGHEADER );

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:

		WriteFile( hfile, &sigheader, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteSigHeader : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, &sigheader, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:

		m_buf.pos += wleng;

		break;
	}
	
	return 0;
}
int CSigFile::WriteMIHeader( CMeshInfo* srcmi )
{
	MIHEADER mih;
	unsigned long wleng, writeleng;
	CVec3f* diffuse;
	CVec3f* specular;
	CVec3f* ambient;

	ZeroMemory( &mih, sizeof( MIHEADER ) );
	mih.type = srcmi->type;
	mih.m = srcmi->m;
	mih.n = srcmi->n;
	mih.total = srcmi->total;
	mih.mclosed = srcmi->mclosed;
	mih.nclosed = srcmi->nclosed;
	mih.skip = srcmi->skip;
	mih.hascolor = srcmi->HasColor();
	diffuse = srcmi->GetMaterial( MAT_DIFFUSE );
	specular = srcmi->GetMaterial( MAT_SPECULAR );
	ambient = srcmi->GetMaterial( MAT_AMBIENT );
	mih.diffuse.x = diffuse->x;
	mih.diffuse.y = diffuse->y;
	mih.diffuse.z = diffuse->z;
	mih.specular.x = specular->x;
	mih.specular.y = specular->y;
	mih.specular.z = specular->z;
	mih.ambient.x = ambient->x;
	mih.ambient.y = ambient->y;
	mih.ambient.z = ambient->z;

	wleng = sizeof( MIHEADER );

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:

		WriteFile( hfile, &mih, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteMIHeader : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, &mih, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}


	
	return 0;
}

int CSigFile::WriteVecLine( CVecLine* srcvl )
{
	CMeshInfo* mi;
	VEC3F* vec;
	int vnum;
	int ret;

	mi = srcvl->meshinfo;
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	vnum = mi->total;
	vec = srcvl->vec;
	ret = WriteVecData( vec, vnum );
	_ASSERT( !ret );

	return ret;
}

int CSigFile::WriteDWData( DWORD* srcdw, int dwnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcdw );

	wleng = sizeof( DWORD ) * dwnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcdw, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "sigfile : WriteDWData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteDWData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, srcdw, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}


int CSigFile::WriteRGBAData( ARGBF* srcargb, int colnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcargb );

	wleng = sizeof( ARGBF ) * colnum;


	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcargb, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "sigfile : WriteARGBData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteRGBAData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, srcargb, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}
	
	return 0;
}


int CSigFile::WriteVecData( VEC3F* srcvec, int vnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcvec );

	wleng = sizeof( VEC3F ) * vnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:

		WriteFile( hfile, srcvec, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "sigfile : WriteVecData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteVecData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, srcvec, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}


	
	return 0;
}

int CSigFile::WriteUCharData( unsigned char* srcuchar, int num )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcuchar );

	wleng = num;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcuchar, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "sigfile : WriteUCharData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteUCharData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, srcuchar, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}
	
	return 0;
}

int CSigFile::WriteFloatData( float* srcfloat, int flnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcfloat );

	wleng = sizeof( float ) * flnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcfloat, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "sigfile : WriteFloatData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteFloatData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, srcfloat, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}
	
	return 0;
}

int CSigFile::WriteIntData( int* srcint, int intnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcint );

	wleng = sizeof( int ) * intnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcint, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "sigfile : WriteIntData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteIntData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, srcint, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}
	

	
	return 0;
}

int CSigFile::WriteMatData( CMatrix2* srcmat, int matnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcmat );

	wleng = sizeof( CMatrix2 ) * matnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcmat, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteMatData : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, srcmat, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	return 0;
}

int CSigFile::WriteMorph( CShdElem* selem )
{
	CMorph* morph = selem->morph;
	CMeshInfo* mi;
	int ret;

	_ASSERT( morph );
	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	//magicno11
	ret = WriteIntData( &(morph->m_baseelem->serialno), 1 );
	_ASSERT( !ret );

	ret = WriteIntData( &(morph->m_boneelem->serialno), 1 );
	_ASSERT( !ret );

	ret = WriteIntData( &(morph->m_targetnum), 1 );
	_ASSERT( !ret );

	int tno;
	for( tno = 0; tno < morph->m_targetnum; tno++ ){
		TARGETPRIM* curtp;
		curtp = morph->m_ptarget + tno;
		ret = WriteIntData( &(curtp->seri), 1 );
		_ASSERT( !ret );
	}

	return ret;
}

int CSigFile::WritePart( CShdElem* selem )
{
	CPart* part = selem->part;
	CMeshInfo* mi;
	int ret;

	_ASSERT( part );
	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	VEC3F jvec;
	CVec3f srcvec;
	srcvec.CopyData( &(part->jointloc) );
	jvec.x = srcvec.x;
	jvec.y = srcvec.y;
	jvec.z = srcvec.z;
	ret = WriteVecData( &jvec, 1 );
	_ASSERT( !ret );

	
	ret = WriteIntData( &part->m_ignorelim01, 1 );
	_ASSERT( !ret );

	ret = WriteUCharData( (unsigned char*)&part->m_lim0, sizeof( DVEC3 ) );
	_ASSERT( !ret );

	ret = WriteUCharData( (unsigned char*)&part->m_lim1, sizeof( DVEC3 ) );
	_ASSERT( !ret );


	//magicno_7
	ret = WriteIntData( &selem->m_notsel, 1 );
	_ASSERT( !ret );

	//magicno_9
	ret = WriteIntData( &selem->m_setmikobonepos, 1 );
	_ASSERT( !ret );

	ret = WriteUCharData( (unsigned char*)&(selem->m_mikobonepos), sizeof( D3DXVECTOR3 ) );
	_ASSERT( !ret );

	//magicno_10
	ret = WriteIntData( &part->m_za4_type, 1 );
	_ASSERT( !ret );

	ret = WriteIntData( &part->m_za4_rotaxis, 1 );
	_ASSERT( !ret );

	ret = WriteUCharData( (unsigned char*)&(part->m_za4q), sizeof( CQuaternion ) );
	_ASSERT( !ret );
		//iktarget
	ret = WriteIntData( &selem->m_iktarget, 1 );
	_ASSERT( !ret );

	ret = WriteIntData( &selem->m_iktargetlevel, 1 );
	_ASSERT( !ret );

	ret = WriteIntData( &selem->m_iktargetcnt, 1 );
	_ASSERT( !ret );

	ret = WriteUCharData( (unsigned char*)&(selem->m_iktargetpos), sizeof( D3DXVECTOR3 ) );
	_ASSERT( !ret );


	return ret;
}
int CSigFile::WritePolyLine( CShdElem* selem )
{
	CVecLine* vline = selem->vline;
	int ret;

	_ASSERT( vline );

	ret = WriteVecLine( vline );
	return ret;
}
int CSigFile::WritePolygon( CShdElem* selem )
{
	CPolygon* polygon = selem->polygon;
	CVecLine* vl;
	CMeshInfo* mi;
	int i, allocleng;
	int ret = 0;

	_ASSERT( polygon );

	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	allocleng = polygon->allocleng;
	for( i = 0; i < allocleng; i++ ){
		vl = (*polygon)( i );
		ret += WriteVecLine( vl );
		_ASSERT( !ret );
	}
	return ret;
}
int CSigFile::WriteSphere( CShdElem* selem )
{
	CSphere* sphere = selem->sphere;
	//CVecLine* vl;
	CMeshInfo* mi;
	//int i, allocleng;
	int ret = 0;

	_ASSERT( sphere );

	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	/***
	allocleng = sphere->allocleng;
	for( i = 0; i < allocleng; i++ ){
		vl = (*sphere)( i );
		ret += WriteVecLine( vl );
		_ASSERT( !ret );
	}
	***/

	ret += WriteMatData( &(sphere->defmat), 1 );
	_ASSERT( !ret );

	return ret;
	

}
int CSigFile::WriteBezMesh( CShdElem* selem )
{
	CBezMesh* bm = selem->bmesh;
	CMeshInfo* mi;
	int ret;

	_ASSERT( bm );

	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	return ret;
}
int CSigFile::WriteRevolved( CShdElem* selem )
{
	CRevolved* revolved = selem->revolved;
	CMeshInfo* mi;
	CVecLine* vl;
	REVINFO ri;
	int ret, i, allocleng;
	unsigned long wleng, writeleng;

	_ASSERT( revolved );

	mi = selem->GetMeshInfo();
	_ASSERT( mi );
	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	allocleng = revolved->allocleng;
	for( i = 0; i < allocleng; i++ ){
		vl = (*revolved)( i );
		ret += WriteVecLine( vl );
		_ASSERT( !ret );
	}
	
	ZeroMemory( &ri, sizeof( REVINFO ) );
	ri.radfrom = revolved->radfrom;
	ri.radto = revolved->radto;
	ri.isnormalset = revolved->isnormalset;
	wleng = sizeof( REVINFO );

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, &ri, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "sigfile : WriteRevolved : bufsize too short error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( m_buf.buf + m_buf.pos, &ri, wleng );
		m_buf.pos += wleng;

		break;
	case SIGWRITEMODE_BUFSIZE:
		m_buf.pos += wleng;

		break;
	}

	ret = WriteMatData( &(revolved->axis), 1 );
	_ASSERT( !ret );

	return ret;
}
int CSigFile::WriteDisk( CShdElem* selem )
{
	CDisk* disk = selem->disk;
	CMeshInfo* mi;
	CVecLine* vl;
	int ret = 0;
	int i, allocleng;

	_ASSERT( disk );
	mi = selem->GetMeshInfo();
	_ASSERT( mi );
	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	allocleng = disk->allocleng;
	for( i = 0; i < allocleng; i++ ){
		vl = (*disk)( i );
		ret += WriteVecLine( vl );
		_ASSERT( !ret );
	}
	
	ret += WriteMatData( &(disk->defmat), 1 );
	_ASSERT( !ret );

	return ret;
}
int CSigFile::WriteBezLine( CShdElem* selem )
{
	CBezLine* bline = selem->bline;
	CMeshInfo* mi;
	CVecLine* vl;
	int i, vlnum;
	int ret = 0;

	_ASSERT( bline );

	mi = selem->GetMeshInfo();
	_ASSERT( mi );
	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	vlnum = mi->total;

	for( i = 0; i < vlnum; i++ ){
		vl = (*bline)( i );
		ret += WriteVecLine( vl );
		_ASSERT( !ret );
	}

	return ret;
}
int CSigFile::WriteExtrude( CShdElem* selem )
{
	CExtrude* extr = selem->extrude;
	CMeshInfo* mi;
	CVecLine* vl;
	int ret = 0;
	int i, allocleng;

	_ASSERT( extr );
	mi = selem->GetMeshInfo();
	_ASSERT( mi );
	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	allocleng = extr->allocleng;
	for( i = 0; i < allocleng; i++ ){
		vl = (*extr)( i );
		ret += WriteVecLine( vl );
		_ASSERT( !ret );
	}
	
	ret += WriteVecData( &(extr->exvec), 1 );
	_ASSERT( !ret );
	
	return ret;
}

int CSigFile::WriteInfScope( CShdElem* selem )
{
	int ret;
	int scno;
	for( scno = 0; scno < selem->scopenum; scno++ ){
		CInfScope* curis;
		curis = *( selem->ppscope + scno );
		_ASSERT( curis );

		SIGHEADER sigheader;
		ZeroMemory( &sigheader, sizeof( SIGHEADER ) );

		sigheader.type = curis->type;
		sprintf_s( sigheader.name, 256, "infscope%d", scno );

		sigheader.transparent = selem->transparent;
		sigheader.flags &= ~FLAGS_ANCHORFLAG;
		sigheader.facetm180 = 0.0f;

		sigheader.flags |= FLAGS_INFELEM;//!!!!!

		if( curis->applychild )
			sigheader.texrepx = curis->applychild->serialno;//!!!!!!!!!!!!
		else
			sigheader.texrepx = 0;
		
		if( curis->target )
			sigheader.texrepy = curis->target->serialno;//!!!!!!!!!!!!!!!!
		else
			sigheader.texrepy = 0;


		if( curis->type == SHDPOLYMESH2 ){
			CPolyMesh2* pm2ptr;
			pm2ptr = curis->polymesh2;

			if( !pm2ptr ){
				DbgOut( "sigfile : WriteInfScope : pm2 not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( pm2ptr->orgnobuf ){
				sigheader.flags |= FLAGS_ORGNOBUF;
			}else{
				sigheader.flags &= ~FLAGS_ORGNOBUF;
			}

			sigheader.facetm180 = curis->pm2facet;//!!!!!!!!!!!

		}
		if( curis->type == SHDPOLYMESH ){
			CPolyMesh* pmptr;
			pmptr = curis->polymesh;

			if( !pmptr ){
				DbgOut( "sigfile : WriteInfScope : pm not exist error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			if( pmptr->orgnobuf ){
				sigheader.flags |= FLAGS_ORGNOBUF;
			}else{
				sigheader.flags &= ~FLAGS_ORGNOBUF;
			}

			if( pmptr->uvbuf ){
				sigheader.flags |= FLAGS_PMUVBUF;
			}else{
				sigheader.flags &= ~FLAGS_PMUVBUF;
			}

		}

		///////
		ret = WriteSigHeader( sigheader );
		if( ret ){
			_ASSERT( 0 );
			return ret;
		}
		////////
		int isinfscope = 1;
		if( curis->type == SHDPOLYMESH ){
			ret = WritePolyMesh( curis->polymesh, isinfscope );
			if( ret ){
				DbgOut( "sigfile : WriteInfScope : WritePolyMesh error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}else if( curis->type == SHDPOLYMESH2 ){
			ret = WritePolyMesh2( curis->polymesh2, isinfscope );
			if( ret ){
				DbgOut( "sigfile : WriteInfScope : WritePolyMesh2 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			DbgOut( "sigfile : WriteInfScope : type error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


	}

	return 0;
}


int CSigFile::WritePolyMesh2( CPolyMesh2* polymesh2, int isinfscope )
{
	_ASSERT( polymesh2 );

//DbgOut( "sigfile : WritePolyMesh2 : 0 \n" );

	CMeshInfo* mi;
	mi = polymesh2->meshinfo;
	if( !mi ){
		DbgOut( "sigfile : WritePolyMesh2 : GetMeshInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = WriteMIHeader( mi );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : WriteMIHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int bufleng, colbufleng;
	bufleng = mi->n * 3;
	colbufleng = mi->n;

//DbgOut( "sigfile : WritePolyMesh2 : 1 \n" );

	ret = WriteVecData( polymesh2->pointbuf, bufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : WriteVecData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteIntData( polymesh2->samepointbuf, bufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : WriteIntData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = WriteIntData( polymesh2->oldpno2optpno, bufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : write old2opt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = WriteIntData( polymesh2->smoothbuf, bufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : write smoothbuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = WriteUCharData( (unsigned char*)polymesh2->uvbuf, sizeof( COORDINATE ) * bufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}
///////
	ret = WriteUCharData( (unsigned char*)polymesh2->diffusebuf, sizeof( ARGBF ) * colbufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteUCharData( (unsigned char*)polymesh2->ambientbuf, sizeof( ARGBF ) * colbufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteUCharData( (unsigned char*)polymesh2->specularbuf, sizeof( float ) * colbufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteUCharData( (unsigned char*)polymesh2->orgnormal, sizeof( VEC3F ) * bufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : orgnormal write error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int optpleng = 0;


	//頂点ごとの色情報の書き出し
	if( writemagicno >= SIGFILEMAGICNO_2 ){

		optpleng = polymesh2->optpleng;

		//optplengを書き出す。
		ret = WriteIntData( &optpleng, 1 );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : WriteIntData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// opttlv->colorを書き出す
		int optno;
		for( optno = 0; optno < optpleng; optno++ ){
			DWORD dwdiff;
			dwdiff = (polymesh2->opttlv + optno)->color;
			ret = WriteDWData( &dwdiff, 1 );
			if( ret ){
				DbgOut( "sigfile : WritePolyMesh2 : WriteDWData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		// optambientを書き出す
		ret = WriteRGBAData( polymesh2->optambient, optpleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : WriteRGBData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// opttlv->specularを書き出す
		for( optno = 0; optno < optpleng; optno++ ){
			DWORD dwspec;
			dwspec = (polymesh2->opttlv + optno)->specular;
			ret = WriteDWData( &dwspec, 1 );
			if( ret ){
				DbgOut( "sigfile : WritePolyMesh2 : WriteDWData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( writemagicno >= SIGFILEMAGICNO_4 ){
		ret = WriteFloatData( polymesh2->diffbuf, colbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write diffbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteFloatData( polymesh2->powerbuf, colbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteFloatData( polymesh2->emissivebuf, colbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write emissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		ret = WriteFloatData( polymesh2->optpowerbuf, optpleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh2->optemissivebuf, optpleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write optemissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	// orgnobufの書き出し
	if( polymesh2->orgnobuf ){
		ret = WriteIntData( polymesh2->orgnobuf, bufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : WriteIntData error %d!!!\n",
				bufleng );
			_ASSERT( 0 );
			return 1;
		}
	}


	_ASSERT( polymesh2->m_IE );
	INFELEMHEADER ieheader;
	SIGINFELEM sigie;
	CInfElem* ieptr;
	int pno;
	for( pno = 0; pno < optpleng; pno++ ){
		ieptr = polymesh2->m_IE + pno;
		_ASSERT( ieptr );

		ZeroMemory( &ieheader, sizeof( INFELEMHEADER ) );
		ieheader.infnum = ieptr->infnum;
		ieheader.normalizeflag = ieptr->normalizeflag;
		ieheader.symaxis = ieptr->symaxis;
		ieheader.symdist = ieptr->symdist;
		ret = WriteUCharData( (unsigned char*)&ieheader, sizeof( INFELEMHEADER ) );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write ieheader error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		int infno;
		for( infno = 0; infno < ieptr->infnum; infno++ ){
			INFELEM* curIE;
			curIE = ieptr->ie + infno;
			_ASSERT( curIE );

			ZeroMemory( &sigie, sizeof( SIGINFELEM ) );
			sigie.childno = curIE->childno;
			sigie.bonematno = curIE->bonematno;
			sigie.kind = curIE->kind;
			sigie.userrate = curIE->userrate;	
			sigie.orginf = curIE->orginf;
			sigie.dispinf = curIE->dispinf;
			ret = WriteUCharData( (unsigned char*)&sigie, sizeof( SIGINFELEM ) );
			if( ret ){
				DbgOut( "sigfile : WritePolyMesh2 : write sigie error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	// vcol
	int validvcolnum;
	validvcolnum = polymesh2->GetValidVColNum();
	ret = WriteIntData( &validvcolnum, 1 );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh2 : write vcolnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	
	int vcno;
	VCOLDATA* curvc;
	for( vcno = 0; vcno < polymesh2->vcolnum; vcno++ ){
		curvc = *( polymesh2->vcoldata + vcno );
		if( curvc && (curvc->vertno >= 0) ){
			ret = WriteUCharData( (unsigned char*)curvc, sizeof( VCOLDATA ) );
			if( ret ){
				DbgOut( "sigfile : WritePolyMesh2 : write curvc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}

	//toon1
	if( polymesh2->m_toon1 ){
		ret = WriteIntData( &polymesh2->m_materialnum, 1 );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write mateiralnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int matno;
		for( matno = 0; matno < polymesh2->m_materialnum; matno++ ){
			SIGTOON1EX sigtoon1;
			ZeroMemory( &sigtoon1, sizeof( SIGTOON1EX ) );

			CToon1Params* curtoon1;
			curtoon1 = polymesh2->m_toon1 + matno;

			strcpy_s( sigtoon1.toon1.name, 32, curtoon1->name );
			sigtoon1.toon1.diffuse = curtoon1->diffuse;
			sigtoon1.toon1.ambient = curtoon1->ambient;
			sigtoon1.toon1.specular = curtoon1->specular;
			sigtoon1.toon1.darkh = curtoon1->darkh;
			sigtoon1.toon1.brighth = curtoon1->brighth;
			sigtoon1.toon1.ambientv = curtoon1->ambientv;
			sigtoon1.toon1.diffusev = curtoon1->diffusev;
			sigtoon1.toon1.specularv = curtoon1->specularv;
			sigtoon1.toon1.darknl = curtoon1->darknl;
			sigtoon1.toon1.brightnl = curtoon1->brightnl;
			sigtoon1.toon1.edgecol0 = curtoon1->edgecol0;
			sigtoon1.toon1.edgevalid0 = curtoon1->edgevalid0;
			sigtoon1.toon1.edgeinv0 = curtoon1->edgeinv0;

			sigtoon1.toon0dnl = curtoon1->toon0dnl;
			sigtoon1.toon0bnl = curtoon1->toon0bnl;

			ret = WriteUCharData( (unsigned char*)&sigtoon1, sizeof( SIGTOON1EX ) );
			if( ret ){
				DbgOut( "sigfile : WritePolyMesh2 : write sigtoon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = WriteUCharData( (unsigned char*)polymesh2->m_attrib, sizeof( DWORD ) * mi->n );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write attrib error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}


	if( polymesh2->m_attrib0 && ( isinfscope == 0 ) ){
		ret = WriteIntData( polymesh2->m_attrib0, colbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh2 : write attrib0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



//DbgOut( "sigfile : WritePolyMesh2 : 2 \n" );

	return ret;

}


int CSigFile::WritePolyMesh( CPolyMesh* polymesh, int isinfscope )
{
	_ASSERT( polymesh );

	//DbgOut( "sigfile : WritePolyMesh : type %d\n", selem->type );

	CMeshInfo* mi;
	mi = polymesh->meshinfo;
	if( !mi ){
		DbgOut( "sigfile : WritePolyMesh : GetMeshInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = WriteMIHeader( mi );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh : WriteMIHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int pointbufleng, indexbufleng;
	pointbufleng = mi->m;
	indexbufleng = mi->n * 3;

	ret = WriteVecData( polymesh->pointbuf, pointbufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh : WriteVecData error %d!!!\n",
			pointbufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteIntData( polymesh->indexbuf, indexbufleng );
	if( ret ){
		DbgOut( "sigfile : WritePolyMesh : WriteIntData error %d!!!\n",
			indexbufleng );
		_ASSERT( 0 );
		return 1;
	}


	//頂点ごとの色情報の書き出し
	if( writemagicno >= SIGFILEMAGICNO_2 ){
		ret = WriteRGBAData( polymesh->diffusebuf, pointbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : WriteRGBAData diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh->ambientbuf, pointbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : WriteRGBAData ambient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh->specularbuf, pointbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : WriteRGBAData specular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( writemagicno >= SIGFILEMAGICNO_4 ){
		ret = WriteFloatData( polymesh->powerbuf, pointbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : WriteFloatData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh->emissivebuf, pointbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : WriteRGBAData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}


	// orgnobufの書き出し
	if( polymesh->orgnobuf ){
		ret = WriteIntData( polymesh->orgnobuf, pointbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : WriteIntData error %d!!!\n",
				pointbufleng );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( polymesh->uvbuf ){
		//UV
		ret = WriteUCharData( (unsigned char*)polymesh->uvbuf, sizeof( COORDINATE ) * pointbufleng );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : WriteUCharData UV error %d!!!\n",
				pointbufleng );
			_ASSERT( 0 );
			return 1;
		}
	}

	_ASSERT( polymesh->m_IE );
	INFELEMHEADER ieheader;
	SIGINFELEM sigie;
	CInfElem* ieptr;
	int pno;
	for( pno = 0; pno < pointbufleng; pno++ ){
		ieptr = polymesh->m_IE + pno;
		_ASSERT( ieptr );

		ZeroMemory( &ieheader, sizeof( INFELEMHEADER ) );
		ieheader.infnum = ieptr->infnum;
		ieheader.normalizeflag = ieptr->normalizeflag;
		ieheader.symaxis = ieptr->symaxis;
		ieheader.symdist = ieptr->symdist;
		ret = WriteUCharData( (unsigned char*)&ieheader, sizeof( INFELEMHEADER ) );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : write ieheader error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		int infno;
		for( infno = 0; infno < ieptr->infnum; infno++ ){
			INFELEM* curIE;
			curIE = ieptr->ie + infno;
			_ASSERT( curIE );

			ZeroMemory( &sigie, sizeof( SIGINFELEM ) );
			sigie.childno = curIE->childno;
			sigie.bonematno = curIE->bonematno;
			sigie.kind = curIE->kind;
			sigie.userrate = curIE->userrate;	
			sigie.orginf = curIE->orginf;
			sigie.dispinf = curIE->dispinf;
			ret = WriteUCharData( (unsigned char*)&sigie, sizeof( SIGINFELEM ) );
			if( ret ){
				DbgOut( "sigfile : WritePolyMesh : write sigie error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}


	//toon1
	if( polymesh->m_toon1 ){
		ret = WriteIntData( &polymesh->m_materialnum, 1 );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : write mateiralnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int matno;
		for( matno = 0; matno < polymesh->m_materialnum; matno++ ){
			SIGTOON1EX sigtoon1;
			ZeroMemory( &sigtoon1, sizeof( SIGTOON1EX ) );

			CToon1Params* curtoon1;
			curtoon1 = polymesh->m_toon1 + matno;

			strcpy_s( sigtoon1.toon1.name, 32, curtoon1->name );
			sigtoon1.toon1.diffuse = curtoon1->diffuse;
			sigtoon1.toon1.ambient = curtoon1->ambient;
			sigtoon1.toon1.specular = curtoon1->specular;
			sigtoon1.toon1.darkh = curtoon1->darkh;
			sigtoon1.toon1.brighth = curtoon1->brighth;
			sigtoon1.toon1.ambientv = curtoon1->ambientv;
			sigtoon1.toon1.diffusev = curtoon1->diffusev;
			sigtoon1.toon1.specularv = curtoon1->specularv;
			sigtoon1.toon1.darknl = curtoon1->darknl;
			sigtoon1.toon1.brightnl = curtoon1->brightnl;
			sigtoon1.toon1.edgecol0 = curtoon1->edgecol0;
			sigtoon1.toon1.edgevalid0 = curtoon1->edgevalid0;
			sigtoon1.toon1.edgeinv0 = curtoon1->edgeinv0;
	
			sigtoon1.toon0dnl = curtoon1->toon0dnl;
			sigtoon1.toon0bnl = curtoon1->toon0bnl;

			ret = WriteUCharData( (unsigned char*)&sigtoon1, sizeof( SIGTOON1EX ) );
			if( ret ){
				DbgOut( "sigfile : WritePolyMesh : write sigtoon1 error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		
		ret = WriteUCharData( (unsigned char*)polymesh->m_attrib, sizeof( DWORD ) * mi->n );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : write attrib error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( polymesh->m_attrib0 && (isinfscope == 0) ){
		ret = WriteIntData( polymesh->m_attrib0, mi->n );
		if( ret ){
			DbgOut( "sigfile : WritePolyMesh : write attrib0 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	return ret;
}



//////////////////////////////
// load
//////////////////////////////
void CSigFile::InitLoadParams()
{
	int  ret;
	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	curseri = 0;
	befseri = 0;
	curdepth = 0;
	befdepth = 0;
	curshdtype = 0;
	befshdtype = 0;

	bezlinenum = 0;
	bezlineno = 0;
	bezsurfno = 0;

	writemagicno = 0;
	loadmagicno = 0;

	m_adjustuvflag = 0;

	m_buf.buf = 0;
	m_buf.bufsize = 0;
	m_buf.pos = 0;

	m_frombuf = 0;

	D3DXMatrixIdentity( &m_offsetmat );

	m_groundflag = 0;
	m_materialoffset = 0;
}

int CSigFile::LoadSigHeader( SIGHEADER* sigheader )
{
	DWORD rleng, readleng;
	
	rleng = sizeof( SIGHEADER );


	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)sigheader, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)sigheader, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	befseri = curseri;
	curseri = sigheader->serialno + m_offset;

	befdepth = curdepth;
	curdepth = sigheader->depth;

	befshdtype = curshdtype;
	curshdtype = sigheader->type;

	return 0;	 
}
int CSigFile::LoadMIHeader( CMeshInfo* dstmi )
{
	DWORD rleng, readleng;
	MIHEADER mih;
	int ret;
	rleng = sizeof( MIHEADER );

	
	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)(&mih), rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)(&mih), m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}
	
	//if( curshdtype != mih.type ){
	//	_ASSERT( 0 );
	//	return 1;
	//}

	ret = dstmi->SetMem( mih.type, BASE_TYPE );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.m, MESHI_M );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.n, MESHI_N );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.total, MESHI_TOTAL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.mclosed, MESHI_MCLOSED );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.nclosed, MESHI_NCLOSED );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.skip, MESHI_SKIP );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.hascolor, MESHI_HASCOLOR );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	// material
	ret = dstmi->SetMem( mih.diffuse, MAT_DIFFUSE | VEC_ALL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.specular, MAT_SPECULAR | VEC_ALL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = dstmi->SetMem( mih.ambient, MAT_AMBIENT | VEC_ALL );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;	
}

int CSigFile::LoadFloatData( float* dstfloat, int flnum )
{
	DWORD rleng, readleng;

	rleng = sizeof( float ) * flnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dstfloat, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dstfloat, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CSigFile::LoadIntData( int* dstint, int intnum )
{
	DWORD rleng, readleng;

	rleng = sizeof( int ) * intnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dstint, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dstint, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CSigFile::LoadDWData( DWORD* dstdw, int dwnum )
{
	DWORD rleng, readleng;

	rleng = sizeof( DWORD ) * dwnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dstdw, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dstdw, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}


int CSigFile::LoadARGBData( ARGBF* dstargb, int colnum )
{
	DWORD rleng, readleng;

	rleng = sizeof( ARGBF ) * colnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dstargb, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dstargb, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}
	
	return 0;
}


int CSigFile::LoadVecData( VEC3F* dstvec, int vnum )
{
	DWORD rleng, readleng;

	rleng = sizeof( VEC3F ) * vnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dstvec, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dstvec, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}
int CSigFile::LoadMatData( CMatrix2* dstmat, int matnum )
{
	DWORD rleng, readleng;

	rleng = sizeof( CMatrix2 ) * matnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dstmat, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dstmat, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

	return 0;
}

int CSigFile::LoadUCharData( unsigned char* dstuc, int ucnum )
{
	DWORD rleng, readleng;

	rleng = ucnum;

	if( m_frombuf == 0 ){
		ReadFile( hfile, (void*)dstuc, rleng, &readleng, NULL );
		if( rleng != readleng ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		if( m_buf.pos + (int)rleng > m_buf.bufsize ){
			_ASSERT( 0 );
			return 1;
		}
		MoveMemory( (void*)dstuc, m_buf.buf + m_buf.pos, rleng );
		m_buf.pos += rleng;
	}

		
	return 0;
}


int CSigFile::LoadSigFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, D3DXVECTOR3 datamult, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot, int groundflag )
{
	int ret = 0;

	if( !filename || !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "sigfile : LoadSigFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	InitLoadParams();

	m_frombuf = 0;//!!!
	m_groundflag = groundflag;

	//m_offsetpos = offsetpos;

	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;
	if( offset > 0 )
		m_offset = offset - 1;//import dataの先頭データもSHDROOTで、無効データなので。
	else
		m_offset = offset;

	m_datamult = datamult;
	m_adjustuvflag = adjustuvflag;

	D3DXMATRIX scalemat, rotxmat, rotymat, rotzmat, shiftmat;
	D3DXMatrixIdentity( &scalemat );
	D3DXMatrixIdentity( &rotxmat );
	D3DXMatrixIdentity( &rotymat );
	D3DXMatrixIdentity( &rotzmat );
	D3DXMatrixIdentity( &shiftmat );

	scalemat._11 = m_datamult.x;
	scalemat._22 = m_datamult.y;
	scalemat._33 = m_datamult.z;

	shiftmat._41 = offsetpos.x;
	shiftmat._42 = offsetpos.y;
	shiftmat._43 = offsetpos.z;

	D3DXMatrixRotationX( &rotxmat, rot.x * (float)DEG2PAI );
	D3DXMatrixRotationY( &rotymat, rot.y * (float)DEG2PAI );
	D3DXMatrixRotationZ( &rotzmat, rot.z * (float)DEG2PAI );

	m_offsetmat = scalemat * rotzmat * rotymat * rotxmat * shiftmat;




	char pathname[2048];
	int ch = '\\';
	char* enptr = 0;
	enptr = strrchr( filename, ch );
	if( enptr ){
		strcpy_s( pathname, 2048, filename );
	}else{
		ZeroMemory( pathname, 2048 );
		int resdirleng;
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)pathname, 2048 );
		_ASSERT( resdirleng );
		strcat_s( pathname, 2048, filename );
	}


	hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		DbgOut( "sigfile : LoadSigFile : media not found error %s!!!\n", pathname );
		ret = D3DAPPERR_MEDIANOTFOUND;
		goto loadsigexit;
	}	

	ret = LoadSigFile_aft( offset );
	if( ret ){
		_ASSERT( 0 );
	}

	goto loadsigexit;

loadsigexit:
	if( hfile != INVALID_HANDLE_VALUE ){
		//FlushFileBuffers( hfile );
		//SetEndOfFile( hfile );
		CloseHandle( hfile );
		hfile = 0;
	}
	return ret;//!!!!!!!

}

int CSigFile::LoadSigFileFromPnd( CPanda* panda, int pndid, int propno, 
		CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot, int groundflag )
{
	int ret = 0;

	InitLoadParams();

	m_frombuf = 1;//!!!
	m_groundflag = groundflag;

	//m_offsetpos = offsetpos;

	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;
	if( offset > 0 )
		m_offset = offset - 1;//import dataの先頭データもSHDROOTで、無効データなので。
	else
		m_offset = offset;

	m_datamult = D3DXVECTOR3( datamult, datamult, datamult );
	m_adjustuvflag = adjustuvflag;


	D3DXMATRIX scalemat, rotxmat, rotymat, rotzmat, shiftmat;
	D3DXMatrixIdentity( &scalemat );
	D3DXMatrixIdentity( &rotxmat );
	D3DXMatrixIdentity( &rotymat );
	D3DXMatrixIdentity( &rotzmat );
	D3DXMatrixIdentity( &shiftmat );

	scalemat._11 = m_datamult.x;
	scalemat._22 = m_datamult.y;
	scalemat._33 = m_datamult.z;

	shiftmat._41 = offsetpos.x;
	shiftmat._42 = offsetpos.y;
	shiftmat._43 = offsetpos.z;

	D3DXMatrixRotationX( &rotxmat, rot.x * (float)DEG2PAI );
	D3DXMatrixRotationY( &rotymat, rot.y * (float)DEG2PAI );
	D3DXMatrixRotationZ( &rotzmat, rot.z * (float)DEG2PAI );

	m_offsetmat = scalemat * rotzmat * rotymat * rotxmat * shiftmat;


	PNDPROP prop;
	ZeroMemory( &prop, sizeof( PNDPROP ) );
	ret = panda->GetProperty( pndid, propno, &prop );
	_ASSERT( !ret );

	m_buf.buf = (char*)malloc( sizeof( char ) * prop.sourcesize );
	if( !(m_buf.buf) ){
		DbgOut( "sigfile : LoadSigFileFromPnd : buf alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto loadsigexit4;
	}
	
	int getsize = 0;
	ret = panda->Decrypt( pndid, prop.path, (unsigned char*)m_buf.buf, prop.sourcesize, &getsize );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	if( getsize != prop.sourcesize ){
		_ASSERT( 0 );
		return 1;
	}

	m_buf.bufsize = prop.sourcesize;
	m_buf.pos = 0;

/////////
	ret = LoadSigFile_aft( offset );
	if( ret ){
		_ASSERT( 0 );
	}

	goto loadsigexit4;

loadsigexit4:
	if( m_buf.buf ){
		free( m_buf.buf );
		m_buf.buf = 0;
		m_buf.bufsize = 0;
		m_buf.pos = 0;
	}
	return ret;//!!!!!!!
}


int CSigFile::LoadSigFileFromBuf( char* bufptr, int bufsize, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult, int adjustuvflag, D3DXVECTOR3 offsetpos, D3DXVECTOR3 rot, int groundflag )
{
	int ret = 0;

	if( !bufptr || (bufsize < 0) || !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "sigfile : LoadSigFileFromBuf : parameter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	InitLoadParams();

	m_frombuf = 1;//!!!
	m_groundflag = groundflag;

	//m_offsetpos = offsetpos;

	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;
	if( offset > 0 )
		m_offset = offset - 1;//import dataの先頭データもSHDROOTで、無効データなので。
	else
		m_offset = offset;

	m_datamult = D3DXVECTOR3( datamult, datamult, datamult );
	m_adjustuvflag = adjustuvflag;


	D3DXMATRIX scalemat, rotxmat, rotymat, rotzmat, shiftmat;
	D3DXMatrixIdentity( &scalemat );
	D3DXMatrixIdentity( &rotxmat );
	D3DXMatrixIdentity( &rotymat );
	D3DXMatrixIdentity( &rotzmat );
	D3DXMatrixIdentity( &shiftmat );

	scalemat._11 = m_datamult.x;
	scalemat._22 = m_datamult.y;
	scalemat._33 = m_datamult.z;

	shiftmat._41 = offsetpos.x;
	shiftmat._42 = offsetpos.y;
	shiftmat._43 = offsetpos.z;

	D3DXMatrixRotationX( &rotxmat, rot.x * (float)DEG2PAI );
	D3DXMatrixRotationY( &rotymat, rot.y * (float)DEG2PAI );
	D3DXMatrixRotationZ( &rotzmat, rot.z * (float)DEG2PAI );

	m_offsetmat = scalemat * rotzmat * rotymat * rotxmat * shiftmat;


	m_buf.buf = (char*)malloc( sizeof( char ) * bufsize );
	if( !(m_buf.buf) ){
		DbgOut( "sigfile : LoadSigFileFromBuf : buf alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto loadsigexit2;
	}
	
	MoveMemory( m_buf.buf, bufptr, sizeof( char ) * bufsize );

	m_buf.bufsize = bufsize;
	m_buf.pos = 0;

/////////
	ret = LoadSigFile_aft( offset );
	if( ret ){
		_ASSERT( 0 );
	}

	goto loadsigexit2;

loadsigexit2:
	if( m_buf.buf ){
		free( m_buf.buf );
		m_buf.buf = 0;
		m_buf.bufsize = 0;
		m_buf.pos = 0;
	}
	return ret;//!!!!!!!
}

int CSigFile::LoadSigFile_aft( int offset )
{
	g_toon1matcnt = 0;//!!!!!!!!!!!!!!!!!!!!!!

	int ret;
	int isend = 0;
	SIGHEADER sh;

	ZeroMemory( &sh, sizeof( SIGHEADER ) );
	ret = LoadSigHeader( &sh );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	if( (sh.serialno != SIGFILEMAGICNO_1) && (sh.serialno != SIGFILEMAGICNO_2) && 
		(sh.serialno != SIGFILEMAGICNO_4) && (sh.serialno != SIGFILEMAGICNO_5) && 
		(sh.serialno != SIGFILEMAGICNO_6) && (sh.serialno != SIGFILEMAGICNO_7) && (sh.serialno != SIGFILEMAGICNO_8) && 
		(sh.serialno != SIGFILEMAGICNO_9) && (sh.serialno != SIGFILEMAGICNO_10) && (sh.serialno != SIGFILEMAGICNO_11) && 
		(sh.serialno != SIGFILEMAGICNO_12) && (sh.serialno != SIGFILEMAGICNO_13) && (sh.serialno != SIGFILEMAGICNO_14) ){
		
		DbgOut( "sigfile : LoadSigFile_aft : file kind error !!!\n" );
		return D3DAPPERR_MEDIANOTFOUND;//
	}


	// set aspect
	if( offset == 0 ){
		if( (sh.aspect >= 0.2f) && (sh.aspect <= 5.0f) ){
			lpsh->m_aspect = sh.aspect;
		}else{
			lpsh->m_aspect = 1.0f;
		}
	}

	// im2enableflag
	if( sh.flags & FLAGS_ORGNOBUF ){
		lpsh->m_im2enableflag = 1;
		DbgOut( "sigfile : LoadSigFile : im2enableflag 1 !!!\n" );

	}else{
		lpsh->m_im2enableflag = 0;
		DbgOut( "sigfile : LoadSigFile : im2enableflag 0 !!!\n" );

	}

	// for miko
	if( sh.flags & FLAGS_BONETYPE ){
		lpsh->m_bonetype = BONETYPE_MIKO;
	}else{
		lpsh->m_bonetype = BONETYPE_RDB2;
	}

	if( sh.flags & FLAGS_MIKOBLEND ){
		lpsh->m_mikoblendtype = MIKOBLEND_MIX;
	}else{
		lpsh->m_mikoblendtype = MIKOBLEND_SKINNING;
	}

	if( sh.flags3 & FLAGS3_STANDARD ){
		lpmh->m_standard = 1;
	}else{
		lpmh->m_standard = 0;
	}

	loadmagicno = sh.serialno;//!!!!!!!!!!!!!
	lpsh->m_sigmagicno = loadmagicno;

//// scene_ambient, shader, overflowの読み込み（MAGICNO_4以降）
	if( loadmagicno >= SIGFILEMAGICNO_4 ){
		ret = LoadARGBData( &lpsh->m_scene_ambient, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadIntData( &lpsh->m_shader, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		if( lpsh->m_shader == COL_TOON0 ){
			lpsh->m_shader = COL_TOON1;//!!!!!!!!!!!!!!!!!!
		}
		ret = LoadIntData( &lpsh->m_overflow, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}else{
		lpsh->m_scene_ambient.a = 1.0f;
		lpsh->m_scene_ambient.r = 0.25f;
		lpsh->m_scene_ambient.g = 0.25f;
		lpsh->m_scene_ambient.b = 0.25f;

		lpsh->m_shader = COL_PHONG;

		lpsh->m_overflow = OVERFLOW_ORG;
	}

///////
	if( m_offset == 0 ){
		ret = lpsh->InitMQOMaterial();
		if( ret ){
			DbgOut( "sigfile : LoadSigFile_aft : sh InitMQOMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		m_materialoffset = 0;
	}else{
		int maxmatno = 0;
		int chkmatcnt;
		CMQOMaterial* curchkmat = lpsh->m_mathead;
		for( chkmatcnt = 0; chkmatcnt < lpsh->m_materialcnt; chkmatcnt++ ){
			if( maxmatno < curchkmat->materialno ){
				maxmatno = curchkmat->materialno;
			}
			curchkmat = curchkmat->next;
		}

		//m_materialoffset = lpsh->m_materialcnt - 1;
		m_materialoffset = maxmatno + 1;
	}


	if( sh.flags2 & FLAGS2_MQOMATERIAL ){

//DbgOut( "checkalp!!! : sigfile : LoadSigFile_aft LoadSigMaterial\r\n" );
		ret = LoadSigMaterial();
		if( ret ){
			DbgOut( "sigfile : LoadSigFile_aft : LoadSigMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

///////

	ret = lpth->Start( m_offset );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

DbgOut( "sigfile : LoadSigFile : start Load Loop %d\n", m_offset );

	while( !isend ){
		ZeroMemory( &sh, sizeof( SIGHEADER ) );
		ret = LoadSigHeader( &sh );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		if( (sh.type != SHDROOT) && (sh.type != -1) ){
			ret = AddShape2Tree( sh );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}

		if( sh.influencenum > 0 ){
			ret = LoadInfluenceList( sh.influencenum, offset );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		if( sh.ignorenum > 0 ){
			ret = LoadIgnoreList( sh.ignorenum, offset );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}

		ret = LoadExtTex( sh );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}


		switch( sh.type ){
		case SHDROTATE:
		case SHDSLIDER:
		case SHDSCALE:
		case SHDUNISCALE:
		case SHDPATH:		
		case SHDPART:
		case SHDBALLJOINT:
			ret = LoadPart( sh.flags2 );
			break;
		case SHDMORPH:
			ret = LoadMorph();
			break;
		case SHDPOLYLINE:
			ret = LoadPolyLine();
			break;
		case SHDPOLYGON:
			ret = LoadPolygon();
			break;
		case SHDSPHERE:
			ret = LoadSphere();
			break;
		case SHDBEZIERSURF:
			ret = LoadBezMesh();
			break;
		case SHDMESHES:
			ret = LoadRevolved();
			break;
		case SHDDISK:
			ret = LoadDisk();
			break;
		case SHDBEZLINE:	// CBezLine
			ret = LoadBezLine();
			break;
		case SHDEXTRUDE:
			ret = LoadExtrude();
			break;
		case SHDPOLYMESH:
			ret = LoadPolyMesh( sh.flags, sh.flags2 );
			break;
		case SHDPOLYMESH2:
			ret = LoadPolyMesh2( m_adjustuvflag, sh.flags, sh.flags2 );
			break;
		case SHDINFSCOPE:
			ret = LoadInfScope( sh.dispswitchno, sh.flags2 );
			break;
		case SHDTYPENONE:
		case SHDTYPEERROR:
		case SHDTYPEMAX:
			// error
			ret = 1;
			break;

		case -1:
			//！！！　終わりの目印。
			isend = 1;
			break;

		case SHDGROUNDDATA:
			ret = LoadGroundData();
			break;


		case SHDDESTROYED:
		default:
			// 未対応
			ret = 0;
			break;
		}
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = lpsh->SetChain( offset );
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : sh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = lpsh->SetColors();
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : sh SetColors error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = lpmh->SetChain( offset );
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : mh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->InfScopeConvTempApplyChild();
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : sh InfScopeConvTempApplyChild error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = lpsh->SetMikoAnchorApply( lpth );
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : sh SetMikoAnchorApply error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->Anchor2InfScope( lpth, lpmh );
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : sh Anchor2InfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->CreateShdBBoxIfNot( lpth, lpmh );
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : sh CreateShdBBoxIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->ConvMorphElem();
	if( ret ){
		DbgOut( "sigfile : LoadSigFile : sh ConvMorphElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


int CSigFile::LoadInfluenceList( int listleng, int offset )
{
	int* templist;
	templist = (int*)malloc( sizeof( int ) * listleng );
	if( !templist ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = LoadIntData( templist, listleng );
	if( ret ){
		_ASSERT( 0 );
		free( templist );
		return 1;
	}

	/***
	int listno;
	for( listno = 0; listno < listleng; listno++ ){
DbgOut( "SigFile : LoadInfluenceList : list %d - bno %d\n", listno, *(templist + listno) );

	}
	***/


	if( offset == 0 ){
		ret = lpsh->SetInfluenceList( curseri, templist, listleng );
		if( ret ){
			_ASSERT( 0 );
			free( templist );
			return 1;
		}
		(*lpsh)( curseri )->neededconvert = 1;
	}else{
		int bnooffset;// = lpsh->m_lastboneno;
		if( loadmagicno >= SIGFILEMAGICNO_5 ){
			bnooffset = lpsh->m_lastboneno;
		}else{
			bnooffset = lpsh->m_lastoldboneno;
		}
		int listno;
		for( listno = 0; listno < listleng; listno++ ){
			*(templist + listno) += bnooffset;
		}

		ret = lpsh->SetInfluenceList( curseri, templist, listleng );
		if( ret ){
			_ASSERT( 0 );
			free( templist );
			return 1;
		}
		(*lpsh)( curseri )->neededconvert = 1;

	}

	free( templist );

	return 0;
}

int CSigFile::LoadIgnoreList( int listleng, int offset )
{
	int* templist;
	templist = (int*)malloc( sizeof( int ) * listleng );
	if( !templist ){
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = LoadIntData( templist, listleng );
	if( ret ){
		_ASSERT( 0 );
		free( templist );
		return 1;
	}

	/***
	int listno;
	for( listno = 0; listno < listleng; listno++ ){
DbgOut( "SigFile : LoadInfluenceList : list %d - bno %d\n", listno, *(templist + listno) );

	}
	***/


	if( offset == 0 ){
		ret = lpsh->SetIgnoreList( curseri, templist, listleng );
		if( ret ){
			_ASSERT( 0 );
			free( templist );
			return 1;
		}
		(*lpsh)( curseri )->neededconvert_ig = 1;
	}else{
		int bnooffset;// = lpsh->m_lastboneno;
		if( loadmagicno >= SIGFILEMAGICNO_5 ){
			bnooffset = lpsh->m_lastboneno;
		}else{
			bnooffset = lpsh->m_lastoldboneno;
		}

		int listno;
		for( listno = 0; listno < listleng; listno++ ){
			*(templist + listno) += bnooffset;
		}

		ret = lpsh->SetIgnoreList( curseri, templist, listleng );
		if( ret ){
			_ASSERT( 0 );
			free( templist );
			return 1;
		}
		(*lpsh)( curseri )->neededconvert_ig = 1;

	}

	free( templist );

	return 0;
}



int CSigFile::AddShape2Tree( SIGHEADER sheader )
{
	int diffdepth, endcnt;
	int ret, i, chkdepth;
	diffdepth = curdepth - befdepth;

	ret = tempinfo.ResetParams();
	_ASSERT( !ret );

	if( diffdepth <= 0 ){
		endcnt = -diffdepth + 1;
		for( i = 0; i < endcnt; i++ ){
			ret = lpth->End();
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	ret = lpth->Begin();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	chkdepth = lpth->GetCurDepth();
	if( chkdepth != curdepth ){
		_ASSERT( 0 );
		return 1;
	}

	ret = lpth->AddTree( sheader.name, curseri );
	if( curseri != ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = lpth->SetElemType( curseri, curshdtype );
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
	
	ret = lpsh->SetClockwise( curseri, sheader.clockwise );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->SetBDivUV( curseri, sheader.bdivu, sheader.bdivv );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int cmp;
	cmp = strcmp( sheader.texname, "0" );
	if( cmp ){
		ret = lpsh->SetTexName( curseri, sheader.texname );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = (*lpsh)( curseri )->SetTexRepetition( sheader.texrepx, sheader.texrepy );
	_ASSERT( !ret );

	ret = (*lpsh)( curseri )->SetTexTransparent( sheader.transparent );
	_ASSERT( !ret );

	ret = (*lpsh)( curseri )->SetUVAnime( sheader.uanime, sheader.vanime );
	_ASSERT( !ret );

	ret = (*lpsh)( curseri )->SetAlpha( sheader.alpha );
	_ASSERT( !ret );

	ret = (*lpsh)( curseri )->SetTexRule( sheader.texrule );
	_ASSERT( !ret );

	ret = (*lpsh)( curseri )->SetNotUse( sheader.notuse );
	_ASSERT( !ret );


	if( sheader.flags & FLAGS_NOSKINNING ){
		ret = (*lpsh)( curseri )->SetNoSkinning( 1 );
		_ASSERT( !ret );
	}else{
		ret = (*lpsh)( curseri )->SetNoSkinning( 0 );
		_ASSERT( !ret );
	}

	if( curshdtype != SHDINFSCOPE ){
		ret = (*lpsh)( curseri )->SetDispSwitchNo( sheader.dispswitchno );
		_ASSERT( !ret );
	}else{
		ret = (*lpsh)( curseri )->SetDispSwitchNo( 0 );
		_ASSERT( !ret );

		(*lpsh)( curseri )->scopenum = 0;//!!!!!!!!!!!!!
	}


	if( sheader.flags & FLAGS_NOTRANSIK ){
		(*lpsh)( curseri )->m_notransik = 1;
	}else{
		(*lpsh)( curseri )->m_notransik = 0;
	}
	if( sheader.flags3 & FLAGS3_IKSKIP ){
		(*lpsh)( curseri )->m_ikskip = 1;
	}else{
		(*lpsh)( curseri )->m_ikskip = 0;
	}
	if( sheader.flags3 & FLAGS3_INVISIBLEFLAG ){
		(*lpsh)( curseri )->invisibleflag = 1;
	}else{
		(*lpsh)( curseri )->invisibleflag = 0;
	}

	if( sheader.flags & FLAGS_ANCHORFLAG ){
		(*lpsh)( curseri )->m_anchorflag = 1;
	}else{
		(*lpsh)( curseri )->m_anchorflag = 0;
	}
	(*lpsh)( curseri )->m_mikodef = sheader.mikodef;


	(*lpsh)( curseri )->m_userint1 = sheader.userint1;//!!!!!!!!!!!!

	if( sheader.iklevel > 0 ){
		(*lpsh)( curseri )->m_iklevel = sheader.iklevel;
	}else{
		(*lpsh)( curseri )->m_iklevel = 5;
	}

	if( sheader.facetm180 != 0.0f ){
		(*lpsh)( curseri )->facet = sheader.facetm180 + 180.0f;
	}else{
		(*lpsh)( curseri )->facet = 180.0f;
	}

	if( curshdtype == SHDINFSCOPE ){
		(*lpsh)( curseri )->alpha = 0.5f;
	}


// mothandler
	ret = lpmh->AddMotionCtrl( curseri, curdepth );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	
	ret = lpmh->SetHasMotion( curseri, sheader.hasmotion );
	_ASSERT( !ret );

	return 0;

}

int CSigFile::LoadPart( unsigned char flags2 )
{
	int ret;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	VEC3F jvec;
	ret = LoadVecData( &jvec, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//jvec.x = jvec.x * m_datamult + m_offsetpos.x;//!!!!!!!!
	//jvec.y = jvec.y * m_datamult + m_offsetpos.y;//!!!!!!!!
	//jvec.z = jvec.z * m_datamult + m_offsetpos.z;//!!!!!!!!

	D3DXVECTOR3 befv, aftv;
	befv.x = jvec.x;
	befv.y = jvec.y;
	befv.z = jvec.z;

	D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
	jvec.x = aftv.x;
	jvec.y = aftv.y;
	jvec.z = aftv.z;


	CVec3f dstvec;
	dstvec.SetMem( jvec, VEC_ALL );

	CShdElem* curelem;
	curelem = (*lpsh)(curseri);
	ret = curelem->SetJointLoc( &dstvec );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	if( flags2 & FLAGS2_BONELIM01 ){
		int ignorelim01;
		ret = LoadIntData( &ignorelim01, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		DVEC3 lim0, lim1;
		ret = LoadUCharData( (unsigned char*)&lim0, sizeof( DVEC3 ) );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadUCharData( (unsigned char*)&lim1, sizeof( DVEC3 ) );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = curelem->SetIgnoreLim( ignorelim01 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = curelem->SetBoneLim01( &lim0, &lim1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	
	if( loadmagicno >= SIGFILEMAGICNO_7 ){
		ret = LoadIntData( &(curelem->m_notsel), 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	if( loadmagicno >= SIGFILEMAGICNO_9 ){
		ret = LoadIntData( &(curelem->m_setmikobonepos), 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadUCharData( (unsigned char*)&(curelem->m_mikobonepos), sizeof( D3DXVECTOR3 ) );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		if( curelem->m_setmikobonepos != 2 ){
			D3DXVECTOR3 befmpos, aftmpos;
			befmpos = curelem->m_mikobonepos;
			D3DXVec3TransformCoord( &aftmpos, &befmpos, &m_offsetmat );
			curelem->m_mikobonepos = aftmpos;
		}
	}

	if( loadmagicno >= SIGFILEMAGICNO_10 ){
		int za4type, za4rotaxis;
		CQuaternion za4q;

		ret = LoadIntData( &za4type, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadIntData( &za4rotaxis, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadUCharData( (unsigned char*)&za4q, sizeof( CQuaternion ) );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = curelem->SetZa4Type( za4type );
		_ASSERT( !ret );
		ret = curelem->SetZa4RotAxis( za4rotaxis );
		_ASSERT( !ret );
		ret = curelem->SetZa4Q( za4q );
		_ASSERT( !ret );


		ret = LoadIntData( &curelem->m_iktarget, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadIntData( &curelem->m_iktargetlevel, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadIntData( &curelem->m_iktargetcnt, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadUCharData( (unsigned char*)&(curelem->m_iktargetpos), sizeof( D3DXVECTOR3 ) );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}
int CSigFile::LoadMorph()
{
	int ret;
	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( loadmagicno >= SIGFILEMAGICNO_11 ){

		CShdElem* selem;
		selem = (*lpsh)( curseri );
		_ASSERT( selem );

		CMorph* morph;
		morph = selem->morph;
		_ASSERT( morph );

		int baseseri;
		ret = LoadIntData( &baseseri, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		baseseri += m_offset;

		int boneseri;
		ret = LoadIntData( &boneseri, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
		boneseri += m_offset;

		ret = morph->SetBaseParams( lpsh, lpmh, baseseri, boneseri );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		int targetnum;
		ret = LoadIntData( &targetnum, 1 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		int tno;
		for( tno = 0; tno < targetnum; tno++ ){
			int tarseri;
			ret = LoadIntData( &tarseri, 1 );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
			tarseri += m_offset;

			ret = morph->AddMorphTarget( tarseri );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	return 0;
}

int CSigFile::LoadPolyLine()
{
	int ret;
	VEC3F* vec = 0;
	int vnum;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	vnum = tempinfo.total;
	vec = (VEC3F*)malloc( sizeof( VEC3F ) * vnum );
	if( !vec ){
		_ASSERT( 0 );
		return 1;
	}

	ret = LoadVecData( vec, vnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//if( m_datamult != 1.0f )
	{
		int vno;
		VEC3F* curv;
		for( vno = 0; vno < vnum; vno++ ){
			curv = vec + vno;

			D3DXVECTOR3 befv, aftv;
			befv.x = curv->x;
			befv.y = curv->y;
			befv.z = curv->z;

			D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
			curv->x = aftv.x;
			curv->y = aftv.y;
			curv->z = aftv.z;
		}
	}
	

	ret = (*lpsh)( curseri )->SetVecLine( vec, vnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	free( vec );

	return 0;
}
int CSigFile::LoadPolygon()
{
	int ret;
	VEC3F* vec = 0;
	int polynum, polyno, vnum;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	polynum = tempinfo.n; // tempinfo.totalではない！！！！
	//Add2CurPolygonは２ラインAddする。
	for( polyno = 0; polyno < polynum; polyno++ ){
		ret = LoadMIHeader( &tempinfo );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		ret = (*lpsh)( curseri )->Add2CurPolygon( &tempinfo );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		vnum = tempinfo.total;
		vec = (VEC3F*)malloc( sizeof( VEC3F ) * vnum );
		if( !vec ){
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadVecData( vec, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		//if( m_datamult != 1.0f )
		{
			int vno;
			VEC3F* curv;
			for( vno = 0; vno < vnum; vno++ ){
				curv = vec + vno;

				D3DXVECTOR3 befv, aftv;
				befv.x = curv->x;
				befv.y = curv->y;
				befv.z = curv->z;

				D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
				curv->x = aftv.x;
				curv->y = aftv.y;
				curv->z = aftv.z;
			}
		}

		ret = (*lpsh)( curseri )->SetPolygon( vec, polyno, LIM2_POS, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		free( vec );
	}

	return 0;
}
int CSigFile::LoadSphere()
{

	return 0;
}
int CSigFile::LoadBezMesh()
{
	int ret;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	bezlinenum = tempinfo.n;
	bezlineno = 0;
	bezsurfno = curseri;

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSigFile::LoadBezLine()
{
	int ret;
	CMeshInfo vlinfo;
	int vlno, vlnum, vnum;
	VEC3F* vec = 0;

	_ASSERT( bezlinenum > bezlineno );

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	vlnum = tempinfo.total;

	vlinfo.ResetParams();
	ret = LoadMIHeader( &vlinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	vnum = vlinfo.total;

	tempinfo.m = vnum;
	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->SetBMesh( bezsurfno, curseri, bezlineno );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//ret = (*lpsh)( curseri )->SetBMesh( tempinfo.skip, MESHI_SKIP );
	//_ASSERT( !ret );

	vec = (VEC3F*)malloc( sizeof( VEC3F ) * vnum );
	if( !vec ){
		_ASSERT( 0 );
		return 1;
	}

	for( vlno = 0; vlno < vlnum; vlno++ ){
		if( vlno != 0 ){
			ret = LoadMIHeader( &vlinfo );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		ret = LoadVecData( vec, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		//if( m_datamult != 1.0f )
		{
			int vno;
			VEC3F* curv;
			for( vno = 0; vno < vnum; vno++ ){
				curv = vec + vno;

				D3DXVECTOR3 befv, aftv;
				befv.x = curv->x;
				befv.y = curv->y;
				befv.z = curv->z;

				D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
				curv->x = aftv.x;
				curv->y = aftv.y;
				curv->z = aftv.z;
			}
		}

		ret = (*lpsh)( curseri )->SetBLine( vec, vlno, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}
	
	free( vec );

	bezlineno++;

	return 0;

}
int CSigFile::LoadRevolved()
{
	int ret;
	CMatrix2 axis;
	CMeshInfo vlinfo;
	int vlno, vlnum, vnum;
	VEC3F* vec = 0;
	REVINFO ri;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	vlinfo.ResetParams();
	ret = LoadMIHeader( &vlinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	tempinfo.m = vlinfo.m;//!!!!

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	vnum = vlinfo.m;
	vlnum = tempinfo.total;
	vec = (VEC3F*)malloc( sizeof( VEC3F ) * vnum );
	if( !vec ){
		_ASSERT( 0 );
		return 0;
	}

	for( vlno = 0; vlno < vlnum; vlno++ ){
		if( vlno != 0 ){
			//初回は上で。
			ret = LoadMIHeader( &vlinfo );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		
		ZeroMemory( vec, sizeof( VEC3F ) * vnum );
		ret = LoadVecData( vec, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		//if( m_datamult != 1.0f )
		{
			int vno;
			VEC3F* curv;
			for( vno = 0; vno < vnum; vno++ ){
				curv = vec + vno;

				D3DXVECTOR3 befv, aftv;
				befv.x = curv->x;
				befv.y = curv->y;
				befv.z = curv->z;

				D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
				curv->x = aftv.x;
				curv->y = aftv.y;
				curv->z = aftv.z;

			}
		}


		ret = (*lpsh)( curseri )->SetRevolved( vec, vlno, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}
	free( vec );

	//revinfo
	ZeroMemory( &ri, sizeof( REVINFO ) );
	DWORD rleng, readleng;
	rleng = sizeof( REVINFO );
	ReadFile( hfile, (void*)(&ri), rleng, &readleng, NULL );
	if( rleng != readleng ){
		_ASSERT( 0 );
		return 1;
	}
	ret = (*lpsh)( curseri )->SetRevolved( &(ri.radfrom), REV_FROM );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = (*lpsh)( curseri )->SetRevolved( &(ri.radto), REV_TO );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = (*lpsh)( curseri )->SetRevolved( &(ri.isnormalset), REV_NSET );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	//axis
	ret = LoadMatData( &axis, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = (*lpsh)( curseri )->SetRevolved( &axis, REV_AXIS );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CSigFile::LoadDisk()
{
	int ret;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;

}
int CSigFile::LoadExtrude()
{
	int ret;
	CMeshInfo vlinfo;
	int vlnum, vlno, vnum;
	VEC3F* vec = 0;
	VEC3F exvec;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	vlnum = tempinfo.total;

	vlinfo.ResetParams();
	ret = LoadMIHeader( &vlinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	vnum = vlinfo.total;

	tempinfo.m = vnum;
	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	vec = (VEC3F*)malloc( sizeof( VEC3F ) * vnum );
	if( !vec ){
		_ASSERT( 0 );
		return 1;
	}

	for( vlno = 0; vlno < vlnum; vlno++ ){
		if( vlno != 0 ){
			ret = LoadMIHeader( &vlinfo );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}
		}
		
		ret = LoadVecData( vec, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

		//if( m_datamult != 1.0f )
		{
			int vno;
			VEC3F* curv;
			for( vno = 0; vno < vnum; vno++ ){
				curv = vec + vno;

				D3DXVECTOR3 befv, aftv;
				befv.x = curv->x;
				befv.y = curv->y;
				befv.z = curv->z;

				D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
				curv->x = aftv.x;
				curv->y = aftv.y;
				curv->z = aftv.z;
			}
		}

		ret = (*lpsh)( curseri )->SetExtrude( vec, vlno, vnum );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}
	}
	free( vec );

	// exvec
	ret = LoadVecData( &exvec, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	ret = (*lpsh)( curseri )->SetExtrude( exvec );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CSigFile::LoadInfScope( int scnum, unsigned char flags2 )
{
	int ret;

	CShdElem* curselem;
	curselem = (*lpsh)( curseri );
	_ASSERT( curselem );


	//importしたときは、SHDINFSCOPEが複数作られる場合がある。
	//その場合は、一番最初のINFSCOPEに、形状を追加する。

	CShdElem* iselem;
	iselem = lpsh->GetInfScopeElem( m_offset );
	if( !iselem ){
		iselem = curselem;

		DbgOut( "sigfile : LoadInfScope : not found infscope before : IS%d\n", iselem->serialno );

	}
	if( iselem != curselem ){
		//curselem は、SHDDESTROYEDにする。

		curselem->type = SHDDESTROYED;
		(*lpth)( curseri )->type = SHDDESTROYED;
		(*lpmh)( curseri )->type = SHDDESTROYED;
		
		DbgOut( "sigfile : LoadInfScope : find infscope before : IS%d CUR%d\n", iselem->serialno, curselem->serialno );
	}



	int scno;
	for( scno = 0; scno < scnum; scno++ ){
		SIGHEADER sh;
		ZeroMemory( &sh, sizeof( SIGHEADER ) );
		ret = LoadUCharData( (unsigned char*)&sh, sizeof( SIGHEADER ) );
		if( ret ){
			DbgOut( "sigfile : LoadInfScope : Load sh error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		if( sh.type == SHDPOLYMESH ){

			ret = LoadMIHeader( &tempinfo );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}

			CPolyMesh* pm;
			pm = new CPolyMesh();
			if( !pm ){
				DbgOut( "sigfile : LoadInfScope : polymesh alloc error !!!\n" );
				return 1;
			}


			ret = iselem->AddInfScope( pm, 0 );
			if( ret ){
				DbgOut( "sigfile : LoadInfScope : AddInfScope error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm->CreatePointBuf( &tempinfo, 0.5f );
			if( ret ){
				DbgOut( "sigfile : LoadInfScope : CreatePointBuf error !!!\n" );
				return 1;
			}


			unsigned char flags;
			flags = FLAGS_ORGNOBUF | FLAGS_PMUVBUF | FLAGS_INFELEM;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			ret = LoadPolyMesh_aft( iselem, pm, flags, flags2 );
			if( ret ){
				DbgOut( "sigfile : LoadInfScope : LoadPolyMesh_aft error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			///////
			pm->tempapplychild = sh.texrepx + m_offset;
			pm->temptarget = sh.texrepy + m_offset;


			DbgOut( "sigfile : LoadInfScope : pm : AddInfScope %d %d %d\n", pm->tempapplychild, pm->temptarget, m_offset );

		}else if( sh.type == SHDPOLYMESH2 ){

			ret = LoadMIHeader( &tempinfo );
			if( ret ){
				_ASSERT( 0 );
				return 1;
			}

			CPolyMesh2* pm2;
			pm2 = new CPolyMesh2();
			if( !pm2 ){
				DbgOut( "sigfile : LoadInfScope : polymesh alloc error !!!\n" );
				return 1;
			}

			ret = iselem->AddInfScope( pm2, 0, sh.facetm180 );
			if( ret ){
				DbgOut( "sigfile : LoadInfScope : AddInfScope error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}


			ret = pm2->CreateBuffers( &tempinfo );
			if( ret ){
				DbgOut( "sigfile : LoadInfScope : CreateBuffers error !!!\n" );
				return 1;
			}

			unsigned char flags;
			flags = FLAGS_ORGNOBUF | FLAGS_INFELEM;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			ret = LoadPolyMesh2_aft( iselem, pm2, 0, flags, flags2, sh.facetm180 );
			if( ret ){
				DbgOut( "sigfile : LoadInfScope : LoadPolyMesh2_aft error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			///////
			pm2->tempapplychild = sh.texrepx + m_offset;
			pm2->temptarget = sh.texrepy + m_offset;

			DbgOut( "sigfile : LoadInfScope : pm2 : AddInfScope %d %d %d\n", pm2->tempapplychild, pm2->temptarget, m_offset );

		}else{
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CSigFile::LoadPolyMesh2( int adjustuvflag, unsigned char flags, unsigned char flags2 )
{

	int ret;

DbgOut( "sigfile : start LoadPolyMesh2\n" );
 
	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int facenum;
	facenum = tempinfo.n;

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* curselem;
	curselem = (*lpsh)( curseri );
	_ASSERT( curselem );

	CPolyMesh2* pm2;
	pm2 = curselem->polymesh2;
	_ASSERT( pm2 );

	pm2->groundflag = m_groundflag;

	if( flags & FLAGS_INFELEM ){
		curselem->m_loadbimflag = 2;//!!!!!!!!!!!
	}

	ret = LoadPolyMesh2_aft( curselem, pm2, adjustuvflag, flags, flags2, curselem->facet );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : LoadPolyMesh2_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


DbgOut( "sigfile : end LoadPolyMesh2\n" );

	return 0;

}

int CSigFile::LoadPolyMesh2_aft( CShdElem* selem, CPolyMesh2* pm2, int adjustuvflag, unsigned char flags, unsigned char flags2, float facet )
{
	int ret;

	int facenum;
	facenum = tempinfo.n;


	pm2->adjustuvflag = adjustuvflag;

	VEC3F* dstvec = pm2->pointbuf;
	ret = LoadVecData( dstvec, facenum * 3 );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : LoadVecData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	
	
	//後で、pm2->MultMatで掛ける。先に掛けると、誤差で、optplengが異なることがある。


	int* dstsamep = pm2->samepointbuf;
	ret = LoadIntData( dstsamep, facenum * 3 );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : LoadIntData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( flags2 & FLAGS2_OLD2OPT ){
		int* dstold2opt = pm2->oldpno2optpno;
		ret = LoadIntData( dstold2opt, facenum * 3 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load old2opt error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( flags2 & FLAGS2_SMOOTHBUF ){
		int* dstsmooth = pm2->smoothbuf;
		ret = LoadIntData( dstsmooth, facenum * 3 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load smooth error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	unsigned char* dstuv = (unsigned char*)pm2->uvbuf;
	ret = LoadUCharData( dstuv, sizeof( COORDINATE ) * facenum * 3 );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////
	unsigned char* dstdiffuse = (unsigned char*)pm2->diffusebuf;
	ret = LoadUCharData( dstdiffuse, sizeof( ARGBF ) * facenum );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* dstambient = (unsigned char*)pm2->ambientbuf;
	ret = LoadUCharData( dstambient, sizeof( ARGBF ) * facenum );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* dstspecular = (unsigned char*)pm2->specularbuf;
	ret = LoadUCharData( dstspecular, sizeof( float ) * facenum );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( flags2 & FLAGS2_ORGNORMAL ){
		unsigned char* dstorgnormal = (unsigned char*)pm2->orgnormal;
		ret = LoadUCharData( dstorgnormal, sizeof( VEC3F ) * facenum * 3 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load orgnormal error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		pm2->m_loadorgnormalflag = 1;//!!!!!!!!!!!!!!!!!!!!!!
	}

//////////

	int optpleng = 0;

	if( loadmagicno >= SIGFILEMAGICNO_2 ){
		
		//optデータを作る
		if( flags2 & FLAGS2_OLD2OPT ){
			
			int calcsmooth = 0;
			if( flags2 & FLAGS2_SMOOTHBUF )
				calcsmooth = 0;
			else
				calcsmooth = 1;

			ret = pm2->CreateOptDataByOld2Opt( adjustuvflag, facet, calcsmooth );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh2 : CreateOptData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = pm2->CreateOptDataBySamePointBufLegacy( adjustuvflag, facet );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh2 : CreateOptData by samepointbuf error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
		//optplengの読み出し。
		ret = LoadIntData( &optpleng, 1 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : LoadIntData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//optplengのチェック
		if( optpleng != pm2->optpleng ){
			DbgOut( "sigfile : LoadPolyMesh2 : optpleng error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// バッファの作成
		DWORD* tempdw;
		tempdw = (DWORD*)malloc( sizeof( DWORD ) * optpleng );
		if( !tempdw ){
			DbgOut( "sigfile : LoadPolyMesh2 : tempdw alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// diffuseの読み込み
		ret = LoadDWData( tempdw, optpleng );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : LoadDWData error !!!\n" );
			_ASSERT( 0 );
			free( tempdw );
			return 1;
		}
		int optno;
		for( optno = 0; optno < optpleng; optno++ ){
			(pm2->opttlv + optno)->color = *(tempdw + optno);
		}

		//ambientの読み込み
		ret = LoadARGBData( pm2->optambient, optpleng );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : LoadARGBData error !!!\n" );
			_ASSERT( 0 );
			free( tempdw );
			return 1;
		}

		//specularの読み込み
		ret = LoadDWData( tempdw, optpleng );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : LoadDWData error !!!\n" );
			_ASSERT( 0 );
			free( tempdw );
			return 1;
		}
		for( optno = 0; optno < optpleng; optno++ ){
			(pm2->opttlv + optno)->specular = *(tempdw + optno);
		}


		free( tempdw );
		tempdw = 0;
	}


	if( loadmagicno >= SIGFILEMAGICNO_4 ){
		ret = LoadFloatData( pm2->diffbuf, facenum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load diffbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadFloatData( pm2->powerbuf, facenum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadFloatData( pm2->emissivebuf, facenum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load emissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		ret = LoadFloatData( pm2->optpowerbuf, optpleng );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm2->optemissivebuf, optpleng );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load optemissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	if( flags & FLAGS_ORGNOBUF ){
		int* dstorgno;
		dstorgno = pm2->orgnobuf;
		if( !dstorgno ){
			DbgOut( "sigfile : LoadPolyMesh2 : pm2 orgnobuf not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadIntData( dstorgno, facenum * 3 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : LoadIntData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = pm2->MultMat( m_offsetmat );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh2 : pm2 MultMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	if( flags & FLAGS_INFELEM ){
		ret = pm2->CreateInfElemIfNot( optpleng );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : CreateInfElemIfNot error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( pm2->m_IE );
		INFELEMHEADER ieheader;
		SIGINFELEM sigie;
		CInfElem* ieptr;
		int pno;
		for( pno = 0; pno < optpleng; pno++ ){
			ieptr = pm2->m_IE + pno;
			_ASSERT( ieptr );
		
			ret = LoadUCharData( (unsigned char*)&ieheader, sizeof( INFELEMHEADER ) );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh2 : Load ieheader error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int infnum;
			infnum = ieheader.infnum;
			ieptr->normalizeflag = ieheader.normalizeflag;
			if( ieheader.symaxis != SYMAXIS_NONE ){
				ieptr->symaxis = ieheader.symaxis;
				ieptr->symdist = ieheader.symdist;
			}else{
				ieptr->symaxis = SYMAXIS_X;
				ieptr->symdist = 100.0f;
			}

			int infno;
			for( infno = 0; infno < infnum; infno++ ){
				ret = LoadUCharData( (unsigned char*)&sigie, sizeof( SIGINFELEM ) );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh2 : Load sigie error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				INFELEM addie;
				ZeroMemory( &addie, sizeof( INFELEM ) );

				if( loadmagicno >= SIGFILEMAGICNO_5 ){
					addie.bonematno = sigie.bonematno + m_offset;
				}else{
					if( sigie.childno > 0 ){
						addie.bonematno = sigie.childno + m_offset;
					}else{
						addie.bonematno = sigie.bonematno + m_offset;
					}
				}

				addie.childno = sigie.childno + m_offset;//!!!!!!!!!!!!!!!!!!
				addie.kind = sigie.kind;
				addie.userrate = sigie.userrate;
				addie.orginf = sigie.orginf;
				addie.dispinf = sigie.dispinf;

				ret = ieptr->AddInfElem( addie );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh2 : AddInfElem error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}


	if( flags2 & FLAGS2_VCOL ){
		int vcolnum;
		ret = LoadIntData( &vcolnum, 1 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : Load vcolnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int vcno;
		VCOLDATA curvc;
		for( vcno = 0; vcno < vcolnum; vcno++ ){
			ret = LoadUCharData( (unsigned char*)&curvc, sizeof( VCOLDATA ) );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh2 : load curvc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm2->SetVCol( curvc.vertno, curvc.vcol );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh2 : pm2 SetVCol error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}

	if( flags2 & FLAGS2_TOON1 ){
		ret = LoadIntData( &pm2->m_materialnum, 1 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh2 : load materialnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		pm2->m_toon1 = new CToon1Params[ pm2->m_materialnum ];
		if( !pm2->m_toon1 ){
			DbgOut( "sigfile : LoadPolyMesh2 : m_toon1 alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int matno;
		for( matno = 0; matno < pm2->m_materialnum; matno++ ){

			SIGTOON1EX* sigtoon1exptr = 0;
			SIGTOON1* sigtoon1ptr = 0;

			SIGTOON1 sigtoon1;
			ZeroMemory( &sigtoon1, sizeof( SIGTOON1 ) );
			SIGTOON1EX sigtoon1ex;
			ZeroMemory( &sigtoon1ex, sizeof( SIGTOON1EX ) );

			if( loadmagicno >= SIGFILEMAGICNO_13 ){
				ret = LoadUCharData( (unsigned char*)&sigtoon1ex, sizeof( SIGTOON1EX ) );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh2 : load sigtoon1ex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sigtoon1ptr = &(sigtoon1ex.toon1);
				sigtoon1exptr = &sigtoon1ex;
			}else{
				ret = LoadUCharData( (unsigned char*)&sigtoon1, sizeof( SIGTOON1 ) );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh2 : load sigtoon1 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sigtoon1ptr = &sigtoon1;
				sigtoon1exptr = 0;
			}

			CToon1Params* curtoon1;
			curtoon1 = pm2->m_toon1 + matno;

			strcpy_s( curtoon1->name, 32, sigtoon1ptr->name );
			curtoon1->diffuse = sigtoon1ptr->diffuse;
			curtoon1->ambient = sigtoon1ptr->ambient;
			curtoon1->specular = sigtoon1ptr->specular;
			curtoon1->darkh = sigtoon1ptr->darkh;
			curtoon1->brighth = sigtoon1ptr->brighth;
			curtoon1->ambientv = sigtoon1ptr->ambientv;
			curtoon1->diffusev = sigtoon1ptr->diffusev;
			curtoon1->specularv = sigtoon1ptr->specularv;
			curtoon1->darknl = sigtoon1ptr->darknl;
			curtoon1->brightnl = sigtoon1ptr->brightnl;
			
			curtoon1->tex = 0;

			if( flags2 & FLAGS2_EDGE0 ){
				curtoon1->edgecol0 = sigtoon1ptr->edgecol0;
				curtoon1->edgevalid0 = sigtoon1ptr->edgevalid0;
				curtoon1->edgeinv0 = sigtoon1ptr->edgeinv0;
			}else{
				curtoon1->edgecol0.r = sigtoon1ptr->ambient.r * 0.6f;
				curtoon1->edgecol0.g = sigtoon1ptr->ambient.g * 0.6f;
				curtoon1->edgecol0.b = sigtoon1ptr->ambient.b * 0.6f;
				curtoon1->edgecol0.a = 8.0f;

				curtoon1->edgevalid0 = 1;
				curtoon1->edgeinv0 = 0;
			}

			if( sigtoon1exptr ){
				curtoon1->toon0dnl = sigtoon1exptr->toon0dnl;
				curtoon1->toon0bnl = sigtoon1exptr->toon0bnl;
			}else{
				curtoon1->toon0dnl = sigtoon1ptr->darknl;
				curtoon1->toon0bnl = sigtoon1ptr->brightnl;
			}

		}

		pm2->m_attrib = new DWORD[ facenum ];
		if( !pm2->m_attrib ){
			DbgOut( "sigfile : LoadPolyMesh2 : attrib alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadUCharData( (unsigned char*)pm2->m_attrib, sizeof( DWORD ) * facenum );
		if( ret ){
			DbgOut( "sigfile : loadPolyMesh2 : attrib load error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}
	if( selem->type != SHDINFSCOPE ){
		if( flags2 & FLAGS2_MQOMATERIAL ){
			if( !pm2->m_attrib0 ){
				DbgOut( "sigfile : LoadPolyMesh2 : attrib0 NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = LoadIntData( pm2->m_attrib0, facenum );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh2 : pm2 attrib0 load error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int fno;
			for( fno = 0; fno < facenum; fno++ ){
				int orgattr = *( pm2->m_attrib0 + fno );
				std::map<int, int>::iterator finditr;
				finditr = m_newmatindex.find( orgattr );
				if( finditr == m_newmatindex.end() ){
					*( pm2->m_attrib0 + fno ) += m_materialoffset;//!!!!!!!!!
				}else{
					*( pm2->m_attrib0 + fno ) = finditr->second;
				}

			}
		}else{
			CMQOMaterial* newmat = 0;
			newmat = AddMQOMaterial( selem );
			if( !newmat ){
				DbgOut( "sigfile : LoadPolyMesh2 : AddMQOMaterial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int faceno;
			for( faceno = 0; faceno < facenum; faceno++ ){
				*( pm2->m_attrib0 + faceno ) = newmat->materialno;
			}

		}
	}else{
		//INFSCOPEの中のpm2にはattrib0なし。
	}

	return 0;

}


int CSigFile::LoadPolyMesh( unsigned int flags, unsigned int flags2 )
{

	int ret;

DbgOut( "sigfile : start LoadPolyMesh\n" );
 
	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Init3DObj();
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh : Init3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CShdElem* curselem;
	curselem = (*lpsh)( curseri );
	_ASSERT( curselem );


	CPolyMesh* pm;	
	pm = curselem->polymesh;
	_ASSERT( pm );

	pm->groundflag = m_groundflag;

	if( flags & FLAGS_PMUVBUF ){
		curselem->texrule = TEXRULE_MQ;
	}

	if( flags & FLAGS_INFELEM ){
		curselem->m_loadbimflag = 2;//!!!!!!!!!!!
	}

	ret = LoadPolyMesh_aft( curselem, pm, flags, flags2 );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh : LoadPolyMesh_aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

DbgOut( "sigfile : end LoadPolyMesh\n" );

	return 0;
}

int CSigFile::LoadPolyMesh_aft( CShdElem* selem, CPolyMesh* pm, unsigned char flags, unsigned char flags2 )
{

	int ret;

	int pointnum;
	pointnum = tempinfo.m;

	VEC3F* ldvec;
	ldvec = (VEC3F*)malloc( sizeof( VEC3F ) * pointnum );
	if( !ldvec ){
		DbgOut( "sigfile : LoadPolyMesh : ldvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = LoadVecData( ldvec, pointnum );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh : LoadVecData error !!!\n" );
		_ASSERT( 0 );
		free( ldvec );
		return 1;
	}


	//if( m_datamult != 1.0f )
	{
		int vno;
		VEC3F* curv;
		for( vno = 0; vno < pointnum; vno++ ){
			curv = ldvec + vno;

			D3DXVECTOR3 befv, aftv;
			befv.x = curv->x;
			befv.y = curv->y;
			befv.z = curv->z;

			D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
			curv->x = aftv.x;
			curv->y = aftv.y;
			curv->z = aftv.z;

		}
	}

	ret = pm->SetPointBuf( pointnum, ldvec );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh : SetPolyMeshPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	free( ldvec );

	
	int* ldint;
	int facenum, dataleng;
	facenum = tempinfo.n;
	dataleng = facenum * 3;

	ret = pm->CreateIndexBuf( facenum );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh : CreatePolyMeshIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ldint = (int*)malloc( sizeof( int ) * dataleng );
	if( !ldint ){
		DbgOut( "sigfile : LoadPolyMesh : ldint alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = LoadIntData( ldint, dataleng );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh : ldint alloc error !!!\n" );
		_ASSERT( 0 );
		free( ldint );
		return 1;
	}

	ret = pm->SetIndexBuf( dataleng, ldint, 0 );
	if( ret ){
		DbgOut( "sigfile : LoadPolyMesh : SetPolyMeshIndex error !!!\n" );
		_ASSERT( 0 );
		free( ldint );
		return 1;
	}
	free( ldint );


	//頂点ごとの色情報の読み込み
	if( loadmagicno >= SIGFILEMAGICNO_2 ){
		_ASSERT( pm->diffusebuf );
		_ASSERT( pm->ambientbuf );
		_ASSERT( pm->specularbuf );

		ret = LoadARGBData( pm->diffusebuf, pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : LoadARGBData : diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm->ambientbuf, pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : LoadARGBData : diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm->specularbuf, pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : LoadARGBData : diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( loadmagicno >= SIGFILEMAGICNO_4 ){
		_ASSERT( pm->powerbuf );
		_ASSERT( pm->emissivebuf );

		ret = LoadFloatData( pm->powerbuf, pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : load powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm->emissivebuf, pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : load emissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( flags & FLAGS_ORGNOBUF ){
		int* dstorgno;
		dstorgno = pm->orgnobuf;
		if( !dstorgno ){
			DbgOut( "sigfile : LoadPolyMesh : pm orgnobuf not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadIntData( dstorgno, pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : LoadIntData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( flags & FLAGS_PMUVBUF ){
		//curselem->texrule = TEXRULE_MQ;

		ret = pm->CreateTextureBuffer();
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : pm CreateTextureBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		unsigned char* dstuv = (unsigned char*)pm->uvbuf;
		ret = LoadUCharData( dstuv, sizeof( COORDINATE ) * pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : LoadUCharData UV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( flags & FLAGS_INFELEM ){
		ret = pm->CreateInfElemIfNot( pointnum );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : CreateInfElemIfNot error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		_ASSERT( pm->m_IE );
		INFELEMHEADER ieheader;
		SIGINFELEM sigie;
		CInfElem* ieptr;
		int pno;
		for( pno = 0; pno < pointnum; pno++ ){
			ieptr = pm->m_IE + pno;
			_ASSERT( ieptr );
		
			ret = LoadUCharData( (unsigned char*)&ieheader, sizeof( INFELEMHEADER ) );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh : Load ieheader error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int infnum;
			infnum = ieheader.infnum;
			ieptr->normalizeflag = ieheader.normalizeflag;
			if( ieheader.symaxis != SYMAXIS_NONE ){
				ieptr->symaxis = ieheader.symaxis;
				ieptr->symdist = ieheader.symdist;
			}else{
				ieptr->symaxis = SYMAXIS_X;
				ieptr->symdist = 100.0f;
			}

			int infno;
			for( infno = 0; infno < infnum; infno++ ){
				ret = LoadUCharData( (unsigned char*)&sigie, sizeof( SIGINFELEM ) );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh : Load sigie error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				
				INFELEM addie;
				ZeroMemory( &addie, sizeof( INFELEM ) );

				if( loadmagicno >= SIGFILEMAGICNO_5 ){
					addie.bonematno = sigie.bonematno + m_offset;
				}else{
					if( sigie.childno > 0 ){
						addie.bonematno = sigie.childno + m_offset;
					}else{
						addie.bonematno = sigie.bonematno + m_offset;
					}
				}

				addie.childno = sigie.childno + m_offset;//!!!!!!!!!
				//addie.bonematno = sigie.bonematno + m_offset;
				addie.kind = sigie.kind;
				addie.userrate = sigie.userrate;
				addie.orginf = sigie.orginf;
				addie.dispinf = sigie.dispinf;

				ret = ieptr->AddInfElem( addie );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh : AddInfElem error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}

	if( flags2 & FLAGS2_TOON1 ){
		ret = LoadIntData( &pm->m_materialnum, 1 );
		if( ret ){
			DbgOut( "sigfile : LoadPolyMesh : load materialnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		pm->m_toon1 = new CToon1Params[ pm->m_materialnum ];
		if( !pm->m_toon1 ){
			DbgOut( "sigfile : LoadPolyMesh : m_toon1 alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int matno;
		for( matno = 0; matno < pm->m_materialnum; matno++ ){
			SIGTOON1EX* sigtoon1exptr = 0;
			SIGTOON1* sigtoon1ptr = 0;

			SIGTOON1 sigtoon1;
			ZeroMemory( &sigtoon1, sizeof( SIGTOON1 ) );
			SIGTOON1EX sigtoon1ex;
			ZeroMemory( &sigtoon1ex, sizeof( SIGTOON1EX ) );

			if( loadmagicno >= SIGFILEMAGICNO_13 ){
				ret = LoadUCharData( (unsigned char*)&sigtoon1ex, sizeof( SIGTOON1EX ) );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh : load sigtoon1ex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sigtoon1ptr = &(sigtoon1ex.toon1);
				sigtoon1exptr = &sigtoon1ex;
			}else{
				ret = LoadUCharData( (unsigned char*)&sigtoon1, sizeof( SIGTOON1 ) );
				if( ret ){
					DbgOut( "sigfile : LoadPolyMesh : load sigtoon1 error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
				sigtoon1ptr = &sigtoon1;
				sigtoon1exptr = 0;
			}

			CToon1Params* curtoon1;
			curtoon1 = pm->m_toon1 + matno;

			strcpy_s( curtoon1->name, 32, sigtoon1ptr->name );
			curtoon1->diffuse = sigtoon1ptr->diffuse;
			curtoon1->ambient = sigtoon1ptr->ambient;
			curtoon1->specular = sigtoon1ptr->specular;
			curtoon1->darkh = sigtoon1ptr->darkh;
			curtoon1->brighth = sigtoon1ptr->brighth;
			curtoon1->ambientv = sigtoon1ptr->ambientv;
			curtoon1->diffusev = sigtoon1ptr->diffusev;
			curtoon1->specularv = sigtoon1ptr->specularv;
			curtoon1->darknl = sigtoon1ptr->darknl;
			curtoon1->brightnl = sigtoon1ptr->brightnl;
			
			curtoon1->tex = 0;

			if( flags2 & FLAGS2_EDGE0 ){
				curtoon1->edgecol0 = sigtoon1ptr->edgecol0;
				curtoon1->edgevalid0 = sigtoon1ptr->edgevalid0;
				curtoon1->edgeinv0 = sigtoon1ptr->edgeinv0;
			}else{
				curtoon1->edgecol0.r = sigtoon1ptr->ambient.r * 0.6f;
				curtoon1->edgecol0.g = sigtoon1ptr->ambient.g * 0.6f;
				curtoon1->edgecol0.b = sigtoon1ptr->ambient.b * 0.6f;
				curtoon1->edgecol0.a = 8.0f;

				curtoon1->edgevalid0 = 1;
				curtoon1->edgeinv0 = 0;
			}

			if( sigtoon1exptr ){
				curtoon1->toon0dnl = sigtoon1exptr->toon0dnl;
				curtoon1->toon0bnl = sigtoon1exptr->toon0bnl;
			}else{
				curtoon1->toon0dnl = sigtoon1ptr->darknl;
				curtoon1->toon0bnl = sigtoon1ptr->brightnl;
			}

		}

		pm->m_attrib = new DWORD[ facenum ];
		if( !pm->m_attrib ){
			DbgOut( "sigfile : LoadPolyMesh : attrib alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = LoadUCharData( (unsigned char*)pm->m_attrib, sizeof( DWORD ) * facenum );
		if( ret ){
			DbgOut( "sigfile : loadPolyMesh : attrib load error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}


	if( selem->type != SHDINFSCOPE ){
		if( flags2 & FLAGS2_MQOMATERIAL ){
			if( !pm->m_attrib0 ){
				DbgOut( "sigfile : LoadPolyMesh : attrib0 NULL error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = LoadIntData( pm->m_attrib0, facenum );
			if( ret ){
				DbgOut( "sigfile : LoadPolyMesh : pm attrib0 load error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			int fno;
			for( fno = 0; fno < facenum; fno++ ){
				*( pm->m_attrib0 + fno ) += m_materialoffset;//!!!!!!!!!
			}



		}else{
			CMQOMaterial* newmat = 0;
			newmat = AddMQOMaterial( selem );
			if( !newmat ){
				DbgOut( "sigfile : LoadPolyMesh : AddMQOMaterial error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			int faceno;
			for( faceno = 0; faceno < facenum; faceno++ ){
				*( pm->m_attrib0 + faceno ) = newmat->materialno;
			}

		}
	}else{
		//INFSCOPEの中のpmにはattrib0データなし。
	}

	return 0;
}



int CSigFile::Init3DObj()
{
	int ret;

	ret = lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CSigFile::CreateHandler()
{
	lpth = new CTreeHandler2( TMODE_IMPORT | TMODE_MULT );
	if( !lpth ){
		_ASSERT( 0 );
		return 1;
	}
	lpsh = new CShdHandler( lpth );
	if( !lpsh ){
		_ASSERT( 0 );
		return 1;
	}
	lpmh = new CMotHandler( lpth );
	if( !lpmh ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}
int CSigFile::DestroyHandler()
{
	if( lpth ){
		delete lpth;
		lpth = 0;
	}

	if( lpsh ){
		delete lpsh;
		lpsh = 0;
	}

	if( lpmh ){
		delete lpmh;
		lpmh = 0;
	}
	return 0;
}

int CSigFile::WriteGroundData( CShdElem* selem )
{
	CMeshInfo* mi;
	int ret;


	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	return ret;
}

int CSigFile::LoadGroundData()
{
	int ret;

	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	ret = Init3DObj();
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CSigFile::WriteMQOMaterial()
{
	int materialnum = 0;
	CMQOMaterial* curmqomat = lpsh->m_mathead;

	while( curmqomat ){
		if( curmqomat->materialno >= 0 ){
			materialnum++;
		}
		curmqomat = curmqomat->next;
	}

	int ret;
	ret = WriteIntData( &materialnum, 1 );
	_ASSERT( !ret );

	curmqomat = lpsh->m_mathead;
	int matcnt = 0;
	while( curmqomat ){
		if( curmqomat->materialno < 0 ){
			curmqomat = curmqomat->next;
			continue;
		}
		matcnt++;

		SIGMATERIAL cursigmat;		
		ZeroMemory( &cursigmat, sizeof( SIGMATERIAL ) );

		cursigmat.materialno = curmqomat->materialno;
		cursigmat.legacyflag = curmqomat->legacyflag;
		if( curmqomat->name[0] != 0 )
			strcpy_s( cursigmat.name, 256, curmqomat->name );

		cursigmat.col = curmqomat->col;
		cursigmat.dif = curmqomat->dif;
		cursigmat.amb = curmqomat->amb;
		cursigmat.emi = curmqomat->emi;
		cursigmat.spc = curmqomat->spc;
		cursigmat.power = curmqomat->power;
		if( curmqomat->tex[0] != 0 )
			strcpy_s( cursigmat.tex, 256, curmqomat->tex );
		if( curmqomat->alpha[0] != 0 )
			strcpy_s( cursigmat.alpha, 256, curmqomat->alpha );
		if( curmqomat->bump[0] != 0 )
			strcpy_s( cursigmat.bump, 256, curmqomat->bump );
		cursigmat.vcolflag = curmqomat->vcolflag;
		cursigmat.shader = curmqomat->shader;
		
		cursigmat.dif4f = curmqomat->dif4f;
		cursigmat.amb3f = curmqomat->amb3f;
		cursigmat.emi3f = curmqomat->emi3f;
		cursigmat.spc3f = curmqomat->spc3f;

		cursigmat.transparent = curmqomat->transparent;
		cursigmat.texrule = curmqomat->texrule;
		cursigmat.blendmode = curmqomat->blendmode;
		cursigmat.texanimtype = curmqomat->texanimtype;

		cursigmat.uanime = curmqomat->uanime;
		cursigmat.vanime = curmqomat->vanime;

		cursigmat.exttexnum = curmqomat->exttexnum;
		cursigmat.exttexmode = curmqomat->exttexmode;
		cursigmat.exttexrep = curmqomat->exttexrep;
		cursigmat.exttexstep = curmqomat->exttexstep;

		cursigmat.convnamenum = curmqomat->convnamenum;

		cursigmat.alphatest0 = curmqomat->alphatest0;
		cursigmat.alphaval0 = curmqomat->alphaval0;
		cursigmat.alphatest1 = curmqomat->alphatest1;
		cursigmat.alphaval1 = curmqomat->alphaval1;

		cursigmat.orgalpha = curmqomat->orgalpha;

		SIGMATERIAL2 cursigmat2;
		ZeroMemory( &cursigmat2, sizeof( SIGMATERIAL2 ) );
		cursigmat2.material1 = cursigmat;
		cursigmat2.glowmult[0] = curmqomat->glowmult[0];
		cursigmat2.glowmult[1] = curmqomat->glowmult[1];
		cursigmat2.glowmult[2] = curmqomat->glowmult[2];

		ret = WriteUCharData( (unsigned char*)&cursigmat2, sizeof( SIGMATERIAL2 ) );
		_ASSERT( !ret );

		if( cursigmat.exttexnum > 0 ){
			ret = WriteExtTexName( cursigmat.exttexnum, curmqomat->exttexname );
			_ASSERT( !ret );
		}

		if( cursigmat.convnamenum > 0 ){
			ret = WriteExtTexName( cursigmat.convnamenum, curmqomat->ppconvname );
			_ASSERT( !ret );
		}

		curmqomat = curmqomat->next;
	}

	if( matcnt != materialnum ){
		DbgOut( "sigfile : WriteMQOMaterial : matcnt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CSigFile::LoadSigMaterial()
{
	int ret;

	m_newmatindex.clear();

	int materialnum = 0;

	ret = LoadIntData( &materialnum, 1 );
	if( ret ){
		DbgOut( "sigfile : LoadSigMaterial : LoadIntData materialnum error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOMaterial* curmqomat = lpsh->GetLastMaterial();
	if( !curmqomat ){
		DbgOut( "sigfile : LoadSigMaterial : mathead NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int matno;
	for( matno = 0; matno < materialnum; matno++ ){
		CMQOMaterial* newmat = 0;


		SIGMATERIAL sigmat1;
		ZeroMemory( &sigmat1, sizeof( SIGMATERIAL ) );
		SIGMATERIAL2 sigmat2;
		ZeroMemory( &sigmat2, sizeof( SIGMATERIAL2 ) );
		SIGMATERIAL* sigmatptr = 0;

		if( loadmagicno >= SIGFILEMAGICNO_12 ){
			ret = LoadUCharData( (unsigned char*)&sigmat2, sizeof( SIGMATERIAL2 ) );
			sigmatptr = &( sigmat2.material1 );//!!!!!!!!!!!!
		}else{
			ret = LoadUCharData( (unsigned char*)&sigmat1, sizeof( SIGMATERIAL ) );
			sigmatptr = &sigmat1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		if( ret ){
			DbgOut( "sigfile : LoadSigMaterial : LoadUCharData sigmat error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		char* curmatname = sigmatptr->name;
		int matno1 = -1;
		lpsh->GetMaterialNoByName( curmatname, &matno1 );
		if( matno1 >= 0 ){
			newmat = lpsh->GetMaterialFromNo( lpsh->m_mathead, matno1 );
		}

		if( newmat ){
			m_newmatindex[ sigmatptr->materialno ] = matno1;

			if( newmat->exttexnum > 0 ){
				if( newmat->exttexnum > MAXEXTTEXNUM ){
					DbgOut( "sigfile : LoadSigMaterial : exttexnum out of range error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ZeroMemory( exttexbuf, 256 * MAXEXTTEXNUM );
				int texno;
				for( texno = 0; texno < MAXEXTTEXNUM; texno++ ){
					exttexlist[texno] = exttexbuf[texno];
				}


				for( texno = 0; texno < newmat->exttexnum; texno++ ){
					unsigned char* dstbuf;
					dstbuf = (unsigned char*)exttexbuf[texno];

					ret = LoadUCharData( dstbuf, 256 );
					if( ret ){
						DbgOut( "SigFile : LoadSigMaterial : LoadUCharData exttex error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}

			if( newmat->convnamenum > 0 ){
				if( newmat->convnamenum > MAXEXTTEXNUM ){
					DbgOut( "sigfile : LoadSigMaterial : convnamenum out of range error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ZeroMemory( exttexbuf, 256 * MAXEXTTEXNUM );
				int texno;
				for( texno = 0; texno < MAXEXTTEXNUM; texno++ ){
					exttexlist[texno] = exttexbuf[texno];
				}

				for( texno = 0; texno < newmat->convnamenum; texno++ ){
					unsigned char* dstbuf;
					dstbuf = (unsigned char*)exttexbuf[texno];

					ret = LoadUCharData( dstbuf, 256 );
					if( ret ){
						DbgOut( "SigFile : LoadSigMaterial : LoadUCharData convname error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}
			}
		}else{

			newmat = new CMQOMaterial();
			if( !newmat ){
				DbgOut( "sigfile : LoadSigMaterial : newmat alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
			curmqomat->next = newmat;
			curmqomat = newmat;



			if( sigmatptr->materialno == -1 ){
				newmat->materialno = -1;
				_ASSERT( 0 );
			}else{
				newmat->materialno = sigmatptr->materialno + m_materialoffset;
			}
			(lpsh->m_materialcnt)++;


			newmat->legacyflag = sigmatptr->legacyflag;

			if( sigmatptr->name[0] != 0 ){
			
				if( m_offset == 0 ){
					strcpy_s( newmat->name, 256, sigmatptr->name );
				}else{
					sprintf_s( newmat->name, 256, "%s_%d", sigmatptr->name, m_offset );
				}
			
			}
			newmat->col = sigmatptr->col;
			newmat->dif = sigmatptr->dif;
			newmat->amb = sigmatptr->amb;
			newmat->emi = sigmatptr->emi;
			newmat->spc = sigmatptr->spc;
			newmat->power = sigmatptr->power;
			if( sigmatptr->tex[0] != 0 ){
				strcpy_s( newmat->tex, 256, sigmatptr->tex );
			}
			if( sigmatptr->alpha[0] != 0 ){
				strcpy_s( newmat->alpha, 256, sigmatptr->alpha );
			}
			if( sigmatptr->bump[0] != 0 ){
				strcpy_s( newmat->bump, 256, sigmatptr->bump );
			}
			newmat->vcolflag = sigmatptr->vcolflag;
			newmat->shader = sigmatptr->shader;
			newmat->dif4f = sigmatptr->dif4f;
			newmat->amb3f = sigmatptr->amb3f;
			newmat->emi3f = sigmatptr->emi3f;
			newmat->spc3f = sigmatptr->spc3f;

			newmat->transparent = sigmatptr->transparent;
			newmat->texrule = sigmatptr->texrule;
			newmat->blendmode = sigmatptr->blendmode;
			newmat->texanimtype = sigmatptr->texanimtype;

			newmat->uanime = sigmatptr->uanime;
			newmat->vanime = sigmatptr->vanime;

			newmat->exttexnum = sigmatptr->exttexnum;
			newmat->exttexmode = sigmatptr->exttexmode;
			newmat->exttexrep = sigmatptr->exttexrep;
			newmat->exttexstep = sigmatptr->exttexstep;

			newmat->convnamenum = sigmatptr->convnamenum;

			if( (loadmagicno >= SIGFILEMAGICNO_8) && (newmat->blendmode >= 100) ){
				newmat->alphatest0 = sigmatptr->alphatest0;
				newmat->alphaval0 = sigmatptr->alphaval0;
				newmat->alphatest1 = sigmatptr->alphatest1;
				newmat->alphaval1 = sigmatptr->alphaval1;
			}else{
				newmat->alphaval0 = 1;
				newmat->alphaval0 = 8;
				newmat->alphaval1 = 8;

				if( newmat->blendmode == 0 ){
					newmat->alphatest1 = 1;
					newmat->alphaval1 = 8;
					newmat->blendmode = 100;//!!!!!!
				}else if( newmat->blendmode == 1 ){
					newmat->alphatest1 = 1;
					newmat->alphaval1 = 0;
					newmat->blendmode = 101;//!!!!!!
				}else if( newmat->blendmode == 2 ){
					newmat->alphatest1 = 1;
					newmat->alphaval1 = 0;
					newmat->blendmode = 102;//!!!!!!
				}else{
					_ASSERT( 0 );
					newmat->alphatest1 = 1;
					newmat->alphaval1 = 8;
					newmat->blendmode = 100;//!!!!!!
				}
			}

			if( loadmagicno >= SIGFILEMAGICNO_12 ){
				MoveMemory( newmat->glowmult, sigmat2.glowmult, sizeof( float ) * 3 );
			}else{
				newmat->glowmult[0] = 1.0f;
				newmat->glowmult[1] = 1.0f;
				newmat->glowmult[2] = 1.0f;
			}

			if( loadmagicno >= SIGFILEMAGICNO_14 ){
				newmat->orgalpha = sigmatptr->orgalpha;
	//DbgOut( "checkalp!!! : sigfile LoadSigMaterial 0: orgalpha %f\r\n", newmat->orgalpha );
			}else{
				newmat->orgalpha = sigmatptr->col.a;
	//DbgOut( "checkalp!!! : sigfile LoadSigMaterial 1: orgalpha %f, dif4f %f\r\n", newmat->orgalpha, sigmatptr->dif4f.a );
			}

		//char** exttexname;
		//char** ppconvname;
			if( newmat->exttexnum > 0 ){
				if( newmat->exttexnum > MAXEXTTEXNUM ){
					DbgOut( "sigfile : LoadSigMaterial : exttexnum out of range error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ZeroMemory( exttexbuf, 256 * MAXEXTTEXNUM );
				int texno;
				for( texno = 0; texno < MAXEXTTEXNUM; texno++ ){
					exttexlist[texno] = exttexbuf[texno];
				}


				for( texno = 0; texno < newmat->exttexnum; texno++ ){
					unsigned char* dstbuf;
					dstbuf = (unsigned char*)exttexbuf[texno];

					ret = LoadUCharData( dstbuf, 256 );
					if( ret ){
						DbgOut( "SigFile : LoadSigMaterial : LoadUCharData exttex error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				ret = newmat->SetExtTex( newmat->exttexnum, exttexlist );
				if( ret ){
					DbgOut( "sigfile : LoadSigMaterial : mqomat SetExtTex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}

			if( newmat->convnamenum > 0 ){
				if( newmat->convnamenum > MAXEXTTEXNUM ){
					DbgOut( "sigfile : LoadSigMaterial : convnamenum out of range error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}

				ZeroMemory( exttexbuf, 256 * MAXEXTTEXNUM );
				int texno;
				for( texno = 0; texno < MAXEXTTEXNUM; texno++ ){
					exttexlist[texno] = exttexbuf[texno];
				}


				for( texno = 0; texno < newmat->convnamenum; texno++ ){
					unsigned char* dstbuf;
					dstbuf = (unsigned char*)exttexbuf[texno];

					ret = LoadUCharData( dstbuf, 256 );
					if( ret ){
						DbgOut( "SigFile : LoadSigMaterial : LoadUCharData convname error !!!\n" );
						_ASSERT( 0 );
						return 1;
					}
				}

				ret = newmat->SetConvName( newmat->convnamenum, exttexlist );
				if( ret ){
					DbgOut( "sigfile : LoadSigMaterial : mqomat SetExtTex error !!!\n" );
					_ASSERT( 0 );
					return 1;
				}
			}
		}

	}

	return 0;
}

int CSigFile::LoadExtTex( SIGHEADER sh )
{
	unsigned char exttexnum, exttexmode, exttexrep, exttexstep;

	exttexnum = (unsigned char)(sh.exttexmem >> 24);
	exttexmode = (unsigned char)((sh.exttexmem & 0x00FF0000) >> 16);
	exttexrep = (unsigned char)((sh.exttexmem & 0x0000FF00) >> 8);
	exttexstep = (unsigned char)(sh.exttexmem & 0x000000FF);

	if( exttexnum <= 0 )
		return 0;

	if( exttexnum > MAXEXTTEXNUM ){
		DbgOut( "SigFile : LoadExtTex : exttexnum error !!!\n" );
		_ASSERT( 0 );
	}

	ZeroMemory( exttexbuf, 256 * MAXEXTTEXNUM );
	int texno;
	for( texno = 0; texno < MAXEXTTEXNUM; texno++ ){
		exttexlist[texno] = exttexbuf[texno];
	}


	char dummyname[256];

	int ret;

	for( texno = 0; texno < exttexnum; texno++ ){
		unsigned char* dstbuf;
		if( texno < MAXEXTTEXNUM )
			dstbuf = (unsigned char*)exttexbuf[texno];
		else
			dstbuf = (unsigned char*)dummyname;

		ret = LoadUCharData( dstbuf, 256 );
		if( ret ){
			DbgOut( "SigFile : LoadExtTex : LoadUCharData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = lpsh->SetExtTex( curseri, exttexnum, exttexmode, exttexrep, exttexstep, exttexlist );
	if( ret ){
		DbgOut( "SigFile : LoadExtTex : SetExtTex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}

CMQOMaterial* CSigFile::AddMQOMaterial( CShdElem* selem )
{
	CMQOMaterial* newmat = 0;

	newmat = new CMQOMaterial();
	if( !newmat ){
		DbgOut( "sigfile : AddMQOMaterial newmat alloc error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	CMQOMaterial* lastmat;
	lastmat = lpsh->GetLastMaterial();
	if( !lastmat ){
		DbgOut( "sigfile : AddMQOMaterial : lastmat NULL error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}
	lastmat->next = newmat;

	newmat->materialno = lpsh->m_materialcnt - 1;
	(lpsh->m_materialcnt)++;

	int ret;
	ret = newmat->SetLegacyMaterial( lpth, selem );
	if( ret ){
		DbgOut( "sigfile : AddMQOMaterial : mqomat SetLegacyMaterial error !!!\n" );
		_ASSERT( 0 );
		return 0;
	}

	return newmat;
}



