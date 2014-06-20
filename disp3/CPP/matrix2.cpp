// !!!!!!! shade plugin sdk からmat4をもらいマチタ。 !!!!!!!
	//(CMatrixとは掛ける方向が逆、あとで変える)

#include <stdafx.h> //ダミー

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <matrix2.h>

CMatrix2::CMatrix2()
{
	//ZeroMemory( data[0], sizeof(float) * 16 );
	Identity();
}

CMatrix2::~CMatrix2()
{

}

CMatrix2::CMatrix2 (
	float a00, float a01, float a02, float a03,
	float a10, float a11, float a12, float a13,
	float a20, float a21, float a22, float a23,
	float a30, float a31, float a32, float a33
) {
	data[0][0] = a00;
	data[0][1] = a01;
	data[0][2] = a02;
	data[0][3] = a03;
	
	data[1][0] = a10;
	data[1][1] = a11;
	data[1][2] = a12;
	data[1][3] = a13;
	
	data[2][0] = a20;
	data[2][1] = a21;
	data[2][2] = a22;
	data[2][3] = a23;
	
	data[3][0] = a30;
	data[3][1] = a31;
	data[3][2] = a32;
	data[3][3] = a33;
}

void CMatrix2::Identity()
{
	ZeroMemory( data, sizeof(float) * 16 );
	for( int i = 0; i < 4; i++ ){
		for( int j = 0; j < 4; j++ ){
			if( i == j )
				(*this)[i][j] = 1.0f;
		}
	}
}

CMatrix2 CMatrix2::operator+ (const CMatrix2 &b) const {
	CMatrix2 c;
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
		c[i][j] = (*this)[i][j] + b[i][j];
	}

	return c;
}

CMatrix2 &CMatrix2::operator+= (const CMatrix2 &b) {
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
		(*this)[i][j] += b[i][j];
	}
	return *this;
}

CMatrix2 CMatrix2::operator- (const CMatrix2 &b) const {
	CMatrix2 c;
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
		c[i][j] = (*this)[i][j] - b[i][j];
	}

	return c;
}

CMatrix2 &CMatrix2::operator-= (const CMatrix2 &b) {
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
		(*this)[i][j] -= b[i][j];
	}
	return *this;
}

CMatrix2 CMatrix2::operator* (float b) const {
	CMatrix2 c;
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
		c[i][j] = (*this)[i][j] * b;
	}
	return c;
}

CMatrix2 &CMatrix2::operator*= (float b) {
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
		(*this)[i][j] *= b;
	}
	return *this;
}

CMatrix2 CMatrix2::operator* (const CMatrix2 &q) const {
	CMatrix2 t;
/***
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
		float r = 0.0;
		for (int k = 0; k < 4; k++) r += (*this)[i][k] * q[k][j];
		t[i][j] = r;
	}
***/

	t[0][0] = data[0][0] * q.data[0][0] + data[0][1] * q.data[1][0] + data[0][2] * q.data[2][0] + data[0][3] * q.data[3][0];
	t[0][1] = data[0][0] * q.data[0][1] + data[0][1] * q.data[1][1] + data[0][2] * q.data[2][1] + data[0][3] * q.data[3][1];
	t[0][2] = data[0][0] * q.data[0][2] + data[0][1] * q.data[1][2] + data[0][2] * q.data[2][2] + data[0][3] * q.data[3][2];
	t[0][3] = data[0][0] * q.data[0][3] + data[0][1] * q.data[1][3] + data[0][2] * q.data[2][3] + data[0][3] * q.data[3][3];

	t[1][0] = data[1][0] * q.data[0][0] + data[1][1] * q.data[1][0] + data[1][2] * q.data[2][0] + data[1][3] * q.data[3][0];
	t[1][1] = data[1][0] * q.data[0][1] + data[1][1] * q.data[1][1] + data[1][2] * q.data[2][1] + data[1][3] * q.data[3][1];
	t[1][2] = data[1][0] * q.data[0][2] + data[1][1] * q.data[1][2] + data[1][2] * q.data[2][2] + data[1][3] * q.data[3][2];
	t[1][3] = data[1][0] * q.data[0][3] + data[1][1] * q.data[1][3] + data[1][2] * q.data[2][3] + data[1][3] * q.data[3][3];

	t[2][0] = data[2][0] * q.data[0][0] + data[2][1] * q.data[1][0] + data[2][2] * q.data[2][0] + data[2][3] * q.data[3][0];
	t[2][1] = data[2][0] * q.data[0][1] + data[2][1] * q.data[1][1] + data[2][2] * q.data[2][1] + data[2][3] * q.data[3][1];
	t[2][2] = data[2][0] * q.data[0][2] + data[2][1] * q.data[1][2] + data[2][2] * q.data[2][2] + data[2][3] * q.data[3][2];
	t[2][3] = data[2][0] * q.data[0][3] + data[2][1] * q.data[1][3] + data[2][2] * q.data[2][3] + data[2][3] * q.data[3][3];

	t[3][0] = data[3][0] * q.data[0][0] + data[3][1] * q.data[1][0] + data[3][2] * q.data[2][0] + data[3][3] * q.data[3][0];
	t[3][1] = data[3][0] * q.data[0][1] + data[3][1] * q.data[1][1] + data[3][2] * q.data[2][1] + data[3][3] * q.data[3][1];
	t[3][2] = data[3][0] * q.data[0][2] + data[3][1] * q.data[1][2] + data[3][2] * q.data[2][2] + data[3][3] * q.data[3][2];
	t[3][3] = data[3][0] * q.data[0][3] + data[3][1] * q.data[1][3] + data[3][2] * q.data[2][3] + data[3][3] * q.data[3][3];
	
/***
	t[0][0] = (*this)[0][0] * q[0][0] + (*this)[0][1] * q[1][0] + (*this)[0][2] * q[2][0] + (*this)[0][3] * q[3][0];
	t[0][1] = (*this)[0][0] * q[0][1] + (*this)[0][1] * q[1][1] + (*this)[0][2] * q[2][1] + (*this)[0][3] * q[3][1];
	t[0][2] = (*this)[0][0] * q[0][2] + (*this)[0][1] * q[1][2] + (*this)[0][2] * q[2][2] + (*this)[0][3] * q[3][2];
	t[0][3] = (*this)[0][0] * q[0][3] + (*this)[0][1] * q[1][3] + (*this)[0][2] * q[2][3] + (*this)[0][3] * q[3][3];

	t[1][0] = (*this)[1][0] * q[0][0] + (*this)[1][1] * q[1][0] + (*this)[1][2] * q[2][0] + (*this)[1][3] * q[3][0];
	t[1][1] = (*this)[1][0] * q[0][1] + (*this)[1][1] * q[1][1] + (*this)[1][2] * q[2][1] + (*this)[1][3] * q[3][1];
	t[1][2] = (*this)[1][0] * q[0][2] + (*this)[1][1] * q[1][2] + (*this)[1][2] * q[2][2] + (*this)[1][3] * q[3][2];
	t[1][3] = (*this)[1][0] * q[0][3] + (*this)[1][1] * q[1][3] + (*this)[1][2] * q[2][3] + (*this)[1][3] * q[3][3];

	t[2][0] = (*this)[2][0] * q[0][0] + (*this)[2][1] * q[1][0] + (*this)[2][2] * q[2][0] + (*this)[2][3] * q[3][0];
	t[2][1] = (*this)[2][0] * q[0][1] + (*this)[2][1] * q[1][1] + (*this)[2][2] * q[2][1] + (*this)[2][3] * q[3][1];
	t[2][2] = (*this)[2][0] * q[0][2] + (*this)[2][1] * q[1][2] + (*this)[2][2] * q[2][2] + (*this)[2][3] * q[3][2];
	t[2][3] = (*this)[2][0] * q[0][3] + (*this)[2][1] * q[1][3] + (*this)[2][2] * q[2][3] + (*this)[2][3] * q[3][3];

	t[3][0] = (*this)[3][0] * q[0][0] + (*this)[3][1] * q[1][0] + (*this)[3][2] * q[2][0] + (*this)[3][3] * q[3][0];
	t[3][1] = (*this)[3][0] * q[0][1] + (*this)[3][1] * q[1][1] + (*this)[3][2] * q[2][1] + (*this)[3][3] * q[3][1];
	t[3][2] = (*this)[3][0] * q[0][2] + (*this)[3][1] * q[1][2] + (*this)[3][2] * q[2][2] + (*this)[3][3] * q[3][2];
	t[3][3] = (*this)[3][0] * q[0][3] + (*this)[3][1] * q[1][3] + (*this)[3][2] * q[2][3] + (*this)[3][3] * q[3][3];
***/
	return t;
}

CMatrix2 &CMatrix2::operator*= (const CMatrix2 &q) {
	*this = *this * q;
	return *this;
}

CMatrix2 &operator>> (const CMatrix2 &p, CMatrix2 &q) {
	q = p * q;
	return q;
}

CMatrix2 &operator<< (CMatrix2 &p, const CMatrix2 &q) {
	p = p * q;
	return p;
}

float det (const CMatrix2 &a, int i1, int i2, int i3, int j1, int j2, int j3) {
	return (a[i1][j1]*a[i2][j2]*a[i3][j3]
			+ a[i1][j2]*a[i2][j3]*a[i3][j1]
			+ a[i1][j3]*a[i2][j1]*a[i3][j2]
			- a[i1][j1]*a[i2][j3]*a[i3][j2]
			- a[i1][j2]*a[i2][j1]*a[i3][j3]
			- a[i1][j3]*a[i2][j2]*a[i3][j1]);
}



bool inv (const CMatrix2 &a, CMatrix2 &m) {

	float d = ::det(a,0,1,2,0,1,2);
	if (0.0 < d || d < -0.0) {
		d =  (float)(1.0 / d);
		m[0][0] =  det(a,1,2,3,1,2,3)*d;
		m[0][1] = -det(a,0,2,3,1,2,3)*d;
		m[0][2] =  det(a,0,1,3,1,2,3)*d;
		m[0][3] = -det(a,0,1,2,1,2,3)*d;
		m[1][0] = -det(a,1,2,3,0,2,3)*d;
		m[1][1] =  det(a,0,2,3,0,2,3)*d;
		m[1][2] = -det(a,0,1,3,0,2,3)*d;
		m[1][3] =  det(a,0,1,2,0,2,3)*d;
		m[2][0] =  det(a,1,2,3,0,1,3)*d;
		m[2][1] = -det(a,0,2,3,0,1,3)*d;
		m[2][2] =  det(a,0,1,3,0,1,3)*d;
		m[2][3] = -det(a,0,1,2,0,1,3)*d;
		m[3][0] = -det(a,1,2,3,0,1,2)*d;
		m[3][1] =  det(a,0,2,3,0,1,2)*d;
		m[3][2] = -det(a,0,1,3,0,1,2)*d;
		m[3][3] = 1.0;
		return true; }
	else {
		m = a;
		return false;
	}
}

CMatrix2 inv (const CMatrix2 &a) {
	CMatrix2 m;
	::inv(a, m);
	return m;
}

bool inv_w (const CMatrix2 &a, CMatrix2 &m) {

	float d = ::det(a,0,1,2,0,1,2);
	if (0.0 < d || d < -0.0) {
		//d =  (float)(1.0 / d);
		m[0][0] =  det(a,1,2,3,1,2,3);
		m[0][1] = -det(a,0,2,3,1,2,3);
		m[0][2] =  det(a,0,1,3,1,2,3);
		m[0][3] = -det(a,0,1,2,1,2,3);
		m[1][0] = -det(a,1,2,3,0,2,3);
		m[1][1] =  det(a,0,2,3,0,2,3);
		m[1][2] = -det(a,0,1,3,0,2,3);
		m[1][3] =  det(a,0,1,2,0,2,3);
		m[2][0] =  det(a,1,2,3,0,1,3);
		m[2][1] = -det(a,0,2,3,0,1,3);
		m[2][2] =  det(a,0,1,3,0,1,3);
		m[2][3] = -det(a,0,1,2,0,1,3);
		m[3][0] = -det(a,1,2,3,0,1,2);
		m[3][1] =  det(a,0,2,3,0,1,2);
		m[3][2] = -det(a,0,1,3,0,1,2);
		m[3][3] = d;
		return true; }
	else {
		m = a;
		return false;
	}
}


CMatrix2 inv_w (const CMatrix2 &a) {

	CMatrix2 m;
	::inv_w(a, m);
	return m;
}

int CMatrix2::SetMoveMat( VEC3F mvvec )
{
	Identity();
	data[3][0] = mvvec.x;
	data[3][1] = mvvec.y;
	data[3][2] = mvvec.z;
	return 0;
}

int CMatrix2::SetMoveMat( float srcx, float srcy, float srcz )
{
	Identity();
	data[3][0] = srcx;
	data[3][1] = srcy;
	data[3][2] = srcz;
	return 0;
}

int CMatrix2::SetScaleMat( float srcx, float srcy, float srcz )
{
	Identity();
	data[0][0] = srcx;
	data[1][1] = srcy;
	data[2][2] = srcz;
	return 0;
}