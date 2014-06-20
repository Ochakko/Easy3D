#ifndef QHANDLERH
#define QHANDLERH


#include <coef.h>
#include <D3DX9.h>

#include <crtdbg.h>
#include <basedat.h>

#include <quaternion2.h>
#include <math.h>

#define	DBGH
#include "dbg.h"


#define ID_ELEM_BIT		10
#define ID_ARRAY_BIT	(32 - ID_ELEM_BIT)

//#define MAX_Q_ELEMNUM	(pow( 2, ID_ELEM_BIT ) - 1)

class CQHandler
{

public:
	CQHandler();
	~CQHandler();

	int InitHandler();

	int CreateQ( int* dstqid ); 
	int DestroyQ( int srcqid );


	int InitQ( int qid );
	int SetQAxisAndDeg( int qid, D3DXVECTOR3 axisvec, float fdeg );
	int GetQAxisAndDeg( int qid, D3DXVECTOR3* axisvecptr, float* degptr );
	int RotateQ( int qid, D3DXVECTOR3 axisvec, float fdeg );
	int MultQ( int resqid, int befqid, int aftqid );
	int NormalizeQ( int qid );
	int CopyQ( int dstqid, int srcqid );

	int GetQ( int srcqid, CQuaternion2** dstppq );
	int SetQ( int dstqid, CQuaternion2* srcq );

private:
	int DestroyTempQ();
	int ExtendData();

	//int MakeQID( int srcarno, int srcelemno, int* dstqid );
	//int ParseQID( int srcqid, int* dstarno, int* dstelemno );

	int FindUnuseElem( int* dstflag, int* dstarno, int* dstelemno );

	//int SetUseFlag( int srcarno, int srcelemno, int srcflag );
	//int GetUseFlag( int srcarno, int srcelemno, int* dstflag );

	//int SetFulluseFlag( int srcarno );
	//int GetFulluseFlag( int srcarno, int* dstflag );


	__inline int CQHandler::MakeQID( int srcarno, int srcelemno, int* dstqid )
	{
		int newid;

		newid = (srcarno << ID_ELEM_BIT) | (srcelemno & elemmask);
		
		*dstqid = newid;

	//DbgOut( "qhandler : MakeQID : %d %d %x\n", srcarno, srcelemno, newid );

		return 0;
	};
	__inline int CQHandler::ParseQID( int srcqid, int* dstarno, int* dstelemno )
	{
		int arno, elemno;

		arno = (srcqid & arraymask) >> ID_ELEM_BIT;
		elemno = srcqid & elemmask;

		*dstarno = arno;
		*dstelemno = elemno;
		
		return 0;
	};

	__inline int CQHandler::SetUseFlag( int srcarno, int srcelemno, int srcflag )
	{
		//内部からしか呼ばないので、srcarno, srcelemnoの範囲チェックはしない。
	#ifdef _DEBUG
		if( (srcarno < 0) || (srcarno >= arraynum) ){
			DbgOut( "qhandler : SetUseFlag : srcarno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (srcelemno < 0) || (srcelemno >= maxqelemnum) ){
			DbgOut( "qhandler : SetUseFlag : srcelemno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	#endif
		
		int* utop;
		utop = *( useflag + srcarno );
		if( !utop ){
			DbgOut ( "qhandler : SetUseFlag : utop error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int* tuptr;
		tuptr = utop + srcelemno;
		if( !tuptr ){
			DbgOut( "qhandler : SetUseFlag : tuptr error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*tuptr = srcflag;

	//DbgOut( "qhandler : SetUseFlag : %d %d %d\n", srcarno, srcelemno, srcflag );	


		return 0;
	};

	__inline int CQHandler::GetUseFlag( int srcarno, int srcelemno, int* dstflag )
	{
		//内部からしか呼ばないので、srcarno, srcelemnoの範囲チェックはしない。
	#ifdef _DEBUG
		if( (srcarno < 0) || (srcarno >= arraynum) ){
			DbgOut( "qhandler : GetUseFlag : srcarno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		if( (srcelemno < 0) || (srcelemno >= maxqelemnum) ){
			DbgOut( "qhandler : GetUseFlag : srcelemno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	#endif
		
		int* utop;
		utop = *( useflag + srcarno );
		if( !utop ){
			DbgOut ( "qhandler : GetUseFlag : utop error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		int* tuptr;
		tuptr = utop + srcelemno;
		if( !tuptr ){
			DbgOut( "qhandler : GetUseFlag : tuptr error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		*dstflag = *tuptr;

		return 0;
	};

	__inline int CQHandler::SetFulluseFlag( int srcarno )
	{
		//内部からしか呼ばないので、srcarnoの範囲チェックはしない。
	#ifdef _DEBUG
		if( (srcarno < 0) || (srcarno >= arraynum) ){
			DbgOut( "qhandler : SetFulluseFlag : srcarno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	#endif
		int* utop;
		utop = *( useflag + srcarno );
		if( !utop ){
			DbgOut( "qhandler : SetFulluseFlag : utop error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		int elemno;
		int curflag;
		int findunuse = 0;
		//for( elemno = 0; elemno < maxqelemnum; elemno++ ){
		for( elemno = maxqelemnum - 1; elemno >= 0; elemno-- ){
			curflag = *( utop + elemno );
			if( curflag == 0 ){
				findunuse = 1;
				break;
			}
		}

		if( findunuse == 1 ){
			*( fulluseflag + srcarno ) = 0;
		}else{
			*( fulluseflag + srcarno ) = 1;
		}

		return 0;
	};

	__inline int CQHandler::GetFulluseFlag( int srcarno, int* dstflag )
	{
		//内部からしか呼ばないので、srcarnoの範囲チェックはしない。
	#ifdef _DEBUG
		if( (srcarno < 0) || (srcarno >= arraynum) ){
			DbgOut( "qhandler : GetFulluseFlag : srcarno error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
	#endif
		
		*dstflag = *( fulluseflag + srcarno );

		return 0;
	};

public:
	DWORD arraymask;
	DWORD elemmask;
	int maxqelemnum;

	int arraynum;

	CQuaternion2** ppq;

	int** useflag;
	int* fulluseflag;
};



#endif