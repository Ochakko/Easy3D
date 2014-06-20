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


#include <crtdbg.h>

#include <gndfile.h>
#include <mqomaterial.h>


///// sigheader の　flags
#define FLAGS_NOSKINNING	0x01
#define FLAGS_ORGNOBUF		0x02	//先頭のsigheader(startheader)と、パーツごとのsigheaderに記録する。
#define FLAGS_NOTRANSIK		0x04
#define FLAGS_BONETYPE		0x08
#define FLAGS_ANCHORFLAG	0x10
#define FLAGS_PMUVBUF		0x20
#define FLAGS_MIKOBLEND		0x40


///// sigheader の flags2
#define FLAGS2_ORGNORMAL	0x01
#define FLAGS2_VCOL			0x02
#define FLAGS2_OLD2OPT		0x04
#define FLAGS2_SMOOTHBUF	0x08


//extern func
//#if !plugin
//	extern int LoadShdData( CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, LPCTSTR fname );
//	extern void FreeShdLexBuff();
//#endif
///////////////



CGndFile::CGndFile()
{
	hfile = 0;
	lpth = 0;
	lpsh = 0;
	lpmh = 0;

	InitLoadParams();

}
CGndFile::~CGndFile()
{

}

int CGndFile::WriteGndFile2Buf( char* buf, int bufsize, int* writesizeptr, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh )
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
		DbgOut( "GndFile : WriteGndFile2Buf : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;


	ret = WriteGndFile_aft();
	if( ret ){
		DbgOut( "GndFile : WriteGndFile2Buf : WriteGndFile_aft error !!!\n" );
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




int CGndFile::WriteGndFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh )
{
	int ret = 0;

	m_writemode = SIGWRITEMODE_FILE;


	if( !filename )
		return 1;

	if( !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "GndFile : WriteGndFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;

DbgOut( "GndFile : WriteGndFile : CreateFile\n" );

	hfile = CreateFile( (LPCTSTR)filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		ret = 1;
		goto writesigexit;
	}	

	ret = WriteGndFile_aft();
	if( ret ){
		DbgOut( "GndFile : WriteGndFile : WriteGndFile_aft error !!!\n" );
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

int CGndFile::WriteGndFile_aft()
{

	CShdElem* rootselem = 0;
	//CMotCtrl* mctrl = 0;
	int ret = 0;
	int errorflag = 0;
	int no_pm2orgno = 0;


	rootselem = (*lpsh)( 0 );
	if( !rootselem ){
		DbgOut( "GndFile : WriteGndFile : rootselem NULL error !!!\n" );
		return 1;
	}

	//ret = lpsh->CheckOrgNoBuf( &no_pm2orgno );
	//if( ret ){
	//	DbgOut( "GndFile : WriteGndFile : shandler : CheckOrgNoBuf error !!!\n" );
	//	_ASSERT( 0 );
	//	return 1;
	//}

	writemagicno = GNDFILEMAGICNO_6;//!!!!!!!!!!!!!!!!


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




	m_writeno = 0;

//DbgOut( "GndFile : WriteGndFile : start WriteShdReq\n" );

	WriteShdReq( rootselem, &errorflag );	
	if( errorflag ){
		_ASSERT( 0 );
		return 1;
	}

//DbgOut( "GndFile : WriteGndFile : end WriteShdReq\n" );

	
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



void CGndFile::WriteShdReq( CShdElem* selem, int* errorflag )
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


int CGndFile::WriteShdElem( CShdElem* selem )
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
		DbgOut( "GndFile : WriteShdElem : name : nameleng too large %s!!!", telem->name );
		nameleng = 255;
	}
	strncpy( sigheader.name, telem->name, nameleng );

	sigheader.brono = telem->brono;
	sigheader.depth = selem->depth;
	sigheader.clockwise = selem->clockwise;
	sigheader.bdivu = selem->bdivU;
	sigheader.bdivv = selem->bdivV;

	if( selem->texname ){
		nameleng = (int)strlen( selem->texname );
		if( nameleng >= 256 ){
			DbgOut( "GndFile : WriteShdElem : texname : nameleng too large %s!!!", selem->texname );
			nameleng = 255;
		}
		strncpy( sigheader.texname, selem->texname, nameleng );
	}else{
		strcpy( sigheader.texname, "0" );
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


	if( selem->type == SHDPOLYMESH2 ){
		CPolyMesh2* pm2ptr;
		pm2ptr = selem->polymesh2;

		if( !pm2ptr ){
			DbgOut( "GndFile : WriteShdElem : pm2 not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( pm2ptr->orgnobuf ){
			sigheader.flags |= FLAGS_ORGNOBUF;
		}else{
			sigheader.flags &= ~FLAGS_ORGNOBUF;
		}
	}
	if( selem->type == SHDPOLYMESH ){
		CPolyMesh* pmptr;
		pmptr = selem->polymesh;

		if( !pmptr ){
			DbgOut( "GndFile : WriteShdElem : pm not exist error !!!\n" );
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


	sigheader.dispswitchno = selem->dispswitchno;

	if( selem->m_notransik ){
		sigheader.flags |= FLAGS_NOTRANSIK;
	}else{
		sigheader.flags &= ~FLAGS_NOTRANSIK;
	}


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
						//DbgOut( "GndFile : WriteShdElem : list%d - seri %d - bno %d\n", templeng, serino, bno );
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
						//DbgOut( "GndFile : WriteShdElem : list%d - seri %d - bno %d\n", tempigleng, serino, bno );
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
			ret = WritePolyMesh( selem );
			break;
		case SHDPOLYMESH2:
			ret = WritePolyMesh2( selem );
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

int CGndFile::WriteExtTexName( int exttexnum, char** exttexname )
{
	int ret, texno;

	if( exttexnum > MAXEXTTEXNUM ){
		DbgOut( "GndFile : WriteExtTexName : exttexnum error !!!\n" );
		_ASSERT( 0 );
		exttexnum = MAXEXTTEXNUM;//!!!!
	}

	for( texno = 0; texno < exttexnum; texno++ ){
		char tempname[256];
		ZeroMemory( tempname, 256 );
		int namelen, cplen;
		namelen = (int)strlen( *( exttexname + texno ) );
		if( namelen >= 256 ){
			DbgOut( "GndFile : WriteExtTexName : namelen error !!! %s\n", *(exttexname + texno) );
			_ASSERT( 0 );
		}

		cplen = min( namelen, 255 );
		strncpy( tempname, *( exttexname + texno ), cplen );
		
		ret = WriteUCharData( (unsigned char*)tempname, 256 );
		if( ret ){
			_ASSERT( 0 );
			return 1;
		}

	}

	return 0;
}

int CGndFile::WriteSigHeader( SIGHEADER sigheader )
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
			DbgOut( "GndFile : WriteSigHeader : bufsize too short error !!!\n" );
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
int CGndFile::WriteMIHeader( CMeshInfo* srcmi )
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
			DbgOut( "GndFile : WriteMIHeader : bufsize too short error !!!\n" );
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

int CGndFile::WriteVecLine( CVecLine* srcvl )
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

int CGndFile::WriteDWData( DWORD* srcdw, int dwnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcdw );

	wleng = sizeof( DWORD ) * dwnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcdw, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "GndFile : WriteDWData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "GndFile : WriteDWData : bufsize too short error !!!\n" );
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


int CGndFile::WriteRGBAData( ARGBF* srcargb, int colnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcargb );

	wleng = sizeof( ARGBF ) * colnum;


	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcargb, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "GndFile : WriteARGBData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "GndFile : WriteRGBAData : bufsize too short error !!!\n" );
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


int CGndFile::WriteVecData( VEC3F* srcvec, int vnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcvec );

	wleng = sizeof( VEC3F ) * vnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:

		WriteFile( hfile, srcvec, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "GndFile : WriteVecData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}
		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "GndFile : WriteVecData : bufsize too short error !!!\n" );
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

int CGndFile::WriteUCharData( unsigned char* srcuchar, int num )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcuchar );

	wleng = num;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcuchar, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "GndFile : WriteUCharData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "GndFile : WriteUCharaData : bufsize too short error !!!\n" );
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

int CGndFile::WriteFloatData( float* srcfloat, int flnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcfloat );

	wleng = sizeof( float ) * flnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcfloat, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "gndfile : WriteFloatData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "gndfile : WriteFloatData : bufsize too short error !!!\n" );
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

int CGndFile::WriteIntData( int* srcint, int intnum )
{
	unsigned long wleng, writeleng;

	_ASSERT( srcint );

	wleng = sizeof( int ) * intnum;

	switch( m_writemode ){
	case SIGWRITEMODE_FILE:
		WriteFile( hfile, srcint, wleng, &writeleng, NULL );
		if( wleng != writeleng ){
			DbgOut( "GndFile : WriteIntData error %d %d\n",
				wleng, writeleng );
			_ASSERT( 0 );
			return 1;
		}

		break;
	case SIGWRITEMODE_BUF:
		if( (m_buf.pos + (int)wleng) > m_buf.bufsize ){
			DbgOut( "GndFile : WriteIntData : bufsize too short error !!!\n" );
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

int CGndFile::WriteMatData( CMatrix2* srcmat, int matnum )
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
			DbgOut( "GndFile : WriteMatData : bufsize too short error !!!\n" );
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

int CGndFile::WriteMorph( CShdElem* selem )
{
	CMorph* morph = selem->morph;
	CMeshInfo* mi;
	int ret;

	_ASSERT( morph );
	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	return ret;
}

int CGndFile::WriteGroundData( CShdElem* selem )
{
	CMeshInfo* mi;
	int ret;


	mi = selem->GetMeshInfo();
	_ASSERT( mi );

	ret = WriteMIHeader( mi );
	_ASSERT( !ret );

	return ret;
}

int CGndFile::WritePart( CShdElem* selem )
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

	return ret;
}
int CGndFile::WritePolyLine( CShdElem* selem )
{
	CVecLine* vline = selem->vline;
	int ret;

	_ASSERT( vline );

	ret = WriteVecLine( vline );
	return ret;
}
int CGndFile::WritePolygon( CShdElem* selem )
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
int CGndFile::WriteSphere( CShdElem* selem )
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
int CGndFile::WriteBezMesh( CShdElem* selem )
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
int CGndFile::WriteRevolved( CShdElem* selem )
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
			DbgOut( "GndFile : WriteRevolved : bufsize too short error !!!\n" );
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
int CGndFile::WriteDisk( CShdElem* selem )
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
int CGndFile::WriteBezLine( CShdElem* selem )
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
int CGndFile::WriteExtrude( CShdElem* selem )
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

int CGndFile::WritePolyMesh2( CShdElem* selem )
{
	CPolyMesh2* polymesh2;
	polymesh2 = selem->polymesh2;
	_ASSERT( polymesh2 );

//DbgOut( "GndFile : WritePolyMesh2 : 0 \n" );

	CMeshInfo* mi;
	mi = selem->GetMeshInfo();
	if( !mi ){
		DbgOut( "GndFile : WritePolyMesh2 : GetMeshInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = WriteMIHeader( mi );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh2 : WriteMIHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int bufleng, colbufleng;
	bufleng = mi->n * 3;
	colbufleng = mi->n;

//DbgOut( "GndFile : WritePolyMesh2 : 1 \n" );

	ret = WriteVecData( polymesh2->pointbuf, bufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh2 : WriteVecData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteIntData( polymesh2->samepointbuf, bufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh2 : WriteIntData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}
	
	ret = WriteIntData( polymesh2->oldpno2optpno, bufleng );
	if( ret ){
		DbgOut( "gndfile : WritePolyMesh2 : write old2opt error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteIntData( polymesh2->smoothbuf, bufleng );
	if( ret ){
		DbgOut( "gndfile : WritePolyMesh2 : write smoothbuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteUCharData( (unsigned char*)polymesh2->uvbuf, sizeof( COORDINATE ) * bufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}
///////
	ret = WriteUCharData( (unsigned char*)polymesh2->diffusebuf, sizeof( ARGBF ) * colbufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteUCharData( (unsigned char*)polymesh2->ambientbuf, sizeof( ARGBF ) * colbufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteUCharData( (unsigned char*)polymesh2->specularbuf, sizeof( float ) * colbufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh2 : WriteUCharData error %d!!!\n",
			bufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteUCharData( (unsigned char*)polymesh2->orgnormal, sizeof( VEC3F ) * bufleng );
	if( ret ){
		DbgOut( "gndfile : WritePolyMesh2 : orgnormal write error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int optpleng = 0;


	//頂点ごとの色情報の書き出し
	if( writemagicno >= GNDFILEMAGICNO_2 ){

		optpleng = polymesh2->optpleng;

		//optplengを書き出す。
		ret = WriteIntData( &optpleng, 1 );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh2 : WriteIntData error !!!\n" );
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
				DbgOut( "GndFile : WritePolyMesh2 : WriteDWData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		// optambientを書き出す
		ret = WriteRGBAData( polymesh2->optambient, optpleng );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh2 : WriteRGBData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// opttlv->specularを書き出す
		for( optno = 0; optno < optpleng; optno++ ){
			DWORD dwspec;
			dwspec = (polymesh2->opttlv + optno)->specular;
			ret = WriteDWData( &dwspec, 1 );
			if( ret ){
				DbgOut( "GndFile : WritePolyMesh2 : WriteDWData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}
	}

	if( writemagicno >= GNDFILEMAGICNO_4 ){
		ret = WriteFloatData( polymesh2->diffbuf, colbufleng );
		if( ret ){
			DbgOut( "gndfile : WritePolyMesh2 : write diffbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteFloatData( polymesh2->powerbuf, colbufleng );
		if( ret ){
			DbgOut( "gndfile : WritePolyMesh2 : write powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteFloatData( polymesh2->emissivebuf, colbufleng );
		if( ret ){
			DbgOut( "gndfile : WritePolyMesh2 : write emissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		ret = WriteFloatData( polymesh2->optpowerbuf, optpleng );
		if( ret ){
			DbgOut( "gndfile : WritePolyMesh2 : write powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh2->optemissivebuf, optpleng );
		if( ret ){
			DbgOut( "gndfile : WritePolyMesh2 : write optemissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	// orgnobufの書き出し
	if( polymesh2->orgnobuf ){
		ret = WriteIntData( polymesh2->orgnobuf, bufleng );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh2 : WriteIntData error %d!!!\n",
				bufleng );
			_ASSERT( 0 );
			return 1;
		}
	}

	// vcol
	int validvcolnum;
	validvcolnum = polymesh2->GetValidVColNum();
	ret = WriteIntData( &validvcolnum, 1 );
	if( ret ){
		DbgOut( "Gndfile : WritePolyMesh2 : write vcolnum error !!!\n" );
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
				DbgOut( "gndfile : WritePolyMesh2 : write curvc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

	}



//DbgOut( "GndFile : WritePolyMesh2 : 2 \n" );

	return ret;

}


int CGndFile::WritePolyMesh( CShdElem* selem )
{
	CPolyMesh* polymesh;
	polymesh = selem->polymesh;
	_ASSERT( polymesh );

	//DbgOut( "GndFile : WritePolyMesh : type %d\n", selem->type );

	CMeshInfo* mi;
	mi = selem->GetMeshInfo();
	if( !mi ){
		DbgOut( "GndFile : WritePolyMesh : GetMeshInfo error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;
	ret = WriteMIHeader( mi );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh : WriteMIHeader error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int pointbufleng, indexbufleng;
	pointbufleng = mi->m;
	indexbufleng = mi->n * 3;

	ret = WriteVecData( polymesh->pointbuf, pointbufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh : WriteVecData error %d!!!\n",
			pointbufleng );
		_ASSERT( 0 );
		return 1;
	}

	ret = WriteIntData( polymesh->indexbuf, indexbufleng );
	if( ret ){
		DbgOut( "GndFile : WritePolyMesh : WriteIntData error %d!!!\n",
			indexbufleng );
		_ASSERT( 0 );
		return 1;
	}


	//頂点ごとの色情報の書き出し
	if( writemagicno >= GNDFILEMAGICNO_2 ){
		ret = WriteRGBAData( polymesh->diffusebuf, pointbufleng );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh : WriteRGBAData diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh->ambientbuf, pointbufleng );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh : WriteRGBAData ambient error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh->specularbuf, pointbufleng );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh : WriteRGBAData specular error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( writemagicno >= GNDFILEMAGICNO_4 ){
		ret = WriteFloatData( polymesh->powerbuf, pointbufleng );
		if( ret ){
			DbgOut( "gndfile : WritePolyMesh : WriteFloatData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = WriteRGBAData( polymesh->emissivebuf, pointbufleng );
		if( ret ){
			DbgOut( "gndfile : WritePolyMesh : WriteRGBAData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}


	// orgnobufの書き出し
	if( polymesh->orgnobuf ){
		ret = WriteIntData( polymesh->orgnobuf, pointbufleng );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh : WriteIntData error %d!!!\n",
				pointbufleng );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( polymesh->uvbuf ){
		//UV
		ret = WriteUCharData( (unsigned char*)polymesh->uvbuf, sizeof( COORDINATE ) * pointbufleng );
		if( ret ){
			DbgOut( "GndFile : WritePolyMesh : WriteUCharData UV error %d!!!\n",
				pointbufleng );
			_ASSERT( 0 );
			return 1;
		}
	}

	return ret;
}



//////////////////////////////
// load
//////////////////////////////
void CGndFile::InitLoadParams()
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

	m_materialoffset = 0;
}

int CGndFile::LoadSigHeader( SIGHEADER* sigheader )
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
int CGndFile::LoadMIHeader( CMeshInfo* dstmi )
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
int CGndFile::LoadFloatData( float* dstfloat, int flnum )
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

int CGndFile::LoadIntData( int* dstint, int intnum )
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

int CGndFile::LoadDWData( DWORD* dstdw, int dwnum )
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


int CGndFile::LoadARGBData( ARGBF* dstargb, int colnum )
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


int CGndFile::LoadVecData( VEC3F* dstvec, int vnum )
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
int CGndFile::LoadMatData( CMatrix2* dstmat, int matnum )
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

int CGndFile::LoadUCharData( unsigned char* dstuc, int ucnum )
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


int CGndFile::LoadGndFile( char* filename, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult, int adjustuvflag )
{
	int ret = 0;

	if( !filename || !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "GndFile : LoadGndFile : handler pointer NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	InitLoadParams();

	m_frombuf = 0;//!!!

	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;
	if( offset > 0 )
		m_offset = offset - 1;//import dataの先頭データもSHDROOTで、無効データなので。
	else
		m_offset = offset;

	m_datamult = datamult;
	m_adjustuvflag = adjustuvflag;

	
	char pathname[2048];
	int ch = '\\';
	char* enptr = 0;
	enptr = strrchr( filename, ch );
	if( enptr ){
		strcpy( pathname, filename );
	}else{
		ZeroMemory( pathname, 1024 );
		int resdirleng;
		resdirleng = GetEnvironmentVariable( (LPCTSTR)"RESDIR", (LPTSTR)pathname, 2048 );
		_ASSERT( resdirleng );
		strcat( pathname, filename );
	}


	hfile = CreateFile( (LPCTSTR)pathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hfile == INVALID_HANDLE_VALUE ){
		ret = D3DAPPERR_MEDIANOTFOUND;
		goto loadsigexit;
	}	

	ret = LoadGndFile_aft( offset );
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

int CGndFile::LoadGndFileFromBuf( char* bufptr, int bufsize, CTreeHandler2* srclpth, CShdHandler* srclpsh, CMotHandler* srclpmh, int offset, float datamult, int adjustuvflag )
{
	int ret = 0;

	if( !bufptr || (bufsize < 0) || !srclpth || !srclpsh || !srclpmh ){
		DbgOut( "GndFile : LoadGndFileFromBuf : parameter error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	InitLoadParams();

	m_frombuf = 1;//!!!

	lpth = srclpth;
	lpsh = srclpsh;
	lpmh = srclpmh;
	if( offset > 0 )
		m_offset = offset - 1;//import dataの先頭データもSHDROOTで、無効データなので。
	else
		m_offset = offset;

	m_datamult = datamult;
	m_adjustuvflag = adjustuvflag;

	m_buf.buf = (char*)malloc( sizeof( char ) * bufsize );
	if( !(m_buf.buf) ){
		DbgOut( "GndFile : LoadGndFileFromBuf : buf alloc error !!!\n" );
		_ASSERT( 0 );
		ret = 1;
		goto loadsigexit2;
	}
	
	MoveMemory( m_buf.buf, bufptr, sizeof( char ) * bufsize );

	m_buf.bufsize = bufsize;
	m_buf.pos = 0;

/////////
	ret = LoadGndFile_aft( offset );
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

int CGndFile::LoadGndFile_aft( int offset )
{

	D3DXMATRIX scalemat, rotxmat, rotymat, rotzmat, shiftmat;
	D3DXMatrixIdentity( &scalemat );
	D3DXMatrixIdentity( &rotxmat );
	D3DXMatrixIdentity( &rotymat );
	D3DXMatrixIdentity( &rotzmat );
	D3DXMatrixIdentity( &shiftmat );

	scalemat._11 = m_datamult;
	scalemat._22 = m_datamult;
	scalemat._33 = m_datamult;

	shiftmat._41 = 0.0f;
	shiftmat._42 = 0.0f;
	shiftmat._43 = 0.0f;

	D3DXMatrixRotationX( &rotxmat, 0.0f * (float)DEG2PAI );
	D3DXMatrixRotationY( &rotymat, 0.0f * (float)DEG2PAI );
	D3DXMatrixRotationZ( &rotzmat, 0.0f * (float)DEG2PAI );

	m_offsetmat = scalemat * rotzmat * rotymat * rotxmat * shiftmat;
	
	
	
	int ret;
	int isend = 0;
	SIGHEADER sh;

	ZeroMemory( &sh, sizeof( SIGHEADER ) );
	ret = LoadSigHeader( &sh );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	if( (sh.serialno != GNDFILEMAGICNO_1) && (sh.serialno != GNDFILEMAGICNO_2) && (sh.serialno != GNDFILEMAGICNO_3) && 
		(sh.serialno != GNDFILEMAGICNO_4) && (sh.serialno != GNDFILEMAGICNO_6) ){
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
		DbgOut( "GndFile : LoadGndFile : im2enableflag 1 !!!\n" );

	}else{
		lpsh->m_im2enableflag = 0;
		DbgOut( "GndFile : LoadGndFile : im2enableflag 0 !!!\n" );

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



	loadmagicno = sh.serialno;//!!!!!!!!!!!!!
	lpsh->m_sigmagicno = loadmagicno;


//// scene_ambient, shader, overflowの読み込み（MAGICNO_4以降）
	if( loadmagicno >= GNDFILEMAGICNO_4 ){
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

		lpsh->m_shader = COL_OLD;

		lpsh->m_overflow = OVERFLOW_ORG;
	}


///////
	if( m_offset == 0 ){
		ret = lpsh->InitMQOMaterial();
		if( ret ){
			DbgOut( "gndfile : LoadGNDFile_aft : sh InitMQOMaterial error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}
	m_materialoffset = lpsh->m_materialcnt - 1;


	ret = lpth->Start( m_offset );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

DbgOut( "GndFile : LoadGndFile : start Load Loop %d\n", m_offset );

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
			ret = LoadPart();
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
			ret = LoadPolyMesh( sh.flags );
			break;
		case SHDPOLYMESH2:
			ret = LoadPolyMesh2( m_adjustuvflag, sh.flags, sh.flags2 );
			break;
		case SHDTYPENONE:
		case SHDTYPEERROR:
		case SHDTYPEMAX:
			// error
			ret = 1;
			break;

		case SHDGROUNDDATA:
			ret = LoadGroundData();
			break;

		case -1:
			//！！！　終わりの目印。
			isend = 1;
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
		DbgOut( "GndFile : LoadGndFile : sh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = lpsh->SetColors();
	if( ret ){
		DbgOut( "GndFile : LoadGndFile : sh SetColors error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = lpmh->SetChain( offset );
	if( ret ){
		DbgOut( "GndFile : LoadGndFile : mh SetChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->SetMikoAnchorApply( lpth );
	if( ret ){
		DbgOut( "GndFile : LoadGndFile : sh SetMikoAnchorApply error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->Anchor2InfScope( lpth, lpmh );
	if( ret ){
		DbgOut( "gndfile : LoadGndFile : sh Anchor2InfScope error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = lpsh->CreateShdBBoxIfNot( lpth, lpmh );
	if( ret ){
		DbgOut( "gndfile : LoadGndFile : sh CreateShdBBoxIfNot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TLMODE
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	lpsh->m_TLmode = TLMODE_D3D;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	return 0;
}


int CGndFile::LoadInfluenceList( int listleng, int offset )
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
DbgOut( "GndFile : LoadInfluenceList : list %d - bno %d\n", listno, *(templist + listno) );

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
		int bnooffset = lpsh->m_lastboneno;
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

int CGndFile::LoadIgnoreList( int listleng, int offset )
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
DbgOut( "GndFile : LoadInfluenceList : list %d - bno %d\n", listno, *(templist + listno) );

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
		int bnooffset = lpsh->m_lastboneno;
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

int CGndFile::LoadExtTex( SIGHEADER sh )
{
	unsigned char exttexnum, exttexmode, exttexrep, exttexstep;

	exttexnum = (unsigned char)(sh.exttexmem >> 24);
	exttexmode = (unsigned char)((sh.exttexmem & 0x00FF0000) >> 16);
	exttexrep = (unsigned char)((sh.exttexmem & 0x0000FF00) >> 8);
	exttexstep = (unsigned char)(sh.exttexmem & 0x000000FF);

	if( exttexnum <= 0 )
		return 0;

	if( exttexnum > MAXEXTTEXNUM ){
		DbgOut( "GndFile : LoadExtTex : exttexnum error !!!\n" );
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
			DbgOut( "GndFile : LoadExtTex : LoadUCharData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = lpsh->SetExtTex( curseri, exttexnum, exttexmode, exttexrep, exttexstep, exttexlist );
	if( ret ){
		DbgOut( "GndFile : LoadExtTex : SetExtTex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


int CGndFile::AddShape2Tree( SIGHEADER sheader )
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

	ret = (*lpsh)( curseri )->SetDispSwitchNo( sheader.dispswitchno );
	_ASSERT( !ret );


	if( sheader.flags & FLAGS_NOTRANSIK ){
		(*lpsh)( curseri )->m_notransik = 1;
	}else{
		(*lpsh)( curseri )->m_notransik = 0;
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

int CGndFile::LoadGroundData()
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

int CGndFile::LoadPart()
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

	if( loadmagicno < GNDFILEMAGICNO_3 ){
		jvec.x *= m_datamult;//!!!!!!!!
		jvec.y *= m_datamult;
		jvec.z *= m_datamult;

	}else{
		D3DXVECTOR3 befv, aftv;
		befv.x = jvec.x;
		befv.y = jvec.y;
		befv.z = jvec.z;

		D3DXVec3TransformCoord( &aftv, &befv, &m_offsetmat );
		jvec.x = aftv.x;
		jvec.y = aftv.y;
		jvec.z = aftv.z;
	}

	CVec3f dstvec;
	dstvec.SetMem( jvec, VEC_ALL );

	CShdElem* curelem;
	curelem = (*lpsh)(curseri);
	ret = curelem->SetJointLoc( &dstvec );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CGndFile::LoadMorph()
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

int CGndFile::LoadPolyLine()
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

	if( loadmagicno < GNDFILEMAGICNO_3 ){

		if( m_datamult != 1.0f ){
			int vno;
			VEC3F* curv;
			for( vno = 0; vno < vnum; vno++ ){
				curv = vec + vno;
				curv->x *= m_datamult;
				curv->y *= m_datamult;
				curv->z *= m_datamult;
			}
		}

	}else{
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
int CGndFile::LoadPolygon()
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


		if( loadmagicno < GNDFILEMAGICNO_3 ){
			if( m_datamult != 1.0f ){
				int vno;
				VEC3F* curv;
				for( vno = 0; vno < vnum; vno++ ){
					curv = vec + vno;
					curv->x *= m_datamult;
					curv->y *= m_datamult;
					curv->z *= m_datamult;
				}
			}

		}else{		
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
int CGndFile::LoadSphere()
{
	int ret;
	CMatrix2 defmat;

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

	ZeroMemory( &defmat, sizeof( CMatrix2 ) );
	ret = LoadMatData( &defmat, 1 );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	if( loadmagicno < GNDFILEMAGICNO_3 ){

		if( m_datamult != 1.0f )
			defmat *= m_datamult;

	}else{
		if( m_datamult != 1.0f )
			defmat *= m_datamult;

		defmat[3][0] += m_offsetmat._41;
		defmat[3][1] += m_offsetmat._42;
		defmat[3][2] += m_offsetmat._43;
	}


	ret = (*lpsh)( curseri )->SetSphere( &defmat, SPH_DEFMAT );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CGndFile::LoadBezMesh()
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
int CGndFile::LoadBezLine()
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


		if( loadmagicno < GNDFILEMAGICNO_3 ){
			if( m_datamult != 1.0f ){
				int vno;
				VEC3F* curv;
				for( vno = 0; vno < vnum; vno++ ){
					curv = vec + vno;
					curv->x *= m_datamult;
					curv->y *= m_datamult;
					curv->z *= m_datamult;
				}
			}
		}else{
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
int CGndFile::LoadRevolved()
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


		if( loadmagicno < GNDFILEMAGICNO_3 ){

			if( m_datamult != 1.0f ){
				int vno;
				VEC3F* curv;
				for( vno = 0; vno < vnum; vno++ ){
					curv = vec + vno;
					curv->x *= m_datamult;
					curv->y *= m_datamult;
					curv->z *= m_datamult;
				}
			}
		}else{
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
int CGndFile::LoadDisk()
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
int CGndFile::LoadExtrude()
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


		if( loadmagicno < GNDFILEMAGICNO_3 ){
			if( m_datamult != 1.0f ){
				int vno;
				VEC3F* curv;
				for( vno = 0; vno < vnum; vno++ ){
					curv = vec + vno;
					curv->x *= m_datamult;
					curv->y *= m_datamult;
					curv->z *= m_datamult;
				}
			}
		}else{
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

int CGndFile::LoadPolyMesh2( int adjustuvflag, unsigned int flags, unsigned int flags2 )
{
	int ret;

DbgOut( "GndFile : start LoadPolyMesh2\n" );
 
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

	pm2->adjustuvflag = adjustuvflag;
	pm2->groundflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	VEC3F* dstvec = pm2->pointbuf;
	ret = LoadVecData( dstvec, facenum * 3 );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh2 : LoadVecData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int* dstsamep = pm2->samepointbuf;
	ret = LoadIntData( dstsamep, facenum * 3 );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh2 : LoadIntData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( flags2 & FLAGS2_OLD2OPT ){
		int* dstold2opt = pm2->oldpno2optpno;
		ret = LoadIntData( dstold2opt, facenum * 3 );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load old2opt error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( flags2 & FLAGS2_SMOOTHBUF ){
		int* dstsmooth = pm2->smoothbuf;
		ret = LoadIntData( dstsmooth, facenum * 3 );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load smooth error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	unsigned char* dstuv = (unsigned char*)pm2->uvbuf;
	ret = LoadUCharData( dstuv, sizeof( COORDINATE ) * facenum * 3 );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
//////////
	unsigned char* dstdiffuse = (unsigned char*)pm2->diffusebuf;
	ret = LoadUCharData( dstdiffuse, sizeof( ARGBF ) * facenum );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* dstambient = (unsigned char*)pm2->ambientbuf;
	ret = LoadUCharData( dstambient, sizeof( ARGBF ) * facenum );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	unsigned char* dstspecular = (unsigned char*)pm2->specularbuf;
	ret = LoadUCharData( dstspecular, sizeof( float ) * facenum );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh2 : LoadUCharData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( flags2 & FLAGS2_ORGNORMAL ){
		unsigned char* dstorgnormal = (unsigned char*)pm2->orgnormal;
		ret = LoadUCharData( dstorgnormal, sizeof( VEC3F ) * facenum * 3 );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load orgnormal error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		pm2->m_loadorgnormalflag = 1;//!!!!!!!!!!!!!!!!!!!!!!
	}


//////////
	int optpleng = 0;


	if( loadmagicno >= GNDFILEMAGICNO_2 ){
		
		//optデータを作る
		if( flags2 & FLAGS2_OLD2OPT ){
			int calcsmooth = 0;
			if( flags2 & FLAGS2_SMOOTHBUF )
				calcsmooth = 0;
			else
				calcsmooth = 1;

//DbgOut( "gndfile : LoadPolyMesh2 : calcsmooth %d\r\n", calcsmooth );

			ret = pm2->CreateOptDataByOld2Opt( adjustuvflag, curselem->facet, calcsmooth );
			if( ret ){
				DbgOut( "gndfile : LoadPolyMesh2 : CreateOptData error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			ret = pm2->CreateOptDataBySamePointBufLegacy( adjustuvflag, curselem->facet );
			if( ret ){
				DbgOut( "gndfile : LoadPolyMesh2 : CreateOptData by samepointbuf error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}

		//optplengの読み出し。
		ret = LoadIntData( &optpleng, 1 );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh2 : LoadIntData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		//optplengのチェック
		if( optpleng != pm2->optpleng ){
			DbgOut( "GndFile : LoadPolyMesh2 : optpleng error %d %d!!!\n", optpleng, pm2->optpleng );
			_ASSERT( 0 );
			return 1;
		}

		// バッファの作成
		DWORD* tempdw;
		tempdw = (DWORD*)malloc( sizeof( DWORD ) * optpleng );
		if( !tempdw ){
			DbgOut( "GndFile : LoadPolyMesh2 : tempdw alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		// diffuseの読み込み
		ret = LoadDWData( tempdw, optpleng );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh2 : LoadDWData error !!!\n" );
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
			DbgOut( "GndFile : LoadPolyMesh2 : LoadARGBData error !!!\n" );
			_ASSERT( 0 );
			free( tempdw );
			return 1;
		}

		//specularの読み込み
		ret = LoadDWData( tempdw, optpleng );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh2 : LoadDWData error !!!\n" );
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


	if( loadmagicno >= GNDFILEMAGICNO_4 ){
		ret = LoadFloatData( pm2->diffbuf, facenum );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load diffbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadFloatData( pm2->powerbuf, facenum );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadFloatData( pm2->emissivebuf, facenum );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load emissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		ret = LoadFloatData( pm2->optpowerbuf, optpleng );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm2->optemissivebuf, optpleng );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : load optemissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}



	if( flags & FLAGS_ORGNOBUF ){
		int* dstorgno;
		dstorgno = pm2->orgnobuf;
		if( !dstorgno ){
			DbgOut( "GndFile : LoadPolyMesh2 : pm2 orgnobuf not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadIntData( dstorgno, facenum * 3 );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh2 : LoadIntData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( flags2 & FLAGS2_VCOL ){
		int vcolnum;
		ret = LoadIntData( &vcolnum, 1 );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh2 : Load vcolnum error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int vcno;
		VCOLDATA curvc;
		for( vcno = 0; vcno < vcolnum; vcno++ ){
			ret = LoadUCharData( (unsigned char*)&curvc, sizeof( VCOLDATA ) );
			if( ret ){
				DbgOut( "gndfile : LoadPolyMesh2 : load curvc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			ret = pm2->SetVCol( curvc.vertno, curvc.vcol );
			if( ret ){
				DbgOut( "gndfile : LoadPolyMesh2 : pm2 SetVCol error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

		}

	}


	ret = pm2->MultMat( m_offsetmat );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh2 : pm2 MultMat error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMQOMaterial* newmat = 0;
	newmat = AddMQOMaterial( curselem );
	if( !newmat ){
		DbgOut( "gndfile : LoadPolyMesh2 : AddMQOMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	for( faceno = 0; faceno < facenum; faceno++ ){
		*( pm2->m_attrib0 + faceno ) = newmat->materialno;
	}



DbgOut( "GndFile : end LoadPolyMesh2\n" );

	return 0;

}


int CGndFile::LoadPolyMesh( unsigned int flags )
{
	int ret;

DbgOut( "GndFile : start LoadPolyMesh\n" );
 
	ret = LoadMIHeader( &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	int pointnum;
	pointnum = tempinfo.m;

	ret = Init3DObj();
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh : Init3DObj error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	VEC3F* ldvec;
	ldvec = (VEC3F*)malloc( sizeof( VEC3F ) * pointnum );
	if( !ldvec ){
		DbgOut( "GndFile : LoadPolyMesh : ldvec alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = LoadVecData( ldvec, pointnum );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh : LoadVecData error !!!\n" );
		_ASSERT( 0 );
		free( ldvec );
		return 1;
	}


	CShdElem* curselem;
	curselem = (*lpsh)( curseri );
	_ASSERT( curselem );


	if( loadmagicno < GNDFILEMAGICNO_3 ){
		if( m_datamult != 1.0f ){
			int vno;
			VEC3F* curv;
			for( vno = 0; vno < pointnum; vno++ ){
				curv = ldvec + vno;
				curv->x *= m_datamult;
				curv->y *= m_datamult;
				curv->z *= m_datamult;
			}
		}
	}else{

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


	ret = curselem->SetPolyMeshPoint( pointnum, ldvec );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh : SetPolyMeshPoint error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	free( ldvec );

	
	int* ldint;
	int facenum, dataleng;
	facenum = tempinfo.n;
	dataleng = facenum * 3;

	ret = curselem->CreatePolyMeshIndex( facenum );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh : CreatePolyMeshIndex error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ldint = (int*)malloc( sizeof( int ) * dataleng );
	if( !ldint ){
		DbgOut( "GndFile : LoadPolyMesh : ldint alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ret = LoadIntData( ldint, dataleng );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh : ldint alloc error !!!\n" );
		_ASSERT( 0 );
		free( ldint );
		return 1;
	}
	ret = curselem->SetPolyMeshIndex( dataleng, ldint, 0 );
	if( ret ){
		DbgOut( "GndFile : LoadPolyMesh : SetPolyMeshIndex error !!!\n" );
		_ASSERT( 0 );
		free( ldint );
		return 1;
	}
	free( ldint );



	CPolyMesh* pm;	
	pm = curselem->polymesh;
	_ASSERT( pm );


	pm->groundflag = 1;//!!!!!!!!!!!!!!!!!!!!!!!!!


	//頂点ごとの色情報の読み込み
	if( loadmagicno >= GNDFILEMAGICNO_2 ){
		_ASSERT( pm->diffusebuf );
		_ASSERT( pm->ambientbuf );
		_ASSERT( pm->specularbuf );

		ret = LoadARGBData( pm->diffusebuf, pointnum );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh : LoadARGBData : diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm->ambientbuf, pointnum );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh : LoadARGBData : diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm->specularbuf, pointnum );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh : LoadARGBData : diffuse error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	if( loadmagicno >= GNDFILEMAGICNO_4 ){
		_ASSERT( pm->powerbuf );
		_ASSERT( pm->emissivebuf );

		ret = LoadFloatData( pm->powerbuf, pointnum );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh : load powerbuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadARGBData( pm->emissivebuf, pointnum );
		if( ret ){
			DbgOut( "gndfile : LoadPolyMesh : load emissivebuf error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( flags & FLAGS_ORGNOBUF ){
		int* dstorgno;
		dstorgno = pm->orgnobuf;
		if( !dstorgno ){
			DbgOut( "GndFile : LoadPolyMesh : pm orgnobuf not exist error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = LoadIntData( dstorgno, pointnum );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh : LoadIntData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	if( flags & FLAGS_PMUVBUF ){
		curselem->texrule = TEXRULE_MQ;

		ret = pm->CreateTextureBuffer();
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh : pm CreateTextureBuffer error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		unsigned char* dstuv = (unsigned char*)pm->uvbuf;
		ret = LoadUCharData( dstuv, sizeof( COORDINATE ) * pointnum );
		if( ret ){
			DbgOut( "GndFile : LoadPolyMesh : LoadUCharData UV error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	CMQOMaterial* newmat = 0;
	newmat = AddMQOMaterial( curselem );
	if( !newmat ){
		DbgOut( "sigfile : LoadPolyMesh : AddMQOMaterial error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int faceno;
	for( faceno = 0; faceno < facenum; faceno++ ){
		*( pm->m_attrib0 + faceno ) = newmat->materialno;
	}


DbgOut( "GndFile : end LoadPolyMesh\n" );

	return 0;
}


int CGndFile::Init3DObj()
{
	int ret;

	ret = lpsh->Init3DObj( curseri, &tempinfo );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}
	return 0;
}

int CGndFile::CreateHandler()
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
int CGndFile::DestroyHandler()
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

CMQOMaterial* CGndFile::AddMQOMaterial( CShdElem* selem )
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
