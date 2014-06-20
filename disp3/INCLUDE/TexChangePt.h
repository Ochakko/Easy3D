#ifndef TEXCHANGEPTH
#define TEXCHANGEPTH



class CMQOMaterial;

class CTexChangePt
{
public:
	CTexChangePt();
	~CTexChangePt();

	int SetChangeName( char* srcname );

private:
	int InitParams();
	int DestroyObjs();

public:
	CMQOMaterial* m_mqomat;
	char* m_change;
};



#endif