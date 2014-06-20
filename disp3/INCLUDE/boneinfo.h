#ifndef BONEINFOH
#define BONEINFOH

#ifndef SHDTREEVIEWH
#include <windows.h>
#endif

#include <D3DX9.h>

//#include "charpos.h"
#include <coef.h>
#include <basedat.h>

class CBoneInfo
{
public:
	CBoneInfo();
	CBoneInfo( CBoneInfo* srcbi );
	
	~CBoneInfo();

public:
	CVec3f bonestart;		//始点
	CVec3f boneend;		//終点
	float boneleng;		//ボーンの長さ
	CVec3f bonevec;		//正規化ボーンベクトル
	int jointno;//オーナーのシリアル
	int parentjoint;//オーナーの親のシリアル

	int childno;// ！！！子供のシリアル　boneendの持ち主　2004/4/23 追加

	D3DXVECTOR3 bonecenter;

};
#endif