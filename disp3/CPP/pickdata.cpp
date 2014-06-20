#include "stdafx.h"

#include <math.h>
#include <stdio.h>
#include <D3DX9.h>

#include <coef.h>
#define DBGH
#include <dbg.h>

#include <crtdbg.h>
#include <tchar.h>

#include <pickdata.h>


CPickData::CPickData()
{
	InitParams();
}
CPickData::~CPickData()
{

}

int CPickData::InitParams()
{
	int datano;

	for( datano = 0; datano < PICKDATAMAX; datano++ ){
		partno[ datano ] = -1;
		vertno[ datano ] = -1;
	}

	setflag = 0;
	scpos.x = 0.0f;
	scpos.y = 0.0f;
	scpos.z = 0.0f;

	dist2 = 1e16;


	return 0;
}



int CPickData::PickVert( int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DTLVERTEX* curtlv, int maxgetnum, int* getnumptr )
{
	float fx, fy;
	fx = (float)posx;
	fy = (float)posy;

	float frangex, frangey;
	frangex = (float)rangex;
	frangey = (float)rangey;

	float distx, disty;
	distx = (float)fabs( fx - curtlv->sx );
	disty = (float)fabs( fy - curtlv->sy );

	float curdist2;
	curdist2 = distx * distx + disty * disty;

	if( (distx <= frangex) && (disty <= frangey) ){
		int setnewflag = 0;

		if( setflag == 0 ){
			setnewflag = 1;
		}else{
			if( curdist2 <= dist2 ){
				setnewflag = 1;
			}else{
				setnewflag = 0;
			}
		}

		if( setnewflag != 0 ){

			//Šù‚ÉŠi”[‚µ‚Ä‚¢‚é‚Ì‚ÆA“¯‚¶À•W‚©’²‚×‚éB
			if( setflag == 0 ){
				setflag = 1;
				*getnumptr = 1;//////
			}else{
				if( (setflag == 1) && (scpos.x == curtlv->sx) && (scpos.y == curtlv->sy) && (scpos.z == curtlv->sz) ){
					(*getnumptr)++;
				}
			}

			int setindex;
			setindex = *getnumptr - 1;

			if( setindex >= maxgetnum ){
				DbgOut( "pickdata : PickVert : arrayleng too short warning !!!\n" );
				_ASSERT( 0 );
				return 0;//!!!!!!!!!!!!!!!!!
			}

			partno[ setindex ] = srcseri;
			vertno[ setindex ] = srcvertno;
			scpos.x = curtlv->sx;
			scpos.y = curtlv->sy;
			scpos.z = curtlv->sz;
			dist2 = curdist2;
				
		}
	}

	return 0;
}

int CPickData::PickVert( int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DXVECTOR3* curtlv, int maxgetnum, int* getnumptr )
{
	float fx, fy;
	fx = (float)posx;
	fy = (float)posy;

	float frangex, frangey;
	frangex = (float)rangex;
	frangey = (float)rangey;

	float distx, disty;
	distx = (float)fabs( fx - curtlv->x );
	disty = (float)fabs( fy - curtlv->y );

	float curdist2;
	curdist2 = distx * distx + disty * disty;

	if( (curtlv->z >= 0.0f) && (curtlv->z <= 1.0f) && (distx <= frangex) && (disty <= frangey) && (curdist2 < dist2) ){
		*getnumptr = 1;

		partno[ 0 ] = srcseri;
		vertno[ 0 ] = srcvertno;
		scpos.x = curtlv->x;
		scpos.y = curtlv->y;
		scpos.z = curtlv->z;
		dist2 = curdist2;

		setflag = 1;

//DbgOut( "check!!! : pickdata : init pick getnum 1 : partno %d, vertno %d\r\n", srcseri, srcvertno );
	}else if( (setflag == 1) && (scpos.x == curtlv->x) && (scpos.y == curtlv->y) && (scpos.z == curtlv->z) ){
		(*getnumptr)++;

		int setindex;
		setindex = *getnumptr - 1;

		if( setindex >= maxgetnum ){
			DbgOut( "pickdata : PickVert : arrayleng too short warning !!!\n" );
			_ASSERT( 0 );
			return 0;//!!!!!!!!!!!!!!!!!
		}

		partno[ setindex ] = srcseri;
		vertno[ setindex ] = srcvertno;
		scpos.x = curtlv->x;
		scpos.y = curtlv->y;
		scpos.z = curtlv->z;
		dist2 = curdist2;

//DbgOut( "check!!! : pickdata : add pick getnum %d : partno %d, vertno %d\r\n", *getnumptr, srcseri, srcvertno );

	}

	return 0;
}


/***
int CPickData::PickVert( int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DXVECTOR3* curtlv, int maxgetnum, int* getnumptr )
{
	float fx, fy;
	fx = (float)posx;
	fy = (float)posy;

	float frangex, frangey;
	frangex = (float)rangex;
	frangey = (float)rangey;

	float distx, disty;
	distx = (float)fabs( fx - curtlv->x );
	disty = (float)fabs( fy - curtlv->y );

	float curdist2;
	curdist2 = distx * distx + disty * disty;

	if( (curtlv->z >= 0.0f) && (curtlv->z <= 1.0f) && (distx <= frangex) && (disty <= frangey) ){
		int setnewflag = 0;

		if( setflag == 0 ){
			setnewflag = 1;
		}else{
			if( curdist2 <= dist2 ){
				setnewflag = 1;
			}else{
				setnewflag = 0;
			}
		}

		if( setnewflag != 0 ){

			//Šù‚ÉŠi”[‚µ‚Ä‚¢‚é‚Ì‚ÆA“¯‚¶À•W‚©’²‚×‚éB
			if( setflag == 0 ){
				setflag = 1;
				*getnumptr = 1;//////
			}else{
				if( (setflag == 1) && (scpos.x == curtlv->x) && (scpos.y == curtlv->y) && (scpos.z == curtlv->z) ){
					(*getnumptr)++;
				}
			}

			int setindex;
			setindex = *getnumptr - 1;

			if( setindex >= maxgetnum ){
				DbgOut( "pickdata : PickVert : arrayleng too short warning !!!\n" );
				_ASSERT( 0 );
				return 0;//!!!!!!!!!!!!!!!!!
			}

			partno[ setindex ] = srcseri;
			vertno[ setindex ] = srcvertno;
			scpos.x = curtlv->x;
			scpos.y = curtlv->y;
			scpos.z = curtlv->z;
			dist2 = curdist2;
				
		}
	}

	return 0;
}
***/

/***
int CPickData::PickVert( int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DTLVERTEX* curtlv, int maxgetnum, int* getnumptr )
{
	float fx, fy;
	fx = (float)posx;
	fy = (float)posy;

	float frangex, frangey;
	frangex = (float)rangex;
	frangey = (float)rangey;

	float distx, disty;
	distx = (float)fabs( fx - curtlv->sx );
	disty = (float)fabs( fy - curtlv->sy );

	float curdist2;
	curdist2 = distx * distx + disty * disty;

	if( (distx <= frangex) && (disty <= frangey) ){
		int setnewflag = 0;

		if( setflag == 0 ){
			setnewflag = 1;
		}else{
			if( curdist2 < dist2 ){
				setnewflag = 1;
			}else{
				setnewflag = 0;
			}
		}

		if( setnewflag != 0 ){
			setflag = 1;
			partno = srcseri;
			vertno = srcvertno;
			scpos.x = curtlv->sx;
			scpos.y = curtlv->sy;
			scpos.z = curtlv->sz;
			dist2 = curdist2;
		}
	}

	return 0;
}
***/

/***
int CPickData::PickVert( int srcseri, int srcvertno, int posx, int posy, int rangex, int rangey, D3DTLVERTEX* curtlv )
{
	float fx, fy;
	fx = (float)posx;
	fy = (float)posy;

	float maxdist2;
	float curdist2;

	maxdist2 = dist * dist;
	curdist2 = (fx - curtlv->sx) * (fx - curtlv->sx) + (fy - curtlv->sy) * (fy - curtlv->sy);

	if( curdist2 <= maxdist2 ){
		int setnewflag = 0;

		if( setflag == 0 ){
			setnewflag = 1;
		}else{
			if( curdist2 < dist2 ){
				setnewflag = 1;
			}else{
				setnewflag = 0;
			}
		}

		if( setnewflag != 0 ){
			setflag = 1;
			partno = srcseri;
			vertno = srcvertno;
			scpos.x = curtlv->sx;
			scpos.y = curtlv->sy;
			scpos.z = curtlv->sz;
			dist2 = curdist2;
		}
	}

	return 0;
}
***/