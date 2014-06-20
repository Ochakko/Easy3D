#ifndef BONEPROPH
#define BONEPROPH


class CShdElem;
class CQuaternion;

#include <d3dx9.h>

int qToEuler( CShdElem* selem, CQuaternion* srcq, D3DXVECTOR3* Euler );
int qToEulerAxis( CQuaternion axisQ, CQuaternion* srcq, D3DXVECTOR3* Euler );

int qToEulerYXZ( CShdElem* selem, CQuaternion* srcq, D3DXVECTOR3* Euler );

int modifyEuler( D3DXVECTOR3* eulerA, D3DXVECTOR3* eulerB );// new, old


float vecDotVec( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2 );
float lengthVec( D3DXVECTOR3* vec );
float aCos( float dot );
int vec3RotateY( D3DXVECTOR3* dstvec, float deg, D3DXVECTOR3* srcvec );
int vec3RotateX( D3DXVECTOR3* dstvec, float deg, D3DXVECTOR3* srcvec );
int vec3RotateZ( D3DXVECTOR3* dstvec, float deg, D3DXVECTOR3* srcvec );

int GetRound( float srcval );

#endif
