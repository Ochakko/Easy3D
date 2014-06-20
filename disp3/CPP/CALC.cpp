//#include	"stdafx.h"
#include	<Calc.h>
#include	<Coef.h>
#include	<math.h>
//#include	<gl.h>

//static float sinindex[720]
//static float cosindex[720] = {
//static float sqrtindex[1001] = {
//static float atanindexpp[1000] = {
//static float atanindexmp[1000] = {
//static float atanindexpm[1000] = {
//static float atanindexmm[1000] = {
//static float tanindex[720] = {

extern int	LoadCalcData( char* path, float* retdata, int datarange );

CCalc::CCalc()
{
	int	indexno;

	for( indexno = 0; indexno < 16; indexno++ ){
		rotXorg[indexno] = 0.0f;
		rotYorg[indexno] = 0.0f;
		rotZorg[indexno] = 0.0f;
		rotXYZorg[indexno] = 0.0f;
		traorg[indexno] = 0.0f;
		scaleorg[indexno] = 0.0f;
	}

	rotXorg[0] = 1.0f;
	rotXorg[15] = 1.0f;
	rotYorg[5] = 1.0f;
	rotYorg[15] = 1.0f;
	rotZorg[10] = 1.0f;
	rotZorg[15] = 1.0f;
	rotXYZorg[15] = 1.0f;
	traorg[0] = 1.0f;
	traorg[5] = 1.0f;
	traorg[10] = 1.0f;
	traorg[15] = 1.0f;
	scaleorg[0] = 1.0f; 
	scaleorg[5] = 1.0f;
	scaleorg[10] = 1.0f;
	scaleorg[15] = 1.0f;

	//InitRGB500();

	InitFuncIndex();
}

void CCalc::InitFuncIndex()
{
	char	calcdir[50];
	char	datapath[200];
	//DWORD	findvar;

	memset( calcdir, 0, 50 );
	/***
	findvar = GetEnvironmentVariable( "ZCALCDAT", calcdir, 50 );
	if( findvar == 0 ){
		TRACE( "CALCCPP : ZCALCDAT directory var not found !!!!! error !!!\n" );
		return;
	}
	***/
	strcpy( calcdir, "C:\\disp0\\CALCDAT\\" );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "sinindex720.dat" );
	LoadCalcData( datapath, sinindex, 720 );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "cosindex720.dat" );
	LoadCalcData( datapath, cosindex, 720 );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "sqrtindex1001.dat" );
	LoadCalcData( datapath, sqrtindex, 1001 );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "atanindexpp1000.dat" );
	LoadCalcData( datapath, atanindexpp, 1000 );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "atanindexmp1000.dat" );
	LoadCalcData( datapath, atanindexmp, 1000 );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "atanindexpm1000.dat" );
	LoadCalcData( datapath, atanindexpm, 1000 );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "atanindexmm1000.dat" );
	LoadCalcData( datapath, atanindexmm, 1000 );

	memset( datapath, 0, 200 );
	strcpy( datapath, calcdir );
	strcat( datapath, "tanindex720.dat" );
	LoadCalcData( datapath, tanindex, 720 );

}


CCalc::~CCalc()
{
}

//HANDLE	CCalc::s_hHeap = NULL;
//unsigned int	CCalc::s_uNumAllocsInHeap = 0;
/***
void	*CCalc::operator new ( size_t size )
{
	//TRACE( "CALC CREATE size %d\n", size );


	if( s_hHeap == NULL ){
		s_hHeap = HeapCreate( HEAP_NO_SERIALIZE, 0, 0 );
		
		if( s_hHeap == NULL )
			return NULL;
	}

	void	*p;
	p = (void *)HeapAlloc( s_hHeap, HEAP_NO_SERIALIZE, size );
		
	if( p != NULL ){
		s_uNumAllocsInHeap++;
	}

	return (p);
}

void	CCalc::operator delete ( void *p )
{
	if( HeapFree( s_hHeap, HEAP_NO_SERIALIZE, p ) ){
		s_uNumAllocsInHeap--;
	}

	if( s_uNumAllocsInHeap == 0 ){
		if( HeapDestroy( s_hHeap ) ){
			s_hHeap = NULL;
		}
	}
}
***/

float CCalc::DEGCos( float deg )
{
	int kaku;

	kaku = (int)(deg * 2) % 720;
	if( kaku < 0 )
		kaku += 720;
	
	return ( cosindex[ kaku ] );
}

float CCalc::DEGSin( float deg )
{ 
	int kaku;

	kaku = (int)(deg * 2) % 720;
	if( kaku < 0 )
		kaku += 720;

	return ( sinindex[ kaku ] );
}

float CCalc::DEGTan( float deg )
{
	int kaku;

	kaku = (int)((deg + 90.0) * 4) % 720;
	if( kaku < 0 )
		kaku += 720;
	return ( tanindex[ kaku ] );
}


float CCalc::Sqrt2( float x, float z )
{
	float x2, z2;
	float max2, min2;
	float max;
	//float maxdiv;
	int   index;

	x2 = x * x;
	z2 = z * z;

	if( x2 > z2 ){
		max = (float)fabs( x );
		max2 = x2;
		min2 = z2;
	}else{
		max = (float)fabs( z );
		max2 = z2;
		min2 = x2;
		//TRACE( "max %f :sqrt2 calc.cpp\n", max );
	}
	if( max2 ){
		index = (int)( 1000 * (min2 / max2) );
		return ( max * sqrtindex[ index ] );
	}
	return 0.0f;
}

float CCalc::Sqrt3( float x, float y, float z )
{
	float x2, z2;
	float max2, min2;
	float max;
	//float maxdiv;
	int   index;

	x2 = x * x;
	z2 = y * y + z * z;

	if( x2 > z2 ){
		max = (float)fabs( x );
		max2 = x2;
		min2 = z2;
	}else{
		max = Sqrt2( y, z );
		max2 = z2;
		min2 = x2;
	}
	if( max2 ){
		index = (int)( 1000 * min2 / max2 );
		return ( max * sqrtindex[ index ] );
	}
	return 0.0f;
}


float CCalc::Sqrt2( LOCATION& loc0, LOCATION& loc1 )
{
	float x2, z2;
	float max2, min2;
	float max;
	//float maxdiv;
	int   index;

	x2 = (loc0.x - loc1.x) * (loc0.x - loc1.x);
	z2 = (loc0.z - loc1.z) * (loc0.z - loc1.z);

	if( x2 > z2 ){
		max = (float)fabs( loc0.x - loc1.x );
		max2 = x2;
		min2 = z2;
	}else{
		max = (float)fabs( loc0.z - loc1.z );
		max2 = z2;
		min2 = x2;
	}
	if( max2 ){
		index = (int)( 1000 * min2 / max2 );
		return ( max * sqrtindex[ index ] );
	}
	return 0.0f;
}

float CCalc::Sqrt3( LOCATION& loc0, LOCATION& loc1 )
{
	float x2, z2;
	float max2, min2;
	float max;
	//float maxdiv;
	int   index;

	x2 = (loc0.x - loc1.x) * (loc0.x - loc1.x);
	z2 = (loc0.y - loc1.y) * (loc0.y - loc1.y) + (loc0.z - loc1.z) * (loc0.z - loc1.z);

	if( x2 > z2 ){
		max = (float)fabs( loc0.x - loc1.x );
		max2 = x2;
		min2 = z2;
	}else{
		max = Sqrt2( loc0.y - loc1.y, loc0.z - loc1.z );
		max2 = z2;
		min2 = x2;
	}
	if( max2 ){
		index = (int)( 1000 * min2 / max2 );
		return ( max * sqrtindex[ index ] );
	}
	return 0.0f;
}

float CCalc::DEGAtan( float z, float x )
{ // atan2( z, x );
	int		index;

	if( x ){
		index = (int)( z / x * 50.0 );
		if( x > 0 ){
			if( index > 0 ){
				if( index >= 1000 )
					index = 999;
				return( atanindexpp[ index ] );
			}else if( index < 0 ){
				if( index <= -1000 )
					index = -999;
				return( atanindexpm[ -index ] );
			}else{
				return( 0.0f );
			}
		}else{
			if( index > 0 ){
				if( index >= 1000 )
					index = 999;
				return( atanindexmm[ index ] );
			}else if( index < 0 ){
				if( index <= -1000 )
					index = -999;
				return( atanindexmp[ -index ] );
			}else{
				return( 180.0f );
			}
		}
	}else{ // x == 0
		if( z >= 0 ){
			return( 90.0f );
		}else{
			return ( -90.0f );
		}
	}
}

/***
LOCATION CCalc::RetRotYLoc( float rotydeg, LOCATION& loc )
{
	float		calccos, calcsin;
	LOCATION	retloc;

	calccos = DEGCos( rotydeg );
	calcsin = DEGSin( rotydeg );

	retloc.x = calccos * loc.x + calcsin * loc.z;
	retloc.y = loc.y;
	retloc.z = -calcsin * loc.x + calccos * loc.z;

	return retloc;
}

void CCalc::MakeRotXYZPoints( float rotdeg, LOCATION& dir, 
					 float* point, float* retpoint, int pointnum )
{ // dir は正規化されているものとする
  // pointnum は 10 まで ！！！！

	float	calcos, calsin, calcosM;
	int	pointno;
	float	savep[10][3];

	if( dir.x == 0 && dir.y == 0 && dir.z == 0 )
		rotdeg = 0.0f;
	//calcos = cos( kaku );
	//calsin = sin( kaku );
	calcos = DEGCos( rotdeg );
	calsin = DEGSin( rotdeg );
	calcosM = 1.0f - calcos;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		savep[pointno][0] = *( point + pointno * 3 );
		savep[pointno][1] = *( point + pointno * 3 + 1);
		savep[pointno][2] = *( point + pointno * 3 + 2);
	}

	for( pointno = 0; pointno < pointnum; pointno++ ){
		*(retpoint + pointno * 3) = (float)(
			(calcosM * dir.x * dir.x + calcos) * savep[pointno][0]
			+ (calcosM * dir.x * dir.y - calsin * dir.z ) * savep[pointno][1]
			+ (calcosM * dir.x * dir.z + calsin * dir.y ) * savep[pointno][2]
			);
		*(retpoint + pointno * 3 + 1 ) = (float)(
			(calcosM * dir.x * dir.y + calsin * dir.z ) * savep[pointno][0]
			+ (calcosM * dir.y * dir.y + calcos ) * savep[pointno][1]
			+ (calcosM * dir.y * dir.z - calsin * dir.x ) * savep[pointno][2]
			);
		*(retpoint + pointno * 3 + 2 ) = (float)(
			(calcosM * dir.x * dir.z - calsin * dir.y ) * savep[pointno][0]
			+ (calcosM * dir.y * dir.z + calsin * dir.x ) * savep[pointno][1]
			+ (calcosM * dir.z * dir.z + calcos ) * savep[pointno][2]
			);
	}
}

void CCalc::MakeRotXYZPoints( float rotdeg, LOCATION& dir, LOCATION& tra, 
					 float* point, float* retpoint, int pointnum )
{ // dir は正規化されているものとする
  // pointnum は 10 まで ！！！！

	float	calcos, calsin, calcosM;
	int	pointno;
	float	savep[10][3];

	if( dir.x == 0 && dir.y == 0 && dir.z == 0 )
		rotdeg = 0.0f;
	//calcos = cos( kaku );
	//calsin = sin( kaku );
	calcos = DEGCos( rotdeg );
	calsin = DEGSin( rotdeg );
	calcosM = 1.0f - calcos;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		savep[pointno][0] = *( point + pointno * 3 ) - tra.x;
		savep[pointno][1] = *( point + pointno * 3 + 1) - tra.y;
		savep[pointno][2] = *( point + pointno * 3 + 2) - tra.z;
	}

	for( pointno = 0; pointno < pointnum; pointno++ ){
		*(retpoint + pointno * 3) = (float)(
			(calcosM * dir.x * dir.x + calcos) * savep[pointno][0]
			+ (calcosM * dir.x * dir.y - calsin * dir.z ) * savep[pointno][1]
			+ (calcosM * dir.x * dir.z + calsin * dir.y ) * savep[pointno][2]
			+ tra.x );
		*(retpoint + pointno * 3 + 1 ) = (float)(
			(calcosM * dir.x * dir.y + calsin * dir.z ) * savep[pointno][0]
			+ (calcosM * dir.y * dir.y + calcos ) * savep[pointno][1]
			+ (calcosM * dir.y * dir.z - calsin * dir.x ) * savep[pointno][2]
			+ tra.y );
		*(retpoint + pointno * 3 + 2 ) = (float)(
			(calcosM * dir.x * dir.z - calsin * dir.y ) * savep[pointno][0]
			+ (calcosM * dir.y * dir.z + calsin * dir.x ) * savep[pointno][1]
			+ (calcosM * dir.z * dir.z + calcos ) * savep[pointno][2]
			+ tra.z );
	}
}

void  CCalc::MakeTraPoints( LOCATION& tra, float* point, 
		float* retpoint, int pointnum )
{
	int	pointno;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		*( retpoint + 3 * pointno ) = *( point + 3 * pointno ) + tra.x;
		*( retpoint + 3 * pointno + 1 ) = *( point + 3 * pointno + 1) + tra.y;
		*( retpoint + 3 * pointno + 2 ) = *( point + 3 * pointno + 2) + tra.z;
	}
}


int	CCalc::IsFaced( CHARPOS& pos0, D3DVECTOR& loc1, float	deg )
{
	LOCATION	vec01;
	LOCATION	dirvec;
	float		cmpcos;
	float		naiseki;

	cmpcos = DEGCos( deg );

	vec01.x = loc1.x - pos0.loc.x;
	vec01.y = 0.0f;
	vec01.z = loc1.z - pos0.loc.z;
	dirvec.x = pos0.dir.x;
	dirvec.y = 0.0f; // <--- ほんとは これが０でないから 面倒なことをする
	dirvec.z = pos0.dir.z;

	VecNormalizeXZ( &vec01 );
	VecNormalizeXZ( &dirvec );

	naiseki = vec01.x * dirvec.x + vec01.z * dirvec.z;

	if( naiseki >= cmpcos )
		return 1;
	else
		return 0;
}

int CCalc::IsClip( D3DTLVERTEX* testvert, D3DVECTOR* camloc, float deg, int pointnum )
{
	LOCATION	cam2test;
	float		seki, cmpseki;
	int			testcnt;
	int			isclip = 0;

	cmpseki = DEGCos( deg );

	for( testcnt = 0; testcnt < pointnum; testcnt++ ){
		cam2test.x = (testvert + testcnt)->sx - camloc->x;
		cam2test.y = (testvert + testcnt)->sy - camloc->y;
		cam2test.z = (testvert + testcnt)->sz - camloc->z;

		VecNormalizeXYZ( &cam2test );

		seki = cam2test.z;
		if( seki >= cmpseki ){ 
			isclip++;
		}
	}
	return isclip;
}


void CCalc::VecNormalizeXYZ( LOCATION* loc )
{
	float leng;
	float	lengdiv;

	leng = Sqrt3( loc->x, loc->y, loc->z );
	if( leng ){
		lengdiv = (float)(1.0 / leng);
		loc->x *= lengdiv;
		loc->y *= lengdiv;
		loc->z *= lengdiv;
	}

}

void CCalc::VecNormalizeXYZ( D3DVECTOR* vec )
{
	float leng;
	float lengdiv;
	leng = Sqrt3( vec->x, vec->y, vec->z );
	if( leng ){
		lengdiv = (float)(1.0 / leng);
		vec->x *= lengdiv;
		vec->y *= lengdiv;
		vec->z *= lengdiv;
	}
}

void CCalc::VecNormalizeXZ( LOCATION* loc )
{

	float leng;
	float lengdiv;

	leng = Sqrt2( loc->x, loc->z );
	if( leng ){
		lengdiv = (float)(1.0 / leng);
		loc->x *= lengdiv;
		loc->z *= lengdiv;
	}
	
}

//EDITPOINT!!!InitCharParam
void CCalc::InitCharParam( CHARPARAM* pparam )
{
		// CTRL CTLALL
	pparam->shiftXctlall = 0.0f;
	pparam->shiftYctlall = 0.0f;
	pparam->shiftZctlall = 0.0f;
	pparam->rotXctlall = 0.0f;
	pparam->rotYctlall = 0.0f;
	pparam->rotZctlall = 0.0f;
		// CTRL HEAD0
	pparam->shiftXhead0 = 0.0f;
	pparam->shiftYhead0 = 0.0f;
	pparam->shiftZhead0 = 0.0f;
	pparam->rotXhead0 = 0.0f;
	pparam->rotYhead0 = 0.0f;
	pparam->rotZhead0 = 0.0f;
		// CTRL HEAD1
	pparam->shiftXhead1 = 0.0f;
	pparam->shiftYhead1 = 0.0f;
	pparam->shiftZhead1 = 0.0f;
	pparam->rotXhead1 = 0.0f;
	pparam->rotYhead1 = 0.0f;
	pparam->rotZhead1 = 0.0f;
		// CTRL HEAD2
	pparam->shiftXhead2 = 0.0f;
	pparam->shiftYhead2 = 0.0f;
	pparam->shiftZhead2 = 0.0f;
	pparam->rotXhead2 = 0.0f;
	pparam->rotYhead2 = 0.0f;
	pparam->rotZhead2 = 0.0f;
		// CTRL HEAD00
	pparam->shiftXhead00 = 0.0f;
	pparam->shiftYhead00 = 0.0f;
	pparam->shiftZhead00 = 0.0f;
	pparam->rotXhead00 = 0.0f;
	pparam->rotYhead00 = 0.0f;
	pparam->rotZhead00 = 0.0f;
	pparam->scaleXhead00 = 1.0f;
	pparam->scaleYhead00 = 1.0f;
	pparam->scaleZhead00 = 1.0f;
		// CTRL HEAD01
	pparam->shiftXhead01 = 0.0f;
	pparam->shiftYhead01 = 0.0f;
	pparam->shiftZhead01 = 0.0f;
	pparam->rotXhead01 = 0.0f;
	pparam->rotYhead01 = 0.0f;
	pparam->rotZhead01 = 0.0f;
		// CTRL HEAD02
	pparam->shiftXhead02 = 0.0f;
	pparam->shiftYhead02 = 0.0f;
	pparam->shiftZhead02 = 0.0f;
	pparam->rotXhead02 = 0.0f;
	pparam->rotYhead02 = 0.0f;
	pparam->rotZhead02 = 0.0f;
		// CTRL HEAD000
	pparam->shiftXhead000 = 0.0f;
	pparam->shiftYhead000 = 0.0f;
	pparam->shiftZhead000 = 0.0f;
	pparam->rotXhead000 = 0.0f;
	pparam->rotYhead000 = 0.0f;
	pparam->rotZhead000 = 0.0f;
		// CTRL HEAD001
	pparam->shiftXhead001 = 0.0f;
	pparam->shiftYhead001 = 0.0f;
	pparam->shiftZhead001 = 0.0f;
	pparam->rotXhead001 = 0.0f;
	pparam->rotYhead001 = 0.0f;
	pparam->rotZhead001 = 0.0f;
		// CTRL HEAD002
	pparam->shiftXhead002 = 0.0f;
	pparam->shiftYhead002 = 0.0f;
	pparam->shiftZhead002 = 0.0f;
	pparam->rotXhead002 = 0.0f;
	pparam->rotYhead002 = 0.0f;
	pparam->rotZhead002 = 0.0f;
		// CTRL HEAD010
	pparam->shiftXhead010 = 0.0f;
	pparam->shiftYhead010 = 0.0f;
	pparam->shiftZhead010 = 0.0f;
	pparam->rotXhead010 = 0.0f;
	pparam->rotYhead010 = 0.0f;
	pparam->rotZhead010 = 0.0f;
		// CTRL HEAD011
	pparam->shiftXhead011 = 0.0f;
	pparam->shiftYhead011 = 0.0f;
	pparam->shiftZhead011 = 0.0f;
	pparam->rotXhead011 = 0.0f;
	pparam->rotYhead011 = 0.0f;
	pparam->rotZhead011 = 0.0f;
		// CTRL HEAD012
	pparam->shiftXhead012 = 0.0f;
	pparam->shiftYhead012 = 0.0f;
	pparam->shiftZhead012 = 0.0f;
	pparam->rotXhead012 = 0.0f;
	pparam->rotYhead012 = 0.0f;
	pparam->rotZhead012 = 0.0f;
		// CTRL ARML0
	pparam->shiftXarml0 = 0.0f;
	pparam->shiftYarml0 = 0.0f;
	pparam->shiftZarml0 = 0.0f;
	pparam->rotXarml0 = 0.0f;
	pparam->rotYarml0 = 0.0f;
	pparam->rotZarml0 = 0.0f;
		// CTRL ARML1
	pparam->shiftXarml1 = 0.0f;
	pparam->shiftYarml1 = 0.0f;
	pparam->shiftZarml1 = 0.0f;
	pparam->rotXarml1 = 0.0f;
	pparam->rotYarml1 = 0.0f;
	pparam->rotZarml1 = 0.0f;
		// CTRL ARML2
	pparam->shiftXarml2 = 0.0f;
	pparam->shiftYarml2 = 0.0f;
	pparam->shiftZarml2 = 0.0f;
	pparam->rotXarml2 = 0.0f;
	pparam->rotYarml2 = 0.0f;
	pparam->rotZarml2 = 0.0f;
		// CTRL ARMR0
	pparam->shiftXarmr0 = 0.0f;
	pparam->shiftYarmr0 = 0.0f;
	pparam->shiftZarmr0 = 0.0f;
	pparam->rotXarmr0 = 0.0f;
	pparam->rotYarmr0 = 0.0f;
	pparam->rotZarmr0 = 0.0f;
		// CTRL ARMR1
	pparam->shiftXarmr1 = 0.0f;
	pparam->shiftYarmr1 = 0.0f;
	pparam->shiftZarmr1 = 0.0f;
	pparam->rotXarmr1 = 0.0f;
	pparam->rotYarmr1 = 0.0f;
	pparam->rotZarmr1 = 0.0f;
		// CTRL ARMR2
	pparam->shiftXarmr2 = 0.0f;
	pparam->shiftYarmr2 = 0.0f;
	pparam->shiftZarmr2 = 0.0f;
	pparam->rotXarmr2 = 0.0f;
	pparam->rotYarmr2 = 0.0f;
	pparam->rotZarmr2 = 0.0f;
		// CTRL LEGL0
	pparam->shiftXlegl0 = 0.0f;
	pparam->shiftYlegl0 = 0.0f;
	pparam->shiftZlegl0 = 0.0f;
	pparam->rotXlegl0 = 0.0f;
	pparam->rotYlegl0 = 0.0f;
	pparam->rotZlegl0 = 0.0f;
		// CTRL LEGL1
	pparam->shiftXlegl1 = 0.0f;
	pparam->shiftYlegl1 = 0.0f;
	pparam->shiftZlegl1 = 0.0f;
	pparam->rotXlegl1 = 0.0f;
	pparam->rotYlegl1 = 0.0f;
	pparam->rotZlegl1 = 0.0f;
		// CTRL LEGL2
	pparam->shiftXlegl2 = 0.0f;
	pparam->shiftYlegl2 = 0.0f;
	pparam->shiftZlegl2 = 0.0f;
	pparam->rotXlegl2 = 0.0f;
	pparam->rotYlegl2 = 0.0f;
	pparam->rotZlegl2 = 0.0f;
		// CTRL LEGR0
	pparam->shiftXlegr0 = 0.0f;
	pparam->shiftYlegr0 = 0.0f;
	pparam->shiftZlegr0 = 0.0f;
	pparam->rotXlegr0 = 0.0f;
	pparam->rotYlegr0 = 0.0f;
	pparam->rotZlegr0 = 0.0f;
		// CTRL LEGR1
	pparam->shiftXlegr1 = 0.0f;
	pparam->shiftYlegr1 = 0.0f;
	pparam->shiftZlegr1 = 0.0f;
	pparam->rotXlegr1 = 0.0f;
	pparam->rotYlegr1 = 0.0f;
	pparam->rotZlegr1 = 0.0f;
		// CTRL LEGR2
	pparam->shiftXlegr2 = 0.0f;
	pparam->shiftYlegr2 = 0.0f;
	pparam->shiftZlegr2 = 0.0f;
	pparam->rotXlegr2 = 0.0f;
	pparam->rotYlegr2 = 0.0f;
	pparam->rotZlegr2 = 0.0f;
		// CTRL PIS
	pparam->scaleXpis = 1.0f;
	pparam->scaleYpis = 1.0f;
	pparam->scaleZpis = 1.0f;
	pparam->actionpis = FALSE;
		// CTRL FIRE
	pparam->scaleXfire = 1.0f;
	pparam->scaleYfire = 1.0f;
	pparam->scaleZfire = 1.0f;
	pparam->rotXfire = 0.0f;
	pparam->rotYfire = 0.0f;
	pparam->rotZfire = 0.0f;
	pparam->actionfire = FALSE;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamCTLALL( CHARPARAM* pparam )
{
	pparam->shiftXctlall = 0.0f;
	pparam->shiftYctlall = 0.0f;
	pparam->shiftZctlall = 0.0f;
	pparam->rotXctlall = 0.0f;
	pparam->rotYctlall = 0.0f;
	pparam->rotZctlall = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamHEAD0( CHARPARAM* pparam )
{
	pparam->shiftXhead0 = 0.0f;
	pparam->shiftYhead0 = 0.0f;
	pparam->shiftZhead0 = 0.0f;
	pparam->rotXhead0 = 0.0f;
	pparam->rotYhead0 = 0.0f;
	pparam->rotZhead0 = 0.0f;
	pparam->shiftXhead1 = 0.0f;
	pparam->shiftYhead1 = 0.0f;
	pparam->shiftZhead1 = 0.0f;
	pparam->rotXhead1 = 0.0f;
	pparam->rotYhead1 = 0.0f;
	pparam->rotZhead1 = 0.0f;
	pparam->shiftXhead2 = 0.0f;
	pparam->shiftYhead2 = 0.0f;
	pparam->shiftZhead2 = 0.0f;
	pparam->rotXhead2 = 0.0f;
	pparam->rotYhead2 = 0.0f;
	pparam->rotZhead2 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamHEAD00( CHARPARAM* pparam )
{
	pparam->shiftXhead00 = 0.0f;
	pparam->shiftYhead00 = 0.0f;
	pparam->shiftZhead00 = 0.0f;
	pparam->rotXhead00 = 0.0f;
	pparam->rotYhead00 = 0.0f;
	pparam->rotZhead00 = 0.0f;
	pparam->scaleXhead00 = 1.0f;
	pparam->scaleYhead00 = 1.0f;
	pparam->scaleZhead00 = 1.0f;
	pparam->shiftXhead01 = 0.0f;
	pparam->shiftYhead01 = 0.0f;
	pparam->shiftZhead01 = 0.0f;
	pparam->rotXhead01 = 0.0f;
	pparam->rotYhead01 = 0.0f;
	pparam->rotZhead01 = 0.0f;
	pparam->shiftXhead02 = 0.0f;
	pparam->shiftYhead02 = 0.0f;
	pparam->shiftZhead02 = 0.0f;
	pparam->rotXhead02 = 0.0f;
	pparam->rotYhead02 = 0.0f;
	pparam->rotZhead02 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamHEAD000( CHARPARAM* pparam )
{
	pparam->shiftXhead000 = 0.0f;
	pparam->shiftYhead000 = 0.0f;
	pparam->shiftZhead000 = 0.0f;
	pparam->rotXhead000 = 0.0f;
	pparam->rotYhead000 = 0.0f;
	pparam->rotZhead000 = 0.0f;
	pparam->shiftXhead001 = 0.0f;
	pparam->shiftYhead001 = 0.0f;
	pparam->shiftZhead001 = 0.0f;
	pparam->rotXhead001 = 0.0f;
	pparam->rotYhead001 = 0.0f;
	pparam->rotZhead001 = 0.0f;
	pparam->shiftXhead002 = 0.0f;
	pparam->shiftYhead002 = 0.0f;
	pparam->shiftZhead002 = 0.0f;
	pparam->rotXhead002 = 0.0f;
	pparam->rotYhead002 = 0.0f;
	pparam->rotZhead002 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamHEAD010( CHARPARAM* pparam )
{
	pparam->shiftXhead010 = 0.0f;
	pparam->shiftYhead010 = 0.0f;
	pparam->shiftZhead010 = 0.0f;
	pparam->rotXhead010 = 0.0f;
	pparam->rotYhead010 = 0.0f;
	pparam->rotZhead010 = 0.0f;
	pparam->shiftXhead011 = 0.0f;
	pparam->shiftYhead011 = 0.0f;
	pparam->shiftZhead011 = 0.0f;
	pparam->rotXhead011 = 0.0f;
	pparam->rotYhead011 = 0.0f;
	pparam->rotZhead011 = 0.0f;
	pparam->shiftXhead012 = 0.0f;
	pparam->shiftYhead012 = 0.0f;
	pparam->shiftZhead012 = 0.0f;
	pparam->rotXhead012 = 0.0f;
	pparam->rotYhead012 = 0.0f;
	pparam->rotZhead012 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamARML0( CHARPARAM* pparam )
{
	pparam->shiftXarml0 = 0.0f;
	pparam->shiftYarml0 = 0.0f;
	pparam->shiftZarml0 = 0.0f;
	pparam->rotXarml0 = 0.0f;
	pparam->rotYarml0 = 0.0f;
	pparam->rotZarml0 = 0.0f;
	pparam->shiftXarml1 = 0.0f;
	pparam->shiftYarml1 = 0.0f;
	pparam->shiftZarml1 = 0.0f;
	pparam->rotXarml1 = 0.0f;
	pparam->rotYarml1 = 0.0f;
	pparam->rotZarml1 = 0.0f;
	pparam->shiftXarml2 = 0.0f;
	pparam->shiftYarml2 = 0.0f;
	pparam->shiftZarml2 = 0.0f;
	pparam->rotXarml2 = 0.0f;
	pparam->rotYarml2 = 0.0f;
	pparam->rotZarml2 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamARMR0( CHARPARAM* pparam )
{
	pparam->shiftXarmr0 = 0.0f;
	pparam->shiftYarmr0 = 0.0f;
	pparam->shiftZarmr0 = 0.0f;
	pparam->rotXarmr0 = 0.0f;
	pparam->rotYarmr0 = 0.0f;
	pparam->rotZarmr0 = 0.0f;
	pparam->shiftXarmr1 = 0.0f;
	pparam->shiftYarmr1 = 0.0f;
	pparam->shiftZarmr1 = 0.0f;
	pparam->rotXarmr1 = 0.0f;
	pparam->rotYarmr1 = 0.0f;
	pparam->rotZarmr1 = 0.0f;
	pparam->shiftXarmr2 = 0.0f;
	pparam->shiftYarmr2 = 0.0f;
	pparam->shiftZarmr2 = 0.0f;
	pparam->rotXarmr2 = 0.0f;
	pparam->rotYarmr2 = 0.0f;
	pparam->rotZarmr2 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamLEGL0( CHARPARAM* pparam )
{
	pparam->shiftXlegl0 = 0.0f;
	pparam->shiftYlegl0 = 0.0f;
	pparam->shiftZlegl0 = 0.0f;
	pparam->rotXlegl0 = 0.0f;
	pparam->rotYlegl0 = 0.0f;
	pparam->rotZlegl0 = 0.0f;
	pparam->shiftXlegl1 = 0.0f;
	pparam->shiftYlegl1 = 0.0f;
	pparam->shiftZlegl1 = 0.0f;
	pparam->rotXlegl1 = 0.0f;
	pparam->rotYlegl1 = 0.0f;
	pparam->rotZlegl1 = 0.0f;
	pparam->shiftXlegl2 = 0.0f;
	pparam->shiftYlegl2 = 0.0f;
	pparam->shiftZlegl2 = 0.0f;
	pparam->rotXlegl2 = 0.0f;
	pparam->rotYlegl2 = 0.0f;
	pparam->rotZlegl2 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamLEGR0( CHARPARAM* pparam )
{
	pparam->shiftXlegr0 = 0.0f;
	pparam->shiftYlegr0 = 0.0f;
	pparam->shiftZlegr0 = 0.0f;
	pparam->rotXlegr0 = 0.0f;
	pparam->rotYlegr0 = 0.0f;
	pparam->rotZlegr0 = 0.0f;
	pparam->shiftXlegr1 = 0.0f;
	pparam->shiftYlegr1 = 0.0f;
	pparam->shiftZlegr1 = 0.0f;
	pparam->rotXlegr1 = 0.0f;
	pparam->rotYlegr1 = 0.0f;
	pparam->rotZlegr1 = 0.0f;
	pparam->shiftXlegr2 = 0.0f;
	pparam->shiftYlegr2 = 0.0f;
	pparam->shiftZlegr2 = 0.0f;
	pparam->rotXlegr2 = 0.0f;
	pparam->rotYlegr2 = 0.0f;
	pparam->rotZlegr2 = 0.0f;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます

void CCalc::InitParamPIS( CHARPARAM* pparam )
{
	pparam->scaleXpis = 1.0f;
	pparam->scaleYpis = 1.0f;
	pparam->scaleZpis = 1.0f;
	pparam->actionpis = FALSE;
	pparam->scaleXfire = 1.0f;
	pparam->scaleYfire = 1.0f;
	pparam->scaleZfire = 1.0f;
	pparam->rotXfire = 0.0f;
	pparam->rotYfire = 0.0f;
	pparam->rotZfire = 0.0f;
	pparam->actionfire = FALSE;
}
	//EDITブランケット内につき手動でのコードはバッチで消去されます
//ENDEDIT


void CCalc::MakeRotPoint3( D3DVERTEX* retvert, float rotx, float roty, float rotz, int pointnum )
{
	// pointnum 100 まで！！！！！
	// チェック 省略
	int			pointno;
	D3DVERTEX	savevert[100];
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		savevert[pointno].x = (retvert + pointno)->x;
		savevert[pointno].y = (retvert + pointno)->y;
		savevert[pointno].z = (retvert + pointno)->z;
	}
	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	for( pointno = 0; pointno < pointnum; pointno++ ){
		(retvert + pointno)->x = 
			ccosz * ccosy * savevert[pointno].x
			- ccosz * csiny * savevert[pointno].y
			+ csinz * savevert[pointno].z;
		(retvert + pointno)->y =
			(csinx * csinz * ccosy + ccosx * csiny ) * savevert[pointno].x
			+ ( -csinx * csinz + ccosx * ccosy ) * savevert[pointno].y
			- csinx * ccosz * savevert[pointno].z;
		(retvert + pointno)->z = 
			( -ccosx * csinz * ccosy + csinx * csiny ) * savevert[pointno].x
			+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert[pointno].y
			+ ccosx * ccosz * savevert[pointno].z;
	}
}

void CCalc::MakeRotPoint3( D3DVERTEX* retvert, float rotx, float roty, float rotz, 
							int planenum, int pointnum )
{
	// planenum 200 まで 
	// pointnum 100 まで！！！！！
	// チェック 省略
	int			planeno, pointno;
	D3DVERTEX	savevert[200][100];
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			savevert[planeno][pointno].x = (retvert + planeno * pointnum + pointno)->x;
			savevert[planeno][pointno].y = (retvert + planeno * pointnum + pointno)->y;
			savevert[planeno][pointno].z = (retvert + planeno * pointnum + pointno)->z;
		}
	}
	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			(retvert + planeno * pointnum + pointno)->x = 
				ccosz * ccosy * savevert[planeno][pointno].x
				- ccosz * csiny * savevert[planeno][pointno].y
				+ csinz * savevert[planeno][pointno].z;
			(retvert + planeno * pointnum + pointno)->y =
				(csinx * csinz * ccosy + ccosx * csiny ) * savevert[planeno][pointno].x
				+ ( -csinx * csinz + ccosx * ccosy ) * savevert[planeno][pointno].y
				- csinx * ccosz * savevert[planeno][pointno].z;
			(retvert + planeno * pointnum + pointno)->z = 
				( -ccosx * csinz * ccosy + csinx * csiny ) * savevert[planeno][pointno].x
				+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert[planeno][pointno].y
				+ ccosx * ccosz * savevert[planeno][pointno].z;
		}
	}
}
***/
/***
void CCalc::MakeRotPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz, 
						int planenum, int pointnum )
{
	// planenum 200 まで 
	// pointnum 100 まで！！！！！
	// チェック 省略
	int			planeno, pointno;
	D3DVERTEX	savevert[200][100];
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			savevert[planeno][pointno].x = (retvert + planeno * pointnum + pointno)->x - centervert.x;
			savevert[planeno][pointno].y = (retvert + planeno * pointnum + pointno)->y - centervert.y;
			savevert[planeno][pointno].z = (retvert + planeno * pointnum + pointno)->z - centervert.z;
		}
	}
	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			(retvert + planeno * pointnum + pointno)->x = 
				ccosz * ccosy * savevert[planeno][pointno].x
				- ccosz * csiny * savevert[planeno][pointno].y
				+ csinz * savevert[planeno][pointno].z
				+ centervert.x;
			(retvert + planeno * pointnum + pointno)->y =
				(csinx * csinz * ccosy + ccosx * csiny ) * savevert[planeno][pointno].x
				+ ( -csinx * csinz + ccosx * ccosy ) * savevert[planeno][pointno].y
				- csinx * ccosz * savevert[planeno][pointno].z
				+ centervert.y;
			(retvert + planeno * pointnum + pointno)->z = 
				( -ccosx * csinz * ccosy + csinx * csiny ) * savevert[planeno][pointno].x
				+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert[planeno][pointno].y
				+ ccosx * ccosz * savevert[planeno][pointno].z
				+ centervert.z;
		}
	}
}

void CCalc::MakeRotPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz )
{
	// チェック 省略
	D3DVERTEX	savevert;
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	savevert.x = retvert->x - centervert.x;
	savevert.y = retvert->y - centervert.y;
	savevert.z = retvert->z - centervert.z;

	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	retvert->x = 
		ccosz * ccosy * savevert.x
		- ccosz * csiny * savevert.y
		+ csinz * savevert.z
		+ centervert.x;
	retvert->y =
		(csinx * csinz * ccosy + ccosx * csiny ) * savevert.x
		+ ( -csinx * csinz + ccosx * ccosy ) * savevert.y
		- csinx * ccosz * savevert.z
		+ centervert.y;
	retvert->z = 
		( -ccosx * csinz * ccosy + csinx * csiny ) * savevert.x
		+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert.y
		+ ccosx * ccosz * savevert.z
		+ centervert.z;
}

void CCalc::MakeRotPoint3f( D3DVERTEX* retvert, D3DVERTEX* firstvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz, 
						int planenum, int pointnum )
{
	// planenum 200 まで 
	// pointnum 100 まで！！！！！
	// チェック 省略
	int			planeno, pointno;
	D3DVERTEX	savevert[200][100];
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			savevert[planeno][pointno].x = (firstvert + planeno * pointnum + pointno)->x - centervert.x;
			savevert[planeno][pointno].y = (firstvert + planeno * pointnum + pointno)->y - centervert.y;
			savevert[planeno][pointno].z = (firstvert + planeno * pointnum + pointno)->z - centervert.z;
		}
	}
	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			(retvert + planeno * pointnum + pointno)->x = 
				ccosz * ccosy * savevert[planeno][pointno].x
				- ccosz * csiny * savevert[planeno][pointno].y
				+ csinz * savevert[planeno][pointno].z
				+ centervert.x;
			(retvert + planeno * pointnum + pointno)->y =
				(csinx * csinz * ccosy + ccosx * csiny ) * savevert[planeno][pointno].x
				+ ( -csinx * csinz + ccosx * ccosy ) * savevert[planeno][pointno].y
				- csinx * ccosz * savevert[planeno][pointno].z
				+ centervert.y;
			(retvert + planeno * pointnum + pointno)->z = 
				( -ccosx * csinz * ccosy + csinx * csiny ) * savevert[planeno][pointno].x
				+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert[planeno][pointno].y
				+ ccosx * ccosz * savevert[planeno][pointno].z
				+ centervert.z;
		}
	}
}

void CCalc::MakeRotPoint3f( D3DVERTEX* retvert, D3DVERTEX* fistvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz )
{
	// チェック 省略
	D3DVERTEX	savevert;
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	savevert.x = fistvert->x - centervert.x;
	savevert.y = fistvert->y - centervert.y;
	savevert.z = fistvert->z - centervert.z;

	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	retvert->x = 
		ccosz * ccosy * savevert.x
		- ccosz * csiny * savevert.y
		+ csinz * savevert.z
		+ centervert.x;
	retvert->y =
		(csinx * csinz * ccosy + ccosx * csiny ) * savevert.x
		+ ( -csinx * csinz + ccosx * ccosy ) * savevert.y
		- csinx * ccosz * savevert.z
		+ centervert.y;
	retvert->z = 
		( -ccosx * csinz * ccosy + csinx * csiny ) * savevert.x
		+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert.y
		+ ccosx * ccosz * savevert.z
		+ centervert.z;
}

// rot + tra
void CCalc::MakeRotTraPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz, 
						float trax, float tray, float traz,
						int planenum, int pointnum )
{
	// planenum 200 まで 
	// pointnum 100 まで！！！！！
	// チェック 省略
	int			planeno, pointno;
	D3DVERTEX	savevert[200][100];
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			savevert[planeno][pointno].x = (retvert + planeno * pointnum + pointno)->x - centervert.x;
			savevert[planeno][pointno].y = (retvert + planeno * pointnum + pointno)->y - centervert.y;
			savevert[planeno][pointno].z = (retvert + planeno * pointnum + pointno)->z - centervert.z;
		}
	}
	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			(retvert + planeno * pointnum + pointno)->x = 
				ccosz * ccosy * savevert[planeno][pointno].x
				- ccosz * csiny * savevert[planeno][pointno].y
				+ csinz * savevert[planeno][pointno].z
				+ centervert.x + trax;
			(retvert + planeno * pointnum + pointno)->y =
				(csinx * csinz * ccosy + ccosx * csiny ) * savevert[planeno][pointno].x
				+ ( -csinx * csinz + ccosx * ccosy ) * savevert[planeno][pointno].y
				- csinx * ccosz * savevert[planeno][pointno].z
				+ centervert.y + tray;
			(retvert + planeno * pointnum + pointno)->z = 
				( -ccosx * csinz * ccosy + csinx * csiny ) * savevert[planeno][pointno].x
				+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert[planeno][pointno].y
				+ ccosx * ccosz * savevert[planeno][pointno].z
				+ centervert.z + traz;
		}
	}
}

void CCalc::MakeRotTraPoint3( D3DVERTEX* retvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz, 
						float trax, float tray, float traz
						)
{
	// チェック 省略
	D3DVERTEX	savevert;
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	savevert.x = retvert->x - centervert.x;
	savevert.y = retvert->y - centervert.y;
	savevert.z = retvert->z - centervert.z;

	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	retvert->x = 
		ccosz * ccosy * savevert.x
		- ccosz * csiny * savevert.y
		+ csinz * savevert.z
		+ centervert.x + trax;
	retvert->y =
		(csinx * csinz * ccosy + ccosx * csiny ) * savevert.x
		+ ( -csinx * csinz + ccosx * ccosy ) * savevert.y
		- csinx * ccosz * savevert.z
		+ centervert.y + tray;
	retvert->z = 
		( -ccosx * csinz * ccosy + csinx * csiny ) * savevert.x
		+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert.y
		+ ccosx * ccosz * savevert.z
		+ centervert.z + traz;
}

void CCalc::MakeRotTraPoint3f( D3DVERTEX* retvert, D3DVERTEX* firstvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz, 
						float trax, float tray, float traz,
						int planenum, int pointnum )
{
	// planenum 200 まで 
	// pointnum 100 まで！！！！！
	// チェック 省略
	int			planeno, pointno;
	D3DVERTEX	savevert[200][100];
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			savevert[planeno][pointno].x = (firstvert + planeno * pointnum + pointno)->x - centervert.x;
			savevert[planeno][pointno].y = (firstvert + planeno * pointnum + pointno)->y - centervert.y;
			savevert[planeno][pointno].z = (firstvert + planeno * pointnum + pointno)->z - centervert.z;
		}
	}
	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	for( planeno = 0; planeno < planenum; planeno++ ){
		for( pointno = 0; pointno < pointnum; pointno++ ){
			(retvert + planeno * pointnum + pointno)->x = 
				ccosz * ccosy * savevert[planeno][pointno].x
				- ccosz * csiny * savevert[planeno][pointno].y
				+ csinz * savevert[planeno][pointno].z
				+ centervert.x + trax;
			(retvert + planeno * pointnum + pointno)->y =
				(csinx * csinz * ccosy + ccosx * csiny ) * savevert[planeno][pointno].x
				+ ( -csinx * csinz + ccosx * ccosy ) * savevert[planeno][pointno].y
				- csinx * ccosz * savevert[planeno][pointno].z
				+ centervert.y + tray;
			(retvert + planeno * pointnum + pointno)->z = 
				( -ccosx * csinz * ccosy + csinx * csiny ) * savevert[planeno][pointno].x
				+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert[planeno][pointno].y
				+ ccosx * ccosz * savevert[planeno][pointno].z
				+ centervert.z + traz;
		}
	}
}

void CCalc::MakeRotTraPoint3f( D3DVERTEX* retvert, D3DVERTEX* fistvert, D3DVERTEX& centervert, 
						float rotx, float roty, float rotz,
						float trax, float tray, float traz )
{
	// チェック 省略
	D3DVERTEX	savevert;
	float		csinx, csiny, csinz, ccosx, ccosy, ccosz;

	savevert.x = fistvert->x - centervert.x;
	savevert.y = fistvert->y - centervert.y;
	savevert.z = fistvert->z - centervert.z;

	// あちゃー y と z を 間違えてーら 応急処置
	csinx = DEGSin( rotx ); csinz = DEGSin( roty ); csiny = DEGSin( rotz );
	ccosx = DEGCos( rotx ); ccosz = DEGCos( roty ); ccosy = DEGCos( rotz );

	retvert->x = 
		ccosz * ccosy * savevert.x
		- ccosz * csiny * savevert.y
		+ csinz * savevert.z
		+ centervert.x + trax;
	retvert->y =
		(csinx * csinz * ccosy + ccosx * csiny ) * savevert.x
		+ ( -csinx * csinz + ccosx * ccosy ) * savevert.y
		- csinx * ccosz * savevert.z
		+ centervert.y + tray;
	retvert->z = 
		( -ccosx * csinz * ccosy + csinx * csiny ) * savevert.x
		+ ( ccosx * csinz * csiny + csinx * ccosy ) * savevert.y
		+ ccosx * ccosz * savevert.z
		+ centervert.z + traz;
}

void CCalc::DEGPlus( float* retdeg, float plusdeg )
{
	*retdeg += plusdeg;
	if( *retdeg >= 360.0 )
		*retdeg -= 360.0f;
	else if( *retdeg < 0.0 )
		*retdeg += 360.0f;

}
***/
/***
void CCalc::CenterLoc( LOCATION* retloc, D3DVERTEX* svert, int pointnum )
{
	int	pointno;
	float	div;

	retloc->x = 0.0f; retloc->y = 0.0f; retloc->z = 0.0f;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		retloc->x += (svert + pointno)->x;
		retloc->y += (svert + pointno)->y;
		retloc->z += (svert + pointno)->z;
	}
	div = (float)(1.0 / (double)pointnum);

	retloc->x *= div;
	retloc->y *= div;
	retloc->z *= div;
}

void CCalc::CenterLoc( LOCATION* retloc, LOCATION* sloc, int pointnum )
{
	int	pointno;
	float	div;

	retloc->x = 0.0f; retloc->y = 0.0f; retloc->z = 0.0f;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		retloc->x += (sloc + pointno)->x;
		retloc->y += (sloc + pointno)->y;
		retloc->z += (sloc + pointno)->z;
	}
	div = (float)(1.0 / (double)pointnum);

	retloc->x *= div;
	retloc->y *= div;
	retloc->z *= div;
}

void CCalc::CenterLoc( LOCATION* retloc, D3DTLVERTEX* svert, int pointnum )
{
	int	pointno;
	float	div;

	retloc->x = 0.0f; retloc->y = 0.0f; retloc->z = 0.0f;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		retloc->x += (svert + pointno)->sx;
		retloc->y += (svert + pointno)->sy;
		retloc->z += (svert + pointno)->sz;
	}

	div = (float)(1.0 / (double)pointnum);

	retloc->x *= div;
	retloc->y *= div;
	retloc->z *= div;
}

void CCalc::AvaLoc( LOCATION* retloc, LOCATION* sloc1, LOCATION* sloc2 )
{
	retloc->x = (sloc1->x + sloc2->x) * 0.5f;
	retloc->y = (sloc1->y + sloc2->y) * 0.5f;
	retloc->z = (sloc1->z + sloc2->z) * 0.5f;
}


void CCalc::CenterVertex( D3DVERTEX* retloc, D3DVERTEX* svert, int pointnum )
{
	int	pointno;
	float	div;

	retloc->x = 0.0f; retloc->y = 0.0f; retloc->z = 0.0f;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		retloc->x += (svert + pointno)->x;
		retloc->y += (svert + pointno)->y;
		retloc->z += (svert + pointno)->z;
	}
	div = (float)(1.0 / (double)pointnum);

	retloc->x *= div;
	retloc->y *= div;
	retloc->z *= div;
}
void CCalc::CenterVertex( D3DTLVERTEX* retloc, D3DTLVERTEX* svert, int pointnum )
{
	int	pointno;
	float	div;

	retloc->sx = 0.0f; retloc->sy = 0.0f; retloc->sz = 0.0f;

	for( pointno = 0; pointno < pointnum; pointno++ ){
		retloc->sx += (svert + pointno)->sx;
		retloc->sy += (svert + pointno)->sy;
		retloc->sz += (svert + pointno)->sz;
	}
	div = (float)(1.0 / (double)pointnum);

	retloc->sx *= div;
	retloc->sy *= div;
	retloc->sz *= div;
}

void CCalc::InitRGB500()
{
	int	data;

	for( data = 0; data <= 500; data++ ){
		if( data <= 255 ){
			rgb500[data] = data;
		}else{
			rgb500[data] = 255;
		}
	}
}

void CCalc::ClampRGB( RGBDATA* rgbcol )
{
	rgbcol->r = rgb500[ rgbcol->r ];
	rgbcol->g = rgb500[ rgbcol->g ];
	rgbcol->b = rgb500[ rgbcol->b ];

}


/////////////////
void	CCalc::SetVec( LOCATION* retvec, LOCATION& vec1, LOCATION& vec0 )
{
	retvec->x = vec1.x - vec0.x;
	retvec->y = vec1.y - vec0.y;
	retvec->z = vec1.z - vec0.z;
}
void	CCalc::SetVec( LOCATION* retvec, D3DTLVERTEX& vec1, D3DTLVERTEX& vec0 )
{
	retvec->x = vec1.sx - vec0.sx;
	retvec->y = vec1.sy - vec0.sy;
	retvec->z = vec1.sz - vec0.sz;
}

void	CCalc::Gaiseki( LOCATION* retlocdat, LOCATION& locdat1, LOCATION& locdat2 )
{
	double	maglocdat;
	double	vecsize;
	float	invsize;

	retlocdat->x = locdat1.y * locdat2.z - locdat1.z * locdat2.y;
	retlocdat->y = locdat1.z * locdat2.x - locdat1.x * locdat2.z;
	retlocdat->z = locdat1.x * locdat2.y - locdat1.y * locdat2.x;

//normalize
	maglocdat = retlocdat->x * retlocdat->x
		+ retlocdat->y * retlocdat->y
		+ retlocdat->z * retlocdat->z;

	if( maglocdat ){
		vecsize = sqrt( maglocdat );

		if( vecsize ){
			invsize = (float)(1.0 / vecsize);
			retlocdat->x *= invsize;
			retlocdat->y *= invsize;
			retlocdat->z *= invsize;
		}else{
			TRACE( "Gaiseki : vecsize Zeor !!!\n" );

			retlocdat->x = 0.0f;
			retlocdat->y = 0.0f;
			retlocdat->z = 0.0f;
		}
	}else{
		
		TRACE( "Gaiseki : maglocdat Zeor !!!\n" );

		retlocdat->x = 0.0f;
		retlocdat->y = 0.0f;
		retlocdat->z = 0.0f;
	}

}
float	CCalc::Naiseki( LOCATION& locdat1, LOCATION& locdat2 )
{
	float	seki;

	seki = locdat1.x * locdat2.x 
		+ locdat1.y * locdat2.y 
		+ locdat1.z * locdat2.z;

	return seki;
}
***/