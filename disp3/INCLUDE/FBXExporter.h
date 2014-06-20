#ifndef FBXEXP_H
#define FBXEXP_H

#include <d3dx9.h>

#include <fbxsdk.h>
#include <stdio.h>

class CTreeHandler2;
class CShdHandler;
class CMotHandler;

int WriteFBXFile( LPDIRECT3DDEVICE9 pdev, CTreeHandler2* lpth, CShdHandler* lpsh, CMotHandler* lpmh, char* pfilename, float fbxmult, CTreeHandler2* lpdtirth, CShdHandler* lpdtirsh );


#endif

