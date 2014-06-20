
// !!!!!!! shade plugin sdk からmat4をもらいマチタ。 !!!!!!!
	//(CMatrixとは掛ける方向が逆、あとで変える)

#ifndef MATRIX2H
#define MATRIX2H

#include <assert.h>
#include <memory.h>
//#include <charpos.h>
#include <coef.h>

class CMatrix2
{
public:
	float data[4][4];

	float *operator[] (int i) { assert(0<=i&&i<4); return (float *)(data[i]); };
	const float *operator[] (int i) const { assert(0<=i&&i<4); return (const float *)(data[i]); };
	CMatrix2 ();
	CMatrix2 (	float a00, float a01, float a02, float a03,
			float a10, float a11, float a12, float a13,
			float a20, float a21, float a22, float a23,
			float a30, float a31, float a32, float a33
	);
	//CMatrix2 (const vec4 &a, const vec4 &b, const vec4 &c, const vec4 &d);
	//CMatrix2 (const CMatrix2 &t) { ::memcpy(this, &t, sizeof(CMatrix2)); }
	~CMatrix2();

	void Identity();

	CMatrix2 &operator= (const CMatrix2 &t) { ::memcpy(this, &t, sizeof(CMatrix2)); return *this; }
	bool operator== (const CMatrix2 &t) const {
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
			if ((*this)[i][j] != t[i][j]) return false; }
		return true;
	}
	bool operator!= (const CMatrix2 &t) const { return !(*this == t); }

	CMatrix2 operator* (float f) const;
	CMatrix2 &operator*= (float f);
	CMatrix2 operator+ (const CMatrix2 &m) const;
	CMatrix2 &operator+= (const CMatrix2 &m);
	CMatrix2 operator- (const CMatrix2 &m) const;
	CMatrix2 &operator-= (const CMatrix2 &m);
	CMatrix2 operator* (const CMatrix2 &m) const;
	CMatrix2 &operator*= (const CMatrix2 &m);
	
	int SetMoveMat( VEC3F mvvec );
	int SetMoveMat( float srcx, float srcy, float srcz );
	int SetRotMat( CMatrix2& axis, float rotrad );
	int SetScaleMat( float srcx, float srcy, float srcz );

};



#endif