#ifndef ALPCHANGEH
#define ALPCHANGEH

class CMQOMaterial;

class CAlpChange
{
public:
	CAlpChange();
	~CAlpChange();

	int SetAlpChange( float srcchange );

private:
	int InitParams();
	int DestroyObjs();

public:
	CMQOMaterial* mqomat;
	float change;
};


#endif