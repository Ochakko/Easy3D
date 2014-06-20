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
	int bonematno; //bone matrix��serialno
	int parmatno;  //�ejoint��matrix��serialno
	float bonerate; //bone matrix �̉e���x

	int childno;
	int calcmode;

//////// for miko
	int mikoapchildnum;
	int* mikoapchildseri;
	CBoneInfo* nearestbi;
	CShdElem* nearestbipar;
	
};

#endif