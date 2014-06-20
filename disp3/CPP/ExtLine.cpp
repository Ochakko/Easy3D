#include <stdafx.h> //�_�~�[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <ExtPoint.h>
#include <ExtLine.h>

#define	DBGH
#include <dbg.h>


static int s_allocno = 0;


CExtLine::CExtLine()
{
	CBaseDat::InitParams();

	InitParams();
	s_allocno++;
}

CExtLine::~CExtLine()
{

	CBaseDat::DestroyObjs();

	CExtPoint* curep = ephead;
	CExtPoint* nextep;
	while( curep ){
		nextep = curep->next;

		delete curep;
		curep = nextep;
	}
	ephead = 0;
	

	if( meshinfo ){
		delete meshinfo;
		meshinfo = 0;
	}

	pnum = 0;

}

void CExtLine::InitParams()
{
	lineid = s_allocno;//e3d���ŁA��ӂ̔ԍ�
	pnum = 0;
	ephead = 0;

	meshinfo = 0;
	//next = 0;
}


int CExtLine::AddExtPoint( int befepid, int ctrlflag, int* epidptr )
{
	// befpointid�̎��Ƀ`�F�C������Bbef==-1�̂Ƃ��Ō�ɁAbef==-2�̂Ƃ��ŏ��Ƀ`�F�C������

	*epidptr = -1;

	CExtPoint* newep;
	newep = new CExtPoint( ctrlflag );
	if( !newep ){
		DbgOut( "ExtLine : AddExtPoint : newep alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	int ret;

	if( befepid == -1 ){
		//�Ō�Ƀ`�F�C��
		CExtPoint* curep = ephead;
		CExtPoint* lastep = 0;
		while( curep ){
			lastep = curep;
			curep = curep->next;
		}

		if( lastep ){
			ret = lastep->AddToNext( newep );
			if( ret ){
				DbgOut( "ExtLine : AddExtPoint : -1 : lastep AddToNext error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}
		}else{
			//�܂��Apoint������Ȃ��̂ŁA�擪�ɁB
			_ASSERT( !ephead );
			ephead = newep;
			newep->ishead = 1;
		}

	}else if( befepid == -2 ){
		//�ŏ��Ƀ`�F�C��

		ret = ephead->AddToPrev( newep );
		if( ret ){
			DbgOut( "ExtLine : AddExtPoint : -2 : ephead AddToPrev error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( newep->ishead != 1 ){
			DbgOut ( "ExtLine : AddExtPoint : -2 : newep ishead error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}else{
		// befepid�̎��Ƀ`�F�C��

		CExtPoint* curep = ephead;
		CExtPoint* findep = 0;
		while( curep ){
			if( curep->pointid == befepid ){
				findep = curep;
				break;
			}
			curep = curep->next;
		}

		if( !findep ){
			DbgOut( "ExtLine : AddExtPoint : befepid : findep error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = findep->AddToNext( newep );
		if( ret ){
			DbgOut( "ExtLine : AddExtPoint : befepid : findep AddToNext error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

	}

	if( newep->ishead == 1 )
		ephead = newep;

	pnum++;


	*epidptr = newep->pointid;
	return 0;
}

CExtPoint* CExtLine::GetExtPoint( int epid )
{
	CExtPoint* curep = ephead;
	CExtPoint* findep = 0;
	while( curep ){
		if( curep->pointid == epid ){
			findep = curep;
			break;
		}
		curep = curep->next;
	}

	return findep;
}
CExtPoint* CExtLine::GetLastExtPoint()
{
	CExtPoint* curep = ephead;
	CExtPoint* lastep = 0;
	while( curep ){
		lastep = curep;

		curep = curep->next;
	}

	return lastep;
}



int CExtLine::RemoveExtPoint( int epid )
{
	CExtPoint* curep = ephead;
	CExtPoint* rmep = 0;
	while( curep ){
		if( curep->pointid == epid ){
			rmep = curep;
			break;
		}
		curep = curep->next;
	}

	if( !rmep ){
		DbgOut( "ExtLine : RemoveExtPoint : rmep not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int ret;

	ret = rmep->LeaveFromChain( &ephead );
	if( ret ){
		DbgOut( "ExtLine : RemoveExtPoint : rmep LeaveFromChain error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	delete rmep;

	pnum--;

	return 0;
}

int CExtLine::GetExtPointPos( int epid, D3DXVECTOR3* posptr )
{
	CExtPoint* curep;
	curep = GetExtPoint( epid );
	if( !curep ){
		DbgOut( "ExtLine : GetExtPointPos : GetExtPoint : epid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*posptr = curep->point;
	
	return 0;

}

int CExtLine::SetExtPointPos( int epid, D3DXVECTOR3* srcpos )
{
	CExtPoint* curep;
	curep = GetExtPoint( epid );
	if( !curep ){
		DbgOut( "ExtLine : SetExtPointPos : GetExtPoint : epid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	curep->point = *srcpos;

	return 0;
}



int CExtLine::GetNextExtPoint( int epid, int* nextptr )
{

	if( epid >= 0 ){
		CExtPoint* curep;
		curep = GetExtPoint( epid );
		if( !curep ){
			DbgOut( "ExtLine : GetNextExtPoint : GetExtPoint : epid not found error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( curep->next ){
			*nextptr = curep->next->pointid;
		}else{
			*nextptr = -1;
		}		
	}else{
		//�擪��ExtPoint��Ԃ�
		if( ephead ){
			*nextptr = ephead->pointid;
		}else{
			*nextptr = -1;
		}
	}

	return 0;
}

int CExtLine::GetPrevExtPoint( int epid, int* prevptr )
{
	CExtPoint* curep;
	curep = GetExtPoint( epid );
	if( !curep ){
		DbgOut( "ExtLine : GetPrevExtPoint : GetExtPoint : epid not found error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( curep->prev ){
		*prevptr = curep->prev->pointid;
	}else{
		*prevptr = -1;
	}
	return 0;
}

int CExtLine::SetMeshInfo( CMeshInfo* srcmeshinfo )
{
	if( meshinfo ){
		DbgOut( "extline : SetMeshInfo : meshinfo already exist error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	// meshinfo
	int ret;
	ret = srcmeshinfo->NewMeshInfo( &meshinfo );
	if( ret ){
		DbgOut( "ExtLine : SetMeshInfo ; srcmeshinfo->NewMeshinfo error !!!\n" );
		return 1;
	}

	_ASSERT( (meshinfo->type > SHDTYPENONE) && (meshinfo->type < SHDTYPEMAX) );

	ret = CBaseDat::SetType( meshinfo->type );
	if( ret ){
		DbgOut( "ExtLine : SetMeshInfo : base::SetType error !!!\n" );
		return 1;
	}

	return 0;
}

