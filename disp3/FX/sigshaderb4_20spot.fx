// Copyright (c) 2006-2009 ochakko lab http://www5d.biglobe.ne.jp/~ochikko  All rights reserved.
//

// Matrix Pallette
float4x4    mWorldMatrixArray[43] : WORLDMATRIXARRAY;// MAXSKINMATRIX = 43 - 1 --> 42
float4x4    mView : VIEW;
float4x4	mProj : PROJECTION;
float4x4	moldView;


//float4x4	mZeroMat = {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};

//float3 mLightDir;
//float3 mLightDiffuse;
//float3 mLightSpecular;
//float3 mLightAmbient;
float3 mEyePos;



float4 mLightParams[3][6];
//0: x=validflag, y=type, z=divRange
//1: x=cosPhi, y=divCosPhi
//2: xyz=diffuse
//3: xyz=specular
//4: xyz=position
//5: xyz=direction

float4 mBumpLight[3][6];



float3	mFogParams;
float4	mToonParams;


float mLightNum;
float4 mEdgeCol0;
float mAlpha;

//for shadow
float4x4	mLP;
float4x4	mLPB;
float2		mShadowCoef;//x:Bias, y:darkness

//motionblur
float2 mMinAlpha;

//glow
float3 mGlowMult;

//toon0
float3 mToonAmbient;
float3 mToonDiffuse;
float3 mToonSpecular;

float2 mZNUser1;
float4 mZNUser2;


//////////////////////////
// texture
//////////////////////////
texture NormalMap;
sampler NormalSamp = sampler_state
{
    Texture = <NormalMap>;
    AddressU = Wrap;
    AddressV = Wrap;
};
texture DecaleTex;
sampler DecaleSamp = sampler_state
{
    Texture = <DecaleTex>;
    AddressU = Wrap;
    AddressV = Wrap;
};
texture DecaleTex1;
sampler DecaleSamp1 = sampler_state
{
    Texture = <DecaleTex1>;
    AddressU = Wrap;
    AddressV = Wrap;
};
texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Clamp;//必要！
    AddressV = Clamp;
};

///////////////////////////////////////////////////////

struct VS_INPUT_TIMA
{
    float4  Pos             : POSITION;
    float4  Normal          : NORMAL;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;

    float2  Tex0            : TEXCOORD0;
	float4	Diffuse			: COLOR0;
	float3	Specular		: COLOR1;
	float3	Ambient			: TEXCOORD1;
	float	Power			: TEXCOORD2;
	float3	Emissive		: TEXCOORD3;
};
struct VS_INPUT_MEMA
{
    float4  Pos             : POSITION;
    float4  Normal          : NORMAL;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;

    float2  Tex0            : TEXCOORD0;
};


struct VSBUMP_INPUT
{
    float4  Pos             : POSITION;
    float4  Normal          : NORMAL;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;

    float2  Tex0            : TEXCOORD0;
	float4	Diffuse			: COLOR0;
	float3	Specular		: COLOR1;
	float3	Ambient			: TEXCOORD1;
	float	Power			: TEXCOORD2;
	float3	Emissive		: TEXCOORD3;
	
	float4	Tangent			: TEXCOORD4;
};



struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
    float2  Tex0    : TEXCOORD0;
    float1	Fog		: FOG;
};

struct VSBUMPL1_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
    float2  Tex0    : TEXCOORD0;
	float4	L		: TEXCOORD1;
	float3	Eye		: TEXCOORD2;
	float3	Ambient : TEXCOORD3;
	float1	Power	: TEXCOORD4;
	float3	Emissive : TEXCOORD5;

    float1	Fog		: FOG;
};

struct VSBUMPL2_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
    float2  Tex0    : TEXCOORD0;
	float4	L1		: TEXCOORD1;
	float4	L2		: TEXCOORD2;
	float3	Eye		: TEXCOORD3;
	float3	Ambient : TEXCOORD4;
	float1	Power	: TEXCOORD5;
	float3	Emissive : TEXCOORD6;

    float1	Fog		: FOG;
};

struct VSBUMPL3_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
    float2  Tex0    : TEXCOORD0;
	float4	L1		: TEXCOORD1;
	float4	L2		: TEXCOORD2;
	float4	L3		: TEXCOORD3;
	float3	Eye		: TEXCOORD4;
	float3	Ambient : TEXCOORD5;
	float1	Power	: TEXCOORD6;
	float3	Emissive : TEXCOORD7;

    float1	Fog		: FOG;
};

struct VSPPL1_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
	float3	wEye	: TEXCOORD0;
	float3	wPos	: TEXCOORD1;
    float2  Tex0    : TEXCOORD2;
	float4	L1		: TEXCOORD3;
	float3	Ambient : TEXCOORD4;
	float4	NandPow	: TEXCOORD5;
	float3	Emissive : TEXCOORD6;

    float1	Fog		: FOG;
};
struct VSPPL2_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
	float3	wEye	: TEXCOORD0;
	float3	wPos	: TEXCOORD1;
    float2  Tex0    : TEXCOORD2;
	float4	L1		: TEXCOORD3;
	float4	L2		: TEXCOORD4;
	float3	AmbEmi : TEXCOORD5;
	float4	NandPow	: TEXCOORD6;

    float1	Fog		: FOG;
};
struct VSPPL3_OUTPUT
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
	float3	wEye	: TEXCOORD0;
	float3	wPos	: TEXCOORD1;
    float2  Tex0    : TEXCOORD2;
	float4	L1		: TEXCOORD3;
	float4	L2		: TEXCOORD4;
	float4	L3		: TEXCOORD5;
	float3	AmbEmi : TEXCOORD6;
	float4	NandPow	: TEXCOORD7;

    float1	Fog		: FOG;
};



struct VSTOON_OUTPUT
{
    float4  Pos     : POSITION;
    float4	Diffuse	: COLOR0;
    float2  Tex0    : TEXCOORD0;
    float1	NL		: TEXCOORD1;
    float3  Diffuse2 : TEXCOORD2;
    float3	Diffuse3 : TEXCOORD3;
    float1	Fog		: FOG;

};

struct VSTOON1_OUTPUT
{
    float4  Pos     : POSITION;
    float4	Diffuse	: COLOR0;
    float2  Tex0    : TEXCOORD0;
    float2  Tex1    : TEXCOORD1;
    float1	Fog		: FOG;
};

struct VSEDGE0_OUTPUT
{
    float4  Pos     : POSITION;
    float4	Diffuse	: COLOR0;
    float1	Fog		: FOG;
};

struct VSSHADOW_OUTPUT0
{
	float4	Pos			: POSITION;
	float2	Tex0		: TEXCOORD0;
	float4	ShadowMapUV	: TEXCOORD1;
};
struct VSSHADOW_OUTPUT1
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
    float4	Ambient	: TEXCOORD0;
    float2  Tex0    : TEXCOORD1;
    float4	ShadowMapUV	: TEXCOORD2;
    float4	Depth	: TEXCOORD3;
    float1	Fog		: FOG;
};

struct VSBUMPSHADOW_OUTPUT1
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
    float2  Tex0    : TEXCOORD0;
	float3	L		: TEXCOORD1;
	float3	Eye		: TEXCOORD2;
	float3	Ambient : TEXCOORD3;
	float1	Power	: TEXCOORD4;
	float3	Emissive : TEXCOORD5;
    float4	ShadowMapUV	: TEXCOORD6;
    float4	Depth	: TEXCOORD7;

    float1	Fog		: FOG;
};

struct VSPPSHADOW_OUTPUT1
{
    float4  Pos     : POSITION;
    float4  Diffuse : COLOR0;
    float4	Specular : COLOR1;
    float2  Tex0    : TEXCOORD0;
	float4	L		: TEXCOORD1;
	float3	Eye		: TEXCOORD2;
	float3	Ambient : TEXCOORD3;
	float4	NandPow	: TEXCOORD4;
	float3	Emissive : TEXCOORD5;
    float4	ShadowMapUV	: TEXCOORD6;
    float4	Depth	: TEXCOORD7;

    float1	Fog		: FOG;
};


struct VSTOONSHADOW_OUTPUT1
{
    float4  Pos     : POSITION;
    float4	Diffuse	: COLOR0;
    float2  Tex0    : TEXCOORD0;
    float1	NL		: TEXCOORD1;
    float3  Diffuse2 : TEXCOORD2;
    float3	Diffuse3 : TEXCOORD3;
    float4	ShadowMapUV	: TEXCOORD4;
    float4	Depth	: TEXCOORD5;
    float1	Fog		: FOG;

};


struct VSTOON1SHADOW_OUTPUT1
{
    float4  Pos     : POSITION;
    float4	Diffuse	: COLOR0;
    float2  Tex0    : TEXCOORD0;
    float2  Tex1    : TEXCOORD1;
    float4	ShadowMapUV	: TEXCOORD2;
    float4	Depth	: TEXCOORD3;
    float1	Fog		: FOG;
};

struct VSZN_OUTPUT
{
	float4	Pos		: POSITION;
	float4	PSPos	: TEXCOORD0;
	float4	PSNormal	: TEXCOORD1;
};


struct PS_OUTPUT
{
	float4 Color	: COLOR0;
};

struct PS_ZNOUTPUT
{
	float4 depth	: COLOR0;
	float4 normal	: COLOR1;
};


///////////////////////

float4 CalcLightDir( in int lno, in float3 v, in float3 worldPos, in float3 Normal, float iPower );
float4 CalcLightPoint( in int lno, in float3 v, in float3 worldPos, in float3 Normal, float iPower );
float4 CalcLightSpot( in int lno, in float3 v, in float3 worldPos, in float3 Normal, float iPower );


float4 CalcLightDir( in int lno, in float3 v, in float3 worldPos, in float3 Normal, float iPower )
{

//float4 mLightParams[5][7];
//0: x=validflag, y=type, z=divRange
//1: x=cosPhi, y=divCosPhi
//2: xyz=diffuse
//3: xyz=specular
//4: xyz=position
//5: xyz=direction

	float3 l;
	float nl;
	float3 h;
	float nh;
	float rate1;
	float mag;
	
	float4 tmplight;
		
	//directional light
	l = -mLightParams[lno][5].xyz;
	nl = dot( Normal.xyz, l );

	mag = 1.0f;
	rate1 = 1.0f;

	h = ( l + v ) * 0.5f;
	nh = dot( Normal.xyz, h );
			
	tmplight= lit( nl, nh, iPower );
	return tmplight;

}
float4 CalcLightPoint( in int lno, in float3 v, in float3 worldPos, in float3 Normal, float iPower )
{
//float4 mLightParams[5][7];
//0: x=validflag, y=type, z=divRange
//1: x=cosPhi, y=divCosPhi
//2: xyz=diffuse
//3: xyz=specular
//4: xyz=position
//5: xyz=direction

	float3 l;
	float nl;
	float3 h;
	float nh;

	float4 tmplight;
				
	l = mLightParams[lno][4].xyz - worldPos.xyz;
	float mag;
	mag = dot( l, l );
	l = normalize( l );
	nl = dot( Normal.xyz, l );
	h = ( l + v ) * 0.5f;
	nh = dot( Normal.xyz, h );
								
	//
	float rate1;
	rate1 = 1.0f - mag * mLightParams[lno][0].z;
	rate1 = clamp( rate1, 0.0f, 1.0f );
	nl *= rate1;
	nh *= rate1;
		
	tmplight= lit( nl, nh, iPower );
	return tmplight;
		
}

float4 CalcLightSpot( in int lno, in float3 v, in float3 worldPos, in float3 Normal, float iPower )
{
//float4 mLightParams[5][7];
//0: x=validflag, y=type, z=divRange
//1: x=cosPhi, y=divCosPhi
//2: xyz=diffuse
//3: xyz=specular
//4: xyz=position
//5: xyz=direction

	float3 l;
	float nl;
	float3 h;
	float nh;
	
	float4 tmplight;
	float rate1, rate2;
	float mag;
	float dot2;

	l = mLightParams[lno][4].xyz - worldPos.xyz;
	mag = dot( l, l );
	l = normalize(l);	
	dot2 = dot( mLightParams[lno][5].xyz, -l );

	nl = dot( Normal.xyz, l );
	h = ( l + v ) * 0.5f;
	nh = dot( Normal.xyz, h );

	rate1 = clamp( 1.0f - mag * mLightParams[lno][0].z, 0.0f, 1.0f );
	rate2 = clamp( ( dot2 - mLightParams[lno][1].x ) * mLightParams[lno][1].y, 0.0f, 1.0f );
		
	nl = rate1 * rate2 * nl;
	nh = rate1 * rate2 * nh;										

	tmplight= lit( nl, nh, iPower );
	return tmplight;

}

///////////////////////


VS_OUTPUT SkinB4(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}
	
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}


VS_OUTPUT SkinB1(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}


VS_OUTPUT SkinB0(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
    //o.Pos = mul(Pos, mViewProj);

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

    return o;
}

VS_OUTPUT SkinB4BlurWorld(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		//o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		//o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}
	
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VS_OUTPUT SkinB4BlurCamera(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		//o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		//o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}
	
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}


VS_OUTPUT SkinB1BlurWorld(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];
	
	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		//o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		//o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VS_OUTPUT SkinB1BlurCamera(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];

	
	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);


	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		//o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		//o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}


VS_OUTPUT SkinB0BlurWorld(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		//o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		//o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

    return o;
}
VS_OUTPUT SkinB0BlurCamera(VS_INPUT_TIMA i)
{
    VS_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);


	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		//o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		//o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
	}

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

    return o;
}



VSTOON_OUTPUT SkinB4Toon(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;


    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSTOON_OUTPUT SkinB1Toon(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;


    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON_OUTPUT SkinB0Toon(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;

    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON_OUTPUT SkinB4ToonBlurWorld(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	//o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;

    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON_OUTPUT SkinB1ToonBlurWorld(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];
	
	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	//o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;


    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSTOON_OUTPUT SkinB0ToonBlurWorld(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	//o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;

    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON_OUTPUT SkinB4ToonBlurCamera(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	//o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;

    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON_OUTPUT SkinB1ToonBlurCamera(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];

	
	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	//o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;


    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSTOON_OUTPUT SkinB0ToonBlurCamera(VS_INPUT_MEMA i)
{
    VSTOON_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	//o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;

    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}




VSTOONSHADOW_OUTPUT1 SkinB4Toon0Shadow1(VS_INPUT_MEMA i)
{
    VSTOONSHADOW_OUTPUT1   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;

    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );
	
	
    return o;
}

VSTOONSHADOW_OUTPUT1 SkinB0Toon0Shadow1(VS_INPUT_MEMA i)
{
    VSTOONSHADOW_OUTPUT1   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	if( mLightNum >= 0.0f ){	
		o.NL.x = nl;
	}else{
		o.NL.x = 0.0f;
	}
	o.Diffuse.xyz = mToonDiffuse;
	o.Diffuse.w = mAlpha;
	o.Diffuse2 = mToonAmbient;
	o.Diffuse3 = mToonSpecular;

    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

	
    return o;
}

//////////////

VSTOON1_OUTPUT SkinB4Toon1(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
	o.Diffuse.w = mAlpha;
	

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSTOON1_OUTPUT SkinB1Toon1(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
	o.Diffuse.w = mAlpha;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON1_OUTPUT SkinB0Toon1(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
	o.Diffuse.w = mAlpha;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON1_OUTPUT SkinB4Toon1BlurWorld(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
//	o.Diffuse.w = mAlpha;
	

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSTOON1_OUTPUT SkinB1Toon1BlurWorld(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];
	
	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
//	o.Diffuse.w = mAlpha;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON1_OUTPUT SkinB0Toon1BlurWorld(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( Normal.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
//	o.Diffuse.w = mAlpha;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSTOON1_OUTPUT SkinB4Toon1BlurCamera(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
//	o.Diffuse.w = mAlpha;
	

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSTOON1_OUTPUT SkinB1Toon1BlurCamera(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];

	
	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);


	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
//	o.Diffuse.w = mAlpha;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSTOON1_OUTPUT SkinB0Toon1BlurCamera(VS_INPUT_MEMA i)
{
    VSTOON1_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    Normal = mul(i.Normal, finalmat1 );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float4 NView = mul( Normal, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);



	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
//	o.Diffuse.w = mAlpha;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}



VSTOON1SHADOW_OUTPUT1 SkinB4Toon1Shadow1(VS_INPUT_MEMA i)
{
    VSTOON1SHADOW_OUTPUT1   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
	o.Diffuse.w = mAlpha;
	

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

	
    return o;
}

VSTOON1SHADOW_OUTPUT1 SkinB0Toon1Shadow1(VS_INPUT_MEMA i)
{
    VSTOON1SHADOW_OUTPUT1   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 l;
	l = -mLightParams[0][5].xyz;
	float nl;
	nl = dot( Normal.xyz, l );

	o.Tex0.x = 0.5f;
	if( mLightNum >= 0.0f ){
		if( nl > mToonParams.w ){
			o.Tex0.y = 0.9f;
		}else{
			if( nl > mToonParams.z ){
				o.Tex0.y = 0.5f;
			}else{
				o.Tex0.y = 0.15f;
			}
		}		
	}else{
		o.Tex0.y = 0.0f;
	}
	o.Tex1 = i.Tex0.xy;

	o.Diffuse.x = 1.0f;
	o.Diffuse.y = 1.0f;
	o.Diffuse.z = 1.0f;
	o.Diffuse.w = mAlpha;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

	
    return o;
}


//////// Edge0
VSEDGE0_OUTPUT SkinB4Edge0(VS_INPUT_MEMA i)
{
    VSEDGE0_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	wPos = wPos + mEdgeCol0.w * Normal;

	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	o.Diffuse.w = mAlpha;
	o.Diffuse.xyz = mEdgeCol0.xyz;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}
VSEDGE0_OUTPUT SkinB1Edge0(VS_INPUT_MEMA i)
{
    VSEDGE0_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[i.BlendIndices.x];
	

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	wPos = wPos + mEdgeCol0.w * Normal;

	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	o.Diffuse.w = mAlpha;
	o.Diffuse.xyz = mEdgeCol0.xyz;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

VSEDGE0_OUTPUT SkinB0Edge0(VS_INPUT_MEMA i)
{
    VSEDGE0_OUTPUT   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[0];
	

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	wPos = wPos + mEdgeCol0.w * Normal;

	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	o.Diffuse.w = mAlpha;
	o.Diffuse.xyz = mEdgeCol0.xyz;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
    return o;
}

///// Bump VS
VSBUMPL1_OUTPUT SkinB4BumpL1(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL2_OUTPUT SkinB4BumpL2(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB4BumpL3(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}


VSBUMPL1_OUTPUT SkinB1BumpL1(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );

	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;
	
    return o;
}
VSBUMPL2_OUTPUT SkinB1BumpL2(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB1BumpL3(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}



VSBUMPL1_OUTPUT SkinB0BumpL1(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
    //o.Pos = mul(Pos, mViewProj);

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );

	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;
	
    return o;
}
VSBUMPL2_OUTPUT SkinB0BumpL2(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB0BumpL3(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}



VSBUMPL1_OUTPUT SkinB4BumpL1BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL2_OUTPUT SkinB4BumpL2BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB4BumpL3BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}


VSBUMPL1_OUTPUT SkinB1BumpL1BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );

	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;
	
    return o;
}
VSBUMPL2_OUTPUT SkinB1BumpL2BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB1BumpL3BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}



VSBUMPL1_OUTPUT SkinB0BumpL1BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );

	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;
	
    return o;
}
VSBUMPL2_OUTPUT SkinB0BumpL2BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB0BumpL3BlurWorld(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}


VSBUMPL1_OUTPUT SkinB4BumpL1BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );


	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL2_OUTPUT SkinB4BumpL2BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB4BumpL3BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}


VSBUMPL1_OUTPUT SkinB1BumpL1BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );

	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;
	
    return o;
}
VSBUMPL2_OUTPUT SkinB1BumpL2BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB1BumpL3BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[i.BlendIndices.x + 20];
	float4x4 finalmat1 = mWorldMatrixArray[i.BlendIndices.x];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}



VSBUMPL1_OUTPUT SkinB0BumpL1BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );

	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;
	
    return o;
}
VSBUMPL2_OUTPUT SkinB0BumpL2BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSBUMPL3_OUTPUT SkinB0BumpL3BlurCamera(VSBUMP_INPUT i)
{
    VSBUMPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
//	float4 N = mul( i.Normal, finalmat );
//	N.w = 0.0f;
//	normalize( N );
	float4 T = mul( i.Tangent, finalmat1 );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float3 L1;
	float Lrate1;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L1 = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate1 = 1.0f - dot( L1, L1 ) * mBumpLight[0][0].z;
		Lrate1 = clamp( Lrate1, 0.0f, 1.0f );
	}
	o.L1.x = dot( L1, T.xyz );
	o.L1.y = dot( L1, B );
	o.L1.z = dot( L1, N.xyz );
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mBumpLight[1][0].y == 0 ){
		L2 = -mBumpLight[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		L2 = mBumpLight[1][4].xyz - wPos.xyz;	
		Lrate2 = 1.0f - dot( L2, L2 ) * mBumpLight[1][0].z;
		Lrate2 = clamp( Lrate2, 0.0f, 1.0f );
	}
	o.L2.x = dot( L2, T.xyz );
	o.L2.y = dot( L2, B );
	o.L2.z = dot( L2, N.xyz );
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mBumpLight[2][0].y == 0 ){
		L3 = -mBumpLight[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		L3 = mBumpLight[2][4].xyz - wPos.xyz;	
		Lrate3 = 1.0f - dot( L3, L3 ) * mBumpLight[2][0].z;
		Lrate3 = clamp( Lrate3, 0.0f, 1.0f );
	}
	o.L3.x = dot( L3, T.xyz );
	o.L3.y = dot( L3, B );
	o.L3.z = dot( L3, N.xyz );
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

    return o;
}


/////////////
/////////////


VSSHADOW_OUTPUT0 SkinB0Shadow0(VS_INPUT_TIMA i)
{
    VSSHADOW_OUTPUT0   o;
    float4      wPos, Pos;

	float4x4 finalmat = mWorldMatrixArray[0];
	    
    wPos = mul( i.Pos, finalmat );
    Pos = mul( wPos, mLP );
    
    o.Pos = Pos;
	o.ShadowMapUV = Pos;
	
	o.Tex0 = i.Tex0;
	
    return o;
}

VSSHADOW_OUTPUT0 SkinB4Shadow0(VS_INPUT_TIMA i)
{
    VSSHADOW_OUTPUT0   o;
    float4      wPos, Pos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	    
    wPos = mul( i.Pos, finalmat );
    Pos = mul( wPos, mLP );
    
    o.Pos = Pos;
	o.ShadowMapUV = Pos;
	
	o.Tex0 = i.Tex0;
	
    return o;
}
VSSHADOW_OUTPUT1 SkinB0Shadow1(VS_INPUT_TIMA i)
{
    VSSHADOW_OUTPUT1   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		//o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		o.Diffuse.xyz = calcdiff + i.Emissive.xyz;
		o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
		
		o.Ambient.xyz = i.Ambient;
		o.Ambient.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;

		o.Ambient.xyz = i.Ambient;
		o.Ambient.w = 0.0f;

	}
	
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

	
    return o;

}

VSSHADOW_OUTPUT1 SkinB4Shadow1(VS_INPUT_TIMA i)
{
    VSSHADOW_OUTPUT1   o;
    float4      wPos, vPos;
    float4      Normal;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);


    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);

	float3 v;
	v = mEyePos - wPos.xyz;
	v = normalize( v );

	float3 calcdiff = {0.0f, 0.0f, 0.0f};
	float3 calcspec = {0.0f, 0.0f, 0.0f};

	float3 tmppos;
	float3 dummypos = {1.0f, 1.0f, 1.0f};

	if( (wPos.x == 0.0f) && (wPos.y == 0.0f) && (wPos.z == 0.0f) ){// wPos が原点のとき、なぜか、計算がおかしくなるので。
		tmppos = dummypos;
	}else{
		tmppos = wPos.xyz;
	}

	float4 tmplight;
	if( mLightNum >= 0.9f ){
		if( mLightParams[0][0].y == 0 ){
			tmplight = CalcLightDir( 0, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 0, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[0][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[0][3].xyz * i.Specular.xyz;
	}

	if( mLightNum >= 1.9f ){
		if( mLightParams[1][0].y == 0 ){
			tmplight = CalcLightDir( 1, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 1, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[1][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[1][3].xyz * i.Specular.xyz;
	}
	
	if( mLightNum >= 2.9f ){
		if( mLightParams[2][0].y == 0 ){
			tmplight = CalcLightDir( 2, v, tmppos, Normal.xyz, i.Power );
		}else{
			tmplight = CalcLightSpot( 2, v, tmppos, Normal.xyz, i.Power );
		}
		calcdiff += tmplight.y * mLightParams[2][2].xyz * i.Diffuse.xyz;
		calcspec += tmplight.z * mLightParams[2][3].xyz * i.Specular.xyz;	
	}

	if( mLightNum >= 0.0f ){
		//o.Diffuse.xyz = i.Ambient + calcdiff + i.Emissive.xyz;
		o.Diffuse.xyz = calcdiff + i.Emissive.xyz;
		o.Diffuse.w = mAlpha;
	
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;
		
		o.Ambient.xyz = i.Ambient;
		o.Ambient.w = 0.0f;
	}else{
		o.Diffuse.xyz = i.Diffuse.xyz;
		o.Diffuse.w = mAlpha;
		o.Specular.xyz = calcspec;
		o.Specular.w = 0.0f;

		o.Ambient.xyz = i.Ambient;
		o.Ambient.w = 0.0f;

	}
	
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}
	
	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

	
    return o;

}

VSBUMPSHADOW_OUTPUT1 SkinB4BumpShadow1(VSBUMP_INPUT i)
{
    VSBUMPSHADOW_OUTPUT1   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );

	o.Diffuse.xyz = i.Diffuse.xyz * Lrate;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz * Lrate;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

    return o;
}

VSBUMPSHADOW_OUTPUT1 SkinB0BumpShadow1(VSBUMP_INPUT i)
{
    VSBUMPSHADOW_OUTPUT1   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );
	float4 T = mul( i.Tangent, finalmat );
	T.w = 0.0f;
	normalize( T );
	float3 B = cross( N.xyz, T.xyz );

	// 鏡面反射用のベクトル
	float3 Eye = mEyePos - wPos.xyz;	// 視線ベクトル
	o.Eye.x = dot( Eye, T.xyz );
	o.Eye.y = dot( Eye, B );
	o.Eye.z = dot( Eye, N.xyz );


	float Lrate;
	float3 L;
	//ライト
	if( mBumpLight[0][0].y == 0 ){
		L = -mBumpLight[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		L = mBumpLight[0][4].xyz - wPos.xyz;	
		Lrate = 1.0f - dot( L, L ) * mBumpLight[0][0].z;
		Lrate = clamp( Lrate, 0.0f, 1.0f );
	}
	o.L.x = dot( L, T.xyz );
	o.L.y = dot( L, B );
	o.L.z = dot( L, N.xyz );

	o.Diffuse.xyz = i.Diffuse.xyz * Lrate;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz * Lrate;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.Power = i.Power;
	o.Emissive = i.Emissive;

	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

    return o;
}

// PPL vertex shader
VSPPL1_OUTPUT SkinB4PPL1(VS_INPUT_TIMA i)
{
    VSPPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float4 Normal;	
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}


	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = Normal.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSPPL2_OUTPUT SkinB4PPL2(VS_INPUT_TIMA i)
{
    VSPPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float4 Normal;	
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;

	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = Normal.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPL3_OUTPUT SkinB4PPL3(VS_INPUT_TIMA i)
{
    VSPPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float4 Normal;	
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mLightParams[2][0].y == 0 ){
		L3 = -mLightParams[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L3 = mLightParams[2][4].xyz - wPos.xyz;
		mag = dot( L3, L3 );
		L3 = normalize( L3 );
		dot2 = dot( mLightParams[2][5].xyz, -L3 );
		rate1 = clamp( 1.0f - mag * mLightParams[2][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[2][1].x ) * mLightParams[2][1].y, 0.0f, 1.0f );
		Lrate3 = rate1 * rate2;
	}
	o.L3.xyz = L3;
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = Normal.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPL1_OUTPUT SkinB4BlurWorldPPL1(VS_INPUT_TIMA i)
{
    VSPPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSPPL2_OUTPUT SkinB4BlurWorldPPL2(VS_INPUT_TIMA i)
{
    VSPPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

//////////
	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}
VSPPL3_OUTPUT SkinB4BlurWorldPPL3(VS_INPUT_TIMA i)
{
    VSPPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}


	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mLightParams[2][0].y == 0 ){
		L3 = -mLightParams[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L3 = mLightParams[2][4].xyz - wPos.xyz;
		mag = dot( L3, L3 );
		L3 = normalize( L3 );
		dot2 = dot( mLightParams[2][5].xyz, -L3 );
		rate1 = clamp( 1.0f - mag * mLightParams[2][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[2][1].x ) * mLightParams[2][1].y, 0.0f, 1.0f );
		Lrate3 = rate1 * rate2;
	}
	o.L3.xyz = L3;
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPL1_OUTPUT SkinB4BlurCameraPPL1(VS_INPUT_TIMA i)
{
    VSPPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSPPL2_OUTPUT SkinB4BlurCameraPPL2(VS_INPUT_TIMA i)
{
    VSPPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

//////////
	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}
VSPPL3_OUTPUT SkinB4BlurCameraPPL3(VS_INPUT_TIMA i)
{
    VSPPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = 0;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.x + 20] * i.BlendWeights.x;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.y + 20] * i.BlendWeights.y;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.z + 20] * i.BlendWeights.z;
	finalmat0 += mWorldMatrixArray[i.BlendIndices.w + 20] * i.BlendWeights.w;

	float4x4 finalmat1 = 0;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat1 += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mLightParams[2][0].y == 0 ){
		L3 = -mLightParams[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L3 = mLightParams[2][4].xyz - wPos.xyz;
		mag = dot( L3, L3 );
		L3 = normalize( L3 );
		dot2 = dot( mLightParams[2][5].xyz, -L3 );
		rate1 = clamp( 1.0f - mag * mLightParams[2][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[2][1].x ) * mLightParams[2][1].y, 0.0f, 1.0f );
		Lrate3 = rate1 * rate2;
	}
	o.L3.xyz = L3;
	o.L3.w = Lrate3;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPL1_OUTPUT SkinB0PPL1(VS_INPUT_TIMA i)
{
    VSPPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float4 Normal;	
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	// 鏡面反射用のベクトル
	//o.Eye = mEyePos - wPos.xyz;	// 視線ベクトル

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = Normal.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSPPL2_OUTPUT SkinB0PPL2(VS_INPUT_TIMA i)
{
    VSPPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float4 Normal;	
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

//////////
	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;


	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = Normal.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPL3_OUTPUT SkinB0PPL3(VS_INPUT_TIMA i)
{
    VSPPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	float4 Normal;	
    Normal = mul(i.Normal, finalmat );
    Normal.w = 0.0f;
    Normal = normalize(Normal);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mLightParams[2][0].y == 0 ){
		L3 = -mLightParams[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L3 = mLightParams[2][4].xyz - wPos.xyz;
		mag = dot( L3, L3 );
		L3 = normalize( L3 );
		dot2 = dot( mLightParams[2][5].xyz, -L3 );
		rate1 = clamp( 1.0f - mag * mLightParams[2][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[2][1].x ) * mLightParams[2][1].y, 0.0f, 1.0f );
		Lrate3 = rate1 * rate2;
	}
	o.L3.xyz = L3;
	o.L3.w = Lrate3;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = Normal.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPL1_OUTPUT SkinB0BlurWorldPPL1(VS_INPUT_TIMA i)
{
    VSPPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSPPL2_OUTPUT SkinB0BlurWorldPPL2(VS_INPUT_TIMA i)
{
    VSPPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

//////////
	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}
VSPPL3_OUTPUT SkinB0BlurWorldPPL3(VS_INPUT_TIMA i)
{
    VSPPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];

	
    // transform position from world space into view and then projection space

	float4 pos0;
	float4 pos1;
	pos0 = mul( i.Pos, finalmat0 );
	pos1 = mul( i.Pos, finalmat1 );
    float4 N = mul(i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 vel;
	vel = pos1 - pos0;
	float dotvel;
	dotvel = dot( N.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = pos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = pos0;
		o.Diffuse.w = mMinAlpha.x;
	}
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mLightParams[2][0].y == 0 ){
		L3 = -mLightParams[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L3 = mLightParams[2][4].xyz - wPos.xyz;
		mag = dot( L3, L3 );
		L3 = normalize( L3 );
		dot2 = dot( mLightParams[2][5].xyz, -L3 );
		rate1 = clamp( 1.0f - mag * mLightParams[2][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[2][1].x ) * mLightParams[2][1].y, 0.0f, 1.0f );
		Lrate3 = rate1 * rate2;
	}
	o.L3.xyz = L3;
	o.L3.w = Lrate3;


	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPL1_OUTPUT SkinB0BlurCameraPPL1(VS_INPUT_TIMA i)
{
    VSPPL1_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

    return o;
}
VSPPL2_OUTPUT SkinB0BlurCameraPPL2(VS_INPUT_TIMA i)
{
    VSPPL2_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

//////////
	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}
VSPPL3_OUTPUT SkinB0BlurCameraPPL3(VS_INPUT_TIMA i)
{
    VSPPL3_OUTPUT   o;
    float4      wPos, vPos;

	float4x4 finalmat0 = mWorldMatrixArray[20];
	float4x4 finalmat1 = mWorldMatrixArray[0];

	float4 wpos0, wpos1, vpos0, vpos1;
	wpos0 = mul( i.Pos, finalmat0 );
	wpos1 = mul( i.Pos, finalmat1 );
	vpos0 = mul( wpos0, moldView );
	vpos1 = mul( wpos1, mView );
	
    float4 N = mul( i.Normal, finalmat1 );
    N.w = 0.0f;
    N = normalize(N);

	float4 NView = mul( N, mView );
	NView.w = 0.0f;
	NView = normalize( NView );

	float4 vel;
	vel = vpos1 - vpos0;
	float dotvel;
	dotvel = dot( NView.xyz, vel.xyz );
	if( dotvel > 0.0f ){
		wPos = wpos1;
		vPos = vpos1;
		o.Diffuse.w = mAlpha * mMinAlpha.y;
	}else{
		wPos = wpos0;
		vPos = vpos0;
		o.Diffuse.w = mMinAlpha.x;
	}
    o.Pos = mul(vPos, mProj);

	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	o.wPos = wPos.xyz / wPos.w * 0.000001f;
	o.wEye = mEyePos * 0.000001f;

	float3 L1;
	float Lrate1;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L1 = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate1 = 1.0f;
	}else{
		L1 = mLightParams[0][4].xyz * 0.000001f;
		Lrate1 = -1.0f;
	}
	o.L1.xyz = L1;
	o.L1.w = Lrate1;


	float3 L2;
	float Lrate2;
	//ライト
	if( mLightParams[1][0].y == 0 ){
		L2 = -mLightParams[1][5].xyz;		// ライトベクトル	
		Lrate2 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L2 = mLightParams[1][4].xyz - wPos.xyz;
		mag = dot( L2, L2 );
		L2 = normalize( L2 );
		dot2 = dot( mLightParams[1][5].xyz, -L2 );
		rate1 = clamp( 1.0f - mag * mLightParams[1][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[1][1].x ) * mLightParams[1][1].y, 0.0f, 1.0f );
		Lrate2 = rate1 * rate2;
	}
	o.L2.xyz = L2;
	o.L2.w = Lrate2;


	float3 L3;
	float Lrate3;
	//ライト
	if( mLightParams[2][0].y == 0 ){
		L3 = -mLightParams[2][5].xyz;		// ライトベクトル	
		Lrate3 = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L3 = mLightParams[2][4].xyz - wPos.xyz;
		mag = dot( L3, L3 );
		L3 = normalize( L3 );
		dot2 = dot( mLightParams[2][5].xyz, -L3 );
		rate1 = clamp( 1.0f - mag * mLightParams[2][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[2][1].x ) * mLightParams[2][1].y, 0.0f, 1.0f );
		Lrate3 = rate1 * rate2;
	}
	o.L3.xyz = L3;
	o.L3.w = Lrate3;

	o.Diffuse.xyz = i.Diffuse.xyz;
//	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.AmbEmi = i.Ambient + i.Emissive;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;

    return o;
}

VSPPSHADOW_OUTPUT1 SkinB0PPShadow1(VS_INPUT_TIMA i)
{
    VSPPSHADOW_OUTPUT1   o;
    float4      wPos, vPos;

	float4x4 finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );

	// 鏡面反射用のベクトル
	o.Eye = mEyePos - wPos.xyz;	// 視線ベクトル

	float Lrate;
	float3 L;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L = mLightParams[0][4].xyz - wPos.xyz;
		mag = dot( L, L );
		L = normalize( L );
		dot2 = dot( mLightParams[0][5].xyz, L );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate = rate1 * rate2;
	}
	o.L.xyz = L;
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

    return o;
}
VSPPSHADOW_OUTPUT1 SkinB4PPShadow1(VS_INPUT_TIMA i)
{
    VSPPSHADOW_OUTPUT1   o;
    float4      wPos, vPos;

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
    o.Pos = mul(vPos, mProj);
	
    // copy the input texture coordinate through
    o.Tex0  = i.Tex0.xy;

	if( mFogParams.z == 0.0f ){
		o.Fog.x = ( mFogParams.y - vPos.z / vPos.w ) / ( mFogParams.y - mFogParams.x );
	}else{
		o.Fog.x = ( wPos.y - mFogParams.x ) / ( mFogParams.y - mFogParams.x );
	}

	// 座標系の変換基底
	float4 N = mul( i.Normal, finalmat );
	N.w = 0.0f;
	normalize( N );

	// 鏡面反射用のベクトル
	o.Eye = mEyePos - wPos.xyz;	// 視線ベクトル

	float Lrate;
	float3 L;
	//ライト
	if( mLightParams[0][0].y == 0 ){
		L = -mLightParams[0][5].xyz;		// ライトベクトル	
		Lrate = 1.0f;
	}else{
		float dot2, mag, rate1, rate2;
		L = mLightParams[0][4].xyz - wPos.xyz;
		mag = dot( L, L );
		L = normalize( L );
		dot2 = dot( mLightParams[0][5].xyz, L );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate = rate1 * rate2;
	}
	o.L.xyz = L;
	o.L.w = Lrate;

	o.Diffuse.xyz = i.Diffuse.xyz;
	o.Diffuse.w = mAlpha;
	o.Specular.xyz = i.Specular.xyz;
	o.Specular.w = 0.0f;
	o.Ambient = i.Ambient;
	o.NandPow.xyz = N.xyz;
	o.NandPow.w = i.Power;
	o.Emissive = i.Emissive;

	o.ShadowMapUV = mul( wPos, mLPB );
	o.Depth = mul( wPos, mLP );

    return o;
}

/***
struct VSZN_OUTPUT
{
	float4	Pos		: POSITION;
	float4	PSPos	: TEXCOORD0;
	float4	PSNormal	: TEXCOORD1;
};
***/

VSZN_OUTPUT VSZN_NormalB4( VS_INPUT_TIMA i )
{
	VSZN_OUTPUT o;
	
    float4      wPos, vPos, pPos;
    float4      wN, rN;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
	pPos = mul(vPos, mProj);
    o.Pos = pPos;
	o.PSPos = pPos;

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    i.Normal.w = 0.0f;
    wN = mul(i.Normal, finalmat );
//	o.PSNormal = rN;
//    float4 off = { 0.5f, 0.5f, 0.5f, 0.0f };
//    rN = wN * 0.5f + off;	
//	o.PSNormal = rN;
	o.PSNormal = wN;
	
	return o;
}

VSZN_OUTPUT VSZN_NormalB0( VS_INPUT_TIMA i )
{
	VSZN_OUTPUT o;
	
    float4      wPos, vPos, pPos;
    float4      wN, rN;    

	float4x4 finalmat;
	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
	pPos = mul(vPos, mProj);
    o.Pos = pPos;
	o.PSPos = pPos;

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    i.Normal.w = 0.0f;
    wN = mul(i.Normal, finalmat );
//	o.PSNormal = rN;
//    float4 off = { 0.5f, 0.5f, 0.5f, 0.0f };
//    rN = wN * 0.5f + off;	
//	o.PSNormal = rN;
	o.PSNormal = wN;

	
	return o;
}

VSZN_OUTPUT VSZN_NormalCamB4( VS_INPUT_TIMA i )
{
	VSZN_OUTPUT o;
	
    float4      wPos, vPos, pPos;
    float4      wN, vN;    

	float4x4 finalmat = 0;
	finalmat += mWorldMatrixArray[i.BlendIndices.x] * i.BlendWeights.x;
	finalmat += mWorldMatrixArray[i.BlendIndices.y] * i.BlendWeights.y;
	finalmat += mWorldMatrixArray[i.BlendIndices.z] * i.BlendWeights.z;
	finalmat += mWorldMatrixArray[i.BlendIndices.w] * i.BlendWeights.w;

//	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
	pPos = mul(vPos, mProj);
    o.Pos = pPos;
	o.PSPos = pPos;

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
    
	i.Normal.w = 0.0f;	
    wN = mul(i.Normal, finalmat );
	vN = mul(wN, mView );
	o.PSNormal = vN;
	
	return o;
}

VSZN_OUTPUT VSZN_NormalCamB0( VS_INPUT_TIMA i )
{
	VSZN_OUTPUT o;
	
    float4      wPos, vPos, pPos;
    float4      wN, vN;    

	float4x4 finalmat;
	finalmat = mWorldMatrixArray[0];
	
    // transform position from world space into view and then projection space
	wPos = mul(i.Pos, finalmat );
	vPos = mul(wPos, mView );
	pPos = mul(vPos, mProj);
    o.Pos = pPos;
	o.PSPos = pPos;

    // normalize normals
    //finalmat._41 = 0.0f; finalmat._42 = 0.0f; finalmat._43 = 0.0f;
	i.Normal.w = 0.0f;	
    wN = mul(i.Normal, finalmat );
	vN = mul(wN, mView );
	o.PSNormal = vN;
	
	return o;
}



//----------------------------------------------------------------
//----------------------------------------------------------------
//						pixcel shader
//----------------------------------------------------------------
//----------------------------------------------------------------


PS_OUTPUT PSToon0Tex( VSTOON_OUTPUT i )
{
	PS_OUTPUT Output;

	float th1 = mToonParams.z;
	float th2 = mToonParams.w;

	float4 texcol0;
	texcol0 = tex2D( DecaleSamp, i.Tex0 );
	

	float3 tempcolor;
	if( i.NL.x > th2 ){
		tempcolor = i.Diffuse3.xyz;
	}else{
		if( i.NL.x > th1 ){
			tempcolor = i.Diffuse.xyz;
		}else{
			tempcolor = i.Diffuse2.xyz;
		}
	}

	Output.Color.xyz = texcol0.xyz * tempcolor;
	Output.Color.w = texcol0.w * i.Diffuse.w;

	return Output;
}
PS_OUTPUT PSToon0NoTex( VSTOON_OUTPUT i )
{
	PS_OUTPUT Output;

	float th1 = mToonParams.z;
	float th2 = mToonParams.w;


	float3 tempcolor;
	if( i.NL.x > th2 ){
		tempcolor = i.Diffuse3.xyz;
	}else{
		if( i.NL.x > th1 ){
			tempcolor = i.Diffuse.xyz;
		}else{
			tempcolor = i.Diffuse2.xyz;
		}
	}

	Output.Color.xyz = tempcolor;
	Output.Color.w = i.Diffuse.w;

	return Output;
}

PS_OUTPUT PSBumpL1( VSBUMPL1_OUTPUT In )
{
	PS_OUTPUT Output;
		
	float Lrate = In.L.w;
		
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL = normalize(In.L.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
			
	float psnl;
	psnl = dot( PSN, PSL );

	float4 col0;

	col0.xyz = max( 0, psnl ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate
				+ mBumpLight[0][3].xyz * In.Specular.xyz * Lrate * pow(max(0,dot(PSR, PSL)), In.Power)
				+ In.Ambient.xyz + In.Emissive.xyz;



	col0.w = In.Diffuse.w;
	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	

	return Output;
}
PS_OUTPUT PSBumpL2( VSBUMPL2_OUTPUT In )
{
	PS_OUTPUT Output;
	
	float Lrate1 = In.L1.w;
	float Lrate2 = In.L2.w;
	
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL1 = normalize(In.L1.xyz);						// ライトベクトル
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
			
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float psnl2;
	psnl2 = dot( PSN, PSL2 );

	float4 col0;

	col0.xyz = max( 0, psnl1 ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mBumpLight[1][2].xyz * In.Diffuse.xyz * Lrate2
		     + mBumpLight[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.Power)
		     + mBumpLight[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.Power)
			 + In.Ambient.xyz + In.Emissive.xyz;

	col0.w = In.Diffuse.w;
	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	

	return Output;
}
PS_OUTPUT PSBumpL3( VSBUMPL3_OUTPUT In )
{
	PS_OUTPUT Output;
	
	float Lrate1 = In.L1.w;
	float Lrate2 = In.L2.w;
	float Lrate3 = In.L3.w;
	
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL1 = normalize(In.L1.xyz);						// ライトベクトル
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSL3 = normalize(In.L3.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
			
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float psnl2;
	psnl2 = dot( PSN, PSL2 );

	float psnl3;
	psnl3 = dot( PSN, PSL3 );


	float4 col0;

	col0.xyz = max( 0, psnl1 ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mBumpLight[1][2].xyz * In.Diffuse.xyz * Lrate2
	         + max( 0, psnl3 ) * mBumpLight[2][2].xyz * In.Diffuse.xyz * Lrate3
		     + mBumpLight[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.Power)
		     + mBumpLight[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.Power)
		     + mBumpLight[2][3].xyz * In.Specular.xyz * Lrate3 * pow(max(0,dot(PSR, PSL3)), In.Power)
			 + In.Ambient.xyz + In.Emissive.xyz;

	col0.w = In.Diffuse.w;
	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	

	return Output;
}


PS_OUTPUT PSNormalShadowTex0( VSSHADOW_OUTPUT0 i )
{
	PS_OUTPUT Output;

	float4 decale = tex2D( DecaleSamp, i.Tex0 );
	
	Output.Color = i.ShadowMapUV.z / i.ShadowMapUV.w;
	Output.Color.w = mAlpha * decale.w;
	
	return Output;
}
PS_OUTPUT PSNormalShadowNoTex0( VSSHADOW_OUTPUT0 i )
{
	PS_OUTPUT Output;

	Output.Color = i.ShadowMapUV.z / i.ShadowMapUV.w;
	Output.Color.w = mAlpha;

	return Output;
}


PS_OUTPUT PSNormalShadowTex1( VSSHADOW_OUTPUT1 i )
{
	PS_OUTPUT Output;

	float2 smap;
	smap.x = i.ShadowMapUV.x / i.ShadowMapUV.w;
	smap.y = i.ShadowMapUV.y / i.ShadowMapUV.w;
	
	float4 shadow = tex2D( ShadowMapSamp, smap );
	float4 decale = tex2D( DecaleSamp, i.Tex0 );
		

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (i.Depth.z / i.Depth.w) > (shadow.x + mShadowCoef.x) ){
			Output.Color.xyz = decale.xyz * i.Ambient.xyz * mShadowCoef.y;
		}else{
			Output.Color.xyz = decale.xyz * ( i.Ambient.xyz + i.Diffuse.xyz + i.Specular.xyz );
		}
	}else{
		Output.Color.xyz = decale.xyz * ( i.Ambient.xyz + i.Diffuse.xyz + i.Specular.xyz );	
	}
	Output.Color.w = i.Diffuse.w * decale.w;

	return Output;
}

PS_OUTPUT PSNormalShadowNoTex1( VSSHADOW_OUTPUT1 i )
{
	PS_OUTPUT Output;

	float2 smap;
	smap.x = i.ShadowMapUV.x / i.ShadowMapUV.w;
	smap.y = i.ShadowMapUV.y / i.ShadowMapUV.w;

	float4 shadow = tex2D( ShadowMapSamp, smap );

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (i.Depth.z / i.Depth.w) > (shadow.x + mShadowCoef.x) ){
			Output.Color.xyz = i.Ambient.xyz * mShadowCoef.y;
		}else{
			Output.Color.xyz = i.Ambient.xyz + i.Diffuse.xyz + i.Specular.xyz;
		}
	}else{
		Output.Color.xyz = i.Ambient.xyz + i.Diffuse.xyz + i.Specular.xyz;	
	}

	Output.Color.w = i.Diffuse.w;

	return Output;
}

PS_OUTPUT PSBumpShadow1( VSBUMPSHADOW_OUTPUT1 In )
{
	PS_OUTPUT Output;

	float2 smap;
	smap.x = In.ShadowMapUV.x / In.ShadowMapUV.w;
	smap.y = In.ShadowMapUV.y / In.ShadowMapUV.w;
		
	float4 shadow = tex2D( ShadowMapSamp, smap );

	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL = normalize(In.L);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
			
	float psnl;
	psnl = dot( PSN, PSL );

	float4 col0;

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (In.Depth.z / In.Depth.w) > (shadow.x + mShadowCoef.x) ){
			col0.xyz = In.Ambient.xyz * mShadowCoef.y;	
		}else{
			col0.xyz = max( 0, psnl ) * mBumpLight[0][2].xyz * In.Diffuse.xyz
						+ mBumpLight[0][3].xyz * In.Specular.xyz * pow(max(0,dot(PSR, PSL)), In.Power)
						+ In.Ambient.xyz + In.Emissive.xyz;
		}
	}else{
		col0.xyz = max( 0, psnl ) * mBumpLight[0][2].xyz * In.Diffuse.xyz
					+ mBumpLight[0][3].xyz * In.Specular.xyz * pow(max(0,dot(PSR, PSL)), In.Power)
					+ In.Ambient.xyz + In.Emissive.xyz;	
	}
	col0.w = In.Diffuse.w;
	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	

	return Output;
}


PS_OUTPUT PSToon1ShadowTex1( VSTOON1SHADOW_OUTPUT1 i )
{
	PS_OUTPUT Output;

	float2 smap;
	smap.x = i.ShadowMapUV.x / i.ShadowMapUV.w;
	smap.y = i.ShadowMapUV.y / i.ShadowMapUV.w;
	
	float4 shadow = tex2D( ShadowMapSamp, smap );
	float4 decale1 = tex2D( DecaleSamp1, i.Tex1 );
		
	float2 tex0;

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (i.Depth.z / i.Depth.w) > (shadow.x + mShadowCoef.x) ){
			//影
			tex0.x = 0.5f;
			tex0.y = 0.15f;
		}else{
			tex0 = i.Tex0;
		}
	}else{
		tex0 = i.Tex0;
	}
	
	float4 decale0 = tex2D( DecaleSamp, tex0 );
	
	Output.Color.xyz = decale1.xyz * decale0.xyz;//i.Diffuseはｒｇｂ１．０
	Output.Color.w = i.Diffuse.w * decale1.w;

	return Output;
}

PS_OUTPUT PSToon1ShadowNoTex1( VSTOON1SHADOW_OUTPUT1 i )
{
	PS_OUTPUT Output;

	float2 smap;
	smap.x = i.ShadowMapUV.x / i.ShadowMapUV.w;
	smap.y = i.ShadowMapUV.y / i.ShadowMapUV.w;
	
	float4 shadow = tex2D( ShadowMapSamp, smap );		
	float2 tex0;

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (i.Depth.z / i.Depth.w) > (shadow.x + mShadowCoef.x) ){
			//影
			tex0.x = 0.5f;
			tex0.y = 0.15f;
		}else{
			tex0 = i.Tex0;
		}
	}else{
		tex0 = i.Tex0;
	}
	
	float4 decale0 = tex2D( DecaleSamp, tex0 );
	
	Output.Color.xyz = decale0.xyz;//i.Diffuseはｒｇｂ１．０
	Output.Color.w = i.Diffuse.w;

	return Output;
}

PS_OUTPUT PSToon0ShadowTex1( VSTOONSHADOW_OUTPUT1 i )
{
	PS_OUTPUT Output;

	float th1 = mToonParams.z;
	float th2 = mToonParams.w;

	float4 texcol0;
	texcol0 = tex2D( DecaleSamp, i.Tex0 );

	float2 smap;
	smap.x = i.ShadowMapUV.x / i.ShadowMapUV.w;
	smap.y = i.ShadowMapUV.y / i.ShadowMapUV.w;
	
	float4 shadow = tex2D( ShadowMapSamp, smap );		
	float3 tempcolor;

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (i.Depth.z / i.Depth.w) > (shadow.x + mShadowCoef.x) ){
			//影
			tempcolor = i.Diffuse2.xyz;	
		}else{
			if( i.NL.x > th2 ){
				tempcolor = i.Diffuse3.xyz;
			}else{
				if( i.NL.x > th1 ){
					tempcolor = i.Diffuse.xyz;
				}else{
					tempcolor = i.Diffuse2.xyz;
				}
			}			
		}
	}else{
		if( i.NL.x > th2 ){
			tempcolor = i.Diffuse3.xyz;
		}else{
			if( i.NL.x > th1 ){
				tempcolor = i.Diffuse.xyz;
			}else{
				tempcolor = i.Diffuse2.xyz;
			}
		}	
	}

	Output.Color.xyz = texcol0.xyz * tempcolor;
	Output.Color.w = texcol0.w * i.Diffuse.w;

	return Output;
}
PS_OUTPUT PSToon0ShadowNoTex1( VSTOONSHADOW_OUTPUT1 i )
{
	PS_OUTPUT Output;

	float th1 = mToonParams.z;
	float th2 = mToonParams.w;

	float2 smap;
	smap.x = i.ShadowMapUV.x / i.ShadowMapUV.w;
	smap.y = i.ShadowMapUV.y / i.ShadowMapUV.w;
	
	float4 shadow = tex2D( ShadowMapSamp, smap );		
	float3 tempcolor;

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (i.Depth.z / i.Depth.w) > (shadow.x + mShadowCoef.x) ){
			//影
			tempcolor = i.Diffuse2.xyz;	
		}else{
			if( i.NL.x > th2 ){
				tempcolor = i.Diffuse3.xyz;
			}else{
				if( i.NL.x > th1 ){
					tempcolor = i.Diffuse.xyz;
				}else{
					tempcolor = i.Diffuse2.xyz;
				}
			}			
		}
	}else{
		if( i.NL.x > th2 ){
			tempcolor = i.Diffuse3.xyz;
		}else{
			if( i.NL.x > th1 ){
				tempcolor = i.Diffuse.xyz;
			}else{
				tempcolor = i.Diffuse2.xyz;
			}
		}	
	}

	Output.Color.xyz = tempcolor;
	Output.Color.w = i.Diffuse.w;

	return Output;
}

PS_OUTPUT PSSkinGlow( VS_OUTPUT i )
{
	PS_OUTPUT Output;

	float4 texcolor = tex2D( DecaleSamp, i.Tex0 );
	float4 vcolor = i.Diffuse + i.Specular;
	
	Output.Color.xyz = vcolor.xyz * texcolor.xyz * texcolor.w * mGlowMult;
	Output.Color.w = i.Diffuse.w;

	return Output;
}
PS_OUTPUT PSSkinGlowAlpha( VS_OUTPUT i )
{
	PS_OUTPUT Output;

	float4 texcolor = tex2D( DecaleSamp, i.Tex0 );
	float4 vcolor = i.Diffuse + i.Specular;
	
	Output.Color.xyz = vcolor.xyz * texcolor.xyz * texcolor.w * mGlowMult;
	Output.Color.w = i.Diffuse.w * texcolor.w;

	return Output;
}


PS_OUTPUT PSToonGlow( VSTOON_OUTPUT i )
{
	PS_OUTPUT Output;

	float th1 = mToonParams.z;
	float th2 = mToonParams.w;

	float4 texcol0;
	texcol0 = tex2D( DecaleSamp, i.Tex0 );
	

	float3 tempcolor;
	if( i.NL.x > th2 ){
		tempcolor = i.Diffuse3.xyz;
	}else{
		if( i.NL.x > th1 ){
			tempcolor = i.Diffuse.xyz;
		}else{
			tempcolor = i.Diffuse2.xyz;
		}
	}

	Output.Color.xyz = texcol0.xyz * tempcolor * texcol0.w * mGlowMult;
	Output.Color.w = i.Diffuse.w;

	return Output;
}
PS_OUTPUT PSToonGlowAlpha( VSTOON_OUTPUT i )
{
	PS_OUTPUT Output;

	float th1 = mToonParams.z;
	float th2 = mToonParams.w;

	float4 texcol0;
	texcol0 = tex2D( DecaleSamp, i.Tex0 );
	

	float3 tempcolor;
	if( i.NL.x > th2 ){
		tempcolor = i.Diffuse3.xyz;
	}else{
		if( i.NL.x > th1 ){
			tempcolor = i.Diffuse.xyz;
		}else{
			tempcolor = i.Diffuse2.xyz;
		}
	}

	Output.Color.xyz = texcol0.xyz * tempcolor * texcol0.w * mGlowMult;
	Output.Color.w = texcol0.w * i.Diffuse.w;

	return Output;
}


PS_OUTPUT PSSkinToon1Glow( VSTOON1_OUTPUT i )
{
	PS_OUTPUT Output;

	float4 texcol0 = tex2D( DecaleSamp, i.Tex0 );//グラデ用
	float4 texcol1 = tex2D( DecaleSamp1, i.Tex1 );//glow
	
	Output.Color.xyz = i.Diffuse.xyz * texcol0.xyz * texcol1.xyz * texcol1.w * mGlowMult;
	Output.Color.w = i.Diffuse.w;

	return Output;
}
PS_OUTPUT PSSkinToon1GlowAlpha( VSTOON1_OUTPUT i )
{
	PS_OUTPUT Output;

	float4 texcol0 = tex2D( DecaleSamp, i.Tex0 );//グラデ用
	float4 texcol1 = tex2D( DecaleSamp1, i.Tex1 );//glow
	
	Output.Color.xyz = i.Diffuse.xyz * texcol0.xyz * texcol1.xyz * texcol1.w * mGlowMult;
	Output.Color.w = i.Diffuse.w * texcol1.w;

	return Output;
}


PS_OUTPUT PSBumpL1Glow( VSBUMPL1_OUTPUT In )
{
	PS_OUTPUT Output;
		
	float Lrate = In.L.w;
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL = normalize(In.L.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
			
	float psnl;
	psnl = dot( PSN, PSL );

	float4 col0;

	col0.xyz = max( 0, psnl ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate
				+ mBumpLight[0][3].xyz * In.Specular.xyz * Lrate * pow(max(0,dot(PSR, PSL)), In.Power)
				+ In.Ambient.xyz + In.Emissive.xyz;
	
	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w;


	return Output;
}
PS_OUTPUT PSBumpL2Glow( VSBUMPL2_OUTPUT In )
{
	PS_OUTPUT Output;
	
	float Lrate1 = In.L1.w;
	float Lrate2 = In.L2.w;
	
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL1 = normalize(In.L1.xyz);						// ライトベクトル
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
			
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float psnl2;
	psnl2 = dot( PSN, PSL2 );

	float4 col0;

	col0.xyz = max( 0, psnl1 ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mBumpLight[1][2].xyz * In.Diffuse.xyz * Lrate2
		     + mBumpLight[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.Power)
		     + mBumpLight[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.Power)
			 + In.Ambient.xyz + In.Emissive.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w;

	return Output;
}
PS_OUTPUT PSBumpL3Glow( VSBUMPL3_OUTPUT In )
{
	PS_OUTPUT Output;
	
	float Lrate1 = In.L1.w;
	float Lrate2 = In.L2.w;
	float Lrate3 = In.L3.w;
	
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL1 = normalize(In.L1.xyz);						// ライトベクトル
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSL3 = normalize(In.L3.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
			
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float psnl2;
	psnl2 = dot( PSN, PSL2 );

	float psnl3;
	psnl3 = dot( PSN, PSL3 );


	float4 col0;

	col0.xyz = max( 0, psnl1 ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mBumpLight[1][2].xyz * In.Diffuse.xyz * Lrate2
	         + max( 0, psnl3 ) * mBumpLight[2][2].xyz * In.Diffuse.xyz * Lrate3
		     + mBumpLight[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.Power)
		     + mBumpLight[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.Power)
		     + mBumpLight[2][3].xyz * In.Specular.xyz * Lrate3 * pow(max(0,dot(PSR, PSL3)), In.Power)
			 + In.Ambient.xyz + In.Emissive.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w;

	return Output;
}

PS_OUTPUT PSBumpL1GlowAlpha( VSBUMPL1_OUTPUT In )
{
	PS_OUTPUT Output;
		
	float Lrate = In.L.w;
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL = normalize(In.L.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
			
	float psnl;
	psnl = dot( PSN, PSL );

	float4 col0;

	col0.xyz = max( 0, psnl ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate
				+ mBumpLight[0][3].xyz * In.Specular.xyz * Lrate * pow(max(0,dot(PSR, PSL)), In.Power)
				+ In.Ambient.xyz + In.Emissive.xyz;
	
	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w * texcol.w;


	return Output;
}
PS_OUTPUT PSBumpL2GlowAlpha( VSBUMPL2_OUTPUT In )
{
	PS_OUTPUT Output;
	
	float Lrate1 = In.L1.w;
	float Lrate2 = In.L2.w;
	
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL1 = normalize(In.L1.xyz);						// ライトベクトル
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
			
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float psnl2;
	psnl2 = dot( PSN, PSL2 );

	float4 col0;

	col0.xyz = max( 0, psnl1 ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mBumpLight[1][2].xyz * In.Diffuse.xyz * Lrate2
		     + mBumpLight[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.Power)
		     + mBumpLight[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.Power)
			 + In.Ambient.xyz + In.Emissive.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w * texcol.w;

	return Output;
}
PS_OUTPUT PSBumpL3GlowAlpha( VSBUMPL3_OUTPUT In )
{
	PS_OUTPUT Output;
	
	float Lrate1 = In.L1.w;
	float Lrate2 = In.L2.w;
	float Lrate3 = In.L3.w;
	
	float3 PSN = 2.0f*tex2D( NormalSamp, In.Tex0 ).xyz - 1.0f;// 法線マップからの法線
	float3 PSL1 = normalize(In.L1.xyz);						// ライトベクトル
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSL3 = normalize(In.L3.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	//float3 PSR = reflect(-normalize(PSE), PSN);		// 反射ベクトル
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
			
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float psnl2;
	psnl2 = dot( PSN, PSL2 );

	float psnl3;
	psnl3 = dot( PSN, PSL3 );


	float3 col0;

	col0.xyz = max( 0, psnl1 ) * mBumpLight[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mBumpLight[1][2].xyz * In.Diffuse.xyz * Lrate2
	         + max( 0, psnl3 ) * mBumpLight[2][2].xyz * In.Diffuse.xyz * Lrate3
		     + mBumpLight[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.Power)
		     + mBumpLight[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.Power)
		     + mBumpLight[2][3].xyz * In.Specular.xyz * Lrate3 * pow(max(0,dot(PSR, PSL3)), In.Power)
			 + In.Ambient.xyz + In.Emissive.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w * texcol.w;

	return Output;
}

PS_OUTPUT PSSkinPPL1( VSPPL1_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1;
	psnl1 = dot( PSN, PSL1 );


	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
			 + In.Ambient.xyz + In.Emissive.xyz;

	col0.w = In.Diffuse.w;	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	
	return Output;
}
PS_OUTPUT PSSkinPPL1NoTex( VSPPL1_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
			 + In.Ambient.xyz + In.Emissive.xyz;

	col0.w = In.Diffuse.w;	
	Output.Color = col0;	
	return Output;
}
PS_OUTPUT PSSkinPPL2( VSPPL2_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSL2;
	float Lrate2;
	PSL2 = normalize( In.L2.xyz );
	Lrate2 = In.L2.w;


	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1 = dot( PSN, PSL1 );
	float psnl2 = dot( PSN, PSL2 );

//	float3 nh1 = dot( PSN, ( PSL1 + PSE ) * 0.5f );
//	float3 nh2 = dot( PSN, ( PSL2 + PSE ) * 0.5f );
//	(psnl1 < 0) || (nh1 < 0) ? 0.0f : ( nh1 * In.NandPow.w );

	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
		     //+ mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * max(0,dot(PSR, PSL2))//max(0,nh2) * In.NandPow.w * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	col0.w = In.Diffuse.w;	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	
	return Output;
}
PS_OUTPUT PSSkinPPL2NoTex( VSPPL2_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSL2;
	float Lrate2;
	PSL2 = normalize( In.L2.xyz );
	Lrate2 = In.L2.w;

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1 = dot( PSN, PSL1 );
	float psnl2 = dot( PSN, PSL2 );

//	float3 nh1 = dot( PSN, ( PSL1 + PSE ) * 0.5f );
//	float3 nh2 = dot( PSN, ( PSL2 + PSE ) * 0.5f );
//	(psnl1 < 0) || (nh1 < 0) ? 0.0f : ( nh1 * In.NandPow.w );

	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
		     //+ mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * max(0,dot(PSR, PSL2))//max(0,nh2) * In.NandPow.w * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	col0.w = In.Diffuse.w;	
	Output.Color = col0;	
	return Output;
}
PS_OUTPUT PSSkinPPL3( VSPPL3_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	
	float3 PSN = normalize(In.NandPow.xyz);

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float Lrate2 = In.L2.w;
	float Lrate3 = In.L3.w;
	
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSL3 = normalize(In.L3.xyz);						// ライトベクトル

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1;
	psnl1 = dot( PSN, PSL1 );
	float psnl2;
	psnl2 = dot( PSN, PSL2 );
	float psnl3;
	psnl3 = dot( PSN, PSL3 );


	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
	         + max( 0, psnl3 ) * mLightParams[2][2].xyz * In.Diffuse.xyz * Lrate3
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
//		     + mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
//		     + mLightParams[2][3].xyz * In.Specular.xyz * Lrate3 * pow(max(0,dot(PSR, PSL3)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	col0.w = In.Diffuse.w;	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	
	return Output;
}
PS_OUTPUT PSSkinPPL3NoTex( VSPPL3_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	
	float3 PSN = normalize(In.NandPow.xyz);

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float Lrate2 = In.L2.w;
	float Lrate3 = In.L3.w;
	
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSL3 = normalize(In.L3.xyz);						// ライトベクトル

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1;
	psnl1 = dot( PSN, PSL1 );
	float psnl2;
	psnl2 = dot( PSN, PSL2 );
	float psnl3;
	psnl3 = dot( PSN, PSL3 );


	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
	         + max( 0, psnl3 ) * mLightParams[2][2].xyz * In.Diffuse.xyz * Lrate3
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
//		     + mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
//		     + mLightParams[2][3].xyz * In.Specular.xyz * Lrate3 * pow(max(0,dot(PSR, PSL3)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	col0.w = In.Diffuse.w;	
	Output.Color = col0;	
	return Output;
}

PS_OUTPUT PSSkinGlowL1( VSPPL1_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
			 + In.Ambient.xyz + In.Emissive.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w;

	return Output;
}
PS_OUTPUT PSSkinGlowL2( VSPPL2_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSL2;
	float Lrate2;
	PSL2 = normalize( In.L2.xyz );
	Lrate2 = In.L2.w;


	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1 = dot( PSN, PSL1 );
	float psnl2 = dot( PSN, PSL2 );

//	float3 nh1 = dot( PSN, ( PSL1 + PSE ) * 0.5f );
//	float3 nh2 = dot( PSN, ( PSL2 + PSE ) * 0.5f );
//	(psnl1 < 0) || (nh1 < 0) ? 0.0f : ( nh1 * In.NandPow.w );

	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
		     //+ mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * max(0,dot(PSR, PSL2))//max(0,nh2) * In.NandPow.w * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w;

	return Output;
}
PS_OUTPUT PSSkinGlowL3( VSPPL3_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	
	float3 PSN = normalize(In.NandPow.xyz);

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float Lrate2 = In.L2.w;
	float Lrate3 = In.L3.w;
	
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSL3 = normalize(In.L3.xyz);						// ライトベクトル

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1;
	psnl1 = dot( PSN, PSL1 );
	float psnl2;
	psnl2 = dot( PSN, PSL2 );
	float psnl3;
	psnl3 = dot( PSN, PSL3 );


	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
	         + max( 0, psnl3 ) * mLightParams[2][2].xyz * In.Diffuse.xyz * Lrate3
//		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
//		     + mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
//		     + mLightParams[2][3].xyz * In.Specular.xyz * Lrate3 * pow(max(0,dot(PSR, PSL3)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w;

	return Output;
}

PS_OUTPUT PSSkinGlowAlphaL1( VSPPL1_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1;
	psnl1 = dot( PSN, PSL1 );

	float3 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
			 + In.Ambient.xyz + In.Emissive.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w * texcol.w;

	return Output;
}
PS_OUTPUT PSSkinGlowAlphaL2( VSPPL2_OUTPUT In )
{
	PS_OUTPUT Output;
	float cmprate = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	

	float3 PSL1;
	float Lrate1;
	if( cmprate > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
		Lrate1 = cmprate;
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float3 PSL2;
	float Lrate2;
	PSL2 = normalize( In.L2.xyz );
	Lrate2 = In.L2.w;

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1 = dot( PSN, PSL1 );
	float psnl2 = dot( PSN, PSL2 );

	float4 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
		     //+ mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * max(0,dot(PSR, PSL2))//max(0,nh2) * In.NandPow.w * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w * texcol.w;

	return Output;
}
PS_OUTPUT PSSkinGlowAlphaL3( VSPPL3_OUTPUT In )
{
	PS_OUTPUT Output;
	float Lrate1 = In.L1.w;
	float3 PSE = normalize( ( In.wEye.xyz - In.wPos ) * 1000000.0f );	
	float3 PSN = normalize(In.NandPow.xyz);

	float3 PSL1;
	if( Lrate1 > 0.0 ){
		PSL1 = normalize( In.L1.xyz );
	}else{
		float3 L1;
		float dot2, mag, rate1, rate2;		
		L1 = ( In.L1.xyz - In.wPos ) * 1000000.0f;
		mag = dot( L1, L1 );
		PSL1 = normalize( L1 );
		dot2 = dot( mLightParams[0][5].xyz, -PSL1 );
		rate1 = clamp( 1.0f - mag * mLightParams[0][0].z, 0.0f, 1.0f );
		rate2 = clamp( ( dot2 - mLightParams[0][1].x ) * mLightParams[0][1].y, 0.0f, 1.0f );
		Lrate1 = rate1 * rate2;
	}

	float Lrate2 = In.L2.w;
	float Lrate3 = In.L3.w;
	
	float3 PSL2 = normalize(In.L2.xyz);						// ライトベクトル
	float3 PSL3 = normalize(In.L3.xyz);						// ライトベクトル

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
	float psnl1 = dot( PSN, PSL1 );
	float psnl2 = dot( PSN, PSL2 );
	float psnl3 = dot( PSN, PSL3 );

	float3 col0;
	col0.xyz = max( 0, psnl1 ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate1
	         + max( 0, psnl2 ) * mLightParams[1][2].xyz * In.Diffuse.xyz * Lrate2
	         + max( 0, psnl3 ) * mLightParams[2][2].xyz * In.Diffuse.xyz * Lrate3
//		     + mLightParams[0][3].xyz * In.Specular.xyz * Lrate1 * pow(max(0,dot(PSR, PSL1)), In.NandPow.w)
//		     + mLightParams[1][3].xyz * In.Specular.xyz * Lrate2 * pow(max(0,dot(PSR, PSL2)), In.NandPow.w)
//		     + mLightParams[2][3].xyz * In.Specular.xyz * Lrate3 * pow(max(0,dot(PSR, PSL3)), In.NandPow.w)
			 + In.AmbEmi.xyz;

	float4 texcol = tex2D( DecaleSamp, In.Tex0 );
	Output.Color.xyz = col0.xyz * texcol.xyz * texcol.w * mGlowMult;	
	Output.Color.w = In.Diffuse.w * texcol.w;

	return Output;
}



PS_OUTPUT PSPPShadow1( VSPPSHADOW_OUTPUT1 In )
{
	PS_OUTPUT Output;

	float Lrate = In.L.w;

	float2 smap;
	smap.x = In.ShadowMapUV.x / In.ShadowMapUV.w;
	smap.y = In.ShadowMapUV.y / In.ShadowMapUV.w;
		
	float4 shadow = tex2D( ShadowMapSamp, smap );

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSL = normalize(In.L.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
			
	float psnl;
	psnl = dot( PSN, PSL );

	float4 col0;

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (In.Depth.z / In.Depth.w) > (shadow.x + mShadowCoef.x) ){
			col0.xyz = In.Ambient.xyz * mShadowCoef.y;	
		}else{
			col0.xyz = max( 0, psnl ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate
						+ mLightParams[0][3].xyz * In.Specular.xyz * Lrate * pow(max(0,dot(PSR, PSL)), In.NandPow.w)
						+ In.Ambient.xyz + In.Emissive.xyz;
		}
	}else{
		col0.xyz = max( 0, psnl ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate
					+ mLightParams[0][3].xyz * In.Specular.xyz * Lrate * pow(max(0,dot(PSR, PSL)), In.NandPow.w)
					+ In.Ambient.xyz + In.Emissive.xyz;	
	}
	col0.w = In.Diffuse.w;
	
	Output.Color = col0 * tex2D( DecaleSamp, In.Tex0 );	

	return Output;
}
PS_OUTPUT PSPPShadowNoTex1( VSPPSHADOW_OUTPUT1 In )
{
	PS_OUTPUT Output;

	float Lrate = In.L.w;

	float2 smap;
	smap.x = In.ShadowMapUV.x / In.ShadowMapUV.w;
	smap.y = In.ShadowMapUV.y / In.ShadowMapUV.w;
		
	float4 shadow = tex2D( ShadowMapSamp, smap );

	float3 PSN = normalize(In.NandPow.xyz);
	float3 PSL = normalize(In.L.xyz);						// ライトベクトル
	float3 PSE = normalize(In.Eye);	

	float3 PSR = reflect(-PSE, PSN);		// 反射ベクトル
			
	float psnl;
	psnl = dot( PSN, PSL );

	float4 col0;

	if( (smap.x >= 0.0f) && (smap.x <= 1.0f) && (smap.y >= 0.0f) && (smap.y <= 1.0f) ){	
		if( (In.Depth.z / In.Depth.w) > (shadow.x + mShadowCoef.x) ){
			col0.xyz = In.Ambient.xyz * mShadowCoef.y;	
		}else{
			col0.xyz = max( 0, psnl ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate
						+ mLightParams[0][3].xyz * In.Specular.xyz * Lrate * pow(max(0,dot(PSR, PSL)), In.NandPow.w)
						+ In.Ambient.xyz + In.Emissive.xyz;
		}
	}else{
		col0.xyz = max( 0, psnl ) * mLightParams[0][2].xyz * In.Diffuse.xyz * Lrate
					+ mLightParams[0][3].xyz * In.Specular.xyz * Lrate * pow(max(0,dot(PSR, PSL)), In.NandPow.w)
					+ In.Ambient.xyz + In.Emissive.xyz;	
	}
	col0.w = In.Diffuse.w;
	
	Output.Color = col0;	

	return Output;
}

/***
struct PS_ZNOUTPUT
{
	float4 depth	: COLOR0;
	float4 normal	: COLOR1;
};
***/

PS_OUTPUT PSZN_OnlyZ( VSZN_OUTPUT i )
{
	PS_OUTPUT o;
		
	float odepth;
	odepth = i.PSPos.z / i.PSPos.w;
	
	o.Color.x = odepth;
	o.Color.y = odepth * 256.0f;
	o.Color.z = odepth * 256.0f * 256.0f;
	o.Color.xyz = frac( o.Color.xyz );
		
//	o.Color.xyz = odepth;
	
	o.Color.w = mAlpha;
		
	return o;
}

PS_OUTPUT PSZN_OnlyN( VSZN_OUTPUT i )
{
	PS_OUTPUT o;
	
	float3 onormal;
	onormal = normalize( i.PSNormal.xyz );

    float3 off = { 0.5f, 0.5f, 0.5f };
    o.Color.xyz = onormal.xyz * 0.5f + off;	

	o.Color.w = mAlpha;
	
	return o;
}


PS_OUTPUT PSZN_OnlyNCam( VSZN_OUTPUT i )
{
	PS_OUTPUT o;
	
	float3 onormal;
	onormal = normalize( i.PSNormal.xyz );

    float3 off = { 0.5f, 0.5f, 0.5f };
    o.Color.xyz = onormal.xyz * 0.5f + off;	
	o.Color.w = mAlpha;
	
	return o;
}

PS_OUTPUT PSZN_OnlyZ2( VSZN_OUTPUT i )
{
	PS_OUTPUT o;
		
	float odepth;
	odepth = i.PSPos.z / i.PSPos.w;
	
	float4 unpacked_depth = float4(0, 0, 256.0f, 256.0f);
	unpacked_depth.g = modf( odepth * 256.0f, unpacked_depth.r);
	unpacked_depth.b *= modf( unpacked_depth.g * 256.0f, unpacked_depth.g );

	o.Color = unpacked_depth / 256.0f;  // 標準化
	
	o.Color.w = mZNUser1.x + 0.001f;
		
	return o;
}

PS_OUTPUT PSZN_OnlyN2( VSZN_OUTPUT i )
{
	PS_OUTPUT o;
	
	float3 onormal;
	onormal = normalize( i.PSNormal.xyz );

    float3 off = { 0.5f, 0.5f, 0.5f };
    o.Color.xyz = onormal.xyz * 0.5f + off;	

	o.Color.w = mZNUser1.y + 0.001f;
	
	return o;
}


PS_OUTPUT PSZN_OnlyNCam2( VSZN_OUTPUT i )
{
	PS_OUTPUT o;
	
	float3 onormal;
	onormal = normalize( i.PSNormal.xyz );

    float3 off = { 0.5f, 0.5f, 0.5f };
    o.Color.xyz = onormal.xyz * 0.5f + off;	
	o.Color.w = mZNUser1.y + 0.001f;
	
	return o;
}

PS_OUTPUT PSZN_User2( VSZN_OUTPUT i )
{
	PS_OUTPUT o;
	
    o.Color.x = mZNUser2.x + 0.001f;	
    o.Color.y = mZNUser2.y + 0.001f;	
    o.Color.z = mZNUser2.z + 0.001f;	
    o.Color.w = mZNUser2.w + 0.001f;	

	return o;
}



//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique RenderSceneBone4
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB4();
        PixelShader  = NULL;
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB4BlurWorld();
        PixelShader  = NULL;
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB4BlurCamera();
        PixelShader  = NULL;
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB4();
        PixelShader  = compile ps_2_0 PSSkinGlow();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB4();
        PixelShader  = compile ps_2_0 PSSkinGlowAlpha();
    }
		
}

technique RenderSceneBone1
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB1();
        PixelShader  = NULL;
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB1BlurWorld();
        PixelShader  = NULL;
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB1BlurCamera();
        PixelShader  = NULL;
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB1();
        PixelShader  = compile ps_2_0 PSSkinGlow();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB1();
        PixelShader  = compile ps_2_0 PSSkinGlowAlpha();
    }
}
technique RenderSceneBone0
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB0();
        PixelShader  = NULL;
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB0BlurWorld();
        PixelShader  = NULL;
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB0BlurCamera();
        PixelShader  = NULL;
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB0();
        PixelShader  = compile ps_2_0 PSSkinGlow();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB0();
        PixelShader  = compile ps_2_0 PSSkinGlowAlpha();
    }    
}

///////PP
technique RenderSceneBone4PP
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB4PPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB4PPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB4PPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3();
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB4PPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1NoTex();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB4PPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2NoTex();
    }
    pass P5
    {          
        VertexShader = compile vs_2_0 SkinB4PPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3NoTex();
    }


    pass P6
    {          
        VertexShader = compile vs_2_0 SkinB4BlurWorldPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1();
    }
    pass P7
    {          
        VertexShader = compile vs_2_0 SkinB4BlurWorldPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2();
    }
    pass P8
    {          
        VertexShader = compile vs_2_0 SkinB4BlurWorldPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3();
    }
    pass P9
    {          
        VertexShader = compile vs_2_0 SkinB4BlurWorldPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1NoTex();
    }
    pass P10
    {          
        VertexShader = compile vs_2_0 SkinB4BlurWorldPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2NoTex();
    }
    pass P11
    {          
        VertexShader = compile vs_2_0 SkinB4BlurWorldPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3NoTex();
    }


    pass P12
    {          
        VertexShader = compile vs_2_0 SkinB4BlurCameraPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1();
    }
    pass P13
    {          
        VertexShader = compile vs_2_0 SkinB4BlurCameraPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2();
    }
    pass P14
    {          
        VertexShader = compile vs_2_0 SkinB4BlurCameraPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3();
    }
    pass P15
    {          
        VertexShader = compile vs_2_0 SkinB4BlurCameraPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1NoTex();
    }
    pass P16
    {          
        VertexShader = compile vs_2_0 SkinB4BlurCameraPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2NoTex();
    }
    pass P17
    {          
        VertexShader = compile vs_2_0 SkinB4BlurCameraPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3NoTex();
    }
    
    
    pass P18
    {          
        VertexShader = compile vs_2_0 SkinB4PPL1();
        PixelShader  = compile ps_2_0 PSSkinGlowL1();
    }
    pass P19
    {          
        VertexShader = compile vs_2_0 SkinB4PPL2();
        PixelShader  = compile ps_2_0 PSSkinGlowL2();
    }
    pass P20
    {          
        VertexShader = compile vs_2_0 SkinB4PPL3();
        PixelShader  = compile ps_2_0 PSSkinGlowL3();
    }
    
    
    pass P21
    {          
        VertexShader = compile vs_2_0 SkinB4PPL1();
        PixelShader  = compile ps_2_0 PSSkinGlowAlphaL1();
    }
    pass P22
    {          
        VertexShader = compile vs_2_0 SkinB4PPL2();
        PixelShader  = compile ps_2_0 PSSkinGlowAlphaL2();
    }
    pass P23
    {          
        VertexShader = compile vs_2_0 SkinB4PPL3();
        PixelShader  = compile ps_2_0 PSSkinGlowAlphaL3();
    }
		
}
technique RenderSceneBone0PP
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB0PPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB0PPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB0PPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3();
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB0PPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1NoTex();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB0PPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2NoTex();
    }
    pass P5
    {          
        VertexShader = compile vs_2_0 SkinB0PPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3NoTex();
    }


    pass P6
    {          
        VertexShader = compile vs_2_0 SkinB0BlurWorldPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1();
    }
    pass P7
    {          
        VertexShader = compile vs_2_0 SkinB0BlurWorldPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2();
    }
    pass P8
    {          
        VertexShader = compile vs_2_0 SkinB0BlurWorldPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3();
    }
    pass P9
    {          
        VertexShader = compile vs_2_0 SkinB0BlurWorldPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1NoTex();
    }
    pass P10
    {          
        VertexShader = compile vs_2_0 SkinB0BlurWorldPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2NoTex();
    }
    pass P11
    {          
        VertexShader = compile vs_2_0 SkinB0BlurWorldPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3NoTex();
    }


    pass P12
    {          
        VertexShader = compile vs_2_0 SkinB0BlurCameraPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1();
    }
    pass P13
    {          
        VertexShader = compile vs_2_0 SkinB0BlurCameraPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2();
    }
    pass P14
    {          
        VertexShader = compile vs_2_0 SkinB0BlurCameraPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3();
    }
    pass P15
    {          
        VertexShader = compile vs_2_0 SkinB0BlurCameraPPL1();
        PixelShader  = compile ps_2_0 PSSkinPPL1NoTex();
    }
    pass P16
    {          
        VertexShader = compile vs_2_0 SkinB0BlurCameraPPL2();
        PixelShader  = compile ps_2_0 PSSkinPPL2NoTex();
    }
    pass P17
    {          
        VertexShader = compile vs_2_0 SkinB0BlurCameraPPL3();
        PixelShader  = compile ps_2_0 PSSkinPPL3NoTex();
    }
    
    
    pass P18
    {          
        VertexShader = compile vs_2_0 SkinB0PPL1();
        PixelShader  = compile ps_2_0 PSSkinGlowL1();
    }
    pass P19
    {          
        VertexShader = compile vs_2_0 SkinB0PPL2();
        PixelShader  = compile ps_2_0 PSSkinGlowL2();
    }
    pass P20
    {          
        VertexShader = compile vs_2_0 SkinB0PPL3();
        PixelShader  = compile ps_2_0 PSSkinGlowL3();
    }
    
    
    pass P21
    {          
        VertexShader = compile vs_2_0 SkinB0PPL1();
        PixelShader  = compile ps_2_0 PSSkinGlowAlphaL1();
    }
    pass P22
    {          
        VertexShader = compile vs_2_0 SkinB0PPL2();
        PixelShader  = compile ps_2_0 PSSkinGlowAlphaL2();
    }
    pass P23
    {          
        VertexShader = compile vs_2_0 SkinB0PPL3();
        PixelShader  = compile ps_2_0 PSSkinGlowAlphaL3();
    }
		
}


///////
technique RenderSceneNormalShadow
{
	pass P0//ボーン４テクスチャありSM
	{
		VertexShader = compile vs_2_0 SkinB4Shadow0();
		PixelShader = compile ps_2_0 PSNormalShadowTex0();
	}
	pass P1//ボーン４テクスチャなしSM
	{
		VertexShader = compile vs_2_0 SkinB4Shadow0();
		PixelShader = compile ps_2_0 PSNormalShadowNoTex0();
	}

	pass P2//ボーン４テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB4Shadow1();
		PixelShader = compile ps_2_0 PSNormalShadowTex1();
	}
	pass P3//ボーン４テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB4Shadow1();
		PixelShader = compile ps_2_0 PSNormalShadowNoTex1();
	}	
	
	
	pass P4//ボーン０テクスチャありSM
	{
		VertexShader = compile vs_2_0 SkinB0Shadow0();
		PixelShader = compile ps_2_0 PSNormalShadowTex0();
	}
	pass P5//ボーン０テクスチャなしSM
	{
		VertexShader = compile vs_2_0 SkinB0Shadow0();
		PixelShader = compile ps_2_0 PSNormalShadowNoTex0();
	}
	pass P6//ボーン０テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB0Shadow1();
		PixelShader = compile ps_2_0 PSNormalShadowTex1();
	}
	pass P7//ボーン０テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB0Shadow1();
		PixelShader = compile ps_2_0 PSNormalShadowNoTex1();
	}
		
}

technique RenderSceneBumpShadow
{
	//シャドウマップへのレンダーはRenderSceneNormalShadowを使う。

	pass P0//ボーン４テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB4BumpShadow1();
		PixelShader = compile ps_2_0 PSBumpShadow1();
	}	
	pass P1//ボーン０テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB0BumpShadow1();
		PixelShader = compile ps_2_0 PSBumpShadow1();
	}
		
}

technique RenderScenePPShadow
{
	//シャドウマップへのレンダーはRenderSceneNormalShadowを使う。

	pass P0//ボーン４テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB4PPShadow1();
		PixelShader = compile ps_2_0 PSPPShadow1();
	}	
	pass P1//ボーン４テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB4PPShadow1();
		PixelShader = compile ps_2_0 PSPPShadowNoTex1();
	}	


	pass P2//ボーン０テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB0PPShadow1();
		PixelShader = compile ps_2_0 PSPPShadow1();
	}
	pass P3//ボーン０テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB0PPShadow1();
		PixelShader = compile ps_2_0 PSPPShadowNoTex1();
	}

}


technique RenderSceneToon0Shadow
{
	//シャドウマップへのレンダーはRenderSceneNormalShadowを使う。

	pass P0//ボーン４テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB4Toon0Shadow1();
		PixelShader = compile ps_2_0 PSToon0ShadowTex1();
	}
	pass P1//ボーン４テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB4Toon0Shadow1();
		PixelShader = compile ps_2_0 PSToon0ShadowNoTex1();
	}
	pass P2//ボーン０テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB0Toon0Shadow1();
		PixelShader = compile ps_2_0 PSToon0ShadowTex1();
	}
	pass P3//ボーン０テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB0Toon0Shadow1();
		PixelShader = compile ps_2_0 PSToon0ShadowNoTex1();
	}
}


technique RenderSceneToon1Shadow
{
	//シャドウマップへのレンダーはRenderSceneNormalShadowを使う。

	pass P0//ボーン４テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB4Toon1Shadow1();
		PixelShader = compile ps_2_0 PSToon1ShadowTex1();
	}
	pass P1//ボーン４テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB4Toon1Shadow1();
		PixelShader = compile ps_2_0 PSToon1ShadowNoTex1();
	}
	pass P2//ボーン０テクスチャあり
	{
		VertexShader = compile vs_2_0 SkinB0Toon1Shadow1();
		PixelShader = compile ps_2_0 PSToon1ShadowTex1();
	}
	pass P3//ボーン０テクスチャなし
	{
		VertexShader = compile vs_2_0 SkinB0Toon1Shadow1();
		PixelShader = compile ps_2_0 PSToon1ShadowNoTex1();
	}
}


/////////
technique RenderSceneBone4Toon
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB4Toon();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB4ToonBlurWorld();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB4ToonBlurCamera();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P3
    {
		VertexShader = compile vs_2_0 SkinB4Toon();
		PixelShader = compile ps_2_0 PSToonGlow();
	}
    pass P4
    {
		VertexShader = compile vs_2_0 SkinB4Toon();
		PixelShader = compile ps_2_0 PSToonGlowAlpha();
	}	
}
technique RenderSceneBone1Toon
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB1Toon();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB1ToonBlurWorld();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB1ToonBlurCamera();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P3
    {
		VertexShader = compile vs_2_0 SkinB1Toon();
		PixelShader = compile ps_2_0 PSToonGlow();
	}
    pass P4
    {
		VertexShader = compile vs_2_0 SkinB1Toon();
		PixelShader = compile ps_2_0 PSToonGlowAlpha();
	}
}
technique RenderSceneBone0Toon
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB0Toon();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB0ToonBlurWorld();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB0ToonBlurCamera();
        PixelShader  = compile ps_2_0 PSToon0Tex();
    }
    pass P3
    {
		VertexShader = compile vs_2_0 SkinB0Toon();
		PixelShader = compile ps_2_0 PSToonGlow();
	}
    pass P4
    {
		VertexShader = compile vs_2_0 SkinB0Toon();
		PixelShader = compile ps_2_0 PSToonGlowAlpha();
	}    
}
///////////
technique RenderSceneBone4ToonNoTex
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB4Toon();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB4ToonBlurWorld();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB4ToonBlurCamera();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
}
technique RenderSceneBone1ToonNoTex
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB1Toon();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB1ToonBlurWorld();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB1ToonBlurCamera();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
}
technique RenderSceneBone0ToonNoTex
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB0Toon();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB0ToonBlurWorld();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB0ToonBlurCamera();
        PixelShader  = compile ps_2_0 PSToon0NoTex();
    }
}
/////////
technique RenderSceneBone4Toon1
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB4Toon1();
        PixelShader  = NULL;
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB4Toon1BlurWorld();
        PixelShader  = NULL;
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB4Toon1BlurCamera();
        PixelShader  = NULL;
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB4Toon1();
        PixelShader  = compile ps_2_0 PSSkinToon1Glow();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB4Toon1();
        PixelShader  = compile ps_2_0 PSSkinToon1GlowAlpha();
    }
    
}
technique RenderSceneBone1Toon1
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB1Toon1();
        PixelShader  = NULL;
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB1Toon1BlurWorld();
        PixelShader  = NULL;
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB1Toon1BlurCamera();
        PixelShader  = NULL;
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB1Toon1();
        PixelShader  = compile ps_2_0 PSSkinToon1Glow();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB1Toon1();
        PixelShader  = compile ps_2_0 PSSkinToon1GlowAlpha();
    }

}
technique RenderSceneBone0Toon1
{
    pass P0
    {          
        VertexShader = compile vs_2_0 SkinB0Toon1();
        PixelShader  = NULL;
    }
    pass P1
    {          
        VertexShader = compile vs_2_0 SkinB0Toon1BlurWorld();
        PixelShader  = NULL;
    }
    pass P2
    {          
        VertexShader = compile vs_2_0 SkinB0Toon1BlurCamera();
        PixelShader  = NULL;
    }
    pass P3
    {          
        VertexShader = compile vs_2_0 SkinB0Toon1();
        PixelShader  = compile ps_2_0 PSSkinToon1Glow();
    }
    pass P4
    {          
        VertexShader = compile vs_2_0 SkinB0Toon1();
        PixelShader  = compile ps_2_0 PSSkinToon1GlowAlpha();
    }
    
}

//////////////
technique RenderSceneBone4Bump
{
	pass P0
	{
		VertexShader = compile vs_2_0 SkinB4BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P1
	{
		VertexShader = compile vs_2_0 SkinB4BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P2
	{
		VertexShader = compile vs_2_0 SkinB4BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P3
	{
		VertexShader = compile vs_2_0 SkinB4BumpL1BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P4
	{
		VertexShader = compile vs_2_0 SkinB4BumpL2BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P5
	{
		VertexShader = compile vs_2_0 SkinB4BumpL3BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P6
	{
		VertexShader = compile vs_2_0 SkinB4BumpL1BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P7
	{
		VertexShader = compile vs_2_0 SkinB4BumpL2BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P8
	{
		VertexShader = compile vs_2_0 SkinB4BumpL3BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P9
	{
		VertexShader = compile vs_2_0 SkinB4BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1Glow();
	}
	pass P10
	{
		VertexShader = compile vs_2_0 SkinB4BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2Glow();
	}
	pass P11
	{
		VertexShader = compile vs_2_0 SkinB4BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3Glow();
	}
	
	pass P12
	{
		VertexShader = compile vs_2_0 SkinB4BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1GlowAlpha();
	}
	pass P13
	{
		VertexShader = compile vs_2_0 SkinB4BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2GlowAlpha();
	}
	pass P14
	{
		VertexShader = compile vs_2_0 SkinB4BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3GlowAlpha();
	}


}
technique RenderSceneBone1Bump
{
	pass P0
	{
		VertexShader = compile vs_2_0 SkinB1BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P1
	{
		VertexShader = compile vs_2_0 SkinB1BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P2
	{
		VertexShader = compile vs_2_0 SkinB1BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P3
	{
		VertexShader = compile vs_2_0 SkinB1BumpL1BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P4
	{
		VertexShader = compile vs_2_0 SkinB1BumpL2BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P5
	{
		VertexShader = compile vs_2_0 SkinB1BumpL3BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P6
	{
		VertexShader = compile vs_2_0 SkinB1BumpL1BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P7
	{
		VertexShader = compile vs_2_0 SkinB1BumpL2BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P8
	{
		VertexShader = compile vs_2_0 SkinB1BumpL3BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P9
	{
		VertexShader = compile vs_2_0 SkinB1BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1Glow();
	}
	pass P10
	{
		VertexShader = compile vs_2_0 SkinB1BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2Glow();
	}
	pass P11
	{
		VertexShader = compile vs_2_0 SkinB1BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3Glow();
	}
	
	pass P12
	{
		VertexShader = compile vs_2_0 SkinB1BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1GlowAlpha();
	}
	pass P13
	{
		VertexShader = compile vs_2_0 SkinB1BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2GlowAlpha();
	}
	pass P14
	{
		VertexShader = compile vs_2_0 SkinB1BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3GlowAlpha();
	}

}
technique RenderSceneBone0Bump
{
	pass P0
	{
		VertexShader = compile vs_2_0 SkinB0BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P1
	{
		VertexShader = compile vs_2_0 SkinB0BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P2
	{
		VertexShader = compile vs_2_0 SkinB0BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P3
	{
		VertexShader = compile vs_2_0 SkinB0BumpL1BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P4
	{
		VertexShader = compile vs_2_0 SkinB0BumpL2BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P5
	{
		VertexShader = compile vs_2_0 SkinB0BumpL3BlurWorld();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P6
	{
		VertexShader = compile vs_2_0 SkinB0BumpL1BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL1();
	}
	pass P7
	{
		VertexShader = compile vs_2_0 SkinB0BumpL2BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL2();
	}
	pass P8
	{
		VertexShader = compile vs_2_0 SkinB0BumpL3BlurCamera();
		PixelShader = compile ps_2_0 PSBumpL3();
	}


	pass P9
	{
		VertexShader = compile vs_2_0 SkinB0BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1Glow();
	}
	pass P10
	{
		VertexShader = compile vs_2_0 SkinB0BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2Glow();
	}
	pass P11
	{
		VertexShader = compile vs_2_0 SkinB0BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3Glow();
	}
	
	pass P12
	{
		VertexShader = compile vs_2_0 SkinB0BumpL1();
		PixelShader = compile ps_2_0 PSBumpL1GlowAlpha();
	}
	pass P13
	{
		VertexShader = compile vs_2_0 SkinB0BumpL2();
		PixelShader = compile ps_2_0 PSBumpL2GlowAlpha();
	}
	pass P14
	{
		VertexShader = compile vs_2_0 SkinB0BumpL3();
		PixelShader = compile ps_2_0 PSBumpL3GlowAlpha();
	}

}

////////////
technique RenderSceneBone4Edge0
{
    pass P0
    {          
        VertexShader = compile vs_1_1 SkinB4Edge0();
        PixelShader  = NULL;
    }
}
technique RenderSceneBone1Edge0
{
    pass P0
    {          
        VertexShader = compile vs_1_1 SkinB1Edge0();
        PixelShader  = NULL;
    }
}
technique RenderSceneBone0Edge0
{
    pass P0
    {          
        VertexShader = compile vs_1_1 SkinB0Edge0();
        PixelShader  = NULL;
    }
}


//// Z, normal 2path
technique RenderZN_2path_NormalBone
{
	pass P0
	{
		VertexShader = compile vs_2_0 VSZN_NormalB4();
		PixelShader = compile ps_2_0 PSZN_OnlyZ();
	}
	pass P1
	{
		VertexShader = compile vs_2_0 VSZN_NormalB4();
		PixelShader = compile ps_2_0 PSZN_OnlyN();
	}
	pass P2
	{
		VertexShader = compile vs_2_0 VSZN_NormalCamB4();
		PixelShader = compile ps_2_0 PSZN_OnlyNCam();
	}
	

	//ZN2
	pass P3
	{
		VertexShader = compile vs_2_0 VSZN_NormalB4();
		PixelShader = compile ps_2_0 PSZN_OnlyZ2();
	}
	pass P4
	{
		VertexShader = compile vs_2_0 VSZN_NormalB4();
		PixelShader = compile ps_2_0 PSZN_OnlyN2();
	}
	pass P5
	{
		VertexShader = compile vs_2_0 VSZN_NormalCamB4();
		PixelShader = compile ps_2_0 PSZN_OnlyNCam2();
	}
	pass P6
	{
		VertexShader = compile vs_2_0 VSZN_NormalB4();
		PixelShader = compile ps_2_0 PSZN_User2();
	}

}
technique RenderZN_2path_NormalNoBone
{
	pass P0
	{
		VertexShader = compile vs_2_0 VSZN_NormalB0();
		PixelShader = compile ps_2_0 PSZN_OnlyZ();
	}
	pass P1
	{
		VertexShader = compile vs_2_0 VSZN_NormalB0();
		PixelShader = compile ps_2_0 PSZN_OnlyN();
	}
	pass P2
	{
		VertexShader = compile vs_2_0 VSZN_NormalCamB0();
		PixelShader = compile ps_2_0 PSZN_OnlyNCam();
	}

	//ZN2
	pass P3
	{
		VertexShader = compile vs_2_0 VSZN_NormalB0();
		PixelShader = compile ps_2_0 PSZN_OnlyZ2();
	}
	pass P4
	{
		VertexShader = compile vs_2_0 VSZN_NormalB0();
		PixelShader = compile ps_2_0 PSZN_OnlyN2();
	}
	pass P5
	{
		VertexShader = compile vs_2_0 VSZN_NormalCamB0();
		PixelShader = compile ps_2_0 PSZN_OnlyNCam2();
	}
	pass P6
	{
		VertexShader = compile vs_2_0 VSZN_NormalB0();
		PixelShader = compile ps_2_0 PSZN_User2();
	}

}
