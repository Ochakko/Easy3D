#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <mcache.h>

#define	DBGH
#include <dbg.h>



CMCache::CMCache()
{
	int i;

	for( i = 0; i < MCNUM; i++ ){
		sno[i] = -2;
		D3DXMatrixIdentity( &(realm[i]) );
		matptr[i] = (realm + i);
	}

	spos = MCNUM - 1;
}

CMCache::~CMCache()
{


}

