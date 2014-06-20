#ifndef MORPHKEYH
#define MORPHKEYH


class CMorphKey
{
public:
	CMorphKey();
	~CMorphKey();

private:
	int InitParams();

public:
	int frameno;
	int target1;
	int target2;
	int master;
	float blend1;
	int interp;

};

#endif