#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <coef.h>

#define SYMMOPEDEC
#include <SymmOpe.h>

#include <shdhandler.h>
#include <shdelem.h>
#include <treehandler2.h>
#include <treeelem2.h>

#include <MotionPoint.h>
#include <MotionPoint2.h>

#include <crtdbg.h>

#define DBGH
#include <dbg.h>

int SOGetSymmNo( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* symmnoptr, int* symmtypeptr )
{
	int ret;
	*symmnoptr = 0;
	*symmtypeptr = SYMMTYPE_NONE;


	CTreeElem2* telem;
	telem = (*lpth)( serino );
	_ASSERT( telem );

	int namelen;
	namelen = (int)strlen( telem->name );
	if( namelen < 3 ){
		return 0;
	}

	char* pathead;
	pathead = telem->name + (namelen - 1) - 2;
	
	int cmp;
	int type;
	int findtype = 0;

	// パターン前半　3文字
	for( type = SYMMTYPE_XP; type <= SYMMTYPE_ZM; type++ ){
		cmp = strncmp( pathead, strsymmtype[type], 3 );
		if( cmp == 0 ){
			findtype = type;
			break;
		}
	}

	// パターン後半　2文字
	if( findtype == 0 ){
		for( type = SYMMTYPE_X; type <= SYMMTYPE_Z; type++ ){
			cmp = strncmp( pathead + 1, strsymmtype[type], 2 );
			if( cmp == 0 ){
				findtype = type;
				break;
			}
		}
	}


//DbgOut( "MotParamDlg : GetSymmNo : name %s, pathead %s, symmtype %d\n",
//	telem->name, pathead, findtype );

	if( findtype == 0 ){
		return 0;
	}else if( (findtype >= SYMMTYPE_X) && (findtype <= SYMMTYPE_Z) ){

// 対称パーツを探す必要がないので、ここで、return する。

		*symmtypeptr = findtype;
		*symmnoptr = serino;//!!!! 自分自身
		return 0;
	}


	char symmname[ 1024 ];
	ZeroMemory( symmname, sizeof( char ) * 1024 );

	strncpy_s( symmname, namelen + 1, telem->name, namelen - 3 );
	*(symmname + namelen - 3) = 0;

	switch( findtype ){
	case SYMMTYPE_XP:
		strcat_s( symmname, namelen + 1, "_X-" );
		break;
	case SYMMTYPE_XM:
		strcat_s( symmname, namelen + 1, "_X+" );
		break;

	case SYMMTYPE_YP:
		strcat_s( symmname, namelen + 1, "_Y-" );
		break;
	case SYMMTYPE_YM:
		strcat_s( symmname, namelen + 1, "_Y+" );
		break;

	case SYMMTYPE_ZP:
		strcat_s( symmname, namelen + 1, "_Z-" );
		break;
	case SYMMTYPE_ZM:
		strcat_s( symmname, namelen + 1, "_Z+" );
		break;

	default:
		DbgOut( "SymmOpe : GetSymmNo : findtype error !!!\n" );
		_ASSERT( 0 );
		return 1;
		break;
	}

	int findno = 0;
	ret = lpth->GetBoneNoByName( symmname, &findno, lpsh, 0 );
	if( ret ){
		DbgOut( "SymmOpe : GetSymmNo : GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	if( findno < 0 ){

//DbgOut( "MotParamDlg : GetSymmNo : findno zero : name %s, symmname %s\n",
//	telem->name, symmname );

		return 0;
	}

//DbgOut( "MotParamDlg : GetSymmNo : findno %d : name %s, symmname %s\n",
//	findno, telem->name, symmname );


/////
	*symmtypeptr = findtype;
	*symmnoptr = findno;

	return 0;
}

int SOGetSymmNo2( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* symmnoptr, int* symmtypeptr )
{
	int ret;
	*symmnoptr = 0;
	*symmtypeptr = SYMMTYPE_NONE;


	CTreeElem2* telem;
	telem = (*lpth)( serino );
	_ASSERT( telem );

	int namelen;
	namelen = (int)strlen( telem->name );
	if( namelen < 6 ){
		return 0;
	}

	char* pathead;
	pathead = telem->name + (namelen - 1) - 5;
	
	int cmp;
	int type;
	int findtype = 0;

	// 
	for( type = SYMMTYPE_XP; type <= SYMMTYPE_XM; type++ ){
		cmp = strncmp( pathead, strsymmtype2[type], 6 );
		if( cmp == 0 ){
			findtype = type;
			break;
		}
	}

	if( findtype == 0 ){
		return 0;
	}


	char symmname[ 1024 ];
	ZeroMemory( symmname, sizeof( char ) * 1024 );

	strncpy_s( symmname, namelen + 1, telem->name, namelen - 6 );
	*(symmname + namelen - 6) = 0;

	switch( findtype ){
	case SYMMTYPE_XP:
		strcat_s( symmname, namelen + 1, "[R]_X-" );
		break;
	case SYMMTYPE_XM:
		strcat_s( symmname, namelen + 1, "[L]_X+" );
		break;

	default:
		DbgOut( "SymmOpe : GetSymmNo2 : findtype error !!!\n" );
		_ASSERT( 0 );
		return 1;
		break;
	}

	int findno = 0;
	ret = lpth->GetBoneNoByName( symmname, &findno, lpsh, 0 );
	if( ret ){
		DbgOut( "SymmOpe : GetSymmNo2 : GetBoneNoByName error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}



	if( findno < 0 ){

		return 0;
	}


/////
	*symmtypeptr = findtype;
	*symmnoptr = findno;

	return 0;
}

int SOSetSymMotionPoint( int symmtype, CMotionPoint2* srcmp, CMotionPoint2* dstmp, int deginvflag )
{

	CMotionPoint2 tempmp;
	tempmp = *srcmp;

	D3DXQUATERNION symmxq;
	symmxq.w = tempmp.m_q.w;
	symmxq.x = tempmp.m_q.x;
	symmxq.y = tempmp.m_q.y;
	symmxq.z = tempmp.m_q.z;

	D3DXVECTOR3 symmaxis;
	float symmangle;

	D3DXQuaternionToAxisAngle( &symmxq, &symmaxis, &symmangle );		

	if( deginvflag != 0 ){
		symmangle *= -1.0f;//!!!
	}

	switch( symmtype ){
	case SYMMTYPE_XP:
	case SYMMTYPE_XM:
	case SYMMTYPE_X:
		symmaxis.x *= -1.0f;
		tempmp.m_mvx *= -1.0f;
		break;
	case SYMMTYPE_YP:
	case SYMMTYPE_YM:
	case SYMMTYPE_Y:
		symmaxis.y *= -1.0f;
		tempmp.m_mvy *= -1.0f;
		break;
	case SYMMTYPE_ZP:
	case SYMMTYPE_ZM:
	case SYMMTYPE_Z:
		symmaxis.z *= -1.0f;
		tempmp.m_mvz *= -1.0f;
		break;
	default:
		DbgOut( "SymmOpe : SetSymMotionPoint : symmtype error !!!\n" );
		_ASSERT( 0 );
		return 1;
		break;
	}

	D3DXQUATERNION newxq;
	D3DXQuaternionRotationAxis( &newxq, &symmaxis, symmangle );

	tempmp.m_q.w = newxq.w;
	tempmp.m_q.x = newxq.x;
	tempmp.m_q.y = newxq.y;
	tempmp.m_q.z = newxq.z;

	*dstmp = tempmp;

	return 0;
}
int SOSetSymMotionPoint( int symmtype, CMotionPoint* srcmp, CMotionPoint* dstmp, int deginvflag )
{

	CMotionPoint tempmp;
	tempmp = *srcmp;

	D3DXQUATERNION symmxq;
	symmxq.w = tempmp.m_q.w;
	symmxq.x = tempmp.m_q.x;
	symmxq.y = tempmp.m_q.y;
	symmxq.z = tempmp.m_q.z;

	D3DXVECTOR3 symmaxis;
	float symmangle;

	D3DXQuaternionToAxisAngle( &symmxq, &symmaxis, &symmangle );		

	if( deginvflag != 0 ){
		symmangle *= -1.0f;//!!!
	}

	switch( symmtype ){
	case SYMMTYPE_XP:
	case SYMMTYPE_XM:
	case SYMMTYPE_X:
		symmaxis.x *= -1.0f;
		tempmp.m_mvx *= -1.0f;
		break;
	case SYMMTYPE_YP:
	case SYMMTYPE_YM:
	case SYMMTYPE_Y:
		symmaxis.y *= -1.0f;
		tempmp.m_mvy *= -1.0f;
		break;
	case SYMMTYPE_ZP:
	case SYMMTYPE_ZM:
	case SYMMTYPE_Z:
		symmaxis.z *= -1.0f;
		tempmp.m_mvz *= -1.0f;
		break;
	default:
		DbgOut( "SymmOpe : SetSymMotionPoint : symmtype error !!!\n" );
		_ASSERT( 0 );
		return 1;
		break;
	}

	D3DXQUATERNION newxq;
	D3DXQuaternionRotationAxis( &newxq, &symmaxis, symmangle );

	tempmp.m_q.w = newxq.w;
	tempmp.m_q.x = newxq.x;
	tempmp.m_q.y = newxq.y;
	tempmp.m_q.z = newxq.z;

	*dstmp = tempmp;

	return 0;
}

int SOIsSymX( CTreeHandler2* lpth, int serino, int* isxptr )
{
	*isxptr = 0;

	if( (serino < 0) || (serino >= lpth->s2e_leng) ){
		_ASSERT( 0 );
		return 1;
	}

	char pat[10] = "_X";
	CTreeElem2* telem;
	telem = (*lpth)( serino );
	_ASSERT( telem );

	if( !telem->IsJoint() ){
		return 0;
	}

	int nameleng;
	nameleng = (int)strlen( telem->name );
	if( nameleng < 3 ){
		return 0;
	}

	char* pathead = 0;
	pathead = telem->name + nameleng - 2;

	int cmp;
	cmp = strcmp( pathead, pat );
	if( cmp == 0 ){
		*isxptr = 1;
	}

	return 0;
}


int SOIsSymXP( CTreeHandler2* lpth, int serino, int* isxpptr )
{
	*isxpptr = 0;

	if( (serino < 0) || (serino >= lpth->s2e_leng) ){
		_ASSERT( 0 );
		return 1;
	}

	char pat[10] = "_X+";
	CTreeElem2* telem;
	telem = (*lpth)( serino );
	_ASSERT( telem );

	if( !telem->IsJoint() ){
		return 0;
	}

	int nameleng;
	nameleng = (int)strlen( telem->name );
	if( nameleng < 4 ){
		return 0;
	}

	char* pathead = 0;
	pathead = telem->name + nameleng - 3;

	int cmp;
	cmp = strcmp( pathead, pat );
	if( cmp == 0 ){
		*isxpptr = 1;
	}

	return 0;
}
int SOIsSymXM( CTreeHandler2* lpth, int serino, int* isxmptr )
{
	*isxmptr = 0;

	if( (serino < 0) || (serino >= lpth->s2e_leng) ){
		_ASSERT( 0 );
		return 1;
	}

	char pat[10] = "_X-";
	CTreeElem2* telem;
	telem = (*lpth)( serino );
	_ASSERT( telem );

	if( !telem->IsJoint() ){
		return 0;
	}


	int nameleng;
	nameleng = (int)strlen( telem->name );
	if( nameleng < 4 ){
		return 0;
	}

	char* pathead = 0;
	pathead = telem->name + nameleng - 3;

	int cmp;
	cmp = strcmp( pathead, pat );
	if( cmp == 0 ){
		*isxmptr = 1;
	}

	return 0;
}

int SOGetSymXName( CTreeHandler2* lpth, CShdHandler* lpsh, int serino, int* setflagptr, char* dstname, int bufleng, CVec3f* dstpos )
{
	*setflagptr = 0;

	if( (serino < 0) || (serino >= lpth->s2e_leng) ){
		_ASSERT( 0 );
		return 1;
	}

	CTreeElem2* telem;
	telem = (*lpth)( serino );
	_ASSERT( telem );

	if( telem->type == SHDDESTROYED ){
		return 0;
	}
	if( !telem->IsJoint() ){
		return 0;
	}

	int nameleng;
	nameleng = (int)strlen( telem->name );
	if( nameleng < 4 ){
		return 0;
	}
	if( nameleng >= bufleng ){
		DbgOut( "symmope : SOGetSymXName : bufleng too short error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	enum {
		SOSYM_NONE,
		SOSYM_MIKOXP,
		SOSYM_MIKOXM,
		SOSYM_RDBXP,
		SOSYM_RDBXM,
		SOSYM_MAX
	};

	int findtype = SOSYM_NONE;
	if( nameleng >= 7 ){
		char patp[10] = "[L]_X+";
		char patm[10] = "[R]_X-";

		char* pathead;
		pathead = telem->name + nameleng - 6;
		int cmpp, cmpm;
		cmpp = strcmp( pathead, patp );
		if( cmpp == 0 ){
			findtype = SOSYM_MIKOXP;
		}
		cmpm = strcmp( pathead, patm );
		if( cmpm == 0 ){
			findtype = SOSYM_MIKOXM;
		}
	}

	if( findtype == SOSYM_NONE ){
		char patp[10] = "_X+";
		char patm[10] = "_X-";

		char* pathead;
		pathead = telem->name + nameleng - 3;
		int cmpp, cmpm;
		cmpp = strcmp( pathead, patp );
		if( cmpp == 0 ){
			findtype = SOSYM_RDBXP;
		}
		cmpm = strcmp( pathead, patm );
		if( cmpm == 0 ){
			findtype = SOSYM_RDBXM;
		}
	}

	if( findtype == SOSYM_NONE ){
		return 0;
	}

	int cpleng;

	switch( findtype ){
	case SOSYM_MIKOXP:
		cpleng = nameleng - 6;
		strncpy_s( dstname, bufleng, telem->name, cpleng );
		*( dstname + cpleng ) = 0;
		strcat_s( dstname, bufleng, "[R]_X-" );
		break;
	case SOSYM_MIKOXM:
		cpleng = nameleng - 6;
		strncpy_s( dstname, bufleng, telem->name, cpleng );
		*( dstname + cpleng ) = 0;
		strcat_s( dstname, bufleng, "[L]_X+" );
		break;
	case SOSYM_RDBXP:
		cpleng = nameleng - 3;
		strncpy_s( dstname, bufleng, telem->name, cpleng );
		*( dstname + cpleng ) = 0;
		strcat_s( dstname, bufleng, "_X-" );
		break;
	case SOSYM_RDBXM:
		cpleng = nameleng - 3;
		strncpy_s( dstname, bufleng, telem->name, cpleng );
		*( dstname + cpleng ) = 0;
		strcat_s( dstname, bufleng, "_X+" );
		break;
	default:
		_ASSERT( 0 );
		return 1;
		break;
	}

	CShdElem* selem;
	selem = (*lpsh)( serino );
	_ASSERT( selem );

	CPart* partptr;
	partptr = selem->part;
	if( !partptr ){
		_ASSERT( 0 );
		return 1;
	}

	CVec3f jloc;
	jloc = partptr->jointloc;
	dstpos->x = -jloc.x;
	dstpos->y = jloc.y;
	dstpos->z = jloc.z;

	*setflagptr = 1;

	return 0;
}

