#ifndef FBXBONE0H
#define FBXBONE0H

#include <coef.h>

#include <D3DX9.h>
#include <crtdbg.h>
#include <fbxsdk.h>

class CShdElem;

class CFBXBone
{
public:
	CFBXBone();
	~CFBXBone();

	int AddChild( CFBXBone* childptr );

private:
	int InitParams();
	int DestroyObjs();

public:
	int type;
	CShdElem* selem;
	KFbxNode* skelnode;
	int bunkinum;

	CFBXBone* m_parent;
	CFBXBone* m_child;
	CFBXBone* m_brother;
	int m_boneinfcnt;
};


#endif