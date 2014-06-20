#include <stdafx.h> //É_É~Å[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <FBXBone.h>

#define	DBGH
#include <dbg.h>

#include <shdelem.h>

#include <crtdbg.h>


CFBXBone::CFBXBone()
{
	InitParams();
}

CFBXBone::~CFBXBone()
{
	DestroyObjs();
}

int CFBXBone::InitParams()
{
	m_boneinfcnt = 0;
	type = FB_NORMAL;
	selem = 0;
	skelnode = 0;
	bunkinum = 0;

	m_parent = 0;
	m_child = 0;
	m_brother = 0;

	return 0;
}

int CFBXBone::DestroyObjs()
{

	return 0;
}

int CFBXBone::AddChild( CFBXBone* childptr )
{

	childptr->m_parent = this;
	if( !m_child ){
		m_child = childptr;
	}else{
		CFBXBone* broptr = m_child;
		while( broptr->m_brother ){
			broptr = broptr->m_brother;
		}
		broptr->m_brother = childptr;
	}

	return  0;
}

