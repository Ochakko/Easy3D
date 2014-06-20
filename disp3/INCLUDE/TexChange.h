#ifndef TEXCHANGEH
#define TEXCHANGEH



class CMQOMaterial;

class CTexChange
{
public:
	CTexChange();
	~CTexChange();

	int SetChangeName( char* srcname );

private:
	int InitParams();
	int DestroyObjs();

public:
	CMQOMaterial* m_mqomat;
	char m_change[256];
};



#endif