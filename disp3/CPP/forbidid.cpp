#include "stdafx.h"

#include <forbidid.h>

//#include <coef.h>
#define DBGH
#include <dbg.h>

#include <crtdbg.h>


int SetMCEForbidID( MCELEM* mcelem, int forbidnum, int* forbidid )
{
	if( mcelem->forbidid ){
		free( mcelem->forbidid );
		mcelem->forbidid = 0;
	}
	mcelem->forbidnum = 0;

	if( (forbidnum > 0) && forbidid ){
		mcelem->forbidid = (int*)malloc( sizeof( int ) * forbidnum );
		if( !mcelem->forbidid ){
			DbgOut( "forbidid : SetMCEForbidID : forbidid alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}

		memcpy( mcelem->forbidid, forbidid, sizeof( int ) * forbidnum );

		mcelem->forbidnum = forbidnum;
	}
	return 0;
}

int HitTestForbidID( MCELEM* mcelem, int srcid )
{
	int findflag = 0;

	int fno;
	for( fno = 0; fno < mcelem->forbidnum; fno++ ){
		int curforbid;
		curforbid = *( mcelem->forbidid + fno );
		if( curforbid == srcid ){
			findflag = 1;
			break;
		}
	}

	return findflag;
}

int IsSameForbidID( MCELEM* mcelem, int forbidnum, int* forbidid )
{
	int issame = 1;

	if( mcelem->forbidnum != forbidnum ){
		issame = 0;
	}else{
		int fno, chkfno;
		for( fno = 0; fno < forbidnum; fno++ ){
			int findflag = 0;
			int curforbid = *(mcelem->forbidid + fno);

			for( chkfno = 0; chkfno < forbidnum; chkfno++ ){
				if( curforbid == *( forbidid + chkfno ) ){
					findflag = 1;
					break;
				}
			}

			if( findflag == 0 ){
				issame = 0;
				break;
			}
		}
	}

	return issame;
}
