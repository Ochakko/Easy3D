#include "stdafx.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include <coef.h>
#include <crtdbg.h>

#include <shdelem.h>

#include <quaternion.h>

#define DBGH
#include <dbg.h>

float vecDotVec( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2 )
{
	return ( vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z );
}

float lengthVec( D3DXVECTOR3* vec )
{
	double mag;
	float leng;
	
	mag = vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
	if( mag == 0.0 ){
		leng = 0.0f;
	}else{
		leng = (float)sqrt( mag );
	}
	return leng;
}

float aCos( float dot )
{
	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;

	double rad;
	rad = acos( dot );

	float degree;
	degree = (float)( rad * PAI2DEG );

	return degree;
}

int vec3RotateY( D3DXVECTOR3* dstvec, float deg, D3DXVECTOR3* srcvec )
{

	int ret;
	CQuaternion dirq;
	D3DXMATRIX	dirm;

	ret = dirq.SetRotation( 0, deg, 0 );
	_ASSERT( !ret );
	dirm = dirq.MakeRotMatX();

	dstvec->x = dirm._11 * srcvec->x + dirm._21 * srcvec->y + dirm._31 * srcvec->z + dirm._41;
	dstvec->y = dirm._12 * srcvec->x + dirm._22 * srcvec->y + dirm._32 * srcvec->z + dirm._42;
	dstvec->z = dirm._13 * srcvec->x + dirm._23 * srcvec->y + dirm._33 * srcvec->z + dirm._43;

	return 0;
}
int vec3RotateX( D3DXVECTOR3* dstvec, float deg, D3DXVECTOR3* srcvec )
{

	int ret;
	CQuaternion dirq;
	D3DXMATRIX	dirm;

	ret = dirq.SetRotation( deg, 0, 0 );
	_ASSERT( !ret );
	dirm = dirq.MakeRotMatX();

	dstvec->x = dirm._11 * srcvec->x + dirm._21 * srcvec->y + dirm._31 * srcvec->z + dirm._41;
	dstvec->y = dirm._12 * srcvec->x + dirm._22 * srcvec->y + dirm._32 * srcvec->z + dirm._42;
	dstvec->z = dirm._13 * srcvec->x + dirm._23 * srcvec->y + dirm._33 * srcvec->z + dirm._43;

	return 0;
}
int vec3RotateZ( D3DXVECTOR3* dstvec, float deg, D3DXVECTOR3* srcvec )
{

	int ret;
	CQuaternion dirq;
	D3DXMATRIX	dirm;

	ret = dirq.SetRotation( 0, 0, deg );
	_ASSERT( !ret );
	dirm = dirq.MakeRotMatX();

	dstvec->x = dirm._11 * srcvec->x + dirm._21 * srcvec->y + dirm._31 * srcvec->z + dirm._41;
	dstvec->y = dirm._12 * srcvec->x + dirm._22 * srcvec->y + dirm._32 * srcvec->z + dirm._42;
	dstvec->z = dirm._13 * srcvec->x + dirm._23 * srcvec->y + dirm._33 * srcvec->z + dirm._43;

	return 0;
}

int qToEuler( CShdElem* selem, CQuaternion* srcq, D3DXVECTOR3* Euler )
{

	int ret;
	CQuaternion axisQ, invaxisQ, EQ;
	if( selem ){
		ret = selem->GetBoneAxisQ( &axisQ );
		if( ret ){
			DbgOut( "qToEuler : se GetBoneAxisQ error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		ret = axisQ.inv( &invaxisQ );
		_ASSERT( !ret );
		EQ = invaxisQ * *srcq * axisQ;
	}else{
		EQ = *srcq;
	}

	D3DXVECTOR3 axisXVec( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisYVec( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZVec( 0.0f, 0.0f, 1.0f );

	D3DXVECTOR3 targetVec, shadowVec;
	D3DXVECTOR3 tmpVec;

	EQ.Rotate( &targetVec, axisZVec );
	shadowVec.x = vecDotVec( &targetVec, &axisXVec );
	shadowVec.y = 0.0f;
	shadowVec.z = vecDotVec( &targetVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->y = 90.0f;
	}else{
		Euler->y = aCos( vecDotVec( &shadowVec, &axisZVec ) / lengthVec( &shadowVec ) );		
	}
	if( vecDotVec( &shadowVec, &axisXVec ) < 0.0f ){
		Euler->y = -Euler->y;
	}

/***
	E3DMultQVec targetQ,0.0,0.0,1.0,targetVec.0,targetVec.1,targetVec.2
	shadowVec.0= vecDotVec(targetVec,axisXVec)
	shadowVec.1= 0.0
	shadowVec.2= vecDotVec(targetVec,axisZVec)
	if( lengthVec(shadowVec) == 0 ){
		eulerY= 90.0
	}else{
		E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerY		//Yé≤âÒì]ìxêî
	}
	if( vecDotVec(shadowVec,axisXVec) < 0.0 ){ eulerY= -eulerY }				//âEâÒÇËÇ…èCê≥
***/
	vec3RotateY( &tmpVec, -Euler->y, &targetVec );
	shadowVec.x = 0.0f;
	shadowVec.y = vecDotVec( &tmpVec, &axisYVec );
	shadowVec.z = vecDotVec( &tmpVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->x = 90.0f;
	}else{
		Euler->x = aCos( vecDotVec( &shadowVec, &axisZVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisYVec ) > 0.0f ){
		Euler->x = -Euler->x;
	}
/***
	E3DVec3RotateY targetVec.0,targetVec.1,targetVec.2,(-eulerY),shadowVec.0,shadowVec.1,shadowVec.2
	if( lengthVec(shadowVec) == 0 ){
		eulerX= 90.0
	}else{
		E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerX		//Xé≤âÒì]ìxêî
	}
	if( vecDotVec(shadowVec,axisYVec) > 0.0 ){ eulerX= -eulerX }				//âEâÒÇËÇ…èCê≥
***/

	EQ.Rotate( &targetVec, axisYVec );
	vec3RotateY( &tmpVec, -Euler->y, &targetVec );
	targetVec = tmpVec;
	vec3RotateX( &tmpVec, -Euler->x, &targetVec );
	shadowVec.x = vecDotVec( &tmpVec, &axisXVec );
	shadowVec.y = vecDotVec( &tmpVec, &axisYVec );
	shadowVec.z = 0.0f;
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->z = 90.0f;
	}else{
		Euler->z = aCos( vecDotVec( &shadowVec, &axisYVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisXVec ) > 0.0f ){
		Euler->z = -Euler->z;
	}
/***
	E3DMultQVec targetQ,0.0,1.0,0.0,targetVec.0,targetVec.1,targetVec.2
	E3DVec3RotateY (targetVec.0+0.0),(targetVec.1+0.0),(targetVec.2+0.0),(-eulerY),targetVec.0,targetVec.1,targetVec.2
	E3DVec3RotateY targetVec.2,targetVec.0,targetVec.1,(-eulerX),shadowVec.2,shadowVec.0,shadowVec.1
	Å@Å@----> à¯êîÇ™Z,X,YÇ…Ç»Ç¡ÇƒÇ¢ÇÈÇΩÇﬂRotateXÇ∆ìØÇ∂ÅIÅIÅI
	
	if( lengthVec(shadowVec) == 0 ){
		eulerZ= 90.0
	}else{
		E3DACos ( vecDotVec(shadowVec,axisYVec)/ lengthVec(shadowVec) ), eulerZ		//Zé≤âÒì]ìxêî
	}
	if( vecDotVec(shadowVec,axisXVec) > 0.0 ){ eulerZ= -eulerZ }				//âEâÒÇËÇ…èCê≥

***/
	return 0;
}

int qToEulerAxis( CQuaternion axisQ, CQuaternion* srcq, D3DXVECTOR3* Euler )
{

	int ret;
	CQuaternion invaxisQ, EQ;
	ret = axisQ.inv( &invaxisQ );
	_ASSERT( !ret );
	EQ = invaxisQ * *srcq * axisQ;


	D3DXVECTOR3 axisXVec( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisYVec( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZVec( 0.0f, 0.0f, 1.0f );

	D3DXVECTOR3 targetVec, shadowVec;
	D3DXVECTOR3 tmpVec;

	EQ.Rotate( &targetVec, axisZVec );
	shadowVec.x = vecDotVec( &targetVec, &axisXVec );
	shadowVec.y = 0.0f;
	shadowVec.z = vecDotVec( &targetVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->y = 90.0f;
	}else{
		Euler->y = aCos( vecDotVec( &shadowVec, &axisZVec ) / lengthVec( &shadowVec ) );		
	}
	if( vecDotVec( &shadowVec, &axisXVec ) < 0.0f ){
		Euler->y = -Euler->y;
	}

/***
	E3DMultQVec targetQ,0.0,0.0,1.0,targetVec.0,targetVec.1,targetVec.2
	shadowVec.0= vecDotVec(targetVec,axisXVec)
	shadowVec.1= 0.0
	shadowVec.2= vecDotVec(targetVec,axisZVec)
	if( lengthVec(shadowVec) == 0 ){
		eulerY= 90.0
	}else{
		E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerY		//Yé≤âÒì]ìxêî
	}
	if( vecDotVec(shadowVec,axisXVec) < 0.0 ){ eulerY= -eulerY }				//âEâÒÇËÇ…èCê≥
***/
	vec3RotateY( &tmpVec, -Euler->y, &targetVec );
	shadowVec.x = 0.0f;
	shadowVec.y = vecDotVec( &tmpVec, &axisYVec );
	shadowVec.z = vecDotVec( &tmpVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->x = 90.0f;
	}else{
		Euler->x = aCos( vecDotVec( &shadowVec, &axisZVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisYVec ) > 0.0f ){
		Euler->x = -Euler->x;
	}
/***
	E3DVec3RotateY targetVec.0,targetVec.1,targetVec.2,(-eulerY),shadowVec.0,shadowVec.1,shadowVec.2
	if( lengthVec(shadowVec) == 0 ){
		eulerX= 90.0
	}else{
		E3DACos ( vecDotVec(shadowVec,axisZVec)/ lengthVec(shadowVec) ), eulerX		//Xé≤âÒì]ìxêî
	}
	if( vecDotVec(shadowVec,axisYVec) > 0.0 ){ eulerX= -eulerX }				//âEâÒÇËÇ…èCê≥
***/

	EQ.Rotate( &targetVec, axisYVec );
	vec3RotateY( &tmpVec, -Euler->y, &targetVec );
	targetVec = tmpVec;
	vec3RotateX( &tmpVec, -Euler->x, &targetVec );
	shadowVec.x = vecDotVec( &tmpVec, &axisXVec );
	shadowVec.y = vecDotVec( &tmpVec, &axisYVec );
	shadowVec.z = 0.0f;
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->z = 90.0f;
	}else{
		Euler->z = aCos( vecDotVec( &shadowVec, &axisYVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisXVec ) > 0.0f ){
		Euler->z = -Euler->z;
	}
/***
	E3DMultQVec targetQ,0.0,1.0,0.0,targetVec.0,targetVec.1,targetVec.2
	E3DVec3RotateY (targetVec.0+0.0),(targetVec.1+0.0),(targetVec.2+0.0),(-eulerY),targetVec.0,targetVec.1,targetVec.2
	E3DVec3RotateY targetVec.2,targetVec.0,targetVec.1,(-eulerX),shadowVec.2,shadowVec.0,shadowVec.1
	Å@Å@----> à¯êîÇ™Z,X,YÇ…Ç»Ç¡ÇƒÇ¢ÇÈÇΩÇﬂRotateXÇ∆ìØÇ∂ÅIÅIÅI
	
	if( lengthVec(shadowVec) == 0 ){
		eulerZ= 90.0
	}else{
		E3DACos ( vecDotVec(shadowVec,axisYVec)/ lengthVec(shadowVec) ), eulerZ		//Zé≤âÒì]ìxêî
	}
	if( vecDotVec(shadowVec,axisXVec) > 0.0 ){ eulerZ= -eulerZ }				//âEâÒÇËÇ…èCê≥

***/
	return 0;
}


int qToEulerYXZ( CShdElem* selem, CQuaternion* srcq, D3DXVECTOR3* Euler )
{

	//YXZÇÕÅAç¿ÇPÇÃÇ›Ç…ëŒâû

	D3DXVECTOR3 axisXVec( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 axisYVec( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 axisZVec( 0.0f, 0.0f, 1.0f );

	D3DXVECTOR3 targetVec, shadowVec;
	D3DXVECTOR3 tmpVec;

	srcq->Rotate( &targetVec, axisYVec );
	shadowVec.x = vecDotVec( &targetVec, &axisXVec );
	shadowVec.y = vecDotVec( &targetVec, &axisYVec );
	shadowVec.z = 0.0f;
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->z = 90.0f;
	}else{
		Euler->z = aCos( vecDotVec( &shadowVec, &axisYVec ) / lengthVec( &shadowVec ) );		
	}
	if( vecDotVec( &shadowVec, &axisXVec ) > 0.0f ){
		Euler->z = -Euler->z;
	}

/***
E3DMultQVec targetQ, 0.0, 1.0, 0.0, targetVec.0, targetVec.1, targetVec.2
shadowVec.0 = vecDotVec( targetVec, axisXVec )
shadowVec.1 = vecDotVec( targetVec, axisYVec )
shadowVec.2 = 0.0
if( lengthVec( shadowVec ) == 0.0 ){
	eulerZ = 90.0
}else{
	E3DACos( vecDotVec( shadowVec, axisYVecc ) / lengthVec( shadowVec ) ), eulerZ
}
if( vecDotVec( shadowVec, axisXVec ) > 0.0 ){ eulerZ= -eulerZ }
***/
	vec3RotateZ( &tmpVec, -Euler->z, &targetVec );
	shadowVec.x = 0.0f;
	shadowVec.y = vecDotVec( &tmpVec, &axisYVec );
	shadowVec.z = vecDotVec( &tmpVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->x = 90.0f;
	}else{
		Euler->x = aCos( vecDotVec( &shadowVec, &axisYVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisZVec ) < 0.0f ){
		Euler->x = -Euler->x;
	}
/***
Vec3RotateZ targetVec.0, targetVec.1, targetVec.2, (-eulerZ), tempVec.0, tempVec.1, tempVec.2
shadowVec.x = 0.0
shadowVec.y = vecDotVec( tempVec, axisYVec )
shadowVec.z = vecDotVec( tempVec, axisZVec )
if( lengthVec( shadowVec ) == 0 ){
	eulerX= 90.0
}else{
	E3DACos ( vecDotVec( shadowVec, axisYVec )/ lengthVec( shadowVec ) ), eulerX
}
if( vecDotVec(shadowVec,axisZVec) < 0.0 ){ eulerX= -eulerX }
***/
	srcq->Rotate( &targetVec, axisZVec );
	vec3RotateZ( &tmpVec, -Euler->z, &targetVec );
	targetVec = tmpVec;
	vec3RotateX( &tmpVec, -Euler->x, &targetVec );
	shadowVec.x = vecDotVec( &tmpVec, &axisXVec );
	shadowVec.y = 0.0f;
	shadowVec.z = vecDotVec( &tmpVec, &axisZVec );
	if( lengthVec( &shadowVec ) == 0.0f ){
		Euler->y = 90.0f;
	}else{
		Euler->y = aCos( vecDotVec( &shadowVec, &axisZVec ) / lengthVec( &shadowVec ) );
	}
	if( vecDotVec( &shadowVec, &axisXVec ) < 0.0f ){
		Euler->y = -Euler->y;
	}
/***
E3DMultQVec targetQ, 0.0, 0.0, 1.0, tempVec.0, tempVec.1, tempVec.2
Vec3RotateZ tempVec.0, tempVec.1, tempVec.2, (-eulerZ), tempVec2.0, tempVec2.1, tempVec2.2
Vec3RotateX tempVec2.0, tempVec2.1, tempVec2.2, (-eulerX), targetVec.0, targetVec.1, targetVec.2
shadowVec.0 = vecDotVec( targetVec, axisXVec )
shadowVec.1 = 0.0
shadowVec.2 = vecDotVec( targetVec, axisZVec )
if( lengthVec( shadowVec ) == 0 ){
	eulerY= 90.0
}else{
	E3DACos ( vecDotVec( shadowVec, axisZVec )/ lengthVec( shadowVec ) ), eulerY
}
if( vecDotVec(shadowVec,axisXVec) < 0.0 ){ eulerY= -eulerY }
***/
	return 0;
}




int GetRound( float srcval )
{
	if( srcval > 0.0f ){
		return (int)( srcval + 0.5f );
	}else{
		return (int)( srcval - 0.5f );
	}
}

int modifyEuler( D3DXVECTOR3* eulerA, D3DXVECTOR3* eulerB )
{

	//ÉIÉCÉâÅ[äpAÇÃílÇÉIÉCÉâÅ[äpBÇÃílÇ…ãﬂÇ¢ï\é¶Ç…èCê≥
	float tmpX1, tmpY1, tmpZ1;
	float tmpX2, tmpY2, tmpZ2;
	float s1, s2;

	//ó\ëzÇ≥ÇÍÇÈäpìx1
	tmpX1 = eulerA->x + 360.0f * GetRound( (eulerB->x - eulerA->x) / 360.0f );
	tmpY1 = eulerA->y + 360.0f * GetRound( (eulerB->y - eulerA->y) / 360.0f );
	tmpZ1 = eulerA->z + 360.0f * GetRound( (eulerB->z - eulerA->z) / 360.0f );

	//ó\ëzÇ≥ÇÍÇÈäpìx2
	tmpX2 = 180.0f - eulerA->x + 360.0f * GetRound( (eulerB->x + eulerA->x - 180.0f) / 360.0f );
	tmpY2 = eulerA->y + 180.0f + 360.0f * GetRound( (eulerB->y - eulerA->y - 180.0f) / 360.0f );
	tmpZ2 = eulerA->z + 180.0f + 360.0f * GetRound( (eulerB->z - eulerA->z - 180.0f) / 360.0f );

	//äpìxïœâªÇÃëÂÇ´Ç≥
	s1 = (eulerB->x - tmpX1) * (eulerB->x - tmpX1) + (eulerB->y - tmpY1) * (eulerB->y - tmpY1) + (eulerB->z - tmpZ1) * (eulerB->z - tmpZ1);
	s2 = (eulerB->x - tmpX2) * (eulerB->x - tmpX2) + (eulerB->y - tmpY2) * (eulerB->y - tmpY2) + (eulerB->z - tmpZ2) * (eulerB->z - tmpZ2);

	//ïœâªÇÃè≠Ç»Ç¢ï˚Ç…èCê≥
	if( s1 < s2 ){
		eulerA->x = tmpX1; eulerA->y = tmpY1; eulerA->z = tmpZ1;
	}else{
		eulerA->x = tmpX2; eulerA->y = tmpY2; eulerA->z = tmpZ2;
	}

	return 0;
}


