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

	int IsSame( CMQOMaterial* compmat, int compnameflag, int cmplevel = 0 );//compnameflag == 0 �̎��́A���O�͔�r���Ȃ��B

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
	int legacyflag;//�Â��f�[�^�̂Ƃ��P�B�p�[�c�P�ʂ���}�e���A�����쐬�����Ƃ��ɂP���Z�b�g����B
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
	unsigned char exttexmode; //�d�˂邩�A�p���p���A�j�����邩�ǂ����B
	unsigned char exttexrep; //�J��Ԃ����ǂ���
	unsigned char exttexstep; //���t���[�����ƂɁA�ω����邩�B
	char** exttexname;

/// for miko
	int convnamenum;
	char** ppconvname;

///
	char* curtexname;

	CMQOMaterial* next;

	ARGBF m_sceneamb;

	char alphatest0;//withalpha 0 �̂Ƃ��̃A���t�@�e�X�g
	unsigned char alphaval0;

	char alphatest1;//withalpha 1 �̂Ƃ��̃A���t�@�e�X�g
	unsigned char alphaval1;

	char texanimtype;
	float glowmult[3];

	float orgalpha;
};


#endif