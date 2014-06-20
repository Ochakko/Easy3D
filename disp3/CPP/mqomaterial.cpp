#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <mqomaterial.h>
#include <motionctrl.h>
#include <motionpoint2.h>

#include <shdelem.h>
#include <treehandler2.h>
#include <treeelem2.h>



CMQOMaterial::CMQOMaterial()
{
	InitParams();
}
CMQOMaterial::~CMQOMaterial()
{
	DestroyObjs();
}

int CMQOMaterial::SetParams( int srcno, ARGBF sceneamb, char* srcchar, int srcleng )
{
	//"肌色" col(1.000 0.792 0.651 1.000) dif(0.490) amb(0.540) emi(0.530) spc(0.020) power(0.00)


//DbgOut( "check!!!: mqomat : SetParams : %d, %s, %d\r\n", srcno, srcchar, srcleng );

	materialno = srcno;

	m_sceneamb = sceneamb;//!!!!!!!!!!!!!!!!!

	char pat[12][20] = 
	{
		"col(",
		"dif(",
		"amb(",
		"emi(",
		"spc(",
		"power(",
		"tex(",
		"alpha(",
		"bump(",
		"shader(",
		"vcol(",
		"\"" //！！！tex, alpha, bumpよりも後でないとだめ。
	};

	int pos = 0;
	int stepnum;
	int ret;

	while( pos < srcleng ){
		while( (pos < srcleng) &&  
		( (*(srcchar + pos) == ' ') || (*(srcchar + pos) == '\t') )
		){
		pos++;
		}

		int cmp;
		int patno, patleng;

		int isfind = 0;

		for( patno = 0; patno < 12; patno++ ){
			if( isfind == 1 )
				break;

			patleng = (int)strlen( pat[patno] );
			if( srcleng - pos >= patleng ){
				cmp = strncmp( pat[patno], srcchar + pos, patleng );
				if( cmp == 0 ){
					isfind = 1;
					switch( patno ){
					case 0:
						ret = SetCol( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 1:
						ret = SetDif( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 2:
						ret = SetAmb( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 3:
						ret = SetEmi( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 4:
						ret = SetSpc( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 5:
						ret = SetPower( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 6:
						ret = SetTex( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 7:
						ret = SetAlpha( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 8:
						ret = SetBump( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 9:
						ret = SetShader( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 10:
						ret = SetVcolFlag( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 11:
						//DbgOut( "MQOMaterial : SetParams : SetName %d\n", pos );
						ret = SetName( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					default:
						_ASSERT( 0 );
						return 1;
						break;
					}

				}
			}
		}

		if( isfind == 1 ){
			pos += stepnum;
		}else{
			pos++;
		}

	}

	ret = ConvParamsTo3F();
	_ASSERT( !ret );


	return 0;
}

int CMQOMaterial::ConvParamsTo3F()
{
	dif4f.a = col.a;
	dif4f.r = col.r * dif;
	dif4f.g = col.g * dif;
	dif4f.b = col.b * dif;

//	amb3f.r = col.r * amb;
//	amb3f.g = col.g * amb;
//	amb3f.b = col.b * amb;

	amb3f.r = m_sceneamb.r * amb;
	amb3f.g = m_sceneamb.g * amb;
	amb3f.b = m_sceneamb.b * amb;

	emi3f.r = col.r * emi;
	emi3f.g = col.g * emi;
	emi3f.b = col.b * emi;

	spc3f.r = col.r * spc;
	spc3f.g = col.g * spc;
	spc3f.b = col.b * spc;

	return 0;
}


int CMQOMaterial::InitParams()
{	
	materialno = -1;
	legacyflag = 0;
	ZeroMemory ( name, 256 );

	col.a = 1.0f;
	col.r = 1.0f;
	col.g = 1.0f;
	col.b = 1.0f;

	dif = 1.0f;
	amb = 0.25f;
	emi = 0.0f;
	spc = 0.0f;
	power = 0.0f;

	ZeroMemory ( tex, 256 );
	ZeroMemory ( alpha, 256 );
	ZeroMemory ( bump, 256 );

	next = 0;

	convnamenum = 0;
	ppconvname = 0;

	shader = 3;

	vcolflag = 0;

	dif4f.a = 1.0f;
	dif4f.r = 1.0f;
	dif4f.g = 1.0f;
	dif4f.b = 1.0f;

	amb3f.r = 0.25f;
	amb3f.g = 0.25f;
	amb3f.b = 0.25f;

	emi3f.r = 0.0f;
	emi3f.g = 0.0f;
	emi3f.b = 0.0f;

	spc3f.r = 0.0f;
	spc3f.g = 0.0f;
	spc3f.b = 0.0f;

	transparent = 0;
	texrule = TEXRULE_MQ;//!!!!!!!!!
	blendmode = 0;

	uanime = 0.0f;
	vanime = 0.0f;

	exttexnum = 0;
	exttexmode = EXTTEXMODE_NORMAL; //重ねるか、パラパラアニメするかどうか。
	exttexrep = EXTTEXREP_ROUND; //繰り返しかどうか
	exttexstep = 1; //何フレームごとに、変化するか。
	exttexname = 0;

	curtexname = 0;

	alphatest0 = 1;
	alphaval0 = 8;
	alphatest1 = 1;
	alphaval1 = 8;

	texanimtype = TEXANIM_WRAP;//!!!!!!!
	glowmult[0] = 1.0f;
	glowmult[1] = 1.0f;
	glowmult[2] = 1.0f;

	orgalpha = 1.0f;
	
	return 0;
}

int CMQOMaterial::DestroyObjs()
{
	if( exttexname ){
		int delcnt;
		for( delcnt = 0; delcnt < exttexnum; delcnt++ ){
			char* delname;
			delname = *( exttexname + delcnt );
			if( delname ){
				free( delname );
			}
		}
		free( exttexname );
		exttexname = 0;
		exttexnum = 0;
	}

	if( convnamenum > 0 ){
		int nameno;
		for( nameno = 0; nameno < convnamenum; nameno++ ){
			char* delconv;
			delconv = *( ppconvname + nameno );
			if( delconv )
				free( delconv );
		}

		free( ppconvname );
		ppconvname = 0;
		convnamenum = 0;
	}

	return 0;
}

int CMQOMaterial::SetName( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//"肌色"
	int step = 1;

	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"')

// "の後に)またはスペースが続いたときのみ終端とする。
//		!( (*(srcchar + pos + step) == '\"') && ( (*(srcchar + pos + step + 1) == ')' ) || (*(srcchar + pos + step + 1) == ' ' ) ) ) 

	){
		step++;
	}

	if( ((step - 1) < 256) && ((step - 1) > 0) ){
		ZeroMemory( name, sizeof( char ) * 256 );
		strncpy_s( name, 256, srcchar + pos + 1, step - 1 );
		name[step -1] = 0;
	}

//	DbgOut( "check!!! : mqomat : SetName %s, %d, %d\r\n", srcchar, pos, srcleng );
//	DbgOut( "check!!! : mqomat : SetName %d, %s\r\n", step, name );


	if( pos + step < srcleng )
		*stepnum = step + 1;
	else
		*stepnum = step;


	return 0;
}
int CMQOMaterial::SetCol( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//col(1.000 0.792 0.651 1.000)
	int ret;
	int step;
	int cnt;
	float dstfloat;
	*stepnum = 4;
	for( cnt = 0; cnt < 4; cnt++ ){		
		ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
		if( ret )
			return ret;

		switch( cnt ){
		case 0:
			col.r = dstfloat;
			break;
		case 1:
			col.g = dstfloat;
			break;
		case 2:
			col.b = dstfloat;
			break;
		case 3:
			col.a = dstfloat;
			orgalpha = dstfloat;
			break;
		default:
			break;
		}

		*stepnum += step;
	}

	(*stepnum)++;//最後の）の分

	return 0;
}


int CMQOMaterial::SetDif( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//dif(0.490) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;
	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	dif = dstfloat;

	dif4f.a = col.a;
	dif4f.r = col.r * dif;
	dif4f.g = col.g * dif;
	dif4f.b = col.b * dif;


	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetAmb( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//amb(0.540) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	amb = dstfloat;

//	amb3f.r = col.r * amb;
//	amb3f.g = col.g * amb;
//	amb3f.b = col.b * amb;

	amb3f.r = m_sceneamb.r * amb;
	amb3f.g = m_sceneamb.g * amb;
	amb3f.b = m_sceneamb.b * amb;

	*stepnum += step + 1;
	return 0;
}
int CMQOMaterial::SetEmi( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//emi(0.530)
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	emi = dstfloat;

	emi3f.r = col.r * emi;
	emi3f.g = col.g * emi;
	emi3f.b = col.b * emi;


	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetSpc( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//spc(0.020) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	spc = dstfloat;

	spc3f.r = col.r * spc;
	spc3f.g = col.g * spc;
	spc3f.b = col.b * spc;

	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetPower( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//power(0.00)
	int step;
	float dstfloat;
	*stepnum = 6;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	power = dstfloat;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetShader( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//shader(3)
	int step;
	int dstint;
	*stepnum = 7;
	int ret;

	ret = GetInt( &dstint, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	shader = dstint;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetVcolFlag( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//vcol(1)
	int step;
	int dstint;
	*stepnum = 5;
	int ret;

	ret = GetInt( &dstint, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	vcolflag = dstint;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetTex( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//tex("sakana.jpg")
	int step = 5;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 5 < 256) && (step - 5 > 0) ){
		strncpy_s( tex, 256, srcchar + pos + 5, step - 5 );
		tex[step -5] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//　")の分
	else
		*stepnum = step;


	return 0;
}
int CMQOMaterial::SetAlpha( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//alpha("sakana.jpg")
	int step = 7;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 7 < 256) && (step - 7 > 0) ){
		strncpy_s( alpha, 256, srcchar + pos + 7, step - 7 );
		alpha[step -7] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//　")の分
	else
		*stepnum = step;

	return 0;
}
int CMQOMaterial::SetBump( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//bump("sakana.jpg")
	int step = 6;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 6 < 256) && (step - 6 > 0) ){
		strncpy_s( bump, 256, srcchar + pos + 6, step - 6 );
		bump[step -6] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//　")の分
	else
		*stepnum = step;

	return 0;
}

int CMQOMaterial::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int flstart;
	int flend;
	int step = 0;

	while( (pos + step < srcleng) && 
	( (*(srcchar + pos + step) == ' ') || (*(srcchar + pos + step) == '\t') )
	//( *(srcchar + pos + step) ) 
	){
		step++;
	}

	flstart = pos+ step;

	while( (pos + step < srcleng) && 
		( isdigit( *(srcchar + pos + step) ) || (*(srcchar + pos + step) == '.') || (*(srcchar + pos + step) == '-') )
	){
		step++;
	}
	flend = pos + step;

	char tempchar[256];
	if( flend - flstart < 256 ){
		strncpy_s( tempchar, 256, srcchar + flstart, flend - flstart );
		tempchar[flend - flstart] = 0;
		*dstfloat = (float)atof( tempchar );
	}else{
		_ASSERT( 0 );
		*dstfloat = 0.0f;
	}

	*stepnum = step;

	return 0;

}

int CMQOMaterial::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int flstart;
	int flend;
	int step = 0;

	while( (pos + step < srcleng) && 
	( (*(srcchar + pos + step) == ' ') || (*(srcchar + pos + step) == '\t') )
	//( *(srcchar + pos + step) ) 
	){
		step++;
	}

	flstart = pos+ step;

	while( (pos + step < srcleng) && 
		( isdigit( *(srcchar + pos + step) ) || (*(srcchar + pos + step) == '-') )
	){
		step++;
	}
	flend = pos + step;

	char tempchar[256];
	if( flend - flstart < 256 ){
		strncpy_s( tempchar, 256, srcchar + flstart, flend - flstart );
		tempchar[flend - flstart] = 0;
		*dstint = atoi( tempchar );
	}else{
		_ASSERT( 0 );
		*dstint = 0;
	}

	*stepnum = step;

	return 0;
}



int CMQOMaterial::Dump()
{
	DbgOut( "MQOMaterial %d : name %s\n", materialno, name );
	DbgOut( "\tcol : r %f, g %f, b %f, a %f\n", col.r, col.g, col.b, col.a );
	DbgOut( "\tdif %f\n", dif );
	DbgOut( "\tamb %f\n", amb );
	DbgOut( "\temi %f\n", emi );
	DbgOut( "\tspc %f\n", spc );
	DbgOut( "\tpower %f\n", power );
	DbgOut( "\ttex %s\n", tex );
	DbgOut( "\talpha %s\n", alpha );
	DbgOut( "\tbump %s\n", bump );
	DbgOut( "\n" );
	return 0;
}

int CMQOMaterial::IsSame( CMQOMaterial* compmat, int compnameflag, int cmplevel )
{
	//compnameflag == 0 の時は、名前は比較しない。
	/***
	int materialno;
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
	***/

	int cmp;

	if( compnameflag ){
		cmp = strcmp( name, compmat->name );
		if( cmp )
			return 0;
	}
	
	if( shader != compmat->shader )
		return 0;

	if( (col.a != compmat->col.a) || (col.r != compmat->col.r) || (col.g != compmat->col.g) || (col.b != compmat->col.b) )
		return 0;

	if( dif != compmat->dif )
		return 0;

	if (amb != compmat->amb )
		return 0;

	if( emi != compmat->emi )
		return 0;

	if( spc != compmat->spc )
		return 0;

	if( power != compmat->power )
		return 0;

	if( *tex && *(compmat->tex) ){
		cmp = strcmp( tex, compmat->tex );
		if( cmp )
			return 0;
	}else{
		if( (*tex != 0) || (*(compmat->tex) != 0) )
			return 0;
	}

	if( *alpha && *(compmat->alpha) ){
		cmp = strcmp( alpha, compmat->alpha );
		if( cmp )
			return 0;
	}else{
		if( (*alpha != 0) || (*(compmat->alpha) != 0) )
			return 0;
	}

	if( *bump && *(compmat->bump) ){
		cmp = strcmp( bump, compmat->bump );
		if( cmp )
			return 0;
	}else{
		if( (*bump != 0) || (*(compmat->bump) != 0) )
			return 0;
	}

	if( cmplevel == 0 ){
		if( (dif4f.a != compmat->dif4f.a) || (dif4f.r != compmat->dif4f.r) || (dif4f.g != compmat->dif4f.g) || (dif4f.b != compmat->dif4f.b) ){
			return 0;
		}

		if( (amb3f.r != compmat->amb3f.r) || (amb3f.g != compmat->amb3f.g) || (amb3f.b != compmat->amb3f.b) ){
			return 0;
		}

		if( (emi3f.r != compmat->emi3f.r) || (emi3f.g != compmat->emi3f.g) || (emi3f.b != compmat->emi3f.b) ){
			return 0;
		}

		if( (spc3f.r != compmat->spc3f.r) || (spc3f.g != compmat->spc3f.g) || (spc3f.b != compmat->spc3f.b) ){
			return 0;
		}
	}


	return 1;
}

int CMQOMaterial::GetName( int srcmatno, char** nameptr )
{
	CMQOMaterial* curmat = this;
	CMQOMaterial* findmat = 0;

	while( curmat ){
		if( curmat->materialno == srcmatno ){
			findmat = curmat;
			break;
		}
		curmat = curmat->next;
	}

	if( findmat ){
		*nameptr = findmat->name;
	}else{
		*nameptr = 0;
	}

	return 0;
}

int CMQOMaterial::AddConvName( char** ppname )
{
	*ppname = 0;

	convnamenum++;

	ppconvname = (char**)realloc( ppconvname, sizeof( char* ) * convnamenum );
	if( !ppconvname ){
		DbgOut( "mqomaterial : AddConvName : ppconvname alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)strlen( name );

	char* newname;
	newname = (char*)malloc( sizeof( char ) * leng + 10 );
	if( !newname ){
		DbgOut( "mqomaterial : AddConvName : newname alloc error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	//sprintf( newname, "%s_%d", name, convnamenum );

	int addno;
	addno = convnamenum - 1;

	if( addno >= 1 ){
		if( addno >= 10 ){
			sprintf_s( newname, leng + 10, "%s%d", name, addno );
		}else{
			sprintf_s( newname, leng + 10, "%s0%d", name, addno );
		}

	}else{
		strcpy_s( newname, leng + 10, name );
	}


	*( ppconvname + convnamenum - 1 ) = newname;

	*ppname = newname;

	return 0;
}

int CMQOMaterial::SetCurTexname( int srcenabletex )
{

	//enabletextureが０の場合
	if( srcenabletex == 0 ){
		curtexname = NULL;
		return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	curtexname = tex;

	return 0;
}

int CMQOMaterial::SetExtTex( int srctexnum, char** srctexname )
{

	int texno;

	//以前のexttexを削除
	if( exttexname ){
		for( texno = 0; texno < exttexnum; texno++ ){
			char* delname = *( exttexname + texno );
			if( delname )
				free( delname );
		}
		free( exttexname );
	}
	exttexname = 0;
	exttexnum = 0;


	if( srctexnum > MAXEXTTEXNUM ){
		DbgOut( "mqomaterial : SetExtTex : texnum too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( srctexnum > 0 ){
		//exttexnameの作り直し。
		exttexname = (char**)malloc( sizeof( char* ) * srctexnum );
		if( !exttexname ){
			DbgOut( "mqomaterial : SetExtTex : exttexname alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		for( texno = 0; texno < srctexnum; texno++ ){
			char* curname;
			int curleng;
			curname = *( srctexname + texno );
			curleng = (int)strlen( curname );
			if( (curleng >= 256) || (curleng < 0) ){
				DbgOut( "mqomaterial : SetExtTex : texname too long error !!! %d, %s\n", texno, curname );
				_ASSERT( 0 );
				curleng = 255;//途中までしか保存しない。shandler::CreateTexture時にエラーになる。
				//return 1;
			}

			char* newname;
			newname = (char*)malloc( curleng + 1 );//!!!!!!
			if( !newname ){
				DbgOut( "mqomaterial : SetExtTex : newname alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			strncpy_s( newname, curleng + 1, curname, curleng );
			*( newname + curleng ) = 0;

			*( exttexname + texno ) = newname;

//DbgOut( "mqomaterial : SetExtTex : loop : texno %d, curname %s, curleng %d, exttexname %s\n",
//	texno, curname, curleng, *( exttexname + texno ) );

		}
	}

	exttexnum = srctexnum;

	return 0;
}

int CMQOMaterial::SetConvName( int srcconvnum, char** srcconvname )
{

	int texno;

	//以前のconvnameを削除
	if( ppconvname ){
		for( texno = 0; texno < convnamenum; texno++ ){
			char* delname = *( ppconvname + texno );
			if( delname )
				free( delname );
		}
		free( ppconvname );
	}
	ppconvname = 0;
	convnamenum = 0;


	if( srcconvnum > MAXEXTTEXNUM ){
		DbgOut( "mqomaterial : SetConvName : texnum too large error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( srcconvnum > 0 ){
		//ppconvnameの作り直し。
		ppconvname = (char**)malloc( sizeof( char* ) * srcconvnum );
		if( !ppconvname ){
			DbgOut( "mqomaterial : SetConvName : ppconvname alloc error !!!\n" );
			_ASSERT( 0 );
			return 1;
		}
		
		for( texno = 0; texno < srcconvnum; texno++ ){
			char* curname;
			int curleng;
			curname = *( srcconvname + texno );
			curleng = (int)strlen( curname );
			if( (curleng >= 256) || (curleng < 0) ){
				DbgOut( "mqomaterial : SetConvName : texname too long error !!! %d, %s\n", texno, curname );
				_ASSERT( 0 );
				curleng = 255;//途中までしか保存しない。shandler::CreateTexture時にエラーになる。
				//return 1;
			}

			char* newname;
			newname = (char*)malloc( curleng + 1 );//!!!!!!
			if( !newname ){
				DbgOut( "mqomaterial : SetConvName : newname alloc error !!!\n" );
				_ASSERT( 0 );
				return 1;
			}

			strncpy_s( newname, curleng + 1, curname, curleng );
			*( newname + curleng ) = 0;

			*( ppconvname + texno ) = newname;

//DbgOut( "mqomaterial : SetConvName : loop : texno %d, curname %s, curleng %d, ppconvname %s\n",
//	texno, curname, curleng, *( ppconvname + texno ) );

		}
	}

	convnamenum = srcconvnum;

	return 0;
}

int CMQOMaterial::SetLegacyMaterial( CTreeHandler2* lpth, CShdElem* selem )
{
	legacyflag = 1;

	CTreeElem2* te;
	te = ( *lpth )( selem->serialno );
	_ASSERT( te );

	char* nameptr = te->name;
	int nameleng;
	nameleng = (int)strlen( nameptr );
	int cpleng;
	cpleng = min( 245, nameleng );

	strncpy_s( name, 256, nameptr, cpleng );
	name[cpleng] = 0;

	char tempchar[256];
	sprintf_s( tempchar, 256, "_mat%d", materialno );
	strcat_s( name, 256, tempchar );

	if( selem->type == SHDINFSCOPE ){
		col.a = 0.5f;
		dif4f.a = 0.5f;
		orgalpha = 0.5f;
	}else{
		col.a = selem->alpha;
		dif4f.a = selem->alpha;
		orgalpha = selem->alpha;
	}

	if( selem->texname ){
		strcpy_s( tex, 256, selem->texname );
	}

	transparent = selem->transparent;
	texrule = selem->texrule;
	blendmode = selem->blendmode;

	uanime = selem->uanime;
	vanime = selem->vanime;

	exttexnum = selem->exttexnum;
	exttexmode = selem->exttexmode;
	exttexrep = selem->exttexrep;
	exttexstep = selem->exttexstep;
	exttexname = selem->exttexname;

	selem->exttexnum = 0;//!!!!!!!!!!!!!
	selem->exttexname = 0;//!!!!!!!!!!!!

	return 0;
}


int CMQOMaterial::GetColorrefDiffuse( COLORREF* dstdiffuse )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( dif4f.r * 255.0f );
	tempg = (unsigned char)( dif4f.g * 255.0f );
	tempb = (unsigned char)( dif4f.b * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstdiffuse = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefSpecular( COLORREF* dstspecular )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( spc3f.r * 255.0f );
	tempg = (unsigned char)( spc3f.g * 255.0f );
	tempb = (unsigned char)( spc3f.b * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstspecular = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefAmbient( COLORREF* dstambient )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( amb3f.r * 255.0f );
	tempg = (unsigned char)( amb3f.g * 255.0f );
	tempb = (unsigned char)( amb3f.b * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstambient = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefEmissive( COLORREF* dstemissive )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( emi3f.r * 255.0f );
	tempg = (unsigned char)( emi3f.g * 255.0f );
	tempb = (unsigned char)( emi3f.b * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstemissive = RGB( tempr, tempg, tempb );

	return 0;
}

