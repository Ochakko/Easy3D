#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#define	DBGH
#include "dbg.h"

#include <crtdbg.h>
#include <qhandler.h>


CQHandler::CQHandler()
{
	arraynum = 0;

	ppq = 0;

	useflag = 0;
	fulluseflag = 0;

	arraymask = ( 0xFFFFFFFF >> ID_ELEM_BIT ) << ID_ELEM_BIT;
	elemmask = ( 0xFFFFFFFF << ID_ARRAY_BIT ) >> ID_ARRAY_BIT;
	maxqelemnum = (int)(pow( (double)2, (double)ID_ELEM_BIT ));

//DbgOut( "qhandler : maxqelemnum %d\n", maxqelemnum );

}

CQHandler::~CQHandler()
{

	DestroyTempQ();

	if( ppq ){
		CQuaternion2* qtop;
		int arno;
		for( arno = 0; arno < arraynum; arno++ ){
			qtop = *( ppq + arno );
			if( qtop ){
				delete [] qtop;
			}
		}

		free( ppq );
		ppq = 0;

	}
	
	if( useflag ){
		int* utop;
		int arno;
		for( arno = 0; arno < arraynum; arno++ ){
			utop = *( useflag + arno );
			if( utop ){
				free( utop );
			}
		}
		free( useflag );
		useflag = 0;
	}

	if( fulluseflag ){
		free( fulluseflag );
		fulluseflag = 0;
	}

	arraynum = 0;


}


int CQHandler::DestroyTempQ()
{

	if( ppq ){
		(*ppq)->DestroyTempQ();
	}
	return 0;
}



int CQHandler::InitHandler()
{

	if( ppq ){
		DbgOut( "qhandler : InitHandler : ppq already exist , return , warning !!!\n" );
		_ASSERT( 0 );
		return 0;
	}


	int ret;
	ret = ExtendData();
	if( ret ){
		DbgOut( "qhandler : InitHandler : ExtendData error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( ppq ){
		ret = (*ppq)->InitTempQ();
		if( ret ){
			DbgOut( "qhandler : InitHandler : CreateTempQ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}


	return 0;
}

int CQHandler::ExtendData()
{
	
	arraynum++;

	CQuaternion2* zeropq = 0;

	ppq = (CQuaternion2**)realloc( ppq, sizeof( CQuaternion2* ) * arraynum );
	if( !ppq ){
		DbgOut( "qhandler : InitHandler : ppq alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*( ppq + arraynum - 1 ) = zeropq;

	CQuaternion2* newq;
	newq = new CQuaternion2[ maxqelemnum ];
	if( !newq ){
		DbgOut( "qhandler : InitHandler : newq alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*( ppq + arraynum - 1 ) = newq;//!!!!!!!!!

	useflag = (int**)realloc( useflag, sizeof( int* ) * arraynum );
	if( !useflag ){
		DbgOut( "qhandler : InitHandler : useflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*( useflag + arraynum - 1 ) = 0;


	fulluseflag = (int*)realloc( fulluseflag, sizeof( int ) * arraynum );
	if( !fulluseflag ){
		DbgOut( "qhandler : InitHandler : fulluseflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	*( fulluseflag + arraynum - 1 ) = 0;


	int* nuseflag = 0;
	nuseflag = (int*)malloc( sizeof( int ) * maxqelemnum );
	if( !nuseflag ){
		DbgOut( "qhandler : InitHandler : nuseflag alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( nuseflag, sizeof( int ) * maxqelemnum );

	*( useflag + arraynum - 1) = nuseflag;//!!!!!!!!

	return 0;
}


int CQHandler::CreateQ( int* dstqid )
{
	int findflag = 0;
	int findarno = 0;
	int findelemno = 0;

	int ret;

	ret = FindUnuseElem( &findflag, &findarno, &findelemno );
	if( ret ){
		DbgOut( "qhandler : CreateQ : FindUnuseElem error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( findflag ){
		ret = MakeQID( findarno, findelemno, dstqid );
		if( ret ){
			DbgOut( "qhandler : CreateQ : MakeQID error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}else{

		//arraynumが上限を超えていないかチェック
		int arraynummax;
		arraynummax = (int)(pow( (double)2, (double)ID_ARRAY_BIT ));

//DbgOut( "qhandler : CreateQ : arraynummax %d\n", arraynummax );

		if( arraynum >= arraynummax ){
			DbgOut( "qhandler : CreateQ : datanum over 32bit range error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}


		//拡張する
		ret = ExtendData();
		if( ret ){
			DbgOut( "qhandler : CreateQ : ExtendData error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = FindUnuseElem( &findflag, &findarno, &findelemno );
		if( ret ){
			DbgOut( "qhandler : CreateQ : FindUnuseElem 2 error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( !findflag ){
			DbgOut( "qhandler : CreateQ : elem not found after extend error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		ret = MakeQID( findarno, findelemno, dstqid );
		if( ret ){
			DbgOut( "qhandler : CreateQ : MakeQID error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	}

	ret = SetUseFlag( findarno, findelemno, 1 );
	if( ret ){
		DbgOut( "qhandler : CreateQ : SetUseFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = SetFulluseFlag( findarno );
	if( ret ){
		DbgOut( "qhandler : CreateQ : SetFulluseFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = InitQ( *dstqid );
	if( ret ){
		DbgOut( "qhandler : CreateQ : InitQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CQHandler::DestroyQ( int srcqid )
{
	int ret;
	int arno, elemno;
	ret = ParseQID( srcqid, &arno, &elemno );
	if( ret ){
		DbgOut( "qhandler : DestroyQ : ParseQID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (arno < 0) || (arno >= arraynum) ){
		DbgOut( "qhandler : DestroyQ : arno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (elemno < 0) || (elemno >= maxqelemnum) ){
		DbgOut( "qhandler : DestroyQ : elemno error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = SetUseFlag( arno, elemno, 0 );
	if( ret ){
		DbgOut( "qhandler : DestroyQ : SetUseFlag error !!!\n ");
		_ASSERT( 0 );
		return 1;
	}
	
	ret = SetFulluseFlag( arno );
	if( ret ){
		DbgOut( "qhandler : DestroyQ : SetFulluseFlag error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CQHandler::GetQ( int srcqid, CQuaternion2** dstppq )
{
	int ret;
	int arno, elemno;

	ret = ParseQID( srcqid, &arno, &elemno );
	if( ret ){
		DbgOut( "qhandler : GetQ : ParseQID error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( (arno < 0) || (arno >= arraynum) ){
		DbgOut( "qhandler : GetQ : arno error !!!\n" );
		_ASSERT( 0 );
		*dstppq = 0;
		return 1;
	}

	if( (elemno < 0) || (elemno >= maxqelemnum) ){
		DbgOut( "qhandler : GetQ : elemno error !!!\n" );
		_ASSERT( 0 );
		*dstppq = 0;
		return 1;
	}

	int uflag;
	ret = GetUseFlag( arno, elemno, &uflag );
	if( ret ){
		DbgOut( "qhandler : GetQ : GetUseFlag error !!!\n" );
		_ASSERT( 0 );
		*dstppq = 0;
		return 1;
	}

	if( uflag == 0 ){
		DbgOut( "qhandler : GetQ : useflag 0 error %d %d!!!\n", arno, elemno );
		_ASSERT( 0 );
		*dstppq = 0;
		return 1;
	}


	CQuaternion2* qtop;
	qtop = *( ppq + arno );
	if( !qtop ){
		DbgOut( "qhandler : GetQ : qtop error !!!\n ");
		_ASSERT( 0 );
		*dstppq = 0;
		return 1;
	}

	CQuaternion2* tq;
	tq = qtop + elemno;
	if( !tq ){
		DbgOut( "qhandler : GetQ : tp error !!!\n" );
		_ASSERT( 0 );
		*dstppq = 0;
		return 1;
	}

	*dstppq = tq;

	return 0;
}


int CQHandler::FindUnuseElem( int* dstflag, int* dstarno, int* dstelemno )
{
	int ret;

	int arno;
	int fuflag;
	int findarno = -1;
	for( arno = 0; arno < arraynum; arno++ ){
		ret = GetFulluseFlag( arno, &fuflag );
		if( ret ){
			DbgOut( "qhandler : FindUnuseElem : GetFulluseFlag error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( fuflag == 0 ){
			findarno = arno;
			break;
		}
	}

	if( findarno < 0 ){
		// 空き要素無し。
		*dstflag = 0;
		return 0;
	}

	int elemno;
	int uflag;
	int findelemno = -1;
	for( elemno = 0; elemno < maxqelemnum; elemno++ ){
		ret = GetUseFlag( findarno, elemno, &uflag );
		if( ret ){
			DbgOut( "qhandler : FindUnuseElem : GetUseFlag error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( uflag == 0 ){
			findelemno = elemno;
			break;
		}
	}

	if( findelemno < 0 ){
		// 空き要素なし、　fulluseflag の内容と矛盾するので、エラーにする。
		*dstflag = 0;
		DbgOut( "qhandler : FindUnuseElem : findelemno not set error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstflag = 1;
	*dstarno = findarno;
	*dstelemno = findelemno;
	
	return 0;
}




int CQHandler::InitQ( int qid )
{
	int ret;
	CQuaternion2* qptr;
	ret = GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "qhandler : InitQ : GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	qptr->w = 1.0f;
	qptr->x = 0.0f;
	qptr->y = 0.0f;
	qptr->z = 0.0f;
	qptr->twist = 0.0f;

	return 0;
}
int CQHandler::SetQAxisAndDeg( int qid, D3DXVECTOR3 axisvec, float fdeg )
{
	int ret;
	CQuaternion2* qptr;
	ret = GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "qhandler : SetQAxisAndDeg : GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVec3Normalize( &axisvec, &axisvec );

	float frad;
	frad = fdeg * (float)DEG2PAI;

	ret = qptr->SetAxisAndRot( axisvec, frad );
	if( ret ){
		DbgOut( "qhandler : SetQAxisAndDeg : qptr SetAxisAndRot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//qptr->normalize();

	return 0;
}
int CQHandler::GetQAxisAndDeg( int qid, D3DXVECTOR3* axisvecptr, float* degptr )
{
	int ret;
	CQuaternion2* qptr;
	ret = GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "qhandler : SetQAxisAndDeg : GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float frad;
	ret = qptr->GetAxisAndRot( axisvecptr, &frad );
	if( ret ){
		DbgOut( "qhandler : SetQAxisAndDeg : qptr GetAxisAndRot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*degptr = frad * (float)PAI2DEG;

	return 0;
}
int CQHandler::RotateQ( int qid, D3DXVECTOR3 axisvec, float fdeg )
{
	int ret;
	CQuaternion2* qptr;
	ret = GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "qhandler : SetQAxisAndDeg : GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	D3DXVec3Normalize( &axisvec, &axisvec );
	float frad = fdeg * (float)DEG2PAI;

	CQuaternion2 multq;
	ret = multq.SetAxisAndRot( axisvec, frad );
	if( ret ){
		DbgOut( "qhandler : RotateQ : multq SetAxisAndRot error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	float savetwist = qptr->twist;
	*qptr = multq * *qptr; //後から掛ける
	qptr->twist = savetwist;

	return 0;
}
int CQHandler::MultQ( int resqid, int befqid, int aftqid )
{
	int ret;
	
	CQuaternion2* resqptr;
	ret = GetQ( resqid, &resqptr );
	if( ret ){
		DbgOut( "qhandler : MultQ : GetQ res error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* befqptr;
	ret = GetQ( befqid, &befqptr );
	if( ret ){
		DbgOut( "qhandler : MultQ : GetQ bef error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* aftqptr;
	ret = GetQ( aftqid, &aftqptr );
	if( ret ){
		DbgOut( "qhandler : MultQ : GetQ aft error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//float savetwist = befqptr->twist;//!!!!!!!!!!!!!

	
	// 0 - 360
	float finaltwist, settwist;
	finaltwist = befqptr->twist + aftqptr->twist;

	if( finaltwist >= 360.0f ){
		while( finaltwist >= 360.0f )
			finaltwist -= 360.0f;
		settwist = finaltwist;
	}else if( finaltwist < 0.0f ){
		while( finaltwist < 0.0f )
			finaltwist += 360.0f;
		settwist = finaltwist;
	}else{
		settwist = finaltwist;
	}
	

	CQuaternion2 tmpq;
	tmpq = *aftqptr * *befqptr;
	*resqptr = tmpq;
	//resqptr->twist = savetwist;

	resqptr->twist = settwist;//!!!!!!!!!!!!!!!!!!!!!!!!!


	return 0;
}

int CQHandler::NormalizeQ( int qid )
{
	int ret;
	
	CQuaternion2* qptr;
	ret = GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "qhandler : NormalizeQ : GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	qptr->normalize();

	return 0;
}

int CQHandler::CopyQ( int dstqid, int srcqid )
{
	int ret;

	CQuaternion2* dstqptr;
	ret = GetQ( dstqid, &dstqptr );
	if( ret ){
		DbgOut( "qhandler : CopyQ : GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* srcqptr;
	ret = GetQ( srcqid, &srcqptr );
	if( ret ){
		DbgOut( "qhandler : CopyQ : GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstqptr = *srcqptr;

	return 0;
}

int CQHandler::SetQ( int dstqid, CQuaternion2* srcq )
{
	int ret;

	CQuaternion2* dstqptr;
	ret = GetQ( dstqid, &dstqptr );
	if( ret ){
		DbgOut( "qhandler : SetQ : SetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	*dstqptr = *srcq;

	return 0;
}






