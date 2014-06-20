#ifndef MQOMATERIALH
#define MQOMATERIALH

#include <stdio.h>
#include <stdarg.h>

#include <coef.h>
#include <basedat.h>

#define DBGH
#include <dbg.h>

class CMotionCtrl;
class CShdElem;
class CTreeHandler2;
class CMQOMaterial
{
public:
	CMQOMaterial();
	~CMQOMaterial();

	int SetParams( int srcno, ARGBF sceneamb, char* srcchar, int srcleng );


	int SetName( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetCol( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetDif( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetAmb( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetEmi( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetSpc( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetPower( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetTex( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetAlpha( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetBump( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetShader( char* srcchar, int pos, int srcleng, int* stepnum );
	int SetVcolFlag( char* srcchar, int pos, int srcleng, int* stepnum );

	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );


	int Dump();

	int IsSame( CMQOMaterial* compmat, int compnameflag, int cmplevel = 0 );//compnameflag == 0 の時は、名前は比較しない。

	int GetName( int srcmatno, char** nameptr );
	int AddConvName( char** ppname );

	int SetCurTexname( int srcenabletex );

	int SetExtTex( int srctexnum, char** srctexname );
	int SetConvName( int srcconvnum, char** srcconvname );

	int SetLegacyMaterial( CTreeHandler2* lpth, CShdElem* selem );

	int GetColorrefDiffuse( COLORREF* dstdiffuse );
	int GetColorrefSpecular( COLORREF* dstspecular );
	int GetColorrefAmbient( COLORREF* dstambient );
	int GetColorrefEmissive( COLORREF* dstemissive );

private:
	int InitParams();
	int DestroyObjs();

	int ConvParamsTo3F();

public:
	int materialno;
	int legacyflag;//古いデータのとき１。パーツ単位からマテリアルを作成したときに１をセットする。
	char name[256];

	ARGBF col;
	float dif;
	float amb;
	float emi;
	float spc;
	float power;
	char tex[256];
	char alpha[256];
	char bump[256];

	int vcolflag;
////

	int shader;

////
	ARGBF dif4f;
	RGB3F amb3f;
	RGB3F emi3f;
	RGB3F spc3f;

	int transparent;
	int texrule;
	int blendmode;

	float uanime;
	float vanime;
/////
	unsigned char exttexnum;
	unsigned char exttexmode; //重ねるか、パラパラアニメするかどうか。
	unsigned char exttexrep; //繰り返しかどうか
	unsigned char exttexstep; //何フレームごとに、変化するか。
	char** exttexname;

/// for miko
	int convnamenum;
	char** ppconvname;

///
	char* curtexname;

	CMQOMaterial* next;

	ARGBF m_sceneamb;

	char alphatest0;//withalpha 0 のときのアルファテスト
	unsigned char alphaval0;

	char alphatest1;//withalpha 1 のときのアルファテスト
	unsigned char alphaval1;

	char texanimtype;
	float glowmult[3];

	float orgalpha;
};


#endif