#ifndef LIGHTDATAH
#define LIGHTDATAH

#include <D3DX9.h>

#include <coef.h>
#include <basedat.h>

#include <math.h>

#include <crtdbg.h>

#define	DBGH
#include "dbg.h"


class CLightData
{
public:
	CLightData( int* dstlid );
	~CLightData();

public:
	int SetDirectionalLight( D3DXVECTOR3 dir, CVec3f difcol );
	int SetPointLight( D3DXVECTOR3 pos, float dist, CVec3f difcol );
	int SetSpotLight( D3DXVECTOR3 pos, D3DXVECTOR3 dir, float dist, float deg, CVec3f difcol );

	int TransformDirection( D3DXMATRIX matView, D3DXMATRIX matProj );


	__inline int CalcLightRDB( D3DTLVERTEX* orgtlv, D3DXVECTOR3* worldv, D3DXMATRIX* matWorld, D3DXVECTOR3* curn, float diffr, float diffg, float diffb, float* addr, float* addg, float* addb, float* addspecr, float* addspecg, float* addspecb )
	{
		float rate, rate1, rate2, rate3;
		//float dot2, dot3;
		float dot3 = 0.0f;


		//float tx = orgtlv->sx;
		//float ty = orgtlv->sy;
		//float tz = orgtlv->sz;
		D3DXVECTOR3 tlv = *worldv;


		switch( Type ){
		case D3DLIGHT_DIRECTIONAL:
			rate3 = CalcRate3( curn, &dot3 );
			rate = rate3;

			break;
		case D3DLIGHT_POINT:
			//tlv.x = matWorld->_11*tx + matWorld->_21*ty + matWorld->_31*tz + matWorld->_41;
			//tlv.y = matWorld->_12*tx + matWorld->_22*ty + matWorld->_32*tz + matWorld->_42;
			//tlv.z = matWorld->_13*tx + matWorld->_23*ty + matWorld->_33*tz + matWorld->_43;
			
			rate1 = CalcRate1( &tlv );
			rate3 = CalcRate3( curn, &dot3 );
			rate = rate1 * rate3;

			break;
		case D3DLIGHT_SPOT:
			//tlv.x = matWorld->_11*tx + matWorld->_21*ty + matWorld->_31*tz + matWorld->_41;
			//tlv.y = matWorld->_12*tx + matWorld->_22*ty + matWorld->_32*tz + matWorld->_42;
			//tlv.z = matWorld->_13*tx + matWorld->_23*ty + matWorld->_33*tz + matWorld->_43;

			rate1 = CalcRate1( &tlv );
			rate2 = CalcRate2( &tlv );
			rate3 = CalcRate3( curn, &dot3 );
			rate = rate1 * rate2 * rate3;

			break;
		default:
			_ASSERT( 0 );
			rate = 0.0f;

			break;
		}

		float lr, lg, lb;
		lr = Diffuse.x * rate;
		lg = Diffuse.y * rate;
		lb = Diffuse.z * rate;

		LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

		//*addr = diffr * Diffuse.x * rate;		
		//*addg = diffg * Diffuse.y * rate;		
		//*addb = diffb * Diffuse.z * rate;		

		//　！！！specularも忘れずに！！！
		

		if( dot3 >= 0.0f ){
			*addspecr = 0.0f;
			*addspecg = 0.0f;
			*addspecb = 0.0f;
		}else{
			if( rate > 0.0f ){
				*addspecr = Diffuse.x * rate;
				*addspecg = Diffuse.y * rate;
				*addspecb = Diffuse.z * rate;
			}else{
				*addspecr = 0.0f;
				*addspecg = 0.0f;
				*addspecb = 0.0f;
			}
		}
	
		return 0;
	};

	__inline int CalcLightPhong( D3DXVECTOR3* veye, D3DTLVERTEX* orgtlv, D3DXVECTOR3* worldv, D3DXMATRIX* matWorld, D3DXVECTOR3* curn, float diffr, float diffg, float diffb, float specr, float specg, float specb, float power, float* addr, float* addg, float* addb, float* addspecr, float* addspecg, float* addspecb )
	{

		float tx = orgtlv->sx;
		float ty = orgtlv->sy;
		float tz = orgtlv->sz;
		D3DXVECTOR3 tlv;


		D3DXVECTOR3 prev = *worldv;


		D3DXVECTOR3 l;
		float nl;
		D3DXVECTOR3 r;
		float rv;
		D3DXVECTOR3 v;
		float rvms;

		//diffuse param
		l = -orgDirection;//!!!!!!!!! 点からライトへ
		nl = l.x * curn->x + l.y * curn->y + l.z * curn->z;

		//specular param
		r = 2.0f * nl * *curn - l;
		v.x = veye->x - prev.x;
		v.y = veye->y - prev.y;
		v.z = veye->z - prev.z;
		D3DXVec3Normalize( &v, &v );
		rv = D3DXVec3Dot( &r, &v );
		rvms = (float)pow( rv, power );

		float rate1, rate2;

		float lr, lg, lb;

		switch( Type ){
		case D3DLIGHT_DIRECTIONAL:

			lr = max( 0.0f, nl * Diffuse.x );
			lg = max( 0.0f, nl * Diffuse.y );
			lb = max( 0.0f, nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rvms * specr * Specular.x );
			*addspecg = max( 0.0f, rvms * specg * Specular.y );
			*addspecb = max( 0.0f, rvms * specb * Specular.z );

			break;

		case D3DLIGHT_POINT:
			rate1 = CalcRate1( &prev );

			lr = max( 0.0f, rate1 * nl * Diffuse.x );
			lg = max( 0.0f, rate1 * nl * Diffuse.y );
			lb = max( 0.0f, rate1 * nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, rate1 * nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, rate1 * nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, rate1 * nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rate1 * rvms * specr * Specular.x );
			*addspecg = max( 0.0f, rate1 * rvms * specg * Specular.y );
			*addspecb = max( 0.0f, rate1 * rvms * specb * Specular.z );

			break;

		case D3DLIGHT_SPOT:
			rate1 = CalcRate1( &prev );
			rate2 = CalcRate2( &prev );


			lr = max( 0.0f, rate1 * rate2 * nl * Diffuse.x );
			lg = max( 0.0f, rate1 * rate2 * nl * Diffuse.y );
			lb = max( 0.0f, rate1 * rate2 * nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, rate1 * rate2 * nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, rate1 * rate2 * nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, rate1 * rate2 * nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rate1 * rate2 * rvms * specr * Specular.x );
			*addspecg = max( 0.0f, rate1 * rate2 * rvms * specg * Specular.y );
			*addspecb = max( 0.0f, rate1 * rate2 * rvms * specb * Specular.z );

			break;

		default:
			break;
		}

		return 0;
	};

	__inline int CalcLightBlinn( D3DXVECTOR3* veye, D3DTLVERTEX* orgtlv, D3DXVECTOR3* worldv, D3DXMATRIX* matWorld, D3DXVECTOR3* curn, float diffr, float diffg, float diffb, float specr, float specg, float specb, float power, float* addr, float* addg, float* addb, float* addspecr, float* addspecg, float* addspecb )
	{

		float tx = orgtlv->sx;
		float ty = orgtlv->sy;
		float tz = orgtlv->sz;
		D3DXVECTOR3 tlv;


		D3DXVECTOR3 prev = *worldv;


		D3DXVECTOR3 l;
		float nl;
		float nh;
		D3DXVECTOR3 v;
		float nhms;

		D3DXVECTOR3 h;//blinn


		//diffuse param
		l = -orgDirection;//!!!!!!!!! 点からライトへ
		nl = l.x * curn->x + l.y * curn->y + l.z * curn->z;

		//specular param
		v.x = veye->x - prev.x;
		v.y = veye->y - prev.y;
		v.z = veye->z - prev.z;
		D3DXVec3Normalize( &v, &v );
		h = (l + v) * 0.5f;
		nh = D3DXVec3Dot( curn, &h );
		nhms = (float)pow( nh, power );

		float rate1, rate2;

		float lr, lg, lb;


		switch( Type ){
		case D3DLIGHT_DIRECTIONAL:

			lr = max( 0.0f, nl * Diffuse.x );
			lg = max( 0.0f, nl * Diffuse.y );
			lb = max( 0.0f, nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, nhms * specr * Specular.x );
			*addspecg = max( 0.0f, nhms * specg * Specular.y );
			*addspecb = max( 0.0f, nhms * specb * Specular.z );

			break;

		case D3DLIGHT_POINT:
			rate1 = CalcRate1( &prev );

			lr = max( 0.0f, rate1 * nl * Diffuse.x );
			lg = max( 0.0f, rate1 * nl * Diffuse.y );
			lb = max( 0.0f, rate1 * nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, rate1 * nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, rate1 * nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, rate1 * nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rate1 * nhms * specr * Specular.x );
			*addspecg = max( 0.0f, rate1 * nhms * specg * Specular.y );
			*addspecb = max( 0.0f, rate1 * nhms * specb * Specular.z );

			break;

		case D3DLIGHT_SPOT:
			rate1 = CalcRate1( &prev );
			rate2 = CalcRate2( &prev );

			lr = max( 0.0f, rate1 * rate2 * nl * Diffuse.x );
			lg = max( 0.0f, rate1 * rate2 * nl * Diffuse.y );
			lb = max( 0.0f, rate1 * rate2 * nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );


			//*addr = max( 0.0f, rate1 * rate2 * nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, rate1 * rate2 * nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, rate1 * rate2 * nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rate1 * rate2 * nhms * specr * Specular.x );
			*addspecg = max( 0.0f, rate1 * rate2 * nhms * specg * Specular.y );
			*addspecb = max( 0.0f, rate1 * rate2 * nhms * specb * Specular.z );

			break;

		default:
			break;
		}

		return 0;
	};


	__inline int CalcLightSchlick( D3DXVECTOR3* veye, D3DTLVERTEX* orgtlv, D3DXVECTOR3* worldv, D3DXMATRIX* matWorld, D3DXVECTOR3* curn, float diffr, float diffg, float diffb, float specr, float specg, float specb, float power, float* addr, float* addg, float* addb, float* addspecr, float* addspecg, float* addspecb )
	{

		float tx = orgtlv->sx;
		float ty = orgtlv->sy;
		float tz = orgtlv->sz;
		D3DXVECTOR3 tlv;


		D3DXVECTOR3 prev = *worldv;

		D3DXVECTOR3 l;
		float nl;
		D3DXVECTOR3 r;
		float rv;
		D3DXVECTOR3 v;
		float rvms;

		//diffuse param
		l = -orgDirection;//!!!!!!!!! 点からライトへ
		nl = l.x * curn->x + l.y * curn->y + l.z * curn->z;

		//specular param
		r = 2.0f * nl * *curn - l;
		v.x = veye->x - prev.x;
		v.y = veye->y - prev.y;
		v.z = veye->z - prev.z;
		D3DXVec3Normalize( &v, &v );
		rv = D3DXVec3Dot( &r, &v );
		//rvms = pow( rv, power );
		rvms = rv / ( power - power * rv + rv );//<---- Schlick !!!!!!!!!!!!!!


		float rate1, rate2;

		float lr, lg, lb;

		switch( Type ){
		case D3DLIGHT_DIRECTIONAL:

			lr = max( 0.0f, nl * Diffuse.x );
			lg = max( 0.0f, nl * Diffuse.y );
			lb = max( 0.0f, nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rvms * specr * Specular.x );
			*addspecg = max( 0.0f, rvms * specg * Specular.y );
			*addspecb = max( 0.0f, rvms * specb * Specular.z );

			break;

		case D3DLIGHT_POINT:
			rate1 = CalcRate1( &prev );

			lr = max( 0.0f, rate1 * nl * Diffuse.x );
			lg = max( 0.0f, rate1 * nl * Diffuse.y );
			lb = max( 0.0f, rate1 * nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, rate1 * nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, rate1 * nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, rate1 * nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rate1 * rvms * specr * Specular.x );
			*addspecg = max( 0.0f, rate1 * rvms * specg * Specular.y );
			*addspecb = max( 0.0f, rate1 * rvms * specb * Specular.z );

			break;

		case D3DLIGHT_SPOT:
			rate1 = CalcRate1( &prev );
			rate2 = CalcRate2( &prev );

			lr = max( 0.0f, rate1 * rate2 * nl * Diffuse.x );
			lg = max( 0.0f, rate1 * rate2 * nl * Diffuse.y );
			lb = max( 0.0f, rate1 * rate2 * nl * Diffuse.z );

			LightBlend( diffr, diffg, diffb, lr, lg, lb, addr, addg, addb );

			//*addr = max( 0.0f, rate1 * rate2 * nl * diffr * Diffuse.x );
			//*addg = max( 0.0f, rate1 * rate2 * nl * diffg * Diffuse.y );
			//*addb = max( 0.0f, rate1 * rate2 * nl * diffb * Diffuse.z );

			*addspecr = max( 0.0f, rate1 * rate2 * rvms * specr * Specular.x );
			*addspecg = max( 0.0f, rate1 * rate2 * rvms * specg * Specular.y );
			*addspecb = max( 0.0f, rate1 * rate2 * rvms * specb * Specular.z );

			break;

		default:
			break;
		}

		return 0;
	};

	__inline int LightBlend( float Par, float Pag, float Pab, float Pbr, float Pbg, float Pbb, float* Pnr, float* Png, float* Pnb )
	{

		//Pa * Pb --> a, Pb --> bとして計算

		float tempr, tempg, tempb;

		switch( lblend ){
		case LBLEND_MULT:
			*Pnr = Par * Pbr;
			*Png = Pag * Pbg;
			*Pnb = Pab * Pbb;
			break;
		case LBLEND_SCREEN:

			if( Pbr != 0.0f ){
				tempr = 1.0f - 2.0f * ( ( 1.0f - Par * Pbr ) * ( 1.0f - Pbr ) );
				*Pnr = max( Par * Pbr, tempr );
			}else{
				*Pnr = Par * Pbr;
			}

			if( Pbg != 0.0f ){
				tempg = 1.0f - 2.0f * ( ( 1.0f - Pag * Pbg ) * ( 1.0f - Pbg ) );
				*Png = max( Pag * Pbg, tempr );
			}else{
				*Png = Pag * Pbg;
			}

			if( Pbb != 0.0f ){
				tempb = 1.0f - 2.0f * ( ( 1.0f - Pab * Pbb ) * ( 1.0f - Pbb ) );
				*Pnb = max( Pab * Pbb, tempb );
			}else{
				*Pnb = Pab * Pbb;
			}

			break;
		case LBLEND_OVERLAY:
			//そのまま
			if( (Par * Pbr) < 0.5f ){
				*Pnr = Par * Pbr * Pbr * 2.0f;
			}else{
				if( Pbr != 0.0f ){
					tempr = 1.0f - 2.0f * ( ( 1.0f - Par * Pbr ) * ( 1.0f - Pbr ) );
					*Pnr = max( Par * Pbr, tempr );
				}else{
					*Pnr = Par * Pbr;
				}
			}

			if( (Pag * Pbg) < 0.5f ){
				*Png = Pag * Pbg * Pbg * 2.0f;
			}else{
				if( Pbg != 0.0f ){
					tempg = 1.0f - 2.0f * ( ( 1.0f - Pag * Pbg ) * ( 1.0f - Pbg ) );
					*Png = max( Pag * Pbg, tempr );
				}else{
					*Png = Pag * Pbg;
				}
			}

			if( (Pab * Pbb) < 0.5f ){
				*Pnb = Pab * Pbb * Pbb * 2.0f;
			}else{
				if( Pbb != 0.0f ){
					tempb = 1.0f - 2.0f * ( ( 1.0f - Pab * Pbb ) * ( 1.0f - Pbb ) );
					*Pnb = max( Pab * Pbb, tempb );
				}else{
					*Pnb = Pab * Pbb;
				}
			}
			break;
		case LBLEND_HARDLIGHT:
			//そのまま
			if( Pbr < 0.5f ){
				*Pnr = Par * Pbr * Pbr * 2.0f;
			}else{
				if( Pbr != 0.0f ){
					tempr = 1.0f - 2.0f * ( ( 1.0f - Par * Pbr ) * ( 1.0f - Pbr ) );
					*Pnr = max( Par * Pbr, tempr );
				}else{
					*Pnr = Par * Pbr;
				}
			}

			if( Pbg < 0.5f ){
				*Png = Pag * Pbg * Pbg * 2.0f;
			}else{
				if( Pbg != 0.0f ){
					tempg = 1.0f - 2.0f * ( ( 1.0f - Pag * Pbg ) * ( 1.0f - Pbg ) );
					*Png = max( Pag * Pbg, tempr );
				}else{
					*Png = Pag * Pbg;
				}
			}

			if( Pbb < 0.5f ){
				*Pnb = Pab * Pbb * Pbb * 2.0f;
			}else{
				if( Pbb != 0.0f ){
					tempb = 1.0f - 2.0f * ( ( 1.0f - Pab * Pbb ) * ( 1.0f - Pbb ) );
					*Pnb = max( Pab * Pbb, tempb );
				}else{
					*Pnb = Pab * Pbb;
				}
			}
			break;
		case LBLEND_DODGE:
			if( Pbr == 0.0f ){
				*Pnr = Par * Pbr;
			}else if( Pbr != 1.0f ){
				*Pnr = Par * Pbr / ( 1.0f - Pbr );
			}else{
				*Pnr = 1.0f;
			}

			if( Pbg == 0.0f ){
				*Png = Pag * Pbg;
			}else if( Pbg != 1.0f ){
				*Png = Pag * Pbg / ( 1.0f - Pbg );
			}else{
				*Png = 1.0f;
			}

			if( Pbb == 0.0f ){
				*Pnb = Pab * Pbb;
			}else if( Pbb != 1.0f ){
				*Pnb = Pab * Pbb / ( 1.0f - Pbb );
			}else{
				*Pnb = 1.0f;
			}
			break;
		default:
			_ASSERT( 0 );
			*Pnr = Par * Pbr;
			*Png = Pag * Pbg;
			*Pnb = Pab * Pbb;
			break;
		}
		return 0;
	};

/***
	__inline int LightBlend( float Par, float Pag, float Pab, float Pbr, float Pbg, float Pbb, float* Pnr, float* Png, float* Pnb )
	{
		switch( lblend ){
		case LBLEND_MULT:
			*Pnr = Par * Pbr;
			*Png = Pag * Pbg;
			*Pnb = Pab * Pbb;
			break;
		case LBLEND_SCREEN:
			*Pnr = 1.0f - ( ( 1.0f - Par * Pbr ) * ( 1.0f - Pbr ) );
			*Png = 1.0f - ( ( 1.0f - Pag * Pbg ) * ( 1.0f - Pbg ) );
			*Pnb = 1.0f - ( ( 1.0f - Pab * Pbb ) * ( 1.0f - Pbb ) );
			break;
		case LBLEND_OVERLAY:
			if( Par < 0.5f ){
				*Pnr = Par * Pbr * Pbr * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Par * Pbr ) * ( 1.0f - Pbr );
				//*Pnr = max( 0.0f, temp );
			}

			if( Pag < 0.5f ){
				*Png = Pag * Pbg * Pbg * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pag * Pbg ) * ( 1.0f - Pbg );
				//*Png = max( 0.0f, temp );
			}

			if( Pab < 0.5f ){
				*Pnb = Pab * Pbb * Pbb * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pab * Pbb ) * ( 1.0f - Pbb );
				//*Pnb = max( 0.0f, temp );
			}
			break;
		case LBLEND_HARDLIGHT:
			if( Pbr < 0.5f ){
				*Pnr = Par * Pbr * Pbr * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Par * Pbr ) * ( 1.0f - Pbr );
				//*Pnr = max( 0.0f, temp );
			}

			if( Pbg < 0.5f ){
				*Png = Pag * Pbg * Pbg * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pag * Pbg ) * ( 1.0f - Pbg );
				//*Png = max( 0.0f, temp );
			}

			if( Pbb < 0.5f ){
				*Pnb = Pab * Pbb * Pbb * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pab * Pbb ) * ( 1.0f - Pbb );
				//*Pnb = max( 0.0f, temp );
			}
			break;
		case LBLEND_DODGE:
			if( Pbr != 1.0f ){
				*Pnr = Par / ( 1.0f - Pbr );
			}else{
				*Pnr = 1.0f;
			}

			if( Pbg != 1.0f ){
				*Png = Pag / ( 1.0f - Pbg );
			}else{
				*Png = 1.0f;
			}

			if( Pbb != 1.0f ){
				*Pnb = Pab / ( 1.0f - Pbb );
			}else{
				*Pnb = 1.0f;
			}
			break;
		default:
			_ASSERT( 0 );
			*Pnr = Par * Pbr;
			*Png = Pag * Pbg;
			*Pnb = Pab * Pbb;
			break;
		}
		return 0;
	};
***/

/***
	__inline int LightBlend( float Par, float Pag, float Pab, float Pbr, float Pbg, float Pbb, float* Pnr, float* Png, float* Pnb )
	{
		switch( lblend ){
		case LBLEND_MULT:
			*Pnr = Par * Pbr;
			*Png = Pag * Pbg;
			*Pnb = Pab * Pbb;
			break;
		case LBLEND_SCREEN:
			*Pnr = 1.0f - ( ( 1.0f - Par ) * ( 1.0f - Pbr ) );
			*Png = 1.0f - ( ( 1.0f - Pag ) * ( 1.0f - Pbg ) );
			*Pnb = 1.0f - ( ( 1.0f - Pab ) * ( 1.0f - Pbb ) );
			break;
		case LBLEND_OVERLAY:
			if( Par < 0.5f ){
				*Pnr = Par * Pbr * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Par ) * ( 1.0f - Pbr );
				*Pnr = max( 0.0f, temp );
			}

			if( Pag < 0.5f ){
				*Png = Pag * Pbg * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pag ) * ( 1.0f - Pbg );
				*Png = max( 0.0f, temp );
			}

			if( Pab < 0.5f ){
				*Pnb = Pab * Pbb * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pab ) * ( 1.0f - Pbb );
				*Pnb = max( 0.0f, temp );
			}
			break;
		case LBLEND_HARDLIGHT:
			if( Pbr < 0.5f ){
				*Pnr = Par * Pbr * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Par ) * ( 1.0f - Pbr );
				*Pnr = max( 0.0f, temp );
			}

			if( Pbg < 0.5f ){
				*Png = Pag * Pbg * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pag ) * ( 1.0f - Pbg );
				*Png = max( 0.0f, temp );
			}

			if( Pbb < 0.5f ){
				*Pnb = Pab * Pbb * 2.0f;
			}else{
				float temp;
				temp = 1.0f - 2.0f * ( 1.0f - Pab ) * ( 1.0f - Pbb );
				*Pnb = max( 0.0f, temp );
			}
			break;
		case LBLEND_DODGE:
			if( Pbr != 1.0f ){
				*Pnr = Par / ( 1.0f - Pbr );
			}else{
				*Pnr = 1.0f;
			}

			if( Pbg != 1.0f ){
				*Png = Pag / ( 1.0f - Pbg );
			}else{
				*Png = 1.0f;
			}

			if( Pbb != 1.0f ){
				*Pnb = Pab / ( 1.0f - Pbb );
			}else{
				*Pnb = 1.0f;
			}
			break;
		default:
			_ASSERT( 0 );
			*Pnr = Par * Pbr;
			*Png = Pag * Pbg;
			*Pnb = Pab * Pbb;
			break;
		}
		return 0;
	};
***/

	__inline float CalcRate1( D3DXVECTOR3* tlv ){
			
		float dist, diffx, diffy, diffz;
		diffx = tlv->x - orgPosition.x;
		diffy = tlv->y - orgPosition.y;
		diffz = tlv->z - orgPosition.z;

		dist = diffx * diffx + diffy * diffy + diffz * diffz;
		
		float retrate;
		retrate = 1.0f - dist * divRange;
		if( retrate < 0.0f )
			retrate = 0.0f;
		else if( retrate > 1.0f )
			retrate = 1.0f;

		return retrate;
	};

	__inline float CalcRate2( D3DXVECTOR3* tlv ){
		D3DXVECTOR3 l2tlv;
		float dot2;
		
		l2tlv.x = tlv->x - orgPosition.x;
		l2tlv.y = tlv->y - orgPosition.y;
		l2tlv.z = tlv->z - orgPosition.z;

		float mag, sqmag, divsqmag;
		mag = l2tlv.x * l2tlv.x + l2tlv.y * l2tlv.y + l2tlv.z * l2tlv.z;
		sqmag = sqrtf( mag );
		if( sqmag != 0.0f ){
			divsqmag = 1.0f / sqmag;
			l2tlv.x *= divsqmag;
			l2tlv.y *= divsqmag;
			l2tlv.z *= divsqmag;
		}else{
			l2tlv.x = 0.0f;
			l2tlv.y = 0.0f;
			l2tlv.z = 0.0f;
		}
		
		dot2 = orgDirection.x * l2tlv.x + orgDirection.y * l2tlv.y + orgDirection.z * l2tlv.z;
	
		float retrate;
		retrate = (dot2 - cosPhi) * divcosPhi;
		if( retrate < 0.0f )
			retrate = 0.0f;
		else if( retrate > 1.0f )
			retrate = 1.0f;

		return retrate;
	};

	__inline float CalcRate3( D3DXVECTOR3* curn, float* dot3ptr ){
		float dot3;

		//dot3 = traDirection.x * curn->x + traDirection.y * curn->y + traDirection.z * curn->z;
		dot3 = orgDirection.x * curn->x + orgDirection.y * curn->y + orgDirection.z * curn->z;

		float retrate;
		retrate = -0.5f * dot3 + 0.5f;
		if( retrate < 0.0f )
			retrate = 0.0f;
		else if( retrate > 1.0f )
			retrate = 1.0f;

		*dot3ptr = dot3;

		return retrate;
	};

public:	
	//D3DLIGHT9 にならって、パラメータを確保。
	//( 使用しないかもしれないが。。)
	int				lightid;
	int				Type;
	CVec3f			Diffuse;// 0 から 1
	CVec3f			Specular;// 0 から 1
	CVec3f			Ambient;// 0 から 1
	D3DXVECTOR3		orgPosition;
	D3DXVECTOR3		traPosition;
	D3DXVECTOR3		orgDirection;
	D3DXVECTOR3		traDirection;
	float           Range;//距離 （の２乗にする）
	float           Falloff;
	float           Attenuation0;
	float           Attenuation1;
	float           Attenuation2;
	float           Theta;//内部コーン　ラジアン
	float           Phi;//SPOTの外側エッジ　ラジアン

	float			divRange;// 1 / Range
	float			cosPhi;// cos( Phi )
	float			divcosPhi;// 1 / (1 - cos( Phi ))

	float			reserved0[3];
	float			reserved1[3];
	float			reserved2[3];

	int				lblend;

	CLightData*		nextlight;
};



#endif