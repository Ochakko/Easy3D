#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include <windows.h>

#define AUTOSPLINECPP
#include <AutoSpline.h>

#include <mothandler.h>
#include <motionctrl.h>
#include <motioninfo.h>
#include <motionpoint2.h>
#include <spparam.h>

#define	DBGH
#include <dbg.h>
#include <crtdbg.h>


int AutoSplineAll( CMotHandler* lpmh, int motid, int boneno, int mkind, int changefumode )
{
	int ret;
	CMotionCtrl* mcptr;
	mcptr = (*lpmh)( boneno );
	_ASSERT( mcptr );

	CMotionPoint2* mpptr = 0;
	mpptr = mcptr->GetMotionPoint( motid );

	while( mpptr ){
		ret = AutoSplineOne( mkind, mpptr, changefumode );
		if( ret ){
			DbgOut( "AutoSpline : AutoSplineAll : AutoSplineOne error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		mpptr = mpptr->next;
	}

	return 0;
}
int AutoSplineOne( int mkind, CMotionPoint2* curmp, int changefumode )
{
	CALCSP calcsp[3];
	ZeroMemory( &calcsp, sizeof( CALCSP ) );

	float yvalmax[3];
	yvalmax[MKIND_ROT] = 15.0f;
	yvalmax[MKIND_MV] = 15.0f;
//	yvalmax[MKIND_SC] = 0.1f;
	yvalmax[MKIND_SC] = 5.0f;

	float tatemax[3];
	tatemax[MKIND_ROT] = 30.0f;
	tatemax[MKIND_MV] = 100.0f;
//	tatemax[MKIND_SC] = 0.3f;
	tatemax[MKIND_SC] = 0.1f;

	CMotionPoint2* befmp = 0;
	CMotionPoint2* aftmp = 0;
	CMotionPoint2* faraftmp = 0;
	SPPARAM* curspp = 0;
	SPPARAM* befspp = 0;
	SPPARAM* aftspp = 0;
	SPPARAM* faraftspp = 0;

	befmp = curmp->prev;
	aftmp = curmp->next;
	if( aftmp ){
		faraftmp = aftmp->next;
	}

	switch( mkind ){
	case MKIND_ROT:
		curspp = &(curmp->m_spp->m_rotparam);
		if( befmp ){
			befspp = &(befmp->m_spp->m_rotparam);
		}
		if( aftmp ){
			aftspp = &(aftmp->m_spp->m_rotparam);
		}
		if( faraftmp ){
			faraftspp = &(faraftmp->m_spp->m_rotparam);
		}
		break;
	case MKIND_MV:
		curspp = &(curmp->m_spp->m_mvparam);
		if( befmp ){
			befspp = &(befmp->m_spp->m_mvparam);
		}
		if( aftmp ){
			aftspp = &(aftmp->m_spp->m_mvparam);
		}
		if( faraftmp ){
			faraftspp = &(faraftmp->m_spp->m_mvparam);
		}
		break;
	case MKIND_SC:
		curspp = &(curmp->m_spp->m_scparam);
		if( befmp ){
			befspp = &(befmp->m_spp->m_scparam);
		}
		if( aftmp ){
			aftspp = &(aftmp->m_spp->m_scparam);
		}
		if( faraftmp ){
			faraftspp = &(faraftmp->m_spp->m_scparam);
		}
		break;
	default:
		_ASSERT( 0 );
		break;
	}

	if( curspp->lockflag == 1 ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	if( !aftmp ){
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	float yokodiff = (float)( aftmp->m_frameno - curmp->m_frameno );

	float tatediff[3];
	float beftatediff[3];
	float afttatediff[3];
	float yokoscale;
	float tatescale;
	switch( mkind ){
	case MKIND_ROT:
		tatediff[INDX] = aftmp->m_eul.x - curmp->m_eul.x;
		tatediff[INDY] = aftmp->m_eul.y - curmp->m_eul.y;
		tatediff[INDZ] = aftmp->m_eul.z - curmp->m_eul.z;
		
		if( befmp ){
			beftatediff[INDX] = curmp->m_eul.x - befmp->m_eul.x;
			beftatediff[INDY] = curmp->m_eul.y - befmp->m_eul.y;
			beftatediff[INDZ] = curmp->m_eul.z - befmp->m_eul.z;
		}
	
		if( faraftmp ){
			afttatediff[INDX] = faraftmp->m_eul.x - aftmp->m_eul.x;
			afttatediff[INDY] = faraftmp->m_eul.y - aftmp->m_eul.y;
			afttatediff[INDZ] = faraftmp->m_eul.z - aftmp->m_eul.z;
		}

		yokoscale = 0.3f;
		tatescale = 1.0f;

		break;
	case MKIND_MV:
		tatediff[INDX] = aftmp->m_mvx - curmp->m_mvx;
		tatediff[INDY] = aftmp->m_mvy - curmp->m_mvy;
		tatediff[INDZ] = aftmp->m_mvz - curmp->m_mvz;

		if( befmp ){
			beftatediff[INDX] = curmp->m_mvx - befmp->m_mvx;
			beftatediff[INDY] = curmp->m_mvy - befmp->m_mvy;
			beftatediff[INDZ] = curmp->m_mvz - befmp->m_mvz;
		}

		if( faraftmp ){
			afttatediff[INDX] = faraftmp->m_mvx - aftmp->m_mvx;
			afttatediff[INDY] = faraftmp->m_mvy - aftmp->m_mvy;
			afttatediff[INDZ] = faraftmp->m_mvz - aftmp->m_mvz;
		}

		yokoscale = 0.3f;
		tatescale = 2.0f;

		break;
	case MKIND_SC:
		tatediff[INDX] = aftmp->m_scalex - curmp->m_scalex;
		tatediff[INDY] = aftmp->m_scaley - curmp->m_scaley;
		tatediff[INDZ] = aftmp->m_scalez - curmp->m_scalez;

		if( befmp ){
			beftatediff[INDX] = curmp->m_scalex - befmp->m_scalex;
			beftatediff[INDY] = curmp->m_scaley - befmp->m_scaley;
			beftatediff[INDZ] = curmp->m_scalez - befmp->m_scalez;
		}

		if( faraftmp ){
			afttatediff[INDX] = faraftmp->m_scalex - aftmp->m_scalex;
			afttatediff[INDY] = faraftmp->m_scaley - aftmp->m_scaley;
			afttatediff[INDZ] = faraftmp->m_scalez - aftmp->m_scalez;
		}

		yokoscale = 0.3f;
		tatescale = 1.0f;
//		yokoscale = 0.5f;
//		tatescale = 1.0f;

		break;
	}


	int ikind;
	for( ikind = INDX; ikind <= INDZ; ikind++ ){
		float setyval;
		setyval = min( yvalmax[mkind], (yokodiff * yokoscale * min( tatescale, (fabs( tatediff[ikind] ) / tatemax[mkind]))) );

//前半
		if( befmp ){
			if( tatediff[ikind] > 0.0f ){
				if( beftatediff[ikind] > 0.0f ){
					calcsp[ikind].fugouS = FUGOU_ZERO;
					calcsp[ikind].yvalS = 0.0f;					
				}else if( beftatediff[ikind] < 0.0f ){
					calcsp[ikind].fugouS = FUGOU_MINUS;
					calcsp[ikind].yvalS = -setyval;					
				}else{
					calcsp[ikind].fugouS = FUGOU_MINUS;
					calcsp[ikind].yvalS = -setyval;					
				}
			}else if( tatediff[ikind] < 0.0f ){
				if( beftatediff[ikind] > 0.0f ){
					calcsp[ikind].fugouS = FUGOU_PLUS;
					calcsp[ikind].yvalS = setyval;
				}else if( beftatediff[ikind] < 0.0f ){
					calcsp[ikind].fugouS = FUGOU_ZERO;
					calcsp[ikind].yvalS = 0.0f;					
				}else{
					calcsp[ikind].fugouS = FUGOU_PLUS;
					calcsp[ikind].yvalS = setyval;
				}
			}else{
				calcsp[ikind].fugouS = FUGOU_ZERO;
				calcsp[ikind].yvalS = 0.0f;					
			}			
		}else{
			if( tatediff[ikind] > 0.0f ){
				calcsp[ikind].fugouS = FUGOU_MINUS;
				calcsp[ikind].yvalS = -setyval;
			}else if( tatediff[ikind] < 0.0f ){
				calcsp[ikind].fugouS = FUGOU_PLUS;
				calcsp[ikind].yvalS = setyval;
			}else{
				calcsp[ikind].fugouS = FUGOU_ZERO;
				calcsp[ikind].yvalS = 0.0f;
			}
		}

//後半
		if( faraftmp ){
			if( tatediff[ikind] > 0.0f ){
				if( afttatediff[ikind] > 0.0f ){
					calcsp[ikind].fugouE = FUGOU_ZERO;
					calcsp[ikind].yvalE = 0.0f;
				}else if( afttatediff[ikind] < 0.0f ){
					calcsp[ikind].fugouE = FUGOU_PLUS;
					calcsp[ikind].yvalE = setyval;
				}else{
					calcsp[ikind].fugouE = FUGOU_PLUS;
					calcsp[ikind].yvalE = setyval;
				}
			}else if( tatediff[ikind] < 0.0f ){
				if( afttatediff[ikind] > 0.0f ){
					calcsp[ikind].fugouE = FUGOU_MINUS;
					calcsp[ikind].yvalE = -setyval;
				}else if( afttatediff[ikind] < 0.0f ){
					calcsp[ikind].fugouE = FUGOU_ZERO;
					calcsp[ikind].yvalE = 0.0f;				
				}else{
					calcsp[ikind].fugouE = FUGOU_MINUS;
					calcsp[ikind].yvalE = -setyval;
				}
			}else{
				calcsp[ikind].fugouE = FUGOU_ZERO;
				calcsp[ikind].yvalE = 0.0f;					
			}

		}else{
			if( tatediff[ikind] > 0.0f ){
				calcsp[ikind].fugouE = FUGOU_PLUS;
				calcsp[ikind].yvalE = setyval;
			}else if( tatediff[ikind] < 0.0f ){
				calcsp[ikind].fugouE = FUGOU_MINUS;
				calcsp[ikind].yvalE = -setyval;
			}else{
				calcsp[ikind].fugouE = FUGOU_ZERO;
				calcsp[ikind].yvalE = 0.0f;
			}
		}

/////////////////////
		if( changefumode == 1 ){
			curmp->interp = INTERPOLATION_SQUAD;
		}


		curspp->ctrlS[ikind].spt = 0.1f;
		curspp->ctrlE[ikind].spt = 0.9f;


		curspp->ctrlS[ikind].spdist = calcsp[ikind].yvalS;
		if( calcsp[ikind].yvalS > curspp->ctrlS[ikind].distmax ){
			curspp->ctrlS[ikind].distmax = calcsp[ikind].yvalS;
		}
		if( calcsp[ikind].yvalS < -curspp->ctrlS[ikind].distmax ){
			curspp->ctrlS[ikind].distmax = -calcsp[ikind].yvalS;
		}

		curspp->ctrlE[ikind].spdist = calcsp[ikind].yvalE;
		if( calcsp[ikind].yvalE > curspp->ctrlE[ikind].distmax ){
			curspp->ctrlE[ikind].distmax = calcsp[ikind].yvalE;
		}
		if( calcsp[ikind].yvalE < -curspp->ctrlE[ikind].distmax ){
			curspp->ctrlE[ikind].distmax = -calcsp[ikind].yvalE;
		}
	}


	return 0;
}

