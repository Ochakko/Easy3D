#ifndef BONEELEMH
#define BONEELEMH

class CShdHandler;
class CBoneInfo;
class CShdElem;

class CBoneElem
{
public :
	CBoneElem();
	~CBoneElem();

	int ClearMikoParams();
	int AddMikoApChild( int srcchildseri );
	int SetNearestBI( CShdHandler* srclpsh, D3DXVECTOR3 srcv );

private:
	int Replace2FloatBone( CShdHandler* srclpsh );

public:
	int bonematno; //bone matrixのserialno
	int parmatno;  //親jointのmatrixのserialno
	float bonerate; //bone matrix の影響度

	int childno;
	int calcmode;

//////// for miko
	int mikoapchildnum;
	int* mikoapchildseri;
	CBoneInfo* nearestbi;
	CShdElem* nearestbipar;
	
};

#endif