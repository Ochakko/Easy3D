#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#define MOEEGLOBAL
#include <MOEEHelper.h>

#include <shdhandler.h>
#include <shdelem.h>

#define	DBGH
#include <dbg.h>

#include <crtdbg.h>




int CheckAndDelInvalidMOEE( CShdHandler* lpsh, MOEELEM* moeeptr )
{
	MOEELEM newmoee;
	ZeroMemory( &newmoee, sizeof( MOEELEM ) );

	newmoee.eventno = moeeptr->eventno;

	int setno1 = 0;
	int lno;
	for( lno = 0; lno < moeeptr->listnum; lno++ ){
		int curlist = moeeptr->list[lno];
		if( curlist == -1 ){
			newmoee.list[setno1] = curlist;
			setno1++;
		}else if( (curlist > 0) && (curlist < lpsh->s2shd_leng) ){
			CShdElem* selem;
			selem = (*lpsh)( curlist );
			_ASSERT( selem );
			if( selem->IsJoint() ){
				newmoee.list[setno1] = curlist;
				setno1++;
			}
		}
	}
	newmoee.listnum = setno1;


	int setno2 = 0;
	int nlno;
	for( nlno = 0; nlno < moeeptr->notlistnum; nlno++ ){
		int curlist = moeeptr->notlist[nlno];
		if( (curlist > 0) && (curlist < lpsh->s2shd_leng) ){
			CShdElem* selem;
			selem = (*lpsh)( curlist );
			_ASSERT( selem );
			if( selem->IsJoint() ){
				newmoee.notlist[setno2] = curlist;
				setno2++;
			}
		}
	}
	newmoee.notlistnum = setno2;

	*moeeptr = newmoee;

	return 0;
}

int DelMOEEListByIndex( int selindex, MOEELEM* moeeptr )
{
	MOEELEM newmoee;
	ZeroMemory( &newmoee, sizeof( MOEELEM ) );

	newmoee.eventno = moeeptr->eventno;

	int setno1 = 0;
	int lno;
	for( lno = 0; lno < moeeptr->listnum; lno++ ){
		int curlist = moeeptr->list[lno];
		if( lno != selindex ){
			newmoee.list[setno1] = curlist;
			setno1++;
		}
	}
	newmoee.listnum = setno1;


	int setno2 = 0;
	int nlno;
	for( nlno = 0; nlno < moeeptr->notlistnum; nlno++ ){
		int curlist = moeeptr->notlist[nlno];
		newmoee.notlist[setno2] = curlist;
		setno2++;
	}
	newmoee.notlistnum = setno2;

	*moeeptr = newmoee;
	return 0;
}

int DelMOEENotListByIndex( int selindex, MOEELEM* moeeptr )
{
	MOEELEM newmoee;
	ZeroMemory( &newmoee, sizeof( MOEELEM ) );

	newmoee.eventno = moeeptr->eventno;

	int setno1 = 0;
	int lno;
	for( lno = 0; lno < moeeptr->listnum; lno++ ){
		int curlist = moeeptr->list[lno];
		newmoee.list[setno1] = curlist;
		setno1++;
	}
	newmoee.listnum = setno1;


	int setno2 = 0;
	int nlno;
	for( nlno = 0; nlno < moeeptr->notlistnum; nlno++ ){
		int curlist = moeeptr->notlist[nlno];
		if( nlno != selindex ){
			newmoee.notlist[setno2] = curlist;
			setno2++;
		}
	}
	newmoee.notlistnum = setno2;

	*moeeptr = newmoee;
	return 0;
}

