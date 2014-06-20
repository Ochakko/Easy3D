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
	CVec3f bonestart;		//�n�_
	CVec3f boneend;		//�I�_
	float boneleng;		//�{�[���̒���
	CVec3f bonevec;		//���K���{�[���x�N�g��
	int jointno;//�I�[�i�[�̃V���A��
	int parentjoint;//�I�[�i�[�̐e�̃V���A��

	int childno;// �I�I�I�q���̃V���A���@boneend�̎�����@2004/4/23 �ǉ�

	D3DXVECTOR3 bonecenter;

};
#endif